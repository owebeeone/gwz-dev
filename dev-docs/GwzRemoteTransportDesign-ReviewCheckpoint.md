# Remote transport design — review checkpoint

Date: 2026-09-19. Status: **accepted at gwz-core
`05842b38e55f109ed3663555680751811a72eb9b` after
[Consistency round 2](GwzRemoteTransportDesign-ReviewConsistency-2.md) and
[Safety round 2](GwzRemoteTransportDesign-ReviewSafety-2.md) reported GO;
this accepts the four-document design draft for implementation planning only**.
This records draft review only; no schema freeze, implementation acceptance,
platform qualification or release is claimed.

## Object and scope

- Core document checkpoint: `3d2cc92c4a233e40ed049c8fe79bc555d39b272f`.
- Parent/base: `c2ca71b7595a0ce332f6f7f4c47db9d0c62ac230`.
- Exact scope: four documents below; no source or generated schema change.
- Other agents' staged history moves and protocol work are excluded. Core's
  unrelated index entries were compared before/after the checkpoint and preserved.
- Draft: `gwz-core/dev-docs/GwzRemoteTransportDesign.md` at the core checkpoint.
- Requirements: `gwz-core/dev-docs/GwzRemoteTransportRequirements.md` there.
- Policy decisions are recorded as agreed; implementation draft is under review.
- Local commit only; no push, tag or release authorized/performed.

## Baseline and independent axes

| Repository | Pinned commit |
|---|---|
| gwz-dev | `7971d6f447692a70d79c9cc09f0bf5f3c5a9d4fe` |
| gwz-core | `3d2cc92c4a233e40ed049c8fe79bc555d39b272f` |
| gwz-cli | `07ad6620f2e26a2b8d70ae0cb8808dd5584058e1` |
| gwz-py | `9c9ac1b5c5bbd800e8454817b99f107cc74d25d2` |
| taut-shape | `74f375c9d3521f3e98110862dcf89ec64a3b6d6c` |
| taut-shape-rs | `df1303656583121e043f7a925c02958571951735` |

Review sources are read from immutable commits. In-flight working-tree content
and any later HEAD are not substituted into the object. The canonical prompts
record both the tuple and inspection rules.

- [Consistency prompt](GwzRemoteTransportDesign-ReviewPromptConsistency.md).
- [Safety prompt](GwzRemoteTransportDesign-ReviewPromptSafety.md).
- Review agents: `transport_consistency` and `transport_safety`, fresh contexts,
  `gpt-5.6-sol`/xhigh. Cross-model binding follows process §4.3.
- Surface gate is not being claimed: this is a draft review, not a CLI/API freeze.

## Executed documentation checks

2026-09-19, macOS, on the exact core checkpoint: four-document scope check,
content equality against committed blobs, whitespace/code-fence/local-link
checks, and `git diff --check c2ca71b7595a0ce332f6f7f4c47db9d0c62ac230 3d2cc92c4a233e40ed049c8fe79bc555d39b272f` all passed.
Earlier requirement-ID checks verified G1–G6, C1–C8, P1–P9, S1–S8 and D1–D15
exactly once and confirmed the historical evidence block was unchanged.
No build, native/platform test, network experiment or performance run was made.

| Committed file | SHA-256 |
|---|---|
| `dev-docs/GwzRemoteTransportRequirements.md` | `62d44b7c26f4a69d14ce974d540fb64f201edce4073b2deb2590d7050e647d11` |
| `dev-docs/GwzRemoteTransportDesign.md` | `7050de7fadc507c36c1df6cf47a66539c2fc4fc318067136813ba2ce3ccf9fb0` |
| `dev-docs/GWZRequirements.md` | `e314730d083c0695c8fe1b7d81b683ea11ce81b23f68cb5198a21518e0178cc9` |
| `dev-docs/GWZDesign.md` | `dd55328cb3e80655bd1ff9fc853892f66e915a1f6fd929cb02a832f53140db1c` |

## Round-1 gate state and metrics (historical)

Round 1 returned NO-GO/NO-GO. Consistency: two P2; Safety: one P1 and
three P2; all discovered at draft review. No implementation or acceptance
existed, so these are not escaped production defects. Both axes noticed global
transport registration/coexistence (Safety blocker, Consistency residual risk).
Reports are filed verbatim:

