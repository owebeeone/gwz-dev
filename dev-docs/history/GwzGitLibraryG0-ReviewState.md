# GWZ-GIT G0 IMPLEMENTATION — STATE-AXIS REVIEW

**Review object:** Complete G0 implementation at `gwz-git` `fcf5296095d230f46c8409a8d6d47b44f4ad918b`, with bootstrap/membership at root `3a6d28f77591acec00334a64877b6411d29f8231` and checkpoint/API status at `gwz-core` `039eb1f315339a61d3d4f1cf5db5b79dce74c4ce`; local implementation candidate, unpublished and not production-activated  
**Baseline:** root `3a6d28f77591acec00334a64877b6411d29f8231`; `gwz-core` `039eb1f315339a61d3d4f1cf5db5b79dce74c4ce`; `gwz-git` `fcf5296095d230f46c8409a8d6d47b44f4ad918b`; `git2-rs` `4c1caabbce7d56426c763dd94114052302b23e4c`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Durable-state semantics and adversity: exact admission, ownership and drop, read isolation, concurrent access, failure direction, side effects, and dependency/source identity. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2, or P3 findings. This accepts the bounded local G0 candidate, not later operations, publication, platform parity, production dependency activation, or fallback removal.

---

## 0. Evidence base

I read the complete committed G0 package:

- `Cargo.toml`, `Cargo.lock`, `AGENTS.md`, and `README.md`.
- `src/lib.rs` lines 1–37, `object_id.rs` lines 1–89, `error.rs` lines 1–71, `repository.rs` lines 1–52, and `commit.rs` lines 1–77.
- `tests/foundation.rs` lines 1–185 and `tests/native_baseline.rs` lines 1–383.
- Core `GwzGitLibraryG0.md` lines 1–91, accepted `GwzGitLibraryDesign.md` G0/package sections, and the complete `GwzGitLibraryApi.md`.
- Root Cargo exclusion and generated `gwz.conf` membership/lock changes.
- Relevant pinned git2-rs repository, OID, commit, and drop implementations and libgit2 ODB verification/hash paths.

`cargo +1.95.0 metadata --locked --offline` resolved exactly one local `git2` 0.21.0 and one local `libgit2-sys` 0.18.8+1.9.7. Features were `unstable-sha256` and `vendored-libgit2`/`vendored`; no SSH or HTTPS feature was selected. The git2-rs tree records libgit2 as gitlink `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`.

I ran the permitted suite with `CARGO_TARGET_DIR=/tmp/gwz-git-g0-review-state cargo +1.95.0 test --locked --offline`. It passed:

- Four foundation tests.
- Seven native-baseline tests.
- One compiling usage doctest.
- Two compile-fail ownership doctests.
- No ignored tests.

The run independently asserted vendored libgit2 1.9.7. I did not repeat the unchanged eight-test native source proof; I verified the exact Rust/C commits and nested gitlink, while the committed checkpoint records source-byte proof before and after the owner’s library run. Changed-range whitespace checks passed.

Start and end checks matched exactly:

| Repository | Commit | Tree |
|---|---|---|
| root | `3a6d28f77591acec00334a64877b6411d29f8231` | `0896513915503d1b764296ebc5b8a1dcaa7fef9a` |
| gwz-core | `039eb1f315339a61d3d4f1cf5db5b79dce74c4ce` | `b49ffec90db725fd11d44957623850935e8e386d` |
| gwz-git | `fcf5296095d230f46c8409a8d6d47b44f4ad918b` | `4c63d41fd7de481e48050ec9c19d499f46f79954` |
| git2-rs | `4c1caabbce7d56426c763dd94114052302b23e4c` | `48c43f1fce89430d51e7f3cea159c509370adb1d` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` | `af52de522aacf3593250c45159a038801153c6b5` |

All members remained clean. Root contained only the three explicitly excluded review prompts.

## 2. Invariant analysis

**The G0 state machine is closed.** `Repository::open_exact` creates one native handle, read methods borrow it, and ordinary `Drop` frees it. No explicit close outcome can become partially durable. Returned commits, signatures, IDs, errors, and diagnostics own their storage and survive repository destruction; the cross-worker test destroys the repository before returning and using its record.

**Admission is exact and ambient state does not redirect it.** `open_exact` calls `git2::Repository::open_ext` with only `NO_SEARCH` and an empty ceiling list. It neither enables `FROM_ENV` nor searches parents. Normal, bare, unborn, explicit Git-directory, linked-worktree, nested rejection, missing path, and hostile `GIT_DIR` cases are exercised. Ordinary filesystem indirection remains the documented boundary.

**Reads are fail-closed.** IDs require exact 40/64-character ASCII hexadecimal representations and retain their format in equality and hashing. Repository-format mismatch is rejected before native lookup without a native diagnostic. Missing objects, blobs, annotated tags, and malformed commits produce `ObjectRead` with copied native code, class, and message; none becomes an empty/default record. Missing referenced parents and trees remain IDs without recursive validation, as required.

**Commit extraction does not invent normalized data.** Native parsing first validates that the object is a commit. A second ODB read supplies length-aware raw bytes, preserving leading newlines, non-UTF-8 data, embedded NUL, parent order, and the first raw encoding value. Native signature parsing provides owned identity bytes and numeric time/offset fields. If an object disappears or becomes unreadable between native parse and raw read, the second operation returns an error rather than a partial record. Content-addressing prevents ordinary replacement with different accepted bytes under the same ID.

**There is no G0 durability or mutation path.** Production source contains no process launcher or filesystem write. The tested open/read sequence leaves HEAD, index bytes, worktree content, environment, and current directory unchanged. The clean-child test clears `PATH`, supplies hostile `GIT_DIR`, and still performs the operation through native APIs. Network features are absent, and the public surface exposes no native handle or remote operation.

**Thread ownership matches the accepted contract.** The wrapper contains the pinned git2-rs repository, which is `Send` and not `Sync`; it adds no clone implementation. Positive `Send`, negative `Sync`/`Clone`, cross-worker read/drop, and record survival all compile and pass. Independent SHA-256 handles read concurrently behind a barrier. Those reads traverse libgit2’s strict ODB hash verification, exercising the builtin SHA-256 path whose prior shared-static race is fixed in the pinned C ancestry.

**Bootstrap state is isolated.** `gwz-git` is a local-only GWZ member with its own workspace and lockfile, excluded from the outer Cargo workspace. No core or CLI dependency was added. The path dependency is accurately described as non-immutable; exact tuple checks, nested gitlink verification, runtime version assertion, and the recorded source-byte proof collectively prevent silent registry, system-native, or alternate-fork substitution at this gate.

## 3. Risks and next action

Execution remains limited to macOS arm64. Windows and the other required macOS/Linux architectures remain explicitly pending, as do clean remote-only reconstruction and publication. Multi-read snapshot isolation is intentionally absent, so callers must tolerate external repository changes between reads. These limits are stated consistently and are not activation claims.

The next action is to accept this local G0 candidate, then keep platform qualification, source distribution, consumer integration, and production activation as separate gates. Any commit/tag, history, fetch, helper, mutation, or cancellation surface still requires its own reviewed package.
