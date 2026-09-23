# Python transport session v2 foundation — STATE-AXIS REVIEW

**Review object:** Candidate foundation diff at root `9cb11fd5d561ee108525357cdaafc11af611a137`, gwz-core `dfb7533ba8cd2eba59fc1d8ba8365a74b7413371`, and gwz-py `33a1f3f4e2ab9a7b97fd03246f7bb3f1905dfead`; status: candidate, not accepted.  
**Baseline:** Root `78a46ef58bdfc3fac0c5a6600597465c9436c20d`; gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; gwz-py `cddb38204fbbd11808cc3c414807aa66a5910ce0`. Sources were inspected with `git diff`, `git show HEAD:`, `rg -n`, and `sed`.  
**Date:** 2026-09-24  
**Axis:** State machines, races, cancellation, close, fault recovery, and fail-closed behavior. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P1 and four P2 findings block. I pre-commit to GO on a revision that resolves P1-1 and P2-1 through P2-4 as specified, provided the corrected tree introduces no new blocker.

---

## 0. Evidence base

I read the accepted `dev-docs/GwzPyTransportSessionV2Design.md` §§2, 4–6 and `dev-docs/GwzPyTransportSessionV2ImplementationCheckpoint.md` at root HEAD. I inspected the baseline-to-HEAD diffs and current source in `gwz-core/src/transport_host/{session,mod,request}.rs`, `gwz-py/native/src/{transport_session,operations,dispatch/mod}.rs`, `gwz-py/src/gwz/{bridge,client}.py`, and the focused candidate tests. The exact three-repository tuple was verified with `git rev-parse HEAD` at both the start and end; it did not move. No tests or builds were run under the read-only command restriction.

The checkpoint expressly defers the full byte ledger, timed expiry, public `start_*` handles, rollover, and platform gates. Their absence is not a finding here.

## 1. Findings

### [P1-1] Early cancellation can restart the request under a new, undisclosed operation ID

**Location:** `gwz-py/src/gwz/bridge.py:345`, `:354–375`; `gwz-py/native/src/transport_session.rs:284–289`, `:726–731`, `:413`.

**Violated invariant:** Cancelling an issued operation before native admission must terminalize that operation with no effects. The worker must never reissue the same pending request under another public ID.

**Counterexample:** `_run_native` reserves ID A, creates an `asyncio.to_thread` task, and is cancelled before that task starts. `cancel_operation(A)` reaches the native branch for a retained record with no `admitting` or `active` entry. It removes A’s `request_operations` mapping and returns a default cleanup report. When the queued worker starts, `call_inner` invokes `operation_for_request`; because the mapping is gone, it issues ID B and proceeds to core registration and Git work. `_run_native` waits for that worker but ultimately raises `CancelledError`, discarding its response. For a push, remote effects can occur under B although cancellation targeted A, and the caller was never given B. The existing Python test at `src/tests/test_transport_session_api.py:299–355` uses a fake session whose `call` does not reproduce native reminting.

**Impact:** A cancelled push can run with an undisclosed identity and inaccessible outcome through the cancelled call. This defeats cancellation attribution and safe replay decisions.

**Required correction:** Bind the reserved ID to the queued native invocation through admission. Cancellation before worker start must mark that exact ID terminal and prevent the queued worker from issuing or executing another operation. Keep the request mapping until the worker acknowledges the cancelled state, or pass the ID explicitly and reject a cancelled token before registration.

**Closure test:** Block the default executor before a real native `push` or `fetch` worker starts; cancel the Python task; release the executor. Assert one issued ID, no second serial, no core registration or remote effect, and a retained terminal outcome for the original ID. Repeat with many cancellation interleavings.

### [P2-1] Outer worker panic leaves either a stuck construction state or an open generation with unconfirmed cleanup

**Location:** `gwz-py/native/src/transport_session.rs:338–356`, `:586–613`, `:633–695`; `gwz-py/native/src/dispatch/mod.rs:466–493`.

**Violated invariant:** Every worker unwind must lead to a closed, joinable state. Unconfirmed physical cleanup must prevent further admission.

**Counterexamples:** If `TransportRuntime::from_environment()` panics after `constructing` becomes `true`, the outer worker catch calls `worker_panicked`, which removes admission entries but does not clear `constructing`. `close_inner` then waits forever for `!state.constructing`. If an accepted worker panics outside the narrower dispatch catch, `worker_panicked` reports `pending_local_work=1` and `peer_cleanup_confirmed=false`, removes the active entry, but leaves `Status::Open` and the runtime available. A subsequent request can enter the same generation despite unconfirmed cleanup.

**Impact:** One fault can make close nonterminating; another permits work in a generation whose physical state is unknown. The latter contradicts the checkpoint’s fail-closed characterization.

**Required correction:** Give construction and accepted workers unwind guards that always settle their state flags and completion. On an accepted panic without proved `TransportRequest.finish()`, move the session to a draining/closed state, cancel peers, and prevent new admission until physical shutdown completes or is reported unconfirmed.

**Closure test:** Inject a panic during runtime construction and verify close terminates with a conservative report. Inject a panic after acceptance but before transport finish; verify every waiter wakes, no new request is admitted to that generation, and the retained terminal identifies the original operation.

