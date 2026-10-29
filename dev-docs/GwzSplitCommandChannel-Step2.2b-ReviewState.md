# Step 2.2b operation service (gwz-core 08ce3de2): State-axis review

**Review object:** gwz-core `9a9a659e..08ce3de273`. This covers the operation service in `crates/session-host` and the core side in `src/session_host/served/`, plus the schema additions, codes 77–86, `check_log_additive.py`, `convert.rs`, the docs, and evidence run `gwz-core-evidence` `17789b74d0` `2026-10-25-p2-2b`.
**Baseline:** lane `/Volumes/projects/limbo/gwz-dev-p2-opsvc`, gwz-core `9a9a659e` (gwz-transport pin `22b53dce5d`).
**Date:** 2026-10-25
**Axis:** State
**Verdict:** **NO-GO.** P0 0, P1 0, P2 5, P3 5. Every P2 has a bounded fix. If each P2 closes with its closure test, and the P3s are fixed or recorded, I commit to GO.

## 0. Evidence base

- **Tuple, checked at start and at end and unchanged:**
  - gwz-core `08ce3de273` (clean)
  - gwz-core-evidence `17789b74d0` (only the inherited `runs/2026-09-22-alpha-setup-timeout/` is untracked)
  - gwz-transport `22b53dce5d`
  - gwz-git `e3bc2a41f6`, gwz-py `22e4f133a4`, gwz-cli `9ecc2b7c1b`, lane root `a32ae33f60`
- **Evidence matches the commit:**
  - `raw/sources/gwz-core-tracked.patch` has sha256 `19300e60…`. That equals `git diff --binary 9a9a659e 08ce3de273 --diff-filter=M`.
  - All 15 files in `gwz-core-untracked.tar` are byte-identical to the files `08ce3de273` adds.
- **Commands run** (1-minute load 7–8; `TMPDIR=/Volumes/gwztmp`; `CARGO_TARGET_DIR=/Volumes/projects/limbo/build-targets/p2-opsvc-ordinary`):
  - `cargo test -p gwz-session-host`: 54 lib tests and 5 doctests passed.
  - `cargo test --lib session_host::` in gwz-core: 38 passed. These include rows 2, 3 and 5 (`cancel_reaches_the_handler`, `events_arrive_before_the_reply`, `cancel_during_a_saturated_spool_attachment_progresses`).
- **Read in full:**
  - In `crates/session-host/src/`: `service.rs`, `service/{table,workers,events}.rs`, `detached.rs`, `lane.rs`, `lane/tests.rs`, the harness and close, eviction and limit tests in `service/tests.rs`, and `gate.rs` `revoke`.
  - In gwz-core: `src/session_host/served.rs`, `served/{methods,dispatch,spool,lane}.rs`, rows 3 and 5 in `served/tests.rs`, `session_host/context.rs`, the `convert.rs` and `error_code.rs` diff, and `operation/workspace_mutator_lock.rs`.
  - The `DiffLogRegistry` (`diff/log_service.rs:330-389`) and `CommitLogOutputRegistry` (`operation/commit_log/handler.rs:19-20, 232-296`).
  - gwz-cli `globalargs/dispatch.rs` (to check whether the CLI takes locks outside the handlers: it takes none).
- **Controlling text read:**
  - 2.0 rev 3: §1 (codes and the second-client row), §3.1–§3.4.
  - Session design §3, §4.1–§4.3, §5.1–§5.6, §6–§8, §10:490.
  - Plan §1 threat model and steps 2.2b, 2.2c and 2.3.
  - The lane brief.
- **No new code was compiled or run.** Every interleaving below comes from reading the code. Each one names the test that would reproduce it.

## Problem, requirement provenance, and scope

- **Requested outcome:** plan step 2.2b. The served core implements the session design's §4.1–§4.2 methods against the §5 clauses that 2.0 §3.2 adopts:
  - records at receipt
  - admission in receipt order
  - classes R, N and W
  - the §1 limits with eviction (§5.4 is adopted unchanged)
  - direct workers
  - cancel, events and close (§8)
  - the transport lane of 2.0 §3.3
  - the five red rows
