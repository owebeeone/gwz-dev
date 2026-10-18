# Native local-fetch correction — STATE REMEDIATION RE-VERDICT

**Review object:** Revised `gwz-core/dev-docs/GwzNoFallbackNativeFix.md` at `e45025d622c0d5993d4daba609a68d1cee463c45`; DRAFT dated 2026-09-20  
**Baseline:** root `179231adbab23144b20b62428ca56525edc7c9c3`; core `e45025d622c0d5993d4daba609a68d1cee463c45`; unchanged git2-rs `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`; unchanged libgit2 `0551dfd4ad989b6a3d5683c0d4cf326c6efef929`  
**Date:** 2026-09-20  
**Axis:** State — focused review of the narrowed error guarantee, failure-state characterization, and activation boundary. Independent, adversarial, read-only, and peer-blind.

**Verdict: GO** — zero open State findings. The merged P2-1 counterexample is correctly represented in the revised design and does not create an unsafe acceptance path.

---

## 0. Evidence base

I read only:

- `dev-docs/GwzNoFallbackNativeFix-RemPlan.md` at root `179231ad...`.
- The committed diff of `GwzNoFallbackNativeFix.md` from core `09a4db4c...` to `e45025d...`.
- The previously inspected pinned libgit2 paths necessary to retrace the counterexample: `transports/local.c:553–573`, `tag.c:27–31`, and `object.c:110–175,367–482`.
- The controlling Lane 1 acceptance boundary in `GwzNoFallbackPlan.md:154–183`.

No builds, tests, edits, peer reports, or peer prompts were used.

The tuple matched at start and end. Final trees were root `34a3fb7f2d12bffe0f93fc8c35858ca762072149`, core `da6232736caecef4205cc6475f7aa924ac4ab83e`, git2-rs `25d5d7354ca204b805f448c294ed8bd62fdecd86`, and libgit2 `3c062647db3b83542fe5ac5ae3591bcad4ede4ce`.

## 1. Closure table

| Item | Revised evidence | State status |
|---|---|---|
| Original State review | Original verdict had no findings | Remains closed |
| Remediation P2-1: type-mismatched annotated tags are also suppressed as `GIT_ENOTFOUND` | Design lines 15–28 and 46–49; RemPlan lines 3–18 | **Closed for this design gate.** The behavior is explicitly preserved and characterized, comprehensive malformed-object rejection is disclaimed, and type-consistency hardening is a named prerequisite before fallback removal. |

## 2. Invariant analysis

The revised description now matches the pinned state machine. A parsed tag records its declared target type. Peeling calls `git_tag_target`, which requests that type from the object database. If the declared type differs from the target object’s actual type, `git_object__init_from_odb_object` returns `GIT_ENOTFOUND`; the existing local-fetch callback clears that result and continues. The design no longer claims that this pre-existing case propagates.

The changed text defines distinct outcomes:

1. Direct tree/blob hints and annotated tags ending in tree/blob are intentionally treated as noncommit negotiation hints through `GIT_EINVALIDSPEC` or `GIT_EPEEL`.
2. Missing tag targets and declared/actual target-type mismatches retain stock `GIT_ENOTFOUND` suppression and must be characterized without being called corrected.
3. A syntax-malformed tag remains a failing case, with requested destination state and hint refs checked rather than silently accepted.

This is a closed and auditable grammar for N1. It prevents a passing stock-behavior test from being reported as malformed-input hardening. The explicit prerequisite at lines 26–28 also prevents N1/N2 acceptance from being used as evidence for production fallback removal. That preserves the controlling plan’s stricter missing/corrupt-object requirement for the later decision.

The changed range does not alter ownership, write ordering, N1-before-N2 sequencing, source admission, lock provenance, or publication behavior. No new state transition or recovery state is introduced.

## 3. Risks and next action

Type-mismatched tag suppression remains an acknowledged native limitation. N1/N2 provide no comprehensive malformed-input safety, cancellation, ref-atomicity, or production-activation acceptance.

Proceed with the bounded N1 implementation. Its acceptance review must verify the three outcomes above, including unchanged destination state on the syntax-malformed failure and truthful recording of both preserved `GIT_ENOTFOUND` cases. Type-consistency hardening must remain open until separately designed and reviewed before any fallback-removal decision.
