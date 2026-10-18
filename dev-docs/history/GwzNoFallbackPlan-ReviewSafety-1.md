# GwzNoFallbackPlan — SAFETY-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzNoFallbackPlan.md` at core `bf9446762a7c51358679ed04e147aa51dedfb2bc`; draft plan-stage correction; 2026-09-20  
**Baseline:** root `57a0aba0a808417cb4c72a796ddb8926ce85179b`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`. Sources were read from immutable commits with `git show`/`git diff`.  
**Date:** 2026-09-20  
**Axis:** Safety — what the corrected plan permits to go wrong. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — **P0 0 · P1 0 · P2 0 · P3 0**. The correction introduces no new Safety finding and closes the planning and activation paths that could otherwise bypass characterization.

---

## 0. Evidence base

I read:

- The prior Safety report and merged remediation plan at root `57a0aba`.
- The complete plan diff `6c592c924e248399cedbdffaafe49d5012870ce5..bf9446762a7c51358679ed04e147aa51dedfb2bc`; only `dev-docs/GwzNoFallbackPlan.md` changed.
- Corrected plan lines 83–142, 175–226, and 290–329.
- The applicable ownership, package-budget, scope-stop, and activation rules in `AgentProcessRules.md`, including L1-05, L1-06, L1-10, L1-32, and §10.2.

`git diff --check` passed. No tests or builds apply to this documentation-only gate and none were run.

The exact tuple was unchanged at the start and end:

| Repository | Commit | Tree |
|---|---|---|
| root | `57a0aba0a808417cb4c72a796ddb8926ce85179b` | `b560504911efbbeaeaeaf0ae2878523b8363f171` |
| core | `bf9446762a7c51358679ed04e147aa51dedfb2bc` | `6a3f9c6b3d705adb356a0cd1a733dea5f2c81e44` |
| transport | `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` | `bf275d0e746b8fc5104f7037cc33e8e685005bc1` |
| taut | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |

Core was clean. Root contained only the two authorized untracked round-one prompts.

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| — | Prior Safety review reported no findings | Prior invariants were retraced across the corrected range; none regressed | N/A |

## Changed-range analysis

The correction adds a mandatory P2 lead checkpoint before any lane implementation, including test runners and fixtures. P2 must freeze shared interfaces, path-level ownership, integration order, review tiers, and grounded numeric ceilings. Unassigned shared paths and TBD ceilings explicitly prevent implementation. P0 and P1 are separately bounded to membership or inspection/documentation with zero production-code and protocol-change budgets.

The ownership table gives every shared surface a sole writer, makes other lanes call-only, and reserves unassigned files and shared harnesses to the integrator. Cross-owner edits and material scope growth trigger review under the standing process rules.

Lane 2 now distinguishes isolated candidate consumption from production activation. Production manifests and locks remain unchanged until the integrator inventories all direct consumers and qualifies the exact candidate package, features, sources, native pins, platforms, object formats, fresh-workspace resolution, standalone core, CLI, and standalone release packaging. Any candidate source, pin, or feature change requires relevant requalification. Missing platform evidence leaves activation pending, and activation remains a separate reviewed change.

The completion definition now includes subprocess routes discovered by the refreshed inventory rather than only the original four. No changed text activates SSH, message, pool, protocol, or production dependency routing.

No change outside the merged dispositions was found, and there is no new architectural root cause.

## 2. Invariant analysis

**No implementation before characterization:** Lines 85–98 bar lane code, runners, and fixtures before reviewed P2 acceptance. The lane dependency table and integration section repeat that prerequisite, leaving no documented bypass.

**No premature dependency activation:** Lines 195–225 keep core consumption isolated and production manifests unchanged. Qualification must cover the exact proposed all-consumer manifest/lock set before a separately reviewed switch.

**Mixed-version and native-link safety:** Qualification requires one native libgit2, preserved consumer features, reproducible packaging without sibling checkouts or root-only patches, and every supported platform and required object format. A partial platform result cannot silently become production activation.

**Ownership and scope containment:** Shared contracts, manifests, lockfiles, package metadata, pins, and release recipes have one integrator owner. Lane ownership is disjoint, and crossing files, owners, protocol scope, or the standing 120% threshold forces review.

**Prior Safety invariants:** The local object/ref, commit/tag failure, no-Git verification, path-history, and separate transport-activation gates remain intact.

## 3. Risks and next action

The numeric ceilings, exact path manifest, shared signatures, and candidate dependency set remain future P2 outputs. That is an explicit plan-stage deferral: the corrected text prevents implementation while any is absent.

Accept the corrected plan-stage gate. Keep implementation paused until the operator resumes P0/P1 and the mandatory P2 checkpoint is filed and independently accepted.
