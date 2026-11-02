# Child-process lifetime: design B of the cancellation split (DRAFT)

Status: DRAFT, revision 2 (2026-11-01); design B of the cancellation split (D-V), remediated per
`GwzChildLifetimeDesign-RemPlan.md`; for round-2 review by fresh reviewers. Authorizes no implementation.

**Citations** are at root `8ade43ac` for documents, gwz-core `b745f1fa` (and `v1.0.18` where named), gwz-cli `c0ba717`,
gwz-py `2185cc6`, gwz-git `9bc4ea9`, gwz-sspi `6f83551`, git2-rs `d13951f`; Rust std 1.95.0 as `std/` (its
`library/std/src/`), tokio 1.53.1 as `tokio/`, Go 1.26.4 as `go/`. A path that begins with a member name is
workspace-relative; any other source path is gwz-core's. In `dev-docs/`: "plan" `GwzTransportReleaseConvergencePlan.md`
(D-V :999-1062; "decision (i)-(iii)" its last bullet, :1055-1062), "2.3 plan" `GwzServerCore-Step2.3-Plan.md`, "parked"
`history/parked/GwzCoreServerDesign.md`, "r1" the cancellation amendment's revision 1 (blob `3448c561`), "Safety" its
round-1 review in `history/`, "A" `GwzCancelInterruptDesign-DRAFT.md` revision 1; "TR1.6"
`gwz-core/dev-docs/history/reference/GwzTransportCredentialHelpersDesign.md`. **(OS)**: an OS fact a row verifies.

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

- **B1 No orphan.** A registered child ends no later than its operation and no later than gwz (OQ2 reversed,
  :1005-1008). Not covered, each a §7 residual: children no registry starts (§3.6: R-B5, R-B7 to R-B9, R-B13), the
  served form's background server (R-B11), and what a retired child leaves (R-B6, R-B12).
- **B2 Death is detected.** The reaper detects gwz's death, gwz detects a child's and the reaper's (:1007).
- **B3 The registry and the reaper** as resolution (2) states them (:1039-1043), created by every command (OD-B3,
  decision (iii)); on Windows every registered child gets a job (Safety P3-3, :256-263).
