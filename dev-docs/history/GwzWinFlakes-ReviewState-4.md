# State-axis re-verdict: win-flakes gwz-core `0f5ec3e0` (round 2)

- **Object:** gwz-core `0f5ec3e0` on `101243b5`, reviewed as `git diff 101243b5 0f5ec3e0` (3 files).
- **Date:** 2026-10-31
- **Axis:** State
- **Round:** 2
- **HEAD:** `0f5ec3e01e8713443ccfc23c4471df3151f958d2` at start and at end. The lane was not mutated (empty `git status --short`).

**Verdict: GO.** S-1 to S-5 are closed or accepted, and I found no new P0 to P2 defect. Two P3 observations (N-1, N-2) are recorded below. As instructed, I do not re-raise the red per-commit gate on `ee14d941` and `101243b5`.

## Evidence

**Parity guard and boundary checks**
- `python3.13 -I scripts/checks/check_windows_parity.py` at HEAD exits 0: "504 files, 271 listed occurrences (unported 1, paired 166, platform 104); nothing new". This is the S-1 counterexample, which exited 1 at `101243b5`.
- `python3.13 -m unittest scripts/checks/test_check_windows_parity.py` passes (61 tests, OK).
- Also rc=0: `check_cfg_boundaries`, `check_process_globals`, `check_candidate_switches`, `check_spawn_gate_minting`, `check_filesystem_boundary`, `check_checked_artifact_boundaries`.

**kill_tree row**
- Build: `cargo +1.95.0 test --locked --offline --lib kill_tree_ends`, with `CARGO_TARGET_DIR=/Volumes/projects/limbo/build-targets/win-flakes-review-fix`, `TMPDIR=/Volumes/gwztmp`, `RUSTFLAGS='--cfg gwz_transport_candidate'` and `GWZ_TEST_GIT=real`. It finished (1m 21s) and the row passes.
- The array expression `[b"\n", KILL_TREE_READY, b"\n"].concat()` compiles. I had doubted it because `b"\n"` is `&[u8;1]` and `KILL_TREE_READY` is `&[u8]`.
- Direct runs of the test binary with the exact test name: 150 of 150 pass with default threads and 150 of 150 with `RUST_TEST_THREADS=1`. `pgrep` found no stray processes.

**Windows arm type-check**
- The Windows arm of `kill_tree` was copied verbatim into a standalone file in the scratchpad. `rustc +1.95.0 --target x86_64-pc-windows-msvc --emit=metadata` accepts it (rc=0). The full Windows crate was not built, per your instruction.

## Prior-finding closure

| ID | Status | Evidence |
|---|---|---|
| S-1 (P2, parity inventory) | Closed | The diff adds the three `os` rows (`libc::SIGKILL`, `libc::kill`, `libc::pid_t`) to `windows_parity/0.5.json`, `paired`, owner 0.5, appendix G24, beside the file's existing rows. The guard exits 0 at HEAD, and its own unit tests pass. |
| S-2 (P3, `EXIT_BOUND` doc threshold) | Closed | The new text says a waiting command is caught only if its exit begins less than `OPEN_CLEANUP_MS` - `EXIT_BOUND` after the handoff (2.5 s today). A lower `OPEN_CLEANUP_MS` narrows that window, and one at `EXIT_BOUND` closes it. This is the condition I derived: the waiting command's duration is `OPEN_CLEANUP_MS - X`, and it fails the row iff that exceeds `EXIT_BOUND`. The test doc is consistent with it. The code is unchanged. See N-1 for line length. |
| S-3 (P3, silent kill failure) | Closed | The Unix `kill_tree` returns `Err(last_os_error())` when `libc::kill` returns non-zero. The killer asserts it with `.expect("kill_tree could not signal the tree")`, so a failed signal now fails immediately with the errno, not after 120 s. 300 of 300 killer runs on Mac pass, so no spurious `EPERM` or `ESRCH`. Residual: see N-1. |
| S-4 (P3, two-write race) | Closed | The root now writes `"\n" + READY + "\n"` as one `write_all`. std's `LineWriter` with an empty buffer passes the span through the last newline to a single `write(2)`. It is 22 bytes, under `PIPE_BUF`, so it is atomic on the pipe. The leaf's `test <name> ... ` prefix can no longer land between the newline and the ready line. Mac run under `RUST_TEST_THREADS=1`: 150 of 150 pass. |
| S-5 (P3, unmeasured exit margin) | Accepted, not changed | The patch makes no change. The comment still says the exit alone was not timed, which is honest. It stays on the follow-up list as the coordinator recorded. |

## New findings

### N-1 (P3, bounded robustness and diagnosability) — failures of the kill signal are discarded on the validator path

- **Location:** `src/git/endpoint/https_worker/native/tests.rs`, `real_request_validation` timeout branch: `let _ = kill_tree(&child); let _ = child.wait();`.
- **Invariant:** the 30 s bound in `real_request_validation` should bound the test.
- **Reproduction:** make `kill_tree` return `Err`, for example by running the validator without `isolate`. The call still discards the error and goes on to an unbounded `child.wait()`.
- **Impact:** a failed signal leaves the child running and `wait()` blocks with no message. This is pre-existing behaviour (the old helper also swallowed failures), and the timeout is itself a rare path, so I rate it P3 and not a blocker.
- **Correction:** report the error in the panic message. For example, `let killed = kill_tree(&child); let _ = child.wait(); panic!("real SSPI validator fixture exceeded bound (kill_tree: {killed:?})")` is bounded whenever the kill succeeded, and names the cause when it did not. Making the wait itself bounded is optional.
- **Regression test:** a unit test that feeds a never-exiting child that is not group-isolated into the timeout path. This is only practical with a configurable bound, which the file does not have, so I would leave it untested.

### N-2 (P3, Windows-only, rate unmeasured) — a non-zero `taskkill` status is now a hard failure in the killer

- **Location:** the `else` arm of `kill_tree`, `tests.rs` around lines 40-49 of the patched file, and the killer's `.expect(...)`.
- **Detail:** previously `taskkill`'s status was ignored. Now `status()?` (spawn failure) and a non-zero exit both fail the row. `taskkill /T` can exit non-zero when a process in the tree vanishes while it is enumerating.
  - In this row the root and leaf are both sleeping and nothing exits concurrently, so the exposure looks small.
  - The earlier dabeest loops (102 of 102) did not record the status.
  - `taskkill`'s stderr is sent to null, so the failure message carries only the exit code, not the reason.
- **Impact:** a possible new Windows flake at an unmeasured rate. Not demonstrated; the Windows receipt the lane owner is running is the evidence.
- **Correction, if the receipt shows it:** keep the status in the error but capture `taskkill`'s stderr into the message, or tolerate exit 128 ("no running instance") only after the root's `wait()` shows it was ended.
- **Regression test:** the Windows receipt loop on dabeest.

## Out-of-scope observations

- **Doc line length.** `close_tests.rs` line 152 is 140 characters; the rest of that comment block is about 118. rustfmt does not wrap comments and I found no line-length check in the repo, so this is cosmetic.
- **N-1 residual.** Beyond the validator path, `libc::kill` after a reap (PID reuse) is still unguarded. No caller does that today.
- **Not run, as instructed.** I ran no WSL gates and no Windows receipt. I did not run clippy. The test-profile build produced no new warning in the changed lines.
- **Housekeeping.** `/Volumes/projects/limbo/build-targets/win-flakes-review-fix` (my target dir) and the scratch archives and files under the session scratchpad remain.
