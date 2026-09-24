# Python transport session v2 foundation — CODE-AXIS REVIEW

**Review object:** Foundation diff from root `9cb11fd5d561ee108525357cdaafc11af611a137`, gwz-core `dfb7533ba8cd2eba59fc1d8ba8365a74b7413371`, and gwz-py `33a1f3f4e2ab9a7b97fd03246f7bb3f1905dfead` through the tuple below, focused on the second correction since root `bf5c7270d9e8e098d131e20392713267160c1600`, core `eebd41bab1fe7a91249a55b48c39237d8c6c1671`, and Python `fe9d6797148cf71315487817d36396d042214a6e`.

**Settled tuple:** root `d957584c8f98bc7f67dd0a2e303f0ff3350e306e`; gwz-core `351a5c565783c53a4e9ae702fe99a699c55299fd`; gwz-py `20b86187c766079a3a85d67a3d4d58d7edb73d41`. All three HEADs matched at the start and end of this read-only review.

**Date:** 2026-09-24  
**Axis:** Code — architecture, interfaces, call graphs, and compatibility reality.

**Verdict: NO-GO.** The six findings named in the second remediation plan are closed for their original counterexamples, but a distinct post-issuance refusal path remains broken. A normal `submit()` admission failure can return an error while the retained ID contains a generic execution result; other pre-worker errors can leave that ID without any terminal outcome.

## 0. Evidence base

I read the controlling design, implementation checkpoint, second remediation plan, and prior Code and State reports. I inspected the current core and Python native sources, their second-correction diffs, and the Python bridge call sites. Prior reports were legitimate remediation inputs; I did not inspect or request the parallel reviewer's current-round work. Inspection was read-only. I ran no build or test; the lane owner's reported 14 core, 13 native, and 33 Python test results are not a fresh claim by this reviewer. Existing dirty root documents and candidate directories were outside the review object.

Candidate activation, the full byte ledger and timer, public `start_*` handles, rollover, SSH/HTTPS and cross-loop proofs, and platform gates remain deferred. Their absence is not counted as a foundation defect here.

## 1. Findings

### [P2-8] Submitted pre-effect failures do not consistently settle the issued ID as a refusal

**Location:** `gwz-py/native/src/transport_session.rs:590–626,745–785,1241–1258,1313–1375`; `gwz-py/native/src/dispatch/mod.rs:384–441,483–507`; `gwz-py/native/src/operations.rs:215–239,430–454`.

**Invariant:** Once an operation ID is issued, a failure before `Accepted` must leave that ID with a retained typed pre-effect refusal. Result and event readers must terminate. The error returned by admission and the retained outcome must describe the same failure.

**Counterexample:** Reserve an ID and submit a valid network request while a live operation requires a different physical capacity. Core returns `TransportCapacityConflict` before registering the new request. In the spawned `call_inner`, `end_admission(..., defer_failure=true)` saves the model failure for the admission signal but returns before calling `OperationStore::refuse`. The outer worker then calls `recorder.finish_error`, which stores a generic failed `OperationResult` with `InternalError`. `complete_failed_admission` sends the typed capacity error to `submit()`. Thus `submit()` raises `TransportCapacityConflict`, while `operation_result(id)` returns a generic execution failure for an operation that was never accepted. A failed endpoint construction follows the same path and additionally converts its model error to a runtime admission error.

There is a separate terminality manifestation at the same post-issuance handoff. `submit()` can acquire an issued ID, then fail `dispatch::submit` before spawning a worker—for example, a direct native caller supplies a valid encoded `FetchRequest` with the wrong response-message name. Its `result.is_err()` branch removes admission and the request mapping without refusing the record. `operation_result(id)` then waits indefinitely. Both native `call()` and `submit()` also obtain the current directory before entering their settlement paths; failure there leaves a previously reserved ID untouched.

**Impact:** Callers cannot use the retained result to distinguish a safe pre-effect retry from a possibly effective failed operation. On the pre-worker exits, by-ID result readers can hang and repeated failures occupy the 64-record ledger until explicitly released. The high-level bridge exposes issued IDs on native errors, so the inconsistency is observable through its result and event methods.

**Correction:** Establish one settlement rule for every post-issuance exit before acceptance. The deferred worker path should retain its original typed model refusal before signalling admission, and outer generic error recording must not replace it. The pre-spawn `submit()` failure branch and native wrapper failures must also settle any supplied issued ID. Keep the failure's pre-effect attribution and wake both result and event waiters.

**Closure test:** Force a differing-capacity `submit()` refusal with a known issued ID. Assert the returned error and `operation_result(id)` have the same typed `TransportCapacityConflict`, event waiting completes, and no `Accepted` or Git effect occurs. Separately force a pre-spawn message-name error and a current-directory failure after reservation; assert each ID becomes terminal. Release the refused IDs, repeat beyond 64 attempts, and verify a later admission succeeds when capacity permits.

This is a **newly identified, non-architectural implementation root cause** in the existing submission/error handoff. It does not introduce a third new architectural root cause under the review-loop cap.

## 2. Invariant analysis

| Prior finding | Current-source disposition |
| --- | --- |
| Code P2-2 — eight-slot refusal leaves an issued ID unsettled | Closed for the original full-slot path. Both direct and submitted checks now call `refuse_full`; the focused native test reads the typed refusal, completes event waiting, and releases 65 IDs. P2-8 identifies different post-issuance exits. |
| Code P2-4 — construction panic strands reserved IDs | Closed for the original counterexample. The panic branch refuses mapped IDs, signals and completes admitting records, publishes a conservative close report, and rejects later admission. Its focused test covers reserved and admitting IDs. |
| State P2-3 — spawn failure loses typed refusal | Closed. `worker_spawn_failure` retains and returns the same `IoError` model refusal; its focused test reads it and checks event completion. |
| State P2-5 — finish panic publishes staged success | Closed. `finish_panic_error` replaces an unpublished pending result, and `finish_after_dispatch` invokes it before `worker_panicked` publishes the terminal. The focused test stages success and injects finish panic. |
| State P2-6 — capacity failure consumes a lifetime request ID | Closed for the identified ordering. `admit_client_request` installs capacity and checks the arrival deadline before `ClientRequest::new` registers the ID. Its focused test times out at the capacity gate and retries the same ID. |
| State P2-7 — dropped staged installation leaves an open, partial generation | Closed for the identified drop path. `CapacityMutation` arms before pool mutation and closes the session if dropped before installed-policy publication. Its focused test drops the future during retirement wait and checks later admission is rejected. |

The capacity leader still serializes installation. Equal installed capacity returns without reinstalling pools; different capacity checks live registrations, endpoint work, and leases before mutation. The mutation guard closes the generation on an abandoned partial transition. I found no separate Code-axis blocker in that changed boundary.

The native session still owns its records and validates nonce and issued serial before by-ID reads. The Python bridge routes lookups to the native session when `issued_operation` exists. The queued worker carries its already-issued ID, preserving the original cancellation fix. The outstanding defect is the inconsistency between admission errors and retained outcomes on the paths described in P2-8.

## 3. Risks and next action

Correct P2-8 and run its causal tests before requesting a Code re-verdict on a new exact tuple. A foundation GO would accept only these identity, admission, capacity, and terminal-publication invariants; the deferred ledger, public handle surface, rollover, transport fixtures, and activation gates still require their separate work.
