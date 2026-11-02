# Cancellation design A r3 — Safety-AXIS REVIEW (round 3)

**Review object:** `/Volumes/projects/limbo/gwz-dev-docs-cancel/dev-docs/GwzCancelInterruptDesign-DRAFT.md`, revision 3 (working tree, modified against root `6d20ef6f`). Hash at start `3760dd61c9af6442945977f19b3bd1abf8eec5c3`; hash at end `3760dd61c9af6442945977f19b3bd1abf8eec5c3` (unchanged). | **Date:** 2026-11-03 | **Axis:** Safety: orphaned processes, stale locks, partial state, misreported effects; signal and handler races; PID reuse; fork without exec; Windows console and jobs; the forced path and the third interrupt (O5); whether each named residual is bounded and recognizable.

**Verdict: NO-GO.** Counts: 0 P0, 0 P1, 1 P2, 5 P3.
- **New architectural root causes: none.** P2-1 is a choice of the final exit action. Every P3 is a text, row or residual fix. The architectural count stays at 2.
- **Prior findings:** all 15 Safety round-2 IDs and all 13 Consistency round-2 IDs close against their own closure tests (§1).
- **Pre-commit:** I will give GO on a revision that does two things:
  - resolves P2-1 by either correction (a) or correction (b), including its closure test;
  - adds no new P0-P2 on this axis.

  The P3 items should close in the same pass but do not block. The line cap (397 of 400) can absorb P2-1(b) and P3-1 to P3-5 as cell-level edits.

## 0. Evidence base

**Pins**
- Member heads: gwz-cli `c0ba717`, gwz-py `2185cc6`, gwz-transport `281a28d`, gwz-git `9bc4ea9`, libgit2 `b172e3d18`, git2-rs `d13951f`.
- gwz-core HEAD is `7f571a18`, three commits past the pin `b745f1fa`. `git diff --stat b745f1fa 7f571a18` touches only tests, a fixture and Windows lane scripts, so every cited production line reads the same as at the pin.
- CPython 3.10.15 and 3.11.16 from `~/.asdf/installs/python/`.
- libssh2 from `libssh2-sys-0.3.3`, the version in the workspace `Cargo.lock`.

**Authority**
- `GwzTransportReleaseConvergencePlan.md` §10 D-V, :999-1069, every bullet, including round 2's O4 (a) and O5 (:1063-1069).
- OD-C1 to OD-C6 at `9d5a4d4c` §6 (OD-C4: "130 on every platform (128 + SIGINT, the shell convention)").
- OD-C7 (a)-(e) at `history/GwzCancellationAmendment-DRAFT.md:695-702`.

**Prior round:** `-ReviewSafety-2.md`, `-ReviewConsistency-2.md`, `-RemPlan-2.md` (including fix-pass GAP-1 to GAP-7).

**Hand-off only:** design B (`GwzChildLifetimeDesign-DRAFT.md`, blob `0cbb9081`, revision 3 on disk): process kinds table, §3.7, R-B1 to R-B19. I did not review B itself.

**Code read**
- **gwz-cli:** `lib.rs:160-350`; `globalargs/dispatch.rs:40-110`; `render_exit.rs:82-95`; `pager.rs:105-146`; `forall.rs:203-262`; `log_exec.rs:1-60`; `progress_detail.rs:196-260`.
- **gwz-core, ordinary build and network paths:**
  - `src/lib.rs:40-60`; `src/git/mod.rs:1-15` (the `endpoint` module is candidate-only).
  - `transport.rs:20-80, :700-760`; `transport_support.rs:16-135`; `transport_host/local_command.rs:40-110`.
  - The serial network callers: `pull_head_merge_preflight.rs:93`, `pull_head_member_preflight/root_lock.rs:58`, `handle_tag.rs:304, :371, :404`, `clone_workspace.rs:69`.
