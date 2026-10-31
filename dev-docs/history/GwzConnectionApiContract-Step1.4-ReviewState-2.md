# Step 1.4 capacity per binding — STATE-AXIS RE-VERDICT (remediation round 1)

**Review object:** gwz-transport `git diff b0e5ad5c..a22d43cd` and gwz-core `git diff a5ce2941..0c288b7b`, in lane `/Volumes/projects/limbo/gwz-dev-p1-capacity`.
**Baseline:** first-round tuple gwz-transport `b0e5ad5c`, gwz-core `a5ce2941`.
**Revised tuple:** gwz-transport `a22d43cd24830db71eeb7b0fa44f72b5bf70462c`, gwz-core `0c288b7b9a0cd5977c5288d363198fde47e9d087`. I checked it at the start and the end; it did not move. Both worktrees were clean.
**Date:** 2026-10-30
**Axis:** State.

**Verdict: NO-GO.** One new P2, from the lane owner's ruling on P2-2 consequence 2. P2-1, P3-1, P3-2 and P3-3 are closed. P2-2 consequence 1 is closed; consequence 2 is closed only at the authority, and that closure creates the new P2.

I pre-commit to GO on a revision that resolves P2-3 as specified, provided no new P0 to P2 appears.

## Prior-finding closure table

"Verified" means I re-ran or re-traced my original counterexample against the corrected tree.

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P2-1 (shared `Owner.session`, so a smaller-limit request starves) | Owner keyed per request on SSH and HTTPS. SSH: `request_owner_session(worker session, request)`. HTTPS: `Input::owner()` in `prepare.rs`, `serve.rs` and `throttle.rs`. | **Pool level.** The new row `operations_that_share_a_session_share_one_count…` reproduces my original probe (shared session: large holds 10, small gets 0; reverse order: small 4, large 28). With a session each: 10 and 4. **SSH.** `overlapping_requests_on_one_worker_are_each_held_to_their_own_limit` passes: [`large`(32)×10, `small`(4)×8] gives opening 10 then 14, so the 4-request is granted exactly 4; reverse gives 4 then 36, so the 32-request gets 32; a lone request at 32 gives 32. **HTTPS.** `overlapping_https_requests_on_one_session_are_each_held_to_their_own_limit` gives (10,4), (4,32) and (32,0) through the production `prepare` path. **Trace.** `admission.rs:343` passes `Some(key.0)`, the request id, so `runner.rs:206` keys the owner when both request and limits are present. The `Owner.operation` that `HttpsPool::request` reads for limits is the same name, because `Input::owner()` keeps `operation` as `Owner.operation`. | **Closed.** Side effect noted in the changed-range analysis. |
| P2-2 consequence 1 (the ratchet outlives its request) | `live: request → (total, per_host)` in the authority. Ceilings are the max over live requests, and the base applies only when none is live. `end_operation` is called from `Session::cancel`. | `a_limit_admitted_by_a_request_that_has_ended_no_longer_raises_the_ceilings` and `live_requests_bound_the_ceilings…` pass. `a_request_s_limits_bound_the_shared_authority_only_while_it_is_live` runs end to end through `TransportRuntime::request`: large 50, then small 4 gives 50; large ends gives 4; small ends gives the base. | **Closed.** |
| P2-2 consequence 2 (a lone request at 4 keeps its cross-scheme bound) | Same mechanism. This is the lane owner's ruling, which I judge for safety and completeness only. | `a_lone_request_below_the_base_bounds_one_host_across_both_schemes` passes: SSH plus HTTPS, 4 reservations fit and the fifth waits on either scheme. `https_only_overlapping…` now asserts that a lone request at 1 per host allows one reservation. | **Closed at the authority. Incomplete as a state machine: see P2-3.** |
| P3-1 (misfit bound smaller than the idle population) | `misfit_bound` is `max(pool per_user_host, binding per_user_host) + 1`. | The new row `p1_4_rem_lost_leases_across_many_idle_entries…` is my probe's shape: 8 idle, 6 lost, binding at 1. It passes, and the open succeeds. The old row now expects 257. The loop stays bounded: each lost lease consumes or discards one entry. | **Closed.** |
| P3-2 (side effects before registration) | Register first, record limits only if the request is still unsealed. | `a_duplicate_request_id_admission_fails_and_leaves_the_first_request_untouched` passes. Traced: `register` does the check and the insert under one lock, so a concurrent duplicate loses atomically before it touches limits or the authority. The sealed-between-register-and-record case is skipped under the same state lock that `cancel` uses, so a request that has already been cancelled never leaves a record. | **Closed.** The implementer's note is accurate: the concurrent race is closed by construction and the row covers only the sequential case. |
| P3-3 (dead `ActiveOperation`) | Deleted, with the `classify_tests` row. | `grep` finds no reference in gwz-core, gwz-transport, gwz-py or gwz-cli. | **Closed.** |

