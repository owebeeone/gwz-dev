# Cancellation and process death: design amendment (DRAFT)

Date: 2026-10-31; revision 2 (2026-11-01). Status: **DRAFT, revision 2, for round-2 review by fresh reviewers
(Consistency and Safety).** Revision 2 applies [the round-1 remediation plan](GwzCancellationAmendment-RemPlan.md)
to revision 1 (blob `3448c561`, root `9d5a4d4c`), which both round-1 reviews found NO-GO, and that plan's two fix
rounds (gaps G1 to G15 and I1 to I19; then GAP-1 to GAP-17, S-R2-1 to S-R2-11 and F1). It takes as given the
operator's OD-C1 to OD-C6 (as recommended) and the three accept-and-remediate resolutions of
`GwzTransportReleaseConvergencePlan.md` §10 D-V (:1032-1045); two narrowings it cannot remove go to the operator as
**OD-C7 and OD-C8** (§6). **It authorizes no implementation, commit to a member, tag, push or publish. The clauses of
other documents it changes are listed in §8 and applied by the lane owner at acceptance.** It states how D-V's
reversal of OQ2 (:1005-1010) is met.

**Citations** are at root `bd082c89`, gwz-core `b745f1fa`, gwz-cli `c0ba717`, gwz-py `2185cc6`, gwz-git `9bc4ea9`,
gwz-transport `281a28d`, libgit2 `b172e3d18`, git2-rs `d13951f`, tokio 1.53.1 (gwz-core `Cargo.toml:139`), Rust std
1.96 and CPython 3.11. **Paths:** one that begins with a member (`gwz-core/`, `gwz-cli/`, `gwz-py/`, `gwz-git/`,
`gwz-transport/`, `gwz-sspi/`, `libgit2/`, `git2-rs/`) is relative to the workspace root; `tokio/` is tokio 1.53.1's
crate directory, `std/` Rust std's `library/std/`, `cpython/` CPython 3.11's `Lib/`; **every other source path is
relative to `gwz-core/`** (`src/…`, `crates/…`, `scripts/…`). A path in a workspace or repository (`.gwz/…`,
`gwz.conf/…`, `objects/…`) names a user's file, not a source. A bare `:N` is a line of the file last named. New files
the steps create are marked "create" in §4. Documents, all in the
workspace root's `dev-docs/`: TR1.6 (`gwz-core/dev-docs/history/reference/GwzTransportCredentialHelpersDesign.md`,
the one exception), the split doc (`GwzSplitCommandChannel.md`), parked (`history/parked/GwzCoreServerDesign.md`),
the session design (`GwzCoreSessionDesign.md`), the contract (`GwzConnectionApiContract.md`), the plan
(`GwzTransportReleaseConvergencePlan.md`) and the 2.3 plan (`GwzServerCore-Step2.3-Plan.md`).

**The operator's requirement (2026-10-31).** Ctrl-C cancels a gwz operation end to end: the client, every
helper process it started, and a serving core's in-flight call. Each side detects that a process it
depends on has gone away, through a pipe closing or another mechanism, and cleans up. Ctrl-C must not
leave anything behind: no orphaned process, no stale lock, no half-written state.

**What it amends.** TR1.6's accepted cost "a helper still waiting outlives it" and OQ2 (TR1.6 :76, :398) are
withdrawn. The split doc's interrupt rules (:239, :242), parked's launch isolation (:809, :1107, :1218-1223) and the
session design's cancellation (§5.3, :257-270) are kept and given owners. §8 lists every clause of another document
this changes.

## 1. Requirements

- **C1 Interrupt is cancellation.** On Unix SIGINT, SIGTERM and SIGHUP, and on Windows CTRL_C, CTRL_BREAK and
  CTRL_CLOSE, a gwz-cli command cancels every operation it runs, through the library's existing cancellation
  (§3.2), and exits only after that cancellation has finished or the bound of C5 has passed. A signal or event that
  was ignored when gwz started stays ignored (`nohup`; a background job of a non-interactive shell). SIGQUIT keeps
  its default action (OD-C7 (b)). gwz-py meets C1 through Python's own interrupt delivery (§3.7).
- **C2 No orphans.** Every process gwz code starts for an operation (§3.4) ends no later than that operation's
  terminal, and no later than gwz itself however gwz ends: an orderly exit, a cancel, a forced exit, SIGKILL, a
  crash or the console closing. The exceptions are named, E1 to E6 (§3.4); E3 and E6 narrow D-V and are OD-C8.
- **C3 Death is detected in both directions,** without polling the peer's state, within the bounds of §3.4's
  table: gwz and its children, the reaper and gwz, a client and its core, the `ssh` child and both.
- **C4 Nothing half-written.** An exit that runs gwz's code (`Cancel`; `Force`, §3.3) leaves every file gwz or
  libgit2 writes complete; or the temporaries of a transfer that `Force` ended (D-V (1) ends network work at once;
  §7 R1a); or the state of a run with a record. An exit that runs no gwz code can tear the one file being written
  and leave a Git `*.lock` (§7 R1, R1b). **Every residue has a named recognizer**, which runs in the commands and
  after the age §3.5's table gives (a younger gwz or libgit2 temporary may be another process's live write); the
  forced exit's own notice names the transfer temporaries it can leave, at once (§3.3). A Git `*.lock` is reported
  with its path and never removed by gwz (OD-C3).
- **C5 Bounded, then forced.** The first interrupt cancels and waits up to its form's bound: in-process, the
  five-second cleanup bound (contract :274); served, the session's close bound `close_wait` (session design :56,
  60 s by default), because the core answers `operation.cancel` only at the target's terminal (:183) and detaches a
  worker at that bound (:430). A second interrupt, or the bound passing, forces the exit (§3.3): children and network
  work end at once, and the exit waits only for each local write sequence to reach an exit point or its end. While a
  foreground program of OD-C6 runs no bound runs, and a second interrupt makes `Force` pending until it exits (E2).
- **C6 Parity and the code rules.** The same behaviour on macOS, Linux and Windows, except where §3 states how a
  platform's mechanism differs. No new global or thread-local state except one, which the OS imposes (OD-C1): the
  write end of the interrupt module's notification pipe (a descriptor on Unix, a handle on Windows). Every other
  signal-disposition change is allow-listed by site (§3.1). Conditional compilation only inside `cfg_if!` or
  platform modules.

## 2. What exists (facts, read 2026-11-01)

- **Signal handling.** gwz installs no handler of its own; `run` goes from `worker_host::early` to parsing
  (`gwz-cli/src/lib.rs:160-163`), so Ctrl-C ends gwz-cli by the OS default. But the credential helper is a
  `tokio::process` child (`src/git/endpoint/https_auth/process_tree.rs:42, :61-71`), and tokio's Unix process driver
  installs a SIGCHLD handler at its first spawn (macOS; Linux without pidfd) and reaps its children itself
  (`tokio/src/process/unix/mod.rs:123-137`; `tokio/src/process/unix/orphan.rs:102`).
  `scripts/checks/check_process_globals.py` is one checker run against each repository's allowlist
  (`--allowlist`, :808-817): gwz-core's `scripts/checks/process_globals_allowlist.json`,
  `gwz-cli/scripts/process_globals_allowlist.json` and `gwz-py/scripts/process_globals_allowlist.json`. It lists
  `libc::signal`, `libc::sigaction` and `libc::chdir` in `LIBC_FUNCS` (`scripts/checks/check_process_globals.py:103-104`),
  classed `env` (:540-541).
- **Cancellation exists; the CLI does not use it.** `CancellationToken` (`crates/session-host/src/token.rs:72, :88,
  :117`), `CallControls::cancel` (`crates/session-host/src/gate.rs:156`) and the cancellable entries
  (`src/transport_host/cancellable.rs:40-68`) reach `Session::cancel` (`src/transport_host/session/requests.rs:37`);
  gwz-cli calls the non-cancellable `with_local_transport_native*` (`src/transport_host/local_command.rs:11-32`;
  `gwz-cli/src/globalargs/dispatch.rs:67-80`).
- **Local handlers.** "A local handler observes cancellation only at gate crossings. Once it holds what it needs,
  such as its member and workspace locks, it runs to its end" (session design :268); every gate crossing, "local
  handlers included", is a cancellation point (:265), and each fetch or push member step is one (:224). The served
  dispatch checks the token once before a handler (`src/session_host/served/dispatch.rs:101-107`). The workspace
  mutator lock is a try-lock (`src/operation/workspace_mutator_lock.rs:13-22`) that commit and stage take; a member
  lock is taken only by network ref updates, and its wait blocks until the member is free and wakes on the call's
  cancellation (`src/operation/memberlockmanager.rs:9-10, :13`). Stage is one `add_all`, whose per-path callback is
  unused, then one index write (`src/git/gitbackend/repository.rs:108-115`; `git2-rs/src/index.rs:51, :357-361`).
  `stash push` is one `stash_save`, which resets the working tree inside libgit2 (`src/git/gitbackend/stash.rs:18`;
  `src/git/gitbackend/repository_support.rs:18`).
- **Commit and tag can start a child (F1).** gwz-core's commit and tag call gwz-git's `native_commit`
  (`src/git/gitbackend/commit_tag.rs:1-5, :31`), which runs porcelain `git commit` or `git tag`, with its hooks and
  signing, when its route rule finds a hook that would fire, signing or a configuration libgit2 cannot reproduce
  (`gwz-git/src/native_commit/route.rs:21-46`; `gwz-git/src/native_commit/cli.rs:1-5, :38`). The child is launched
  through the caller's `ProcessGate::output` (`gwz-git/src/native_commit.rs:88-95`), which gwz-core implements with
  its spawn gate (`src/git/spawn_gate.rs:449-454`).
- **Native transfers.** git2-rs's `sideband_progress` and `transfer_progress` return `bool` and can abort
  (`git2-rs/src/remote_callbacks.rs:48, :184`); `push_negotiation` returns a `Result` before the pack (:105);
  `push_transfer_progress` returns nothing (:86, :528-546). libgit2's fetch is `download` then `update_tips`
  (`libgit2/src/libgit2/remote.c:1352-1409`) and its push is `upload` then `update_tips` (:3045-3067); git2-rs exposes
  `download` and `update_tips` (`git2-rs/src/remote.rs:265, :341`) but no `upload` (:370 is `push`). The indexer
  writes `pack-<hash>.idx` through a `.lock` and then renames the pack (`libgit2/src/libgit2/indexer.c:1325, :1391,
  :1428`). gwz fetches with `Remote::fetch` (`src/git/gitbackend/transport.rs:110, :150, :340`); a push to a local
  URL goes through an anonymous remote and gwz writes the tracking refs itself (:480-494, :611-616).
- **Checkout.** libgit2's checkout progress callback returns `void` (`libgit2/include/git2/checkout.h:291-295`) and
  runs after each file is written (`libgit2/src/libgit2/checkout.c:1886-1891`); the index is written once, at the end
  (:2686). gwz checks out through `CheckoutBuilder` at `src/git/gitbackend/refs.rs:33, :134`,
  `src/git/gitbackend/repository.rs:56, :81`, `src/git/gitbackend/merge_prepared.rs:264`,
  `src/git/gitbackend/merge_recovery.rs:166, :246` and `src/git/gitbackend/transport.rs:41` (the clone).
