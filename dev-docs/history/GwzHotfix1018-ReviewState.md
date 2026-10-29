**Verdict: NO-GO.** The fix never proves a dependency that isn't on the remote: no false composition, so no P0 or P1. But a second, reproducible path to the same `GitCommandFailed "object not found"` symptom is still open (P2).

**Tuple:** checked at start and end. HEAD is `00e3349ab04c` on `hotfix/v1.0.18`, parent `cc2018c00c25` = `v1.0.17`, clean tree. I modified nothing in the lane. All builds ran under `/Volumes/projects/limbo/build-targets/hotfix-1018-review-state/`, which I have deleted.

## Fail-closed analysis
`descends_from_advertised` (`src/workspace_ops/publication.rs` +483..519) can answer true in only two ways:
1. The unchanged `is_ancestor(locked, advertised)`.
2. `is_ancestor(locked, peel(advertised))`. This runs only when the first query failed, the member holds the advertised object, and it peels to a different commit.

The advertised id is content-addressed and the remote holds it, so by connectivity the remote also holds the peeled commit and its history. The peel path therefore adds no proof route that the existing commit path doesn't already have. Each case I checked:

- **Unheld objects, objects held as a tree or blob, malformed ids:** the answer is false, so the push is refused.
- **Classification fails** (`commit_exists` errors), or the object peels to itself: the original error is returned.
- **Nested tags:** peeled recursively.
- **Ref order:** `Ok(true)` from any ref wins, and errors are kept until the loop ends, same as 1.0.17. One failing ref never stops a later ref from proving.
- **Concurrent fetch during the proof:** at worst a transient original error, never a proof.
- **Force-pushes by others, shallow or grafted members:** the exposure is unchanged from 1.0.17 (a race between reading the advertisement and pushing, or a false negative).
- **Refuse path vs. 1.0.17's error path:** both go through `push_root_error(error, Rejected)` (`push_member.rs:389`), and through `?` in `handle_tag.rs:307`. Only the error code differs; nothing is persisted, locked or marked differently.

Experiments, each confirmed by test:
- **Red check:** both new tests fail on 1.0.17's `publication.rs` with `GitCommandFailed`.
- **False-proof mutant** (peel branch returns `Ok(true)`): caught by the annotated-tag test.
- **E5:** a remote ref to a tree, and an annotated tag to a tree, with nothing proving → refusal.
- **E6:** nested tags to an older commit → refusal. A nested tag to the locked commit pushed to the remote → proven, which is correct.
- **Unfetched-commit row:** after the fetch, the root remote's `refs/heads/main` equals the root commit, the same result as a plain `git push`.
- **Full suite** (`python3.13 scripts/run_tests.py --lib`, own target dir): 2032 passed, 0 failed, exit 0.
  - An earlier run reported 2 failures, but it shared a target dir with my mutant build and ran that binary. I discarded it and do not count it.

## Findings

**P2-1: the same symptom remains when the member lacks the locked commit but holds any advertised commit or tag**
- **Location:** `publication.rs`, `descends_from_advertised`. When `is_ancestor` fails, only the *advertised* object is classified. If the *locked* commit is the missing object, a held advertised commit reaches `_ => Err(failure)`. A held annotated tag peels and fails the same way.
- **Violated invariant:** a dependency that cannot be proven is refused with the remedies. The doc comment on `available` says no remedy "would repair a missing … object", but a fetch does repair this case.
- **Reproduction (native):**
  1. Upstream has `refs/heads/old` at the initial commit, which the member holds.
  2. Upstream gets commit L, then M on top; the member has fetched neither.
  3. The root lock names L, and the root is pushed alone, as `gwz --target @root push` does.
  4. Result: `Rejected GitCommandFailed "object not found - no match for id (L)"` on every try.
  5. After `fetch` of the member, the same push succeeds.
- **What decides the outcome:** whether some unrelated advertised ref happens to be held locally. Without the `old` branch, the same state is refused correctly. Real remotes almost always have held branches or release tags.
- **How it arises:** another clone pushes the member and a root lock naming its new commit. This clone pulls only the root, then runs `gwz --target @root push`.
- **Required correction:** after `is_ancestor` fails, also run `commit_exists(&dependency.path, &dependency.commit)`. If that returns `Ok(false)`, answer `Ok(false)`. This is fail-closed: it can only refuse, never prove. Fix the `available` doc comment to match.
- **Regression test:** the row above, plus a variant where the held object is an annotated tag instead of `old`. Assert a `RemoteRejected` refusal that contains "fetch its advertised history and retry". After the fetch, assert `Ok` and the root remote's `refs/heads/main` equals the root commit.

**P3-1: two of the three remedies in the refusal mislead in the scenario the fix now refuses**
- **Location:** `PUBLISH_FETCH_OR_CHECK_REMOTES` in `publication.rs`.
- **Reproduction (E3):** in the unfetched-remote-commit state, `gwz push --check-remotes` fails. The member push is rejected as non-fast-forward ("contains commits that are not present locally"), and the root is "not attempted".
- The "publish the member by pushing a branch" remedy is also misleading: the remote already contains the commit. Only the fetch remedy works.
- **Correction:** when the refusal comes from advertised objects the member doesn't hold, name fetch first, or only fetch. Assert the remedy text in the regression test.

**P3-2: the positive peel answer is never exercised and is redundant with libgit2's advertisement**
- libgit2's ref list already includes the peeled `refs/tags/x^{}` → commit entry for every annotated tag. So `is_ancestor(locked, peel)` is always also evaluated directly.
- **Evidence:** a mutant whose peel branch returns `Ok(false)` still passes `an_annotated_tag_on_the_remote_proves_through_its_commit`.
  - Its second half is proven by the existing direct-advertisement match, not by the peel.
  - libgit2's `graph_descendant_of` doesn't count a commit as its own descendant, so the peel path could never prove a tag sitting exactly on the locked commit anyway.
- **Correction:** either reduce the branch to "a held non-commit object proves nothing" (relying on `^{}` entries, with a comment saying so), or add a row with a hand-built advertisement that has no `^{}` entries and the locked commit strictly an ancestor of the peeled commit. Also fix the test's docstring.

**P3-3: the new annotated-tag test depends on the machine's global git config**
- `last_known_refs.rs:516-520` runs the system `git tag -a` with only `user.name` and `user.email` overridden.
- A developer or CI host with `tag.gpgSign=true` or `tag.forceSignAnnotated=true` will fail the test. The native fixture (`fixture_native.rs:23-24`) pins these settings for exactly this reason.
- **Correction:** add `-c tag.gpgSign=false -c tag.forceSignAnnotated=false`, or create the tag through the backend.
- Otherwise the tests are deterministic: they use local bare repos and no timing. They passed 15 times alone and 3 times in the parallel full suite. They do cover the main fail-closed cases (refusal when the object isn't held, refusal for a tag on an unpublished commit).

**Not a finding:** `git::tests::g08::named_push_reports_server_hook_rejection` failed once, in the discarded mutant-binary run only. It passed in all 4 later full runs, and nothing in this change touches it.

**NO-GO.** P2-1 is open.
