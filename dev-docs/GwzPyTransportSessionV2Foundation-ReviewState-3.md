# Python transport session v2 foundation — STATE-AXIS REVIEW

**Review object:** Foundation diff from root `9cb11fd5d561ee108525357cdaafc11af611a137`, gwz-core `dfb7533ba8cd2eba59fc1d8ba8365a74b7413371`, and gwz-py `33a1f3f4e2ab9a7b97fd03246f7bb3f1905dfead`, focused on the second correction since root `bf5c7270d9e8e098d131e20392713267160c1600`, core `eebd41bab1fe7a91249a55b48c39237d8c6c1671`, and Python `fe9d6797148cf71315487817d36396d042214a6e`.  
**Exact reviewed tuple:** root `d957584c8f98bc7f67dd0a2e303f0ff3350e306e`; gwz-core `351a5c565783c53a4e9ae702fe99a699c55299fd`; gwz-py `20b86187c766079a3a85d67a3d4d58d7edb73d41`. Verified at start and end without movement.  
**Date:** 2026-09-24  
**Axis:** State machines, races, cancellation, fault recovery, and fail-closed behavior. Independent, adversarial, read-only.

**Verdict: NO-GO.** Two P2 findings remain. The first is a **third new architectural root cause** on this foundation: Python infers whether core registered an ID from an error code, but the same code can arise on either side of registration. The review-loop cap therefore calls for stopping this lane for a redesign-or-accept decision rather than another corrective patch.

## 0. Evidence base

I read the controlling design, the second remediation plan, the prior State report, the relevant core and native source, and their focused tests. I inspected the correction diffs and traced the current call paths. I did not read or rely on the other reviewer’s current-round report. I ran no build or tests under the read-only mandate; the cited tests are source coverage, not fresh results. The existing unrelated dirty root files and candidate directories were outside the review object.

The full byte ledger and timer, public `start_*` handles, rollover, SSH/HTTPS activation, and cross-loop, performance, and platform gates are deferred and are not findings here.

## 1. Findings

### [P2-8] Python retains a request-ID mapping after a pre-registration capacity timeout

**Location:** `gwz-core/src/transport_host/session.rs:449–511,514–538`; `gwz-core/src/transport_host/mod.rs:185–236,344–346`; `gwz-py/native/src/transport_session.rs:268–299,605–627`.

**Invariant:** A refusal before core registration consumes no generation request ID. A new operation may retry that same caller request ID after the conflict clears, as the design §2 specifies. The Python mapping must reflect the actual registration boundary.

**Counterexample:** Reserve operation A for request ID `r`. Hold the core capacity gate until A’s arrival deadline expires. Core returns `IoError` from `install_capacity` before `ClientRequest::new` registers `r`; the new core test at `tests.rs:105–125` verifies that `r` remains retryable in core. Python’s `call_inner` only removes its `request_operations[r]` mapping for `TransportCapacityConflict`, `InvalidRequest`, or `UnsupportedOperation`. It retains the mapping for this `IoError`, terminalizes A, and returns. A new `reserve_operation("r")` then fails as a duplicate. Calling without a new reservation reuses A’s already-refused operation ID, so that route cannot retry either.

**Impact:** A recoverable five-second capacity refusal permanently blocks that request ID in the Python Client, despite no core registration or Git effect. The client may need to be discarded to recover its intended identity.

**Root cause and correction:** This is a **new architectural root cause**: Python uses an error-code allowlist as a proxy for registration provenance. `IoError` can also arise after registration during `begin` or `ready`, so adding it to that allowlist would permit an unsafe same-ID retry. Core must return explicit registration or effect-phase evidence to Python, or the admission API must make pre-registration refusal and post-registration failure distinct by construction. Remove the mapping only on proved pre-registration refusal.

**Closure test:** Through the native session, hold capacity leadership until timeout, inspect A’s retained typed refusal, release the gate, reserve a new operation with the same `r`, and complete its admission. Separately induce a post-registration `IoError` and assert that `r` remains consumed. The two cases must not be distinguished by message text.

