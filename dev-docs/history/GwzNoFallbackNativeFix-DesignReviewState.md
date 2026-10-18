# Native local-fetch correction and isolated Rust integration — STATE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzNoFallbackNativeFix.md` at `09a4db4c6db74a422673417f195dc1f86dceb15d`; DRAFT dated 2026-09-20  
**Baseline:** root `167c0db46e8154e3a24e2e0a51c078b0e04335ff`; core `09a4db4c6db74a422673417f195dc1f86dceb15d`; git2-rs `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`; libgit2 `0551dfd4ad989b6a3d5683c0d4cf326c6efef929`. Sources were read from immutable objects with `git show`, `git diff`, `git ls-tree`, and `git cat-file`.  
**Date:** 2026-09-20  
**Axis:** Durable-state semantics and adversity: failure classification, publication ordering, recovery states, source admission, and provenance. Independent, adversarial, read-only. The other axis ran in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2, or P3 findings.

---

## 0. Evidence base

I inspected:

- `GwzNoFallbackNativeFix.md` lines 9–27, 34–85, and 87–108, including intended error behavior, N1/N2 ownership, source admission, sequencing, budgets, and acceptance boundaries.
- The controlling plan’s Lane 1 investigation and acceptance clauses, the accepted checkpoint’s Lane 1 requirements, `GwzNoFallbackLocalFetchInvestigation.md`, `GwzNoFallbackBindingPort.md`, and the root checkpoint’s current native-package entry.
- libgit2 `local.c` at both main `0551dfd...` and qualified 1.9.7 `49e408b...`, especially `foreach_reference_cb` and `local_download_pack`.
- `revwalk.c`, `object.c`, `tag.c`, `commit_list.c`, and `errors.h` at the pinned libgit2 source.
- The upstream git2-rs sys transition from release `dffaf272...` to `6c93812...`: exactly five `libgit2-sys` paths, 115 insertions/21 deletions, and native gitlink movement from `1affb8b...` to qualified 1.9.7 `49e408b...`.
- The existing binding pin, source-admission implementation, lock normalization, and isolation behavior.

No builds or tests were run, as required.

The tuple was verified at both start and end. Final trees were root `dec6f36574e15d37256321987ae76aa83d213a51`, core `2fdb89d9e88fa9e0e5b6e6833a8c5f0b95eba746`, git2-rs `25d5d7354ca204b805f448c294ed8bd62fdecd86`, and libgit2 `3c062647db3b83542fe5ac5ae3591bcad4ede4ce`. Member worktrees remained clean. Root contained only the two explicitly excluded untracked review prompts.

## 2. Invariant analysis

The proposed native classification matches the pinned implementation. A direct tree or blob reaches `git_object_peel(..., GIT_OBJECT_COMMIT)` and returns `GIT_EINVALIDSPEC`. A successfully parsed annotated tag ending at a tree or blob returns `GIT_EPEEL`. Missing targets return `GIT_ENOTFOUND`, preserving existing behavior. Malformed object parsing, allocation failures, and other lookup failures return different errors and remain propagating. The design requires origin verification through peeling, forbids class/message matching, and includes a malformed-object regression, so the correction is bounded against error masking.

Failure ordering is safe for this change. Receiver hints are classified before `git_odb_write_pack` is acquired or destination pack bytes are written. A genuine receiver-object error aborts during `git_reference_foreach`; an accepted noncommit hint only changes the revwalk exclusion set. Wanted noncommit objects remain independently inserted into the packbuilder. Tests force a real transfer, distinguish wanted objects from receiver hints, check destination object availability and OIDs, and require hint refs to remain unchanged.

The 1.9.7 backport has the same callback and revwalk semantics as main. The exact two-file limit, duplicate focused/full qualification, and N1-before-N2 ordering prevent an unqualified native revision from entering the Rust fixture.

N2’s admission grammar is closed: exact release files and binding hashes, exact upstream sys tree, explicit manifest/submodule exceptions, exact patched C commit, Git-object-derived modes/paths/bytes, isolated copying before Cargo, forced vendoring, and lock comparison permitting only the two named provenance transitions. Missing or mismatched nested source, dirty bytes, wrong sys identity, and unrelated dependency drift are required rejection states. The unpublished sibling dependency and inability to qualify a clean remote-only clone are explicitly reported rather than hidden.

The inspected local transport’s cancellation flag is set but not consumed by `local.c`. This package does not claim to qualify cancellation, remove fallback, or activate production. The controlling plan’s cancellation requirement therefore remains open for the later fallback-removal/activation gate; this GO cannot be cited as closing it.

## 3. Risks and next action

FETCH_HEAD behavior, ref-update atomicity, cancellation qualification, publication, complete consumer/platform coverage, and production activation remain outside this package and retain their existing gates. Successful pack installation may still leave unreachable objects after a later failure under stock behavior; the design neither changes nor accepts that broader publication contract.

Proceed with N1 exactly within the two-file budget, preserving explicit error-origin classification and the malformed-object regression. Only after both native branches qualify should N2 import the pinned sys baseline and exact patched C identity for isolated Rust qualification.
