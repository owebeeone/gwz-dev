# Remote transport design — review checkpoint

Date: 2026-09-19. Status: **round 1 NO-GO; merged remediation round 1 prepared**.
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

## Gate state and metrics

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
