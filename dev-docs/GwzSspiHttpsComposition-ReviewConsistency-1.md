# HTTPS SSPI composition proposal — Consistency-AXIS REVIEW

**Review object:** `dev-docs/GwzSspiHttpsCompositionDesign-DRAFT.md`, lines 1–593, committed corrected DRAFT at `721e07d65aa78a8bd79d41dae86ad99629a7aefc`. Documents-only contract review, dated 2026-10-04; no implementation acceptance.

**Baseline:**

| Repository | Reviewed HEAD |
|---|---|
| gwz-dev | `721e07d65aa78a8bd79d41dae86ad99629a7aefc` |
| gwz-core | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
| gwz-sspi | `616e32cceeea1b7df1d7bbe1c1695a409a733f6d` |
| gwz-cli | `0c7dfaf0199731648d2360358284010b2b4575c1` |
| gwz-py | `ded47130af23720099e7b6a92ccb9a161bb5db9a` |
| gwz-transport | `1aab733783e06b25cb5d2321d71ec0b34417a29c` |

The corrected object and caller guide were read with `git show 721e07d65aa78a8bd79d41dae86ad99629a7aefc:<path>`. All six HEADs matched at the start and end. Every member HEAD remains unchanged from the original review.

**Date:** 2026-10-04

**Axis:** Focused Consistency re-verdict against the controlling graph, original P2-1 counterexample, all requested closure cases, and the complete consequential document changes. Independent, adversarial, read-only. Other axes run in parallel; nothing here relies on their current results. Filed verbatim by the lane owner.

**Verdict: GO** — P2-1 is independently closed for the proposed contract. Zero open P0, P1, P2 or P3 findings. The earlier pre-commit was not treated as closure.

---

## 0. Evidence base

This round inspected:

- The complete corrected composition proposal, §§1–9, lines 1–593.
- The complete corrected `GwzSspiHttpsCallerGuide-DRAFT.md`.
- The original `GwzSspiHttpsComposition-ReviewConsistency.md` and complete merged `GwzSspiHttpsComposition-RemPlan.md`.
- The complete proposal and caller-guide diff from original root `fe40ba9b23e31da95358441ae5214a7cadb31b31` to corrected root `721e07d65aa78a8bd79d41dae86ad99629a7aefc`.
- The changed-file inventory and checkpoint/release-readiness diffs over that range.
- The unchanged sources establishing the original counterexample:
  - `gwz-sspi/src/worker/windows/conversation.rs:255–263`;
  - `gwz-sspi/src/worker/session.rs:16–71`;
  - `gwz-sspi/src/protocol/profile.rs:192–222`;
  - `gwz-sspi/src/protocol/bounds_tests.rs:295–330`.
- The retained controlling mechanism and deadline clauses:
  - `GwzSspiDesign.md:70–80, 224–250`;
  - `GwzSspiCallerGuide-DRAFT.md:88–105`.

The original review’s inspection of the unchanged retry, helper timing/configuration, SSH helper-clock, Windows policy, host capture, transport capability, lease and cleanup contracts remains applicable. This round checked the complete corrected text for consequences against those retained invariants.

Commands were inspection-only: `git rev-parse`, `git show`, `git diff`, `git status`, `nl` and document reads. No edits, Git mutations, tests, builds, source probes or native execution occurred. The attempted review completed; no execution-quota failure prevented inspection.

Current Safety results were not inspected. Archived peer review contents were not used as evidence for this verdict.

## 2. Invariant analysis

### Prior-finding closure

| Finding | Independently verified correction | Disposition |
|---|---|---|
| P2-1 — Native facts conflate authoritative mechanism selection with token completion | §7 lines 432–437 preserves the actual authority flag independently of TokenStatus, expressly admits authoritative NTLM Continue, and retains native Complete separately in owned conversation state. Lines 444–451 require that independently observed Complete before authenticated success. §9 lines 570–573 adds the requested regression obligations. | **Closed for the proposed contract.** Future executable bridge and wire-vector evidence remains required for implementation acceptance. |

