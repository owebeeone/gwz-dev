# Child-process lifetime: design B of the cancellation split (DRAFT)

**Status: accepted at `4d470544` after the round-3 reviews (-ReviewConsistency-3.md, -ReviewSafety-3.md) and the closure
pass (verifiers GO on every axis, 2026-11-03); this accepts the design text and its clause amendments, not an
implementation.**

**Citations** are at root `6d20ef6f` for documents, gwz-core `b745f1fa` (and `v1.0.18` where named), gwz-cli `c0ba717`,
gwz-py `2185cc6`, gwz-git `9bc4ea9`, gwz-sspi `6f83551`, git2-rs `d13951f`; Rust std 1.95.0 as `std/`
(`library/std/src/`), tokio 1.53.1 as `tokio/`, Go 1.26.4 as `go/`. A path beginning with a member name or "root" is
workspace-relative, any other gwz-core's. In `dev-docs/`: "plan" `GwzTransportReleaseConvergencePlan.md` (D-V :999-1069;
decisions (i)-(iii) :1055-1062; O4, O5 :1063-1069), "2.3 plan" `GwzServerCore-Step2.3-Plan.md`, "parked"
`history/parked/GwzCoreServerDesign.md`, "r1" the cancellation amendment's revision 1 (blob `3448c561`), "Safety" its
round-1 review in `history/`, "A" `GwzCancelInterruptDesign-DRAFT.md` revision 2 (root `6d20ef6f`) by line, "A r3" its
revision 3 in the lane by section; "TR1.6" `gwz-core/dev-docs/history/reference/GwzTransportCredentialHelpersDesign.md`.
**(OS)**: an OS fact a row verifies.

## 0. Scope

Every process gwz-core's child registry starts ends no later than its operation and no later than gwz, however gwz ends,
except §7's residuals. B states the registry (the composition's, passed explicitly), the Unix reaper and Windows jobs
that end registered children when gwz is gone, how each child ends, why no kill reaches a reused ID, and the steps.

| Not in B | Owner |
|---|---|
| The interrupt source, Cancel and Force, safe points, exit status, what a cancelled member reports | A |
| The served form: which child takes which target and ending, the `gwz server stdio`, `ssh` and background server children, the protocol on cancel | C (B gives the spawn interface, §3.7) |
| Temporaries, locks and partial state a killed child or gwz leaves; the operator's list of accepted residuals | D (B hands §7 to D) |

## 1. Requirements

| # | Requirement | Statement and source |
|---|---|---|
| BR1 | No orphan | A registered child ends no later than its operation and no later than gwz (OQ2 reversed, :1005-1008). Not covered, each a §7 residual: children no registry starts (§3.6: R-B5, R-B7 to R-B9, R-B13), the served form's background server (R-B11), registered or tracked children whose cover fails (R-B2, R-B3, R-B14, R-B15, R-B20), and what a retired child leaves (R-B6, R-B12). |
| BR2 | Death is detected | The reaper detects gwz's death, gwz detects a child's and the reaper's (:1007). |
| BR3 | The registry and the reaper | Built as resolution (2) states them (:1039-1043), created by every command (OD-B3, decision (iii)); on Windows every registered child gets a job (Safety P3-3, :256-263). |
| BR4 | Never the wrong process | A kill reaches only a registered child or its group (D-V :1006's "every helper process it started"), except in R-B1's cases: gwz's death between a child's exit and its unregistration (or with entries a dead writer left registered), std's reap after an `exec` failure, and an application's process-wide reaper, unsupported. |
| BR5 | Writes finish | A registered Git CLI route `git commit` or `git tag` (a `Wait` entry), its hooks and signers, run in their own process group and are never killed while gwz lives: not by a cancel, a Force or `close()` (decision (ii), :1059-1060; decision (i), :1056-1059: Force waits without a bound for a local write). Only gwz's death ends one (R-B4; the signal is OD-B4). Not covered: one a gate without a registry starts (OD-B2, R-B13). |
| BR6 | Placement | A Phase 2 sub-step before 2.3j (resolution (3), :1043). |
| BR7 | Cleanup never changes the outcome | Ending a helper's tree returns its lookup's own outcome (A's A6; OQ2's "cleans up"; the f9 report, `build-targets/winflk-logs/f9-report.md` beside the root checkout, "Product classification limitation"). B carries it because 2.3r-2 owns `owner.rs`; A states the member report (A §3.3). |
| BR8 | Rules | No global or thread-local (OD-C1 allows only A's handler); braces; `cfg` only in `cfg_if!` or platform modules; no production code under `tests/`; parity on M, L, W except where §3.2, §3.3, §3.6 and OD-B4 name a mechanism. |

## 2. What exists

| Child | Where it is started | Today on Unix | Today on Windows |
|---|---|---|---|
| Credential helper `git credential fill` | `src/git/endpoint/https_auth/runner.rs:110` → `process_tree.rs:57-75` | `tokio::process` child (`:42`), leader of its own group (`:71`); `kill(-pgid, SIGKILL)` (`:79-86`) on bound, cancel and drop (`owner.rs:253-303`); `retire` leaves its group running (`process_tree.rs:89-90`) | own `KILL_ON_JOB_CLOSE` job from its first instruction via a job list (`process_tree/windows.rs:54-80`, `windows/launch.rs:1-3`), `CREATE_NO_WINDOW` (`:224`); `retire` clears the limit (`windows.rs:115-118`) |
| `git rev-list` (path-filtered `log`) | `src/operation/commit_log/mod.rs:330-358` | `SpawnGate::spawn`, then std's `wait_with_output` (`:354-356`), gwz's group | std spawn, no job |
| `git commit`, `git tag` (Git CLI route) | gwz-git `src/native_commit.rs:88-95`; `src/git/spawn_gate.rs:450-454` | `SpawnGate::output` (`:438-446`), gwz's group, waited for | std spawn, no job |
| gwz-sspi worker | gwz-sspi `src/supervisor/windows/launch.rs:205-280` | none (`src/supervisor/platform.rs:1-9`) | own job (`:276-280`), ended by its supervisor (`handles.rs:50-56`) |
| Native route helpers | git2-rs `src/cred.rs:124, :387-420`, from `src/git/gitbackend/transport_support.rs:313-321` | spawned and waited inside one git2-rs call, gwz's group | the same, `CREATE_NO_WINDOW` (`cred.rs:391-392, :408-410`) |

- These are gwz-core's only production spawns (every other `Command::new` in `src/` is `cfg(test)`). Production uses
  `tokio::process` only at `process_tree.rs:42`; three test files use it (`src/transport_host/https_tests.rs:24`,
  `https_auth/process_tree/windows/tests.rs:505`, `ssh_controlled_server/handler.rs:18-21`). tokio is candidate-only
  (`Cargo.toml:132-139`) and reaps its own children (`tokio/src/process/unix/mod.rs:123-137`).
- gwz-cli's spawns are not gwz-core's: the pager (`gwz-cli/src/pager.rs:157-170`), `forall` (`forall.rs:231-262`), `gwz
  hook`'s utilities (`hook/env.rs:158-348`, `hook/fallback.rs:21-25`); SSPI worker mode exits first (`lib.rs:160-163`).
- **The gate.** The composition (candidate-only, `src/lib.rs:47-51`) makes the process's gate (`composition.rs:30-35`);
  `ProcessGate` exposes only `pub(crate) gate()` (`:20-28`); `spawn_gate()` (`:37-40`) makes one per call for the old
  entries (`local_command.rs:79`, `cancellable.rs:238`) and for `DriverOnly`, the served core's host
  (`driver_only.rs:61`, plan 1.7), whose request backends carry it (`:152`). A hostless backend's is ungated
  (`backend.rs:69, :114`; `with_host_context` is `pub(crate)`, `:118-125`): gwz-cli's (`globalargs/dispatch.rs:93`,
  `log_exec.rs:118`, `hook/family.rs:63, :149, :187`), gwz-py's non-candidate two
  (`gwz-py/native/src/client_host.rs:66`, `route/native.rs:46`) and gwz-core's own
  (`src/workspace_ops/handle_create_repo/create_workspace.rs:18`; in `src/diff/`, `classify.rs:204, :221`,
  `handle_diff.rs:595`, `tagged.rs:63`), which reach none of the three spawn sites:
  `src/workspace_ops/handle_commit.rs:186, :260`, `handle_tag.rs:246` and `handle_log`
  (`src/operation/push_event.rs:546`) use their caller's backend. One host backend does: `NativeHost::backend()`
  (`src/git/endpoint/https_worker/native/sspi.rs:76-80`), on the gate `NativeHost::new` takes from the registry-less
  `process_safety()` (`:32-34`), serves gwz-py's commit, tag and log (`gwz-py/native/src/client_host.rs:46, :52-53`;
  `shims.rs:74`); gwz-cli's network route calls `NativeHost::new` too (`dispatch.rs:61`).
