# Step 2.2c rework — CODE-AXIS REVIEW

**Review object:** plan step 2.2c (rework). It covers gwz-transport `a22d43cd..9d5e0866` (gwz-connection shared types and the `remote/` module, `src/codec/validate.rs`, `tests/mux.rs`), gwz-core `05ea42b5` (carrier body on `TransportLaneFrame`, carrier seams, `transport_host/remote_lane.rs`), root `a4ff0e15..49504e6d` (the contract §15 row) and gwz-core-evidence `b79e563d` (run `2026-10-30-p2-2c-b`). Lane: `/Volumes/projects/limbo/gwz-dev-p2-rework`.
**Baseline:** gwz-transport `a22d43cd`; gwz-core `4703a4e8` (see §0); root `a4ff0e15`. **Date:** 2026-10-30
**Axis:** Code (architecture, interfaces, call graphs, compatibility reality). Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: NO-GO.** 0 P0, 0 P1, 2 P2, 6 P3. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified.

## 0. Evidence base

- **Tuple.** I checked it at the start and again at the end. It was identical both times: root `49504e6d`, gwz-transport `9d5e0866`, gwz-core `05ea42b5`, gwz-core-evidence `b79e563d`. Every member had no tracked changes. Untracked files were present only where the brief says they would be.
- **Baseline discrepancy (not a stop condition).** The brief names the gwz-core object as `58e506ee..05ea42b5`, but `58e506ee` does not exist in the lane's gwz-core. The parent of `05ea42b5` is `4703a4e8`. `58e506ee` exists only on main, and it changes only `.github/workflows/transport-candidate.yml` (the row count). The diff I reviewed is therefore `4703a4e8..05ea42b5`. Landing this commit needs a merge with `58e506ee`, which touches no file in the object.
- **Read in full:**
  - gwz-connection: `remote/{mod,lane,backend,core,stream,receiver,served,table}.rs`, plus the diffs of `admission.rs`, `exchange.rs`, `host.rs`, `operation.rs`, `lib.rs`, `binding.rs` and `instance.rs`.
  - gwz-transport: `validate.rs`, the `tests/mux.rs` diff, the related README passages, and the mux's `cancel`/`finish`/`advance`/`disconnect`.
  - gwz-core: the diffs of `carrier/{wire,mod,stream}.rs`, `stream/{inbound,state}.rs`, the new rows, all of `remote_lane.rs`, and 2.2b's `session_host/served/lane.rs`.
  - Documents: GwzSplitCommandChannel §3.1–3.4 (including the 2026-10-27 amendment), the plan's step 2.2c and §5 file ownership, contract §4, §5 and the new §15 row, both briefs, and the evidence README.
- **Runs.** All used `CARGO_TARGET_DIR=/Volumes/projects/limbo/build-targets/p2-2c-review-code` and `TMPDIR=/Volumes/gwztmp`, with no network.
  - `cargo +1.96.0 test -p gwz-connection --offline` (lane): 153 passed, 65 of them in `remote::`.
  - `cargo +1.96.0 test -p gwz-transport --offline` (lane): every target passed.
  - Candidate tree, prepared outside the workspace with `gwz-core/tests/transport_backend/prepare.py` at `/Volumes/projects/limbo/build-targets/p2-2c-review-code-candidate`, with `RUSTFLAGS='--cfg gwz_transport_candidate'`: `cargo +1.96.0 test --lib -- transport_host::carrier transport_host::remote_lane session_host::served::lane` gave 106 passed.
  - **Reproductions.** These ran on a `git archive` copy of gwz-transport `9d5e0866` at `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/repro`, with one added test file `remote/tests/review_repro.rs`. Nothing in the lane was written. The rows and their output are quoted under P2-1 and P2-2.

## Problem, requirement provenance, and scope

