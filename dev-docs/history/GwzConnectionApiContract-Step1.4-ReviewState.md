# Step 1.4 capacity per binding — STATE-AXIS REVIEW

**Review object:** gwz-transport `git diff 398046991830..b0e5ad5c` and gwz-core `git diff 6d7c8372..a5ce2941`, in lane `/Volumes/projects/limbo/gwz-dev-p1-capacity`.
**Baseline:** gwz-transport `398046991830af465f24557d4396ebff26bcfb3a`; gwz-core `6d7c837280f0885a41ca21d6aa1cf37dab1033c9`.
**Date:** 2026-10-29
**Axis:** State (durable-state semantics and adversity). Single review; no second reviewer this round.

**Verdict: NO-GO.** Two P2 findings, three P3 findings, no P0 or P1.

I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified below, provided no new P0 to P2 appears. The P3 findings need not block.

## 0. Evidence base

**Tuple.** I checked it at start and at end. It did not move.
- gwz-transport HEAD was `b0e5ad5c4153023aee834fa2763c09c0b948f4ef`.
- gwz-core HEAD was `a5ce29418039213a7278c01a01ac1c79b44a66c8`.
- Both worktrees were clean before and after the review.
- I wrote nothing in the lane. My probes ran in a scratch copy of gwz-transport at `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/probe`. They used the review target dir `/Volumes/projects/limbo/build-targets/p1-capacity-review`.

**Documents read.**
- The brief and its scope amendment.
- Contract §8.
- Reuse design §5, §14 (T1, T2, C5, C11, T5) and §15 item 7.
- The 1.4 evidence README, including its Limitations section.

**Code read.**
- The full gwz-transport diff.
- The gwz-core diff, in these files:
  - `shared_reservation.rs`
  - `session/capacity.rs`, `session.rs` and `session/requests.rs`
  - `placement_endpoint*`
  - `https_endpoint*`
  - `https_pool.rs` and `https_worker.rs`
  - `ssh_worker/*`
  - `setup_retry/limit/governor.rs`
- The surrounding pool code in `allocation.rs`, `lifecycle.rs` and `limit.rs`.

**Runs (all green).**

| Run | Result |
|---|---|
| gwz-transport `--features unstable-sequenced --test pool_owner_limits` | 9/9 |
| gwz-transport `--test pool --test pool_limit --test pool_settle --test pool_tag` | 17, 13, 10 and 2 passed |
| `-p gwz-connection capacity` | 3/3 |
| gwz-core `--lib` with `RUSTFLAGS='--cfg gwz_transport_candidate'`, `shared_reservation` | 11/11 |
| `transport_host::tests` | 9/9 |
| `an_open_carries_its_operations_limits…` | 1/1 |
| `https_only_overlapping…` | 1/1 |
| `an_operations_limits_hold_its_https_checkouts…` | 1/1 |
| `live_request_between_leases…` | 1/1 |
| `https_only_reservation_is_held…` | 1/1 |

Without the candidate cfg, gwz-core's endpoint and transport-host modules are compiled out and the filters match 0 tests. This is not a defect.

**Probes (scratch copy only).**
- `probe_state.rs` ran two pool-machine cases under one shared `Owner.session`.
- `probe_tests.rs` ran a gwz-connection case with lost idle leases.

## Problem, requirement provenance, and scope

**Requested outcome.** Plan step 1.4 and the brief ask for four things:
- Limits travel with the binding.
- Differing limits no longer produce `TransportCapacityConflict`.
- The host keeps physical ceilings.
- Red row: two bindings with different `--max-per-host` overlap, "each held to its own limit".

**Observed problem.** Capacity was installed runtime-wide. A differing request was refused while any registration was live. A failed resize closed the runtime.

