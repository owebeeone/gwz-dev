# GwzCancelInterruptDesign — remediation plan, round 1

Date: 2026-11-01. Status: **remediation plan for the lane owner; it authorizes nothing beyond revision 2 of design A.**
Drafted read-only except for this file and the design; no git operation was run (the lane owner commits).

- **Object:** `dev-docs/GwzCancelInterruptDesign-DRAFT.md`, revision 1, blob `d516bac8`, as committed at root `d146bc9a`
  (unchanged at `8ade43ac` and `48feeaad`).
- **Reviews:** [Consistency](GwzCancelInterruptDesign-ReviewConsistency.md), NO-GO, 3 P2 (P2-1 and P2-3 ARCHITECTURAL)
  and 9 P3; [Safety](GwzCancelInterruptDesign-ReviewSafety.md), NO-GO, 7 P2 (P2-1 ARCHITECTURAL) and 7 P3. No P0 or P1.
  IDs keep each review's numbering with a prefix: `C-` Consistency, `S-` Safety.
- **Architectural count: 2 root causes** (three flagged findings), as the operator's record says (plan :1055-1056):
  (1) the cancel and the write gate were carried on the per-operation entry that plan 3.1 replaces and 3.3 removes
  (C-P2-1); (2) the park/write boundary followed API calls, not transfer and lock state, so Force's exit could land in a
  held Git lock file (C-P2-3 and S-P2-1, one root cause found blind from two sides).
- **Result:** revision 2, edited in place, then amended by the fix pass below (gaps G1-G13): 399 lines (cap 400);
  status "revision 2 (2026-11-01) … for round-2 review by fresh reviewers"; its §10 maps every C-/S- ID to its sections,
  and this plan's "Fix pass" table maps G1-G13.
- **Verification.** Every file:line revision 2 adds was re-read at gwz-core `b745f1fa`, gwz-cli `c0ba717`, gwz-py
  `2185cc6`, gwz-transport `281a28d`, libgit2 `b172e3d18`, CPython 3.10.15 and 3.11.16
  (`~/.asdf/installs/python/3.1{0.15,1.16}/lib/python3.1x/asyncio/runners.py`), plan and 2.3 plan at root `48feeaad`.
  Facts that changed a disposition: libgit2 calls `push_update_reference` after the remote's report and before the
  tracking-ref update (`remote.c:3032-3038, :3064-3067`), so no git2-rs `upload` binding is needed (Safety's §4 note);
  libgit2's clone removes what it made on any error (`clone.c:666-673`); an uncommitted pack is unlinked when the indexer is
  freed (`indexer.c:1469`); `HostOwner::close` seals admission at the call
  (`gwz-transport/crates/gwz-connection/src/host/owner.rs:21-32`); gwz-py's own design already promises that a cancel
  before start is refused `Cancelled` (`gwz-py/dev-docs/GwzPyPerOperationTransportDesign.md:104`) and keeps a bounded
  record of 64 (`client_host/operations.rs:27`). Refined reviewer fact: the process-globals check already runs in all
  five repositories (gwz-transport through `scripts/run_tests.py:316-327`, gwz-git :472-479, gwz-py through
  `gwz-py/src/tests/test_process_globals.py:11-23`), not only gwz-cli's CI (C-P3-3). Fix-pass facts, re-read at the same
  heads: libgit2's local transport writes the remote's pack and refs during a push (`transports/local.c:426, :354`;
  chosen for `file://` or a directory, `transport.c:36, :88-89`); `HostOwner::close` returns a `HostClose` future
  (`host/owner.rs:21-32, :50-59`), bounded by `close_bound_ms` 5 s (`host/config.rs:22, :36`); `ProcessGate` is minted only
  in `composition.rs:30-31`; `merge_prepared.rs:280` (`repo.merge`) runs inside the transaction locked at :248-249; gwz-cli
  writes stdout/stderr directly at `append_branch_summary/machine.rs:13`, `lib.rs:167, :174, :279`, `pager.rs:130, :146`,
  `log_exec.rs:33, :36`, `progress_detail.rs:233`; gwz-cli's CI runs no clippy (`gwz-cli/.github/workflows/ci.yml`);
  gwz-py's Python has no `signal` call today; `forall`'s member wait is `forall.rs:260`.

## Blind convergences

1. C-P2-3 = S-P2-1: Force's exit lands inside a held Git lock file (ref transactions around merge checkouts; filebuf locks
   inside fetch, clone and push), and §7 classed the residue "no".
