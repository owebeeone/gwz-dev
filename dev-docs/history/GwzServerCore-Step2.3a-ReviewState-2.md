# Step 2.3a (peer sandbox probes) — State-AXIS REVIEW

**Review object:** gwz-core 9261b0ef in lane p2-3a; evidence 2026-10-31-p2-3a. This is round 2: the object plus remediation d0e92a5a. I read it as `git -C gwz-core diff 9261b0ef^1 d0e92a5a` restricted to the 2.3a files, and the remediation alone as `git diff f2b1bc71 d0e92a5a`.

**Baseline:**
- **Tuple, the same at start and end:** lane root befd1270, gwz-core d0e92a5a with a clean tree, gwz-core-evidence 3464ef42.
- **Code read:** the remediation diff, and the post-change `unix.rs` (`check_peer`, both `Hold` arms, `peer_pidfd`, `descriptor_pid`, `signalled`), `sandbox.rs`, `sandbox/linux.rs`, `sandbox/macos.rs`, `peer.rs`, `local/mod.rs` and the `windows.rs` hold.
- **Rows read:** `hold_rows.rs`, `probe_rows.rs`, `sandbox_rows.rs`, the `ending_rows.rs` hold row, `transport-candidate.yml`, `run_tests.py` and its new test.
- **Round 1 inputs:** the two reports and the RemPlan.
- **Evidence:** the drafter's `rem1/` README, tools and raw logs, plus the lane owner's WSL gate `p2-3a-remote-20261031-c` and the round-0 Windows receipt `p2-3a-20261031-b`.
- **Kernel source** (read-only, github.com/torvalds/linux): `net/core/sock.c` at v6.12, v6.15 and v6.16, `kernel/fork.c` at v6.12, and `kernel/pid.c` at v6.16. I fetched v6.16 `kernel/pid.c` looking for `pidfd_prepare` and did not find it there.

**Date:** 2026-10-31
**Axis:** State: adversity, TOCTOU across the hold, and whether each probe and the new hold fail closed and say why.
**Verdict: NO-GO** — 0 P0, 0 P1, 1 P2 (N-1), 4 P3 (N-2 to N-5).
- The round-1 err-open (State P2-1) is closed on every kernel. Nothing I tried accepts a sandboxed or replaced peer, except the narrow race in N-2.
- N-1 is a wrong errno mapping. On Linux 6.5 to 6.15, a peer that has gone is refused under the wrong cause, and the pid-reuse row goes red there.
- **Pre-commitment:** GO on a revision that closes N-1 with its closure tests, if no new P0/P1/P2 appears in the changed lines. N-2 to N-5 are recommended, not required.

## Prior-finding closure table

