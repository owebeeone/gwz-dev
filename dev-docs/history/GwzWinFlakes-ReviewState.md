# Windows flake hardening — STATE-AXIS REVIEW
**Review object:** gwz-core `b0ac8de7..7db78119` (8 test/fixture files under `src/git/endpoint` and `src/transport_host`, +289 −54) and the evidence run `campaigns/transport-qualification/runs/2026-10-30-win-flakes/` in gwz-core-evidence `973168e9`. **Baseline:** gwz-core `b0ac8de7`. **Date:** 2026-10-30 **Axis:** State (races, ordering, lifecycle, fail-closed direction in the test fixtures).
**Verdict: NO-GO** — P0 0, P1 0, P2 1 (S-1), P3 4 (S-2 to S-5). Pre-committed GO on a revision that closes S-1 and records S-2 to S-5 as accepted or fixed.

## 0. Evidence base
- **Tuple.** gwz-core was `7db78119` and gwz-core-evidence was `973168e9` at the start and again at the end. The lane gwz-core was clean. The evidence repo shows only an unrelated untracked `2026-09-22-alpha-setup-timeout/`. I built only in `/Volumes/projects/limbo/build-targets/win-flakes-review` and a throwaway `cp`/`rsync -L` copy outside the lane. I removed the copy afterwards.
- **Focused run on the lane tree.** Command: `RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real cargo +1.95.0 test --lib --locked -- close_tests kex_retry ssh_controlled_server kill_tree`. Result: 34 passed, 0 failed, with no warnings in the touched files. It includes `command_exit_does_not_wait_for_a_close_in_flight`, `a_gated_server_…until_released`, the kex row and the `kill_tree` row.
- **Mutation probes** on a copy of the lane tree, with env-gated edits and 4 min 05 s of build. Results are under S-1 and in the invariant analysis.
- **Evidence cross-checks.**
  - `raw/lane.diff` is identical to `git diff b0ac8de7 7db78119` once `index` lines are ignored.
  - The 118 files in `manifest.json` all hash correctly.
  - The loop counts and failure counts in the README match every `loop.log` and `results.tsv`:

    | Loop | Runs | Failures |
    |---|---|---|
    | original close row | 102 | 1 |
    | fixed close, gated and `kill_tree` rows | 102 + 36 each | 0 |
    | hang candidate | 102 | 0 |
    | kex loops | 204 / 102 / 408 / 1200 / 1200 / 1200 | 1 / 0 / 0 / 2 / 2 / 0 |
  - I recounted the "63 reruns caused by a real WinCNG −8" from the per-run logs (7 + 8 + 17 + 12 + 19 = 63 over 4212 row-runs). It matches.
  - The red logs match the README: `orig-close` run 58 (`(0,1)`, 0.64 s), `red-close` 3/3, `red-killtree` showing the split line, and `red-kex` (`runs` 3, not 2).
  - The 7944 s log excerpt shows `test … ... SUCCESS: The process with PID …` then `ok` on the next line. The lib suite finished at 1335.93 s, then a 103 m 46 s silent compile.
- **Not checked** (queued, out of scope): the WSL matrix and the Windows receipt for this tuple. The dabeest build tree is tied to the commit only by the README and `lane.diff`; there is no recorded build hash.

## Problem, requirement provenance, and scope
The operator authorised "Windows flake hardening" on 2026-10-30: fix the close-timer row and the kex-retry row at their cause, investigate the hang candidate, and check the `agent_login_fixture_pipe.rs` twin. The brief's three items trace to the changes as follows.

- **Close row** → `Ending::Gated`, `release_closes`, `Holdable` gate, `gated_close_server`, `close_tests.rs`, plus a fixture row in `ssh_controlled_server/tests.rs`. In scope. `Ending::Late` and `late_close_server` had no other users; `grep` finds no residue.
- **Kex row** → new `assert_reruns_follow_real_key_exchange_failures`, plus diagnostics in `Scripted`. In scope. The product's `KexRetry` is untouched.
- **Hang candidate** → `kill_tree` now uses `.output()`, plus the new row `kill_tree_ends_the_tree_and_writes_nothing_to_the_test_runs_output`. In scope as the "test or fixture" cause that brief item 3 allows fixing red-first.
- **Drafter claim 4 (editing `https_worker/native/tests.rs`).** I find it necessary and authorised. It is the only place the cause lives: the split line comes from `kill_tree` there. It is not a file another lane owns; the overrides file reserves only `transport_host/mod.rs` construction, `driver_only*.rs`, route selection and the gate-fixes scripts. It is a fourth file beyond the three named flaky files. Item 3 is what authorises it, not item 2 or item 1.
- **Drafter claim 1.** Mostly verified. The original failure was a ~30 ms window, not the 3 s timer: `mac-exp-1.log` shows "lease released after 30.771ms", and the 500 ms injection turns the original row red on dabeest. `Ending::Gated` does hold the close in flight, and the fixture row shows a 1 s `WouldBlock` before release. The word "really" overstates it; see S-1.
- **Drafter claim 2.** Verified. `settled`'s promise is what the row now pins, and the product code is untouched.
- **Drafter claim 3.** Verified, with the unexplained 103-minute compile recorded under S-4.

