# gwz-git H1/Q1 Evidence — CODE-AXIS REVIEW

**Review object:** H1/Q1 implementation in `gwz-core` at `deba48c93a04e6aaf0bab36066b12d1547d5469e`; bounded characterization/readiness checkpoint; 2026-09-21  
**Baseline:** root `9fc664de8389ea334f36bc41135cd59448893e05`; gwz-core `deba48c93a04e6aaf0bab36066b12d1547d5469e`; gwz-git `aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable commits.  
**Date:** 2026-09-21  
**Axis:** Code — H1 source characterization and Q1 source/consumer readiness. Independent, adversarial, read-only. The other axis ran independently; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P2 findings; one nonblocking P3 documentation finding.

---

## 0. Evidence base

Reviewed:

- Accepted scope in `GwzGitLibraryNextPackages.md:88-163`.
- H1 implementation in `src/operation/commit_log/path_characterization.rs:9-471`, its existing private helpers, routing entry point, and `GwzNoFallbackHistoryDesign.md:103-160`.
- Q1 inventory and gates in `GwzGitLibraryQualification.md:1-131`.
- Active-member inventory, all relevant Cargo manifests, and the pinned Python and standalone CLI manifests/locks.
- The seven-file core change from `6586768396886fe1aeb1371bbd3064377cfa70ec`.

Independent commands confirmed:

- The permitted focused characterization target passed all 17 selected tests, including both H1 tests, with zero ignored or failed.
- Locked offline metadata passed for root, core, gwz-git, and gwz-py. Each graph had one `git2` and one `links = "git2"` provider, with the versions and provenance Q1 records.
- Start and end commits and trees matched exactly. Only the two declared generated root prompts were untracked.

## 1. Findings

### [P3-1] Historical pending status contradicts the recorded completed H1 run

`dev-docs/GwzNoFallbackHistoryDesign.md:10,78-80` still says the H1 extension is pending its coordinated run, while `:131-137` records the completed 17-test run and says only Code review is pending.

The document therefore gives two incompatible answers about whether the required run occurred. A later auditor reading the status or verification section can incorrectly treat H1 execution as missing.

Update the header and G0-baseline paragraph to distinguish the historical three-test baseline from the now-completed H1 run. Closure is a committed-text check showing no remaining assertion that the H1 coordinated run is pending while retaining the accurate Code-review status and 17-test result.

## 2. Invariant analysis

H1 stays within its accepted test-only boundary. Direct-member cases assert the selected member, preserve the complete pathspec magic envelope during rerooting, and compare exact native and current sequences separately. Bare and `info/attributes` cases pin exact positive and unspecified vectors; refusals or degradations fail rather than count as equivalent behavior. Fixed commit dates make object identities deterministic. Repository/object/index and attribute bytes are snapshotted around read phases.

The self-reexecuted child clears ambient configuration, uses isolated HOME/XDG/config and temporary roots, and proves exactly one fully qualified test ran. No production source, dependency, public API, protocol, or helper visibility changed.

Q1 inventories every active member and every independently resolved product graph, including Python and standalone CLI. It accurately exposes their older `libgit2-sys 0.18.5+1.9.4` locks versus root/core `0.18.8+1.9.7`, and makes any divergent or unqualified consumer an activation blocker. Provider identity, source admission, package reconstruction, Windows runner correction, native execution, and all-platform/all-consumer activation remain explicit future gates rather than claimed results.

## 3. Risks and next action

H1 establishes only direct-member and bare attribute behavior; fan-out and the other named history rows remain deferred. Q1 is a readiness map, not qualification or activation evidence.

Correct P3-1 when filing the checkpoint, then proceed with the separately gated follow-up packages.