## Changed-range analysis

**gwz-transport (`b0e5ad5c..a22d43cd`, 6 files, +82 −12)**
- `instance.rs`: the P3-1 bound, derived from the pool ceiling and the binding limit. No new state.
- `pool/mod.rs`: the `ActiveOperation` variant is deleted.
- Tests and fakes: the `start_lost_n` fake, the new capacity row, the 257 expectation and the pool row for the shared-session meaning. All pass.
- Run: `pool_owner_limits`, `pool`, `pool_limit`, `pool_settle` and `pool_tag` pass, and `-p gwz-connection` passes 88/88.

**gwz-core (`a5ce2941..0c288b7b`, 19 files, +508 −108)**
- `shared_reservation.rs`: the `live` map, `admit_operation(request, …)` and `end_operation`.
  - Admission wakes waiters when the ceilings rise.
  - Removal does not wake waiters.
  - I could not construct a waiter that needs that wake. Waiters belong to requests that are still live; once the last live request ends, only that request's own abandoned connects remain.
- `request_owner_session`: owner key `base/request`, with a digest fallback past 128 bytes.
  - `DefaultHasher::new()` is deterministic within the process, which is all the key needs.
  - A collision only makes two requests share a count, which is fail-closed.
  - The digest is session-qualified, so cross-client collisions would need a 64-bit collision on the pair.
- SSH `OpenRequest.request` and `runner.rs`: the owner is keyed only when both the request and the limits are present. An open with no request keeps the worker owner.
- HTTPS `Input::owner()` and `recheckout(owner, …)`: all three checkout sites now use the keyed owner, so a request's checkouts still share one limit.
- `capacity.rs`: register first, then record, with a sealed check under the state lock.
- `requests.rs`: `authority.end_operation(request)` inside `Session::cancel`, under the same lock that seals.
  - Normal end goes `finish`, then drop of the `ClientRequest`, then `cancel`.
  - A request cannot be re-admitted afterwards, because the `used` set refuses a reused ID.
- Run, all with the candidate cfg:
  - `shared_reservation` 16/16.
  - `transport_host::` 442 passed, 0 failed, 3 ignored.
  - `ssh_worker` 7/7.
  - The new SSH, HTTPS and transport-host rows pass.
  - The reuse rows `p1_6_*` pass (2 pass, 1 ignored with its existing OPEN note).

**Side effect to record, not a finding.** `serves_binding` (`https_pool.rs:355`) reads `request.owner.session` as the binding for last-lessee tracking.
- That is now per request, not per mux session, for HTTPS.
- It only matters for `HttpsScoped` connections.
- It is tighter, which is fail-closed.
- It matches design §2, where the binding is the operation.
- It costs nothing in practice, because scoped connections are retired with their credential (`credentials.rs:13`, `native.rs:408`).

## Findings

### [P2-3] NEW (architectural): the authority ceiling can sit below reservations already held, and nothing reclaims them

This is a new architectural root cause. It comes from the ruling "ceilings are the live requests' max; the base applies only when none is live". The ruling is stricter than contract §8 and could not reasonably have been foreseen from my round-1 remedy text. Together with the absence of reclamation, it creates a new stuck state.

**Location.**
- `gwz-core/src/git/endpoint/shared_reservation.rs`: `State::ceilings` and `take`.
- Reservations live as long as their resource, idle included (`Slot::Started` holds the reservation until disposal; row `disposal_releases_the_shared_slot…`; contract §8: "opening, idle, leased and closing … retain the shared reservation until actual disposal").
- The pool's eviction is judged only against the pool's own ceilings (`allocation.rs`, `ceilings_for`), and the cross-scheme "oldest idle retires" path is not built (evidence README limitations; design §5, T4 and C11).

**Invariant.** A request whose limits are within its own owner count must make progress, or fail only by its own allocation deadline for a reason that removing its own work would cure. Fail-closed means progress may be lost; here it is lost for longer than the request's whole allocation window.

**Mechanism.**
- The legacy `install_capacity` retired surplus idle entries before it installed lower ceilings. The remediation removed that.
- The authority ceiling for a live request at 4 is now 4 per host.
- Reservations held by idle connections of earlier requests still count.

