# L2-A binding member port — STATE-AXIS REVIEW

**Review object:** Corrected L2-A candidate at core `5eb29f073a536901b23f96c3d4b1d05ac59ac01c`; fork `e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`; 2026-09-20  
**Baseline:** root `a02f91f66b4efe483e44c3fa4e85dfb6d94c19e0`; transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`; taut `733e8a78897a90f017f4726e4331aed95e8cb977`. Committed files were read with immutable `git show`/`git diff`; unrelated working changes were excluded.  
**Date:** 2026-09-20  
**Axis:** State — source-admission identity, isolation, and fail-closed handling of hidden checkout state. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — **P0 0 · P1 0 · P2 0 · P3 0**.

---

## 0. Evidence base

I read the remediation plan and complete correction range `f9794d55..5eb29f07`. The only changed files are `prove.py`, `test_prove.py`, and the L2-A evidence document; binding bytes, manifest, README/help, API, and activation scope are unchanged.

The permitted Python suite passed: **7 tests**. I inspected the recorded successful source/archive native modes and actual-release clone attack; costly native modes were not repeated. Diff checks passed.

The tuple was unchanged at start and end:

| Repository | Commit | Tree |
|---|---|---|
| root | `a02f91f...` | `7dc6a732...` |
| core | `5eb29f07...` | `d3d894c6...` |
| git2-rs | `e883be38...` | `25d5d735...` |
| transport | `28f5afb3...` | `bf275d0e...` |
| taut | `733e8a78...` | `bb0694d0...` |

## Prior-finding closure table

| ID | Disposition | Verification | Status |
|---|---|---|---|
| State P2-1 | Replace attribute-sensitive archive baseline with fixed-object reads | `copy_member` now enumerates the pinned commit using `git --no-replace-objects ls-tree -rz`, reads each blob with `cat-file`, validates modes/types, and omits only the named native gitlink. The real-repository `info/attributes` plus deleted-file regression passes by refusing admission. The same attack against the actual pinned release clone was also refused. | **Closed** |

## Changed-range analysis

The correction removes `git archive` from source admission. Checkout-local attributes can no longer redefine the expected file set. Regular files, executable modes, symlinks, content, and the sole allowed manifest transformation remain checked before bytes are copied.

No new root cause or scope expansion was found.

## 2. Invariant analysis

The original counterexample now fails closed: `ls-tree` still lists the deleted `CONTRIBUTING.md` regardless of `.git/info/attributes`, so observed and expected sets differ. Replacement objects are disabled, blob bytes come from the pinned commit, and the unused gitlink is the only explicit omission.

Positive member admission, unrelated/missing/extra/symlink refusal, lock-graph identity, archive compatibility, copy-before-build isolation, and checkout/cache/production non-mutation remain intact.

## 3. Risks and next action

Platform, packaging, all-consumer, and production activation evidence remains intentionally deferred. Accept L2-A State closure on this tuple; proceed only after the paired Code verdict is also GO.