- **Children gwz code spawns.** (1) The credential helper `git credential fill`: on Unix the leader of its own group,
  killed with `kill(-pgid, SIGKILL)` at the bound, on cancel and on drop; `retire` leaves what it started running
  (`src/git/endpoint/https_auth/process_tree.rs:12-13, :57-95`; `src/git/endpoint/https_auth/owner.rs:253-303`). On
  Windows its own `HelperChild` (named pipes, a polled wait, `src/git/endpoint/https_auth/process_tree/windows.rs:41-57,
  :151-159`) in a `KILL_ON_JOB_CLOSE` job whose limit `retire` clears (:54-80, :106-118). Its input, `url=…` and a
  blank line, is written and shut (`src/git/endpoint/https_auth/lookup.rs:126`; `src/git/endpoint/https_auth/pipes.rs:9-16`).
  (2) `git rev-list` for a path-filtered `log`, through `SpawnGate::spawn`, no group, no kill on cancel
  (`src/operation/commit_log/mod.rs:330-358`, :354). (3) The Git CLI route's `git commit` and `git tag` (F1), through
  `SpawnGate::output`. These are gwz-core's only production spawns. (4) In gwz-cli, the pager
  (`gwz-cli/src/pager.rs:147-165`), `forall`'s commands (`gwz-cli/src/forall.rs:231-262`) and `hook`'s `df`, `stat`,
  `mount`, `cp`, `fsutil` (`gwz-cli/src/hook/env.rs:158-344`) and `git` (`gwz-cli/src/hook/fallback.rs:21`). (5) On
  Windows the gwz-sspi worker, in a `KILL_ON_JOB_CLOSE` job (`gwz-sspi/src/supervisor/windows/launch.rs:276-278`).
  Not gwz's spawn: the native route's helpers, started by git2-rs inside `Cred::credential_helper`
  (`src/git/gitbackend/transport_support.rs:313-321`).
- **The spawn gate.** On Windows a `CreateProcessW` with inheritance inherits every inheritable handle, so a launch
  that hands a child its own pipe ends holds the gate for that window, and every other gwz spawn holds it across the
  creation only (`src/git/spawn_gate.rs:1-11`; `spawn` and `output`, :431-446). The composition makes it once
  (`src/transport_host/composition.rs:1-8, :31-35`); a backend holds the request context's gate
  (`src/git/gitbackend/backend.rs:26, :120-125`), and a backend with no host an ungated one of its own (:58-70, :69).
- **Rust std's spawn** runs `setpgid` (`std/src/sys/process/unix/unix.rs:342`), then resets the signal mask and sets
  SIGPIPE to `SIG_DFL` (:353-368), then the `pre_exec` closures (:383), then `execvp` (:408). When `exec` fails it
  reaps the child itself and returns the error (:141-150).
- **The carrier detects a dead peer.** End of file and peer-side errors are `PeerGone`; a peer that stops reading is
  `Stalled` after 60 s (`src/transport_host/carrier/mod.rs:328-332`; `src/transport_host/carrier/stream/inbound.rs:362-365`;
  `src/transport_host/carrier/stream/writer.rs:283-291, :378`). On carrier end the operation service cancels running
  calls and settles unstarted ones `Cancelled`, waiting up to `close_wait` (`crates/session-host/src/service/workers.rs:213-236`);
  the client fails open exchanges `carrier_lost` (`gwz-transport/crates/gwz-connection/src/remote/core.rs:337-347, :501-505`).
- **Leftover state.** gwz's cross-process locks are advisory, released when the last descriptor of the file closes
  (`src/operation/workspace_mutator_lock.rs:49-52`); `FileSystem::try_lock_file` (`src/filesystem.rs:350`) is
  `flock(LOCK_EX | LOCK_NB)` on Unix and `LockFileEx` on Windows (`src/filesystem/native/platform_lock.rs:7-24,
  :44-71`). No production code removes a temp a crash left (`crates/family-store/src/publish.rs:239` is a test).
  libgit2 stages `objects/pack/pack_git2_<16 hex>` and `objects/tmp_object_git2_<16 hex>`
  (`libgit2/src/util/futils.c:28-45`; `libgit2/src/libgit2/indexer.c:202`; `libgit2/src/libgit2/odb_loose.c:884`),
  removes a pack only on error or when the indexer is freed (`libgit2/src/libgit2/indexer.c:221-228`), and refuses an
  existing `<path>.lock` with `GIT_ELOCKED`, never judging staleness (`libgit2/src/util/filebuf.c:44-52`). The merge
  record's verified write "is not crash recovery": a leftover is operator cleanup (`src/verified_write.rs:30-34`).
- **Output.** Exit codes are 0, 1 or 2 from the aggregate (`gwz-cli/src/globalargs/render_exit.rs:82-95`;
  `gwz-cli/docs/MachineOutput.md:954-964`); gwz-cli writes through `print!`, `println!`, `eprint!` and `eprintln!` at 32
  production sites in 8 files (e.g. `gwz-cli/src/lib.rs:167-216`), which panic on a failed write. `AggregateStatus`
  has no cancelled value (`src/protocol/generated.rs:1133-1142`); a response carries `errors` (:4945-4949);
  `cancelled` is code 73 (`docs/ErrorCatalog.md:35`); the catalog's table has no `open_operation` row.
- **gwz-py.** `_run_native` runs the call on `asyncio.to_thread` and on `CancelledError` only waits
  (`gwz-py/src/gwz/bridge.py:298-316`, :28-35). The native route has no `Canceller`
  (`gwz-py/native/src/route/transport.rs:171`; `gwz-py/native/src/client_host/operations.rs:216-221`). The CLI runs
  `asyncio.run` (`gwz-py/src/gwz/cli.py:147`) on Python 3.10 or later (`gwz-py/pyproject.toml:6`). CPython 3.11's
  runner cancels the main task on the first SIGINT and raises `KeyboardInterrupt` on the next
  (`cpython/asyncio/runners.py:150-157`); its `shutdown_default_executor` waits for the executor with no bound
  (`cpython/asyncio/base_events.py:565-580`); `concurrent.futures` joins its worker threads at interpreter exit
  (`cpython/concurrent/futures/thread.py:23-37`); asyncio's `FastChildWatcher` reaps with `os.waitpid(-1)`
  (`cpython/asyncio/unix_events.py:1115-1182`).

## 3. Design

### 3.1 The interrupt source (OD-C1)

- **One module.** gwz-cli's `gwz-cli/src/interrupt/` holds the process's only signal or console handler and every
  disposition change gwz-cli makes. It is one entry of kind `signal` in `gwz-cli/scripts/process_globals_allowlist.json`
  (disposition `permanent`; reason: this section; its `count` is the module's sites).
- **Where `run` calls it.** First `reset_child_disposition()`: a SIGCHLD inherited as `SIG_IGN` or with
  `SA_NOCLDWAIT` is set to `SIG_DFL` (§3.4 "Children that are never zombies"); on Windows a no-op. Then, from step
  3.1, `child_registry()` (§3.4: it forks while the process has one thread). Then `install()`, only for a command
  that consumes `Interrupts`: from step 2.3s the server family (`server start --foreground`, `server stdio`); from
  step 3.1 also every command run with `--server`; from step 3.1c every command. **A command that does not install
  keeps the OS default**, so until 3.1c an in-process command dies on Ctrl-C as on 1.0.18 (row 8a).
- **Inherited dispositions.** `install()` reads each signal's disposition first and leaves one that is `SIG_IGN`
  ignored; it saves the others. On Windows a process whose CTRL_C was disabled at creation
  (`SetConsoleCtrlHandler(NULL, TRUE)`, inherited; or `CREATE_NEW_PROCESS_GROUP`) gets no CTRL_C in any handler, so
  adding the module's handler leaves that unchanged; CTRL_BREAK and CTRL_CLOSE still arrive.
- **Enforced.** `scripts/checks/check_process_globals.py` gains the kind `signal`, matching `libc::signal`, `libc::sigaction`,
  `SetConsoleCtrlHandler`, `signal_hook`, `nix::sys::signal`, `ctrlc` and `tokio::signal`. Its occurrences are
  exactly two allow-listed entries: gwz-cli's module (above) and gwz-core's children crate (`crates/children/`) in
  `scripts/checks/process_globals_allowlist.json`, reason "a spawned child's or the reaper's own dispositions, and the
  SIGCHLD query" (§3.4: the pre-exec SIGPIPE step, the `Process` kind's ignored SIGINT and SIGQUIT, the reaper's
  dispositions, the SIGCHLD read). The children crate's `libc::chdir` (the reaper's) is one `env` entry in the same
  file. gwz-py's allowlist gains nothing, so any use there fails. A dependency check
  (`scripts/checks/check_tokio_features.py`, `cargo tree -e features` for gwz-core, gwz-cli and the gwz-py module)
  refuses tokio's `process` and `signal` features, so no SIGCHLD handler is compiled in (§3.4).
- **What it yields.** An `Interrupts` value: a receiver of `InterruptEvent::Cancel` (the first interrupt) and
  `InterruptEvent::Force` (the second, or CTRL_CLOSE). The type is gwz-core's (`src/interrupt.rs`, step 2.3i: a
  channel, no state of its own); gwz-cli's module is its only producer. The composition root passes it by value; it
  also holds the saved dispositions, so restoring them needs no global.
- **Unix.** `sigaction` with `SA_RESTART` and an empty `sa_mask` for each of SIGINT, SIGTERM and SIGHUP not
  inherited ignored. The handler saves `errno`, writes one byte to a pipe whose ends are `O_CLOEXEC` and whose write
  end is `O_NONBLOCK` (made while the process has one thread; on macOS `pipe`, then `fcntl`), ignores `EAGAIN`,
  restores `errno` and returns. A thread the module owns reads the pipe: its first byte is `Cancel`, its second `Force`.
- **Windows.** `SetConsoleCtrlHandler`; the handler runs on a thread the OS creates and writes to the same kind of
  pipe (an anonymous pipe's write end, the allow-listed global). CTRL_C and CTRL_BREAK write a count byte and return
  `TRUE`. CTRL_CLOSE writes a byte that means `Force` at once, then sleeps without end: the process ends by
  `force_exit`'s `TerminateProcess` or by the OS about 5 s after CTRL_CLOSE, whatever runs (OD-C7 (c)). The handler
  needs no other state.
- **Giving control back (OD-C7 (a)).** When `Force` begins, and when the `Interrupts` value is dropped, the module
  restores each saved disposition (on Windows it removes its handler). An interrupt that arrives after `Force` has
  begun therefore acts as it would have without gwz's handler: by default it ends gwz (the second, when the bound
  began `Force`, else the third).
- **The server.** `gwz server start --foreground`, the background server and `gwz server stdio` receive the value:
  `Cancel` is the server design's first stop signal and `Force` its second (split doc :239). 2.3i takes the value and
  installs no handler (§8 rows 5 and 19).

### 3.2 Cancel reaches the work

- **The command's operations.** From step 3.1 every operation of a command is admitted through its `HostOwner` with
  its own `CallControls`. In-process, `Cancel` cancels each; served, it sends `operation.cancel` for each live call
  (§3.6). Everything below the token is existing plumbing.
- **Local handlers: §5.3 unchanged.** A handler observes `Cancel` only at the gate crossings §5.3 already has:
  before it starts, in a lock wait, and at each network request it makes (session design :224, :265). So:
  - a W handler that holds the workspace mutator lock and makes no network request (commit, stage) runs to its end:
    every member completes or none starts; its Git CLI route child (F1) is not killed by `Cancel` (§3.4 `Writer`);
  - a W handler whose members each make an idempotent network step (pull, materialize) can stop at a later member's
    crossing: each earlier member is whole, later members are not started, the response is `Partial` with those
    members `cancelled`, and re-running the command finishes it. No member stops part-way under `Cancel`.
