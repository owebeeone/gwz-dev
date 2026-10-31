# Ctrl-C in the CLI: design A of the cancellation split (DRAFT)

Status: **DRAFT, revision 1, 2026-11-01; design A of the cancellation split (D-V); for dual review. Authorizes no
implementation.** No commit, tag, push or publish; the lane owner applies §8 at acceptance. Fix pass 1 closed A-1 to A-17.

**Authority.** `GwzTransportReleaseConvergencePlan.md` §10 D-V (cited "D-V", plan :999-1056): OQ2 reversed (:1005-1010);
OD-C1 to OD-C6 as recommended (:1030-1031; their text in `history/GwzCancellationAmendment-DRAFT.md` revision 1, root
`9d5a4d4c`, §6); resolutions (1) to (3) (:1035-1045); the split into designs A to D (:1046-1055). OD-C7 is open (r2 §6,
`history/GwzCancellationAmendment-DRAFT.md:695-702`); §9 puts A's part of it to the operator. Revision 2, its RemPlan and
its reviews in `history/` are source material, not authority. Design B is `GwzChildLifetimeDesign-DRAFT.md` ("B").

**Citations** are at root `b4b827d6`, gwz-core `b745f1fa`, gwz-cli `c0ba717`, gwz-py `2185cc6`, gwz-git `9bc4ea9`,
git2-rs `d13951f`, libgit2 `b172e3d18`, CPython 3.11.16. Member-named paths are workspace-relative; `src/…`, `crates/…`,
`scripts/…` are gwz-core's; bare documents are in the root's `dev-docs/`; a bare `:N` is a line of the file last named.
Operating-system facts no file holds are marked **(OS)**; a row (§6) verifies each.

## 0. Scope

What an interrupt does to a gwz-cli command: the one interrupt source (OD-C1); the first interrupt, which cancels the
command's request through the cancellable entry's token and waits up to the cleanup bound; the second interrupt or the
bound, which forces the exit under D-V resolution (1)'s write rule, with file-granular safe points in the long local
phases; exit 130 and the `--json` marking (OD-C4) in gwz-cli and the gwz-py CLI; what a cancelled member reports,
including a helper whose cleanup outlasts its grace; the pager and `forall` rule (OD-C6); and gwz-py's first caller
cancellation through the `Canceller` (resolution (2)). §7 names every residual A leaves.

| Not in A | Owner |
|---|---|
| The child registry, the reaper, process kinds, which children the terminal's interrupt reaches, `kill_killable` and `close` | B |
| The served form: the server lifecycle commands' interrupt (2.3i), what Cancel and Force do for a `--server` command and its bound (3.1), `server stdio`, `ssh` | C |
| Temp sweeps, Git `*.lock` reporting, a push's possible remote effect in JSON, durable recognizers, the accepted residual list | D |
| The native route's cancellation (libgit2 progress-callback abort), gwz-py's second interrupt | not designed; §7 R10, R11 |

## 1. Requirements (each traced to D-V)

- **A1 One source.** One gwz-cli module owns the process's signal and console handlers; gwz-core and gwz-py install
  none (OD-C1 as decided). A check enforces the one site. §9 O1 puts the brief's second site (2.3i) to the operator.
- **A2 First interrupt cancels.** It cancels the command's network work through its existing cancellation, and the
  command exits once that has ended or the cleanup bound has passed (OQ2 reversed; OD-C4).
- **A3 Forced exit.** A second interrupt, the bound passing, or a console close forces the exit: killable children and
  network work end at once, a local write in progress runs to its next safe point, and long local phases have
  file-granular safe points (resolution (1)). A child that performs the write itself is part of the write (§3.4; §9 O2).
- **A4 Status and marking.** An interrupted command exits 130 on every platform and its JSON marks it cancelled
  (OD-C4), in gwz-cli and in the gwz-py CLI.
- **A5 Honest member reports.** A member whose operation was cancelled reports `Cancelled`, also when its helper's
  cleanup is still pending (D-V "cleans up"; `build-targets/winflk-logs/f9-report.md`, "Findings" 2).
- **A6 Foreground programs.** gwz never kills the pager or a `forall` command; it waits and starts no further one (OD-C6).
- **A7 gwz-py.** A caller's cancellation of an awaiting call cancels through its `Canceller` first (resolution (2)).
- **A8 Code rules.** Braces; `cfg` only in `cfg_if!` or platform modules; no global or thread-local except the
  handler's pipe write end (OD-C1); no production code under `tests/`; one behaviour on macOS, Linux and Windows except
  where §3 names a platform's mechanism.

## 2. Facts this design builds on

- **No handler today.** No production source in gwz-cli, gwz-core, gwz-transport, gwz-py or gwz-git names
  `libc::signal`, `libc::sigaction`, `SetConsoleCtrlHandler`, `ctrlc` or `tokio::signal` (searched at the heads above);
  `run` goes from `worker_host::early` to parsing (`gwz-cli/src/lib.rs:160-178`), so Ctrl-C ends gwz by the OS default.
  `scripts/checks/check_process_globals.py` lists `signal` and `sigaction` in `LIBC_FUNCS` (:103-104), classed `env`
  (:540-541), run per repository with `--repo` and `--allowlist` (:810-811).
