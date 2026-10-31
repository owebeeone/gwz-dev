# Step 2.2c rework — CODE-AXIS RE-VERDICT (round 1)

**Review object:** the step 2.2c rework after remediation round 1:
- gwz-transport `96938616` (changed range `9d5e0866..96938616`);
- gwz-core `09f4b8a8` (changed range `05ea42b5..09f4b8a8`);
- root `044dad8f` (`e4c95774`: the filed reviews and the remediation plan; `044dad8f`: the §15 row rewritten);
- gwz-core-evidence `33a0c9fa` (run `2026-10-30-p2-2c-rem1`).

Lane: `/Volumes/projects/limbo/gwz-dev-p2-rework`.
**Baseline:** my round-1 report, filed as `dev-docs/GwzSplitCommandChannel-Step2.2c-ReviewCode.md`, which reviewed gwz-transport `9d5e0866`, gwz-core `05ea42b5` and root `49504e6d`. **Date:** 2026-10-30
**Axis:** Code (architecture, interfaces, call graphs, compatibility reality). Independent, adversarial, read-only. Filed verbatim by the lane owner.
**Verdict: GO.** 0 P0, 0 P1, 0 P2 open. Two new P3s are open (P3-7 and P3-8); neither blocks. All eight round-1 findings are closed, P3-1 subject to the one inaccuracy recorded as P3-8.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| P2-1: cancel while an `open` is unanswered hangs `OpenWait` | Fix. `cancel_all` on `Stage::Opening` sets `outcome = Err(Cancelled)` plus a distinct `cancel_on_open` flag (`dropped` again means only "no handle"). `poll_open` and `RemoteFinish::poll` treat a missing record as terminal. Wakers are per condition. | I re-ran my original row on `96938616`: after cancel and settle, `is_ready=true poll=Ready(Err(Cancelled))` (round 1: `Pending`). New probes: cancel, then finish, gives `Ready(OperationEnd{… cancelled: true})` with `bindings()==0` (the binding retires); cancel then carrier end before the poll gives `Ready(Err(CarrierLost{effect: None}))`, and `is_ready()` agrees. Code: `core.rs` `cancel_all`/`decide`/`abort_all`, `stream.rs:96-117, 233-252`. The lane adds 9 cancel rows and 2 driver-only rows. | Closed |
| P2-2: a pending cancel or finish reports `Fault(Cancelled)` | Fix. `abort_all` maps `Cancelled` to `OpenFailure::Cancelled`. | Re-ran my probe: the pending cancel gives `registration=NeverRegistered(Cancelled) wait=Ready(Err(Cancelled))` (round 1: `Fault(Cancelled)`); a registered cancel gives `Ready(Err(Cancelled))`. Lane rows cover cancel and finish of a pending operation. | Closed |
| P3-1: §15 row incomplete | Rewrite the row. | Root `044dad8f`. The row now lists every changed public variant and field, the `Eq` removal, `#[non_exhaustive]`, `Registered`, `AdmitInput`, `OperationRef`, the extra `ClientRefused` codes, the pending-cancel and `TimedOut` outcomes, the three §5 sentences amended, and the `remote` module's ports including `BindBounds`. I checked it against `lib.rs` `pub use` and `remote/mod.rs`. One sentence does not hold: the row says `OpenFailure::Fault` occurs only for broken rules, but the unchanged code still produces `Fault(Capacity)` (see P3-8). | Closed (residue under P3-8) |
| P3-2: `OperationRef` 96 vs 128 bytes | Fix to 128. | `operation.rs:375-383` is now `len() > 128`. Row `a_128_byte_request_id_is_admitted_and_registers_over_the_remote_handle`. | Closed |
| P3-3: `bind_rejected` admits a capacity detail; README stale | Refuse any detail; correct the README. | `validate.rs:297` adds `failure.detail.is_some()`. README:379-397 is restated (five codes, `Effect::None`, no facts or detail). Row `a_bind_rejected_carries_no_failure_detail_not_even_a_capacity_detail`. No sender sets a detail on a rejection (`binding.rs:138`, `mux/mod.rs:439, 566`, `mux/routing.rs:123` all build `detail: None`), so tightening the codec breaks no existing writer. | Closed |
| P3-4: carrier binding bounds default to 1 MiB × 4096 | Derive from the binding's limits. | `bind_open(session, BindingLimits)` and `accept_binds(BindingLimits)`. The values come from `BindBounds::core(&RemoteConfig)` (64 KiB, `streams_per_binding`) and `BindBounds::client()` (64 KiB, 64, with the endpoint mux now set to `max_streams: STREAMS`, which equals the mux default, so client behaviour is unchanged). gwz-core rows check the window and stream overrun for both the core-opened and the client-accepted cases. `BindBounds` is judged below; the residual coupling is P3-7. | Closed |
| P3-5: `remote_lane.rs` cites `Service::is_live` | Doc fix. | `remote_lane.rs:8, 39-41`: "`Calls`, which no type implements yet … step 2.3 adds it". | Closed |
| P3-6: refusal snapshot says `Open` after carrier loss | Truthful phase; docs say counts are meaningless. | `backend.rs` `snapshot()` reports `Closed` once `lost`. The `HostSnapshot` and `refused()` docs now say every count is meaningless. Row `a_refused_snapshot_says_closed_once_the_carrier_is_lost_and_open_before`. | Closed |