- **Native transfers (step 3.0c).** Every fetch site calls `download` and then `update_tips` instead of `fetch`,
  with `transfer_progress` and `sideband_progress` installed whatever the progress sink; both return `false` once the
  token is cancelled, and libgit2 aborts and frees its indexer, which removes its temporary pack
  (`libgit2/src/libgit2/indexer.c:221-228`); a cancelled `download` writes no ref. `push_negotiation` returns an error
  once cancelled, so a push cancelled before its pack has no effect. Every push goes through an anonymous remote,
  whose `update_tips` writes no tracking ref (`src/git/gitbackend/transport.rs:611-616`), and gwz writes the tracking
  refs after it. A clone downloads with checkout disabled, then checks out as its own step (§3.3). Not covered, and
  bounded by C5: connect, negotiation and credential phases (no callback runs; the libgit2 timeout clock bounds them)
  and a native push's upload (§7 R2). The credential callback is not changed by 3.0c.
- **Push phase.** Every push member publishes its phase (`before_upload`, then `upload_started`) where a forced exit
  reads it without a lock a worker can hold. A member cancelled or force-ended after `upload_started` reports
  `cancelled` with a `detail` that begins `possible remote effect:`: in the rendered text and JSON when a rendering
  or envelope is written, and otherwise in the forced notice on standard error (§3.3). The transport already maps
  `Effect::Possible` (`src/git/endpoint/https_worker/serve.rs:151`; `src/transport_host/session/driver/opening.rs:326`).
  Served, the client reports every push call it abandons this way, whatever its phase, since it cannot know it (§3.6).

### 3.3 Exit: Cancel first, Force second

1. **`Cancel` (the first interrupt).** Cancel the command's operations (§3.2); wait up to C5's bound for their
   terminals and for every registered `Group`, `Process` and `Writer` child to be reaped (not `Stdio`, §3.4); render
   the response through the `Output` sink (below), each cancelled member with `error.code = cancelled`; write one line
   per cancelled operation to standard error; exit 130.
2. **`Force` (a second interrupt, the bound passing, or CTRL_CLOSE),** `exit_points::force_exit` (gwz-core), in this
   order: (a) the caller restores the saved dispositions (§3.1); (b) `kill_all` on the child registry's `Group` and
   `Process` entries (§3.4; a `Writer` is waited for with its W run, and the local stdio core's standard input is
   closed instead, §3.6), and a served command closes its carrier; (c) close the exit points, so no write sequence
   begins or leaves a parked state and the registry refuses spawns; (d) start the notice on a detached writer thread
   (single writes whose errors are ignored); (e) wait until every write sequence is parked or ended; (f) wait at most
   1 s more for the writer, then exit 130 without unwinding or destructors (`_exit`; on Windows `TerminateProcess` on
   the current process).
- **Write sequences: Force's wait set.** `ExitPoints` (gwz-core, minted by the composition with the registry; a clone
  is another handle) hands out `WriteSequence` guards, and Force waits for exactly these:
  - **every W method's run:** the dispatch that runs a handler (the served dispatch and the in-process one) opens a
    sequence before a W handler starts and ends it after the handler returns; every atomic write, libgit2 ref, index
    or config update, and child the handler makes lies inside it;
  - **every N step's local write:** a fetch's `update_tips` and a push's tracking-ref update each run in a sequence.
  A sequence is **parked** (a) at an exit point; (b) for the duration of a network call it brackets
  (`WriteSequence::network`: a fetch's `download`, a push, a clone's download), during which gwz writes nothing and
  libgit2 writes only its temporaries and the pack's publication (§3.5's rows); and (c) for the duration of a member
  lock wait (`WriteSequence::waiting`), during which it writes nothing. Once the exit points are closed, a sequence
  that reaches an exit point, returns from a network call or obtains its lock blocks until the process exits, so it
  writes nothing more. So a W run waiting for a member that another session's parked fetch holds does not hold up
  `Force`, a five-member commit is waited for to its end (no exit point; its CLI route child included), and a
  stalled fetch is not waited for.
- **Exit points** exist only where an exit leaves the state unchanged, recorded or under an existing recovery:
  a libgit2 checkout's progress callback (between two complete files, §2); the `Cancellation` poll of the copy loops of
  `local clone`, refcopy and the family import (`crates/copy-contract/src/lib.rs:40-52`); and stage's per-path points,
  `add_all`'s callback (today `None`, `src/git/gitbackend/repository.rs:112`) and the per-path loop of
  `stage_paths_allowing_other_conflicts` (:151-168): before each path only complete loose objects exist and the index is
  written once at the end (:115, :169), so a stage needs no record.
- **Long local phases (D-V (1)).** The W phases whose length grows with the working tree or the data are checkout,
  the copy loops and stage, each with file-granular exit points (above). Two have no callback to place one in: `stash
  push`'s working-tree reset inside one `stash_save` (§2), and a Git CLI route child with its hooks (F1). `Force`
  waits for them (§7 R10). Every other W phase writes a bounded set per member: one index, one tree and commit, the
  lock and markers (`src/workspace_ops/handle_commit.rs:179, :200, :255`).
- **Records.** A handler whose run has exit points and no existing recovery (every checkout outside a merge:
  `switch`, pull's update, `stash apply`, materialize's checkout and clone) writes an **interrupted-operation
  record**, `.gwz/interrupted/<operation id>.yaml` (the command, its target, its members, its start time), before its
  first write, and removes it when the handler ends, whatever the outcome. **Liveness is the record's own lock:** the
  handler writes it as the durable write does (unique temp, fsync, rename) but takes `try_lock_file` on the temp before
  the rename, and holds that lock until after it has removed the record. A reader treats a record as interrupted only
  when its own try-lock succeeds and the path still names the file it locked; a record whose lock is held is a live run
  ("running in another process"), refused nothing. **On Windows every open of a record, the owner's and each
  reader's, shares `FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE`**, so a reader's handle never blocks the
  owner's rename or removal; a reader whose open finds the record gone or delete-pending treats the run as ended. A
  merge's checkout belongs to the merge record, `local clone`'s copies to the `creating` row.
- **The `Output` sink (gwz-cli, step 3.1c).** Every write gwz-cli makes to standard output or standard error goes
  through one sink: `write_all` on the stream under a lock; an error ends that stream for the rest of the process and
  never panics. A source check refuses the four print macros in gwz-cli production code. Force's writer never waits
  for a stream lock: a stream another write holds gets nothing.
- **Output and status (OD-C4).** An interrupted command exits 130 on every platform, whatever its aggregate; on
  Windows this replaces `STATUS_CONTROL_C_EXIT`. The wire is unchanged. The aggregate is computed from the members as
  today; the response's `errors[0].code` is `cancelled`. After `Cancel`, standard output carries the usual rendering
  and standard error the notice. After `Force`: if nothing has been written to standard output, `--json` writes one
  minimal envelope (`meta`, `members: []`, `errors: [cancelled]`) whose `detail` names each operation that reached no
  terminal and each push past `upload_started` as `possible remote effect`, and `--jsonl` the same as one final line;
  if a rendering had begun, and in human and porcelain modes, Force writes nothing more to standard output. **The
  forced notice** on standard error is, in every mode: one line `interrupted` (with `output incomplete` when a
  rendering had begun), then one line `possible remote effect: <member> (<remote>)` per push past `upload_started`,
  and one line per member whose `download` Force ended, naming its `objects` directory and that a later fetch reports
  what remains (§3.5). A consumer that sees exit 130 and no complete document treats the run as interrupted.

### 3.4 Children: the registry, the reaper and jobs

- **Registry.** `ChildRegistry`, in a new crate `crates/children` that does not depend on gwz-core, is the only
  spawner of an operation's children. The composition (`src/transport_host/composition.rs`, beside `process_safety()`)
  is its only minting site (a source check, as `scripts/checks/check_spawn_gate_minting.py` holds the gate's):
  `child_registry()`, with a reaper, called only by the composition roots (gwz-cli's `run`, step 3.1; gwz-py's
  composition object, step 3.2); and `child_registry_without_reaper()`, called only by the old entries
  (`src/transport_host/local_command.rs`, `src/transport_host/cancellable.rs`) and the hostless backend constructors
  (below) until step 3.3. A reaperless registry ends its `Group`, `Process` and `Writer` children by the kill paths
  of the kinds table, but not after a SIGKILL of gwz on Unix; it makes no lifeline. The registry is the
  Composition/CleanupDomain's kill authority (contract :289); a child not reaped within its grace is retained with its
  slot as pending cleanup (`src/git/endpoint/https_auth/owner.rs:280-303`) and counted once, in the retained record
  (§8 row 11).
- **The spawn gate.** The registry does not replace it. The composition mints the registry with the process's gate,
  passed as the children crate's `SpawnExclusion` trait, which `SpawnGate` implements; every registry spawn holds it
  across the creation only (a launch with a handle list as a window), never across the wait (§2). `SpawnGate::spawn`
  and `output` are removed; their callers, `rev-list` and gwz-core's `ProcessGate` impl for gwz-git's CLI route, call
  the registry (`Writer`). A backend holds a registry handle beside its gate, set from the request context as the
  gate is (`src/git/gitbackend/backend.rs:120-125`); a backend with no host (`Git2Backend::new` and
  `without_credential_helpers`, :58-70, :103-114) gets a reaperless registry of its own, as it gets an ungated gate (R9).
- **How it spawns.** On Unix with std's `Command` and one `pre_exec` step (the crate is the only user of `pre_exec`),
  never `tokio::process`: the helper's Unix arm gets its own `HelperChild` (its pipes as `tokio::net::unix::pipe` ends,
  `tokio/src/net/unix/pipe.rs:394, :942`; its wait a future the registry completes), as the Windows arm already has,
  and gwz-core drops tokio's `process` feature (`Cargo.toml:139`). A **waiter thread** per running child blocks in
  `waitid(P_PID, pid, WEXITED | WNOWAIT)`, writes the unregistration, then reaps with `waitpid`; until then the zombie
  keeps its process and group ID from reuse. No SIGCHLD handler exists. On Windows with `CreateProcessW`, as
  `src/git/endpoint/https_auth/process_tree/windows.rs` does, and a thread per child waiting on its handle.
- **Which spawns.** A source check (`scripts/checks/check_child_spawns.py`) refuses `Command::spawn`, `output`,
  `status`, `CreateProcessW`, `fork` and `tokio::process` in production code outside the children crate, with an
  allow-list: the E2 sites (`gwz-cli/src/pager.rs`, `gwz-cli/src/forall.rs`), E4's background `server start` launch
  (2.3k), and nothing else. It covers gwz-core from step 2.3r (whose `src/git/spawn_gate.rs` sites it removes),
  gwz-cli from step 3.1c (after `hook` moves onto the registry) and the gwz-py module from 3.2b. The gwz-sspi worker is
  in its own repository and keeps its own kill-on-close job (§2). Kinds, and the only paths that kill each:

