# Cancellation design B r1 — Safety-AXIS REVIEW

**Review object:** /Volumes/projects/limbo/gwz-dev/dev-docs/GwzChildLifetimeDesign-DRAFT.md (hash at start `48b992d945f9570730103ae80f5c4a438dcaf504`, at end `48b992d945f9570730103ae80f5c4a438dcaf504`, unchanged) | **Date:** 2026-11-01 | **Axis:** Safety: what the text allows to go wrong, including orphans, wrong-process kills, stale locks, partial state, misreports, races, Windows job and console behaviour, forced-exit paths, and how bounded each residual is.

**Verdict: NO-GO** — 0 P0, 0 P1, 5 P2, 10 P3. None of the root causes is architectural. Each blocking finding has a bounded correction in the design text, a platform module or an operator decision's cost statement. I pre-commit to GO on a revision that resolves P2-1 to P2-5 as specified below. Each P3 must either be resolved or recorded as an explicit deferral with its owner.

## 0. Evidence base

- **Pin.** I ran `git hash-object` on the object at the start and at the end, and both gave `48b992d9…`. The content did not move during the review.
- **Controlling decision.** `GwzTransportReleaseConvergencePlan.md` §10 D-V, :999-1056 (all bullets): OQ2 reversed (:1005-1010); resolutions (1)-(3) (:1032-1045); the split (:1046-1054).
- **Source history (not authority).** `history/GwzCancellationAmendment-ReviewSafety.md` (round 1, all findings), and r1 blob `3448c561` lines :146-150, :244-247 and :256-259.
- **Code read, at the design's stated pins:**
  - **gwz-core `b745f1fa`:**
    - `https_auth/process_tree.rs:1-112`, `process_tree/windows.rs:50-122`, `windows/launch.rs:1-12, :215-230`
    - `owner.rs:245-310`, `https_auth.rs:26-30, :185-193`, `https_worker.rs:372-380`
    - `spawn_gate.rs:1-40, :148-175, :420-460`, `transport_host/composition.rs:20-45`
    - `operation/commit_log/mod.rs:326-362`, `gitbackend/backend.rs:60-128`, `Cargo.toml:45-52, :128-155`, `src/lib.rs:45-52`
    - every non-test `Git2Backend::new()` and `without_credential_helpers` site in `src/`
    - `carrier/tests/handshake_local_rows.rs:445-449`
    - the existence of `scripts/checks/{check_spawn_gate_minting.py, check_process_globals.py, process_globals_allowlist.json}`
  - **Other members:**
    - gwz-cli `c0ba717`: `src/lib.rs:140-200`
    - gwz-git `9bc4ea9`: the `ProcessGate` and `.output(` sites
    - gwz-py `2185cc6`: the count of `Git2Backend::new()` in `native/src` (8, matches)
- **Toolchain sources read:**
  - Rust std 1.95.0: `sys/pipe/unix.rs:20-45`, `sys/process/unix/unix.rs:135-150, :336-410, :455-470`, `os/unix/process.rs:212-224`, `os/windows/process.rs` (the unstable markers)
  - tokio 1.53.1: `process/unix/mod.rs:115-140`, `net/unix/pipe.rs:394-1042`
  - Go 1.26.4: `src/os/wait_waitid.go:1-30` (local Homebrew install)
- **Documents spot-checked:**
  - parked server design :806-810, :1210-1226
  - 2.3 plan :200-214
  - plan §4 3.1-3.5 (:714-740)
  - `build-targets/winflk-logs/f9-report.md:140`, found at the workspace root (`/Volumes/projects/limbo/build-targets/`), not under gwz-core
- **Platform facts relied on (stated, not measured here):**
  - SIGTTIN goes to the reading process's whole process group.
  - POSIX orphaned-process-group rule: SIGHUP and SIGCONT go to a newly orphaned group that has a stopped member.
  - git's tempfile/lockfile code removes its `*.lock` files on SIGINT, SIGHUP, SIGTERM, SIGQUIT and SIGPIPE through `sigchain_push_common`.
  - Under `vm.overcommit_memory=2`, `fork` must commit the parent's private writable memory. CPython's switch of `subprocess` to `vfork` (bpo-35823) was made for this reason.
  - macOS `setrlimit` rejects an infinite `RLIMIT_NOFILE` soft limit, but the default hard limit is `RLIM_INFINITY`.
  - Linux `close_range` needs 5.9 or later. macOS has none.
  - `CreateProcessW` with `bInheritHandles=TRUE` and no handle list hands the child every inheritable handle.
