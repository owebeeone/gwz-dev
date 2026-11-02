# Cancellation design A r2 — Safety-AXIS REVIEW (round 2)

**Review object:** `/Volumes/projects/limbo/gwz-dev/dev-docs/GwzCancelInterruptDesign-DRAFT.md`, revision 2. The working-tree copy is uncommitted; root HEAD is `7f07dfc6`. Its hash was `bfe7d179e9decb7260ec8fb499b9a8a23f2e28f9` at the start and the same at the end, so the object did not change. | **Date:** 2026-11-01 | **Axis:** Safety. The question is what the text lets go wrong: orphaned processes, stale locks, partial state, misreported effects, races, the forced path, and whether each residual is bounded and recognizable.

**Verdict: NO-GO.** Counts: 0 P0, 0 P1, 5 P2, 10 P3.
- None of the new findings is a new architectural root cause. P2-1 is the last open edge of counted root cause (2), "the park/write boundary follows API calls, not write state". The architectural count stays at two.
- Prior findings: every round-1 finding of both reviews closes against its own closure test.
- Pre-commit: I will give GO on a revision that does two things:
  - resolves P2-1 to P2-5 as each correction states, or by an equivalent that passes the stated closure test;
  - adds those rows to §6, with no new P0 to P2 on this axis.

## 0. Evidence base

- **Pins.** Member heads match the design's citation line:
  - gwz-core `b745f1fa`
  - gwz-cli `c0ba717`
  - gwz-py `2185cc6`
  - gwz-transport `281a28d`
  - libgit2 `b172e3d18`
  - git2-rs `d13951f`
  - gwz-git `9bc4ea9`
  - CPython 3.10.15 and 3.11.16, from `~/.asdf/installs/python/`
  - Rust std 1.95.0, from rustup's `library/std` source
- **Authority.**
  - `GwzTransportReleaseConvergencePlan.md` §10 D-V, :999-1062. I read every bullet, including decisions (i) to (iii) at :1055-1062.
  - OD-C1 to OD-C6, read at `9d5a4d4c` §6.
  - OD-C7 (a) to (e), read at `history/GwzCancellationAmendment-DRAFT.md:695-702`.
- **Prior round.**
  - `GwzCancelInterruptDesign-ReviewConsistency.md`: C-P2-1 to C-P2-3, C-P3-1 to C-P3-9.
  - `GwzCancelInterruptDesign-ReviewSafety.md`: S-P2-1 to S-P2-7, S-P3-1 to S-P3-7.
  - `GwzCancelInterruptDesign-RemPlan.md`, including fix-pass gaps G1 to G13.
- **Hand-off only.** `GwzChildLifetimeDesign-DRAFT.md`, revision 2 on disk, blob `9724eae6`: §3.1 to §3.7, §4 and §7. I did not review B itself.
- **Code read:**
  - **gwz-cli:** `lib.rs:160-180, :300-350`; `globalargs/dispatch.rs:57-100`; `render_exit.rs:82-95`; `pager.rs:95-170`; `diff_exec.rs:161-167, :347-352`; `forall.rs:258-262`; `append_branch_summary/machine.rs:10-16`; `log_exec.rs:33-36`; `progress_detail.rs:233`.
  - **gwz-core:** `merge_prepared.rs:225-344`; `merge_recovery.rs:143-251`; `refs.rs:33, :134`; `repository.rs:31, :56, :81, :112`; `stash.rs:18`; `preservation.rs:344`; `stash_support.rs:18-26`; `handle_commit.rs:179-186`; `handle_materialize/apply.rs:20-190`; `handle_init_from_sources.rs:147-245`; `transport.rs:25-80, :340-660`; `transport_support.rs:85-135`; `repository_support.rs:28-36`; `composition.rs:21-35`; `session_host/context.rs:78-80`; `workspace_mutator_lock.rs:1-60`; `https_auth/process_tree/windows*.rs` (creation flags); `Cargo.toml:122, :165-205`; `scripts/checks/check_process_globals.py:101-105, :538-542`; `scripts/run_tests.py:550-553`. I also grepped production `Command::new`, `catch_unwind` and blocking lock sites.
  - **gwz-transport:** `gwz-connection/src/host.rs:1-12`, `host/owner.rs:15-62`, `host/config.rs:18-40`, `operation.rs:338-420`; `GwzConnectionApiContract.md:265-276`.
  - **libgit2:** `fetch.c:195-226`; `transports/local.c:340-430, :583-695`; `transports/smart_protocol.c:780-783`; `indexer.c:865-878, :925-936, :1466-1470`; `clone.c:640-680, :385-455`; `remote.c:3005-3075`; `transport.c:34-37, :86-90`; `refdb_fs.c:1175-1179`; `checkout.c:1884-1892, :2079-2150, :2686`; `reset.c`; `stash.c:1124-1159, :1232, :1280-1286`.
  - **gwz-py:** `src/gwz/bridge.py:20-40, :216, :290-330`; `src/gwz/cli.py:74-78, :140-158`; `src/gwz/client.py:277-302`; `native/src/client_host.rs:210-240`; `client_host/operations.rs:15-70, :185-235`; `native/src/shims.rs:60-100`; `dev-docs/GwzPyPerOperationTransportDesign.md:98-110`.
  - **CPython and Rust std:** CPython 3.11 `asyncio/runners.py:60-75, :95-165`, CPython 3.10 `runners.py:40-75` and `concurrent/futures/thread.py:34-37`; Rust std `sys/pal/unix/mod.rs:30-130` (`sanitize_standard_fds`).