### [P2-2] Cancellation and close can join before a failed operation’s result is published

**Location:** `gwz-py/native/src/transport_session.rs:529–540`, `:697–757`; `gwz-py/native/src/dispatch/mod.rs:482–493`; `gwz-py/native/src/operations.rs:204–213`.

**Violated invariant:** A completed cancellation or close join must imply that the attributed terminal result is already retained. A live record must not become releasable before publication.

**Counterexample:** A dispatched Git operation returns an error before its recorder creates a result. `call_inner` finishes transport, calls `publish_terminal` while no result is pending, removes the active entry, and signals `done`. Only after `call_inner` returns does the outer `spawn_call` branch call `recorder.finish_error`. A cancellation waiter can therefore return while `try_operation_result` is still `None`; close can also finish while the terminal is absent. A concurrent `release_operation` observes no active entry, discards the record, and the later `finish_error` writes only to the recorder’s detached `Arc`, losing the retained result by ID.

**Impact:** Possible-effect failures have a publication gap precisely when callers need their final evidence. The loss after release is an allowed interleaving of the current state machine, not a byte-ledger deferral.

**Required correction:** Make terminal recording, transport-finish completion, active removal, and wakeup one ordered transition. The outer dispatcher must provide its failure to the session before `done` and close waiters can finish; release must reject the record until that publication is settled.

**Closure test:** Pause the worker between transport finish and outer failure recording. Race `cancel_operation`, `close`, `try_operation_result`, and `release_operation`. Assert cancel/close do not complete before one retained terminal exists; release reports `OpenOperation` until publication and then follows the documented terminal-record rule.

### [P2-3] Native worker spawn failure strands an issued record without a terminal

**Location:** `gwz-py/native/src/dispatch/mod.rs:395–437`, `:466–501`; `gwz-py/native/src/transport_session.rs:889–907`; `gwz-py/native/src/operations.rs:406–413`.

**Violated invariant:** A fallible worker launch must terminalize its issued record before returning a pre-effect refusal.

**Counterexample:** `submit_accepted` calls `operations::begin`, then `spawn_call`. If `thread::Builder::spawn` fails, its error is returned without invoking `recorder.finish_error`. Native `submit` removes the admission entry and returns the error, but the issued `OperationRecord` has no result. `operation_result(id)` waits indefinitely, and the record continues to count against the 64-record ceiling until explicit release.

**Impact:** An ordinary resource failure creates a retained ID with no terminal state, contrary to the checkpoint’s claimed pre-acceptance error recording and the design’s closed outcome grammar.

**Required correction:** Settle the issued record with an attributed, typed pre-effect refusal on launch failure before removing admission or returning to Python. Ensure result and event waiters are woken.

**Closure test:** Inject one native thread-spawn failure for `submit`; assert no Git effects, no `Accepted` response, one terminal result or retained refusal for the issued ID, prompt waiter completion, and recovery of the admission slot.

### [P2-4] Capacity admission wait has no five-second bound

**Location:** `gwz-core/src/transport_host/session.rs:385–410`, `:421–440`, `:539–570`.

**Violated invariant:** The accepted design gives equal-target capacity waiters a cancellable five-second bound.

**Counterexample:** `admit_client_request` awaits `admission_gate.lock()` without a deadline, then holds that mutex across the entire capacity installation and retirement waits. The five-second check in `install_capacity` starts only after acquiring this outer mutex. A request queued behind one or more installations can wait longer than five seconds before its own deadline even begins. The serial admission mutex also delays equal-capacity requests that require no reinstall.

**Impact:** Admission latency is unbounded by the stated five-second contract, and a slow physical retirement holds up otherwise compatible work.

**Required correction:** Apply one deadline that starts before waiting for admission leadership and covers all installation stages. Permit already-installed equal-capacity admission without waiting behind unrelated retirement where the shared physical state is stable, while preserving serialized capacity mutation.

**Closure test:** Hold a capacity leader in retirement and queue equal-capacity waiters. Assert each waiter either joins the stable equal policy or returns within five seconds of its own arrival; cancellation must promptly remove a waiter without consuming its core request ID.

## 2. Invariant analysis

The core change does close one important race: `admit_client_request` checks a differing capacity before registration under serialized local admission, and an equal installed capacity returns without reinstalling pools. The existing between-leases core test exercises a different-capacity refusal and same-request-ID retry. The native session also gives separate Clients random-nonce IDs and separate operation stores; lookups validate nonce and high-water serial before reading the session store. Those paths did not yield a cross-Client read in this review.

The new eight-operation count check excludes merely issued unstarted IDs, as the design requires, and `submit` waits for a registration signal before returning its `Accepted` envelope. These properties hold on the ordinary path. They do not settle the queued-worker cancellation, unwind, or terminal-publication interleavings above.

The acknowledged 64 MiB accounting, 15-minute timer, public handle surface, and 256-ID rollover work remains outside this verdict. No claim of their completion is made.

## 3. Risks and next action

The highest-risk sequence is P1-1 because a cancelled push can acquire a second, undisclosed identity. The single next action is a consolidated correction of these five state transitions, followed by focused fault-injection and race tests and a State re-verdict on the new exact tuple.