| ID (round 1) | Disposition | Status at d0e92a5a | Evidence (mine unless marked) |
| --- | --- | --- | --- |
| **State P2-1** (ARCH): Linux hold judged whoever held the pid at check time | Fix: `SO_PEERPIDFD` hold | **Closed for the err-open.** The closure test is red on 6.5–6.15 kernels (see N-1). | My root container (kernel 7.2.12): `hold_rows::a_reused_process_id_…` passes for both client and listener. My mutation back to `pidfd_open(pid)` makes that row panic "the connection's holder is a sandboxed process: refused", meaning the connection was *accepted*. 4 more rows go red. The fresh `SO_PEERPIDFD` on a reaped connector gives fdinfo `Pid: -1` and is refused `Unreadable("a live peer process")`. WSL 6.18 root gate `-c`: all hold rows ok. |
| **State P2-2**: hosted Linux leg red on namespace rows | Fix: sysctl step, `--skip-exact` in the suites, root step | **Closed by reading and emulation.** The hosted run is still outstanding. | The workflow reads correctly (see the changed-range analysis). The drafter's `ci-emulation-*` logs show 113 of 115 without capabilities and 2 of 2 in the root step with the count check. Today's hosted image (`ubuntu24/20261026.354.1`) runs kernel **7.0.0-1017-azure**, so the pid-reuse row will pass there. N-1 does not show on hosted CI. One hypothesis I did not verify: if the root step's fingerprints differed, it would rebuild as root into the shared `candidate-target`. Only the hosted run settles that. The `--skip-exact` guard is weak (N-5). |
| **State P3-1**: second-thread seccomp not seen | Fix: read every `task/*/status` | **Closed for the stated fixture.** A residual race remains (N-2). | `hold_rows::a_seccomp_filter_on_any_thread_…` passes (client, listener, own). The mutation `threads-ignored` is red (drafter). |
| **State P3-2**: Linux "sandboxed after the check" undocumented | Doc | **Closed** (text review: `sandbox.rs` L17-30, `local/mod.rs`) | — |
| **State P3-3**: macOS gone or replaced peer named `Profile` | Fix: `holds()` before `Profile` | **Partly closed.** The "gone" case is fixed. The "pid reused" case still says `Profile` (N-4). | Real Mac, a real child reaped before the probe: `Err(Unreadable("a live peer process"))`. The real `sandbox_check_by_audit_token` with a stale token (version word +1) on a live pid answers 1, `hold.check` passes, and the result is `profile: true`. |
| **State P3-4** = Code C-3: caller told "the peer" | Fix: `LocalRefusal::Own`, `about_caller` | **Closed** | Mac and Linux rows `a_refused_caller_is_told_…` and `own_is_refused` (no "peer") pass. |
| **State P3-5**: Windows own-process via a child's identity | Record | Accepted, recorded in the rem1 README | Round-0 receipt `-b` (at f2b1bc71): PASS, 4365 tests, only 3 named Python gaps, so the AppContainer row passed. |
| State §4: Windows `ProcessHold` pins at check time | Record for the 2.1 owner | Recorded | — |
| State §4: daemonizing servers | Record for 2.3f/2.3k | Recorded. It also applies to N-1: a dead `listen()`-er triggers the wrong cause. | — |
| **Code C-2**: probe not bound to the hold | `confirm(self)`, token resolver row | **Closed** | M-d on Linux gives `error[E0382]: borrow of moved value: hold`. M-c on Mac turns `probe_rows::the_probe_of_a_peer_asks_about_the_token_the_hold_recorded` red. |
| Code C-1: guard missed `cfg!` | Regex | **Closed** | On a copy, `cfg!(not(test))`, `cfg!(debug_assertions)`, `cfg ! (feature = "x")` and `core::cfg!(unix)` in `probe_dir` all fail the peer guard, which then passes again after the copy is restored. M-b (`cfg_attr(…, path=…)` on `mod linux`) still passes the peer guard. `check_cfg_boundaries` reports it NEW, as in round 1. |
| Code C-4: `windows.rs` visibility | Private field, test accessor | Guard green. **Not compiled** until receipt `p2-3a-20261031-c`. | `check_carrier_peer_rule` ok (17 files) on `git archive d0e92a5a` |

## Changed-range analysis

`git diff f2b1bc71 d0e92a5a`: 19 files, +1116/−160.

- **`unix.rs`, Linux `Hold`:**
  - `take` = `take_with(fd, pid, uid, &peer_pidfd)`. The order is: `getsockopt(SO_PEERPIDFD)`, then fdinfo `Pid:` == `SO_PEERCRED` pid, then `signalled`, then `process_start`, then `signalled`.
  - `check` = `process_start` == (start, not dead), then `signalled`. `confirm(self)` = `check`.
  - The bracket is sound. The descriptor names a live or zombie process at both ends of every read, so the pid cannot be recycled between them, and a zombie is refused by `process_start`.
  - Defects found: the errno mapping (N-1) and the untested pid-equality guard (N-3).
  - `SO_PEERPIDFD` on Android is a literal 77. That is correct, and it sits inside `cfg_if!`.
- **`unix.rs`, macOS `Hold`:** `check` and `confirm(self)` were split. `check` cannot see pid reuse (N-4).
- **`check_peer`:** probe, then `verify`, then `confirm(hold)`. Ordering is compile-enforced (M-d). Nothing is read from the stream.
- **`sandbox/linux.rs`:** the leader's `status`, then every `task/*/status` (taking the max), then `ns/user` and `ns/mnt`.
  - A thread is treated as vanished on `NotFound` or `ESRCH` only.
  - A missing `Seccomp:` line, a non-numeric value, a missing task dir, `ns` or `status`: each refuses. The synthetic row covers these, and I reran it.
  - The leader is not re-read after the scan (N-2).