**Requested outcome.** This comes from the operator's decision of 2026-10-29 and 2.0 §3.4. A served core must hold the crate's own `HostHandle`, `Admission`, `Operation`, `Opening` and `Exchange` whether the connection is local or carrier-backed. The brief adds three things in scope:
- the carrier lane body moves onto `TransportLaneFrame`;
- `bind_rejected` is widened to the §3.4 codes;
- the old lane's item 10 (cancel retires the slot) and item 11 (what releases a client binding) are fixed.

**Scope by item:**

| Item | Necessity and authorization |
| --- | --- |
| `Carrier::bind_open`, `bind_retire`, `accept_binds` | Necessary. The stream carrier routes frames only for open bindings (2.1 amendment "Routing by request"). The core mints bindings, and the client must admit a `bind` it has not opened. They sit in carrier files (trait in `mod.rs`, router in `stream/{state,inbound}.rs`), which are inside "carrier wire/router files". |
| `try_recv_transport`, `Scope::Transport` | Necessary. `try_recv` on a stream end ends the carrier when it takes a registry frame (evidence red log `red-carrier-transport-take.log`). Authorized files. |
| `bind_fault` | Necessary. The carrier drops a faulted binding's queue, so a binding learns of its fault only through this call. Authorized. |
| `Lane::retire`/`faulted`; `progress()` on `CoreBinding`/`ClientSession` | Necessary for driver-led progress and the fault scope. Inside `remote/`. |
| `transport_host/mod.rs` (+3 lines, `mod remote_lane;`) | Plan §5 assigns this file to the trunk. The edit is the minimum needed to compile the module the brief names, so I judge it necessary. Recorded here; not a finding. |
| `.github/gwz-transport.commit` and the allowlist's `reconciled_commit` | Required by gwz-core's bump rule. Both point at `9d5e0866`, which is not yet pushed. That is the known upstream-first order. |
| New variants on `AdmissionError`, `TerminalReason`, `Registration`, `OpenError`, `OpenFailure`, `StreamError` | Necessary to report the deferred commit and carrier outcomes without a duplicate type family. |
| `Eq` removed from `TerminalReason` and `Registration` | Forced by `ClientRefused(ErrorCode)`. gwz-transport's `ErrorCode` derives only `PartialEq` (`src/protocol.rs:241`), and `protocol.rs` is outside the brief's ownership. Grepping the whole lane (gwz-core, gwz-py, gwz-cli, gwz-git) finds no consumer that needs `Eq`, no hash keys and no exhaustive match. |
| `StreamError #[non_exhaustive]` | Not necessary for the outcome. It is a design choice and harmless: the only consumers are gwz-core tests using `matches!`. It is unrecorded in the contract (P3-1). |
| Old-lane item 10 (cancel retires slot) | Fixed in `served.rs:356-378` by the endpoint answering the terminal. The row exercises 12 rounds against 4 slots. |
| Old-lane item 11 (client binding release) | Reported as a §3.3 gap (no kind exists) and pinned by a row. This conforms to the brief. |
| `remote_lane.rs` `#[allow(dead_code, reason = "first production caller: plan steps 2.3 and 3.1")]` | Exactly the authorized form, placed on the `mod` item. |

No 1.3b/1.3c, 1.4, 1.5 or 1.6 file is touched. 2.2b's `Service` is unchanged: `session_host/` has no diff. `FrameSink`, `FrameRouter` and `FrameKey` are unchanged: no `crates/` diff in gwz-core.

## 1. Findings

### [P2-1] Cancelling an operation while an `open` is unanswered leaves `OpenWait` pending forever, with no waker, even after carrier loss

- **Location.** Three pieces combine:
  - `gwz-connection/src/remote/core.rs:420`: `cancel_all` sets `exch.dropped = true` for `Stage::Opening`. This reuses the "handle is gone" flag.
  - `core.rs:603-611` and `731-733`: when `opened` arrives, the machine is cancelled. `close_result()` is then immediately `Err(Cancelled)` (`gwz-transport/src/stream/machine.rs:269-287`). `settle` ends the exchange and reclaims it because it is `dropped && Ended`, all in the same pump.
  - `remote/stream.rs:97-113`: `poll_open` does `entry.exchanges.get_mut(&token)?`. A missing exchange makes the closure return `None` before the waker is pushed, and that maps to `Poll::Pending`. Meanwhile `is_ready()` (`stream.rs:79-89`, `is_none_or`) returns `true`.
