# Cancellation design A r1: Safety-axis review

**Review object:** /Volumes/projects/limbo/gwz-dev/dev-docs/GwzCancelInterruptDesign-DRAFT.md. The hash was `d516bac88df5cef86f91b35c03019db8a8642464` at the start and again at the end. The file is untracked at root `b4b827d6`.
**Date:** 2026-11-01
**Axis:** Safety. The question is what the text lets go wrong:
- orphaned processes, stale locks, partial state and misreported effects;
- races and forced-exit paths;
- whether each named residual is really bounded.

**Verdict: NO-GO.** 0 P0, 0 P1, 7 P2, 7 P3. One P2 (P2-1) is ARCHITECTURAL. I pre-commit to GO on a revision that:
- resolves P2-1 to P2-7 as each correction below states, or by an equivalent that passes the stated closure test;
- adds those closure rows to §6.

---

## 0. Evidence base

- **Pin.** `git hash-object` gave `d516bac8…` at the start and at the end, so the object did not move. Root HEAD is `b4b827d6`. Member heads match the object's citation line:
  - gwz-core `b745f1fa`
  - gwz-cli `c0ba717`
  - gwz-py `2185cc6`
- **Controlling decision.** `GwzTransportReleaseConvergencePlan.md` §10 D-V, :999-1056: OQ2 reversed, OD-C1 to OD-C6 as recommended, resolutions (1) to (3), and the A to D split. OD-C1 to OD-C6 text was read at `9d5a4d4c` (r1 §6, :241-258). OD-C7 text was read at `history/GwzCancellationAmendment-DRAFT.md:695-702`.
- **Source history (not authority).** The round-1 Safety review (`history/GwzCancellationAmendment-ReviewSafety.md`). I checked which of its findings A inherits: P2-6 and P2-7 shape A's §3.1 and §3.4, and P2-11 goes to D.
- **Interface reads.** Design B, `GwzChildLifetimeDesign-DRAFT.md` §3.1 to §3.8, covering registry creation order (OD-B3), `pre_exec`, reaper launch, `kill_killable` and `close`.
- **Code read:**
  - gwz-cli: `lib.rs:160-350`, `globalargs/dispatch.rs:57-96`, `render_exit.rs:82-95`, `append_branch_summary/machine.rs:1-17` (`JsonlSink`), `progress_detail.rs:225-266`, `pager.rs:138-145`, `forall.rs:203-217`, and every `println!`/`eprintln!` site (32).
  - gwz-core: `transport_host/cancellable.rs:1-200`, `git/gitbackend/transport.rs:25-100, :470-500`, `repository_support.rs:30-41`, `workspace_ops/handle_materialize/apply.rs:30-80`, `operation/par_map_per_host.rs:25-200`, `handle_commit.rs:179-186`, `https_auth/runner.rs:200-232`, and the production thread spawns (grep).
  - libgit2: `remote.c:1345-1415`, `fetch.c:195-226`, `repository.c:3805-3845`, `util/filebuf.c:44-60`, `indexer.c:1320-1440`, `checkout.c:1884-1892`, `reset.c:152-168`.
  - git2-rs: `remote.rs`. It has `download`, `update_tips` and `push`, and no `upload`.
  - gwz-py: `src/gwz/bridge.py:20-40, :290-320`, `src/gwz/cli.py:130-160`, `native/src/client_host.rs:150-240`, `client_host/operations.rs:190-230`.
- **Platform facts relied on (not measured here):**
  - Caught signal dispositions survive `fork` and are reset only at `exec`.
  - Rust std uses the fork path whenever `pre_exec` closures exist and does not block signals in the parent around `fork`. This is inferred from std's implementation and not checked line by line in this review.
  - glibc and macOS `posix_spawn` leave no handler window.
  - Windows delivers CTRL_LOGOFF and CTRL_SHUTDOWN to console handlers.
  - CPython 3.11's `asyncio.Runner` turns the main task's SIGINT cancel into `KeyboardInterrupt`.