| Kind | Children | Unix | Windows | Killed by |
|---|---|---|---|---|
| `Group` | the credential helper; `git rev-list` (moved into a group of its own: it needs no terminal); `hook`'s utilities | leads its own group; a lifeline record; `kill(-pgid, SIGKILL)` | its own `KILL_ON_JOB_CLOSE` job; `TerminateJobObject` | its operation's cancel, `Force`, registry drop and close, the reaper or job |
| `Process` | the `ssh` child of `ssh://` | stays in gwz's process group, since its prompts read the terminal (parked :1212), with SIGINT and SIGQUIT ignored in its pre-exec step (:1218); a lifeline record; `kill(pid, SIGKILL)`, on Linux through a pidfd (below) | `CREATE_NEW_PROCESS_GROUP`, its own `KILL_ON_JOB_CLOSE` job | as `Group` |
| `Writer` | the Git CLI route's `git commit` and `git tag` (F1) | leads its own group, so the terminal's interrupt never reaches it mid-write (a hook that reads the terminal is stopped, §7 R11); a lifeline record; `kill(-pgid, SIGKILL)` | `CREATE_NEW_PROCESS_GROUP`, its own `KILL_ON_JOB_CLOSE` job | registry drop and close (an orderly path ends its W run first), the reaper or job; **never cancel or `Force`**, which wait for its W run |
| `Stdio` | the local `gwz server stdio` child | a new session (parked :809); **no lifeline record**, so the reaper never knows it | `CREATE_NEW_PROCESS_GROUP`, **no** kill-on-close job | **none of the registry's paths**; it ends itself (§3.6) |

  `ssh`'s own children (a `ProxyCommand` or `ProxyJump`; `ControlMaster=no` and `ControlPath=none` rule out a
  master, split doc :247) are not registered: they talk to `ssh` through pipes and end at those pipes' end of file
  when `ssh` dies; one that ignores it lives on (E5).
- **Registration.** Each `Group`, `Process` and `Writer` spawn takes a token (the registry's counter). The child
  registers itself in its pre-exec step, so a gwz killed just after `spawn` returns leaves it registered. Its 16-byte
  record is the operation (register or unregister), the kind, the token and the ID. std has already set SIGPIPE to
  `SIG_DFL` and emptied the mask (§2), so the step: sets SIGPIPE to `SIG_IGN` with `sigaction`; writes the record
  with one `write` (atomic: below `PIPE_BUF`); sets SIGPIPE back to `SIG_DFL` with `sigaction`; closes the lifeline;
  and returns the write's error (`EPIPE` once the reaper is gone), which fails the spawn before the program runs.
  Every call is async-signal-safe. **On every spawn error the registry writes the token's unregistration:** when
  `exec` fails after the step ran, std reaps the child itself before it returns the error (§2), so no waiter exists;
  the reaper ignores the unregistration of a token it does not hold. Between std's reap and that write the ID is not
  pinned (§7 R4). gwz's own lifeline writes (unregistrations, on waiter threads and after a spawn error) block SIGPIPE
  on the writing thread and, on `EPIPE`, consume the pending one with `sigpending` then `sigwait` (ordinary threads,
  not after a fork). `retire` (a helper that succeeded) unregisters (E1). "A group ends" means its entry was
  unregistered, at `retire` or after its leader exited.
- **Foreign reaping.** If the waiter's `waitid` returns `ECHILD`, something else reaped the leader (an embedding
  application calling `waitpid(-1)`, such as asyncio's `FastChildWatcher`); the waiter unregisters at once. Between
  that reap and the unregistration the ID can be reused (§7 R4); gwz-py's README states the constraint.
- **Children that are never zombies.** If SIGCHLD is ignored or has `SA_NOCLDWAIT` (inherited across `exec` from a
  parent, or set by an embedding application), children are reaped by the kernel and no ID can be pinned. The registry
  reads SIGCHLD's disposition (`sigaction` with no new action) when it is created and before each spawn, and while
  either holds refuses with `RegistryFault::ChildReapingDisabled` before the child starts. gwz-cli resets an inherited
  one first (§3.1; until 2.3s lands, gwz-cli gets the refusal); gwz-py installs nothing and its README states it.
- **The reaper, on macOS and Linux (D-V resolution (2)).** `child_registry()` forks it. gwz-cli's `run` calls it
  before any thread exists (before the interrupt module's reader thread); gwz-py's composition object calls it once
  per process, before any host exists. Before the fork the creating thread makes the lifeline pipe (`O_CLOEXEC`;
  `pipe2` on Linux; on macOS `pipe` then `fcntl`, while no gwz spawn can run), allocates the entry table (capacity set
  by the composition, 256 by default; a pidfd slot per entry on Linux), opens `/dev/null` and reads the soft
  `RLIMIT_NOFILE`. **Right after the fork the parent closes its read end**, so the reaper holds the only one: a write
  fails with `EPIPE` once the reaper is gone, and no later child inherits it. The forked child runs only
  async-signal-safe calls and raw system calls, never allocates, and never returns into Rust or Python:
  - `setsid()`, so no terminal or job-control signal reaches it; `sigaction` ignoring SIGINT, SIGQUIT, SIGHUP,
    SIGTERM, SIGTSTP, SIGTTIN, SIGTTOU and SIGPIPE; an empty mask; `chdir("/")`; `/dev/null` onto descriptors 0 to 2;
  - it closes every other descriptor, the lifeline's write end included (`close_range` where the Linux kernel has it,
    otherwise each up to the soft limit), so it holds no lock, socket, pipe or terminal of gwz's;
  - its loop reads 16-byte records into the table by token, accumulating partial reads. For a `Process` entry on
    Linux it opens a pidfd (`pidfd_open`) as it reads the registration, while gwz's zombie pin holds the ID, and
    closes it at the unregistration; where `pidfd_open` fails it keeps the PID. On end of file it sends `SIGKILL` to
    every entry still registered (`kill(-pgid)`; `pidfd_send_signal`, else `kill(pid)`) and calls `_exit(0)`; any
    other read error calls `_exit(1)` without killing anything. It reads no environment and runs no program.
  - **Its death.** gwz's next lifeline write (a pre-exec registration, or an unregistration) fails with `EPIPE`; the
    registry then refuses every later spawn with `RegistryFault::ReaperGone` before the child starts (fail closed).
    The children already running stay covered by the kill paths of their kind, not by a SIGKILL of gwz (§7 R9). A
    failed fork leaves the registry in that state from its creation. A full table refuses a spawn with `capacity`.
  - **Its end.** Registry close kills what remains registered of `Group`, `Process` and `Writer` and closes the write
    end; the reaper reads end of file and exits; gwz reaps it and treats `ECHILD` as already reaped.
  - **Embedding constraints (gwz-py).** An application that forks without exec (`multiprocessing`'s `fork` start
    method) copies the write end into its workers, and the reaper sees end of file only when every copy has closed
    (§7 R6). gwz-py's README states this and recommends `spawn` or `forkserver`; it states the foreign-reaping case and
    the SIGCHLD refusal.
  - **Cost.** One fork per process. Its pages are shared copy-on-write; its cost grows with the pages the parent
    writes later, up to the parent's size at the fork (§7 R7). Step 3.4 measures it.
- **Windows.** No reaper: each `Group`, `Process` and `Writer` entry has a `KILL_ON_JOB_CLOSE` job that holds it from
  its first instruction (as `src/git/endpoint/https_auth/process_tree/windows.rs:54-80`); however gwz's process ends,
  its handles close and the jobs end their processes.
- **Exceptions to C2.**
  - **E1** what a successful helper leaves running (TR1.6 §3.3, :82; `src/git/endpoint/https_auth/process_tree.rs:12-13`).
  - **E2** the user's foreground programs, the pager and `forall`'s commands (OD-C6). They keep the terminal's
    interrupt. On `Cancel` gwz stops producing output or iterating, closes the pager's input, writes "waiting for
    <program> to exit" to standard error and waits with no bound. **A second interrupt during the wait** restores
    the saved dispositions (§3.1) and makes `Force` pending; `Force` runs when the program exits. A third interrupt
    then ends gwz by its default action; gwz writes nothing during the wait, so this tears nothing, and the program
    runs on as when it outlives gwz on 1.0.18. **CTRL_CLOSE does not wait:** it runs `Force` at once, since the
    console is closing and the program receives its own CTRL_CLOSE. A SIGKILL of gwz leaves the program running.
  - **E3** the local stdio core, which outlives its client by at most `close_wait` + 10 s (§3.6). OD-C8 (b).
  - **E4** `gwz server start`'s background server, detached on purpose (2.3k).
  - **E5** processes another program or broker starts: gpg-agent's pinentry, WMI or COM (TR1.6 :75;
    `src/git/endpoint/https_auth/process_tree.rs:22-24`), and `ssh`'s own children.
  - **E6** the native route's helpers, spawned by git2-rs inside one call (`src/git/gitbackend/transport_support.rs:313-321`),
    in gwz's foreground group; after a SIGKILL of gwz, or a `Force` from a signal sent to gwz alone, they live until
    they end, as on 1.0.18. OD-C8 (a) would remove E6.
- **Death detection (C3).**

| Who notices | Whose death | How | Bound |
|---|---|---|---|
| the reaper | gwz (macOS, Linux) | the lifeline's end of file | every registered child killed within 1 s (row 2) |
| the jobs | gwz (Windows) | gwz's handles close | at gwz's end |
| gwz | a child | its waiter's `waitid`; its pipes' end of file | immediate |
| gwz | the reaper | `EPIPE` on its next lifeline write | at the next spawn or unregistration, which then fails closed |
| a core | its client | `PeerGone`; the stdio core's end of input | immediate; `Stalled` after 60 s |
| a client | its core | `PeerGone`; `Stalled` | immediate; 60 s; a hung idle core is OD-C5's residual |
| the `ssh` child | its client | end of its standard input; the reaper or its job | immediate |
| a remote core | its client, over `ssh` | `ssh` exits, so its input ends; `ServerAliveInterval=15` × 3 | at most 45 s (split doc :247) |

### 3.5 Leftover state: each residue and its recognizer

No residue is removed by gwz: each is reported with its path, its age by `mtime`, what left it and the removal
command (`rm -- '<path>'`; on Windows `Remove-Item -LiteralPath '<path>'` and `del "<path>"`). A report is one line on
standard error in every output mode (§8 row 12).

| Residue | Left by | Recognized by | Then |
|---|---|---|---|
| a gwz temp of the inventory below | an exit inside an atomic write | every command that resolves the workspace, listing the inventory's directories | reported when older than 1 h; a younger one may be another process's live write |
| in `objects/pack`: `pack_git2_*`, a `pack-<hash>.idx.lock`, a `pack-<hash>.idx` with no `.pack`; `objects/tmp_object_git2_*` | `Force` (§7 R1a) or a kill during a `download` (§3.3) | the forced notice, at once, by member (§3.3); then a fetch-class step on the member, before its network work | reported when older than 1 h; no cross-process exclusion covers fetch (`src/operation/memberlockmanager.rs:4-15`), so a younger one may be a live fetch's |
| a Git `*.lock` (a ref, `packed-refs`, `index`, `config`) | an exit that runs no gwz code inside a libgit2 update or a `Writer` child | the operation that meets `GIT_ELOCKED`, or the Git CLI route's own error | reported (below); never removed (OD-C3) |
| complete files, or one torn file after an exit that ran no gwz code, in a recorded handler's members | an exit inside a recorded run | the interrupted-operation record, read by every command that resolves the workspace | reported; `commit` and `stage` on its members refuse with `open_operation` naming the record; re-running the command finishes it or says what blocks it; or the user removes the record after inspecting |
| an open merge | an exit inside a merge | `gwz merge --status` (existing) | `--continue` or `--abort` |
| a `creating` or `disposing` local clone | an exit inside `local clone` or `dispose` | `gwz local list` (existing) | `gwz local dispose` |
| a server's socket and lock record | a killed server | 2.3f's stale-socket rule | existing |
| a push whose effect is unknown | `Force` or a kill after `upload_started`; an abandoned served push | the member's `possible remote effect` report (§3.2), or the forced notice | the user checks the remote |