- [Consistency](GwzRemoteTransportDesign-ReviewConsistency.md).
- [Safety](GwzRemoteTransportDesign-ReviewSafety.md).
- [Merged remediation](GwzRemoteTransportDesign-RemPlan.md): one patch, one
  disposition and closure scenario group per report-qualified finding.

Remediation rounds used: 1 of 2. Revised design is not yet accepted.
Because the corrections change proposed interface/binding/lifecycle semantics,
round 2 must use fresh contexts under review-loop step 5.5. Original reviewers
also verify their own original counterexamples; no blocking finding is
self-closed. Next action: commit the merged patch, pin the revised object,
dispatch fresh dual review and focused original-reviewer closure checks.

## Initial round-2 dispatch (superseded by operator override below)

Merged documentation remediation is committed at core
`05842b38e55f109ed3663555680751811a72eb9b`; root review records/plan are
committed at `3a0b8fa975bc013673ee919b087f69da9f3853ff`. Supporting CLI,
Python and taut source pins remain those in the original tuple so unrelated
archive/protocol changes are not silently substituted into the review.

Fresh peer-blind reviewers: `transport_consistency_r2` and
`transport_safety_r2`, `gpt-5.6-sol`/xhigh. Generated round-2 prompts are
[Consistency](GwzRemoteTransportDesign-ReviewPromptConsistency-2-AbandonedFresh.md) and
[Safety](GwzRemoteTransportDesign-ReviewPromptSafety-2-AbandonedFresh.md). Original reviewers
are checking their own original counterexamples separately; they do not read
fresh current-round output. Remediation rounds consumed: 1.

On the revised documents, local links, fences, whitespace, G1–G7/C1–C8/P1–P9/
S1–S9/D1–D15 coverage, unchanged historical evidence and core diff formatting
checks passed. These are document checks only. Acceptance remains pending.

## Operator review-routing override — 2026-09-19

Operator: **"use the old reviewers"**. Both fresh-context review agents were
interrupted; neither produced a final report/verdict and neither's unfinished
work contributes to this gate. Round 2 proceeds with the original
`transport_consistency` and `transport_safety` agents, context retained.
Both verified their original findings as closed, then returned full axis GO
re-verdicts covering the corrected text's interactions. This is a case-specific
operator override of review-loop step 5.5, not a change to the transport design
or a general amendment of the review skill. Only completed report-bearing
rounds count toward the review ledger. Remediation rounds consumed remains 1.

## Final verdict merge and acceptance

The original agents returned **GO/GO** on core
`05842b38e55f109ed3663555680751811a72eb9b`, tree
`226102463b82fd1d334663245df78fe1a00cd963`. Root prior-round inputs were
`3a0b8fa975bc013673ee919b087f69da9f3853ff`, tree
`445c63ee7e6a692b8453b297ea94265437dbbddc`. Supporting source pins remain
those listed above. Both reviewers verified immutable pins and the filed
round-1 reports. Round-2 reports are filed verbatim:

- [Consistency full re-verdict](GwzRemoteTransportDesign-ReviewConsistency-2.md).
- [Safety full re-verdict](GwzRemoteTransportDesign-ReviewSafety-2.md).
- Earlier focused closure testimony: [Consistency](GwzRemoteTransportDesign-ClosureConsistency-2.md), [Safety](GwzRemoteTransportDesign-ClosureSafety-2.md).
- Original-reviewer round-2 prompts: [Consistency](GwzRemoteTransportDesign-ReviewPromptConsistency-2.md), [Safety](GwzRemoteTransportDesign-ReviewPromptSafety-2.md).

All six blockers are reviewer-closed; no new findings were reported. Completed
review rounds: 2; merged remediation rounds: 1 of 2. One P1 and five P2 were
found at draft review; no implementation exists to assess escaped defects.
The abandoned fresh attempt contributes no report, finding, or verdict.
Acceptance covers the design draft only. Safe git2 binding, transport libraries,
platform qualification, parameter tuning, schema/API freeze and executable
acceptance tests remain future work. The subsequent landing changes only
acceptance annotations and audit records; it does not alter the reviewed
transport contract. No push, tag, release or implementation work is included.

Final audit filing checks: local links and code fences passed across all 15
transport design/audit documents. Core's acceptance-only diff passes whitespace
checks. Root audit whitespace diagnostics are limited to reviewers' intentional
two-space Markdown line breaks, preserved to keep their reports verbatim.
