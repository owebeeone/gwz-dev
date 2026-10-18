# Core session crate map step 4, phase B: moving the gate, limits and supervisor into gwz-session-host

Date: 2026-09-29. Status: **applied in the tree, uncommitted, for review.** Its home is gwz-dev `dev-docs/`, which phase A could not write to. It plans the in-tree move of step 4 of the [core session crate map](GwzCoreSessionCrateMap.md) (§6 step 4, the §2 `gwz-session-host` row and notes). Phase A drafted the crate outside the tree, and the coordinator accepted it as phase B's basis with six decisions (§12). Phase B then applied the preconditions and B1 to B6. §12 records each decision and everything that departed from the plan as first written; the sections before it are updated to match.

## 0. Preconditions

- **Steps 2 and 3 have landed.** Step 2 is `gwz-ids`. Step 3 is `gwz-session-contract` and `gwz-session-channel`, registered with the inventory, `EXPECTED_CRATES` and Bazel. Phase A found them arriving in `gwz-core/crates/`.
- **Step 3's tests run in CI.** They sat in `tests/`, and gwz-core runs a crate only as the Tier A command, `cargo test -p <name> --lib --locked` (`checked-artifact-boundary.yml`), with `run_tests.py` covering only the root package. So none of their 35 tests ran in CI.
  - **Done:** moved into their lib targets: gwz-session-contract's `src/tests.rs`, and gwz-session-channel's `src/in_process/tests.rs`, `src/byte_stream/tests.rs` and `src/tests.rs`. Each is declared `mod tests;` over a file whose first line is `#![cfg(test)]`, and each test keeps its name (35 before, the same 35 after).
  - The vectors stay in `tests/vectors/`, read with `include_bytes!(concat!(env!("CARGO_MANIFEST_DIR"), "/tests/vectors/…"))`.
  - Tier A now runs 12 and 23.
- **The code to move is still the accepted object.** Check these gwz-core files against their SHA-256s (CS1.4/CS1.5 Verdict-1, CS1.9 Verdict). Phase A's check on 2026-09-29 matched all nine. If any differs, re-diff it against the draft first.

  | File | SHA-256 |
  | --- | --- |
  | `src/session_host/gate.rs` | `68343fae…b260` |
  | `src/session_host/gate/tests.rs` | `d4bbf919…747b` |
  | `src/session_host/limits.rs` | `2cb47965…39d4` |
  | `src/session_host/context.rs` | `504ef99e…07d1` |
  | `src/session_host/context/tests.rs` | `e233fb1d…9d47` |
  | `src/session_host/mod.rs` | `dddbdcb1…ce01` |
  | `src/session_host/environment.rs` | `0df22620…c5f8` |
  | `src/session_host/environment/tests.rs` | `38c37b67…a866` |
  | `docs/RustApi.md` | `3b6906c0…0e59` |

## 1. What moves and what stays

