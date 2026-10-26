# Code review: gwz-core `00e3349a` (hotfix/v1.0.18), Code axis

**Verdict: GO.** No P0, P1 or P2 finding is open against the object. One defect I found and confirmed with the same cause sits outside the hotfix's scope; it is recorded separately as OOS-1 and does not block. Three P3 items follow.

## Tuple
I checked it at the start and again at the end, and it did not change:
- HEAD is `00e3349a` on `hotfix/v1.0.18`, its parent `cc2018c0` is `v1.0.17`, and the tree is clean.
- main is `ae2c2f3d` and clean.
- The sibling repos used read-only for the main build are clean: `git2-rs` d13951f, `gwz-git` 6dcb6b8, `gwz-transport` 3980469, `gwz-sspi` a2978a8.
- All builds ran in `/Volumes/projects/limbo/build-targets/hotfix-1018-review-code/`, which is now deleted. `TMPDIR` was `/Volumes/gwztmp`, `SSH_AUTH_SOCK` was unset, and no network remote was touched.

## What I verified
- **Root cause.** I put 1.0.17's `publication.rs` under the new rows. Both fail with the reported errors: `object not found - no match for id (4ce5d24b…)` and `object is no commit object`. With the fix, both pass.
- **Classification uses error types, not message text.**
  - It uses `commit_exists` (`src/git/gitbackend/repository.rs:15`), which matches `ErrorCode::NotFound` and then `peel_to_commit()`. No error message is parsed.
  - A lightweight or annotated tag pointing at a tree or blob answers false, so it proves nothing. A tag of a tag resolves through `^{commit}`.
  - In the fake backend, `is_ancestor` returns `Err("commit missing")` and `commit_exists` returns false, so an unheld object proves nothing.
  - When classification itself fails, the original error is returned. For example, the tracking double's default `commit_exists` returns `UnsupportedOperation`.
- **No false proof.** The only new way to answer `Ok(true)` is ancestry of a commit peeled from a tag object the remote advertises, and the remote must hold that commit.
- **Earlier outcomes are kept.**
  - An unrelated history still answers `Ok(false)`.
  - The exact-match prefilter is unchanged.
  - A held commit whose ancestry query fails still returns the original error. This is true in the code, but no test pins it (see P3-2).
- **Mutants against g08 + g26 (73 rows).**
  - Treating every failed query as false is caught by `g26::an_ancestry_error_refuses_the_root_with_the_git_error_not_the_publish_remedy`.
  - Classifying without peeling is caught by the first half of the annotated-tag row.
- **API and compatibility.** Only private functions changed. There is no new dependency and no wire change. The only behaviour change is the intended one: these cases now come back as a `RemoteRejected` refusal instead of `GitCommandFailed`.
- **Release lints.** `cargo clippy --all-targets --all-features -D warnings` is clean and `cargo fmt --check` is clean. The g08::last_known_refs and g26 rows pass (50 tests), and all of g08 + g26 pass (73). I did not run the full suite either.
- **Other call sites.** `available()` serves both `RootProof::plan` and `check`, and covers both the push path and `gwz tag push`. The `is_ancestor` uses in pull preflight and merge work on fetched or local objects, not on advertised ones. The one remaining site with the same pattern is OOS-1.
- **Forward-port to main `ae2c2f3d`.** `git apply` applies the patch cleanly. On main the backend has moved into `gwz-git`, but the semantics match: `commit_exists` peels tags, `resolve` maps `revparse` NotFound to None, and `is_ancestor` uses `graph_descendant_of`. With the patch applied to a copy of main, the new rows plus g26 pass (48 tests).

## Out-of-scope finding (does not block)

