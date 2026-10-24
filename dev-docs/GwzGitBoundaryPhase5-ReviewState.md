# Phase 5 rows A, B and guard (steps 5.2, 5.3, 5.5) — STATE-AXIS REVIEW

**Review object:** gwz-git `7444b1f..12521b4302` (4 commits: `0b9d2be`, `a5a4a74`, `e3bc2a4`, `12521b4`) and gwz-core `642211b9..f0bff70f80` (`0b378bd4`, `63fbd5b1`, `849691c7`, `f0bff70f`).
**Baseline:** gwz-core `642211b9` (A1 amendment), gwz-git `7444b1f` (5.1b). Lane `/Volumes/projects/limbo/gwz-dev-p5-gitmap`.
**Date:** 2026-10-25.
**Axis:** State.
**Verdict:** **NO-GO** (P0 0, P1 0, **P2 1**, P3 2). I commit to GO once P2-1 has its bounded remedy and closure test. The P3s can close in the same package or be recorded as dated follow-ups.

## 0. Evidence base

- **Tuple at start and at end (unchanged):**
  - gwz-core `f0bff70f8035bf6aede24452dfd8374178acde15`, clean.
  - gwz-git `12521b4302a2e68fd1350da2aa6b633808ad7e88`, clean.
  - gwz-core-evidence `efba6961a9d8cc73865fd92a82e8e9ff695fe4b4`. It has one untracked directory, `campaigns/transport-qualification/runs/2026-09-22-alpha-setup-timeout/`. That directory is not part of this object and I did not create it.
  - Other members:
    - git2-rs `d13951f7e0`
    - libgit2 `b172e3d187`
    - gwz-cli `cea7bd4cab`
    - gwz-py `71d2d07249`
    - gwz-transport `a27817b9c8`
    - gwz-credential `6d78c593a4`
    - gwz-sspi `a2978a8f3f`
    - taut `913e0dc35c`
    - taut-shape `89916c4d01`
    - taut-shape-rs `959f0f048a`
    - taut-shape-py `da79bfb743`
    - lane root `f0e7c0ecd2`
- **Load:** the 1-minute load was 48–74 for the whole review. I ran no `cargo test`/`cargo build`, as the brief advises. My dynamic evidence is one `rustc +1.95.0` link of a small program in my scratchpad against the lane's existing gwz-git rlib. The rlib is `build-targets/p5-gitmap-gwzgit/debug/deps/libgwz_git-a6bf205a04c2a332.rlib`, built 04:56; no gwz-git source file is newer than it. That link wrote nothing in the lane.
- **Read:**
  - The full diffs of both ranges.
  - gwz-git `src/native_commit.rs`, `native_commit/{cli,verify,exec,environment,route}.rs`, `src/{refs,objects,graph,status,remotes,index,error,repository,object_id}.rs`.
  - gwz-core `git/gitbackend/{commit_tag,reads,backend}.rs`, `git/verify_checkout_state.rs`, `git/spawn_gate.rs`, `gitbackend/gwz_git_tests.rs`, `workspace_ops/handle_tag.rs`.
  - The old files at `642211b9`: `native_commit.rs`, `repository.rs`, `refs.rs`, `repository_support.rs`, `verify_checkout_state.rs`, `comparison.rs`, `transport.rs`.
  - `scripts/checks/{check_filesystem_boundary.py, check_gwz_git_adoption.py, gwz_git_boundary.json, gwz_git_adoption.json}`.
  - Guide A1–A3.
  - Root plan §4 Phase 5 and D-K.
  - libgit2 `revparse.c`, `repository.c` (open/worktree), `merge.c`.
- **Evidence runs read:**
  - `2026-10-25-p5-2`, `-p5-3` and `-p5-5` READMEs.
  - The `raw/inventory.md` files.
  - The Windows logs for gwz-git and for receipt b.
  - I name-diffed the macOS and Windows gwz-git test lists (`mac/12-gwzgit-test.log` against `windows/gwz-git/03-test.log`).
  - Every patched blob in `p5-5/raw/source/{gwz-core,gwz-git}.patch` equals the committed blob at `f0bff70f`/`12521b4`; the only extra committed file is the pin `.github/gwz-git.commit`.

## Problem, requirement provenance, and scope

