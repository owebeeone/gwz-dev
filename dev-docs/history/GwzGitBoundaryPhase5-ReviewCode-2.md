# Phase 5 rows A, B and guard (steps 5.2, 5.3, 5.5), round 2: code-axis re-verdict

**Review object:** the same rows as round 1 plus the remediation patch, in lane `p5-gitmap`: gwz-git `7444b1f..6dcb6b84f7` and gwz-core `642211b9..a6ae99ae95`. The remediation adds one commit in each member: gwz-git `6dcb6b8` and gwz-core `a6ae99ae`. Evidence run: gwz-core-evidence `75548ae558`, `2026-10-25-p5-rem1`. Lane root: `7d13f912f0`, which holds the plan `dev-docs/GwzGitBoundaryPhase5-RemPlan.md` and the filed reports.
**Baseline:** round 1 at gwz-git `12521b4302` and gwz-core `f0bff70f80`.
**Date:** 2026-10-25
**Axis:** Code
**Verdict:** **GO.** P0 0, P1 0, P2 0, P3 1 (P3-2 partly open, carried; it does not block).

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P2-1 (transport-candidate jobs lack gwz-git) | Both jobs gained "Check out gwz-git at its pin" through `checkout-siblings.sh` (`transport-candidate.yml` around lines 107 and 296). gwz-git joined the "pinned commits are on main" loops. New source test `test_workflow_siblings.py`, added to the boundary unittest list. | `test_every_real_workflow_job_has_every_sibling` passes. `test_the_transport_candidate_workflow_at_the_tuple_before_the_remedy_is_red` reproduces round 1's gap: both jobs missing `gwz-git`. The new step comes before every cargo, prepare.py and run_tests.py step in both jobs. My round-1 prepare.py probe passes once the sibling is present. A green run after push is still the lane owner's to show, as the RemPlan states. | **Closed** |
| P2-2 (no Linux or macOS CI for gwz-git) | New `unix` matrix job in gwz-git `ci.yml` on `ubuntu-24.04-arm` and `macos-14`: fmt, clippy `-D warnings`, `cargo test --locked`. Stale comment rewritten. Source assertion `test_gwz_git_tests_run_on_linux_and_macos_and_windows`. | The workflow text matches the claim. Linux logs (`raw/remote/gwz-git-linux/03,04`, x86-64 WSL; `linux-raw-sys` in the build) show 139 lib, 4, 6, 9, 7, 18 and 1 + 6 doctests, with and without `test-support`. That exercises `Host::Unix`. I re-ran `cargo test --locked --no-fail-fast` in gwz-git on macOS: 139 lib, 4, 6, 9, 7, 18, doctests 1 + 6, all green. The CI leg is arm64 while the remote run was x86-64; that is acceptable. | **Closed** |
| P3-1 (guard misses native handles reached through core helpers) | The guard counts uses of `native_producers` (`open_repo`, `repo_head`, `find_remote`, `git_error`): zero allowed in `zero` files, exact counts in exceptions. `comparison.rs` moved to `exceptions` (G8: 0 git2 tokens, 3 producer uses). | Re-ran my samples through `producer_uses`: the glob-helper `open_repo(p)?.merge_base` sample counts 1 use; `use … open_repo as o` counts 1; a qualified path call counts 1; a function-pointer reference counts 1. `open_repo` is the only core function that returns `git2::Repository`. The checker reports 4 zero files, 7 exception files, 82 git2 tokens and 142 producer uses. Unit tests pass. | **Closed** |
| P3-2 (= State S-P3-1; adoption check credits same-name calls on git2 or `GitBackend`) | A receiver call now counts only in a file that takes the owner from gwz-git. Test-gated `mod` files, and everything below them, are treated as test source. Caveat in the README: a file that takes the owner from gwz-git and also names `git2::Owner` is still credited; "no such file exists in gwz-core today". | The `git2` and `StatusOptions` false positives are gone. The caveat's premise is false. `src/git/gitbackend/repository.rs` takes `Repository` from gwz-git (line 7, `gwz_git::Repository::is_repository`), names `git2::Repository` (line 13), and calls `backend.head(path)` (lines 35 and 88), which is a `GitBackend` call. My probe removed every `.head(` from `reads.rs` in memory: `Repository::head` was then still credited to `repository.rs`. Round 1's counterexample still reproduces for this capability. | **Partly open, P3-2 carried** (see §1) |
| P3-3 (unconsumed `open_from_env` and a duplicated open) | `route::open` delegates to `Repository::open_from_env` through a crate-private `into_native`. Ratchet reason, guide A1 and `lib.rs` corrected. | `grep FROM_ENV gwz-git/src` finds only `repository.rs:31`. `route.rs:83-87` delegates. The ratchet now says honestly that the consumer is inside gwz-git and that core has none. The route and differential tests are green in my run. | **Closed** |
| P3-4 (test-only route knob in the public API) | `RoutePolicy::CLI_ONLY` and `Services::with_policy` sit behind `cfg(any(test, feature = "test-support"))`. `Services::policy` is crate-private. gwz-core enables the feature only in `[dev-dependencies]` and calls `with_policy` only under `cfg(test)`. The adoption check lists public constants; two dated exceptions added. | `cargo tree -e normal,features -i gwz-git` shows only `default` (no `test-support`); evidence `04-greps.log` shows `[default,test-support]` with dev edges. In core, production code calls `with_policy` only inside the `cfg_if` test arm (`commit_tag.rs:16-22`). The adoption check reports 39 capabilities, 32 consumed and 7 dated exceptions. `RoutePolicy::AUTO` has a real consumer (`backend.rs:67,101`). gwz-cli takes gwz-core as a path dependency, so gwz-core's dev-dependency features do not reach it. | **Closed** |