**OOS-1. The same root cause remains in `dependency_was_published`.** This is a confirmed defect that already exists in 1.0.17; the hotfix did not introduce it.
- **Location.** `src/workspace_ops/publication.rs`, in `dependency_was_published` (about lines 545–580 at `00e3349a`). It calls `backend.is_ancestor(&dependency.path, &dependency.commit, source)`, where `source` is the oid pinned for a member push the remote accepted.
- **What goes wrong.** For an annotated-tag refspec, that oid is the tag object. libgit2 fails the ancestry query and the root is rejected, even though the remote accepted the member's push and so holds the commit (D8).
- **Reproduction.** I ran a native probe in the scratch copy:
  - Setup: the root lock names an unpublished member commit `work`. The member has an annotated `v9` on `work`, and the root has an annotated `v9`.
  - Request: `handle_push` with `PushRequest.refspec = "refs/tags/v9:refs/tags/v9"`.
  - Result: `mem_app` comes back `Ok` and the upstream now holds `v9`. `@root` is `Rejected` with `GitCommandFailed "object is no commit object"`, and the root tag is not pushed.
- **Reachability.** It is reachable through the public gwz-core API (`PushRequest.refspec`) and through gwz-py `client.push(refspec=…)` (`gwz-py/src/gwz/client.py:829,840`). It is not reachable from the gwz CLI, which always sends `refspec: None` (`gwz-cli/src/clirequest/invocation.rs:258`). It is not reachable from `gwz tag push` either, which passes an empty `published`.
- **Correction.** Generalize `descends_from_advertised` to take any object and use it for the pushed sources. Pushed sources are held locally, so the peel path would prove them. This is roughly 10 lines of code plus the probe as a native test row.
- **Severity and decision.** On its own merits this is P2-class. It is outside the hotfix boundary and does not block the reported outcome, so whether to fold it into 1.0.18 is your decision.

## P3 findings

**P3-1. The peel-and-ask branch is not pinned, and the test overstates what it shows.**
- **What the test claims.** The second half of `an_annotated_tag_on_the_remote_proves_through_its_commit` (`last_known_refs.rs`, at the end of the file) is described as proving "through the commit it peels to".
- **What actually happens.** libgit2 always advertises a `refs/tags/v2^{}` row. A probe confirmed this for the native local transport: `refs/tags/v1^{}` appears with the peeled commit. That row matches `work` in `available()`'s exact-match prefilter, so the proof succeeds before the peel branch runs.
- **Evidence.** A mutant that answers `Ok(false)` after peeling survives all 73 rows.
- **Consequences.**
  - The ask is redundant in practice.
  - On a refusal path, each annotated tag now costs an extra ancestry walk that repeats the walk for its `^{}` row.
  - There is a latent corner case. If the peeled commit equals `dependency.commit` and no `^{}` row is advertised, native `graph_descendant_of` returns false for equal commits and the proof refuses. That is the safe direction, and it cannot happen with libgit2 1.9 advertisements.
- **Correction.** Choose one:
  - Add a tracking-double row that serves only the tag object, and short-circuit `commit == dependency.commit → Ok(true)`.
  - Or remove the ask and the commit-equality concern, and reword the test and the code comments.

**P3-2. Preservation of the original error is not pinned on any backend that classifies.**
- **The gap.** The g26 shallow-history row passes only through the classification-failure route, because the tracking double lacks `commit_exists` and falls back to the trait default `UnsupportedOperation`.
- **Evidence.** A mutant that changes `Ok(Some(_))` (a held commit whose ancestry failed) to `Ok(false)` survives all 73 rows.
- **Why it matters.** The claim that shallow-history behaviour is unchanged rests on untested code.
- **Correction.** Give the tracking double `commit_exists` and `^{commit}` answers. Add a row where the advertised object is a held commit, its ancestry query returns `Err("shallow history")`, and the root error carries `"shallow history"` rather than the remedy text.

**P3-3. Parity note, existing behaviour, informational.** Native `is_ancestor(x, x)` returns false, while the fake backend returns true. The fix makes this observable only through the corner case in P3-1.

## Verdict
**GO.** The fix is correct for its stated defect, is typed rather than string-matched, introduces no false proof, keeps earlier outcomes, is safe as a patch, and forward-ports cleanly to main. OOS-1 is a confirmed same-cause defect off the CLI route, for your decision on folding it into 1.0.18. P3-1 and P3-2 are gaps in test coverage.