## 1. Findings

### S-1 (P2): the gated close is not held "for as long as the test wants", and the close row can pass with the property broken
- **Location.** `src/transport_host/close_tests.rs`, `command_exit_does_not_wait_for_a_close_in_flight` (its doc comment, and the 120 s guard). Also the `Ending::Gated` doc in `ssh_controlled_server.rs` ("for as long as the test wants it slow").
- **Invariant.** The row must fail if command exit waits for a close in flight. It must not fail because a timer elapsed while the close was held.
- **Defect.** The product itself ends a held close at its own 5 s cleanup bound.
  - `ssh_adapter/worker.rs` sets `close_deadline = now + cleanup_timeout_ms` and then marks the connection `Broken`.
  - The row's only deadline is the 120 s guard, so a product that waits out the 5 s bound and then discards finishes in about 5 s and passes.
  - The old row failed at ≥1.5 s (`started.elapsed() < delay/2`), so this is a loss of strength.
  - Separately, the `(leased, idle) == (1, 0)` window is now about 5 s, not "nothing but the test ends it". It was 30 ms before, so it is much smaller, but it is not zero.
- **Reproduction** (copy of the lane tree; env-gated edits):

  | Probe | Outcome |
  |---|---|
  | `sleep 4000 ms` before the count read | passes |
  | `sleep 6000 ms` before the count read | `(0, 0)`, fails |
  | stand-in for a product that waits ~4 s (`sleep 4000 ms` before `finish()`) | **passes** |
  | pool `cleanup_timeout_ms = 600000`, with the 6 s sleep | still `(0, 0)` |

  The last probe shows that raising the pool's cleanup is not a fix. The per-open `OPEN_CLEANUP_MS = 5_000` (and `CLEANUP = 5 s` in `session.rs`) is hard-coded. The drafter's own injection test used 4 s, just under the bound (`mac-inject-fixed-2.log`). The 6 s case, which fails, is not in the evidence.
- **Impact.** A realistic regression ("shutdown drains the in-flight close up to its cleanup deadline") is not detected. The "no runner's load can end it" claim in the doc comment is false for loads above 5 s.
- **Correction.**
  1. Restore a bound that is tied to the product's 5 s, such as `recv_timeout` at about 2.5 s measured from the `close()` handoff. It is one-sided: a healthy exit takes milliseconds, and the dabeest whole-process maxima were 1.3 to 1.6 s.
  2. Or add a deterministic product-side observation of "waited on close".
  3. Correct the "as long as the test wants" and "nothing but the test ends it" wording either way, and record the residual 5 s read window.
- **Closure test.** With the stand-in `sleep 4000 ms` before `finish()`, the row must go red. With a 4 s sleep before the count read it may stay green, and with a 6 s sleep it must stay red with the documented message.

### S-2 (P3): `kill_tree` with `.output()` creates pipes where `Stdio::null()` would do
- **Location.** `https_worker/native/tests.rs`, both `kill_tree` variants.
- **Defect.** The fix captures the killer's output only to discard it. `.output()` drains both pipes concurrently, so there is no deadlock on large output; exit semantics are unchanged because the result is ignored, and the tree is still killed.
  - `.output()` does wait for pipe EOF. On Windows, a concurrently spawned child in another test thread can inherit the pipe's write handle, which would make `kill_tree` block until that child exits. This is a hypothesis; I did not reproduce it, and the path runs only on the 30 s validator timeout.
  - The cfg structure is correct: the Windows and Unix variants stay inside `cfg_if::cfg_if!`.
- **Correction.** `.stdout(Stdio::null()).stderr(Stdio::null()).status()`. The new row is unchanged in meaning.
- **Closure test.** The existing new row stays green on Unix, and `red-killtree` stays red against the original on dabeest.

### S-3 (P3): the kex row still fails on non-kex errors under heavy load, and the original row's baseline was not measured
- **Evidence.** Under 1.5× oversubscription (12 wide, 24 burners), the pre-hardening build had 4 failures in 2400 runs, in `loop-kex-nocapture-3` and `-4`.
  - Two were `.expect("the last run's real handshake completes")` or a stand-in run ending early on `Uncategorized` or `Other`.
  - Two were the unchanged third scenario ending early.
  - The final build had 0 in 1200, which is not significant at a rate of about 0.17%.
- **Defect.** The README says "the old row has the same exposure", but no loop of the original kex row at that load exists. The new row also adds about six connections to the fixture sshd (two cases of 2 and 3 runs) in place of the old four. The `expect` asserts environmental health, which is beyond `settled`'s promise.
- **Closure.** Either record the exposure as accepted with a measured baseline of the original row at the same load, or drop the completion `expect` for a run whose cause is not a kex failure.
- **Not a regression of the hosted failure.** The −8 class itself is absorbed in every one of the 63 observed cases.

