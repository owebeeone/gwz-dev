# GwzGitLibraryNextPackages — State-Axis Review

**Review object:** `gwz-core/dev-docs/GwzGitLibraryNextPackages.md` at `5b05768c98f246a4f7ac8c00d5961ba19ebc5310`; pre-implementation scope proposal dated 2026-09-21  
**Baseline:** root `74fa1855fcc5ae336ba9edc4541e164b5c6fc45a`; gwz-core `5b05768c98f246a4f7ac8c00d5961ba19ebc5310`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed sources were read with `git show`; all tuple commits and worktree HEADs matched at start and end.  
**Date:** 2026-09-21  
**Axis:** Durable-state semantics, isolation, failure aftermath, and qualification-state completeness. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks the scope. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

Read the complete proposed scope; accepted `GwzGitLibraryDesign.md`, `GwzGitLibraryApi.md`, `GwzGitLibraryG0.md`, `GwzNoFallbackPlan.md`, and the root `CurrentProgramCheckpoint.md`; L3-A/L4-A characterization records; and the named commit/tag and path-history implementation and test helpers.

I inspected the workspace consumer set with `gwz status`, then inspected the active `gwz-py` manifest and lock solely to test Q1’s “every consumer and lock source” claim. No builds, tests, experiments, edits, or Git mutations were performed. The only root noise remained the two allowed untracked review prompts.

## 1. Findings

### [P2-1] Q1’s enumerated consumer/lock set omits the active Python consumer

**Location:** `GwzGitLibraryNextPackages.md:120-137`, especially the purported complete inventory at lines 122-125.

**Violated invariant:** A source/readiness gate must enumerate every independently resolved consumer graph before activation can claim one qualified Rust/native provider.

**Concrete state sequence:** `gwz status` identifies `gwz-py` as an active workspace member. Its manifest depends on `gwz-core` and owns an independent `Cargo.lock`. At the observed member HEAD, that lock resolves `git2 0.21.0` with `libgit2-sys 0.18.5+1.9.4`, while the reviewed core lock resolves `libgit2-sys 0.18.8+1.9.7`. Q1 names core, repo-inspect, local-testrepo, CLI, the direct sys dependency, and gwz-git, but neither names nor pins the Python consumer and its lock. Following the stated list can therefore produce a green qualification report while an active product consumer still resolves a different native implementation.

**Impact:** The later activation decision could falsely claim all-consumer/native-provider qualification and leave the Python extension on an unqualified composition.

**Required correction:** Add `gwz-py` and its independent lock/package path to Q1’s required inventory and source pinning. Require the eventual qualification report to resolve and inspect that graph, include it in lock/provider uniqueness and applicable native-platform/package checks, and keep activation pending if it differs.

**Closure check:** On the corrected scope, verify that the named consumer/lock inventory accounts for every active member returned by `gwz status`; the eventual Q1 report must record `gwz-py`’s exact revision, resolved git2/sys identities, and isolated qualification disposition.

## 2. Invariant analysis

C1 otherwise has a closed test-only owner and credible isolation boundary. Its self-reexecuted child clears ambient configuration, prohibits process-global mutation, requires exact-test execution, and fixes signing/hooks/editor inputs. The success rows inspect stored signatures, dates, messages, object forms, refs, index and worktree state. C-EMPTY distinguishes message rejection from empty-tree rejection and records refs, staged content, object/reflog aftermath, and possible message-file residue without promising rollback.

H1 fits the existing characterization module and helper visibility without new production wiring. Direct-member routing, bare repositories, and `info/attributes` overrides are separated; native and current ordered IDs are asserted independently. Refusal remains an admissible observation, while member fan-out and lost magic remain explicitly unresolved. Fixture-local remotes, post-construction snapshots, hermetic children, and exact-test checks bound external and durable-state effects.

All later APIs, omitted rows, platforms, publication, replacement, and activation are accurately deferred.

## 3. Risks and next action

Deferred platform execution and implementation behavior remain ordinary later-gate risks. The next action is a documentation-only scope correction adding the omitted Python consumer graph, followed by focused State closure before C1/H1/Q1 implementation begins.