- **Invariant violated.**
  - Contract §5: `OperationCancel::cancel` "wake[s] its waiters". The local behaviour, asserted at `gwz-core/src/transport_host/connection_host/tests.rs:564`, is that a cancelled opening resolves to `OpenFailure::Cancelled`.
  - `OpenWait::is_ready()` says "the next poll is ready".
  - 2.0 §3.4: carrier loss fails open exchanges with `carrier_lost`.
- **Reproduction.** I ran this row against the committed code:
  ```
  op = bound(&w,"call-1"); wait = op.open(exchange_spec()).wait(); poll → Pending
  w.remote.pump()                // the open is on the lane, unanswered
  op.cancel_handle().cancel()    // Cancelled
  w.settle()                     // `opened` arrives; exchange cancelled, ended, reclaimed
  → "after cancel+settle: is_ready=true poll=Pending"
  w.carrier.end(); w.settle()
  → "after carrier end: poll=Pending"
  ```
  The case is not narrow. `RemoteOp::open` runs inside `Shared::with`, which pumps after `f`, so a registered operation's exchange moves from `Queued` to `Opening` on the `open()` call itself. Almost every cancel of an in-flight open therefore lands in `Stage::Opening`. My second probe (`review_cancel_of_a_registered_operation…`) shows the open already past `Queued` when `cancel()` runs.
- **Impact.**
  - A core worker awaiting `opening.wait()` hangs for good when its operation is cancelled. This is exactly the `operation.cancel` path 2.0 §3.2/§5.3 depends on.
  - The `Live` the wait holds keeps `entry.live > 0`, so the binding never retires. Its core-side slot (`state.active`) and its carrier binding leak until the future is dropped.
  - Carrier loss does not resolve the wait either.
  - The plan's red rows do not cover this case.
- **Required correction.** Do not overload `dropped` for cancel-while-opening:
  - Add a distinct flag (for example `cancel_on_open`) that cancels the machine when `opened` arrives without making the exchange reclaimable while its handle lives. In that case set `outcome = Some(Err(OpenFailure::Cancelled))` rather than `Ok`.
  - Separately, make `poll_open` and `RemoteFinish::poll` treat a missing binding or exchange as a terminal answer (`Err(OpenFailure::Cancelled)` / a discarded `ExchangeEnd`), never as `Pending` without a waker. That keeps `is_ready()` and `poll` consistent.
- **Closure test.** Add a row to `remote/tests/shared_rows.rs`: bind, open, pump once, cancel, settle. Assert that `wait` resolves to `Err(OpenFailure::Cancelled)` and that `is_ready()` matches the poll. Then assert that the binding retires (`w.remote.bindings() == 0` after `op.finish()`). Add a variant that ends the carrier instead and expects `OpenFailure::CarrierLost`.

### [P2-2] Cancelling or finishing a pending operation reports its queued openings as `OpenFailure::Fault(Cancelled)`, not `OpenFailure::Cancelled`

- **Location.**
  - `remote/core.rs:371-376`: `abort_all` maps every non-revoked code to `OpenFailure::Fault(code)`.
  - The paths that reach it: `core.rs:404-407` (`cancel_all` on `Reg::Pending`), `backend.rs:417-421` (`RemoteOp::finish` on a pending binding), and the `queue_exchange` faulted path at `core.rs:446-452`.
- **Invariant violated.** Parity of the one type family (2.0 §3.4, brief requirement 1: "callers cannot tell local from remote").
  - A local cancel gives `OpenFailure::Cancelled`.
  - A registered remote cancel of a queued exchange also gives `OpenFailure::Cancelled` (`core.rs:411-419`).
  - `OpenFailure::Fault` is documented as "the binding broke a rule of the transport … or the table refused the spec" (`operation.rs`). A cancel is neither.
