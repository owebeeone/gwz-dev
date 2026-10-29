# Step 2.2b operation service: remediation plan (round 1)

Date: 2026-10-25. Lane owner's plan after [Code](GwzSplitCommandChannel-Step2.2b-ReviewCode.md) (NO-GO: 4 P2, 7 P3) and [State](GwzSplitCommandChannel-Step2.2b-ReviewState.md) (NO-GO: 5 P2, 5 P3) on gwz-core `08ce3de273` (lane p2-opsvc). Both pre-committed to GO on bounded remedies. One patch; the same two reviewers re-verdict.

**Architectural root-cause count on this object: 1.** C-P2-2 and S-P2-4 are one root cause, found blind by both axes: the crate carries its own transport-lane routing (`Lanes`, `route_frame`'s lane branch, `CallLane`), keyed by call rather than binding, duplicating what 2.1's carrier implements per 2.0 §3.3 "Reader interface".

Convergences: C-P2-2 = S-P2-4 (lane); C-P2-3 = S-P2-2 (close ordering); C-P2-4 = S-P2-1 (detached record); C-P3-2 ⊂ S-P2-3 (eviction by one view); C-P3-1 ⊂ S-P2-5 (spools); C-P3-7 = S-P3-4 (member lock manager).

## Lane owner's decisions

1. **Ownership of the transport lane (closes C-P2-2 = S-P2-4):** 2.1's carrier (`FrameRouter`, per-binding state, tags 16/17, per-stream bounds, binding-only faults) owns lane routing, as 2.0 §3.3 "Reader interface" and this step's brief ("a local fake router in your tests") say. 2.2b deletes `Lanes`' admission state, `CallLane` and `route_frame`'s lane branch from `gwz-session-host`, and `src/session_host/served/lane.rs`'s routing (the taut `TransportLaneFrame` codec stays as the frozen body form; 2.1's provisional body moves to it at 2.2c). `Service::receive` keeps the registry-class hand-off and exposes the seam a router calls for tags 1–8. Rows 1 and 4 run through a test router (the brief's fake) now and are re-pointed at 2.1's `FrameRouter` at the merge. A source assertion that exactly one type implements per-binding transport state lands at the merge.
2. The both-switch shape is a gate: `RUSTFLAGS='--cfg gwz_transport_candidate --cfg gwz_session_candidate' cargo check --lib --profile test` runs in this patch and is added to the remote runner by the lane owner (`--shape both`).
3. The two §4.2 deviations (probe reads for `diff.output`; a spent cancel → `operation_expired`) are accepted and recorded by the lane owner in 2.0 §3.2 at the next clerical amendment, not silently.
4. The §5.1 member lock manager is assigned to step 2.3 (plan clerical, lane owner).

## Blocking

| ID | Disposition | Where | Closure |
| --- | --- | --- | --- |
| C-P2-2 = S-P2-4 | Decision 1 above, executed now in this lane: delete the duplicate routing; keep the registry hand-off and the frozen `TransportLaneFrame` codec; rows 1 and 4 through a test router. | `crates/session-host/src/lane.rs`, `lib.rs`, `src/session_host/served/lane.rs` | `grep` finds no per-binding transport state in `gwz-session-host`; rows 1 and 4 pass through the test router; the crate still decodes no body. |
| C-P2-1 | `cancellation: None` added to the exhaustive literal at `src/transport_host/cli_placement.rs:125` (authorized here although 2.2 owns the file; trivial at the merge). | `cli_placement.rs` | both-switch `cargo check --lib --profile test` exits 0; hosted `transport-and-session` leg green after push. |
| C-P2-3 = S-P2-2 | Sends ordered with settlement: count unflushed outgoing batches under the table lock; close waits for zero before step 8 (report) and step 9 (channel close); `flush` never silently ignores `Closed` for a terminal reply (log it as a dropped-reply fault, which the row asserts never happens). | `service/workers.rs:83–92, 236–301`, `table.rs:788–792`, `service.rs:676–730` | delayed-sink row: a settled unary reply precedes the close report; `closed` last; run 1000× with an injected yield. |
| C-P2-4 = S-P2-1 | Clear `record.alive` in `settle` (the worker's thread owns the only strong marker; close records a weak reference). | `workers.rs:39–41, 256–258`, `table.rs:701–751`, `detached.rs:63–75` | the submit variant of `close_settles_cancels_detaches…`: `running_on` turns false within the bound after the worker ends; a later `commit` in another session runs. |
| S-P2-3 (+ C-P3-2) | Per-view delivery: result and response tracked separately; `HostPorts`/`classify` names the views a method declares (result always; response for merge); evict only when every declared view is delivered or the record released; the flag set when the reply is handed over. | `table.rs:54–55, 235–247, 515–519, 698–700` | table of 2: merge with only its result read → third admission `SessionFull`, `operation.response` still answers; fetch with only `operation.response` read likewise. |
| S-P2-5 (+ C-P3-1) | A served spool layer over the two registries: 64 open logs, release-to-make-room (sealed ≥ 30 s, no reader) else `transport_session_full`; release on last reader end; `read_bytes` applied to `log.output` (≥ 1 record, ≤ 1 MiB, split or refuse a single record over the limit); §5.4 codes (`operation_expired` for released, `operation_not_found` for another session's). | `served.rs:552–557`, `served/spool.rs`, `limits.rs` | 65th `log` with 64 unreleasable → 77; sealed+ended diff log released → 78 on read; `log.output` ≤ 1 MiB over 512 KiB messages; foreign `log_id` → `operation_not_found`. |

## Non-blocking (same patch)

| ID | Disposition | Where |
| --- | --- | --- |
| S-P3-1 | `schedule` re-checks `detached.running_on` and settles `WorkspaceBusy`. | `table.rs:905–938` |
| S-P3-2 | Refusal lane derived from the method name before decoding. | `table.rs:281–286` |
| S-P3-3 | Accepted deviation (decision 3); module doc says so. | doc |
| S-P3-4 = C-P3-7 | Decision 4 (plan clerical, lane owner); the within-session difference recorded in the module doc. | doc |
| S-P3-5 / Code risk | `cargo test -p gwz-session-host`, `--doc` and `clippy --all-targets -D warnings` added to `run_tests.py`'s groups (so the remote runner covers them). | `scripts/run_tests.py` |
| C-P3-3 | `check_appended_codes` anchors on `capacity` (76) with 77–86 consecutive; a future code after 86 passes; a taken 77 still refuses. | `check_log_additive.py:266–267` |
| C-P3-4 | `worker.token()` passed into the handler context; a `CoreDispatch` handler observes cancellation at a gated step (or, if no production handler reads it yet, record the row as plumbing-only with owner 2.2c). | `served/dispatch.rs`, `served.rs:240` |
| C-P3-5 | Mutation reds for rows 1–3 archived. | evidence |
| C-P3-6 | Schema comments say "null means …" for `SessionCall.mode`/`log_verb`, matching the decoder. | `gwz.taut.py:2713, 2716` |
| 2.3 seam | `Service::open()` doc: 2.3 calls it BEFORE writing `SessionOpened`. | `service.rs:649` |

## Observations carried

gwz-py IR drift (3.2 / integration); 2.1's provisional lane body vs the frozen `TransportLaneFrame` (2.2c adopts the frozen form); `CallControls::cancel()`/`revoke()` under the table lock (bound before 2.2c registers callbacks); `pending_local_work` real cleanup via 2.2c; size and `tests/protocol.rs` (operator).

## Process

One patch in lane p2-opsvc; gates: both-switch check, crate tests/doc/clippy, remote matrix (`p2-2b-rem1-20261025-a`), Windows receipt `p2-2b-rem1-20261025-a` (scripts/ and protocol pins change); evidence appended to run `2026-10-25-p2-2b`; the lane owner commits; re-verdict by the same reviewers (`-ReviewCode-2.md`, `-ReviewState-2.md`). Round count after this patch: 1 of 2.
