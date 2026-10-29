# Step 1.4 capacity per binding — STATE-AXIS RE-VERDICT 3 (remediation round 2, P2-3 only)

**Review object:** gwz-core `git diff 0c288b7b..408a8b24` (4 files, +444 −11); gwz-transport unchanged.
**Tuple:** gwz-transport `a22d43cd24830db71eeb7b0fa44f72b5bf70462c`, gwz-core `408a8b2449699640835ca613eb6de47fbcd7fe81`. I checked it at the start and the end; it did not move. Both worktrees were clean.
**Date:** 2026-10-30
**Axis:** State.

**Verdict: GO.** P2-3 is closed. I found no new P0, P1 or P2 and no new architectural root cause. Three P3 notes follow; none blocks.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P2-3 (authority ceiling below idle-held reservations; a deferred connect waits out the 60 s idle timeout, longer than the 30 s allocation window) | Retire on lowering. `admit_operation` and `end_operation` call `retire_surplus` when the effective ceilings fall below what a host or the total holds. The surplus idle connections are discarded through the pools' `PoolControl::discard_idle`, and their disposal releases the reservations and wakes the deferred connect. | **Original interleaving, re-traced and re-run.** R1 at the default leaves 6 idle SSH connections to host H, and R2 is admitted at 4. `a_request_below_the_base_retires_the_idle_connections_a_replacement_would_wait_behind` (SSH pool and HTTPS pool on one authority) shows R2's HTTPS checkout served, the SSH idle count 0, and the authority's count for H at `(1,1)`, just R2's lease. The implementer's red log has that row failing against the inert `retire_surplus`, and the green run passes it. **Variants.** The default-limit variant retires nothing and leaves 6 idle plus R2's lease (`a_request_at_the_default_retires_nothing`). A request at 64 ending with 60 idle held falls to the base: the held count is at most 32 and the next request is served (`…at_64_ending…`). A lowering never touches leased entries (40 leased stay, the 20 idle go, and the leased resources are still present). The total ceiling falling retires idle connections of the busiest hosts. `the_session_registers_each_of_its_pools_with_the_authority` shows the production session registers each pool. **Production path.** `ReservedResource` is the only production reserver, and it now uses `reserve_site_or_wait`, so every production reservation is tracked by site. | **Closed.** |

**Earlier closures not reopened.** I re-ran the families that touch this range, and none regressed:
- `shared_reservation` and its `retire` rows: 21/21.
- `transport_host::`: 443 passed, 0 failed, 3 ignored.
- `git::endpoint::ssh_worker`: 7/7.
- `https_worker::budget_tests`: 9/9, including the P2-1 HTTPS row.
- `https_worker::reuse_tests`: 2 passed, 1 ignored with its existing OPEN note.

The P2-1, P2-2, P3-1, P3-2 and P3-3 mechanisms live in files this diff does not change.

## Changed-range analysis

**`shared_reservation.rs` (+55 −11).**
- The `State` gains `sites` (reservation counts by scheme, host and port) and `pools` (registered `PoolControl`s). `Reservation` gains its `site`.
- `take` records the site under the state lock and `Drop` removes it under the same lock, so the site counts always equal the reservations.
- `admit_operation` wakes waiters when the ceilings rise (unchanged) and then calls `retire_surplus`.
- `end_operation` calls `retire_surplus` only if the request was live.

**`shared_reservation/retire.rs` (new).**
- `retire_surplus` snapshots under the authority lock and releases it before it touches any pool.
- It computes, per host, `held − per-host ceiling` and, overall, `total − total ceiling`.
- It discards idle connections site by site, largest first. The count each discard returns stops the walk.
- The first pass handles the per-host surplus and the second the total surplus.

**`session.rs`.** Registers `engine.pool().control()` and `endpoint.pool().control()` with the authority, right after each endpoint is built.

### Attacks on the retirement

**Lock ordering and re-entrancy: no deadlock found.**
- The edges are session state → authority, session state → pool lock, and pool lock → authority (a host step that polls a resource takes the authority lock). The snapshot drops the authority lock before any pool call, so there is no authority → pool edge and therefore no cycle.
- `discard_idle` only moves Idle entries to Closing inside the pool machine. The machine holds no physical resource, so no `Reservation` drops inside the call and there is no re-entry into the authority.
- The reservations drop later, when the host disposes the closed resource. That path takes only the authority lock and wakes outside it, as before.
- `retire_surplus` runs under the session state lock, called from `admit_client_request` and `Session::cancel`. Those callers already took pool locks under that lock (`engine.cancel_request`). `discard_idle` wakes pool waiters and the driver the same way; those wakers are tokio or thread-unpark wakers that take no session lock.
- Governor interplay: the governor takes its own lock and then the pool lock (`sync`, then `discard_idle`). The retire path takes only the pool lock. A `Closing` observation of an idle connection is the same event as idle expiry or eviction, so the limit machine is not given a new signal.