- **Reproduction** (my row `review_cancel_of_a_pending_operation…`): admit, acquire, register (pending), `open(exchange_spec()).wait()`, pump, `cancel()`. The output is `registration=NeverRegistered(Cancelled) wait=Ready(Err(Fault(Cancelled)))`.
- **Impact.** The same user action yields different `OpenFailure` variants depending on whether `bound` had arrived. A consumer (gwz-git step 5.4) that treats `Fault` as a transport protocol fault will report a user cancel as a protocol error and may classify it as a defect rather than a cancellation. This is a diagnosability and parity defect on the shared public type.
- **Required correction.** In `abort_all`, map `ErrorCode::Cancelled` to `OpenFailure::Cancelled`. Keep `Fault(code)` only for rule breaches (`Protocol`, and `InvalidRequest` from the table).
- **Closure test.** For both pending and registered operations, assert `Err(OpenFailure::Cancelled)` after `cancel()` and after `finish()` of a pending operation with a queued opening.

### [P3-1] The contract §15 row does not state the implemented public surface exactly

- **Location.** Root `dev-docs/GwzConnectionApiContract.md:881`.
- **Invariant violated.** The contract "controls these points on acceptance". The row is the only record of the 2.2c surface change.
- **Gaps against the code:**
  - **`ClientRefused` codes.** The row lists them as `capacity`, `invalid_request`, `cancelled`. The code also produces:
    - `ClientRefused(Protocol)`, for a binding the carrier faulted or a message under another call's name (`core.rs:186-187`, `346-351`);
    - `ClientRefused(UnsupportedVersion | UnsupportedOperation)`, from the mux's `bootstrap_failure()` (`core.rs:480-488`).
  - **Unrecorded public changes:**
    - `AdmissionError::{UnknownCall, DuplicateOperation, CarrierLost}`;
    - `OpenError::Refused(remote::TableRefusal)`;
    - `OpenFailure::{Refused, CarrierLost{effect}, Fault}`;
    - `StreamError::{CarrierLost, Failed, Closed, Protocol}` and its new `#[non_exhaustive]`;
    - the new public fields `ExchangeEnd::failure` and `OperationEnd::revoked` (both structs are exhaustive, so outside literals break);
    - `Eq` removed from `TerminalReason`/`Registration`;
    - `Registered`, `AdmitInput`, `OperationRef`;
    - the `remote` module's public ports: `Lane`, `CoreBinding`, `ClientSession`, `LaneSend`, `LaneEnded`, `Receiver`, `serve_bindings`, `Invocation(s)`, `NamedKey`, `RefusalNote`, `AllowedSet`, `Allowance`, `Canonical`, `CanonError`, `Refusal`, `ServiceClass`, `admit_service`, `RemoteConfig`, `handle_with`.
  - **Outcomes the row does not mention.** A cancel or `finish()` of a pending operation ends it `NeverRegistered(Cancelled)`, and the bootstrap bound ends it `NeverRegistered(TimedOut)`. That is, the registration outcome can now be decided after `register`, by events that §5's table does not list.
  - **Vocabulary.** The row's "third revocation outcome (amends §5)" borrows 2.0's phrase. §5 has "two legal race outcomes" (register vs close), not revocation outcomes. The row should say which sentences of §5 (lines 239-240 and 247-251) it amends. On substance it contradicts no other section.
- **Impact.** The next consumer (2.3, 5.4) gets an API authority that does not describe the variants it must handle.
- **Remedy.** Extend the row with the full variant and field list, the extra `ClientRefused` codes (or narrow the code to the documented three), the pending-cancel and timeout outcomes, the `remote` module's public ports, and the exact §5 sentences amended.
- **Closure.** A doc-only revision. The lane owner checks the row against `lib.rs`'s `pub use` list and `remote/mod.rs`.

### [P3-2] `OperationRef` accepts at most 96 bytes, but a lane request name may be 128 bytes and `RequestMeta.request_id` is unbounded