- **(OS) facts relied on, not measured here:**
  - Windows does not call a console `HandlerRoutine` for CTRL_LOGOFF_EVENT or CTRL_SHUTDOWN_EVENT once the process has loaded `user32.dll` or `gdi32.dll` (SetConsoleCtrlHandler / HandlerRoutine documentation).
  - `less` reads its input only as it displays it.
  - A caught disposition survives `fork` and is reset at `exec`.
- Nothing was built, run or modified, and no git mutation was made.

## Problem, requirement provenance, and scope

- **Requirement.** D-V reverses OQ2: Ctrl-C "leaves no orphaned process, stale lock or partial state". Decision (i): Force "kills children and network work at once, then waits, with no time bound, for any local write in progress to reach its next safe point or finish; only SIGKILL ends a hung write". Decision (ii) lets `git commit` and `git tag`, with their hooks and signers, finish. OD-C4 sets exit 130 and a JSON marking. OD-C6 says gwz waits for the pager and for `forall` commands and never kills them. OD-C7 (a) to (d) are accepted. Resolution (2) makes gwz-py call the canceller first.
- **Mechanisms and their provenance:**
  - The interrupt module traces to OD-C1 and O1.
  - The HostOwner slot traces to OQ2 and plan 3.1.
  - The write gate and safe points trace to decision (i), as demonstrated necessity.
  - The claim and the 130 status trace to OD-C4.
  - The foreground rule traces to OD-C6.
  - `cancel_call` traces to resolution (2).
  - The output writer is demonstrated necessity (S-P2-2).
  - **No widening found.**
- **Narrowing the text does not disclose:**
  - A Force that has passed `wait()` still lets parked threads resume into writes (P2-1).
  - An interrupt before the command's host and frame exist does not prevent the command's work (P2-2).
  - gwz-py's cancel record drops cancels under load (P2-3).
  - The gwz-py CLI's later SIGINTs tear native writes (P2-4).
  - OD-C6's wait is not delivered while gwz is still feeding the pager (P2-5).

## 1. Prior-finding closure table