**Scope trace.** The scope amendment authorizes the pool changes T1 and T2, C5 and C11/T5 "only as needed", and the README paragraph. I judge the changes inside that amendment as follows.
- The pool changes are authorized.
- The README paragraph is authorized.
- The gwz-core removals are required by "no install" (design §5). The removed items are `install_capacity`, `CapacityMutation`, both gates, `set_request_capacity`, `Authority::install_capacity` and the `ErrorCode` variant.
- The per-request `limits` maps and the `Request.limits` plumbing are necessary.
- The gwz-connection `limits()`, the binding field and the 256 instance ceilings follow contract §8 and design §5.

**Extending the max rule to `per_user_host`.** It is beyond T2's text, which names `total`, `per_host` and `max_requests`. I judge it necessary, authorized and safe.
- Necessary: legacy `install_capacity` set `config.per_user_host` to the operation's value. Without the extension, `--max-per-host 64` would be clamped to the pool's 32 per user and host, which is a clamp regression.
- Authorized: design §5 derives `per_user_host` and `per_host` from the same `--max-per-host`, and the amendment says no clamp.
- Safe: the owner's own `per_user_host` still bounds that owner, and the pool-wide check uses the same request-local max as `per_host`.

**TransportCapacityConflict deletion.** It is safe.
- `ErrorCode` serialises by variant name through serde `snake_case`. It has no `repr` or numeric use.
- The generated protocol enum has no such variant, because the old mapping went to `IoError`.
- The lane's gwz-py, gwz-cli, `protocol/` schemas and gwz-core source have no reader other than the deleted producer and tests. The remaining hits are comments, dev-docs and evidence snapshots.
- The `convert.rs` mapping went with it.

**Deferred, not reported as findings.**
- The production consumer of gwz-connection's binding limits (phase 3).
- C5 beyond what the step needed. Finding P2-1 argues the shared-owner part was needed.
- Cross-scheme idle eviction.

**Out-of-scope observation (follow-up proposal, not a finding).** In `src/transport_host/mod.rs:351`, `connection_factory` builds its own `Authority::new(local.pool.total, local.pool.per_host)` with a per-host value of 32. Nothing calls `admit_operation` on it. When phase 3 wires `gwz-connection`, a binding above 32 per host will pass the instance pool (256 ceilings) and be held at 32 by reservation deferral. That would recreate the clamp for bindings above 32.
- Confirmed: the code state. Hypothesis: the phase-3 impact.
- Practical impact: none until the consumer lands. It does not block 1.4.
- Suggested improvement: make the factory authority's ceilings the instance's 256, or have it learn each binding's limits when the Connect action carries them (T5).

## 1. Findings

### [P2-1] Overlapping requests on the legacy paths share one `Owner.session`, so per-request limits are judged against the combined count

**Location.**
- SSH: `gwz-core/src/git/endpoint/ssh_worker/runner.rs:37` (`session = "ssh-worker-{id}"`) and `:205` (`Owner::new(&session, serial)`).
- HTTPS: `https_worker/prepare.rs:242` and `throttle.rs:341` (`Owner::new(&input.session, …)`, where `input.session` is the mux session id, `https_endpoint/retry.rs:586`).
- Counting: `gwz-transport/src/pool/machine.rs` `owned_where` and `owner_full`.

**Invariant.** Contract §8 and the brief's red row: overlapping operations with differing `--max-per-host` are "each held to its own limit". Design §5: accounting is per owner.

**Reproduction.** Run `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/probe/tests/probe_state.rs` with `cargo test --features unstable-sequenced --test probe_state -- --nocapture`. It uses the production owner shape, one session with a different operation per request.
- Request A (limit 32) holds 10 leases. Request B (limit 4) is then granted 0 leases, because `owner_full(B)` compares B's limit of 4 with the session's 10.
- B waits in the pool until its allocation deadline, then fails.
- In the reverse order, B takes 4 first, and A (limit 32) then gets only 28 (`B got 4; A got 28`).
- The step's own red rows pass because `bindings_with_different_limits…` and the gwz-connection rows give each binding a distinct session. The gwz-core rows only assert recorded `limits_of(...)`, never behaviour. `an_operations_limits_hold_its_https_checkouts_and_nobody_else_s` shares the session "session" but does not test the reverse direction.

