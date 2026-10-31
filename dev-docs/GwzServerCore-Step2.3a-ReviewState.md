# Step 2.3a (peer sandbox probes) — State-AXIS REVIEW
**Review object:** gwz-core 9261b0ef in lane p2-3a; evidence 2026-10-31-p2-3a
**Baseline:** I read the commit with `git -C gwz-core show 9261b0ef`, and the files at lane HEAD gwz-core f2b1bc71. That merge changes only `connection_host/https_tests.rs` and `candidate_clippy_allowlist.json` against the object, so the object holds unchanged at HEAD. The tuple was the same at start and end: root 32ad237a, gwz-core f2b1bc71 with a clean tree, gwz-core-evidence 103527b5. I checked the evidence patches (`patches/*.new`, `gwz-core-tracked.diff`) against the commit, and they are byte-identical. I read the controlling documents at the lane root.
**Date:** 2026-10-31
**Axis:** State: adversity, TOCTOU, and whether each probe fails closed when it cannot complete.
**Verdict: NO-GO** — 0 P0, 0 P1, 2 P2, 5 P3. I pre-commit to GO on a revision that closes P2-1 and P2-2 with the closure tests named below, provided no new P0/P1/P2 appears in the changed lines.

## 0. Evidence base
- **Builds and runs in my own copy.** I made an `rsync -aL` copy of gwz-core, gwz-transport, gwz-git, git2-rs, gwz-sspi, gwz-credential and taut under `build-targets/review-p2-3a-state-src`, with my own target directories. All of it was deleted afterwards. The candidate shape was `RUSTFLAGS='--cfg gwz_transport_candidate' GWZ_TEST_GIT=real`.
  - **Mac** (macOS arm64, rustc 1.95.0, `TMPDIR=/Volumes/gwztmp`): `sandbox` 8/8 passed and `transport_host::carrier` 108/108 passed. This matches the drafter.
  - **Linux** (OrbStack kernel 7.2.12, my own tag `review-p2-3a-state-img` of the drafter's image and my own cargo volume; both removed):
    - root, `--privileged`, seccomp and apparmor unconfined: carrier 110/110 with my rows skipped. The sandbox rows pass, plus my adversarial rows below.
    - uid 1000, unconfined: 10/11. The mount-namespace row fails with "...cannot be made on this host... needs CAP_SYS_ADMIN", which reproduces the drafter's `nonroot-linux-sandbox.log`.
    - uid 1000 in a **default** Docker container: the own process shows `Seccomp: 2`, and 26 of 110 carrier rows fail (11 of 15 sandbox rows).
- **My adversarial fixtures.** I added these to `sandbox_rows.rs` in my copy only:
  1. A seccomp-filtered child connects, hands the connection to a grandchild (`sleep`, which inherits the filter) and exits. The child is reaped, and its pid is reused through `/proc/sys/kernel/ns_last_pid` by an unsandboxed `sleep`. Then the listener accepts. The same sequence was run for a seccomp-filtered *listener* that hands off its listening socket.
  2. A seccomp filter installed on a second thread (no TSYNC), which connects from that thread. The same thread also calls `current_user`.
  3. A child whose `/proc/self/comm` is set to `"\nSeccomp:\t0"` (an attempt to forge a status line), under seccomp.
- **The SO_PEERPIDFD fix tested in C** (in the container, root): after the connector is reaped and its pid reused, `SO_PEERCRED` still names the pid. `SO_PEERPIDFD` returns a descriptor whose `pidfd_send_signal(0)` fails with ESRCH. 2.1's `pidfd_open(pid)` instead names the reused process and signals "alive".
- **macOS `sandbox_check*` behaviour** (my C probe over `LOCAL_PEERTOKEN`, with a plain child and a `sandbox-exec` child):

  | Case | Plain child | Sandboxed child |
  | --- | --- | --- |
  | Live token | 0 | 1 |
  | Live token with the version word +1 | 1 | 1 |
  | Token after the child exited and was reaped | 1 | 1 |
  | `sandbox_check(pid)` after exit | 1 | 1 |

  errno was 0 in every case. So the token form does validate the pid version, and a stale token gives a non-zero answer.
- **Gates on a pristine `git archive f2b1bc71`:**
  - `check_carrier_peer_rule.py` ok (17 files), and its unit tests pass.
  - `check_cfg_boundaries.py` (skipping gwz-cli and gwz-py): nothing new.
  - `check_process_globals.py`: nothing new.
  - New and changed control-flow bodies are braced. No bare `#[cfg]` outside `cfg_if!`. No new globals. No production code under `tests/`.
- **Evidence I read:** the drafter's README, raw logs and mutation table; `build-targets/p2-3a-logs/remote-a.log` (lane owner's WSL root gates, label `p2-3a-remote-20261031-a`: `candidate-test` PASS, 4111 passed); and `transport-candidate.yml`.
- **Windows:** I made no run. `p2-3a-receipts/` did not exist when I checked, so I could not judge the receipt.