**Race with a lease of the same idle entry: none.**
- `discard_idle` closes entries that are Idle at that moment, under the pool lock. A concurrent lease of the same entry either wins first, in which case the entry is Leased and untouched, or loses, in which case the entry is Closing and the lease takes another entry or opens a new one.
- The authority snapshot can be stale (a site emptied, or a larger request admitted in between). The outcomes are a no-op or an unnecessary discard, never a wrong discard of a leased or opening entry. That direction is fail-closed.

**Wakeups after release.** The disposal drops the `Reservation`, which wakes every registered waiter, so the deferred `ReservedResource` is re-polled. `discard_idle` also changes the pool revision, so the pool's driver and waiters are woken and the Closing entries are processed. Row (a) shows the deferred connect served after the retirement.

**Registration lifetime of pools versus the authority.**
- The authority holds `PoolControl` clones. A control is not a pool owner, so it does not keep the pool alive or block shutdown.
- `Shared` holds only wakers and the machine, not the authority, so the authority is not kept alive by the controls.
- A control left after a pool stops is harmless: `discard_idle` on a stopped machine has nothing idle to close.
- If the second endpoint's construction fails after the first was registered, the session creation fails and the authority is dropped with its controls.
- The connection host's own authority (`transport_host/mod.rs:351`) registers no pool and `retire_surplus` returns at once on an empty `pools`. That authority also has no admit call, which is the latent phase-3 item already recorded.

**Anything closed earlier that this reopens: no.**
- The ceilings and the live set are unchanged by this diff.
- Retirement acts only when the ceilings fall. When overlapping requests are live the ceiling is their max, so the retired idle connections are surplus to every live request's limit.
- The session state lock window grows by a bounded pool call.

## Findings

### [P3-A] The retirement is site-granular, so a lowering can discard idle connections the new request could have reused

**Location.** `retire.rs`, `retire_sites` (`PoolControl::discard_idle` takes a whole site).

**Evidence.** In row (a), 6 idle SSH connections are held under a ceiling of 4, and all 6 go, not 2. The implementer discloses this. The old `install_capacity` kept idle connections up to the new capacity (`kept_total < capacity.total …` in the removed code), and the `retire.rs` header comment's "retired every connection of both pools" is not what the removed code did.

**Impact.** A reconnect (handshake, auth, reuse proof) where up to 4 idle connections could have been reused. Efficiency, bounded, not correctness; no stuck state.

**Suggested improvement.** A count-limited pool discard, oldest first, in a later step.

### [P3-B] Retirement is edge-triggered at admission and request end

**Evidence.** Reservations held by connections that become idle after the trigger are not retired, so they would wait out the idle timeout. I could not construct this from the production paths:
- A normal request end completes all its exchanges before the request is dropped and cancelled, so the leases are already idle when the trigger runs (rows (c) and (d)).
- A cancelled request discards its in-flight leases.

**Confidence.** Reasoned from code, not run.

**Impact.** At most a bounded residual wait in an unusual interleaving.

**Suggested improvement.** Re-run the check when a release leaves a host over its ceiling. Not required.

### [P3-C] Disposals still in flight count as held

**Evidence.** The computation counts a Closing connection until its disposal finishes, so a second lowering before then can discard slightly more idle than the excess. The implementer discloses it.

**Impact.** A small efficiency loss; fail-closed.

## Invariant analysis

- **Owner and ceiling counting:** unchanged in this range.
- **Reservation and site accounting:** consistent. Both are updated under the authority lock at take and drop, so `sites` equals the reservations with a site.
- **Fail-closed direction:** every defect noted above loses reuse, never invents progress or exceeds a limit.
- **Reservations held by Opening, Leased and Closing entries** are never retired by this mechanism. Opening and Leased entries are not Idle, so `discard_idle` skips them.
- **Cap classification.** There is no new root cause. All three P3 notes are efficiency or edge residuals of the chosen remedy, inside the existing authority and pool design. The architecture the lane owner's ruling needed (retire on lowering) is now present, and nothing new is architectural.

## Risks and next action

**Next action.** None required for 1.4 State. The patch can proceed through the remaining gates (the Linux matrix and the Windows receipt are outside this review).

**Residual risks.**
- The three P3 notes above, for the lane owner's findings document.
- The latent connection-host factory authority (base of 32 per host, no `admit_operation`, no registered pools) is still a phase-3 item, as recorded in round 1.