- **The command's path.** The transport build runs a network command through the non-cancellable
  `with_local_transport_native*` (`gwz-cli/src/globalargs/dispatch.rs:57-89`), others on a hostless `Git2Backend::new()`
  (:91-96); a response is one `println!` then `exit(exit_code_for_response)` (`gwz-cli/src/lib.rs:323-326`), an
  execution error one `println!`/`eprintln!` then exit 1 (:340-344); `diff`, `hook`, `claude-code-setup` and `log` own
  their lifecycles (:228-300). Step 3.1 builds one `HostOwner` per command (plan :721-723).
- **The cancellable entry** (`src/transport_host/cancellable.rs:40-88`) runs the action under one request registered
  with a `CancellationToken`: a token cancelled first refuses with `Cancelled`, cancelling it fails the running
  request's network I/O with `Cancelled`, and it is "the request's only cancellation authority" (:10-11, :145-147, :160,
  :178). Token and canceller come from `CallControls::new` and `cancel` (`crates/session-host/src/gate.rs:140, :156`),
  as gwz-py's route makes them (`gwz-py/native/src/route/transport.rs:150-152`). `HostOwner` close seals admission and
  cancels the host's operations (`GwzConnectionApiContract.md:269`); `ConnectionHost::close` takes the owner
  (`src/transport_host/connection_host.rs:438-449`).
- **Host-built backends.** The transport host builds each request's backend with `Git2Backend::new().with_host_context(..)`
  (`src/transport_host/mod.rs:504`; `driver_only.rs:152`), which copies the context's spawn gate into it
  (`src/git/gitbackend/backend.rs:120-125`, field :26); the runtime's state holds that gate and each request's context
  takes it (`mod.rs:269, :463-467`; `request.rs:31-32`). gwz-core's other production backends do local work (for
  example `src/workspace_ops/handle_create_repo/create_workspace.rs:18`; `src/git/endpoint/https_worker/native/sspi.rs:77`).
- **Members fan out** through `par_map_per_host` (`src/operation/par_map_per_host.rs:35`; e.g. `handle_fetch.rs:154`,
  `push_member.rs:337`); `commit` runs its members in one loop (`src/workspace_ops/handle_commit.rs:179-186`), then the root (:260).