- **gwz-core, joins, locks and errors:**
  - `handle_materialize/apply.rs:176-184`; `handle_init_from_sources.rs:225-239`.
  - `operation/workspace_mutator_lock.rs` (an OS-advisory lock, released at process death).
  - `https_adapter.rs:358-369`; `stream_io.rs:215-245`.
  - A grep of production `Git2Backend::new()` sites: the CLI-reachable ones are the dispatch, read-only `diff`/`log`, and `hook`.
- **libgit2:** `streams/socket.c:134-160, :261-330`; `remote.c:1392-1410, :3030-3071`; `clone.c:394-407, :666-673`; `indexer.c:855-878`; `checkout.c:1880-1895, :2079-2150`; `reset.c:135-175`; `mwindow.c`/`pack.c` mutex sites.
- **libssh2:** `session.c:594-720` (`_libssh2_wait_socket` retries on `EINTR`).
- **gwz-py:** `bridge.py:20-40, :295-335`; `cli.py:70-80, :138-160`; `client_host.rs:160-166`; `client_host/operations.rs:26-30, :154-158, :199-208`.
- **CPython 3.11.16:** `asyncio/runners.py:71, :129`; `concurrent/futures/thread.py:29-38`; `test/test_signal.py:162-176, :214-226`.
- **gwz-transport:** `host/owner.rs:21-32`.

**External facts relied on, not measured here, marked (ext):**
- bash's "wait and cooperative exit" (WCE) on SIGINT (bash manual, "Signals"): a script continues if a foreground child received SIGINT but did not die of it.
- Linux `complete_signal()` and XNU deliver a process-directed signal to the main thread first if it does not block it.
- poll(2) is never restarted by `SA_RESTART`.

Nothing was built, run or modified, and no git mutation was made.

**Requirement provenance and scope**
- Every mechanism in revision 3 traces to D-V as round 2 recorded. The round-3 additions also trace:
  - G4 → decision (i);
  - the `Interrupted` slot state → OQ2;
  - the foreground scope from the pager's spawn → OD-C6;
  - `forget_cancel` → resolution (2);
  - the server lifecycle mode → O1 and O4 (a).
- No widening found.
- One narrowing is not disclosed: P2-1. The design reads OD-C4's "130 … the shell convention" as `_exit(130)` "over a signal death" (§3.6). That changes what a calling shell does after Ctrl-C, and nothing in D-V records that consequence.

## 1. Prior-finding closure table