## Changed-range analysis

What the remediation commits changed beyond my own dispositions:

- **gwz-git `verify.rs` and `refs.rs`, the State findings S-P2-1 and S-P3-2.**
  - Tag existence is now an exact `find_reference("refs/tags/{name}")` lookup (`refs.rs:61-80`), and the tagged commit comes from peeling that reference. This removes the revparse ambiguity the State axis found: describe-form names, and a branch named `refs/tags/X`.
  - A verification read that fails after a write is now `Error::written_but_unverified` (`error.rs:119`): kind `Operation`, native diagnostics kept, message `<what>: written; verification read failed: …`. Reads before the write (`verify::head` in `cli::commit`) keep their own kinds.
  - Core's error code is unchanged: `model_error` maps `Operation` to `GitCommandFailed`. Only the message text changes. No core caller matches on these texts.
  - A tag that exists but cannot be peeled to a commit now reports "verification read failed" instead of "did not resolve". Neither route can produce such a tag, because tags are always created at HEAD.
  - `verify.rs` has 6 tests (red log `01-sp2-red.log`); all are green in my run.
- **Adoption check detection of test-gated modules** (`gated_mods`, `CFG_ATTR` anchored with `$`). It reads raw text with regexes. A doc comment between `#[cfg(test)]` and `mod x;` would leave `x` counted as production. That gap is bounded and causes no false credit today.
- **`test_workflow_siblings.py`** reads workflows as text; there is no YAML library on CI. It exempts the bootstrap crate. The heuristics fit the current workflows.
- **`Git2Backend.route_policy`** is still read in production code (copied at `gitbackend.rs:226`), but non-test builds ignore it. It is a dormant field, not a defect.
- **Scope.** The `test-support` feature, the gwz-git CI legs, `test_workflow_siblings.py` and the guard and adoption extensions each trace to a filed finding (RemPlan). They change only gwz-git, gwz-core and the evidence member.
- **No new architectural root cause.**

## 0. Evidence base

**Tuple, identical at start and end**

| Member | HEAD | State |
|---|---|---|
| gwz-core | `a6ae99ae95277a225227f649e713e346b116f9b0` | clean; pin `6dcb6b84f77e1448cfebbb306e1c0d9bc2b96d49` |
| gwz-git | `6dcb6b84f77e1448cfebbb306e1c0d9bc2b96d49` | clean; `.github/git2-rs.commit` `d13951f7`, equal to core's |
| gwz-core-evidence | `75548ae55809c7d6fce13924b6e9af909d8d2f87` | the pre-existing untracked run directory |
| gwz-transport | `a27817b9c8` | clean |
| git2-rs | `d13951f7e0` | clean |
| lane root | `7d13f912f0` | the pre-existing untracked `dev-docs` |

**Read:**
- `git diff f0bff70f..a6ae99ae` and `git diff 12521b43..6dcb6b84` in full.
- The rem1 README and its logs: `raw/mac/04-greps.log`, `raw/mac/12-*`, and the Linux test logs under `raw/remote/gwz-git-linux/`.