- **Network calls of a backend** (`src/git/gitbackend/transport.rs`): clone (`builder.clone` :67, after the empty-target
  check :32), `fetch` (:110), `tag_fetch` (:150), `ls_remote*` (`connect_auth` :223, `list` :235), `fetch_anonymous`
  (:340), `push_anonymous` (:379), `perform_push` (:493-494), `read_remote_file` (a `tempfile::tempdir` :718, cloned into
  at :740). `Remote::fetch` is libgit2's download, disconnect, then `update_tips` (`libgit2/src/libgit2/remote.c:1352,
  :1391, :1394, :1409`); git2-rs exposes the three (`git2-rs/src/remote.rs:248, :265, :341`).
- **Long local phases.** libgit2 checkout calls its progress callback after each complete file
  (`libgit2/src/libgit2/checkout.c:1886-1891`) and writes the index once at the end (:2686). gwz's checkouts:
  `src/git/gitbackend/refs.rs:33, :134`, `repository.rs:56, :81`, `merge_prepared.rs:264`, `merge_recovery.rs:166, :246`,
  the clone (`transport.rs:41`). `reset_hard` passes no checkout options (`repository.rs:31`; pull's
  `src/workspace_ops/pull_head_plan.rs:182`, `pull_head_member_preflight/root_lock.rs:166`); libgit2 checks the tree out,
  then moves HEAD, then writes the index (`libgit2/src/libgit2/reset.c:154, :159, :167`); `reset` takes a
  `CheckoutBuilder` (`git2-rs/src/repo.rs:782-787`). `stash_apply`/`stash_pop` (`stash.rs:44, :59`) use
  `stash_restore_options` (`stash_support.rs:18-26`), which sets none; `StashApplyOptions::checkout_options` takes one
  (`git2-rs/src/stash.rs:117`). Stage is one `add_all` with no callback then one index write (`repository.rs:108-115`;
  the callback may abort, `git2-rs/src/index.rs:49-51`), or a per-path loop then one write (:151-169). `stash push` is
  one `stash_save` (`stash.rs:18`); commit and tag may run `git commit`/`git tag` with hooks (`commit_tag.rs:1-5, :31`;
  `gwz-git/src/native_commit/cli.rs:36-38`), B's `Wait` kind (B §3.6).
- **Copy loops.** `gwz_copy_contract`'s `Cancellation` is polled per directory entry and between buffered chunks of a
  file (`crates/copy-contract/src/lib.rs:39-52`; `crates/refcopy/src/ordinary.rs:321, :648`); refcopy writes each file
  to a sibling temporary, renames it into place and removes the temporary on any failure, a cancel included
  (`ordinary.rs:14-17, :477`). `gwz_local_import`'s own trait is polled before each participant
  (`crates/local-import/src/lib.rs:337-339, :529, :583`). `handle_local.rs:72` and `src/local_clone/family_merge.rs:424`
  pass each trait's `NeverCancelled`.
- **The cleanup-grace finding.** `finish_job` answers a failure with `job.terminate().await?`
  (`src/git/endpoint/https_auth/runner.rs:213-228`); `terminate` waits `CLEANUP_GRACE` (500 ms, `https_auth.rs:28`), then
  returns `CleanupPending` (`owner.rs:253-276`), mapped to `ErrorCode::Io` (`https_auth.rs:190`), so `Cancelled` and a
  `Timeout`'s `SetupFailureCause::Interaction` (`src/git/endpoint/https_worker.rs:377`) are lost. The file worker
  returns `CleanupPending` itself (`https_auth/file_worker.rs:91-98`). B's 2.3r-2 makes `terminate` return the
  lookup's own outcome (B §3.6).
- **Output and codes.** Exit codes are 0, 1, 2 from the aggregate (`gwz-cli/src/globalargs/render_exit.rs:82-95`;
  `gwz-cli/docs/MachineOutput.md:954-964`). Codes render as `{:?}` (`gwz-cli/src/git_status_json.rs:71`), so
  `cancelled` (73, `gwz-core/docs/ErrorCatalog.md:35`, "A result's first error") is `"Cancelled"`. `errors` holds copies
  of failed members' errors first, then the operation's own (`MachineOutput.md:141-142`); a top-level error has
  `meta: null`, no members, one error (:67-69); a `--jsonl` stream ends with one response object (:669-670).
  `eprintln!`/`println!` panic when the write fails (std `print_to`), and Rust ignores SIGPIPE.
- **Foreground programs.** The pager is spawned through the shell and waited for after its input closes
  (`gwz-cli/src/pager.rs:138-145, :155-170`); `forall` runs members with `Command::status` (`gwz-cli/src/forall.rs:203-217, :260`).
- **gwz-py.** `_run_native` runs the call on `asyncio.to_thread` and on `CancelledError` only waits
  (`gwz-py/src/gwz/bridge.py:298-316`; `_await_completion` :28-35). A unary network call registers as `op_<request_id>`
  with the route's canceller (`gwz-py/native/src/client_host.rs:226-231`; `shims.rs:79-80`); `cancel_operation` cancels
  a running one and waits up to `CLEANUP_BOUND` (`client_host.rs:164-171`; `client_host/operations.rs:194-227`),
  refusing a local or completed one (`InvalidRequest`, :202-208) and the native route (`UnsupportedOperation`,
  :216-222). The CLI's `main` catches only `CliUsageError` and `GwzError` around `asyncio.run`
  (`gwz-py/src/gwz/cli.py:142-156`); CPython 3.11's runner cancels the main task on the first SIGINT and raises
  `KeyboardInterrupt` on the next (`asyncio/runners.py:150-157`). gwz-py already names a type `ExitGate`
  (`client_host/operations.rs:55`), so A's gate is `WriteGate`.

## 3. Design

### 3.1 The interrupt source (OD-C1)

- **Module.** `gwz-cli/src/interrupt/`: `mod.rs` (reader, writer, `Interrupts`, `OutputClaim`), `unix.rs`, `windows.rs`
  under one `cfg_if!`. Its one allow-listed global is the pipe's write end; everything else is a value `run` passes down.
- **Install.** `run` installs right after parsing (`gwz-cli/src/lib.rs:178`), before any gwz thread or child, for every
  command except the server lifecycle commands (`server start`, the background server, `server stdio`; 2.3k; C, §9 O1),
  `hook` and `claude-code-setup` (OS default, §7 R12); `--build-info` and the SSPI worker mode return before parsing.
  The client commands `server stop`, `status` and `list` take it.
- **Unix.** For SIGINT, SIGTERM and SIGHUP, `install` saves the inherited disposition and leaves an ignored one ignored
  (`nohup`, a background job); otherwise `sigaction` with `SA_RESTART` and a handler that saves `errno`, writes one byte
  to the pipe (both ends close-on-exec, write end non-blocking, `EAGAIN` ignored), restores `errno` and returns.
  SIGQUIT keeps its default (OD-C7 (b) as recommended; §9 O3).
- **Windows.** `SetConsoleCtrlHandler`. CTRL_C and CTRL_BREAK write a count byte and return `TRUE`. CTRL_CLOSE writes a
  Force byte and sleeps without end: Force's `TerminateProcess` ends the process, or the OS does a short time after
  CTRL_CLOSE **(OS)** (§7 R9). The handler needs no other object. A CTRL_C disabled at creation stays disabled.
- **Reader and writer.** A module thread reads the pipe: the first count byte is **Cancel**, the second (or a Force
  byte) **Force**; after Cancel it waits for the next byte or the bound's deadline. It never writes output: a second
  module thread writes all of it (§3.2's line, §3.6's notice and envelope) with `write_all`, ignoring every error.
- **Giving control back (OD-C7 (a) as recommended; §9 O3).** When Force begins, the reader restores each saved
  disposition (Windows: removes its handler), so a later interrupt ends gwz by default (killed by the signal;
  `STATUS_CONTROL_C_EXIT`): the escape from a Force waiting on a hung write (§7 R14).
- **Check.** `check_process_globals.py` gains the kind `signal` (`libc::signal`, `libc::sigaction`,
  `SetConsoleCtrlHandler`, `signal_hook`, `ctrlc`, `tokio::signal`), run for gwz-core, gwz-cli, gwz-py and
  gwz-transport; only gwz-cli's allowlist has an entry (this module). B's forked-child entries are B's (B §3.7).

### 3.2 First interrupt: Cancel

- **What it cancels.** 3.1c runs the network command through the cancellable entry's gated form (3.0d) in place of
  `with_local_transport_native*` (`dispatch.rs:66-79`), with the process environment as snapshot and a token from
  `CallControls::new` (under 3.1's host, the command's request takes the same token and gate). Cancel calls the
  controls' `cancel`; the command is one request, so every member's network I/O then fails `Cancelled`. A does not close
  the `HostOwner` from the reader: the token is the request's only cancellation authority, the entry closes the host
  at its end, and gwz-core gives the CLI no per-admission hook.
- **`--server` commands.** The module is installed (one source); what Cancel and Force do and the bound are C's (§4).
- **Local work** has nothing to cancel and runs on (session design §5.3). In a multi-member W command whose members
  fetch (pull, materialize), earlier members are whole and later ones report `Cancelled` (§7 R7).
- **Wait.** Cancel starts the bound, `CLEANUP_BOUND` (5 s, `src/session_host/context.rs:80`): if the command returns
  first, the main thread renders (§3.6) and exits 130; otherwise Force begins. Human output gets one writer line at
  Cancel: `gwz: interrupted; cancelling (interrupt again to stop now)`.

### 3.3 What a cancelled member reports

- A member whose operation observed the cancel is `Failed` with `Cancelled`; one that failed first keeps its reason.
- **The cause survives a slow cleanup.** With B's `terminate` (§2), `finish_job`'s `?` (`runner.rs:226`) passes the
  lookup's own `Cancelled` or `Timeout` up; 3.0e makes the file worker return `runner.check()`'s cause after its grace
  instead of `CleanupPending` (`file_worker.rs:91-98`), kept only for a failure with no other cause. So a cancel on a
  loaded Windows host is `Cancelled`, and a timeout keeps `Interaction` (`https_worker.rs:377`).
- **The pending cleanup is a finding, not the member's error:** the retained tree is counted (`owner.rs:77, :163`) in
  the report's `pending_local_work` (row 6.7; 3.0e adds it if missing), which 3.1c prints through the writer, not
  `dispatch.rs:82-87`'s `eprintln!`.

### 3.4 Second interrupt or the bound: Force and the write rule

**The write gate** (`crates/write-gate/`, a new gwz-core crate that builds and tests alone). `WriteGate` is a cloneable
value the CLI makes per process and passes down; its state is a `ThreadId` map under one mutex and condition variable.

| Call | Meaning |
|---|---|
| `writing(label, f)` | the current thread runs `f` as a write sequence (nesting allowed) |
| `outside(label, f)` | the thread's sequence is parked while `f` runs (network, lock wait, join); if the gate closed meanwhile, the thread blocks for good when `f` returns |
| `safe_point(label)` | if the gate is closed, the writing thread records `label` as stopped and blocks for good |
| `is_closed()`, `close()`, `wait()` | `wait` returns, with the stopped labels, once every writing thread is parked, stopped or ended |

**How the gate reaches every backend that does gwz's network.** `Git2Backend` gets a `write_gate` field beside
`spawn_gate` (`backend.rs:26`, open by default) and `with_write_gate`. The runtime's state holds the gate as it holds
the spawn gate, each request's context takes it, and `with_host_context` copies it (`backend.rs:120-125`), so the
transport host's per-request backends (`mod.rs:504`, `driver_only.rs:152`) carry it. The gated entry passes it to the
runtime; the hostless dispatch backend (`dispatch.rs:91-96`) takes it through `with_write_gate`. gwz-core's own local
backends (§2) carry an open gate: they have no safe point and run to their end inside the command's sequence (R8).

**Where sequences open and park.**
- 3.1c's dispatch runs every request except an explicit read-only list as `writing`; the list is an exhaustive `match`
  over `CliRequest` with no wildcard, so a new request is a writer until it is classified.
- `par_map_per_host` runs its join as `outside` and each worker as `writing` when the caller was writing.
- Every network call of §2's list runs as `outside`. Fetch sites call `download`, `disconnect`, then `update_tips`
  (with the tag option and prune `Remote::fetch` uses) instead of `fetch`, so only the download is parked and the ref
  update is a write. A push, a clone and `read_remote_file` are parked whole (§7 R5, R6, R16).
- The member-lock wait (`src/operation/memberlockmanager.rs`) and family-lock poll (`src/local_clone/wait.rs`) are `outside`.

**Force**, on the reader thread, in this order: (1) restore the dispositions (§3.1); cancel the token if Cancel did not;
(2) claim the output (§3.6) and hand the writer the notice, and the forced envelope if `--json` won the claim;
(3) `close()` the gate; (4) B's `kill_killable()`, which ends every `Kill` child at once; (5) `wait()` and hand the
writer the stopped labels; (6) B's `close()`; (7) give the writer up to 1 s; (8) exit 130 without unwinding (`_exit` on
Unix, `TerminateProcess` on the current process on Windows). A blocked pipe never holds Force past step 7. **Network
work ends at once:** network calls are parked, so step 5 never waits for one, and step 8 ends them.

- **A local write runs to its next safe point or its end.** A sequence still running after step 3 (the commit loop of
  `handle_commit.rs:179-186`, for one) may start B's `Wait` children (`git commit`, `git tag` and their hooks) until it
  ends: those children perform the write, so they are part of it, never killed (B OD-B1 (a)) and waited for without a
  bound (R8). This narrows D-V (1)'s "kills children … at once" to the `Kill` children (§9 O2). A `Kill` child a parked
  network call starts after step 4 is ended by B's reaper when the process exits (B §3.4).

### 3.5 Long local phases and their safe points

| Phase | Safe point | What a stop leaves |
|---|---|---|
| libgit2 checkout: switch, pull's update, merge, materialize at the seven checkout sites of §2; `reset_hard` (a `CheckoutBuilder` passed to `reset`); `stash apply` and `stash pop` (`checkout_options` in `stash_restore_options`) | the checkout progress callback, after each complete file | earlier files new, later files old, index and HEAD old (§7 R1) |
| copy: `local clone` (`handle_local.rs:72`), family import (`family_merge.rs:424`) | one adapter type implementing both `Cancellation` traits, answering `is_closed()`: refcopy stops between entries or mid-file and removes that file's temporary; local-import stops before the next participant | whole files only; a `creating` lane or partial import (§7 R2) |
| stage, `add_all` | its matched-path callback (today `None`, `repository.rs:112`) | loose objects only; the index is unchanged (§7 R3) |
| stage, per-path loop | each iteration (`repository.rs:151-168`) | the same |
| `stash push` (`stash_save`), Git CLI route commit or tag with hooks | none: runs to its end | nothing; Force waits without a bound (§7 R8) |

A stopped copy's thread runs its error path to the sequence's end. Every other write sequence writes a bounded set
per member (refs, one index, one commit, atomic workspace artifacts) and runs to its end.

### 3.6 Exit status and output (OD-C4)

- **Status.** An interrupted command exits **130** on every platform, over 0, 1 and 2 and in place of the OS's signal
  death or `STATUS_CONTROL_C_EXIT`: the response and execution-error paths, `diff`, `log` and `forall`.
- **The claim.** `OutputClaim` (one atomic, owned by `run`) is taken once: by the main thread before its final stdout
  write, or by Force. The main thread's final write after an interrupt uses `write_all` and ignores its error. A main
  thread that loses the claim writes nothing and waits for Force's exit.
- **After Cancel** (the command returned). The response renders as usual, with the first own entry of `errors` (after
  the member copies, before any other own error; §8 rows 7, 8) `{"code":"Cancelled","message":"interrupted",
  "member_id":null,"member_path":null,"target_kind":null,"detail":null,"record_context":null}`; `aggregate_status` as
  today, nothing added to the wire. An execution error's top-level `errors` is [that entry, its own error].
- **After Force.** `--json`: if Force won the claim, the writer writes exactly the top-level shape with `meta: null`, no
  members and the one `Cancelled` entry whose `message` is `interrupted; forced exit before the command finished`;
  otherwise stdout holds what the main thread wrote. `--jsonl`, human and porcelain: Force writes nothing to stdout, so
  a `--jsonl` stream has no final response object. A consumer reads exit 130 with no complete document as forced.
- **The forced notice** (the writer, stderr, every mode): `gwz: forced exit; waiting for local writes to reach a safe
  point`, then per label: `gwz: stopped between files: <member path> (<phase>)` or `gwz: transfer ended: <member path>
  (fetch|push|clone)`, a push adding `; the remote may have received it`.

### 3.7 The pager and `forall` (OD-C6)

A foreground program (the pager after its input closes, a `forall` member's `status`) gets the terminal's interrupt
itself; gwz never signals it. `Interrupts::foreground(f)` wraps that wait: no bound runs and a Force is deferred until it
returns. On Cancel, `diff` and `log` close the pager's input and wait; `forall` finishes the current member, starts no
further one, appends `interrupted: <k> of <m> members not run` and exits 130.

### 3.8 gwz-py (resolution (2) and OD-C4)

- **The bridge.** `ClientHost.call` passes `_run_native` the operation id (one exported native function over
  `shims::operation_id`, `shims.rs:79-80`). On the first `CancelledError`, `_run_native` calls `cancel_operation(id)`
  on its own thread, shielded, ignoring its refusal, then waits through `_await_completion` as today and re-raises; a
  running network operation ends `Cancelled` within the cleanup bound. `submit` is unchanged; gwz-py installs no handler.
- **The gwz-py CLI.** Its first SIGINT reaches the bridge through `asyncio.run` (`runners.py:150-157`; 3.10, row 6.16).
  `main` (`cli.py:142-156`) catches `KeyboardInterrupt` beside its error clause, renders
  `GwzBridgeError("interrupted", code="Cancelled")` as that clause does (stdout with `--json`), and returns **130**.
  SIGTERM, SIGHUP, CTRL_BREAK and CTRL_CLOSE keep Python's default (§7 R15; §9 O3).

## 4. Interfaces with B, C and D

- **B** provides `kill_killable()` and `close()` (B §3.7), which Force calls at steps 4 and 6, and the `Wait` kind for
  the Git CLI route's write child (B OD-B1). B changes no disposition in gwz's process. B's 2.3r-2 makes `terminate`
  return the lookup's own outcome; A states the member report from it (§3.3).
- **C** owns the server lifecycle commands' interrupt (2.3i's handler: §9 O1) and, for a `--server` command, what
  Cancel and Force send over the carrier and their bound (the session close bound, not `CLEANUP_BOUND`), taking A's
  `Interrupts` value; A fixes nothing of the served form.
- **D** gives each §7 row its recognizer, carries a push's possible remote effect into JSON (round-1 Safety P2-11; R5),
  and puts the "beyond D-V (1)" rows to the operator.

## 5. Steps

Phase 3, after Phases 1 and 2 (plan :315); phase-reviewed (3.5); no secrets or wire. Sizes: implementation + rows.
3.0d edits the trunk's `transport_host/mod.rs` (plan :325) after the trunk merges, else asks it; `refs.rs` and
`repository.rs` go through the Phase 5 lane (:329-331).

| Step | Content | Owns | Size | After |
|---|---|---|---|---|
| **3.0d** (new, gwz-core) | the `write-gate` crate; the backend field, the runtime and request carriage, the gated cancellable entry (§3.4); `par_map_per_host`; the `outside` brackets and fetch split; lock waits; the `signal` kind | create `crates/write-gate/`; edit `Cargo.toml`, `src/git/gitbackend/{backend,transport}.rs`, `src/transport_host/{mod,request,cancellable}.rs`, `src/operation/{par_map_per_host,memberlockmanager}.rs`, `src/local_clone/wait.rs`, `scripts/checks/check_process_globals.py` | ~480 + 450 | Phase 1 |
| **3.0e** (new, gwz-core) | the file worker's cause; the pending count in the report (§3.3); rows over B's `terminate` | edit `src/git/endpoint/https_auth/file_worker.rs`, the runtime's shutdown report if row 6.7 needs it | ~30 + 200 | 2.3r-2 |
| **3.0f** (new, gwz-core) | the safe points of §3.5 | edit `src/git/gitbackend/{merge_prepared,merge_recovery,stash,stash_support}.rs`, `src/workspace_ops/handle_local.rs`, `src/local_clone/family_merge.rs`; `refs.rs`, `repository.rs` through Phase 5 | ~200 + 380 | 3.0d |
| **3.1c** (new, gwz-cli) | §3.1; the token; Cancel, bound, Force; writer, claim, marking, notice, exit 130; the read-only list; pager and `forall` | create `gwz-cli/src/interrupt/`; edit `src/lib.rs`, `src/globalargs/dispatch.rs`, `src/pager.rs`, `src/forall.rs`, `src/diff_exec.rs`, `src/log_exec.rs`, `Cargo.toml` (`libc` on Unix, `windows-sys` on Windows), `scripts/process_globals_allowlist.json` | ~470 + 450 | 3.1, 3.0d, 3.0e, B's 3.1 amendment |
| **3.2b** (new, gwz-py) | §3.8 | edit `gwz-py/src/gwz/bridge.py`, `gwz-py/src/gwz/cli.py`, `gwz-py/native/src/client_host.rs` (the exported function) | ~70 + 180 | 3.2 |

Graph: `{3.0 ∥ 3.0b ∥ 3.0d ∥ 3.0e ∥ 3.1 ∥ 3.2} ; 3.0d → 3.0f ; {3.1, 3.0d, 3.0e} → 3.1c ; 3.2 → 3.2b ;
{3.0f, 3.1c, 3.2b} → 3.3`, at most three compiling lanes at once. 3.0d settles `WriteGate` before 3.1c starts.

## 6. Rows

Each is written and seen red first; "red by" says how. M, L, W: macOS, Linux, Windows.

- **6.1** [3.0d; M L W] `wait` returns at once with no writer, waits for an unparked writer, returns when it reaches
  `safe_point` (which never returns) and does not wait for one inside `outside`, which blocks on return. Red: no crate.
- **6.2** [3.0d; M L W] A writing caller of `par_map_per_host`: the gate waits for a worker in `update_tips` and not for
  one in `download`. Red: join not parked.
- **6.3** [3.0d; M L W] The split fetch writes the same refs, tags, `FETCH_HEAD` and prunes as `Remote::fetch` on the
  existing fetch fixtures. Red: by mutation (prune dropped).
- **6.4** [3.0d; M L W] A child process forced during a fixture's slow `download` through the transport host's
  per-request backend (`mod.rs:504`) and through the hostless backend exits within 1 s; its refs are unchanged. Red:
  the host-built backend has no gate.
- **6.5** [3.0d; any] `check_process_globals.py` fails a `sigaction` or `SetConsoleCtrlHandler` occurrence outside
  gwz-cli's module in each of the four repositories' runs. Red: no `signal` kind.
- **6.6** [3.0e; W, and M L with the f9 slow-drain fixture] A cancel whose tree outlasts `CLEANUP_GRACE` returns
  `Cancelled` from both the helper and the file worker; the slot comes back at the reap within 30 s. Red: `CleanupPending`.
- **6.7** [3.0e; M L W] The same through the HTTPS worker: the member error is `Cancelled`, a timeout keeps
  `Interaction`, and `pending_local_work` counts the retained tree until the reap. Red: `Io`.
- **6.8** [3.0f; M L W] Force at file k of an N-file checkout, of `reset_hard` and of `stash apply` (in a child
  process): each file is wholly old or wholly new, index and HEAD are unchanged, the notice names the member and the
  phase. Red: no callback.
- **6.9** [3.0f; M L W] Force during `local clone`'s copy of a file larger than one chunk: every destination file is
  complete and no temporary remains; Force during a family import stops before a participant; `stage` (both forms)
  leaves the index unchanged. Red: `NeverCancelled`.
- **6.10** [3.1c; M L, W by `GenerateConsoleCtrlEvent` in a child console] SIGINT, SIGTERM, SIGHUP and CTRL_C,
  CTRL_BREAK on `gwz --json fetch` against a stalled fixture: exit 130 within the bound; the member is `Failed` with
  `Cancelled`; the `Cancelled` entry is the first own entry of `errors`. Red: dies by signal.
- **6.11** [3.1c; M L] An inherited ignored SIGINT (`trap '' INT`) or SIGHUP (`nohup`) stays ignored; SIGQUIT keeps
  its default. Red by mutation (install ignoring the inherited disposition).
- **6.12** [3.1c; M L W] A second interrupt during a stalled download: exit 130 within 1 s, `transfer ended`; `--json`
  stdout is exactly the forced envelope, `--jsonl` has no response object, human stdout is empty; with stdout a pipe
  nobody reads and stderr closed, exit 130 within 2 s, no panic. Red: no Force.
- **6.13** [3.1c; M L W] A three-member `commit` whose hook sleeps 8 s, one interrupt: the bound passes, Force waits,
  every member and the root are committed (later `git commit`s start after Force began), exit 130; an interrupt during
  that wait ends gwz by the default (SIGINT; W: `STATUS_CONTROL_C_EXIT`). Red: no gate in the dispatch.
- **6.14** [3.1c; M L W] Interrupt after every member succeeded: aggregate `Ok`, exit 130, the entry. Red: exit 0.
- **6.15** [3.1c; M L W] A `forall` member command running past the bound still runs after it, no further member
  starts, a second interrupt does not end gwz first, exit 130; `diff` waits for its pager without a bound. Red: no
  foreground rule. W: CTRL_CLOSE runs Force before the OS ends the process (§3.1's **(OS)** fact).
- **6.16** [3.2b; M L W; 3.10 and 3.11+] Cancelling the task awaiting a transport-route fetch calls `cancel_operation`
  before waiting; the call ends `Cancelled` within the cleanup bound; a local and a native-route call wait to their end
  and raise nothing new; the exported id equals the registered one; the gwz-py CLI's `--json fetch` on SIGINT exits 130
  with code `Cancelled`. Red: `_run_native` only waits; the CLI dies by SIGINT.

## 7. Residuals (for design D)

"Beyond D-V (1)": not covered by D-V (1)'s accepted SIGKILL, crash and power loss; D puts it to the operator.

| # | What can remain | When | How it is recognized now | Beyond D-V (1) |
|---|---|---|---|---|
| R1 | a partly checked-out tree, index and HEAD old | Force at a checkout safe point (checkout, `reset_hard`, stash apply or pop) | the notice line; D | no (file-granular stop) |
| R2 | a `creating` lane or partial family import (whole files only) | Force at a copy safe point | the notice line; D | no |
| R3 | unreachable loose objects | Force during stage | none needed (Git's `gc`) | no |
| R4 | libgit2's temporary pack files in `objects/pack` | Force during a fetch download (freed only on libgit2's error path) | `transfer ended`; D | no (network ends at once) |
| R5 | an unknown remote effect; a tracking-ref `.lock` of a named remote | Cancel or Force during a push | the notice's `may have received it`; the catalog's remedy (`ErrorCatalog.md:35`); D carries it into JSON | yes (the `.lock`) |
| R6 | a partial new member directory | Force during a clone | the notice; the next clone refuses a non-empty target (`transport.rs:32`); D | no |
| R7 | earlier members updated, later ones not | Cancel during pull or materialize | the response (`Partial`, members `Cancelled`); re-running completes | yes (reading of "no partial state") |
| R8 | none; gwz waits without a bound | Force during `stash push`, a `Wait` child and its hooks, a gwz-core local backend's write, or a write blocked in the filesystem | the notice's first line; R14's escape | yes (`Wait` children are not killed at once, §9 O2) |
| R9 | the residues of SIGKILL | Windows CTRL_CLOSE when Force outlasts the OS's timeout **(OS)** | as SIGKILL's; D | yes (OD-C7 (c)) |
| R10 | a native-route network call runs on after Cancel | transport off or an ordinary build: no token | Force at the bound ends it (parked); gwz-py's cancel refuses | yes |
| R11 | a gwz-py local or native-route call runs to its end; a second interrupt keeps waiting | gwz-py | `_await_completion` (unchanged) | yes |
| R12 | the OS default: gwz dies by the signal | `hook`, `claude-code-setup`; the server lifecycle commands until C | as SIGKILL's; D | yes |
| R13 | a truncated stdout document | Force while the main thread writes | exit 130 without a complete document (§3.6) | no |
| R14 | a write torn by the default disposition | an interrupt after Force began; SIGQUIT at any time | death by signal (or a core file) instead of 130; D | yes (OD-C7 (a), (b)) |
| R15 | a gwz-py CLI write torn, no cancel | SIGTERM, SIGHUP; Windows CTRL_BREAK, CTRL_CLOSE | death by signal; D | yes (OD-C7 (d)) |
| R16 | a `tempfile` directory holding a partial clone in the system temp directory | Force during `read_remote_file` (`transport.rs:718, :740`; `_exit` skips its `Drop`) | D's temp sweep | no (network ends at once) |
| R17 | exit 101 with a panic message instead of 130; the panicking thread unwinds | a gwz-cli `eprintln!`/`println!` outside the module fails (closed stderr, `2>&1 \| tee` ended by the same Ctrl-C) after an interrupt | the exit status; D | yes (an induced crash) |

## 8. Superseded or amended clauses (applied by the lane owner at acceptance)

| # | Document and clause | Replacement or addition |
|---|---|---|
| 1 | Plan §4 graph, Phase 3 line (:315) | `Phase 3  ─ {3.0 ∥ 3.0b ∥ 3.0d ∥ 3.0e ∥ 3.1 ∥ 3.2} ; 3.0d → 3.0f ; {3.1, 3.0d, 3.0e} → 3.1c ; 3.2 → 3.2b ; {3.0f, 3.1c, 3.2b} → 3.3 → 3.4 → 3.5 (review)   (after Phases 1 and 2)` |
| 2 | Plan §4 Phase 3 heading (:712) | "(three lanes then one, 19–23 lane-days)" for "(two lanes then one, 11–13 lane-days)" |
| 3 | Plan §4, after the 3.0b bullet (:717-720) | Five bullets 3.0d, 3.0e, 3.0f, 3.1c, 3.2b with §5's content, sizes and "After", citing `GwzCancelInterruptDesign-DRAFT.md` §5 |
| 4 | `gwz-cli/docs/MachineOutput.md` Exit Codes (:954-964) | Add row "interrupted (Ctrl-C, SIGINT, SIGTERM, SIGHUP; Windows Ctrl-C, Ctrl-Break, console close) \| `130`, whatever the aggregate" and a paragraph "Interrupted commands" with §3.6's entry, its place, the forced envelope and the `--jsonl` rule |
| 5 | MachineOutput Notes (:280-287) | Append: "The interrupt and forced-exit notices are written to stderr in every output mode." |
| 6 | MachineOutput JSONL (:669-670) "exactly one final `kind: \"response\"` object" | Append: "unless the command was forced to exit after an interrupt (exit 130), which ends the stream without one." |
| 7 | MachineOutput (:141-142) "Any other entry in `errors` is an error the operation reports in its own right." | Append: "After an interrupt the first of these is the `Cancelled` entry with no member." |
| 8 | `gwz-core/docs/ErrorCatalog.md:35` (`cancelled`), "A result's first error;" | "A result's first error of its own (in a response's `errors`, after the copies of member errors);" and append to the remedy: "An interrupted gwz-cli or gwz-py CLI command adds one `cancelled` entry with no member and exits 130." |
| 9 | `gwz-py/dev-docs/GwzPyPerOperationTransportDesign.md` §2.5 (:103-106) | Add: "**A caller's cancellation of an awaiting call** first cancels its operation through `cancel_operation`, ignoring its refusal, then waits as before; the CLI exits 130 on an interrupt (cancellation design A §3.8)." |
| 10 | `gwz-py/README.md:225` | Append: "Cancelling the task that awaits a transport-route call cancels its operation first." |