- Nothing was built or run. No file was modified.

## Problem, requirement provenance, and scope

The design gives gwz-core one child registry and, on macOS and Linux, a forked lifeline reaper. On Windows it gives each entry a kill-on-close job. The aim is that every child gwz-core starts ends no later than its operation and no later than gwz, without a kill ever reaching a reused process ID.

**Provenance of each mechanism:**

| Mechanism | Traces to | Assessment |
|---|---|---|
| Registry, reaper, fork check, Phase 2 sub-step | D-V (2), (3) | Traced |
| Reaper launch contract, child-side registration, zombie pinning | Demonstrated necessity (round-1 P2-1 and P2-2 interleavings) | Traced. The pinning primitive is unsound on macOS (P2-1) and under external reapers (P2-2). |
| A Windows job for every entry | Necessity for B1 (`TerminateProcess` of gwz orphans `rev-list` and `git commit` today) | Traced. The creation route leaves an unlisted window (P3-1). |
| Moving the helper off `tokio::process` | Necessity for B4 | Stronger than the text states. tokio's own reaper (pidfd on Linux, `tokio/src/process/unix/mod.rs:123-134`, or SIGCHLD plus `waitpid(pid)`) reaps independently of the waiter, so a tokio child can never be zombie-pinned. The design gives only the SIGCHLD handler as the reason. |
| `Process`, `Session`, `OwnInput` and `Detach` targets | The D-V split (B gives C a spawn interface) | Traced. `Detach` widens the blast radius on purpose: it starts a process that outlives gwz. It is fenced (Unix only, forgotten at once) and listed (R-B11). |
| OD-B3: a reaper for every command | Necessity, to close the macOS lifeline window | Traced. Its cost is priced for gwz-cli only (P2-5). |
| The change to `terminate`'s outcome (f9) | Not D-V and not B1-B7. It comes from the f9 Windows report. | Scope creep into A's "what a cancelled member reports" (P3-8) |

**Silent narrowing of the operator's requirement:**
- B4 holds on Linux only. The macOS waiter (P2-1) and gwz's own in-process kills under an embedding application's reaper (P2-2) void it, and neither is listed.
- B5 and D-V's "no stale lock" fail on every orderly exit that leaves a `Wait` entry running (P2-3). D-V's residual exception (1) covers only SIGKILL, a crash or power loss.
- OD-B1's stated cost leaves out the loss of interruptibility (P2-4).

**Surfaced, not silent:** R-B13 shipping (OD-B2) and every other §7 residual except those named in P2-3, P3-1 and P3-5.

**Out of scope, as directed:** the operator's choices themselves (OD-C1 to OD-C6, resolutions (1)-(3)) are not attacked. Their shape and stated costs are.

## 1. Findings

### P2-1: On macOS the waiter's `waitid(P_PID, pid, WEXITED | WNOWAIT)` also returns for a stopped child, so the pinning order misfires exactly in R-B10's case

- **Location:** §3.3 "Order"; §3.6 `git commit` row; R-B10.
- **Violated invariant:** §3.3's order (a `Wait` entry is retired only when its leader exits; the waiter observes exit without reaping); B1; B4's proof.
- **Root cause:** the design uses one wait primitive for both Unix platforms. Go's runtime stopped using exactly this call on Darwin. `src/os/wait_waitid.go:5-6` says: "We used to use this code for Darwin, but according to issue #19314 waitid returns if the process is stopped, even when using WEXITED". Go's file is now `//go:build linux` only. No row exercises a stopped child.
- **Interleaving:**
  1. OD-B1 (a) puts `git commit` in its own group. A signing step (`ssh-keygen -Y sign`, no agent) reads `/dev/tty`, and SIGTTIN stops git's whole group (R-B10).
  2. On macOS the waiter's `waitid` returns for the stop. The design treats that as the leader's exit.
  3. The implementation then goes one of two ways:
     - (a) It believes it: the `Wait` entry is retired, so the unregister record is written and the reaper's coverage of git is dropped while git lives. `waitpid` then blocks until git really exits. If the user gives up and SIGKILLs gwz, the reaper no longer covers git. What ends git and the hook is the kernel's orphaned-group SIGHUP, not the design.
     - (b) It checks `si_code` and loops: with `WNOWAIT` the stop is never consumed, so the waiter thread spins at 100% CPU for as long as git stays stopped, which under R-B10 is unbounded.
  4. For a `Kill` entry (a helper stopped on a tty read), `exited()` resolves early and the owner acts on a status that does not exist.
