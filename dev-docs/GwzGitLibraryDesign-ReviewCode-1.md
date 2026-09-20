# GWZ Git Library Design and G0 API — CODE-AXIS REMEDIATION REVIEW 1

**Review object:** Corrected `gwz-core` commit `3efc1a79a1e5044b6e2495ed392a5c43d8f90b64`, `dev-docs/GwzGitLibraryDesign.md` and `dev-docs/GwzGitLibraryApi.md`; status **DRAFT, unimplemented, unpublished**.  
**Baseline:** root `bba620ed7806628cdde26254261043eb9266b0f9`; `gwz-core` `3efc1a79a1e5044b6e2495ed392a5c43d8f90b64`; `git2-rs` `4c1caabbce7d56426c763dd94114052302b23e4c`; `libgit2` `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`.  
**Date:** 2026-09-21  
**Axis:** Focused Code re-verdict of original P2-1 and changed-range interactions. Independent, adversarial, read-only. No current-round peer report was read. Filed verbatim by the lane owner.

**Verdict: GO** — original P2-1 is closed; no P0–P3 finding remains in the corrected range.

---

## 0. Evidence base

I read the committed merged remediation plan at root `bba620e`, the complete diff from core `7dd9f37fecba8d8193585a2dba82be9db4ae5bbf` to `3efc1a79a1e5044b6e2495ed392a5c43d8f90b64`, and the corrected passages in both documents.

The core change is limited to:

- `GwzGitLibraryDesign.md:84-90`: explicit `Send + !Sync + !Clone` ownership contract.
- `GwzGitLibraryDesign.md:209-221`: positive and negative compile checks, sequential cross-worker read/drop, and concurrent independent-handle SHA-256 regression.
- `GwzGitLibraryApi.md:39-45`: matching caller-facing transfer, sharing, drop, and independent-handle semantics.

I retraced the source boundary against pinned `git2-rs/src/repo.rs:101-116`, where the native wrapper explicitly implements `Send`, and pinned libgit2’s corrected builtin SHA-256 implementation and 1.9.4 changelog entry.

No build or test was run or claimed; this remains a documentation/interface gate.

The tuple was unchanged at both boundaries. End trees were root `d744034a6c7c350c836ce6745c59cb7bede08edf`, core `7e6de34673c4d55f46b81b74d87d955512f2b2fb`, git2-rs `48c43f1fce89430d51e7f3cea159c509370adb1d`, and libgit2 `af52de522aacf3593250c45159a038801153c6b5`. All worktrees were clean and `git diff --check` passed.

## 1. Prior-finding closure

| Original finding | Status | Closure evidence |
| --- | --- | --- |
| P2-1 — The frozen `Repository` API did not decide its observable `Send` contract | **Closed** | Both authoritative documents now freeze `Repository: Send + !Sync + !Clone`. Ownership may cross workers only by sequential move; one handle cannot be accessed concurrently. G0 requires a positive `Send` assertion, negative `Sync` and `Clone` compile checks, and an open/read/drop sequence spanning workers. |

The original counterexample no longer has two conforming implementations. A transparent wrapper over pinned `git2::Repository` remains `Send` and conforms. A wrapper that suppresses `Send` with a private marker now violates the explicit API and positive compile assertion. Conversely, implementing `Sync` or `Clone` violates both the written contract and required negative checks. The public auto-trait surface is therefore determinate.

## 2. Changed-range analysis

Sequential transfer is compatible with the pinned binding’s explicit `unsafe impl Send for Repository`. Keeping the wrapper `!Sync` prevents ordinary shared-reference access across workers; Rust also prevents moving the handle while its path or future cursor borrows remain live. Owned commit records remain transferable independently of the repository lifetime.

The API guide and design agree that construction, reads, and destruction may occur on different workers. They also agree that separate handles may operate concurrently while one handle has no shared concurrent access. This does not add a snapshot, cross-process lock, or transaction guarantee.

The added independent-handle SHA-256 regression is correctly bounded. Pinned libgit2 1.9.7 contains the thread-safety correction recorded in its 1.9.4 changelog: the former shared length accumulator is function-local. The test records that selected-source fact without expanding the native patch or activating a network feature.

No future operation was accidentally authorized. Borrowed cursor signatures, mutation APIs, history behavior, remote adapters, production dependency activation, and platform qualification retain their existing separate gates. The change adds no public method, owner, process execution, network path, or wire behavior.

## 3. Risks and next action

Implementation evidence is still pending by design. G0 acceptance must actually demonstrate the specified trait checks, sequential cross-worker lifecycle, independent-handle SHA-256 concurrency, exact source proof, read-only behavior, and platform status.

The Code axis permits the corrected design/API to proceed to the bounded G0 implementation package.