- **Problem:** move native commit and tag (G1, G9) and the single-repository refs and reads (G2) from gwz-core to gwz-git. Core delegates in the same change, the old path is removed, and a guard plus an adoption check prevent regression.
- **Authority:**
  - D-G and D-K: F2 is superseded, and G3/G7/G8/G10 stay in core.
  - Git boundary plan §3/§5/§6.
  - R9.
  - Root plan Phase 5 rows 5.2, 5.3 and 5.5.
- **Changed members:** gwz-git, gwz-core and the evidence member only. This is within scope.
- **Lane-owner additions I assessed:**
  - **(a) 5.5 moved `repository_paths`/`repository_index` behind `reads.rs`.** The capability map §2.2 lists these as G2 callers, and the 5.3 README's finding 1 shows the decision being taken. The behaviour is byte-identical: the same `NO_SEARCH` open, the same index iteration, and `ObjectId` converted back to the same raw bytes. Opening a linked-worktree gitdir is supported by libgit2 (`repo_is_worktree` and `load_workdir` read `gitdir`). Provenance is adequate and this is not an expansion.
  - **(b) 5.2's reading of "the reasons stay in core".** Whether anyone is told stays in core (`with_route_notes`), while `CliReason` moved with the rule. This has no state effect.
- **Pre-existing behaviour I did not count as findings:**
  - The CLI route's live-environment inheritance (5.2 finding 7).
  - The ungated `ProcessGate` for a bare consumer (Phase 1).
  - The gwz-cli/gwz-py lockfiles (5.2 finding 1, outside the lane).

## 1. Findings

### P2-1 — The AD1 tag self-verification resolves revisions where it needs an exact ref lookup: `tag_delete` reports failure after a real delete, and `tag_create`'s check can confirm a tag that does not exist

- **Location:** `gwz-git/src/native_commit/verify.rs` (new in `a5a4a74`), `tag_created` and `tag_deleted`. Both use `Repository::resolve(&format!("refs/tags/{name}"))`, which is `revparse_single`.
- **What the old code did:** core at `642211b9` (`refs.rs: verify_tag_created`/`verify_tag_deleted`) tested existence by exact membership in `tag_list`, which enumerates `refs/tags/*`.
- **Root cause:** libgit2's `revparse_lookup_object` (`libgit2/src/libgit2/revparse.c:93-129`) tries these in order: a full sha, then `git_reference_dwim`, then an abbreviation, and finally `maybe_describe`.
  - `maybe_describe` uses an unanchored regex, `.+-[0-9]+-g[0-9a-fA-F]+`, and resolves the hex after the first `-g` as an object prefix.
  - So once `refs/tags/<name>` is absent, a name in `git describe` form (`build-12-g1a2b3c4`, `v1.0-3-gface`) still "resolves" to whatever object has that prefix.
  - The same applies to the dwim fall-through rules: after a delete, a branch named `refs/tags/X` (`refs/heads/refs/tags/X`) also resolves.
- **Violated invariant:** AD1 must prove the state it claims; "may lose progress, never invent it."
- **Reproduction, run against the lane's built rlib with rustc 1.95.0:**
  - Setup: `git init` a repository and make two commits (HEAD `75861e0…`). Then call `native_commit::tag_create` and `tag_delete` with name `nightly-1-g75861e0`, `Services::new(GitEnvironment::from_vars(env::vars_os()), plain Command::output gate)`.
  - With `RoutePolicy::AUTO` and with `CLI_ONLY`:
    - `tag_create` → `Ok(commit 75861e0…)`.
    - `tag_delete` → `Err(Operation, "tag 'nightly-1-g75861e0' still present after delete")`.
    - `git tag -l nightly-1-g75861e0` afterwards → empty, so the tag was deleted.
    - A retry → `Err("git tag -d failed: error: tag 'nightly-1-g75861e0' not found.")`.
  - `Repository::resolve("refs/tags/never-created-3-g75861e0d")` → `Ok(Some(75861e0…))`. Adding `^{commit}` gives the same result. This is exactly the probe `verify::tag_created` runs. If a tag in describe form disappears between write and verify (a concurrent `git tag -d`, or a CLI route that exits 0 without creating it), `tag_create` returns `Ok(TagOutcome)` with a commit taken from the describe fallback.