| ID | Round-1 finding | Status | What I checked |
|---|---|---|---|
| C-P2-1 (ARCH) | Cancel and gate carried on the entry 3.1 replaces and 3.3 removes | **Closed** | §3.2 uses the HostOwner slot. `close()` seals at the call (`host/owner.rs:21-32`; drop also seals, :38-46). The gate is carried as a `ProcessGate` field minted in `composition.rs` (:30-31, the only site) through B's `with_process_gate`. 3.0d's Owns no longer lists `cancellable.rs`. §8 row 4 amends plan 3.1. A slot race remains; it is new (P2-2, P3-2) |
| C-P2-2 | Server lifecycle in Phase 2 cannot take a Phase 3 module | **Closed** | 2.3s is in Phase 2 wave 2 lane 2 (§5). §8 rows 5-7 amend 2.3i and 2.3k and order 2.3r-2 after 2.3s. §3.1 Install limits 2.3s to the server lifecycle |
| C-P2-3 (ARCH) | Safe points inside a held ref transaction | **Closed** | G2. Five run-to-end writes: `merge_prepared.rs:266, :280, :339` inside the lock at :248-249, committed at :271 and :344; `merge_recovery.rs:175` (lock :143-144) and :251 (lock :197-198), all re-read. Row 6.9 covers them. `stash_drop`'s transaction (`stash.c:1232`) follows apply and has no safe point inside |
| C-P3-1 | §8 omits clauses the design contradicts | **Closed** | §8 rows 9-12 and 14 are present |
| C-P3-2 | Inventory misses `:339` and `preservation.rs:344` | **Closed** | Both sites are in §2 and §3.5, as is `:280` |
| C-P3-3 | Check coverage and B's entries | **Closed** | `run_tests.py:552` re-read. The five runs are stated in §2. §3.1 Check gives B's entries and the edge |
| C-P3-4 | `par_map_per_host` plumbing | **Closed** | `par_map_per_host_gated` plus `GitRepository::write_gate()`. 3.0d owns the six caller files. The frame is per item, not per worker loop |
| C-P3-5 | Hand-off wording with B | **Closed (residual note)** | §4 points 1-7 agree with B §3.7. One inaccuracy remains: "the reader and writer are gwz's first threads" (§3.1). B's registry, created first, starts a writer thread and a watch thread (B §3.4). It does no harm, because neither thread spawns |
| C-P3-6 | OD-C4 marking on the forced `--jsonl` path and on CTRL_CLOSE | **Closed** | §3.6 claim rule and R13. The claim's loser is unspecified; that is new (P3-3) |
| C-P3-7 | SIGTERM and SIGHUP to 130 without provenance | **Closed** | §9 O4 states the options and their reach |
| C-P3-8 | Notice modes contradict | **Closed** | §3.6 Notices and §8 row 9 agree |
| C-P3-9 | Citation and fact errors | **Closed** | `bridge.py:216/318` (`NativeCoreBridge.call`), 3.10 `runners.py:44-49, :55-63`, f9 path absolute, 3.0d After, graph edges: all re-read |
| S-P2-1 (ARCH) | Parked network calls contain local writes | **Closed (last edge reopened as P2-1)** | Fetch: the callback at `indexer.c:874` unparks when all objects are received, before `writepack->commit` (`smart_protocol.c:782`; local transport `local.c:688`). `.git/shallow` (`fetch.c:223`) is a write. `indexer.c:1469` unlinks an uncommitted pack. Clone pins move to `remote_create`. Push unparks at `push_update_reference` (`remote.c:3032-3038`, before :3064-3067). A local-path push writes throughout (`local.c:354, :426`; `transport.c:36, :88-89`). gwz already has the same predicate, `served_by_local_transport` (`transport.rs:583-614`). The remaining gap is resumption after `wait()`: P2-1 |
| S-P2-2 | Force `wait()` unbounded on output or scheduler waits | **Closed** | G1 and G2. Writer and `Output` hand-off. 3.1c moves every direct handle onto `Output` and adds `check_direct_output.py`: `machine.rs:13`, `lib.rs:279`, `pager.rs:130, :146`, `log_exec.rs:33, :36`, `progress_detail.rs:233`, all re-read. Per-item frames. No blocking workspace-lock wait exists (`workspace_mutator_lock.rs:13-31` only tries). The panic-output gap is new (P3-6) |
| S-P2-3 | A closed gate admits new work | **Closed** | G3 refuses frames, parks and `outside`. `is_closed` acts as par_map's `cancelled`. Member-head safe points. Rows 6.7 and 6.10. The Cancel-path analogue is new (P2-2) |
| S-P2-4 | Forced envelope asserts non-completion | **Closed** | The claim is taken inside the frame. Force waits for a finished dispatch. The new message does not assert non-completion. Row 6.13 |
| S-P2-5 | Triple Ctrl-C tears via OD-C7 (a) | **Closed (decided)** | The operator accepted OD-C7 (a) as recommended (:1061-1062). R14 names the case, §9 O5 raises the tension with (i), and row 6.13 pins the 50 ms case. A notice wording gap is new (P3-8) |
| S-P2-6 | gwz-py CLI drops completed results | **Closed** | `CancelledError` carries `gwz_response`. `run` renders and returns 130. `main` renders only once. Row 6.16 |
| S-P2-7 | A pre-registration cancel is lost | **Closed against its closure test** | `cancel_call` records an id that is not yet live, and registration refuses it (`operations.rs:199-208` today refuses; the design adds the record). The single queued-call row passes. The record's capacity and lifetime are new (P2-3) |
| S-P3-1 | Handler runs in forked children | **Closed** | PID check before `errno` handling. Row 6.1 |
| S-P3-2 | Deferred Force has no escape | **Closed** | Step 1 runs at once during a foreground wait. Row 6.15. The orphan that follows is unnamed: new P3-7 |
| S-P3-3 | R5's Cancel-path notice | **Closed** | R5's recognition column now matches §3.6 |
| S-P3-4 | Read-only list unchecked | **Closed** | Exhaustive `match`, membership rule, row 6.14 |
| S-P3-5 | CTRL_LOGOFF and CTRL_SHUTDOWN unspecified | **Closed (OS caveat new, P3-4)** | Both are mapped to Force, and R9 marks them O4 |
| S-P3-6 | R17 avoidable | **Closed** | R17 is removed and the writer owns output |
| S-P3-7 | `--server` interim undefined | **Closed** | §3.2 `--server` rule |

