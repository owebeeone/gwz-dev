# Step 2.2b operation service, round-2 re-verdict on the State axis (gwz-core 9359a180)

**Verdict: GO.** All five round-1 P2s are closed, and each closure test passes on this tree. There are no open P0–P2 findings. Two new P3s (P3-6 and P3-7 below), neither architectural, should be fixed or recorded before the merge.

**Object:** gwz-core `08ce3de2..9359a180` (24 files) and evidence `0d9c4a2c` (`runs/2026-10-25-p2-2b/rem1/`).
**Tuple:** checked at start and end, unchanged: gwz-core `9359a180b5` (clean), gwz-core-evidence `0d9c4a2c39`, gwz-transport `22b53dce5d`.

**Commands** (1-minute load 4–8; `TMPDIR=/Volumes/gwztmp`; `CARGO_TARGET_DIR=/Volumes/projects/limbo/build-targets/p2-opsvc-ordinary`):
- `cargo test -p gwz-session-host`: 55 lib, 1 doctest and 4 compile-fail tests passed.
- `cargo test --lib session_host::` in gwz-core: 42 passed.

## Closure of round-1 findings

| ID | Round-1 counterexample | Fix on this tree | Status |
|---|---|---|---|
| P2-1 | A detached *submit* record kept the workspace recorded after its worker ended | `settle` sets `record.alive = None` (`table.rs`). Close records the weak marker *before* settling, so the worker thread now holds the only strong marker. Test `a_detached_submitted_operation_releases_the_workspace_when_its_worker_ends`: `running_on` turns false while `host` is still alive, and a `commit` in another session then runs. Mutation red `rem1-red-detached-submit.log`. | **Closed** |
| P2-2 | A settled reply's flush could land after the close report, or be dropped by `close_channel` | Every batch is counted in `unflushed` while the table lock is held (receive, worker, admission, read loop, each close batch) and uncounted in `flush`. Step 8 and step 9 both wait for zero. A `Closed` refusal is now counted in `dropped_replies` instead of being silently ignored. Delayed-sink test passes, including 1000 runs with injected yields; mutation red `rem1-red-close-order.log`. The step-9 wait has no mutation red of its own; it is recorded and only covers refusals sent after close starts. | **Closed** (one gap, see P3-6) |
| P2-3 | Eviction counted a record as delivered after any one view | Result and response delivery are tracked separately. `response_view` is true only for `merge` (`methods.rs`). The flag is set in `flush` only after `Ok` from the sink. Test `a_record_is_evicted_only_once_every_declared_view_is_delivered` covers both the merge case and the response-only fetch case; mutation red `rem1-red-views.log`. | **Closed** |
| P2-4 | Lane state was keyed by call, not binding (architectural) | The duplicate routing is deleted from `gwz-session-host`: `Lanes`, `CallLane`, `send_lane` and `route_frame`'s lane branch are gone, and grep finds no `receive_window`/`faulted`/`CallLane` state. What remains is `Service::is_live(request_id)` and `Service::route` for tags 1–8. Lane routing is reassigned to 2.1's router, which matches 2.0 §3.3 "Reader interface" and the lane brief. The both-switch check passes (`rem1b-both-switch.log`). The bounds, scope and fault-scope tests become 2.1/merge obligations, as RemPlan decision 1 says. | **Closed** (moved to 2.1, not fixed here) |
| P2-5 | Served spools had no 64-open cap, no release rule, wrong codes, and `log.output` replies had no byte limit | New spool layer in `spool.rs`: per-session id prefix; `reserve()` with the 64 cap, release-to-make-room (sealed ≥ 30 s, no reader), else 77; released → 78; never issued or foreign → `operation_not_found`; last diff reader ending releases the log; `log.output` reads one record at a time up to `read_bytes`, and a single record over the limit is refused. Tests `the_open_log_after_the_limit_…`, `a_sealed_diff_log_is_released_…`, `a_log_output_reply_stays_within_the_read_limit`; mutation red `rem1-red-open-logs.log`. | **Closed** (one gap, see P3-7) |
| P3-1 | A queued write was not re-checked against the detached record | `schedule` re-checks for W and push and settles `WorkspaceBusy`. Test `a_queued_write_is_refused_…`. | **Closed** |
| P3-2 | A malformed control call's refusal went on the call lane | `HostPorts::lane(method)` decides the lane before decoding. Test `a_malformed_control_calls_refusal_takes_the_control_lane`. | **Closed** |
| P3-3 | Cancelling a spent call returned `operation_expired` | Accepted as a deviation (RemPlan decision 3) and noted in the module doc. The 2.0 §3.2 amendment is still pending with the lane owner. | **Accepted** |
| P3-4 | Member lock manager not built | Assigned to step 2.3 (decision 4); the within-session difference is noted in the module doc. | **Accepted** |
| P3-5 | No gate ran the crate's tests | `run_tests.py` now runs `cargo test` and `clippy -D warnings` for `gwz-session-host`. The Linux matrix shows `run_tests` 2455 passed. | **Closed** |

