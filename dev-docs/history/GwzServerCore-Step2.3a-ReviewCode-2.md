# Step 2.3a (peer sandbox probes) — Code-AXIS REVIEW
**Review object:** gwz-core 9261b0ef plus remediation d0e92a5a in lane p2-3a (`git -C gwz-core diff 9261b0ef^1 d0e92a5a`, restricted to the 2.3a files; the remediation alone is `diff f2b1bc71 d0e92a5a`); evidence 2026-10-31-p2-3a (including `rem1/`).
**Baseline:** I read the whole remediation diff. I also read the post-change `unix.rs` (`check_peer`, both `Hold` arms, `accept`, `connect`), `peer.rs`, `mod.rs`, `sandbox.rs`, `sandbox/{linux,macos}.rs`, the `windows.rs` token/hold/`judge`/`own_identity` code, `hold_rows.rs`, `probe_rows.rs`, the `sandbox_rows.rs` diff and Windows arm, the guard and its tests, `run_tests.py` and its new test, the workflow, `2.3a.json`/`2.1.json`/`meta.json`, the round-1 reports and the RemPlan, the rem1 README and its `raw/` logs (CI emulation included), plan §3 2.3a, §5 and Q6, and the lane owner's receipt `p2-3a-20261031-b` and WSL environment. I fetched Linux kernel sources (v6.8, v6.15, v6.16, v6.17: `net/core/sock.c`, `kernel/fork.c`, `net/unix/af_unix.c`) to check the `SO_PEERPIDFD` error semantics.

I built and ran in an `rsync -aL` copy, `build-targets/review-p2-3a-code-src` (gwz-core, gwz-transport, gwz-git, git2-rs, gwz-sspi, gwz-credential, taut). It had its own target dirs, `-code-mac` and `-code-linux`, and a separate guard-mutation copy, `-code-guard`. The Linux runs used my own image tag `review-p2-3a-code2-img` (a tag of `p2-3a-linux-img`) and my own volume `review-p2-3a-code2-cargo`, run root `--privileged` on OrbStack kernel 7.2.12. Every copy, target dir, tag and volume is now removed. Nothing in the lane was modified.
**Date:** 2026-10-31
**Axis:** Code: architecture, interfaces, call graphs and compatibility of the probes, the remediated Linux hold, the guard, the `--skip-exact` option and the CI steps.
**Verdict: NO-GO** — 0 P0, 0 P1, 1 P2 (C2-1), 4 P3 (C2-2 to C2-5). No new ARCHITECTURAL root cause. I pre-commit to GO on a revision that closes C2-1 with its closure tests, provided no new P0, P1 or P2 appears in the changed lines. The P3s may be fixed or recorded. The Windows receipt `p2-3a-20261031-c` is still pending; a red receipt reopens this verdict.

## Prior-finding closure table