RemPlan fix pass G1 to G13: each is reflected in revision 2 at the section the RemPlan names. I checked G1, G2, G3, G5, G9, G11 and G12 against the text and the cited code. G2's claim-in-frame and G3's await both stand, subject to P3-2 and P3-3.

## 2. New findings

### P2-1: once `wait()` has returned, a parked thread can still resume into a local write, so Force's exit lands mid-write
*An instance of counted root cause (2); not a new architectural root cause.*

- **Location:**
  - §3.4 gate table: `unpark` "makes it writing again … the caller then taking its abort path"; `wait` returns once every thread is "parked, stopped or out of its frames".
  - Force steps 5 to 8.
  - G1: "A frame is parked only while bytes are in transfer or while it waits".
  - `par_map_per_host_gated` "parks for its join".
  - Clone: "On `Closed` libgit2 removes the clone inside the frame".
  - R6.
- **Violated invariant:** decision (i) and A3, "Force never exits mid-write". Also G1, that a parked frame does no local write.
- **Root cause.** `wait()` takes a one-shot snapshot. `outside` stops a thread that returns to a closed gate, but `unpark` makes it writing again, and Force does not wait again after step 5. Writes performed after a Closed unpark therefore race step 8's `_exit`:
  - **gwz-level continuations.** `handle_materialize/apply.rs:176-183` runs the F2 rollback `remove_dir_all` over every fresh clone after the join. `handle_init_from_sources.rs:221-237` does the same, or on success writes the manifest and lock, `sync_workspace_boundary_in` and the bootstrap files.
  - **libgit2's own cleanup while still parked.** A clone whose transfer fails, because the host was closed, runs `git_futils_rmdir_r` (`clone.c:666-673`) with no callback to unpark it.
- **Reproduction** (`gwz materialize` of N fresh members on a host, double Ctrl-C):
  1. Cancel closes the host, and in-flight transfers start failing.
  2. Force closes the gate. `wait()` returns at once, because the dispatch is parked in its join and the workers are parked in transfer.
  3. Force takes the free claim and writes the envelope. Step 8 runs within ≤1 s.
  4. Meanwhile libgit2 removes the failed clones, the join completes, and the dispatch unparks with `Closed`.
  5. Its continuation runs `remove_dir_all` over the fresh members, completed ones included, or, if every outcome was Ok, writes `gwz.toml` and `gwz.lock`.
  6. `_exit` cuts it. The result is half-deleted member directories, possibly with `.git/objects` partly removed, or a manifest written without its lock.
- **Impact:**
  - A partial state that decision (i) excludes, produced on the most common gesture.
  - R6 attributes partial directories only to "Force during a clone's transfer", classes them "Beyond: no", and does not describe a cut rollback.
  - A manifest/lock mismatch is not named at all.
- **Correction:**
  - Add invariant **G4**: after `close()`, an `unpark` that reports `Closed` stops the thread, exactly as `outside` does. The fetch callback then leaves the temp pack (R4), the clone callback leaves the partial clone (R6), the push callback leaves the tracking ref (R5), and the join leaves members undone (R7). No continuation runs.
  - R6 also states that libgit2's error-path removal of a clone whose transfer failed may be cut by step 8, which leaves a partly removed directory.
  - Rows:
    - **6.4:** a parked thread whose `unpark` returns `Closed` after `wait()` blocks, with nothing written after it.
    - **6.6:** double interrupt during a four-member `materialize`, with test hooks delaying step 8: the workspace's `gwz.toml` and `gwz.lock` are byte-identical to before or both new, and every member directory is either absent or a complete pinned clone (or R6's named partial clone).
- **Closure test:** rows 6.4 and 6.6 as above, on M, L and W. Each is red against "unpark makes it writing again".

### P2-2: an interrupt that arrives before the command's host and write frame exist cancels nothing, so the command then starts network and local work the user asked to stop

