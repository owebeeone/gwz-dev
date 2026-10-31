# Child-process lifetime: design B of the cancellation split (DRAFT)

Status: DRAFT, revision 1, 2026-11-01; design B of the cancellation split (D-V); for dual review. Authorizes no
implementation.

**Citations** are at root `b4b827d6`, gwz-core `b745f1fa`, gwz-cli `c0ba717`, gwz-py `2185cc6`, gwz-git `9bc4ea9`,
gwz-sspi `6f83551`, git2-rs `d13951f`; Rust std 1.95.0 (`gwz-core/rust-toolchain.toml`), cited as `std/` for its
`library/std/src/`; tokio 1.53.1 (`gwz-core/Cargo.toml:139`), cited as `tokio/`. A path that begins with a member
name is workspace-relative; every other source path is relative to `gwz-core/`. "Plan" is
`GwzTransportReleaseConvergencePlan.md`, "2.3 plan" `GwzServerCore-Step2.3-Plan.md`, "parked"
`history/parked/GwzCoreServerDesign.md`, "TR1.6" `gwz-core/dev-docs/history/reference/GwzTransportCredentialHelpersDesign.md`,
"r1" the cancellation amendment's revision 1 (blob `3448c561`), "Safety" its round-1 review
`history/GwzCancellationAmendment-ReviewSafety.md`, all under `dev-docs/` unless a member is named. Source material,
not authority: `history/GwzCancellationAmendment-DRAFT.md` (revision 2), its reviews and its 31 open gaps.

## 0. Scope

Every process gwz-core starts for an operation ends no later than that operation and no later than gwz itself,
however gwz ends: an orderly exit, a cancel, a forced exit, SIGKILL, a crash or the console closing. This design
states the child registry that starts every such process (owned by the composition, passed explicitly, never a
global), the reaper that ends registered children on macOS and Linux when gwz is gone, the Windows job objects that
do the same there, the kinds of child that exist today and how each is ended, why no kill reaches a reused process ID,
the reaper's own death, the lifeline's descriptor inheritance, and where the work lands in Phase 2 and Phase 3.

| Not in B | Owner |
|---|---|
| The interrupt source, the cancellable entry, safe points, the forced exit's order, exit status and what a cancelled member reports | A |
| The served form: which child takes which target and ending, how the `gwz server stdio`, `ssh` and background server children end, the protocol on cancel | C (B gives only the spawn interface, §3.7) |
| Temporaries, locks and partial state a killed child or gwz leaves; the operator's list of accepted residuals | D (B hands its residuals to D in §7) |

## 1. Requirements

Each traces to plan §10 D-V (:999-1054).
- **B1 No orphan.** A child gwz-core starts for an operation ends no later than the operation and no later than gwz
  (OQ2 reversed, :1005-1008: "every helper process it started … no orphaned process"). The one process that is not an
  operation's child, the served form's background server, is C's (§3.7).
- **B2 Death is detected.** The reaper detects gwz's death, gwz detects a child's and the reaper's (:1007).
- **B3 The registry and the reaper.** The reaper lives in gwz-core, is forked when the child registry is created,
  its loop only reads the lifeline and calls `kill()`, and gwz-cli and gwz-py share it; fork without exec in an
  embedding application is a documented constraint (resolution (2), :1039-1043). On Windows every registered child
  gets a job (Safety P3-3, :256-263; r1 :148-149 for the helper).
