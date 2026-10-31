# Step 2.2c rework — STATE-AXIS REVIEW
**Review object:** plan step 2.2c (rework). A served core holds the same public `HostHandle`/`Admission`/`Operation`/`Opening`/`Exchange` types for local and carrier-backed connections. The carrier lane body moves to `TransportLaneFrame`, and the codec's `bind_rejected` is widened to the five §3.4 codes. Lane `/Volumes/projects/limbo/gwz-dev-p2-rework`.
**Baseline:** gwz-transport `a22d43cd..9d5e0866`, gwz-core `4703a4e8..05ea42b5` (see §0 about the stated `58e506ee`), root `a4ff0e15..49504e6d`, evidence `b79e563d`. **Date:** 2026-10-30
**Axis:** State: durable-state semantics and adversity. Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: NO-GO** — 1 P1, 3 P2, 4 P3. I pre-commit to GO on a revision that resolves P1-1, P2-1, P2-2 and P2-3 as specified.

## 0. Evidence base
- **Tuple.** I checked it at the start and again at the end. It was identical both times: root `49504e6d`, gwz-transport `9d5e0866`, gwz-core `05ea42b5`, gwz-core-evidence `b79e563d`.
  - gwz-transport and gwz-core are clean.
  - The only untracked files are the out-of-scope operator drafts in the root and `2026-09-22-alpha-setup-timeout/` in the evidence repo.
- **Baseline discrepancy (not a finding).** The brief names the gwz-core object as `58e506ee..05ea42b5`, but `58e506ee` does not exist in the lane's gwz-core. `05ea42b5`'s parent is `4703a4e8`. In main, `58e506ee` is a sibling child of `4703a4e8`: a CI-workflow-only row-count change (`.github/workflows/transport-candidate.yml`). The object is effectively `4703a4e8..05ea42b5`. Merging will need `58e506ee` brought along, but it touches no file of this object.
- **Read in full:**
  - `crates/gwz-connection/src/remote/{mod,lane,core,backend,stream,receiver,served}.rs`
  - the remote cases of `exchange.rs`, `operation.rs`, `host.rs` and `lib.rs`
  - `src/codec/validate.rs`
  - gwz-core `transport_host/remote_lane.rs` and the carrier diff (`mod.rs`, `stream.rs`, `stream/{inbound,state}.rs`, `wire.rs`)
  - the relevant parts of `gwz-transport/src/stream/machine.rs` and `src/mux/mod.rs` (`fail`, `cancel`, `close_result`, `disconnect`, `advance`)
  - 2.0 §§3.1–3.4, the §15 row diff, the implementer brief, and the evidence README.
- **Runs.** All used `CARGO_TARGET_DIR=/Volumes/projects/limbo/build-targets/p2-2c-review-state` and `TMPDIR=/Volumes/gwztmp`.
  - `cargo +1.96.0 test -p gwz-connection --manifest-path gwz-transport/Cargo.toml --offline` from the lane: 153 passed, 0 failed.
  - `cargo +1.96.0 test --manifest-path gwz-transport/Cargo.toml --offline --test mux`: 27 passed.
- **Reviewer probes.**
  - Setup: I exported gwz-transport `9d5e0866` with `git archive` to `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/gt`. I added one file, `crates/gwz-connection/src/remote/tests/probe.rs` (plus `mod probe;`), which uses the object's own `World` fixture.
  - Five probes, all failing on the object:
    - `probe_a_pending_wait_wakes_itself` → "5 pending polls, 5 self-wakes" (P2-1)
    - `probe_cancel_during_opening_resolves_the_wait` → "ready=false … wakes=0 … after carrier loss ready=false … operation finish ready=false" (P1-1)
    - `probe_end_call_under_a_forwarding_push_reports_its_effect` → echo "pack-bytes" crossed the client's exchange, then `failure: Some((Cancelled, None))` (P2-2)
    - `probe_a_bind_sent_after_its_binding_was_retired` → `NeverRegistered(Cancelled)`, retired `…c0.1`, then "binds sent after retire: 1", "client bindings held: 1, client active ops: 1" (P2-3)
    - `probe_bind_flood_with_a_full_lane_grows_the_receivers_outbox` → "20000 … held … sent after: 20000" (P3-1)