- Nothing was built, run or modified.

## Problem, requirement provenance, and scope

- **Requested outcome:** D-V reverses OQ2: "Ctrl-C cancels the operation end to end … leaves no orphaned process, stale lock or partial state". Resolution (1) adds:
  - "the forced exit kills children and network work at once, but a local write sequence in progress runs to its next safe point";
  - long local phases get file-granular safe points;
  - SIGKILL, crash and power loss are the stated residual.

  Resolution (2) makes gwz-py call the `Canceller` before waiting. A's part is the interrupt source, Cancel, Force, safe points, the exit status, member reports and gwz-py's caller cancellation.
- **Where each mechanism comes from:**
  - The interrupt module traces to OD-C1.
  - The cancellable entry and token trace to OQ2 and OD-C4.
  - `WriteGate`, the safe points and the Force order trace to resolution (1).
  - Exit 130 and the JSON marking trace to OD-C4.
  - The foreground rule traces to OD-C6.
  - The gwz-py bridge traces to resolution (2).
  - The fetch split (download, then `update_tips`) is demonstrated necessity, because it makes the ref update a write.
  - The write-gate crate follows the small-crate rule.
- **Scope widening:** none found. Every mechanism traces to D-V.
- **Narrowing:**
  - **Disclosed (put to the operator):** O2 (`Wait` children are not killed and may be started after Force) and the "beyond D-V (1)" rows of §7.
  - **Not disclosed:**
    - The premise "parked = no local write" lets Force land in libgit2's local writes inside fetch, clone and push. Only part of that is named (P2-1).
    - Force's "forced exit before the command finished" envelope can misreport writes that Force itself waited to completion (P2-4).
    - In the gwz-py CLI, OD-C4's "JSON response marking the command cancelled" becomes a bare error with no member report (P2-6).

## 1. Findings

### P2-1 (ARCHITECTURAL): "parked" network calls contain local writes, so Force's `_exit` lands inside them and leaves unnamed stale locks and an unpinned clone

- **Location:** §3.4 "Where sequences open and park" (third bullet), Force step 8 ("network calls are parked, so step 5 never waits for one, and step 8 ends them"); §3.5 table row 1 (lists "the clone (`transport.rs:41`)" as a safe-pointed checkout); §7 R4, R6.
- **Violated invariant:** D-V's "no … stale lock or partial state", beyond resolution (1)'s SIGKILL, crash and power-loss residual. Also the `outside` contract: a parked frame is supposed to do no local write.
- **Root cause:** A treats each libgit2 network entry point as pure network. The code says otherwise:
  - **Fetch download.** `git_remote_download` → `git_fetch_download_pack` (`fetch.c:209-226`) commits the pack and its index:
    - idx through a filebuf lock (`indexer.c:1325, :1391`), then renames the pack (`:1428`);
    - then it always calls `git_repository__shallow_roots_write` (`fetch.c:223`), which opens `.git/shallow` through a filebuf. That creates `.git/shallow.lock` (`repository.c:3821`, `filebuf.c:44-60`). It then commits, and removes the file when there are no roots (`:3838-3839`).

    All of this sits inside the frame §3.4 parks.
  - **Clone.** `clone_repo_named` runs `builder.clone` (`transport.rs:67`), then calls `pin_creation_time_filter_neutralization` (`:73`). That function is creation-time only: "Never call this on a repository that may already have materialized content" (`repository_support.rs:30-35`).
    - If `outside` wraps `builder.clone`, as §2 lists it, a clone that finishes after `close()` "blocks for good when f returns". That leaves a complete, checked-out repository without its pins.
    - If `outside` wraps the whole function, the pin write itself can be torn (`.git/config.lock`).
    - Either way, the clone's checkout runs in a parked frame. That contradicts §3.5, which counts it among the seven checkout sites with a safe point.
  - **Push.** libgit2's push includes the tracking-ref update. R5 names its `.lock`; git2-rs has no `upload`, so it cannot be split today.