- **The temp inventory** (shape, where `<id>` is `{prefix:016x}-{sequence:x}`, matched as `[0-9a-f]{16}-[0-9a-f]+`
  (`crates/ids/src/lib.rs:57`); writer; directories). Only a regular file by `lstat` whose name matches the exact
  pattern is reported; nothing outside these directories is listed.
  - `<name>.<id>.tmp`, the artifact durable write (`src/artifact/encoding.rs:47-56`): `gwz.conf/` (manifest, lock),
    `gwz.conf/snapshots/`, `gwz.conf/markers/` (`src/artifact/mod.rs:49-50`; `src/artifact/conf_integrity.rs:51`),
    `.gwz/` (`src/workspace_ops/url_scheme_state.rs:19`), `.gwz/stash/bundles/` (`src/stash/mod.rs:292-295`); and only
    for these target names: the root's `AGENTS_GWZ.md` and `AGENTS.md` (`src/workspace_ops/workspace_bootstrap.rs:39-40,
    :299, :305`), `.claude/settings.json` (`src/workspace_ops/workspace_bootstrap/claude_settings.rs:23, :125`) and
    `.git/info/exclude` (`src/workspace_ops/sync_workspace_boundary.rs:65, :121, :124-125`);
  - `<stem>.yaml.<id>.tmp` (`src/verified_write.rs:64`) and `<stem>.yaml.<id>.v1.tmp`
    (`src/workspace_ops/merge/v1_lifecycle/store/rewrite.rs:229`): `.gwz/merge/`
    (`src/workspace_ops/merge/store/mod.rs:32`), whose leftovers are operator cleanup by their owner's rule (§2),
    which a report serves;
  - `.<name>.tmp.<id>`, the family store (`crates/family-store/src/publish.rs:150-168`): the family root's `.gwz/`;
  - `.gwz/interrupted/` (records' temps, the first shape);
  - `.gwz-refcopy.<pid>.<n>.tmp` (`crates/refcopy/src/ordinary.rs:612-616`) lives only inside a lane being created,
    so the `creating` row covers it. (`gwz-memory-*.tmp`, `src/filesystem.rs:41-51`, is test-only.)
  A source check (step 3.0e) fails when a temp-name constructor or an atomic-write target directory is missing from
  the inventory.
- **The `GIT_ELOCKED` message** names the lock file and its age, says gwz cannot tell whether a process is using it,
  lists what to check first (no `git`, `gwz`, gwz server or IDE process working in that repository), and gives the
  removal command. It draws nothing from the workspace mutator lock: network ref updates take only the in-process
  member lock (`src/operation/memberlockmanager.rs:4-15`).
- **Transfers.** A cancelled fetch publishes no ref and leaves no pack (§3.2). A forced fetch can leave the `objects`
  row's residues and never a ref lock (`update_tips` is waited for). Pushes: §3.2.

### 3.6 The served form

- **Client interrupt (step 3.1).** `Cancel`: `operation.cancel` for each live call, then wait up to `close_wait` for
  the replies, which carry each operation's cleanup report (session design :183); render; exit 130. At the bound
  `Force` follows. `Force`: close the carrier (split doc :242, "the second disconnects"); close `ssh`'s standard
  input and kill it at once (parked :1222 amended for the forced exit only; :1223's 10 s wait after `session.close`
  stands); close the local stdio core's standard input and never signal it (E3); then `force_exit` (§3.3 step 2,
  from 3.0d). Each push call that reached no terminal is reported `possible remote effect` (§3.2).
- **The core notices its client.** `PeerGone`, or the stdio core's end of input, runs channel closure: running calls
  are cancelled, unstarted ones settled `Cancelled`, workers awaited up to `close_wait` and then detached (2.3g;
  `crates/session-host/src/service/workers.rs:213-236`). Handlers follow §5.3 exactly as in-process (§3.2).
- **The stdio core's end (E3, step 3.0d).** When its input ends it starts a watchdog thread. At `close_wait` the
  watchdog runs `force_exit` (§3.3 step 2 (b)-(f)): its own registered children are killed and it exits once every
  write sequence is parked or ended. At `close_wait` + 10 s, if it still runs (a write that never ends, such as a hung
  filesystem), the watchdog calls `_exit` (OD-C7 (e)). No other process signals it: after its client's death it is
  init's child, and a kill by PID from outside could reach a reused PID.
- **A started server's forced stop (step 3.0d).** The server's second stop signal and `gwz server stop --force` end
  the detached wait (2.3i) and then call `force_exit`. Since member-lock waits are parked (§3.3), it completes once
  each W run of every session is parked or ended. A write that never ends keeps it running, and a further signal
  ends it by the OS default (OD-C7 (a)).
- **The client notices its core.** `PeerGone` and `Stalled` fail every open exchange with `carrier_lost` (§2). A core
  that is alive but hung while nothing is queued is OD-C5's residual (split doc :237).
- **Spawns (step 2.3j).** The stdio child: `Stdio` kind, a new session, `CREATE_NEW_PROCESS_GROUP` on Windows. The
  `ssh` child: `Process` kind, SIGINT and SIGQUIT ignored, in the client's process group, its own job on Windows.
  The terminal's interrupt reaches only gwz; a hang-up also reaches `ssh`, which ends. Before `SessionOpened` the
  first interrupt kills `ssh` (parked :1219). 2.3j's spawn takes a `&ChildRegistry`; its rows make one.

### 3.7 gwz-py

- **The reaper.** The composition object of step 3.2 calls `child_registry()` once per process, before any host;
  until then the old entries use a reaperless registry (§3.4). The embedding constraints are in §3.4.
- **Every operation is cancellable.** Network or local, each operation gwz-py runs is admitted with a token and a
  `Canceller`, as §3.2's in-process path; a local one stops at its next gate crossing (§5.3) with an empty cleanup
  report. `cancel_operation` refuses only a wrong, foreign or completed operation (§8 row 15).
- **The first caller cancellation.** `_run_native`'s `CancelledError` path first cancels the operation through its
  `Canceller`, waits up to the cleanup bound for it to end (the bound `cancel_operation` takes,
  `gwz-py/native/src/client_host/operations.rs:199`), then re-raises. A unary call becomes cancellable as `submit`
  already is: the bridge holds a cancel handle that the native call registers before it starts (`ClientHost.call`
  gains it).
- **The second.** A further `CancelledError` while waiting kills the operation's registered `Group` and `Process`
  children at once, stops waiting and re-raises. The `to_thread` worker runs on to its next gate crossing or its
  terminal; a library caller's interpreter exit joins it (`cpython/concurrent/futures/thread.py:23-37`). This
  replaces `_await_completion` (`gwz-py/src/gwz/bridge.py:28-35`).
- **The native route** gets a token and a `Canceller` like the transport route, through §3.2's callbacks;
  `cancel_operation` on it stops returning `UnsupportedOperation` (§8 rows 14 and 15).
- **The CLI** installs no handler (OD-C1). The first SIGINT reaches the bridge as the first cancellation: on 3.11 and
  later the runner cancels the main task (`cpython/asyncio/runners.py:150-157`); on 3.10 `KeyboardInterrupt` leaves
  the loop and `asyncio.run`'s cleanup cancels the task. A second SIGINT raises `KeyboardInterrupt` (3.11 and later:
  the runner's handler; 3.10: inside the cleanup); on 3.11 and later the runner's close cancels the task again (the
  second path). The CLI never waits on the executor: its `run` coroutine catches the re-raised `CancelledError`, and
  `main` catches `KeyboardInterrupt`; either, when an operation of the process is still live, calls the gwz-py
  module's private `_force_exit(130)` (`force_exit`, §3.3 step 2 (b)-(f)); when none is, `main` writes the notice and
  returns 130. `_force_exit` is called only by `gwz-py/src/gwz/cli.py` and is not documented for library use.
  SIGTERM and SIGHUP keep Python's default, so the process dies: the reaper ends its children; a local write can tear
  (OD-C7 (d)).

## 4. Where it lands

**Phase 2** (2.3 plan; D-V resolution (3)). "Owns" is disjoint from every step in the same wave (plan :322), apart
from the 2.3 plan's one-line shared files (2.3 plan :244-246).

| Step | Content | Owns | Review | Size | After |
|---|---|---|---|---|---|
| **2.3r** (new; D-V (3)) | the registry, kinds, tokens, waiter threads, the SIGCHLD read, the Unix reaper and lifeline, the Windows jobs, the reaperless form; the minting site; `SpawnExclusion`; the spawn and minting checks; the helper (both arms), `rev-list` and the CLI route (`Writer`) onto it; tokio's `process` feature dropped | create `crates/children/`, `scripts/checks/check_child_spawns.py`; edit `scripts/checks/check_spawn_gate_minting.py`, `scripts/checks/process_globals_allowlist.json` (the crate's `libc::sigaction` and `libc::chdir`, kind `env` until 2.3s), `src/git/endpoint/https_auth/{process_tree,owner}.rs`, `src/git/endpoint/https_auth/process_tree/windows*`, `src/operation/commit_log/mod.rs`, `src/git/spawn_gate.rs`, `src/git/gitbackend/{backend,commit_tag}.rs`, `src/transport_host/{composition,local_command,cancellable,request}.rs`, `Cargo.toml` | dual (the helper's spawn) | ~500 + 500 | none; wave 2, lane 3, after 2.3e |
| 2.3i (amended) | takes `Interrupts`; defines gwz-core's interrupt type; installs no handler | as planned, plus create `src/interrupt.rs` and its `mod` line in `src/lib.rs` (a shared one-line file) | single | +~40 | as planned |
| 2.3j (amended) | `src/server/spawn.rs`: the stdio and `ssh` children through a `&ChildRegistry` (§3.6) | adds `src/server/spawn.rs` | dual, as planned | ~400 + 400 | 2.3g, 2.3r |
| **2.3s** (new; a choice, §6) | `gwz-cli/src/interrupt/` (§3.1): reset, install for the server family, restore; its allowlist entry; the `signal` kind; the dependency check | create `gwz-cli/src/interrupt/`, `scripts/checks/check_tokio_features.py`; edit `gwz-cli/src/lib.rs` (`run`), `gwz-cli/scripts/process_globals_allowlist.json`, `scripts/checks/check_process_globals.py`, `scripts/checks/process_globals_allowlist.json` (re-kind the crate's entries `signal`) | single (State) | ~250 + 250 | 2.3i, 2.3r; wave 4, lane 1 |
| 2.3k (amended) | `server start --foreground` and `server stdio` consume `Interrupts`; no client interrupt protocol | as planned | single | as planned | adds 2.3s; wave 4, lane 1 after 2.3s |

Waves (2.3 plan :236-242, three lanes): wave 2 lane 3 runs 2.3e then 2.3r, as wave 1's lane 3 ran b then c, so 2.3r
merges before wave 3 (and 2.3j) starts; wave 4 lane 1 runs 2.3s then 2.3k. 2.3g is unchanged: its carrier-loss row
needs only §5.3's crossings.

**Phase 3** (plan §4). Lanes own disjoint files; a file another lane owns is asked for, not edited (plan :329-331).
Review per plan §4 (:302-306): the 3.5 phase review, plus a per-step dual for a secret-handling step.

| Step | Content | Owns | Review | Size | After |
|---|---|---|---|---|---|
| **3.0d** (new, gwz-core) | `ExitPoints`, `WriteSequence` (network bracket, lock-wait park), `force_exit`; the dispatch opening W sequences; checkout, copy and stage exit points; the interrupted-operation record, its lock and recognizer; the commit and stage refusal; the stdio core's watchdog; the started server's forced stop | create `src/exit_points/`, `src/interrupted/`, `src/git/gitbackend/checkout_exit.rs`; edit `src/session_host/served/dispatch.rs` and the in-process dispatch, `src/operation/memberlockmanager.rs`, `src/git/gitbackend/{merge_prepared,merge_recovery}.rs`, the copy loops (`crates/{copy-contract,refcopy,family-store}`, `src/local_clone/`), the recording handlers (`src/workspace_ops/{handle_branch,pull_head*}.rs`, `src/stash/`, `src/workspace_ops/handle_materialize/`), `src/workspace_ops/{handle_commit,handle_stage}.rs`, `src/server/{stdio,shutdown}.rs`, the workspace-resolution hook; `src/git/gitbackend/{refs,repository}.rs` checkout and stage lines through the Phase 5 lane | phase | ~500 + 500 | none |
| **3.0e** (new, gwz-core) | the temp inventory and its report, the `objects` recognizer, the `GIT_ELOCKED` message, their source check | create `src/residue/`; edit `src/git/verify_checkout_state.rs` (`git_error`, :106), the resolution hook (one call) | phase | ~300 + 350 | 3.0d |
| **3.0c** (new, gwz-core) | native-transfer cancellation, `download` + `update_tips`, anonymous pushes, the clone split, the network bracket, the push phase, the fetch-time `objects` report call (§3.2) | edit `src/git/gitbackend/{transport,transport_support}.rs`; create `src/operation/push_phase.rs` | phase: it adds progress and negotiation callbacks beside the credential callback and changes no credential handling (a diff of that callback is empty) | ~300 + 300 | 3.0d, 3.0e |
| 3.1 (amended) | `run` calls `child_registry()` before 2.3s's install, which extends to `--server` commands; the client interrupt protocol over the carrier, whose `Force` ends in `force_exit` (D-V (3); §3.6) | as planned, plus `gwz-cli/src/lib.rs` (`run`) | phase | ~350 + 250 | adds 2.3s, 2.3j, 3.0d |
| **3.1c** (new, gwz-cli) | in-process `Cancel` and `Force`; the install for every command; the `Output` sink and its check; the forced output and notice per mode; the E2 rule; `hook` onto the registry; the spawn check over gwz-cli | create `gwz-cli/src/output/`; edit the 8 printing files, `gwz-cli/src/globalargs/render_exit.rs`, `gwz-cli/src/pager.rs`, `gwz-cli/src/forall.rs`, `gwz-cli/src/hook/` | phase | ~400 + 300 | 3.1, 3.0c |
| 3.2 (amended) | the composition object calls `child_registry()` | as planned | phase | +~30 | as planned |
| **3.2b** (new, gwz-py) | §3.7: bridge, CLI, every operation cancellable, native route `Canceller`, `_force_exit` | edit `gwz-py/src/gwz/{bridge,cli}.py`, `gwz-py/native/src/client_host/operations.rs`, `gwz-py/native/src/route/transport.rs` | phase | ~150 Rust + 150 Python | 3.2, 3.0c |
| 3.4 (amended) | adds the reaper's fork time and resident cost | | phase | | |

Graph: `{ 3.0 ∥ 3.0b ∥ 3.0d ∥ 3.2 } ; 3.0d → { 3.1 ∥ 3.0e } ; 3.0e → 3.0c ; { 3.1, 3.0c } → 3.1c ;
{ 3.2, 3.0c } → 3.2b ; { 3.1c, 3.2b } → 3.3 → 3.4 → 3.5`, at most three compiling lanes at once; 3.1 now follows
3.0d, so nothing runs in parallel on both sides of `force_exit` (plan :322-323); 3.3 removes the old entries and the
reaperless registry where no hostless backend remains. Under OD-C8 (a), a step **3.0f** (gwz-core, ~150 + 150, after
3.0c; **dual: secret handling**, since it changes where the native route's credential comes from) makes the native
route's credential callback call the operation's helper lookup through the registry instead of
`Cred::credential_helper`.

**Reviews:** plan §4 has one review per phase and a per-step dual only for a secret-handling or wire-format step
(plan :302-306); the 2.3 plan reviews each sub-step singly except a, d, j and l (:249), to which 2.3r is added (it
moves the helper's spawn). 2.3s is single. In Phase 3 only 3.0f is dual; every other step has the 3.5 review, whose
Surface reads §3.3's output and gwz-py's lifecycle. No step changes the wire.

## 5. Rows

Each row is seen red first unless marked **(reg)**, a mark that applies to the leg it follows. Platforms: M macOS,
L Linux, W Windows. Each row has one owning step, in brackets, whose predecessors build all it needs. "The harness"
is a test process that calls `child_registry()` before any thread exists.

1. [3.1c] SIGINT, SIGTERM and SIGHUP, each to the process group of a `gwz fetch` blocked in a sleeping recording
   helper: the helper's group is gone within the bound, exit 130, member error `cancelled`; on W, CTRL_C and
   CTRL_BREAK; SIGKILL to gwz during the lookup: the group is gone within 1 s (W **(reg)**: the job). (M, L, W)
2. [2.3r] SIGKILL to the harness during that lookup: the helper's group is gone within 1 s. (M, L; W **(reg)**)
3. [2.3r] SIGINT to the harness's group then SIGKILL to it; `kill -9 -<pgid>`; SIGQUIT to the group: each time the
   helper's group is gone within 1 s. (M, L)
4a. [2.3r] The harness under a pseudo-terminal that hangs up: the helper's group is gone within 1 s. On W, closing
   its console: the helper's and `rev-list`'s jobs end. (M, L, W)
4b. [3.1c] `gwz fetch` under a hung-up pseudo-terminal: it cancels, exits 130 (not 101), no panic, the helper is
   gone. On W, closing the console with a helper, `rev-list` and an `ssh` child running: the handler blocks until the
   forced exit ends; each job ends its process. (M, L, W)
5. [2.3r] The reaper killed alone: the next spawn is refused `ReaperGone` and starts nothing; a pre-exec registration
   write fails the spawn with `EPIPE` and the child dies of no SIGPIPE; a probe program spawned normally has the
   default mask and SIGPIPE disposition. The reaper holds only the lifeline's read end and gwz none (`lsof`); a lock
   the harness held is free once it exits while the reaper lives; under concurrent spawns no child holds the
   lifeline. (M, L)
6. [2.3r] A helper that succeeds and leaves a descendant in its group: it lives after the harness's normal exit and
   after its SIGKILL **(reg)**; the reaper's trace shows no kill of an unregistered group and each unregistration
   before its reap; SIGKILL just after `spawn` returns: gone within 1 s; a child whose reap a fault holds past its
   grace is retained with its slot, counted once in the cleanup report, and killed at the harness's exit; on L, the
   reaper's trace shows a pidfd per `Process` entry. (M, L; W **(reg)**: `retire` clears the limit)
7. [2.3r] A cancelled `rev-list` kills only `rev-list`; a full registry refuses with `capacity`; an injected fork
   failure leaves the registry refusing every spawn with `ReaperGone`, a lookup fails before its helper starts, and a
   command that starts no child runs; the spawn check fails on a `tokio::process` use, a `fork` and a spawn outside
   the crate. (M, L, W)
8a. [2.3s] The `interrupt` module in a test binary: the first byte is `Cancel`, the second `Force`; SIGQUIT is not
   caught; after `Force` is delivered, and after `Interrupts` is dropped, a SIGINT ends the process by default; a
   signal flood with no reader leaves the handler returning (`EAGAIN`) with `errno` unchanged; started with SIGHUP
   ignored, the module leaves it ignored and a SIGHUP does nothing, and the restore puts back `SIG_IGN`; started
   with SIGCHLD ignored, `run` resets it and a helper spawn succeeds. `gwz fetch` with a sleeping helper and SIGINT
   dies by the default action, as on 1.0.18 **(reg)**. (M, L; W CTRL_C, CTRL_BREAK, and CTRL_C disabled at start)
8b. [3.1c] A second SIGINT during a cleanup that does not finish: exit within 1 s, 130, no child left.
   `gwz fetch 2>&1 | tee log` with a hung helper and Ctrl-C to the pipeline: exit within the bound plus 1 s, 130; with
   standard output a full unread pipe, `Force` still exits; the print-macro check fails on a mutation. (M, L; W CTRL_C)
9. [3.0d] `force_exit` called by a harness while a large checkout is past its N-th file: exit at an exit point, no
   torn file, no `index.lock`, the record present; the next `gwz status` reports it; `gwz commit` on that member
   refuses with `open_operation` naming it; re-running the command finishes and removes it; a `gwz status` during a
   live recorded run in another process reports it running and refuses nothing; on W, a reader holding the record
   open in a loop while the owner completes: the owner's rename and removal succeed and no reader reports it
   interrupted. (M, L, W)
10. [3.0d] SIGKILL during a checkout: the next command reports the record. `force_exit` during `gwz local clone`'s
    copy: it stops at the next poll, and `gwz local dispose` accepts the `creating` row. (M, L, W)
11. [3.1c] A five-member `gwz commit` cancelled after its first member: every member committed; with the bound
    passing first, the same, and `Force` exits after it ends; the same with a CLI-route member whose `pre-commit` hook
    sleeps 3 s. A five-member `gwz pull` cancelled during member 3's fetch: members 1-2 updated, 3-5 untouched,
    `Partial` with 3-5 `cancelled`; re-running finishes. (M, L, W)
12. [3.0c] A native `file://` fetch cancelled during its pack, and over native SSH: no ref moved, no `*.lock`, no
    `pack_git2_*`. A native push cancelled before its upload: no effect; after its upload began (a delaying hook):
    `cancelled` with `possible remote effect` in `detail`. A trace shows `update_tips` and the tracking-ref update
    outside the network bracket. (M, L, W)