### S-4 (P3): the 103-minute compile in the 7944 s receipt is unexplained
- The brief asked to root-cause the hang. The "hang" is shown to be a display artefact (the split line) plus a silent compile, but the compile itself is neither reproduced nor attributed. The README admits this. A contention hypothesis is plausible and untested.
- **Closure.** Keep it on the follow-up list; the Windows receipt watchdog (`b0ac8de7`) bounds the stall.

### S-5 (P3): diagnosability of the close-row guard and one test-name claim
- If the spawned thread panics (for example, `pending_local_work != 0`), `done` is dropped, `recv_timeout` returns `Disconnected`, and the assertion reports "the command's exit waited for a close that was held in flight". That misattributes a different failure. The original panic text still prints.
- `kill_tree_ends_the_tree_and_writes_nothing_to_the_test_runs_output` kills only a root with no descendants, so the "tree" in its name is not exercised. That coverage is unchanged by this lane.
- **Closure.** Distinguish `Timeout` from `Disconnected`, and rename the row or add a grandchild.

## 2. Invariant analysis
- **Gate lifecycle.**
  - `release_closes` uses `send_replace(true)`, so it is idempotent.
  - The `wait_for` result is consumed by `.is_err()` before any later await, so no watch `Ref` is held across an await (the future is `Send` and the build is green).
  - A sender dropped while a task waits returns without closing.
  - `ControlledSshd::drop` stops and joins the server thread, which cancels per-connection tasks (`kill_on_drop(true)` on the child), so a never-released gate leaks no thread or socket.
  - Opening the gate before the hold is set makes the hold inert, which is intended.
- **Ordering.** russh 0.63.3 reads one packet per `reading` future, so the client's EOF packet is processed (setting `hold`) before the next read starts and the CLOSE is never read first. A wakeup on release comes from the gate task's `handle.*` message, which re-polls `reading`. I found no lost wake in the exercised path.
- **Latent:** a held connection with outbound data blocked on the client's window would not read the window adjustment. This is the same as `Stuck`, and the rows send tiny output.
- **Globals.** The new shared state is a per-server `watch` channel, not a global or thread-local. The new test's env var is passed only to child processes.
- **Other users of the removed `Ending::Late`:** none.
- **Kex row non-vacuity.** Probes (copy of the lane tree; `settled` mutated by env):

  | Mutation of `settled` | Row outcome |
  |---|---|
  | never rerun | fails ("a run that ends on its stand-in is made again") |
  | capped at 2 runs | fails (the same assertion, on the `stand_ins = 2` case) |
  | one extra run after a clean run | fails ("a run is made again only after a real −8") |
  | unmodified | passes |

  The regressions it catches are `settled` not rerunning on a real kex failure, stopping early on the second, and rerunning a clean run. A rerun on any error is caught by the unchanged scenario that expects `runs == 1` for code −5. The `TRIES` bound is pinned by the unchanged third scenario (`runs == TRIES`). The `assert_eq!(counts.real_kex(), ends[runs-1])` check is harmless. Product-level over-retry is not pinned by this row, as before.
- **`kill_tree`.** Windows-only logic stays inside `cfg_if!`. The `taskkill`-output split is reproduced red on dabeest and green after the fix. The new row cannot fail on Unix, because `/bin/kill` is silent, so on Mac it is a smoke test of the harness only.
- **Repository rules.** Braces and `cfg_if!` conventions are followed, and no `#[cfg]` is attached to an unbraced item. `kex_retry_tests.rs` is now 910 lines against the 1000-line review threshold.

## 3. Risks and next action
- Revise S-1 (bound plus corrected wording). Address S-2 (mechanical) and S-3 (baseline or drop the `expect`) when convenient. Both S-4 and S-5 may be recorded as accepted.
- Rerun the close row's dabeest loop (at least 100 runs) and the Mac injection probes after the change, then queue the Windows receipt under the existing labels (`win-flakes-20261030-a`, `win-flakes-remote-20261030-a`). Do not claim the close row immune to load until S-1's window is documented.

## 4. Out-of-scope observations
- The windows `agent_login_fixture_pipe.rs` twin is reported only. `raw/dabeest/twin-race/twin-race.txt` shows 300/300 `ConnectionAborted` reads, so the Unix queued-bytes race has no Windows equivalent. I found no counter-evidence.
- The product hard-codes a 5 s per-open cleanup (`OPEN_CLEANUP_MS` in `session/driver.rs`) that the pool's `cleanup_timeout_ms` does not govern for this route. That makes a row's bounded-wait detection depend on a constant the test cannot configure.
- The dabeest scratch tree `E:/gwz-tests/wflk-loop-20261030-a` is left for the lane owner to remove, per the README.