- **B4 Never the wrong process.** A kill reaches only a registered child or its group (needed by B1's "every helper
  process it started", and by D-V's "no … partial state" (:1008) for processes gwz does not own).
- **B5 Writes run on.** A child that is part of a local write sequence is not killed while gwz lives (resolution (1),
  :1035-1037: "a local write sequence in progress runs to its next safe point").
- **B6 Placement.** The registry and reaper are a Phase 2 sub-step before 2.3j (resolution (3), :1043).
- **B7 Rules.** No global or thread-local (OD-C1 allows only A's handler); braces; `cfg` only in `cfg_if!` or platform
  modules; no production code under `tests/`; the same behaviour on macOS, Linux and Windows except where §3.2 says
  how a platform's mechanism differs.

## 2. What exists

| Child | Where it is started | Today on Unix | Today on Windows |
|---|---|---|---|
| Credential helper `git credential fill` | `src/git/endpoint/https_auth/runner.rs:110` → `process_tree.rs:57-75` | `tokio::process` child (`:42`), leader of its own group (`:71`); `kill(-pgid, SIGKILL)` (`:79-86`) on bound, cancel and drop (`owner.rs:253-303`); `retire` leaves its group running (`process_tree.rs:89-90`) | own `KILL_ON_JOB_CLOSE` job from its first instruction via a job list (`process_tree/windows.rs:54-80`, `windows/launch.rs:1-3`), `CREATE_NO_WINDOW` (`:224`); `retire` clears the limit (`windows.rs:115-118`) |
| `git rev-list` (path-filtered `log`) | `src/operation/commit_log/mod.rs:330-358` | `SpawnGate::spawn`, gwz's group, waited for, not killed | std spawn, no job |
| `git commit`, `git tag` (Git CLI route) | gwz-git `src/native_commit.rs:88-95`; gwz-core `src/git/spawn_gate.rs:450-454` | `SpawnGate::output` (`:438-446`), gwz's group, waited for | std spawn, no job |
| gwz-sspi worker | gwz-sspi `src/supervisor/windows/launch.rs:205-280` | none (`src/supervisor/platform.rs:1-9`) | own job (`:276-280`), `CREATE_SUSPENDED` and `CREATE_NO_WINDOW` (`:212-213`), ended by its supervisor (`handles.rs:50-56`) |
| Native route helpers | git2-rs `src/cred.rs:124, :387-420`, called at `src/git/gitbackend/transport_support.rs:313-321` | spawned and waited inside one git2-rs call, gwz's group | the same |

- These are gwz-core's only production spawns: every other `Command::new` in `src/` is under `cfg(test)` (for example
  `src/diff/output.rs:271`). `tokio::process` is used only at `process_tree.rs:42` in production, and in three test
  files (`src/transport_host/https_tests.rs:24`, `src/git/endpoint/https_auth/process_tree/windows/tests.rs:505`,
  `src/git/endpoint/ssh_controlled_server/handler.rs:18-21`, a `cfg(test)` module, `src/git/endpoint/mod.rs:130-137`).
  tokio is a dependency only of the candidate build (`Cargo.toml:132-139`); it installs a SIGCHLD handler at a process
  child's spawn when Linux pidfd is not used (`tokio/src/process/unix/mod.rs:123-137`).
- gwz-cli's own spawns are not gwz-core's: the pager (`gwz-cli/src/pager.rs:157-170`), `forall`'s commands
  (`gwz-cli/src/forall.rs:231-262`), and `gwz hook`'s `df`, `stat`, `mount`, `cp`, `fsutil`
  (`gwz-cli/src/hook/env.rs:158-348`) and `git` (`gwz-cli/src/hook/fallback.rs:21-25`). The gwz binary is also the
  gwz-sspi worker, which `run` hands off before anything else (`gwz-cli/src/lib.rs:160-163`).
- **The gate.** `transport_host`, and so the composition, exists only in the candidate build (`src/lib.rs:47-51`). The
  composition makes the process's gate (`composition.rs:30-35`); the old entries make one per call through
  `spawn_gate()` (`:37-40`; `local_command.rs:79`, `cancellable.rs:238`). A backend with no host has an ungated one
  (`src/git/gitbackend/backend.rs:69, :114`); the only setter, `with_host_context`, is `pub(crate)` and candidate-only
  (`:118-125`). gwz-cli builds such backends at `globalargs/dispatch.rs:93`, `log_exec.rs:118` and
  `hook/family.rs:63, :149, :187`; gwz-py's `native/src` names `Git2Backend::new()` on 8 lines. The gate holds no
  registry and nothing holds the gate but its clones (`spawn_gate.rs:153-155`).
- **Rust std's spawn** (fork path) runs `setpgid` (`std/sys/process/unix/unix.rs:342`), resets SIGPIPE to `SIG_DFL`
  (`:359-372`), inherits the signal mask (`:352-353`), runs the `pre_exec` closures (`:383-385`), then `execvp`
  (`:408`); a closure forces the fork path (`:463`). On an `exec` failure the parent reaps the child before returning
  the error (`:141-149`). Pipes are `pipe2(O_CLOEXEC)` on Linux and `pipe` then `FD_CLOEXEC` on macOS
  (`std/sys/pipe/unix.rs:28, :34-40`). On stable, `CommandExt::setsid` is unavailable (`std/os/unix/process.rs:220-221`),
  and on Windows so are `ChildExt::main_thread_handle` (`std/os/windows/process.rs:445-449`) and attribute lists
  (`:342`); `creation_flags` (`:181-182`) and the child's process handle (`:37-39`) are stable.
- **Cleanup classification (f9).** `terminate` returns `CleanupPending` when the tree is not confirmed ended within
  `CLEANUP_GRACE` (500 ms, `src/git/endpoint/https_auth.rs:28`; `owner.rs:261-275`), which replaces the lookup's
  `Cancelled` or `Timeout` and maps to `ErrorCode::Io` (`https_auth.rs:190`); a `Timeout` would map to
  `SetupFailureCause::Interaction` (`src/git/endpoint/https_worker.rs:377`). On a loaded Windows host
  `TerminateJobObject` drains past the grace, so a Ctrl-C can read as an I/O failure
  (`build-targets/winflk-logs/f9-report.md`, "Product classification limitation").

## 3. Design

### 3.1 The registry

- **Crate.** `crates/children` (a gwz-core workspace member by the `crates/*` glob, `Cargo.toml:50`; its own
  `BUILD.bazel` as `crates/session-host/` has; depends on `libc`, `windows-sys` and, behind its `tokio` feature, tokio
  `sync` and `net`; never on gwz-core), with `unix` and `windows` platform modules selected in one `cfg_if!`.
- **Value.** `ChildRegistry` is a cloneable handle (`Arc` inside); dropping the last handle closes it (§3.4). It holds
  no gate and no reference to its holders; a `SpawnGate` holds an optional registry handle and takes its own hold
  around the registry's spawn (as `spawn_gate.rs:431-436` does around std's), so the two never form a cycle and a
  Windows creation is inside the gate as `spawn_gate.rs:1-11` requires. No static, no thread-local.