13. [3.0c] A stalled fetch ended by `force_exit` during `download` leaves a `pack_git2_*`: the member's next fetch
    reports it (path, age, command) once backdated past 1 h, does not report a younger one, and removes nothing.
    (M, L, W)
14. [3.0e] A dead temp of each inventoried shape in each inventoried directory, backdated: reported by the next
    command, never removed; `AGENTS.md.backup.tmp`, a symbolic link named like a gwz temp, another ID shape, a young
    temp and a file outside the inventory are not reported. (M, L, W)
15. [3.0e] A fetch held at a ref lock and a commit in another process meeting `GIT_ELOCKED`: the message names the
    path, its age, the checks and the platform's removal command, and makes no "not held" claim. (M, L, W)
16. [3.1] Served: SIGINT sends `operation.cancel` and receives the cleanup report; a local handler needing 10 s
    answers within `close_wait` and the command exits 130 with it; a second SIGINT closes the carrier and exits 130
    through `force_exit`; a SIGKILL of the client makes the core cancel (`PeerGone`) and release the member lock when
    the handler ends **(reg)**: 2.3g. (M, L, W)
17a. [2.3j] `ssh://` in the harness: `ssh` gets no terminal SIGINT; in a `| tee` pipeline with the harness SIGKILLed,
    `tee` lives and `ssh` is gone within 1 s; a prompting `ssh` under a pseudo-terminal reaches its prompt. (M, L, W)
17b. [3.1c] `--server stdio`: the client SIGKILLed while the core is inside a W write leaves no `*.lock`, no
    `pack_git2_*` and no torn file; the core logs a channel-closure cancel and exits within `close_wait` and one exit
    point; a core whose worker a fault holds in a write is ended by its watchdog at `close_wait` + 10 s; the client's
    registry close and a cancel of the client's operation never signal it. (M, L, W)
18a. [2.3s] Source check: the `signal` kind's occurrences are exactly gwz-cli's interrupt module and gwz-core's
    children crate, each one allowlist entry, and the crate's `libc::chdir` its one `env` entry; a mutation adding each
    listed spelling elsewhere in gwz-core, gwz-cli or gwz-py fails; the dependency check fails on tokio's `process`
    or `signal` feature in gwz-core, gwz-cli or the gwz-py module. (M, L, W)