| ID | Round-1 claim | Closure test | Result on d0e92a5a | Status |
| --- | --- | --- | --- | --- |
| Code C-1 (P3) | The guard does not see `cfg!(..)` | M-a fails the guard | M-a (`cfg!(debug_assertions)` early return in `linux::probe_dir`) now fails: `local/sandbox/linux.rs:69: … cfg(debug_assertions) the rule does not list`. But the expression form takes the attribute allow-list, which lists `test` in peer.rs, windows.rs and mod.rs. My G1 (`if !cfg!(test) { return Ok(()); }` at the top of `PeerPolicy::verify`) passes the peer guard, `check_cfg_boundaries`, `check_candidate_switches` and `check_process_globals`, and every row (Mac 13 of 13). The guard's own unit test asserts that `cfg!(test)` in windows.rs is clean. | **Partly closed; residual is C2-2** |
| Code C-2 (P3) | The probe is not bound to the hold | M-d fails to compile; M-c red | M-d (confirm before the probe): `error[E0382]: borrow of moved value: hold` on Mac and Linux. M-c (`Subject::Pid(hold.pid())`): `probe_rows::the_probe_of_a_peer_asks_about_the_token_the_hold_recorded` FAILED. But Md2 (`hold.check(fd)?` before the probe, `confirm` dropped) passes Linux 16 of 16 and the peer guard (G4). | **Closed for the named spellings; residual is C2-4** |
| Code C-3 = State P3-4 (P3) | A refused caller is told "the peer" | Display row without "peer" | Closed. `LocalRefusal::Own` and `about_caller`; rows `a_refused_caller_is_told_about_this_process_and_never_about_a_peer` and `own_text:` in the real-process rows; green on Mac and Linux. Residual, not a finding: Windows `token_identity` on the own token can still return `LocalRefusal::Peer(Unreadable("an integrity level"))` (`windows.rs:170`), and `create_instance` returns `Peer(Unreadable("a user SID"))` (:293). Both are reachable only on a malformed token. | **Closed** |
| Code C-4 (P3) | `windows.rs` visibility widened | Guard and Windows receipt green with the field private | By reading: `identity` is private, and the accessor sits in the existing `cfg_if! { if #[cfg(test)] …` arm. The only other users (`judge`, :263/:265) are in the same module. The peer guard is green. Receipt `p2-3a-20261031-b` (f2b1bc71, round 0) passed with 4289 library tests and 0 failures, so the arm as of round 0 compiled and ran. Receipt `-c` for d0e92a5a does not exist yet. | **Closed pending receipt c** |
| State P2-1 (P2, ARCH) | The Linux hold judges whoever holds the pid now | The pid-reuse rows refused `Unreadable`; a mutation back to `pidfd_open(pid)` red; an injected-failure kernel row | On kernel 7.2: `a_reused_process_id_does_not_stand_for_the_process_that_connected_or_listened` is green in my container (root, `ns_last_pid`). Mp (`pidfd_open(pid)` restored through `take_with`) makes 3 rows red, and the reuse row panics at `hold_rows.rs:93` because the connection was **accepted**. That is the original counterexample. On kernels 6.5 to 6.15 the reaped-peer answer is mapped to the wrong cause and the row goes red. | **Closed on 6.16 and later; open on 6.5 to 6.15 as C2-1** |
| State P2-2 (touches CI) | The hosted leg goes red on the namespace rows | A green hosted run with the rows passed | The workflow was read and the emulation logs read. No hosted run exists. C2-1 can make the root step red on a 6.5 to 6.15 runner kernel. C2-3 concerns the option the step relies on. | **Open (hosted run is the lane owner's closure test)** |
| State P3-1 (touches probe) | Seccomp on a second thread is not seen | Second-thread fixture refused as client and as own | `hold_rows::a_seccomp_filter_on_any_thread_refuses_the_client_the_listener_and_the_caller` is green on Linux. Mutation Mt (every read error treated as a vanished thread) is green on all 16 rows: the "unreadable thread refuses" branch has no row (C2-5). | **Closed; coverage gap C2-5** |
| State P3-3 (stale token) | A gone macOS peer is named `Profile` | Stub row `Unreadable` | Stub row green. My real-process rows (copy only): a plain `nc -U` child and a `sandbox-exec -p '(version 1)(allow default)'` child are held, probed live, then killed and reaped, then probed again. Live: plain `profile: false`, sandboxed `profile: true`. After the reap, both give `Err(Unreadable("a live peer process"))`, and `confirm` errs. Residual: a *reused* pid passes `check` (same socket token, `process_alive(pid)` true), so the stale token's 1 is still named `Profile`. It fails closed; this is a diagnosis-only limitation. | **Closed for gone peers** |
| Code §4 (record items) | Windows restricted tokens; macOS by-pid fallback unpinned; `sandbox.rs` default arm | Records | Recorded in the README. The by-pid fallback and the default arm are unchanged. | Recorded |

## Changed-range analysis

These are the remediation hunks `f2b1bc71..d0e92a5a`.

- `local/unix.rs`:
  - `Hold` (Linux) now takes the pin from `SO_PEERPIDFD` (`peer_pidfd`, `take_with` with an injectable `ask`).
  - It cross-checks `/proc/self/fdinfo` `Pid:` against `SO_PEERCRED` and signals before and after the start-time read.
  - It splits `check(&self)` from a consuming `confirm(self)`.
  - It adds `SO_PEERPIDFD = 77` for Android in a nested `cfg_if!`.

  The take sequence is sound: the descriptor names a live process with the credential pid, and that process is alive again after the start time is read. `check` reads the start time, then signals. Defects: the errno map (C2-1) and the non-consuming `check` escape (C2-4).
- `local/sandbox/linux.rs`: `thread_seccomp` over `task/*/status` takes the highest mode. ENOENT or ESRCH counts as a vanished thread; any other error refuses. This is correct; the refusing branch is untested (C2-5).
- `local/sandbox/macos.rs`: `probe_with(…, holds)` confirms a non-zero answer, and `of_peer_with(resolve, &hold, fd)` exists. This is correct; the reused-pid diagnosis is noted above.
- `peer.rs`, `mod.rs`: `LocalRefusal::Own` and `about_caller`. There are no consumers outside the carrier (grep), and the Windows `judge` maps with a wildcard. Correct.
- `windows.rs`: field privacy and the test accessor. Correct by reading.
- `sandbox.rs` docs:
  - "sandboxed after the check", the kernel requirement and the PID-namespace sentence were added;
  - the PID-namespace sentence says "refused the same way" (as the kernel requirement), but such a peer is refused as `a live peer process` (`descriptor_pid` gives 0). This is cosmetic; fold it into C2-1's text fix.
- `check_carrier_peer_rule.py`: the `cfg!` scan, and `target_os = "android"` in unix.rs. See C2-2.
- `run_tests.py` `--skip-exact` and `test_run_tests_skip_exact.py`: C2-3.
- The workflow: the sysctl step, the `--skip-exact` pair and the `sudo -E` root step with a count check. These are sound by reading. The risks (runner kernel, root-owned artifacts) are in §3.
- Tests:
  - `hold_rows.rs` and `probe_rows.rs` are new;
  - `sandbox_rows.rs` is at 983 lines and now takes the fixture role per call (`thread_*`, `*_handoff`);
  - `ending_rows.rs` is one line;
  - `tests/mod.rs` gains `cfg_if!` lines.

  `a_kernel_or_socket_that_cannot_name_the_peer_process_is_refused_with_its_cause` asserts the wrong mapping for EINVAL (C2-1).
- `windows_parity/2.3a.json`: 42 entries (38 platform, 4 paired), `done: false`. `2.1.json` changes counts only (`getsockopt` 4, `pid_t` 2, `socklen_t` 4, `SYS_pidfd_open` removed), as the parity check requires. `meta.json` is unchanged in rem1. The drafter's `checks.log` and the remote `check_windows_parity` pass.

## 0. Evidence base

- **Tuple:**
  - Start: root befd1270, gwz-core d0e92a5a with a clean tree, gwz-core-evidence 3464ef42.
  - End: the same, with gwz-core clean.
  - The object diff `9261b0ef^1..d0e92a5a` also carries merged-in root changes (`spawn_gate*`, `https_tests.rs`, the pin, the clippy allow-list). These are out of scope and were not reviewed.
- **Mac** (my copy, candidate shape, `TMPDIR=/Volumes/gwztmp`): `cargo test --lib -- sandbox_rows probe_rows` passed 11 of 11; 13 of 13 with my two stale-token rows.
- **Linux** (my container, root, privileged, kernel 7.2.12): `-- sandbox_rows hold_rows` passed 16 of 16. That includes:
  - the pid-reuse row (client and listener, `ns_last_pid`);
  - the second-thread seccomp row (`thread_connect`, `thread_listen`, `thread_own`);
  - the mount and user namespace rows.
- **Mutations (copy only):**

| Mutation | Result | Meaning |
| --- | --- | --- |
| M-a / G3: `cfg!(debug_assertions)` early return in `probe_dir` | Peer guard exit 1 | C-1 closed for this spelling |
| M-b: `#[cfg_attr(debug_assertions, path = …)]` on `mod linux` | Peer guard passes; `check_cfg_boundaries` reports NEW | Covered by the other gate (unchanged) |
| M-c: `Subject::Pid(hold.pid())` in `of_peer_with` | Mac: the probe-binding row FAILED | C-2 closed for M-c |
| M-d: `confirm` before the probe | E0382 on Mac and Linux | C-2 closed for M-d |
| Md2 / G4: `hold.check(fd)?` before the probe, `confirm` dropped | Linux 16 of 16 green; peer guard passes | C2-4 |
| Mp: `pidfd_open(pid)` through `take_with` | Linux: 3 red; the reuse row accepts the connection | P2-1 counterexample reproduced on the old hold, closed on the new |
| Mt: every thread-status read error treated as vanished | Linux 16 of 16 green | C2-5 |
| G1: `if !cfg!(test) { return Ok(()); }` in `verify` | All four source gates pass; Mac 13 of 13 green | C2-2 |
| G2: `if cfg!(target_os = "android") { return Ok(peer); }` in `check_peer` | All four source gates pass | C2-2 |
| G5: `Hold::take_with(…, &|_| Err(..))` in `check_peer` | Guard passes (fails closed, so the rows go red) | Injectable seam accepted, like the resolver |
| Emulated 6.5 to 6.15 kernel (review row): real reaped connector, `ask` returns EINVAL when the kernel's stale descriptor shows `Pid: -1` | This kernel: `Unreadable("a live peer process")`. Emulated: `Unreadable("a peer process descriptor (SO_PEERPIDFD, Linux 6.5 or later)")` | C2-1 |

- **Kernel source:**
  - v6.8 and v6.15 `pidfd_prepare` returns `-EINVAL` when `!pid_has_task(pid, PIDTYPE_TGID)`, that is, when the peer has been reaped.
  - v6.16 adds `PIDFD_STALE` for `AF_UNIX` in `SO_PEERPIDFD` (`af_unix.c` gains `pidfs_register_pid`; v6.15 has none), so a reaped peer gets a stale descriptor.
  - Before 6.5, an unknown `SOL_SOCKET` option is `-ENOPROTOOPT`.
  - All Linux evidence of this round is on 6.16 or later: OrbStack 7.2.12, and WSL `6.18.33.2-microsoft-standard-WSL2` (lane-owner environment `remote-…-a`).
- **`--skip-exact`:** `skip_exact_name('::')`, `('transport_host::')` and `('a::b')` are all accepted. Against the green Mac lib binary, `--list` shows 4080 tests; with `--skip ::` it shows 0, and with `--skip transport_host::` it shows 3570.
- **CI:** no hosted run. The drafter's emulation shows:
  - the filtered form: 113 of 115, two printed SKIPPED TEST;
  - the unfiltered form: the command is right, but 528 unrelated failures from missing fixtures;
  - the root step: 2 passed.

  All of it was on kernel 7.2, as one user and root inside one container.
- **Guard unit tests:** 20 of 20 pass, including the one that asserts `cfg!(test)` in windows.rs is clean.

## Problem, requirement provenance, and scope

- **Requested outcome:**
  - Plan §3 2.3a: a sandboxed same-user peer is refused on Linux and macOS, a sandboxed caller gets no policy, and a real AppContainer process is refused on Windows. The evidence includes "the hosted ubuntu runner".
  - The RemPlan, with the operator's 2026-10-31 decisions:
    - "widen 2.3a": the Linux hold comes from `SO_PEERPIDFD`, and a kernel without it is refused;
    - Q6 "privileged step in CI".
- **Scope of the remediation:**
  - **Authorized:** the Linux `Hold` `take`/`confirm` and its 2.1 callers (one line in `ending_rows.rs`), and the workflow.
  - **Not named in the RemPlan:** `run_tests.py --skip-exact` and its test. The README records a lane-owner decision "after the first draft" to keep ambient capabilities out of the suites. It is necessary for that chosen workflow form, and its provenance exists only in the evidence README.
  - **Forced:** the `2.1.json` count edits, by the parity check.
  - **Size-driven:** `hold_rows.rs` and `probe_rows.rs` keep `sandbox_rows.rs` under 1000 lines.
  - Every mechanism traces to a RemPlan row.
- **Necessity:**
  - `take_with`'s `ask` and `of_peer_with`'s `resolve` are function-parameter seams in production code, which is the brief's pattern ("a function parameter in private code, not a build switch"). Production passes only `peer_pidfd` and `dlsym_default`.
  - The `/proc/self/fdinfo` cross-check is needed: without it a descriptor from a socket whose credentials name another pid would be accepted.
  - The Android constant is needed only because the existing condition names Android. Android is not a parity platform.
- **The drafter's notes:**
  - *Budget:* rem1 is +1116/−160 on top of round 0's overrun. This is a process item.
  - *Windows partly type-checked:* round 0's arm compiled and passed on receipt b. The rem1 Windows edits are two small hunks that read clean. Receipt c is outstanding.
  - *Blind spots:* Landlock, LSMs, cgroups and capabilities are limitations the design names. The thread case is now read. "Sandboxed after the check" is now stated for every platform. I agree with these.
  - *Pipe DACL:* unchanged from round 1. Plausible, not verified here.

## 1. Findings

### C2-1 (P2). EINVAL from `SO_PEERPIDFD` is reported as a kernel without `SO_PEERPIDFD`; on Linux 6.5 to 6.15 it means the peer has been reaped
- **Root cause.** `Hold::take_with` maps `Some(libc::ENOPROTOOPT | libc::EINVAL)` to `Unreadable(KERNEL_REQUIREMENT)` (`unix.rs:460`). The kernel semantics differ:
  - only ENOPROTOOPT means "no such option" (kernels before 6.5);
  - on 6.5 to 6.15, `pidfd_prepare` returns EINVAL when the recorded peer's thread group has no task, which is exactly the reaped connector or listener of State P2-1;
  - from 6.16 an `AF_UNIX` socket gets a stale descriptor instead, which the code handles (`descriptor_pid` gives -1, so GONE).

  The RemPlan text ("ENOPROTOOPT or EINVAL") carried the error. The row `a_kernel_or_socket_that_cannot_name_the_peer_process_is_refused_with_its_cause` (`hold_rows.rs:148`) asserts it. Not architectural.
- **Location:** `src/transport_host/carrier/local/unix.rs:459-462`; `carrier/tests/hold_rows.rs:148-160`; `sandbox.rs:27-30` (the doc).
- **Violated invariant.** A refusal names its cause (design §4; the RemPlan's "with a cause naming the kernel requirement" applies only to kernels without the option). There is also Mac/Linux/Windows parity of evidence across supported Linux kernels, and the plan's hosted-runner evidence.
- **Reproduction:**
  - Kernel source, v6.8 and v6.15 `kernel/fork.c` `pidfd_prepare`: `if (!pid || !pid_has_task(pid, PIDTYPE_TGID)) return -EINVAL;`. v6.16 `net/core/sock.c` sets `PIDFD_STALE` for `AF_UNIX`.
  - My emulation row (reaped real connector, `ask` returns EINVAL for the stale descriptor, as 6.5 to 6.15 does) gives `Some(Unreadable("a peer process descriptor (SO_PEERPIDFD, Linux 6.5 or later)"))`, while the real 7.2 path gives `Some(Unreadable("a live peer process"))`.
  - `a_reused_process_id_does_not_stand_for_the_process_that_connected_or_listened` asserts `LocalRefusal::Peer(GONE)` exactly (`hold_rows.rs:94`, `:120`), so it fails on any 6.5 to 6.15 kernel.
- **Impact:**
  - On Ubuntu 24.04 GA (6.8), Debian 13 (6.12) and similar kernels, a peer that went away is diagnosed to the user (and, on the listener side, on the wire in `server_peer_refused`) as "your kernel lacks SO_PEERPIDFD" on a kernel that has it.
  - The admission row is red there. That includes the CI root step if the hosted ubuntu-24.04 kernel is in that range (not verified; no hosted run exists).
  - All green Linux evidence is from 6.16-or-later kernels.
  - The rule still fails closed: this is diagnosability and parity of evidence, not an open door.
- **Correction:**
  - Map ENOPROTOOPT alone to `KERNEL_REQUIREMENT`.
  - Map EINVAL (and ESRCH, for robustness) to `Unreadable("a live peer process")`.
  - Leave ENODATA and anything else as "the peer's process descriptor".
  - Change the kernel row's EINVAL expectation to GONE.
  - Add an emulated row: a reaped real connector through `take_with` with an `ask` that returns EINVAL, expecting GONE.
  - Correct the RemPlan wording and the `sandbox.rs` PID-namespace sentence. That sentence says "refused the same way", but such a peer is refused as `a live peer process`.
- **Closure test.** The kernel row with EINVAL expecting GONE and ENOPROTOOPT expecting the kernel cause. The emulated reaped-peer row green. The mutation that maps EINVAL back to the kernel cause turns both red. A hosted run, or the lane owner's run, that reports `uname -r` beside the root step's result.

### C2-2 (P3). The guard's expression scan allows `cfg!(test)` and `cfg!(target_os = "android")`, so a production-only bypass passes every gate and every row
- **Root cause.** The `cfg!` scan reuses the attribute allow-list (`check_carrier_peer_rule.py:139` with `ALLOWED_CFG`/`PLATFORM_CFG_FILES`, :45-53). In expression position, `!cfg!(test)` is a run-time branch that is strict in test builds and lax in production. The rows can never see it, by construction. The added `target_os = "android"` entry also admits `cfg!(target_os = "android")` anywhere in unix.rs, and no row runs on Android. No carrier production file uses `cfg!` today.
- **Location:** `scripts/checks/check_carrier_peer_rule.py:45-53, 139`; `test_check_carrier_peer_rule.py:112` (asserts that `cfg!(test)` in windows.rs is clean).
- **Violated invariant:** split doc §4.3, "no test-only override exists in production code (2.1 source check)"; the brief's rule that the guard still means what it says.
- **Reproduction:** G1 and G2 in §0. All four source gates pass, and Mac rows are 13 of 13 under G1.
- **Impact.** No current defect. It is a hole in the only enforcement against this class.
- **Correction.**
  - Give `cfg!(..)` its own allow-list, empty for every carrier file.
  - Allow the Android condition only as a `cfg_if!`/attribute condition, not as an expression.
  - Invert the unit test, and add `!cfg!(test)` cases for peer.rs, windows.rs and mod.rs.
- **Closure test:** G1 and G2 fail the guard.

### C2-3 (P3). `--skip-exact` accepts any name containing `::`, so it is a general bypass, contrary to its documentation
- **Root cause.**
  - `skip_exact_name` (`run_tests.py:157`) checks only for `::` and no whitespace.
  - The unfiltered path appends libtest `--skip NAME` (:128), which is a substring match.
  - The filtered path uses exact membership (:138), so the same option has two semantics.
- **Violated invariant.** The docstring (:43-48) says "only a full path (which names one test) is accepted". The RemPlan says the non-root suite keeps the rows failing loudly. The option must not become a general bypass (round-2 mandate).
- **Reproduction:** `skip_exact_name('::')` and `('transport_host::')` are accepted. On the lib binary, `--skip ::` lists 0 of 4080 tests and `--skip transport_host::` lists 3570. The run then prints `SKIPPED TEST ::: run in the privileged step` and exits 0.
- **Impact.** Bounded: it takes a workflow edit to use. It is still a silent, plausible-looking way to drop whole suites from CI.
- **Correction.** Accept a name only if it equals exactly one entry of the lib test binary's `--list`. Fail the run if a name matches none, or if it is a prefix or substring of any other listed test.
- **Closure test:** unit rows that `::`, `transport_host::` and a name that is a substring of another test's path are rejected.

### C2-4 (P3). The consuming `confirm` can be sidestepped through `check(&self)`, and the guard does not require the confirmation
- **Root cause.**
  - `Hold::check` is `pub(crate)` on both Unix arms (`unix.rs:477`, `:561`).
  - `check_peer` can call `check` before the probe and drop `confirm` without any compile error.
  - `REQUIRED_CALLS` names `sandbox::of_peer(` but not `hold.confirm(`.
- **Location:** `unix.rs:477, 561, 337-343`; `check_carrier_peer_rule.py` `REQUIRED_CALLS`.
- **Violated invariant.** Design §4: a check reads a process only through a hold, confirmed after the read. C-2's closure is "compile-enforced".
- **Reproduction:** Md2 / G4 (§0). Linux 16 of 16 green; the guard passes.
- **Impact.** Only a later edit could open it. The current order is correct.
- **Correction.**
  - Require `hold.confirm(` in unix.rs, and forbid `.check(` in `check_peer` (a token scan).
  - Alternatively, make `check` private and hand macOS's probe a closure built in unix.rs.
- **Closure test:** Md2 fails the guard.

### C2-5 (P3). No row exercises the "unreadable thread refuses" branch of the per-thread read
- **Root cause.**
  - The fake-`/proc` row covers a missing `task/`, a missing per-thread status (vanished) and a status without a mode.
  - No row has a status that exists but cannot be read for any reason other than vanishing (for example a directory in its place, which gives EISDIR, or mode 000 for a non-root runner).
- **Location:** `sandbox/linux.rs:59-60`; `sandbox_rows.rs` `an_unreadable_or_unparsable_file_refuses_and_a_readable_one_is_read`.
- **Violated invariant:** plan 2.3a, "each probe mutation-red"; the probe errs closed.
- **Reproduction:** Mt (§0). Treating every error as vanished stays green, 16 of 16.
- **Impact.** A later edit could make an unreadable thread count as "no filter", that is, fail open, unseen.
- **Correction:** add `task/12/status` as a directory to the fake-`/proc` row and expect `Unreadable("the process's threads")`.
- **Closure test:** Mt red.

## 2. Invariant analysis

- **The probe is on every path:**
  - Linux and macOS: `check_peer` is the only builder of a checked identity, called by `accept` (`ListenerChecksClient`) and `connect` (`ClientChecksListener`). It runs take, then `of_peer(&hold, fd)`, then `verify`, then a consuming `confirm`.
  - Windows: `judge` (accept and connect) reads the AppContainer and integrity flags through `ProcessHold`.
  - Caller: `current_user` is the only production constructor (`from_identity`/`try_from_identity` are `cfg(test)`), and it always runs `of_self`. It now refuses with `LocalRefusal::Own`.
  - No production caller of `current_user` exists yet; 2.3d and 2.3k inherit the obligation.
- **Pinning (Linux).** The pin is the kernel's connect-time or listen-time descriptor. The fdinfo pid equals the `SO_PEERCRED` pid. The process is signalled alive around the start-time read, and again in `check`. While the descriptor's process is alive its pid cannot be reused, so every `/proc/<pid>` read between take and confirm names the held process. A zombie is refused, because the state is Z. On 6.16 and later a reaped peer gives `Pid: -1`, so GONE. On 6.5 to 6.15 it gives EINVAL, which is mis-mapped (C2-1). A peer whose pid is not visible in this process's `/proc` gives 0 and is refused.
- **Closed `Cause` list.** It is unchanged: four variants, static texts, and `server_peer_refused` then EOF. The new `Unreadable` texts carry no pid. `KERNEL_REQUIREMENT` contains digits (a kernel version), not process data.
- **Per-thread read.** The verdict is the highest mode among the leader and every listed thread. A vanished thread (ENOENT or ESRCH) is skipped, which is correct, because its filter left with it. Any other error refuses, but that branch has no row (C2-5). Threads made later are in the "after the check" class (documented).
- **Resolver and hold seams.** `probe_with(resolve, subject, holds)`, `of_peer_with` and `take_with(ask)` are parameters, with no `cfg` and no environment read. Production passes `dlsym_default` and `peer_pidfd`. A malicious `ask` in production is caught by the root-only rows (Mp), not by the guard; this is accepted, like the resolver.
- **Repo rules:**
  - New `cfg` sits in `cfg_if!` (the Android constant is nested in the Linux arm; `tests/mod.rs`).
  - Control-flow bodies are braced.
  - No globals or thread-locals: `NS_LAST_PID` is a `const`, and the pid-reuse row writes a kernel sysctl file, which is test-only.
  - No production code under `tests/`.
  - Parity JSON is consistent with the drafter's checks.
- **Rows as proof.**
  - The decision table, the causes, the probes, the hold binding to `SO_PEERPIDFD` (Mp), the token binding (M-c) and the order (M-d) are red-guarded.
  - Not red-guarded: the `check` escape (C2-4), the unreadable-thread branch (C2-5) and production-only `cfg!` (C2-2).
  - The kernel-version dependence of the reuse row (C2-1) means its green proves the rule only on 6.16 and later.

## 3. Risks and next action

- **Next action.** A revision for C2-1 (errno map, row, emulated row, docs). The P3s are cheap: C2-2 is a regex plus a test, C2-3 is list validation, C2-4 is one required call, C2-5 is one fixture line.
- **Windows receipt `p2-3a-20261031-c`:** outstanding.
- **Hosted CI.** It has never run. The runner kernel decides whether the root step passes until C2-1 is fixed; record `uname -r` in that step. These are hypotheses, not verified:
  - `sudo -E cargo test` can rebuild as root if any fingerprint input differs from the suites step, leaving root-owned files in `candidate-target` for the clippy ratchet and the cache save. The emulation ran as one identity in one container, so it cannot show this. Compile time in the step log ("Compiling" lines) would reveal it.
  - The two root rows run in parallel. The mount row's forks can take the pid the reuse row aims at. `reuse()` retries 2000 times, which makes this unlikely to be flaky, but not proven.
- **Deployment cost (operator-decided):** kernels before 6.5 (Debian 12's 6.1, RHEL 9's 5.14) refuse every local peer. This belongs in release notes.

## 4. Out-of-scope observations

- **macOS stale-token diagnosis:** after pid reuse, `check` passes (same socket token, live pid), so a stale answer of 1 is named `Profile`. It fails closed. Comparing the token's pid version (word 7) with the live process would name it correctly.
- **Windows own-token errors** still use `LocalRefusal::Peer` (`windows.rs:170`, `:293`). They are practically unreachable.
- **The macOS by-pid fallback** (no `sandbox_check_by_audit_token`) is still not pinned to the token's pid version. Current macOS has the token symbol.
- **Provenance:** `--skip-exact` is recorded only in the evidence README. The RemPlan (or a later step's plan) should name it.
