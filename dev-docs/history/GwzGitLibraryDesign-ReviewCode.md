# GWZ Git Library Design and G0 API — CODE-AXIS REVIEW

**Review object:** `gwz-core` commit `7dd9f37fecba8d8193585a2dba82be9db4ae5bbf`, `dev-docs/GwzGitLibraryDesign.md` and `dev-docs/GwzGitLibraryApi.md`; status **DRAFT for interface review; unimplemented and unpublished**.  
**Baseline:** root `37d5038f820f9ecd3934f37545029f80d3d45b58`; `gwz-core` `7dd9f37fecba8d8193585a2dba82be9db4ae5bbf`; `git2-rs` `4c1caabbce7d56426c763dd94114052302b23e4c`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Documents and sources were read from immutable Git objects with `git show`; relevant checked-out files were used only for line-oriented inspection after confirming the same commits.  
**Date:** 2026-09-21  
**Axis:** Architecture, interfaces, call graphs, ownership, and compatibility reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 compatibility finding blocks the G0 interface freeze. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

I read the complete design and API guide, including the ownership map, G0 bootstrap, read-only guarantees, future L1–L4 gates, budgets, proposed signatures, record lifetimes, and error taxonomy. I checked them against the cited no-fallback preparation/checkpoint, native-fix, commit/tag, history, transport, product-design, and requirements boundaries.

Relevant source inspection covered:

- `git2-rs` `src/repo.rs`: `Repository` representation and its explicit `unsafe impl Send`, `open_ext`, repository path accessors, object format, work directory, and common directory.
- `git2-rs` `src/oid.rs`, `src/commit.rs`, and `src/signature.rs`: format-aware IDs, full raw commit message/header access, parent/tree IDs, raw identity bytes, and numeric time fields.
- `git2-rs` and `libgit2-sys` manifests/build selection for `unstable-sha256`, vendoring, HTTPS, and SHA backends.
- `libgit2` object-read strict hash verification and the pinned 1.9.7 SHA-256 implementation/changelog.
- `gwz-core` dependency features and representative concurrent fan-out boundaries.

No build or test was run, as required for this design review.

The tuple was verified unchanged at both review boundaries. End trees were root `d34cc9a06906a6a6f9a0ad129295fd05d9113e1e`, core `a453206cb82cac24a2849b0059113053ce2487c5`, git2-rs `48c43f1fce89430d51e7f3cea159c509370adb1d`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. `git diff --check` was clean in all four repositories. The only root changes were the three explicitly excluded untracked review prompts.

## 1. Findings

### [P2-1] The frozen `Repository` API does not decide its observable `Send` contract

**Location:** `GwzGitLibraryDesign.md:18-20,84-89,203-215`; `GwzGitLibraryApi.md:39-42`; pinned `git2-rs/src/repo.rs:101-116`.

The design says acceptance freezes the complete G0 public surface, declares `Repository` not `Clone` or `Sync`, and requires construct/use/drop on the operation’s worker, but describes cross-thread transfer only as having “no promised” or “no Send guarantee.” That does not determine the Rust API’s actual auto-trait surface.

The wrapped `git2::Repository` explicitly implements `Send`. A direct opaque wrapper containing it therefore becomes `Send` automatically. An equally plausible implementation can add a private non-`Send` marker and become `!Send`. Both can be read as satisfying the prose, yet `assert_send::<gwz_git::Repository>()` compiles for one and fails for the other; moving a repository into a worker thread likewise changes from accepted to rejected. Removing an initially inherited `Send` implementation after G0 acceptance would break downstream compilation, while unintentionally inheriting it would leave the stated same-worker restriction unenforced.

Choose and document one contract before freezing G0:

- freeze `Repository: Send + !Sync`, explicitly permitting ownership transfer between workers while prohibiting shared concurrent access; or
- freeze `Repository: !Send + !Sync`, require the implementation to suppress the underlying auto trait, and retain strict same-worker construction/use/drop.

Add a compile-time trait assertion or compile-fail test for the selected contract. If `Send` is selected, the guide must distinguish sequential ownership transfer from concurrent sharing. This finding is independent of the SHA backend: the pinned native source has corrected the previously reported builtin SHA-256 race.

## 2. Invariant analysis

The remaining attacks did not produce findings.

The proposed dependency selects builtin SHA-256 when HTTPS is absent, and the Rust manifest still warns that this backend is not thread-safe. That warning is stale for the pinned native source: libgit2’s 1.9.4 changelog records the correction, and pinned 1.9.7 has a function-local `addTemp` in `SHA224_256AddLength` instead of the former shared static variable. I found no remaining mutable shared state supporting the old counterexample.

The G0 read path is feasible with the pinned binding. `open_ext` can apply `NO_SEARCH` without `FROM_ENV`; repository, common, and worktree paths remain handle-borrowed; object format is exposed; full IDs can be checked before lookup; and raw message, header, identity, time, tree, and ordered parent data can be copied into owned records. Missing referenced parents or trees need not be loaded.

Ownership boundaries remain coherent: native algorithms stay below the library, single-repository behavior stays in `gwz-git`, and workspace selection, ordering, locks, recovery, fan-out, and result lowering remain in core. G0 exposes no native escape hatch, remote operation, helper execution, subprocess wrapper, or workspace policy.

The bootstrap accurately limits reproducibility to the prepared workspace, requires exact fork/submodule proof and one native provider, and does not claim publication or production activation. Later commit/tag, history, and remote work has named prerequisites and separate freezes; accepting G0 does not authorize those outcomes.

## 3. Risks and next action

Implementation, platform execution, publication, and production consumption remain explicitly pending. The stale SHA warning in the pinned Rust manifest is a maintenance hazard, so the planned native-baseline evidence should retain exact source/version proof and a concurrent SHA-256 regression, but the corrected pinned C source does not justify a blocking finding.

The next action is to amend both G0 documents with one explicit `Send` decision and its compile-time closure test, then perform the focused re-verdict.