## Changed-range analysis

I read every production hunk of `9d5e0866..96938616` and `05ea42b5..09f4b8a8`, plus the root diff. I found no NEW ARCHITECTURAL root cause.

- **Within the dispositions:**
  - Per-condition waiters (`core.rs` `Wait`, `wait_for`, `holds`, `ready_wakers`; State P2-1/P3-3). A waiter is woken only when its own condition holds. At most 16 wakers are kept per condition, and `will_wake` deduplicates them. The admission queue wakes only its head while a slot is free, or every entry on loss. I traced the lost-wakeup cases:
    - registration and pending-poll both run under one lock;
    - a slot is freed by `retire_finished` within a `with`, which wakes the head in the same call;
    - a dropped head admission runs `with`, which wakes the new head;
    - a timeout is driven by the acquire's own `Sleep`.

    None loses a wake.
  - `last: Arc<OnceLock<Registration>>` is set under the lock before the binding leaves the table. `registration()` falls back to `NeverRegistered(HostFault)` only if that unreachable invariant breaks; that is acceptable.
  - `purge_ended` drops a faulted or ended binding's unsent outbox messages (State P2-3). `CarrierLane` remembers up to 1024 retired names and silently drops a `bind` for one. That bound equals `max_bindings`; it is per-lane state, not a global.
  - `Served::terminate` uses the shared `effect_after` (State P2-2).
  - Receiver backpressure stops `recv` at `OUTBOX_ROOM` (State P3-1). No deadlock is possible: the core end drains its lane unconditionally and the carrier writer is independent.
  - `#[cfg(test)]` moved into `cfg_if!` (State P3-4). A grep of `remote/` now shows only `if #[cfg(test)]` inside `cfg_if!`.
  - The carrier changes are a signature widening of `bind_open` and `accept_binds` plus `BindingLimits::bounded`. In-process defaults are unchanged. `FrameSink`, `FrameRouter`, `FrameKey` and 2.2b's `Service` are untouched (no `crates/` or `session_host/` diff).
  - `.github/gwz-transport.commit` and the allowlist's `reconciled_commit` move to `96938616`, per the bump rule. That commit is not yet pushed, which is the known upstream-first order.
- **Outside the dispositions:** nothing found. One limitation: the `RemoteFinish` missing-record fallback reports `(Cancelled, Possible)` for every service. Its comment says it is unreachable while the future holds its `Live`, and I found no path to it.
- **New public type `remote::BindBounds` (judged).**
  - **Necessary.** gwz-core cannot name gwz-connection's crate-private `binding::limits()` or the endpoint mux's stream bound, and the derivation is 2.2c's (2.0 §3.3 amendment of 2026-10-27).
  - **Narrow.** It is two `usize` fields and two constructors, it holds no GWZ or carrier type, and it is recorded in the §15 row.
  - **Shape defect.** The core-side value is built from a `RemoteConfig` passed to `CarrierLane::core` separately from the one passed to `remote::handle_with`. That coupling is P3-7.

## 0. Evidence base

- **Tuple.** Checked at the start and at the end, identical both times: root `044dad8f`, gwz-transport `96938616`, gwz-core `09f4b8a8`, gwz-core-evidence `33a0c9fa`. No tracked changes in any member.
- **Read:**
  - the remediation plan `dev-docs/GwzSplitCommandChannel-Step2.2c-RemPlan.md`;
  - every production hunk of both changed ranges;
  - the names and closure rows of the new tests (`cancel_rows.rs`, `state_rows.rs`, the `core_rows.rs`/`mux.rs` additions, the gwz-core `remote_lane/tests.rs` additions);
  - the rewritten §15 row;
  - the mutation summary in the rem1 evidence README.