- **Where it comes from.** It rides inside the gate, so every path that carries the gate carries the registry with no
  new parameter: the host's helper slots, the request context and the backend. `SpawnGate::spawn` and `output` become
  the registry's spawns when the gate has one; their callers (`commit_log/mod.rs:354`, `spawn_gate.rs:450-454`) do not
  change. `composition::process_safety_with_children()` (beside `:31`) returns the existing `ProcessGate` with a new
  registry; `spawn_gate()` (`:38`) also makes one, so each old entry call has its own until 3.3 removes them. 2.3r-2
  adds `Git2Backend::with_process_gate(&ProcessGate) -> Self`, public, beside `with_host_context` in its `cfg_if!`, so
  gwz-cli and gwz-py can hand the root's registry to a hostless backend. A gate with no registry (`ungated()`, and
  every gate of a non-candidate build) spawns as today, unregistered (R-B13, OD-B2). `check_spawn_gate_minting.py`
  gains `ChildRegistry::create`, allowed only in `composition.rs`.
- **API.** `create(capacity) -> ChildRegistry` (forks the reaper on Unix; capacity 256 by default);
  `spawn(Command, Spec) -> Result<Child, RegistryFault>`; `output(Command, Spec) -> io::Result<Output>`;
  `kill_killable()`; `close()`. `Spec` is a target (`Group`; `Process { ignore_interrupts }`; `Session`) and an ending
  (`Kill`, `Wait`, `OwnInput` or `Detach`). `Child` gives its three pipe ends, `exited()` (blocking; async with the
  `tokio` feature), `kill()`, `retire()` and its status. `RegistryFault` is `Capacity`, `ReaperGone`, `ForkedProcess`,
  `Closed` or `Unsupported`.
- **Entries.** A `Kill` or `Wait` child is an entry: registered with the reaper (Unix) or given a kill-on-close job
  (Windows). An `OwnInput` child is tracked for its handle only: no record, no job, never killed by the registry; it
  ends at its input's end of file (§3.7). A `Detach` child is not tracked after its spawn returns.
- **Fork check.** The registry records its creator's PID; in any other process (a fork without exec) every call is
  refused `ForkedProcess` before anything starts, so a forked copy never registers with, or unregisters from, its
  parent's reaper.
- **Capacity.** gwz's live entry count never exceeds the reaper's preallocated table: a spawn beyond it is refused
  `Capacity` before the child starts.

### 3.2 Starting a child

