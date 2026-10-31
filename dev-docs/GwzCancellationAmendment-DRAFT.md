# Cancellation and process death: design amendment (DRAFT)

Date: 2026-10-31; revision 0. Status: **DRAFT for the operator's review. It authorizes no implementation,
commit to a member, tag, push or publish.** It states how decision **D-V (OQ2 reversed)** of
`GwzTransportReleaseConvergencePlan.md` §10 is met. Code citations are at root `a519ee58` (gwz-core
`07e99924`); paths are relative to the workspace root.

**The operator's requirement (2026-10-31).** Ctrl-C cancels a gwz operation end to end: the client, every
helper process it started, and a serving core's in-flight call. Each side detects that a process it
depends on has gone away, through a pipe closing or another mechanism, and cleans up. Ctrl-C must not
leave anything behind: no orphaned process, no stale lock, no half-written state.

**What it amends.** TR1.6 §3.2 and OQ2 (`gwz-core/dev-docs/history/reference/GwzTransportCredentialHelpersDesign.md:75-76, :398`):
the accepted cost "a helper still waiting outlives it" is withdrawn. The split doc's interrupt rules
(`GwzSplitCommandChannel.md:232, :235, :239-240`) and the parked server design's launch isolation
(`history/parked/GwzCoreServerDesign.md:809, :1107, :1218-1224`) are adopted and given owners. The
session design's accepted exposure "a local write it has in progress can be torn by exit, the same
exposure the CLI has under Ctrl-C" (`GwzCoreSessionDesign.md:687`) is narrowed by §3.5.

## 1. Requirements

- **C1 Interrupt is cancellation.** On Unix SIGINT, SIGTERM and SIGHUP, and on Windows CTRL_C, CTRL_BREAK
  and CTRL_CLOSE, a gwz-cli command cancels every operation it runs, through the same cancellation the
  library already has (§2). The command exits only after that cancellation has finished its cleanup, or
  after the bound in C5.
- **C2 No orphans.** Every process gwz starts ends no later than the operation that started it, and no
  later than gwz itself, however gwz ends: an orderly exit, a cancel, SIGKILL, a crash, or the console
  closing.
- **C3 Death is detected in both directions.** Each side of every dependency notices its peer's death
  without polling the peer's state, within a stated bound: a helper notices gwz; gwz notices a helper;
  a client notices its core; a core notices its client; the `ssh` child of the `ssh://` form notices
  both.
- **C4 Nothing half-written.** After any interruption, every file gwz writes is either complete, never
  published, or recognized and resolved by the next gwz command in that workspace. A file only Git's
  own rules govern (`*.lock` in a repository) is reported with its path and its safe removal; gwz never
  deletes one silently (§3.5, OD-C3).
