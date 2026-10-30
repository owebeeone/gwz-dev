# State-axis review: win-flakes gwz-core `ee14d941` + `101243b5` on `298b0783`

- **Object:** gwz-core `298b0783..101243b5` in lane `/Volumes/projects/limbo/gwz-dev-win-flakes`. It touches `src/git/endpoint/https_worker/native/tests.rs` and `src/transport_host/close_tests.rs`.
- **Date:** 2026-10-31
- **Axis:** State (process lifetime, signals, races, failure direction, platform parity)
- **HEAD:** `101243b5c4e706ff2d5f678c1278565c63495ac7` at start and at end. The lane was not mutated; the lane's `git status --short` printed nothing.

**Verdict: NO-GO**

Reason: finding S-1 (P2) is a deterministic CI failure. The `check_windows_parity.py` step in `checked-artifact-boundary.yml` fails on `ee14d941`. It passes at `298b0783`. The product-side kill fix itself is sound.

## Evidence

Commands run (all read-only on the lane):

```
git -C <lane>/gwz-core diff 298b0783 101243b5          # 2 files, +15 -5
grep -nE "kill|procps|root was not ended|gwz-kill-tree" ci-b31222bd-candidate.log
WebFetch https://gitlab.com/procps-ng/procps/-/issues/354
grep -rnE 'bin/kill|kill -|"kill"|pkill|killpg|libc::kill|taskkill' --include=*.rs/py/sh/zsh/yml/yaml/toml over the whole lane
python3.13 -I scripts/checks/check_windows_parity.py   # at HEAD: rc=1; at 298b0783 (git archive to scratch): rc=0
check_cfg_boundaries, check_process_globals, check_candidate_switches, check_spawn_gate_minting,
  check_filesystem_boundary, check_carrier_peer_rule, check_checked_artifact_boundaries   # all rc=0
```

I also ran a fix-up experiment on a `git archive 101243b5` copy in the scratchpad: three rows added to `windows_parity/0.5.json`. The parity check then passes (rc=0, "nothing new").

Test runs, with `CARGO_TARGET_DIR=/Volumes/projects/limbo/build-targets/win-flakes-review-fix` (a `cp -Rc` clone of the warm win-flakes target), `TMPDIR=/Volumes/gwztmp`, `RUSTFLAGS=--cfg gwz_transport_candidate`, `GWZ_TEST_GIT=real`, and `cargo +1.95.0 test --locked --offline --lib kill_tree_ends`:

- The row passes.
- 150 of 150 direct runs of the test binary pass with default threads.
- 150 of 150 pass with `RUST_TEST_THREADS=1`.
- `pgrep` found no stray root or leaf processes afterwards.

### Diagnosis against the CI log

- **Timeline.** The CI log (`ci-b31222bd-candidate.log`) shows the row "has been running for over 60 seconds" at 13:58:32. The lib run reports `finished in 120.01s` for the killer. The killer's `wait()` returned success, because `tests.rs:238` ("the tree's root was not ended") fired. The root's `sleep(120)` therefore ended normally, so no signal ever reached the root. This fits a kill that did nothing.
- **procps issue #354.** It exists and matches the stated mechanism: after a signal option, a negative PID is mis-parsed, and `pid = '0' - optopt` keeps only the first digit. `kill -SIGTERM -432` signals group 4. The page names neither the buggy nor the fixed release; the "4.0.4 / 4.0.5" attribution in the comment and commit message is not verifiable from it, but nothing contradicts it. The page does say 3.3.17 (Ubuntu 22.04) behaves correctly. A PID starting with 1 gives `kill(-1, SIGKILL)`, so the old code was also a real hazard on the runner, not only a flake.
- **Alternatives the fix would not cover.**
  - `/bin/kill` missing or failing to spawn is covered, because the old code discarded the `status()` error and the new code never spawns.
  - Root not yet in its own group is excluded: `isolate` runs before `spawn`, and the ready line is written only after the leaf is spawned, so the leaf is already in the group.
  - A `setsid` descendant escapes any group kill, but this tree has none.

  I found no alternative cause that `libc::kill(-pid)` leaves open.

### Wrong-group analysis for `libc::kill(-(child.id() as pid_t), SIGKILL)`

- **PID casts.** Linux PIDs are at most 2^22 and macOS at most 99999, so the `u32` to `i32` cast cannot overflow. The negation of a nonzero `i32` cannot wrap. `child.id()` is never 0 (kill of own group) or 1 (kill of all), and a child cannot be PID 1.
- **Both callers isolate.** `isolate` is applied before `spawn` at the killer (`tests.rs:233`) and at `real_request_validation` (`tests.rs:64`).
- **PID reuse is closed.** Both callers call `kill_tree` before `wait()` and neither reaps first.
  - Killer: no reap happens before `kill_tree` at line 245.
  - Validator timeout path: `kill_tree` runs only after `try_wait()` returned `None`.
  - A zombie leader keeps its PID, and a live group keeps its PGID, so the number cannot be recycled in that window.
