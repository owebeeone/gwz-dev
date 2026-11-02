# GwzCancelInterruptDesign — remediation plan, round 2

Date: 2026-11-03. Status: **remediation plan for the lane owner; it authorizes nothing beyond revision 3 of design A.**
Drafted in the lane `/Volumes/projects/limbo/gwz-dev-docs-cancel`, writing only this file and the design; no git operation
was run (the lane owner commits).

- **Object:** `dev-docs/GwzCancelInterruptDesign-DRAFT.md`, revision 2, blob `bfe7d179`, committed at root `50fc35e8`
  (the blob both round-2 reviews read).
- **Reviews:** [Consistency](GwzCancelInterruptDesign-ReviewConsistency-2.md), NO-GO, 2 P2, 11 P3;
  [Safety](GwzCancelInterruptDesign-ReviewSafety-2.md), NO-GO, 5 P2, 10 P3. No P0 or P1; no new architectural root
  cause (count stays 2; Safety P2-1 is the last edge of round 1's root cause (2)). IDs keep each review's numbering with
  a prefix: `C-` Consistency, `S-` Safety.
- **Operator (plan §10 D-V, "Designs A and B, round 2", :1063-1069):** O4 (a) and O5 (OD-C7 (a) kept; decision (i)
  governs gwz's own exit, not the user's further interrupt). Revision 3 applies both (§9 lists every place) and closes
  §9: nothing is open.
- **Result:** revision 3, edited in place: 397 lines (cap 400); status "revision 3 (2026-11-03) … for round-3 review";
  §10 "Changes from revision 2" points here: this table's "Rev 3 §" column is the one map of each C-/S- ID's sections
  (after the fix pass below). §6's rows became one table (as §5, §7, §8 are) to stay under the cap; no row was dropped;
  §8 rows 2-3 and 15-16 were merged (as 9-12 already was) for the same reason, their text unchanged.
- **Last round under the cap: no new mechanism.** Each fix narrows or re-routes something revision 2 already has: the
  write gate's `unpark` stops instead of resuming (G4); the existing gate reaches the ordinary build by a setter, not a
  new carrier; the existing slot gains an `Interrupted` state; the existing foreground scope starts earlier; the existing
  cancel record gets removal rules; nested frames are refused less. No thread, process, handler, hook, file format or
  protocol is added. Two reviewer suggestions that would have added one were replaced: a panic hook routed through
  `Output` (S-P3-6) is a named residual (R8), and an operator question for the gwz-py CLI's later SIGINTs (S-P2-4) is a
  "beyond" row D puts to the operator.

## Verification

Every file:line revision 3 adds was re-read at gwz-core `b745f1fa`, gwz-cli `c0ba717`, gwz-py `2185cc6`, gwz-transport
`281a28d`, libgit2 `b172e3d18`, CPython 3.10.15 and 3.11.16 (`~/.asdf/installs/python/`), documents at root `6d20ef6f`
(identical to `48feeaad` for every cited document but the plan, whose change is only the appended bullet :1063-1069).
Facts that set a disposition:

- `transport_host` is candidate-only (`src/lib.rs:47-51`); the ordinary dispatch uses `Git2Backend::new()`
  (`gwz-cli/src/globalargs/dispatch.rs:91-96`); `Git2Backend` has `with_route_notes(&self, …) -> Self` (`backend.rs:75`)
  to copy for `with_write_gate`, and its `spawn_gate` field at :26 (C-P2-1).
- After the join, materialize removes fresh clones (`handle_materialize/apply.rs:178-183`); init-from-sources removes them
  (`handle_init_from_sources.rs:227-231`) or writes the manifest and lock (:236-238); libgit2's clone sets HEAD before
  checkout (`clone.c:394-407`) and removes on error (`:666-673`); push disconnects at `remote.c:3070` (S-P2-1, S-P3-1,
  S-P3-5).
- `RECORDED_CANCELLATIONS` (64) is the completed-cancellation record (`client_host/operations.rs:28`); `register` is at
  :156; `_run_native` runs on `asyncio.to_thread` (`bridge.py:307`), whose executor threads are joined at exit
  (`concurrent/futures/thread.py:31`, registered :37); 3.11 restores `default_int_handler` (`runners.py:129`) and
  cancels all tasks in `close()` (:71) (S-P2-3, S-P2-4).
- The pager is fed by blocking `write_all` (`gwz-cli/src/pager.rs:123-128`) from its spawn (:107-115) to `finish`'s
  wait (:138-145) (S-P2-5).
- The pending link is the HTTPS adapter's disposal (`src/git/endpoint/https_adapter/worker.rs:268`, waiting on
  `pending_cleanup_count()` at :340); `gwz_local_import`'s poll is `crates/local-import/src/lib.rs:508-509`
  (:337-339 is the trait); the plan's split names a "cancellable entry" at :1050 (C-P3-9).
- The process-globals check scans `crates/*/src/lib.rs` (gwz-core `scripts/checks/process_globals_allowlist.json`
  "roots"), so B's 2.3r-1 brings its own `signal` occurrences (C-P3-2); gwz-core's root `BUILD.bazel` lists each crate's
  label (:19-24) (C-P3-2).