- **Not run.** I did not rebuild gwz-core's candidate-shape `transport_host` rows, because another reviewer shares the machine and a heavy build would slow both. For gwz-core rows I rely on the evidence logs (`raw/mac/green/final-test-core-transport-host.log`) and on reading the code. Every gwz-core claim below is from reading `remote_lane.rs` and `stream/state.rs`.

## Problem, requirement provenance, and scope
- **Requested outcome** (operator, 2026-10-29, "option one", via brief-2.2c-rework.md): one public type family for local and carrier-backed operations, plus the carrier body on `TransportLaneFrame` and the `bind_rejected` widening.
- **Fixes in scope:** the initiator-Cancel slot fix and the item-11 disposition.
- **Seams the implementer added**, judged by necessity:
  - `bind_open`, `bind_retire`, `accept_binds`: necessary. The 2.1 stream carrier routes frames only for open bindings, the core mints its bindings, and the client must admit the peer's `bind`.
  - `try_recv_transport` / `Scope::Transport`: necessary. Without them the lane would take registry frames, and `arrival()` turns a registry frame into a protocol fault that ends the carrier.
  - `bind_fault` and `Lane::faulted`: necessary for contract §6 fault scope, because the carrier drops queued frames silently.
  - `Lane::retire`: necessary, or the carrier's binding table fills with dead bindings.
  - `progress()` on `CoreBinding`/`ClientSession`: necessary for progress without application calls (contract §6).
  - Public enum changes (`Registration::Pending`; new `TerminalReason`/`OpenFailure`/`AdmissionError` variants; `StreamError` `#[non_exhaustive]` with new variants; `Eq` dropped because `ErrorCode` is carried): these follow from the shared-type requirement and the §15 row.
- **Files touched outside the brief's list:** `operation.rs`, `binding.rs`, `instance.rs` (gwz-connection), and gwz-core `.github/gwz-transport.commit` plus the allowlist pin. The first three are forced by the new variants and fields; the pins are forced by the dependency bump.
- **Scope conclusion:** the deliverables stay inside the authorized boundary. No 1.3b/1.3c/1.4/1.5/1.6 or 2.2b `Service` file changed.

## 1. Findings

### [P1-1] Cancelling an operation while an opening is in flight hangs `Opening::wait` forever and leaks the binding
- **Location:**
  - `remote/core.rs` `Binding::cancel_all`: the `Stage::Opening => exch.dropped = true` arm.
  - `remote/core.rs` `settle`: `self.exchanges.retain(|_, e| !(e.dropped && e.stage == Stage::Ended))`.
  - `remote/stream.rs` `RemoteOpenWait::poll_open`: `let entry = state.bindings.get_mut(&serial)?; let exch = entry.exchanges.get_mut(&token)?;` returns `None` and therefore `Poll::Pending` without registering a waker.
- **Invariants violated:**
  - Every pending wait resolves on a terminal event.
  - Cancellation ends an opening with `Cancelled`, as the local `LocalOpenWait` does.
  - Carrier loss resolves every open wait (§3.4 "open exchanges fail carrier_lost").
- **Reproduction:**
  1. Register an operation and let `bound` arrive.
  2. Call `op.open(spec).wait()`.
  3. Pump the core once, so the `Open` leaves.
  4. Call `op.cancel_handle().cancel()` before `opened` arrives.
  5. `cancel_all` marks the opening `dropped`, a flag that means "the handle is gone, reclaim at end", although the wait still holds it.
  6. In the same pump, when `opened` (or `open_failed`) arrives:
     - `opened` path: `drain_actions` builds and cancels the machine, the machine's `close_result()` is immediately `Err(Cancelled)` (`machine.rs` `fail`), `settle` marks the exchange `Ended`, and `retain` deletes it.
     - `open_failed` path: the exchange is `Ended` and deleted the same way.
  7. Every later `poll_open` returns `Pending` with no waker.
  - Probe `probe_cancel_during_opening_resolves_the_wait`: the wait is still pending after settle, after `carrier.end()`, and after more pumps; zero wakes; `op.finish()` never resolves.
- **Impact:**
  - Stage `Opening` spans the client's whole connect, including SSH login (seconds), so a cancel during connect — the most common cancel — hangs the core worker permanently. Even carrier loss does not release it.
  - `live > 0` forever, so the binding never retires. The core slot (`active`) and the `OperationEnd` receipt leak for the session's life.
  - Under 2.0 §3.2, a worker that only the close bound can detach poisons later W/push on its workspace.