- **Provenance:** every finding below cites a clause the step adopts: 2.0 §3.2's table, 2.0 §3.3, or session design §5.1, §5.4, §8 or §10. None rests on an implementation preference.
- **Components touched:** only gwz-core and gwz-core-evidence. I found no unauthorized expansion.
- **Out-of-scope observations (not findings):**
  1. gwz-py's packaged IR now drifts from the schema (recorded in `raw/mac/finding-gwz-py-drift.log`). `convert.rs` now puts 77/78 on the wire where it used to put `io_error`; the session design §4.2:102 mandates that. A gwz-py built from the old IR that decodes a `GwzError` with code 77/78 from a served core would see an enum value it does not know. That needs the gwz-py regeneration step.
  2. Session design §4.2 discards a unary record once its reply is sent, and a unary client learns its `operation_id` only from that reply. So `events.subscribe` cannot serve a unary call's progress except by guessing the predictable `op-N`. This is a design gap, not a 2.2b defect.

## 1. Findings

### P2-1: A detached submitted operation keeps the workspace recorded after its worker ends (stuck state)

- **Location:**
  - `crates/session-host/src/service/workers.rs:39-41`: `take` stores `record.alive = Some(alive.clone())`, a strong `Arc`.
  - `workers.rs:256-258`: close records that same marker.
  - `service/table.rs:701-704, 749-751`: an accepted submit is kept in `retained`, not discarded.
  - `detached.rs:63-75`: an entry stays while `strong_count() > 0`.
- **Violated invariant:**
  - Session design §8 step 5 records the worker "until it ends", and 2.0 §3.2 adopts that.
  - The crate's own doc (`detached.rs:8`) says "until the worker's thread ends".
- **Interleaving:**
  1. Session A submits `commit` (class W) and its handler hangs.
  2. A sends `session.close`, the close bound passes, and step 5 detaches the worker and records it.
  3. The worker later returns, and `run` drops its own marker (`workers.rs:94`).
  4. The settled record is a retained submit and still holds the marker's clone. Nothing discards it after the session ends: no release or eviction is possible, and `ended` clears nothing.
  5. So `running_on(root)` stays true for as long as anything holds A's `Service`/`Inner`. Every `commit`, every other W method and every `push`, in every session of the server, is refused with `WorkspaceBusy` (wire `unsupported_operation`).
  6. If the server keeps closed sessions' handles, the workspace is wedged until the process exits.
- **Why the existing test misses it:** `close_settles_cancels_detaches_and_reports_and_the_detached_record_refuses_writes` (`service/tests.rs:731-788`) uses a unary call (`call("commit", false, …)`), whose record is discarded at settle.
- **Impact:** the failure is a refusal, not unsafe concurrency. But it is a stuck state, and the record no longer means what close reports.
- **Correction:** drop the record's copy of the marker when the record turns terminal (`record.alive.take()` in `settle`). Better still, only the thread should own the marker, and close should take a weak reference from it.
- **Closure test:** the same test with `call("commit", true, "hold-stubborn")` while `host` stays alive. After `release("hold-stubborn")`, `detached.running_on("/ws")` must turn false within the bound, and a later `commit` in `other` must run.

### P2-2: Replies settled before close can be overtaken by the close report or dropped by step 9

- **Location:**
  - `crates/session-host/src/service.rs:721-730`: `flush` sends after the table lock is released.
  - `workers.rs:83-91`: a worker settles under the lock, unlocks, then flushes.
  - The admission thread (`table.rs:788-792`) and the read thread (`events.rs:146-171`) do the same.
  - `workers.rs:285-301`: the close thread sends the report, then calls `close_channel()`.
  - `flush` silently ignores `SendError::Closed`.
- **Violated invariants:**
  - 2.0 §3.3 "Close report": it is written "only after every frame the host queued before it", and the channel closes only after the writer drains.
  - Session design §8 step 6: every outstanding call is answered before step 8.
  - §10:490: a result or response reply is never dropped.
- **Interleaving:**
  1. A unary `push` worker W returns `Completed`. `settle` marks it terminal and builds its reply, then notifies `settled` under the lock and unlocks.
  2. W is descheduled before `flush`.
  3. The close thread C, waiting in step 4, takes the lock and sees nothing running. It runs steps 5–8, which include this push's cleanup in the report, flushes the report and calls `close_channel()`.
  4. W resumes. `send_reply` returns `Closed`, and the push's success reply is lost.
  5. The client fails that call as undelivered and must report an unknown or possible effect for a push that completed. The close report says the push finished.
- **Smaller variants:**
  - An `accepted` reply flushed by the admission thread can arrive after the report.
  - A held read answered by the read thread can arrive after the report.