- Fix pass: `ensure_clone_target_is_empty` runs first in `clone` (`src/git/gitbackend/transport.rs:32`); init-from-sources
  writes the manifest and lock at `handle_init_from_sources.rs:236`; gwz-core's `scripts/checks/process_globals_allowlist.json`
  has no `signal` or `sigaction` entry; `_gwz_core`'s module functions are registered in `gwz-py/native/src/lib.rs:169-185`,
  while `ClientHost` (`#[pyclass]`, `client_host.rs:71`) has `cancel_operation` at :164 and every bridge call goes through
  `self._host` (`bridge.py:237, :329`); `shims::operation_id` is `pub(crate)` (`shims.rs:79-81`); libgit2's `git_remote_fetch`
  disconnects (`remote.c:1394`) before `update_tips` (:1409). B revision 3 is being edited in parallel (its line numbers moved during this pass), so A cites it by
  section. As last read it: names A revision 2 by line ("A") and revision 3 by section ("A r3"); maps its cancel and
  pending cleanup to "A r3's host close, A r3 §3.2" and "the host close's `Pending` (A r3 §3.3)" (§3.6); merges 2.3r-1 and
  2.3r-2 after 2.3s ("merged after A's 2.3s", §4, §8 row 1); and still sets the totals to "across 14 steps" (§8 row 4).
- The candidate build's `windows-sys` features include `Win32_UI_WindowsAndMessaging` (`gwz-core/Cargo.toml:183, :203`)
  (S-P3-4; the LOGOFF/SHUTDOWN behaviour itself is **(OS)**, unverified here).
- The 2.3 plan totals (`GwzServerCore-Step2.3-Plan.md:249-250`) are 4,300 + 4,750 across 12 steps; B §8 row 4 makes them
  5,250 + 5,710 across 14 at B revision 2 and 5,280 + 5,820 at B revision 3 (2.3r-1 ~600 + 650, 2.3r-2
  ~380 + 420); with 2.3s (~300 + 300): 5,580 + 6,120 across 15 (C-P3-2). A §8 row 7 states the rule (B's figures plus
  2.3s) and the current figure, so B's final sizes need no A edit.

## Blind convergences

1. C-P2-1 and S-P3-9: the ordinary (non-candidate) build lacks what A assumes: the write gate's carriage (C) and B's
   registry for `git commit` (S). One answer: the gate reaches both builds; the registry gap is B's R-B13 (A's R17).
2. C-P3-5 and Safety's C-P3-5 closure note: "the reader and writer are gwz's first threads" is false after B's registry.
3. C-P3-6, S-P3-7 and S-P3-8: O5's consequences were not carried into every clause; with O5 answered, A3, R8, R14 and
   the Force notice now say what a further interrupt does.

## Finding disposition

Dispositions: **Fix** (design change), **Fix-text** (text, cite, cost, residual or ordering only), **Dispute** (with
evidence), **Record** (named residual or decision, no mechanism).

Counts: Fix 7 (C-P2-1, C-P2-2, S-P2-1, S-P2-2, S-P2-3, S-P2-5, S-P3-10); Fix-text 19 (C-P3-1 to C-P3-11, S-P3-1,
S-P3-2, S-P3-3, S-P3-5 to S-P3-9); Record 2 (S-P2-4, S-P3-4); Dispute 0.

