# Ctrl-C in the CLI: design A of the cancellation split (DRAFT)

Status: **DRAFT, revision 2 (2026-11-01); design A of the cancellation split (D-V), remediated per
`GwzCancelInterruptDesign-RemPlan.md`; for round-2 review by fresh reviewers. Authorizes no implementation.** No commit,
tag, push or publish; the lane owner applies §8 at acceptance.

**Authority.** `GwzTransportReleaseConvergencePlan.md` §10 D-V (plan :999-1062): OQ2 reversed (:1005-1010); OD-C1 to
OD-C6 (:1030-1031; text in `history/GwzCancellationAmendment-DRAFT.md` revision 1, root `9d5a4d4c`, §6, OD-C7 :695-702);
resolutions (1)-(3) (:1035-1045); the split (:1046-1054); round 1's decisions (:1055-1062): **(i)** Force never exits
mid-write, **(ii)** `git commit`/`git tag`, hooks and signers finish, **(iii)** O1, O3 / OD-C7 (a)-(d) as recommended.
"B" is `GwzChildLifetimeDesign-DRAFT.md` (revision 1, root `d146bc9a`; §4 follows revision 2's §3.7, :224-231).

**Citations** are at root `48feeaad`, gwz-core `b745f1fa`, gwz-cli `c0ba717`, gwz-py `2185cc6`, gwz-git `9bc4ea9`,
gwz-transport `281a28d`, git2-rs `d13951f`, libgit2 `b172e3d18`, CPython 3.10.15 and 3.11.16. Member-named paths are
workspace-relative, `src/…`, `crates/…`, `scripts/…` gwz-core's, bare documents the root's `dev-docs/`; a bare `:N`
is a line of the file last named; **(OS)** marks an operating-system fact no file holds, which a row checks.

## 0. Scope

What an interrupt does to a gwz-cli command and to gwz-py (§3.8); §7 names every residual. Child lifetime is B's; the
served form (2.3i, 2.3k, a served core's response) C's; temp sweeps, `*.lock` reports, recognizers and residual list D's.

## 1. Requirements (each traced to D-V)

- **A1 One source** (OD-C1; O1). One gwz-cli module owns the signal and console handlers, the server lifecycle's too;
  gwz-core and gwz-py install none: the process-globals check (Rust) and 3.2b's test (gwz-py's Python, §3.8) enforce it.
- **A2 Cancel** (OQ2; OD-C4). The first interrupt cancels network work; gwz exits when it returns or the bound passes.
- **A3 Force** (decision (i)). A second interrupt or the bound kills `Kill` children and ends network work at once, then
  waits, unbounded, for each local write in progress to reach its next safe point or finish: Force never exits mid-write.
  Safe points are file-granular, never in a held Git lock file (OQ2: "no … stale lock"); only SIGKILL ends a hung write.
- **A4 Commit and tag** (decision (ii)). A Git CLI route `git commit`/`git tag`, hooks and signer are never killed.
- **A5 Status and marking** (OD-C4). 130 and a JSON `Cancelled` marking in both CLIs; no output denies a write it made.
- **A6 Honest member reports** (D-V "cleans up"; `/Volumes/projects/limbo/build-targets/winflk-logs/f9-report.md`,
  "Findings outside scope" 2): `Cancelled`, also when the helper's cleanup is still pending.
- **A7 Foreground programs** (OD-C6). gwz never kills the pager or a `forall` command; it waits and starts no further one.
- **A8 gwz-py** (resolution (2)). A caller's cancel goes through the canceller first, also before the operation registers.
- **A9 Code rules.** Braces; `cfg` only in `cfg_if!` or platform modules; no global or thread-local but the handler's
  (OD-C1); no production code under `tests/`; one behaviour on three platforms unless §3 names a mechanism.

## 2. Facts this design builds on

- **No handler today.** No production source of the five repositories names `libc::signal`, `libc::sigaction`,
  `SetConsoleCtrlHandler`, `ctrlc` or `tokio::signal`; `run` parses at `gwz-cli/src/lib.rs:170-178`. The check
  (`scripts/checks/check_process_globals.py`) classes `signal`, `sigaction` as `env` (:103-104, :540-541) and runs for
  gwz-core (`scripts/run_tests.py:552`), gwz-transport (:316-327), gwz-git (:472-479), gwz-cli
  (`gwz-cli/.github/workflows/ci.yml:26-31`) and gwz-py (`gwz-py/src/tests/test_process_globals.py:11-23`).
- **The host.** Network commands run on `with_local_transport_native*` (`gwz-cli/src/globalargs/dispatch.rs:57-89`);
  plan 3.1 (:721-723) builds one `HostOwner` per command, local or `--server`; 3.3 removes the old entries (:736-738).
  `HostOwner::close` seals admission at the call (`Closing(HostClosing)`), cancels the host's operations and returns a
  `HostClose` future, resolved within the close bound (5 s, `host/config.rs:22, :36`), `Pending` after it
  (`gwz-transport/crates/gwz-connection/src/host.rs:6-10`, `host/owner.rs:21-32, :50-59`, `operation.rs:343-344,
  :414-417`; `GwzConnectionApiContract.md:269, :272-275`); no gwz-core code maps `HostClosing` yet. `ProcessGate` is
  minted only in `src/transport_host/composition.rs` (:21-35; "the only site", :30-31).
- **Fan-out.** `par_map_per_host` is public (`src/operation/par_map_per_host.rs:35-47`); `_with_cancel` gives queued items
  `on_cancel` without entering `f` (:58-60). Its seven calls (`handle_fetch.rs:154`, `push_member.rs:337`,
  `publication.rs:245`, `handle_materialize/apply.rs:43`, `member_validation.rs:33, :49`, `handle_init_from_sources.rs:147`)
  hold `backend: &B`, `B: GitRepository` (`src/git/gitbackend/contract.rs:42`). `commit` loops members serially
  (`src/workspace_ops/handle_commit.rs:179-186`); other thread starts are endpoints' and history readers' (`merge.rs:189`).
- **libgit2's network calls write locally.** A fetch download commits the pack (index through a filebuf, a rename) after
  the stream (`libgit2/src/libgit2/transports/smart_protocol.c:782`), then writes `.git/shallow` (`fetch.c:218-223`); the
  transfer callback fires after each received object (`indexer.c:874`); an uncommitted pack is unlinked when freed
  (`indexer.c:1469`). A clone inits, calls `remote_create` (gwz's: `src/git/gitbackend/transport.rs:55-59`), fetches and
  checks out; gwz then pins `core.autocrlf`/`core.eol` (:73), creation time only (`repository_support.rs:30-35`); on error
  libgit2 removes what it made (`clone.c:666-673`). A push calls `push_update_reference` after the remote's report
  (`remote.c:3032-3038`), before the tracking-ref update (:3064-3067); gwz sets it (`transport_support.rs:130`;
  `transport.rs:368`), and the transfer callback only with a progress sink (`transport_support.rs:89-92`). A push to a
  local-path remote (libgit2's local transport: `file://` or an existing directory, `transport.c:36, :88-89`) writes the
  remote repository itself during the upload: the pack (`transports/local.c:426`) and each ref (`:354`, `<ref>.lock`).
- **Long local phases.** Checkout calls its progress callback after each complete file, holding no lock across files
  (`checkout.c:1886-1891`), and writes the index at the end (:2686). `reset_hard` (`repository.rs:31`) checks out, moves
  HEAD, writes the index (`reset.c:154, :159, :167`); stash apply checks out twice, then writes the index, and pop then
  drops (`stash.c:1126, :1149, :1159, :1282-1285`). Five writes lie **inside a held ref transaction**, whose `lock_ref`
  creates `<ref>.lock` (`refdb_fs.c:1175-1179`): checkouts at `merge_prepared.rs:266, :339` and its expected-conflict
  `repo.merge` (worktree and index, :280; all locked at :248-249), `merge_recovery.rs:175` (:143-144) and :251
  (:197-198). Copies poll `Cancellation` per entry and chunk, removing a partial temporary
  (`crates/copy-contract/src/lib.rs:39-52`; `crates/refcopy/src/ordinary.rs:14-17, :477`); `gwz_local_import` polls per
  participant (`crates/local-import/src/lib.rs:337-339`); the family lock is OS-advisory (`src/local_clone/wait.rs:3-6`).

## 3. Design

### 3.1 The interrupt source (OD-C1; step 2.3s)

- **Module.** `gwz-cli/src/interrupt/`: `mod.rs` (install, reader, writer, `Interrupts`, `Output`, `OutputClaim`),
  `unix.rs`, `windows.rs` under one `cfg_if!`; allow-listed globals: the pipe's write end and, on Unix, the installer's
  PID. For the reader the first count byte is **Cancel**; the second, a Force byte or the bound is **Force**.
- **Install.** `run` installs right after parsing, so the reader and writer are gwz's first threads; 3.1 puts B's
  registry before it (B §4; OD-B3 (a)). 2.3s installs only for a server lifecycle command (`server start`, its background
  copy, `server stdio`), which takes the events through `Interrupts` (their use is C's); every other command keeps the OS
  default until 3.1c brings Cancel's slot, the bound and Force and installs for all but `hook`, `claude-code-setup` (R12).
- **Unix.** For SIGINT, SIGTERM and SIGHUP (§9 O4), `install` keeps an inherited ignored disposition (`nohup`,
  `trap '' INT`), else sets `sigaction` with `SA_RESTART`. The handler returns at once unless `getpid()` is the saved PID
  (a child between `fork` and `exec` runs it too); else it saves `errno`, writes one byte (both ends close-on-exec, write
  end non-blocking, `EAGAIN` ignored) and restores `errno`. SIGQUIT keeps its default (OD-C7 (b)).
- **Windows.** `SetConsoleCtrlHandler`. CTRL_C and CTRL_BREAK write a count byte and return `TRUE`; CTRL_CLOSE,
  CTRL_LOGOFF and CTRL_SHUTDOWN (§9 O4) write a Force byte and sleep without end: Force ends the process, or the OS does a
  short time later **(OS)** (R9). A CTRL_C disabled at creation stays disabled.
- **Writer.** Once installed, one module thread owns stdout and stderr; `Output` is one FIFO to it (events in order;
  before install, `lib.rs:167, :174`, and in `hook` it writes directly). A sender waits on a full queue only until Cancel,
  then event and progress lines are dropped (counted), never a notice or the final document; write errors are ignored.
  3.1c moves every direct write onto `Output` (its files' print macros; the handles at `append_branch_summary/machine.rs:13`,
  `lib.rs:279`, `pager.rs:130, :146`, `log_exec.rs:33, :36`, `progress_detail.rs:233`'s `Term::stderr()`) and adds
  `gwz-cli/scripts/check_direct_output.py`, a `cfg`-blind token check in gwz-cli's CI failing `print!`, `println!`,
  `eprint!`, `eprintln!`, `stdout()`, `stderr()` (std's, `Term`'s) outside `src/interrupt/`, `src/hook/`, `src/hook_exec.rs`.
- **Giving control back** (OD-C7 (a)). When Force begins, also when a foreground wait defers its later steps (§3.7), the
  reader restores each saved disposition (Windows: removes its handler); a later interrupt ends gwz by default (R14).
- **Check.** The kind `signal` (`libc::signal`, `libc::sigaction`, `SetConsoleCtrlHandler`, `signal_hook`, `ctrlc`,
  `tokio::signal`), out of `env`, runs in §2's five runs with no new wiring; only gwz-cli's allowlist gets an entry, this
  module. B's forked-child entries in gwz-core's allowlist take it in B's 2.3r-2, which also edits the check, after 2.3s.

### 3.2 First interrupt: Cancel

- **What it cancels.** `run` keeps the command's `HostOwner` (plan 3.1) in a slot shared with the reader. Cancel (or
  Force, if no Cancel came) takes it, calls `close()` and puts the `HostClose` back, never awaiting it: admission is
  sealed and the host's operations cancelled, so a running member's network I/O fails and a later member's admission is
  refused, both `Cancelled` (plan 3.1, §8 row 4). After its write frame the dispatch (main) thread empties the slot,
  closing an owner still there, and awaits the `HostClose` outside any frame, `Complete` or `Pending` (§3.3).
- **`--server`**: the carrier-backed host closes the same way; the served core's response and bound are C's (until then
  the local rule). **No host** (ordinary build, transport off, native route): nothing (R10). **Local work** runs on
  (`GwzCoreSessionDesign.md` §5.3; R7). **Wait:** Cancel starts `CLEANUP_BOUND` (5 s, `src/session_host/context.rs:80`);
  if the command returns first, the main thread renders (§3.6) and exits 130, otherwise Force begins.

### 3.3 What a cancelled member reports

- A member whose operation observed the cancel is `Failed` with `Cancelled`; one that failed first keeps its reason.
- **The cause survives a slow cleanup.** `terminate` returns `CleanupPending` after `CLEANUP_GRACE` (500 ms,
  `src/git/endpoint/https_auth.rs:28`; `owner.rs:253-276`), mapped to `Io` (`https_auth.rs:190`), losing `Cancelled` and
  a timeout's `Interaction` (`https_worker.rs:377`); B returns the lookup's own outcome (B §3.6; `runner.rs:226`) and 3.0e
  the file worker's `runner.check()` cause (`https_auth/file_worker.rs:91-98`), `CleanupPending` only with no other cause.
- **Pending cleanup is a finding, not the member's error.** B retains the tree (`owner.rs:298`) in the cleanup domain, so
  the `HostClose` the dispatch awaits (§3.2) is `Pending` (contract :272-275); 3.1c then writes `gwz: cleanup still
  pending` to stderr, replacing `dispatch.rs:82-87`'s `gwz-alpha` line.

### 3.4 Force: the write gate and the forced exit

**The write gate** (`crates/write-gate/`, a gwz-core crate that builds and tests alone): a cloneable `WriteGate`, one per
process; a `ThreadId` state map under one mutex and condition variable.

| Call | Meaning |
|---|---|
| `writing(label, f)` | `f` is a write frame of this thread (nesting allowed); refused `Closed` before `f` if the gate is closed |
| `park()`, `unpark()`, each `-> Open \| Closed` | in a frame: `park` makes the thread parked (no local write; Force does not wait for it) or, on a closed gate, leaves it writing and returns `Closed`, the caller failing `Cancelled` without starting the parked phase; `unpark` makes it writing again and says whether the gate closed meanwhile, the caller then taking its abort path |
| `outside(label, f)` | `park`, `f`, `unpark`; refused before `f` if closed; on `Closed` after `f` the thread stops |
| `safe_point(label)`; `close()`, `is_closed()`, `wait()` | `safe_point`: if closed, records `label` as stopped and the thread stops; `wait` returns the stopped labels once every thread in a frame is parked, stopped or out of its frames |

A thread **stops** by blocking for good. **(G1)** A frame is parked only while bytes are in transfer or while it waits (a
join, a slot, a lock poll); a writing thread blocks only in a local write, a `Wait` child, or a clone's network I/O
before its first object, which step 1's host close fails (no host: R10). **(G2)** No stop with an in-process lock held or
inside a held Git lock file (a ref transaction, a filebuf). **(G3)** After `close()` nothing new starts: frames and parks
are refused, queued members report `Cancelled`, and every serial member loop has a safe point at each member's head.

**Carriage.** 3.0d adds a `write_gate` field to `ProcessGate`, filled where composition mints it (§2; B's minting check
unchanged): `process_safety_with_children()` (B, once per process, 3.1) makes the process's `WriteGate`, read by gwz-cli
with `ProcessGate::write_gate()`; `process_safety()` gives an open one. `with_process_gate` copies it into a new field of
`Git2Backend` (open by default, beside `spawn_gate`, `backend.rs:26`), exposed by `GitRepository::write_gate()`; every
request backend takes the process gate (plan 3.1, §8 row 4); one built without it has an open gate and runs to its end.

**Where frames open and park.**
- 3.1c's dispatch runs every request as `writing` except a read-only list, an exhaustive `match` over `CliRequest` with
  no wildcard (a new request is a writer until classified); a request is listed only if it writes no file (row 6.14).
- `par_map_per_host_gated(gate, …)`, new beside `par_map_per_host` (unchanged), parks for its join, runs each item's `f`
  (not the worker loop, so slot waits stay outside) as `writing`, and passes `is_closed` as `cancelled`; §2's seven calls
  switch to it with `backend.write_gate()`.
- **Fetch** (`fetch`, `tag_fetch`, `fetch_anonymous`): `download` parked, `disconnect` `outside`, then `update_tips` as a
  write (with the tag option and prune `Remote::fetch` uses). The transfer callback, now always set, unparks at its first
  call with every object received, so the pack commit and `.git/shallow` are writes; on `Closed` it returns `false` and
  libgit2 frees the uncommitted pack in the frame. A download that returns still parked (no pack) unparks; a refused
  `download` park fails the member `Cancelled` before it contacts the remote.
- **Clone**: a write frame. The `remote_create` callback writes the pins (moved from :73, so every clone that reaches a
  checkout is pinned); the transfer callback parks while objects remain and unparks once all are received, so the pack
  commit and the checkout, with its safe point, are writes; a clone that never calls it (an empty remote) stays writing
  (G1). On `Closed` libgit2 removes the clone inside the frame (`clone.c:666-673`).
- **Push**: parked; the first `push_update_reference` unparks, so the tracking-ref update is a write; on `Closed` it
  returns an error and libgit2 skips that update (R5). A push to a local-path remote (§2) is a write throughout, as its
  upload writes the remote repository: Force waits for it (R8 if hung).
- `ls_remote`, `read_remote_file` (R16) and the family-lock poll are `outside` whole. Member-lock waits are on the served
  path only (`src/session_host/served.rs:81`): C's.

**Force**, on the reader thread, in order: (1) restore the dispositions (§3.1); close the host if Cancel did not (§3.2);
(2) the first Force notice (§3.6); (3) `close()` the gate; (4) B's `kill_killable()`: every `Kill` child ends now and a
later `Kill` spawn is refused `Closed`, its member reporting `Cancelled`; (5) `wait()`, without a time bound, the writer
printing each stopped label as recorded; (6) the claim (§3.6): taken if free, else Force waits for the main thread's
exit; (7) the output (§3.6); (8) up to 1 s for the writer, then exit 130 without unwinding (`_exit` on Unix,
`TerminateProcess` on the current process on Windows). A calls no `close()` of B's: every `Wait` child runs in a write
frame and G3 refuses new frames, so none starts after step 5. **Network work ends at once:** step 1's close fails the
host's I/O; a transfer still running (R10) is parked, not waited for, and ended by step 8 (R4, R6). **Local writes are
waited for** to a safe point or their end, a `Wait` child (`git commit`, `git tag`, hooks, signer; B OD-B1 (a)) without
a bound (decision (ii); R8).

### 3.5 Long local phases and their safe points

| Phase | Safe point | What a stop leaves |
|---|---|---|
| checkout outside a ref transaction: `refs.rs:33, :134`, `repository.rs:56, :81`, the clone's (`transport.rs:41`); `reset_hard` (a `CheckoutBuilder` passed to `reset`); stash apply and pop (`checkout_options` in `stash_restore_options`, `stash_support.rs:18-26`) | the progress callback, after each complete file | earlier files new, later old; index and HEAD old; a pop's stash kept (R1) |
| the five writes inside a ref transaction (§2): four checkouts, the expected-conflict `repo.merge` | none (G2): run to their end | nothing; R8 if hung |
| copy: `local clone` (`handle_local.rs:72`), family import (`family_merge.rs:424`), today `NeverCancelled` | one adapter implementing both `Cancellation` traits, answering `is_closed()` | whole files; a `creating` lane or partial import (R2) |
| stage: `add_all`'s matched-path callback (today `None`, `repository.rs:112`); the per-path loop | the callback; each iteration | loose objects only; index unchanged (R3) |
| serial member loops: `handle_commit.rs:179-186` and any other 3.0f's inventory finds in `src/workspace_ops/` | each member's head | earlier members done, later not (R7) |
| `stash_save` (`stash.rs:18`, `preservation.rs:344`); a Git CLI route commit or tag | none: run to the end | nothing; R8 if hung |

A stopped copy runs its error path to the frame's end. Every write sequence the table does not name runs to its end,
writing a bounded set per member (refs, one index, one commit, workspace artifacts).

### 3.6 Exit status and output (OD-C4)

- **Status.** **130** on every platform, over 0, 1 and 2 (`gwz-cli/src/globalargs/render_exit.rs:82-95`) and in place of
  a signal death or `STATUS_CONTROL_C_EXIT`, on the response and error paths, `diff`, `log` and `forall` (§9 O4).
- **The claim.** `OutputClaim` (one atomic, owned by `run`) is taken once: by the dispatch (main) thread as the last act
  in its write frame (a read-only request: at its return), or by Force at step 6 if free; as `wait()` returns only after
  that frame ends, a finished dispatch holds it first. Its holder alone writes stdout's final document. If the dispatch
  holds it, Force waits up to the close bound plus 1 s for the main thread to await the host's close, render and exit,
  then exits 130 (R13). The status is 130 when an interrupt preceded the claim; a later one changes neither.
- **When the dispatch holds the claim**, the main thread renders as usual, with the first own entry of `errors` (after
  the member copies; `ErrorCatalog.md:35`) `{"code":"Cancelled","message":"interrupted","member_id":null,
  "member_path":null,"target_kind":null,"detail":null,"record_context":null}`; `aggregate_status` as today. An execution
  error's top-level `errors` is [that entry, its own error].
- **When Force takes it**, `--json` gets the top-level shape with `meta: null`, no members and the one `Cancelled` entry,
  `message` `interrupted; forced exit, see stderr for what stopped`; `--jsonl`, human and porcelain get nothing more on
  stdout, so a `--jsonl` stream has no final response (R13).
- **Notices** (the writer, stderr, every output mode): at Cancel `gwz: interrupted; cancelling (interrupt again to stop
  now)`; at Force `gwz: forced exit; network work ended, waiting for local writes to finish or reach a safe point`, then
  `gwz: stopped: <member path> (<phase>)` per label and `gwz: transfer ended: <member path> (fetch|push|clone)` per parked
  transfer, a push adding `; the remote may have received it`; `gwz: <n> progress lines dropped` when any were.

### 3.7 The pager and `forall` (OD-C6)

A foreground program (the pager after its input closes, `gwz-cli/src/pager.rs:138-145, :155-170`; a `forall` member's
`status`, `gwz-cli/src/forall.rs:260`) gets the terminal's interrupt; gwz never signals it. `Interrupts::foreground(f)`
wraps that wait: no bound runs; Force's step 1 runs at once (a further interrupt or SIGTERM then ends gwz by default,
R14), its later steps waiting for `f`. On Cancel, `diff` and `log` close the pager's input and wait; `forall` finishes
the current member, starts no further one, appends `interrupted: <k> of <m> members not run` and exits 130.

### 3.8 gwz-py (resolution (2) and OD-C4; step 3.2b)

- **The bridge.** `_run_native` (`gwz-py/src/gwz/bridge.py:298-316`) today only waits on `CancelledError` (:28-35); a
  network call registers `op_<request_id>` on its worker thread (`gwz-py/native/src/client_host.rs:226-231`;
  `shims.rs:79-80`), and `cancel` refuses an id not yet live (`client_host/operations.rs:199-208`).
  `NativeCoreBridge.call` (`bridge.py:318`) passes `_run_native` the call's operation id (an exported native function
  over `shims::operation_id`); on the first `CancelledError`, `_run_native` calls the native `cancel_call(id)` on its own
  thread, shielded: a live operation is cancelled as by `cancel`; an id not yet live is recorded (the last 64, as
  `operations.rs:27`) and its registration refused `Cancelled`, as `gwz-py/dev-docs/GwzPyPerOperationTransportDesign.md:104`
  says. It then waits through `_await_completion` and re-raises `CancelledError`, carrying the decoded response, if any,
  as `gwz_response`; a library caller still sees `CancelledError`; `Client.cancel_operation` keeps its refusals. No
  handler: 3.2b's test beside `test_process_globals.py`'s parses `gwz-py/src/gwz/` (`ast`) and fails any `signal.signal`,
  `signal.set_wakeup_fd` or `add_signal_handler` call (`asyncio.run`'s own handler is Python's, used below).
- **The gwz-py CLI.** `main` catches only `CliUsageError` and `GwzError` (`cli.py:142-156`). `run` (:76) catches
  `CancelledError` around the command and renders the carried response (members as completed, the `Cancelled` entry
  first among its own errors, as §3.6) or, with none, `GwzBridgeError("interrupted", code="Cancelled")` as `main`'s error
  clause does, and returns **130**. On 3.11+ the first SIGINT cancels the main task (`asyncio/runners.py:150-157`) and
  `asyncio.run` returns its value (:117-124); on 3.10 its `finally` cancels the task (the same path), then raises
  `KeyboardInterrupt` (`runners.py:44-49, :55-63`). `main` catches `KeyboardInterrupt` and returns 130, rendering only if
  `run` rendered nothing (a flag `main` passes). SIGTERM, SIGHUP, CTRL_BREAK, CTRL_CLOSE keep Python's default (R15).

## 4. Interfaces with B, C and D

**B points A relies on** (B §3.7, revision 2): (1) `kill_killable()` ends every live `Kill` entry at once, leaving `Wait`
and `OwnInput` children, and refuses later `Kill` spawns `Closed` (row B22); (2) gwz-cli calls no `close()` (§3.4);
(3) `run` creates the registry before any thread, so before A's install (B §4 3.1; OD-B3 (a));
(4) `process_safety_with_children()` mints the `ProcessGate`, `with_process_gate` carries it to every backend (B §3.1,
§4), A's write gate inside; (5) `terminate` returns the lookup's own outcome and retains the tree as pending cleanup
(B §3.6), reported as the host close's `Pending` (§3.3); (6) B changes no disposition in gwz's process, its forked-child
allowlist entries taking the kind `signal`; (7) B's 2.3r-2 runs after 2.3s (both edit the check, §8 row 7); B's R-B4
covers a `git commit` torn by R14 or SIGKILL. **C**: a server lifecycle command's use of `Interrupts` (2.3i's stop
input, §8 row 5), a served core's response to a `--server` host's close, its bound. **D**: §7's recognizers, a push's
remote effect in JSON (R5), the "beyond" rows to the operator.

## 5. Steps

2.3s is Phase 2 (wave 2, lane 2, after 2.3f, before 2.3k); the rest Phase 3 (graph: §8 row 1; ≤3 compiling lanes; 3.0d
settles `WriteGate` before 3.1c), phase-reviewed (3.5). Sizes: code + rows. No trunk file (:325); `refs.rs`,
`repository.rs` go through the Phase 5 lane (:329-331).

| Step | Content | Owns | Size | After |
|---|---|---|---|---|
| **2.3s** (gwz-cli; gwz-core check) | §3.1: module, handlers, reader, writer and `Output`, `Interrupts`, installed for the server lifecycle only; the `signal` kind | create `gwz-cli/src/interrupt/`; edit `gwz-cli/src/lib.rs`, `gwz-cli/Cargo.toml` (`libc` on Unix, `windows-sys` on Windows), `gwz-cli/scripts/process_globals_allowlist.json`, `scripts/checks/{check_process_globals,test_check_process_globals}.py` | ~300 + 300 | 2.3f (lane); before B's 2.3r-2 |
| **3.0d** (gwz-core) | `write-gate`; carriage; `par_map_per_host_gated`; fetch, clone and push frames | create `crates/write-gate/`; edit `Cargo.toml`, `src/transport_host/composition.rs`, `src/git/gitbackend/{backend,contract,transport,transport_support}.rs`, `src/operation/par_map_per_host.rs`, §2's six caller files, `src/local_clone/wait.rs` | ~460 + 480 | Phases 1, 2 |
| **3.0e** (gwz-core) | the file worker's cause (§3.3) | edit `src/git/endpoint/https_auth/file_worker.rs` | ~30 + 200 | Phases 1, 2 |
| **3.0f** (gwz-core) | §3.5's safe points; the member-loop inventory | edit `src/git/gitbackend/{stash,stash_support,transport}.rs`, `src/workspace_ops/{handle_local,handle_commit}.rs`, `src/local_clone/family_merge.rs`, loops found; `refs.rs`, `repository.rs` via Phase 5 | ~200 + 400 | 3.0d |
| **3.1c** (gwz-cli) | install for every command; Cancel's slot, bound, Force; claim, marking, notices, 130; every write through `Output` and its check; read-only list; pager, `forall` | edit `src/lib.rs`, `src/globalargs/dispatch.rs`, `src/pager.rs`, `src/forall.rs`, `src/diff_exec.rs`, `src/diff_render.rs`, `src/log_exec.rs`, `src/clirequest/invocation.rs`, `src/append_branch_summary/machine.rs`, `src/progress_detail.rs`, `src/interrupt/`, `.github/workflows/ci.yml`; create `scripts/{check_direct_output,test_check_direct_output}.py` | ~500 + 500 | 3.1, 3.0d, 3.0e |
| **3.2b** (gwz-py) | §3.8; the Python handler test | edit `gwz-py/src/gwz/{bridge,cli}.py`, `gwz-py/native/src/client_host.rs`, `client_host/operations.rs`, `gwz-py/src/tests/test_process_globals.py` | ~110 + 240 | 3.2 |

## 6. Rows

Each is seen red first; each runs on macOS, Linux and Windows (M, L, W) unless marked; W's console events come from
`GenerateConsoleCtrlEvent` in a child console.

- **6.1** [2.3s] Each source gives Cancel then Force; an inherited ignored SIGINT or SIGHUP stays ignored; SIGQUIT keeps
  its default; children spawned in a loop and one SIGINT to the group give one Cancel. Red: no module; no PID check.
- **6.2** [2.3s, 3.1c; any] The check fails a `sigaction` or `SetConsoleCtrlHandler` outside gwz-cli's module in each of
  the five runs; [3.1c] the direct-output check fails a `println!` and an `io::stdout()` outside its allowed files, also
  in a `cfg(windows)` branch on Linux. Red: no `signal` kind; no check.
- **6.3** [2.3s] Through `Output`, events and the final document keep their order; with a consumer that stopped reading,
  a sender blocks before Cancel, never after it; no notice or document is dropped. Red: direct writes.
- **6.4** [3.0d] `wait` returns at once with no frame, waits for a writing thread until its `safe_point`, not for a parked
  one; after `close()` frames and parks are refused. Red: no crate.
- **6.5** [3.0d] `wait` waits for a gated worker in `update_tips`, not in `download` or a slot wait; two same-host
  members, per-host 1, member 1 stopped at a safe point: `wait` returns in 1 s, member 2 `Cancelled`. Red: a looped frame.
- **6.6** [3.0d] The split fetch writes the refs, tags, `FETCH_HEAD` and prunes `Remote::fetch` does; Force at a
  transfer's end, in its pack commit or shallow write leaves no `shallow.lock` or `*.idx.lock`, the next fetch succeeding;
  Force after a clone's transfer, after `builder.clone` or in an empty remote's clone leaves no member or a pinned one; a
  refused `download` park is `Cancelled`, no fixture contact. Red: `download` parked whole; late pins; empty clone parked.
- **6.7** [3.0d] A native-route three-member push to a bare file remote, Force in member 1's pack write, ref write and
  tracking-ref update: no `*.lock` in the member or the remote, whose `git fsck` passes with the push whole or absent;
  members 2 and 3 never contact the remote (its log) and report `Cancelled`. Red: a parked local-path upload; no refusal.
- **6.8** [3.0e, 3.1c; W, and M L with the f9 slow-drain fixture] A cancel whose tree outlasts `CLEANUP_GRACE` returns
  `Cancelled` from the helper and the file worker; through the HTTPS worker the member is `Cancelled`, a timeout keeps
  `Interaction`; after a gwz-cli Cancel the `HostClose` the dispatch awaits is `Pending`, stderr has `gwz: cleanup still
  pending`, the reap follows within 30 s. Red: `CleanupPending`; `Io`; a `HostClose` nobody awaits.
- **6.9** [3.0f] Force at file k of an N-file checkout, `reset_hard` and stash apply (child process): each file wholly old
  or new, index and HEAD unchanged, the notice names member and phase; Force during a fast-forward merge's checkout and
  both recovery checkouts: each completes, no `refs/**/*.lock`, the next `gwz merge --abort` succeeds. Red: no callback;
  a safe point inside the transaction.
- **6.10** [3.0f] Force during `local clone`'s copy of a file over one chunk: every file complete, no temporary; in a
  family import: stops before a participant; in `stage` (both forms): index unchanged; in member 1's 8 s hook of a
  three-member `commit`: member 1 commits, members 2, 3 and the root do not start. Red: `NeverCancelled`; no loop point.
- **6.11** [3.1c] SIGINT, SIGTERM, SIGHUP, CTRL_C, CTRL_BREAK on `gwz --json --max-per-host 1 fetch` of two members on
  one host, the first stalled: exit 130 within the bound, both `Failed` with `Cancelled` (the second refused admission),
  the entry first among own entries; after every member succeeded: `Ok`, exit 130, the entry. Red: signal death.
- **6.12** [3.1c] A second interrupt during a stalled download: exit 130 within 1 s, `transfer ended`; `--json` stdout is
  the forced envelope, `--jsonl` has no response, human stdout is empty; stdout a pipe nobody reads and stderr closed:
  exit 130 within 2 s, no panic; `gwz --jsonl pull` whose consumer stops reading after one event, then SIGTERM: exit 130
  within the bound plus 1 s. Red: no Force; sinks write directly.
- **6.13** [3.1c] `--json commit`, an 8 s hook, one interrupt (Force at the bound): one member: Force waits, stdout is
  the full response (member `Ok`, the entry), exit 130, also in 100 runs with a second interrupt at the hook's exit;
  three members: member 1 commits, stdout is the envelope, stderr names the stop; an interrupt during the wait ends gwz
  by default, also 50 ms after the second (R14). Red: claim taken at Force's start, or after the frame.
- **6.14** [3.1c] Each read-only request leaves all fixture files' bytes and mtimes as they were. Red: a writer listed.
- **6.15** [3.1c] A `forall` member past the bound still runs, no further member starts, exit 130; a member that ignores
  SIGINT, and three SIGTERMs to gwz: the third ends gwz, the member runs on; `diff` waits for its pager without a bound;
  W: CTRL_CLOSE runs Force before the OS ends the process **(OS)**. Red: no foreground rule.
- **6.16** [3.2b; 3.10 and 3.11+] Cancelling a task awaiting a transport-route fetch calls `cancel_call` first, the call
  ending `Cancelled` within the bound; with `max_workers=1` and one blocking call, a cancelled queued fetch ends
  `Cancelled` without contacting the fixture; local and native-route calls wait to their end; the exported id equals the
  registered one; the gwz-py CLI's `--json` exits 130 with `Cancelled` on SIGINT and, for a three-member local `commit`,
  shows what committed; the source test fails a planted `loop.add_signal_handler`. Red: `_run_native` only waits; no test.

## 7. Residuals (for design D)

"Beyond": not covered by D-V's SIGKILL, crash and power loss or decision (i)'s hung write; D puts it to the operator.

| # | What can remain | When | How it is recognized now | Beyond |
|---|---|---|---|---|
| R1 | a partly checked-out tree; index and HEAD old; a popped stash kept | Force at a checkout safe point | the notice; D | no |
| R2 | a `creating` lane or partial family import (whole files) | Force at a copy safe point | the notice; D | no |
| R3 | unreachable loose objects | Force during stage | none needed (`gc`) | no |
| R4 | libgit2's temporary pack in `objects/pack` | Force while a pack is being received | `transfer ended`; D's sweep | no (network ends at once) |
| R5 | an unknown remote effect; or the remote has the push and the tracking ref is stale | Cancel or Force during a network upload; Force after the remote's report | the member's `Cancelled` and the catalog's remedy (`ErrorCatalog.md:35`); Force's `may have received it`; D's JSON | no |
| R6 | a partial new member directory | Force during a clone's transfer | the next clone refuses a non-empty target (`transport.rs:32`); D | no |
| R7 | earlier members done, later not | Cancel during pull or materialize; Force between members | the response (`Partial`, members `Cancelled`) or the stop notice; re-running completes | yes ("no partial state") |
| R8 | none; gwz waits without a bound and only SIGKILL ends it | a hung `Wait` child, hook or signer (R-B10), `stash_save`, a write in a ref transaction, a local-path push, a local backend's write, a write blocked in the filesystem | the first Force notice; `ps` | decided (i) |
| R9 | SIGKILL's residues | Windows ends the process after CTRL_CLOSE, LOGOFF or SHUTDOWN **(OS)** | as SIGKILL's; D | CTRL_CLOSE decided (OD-C7 (c)); LOGOFF, SHUTDOWN with O4 |
| R10 | a network call runs on after Cancel; Force waits on a stalled native read between a pack's last object and its end, or in a clone's negotiation | no host: native route, ordinary build, transport off | Force at the bound ends a parked call | yes |
| R11 | a gwz-py local or native-route call runs to its end; a second interrupt keeps waiting; on 3.10 a SIGINT inside a task step skips the cancel | gwz-py | `_await_completion` (unchanged) | yes |
| R12 | the OS default: gwz dies by the signal | `hook`, `claude-code-setup` | as SIGKILL's; D | yes |
| R13 | no complete final document: a `--jsonl` stream without its response; a document cut at step 8 | Force taking the claim; a stalled stdout or host close past Force's wait | exit 130 without a complete document | yes (OD-C4 marking) |
| R14 | a write torn by the default disposition | an interrupt after Force began, however soon; SIGQUIT at any time | death by signal (or a core) instead of 130; B's R-B4 | decided (OD-C7 (a), (b)) |
| R15 | a gwz-py CLI write torn, no cancel | SIGTERM, SIGHUP; Windows CTRL_BREAK, CTRL_CLOSE | death by signal; D | SIGTERM, SIGHUP decided (OD-C7 (d)); the Windows events with O4 |
| R16 | a `tempfile` directory with a partial clone in the system temp directory | Force during `read_remote_file` (`transport.rs:718, :740`) | D's temp sweep | no (network ends at once) |

## 8. Superseded or amended clauses (applied by the lane owner at acceptance)

| # | Document and clause | Replacement or addition |
|---|---|---|
| 1 | Plan §4 graph, Phase 3 (:315) | `Phase 3  ─ {3.0 ∥ 3.0b ∥ 3.0d ∥ 3.0e ∥ 3.1 ∥ 3.2} ; 3.0d → 3.0f ; {3.1, 3.0d, 3.0e} → 3.1c ; 3.2 → 3.2b ; {3.0, 3.0b, 3.0f, 3.1c, 3.2b} → 3.3 → 3.4 → 3.5 (review)   (after Phases 1 and 2)` |
| 2 | Plan Phase 3 heading (:712) | "(three lanes then one, 19–23 lane-days)" for "(two lanes then one, 11–13 lane-days)" |
| 3 | Plan §4, after 3.0b (:717-720) | Bullets 3.0d, 3.0e, 3.0f, 3.1c, 3.2b with §5's content, sizes and "After", citing this design's §5 |
| 4 | Plan 3.1 (:721-728) | Append: "The command's `HostOwner` sits in a slot the interrupt reader closes on Cancel, the dispatch awaiting the returned `HostClose` after its write frame (GwzCancelInterruptDesign §3.2); a request refused `AdmissionError::Closing(HostClosing)` or ended by that close reports `Cancelled`; every request backend takes the process gate, with its write gate, through `with_process_gate` (§3.4)." |
| 5 | 2.3 plan 2.3i (:186-188) | "SIGINT/SIGTERM and Windows Ctrl-Break through a handler that sets a flag only" → "stop requests from a stop input its caller passes (gwz-cli's interrupt module, GwzCancelInterruptDesign §3.1; gwz-core installs no handler, OD-C1)"; red row "a second signal or `force`" → "a second stop request or `force`" (the signal leg moves to 2.3k) |
| 6 | 2.3 plan 2.3k (:207-217) | Add: "`server start`, its background copy and `server stdio` take design A's `Interrupts` (2.3s) and pass 2.3i its stop input; red row: SIGINT, SIGTERM and Ctrl-Break stop a foreground server". "After" adds 2.3s |
| 7 | 2.3 plan §3, §4 (:240, :249-250) | Add "**2.3s Interrupt source (single, Surface).** GwzCancelInterruptDesign §3.1, §5, rows 6.1-6.3; ~300 + 300"; wave 2 lane 2: "2.3f listener files, then 2.3s interrupt source"; totals one step, 300 and 300 more; B's 2.3r-2 (its §8) gains "After 2.3s": both edit `scripts/checks/check_process_globals.py`, merged serially, 2.3s first |
| 8 | `MachineOutput.md` Exit Codes (:954-964) | Subject to §9 O4 (shown as recommended): add row "interrupted (Ctrl-C, SIGINT, SIGTERM, SIGHUP; Windows Ctrl-C, Ctrl-Break, console close, logoff, shutdown) \| `130`, whatever the aggregate" and a paragraph "Interrupted commands" with §3.6's entry, its place, the forced envelope and the `--jsonl` rule |
| 9-12 | MachineOutput: (9) Notes (:280-287); (10) JSONL (:669-673); (11) :142-143; (12) :66-69, "and one error entry" | (9) Append: "Unlike `note:` lines, the interrupt and forced-exit notices are written to stderr in every output mode." (10) Append: "A command forced to exit after an interrupt (exit 130) may end the stream without that final response." (11) Append: "After an interrupt the first of these is the `Cancelled` entry with no member." (12) "and one error entry (two after an interrupt: the `Cancelled` entry, then the error)" |
| 13 | `gwz-core/docs/ErrorCatalog.md:35` | "A result's first error;" → "A result's first error of its own (in a response's `errors`, after the copies of member errors);" and append to the remedy: "An interrupted gwz-cli or gwz-py CLI command adds one `cancelled` entry with no member and exits 130." |
| 14 | `GwzCoreSessionDesign.md:687` | Append: "gwz-cli's interrupt no longer tears a write it is running (GwzCancelInterruptDesign §3.4); a detached worker's exposure stands." |
| 15 | `gwz-py/dev-docs/GwzPyPerOperationTransportDesign.md` §2.5 (:103-106) | Add: "**A caller's cancellation of an awaiting call** first cancels it through `cancel_call`, which also refuses a not-yet-registered one, then waits; the CLI renders what completed and exits 130 (cancellation design A §3.8)." |
| 16 | `gwz-py/README.md:225` | Append: "Cancelling the task that awaits a transport-route call cancels its operation first." |

The helper-owner contract's `CleanupPending` clauses are history and stay unedited (AgentProcessRules L1-33).

## 9. Decisions

Recorded (D-V :1055-1062): **(i)** A3, §3.4, R8 (revision 1's bounded wait, its residuals for writes cut by the exit, its
O2 and Q1 are gone); **(ii)** A4, §3.4; **(iii)** O1 §3.1, §5; OD-C7 (a)-(d) §3.1, §3.8, R9, R14, R15. To the operator:
- **O4 Which signals take the interrupt path, and their status** (OD-C4 says 130 "on interrupt"). **Recommended (a):**
  SIGINT, SIGTERM, SIGHUP and Windows' console events, all 130: one rule, and a supervisor's SIGTERM never tears a write.
  (b) The same, exiting 128 + the signal (129, 143). (c) SIGINT only; SIGTERM and SIGHUP keep their default (tearing).
  It changes only §3.1's lists, §3.6 "Status", §8 row 8 (as (a)), R9's and R15's O4 cells and rows 6.11-6.12's legs.
- **O5 Decision (i) beside OD-C7 (a), to confirm.** (i) says only SIGKILL ends a hung write; (a) lets any interrupt after
  Force began end gwz by default, which can tear a write that is not hung (row 6.13's 50 ms case). A applies both (R14).
  If (i) withdraws (a), the handler ignores later interrupts, changing only §3.1 "Giving control back", §3.4 Force step
  (1), §3.7, R14, row 6.13's 50 ms case and row 6.15's third SIGTERM.

## 10. Changes from revision 1

C-P2-1 §3.2, §3.4 "Carriage", §8 row 4; C-P2-2 §3.1, §5, §8 rows 5-7; C-P2-3 §3.4 (G2), §3.5, row 6.9; C-P3-1 §8 rows
10-12, 14; C-P3-2 §2, §3.5; C-P3-3 §3.1, row 6.2; C-P3-4 §3.4, §5; C-P3-5 §3.1, §4; C-P3-6 §3.6, R13; C-P3-7 §9 O4;
C-P3-8 §3.6, §8 row 9; C-P3-9 §1, §3.7, §3.8, §5. S-P2-1 §2, §3.4 (G1), rows 6.6-6.7, R4-R6; S-P2-2 §3.1, §3.4, rows
6.3, 6.5, 6.12; S-P2-3 §3.4 (G3), rows 6.7, 6.10; S-P2-4 §3.6, row 6.13; S-P2-5 §9 O5, R14; S-P2-6, S-P2-7 §3.8, row
6.16; S-P3-1 §3.1, row 6.1; S-P3-2 §3.7, row 6.15; S-P3-3 R5; S-P3-4 row 6.14; S-P3-5 R9; S-P3-6 §3.1 (R17 gone);
S-P3-7 §3.2. The fix pass (its G1-G13, not §3.4's invariants) is mapped in the RemPlan.
