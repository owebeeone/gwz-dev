# Python concurrent transport session v2 — Safety re-review 2

**Review object:** `dev-docs/GwzPyTransportSessionV2Design.md` and `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md`  
**Baseline:** root `59c13d6184c82789d9dc558ca436b3a43be8bdcd`; gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; gwz-py `ffa184bbb50bf56ff49ebee65a79c228e0bf6489`  
**Date:** 2026-09-24  
**Axis:** Safety — degraded paths, irreversible effects, stuck states, privacy, races, capacity and cleanup. Independent, read-only review.

**Verdict: NO-GO.** The two prior Safety P2 findings are closed in the document contract. One new P2 resource-bound finding remains open. I pre-commit to **GO** if the bounded correction below closes it without introducing another blocking safety defect.

## Prior-finding closure table

| Prior finding | Re-traced counterexample | Verdict |
| --- | --- | --- |
| Safety P2-1 — an accepted stream cannot afford its first reader | The 8 MiB admission reservation now includes a primary cursor; a stream helper claims it before the worker gate opens. Eight full reservations can therefore still read their terminal events. Additional readers may refuse without changing the operation outcome. Root design §§5–6, lines 37 and 47; caller guide line 38. | **Closed in the design text.** The required implementation test remains a later gate. |
| Safety P2-2 — abandoning a push stream erases its outcome before close | `OperationStream` exposes its ID and `aclose()` outcome. Dropping it requests cancellation but leaves a Client-owned record; `recent_operations()` discovers that record even if the push terminalizes before close. Retention lasts through the advertised deadline. Root design lines 17, 43–49; caller guide lines 69–71. | **Closed in the design text.** The required remote-acceptance and abandonment test remains a later gate. |

## Changed-range analysis

Relative to root `f874d421d9f0f453b673d1de17a1cf4fa4fc621e` and gwz-py `24a4314487ff477eaca0535bdeae3289fead7ab9`, the correction adds explicit generation-pinned cancellation; reserves the primary reader before acceptance; gives cancelled helpers a named, detached `.response`; retains abandoned stream results; adds stream IDs, `aclose()` and `recent_operations()`; and moves the inherited physical per-host default to `Client`. It also clarifies request-ID consumption, outer enum compatibility and focused proof obligations. gwz-core remains at the specified old and new commit `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`.

The retained-ledger change makes the ledger’s claimed byte bound more consequential. Its new discoverability path keeps records after their handles are dropped, including records created before admission.

## 0. Evidence base

I read the complete two review objects, the first-round Safety report, remediation plan and prior concurrency verdict; the accepted Python transport design; relevant core retry, remote transport and v1.1.0 plan clauses; and the `RequestMeta` and `OperationResult` schema. Process authority was `dev-docs/AgentProcessRules.md` as amended by `GwzProcessOptimization.md`, with the review-loop skill. Inspection was read-only. No builds, tests, source checks or wire proof were run.

## 1. Findings

### [P2-1] Unstarted records can retain unbounded caller IDs outside the ledger byte cap

**Location and invariant:** Root design lines 13 and 47–49; caller guide lines 53 and 63; `gwz-core/protocol/gwz.taut.py` line 1111. The design says an unstarted handle consumes a *small fixed record slot* and that the session has a 64 MiB charge for events, results and reader metadata. Yet the synchronous factory stores the caller’s `RequestMeta.request_id` in that record, while the schema declares it as an unconstrained string and the design states no length check or charge for it. Retained records must obey the advertised memory bound, including before admission.

**Credible sequence:** Create 64 unstarted handles with distinct, large explicit `request_id` strings, then drop the caller’s references to those strings while retaining the handles. No operation reaches the 8 MiB reservation or Git work. The session keeps all 64 IDs in its records for up to 15 minutes. For example, 64 IDs of 16 MiB each retain about 1 GiB of string data despite the stated 64 MiB ledger cap. Accepted records have the same omission after abandonment.

**Impact:** A caller can make one Client retain memory far beyond its stated bound without admission. The 64-record limit does not bound bytes, and expiry only ends the exposure later. This is a concrete capacity and recovery defect.

**Required correction:** Set and enforce a finite byte limit on caller request IDs before issuing/storing an ID, or charge all variable-size record identity data against a stated aggregate budget. Keep the refusal pre-effect and state its typed error. Apply the same accounting to compact descriptors and close summaries if they copy the ID.

**Closure test:** Supply an over-limit explicit request ID to a synchronous `start_*` factory and verify pre-effect refusal with no issued record or endpoint construction. Fill 64 records at the maximum permitted size, drop external references, and verify retained bytes—including IDs and any descriptor copies—remain within the documented budget through completion, close and expiry.

**Classification:** New bounded quota-specification defect; **not a new architectural root cause**. The ledger ownership and lifecycle design need no replacement.

## 2. Invariant analysis

The revised primary-reader reservation removes the first-round post-effect capacity failure. The revised Client-owned record and discovery API remove the first-round early-abandonment gap. Close joins live operations, preserves records, and reports bounded summaries for operations live when it began. Cancellation retains identity and a detached typed result for helper-task cancellation; rollover pins cancellation to the original generation. Different physical capacities refuse while live work, leases or cleanup remain, including between leases; equal capacity joins without reinstalling. Explicit CLI placement still refuses before credentials.

The new enum members are assigned unused values 73 and 74 and remain subject to the outer GWZ compatibility rule. The described Python session path uses paired projections; this review found no concrete mixed-version route requiring a separate finding. The stated possible-effect handling for admitted cancellation and record overflow preserves the push-reconciliation warning. These are design-text conclusions, not implementation or wire evidence.

## 3. Risks and next action

Keep the design gate **NO-GO** until request-ID retention is bounded or charged, then recheck the fixed-size record and 64 MiB claims on a new pinned tuple. The first-round safety counterexamples need no further design change. Implementation, platform checks and physical wire/iroh proof remain separate gates.

The tuple matched at the start and final recheck: root `59c13d6184c82789d9dc558ca436b3a43be8bdcd`; gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; gwz-py `ffa184bbb50bf56ff49ebee65a79c228e0bf6489`.