- **Impact:** a false "exited" on one of the two reaper platforms, triggered by the very case the design already names as a cost. It loses reaper coverage or burns a core, and both are diagnosable only by `ps`.
- **Correction:**
  - On macOS, observe exit with a primitive that reports only exit and does not reap, for example `kqueue` `EVFILT_PROC`/`NOTE_EXIT` on the PID (the zombie persists until `waitpid`).
  - Keep `waitid(WNOWAIT)` for Linux only, in the platform module.
  - State in §3.3 that a stop is never an exit.
- **Closure test:** a new row (M, L). A `Wait` child and a `Kill` child are each stopped by SIGSTOP and by a real SIGTTIN under a pty. For 5 s neither is retired or released, no thread uses more than 5% CPU, and the reaper's trace still holds both registrations. After SIGCONT and exit, the normal order resumes. Then SIGKILL the harness while the child is stopped: the group is gone within 1 s, killed by the reaper.

### P2-2: Under an embedding application's reaper, gwz's own in-process kills can reach a reused ID while gwz is alive, and neither R-B1 nor the README says so

- **Location:** §3.3 "Why no kill reaches a reused ID"; §3.8; R-B1; §8 row 7; row B11.
- **Violated invariant:** B4 ("A kill reaches only a registered child or its group").
- **Root cause:**
  - The proof assumes that only gwz reaps its children ("Every kill gwz sends is sent while the entry is live, so its ID is pinned by a zombie").
  - An application thread calling `waitpid(-1)` can reap the zombie after the waiter's `WNOWAIT` call has already returned success. That loss cannot be detected: no `ECHILD` is ever seen.
  - The design treats external reaping only as a window that matters when gwz dies (R-B1), and the README text says it "weakens that protection".
  - Nothing says that `Child::kill()`, the release kill of a non-retired `Kill` entry, or `kill_killable()` are suppressed once pinning may be lost.
- **Interleaving (gwz-py, gwz alive):**
  1. A helper (`Kill`, `Group`) exits. The waiter's `waitid(WNOWAIT)` returns.
  2. An application thread's `os.waitpid(-1, 0)` (for example asyncio's `FastChildWatcher` on Python 3.11 or earlier, or a daemon's catch-all reaper) reaps it.
  3. The PID and group ID are free, and the PID wraps (macOS PIDs are sequential up to 99999).
  4. The lookup's owner releases the entry. The waiter, which believes the zombie is pinned, sends `kill(-pgid, SIGKILL)` and kills an unrelated job.
- **Impact:** gwz itself SIGKILLs an unrelated process group while running. That is worse than R-B1, which needs gwz to die inside a microsecond window, and it is presented to users as a mere weakening.
- **Correction:**
  - (a) State in §3.3, R-B1 and the README that any process-wide reaper (`waitpid(-1)`, `wait()`, SIGCHLD `SIG_IGN`) voids B4 for gwz's own kills as well, not just the reaper's. The README should say plainly that this is unsupported, not that it "weakens" protection.
  - (b) Make every gwz-side kill go through the waiter, which sends it only if a fresh `waitid(P_PID, pid, WEXITED | WNOHANG | WNOWAIT)` succeeds or shows the child still alive. If that returns `ECHILD`, the kill becomes a no-op.
  - (c) At creation, if SIGCHLD is `SIG_IGN` or has `SA_NOCLDWAIT`, refuse group kills, or the registry entirely, with a named fault.
  - Record the leftover race (b) cannot close as a separate residual for D.
- **Closure test:** extend B11 (M, L). A harness thread reaps with `waitpid(-1)`. The owner then calls `Child::kill()` and releases a non-retired `Kill` entry. A trace of every `kill(2)` from the registry, gwz side and reaper side, shows no signal sent after the external reap. Repeat with SIGCHLD `SIG_IGN` and with `SA_NOCLDWAIT`.

### P2-3: Every orderly exit that leaves a `Wait` entry running SIGKILLs `git commit` mid-write, and only `close()` is guarded

- **Location:**
  - §3.4 "Its end": "An exit without destructors closes the write end in the kernel, with the same result"; `close()` and the last drop "kill every live entry (`Kill` and `Wait`)".
  - §3.2 Windows: the end closes the job.
  - §3.7 "To A": the guard is only that A calls `close()` "only after the last write sequence has ended".
  - §3.8; §7 R-B4; plan 3.2 ("module shutdown and the atexit join close hosts through the cleanup domain").
