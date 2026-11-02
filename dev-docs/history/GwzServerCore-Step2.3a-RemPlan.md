# Step 2.3a (peer sandbox probes) — remediation plan, round 1

**Object:** gwz-core `9261b0ef` (lane p2-3a, merged with root at `f2b1bc71`).

**Reviews:**
- `GwzServerCore-Step2.3a-ReviewCode.md`: GO, conditional on the Windows receipt; P3 C-1 to C-4.
- `GwzServerCore-Step2.3a-ReviewState.md`: NO-GO, P2-1 (ARCHITECTURAL, inherited from 2.1) and P2-2, plus P3-1 to P3-5. It pre-commits to GO on P2-1 and P2-2 closed.

**Blind convergence:** Code C-3 = State P3-4 (a sandboxed caller is told "the peer" is sandboxed). Code C-2 (the probe is not bound to the hold by type or row) is the structural neighbour of State P2-1.

**Architectural root causes on this object:** one (State P2-1).

**Operator decisions (2026-10-31):**
- "Widen 2.3a to fix it": the Linux hold is taken from `SO_PEERPIDFD`, refusing without it;
- Q6 "Privileged step in CI": hosted CI enables unprivileged user namespaces and runs the mount-namespace row with sudo; the fail-don't-skip rule is kept.

The fix is one patch, a new commit in the lane. Round 2 goes to the same two reviewers.

| ID | Disposition | Closure test |
|---|---|---|
| State P2-1 (ARCH) | Fix. The Linux `Hold::take` takes the pin from `getsockopt(SO_PEERPIDFD)` on the connected socket (connector at connect, listener at listen), never `pidfd_open(SO_PEERCRED.pid)`. `confirm` keeps `pidfd_send_signal(0)` and the start-time check on that descriptor. Where `SO_PEERPIDFD` is unavailable (kernel before 6.5, ENOPROTOOPT or EINVAL), refuse `Unreadable` with a cause naming the kernel requirement. State this in the design's limits and in the README. Ownership widens to `unix.rs` `Hold` (`take`/`confirm`) and its 2.1 callers | The reviewer's `review_pid_reuse_*` rows (client and listener, root plus `ns_last_pid`, failing with a named capability otherwise) are refused `Unreadable`. A mutation back to `pidfd_open(pid)` turns them red. A row: a kernel or socket without `SO_PEERPIDFD` (injected `getsockopt` failure) refuses with the kernel cause |
| State P2-2 (Q6) | Fix. In `.github/workflows/transport-candidate.yml` (the Linux leg), before the suites: `sudo sysctl -w kernel.apparmor_restrict_unprivileged_userns=0` (and `kernel.unprivileged_userns_clone=1` where present). Run the root-needing mount-namespace row (and any `ns_last_pid` row) in a separate step under `sudo -E` with the same target dir, by an exact filter. The non-root suite keeps them failing loudly if run without privilege | A hosted transport-candidate run of the lane commit is green, with the namespace rows listed as passed |
| Code C-1 | Fix. The peer guard also scans `\bcfg!\s*\(` with the same allow-list. Unit tests for `cfg!(debug_assertions)` and `cfg!(not(test))` in `local/sandbox/linux.rs` | Mutation M-a fails the guard |
| Code C-2 | Fix. `Hold::confirm(self, …)` consumes the hold, so no probe can follow it (compile-enforced). `of_peer_with(resolve, &hold)` gets a resolver row recording that the hold's token reached `sandbox_check_by_audit_token` | M-c is red in the resolver row; M-d fails to compile |
| Code C-3 = State P3-4 | Fix. The own-process refusal and the `of_self` `Unreadable` texts carry no "peer" subject (for example "this process is sandboxed by …") | A row: `current_user`'s Display under seccomp does not contain "peer" |
| Code C-4 | Fix. The `ProcessHold.identity` field stays private, with a `#[cfg(test)]`-scoped accessor through the guard-allowed form; `judge` stays re-exported only under `all(test, windows)` | Guard green; Windows receipt green |
| State P3-1 | Fix (cheap). Read every `/proc/<pid>/task/*/status` inside the hold window, and `/proc/self/task/*` for the caller; any thread with a non-zero `Seccomp` mode refuses | The reviewer's second-thread fixture is refused `Seccomp` as client and as own |
| State P3-2 | Fix (doc). The Linux limits state "sandboxed after the check" (seccomp or unshare after connect; a sandboxed child inheriting the descriptor) | Text review |
| State P3-3 | Fix. On a non-zero `sandbox_check` answer, run the hold's confirm before naming `Profile`; a failed confirm gives `Unreadable` | A resolver row: stub answers 1 while the hold reports the process gone gives `Unreadable` |
| State P3-5 | Record. The Windows own-process row runs `try_from_identity` on a child's identity; `current_user` inside an AppContainer is not exercised. Accepted, with this recorded in the README | — |
| State §4: Windows `ProcessHold` pins at check time | Record for the 2.1 owner, with low impact (the pipe DACL and integrity rule keep sandboxed processes out) | — |
| State §4: daemonizing servers | Record for 2.3f and 2.3k: server start must `listen()` in its final process | Carried into those briefs |
| Code §4: Windows restricted tokens and job objects not refused | Record as a design limit (Windows analogue of Landlock); consider `TokenHasRestrictions` in a later step | — |