| Item (gwz-core today) | Goes to | Reason |
| --- | --- | --- |
| `gate.rs`: `CallControls`, `OperationGate`, `GateScope`, `GateState`, `HandlerContext` | `gwz-session-host` `gate.rs`, generic over `S` | Map §6.4: the gate moves with §2's nesting design. It touches only per-session data, passed opaquely. |
| `gate.rs`: `CancellationToken`, `CancelRegistration` | `gwz-session-host` `token.rs` | The token is the gate's; split out so each file stays under 500 lines |
| `gate.rs`: `thread_local! CROSSING`, `CrossingMark`, `forbid_nesting` | removed | Map §1, no thread-locals. The capability design and a per-gate `crosser` record replace it. |
| `gate.rs`: `cancelled()` → `ModelError(Cancelled)` | a crate-local `Refused { Cancelled, Revoked }`; core maps it | Map §6.4, crate-local errors; core keeps its model errors |
| `gate/tests.rs` | `gwz-session-host` `src/gate/tests.rs` | Tests move with their code |
| `limits.rs`: `Limits`, `MAX_READ_WAIT` | `gwz-session-host` `limits.rs`; core re-exports | Map §6.4: the limits move |
| `limits.rs`: `MAX_FRAME_BYTES` | gone from core; core re-exports `gwz_session_contract::MAX_FRAME_BYTES` | One definition, the frame layer's. Same name, type and value. |
| `limits.rs`: `Limits::validate` (`pub(crate)`, `ModelResult`) | `gwz_session_host::validate_limits(&Limits) -> Result<gwz_session_contract::Limits, InvalidLimits>` | Crate-local error. A free function, so the frozen `Limits` gains no method. It also returns the channel's two limits for `pair`. |
| `limits.rs` tests | `gwz-session-host` `src/limits/tests.rs` | Tests move with their code |
| `context.rs`: `SupervisedJob`, `Supervisor`, `SupervisorState`, `discard`, `SUPERVISOR_POLL`, the thread's `run`, `release`, `dispose` | `gwz-session-host` `supervisor.rs`: `Supervisor { new, supervise, shutdown(bound) }` | Map §6.4, "the supervisor with `shutdown(bound)`" |
| `context.rs`: `HostContext::supervise`'s body, `HostShared::shutdown`'s disposal | delegates to the crate's `Supervisor` | The composite stays in core (map §7, §5.6) |
| `context.rs`: `HostContext`, `HostShared`, `ShutdownReport`, `CLEANUP_BOUND`, the `shut_down` flag and report cache | stay in core | The host context is core's composite. Its report adds CS3.7's registry to the supervisor's count. The 5 s bound is the composite's (reuse §7). |
| `context.rs`: `SessionContext` | stays; it is the crate's `S` | It holds the snapshot, which stays in core (map §2 note, CS1.5/CS1.9 reviews) |
| `context.rs`: `SessionOptions`, `open`, `ClientChannel` | stay | Frozen API (`docs/RustApi.md`); `open` is core's (map §3) |
| `context.rs`: `SupervisorWatch`, `supervisor_watch` (test-only) | replaced by the crate's `test_support::watch`, behind its `test-support` feature | Map §1: crate internals reached through a feature that only core's dev-dependency enables |
| context tests about the supervisor | move, renamed where their subject was the host context (below) | Tests move with their code |
| context tests about `open`, the composite, the snapshot and `transport_off` | stay | Their code stays |
| `environment.rs` and its tests | stay | Secrets stay in core (map §1) |

What moves is the supervisor, not what it will hold. CS3.5 moves the SSH setup hub onto it later.