**Interleaving.**
1. Request R1, default 32 per host, fetches many members from host H and leaves, say, 20 idle connections. Their reservations stay held; `hosts[H] = 20`.
2. Within the idle timeout (60 s default), request R2 on the same runtime arrives with `--max-per-host 4` (gwz-py `max_connections_per_host=4`, a long-lived runtime).
3. The authority ceiling is now `(…, 4)`.
4. R2 needs a connection that no idle entry can serve: the other scheme to the same host, a different user, port or identity, or a replacement for a connection it discarded.
5. The pool sees its host total (20) below its ceiling (`max(32, 4) = 32`) and R2's owner below 4, so it opens an entry and does not evict.
6. `reserve_or_wait` finds `hosts[H] = 20 ≥ 4` and defers the connect.
7. Nothing disposes the idle entries until their 60 s idle expiry, which is longer than the 30 s allocation default. R2 fails with an allocation timeout.

**Confidence.**
- The arithmetic and the reservation retention are certain from code and existing rows. `a_lone_request_below_the_base…` already shows a held count at the ceiling deferring a fifth connect, and held idle and held leased reservations are indistinguishable to `take`.
- I did not run an end-to-end gwz-core interleaving. The same-key case (idle SSH reused by R2) works, because leasing an idle entry needs no new reservation. The failure is confined to the cases in step 4.

**Impact.**
- A hard allocation failure for an operation in a long-lived runtime, in a state the legacy path did not have. Legacy retired the surplus at admission, and the reviewed first-round code never lowered the ceiling.
- Bounded by the idle timeout, not permanent.

**Remedy (any one of these).**
1. **Retire on admission.** When admission or a request end lowers the ceiling below `hosts` or `total`, discard the pools' idle entries beyond the ceiling and let disposal free the reservations. `PoolControl::discard_idle(site)` exists. Wait for disposal inside the request's allocation window, not at admission.
2. **Reclaim on deferral.** When `reserve_or_wait` defers, ask the pools for the oldest idle entry of the constrained host or total (design §5; T4 and C11), and re-select if it is raced to leased.
3. **Drop the cross-scheme part of consequence 2** for idle-held reservations, by not counting idle reservations against a live request's ceiling. That changes what "as `install_capacity` gave" means and needs the lane owner's sign-off.

**Closure test.**
- A gwz-core row: R1 leaves N ≥ 4 idle reservations to H; R2 admitted at 4 needs a connection on the other scheme (or a replacement); assert it is served within the allocation deadline, with the idle entries retired and the reservations released.
- A row that the same sequence with R2 at the default still works.
- Keep `a_lone_request_below_the_base…` green.

## Invariant analysis (changed range)

- **Per-owner counting** is unchanged in the machine. The keying change only moves requests into separate owners, so one request's workers still share one limit, which is the legacy lone-operation meaning.
- **Authority liveness set.** `admit` and `end` are keyed by the request id, which is unique per session. Admission is guarded by registration and by the sealed check, both under the state lock. Removal is under the same lock as the seal. The only paths that leave a live record are a leaked `ClientRequest` or session close, and the authority dies with the session. Fail-direction for an over-long live record is conservative-high.
- **Wakeups.** Admission raises the ceiling and wakes waiters; the row `admitting_a_larger_limit_wakes…` covers it. Removal lowers the ceiling or restores the base. I found no waiter that needs a wake there.
- **Lone request at the base.** With a limit of 32 and a base of 32, behaviour is as before. With a limit above the base, the lone request gets its own limit and nothing else changes.
- **Overlap.** The ceiling is the larger live limit; each request is held by its own owner. The observable behaviour is the same as the first-round "largest admitted", but scoped to live requests.
- **Everything else I attacked in round 1** (derived counts, owner lifecycle, wakeups, fairness, the removed gates, `TransportCapacityConflict` readers) is unchanged in this range and was not re-opened.

## Risks and next action

**Next action.** Fix P2-3 as specified and re-review only that finding with its closure rows.

**Residual risks.**
- The latent factory-authority mismatch (`transport_host/mod.rs:351`, a base of 32 per host and no `admit_operation`) is still recorded for phase 3, as the remediation plan states. It is not part of this verdict.
- If P2-3 is fixed with remedy 2, the reclamation in `reserve_or_wait` must re-select when the chosen idle entry has been raced to leased (design §5), or it will recreate a different stuck state.
- The Windows receipt is outside this review.
