# GWZ Git Library Foundation — SURFACE-AXIS REVIEW

**Review object:** Proposed, unimplemented, unpublished G0 API in `gwz-core/dev-docs/GwzGitLibraryApi.md` at `7dd9f37fecba8d8193585a2dba82be9db4ae5bbf`.  
**Date:** 2026-09-20  
**Axis:** Caller-facing API shape, inputs, lifetimes, errors, read-only guarantees, and setup. Independent, adversarial, read-only; no peer report informed this verdict.

**Baseline:**

| Repository | Exact SHA |
|---|---|
| root | `37d5038f820f9ecd3934f37545029f80d3d45b58` |
| gwz-core | `7dd9f37fecba8d8193585a2dba82be9db4ae5bbf` |
| git2-rs | `4c1caabbce7d56426c763dd94114052302b23e4c` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |

The API guide was read from its immutable core commit. All four HEADs matched at start and end. Workspace status showed only the three authorized untracked review prompts.

**Verdict: GO** — P0: 0 · P1: 0 · P2: 0 · P3: 0. No Surface findings.

---

## 0. Evidence base

Read the entire API guide through:

```sh
git -C gwz-core show \
  7dd9f37fecba8d8193585a2dba82be9db4ae5bbf:dev-docs/GwzGitLibraryApi.md
```

Reviewed its introduction, “Open, inspect, close,” “IDs and commit data,” and “Errors,” plus the supplied Surface prompt. Ran the specified revision checks and `gwz status` before and after review.

No implementation, controlling design, plan, or peer report was read. No builds, tests, edits, or mutations were performed. The walkthrough was documentary; the guide expressly marks its example as not runnable yet.

## 2. Invariant analysis

**First-day setup:** The guide identifies a future sibling `gwz-git` member, separate Cargo workspace/lockfile, Rust 1.95, and the qualified sibling git2-rs checkout. It explicitly excludes crates.io installation and remote-only bootstrap. Production use awaits source distribution and platform qualification. No nonexistent setup command is presented as usable today.

**Open/use/close lifecycle:** A caller supplies an explicit repository location, opens it, parses a format-specific ID, reads a commit, and drops the repository. `open_exact` does not create a repository or search parents from a nested directory. Supported worktree, Git-directory, bare, and linked-worktree cases are stated. Ordinary links are followed without claiming a security boundary.

**Worker and record lifetimes:** Repository is opaque, non-Clone and non-Sync, with same-worker creation, use, and drop required. Path accessors borrow from it. Commit records own their memory and remain usable after repository drop. The example demonstrates that ownership boundary. No explicit close or cleanup service is missing.

**SHA-1/SHA-256 inputs:** Format selection is mandatory. IDs require full, exact-length ASCII hexadecimal text; prefixes, whitespace, abbreviations, and revision syntax are excluded. Equality includes format, display is specified, and parsing does not imply existence. A correctly represented ID of the wrong repository format fails before native lookup.

**Read semantics:** The contract distinguishes a commit read from tag peeling or reference resolution. Missing, malformed, and non-commit objects fail rather than yielding default records. Parent order and raw message behavior are defined. Referenced parents/trees need not exist, preventing an implied recursive completeness guarantee.

**Errors:** Machine-readable kinds are separated from diagnostic display text. Invalid representation and format mismatch have no native diagnostic; opening and reading failures retain native code, class, and message. Non-exhaustive categories require forward-compatible handling.

**Read-only and consistency boundaries:** No refs, index, worktree, cwd, or environment changes are authorized. Remote access, lazy fetching, credential prompts, subprocesses, retries, and repair mutations are excluded. Sequential reads are explicitly not a snapshot.

**Limits and future scope:** There is no implicit object format or exposed optional setting lacking a default. The guide does not promise unlimited object size or recoverable allocation failure. Commit/tag/history/fetch operations and future mutation/cancellation semantics remain outside G0; the error list does not authorize those behaviors.

## 3. Risks and next action

These are proposed guarantees, not demonstrated implementation properties. Native error preservation, exact opening, byte fidelity, SHA-256 handling, resource release, and absence of side effects require later implementation qualification. The guide provides no configurable memory budget or cancellation guarantee; callers should not infer either.

**Next action:** Record Surface GO for this proposed API guide while retaining its implementation, distribution, and platform gates.
