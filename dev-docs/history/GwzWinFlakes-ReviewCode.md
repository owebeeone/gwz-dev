# Windows flake hardening — CODE-AXIS REVIEW

**Review object:** gwz-core `298b0783` (`b0ac8de7..298b0783`, 8 test/fixture files under `src/git/endpoint` and `src/transport_host`, +340/−54) and gwz-core-evidence `f9f67799` (`campaigns/transport-qualification/runs/2026-10-30-win-flakes/`, 175 files)
**Baseline:** gwz-core `b0ac8de7`; brief `/Volumes/projects/limbo/build-targets/tools/brief-win-flakes.md`; background `/Volumes/projects/limbo/build-targets/tools/findings-2026-10-30-windows-flakes.md` (first section only)
**Date:** 2026-10-30 **Axis:** Code (peer-blind second axis, escalated after the State-axis P2). I did not read the State review or the RemPlan.
**Verdict: GO** — 0 P0, 0 P1, 0 P2, 5 P3. I found no open P0, P1 or P2. The P3s are bounded robustness, maintainability and accuracy items. I do not pre-commit to anything further.

## 0. Evidence base

**Tuple.** The tuple was unmoved at the start and at the end of the review. gwz-core was `298b0783a75e…`, clean tree. gwz-core-evidence was `f9f67799ce88…`. The only untracked item in the evidence repo is an unrelated `2026-09-22-alpha-setup-timeout/` directory.

**Read:**
- The full diff, 675 lines.
- The final files `ssh_controlled_server.rs`, `handler.rs`, `holdable.rs`, `ssh_controlled_server/tests.rs`, `close_tests.rs` and `kex_retry_tests.rs`.
- The product files `kex_retry.rs`, `observed.rs`, `session/driver.rs`, `request.rs` and `transport_host/mod.rs`.
- The Windows twin `ssh_tests/agent_login_fixture_pipe.rs`.
- The evidence README and manifest.
- The evidence logs `mac-exp-1.log`, `mac-mutation-1.log`, `p1-capacity-20261030-d-diagnosis-lines.txt` and the gzipped receipt log.

**Commands run** (Mac, rustc 1.95.0, `--cfg gwz_transport_candidate`, `GWZ_TEST_GIT=real`, `CARGO_TARGET_DIR=…/build-targets/win-flakes-review-code`, `TMPDIR=/Volumes/gwztmp`):
- Focused run of `close_tests kex_retry ssh_controlled_server kill_tree` from the lane: **34 passed, 0 failed**. None of the compiler warnings are in the changed files.
- The `kill_tree` row run alone with `RUST_TEST_THREADS=1`: it **fails after 120.0 s** (see P3-1).
- Mutation probes in a copy outside the lane (`cp -RL`, since deleted; the lane was never written):
  - A 4 s stand-in wait before `finish()` makes the close row fail with the "waited for the close" message.
  - An `OPEN_CLEANUP_MS` window probe reads the lease for 5.0066 s with the gate held.
  - A sleep sweep in `finish()` at 2.0, 2.4, 2.6, 4.0 and 4.8 s.
  - 20 repetitions of the close row on the lane build under Mac load of about 60: 0 failures.
- Evidence checks, all from the committed files:
  - **Manifest:** all 174 listed hashes match and no file is unlisted.
  - **Loop counts:** every `results.tsv` row count and failure count matches the README table and prose.
  - **Absorbed reruns:** parsing the 5 `loop-kex-nocapture*` run logs gives 7+8+17+12+19 = **63** extra runs over 4212 row-runs, exactly as claimed.
  - **Failing run logs:** I read them: `run-103`, `-610`, `-932`, `-395`, `-895` and `-536`.
  - **Receipt split line:** receipt lines 2796–2797 show the split `… ... SUCCESS: The process with PID … terminated.` followed by `ok`. The lib suite finished in 1335.93 s, and the later integration build took 103m 46s.
  - **Red runs:** the red logs for `red-close`, `red-kex` and `rem1-red-killtree` show the stated failures.

