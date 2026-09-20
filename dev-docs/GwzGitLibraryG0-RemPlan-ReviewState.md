# GWZ-GIT G0 REMEDIATION SCOPE AMENDMENT — STATE-AXIS REVIEW

**Review object:** `dev-docs/GwzGitLibraryG0-RemPlan.md` at root `409d86d8e1c61831dfd52d6dd57d71b386c920b4`; bounded pre-implementation remediation amendment  
**Baseline:** root `409d86d8e1c61831dfd52d6dd57d71b386c920b4`; `gwz-core` `039eb1f315339a61d3d4f1cf5db5b79dce74c4ce`; `gwz-git` `fcf5296095d230f46c8409a8d6d47b44f4ad918b`; `git2-rs` `4c1caabbce7d56426c763dd94114052302b23e4c`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Durable-state semantics and adversity at the proposed raw-object, native-diagnostic, and source-admission boundaries. Independent, adversarial, read-only. No current-round peer report was read. Filed verbatim by the lane owner.

**Verdict: GO** — zero P0, P1, P2, or P3 findings in the scope amendment. This authorizes the bounded remediation implementation; it does not close the original implementation findings or accept an unimplemented correction.

---

## 0. Evidence base

I read:

- `GwzGitLibraryG0-RemPlan.md` lines 1–51.
- The legitimate round-one Code findings and the original State evidence.
- Existing `gwz-git/src/commit.rs` and `src/error.rs`.
- Pinned git2-rs `src/error.rs`, including `Error::from_raw`, `class`, `raw_class`, and `raw_set_git_error`.
- Pinned libgit2 commit graft application and `include/git2/errors.h`, where `GIT_ERROR_GRAFTS` is 36.
- Existing native qualification inputs: `binding-pin.json`, `prove.py`, and its immutable-tree, file-hash, archive-patch, source-member, nested-C, lock-graph, and test admission paths.

No build or test was run because the amendment contains no runtime change. Start and end verification matched:

| Repository | Commit | Tree |
|---|---|---|
| root | `409d86d8e1c61831dfd52d6dd57d71b386c920b4` | `b710c0081e1a89d96fbdaba48e5470ccd2c7e784` |
| gwz-core | `039eb1f315339a61d3d4f1cf5db5b79dce74c4ce` | `b49ffec90db725fd11d44957623850935e8e386d` |
| gwz-git | `fcf5296095d230f46c8409a8d6d47b44f4ad918b` | `4c63d41fd7de481e48050ec9c19d499f46f79954` |
| git2-rs | `4c1caabbce7d56426c763dd94114052302b23e4c` | `48c43f1fce89430d51e7f3cea159c509370adb1d` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` | `af52de522aacf3593250c45159a038801153c6b5` |

All member worktrees remained clean.

## 2. Invariant analysis

| Original finding | Amendment coverage | State assessment |
|---|---|---|
| Code P2-1: graft/shallow metadata rewrites native parent results | Validate through native commit lookup, then extract stored tree and ordered parent IDs from the raw ODB header. Test SHA-1/SHA-256 shallow and explicit-graft cases and assert metadata unchanged. | **Adequately scoped.** Validation remains native, while the returned content becomes independent of traversal metadata. No graft/shallow mutation or traversal bypass is authorized. |
| Code P2-2: `raw_class` loses newer native classes | Return the stored class integer through the existing raw alias, use it for callback replay, retain safe `class()` compatibility mapping, and test known/new/arbitrary values plus malformed grafts against native constant 36. | **Adequately scoped.** This preserves diagnostics and callback round trips without parsing messages, shadowing FFI layout, adding sys enums, or changing the public signature. |
| Code P3-1: unreviewed public error traits | Remove `Clone`, `Eq`, and `PartialEq` from `Error` and `NativeDiagnostic`; add negative compile checks. | **Adequately scoped.** No state behavior or accepted diagnostic field is removed. |

The parent/tree correction restores the accepted distinction between immutable object content and repository-local traversal state. A sequence that stores `C -> P`, marks `C` shallow or grafts it to `Q`, and reads `C` through G0 must return stored `P` while native traversal may still observe the shallow/grafted relationship. Reading the already validated raw object provides that separation and introduces no write, rollback, or recovery state.

The error correction targets the actual loss point. Pinned git2-rs stores the native `klass` integer but `raw_class` currently whitelists older constants and substitutes zero. Returning the stored value and replaying that value through `raw_set_git_error` preserves both observation and callback propagation. Keeping safe `class()` unchanged avoids an unrelated public enum expansion.

The source amendment is closed rather than open-ended. Only `git2-rs/src/error.rs` may change; the new fork must descend from the accepted pin; the C gitlink and both per-remote files remain byte-identical. Adding baseline/patched `error.rs` hashes to the existing proof makes both archive reconstruction and source-member admission cover the third patched file. Lock, version, feature, C-source, and duplicate-provider checks remain required.

Budgets, ownership, and execution order are explicit: regressions must fail first, source pins and reproducible patch are regenerated from exact bytes, proof runs before and after library checks, and closure belongs to the original reviewers. No runtime behavior, publication, production dependency, or later operation API is claimed by this amendment.

## 3. Risks and next action

Correct raw-header parsing, error classification on any post-validation inconsistency, and cross-platform integer conversion remain implementation evidence, not assumptions granted by this GO. The revised SHA-1/SHA-256 graft/shallow tests, arbitrary-class round trips, malformed-grafts oracle, and source/archive proof must demonstrate them.

The next action is the bounded implementation exactly within the listed files and ceilings, followed by focused original-reviewer closure against the new exact tuple.
