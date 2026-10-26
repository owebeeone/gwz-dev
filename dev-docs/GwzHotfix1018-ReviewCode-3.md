# Final re-verdict (round 2 of 2), Code axis: gwz-core `17b965f7` (range `659118d2..17b965f7`)

**GO.** R2-1 and R2-2 are both closed, and there are no new findings. Everything I raised in earlier rounds stays closed.

**Tuple.** I checked it at the start and the end: HEAD is `17b965f7` on `hotfix/v1.0.18` and the tree is clean. My scratch dir under `build-targets/hotfix-1018-review-code/` is deleted. On this tree:
- g08 + g26 pass: 80 rows.
- `cargo clippy --all-targets --all-features -D warnings` is clean, which is how release.py runs it.
- `cargo fmt --check` is clean.

## Closure table

| Finding | Status | Evidence on `17b965f7` |
| --- | --- | --- |
| **R2-1** (P2: a locked commit published nowhere got an endless "fetch and retry") | **Closed** | The locked commit unheld with the advertised object held now answers `Diverges`, which leads to the full remedy. My probe is now the native row `a_locked_commit_published_nowhere_is_refused_with_the_publish_remedy`. It asserts the full remedy both before and after a fetch. A mutant that puts back `(false, true) → Unheld` fails that row and the g26 row `a_missing_object_names_the_fetch_first_only_when_a_fetch_could_supply_the_commit`. The fetch-first text can no longer loop, because it is now only a prefix ("…; if the retry is still refused, ") followed by the full remedy. The doc comments on `FETCH_FIRST`, `Ancestry` and `descends_from_advertised` now match the behaviour. |
| **R2-2** (P3: equality short-circuit in `pushed_source_descends` not pinned) | **Closed** | The positive OOS-1 row now asserts the exact transport sequence: two reads, then the member push, then the root push. A mutant that deletes the short-circuit fails `a_pushed_annotated_tag_proves_the_root_dependency_through_its_commit`. |
| OOS-1, P3-1, P3-2, P3-3 | Still closed | Their guard rows are unchanged and green. |

## Checks on the new classification
- **Nothing new can prove a dependency.** The `(commit_held, object_held)` table only ever chooses between `Diverges` and `Unheld`, so the new code can refuse but never prove.
- **The originally reported case still names the fetch.** That is the locked commit held and the advertised object unheld. It now gets the full remedy, which includes "fetch its advertised history and retry".
- **Implementer's tag-to-tree edge is harmless.** A locked commit unheld plus a held tag pointing at a tree is classified as both unheld. It ends at the full remedy, so it does not loop.
- **Shallow member: not a defect.** If the locked commit lies beyond the shallow boundary while the advertised object is held, the result is `Diverges` and the full remedy (which names the fetch), not the raw error. The full remedy still names the fetch, and the refusal is still correct.

## New findings
None.

## Verdict
**GO.**