| ID | Round-2 finding | Status | What I checked in revision 3 |
|---|---|---|---|
| S-P2-1 | After `wait()` a parked thread resumes into writes (rollback, manifest) | **Closed** | G4 (§3.4 table, `unpark` "stops the thread (G4)"; G4 paragraph). The fetch, clone, push and join continuations stop. R6 names the cut libgit2 removal as "beyond" (`clone.c:666-673` re-read). R7 names the join residue (`apply.rs:178-183`, `handle_init_from_sources.rs:227-231, :236` re-read). Rows 6.4 and 6.6 match the closure test, including the four-source `init` and the 2 s seam. The label gap of a G4 stop is new: P3-2 |
| S-P2-2 | An interrupt before the host and frame exist cancels nothing | **Closed** | The slot's `Interrupted` state; the dispatch checks before the host and before the frame; an owner put into an `Interrupted` slot is closed at once (§3.2); §8 row 4; row 6.11's seam leg (`push` never contacts the fixture; `switch` changes no file) |
| S-P2-3 | The pre-registration record is a 64-entry FIFO with no removal | **Closed** | §3.8: one entry per awaited call, consumed at registration, else `forget_cancel` when `_await_completion` returns; no capacity. `RECORDED_CANCELLATIONS` (`operations.rs:28`) is distinguished. Row 6.16 has 200 queued fetches, and a reused `request_id` that runs |
| S-P2-4 | The gwz-py CLI's later SIGINTs end the interpreter mid-native-write | **Closed (Record)** | R11 gives the count per version (3.10: 3rd; 3.11: 4th; `runners.py:129, :71`; `thread.py:31, :37`), the stale `index.lock` or ref `.lock`, D's report, and "beyond" beside OD-C7 (d). Row 6.16 records the count |
| S-P2-5 | OD-C6's wait not delivered while gwz feeds the pager | **Closed** | §3.7: `foreground` spans the pager from spawn (`pager.rs:107-115`) to exit (:138-145); the feed stops at the next chunk (:123-128). Row 6.15 has 1 MiB into `less`/`more` with two interrupts at page 1. Routine in-pager Ctrl-C is a new note: P3-4 |
| S-P3-1 | The clone checkout's stop residue was misdescribed | **Closed** | §3.5 row 2 and R6 give "new HEAD, no index, earlier files only" (`clone.c:394-407` re-read) |
| S-P3-2 | The slot's take, close and put were not atomic | **Closed** | §3.2: "under the mutex, turns `Owner` into `Closing` by `close()` (non-blocking)" (`owner.rs:21-32` re-read: `seal` then return) |
| S-P3-3 | The claim's loser was unspecified | **Closed** | §3.6: "a main thread that fails to take it blocks until Force exits"; row 6.13 has 100 runs |
| S-P3-4 | Windows LOGOFF and SHUTDOWN may never reach the handler | **Closed (Record)** | §3.1 and R9, marked **(OS)**. `Cargo.toml:183, :203` (`Win32_UI_WindowsAndMessaging`, candidate) re-read |
| S-P3-5 | G1 and R10 omit network I/O in the writing state | **Closed** | G1 lists "a clone before its first object, a fetch's pack trailer, a push's disconnect". R10 adds the push disconnect (`remote.c:3070` re-read) |
| S-P3-6 | Panic output, `dbg!`, unwind and reader death | **Closed (panic hook → R8)** | `dbg!` is in the check; guards are restored on unwind; the reader runs under `catch_unwind` and restores the dispositions; R8 names a panic on a stalled stderr. Rows 6.1, 6.2 and 6.4 carry these |
| S-P3-7 | An orphan after R14 was unnamed | **Closed** | R14 now includes "an orphaned pager or `forall` command" |
| S-P3-8 | The Cancel notice invites the tearing interrupt | **Closed** | §3.6's Force notice carries "(a further interrupt ends gwz at once and may leave a partial write)"; §8 row 8 includes the notices |
| S-P3-9 | A4 was unqualified for the ordinary build | **Closed** | A4 states that a registry-less commit gets the terminal's interrupt as on 1.0.18 (B R-B13); R17 |
| S-P3-10 | A refused nested frame was an unlisted stop point | **Closed** | Only outermost frames are refused. G3 says a refused park or `outside` fails its sequence `Cancelled` and keeps earlier writes. §3.5's last row lists the refusal points; R18 (fetch: pack and `.git/shallow` new, refs old); rows 6.4 and 6.6 |
| C-P2-1 | The write gate existed only in candidate builds | **Closed** | §3.4 "Carriage, both builds": `run` owns the gate; `Git2Backend::with_write_gate` is unconditional and taken by the ordinary dispatch (`dispatch.rs:91-96` re-read); the candidate route goes through the `ProcessGate` field. Production CLI paths that build their own `Git2Backend::new()` are read-only (`diff`, `log`) or `hook` (excluded, R12). Row 6.9 covers both builds |
| C-P2-2 | The reader's Force and `_exit` applied to server lifecycle commands | **Closed** | §3.1 "Server lifecycle mode" (no slot, bound, gate, claim or forced exit; C exits 130); §4 C; §8 rows 6 and 8; row 6.1 |
| C-P3-1 | 2.3s installed for a command that does not exist yet | **Closed** | 2.3s calls neither from `run`; 2.3k installs in its branch (§3.1, §5, §8 row 6) |
| C-P3-2 | Ordering, allowlist, totals, Owns | **Closed** | 2.3r-1 and 2.3r-2 come after 2.3s; the gwz-core half merges first; one absolute totals sentence in §8 row 7; 3.0d owns `Cargo.lock` and `BUILD.bazel` |
| C-P3-3 | The graph lacked `3.0f → 3.1c` | **Closed** | §8 row 1: `{3.1, 3.0d, 3.0e, 3.0f} → 3.1c` |
| C-P3-4 | The direct-output check's scope | **Closed** | 3.1c owns every flagged file on its base; test sources are excluded; row 6.2. Ordering against inherited file descriptors is new: P3-3 |
| C-P3-5 | "First threads" conflicted with B | **Closed** | "The first threads after the registry's"; C creates any registry first |
| C-P3-6 | O4, O5 and dependent residuals were inconsistent | **Closed** | O4 (a) and O5 applied. A3 and R8 name SIGKILL, a further interrupt after Force (R14) and Windows' end (R9). R15's Windows events are "beyond (OD-C1)" |
| C-P3-7 | The "Beyond" definition | **Closed** | The column is now "Covered by", and each cell names its clause |
| C-P3-8 | Rows unmeetable on Windows | **Closed** | Rows 6.1, 6.11, 6.12 and 6.15 mark M L legs and give W analogs (CTRL_BREAK; a `CREATE_NEW_PROCESS_GROUP` child) |
| C-P3-9 | Citation precision and provenance | **Closed** | `https_adapter/worker.rs:268, :340`, `local-import` :508-509 and `operations.rs:28` re-read; §0 maps D-V's "cancellable entry" (:1050) to the `HostOwner` slot |
| C-P3-10 | `gwz_response` was not frozen; §8 row 8 over-claimed | **Closed** | §8 rows 15 and 16 name `gwz_response`; row 8 adds "before the response was claimed", excludes `hook` and `claude-code-setup`, and includes servers |
| C-P3-11 | `diff`/`log` took no claim; senders blocked when Force comes without Cancel | **Closed** | §3.6: `diff` and `log` take the claim at their first stdout write; §3.1: "until Cancel or Force"; row 6.3 |

