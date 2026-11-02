# Step 2.3a (peer sandbox probes) — Code-AXIS RE-VERDICT (round 3)
**Review object:** gwz-core 631ee1d8 (remediation round 2, parent d0e92a5a) in lane p2-3a; evidence 2026-10-31-p2-3a/rem2 (gwz-core-evidence 77e7333c).
**Baseline:**
- **What I read:**
  - `git -C gwz-core show 631ee1d8`, all 12 files;
  - `GwzServerCore-Step2.3a-RemPlan-2.md`;
  - the rem2 `raw/` file list;
  - the lane owner's gate logs for WSL `p2-3a-remote-20261031-d` and receipt `p2-3a-20261031-c`;
  - the coordinator's gate results for 631ee1d8.
- **Where I ran things:**
  - An `rsync -aL` copy at `build-targets/review-p2-3a-code3-src`, with its own target `-code3-linux`.
  - A separate guard-mutation copy, `-code3-guard`.
  - My own image tag `review-p2-3a-code3-img`, my own volume `review-p2-3a-code3-cargo`, and the container names `review-p2-3a-code3-run` and `-uid`, on OrbStack kernel 7.2.12.
- **Cleanup:** every copy, target, tag and volume is removed.
- **Tuple:** root 0ce82044, gwz-core 631ee1d8 with a clean tree, gwz-core-evidence 77e7333c, the same at start and at end.
**Date:** 2026-10-31
**Axis:** Code. I re-checked my round-2 findings C2-1 to C2-5, their counterexamples, and the changed lines.
**Verdict: GO** — 0 P0, 0 P1, 0 P2, 1 new P3 (C3-1, not architectural, and not blocking). This is the GO I pre-committed to in round 2: C2-1 is closed by its closure tests, and the changed lines contain no new P0, P1 or P2. Both Windows receipts passed: `p2-3a-20261031-c` (d0e92a5a), which meets C-4's condition, and `p2-3a-20261031-d` (631ee1d8, 4366 passed, only the 3 named Python host gaps). WSL `p2-3a-remote-20261031-d` passed on 631ee1d8 (Linux 6.18).

## Prior-finding closure table