- **std's spawn**: `setpgid` (`std/sys/process/unix/unix.rs:342`), SIGPIPE reset (`:359-372`), mask inherited
  (`:352-353`), `pre_exec` (`:383-385`; it forces the fork path, `:463`), `execvp` (`:408`); the fork path blocks no
  signal around `fork` (`:93-94`); the parent reaps a failed `exec` (`:141-149`); `kill` after the reap does nothing
  (`:990-996`). Pipes: Linux `pipe2(O_CLOEXEC)`, macOS `pipe` then `FD_CLOEXEC` (`std/sys/pipe/unix.rs:28, :34-40`).
  Unstable: `setsid` (`std/os/unix/process.rs:220-221`), Windows `main_thread_handle` and attribute lists
  (`std/os/windows/process.rs:445-449, :342`); stable: `creation_flags` (`:181-182`), the process handle (`:37-39`).
  macOS `waitid` returns for a stopped child even with `WEXITED` alone (`go/src/os/wait_waitid.go:5-6`).
- **Cleanup classification.** `terminate` returns `CleanupPending` (`ErrorCode::Io`, `https_auth.rs:190`) when the tree
  is not confirmed ended within `CLEANUP_GRACE` (500 ms, `https_auth.rs:28`; `owner.rs:261-275`), in place of
  `Cancelled` or a `Timeout`'s `Interaction` (`https_worker.rs:377`); a loaded Windows host's drain outlasts it (BR7).

## 3. Design

### 3.1 The registry

- **Crate.** `crates/children` (a gwz-core member by the `crates/*` glob, `Cargo.toml:50`; its own `BUILD.bazel`;
  depends on `libc`, `windows-sys` and, behind its `tokio` feature, tokio `sync` and `net`; never on gwz-core), with
  `unix` (`linux`, `macos` inside) and `windows` modules selected in one `cfg_if!`.
- **Value.** `ChildRegistry` is a cloneable handle (`Arc` inside); dropping the last runs `close()` (§3.4). It has its
  own creation lock (§3.2). A `SpawnGate` holds an optional registry handle and takes its own hold around the registry's
  spawn (as `spawn_gate.rs:431-436` does); the registry never takes the gate, so no cycle forms. No static.
- **Where it comes from.** It rides inside the gate, which helper slots, request context and backend carry. The gate's
  `spawn` takes a `Spec` and returns the crate's `Child`, its `output` the crate's `Output` (over std's when the gate has
  no registry); the `Child` has no std `wait`, so std never reaps a registered child. Changed callers:
  `commit_log/mod.rs:354-356` (`Group`, `Kill`), the gwz-git `ProcessGate` impl (`spawn_gate.rs:450-454`; `Group`,
  `Wait`), the helper's Unix arm (§3.6). `composition::process_safety_with_children()` (beside `:31`) returns the
  `ProcessGate` with a new registry; 2.3r-2 adds public `ProcessGate::kill_killable()`, `ProcessGate::close_children()`
  (no-ops without a registry; A's Force step 4 and gwz-py's close call them) and
  `Git2Backend::with_process_gate(&ProcessGate) -> Self`. `spawn_gate()` gives none, so the old entries spawn
  unregistered until 3.3, and `DriverOnly`'s requests until C gives it one (§3.7). `check_spawn_gate_minting.py` allows
  `ChildRegistry::create` only in `composition.rs`.
- **Creation precedes credentials.** `create` runs before the process holds any credential (gwz-cli before any thread,
  3.1; gwz-py when its composition object is made, before any lookup, 3.2), so the reaper's copy of memory holds none.