2. C-P2-2 = S-P3-7: the interface to C cannot be met in the plan's order (the server lifecycle in Phase 2 and `--server`
   commands both need A's module or value before 3.1c creates it).
3. C-P3-4 = S-P2-2 (part): `par_map_per_host`'s frame is unspecified: no plumbing (C), and a frame around the worker loop
   deadlocks on slot waits (S).
4. C-P3-6 = S-P2-4 (part): Force's output claim misreports (an unmarked document or no marking; an envelope asserting
   non-completion after Force waited every write to its end).
5. C-P3-5 = Safety §4 note on B: the hand-off with B's `close()` and reaper (precondition wording; the reaper's kill of
   `Wait` entries when gwz dies, which R14 reaches).

## The operator's decisions (plan §10 D-V, "Designs A and B, round 1", :1055-1062) and how revision 2 applies them

- **(i) Force never exits mid-write.** A3 and §3.4: Force closes the host, kills `Kill` children, then `wait()` has no time
  bound; the exit (step 8) happens only when every writing thread is parked (in transfer only, G1), stopped at a safe point
  or done. Removed because they existed only for an exit landing mid-write: revision 1's residuals for writes cut by the
  exit (R5's tracking-ref `.lock`, the torn index commit and `shallow.lock` of S-P2-1, the unpinned clone), revision 1's
  R8 framing as a narrowing, O2 and Q1. R8 now states decision (i)'s own residual: a hung write ends only by SIGKILL.
- **(ii) `git commit`/`git tag`, hooks and signers finish.** A4 and §3.4 cite it; O2 is gone.
- **(iii) O1** (one interrupt site; the 2.3i server uses A's module): §3.1 and the new Phase 2 step 2.3s, with §8 rows 5-7
  amending 2.3i, 2.3k and the 2.3 plan's waves. **O3 / OD-C7 (a)-(d)** as recommended: §3.1 "Giving control back" (a),
  SIGQUIT's default (b), R9 (c), §3.8 and R15 (d). **B's OD-B2 (a), OD-B3 (a)** are B's; A relies on OD-B3 (a) for the
  order in `run` (§4 point 4).
- **Tension raised, not resolved by A (§9 O5).** (i) says "only SIGKILL ends a hung write"; OD-C7 (a), accepted in the
  same bullet, lets any interrupt after Force began end gwz by its default disposition, which can tear a write that is not
  hung (S-P2-5). Revision 2 applies both as written (R14) and names every place that changes if (i) withdraws (a) (§9 O5).

## Hand-off points with design B (B is being revised in parallel)

A revision 2 (§4) relies on these points. Points 1-2 follow B **revision 2's** §3.7 (`GwzChildLifetimeDesign-DRAFT.md`
:224-231, on disk, uncommitted), which changed them from revision 1; points 3-6 are as B revision 2 states them
(§3.1, §3.6, §4); point 7 is a new edge B has yet to write.

1. `kill_killable()` ends every live `Kill` entry at once, leaves `Wait` and `OwnInput` children, and refuses later
   `Kill` spawns `Closed` (B §3.7, row B22). A rev 1 let such a spawn run until the reaper; A rev 2 adopts B's refusal
   (Force step 4: the member reports `Cancelled`).
2. gwz-cli calls no `close()` (B §3.7: "gwz-cli needs no `close()`"). Force no longer calls it (step 6 is now the
   claim): every `Wait` child runs inside a write frame and the closed gate refuses new frames, so none starts after
   Force's `wait()` (A §3.4).
3. `run` creates the registry before any thread (B §4 3.1; OD-B3 (a)), so before A's install.
4. `process_safety_with_children()` mints the `ProcessGate` in `composition.rs` and `Git2Backend::with_process_gate`
   carries it to every backend (B §3.1, §4); A's 3.0d adds the `write_gate` field there (A §3.4 "Carriage").
5. `terminate` returns the lookup's own outcome and retains the tree as pending cleanup (B §3.6); gwz-cli reports it as
   the host close's `Pending` (A §3.3). B's §3.6 still calls this A's `pending_local_work`, revision 1's word; B should
   say "the host close's `Pending` (A §3.3)".
6. B changes no disposition in gwz's process; its forked-child allowlist entries take A's kind `signal`.
7. **New edge:** B's 2.3r-2 and A's 2.3s both edit `scripts/checks/check_process_globals.py`; 2.3r-2 runs after 2.3s
   (A §4 point 7, §5, §8 row 7). B's §4 lists 2.3r-2's "After" as 2.3r-1 only and should add 2.3s (wave 3 already
   follows wave 2, so the waves agree).

B relies on A's Force steps 4 (`kill_killable`), 5 (unbounded wait) and 8 (exit after it); A rev 2 keeps these numbers.

## Finding disposition

Dispositions: **Fix** (design change), **Fix-text** (text, cite, cost, residual or operator question only), **Dispute**
(with evidence), **Record** (named residual or decision, no mechanism).

Counts: Fix 14 (C-P2-1, C-P2-2, C-P2-3, C-P3-4, C-P3-6, S-P2-1, S-P2-2, S-P2-3, S-P2-4, S-P2-6, S-P2-7, S-P3-1, S-P3-4,
S-P3-6); Fix-text 11 (C-P3-1, C-P3-2, C-P3-3, C-P3-5, C-P3-7, C-P3-8, C-P3-9, S-P3-2, S-P3-3, S-P3-5, S-P3-7); Record 1
(S-P2-5); Dispute 0 (C-P3-3's run fact is refined with evidence inside its row).

| ID | Finding | Disposition | Closure test | Rev 2 § |
|---|---|---|---|---|
| C-P2-1 | (ARCH) 3.1c cancels through the per-operation entry that 3.1 replaces and 3.3 removes | Fix. Cancel takes the command's `HostOwner` from a slot shared with the reader and calls `close()`, which seals admission and cancels the host's operations (`host/owner.rs:21-32`; contract :269), putting the returned `HostClose` back in the slot; the dispatch thread, after its write frame, empties the slot (closing an owner still there) and awaits the `HostClose` (fix pass G3); plan 3.1 maps `Closing(HostClosing)` and a close-ended operation to `Cancelled`; the gated cancellable entry is dropped from 3.0d; the write gate is a field of the `ProcessGate` composition mints (fix pass G9) and reaches every backend through B's `with_process_gate`; no step edits the trunk's `transport_host/mod.rs` | §3.2 and §3.4 name only the 3.1 host API, composition's minting and B's setter, each cited; no step builds on what 3.3 removes; §8 row 4 amends plan 3.1 (3.3's text needs no change); row 6.11's second member is refused admission; row 6.8's `Pending` reaches stderr | §2, §3.2, §3.4 "Carriage", §5, §8 row 4, rows 6.8, 6.11 |
| C-P2-2 | O1's route impossible in the plan's order (server lifecycle in Phase 2, module in Phase 3) | Fix. New Phase 2 step 2.3s (wave 2, lane 2, after 2.3f) builds the module and the `signal` kind; 2.3i installs no handler and takes a stop input; 2.3k passes it from `Interrupts` and owns the signal rows; O1 is decided | the plan graph puts 2.3s before 2.3k and every row that sends a signal; no 2.3 plan text installs a handler in gwz-core | §3.1, §5, §8 rows 5-7, rows 6.1-6.3 |
| C-P2-3 | (ARCH) Three of the checkout safe points sit inside a held ref transaction; R1 hides the `.lock` | Fix. Invariant G2: no stop inside a held Git lock file; the four checkouts under a transaction (`merge_prepared.rs:266, :339`, `merge_recovery.rs:175, :251`) run to their end; under decision (i) Force waits for them (R8 if hung) | row 6.9: Force during a fast-forward merge and both recovery checkouts leaves no `refs/**/*.lock` and the next `gwz merge --abort` succeeds | §2, §3.4 (G2), §3.5, row 6.9 |
| C-P3-1 | §8 omits clauses the design contradicts; `:141-142` off by one | Fix-text. §8 rows for MachineOutput :66-69 and :669-673 (whole sentence), `GwzCoreSessionDesign.md:687`; cite `:142-143` | every live clause §3.6 or §3.4 contradicts is in §8 with exact lines | §2, §8 rows 9-12 (items 10-12), 14 |
| C-P3-2 | Inventory misses `merge_prepared.rs:339` and `preservation.rs:344`; "seven" uncounted | Fix-text. `:339` is one of §2's run-to-end checkouts; the expected-conflict `repo.merge` (`merge_prepared.rs:280`, worktree and index, inside the transaction locked at :248-249) joins them, making five run-to-end writes (fix pass G10); `preservation.rs:344` joins `stash_save` in §3.5; the clone's checkout now has a safe point (its frame unparks at transfer end) | §2 and §3.5 together match a grep of `checkout_tree`, `reset(`, `.merge(` and `stash_save` in non-test gwz-core source (`fixture_native.rs` is `#[cfg(test)]`, `gitbackend.rs:445-446`); §3.5's closing sentence claims a bounded set only for sequences the table does not name | §2, §3.5 |
| C-P3-3 | The `signal` check's runs and B's entries conflict with A's text | Fix-text. The check already runs in all five repositories (§2, evidence above), so no wiring is added; only gwz-cli's allowlist gets A's entry; B's forked-child entries in gwz-core's allowlist take the kind `signal` in 2.3r-2; 2.3s owns the script and its test; because both steps edit `check_process_globals.py`, 2.3r-2 gets an explicit "After 2.3s" edge (§5, §8 row 7; fix pass G6) | row 6.2 names only runs that exist; gwz-core's check stays green with B's entries; §5 and §8 row 7 state the shared file and its order | §2, §3.1 "Check", §4 point 7, §5, §8 row 7, row 6.2 |
| C-P3-4 | `par_map_per_host` plumbing unspecified (only a thread-local avoids it); member lock is the served path's | Fix. New `par_map_per_host_gated(gate, …)` beside the unchanged public function; the seven calls pass `backend.write_gate()` (`GitRepository::write_gate()`, provided, open); the six caller files are in 3.0d's Owns; the member-lock wait is dropped as C's | 3.0d's Owns names every file whose signature or call changes; no thread-local | §2, §3.4, §5 (3.0d) |
| C-P3-5 | Hand-off wording with B (`close()` precondition; order in `run`; Windows job) | Fix-text. Aligned with B revision 2 §3.7 (fix pass G6): gwz-cli calls no `close()` (every `Wait` child runs inside a write frame, which the closed gate refuses after step 5); a `Kill` spawn after `kill_killable()` is refused `Closed` and its member reports `Cancelled`; `run` creates B's registry (3.1), then installs | A and B say the same about `close()` (not called by gwz-cli), about a late `Kill` spawn (refused) and about the order in `run` | §3.1 "Install", §3.4, §4 |
| C-P3-6 | OD-C4 marking missing on forced `--jsonl` and on a Windows CTRL_CLOSE race | Fix (claim rule) and residual. The claim decides stdout and the status: 130 only when an interrupt preceded the main thread's claim, so a document written after the command finished is never re-marked; a forced `--jsonl` stream without its response is R13, "beyond" (OD-C4 marking) | a §7 row names each case; row 6.12 shows the `--jsonl` case | §3.6 "The claim", R13 |
| C-P3-7 | SIGTERM and SIGHUP mapped to 130 without D-V provenance | Fix-text: put to the operator as O4, (a) recommended (130 for every stop request), (b) 128 + signal, (c) SIGINT only. The design names what the answer changes (§3.1's signal lists, §3.6 "Status", §8 row 8, the O4 cells of R9 and R15, rows 6.11-6.12's legs); §8 row 8 is marked "subject to §9 O4"; R9 and R15 mark as decided only what OD-C7 (c) and (d) name (fix pass G8) | §9 states the authority question with options and its blast radius; no §7 or §8 cell claims a decision OD-C7 does not hold | §9 O4 |
| C-P3-8 | Notice modes contradict (human only vs every mode) | Fix-text. Every notice is on stderr in every output mode, unlike `note:` lines | §3.2, §3.6 and §8 row 9-12 item (9) agree | §3.6 "Notices", §8 row 9 |
| C-P3-9 | Citation and fact errors (R17 closed stderr; 3.10 runner; `ClientHost.call`; f9 path; 3.0d After; graph edges; "adds it if missing") | Fix-text. R17 is gone (S-P3-6); 3.10 cited as `runners.py:44-49, :55-63` with its in-step case in R11; `NativeCoreBridge.call` (`bridge.py:318`); the f9 path is absolute with its heading; 3.0d After "Phases 1, 2"; the graph keeps `3.0, 3.0b → 3.3`; the pending count is the host close's `Pending` | each cite re-verified at its head | §1 A6, §3.3, §3.8, §5, §8 row 1, R11 |
| S-P2-1 | (ARCH) Parked network calls contain local writes (pack commit, `shallow.lock`, clone pins, push tracking refs) | Fix. G1: park only while bytes are in transfer. Fetch: the transfer callback unparks once every object is received, so the pack commit and `.git/shallow` are writes, and on `Closed` returns `false` (libgit2 unlinks the pack, `indexer.c:1469`). Clone: a write frame whose `remote_create` writes the pins, then parks; the transfer callback unparks; on `Closed` libgit2 removes the clone (`clone.c:666-673`). Push: the first `push_update_reference` unparks, so the tracking-ref update is a write (no git2-rs change); a push to a local-path remote, whose upload writes the remote's pack and refs (`local.c:426, :354`), is a write throughout (fix pass G1); an empty remote's clone stays writing (G11) | row 6.6: Force injected at a transfer's end, in the pack commit and in the shallow write leaves no `shallow.lock` or `*.idx.lock` and the next fetch succeeds; Force after a clone's transfer, `builder.clone` or in an empty remote's clone leaves no member or a pinned one; row 6.7: Force in a file-remote push's pack write, ref write and tracking-ref update leaves no `*.lock` in the member or the remote, whose `git fsck` passes; M L W | §2, §3.4, rows 6.6-6.7, R4-R6 |
| S-P2-2 | Force's `wait()` blocks without bound on output or scheduler waits | Fix. G1 (a writing thread blocks only in a local write, a `Wait` child, or a clone's pre-transfer I/O that the host close fails) and G2 (no stop with an in-process lock held); the writer owns all output and a sender never blocks after Cancel; 3.1c moves every direct handle write (`JsonlSink` at `machine.rs:13`, `lib.rs:279`, `pager.rs:130, :146`, `log_exec.rs:33, :36`, `StderrProgressSink`) onto `Output` and enforces it with `check_direct_output.py` (fix pass G5); `par_map_per_host_gated` frames each item's `f`, not the worker loop; the host's close is awaited outside any frame | row 6.12: a `--jsonl` consumer stops reading, then SIGTERM: exit 130 within the bound plus 1 s; row 6.5: two same-host members, per-host 1, member 1 stopped: `wait` returns within 1 s; both red against a frame around the worker loop | §3.1 "Writer", §3.4, rows 6.3, 6.5, 6.12 |
| S-P2-3 | A closed gate still admits new work | Fix. G3: `writing`, `park` and `outside` are refused after `close()`; `par_map_per_host_gated` passes `is_closed` as `cancelled`, so queued members report `Cancelled`; serial member loops (commit and any 3.0f's inventory finds) get a safe point at each member's head | row 6.7: native-route three-member push, Force during member 1's tracking-ref update: members 2 and 3 never contact the remote and report `Cancelled`; row 6.10's commit case; M L W | §3.4 (G3), §3.5, rows 6.7, 6.10 |
| S-P2-4 | The forced envelope says "before the command finished" after Force waited every write to its end | Fix. The dispatch takes the claim as the last act inside its write frame, and `wait()` returns only after that frame ends, so a finished dispatch always holds it before Force's step 6 (fix pass G2); Force then waits up to the close bound plus 1 s for the main thread, which renders the real response with the `Cancelled` entry; Force takes the claim only when it is free (a stopped or parked dispatch); the envelope's message no longer asserts non-completion | row 6.13: one member, the full response with the member `Ok`, also over 100 runs with the second interrupt at the hook's exit; three members, the envelope `interrupted; forced exit, see stderr for what stopped`; red against a claim taken after the frame; M L W | §3.6, row 6.13 |
| S-P2-5 | OD-C7 (a)'s shape tears writes on a rapid triple Ctrl-C | Record. The operator accepted O3 / OD-C7 (a) as recommended (restore at Force's start) after round 1 (plan :1061-1062); the grace variant is a new mechanism nobody chose. The 50 ms case is R14 and is shown by row 6.13; the tension with decision (i) goes to the operator as O5 with the change it would make | row 6.13 shows the decided behaviour (an interrupt 50 ms after the second ends gwz by default); §9 O5 names the alternative and every place it changes (§3.1, §3.4 step 1, §3.7, R14, rows 6.13 and 6.15; fix pass G7) | §9 O5, R14, row 6.13 |
| S-P2-6 | The gwz-py CLI drops the completed result on an interrupt | Fix. `_run_native` re-raises `CancelledError` carrying the decoded response as `gwz_response`; the CLI's `run` renders it with the `Cancelled` entry first among its own errors and returns 130; `main` renders only if `run` rendered nothing; a library caller still sees `CancelledError` | row 6.16: `gwz-py --json` on a three-member local `commit` interrupted mid-run shows what committed, the entry, exit 130; 3.10 and 3.11+; M L W | §3.8, row 6.16 |
| S-P2-7 | A cancel before registration is refused, ignored and lost | Fix. The bridge calls a native `cancel_call(id)`: a live operation is cancelled; an id not yet live is recorded (last 64, as `operations.rs:27`) and its registration refused `Cancelled`, as the gwz-py design already says (:104); `Client.cancel_operation` keeps its refusals | row 6.16: `max_workers=1` and one blocking call, a cancelled queued fetch ends `Cancelled` without contacting the fixture; 3.10 and 3.11+; M L W | §1 A8, §3.8, row 6.16 |
| S-P3-1 | The handler runs in forked children before `exec`; one Ctrl-C can count twice | Fix. The handler does nothing unless `getpid()` equals the installer's PID (saved at install, an allow-listed handler global) | row 6.1: registered children spawned in a loop and one SIGINT to the group give one Cancel; red by mutation | §3.1 "Unix", row 6.1 |
| S-P3-2 | A deferred Force has no escape during a foreground wait | Fix-text. Force's step 1 (restore dispositions) runs at once during a foreground wait; only its later steps wait | row 6.15: a `forall` member ignoring SIGINT, three SIGTERMs: the third ends gwz, the member runs on | §3.1, §3.7, row 6.15 |
| S-P3-3 | R5 claims a Cancel-path notice that exists only on Force | Fix-text. R5's recognition is the member's `Cancelled` with the catalog's remedy on Cancel, and Force's `may have received it` | R5's column matches §3.6 | R5 |
| S-P3-4 | Nothing checks the read-only list | Fix. Membership rule (a request that writes no file) and a row comparing every fixture file's bytes and modification time; the list stays an exhaustive `match` | row 6.14, red by mutation (a writer listed) | §3.4, row 6.14 |
| S-P3-5 | Windows CTRL_LOGOFF and CTRL_SHUTDOWN unspecified | Fix-text. Both are treated as CTRL_CLOSE (a Force byte, then sleep); the OS's end is R9 **(OS)**; R9 marks only CTRL_CLOSE decided (OD-C7 (c)) and puts LOGOFF and SHUTDOWN with O4 (fix pass G8) | §3.1 and R9 name both, with their provenance | §3.1 "Windows", R9, §8 row 8 |
| S-P3-6 | R17 is avoidable in 3.1c's own files | Fix. The module's writer owns stdout and stderr; 3.1c moves every gwz-cli write, macros and direct handles, onto `Output`; enforcement is `gwz-cli/scripts/check_direct_output.py`, a `cfg`-blind token check in gwz-cli's CI (whose CI runs no clippy), allowed only in `src/interrupt/`, `src/hook/` and `src/hook_exec.rs` (whose OS default is R12); it lands with the migration in 3.1c, not in 2.3s (fix pass G5); R17 is removed | row 6.2 (the check fails a `println!` and an `io::stdout()`, also in a `cfg(windows)` branch on Linux) and row 6.12 (closed stderr, unread stdout: exit 130, no panic) | §3.1 "Writer", §5 (3.1c Owns), rows 6.2, 6.12 |
| S-P3-7 | A `--server` command's behaviour between 3.1c and C is undefined | Fix-text. Cancel closes the command's carrier-backed `HostOwner` the same way; until C's piece lands the local rule (bound, Force) applies; what the served core does is C's | §3.2 states the interim rule | §3.2 "`--server`" |

## Fix pass (gaps G1-G13 found after revision 2 was drafted)

The lane's check of revision 2 found thirteen gaps. They are this plan's IDs G1-G13 (not design §3.4's invariants
G1-G3). Each is closed in revision 2 in place. To stay under 400 lines, revision 2 merged §8's MachineOutput rows 9-12
into one row "9-12" (items keep their numbers) and tightened prose without dropping a clause.

| Gap | Problem | Disposition | Closure test | Rev 2 § |
|---|---|---|---|---|
| G1 | S-P2-1 open for file remotes: libgit2's local transport writes the remote's pack (`local.c:426`) and refs (`:354`, `<ref>.lock`) during a parked upload, so the exit can leave them torn | Fix. A push to a local-path remote (`file://` or a directory, `transport.c:36, :88-89`) is a write throughout; Force waits for it (R8 if hung) | row 6.7: Force in member 1's pack write, ref write and tracking-ref update leaves no `*.lock` in the member or the remote, whose `git fsck` passes with the push whole or absent; red against a parked local-path upload | §2, §3.4 "Push", row 6.7, R8 |
| G2 | The claim can race: `wait()` returns at the frame's end, before the dispatch is marked returned | Fix. The dispatch takes the claim as the last act inside its write frame; `wait()` returns only after that frame ends, so a finished dispatch holds it first; Force takes it only if free, else waits (bounded) for the main thread | row 6.13's one-member case over 100 runs, the second interrupt at the hook's exit: always the full response, member `Ok`; red against a claim taken after the frame | §3.4 step 6, §3.6 "The claim", row 6.13 |
| G3 | Nobody awaits the `HostClose` that Cancel's `close()` returns | Fix. The reader puts the `HostClose` back in the slot; the dispatch thread, after its frame, empties the slot and awaits it, so `Pending` reaches `gwz: cleanup still pending` | row 6.8 [3.1c]: after a gwz-cli Cancel whose helper tree outlasts the grace, stderr has the line; red against a `HostClose` nobody awaits | §3.2, §3.3, §8 row 4, row 6.8 |
| G4 | Between 2.3s and 3.1c every command would ignore Ctrl-C | Fix. 2.3s installs only for the server lifecycle commands; every other command keeps the OS default until 3.1c installs with Cancel's slot, the bound and Force | §3.1 "Install" and §5 (2.3s, 3.1c content) say so | §3.1 "Install", §5 |
| G5 | Output enforcement: clippy deny in 2.3s precedes the migration in 3.1c, and clippy misses direct handle writes (`JsonlSink`, `lib.rs:279`, `pager.rs`, `log_exec.rs`, `StderrProgressSink`) | Fix. The migration and its enforcement both land in 3.1c; enforcement is `gwz-cli/scripts/check_direct_output.py`, a `cfg`-blind token check in gwz-cli's CI (which runs no clippy) failing print macros and `stdout()`/`stderr()` (std's, `console::Term`'s) outside `src/interrupt/` and the hook files; before install and in `hook`, `Output` writes directly | row 6.2 [3.1c]: the check fails a `println!` and an `io::stdout()`, also in a `cfg(windows)` branch on Linux | §3.1 "Writer", §5 (3.1c Owns), row 6.2 |
| G6 | B revision 2 contradicts A's points: refused `Kill` spawns, no gwz-cli `close()`; 2.3r-2 and 2.3s share `check_process_globals.py` with no edge | Fix. A adopts B revision 2's §3.7 on both (Force step 4 notes the refusal; A calls no `close()`, since every `Wait` child runs in a write frame the closed gate refuses); 2.3r-2 gets "After 2.3s" | §4 points 1, 2, 7 match B revision 2 §3.7 (:224-231); §5 and §8 row 7 state the shared file and order | §3.4 "Force", §4, §5, §8 row 7 |
| G7 | O5's list of what changes is incomplete | Fix-text. O5 lists §3.1 "Giving control back", §3.4 Force step (1), §3.7, R14, row 6.13's 50 ms case and row 6.15's third SIGTERM | §9 O5 names every place that restores dispositions or tests a later interrupt | §9 O5 |
| G8 | R9 and R15 claim decisions OD-C7 (c) and (d) do not hold; §8 row 8 freezes O4's open signals | Fix-text. R9: CTRL_CLOSE decided (c), LOGOFF and SHUTDOWN with O4; R15: SIGTERM and SIGHUP decided (d), the Windows events with O4; §8 row 8 is "subject to §9 O4"; O4 names the places its answer changes | each "decided" cell matches `history/GwzCancellationAmendment-DRAFT.md:697-699` | §7 R9, R15, §8 row 8, §9 O4 |
| G9 | No constructor puts gwz-cli's `WriteGate` into the `ProcessGate` | Fix. The `WriteGate` is a `ProcessGate` field filled where composition mints it (`composition.rs`, the only minting site, so B's minting check is unchanged): `process_safety_with_children()` makes the process's gate, read by gwz-cli with `ProcessGate::write_gate()`; `process_safety()` gives an open one | §3.4 "Carriage" names the minting site, the getter and the backend field; 3.0d owns `composition.rs` | §2, §3.4 "Carriage", §5 (3.0d) |
| G10 | `merge_prepared.rs:280` (`repo.merge`, expected conflict) writes inside the transaction and is missing | Fix-text. It joins the run-to-end writes inside a ref transaction (now five); §3.5's closing sentence no longer claims a bounded set for the table's run-to-end rows | §2 and §3.5 list it (see C-P3-2's grep) | §2, §3.5 |
| G11 | A refused `park()` has no result; an empty remote's clone runs its writes parked | Fix. `park()` returns `Open \| Closed`; on `Closed` the thread stays writing and the caller fails `Cancelled` without starting the parked phase; the clone's transfer callback parks while objects remain, so a clone that never calls it stays writing (its pre-transfer I/O is failed by the host close; no host: R10) | row 6.6: a refused `download` park is `Cancelled` with no fixture contact; Force in an empty remote's clone leaves no member or a pinned one | §3.4 table, G1, "Fetch", "Clone", row 6.6, R10 |
| G12 | The process-globals check scans Rust only, so gwz-py's Python could install a handler unseen | Fix. 3.2b adds a test beside `gwz-py/src/tests/test_process_globals.py`'s that parses `gwz-py/src/gwz/` with `ast` and fails `signal.signal`, `signal.set_wakeup_fd`, `add_signal_handler` (`asyncio.run`'s own handler is Python's) | row 6.16: the test fails a planted `loop.add_signal_handler` | §1 A1, §3.8, §5 (3.2b), row 6.16 |
| G13 | §10's C-P3-1 mapping omits row 10; bare `render_exit.rs` cite; `forall.rs:203-217` for `status` | Fix-text. §10 maps C-P3-1 to §8 rows 10-12 and 14; §3.6 cites `gwz-cli/src/globalargs/render_exit.rs:82-95`; §3.7 cites `gwz-cli/src/forall.rs:260` | each cite re-read at gwz-cli `c0ba717` | §3.6, §3.7, §10 |

Fix-pass counts: Fix 9 (G1-G6, G9, G11, G12); Fix-text 4 (G7, G8, G10, G13); Dispute 0; Record 0.

## Other review notes

- **Consistency SURFACE:** `pending_local_work`'s line becomes `gwz: cleanup still pending` (§3.3); the gwz-py CLI's
  member results are no longer lost (S-P2-6); the top-level error's two entries are in §8 row 12.
- **Consistency §4 (out of scope):** D-V :1009 still names the amendment at the root path, now in `history/` (the plan's
  reference, not A's); `crates/copy-contract/src/lib.rs:36` has `#[cfg(...)]` directly on `pub mod contract_tests;`, an
  existing-code migration item under the standing conditional-compilation rule, not changed here; `pager.rs:156`'s
  `cfg!` macro is outside the rule.
- **Safety §4 (out of scope):** B's reaper kills `Wait` entries when gwz dies; A cross-references it in §4 and R14 (B's
  R-B4). `handle_materialize/apply.rs:30-31`'s rollback cannot run under an exit; under decision (i) Force never exits
  inside a member's write, and a stop at a safe point or between members is R2, R6 or R7. git2-rs's missing `upload`
  binding no longer matters to A (push unparks at `push_update_reference`).

## Unresolved (for the operator or round 2)

- **O5** (decision (i) beside OD-C7 (a)): applied as written; the operator may withdraw (a) (§9 O5 lists the six places).
- **O4** (which signals take the interrupt path, and their status): new; (a) recommended; §8 row 8 is written as (a)
  and marked subject to O4; R9's LOGOFF and SHUTDOWN and R15's Windows events wait on it.
- **B revision 2 is uncommitted and under review in parallel.** A rev 2 follows its §3.7 (points 1-2); B's §3.6 still
  names `pending_local_work` and B's §4 lacks 2.3r-2's "After 2.3s" (hand-off points 5 and 7): B's edits, not A's.
- **3.1's host API is not built yet** (Phase 1 API, plan 3.1): the `HostOwner` slot and the `HostClosing` mapping rest on
  §8 row 4's amendment to 3.1, not on existing code.
- **The serial member-loop inventory** is 3.0f's obligation (commit's loop is named; others are found by 3.0f's grep of
  `src/workspace_ops/`), not enumerated in the design.
- **(OS) facts** to confirm by rows: Windows' end after CTRL_CLOSE, LOGOFF and SHUTDOWN (row 6.15).

## Re-review

Fresh Consistency and Safety reviewers review revision 2 against D-V (:999-1062), this plan and design B's current
revision. Exit: GO on both axes with no new P0-P2. Open for round 2: O4 and O5, the 3.1 dependency, whether B
revision 2 states hand-off points 1-7 as A does, and whether the fix pass closed G1-G13.
