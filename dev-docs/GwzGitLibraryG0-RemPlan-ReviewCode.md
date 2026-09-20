# G0 Remediation Scope Amendment — CODE-AXIS REVIEW

**Review object:** `dev-docs/GwzGitLibraryG0-RemPlan.md` at root `409d86d8e1c61831dfd52d6dd57d71b386c920b4`; status **scope amendment awaiting retained Code/State review**, dated 2026-09-21. No runtime delta is part of this object.  
**Baseline:** root `409d86d8e1c61831dfd52d6dd57d71b386c920b4`; core `039eb1f315339a61d3d4f1cf5db5b79dce74c4ce`; library `fcf5296095d230f46c8409a8d6d47b44f4ad918b`; git2-rs `4c1caabbce7d56426c763dd94114052302b23e4c`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Sources were read from immutable Git objects after exact-commit verification.  
**Date:** 2026-09-21  
**Axis:** Architecture, interfaces, native call paths, ownership, and compatibility reality of the bounded remediation scope. Independent, adversarial, read-only. The other axis runs independently; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no P0–P3 findings in this scope amendment. This verdict authorizes the bounded implementation; it does not close the original runtime findings, which require re-verdict on the corrected tuple.

---

## 0. Evidence base

I read the complete amendment, the original Code report, the affected G0 implementation, and the relevant pinned binding/native source:

- amendment lines 11–50;
- `gwz-git/src/commit.rs:25-75` and `src/error.rs:12-71`;
- git2-rs `src/error.rs:66-73,168-255,305-365`;
- libgit2-sys `lib.rs:49-80,197-226`;
- libgit2 `include/git2/errors.h:79-128`;
- core native-proof runner `prove.py:160-245`, its pin manifest, patch composition, and qualification instructions.

The tuple was exact and unchanged at both boundaries. End trees were root `b710c0081e1a89d96fbdaba48e5470ccd2c7e784`, core `b49ffec90db725fd11d44957623850935e8e386d`, library `4c63d41fd7de481e48050ec9c19d499f46f79954`, git2-rs `48c43f1fce89430d51e7f3cea159c509370adb1d`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. Worktrees remained clean and `git diff --check` passed. Per scope, I ran no tests or builds.

## 2. Invariant analysis

| Original finding | Scope-amendment assessment |
| --- | --- |
| P2-1: graft/shallow rewrites stored parents | **Bounded and sufficient.** Lines 13 and 39–47 retain native commit validation and signature parsing while sourcing tree and ordered parents from the validated raw ODB header. This directly separates stored-object content from graft-aware traversal without modifying graft/shallow state or adding public API. SHA-1/SHA-256 shallow and explicit-graft fixtures are required to demonstrate both stored-record fidelity and unchanged native traversal. |
| P2-2: `raw_class()` loses newer native classes | **Bounded and ABI-compatible.** Lines 14, 24–38, and 43–47 limit the fork delta to `git2-rs/src/error.rs`, preserve the existing public signature, leave the safe `class()` mapping unchanged, and route callback restoration through the lossless raw value. The existing `raw::git_error_t` is an integer alias, so returning the stored class requires no enum, layout, C, or version change. Known, newer, and arbitrary-value rows plus a native `GIT_ERROR_GRAFTS = 36` regression avoid using the defective getter as their oracle. |
| P3-1: unaccepted public error traits | **Directly resolved in scope.** Line 15 removes `Clone`, `Eq`, and `PartialEq` from `Error` and `NativeDiagnostic` while retaining the accepted diagnostics surface. Negative compile checks make the intended absence observable. |

The ownership crossing is explicit: only `git2-rs/src/error.rs` may change in the fork; existing per-remote binding files and the C gitlink remain byte-identical. The core pin manifest already drives both archive-mode patch admission and source-mode replacement from its `original_files` and `patched_files` maps, so adding `src/error.rs` hashes extends the existing trust path. It does not create a second source-selection mechanism.

The amendment preserves G0 signatures, dependency versions, features, locks, native C, production activation state, and operation API. Its proof order requires regressions to fail on the old tuple, regenerates patch and hashes from exact source, qualifies archive and source modes, and records the superseding tuple before re-review.

## 3. Risks and next action

Implementation must still demonstrate that raw-header extraction preserves repeated parent order for both object formats and that the binding change round-trips the full stored class integer through callback restoration. Those are required closure checks, not defects in this scope document.

Proceed with this single bounded remediation. The corrected runtime tuple then requires the retained Code re-verdict to close P2-1, P2-2, and P3-1.