- **API.** `create(capacity)` (forks the reaper on Unix; capacity 256); `spawn(Command, Spec) -> io::Result<Child>`;
  `output(Command, Spec) -> io::Result<Output>`; `kill_killable()`; `close()`. `Spec` is a target (`Group`; `Process {
  ignore_interrupts }`; `Session`) and an ending (`Kill`, `Wait`, `OwnInput`, `Detach`). `Child` gives its pipe ends,
  `exited()` (async with `tokio`), `try_exited()`, `kill()`, `retire()`, each an `io::Result`. std's own errors
  (`NotFound`, `ArgumentListTooLong`, …) pass through unchanged; a fault (an entry's `ENOMEM` included) is
  `io::Error::other(fault)`.
- **Faults.** `RegistryFault` is a `Copy` enum, one fixed message per variant: `ReaperNotStarted` `child-process reaper
  could not be started` (its fork failed); `ReaperGone` `child-process reaper unavailable`; `Capacity` `child-process
  limit reached`; `ForkedProcess` `child processes refused in a forked copy`; `Closed` `child processes refused:
  closing`; `Unsupported` `child-process mode unsupported on this platform`; `OutOfMemory` `child process not started:
  out of memory` (an entry spawn std refuses `ENOMEM`, §3.4 "Cost"); `StatusLost` `child-process exit status lost:
  reaped elsewhere` (§3.3). `io::Error::other(fault)`'s text is the message. What the member gets:

| Site | A spawn or exit error today | A fault with B (rows B5, B19, B27) |
|---|---|---|
| Helper | all but `NotFound`, `ArgumentListTooLong` → `AuthError::SpawnFailed` (`runner.rs:115-121`), `ErrorCode::Unavailable` (`https_auth.rs:175`); `AuthError` is `Copy`, its messages fixed (`:106-167`) | `runner.rs` downcasts: `Closed` → `AuthError::Cancelled`, `ErrorCode::Cancelled` (A's Force step 4); any other → new `AuthError::ChildProcess(fault)`, the transport's `ErrorCode::Io` (as `:190`), the fault's message |
| CLI-route `git commit`, `git tag` | gwz-git's `ProcessGate::output` is `io::Result<Output>` (gwz-git `src/native_commit.rs:93-95`); `failed to run <what>: <error>` (`src/native_commit/cli.rs:17-21`), `GitCommandFailed` (`src/git/verify_checkout_state.rs:124-129`) | the same, `<error>` the fault's message (gwz-git unchanged) |
| `rev-list` | a `HistoryUnreadable` degradation, `could not run local path history cursor: <error>` (`commit_log/mod.rs:296-300, :359-361`) | the same, with the fault's message |
| Helper, at exit | a wait error → `AuthError::Io`, `process wait failed` (`runner.rs:141, :145`; `https_auth.rs:142`) | `runner.rs:145` downcasts as at spawn: `StatusLost` → `AuthError::ChildProcess(StatusLost)`, `ErrorCode::Io`, its message; the CLI route and `rev-list` carry it as above |

- **Without a registry** the crate's `Child` is no entry (no record, no job) and spawns as today: `rev-list` and the CLI
  route stay in gwz's group (OD-B2 (a)), waited for by std's `output`, never signalled; the helper leads its own group
  (`process_tree.rs:71`) and keeps §3.3's observation and order, so its kills, group kill included, precede its reap.
- **Entries.** A `Kill` or `Wait` child is an entry: registered with the reaper (Unix), given a kill-on-close job
  (Windows). An `OwnInput` child is tracked for its handle only (§3.7); a `Detach` child not at all after its spawn.
- **Fork check.** The registry records its creator's PID; every method and every `Drop` of `ChildRegistry` and `Child`
  compares it first, before any lock. In a forked copy a call is refused `ForkedProcess` and a drop does nothing, so the
  copy never kills, registers or unregisters on its parent's behalf.
- **Capacity.** On every platform the live entries never exceed the capacity (on Unix the reaper's preallocated table):
  a spawn beyond it is refused `Capacity` before the child starts.

### 3.2 Starting a child

- **Unix.** The registry sets `process_group(0)` for `Group` and adds its one `pre_exec` closure (no other gwz-core code
  uses `pre_exec`). The spawning thread saves its mask and blocks every signal around the spawn (`pthread_sigmask`), as
  std's fork path does not (§2), so no handler of gwz's, Python's or an application's runs in the child. In the child,
  after std's `setpgid` and SIGPIPE reset, in order: `setsid()` for `Session`; for every target, SIGINT, SIGQUIT,
  SIGTERM, SIGTSTP and SIGHUP `SIG_IGN`, which discards one sent to gwz's group before the child left it and pending
  since **(OS)** (B8), so a Ctrl-C, Ctrl-\, Ctrl-Z or group SIGTERM (O4 (a)) in that window is gwz's alone and never
  kills a `Wait` child before `exec` or stops one in a group no `fg` continues (any other signal keeps its default);
  every caught handler and each of those five `SIG_DFL`, unless gwz ignored it (`sigaction`'s old action), as `exec`
  leaves it; SIGINT and SIGQUIT `SIG_IGN` for `ignore_interrupts`; for an entry, SIGPIPE `SIG_IGN`, the register record
  (§3.4) in one atomic `write` (below `PIPE_BUF`), SIGPIPE `SIG_DFL`, and the write's error returned, so with the reaper
  gone the program never runs; last, the saved mask, as std and 1.0.18 pass it on (§2). All of it is async-signal-safe.
- **The creation lock.** An entry spawn holds the registry's creation lock from its `Closed` and `Capacity` checks until
  std's spawn returns and the entry is recorded live (Windows: through its resume); a spawn that fails unregisters its
  token. `close()` and `kill_killable()` take it only to set `Closed` and snapshot the live entries, and `close()`
  closes the write end after that, so a spawn in flight is refused before its fork or is an entry they act on. The child
  takes none (its fork check precedes any lock); spawns already serialize under the gate's hold.
- **Windows.** An entry is created `CREATE_SUSPENDED` through `creation_flags` under the gate's hold and the creation
  lock (above), assigned to a new unnamed, non-inheritable job with `KILL_ON_JOB_CLOSE | DIE_ON_UNHANDLED_EXCEPTION` and
  no breakaway; its one thread, found with `CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD)` by process ID (as
  `handshake_local_rows.rs:447` does), is opened `THREAD_SUSPEND_RESUME` and resumed. Any failure, or other than one
  thread, terminates the child and refuses the spawn. `Group` adds `CREATE_NO_WINDOW`; `Process` and `Session` add
  `CREATE_NEW_PROCESS_GROUP`. `kill()` is `TerminateJobObject`; `retire()` clears the limit; the end closes the job. gwz
  ended from outside before the assignment leaves R-B14. The helper keeps its own launch (`windows.rs:54-62`). `Detach`
  is refused `Unsupported` (parked :808 needs a handle list, which std cannot express on stable).
- **Windows targets differ.** `CREATE_NEW_PROCESS_GROUP` disables CTRL_C for the child and its descendants whatever
  `ignore_interrupts` says (`Process { ignore_interrupts: false }` ignores Ctrl-C on W, not on M, L); CTRL_BREAK and
  CTRL_CLOSE still end it. A `Process` entry's descendants end with its job on W; M and L kill its PID alone. C decides
  whether that suits each `Process` child (parked :809). No console event reaches a `Group` child (O4 (a): gwz's own
  take A's path).

### 3.3 Exit observation, unregistration and ID reuse

- **Exit observation** (platform modules). Linux: `waitid(P_PID, pid, WEXITED | WNOWAIT)`. macOS, where that call also
  returns for a stop (§2): a kqueue `EVFILT_PROC` `NOTE_EXIT` on the PID, which does not reap; a registration refused
  `ESRCH` is an exit once `waitid(P_PID, pid, WEXITED | WNOHANG | WNOWAIT)` reports `CLD_EXITED`, `CLD_KILLED` or
  `CLD_DUMPED`, both retried after 1 ms, doubling to 100 ms, while it reports none (a process exiting, not yet a zombie)
  **(OS)**. On both, `ECHILD` from that call or the reap (another reaper took the child, R-B1) is an exit: the entry is
  unregistered at once, reaping nothing; every later kill of it by gwz does nothing; `exited()` and `output` return
  `StatusLost`, never a made-up status. A stop or continue is never an exit and nothing spins meanwhile (B25). Until the
  reap the zombie pins the PID and, for a leader, its group ID.
- **Order.** A `Wait` entry is retired when its leader exits, so what its group leaves runs on as under 1.0.18 (R-B12).
  A `Kill` entry, when its owner releases it (an `output` call at once; the helper's owner after its decision), is sent
  `kill(-pgid, SIGKILL)` (`Group`, `Session`) unless retired. Then the waiter has the writer thread write the unregister
  record, waits for its acknowledgement, and reaps. `retire()` writes the unregister record first and kills nothing
  (TR1.6 :82). On Windows a `Wait` entry's waiter clears its job's limit at the leader's exit.
- **The writer thread.** One per registry makes every parent-side write, blocking; its mask blocks SIGPIPE, so a dead
  reaper is an `EPIPE`. The registry's core owns the write end and closes it only in `close()` or at process exit. A
  writer thread that dies sets `ReaperGone` and closes nothing, so its death kills no entry. A waiter reaps only after
  its acknowledgement or after the watch thread has reaped the reaper (nothing can then kill); otherwise its zombie stays
  pinned until gwz exits (R-B1), and `close()` counts an observed exit as ended.
- **Why no kill reaches a reused ID (Unix).** gwz kills only live entries, pinned by a zombie or a live process; the
  reaper kills only at end of file and only entries still registered; records arrive in order and gwz reaps only after
  the unregister record is written. Left open (R-B1, BR4): gwz's death between a child's exit and its unregistration (or
  with entries a dead writer left registered); std's reap after an `exec` failure; a process-wide reaper (`waitpid(-1)`,
  `wait()`, SIGCHLD `SIG_IGN`, `SA_NOCLDWAIT`) reaping a zombie before the waiter sees it, which exposes gwz's every
  kill while it lives, unsupported in gwz-py (§8 row 6). Linux keeps an inherited SIGCHLD `SIG_IGN` across `exec`: every
  status is then `StatusLost`, as 1.0.18's std wait failed `ECHILD`.
- **Windows.** Every kill goes through a job or process handle, which holds the kernel object; no ID is reused.

### 3.4 The reaper (macOS and Linux)

- **Creation.** `create` makes the lifeline, opens `/dev/null`, allocates the table and a descriptor buffer, and forks
  with every signal blocked (as §3.2); a failed fork leaves `ReaperNotStarted`. The parent closes the read end and starts
  the writer thread and a watch thread that observes the reaper's exit as §3.3 observes a child's (a stopped reaper is
  not dead, B25) and reaps it.
- **Launch contract** (after `fork` returns, R-B20). Only async-signal-safe calls, no allocation, no return into Rust or
  Python: `setsid()`; every caught handler `SIG_DFL`, then `SIG_IGN` for SIGINT, SIGQUIT, SIGHUP, SIGTERM, SIGTSTP,
  SIGTTIN, SIGTTOU, SIGPIPE; an empty mask; `chdir("/")`; `/dev/null` onto 0-2, the read end onto 3, its copy of the
  write end closed by number, every other descriptor closed: `close_range(4, ~0)`, on any failure the `/proc/self/fd`
  entries read with `getdents64` (Linux); `proc_pidinfo(PROC_PIDLISTFDS)` repeated until it lists only 0-3 (macOS)
  **(OS)**; bounded by the open descriptors, not `RLIMIT_NOFILE`. If no method confirms only 0-3 remain, `_exit(1)`
  (`ReaperGone`).
- **Records.** 16 bytes: magic (u16), version (u8), operation and target (u8), PID (i32), token (u64, never reused).
- **Loop.** It reads records (partial reads accumulated, `EINTR` retried) into the table by token, ignoring an
  unregister record for a token it does not hold. At end of file it sends SIGKILL to every entry still registered
  (`kill(-pgid)` for `Group` and `Session`, `kill(pid)` for `Process`; OD-B4 (a), §6) and calls `_exit(0)`. A read
  error, a bad magic, version or operation, or a register record with the table full: `_exit(1)`, killing nothing.
- **Its death.** The watch thread or the writer's `EPIPE` sets `ReaperGone`: every later entry spawn is refused before
  its program runs. Running children keep their in-process paths but lose cover against gwz's death (R-B2).
- **Its end.** `close()` (and the last drop) sets `Closed` and takes the live entries under the creation lock (§3.2),
  kills every live `Kill` entry, waits without a bound for every live `Wait` entry (BR5), then closes the write end
  (Windows: the jobs); the reaper exits at end of file and the watch thread reaps it. An exit without `close()` closes
  the write end in the kernel and the reaper kills every entry still registered, `Wait` included (R-B4). `OwnInput` and
  `Detach` children are never killed.
- **Cost (OD-B3).** The reaper is a fork of the host: of gwz-cli before any thread (small), of Python when gwz-py's
  composition is made. Until gwz exits it keeps every page the parent writes after the fork, at most the parent's
  private writable memory at the fork. Under `vm.overcommit_memory=2` the fork can fail `ENOMEM` and every entry spawn
  is refused (R-B16). Linux's OOM killer counts the shared pages and may choose the reaper (R-B2). **Per spawn:** the
  closure puts every entry spawn on std's fork path (§2), where today's `rev-list` and 1.0.18's CLI route could use
  `posix_spawn` (glibc's a `CLONE_VM | CLONE_VFORK` clone **(OS)**): each spawn copies the host's page tables, so a path
  walk (a spawn per step) slows with the host's size, and under strict overcommit each fork is charged the host's
  private writable memory again, so a spawn can fail `OutOfMemory` (R-B19). 3.4 measures both (§4). A `vfork`-style
  spawn in the crate is not taken: the operator's, if 3.4's numbers do not suit. **atfork:** each fork here, the
  reaper's and std's per entry spawn (`libc::fork()`, `std/sys/process/unix/unix.rs:187-189`), runs the host's
  `pthread_atfork` handlers, which glibc's `posix_spawn` does not **(OS)**: R-B20.

### 3.5 The lifeline's descriptors

- Made with `pipe2(O_CLOEXEC)` on Linux, `pipe` on macOS; each end is moved to 3 or above with `F_DUPFD_CLOEXEC` and the
  originals closed, so a process started with 0-2 closed cannot write into it. The read end is only in the reaper; the
  write end in gwz, and in a registered child only between its fork and its `exec`.
- On macOS the originals are inheritable until closed: a child another thread starts then holds the write end, delaying
  the reaper's end of file until it exits, as with std's pipes (§2); so does a fork without exec (R-B3). gwz-cli makes
  its registry before any thread. That window can also hand an `OwnInput` child's input to another spawn (R-B15, C's).

### 3.6 The kinds and how each is ended

| Child | Spec | While gwz lives | When gwz is gone |
|---|---|---|---|
| Credential helper (Unix arm onto the registry; Windows arm unchanged) | `Group`, `Kill` | its lookup's bound, cancel and drop kill the group (`owner.rs:253-303`); success retires it. W: its own job, no entry, so `kill_killable()` misses it; it ends at its lookup's cancel (A r3's host close, A r3 §3.2) | reaper; its job |
| `git rev-list` path cursor | `Group`, `Kill` | waited for (one `--max-count=1` step, `commit_log/mod.rs:337`); drop and `kill_killable` kill it; it writes nothing (`GIT_OPTIONAL_LOCKS=0`, `:347`) | reaper; its job |
| `git commit`, `git tag` | `Group`, `Wait` | waited for by its write sequence, by Force and by `close()`; never killed (BR5); retired at its exit | reaper (OD-B4); its job (R-B4) |
| gwz-sspi worker (Windows) | unchanged | its supervisor | its job |

- **The helper's Unix arm** drops `tokio::process`, whose own reaping (§2) would reap a child the waiter pins. Its
  `HelperChild` becomes a struct over the crate's `Child` with the Windows arm's shape (`process_tree/windows.rs:44-48,
  :153-183`: three pipe fields, `wait`, `try_wait`, `start_kill`), its pipes `tokio::net::unix::pipe` ends
  (`tokio/src/net/unix/pipe.rs:419, :967`), so `runner.rs:123-144` (`:145` maps `StatusLost`, §3.1) and `owner.rs`
  outside `terminate` are unchanged. tokio's `process` feature leaves the candidate table (`Cargo.toml:139`) for the
  candidate dev-dependencies (`:152`), which the three test files use and no normal build reaches.
- **The outcome stands (BR7).** `terminate` returns the lookup's `Cancelled` or `Timeout`; a tree not seen ended within
  `CLEANUP_GRACE` is retained (`owner.rs:298`) for `reap_pending` (`:97`), counted by
  `AuthOwner::pending_cleanup_count()` (`:77`) in `https_worker.rs:303`'s count, the host close's `Pending` (A r3 §3.3).
- **Not the registry's.** The pager and `forall`'s commands (OD-C6): waited for by A, never killed (R-B8). `gwz hook`'s
  utilities and native route helpers stay as they are (R-B9, R-B5). A gate with no registry spawns as §3.1 states
  (R-B13). What a helper, `git` or a broker starts in another session or job (pinentry, WMI or COM, a `setsid`
  descendant) is outside any group or job (TR1.6 :75; `process_tree.rs:21-24`; R-B7).

### 3.7 Interfaces to A and C

| To | What B gives |
|---|---|
| **A** (revision 2) | `kill_killable()` kills every live `Kill` entry and refuses later `Kill` spawns `Closed` (decision (i)), after any entry spawn in flight is recorded (the creation lock, §3.2), so a `Kill` spawn past its check is killed too; `Wait` spawns stay allowed for a write sequence still running. A's Force calls it at step 4 through `ProcessGate::kill_killable()` (A :179-180): a helper spawn it refuses reports `Cancelled`, a `rev-list` step's degrades (§3.1). A calls no `close()` (A :183-184), and gwz-cli needs none: a `Wait` child is started only by `output` (`spawn_gate.rs:450-454`), which returns after its exit, and each `std::process::exit` in `run` precedes any command (`gwz-cli/src/lib.rs:162, :175-177, :187, :207, :350`) or follows its command's return (`:235, :248, :256, :259, :274, :281, :288, :299, :326, :344`). B changes no disposition in gwz's process: only a forked child before `exec` and the reaper do (allowlist entries, §4). O4 (a) and O5 reach B only through R-B4: SIGTERM and SIGHUP no longer end gwz-cli by default; an interrupt after Force began, and SIGQUIT at any time, do (A R14). |
| **C** | `Process { ignore_interrupts: true }` keeps a child in the terminal's foreground group with SIGINT and SIGQUIT ignored, killed by PID (parked :1212, :1218), with §3.2's Windows differences. `Session` gives a new session (parked :809). `OwnInput` registers nothing and gets no job; the child ends at its input's end of file (parked :1224; Safety P2-4, :109), subject to R-B15. `Detach` (with `Session`, Unix only) outlives gwz by design, forgotten at once (R-B11). Which child takes which, how 2.3k (before 3.1) and `DriverOnly` (`driver_only.rs:61`) reach a registry, the Windows background launch, and the proof that each ends, are C's; until then `DriverOnly` spawns unregistered (R-B13). |

### 3.8 Embedding (gwz-py)

gwz-py's composition object creates the registry (3.2), perhaps in a threaded process; its close (3.2's atexit join)
runs `ProcessGate::close_children()`, which waits for a daemon thread's running commit and refuses that thread's later
spawns (R-B17), a spawn in flight refused or waited for (§3.2). The README (§8 row 6) states R-B1, R-B3, R-B4, R-B16,
R-B19, R-B20 and both forks' memory.

## 4. Steps

| Step | Content | Owns | Size | After | Review |
|---|---|---|---|---|---|
| **2.3r-1 (new)** | §3.1-§3.5 in `crates/children`; rows B1-B12, B20-B25 | create `crates/children/` (with its `BUILD.bazel`); gwz-core `Cargo.lock` (the package entry); root `MODULE.bazel` and `MODULE.bazel.lock` (the manifest in `@gwz_core_crates`, root `MODULE.bazel:73-96`); `scripts/checks/local_clone_inventory.json` (the crate's entry `gwz-children`, directory `children`: role `integration`, owner `CS`, rationale "the child registry, its Unix reaper and Windows jobs over documented concrete OS dependencies; it implements no named contract"; edges `libc`, `windows-sys`, `tokio`; LBT-001; lane C reviews the entry, LBT-012, named in the brief), `check_crate_versions.py` (`EXPECTED_CRATES`, :85), `process_globals_allowlist.json` (the crate's `sigaction` and `signal` entries under A's `signal` kind, `chdir` under `env`) | ~600 + 650 | nothing to build (the crate builds and tests alone); merged after A's 2.3s, which makes the `signal` kind (A r3 §3.1 "Check", §4 point (6), its 2.3s row) | single (State) |
| **2.3r-2 (new)** | §3.1's gate slot, `Spec` at the three call sites, the fault mapping, `process_safety_with_children`, the two `ProcessGate` methods, `with_process_gate`; §3.6: the helper's Unix arm, `rev-list` and the CLI route onto the registry, tokio's `process` feature to the dev table, `terminate`'s outcome; the two checks; rows B13-B17, B27, B28 | `src/git/spawn_gate.rs`, `src/git/gitbackend/backend.rs` (`:118-125`), `src/git/endpoint/https_auth.rs` (`AuthError`), `https_auth/runner.rs` (`:115-121`, `:145`), `process_tree.rs` (Unix arm), `owner.rs` (`terminate`), `src/operation/commit_log/mod.rs` (`:330-361`), `src/transport_host/composition.rs`; `src/transport_host/mod.rs` (one `pub use` of `process_safety_with_children` beside `:32`, asked of the trunk, plan :330-331); gwz-core `Cargo.toml`, `Cargo.lock`, `BUILD.bazel` (one label); `scripts/checks/check_spawn_gate_minting.py`, `check_process_globals.py` and their tests `test_check_spawn_gate_minting.py`, `test_check_process_globals.py`, the inventory's gwz-core edge to the crate | ~380 + 420 | 2.3r-1; A's 2.3s | dual (it moves the helper's spawn, whose environment carries secrets); the brief names §3.1's invariant "creation precedes credentials" |
| 2.3j (amended) | spawns nothing new; ordered after the registry by D-V (3) | as planned | as planned | 2.3g, 2.3r-2 | as planned |
| 3.1 (amended) | `run` calls `process_safety_with_children()` after `worker_host::early` and before any thread; the host takes it through `NativeHost::with_process_safety` (in place of `dispatch.rs:61`'s `NativeHost::new`), every hostless backend through `with_process_gate` (`dispatch.rs:93`, `log_exec.rs:118`, `hook/family.rs:63, :149, :187`); row B18 | those lines in 3.1's files | +40 | as planned | phase |
| 3.2 (amended) | the composition object creates the registry once, before any lookup, and its close runs `close_children()`; the Client's `NativeRoute` is built with `NativeHost::with_process_safety(worker, &gate)` on that `ProcessGate` in place of `NativeHost::new` (`client_host.rs:46`), so its `backend()` carries the registry to commit, tag and log (§2); the two hostless production lines (`client_host.rs:66`, `route/native.rs:46`) are the non-candidate build's and stay unregistered ("Interim"); README text (§8 row 6); rows B19, B26, B29 | those lines in 3.2's files; `gwz-py/README.md` | +60 | as planned | phase |
| 3.4 (amended) | the reaper's fork time and unique memory (gwz-cli after the parent writes 100 MiB; gwz-py with a 2 GiB heap, then 1 GiB written after the fork); per-spawn fork time and a path walk's step rate at that heap (M, L) | evidence | | as planned | |

- **Waves (2.3 plan §4).** Lane 3: 2.3e then 2.3r-1 in wave 2, 2.3r-2 then 2.3j in wave 3; at most three compiling
  lanes. 2.3r-1's shared files (§4 table) are merged serially with any lane that adds a crate or an entry (A's 2.3s; A's
  3.0d later). It exceeds the 500-line aim: registry, reaper and jobs build and test only together. 2.3r-2's files are
  outside every 2.3 sub-step's (2.3 plan :240-242) and the Phase 1 trunk's (plan :324-326) but `transport_host/mod.rs`
  (the trunk's, plan :325; 2.3g's and 2.3k's, 2.3 plan :246), whose one line it asks for (plan :330-331).
- **Interim.** Until 3.1 and 3.2, gwz-cli's and gwz-py's backends and the old entries spawn unregistered, as today;
  after them the non-candidate build does, and `DriverOnly` until C gives it a registry (§3.7; R-B13; OD-B2 (a)).

## 5. Rows

Each is red first unless a leg is **(reg)**. M macOS, L Linux, W Windows. "Harness": a test of `crates/children`
(B1-B12, B20-B25), of gwz-core (B13-B17, B27, B28) or gwz-py (B19, B26, B29); a fixture `sh` starts a sleeping grandchild.

| Row | Step | What is checked | Where |
|---|---|---|---|
| B1 | 2.3r-1 | SIGKILL of the harness while a registered `Group` child and its grandchild run: both gone within 1 s. W: `TerminateProcess` of the harness; the job ends both within 1 s. | M, L, W |
| B2 | 2.3r-1 | SIGKILL, SIGQUIT, SIGINT-then-SIGKILL to the harness's whole group, and a hung-up pseudo-terminal: the reaper outlives each and the registered group is gone within 1 s. | M, L |
| B3 | 2.3r-1 | A retired child's grandchild lives after the harness's exit and its SIGKILL; a non-retired `Kill` child whose leader exited leaves no member; the reaper's trace shows no kill of an unregistered token and each unregister record before its reap. W: a retired job's grandchild lives. | M, L, W |
| B4 | 2.3r-1 | SIGKILL of the harness right after `spawn` returns: the group is gone within 1 s, also with `ulimit -n` at its hard maximum (L through a seam that refuses `close_range`, a container limit of 2^20; M). With `close_range` refused by a seccomp filter and `/proc` hidden (L): the group ends within 1 s or `ReaperGone` is raised, never neither. | M, L |
| B5 | 2.3r-1 | The reaper SIGKILLed alone: `ReaperGone` within 1 s; the next spawn is refused and never runs; a running child still ends by its handle. An injected fork `ENOMEM` at `create`, then at an entry spawn: `ReaperNotStarted`, then `OutOfMemory`; each refusal with §3.1's exact message. | M, L |
| B6 | 2.3r-1 | `lsof`: the harness holds no read end; the reaper only it and `/dev/null` on 0-2; under 50 concurrent spawns no child holds either end (L; M with the registry made first); a harness `flock` frees at its exit. | M, L |
| B7 | 2.3r-1 | The harness started with 0-2 closed: both ends are 3 or above and SIGKILL still ends its child. Faults: bad-magic bytes (the reaper exits, kills nothing, `ReaperGone`); a writer-thread panic with a live `Wait` and a live `Kill` entry (no child killed, later entry spawns refused; both exits observed, `close()` returns, no zombie reaped while the reaper lives); a token counter started near its wrap (no collision). With a blocking `pthread_atfork` child handler registered before `create`: whether the reaper still ends a child at the harness's SIGKILL is recorded, not asserted (R-B20). | M, L |
| B8 | 2.3r-1 | A spawn from a thread with SIGINT and SIGPIPE blocked: the child's mask equals that thread's (both blocked) and SIGPIPE `SIG_DFL`; a spawn after the reaper died fails, and no child died of SIGPIPE. A seam holds a child between `fork` and its closure while SIGINT reaches the harness's group: the harness's own handler's pipe gets one byte, a `Process { ignore_interrupts }` child with SIGINT pending runs its program; a `Group` `Wait` child and a `Session` child, each held there while SIGINT, SIGQUIT, SIGTERM and SIGTSTP reach the harness's group, run their program to exit, never stopped (state never `T`), and the harness ends without waiting on a stopped child. | M, L |
| B9 | 2.3r-1 | A copy forked without exec while another thread holds the registry's lock: calls refused `ForkedProcess`; its drop of a live `Kill` entry's `Child`, `kill_killable()` and `close()` leave the parent's child running, and it exits without deadlock; the parent's SIGKILL still ends its children once the copy exited. | M, L |
| B10 | 2.3r-1 | A full registry refuses `Capacity`; 300 spawns of a missing program are each refused with std's `NotFound`, unchanged, and the live count is zero; on M and L every registered token has its unregister record. | M, L, W |
| B11 | 2.3r-1 | A thread reaping with `waitpid(-1)`; SIGCHLD `SIG_IGN`; `SA_NOCLDWAIT`: an exited child's entry is unregistered at once (`ECHILD`; M by both `NOTE_EXIT` and the `ESRCH` path), the reaper never kills it; every `kill(2)` traced, none follows the `ECHILD`; a `Wait` child's `output` returns `StatusLost`. | M, L |
| B12 | 2.3r-1 | A child that spawns a grandchild first: both in the job; `GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0)` reaches neither. An injected resume failure: the child terminated, the spawn refused, no process left. `close()` during a creation held at a seam returns after the assignment. | W |
| B13 | 2.3r-2 | A sleeping `HelperSlots` helper: harness SIGKILL ends its group in 1 s; W: job **(reg)**. | M, L, W |
| B14 | 2.3r-2 | A path-filtered history walk: `rev-list` leads its own group; `kill_killable()` and the harness's SIGKILL each end it within 1 s. W: a job and no console. | M, L, W |
| B15 | 2.3r-2 | A CLI-route commit, `pre-commit` sleeping, `post-commit` starting a background `sleep`: SIGINT to the harness's group, also inside the fork-to-`setpgid` window (B8's seam), reaches neither `git` nor the hook, `kill_killable()` leaves it running, `close()` returns after it, the `sleep` outlives `git`. A `pre-commit` that never exits: the harness's SIGKILL ends both within 1 s, `index.lock` remains (R-B4). W: CTRL_BREAK reaches neither; a hook reading `CONIN$`: whether it waits or fails without a console is recorded, not asserted (R-B10). | M, L, W |
| B16 | 2.3r-2 | The drain held past `CLEANUP_GRACE` (a `cfg(test)` seam in `owner.rs`): a cancelled lookup returns `Cancelled`, a timed-out one `Timeout` with `Interaction`; `AuthOwner::pending_cleanup_count()` (`owner.rs:77`) is one until `reap_pending` reaps it within the existing rows' 30 s (`https_auth/runner_tests.rs:299`). | M, L, W |
| B17 | 2.3r-2 | The checks fail on `ChildRegistry::create` outside `composition.rs` and on `tokio::process`, `libc::fork`, `pre_exec` in gwz-core production outside the crate; `cargo tree -e features,no-dev` shows no tokio `process` for gwz-core, gwz-cli and gwz-py. | M, L, W |
| B18 | 3.1 | `gwz fetch` with a sleeping helper, `gwz commit` with a sleeping hook, `gwz log -- <path>`: one registry, `run`'s, holds every spawn; gwz's SIGKILL ends each child within 1 s, the commit's leaving `index.lock` (R-B4; OD-B4 (a)); SIGQUIT to the terminal's group mid-hook (M, L) ends gwz by default and leaves the same. | M, L, W; W's helper leg **(reg)** |
| B19 | 3.2 | gwz-py's fetch, commit and log: one registry holds every spawn; SIGKILL of Python mid-lookup ends the helper within 1 s; with the reaper SIGKILLed (M, L), the next fetch raises `ErrorCode::Io` with `child-process reaper unavailable` and no helper runs; with an entry spawn's fork failing `ENOMEM` (injected as in B5; M, L), it raises `Io` with `child process not started: out of memory`. | M, L, W; W's helper leg **(reg)** |
| B20 | 2.3r-1 | A `Process { ignore_interrupts: true }` child: SIGINT and SIGQUIT to the harness's group leave it running in that group; at the harness's SIGKILL the reaper kills it by PID within 1 s while an unregistered `tee` in the group survives. W: CTRL_C leaves it running, CTRL_BREAK ends it, `TerminateProcess` of the harness ends it. | M, L, W |
| B21 | 2.3r-1 | An `OwnInput` child: no register record, in none of the registry's jobs (`IsProcessInJob` against each); `close()` leaves it running; at the harness's SIGKILL or `TerminateProcess` it exits 0 at end of file. | M, L, W |
| B22 | 2.3r-1 | `kill_killable()` ends a live `Kill` child and its grandchild at once and refuses a later `Kill` spawn `Closed`; `Wait` spawns, `Wait` and `OwnInput` children run on; after `close()` no spawn runs its program. A seam holds an entry spawn after its check, and separately between `fork` and `exec` (W: before its resume), while `close()` or `kill_killable()` runs: the spawn is refused `Closed` before its fork, or is an entry they act on (a `Wait` child runs to exit and `close()` returns after it; a `Kill` child is killed); the reaper's trace shows no kill of a `Wait` token; no write reaches a descriptor other than the lifeline (traced). | M, L, W |
| B23 | 2.3r-1 | A `Wait` child's background grandchild outlives the child's reap, `close()` and the harness. | M, L, W |
| B24 | 2.3r-1 | A `Session` child's session ID is its PID; it and its grandchild are gone within 1 s of the harness's SIGKILL; a `Session` + `Detach` child writes no record and outlives it. W: `Detach` refused `Unsupported`. | M, L, W |
| B25 | 2.3r-1 | A `Wait` and a `Kill` child each stopped by SIGSTOP and by a real SIGTTIN under a pty, and the reaper stopped by SIGSTOP: for 5 s nothing is retired, released or `ReaperGone`, no thread uses over 5% CPU, the reaper holds both; after SIGCONT and exit the normal order runs; the harness's SIGKILL while the children are stopped ends both groups within 1 s. An at-once exit is seen (M: the `ESRCH` path, and a seam that forces `ESRCH` while `waitid` reports nothing for 50 ms: seen within the backoff, under the CPU cap). With the reaper stopped, 5,000 entry spawns: where gwz blocks is recorded (R-B2). | M, L |
| B26 | 3.2 | A gwz-py script whose daemon thread commits with a sleeping hook calls `sys.exit(0)`: the commit completes before the process exits, no `index.lock` remains, a later member's commit is refused `Closed`; with B22's seam holding the daemon thread's entry spawn after its check, and between `fork` and `exec` (M, L), the member is refused `Closed` or completes, never killed. | M, L, W |
| B27 | 2.3r-2 | With the reaper SIGKILLed: a helper lookup fails `Io` `child-process reaper unavailable`; a CLI-route commit fails `GitCommandFailed` `failed to run git commit: child-process reaper unavailable`; a path-filtered log degrades `HistoryUnreadable` with that text. After `kill_killable()` a helper lookup reports `Cancelled`. A thread reaping with `waitpid(-1)` takes a helper's exit: the lookup fails `Io` `child-process exit status lost: reaped elsewhere`; a missing helper program still reports `MissingExecutable`. | M, L |
| B28 | 2.3r-2 | A gate without a registry: the helper leads its own group, a CLI-route commit and `rev-list` stay in the harness's group and are never signalled; a helper whose leader exited, then cancelled: its group kill precedes its reap and no signal follows the reap (traced). | M, L |
| B29 | 3.2 | A gwz-py script whose commit's `pre-commit` sleeps 3 s gets SIGTERM (Python's default action): `git`'s group is gone within 1 s and `index.lock` remains, R-B4's residue under OD-B4 (a) (D amends this row if the operator takes (b) or (c), §6). W: CTRL_BREAK (A R15) and the console closed, each recorded, not asserted. | M, L, W |

## 6. Decisions

| Decision | Taken (D-V) | Cost and alternatives not chosen |
|---|---|---|
| **OD-B1** The CLI route's `git commit` and `git tag` | (a), decision (ii): own group (no console on Windows), waited for by every cancel, by Force and by `close()`, killed only when gwz is gone | A hook or signer reading the terminal stops on SIGTTIN or SIGTTOU (Unix) or prompts on a hidden console (Windows; 1.0.18 shared gwz's console, `v1.0.18:src/git/gitbackend/repository.rs:299-305`, `refs.rs:172-183`); a hung one makes `gwz commit` wait through Cancel and Force (to which O4 (a) also routes SIGTERM, SIGHUP and console events) until gwz dies. gwz's death while a commit runs, hung or not, ends `git`'s group mid-write (OD-B4 (a): SIGKILL; Windows: the job), leaving `index.lock` and a partly run hook (R-B4, R-B10). Such deaths: SIGKILL, a crash, the OOM killer; an interrupt once Force has begun (O5); SIGQUIT, which keeps its default (A r3 §3.1 "Unix", row 6.1, R14), so a terminal Ctrl-\ ends gwz alone, `git` being in its own group, and the reaper then kills `git`; a gwz-py host's default SIGTERM or SIGHUP, `os._exit`, a foreign `exit()`, an `exec`; Windows ending gwz after CTRL_CLOSE, LOGOFF or SHUTDOWN (A R9), and a gwz-py host's default CTRL_BREAK (A R15). 1.0.18 ran `git` in gwz's group: a death signalled to gwz alone let the commit finish, a signal to the group let `git` remove its locks. Not chosen: (b) gwz's group; SIGINT to `git`'s group at Force (Safety's (c)); on Windows a kept console with `CREATE_NEW_PROCESS_GROUP` (visible prompts; CTRL_BREAK ends it) |
| **OD-B2** Gates without a registry | (a), decision (iii): they spawn as today (§3.1); after 3.1 and 3.2 that is the non-candidate build, and `DriverOnly` until C gives it a registry (§3.7); of gwz-core's own backends only `NativeHost::backend()` reaches the three spawn sites, on `process_safety()`'s registry-less gate, and 3.1 and 3.2 build gwz-cli's and gwz-py's hosts on the registry (§2, §4) | R-B13: an interrupt reaches `git` and its hooks mid-write, as on 1.0.18. Not chosen: an own group without a registry (decision (ii) taken literally): no terminal Ctrl-C reaches `git`, which then outlives gwz to finish its write, with nothing to end a hung one |
| **OD-B3** Every command forks the reaper | (a), decision (iii) | §3.4 "Cost", R-B16, R-B19; measured in 3.4 |
| **OD-B4** The reaper's signal for a `Wait` group when gwz is gone | None in D-V (decision (i) governs Force, not gwz's death). B is written, reviewed and complete as (a), SIGKILL (§3.4 "Loop"; B18, B29 assert it). Handed to D, which owns R-B4's `*.lock` files and the accepted-residual list (§0), to put to the operator | (a): every such death mid-commit tears it (R-B4). (b), B's recommendation: SIGTERM then SIGCONT to each `Wait` group, SIGKILL to the other entries: `git` removes its own `*.lock` files and exits, as under 1.0.18's Ctrl-C; a hook or signer that ignores SIGTERM outlives gwz (R-B18); every signal still goes at end of file, at once, so §3.3's reuse argument stands. It is a new mechanism (an ending bit in §3.4 "Records", a second signal in "Loop"), so not adopted under the review cap. (c) leave `Wait` groups to finish, as 1.0.18 did: a hung one outlives gwz (against BR1). If the operator takes (b) or (c), D's design amends §3.4 "Records", "Loop", R-B4, R-B18, B29 and reviews that in its own loop; no B step waits for it. Windows stays (a) under every choice: no console event reaches a `CREATE_NO_WINDOW` `git` |

- **Choices open to review:** capacity 256; the group kill of a non-retired `Kill` group at its leader's exit; one
  writer thread per registry; a blocking lifeline; suspended creation with a Toolhelp resume rather than generalizing
  the helper's job-list launch; no re-fork of a dead reaper; no Linux pidfd (D-V (2) says `kill()`); kqueue only on macOS.

## 7. Residuals for design D

| # | What can remain | When | How it is recognized |
|---|---|---|---|
| R-B1 | A SIGKILL to a process or group that reused a registered ID | gwz dies just after a registered child exited and before its unregistration (or with entries a dead writer thread left registered); a child reaped early (std after an `exec` failure); an application's process-wide reaper, which also exposes gwz's own kills while it lives (unsupported) | Not recognizable afterwards; an inherited SIGCHLD `SIG_IGN` shows as `StatusLost` |
| R-B2 | Children that outlive a later SIGKILL of gwz; gwz blocked | The reaper was killed alone, or with gwz by name (`pkill -9 gwz`, `killall -9 python`), or chosen by the OOM killer; a stopped reaper (SIGSTOP) delays this until continued, and once about 4,096 records fill the lifeline it blocks entry spawns and releases (an entry spawn blocked there holds the creation lock, so `close()` and `kill_killable()`, A's Force step 4, wait too, §3.2) | Spawns fail `child-process reaper unavailable`; children run on with parent PID 1; a reaper in state `T` |
| R-B3 | Registered children outlive gwz until another process exits | A fork without exec in an embedding application; on macOS a child another thread started while the lifeline was made | The reaper (a copy of gwz or Python in its own session) still runs after gwz died |
| R-B4 | Git `*.lock` files and a partly run hook | gwz dies while a CLI-route child runs: SIGKILL, a crash, the OOM killer, an interrupt once Force has begun (O5), SIGQUIT (its default, a terminal Ctrl-\; A r3 R14); for a gwz-py host also Python's default SIGTERM or SIGHUP, `os._exit`, a foreign `exit()`, an `exec`; W: the OS ending gwz after CTRL_CLOSE, LOGOFF or SHUTDOWN (A R9), and a gwz-py host's default CTRL_BREAK (A R15). The reaper or job kills it (OD-B4 (a)) | D's `*.lock` report (OD-C3); a hook's effects are the user's |
| R-B5 | A native route helper still waiting | gwz exits by SIGKILL, a crash or A's Force during `Cred::credential_helper` (on Windows Ctrl-C never reaches it) | A `git credential-*` process with parent PID 1, as on 1.0.18 |
| R-B6 | What a successful helper left running (`credential-cache`) | Every successful lookup (TR1.6 :82) | Not gwz's; accepted design |
| R-B7 | Processes another program or broker started (pinentry, WMI or COM, a `setsid` descendant, `ssh`'s children) | Any | M, L: in no group or job of gwz's; W: a broker's in no job, a registered `Process` entry's own descendants in its job (§3.2) |
| R-B8 | The pager or a `forall` command | gwz is SIGKILLed while it runs (OD-C6) | The user's program is still on the terminal |
| R-B9 | `gwz hook`'s utilities; what `git worktree add` leaves if interrupted | gwz is killed during `gwz hook` | Short commands end alone; `git worktree list` |
| R-B10 | `gwz commit` waiting on a stopped, hidden or hung reader | OD-B1 (a): a hook or signer reads the terminal or console, or hangs | Unix: `ps` shows the reader stopped (`T`) in `git`'s group; W: the reader alive in `git`'s own job (§3.2) with no visible console |
| R-B11 | The served form's background server | Every `gwz server start` without `--foreground` (2.3k) | `gwz server status`, `list` (C) |
| R-B12 | A background job a commit or tag hook left (`ctags … &`) | Every such commit; it outlives `git` and gwz | As on 1.0.18, in `git`'s former group |
| R-B13 | A registry-less `git commit`, `git tag`, `rev-list` or helper, or a hook's partial effects | Non-candidate build (M, L, W), and `DriverOnly`'s requests until C (§3.7): an interrupt reaches `git` mid-write, as on 1.0.18 (`git` removes its own `*.lock` on SIGINT); gwz's death leaves each running | A `git` process with parent PID 1; R-B4's `*.lock` files |
| R-B14 | A suspended child outside any job, holding inherited handles (a pipeline may never see end of file) | W: gwz terminated from outside between its creation and its job assignment | A suspended process whose parent is gone |
| R-B15 | An `OwnInput` child that misses its end of file | M: another thread's spawn inherits its input inside std's `pipe`-then-`FD_CLOEXEC` window (C decides) | The child runs on after gwz is gone |
| R-B16 | Every entry spawn refused | The reaper's fork fails (`ENOMEM` under strict overcommit) | `child-process reaper could not be started` |
| R-B17 | Later members of a commit not committed | gwz-py's close runs while a daemon thread's commit loop continues | Those members' changes are uncommitted |
| R-B18 | (only if the operator takes OD-B4 (b), through D) A hook or signer that ignores SIGTERM | gwz dies while a CLI-route commit runs | A hook process with parent PID 1 after `git` exited |
| R-B19 | An entry spawn refused | Strict overcommit: each spawn's fork is charged the host's private writable memory (§3.4 "Cost") | `child process not started: out of memory` |
| R-B20 | A reaper that never reads, so registered children outlive gwz; the host's file mappings (a deleted mapped file's space) and SysV attachments, held for the reaper's life | **(OS)** A host's `pthread_atfork` child handler that blocks or misbehaves in the reaper (a C library in gwz-py's host); every fork B makes runs the host's handlers (§3.4 "Cost"). A Linux reaper fork that skips them (`_Fork`, glibc 2.34+, or a raw `clone(SIGCHLD)`) is not taken under the review cap; D puts it to the operator with this residual | A reaper alive and not stopped (not `T`) after gwz died; `lsof` of the reaper lists the host's mapped files |

## 8. Superseded or amended clauses (applied by the lane owner at acceptance; row 6 by step 3.2)

| # | Document and clause | Replacement or addition |
|---|---|---|
| 1 | 2.3 plan §3, after 2.3l (:220-230) | Add: "**2.3r-1 Child registry and reaper (single, State).** Goal: plan §10 D-V resolution (2). Owns: GwzChildLifetimeDesign §4 (create `crates/children/`; `Cargo.lock`, root `MODULE.bazel` and its lock, the crate inventory, `check_crate_versions.py`, the process-globals allowlist). Content and red rows: GwzChildLifetimeDesign §3.1-§3.5, rows B1-B12, B20-B25. Size ~600 + 650. After: nothing to build; merged after 2.3s (its entries take the `signal` kind). Evidence: Mac, WSL, Windows receipt." and "**2.3r-2 Spawns onto the registry (dual: secret handling; the brief names the invariant 'creation precedes credentials').** Goal: D-V resolution (2) for gwz-core's three spawn sites and its public gate and backend setters. Owns: GwzChildLifetimeDesign §4. Rows B13-B17, B27, B28. Size ~380 + 420. After: 2.3r-1, 2.3s. Evidence: Mac, WSL, Windows receipt." |
| 2 | 2.3 plan 2.3j, "After: 2.3g (stdio serves through it)." (:202) | "After: 2.3g (stdio serves through it) and 2.3r-2 (plan §10 D-V resolution (3))." |
| 3 | 2.3 plan §4, the critical path (:234) and table, waves 2 and 3, lane 3 (:240-241) | :234: "Critical path: the longer of 2.3d, then 2.3g, then 2.3h, then 2.3l, and 2.3e, then 2.3r-1, then 2.3r-2 (also after 2.3f, then 2.3s), then 2.3j, then 2.3l. Off it: 2.3a, 2.3b, 2.3c." Wave 2 lane 3: "2.3e address, then 2.3r-1 registry"; wave 3 lane 3: "2.3r-2 spawns, then 2.3j credentials and stdio"; reasons: "r-1 owns `crates/children/` and shares `Cargo.lock`, root `MODULE.bazel` and its lock, the inventory, `check_crate_versions.py` and the allowlist, merged serially; r-2 owns the files GwzChildLifetimeDesign §4 lists". |
| 4 | 2.3 plan §4, "Review: dual on a, d, j, l" and totals (:249-250); §1's "about 4,300 implementation lines" (:71) | :71's "Summed below: about 4,300 implementation lines, a little more again in rows." becomes "Summed in §4's totals sentence."; "Review: dual on a, d, j, l and r-2" and "about 5,280 implementation lines and 5,820 rows across 14 steps" (A r3 §8 row 7's totals sentence, which adds 2.3s, supersedes this clause in either order of applying them). |
| 5 | Plan §4, 3.1 (:721-728), 3.2 (:729-735), 3.4 (:739) | Append to 3.1: "`run` creates the child registry after `worker_host::early` and before any thread; the host and every hostless backend take it through `NativeHost::with_process_safety` and `Git2Backend::with_process_gate` (GwzChildLifetimeDesign §4, row B18). +40." Append to 3.2: "The composition object creates the child registry once, before any lookup, and its close runs `ProcessGate::close_children()`; the Client's `NativeHost` is built on it with `NativeHost::with_process_safety` (GwzChildLifetimeDesign §4 3.2); the README text (rows B19, B26, B29). +60." Append to 3.4: "and the reaper's fork time and unique memory, and per-spawn fork time and path-walk step rate, gwz-cli and gwz-py (M, L)." |
| 6 | `gwz-py/README.md`, "Platform And Status" (:163), applied by step 3.2 | Add: "gwz starts one helper process per Python process that ends gwz's child processes if Python is killed. It is a fork of Python and keeps a copy of memory Python changes later; if the fork fails (for example under strict memory overcommit), gwz refuses to start processes. Each child process gwz starts is also a fork of Python, so it costs more in a large process and can fail `child process not started: out of memory` under strict overcommit. After `os.fork()` without exec (for example `multiprocessing`'s `fork` start method) gwz refuses to start processes in the child, and the parent's helper waits until the child exits; use `spawn`, or `forkserver` without preloading gwz. Reaping all children (`waitpid(-1)`) or ignoring SIGCHLD is unsupported: gwz may then signal an unrelated process. Both forks run the `pthread_atfork` handlers Python's libraries registered, and the helper keeps Python's memory mappings (a deleted mapped file's disk space) until Python exits. Python's default SIGTERM or SIGHUP (Windows: CTRL_BREAK), or `os._exit`, during a commit can leave `index.lock`." |
| 7 | Contract §6, "Composition/CleanupDomain" (`GwzConnectionApiContract.md:289`) | Append to "State owned": "; the child registry, starter and kill authority of every child a gate with a registry starts, with its reaper (GwzChildLifetimeDesign §3.6 lists the children it does not start)". |

Rows 5-6 apply OD-B3 (a) and OD-B2 (a); row 4: 4,300 + 600 + 380 and 4,750 + 650 + 420. History stays unedited: TR1.6
"Its cost" (:76) and OQ2 (:398), withdrawn by D-V (:1005-1010); r1's OD-C2 (a), "one extra process per gwz process that
starts children" (`3448c561` :245-246), superseded by OD-B3 (a);
`gwz-core/dev-docs/history/GwzTransportCredentialHelpers-ReviewCode-2.md:66` (`CleanupPending`), superseded by §3.6 from
2.3r-2. 2.3k's background copy is C's (§3.7).

## 9. Changes

IDs as in `GwzChildLifetimeDesign-RemPlan-2.md` (round 3: its "Closure pass", prefixes `S3-`, `C3-`).

| Revision | ID: sections |
|---|---|
| 3 (round 2) | O4 (a), O5: §3.2, §3.7, OD-B1, R-B4, §8 row 6. C-N-P2-1: §3.1 (fixed messages, per-site table), §4, B5, B19, B27, R-B2, R-B16. C-N-P3-1: §4, §8 rows 1, 3, 4. C-N-P3-2: §3.1, §3.2, §3.7, §3.8, §8 row 5. C-N-P3-3, S-N-P3-5: §3.3, R-B1, R-B2, B7, B25. C-N-P3-4: §1 (BR1-BR8). C-N-P3-5: B3, B18, B19. C-N-P3-6: §8. C-N-P3-7, -8: §8 note, OD-B2. C-N-P3-9: §3.2, R-B7. S-N-P2-1: OD-B1, OD-B4 (D's), §3.4, §3.6, R-B4, R-B18, B18, B29. S-N-P2-2: §3.1, §3.4, R-B19, §4, §8 rows 5-6, B5, B19. S-N-P3-1: §2, §3.2, §3.4, B8, B15. S-N-P3-2: §3.1 (exit row), §3.3, §3.6, R-B1, B11, B27. S-N-P3-3: §3.3, B25. S-N-P3-4: §3.4, B4. S-N-P3-6: §3.1, §3.6, R-B13, B28. BR7 cites A's A6; §3.6 and §8 row 4 follow A r3 §4. §1, §5, §6 became tables. |
| 3.1 (round 3) | S3-N-P2-1: §3.2 "The creation lock", "Windows", §3.4 "Its end", §3.7 (A), §3.8, R-B2, B22, B26. S3-N-P3-1: §3.2 "Unix", B8. S3-N-P3-2: §3.2 "Unix", B8. S3-N-P3-3: §1 BR1, §3.4 "Launch contract", "Cost", §3.8, R-B20, B7, §8 row 6 (the Linux reaper fork deferred to D). C3-N-P3-1: §4 2.3r-2, "Waves". C3-N-P3-2: §2 "The gate", §4 3.1, 3.2, OD-B2, §8 row 5. C3-N-P3-3: §1 BR1, BR4. C3-N-P3-4: OD-B1, R-B4, B29, §8 row 6. C3-N-P3-5: §4 2.3r-1. C3-N-P3-6: §8 rows 3-5. §3.7 became a table. |