- **Impact:**
  - `gwz tag delete <name>` (`workspace_ops/handle_tag.rs:251-260`) fans out over root and members and stops at the first `?`.
  - For a name in describe form, each run deletes the tag in one repository, then reports `GitCommandFailed`. The next run skips that repository (`tag_list` no longer has the tag) and fails on the next one.
  - The workspace converges only after N failing runs, and every run reports a failure that did not happen. A short hex suffix (fewer than 4 digits) or an ambiguous prefix turns into an `ObjectRead` error on the same path.
  - On create, the AD1 check is fail-open for this name class.
  - This is a regression against the pre-move core path and against 1.0.17, and it is identical on all three platforms.
- **Correction:**
  - In `verify.rs`, test existence by exact reference lookup. Either use `find_reference("refs/tags/{name}")`, mapping `NotFound` to absent (a `pub(crate)` helper is enough), or use `tag_names()` membership as the old code did.
  - Take the commit by peeling the found reference.
  - Keep `resolve("HEAD")` for the commit check; it is unaffected.
  - Correct guide A2's sentence that says the tag checks use `Repository::resolve`.
- **Closure test:**
  - In gwz-git, use `on_both_routes!`: create and delete the tag `t-1-g<abbrev of HEAD>`, and expect `Ok` from both and no tag left.
  - Add a verify unit test: `tag_created` for an absent describe-form name must return `Err("… missing after creation")`.
  - Add a dwim case: after deleting tag `X`, with a branch `refs/tags/X` present, `tag_deleted` must return `Ok`.

### P3-1 — The adoption check credits git2 calls as gwz-git consumption, so its ratchet can record false adoption

- **Location:** `gwz-core/scripts/checks/check_gwz_git_adoption.py`, `first_caller`. A file counts as a caller when it names `gwz_git` anywhere and has the owner token anywhere, plus `.name(` or `Owner::name` anywhere.
- **Violated invariant:** R9. The adoption evidence must show a production call of the gwz-git capability, and the dated ratchet must not drift without a decision.
- **Evidence:** I enumerated every matching file per capability with the check's own functions.
  - `Repository::head` matches `src/git/verify_checkout_state.rs` and `src/git/gitbackend/repository.rs` as well as `reads.rs`.
  - `Repository::status` and `StatusOptions::new` match `verify_checkout_state.rs`.
  - Those matches come from `git2::Repository`, `repo\n.head()`, `git2::StatusOptions::new()` and `entry.status()`. The file mentions `gwz_git` only in `model_error`.
- **Impact:**
  - Today's GO is correct, because each of these capabilities also has a real caller in `reads.rs`. The 5.3 probe test (`gwz_git_tests.rs:87-143`) and clippy's dead-code check are the actual backstops.
  - But the adoption gate cannot fail for these three capabilities while the spurious files exist.
  - An unconsumed entry such as `ObjectId::format` (`.format(` is a common call) or `Error::native` will be reported "consumed now; delete its entry" as soon as any file that names `gwz_git` makes a same-named call. Deleting the entry then removes the R9 debt record with no real consumer behind it.
  - The `gwz_git_boundary.json` ratchet, by contrast, cannot drift silently: its counts are exact in both directions (attack in §2).
- **Correction:** ignore owner tokens written as `git2::Owner`. For receiver methods, require that the file imports the owner from `gwz_git` (`use gwz_git::{…Owner…}`) or names `gwz_git::Owner`.
- **Closure test:** a fixture with `git2::Repository`, `gwz_git::Error` and `repo.head()` and no gwz-git `Repository` must be reported as "no production caller" for `Repository::head`.

### P3-2 — Guide A2's error contract is wrong for the verification reads: a failure after the write is not `ErrorKind::Operation`

- **Location:**
  - `gwz-core/dev-docs/GwzGitLibraryApi.md` A2, "Errors": "Every failure is `ErrorKind::Operation`".
  - `gwz-git/src/native_commit/verify.rs` and `cli.rs`. These return the errors of `Repository::open_exact` (`RepositoryOpen`/`NotARepository`) and of `resolve` (`ObjectRead`, for example a `^{commit}` peel failure or an ambiguous describe prefix) unchanged. The CLI route's `verify::head` read before the write does the same.