- **Location.**
  - `gwz-connection/src/operation.rs:375-383` (`len() > 96`).
  - The carrier's `request_name_ok` and `MAX_REQUEST_NAME = 128` (`gwz-core/src/transport_host/carrier/wire.rs`).
  - gwz-core does not bound `RequestMeta.request_id` anywhere (grep finds no length check).
- **Invariant violated.** 2.0 §3.4: `admit(op_ref)` carries "the call's `request_id`". Any `request_id` the lane can carry should be admissible.
- **Reproduction.** A client call with a 100-byte `request_id` is a valid lane name, but `OperationRef::new` fails with `InvalidInput::Operation`. The core cannot run that call's network operation, and the error names neither cause.
- **Impact.** A bounded robustness and diagnosability gap, triggered by a client-chosen value.
- **Remedy.** Use the lane's 128-byte name rule in `OperationRef::new`, or bound `request_id` at call receipt (2.2b/2.3) and cite that bound.
- **Closure test.** A row admitting a 128-byte `request_id` over the remote handle.

### [P3-3] Widening `bind_rejection` lets `bind_rejected(capacity)` carry a `CapacityDetail`, and the gwz-transport README now says the opposite

- **Location.** `gwz-transport/src/codec/validate.rs:287-302` and `:108-123`. `failure_detail::validate` runs on `bind_rejected` too, and accepts a capacity detail now that `Capacity` is admitted. The stale text is at `gwz-transport/README.md:379-381` ("a `BindRejected` already admits only two other codes").
- **Invariant violated.** 2.0 §3.4 defines `bind_rejected` as code plus `Effect::None` only. The README is the protocol's reader-facing statement.
- **Reproduction.** An envelope `BindRejected{code: Capacity, effect: None, detail: CapacityDetail{cause: Local}}` passes `codec::validate` after this change; before it, the envelope was refused.
- **Impact.** Small. The receiver never sends a detail and the core reads only `code`. But the wire now admits a shape no clause defines, and the README misstates the codec.
- **Remedy.** Do both:
  - Refuse a capacity detail in `bind_rejection`. This file is in ownership, and the change keeps the admitted shape exactly §3.4's.
  - Correct README:381 (five codes, no detail) in the same wire-change commit, or record it as a follow-up if the README is held to be outside ownership.
- **Closure test.** Add a `tests/mux.rs` row: `bind_rejected(capacity)` with a `CapacityDetail` is `InvalidMessage`.

### [P3-4] Carrier bindings opened through the new seams use `BindingLimits::default()`, not the binding's limits; the §3.3 stream-bound derivation assigned to 2.2c is not done

- **Location.** `gwz-core/src/transport_host/carrier/stream.rs` `bind_open` → `open_binding(session, BindingLimits::default())`, and `stream/inbound.rs:131-138` for accepted binds.
- **Root cause.** The trait seam `Carrier::bind_open(&self, session)` has no limits parameter, so the derivation cannot be expressed.
- **Invariant violated.**
  - 2.0 §3.3 "Transport-class bound": `data` is bounded "by the binding's `receive_window`". The amendment of 2026-10-27 adds: "`max_streams` … step 2.2c owns deriving it from the binding's stream limit".
  - The default gives a 1 MiB window per stream (`gwz_transport::binding::default_limits()`) and 4096 streams. The bindings' negotiated `receive_window` is 64 KiB (`gwz-connection/src/binding.rs:69-72`), and the core's mux holds `streams_per_binding` (64).
- **Impact.** Bounded. Before the carrier faults the binding, a peer can queue up to 4096 × 1 MiB at the receiving end instead of 64 × 64 KiB. Today the remote module's pump drains the transport class eagerly and the mux faults window violations downstream, so I do not claim a demonstrated memory blow-up. The clause's bound is still not the one implemented, and the seam cannot be fixed without a signature change.
- **Remedy.** Give `bind_open` (and the accept path) a `BindingLimits` derived from the binding's `Limits` and stream limit, using `BindingLimits::from_transport` with `max_streams` from the mux config. If the lane owner defers this, record the deferral against the §3.3 amendment.
- **Closure test.** A stream-carrier row where a peer sends more than 64 KiB on one stream of a remote binding. It should fault that binding at the carrier, and a frame for stream 65 should fault it too.

