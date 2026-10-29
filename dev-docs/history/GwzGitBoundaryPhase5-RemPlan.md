# Phase 5 rows A, B and guard (5.2, 5.3, 5.5): remediation plan (round 1)

Date: 2026-10-25. Lane owner's plan after [Code](GwzGitBoundaryPhase5-ReviewCode.md) (NO-GO: 2 P2, 4 P3) and [State](GwzGitBoundaryPhase5-ReviewState.md) (NO-GO: 1 P2, 2 P3) on the lane tuple gwz-git `12521b4302`, gwz-core `f0bff70f80`. Both reviewers pre-committed to GO on a bounded remedy. One patch resolves every finding; the same two reviewers re-verdict. No finding is architectural.

Convergence: Code P3-2 and State P3-1 are the same defect (the adoption check credits git2 calls).

C = Code, S = State.

## Blocking

| ID | Disposition | Where | Closure test |
| --- | --- | --- | --- |
| C-P2-1 | Both `transport-candidate.yml` jobs call `checkout-siblings.sh` after their hand checkouts (it leaves existing siblings alone) so gwz-git lands at the pin; gwz-git joins the "pinned commits are on main" loops. Add a source check (Python unit test beside `test_run_boundary_job.py`) asserting that every gwz-core workflow job running `cargo`, `prepare.py` or `run_tests.py` either calls `checkout-siblings.sh` or checks out every `path = "../X"` sibling in `Cargo.toml`; red on the tuple, green after. | gwz-core `.github/workflows/transport-candidate.yml`, `scripts/checks/` | the source check; a green `Transport candidate` run after push (the lane owner verifies post-push). |
| C-P2-2 | gwz-git's `ci.yml` gains `ubuntu-24.04-arm` and `macos-14` legs running fmt, clippy `-D warnings` and `cargo test --locked` (same shape as the Windows job; the LBT rule: the crate tests alone); the stale comment at `ci.yml:11-12` is replaced. Validate the Linux leg's commands on dabeest WSL (`wsl -u root`, 24 cores; rsync the gwz-git and git2-rs/libgit2 checkouts; no credentials) and the macOS leg locally once load allows. | gwz-git `.github/workflows/ci.yml` | gwz-git tests green on Linux (WSL log) and macOS before push; CI green after push. |
| S-P2-1 | `verify.rs`: tag existence by exact reference lookup (`find_reference("refs/tags/{name}")`, `NotFound` → absent; commit by peeling the reference); `resolve("HEAD")` stays for the commit check. Tests on both routes: create+delete `t-1-g<abbrev of HEAD>` → both `Ok`, no tag left; `tag_created` for an absent describe-form name → `Err(… missing after creation)`; after deleting tag `X` with a branch `refs/tags/X` present, `tag_deleted` → `Ok`. Guide A2's "uses `Repository::resolve`" sentence corrected. | gwz-git `src/native_commit/verify.rs`, tests; `GwzGitLibraryApi.md` A2 | the three rows above, red first. |

## Non-blocking (same patch)

| ID | Disposition | Where |
| --- | --- | --- |
| C-P3-1 | The guard also counts, in zero-tolerance files, calls to core's native-handle producers (`open_repo`, `repo_head`, `find_remote`, `git_error`), named in `gwz_git_boundary.json` as `native_producers`; `comparison.rs` moves from `zero` to `exceptions` with its exact count and the G8 row. Unit test: an `open_repo(path)?.merge_base(..)` sample in a zero file is red. | `check_filesystem_boundary.py`, `gwz_git_boundary.json`, tests |
| C-P3-2 = S-P3-1 | The adoption check credits a receiver call only when the file imports the owner from `gwz_git` (`use gwz_git::{…Owner…}`) or writes `gwz_git::Owner`, never when the owner appears as `git2::Owner`; a file reached only through `#[cfg(test)] mod x;` counts as test source. Fixtures: `git2::Repository` + `repo.head()` + `use gwz_git::Error` is not a caller of `Repository::head`. Re-run on the tuple and report any capability that becomes unconsumed (add to the dated ratchet with a reason, do not delete). | `check_gwz_git_adoption.py`, tests, `gwz_git_adoption.json` |
| C-P3-3 | `route::open` delegates to `Repository::open_from_env` through a `pub(crate)` accessor to the native handle; `grep FROM_ENV` finds only `repository.rs`; the `open_from_env` ratchet entry, guide A1 and `lib.rs:24` are corrected (it now has a consumer inside gwz-git; whether that satisfies R9's "production consumer in core" is stated honestly: it does not, the entry stays with the corrected reason). | gwz-git `native_commit/route.rs`, `repository.rs`, `lib.rs`; `gwz_git_adoption.json`; guide A1 |
| C-P3-4 | `RoutePolicy::CLI_ONLY` and `Services::with_policy` move behind a non-default gwz-git cargo feature `test-support`; gwz-core enables it in `[dev-dependencies]` only (`gwz-git = { path = "../gwz-git", features = ["test-support"] }`), so production gwz-core cannot force the CLI route. The adoption check lists public associated constants from now on. | gwz-git `Cargo.toml`, `native_commit.rs`; gwz-core `Cargo.toml`, `spawn_gate_tests.rs`; `check_gwz_git_adoption.py` |
| S-P3-2 | Verify-read failures after a successful write map to `ErrorKind::Operation` with a message stating the write happened and verification failed (`…: written; verification read failed: <native message>`); guide A2 documents the kinds per side of the write. Unit test forcing a verify-read failure after a native write asserts the kind and text. | gwz-git `native_commit/verify.rs`, `cli.rs`; guide A2 |

## Observations carried (no change in this patch)

- Code: lossy-UTF-8 paths and `merge_base` `None` conflation are pre-existing public-API limitations for the Phase 6 API review; the git2-rs pin equality between gwz-git and gwz-core is unchecked; the boundary check's quadratic tokenizer costs ~10 s.
- State: the CLI route's child inherits the live environment (5.2 finding 7; `env_clear()` + `GitEnvironment` snapshot is the remover, scheduled before release); `ProcessGate` as a public trait lets any consumer choose its gating (Phase 1's recorded finding).

## Process

One patch in lane p5-gitmap; the lane owner commits (gwz-git first, pin moved in the gwz-core commit); re-verdict by the same two reviewers against the new tuple; files `-ReviewCode-2.md` / `-ReviewState-2.md`. Round count after this patch: 1 of 2.
