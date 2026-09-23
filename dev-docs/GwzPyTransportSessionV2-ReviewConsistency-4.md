# Python concurrent transport session v2 — CONSISTENCY-AXIS RE-VERDICT 4

**Review object:** Committed `dev-docs/GwzPyTransportSessionV2Design.md` and `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md`.  
**Baseline:** root `78a46ef58bdfc3fac0c5a6600597465c9436c20d`; gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; gwz-py `cddb38204fbbd11808cc3c414807aa66a5910ce0`. All three heads matched at review start and end.  
**Date:** 2026-09-24.  
**Axis:** Consistency — focused, independent, adversarial, read-only.

**Verdict: GO for the design text at this tuple.** My remaining P2 counterexample is closed. I found no new blocking finding or new architectural root cause in the changed range. This verdict does not accept implementation.

## Prior-finding closure table

| Finding | Re-trace at this tuple | Disposition |
| --- | --- | --- |
| Consistency-3 P2-2 — Python promised 256-byte request IDs while pinned core accepted at most 128 bytes and rejected controls | Root design §2 and the caller guide now require a nonempty ID of at most **128 UTF-8 bytes** with no Unicode control characters. Invalid IDs refuse synchronously before an operation ID, record or endpoint is created. Section 8 calls for valid 128-byte admission and synchronous refusal of 129-byte and control-containing IDs. This matches the pinned core `identifier` rule. | **Closed in the document contract.** The boundary cases remain implementation tests. |
| Consistency-2 P2-1 — native close contradicted the custom-bridge return contract | Native close remains scoped to `NativeCoreBridge`; custom bridges without `close` return `None` and retain `close_report=None`. This correction is unchanged. | **Remains closed.** |
| Consistency-2 P3-1 — dropped-stream proof required `aclose()` on a discarded object | Section 8 still has separate retained-stream `aclose()` and true dropped-stream recovery cases. | **Remains closed.** |
| First-round Consistency P2-1/P2-2/P2-3 | Generation-bound cancellation, reserved primary reader and named detached cancellation response are unchanged by this correction. | **Remain closed in the document contract.** |

## Changed-range analysis

Relative to root `5b39c6f360506844695cbd658a20a57f8bda430a` and Python `53075fbf56856e51cc1aac3f146ab7f7c84cdfc5`, the request-ID limit changed from 256 to 128 UTF-8 bytes and gained synchronous empty/control-character refusal. The root design also gives each unstarted record a 4 KiB required-metadata reservation, upgraded into its 8 MiB total admission allowance. Required discovery and close-summary metadata remain charged within the 64 MiB ledger; the caller guide and implementation proof now say the same. Core is unchanged.

Eight 8 MiB admissions account for exactly 64 MiB including their required metadata. Sixty-four unstarted records account for at most 256 KiB before admission. Required descriptors use reserved capacity, while optional retained copies may refuse. The close-summary charge transfers into Client reporting and remains charged after its operation record expires or is released. I found no contradiction between those lifetimes and the stated aggregate bound.

## 0. Evidence base

I read `GwzPyTransportSessionV2-RemPlan-3.md` and my filed third report, compared the committed root and Python document changes, and re-traced the ID-boundary counterexample against the pinned core `validate_meta` and `identifier` rule in `gwz-core/src/transport_host/request.rs:376–395`. I inspected the changed metadata and proof clauses for new blocking inconsistencies. Only read-only inspection commands ran; no file was changed, built or tested.

## 1. Findings

**None.** No P0–P3 finding remains open on this axis.

## 2. Invariant analysis

The public factory grammar now matches core’s nonempty, 128-byte, no-control-character admission grammar. A valid boundary ID can be issued and presented to registration; a predictably invalid ID refuses before identity issuance or endpoint effects. The revised metadata reservation preserves required post-effect discovery and close summaries even when eight operations consume the full ledger allowance. The custom-bridge, dropped-stream, generation, reader and cancellation-response contracts remain coherent across the changed range.

## 3. Risks and next action

Accept the **Consistency design gate** at this exact tuple. The specified boundary, recovery and accounting cases still need execution during implementation and a separate settled Code/State review. No new architectural root cause triggers the third-round stop rule.