| ID | Finding | Disposition | Closure test | Rev 3 § |
|---|---|---|---|---|
| C-P2-1 | The write gate exists only in candidate builds, so Force exits mid-write in the ordinary build | Fix (the reviewer's (a)). `run` makes the process's `WriteGate`, a value it owns and passes explicitly; 3.0d adds an unconditional `Git2Backend::with_write_gate`, which the ordinary dispatch backend takes, and, candidate-only, a `ProcessGate` field and copying setter (minting unchanged) that `with_process_gate` carries; A9 states both builds | §3.4 "Carriage, both builds" names the ordinary route with its cite (`dispatch.rs:91-96`, `backend.rs:26`); row 6.9 runs in both builds | A9, §2, §3.4, §5 (3.0d, 3.1c), row 6.9 |
| C-P2-2 | After 3.1c the reader's bound, Force and `_exit` also apply to server lifecycle commands | Fix (one scoping clause). Server lifecycle mode: the reader only delivers Cancel and Force to `Interrupts` and restores the dispositions at Force (OD-C7 (a)); no slot, bound, gate close, claim or forced exit; shutdown is C's, and so is delivering the status, which O4 (a) sets to 130 (fix pass, GAP-1) | §3.1, §3.6 and §8 row 8 agree; §8 row 6's 2.3k red row: a second SIGINT during the detached wait still removes the socket and clears the record; row 6.1 shows the mode. Fix pass (GAP-1): O4 (a) is not narrowed: the mode's exit status after an interrupt from any O4 (a) source is 130, which C delivers; §4 C and §8 row 6's 2.3k red rows (SIGINT, SIGTERM, SIGHUP; W: Ctrl-C, Ctrl-Break, console close; exit 130) and §8 row 8 say so | §3.1, §3.6, §4 C, §8 rows 6, 8, §9, row 6.1 |
| C-P3-1 | 2.3s "installs only for a server command", none exists until 2.3k | Fix-text. 2.3s provides `install`, `Interrupts` and a re-exec harness and calls neither from `run`; 2.3k calls `install` in its run-own-lifecycle branch; 2.3s edits `lib.rs` only for the `mod` line | §5 and §8 row 6 name 2.3k as the step that installs for server commands, after the command exists | §3.1 "Install", §5, §6 preamble, §8 row 6 |
| C-P3-2 | B's signal calls land in 2.3r-1; ordering, allowlist, totals and 3.0d's Owns incomplete | Fix-text. 2.3s's gwz-core half (the kind, its test, and re-kinding any `signal`/`sigaction` entry already in gwz-core's allowlist, B's from 2.3r-1 if it merged first) lands before 2.3s's gwz-cli half; a step merged later adds such entries as `signal`, so 2.3s and 2.3r-1 merge in either order; 2.3r-2 stays after 2.3s; one absolute totals sentence, B §8 row 4's figures plus 300 + 300 and one step; 3.0d owns `Cargo.lock` and the root `BUILD.bazel` label | whichever wave-2 step merges first, both checks stay green; one totals sentence whatever the order. Fix pass (GAP-4): A takes B revision 3's fixed order (2.3r-1 and 2.3r-2 after 2.3s, B §4, §8 row 1), so 2.3s re-kinds nothing (gwz-core's allowlist has no `signal` entry) and leaves gwz-core's allowlist out of its Owns; A §8 row 7's totals sentence supersedes B §8 row 4's totals clause, so either application order leaves it | header, §3.1 "Check", §4 (6), (8), §5, §8 row 7 |
| C-P3-3 | The graph lacks `3.0f → 3.1c` | Fix-text. Edge added; 3.0f leaves the `→ 3.3` set (transitive) | every [3.1c] row's prerequisites are in its "After" | §5 (3.1c), §8 row 1 |
| C-P3-4 | `check_direct_output.py` misses 2.3k's files and test sources | Fix-text. 3.1c owns every file the check flags on its base (2.3k's included); test sources (`src/tests/`, `tests.rs`, `*_tests.rs`) are excluded | the check passes on 3.1c's merge base with only 3.1c's Owns edited; row 6.2 passes `src/tests/` | §3.1 "Writer", §5, row 6.2 |
| C-P3-5 | "gwz's first threads" conflicts with B's registry | Fix-text. "The first threads after the registry's"; for server commands, if C gives them a registry, C creates it before `install` | A and B name one order in `run` per command class | §3.1 "Install", §4 (3), §4 C |
| C-P3-6 | O4, O5 and dependent residuals inconsistent | Fix-text. Both are answered: O4 (a) and O5 are applied and §9 lists every clause each governs; A3 and R8 say "only SIGKILL or a further interrupt after Force began"; R15's Windows cell is "beyond (OD-C1)", no option referring to it | no cell refers to an open option; §9 lists every clause. Fix pass (GAP-5): A3 and R8 add Windows ending the process after CTRL_CLOSE, LOGOFF or SHUTDOWN (R9) as the third way a write in progress ends | A3, §3.1, §3.6, §3.7, R8, R9, R14, R15, §9 |
| C-P3-7 | §7's "Beyond" definition does not match its use | Fix-text. The column becomes "Covered by", naming the D-V clause whose mechanism leaves each residue ((1) safe points; (i) "at once"; (i) hung write; OD-C7; O4; O5; OD-B2), else "beyond"; R6 says a partial directory blocks re-materialize | each non-beyond cell names its clause | §7 |
| C-P3-8 | Some rows cannot be met on Windows | Fix-text. Unix legs marked M L; W analogs: CTRL_BREAK for SIGTERM legs, a `CREATE_NEW_PROCESS_GROUP` child for "stays disabled" | every leg is satisfiable on the platforms it names | rows 6.1, 6.11, 6.12, 6.15 |
| C-P3-9 | Citation precision and provenance | Fix-text. `https_adapter/worker.rs:268, :340`; `local-import/src/lib.rs:508-509`; `operations.rs:28`; §0 says D-V's "cancellable entry" (:1050) is the 3.1 `HostOwner` in a slot | each cite re-read at its pin (above) | §0, §2, §3.3, §3.8 |
| C-P3-10 | `gwz_response` not frozen; §8 row 8 over-claims | Fix-text. §8 rows 15 and 16 name `gwz_response`; row 8 adds "before the response was claimed" and excludes `hook` and `claude-code-setup` (the `server` exclusion was dropped in the fix pass, GAP-1: a server lifecycle command stopped by an interrupt also exits 130) | rows 8, 15, 16 match §3.6 and §3.8 | §8 rows 8, 15, 16 |
| C-P3-11 | `diff`/`log` take no claim; senders blocked when Force comes without Cancel | Fix-text. `diff` and `log` take the claim at their first stdout write; a sender waits "until Cancel or Force" | §3.6 states the rule for own-lifecycle commands; row 6.3 says "before Cancel or Force" | §3.1 "Writer", §3.6, row 6.3 |
| S-P2-1 | After `wait()` a parked thread can resume into a write (rollback, manifest, libgit2 cleanup) | Fix. Invariant G4: after `close()` an `unpark` stops the thread, as `outside` does; the fetch, clone and push callbacks and the join leave R4, R6, R5, R7 instead of running an abort path. Record: libgit2's removal of a clone whose transfer failed runs parked (no callback) and may be cut by step 8, named in A3, G1 and R6 as "beyond" (a partly removed new directory) | row 6.4: a parked thread whose `unpark` follows `wait()` blocks, nothing written; row 6.6: two interrupts in a four-member `materialize` with a seam delaying step 8: `gwz.toml`/`gwz.lock` both old or both new, each member absent, complete or R6's; red against "unpark writing again"; M L W. Fix pass (GAP-2): R7 names the join's residue: at a `materialize` or init-from-sources join G4 stopped, the complete fresh clones the F2 rollback would remove stay, init writes no `gwz.toml`/`gwz.lock`, and a re-run's clone refuses such a non-empty target (`transport.rs:32`); row 6.6 adds a four-source `init` | A3, §2, §3.4 (table, G1, G4, Fetch, Clone, Push), R6, R7, rows 6.4, 6.6 |
| S-P2-2 | An interrupt before the host and frame exist cancels nothing | Fix. The slot gains `Interrupted`: Cancel (or Force) on an empty slot sets it; the dispatch checks before building the host and before its frame and refuses `Cancelled` (130, nothing written); an owner put into it is closed at once; §8 row 4 carries it into plan 3.1 | row 6.11: a seam holds the dispatch before the slot's fill, one interrupt: `--json push` of two members never contacts the fixture, both `Cancelled`, 130; `switch` changes no file; M L W (CTRL_C) | §3.2, §8 row 4, row 6.11 |
| S-P2-3 | The pre-registration record is a 64-entry FIFO with no removal | Fix. The record holds one entry per cancelled call still awaited: consumed by its registration (refused `Cancelled`), else removed by a native `forget_cancel(id)` when `_await_completion` returns; no capacity (the 64 is another record's). The reviewer's "network methods only" rule is then unnecessary: a local call's entry is forgotten at its return | row 6.16: `max_workers=1`, one blocking call, 200 queued fetches cancelled: none contacts the fixture; a cancelled local call, then a fetch with its `request_id`: the fetch runs; 3.10, 3.11+; M L W. Fix pass (GAP-7): `operation_id`, `cancel_call` and `forget_cancel` are `ClientHost` methods (`client_host.rs`, in 3.2b's Owns), no module function, so `native/src/lib.rs` and `shims.rs` stay unedited | §3.8, §5 (3.2b), §8 row 15, row 6.16 |
| S-P2-4 | The gwz-py CLI's later SIGINTs end the interpreter mid-native-write; R11 said "keeps waiting" | Record. R11 states it: the 3rd SIGINT (3.10) or 4th (3.11) ends the interpreter with a native write running (torn checkout, `index.lock`, a ref `.lock`), recognized by D's `*.lock` report, "beyond", put to the operator by D beside OD-C7 (d); R11 notes that OD-C7 (d)'s `cli.py`-only alternative would need OD-C1's exception and 3.2b's test to allow `cli.py` | R11 names the case and the count per version; row 6.16 records the count (not asserted safe) | §3.8, R11, row 6.16 |
| S-P2-5 | OD-C6's wait is not delivered while gwz still feeds the pager | Fix. `Interrupts::foreground` spans the pager from its spawn to its exit, so no bound runs and Force's later steps wait for the pager; on Cancel the feed stops at its next chunk (a blocked feed returns when the pager reads or quits, `EPIPE`) | row 6.15: `gwz diff` of 1 MiB into `less` (W: `more`), two interrupts at page 1: gwz lives until the pager exits, then 130; red against a scope opened after feeding; M L W | §3.7, row 6.15 |
| S-P3-1 | The clone checkout's stop residue is misdescribed | Fix-text. §3.5 row for the clone's checkout and R6: new HEAD, no index, some files; a later `stage` would record deletions; the notice names `clone` | §3.5 and R6 agree with `clone.c:394-407` | §2, §3.5, R6 |
| S-P3-2 | The slot's take, close and put are not atomic with the dispatch's empty | Fix-text. One mutex-guarded slot held across take, `close()` (non-blocking) and put | §3.2 states it | §3.2 |
| S-P3-3 | The claim's loser is unspecified | Fix-text. A main thread that fails to take the claim blocks until Force exits | row 6.13: 100 runs with the main thread ending its frame after Force's claim: always 130 | §3.6, row 6.13 |
| S-P3-4 | Windows LOGOFF and SHUTDOWN may never reach the handler | Record. §3.1 and R9 state that a process that loaded `user32.dll` (the candidate's Pageant) gets no LOGOFF or SHUTDOWN call **(OS)**, the OS then ending gwz (SIGKILL's residues); not row-tested (no logoff in CI), marked **(OS)** | §3.1 and R9 name it with its provenance | §3.1 "Windows", R9 |
| S-P3-5 | G1 and R10 omit network I/O in the writing state after an unpark | Fix-text. G1's list adds a fetch's pack trailer and a push's disconnect (`remote.c:3070`); R10 adds the push's disconnect | G1 and R10 list every writing-state network wait | §3.4 (G1), R10 |
| S-P3-6 | Panic output, `dbg!`, unwind and reader death outside the contract | Fix-text. `dbg!` joins the check's tokens; every frame and park is a guard restored on unwind; the reader runs under `catch_unwind` and restores the dispositions on a panic. Record instead of a panic hook (a new process-global mechanism): a panic message on a stalled stderr blocks its thread, R8 | row 6.2 (`dbg!`), row 6.4 (a frame unwound by a panic), row 6.1 (a reader panic restores the dispositions); R8 names the panic case | §3.1, §3.4, R8, rows 6.1, 6.2, 6.4 |
| S-P3-7 | An orphaned foreground program after R14 is not named | Fix-text. R14 adds the orphaned pager or `forall` command, recognized by the program still on the terminal | R14's columns name it; row 6.15's third interrupt cites R14 | R14, row 6.15 |
| S-P3-8 | The Cancel notice invites the interrupt that tears a write | Fix-text. The Force notice adds "(a further interrupt ends gwz at once and may leave a partial write)"; §8 row 8's paragraph includes the notices | §3.6 and §8 row 8 carry the text | §3.6, §8 row 8 |
| S-P3-9 | A4 is unqualified for the non-candidate build | Fix-text. A4 covers registered commits; a registry-less one gets the terminal's interrupt as on 1.0.18 (B's R-B13), R17 points there | A4 and R17 cite R-B13 | A4, R17 |
| S-P3-10 | A refused nested frame is an unlisted stop point | Fix (narrowing). Only a thread's outermost frame is refused on a closed gate; a nested frame never is, so a running sequence (a stage, then its commit) runs through | row 6.4: after `close()` a nested frame is not refused; §3.5 says so. Fix pass (GAP-3, the reviewer's second option for parks and `outside`): G3 says a refused park or `outside` fails its sequence `Cancelled` there, keeping what it wrote before; §3.5's last row lists the refusal points inside a running sequence (a fetch's `disconnect` after its pack commit, a clone's first transfer park, a join's park) and their residue; R18 names the fetch's (new pack and `.git/shallow`, refs and `FETCH_HEAD` old); row 6.6 adds Force before `disconnect` | §3.4 table, G3, §3.5, R18, rows 6.4, 6.6 |

