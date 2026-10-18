# GwzNoFallbackPlan — CONSISTENCY-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzNoFallbackPlan.md` at `6c592c924e248399cedbdffaafe49d5012870ce5`; draft for plan-stage review, implementation paused, dated 2026-09-20  
**Baseline:** root `d3b6cada131fde80726703236ac8eec694a89ccb`; gwz-core `6c592c924e248399cedbdffaafe49d5012870ce5`; gwz-transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`. Committed sources were read with `git show`; relevant clean source files were checked at the pinned core HEAD.  
**Date:** 2026-09-20  
**Axis:** Consistency against the controlling process, core policy, gap inventory, and accepted transport/package graph. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block plan acceptance. I pre-commit to GO on a revision that resolves **P2-1** and **P2-2** as specified.

---

## 0. Evidence base

Read:

- `GwzNoFallbackPlan.md:1-260`.
- `GwzLibgit2Gaps.md:1-225` and pinned production launch sites in `repository.rs:297-321`, `refs.rs:164-263`, `transport.rs:300-366`, and `commit_log/mod.rs:311-360`.
- `GWZDesign.md:670-774`, `GWZRequirements.md` Git/tag, external-tool, and anonymous-local-transport clauses, and ratified AD1 at `history/GwzAuditResolutionPlan.md:45-89`.
- `GwzRemoteTransportNativeBinding.md:17-86`, `GwzRemoteTransportAdapterFoundation.md:15-70`, `GwzRemoteTransportPlan.md:39-160,215-264,356-396`, and `GwzRemoteTransportDesign.md:528-589,785-834`.
- `AgentProcessRules.md` L1-04 through L1-12 and L1-32; `GwzProcessOptimization.md:94-127`; relevant current checkpoint sections.
- `git diff --check` for the plan commit; no errors. No tests or builds were authorized or run.

The exact commits and trees matched at start and end. Core, transport, and taut were clean. Root contained only the two authorized review prompts. The peer prompt/report was not read.

## 1. Findings

### [P2-1] Parallel implementation lacks the required lead interface, ownership, and budget checkpoint

**Location:** Plan §§2–3 and §8, especially lines 62-97 and 224-242. The plan assigns conceptual owners and a final integrator, then allows lanes to begin tests and implementation. It never requires a reviewed checkpoint freezing shared signatures, visibility, files, or integration order, and gives no quantitative package ceilings.

This violates `AgentProcessRules` L1-05, L1-06, and L1-10. The omission is concrete because lanes 1 and 2 may both modify the fork and package inputs; lanes 1, 3, and 4 meet at `GitBackend`; and manifests, lockfiles, and pins are shared.

**Reproduction:** Lane 1 selects the C route while lane 2 packages its Rust binding patch and lane 3 changes backend contracts. Each is permitted to implement before a common file/API boundary exists. Conflicts and scope growth are discovered only during final reconciliation, after the process-required freeze.

**Impact:** incompatible fork baselines, overlapping ownership, and changes too broad to attribute or review independently.

**Required correction:** add a reviewed pre-implementation checkpoint covering every preparation item and lane: shared APIs/signatures, visibility, files owned/call-only/forbidden, fork and manifest ownership, integration order, review tier, and quantitative ceilings for production additions/moves, tests/tools/docs, files, and protocol change.

**Closure test:** the checkpoint table assigns every shared file and interface exactly one writer, exposes no lane-start path that bypasses it, and requires final actual-versus-budget accounting with the standard stop triggers.

### [P2-2] Lane 2 can activate the fork in core before the accepted consumer/platform gate

**Location:** Plan lines 85-90, 146-160, 232-246, and 250-255. Lane 2 independently delivers an API “consumed by core” and explicitly connects the dependency to core. The combined feature/platform matrix appears only in aggregate integration and is not made a prerequisite to that manifest switch.

`GwzRemoteTransportAdapterFoundation.md:17-30` requires production manifests to remain stock until distribution **and platform** gates pass, and requires core, repo-inspect, local-testrepo, and CLI’s direct dependency to select one package while preserving features and native-library identity.

**Reproduction:** Lane 2 publishes or pins the fork, changes core’s manifest, reruns the existing host-local binding/adapter tests, and declares its independent result while another direct consumer still resolves upstream `git2` or a supported platform remains unqualified.

**Impact:** mixed Rust/native package identity, platform build failure, or premature production dependency activation.

**Required correction:** separate isolated/preactivation consumption from production activation. Require, before any production manifest/lock switch, an enumerated all-consumer package/feature/pin matrix, single-native-link proof, supported-platform and object-format gates, standalone release packaging, and a separately reviewed activation change.

**Closure test:** fresh root, standalone-core, and CLI builds on every supported platform resolve the same qualified package/native library and preserve each consumer’s feature set before activation.

## 2. Invariant analysis

The four substantive lanes otherwise match the gap inventory: local import, per-remote binding, commit/tag, and path-sensitive history. P1’s refreshed launch-site audit plus the final no-exec operation matrix is capable of catching the currently separate `tag_create` and `tag_delete` subprocesses. Local-fetch evidence distinguishes negotiation refs, requested non-commit objects, corruption, ref publication, and `FETCH_HEAD`. Lane 3 preserves hooks, signing, identity, staging, failure effects, and internal-commit policy separation. Lane 4 correctly rejects ordinary filtered revwalks and requires ordered parity, simplification, cancellation, replayable randomized evidence, and scaling.

The plan also correctly schedules authority amendments before changed behavior, keeps permitted user hooks/filters/helpers distinct from core Git delegation, preserves the CLI’s named non-GWZ fallback exception, and does not alter frozen transport schema/runtime boundaries.

## 3. Risks and next action

Future route, oracle, packaging, commit-policy, and traversal choices are valid deferred outcomes. Platform parity, fork provisioning, publication, and implementation remain unclaimed.

Revise the plan with the two missing gates and repeat this focused Consistency review before implementation resumes.