- **Violated invariants:** B5 ("not killed while gwz lives", and an orderly exit is not one of D-V (1)'s tearing events); D-V's "no stale lock"; residual exception (1) covers only "SIGKILL, a crash or power loss".
- **Root cause:**
  - The design makes the end of the process the same as `close()`, and `close()` kills `Wait` entries.
  - The only stated precondition is on the call to `close()`. None is stated on process exit or on the last drop.
  - Orderly exits that run with a `Wait` entry still live are:
    - returning from `main` while a worker thread runs;
    - `std::process::exit` on another member's error;
    - a gwz-py application's `sys.exit()` with a daemon thread mid-commit;
    - gwz-py's own 3.2 atexit close of the host.
  - On 1.0.18 the `git commit` child was in gwz's group, survived such an exit and finished.
- **Interleaving:**
  1. A gwz-py application runs `client.commit(...)` on a daemon thread. A sleeping `pre-commit` hook runs while git holds `index.lock`.
  2. The main thread returns, Python finalizes, and either 3.2's atexit closes the host (`close()`) or the process ends.
  3. The reaper reads end of file and sends SIGKILL to git's group (on Windows the job closes).
  4. `index.lock` is left behind. The next `git commit` fails with "Another git process seems to be running".
- **Impact:** a stale lock and a half-run hook on an orderly exit. These are neither listed in §7 nor recognized by any residual condition (R-B4 says "SIGKILLed or crashes").
- **Correction:**
  - State the invariant as a requirement on every composition owner. No orderly exit, `close()` or last drop happens while a `Wait` entry is live: A for gwz-cli, step 3.2 for gwz-py (its atexit hook waits for `Wait` entries before closing).
  - Make `close()` refuse, or wait, while a `Wait` entry is live rather than kill it.
  - Add R-B4's sibling row for the paths that cannot be guarded (`os._exit`, a foreign `exit()` from C code), with the `*.lock` recognizer.
- **Closure test:** two new rows (M, L, W):
  - a gwz-py script whose daemon thread commits with a sleeping hook calls `sys.exit(0)`: the commit completes, or the exit waits for it, and no `index.lock` remains;
  - gwz-cli's `std::process::exit` path with a live `Wait` entry gives the same result.

### P2-4: OD-B1 (a)'s stated cost leaves out that a hung hook or signer makes `gwz commit` impossible to end without SIGKILL, and the Windows console loss

- **Location:** §6 OD-B1; §3.6 (`Wait`: "never killed by a cancel or by `kill_killable`"; waited for "by the forced exit"); R-B10; row B15.
- **Violated invariant:** the operator decision must state its true cost. Round-1 P2-6 also set "never worse than today": on 1.0.18, Ctrl-C ends a hung `git commit` and its hooks at once.
- **Root cause:**
  - With git in its own group, the terminal's SIGINT reaches only gwz, and every cancel and the forced exit wait on the `Wait` entry with no bound.
  - The cost text names only terminal readers stopped by SIGTTIN. It omits:
    - every hook that hangs for any other reason (a `pre-commit` test suite that deadlocks, a network call, a smartcard touch for `gpg`);
    - on Windows, that `CREATE_NO_WINDOW` removes the console from git and its hooks, so console prompts fail where 1.0.18 prompted.
  - The cost text also does not weigh that git handles SIGINT itself (its tempfile/lockfile code removes its `*.lock` files and re-raises). An interrupt delivered to git is therefore cleaner than the reaper's SIGKILL, which is what R-B4 records.
- **Interleaving:** `gwz commit`; the `pre-commit` hook hangs on the network. First Ctrl-C: A cancels and waits. Second Ctrl-C: the forced exit waits too (§3.6). The user's only way out is `kill -9 gwz`. The reaper then SIGKILLs git and leaves `index.lock` (R-B4), the worst outcome of all the options.
- **Impact:** a process that only SIGKILL can end, a regression from 1.0.18. The operator is asked to decide on an incomplete cost.
- **Correction:**
  - Restate OD-B1's cost to include hung hooks and signers, the forced exit's unbounded wait, and Windows console prompts.
  - Add an option (c): `Wait` entries are not interrupted by the first cancel, but the forced exit (or a third interrupt) sends SIGINT to git's group, so git removes its own locks. On Windows, because there is no console, `TerminateJobObject` follows after a bound, and the residual is recorded.
  - Let the operator choose.
- **Closure test:** a B15 variant (M, L) with a `pre-commit` hook that never exits. Under (a) as decided, the row documents that only SIGKILL ends gwz and that `index.lock` remains. Under (c), the forced exit ends gwz within its bound and no `*.lock` remains. W: the console-prompting hook's outcome is recorded.

### P2-5: The reaper's cost and failure modes in an embedding process are not stated: copy-on-write retention, refusal of every spawn under strict overcommit, and OOM selection

- **Location:** §3.4 "Creation"; §3.8; OD-B3; §4 step 3.4 ("after the parent writes 100 MiB (M, L)"); §8 row 7; R-B2.
- **Violated invariants:** the operator decision must state its true cost (D-V (2)'s forked reaper is priced against gwz-cli only); compatibility with 1.0.18 for gwz-py.
- **Root cause:**
  - The reaper is a fork of the host process. In gwz-py that is the Python interpreter, which may be gigabytes when the module-level composition object is made (plan 3.2: at import).
  - The reaper keeps every page the parent later modifies, so its unique resident set grows toward the parent's size at the fork, for the process's whole life.
  - Under `vm.overcommit_memory=2` the fork must commit that memory and can fail with `ENOMEM`. The design then sets `ReaperGone` from creation, and every entry spawn is refused: the helper, `rev-list` and CLI-route commits.
  - Linux's OOM killer counts shared copy-on-write pages in each process's RSS, so the reaper looks as large as its parent and may be chosen first. That opens R-B2 silently.
  - The 3.4 measurement uses gwz-cli's profile only.
- **Interleaving:** a 6 GiB Jupyter or pandas process imports gwz-py on a host with strict overcommit. The fork fails, and every `client.fetch()` that needs a helper is refused, naming the reaper, where 1.0.18 worked. On a host with overcommit enabled, the same process's footprint drifts toward 12 GiB as the notebook churns.
- **Impact:** gwz-py loses function or doubles its memory in the large-process case. Neither the operator decision nor §7 says so.
- **Correction:**
  - State in OD-B3 and §3.8 the copy-on-write retention bound, the `ENOMEM` behaviour (fail closed, and the exact message) and the OOM exposure.
  - Add a gwz-py measurement to 3.4 (a 2 GiB heap, then 1 GiB written after the fork).
  - List the strict-overcommit refusal as a residual or a constraint in the README text (§8 row 7).
  - Optionally have the reaper raise its own `oom_score_adj` so that it is not chosen ahead of its parent; check that this does not cut the other way.
- **Closure test:**
  - a gwz-py row with a large heap records the reaper's PSS after the parent writes;
  - an injected `ENOMEM` on the fork gives the documented message (B5 already injects a fork failure; extend it to gwz-py and assert the user-visible text).

### P3-1: On Windows, gwz's death between `CreateProcess` and `AssignProcessToJobObject` leaves a permanently suspended orphan holding inherited handles, and this is not in §7

- **Location:** §3.2 Windows; §6 "Choices open to review" (suspended creation versus the helper's job-list launch); §7.
- **Root cause:**
  - A std-built entry exists suspended, outside any job, until the assignment. Its `bInheritHandles=TRUE` creation hands it every inheritable handle of gwz's, including gwz's own inherited stdout pipe.
  - If gwz is terminated in that window (a forced exit's `ExitProcess`, `TerminateProcess` from a CI cancel, or a console close), the process never runs and never exits.
  - `gwz log -- <path>` spawns one `rev-list` per step, so this window recurs thousands of times.
  - Separately, the "other than one thread → refuse" rule fails every spawn closed on hosts whose security software creates a thread in new suspended processes. That is a hypothesis, not observed.
- **Impact:** an unlisted, permanent orphan that can also hold a parent pipeline's write end, so `| tee` or a CI log reader never sees end of file.
- **Correction:** generalize the helper's job-list launch (`windows/launch.rs:1-3`: the job from the first instruction, plus a handle list), which closes both issues and is already proven. Otherwise list the window as R-B14 with its recognizer (a suspended process whose parent PID is dead) and resume every thread of the PID instead of refusing.
- **Closure test (W):** a test seam terminates the harness between creation and assignment, and no process is left. A row with a second thread injected into the suspended child.

### P3-2: The reaper's start time is unbounded by `RLIMIT_NOFILE`, and the text does not say which limit

- **Location:** §3.4 "Creation" and the launch contract ("each up to the limit").
- **Root cause:**
  - macOS has no `close_range`, and neither does Linux before 5.9 (RHEL 8, Ubuntu 20.04).
  - The hard limit on macOS is `RLIM_INFINITY` by default, and containers can set a soft limit near 2^30.
  - The reaper does not read the lifeline until its close loop ends. If gwz dies during that loop, the kill is delayed by the loop's length; with the hard limit on macOS the loop never ends.
- **Correction:**
  - Use the soft limit, capped.
  - Or enumerate the open descriptors with raw `getdents64` on a `/proc/self/fd` descriptor opened before the fork (Linux) and with `proc_pidinfo(PROC_PIDLISTFDS)` into a preallocated buffer (macOS).
  - State the bound.
- **Closure test:** B4 run with `ulimit -n` at its maximum (L without `close_range`, under a container limit of 2^20 or more): the group is gone within 1 s.

### P3-3: The lifeline protocol is under-specified in ways that matter to safety

- **Location:** §3.1, §3.2, §3.3, §3.4.
- **Gaps:**
  - The token's width and whether it is ever reused are not stated. A stale unregister record for a reused token would unregister a live entry ("ignores … a token it does not hold" does not help).
  - Whether `EINTR` is "a read error" that calls `_exit(1)` is not stated.
  - What the reaper does with a register record when its table is full is not stated.
  - Who owns the write end's descriptor is not stated. If the writer thread owns it and panics, the descriptor closes, the reaper reads end of file, and it SIGKILLs every entry, `Wait` included, while gwz lives.
- **Correction:**
  - Tokens are 64-bit and never reused for a registry's life.
  - `EINTR` is retried.
  - A full table makes the reaper `_exit` (it kills nothing) so that gwz sees `ReaperGone`.
  - The write end is owned by the registry core and closed only by `close()` or process exit. A writer-thread death is treated as `ReaperGone` without closing the descriptor.
- **Closure test:**
  - inject a writer-thread panic: no child is killed, and later spawns are refused;
  - wrap the token counter in a test build: no collision.

### P3-4: The fork check does not cover `Child::kill`, Drop, `kill_killable` and `close` in a forked copy, or come before locks

- **Location:** §3.1 "Fork check"; row B9.
- **Root cause:**
  - "Every call is refused" names registry calls only.
  - Today's `HelperJob::drop` sends `kill(-pgid)` (`owner.rs:288-291`). A forked Python child that finalizes and drops copied handles would SIGKILL the parent's live helper groups.
  - A mutex held by another thread at the fork would deadlock the forked copy before it ever reaches the check.
- **Correction:** the PID check runs first, before any lock, in every method and in every Drop of `ChildRegistry` and `Child`. In a forked copy they are inert.
- **Closure test:** extend B9. The forked child drops a copied `Child` of a live `Kill` entry and calls `kill_killable()` and `close()`. The parent's child keeps running, and the forked copy exits without deadlocking.

### P3-5: Killing gwz and its reaper together (`pkill -9 gwz`, `killall -9 gwz`; `python` for gwz-py) orphans every registered child, and this is unlisted

- **Location:** §7 (R-B2 covers only "the reaper was killed alone").
- **Root cause:** the reaper is a fork, so it has gwz's (or Python's) process name and is matched by kills by name.
- **Correction:** list it as a residual (the children run on with parent PID 1, recognizable as R-B2 is). On Linux, optionally rename the reaper with `PR_SET_NAME` to reduce exact-name matches.
- **Closure test:** none is needed if listed. Otherwise a row runs `killall -9` on the harness's name.

### P3-6: The gate seam is implicit: the ending is chosen by which gate method is called, and "callers do not change" conflicts with std reaping in `commit_log`

- **Location:** §3.1 "Where it comes from" ("their callers … do not change"); `commit_log/mod.rs:354-356` (`gate.spawn(...).and_then(|child| child.wait_with_output())` on a std `Child`); §4 2.3r-2 Owns (which does list `commit_log`).
- **Root cause:**
  - If `SpawnGate::spawn` still returns a std `Child` so that callers stay unchanged, std's `waitpid` reaps outside §3.3's order (a reap before the unregister record), and B4 is lost for `rev-list`.
  - The text never says how `spawn` maps to `Kill` and `output` to `Wait`. A future write through `spawn` would be killed by `kill_killable()` mid-write.
- **Correction:**
  - The gate's methods return the registry's `Child`.
  - The `Spec` is named at each call site (or by explicitly named gate methods).
  - Delete "callers do not change".
  - The process-globals check fails on std `Child::wait*` applied to a registry spawn.
- **Closure test:** B17 gains a negative fixture that uses a std `wait` on a gated spawn.

### P3-7: The reaper kills `Session` entries by PID only, so a `Session` entry's group members survive gwz's death

- **Location:** §3.4 "Loop" (`kill(pid)` for `Process` and `Session`).
- **Root cause:** a session leader leads a process group as well (pgid = pid). Unlike `Process`, nothing stops `kill(-pid)`, so the asymmetry is unexplained. A `Session` `Kill` or `Wait` entry's descendants are orphaned, contrary to B1.
- **Correction:** use `kill(-pid)` for `Session` entries, or state why not and list the residual.
- **Closure test:** a B24 variant. A registered `Session` child with a grandchild; at the harness's SIGKILL both are gone within 1 s.

### P3-8: The change to `terminate`'s outcome is not traced to D-V or B1-B7, it overlaps A's scope, and B16's 30 s bound has no source

- **Location:** §3.6 "Ending never changes the outcome (f9)"; row B16; §8 closing paragraph.
- **Root cause:**
  - The source is the f9 Windows report, not D-V.
  - "What a cancelled member reports" is A's (§0 table).
  - The change hides a still-live tree behind `Cancelled` and `Timeout`, relying on a cleanup report count whose surface is not specified.
  - "Within 30 s" in B16 appears nowhere else.
  - The f9 citation path resolves under gwz-core by the document's own convention, but the file is at the workspace root.
- **Correction:** either move the change to A with its provenance, or record it as an explicitly authorized scope addition. Name where the pending-cleanup count is reported. Source or remove the 30 s bound. Fix the path.
- **Closure test:** none beyond B16 once the bound is sourced.

### P3-9: The reaper's copy-on-write snapshot is an unwiped copy of secrets outside the helper-owner contract's accepted list, unless the fork happens before any credential exists

- **Location:** §3.4 "Creation"; §4 "Interim" (2.3r-2 to 3.3: "each old entry call makes its own registry and reaper"); OD-B2 (b); `process_tree.rs:30-31, :52-56` (values in owners that wipe them; OQ-C lists the accepted copies).
- **Root cause:**
  - A fork taken while another thread holds a live credential, or while freed but unwiped `Command` environment copies are still in memory, freezes them in the reaper for its life, after the parent has wiped its own copy.
  - This holds for gwz-cli's final shape (the fork comes before any thread).
  - It is not stated as a rule, and it does not hold for the interim per-call registries in threaded processes, or for OD-B2 (b).
- **Correction:** state as an invariant that a registry is created before any credential is held in the process. Add the snapshot to OQ-C if the interim is kept, or keep the interim's per-call registries out of threaded gwz-py.
- **Closure test:** a source check that `create` is reached only from the composition's early path (already partly covered by the minting check). A note in 2.3r-2's dual-review brief.

### P3-10: The `OwnInput` end-of-file guarantee depends on std's per-spawn pipes not leaking on macOS, but the Unix gate does not exclude

- **Location:** §3.1 "Entries" ("it ends at its input's end of file"); §3.5 (which analyses the lifeline only); `spawn_gate.rs:20-21` ("On every platform other than Windows the gate does not exclude"); `std/sys/pipe/unix.rs:34-40` (`pipe`, then `FD_CLOEXEC`, on macOS).
- **Root cause:**
  - A registered spawn on another thread can inherit an `OwnInput` child's stdin write end inside std's two-step window.
  - If that spawn is a helper whose retired daemon lives on (R-B6), the `OwnInput` child never sees end of file after gwz's SIGKILL.
  - This is inferred from std's code, not observed.
- **Correction:** on Apple platforms the registry's spawns take an exclusive in-process hold, or §3.5 lists the residual and hands it to C with the condition.
- **Closure test:** a B21 variant (M) with 50 concurrent `Group` spawns while an `OwnInput` child is started. `lsof` shows that no other child holds its stdin write end.

## 2. Verified citations

| Citation | Result |
|---|---|
| Plan §10 D-V :999-1054; OQ2 :1005-1008; resolutions :1035-1043 | Correct (D-V runs to :1054; "Open" is :1056) |
| r1 `3448c561` :148-149, :245-246, :257-258 | Correct |
| Parked :808 (background start, handle list), :809 (stdio child, `setsid`), :1212/:1218/:1224 (prompts, interrupt, kernel closes the pipe) | Correct, within a few lines |
| 2.3 plan :202, :212-213 | Correct |
| `process_tree.rs:42, :57-75, :71, :79-86, :89-90, :21-24` | Correct |
| `owner.rs:253-303`, `:298` (`retain_pending`) | Correct |
| `https_auth.rs:28` (500 ms), `:190`; `https_worker.rs:377` | Correct |
| `process_tree/windows.rs:54-80, :115-118`; `windows/launch.rs:1-3, :224` | Correct |
| `spawn_gate.rs:1-11, :153-155, :431-436, :438-446, :450-454` | Correct. Note `:20-21` (no exclusion on Unix), which bears on P3-10. |
| `composition.rs:30-35, :37-40` | Correct |
| `commit_log/mod.rs:330-358, :337, :347` | Correct (spawn, then std `wait_with_output`; P3-6) |
| `backend.rs:69, :114, :118-125` | Correct |
| `Cargo.toml:50, :132-139, :152` | Correct |
| `src/lib.rs:47-51` | Correct |
| `handshake_local_rows.rs:447` | Correct |
| gwz-cli `lib.rs:160-163` | Correct |
| gwz-py `native/src` `Git2Backend::new()` × 8 | Correct |
| std `unix.rs:141-149, :342, :352-372, :383-385, :408, :463`; `pipe/unix.rs:28, :34-40`; `os/unix/process.rs:220-221`; `os/windows/process.rs:181-182, :342, :445-449` | Correct |
| tokio `process/unix/mod.rs:123-137`; `net/unix/pipe.rs:419, :967` | Correct. `:123-134` also shows tokio's own pidfd reaper on Linux, the stronger reason to leave `tokio::process`. |
| `build-targets/winflk-logs/f9-report.md` | Exists at the workspace root (`:140`), not under gwz-core as the document's path convention implies (P3-8) |
| `scripts/checks/check_spawn_gate_minting.py`, `check_process_globals.py`, `process_globals_allowlist.json` | Exist |
| Not re-verified on this axis | `GwzConnectionApiContract.md:289`, gwz-sspi `launch.rs:205-280`, git2-rs `cred.rs`, gwz-py `README.md:163`, gwz-cli `pager.rs`, `forall.rs`, `hook/*` |

## 3. Risks and next action

- **Highest risk:** the pinning order in §3.3, which carries B4.
  - It is unsound on macOS (P2-1, evidenced by Go's runtime).
  - It is unsound under embedding reapers while gwz lives (P2-2).
  - Both are cheap to fix in the platform module and in the kill path.
- **Second:** `Wait` entries.
  - The reaper and `close()` SIGKILL them on orderly exits (P2-3).
  - OD-B1's cost hides that a hung hook makes gwz unkillable except by SIGKILL, which is also the one path that leaves `index.lock` (P2-4).
  - Git's own SIGINT cleanup offers a route that is better on both counts, and the operator should see it.
- **Third:** gwz-py's forked reaper (P2-5). The cost and failure modes need stating and measuring before 3.2 is briefed.
- **Next action:** revision 2 resolves P2-1 to P2-5 as specified, with the new rows, and resolves or explicitly defers P3-1 to P3-10. The operator re-decides OD-B1 with the corrected cost and option (c), and sees the restated OD-B3 cost. This axis then re-reviews against the new blob.

## 4. Out-of-scope observations

- **Possible defect in existing code, not introduced by B:** `HelperJob::drop` (`owner.rs:288-291`) sends `kill(-pgid)` unconditionally, including in a forked copy of the process. Today this is reachable from gwz-py when a forked Python child finalizes copied objects. P3-4's correction covers it once the helper moves onto the registry.
- **Hand-off to A (not B's defect):** A's forced exit (`ExitProcess` or `_exit`) can land inside a spawn. On Unix B's child-side registration closes that window. On Windows it does not (P3-1). A may prefer to take the gate before `ExitProcess`.
- **Hand-off to C:** the parked design's 10 s wait for `ssh` after `session.close` (:1223) and the `Process` target's PID-only kill leave `ssh`'s `ProxyCommand` to end through its pipes (R-B7). C's proof should include a row for it.