- **Remedy:**
  - In `cancel_all`, for `Stage::Opening`, set `outcome = Some(Err(OpenFailure::Cancelled))` at once, as the local path does. Keep a separate "cancel when opened" flag, so a late `opened` is still answered with `cancel`. Do not use `dropped`, which must mean only "no handle".
  - In `poll_open`, a missing binding or exchange record must yield `Ready(Err(...))`, never `Pending`.
  - Audit `RemoteFinish::poll` for the same missing-record case: it pushes a waker and waits forever on a record `settle` may have deleted.
- **Closure test:** port the probe as a row.
  - Assert the wait is `Ready(Err(Cancelled))` both when the client answers `opened` and when it answers `open_failed`.
  - Assert `op.finish()` resolves.
  - Add a second row with `carrier.end()` instead of a client answer.

### [P2-1] Every pending carrier-backed wait wakes itself inside its own poll (busy spin)
- **Location:** `remote/backend.rs` `Shared::with`.
  - It runs `f`, then `state.wake_all()`, which drains `state.waiters` and every queued admission's waker.
  - The waiting futures push their waker inside `f`: `RemoteOp::poll_registered` (`state.waiters.push`), `RemoteOpenWait::poll_open`, `RemoteFinish::poll`, and `RemoteAcquire::poll` (`waiting.waker = Some(waker)`).
- **Invariant violated:** a `Pending` future is woken only by a state change. Contract §6 progress comes from the driver, not from polling.
- **Reproduction:** probe `probe_a_pending_wait_wakes_itself`. Five `Pending` polls of `op.registered()` and of an `OpenWait` on a pending operation produce five wakes each, with no lane event.
- **Impact:**
  - On a real executor, each waiting task — acquire (up to 30 s), registered (up to 5 s), opening (connect time), exchange finish (push processing, up to `interaction_ms`) — is re-polled continuously.
  - Each poll runs a full pump: carrier lock, recv loop, `faulted()` per binding, `endpoint_path_base()` under the state lock, flush. That is one CPU core per waiting task, contending with the carrier's reader thread.
  - The rows pass only because the fixture pumps by hand.
- **Remedy:**
  - Collect the wakers to wake before running `f`, or let `f` register waiters that only the next state-changing pump wakes. Equivalently, take `waiters` in `pump` only when something moved.
  - When fixing this, make `done_registration` poll the `Cell` with the real waker (see P3-3); otherwise the publication window becomes a lost wakeup bounded only by the driver's 100 ms tick.
- **Closure test:** the probe as a row: N `Pending` polls with no lane event give 0 wakes, and a later `bound` or `opened` gives at least one.

### [P2-2] The client reports `Effect::None` for a push whose bytes it already forwarded when the call ends or the binding faults
- **Location:** `remote/served.rs` `Served::terminate`, used by `end_call`, `fault` and `lane_fault`. For every `Ex::Open` stream it sends `Failed { code, effect: Effect::None }` regardless of `run.service` and `run.forwarded`. Contrast `cancel_stream` and `fail()`, which correctly send `Effect::Possible` when `service == ReceivePackExchange && forwarded > 0`.
- **Invariant violated:** fail-closed effects — a push whose bytes may have left must never be reported `Effect::None` (§3.4 and the §15 row on revocation).
- **Reproduction:** probe `probe_end_call_under_a_forwarding_push_reports_its_effect`.
  1. Open a push-class binding.
  2. Write `pack-bytes`; they echo back through the client's owner exchange, proving `forwarded > 0`.
  3. Call `receiver.end_call("call-1")`.
  4. The core's `ExchangeEnd.failure` is `Some((Cancelled, None))`.
- **Impact:** the core's gwz-git push sees a cancelled push with no effect and may report "nothing happened" or retry, when the remote may have applied ref updates. The same under-report happens on a client-side fault of a binding mid-push.
- **Remedy:** in `terminate`, compute the effect per running exchange exactly as `cancel_stream` does, and reuse one helper for all three sites.
- **Closure test:** the probe as a row for `end_call`, and a second row for `carrier.fault_at_client(session)` mid-push. Both must report `(code, Effect::Possible)`; a fetch exchange keeps `None`.

