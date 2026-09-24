# Python concurrent transport session v2 — Safety re-review 4

**Review object:** `dev-docs/GwzPyTransportSessionV2Design.md` and `gwz-py/dev-docs/GwzPyConcurrentOperationsV2.md`  
**Baseline:** root `78a46ef58bdfc3fac0c5a6600597465c9436c20d`; gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; gwz-py `cddb38204fbbd11808cc3c414807aa66a5910ce0`  
**Date:** 2026-09-24  
**Axis:** Safety. Final focused, read-only re-verdict under the third-round non-architectural cap.

**Verdict: GO.** Safety P2-1 is closed in the design contract. I found no new P0–P2 finding and **no new architectural root cause**.

## Prior-finding closure table

| Prior finding | Re-trace | Status |
| --- | --- | --- |
| Safety-3 P2-1 — recovery descriptors and close summaries lack reserved space | An unstarted record reserves 4 KiB. Admission atomically upgrades that charge to an 8 MiB **total** allowance containing 4 KiB for identity, required discovery and a possible close summary. Eight admissions therefore charge exactly 64 MiB, including required recovery metadata. `recent_operations()` reads its pre-reserved descriptor; close transfers the summary charge into Client reporting without a new aggregate charge. Root design line 47; caller guide line 63. | **Closed in design text.** |
| Safety-2 P2-1 — unbounded retained request IDs | Explicit IDs are limited to 128 UTF-8 bytes before record issuance, and generated IDs obey the same rule. The limit matches core’s existing `identifier` validator (`gwz-core/src/transport_host/request.rs:393–395`). | **Closed in design text.** |
| Safety-1 P2-1 — first reader unavailable after acceptance | Its cursor remains inside the 8 MiB admission reservation and is claimed before a stream worker starts. | **Closed in design text.** |
| Safety-1 P2-2 — abandoned push outcome erased | The Client retains accepted records; dropped streams remain discoverable by ID through `recent_operations()` until release or expiry. | **Closed in design text.** |

## Changed-range analysis

Relative to root `5b39c6f360506844695cbd658a20a57f8bda430a` and gwz-py `53075fbf56856e51cc1aac3f146ab7f7c84cdfc5`, the correction narrows request-ID validation to core’s existing nonempty, 128-byte, no-control-character rule. It replaces unspecified recovery-metadata charging with a 4 KiB unstarted reservation that becomes part of the 8 MiB accepted allowance. Required discovery uses that reservation; a close summary transfers its charge to the retained Client report until Client drop. The proof inventory now covers 64 unstarted records, eight full admissions, abandoned-push discovery, close summaries, record release and expiry. Core is unchanged.

## 0. Evidence base

I read `GwzPyTransportSessionV2-RemPlan-3.md`, my filed Safety-3 report, both revised review objects and their prior-to-current diffs. I checked core’s `identifier` rule and traced the full-ledger sequence through admission, discovery, terminal publication, close, release and expiry. No files were changed; no builds, tests, source checks or wire proof ran. The tuple matched at the start and end.

## 1. Findings

None.

## 2. Invariant analysis

The earlier eight-operation counterexample is now arithmetically satisfiable: eight atomic upgrades to 8 MiB consume 64 MiB, and each includes the required recovery metadata. A dropped, possibly effective push can be listed without seeking new ledger capacity. Close can report all eight operations live at its start by transferring already-reserved summary charges. Completion retains the required charges while releasing unused operation allowance; record expiry does not erase a still-retained Client close summary. Optional extra copies may refuse without blocking required discovery or close reporting.

Invalid request IDs refuse synchronously before an operation ID, endpoint or credential work. The changed range does not alter capacity installation, generation-pinned cancellation, primary-reader access, possible-effect terminal handling or cross-Client ledger isolation.

## 3. Risks and next action

This **GO accepts the revised design text only**. The specified focused core, native and Python tests must prove the accounting and recovery sequence during implementation; settled Code/State review and platform gates remain separate.

The final tuple matched: root `78a46ef58bdfc3fac0c5a6600597465c9436c20d`; gwz-core `7a8b8efb7f9a08d9d0055cb860071a36b26c3d9d`; gwz-py `cddb38204fbbd11808cc3c414807aa66a5910ce0`.
