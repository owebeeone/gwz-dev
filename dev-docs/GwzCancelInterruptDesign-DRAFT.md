# Ctrl-C in the CLI: design A of the cancellation split (DRAFT)

**Status: accepted at `8facabd8` after the round-3 reviews (-ReviewConsistency-3.md, -ReviewSafety-3.md) and the closure
pass (verifiers GO on every axis, 2026-11-03); this accepts the design text and its clause amendments, not an
implementation.**

**Authority.** `GwzTransportReleaseConvergencePlan.md` §10 D-V (plan :999-1069): OQ2 reversed (:1005-1010); OD-C1 to OD-C6
(:1030-1031; text in `history/GwzCancellationAmendment-DRAFT.md` revision 1, root `9d5a4d4c`, §6, OD-C7 :695-702); resolutions
(1)-(3) (:1035-1045); the split (:1046-1054); round 1 (:1055-1062): **(i)** Force never exits mid-write,
**(ii)** `git commit`/`git tag`, hooks and signers finish, **(iii)** O1, O3 / OD-C7 (a)-(d); round 2 (:1063-1069): **O4
(a)** SIGINT, SIGTERM, SIGHUP and the console events exit 130, **O5** OD-C7 (a) kept, (i) governing gwz's own exit. "B" is
`GwzChildLifetimeDesign-DRAFT.md` (revision 3 as on disk 2026-11-03, for round-3 review; §4).

**Citations** are at root `6d20ef6f` (cited documents as at `48feeaad` but the plan's :1063-1069), gwz-core `b745f1fa`, gwz-cli
`c0ba717`, gwz-py `2185cc6`, gwz-git `9bc4ea9`, gwz-transport `281a28d`, git2-rs `d13951f`, libgit2 `b172e3d18`, CPython 3.10.15
and 3.11.16. Member-named paths are workspace-relative, `src/…`, `crates/…`, `scripts/…` gwz-core's, bare documents the root's
`dev-docs/`; a bare `:N` is a line of the file last named; **(OS)**: an OS fact no file holds; **(ext)**: a non-OS one.

## 0. Scope

An interrupt in gwz-cli or gwz-py (§3.8); §7 names every residual. Child lifetime is B's; the served form (2.3i, 2.3k, a
`server` subcommand's shutdown and status, a served core's response) C's; sweeps, `*.lock` reports, recognizers, the residual
list D's. D-V's "cancellable entry" (:1050) is the 3.1 `HostOwner` in a slot (§3.2; 3.3 removes old entries, :736-738).

## 1. Requirements (each traced to D-V)

- **A1 One source** (OD-C1; O1). One gwz-cli module owns the signal and console handlers, the server lifecycle's too;
  gwz-core and gwz-py install none: the process-globals check (Rust) and 3.2b's test (gwz-py's Python, §3.8) enforce it.
- **A2 Cancel** (OQ2; OD-C4). The first interrupt cancels network work; gwz exits when it returns or the bound passes.
- **A3 Force** (decision (i)). A second interrupt or the bound kills `Kill` children and ends network work at once, then waits,
  unbounded, for each local write in progress to reach its next safe point or finish: gwz's own exit is never mid-write (but a
  failed clone's removal, R6). Safe points are file-granular, never in a held Git lock file (OQ2: "no … stale lock"); only
  SIGKILL, a crash, power loss (resolution (1)) and §7's R9, R11, R12, R14 (a further interrupt, SIGQUIT), R15 end such a write.
- **A4 Commit and tag** (decision (ii)). A registered Git CLI route `git commit`/`git tag`, hooks and signer are never
  killed; a registry-less one (ordinary build) gets the terminal's interrupt as on 1.0.18 (B R-B13; R17).