- **Without `isolate`, the failure is safe.** The child's pgid is its parent's, so no group named by the child's PID exists. The call returns `ESRCH` and the test's own group is never hit.
- **Real validator timeout path.** It spawns `cargo test` in its own group, so rustc and the test binary are in the group too. This is the same shape as `https_auth/process_tree.rs`.

### Windows branch

The `else` arm of the `cfg_if!` is byte-identical apart from a comment rewording ("The killer's own output" became "`taskkill`'s own output"). `isolate` is a no-op and `kill_tree` is still `taskkill /T /F /PID`. There is no behaviour change on Windows.

### Does "\n" alter the driver's noise filter?

No. The root's stdout is a pipe held by the killer, and the leaf inherits that pipe. The driver only sees the killer's own stdout and stderr. Its filter already tolerates empty lines. On Windows the leading empty line is skipped by the read loop, and `trim_ascii_end` handles CRLF.

### Does anything else still call `/bin/kill` with a negative PID?

No hit in src, tests, scripts or the other lane members. The remaining `kill` uses all take positive PIDs, or the pgid is signalled through `os.killpg` in Python:

- `ssh_fixture.rs:563,602` (`kill -CONT/-STOP <pid>`)
- shell fixtures (`kill -9 "$pid"`, `kill "$reader"`)
- `kill -0` probes in `key_fixture.rs` and `password_helpers.rs`
- Python `os.killpg` in `gwz-py` and `retained_readers`

### 101243b5 numbers against the README

- "At most 4.7 s in 102 runs with 18 burners" matches `rem1-loop-close` (max 4657 ms, whole process).
- "2.0 s in 600 with 24" matches `rem1-loop-close-heavy` (max 1992 ms, 600/600).
- "Whole process, fixture setup included" matches the README's "whole process".

The numbers are correct. The new sentence on `OPEN_CLEANUP_MS` is not (S-2).

## Findings

### S-1 (P2): the Windows-parity guard fails on the new `libc::kill` in `native/tests.rs`

- **Location:** `src/git/endpoint/https_worker/native/tests.rs:33` (from `ee14d941`). The inventory is `scripts/checks/windows_parity/0.5.json`, which holds the rows for this file.
- **Invariant:** every Unix-only OS call under the transport scope has an inventory row (Windows-parity plan step 0.3). CI step "Check complete entry and call graph" runs `python scripts/checks/check_windows_parity.py`.
- **Reproduction:**
  ```
  cd gwz-core && python3.13 -I scripts/checks/check_windows_parity.py
  ```
  At HEAD the output is:
  ```
  NEW os libc::SIGKILL at src/git/endpoint/https_worker/native/tests.rs:33
  NEW os libc::kill    at ...:33
  NEW os libc::pid_t   at ...:33
  ```
  This exits 1. The same command on a `git archive 298b0783` copy exits 0.
- **Impact:** the checked-artifact-boundary workflow goes red on the commit that fixes the hosted-Ubuntu failure. The lane would trade one red CI job for another.
- **Correction:** add three `os` rows to `windows_parity/0.5.json` beside the existing `os::unix::process::CommandExt` row for this file. Each is `"state": "paired", "owner": ["0.5"], "appendix": "G24"`, for `libc::SIGKILL`, `libc::kill` and `libc::pid_t`. The `else` arm of the same `cfg_if!` is the Windows pair (`taskkill`). I applied exactly this to a scratch copy, and the check then passed.
- **Regression test:** the checker itself. Run `check_windows_parity.py` in the lane's pre-push gate (`run_boundary_job.py` or `check_lane_commits.sh`), since the lane's Mac test run does not exercise it.

### S-2 (P3): the new `EXIT_BOUND` doc states the wrong threshold

- **Location:** `src/transport_host/close_tests.rs:150-151` (`101243b5`): "if that falls below twice this bound, a command that waits for the close passes".
- **Invariant:** the comment must state the real blind-spot condition.
- **Why it is wrong:**
  - A command that waits for the close finishes at about `OPEN_CLEANUP_MS − X`, where X is how long after the handoff the exit began.
  - The row misses it only when that is at most `EXIT_BOUND`, that is `OPEN_CLEANUP_MS ≤ EXIT_BOUND + X`. At X ≈ 0 that is `OPEN_CLEANUP_MS ≤ 2.5 s`, not below 5 s.
  - With `OPEN_CLEANUP_MS = 4 s` and a prompt exit, a waiting command takes 4 s and the row still fails it correctly.
  - The test's own doc says a waiting command is caught "provided its exit began less than `EXIT_BOUND` after the handoff". That equals `OPEN_CLEANUP_MS − EXIT_BOUND` only because 5 s = 2 × 2.5 s, so the two sentences agree only at today's constants.
