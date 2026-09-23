# Python transport session v2 candidate foundation — CODE-AXIS RE-REVIEW

**Review object:** Corrected foundation at root `bf5c7270d9e8e098d131e20392713267160c1600`, core `eebd41bab1fe7a91249a55b48c39237d8c6c1671`, Python `fe9d6797148cf71315487817d36396d042214a6e`; diff from the prior reviewed tuple.
**Baseline:** Prior review at root `9cb11fd5d561ee108525357cdaafc11af611a137`, core `dfb7533ba8cd2eba59fc1d8ba8365a74b7413371`, Python `33a1f3f4e2ab9a7b97fd03246f7bb3f1905dfead`. All three corrected HEADs matched at the start and end of this review.
**Date:** 2026-09-24
**Axis:** Code — architecture, interfaces, call graphs, and compatibility reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — prior P2-2 remains open and one new P2-4 blocks. Prior P1-1, P2-1 and P2-3 close on the corrected source. I pre-commit to GO on a revision that resolves P2-2 and P2-4 as specified and passes their focused closure tests, absent a new blocking root in that revision.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| P1-1 | Bind the issued ID to the queued native call and refuse a cancelled ID before registration. | `bridge.py:353–365` passes the ID to native `call`/`submit`; `transport_session.rs:291–320, 997–1029, 1032–1066` validates that exact ID, and the spawned worker receives it at lines 799–812. Cancellation retains the mapping and a typed refusal at lines 900–949. The focused native test queues both forms behind a blocked executor and checks that the next serial is exactly one higher. | Closed |
| P2-1 | Route native-session lookups through their own validator. | `bridge.py:252–260` now selects the session whenever it exposes `issued_operation`; result, event and merge lookups share this route. The new Python test checks a foreign ID receives typed `InvalidRequest` without touching the legacy module. | Closed |
| P2-2 | Settle every issued pre-effect refusal and expose or release its ID. | The bridge now attaches the issued ID to ordinary errors, and `OperationStore::refuse` wakes result/event waiters. The 65-refusal test checks explicit release after each error. But both eight-slot-full exits return before calling `refuse`, leaving an issued record with neither result nor refusal. | Open |
| P2-3 | Refuse explicit CLI placement before endpoint construction. | `transport_session.rs:484–505` checks placement before `runtime()` at line 561; the focused native test covers direct and submitted requests with and without `HOME`. | Closed |

## Changed-range analysis

The core diff replaces the unbounded admission mutex wait with one five-second deadline carried through capacity installation. The Python diff binds queued calls to issued IDs, routes session lookups to the session, adds typed pre-effect refusals, and adds construction and worker panic bookkeeping. Focused tests for those paths are present in the changed range; this review did not execute them.

P2-2 is an incomplete disposition of the original pre-effect-record root. P2-4 below is a **new, non-architectural root cause** introduced by the construction-panic transition: setting `Closed` bypasses the normal close path that settles unstarted records. No other changed-range Code blocker was found.

## 0. Evidence base

I read `dev-docs/GwzPyTransportSessionV2Foundation-RemPlan.md`, the filed prior Code report, the accepted design and candidate checkpoint; inspected `git diff` from the prior tuple in root, core and Python; and traced the corrected `gwz-core/src/transport_host/session.rs`, `gwz-py/native/src/{transport_session,operations,dispatch/mod}.rs`, `gwz-py/src/gwz/{bridge,errors}.py`, and focused tests. Inspection used `git rev-parse`, `git diff`, `rg`, `sed` and `cat`. No reviewed file was edited and no build or test was run. The State report was not read.

The full byte ledger, timer, public handles, rollover and platform gates remain deferred by the candidate checkpoint and are not findings here.

## 1. Findings

### [P2-2] Eight-slot pre-effect refusal still leaves an issued record unsettled

**Location:** `gwz-py/native/src/transport_session.rs:543–548, 1085–1093`; `gwz-py/native/src/operations.rs:432–443`.

**Violated invariant:** Every issued operation refused before effects must have a retained terminal refusal or be released, so by-ID result and event readers terminate and the count-limited ledger remains recoverable.

**Counterexample:** Keep eight operations in admitting or active state. Issue a ninth ID and invoke either native `call()` or `submit()`. Each eight-slot check removes the request-to-ID mapping and returns `TransportSessionFull` without calling `OperationStore::refuse` or discarding the record. The bridge now exposes the ninth ID on the error, but `operation_result(id)` waits indefinitely because that record has neither a result nor refusal. `wait_events(id)` likewise has no terminal completion. Normal close cannot settle it through `request_operations`, because the full branch removed its mapping.

**Impact:** A normal, bounded admission refusal leaves a misleading live record and can strand a caller waiting for its outcome. Repeating the refusal also fills the 64-record ceiling unless callers know to release records whose result never becomes terminal.

**Required correction:** Set a typed `TransportSessionFull` refusal on the issued record before returning from both full branches, preserving the exposed ID and high-water history. Audit other early-return paths after `operation_for_request()` for the same settlement rule.

**Closure test:** Hold eight admitted or admitting operations, attempt a ninth through both normal Python call forms, and assert its error carries the issued ID; `operation_result(id)` and event waiting terminate with the retained typed pre-effect refusal. Repeat and release enough refusals to cross 64 attempts, then verify admission recovers when a slot opens.

### [P2-4] Constructor panic marks the session Closed before unstarted records are settled

**Location:** `gwz-py/native/src/transport_session.rs:384–403, 815–825, 862–874`; `gwz-py/native/src/operations.rs:432–443`.

**Violated invariant:** Closing or faulting a Client must terminalize every already-issued unstarted record and wake its ledger waiters without reopening an endpoint.

**Counterexample:** Reserve ID A without starting it. Start a different network operation B and inject a panic in `TransportRuntime::from_environment()`. The new catch path sets `state.status = Closed` and a close report, but does not refuse A. A later `close()` takes the `Status::Closed` early return, so it never reaches the loop at lines 862–874 that refuses unstarted mappings. `operation_result(A)` has neither result nor refusal and waits indefinitely; the deferred expiry timer cannot currently clear it.

**Impact:** The new fail-closed construction path can leave a caller-owned ID unreadable after close, contrary to the candidate's panic-wakeup and post-close outcome claims.

**Required correction:** Drive construction panic through the same ledger-finalization path as close, or settle all unstarted and admitting records before publishing `Closed`. Keep the conservative physical-cleanup report and reject later admission.

**Closure test:** Reserve A, inject endpoint-construction panic through a separate request B, then call close. Assert A and B reach typed terminal/refusal states, their result/event waiters wake, close returns the same conservative report on repeat, and later admission refuses.

## 2. Invariant analysis

The original queued-cancellation counterexample no longer remints an ID: the Python worker carries the issued token into native entry, native verifies its request mapping, and cancellation keeps that token marked. The corrected test checks both `call` and `submit` serial continuity. Native Client lookups now enter the per-session validator, so the prior global-store fallthrough is removed. Explicit CLI placement is rejected before `runtime()` can inspect endpoint configuration. These are source-level closures supported by focused test code, not claims of tests executed in this review.

The core capacity change carries one deadline from admission leadership into pool installation, and its new test checks timeout before request registration. The generated protocol method set remains unchanged in this remediation. The findings above concern the native ledger's terminality on two early-exit paths.

## 3. Risks and next action

Correct P2-2 and P2-4 in one focused patch, run their causal tests, and return the exact revised tuple for Code re-verdict. The deferred byte budget, timed expiry, public handle surface, rollover and platform gates remain separate work before activation.