- **`sandbox/macos.rs`:** `of_peer_with(resolve, &hold, fd)`. A positive answer calls `hold.check(fd)` first; a negative answer refuses; a missing symbol refuses.
- **`peer.rs`, `local/mod.rs`:** `LocalRefusal::Own` and `about_caller`. Display only; no state change.
- **`windows.rs`:** a private `identity`, plus a `#[cfg(test)]` accessor inside the existing `cfg_if!`. Windows `confirm` stays `&self`. That is harmless there, because `judge` reads nothing after it.
- **`run_tests.py --skip-exact`:**
  - In unfiltered runs it becomes libtest `--skip` (a substring match). In filtered runs it is an exact list exclusion.
  - Validation is only "contains `::`, no whitespace" (N-5).
- **`transport-candidate.yml`:**
  - The sysctl step comes before the suites.
  - The suites skip exactly the two root rows by name.
  - The root step uses `sudo -E env HOME PATH RUSTFLAGS GWZ_TEST_GIT cargo test --locked --lib --exact <2 names>` and checks `passed == 2`.
  - Under Actions' `bash -eo pipefail`, a failing row fails before the count check.
  - Its `if: !cancelled() && suites != skipped` is correct.
- **Rows:**
  - `hold_rows.rs`: pid reuse, thread filters, kernel/socket refusal, credential equality.
  - `probe_rows.rs`: token binding, gone-peer answer.
  - `sandbox_rows.rs`: the `Own` text, thread-file shapes.
  - `ending_rows.rs`: one row now uses `check`, then `confirm`.

## 0. Evidence base

