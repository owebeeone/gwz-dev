# Native local-fetch correction and isolated Rust integration — CODE-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzNoFallbackNativeFix.md` at `09a4db4c6db74a422673417f195dc1f86dceb15d`, DRAFT design, 2026-09-20  
**Baseline:** root `167c0db46e8154e3a24e2e0a51c078b0e04335ff`; core `09a4db4c6db74a422673417f195dc1f86dceb15d`; git2-rs `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`; libgit2 `0551dfd4ad989b6a3d5683c0d4cf326c6efef929`. Sources were read from immutable Git objects with `git show`, `git diff`, `git ls-tree`, and `git rev-parse`.  
**Date:** 2026-09-20  
**Axis:** Architecture, interfaces, call graphs, compatibility, and error-path reality. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

I read the complete design, `CurrentProgramCheckpoint.md` lines 1–26, the accepted local-fetch characterization, and the current proof runner, manifest, pin file, tests, and README.

At both libgit2 bases I inspected `src/libgit2/transports/local.c`, especially `foreach_reference_cb` and `local_download_pack`; `src/libgit2/revwalk.c` lines 45–126; `src/libgit2/object.c` object lookup and peeling; `src/libgit2/tag.c` tag-target lookup; and the existing fetchlocal tests. The faulty callback is identical at the relevant boundary on main and `49e408b3208bc3093757a1c2db938d3590f3f412`.

I verified the proposed sys baseline: `dffaf272…` to `6c93812…` changes exactly five `libgit2-sys` paths by 115 insertions/21 deletions; the latter declares `0.18.8+1.9.7` and pins native commit `49e408b…`. No builds or tests were run, as required.

The end tuple matched the start tuple and tree IDs. Core, git2-rs, and libgit2 remained clean. Root contained only the two excluded untracked owner prompt artifacts.

## 1. Findings

### [P2-1] Preserving blanket `GIT_ENOTFOUND` suppression still masks a malformed tag lookup

**Location:** design lines 11–23 and 38–44; libgit2 `src/libgit2/transports/local.c` lines 553–574 at `0551dfd…` and lines 560–581 at `49e408b…`; `src/libgit2/tag.c` lines 27–30; `src/libgit2/object.c` lines 122–126.

**Violated invariant:** The design requires missing source-side receiver hints to remain ignored while other lookup/parse failures propagate, and requires a genuine malformed-object regression to prevent error masking.

A concrete counterexample defeats that distinction:

1. Write a raw annotated-tag object `T` into source and receiver. Its body names blob `B` but declares `type tree`; both `T` and `B` exist in the source.
2. Point a receiver ref at `T`.
3. Request a new source commit absent from the receiver, forcing negotiation.
4. `git_revwalk_hide(T)` parses `T`, then `git_tag_target` looks up `B` as a tree.
5. `git_object_lookup` detects that `B` is actually a blob, sets “requested type does not match,” and returns `GIT_ENOTFOUND`.
6. `foreach_reference_cb` treats every `GIT_ENOTFOUND` as an absent source hint, clears the error, and continues successfully.

This target is present; the failure is malformed tag metadata, not the explicitly deferred missing-tag-target case. Replacing only the mistaken `GIT_ERROR_INVALID` comparison while preserving unconditional `GIT_ENOTFOUND` suppression leaves this error masked. A syntax-corrupt-object test could pass while missing the counterexample.

**Required correction:** Define the N1 decision rule so absence of the receiver hint’s object in the source is distinguished from failure while validating or peeling an object that is present. Valid tree/blob and valid tags ending in them may be skipped; malformed type relationships must propagate. Rescope the production budget if explicit tag-chain validation requires it.

**Closure test:** Add a fetchlocal case using the raw mismatched-type tag above, with the receiver hint object and its target present in the source and a wanted commit absent from the receiver. Assert the fetch returns the native lookup/type error, does not publish the requested destination, and leaves the hint ref unchanged. Retain separate passing cases for a truly absent receiver hint and the explicitly deferred missing tag target.

## 2. Invariant analysis

The central return-code diagnosis otherwise holds. Direct trees/blobs reach `GIT_EINVALIDSPEC`; tags ending in trees/blobs reach `GIT_EPEEL`; commits and tags ending in commits remain valid revwalk hides. Wanted noncommit objects are inserted into the pack before receiver-hint traversal, so the proposed hint correction need not alter wanted-object policy.

The main and 1.9.7 backport bases expose the same faulty callback, making the two-file branch plan coherent. The exact sys import, path dependency, native gitlink, vendored-source requirement, isolated copy, dual git2/sys lock provenance, and rejection of unrelated dependency drift form a feasible N2 admission boundary. Existing mutually exclusive archive/source proof inputs provide the mode boundary; no new product API is implied.

## 3. Risks and next action

The deferred activation, publication, platform matrix, `FETCH_HEAD`, and ref-atomicity work remain outside this verdict. The single next action is to correct the N1 error-classification design and add the mismatched-tag closure oracle before starting implementation.
