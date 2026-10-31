# Step 2.3a (peer sandbox probes) — State-AXIS RE-VERDICT (round 2 remediation)

**Review object:** gwz-core 631ee1d8 (remediation round 2, parent d0e92a5a) in lane p2-3a. Evidence 2026-10-31-p2-3a/rem2.

**Baseline:**
- **Tuple, the same at start and end:** lane root 0ce82044, gwz-core 631ee1d8 with a clean tree, gwz-core-evidence 77e7333c.
- **Read:**
  - `git -C gwz-core show 631ee1d8`, all 12 files;
  - `GwzServerCore-Step2.3a-RemPlan-2.md`;
  - the rem2 README with its closure table and deviations;
  - the lane owner's gate logs for `-d`.
- **Copy:** an `rsync -aL` copy at `build-targets/review-p2-3a-state-3-src`. Its key files are byte-identical to `631ee1d8` (`cmp`).
- **Build and container:** my own target dir, image tag `review-p2-3a-state-3-img` and volume `review-p2-3a-state-3-cargo`. All of them are removed.

**Date:** 2026-10-31
**Axis:** State: adversity, TOCTOU, and whether the probes and hold fail closed and name their cause.
**Verdict: GO** — 0 P0, 0 P1, 0 P2, 1 P3 (R3-1, documentation).
- N-1, the round-2 P2, is closed by its closure tests and by my own counterexamples.
- My round-2 pre-commitment applies: N-1 is closed, and I found no new P0/P1/P2 in the changed lines.

## Prior-finding closure table