**Impact.**
- On the legacy session path, which is the only production path, differing-limit overlap is now admitted.
- It is not held per request. The smaller-limit request is starved while any larger-limit request holds that many entries, and a smaller-limit request can consume the larger one's share.
- Before the step the same overlap was refused immediately with a typed error. Now it is admitted and silently waits until `AllocationTimeout`. This is a new stuck-until-deadline state, though bounded.
- Direction is fail-closed: nobody exceeds its limit, and progress is lost, not invented.
- The evidence README states the shared owner ("one operation's workers share one limit as before") but not this consequence for overlapping requests.

**Remedy.**
- Make the accounting key unique per limit-bearing request on both schemes.
- SSH: thread the request id from the `OpenRequest` into `Owner.session`, for example `format!("{session}/{request}")`.
- HTTPS: do the same in `prepare.rs`, `throttle.rs` and `exchange.rs:483`.
- One request still maps to one owner, so a lone operation is unmultiplied.
- The alternative is an operator-acknowledged written limitation. That would have to withdraw the "each held to its own limit" claim for the legacy path.

**Closure test.**
- A gwz-core row with two overlapping requests on one runtime at limits 4 and 32, on SSH (the `opening_with` worker rig) and on HTTPS (`checkout_scoped`).
- It asserts the 4-request is granted exactly 4 while the 32-request holds 10.
- It also asserts a lone request at 32 is unchanged.
- Also add a pool-level row with a shared session, to document the key.

### [P2-2] The shared authority's ceilings are a one-way ratchet of session history, not the requesting reservation's own max

**Location.** `gwz-core/src/git/endpoint/shared_reservation.rs`: `admit_operation` (about line 401) and `State::take` (about line 427). The caller is `session/capacity.rs:25`.

**Invariant.** Contract §8: "The shared physical total/host ceilings remain the accepted provisional 256, taking max(ceiling, requesting limit) for new physical allocations." Design §5: "max(C_host, its per_host)", per reservation. Brief: "the host keeps physical ceilings", and a lone operation behaves exactly as before.

**Mechanism.**
- `admitted_total` and `admitted_per_host` only grow (`max`). Nothing lowers them: `grep admitted_` shows only the initialiser, `admit_operation` and `take`.
- `admit_operation` is not paired with any end or cancel hook.
- Every later reservation by anyone then uses `max(ceiling, largest limit ever admitted)`.

**Consequences.**
1. **Permanent raise.** A single request with a huge `--max-per-host` or `--jobs` raises the cross-scheme per-host and total ceilings for the rest of the session, after the request has ended.
   - `resolve_jobs` and `resolve_per_host` are unbounded above.
   - In a long-lived session this is a resource lever (file descriptors). It is bounded only by the pools' own per-request max, so at most about 2× per scheme.
2. **Limit below the default.** A lone operation with a limit below the 32 default no longer keeps its cross-scheme bound.
   - Before: `Authority::install_capacity(total, 4)` made the host bound 4 across SSH and HTTPS. The `shrinking_authority…` row asserted shrinking.
   - After: `admit_operation(256, 4)` leaves the host bound at 32. Each pool then holds its owner at 4, so one host reached over both schemes (for example github.com over SSH and HTTPS) can see up to 8.
   - This part follows from the design's own formula with a physical ceiling above 4. It contradicts the brief's hard "exactly as before" requirement, so it needs an explicit operator decision or a recorded exception.

**Reproduction.**
- (1) From the code and the unit rows: `Authority::new(2,2); admit_operation(16,16); admit_operation(4,4)` still allows 16 reservations. There is no inverse call to run after the 16-limit request ends.
- (2) Arithmetic from `take` plus `admit_operation(256, 4)` against `Authority::new(256, 32)`. I did not run an end-to-end mixed-scheme probe.