Tests that stay in core's `context/tests.rs`:
- `the_public_types_cross_threads`
- `open_creates_the_session_context_from_its_options`
- `open_refuses_invalid_limits_with_invalid_request`
- `sessions_share_the_host_context_they_are_given`
- `dropping_a_host_context_ends_its_supervisor_thread_once_its_jobs_finish`
- `a_host_context_that_never_supervised_ends_at_once`
- `an_open_session_keeps_its_host_context`
- `shutdown_returns_at_its_bound_while_a_job_runs_and_a_second_call_returns_the_same_report` (the composite's `ShutdownReport`)
- `concurrent_shutdowns_wait_for_the_first_and_return_its_report` (the composite's cache)
- `after_shutdown_no_job_is_supervised_and_no_session_opens`
- `a_session_reads_transport_off_from_its_options_and_never_from_its_snapshot`
- `a_session_drops_its_snapshot_when_it_ends`

Those that need the thread use `gwz_session_host::test_support::watch(&host.shared.supervisor)`.

Tests that move to the crate:
- Unchanged names:
  - `a_job_that_panics_is_quarantined_and_never_polled_again`
  - `a_quarantined_job_does_not_stop_the_others`
  - `shutdown_returns_at_its_bound…` and `concurrent_shutdowns…`, which core also keeps
  - `shutdown_returns_once_its_jobs_finish_and_releases_the_supervisor`
  - `a_quarantined_job_appears_in_the_pending_report`
  - `a_drop_after_shutdown_disposes_nothing_more`
- Renamed:
  - `dropping_a_supervisor_ends_its_thread_once_its_jobs_finish`
  - `a_supervisor_that_never_supervised_ends_at_once`
  - `after_shutdown_no_job_is_supervised`
- Every gate and limits test moves, with three exceptions:
  - `a_crossing_inside_a_crossing_of_the_same_gate_panics_instead_of_deadlocking` can no longer be written. A `compile_fail,E0499` example on `OperationGate` stands in for it.
  - `a_crossing_of_another_gate_inside_a_crossing_panics` is dropped. It is the undetected path the map states.
  - `a_cancel_callback_that_crosses_a_gate_is_stopped_and_the_canceller_goes_on` becomes `a_cancel_callback_that_revokes_inside_a_crossing_is_stopped_and_the_canceller_goes_on`.

## 2. Phases and steps

Each step is one commit, with an aspirational budget under 500 lines of hand-written change. Moved lines are counted separately.

### Phase B-1: the move

Milestone: the gate, limits and supervisor live in `gwz-session-host`; gwz-core re-exports the frozen items; `CROSSING` is gone; nothing changes for a driver.

- **B1: Land and register the crate.** No core code changes.
  - Files: `crates/session-host/**`, copied from phase A's scratch workspace with the README's cancel-callback section added (decision 2); `scripts/checks/local_clone_inventory.json` (entry below); `scripts/checks/check_crate_versions.py` (`EXPECTED_CRATES` 17 → 18); `Cargo.lock` (the new path package, and no registry package); gwz-dev `MODULE.bazel` (the manifest).
  - The count pins that an eighteenth crate trips, found in B1:
    - `scripts/checks/test_check_crate_versions.py`: the fixture's crate list and `PLAN_LAYERS` gain the crate at layer 2, the counts move by one, and two tests are renamed, `…prints_eighteen_names…` and `…off_eighteen_is_rejected`;
    - `scripts/checks/test_check_local_clone_boundaries.py`: the expected inventory table gains the crate, and the present count moves 17 → 18;
    - `scripts/test_publish_crates.py`: 17 → 18;
    - `scripts/test_release_bump.py`: five numbers only (17 → 18 three times, 18 → 19, and edges 35 → 36), leaving its pending fixes intact;
    - gwz-core `dev-docs/GwzCratesIoPlan.md` §1, the layers these tests check: `gwz-session-host` joins layer 2, and D1's count becomes nineteen.
  - Not done: regenerating `MODULE.bazel.lock`. It records a hash of each crate manifest and of gwz-core's `Cargo.toml` and `Cargo.lock`, and was already stale after steps 2 and 3 (no `ids` or session entries), so one `bazel mod deps --lockfile_mode=update` should cover all four crates.
  - Gate: the crate's Tier A command, `cargo test -p gwz-session-host --lib --locked` (35 tests); `cargo test -p gwz-session-host --doc --locked` (5); the boundary gate; `check_crate_versions.py`; `check_process_globals.py` (roots `crates/*/src/lib.rs`: nothing new); `check_cfg_boundaries.py`.
  - About 40 lines outside the crate.
- **B2: Core takes its limits from the crate.** After B1.
  - Files: `Cargo.toml` (`gwz-session-host` and `gwz-session-contract` as path+version dependencies); `BUILD.bazel` (both labels in `_LOCAL_CLONE_LIBRARIES`); delete `src/session_host/limits.rs`; `src/session_host/mod.rs` (re-exports, §3); `src/session_host/context.rs` (`open` calls `validate_limits`, §4).
  - Test-first: `open_refuses_invalid_limits_with_invalid_request` stays, and a new `each_limits_refusal_keeps_its_text` pins that `open`'s messages equal the pre-move texts.
  - About 60 lines, most of them deletions.
- **B3: Core's host context composes the crate's supervisor.** After B2, since it touches `mod.rs` and `context.rs`.
  - Files: `context.rs` loses the supervisor (about 150 lines), and `HostShared` holds a `gwz_session_host::Supervisor`; `context/tests.rs` loses the moved tests and uses `test_support::watch`; `Cargo.toml` gains the dev-dependency `gwz-session-host = { path = "crates/session-host", features = ["test-support"] }`, with a path only, as the versions gate requires.
  - About 120 lines of change, plus about 250 of test deletions.
- **B4: Core drops its gate; `CROSSING` goes.** After B2. It can go beside B3 if one agent merges both, since `mod.rs` is shared.
  - Files: delete `src/session_host/gate.rs` and `gate/tests.rs`; `mod.rs` (the `pub(crate) mod gate` declaration and its `allow`); `scripts/checks/process_globals_allowlist.json` loses the `CROSSING` entry in the same commit, since the checker fails on a stale entry; `src/session_host/errors.rs`, begun in B2, gains the `Refused` mapping (§4), with `errors/tests.rs`.
  - `scripts/checks/test_check_process_globals.py`, which pinned the entry: 27 → 26; `test_counters_and_crossing_are_debt_with_their_owners` becomes `test_the_counters_are_debt_with_their_owners`; and a new `test_crossing_is_gone` asserts that no `session_host` entry remains.
  - Nothing in gwz-core calls the gate yet, since CS1.6 and Phase 2 are its first callers, so no call site changes.
  - Test-first: `a_gate_over_the_session_context_reaches_its_limits` (core's half of the old gate tests, over `CallControls<SessionContext>`) and `a_refused_crossing_is_cancelled_with_its_text`.
  - About 60 lines, plus 675 of deletions.

### Phase B-2: the channel and the text

Milestone: `open` returns a working client end, and the documents match the code.

- **B5: Wire `pair(Limits)` into `ClientChannel`.** This is the `TODO(CS1.2)` in `context.rs`. After B2, and after step 3's `gwz-session-channel` has landed.
  - Files: `Cargo.toml` and `BUILD.bazel` (`gwz-session-channel`); `context.rs` (`open` and `ClientChannel`, §5); `mod.rs` (the frame re-exports, §3); `context/tests.rs`.
  - Test-first:
    - `open_returns_the_client_end_of_an_in_process_channel`: a frame sent on it reaches the parked host end, and the host's reply comes back;
    - `a_full_lane_refuses_at_the_client_end`: with `options.limits.outstanding_calls = 1` and `options.limits.control_reserve = 1`, the second call frame gets `SendError::Full` while a control frame still goes. The fields are set by assignment: once `Limits` is the crate's, it is a foreign `non_exhaustive` type in core, as core's existing tests already treat it;
    - `closing_the_client_channel_ends_the_session_at_both_ends`. Dropping it is `a_session_drops_its_snapshot_when_it_ends`, which already stands, and a dropped end cannot be observed from the host end it drops with;
    - `the_public_types_cross_threads` stays green.
  - About 150 lines.
- **B6: Text.**
  - `docs/RustApi.md` (§6 below, exactly);
  - gwz-core `dev-docs/GWZDesign.md` (§9);
  - `src/lib.rs`'s comment on `session_host`;
  - `mod.rs`'s module doc.
  - The contract edits that map §7 ties to this move are listed in §10.
  - About 60 lines of prose.

B1 must come first. B2, B3 and B4 each touch `mod.rs`, so they are serial or one agent's. B5 needs only B2 and step 3. B6 closes the phase. Review: one review for the phase, per the review granularity ruling. The gate's API is an interface re-freeze (plan §2.2: CS1.4 with CS1.5's freeze was dual), so its changes (§7) go to that review as a list.

## 3. Re-exports that keep today's paths

`src/session_host/mod.rs` after B5:

```rust
pub use context::{ClientChannel, HostContext, SessionOptions, ShutdownReport, open};
pub use environment::EnvironmentSnapshot;
pub use gwz_session_contract::MAX_FRAME_BYTES;
pub use gwz_session_host::{Limits, MAX_READ_WAIT};
// B5: what a ClientChannel's send and recv take and return.
pub use gwz_session_contract::{
    Closed, Frame, FrameError, FrameSink, FrameSource, Lane, SendError, Tag,
};
```

- gwz-core re-exports no gate type. They are crate-private in core today, and drivers never hold one.
- Core's own code names the crate's types directly, as `gwz_session_host::CallControls<SessionContext>`.
- `git/endpoint` and `transport_host` already import `tokio_util::sync::CancellationToken`. CS3.7's binding of the session token to the HTTPS engine should name the session one by its path, never glob it.

## 4. How core maps the crate's errors and values

| Crate | gwz-core | Text |
| --- | --- | --- |
| `InvalidLimits::*` from `validate_limits` | `ModelError::new(ErrorCode::InvalidRequest, error.to_string())`, in `errors.rs` as `impl From<InvalidLimits> for ModelError`, so `open`'s `?` reports it before any effect | Unchanged: `refusals_say_what_core_says_today` pins each text in the crate |
| `Refused::Cancelled`, `Refused::Revoked` | `ModelError::new(ErrorCode::Cancelled, refused.to_string())`, in `errors.rs` as `impl From<Refused> for ModelError` | Unchanged: CS1.4's two messages |
| `SuperviseError::ShutDown` | `io::Error::other("the host context has been shut down")`, in `HostContext::supervise` | Unchanged |
| `SuperviseError::Spawn(error)` | `error` | Unchanged |
| `Supervisor::shutdown(bound) -> usize` | `ShutdownReport { pending_local_work: u32::try_from(n).unwrap_or(u32::MAX), peer_cleanup_confirmed: false }`, cached by `HostShared`, which CS3.7 extends | Unchanged |
| channel `SendError::Full` | `transport_session_full` (75), where a caller reports it (bridges, CS4.x) | Contract §3, §9 |
| a closed channel | `server_session_closed` (83) at the bridges (server design §8) | |
| `Closed::Protocol` | the session ends and no reply is attributed (contract §3) | |

`HostShared::shutdown(bound)` keeps CS1.9's shape:
- it takes its report lock;
- it returns a cached report;
- otherwise it sets `shut_down` and calls `self.supervisor.shutdown(bound)`, which CS3.7 will put after the endpoint registry's disposal, passing what remains of the one deadline (CS1.9 verdict, carried to CS3.7);
- it builds and caches the `ShutdownReport`.

`Drop for HostShared` goes, because the crate's `Supervisor` releases its thread in its own `Drop`.

## 5. `pair(Limits)` in `ClientChannel` (B5)

```rust
pub fn open(options: SessionOptions) -> ModelResult<ClientChannel> {
    let channel_limits = gwz_session_host::validate_limits(&options.limits)
        .map_err(|error| ModelError::new(ErrorCode::InvalidRequest, error.to_string()))?;
    // ... the shut-down check and the session context, as today ...
    let (end, host_end) = gwz_session_channel::pair(channel_limits);
    Ok(ClientChannel { end, host_end, session })
}

pub struct ClientChannel {
    end: InProcessEnd,
    /// The host's end, parked until the session host serves it (CS2.2, in
    /// gwz-session-host's `serve`); it drops with the client's.
    host_end: InProcessEnd,
    session: Arc<SessionContext>,
}
```

- **The methods.** Inherent `send(&self, Frame, Lane) -> Result<(), SendError>`, `recv(&self) -> Result<Frame, Closed>` and `close(&self)` delegate to `end`, and `ClientChannel` also implements `FrameSink` and `FrameSource`.
- **The host end.** It stays in the `ClientChannel`, never in `SessionContext`. That context is the gates' `S`, and a crossing's closure must never reach a channel end (O6).
- **Until CS2.2.** Nothing answers, so a driver's `recv` would wait. No driver calls it before the plan's Phase 4 or 6.
- **Tests** reach the host end through the `cfg_if!` test accessor that `context.rs` already uses for `session()`.
- **At CS2.2**, `open` hands `host_end` and the session to `serve` instead.

## 6. The frozen API: `docs/RustApi.md`, exactly

The nesting design changes nothing in `docs/RustApi.md`:
- every gate item is `pub(crate)` in gwz-core today (`mod.rs`: `pub(crate) mod gate;`);
- `RustApi.md` names none of them;
- core will not re-export them.

The limits change nothing either:
- `gwz_core::session_host::Limits` is the same type, re-exported, with the same fields, defaults, derives (`Clone, Debug, Eq, PartialEq`) and `non_exhaustive`;
- validation became a crate function, not a method;
- `MAX_READ_WAIT` and `MAX_FRAME_BYTES` keep their names, types and values.

B5's wiring changes three passages.

**1. Line 19, the module table.**

Frozen:
> | `session_host` | The core session host's frozen foundations (see "Session Host" below): `HostContext` with its `ShutdownReport`, `EnvironmentSnapshot`, `Limits` with `MAX_READ_WAIT` and `MAX_FRAME_BYTES`, `SessionOptions`, `open` and `ClientChannel`. Nothing calls them yet. |

Proposed:
> | `session_host` | The core session host's frozen foundations (see "Session Host" below): `HostContext` with its `ShutdownReport`, `EnvironmentSnapshot`, `Limits` with `MAX_READ_WAIT` and `MAX_FRAME_BYTES`, `SessionOptions`, `open` and `ClientChannel`, with the frame types a `ClientChannel` carries: `Frame`, `Tag`, `Lane`, `SendError`, `Closed`, `FrameError`, `FrameSink` and `FrameSource`. `Limits` and `MAX_READ_WAIT` are re-exported from the `gwz-session-host` crate, and the frame types and `MAX_FRAME_BYTES` from `gwz-session-contract`. Nothing calls them yet. |

**2. Lines 126 to 130, the section's opening.**

Frozen:
> `session_host` holds the first interfaces of the core session host that the core session contract specifies (gwz-dev `dev-docs/GwzCoreSessionDesign.md`, built by steps CS1.4, CS1.5 and CS1.9 of `dev-docs/GwzCoreSessionPlan.md`). A driver opens a session with them. The channel's `send` and `recv` arrive with CS1.2, so a session cannot carry calls yet.

Proposed:
> `session_host` holds the first interfaces of the core session host that the core session contract specifies (gwz-dev `dev-docs/GwzCoreSessionDesign.md`, built by steps CS1.4, CS1.5 and CS1.9 of `dev-docs/GwzCoreSessionPlan.md`, and steps 3 and 4 of `dev-docs/GwzCoreSessionCrateMap.md`). A driver opens a session with them. Its `ClientChannel` sends and receives frames, but no session host answers them before the plan's Phase 2, so a session cannot carry calls yet.

**3. A new bullet after the `open(options)` bullet**, which itself is unchanged. Frozen: none. Proposed:
> - `ClientChannel` is the client end of the session's in-process channel (contract §3). `send(frame, lane)` never waits: on a full lane it refuses with `SendError::Full`, which gives the frame back, and the channel stays open. A control call, `operation.cancel` or `session.close`, goes on `Lane::Control`, whose reserve of `control_reserve` frames the outstanding-call limit never refuses; every other call goes on `Lane::Call`. `recv()` waits for the host's next frame or the session's end, which it reports as `Closed`. `close()` ends the session as dropping the `ClientChannel` does. A frame whose tag the in-process channel does not carry (it carries 1 to 3), or larger than `MAX_FRAME_BYTES`, ends the session. `ClientChannel` implements `FrameSink` and `FrameSource`.

The example block and every other bullet are unchanged.

## 7. The gate's re-freeze: CS1.4's accepted interface against the crate's

This is not `RustApi.md`, since everything on the left is crate-private in gwz-core. But CS1.4 froze it (dual review), so the review gets it item by item.

| CS1.4, accepted (`pub(crate)`, gwz-core) | Phase A (`pub`, gwz-session-host) |
| --- | --- |
| `CallControls::new(&Arc<SessionContext>) -> CallControls` | `CallControls::<S>::new(&Arc<S>) -> (CallControls<S>, OperationGate<S>)` |
| `CallControls::gate(&self) -> &OperationGate` | removed: the gate goes to the worker, and the record keeps `view(&self) -> GateView<S>` |
| `CallControls::cancel(&self)`, `revoke(&self)` | the same. `revoke` panics from the thread running this gate's closure, before it cancels |
| `#[derive(Clone)] OperationGate` | `OperationGate<S>`, not `Clone` |
| `effect`/`append(&self, impl FnOnce(GateScope<'_>) -> R) -> ModelResult<R>` | `effect`/`append(&mut self, impl FnOnce(GateScope<'_, S>) -> R) -> Result<R, Refused>` |
| `report(&self, …) -> Option<R>` | `report(&mut self, …) -> Option<R>` |
| `OperationGate::token(&self)`, `state(&self)` | the same, plus `view(&self) -> GateView<S>` |
| `GateScope<'a>::session(&self) -> &'a SessionContext` | `GateScope<'a, S>::session(&self) -> &'a S`, not `Clone` |
| none | `GateView<S>`: `Clone`, `state()`, `token()` |
| `#[derive(Clone)] HandlerContext`; `gate(&self) -> &OperationGate`; `token(&self)` | `HandlerContext<S>`, not `Clone`; `gate(&mut self) -> &mut OperationGate<S>`; `token(&self)`; `view(&self)` |
| `CancellationToken::on_cancel(impl FnOnce() + Send + 'static) -> ModelResult<CancelRegistration>` | `-> Result<CancelRegistration, Refused>`. The callback still receives nothing (see below) |
| `GateState { Live, Cancelled, Revoked }` | the same, plus `Hash` |
| errors: `ModelError { code: Cancelled }` with "the operation's token is cancelled" or "the operation's gate is revoked" | `Refused::Cancelled` or `Refused::Revoked`, with the same texts |
| `thread_local CROSSING`: any crossing or revoke, of any gate, inside any closure or callback, panics | capabilities, plus a per-gate `crosser` record: a revoke, or a crossing, of a gate from the thread running its closure panics. A crossing of another gate from a closure is not detected: the map's stated residual |
| none | `Debug` on the gate types, showing the state only; `GateScope` has none |

**Deviation for the review: cancel callbacks receive nothing.** Map §2 says a cancel callback receives "only a `GateScope`". Phase A keeps CS1.4's `FnOnce()`, which is narrower and guards the same thing: neither can cross or revoke. The reasons:
- every planned callback (CS3.7's runtime signal, §5.8's helper kill, §5.1's lock-wait wake) needs no session data;
- CS3.7's callbacks may not report;
- a scope needs the session alive when the token is cancelled, which a weakly held session cannot promise;
- callbacks run on the canceller's thread (the reading thread, for `operation.cancel`), where session data invites a session lock.

If the review wants the literal text, the change is local: `on_cancel(impl FnOnce(Option<GateScope<'_, S>>) + Send + 'static)`, with `None` once the session has ended, and a token generic over `S`.

**A `'static` callback can still own a gate** if the worker moves its own gate into it: `on_cancel(move || gate.report(..))` compiles (phase A probe). The worker then has no gate, and the callback's crossing cannot deadlock, since the backstop covers re-entry. Round 2's "a `'static` callback can no longer own a crossing capability" should read "can own one only by taking the worker's".

## 8. The allowlist, the inventory and registration

- **`process_globals_allowlist.json`.** B4 removes the one `session_host` entry, `{"path": "src/session_host/gate.rs", "kind": "thread_local", "name": "CROSSING", "disposition": "debt", "owner": "crate map §6 step 4 (gwz-session-host, capability design)"}`, in the commit that deletes `gate.rs`. The crate adds none: phase A's scan of it lists no item.
- **`cfg_boundaries_allowlist.json`.** No change. It has no `session_host` entry, and the crate declares its test modules as `mod tests;` over files whose first line is `#![cfg(test)]`, an inner attribute that the check passes.
- **`local_clone_inventory.json`.** B1 adds:
  ```json
  "gwz-session-host": {
    "directory": "session-host", "role": "integration", "owner": "CS",
    "rationale": "The session host's gates, limits and supervisor, generic over core's per-session data, which passes through opaquely; Phase 2's serve and HostPorts land here (GwzCoreSessionCrateMap §2).",
    "first_party": ["gwz-ids", "gwz-session-contract"], "third_party": [],
    "dev_first_party": [], "dev_third_party": [], "expected": "present"
  }
  ```
  `gwz-ids` is allowed by the map's row but not declared yet, because nothing moved needs unique numbers. The owner lane is the operator's to name; `C` follows `gwz-ids`.
- **`check_crate_versions.py`.** `EXPECTED_CRATES` +1. The release bump and the publish order are derived from `crates/*`, and phase A's run puts `gwz-session-host` after `gwz-session-contract`.
- **Bazel.** Add the crate's `BUILD.bazel` (phase A drafted it, not run under Bazel), its `MODULE.bazel` manifest line, and the regenerated `MODULE.bazel.lock`. The three session crates join gwz-core's `_LOCAL_CLONE_LIBRARIES` once core depends on them (B2, B5).
- **crates.io.** `gwz-session-host` is one of the thirteen names that step 5 bootstraps.

## 9. `GWZDesign.md`, "Core session host" (B6)

**Paragraph 1.** After "in-process for embedded clients, or a byte stream for a separately hosted core." insert:
> The frame layer and both adapters are the gwz-session-contract and gwz-session-channel crates, which carry bytes only. Each frame travels on a lane that core classifies: `operation.cancel`, `session.close` and their replies on the control lane, whose reserve the outstanding-call limit never refuses, and every other call and its reply on the call lane. A reply takes its call's lane, so the reply queue never overflows while the client keeps its bounds.

**Paragraph 2.** After "A worker reaches session state and resources only through its gate, which refuses effectful requests after cancellation and ignores reports after revocation." insert:
> The gates, the session limits and the host context's supervisor are the gwz-session-host crate's, generic over core's per-session data, so the environment snapshot never leaves core. The host context stays a core composite, and core's class table and dispatch implement the host crate's ports. Crossing and revoking are capabilities: the call's record holds the only controls that cancel and revoke, the worker holds the only gate that crosses, and neither can be cloned. A crossing's closure receives only a scope over the session's data. A gate records the thread running its closure, so a re-entry from that thread panics instead of deadlocking.

If step 1 has not done so by then, replace in the same paragraph "other than inventoried permanent entries, which carry no session-relevant state or are imposed by a dependency" with:
> other than inventoried permanent entries, which are immutable data, caches of immutable data, or state a named dependency imposes; no counter, flag or thread-local is ever permanent

That is map §1 and §7's O9 edit.

## 10. Other text that falls due (map §7), drafted at B6

These were drafted as `step4/contract-rev6.patch`, and the coordinator applied it on 2026-09-29 as the contract's status bullet and changelog entry. `GWZDesign.md` and `GWZRequirements.md` also took the O9 sentence that the contract defers.
- **Contract §3 and §9:** `send` takes a lane, and core classifies each frame. The frame layer and both adapters are the two session crates.
- **Contract §5.1 and §5.2:** the class table and dispatch are core's implementation of the host crate's ports (`HostPorts`).
- **Contract §5.6:** `HostContext` stays a core composite, over members whose machinery may live in crates, such as the host crate's supervisor.

The patch adds a status bullet naming the amended sections, and a changelog entry dated 2026-09-29. It keeps the contract's convention of recording an amendment by status bullet, not by bumping the revision number.

Still due: the map's other two contract items, O9 and §5.7's row of ID counters. The status bullet leaves them to the contract's next revision, under the map's own rule that it controls until then.

The session plan takes status edits only, since map §7 carries its changes until its next revision: its §3.0 and §5.4 still name `thread_local CROSSING` as a `permanent` example.

## 11. Risks

- **The re-freeze:** the capability API and the callback deviation above.
- **Undetected paths:** smuggled controls, a worker's gate moved into a callback, and a closure blocking on another thread's crossing. Review checks for each; no mechanism does.
- **`test-support`:** only core's dev-dependency may enable it. The boundary gate sees the edge but not the feature, so review keeps it out of `[dependencies]`.
- **Timing:** the supervisor tests keep CS1.9's 300 ms bound and 700 ms slack, and the revocation test keeps its 100 ms negative wait. Watch them on loaded CI (CS1.9 verdict).
- **Doctests:** Tier A ran only `--lib`, which excludes them; after the review (Consistency P3-3) it also runs `--doc`, so the four `compile_fail` examples, which pin the E0499 no-nesting borrow and a scope without crossing or revoking, run in CI. Stable rustdoc ignores their pinned error codes; phase A checked each code under nightly and in a probe crate. The `NotClone` checks are unit tests and run in Tier A.
- **Size:** `context.rs` shrinks by about 150 lines in B3, which eases the CS1.9 verdict's size warning.
- **A callback that revokes another operation's gate** (the Safety review's §2): run from the reading thread under the table lock, while that gate's closure wants the lock, it deadlocks undetected, where CS1.4's thread-local would have panicked. Nothing calls the gate yet. CS2.3, CS2.12 and CS3.7 keep two rules: callbacks never hold controls, and the reading thread runs callbacks holding no session lock.
- **gwz-py lands first or together:** gwz-py's CI runs gwz-core `main`'s process-globals checker, which requires the `owner` field that gwz-py's allowlist gains in the same change.

## 12. What phase B did against this plan

**The coordinator's decisions:**
1. The owner lane is "CS", as for the other two session crates.
2. Cancel callbacks keep CS1.4's `FnOnce()`. The crate's README carries the rationale and the literal alternative.
3. B5 is in scope, and `RustApi.md` takes §6's text exactly.
4. `GWZDesign.md` takes §9's edits, the O9 sentence included. The contract's §3, §9, §5.1, §5.2 and §5.6 edits were drafted as a patch, which the coordinator applied on 2026-09-29.
5. `gwz-ids` is listed in `first_party` only if the gate accepts an allowed edge that is not declared. It does: a scratch inventory with it passed the real gate before the real one took it.
6. The `compile_fail` doctests stay, and CI is not changed. (Reversed by the review's Consistency P3-3: Tier A now also runs `--doc`.)

**Departures from the text above, each already folded in:**
- Step 3's tests moved into their lib targets, so that CI runs them (§0).
- B1 found more count pins than the plan named: the version and boundary gates' own tests, the release scripts' tests and the crates.io plan's layers (B1). `MODULE.bazel.lock` was left for one regeneration with steps 2 and 3.
- The two error mappings share one file, `errors.rs`, begun in B2, instead of a `refusal.rs` of B4's.
- `test_check_process_globals.py` pinned the `CROSSING` entry and the list's size (B4).
- B5's closure test closes the channel instead of dropping it (B5).
- `open` took its channel limits from `validate_limits`, as §5 sketched. Core's tests import `Tag` themselves, since the module imports only what it uses.

**The review, 2026-09-29:** Consistency and Safety both reported GO on steps 1 to 4, with eight P3s between them ([Consistency](GwzCoreSessionCrateMapSteps-ReviewConsistency.md), [Safety](GwzCoreSessionCrateMapSteps-ReviewSafety.md)). One is a deviation this plan now records: until CS1.6 lets core classify a frame, `ClientChannel::send(frame, lane)` takes the lane its caller classifies (Consistency P3-1; the contract's 2026-09-29 status bullet and `RustApi.md` say so). Two corrections changed what §5 describes: Safety P3-2 replaced `ClientChannel`'s `host_end` and `session` fields with `held: Mutex<Option<Held>>`, which `close()` takes, so the host end and the session context, snapshot included, drop at the close; and Safety P3-1 keeps a byte-stream body behind a guard that wipes it on every exit but delivery. Both reviewers confirmed the corrections ([Consistency-1](GwzCoreSessionCrateMapSteps-ReviewConsistency-1.md), [Safety-1](GwzCoreSessionCrateMapSteps-ReviewSafety-1.md)).

**What stays open:** `MODULE.bazel.lock`.
