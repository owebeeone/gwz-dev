**Final re-verdict (round 2 of 2): GO.** P2-A is fixed, and no change in this round can prove a dependency the remote doesn't have. Every open finding is closed and there is nothing new at P0–P2.

**Tuple:** checked at start and end. gwz-core is at `17b965f7` on `hotfix/v1.0.18`, clean tree. I worked in a scratch copy under the review build-targets directory and deleted it afterwards. Only local bare remotes were used.

**Tests:**
- 16/16 `last_known_refs` rows pass: the 13 in the commit plus my 3 counterexample rows.
- All 391 `workspace_ops::tests::` pass on the real Git backend.
- I did not re-run the full suite; the implementer reports 2041 passed, 0 failed.

## Closure table

| Finding | Status | Evidence on `17b965f7` |
|---|---|---|
| P2-1 locked commit missing in the member gives `GitCommandFailed` | Closed | My E4 row: upstream has a held `old` branch, plus L and M the member never fetched; the lock names L. The first push is refused with fetch-first followed by the full remedy. After a fetch it returns `Ok`, and the root remote's `refs/heads/main` equals the root commit. |
| P3-1 misleading remedies | Closed | Fetch is named alone only as a first step. Every refusal now ends with the full remedy. |
| P3-2 peel branch never proved anything | Closed | Unchanged since round 2. |
| P3-3 tag fixtures depend on global git config | Closed | Unchanged since round 2. |
| P2-A fetch-only remedy hid the working remedies | Closed | My C1 row (an unfetched `refs/pull/1/head` plus an unpublished `work`) gets the full remedy, including "publish the member" and `--check-remotes`, both before and after a fetch. The implementer's rows for the PR ref and for a held annotated tag pointing to a tree pass as well. |

## False-proof re-attack
- Only the `Proven` answer lets the root through. It still comes only from:
  - the advertisement naming the locked commit itself;
  - Git confirming the locked commit is an ancestor of an advertised commit;
  - a push in this operation proving it, including the peeled commit of a held annotated tag the remote accepted.
- In `descends_from_advertised`, every combination of held and unheld objects maps to `Diverges` or `Unheld`. Both only refuse. If `commit_exists` errors, the original Git error is kept.
- Fabricated locked commit (my F1 row: an id that exists nowhere, with unfetched commits on the remote): refused before and after a fetch, never proven. It falls back to the full remedy as intended.
- The edge case the implementer reported (locked commit missing plus a held tag pointing to a tree counts as both missing) only refuses, and the refusal ends with the full remedy. Not a defect.

## New findings
None at P0–P2. One cosmetic note, outside the severity scale: the fetch-first text plus `PUBLISH_FETCH_OR_CHECK_REMOTES` repeats "fetch its advertised history and retry" in the same message. Harmless, not a release blocker.

**GO.**