## 2. New findings

### P2-1: replacing a SIGINT death with a normal `exit(130)` makes a calling bash script continue after Ctrl-C, so the script can start the next command's remote effects

*Not architectural; not a new architectural root cause.* It concerns the final exit action only (step 8, the main thread's exit and gwz-py's `main`).

**Location**
- §3.6 Status: "130 … over 0, 1, 2 … and a signal death or `STATUS_CONTROL_C_EXIT`".
- §3.4 Force step 8 (`_exit`).
- §3.8: `main` "returns 130".
- Row 6.11, which is red against "signal death".
- §8 row 8.

**Violated invariants**
- D-V OQ2's end-to-end cancel ("Ctrl-C cancels the operation end to end").
- OD-C4's own rationale, "128 + SIGINT, the shell convention".
- No misreported effect: the exit status is gwz's report to its caller.
- Parity with 1.0.18 and git: both die by SIGINT (git's sigchain handlers re-raise after cleanup).

**Root cause**
- bash (ext) waits for a foreground child after it receives SIGINT. If the child died of SIGINT, bash ends the script or breaks the loop. If the child exited normally, bash assumes the child consumed the SIGINT and continues.
- Revision 3 ends every SIGINT path with a normal exit: `_exit(130)` or `exit(130)` on Unix, and `return 130` in the gwz-py CLI. To the calling shell, Ctrl-C now means "the child handled it".
- CPython made the opposite choice deliberately. An unhandled KeyboardInterrupt exits by SIGINT, and its test notes that "POSIX shells do more than look at the 8 bit value" (`test/test_signal.py:164-176`, 3.11.16). So gwz-py's CLI, which today dies by SIGINT through the unhandled KeyboardInterrupt, also regresses.

**Reproduction**
1. Take a bash script without `set -e`: `gwz pull; gwz push`. Or an interactive loop: `for d in */; do (cd "$d" && gwz pull); done`.
2. Press Ctrl-C during the first `gwz pull`.
3. gwz cancels and exits 130. bash continues to `gwz push`, which then pushes to the remote, or to the next loop iteration.
4. On 1.0.18, the same Ctrl-C ends the script.
5. In the loop case, each Ctrl-C now stops only one iteration.

**Impact**
- A remote effect the user tried to stop.
- A loop that one Ctrl-C can no longer end.
- No §7 row, recognizer or operator decision covers it. O4 (a) decided which sources take the interrupt path, not this.

**Correction (either closes it)**
- **(a) Re-raise SIGINT.** When the interrupt that preceded the claim was SIGINT on Unix, end by `raise(SIGINT)` under the default disposition (which step 1 already restored) after the final document, instead of `_exit(130)`. The shell then sees $? = 130 and WIFSIGNALED.
  - Other sources keep `exit(130)` (O4 (a); bash's WCE concerns SIGINT, not SIGTERM or SIGHUP). Windows keeps 130.
  - gwz-py's `main` renders, then lets KeyboardInterrupt propagate (or calls `signal.raise_signal(SIGINT)` with the default handler).
  - Row 6.11's red becomes "exit status via a normal exit", and §3.6 and §8 row 8 say "130 as the shell reports it".
  - Because this changes how a frozen surface reads (a programmatic `waitpid` sees a signal, and Python's `returncode` is -2), the lane owner confirms it as the reading of OD-C4.
- **(b) Keep `exit(130)` and record it.** Add a §7 row: "a calling bash script or loop continues after Ctrl-C; recognized by $? = 130 and the `Cancelled` entry". Mark it "beyond (OD-C4 shape)" for D to put to the operator, with this alternative stated.

**Closure test**
- (a): a row on M and L runs `bash -c 'gwz <cmd>; touch after'` and sends SIGINT to the process group. `after` is absent and `$?` is 130, for gwz-cli and for the gwz-py CLI.
- (b): §7 names the case, and §9 lists it among the "beyond" items for D.

### P3-1: signals are not masked outside the reader, so in no-host builds the signalled thread's libgit2 HTTPS wait fails at once with `EINTR`, which R10 and the Force notice misdescribe

**Location:** §3.1 Unix ("sets `sigaction` with `SA_RESTART`"); §3.2 "No host … `Interrupted` only (R10)"; R10; the Force notice "network work ended".

**Root cause**
- `SA_RESTART` does not restart poll(2) (ext).
- gwz sets both libgit2 timeouts (`transport_support.rs:62-69`), so libgit2's socket stream is non-blocking. Its read and write wait in `p_poll` and treat any `ret < 0` as "error receiving data from socket" (`streams/socket.c:301-329`; connect: :134-160).
- The ordinary build has no `endpoint` module (`src/git/mod.rs:4-8`), so HTTPS goes through this socket stream on macOS and Linux.
- Process-directed signals go to the main thread first (ext). The main thread runs serial network calls: `pull`'s root fetch (`pull_head_merge_preflight.rs:93`, `root_lock.rs:58`), `tag` (`handle_tag.rs:304, :371`), and `read_remote_file` (`clone_workspace.rs:69`).
- libssh2 does retry on `EINTR` (`session.c:705-713`); gwz's own candidate streams treat `Interrupted` as retry.

**Reproduction:** an ordinary build on Linux runs `gwz pull` over HTTPS. Ctrl-C during the root fetch gives an immediate "error receiving data from socket: Interrupted system call", not R10's "runs on". The member and the root report a network error, not `Cancelled` (A6).

**Impact:** diagnosability only. The `Cancelled` entry still leads, and a failed download unlinks its pack (`indexer.c:1469`).

**Correction:** either:
- `run` blocks SIGINT, SIGTERM and SIGHUP before B's registry (an A call first in `run`, as a B hand-off point); the reader alone unblocks them (std clears a child's mask; B row B8 already tests a blocked spawner); or
- R10 and §3.3 state the `EINTR` case and map an interrupted libgit2 socket error after Cancel to `Cancelled`.

**Closure test:** an ordinary-build row (M, L): SIGINT during an HTTPS fetch on the main thread. The member is `Cancelled`, or the row matches the stated residual.

### P3-2: a thread stopped by `unpark` or by `outside` after `f` records no label, so a push the remote accepted gets no stderr line

**Location:** §3.4 gate table (`safe_point(label)` records a label; `unpark()` and `outside` stops do not); "Push … on a closed gate it stops, the tracking ref stale (R5)"; §3.6 Notices; R5's "How recognized".

**Problem**
- The push callback unparks after the remote's report (`remote.c:3032-3038`). A G4 stop there leaves the thread stopped, not parked.
- So it gets neither "transfer ended … may have received it" (that line is printed per parked transfer) nor a "stopped:" line (no label).
- The forced envelope has no members. A push the remote certainly received is therefore reported nowhere, although R5 claims Force's notice recognizes it.
- The same gap applies to a fetch stopped at its unpark (R4) and to the join's stop (R7: "the stop notice").

**Correction:** every G4 or `outside` stop records the frame's label and phase. The push's line reads "the remote received it; tracking ref not updated".

**Closure test:** a row (M, L, W): Force during a network push's `push_update_reference`. stderr names the member and says the remote received it, and the tracking ref is old.

### P3-3: async `Output` is not ordered against programs that inherit gwz's file descriptors, and the claim holder's drain before exit is unstated

**Location:** §3.1 Writer; §3.7; `forall.rs:208` (the `eprintln!` banner, which 3.1c moves onto `Output`), then the member, which inherits fd 2 (:258-260).

**Problem**
- Once the banner is queued, the member's own stderr can reach the terminal before the `=== member ===` line, which attributes its output to the previous member.
- Separately, no text says that the main thread, after rendering through `Output`, waits for the writer to drain before `exit`. A plain `exit` can lose the final document on every command.

**Correction:**
- Add an `Output` drain barrier that a sender waits on before spawning a program that inherits stdout or stderr (a `forall` member, the pager).
- The claim holder drains before `exit` (unbounded, as `println!` is today; Force's 1 s bound is unchanged).

**Closure test:** a 6.3 leg:
- 1,000 `forall` members, each writing a line to stderr: every banner precedes its member's output.
- A normal `--json` command whose stdout is read slowly: the document is complete.

### P3-4: inside a pager, routine Ctrl-C (aborting a search, leaving follow mode) reaches O5's third interrupt, so R14's orphan is an everyday path for `gwz log`/`diff`

**Location:** §3.7 (step 1 runs at once in a foreground wait); R14.

**Problem**
- `less` shares gwz's foreground group, so each in-pager Ctrl-C is also gwz's interrupt. The third press kills gwz by the default disposition and leaves `less` on the terminal beside the shell prompt.
- No write is in progress, so O5's escape buys nothing here.
- This is strictly better than 1.0.18, which orphans on the first press, and it is decided by O5 as written. But R14's "When" does not show that this case is routine.

**Correction:** R14's "When" adds "including Ctrl-C used inside the pager". D puts to the operator whether O5's restore should wait while only a pager runs (no write frame open). No change is required for GO.

**Closure test:** R14 names the case.

### P3-5: G2's "no stop with an in-process lock held" has no row, though the transfer callback where G4 stops shares the progress sink's mutex

**Location:** §3.4 G2 and G4; "the transfer callback, now always set" (`transport_support.rs:89-92`); `StderrProgressSink.state: Mutex` (`gwz-cli/src/progress_detail.rs:199`), also taken by route notes (`dispatch.rs:40-44`).

**Problem:** if an implementation unparks inside the sink's critical section, a G4 stop holds the mutex. A writing thread that emits a route note (a clone negotiating in its writing phase) then blocks for good, `wait()` never returns, and only R14 ends gwz.

**Correction:** G2 says the callback unparks before any shared lock.

**Closure test:** a row 6.5 leg: member A's fetch stops at its unpark while member B's clone, in its writing phase, emits a route note. `wait()` returns.

## 3. Verified citations

All were opened at the pins in §0. Each matches unless a finding above says otherwise.

| Citation | Result |
|---|---|
| Plan D-V :999-1069 (O4 (a), O5 :1063-1069); OD-C4 at `9d5a4d4c` §6; OD-C7 at history :695-702 | Correct |
| `host/owner.rs:21-32` (`close` seals and returns `HostClose`) | Correct |
| `context.rs:80` `CLEANUP_BOUND` 5 s; `https_auth.rs:28` `CLEANUP_GRACE` 500 ms | Correct |
| `https_adapter/worker.rs:268` (`dispose_all`), `:340` (`pending_cleanup_count`) | Correct |
| `apply.rs:178-183` (F2 rollback); `handle_init_from_sources.rs:227-231, :236-238` | Correct |
| `transport.rs:32` (empty-target check), `:41` (checkout), `:55-59` (`remote_create`), `:73` (pins), `:718, :740` (`read_remote_file`) | Correct. Line 73 is the call `pin_creation_time_filter_neutralization` |
| `dispatch.rs:57-89, :82-87, :91-96`; `lib.rs:167, :174, :226-285`; `render_exit.rs:82-95` | Correct |
| `pager.rs:107-115, :123-128, :138-145`; `forall.rs:260` | Correct |
| libgit2 `clone.c:394-407, :666-673`; `remote.c:1394, :1409, :3032-3038, :3064-3067, :3070`; `indexer.c:874` | Correct |
| `streams/socket.c:301-329` (poll, no `EINTR` retry) | Not cited by the design; evidence for P3-1 |
| CPython 3.11 `runners.py:71, :129`; `thread.py:31, :37`; `test_signal.py:164-176` | Correct. `test_signal` is evidence for P2-1 |
| gwz-py `bridge.py:28-35, :298-316, :318`; `cli.py:76, :142-156`; `client_host.rs:164`; `operations.rs:28, :156, :199-208` | Correct |
| `gwz-core/Cargo.toml:183, :203` | Correct (candidate Windows features) |
| `operation/workspace_mutator_lock.rs` | Not cited. It is an OS-advisory lock, so a forced exit leaves no stale workspace lock (checked for this axis) |
| Not re-verified on this axis | `copy-contract`, `refcopy` and `local-import` line numbers; `owner.rs:253-303`; `https_worker.rs:377`; `file_worker.rs:91-98`; MachineOutput and ErrorCatalog lines |

## 4. Risks and next action

**Highest: P2-1.** The design turns Ctrl-C's "stop the script" into "stop this command". It is cheap to fix either way: re-raise SIGINT for a SIGINT source, or record it as a "beyond" row for D. The lane owner should pick (a) or (b). (a) changes how §8 row 8's "130" reads for programmatic callers; (b) is text only.

**Safety posture otherwise**
- G4 closes the resumption edge.
- The `Interrupted` state closes the pre-host window.
- Every residual in §7 is named, has a "Covered by" clause, and is bounded or explicitly "beyond", with a recognizer.
- The remaining gaps (P3-1 to P3-5) are diagnosability and implementation-guard rows.

**Operator-facing through D**
- P2-1(b), if chosen.
- P3-4: whether O5's restore should wait while only a pager runs.
- The existing "beyond" set: R6's cut removal, R7, R10-R13, R15's Windows events.

**Line budget:** 397 of 400. P2-1(b) and P3-1 to P3-5 fit as cell edits or appended clauses if as much text is removed elsewhere.

**Next action:** revision 4 resolves P2-1 (and ideally P3-1 to P3-5). Under my pre-commit, that is GO on this axis with no further mechanism. It is the last round under the cap, so it needs no third full round, only verification of the named edits.