18b. [2.3k] `gwz server start --foreground`: SIGINT runs the shutdown in order; a second ends the detached wait. (M, L, W)
19. [3.2b] gwz-py CLI, on Python 3.10, 3.11 and 3.13: SIGINT during a sleeping-helper lookup runs the `Canceller`,
    the helper's group is gone within the bound, exit 130; a second SIGINT during that wait exits 130 within 1 s with
    no child left. A gwz-py process SIGKILLed, or its CLI sent SIGHUP: the helper is gone within 1 s; a native-route
    fetch is cancelled during its pack; `cancel_operation` on a local operation cancels it at its next crossing.
    (M, L; W for the first and the last two)
20. [3.1c] A `forall` child that traps SIGINT, and a pager: gwz writes the waiting line, waits, and exits 130 after
    it ends; a second SIGINT during the wait leaves gwz waiting and `Force` runs when the child exits; a third ends gwz
    by default while the child runs on; on W, CTRL_CLOSE during the wait runs `Force` at once.
    `forall -- sleep 1000` with gwz SIGKILLed: `sleep` lives **(reg)**. (M, L, W)
21. [3.1c] `--json`: after the first interrupt, the document with `errors[0].code` `cancelled`, exit 130; after
    `Force` with nothing yet written, the minimal envelope parses; with a rendering blocked on a full pipe, `Force`
    writes no envelope and standard error says the output is incomplete; exit 130 in each. (M, L, W)
22. [3.4] The reaper's creation time, and its resident cost after the parent writes 100 MiB. (M, L)
23. [3.1c] Forced exits during pushes: a native push after its upload began (a delaying hook), a transport push after
    receive-pack began, and a served push abandoned by a second SIGINT each report `possible remote effect`: in JSON
    when nothing had been written, and in the forced notice when `--jsonl` had begun or in human mode. (M, L, W)
24. [2.3j] On W, `TerminateProcess` of the harness while a helper, a `rev-list` and an `ssh` child run: each job ends
    its process. (W)
25. [3.0f, only under OD-C8 (a)] A native-route lookup's helper is registered: SIGKILL of the harness ends it within
    1 s. (M, L; W **(reg)**)
26. [2.3r] A spawn of a program that does not exist, and of one without execute permission: refused; the reaper's
    trace shows the token's registration and unregistration; 300 such spawns leave the table's count at zero. (M, L, W)
27. [2.3r] The harness with SIGCHLD set to `SIG_IGN`, then with `SA_NOCLDWAIT`: `child_registry()` and each spawn
    refuse with `ChildReapingDisabled` before a child starts. (M, L)
28. [3.1c] `nohup gwz fetch` sent SIGHUP, and `gwz fetch` started under `trap '' INT` sent SIGINT: each completes,
    exit 0; on W, a gwz started with CTRL_C disabled ignores CTRL_C and cancels on CTRL_BREAK. (M, L, W)
29. [3.0d] A harness core with two sessions: session A's fetch parked in `download` holds member m; session B's
    `pull` waits for m; `force_exit` completes, and B's run writes nothing after it. (M, L, W)
30. [3.0d] `force_exit` during a large `gwz stage`: exit at the next path, the index unchanged, no `index.lock`.
    (M, L, W)
31. [2.3r] A Git CLI route commit in the harness whose `pre-commit` hook sleeps: its group is not the harness's;
    a SIGINT to the harness's group and a cancel of its operation leave it running to its end; SIGKILL of the
    harness: its group is gone within 1 s. (M, L; W: its job)

## 6. Decisions