- **Violated invariant:** the recovery grammar must be a documented, closed set. A consumer must be able to tell "refused or failed before the write" apart from "written, verification read failed".
- **State sequence:** a native `exec::tag_create` succeeds, then `verify::tag_created` fails its peel. The result has kind `ObjectRead`, a kind the contract says this module never returns. A consumer that matches `Operation` to classify commit/tag failures mis-files a post-write state as an unrelated read failure.
- **Impact:** small today. gwz-core maps every kind except `InvalidPath` to `GitCommandFailed`, so user-facing codes and text are unchanged (verified: `model_error` copies `Error::message()`, which is the same text the old `ModelError` carried). The cost falls on the next gwz-git consumer R9 brings in, for example gwz-py.
- **Correction:** either map verify-read errors to `Operation` (with a message stating that the write happened and verification failed), or document the full set of kinds and which side of the write each one occurs on.
- **Closure test:** a unit test that forces a verify-read failure after a native write and asserts the documented kind.

## 2. Invariant analysis (attacks that failed, with evidence)

- **G1: the moved code is mechanically identical.**
  - The production diffs against `642211b9` are `exec.rs` (error type only: `failed`→`Error::operation`, `git_error`→`operation_native`) and `environment.rs` (`process()` removed). `route`, `hooks`, `ident`, `message`, `notes`, `staging`, `system_config` and `config_view` are byte-identical.
  - The route rule, the `index.lock` refusal (`route.rs:329`), the HEAD-shape and in-progress refusals, and the cases where the CLI is forced are all unchanged.
  - `RoutePolicy` derives `Default` = `AUTO`. `NATIVE_ALL` is `cfg(test)` and crate-only, so no production or external path reaches native `commit -a`.
- **G1: commit self-verify.**
  - The old `backend.head(path)` used `git2::Repository::open`, which is `open_ext(NO_SEARCH)` (`repository.c:1188`), followed by `head.target()`.
  - The new code is `open_exact` (`NO_SEARCH`) followed by `resolve("HEAD")`.
  - `"HEAD"` is not hex and dwim matches `HEAD` exactly, so the two read the same thing. For an unborn HEAD both give `None`, which leads to an error in both versions. This is fail-closed.
  - A concurrent writer between the write and the verify still produces "HEAD is not the new commit", as before.
  - A kill between the write and the verify leaves the same state as before: the commit landed and no result was returned.
- **G1: outcomes and codes in core.**
  - CLI messages are the same: "git commit failed: …", "git commit did not advance HEAD", "tag '…' still present after delete".
  - `ErrorCode` is `GitCommandFailed` throughout. The `CredentialHelper*` branches of `git_error` were unreachable for local commit/tag.
  - `GitCommitResult` and `GitTagResult` have the same values (`ObjectId::to_string`).
  - `handle_tag`'s recovery grammar is unchanged, apart from P2-1.
- **G1: spawn and environment.**
  - `SpawnGate::output` (`spawn_gate.rs:129`) sets null stdin and piped stdout/stderr, and holds the gate for the spawn only. The trait implementation forwards to it.
  - gwz-git's three `Command::new("git")` sites are its only spawns: grep found no other production spawn, and they are allowlisted as `debt`.
  - The CLI child still inherits the live environment, unchanged by the move. That consequence is real (see §3) but pre-existing.
- **G2: reads.**
  - The status options are verbatim (`include_untracked`, `recurse_untracked_dirs`, both rename passes, ignored only on request). So are the status masks and the per-file characters (now `char`, projected back to the same `String`s), `changed_paths`, `merge_base`, `graph_*`, `commit_exists` and `blob_at`.
  - A missing object in `merge_base` gives a generic error (`merge.c:259`, `on_error` → -1), not `NotFound` → `None`, so it is not falsely reported as "unrelated". Both core callers treat `None` fail-closed (`root_lock.rs:174`, `pull_head_merge_preflight.rs:177`).
  - Strict full-hex ids: shorter or zero-padded text never named an object, so the old code errored as well (or returned `false` in `commit_exists`, which is kept). On SHA-256 repositories the old `Oid::from_str` parsed as SHA-1 and failed, so the new code improves on it.
  - `remotes` is equivalent except in one race: a remote removed between list and lookup now gives `GitCommandFailed` instead of `MissingRemote`. No caller branches on that.
