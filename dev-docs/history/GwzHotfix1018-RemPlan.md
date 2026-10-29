# Hotfix 1.0.18 (root push proof): remediation plan (round 1)

Date: 2026-10-27. Lane owner's plan after [Code](GwzHotfix1018-ReviewCode.md) (GO; 1 out-of-scope same-cause defect, 3 P3) and [State](GwzHotfix1018-ReviewState.md) (NO-GO: 1 P2, 3 P3) on gwz-core `00e3349a` (branch `hotfix/v1.0.18`, parent `v1.0.17` `cc2018c0`, lane `hotfix-1018`). No architectural root cause; no false proof found by either axis. One patch; the same reviewers re-verdict.

## Blocking

| ID | Disposition | Where | Closure |
| --- | --- | --- | --- |
| State P2-1 | After `is_ancestor` fails, also ask `commit_exists(member, dependency.commit)`; `Ok(false)` → answer `Ok(false)` (refuse with the remedies; can only refuse, never prove). Correct the `available` doc comment (a fetch does repair a missing locked commit). | `publication.rs` `descends_from_advertised` / `available` | Native rows: member holds an unrelated advertised branch (`old`) but not the locked commit L → `RemoteRejected` containing "fetch its advertised history and retry"; variant with a held annotated tag instead of `old`; after `fetch` → `Ok` and the root remote's `refs/heads/main` equals the root commit. |

## Non-blocking (same patch)

| ID | Disposition | Where |
| --- | --- | --- |
| State P3-1 | When the refusal comes from advertised objects (or the locked commit) the member does not hold, the remedy text names fetch first (or only fetch); assert it in the regression rows. | `PUBLISH_FETCH_OR_CHECK_REMOTES` and its use |
| State P3-2 = Code P3-1 | Reduce the peel branch to "a held non-commit object proves nothing" and rely on libgit2's `^{}` advertisement rows, with a comment saying so; reword the annotated-tag test's docstring to what it proves. (Removes the redundant walk and the equal-commit corner case.) | `publication.rs`, `last_known_refs.rs` |
| State P3-3 | The new tag fixture pins `-c tag.gpgSign=false -c tag.forceSignAnnotated=false` (or creates the tag through the backend), as `fixture_native.rs` does. | `last_known_refs.rs:516-520` |
| Code P3-2 | Give the tracking double `commit_exists`/`^{commit}` answers and add a row: advertised object is a held commit, its ancestry query errors "shallow history" → the root error carries "shallow history", not the remedy. | g26 / tracking double |

## Operator decision: folded in (2026-10-27)

The operator approved adding Code OOS-1 to this patch. Code OOS-1 (same root cause, pre-existing, P2-class, reachable through `PushRequest.refspec` / gwz-py `client.push(refspec=…)`, not the CLI): `dependency_was_published` asks `is_ancestor` of a pushed annotated-tag oid. Disposition: classify/peel the pushed source like the advertised-object path (never answer true unproven); closure: the reviewer's probe as a red-first native row (member annotated-tag refspec push, then the root proves and its tag is pushed to a local bare remote).

## Process

One patch on `00e3349a` in lane `hotfix-1018`; gates: fmt, clippy as release.py runs it, `scripts/run_tests.py --lib` (the State reviewer ran it green at 2032); the lane owner commits; Code and State re-verdict their own findings. Then the forward-port to gwz-core main and the 1.0.18 release, both on the operator's go.
