# Native local-fetch correction and isolated Rust integration — CODE-AXIS RE-VERDICT

**Review object:** `gwz-core/dev-docs/GwzNoFallbackNativeFix.md` at `e45025d622c0d5993d4daba609a68d1cee463c45`, DRAFT design  
**Remediation input:** `dev-docs/GwzNoFallbackNativeFix-RemPlan.md` at root `179231adbab23144b20b62428ca56525edc7c9c3`  
**Baseline:** git2-rs `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`; libgit2 `0551dfd4ad989b6a3d5683c0d4cf326c6efef929`; 1.9.7 backport base `49e408b3208bc3093757a1c2db938d3590f3f412`  
**Date:** 2026-09-20  
**Axis:** Focused Code re-verdict on original P2-1 and changed-range interactions. Independent, adversarial, read-only, and peer-blind.

**Verdict: GO** — original P2-1 is closed. One nonblocking P3 documentation/oracle correction remains.

---

## 0. Evidence base

I read the merged remediation plan and the complete `09a4db4…e45025d` core diff. I retraced the original mismatched-tag counterexample through immutable libgit2 objects at both C bases:

- `transports/local.c`: unconditional `GIT_ENOTFOUND` suppression in `foreach_reference_cb`;
- `tag.c`: tag-target lookup using the tag’s declared target type;
- `object.c`: actual/declared type mismatch returning `GIT_ENOTFOUND`;
- `revwalk.c`: lookup and peel propagation.

I also checked the syntax-malformed-tag path in `tag.c` and `object.c`. No builds or tests were run.

The tuple was verified at start and end. Final trees were root `34a3fb7f2d12bffe0f93fc8c35858ca762072149`, core `da6232736caecef4205cc6475f7aa924ac4ab83e`, git2-rs `25d5d7354ca204b805f448c294ed8bd62fdecd86`, and libgit2 `3c062647db3b83542fe5ac5ae3591bcad4ede4ce`. All four worktrees were clean.

## 1. Prior-finding closure

| Finding | Status | Evidence |
|---|---|---|
| P2-1: blanket `GIT_ENOTFOUND` suppression contradicts the claimed malformed-lookup guarantee | **Closed** | Revised lines 15–28 no longer claim comprehensive malformed-object rejection. They explicitly state that mismatched declared/actual tag types share the pre-existing `GIT_ENOTFOUND` suppression, preserve that behavior for this minimal fix, and make separate type-consistency hardening a prerequisite to any production fallback-removal decision. Lines 46–49 require characterization of both the original mismatched-type counterexample and a missing tag target without calling either corrected. |

The revised boundary is internally coherent: N1 corrects the mistaken error-class comparison by admitting the actual noncommittish return codes while leaving existing `GIT_ENOTFOUND` behavior unchanged. It makes no activation or safety claim from that bounded result.

## 2. Changed-range finding

### [P3-1] The syntax-malformed-tag oracle names the parser’s internal code, not the fetch-visible code

**Location:** revised design lines 46–48; remediation plan lines 10–12.

`tag_error` returns `GIT_EINVALID`, but `git_object__from_odb_object` explicitly converts any parser failure to generic `GIT_ERROR` (`-1`) before `git_revwalk_hide` and local fetch observe it. This is identical at both C bases. Therefore an actual fetchlocal regression cannot correctly assert that the fetch returns `GIT_EINVALID`, as the parenthetical currently implies.

The behavioral requirement remains valid: the syntax-malformed tag must fail, preserve its tag error detail, leave the destination absent, and leave the hint ref unchanged. Before or during N1 implementation, replace the parenthetical with the observable boundary, for example: “a syntax-malformed tag whose parser originates `GIT_EINVALID` and whose object lookup/fetch returns `GIT_ERROR`.” The closure test should assert the fetch-visible code plus tag error class/message and unchanged state.

This is nonblocking because it does not alter the proposed runtime condition or reopen P2-1; it corrects the exact test oracle.

## 3. Changed-range analysis

The correction changes only the overstated propagation contract and its test obligations. Ownership, two-branch backport, line budgets, exact sys import, N2 source admission, lock provenance, and activation exclusions remain unchanged.

The new characterization matrix prevents the accepted limitation from being mistaken for a fix: valid noncommittish hints must newly succeed, mismatched-type and missing-target tags retain stock suppression, and syntax-malformed tag data must still fail. The explicit hardening prerequisite prevents this isolated result from being used as evidence for fallback removal.

## 4. Risks and next action

The known `GIT_ENOTFOUND` ambiguity remains intentionally unresolved and cannot support production fallback removal. Proceed with bounded N1 test-first implementation, correcting P3-1’s observable error-code wording and oracle in that package.