- **Impact:** a maintainer who changes `OPEN_CLEANUP_MS` gets a wrong rule: they either over-tighten the bound or miss the real cliff.
- **Correction:** state the condition as `OPEN_CLEANUP_MS − X > EXIT_BOUND` (so X < `OPEN_CLEANUP_MS − EXIT_BOUND`), and phrase the test doc's "provided" clause the same way.
- **Regression test:** the guard that `101243b5` leaves on the follow-up list. For example, a `const _: () = assert!(OPEN_CLEANUP_MS >= 2 * EXIT_BOUND_MS)` needs the product constant to be `pub(crate)`. That then ties the two values mechanically.

### S-3 (P3): the failure of `kill_tree` is invisible, which cost the diagnosis campaign

- **Location:** `tests.rs:29-34` and `tests.rs:245-246`.
- **Invariant:** a test helper whose job is to end a tree should say when the signal was not delivered.
- **Why it matters:** `libc::kill` returns -1 on `ESRCH` or `EPERM`, and the helper discards the return value. In the CI incident the old helper discarded its `status()` result in the same way. The only symptom was "the tree's root was not ended" after 120 s, with no cause. The product code in `process_tree.rs` ignores the result too, which is acceptable there, but not for a test's killer.
- **Reproduction:** run the killer with a `kill_tree` that no-ops or targets a group that does not exist. It still takes 120 s with a message that does not distinguish "signal failed" from "signal ignored".
- **Impact:** diagnosability only. The old and new code behave the same on success.
- **Correction:** have the Unix `kill_tree` return `std::io::Result<()>` from `libc::kill` and `last_os_error`. The killer asserts it with `.unwrap()` or a message; `real_request_validation` may ignore it.
- **Regression test:** the existing row, which now fails at once with the errno instead of after 120 s.

### S-4 (P3): the ready line is written as two writes, so the glue fix has a residual race

- **Location:** `tests.rs:223-226`.
- **Detail:** the new code does `write_all(b"\n")` and then `write_all(READY)` and `write_all(b"\n")`.
  - With `RUST_TEST_THREADS=1`, the leaf, already spawned, prints its own `test <name> ... ` prefix as a partial line on the shared pipe.
  - If that write lands between the root's `"\n"` and the ready line, the pipe holds `test <name> ... gwz-kill-tree-ready`, which does not match, and the killer then blocks until EOF.
  - The window is microseconds against a leaf start-up of milliseconds, and 150 of 150 runs with `RUST_TEST_THREADS=1` passed, so this is a robustness point only.
- **Correction:** a single `write_all(b"\ngwz-kill-tree-ready\n")`. `LineWriter` hands the buffer containing the last newline to `write(2)` in one call.
- **Regression test:** the existing row under `RUST_TEST_THREADS=1`. An interleaving race has no deterministic reproduction.

### S-5 (P3, evidence): the `EXIT_BOUND` margin is not measured

- **Location:** `close_tests.rs:153-154`.
- **Detail:** `101243b5` now discloses that only the whole process was timed, at most 4.7 s. The exit alone, which is what the 2.5 s bound applies to, was not timed. The sentence "a healthy exit takes milliseconds" therefore rests on no measurement.
- **Impact:** if the exit were a large share of that 4.7 s, the 2.5 s bound would be close to flaking under load on a slow host.
- **Correction:** record the exit's elapsed time in the pass path (for example, in the failure and success messages) and run it under the 18-burner loop.

## Out-of-scope observations

- **`libc::kill` after reap.** The helper has no guard against a call after `wait()`, unlike `Child::kill`, which refuses once the child is reaped. Both current callers call it before any reap, so there is no defect today. A future caller that reaps first could signal a recycled PGID.
- **Duplicated `unsafe` kill-group snippet.** It now exists in two places (`process_tree.rs:84` and `tests.rs:33`). A test could reuse the product's `ProcessTree` instead, but the lane chose to mirror it. The Windows-parity inventory will track both once S-1 is fixed.
- **Comment version claims.** The "4.0.4 / fixed in 4.0.5" attribution in the comment is not confirmable from the issue page. The issue only says 3.3.17 is correct. This is cosmetic.
- **Review artefacts.** I created `/Volumes/projects/limbo/build-targets/win-flakes-review-fix` (a clone of the warm target, as allowed) and scratch archives under the session scratchpad. No remote host was touched.
