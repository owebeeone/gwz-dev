# gwz-git G0 Implementation — CODE-AXIS REVIEW

**Review object:** `gwz-git` complete G0 candidate at `fcf5296095d230f46c8409a8d6d47b44f4ad918b`, with checkpoint `gwz-core/dev-docs/GwzGitLibraryG0.md` at `039eb1f315339a61d3d4f1cf5db5b79dce74c4ce` and root membership at `3a6d28f77591acec00334a64877b6411d29f8231`; status **implemented local candidate, acceptance pending**, dated 2026-09-21.  
**Baseline:** root `3a6d28f77591acec00334a64877b6411d29f8231`; core `039eb1f315339a61d3d4f1cf5db5b79dce74c4ce`; library `fcf5296095d230f46c8409a8d6d47b44f4ad918b`; git2-rs `4c1caabbce7d56426c763dd94114052302b23e4c`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects and checked-out files only after exact-commit verification.  
**Date:** 2026-09-21  
**Axis:** Architecture, interfaces, native call paths, ownership, error preservation, and compatibility reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block; one P3 finding is also open. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified; P3-1 is bounded and should be corrected in the same patch.

---

## 0. Evidence base

I read all twelve committed library files, the accepted design/API, the G0 checkpoint, root registration/exclusion, the resolved lock graph, and relevant pinned git2-rs/libgit2 implementations.

The allowed Rust 1.95 offline suite passed from an isolated target directory:

- four foundation integration tests;
- seven native-baseline integration tests;
- one usage doctest;
- two ownership compile-fail doctests;
- no ignored or failed tests.

`cargo metadata --locked --offline` confirmed one local git2 0.21.0 provider and one local libgit2-sys 0.18.8+1.9.7 provider, with only `unstable-sha256`/`vendored-libgit2` and `unstable-sha256`/`vendored` selected. I did not duplicate the full native proof because exact fork/C identities were unchanged and no evidence required it.

The tuple was unchanged at both boundaries. End trees were root `0896513915503d1b764296ebc5b8a1dcaa7fef9a`, core `b49ffec90db725fd11d44957623850935e8e386d`, library `4c63d41fd7de481e48050ec9c19d499f46f79954`, git2-rs `48c43f1fce89430d51e7f3cea159c509370adb1d`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. Worktrees remained clean except for the three explicitly excluded root prompts; `git diff --check` passed and no library-local target directory was created.

## 1. Findings

### [P2-1] Native graft processing replaces stored commit parents before G0 copies them

**Location:** `gwz-git/src/commit.rs:25-42`; accepted API `GwzGitLibraryApi.md:98-108`; pinned libgit2 `src/libgit2/commit.c:532-566`.

The API requires `CommitRecord.parents` to contain the parent IDs in stored commit order and says missing referenced objects do not remove their IDs. The implementation validates with `Repository::find_commit` and then copies `native.parent_ids()`.

Pinned libgit2 parses the stored header, then explicitly replaces the commit’s parent array from `.git/info/grafts` or the repository’s `shallow` file. Its own graft tests confirm that `git_commit_lookup` returns the rewritten parent count, including zero parents for a shallow-boundary commit.

A concrete sequence is:

1. Store commit `C` whose raw header names parent `P`.
2. Mark `C` shallow, or add an `info/grafts` entry replacing `P`.
3. Open through G0 and call `read_commit(C)`.
4. G0 returns no parent or the graft replacement, although the object stored under `C` still names `P`.

This makes the supposedly owned object record depend on traversal metadata and gives callers false commit content. Extract tree and ordered parent IDs from the already validated raw ODB header, or use a reviewed native raw-parse binding that bypasses graft application. Do not modify or suppress repository graft metadata. Add regressions for both a shallow boundary and an explicit graft, asserting that G0 returns the raw object’s parents while ordinary native traversal remains graft-aware.

### [P2-2] “Raw” native error-class conversion discards classes present in pinned libgit2

**Location:** `gwz-git/src/error.rs:51-60`; pinned git2-rs `src/error.rs:305-350` and `libgit2-sys/lib.rs:324-361`; pinned libgit2 `include/git2/errors.h:79-117`, `src/libgit2/grafts.c:142-169`, and `src/libgit2/repository.c:877-917,1149-1154`.

G0 promises to retain native diagnostic code, class, and message. It records `git2::Error::raw_class()`. That accessor does not expose the stored integer unchanged: it recognizes the older bound enum and maps every unknown class to `GIT_ERROR_NONE`.

Pinned libgit2 has `GIT_ERROR_INTERNAL` and `GIT_ERROR_GRAFTS`, while the pinned Rust sys enum stops at `GIT_ERROR_HTTP`. A malformed `.git/info/grafts` file causes repository opening to set `GIT_ERROR_GRAFTS`; `open_exact` then returns a `NativeDiagnostic` whose class is zero instead of the native graft class. The current diagnostic tests exercise older recognized classes and therefore miss this loss.

Add a narrow git2-rs/sys correction that exposes every class in the selected native ABI or provides an actual raw integer accessor, then update the exact source pin/proof. Add a malformed-grafts opening regression that compares against the native `GIT_ERROR_GRAFTS` value, not against the same lossy `git2::Error::raw_class()` accessor.

### [P3-1] The implementation adds public error traits outside the accepted G0 surface

**Location:** `gwz-git/src/error.rs:12-24`; accepted API `GwzGitLibraryApi.md:111-130`; design `GwzGitLibraryDesign.md:25,86`.

The accepted complete surface makes `Error` opaque and specifies `Debug`, `Display`, and `std::error::Error`. The implementation additionally exposes `Clone`, `Eq`, and `PartialEq` for `Error`, and cloning/equality for `NativeDiagnostic`, without an accepted contract for their semantics.

Downstream code can immediately require those traits; removing them when later operation errors gain richer sources would be a compatibility break. Remove the unreviewed derives or explicitly amend and review the API contract. A compile-fail check can guard the intentionally absent traits if they are removed.

## 2. Invariant analysis

Full-width SHA-1/SHA-256 parsing, case normalization, format-sensitive equality, pre-lookup format rejection, and opaque validated ID conversion held. The only internal `expect` receives an opaque ID whose byte length is fixed by its format.

Exact opening uses `NO_SEARCH` without `FROM_ENV`; normal, Git-directory, bare, unborn, nested rejection, linked-worktree, hostile `GIT_DIR`, and missing-path behavior passed.

Commit message bytes, embedded NUL, raw identity bytes, encoding, numeric times, missing referenced objects, malformed/non-commit rejection, and owned lifetime behavior passed for the tested non-grafted repositories. The parent extraction failure arises specifically after native validation rewrites the parsed parent array.

`Repository` is observably `Send`, remains `!Sync` and `!Clone`, and survives sequential cross-worker read/drop. Independent SHA-256 handles completed concurrent reads against vendored 1.9.7.

Production source contains no subprocess, network, credential, environment, cwd, ref, index, or worktree mutation path. The Git process in the linked-worktree test is fixture construction only. State-preservation and no-Git-on-PATH tests passed.

Root membership is local-only, the independent workspace is excluded correctly, budgets hold, and no production consumer or later operation API was activated.

## 3. Risks and next action

Four native platform rows, publication, remote-only reconstruction, and production activation remain explicitly deferred and are not findings here.

The next action is one bounded remediation spanning the library and its pinned binding: derive stored tree/parents from validated raw commit data, expose lossless native error-class integers for the selected ABI, add graft/shallow regressions, and reconcile the extra public error traits. Then rerun the focused Code re-verdict on the updated exact tuple.
