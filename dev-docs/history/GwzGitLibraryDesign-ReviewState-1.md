# GWZ-GIT LIBRARY DESIGN AND G0 API — STATE-AXIS RE-REVIEW

**Review object:** Corrected DRAFT design/API at `gwz-core` `3efc1a79a1e5044b6e2495ed392a5c43d8f90b64`: `dev-docs/GwzGitLibraryDesign.md` and `dev-docs/GwzGitLibraryApi.md`. Proposed architecture and G0 API only; no implementation or production activation.  
**Baseline:** root `bba620ed7806628cdde26254261043eb9266b0f9`; `gwz-core` `3efc1a79a1e5044b6e2495ed392a5c43d8f90b64`; `git2-rs` `4c1caabbce7d56426c763dd94114052302b23e4c`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Changed range: root remediation plan plus `gwz-core` `7dd9f37fecba8d8193585a2dba82be9db4ae5bbf..3efc1a79a1e5044b6e2495ed392a5c43d8f90b64`. Sources were read from immutable Git objects.  
**Date:** 2026-09-21  
**Axis:** Durable-state semantics and adversity at the changed ownership/thread boundary. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — no prior State findings and no new P0, P1, P2, or P3 findings. The corrected revision retains the prior State GO.

---

## 0. Evidence base

I read:

- Root `dev-docs/GwzGitLibraryDesign-RemPlan.md` lines 1–19.
- The complete two-file diff from `gwz-core` `7dd9f37...` to `3efc1a79...`; these were the only changed core paths.
- Corrected design lines 77–113 and 198–228.
- Corrected API guide lines 32–65.
- Pinned `git2-rs` ownership evidence at `src/repo.rs:116` (`unsafe impl Send for Repository`) and its handle `Drop` implementation at `src/repo.rs:3477`; no `Sync` implementation exists.
- The pinned libgit2 builtin SHA-256 implementation and history. Commit `e2e2cfb006fb82083de2a8214cb25c154453ac65`, which removes the shared static SHA-256 temporary and makes it invocation-local, is an ancestor of the pinned C commit. The Rust manifest’s warning is therefore stale for this source.
- `git diff --check`, which passed.

No build or test was run because this is a documentation re-verdict. Start and end checks both resolved exactly to:

| Repository | Commit | Tree |
|---|---|---|
| root | `bba620ed7806628cdde26254261043eb9266b0f9` | `d744034a6c7c350c836ce6745c59cb7bede08edf` |
| gwz-core | `3efc1a79a1e5044b6e2495ed392a5c43d8f90b64` | `7e6de34673c4d55f46b81b74d87d955512f2b2fb` |
| git2-rs | `4c1caabbce7d56426c763dd94114052302b23e4c` | `48c43f1fce89430d51e7f3cea159c509370adb1d` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` | `af52de522aacf3593250c45159a038801153c6b5` |

All repositories remained clean.

## 1. Findings and prior-finding closure

| Prior State item | Corrected evidence | Status |
|---|---|---|
| No findings; original verdict GO | The changed ownership boundary and its interactions were re-attacked below. No prior State invariant regressed. | **GO retained** |

No new findings.

## 2. Changed-range invariant analysis

**Sequential transfer has a closed ownership sequence.** The corrected contract permits `open_exact` on worker A, ownership transfer to worker B, then read and `Drop` on B. At every point one Rust value owns one native handle. `Send` admits the move; `!Clone` prevents safe handle duplication; `!Sync` prevents simultaneous direct shared-reference access. A synchronization wrapper can serialize access across workers, which remains sequential use rather than concurrent entry into one handle.

**Drop remains complete after transfer.** The API explicitly allows opening, reading, or dropping on different workers and requires synchronous calls on a suitable worker. The pinned binding already declares its repository handle `Send`, and release has no separate durable success state. Moving before drop therefore adds no abandoned-resource or half-closed state.

**Independent concurrency is distinguished from handle sharing.** The API guide permits independent handles concurrently while continuing to prohibit concurrent access to one handle. Each handle retains its prior non-snapshot semantics; an external repository change may make two reads observe different valid states, but the contract already disclaims locking and snapshot isolation. G0 still performs no mutation, retry, fallback, helper execution, or recovery write.

**SHA-256 concurrency is gated with the correct composition.** The pinned C source contains the fix for the historical shared-static race. The new native-baseline requirement exercises concurrent SHA-256 reads through independent handles without widening G0 or adding a C patch. Exact source verification before and after tests remains mandatory, so the regression cannot silently qualify another native implementation.

**The API and design now agree.** Both freeze `Repository` as `Send + !Sync + !Clone`, permit sequential cross-worker ownership transfer, and allow independent handles concurrently. Required validation includes a positive `Send` assertion, negative `Sync` and `Clone` compile checks, cross-worker read/drop, and independent-handle SHA-256 concurrency. Borrowed paths still end with the repository borrow, owned records still outlive it, and future cursor and mutation APIs remain separately gated.

## 3. Risks and next action

The trait and concurrency properties remain documentary until G0 exists. Future additions to `Repository` must preserve the frozen transfer contract or return for interface review; this correction does not authorize future mutation, cursor, or helper behavior.

The next action is the bounded G0 implementation, including the newly specified compile-time ownership checks, sequential transfer test, independent-handle SHA-256 regression, exact native-source proof, and the unchanged implementation review and platform gates.