- **Runs.** All used `CARGO_TARGET_DIR` under `/Volumes/projects/limbo/build-targets/p2-2c-review-code`, `TMPDIR=/Volumes/gwztmp`, no network.

  | Run | Result |
  | --- | --- |
  | `cargo +1.96.0 test -p gwz-connection --offline` | 184 passed (round 1: 153) |
  | `cargo +1.96.0 test -p gwz-transport --offline` | 261 passed, 0 failed |
  | `cargo +1.96.0 clippy -p gwz-connection --all-targets -- -D warnings` | clean |
  | Candidate tree refreshed with `prepare.py --refresh`, `RUSTFLAGS='--cfg gwz_transport_candidate'`, `test --lib -- transport_host::carrier transport_host::remote_lane session_host::served::lane` | 113 passed |

- **Probes.** These ran on a `git archive` copy of `96938616` at `/private/tmp/claude-501/-Volumes-projects-limbo-gwz-dev/351b18f9-4ec1-4306-ac0a-299e9bded6dd/scratchpad/repro2`, with my round-1 probe file plus three new probes, built in the separate target `…/p2-2c-review-code/repro`. All 6 ran; the outputs are quoted in the closure table and under P3-8. Nothing in the lane was written.

## Problem, requirement provenance, and scope

The requested outcome and authority are unchanged from round 1. The remediation plan, merged by the lane owner from both reviews, is the authority for the round-1 changes. Every changed file is one the brief authorized or one already judged necessary in round 1 (the carrier trait and router files, `remote/`, `validate.rs`, the README for the wire change, and the pins).

The only surface the plan did not name is `BindBounds`. It is judged above as necessary and narrow.

No 1.3b/1.3c, 1.4, 1.5 or 1.6 file is touched, and nothing of 2.2b changed. The P3s below are inside authorized files.

## 1. Findings

### [P3-7] The carrier's binding bounds come from configs built apart from the binding they bound

- **Location.**
  - `gwz-core/src/transport_host/remote_lane.rs` `CarrierLane::core(…, bounds: BindBounds)` versus `remote::handle_with(lane, config)`.
  - `gwz-connection/src/remote/lane.rs` `BindBounds::core(&RemoteConfig)`.
  - `BindBounds::client()`, fixed at 64 streams, while `RemoteConfig::streams_per_binding` admits up to 1024 (`backend.rs` `streams()`).
- **Invariant.** 2.0 §3.3 "Transport-class bound" and its amendment: the carrier holds a binding to that binding's own stream limit, and a sender that obeys its limits cannot overrun.
- **State sequence.**
  - **Core mismatch.** The composition passes `BindBounds::core(&RemoteConfig::default())` to the lane and `handle_with(lane, RemoteConfig{streams_per_binding: 128, ..})` to the handle. The core's mux then legitimately holds 128 streams, but the core's carrier faults the binding at 65 queued streams.
  - **Client mismatch.** A core with `streams_per_binding > 64`, against a client whose receiver stops taking (the new `OUTBOX_ROOM` backpressure), has more than 64 streams of queued state at the client carrier. The client faults the binding. It does not refuse the excess opens with `capacity`.
- **Impact.** Bounded. Defaults agree at 64/64 and no production composition exists yet (step 2.3 writes it). It is still a trap at the composition seam.
- **Remedy.** Do one of the following, and record the cross-end ceiling in the §15 row:
  - have the handle or lane pair derive the bounds from the single config, for example `handle_with` returning or accepting the lane's `BindBounds`, or the lane reading it from the `CoreBinding`;
  - or clamp `RemoteConfig::streams_per_binding` to the client's 64, since the client cannot accept more without faulting.
- **Closure test.** A row that builds the core lane and handle from one config with `streams_per_binding = 128`, opens 128 streams with the client lagging, and asserts no fault. Plus a row pinning the client-side behaviour beyond 64.

### [P3-8] Re-opening at the stream bound right after an abort fails with `OpenFailure::Fault(Capacity)`, which the new §15 row says cannot happen