### [P2-3] A `bind` queued in the core outbox is sent after its binding was retired; the stream carrier re-opens and leaks the binding
- **Location:** `remote/core.rs`.
  - `abort_all` and `cancel_all`/`finish` on a `Pending` binding change state but leave that binding's messages in the shared `State::outbox`.
  - `retire_finished` then calls `lane.retire(session)` while the stale `bind` is still queued.
  - gwz-core `transport_host/remote_lane.rs` `CarrierLane::send` calls `carrier.bind_open(session)` for any outgoing `Bind`. Retire removed the record, so the open succeeds, and nothing ever retires it again.
- **Invariant violated:**
  - A retired binding makes no state (§3.3 "Scope").
  - A terminal `NeverRegistered` means nothing more happens for that binding.
- **Reproduction:** probe `probe_a_bind_sent_after_its_binding_was_retired`.
  1. Set lane capacity to 0 (full lane).
  2. Register; the `bind` is now in the outbox.
  3. Call `op.finish()`, which gives `NeverRegistered(Cancelled)`, and the core retires session `…c0.1` on its lane.
  4. Raise capacity: the `bind` goes out anyway.
  5. The client admits and registers an operation (client binding 1, active ops 1) for an operation the core has already ended.
  - Same path in production: the mux bootstrap bound is 5 s from `begin`, so under a lane saturated for more than 5 s (a large push), a bind still in the outbox times out, the app finishes the `TimedOut` operation, and the stale bind is sent.
- **Impact:**
  - Each occurrence leaks one core-end carrier binding record, up to `max_bindings` (1024), plus an orphan client registration held until the call ends.
  - At the cap, `bind_open` returns `TooMany`, which `CarrierLane::send` maps to `LaneSend::Full`. `flush` stops at the head of the FIFO outbox, so one stuck `bind` blocks every other binding's data, window and cancel frames: the session's transport lane wedges.
- **Remedy:**
  - When a binding is aborted, cancelled while pending, or retired, purge its unsent messages from `State::outbox` (`retain(|m| m.1.session_id != session)`). An unsent `bind` then never leaves.
  - Defence in depth: `CarrierLane` should refuse to re-open a session it has retired.
