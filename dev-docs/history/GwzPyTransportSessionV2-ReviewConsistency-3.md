# Python concurrent transport session v2 — CONSISTENCY-AXIS RE-VERDICT 3

**Review object:** Committed `dev-docs/GwzPyTransportSessionV2Design.md` and `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md`.  
**Baseline:** root `5b39c6f360506844695cbd658a20a57f8bda430a`; gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; gwz-py `53075fbf56856e51cc1aac3f146ab7f7c84cdfc5`. All three heads matched at review start and end.  
**Date:** 2026-09-24.  
**Axis:** Consistency — focused, independent, adversarial, read-only.

**Verdict: NO-GO.** Both findings from my second report close in the revised document, but the changed request-ID rule introduces one new P2 mismatch with the pinned core validator. This is a bounded contract correction, **not a new architectural root cause**.

## Prior-finding closure table

| Second-report finding | Re-trace at revised tuple | Disposition |
| --- | --- | --- |
| P2-1 — `Client.close()` conflicted with the accepted custom-bridge contract | Root design §§1, 2 and 6 now scope host and ledger behavior to `NativeCoreBridge`. Section 6 preserves `TransportCleanup | None`, requires `None` and `close_report=None` for a custom bridge without `close`, and retains exactly a custom bridge’s own report when provided. The caller guide agrees. Section 8 requires the native/custom close cases. | **Closed in the document contract.** The custom-bridge and native-close cases remain implementation tests. |
| P3-1 — dropped-stream proof required calling `aclose()` on the discarded object | Section 8 now specifies separate tests: retain one stream for `aclose()` and by-ID result; record another stream’s ID, drop every stream reference, then recover through `recent_operations()`. | **Closed.** Both proof paths are executable and distinguishable. |

## Changed-range analysis

Relative to root `59c13d6184c82789d9dc558ca436b3a43be8bdcd` and Python `ffa184bbb50bf56ff49ebee65a79c228e0bf6489`, the correction scoped native-only APIs, reconciled custom-bridge close behavior, separated stream-abandonment tests, bounded request IDs and their charged copies, and clarified push defaults in the guide. Core is unchanged. The new finding is confined to the newly specified request-ID range; I found no new blocking defect in the custom-bridge or stream-proof corrections.

## 0. Evidence base

I read the second remediation plan and my filed second report, compared the committed root and Python document ranges, and re-traced both prior counterexamples. I checked the request validator in the committed core source at `gwz-core/src/transport_host/request.rs:376–395`. Inspection used only read-only commands. No files were changed, and no build or test ran. Implementation acceptance and physical wire or iroh proof remain outside scope.

## 1. Findings

### [P2-2] The documented 256-byte request-ID range exceeds core’s 128-byte admission limit

**Root cause and location:** Root design §2, line 13, and caller guide line 53 specify a nonempty caller request ID of at most **256 UTF-8 bytes**, validated synchronously before an operation ID is issued. The unchanged, pinned core `validate_meta` calls `identifier` for `RequestMeta.request_id`; that validator accepts at most **128 bytes** and rejects control characters (`gwz-core/src/transport_host/request.rs:376–395`). The design does not amend that core grammar.

**Violated invariant:** An ID documented as valid at synchronous handle construction must be admissible by the required core registration path, or its refusal boundary must be stated truthfully.

**Credible sequence and impact:** Call `client.start_fetch(request_id="x" * 129, ...)`. The new Python rule permits and issues a handle, while core rejects that same ID during `accepted()` before registration. A 256-byte ID meets the published maximum but cannot be accepted. An ID containing a control character has the same gap even below 128 bytes. This gives callers an incorrect public range and moves a predictable input error from synchronous construction to asynchronous admission.

**Remedy:** Align the public synchronous validation with the pinned core identifier grammar: nonempty, at most 128 UTF-8 bytes, and no control characters. If 256 bytes is intentional, explicitly amend and review the core and transport identifier limits before promising that range.

**Closure test:** At the factory boundary, accept a valid 128-byte ID; synchronously reject 129-byte and control-character IDs without issuing an operation ID or constructing an endpoint. Confirm the accepted boundary ID reaches core registration.

## 2. Invariant analysis

The revised custom-bridge rule now permits `Client.close()` to return truthful native cleanup while preserving the accepted `None` behavior for a custom bridge without a close hook. Native-only handle and ledger methods refuse unsupported custom bridges before effects. The two stream tests now separately prove explicit closure and dropped-object recovery. The earlier generation, primary-reader, and detached-cancellation-result corrections remain intact.

The 256-byte rule conflicts with an unchanged core admission precondition. Choosing the existing 128-byte grammar is a bounded documentation correction; widening core’s grammar would cross a separate interface boundary and needs its own reviewed amendment.

## 3. Risks and next action

Keep this design **NO-GO** until the request-ID grammar and synchronous refusal test agree with the selected core contract. Recheck the corrected ID boundary on an exact tuple. The two prior findings need no further remediation.