- **A5 Status and marking** (OD-C4; O4 (a)). 130 and a JSON `Cancelled` marking in both CLIs; no output denies a write.
- **A6 Honest member reports** (D-V "cleans up"; `/Volumes/projects/limbo/build-targets/winflk-logs/f9-report.md`, "Findings
  outside scope" 2): `Cancelled`, also when the helper's cleanup is still pending.
- **A7 Foreground programs** (OD-C6). gwz never kills the pager or a `forall` command; it waits and starts no further one.
- **A8 gwz-py** (resolution (2)). A caller's cancel goes through the canceller first, also before the operation registers.
- **A9 Code rules.** Braces; `cfg` only in `cfg_if!` or platform modules; no global or thread-local but the module's handler
  state (OD-C1); no production code under `tests/`; one behaviour on three platforms and both builds unless §3 says.

## 2. Facts this design builds on

- **No handler today.** No production source of the five repositories names `libc::signal`, `libc::sigaction`,
  `SetConsoleCtrlHandler`, `ctrlc` or `tokio::signal`; `run` parses at `gwz-cli/src/lib.rs:170-178`. The check
  (`scripts/checks/check_process_globals.py`) classes `signal`, `sigaction` as `env` (:103-104, :540-541), scans
  `crates/*/src/lib.rs` too (`scripts/checks/process_globals_allowlist.json` "roots"), and runs for gwz-core
  (`scripts/run_tests.py:552`), gwz-transport (:316-327), gwz-git (:472-479), gwz-cli
  (`gwz-cli/.github/workflows/ci.yml:26-31`) and gwz-py (`gwz-py/src/tests/test_process_globals.py:11-23`).
- **Two builds.** `transport_host` (`ProcessGate`, its composition) is candidate-only (`src/lib.rs:47-51`); the ordinary
  build's dispatch runs on `Git2Backend::new()` (`gwz-cli/src/globalargs/dispatch.rs:91-96`; `spawn_gate`, `backend.rs:26`).
- **The host.** Candidate network commands run on `with_local_transport_native*` (`dispatch.rs:57-89`); plan 3.1 (:721-723)
  builds one `HostOwner` per command, local or `--server`; 3.3 removes the old entries (:736-738). `HostOwner::close` seals
  admission at the call (`Closing(HostClosing)`), cancels the host's operations and returns a `HostClose` future, resolved
  within the close bound (5 s, `host/config.rs:22, :36`), `Pending` after it
  (`gwz-transport/crates/gwz-connection/src/host.rs:6-10`, `host/owner.rs:21-32, :50-59`, `operation.rs:343-344, :414-417`;
  `GwzConnectionApiContract.md:269, :272-275`); no gwz-core code maps `HostClosing` yet. `ProcessGate` is minted only in
  `src/transport_host/composition.rs` (:21-35; "the only site", :30-31).
- **Fan-out.** `par_map_per_host` is public (`src/operation/par_map_per_host.rs:35-47`); `_with_cancel` gives queued items
  `on_cancel` without entering `f` (:58-60). Its seven calls (`handle_fetch.rs:154`, `push_member.rs:337`,
  `publication.rs:245`, `handle_materialize/apply.rs:43`, `member_validation.rs:33, :49`, `handle_init_from_sources.rs:147`)
  hold `backend: &B`, `B: GitRepository` (`src/git/gitbackend/contract.rs:42`); after the join, materialize removes its fresh
  clones on an error (`apply.rs:178-183`), init-from-sources too or writes the manifest and lock (:227-238). `commit` loops
  members serially (`src/workspace_ops/handle_commit.rs:179-186`).
- **libgit2's network calls write locally.** A fetch download commits the pack (index through a filebuf, a rename) after the
  stream (`libgit2/src/libgit2/transports/smart_protocol.c:782`), then writes `.git/shallow` (`fetch.c:218-223`); the
  transfer callback fires after each received object (`indexer.c:874`); an uncommitted pack is unlinked when freed
  (`indexer.c:1469`). A clone inits, calls `remote_create` (gwz's: `src/git/gitbackend/transport.rs:55-59`), fetches, sets
  HEAD, then checks out (`clone.c:394-407`); gwz then pins `core.autocrlf`/`core.eol` (:73), creation time only
  (`repository_support.rs:30-35`); on error libgit2 removes what it made (`clone.c:666-673`). A push calls
  `push_update_reference` after the remote's report (`remote.c:3032-3038`), before the tracking-ref update (:3064-3067) and
  the disconnect (:3070); gwz sets it (`transport_support.rs:130`; `transport.rs:368`), and the transfer callback only with a
  progress sink (`transport_support.rs:89-92`). A push to a local-path remote (libgit2's local transport: `file://` or an
  existing directory, `transport.c:36, :88-89`) writes the remote repository during the upload: the pack
  (`transports/local.c:426`) and each ref (`:354`, `<ref>.lock`).
- **Long local phases.** Checkout calls its progress callback after each complete file, holding no lock across files
  (`checkout.c:1886-1891`), and writes the index at the end (:2686). `reset_hard` (`repository.rs:31`) checks out, moves
  HEAD, writes the index (`reset.c:154, :159, :167`); stash apply checks out twice, then writes the index, and pop then drops
  (`stash.c:1126, :1149, :1159, :1282-1285`). Five writes lie **inside a held ref transaction**, whose `lock_ref` creates
  `<ref>.lock` (`refdb_fs.c:1175-1179`): checkouts at `merge_prepared.rs:266, :339` and its expected-conflict `repo.merge`
  (worktree and index, :280; all locked at :248-249), `merge_recovery.rs:175` (:143-144) and :251 (:197-198). Copies poll
  `Cancellation` per entry and chunk, removing a partial temporary (`crates/copy-contract/src/lib.rs:39-52`;
  `crates/refcopy/src/ordinary.rs:14-17, :477`); `gwz_local_import` polls per participant
  (`crates/local-import/src/lib.rs:508-509`); the family lock is OS-advisory (`src/local_clone/wait.rs:3-6`).

## 3. Design

### 3.1 The interrupt source (OD-C1; step 2.3s)

- **Module.** `gwz-cli/src/interrupt/`: `mod.rs` (install, reader, writer, `Interrupts`, `Output`, `OutputClaim`), `unix.rs`,
  `windows.rs` under one `cfg_if!`; allow-listed globals: the pipe's write end and, on Unix, the installer's PID. For the
  reader the first count byte is **Cancel**; the second, a Force byte or the bound is **Force**.
- **Install.** 2.3s provides `install` and `Interrupts` with a re-exec test harness (§6) and calls neither from `run`: 2.3k
  calls `install` in lifecycle mode for every `server` subcommand (§8 row 6), and 3.1c in `run` right after parsing for every
  other command but `hook` and `claude-code-setup` (R12), after B's registry (3.1; B §4, OD-B3 (a)), so the module's threads are
  the first after the registry's. If C gives a server command a registry, C creates it first.
- **Server lifecycle mode** (every `gwz server` subcommand: `start`, its background copy, `stop`, `status`, `list`, `stdio`; 2.3
  plan :212-213). The reader only delivers Cancel and Force to `Interrupts` and restores the dispositions at Force (OD-C7 (a));
  no writer, notice, slot, bound, gate close, claim or forced exit: C owns output (`server stdio`'s stream too), shutdown (2.3i,
  2.3k) and 130 after any O4 (a) interrupt (§3.6's end; §4 C, §8 rows 6, 8). §3.2-§3.7 cover every other installed command.
- **Unix.** For SIGINT, SIGTERM and SIGHUP (O4 (a)), `install` keeps an inherited ignored disposition (`nohup`, `trap '' INT`),
  else sets `sigaction` with `SA_RESTART`. The handler returns at once unless `getpid()` is the saved PID (a child between
  `fork` and `exec` runs it too, B's included); else it saves `errno`, writes one byte naming the signal (both ends
  close-on-exec, write end non-blocking, `EAGAIN` ignored), restores `errno`. SIGQUIT keeps its default (OD-C7 (b)).
- **Windows.** `SetConsoleCtrlHandler`. CTRL_C and CTRL_BREAK write a count byte and return `TRUE`; CTRL_CLOSE, CTRL_LOGOFF and
  CTRL_SHUTDOWN (O4 (a)) write a Force byte and sleep without end: Force ends the process, or the OS does a short time later
  **(OS)** (R9). A CTRL_C disabled at creation stays disabled. A process that loads `user32.dll` (the candidate's Pageant,
  `gwz-core/Cargo.toml:183, :203`) gets no LOGOFF or SHUTDOWN call; the OS ends it **(OS)** (R9).
- **Writer.** Once installed, one module thread owns stdout and stderr; `Output` is one FIFO to it (events in order; before
  install, `lib.rs:167, :174`, and in `hook` it writes directly). A sender waits on a full queue only until Cancel or Force,
  then event and progress lines are dropped (counted), never a notice or the final document. After a stdout write error
  `Output`'s stdout send returns `ConsumerGone` at once, `diff` and `log` taking their `BrokenPipe` stop
  (`gwz-cli/src/pager.rs:11-15`, `diff_exec.rs:271`, `log_exec.rs:204, :286`); other errors are ignored. `Output::drain()`
  (until all is written) precedes a spawn inheriting stdout or stderr (pager, `forall` member) and the claim holder's end. 3.1c
  moves every direct write onto `Output` and adds `gwz-cli/scripts/check_direct_output.py`, a `cfg`-blind token check in
  gwz-cli's CI failing `print!`, `println!`, `eprint!`, `eprintln!`, `dbg!`, `stdout()`, `stderr()` (std's, `Term`'s) outside
  gwz-cli's `src/interrupt/`, `src/hook/`, `src/hook_exec.rs`, 2.3k's `src/server_{exec,after,long}.rs` and test sources
  (`src/tests/`, `tests.rs`, `*_tests.rs`); 3.1c owns every other file it flags on its base. A panic message goes directly (R8).
- **Giving control back** (OD-C7 (a), O5). When Force begins, also when a foreground wait defers its later steps (§3.7), the
  reader restores each saved disposition (Windows: removes its handler); a later interrupt ends gwz by default (R14). The
  reader runs under `catch_unwind`; a panic in it restores the dispositions first.
- **Check.** The kind `signal` (`libc::signal`, `libc::sigaction`, `SetConsoleCtrlHandler`, `signal_hook`, `ctrlc`,
  `tokio::signal`), out of `env`, runs in §2's five runs with no new wiring; gwz-cli's allowlist gets this module's entry.
  2.3s's gwz-core half (the kind, its test; no gwz-core allowlist entry is `signal`/`sigaction` at `b745f1fa`) merges before
  its gwz-cli half (checked against gwz-core main); B's 2.3r-1 and 2.3r-2 follow 2.3s, adding `signal` entries (B §8 row 1).

### 3.2 First interrupt: Cancel

- **The slot.** `run` keeps one mutex-guarded slot shared with the reader: `Empty`, `Owner(HostOwner)` (plan 3.1),
  `Closing(HostClose)` or `Interrupted`. Cancel (or Force, if no Cancel came), under the mutex, turns `Owner` into `Closing`
  by `close()` (non-blocking, `host/owner.rs:21-32`) and `Empty` into `Interrupted`: admission is sealed, the host's
  operations cancelled, so running network I/O fails and later admission is refused, both `Cancelled` (plan 3.1, §8 row 4).
  The dispatch checks the slot before building the host and before its write frame: `Interrupted` refuses the request
  `Cancelled`, writing nothing (130, §3.6); an owner put into it is closed at once. After its write frame the main thread
  empties the slot, closing an owner still there, and awaits the `HostClose` outside any frame (`Complete`, `Pending`, §3.3).
- **`--server`**: the carrier-backed host closes the same way; the served core's response and bound are C's (until then the
  local rule). **No host** (ordinary build, transport off, native route): `Interrupted` only (R10). **Local work** runs on
  (`GwzCoreSessionDesign.md` §5.3; R7). **Wait:** Cancel starts `CLEANUP_BOUND` (5 s, `src/session_host/context.rs:80`); a
  command that returns first renders (§3.6) and exits 130; else Force.

### 3.3 What a cancelled member reports

- A member whose operation observed the cancel is `Failed` with `Cancelled`; one that failed first keeps its reason.
- **The cause survives a slow cleanup.** `terminate` returns `CleanupPending` after `CLEANUP_GRACE` (500 ms,
  `src/git/endpoint/https_auth.rs:28`; `owner.rs:253-276`), mapped to `Io` (`https_auth.rs:190`), losing `Cancelled` and a
  timeout's `Interaction` (`https_worker.rs:377`); B returns the lookup's own outcome (B §3.6; `runner.rs:226`) and 3.0e the
  file worker's `runner.check()` cause (`https_auth/file_worker.rs:91-98`), `CleanupPending` only with no other cause.
- **Pending cleanup is a finding, not the member's error.** B retains the tree in the endpoint's owner (`owner.rs:298`); the
  HTTPS adapter's disposal waits on `pending_cleanup_count()` (`src/git/endpoint/https_adapter/worker.rs:268, :340`), so the
  `HostClose` the dispatch awaits (§3.2) is `Pending` (contract :272-275); 3.1c then writes `gwz: cleanup still pending` to
  stderr, replacing `dispatch.rs:82-87`'s `gwz-alpha` line.

### 3.4 Force: the write gate and the forced exit

**The write gate** (`crates/write-gate/`, a gwz-core crate that builds and tests alone): a cloneable `WriteGate`, one per
process; a `ThreadId` state map under one mutex and condition variable; every frame and park is a guard restored on unwind.

| Call | Meaning |
|---|---|
| `writing(label, f)` | `f` is a write frame of this thread; nested frames are never refused; a thread's outermost frame is refused `Closed` before `f` if the gate is closed |
| `park()`, `unpark()`, each `-> Open \| Closed` | in a frame: `park` makes the thread parked (no local write; Force does not wait for it) or, on a closed gate, leaves it writing and returns `Closed`, the caller failing `Cancelled` without starting the parked phase; `unpark` makes it writing again, or, on a closed gate, records the frame's label and phase and stops the thread (G4) |
| `outside(label, f)` | `park`, `f`, `unpark`; refused before `f` if closed; on `Closed` after `f` the thread records its label and stops |
| `safe_point(label)`; `close()`, `is_closed()`, `wait()` | `safe_point`: if closed, records `label` as stopped and the thread stops; `wait` returns the stopped labels once every thread in a frame is parked, stopped or out of its frames |

A thread **stops** by blocking for good. **(G1)** A frame is parked only while bytes are in transfer or while it waits (a join,
a slot, a lock poll); a writing thread blocks only in a local write, a `Wait` child, or network I/O of its writing phase (a
clone before its first object, a fetch's pack trailer, a push's disconnect), which step 1's host close fails (no host: R10).
Exception: libgit2's removal of a clone whose transfer failed runs parked (R6). **(G2)** No stop with an in-process lock held or
inside a held Git lock file (a ref transaction, a filebuf); a callback (un)parks outside any shared lock (as the progress
sink's, `gwz-cli/src/progress_detail.rs:199`). **(G3)** After `close()` nothing new starts: outermost frames, parks and
`outside` are refused, queued members report `Cancelled`, and every serial member loop has a safe point at each member's head; a
refusal fails its sequence `Cancelled` there, keeping what it wrote before (§3.5's last row). **(G4)** After `close()` a parked
thread never writes again: its `unpark` stops it, so no continuation of a parked phase (a callback's abort path, a join's
rollback or manifest write) runs after `wait()`.

**Carriage, both builds.** 3.1c's `run` makes the process's `WriteGate` (a value it owns and passes, as the claim). 3.0d adds
`Git2Backend::with_write_gate(WriteGate) -> Self` (unconditional; a field beside `spawn_gate`, open by default; read by
`GitRepository::write_gate()`), which the ordinary build's dispatch backend takes; and, candidate-only, a `ProcessGate` field
and `ProcessGate::with_write_gate` (a copy; minting unchanged): `run` sets it on B's `process_safety_with_children()` gate,
and `with_process_gate` copies it into every request backend (plan 3.1, §8 row 4).

**Where frames open and park.**
- 3.1c's dispatch runs every request as `writing` except a read-only list, an exhaustive `match` over `CliRequest` with no
  wildcard (a new request is a writer until classified); a request is listed only if it writes no file (row 6.14).
- `par_map_per_host_gated(gate, …)`, new beside `par_map_per_host` (unchanged), parks for its join, runs each item's `f` (not
  the worker loop, so slot waits stay outside) as `writing`, and passes `is_closed` as `cancelled`; §2's seven calls switch
  to it with `backend.write_gate()`.
- **Fetch** (`fetch`, `tag_fetch`, `fetch_anonymous`): `download` parked, `disconnect` `outside`, then `update_tips` as a
  write (with the tag option and prune `Remote::fetch` uses). The transfer callback, now always set, unparks at its first
  call with every object received, so the pack commit and `.git/shallow` are writes; on a closed gate it stops (G4), leaving
  the temporary pack (R4). A download that returns still parked (no pack) unparks; a refused `download` park fails the member
  `Cancelled` before it contacts the remote.
- **Clone**: a write frame. The `remote_create` callback writes the pins (moved from :73, so every clone that reaches a
  checkout is pinned); the transfer callback parks while objects remain and unparks once all are received, so the pack commit
  and the checkout, with its safe point, are writes; a clone that never calls it (an empty remote) stays writing (G1). On a
  closed gate the unpark stops it, leaving the partial clone (R6).
- **Push**: parked; the first `push_update_reference` unparks, so the tracking-ref update is a write; on a closed gate it
  stops, the tracking ref stale (R5). A push to a local-path remote (§2) is a write throughout, as its upload writes the
  remote repository: Force waits for it (R8 if hung).
- `ls_remote`, `read_remote_file` (R16) and the family-lock poll are `outside` whole. Member-lock waits are on the served
  path only (`src/session_host/served.rs:81`): C's.

**Force**, on the reader thread, in order: (1) restore the dispositions (§3.1); close the host if Cancel did not (§3.2); (2) the
first Force notice (§3.6); (3) `close()` the gate; (4) B's `kill_killable()` (none without a registry): every `Kill` child ends
now and a later `Kill` spawn is refused `Closed`, its member reporting `Cancelled`; (5) `wait()`, unbounded, the writer printing
each stopped label as recorded; (6) the claim (§3.6): taken if free, else Force waits as §3.6 says; (7) the output (§3.6); (8)
up to 1 s for the writer, then §3.6's end without unwinding (Windows: `TerminateProcess` on itself). A calls no `close()` of
B's: every `Wait` child runs in a write frame, which `wait()` waits for, and G3 and G4 start nothing after step 5. **Network
work ends at once:** step 1's close fails the host's I/O; a transfer still running (R10) is parked, not waited for, and ended by
step 8 (R4, R6). **Local writes are waited for** to a safe point or their end, a `Wait` child (`git commit`, `git tag`, hooks,
signer; B OD-B1 (a)) without a bound (decision (ii); R8).

### 3.5 Long local phases and their safe points

| Phase | Safe point | What a stop leaves |
|---|---|---|
| checkout outside a ref transaction: `refs.rs:33, :134`, `repository.rs:56, :81`; `reset_hard` (a `CheckoutBuilder` passed to `reset`); stash apply and pop (`checkout_options` in `stash_restore_options`, `stash_support.rs:18-26`) | the progress callback, after each complete file | earlier files new, later old; index and HEAD old; a pop's stash kept (R1) |
| the clone's checkout (`transport.rs:41`) | the progress callback | new HEAD, no index, earlier files only (R6) |
| the five writes inside a ref transaction (§2): four checkouts, the expected-conflict `repo.merge` | none (G2): run to their end | nothing; R8 if hung |
| copy: `local clone` (`handle_local.rs:72`), family import (`family_merge.rs:424`), today `NeverCancelled` | one adapter implementing both `Cancellation` traits, answering `is_closed()` | whole files; a `creating` lane or partial import (R2) |
| stage: `add_all`'s matched-path callback (today `None`, `repository.rs:112`); the per-path loop | the callback; each iteration | loose objects only; index unchanged (R3) |
| serial member loops: `handle_commit.rs:179-186` and any other 3.0f's inventory finds in `src/workspace_ops/` | each member's head | earlier members done, later not (R7) |
| `stash_save` (`stash.rs:18`, `preservation.rs:344`); a Git CLI route commit or tag | none: run to the end | nothing; R8 if hung |
| a refusal inside a running sequence (G3): a fetch's `disconnect` (`outside`) after its pack commit; a clone's first transfer park; a join's park | the refusal | fetch: a complete new pack and `.git/shallow`, refs and `FETCH_HEAD` old (R18); clone: none (libgit2 removes it, `clone.c:666-673`, the thread still writing); join: no item starts, the request's earlier writes kept (R7) |

A stopped copy runs its error path to the frame's end. A sequence that opens a nested frame (a stage, then its commit) runs
on through it. Every write sequence the table does not name runs to its end, writing a bounded set per member.

### 3.6 Exit status and output (OD-C4)

- **Status** (O4 (a), OD-C4). **130**, as a calling shell reports it, on every platform, over 0, 1, 2
  (`gwz-cli/src/globalargs/render_exit.rs:82-95`) and another signal's death or `STATUS_CONTROL_C_EXIT`, on the response and
  error paths, `diff`, `log`, `forall`, if an interrupt preceded the claim (a later one changes neither); not `hook`,
  `claude-code-setup` (R12); a `server` subcommand's is C's. **The end**: after a first SIGINT on Unix, `raise(SIGINT)` under
  the default disposition, so a shell sees a SIGINT death ($? 130) and ends its script as on 1.0.18 (bash's
  wait-and-cooperative-exit (ext)); else `exit(130)`, Windows too (its shells get the console event themselves (ext), row 6.11).
- **The claim.** `OutputClaim` (one atomic, owned by `run`) is taken once: by the dispatch (main) thread as the last act in its
  write frame (a read-only request: at its return; `diff` and `log`, which run outside dispatch, `gwz-cli/src/lib.rs:226-285`:
  at their first stdout write), or by Force at step 6 if free; as `wait()` returns only after that frame ends, a finished
  dispatch holds it first. Its holder alone writes stdout's final document and drains `Output` before its end (unbounded with no
  interrupt, as today, else ≤ 1 s); a main thread that fails to take it blocks until Force exits. If the dispatch holds it,
  Force waits up to the close bound plus 1 s for the main thread to await the host's close, render and end, then ends (R13).
- **When the dispatch holds the claim**, the main thread renders as usual, with the first own entry of `errors` (after the
  member copies; `ErrorCatalog.md:35`) `{"code":"Cancelled","message":"interrupted","member_id":null,
  "member_path":null,"target_kind":null,"detail":null,"record_context":null}`; `aggregate_status` as today. An execution
  error's top-level `errors` is [that entry, its own error].
- **When Force takes it**, `--json` gets the top-level shape with `meta: null`, no members and the one `Cancelled` entry,
  `message` `interrupted; forced exit, see stderr for what stopped`; `--jsonl`, human and porcelain get nothing more on
  stdout, so a `--jsonl` stream has no final response (R13).
- **Notices** (the writer, stderr, every output mode): at Cancel `gwz: interrupted; cancelling (interrupt again to stop now)`;
  at Force `gwz: forced exit; network work ended, waiting for local writes to finish or reach a safe point (a further interrupt
  ends gwz at once and may leave a partial write)`, then `gwz: stopped: <member path> (<phase>)` per label, a push after the
  report adding `: the remote received it; tracking ref not updated`, `gwz: transfer ended: <member path> (fetch|push|clone)`
  per parked transfer, a push adding `; the remote may have received it`; `gwz: <n> progress lines dropped` when any were.

### 3.7 The pager and `forall` (OD-C6)

A foreground program (the pager from its spawn to its exit, `gwz-cli/src/pager.rs:107-115, :138-145`; a `forall` member's
`status`, `gwz-cli/src/forall.rs:260`) gets the terminal's interrupt; gwz never signals it. `Interrupts::foreground(f)` wraps
that span: no bound runs; Force's step 1 runs at once (a further interrupt then ends gwz by default, R14), its later steps
waiting for `f`. On Cancel, `diff` and `log` stop feeding at the next chunk (a blocking feed, `pager.rs:123-128`, waits until
the pager reads, or quits: `EPIPE`), close the pager's input and wait; `forall` finishes the current member, starts no
further one, appends `interrupted: <k> of <m> members not run` and exits 130.

### 3.8 gwz-py (resolution (2) and OD-C4; step 3.2b)

- **The bridge.** `_run_native` (`gwz-py/src/gwz/bridge.py:298-316`) today only waits on `CancelledError` (:28-35); a network
  call registers `op_<request_id>` on its worker thread (`gwz-py/native/src/client_host.rs:226-231`; `shims.rs:79-81`;
  `client_host/operations.rs:156`), and `cancel` refuses an id not yet live (:199-208). 3.2b adds three `ClientHost` methods
  beside `cancel_operation` (`client_host.rs:164`), no module function (`native/src/lib.rs`, `shims.rs` unchanged):
  `operation_id(request_id)` over the crate's `shims::operation_id`, `cancel_call(id)` and `forget_cancel(id)`.
  `NativeCoreBridge.call` (`bridge.py:318`) passes `_run_native` the call's operation id; on the first `CancelledError`,
  `_run_native` calls `cancel_call(id)` on its own thread, shielded: a live operation is cancelled as by `cancel`; an id not
  yet live is recorded, and its registration consumes the record and is refused `Cancelled`, as
  `gwz-py/dev-docs/GwzPyPerOperationTransportDesign.md:104` says; when `_await_completion` returns, `_run_native` calls
  `forget_cancel(id)`, so the record holds one entry per cancelled call still awaited and needs no capacity (unlike
  `operations.rs:28`'s completed-cancellation record). It then re-raises `CancelledError`, carrying the decoded response, if
  any, as `gwz_response`; a library caller still sees `CancelledError`; `Client.cancel_operation` keeps its refusals. No
  handler: 3.2b's test beside `test_process_globals.py`'s parses `gwz-py/src/gwz/` (`ast`), failing `signal.signal`,
  `signal.set_wakeup_fd`, `add_signal_handler` (`asyncio.run`'s own handler is Python's, used below).
- **The gwz-py CLI.** `main` catches only `CliUsageError` and `GwzError` (`cli.py:142-156`). `run` (:76) catches
  `CancelledError` around the command and renders the carried response (members as completed, the `Cancelled` entry first among
  its own errors, as §3.6) or, with none, `GwzBridgeError("interrupted", code="Cancelled")` as `main`'s error clause does, and
  returns **130**. On 3.11+ the first SIGINT cancels the main task (`asyncio/runners.py:150-157`) and `asyncio.run` returns its
  value (:117-124); on 3.10 its `finally` cancels the task (the same path), then raises `KeyboardInterrupt` (`runners.py:44-49,
  :55-63`). `main` catches `KeyboardInterrupt`, renders unless `run` did (a flag `main` passes), then after an interrupt raises
  it on Unix (§3.6), so CPython ends by SIGINT (3.11 `test/test_signal.py:164-176`), and returns 130 on Windows (row 6.16).
  Later SIGINTs end the interpreter (R11); SIGTERM, SIGHUP, CTRL_BREAK, CTRL_CLOSE keep Python's default (R15).

## 4. Interfaces with B, C and D

**B points A relies on** (B revision 3, on disk; cited by section, as it is revised in parallel): (1) `kill_killable()` ends
every live `Kill` entry at once, leaving `Wait` and `OwnInput` children, refuses later `Kill` spawns `Closed` (B §3.7, row B22),
reached through `ProcessGate::kill_killable()` (B §3.1; a no-op without a registry); (2) gwz-cli calls no `close()` (B §3.7,
citing A revision 2's :179-184, now §3.4 "Force" step 4 and "A calls no `close()`"); (3) `run` creates the registry before any
thread, so before A's install (B §4 3.1; OD-B3 (a)); (4) `process_safety_with_children()` mints the `ProcessGate`,
`with_process_gate` carries it to every backend (B §3.1, §4), A's write-gate field inside; (5) `terminate` returns the lookup's
own outcome and retains the tree as pending cleanup, reported as the host close's `Pending` (B §3.6, citing §3.2, §3.3); (6) B
changes no disposition in gwz's process; 2.3r-1 and 2.3r-2 merge after 2.3s, adding entries under `signal` (B §4, §8 row 1); A's
PID check covers B's child before `exec`; (7) R-B4 covers a `git commit` torn by R14, R9 or SIGKILL, R-B13 the ordinary build
(R17); (8) A §8 row 7's totals sentence supersedes B §8 row 4's totals clause, as B's row 4 says. **C**: the server lifecycle
mode for every `server` subcommand (§3.1): shutdown, stop input, each command's output and `server stdio`'s protocol stream (no
writer runs), any registry before `install`, §3.6's end with 130 after an interrupt from any O4 (a) source (SIGINT, SIGTERM,
SIGHUP; CTRL_C, CTRL_BREAK, CTRL_CLOSE, CTRL_LOGOFF, CTRL_SHUTDOWN); a served core's response to a `--server` host's close and
its bound. **D**: §7's recognizers, a push's remote effect in JSON (R5), the "beyond" rows and §9's narrowings.

## 5. Steps

2.3s is Phase 2 (wave 2, lane 2, after 2.3f, before 2.3k); the rest Phase 3 (§8 row 1; ≤3 compiling lanes), phase-reviewed
(3.5). Sizes: code + rows. No trunk file (:325); `refs.rs`, `repository.rs` go through the Phase 5 lane (:329-331).

| Step | Content | Owns | Size | After |
|---|---|---|---|---|
| **2.3s** (gwz-core check, merged first; then gwz-cli with the root, one root commit) | §3.1: module, handlers, reader, writer and `Output`, `Interrupts`, server lifecycle mode, re-exec harness; no call from `run`; the `signal` kind | create `gwz-cli/src/interrupt/`; edit `gwz-cli/src/lib.rs` (the `mod` line), `gwz-cli/Cargo.toml` (`libc` on Unix, `windows-sys` on Windows), `gwz-cli/scripts/process_globals_allowlist.json`, `gwz-cli/BUILD.bazel` deps (:34-40), root `Cargo.lock` and `MODULE.bazel` (the hand-pinned `@crates` specs, :15-17, :105-129; `gwz-core/scripts/checks/check_bazel_pin_drift.py` reads only `[dependencies]`, :181, so the Bazel build checks the `cfg` ones), `scripts/checks/{check_process_globals,test_check_process_globals}.py` | ~300 + 300 | 2.3f (lane); before 2.3k, 2.3r-1, 2.3r-2 |
| **3.0d** (gwz-core) | `write-gate`; carriage (both builds); `par_map_per_host_gated`; fetch, clone and push frames | create `crates/write-gate/` (its `BUILD.bazel`); edit `Cargo.toml`, `Cargo.lock`, `BUILD.bazel` (one label), `src/transport_host/composition.rs`, `src/git/gitbackend/{backend,contract,transport,transport_support}.rs`, `src/operation/par_map_per_host.rs`, §2's six caller files, `src/local_clone/wait.rs`, `scripts/checks/local_clone_inventory.json` (LBT-001, `check_local_clone_boundaries.py:17`), `scripts/checks/check_crate_versions.py` (`EXPECTED_CRATES` :85, bumped after B's 2.3r-1; it sets the publish order, :198) | ~470 + 500 | Phases 1, 2 |
| **3.0e** (gwz-core) | the file worker's cause (§3.3) | edit `src/git/endpoint/https_auth/file_worker.rs` | ~30 + 200 | Phases 1, 2 |
| **3.0f** (gwz-core) | §3.5's safe points; the member-loop inventory | edit `src/git/gitbackend/{stash,stash_support,transport}.rs`, `src/workspace_ops/{handle_local,handle_commit}.rs`, `src/local_clone/family_merge.rs`, loops found; `refs.rs`, `repository.rs` via Phase 5 | ~200 + 400 | 3.0d |
| **3.1c** (gwz-cli) | install for every command; the slot, bound, Force; claim, marking, notices, 130; every write through `Output` and its check; read-only list; pager, `forall`; the `WriteGate` in `run` | in `gwz-cli/`: edit `src/lib.rs`, `src/globalargs/dispatch.rs`, `src/pager.rs`, `src/forall.rs`, `src/diff_exec.rs`, `src/diff_render.rs`, `src/log_exec.rs`, `src/clirequest/invocation.rs`, `src/append_branch_summary/machine.rs`, `src/progress_detail.rs`, `src/interrupt/`, every file the check flags on its base, `.github/workflows/ci.yml`; create `scripts/{check_direct_output,test_check_direct_output}.py` | ~520 + 560 | 3.1, 3.0d, 3.0e, 3.0f |
| **3.2b** (gwz-py) | §3.8; the Python handler test | edit `gwz-py/src/gwz/{bridge,cli}.py`, `gwz-py/native/src/client_host.rs` (the three methods), `client_host/operations.rs`, `gwz-py/src/tests/test_process_globals.py` | ~130 + 280 | 3.2 |

## 6. Rows

Each is seen red first; each runs on macOS, Linux and Windows (M, L, W) unless a leg is marked; W's console events come from
`GenerateConsoleCtrlEvent` in a child console. A row needing a seam runs `run` in a child re-executed from gwz-cli's unit-test
binary (the module's `cfg(test)` harness, inside its `cfg_if!`; nothing under `tests/`). A row's 130 is a shell's (§3.6).

| Row | Step | What it shows | Red against |
|---|---|---|---|
| 6.1 | 2.3s | Each source gives Cancel then Force (M L: SIGINT, SIGTERM, SIGHUP; W: CTRL_C, CTRL_BREAK); an inherited ignored SIGINT or SIGHUP stays ignored (M L); a CTRL_C disabled by `CREATE_NEW_PROCESS_GROUP` stays disabled (W); SIGQUIT keeps its default (M L); children forked in a loop and one SIGINT to the group give one Cancel (M L); in server lifecycle mode the reader only delivers events and restores the dispositions at Force; a reader panic restores the dispositions | no module; no PID check |
| 6.2 | 2.3s, 3.1c; any | The check fails a `sigaction` or `SetConsoleCtrlHandler` outside gwz-cli's module in each of the five runs; [3.1c] the direct-output check fails a `println!`, a `dbg!` and an `io::stdout()` outside its allowed files, also in a `cfg(windows)` branch on Linux, and passes `src/tests/` | no `signal` kind; no check |
| 6.3 | 2.3s; 3.1c | Through `Output`, events and the final document keep their order; with a consumer that stopped reading, a sender blocks before Cancel or Force, never after; no notice or document is dropped; after a stdout write error a send returns `ConsumerGone` at once; `drain()` returns once all is written; [3.1c] `gwz log \| head -1` on a 50,000-commit fixture and `gwz diff --no-pager \| head -1` on a 10,000-file change each exit 0 within 2 s, the core spool stopping (`ConsumerGone` observed); 1,000 `forall` members each writing a stderr line: every banner precedes its member's output; a `--json` command whose stdout is read slowly: the document is complete | direct writes; write errors ignored; no drain |
| 6.4 | 3.0d | `wait` returns at once with no frame, waits for a writing thread until its `safe_point`, not for a parked one; after `close()` outermost frames and parks are refused, a nested frame is not; a parked thread whose `unpark` follows `wait()` blocks with nothing written after it; a frame unwound by a panic leaves the thread out of it | no crate; `unpark` writing again |
| 6.5 | 3.0d | `wait` waits for a gated worker in `update_tips`, not in `download` or a slot wait; two same-host members, per-host 1, member 1 stopped at a safe point: `wait` returns in 1 s, member 2 `Cancelled`; member A's fetch stopped at its unpark while member B's clone, in its writing phase, emits a route note through a sink holding a mutex: `wait` returns | a looped frame; an unpark inside the sink's lock |
| 6.6 | 3.0d, 3.1c | The split fetch writes the refs, tags, `FETCH_HEAD` and prunes `Remote::fetch` does; Force at a transfer's end, in its pack commit or shallow write or before `disconnect` (R18) leaves no `shallow.lock` or `*.idx.lock`, the next fetch succeeding; Force after a clone's transfer, after `builder.clone` or in an empty remote's clone leaves no member, a pinned one or R6's; a refused `download` park is `Cancelled`, no fixture contact; [3.1c] two interrupts during a four-member `materialize` and a four-source `init`, a seam delaying step 8 by 2 s: `gwz.toml` and `gwz.lock` both as before or both new, each member absent, a complete pinned clone or R6's, nothing changed after `wait()`, kept clones as R7 names them | `download` parked whole; late pins; a rollback after `wait()` |
| 6.7 | 3.0d | A native-route three-member push to a bare file remote, Force in member 1's pack write, ref write and tracking-ref update: no `*.lock` in the member or the remote, whose `git fsck` passes with the push whole or absent; members 2 and 3 never contact the remote (its log) and report `Cancelled` | a parked local-path upload; no refusal |
| 6.8 | 3.0e, 3.1c; W, and M L with the f9 slow-drain fixture | A cancel whose tree outlasts `CLEANUP_GRACE` returns `Cancelled` from the helper and the file worker; through the HTTPS worker the member is `Cancelled`, a timeout keeps `Interaction`; after a gwz-cli Cancel the `HostClose` the dispatch awaits is `Pending`, stderr has `gwz: cleanup still pending`, the reap follows within 30 s | `CleanupPending`; `Io`; a `HostClose` nobody awaits |
| 6.9 | 3.0f; both builds | Force at file k of an N-file checkout, `reset_hard` and stash apply (child process): each file wholly old or new, index and HEAD unchanged, `wait()` returns the label naming member and phase; Force during a fast-forward merge's checkout and both recovery checkouts: each completes, no `refs/**/*.lock`, the next `gwz merge --abort` succeeds | no callback; a safe point inside the transaction |
| 6.10 | 3.0f | Force during `local clone`'s copy of a file over one chunk: every file complete, no temporary; in a family import: stops before a participant; in `stage` (both forms): index unchanged; in member 1's 8 s hook of a three-member `commit`: member 1 commits, members 2, 3 and the root do not start | `NeverCancelled`; no loop point |
| 6.11 | 3.1c | Each source (M L: SIGINT, SIGTERM, SIGHUP; W: CTRL_C, CTRL_BREAK) on `gwz --json --max-per-host 1 fetch` of two members on one host, the first stalled: exit 130 within the bound, both `Failed` with `Cancelled` (the second refused admission), the entry first among own entries; after every member succeeded: `Ok`, exit 130, the entry; one interrupt while a seam holds the dispatch before the slot's fill: `--json push` of two members never contacts the fixture, both `Cancelled`, 130, and `switch` changes no file; M L: `bash -c 'gwz --json fetch; touch after'` and a two-pass `for` loop of it, SIGINT to the process group: `after` absent, the second pass not run, bash's own status 130 (a death by SIGINT); W: the same call in a `cmd /c` batch file and a PowerShell script, CTRL_C: gwz exits 130 and the row records whether each script went on (§9); M L, ordinary build: SIGINT during `pull`'s HTTPS root fetch on the main thread: the member `Cancelled` or R10's `EINTR` network error, the `Cancelled` entry first | `exit(130)` after SIGINT, a signal death after SIGTERM or SIGHUP (M L); an empty slot ignoring Cancel |
| 6.12 | 3.1c | A second interrupt during a stalled download: exit 130 within 1 s, `transfer ended`; `--json` stdout is the forced envelope, `--jsonl` has no response, human stdout is empty; stdout a pipe nobody reads and stderr closed: exit 130 within 2 s, no panic; `gwz --jsonl pull` whose consumer stops reading after one event, then SIGTERM (W: CTRL_BREAK): exit 130 within the bound plus 1 s; Force during a network push's `push_update_reference`: stderr names the member and says the remote received it, the tracking ref old; Force at file k of row 6.9's checkout: the notice names member and phase | no Force; sinks write directly; a G4 stop with no label |
| 6.13 | 3.1c | `--json commit`, an 8 s hook, one interrupt (Force at the bound): one member: Force waits, stdout is the full response (member `Ok`, the entry), exit 130, also in 100 runs with a second interrupt at the hook's exit; three members: member 1 commits, stdout is the envelope, stderr names the stop; 100 runs with the main thread ending its frame after Force's claim: always 130; an interrupt during the wait ends gwz by default, also 50 ms after the second (R14) | claim taken at Force's start, or after the frame |
| 6.14 | 3.1c | Each read-only request leaves all fixture files' bytes and mtimes as they were | a writer listed |
| 6.15 | 3.1c | A `forall` member past the bound still runs, no further member starts, exit 130; a member that ignores SIGINT and three interrupts to gwz (M L: SIGTERM; W: CTRL_BREAK): the third ends gwz, the member runs on (R14); `gwz diff` of 1 MiB into `less` (W: `more`), two interrupts at page 1: gwz lives until the pager exits, then exits 130; W: CTRL_CLOSE runs Force before the OS ends the process **(OS)** | no foreground rule; a scope opened after feeding |
| 6.16 | 3.2b; 3.10 and 3.11+ | Cancelling a task awaiting a transport-route fetch calls `cancel_call` first, the call ending `Cancelled` within the bound; with `max_workers=1` and one blocking call, 200 queued fetches cancelled together never contact the fixture; a cancelled local call, then a fetch with its `request_id`: the fetch runs; local and native-route calls wait to their end; the exported id equals the registered one; the gwz-py CLI's `--json` exits 130 with `Cancelled` on SIGINT and, for a three-member local `commit`, shows what committed; SIGINTs 50 ms apart during a native checkout record R11's count (3 on 3.10, 4 on 3.11); the source test fails a planted `loop.add_signal_handler`; M L: `bash -c 'gwz-py --json fetch; touch after'`, SIGINT to the process group: `after` absent, bash's status 130 (a death by SIGINT); W: the CLI exits 130, not CPython's `STATUS_CONTROL_C_EXIT` (3.11 `test/test_signal.py:214-226`) | `_run_native` only waits; a capped record; no test; `return 130` on Unix |

## 7. Residuals (for design D)

"Covered by" names the D-V clause whose mechanism leaves the residue (resolution (1)'s safe points; decision (i)'s network
"at once" or hung write; an OD or operator answer). "Beyond": none covers it; D puts it to the operator.

| # | What can remain | When | How it is recognized now | Covered by |
|---|---|---|---|---|
| R1 | a partly checked-out tree; index and HEAD old; a popped stash kept | Force at a checkout safe point | the notice; D | (1) |
| R2 | a `creating` lane or partial family import (whole files) | Force at a copy safe point | the notice; D | (1) |
| R3 | unreachable loose objects | Force during stage | none needed (`gc`) | (1) |
| R4 | libgit2's temporary pack in `objects/pack` | Force while a pack is received or freed | `transfer ended`; D's sweep | (i) at once |
| R5 | an unknown remote effect; or the remote has the push and the tracking ref is stale | Cancel or Force during a network upload; Force after the remote's report | the member's `Cancelled` and the catalog's remedy (`ErrorCatalog.md:35`); Force's `may have received it`, or, for a push stopped after the report, `the remote received it; tracking ref not updated`; D's JSON | (i) at once |
| R6 | a partial new member directory: a stopped transfer's; a clone stopped in its checkout (new HEAD, no index, some files; a later `stage` would record deletions); libgit2's removal of a failed clone cut by step 8 | Force during a clone | the notice names `clone`; the next clone refuses a non-empty target (`transport.rs:32`), so re-materialize needs the directory removed; D | (i) at once; (1); the cut removal beyond |
| R7 | earlier members done, later not; at a `materialize` or init-from-sources join G4 stopped, the complete fresh clones the F2 rollback would remove (`apply.rs:178-183`, `handle_init_from_sources.rs:227-231`), and for init no `gwz.toml` or `gwz.lock` (:236) | Cancel during pull or materialize; Force between members, or Force before such a join's `unpark` | the response (`Partial`, members `Cancelled`), the stop notice or the forced envelope; re-running pull completes; a re-run's clone refuses a kept clone's non-empty target (`transport.rs:32`) until it is removed; D | beyond ("no partial state") |
| R8 | none; gwz waits without a bound | a hung `Wait` child, hook or signer (R-B10), `stash_save`, a write in a ref transaction, a local-path push, a write blocked in the filesystem, a panic message on a stalled stderr | the first Force notice; `ps`; SIGKILL, a further interrupt (R14) or Windows ending the process (R9) ends it | (i); O5; the panic case beyond |
| R9 | SIGKILL's residues | Windows ends the process after CTRL_CLOSE, LOGOFF or SHUTDOWN **(OS)**; LOGOFF and SHUTDOWN never reach a process that loaded `user32.dll` (the candidate build) **(OS)** | as SIGKILL's; D | OD-C7 (c); O4 (a); LOGOFF and SHUTDOWN under `user32` beyond **(OS)**, a narrowing of O4 (a) (§9) |
| R10 | a network call runs on after Cancel; Force waits on a stalled native read between a pack's last object and its end, in a clone's negotiation or a push's disconnect; in the ordinary build an interrupt the kernel gives the main thread (ext) fails a libgit2 socket wait there at once with `EINTR` (gwz sets libgit2's timeouts, `transport_support.rs:21-28, :62-69`, so it waits in `poll`, which `SA_RESTART` does not restart, `libgit2/src/libgit2/streams/socket.c:155, :282-296, :318-330`): the member reports that network error, not `Cancelled` | no host: native route, ordinary build, transport off | Force at the bound ends a parked call; the leading `Cancelled` entry (row 6.11) | beyond |
| R11 | a gwz-py local or native-route call runs to its end; on 3.10 a SIGINT inside a task step skips the cancel; the 3rd SIGINT (3.10) or 4th (3.11: `runners.py:129`, `:71`; `concurrent/futures/thread.py:31, :37`) ends the interpreter mid-native-write: a torn checkout, `index.lock` or a ref `.lock` | gwz-py | `_await_completion` (unchanged); D's `*.lock` report | beyond (beside OD-C7 (d); a `cli.py`-only handler would need OD-C1's exception and 3.2b's test to allow it) |
| R12 | the OS default: gwz dies by the signal | `hook`, `claude-code-setup` (a `server` subcommand is C's, §3.1) | as SIGKILL's; D | beyond |
| R13 | no complete final document: a `--jsonl` stream without its response; a document cut at step 8 | Force taking the claim; a stalled stdout or host close past Force's wait | exit 130 without a complete document | beyond (OD-C4 marking) |
| R14 | a write torn by the default disposition; an orphaned pager or `forall` command | an interrupt after Force began, however soon, routine in `log` and `diff` as a third Ctrl-C used inside the pager (a search abort, leaving follow mode); SIGQUIT at any time | a death by SIGTERM, SIGHUP or SIGQUIT (or a core); after a first SIGINT a death by SIGINT, as §3.6's end, so only a missing `--json` document or Force's output tells it; B's R-B4; the program still on the terminal | OD-C7 (a), O5; (b); the in-pager case for D (§9) |
| R15 | a gwz-py CLI write torn, no cancel | SIGTERM, SIGHUP; Windows CTRL_BREAK, CTRL_CLOSE | death by signal; D | OD-C7 (d) for SIGTERM, SIGHUP; the Windows events beyond (OD-C1) |
| R16 | a `tempfile` directory with a partial clone in the system temp directory | Force during `read_remote_file` (`transport.rs:718, :740`) | D's temp sweep | (i) at once |
| R17 | a registry-less `git commit` or `git tag` and its hooks reached by the terminal's interrupt mid-write | the ordinary build | B's R-B13 | OD-B2 (a) |
| R18 | a fetch's complete new pack and `.git/shallow`; refs and `FETCH_HEAD` old | Force after a fetch's transfer, before its `disconnect` is admitted (§3.5) | the member's `Cancelled`; the next fetch completes (row 6.6) | (i) at once |

## 8. Superseded or amended clauses (applied by the lane owner at acceptance)

| # | Document and clause | Replacement or addition |
|---|---|---|
| 1 | Plan §4 graph, Phase 3 (:315) | `Phase 3  ─ {3.0 ∥ 3.0b ∥ 3.0d ∥ 3.0e ∥ 3.1 ∥ 3.2} ; 3.0d → 3.0f ; {3.1, 3.0d, 3.0e, 3.0f} → 3.1c ; 3.2 → 3.2b ; {3.0, 3.0b, 3.1c, 3.2b} → 3.3 → 3.4 → 3.5 (review)   (after Phases 1 and 2)` |
| 2-3 | Plan (2) Phase 3 heading (:712); (3) §4, after 3.0b (:717-720) | (2) "(three lanes then one, 19–23 lane-days)" for "(two lanes then one, 11–13 lane-days)"; (3) bullets 3.0d, 3.0e, 3.0f, 3.1c, 3.2b with §5's content, sizes and "After", citing this design's §5 |
| 4 | Plan 3.1 (:721-728) | Append: "The command's `HostOwner` sits in a mutex-guarded slot the interrupt reader closes on Cancel (an interrupt before the slot's fill refuses the request), the dispatch awaiting the returned `HostClose` after its write frame (GwzCancelInterruptDesign §3.2); a request refused `AdmissionError::Closing(HostClosing)` or ended by that close reports `Cancelled`; every request backend takes the process gate, with the write gate `run` sets on it, through `with_process_gate` (§3.4)." |
| 5 | 2.3 plan 2.3i (:186-188) | "SIGINT/SIGTERM and Windows Ctrl-Break through a handler that sets a flag only" → "stop requests from a stop input its caller passes (gwz-cli's interrupt module in its server lifecycle mode, GwzCancelInterruptDesign §3.1; gwz-core installs no handler, OD-C1)"; red row "a second signal or `force`" → "a second stop request or `force`" (the signal leg moves to 2.3k) |
| 6 | 2.3 plan 2.3k (:207-217) | Add: "the run-own-lifecycle branch calls design A's `install` (2.3s) in lifecycle mode for every `server` subcommand (`start`, its background copy, `stop`, `status`, `list`, `stdio`), passes 2.3i the stop input from `Interrupts`, owns shutdown and the commands' output, and after an interrupt ends by design A's end with 130 (GwzCancelInterruptDesign §3.6; plan §10 D-V, O4 (a)); red rows: SIGINT, SIGTERM, SIGHUP (W: Ctrl-C, Ctrl-Break, console close) stop a foreground server and `server stdio` and interrupt `server stop` during its detached wait, each exiting 130; a second SIGINT during the detached wait still removes the socket and clears the record". "After" adds 2.3s |
| 7 | 2.3 plan :71, §3, §4 (:240, :249-250) | Add after 2.3l (:220-230) "**2.3s Interrupt source (single, Surface).** GwzCancelInterruptDesign §3.1, §5, rows 6.1-6.3; ~300 + 300; its gwz-core half merges first, then gwz-cli with the root"; wave 2 lane 2: "2.3f listener files, then 2.3s interrupt source"; totals: one absolute sentence, B §8 row 4's figures plus 300, 300 and one step (at B revision 3: "about 5,580 implementation lines and 6,120 rows across 15 steps"), superseding B §8 row 4's totals clause whichever row is applied first, and :71's "about 4,300 implementation lines" takes the same figure; 2.3r-1 and 2.3r-2 after 2.3s (B §8 row 1) |
| 8 | `MachineOutput.md` Exit Codes (:954-964) | Add row "interrupted (Ctrl-C, SIGINT, SIGTERM, SIGHUP; Windows Ctrl-C, Ctrl-Break, console close, logoff, shutdown) before the response was claimed, or a `server` subcommand stopped by one, when gwz exits by itself; not `hook` or `claude-code-setup` \| `130` as a shell reports it (on Unix after Ctrl-C or SIGINT, a death by SIGINT), whatever the aggregate" and a paragraph "Interrupted commands" with §3.6's entry, its place, the forced envelope, the `--jsonl` rule and the notices |
| 9-12 | MachineOutput: (9) Notes (:280-287); (10) JSONL (:669-673); (11) :142-143; (12) :66-69, "and one error entry" | (9) Append: "Unlike `note:` lines, the interrupt and forced-exit notices are written to stderr in every output mode." (10) Append: "A command forced to exit after an interrupt (exit 130) may end the stream without that final response." (11) Append: "After an interrupt the first of these is the `Cancelled` entry with no member." (12) "and one error entry (two after an interrupt: the `Cancelled` entry, then the error)" |
| 13 | `gwz-core/docs/ErrorCatalog.md:35` | "A result's first error;" → "A result's first error of its own (in a response's `errors`, after the copies of member errors);" and append to the remedy: "An interrupted gwz-cli or gwz-py CLI command adds one `cancelled` entry with no member and exits 130." |
| 14 | `GwzCoreSessionDesign.md:687` | Append: "gwz-cli's interrupt no longer tears a write it is running (GwzCancelInterruptDesign §3.4); a detached worker's exposure stands." |
| 15-16 | (15) `gwz-py/dev-docs/GwzPyPerOperationTransportDesign.md` §2.5 (:103-106); (16) `gwz-py/README.md:225` | (15) Add: "**A caller's cancellation of an awaiting call** first cancels it through `cancel_call`, which also refuses a not-yet-registered one (one record per awaited call, forgotten at its return), then waits and re-raises `CancelledError` with the decoded response, if any, as `gwz_response`; the CLI renders it and exits 130 (cancellation design A §3.8)."; (16) Append: "Cancelling the task that awaits a transport-route call cancels its operation first; the `CancelledError` carries any response as `gwz_response`." |

The helper-owner contract's `CleanupPending` clauses are history and stay unedited (AgentProcessRules L1-33).

## 9. Decisions

All recorded, none open. D-V :1055-1062: **(i)** A3, §3.4, R8; **(ii)** A4, §3.4; **(iii)** O1 §3.1, §5; OD-C7 (a)-(d) §3.1,
§3.8, R9, R14, R15. D-V :1063-1069: **O4 (a)** A3, §3.1, §3.6 "Status", §4 C, §8 rows 6, 8, R8, R9, rows 6.1, 6.11, 6.12, 6.15;
**O5** A3, §3.1 "Giving control back", §3.6's Force notice, §3.7, R8, R14, rows 6.13, 6.15. **Decided by the operator (2026-11-03):**
§3.6's end as O4 (a)'s 130 read by OD-C4's "128 + SIGINT" (Safety-3 P2-1 (a)): on Unix after SIGINT gwz cleans up, then
re-raises SIGINT (a shell shows 130; a calling script stops); SIGTERM, SIGHUP and Windows console events exit normally
with 130. **Beyond, for D:** R6's cut removal, R7, R8's panic,
R10-R13, R14's in-pager case (should O5's restore wait while only a pager runs?). **O4 (a) narrowings, for D to put to the
operator:** R15's Windows events, R9's LOGOFF and SHUTDOWN under `user32` **(OS)**, a Windows script row 6.11 finds going on.