## New findings

### P3-6: The `unflushed` count has no guard and the close waits on it have no bound, so a counted batch that never flushes hangs close forever (not architectural)

- **Location:**
  - `service.rs` `Service::receive`: `unflushed += 1` happens *before* `table::receive`, and the matching `flush` runs only after that returns.
  - `table.rs` Close path: `workers::close`.
  - `workers.rs` `close`: falls back to running `run_close` inline when the close thread cannot be spawned.
  - `run_close` step 8 and step 9: `while unflushed > 0 { settled.wait }`, with no deadline.
- **Interleaving:**
  1. `session.close` arrives on the reader thread, and `receive` counts its batch (`unflushed = 1`).
  2. The close thread fails to spawn, so `run_close` runs inline on the reader thread.
  3. Step 8 waits for `unflushed == 0`. That count only drops when this same thread returns and flushes, so it never does.
  4. Result: close never completes, the channel is never closed, and the reader thread is stuck for good.
- **Wider effect:** the same hang happens if a port panics between the increment and `flush` (for example while encoding under the lock). Before remediation the inline fallback did not deadlock.
- **Correction:** make the count an RAII guard that decrements on drop, including during unwinding. Start the inline close only after `receive` has flushed its own batch. Bound the step-8 and step-9 waits by the close bound.
- **Closure test:** inject a spawn failure in `workers::close`. `session.close` must return its report and close the channel within the bound.

### P3-7: The spool registries can hold logs the cap table does not own (not architectural)

- **Location:**
  - `diff/handle_diff.rs:216-222`: `registry.create()` runs before `build_producer_targets(..)?` and `run_producer(..)?`.
  - `dispatch.rs`: when a handler errors, its `Reservation` is dropped, but the log it already minted stays in `spools.diffs`.
  - `spool.rs` `Reservation::commit`: does not check `table.ended`. A detached `diff`/`log` worker that commits after step 7's `release_all` inserts a log that is never released.
- **Violated invariant:** §5.4 (64 open logs; every log released at the session's end) and §8 step 7.
- **Impact:**
  - Each `diff` call that fails after minting leaks one in-memory `DiffLog`, uncapped, for the rest of the session.
  - A commit after close leaks at most the 64 outstanding reservations (each `log` one holding a tempfile descriptor) until the session's data drops.
  - Both are bounded by the session's lifetime, so this is not a stuck state.
- **Correction:** on handler error, release the log the handler minted. In `commit`, if `ended` is set, release the log immediately.
- **Closure test:** a `diff` whose producer fails leaves the registry empty, and a commit after `release_all` leaves no open log.

## Risks carried forward

- **For 2.1/merge:** binding scope, per-stream bounds (frame count and number of targets), binding-only faults and the single-owner source assertion now sit with 2.1's router. The rem1 tests do not cover them; the merge review must.
- **Before 2.2c registers callbacks:** `CallControls::cancel()` and `revoke()` still run while the table lock is held.
- **For R5:** `pending_local_work` reflects only detached workers until 2.2c feeds real cleanup into `Outcome.cleanup`.