- **OD-C1 to OD-C6 (the operator's, 2026-11-01).** OD-C1: one gwz-cli module owns the handler, installed once and
  allow-listed; gwz-core and gwz-py install none (§3.1). OD-C2 (a) in D-V (2)'s shape: the forked reaper shared by
  gwz-cli and gwz-py; jobs on Windows (§3.4). OD-C3 (a): a Git `*.lock` is reported, never removed (§3.5). OD-C4:
  exit 130 everywhere; JSON as §3.3. OD-C5 (a): no keepalive (§7 R8). OD-C6: gwz never kills the pager or `forall`'s
  commands and waits for the current one (E2).
- **D-V resolutions:** (1) §3.3, §3.5, §7 R1, R1a, R10; (2) §3.4, §3.7; (3) 2.3r and the client interrupt in 3.1, §4.
- **OD-C7 (open, for the operator): exits that can tear a write beyond D-V (1)'s SIGKILL, crash and power loss.**
  (a) an interrupt after `Force` began acts by its inherited disposition, by default ending gwz (the escape from a
  `Force` stuck in a write on a hung filesystem; else only SIGKILL ends it); (b) SIGQUIT keeps its default (core
  dump); (c) Windows ends a process about 5 s after CTRL_CLOSE, whatever it is writing (OS-imposed); (d) the gwz-py
  CLI dies by default on SIGTERM and SIGHUP (OD-C1 keeps handlers out of gwz-py); (e) the stdio core's watchdog
  `_exit` at `close_wait` + 10 s. Recommended: accept all five as R1b, each residue recognized by §3.5. Alternatives:
  (a) keep the handler through `Force`; (b) map SIGQUIT to `Force`; (d) let `gwz-py/src/gwz/cli.py`'s `main` (not the library)
  route SIGTERM and SIGHUP to SIGINT's path; (e) no second deadline. (c) has none.
- **OD-C8 (open, for the operator): two exceptions that narrow D-V's "every helper process it started".** (a) E6:
  the native route's helpers run outside the registry, so a SIGKILL of gwz, or a `Force` from a signal sent to gwz
  alone, leaves a waiting one running; recommended: remove E6 with step 3.0f (§4), which changes native credential
  lookup to the transport's helper path; alternative: accept E6 as on 1.0.18. (b) E3: the local stdio core outlives
  its client by at most `close_wait` + 10 s, because killing it mid-write is the C4 defect Safety P2-4 found;
  recommended: accept.
- **Choices this revision makes inside them, open to review (not operator decisions):**
  - §5.3 unchanged, and the pull/materialize stop between members (§3.2); the served first-interrupt bound is
    `close_wait`; no new `AggregateStatus` or error code;
  - residues reported, never removed, by the command that resolves the workspace or fetches, and by the forced
    notice (§3.5); `open_operation` refuses commit and stage on a recorded member; the record's liveness lock; the 1 h
    report age;
  - the registry holds 256; the reaperless interim registries; the stdio watchdog's 10 s; the reaper's pidfd on Linux
    for `Process` entries, a form of D-V (2)'s "calls `kill()`" (`pidfd_send_signal`) with one `pidfd_open` per
    registration;
  - inherited ignored signals stay ignored, and the restore puts back what gwz inherited; a second interrupt during
    an E2 wait makes `Force` pending; gwz-cli resets an inherited ignored SIGCHLD; the registry refuses spawns while
    SIGCHLD is ignored;
  - the Git CLI route's child is a `Writer` in its own group, waited for by `Cancel` and `Force` (R11);
  - placement: **2.3r** in wave 2 lane 3 is D-V (3)'s sub-step; **2.3s** in wave 4 lane 1 is this revision's choice,
    not an operator decision: 2.3i installs no handler (OD-C1) and 2.3k's foreground server needs a producer of
    `Interrupts`, so OD-C1's module lands in Phase 2 with only the server family installing it; the new Phase 3 steps
    3.0c, 3.0d, 3.0e, 3.1c, 3.2b (and 3.0f under OD-C8 (a)), and 3.1 after 3.0d.

## 7. Residuals

- **R1** SIGKILL, a crash or power loss can tear the file being written and leave a Git `*.lock`, including a
  `Writer` child's; §3.5 names a recognizer for each residue. Accepted by the operator (D-V resolution (1)).
- **R1a** A `Force` that ends a `download` at once, as D-V (1) has it end network work, leaves that transfer's
  libgit2 temporaries (§3.5's `objects` row). gwz never removes them (no cross-process exclusion covers fetch); the
  forced notice names them at once and the member's next fetch reports them after 1 h, with the removal command.
  Attributed to D-V (1)'s own rule; listed so round 2 can judge it.
- **R1b** The OD-C7 triggers (a) to (e), with R1's recognizers; open until OD-C7 is decided.
- **R2** A native push's upload cannot be aborted (§2): `Cancel` waits for it, `Force` exits during it and reports a
  possible remote effect. Lifting it needs a change to the gwz-git2 fork, which this amendment does not propose.
- **R3** A native transfer's connect, negotiation and credential phases run no callback; the libgit2 timeout clock
  and C5 bound them. A libgit2 checkout cannot be aborted; `Force` parks it between two files.
- **R4** A process or group ID reused within the reaper's reaction time, after that group emptied and init reaped its
  leader; for a `Process` entry on macOS, and on Linux without `pidfd_open`, after the process exited and init reaped
  it (the `ssh` child exits at its input's end of file as gwz dies); after a foreign reaper reaped a leader and before
  gwz's waiter unregistered it; and after std reaped a child whose `exec` failed and before the registry wrote its
  unregistration.
- **R5** E1 to E6 (§3.4); E3 and E6 pending OD-C8.
- **R6** In gwz-py, an embedding application's fork without exec, or its own thread spawning on macOS while the
  composition object makes the lifeline, delays the reaper's end of file until that process exits.
- **R7** The reaper's copy-on-write cost, up to the parent's size at the fork; larger in a gwz-py process that
  creates its composition object late.
- **R8** A core that hangs while nothing is queued is not detected without a keepalive (OD-C5; split doc :237).
- **R9** After the reaper dies alone, the children already running are not covered against a SIGKILL of gwz until
  they end; new spawns are refused. Before step 3.1 (gwz-cli) and 3.2 (gwz-py), registries are reaperless, so a
  SIGKILL of gwz leaves a helper as today; so does a hostless backend's registry until 3.3 finds none remain.
- **R10** `Force` waits without a bound for a long local phase that has no callback: `stash push`'s working-tree
  reset and a Git CLI route child with its hooks (§3.3). An interrupt after `Force` began ends gwz by default
  (OD-C7 (a)).
- **R11** A Git CLI route child leads its own group, so a hook or signing program that reads the terminal itself is
  stopped (SIGTTIN) until the bound; on 1.0.18 it shared gwz's group and received the terminal's interrupt.

## 8. Clauses of other documents this amends (applied by the lane owner at acceptance)

| # | Document and clause | Amendment |
|---|---|---|
| 1 | TR1.6 §3.2 "Its cost" (:76) and OQ2 (:398) | Append to each: "Withdrawn 2026-10-31 by D-V: Ctrl-C cancels the operation; the child registry and its reaper or job end the group (GwzCancellationAmendment-DRAFT.md §3.4)." |
| 2 | Split doc §4.3 "SIGINT/SIGTERM" (:239) | Append: "The signals reach the server as the `Interrupts` value gwz-cli's handler produces; gwz-core installs no handler (cancellation amendment §3.1)." |
| 3 | Split doc §4.3 "Client interrupts" (:242) | Replace with: "first Ctrl-C sends `operation.cancel` and waits up to the session's close bound (`close_wait`); the second disconnects (server design §7); the command exits 130 (cancellation amendment §3.3, §3.6)." |
| 4 | Split doc §4.4 `--server stdio` (:246) and header | :246 append: "When its input ends the core runs channel closure, its forced exit at `close_wait` and a watchdog exit at `close_wait` + 10 s (cancellation amendment §3.6)." Header: "Amended 2026-11-01 by the cancellation amendment: §4.3 SIGINT/SIGTERM and Client interrupts; §4.4 stdio." |
| 5 | 2.3 plan, 2.3i (:186-187) "SIGINT/SIGTERM and Windows Ctrl-Break through a handler that sets a flag only"; shared files (:244-246) | Replace with: "the first and second events of the `Interrupts` value the command passes (gwz-core's interrupt type, created here in `src/interrupt.rs`); gwz-core installs no handler (cancellation amendment §3.1)." Size ~520 + 400. Shared files: "`transport_host/mod.rs` and `lib.rs` (one export each in g and k; one `mod` line in i)". |
| 6 | 2.3 plan, 2.3j (:194-205) | Add `server/spawn.rs` (the stdio and `ssh` children through a `&ChildRegistry`, cancellation amendment §3.4, §3.6); "After: 2.3g" becomes "After: 2.3g, 2.3r"; size ~400 + 400; red rows 17a and 24 of the amendment. |
| 7 | 2.3 plan, 2.3k (:207-218) and the waves (:236-242) | 2.3k: "`start --foreground` and `stdio` consume `Interrupts`"; "After" adds 2.3s. New paragraphs 2.3r and 2.3s from §4. Wave 2, lane 3: "2.3e then 2.3r (two steps, one lane)"; wave 4, lane 1: "2.3s then 2.3k". |
| 8 | Plan §4 (:315, :721, :729, :739) and §10 (:1047) | The Phase 3 graph of §4 above; 3.1 adds `child_registry()` in `run`, the client interrupt protocol, exit status 130, and "After: 3.0d"; 3.2 adds the registry's creation; new steps 3.0c, 3.0d, 3.0e, 3.1c, 3.2b (and 3.0f, dual, under OD-C8 (a)) with §4's sizes and files; 3.4 adds the reaper measurement; :1047 records the acceptance and OD-C7, OD-C8. |
| 9 | Session design §16 (:687) | Replace with: "Process exit does not join a detached worker. A forced exit waits until each local write sequence is parked or ended; only an exit that runs no gwz code can tear a write, and the next command recognizes each residue (cancellation amendment §3.3, §3.5)." |
| 10 | Session design §5.8 (:375) and §15 row 15 (:679); its header | :375: "Cancelling a running network operation waits for it to finish, except during a fetch's pack transfer, which libgit2's progress callbacks abort; a native push's upload is not aborted." Row 15: "Cancelling a running fetch during its pack transfer ends it `Cancelled`; in its other phases it returns after the phase. `transport_capabilities` still reports no cancellation support." Header: "Amended 2026-11-01 by the cancellation amendment for §5.8, §15 and §16; §5.3 unchanged." |
| 11 | Contract §6 (:289) and §15 | Composition/CleanupDomain row adds: "the child registry, the kill authority for every child process; a retained child stays pending cleanup and is counted once." A §15 row records it. |
| 12 | `gwz-cli/docs/MachineOutput.md:954-964` and "Notes" (:280-287) | Exit table adds "an interrupted command: 130, on every platform, whatever its aggregate"; a paragraph gives §3.3's output per mode, the forced notice, the incomplete-output rule, and that residue reports and interrupted-operation records are one standard-error line in every mode (the Notes rule does not apply to them); `commit` and `stage` refuse a recorded member with `open_operation` whose `detail` names the record. |
| 13 | gwz-cli root help (`gwz-cli/src/help.rs:25-32`); `gwz-cli/docs/CLI.md` regenerated | Add: "Ctrl-C cancels and waits; a second Ctrl-C forces the exit; exit status 130. A command interrupted while it rewrites files leaves a record that later commands report: re-run it." |
| 14 | `gwz-py/README.md:225` | Replace with: "Cancelling a running native operation aborts a fetch's pack transfer; its other phases end at their own bounds." Add the fork-without-exec, foreign-reaping and ignored-SIGCHLD paragraphs (§3.4). |
| 15 | gwz-py design §2.5 (`gwz-py/dev-docs/GwzPyPerOperationTransportDesign.md:101-106`) | Replace :106 with: "**A wrong, foreign or completed operation** fails without cancelling anything; the record of completed cancellations stays bounded. **A local (non-network) operation** is cancelled at its next gate crossing (session design §5.3), with an empty cleanup report." Add: "A caller's cancellation of an awaiting call cancels its operation first, then waits up to the cleanup bound; a second stops waiting. The native route is cancellable during a fetch's pack. The CLI force-exits on a second interrupt (cancellation amendment §3.7)." |
| 16 | Parked :584 "The extension handles `SIGTERM` and `SIGINT` itself." | Superseded: gwz-py installs no handler (OD-C1); the embedding application delivers interrupts. |
| 17 | Parked :1222 | Append: "On a forced exit (cancellation amendment §3.3) the client closes `ssh`'s standard input and kills it at once." |
| 18 | `gwz-core/docs/ErrorCatalog.md:35` (`cancelled`) and its table | `cancelled` remedy adds: "A `detail` that begins `possible remote effect:` marks a push cancelled after its upload began." New table row: "`open_operation` (41) \| An open gwz merge, a held family lock (:117, :189), or an interrupted-operation record naming the member \| Finish or abort the merge; for a record, re-run the recorded command or remove the record after inspecting its members." |
| 19 | 2.3 plan, 2.3i red row (:188) and evidence (:191) | Red row: "a second `Interrupts` event (`Force`) or `force` ends the detached wait (a test producer of the value)"; evidence: "dabeest (pipe); the signal rows are 2.3s's". |
| 20 | 2.3 plan review list and totals (:249-250) | "Review: dual on a, d, j, l and r"; "Totals: about 5,250 implementation lines and 5,600 rows across 14 steps". |
| 21 | `gwz-core/docs/WorkspaceArtifacts.md` paths (:9-21) and "Atomic Writes" (:191-198) | Paths: "`.gwz/interrupted/<operation id>.yaml` \| local only \| An interrupted-operation record: a run that a forced exit or a kill can stop between files; held under its own lock while the run lives; on Windows opened with delete sharing." Atomic Writes adds: "A temp an interrupted write left is reported by later commands with its path, never removed by gwz." |

## 9. What this does not change

The contract's §5 cancellation semantics (cancel, abandon, host close); the session design's §5.3, `operation.cancel`
and channel closure; the carrier's end reasons and bounds; the helper's own process group and job; TR1.6 §3.3's
completion rule (E1); the frozen wire, which gains no status, code or message. It adds the interrupt that drives
them, the reaper and jobs that keep them true when gwz itself is gone, and the exit points and recognizers that
bound what a forced exit leaves.

## 10. Changes from revision 1

Where revision 2 closes each round-1 finding (C- Consistency, S- Safety; `=` joins a blind convergence) and each
fix-round gap. Dispositions and closure tests are in the remediation plan.
- **Architectural (D-V):** C-P2-1 → §4 (Owns and Review columns, waves, graph), §5 (one owner per row), §8 rows 6-8,
  19-20; C-P2-5 = S-P2-7 → C4, §3.3 (write sequences, exit points, long local phases), §3.5, §7 R1-R1b, R10, §8 row 9;
  C-P2-6 = S-P2-5 → §2, §3.4, §3.7, §4, §5 row 19, §8 rows 14-16.
- **The interrupt:** C-P2-2 = S-P3-5 → §3.1, §4, §8 rows 2, 5, 19; S-P2-6 → §3.1, §3.3 (`Output` sink, envelope
  rule), OD-C7; C-P3-5 → §3.1; S-P3-3 → §3.1, §3.4, rows 4b, 24.
- **Bounds and output:** C-P2-3 → C5, §3.6, §8 row 3; C-P2-10 → §3.3, §8 rows 12, 13, 18, 21; C-P3-7 = S-P3-4 → C5, E2.
- **Local work:** C-P2-4 → §3.2 (two handler kinds), §3.3 (records and their lock), row 11.
- **Children:** C-P2-7 = S-P2-1 → §3.4 (read end closed, pre-exec SIGPIPE, fork failure), §7 R9, rows 5, 7;
  C-P2-8 = S-P2-2 → C2, §3.4 (registration, foreign reaping, E1-E6), OD-C8; S-P2-3 → §3.4 (kinds, `ssh`'s children);
  S-P2-4 → §3.4 (`Stdio` off the lifeline), §3.6 (watchdog), OD-C8 (b); S-P3-1 → §3.4 (reaperless interim), §7 R6;
  C-P3-3 = S-P3-2 → removed; C-P3-8 → §3.4, §8 row 11, row 6.
- **Leftover state:** C-P2-9 = S-P2-10, S-P2-8, S-P2-9 and C-P3-2 → §2, §3.5 (report only, inventory); S-P2-11 and
  C-P3-6 → §3.2 (split calls, served pushes), §7 R2-R3, §8 row 10, row 23.
- **Text:** C-P3-1 → the citation tuple and path rule, every citation re-read; C-P3-4 → §5; C-P3-9 → §4.
- **Fix round 1:** G1 → §5 rows 4a-b, 8a-b, 9, 12, 17a-b, 18a-b, 23; G2 → §4 waves, §8 rows 7, 19, 20; G3 → §4 Owns
  and graph; G4 → §3.3 write sequences, rows 11, 13; G5 → C4, §3.1, OD-C7, §7 R1b; G6 → OD-C8, §4 3.0f; G7 → §2,
  §3.1, §3.4 (no `tokio::process`); G8 → §3.4 `Stdio` row; G9 → §3.4 (reaperless interim, fork before the interrupt
  thread); G10 → §3.2; G11 → §3.5 `objects` row; G12 → §3.5 inventory; G13 → §3.3 `Output` sink; G14 → §8 rows 12,
  13, 18, 21; G15 → rows 4b, 6, 7, 24 and §3.4 `ssh`'s children.
- **Fix round 2:** GAP-1 → row 13 [3.0c]; GAP-2 → rows 7, 18a; GAP-3 → §4 3.1 After, graph; GAP-4 → §6 choices;
  GAP-5 → §3.2 push phase, §3.3 forced notice, row 23; GAP-6 → §3.3 exit points and long local phases, R10, rows 30;
  GAP-7 → §4 Review column, 3.0f dual; GAP-8 → §3.1 Enforced, §4 2.3r/2.3s Owns, row 18a; GAP-9 → §2 spawn gate,
  §3.4 spawn gate and `Writer`; GAP-10 → rows 6, 16, 20 **(reg)** legs; GAP-11 → §4 2.3i Owns, §8 row 5; GAP-12 =
  S-R2-4 → C5, E2, row 20; GAP-13 → C6, §3.1 Windows; GAP-14 → §3.7, §8 row 15, row 19; GAP-15 → C4, §7 R1a;
  GAP-17 → header path rule, every path; S-R2-1 → §3.4 registration, R4, row 26; S-R2-2 → §3.1, §3.4 never-zombie
  children, row 27, §8 row 14; S-R2-3 → C1, §3.1, rows 8a, 28; S-R2-5 → §3.4 kinds table "Killed by", §3.3 step 1,
  row 17b; S-R2-6 → §3.1 "Where `run` calls it", row 8a; S-R2-7 → §3.3 lock-wait park, §3.6 forced stop, row 29;
  S-R2-8 → §3.4 reaper pidfd, R4, row 6; S-R2-9 → §3.3 records, row 9, §8 row 21; S-R2-10 → C4, §3.3 forced notice,
  §3.5 table; S-R2-11 → §3.4 registration; F1 (found in this round: commit and tag can start a child) → §2, §3.2,
  §3.3, §3.4 `Writer`, R10, R11, row 31.
