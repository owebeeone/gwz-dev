# Step 2.3a (peer sandbox probes) — remediation plan, round 2

Status: **accepted at lane root `0ce82044`, gwz-core `631ee1d8`, gwz-core-evidence `77e7333c` after
`GwzServerCore-Step2.3a-ReviewCode-3.md` and `GwzServerCore-Step2.3a-ReviewState-3.md` reported GO; this accepts step 2.3a
(the peer sandbox rule's platform probes and the `SO_PEERPIDFD` hold) only.** Gates on `631ee1d8`: WSL
`p2-3a-remote-20261031-d` PASS (Linux 6.18); Windows receipt `p2-3a-20261031-d` PASS. Rounds: 2 remediation rounds; one
architectural root cause (State P2-1, round 1). Landed in root at gwz-core `101ebff5` (pushed 2026-10-31). State P2-2 closed by hosted
CI: transport-candidate run 41184699893 at `101ebff5` passed; its root step printed `kernel 7.0.0-1017-azure: 2 of the 2
root-only sandbox rows passed`, built nothing as root (finished in 0.12 s), and the non-root suites reported exactly those
two rows skipped. Recorded P3s, not fixed: Code C3-1 (the guard admits a `cfg_if!` `test` arm with an
`else`), State R3-1 (`sandbox.rs`'s per-thread sentence should name the exec residual). Both are carried to the Phase 2 review
(2.6). The 6.5 to 6.15 EINVAL answer is shown by emulation only; a real host run replaces it when one is available.

**Object:** gwz-core `9261b0ef` plus remediation round 1 `d0e92a5a` (lane p2-3a).

**Reviews:**
- `GwzServerCore-Step2.3a-ReviewCode-2.md`: NO-GO, P2 C2-1, P3 C2-2 to C2-5. It pre-commits to GO on C2-1 closed.
- `GwzServerCore-Step2.3a-ReviewState-2.md`: NO-GO, P2 N-1, P3 N-2 to N-5. It pre-commits to GO on N-1 closed.

**Blind convergence:**
- Code C2-1 = State N-1: `SO_PEERPIDFD`'s EINVAL is read as "kernel before 6.5", but on 6.5 to 6.15 it means the peer has been reaped. Both reviewers found it separately in the kernel source (`pidfd_prepare`) and confirmed it with an emulated reaped-peer row.
- Code C2-3 = State N-5: `--skip-exact` accepts names that skip whole modules.

**Architectural root causes on this object:** one, State P2-1 from round 1. Both reviewers classify every round-2 finding as not architectural.

**Gates on `d0e92a5a`:**
- WSL `p2-3a-remote-20261031-c`: PASS, kernel 6.18.
- Windows receipt `p2-3a-20261031-c`: PASS, 4366 passed. Its only failures are the 3 named Python gaps, so Code C-4 is closed.

**Hosted kernel:** the State reviewer reports that today's ubuntu-24.04 image runs `7.0.0-1017-azure`, so N-1 does not show there. That is a 6.16-or-later kernel. No 6.5 to 6.15 host is in the current gates.

**Correction of round 1:** RemPlan round 1's P2-1 row said "kernel before 6.5, ENOPROTOOPT or EINVAL". This plan supersedes that clause: only ENOPROTOOPT means the option is missing.

**Provenance of `--skip-exact`:** the lane owner decided, after the round-1 first draft, to keep ambient capabilities out of the non-root suites. The option exists for that decision, under Q6 ("privileged step in CI"). It is named here, since round 1 named it only in the evidence README.

The fix is one patch, a new commit in the lane. Round 2's re-verdicts go to the same two reviewers, whose context is intact. This is the second and last remediation round; a third round may carry only non-architectural corrections.

| ID | Disposition | Closure test |
|---|---|---|
| C2-1 = N-1 (P2) | Fix. In `unix.rs` `take_with`: only `ENOPROTOOPT` maps to `Unreadable(KERNEL_REQUIREMENT)`. `EINVAL` and `ESRCH` map to `Unreadable("a live peer process")` (GONE). `ENODATA` and anything else stay "the peer's process descriptor". Correct the kernel-requirement text in `local/mod.rs` and `sandbox.rs` to say that a kernel before 6.5 answers ENOPROTOOPT, and that 6.5 to 6.15 answer EINVAL for a peer that has gone. Correct the PID-namespace sentence in `sandbox.rs`: such a peer is refused as `a live peer process`, not "the same way" | The kernel row (`hold_rows.rs`): ENOPROTOOPT names the kernel requirement; EINVAL and ESRCH give GONE, with no "Linux 6.5" in the text. A new emulated row: a real reaped connector and a real reaped listener through `take_with`, with an `ask` that answers EINVAL as 6.5 to 6.15 do, are each refused GONE. The mutation that maps EINVAL back to the kernel cause turns both rows red |
| C2-2 (P3) | Fix. `check_carrier_peer_rule.py` gives `cfg!(..)` in expression position its own allow-list, empty for every carrier file. The Android condition is allowed only as a `cfg_if!` or attribute condition. Invert the unit test that asserts `cfg!(test)` in windows.rs is clean, and add `!cfg!(test)` cases for peer.rs, windows.rs and mod.rs | G1 (`if !cfg!(test) { return Ok(()); }` in `PeerPolicy::verify`) and G2 (`if cfg!(target_os = "android") { return Ok(peer); }` in `check_peer`) each fail the guard |
| C2-3 = N-5 (P3) | Fix. `run_tests.py --skip-exact NAME` checks each NAME against the test binary's `--list` for the group. NAME must equal exactly one listed test and be a substring of no other listed test name, or the run exits non-zero before any test runs. Keep one semantics in both paths: an exact exclusion | Unit rows: `::`, `a::`, `transport_host::`, `transport_host::carrier`, a name that matches no listed test, and a name that is a substring of another listed test are each refused; a full single-test name is accepted |
| C2-4 (P3) | Fix. The guard requires `hold.confirm(` in `unix.rs` and forbids `.check(` inside `check_peer`, by a token scan of that function's body | Md2/G4 (`hold.check(fd)?` before the probe, `confirm` dropped) fails the guard |
| C2-5 (P3) | Fix. Add `task/12/status` as a directory (EISDIR) to the fake-`/proc` row, expecting `Unreadable("the process's threads")` | Mutation Mt (every thread-status read error treated as vanished) turns the row red |
| N-2 (P3) | Fix. After the thread scan, `probe_dir` re-reads the leader's `status` and takes its mode into the maximum, so a filtered non-leader thread that execs mid-scan is still seen. Keep the `sandbox.rs` sentence | The reviewer's exec-race row (root not needed, a bounded trial count of about 150, all trials refused or showing a filter, 0 clean scans while the hold checks). Removing the re-read turns it red (the reviewer measured about 20% misses per trial) |
| N-3 (P3) | Fix (row). `take_with(fd, own_pid, uid, ask)` where `ask` returns `pidfd_open` of a live child (a descriptor naming a different live process) is refused GONE | Deleting the `descriptor_pid(&pidfd) != Some(pid)` clause turns the row red |
| N-4 (P3) = Code §4 macOS stale token | Record. The macOS `Hold::check` cannot see pid reuse, so a replaced peer is refused (fail-closed) but named `Profile`. State this limit in `check`'s doc and in the evidence README. A pid-version check (token word 7 against the live process) is a hypothesis to evaluate in a later step, not part of this patch | Text review |
| Code CI note | Fix. The root step prints `uname -r` beside its result | Text review; the hosted run shows the kernel |
| Code §4: Windows own-token errors use `LocalRefusal::Peer` (`windows.rs:170`, `:293`) | Record (unreachable except on a malformed token) | — |
| State §4: parked design still states the pre-6.5 fallback | Record; the lane does not edit the parked design | — |

**Not changed by this patch:** the `Hold` interface, `check_peer` order, the `Cause` list, the workflow beyond the `uname -r` line, and every round-1 closure.

**Gates for the revision:**
- Mac carrier rows and the Linux container rows (root and uid 1000), with every closure mutation above red;
- the peer guard and its unit tests, `test_run_tests_skip_exact.py`, and the source checks;
- the WSL gate and Windows receipt `p2-3a-20261031-d`, `--cache-from p2-3a-20261031-c`;
- after landing, the hosted transport-candidate run closes State P2-2 from round 1.