**Not run (out of scope):** dabeest, WSL matrix, Windows receipt, network.

## Problem, requirement provenance, and scope

**Problem and provenance.** The operator go (2026-10-30) and the brief give three items, each to be fixed at its cause with no retries, sleeps or skips:
1. The close-timer row.
2. The kex-retry row.
3. The hang candidate, with a report-only check of the `agent_login_fixture_pipe.rs` twin.

**Trace of each change to the brief:**
- **Close row** (`close_tests.rs`, `ssh_close_fixture.rs`, `ssh_controlled_server{,/handler,/holdable,/tests}.rs`): this is brief item 1, "replace the timer with a gate". Touching the fixture is necessary, because the timer lived in `Ending::Late`.
- **Kex row** (`kex_retry_tests.rs`): this is brief item 2. The `eprintln!` diagnostics in `Scripted::connect/handshake` go slightly beyond the row. They are test-only, shared by the other kex rows, and print only on failure or with `--nocapture`. I treat this as in scope and not as a finding.
- **`kill_tree` and its new row** (`https_worker/native/tests.rs`, claim 4): brief item 3 says "if the cause is in the test or fixture, fix it red-first". `kill_tree` is a test helper, so I judge the edit **necessary and authorized**. The cause is verified (see claim 3 below), no product code is touched, and the file is inside the brief's test/fixture boundary, though not one of its named items. Without the edit the investigation would end in a report only.
- **Twin check:** report-only, and no change was made. This complies.
- **Product code:** no product file is in the diff. `kex_retry.rs` and `observed.rs` are untouched, so claim 2 (the product's `KexRetry` is untouched) holds. The three fixture modules are `cfg(test)`-only, declared in `endpoint/mod.rs` inside `cfg_if!{ if #[cfg(test)] … }`.

**Drafter claims:**
- **(1) Verified.**
  - Evidence: `mac-exp-1.log` shows the lease released after 30.771 ms with `idle:1, leased:0`.
  - My `PROBE_POLL` run shows that with `Ending::Gated` the lease is held for 5.0066 s, then `(0,0,0,0)`. That is the product's `OPEN_CLEANUP_MS` window.
  - Conclusion: the original failure was a ~30 ms window, and `Gated` really holds the close in flight.
- **(2) Verified**, with the caveat in P3-3.
- **(3) Verified.** Receipt lines 2796–2797 are a libtest line split by `taskkill`'s output on the inherited stdout. The ~103-minute silent integration build is documented and unexplained, but is not in the object (README Limitations).
- **(4) Verified** as above.

**Repository rules:**
- **Braces:** all added control flow is braced.
- **`cfg_if` / conditional compilation:** the `kill_tree` Unix/Windows split is still a `cfg_if!` block, the new row is inside the existing `cfg_if!{ if #[cfg(test)] … }`, and no bare `#[cfg]` was added.
- **Globals and thread-locals:** none were added. The gate is per-`ControlledSshd` instance state (`watch` channel).
- **Production code under `tests/`:** none.
- **File sizes:** the largest changed file is `kex_retry_tests.rs` at 905 lines, under the 1000-line review threshold.
- **Dead code:** `Ending::Late` and `late_close_server` are fully removed. A workspace-wide grep finds no remaining reference outside historical evidence.

## 1. Findings

No P0, P1 or P2. Five P3 findings.

**P3-1: the `kill_tree` row hard-fails after 120 s when libtest runs single-threaded.**
- **Location:** `https_worker/native/tests.rs`, the `Ok("root")` arm (writes `KILL_TREE_READY`) and the `"killer"` arm (loop that matches `line.trim_ascii_end() == KILL_TREE_READY`).
- **Invariant:** the killer must see the ready line before it kills.
- **Reproduction:** `RUST_TEST_THREADS=1 <test binary> --exact git::endpoint::https_worker::native::tests::kill_tree_ends_the_tree_and_writes_nothing_to_the_test_runs_output`. The result is FAILED after 120.0 s. With one libtest thread (which also covers a 1-CPU host), the pretty formatter prints `test <name> ... ` before the test runs. That prefix is glued to the ready line, so it never equals `READY`. The killer then reads until the root ends at its 120 s sleep and fails with "the root ended before it started its child".
- **Impact:** the row is deterministic with default threading. No runner in the lane sets `RUST_TEST_THREADS`; I grepped `scripts/` and `.github/`. It is latent, and it fails closed but slowly and with a misleading message.
- **Correction:** make the root write `\n` before `READY`, or match `ends_with(READY)`, or pass an explicit thread count to the child roles.
- **Closure test:** the same command with `RUST_TEST_THREADS=1` passes in under 5 s.

**P3-2: `EXIT_BOUND` is a literal not tied to the product constant, and its empirical comment is stale.**
- **Location:** `close_tests.rs`, `EXIT_BOUND = Duration::from_millis(2_500)` and its doc comment.
- **Invariant:** the bound should hold only while the product's own bound is larger than it. The row is meant to pass only if the exit does not wait.
- **Detail:**
  - `OPEN_CLEANUP_MS` (5 s) is private to `session/driver.rs`. The comment says "half of that", but no compile-time or run-time link exists.
  - If `OPEN_CLEANUP_MS` is lowered below 2.5 s, a command that waits for the close ends at the product's bound, which is inside the 2.5 s limit, and the row passes with the property broken. The count read still sees `(1,0)` inside the shorter window, so nothing else would flag it.
  - The comment says the whole test took "1.6 s at most in 102 runs". The README's own round-1 loop `rem1-loop-close` records a 4657 ms maximum for the whole process. The load is the same 18-burner load.
- **Effect:** I confirmed the bound bites as documented. A sleep in `finish()` of 2000 or 2400 ms passes, and 2600, 4000 and 4800 ms fail. The documented residual is correct: a stall of 2.5 s or more between handoff and exit lets a waiting command pass.
- **Correction:** derive the bound from a named `pub(crate)` constant, or add a guard that fails when `OPEN_CLEANUP_MS / 2 < EXIT_BOUND`. Replace the stale 1.6 s figure with the measured range.
- **Closure test:** lowering `OPEN_CLEANUP_MS` fails a compile-time or unit guard.

**P3-3: the kex row's last-run real handshake is kept but no longer asserted, so it only adds flake exposure.**
- **Location:** `kex_retry_tests.rs`, `assert_reruns_follow_real_key_exchange_failures` and the row's unchanged third scenario.
- **Detail:**
  - The row no longer asserts that the final real handshake completes (round 1 removed that on purpose).
  - The evidence shows 5 residual failures, all from real connect or handshake steps under 1.5× oversubscription: 2 in 1200, 2 in 1200, 0 in 1200, and 1 in 1200 on the round-1 tree. Run 536 is `a real connect ended with Uncategorized` in the unchanged "fails every real handshake" scenario, which ends in `assert_eq!(runs, TRIES)` with 2 runs. Run 932 ended on its stand-in with `Other: other error` and `Uncategorized: uncategorized error`, and was caught by `runs > stand_ins`.
  - Normal-load loops show 0 failures in 102.
- **Impact:** the hosted failure is fixed, since the 63 real WinCNG reruns were all absorbed. But the row still has a small residual flake rate from a step it no longer asserts on.
- **Correction:** make the post-stand-in run also a deterministic stand-in that ends on a non-kex code, so no real network step remains. The README records the same class as finding F-1.
- **Closure test:** 1200 runs at 24 burners with 0 failures.

**P3-4: assertions 3 and 4 of the new helper and the final `assert_eq` are tautologies of `settled`'s own loop.**
- **Location:** `kex_retry_tests.rs`, the assertions on `ends[..runs-1] > 0`, `ends[runs-1] == 0 || runs == TRIES` and `counts.real_kex() == ends[runs-1]`.
- **Detail:** `ends` is read from the same counter that `settled` decides on, so against the current helper these can never fail. They catch only a change to `settled` or to the `Counts` plumbing: mutations that never rerun, cap the runs, or add an extra run. The README reports those three caught, and by reasoning the "rerun on every run" case is caught as well.
- **What the row is:** it pins the test helper, not the product. A change to the product's retry count (for example `Times(1)` allowing two retries) is not caught here, because each run's `ends` come out identical. The product's retry bound is pinned by the neighbouring rows, which all passed.
- **Impact:** this is fine for its stated purpose, as long as the row name and comments keep saying it is the helper's promise, which they do.
- **Correction:** none required.

**P3-5: the commit message and one README statement contradict the final tree.**
- **Location:** the `298b0783` message and the README.
- **Detail:**
  - The commit message says "The row has only a 120 s hang guard". Round 1 replaced that with `EXIT_BOUND`.
  - It says "kill_tree now captures it". Round 1 changed that to the null device.
  - It says "0 failures in 1608 runs on the final tree". The README's own round-1 final-tree heavy loop `rem1-loop-kex-heavy` has 1 failure in 1200.
  - The README sums the five `loop-kex-nocapture*` loops (4212 row-runs) correctly, but the commit's 1608 matches none of the loop groupings.
- **Impact:** a later auditor reading the commit alone would get the wrong picture of the fixture and the evidence.
- **Correction:** none to the tree. The README is accurate on these points; the commit message cannot be amended without a mutation.

## 2. Invariant analysis

**Fixture API (`Ending::Late(Duration)` → `Ending::Gated`, `release_closes`, `Holdable` gate).**
- **Call sites:**
  - `gated_close_server()` is called only by `close_tests.rs`.
  - `Ending::Gated` is used by `ssh_controlled_server.rs`, `handler.rs`, and the new fixture unit test.
  - Nothing else depended on `Late`.
- **Other `Ending`s unaffected:** `Holdable::new(.., release)` receives `Some(gate)` only for `Gated`, so `Stuck` and `Prompt` behave exactly as before. `held()` equals the old `hold` read when `release` is `None`. The `Client` is handed a gate receiver for every ending but reads it only in the `Gated` arm. That is harmless, and the extra field is not dead code.
- **Lifetime and safety:**
  - A gated close can leak nothing past the test. `ControlledSshd::drop` sends stop and joins. The server thread then runs `shutdown_timeout(2 s)`, which drops the pending `wait_for` tasks. The `Sender` is dropped afterwards, so `wait_for(..).is_err()` returns the task cleanly.
  - There is no double release. `release_closes` is idempotent (`send_replace(true)`) and the gate never closes again.
  - The `watch::Ref` returned by `wait_for` is dropped at the end of the `if` condition, before the following `.await`. It is not held across an await, so `send_replace` cannot block on it.
- **Wake-up is correct but implicit.** `poll_read` returns `Pending` with no waker registered while held. Reads resume because the gate task's `exit_status_request` and `close` wake the session loop, which re-polls the read. This is documented in the `holdable.rs` comment, and the new fixture row pins that the release produces the close.
- **Naming versus behaviour:** the docs state that the whole connection is unread from the first channel EOF until the gate opens. The name `Gated` describes the close, which is what the test sees. Not a finding.

**Close row.**
- **Still pins the product property.** The command's exit (`request.finish()` plus `runtime.shutdown()`) must complete within `EXIT_BOUND` while the close is held in flight.
- **Cannot pass with the property broken** while the product's own cleanup bound (5 s) is larger than `EXIT_BOUND`, and while the stall before the exit is under 2.5 s. P3-2 records the first condition. The second is documented in the test's comments and README.
- **Could it fail on a healthy product?**
  - The count window is now about 5 s (it was 30 ms). A stall past that fails on `(0,0)`.
  - A healthy exit takes milliseconds, and 20 reps under Mac load of about 60 had no failure.
  - The README's round-1 loops (102 under 18 burners, 600 under 24) had none.
  - The residual risk is a very large stall. The 4657 ms whole-process maximum is not decomposed into exit time.
- **Failure behaviour is bounded.** On timeout the gate opens before the panic, so the scoped thread finishes and the scope joins. `block_on` has its own 30 s deadline, so the worst case is bounded.

**Kex row.**
- **Relation to `settled` and `KexRetry`:** the row pins that `settled` reruns exactly when a run's real handshake ended in a key exchange failure, up to `TRIES = 5` runs, with the last run being the result. The product's retry bound is not exercised or changed.
- **Not tautological in the substantive parts.** `runs > stand_ins` and `ends[..stand_ins] == 1` bite if the stand-in is not consumed, or if `settled` stops rerunning after a stand-in.
- **Regressions caught:** a `settled` that never reruns, caps below 3 runs, adds a run after a clean one, or reruns every time. See P3-4 for the rest.

**`kill_tree`.**
- **Platform split:** `cfg_if` structure is preserved, and Unix and Windows both send the killer's stdout and stderr to the null device. Only the null redirect changed, so exit semantics are unchanged because `.status()` is kept. There is no pipe to drain, so no deadlock risk from large output.
- **Still kills the tree.** The new row builds a root plus a leaf and requires the pipe to end, which only happens when both are gone, within 60 s. The README reports a red result against the original `kill_tree` on dabeest (3/3 fail with the garbled line) and against a root-only killer on Mac.
- **Orphans:** if the killer asserts after starting the tree, the leaf and root sleep 120 s and then exit. They are bounded and not a defect.

**Evidence versus raw logs.** The README's loop counts, failure indices (58, 610, 932, 395, 895, 536, 103), the 63 absorbed reruns, the red-run texts, the 1335.93 s suite, the 103m46s build and the split line all match the raw files. The only discrepancies are the stale 1.6 s figure in the code comment (P3-2) and the commit message (P3-5).

**Twin check.** `agent_login_fixture_pipe.rs` has a single one-byte `socket.read`, and `reads_closed` accepts EOF, `ConnectionReset` and `ConnectionAborted`. The evidence supports "no change" with a Python model of the sequence (`twin_race.py`): 300 of 300 reads returned `ConnectionAborted`. The limit is that this models the call sequence, not the Rust fixture. For a report-only item that is adequate.

## 3. Risks and next action

**Next action:** the lane owner may proceed to the queued WSL matrix and the Windows receipt (`win-flakes-20261030-b`) on `298b0783`.
- Nothing here blocks that receipt.
- P3-1, P3-2 and P3-3 are cheap and mechanical. I suggest folding them into the next touch of these files, or recording them in the follow-up list.
- P3-5 needs no tree change.

**Residual risks the receipt will not settle:**
- The kex row's ~0.1% flake at 1.5× oversubscription (P3-3).
- A broken-product pass if the product's cleanup bound is later lowered (P3-2).
- The one-thread `kill_tree` failure (P3-1).
- The unexplained 103-minute silent compile in the 7944 s receipt, which the README keeps open.

## 4. Out-of-scope observations

- **Product semantics.** `OPEN_CLEANUP_MS`, the product's 5 s discard of an in-flight close, is the only reason the close row and the exit bound work as they do. The `endpoint.close()` / `request.finish()` path releases the lease immediately, so the exit never actually waits for the close. I could not build a faithful "waits for the close" product mutation: an attempted wait inside `shutdown()` and one in `finish()` both passed, because the lease was already zero there. I therefore used a sleep in `finish()` as the stand-in, as the README does. This is not a defect, but the row's real-regression coverage rests on stand-ins.
- **cut_proxy WSAENOBUFS row.** I did not review it (out of scope). Its WSAENOBUFS failure shares a host condition with the connect failures in the residual kex runs. A `connect` ending in `Uncategorized` under 1.5× oversubscription is plausibly the same cause, but that is an inference, not a measurement.
- **Module doc of `ssh_controlled_server.rs`.** The header still describes the stuck rows only, and does not mention `Gated`. It is a documentation nit, with no correctness impact.
- **Probe artefacts.** The probe copy I built at `/Volumes/projects/limbo/build-targets/wfr-probe` is removed. The build output stays in `/Volumes/projects/limbo/build-targets/win-flakes-review-code`, outside the lane. Nothing in the lane was modified.