- **B4 Never the wrong process.** A kill reaches only a registered child or its group (B1's "every helper process it
  started"; D-V :1008), except under an application's process-wide reaper, unsupported (R-B1).
- **B5 Writes finish.** A registered Git CLI route `git commit` or `git tag` (a `Wait` entry), its hooks and signers,
  run in their own process group and are never killed while gwz lives: not by a cancel, a Force or `close()` (decision
  (ii), :1059-1060; decision (i), :1056-1059: Force waits without a bound for a local write). Only gwz's death ends one
  (R-B4). Not covered: one a gate without a registry starts, which an interrupt can reach mid-write (OD-B2, R-B13).
- **B6 Placement.** A Phase 2 sub-step before 2.3j (resolution (3), :1043).
- **B7 Cleanup never changes the outcome.** Ending a helper's tree returns its lookup's own outcome (A's A5; OQ2's
  "cleans up"; the f9 report, `build-targets/winflk-logs/f9-report.md` beside the root checkout, "Product classification
  limitation"). B carries it because 2.3r-2 owns `owner.rs`; A states the member report (A §3.3).
- **B8 Rules.** No global or thread-local (OD-C1 allows only A's handler); braces; `cfg` only in `cfg_if!` or platform
  modules; no production code under `tests/`; parity on M, L, W except where §3.2, §3.3 and §3.6 name a mechanism.

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
  `spawn_gate()` (`:37-40`) makes one per call for the old entries (`local_command.rs:79`, `cancellable.rs:238`) and for
  `DriverOnly`, the served core's host (`driver_only.rs:61`, plan 1.7), whose request backends carry it (`:152`). A
  hostless backend's is ungated (`backend.rs:69, :114`; `with_host_context` is `pub(crate)`, `:118-125`): gwz-cli's
  (`globalargs/dispatch.rs:93`, `log_exec.rs:118`, `hook/family.rs:63, :149, :187`), gwz-py's (8 lines of `native/src`)
  and gwz-core's own (`src/workspace_ops/handle_create_repo/create_workspace.rs:18`; in `src/diff/`, `classify.rs:204,
  :221`, `handle_diff.rs:595`, `tagged.rs:63`; `src/git/endpoint/https_worker/native/sspi.rs:77`), which reach none of
  the three spawn sites: `src/workspace_ops/handle_commit.rs:186, :260`, `handle_tag.rs:246` and `handle_log`
  (`src/operation/push_event.rs:546`) use their caller's backend.
- **std's spawn**: `setpgid` (`std/sys/process/unix/unix.rs:342`), SIGPIPE reset (`:359-372`), mask inherited
  (`:352-353`), `pre_exec` (`:383-385`; it forces the fork path, `:463`), `execvp` (`:408`); the parent reaps a failed
  `exec` (`:141-149`). Pipes: Linux `pipe2(O_CLOEXEC)`, macOS `pipe` then `FD_CLOEXEC` (`std/sys/pipe/unix.rs:28,
  :34-40`). Unstable: `setsid` (`std/os/unix/process.rs:220-221`), Windows `main_thread_handle` and attribute lists
  (`std/os/windows/process.rs:445-449, :342`); stable: `creation_flags` (`:181-182`), the process handle (`:37-39`).
  macOS `waitid` returns for a stopped child even with `WEXITED` alone (`go/src/os/wait_waitid.go:5-6`).
- **Cleanup classification.** `terminate` returns `CleanupPending` (`ErrorCode::Io`, `https_auth.rs:190`) when the tree
  is not confirmed ended within `CLEANUP_GRACE` (500 ms, `https_auth.rs:28`; `owner.rs:261-275`), in place of
  `Cancelled` or a `Timeout`'s `Interaction` (`https_worker.rs:377`); a loaded Windows host's drain outlasts it (B7).

## 3. Design

### 3.1 The registry

- **Crate.** `crates/children` (a gwz-core member by the `crates/*` glob, `Cargo.toml:50`; its own `BUILD.bazel`;
  depends on `libc`, `windows-sys` and, behind its `tokio` feature, tokio `sync` and `net`; never on gwz-core), with
  `unix` (`linux`, `macos` inside) and `windows` modules selected in one `cfg_if!`.
- **Value.** `ChildRegistry` is a cloneable handle (`Arc` inside); dropping the last runs `close()` (§3.4). A
  `SpawnGate` holds an optional registry handle and takes its own hold around the registry's spawn (as
  `spawn_gate.rs:431-436` does), so no cycle forms and a Windows creation is inside the gate. No static.
- **Where it comes from.** It rides inside the gate, which helper slots, request context and backend carry. The gate's
  `spawn` and `output` take a `Spec` and return the crate's `Child` (over a std child when the gate has no registry),
  which has no std `wait`, so std never reaps a registered child. Changed callers: `commit_log/mod.rs:354-356` (`Group`,
  `Kill`), the gwz-git `ProcessGate` impl (`spawn_gate.rs:450-454`; `Group`, `Wait`), the helper's Unix arm (§3.6).
  `composition::process_safety_with_children()` (beside `:31`) returns the `ProcessGate` with a new registry;
  `spawn_gate()` gives none, so the old entries spawn unregistered until 3.3, and `DriverOnly`'s requests until C gives
  it one (§3.7). 2.3r-2 adds public `Git2Backend::with_process_gate(&ProcessGate) -> Self`;
  `check_spawn_gate_minting.py` allows `ChildRegistry::create` only in `composition.rs`.
- **Creation precedes credentials.** `create` runs before the process holds any credential (gwz-cli before any thread,
  3.1; gwz-py when its composition object is made, before any lookup, 3.2), so the reaper's copy of memory holds none.
- **API.** `create(capacity)` (forks the reaper on Unix; capacity 256); `spawn(Command, Spec) -> Result<Child,
  RegistryFault>`; `output(Command, Spec)`; `kill_killable()`; `close()`. `Spec` is a target (`Group`; `Process {
  ignore_interrupts }`; `Session`) and an ending (`Kill`, `Wait`, `OwnInput`, `Detach`). `Child` gives its pipe ends,
  `exited()` (async with `tokio`), `try_exited()`, `kill()`, `retire()`. A `RegistryFault` reaches the member as
  `ErrorCode::Io` with exactly one message: `ReaperGone` `child-process reaper unavailable: <cause>` (cause `fork: <e>`,
  `exited` or `lifeline: <e>`, `<e>` as std's `io::Error` displays it); `Capacity` `child-process limit reached
  (<capacity>)`; `ForkedProcess` `child processes refused in a forked copy`; `Closed` `child processes refused:
  closing`; `Unsupported` `child-process mode unsupported on this platform`.
- **Entries.** A `Kill` or `Wait` child is an entry: registered with the reaper (Unix), given a kill-on-close job
  (Windows). An `OwnInput` child is tracked for its handle only (§3.7); a `Detach` child not at all after its spawn.
- **Fork check.** The registry records its creator's PID; every method and every `Drop` of `ChildRegistry` and `Child`
  compares it first, before any lock. In a forked copy a call is refused `ForkedProcess` and a drop does nothing, so the
  copy never kills, registers or unregisters on its parent's behalf.
- **Capacity.** On every platform the live entries never exceed the capacity (on Unix the reaper's preallocated table):
  a spawn beyond it is refused `Capacity` before the child starts.

### 3.2 Starting a child

- **Unix.** The registry sets `process_group(0)` for `Group` and adds its one `pre_exec` closure (no other gwz-core code
  uses `pre_exec`). In the child, after std's `setpgid` and SIGPIPE reset: `setsid()` for `Session`; an empty signal
  mask; SIGINT and SIGQUIT `SIG_IGN` for `ignore_interrupts`; for an entry, SIGPIPE `SIG_IGN`, the register record
  (§3.4) in one atomic `write` (below `PIPE_BUF`), SIGPIPE back to `SIG_DFL`, and the write's error returned, so with
  the reaper gone the program never runs. All of it is async-signal-safe.
- **Windows.** An entry is created `CREATE_SUSPENDED` through `creation_flags` under the gate's hold, assigned to a new
  unnamed, non-inheritable job with `KILL_ON_JOB_CLOSE | DIE_ON_UNHANDLED_EXCEPTION` and no breakaway; its one thread,
  found with `CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD)` by process ID (as `handshake_local_rows.rs:447` does), is
  opened `THREAD_SUSPEND_RESUME` and resumed. Any failure, or other than one thread, terminates the child and refuses
  the spawn. `Group` adds `CREATE_NO_WINDOW` (no console event reaches it); `Process` and `Session` add
  `CREATE_NEW_PROCESS_GROUP`. `kill()` is `TerminateJobObject`; `retire()` clears the limit; the end closes the job.
  `close()` takes the gate's hold, so a creation in flight is in its job or terminated before it returns; gwz terminated
  from outside inside that window leaves R-B14. The helper keeps its own launch (`windows.rs:54-62`). `Detach` is
  refused `Unsupported` (parked :808 needs a handle list, which std cannot express on stable).
- **Windows `ignore_interrupts`.** `CREATE_NEW_PROCESS_GROUP` disables only CTRL_C for the child and its descendants;
  CTRL_BREAK and CTRL_CLOSE still end it. C decides whether that suits each `Process` child (parked :809).

### 3.3 Exit observation, unregistration and ID reuse

- **Exit observation** (platform modules). Linux: `waitid(P_PID, pid, WEXITED | WNOWAIT)`. macOS, where that call also
  returns for a stop (§2): a kqueue `EVFILT_PROC` `NOTE_EXIT` on the PID, which does not reap; a registration refused
  `ESRCH` (already a zombie) is an exit once `waitid(P_PID, pid, WEXITED | WNOHANG | WNOWAIT)` reports `CLD_EXITED`,
  `CLD_KILLED` or `CLD_DUMPED` **(OS)**. On both, `ECHILD` from that call or the reap (another reaper took the child,
  R-B1) is an exit: the entry is unregistered at once, reaping nothing (B11). A stop or continue is never an exit and
  nothing spins meanwhile (B25). Until the reap the zombie pins the PID and, for a leader, its group ID.
- **Order.** A `Wait` entry is retired when its leader exits, so what its group leaves runs on as under 1.0.18 (R-B12).
  A `Kill` entry, when its owner releases it (an `output` call at once; the helper's owner after its decision), is sent
  `kill(-pgid, SIGKILL)` (`Group`, `Session`) unless retired. Then the waiter has the writer thread write the unregister
  record, waits for its acknowledgement, and reaps. `retire()` writes the unregister record first and kills nothing
  (TR1.6 :82). On Windows a `Wait` entry's waiter clears its job's limit at the leader's exit.
- **The writer thread.** One per registry makes every parent-side write; its mask blocks SIGPIPE, so a dead reaper is an
  `EPIPE`. The registry's core owns the write end and closes it only in `close()` or at process exit; a writer thread
  that dies sets `ReaperGone` and closes nothing, so its death kills no entry.
- **Why no kill reaches a reused ID (Unix).** gwz kills only live entries, pinned by a zombie or a live process; the
  reaper kills only at end of file and only entries still registered; records arrive in order and gwz reaps only after
  the unregister record is written. Left open (R-B1): gwz dies after a child exited and before its unregistration; std
  reaps a child after an `exec` failure; a process-wide reaper (`waitpid(-1)`, `wait()`, SIGCHLD `SIG_IGN`,
  `SA_NOCLDWAIT`) reaps a zombie the waiter has seen, after which gwz's own kills (`Child::kill`, the release kill,
  `kill_killable`, `close`) can reach a reused ID too; gwz-py calls that unsupported (§8 row 6).
- **Windows.** Every kill goes through a job or process handle, which holds the kernel object; no ID is reused.

### 3.4 The reaper (macOS and Linux)

- **Creation.** `create` makes the lifeline, opens `/dev/null`, allocates the table and a descriptor buffer, and forks;
  a failed fork leaves `ReaperGone(cause)`. The parent closes the read end and starts the writer thread and a watch
  thread that observes the reaper's exit as §3.3 observes a child's (a stopped reaper is not dead, B25) and reaps it.
- **Launch contract.** Only async-signal-safe calls, no allocation, no return into Rust or Python: `setsid()`; `SIG_IGN`
  for SIGINT, SIGQUIT, SIGHUP, SIGTERM, SIGTSTP, SIGTTIN, SIGTTOU, SIGPIPE; an empty mask; `chdir("/")`; `/dev/null`
  onto 0-2, the read end onto 3, every other descriptor closed (`close_range(4, ~0)` on Linux 5.9+, else the
  `/proc/self/fd` entries read with `getdents64`; on macOS `proc_pidinfo(PROC_PIDLISTFDS)` into the buffer, repeated
  until it lists only 0-3 **(OS)**), bounded by the open descriptors, not `RLIMIT_NOFILE`.
- **Records.** 16 bytes: magic (u16), version (u8), operation and target (u8), PID (i32), token (u64, never reused).
- **Loop.** It reads records (partial reads accumulated, `EINTR` retried) into the table by token, ignoring an
  unregister record for a token it does not hold. At end of file it sends SIGKILL to every entry still registered
  (`kill(-pgid)` for `Group` and `Session`, `kill(pid)` for `Process`) and calls `_exit(0)`. A read error, a bad magic,
  version or operation, or a register record with the table full: `_exit(1)`, killing nothing.
- **Its death.** The watch thread or the writer's `EPIPE` sets `ReaperGone`: every later entry spawn is refused before
  its program runs. Running children keep their in-process paths but lose cover against gwz's death (R-B2).
- **Its end.** `close()` (and the last drop) refuses new spawns (`Closed`), kills every live `Kill` entry, waits without
  a bound for every live `Wait` entry (B5), then closes the write end (Windows: the jobs); the reaper exits at end of
  file and the watch thread reaps it. An exit without `close()` closes the write end in the kernel and the reaper kills
  every entry still registered, `Wait` included (R-B4). `OwnInput` and `Detach` children are never killed.
- **Cost (OD-B3).** The reaper is a fork of the host: of gwz-cli before any thread (small), of Python when gwz-py's
  composition is made. Until gwz exits it keeps every page the parent writes after the fork, at most the parent's
  private writable memory at the fork. Under `vm.overcommit_memory=2` the fork can fail `ENOMEM` and every entry spawn
  is refused (R-B16). Linux's OOM killer counts the shared pages and may choose the reaper (R-B2). 3.4 measures both.

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
| Credential helper (Unix arm onto the registry; Windows arm unchanged) | `Group`, `Kill` | its lookup's bound, cancel and drop kill the group (`owner.rs:253-303`); success retires it | reaper; its job |
| `git rev-list` path cursor | `Group`, `Kill` | waited for (one `--max-count=1` step, `commit_log/mod.rs:337`); drop and `kill_killable` kill it; it writes nothing (`GIT_OPTIONAL_LOCKS=0`, `:347`) | reaper; its job |
| `git commit`, `git tag` | `Group`, `Wait` | waited for by its write sequence, by Force and by `close()`; never killed (B5); retired at its exit | reaper; its job (R-B4) |
| gwz-sspi worker (Windows) | unchanged | its supervisor | its job |

- **The helper's Unix arm** drops `tokio::process`, whose own reaping (§2) would reap a child the waiter pins. Its
  `HelperChild` becomes a struct over the crate's `Child` with the Windows arm's shape (`process_tree/windows.rs:44-48,
  :153-183`: three pipe fields, `wait`, `try_wait`, `start_kill`), its pipes `tokio::net::unix::pipe` ends
  (`tokio/src/net/unix/pipe.rs:419, :967`), so `runner.rs:123-141` and `owner.rs` outside `terminate` are unchanged.
  tokio's `process` feature leaves the candidate table (`Cargo.toml:139`) for the candidate dev-dependencies (`:152`),
  which the three test files use and no normal build reaches.
- **The helper on Windows** keeps its own job and is no entry, so `kill_killable()` misses it; it ends at its lookup's
  cancel (A's Cancel token) or with its job at gwz's exit.
- **The outcome stands (B7).** `terminate` returns the lookup's `Cancelled` or `Timeout`; a tree not seen ended within
  `CLEANUP_GRACE` is retained (`owner.rs:298`) for `reap_pending` (`:97`), counted by
  `AuthOwner::pending_cleanup_count()` (`:77`) in `https_worker.rs:303`'s count, A's `pending_local_work` (A §3.3).
- **Not the registry's.** The pager and `forall`'s commands (OD-C6): waited for by A, never killed (R-B8). `gwz hook`'s
  utilities and native route helpers stay as they are (R-B9, R-B5). A gate with no registry spawns in gwz's group
  (R-B13). What a helper, `git` or a broker starts in another session or job (pinentry, WMI or COM, a `setsid`
  descendant) is outside any group or job (TR1.6 :75; `process_tree.rs:21-24`; R-B7).

### 3.7 Interfaces to A and C

- **To A.** `kill_killable()` kills every live `Kill` entry and refuses later `Kill` spawns `Closed` (decision (i));
  `Wait` spawns stay allowed for a write sequence still running. `close()` is §3.4's. B relies on A revision 1's Force:
  `kill_killable()` at step 4, `close()` after the wait for write sequences (steps 5-6), exit after it returns (step 8).
  Unlike A revision 1 (A :216-217), a `Kill` spawn a parked network call makes after step 4 is refused `Closed`, not run
  until the reaper ends it; that member's report is A's. gwz-cli needs no `close()`: a `Wait` child is started only by
  `output` (`spawn_gate.rs:450-454`), which returns after its exit, and each `std::process::exit` in `run` precedes any
  command (`gwz-cli/src/lib.rs:162, :175-177, :187, :207, :350`) or follows its command's return (`:235, :248, :256,
  :259, :274, :281, :288, :299, :326, :344`). B changes no disposition in gwz's process: only a forked child before
  `exec` and the reaper do, one entry each for `crates/children` in `scripts/checks/process_globals_allowlist.json`.
- **To C.** `Process { ignore_interrupts: true }` keeps a child in the terminal's foreground group with SIGINT and
  SIGQUIT ignored, killed by PID (parked :1212, :1218), with §3.2's Windows difference. `Session` gives a new session
  (parked :809). `OwnInput` registers nothing and gets no job; the child ends at its input's end of file (parked :1224;
  Safety P2-4, :109), subject to R-B15. `Detach` (with `Session`, Unix only) outlives gwz by design, forgotten at once
  (R-B11). Which child takes which, how 2.3k (before 3.1) and `DriverOnly` (`driver_only.rs:61`) reach a registry, the
  Windows background launch, and the proof that each ends, are C's; until then `DriverOnly` spawns unregistered (R-B13).

### 3.8 Embedding (gwz-py)

gwz-py's composition object creates the registry (3.2), perhaps in a threaded process, which §3.4's child tolerates; its
close (3.2's atexit join) runs `close()`, which waits for a daemon thread's running commit and refuses that thread's
later spawns (R-B17). The README (§8 row 6) states R-B1, R-B3, R-B16, the reaper's memory and `os._exit`'s R-B4.

## 4. Steps

| Step | Content | Owns | Size | After | Review |
|---|---|---|---|---|---|
| **2.3r-1 (new)** | §3.1-§3.5 in `crates/children`; rows B1-B12, B20-B25 | create `crates/children/` (with its `BUILD.bazel`); gwz-core `Cargo.lock` (the package entry) | ~600 + 600 | nothing (the crate builds and tests alone) | single (State) |
| **2.3r-2 (new)** | §3.1's gate slot, `Spec` at the three call sites, `process_safety_with_children`, `with_process_gate`; §3.6: the helper's Unix arm, `rev-list` and the CLI route onto the registry, tokio's `process` feature to the dev table, `terminate`'s outcome; the two checks; rows B13-B17 | `src/git/spawn_gate.rs`, `src/git/gitbackend/backend.rs` (`:118-125`), `src/git/endpoint/https_auth/process_tree.rs` (Unix arm), `owner.rs` (`terminate`), `src/operation/commit_log/mod.rs` (`:330-358`), `src/transport_host/composition.rs`; gwz-core `Cargo.toml`, `Cargo.lock`, `BUILD.bazel` (one label); `scripts/checks/check_spawn_gate_minting.py`, `check_process_globals.py` and its allowlist | ~350 + 360 | 2.3r-1 | dual (it moves the helper's spawn, whose environment carries secrets); the brief names §3.1's invariant "creation precedes credentials" |
| 2.3j (amended) | spawns nothing new; ordered after the registry by D-V (3) | as planned | as planned | 2.3g, 2.3r-2 | as planned |
| 3.1 (amended) | `run` calls `process_safety_with_children()` after `worker_host::early` and before any thread; the host and every hostless backend take it through `with_process_gate` (`dispatch.rs:93`, `log_exec.rs:118`, `hook/family.rs:63, :149, :187`); row B18 | those lines in 3.1's files | +40 | as planned | phase |
| 3.2 (amended) | the composition object creates the registry once, before any lookup, and its close runs `close()`; the host and gwz-py's 8 hostless backend lines take it; README text; rows B19, B26 | those lines in 3.2's files | +50 | as planned | phase |
| 3.4 (amended) | the reaper's fork time and unique memory: gwz-cli after the parent writes 100 MiB; gwz-py with a 2 GiB heap, then 1 GiB written after the fork (M, L) | evidence | | as planned | |

- **Waves (2.3 plan §4).** Lane 3: 2.3e then 2.3r-1 in wave 2, 2.3r-2 then 2.3j in wave 3; 2.3r-1 shares only
  `Cargo.lock`, merged serially; at most three compiling lanes. 2.3r-1 exceeds the 500-line aim: registry, reaper and
  jobs build and test only together. 2.3r-2's files are outside every 2.3 sub-step's (2.3 plan :240-242) and the Phase 1
  trunk's (plan :324-326); a file another lane holds is asked for (plan :330-331).
- **Interim.** Until 3.1 and 3.2, gwz-cli's and gwz-py's backends and the old entries spawn unregistered, as today;
  after them the non-candidate build does, and `DriverOnly` until C gives it a registry (§3.7; R-B13; OD-B2 (a)).

## 5. Rows

Each is red first unless a leg is **(reg)**. M macOS, L Linux, W Windows. "Harness": a test binary of `crates/children`
(B1-B12, B20-B25), of gwz-core (B13-B17) or gwz-py (B19, B26); a fixture `sh` child starts a sleeping grandchild.

1. **B1** SIGKILL of the harness while a registered `Group` child and its grandchild run: both gone within 1 s. W:
   `TerminateProcess` of the harness; the job ends both within 1 s. (M, L, W)
2. **B2** SIGKILL, SIGQUIT, SIGINT-then-SIGKILL to the harness's whole group, and a hung-up pseudo-terminal: the reaper
   outlives each and the registered group is gone within 1 s. (M, L)
3. **B3** A retired child's grandchild lives after the harness's exit and its SIGKILL; a non-retired `Kill` child whose
   leader exited leaves no member; the reaper's trace shows no kill of an unregistered token and each unregister record
   before its reap. W: a retired job's grandchild lives **(reg)**. (M, L, W)
4. **B4** SIGKILL of the harness right after `spawn` returns: the group is gone within 1 s, also with `ulimit -n` at its
   hard maximum (L without `close_range`, a container limit of 2^20; M). (M, L)
5. **B5** The reaper SIGKILLed alone: `ReaperGone` within 1 s; the next spawn is refused and never runs; a running child
   still ends by its handle. An injected fork `ENOMEM`: spawns refused with §3.1's exact message. (M, L)
6. **B6** `lsof`: the harness holds no read end; the reaper only it and `/dev/null` on 0-2; under 50 concurrent spawns
   no child holds either end (L; M with the registry made first); a harness `flock` frees at its exit. (M, L)
7. **B7** The harness started with 0-2 closed: both ends are 3 or above and SIGKILL still ends its child. Faults:
   bad-magic bytes (the reaper exits, kills nothing, `ReaperGone`); a writer-thread panic (no child killed, later entry
   spawns refused); a token counter started near its wrap (no collision). (M, L)
8. **B8** A spawn from a thread with SIGINT and SIGPIPE blocked: the child's mask is empty and SIGPIPE `SIG_DFL`; a
   spawn after the reaper died fails, and no child died of SIGPIPE. (M, L)
9. **B9** A copy forked without exec while another thread holds the registry's lock: calls refused `ForkedProcess`; its
   drop of a live `Kill` entry's `Child`, `kill_killable()` and `close()` leave the parent's child running, and it exits
   without deadlock; the parent's SIGKILL still ends its children once the copy exited. (M, L)
10. **B10** A full registry refuses `Capacity`; 300 spawns of a missing program are each refused and the live count is
    zero; on M and L every registered token has its unregister record. (M, L, W)
11. **B11** A thread reaping with `waitpid(-1)`; SIGCHLD `SIG_IGN`; `SA_NOCLDWAIT`: an exited child's entry is
    unregistered at once (`ECHILD`; M by both `NOTE_EXIT` and the `ESRCH` path), the reaper never kills it. gwz-side
    kills after an external reap are R-B1's. (M, L)
12. **B12** A child that spawns a grandchild first: both in the job; `GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0)`
    reaches neither. An injected resume failure: the child terminated, the spawn refused, no process left. `close()`
    during a creation held at a seam returns after the assignment. (W)
13. **B13** [2.3r-2] A sleeping `HelperSlots` helper: harness SIGKILL ends its group in 1 s; W: job **(reg)**. (M, L, W)
14. **B14** [2.3r-2] A path-filtered history walk: `rev-list` leads its own group; `kill_killable()` and the harness's
    SIGKILL each end it within 1 s. W: a job and no console. (M, L, W)
15. **B15** [2.3r-2] A CLI-route commit, `pre-commit` sleeping, `post-commit` starting a background `sleep`: SIGINT to
    the harness's group reaches neither `git` nor the hook, `kill_killable()` leaves it running, `close()` returns after
    it, the `sleep` outlives `git`. A `pre-commit` that never exits: the harness's SIGKILL ends both within 1 s,
    `index.lock` remains (R-B4). W: CTRL_BREAK reaches neither; a hook reading `CONIN$`: whether it waits or fails
    without a console is recorded, not asserted (R-B10). (M, L, W)
16. **B16** [2.3r-2] The drain held past `CLEANUP_GRACE` (a `cfg(test)` seam in `owner.rs`): a cancelled lookup returns
    `Cancelled`, a timed-out one `Timeout` with `Interaction`; `AuthOwner::pending_cleanup_count()` (`owner.rs:77`) is
    one until `reap_pending` reaps it within the existing rows' 30 s (`https_auth/runner_tests.rs:299`). (M, L, W)
17. **B17** [2.3r-2] The checks fail on `ChildRegistry::create` outside `composition.rs` and on `tokio::process`,
    `libc::fork`, `pre_exec` in gwz-core production outside the crate; `cargo tree -e features,no-dev` shows no tokio
    `process` for gwz-core, gwz-cli and gwz-py. (M, L, W)
18. **B18** [3.1] `gwz fetch` with a sleeping helper, `gwz commit` with a sleeping hook, `gwz log -- <path>`: one
    registry, `run`'s, holds every spawn; gwz's SIGKILL ends each child within 1 s **(reg)**. (M, L; W **(reg)**)
19. **B19** [3.2] gwz-py's fetch, commit and log: one registry holds every spawn; SIGKILL of Python mid-lookup ends the
    helper within 1 s **(reg)**; with the reaper SIGKILLed (M, L), the next fetch raises `ErrorCode::Io` with §3.1's
    exact message and no helper runs. (M, L; W **(reg)**)
20. **B20** A `Process { ignore_interrupts: true }` child: SIGINT and SIGQUIT to the harness's group leave it running in
    that group; at the harness's SIGKILL the reaper kills it by PID within 1 s while an unregistered `tee` in the group
    survives. W: CTRL_C leaves it running, CTRL_BREAK ends it, `TerminateProcess` of the harness ends it. (M, L, W)
21. **B21** An `OwnInput` child: no register record, in none of the registry's jobs (`IsProcessInJob` against each);
    `close()` leaves it running; at the harness's SIGKILL or `TerminateProcess` it exits 0 at end of file. (M, L, W)
22. **B22** `kill_killable()` ends a live `Kill` child and its grandchild at once and refuses a later `Kill` spawn
    `Closed`; `Wait` spawns, `Wait` and `OwnInput` children run on; after `close()` no spawn runs its program. (M, L, W)
23. **B23** A `Wait` child's background grandchild outlives the child's reap, `close()` and the harness. (M, L, W)
24. **B24** A `Session` child's session ID is its PID; it and its grandchild are gone within 1 s of the harness's
    SIGKILL; a `Session` + `Detach` child writes no record and outlives it. W: `Detach` refused `Unsupported`. (M, L, W)
25. **B25** A `Wait` and a `Kill` child each stopped by SIGSTOP and by a real SIGTTIN under a pty, and the reaper
    stopped by SIGSTOP: for 5 s nothing is retired, released or `ReaperGone`, no thread uses over 5% CPU, the reaper
    holds both; after SIGCONT and exit the normal order runs; the harness's SIGKILL while the children are stopped ends
    both groups within 1 s. An at-once exit is seen (M: the `ESRCH` path). (M, L)
26. **B26** [3.2] A gwz-py script whose daemon thread commits with a sleeping hook calls `sys.exit(0)`: the commit
    completes before the process exits, no `index.lock` remains, a later member's commit is refused `Closed`. (M, L, W)

## 6. Decisions

- **OD-B1 The CLI route's `git commit` and `git tag`: (a), decided** (decision (ii)): own group (no console on Windows),
  waited for by every cancel, by Force and by `close()`, killed only when gwz is gone. Full cost: a hook or signer
  reading the terminal stops on SIGTTIN or SIGTTOU (Unix) or prompts on a hidden console (Windows; 1.0.18 shared gwz's
  console, `v1.0.18:src/git/gitbackend/repository.rs:299-305`, `refs.rs:172-183`); a hung hook or signer makes `gwz
  commit` wait through Cancel and Force until gwz dies (SIGKILL, or an interrupt after Force restored the default, A O3
  (a)), and the reaper or job then kills `git`'s group mid-write, leaving `index.lock` (R-B4, R-B10) where 1.0.18's
  Ctrl-C let `git` remove its locks. Not chosen: (b) gwz's group; SIGINT to `git`'s group at Force (Safety's (c)); on
  Windows a kept console with `CREATE_NEW_PROCESS_GROUP` (visible prompts; CTRL_BREAK ends `git`).
- **OD-B2 Gates without a registry: (a), decided** (decision (iii)): they spawn as today. After 3.1 and 3.2 that is the
  non-candidate build, and `DriverOnly` until C gives it a registry (§3.7); no gwz-core-internal hostless backend
  reaches the three spawn sites (§2). Cost (R-B13): an interrupt reaches `git` and its hooks mid-write, as on 1.0.18.
- **OD-B3 Every command forks the reaper: (a), decided** (decision (iii)); cost in §3.4 and R-B16, measured in 3.4.
- **Choices open to review:** capacity 256; the group kill of a non-retired `Kill` group at its leader's exit; one
  writer thread per registry; suspended creation with a Toolhelp resume rather than generalizing the helper's job-list
  launch; no re-fork of a dead reaper; no Linux pidfd (D-V (2) says `kill()`); kqueue only on macOS.

## 7. Residuals for design D

| # | What can remain | When | How it is recognized |
|---|---|---|---|
| R-B1 | A SIGKILL to a process or group that reused a registered ID | gwz dies just after a registered child exited and before its unregistration; a child reaped early (std after an `exec` failure); an application's process-wide reaper, which also exposes gwz's own kills while it lives (unsupported) | Not recognizable afterwards |
| R-B2 | Children that outlive a later SIGKILL of gwz | The reaper was killed alone, or with gwz by name (`pkill -9 gwz`, `killall -9 python`), or chosen by the OOM killer | Spawns fail naming the reaper; children run on with parent PID 1. A stopped reaper (SIGSTOP) delays this until continued |
| R-B3 | Registered children outlive gwz until another process exits | A fork without exec in an embedding application; on macOS a child another thread started while the lifeline was made | The reaper (a copy of gwz or Python in its own session) still runs after gwz died |
| R-B4 | Git `*.lock` files and a partly run hook | gwz dies while a CLI-route child runs (SIGKILL, a crash, an interrupt after Force restored the default) or exits without `close()` (`os._exit`, a foreign `exit()`); the reaper or job kills it | D's `*.lock` report (OD-C3); a hook's effects are the user's |
| R-B5 | A native route helper still waiting | gwz exits by SIGKILL, a crash or A's Force during `Cred::credential_helper` (on Windows Ctrl-C never reaches it) | A `git credential-*` process with parent PID 1, as on 1.0.18 |
| R-B6 | What a successful helper left running (`credential-cache`) | Every successful lookup (TR1.6 :82) | Not gwz's; accepted design |
| R-B7 | Processes another program or broker started (pinentry, WMI or COM, a `setsid` descendant, `ssh`'s children) | Any | In no group or job of gwz's; as on 1.0.18 |
| R-B8 | The pager or a `forall` command | gwz is SIGKILLed while it runs (OD-C6) | The user's program is still on the terminal |
| R-B9 | `gwz hook`'s utilities; what `git worktree add` leaves if interrupted | gwz is killed during `gwz hook` | Short commands end alone; `git worktree list` |
| R-B10 | `gwz commit` waiting on a stopped, hidden or hung reader | OD-B1 (a): a hook or signer reads the terminal or console, or hangs | Unix: `ps` shows the reader stopped (`T`) in `git`'s group; W: the reader alive in `git`'s own job (§3.2) with no visible console |
| R-B11 | The served form's background server | Every `gwz server start` without `--foreground` (2.3k) | `gwz server status`, `list` (C) |
| R-B12 | A background job a commit or tag hook left (`ctags … &`) | Every such commit; it outlives `git` and gwz | As on 1.0.18, in `git`'s former group |
| R-B13 | A registry-less `git commit`, `git tag` or `rev-list`, or a hook's partial effects | Non-candidate build (M, L, W), and `DriverOnly`'s requests until C (§3.7): an interrupt reaches it mid-write, as on 1.0.18 (`git` removes its own `*.lock` on SIGINT); gwz's death leaves it running | A `git` process with parent PID 1; R-B4's `*.lock` files |
| R-B14 | A suspended child outside any job, holding inherited handles (a pipeline may never see end of file) | W: gwz terminated from outside between its creation and its job assignment | A suspended process whose parent is gone |
| R-B15 | An `OwnInput` child that misses its end of file | M: another thread's spawn inherits its input inside std's `pipe`-then-`FD_CLOEXEC` window (C decides) | The child runs on after gwz is gone |
| R-B16 | Every entry spawn refused | The reaper's fork fails (`ENOMEM` under strict overcommit) | The refusal names the reaper and `fork` |
| R-B17 | Later members of a commit not committed | gwz-py's close runs while a daemon thread's commit loop continues | Those members' changes are uncommitted |

## 8. Superseded or amended clauses (applied by the lane owner at acceptance)

| # | Document and clause | Replacement or addition |
|---|---|---|
| 1 | 2.3 plan §3, after 2.3l (:220-230) | Add: "**2.3r-1 Child registry and reaper (single, State).** Goal: plan §10 D-V resolution (2). Owns: create `crates/children/`; gwz-core `Cargo.lock`. Content and red rows: GwzChildLifetimeDesign §3.1-§3.5, rows B1-B12, B20-B25. Size ~600 + 600. After: nothing. Evidence: Mac, WSL, Windows receipt." and "**2.3r-2 Spawns onto the registry (dual: secret handling; the brief names the invariant 'creation precedes credentials').** Goal: D-V resolution (2) for gwz-core's three spawn sites and its public backend setter. Owns: GwzChildLifetimeDesign §4. Rows B13-B17. Size ~350 + 360. After: 2.3r-1. Evidence: Mac, WSL, Windows receipt." |
| 2 | 2.3 plan 2.3j, "After: 2.3g (stdio serves through it)." (:202) | "After: 2.3g (stdio serves through it) and 2.3r-2 (plan §10 D-V resolution (3))." |
| 3 | 2.3 plan §4 table, waves 2 and 3, lane 3 (:240-241) | Wave 2 lane 3: "2.3e address, then 2.3r-1 registry"; wave 3 lane 3: "2.3r-2 spawns, then 2.3j credentials and stdio"; reasons: "r-1 owns `crates/children/` and shares only `Cargo.lock`; r-2 owns the files GwzChildLifetimeDesign §4 lists". |
| 4 | 2.3 plan §4, "Review: dual on a, d, j, l" and totals (:249-250) | "Review: dual on a, d, j, l and r-2" and "about 5,250 implementation lines and 5,710 rows across 14 steps". |
| 5 | Plan §4, 3.1 (:721-728), 3.2 (:729-735), 3.4 (:739) | 3.1: "`run` creates the child registry after `worker_host::early` and before any thread; the host and every hostless backend take it through `Git2Backend::with_process_gate` (GwzChildLifetimeDesign §4, row B18). +40." 3.2: "The composition object creates the child registry once, before any lookup, and its close runs the registry's `close()`; the host and every hostless backend take it (rows B19, B26). +50." 3.4: "and the reaper's fork time and unique memory, gwz-cli and gwz-py (M, L)." |
| 6 | `gwz-py/README.md`, "Platform And Status" (:163) | Add: "gwz starts one helper process per Python process that ends gwz's child processes if Python is killed. It is a fork of Python and keeps a copy of memory Python changes later; if the fork fails (for example under strict memory overcommit), gwz refuses to start processes. After `os.fork()` without exec (for example `multiprocessing`'s `fork` start method) gwz refuses to start processes in the child, and the parent's helper waits until the child exits; use `spawn`, or `forkserver` without preloading gwz. Reaping all children (`waitpid(-1)`) or ignoring SIGCHLD is unsupported: gwz may then signal an unrelated process. `os._exit` during a commit can leave `index.lock`." |
| 7 | Contract §6, "Composition/CleanupDomain" (`GwzConnectionApiContract.md:289`) | Append to "State owned": "; the child registry, starter and kill authority of every child a gate with a registry starts, with its reaper (GwzChildLifetimeDesign §3.6 lists the children it does not start)". |

Rows 5-6 apply OD-B3 (a) and OD-B2 (a). History stays unedited: TR1.6 "Its cost" (:76) and OQ2 (:398), withdrawn by D-V
(:1005-1010), and `gwz-core/dev-docs/history/GwzTransportCredentialHelpers-ReviewCode-2.md:66` (`CleanupPending`),
superseded by §3.6 from 2.3r-2. 2.3k's background copy is C's (§3.7).

## 9. Changes from revision 1

IDs as in `GwzChildLifetimeDesign-RemPlan.md`. Decisions (i)-(iii): B3, B5, §3.4, §3.7, §6. S-P2-1: §2, §3.3, §3.4, B25,
R-B2. S-P2-2: B4, §3.3, R-B1, §8 row 6, B11. S-P2-3, S-P3-3: §3.3, §3.4, §3.7, §3.8, R-B4, R-B17, B7, B26. S-P2-4,
C-P2-1: OD-B1, R-B10, B15. S-P2-5: §3.1, §3.4, R-B16, §4, §8 rows 5-6, B5, B19. C-P2-2: B1, B5, §2, §3.1, §3.6, §3.7,
§4, OD-B2, R-B5, R-B13. S-P3-1: §3.2, R-B14, B12. S-P3-2, S-P3-7: §3.4, B4, B24. S-P3-4: §3.1, B9. S-P3-5: R-B2. S-P3-6,
C-P3-4: §2, §3.1, §3.6, §4. S-P3-8, C-P3-6: B7, §2, §3.6, B16. S-P3-9: §3.1, §4, §8 row 1. S-P3-10: §3.5, §3.7, R-B15.
C-P3-1: §8 row 7. C-P3-2: §3.6. C-P3-3: revision 1's §8 row 5 removed, §3.7. C-P3-5: §3.1, B10, B21. C-P3-7: §2, §6.
C-P3-8: B5.