**Ran:**
- Unit tests `test_workflow_siblings.py`, `test_check_gwz_git_adoption.py` and `test_check_filesystem_boundary.py`: 48 OK.
- `check_gwz_git_adoption.py` and `check_filesystem_boundary.py`: both pass.
- `CARGO_TARGET_DIR=/Volumes/projects/limbo/build-targets/p5-gitmap-gwzgit cargo test --locked --no-fail-fast` in gwz-git: all green. The load was 27 when it started.
- `cargo tree --locked --offline` feature inversion in gwz-core.
- Scratchpad probes: the producer-use samples, caller attribution per file, and the reads.rs head-removal simulation for P3-2.

## Problem, requirement provenance, and scope

Authority is unchanged: D-G, D-K, plan steps 5.2, 5.3 and 5.5, and R9. Every remediation item traces to a filed round-1 finding (Code C-*, State S-*) through the RemPlan.

- The `test-support` feature is a narrowing, not a new capability. It makes test facilities unavailable to production builds; that is R9's "test-only consumer does not count".
- The gwz-git CI legs restore coverage the move removed.
- The workflow test restores a demonstrated CI gap.

Only gwz-git, gwz-core and the evidence member changed. Nothing unauthorized was found.

## 1. Findings

### P3-2 (carried, partly open): the adoption check still credits a gwz-git receiver capability to a same-name call in a file that also takes the owner from gwz-git

- **Location:** `gwz-core/scripts/checks/check_gwz_git_adoption.py:336-352` (`names_owner`) together with `:354` (`first_caller`). The live instance is `src/git/gitbackend/repository.rs`:
  - line 7: `gwz_git::Repository::is_repository`;
  - line 13: `git2::Repository::init_opts`;
  - lines 35 and 88: `backend.head(path)`, a `GitBackend` call.
- **Violated invariant:** R9. Map §6 says the ratchet must fail when a capability loses its last production caller.
- **Reproduction:** in the checker's in-memory caller tokens, remove every `. head (` sequence from `reads.rs`. `first_caller(Repository::head)` then returns `src/git/gitbackend/repository.rs`.
- **Impact:** bounded. If `reads.rs` stopped delegating `head` to gwz-git, the adoption check would not object. The same holds for any receiver name that collides with a `GitBackend` method in a file that names `gwz_git::Repository`.
- **The literal-reading caveat:** taking the owner from gwz-git is a reasonable lexical rule, but the README's "no such file exists" is wrong. The plan's original wording, "not when `git2::Owner` appears", restricted to receiver matches, is implementable. `repository.rs` would then still be credited for `is_repository`, because that is a qualified path match, not a receiver match. The cost is in the safe direction: a file that genuinely does both would be under-credited and fail loudly.
- **Required correction:** for receiver matches only, do not credit a file that also names `git2::Owner`. Alternatively, record an expected-caller file for each colliding capability in `gwz_git_adoption.json`.
- **Closure test:** a unit test with a file that holds `gwz_git::Repository::is_repository(..)`, `git2::Repository::init(..)` and `backend.head(p)`. It must not satisfy `Repository::head`.
- **Release impact:** it does not block GO.

## 2. Invariant analysis (attacks that failed, with evidence)

- **P2-1:** with the sibling present, prepare.py anchors every path dependency, and the workflow test covers both jobs.
- **P2-2:** the gwz-git suite is green on Linux (log) and on macOS (my run).
- **P3-1:** none of my bypass samples (glob import, alias, qualified path, function pointer) escapes the producer count. `open_repo` is the only `git2::Repository` producer.
- **P3-3:** there is exactly one `FROM_ENV` site.
- **P3-4:** production gwz-core builds gwz-git with features `[default]` only.
- **Public surface:** no git2 type appears on gwz-git's public API after the patch. `into_native`, `tag_ref_exists` and `tag_commit` are crate-private.
- **Delegation:** round-1 §2 still holds. The pin names the gwz-git commit that contains the code core calls.

## 3. Risks and next action

**Risks:**
- The post-push `Transport candidate` and gwz-git CI runs are still unobserved; they are the lane owner's to show, as recorded.
- The guard and the adoption check are lexical by design. The P3-2 residual and the doc-comment-between-attribute-and-`mod` gap are the known edges.
- Round 1's observations still stand: lossy path strings in the public API, `merge_base` returning `None` when a commit is missing, case-insensitive environment names on Unix, and the publish order of gwz-git and gwz-core at Phase 8.

**Next action:**
1. Close P3-2 with the receiver-match rule and its unit test, in any later package or before the phase review closes.
2. Confirm green CI after push: `Transport candidate` (both jobs) and gwz-git `ci.yml` (all three legs).