### [P2-9] Errors before native admission setup leave issued records pending

**Location:** `gwz-py/native/src/transport_session.rs:469–501,1183–1224,1226–1251,1466–1495`; `gwz-py/native/src/operations.rs:267–291,465–475`.

**Invariant:** Once `reserve_operation` issues an ID, every attempted call or submit must either admit it or terminalize it with an inspectable pre-effect refusal. Result and event waiters must not require Client close to finish.

**Counterexample:** Reserve an ID, then invoke either native `call` or `submit` while the process working directory has been removed. Both methods call `std::env::current_dir()` before entering their admission logic. That error returns immediately without refusing the issued record. Its `operation_result(id)` waits indefinitely and `wait_events(id, …)` continues to report incomplete. Independently, malformed network request bytes fail `network_meta`; both paths call `abandon_unstarted(None)`, which cannot identify the reserved record and does nothing. These failures occur before core or Git effects.

**Impact:** Ordinary environment failure, or a malformed direct-native request, leaves a discoverable issued operation with no terminal evidence. A caller waiting for its result can hang until an unrelated close.

**Correction:** Carry the issued ID through every pre-dispatch fallible step. On a pre-effect exit, settle that exact record with a typed refusal, wake waiters, and remove its request mapping where registration is proved absent. Make the cleanup independent of successful request metadata decoding.

**Closure test:** Reserve an ID, inject `current_dir()` failure, and exercise both `call` and `submit`. Repeat with malformed network CBOR. In every case, assert prompt typed result refusal, completed event waiting, no core registration or Git effect, and a reusable request ID.

## 2. Invariant analysis

| Prior finding in RemPlan-2 | State-axis disposition on this tuple |
| --- | --- |
| Code P2-2, eight-slot refusals | Closed for the original counterexample. Both direct and submit full-slot exits call `refuse_full` on the issued ID. The focused test covers alternating forms and 65 released refusals. |
| Code P2-4, construction panic | Closed for the original counterexample. `runtime_with` refuses reserved records, signals admitting callers, clears construction, and closes with a conservative report. The focused test exercises those states. |
| State P2-3, failed worker spawn | Closed for the original counterexample. `worker_spawn_failure` stores and returns the same typed `IoError` refusal; event waiting ends. The focused test checks both projections. |
| State P2-5, cleanup panic after staged success | Closed for the original counterexample. `finish_panic_error` replaces an unpublished terminal, and `finish_after_dispatch` invokes the fault path before publication. The focused test proves a staged success becomes failure. |
| State P2-6, capacity failure consuming a core ID | Closed in core for the original counterexample. Capacity installation and the arrival deadline now precede `ClientRequest::new`; the core held-gate test retries the same ID. P2-8 identifies a separate Python state mismatch exposed by this boundary. |
| State P2-7, dropped capacity installation | Closed for the original counterexample. An armed `CapacityMutation` guard closes the generation if the future drops after pool mutation. The focused test drops at retirement wait and observes closed admission. |

The core admission and capacity gates retain one arrival deadline. A differing live physical policy still refuses before pool mutation, and equal capacity avoids reinstall. The mutation guard covers the pending retirement interval; its drop calls `Session::close()`, which shuts the endpoint and signals waiters. Native accepted-operation publication remains after `request.finish()`, with the revised panic path replacing staged success before publication. Those attacks did not yield another blocker in the inspected paths.

P2-8 is the **third new architectural root cause** after the prior State report classified P2-6 and P2-7 as two new architectural roots at the capacity boundary. P2-9 is a separate implementation-level settlement gap.

## 3. Risks and next action

The reviewed tuple is stable, but its Python retry state disagrees with core after a pre-registration timeout, and some issued IDs can remain pending on pre-dispatch errors. Both are foundation correctness failures.

Under the review-loop cap, the next action is to stop this lane and ask the operator for a redesign-or-accept decision on the registration-phase contract. Any resumed foundation should also settle every issued-ID early exit and receive a fresh State review on a new exact tuple.