The original counterexample remains valid in the unchanged library: direct NTLM reports authoritative selected NTLM independently of token status; the worker bridge and SSPI codec admit that observation on Continue. The corrected proposal now projects it without changing or rejecting its authority flag.

All five requested closure cases have a coherent outcome:

| Closure case | Corrected contract outcome | Evidence |
|---|---|---|
| Authoritative selected NTLM on Continue | Preserve `Selected(Ntlm, authoritative=true)`; do not infer completion. | §7 lines 432–437; §9 lines 570–573 |
| Cancellation or expiry before Complete | Preserve mechanism authority; authentication remains unknown unless actual remote rejection was observed. Credential publication remains independently truthful. | §7 lines 449–455 |
| Remote response before native Complete | Mechanism authority cannot satisfy the separate completion prerequisite; authenticated success is forbidden. | §7 lines 446–449 |
| Native Complete plus valid remote acceptance | Permit authenticated success only with credential publication, authoritative selection, independently observed Complete and terminal/publication checks. | §7 lines 444–448; §8 lines 512–519 |
| Unresolved/provisional Negotiate, remote rejection, or Complete without acceptance | Preserve unresolved/provisional observations; actual rejection yields false; Complete alone leaves authentication unknown. | §7 lines 429–445; §9 lines 570–573 |

These are document-level closure checks, not claims that future tests have run.

### Changed-range classification

| Changed area | Classification and consequence |
|---|---|
| Native facts validity, success projection and regression rows | Material interface-semantics correction. It separates existing mechanism authority from token completion using private producer state. No new wire field, schema tag, public API, runtime owner or dependency is introduced. |
| Proposal status and timeout-zero disposition | Records the owner’s delegated selection of the already proposed behavior. Native selection refuses before publication without a finite setup deadline; no replacement allowance appears. This does not waive independent review. |
| Caller-guide status paragraphs | Disposition-status clarification only. Signatures, ownership, defaults, lifecycle, deadline source and refusal boundary remain unchanged. |
| Program checkpoint, release-readiness history and remediation plan | Review/disposition bookkeeping. They preserve the distinction between design review, implementation acceptance and Windows activation. |
| Filed original prompts and reports | Audit artifacts; no product-contract effect. Current peer conclusions were not used. |
| Product members | No changed HEADs or implementation changes. No new architecture or platform cause arose in this remediation. |

### Retained invariants

The correction does not weaken first-terminal arbitration. Authoritative Continue is now reportable, but neither that authority nor a later remote response can bypass Complete or resurrect a cancelled/expired result.

The selected zero behavior preserves the finite SSPI deadline contract: zero supplies no finite deadline and refuses native selection before Begin or Authorization publication. An expired positive deadline remains Timeout. Anonymous, existing Basic and SSH behavior is unchanged. The guide records this selection without presenting it as implemented.

The positive logical Open deadline still runs without pauses or resets through establishment, discovery, continuation, helper work and native rounds. Active HTTP I/O accounting remains separate, and enclosing expiry gains no helper provenance. The quoted absolute-deadline requirement still agrees with `GwzSspiDesign §6`.

Original-thread capture, context binding, multi-Open ownership, launch-time rechecks and disposal exceptions remain unchanged. The correction does not add executor-thread recapture or make native availability a prerequisite for ordinary transport paths.

Capability admission, old-decoder refusal, final-origin CBT, exclusive lease ownership, retained cleanup, route retirement and the post-effect replay prohibition are unchanged. No new contradiction was established in those contracts.

## 3. Risks and next action

This GO accepts the corrected proposed contract on the Consistency axis. It does not certify future implementation, executed projection tests, native provider behavior or Windows activation.

The next action is to combine this closure with the remaining required independent design reviews and settle the design checkpoint before dependent implementation. Implementation must deliver the specified bridge and independent wire-vector regressions. Native qualification, activation and release retain their separate gates.