- **My copy.** `rsync -aL` of gwz-core (no target or .git), gwz-transport, gwz-git, git2-rs, gwz-sspi, gwz-credential and taut, to `build-targets/review-p2-3a-state-2-src`. Own target dirs `-mac` and `-linux`, plus `TMPDIR=/Volumes/gwztmp/rv2`. Candidate shape. All of it was deleted afterwards. My rows were added in the copy only (`review_rows.rs` for Linux, `review_mac_rows.rs`, and one fixture role `review_thread_exec`).
- **Mac** (rustc 1.95.0):
  - `sandbox_rows probe_rows review_mac`: 13 of 13.
  - `transport_host::carrier`: 113 of 113 (the drafter's 111, plus my 2).
  - M-c is red, as stated above.
- **Linux.** My tag `review-p2-3a-state-2-img` of the lane image and my volume `review-p2-3a-state-2-cargo`. Root, `--privileged`, seccomp/apparmor unconfined, OrbStack kernel 7.2.12. The tag and volume are removed.
  - `transport_host::carrier`: 115 passed. The only 2 failures are my review rows, each a finding (N-1, N-2).
  - uid 1000, unprivileged: `hold_rows sandbox_rows` 14 passed, 2 failed. These are exactly the mount row ("needs CAP_SYS_ADMIN") and the pid-reuse row ("needs write access to /proc/sys/kernel/ns_last_pid"), each naming the capability.
- **Linux mutations** (in the copy, each rebuilt, then restored; restoration checked with `cmp` against the lane):
  - **M-d** (confirm before the probe): E0382, compile-red.
  - **`pidfd_open(pid)` in `take_with`:** 5 red, including the pid-reuse row, where the connection is *accepted*.
  - **Drop `descriptor_pid(&pidfd) != Some(pid)`:** 0 drafter rows red (N-3).
  - **Re-read the leader's status after the thread scan:** my exec race goes to 0 misses (N-2), and all 17 sandbox and hold rows stay green.
- **My fixtures and their results:**
  - (a) A reaped connector whose `SO_PEERPIDFD` is answered as a 6.5–6.15 kernel answers it (EINVAL, see N-1). Result: `Unreadable("a peer process descriptor (SO_PEERPIDFD, Linux 6.5 or later)")`. On this 7.2 kernel the same connector gives `Unreadable("a live peer process")`.
  - (b) A filtered non-leader thread that `exec`s while the probe scans (N-2). Baseline: 29 of 150 and then 31 of 150 trials had a clean scan with the hold still checking. With the re-read: 0 of 148.
  - (c) macOS stale token on a live pid gives `profile: true`; a gone child gives `Unreadable`.
  - (d) The second-thread seccomp and pid-reuse fixtures: the drafter's rows reran green on the corrected tree.
- **Gates on `git archive d0e92a5a`:**
  - `check_carrier_peer_rule` ok; its unit tests and `test_run_tests_skip_exact`: 25 tests OK.
  - `check_cfg_boundaries`, `check_process_globals`, `check_windows_parity`: nothing new.
  - New `cfg` only inside `cfg_if!`; no globals or thread-locals; no production code under `tests/`.
- **Lane owner gates:**
  - WSL `p2-3a-remote-20261031-c` (gwz-core d0e92a5a, kernel 6.18, root): OVERALL PASS, candidate-test 4116 passed, every `hold_rows`/`sandbox_rows` row ok.
  - Windows receipt `-c` for d0e92a5a: not made.
  - Hosted CI: not run.

## Problem, requirement provenance, and scope

- **Requested outcome.** Plan 2.3a (C1):
  - a sandboxed same-user peer is refused on Linux and macOS;
  - a sandboxed caller gets no policy;
  - a real AppContainer is refused on Windows.

  The governing text is the parked design §4: "The sandbox rule", "the check cannot be made: the process has gone, began after the connection", and "Process IDs are pinned … Linux: `SO_PEERPIDFD`".
- **Authority for the widened scope.** The operator's 2026-10-31 decisions in the RemPlan:
  - "Widen 2.3a": the Linux hold comes from `SO_PEERPIDFD` and refuses without it, instead of the design's pre-6.5 start-time fallback. That is an explicit decision and is recorded.
  - "Q6, privileged step in CI".

  Every changed file traces to a RemPlan row. Two items trace to the lane owner's decision, recorded in the rem1 README:
  - `run_tests.py --skip-exact`. Its purpose is to keep the non-root suites free of ambient CAP_SYS_ADMIN while the rows still "fail, not skip". It is necessary given that decision.
  - The new row files `hold_rows.rs` and `probe_rows.rs`. They keep `sandbox_rows.rs` under 1000 lines.
- **Necessity of each mechanism:**
  - `SO_PEERPIDFD`: the only kernel-provided connect-time pin.
  - fdinfo pid equality: the only guard against a `/proc` from another pid namespace.
  - The double `signalled`: brackets the start-time read.
  - The thread scan: P3-1.
  - `confirm(self)`: C-2.
  - `holds()` on macOS: P3-3.
  - The injectable `ask` and `resolve`: the only way to row-test kernel refusals and stub answers. Production passes only `peer_pidfd` and `dlsym_default`.
- **Out of scope and unchanged:** other 2.3 sub-steps, and merged root changes.
- **Process note:** the remediation is +1116 lines, against the step's ~500-line aim. That is the lane owner's matter, not a state defect.
- **Drafter's round-0 notes, re-judged:**
  - **Budget:** process, as above.
  - **Windows partly type-checked:** round 0 passed receipt `-b`. The round-1 Windows edits remain uncompiled until `-c`.
  - **Blind spots (Landlock, LSM, cgroup/capabilities):** stated limits. The non-first-thread case is now read, apart from N-2.
  - **macOS undocumented answer:** my stale-token result again shows a non-zero answer for a stale token, so it fails closed. The diagnosis is wrong in the reuse case (N-4).
  - **Pipe DACL:** consistent with the receipt. Not re-verified by me.

## 1. Findings

### N-1 (P2): `SO_PEERPIDFD`'s EINVAL is read as "kernel before 6.5", so on Linux 6.5–6.15 a gone peer is refused under the wrong cause, and the pid-reuse row is red there
- **Root cause** (not architectural): the errno mapping. `unix.rs` `take_with`, `Some(libc::ENOPROTOOPT | libc::EINVAL) => PeerRefusal::Unreadable(KERNEL_REQUIREMENT)`.
  - A kernel without the option answers **ENOPROTOOPT**: `sk_getsockopt`'s default case, since 77 is unknown before 6.5.
  - **EINVAL** comes from kernels that *do* have it.
    - In v6.5 to v6.15, `SO_PEERPIDFD` calls `pidfd_prepare(peer_pid, 0, …)` (verified at v6.12 and v6.15).
    - `pidfd_prepare` returns `-EINVAL` when `!pid_has_task(pid, PIDTYPE_TGID)`, that is, when the connector or listener has been reaped (v6.12 `kernel/fork.c`).
    - Only v6.16 and later pass `PIDFD_STALE` for AF_UNIX (v6.16 `sock.c`). Those kernels return a stale descriptor (fdinfo `Pid: -1`), which the code refuses correctly as GONE.
- **Locations:**
  - `unix.rs` `take_with`;
  - `hold_rows.rs` `a_kernel_or_socket_that_cannot_name_the_peer_process_is_refused_with_its_cause`, which asserts that EINVAL names "Linux 6.5";
  - the RemPlan P2-1 row, which says "ENOPROTOOPT or EINVAL";
  - `local/mod.rs` and `sandbox.rs` kernel-requirement text.
- **Violated invariant:**
  - Design §4 and §17: a refusal names its cause, and here the cause is "the process has gone".
  - Linux parity across the supported kernel range: rows are not kernel-version-dependent.
- **Reproduction:**
  - My row `review_reaped_connector_kernel_variants`, root container. A plain child connects, hands off to `sleep` and is reaped. The real hold gives `Unreadable("a live peer process")`. The same socket, with the reaped case answered EINVAL as on 6.5–6.15, gives `Unreadable("a peer process descriptor (SO_PEERPIDFD, Linux 6.5 or later)")`, displayed as "the peer check could not read a peer process descriptor (SO_PEERPIDFD, Linux 6.5 or later)".
  - It follows that `hold_rows::a_reused_process_id_…`, which asserts `LocalRefusal::Peer(GONE)` exactly, fails on any 6.5–6.15 kernel in both directions.
- **Impact:**
  - It fails closed. Nothing is accepted.
  - On Ubuntu 24.04's GA kernel (6.8) and on 6.12, the Debian 13 and RHEL 10 kernel, two kinds of user are told to upgrade to Linux 6.5 on a kernel that already has the option:
    - a client whose server's `listen()` process has exited (the daemonizing case recorded for 2.3f and 2.3k);
    - a server whose connector has gone.
  - The parity row is red on such a host, for example a Debian 13 lab host. I have not checked weftpi's kernel.
  - No current gate shows it. Hosted CI is on 7.0.0-azure and WSL is on 6.18.
- **Correction:**
  - Map only ENOPROTOOPT to `KERNEL_REQUIREMENT`.
  - Map EINVAL (and ESRCH) to `Unreadable("a live peer process")`.
  - Fix the doc text ("a kernel before 6.5 answers ENOPROTOOPT; 6.5–6.15 answer EINVAL for a peer that has gone").
- **Closure test:**
  - The kernel row expects ENOPROTOOPT to name the requirement, and EINVAL to give `GONE` without "Linux 6.5".
  - A pid-reuse-shaped row with `take_with` and an `ask` that answers EINVAL for a stale descriptor (my emulation) is refused `GONE` in both directions.
  - Optionally, `hold_rows` passes on a real 6.5–6.15 host.
  - A mutation that restores the EINVAL arm must be red.

### N-2 (P3): a filtered thread that execs during the thread scan hides its filter
- **Root cause:** `probe_dir` reads the leader's `status`, lists `task/`, and treats a listed thread whose file has gone as vanished. A non-leader thread T under a filter that `exec`s takes over the leader's ID (`de_thread`), and its `task/<T>` entry disappears.
  - A scan that read the old leader before the switch, and T after it, sees only mode 0.
  - The start time is inherited and the pidfd stays the group's, so `check` passes.
- **Location:** `sandbox/linux.rs` `probe_dir` and `thread_seccomp`.
- **Violated invariant:** `sandbox.rs`: "Linux reads every thread, so a filter on any thread counts at the time of the check." At every instant of the check some thread of the process carried the filter, and after the exec the whole process does.
- **Reproduction:** my row `review_filtered_thread_exec_race`, root container. A child's second thread installs an allow-all filter, connects, waits 0–3 ms, then `exec`s `sleep`. The test, holding a real `Hold`, loops `of_peer` plus `check`. Result: 29 of 150 and then 31 of 150 trials had a scan with `seccomp: 0` while the hold still checked. The post-exec probe gives `seccomp: 2`.
- **Impact:** a narrow window, and the process must cooperate in it. Its unfiltered leader could hand the connection to a sandboxed program after the check anyway, which is the stated "sandboxed after the check" limit. So this gives an attacker nothing new, but the documented guarantee is overstated.
- **Correction:** after the thread scan, re-read `dir/status` and take its mode into the maximum. Validated: 0 of 148 misses, all sandbox and hold rows green. Alternatively, weaken the sentence.
- **Closure test:** the exec-race row (bounded trials, 0 clean-while-held scans). Removing the re-read must turn it red.

### N-3 (P3): the credentials-equality guard in `take_with` is load-bearing but no row tests it
- **Root cause:** `descriptor_pid(&pidfd) != Some(pid)` is the only check that a `/proc/<pid>` read names the held process when `/proc` belongs to another pid namespace. An example is a process in `unshare --pid --fork` without a remounted `/proc`. There the `SO_PEERCRED` pid and the `/proc` numbering differ, and without the check the probe would read an unrelated process.
  - `a_hold_is_the_descriptor_of_the_process_the_credentials_name_and_no_other` uses `pid + 1`, which `process_start` refuses on its own whenever that pid does not exist.
- **Location:** `unix.rs` `take_with`; `hold_rows.rs`.
- **Violated invariant:** plan 2.3a, "each probe mutation-red". Design §4, "A check reads a process only through a hold on it."
- **Reproduction:** I deleted the equality clause in my copy. All 35 sandbox, hold and ending rows stay green.
- **Impact:** no current defect. A later edit could remove the guard silently.
- **Correction and closure test:** a row calling `take_with(fd, own_pid, uid, ask)` where `ask` returns `pidfd_open` of a live child. It is refused `GONE`, and removing the clause turns it red.

### N-4 (P3): on macOS, `Hold::check` cannot see pid reuse, so a reused pid is still diagnosed `Profile`
- **Root cause:** the macOS `check` compares the socket's token with itself (it is constant per connection) and calls `process_alive(pid)`. An impostor holding the pid passes both.
  - P3-3's correction therefore covers "gone" but not "replaced".
  - The token-form probe answers 1 for the stale token, which then becomes `Profile`.
- **Location:** `unix.rs` macOS `Hold::check` and its doc ("Whether the process is the one held"); `sandbox/macos.rs` `probe_with`.
- **Reproduction:** my Mac row `review_mac_stale_token_live_pid`. The real `sandbox_check_by_audit_token` with a version-word+1 token on a live, unsandboxed pid gives `Ok(… profile: true)`. A gone child gives `Err(Unreadable("a live peer process"))`.
- **Impact:** it fails closed, but the cause is wrong. The by-pid fallback is also unpinned (Code §4, round 1).
- **Correction:** either compare the live process's pid version with token word 7 in `check`, or state the limit in `check`'s doc and in the README. One possible source of the live version is `task_name_for_pid` plus `TASK_AUDIT_TOKEN` (hypothesis, unverified).
- **Closure test:** the stale-token row expects `Unreadable`, or a text review if the limit is only documented.

### N-5 (P3): `--skip-exact` accepts patterns that skip whole modules
- **Root cause:** `skip_exact_name` accepts any string that contains `::` and no whitespace. In unfiltered runs, the value becomes libtest `--skip`, which is a substring match.
- **Location:** `scripts/run_tests.py` `skip_exact_name` and `run`.
- **Violated invariant:** its own doc says "only a full path (which names one test) is accepted". The review brief adds that it must not become a general bypass.
- **Reproduction:** `skip_exact_name` accepts `'::'`, `'a::'`, `'transport_host::carrier'` and `'sandbox_rows::a_process'`. `--skip ::` leaves out almost every library test, and the run stays green with only a "SKIPPED TEST" line.
- **Impact:** a typo or a later workflow edit can hide rows. It is reachable only by editing CI or the command line.
- **Correction:** check every name against the group's `--list`. It must equal exactly one listed test and be a substring of no other listed name. Otherwise exit non-zero.
- **Closure test:** unit cases where `::` and `transport_host::carrier` are refused, and a name that matches no listed test is refused.

## 2. Invariant analysis

| Invariant | Result |
| --- | --- |
| Hold names the connect-time or listen-time process (Linux) | **Holds** (`SO_PEERPIDFD`). Pid reuse is refused as client and as listener; the `pidfd_open` mutation goes red. |
| TOCTOU take → probe → confirm | **Sound.** A pidfd that is live (or zombie) at both ends of every `/proc` read rules out pid recycling in between; zombies are refused by `process_start`; leader exec keeps the start time and changes no field the probe reads. Exception: a non-leader exec mid-scan (N-2). |
| Kernel or socket that cannot answer | Refuses. A pipe fd (ENOTSOCK) and ENODATA give "the peer's process descriptor". **Wrong cause** for EINVAL on 6.5–6.15 (N-1). |
| `SO_PEERCRED` pid = descriptor pid | Enforced, untested (N-3). Mismatch, a hidden pid (fdinfo 0) or a reaped pid (−1) refuse. |
| Every odd file shape refuses | Holds. Missing `status`, task dir, `ns/user` or `ns/mnt`, a missing or non-numeric `Seccomp:` line, and a thread status without a mode all refuse (row rerun). In production the `ns` paths are always `/proc` links. |
| Vanished thread vs unreadable thread | `NotFound`/`ESRCH` are ignored. Any other error refuses (the code reads `Err(_) => Err`; root cannot make an EACCES row). Vanishing hides an exec'd filter (N-2). |
| Namespace read on one side only | Refuses (`Option` comparison; decision table). In production both sides are `Some`. |
| macOS: error, missing symbol, stale token | A negative answer or a missing symbol refuses. A stale token answers 1, so it fails closed, with the cause right for "gone" and wrong for "reused" (N-4). |
| Own-process refusal | Structural. `bind` and `connect` both need a `PeerPolicy`, whose only production constructor, `current_user`, probes `/proc/self` (all threads). The texts carry no "peer". |
| Listener reads nothing, then exactly one SessionError, then EOF | Holds. `check_peer` reads nothing; `turn_away` sends one frame (wire rows rerun green on both hosts). |
| Any path errs OPEN | Only N-2 (narrow, cooperative). |
| Seams | `take_with(ask)` and `probe_with(resolve)` are production `pub(crate)` with only `peer_pidfd`/`dlsym_default` passed. The guard's `REQUIRED_CALLS` does not pin those arguments; the rows' mutations do. |

## 3. Risks and next action

- **Next action:** a revision for N-1, with its closure tests. N-2 is a short addition. N-3 and N-5 are a row and a check. N-4 is a doc line or a version check. Then a re-review against the closure tests named in §1.
- **Pre-commit check for the lane owner:**
  - receipt `p2-3a-20261031-c`, since the Windows edits are uncompiled;
  - the hosted `transport-candidate` run (P2-2's closure), watched for a root-step rebuild into `candidate-target`;
  - a run on a 6.5–6.15 kernel host, if one is available.

## 4. Out-of-scope observations

- `ending_rows.rs` now calls `check` and then `confirm`. Windows `ProcessHold::confirm` stays `&self`, which is harmless there.
- The design's pre-6.5 fallback ("start time must precede the connection") is replaced by refusal under the operator's decision. The parked design text still states the fallback; the lane does not edit it.
- Hosted `ubuntu-latest` moves to 26.04 in November 2026 (runner-images announcement). That is a later kernel, and it does not affect N-1's hosted invisibility.