- **G2: `repo_head(&git2::Repository)` → `head_at(repo.path())`.**
  - The reopen reads on-disk HEAD, which the old handle read too, because libgit2 does not cache refs.
  - A linked-worktree gitdir, a bare repository or a `.git/modules` gitdir all open with `NO_SEARCH` (`repository.c` `repo_is_worktree`/`load_workdir`).
  - The G10 `MergeDrift` and recovery-drift checks (`merge_support.rs:42`, `recovery_support.rs:358`) still return their own codes on mismatch. Read errors stay `GitCommandFailed`.
- **5.5: index snapshot.**
  - The iteration and every field are verbatim, and the ids are byte-identical (`ObjectId::from_git2`, then `as_bytes().to_vec()`).
  - Reads while `index.lock` is held, racing `git add`, unsupported mandatory extensions (split index `link`, `sdir`) and a bare repository (empty in-memory index) behave exactly as before, because the native call is the same `repo.index()`.
  - The `checked_artifact` consumers of `ctime`/`mtime`/`stat` are unchanged.
- **Boundary-guard ratchet drift.**
  - `git2_boundary_failures` fails when a count goes up or down, and on a duplicate entry, a missing field, a bad date or an exception file that no longer exists.
  - The `zero` list overlaps no exception.
  - `production_source` blanks only items whose cfg requires `test` (`all`/`any` folding), so platform branches are scanned.
  - Both CI and `run_tests.py` invoke it without arguments, so the git guard is not skipped (it runs only when `paths` is empty).
  - Planted `use git2`, `extern crate git2` and ±1 counts all turned it red (`p5-5/raw/mac/05`).
  - A git2 use in a new unlisted file passes by design (operator-narrowed scope). The 5.3 probe and clippy dead-code catch relocating a delegation away from `reads.rs`.
- **Platform parity.**
  - The gwz-git name-diff gives 15 macOS-only tests (5 characterization bodies × 2 routes inside `#[cfg(unix)] mod unix`, plus the hook-executable, mode-change, typechange, unreadable-file and share-git-core cases) and about 3 Windows-only tests. That nets the README's "121 = 133 − 12".
  - The characterization tests were inside `mod unix` in core too, so gating the whole module on `cfg(unix)` removed no Windows coverage. It only removed dead helpers that Windows clippy found.
  - 5.3 skipped its own core receipt, but receipt `p5-5-20261025-b` ran on T1. T1 contains 5.3 (`849691c7`) and equals the committed blobs. Result: 3,931 passed, 4 failed (the 4 named host gaps), and the guard and adoption lines printed on Windows. Windows coverage of the final tree exists.
  - Not covered on Windows: the hosted-like gaps (taut-proto/gwz-py absent), and GitHub-hosted Windows. Also, `windows_lane_check` can PASS a suite that never ran (5.2 finding 3; receipt b's log shows the suite did run).

## 3. Risks and next action

- **Next action:** fix P2-1 in gwz-git `verify.rs` (exact ref lookup and peel), add its closure tests to the native_commit suite on both routes, and correct guide A2 (P3-2 can go in the same edit). Re-run gwz-git's gates and one Windows gwz-git run, then return for re-review of P2-1 only. P3-1 is a bounded change to the check script plus one fixture.
- **Observation (pre-existing, not counted; 5.2 finding 7):**
  - The CLI route's child inherits the live environment.
  - `GIT_INDEX_FILE`, `GIT_OBJECT_DIRECTORY` or `GIT_WORK_TREE` already in gwz's environment (gwz run from a Git hook, for example) correctly send the operation to the CLI route. But `git -C <member> commit` then writes with them, for example using the outer repository's index file.
  - AD1 only checks that HEAD advanced, so the result is a member commit with the wrong tree, reported as success.
  - The listed remover (`env_clear()` plus the `GitEnvironment` snapshot) should be scheduled before 1.1.0. The move did not make this worse.
- **Integration (outside the lane):** gwz-cli and gwz-py need a `Cargo.lock` with `gwz-git` and sibling checkout before they build against `f0bff70f`. gwz-git must also be published before gwz-core in the Phase 8 chain.
- **Phase 1 finding:** the ungated `ProcessGate` is unchanged in shape. `ProcessGate` is now a public trait, so any gwz-git consumer chooses its own gating. This is the same Phase 1 finding, not made worse.