- **Interleavings:**
  1. `gwz pull`. A member's pack transfer finishes. The second Ctrl-C arrives while `git_repository__shallow_roots_write` holds `.git/shallow.lock`, and `_exit` lands. Every later libgit2 fetch of that member that needs a pack fails `GIT_ELOCKED` ("failed to lock file") after downloading the whole pack. Git's own non-shallow fetch does not notice, so the user sees a gwz-only failure. This window is small, but nothing bounds how often it is hit and nothing recognizes it.
  2. `gwz materialize`. One member's clone completes just after the bound's Force closed the gate. The worker blocks before the pins and gwz exits 130. The member directory is a full repository with no `core.autocrlf=false`/`core.eol=lf`. With Git for Windows' usual `core.autocrlf=true` and CRLF blobs, every such file shows as modified: the "manufactured dirt" the pin exists to prevent. A later materialize refuses the non-empty target (`transport.rs:32`); other commands use the member as is. R6 ("partial new member directory") does not describe this state.
  3. Force during index commit for a large pack leaves `pack-*.idx.lock` or an `.idx` whose pack is still `pack_git2_*`. R4 names only the temp pack.
- **Impact:** stale locks and partial state that D-V forbids. They are classified "no (network ends at once)" or not listed at all. One leaves a member silently mis-configured.
- **Correction:**
  1. State the rule: a frame may be parked only while bytes are in transfer.
  2. **Fetch.** Before Force ends, the transfer-progress callback leaves the parked state when the transfer completes (`received_objects == total_objects`, deltas indexed):
     - if the gate is already closed at that point, the callback returns `false`, so libgit2's own error path removes the temp pack and writes no `shallow`;
     - otherwise the thread becomes `writing`, so the pack commit and shallow write run under the gate.
  3. **Clone.** Write the pins in the `remote_create` callback, where the repository exists and nothing has been materialized. Also leave the parked state at checkout start, so the clone's checkout honours §3.5's safe point.
  4. **Push.** Keep R5, but state the alternative: bind `git_remote_upload` in git2-rs, then split as for fetch.
  5. Add `shallow.lock`, the `.idx` lock and the unpinned clone to §7, or show that they cannot arise.
- **Closure test:**
  - Fault injection (a test hook in the callback) that forces at the end of the transfer, and at the shallow write, of a fixture fetch: no `.git/shallow.lock`, no `*.idx.lock`, and the next fetch succeeds.
  - Force injected just after `builder.clone` returns: the member either does not exist or has both pins.
  - Run on M, L and W.

### P2-2: Force's `wait()` can block without bound on writing threads that are blocked on output or on the scheduler, not on a write

- **Location:** §3.4 gate table (`wait` "returns … once every writing thread is parked, stopped or ended"); "par_map_per_host runs … each worker as `writing`"; Force step 5; the claim "A blocked pipe never holds Force past step 7".
- **Violated invariant:** Force ends the process within a bound, except for the named R8 writes. The design's own sentence also fails.
- **Root cause:** the gate counts a thread as writing for its whole frame, including blocking waits that are not repository writes:
  - **Output (confirmed).** `JsonlSink::deliver` writes each event to a locked stdout and flushes it, from the member worker threads (`append_branch_summary/machine.rs:10-16`). With a `--jsonl` consumer that has stopped reading and a full pipe, the worker blocks in `write` while it is `writing`. Every other worker then blocks on the stdout lock. `StderrProgressSink` does the same on a terminal stopped with XOFF.
  - **Scheduler (depends on frame placement, which §3.4 leaves open).** A per-host worker waits on `changed` for a host slot (`par_map_per_host.rs:176-200`). If the `writing` frame wraps the worker loop, as "each worker as `writing`" reads, then: worker W1 stops for good at a checkout safe point while holding its host slot (its guard never drops); W2, on the same host, waits on the condvar for ever. W2 is neither parked, nor stopped, nor ended.
  - **Entry close (bounded).** After the action returns, `owned.close` (`cancellable.rs:172`) runs finish and shutdown on the dispatch thread while it is `writing`. That adds up to the runtime's cleanup bound to a second-interrupt Force, against row 6.12's 1 s.