## Problem, requirement provenance, and scope
- **Requested outcome.** Plan 2.3a, carried item C1, traced through the convergence plan :668-669 to the 2.1 review carry and the operator's 2026-10-31 go:
  - a sandboxed same-user peer is refused on Linux and macOS;
  - a sandboxed caller gets no policy;
  - a real AppContainer process is shown refused on Windows.
- **Governing rule.** Parked server design §4 "The sandbox rule" and "Process IDs are pinned". The design says a check refuses when the process "began after the connection", and that the Linux pin is `SO_PEERPIDFD` on 6.5 and later.
- **Necessity of each mechanism:**
  - Seccomp, user-namespace and mount-namespace probes, and `sandbox_check_by_audit_token`: each is named by the design, and nothing smaller would do.
  - Findings carried inside `PeerIdentity`: needed by the brief's rule that `verify` stays pure.
  - Two `dlsym` forms: needed because the symbols are undocumented.
  - `try_from_identity` as a test constructor: justified by the injected own-sandbox rows, and the source check covers it.
- **Scope.** The edits stay inside the brief's file ownership: `check_peer`, the `Hold` accessors, `pub(crate)` on `judge` and `ProcessHold::identity` for the Windows row, and the parity JSON.
  - The size overrun (rows 824 against about 350) is a process deviation for the lane owner. It is not a state defect.
- **Out of the brief's ownership:** both P2 corrections. P2-1 needs `Hold::take` (a 2.1 function) and P2-2 needs `transport-candidate.yml`. Neither can be fixed by the drafter without the lane owner's authorization.

## 1. Findings

### P2-1 — Linux probes judge whoever holds the pid now, not the process that connected or listened (ARCHITECTURAL root cause in the inherited 2.1 hold)
- **Root cause.** Linux `Hold::take` (`carrier/local/unix.rs:410-428`) pins the process by `pidfd_open(SO_PEERCRED.pid)` at check time. Its start-time bracket only brackets that open. The probe (`sandbox/linux.rs` `of_peer`, called at `unix.rs:341`) reads `/proc/<pid>` through that hold.
  - `SO_PEERCRED`'s pid stays recorded after the connector (or the `listen()` caller) exits.
  - Once the pid is reused, every read sees the new process, and `confirm` (`unix.rs:430`) passes.
- **Violated invariant.** Design §4: "a process that began after the connection is refused, since a reused process ID would otherwise name another process" (Linux: `SO_PEERPIDFD`). The commit's claim is also broken: "a same-user process under a seccomp filter... was accepted" is said to be fixed, but this path still accepts one.
- **Reproduction** (my fixture 1, root container): the connection is held by the pid-445 grandchild with `Seccomp: 2`, and the listener reports:
  ```
  REVIEW pid 442 now names an unsandboxed process: Seccomp:	0
  REVIEW ACCEPTED: peer Verified(PeerIdentity { ... pid: Some(442), ... sandbox: SandboxFindings { seccomp: 0, ... } })
  ```
  The listener direction behaves the same way: `REVIEW ACCEPTED listener: ... pid: Some(436)`, while the socket is held by a `Seccomp: 2` process.
  - `ns_last_pid` only makes the reuse deterministic.
  - An attacker can drive it without that. A sandboxed listener can wait any length of time for a same-user pid reuse. A sandboxed client can keep its orphaned connection in the backlog, because each refused trickling connection holds the accept thread for `TURN_AWAY_BOUND` (1 s).
- **Impact.** The rule errs OPEN: a sandboxed process can reach a server with its user's authority (Git hooks, `core.fsmonitor`), or can pose as a listener to a real client.
  - It is probabilistic, so P2 rather than P1. It is still a concrete correctness defect of an admission step.
  - macOS is not affected. The token form validates the pid version (§0), so a stale token returns 1 and the peer is refused, though under the wrong cause (P3-3).
- **Correction.**
  - Take the Linux hold from `getsockopt(SO_PEERPIDFD)` on the connected socket. That descriptor names the connect-time or listen-time process. Keep `confirm`'s `pidfd_send_signal(0)` and the start-time check on that descriptor.
  - Where `SO_PEERPIDFD` is unavailable (kernels before 6.5, or ENOPROTOOPT), refuse, or record the design's "start precedes connection" fallback as an explicit decision.
  - This needs the lane owner to widen ownership to `Hold::take`, or a 2.1 remediation.