## Hand-off points with design B (revised in parallel)

Revision 3 relies on B revision 3 as on disk (A §4 points 1-8). Points marked **B edit** need B's text to say the same;
none changes B's mechanism, and A's text does not depend on the order in which the lane owner applies the two designs.

1. `kill_killable()` ends every live `Kill` entry at once and refuses later `Kill` spawns `Closed` (B §3.7, row B22),
   reached as `ProcessGate::kill_killable()`, a no-op without a registry (B §3.1, 2.3r-2). Matches.
2. gwz-cli calls no `close()` (B §3.7). Matches; B cites A revision 2 by line (":179-180", ":183-184"), which A §4 (2)
   maps to revision 3's §3.4 "Force" step 4 and its "A calls no `close()`" sentence.
3. `run` creates the registry before any thread, so before A's install (B §4 3.1; OD-B3 (a)). A's reader and writer are
   the first threads after the registry's. Matches.
4. `process_safety_with_children()` mints the `ProcessGate`; A's 3.0d adds a `write_gate` field and a copying setter in
   `composition.rs` (no new mint, B's minting check unchanged); `with_process_gate` copies it into the backend. Matches.
5. `terminate` returns the lookup's own outcome and retains the tree (B §3.6), reported as the host close's `Pending`
   (A §3.3). Matches: B's current §3.6 says "A r3's host close" and "the host close's `Pending` (A r3 §3.3)" (its earlier
   "A's Cancel token" and "A's `pending_local_work`" are gone).