- **Byte streams:** the same race applies to every byte-stream sink, since ordering there is defined by send order.
- **Correction:** give sends one order that is tied to the table lock. Two options:
  - Hand frames to the sink while still holding an ordering lock that is taken before the table lock is released (lock coupling).
  - Count in-flight flush batches under the table lock, and have close wait for that count to reach zero before steps 8 and 9.
- **Closure test:** a test sink that blocks `send_reply` for one chosen `call_id` until released. Settle that call, start `session.close` (close_wait large), then release the block. Assert that the sink receives the call's reply before the close report and that `close_channel` comes after both. Run it 1000 times with a yield injected between unlock and flush.

### P2-3: Eviction counts a record as delivered after any one view, so undelivered results or responses are evicted

- **Location:**
  - `crates/session-host/src/service/table.rs:54-55`: one `delivered: bool`.
  - `:515-519` and `:698-700`: either `operation.result` or `operation.response` sets it.
  - `:235-247`: `evict` takes the oldest `delivered` record.
  - `service/tests.rs:667-695` encodes the rule: op-2 counts as delivered after `operation.response` alone, and op-1 after `operation.result` alone.
- **Violated invariant:** session design §5.4, adopted unchanged by 2.0 §3.2. A record is delivered "once every view its method declares has been delivered". The views are the result for every method, and for a merge the response as well. "Terminal records not yet delivered are never evicted." §10:490 builds the bridge's never-dropped guarantee on that.
- **Interleaving:**
  - Merge case:
    1. The table is near 128 entries.
    2. The bridge reads a submitted merge's `operation.result` but not yet its `operation.response`.
    3. The next admission evicts the merge.
    4. `merge_operation_response` gets `operation_expired`, and the response is lost for good.
  - Fetch case: a client reads only `operation.response` of a fetch. The `OperationResult`, which is the declared view, can be evicted unread.