- **Location:** §3.1 Install (installed "right after parsing"); §3.2 "Cancel (or Force, if no Cancel came) takes it, calls `close()`"; G3 (only after Force's `close()`).
- **Violated invariants:**
  - D-V OQ2 (Ctrl-C cancels end to end).
  - No remote effect or partial state starts after the interrupt (the S-P2-3 invariant, now on the Cancel path).
- **Root cause.**
  - The slot holds an owner only after the dispatch builds it (plan 3.1, in `dispatch.rs`, after `lib.rs:180-300`'s setup).
  - A Cancel that finds the slot empty does nothing, and nothing records it.
  - The later `HostOwner` opens normally, and so does the dispatch's write frame, because only Force closes the gate.
- **Reproduction:**
  1. A supervisor sends SIGTERM (or the user presses Ctrl-C) within the first tens of milliseconds of `gwz push` across members.
  2. Cancel finds no owner.
  3. The host is then built and the pushes start.
  4. At the 5 s bound Force closes the host, and each push in its upload ends with R5 ("the remote may have received it").

  The same happens for local-only `gwz switch`: its checkouts start after the interrupt and stop at safe points (R1). Today the OS default ends gwz before any push or checkout begins.
- **Impact:** remote effects and partial trees that A itself starts after the stop request. It is a regression against today on that window, and §7 does not name it (R10 covers only builds and routes that never have a host).
- **Correction:**
  - The slot has an `Interrupted` state. Cancel on an empty slot sets it.
  - The dispatch checks the slot before it builds the host and before it opens its write frame. If interrupted, it refuses the request with `Cancelled`, writing nothing, exit 130 with §3.6's entry.
  - An owner put into an interrupted slot is closed at once.
- **Closure test:** a test hook delays the dispatch between install and slot fill; one SIGINT is sent in the gap.
  - `gwz --json push` of two members: the fixture is never contacted, both members are `Cancelled`, exit 130.
  - `gwz switch`: fixture files are unchanged.
  - Run on M, L and W (CTRL_C).

### P2-3: gwz-py's pre-registration cancel record is a bounded FIFO with no removal, so cancels are lost under load and stale entries refuse later calls

- **Location:** §3.8 "an id not yet live is recorded (the last 64, as `operations.rs:27`) and its registration refused `Cancelled`"; §1 A8.
- **Violated invariant:** A8: a caller's cancel goes through the canceller first, also before the operation registers.
- **Root cause.**
  - `operations.rs:27` sizes the record of completed cancellations, which is a different object.
  - A's pre-cancel entries reuse its size of 64 but have no removal at the call's end, which round 1's S-P2-7 correction required ("evicted at the call's return").
  - `_run_native` calls `cancel_call` for every cancelled call (`bridge.py:298-316`, plus the new id), including local and native-route calls that never register. Each of those leaves an entry that is never consumed.
  - Request ids can be supplied by the caller (`client.py:277, :297`), and operation ids are `op_<request_id>` (`shims.rs:79-80`).
- **Reproduction:**
  - **Lost cancels.** An application's TaskGroup of 200 fetches. One task fails, so the group cancels all of them. Python's default executor runs only min(32, CPU count + 4) of them, so 168 are still queued and unregistered. Only the last 64 records survive. About 104 queued fetches later register, run and finish while their tasks report `CancelledError` (with `gwz_response` set).
  - **Stale entries.** A cancelled local `commit` with `request_id="R"` leaves a record. A later `fetch(request_id="R")` (an idempotent retry) is refused `Cancelled` without running.
- **Impact:** a silent loss of cancellation and misreported effects, which is a push effect for a push batch, plus spurious refusals.
- **Correction:**
  - `cancel_call` records only network methods. The bridge already knows the method, and `transport_meta` decides network status.
  - The bridge clears its record when `_await_completion` returns, through a native `forget_cancel(id)` or by consuming the record at registration.
  - With those two rules, the record needs no capacity bound: it is bounded by the number of awaiting tasks.
  - §3.8 states both rules.
- **Closure test:**
  - `max_workers=1`, one blocking call, 200 queued fetches cancelled together: none contacts the fixture.
  - A cancelled local call followed by a network call with the same `request_id`: the second runs normally.
  - Run on 3.10 and 3.11+, M, L and W.

### P2-4: the gwz-py CLI's later SIGINTs end the interpreter while a native write runs, and R11 calls this "keeps waiting"

- **Location:** §3.8 "The gwz-py CLI"; §7 R11 ("a second interrupt keeps waiting"); R15.
- **Violated invariants:** OQ2's "no stale lock or partial state"; named residuals must be accurate.
- **Root cause.** The native call runs on the default executor (`bridge.py:307`, `asyncio.to_thread`), whose workers are non-daemon threads joined at exit (`concurrent/futures/thread.py:37`). On 3.11 the sequence is:
  1. The first SIGINT cancels the main task (`runners.py` `_on_sigint`).
  2. The second raises `KeyboardInterrupt`. `Runner.run`'s `finally` restores `default_int_handler`, and `close()` runs `_cancel_all_tasks`, which waits.
  3. The third raises out of `close()`. `main` returns 130, and finalization's `threading._shutdown` joins the worker.
  4. The fourth interrupts that join. Finalization continues and the process exits with the native thread mid-write.

  3.10 needs one interrupt fewer. gwz-py itself expects native work to outlive finalization (`GwzPyPerOperationTransportDesign.md` §2.6).
- **Reproduction:** `gwz-py commit` across members (local, native), then SIGINT ×3 on 3.10 or ×4 on 3.11, 50 ms apart, during member 2's ref transaction or checkout. The result can be a torn checkout or a stale `index.lock` or `refs/…lock`.
- **Impact:** a tear and stale-lock path that no residual names and no decision covers. OD-C7 (d) covers only SIGTERM and SIGHUP for the gwz-py CLI.
- **Correction:**
  - State the path in R11, "Beyond: yes", as a torn native write and stale lock after the k-th SIGINT, with D's lock report as the recognizer.
  - Put it to the operator beside OD-C7 (d). The OD-C7 (d) alternative already offered CLI-only routing in `cli.py`; note that 3.2b's test as written would forbid that alternative.
- **Closure test:** a §7 row names the case and its count on 3.10 and 3.11, and a 6.16 leg demonstrates it (or the chosen alternative).

### P2-5: OD-C6's "wait for the pager" is not delivered while gwz is still feeding it, so a second interrupt orphans the pager

- **Location:** §3.7 ("the pager after its input closes … `Interrupts::foreground(f)` wraps that wait … On Cancel, `diff` and `log` close the pager's input and wait"); row 6.15.
- **Violated invariants:** OD-C6 (gwz waits and never kills the pager); D-V's "no orphaned process".
- **Root cause.**
  - Feeding is a blocking `write_all` into the pager's stdin on the calling thread (`pager.rs:122-128`; `diff_exec.rs:161-167, :347-352`).
  - `less` reads only as it displays, so with any diff larger than the pipe buffer the feed blocks while the user reads page 1.
  - A blocked writer cannot observe Cancel, so it cannot "close the pager's input".
  - Feeding is not a foreground wait, and `diff` and `log` are on the read-only list, so a Force at that point proceeds: `wait()` returns, the claim is taken, and `_exit(130)` runs.
- **Reproduction:** `gwz diff` with a diff over 64 KiB, `less` showing page 1, two Ctrl-C presses (common inside `less`, where ^C aborts a search). gwz exits 130, and `less` keeps the terminal in raw mode beside the shell prompt.
- **Impact:**
  - The decided OD-C6 behaviour fails on the default interactive path.
  - An orphaned foreground program that §7 does not name (R-B8 covers only SIGKILL).
  - Row 6.15's "diff waits for its pager" passes with a small diff.
- **Correction:**
  - The foreground scope starts at the pager's spawn, so Force's later steps wait for the pager child's exit whenever a pager is alive.
  - On Cancel the feed stops at its next chunk. The blocked write returns `EPIPE` when the pager quits.
- **Closure test:** row 6.15 adds `gwz diff` of 1 MiB into `less` with two SIGINTs at page 1. gwz stays alive until `less` exits, then exits 130, and no prompt appears while `less` runs. Run on M and L; on W use CTRL_C with `more`.

### P3-1: the residue of a stop at the clone's checkout safe point is misdescribed

- **Location:** §3.5 row 1 ("index and HEAD old"); R1; R6 ("Force during a clone's transfer").
- **Problem.** A clone's checkout sets HEAD before checkout (`clone.c:394-407`), and no index exists until checkout's end (`checkout.c:2686`). A stop at file k leaves a new HEAD, no index and a partial tree. A later `stage`+`commit` on that member records the deletion of every file not yet checked out.
- **Correction:** add a clone row to §3.5 and R6 ("new HEAD, no index, partial tree; the notice names `clone`"), so that D's recognizer covers it.

### P3-2: the slot's take, close and put are not atomic with the dispatch's empty

- **Location:** §3.2.
- **Problem.** If the dispatch empties the slot after the reader has taken the owner and before it has put the `HostClose` back, the main thread awaits nothing. `Pending` is then never reported, contrary to §3.3 and row 6.8, and the main thread exits before the host's cancel has settled.
- **Correction:** one mutex-guarded slot, held across take, `close()` (non-blocking by contract, `owner.rs:21-32`) and put.

### P3-3: the claim's loser is unspecified

- **Location:** §3.6 "The claim".
- **Problem.** A main thread that ends its frame after Force took the claim may compute `exit_code_for_response` and call `exit(0|1)`, racing Force's `_exit(130)`. The outcome is exit 1 with an empty stdout, against A5.
- **Correction:** state that a main thread that fails to take the claim blocks until Force exits.

### P3-4: Windows CTRL_LOGOFF and CTRL_SHUTDOWN may never reach the handler

- **Location:** §3.1 Windows; R9.
- **Problem.** The candidate build imports `Win32_UI_WindowsAndMessaging` (Pageant: `FindWindowW`, `SendMessageW`; `gwz-core/Cargo.toml:172, :203`), so `user32.dll` is loaded. The handler then receives no LOGOFF or SHUTDOWN events **(OS)**: no Force runs, and the OS ends gwz.
- **Correction:** state this in §3.1 and in R9's "When" cell, and add a row (CTRL_LOGOFF via a test session) or mark the fact unverified.

### P3-5: G1's list of blocking states and R10 omit network I/O in the writing state after an unpark

- **Location:** §3.4 G1; R10.
- **Problem.** After a fetch unparks at "all objects received", it still reads the pack trailer. After a push's `update_tips` comes `git_remote_disconnect` (`remote.c:3070`). Both run in the writing state. With a host, the close fails them. With no host, R10 lists only fetch and clone, not push's disconnect.
- **Correction:** extend G1's list and R10's "What can remain" cell.

### P3-6: panic output and a dying reader are outside the contract

- **Location:** §3.1 Writer and Check; §3.4.
- **Problems:**
  - Rust's default panic hook writes stderr directly. `par_map_per_host.rs:291` catches item panics, so a panicking item in a write frame blocks on a stalled stderr. That is a G1 breach, and `wait()` becomes unbounded.
  - `dbg!` is not in the token list of `check_direct_output.py`.
  - `writing`, `park` and `outside` are not stated as unwind-safe guards, so an unwind can leave a thread marked parked or writing.
  - If the reader thread dies, nothing restores the dispositions, and gwz then ignores SIGINT and SIGTERM.
- **Correction:**
  - Route panic messages through `Output`, with a panic hook allow-listed in the interrupt module (the `HOOKS` class, `check_process_globals.py:105`), and add `dbg!` to the check.
  - Make frames and parks RAII guards.
  - The reader runs under `catch_unwind`; on failure it restores the dispositions and aborts.

### P3-7: an orphaned foreground program after R14 is not named

- **Location:** §7 R14; row 6.15 ("the member runs on").
- **Problem.** R14 names only a torn write. The orphaned `forall` command or pager that the third SIGTERM leaves is not named (R-B8 covers only SIGKILL).
- **Correction:** add the orphan to R14's "What can remain".

### P3-8: after a Force started by the bound, the Cancel notice invites the interrupt that tears a write

- **Location:** §3.6 Notices.
- **Problem.** The Cancel notice says "interrupt again to stop now". Once the bound has started Force and step 1 has restored the default disposition, that invited interrupt kills gwz (R14). The Force notice does not say so.
- **Correction:** the Force notice adds "a further interrupt ends gwz at once and may leave a partial write". This is a frozen surface, so §8 row 8 changes with it.

### P3-9: A4 is unqualified for the non-candidate build

- **Location:** §1 A4; §7.
- **Problem.** Without B's registry, `git commit` runs in gwz's process group, so the terminal's SIGINT kills it mid-hook (B R-B13). A's §7 does not cross-reference that.
- **Correction:** qualify A4 and add an R-B13 pointer to §7.

### P3-10: a refused nested frame is an unlisted stop point

- **Location:** §3.4 gate table (`writing`: "nesting allowed; refused `Closed` before f if the gate is closed").
- **Problem.** A nested frame opened inside a running sequence after `close()` fails that sequence mid-way, not at a §3.5 safe point. For example, a `git commit` frame opened after `stage` leaves the member staged but uncommitted, and nothing names that state.
- **Correction:** either refuse only outermost frames, or list nested refusal points and their residue in §3.5 and §7.

## 3. Verified citations

| Citation | Result |
|---|---|
| Plan D-V :999-1062; OD-C1 to OD-C6 at `9d5a4d4c` §6; OD-C7 at `history/GwzCancellationAmendment-DRAFT.md:695-702` | Correct |
| `merge_prepared.rs:248-249, :266, :280, :339` (commit :271, :344); `merge_recovery.rs:143-144, :175, :197-198, :251`; `refs.rs:33, :134`; `repository.rs:31, :56, :81, :112`; `stash.rs:18`; `preservation.rs:344`; `handle_commit.rs:179-186` | Correct |
| `stash_support.rs:18-26` | Correct as the edit site. It has no `checkout_options` today; 3.0f adds them |
| `transport.rs:32, :55-59, :73`; `repository_support.rs:30-35`; `transport_support.rs:89-92, :130`; `transport.rs:368` | Correct |
| `composition.rs:21-35`; `context.rs:80`; `check_process_globals.py:103-104, :540-541`; `run_tests.py:552`; `gwz-cli/.github/workflows/ci.yml:26-31` | Correct |
| `host.rs:6-10`; `host/owner.rs:21-32, :50-59`; `host/config.rs:22, :36`; `operation.rs:343-344, :414-417`; contract :269-275 | Correct. Drop also seals (`owner.rs:38-46`) |
| `lib.rs:167, :174, :279`; `machine.rs:13`; `pager.rs:130, :138-145, :146, :155-170`; `log_exec.rs:33, :36`; `progress_detail.rs:233`; `forall.rs:260`; `render_exit.rs:82-95` | Correct |
| libgit2 `smart_protocol.c:782`; `fetch.c:218-223`; `indexer.c:874, :1469`; `clone.c:666-673`; `remote.c:3032-3038, :3064-3067`; `transport.c:36, :88-89`; `local.c:354, :426`; `refdb_fs.c:1175-1179`; `checkout.c:1886-1891, :2686`; `stash.c:1126, :1149, :1159, :1282-1285` | Correct. `local.c:670-688` also calls the transfer callback per object before `commit`, so the fetch rule holds on the local transport |
| gwz-py `bridge.py:28-35, :298-316, :318` (class at :216); `cli.py:76, :142-156`; `client_host.rs:226-231`; `shims.rs:79-80`; `operations.rs:27, :199-208`; gwz-py design :104 | Correct. `operations.rs:27` is the record of completed cancellations (P2-3) |
| CPython 3.11 `runners.py:117-124, :150-157`; 3.10 `runners.py:44-49, :55-63` | Correct |
| §3.1 "the reader and writer are gwz's first threads" | Inaccurate: B §3.4 starts two threads earlier. Harmless (C-P3-5 note) |
| Not verified on this axis | `crates/copy-contract`, `refcopy`, `local-import` line numbers; `owner.rs:253-276, :298`; `https_worker.rs:377`; `file_worker.rs:91-98`; `MachineOutput.md` lines; plan :315, :712, :717-728 |

## 4. Risks and next action

- **Highest risk: P2-1.** Force's guarantee that it never exits mid-write fails at the resumption edge of the gate. The fix is one contract rule (G4: a Closed unpark stops the thread, as `outside` does) plus two rows. It closes the last edge of counted root cause (2) without a new architectural cause.
- **Second: P2-2 and P2-5.** P2-2 is an interrupt lost before the host exists. P2-5 is a pager orphaned while gwz is still feeding it. Both deliver less than D-V and OD-C6 promise on common paths, and both have local fixes: an `Interrupted` slot state, and a foreground scope that starts at the pager's spawn.
- **Third: P2-3 and P2-4 (gwz-py).** One fixes the cancel record's rules; the other names a residual and puts it to the operator beside OD-C7 (d).
- **Operator-facing:**
  - O5, decision (i) beside OD-C7 (a), stays open. P3-8 and the GitHub Actions cancel sequence make it concrete: SIGINT, then SIGTERM 7.5 s later. After a bound-started Force, that SIGTERM tears whatever local write is still running.
  - P2-4 joins OD-C7 (d).
- **Next action:** revision 3 resolves P2-1 to P2-5, adds the rows named above to §6, and settles P3-1 to P3-10. Then a fresh Safety re-review against the new blob.