- **Closure test:** the probe as a row (no `Bind` after the core's retire of that session), plus a gwz-core `remote_lane` row over the stream carrier asserting the binding count returns to zero.

### [P3-1] A core that sends binds but does not read grows the client receiver's outbox without bound
- **Location:** `remote/receiver.rs` `start_bind`'s `reject` and `served.rs` `admitted`'s `reject` push to `RState::outbox` without checking `OUTBOX_ROOM`. `pump_locked` keeps calling `recv()` while `send` returns `Full`.
- **Invariant violated:** 2.0 §3.3 "each end's receiving side keeps … state bounded in quantities the sender controls"; a hostile core must not cost the client unbounded memory.
- **Reproduction:** probe `probe_bind_flood_with_a_full_lane_grows_the_receivers_outbox`. 20,000 injected binds against a full lane are all held, and 20,000 rejections are sent later. Each bind opens a carrier binding, which the receiver retires at once, so `max_bindings` does not bound the flood. A core that reads one byte occasionally also avoids the 60 s stall bound.
- **Impact:** memory growth equal to the rate of the hostile core's binds.
- **Remedy:** stop taking from the lane while `outbox.len() >= OUTBOX_ROOM` (backpressure), or treat the overrun as a session-class protocol error.
- **Closure test:** the probe asserting at most `OUTBOX_ROOM` messages held.

### [P3-2] `snapshot()` on a carrier-backed handle reports `phase: Open` even after carrier loss (lane-owner question)
- **Location:** `host.rs` `HostSnapshot::refusal` hardcodes `phase: HostPhase::Open` and zero counts, with only `refused()` to tell it apart.
- **Invariant violated:** a public field must not misstate state. After `lose()`, `check_open()` is `HostClosing` and `admit` is `CarrierLost`, yet `snapshot().phase == Open`.
- **Impact:** a caller that reads `phase` or the counts without first checking `refused()` gets a plausible but false value.
- **My answer to the owner's question:** a refusal should be unmissable. Prefer `snapshot()` returning a type the caller must match (an enum or `Result`). If the struct is kept for local compatibility, at least make `phase` truthful (`Closed` once the carrier has ended) and document that every field except `phase`/`namespace` is meaningless when `refused()`.
- **Closure test:** after `carrier.end()`, the snapshot's `phase` is not `Open`, or the refusal is a distinct variant.

### [P3-3] `Operation::registration()` can read `Pending` after `Registered` (non-monotonic)
- **Location:** `remote/backend.rs`.
  - `retire_finished` removes the binding under the lock, but `Shared::with` sets the receipt `Cell` only after releasing it.
  - `RemoteOp::registration` reads `bindings.get(serial)` as `None` and the `Cell` as unset, and reports `Pending`.
- **Interleaving:** thread A is inside `with` and retires the binding; between A's lock release and `cell.set`, thread B calls `op.registration()`.
- **Impact:** an observer sees `Registered`, then `Pending`, then `Registered`.
- **Remedy:** keep the final `Registration` in `RemoteOp` (or set the cell inside the lock), and never answer `Pending` once the binding is gone.
- **Closure test:** a unit test of `registration()` after removal with the cell unset.

### [P3-4] Direct `#[cfg(test)]` on new declarations
- **Location:** `remote/receiver.rs`, the two `#[cfg(test)]` attributes on `Receiver::discarded` and `Receiver::bindings` at the file's end.
- **Rule violated:** AGENTS.md / CLAUDE.md "Explicit scope for control flow and conditional compilation". New code must use an enclosing `cfg_if!` boundary, as the rest of the object does.
- **Remedy:** move both into a `cfg_if::cfg_if! { if #[cfg(test)] { impl Receiver { … } } }` block.
- **Closure test:** the repo's syntax-aware cfg check, if one exists; otherwise `grep -n '#\[cfg(test)\]' crates/gwz-connection/src/remote` returns only `if #[cfg(test)]` lines.

## 2. Invariant analysis (attacks that failed are part of the result)
- **Registration lifecycle.**
  - `Pending → Registered` on mux `Ready`.
  - `→ NeverRegistered(ClientRefused(code))` on `bind_rejected`.
  - `→ NeverRegistered(TimedOut)` on the mux bootstrap bound (5 s).
  - `→ NeverRegistered(CarrierLost)` on `lose()`.
  - `→ NeverRegistered(Cancelled)` on cancel or finish while pending.
  - `abort_all` changes `reg` only from `Pending`, so `Registered` never reverts and `Never` is never overwritten. No double terminal found.
  - Carrier loss between `bind` and `bind_rejected` gives `CarrierLost`, and the later frame is never processed (`lost`).
  - A late `bound` after cancel is dropped by `receive` (`faulted`) or by the carrier (retired).
  - A duplicate `request_id`: core-side `DuplicateOperation` while live; client-side `seen_set` gives `bind_rejected(invalid_request)`.
  - Opens are issued only once `Registered`, so `NeverRegistered` never coexists with a sent `Open`: zero effects holds.
  - One mislabel, not raised as a finding: a pending binding faulted by the core's own carrier, or by a misnamed frame, ends `ClientRefused(Protocol)`. The `TerminalReason` docs cover it ("or broke the bind's rules"); the §15 row lists only the three `bind_rejected` codes. Worth one clause in §15.
- **Revocation.**
  - `lose()` → `revoke()` → `Registered` bindings get `revoked = Some(CarrierLost)`.
  - `effect_on_loss` is `Possible` for a `ReceivePackExchange` once `open_sent`. `open_sent` is set when the `Open` enters the core outbox, before it reaches the lane, so it over-reports, which is the allowed direction.
  - Fetch and advertisement exchanges report `None`, which is correct.
  - Opens queued on an already-faulted binding report `None`, correctly, since nothing was sent.
  - Mux route deadlines report `Possible` for openings (conservative).
  - Under-reporting was found only at the client (P2-2). The core passes through the peer's effect for `PeerFailed`.
- **Stream slot.** The client's `cancel_stream` removes the exchange and answers exactly one terminal. If the exchange's `closed` already went into the mux, the record is gone and nothing more is sent; if `closed` was still in the machine, the `Running` is dropped with it, so only `Failed` goes. No double terminal from the client. On the core, local `cancel()` ends the machine at once, and a racing `closed` is ignored by the failed machine. The rows show the slot returns, and the mux's route deadline (`cleanup_timeout`) bounds a missing answer.
- **Binding release gap (item 11).** Not unbounded.
  - The receiver allows exactly one `bind` per `request_id` (`seen_set` plus `Invocations::take` once), and `end_call` releases it. Client bindings are therefore at most one per call, bounded by the session's outstanding calls and by `max_bindings` (1024).
  - Cost 1: client owner slots (`active_slots` default 8) are held for the call's life, not the operation's.
  - Cost 2: a core that re-admits the same `request_id` after finishing is always refused `invalid_request`.
  - Cost 3: with the client's `admission_bound_ms` (5 s) equal to the core's bootstrap bound (5 s), a client that gets its slot at about 5 s registers after the core has timed out. That leaves an orphan registration until call end, which shrinks the client's slots further under load.
  - This is design-consistent with §3.4 and recorded in §15. I recommend recording the 5 s/5 s race and the one-operation-per-call consequence beside it.
- **`accept_binds`.**
  - A candidate bind is decoded once (`decode_envelope`) and only for a control-tag frame naming an unopened binding.
  - At the binding limit the bind is silently discarded, with no `bind_rejected(capacity)`, so the core waits out its 5 s bound and reports `TimedOut` rather than capacity. That is diagnosability only, and bounded.
  - A bind naming an open binding goes to that binding's mux and faults it alone.
  - Receiver-side growth: see P3-1.
- **`try_recv_transport` vs registry frames.** `Scope::Transport` takes only binding entries (the minimum first sequence number across bindings preserves global arrival order), and `Scope::Registry` takes only registry entries. No cross-consumption or reordering. A transport consumer that does not drain holds back only its own bindings. Registry wakers stay separate (`register_registry`), and the lane uses the end's own waker. No production caller of the registry path exists yet.
- **`bind_fault`, `Lane::faulted`, `Lane::retire`.**
  - Faulted records keep their fault until retired.
  - `retire_binding` drops inbound entries only, so outbound terminals queued before retire still go.
  - Frames for a retired binding are discarded with no state.
  - The one ordering defect is the outbound stale `bind` (P2-3).
- **Drivers.**
  - The core driver registers the lane waker after its pump. A message arriving in that window wakes the previous poll's waker for the same task. On the first poll the 100 ms tick covers it.
  - The receiver driver ticks every 5 ms while any served binding lives. Because of item 11 that is for the whole call (about 200 pumps/s per session), not just the exchange.
  - No permanent stall found: the response direction depends on the tick (latency up to 5 ms per pump, at most 16 machine messages per pump).
  - Both `spawn` results are ignored (`let _ =`). If `Progress::spawn` fails, the receiver is silently inert and every core bind times out. That is a risk worth a named error.
- **Locks.**
  - No lock is held across an `.await`.
  - Core lock order is remote `State` → carrier → `Calls` registry (`endpoint_path_base` is called per binding on every pump under the state lock).
  - Client lock order is `RState` → carrier and owner host locks; neither carrier nor owner calls back into `RState`.
  - No deadlock exists today. It is latent for step 2.3: if the 2.2b `Service` calls `OperationCancel::cancel` while holding its registry lock, it inverts the order against `endpoint_path_base`.
- **Codec widening.** It admits exactly the five codes, with `Effect::None` and no facts. Local paths never emit the new codes, and the 27 mux rows pass.

## 3. Risks and next action
- **Next action.** Remediate P1-1, P2-1, P2-2 and P2-3, each red-first with the closure rows above (the probes in `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/gt/crates/gwz-connection/src/remote/tests/probe.rs` can be ported directly). Then send it to one re-review.
  - P2-1 and P3-3 interact: fix them together.
  - P3s are bounded and can be scheduled with the remediation or recorded.
- **Risk.** The test fixture pumps by hand, which is how P2-1 and P1-1 stayed hidden. At least one row should drive the core and client through real wakers (a counting waker, or the `ManualProgress` drivers only) with no explicit `settle()`.
- **Risk.** No production caller exists yet (dead_code allowed until 2.3/3.1). Before step 2.3 wires the `Service`:
  - capture `endpoint_path_base` once at `register`, rather than under the state lock on every pump;
  - surface `serve_bindings`/`spawn_driver` failures.

## 4. Out-of-scope observations (follow-up proposals; not findings)
- **Merge prerequisite.** gwz-core `58e506ee` (CI row count, in main) is not an ancestor of the lane's `05ea42b5`; the lane-to-main merge will need it. Confirmed fact, CI-only, does not block the object.
- **Design question for 2.0 §3.4.** One binding per `request_id`, and no wire kind that releases it, make client slot occupancy call-scoped. If a served call ever needs a second operation (a different route), §3.4 needs either a release kind or a multi-bind rule. This is a limitation, not a defect, and needs an operator decision.