- **Location.** `gwz-connection/src/remote/core.rs:673` (`Err(mux::Error::Capacity) => Some(OpenFailure::Fault(ErrorCode::Capacity))`). This is code unchanged since round 1; I missed it then. Also `core.rs:684-687`, where the `ExchEnd` records `InvalidRequest` for that same case. The contradicting text is root `GwzConnectionApiContract.md` §15 ("`OpenFailure::Fault(code)` is only a rule of the transport broken (`protocol`, or the table's `invalid_request`)").
- **Invariant.** The contract row as now written. Parity with the local `open`, which answers a full binding synchronously with `OpenError::BindingLimit`, never with a later fault.
- **Reproduction.** My probe `review_r2_reopen_right_after_an_abort_at_the_stream_bound`: `streams_per_binding = 2`, open two exchanges, abort one, immediately `open().wait()`. The first poll returns `Ready(Err(Fault(Capacity)))`. The reason: `RemoteOp::open` counts the aborted exchange as `Ended`, but the core mux keeps the stream's route until the client's terminal returns.
- **Impact.** A transient, timing-only refusal reaches the consumer classified as a transport fault. A fast abort-and-retry (gwz-git) fails spuriously, and the §15 row misstates the surface.
- **Remedy.** In `issue_opens`, treat `mux::Error::Capacity` like `WouldBlock`: keep the exchange queued until a route retires, since `RemoteOp::open` already enforces the binding limit. Otherwise, add `Capacity` to the row's list and map the case to a non-fault variant. Either way, record the right `ExchEnd` code.
- **Closure test.** The probe above should either succeed once the client's terminal arrives or resolve with a documented non-`Fault` outcome. The §15 row should be updated to match.

## 2. Invariant analysis (attacks that failed are part of the result)

- **One type family.** Still no public `Remote*` type. The new public surface is `BindBounds` only. No `pub(crate)` type is exposed through a public signature (clippy `-D warnings` is clean).
- **Cancel matrix.** Attacked and holding:
  - queued (pending: cancel and finish → `Cancelled`);
  - opening (registered: `opened`, `open_failed` or carrier loss → `Cancelled`/`Cancelled`/`CarrierLost`);
  - open and closing (finish resolves, binding retires).

  A pending operation cannot reach opening, open or closing. A registered operation's queued stage is transient, because `open()` issues the `Open` inside the same `with`. A late `opened` after a cancel is answered with `cancel`, and the slot comes back (lane row). Polling a completed wait again returns `Cancelled` rather than hanging.
- **Wake discipline.** I traced every `wait_for` site (`poll_registered`, `poll_open`, `RemoteFinish`) and the admission queue. Each registers under the same lock that observed "not ready". `ready_wakers` runs after the final pump of the same `with`, so no state change between the check and the registration can be missed. The driver's 100 ms tick remains a backstop, not a correctness dependency.
- **Wire compatibility.** The `bind_rejection` tightening only narrows what is admitted. No writer in any repo sets a rejection detail. No schema, generated code or IR digest changed. Carrier body handling is unchanged since round 1, and its rows passed in the candidate run.
- **Retired names.** A `bind` for a retired session is dropped at both layers: the core's outbox purge and `CarrierLane::send`. The retired list is bounded at 1024. Evicting an old name could in principle let a bind reopen it, but session names are minted fresh per binding (`session_name(serial)`), so a stale bind for an evicted name requires 1024 later retirements while that bind sits in the outbox. Purging on fault makes that unreachable.
- **House rules.** Conditional compilation in the changed range sits only inside `cfg_if!`. No globals or thread-locals: `retired` is per-lane. Brace rule satisfied.
- **Contract row versus code.** It matches `lib.rs` and `remote/mod.rs`, except for the P3-8 sentence.

## 3. Risks and next action

- **Next action.** GO for the step. P3-7 and P3-8 can be fixed in this lane or carried as named items into step 2.3, which writes the composition P3-7 constrains. If carried, the lane owner should correct the §15 sentence now (P3-8's doc half), so the contract does not assert a false invariant while the code catches up.
- **Windows receipt.** Still outstanding, under a new label on `09f4b8a8` after gwz-transport `96938616` is pushed.
- **Merge.** gwz-core still sits on `4703a4e8`. Main's `58e506ee` and `fb62344d` touch none of this object's files.
- **Risk.** Lock order for 2.3 is remote-state → registry, as in round 1. `Shared::with` now also calls `CarrierLane::send`, which takes the lane's `retired` mutex under the remote-state lock; that order must not be reversed either.

## 4. Out-of-scope observations (follow-up proposals; not findings)

- Unchanged from round 1, and recorded in the remediation plan:
  - 2.2b's `encode_lane`/`decode_lane` carry `dead_code` reasons that name 2.2c, which does not use them;
  - idle polling (100 ms core tick, 5 ms receiver tick);
  - the stream machine sends no terminal for a peer's cancel;
  - `CarrierLane` serves both roles.
- The §15 row's note on the 5 s admission bound versus the 5 s bootstrap race, and on call-scoped client slots, correctly hands both to the operator as a 2.0 §3.4 decision (a release kind or a multi-bind rule). I agree they are outside this step.