- **Interleaving:**
  1. A supervisor runs `gwz --jsonl pull` across 200 members and reads lines.
  2. On its timeout it sends SIGTERM and stops reading while it waits for exit; the pipe holds more than 64 KiB of events.
  3. Cancel happens, the bound passes, and Force runs steps 1 to 4.
  4. Step 5 waits on a worker blocked in `deliver`, with no end.
  5. gwz runs until SIGKILL, which tears whatever write is in flight. Today, SIGTERM ends gwz at once.
- **Impact:** an unbounded Force that rows 6.10 and 6.12 do not catch, because they use a stalled network fixture rather than a stalled consumer. The second Ctrl-C (R14) is the only escape, and it tears writes.
- **Correction:**
  - State the gate's deadlock rule: a writing thread may block only in a write; every other blocking wait runs as `outside`. That covers event delivery, scheduler and slot waits, and the entry's finish and shutdown.
  - `safe_point` and the return from `outside` are never reached with an in-process lock held.
  - Event sinks drop events once Cancel or Force has begun, or hand them to the module's writer thread through a bounded non-blocking queue.
  - The `writing` frame of `par_map_per_host` wraps each item's `f`, not the worker loop.
- **Closure test:**
  - Rows on M, L and W:
    - a `--jsonl` command whose consumer stops reading after the first event, then SIGTERM: exit 130 within bound + 1 s;
    - a two-member, same-host `par_map_per_host` command with a per-host limit of 1: Force while member 1 stops at a checkout safe point returns within 1 s.
  - Each row is red against the literal "worker as `writing`".

### P2-3: the closed gate still admits new work, so Force starts transfers and writes the user has told it to stop

- **Location:** §3.4 `outside` row ("if the gate closed meanwhile, the thread blocks for good when f returns"); the "A local write runs to its next safe point or its end" bullet; §3.2 "Local work … runs on"; §7 R10.
- **Violated invariants:**
  - D-V (1): network work ends at once, and only a write sequence *in progress* runs on.
  - No remote effect starts after Force.
- **Root cause:**
  - `outside` checks the gate only on return, so an `outside` entered after `close()` runs `f`.
  - A sequence "runs to its end", so after its current write it moves on to the next member's network call.
  - `par_map_per_host` workers keep claiming queued items, even though `par_map_per_host_with_cancel` already has a cancel predicate. Its queued items get `on_cancel` without entering `f` (`par_map_per_host.rs:58-60`). A wires nothing to that predicate.
- **Interleaving:**
  - **Native route** (an ordinary build or transport off: R10, where there is no token):
    1. `gwz push` across members, with per-host serial workers.
    2. A second Ctrl-C arrives while worker W finishes member 3's tracking-ref write.
    3. W claims member 4 and enters `outside(push)`; libgit2 starts a fresh push.
    4. Step 8's `_exit` cuts it mid-upload.

    The result is a remote effect that began after the user forced the exit, plus R5's `.lock`.
  - **Checkouts** (switch or pull updates): every newly claimed member writes one file and stops. That multiplies R1 by the worker count.
- **Impact:** possible remote effects and partial trees created by Force itself, on a supported route.
- **Correction:**
  - `outside` checks `is_closed()` on entry. If the gate is closed, it does not run `f`: it returns a `Cancelled` error, or it records a label and blocks.
  - `par_map_per_host` takes the gate (and Cancel) through the existing `_with_cancel` predicate, so queued items report `Cancelled`.
  - The sequence rule of §3.4 reads "runs to its next safe point; a sequence starts no new member after the gate closes". O2 then concerns only `Wait` children of a member whose write had already begun.