- **C5 Bounded, then forced.** The first interrupt cancels and waits up to the cleanup bound (5 s, the
  host's close bound, contract §5 `GwzConnectionApiContract.md:253-284`). A second interrupt, or the
  bound passing, forces the exit; the forced path still ends every child tree (C2) and still leaves only
  state C4 allows.
- **C6 Parity and the code rules.** The same behaviour on macOS, Linux and Windows. No new global or
  thread-local state except the one the OS forces, which is confined and decided by the operator
  (OD-C1). Conditional compilation only inside `cfg_if!` or platform modules.

## 2. What exists (facts, read 2026-10-31)

- **No interrupt handling anywhere.** gwz-cli, gwz-core and gwz-py install no SIGINT, SIGTERM or console
  handler; `check_process_globals.py:103-104` forbids `signal`/`sigaction` as process-global state.
  Ctrl-C ends gwz-cli by the OS default: no destructor runs, so no cancel, no helper kill and no
  `finish()` (`gwz-cli/src/lib.rs:160-330`).
- **Cancellation exists but the CLI does not use it.** `CancellationToken` and `CallControls::cancel`
  (`gwz-core/crates/session-host/src/token.rs:72-132`, `gate.rs:140-158`); the cancellable entries
  `with_cancellable_local_transport*` (`gwz-core/src/transport_host/cancellable.rs:40-68`) reach
  `Session::cancel`, which cancels bindings, engine requests, streams and checks
  (`transport_host/session/requests.rs:37-75`). gwz-py uses them (`gwz-py/native/src/route/transport.rs:20-21, 194`);
  gwz-cli calls the non-cancellable entries (`gwz-cli/src/globalargs/dispatch.rs:67-80`).
- **Local phases barely observe a cancel.** The served dispatch checks the token once before a handler
  starts (`session_host/served/dispatch.rs:100-107`); the member-lock wait wakes on it
  (`operation/memberlockmanager.rs:99-120`); `gwz local clone`'s copy takes a cancellation but production
  passes `NeverCancelled` (`workspace_ops/handle_local.rs:72`). The libgit2 transfer-progress callback
  always returns `true` (`git/gitbackend/transport_support.rs:90-93`), so a native transfer (ordinary
  builds; `git://`, `http://`, `file://` remotes) cannot be cancelled.
- **Children.** The credential helper (`git credential fill` and what it starts) is the only
  credential-carrying child. Unix: its own process group, `kill(-pgid, SIGKILL)` on timeout, cancel and
  drop (`git/endpoint/https_auth/process_tree.rs:57-95`, `owner.rs:253-303`). Windows: a job with
  `KILL_ON_JOB_CLOSE`, so gwz's exit, even by `ExitProcess`, ends the job
  (`process_tree/windows.rs:65-133`); a test covers drop, not a killed gwz. Nothing uses
  `PR_SET_PDEATHSIG` or a watchdog; gwz closes the helper's stdin after the request
  (`https_auth/pipes.rs:9-16`), so a helper sees no EOF when gwz dies. Other children: `git rev-list`
  for path-filtered `log` (no group, no kill on cancel; `operation/commit_log/mod.rs:330-358`), the
  pager and `forall` commands (gwz's own group; `gwz-cli/src/pager.rs:147-165`, `forall.rs:231-262`),
  and on Windows the gwz-sspi worker (a `KILL_ON_JOB_CLOSE` job). SSH is in-process (libssh2); gwz spawns
  no `ssh` today.
- **The carrier detects a dead peer.** Reader EOF and the peer-side error kinds become `PeerGone`; a peer
  that stops reading becomes `Stalled` after 60 s (`transport_host/carrier/mod.rs:312-327`,
  `stream/inbound.rs:358-367`, `stream/writer.rs:344-378`); a broken Windows pipe reads as end of
  stream (`carrier/local/windows.rs:560-572`). On carrier end the operation service cancels every
  running call and settles the unstarted ones `Cancelled` (`crates/session-host/src/service/workers.rs:213-235`),
  detaching after `close_wait` 60 s. Only the in-process adapter calls it today; the byte-stream
  session is 2.3g.
- **Leftover state.** gwz's own cross-process locks are OS advisory locks, released by death
  (`operation/workspace_mutator_lock.rs:42-50`, `crates/family-store/src/lock.rs`). Durable writes stage
  `<name>.<id>.tmp`, fsync and rename; nothing sweeps a temp a crash left (`artifact/encoding.rs:47-56`).
  libgit2 refuses an existing `<path>.lock` with `GIT_ELOCKED` and never judges staleness
  (`libgit2/src/util/filebuf.c:44-52`); gwz has no stale-lock handling. Merge state and local-clone rows
  already have explicit recovery (`gwz merge --status/--continue/--abort`; `gwz local list`/`dispose`).
  A killed server leaves its Unix socket (2.3f owns stale-socket recovery).

## 3. Design

### 3.1 One interrupt source per process, owned by the composition root

gwz-cli's composition root (the per-command `HostOwner` of plan step 3.1) creates one **interrupt
source** before it runs the command and passes it explicitly; nothing reads it from a global.

- **Unix.** A self-pipe: the handler for SIGINT, SIGTERM and SIGHUP writes one byte and does nothing
  else (async-signal-safe). A thread owned by the interrupt source reads the pipe and turns bytes into
  events: the first is `Cancel`, a later one `Force`.
- **Windows.** `SetConsoleCtrlHandler` for CTRL_C, CTRL_BREAK and CTRL_CLOSE signals the same event
  sequence. CTRL_CLOSE gives the process about 5 s before the OS ends it, so it goes straight to the
  forced path (§3.3).
- **The unavoidable global.** The handler's write end (a descriptor, or the Windows handler's event) is
  process-global by the OS's design. It lives in one module, `interrupt/` in gwz-cli, installed once by
  `main` and named in `check_process_globals.py`'s allow-list with this section as its reason. Every
  other component receives the source as a value. **OD-C1.**
- **gwz-py.** No handler of its own: Python delivers SIGINT as `KeyboardInterrupt` to the awaiting call,
  which already cancels through the operation's `Canceller` (`route/transport.rs:44, 167-173`). The
  change is to make `_await_completion` (`gwz-py/src/gwz/bridge.py:29-35`) honour the second interrupt as
  `Force` (§3.3) instead of waiting through it, and to name in its docs that an embedding application
  owning the signal disposition keeps that control.

### 3.2 Cancel reaches the work

- **The command's operations.** gwz-cli moves to the cancellable entry points now, or to the
  `HostOwner` of step 3.1 when that lands, holding one `CallControls` per command. `Cancel` calls it.
  Everything below the token is existing plumbing: bindings, exchanges, the HTTPS and SSH engines, the
  helper's kill on cancel.
- **Native transfers.** The transfer-progress and sideband callbacks return `false` once the token is
  cancelled, which libgit2 treats as a user abort and cleans up through its own error path (it removes
  its temporary pack and releases its lock files). This closes the ordinary-build gap and the native
  `git://`/`http://`/`file://` gap.
- **Local phases.** A cancel is observed at **safe points only**: between members, between files of a
  copy, and before a publish step; never in the middle of a write sequence that would leave a torn file.
  The safe points are named per handler in the implementing step's brief (§4); `gwz local clone`'s copy
  receives the real token instead of `NeverCancelled`. A phase between two safe points runs to its next
  one; the cleanup bound (C5) covers the wait.

### 3.3 Exit: graceful first, forced second

1. **`Cancel` (first interrupt).** Cancel the command's operations; wait up to the cleanup bound for
   their terminals and for every child tree to end; print what was cancelled; exit with the interrupted
   status (OD-C4).
2. **`Force` (second interrupt, the bound passing, or CTRL_CLOSE).** Synchronously end every live child
   tree (§3.4's registry), release nothing else by hand (gwz's own locks are advisory and die with the
   process), print one line naming any operation that did not reach a terminal, and exit with the same
   status. No destructor-dependent cleanup is relied on.

### 3.4 Children: registered, and killed even when gwz dies

- **Registry.** Every child gwz starts for an operation (helpers, `git rev-list`, and later the
  `stdio`/`ssh` child of §3.6) is registered with the owning host's child registry, owned by the
  composition and passed down, never global. The registry ends every tree on cancel, on the forced path,
  and on host close. The pager and `forall` commands are the user's foreground programs: they receive
  the terminal's interrupt themselves; gwz waits for the current one to exit and then stops iterating,
  instead of killing it (OD-C6).
- **When gwz dies (SIGKILL, a crash).** No code in gwz runs, so the kill must already be armed:
  - **Windows:** the job with `KILL_ON_JOB_CLOSE` already does this; add the missing row (gwz killed by
    `TerminateProcess`; the helper's job ends).
  - **macOS and Linux: a lifeline reaper.** The first child of a process starts one small reaper
    process (gwz re-executed in a hidden `__reaper` mode, so nothing new ships). It holds the read end of
    a **lifeline pipe** whose only write end gwz keeps open (close-on-exec everywhere else). gwz tells
    the reaper each child's process group as it starts and ends. When the lifeline reads EOF, gwz is
    gone however it died, and the reaper sends `SIGKILL` to every registered group and exits. It is the
    one mechanism that covers macOS (which has no `PR_SET_PDEATHSIG`) and grandchildren (which
    `PR_SET_PDEATHSIG` on the direct child does not reach). Alternative: `PR_SET_PDEATHSIG` on Linux only
    and a recorded macOS gap (OD-C2).
  - **The helper side of the pipe.** gwz keeps the helper's stdin open until the lookup ends instead of
    shutting it after the request, so a helper that reads stdin also sees EOF when gwz dies; this is a
    second, cheaper signal, not a replacement for the reaper (a helper blocked on a window never reads).
- **gwz notices a dead child** as today: the child's exit and its pipes' EOF.

### 3.5 Leftover state

- **gwz's temps.** A durable write already writes `<name>.<id>.tmp` and renames. The next durable write
  into the same directory, while it holds the workspace mutator lock (so no other gwz writer is live),
  removes `<name>.*.tmp` siblings left by an earlier process. Row: a temp left by a killed writer is gone
  after the next command; a temp of a live writer in another workspace is untouched.
- **Git's `*.lock` files.** Cancelling at safe points and the libgit2 abort path (§3.2) leave none on the
  graceful path. On the forced path or a crash they can remain. gwz does not judge their staleness
  silently, since a plain `git` process may own one. When an operation meets `GIT_ELOCKED`, the member's
  error names the lock file, says no gwz mutation holds the workspace (when gwz can take the mutator
  lock), and gives the removal command. An explicit opt-in to remove such locks is OD-C3.
- **Partial Git transfers.** A cancelled fetch publishes no ref (libgit2 updates refs after the pack is
  indexed); a cancelled push reports `Effect::Possible` once its receive-pack exchange has started, as
  the contract already says. Rows assert both.
- **Already recoverable.** Merge state, local-clone `creating`/`disposing` rows, and the server's listener
  files (2.3f) keep their existing recovery; this amendment adds a row each that an interrupt at their
  worst point leaves a state their recovery command accepts.

### 3.6 The served form

- **Client interrupt.** First interrupt: `operation.cancel` for the command's calls, then wait up to the
  cleanup bound for their replies, which carry the core's cleanup report (`GwzSplitCommandChannel.md:235`).
  Second interrupt: close the carrier.
- **The core notices the client.** A closed or broken carrier is `PeerGone`; the operation service
  cancels every call and settles the rest `Cancelled` (exists for the in-process adapter; 2.3g gives it
  to the byte-stream session with the row "carrier loss cancels every live operation including local
  phases"). The core's member locks are in memory and released when each handler reaches its next safe
  point. A handler stuck between safe points is detached after `close_wait`, as the session design
  says; the core's own `Force` is its shutdown's second signal (2.3i).
- **The client notices the core.** `PeerGone` and `Stalled` fail every open exchange with `carrier_lost`
  and revoke bindings (exists, gwz-transport `remote/core.rs`). A core that is alive but hung with
  nothing queued is not detected without an application keepalive; the client's own interrupt still
  ends it. Whether this release adds a session keepalive is OD-C5.
- **The `stdio` and `ssh://` children.** The client starts them through the registry (§3.4) with the
  parked design's isolation: on POSIX a new session for `stdio` and SIGINT/SIGQUIT ignored for `ssh`; on
  Windows `CREATE_NEW_PROCESS_GROUP`. The terminal's Ctrl-C therefore reaches only gwz, which turns it
  into `operation.cancel`; a second interrupt closes the child's stdin, which ends the remote session
  through `PeerGone`. If the client dies, the lifeline reaper (or the job) ends the child and the
  kernel closes its pipes, with the same result at the core. `ssh`'s `ServerAliveInterval=15`,
  `ServerAliveCountMax=3` already bound a dead network path at 45 s. No 2.3 sub-step owns this spawn
  today (fact list gap 11); §4 assigns it.

## 4. Where it lands

| Piece | Owner (proposed) | Size |
|---|---|---|
| Interrupt source, gwz-cli `interrupt/` module, allow-list entry, exit status; cancellable entry in dispatch | new step **3.1c**, after 3.1 (gwz-cli owns the host) | ~350 |
| Native-transfer abort through the progress callbacks | gwz-core gitbackend, step **3.1c** or its own small step | ~120 |
| Child registry and the lifeline reaper; helper stdin kept open; Windows killed-gwz row | new step **3.0c** (gwz-core `https_auth/process_tree`, `operation/commit_log`), parallel with 3.0/3.0b | ~450 |
| Safe-point cancellation in local handlers; `local clone` real token | **3.1c-2**, after 3.1c | ~400 |
| Temp sweep under the mutator lock; `GIT_ELOCKED` reporting | **3.1c-3** | ~250 |
| Byte-stream channel-closure cancel-all (already 2.3g's row) | 2.3g (unchanged) | — |
| Client-side spawn of `stdio`/`ssh` with isolation, through the registry | **2.3j** gains it (it owns `server/launch.rs`) | ~150 |
| Client interrupt protocol over the carrier | **2.3k** (gwz-cli's `--server` client) | ~150 |
| gwz-py second interrupt as `Force` | **3.2** (gwz-py owns the host) | ~80 |

Each step is TDD; the review tier is single (State) except the reaper and the stdio/ssh spawn, which
handle process lifetime across a credential boundary: dual.

## 5. Rows (each seen red first)

1. SIGINT to a `gwz fetch` blocked in a recording helper that sleeps: the helper's whole group is gone
   within the cleanup bound, the command exits with the interrupted status, and the member row says
   cancelled. Same with CTRL_C on Windows.
2. SIGKILL to gwz during the same lookup: the helper's group is gone within 1 s (macOS, Linux: the
   reaper; Windows: the job).
3. Second SIGINT during a cleanup that does not finish: exit within 1 s, no child left.
4. A cancelled native (`file://`) fetch: no ref moved, no `*.lock` and no temporary pack left.
5. A cancel during `gwz local clone`'s copy stops at the next file and leaves a `creating` row that
   `dispose` accepts.
6. A temp left by a killed durable writer is removed by the next command; a live writer's temp is not.
7. A member with a leftover `refs/heads/x.lock`: the error names the path and the removal command.
8. Served: client SIGINT sends `operation.cancel` and receives the cleanup report; client SIGKILL makes
   the core cancel the call (`PeerGone`) and release its member lock; the `ssh` child does not receive
   the terminal's SIGINT.

## 6. Decisions for the operator

- **OD-C1 The interrupt handler's process-global state.** The OS makes a signal or console handler
  process-global. Recommended: one gwz-cli module owns it, installed once by `main`, allow-listed in
  `check_process_globals.py` with this document as the reason; everything else receives the source
  explicitly. gwz-core and gwz-py install none.
- **OD-C2 Orphans when gwz is killed, on macOS and Linux.** (a) **Recommended:** the lifeline reaper,
  one extra process per gwz process that starts children, covering grandchildren and macOS. (b)
  `PR_SET_PDEATHSIG` on Linux only, reaching only the direct child; macOS keeps the orphan.
- **OD-C3 Git `*.lock` files after a forced exit.** (a) **Recommended:** report with the path and the
  removal command, never remove silently. (b) Also an explicit opt-in (`gwz doctor --remove-stale-locks`
  or a flag) that removes them only while gwz holds the workspace mutator lock and no `git` process has
  the repository open (best effort on each platform).
- **OD-C4 Exit status on interrupt.** Recommended: 130 on every platform (128 + SIGINT, the shell
  convention), with the JSON response marking the command `cancelled`.
- **OD-C5 A hung (not dead) core.** (a) **Recommended for this release:** no keepalive; the client's
  interrupt ends it, and `Stalled` covers a peer that stops reading. (b) A session keepalive (a
  `ServerState` ping with a bound), which changes the frozen wire and so is a 2.0-level amendment.
- **OD-C6 The pager and `forall` children.** Recommended: they keep the terminal's interrupt as the
  user's foreground programs; gwz stops iterating after the current one exits and never kills them.

## 7. What this does not change

The cancellation semantics of the contract (§5 cancel, abandon, host close), the session design's
`operation.cancel` and channel closure, the carrier's end reasons and bounds, and the helper's own
process group and job. It adds the interrupt that drives them, the safe points that let local work hear
them, and the reaper that keeps them true when gwz itself is gone.
