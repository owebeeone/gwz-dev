# Step 2.2b operation service, round 2: Code-axis re-verdict

**Object:** gwz-core `08ce3de2..9359a180` (24 files), evidence `0d9c4a2c` (`runs/2026-10-25-p2-2b/rem1/`).
**Tuple:** gwz-core `9359a180b5`, evidence `0d9c4a2c39` and gwz-transport `22b53dce5d`, unchanged at start and end.
**Date:** 2026-10-26. **Axis:** Code.
**Verdict:** **GO.** All four round-1 P2s are closed. Two new P3s, neither architectural.

## Closure of round-1 findings

| ID | Round-1 counterexample | Result on this tree | Status |
| --- | --- | --- | --- |
| P2-1 | The both-switch shape failed to compile with E0063 at `cli_placement.rs:125`. | `cancellation: None` was added at `cli_placement.rs:146–148`. Evidence shows the remote `both-switch-check` PASS twice (`rem1b-both-switch.log`, `raw/remote*/summary.txt`). | **Closed** |
| P2-2 (architectural) | The crate carried its own per-call lane routing that duplicated 2.1's. | `crates/session-host/src/lane.rs` and its tests are deleted, and the `lib.rs` exports are gone. `ReplySink::send_lane` is removed. The crate keeps `Service::route` (tags 1–8, tag 1 only once open) and `is_live(request_id)`. `served/lane.rs` is now just the `TransportLaneFrame` codec. A grep finds no `receive_window`, `StreamTarget`, `Lanes`, `CallLane` or `session_id` state in the crate or in `served`. The ownership decision is recorded (RemPlan decision 1). The single-implementer assertion lands at the 2.1 merge, as planned. | **Closed in this lane** (the merge assertion is carried) |
| P2-3 | A worker's reply was flushed after unlocking, so the close report or channel close could overtake it. | Every lock section that builds replies now increments `unflushed`: receive (`service.rs:709`), admission (`table.rs:846`), workers (`workers.rs:87`), the read loop (`events.rs:148`) and close (`workers.rs:217`, `:282`, `:308`). `flush` always decrements and notifies (`service.rs:763–787`). Close waits for zero before step 8 and again before step 9 (`workers.rs:290–323`). I checked every `flush` call site, and each is paired. The delayed-sink row `a_settled_reply_is_never_overtaken_by_the_close_report` fails red with the fix removed (`rem1-red-close-order.log`). | **Closed** |
| P2-4 | `record.alive` kept a strong marker after `settle`. | `settle` now sets `record.alive = None` (`table.rs:738–740`). Close records the detached worker before settling it. The submit variant `a_detached_submitted_operation_releases_the_workspace_when_its_worker_ends` has a red log (`rem1-red-detached-submit.log`). | **Closed** |
| P3-1 | The 64-open-log limit was not enforced. | `served/spool.rs` reserves a slot before `diff`/`log` register a log, and with nothing it can release it refuses with 77 (`spool.rs:146–170`). Red log `rem1-red-open-logs.log`. | **Closed** |
| P3-2 | One view marked a merge delivered. | Views are now tracked separately (`result_delivered`/`response_delivered`, `response_view` for merge). Delivery is marked only after the reply is handed to the sink (`mark_delivered`). Row `a_record_is_evicted_only_once_every_declared_view_is_delivered` has a red log (`rem1-red-views.log`). | **Closed** |
| P3-3 | The tail check refused any code appended after 86. | The check is now anchored on `capacity` (76) (`check_log_additive.py:267–278`), so a later code after 86 no longer trips it. | **Closed** |
| P3-4 | No production handler read the cancellation token. | `CoreDispatch` refuses a call that is already cancelled at entry (`dispatch.rs:89–96`). The module doc records the remaining gap as plumbing-only, owned by 2.2c. | **Closed as a recorded limitation** |
| P3-5 | Rows 1–3 had no red evidence. | `rem1-red-row1-live-registry.log`, `rem1-red-row2-cancel.log` and `rem1-red-row3-events.log` are archived. Row 1 now runs through the test router (`test_route`, `service/tests.rs:136`). | **Closed** |
| P3-6 | Schema comments said "absent" where the key is required. | Both comments now say "the key is always present; null means …" (`gwz.taut.py:2713`, `:2716`). | **Closed** |
| P3-7 | The member lock manager had no owner. | It is assigned to 2.3 (decision 4), and the effect on two fetches within one session is recorded in the `served.rs` module doc. | **Closed** (record-only) |

Two other round-1 items are also resolved:
- **Risks:** `run_tests.py` now runs `cargo test` and `clippy --all-targets -D warnings` for `gwz-session-host` (`SESSION_CRATES`).
- **2.3 seam:** the `Service::open()` doc now says to call it before writing `SessionOpened`.

## New findings

- **N-P3-1 (not architectural): close can deadlock when its thread fails to spawn.**
  - **Location:** `service.rs:709` increments `unflushed` before `table::receive`. For `session.close`, the `Request::Close` branch calls `workers::close` (`table.rs:399–401`). If the close thread cannot be spawned, `run_close` runs inline on the reader thread (`workers.rs:207–209`). Step 8 then waits for `unflushed == 0` (`workers.rs:290–296`), but the reader's own count is released only after `table::receive` returns.
  - **Impact:** only when thread creation fails, which round 1 handled by running close inline. In that case the reader hangs, the close report is never sent, the channel never closes and the session never ends.
  - **Correction:** in the inline fallback, either uncount the caller's batch before running close, or defer `run_close` until after `receive`'s flush.
  - **Closure test:** inject a spawn failure into `workers::close` and check that `session.close` is answered and `wait_ended` returns.
- **N-P3-2 (structural, for the operator): two files cross the 1,000-line split threshold.** `crates/session-host/src/service/table.rs` is now 1,015 lines and `service/tests.rs` is 1,168. The lane rule is to split files over 1,000 lines with `rust-split` at a stable point. Surface this as a decision for the operator; it does not block GO.

## Carried to the merges (not findings)

- The single-implementer source assertion when 2.1 merges.
- Rows 1 and 4 re-pointed at 2.1's `FrameRouter`.
- 2.1's provisional lane body moves to the frozen `TransportLaneFrame` at 2.2c.
- Recording decisions 3 and 4 in the plan text (lane owner, clerical).
- The 2.2 port plan from round 1, including `cancellation` in the now-ordinary `cli_placement.rs`.
