# GWZ Git Library Foundation — SURFACE-AXIS RE-REVIEW

**Review object:** Proposed, unimplemented API guide at core `3efc1a79a1e5044b6e2495ed392a5c43d8f90b64`; focused worker-ownership revision.  
**Date:** 2026-09-21  
**Axis:** Public ownership, threading, and lifecycle contract. Independent, adversarial, read-only; no peer report informed this verdict.

**Baseline:**

| Repository | Exact SHA |
|---|---|
| root | `bba620ed7806628cdde26254261043eb9266b0f9` |
| gwz-core | `3efc1a79a1e5044b6e2495ed392a5c43d8f90b64` |
| git2-rs | `4c1caabbce7d56426c763dd94114052302b23e4c` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

All four revisions matched at start and end. Workspace status was clean.

**Verdict: GO** — P0: 0 · P1: 0 · P2: 0 · P3: 0.

---

## 0. Evidence base

Read the complete corrected `dev-docs/GwzGitLibraryApi.md` through `git show` at the exact core commit, and its immutable diff over:

```text
7dd9f37fecba8d8193585a2dba82be9db4ae5bbf
..
3efc1a79a1e5044b6e2495ed392a5c43d8f90b64
```

Rechecked repository revisions and `gwz status`. No implementation, design, plan, or peer report was read. No builds, tests, edits, or mutations were performed; this remains a documentary review.

### Prior-finding closure

| Prior Surface result | Current disposition |
|---|---|
| GO with no findings | No inherited finding requires closure. The revised threading contract was independently reassessed. |

## 2. Changed-range and invariant analysis

The API-guide diff replaces same-worker affinity and an unspecified Send guarantee with an explicit contract: `Repository` is **Send, not Sync, and not Clone**.

The revised text consistently distinguishes three cases:

- **Sequential ownership transfer:** Opening on one worker, transferring ownership, and reading or dropping on another are expressly permitted.
- **One shared handle:** Concurrent shared access remains prohibited.
- **Independent handles:** Concurrent use is expressly permitted, without introducing repository-wide locking or snapshot guarantees.

The caller no longer has to infer whether “use a suitable worker” means permanent thread affinity. Synchronous reads still belong on an appropriate worker, while ownership may move between workers.

The change preserves the documented lifecycle: drop releases native resources; no explicit close is required; returned records remain owned; path accessors remain borrowed from the repository. The existing example remains consistent with the revised contract.

No method signature, input default, error category, read-only guarantee, or setup requirement changed in this API-guide range. The revision does not authorize new operations, background execution, subprocesses, or production dependency activation.

No new Surface defect was identified.

## 3. Risks and next action

The stronger threading contract remains unimplemented. Later qualification must establish Send support, absence of Sync/Clone, valid cross-worker use and destruction, and concurrent independent-handle behavior. This review supplies no runtime evidence for those properties.

**Next action:** Record Surface GO for the corrected guide at the exact tuple, retaining the implementation and qualification gates.