- **Closure test.** My two `review_pid_reuse_*` rows (needing root and `ns_last_pid`, failing with a named capability otherwise) are red now. They must be refused `Unreadable` after the fix. A mutation that goes back to `pidfd_open(pid)` must turn them red.

### P2-2 — The hosted Linux candidate leg goes red on the new real-process rows (open plan Q6)
- **Root cause.** The plan (Q6, "Needed before 2.3a is reviewed") was to decide how the namespace rows behave on hosted CI. No decision is recorded, and `transport-candidate.yml` is unchanged. The leg runs `scripts/run_tests.py` on `ubuntu-24.04` as the non-root runner, with no `sudo`, `sysctl` or container (`transport-candidate.yml:30, :189`).
- **Location.** `sandbox_rows.rs:592` (mount namespace), the user-namespace row, and `.github/workflows/transport-candidate.yml`.
- **Violated invariant.** The candidate shape stays green (workflow rule (a), TR2.12), together with the brief's rule that rows "fail, do not skip".
- **Reproduction.**
  - *Confirmed:* the mount row fails deterministically without CAP_SYS_ADMIN (drafter's `nonroot-linux-sandbox.log` and my uid-1000 rerun).
  - *Hypothesis, not run by me:* the user-namespace rows also fail on ubuntu-24.04, whose default `kernel.apparmor_restrict_unprivileged_userns=1` makes the `uid_map` write fail.
  - The lane owner's WSL run is root, so it cannot show this.
- **Impact.** Red CI after the push. The "CI after push" rule applies.
- **Correction.**
  - The operator decides Q6.
  - Then the workflow either runs the namespace rows with privilege (`sudo -E` for the filtered rows, or the user-namespace sysctl plus a root step for the mount row), or the rows take a recorded decision for how they behave on a non-root host.
  - Outside the brief's ownership.
- **Closure test.** A hosted `transport-candidate` run of the lane commit that is green and contains the three namespace rows as passed (not ignored).

### P3-1 — A seccomp filter on a second thread is not seen, for a peer or for the caller (limitation, confirmed)
- **Location.** `sandbox/linux.rs` reads `/proc/<pid>/status` (the first thread) and `/proc/self/status`.
- **Reproduction** (fixture 2):
  ```
  this-thread "Seccomp:\t2" /proc/self "Seccomp:\t0"
  second-thread seccomp client: Ok(Verified(...seccomp: 0...))
  second-thread seccomp own: own:Ok(())
  ```
- **Status.** The limit is stated for peers, so this is not a defect. It is not stated for the caller's own process.
- **Cheap improvement.** Read every `/proc/<pid>/task/*/status` inside the hold window (and `/proc/self/task/*` for the caller). Any thread with a non-zero mode refuses. This shrinks the gap to "sandboxed after the check".
- **Closure test.** The second-thread fixture refused `Seccomp` as client and as own.

### P3-2 — The Linux limits omit the "sandboxed after the check" class
- **Location.** `sandbox.rs:17-20` and the README limits.
- **Gap.** "Enters its sandbox after the check" is stated for macOS only. It applies on Linux too:
  - a peer may `seccomp` or `unshare` after the check;
  - it may fork a sandboxed child that inherits the connected descriptor;
  - a thread may `unshare(CLONE_NEWNS)` (root).
- **Impact.** The documentation overstates the Linux claim.
- **Correction.** One sentence in the module doc and in the README. **Closure:** text review.

### P3-3 — macOS: a peer that is gone, or whose pid version changed, is refused as "sandboxed by a sandbox profile"
- **Location.** `sandbox/macos.rs:60`: any positive answer becomes `Profile`.
- **Reproduction.** My C probe: a stale token (version changed, or child reaped) answers 1 with errno 0. A peer that exits between `Hold::take` and the probe therefore gets `Sandboxed(Profile)`, not `Unreadable("a live peer process")`.
- **Impact.** It fails closed, but the diagnosis is misleading (§17 names causes to users).
- **Correction.** On a non-zero answer, run the hold's `confirm` before naming `Profile`, and map a failed confirm to `Unreadable`.
- **Closure test.** A probe-resolver row whose stub answers 1 while the hold reports the process gone expects `Unreadable`.

### P3-4 — A caller's own refusal reads "the peer is sandboxed by ..."
- **Location.** `peer.rs:111`; `from_own` reuses `PeerRefusal::Sandboxed`.
- **Impact.** When the caller itself is sandboxed (for example gwz inside a Docker default seccomp profile), the message blames a peer. That misleads the very users the design says must be told to use `--no-server`.
- **Correction.** A distinct display for the own-process case. The design's `server_peer_refused` code can stay.
- **Closure test.** A message row for `try_from_identity(seccomp(2))`.

### P3-5 — Windows own-process coverage is through a child's identity, not `current_user`
- **Location.** `sandbox_rows.rs:806`.
- **Gap.** The brief's "own-process AppContainer refuses `current_user`" is exercised as `try_from_identity(hold.identity)` on a child. Windows `own_identity()` running inside an AppContainer is not exercised.
- **Status.** The rows are still unrun (only the FFI part was type-checked) until the receipt `p2-3a-20261031-a`, which I could not see.
- **Correction.** Accept as is, recording this, or add a child-process row that runs `current_user` inside the container.
- **Closure.** The Windows receipt green with the AppContainer row listed as passed.

## 2. Invariant analysis
| Invariant | Result |
| --- | --- |
| A probe that cannot complete refuses | Holds. Missing status, no `Seccomp:` line, a non-numeric mode, missing `ns/user` or `ns/mnt`, a reaped pid, a missing symbol, and a negative answer all give `Unreadable` (rows plus mutations). |
| Status parser cannot be forged | Holds. The first `Seccomp:` line is used. The kernel escapes newlines in the only field the peer controls (`Name`); fixture 3 shows `Name:\t\nSeccomp:\t0` and the peer is still refused `Seccomp`. |
| Namespace read for only one side | Refuses (`same_namespaces` compares the `Option`s; decision-table rows). In production both sides are always `Some` on Linux, because `probe_dir` errors otherwise. |
| TOCTOU inside the hold window (take → probe → confirm) | Sound. A pidfd plus start time catches exit, reap or reuse during the window. An exec changes nothing the probe reads (seccomp and namespaces survive exec). |
| TOCTOU before the hold (connect or listen → take) | **Broken on Linux (P2-1).** Sound on macOS (the token version is validated). Windows has the same pattern, but a sandboxed process cannot make the connection (pipe DACL and integrity), so I judge it lower risk; not verified by me. |
| Own-process refusal | Structural. `current_user` is the only production constructor (the source check enforces this) and it runs `of_self`. No production client connect or server start exists yet (2.3d and 2.3k), so "both build it" is an obligation on later steps, which the constructor rule enforces. |
| Listener reads nothing, then one SessionError, then EOF | Holds for sandbox refusals (wire rows; the fixture clients decode exactly one frame). A probe error flows through the same `turn_away`. |
| Any path errs OPEN | P2-1 (pid reuse) only. P3-1 and P3-2 are accepted limitations, not error-open paths of the probes as specified. |

On the drafter's notes:
- **Budget:** a process matter, not a defect.
- **Windows partly type-checked:** open until the receipt.
- **Probes blind to Landlock, LSM profiles, cgroup and capability limits, and seccomp on a non-first thread:** consistent with the design's enumerated rule. The thread case is confirmed by my fixture, and it is cheaply improvable (P3-1).
- **macOS answer undocumented:** my probe shows the token form validates the pid version and answers non-zero for a stale token, so it fails closed.
- **Pipe DACL keeps AppContainer clients out:** plausible and consistent with the 2.1 DACL. Not verified (no Windows).

## 3. Risks and next action
- **Next action.** The lane owner decides on P2-1 (widen 2.3a to `Hold::take`, or a 2.1 remediation) and Q6 for P2-2. Then a revision, and a re-review against the closure tests above.
- **Deployment cost (design-accepted).** Under Docker's default seccomp profile, or any containerised CI, the own process is refused. 26 of 110 carrier rows (the 2.1 rows included) fail there through `local_rows.rs:73`'s `expect`, without naming the missing capability. Any future container CI leg needs this recorded.
- **Kernels without `CONFIG_SECCOMP` or `CONFIG_USER_NS`:** every peer and the caller are refused (stated). Non-dumpable peers are refused as `Unreadable` (stated).

## 4. Out-of-scope observations
- **Windows `ProcessHold`** (`windows.rs`, 2.1) has the same "pin at check time" pattern as P2-1. The design wants the creation time to precede the connection. I judge it low impact because a sandboxed Windows process cannot open the pipe. Recorded for the 2.1 owner; not verified.
- **Later steps that daemonize:** a server that calls `listen()` in a process that then exits (fork after listen) will be refused by every client, whether through 2.1's hold or through the `SO_PEERPIDFD` correction. Server start (2.3f and 2.3k) must listen in its final process.
- **`REQUIRED_CALLS`** in `check_carrier_peer_rule.py` is a substring check. It cannot tell whether the probe's result is used. The `peer-unprobed` and `caller-unprobed` mutation rows cover that today.