**Remedy.**
- Track admitted limits per live request: a map from request to `(total, per_host)`, set at admission and removed in `Session::cancel` or `finish`, with the effective ceiling computed as `max(base, max over live requests)`.
- Pass admission only after registration succeeds (see P3-2).
- Either record consequence 2 as an accepted design effect with operator sign-off, or make the cross-scheme per-host ceiling for a lone request equal to its limit.

**Closure test.**
- A row that admits 16, ends that request, then admits 4, and asserts the ceiling falls back.
- A row for two live requests at 16 and 4.
- A row for the mixed-scheme lone request at 4.
- The existing `an_admitted_limit_above…` and `admitting_a_lower_limit…` rows stay green.

### [P3-1] gwz-connection's misfit and lost-lease termination bound was shrunk below the number of idle entries it must outlast

**Location.** `gwz-transport/crates/gwz-connection/src/instance.rs`: `misfit_bound` (about line 666), used in `openings` (about line 559) and `lost_lease`.

**Invariant.** The bound exists to end a pathological loop, not to cap legitimate progress. Before, the bound was the pool's `per_user_host + 1` (33) and idle entries on a key could not exceed 32, so the bound always outlasted the entries.

**Mechanism.**
- The bound is now the binding's own `per_user_host + 1`.
- The instance ceiling is 256, so idle entries on a key can exceed the bound. Two overlapping bindings at 32 already leave 64.
- Each lost lease (an idle entry the adapter has already disposed of) counts one against `pending.misfits`.

**Reproduction.** `probe_tests.rs` in the scratch copy (`cargo test -p gwz-connection probe_lost -- --nocapture`).
- Eight idle entries, then a binding at 1 per host with 6 consecutive lost leases.
- The opening fails `NoFittingConnection` after 3 lost leases, with 5 idle entries left and room to open.
- Under the old formula (bound 33) it would continue to a new connection.

**Impact.**
- A typed open failure where recovery was possible, after a network flap or a failed proof retirement across many idle entries.
- Latent, because the gwz-connection consumer lands in phase 3.
- Bounded, because the failure is typed and retryable.

**Remedy.** Bound by `max(pool.capacity().per_user_host, limits.per_user_host) + 1`, or by the number of entries; either outlasts the idle entries.

**Closure test.** The probe converted to a regression row: 8 idle, 6 lost leases, a binding at 1 per host, and the open succeeds.

### [P3-2] Admission side effects precede registration, so a failed or duplicate-ID registration leaves state behind

**Location.** `gwz-core/src/transport_host/session/capacity.rs:11-35`. The validity check and the limits and ratchet writes happen under one lock, which is then released. `ClientRequest::new` then calls `register` under a second lock (`session.rs:~596`, where `owner.register` can still fail).

**Invariant.** A refused registration must not change durable state, and one request's limits must not be rewritten by another request.

**Interleaving (reasoned, not run).**
- Two concurrent `request()` calls with the same ID both pass the `used.contains` check and both write limits. The loser's `set_limits` overwrites the live winner's limits, including upward, before `register` rejects the loser. The winner's later opens read the loser's numbers.
- Any `register` failure, for example `owner.register(...)` returning a mux error, leaves an orphan `limits[request]` entry and has already ratcheted the authority (P2-2).
- Before, a differing-capacity install was serialised behind `admission_gate`.

**Impact.**
- Misbehaving-caller scope only.
- Bounded by 256 IDs for the orphan entries, except that failed registrations never reach `used`.

**Remedy.** Register first, or do check, register and record under one lock acquisition, with rollback on failure.

**Closure test.** A row where a duplicate-ID second admission fails and leaves the first request's limits and the authority untouched.

### [P3-3] `pool::Error::ActiveOperation` is now unproduced dead code

**Location.** `gwz-transport/src/pool/mod.rs:316`; the only reference is `gwz-core/.../classify_tests.rs:154`.

**Invariant.** Memory policy "remove dead code now". The removed `can_install_capacity` was its only producer.

**Impact.** A public enum variant that nothing can return, plus a test row that keeps it alive. Trivial.

