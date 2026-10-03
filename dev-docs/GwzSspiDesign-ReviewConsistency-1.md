# GWZ SSPI design package — CONSISTENCY-AXIS RE-REVIEW

**Review object:** DRAFT revision 2, controlled by `dev-docs/GwzSspiDesign.md`, remediation round 1, at root `f2029e4b1739c0214138675dfb16abdb44f6a0d7`.

**Baseline:** Root `f2029e4b1739c0214138675dfb16abdb44f6a0d7`; gwz-core `d78a664e3c5a325c6f12be409eb7645c1c1b51d0`; evidence `1beb1d204c824701ddbd033c7f89df9a3561f5e5`. Documents read through `git show` at these commits.

**Date:** 2026-10-03.

**Axis:** Consistency against the controlling graph and original counterexamples. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on its current report. Filed verbatim by the lane owner.

**Verdict: GO** — original P2-1, P3-1 and P3-2 are closed at the document-contract level. No new findings. This accepts the bounded mechanism/API design only.

---

## 0. Evidence base

Verified all three HEADs at start and end; both checks returned the prescribed tuple unchanged.

Read the entire revised package:

- `GwzSspiDesign.md`, lines 1–274.
- `GwzSspiCallerGuide-DRAFT.md`, lines 1–93.
- `GwzSspiPlan.md`, lines 1–42.
- `GwzSspiDesign-RemPlan-1.md`, lines 1–33.
- Complete package-document diff `377c5e2..f2029e4`, including checks for changes to the crate-map and library-boundary documents; those two documents were unchanged.

Retained the unchanged controlling graph inspected in the original review and rechecked Windows parity’s explicit amendment and §8–§9 clauses, especially Digest inputs at lines 316–323. Verified the newly cited negotiation-state contract against [Microsoft’s primary documentation](https://learn.microsoft.com/en-us/windows/win32/api/sspi/ns-sspi-secpkgcontext_negotiationinfoa).

No writes, builds, tests or mutations occurred. Future executable regressions remain implementation obligations.

## 1. Original finding closure

| Finding | Original counterexample retraced on revision 2 | Result |
|---|---|---|
| **P2-1 — missing initial Digest challenge carrier** | Construct Digest request with explicit identity, challenge C, method, URI and binding; follow `start` → `step(None)`. Design lines 144–154 now place C in Begin and require the first Initialize to consume it. Guide lines 34–40 agrees. Missing, oversized and wrong-package fields refuse before native work, with zeroizing ownership and existing token/frame bounds. No invented exchange or undeclared field is necessary. | **Closed.** Design lines 260–261 require the corresponding fake protocol tests. |
| **P3-1 — finish before Begin unspecified** | Follow successful `start` directly with `finish`. Design lines 155–158 explicitly permit Finish after verified Hello and before Begin, with no native credentials/context initialized. Guide line 25 agrees; design lines 191–193 retain full exit/Job/thread confirmation. | **Closed.** Pre-Begin finish is an explicit future regression obligation. |
| **P3-2 — library secret review stops missing** | Walk the numbered plan from codec/API implementation through native handling and composition. Plan lines 12–13 stop for dual Code/State before step 2; lines 23–24 stop for native secret ownership/disposal review before step 4. Lines 37–39 retain those stops within the cohesive sequence. | **Closed.** The executable schedule now preserves the design and process requirements. |

No new findings were established.

## 2. Changed-range and invariant analysis

**Digest bootstrap:** The correction supplies the missing input without altering HTTP ownership, helper selection or conversation sequencing. Explicit-only Digest identity matches Windows parity §8. Bounds and wiping apply to the newly carried challenge; provider qualification remains gated.

**Finish lifecycle:** The added pre-Begin and post-Complete legality completes the normal disposal contract. It does not weaken cancellation, retained capacity or cleanup confirmation.

**Mechanism observation:** The revised Token payload, caller result and walkthrough consistently distinguish requested Negotiate from actual selected mechanism. Intermediate unresolved/provisional observations do not establish Kerberos identity. Complete requires authoritative selection or refusal before token publication. The treatment of COMPLETE versus OPTIMISTIC/IN_PROGRESS agrees with [Microsoft’s negotiation-state contract](https://learn.microsoft.com/en-us/windows/win32/api/sspi/ns-sspi-secpkgcontext_negotiationinfoa). The API expressly permits Kerberos or NTLM under Negotiate and requires callers needing unsupported restrictions to refuse before start; no new selection-policy knob is implied.

**Deadline shape:** Design lines 216–226 and guide lines 56–61 agree: deadline is immutable after start, earlier enclosing expiry uses the supplied cancellation signal, and no worker budget crosses IPC. Removing the cooperative-budget promise leaves parent supervision as the single deadline authority. Delayed Begin or another challenge cannot obtain a fresh allowance.

**Preserved invariants:** Admission remains charged through confirmed reaping; creation-time containment remains mandatory; late publication remains revoked; identity/CBT and exclusive-route ownership remain unchanged; SSPI errors gain no helper provenance; forced exit remains distinct from normal secret cleanup.

**Architectural classification:** Original P2-1 remains an architectural interface-contract omission, now closed by a bounded schema correction. P3-1 and P3-2 remain non-architectural contract/planning omissions, also closed. The new result and deadline text were reviewed as changed contracts, not assumed covered by the original review. No new architectural root cause was found.

## 3. Risks and next action

This GO establishes document consistency, not production behavior or native provider qualification. Codec wiping, mechanism observation, installed workers, cancellation and cleanup still require the specified implementation tests and reviews. Full Windows parity remains separately NO-GO.

The next action is to merge the independent same-tuple design verdicts; if all required axes report GO, record bounded acceptance and proceed only within the authorized implementation scope and explicit review stops.