| ID | Round | Status at 631ee1d8 | Evidence (mine unless marked) |
| --- | --- | --- | --- |
| **N-1** (P2) EINVAL read as "kernel before 6.5" | 2 | **Closed** | **Mapping:** only `ENOPROTOOPT` gives `KERNEL_REQUIREMENT`. `EINVAL` and `ESRCH` give GONE. `ENODATA` and anything else give "the peer's process descriptor". **My counterexample, both directions** (root container, kernel 7.2.12). Peers: a reaped connector that handed off its connection, and a reaped listener that handed off its socket. The real hold returns `Unreadable("a live peer process")`: the descriptor's fdinfo says `Pid: -1`, so `descriptor_pid` reads it as gone. My 6.5–6.15 emulation answers EINVAL only for a stale descriptor, and also returns `Unreadable("a live peer process")`. A live peer through the same emulation is still held. **Mutation** (EINVAL mapped back to the kernel cause): 3 rows red, namely the kernel row, the drafter's new reaped connector/listener row and my row. Only emulated: no 6.5–6.15 host is available, as the rem2 README also states. |
| **N-2** (P3) exec race hides a thread's filter | 2 | **Closed, with a measured residual** (R3-1) | **The drafter's row** on the fixed tree: 55 runs (20 sequential, 30 six at a time, 5 as uid 1000). 52 runs had 0 missed trials and 3 had 1, against the limit of 10. **Mutation** (re-read removed): 52 of 150 trials missed. **My round-2 fixture** (scanning until the exec is seen): 0 of 150 on the fixed tree, 40 of 150 under the mutation. WSL `-d`: the row passes. |
| **N-3** (P3) descriptor-pid clause untested | 2 | **Closed** | Deleting `descriptor_pid(&pidfd) != Some(pid)` turns 2 rows red: the new `a_descriptor_that_names_another_live_process_is_refused_gone`, and `a_hold_is_the_descriptor_…`. |
| **N-4** (P3) macOS reused pid named `Profile` | 2 | **Recorded**, as the plan decided | The `Hold::check` doc in `unix.rs` states the limit. Fail-closed is unchanged (round-2 stale-token result). |
| **N-5** (P3) `--skip-exact` module-wide skips | 2 | **Closed** | `check_skip_exact` runs before any test, against a mocked listing. `::`, `a::`, `transport_host::carrier`, `a::b` (a substring of `a::b_more`) and an unlisted name each exit 2. The two real root-row names are accepted. A repeated name is accepted, which is harmless. The unit tests pass (`test_run_tests_skip_exact`, `test_check_carrier_peer_rule`: OK). |
| State P2-1 (ARCH) | 1 | Closed (round 2). Re-confirmed. | `hold_rows::a_reused_process_id_…` passes in my root run and in WSL `-d`. |
| State P2-2 | 1 | Closed by reading and emulation. The hosted run is still outstanding. | The workflow change in this commit is only the `uname -r` lines (deviation 2). |
| State P3-1 to P3-5, round-1 §4 | 1 | Unchanged since round 2: closed or recorded | All carrier rows green: root 121 of 121, which is 119 plus my 2. |
| Code C-2 | 1 | Closed (round 2) | The guard now also requires `hold.confirm(` and forbids `.check(` in `check_peer` (Code C2-4, drafter's red logs). |
| Code C2-1 = N-1 | 2 | As N-1 | — |
| Code C2-3 = N-5 | 2 | As N-5 | — |

## Changed-range analysis

- **`unix.rs`:** errno map (above), plus doc lines. No other logic changed. The `Hold` interface and the order in `check_peer` are unchanged.
- **`sandbox/linux.rs`:** `probe_dir` now reads, in order:
  1. the leader's status;
  2. the threads;
  3. `ns/user` and `ns/mnt`;
  4. the leader's status again (`after`), last.

  The result is the maximum of all three modes. Each new read refuses on error, so the process going away mid-scan is refused. No path accepts on error.
- **`sandbox.rs`, `local/mod.rs`:** kernel text corrected (ENOPROTOOPT before 6.5; EINVAL on 6.5–6.15; a stale descriptor on 6.16 and later). The PID-namespace sentence now names `a live peer process`, which matches `descriptor_pid` returning `None` for pid 0.
- **`hold_rows.rs`:** four row changes:
  - the kernel row is split (ENOPROTOOPT versus EINVAL/ESRCH);
  - a reaped connector and listener, each through the real `take` and through the EINVAL `ask`;
  - another live process's pidfd;
  - the exec row, with its fixture `exec_child` (a no-op without its env vars).
- **`sandbox_rows.rs`:** `seccomp_allow_all` becomes `pub(in super::super)`. The fake-`/proc` row gains a directory status file (EISDIR), which refuses as "the process's threads".
- **`run_tests.py`:** `check_skip_exact` runs before the first group. `SKIPPED TEST` lines are printed only after it passes.
- **Workflow:** two `echo` lines.
- **Peer guard:** an expression `cfg!` allow-list, `REQUIRED_CALLS` for `hold.confirm(`, and a `check_peer` body scan. The Code axis owns these. I ran the guard and its unit tests: OK.

I found no new P0, P1 or P2 in these lines.

## 0. Evidence base

- **Linux, my container, root, `--privileged`, 7.2.12:** `transport_host::carrier` 121 passed, 0 failed. That includes my two review rows:
  - `review_reaped_connector_and_listener_kernel_variants`;
  - `review_filtered_thread_exec_race` (0 of 150).
- **Mutations** (each built, then restored and checked with `cmp` against 631ee1d8):

  | Mutation | Result |
  | --- | --- |
  | EINVAL mapped back to the kernel cause | 3 red |
  | Descriptor-pid clause deleted | 2 red |
  | Leader re-read removed | exec row 52 of 150 missed and red; my fixture 40 of 150 and red |

- **Exec-row stress** (restored build): 55 runs. 52 had 0 missed trials and 3 had 1, against the limit of 10. Load on the Mac was high throughout, since other lanes were compiling.
- **Gates (lane owner, as reported, checked in the logs):**
  - WSL `p2-3a-remote-20261031-d`: kernel 6.18, HEAD 631ee1d8, OVERALL PASS. All 8 `hold_rows` rows ok, the exec row included.
  - Windows receipt `p2-3a-20261031-d`: commit 631ee1d8, PASS, 4366 passed, only the 3 named Python gaps.
  - Receipt `-c` (d0e92a5a) PASS closes the round-1 Windows compile question (Code C-4).
- **Evidence archive:** the regenerated `manifest.json` has 155 entries, all hashes match, and no file is unlisted. `tools/verify_archive.py` passes.
- **Mac:** not rebuilt. On Apple targets this commit changes only doc comments (the macOS `Hold::check` doc, `sandbox.rs`, `local/mod.rs`). Every code change is in Linux-only arms or Python. I rely on the drafter's `raw/green-mac-*.log` (111 of 111).

## Problem, requirement provenance, and scope

- **Every change traces to a RemPlan-2 row.** The RemPlan-2 rows trace in turn to the round-2 reviews and to the operator's 2026-10-31 decisions (the `SO_PEERPIDFD` refusal, and Q6).
- **Deviations declared by the drafter, judged:**
  1. **Exec row states a rate (≤10 of 150), not zero. Accepted.**
     - **The residual is real.** I measured 3 single-trial misses in 55 runs. The drafter's explanation is plausible and fits my data: while a non-leader thread's `exec` kills its siblings and takes over the ID, `/proc` does not show it. I did not trace it in the kernel myself.
     - **It needs a cooperating process.** A process that sets this up could equally apply the filter just after the check, which is the stated "sandboxed after the check" class. The Threads-count alternative refuses busy multi-threaded processes, and dropping it is right: a fix that breaks unrelated peers is worse than a documented residual.
     - **The row is sound and not flaky.** The fixed tree's worst run had 1 missed trial against the limit of 10, while the mutation gives 40 to 52. So the row separates the two by more than a factor of four in both directions.
  2. **Two `uname -r` echoes:** harmless, and useful, because the first one prints even when the step goes red.
  3. **`--skip-exact` lists with the run's own cargo arguments:** acceptable. The listing is the set of binaries the run executes, which is the right set to check names against. One observation: a compile failure in any test target now stops the run at the listing, before the library groups. It fails either way, but less of the run's output is shown.
  4. **`seccomp_allow_all` made `pub(in super::super)`:** test-module visibility only. No production effect.
- **Scope:** inside the RemPlan-2 rows. No new file. The parked design is not edited, as recorded.

## 1. Findings

### R3-1 (P3): `sandbox.rs` still states an unqualified per-thread guarantee
- **Location:** `sandbox.rs`, "Sandboxed after the check" paragraph: "Linux reads every thread, so a filter on any thread counts at the time of the check."
- **Gap:** the measured residual (above) is documented in `linux.rs` `probe_dir` and in the evidence README, but not in the module's guarantee table, which is where readers look.
- **Impact:** documentation only. The behaviour is fail-closed apart from the cooperative residual.
- **Correction:** add one clause, for example: "except, rarely, a thread that `exec`s while it is read (see `linux.rs`)".
- **Closure:** text review. This does not block GO.

## 2. Invariant analysis

- **A process that has gone is refused under its true cause on every kernel answer:**

  | Kernel answer | Result |
  | --- | --- |
  | Stale descriptor (6.16 and later) | GONE |
  | EINVAL (6.5–6.15, emulated) | GONE |
  | ESRCH | GONE |
  | ENOPROTOOPT (before 6.5) | names the kernel requirement |
  | Anything else | "the peer's process descriptor" |

  This holds in both directions.
- **The hold bracket (`take` → probe → `confirm`):** unchanged and sound. The pidfd is live at both ends, and a zombie is refused.
- **Fail-closed reads:** the extra `status` read refuses on error. A directory where a thread's status file should be refuses (C2-5). A vanished thread is ignored only on `NotFound`/`ESRCH`.
- **One-sided namespace reads, own-process refusal, and "nothing read, then one SessionError, then EOF":** unchanged from round 2, and all carrier rows are green.
- **Err-open paths:** only the documented exec residual, which needs a cooperating process.

## 3. Risks and next action

- **GO.** Two items remain before or after landing, outside this verdict:
  - The lane owner's hosted transport-candidate run, the closure of State P2-2. Watch the root step's `uname -r` line and look for a root rebuild into `candidate-target`.
  - A real 6.5–6.15 host run of `hold_rows`, when one is available, to replace the emulation.
- **R3-1:** a one-line doc fix whenever convenient.

## 4. Out-of-scope observations

- The parked design still states the pre-6.5 fallback (already recorded).
- The macOS pid-version check (token word 7) remains a hypothesis for a later step (N-4).