- **Closure test:** a native-route, file-remote, three-member push with a slow fixture for member 2, and Force during member 1's tracking-ref update. Members 2 and 3 never contact the remote (server log) and report `Cancelled`. Run on M, L and W.

### P2-4: the forced `--json` envelope reports "forced exit before the command finished" when Force has waited every write to completion

- **Location:** §3.4 Force step 2 (the claim is taken before step 5's wait); §3.6 "After Force"; §9 Q1; row 6.13.
- **Violated invariant:** a response never misreports an effect, and an automation can tell which members committed.
- **Root cause:**
  - Force claims stdout and writes the envelope (no members, `meta: null`) before it waits.
  - A write sequence with no safe point (a commit loop, `stash push`, a gwz-core local backend) then runs to its end inside step 5.
  - The main thread has lost the claim and "writes nothing".
- **Interleaving:** row 6.13 with `--json`:
  1. Three members with an 8 s hook, and one interrupt; the bound passes and Force begins.
  2. All three members and the root commit.
  3. stdout says `interrupted; forced exit before the command finished`, with no members.
  4. An automation re-runs `gwz tag` or `gwz commit` and fails on "exists", or reads the whole command as not run.
- **Impact:** a misreported effect on the exact path §9 Q1 raises, with no fix proposed.
- **Correction:**
  - Take the stdout claim after step 5, not at step 2. Write the notice to stderr at step 2.
  - Once `wait()` returns, if the dispatch sequence ended rather than stopped, let the main thread render the real response, within step 7's budget.
  - Otherwise write the envelope, with a message that does not assert non-completion, for example `forced exit; see stderr for the writes that stopped`.
- **Closure test:** row 6.13 in `--json`. stdout is either the full response with the three members `Ok` plus the `Cancelled` entry, or an envelope whose message does not claim the writes were not made. Run on M, L and W.

### P2-5: OD-C7 (a)'s shape tears writes on an ordinary rapid triple Ctrl-C, and the options given to the operator omit an escape that avoids it

- **Location:** §3.1 "Giving control back"; Force step (1); §7 R14; §9 O3 (a).
- **Violated invariant:** D-V's "no partial state". OD-C7 is open, so the shape of the option is in scope.
- **Root cause:**
  - Dispositions are restored as the first act of Force, so the third interrupt kills gwz by default however soon it comes.
  - The stated purpose is the escape from "a Force waiting on a hung write". That needs only an interrupt that comes after Force has waited, not one milliseconds after the second press.
- **Interleaving:** a user presses Ctrl-C three times within about 300 ms during `gwz switch`:
  - press 1 is Cancel;
  - press 2 is Force, and dispositions are restored;
  - press 3 is SIGINT with the default disposition, so the process dies mid-file in a checkout (not at a safe point), or inside a `git commit` of a `Wait` child (B's reaper kills `Wait` entries at end of file: B §3.4 "Its end").

  The result is a torn file, `index.lock` or `HEAD.lock`, which is R14's residue reached by a normal gesture.
- **Impact:** resolution (1)'s safe points are bypassed by the most common interrupt pattern. No row exercises it (6.13's interrupt comes during a long wait).
- **Correction:** add to O3 a recommended variant: keep the handler through Force, and restore the dispositions only once `wait()` has not returned after a short grace (for example 2 s), or count a third interrupt only if it comes at least that long after Force began. Both keep the hung-write escape. Keep "restore at Force begin" as a listed alternative.
- **Closure test:** three SIGINTs 50 ms apart during a fixture's N-file checkout. Every file is wholly old or wholly new, no `*.lock` remains, and the exit is 130. A further interrupt more than 2 s into a Force blocked on a stalled write still ends gwz. Run on M and L; on W use CTRL_C.

### P2-6: the gwz-py CLI throws away the completed (or partial) result on an interrupt, so it reports `Cancelled` with no members even when the work landed

- **Location:** §3.8 "The bridge" ("waits through `_await_completion` as today and re-raises") and "The gwz-py CLI" (renders `GwzBridgeError("interrupted", …)`); A4.
- **Violated invariants:**
  - OD-C4's "JSON response marking the command cancelled". A4 requires it in both CLIs.
  - Parity with gwz-cli, which after Cancel renders the response with its members (§3.6).
- **Root cause:** `_run_native` re-raises `CancelledError` after the worker has finished, discarding `worker.result()` (`bridge.py:306-316`). `main` then prints only a top-level error (`cli.py:142-156`). R11 lets local and native-route calls run to their end, and so does a transport call that completes before the cancel lands.
- **Interleaving:**
  1. `gwz-py push --json` across members; Ctrl-C after members 1 and 2 have pushed.
  2. The transport cancel fails member 3 with `Cancelled`.
  3. The CLI prints `{"code":"Cancelled",…}` with no members.

  The equivalent `gwz commit` (local; cancel refused) commits everything and still prints only "interrupted".
- **Impact:** a misreported effect and a parity break that the design does not disclose.
- **Correction:**
  - The CLI path (not the library) keeps the completed native result. For example, `_run_native` returns it when a flag says the caller is the CLI, or attaches it to the raised exception.
  - `main` then renders the response with the `Cancelled` entry first among the own errors, as §3.6 does.
  - The library keeps standard asyncio semantics, and §3.8 states that.
- **Closure test:** row 6.16 is extended. `gwz-py --json` on a three-member local `commit`, interrupted mid-run, prints a response whose members show what committed, plus the `Cancelled` entry, and exits 130. Run on M, L and W.

### P2-7: a caller's cancel that arrives before the operation registers is refused, ignored and lost, so the call then runs uncancelled to completion

- **Location:** §3.8 "The bridge" ("calls `cancel_operation(id)` … ignoring its refusal").
- **Violated invariant:** resolution (2): the caller's cancellation goes through the `Canceller`, and Ctrl-C cancels end to end.
- **Root cause:**
  - Registration happens inside the native call, on the `to_thread` worker (`client_host.rs:216-231`, via `scope`/`network`).
  - `cancel` refuses an unknown id with `InvalidRequest` (`operations.rs:199-207`) and keeps no tombstone.
  - The shielded executor job still runs later.
  - The window lasts from `create_task` to registration, and has no bound when the default executor is saturated. gwz-py's own note says slot-waiting calls hold executor threads.
- **Interleaving:**
  1. An application has 32 or more concurrent calls, and fetch #33 is queued in the executor.
  2. The caller cancels; `cancel_operation` gets `InvalidRequest`, which is ignored.
  3. The job starts, registers, and fetches to completion while the caller believes it was cancelled.
- **Impact:** a silent loss of cancellation in an embedding application, and a misreported effect.
- **Correction:** `cancel_operation` for an id not yet registered records a pre-cancel, keyed by the id the bridge already has. A later `register` of that id then refuses with `Cancelled`, which the existing waiting-state path already supports (`live.cancelled`). The tombstone is evicted at the call's return. Alternatively, register before submitting to the executor.
- **Closure test:** a row with a saturated executor (`max_workers=1` and one blocking call) that cancels a queued fetch. The fetch ends `Cancelled` without contacting the fixture. Run on M, L and W, on 3.10 and 3.11+.

### P3-1: the handler runs in forked children before `exec`, so one Ctrl-C can count twice and go straight to Force

- **Location:** §3.1 Unix (the handler writes a byte unconditionally) and B §3.2 (registered children use `pre_exec`, which forces std's fork path).
- **Root cause:** a caught disposition survives `fork` until `exec`, and the pipe's write end is open in the child until `exec`. A terminal SIGINT that lands between `fork` and `setpgid`, or for a `Process` child until `exec`, runs A's handler in the child as well as in gwz. That puts two count bytes in the pipe.
- **Interleaving:** a helper spawn during a fetch and a single Ctrl-C: the reader sees Cancel and then Force, and skips the graceful path. This is plausible (inferred from std), not observed.
- **Correction:** the handler compares `getpid()` (async-signal-safe) with the installer's PID, saved at install, and does nothing otherwise.
- **Closure test:** a stress row on M and L that spawns registered children in a loop and sends one SIGINT to the group: only Cancel follows.

### P3-2: a deferred Force has no escape, so a foreground wait makes gwz end only by SIGKILL

- **Location:** §3.7 (`Interrupts::foreground`: "a Force is deferred until it returns"); §3.1 (dispositions are restored only when Force begins); row 6.15.
- **Root cause:** while a pager or `forall` member runs, every SIGINT, SIGTERM and SIGHUP is swallowed, with no restore. A `kill -TERM <gwz>` from another shell or a supervisor reaches only gwz, and gwz waits for as long as the member runs. Today it exits at once.
- **Correction:** list this as a §7 row. Alternatively, apply P2-5's grace rule: after N interrupts, or N seconds into a deferred Force, restore the dispositions.
- **Closure test:** a `forall` member that ignores SIGINT, and SIGTERM sent to gwz three times: the row shows whichever behaviour is chosen.

### P3-3: R5 claims a "may have received it" notice on the Cancel path, but the notice exists only on Force

- **Location:** §7 R5 ("Cancel or Force during a push | the notice's `may have received it`"); §3.6 (notices are Force-only).
- **Impact:** after Cancel during a push, both human and JSON output show only `Cancelled`. The catalog remedy (`ErrorCatalog.md:35`) is the only hint. The recognition A claims does not exist.
- **Correction:** add the push line to the Cancel rendering for members whose push had started, or correct R5's recognition column.

### P3-4: nothing checks the read-only list, and a writer on it would be torn by Force

- **Location:** §3.4 "an explicit read-only list"; §5 3.1c.
- **Root cause:** a request on the list runs with no `writing` frame, so Force does not wait for it. No row checks that each listed request writes nothing (index refresh, workspace artifacts, `.gwz` caches).
- **Correction:** state the rule for membership. Add a row that runs each listed request under a write-detecting gate (`safe_point` or `writing` calls panic in a test build), or one that checks for no repository mtime change.

### P3-5: Windows CTRL_LOGOFF and CTRL_SHUTDOWN are unspecified

- **Location:** §3.1 Windows.
- **Root cause:** the handler's return value for these events is not stated. A `FALSE` return falls to the default `ExitProcess` mid-write. That residual is not in §7.
- **Correction:** map both to Force, like CTRL_CLOSE, or list them as a residual.

### P3-6: R17 is avoidable inside 3.1c's own files but is classed "beyond D-V (1)"

- **Location:** §7 R17.
- **Evidence:** gwz-cli has 32 `println!`/`eprintln!` sites, all in files 3.1c already edits or neighbours (`lib.rs`, `dispatch.rs`, `forall.rs:208`, `diff_exec.rs`, `hook_exec.rs`). gwz-core's production prints are almost all in tests.
- **Correction:** 3.1c replaces them with `writeln!` that ignores errors, and a lint bans the macros in gwz-cli. Then drop R17, or downgrade it to hooks only.

### P3-7: a `--server` command's behaviour between 3.1c and C's piece is undefined

- **Location:** §3.2 "`--server` commands"; §5 3.1c "After" (C is not listed); §4 C ("taking A's `Interrupts` value").
- **Root cause:** C's 3.1 work needs A's `Interrupts` value, which only 3.1c creates. Until then, Cancel's effect on a `--server` command is unstated.
- **Correction:** state the fallback: for a `--server` command, Cancel starts the bound and Force runs steps 1 to 8. Order C's piece explicitly after 3.1c, or make it part of 3.1c.

## 2. Verified citations

| Citation | Result |
|---|---|
| `gwz-cli/src/lib.rs:160-178, :323-326, :340-344, :228-300` | Correct (parse at :170, response `println!` and exit at :324-326, execution error :341-347, own-lifecycle commands :222-300). |
| `gwz-cli/src/globalargs/dispatch.rs:57-89, :91-96, :82-87` | Correct (native non-cancellable entries; hostless `Git2Backend::new()`; the `eprintln!` pending-jobs line at :83). |
| `src/transport_host/cancellable.rs:10-11, :40-88` | Correct. The action runs on the calling thread (`:166-167`), which supports the `ThreadId` gate; `close` runs after the action on the same thread (P2-2). |
| `render_exit.rs:82-95`; `MachineOutput.md:954-964, :669-670, :141-142` | Correct. |
| `context.rs:80` (`CLEANUP_BOUND` 5 s); `https_auth.rs:28` (`CLEANUP_GRACE` 500 ms); `runner.rs:213-228` | Correct. |
| `libgit2 checkout.c:1886-1891`; `reset.c:154, :159, :167`; `remote.c:1352-1409` | Correct. `git_remote__download` also commits the pack and writes `shallow` (`fetch.c:209-226`), which the design omits (P2-1). |
| `transport.rs:32, :67` (clone) | Correct. `:73`'s pin write after the clone is not mentioned (P2-1). |
| `pager.rs:138-145`; `forall.rs:203-217` | Correct. |
| `handle_commit.rs:179-186` | Correct (one serial member loop). |
| `bridge.py:28-35, :298-316`; `cli.py:142-156`; `client_host.rs:164-171, :226-231`; `operations.rs:194-227` | Correct. Registration happens on the worker thread (P2-7). |
| `ErrorCatalog.md:35` | Correct ("A result's first error;"; the remedy names a push's possible remote effect). |
| `par_map_per_host.rs:35` | Correct. `_with_cancel` exists (`:58-60`) and is unused by A (P2-3). |
| Not verified on this axis | `checkout.c:2686`; `crates/copy-contract`, `refcopy`, `local-import` line numbers; `owner.rs:77, :163, :253-276`; `file_worker.rs:91-98`; `asyncio/runners.py:150-157` (no CPython source read); `stash_support.rs`, `repository.rs:108-169` line numbers; plan :315, :712, :717-720. |

## 3. Risks and next action

- **Highest risk:** P2-1. Force's premise that a parked call does no local write is false for every libgit2 network entry point. It is architectural: the gate's park/write boundary has to follow the transfer, not the API call.
- **Second:** P2-2 and P2-3. Force can either hang with no bound (blocked output, scheduler waits) or start new remote effects (a closed gate that still admits work). Both are code-traced, and §6 rows miss both.
- **Third:** the report-shape defects P2-4 and P2-6. These are misreported effects on the Force and gwz-py paths. P2-5 and P2-7 each have a bounded fix.
- **Next action:**
  1. Revision 2 resolves P2-1 to P2-7, adds the closure rows and settles the P3 items.
  2. The operator receives P2-5's grace variant alongside OD-C7 (a).
  3. Re-review on this axis against the new blob.

## 4. Out-of-scope observations

- **Design B (interface, not A's defect):** B's reaper kills `Wait` entries at end of file (B §3.4 "Its end"). Every path in which gwz dies by a default disposition after Force has begun therefore tears a `git commit` mid-write (R14, P2-5). B and A should cross-reference this in R14.
- **Existing code (limitation):** `materialize`'s "F2 … rolled back on any mid-batch failure (Q6 reject-partial)" (`handle_materialize/apply.rs:30-31`) cannot run under Force's `_exit`. That rollback guarantee becomes R2/R6 territory for D's recognizers.
- **git2-rs (limitation):** there is no `Remote::upload` binding, so no push can split its upload from its tracking-ref update. That keeps R5's `.lock` a residual until git2-rs gains it, which is a separate, authorized change.