| ID | Round-2 closure test | Result on 631ee1d8 | Status |
| --- | --- | --- | --- |
| C2-1 (P2) EINVAL read as a missing kernel feature | Kernel row: ENOPROTOOPT gives the requirement, EINVAL and ESRCH give GONE. Emulated reaped connector and listener give GONE. Mapping EINVAL back to the kernel cause turns them red. | `take_with` now maps only ENOPROTOOPT to `KERNEL_REQUIREMENT`; EINVAL and ESRCH give `a live peer process` (`unix.rs`). The docs in `mod.rs`, `sandbox.rs` and on the `KERNEL_REQUIREMENT` constant are corrected, as is the PID-namespace sentence. Green, root, 21 of 21: the new kernel row, the new `a_reaped_connector_and_a_reaped_listener_are_refused_gone_on_every_kernel_answer`, and my round-2 emulation row re-added in my copy (`this kernel Some(Unreadable("a live peer process")); as 6.5-6.15 Some(Unreadable("a live peer process"))`). Green as uid 1000, 3 runs × 4 rows. With the EINVAL arm mutated back, the kernel row, the drafter's reaped row and my emulation row each FAIL with `Unreadable("a peer process descriptor (SO_PEERPIDFD, Linux 6.5 or later)")`. | **Closed** |
| C2-2 (P3) `cfg!(test)` and `cfg!(target_os = "android")` pass the guard | G1 and G2 fail the guard | `cfg!(..)` now has its own, empty allow-list. G1 fails: `local/peer.rs:174: … cfg(test) the rule does not list`. G2 fails: `local/unix.rs:338: … cfg(target_os = "android")`. The guard's own unit tests are inverted and extended (they include `core::cfg!`). | **Closed** (the attribute form is C3-1) |
| C2-3 (P3) `--skip-exact` is a general bypass | Broad names refused; one full name accepted | `check_skip_exact` lists the run's tests before any test runs. Against the real lib listing in my container: `::` was refused (exit 2; it is part of 4128 other tests), so were `transport_host::` (524) and `transport_host::carrier` (119), and so was a misspelled row name (0 matches). The two CI names were accepted. | **Closed** |
| C2-4 (P3) `check` lets a probe follow and `confirm` be dropped | Md2 fails the guard | Md2 now fails twice: `never calls hold.confirm(` and `local/unix.rs:341: check_peer calls .check(`. The remaining escape (a helper outside `check_peer` plus a dead `confirm`) is a limitation of the token scan; see §2. | **Closed** |
| C2-5 (P3) the unreadable-thread branch has no row | Mt red | Fake `/proc` `task/12/status` is a directory and expects `Unreadable("the process's threads")`. Under Mt the row FAILS at `sandbox_rows.rs:783` (`left: None`). | **Closed** |
| C-1, C-2, C-3 (round 1) | — | Unchanged by this patch. | Closed |
| C-4 (round 1) | Windows receipt green with the field private | Receipt `-c` (d0e92a5a) PASSED: 4290 library tests in the hosted-like shape and 0 failures beyond the 3 named Python gaps. Receipt `-d` (631ee1d8) PASSED: 4366 passed, only the 3 named Python host gaps. | **Closed** |
| State P2-1 (ARCH, round 1) | — | The pid-reuse row is green as root on 7.2 and in WSL `-d` (6.18). C2-1's fix extends the refusal cause to kernels 6.5 to 6.15, through the emulated rows. | Closed |
| State P2-2 (hosted CI) | A green hosted run | Not run yet. The root step now prints `uname -r` with its result. | Open (the lane owner's post-landing closure, per the RemPlan) |

## Changed-range analysis

- **`unix.rs`.**
  - The errno map is correct against the kernel source cited in round 2: ENOPROTOOPT before 6.5, EINVAL from `pidfd_prepare` on 6.5 to 6.15, a stale descriptor from 6.16. ESRCH is harmless defence.
  - The macOS `check` gains a doc stating the reused-pid limitation. It is text only.
- **`sandbox/linux.rs`.**
  - `probe_dir` now reads the leader, then the threads, then both namespace links, then the leader again, and takes the maximum. Each read errs closed.
  - The added read is correct and narrows the State N-2 exec race. It cannot close the race: during an `exec`'s ID takeover, `/proc` shows the exec'ing thread nowhere.
- **`check_carrier_peer_rule.py`.**
  - The expression form has a separate, empty allow-list.
  - `REQUIRED_CALLS` takes patterns and now requires `hold.confirm(`.
  - `function_body(check_peer)` scans for `.check(` or `::check(`. It fails closed when `check_peer` is missing.
  - The parsing is sound for the current source, since comments and strings are stripped first.
- **`run_tests.py`.**
  - The check runs after the source checks and before any test. It lists with the run's own cargo arguments.
  - Its substring rule makes libtest's `--skip` and the filtered path's equality mean the same thing.
  - It exits non-zero on a failed listing.
- **Workflow.** Two `uname -r` echoes, around the root step. Text only.
- **Tests.**
  - New in `hold_rows.rs`: the reaped connector and listener row, the "descriptor of another live process" row (State N-3), and the exec-race row with its `exec_child` fixture. The fixture is the same "test that is a fixture when an environment variable is set" pattern as `sandbox_child`; the variable is read in tests only, outside the guard's scope.
  - In `sandbox_rows.rs`: the `task/12` step, and `seccomp_allow_all` widened to `pub(in super::super)`. That is test-only visibility.
  - These are green as root (21 of 21) and as uid 1000. The exec row passed 10 of 10 repeated runs as root.
- **`2.3a.json`.** `check_windows_parity` passes with "nothing new" in my guard copy. `check_process_globals` passes with "nothing new".
- **Gates on 631ee1d8:**
  - WSL `p2-3a-remote-20261031-d` (Linux 6.18): OVERALL PASS, `candidate-test` 4120 passed.
  - Windows receipt `p2-3a-20261031-d`: PASS, 4366 passed, only the 3 named Python host gaps.

## Drafter-declared deviations

1. **A rate, not zero, in the exec-race row.** I accept it.
   - The residual is a kernel visibility interval: the exec'ing thread is listed nowhere while it takes over the ID. No `/proc` read order can close it.
   - The row still separates the two cases: at most 10 of 150 trials may miss, against about 1 in 1000 measured with the fix and about 1 in 4 without. So removing the re-read turns it red (the drafter's `red-linux-leader-reread-removed.log`), and it is not flaky in my 10 runs.
   - The residual belongs in the limits text. The State axis owns its adequacy.
2. **`uname -r` echoed twice.** Harmless.
3. **`--skip-exact` lists with the run's own cargo arguments.** I accept it. Without `--lib` it builds every test target up front, which the suites build anyway. It is the right listing for the unfiltered path, where `--skip` reaches every binary.
4. **`seccomp_allow_all` widened to `pub(in super::super)`.** Test-only visibility. No production effect.
5. **The evidence manifest regenerated with whole keys.** An evidence-archive repair, outside the code.

## 0. Evidence base

| Run (my copy, Linux 7.2.12) | Result |
| --- | --- |
| `-- sandbox_rows hold_rows`, root | 20 of 20; 21 of 21 with my emulation row |
| The new hold rows plus my emulation row, uid 1000, 3 runs | 4 of 4 each time |
| Exec-race row, root, 10 runs | 10 of 10 |
| EINVAL arm mutated back to the kernel cause, plus Mt, in one build | 4 FAILED: the kernel row (`hold_rows.rs:180`), the reaped row (:238), my emulation row, and the unreadable-file row (`sandbox_rows.rs:783`). Each fails on its own assertion. |
| Guard copy: G1, G2, Md2 | All fail the peer guard (messages in the closure table) |
| Guard copy: G6, G7 | Pass (C3-1, and §2) |
| `check_skip_exact` against the real listing | `::`, `transport_host::`, `transport_host::carrier` and a misspelled row refused with exit 2; the two CI names accepted |
| Unit tests | `test_check_carrier_peer_rule.py` OK, `test_run_tests_skip_exact.py` OK |

I did not rebuild on macOS this round. The only macOS change is a doc comment, and my round-2 macOS results stand. The lane owner's gates on 631ee1d8, WSL `-d` and Windows receipt `-d`, both passed.

## Problem, requirement provenance, and scope

- **Provenance.**
  - Every change traces to a RemPlan-2 row: C2-1 to C2-5, State N-2, N-3 and N-4, and the CI note.
  - The RemPlan explicitly supersedes round 1's "ENOPROTOOPT or EINVAL" clause.
  - It names `--skip-exact` under Q6 (the lane owner's decision), which settles round 2's provenance note.
- **Scope.**
  - The production edits stay inside the RemPlan's boundary: the `take_with` errno arm, the leader re-read in `probe_dir`, and docs.
  - The `Hold` interface, the order in `check_peer` and the `Cause` list are unchanged, as the RemPlan says.
  - Test, guard and runner edits sit in already-owned files.
- **Round.** This is the last remediation round. C3-1 below is a P3 and does not require one.

## 1. Findings

### C3-1 (P3, new, not architectural). The guard's attribute allow-list still admits a production-only rule in `cfg_if!` form
- **Root cause.**
  - `ALLOWED_CFG` lists the condition `test` for `peer.rs`, `windows.rs` and `mod.rs`. This allowance has existed since 2.1.
  - The guard checks only the condition string. It does not check where the condition is used, or whether it has an `else` arm.
  - So `cfg_if! { if #[cfg(test)] { fn lax() -> bool { false } } else { fn lax() -> bool { true } } }`, with `if lax() { return Ok(()); }` in `verify`, is strict in test builds and lax in production.
  - This is the attribute spelling of the class that C2-2 closed for `cfg!`. `cfg_if!` is the form the repository rule prescribes, which makes it a likely spelling.
- **Location:** `scripts/checks/check_carrier_peer_rule.py`, `ALLOWED_CFG` and the switch loop.
- **Violated invariant.** Split doc §4.3, "no test-only override exists in production code"; the guard docstring's "no build variant has a different rule".
- **Reproduction:** mutation G6 in my guard copy. `check_carrier_peer_rule` reports ok over 17 files, and `check_cfg_boundaries` reports nothing new. The rows cannot see it, because they run in the test build.
- **Impact.** No current defect. It is an enforcement gap against a later edit.
- **Correction.** Refuse an `else` arm, and an `else if` chain, after a `test` condition in carrier production files. The current uses are `mod tests;` and the `cfg(test)` items, which have no `else` arm. Alternatively, allow `test` only on the item names listed today (`try_from_identity`, `from_identity`, the windows.rs test helpers, the re-exports).
- **Closure test:** G6 fails the guard. Add a unit case with `if #[cfg(test)] {…} else {…}` in peer.rs.

## 2. Invariant analysis

- **The rule refuses on every path, for the right cause.**
  - Accept and connect still go through `check_peer`: take, then probe, then verify, then a consuming confirm.
  - On Linux, a peer that has gone is now refused as `a live peer process` whatever the kernel answers. That covers 6.5 to 6.15 (EINVAL, shown by emulation) and 6.16 or later (a stale descriptor, real on 7.2 and 6.18).
  - Only ENOPROTOOPT, from a kernel before 6.5, names the kernel requirement.
- **Errs closed.** The extra leader read and the per-thread read refuse on any non-vanished read error. That branch is now red-guarded (Mt).
- **Guard strength.**
  - Expression switches are refused everywhere.
  - The confirm is required, and `.check(` is barred inside `check_peer`.
  - Remaining token-scan limits:
    - G7, a helper that calls `check` from outside `check_peer` plus a dead `confirm` elsewhere in unix.rs, passes. That needs deliberate obfuscation and is consistent with the guard's stated "conservative, not name resolution".
    - The attribute form, C3-1.
- **The runner option.** It is an exact exclusion in both paths and is checked before any test runs. It is no longer a bypass.
- **Parity.** Mac rows (round 2) are green. Linux is green on 7.2 and 6.18, with 6.5 to 6.15 covered by emulation. Windows receipt `-d` is green.
- **Repository rules.** The new cfg in the rows sits inside the existing `cfg_if!` arms. There are no new globals: `EXEC_PATH` and `EXEC_DELAY` are environment variable names, not state. There is no production code under `tests/`. Bodies are braced.

## 3. Risks and next action

- **Hosted CI.** It runs after landing and closes State P2-2. It will show the runner's kernel. Today's image reportedly runs 7.0, so only my emulation and the drafter's covers 6.5 to 6.15. Round 2's hypotheses about root-owned artifacts and the parallel root rows stand.
- **Next action:** land the change, then make the hosted run. C3-1 can be fixed in a later step or recorded.

## 4. Out-of-scope observations

- **macOS reused-pid diagnosis.** It is now documented on `check`. The pid-version comparison (token word 7) remains a hypothesis for a later step.
- **Windows own-token errors under `LocalRefusal::Peer`.** Recorded by the RemPlan; unchanged.