### [P3-5] `remote_lane.rs` cites a live-call registry method that does not exist

- **Location.** `gwz-core/src/transport_host/remote_lane.rs:8` and `:31` ("`Service::is_live`").
- **Problem.** No `is_live` exists in `session_host/`. 2.2b's `Service` has no live-call query, so `Calls` cannot be implemented from the unchanged `Service`.
- **Impact.** A misleading production-module doc about the one dependency 2.3 must supply. 2.3 has to add the query to 2.2b's `Service`.
- **Remedy.** Say that `Calls` has no implementor yet and that 2.3 adds the registry query (a request to 2.2b's owner).
- **Closure.** A doc revision.

### [P3-6] A carrier-backed `snapshot()` reports `phase: HostPhase::Open` after the carrier is lost (lane-owner question)

- **Location.** `gwz-connection/src/host.rs` `HostSnapshot::refusal`, which always sets `HostPhase::Open`. Compare `remote/backend.rs` `check_open`, which returns `HostClosing` once `lost`.
- **Reproduction.** End the carrier, then `handle.snapshot().phase == HostPhase::Open` while `check_open()` is `Err(HostClosing)`.
- **Impact.** A caller reading `phase` sees the wrong state. The counts are zero but readable, so only `refused()` separates "refused" from "idle".
- **Remedy.** Do not set `HostPhase::Open` in a refusal. Either derive the phase from `lost`, or document `phase` as meaningless when `refused()`.
- **Closure test.** Extend `snapshot_is_refused_and_carries_no_counts` across carrier loss.

**Lane-owner question (snapshot shape).** I support keeping `snapshot() -> HostSnapshot` with `refused()`. Changing it to `Result<HostSnapshot, _>` would break every local caller (gwz-core's `connection_host` tests, contract §4's signature) to serve the one remote case. 2.0 §3.4 says only "a refusal-only value with no counts". A `HostSnapshot` whose zeros are marked meaningless by `refused()` meets that, provided the docs say every field is meaningless when `refused()` and P3-6 is fixed. I see no stronger defect in the choice.

## 2. Invariant analysis (attacks that failed are part of the result)

- **One type family.**
  - `lib.rs` re-exports no `Remote*` type.
  - `RemoteAdmission`, `RemoteAcquire`, `RemotePermit`, `RemoteOp`, `RemoteCancel`, `RemoteOpening`, `RemoteOpenWait`, `RemoteStream`, `RemoteFinish` and `Live` are all `pub(crate)`, and each sits behind a private enum (`AdmissionKind`, `AcquireKind`, `PermitKind`, `Kind`, `CancelKind`, `OpeningKind`, `OpenWaitKind`, `StreamKind`, `Back`, `FinishKind`).
  - No `pub(crate)` type appears in a public signature. `TableRefusal`, inside `OpenError::Refused`, is `pub`. Clippy with `-D warnings` is in the evidence.
  - `RemoteConfig` is public, but it holds the core's own bounds, not a duplicate of the family.
- **Every local method has a remote counterpart or a defined refusal:**
  - `HostHandle::{admit, snapshot}`: implemented, and refused respectively.
  - `Admission::acquire`, `AdmissionPermit::register`: implemented.
  - `Operation::{registration, registered, cancel_handle, open, finish}`: implemented. `Drop` maps to `abandon`.
  - `OperationCancel::cancel`, `Opening::wait`, `OpenWait::is_ready`: implemented, but `is_ready` is wrong in the P2-1 state.
  - `Exchange::{stream, reused, finish, abort}`, `MessageStream::{read, write}` (`WouldBlock` as `Io(WouldBlock)`), `ExchangeFinish`: implemented.
  - The crate-private `attach_instance` is refused and `route` returns `Discarded(ForeignCarrier)`. Neither is public.
  - No other public method lacks remote semantics.
- **Forgery through the remote path failed:**
  - `Backend::admit` refuses `AdmitInput::Prepared` with `WrongInput` (`backend.rs:156-159`).
  - The `bind` is built by `Mux::initiator` with the crate's fixed `binding::limits()`. The client builds the `PreparedOperation` from its own `Invocation`.
  - On `open`, the client derives key, identity and username from its allowed sets and named keys (`served.rs:392-500`). It never uses the `Open`'s `policy`, `deadlines` or `receive_limits` to build its spec.
  - A core cannot supply a profile, auth context, policy or capacity.
  - `WrongInput` is reachable only by passing the other handle's input, which is misuse.
- **Public enum changes and consumers.** Grepping the whole lane finds no exhaustive `match` on any changed enum and no `Eq` bound outside gwz-connection. gwz-core uses `matches!`/`assert!` only (`connection_host/tests.rs:331-590`, `https_tests.rs:244-332`). gwz-py and gwz-cli reach gwz-connection only through their lockfiles. `binding.rs:185-191` `closing_code` now has a `_ => 3` arm; it is reachable only by carrier-backed reasons, which never meet a local binding.
- **Wire, `bind_rejected`.**
  - The codec set is exactly §3.4's five codes, with `Effect::None` and no facts.
  - Older readers: an initiator on `a22d43cd` treats `capacity`, `invalid_request` and `cancelled` as `Error::Protocol` (`mux/routing.rs:165-166`). No released reader exists on the served path, since the remote handle is new, and the in-process and native endpoints never send these codes. That failure is not reachable in practice.
  - No schema, generated codec or IR digest is involved: `validate.rs` is a codec rule, and the dependents are path dependencies. No taut schema file changed in any repo.
  - Residue: P3-3.
- **Carrier body:**
  - Tags: `Lane::of` (`carrier/mod.rs:77-82`) and `served/lane.rs:24-29` agree. Tag 16 carries `data`, `open` and `check_identity`; tag 17 carries the rest. A kind under the other tag is a `Message` fault, unchanged from 2.1.
  - Byte identity with `encode_lane`/`decode_lane` is proven both ways by `wire_rows.rs` for `data`, `window`, `cancel`, `close` and `open`, and the row passed here. `bind`, `bound`, `bind_rejected`, `opened`, `failed` and `check_identity` are not in that row. Production interop does not depend on them, because both ends decode with gwz-transport's codec, but the row could cover all 18 kinds.
  - `MAX_TRANSPORT_FRAME` is the frame plus head bytes (`a2`, `01`, `78 nn`, ≤128-byte name, `02`), which is correct.
  - Mixed versions: an old-form body (first byte 1..128) fails the `a2` head check, and a new body read by an old reader has a length byte of 162 > 128. Both fail closed as `ProtocolFault::Message` and the session ends. No release carries the old form.
- **Carrier seams and defaults.**
  - The in-process carrier's defaults are correct: `try_recv_transport = try_recv` (its `Arrival` carries only transport messages), and the bind seams are no-ops because it has no bindings.
  - The stream carrier's end waker is always included in `wakers()` (`state.rs:298-313`), so `Lane::register` sees transport arrivals. The registry keeps its own waker (`register_registry`), so the remote lane and 2.2b's consumer do not displace each other's wakers.
  - The accept-binds path decodes a whole envelope for a control-tag frame of an unopened binding (`inbound.rs:110-121`). That contradicts `wire.rs`'s "decodes the envelope only for a live binding" for client ends. It is bounded by the 128 KiB frame and stated in the evidence README. Not a finding.
- **`remote_lane` adapter and house rules.**
  - The only conditional compilation is in `cfg_if!` blocks. The two `#[cfg(test)]` methods at `receiver.rs:395-402` are braced `fn` items, which `check_cfg_boundaries.py` defines as their own boundary.
  - Rust enforces the brace rule.
  - No `static`, `thread_local` or global is added: the module's state lives in `Arc<Shared>`/`Arc<RShared>`.
- **Lifecycle.**
  - Cancel retires the stream slot: the row passed, and the mux shows `streams_held ≤ 4` after 12 aborted exchanges.
  - Carrier loss: a pending registration ends `NeverRegistered(CarrierLost)` and a live one sets `revoked`. A receive-pack exchange reports `Possible` once its `open` reached the outbox. That over-reports for an open that never left, which is conservative.
  - `admit` sends nothing for its own reference. Every `Shared::with` call pumps other bindings' traffic, but that is non-blocking, so the "no carrier I/O" claim holds in intent.
  - Panic paths: only `expect`s on states that were checked beforehand.
- **`finish()` releases nothing at the client.** Confirmed: 2.0 §3.3 has no binding-close kind, and the mux's `disconnect` is local. A consequence the row pins: a core-side re-admission of the same `request_id` after retirement passes `admit`, then ends `NeverRegistered(ClientRefused(InvalidRequest))` at the client's `seen` set. This conforms to §3.4 ("already-bound … `invalid_request`").

## 3. Risks and next action

- **Next action.** Fix P2-1 with a distinct cancel-on-open state and never-silent `Pending` in `poll_open`/`RemoteFinish`, and fix P2-2 with the `Cancelled → OpenFailure::Cancelled` mapping. Add the rows named under each. Then re-run the gwz-connection gate and the 2.2c rows. The P3s can ride the same revision; P3-1 is doc-only.
- **Risk.** P2-1 sits on the cancellation path that 2.3 (`operation.cancel`, carrier loss cancels the core operation) depends on. The plan's red rows did not cover it, and 55 mutations did not find it. Before the 2.3 lane builds on the handle, a row matrix of cancel × {queued, opening, open, closing} × {pending, registered} would close the class.
- **Risk.** `Shared::with` calls the `CoreBinding` (`endpoint_path_base`, `recv`/`send`) under the remote state lock on every pump. When 2.3 supplies `Calls` from 2.2b's `Service`, the lock order must be remote-state → registry and never the reverse.
- **Merge.** gwz-core `05ea42b5` sits on `4703a4e8`, not `58e506ee`. It must be merged with main's `58e506ee` (workflow-only). The Windows receipt `p2-2c-20261030-c` remains outstanding, as stated.

## 4. Out-of-scope observations (follow-up proposals; not findings)

- **Stale `dead_code` reasons in 2.2b's lane codec.** In 2.2b's `session_host/served/lane.rs:33, 52-57`, `encode_lane`/`decode_lane` carry `#[allow(dead_code, reason = "the carrier-backed handle (2.2c) sends/receives with it")]`. 2.2c did not use them: the carrier hand-encodes the same body, and the generated pair is now used only by `wire_rows.rs`. The reasons are false.
  - Proposal: 2.2b's owner either makes the carrier's encode call `encode_lane`, leaving one encoder, or moves the pair to test support.
  - Under the operator's "remove dead code now" rule this needs that owner's decision. It does not block.
- **Idle polling.** The core driver ticks every 100 ms for the session's life. The receiver ticks every 5 ms while any binding lives. The evidence states this. A wake from the owner's `ExchangeIo` would remove the polling, but that is a 1.3b adapter-port change, outside this step.
- **Missing transport terminal for a peer's `cancel`.** gwz-transport's stream machine sends no terminal when the peer cancels, and the client's endpoint half compensates (`served.rs:352-378`). A protocol clarification in gwz-transport, as the evidence suggests, would remove the special case.
- **Shared adapter type.** `CarrierLane` implements both `CoreBinding` and `ClientSession` and selects its role with `core: Option<…>`. A client end used as a `CoreBinding` gets namespace 0 and every `admit` returns `UnknownCall`. Two adapter types would make that misuse impossible by type. This is a maintainability proposal only.