**Remedy.** Delete the variant and the row, or record why the variant stays.

**Closure test.** `cargo build`. Nothing refers to the variant.

## 2. Invariant analysis

**Per-owner counting.**
- Counts are derived from `Entry.state` and `Entry.owner` at the moment of the check. There are no incremental counters, so there is nothing to leak or underflow on success, failure, cancel or abort. "Decrement exactly once" holds by construction.
- Opening and Leased entries count. Idle entries (owner cleared in `release_at`) and Closing entries (cleared in `start_closing`) do not.
- An Opening entry whose request was cancelled keeps `owner` and counts while it is physically connecting, which is correct.
- Every owner assignment is covered. `open_for` and the idle-lease in `schedule` set `owner`; Opening to Leased keeps it; `release_at` and `start_closing` clear it.

**Ceilings.**
- `ceilings_for` is `max(config, request limit)` per request, with no history. A lone operation keeps its limit. Overlapping operations share the ceilings, which matches design §5 and verification item 7.
- Holds are not owner-counted. Under the governor, holds exist only while a site limit is set (`settle_ms` is 0 in SATURATED), and `site_full` counts them. I found no regression there.

**Owner at limit.**
- `owner_full` guards idle reuse, `open_for` and the eviction pass, so an owner at its limit evicts nothing and does not displace a ceiling user.
- A request that is not owner-full but ceiling-blocked still evicts the oldest idle entry in the constrained group, as before.
- A blocked request does not hold the queue head. `schedule` iterates all waiting requests in FIFO order and skips owner-full ones, which satisfies contract §8 fairness.

**Wakeups.**
- Every release, discard, close, cancel and connect-failure path calls `schedule()`.
- The async `Pool::change` wakes all waiters on every revision change, so a request queued under one owner's limit is woken when that owner releases. The gwz-connection instance re-polls `take` on the same events.

**Legacy no-regression.**
- A lone operation: one request means one owner on both schemes, so limits are unmultiplied.
- Several requests with equal limits: they share the combined count, which is as before.
- Differing limits on one runtime: P2-1.
- `max_requests`: pool-wide `max(config, limit)` plus per-owner. The SSH admission counter uses `policy.max_requests.max(limits.max_requests)`, equal to the old value because the installed `max_requests` was at least 1024.

**Removed gates.**
- The admission and capacity gates, `CapacityMutation` and the retirement hooks all served resize exclusion. With no resize, nothing they excluded remains.
- The pool numbers (limit, settle, no-evict) that `install_capacity` cleared are now cleared by `Governor::reconcile` when no live scope wants a site. That runs from `end_operation`, which both `cancel_request` paths call.
- The one gap is a late `recheckout` or open after cancel. It falls back to the pool's own capacity, which is more permissive but bounded and post-finish.

**Fail-closed direction.**
- The pool changes only lose progress (waiting).
- The authority ratchet (P2-2) is fail-open for a resource cap.
- No finding invents progress.

**Reserve-before-connect under two live limits.** The authority defers the connect without failing it; `a_connection_waits_for_a_reservation_under_a_higher_limit_too` covers this. The authority is bounded by the largest admitted limit, as P2-2 notes.

**Compatibility.** `TransportCapacityConflict` has no remaining reader or schema, and the numeric enum risk does not apply (see the scope section above).

## 3. Risks and next action

**Next action.** Remediate P2-1 and P2-2, then re-review those two with their closure rows. P3-1, P3-2 and P3-3 may be fixed in the same round or recorded.

**Residual risks (not findings).**
- The latent factory-authority mismatch for phase 3 (scope section above).
- The Windows receipt and the Linux matrix are running separately. I did not assess them.
- Performance: `owner_full` does three scans of the entries per waiting request, inside `schedule`'s three passes. This is the same order as the existing `host_total` scans. I did not measure it.
- The claim that older-era pool numbers outliving an operation are only a bounded delay rests on the governor tests I read, not on an end-to-end run.