- **Unix.** The registry sets `process_group(0)` for `Group` and adds its one `pre_exec` closure (the crate is the only
  user of `pre_exec`). In the child, after std's `setpgid` and SIGPIPE reset (§2), the closure: for `Session` calls
  `setsid()` (std's own is unstable, §2); sets the signal mask empty (`sigprocmask`), so no caller's blocked signals
  pass to the program; for `Process` with `ignore_interrupts` sets SIGINT and SIGQUIT to `SIG_IGN`; for an entry, sets
  SIGPIPE to `SIG_IGN`, writes the 16-byte register record (magic, version, operation, target, token, `getpid()`) with
  one `write` (atomic, below `PIPE_BUF`), sets SIGPIPE back to `SIG_DFL`, and returns the write's error, so with the
  reaper gone the spawn fails and the program never runs. Every call is async-signal-safe.
- **Windows.** No reaper. An entry is created with `CREATE_SUSPENDED` through `creation_flags` (stable, §2), under the
  gate's hold; it is assigned to a new unnamed, non-inheritable job with `KILL_ON_JOB_CLOSE |
  DIE_ON_UNHANDLED_EXCEPTION` and no breakaway through the child's process handle; its one thread is then found with
  `CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD)` filtered by the child's process ID (pinned by the open handle; the same
  call a test already makes, `src/transport_host/carrier/tests/handshake_local_rows.rs:447`), opened with
  `THREAD_SUSPEND_RESUME` and resumed. Any failure, or other than one thread, terminates the child and refuses the
  spawn. `Group` adds `CREATE_NO_WINDOW` (no console, so no console control event reaches it, as the helper,
  `windows/launch.rs:224`); `Process` and `Session` add `CREATE_NEW_PROCESS_GROUP`. `kill()` is `TerminateJobObject`;
  `retire()` clears the limit (as `windows.rs:115-118`); the end closes the job, which kills any member left. The
  helper keeps its own launch (`windows.rs:54-62`), which has this shape with no suspension. `Detach` is refused
  `Unsupported`: the background copy needs a handle list (parked :808), which std cannot express on stable (§2).
- **Windows `ignore_interrupts` (the platform difference B7 names).** `CREATE_NEW_PROCESS_GROUP` disables only
  CTRL_C for the child and its descendants; a console's CTRL_BREAK and CTRL_CLOSE still end it unless the program
  installs its own handler, which a parent cannot do for it. Unix ignores SIGINT and SIGQUIT. C decides whether that
  is acceptable for each child it starts as `Process` (parked :809 has the stdio child, gwz itself, ignore CTRL_BREAK).

### 3.3 Unregistration, reaping and ID reuse

- **Order.** A waiter thread per entry blocks in `waitid(P_PID, pid, WEXITED | WNOWAIT)`: the child stays a zombie, so
  its PID, and for a group leader its group ID, cannot be reused. A `Wait` entry is retired when its leader exits, so
  what its group leaves (a hook's background job) runs on as under 1.0.18 (R-B12). For a `Kill` entry, when its owner
  releases it (an `output` call at once; the helper's owner after its decision), the waiter: if not retired, sends
  `kill(-pgid, SIGKILL)` (a `Group`), ending any member the leader left, as the Windows job does on close. Then, for
  every entry, it asks the writer thread to write the unregister record, waits for its acknowledgement, and reaps with
  `waitpid`. `retire()` writes the unregister record first, and nothing in its group is killed (TR1.6 :82). On
  Windows a `Wait` entry's waiter clears the job's limit at the leader's exit before closing it.
- **The writer thread.** One per registry makes every parent-side lifeline write. Its own mask blocks SIGPIPE for its
  life, so a dead reaper is an `EPIPE`, not a signal; no other thread's mask changes.
- **Why no kill reaches a reused ID (Unix).** Every kill gwz sends is sent while the entry is live, so its ID is pinned
  by a zombie or a live process. The reaper kills only at end of file and only entries still registered; the pipe
  delivers records in order, and gwz reaps only after the unregister record is written, so no entry that gwz reaped
  is still registered. The windows that remain are R-B1: gwz dies after a registered child exited, and init reaps it
  before the reaper's kill; a child reaped before its unregistration was written, by std after an `exec` failure (§2)
  or by another reaper (`waitpid(-1)` in an embedding application, or SIGCHLD set to `SIG_IGN`, seen as `ECHILD`, which
  unregisters at once).
- **Windows.** Every kill goes through a job or process handle, which holds the kernel object; no ID is used, so there
  is no reuse window.

### 3.4 The reaper (macOS and Linux)

- **Creation.** `create` makes the lifeline (§3.5), opens `/dev/null`, reads `RLIMIT_NOFILE`, allocates the table, and
  forks. The parent closes the read end, then starts the writer thread and a watch thread that blocks in
  `waitid(P_PID, reaper, WEXITED)`. A failed fork leaves the registry `ReaperGone` from its creation.
- **Launch contract.** The child calls only async-signal-safe functions and never allocates or returns into Rust or
  Python: `setsid()` (no terminal or group signal reaches it); `SIG_IGN` for SIGINT, SIGQUIT, SIGHUP, SIGTERM, SIGTSTP,
  SIGTTIN, SIGTTOU and SIGPIPE; an empty mask; `chdir("/")`; `/dev/null` onto 0 to 2; every other descriptor closed
  except the read end (`close_range` where Linux has it, else each up to the limit), so it holds no lock, socket,
  pipe or terminal of gwz's.
- **Loop.** It reads 16-byte records, accumulating partial reads, into the table by token; it ignores the unregister
  record of a token it does not hold. At end of file it sends SIGKILL to every entry still registered (`kill(-pgid)`
  for `Group`, `kill(pid)` for `Process` and `Session`) and calls `_exit(0)`. A read error or a record with a bad
  magic, version or operation calls `_exit(1)` and kills nothing.
- **Its death.** The watch thread sees it at once, and the writer's next write sees `EPIPE`. Either sets the registry
  `ReaperGone`: every later entry spawn is refused before its program runs (fail closed), with a message naming the
  reaper. Children already running keep their in-process paths (§3.6) but are not covered against gwz's death (R-B2).
- **Its end.** `close()` (and the last drop) refuses new spawns, kills every live entry (`Kill` and `Wait`; never an
  `OwnInput` or `Detach` child), closes the write end; the reaper reads end of file and exits; the watch thread reaps
  it. An exit without destructors closes the write end in the kernel, with the same result.

### 3.5 The lifeline's descriptors

- Made with `pipe2(O_CLOEXEC)` on Linux and `pipe` on macOS; each end is then moved to a descriptor of 3 or more with
  `F_DUPFD_CLOEXEC` and the originals are closed, so a process started with 0, 1 or 2 closed cannot have gwz's output
  written into the lifeline or the reaper's `/dev/null` placed over its read end.
- The read end exists only in the reaper. The write end is in gwz, and in a registered child only between its fork
  and its `exec`, where close-on-exec closes it.
- On macOS the originals are inheritable between `pipe` and their close; a child that another thread starts in that
  window keeps the write end and delays the reaper's end of file until it exits (R-B3), the same window std's own pipes
  have (§2). gwz-cli's root creates its registry before any thread exists (OD-B3), so no gwz-cli spawn is in it.
- A fork without exec copies the write end, which delays end of file until every copy closes (R-B3, §3.8).

### 3.6 The kinds and how each is ended

| Child | Spec | While gwz lives | When gwz is gone |
|---|---|---|---|
| Credential helper (Unix arm moved onto the registry; Windows arm unchanged) | `Group`, `Kill` | its lookup's bound, cancel and drop kill the group (`owner.rs:253-303`); success retires it | reaper; its job |
| `git rev-list` path cursor | `Group`, `Kill` | waited for (one `--max-count=1` step, `commit_log/mod.rs:337`); drop kills it; it writes nothing (`GIT_OPTIONAL_LOCKS=0`, `:347`) | reaper; its job |
| `git commit`, `git tag` | `Group`, `Wait` | waited for with its write run; never killed by a cancel or by `kill_killable`; retired at its exit (OD-B1) | reaper; its job (R-B4) |
| gwz-sspi worker (Windows only) | unchanged | its supervisor | its job |

- **The helper's Unix arm** drops `tokio::process`: its pipe ends become `tokio::net::unix::pipe` ends
  (`tokio/src/net/unix/pipe.rs:419, :967`) and its exit an `exited()` future, so gwz-core's candidate table drops
  tokio's `process` feature (`Cargo.toml:139`), removing tokio's SIGCHLD handler (§2) from every gwz process. The three
  test files keep `tokio::process` through a `process` feature on tokio in the candidate dev-dependencies
  (`Cargo.toml:152`); dev features do not reach a normal build.
- **Ending never changes the outcome (f9).** `terminate` returns the lookup's own `Cancelled` or `Timeout`; a tree not
  confirmed ended within `CLEANUP_GRACE` is retained (`owner.rs:298`, `retain_pending`) and counted as pending
  cleanup, never returned as the lookup's error. A designs what the member reports from that outcome.
- **Not gwz-core's children.** The pager and `forall`'s commands: OD-C6 (r1 :257-258), never killed, waited for by A;
  after gwz's SIGKILL they run on (R-B8). `gwz hook`'s utilities and native route helpers have no registry hook and
  stay as they are (R-B5, R-B9). Processes a helper, `git` or a broker starts in another session or job (gpg-agent's
  pinentry, WMI or COM, a `setsid` descendant) are outside any group or job (TR1.6 :75; `process_tree.rs:21-24`; R-B7).

### 3.7 Interfaces to A and C

- **To A.** `kill_killable()` kills every live `Kill` entry now (the forced exit's "kills children … at once", D-V (1));
  `Wait` entries belong to their write run, and `OwnInput` children to C. `close()` refuses spawns; A calls it only
  after the last write sequence has ended, so a write run still going after the forced exit began can start its `Wait`
  child. B changes no signal disposition in gwz's process: dispositions change only in a forked child before `exec`
  and in the reaper, each one entry in `scripts/checks/process_globals_allowlist.json` for `crates/children`.
- **To C.** `Process { ignore_interrupts: true }` keeps a child in the terminal's foreground group with SIGINT and
  SIGQUIT ignored, killed by PID (parked :1212, :1218), with §3.2's Windows difference. `Session` puts a child in a new
  session (parked :809). `OwnInput` registers nothing and gets no job: the child ends at its input's end of file
  (parked :1224; Safety P2-4, :109). `Detach` (with `Session`, Unix only) starts the one process that outlives gwz by
  design, 2.3k's background `server start` copy (2.3 plan :212-213); the registry forgets it at once (R-B11). Which
  child takes which, the Windows background launch, and the proof that each ends, are C's.

### 3.8 Embedding (gwz-py)

gwz-py's composition object creates the process's registry (§4, 3.2), so the fork happens in a process that may
already have threads; §3.4's child is safe there because it calls only async-signal-safe functions. Three constraints
are documented in gwz-py's README (§8 row 6): an application that forks without exec (for example `multiprocessing`'s
`fork` start method) gets a registry that refuses in the child, and its copy of the write end keeps the parent's
reaper waiting until the copy closes (R-B3); an application that reaps with `waitpid(-1)` or ignores SIGCHLD opens
R-B1's window; on macOS a spawn by another thread while the registry is created can delay the reaper (R-B3).

## 4. Steps

| Step | Content | Owns | Size | After | Review |
|---|---|---|---|---|---|
| **2.3r-1 (new)** | §3.1-§3.5 in `crates/children`: the registry, the Unix spawn and registration, the Windows suspended job spawn, waiters, the writer and watch threads, the reaper, the fork check; rows B1-B12, B20-B24 | create `crates/children/` (with its `BUILD.bazel`); gwz-core `Cargo.lock` (the crate's package entry) | ~500 + 500 | nothing (the crate builds and tests alone) | single (State) |
| **2.3r-2 (new)** | §3.1's gate slot, `process_safety_with_children`, `spawn_gate()`'s registry and `with_process_gate`; §3.6: the helper's Unix arm, `rev-list` and the CLI route onto the registry, tokio's `process` feature moved to the dev table, `terminate`'s outcome; the two checks amended; rows B13-B17 | `src/git/spawn_gate.rs`, `src/git/gitbackend/backend.rs` (`:118-125`), `src/git/endpoint/https_auth/process_tree.rs` (Unix arm), `owner.rs` (`terminate`), `src/operation/commit_log/mod.rs` (`:330-358`), `src/transport_host/composition.rs`; gwz-core `Cargo.toml` (path dependency, crate feature, two tokio feature lines), `Cargo.lock`, `BUILD.bazel` (one label in a list at `:12-40`); `scripts/checks/check_spawn_gate_minting.py`, `check_process_globals.py` and its allowlist | ~350 + 360 | 2.3r-1 | dual (it moves the helper's spawn, whose environment carries secrets) |
| 2.3j (amended) | spawns nothing new; ordered after the registry by D-V (3) | as planned | as planned | 2.3g, 2.3r-2 | as planned |
| 3.1 (amended) | `run` calls `process_safety_with_children()` after `worker_host::early` and before any thread (OD-B3); the host and every hostless backend take it through `with_process_gate` (`dispatch.rs:93`, `log_exec.rs:118`, `hook/family.rs:63, :149, :187`); row B18 | those five lines added to 3.1's files | +40 | as planned | phase |
| 3.2 (amended) | the composition object creates the registry once; the host and gwz-py's 8 hostless backend lines take it; README text; row B19 | those lines added to 3.2's files | +40 | as planned | phase |
| 3.4 (amended) | measures the reaper's fork time and resident cost after the parent writes 100 MiB (M, L) | evidence | | as planned | |

- **Waves (2.3 plan §4).** Lane 3 runs 2.3e then 2.3r-1 in wave 2 and 2.3r-2 then 2.3j in wave 3. 2.3r-1's only
  shared file is `Cargo.lock`, merged serially by the lane owner like any lock change. No other wave changes; at most
  three compiling lanes.
- **Ownership.** 2.3r-2's files are outside every 2.3 sub-step's (2.3 plan :240-242) and outside the Phase 1 trunk's
  list (plan :324-326). A file another lane holds is asked for, not edited (plan :330-331).
- **Interim.** Between 2.3r-2 and 3.3 each old entry call makes its own registry and reaper (3.3 removes the entries).
  Until 3.1 and 3.2, gwz-cli's and gwz-py's hostless backends spawn unregistered, as today. After them only gwz-core's
  own hostless backends do (R-B13), and that state ships (OD-B2).

## 5. Rows

Each row is seen red first unless a leg is marked **(reg)**. Platforms: M macOS, L Linux, W Windows. "Harness" is a
test binary of `crates/children` (rows B1-B12, B20-B24) or of gwz-core (B13-B17); a fixture `sh` child starts a
sleeping grandchild and records both PIDs.

1. **B1** [2.3r-1] SIGKILL of the harness while a registered `Group` child and its grandchild run: both gone within
   1 s. W: `TerminateProcess` of the harness: the job ends both within 1 s. (M, L, W)
2. **B2** [2.3r-1] SIGKILL, SIGQUIT and SIGINT-then-SIGKILL sent to the harness's whole process group, and a hung-up
   pseudo-terminal: the reaper outlives each, and the registered group is gone within 1 s. (M, L)
3. **B3** [2.3r-1] A retired child's grandchild is alive after the harness's normal exit and after its SIGKILL; a
   non-retired `Kill` child whose leader exited leaves no member; the reaper's test trace shows no kill of an
   unregistered token and each unregister record before its reap. W: a retired job's grandchild lives **(reg)**. (M, L, W)
4. **B4** [2.3r-1] SIGKILL of the harness immediately after `spawn` returns: the child's group is gone within 1 s. (M, L)
5. **B5** [2.3r-1] The reaper SIGKILLed alone: `ReaperGone` within 1 s; the next spawn is refused and its program,
   which would create a file, never runs; a running child is still ended by its handle. An injected fork failure:
   every entry spawn refused. (M, L)
6. **B6** [2.3r-1] Descriptors (`lsof`): the harness holds no read end; the reaper holds only the read end and
   `/dev/null` on 0-2; under 50 concurrent spawns no child holds either end (L; M with the registry made before any
   thread); a `flock` the harness held is free after it exits while the reaper lives. (M, L)
7. **B7** [2.3r-1] The harness started with descriptors 0-2 closed: both lifeline ends are 3 or more, text it prints
   never reaches the reaper, and SIGKILL still ends its child. Bytes with a bad magic injected into the lifeline (test
   seam): the reaper exits, kills nothing, and the registry is `ReaperGone`. (M, L)
8. **B8** [2.3r-1] A spawn from a thread with SIGINT and SIGPIPE blocked: the child's mask is empty and SIGPIPE is
   `SIG_DFL` (a probe program); a spawn after the reaper died fails, and no child died of SIGPIPE. (M, L)
9. **B9** [2.3r-1] In a child forked without exec, every registry call is refused `ForkedProcess`; the parent's
   children are still killed at the parent's SIGKILL once the forked child has exited. (M, L)
10. **B10** [2.3r-1] A full registry refuses `Capacity`; 300 spawns of a missing program are each refused, the live
    count is zero and every registered token has its unregister record. (M, L, W)
11. **B11** [2.3r-1] A harness thread reaping with `waitpid(-1)`, and the harness with SIGCHLD set to `SIG_IGN`: the
    child's entry is unregistered at once and the reaper's trace shows no kill for it. (M, L)
12. **B12** [2.3r-1] A child that spawns a grandchild as its first act: both are in the job (`IsProcessInJob`); a
    `GenerateConsoleCtrlEvent(CTRL_BREAK_EVENT, 0)` reaches neither. An injected resume failure: the child is
    terminated, the spawn refused, and no process is left. (W)
13. **B13** [2.3r-2] A lookup through `HelperSlots` with a sleeping recording helper: SIGKILL of the harness ends the
    helper's group within 1 s. W: the job **(reg)**. (M, L, W)
14. **B14** [2.3r-2] A path-filtered history walk on a large repository: `rev-list` leads its own group, and SIGKILL of
    the harness ends it within 1 s. W: it has a job and no console. (M, L, W)
15. **B15** [2.3r-2] A CLI-route commit whose `pre-commit` hook sleeps and whose `post-commit` hook starts a background
    `sleep`: SIGINT to the harness's process group reaches neither `git` nor the hook and the commit completes;
    `kill_killable()` leaves it running; the background `sleep` outlives `git`'s exit; SIGKILL of the harness during
    `pre-commit` ends `git` and the hook within 1 s. W: CTRL_BREAK reaches neither; `TerminateProcess` ends both. (M, L, W)
16. **B16** [2.3r-2] With the drain held past `CLEANUP_GRACE` (a `cfg(test)` seam in `owner.rs`): a cancelled lookup
    returns `Cancelled`, a timed-out one `Timeout` with `SetupFailureCause::Interaction`, the cleanup report counts one
    pending child, and the owner reaps it within 30 s. (M, L, W)
17. **B17** [2.3r-2] The minting check fails on `ChildRegistry::create` in `src/` outside `composition.rs`; the
    process-globals check fails on `tokio::process`, `libc::fork` and `pre_exec` in gwz-core production outside the
    crate; `cargo tree -e features,no-dev` shows no tokio `process` for gwz-core, gwz-cli and gwz-py. (M, L, W)
18. **B18** [3.1] `gwz fetch` with a sleeping helper, `gwz commit` with a sleeping hook and `gwz log -- <path>`: the
    crate's trace shows one registry, `run`'s, and every spawn registered with it; SIGKILL of gwz ends each child
    within 1 s **(reg)**. (M, L; W jobs **(reg)**)
19. **B19** [3.2] The gwz-py module's fetch, commit and log: one registry, every spawn registered with it; SIGKILL of
    the Python process mid-lookup ends the helper within 1 s **(reg)**. (M, L; W **(reg)**)
20. **B20** [2.3r-1] A `Process { ignore_interrupts: true }` child: SIGINT and SIGQUIT to the harness's group leave it
    running, its group is the harness's, and at the harness's SIGKILL the reaper kills it by PID within 1 s while a
    `tee` the harness started in the same group (unregistered) survives (Safety P2-3, :92). W: a console CTRL_C leaves
    it running, a CTRL_BREAK to its group ends it (§3.2's difference), and the harness's `TerminateProcess` ends it
    through its job. (M, L, W)
21. **B21** [2.3r-1] An `OwnInput` child that exits at its input's end of file: no register record in the reaper's
    trace and no job (`IsProcessInJob` false); `close()` leaves it running; at the harness's SIGKILL or
    `TerminateProcess` it exits 0 at end of file, not by a kill. (M, L, W)
22. **B22** [2.3r-1] `kill_killable()`: a live `Kill` child and its grandchild are gone at once, while a `Wait` child
    and an `OwnInput` child run on; after `close()` a spawn is refused `Closed` and its program never runs. (M, L, W)
23. **B23** [2.3r-1] A `Wait` child that starts a background grandchild and exits 0: the grandchild is alive after the
    child's reap, after `close()` and after the harness's normal exit. (M, L, W)
24. **B24** [2.3r-1] A `Session` child's session ID is its own PID; a `Session` + `Detach` child writes no record and
    is alive after the harness's SIGKILL. W: `Detach` is refused `Unsupported`. (M, L, W)

## 6. Decisions

- **OD-B1 The CLI route's `git commit` and `git tag` (for the operator).** (a) **Recommended:** its own group (no console
  on Windows), waited for by every cancel and by the forced exit, retired at its exit, killed only when gwz is gone.
  Resolution (1) needs this: the terminal's interrupt must not end a write mid-way. Cost: a hook or signer that reads
  the terminal itself (a passphrase prompt from `ssh-keygen -Y sign` with no agent) is stopped by SIGTTIN and
  `gwz commit` waits with no bound (R-B10); 1.0.18 ran it in gwz's group (`spawn_gate.rs:431-436` sets none). (b) Keep
  it in gwz's group as a `Process`: prompts work, but the terminal's interrupt reaches `git` and its hooks mid-write,
  and gwz's death kills `git` alone, not its hooks.
- **OD-B2 Gates without a registry (for the operator).** (a) **Recommended:** they spawn as today, unregistered
  (R-B13); after 3.1 and 3.2 that is only gwz-core's own hostless backends that commit by the CLI route or walk
  path-filtered history (for example `src/workspace_ops/handle_create_repo/create_workspace.rs:18`). (b) Such a gate
  creates its own registry and reaper at its first spawn: covered, but one more reaper each and a fork in a threaded
  process. (c) Thread the root's gate into every `Git2Backend::new` and `without_credential_helpers` site: about 560
  lines in the three members' sources (454 and 108), 47 of them in files whose names are not tests; outside this design.
- **OD-B3 Every gwz command forks the reaper (for the operator).** r1's OD-C2 (a) (:245-246), decided as recommended,
  prices the reaper as "one extra process per gwz process that starts children". (a) **Recommended:** `run` creates
  the registry for every command, before any thread, including commands that start no child (`status`, `diff`), and
  gwz-py once per Python process; this is a departure from that pricing, measured in 3.4. It closes the macOS window
  (§3.5) for every gwz-cli spawn. (b) Create it at the first spawn: priced as OD-C2 says, but the fork happens in a
  threaded process and every gwz-cli spawn is in R-B3's macOS window.
- **Choices open to review (not operator decisions):** capacity 256; the group kill of a non-retired `Kill` group at
  its leader's exit; one writer thread per registry; Windows suspended creation with a Toolhelp resume for std-built
  commands, rather than generalizing the helper's job-list launch (`windows/launch.rs:1-3`); no re-fork of a dead
  reaper; no Linux pidfd (D-V (2) says `kill()`).

## 7. Residuals for design D

| # | What can remain | When | How it is recognized |
|---|---|---|---|
| R-B1 | A SIGKILL sent to a process or group that reused a registered ID | gwz dies in the instant after a registered child exited and init reaped it; or a child was reaped before its unregistration (std after an `exec` failure; `waitpid(-1)` or ignored SIGCHLD in an embedding application) and gwz dies before the record is written. Needs the ID reused inside that window | Not recognizable afterwards; listed so the operator accepts it |
| R-B2 | Children that outlive a later SIGKILL of gwz | The reaper was killed alone while they ran | Every later spawn fails naming the reaper; the children run on with parent PID 1 |
| R-B3 | Registered children outlive gwz until another process exits | A fork without exec in an embedding application, or (macOS) a child started by another thread while the lifeline was made | The reaper (a copy of gwz or Python in its own session) is still running after gwz died, with the registered children |
| R-B4 | What `git commit`, `git tag` or a hook was writing: Git `*.lock` files, a partly run hook's effects | gwz is SIGKILLed or crashes while a CLI-route child runs; the reaper or job kills it | D's Git `*.lock` report (OD-C3); a hook's own effects are the user's |
| R-B5 | A native route helper still waiting | gwz is SIGKILLed during `Cred::credential_helper` (not registered) | A `git credential-*` or helper process with parent PID 1, as on 1.0.18 |
| R-B6 | What a successful helper deliberately left running (`credential-cache`) | Every successful lookup (TR1.6 :82) | Not gwz's; by the accepted design |
| R-B7 | Processes another program or broker started (pinentry, WMI or COM, a `setsid` descendant, `ssh`'s own children) | Any | Not in any group or job of gwz's; as on 1.0.18 |
| R-B8 | The pager or a `forall` command | gwz is SIGKILLed while it runs (OD-C6) | The user's own program is still on the terminal |
| R-B9 | `gwz hook`'s utilities, and what `git worktree add` leaves if the terminal's interrupt ends it | gwz is killed during `gwz hook` | They end on their own (one short command each); a partial worktree shows in `git worktree list` |
| R-B10 | A `gwz commit` waiting on a stopped terminal reader | OD-B1 (a): a hook or signer reads the terminal | The command waits; `ps` shows the reader stopped (`T`) in `git`'s group |
| R-B11 | The served form's background server | Every `gwz server start` without `--foreground` (2.3k), by design | `gwz server status` and `list` (C) |
| R-B12 | A background job a `git commit` or `git tag` hook left (a post-commit `ctags … &`) | Every such commit; it runs on after `git` exits, also through gwz's later death | As on 1.0.18, the user's own job; it runs in `git`'s former group, not gwz's |
| R-B13 | A child of a gate with no registry (`git commit`, `git tag`, `rev-list`) after gwz's SIGKILL | A gwz-core hostless backend (OD-B2 (a)), or a non-candidate build | As on 1.0.18: a `git` process with parent PID 1, and R-B4's `*.lock` files |

## 8. Superseded or amended clauses (applied by the lane owner at acceptance)

| # | Document and clause | Replacement or addition |
|---|---|---|
| 1 | 2.3 plan §3, after 2.3l (:220-230) | Add: "**2.3r-1 Child registry and reaper (single, State).** Goal: plan §10 D-V resolution (2). Owns: create `crates/children/`; gwz-core `Cargo.lock`. Content and red rows: GwzChildLifetimeDesign §3.1-§3.5, rows B1-B12, B20-B24. Size ~500 + 500. After: nothing. Evidence: Mac, WSL, Windows receipt." and "**2.3r-2 Spawns onto the registry (dual: secret handling).** Goal: D-V resolution (2) for gwz-core's three spawn sites and its public backend setter. Owns: GwzChildLifetimeDesign §4. Rows B13-B17. Size ~350 + 360. After: 2.3r-1. Evidence: Mac, WSL, Windows receipt." |
| 2 | 2.3 plan 2.3j, "After: 2.3g (stdio serves through it)." (:202) | "After: 2.3g (stdio serves through it) and 2.3r-2 (plan §10 D-V resolution (3))." |
| 3 | 2.3 plan §4 table, waves 2 and 3, lane 3 (:240-241) | Wave 2 lane 3: "2.3e address, then 2.3r-1 registry"; wave 3 lane 3: "2.3r-2 spawns, then 2.3j credentials and stdio"; add to the reasons: "r-1 owns `crates/children/` and shares only `Cargo.lock`; r-2 owns the files GwzChildLifetimeDesign §4 lists". |
| 4 | 2.3 plan §4, "Review: dual on a, d, j, l" and totals (:249-250) | "Review: dual on a, d, j, l and r-2" and "about 5,150 implementation lines and 5,610 rows across 14 steps". |
| 5 | 2.3 plan 2.3k, "background copy via the launcher with closed descriptors and a new session" (:212-213) | Append: "(on macOS and Linux through the child registry's `Session` target with ending `Detach`, GwzChildLifetimeDesign §3.7)". |
| 6 | Plan §4, 3.1 (:721-728), 3.2 (:729-735), 3.4 (:739) | 3.1 append: "`run` creates the child registry after `worker_host::early` and before any thread; the host and every hostless backend the command builds take it through `Git2Backend::with_process_gate` (GwzChildLifetimeDesign §4, row B18). +40." 3.2 append: "The composition object creates the child registry once; the host and every hostless backend take it (§3.8, row B19). +40." 3.4 append: "and the reaper's fork time and resident cost (M, L)." |
| 7 | `gwz-py/README.md`, "Platform And Status" (:163) | Add: "gwz starts one helper process per Python process that ends gwz's child processes if Python is killed. After `os.fork()` without exec (for example `multiprocessing`'s `fork` start method) gwz refuses to start processes in the child, and the parent's helper waits until the child exits; use `spawn` or `forkserver`. Reaping all children (`waitpid(-1)`) or ignoring SIGCHLD weakens that protection." |
| 8 | Contract §6 table, row "Composition/CleanupDomain" (`GwzConnectionApiContract.md:289`) | Append to "State owned": "; the child registry, the only starter and kill authority of gwz-core's child processes, with its reaper (GwzChildLifetimeDesign)". |

TR1.6 §3.2 "Its cost" (:76) and OQ2 (:398) are history and are not edited; plan §10 D-V (:1005-1010) already withdraws
them. The credential review's "a cleanup miss continues to return CleanupPending"
(`gwz-core/dev-docs/history/GwzTransportCredentialHelpers-ReviewCode-2.md:66`) is history too; §3.6 supersedes it for
cancel and timeout from 2.3r-2.
