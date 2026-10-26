# Round-2 re-verdict, Code axis: gwz-core `659118d2` (range `00e3349a..659118d2`)

**NO-GO.** OOS-1 and all three of my P3 findings are closed. But the remediation introduced one new P2 (R2-1): when the locked commit was never published anywhere, the refusal tells the operator to fetch, and fetching never helps.

**Tuple.** I checked it at the start and the end: HEAD is `659118d2` on `hotfix/v1.0.18` and the tree is clean. My scratch dir under `build-targets/hotfix-1018-review-code/` is deleted. g08 + g26 pass on this tree (78 rows, including one probe I added).

## Closure table

| Finding | Status | Evidence on `659118d2` |
| --- | --- | --- |
| **OOS-1** (`dependency_was_published` asked ancestry of a pushed annotated-tag object) | **Closed** | `pushed_source_descends` peels the tag. The positive row `a_pushed_annotated_tag_proves_the_root_dependency_through_its_commit` (my probe) is green. A mutant where the peel always answers true fails the negative row `a_pushed_annotated_tag_below_the_locked_commit_does_not_prove_the_root`. The proof stays sound: a remote that accepted a tag object holds the peeled commit and its history. |
| **P3-1** (peel branch not pinned, test docstring overstated it) | **Closed** | A held non-commit object now answers `Diverges`, and the redundant ancestry walk is gone. The `Descends` mutant fails `an_annotated_tag_on_the_remote_is_decided_by_its_peeled_row` and the below-lock row. The docstring is accurate. |
| **P3-2** (keeping the original error for a held commit was not pinned) | **Closed** | The tracking double now answers `commit_exists` (`lack_commits`). Both the `_ => Diverges` and `Ok(Some(_)) => Diverges` mutants fail `g26::an_ancestry_error_between_held_commits_is_reported_when_the_double_classifies`. |
| **P3-3** (native/fake disagree on whether a commit is its own ancestor) | **Closed** | The ask on the advertised path was removed. The pushed path compares `commit == dependency.commit` explicitly. |

## New findings

**R2-1 (P2, recovery and diagnosability; introduced in round 2): a locked commit that is published nowhere gets an endless "fetch and retry" refusal.**
- **Location.** `src/workspace_ops/publication.rs`, in `descends_from_advertised`. The new loop `for held in [dependency.commit.as_str(), object]` returns `Ancestry::Unheld` whenever the member lacks the locked commit. `check` then answers with `FETCH_FIRST`, defined at line 378.
- **Violated invariant.** The doc comment on `FETCH_FIRST` (lines 373–377) says "the remote already has the history" and "If the retry is still refused, it names the remedies of a missing dependency". The g26 docstring at line 558 makes the same claim. Neither holds when the locked commit is on no remote.
- **Reproduction.** Native probe:
  - Setup: `ClonedMember`. The locked commit is made with `commit_file` in `upstream.source` and never pushed. The member holds everything the remote advertises.
  - Round 0: `push_root_only` → `RemoteRejected`, message "…; fetch its advertised history and retry; the member does not hold every object needed to compare with it".
  - Then `backend.fetch(app, "origin")`, and push again. Round 1 gives exactly the same refusal.
  - The publish remedy is never named.
- **Impact.**
  - This is the case the proof exists to catch: the lock names a member commit that was never published (another clone made it). Here the clone pushing the root does not hold it.
  - The operator is told to fetch indefinitely, and the real remedy (publish the member from the clone that holds it) is never named.
  - Before this round, the outcome was the raw `object not found` error naming the commit id. That was unhelpful, but it was not misleading.
  - The refusal itself is still correct, so nothing is corrupted or lost.
- **Required correction.** Return `Unheld` only when an *advertised object* is unheld. If the member holds the advertised object but lacks the locked commit, then the locked commit is not in the advertised history. That case should answer `Diverges`, or the query's own error if you want shallow-history caution, so the refusal carries the usual publish/fetch/check-remotes remedy. State's two rows (`a_locked_commit_the_member_never_fetched_is_refused_though_it_holds_{a_branch,a_tag}`) stay `FETCH_FIRST` under this change, because the remote's `main` (M) is unheld there. Fix both doc comments.
- **Regression test.** Add the probe above as a native row. It should refuse with `PUBLISH_FETCH_OR_CHECK_REMOTES`, not `FETCH_FIRST`. Optionally, after a fetch, assert the same publish remedy. In g26, add a tracking-double variant: `lack_commits(&[APP_HEAD])` with every advertised object held should give the publish remedy.

**R2-2 (P3, coverage; does not block): the equality short-circuit in `pushed_source_descends` is not pinned.**
- A mutant that deletes `if commit == dependency.commit { return Ok(true); }` passes all 78 rows.
- The positive row still goes green because the dependency then falls through to the read after the transfer, where the `refs/tags/v9^{}` row matches exactly. The mutant only costs an extra read, so it is harmless, but the branch is untested.
- Fix: in the positive row, assert that no dependency `ReadAdvertisement` happens after the member push. The transport rows already record this.

## Verdict

**NO-GO**, because R2-1 (P2) is open. OOS-1 and P3-1, P3-2 and P3-3 are closed. R2-1 is a small fix in one place plus one native row. R2-2 does not block.
