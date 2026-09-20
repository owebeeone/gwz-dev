# GWZ-GIT LIBRARY DESIGN AND G0 API — STATE-AXIS REVIEW

**Review object:** `gwz-core` commit `7dd9f37fecba8d8193585a2dba82be9db4ae5bbf`, `dev-docs/GwzGitLibraryDesign.md` and `dev-docs/GwzGitLibraryApi.md`; DRAFT interface proposal, unimplemented and unpublished  
**Baseline:** root `37d5038f820f9ecd3934f37545029f80d3d45b58`; `gwz-core` `7dd9f37fecba8d8193585a2dba82be9db4ae5bbf`; `git2-rs` `4c1caabbce7d56426c763dd94114052302b23e4c`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Committed sources were read from immutable objects with `git show <SHA>:<path>`.  
**Date:** 2026-09-20  
**Axis:** Durable-state semantics and adversity: ownership, lifecycle, state transitions, read isolation, failure direction, dependency composition, and future durability gates. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2, or P3 findings. This approves the proposed architecture and G0 API shape only; it does not accept an implementation, platform qualification, publication, or production activation.

---

## 0. Evidence base

I read:

- `GwzGitLibraryDesign.md` lines 1–43 for authority and ownership; 45–75 for placement, exact native composition, lock/source proof, and consumer gates; 77–112 for the complete G0 state contract; 114–186 for future commit/tag, history, fetch, and transport boundaries; and 188–231 for package ownership, budgets, tests, platform gates, and activation limits.
- `GwzGitLibraryApi.md` lines 1–56 for setup, open/use/drop lifecycle, path behavior, worker restrictions, and snapshot disclaimer; 58–102 for object-format and owned-record semantics; and 104–125 for failure categories and native diagnostics.
- The controlling no-fallback plan, preparation, checkpoint, native-fix, commit/tag characterization, history characterization, remote-transport design, and relevant product design/requirements sections.
- Pinned `git2-rs` capabilities at `src/repo.rs:207,464,474`, `src/lib.rs:582`, `src/oid.rs:109,284`, `src/commit.rs:96,120,195,311`, `src/time.rs:38,44`, and `Cargo.toml:52`.
- Pinned libgit2 declarations and implementation around `include/git2/commit.h:121,213`, `include/git2/repository.h:108–181`, and `src/libgit2/repository.c:1107`.

The source trace confirmed that the proposed layer can use exact-open/`NO_SEARCH`, repository object-format and common-directory queries, SHA-1/SHA-256-aware IDs, raw commit message/header access, ordered parent IDs without recursive object loading, and owned copies of native diagnostics. No missing native primitive forces a subprocess or an expansion of the C fork for G0.

I ran only permitted inspection commands: tuple/tree resolution, repository and workspace status, immutable-object reads, searches, and `git diff --check`. No build or test was run. The final tuple matched the baseline exactly:

- root tree `d34cc9a06906a6a6f9a0ad129295fd05d9113e1e`
- `gwz-core` tree `a453206cb82cac24a2849b0059113053ce2487c5`
- `git2-rs` tree `48c43f1fce89430d51e7f3cea159c509370adb1d`
- `libgit2` tree `af52de522aacf3593250c45159a038801153c6b5`

All member repositories remained clean. Root contained only the three explicitly excluded untracked review prompts.

## 2. Invariant analysis

**Repository lifecycle is closed and non-durable.** The only G0 transition is exact open, synchronous reads, then `Drop`. Borrowed path accessors cannot outlive the handle; commit records and errors own their data and can. There is no explicit close operation whose success could be lost, because native release has no durable completion outcome. The API denies repository locking and snapshot semantics, so an external ref or object-database change between two reads may produce two different valid observations without violating the contract.

**Reads fail closed.** Full object IDs are tied to an explicit SHA-1 or SHA-256 format. Wrong-format IDs fail before native lookup; missing, malformed, and non-commit objects remain errors; missing referenced parents or trees do not erase the parsed IDs. Errors cannot be mistaken for EOF or empty data, and native code, class, and message remain separate owned fields.

**Exact opening does not broaden admission.** `NO_SEARCH`, no `FROM_ENV`, no parent discovery, and no implicit `HEAD`/ref/revspec resolution prevent ambient state from selecting another repository or object. Worktree, Git-directory, linked-worktree, and bare roots are intentional admissions. Symlink and Git-file indirection are explicitly ordinary filesystem behavior rather than a sandbox promise.

**G0 does not acquire mutation or process authority.** The design prohibits repository creation, ref/index/worktree changes, global native-setting changes, environment or current-directory mutation, subprocesses, helpers, hooks, credentials, lazy fetch, and network activity. Existing direct git2 users remain until separately reviewed migrations; accepting this design removes no fallback.

**Dependency state is bounded.** The path dependency is explicitly declared insufficient as a pin. Acceptance requires exact Rust and nested C source verification before and after testing, an owned lockfile, native-version assertion, and existing source-byte proof. Clean reconstruction, publication, all-consumer resolution, platform qualification, and duplicate native `links` prevention remain activation gates. Registry or system-library substitution is expressly disallowed.

**Future durable transitions are not accidentally authorized.** Commit/tag work must distinguish pre-publication failure, published-with-advisory-failure, and indeterminate verification; hooks and index changes preclude blanket rollback or retry claims; ref transactions are not described as atomic across refs. History cancellation must remain distinct from EOF, and attribute, missing-object, ordering, and resource-bound semantics require later freezes. Transport, credentials, pooling, workspace locks, markers, snapshots, recovery, and cross-repository ordering remain with their stated owners.

## 3. Risks and next action

G0 remains unimplemented, so its read-only behavior, auto-trait restrictions, raw-field extraction, exact source composition, and unchanged-state assertions still require implementation evidence. The record is deliberately structured rather than a raw commit serialization; omitted headers, signatures, and original date spelling cannot later be inferred from it. Multi-read consistency remains the caller’s responsibility because the handle is not a snapshot.

The next action is the bounded G0 implementation package described in lines 190–222, followed by its specified SHA-format, exact-open, raw-record, lifetime, error, unchanged-state, source-proof, toolchain, and platform gates and independent implementation review. Production dependency activation remains outside that package.