The helper-owner contract's `CleanupPending` clauses (C5) are history (`gwz-core/dev-docs/history/reference/`) and stay
unedited (AgentProcessRules L1-33); the live statement is the code of §3.3, whose doc comments 3.0e and 2.3r-2 update.

## 9. Decisions and points for review

For the operator (A is written to the option marked; only the named rows change with the answer):
- **O1 OD-C1 and 2.3i.** The brief allows 2.3i's "handler that sets a flag only" (2.3 plan :186-187) as a second site;
  OD-C1 as decided says gwz-core installs none (round-1 Consistency P2-2). **A follows OD-C1**: one site; C makes the
  server lifecycle take A's module. Alternative: the operator records 2.3i's handler in OD-C1 and C adds its entry.
- **O2 `Wait` children under Force.** D-V (1) kills children at once; A and B leave the Git CLI route's `git commit`/
  `git tag` and hooks running, and a running sequence may start more, so no commit is torn (B OD-B1 (a); R8). **A
  follows B.** Alternative: kill them at Force step 4, tearing that member's commit (an R1-like residue with a `.lock`).
- **O3 OD-C7, A's part.** (a) dispositions restored when Force begins, so the next interrupt ends gwz (R14; round-1
  Safety P2-6); (b) SIGQUIT keeps its default (R14); (c) CTRL_CLOSE's OS timeout (R9); (d) the gwz-py CLI keeps
  Python's default for SIGTERM, SIGHUP, CTRL_BREAK, CTRL_CLOSE (R15). **A accepts each, as recommended.** Alternatives:
  (a) keep the handler through Force; (b) map SIGQUIT to Force; (d) `cli.py` routes them to SIGINT's path.

For review:
- **Q1.** The bound counts from Cancel even while a write sequence without a safe point runs (row 6.13): such a command
  completes its writes but ends with the forced notice instead of its rendering.