- **Impact:** a terminal record the client has not read is lost when limits are reached. That is exactly the durability property the 128-entry limit was meant to keep. The refusal it should have produced (`transport_session_full`, attributable to the client's own unreleased records) never happens.
- **Correction:**
  - Track result and response delivery separately.
  - Let `HostPorts` (or `classify`) say which views a method declares: result always, response for `merge`.
  - Evict only when all declared views are delivered, or the record was released.
  - Set the flag when the reply is handed over, not when it is built.
- **Closure test:** a table of 2.
  - Submit a merge-class call and read only its result. A third admission must be refused with `SessionFull`, and the merge's `operation.response` must still answer.
  - Repeat for a fetch with only `operation.response` read.

### P2-4: The transport lane's state is keyed by call, not by binding, so scope, per-stream bounds and fault scope fall short of 2.0 §3.3. ARCHITECTURAL (interface shape)

- **Location:**
  - `crates/session-host/src/lane.rs:138-155`: one `CallState` per `request_id`, with no binding registry.
  - `:194-221`: `route` accepts any `session_id`, and a fault sets the call's `faulted`.
  - `:225-267`: `admit` creates `data` entries for any target and queues every frame.
  - `src/session_host/served/lane.rs:24-30`: a negative `stream_id` is folded to 0.
  - `served/lane.rs:74-79`: the whole raw frame body is retained.
  - `lane/tests.rs:113-156`: "faults that binding only" is tested as "the other call goes on".
- **Violated invariants (2.0 §3.3):**
  - "Scope": `session_id` "is the binding, valid only on the carrier connection and request that bound it". A frame for a retired binding "is discarded with no state created".
  - "Transport-class bound": per-stream state bounded by quantities the sender controls, with `data` appended to a buffer bounded by `receive_window`.
  - "Fault scope": a fault affects "that binding only", and the carrier's other bindings continue.
- **Reproduction** (fake router, as in `lane/tests.rs`). For a registered `r1`:
  1. Scope: a `data` frame with an unknown or retired `session_id` returns `Delivered` and creates `data[(r1, bogus, n)]`. It should be discarded.
  2. Bound: N frames with `Data(0)`, or with distinct `(session_id, stream_id)` targets, are all queued. Each target gets its own `receive_window`, and each frame keeps its full body. A frame of up to 64 MiB counts as 0 bytes when its envelope has no payload, or when non-payload fields such as `session_id` are large. Memory is bounded neither by frame count nor by the number of targets.
  3. Fault scope: a second `closed` on stream 2 faults the whole call, and data already queued for stream 1 and every other binding of the same call is cleared (`inbound.queue.clear()`). A multi-member operation's other bindings die with it.
- **Impact:**
  - It is latent: no `CallLane` is registered until 2.2c. The `#[allow(dead_code)]` on `ServedSession.lanes` confirms that.
  - But 2.2c would inherit a frozen lane API that cannot express a binding.
  - A buggy client could grow the core's memory without bound once it is wired.
  - `served/lane.rs` has never been compiled (a recorded limitation), so its folding and retention behaviour has never run.
- **Correction:**
  - Give `Lanes` per-call bindings that the core registers when it issues `bind` (the core picks `session_id`, 2.0 §3.4) and retires on finish.
  - Discard frames for unknown or retired `session_id`s without creating state.
  - Bound streams per binding by the binding's `Limits`.
  - Keep `data` as a byte buffer, or bound queued frames per stream by the negotiated `queued_frames`.
  - Keep `faulted` per binding.
  - Refuse a negative `stream_id` instead of folding it.
- **Closure tests:**
  - Unknown or retired `session_id` returns `Discarded` with no map entry.
  - 10⁶ `Data(0)` frames or 10⁴ distinct stream targets fault the binding at its limit.
  - A fault on binding B1 of call r1 leaves B2 of r1 delivering.
  - Compile and run `served/lane/tests.rs` in the `gwz_session_candidate` shape.

### P2-5: The served session's spools omit the §5.4 and §4.2 bounds

- **Location:**
  - `src/session_host/served.rs:552-557`: per-session `DiffLogRegistry` and `CommitLogOutputRegistry`, created without any limit.
  - `served/spool.rs:31-39`: `end_stream` on `diff.output` only feeds the engine. Nothing calls `diffs.release`; `grep` finds no caller.
  - `spool.rs:47-69`: `log.output` passes no byte limit. `CommitLogReadRequest` has only `cursor` and `max_records`: 128 by default, up to 1024 (`commit_log/handler.rs:19-20`).
  - `log_service.rs:368-380` and `handler.rs:274-278`: unknown or released logs are `InvalidRequest`.
  - Each commit-log spool is a `tempfile()`, so one open file descriptor each (`handler.rs:296`).
- **Violated invariants:**
  - Session design §5.4 (adopted unchanged):
    - At most 64 logs open; an open with nothing releasable is refused `transport_session_full`.
    - A log is released when its producer has sealed it and its last reader stream has ended.
    - A released log gets `operation_expired`.
    - Another session's log gets `operation_not_found`.
  - Session design §4.2 and 2.0 §3.1: each log read reply is at most 1 MiB.
- **Reproduction:**
  - A client repeats `diff` or `log` (direct calls, which take no table entry) and never ends or reads the logs. Logs and file descriptors grow without bound until the session ends.
  - The file descriptors belong to the one server process, so sibling sessions start failing with EMFILE. The refusal is not attributed to the session that caused it.
  - A `log.output` read with `max_bytes: 1048576` over commits with large messages returns up to 128 (or 1024) records whatever their size. That reply can exceed 1 MiB, and in the limit 64 MiB.
  - Row 5 depends on spool replies being bounded. Only `diff.output` is bounded.
- **Impact:** an unbounded resource reachable by a buggy client, which affects every session of the server. A reply bound the control-progress argument relies on is broken. A client sees the wrong codes when it tries to recover.
- **Correction:**
  - A served spool layer over the two registries with:
    - the 64-open cap and the release-to-make-room rule (sealed at least 30 s ago and no reader open), otherwise `transport_session_full`;
    - release on last reader end;
    - `read_bytes` applied to `log.output`, returning at least one record and splitting or refusing a single record over the limit;
    - §5.4's codes.
  - Charge the cap where `diff` and `log` register their logs, which the gate already reaches via `effect`.
- **Closure tests:**
  - The 65th `log` with 64 open and unreleasable logs returns `transport_session_full`.
  - A sealed and ended diff log is released, and a later read returns `operation_expired`.
  - A `log.output` reply over commits with 512 KiB messages stays at or under 1 MiB.
  - Another session's `log_id` returns `operation_not_found`.

### P3-1: A queued write or push is not re-checked against the detached record when it starts

- **Location:** `service/table.rs:839-845` checks `detached.running_on` only in `admit`. `schedule` (`:905-938`) starts queued W and push operations without checking.
- **Interleaving:**
  1. Session B's `commit` waits in B's queue behind B's running `fetch`.
  2. Session A closes and detaches a worker on the root.
  3. B's fetch ends, and `schedule` starts B's commit beside the detached worker.
- **Violated invariant:** 2.0 §3.2, a W or push on a detached worker's workspace is "refused, not run beside them".
- **Backstop:** the cross-process mutator lock, but only for the handlers that take it.
- **Correction:** check again in `schedule`, and settle the operation with `WorkspaceBusy`.
- **Test:** the interleaving above.

### P3-2: Refusals of malformed control calls are sent on the call lane

- **Location:** `table.rs:281-286`. A `classify` error is refused on `Lane::Call` before the lane is known. For example: an `operation.cancel` naming both targets, or an undecodable `SessionCloseRequest`.
- **Impact:** with the in-process call lane full (the row-5 situation), the refusal gets `Full`, and `flush` then closes the session. This contradicts §3: "a reply goes on the lane of the call it answers".
- **Correction:** derive the lane from the method name before decoding.

### P3-3: Cancelling a spent read, result or control call returns `operation_expired` (recorded limitation)

- **Location:** `table.rs:558-571`. Only live calls are in `others`, so a spent one falls through to `OperationExpired`. That also covers spool calls and calls refused at receipt.
- **Assessment:** session design §4.2:97 asks for `invalid_request`. The bridge treats `operation_expired` as "already finished", so a conforming bridge's recovery is unaffected; only a misbehaving client gets a weaker diagnostic.
- **Action:** acceptable as a recorded deviation, but it must be recorded in the design or plan text, not only in the evidence README.
- **Optional fix:** keep a bounded record of call kinds for answered calls.

### P3-4: Member lock manager not built: what it permits, and where it should land

- **What it permits:** fetch+fetch on one member, across sessions and within one session. `Busy` lets fetches run side by side, and nothing serializes the member step. Push+push across sessions runs as well: the push handler takes no `WorkspaceMutatorLock` (grep: only merge, create, init and archive take it).
- **Fail direction:** Git's own ref-lock refusal, i.e. the outcome two CLI processes get. That matches 2.0 §1 ("changes no outcome").
- **Gap:** within one session it is weaker than session design §5.1, which says the lock "blocks rather than refusing". A single client's two fetches can therefore have one fail spuriously.
- **Action:** a recorded limitation, not a defect, but no plan step names where it lands. Record the landing step (2.3 or 2.2c) and the within-session difference.

### P3-5: The crate's tests, including red rows 1 and 4, run in no gate

- **Location:** README limitation: "no gate … runs `cargo test -p gwz-session-host`". The remote `run_tests.py` pass (2391 tests) does not cover the 54 crate tests.
- **Impact:** rows 1 and 4 and every admission, close and lane test are protected only by a manual Mac run. I re-ran them: green.
- **Action:** add the crate to gwz-core's test runner or boundary job, or record why not.

## 2. Invariant analysis (attacks that failed, with evidence)

- **Records at receipt, admission order, and a cancel before admission:**
  - The record, token and gate are created under the table lock in `receive` (`table.rs:381-426`).
  - A cancel of a `Received` record cancels its token, and `admit` re-reads the token under the lock (`:810-821`). A call cancelled while its resolution is in flight is settled `Cancelled` and never runs.
  - Close step 2 settles a record that the admission thread is still resolving; the late `admit` sees it is not `Received` and does nothing (`:806`). That is the "does not wait for a stuck resolution" case.
  - Holds.
- **Classes within one session:**
  - `Busy::admits` (`table.rs:70-76`) gives: fetch beside fetch and push; at most one push; W alone.
  - `blocked` keeps per-workspace FIFO order (`:929-932`).
  - Running and busy counts drop only for a record that was running (`:736-743`), including spawn failure (`workers.rs:159-189`).
  - Holds.
- **Across sessions** (fetch+fetch, fetch+push, push+push, write+direct, write+write):
  - Nothing is shared except `DetachedWorkers`. The handlers are the CLI's own: gwz-cli `execute_with_backend` takes no lock outside them.
  - `WorkspaceMutatorLock` is an `flock` on a separately opened file. It refuses within one process: test `…first_race`, exactly one winner among threads (`workspace_mutator_lock.rs:240-269`). So write+write between two sessions gets "already held" (`unsupported_operation`) exactly as two CLIs do.
  - This matches 2.0 §1's "no cross-session queue", with refusal as the fail direction.
- **Detached-worker record:**
  - Shared through `HostContext::detached()`.
  - Refuses W and push and allows fetch, as the test shows.
  - It does release for unary records once the thread ends, as the test shows.
  - The remaining defects are P2-1 and P3-1.
  - A worker that reported just before revocation is still marked detached with "outcome unknown" (`workers.rs:83` against `:255`). That errs on the conservative side and is acceptable.
- **Limits and attribution:**
  - The queue, table and outstanding/control counts are per session (`table.rs:295-316, 846-855`).
  - A client that never reads results fills only its own table and receives `transport_session_full` itself. No cross-session starvation through the table.
  - The cross-session file-descriptor path is P2-5.
  - 64 cancels waiting on long operations make a 65th control call, `session.close` included, `SessionFull`. That is design-conformant (§5.1: "at most 64"), and channel drop still closes.
- **Cancel:**
  - Lookup by `call_id` or `operation_id` (`:552-611`).
  - Queued and direct-waiting records are settled under the table lock along with `schedule`, so there is no double start (§5.3).
  - The cancel's reply goes on the control lane (row 5, plus the mutation red `red-mutation-control-lane.log`).
  - The spool replies themselves are bounded only for `diff.output` (P2-5).
- **Close:**
  - Steps 1–9 are present (`workers.rs:212-308`). Repeated `session.close` calls get the same report (`table.rs:347-360`).
  - The report is keyed ordinary (`control: false`, `:366-377`; test at `service/tests.rs:753`).
  - A kill mid-close loses only the in-memory detached record, and the OS releases `flock`s with the process.
  - Revocation and cancel callbacks run while the table lock is held (`workers.rs:251-258`, `table.rs:597, 602`). Today every crossing is trivial: event append, cloning the spools. See §3.
- **Events and row 3:**
  - Appends go through the gate without taking the table lock (`events.rs:55-94`).
  - Held reads complete on append, completion, deadline or close (`events.rs:144-180`).
  - The ring keeps a reset entry.
  - A held read survives release or eviction through its `Arc<EventLog>`.
  - 1024 held reads are bounded by the outstanding limit and attributed to their own session.
  - Events reaching the client before the result reply holds for submit (row 3 test).
- **Trust direction:**
  - The refusal text names the served root and the root the client's path resolved to (`served.rs:286-299`). The client chose the server and passes the same-user peer check, so this is acceptable.
  - A method missing from the table is classed W and admitted. It then gets `unsupported_operation` from `CoreDispatch` (`dispatch.rs:192-195`) before any effect: it runs on a worker under W semantics only to be refused.
  - Path fields in requests are no more trusted than in the CLI, and the server runs as the same user.
  - Spool and event registries are per session, so one session cannot read another's (P2-5 covers only the codes).
- **Handshake seam:** `route_frame` and `receive` both check `is_open()` (`lane.rs:373`, `service.rs:673`). `open()` is one-way. Row 4 is red under the recorded mutation (`red-mutation-handshake.log`).
- **Evidence:**
  - The five rows are green on my re-run. The reds were written first (`red1-protocol.log`, `red-additive-old-schema.log`).
  - Rows 4 and 5 were shown red by mutation, which is legitimate.
  - Row 1 runs over a fake carrier in the crate, not the in-process carrier the plan names. That is recorded: the in-process adapter takes tags 16 and 17 only once 2.1 lands.
  - The digests are consistent, and the tracked patch and untracked tar match the commit.
  - Windows receipt `a` failed on the tail-check import and was fixed. Receipt `b` passed with only the 4 named host gaps.

## 3. Risks and next action

- **Next action:** remediate P2-1 to P2-5, with P2-4's lane API agreed before 2.2c consumes it. Fix P3-1, P3-2 and P3-5 as well, or record them, and record P3-3 and P3-4 with their landing step. A re-review limited to the closure tests above can then return GO.
- **Risk for 2.2c:** `CallControls::cancel()` and `revoke()` are called while the table lock is held. Once 2.2c registers cancel callbacks (the transport runtime signal) or longer `effect` crossings (a transport open through the gate), a slow callback or crossing will stall the reading thread. That would break "control frames never wait" and push close past its bound. Move those calls outside the table lock, or bound them, before 2.2c.
- **Risk for 2.3:** P2-1's lifetime depends on how long the server keeps a closed session's `Service`. 2.3 should drop it when the session ends, but the fix in P2-1 must not depend on that.
- **Risk to R5:** `pending_local_work` comes only from detached workers, since every served outcome reports `Cleanup::default()`. The R5 rule "cleanup reports name what the client still owns" depends on 2.2c feeding real cleanup into `Outcome.cleanup`.
