# Phase 5 rows A, B and guard (steps 5.2, 5.3, 5.5) — STATE-AXIS REVIEW, round 2 (re-verdict)

**Review object:** the remediation patch.
- gwz-git `12521b43..6dcb6b84f7` (1 commit)
- gwz-core `f0bff70f80..a6ae99ae95` (1 commit; pin moved to `6dcb6b84`)
- gwz-core-evidence `75548ae558` (run `2026-10-25-p5-rem1`)
- lane root `7d13f912f0` (filed reports and `dev-docs/GwzGitBoundaryPhase5-RemPlan.md`)

**Baseline:** the round-1 object, gwz-git `12521b4302`, gwz-core `f0bff70f80`.
**Date:** 2026-10-25.
**Axis:** State.
**Verdict:** **GO.** Prior findings closed: P2-1, P3-1, P3-2. New findings: P0 0, P1 0, P2 0, P3 0. Two residual observations, neither blocking.

## Prior-finding closure table

| ID | Original defect | Remedy in the changed range | Re-run of the original counterexample | Status |
|---|---|---|---|---|
| **P2-1** | `verify.rs` checked whether a tag exists with `Repository::resolve` (`revparse`). The describe fallback and dwim rules made a deleted or absent tag look present. `tag_delete` of `nightly-1-g<abbrev>` reported "still present after delete" after a real delete, and `tag_created` could confirm a tag that was missing. | `verify.rs` `tag_created` → `Repository::tag_commit` and `tag_deleted` → `tag_ref_exists`. Both are new `pub(crate)` in `refs.rs:58-80`, with exact `find_reference("refs/tags/<name>")`, `NotFound` meaning absent, and the commit taken by peeling the found reference. `resolve("HEAD")` stays for the commit check. | **gwz-git `cargo test --locked` (mine, rustc 1.95.0):** 139 lib pass, including all six `native_commit::tests::verify::*` rows. These cover `t-1-g<abbrev>` lightweight and annotated, on both routes through the hermetic twin; the twin asserts `Route::Native` on its native side and uses `CLI_ONLY` on its CLI side (`support.rs:366-400`). They also cover the dwim branch `refs/heads/refs/tags/X`, and `tag_created` refusing an absent describe-form name ("missing after creation"). The red log `raw/mac/01-sp2-red.log` shows 5 of the 6 failing before the fix; the sixth is a regression guard that passed before too. **My own program on the new production rlib** (`libgwz_git-7d81c323752e79ed.rlib`, no features): `t-1-g62cf04f`, `nightly-12-g62cf04f21` and `X` (with the branch `refs/heads/refs/tags/X` present), each lightweight and annotated, all returned `create=Ok(HEAD)`, `delete=Ok(())`, and left no tag. On this host both legs ran the CLI route (Homebrew's system gitconfig and an injected snapshot `GIT_*` both force it), so my program covers the CLI route. The native route is covered by the gwz-git twin rows above. | **Closed** |
| **P3-1** | The adoption check credited `git2` calls (`git2::Repository`, `repo.head()`, `git2::StatusOptions::new()`, `entry.status()`) as gwz-git consumption, because a file only had to name `gwz_git` anywhere. | `names_owner`: a call counts only when the file takes the owner from gwz-git (`use gwz_git::..` leaf, or a path `gwz_git::..::Owner`; `git2::Owner` is excluded). Files reached only through a test-gated `mod x;` are excluded. Public associated `const`s are now listed. | My fixture (`use gwz_git::Error;` + `repo: &git2::Repository` + `repo.head()` + `git2::StatusOptions::new()` + `entry.status()` + `x.format(1)`) now credits **nothing** for `Repository::head`, `Repository::status`, `StatusOptions::new` or `ObjectId::format`. In the real tree, `verify_checkout_state.rs` no longer counts for `Repository::head`/`status`/`StatusOptions::new`. The check itself: 39 capabilities, 32 consumed, 7 dated exceptions, no failures. The 48 unit tests across the three check suites pass. | **Closed** (residual O-1, §3) |
| **P3-2** | Guide A2 said "every failure is `Operation`", yet a verification read after the write returned `RepositoryOpen`/`NotARepository`/`ObjectRead`. | New `Error::written_but_unverified` (`error.rs:116-128`): kind `Operation`, message `<what>: written; verification read failed: <native message>`, native diagnostics kept. `verify::head_after` covers native and CLI commit; `tag_created` and `tag_deleted` wrap both their open and their lookup. The pre-write `verify::head` keeps its own kind. Guide A2 now says "Errors, by side of the write", and the Errors table row for `Operation` is amended. | The row `a_verification_read_that_fails_after_the_write_says_so_with_the_operation_kind` passes in my run. It asserts kind `Operation` and the "written; verification read failed:" text for a garbage loose tag ref and for a non-repository path, across `tag_created`, `tag_deleted` and `head_after`, and asserts `NotARepository` for the pre-write `head`. I read guide A2's new text against the code and it matches. | **Closed** (residual O-2, §3) |

## Changed-range analysis

No NEW ARCHITECTURAL root cause.

- **gwz-git `verify.rs`/`refs.rs` (state).**
  - Exact lookup reads loose and packed refs. A concurrent re-creation after a delete is still caught ("still present"), and a concurrent deletion after a create is now caught ("missing after creation"). The direction is fail-closed.
  - An invalid refname reaching `find_reference` (`EINVALIDSPEC`) is wrapped as "written; verification read failed". It cannot occur after a successful write: the native route refuses bad names before writing (`CliReason::TagName`), and `git tag` refuses them with a nonzero exit.
  - A tag pointing at a non-commit fails `peel_to_commit`, so it is still an error, now with the "written" text. The old code's `^{commit}` revparse also errored there.
- **`route::open` → `Repository::open_from_env(..).map(into_native)`.** The flags are identical (`NO_SEARCH | FROM_ENV`, no ceiling directories) and the error still maps to `CliReason::CannotOpen`, so route behaviour is unchanged. `into_native` is `pub(crate)` and consumes the wrapper, so the public surface does not grow. `grep FROM_ENV` in gwz-git now finds only `repository.rs`.
- **`test-support` feature.**
  - `RoutePolicy::CLI_ONLY` and `Services::with_policy` sit behind `cfg(any(test, feature = "test-support"))`, and `Services.policy` is `pub(crate)`.
  - My negative compile against the production rlib fails with `E0599: no associated item named CLI_ONLY`.
  - gwz-core enables the feature in `[dev-dependencies]` only. Its production `commit_tag.rs` builds `Services` without a policy (`cfg_if!` test arm only), and both constructors set `RoutePolicy::AUTO`, so production cannot force a route.
  - Resolver 2: dev-dependency features apply only to builds that include test targets, so release builds of gwz-cli or gwz-core never see the feature. Core's spawn-gate tests, which use `CLI_ONLY` through the dev-dependency, pass (12 of 12, `GWZ_TEST_GIT=real`).
- **Guard (`check_filesystem_boundary.py`, `gwz_git_boundary.json`).**
  - Uses of `open_repo`, `repo_head`, `find_remote` and `git_error` are now counted exactly per exception (142 in 7 files), and are forbidden outright in `zero` files.
  - An exception with no uses left fails until it is deleted.
  - `comparison.rs` moved from `zero` to an exception with `git2: 0` and `producers: 3`. Its git2 count is still exact, so any git2 token fails. The ratchet is not weakened.
  - My run: passed, "4 zero-tolerance files, 7 files with 82 … and 142 …".
- **Adoption-check changes only reduce credit**, through the owner-provenance requirement and the test-gated-module exclusion. A false GO cannot newly arise from them; a mistake would show up as red.
- **CI legs and workflow siblings.** gwz-git now has `ubuntu-24.04-arm` and `macos-14` jobs, and `transport-candidate.yml` checks out gwz-git at the pin. These are configuration, not durable-state changes. The evidence shows them green on WSL Linux, macOS and the Windows receipt; the post-push CI result is the lane owner's to report.
- **Scope and provenance.**
  - Every change traces to a round-1 finding (S-P2-1, S-P3-1/C-P3-2, S-P3-2, C-P2-1, C-P2-2, C-P3-1, C-P3-3, C-P3-4) through the RemPlan.
  - Only gwz-git, gwz-core and the evidence member changed code. The lane root carries only the filed process documents.
  - The `test-support` feature, `into_native` and the CI legs are bounded remedies of those findings. None is an expansion.

## 0. Evidence base

- **Tuple at start and end:** unchanged.
  - gwz-git `6dcb6b84f77e1448cfebbb306e1c0d9bc2b96d49`, clean.
  - gwz-core `a6ae99ae95277a225227f649e713e346b116f9b0`, clean.
  - gwz-core-evidence `75548ae55809c7d6fce13924b6e9af909d8d2f87`. The only untracked path is `campaigns/transport-qualification/runs/2026-09-22-alpha-setup-timeout/`, which was already there in round 1 and is not part of the object.
  - Lane root `7d13f912f0c9743354924933017c85f10872baaa`.
- **Load:** the 1-minute load was 23–48. I ran one cargo command at a time:
  - gwz-git `cargo +1.95.0 test --locked` (target `p5-gitmap-gwzgit`): 139 lib, 4, 6, 9, 7, 18, 1 + 6 doctests, all passing.
  - gwz-core `cargo test --locked --lib gwz_git_tests` with `GWZ_TEST_GIT=real` (target `p5-gitmap-core`): 8 of 8.
  - gwz-core `cargo test --locked --lib spawn_gate`: 12 of 12.
- **Python, no build, run with `-B` (no `__pycache__`; the tree stayed clean):**
  - `check_filesystem_boundary.py`: pass.
  - `check_gwz_git_adoption.py`: pass.
  - The unit tests of the guard, adoption and workflow-siblings checks: 48 OK.
- **Scratch programs** (in my scratchpad only), linked with rustc 1.95.0 against the production rlib: the P2-1 counterexamples and the negative `CLI_ONLY` compile.
- **Read:**
  - The full diffs of both commits.
  - The rem1 README.
  - The `01-sp2-red.log` red log.
  - The listing of the remote (WSL Linux) and Windows results.

## 1. Findings

None open. The three round-1 State findings are closed (table above). This round found no new P0–P3.

## 2. Invariant analysis (attacks that failed, with evidence)

- **AD1 tag existence cannot be impersonated.**
  - Neither a describe-form name, lightweight or annotated, nor a branch `refs/tags/X` can stand in for the tag. The verify rows and my program show this; the native route is covered by the twin rows, the CLI route by both.
  - An absent describe-form tag is reported missing.
- **Fail direction is preserved.** No path now reports success without the write having happened.
  - Read-backs after a write either confirm the state or return `Operation` saying the write happened.
  - The pre-write read keeps its own kind, so a consumer that matches on kind still sees "nothing written" there.
- **Route behaviour is unchanged.** `route::open` uses identical flags. The full moved route and differential suites pass: 139 = 133 + 6 on macOS, Linux and Windows (127 there).
- **Production cannot force a route.** The negative compile fails, the core production `services()` sets no policy, and the feature is in dev-dependencies only.
- **The ratchet files cannot drift silently.**
  - The guard counts exactly in both directions for git2 and producer uses, and deletes an entry only at zero.
  - The adoption ratchet now credits only calls on owners taken from gwz-git. Unconsumed `CLI_ONLY`/`with_policy`/`open_from_env` stay listed with corrected reasons.
- **Platform parity.**
  - Windows receipt `p5-rem1-20261025-a`: PASS, hosted-like 3,931 passed with the 4 named host gaps.
  - gwz-git on Windows: 127 lib passed.
  - WSL Linux: 139 lib passed, with and without `test-support`.
  - The new behaviour has no platform branch.

## 3. Risks and next action

- **O-1 (residual of P3-1, lexical by design):**
  - Once a file takes an owner from gwz-git, any receiver call with the same method name counts. `Repository::head` is still credited to `gitbackend/repository.rs`, because it names `gwz_git::Repository::is_repository` and calls `backend.head(path)` (the `Git2Backend` method). A file that imports `gwz_git::Error` and calls `y.native()` on anything would credit `Error::native`.
  - The real callers exist today, and the probe tests (`gwz_git_tests.rs`) plus clippy's dead-code check are the backstops, so this does not block.
  - Typed receivers would need name resolution, which the check's docstring explicitly rules out. Record it if a later row leans on adoption for a common method name.
- **O-2 (residual of P3-2):**
  - The guide says a consumer "can therefore tell 'nothing changed' from 'written, not confirmed'". That holds by kind only for the pre-write reads. Among `Operation` errors, the distinction rests on the "written; verification read failed" text, which the same guide calls diagnostic rather than a stable match key.
  - Post-write mismatches carry plain `Operation` text with no "written" marker: "HEAD is not the new commit after commit", "git commit did not advance HEAD", "tag … missing after creation", "… still present after delete".
  - Core maps all of these to `GitCommandFailed`, so nothing breaks today. If a consumer needs the distinction, give it a stable discriminator (an accessor or a kind) rather than text.
- **Carried observations (outside this object):**
  - The CLI route still inherits the live environment (the `GIT_INDEX_FILE`-from-a-hook hazard from round 1).
  - gwz-cli, gwz-py and the lane-root `Cargo.lock` have no `gwz-git` entry; this must be refreshed at the root merge.
  - The rem1 README notes that `build-targets/tools/remote-gates.zsh` is broken (`LANE_DIR` unbound).
- **Next action:** none on the State axis. The lane owner merges once the Code axis verdict and post-push CI (gwz-git `ci.yml` unix legs, `Transport candidate`) are green.
