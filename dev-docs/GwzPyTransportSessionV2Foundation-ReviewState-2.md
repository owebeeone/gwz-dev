# Python transport session v2 foundation — STATE-AXIS RE-REVIEW

**Review object:** Corrected candidate foundation at root `bf5c7270d9e8e098d131e20392713267160c1600`, gwz-core `eebd41bab1fe7a91249a55b48c39237d8c6c1671`, and gwz-py `fe9d6797148cf71315487817d36396d042214a6e`; candidate remains disabled.  
**Baseline:** Original candidate root `9cb11fd5d561ee108525357cdaafc11af611a137`, core `dfb7533ba8cd2eba59fc1d8ba8365a74b7413371`, Python `33a1f3f4e2ab9a7b97fd03246f7bb3f1905dfead`. Original foundation baselines were root `78a46ef58bdfc3fac0c5a6600597465c9436c20d`, core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`, Python `cddb38204fbbd11808cc3c414807aa66a5910ce0`. Sources were read with `git diff`, `rg`, `sed`, and `cat`.  
**Date:** 2026-09-24  
**Axis:** State machines, races, cancellation, close, fault recovery, and fail-closed direction. Independent, adversarial, read-only. The other axis ran independently; nothing here relies on its report. Filed verbatim by the lane owner.

**Verdict: NO-GO** — four P2 findings block: original P2-3 remains partially open; P2-5 through P2-7 are distinct newly identified roots. I pre-commit to GO on a revision that resolves P2-3 and P2-5 through P2-7 as specified, provided the corrected tree introduces no new blocker.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| P1-1 | Bind the issued ID to queued call/submit and retain cancellation authority. | `bridge.py:330–385` passes the reserved ID; `transport_session.rs:296–322,897–946` refuses the cancelled ID without reminting. The real-native blocked-executor test at `test_transport_session_native.py:169–207` covers both methods and checks the next serial. | Closed for the original counterexample. |
| P2-1 | Clear construction state on unwind; stop admission after accepted panic. | `transport_session.rs:367–402` clears `constructing`, closes, and wakes waiters on construction panic; `:746–792` marks the generation faulted and cancels peers; `:256–274,815–895` reject new work and make close return a conservative report. | Closed for the original counterexamples; P2-5 is a separate post-acceptance panic/publication root. |
| P2-2 | Record an ordinary dispatch error before transport finish, publication, active removal, and completion. | `transport_session.rs:641–677` records dispatch failure before `request.finish()`, publishes before removing `active`, and then signals `done`; `release_inner` rejects active records. | Closed for the original ordinary-error interleaving; P2-5 concerns a later cleanup panic with a pending success. |
| P2-3 | Settle a failed thread spawn as a retained typed pre-effect refusal and wake waiters. | `dispatch/mod.rs:511–520` now writes a terminal, and `:529–543` tests that narrow property. It writes a generic failed `OperationResult` and returns an untyped runtime error, rather than retaining the required typed pre-effect refusal. | Partially fixed; blocking remainder P2-3 below. |
| P2-4 | Start one cancellable five-second deadline before admission leadership and carry it through installation. | `session.rs:397–419,448–449,453–474,578–603` uses a shared deadline. The held-leadership test at `tests.rs:82–102` checks timeout and same-ID retry before gate acquisition. | Closed for the original unbounded gate wait; P2-6 and P2-7 concern different, later capacity transitions. |

## Changed-range analysis

The correction changed root remediation/checkpoint material, core model/protocol projection plus `transport_host/session.rs` and its tests, and Python native dispatch, operation store, session, bridge/error routing, and focused tests. I read the root remediation plan and prior State report, not the other reviewer's report. The Python queued-worker test now exercises the real native call and submit paths. The core timeout test holds leadership until timeout; it does not exercise the interval after registration. The spawn test asserts a failed result, not a typed refusal. There is no focused construction-panic, post-acceptance cleanup-panic, or mid-install cancellation test in the inspected candidate tests.

Two **NEW ARCHITECTURAL root causes** are exposed at core's capacity transaction boundary: P2-6 registers a lifetime request ID before all fallible capacity steps pass, and P2-7 has no rollback/fail-closed guard if an installation future is dropped after mutating pools. P2-5 is a separate native terminal-precedence defect: a pending success wins over a later cleanup panic. P2-3 is the original finding's remaining typed-refusal requirement. None is merely absence of the explicitly deferred byte ledger, timer, public handles, rollover, or platform gates.

## 0. Evidence base

I read `dev-docs/GwzPyTransportSessionV2Design.md` §§2, 4–6, `dev-docs/GwzPyTransportSessionV2ImplementationCheckpoint.md`, `dev-docs/GwzPyTransportSessionV2Foundation-RemPlan.md`, and the prior State report at root HEAD. I inspected the corrected diffs and current lines in `gwz-core/src/transport_host/{session,request}.rs`, `gwz-core/src/transport_host/tests.rs`, `gwz-py/native/src/{transport_session,operations,dispatch/mod,error}.rs`, `gwz-py/src/gwz/{bridge,errors}.py`, and focused native/API tests. The exact three-repository tuple was checked with `git rev-parse HEAD` at both start and end and did not move. I ran no builds or tests, as required by the read-only review restriction. Test references below describe coverage visible in source, not a claimed fresh pass.

The checkpoint expressly defers the full 64 MiB ledger, 15-minute expiry, public `start_*` handles, generation rollover, and platform gates. Their absence is outside this foundation verdict.

## 1. Findings

### [P2-3] Failed native worker launch still loses the typed pre-effect refusal

**Location:** `gwz-py/native/src/dispatch/mod.rs:505–520,529–543`; `gwz-py/native/src/operations.rs:216–239,315–343,425–452`; `gwz-py/native/src/error.rs:8–15`.

**Violated invariant:** The accepted design §5 and remediation plan require a failed spawn to terminalize the already-issued ID with a *typed pre-effect refusal*, effect `none`, before returning or freeing admission. The retained result path must make that refusal inspectable, and event/result waiters must wake.

**Counterexample:** Inject one `thread::Builder::spawn` error for a submitted network operation. `worker_spawn_failure` calls `recorder.finish_error`, which stores a generic `OperationResult` with `InternalError`, then returns `error::runtime(message)`, a `PyRuntimeError` without the model `code` attribute. Native submit removes the admission entry. The result waiter terminates, but `operation_result(id)` returns a failed operation result instead of raising the same typed pre-effect refusal. The focused unit test explicitly checks only `aggregate_status == Failed`; it does not check the refusal type or no-effect meaning.

**Impact:** Callers cannot reliably distinguish a worker that never started from a possibly effective failed operation using the retained outcome. This defeats the design's retry and recovery grammar for an ordinary resource failure, even though the former infinite wait is fixed.

**Required correction:** On spawn failure, store one `ModelError` refusal in the issued session record and return its typed Python projection. Preserve its pre-effect attribution; prevent the outer generic recorder from replacing it. Wake result/event waiters before removing admission and return the slot.

**Closure test:** Inject spawn failure on `submit`; assert no `Accepted`, no core/Git effect, one retained typed refusal with the chosen model code and effect `none`, prompt result/event completion, and recovery of the worker slot.

### [P2-5] Cleanup panic can publish a prepared success without completed transport finish

**Location:** `gwz-py/native/src/transport_session.rs:641–677,746–792`; `gwz-py/native/src/dispatch/mod.rs:481–500`; `gwz-py/native/src/operations.rs:216–239,400–418`.

**Violated invariant:** An accepted operation may publish success only after `TransportRequest.finish()` completes. A worker panic without proved finish must retain an attributed failure and conservative cleanup, never a success that implies completed transport settlement.

**Counterexample:** Let dispatch successfully record an operation result while terminal publication is deferred. Inject a panic in `request.finish()` at `transport_session.rs:666`. The outer worker catch calls `recorder.finish_error`, but `OperationRecord::complete` rejects it because the success already occupies `pending_terminal`; that error is ignored. `worker_panicked` then calls `publish_terminal` and makes the prepared success visible, removes the active entry, and wakes joins with an unconfirmed cleanup report. A caller can read success by ID although transport finish never completed.

**Impact:** The result ledger can invent a safe completion in the exact accepted-panic path that should be fail-closed. A push caller may make an unsafe replay or cleanup decision from contradictory success and unconfirmed physical state.

**Required correction:** Treat post-acceptance unwind as a terminal failure unless finish was proved. A panic path must supersede or invalidate a deferred success before publication; the finish/terminal state should be committed once, with success permitted only after finish. Preserve the conservative cleanup report and wake all waiters.

**Closure test:** Fault-inject an unwind after a successful dispatch has staged its result but before/during `TransportRequest.finish()`. Race result, cancel, close, and release; assert one attributed failure, no success result, no early join, and no further admission to the faulted generation.

### [P2-6] A late capacity timeout consumes a request ID before admission passes

**Location:** `gwz-core/src/transport_host/session.rs:397–419,448–474,609–625`; `gwz-core/src/transport_host/request.rs:352–366`.

**Violated invariant:** Design §§2 and 5 state that a pre-registration capacity refusal does not consume the caller's request ID and that capacity passes before core registration. The lifetime-used set must grow only once the capacity prerequisite succeeds.

**Counterexample:** Hold admission leadership until just before B's five-second deadline, then release it. B acquires the gate while time remains, passes the preliminary state check, and `ClientRequest::new` inserts its ID into `state.used`. Before or on entering `install_capacity`, the deadline expires. The first poll at `session.rs:466` returns a capacity timeout; dropping `ClientRequest` cancels/seals the registration but never removes it from `used`. A retry with the same ID is refused as already registered. The new timeout test holds the gate until *before* registration and therefore misses this boundary. The same ordering also affects later fallible retirement or authority installation.

**Impact:** A capacity failure that performed no operation consumes a scarce generation ID and removes the promised same-ID retry path. Repeated near-deadline failures can accelerate exhaustion of the 256-ID generation before the deferred rollover exists.

**Required correction:** Make capacity validation/installation a pre-registration phase with a provisional admission token; commit the lifetime ID only after that phase passes. Keep the capacity leader's serialization and the single arrival deadline. If the installation cannot be rolled back, fail the generation closed rather than reporting a retryable pre-registration refusal while keeping the ID.

**Closure test:** Pause B after admission leadership and before the fallible capacity step, advance past its arrival deadline, then resume. Assert timeout within the bound, no core ID registration, and successful same-ID retry after the conflict clears; repeat for retirement failure.

### [P2-7] Dropping a staged capacity installation leaves an open, partially updated generation

**Location:** `gwz-core/src/transport_host/session.rs:548–603`, especially `:558–578`; `gwz-core/src/transport_host/request.rs:360–366`.

**Violated invariant:** Design §4 requires partial physical capacity changes whose rollback cannot be proved to fail closed and drain. Cancellation must not leave paired pools and shared reservation authority in different capacity epochs while the session remains open.

**Counterexample:** Admit a different policy once previous work and leases are quiescent but pool retirement still has closing resources. `install_capacity_pair` (or single-pool install) mutates pool policy, `state.installed_capacity` becomes `None`, and the future waits at `retired = poll_fn(...).await`. Drop the admission future at that pending point (for example, cancellation of a direct core request or unwind of its owning worker). There is no drop guard around the staged transaction. `CapacityLeader` and `AdmissionLeader` only release gates and signal; `ClientRequest` Drop cancels/seals the logical request. The shared authority is not updated and `Session::close()` is not called, so the generation remains open with a partially committed physical policy. The explicit error branch would close the generation, but a dropped future never enters it.

**Impact:** Subsequent admission can encounter an open generation whose pool and authority limits disagree. The resulting behavior depends on a later request repairing the state; the cancellation itself has not failed closed as promised.

**Required correction:** Add a transaction/drop guard spanning pool mutation through authority installation and final `installed_capacity` publication. On cancellation after mutation, prove rollback or close and drain the generation; wake waiters. Keep pre-mutation cancellation free of request-ID consumption under P2-6.

**Closure test:** Hold the retirement wait after pool mutation, poll the request to pending, drop its future, then assert the generation closes/drains conservatively, new admission does not reuse the partially updated host, and waiters finish. Verify the paired pool and authority policy on the successful path.

## 2. Invariant analysis

The real-native blocked-executor test and explicit operation-ID parameter now defeat the former undisclosed-remint sequence for both call and submit. A cancelled reservation remains mapped until the delayed worker checks `last_cancel`; the worker refuses before core registration. Separate native session stores and nonce/high-water validation still prevent the cross-Client identity mixup in the inspected paths.

Construction panic now clears its wait flag and makes close return a conservative report. An accepted outer panic sets `faulted`, so new admission is refused; close cancels and joins peers. Ordinary dispatch errors are recorded before `request.finish()`, then terminal publication precedes active removal and `done` notification. Those attacks failed on the corrected tree. The pending-success cleanup-panic sequence in P2-5 bypasses that ordinary-error ordering.

The new atomic admission gate and arrival-time deadline bound the original gate wait; the core held-gate test covers that case and a retry before registration. Equal installed capacity still skips reinstall, and a differing live-operation policy still refuses before registration. The later registration and staged-install boundaries in P2-6 and P2-7 remain outside those successful checks.

## 3. Risks and next action

The remaining deferred ledger, timer, public-handle, rollover, and platform work is not a foundation finding. The current focused tests do not yet fault-inject the accepted cleanup panic or staged-install cancellation, and no test was run during this read-only review. The next action is to correct P2-3 and P2-5 through P2-7, add the specified transition tests, and request a State re-verdict on a new exact tuple before building the deferred layers on this foundation.