6. B changes no disposition in gwz's process. **A follows B** (fix pass, GAP-4): 2.3r-1 and 2.3r-2 merge after 2.3s (B
   §4, §8 row 1), adding B's entries under the `signal` kind 2.3s makes; A no longer claims "either order" and 2.3s
   re-kinds nothing. A's PID check covers B's registered child between `fork` and `exec` (B Safety-2 N-P3-1's first leg).
7. R-B4 covers a `git commit` torn by R14, R9 or SIGKILL (B §7 R-B4); R-B13 the ordinary build (A's R17). Matches.
8. **B edit:** A §8 row 7's totals sentence ("about 5,580 implementation lines and 6,120 rows across 15 steps" at B
   revision 3's figures) supersedes B §8 row 4's totals clause ("5,280 … 5,820 … across 14 steps"), so applying
   either row first leaves A's sentence; B's row 4 should defer its totals to A's row 7 (or state the same figure).

## Fix pass (2026-11-03, before round-3 review)

A completeness check after revision 3 found seven gaps; each is closed by text, a named residual or a hand-off, no
mechanism. Rows above carry each fix in their closure test and "Rev 3 §".

| Gap | Where | Disposition and change | Rows touched |
|---|---|---|---|
| GAP-1 | server lifecycle status vs O4 (a) | Fix-text: O4 (a) is not narrowed; the server lifecycle commands exit 130 after any O4 (a) interrupt, C delivering it; §3.1, §3.6, §4 C (full source list), §8 row 6's 2.3k red rows (SIGINT, SIGTERM, SIGHUP; W: Ctrl-C, Ctrl-Break, console close; exit 130), §8 row 8, §9 | C-P2-2 |
| GAP-2 | G4's join residue | Record: R7 names the complete fresh clones kept at a stopped `materialize` or init-from-sources join, init's missing `gwz.toml`/`gwz.lock` and the re-run's refusal (`transport.rs:32`); row 6.6 adds a four-source `init` | S-P2-1 |
| GAP-3 | parks and `outside` refused mid-sequence | Fix-text + Record (the reviewer's "list the refusal points"): G3; §3.5's last row; R18 (fetch: new pack and `.git/shallow`, refs and `FETCH_HEAD` old); row 6.6 | S-P3-10 |
| GAP-4 | contradictions with B revision 3 | Fix-text: header names B revision 3; §3.1 "Check", §5 and §8 row 7 take B's fixed order; §4 cites B by section, maps B's revision-2 line cites; §8 row 7 supersedes B's totals clause; B edit (point 8) listed above | C-P3-2 |
| GAP-5 | A3/R8 omit Windows' end (R9) | Fix-text: A3 and R8 add R9 | C-P3-6 |
| GAP-6 | §10 vs this table | Fix-text: §10 now points to this table's "Rev 3 §" column as the one map instead of repeating it | all |
| GAP-7 | where the native calls live | Fix-text: three `ClientHost` methods (3.2b owns `client_host.rs`), no module function; `lib.rs`, `shims.rs` unedited | S-P2-3 |

## Unresolved

- **B edit** (point 8 above): B revision 3 as last read still sets "across 14 steps". A's text is correct either way: §8
  row 7 supersedes B's totals clause whichever is applied first. Points 2, 5 and 6 now match B's current text.
- **C** must accept the server lifecycle mode (§3.1): shutdown, stop input, any registry before `install`, and exit 130
  after an interrupt from any O4 (a) source (§8 rows 5-6 hand them to 2.3i and 2.3k).
- **(OS) facts** unverified here: Windows ends a process a short time after CTRL_CLOSE (row 6.15); a process that loaded
  `user32.dll` gets no LOGOFF or SHUTDOWN call (S-P3-4, no row).
- **3.1's host API is not built yet**: the slot and the `HostClosing` mapping rest on §8 row 4's amendment to plan 3.1.
- **"Beyond" rows for D to put to the operator:** R6's cut removal (a narrowing of decision (i), S-P2-1), R7 (now with
  the join residue), R10-R13, R15's Windows events (R11 includes S-P2-4's case).
- **Line budget:** revision 3 is at 397 of 400 lines; a round-3 fix that adds text must remove as much.

## Re-review

Fresh Consistency and Safety reviewers review revision 3 against D-V (:999-1069), this plan and design B's current
revision. Exit: GO on both axes with no new P0-P2. This is the last round under the cap: a finding that needs a new
mechanism stops the design for the operator.

## Closure pass (round 3 pre-commitments)

Date: 2026-11-03. Object: revision 3 (blob `3760dd61`, the blob both round-3 reviews read) edited in place to
**revision 3.1** (blob `8facabd8`), 399 lines (cap 400). Reviews: `-ReviewConsistency-3.md` (NO-GO, 2 P2, 8 P3; GO
pre-committed on P2-1 and P2-2 as corrected) and `-ReviewSafety-3.md` (NO-GO, 1 P2, 5 P3; GO pre-committed on P2-1 by (a)
or (b) with its closure test). IDs: `C3-` Consistency round 3, `S3-` Safety round 3. No new mechanism: every change is a
scoping clause, a return value or end action of something revision 3 already has (`install`'s lifecycle mode, `Output`,
the gate's labels, the claim, the reader's byte), a named residual or text. Design §10 was folded into the status line,
which now points here and to "Rev 3 §" above. Every file:line the pass touched was re-read at the §"Verification" pins;
new ones: CPython 3.11.16 `test/test_signal.py:164-176` (POSIX: unhandled `KeyboardInterrupt` ends by SIGINT) and
:214-226 (Windows: `STATUS_CONTROL_C_EXIT`); libgit2 `streams/socket.c:155, :282-296, :318-330` (`poll` failure is an
error, no `EINTR` retry); gwz-core `transport_support.rs:21-28, :62-69` (default 9 s libgit2 timeouts);
`check_local_clone_boundaries.py:17`, `check_crate_versions.py:85, :198`, `check_bazel_pin_drift.py:181`; root
`MODULE.bazel:15-17, :105-129`, `gwz-cli/BUILD.bazel:34-40`; 2.3 plan :71, :207-208, :212-213, :220-230; gwz-cli
`pager.rs:11-15`, `diff_exec.rs:271`, `log_exec.rs:204, :286`, `progress_detail.rs:199`; B r3 §8 row 4 (defers to A).
External facts marked **(ext)**: bash's wait-and-cooperative-exit; the kernel's choice of the main thread; Windows shells
acting on their own console event.

| ID | Finding | Disposition | Change (rev 3.1 §) | Closure test |
|---|---|---|---|---|
| C3-P2-1 | `server stop`, `status`, `list` had no handler, yet §3.6 and §8 row 8 froze 130 | Fix-text, correction (a) | 2.3k installs lifecycle mode for every `server` subcommand (`start`, background copy, `stop`, `status`, `list`, `stdio`; 2.3 plan :212-213); C ends each with §3.6's 130 after any O4 (a) source. §0, §3.1 "Install" and "Server lifecycle mode", §3.6 "Status", §4 C, R12 "When", §8 row 6 (red row: `server stop` interrupted in its detached wait by each source, 130) and row 8 ("a `server` subcommand") | one owner (C, through lifecycle mode) and one exit (130 by §3.6's end) per subcommand in §3.1, §3.6, §4 C, §7 R12, §8 rows 6, 8; §8 row 6's 2.3k red rows cover `stop` under each O4 (a) source |
| C3-P2-2 | `Output` swallowed EPIPE, removing `diff`/`log`'s stop on a closed consumer | Fix-text as corrected | §3.1 "Writer": after a stdout write error `Output`'s stdout send returns `ConsumerGone` at once, never blocking; `diff` and `log` take their `BrokenPipe` stop (`pager.rs:11-15`, `diff_exec.rs:271`, `log_exec.rs:204, :286`); other errors ignored, so stderr's notices are unaffected | row 6.3 [3.1c]: `gwz log \| head -1` (50,000 commits) and `gwz diff --no-pager \| head -1` (10,000 files) exit 0 within 2 s, `ConsumerGone` observed; red against "write errors ignored" |
| S3-P2-1 | `exit(130)` after Ctrl-C lets a calling bash script or loop continue | Fix, correction (a) | §3.6 "Status": 130 "as a calling shell reports it"; **the end**: after a first SIGINT on Unix, `raise(SIGINT)` under the default disposition (a SIGINT death, $? 130, bash ending the script as on 1.0.18); SIGTERM and SIGHUP `exit(130)` (wait-and-cooperative-exit concerns SIGINT only); Windows `exit(130)` for every console event, as O4 (a) says (no signal death exists; cmd.exe and PowerShell get the console event themselves (ext), recorded by row 6.11). §3.1 Unix (the byte names its signal), §3.4 Force steps 6 and 8, §3.8 (`main` raises `KeyboardInterrupt` on Unix after rendering, returns 130 on Windows), R14's recognizer, §8 row 8 ("130 as a shell reports it … a death by SIGINT"), §6 preamble, §9. **O4 (a) consistency:** O4 (a) and OD-C4 say 130 as "128 + SIGINT, the shell convention"; a SIGINT death is what a shell reports as 130, so every source still ends 130 to a shell; a `waitpid` caller sees `WIFSIGNALED` (Python `returncode` -2). §9 asks the lane owner to confirm this reading; a Windows script that goes on after CTRL_C is listed for D as an O4 (a) narrowing | row 6.11 (M L): `bash -c 'gwz --json fetch; touch after'` and a two-pass loop, SIGINT to the group: `after` absent, second pass not run, status 130 by SIGINT; red against `exit(130)` after SIGINT; W leg records `cmd /c` and PowerShell. Row 6.16 (M L): the same for `gwz-py`; W exits 130, not `STATUS_CONTROL_C_EXIT`; red against `return 130` on Unix |
| C3-P3-1 | A3's "only … ends a write" list contradicted §7 | Fix-text, the reviewer's second option | A3: only SIGKILL, a crash, power loss (resolution (1)) and §7's R9, R11, R12, R14 (a further interrupt, SIGQUIT), R15 end such a write | A3 and §7 name the same set |
| C3-P3-2 | Windows end states and O4 (a)'s narrowing labelled "covered" | Fix-text | §8 row 8 adds "when gwz exits by itself"; R9 "Covered by": LOGOFF and SHUTDOWN under `user32` beyond **(OS)**, a narrowing of O4 (a); §9 drops "none narrowed" and lists R15's Windows events and R9's LOGOFF/SHUTDOWN as O4 (a) narrowings D puts to the operator | row 8, R9 and §9 agree |
| C3-P3-3 | Owns lists missed gate-enforced build files | Fix-text | §5 3.0d adds `scripts/checks/local_clone_inventory.json` (LBT-001, `check_local_clone_boundaries.py:17`) and `check_crate_versions.py` (`EXPECTED_CRATES` :85, after B's 2.3r-1 bump; its publish order :198, which the release workflow walks); 2.3s adds `gwz-cli/BUILD.bazel` deps (:34-40), root `Cargo.lock` and `MODULE.bazel` (`@crates` specs :15-17, :105-129), notes `check_bazel_pin_drift.py:181` reads only `[dependencies]` (the Bazel build checks the `cfg` ones), and lands "gwz-core check first, then gwz-cli with the root, one root commit"; §8 row 7 carries the order | both Owns name every gate input their crate or dependency touches |
| C3-P3-4 | Main thread's writer drain unbounded; row 6.12 contradicted it | Fix-text, the first option | §3.6 "The claim": the holder drains `Output` before its end, unbounded with no interrupt (as today), else at most 1 s; §3.4 Force step 6 now defers to §3.6's bounded wait (close bound + 1 s) | row 6.12's "within the bound plus 1 s" holds: Cancel's close resolves within the close bound, then ≤ 1 s |
| C3-P3-5 | Row 6.9 [3.0f] asserted 3.1c's notice | Fix-text | row 6.9: `wait()` returns the label naming member and phase; the notice leg moved to row 6.12 (3.1c) | each row's legs are visible red then green in its own step |
| C3-P3-6 | Lifecycle-mode output and `server stdio`'s channel unspecified | Fix-text | §3.1 "Server lifecycle mode": no writer and no notice; C owns output, `server stdio`'s stream too; "Writer": the direct-output check exempts 2.3k's `src/server_{exec,after,long}.rs` (2.3 plan :208); §4 C lists it | lifecycle mode states writer, notices and channel; the check does not flag 2.3k's lifecycle files |
| C3-P3-7 | R8's panic on a stalled stderr classed "(i)" | Fix-text | R8 "Covered by": "the panic case beyond"; §9's "Beyond" list adds R8's panic | R8 and §9 agree |
| C3-P3-8 | Text drift | Fix-text | §4 (8): B's row 4 already defers ("as B's row 4 says"), so RemPlan-2 "Unresolved" B edit is closed (the lane owner need not edit B for it); path convention: §3.1 "Writer" names "gwz-cli's `src/…`", §5 3.1c's Owns opens "in `gwz-cli/`:"; §8 row 7 adds ":71" ("about 4,300 implementation lines" takes the same absolute figure) and the anchor "after 2.3l (:220-230)" | each named spot reads as corrected |
| S3-P3-1 | Unmasked signals fail the main thread's libgit2 HTTPS wait with `EINTR` in no-host builds | Record, the reviewer's second option (no mask: that would add a call to `run`) | R10 states it: gwz sets libgit2's timeouts (`transport_support.rs:21-28, :62-69`), so the socket waits in `poll`, which `SA_RESTART` does not restart (`socket.c:155, :282-296, :318-330`); the member reports that network error, not `Cancelled`; recognized by the leading `Cancelled` entry | row 6.11 (M L, ordinary build): SIGINT during `pull`'s HTTPS root fetch: the member `Cancelled` or R10's `EINTR` error, the `Cancelled` entry first |
| S3-P3-2 | A G4 or `outside` stop recorded no label, so a received push got no line | Fix-text | §3.4 table: `unpark` on a closed gate and `outside` after `f` record the frame's label and phase before stopping; §3.6 Notices: a push after the report adds `: the remote received it; tracking ref not updated`; R5's recognizer names it | row 6.12 (M L W): Force during a network push's `push_update_reference`: stderr names the member and says the remote received it, tracking ref old; red against a G4 stop with no label |
| S3-P3-3 | Async `Output` unordered against inherited descriptors; final drain unstated | Fix-text | §3.1 "Writer": `Output::drain()` precedes a spawn inheriting stdout or stderr (pager, `forall` member) and the claim holder's end; §3.6 "The claim" states the drain (unbounded with no interrupt; Force's 1 s unchanged) | row 6.3 [3.1c]: 1,000 `forall` members: every banner precedes its member's output; a slowly read `--json` document is complete |
| S3-P3-4 | In-pager Ctrl-C makes R14's orphan routine | Record (no change required for GO) | R14 "When": routine in `log`/`diff` as a third Ctrl-C inside the pager; "Covered by" and §9: whether O5's restore should wait while only a pager runs is D's to put to the operator (owner: D) | R14 names the case |
| S3-P3-5 | G2 lacked a guard against a G4 stop inside the progress sink's lock | Fix-text | G2: a callback (un)parks outside any shared lock (the progress sink's, `gwz-cli/src/progress_detail.rs:199`) | row 6.5: member A stopped at its unpark while member B's clone in its writing phase emits a route note through a sink holding a mutex: `wait` returns; red against an unpark inside the sink's lock |

**Deferred with an owner:** none of the IDs is deferred. Questions passed on, not closures: the lane owner confirms §3.6's
reading of O4 (a)/OD-C4 (S3-P2-1); D puts to the operator the O4 (a) narrowings (R15's Windows events, R9's LOGOFF/SHUTDOWN
under `user32`, any Windows script row 6.11 finds going on) and S3-P3-4's pager question; C accepts lifecycle mode for every
`server` subcommand (C3-P2-1, C3-P3-6).
