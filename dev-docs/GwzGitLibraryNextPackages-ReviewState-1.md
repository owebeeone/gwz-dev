# GwzGitLibraryNextPackages — State-Axis Re-Review

**Review object:** Corrected `gwz-core/dev-docs/GwzGitLibraryNextPackages.md` at `6586768396886fe1aeb1371bbd3064377cfa70ec`; focused Q1 scope remediation  
**Baseline:** root `3589aa05092abb7c38d90747384954565510301f`; gwz-core `6586768396886fe1aeb1371bbd3064377cfa70ec`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`  
**Date:** 2026-09-21  
**Axis:** Focused State closure of original P2-1. Independent, adversarial, read-only; no current peer output was consulted.

**Verdict: GO** — original P2-1 is closed; no new findings in the changed boundary.

---

## 0. Evidence base

Read the generated remediation prompt, merged `GwzGitLibraryNextPackages-RemPlan.md`, filed original State report, and the immutable core diff `5b05768c…65867683` limited to `GwzGitLibraryNextPackages.md`.

`gwz status` confirmed `gwz-py` and `gwz-cli` remain active members. Immutable inspection confirmed:

- Python revision `d07d55dacb1725d9306be9c04d157ac29a78e000` depends indirectly through `gwz-core`; its independent lock resolves git2 0.21.0 and libgit2-sys 0.18.5+1.9.4.
- CLI revision `7db07bbdefd2897c07fd0f9e550bf032bd8b1314` has a separate standalone lock that also records libgit2-sys 0.18.5+1.9.4.

No builds, tests, experiments, edits, or mutations were performed. Start and end tuple checks matched. The only worktree noise was the two allowed generated re-review prompts.

## 1. Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| P2-1 — Q1 omitted the active Python consumer and independent lock | **CLOSED** | Lines 122-129 explicitly name and pin `gwz-py`, record its current resolved git2/sys composition, require independent graph inspection, and include it in provider-uniqueness, native-platform, and package gates. Lines 130-134 separately pin the CLI, distinguish root-workspace resolution from its standalone lock, require enumeration of all active GWZ members and direct/indirect dependencies, and make any differing or unqualified consumer block activation. |

## 2. Changed-boundary analysis

The amendment closes both failure paths from the original counterexample. An implementer can no longer satisfy Q1 by checking only core and the root workspace, and a green result from one Cargo lock cannot stand in for another consumer’s independently resolved graph.

The correction remains a qualification-plan change only. It changes no manifest, lock, runtime path, API, C1/H1 scope, or activation state. Its pinning and fail-closed sentence make later drift visible: unresolved or differing consumer state leaves activation pending rather than being inferred safe.

## 3. Risks and next action

Platform execution, remote-only reconstruction, publication, and production activation remain accurately deferred. The corrected scope may proceed to the bounded C1, H1, and Q1 packages under their recorded review gates.
