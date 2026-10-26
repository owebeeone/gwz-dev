**Re-verdict, round 2: NO-GO.** All four of my round-1 findings are fixed, and I found no false proof. But the P3-1 fix introduced a new P2: when the member's own commit was simply never pushed, the refusal now tells the user only to fetch, which never helps.

**Tuple:** checked at start and end. gwz-core is at `659118d2` on `hotfix/v1.0.18`, clean tree. I ran everything in a scratch copy under the review build-targets directory and deleted it afterwards. Only local bare remotes were used.

**Tests:**
- All 12 `last_known_refs` rows pass: the 10 in the commit plus my 2 counterexample rows.
- All 387 `workspace_ops::tests::` pass on the real Git backend.
- I did not re-run the full suite; the implementer reports 2038 passed, 0 failed.

## Closure table

| Finding | Status | Evidence on `659118d2` |
|---|---|---|
| P2-1 locked commit missing in the member gives `GitCommandFailed` | Closed | When ancestry fails, `descends_from_advertised` asks `commit_exists` for the locked commit, then the advertised object; `Ok(false)` on either is `Unheld`, which only ever refuses. Rows `a_locked_commit_the_member_never_fetched_is_refused_though_it_holds_a_branch` and `…_a_tag` pass: refused, then `Ok` after a fetch. If `commit_exists` errors, the original error is kept. The `available` doc is corrected. |
| P3-1 misleading remedies for an unfetched remote commit | Closed for that case | `FETCH_FIRST` is named alone. But the same change causes the new P2-A below. |
| P3-2 peel branch never proved anything | Closed | A held object that isn't a commit now only answers `Diverges`; the peeled `^{}` row decides. Row `an_annotated_tag_on_the_remote_is_decided_by_its_peeled_row` passes. |
| P3-3 tag fixtures depend on global git config | Closed | The tag fixtures pin `tag.gpgSign=false` and `tag.forceSignAnnotated=false`. |

## Operator addition: `pushed_source_descends` (publication.rs ~643)

I attacked it for false proof and found none.
- It reaches the peel only after `commit_exists(source) == Ok(true)`. That requires a valid object id the member holds, so a ref name or a moved local tag cannot get there.
- The peeled commit belongs to a tag object the remote accepted in this push, so the remote holds that commit and its history.
- Counting a peeled commit equal to the locked commit as proof is therefore sound.
- A forced or deleting push still voids the whole operation's push evidence (via `reads.voided`).
- Row `a_pushed_annotated_tag_below_the_locked_commit_does_not_prove_the_root` covers the negative case and passes.

## New finding

**P2-A: any advertised object the member never holds replaces the remedy with "fetch" alone, permanently, and hides the remedies that actually work**
- **Root cause:** `available` sets `unheld = true` for *any* advertised object the member lacks. `RootProof::check` then swaps the caller's remedy for `FETCH_FIRST` (publication.rs ~436-440). A remote can advertise objects that no fetch brings in:
  - refs outside the fetch refspec: GitHub's `refs/pull/*`, Gerrit's `refs/changes/*`, notes;
  - tags pointing to a tree or blob.
  So the refusal for a member commit that was never published names only fetch, every time.
- **Violated invariant:** the refusal must name what the user actually has to do. It also contradicts the code's own doc on `FETCH_FIRST`: "If the retry is still refused, it names the remedies of a missing dependency."
- **Reproduction (native, run on this tree):**
  1. Upstream gets `refs/pull/1/head` pointing at a commit not on any branch.
  2. The member commits `work` and never pushes it.
  3. The root lock names `work`, and the root is pushed alone.
  4. Result: `RemoteRejected … ; fetch its advertised history and retry; the member does not hold every object needed to compare with it`.
  5. After `fetch`, the result is identical. A held annotated tag pointing to a tree gives the same output before and after the fetch.
- **Impact:**
  - The ordinary "member not pushed" refusal loses the "publish the member" and `--check-remotes` remedies on any GitHub-hosted member that has a PR, and the user is sent round a fetch-and-retry loop that cannot succeed.
  - At `00e3349a` this case got the full set of remedies, so this is a regression from round 1, though nothing is ever falsely proven.
- **Required correction:**
  - Use `FETCH_FIRST` alone only when the locked commit itself is not held. A fetch is then strictly necessary, and the 2-row case from P2-1 shows it is enough.
  - When only advertised objects are missing, keep the caller's remedy, which already lists fetch, or say "fetch … and retry; if still refused, <remedy>".
- **Regression tests:** the two counterexamples above as native rows: an unfetched `refs/pull/1/head` plus an unpublished `work`, and a held annotated tag pointing to a tree plus an unpublished `work`. Each should assert that both before and after a fetch the refusal contains "publish the member" and "--check-remotes". Add a double-backend row in `g26` with the advertised object missing but the locked commit held, asserting the full remedy.

**NO-GO**, because P2-A is open.
