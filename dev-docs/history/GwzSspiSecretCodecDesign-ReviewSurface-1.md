# GWZ SSPI secret codec and caller values — SURFACE-AXIS REVIEW 1

**Review object:** Focused documentation closure of P3-1 from `GwzSspiSecretCodecDesign-ReviewSurface.md`. Revised member checkpoint `44879481fbd54dab84b99fecdadc89a34a84dcbd`; root unchanged at `e9f80c697acc5860ad90dbf5acd5888ccf2bd586`. Authentication and release remain gated. Reviewed 2026-10-03.

**Baseline:** Previous member `e3851768da8d58140d92590bf61575f6edbe333c`; revised member `44879481fbd54dab84b99fecdadc89a34a84dcbd`. Root `e9f80c697acc5860ad90dbf5acd5888ccf2bd586` and reference gwz-core `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` are unchanged. Caller documents were read with `git show exactSHA:path`. All three HEADs matched the revised tuple at start and end.

**Date:** 2026-10-03

**Axis:** Surface: cold discovery, construction, inspection, source ownership and disposal. Independent, adversarial, read-only. Other axes run independently; nothing here relies on their reports. Filed verbatim by the lane owner.

**Verdict: GO** — P3-1 closed; zero open P0, P1, P2 or P3 findings within this review.

---

## Prior-finding closure

| Finding | Disposition | Independent closure evidence |
|---|---|---|
| P3-1 — Missing public import paths and complete implemented-value walkthrough | **Closed** | Revised `docs/CallerValues.md:62–112` names the crate-root exports and supplies a complete example constructing both secret types, TokenLimit, Explicit identity and AuthRequest; using borrowed accessors; and disposing of caller sources separately from library copies. The walkthrough no longer requires guessing public paths or construction fields. |

The lane owner reports that the exact documented example passed as a Cargo doctest, the existing 12 compile-fail checks passed, and formatting passed. These are owner-supplied execution results, not tests independently executed by this reviewer. Closure also rests on independently repeating the original caller-document counterexample.

## Changed-range analysis

Comparison of the original and revised caller-value guides shows that the original contract text remains at lines 1–60. The new construction walkthrough occupies lines 62–112. It adds:

- Explicit crate-root exports and import statements.
- The external `zeroize` dependency version and required feature.
- Synthetic byte and credential sources held by `Zeroizing`.
- Accessor use and separate source/copy disposal.
- A complete AuthRequest with every field supplied.
- Warnings that the binding is synthetic and request construction neither validates the complete profile nor authenticates.
- Error-path source cleanup guidance.

The member README’s closing status now records completed dual secret-boundary review while continuing to identify authentication and release as gated. The root caller guide and release instructions preserve their existing capability boundaries.

The lane owner states that the member revision changes documentation, status/testing material and a Rustdoc include, with no executable API or protocol change. This Surface review inspected only permitted caller documents; it did not independently audit that source-change assertion.

## 0. Evidence base

Standing process instructions and the canonical Surface mandate were retained from the initial review. No conflicting authority was introduced by this focused closure request.

Documents read in full during this round:

| Document | Commit | Lines |
|---|---|---:|
| Member `docs/CallerValues.md` | `44879481fbd54dab84b99fecdadc89a34a84dcbd` | 1–112 |
| Member `README.md` | `44879481fbd54dab84b99fecdadc89a34a84dcbd` | 1–51 |
| Member `RELEASE.md` | `44879481fbd54dab84b99fecdadc89a34a84dcbd` | 1–155 |
| Root `dev-docs/GwzSspiCallerGuide-DRAFT.md` | `e9f80c697acc5860ad90dbf5acd5888ccf2bd586` | 1–122 |
| Original member `docs/CallerValues.md` | `e3851768da8d58140d92590bf61575f6edbe333c` | 1–60 |

All reads used `git show exactSHA:path | nl -ba`. Start and end `git rev-parse HEAD` checks succeeded for root, member and core and returned the revised tuple.

No source, design, plan or current peer report was read. No builds, tests, worker execution, writes or Git mutations were performed. Auto-managed root working changes and current reports/prompts were outside scope.

## 2. Invariant analysis

The original first-day counterexample now fails:

1. The README and root guide lead to the implemented-value guide.
2. The guide states that the public types are exported from `gwz_sspi` and supplies their imports.
3. A caller can construct SecretBytes and SecretText without guessing signatures or ownership.
4. TokenLimit construction and inspection explicitly use raw bytes.
5. The example supplies every AuthRequest field and demonstrates Explicit identity construction.
6. Borrowed accessors inspect values without transferring ownership.
7. Caller sources and library-owned copies have separate, visible disposal steps.
8. `Zeroizing` retains source cleanup on error paths as well as success.
9. The synthetic fixture warnings prevent the example from implying verified TLS binding or functioning authentication.

The surrounding invariants remain documented: no Clone/Debug for secret-bearing values; no TokenLimit default; full request validation occurs after struct construction at the private adapter; worker calls remain unavailable; and private codecs remain inaccessible.

No new finding arose from the added walkthrough.

## 3. Risks and next action

This verdict closes the documentation defect. It does not independently establish compiled API behavior, zeroization implementation or native qualification. Runtime deferrals remain outside scope.

The next action is to record P3-1 as closed at this revised tuple. No further Surface remediation is required for this object. The final tuple matched the starting tuple exactly.
