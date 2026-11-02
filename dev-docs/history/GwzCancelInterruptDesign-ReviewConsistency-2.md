# Cancellation design A r2: Consistency-axis review (round 2)

**Review object:** `/Volumes/projects/limbo/gwz-dev/dev-docs/GwzCancelInterruptDesign-DRAFT.md`, revision 2 (working tree, modified against root `7f07dfc6`). Hash at start `bfe7d179e9decb7260ec8fb499b9a8a23f2e28f9`; hash at end `bfe7d179e9decb7260ec8fb499b9a8a23f2e28f9` (unchanged).
**Date:** 2026-11-01.
**Axis:** Consistency. This review checks for internal contradictions. It checks agreement with D-V (every bullet) and with every document and code line the object cites. It checks the superseded-clause table, whether each row can be met on every platform it names, placement against convergence plan §4 and the 2.3 plan, and the hand-offs to designs B, C and D.

**Verdict: NO-GO.** 0 P0, 0 P1, 2 P2, 11 P3.
- Neither P2 is architectural. The architectural count stays at 2 (round 1's), so this round adds no third.
- All 28 round-1 findings are closed on this axis (§1).
- Pre-commit: I will give GO on a revision that resolves P2-1 and P2-2 as their corrections state, provided it adds no new P0-P2 on this axis. The P3 items should be closed in the same pass but do not block.

## 0. Evidence base

- **Pins.** Root HEAD is `7f07dfc6`: one evidence commit after the object's citation root `48feeaad`, touching no cited document. Member heads match the object's citation line: gwz-core `b745f1fa`, gwz-cli `c0ba717`, gwz-py `2185cc6`, gwz-git `9bc4ea9`, gwz-transport `281a28d`, libgit2 `b172e3d18`. CPython 3.10.15 and 3.11.16 `asyncio/runners.py` were read from `~/.asdf/installs/python/`.
- **Authority read:**
  - `GwzTransportReleaseConvergencePlan.md` §10 D-V, :999-1062, every bullet: OQ2 reversed; OD-C1 to OD-C6; the review cap with resolutions (1)-(3); the split; "Designs A and B, round 1", decisions (i)-(iii).
  - OD-C1 to OD-C6 as worded in `git show 9d5a4d4c:dev-docs/GwzCancellationAmendment-DRAFT.md` :238-260.
  - OD-C7 in `history/GwzCancellationAmendment-DRAFT.md`: the wording is at :695-702. The decision is recorded at :687-694.
- **Placement read:** plan §4 graph (:300-331) and Phase 3 (:712-745); 2.3 plan 2.3i, 2.3j, 2.3k, 2.3l and §4 (:181-250).
- **Prior round read:** both round-1 reviews and `GwzCancelInterruptDesign-RemPlan.md`, including fix-pass G1-G13.
- **Hand-offs:** design B is `GwzChildLifetimeDesign-DRAFT.md`, working tree blob `9724eae6` (revision 2). I read its §0, §3.1-§3.8, §4, §6-§8, and its revision 1 at `d146bc9a`, for hand-off points only. Designs C and D do not exist yet.
- **Method:**
  - Every `file:line` in §2, §3, §5, §6 and §8 was opened at its pin with sed, grep and awk.
  - Build-configuration facts came from `gwz-core/src/lib.rs:47-51`.
  - The process-globals wiring came from `gwz-core/scripts/run_tests.py` and the four allowlists.
  - No build or test was run. Nothing was modified. No git mutation was made.

## 1. Prior-finding closure table

| ID | Round-1 finding | Status | What I checked |
|---|---|---|---|
| C-P2-1 (ARCH) | Cancel and the gate were carried on the per-operation entry that 3.1 replaces and 3.3 removes | **Closed** | §3.2 holds the `HostOwner` in a slot and calls `close()`. That call seals admission and cancels operations (`host/owner.rs:21-32`; `shared.rs:154-180`; contract :269). §3.4's carriage goes through `ProcessGate` in `composition.rs:30-35`. 3.0d no longer builds a gated cancellable entry. §8 row 4 amends plan 3.1. No step builds on 3.3's removals. (P2-1 is a different defect: that carriage exists only in candidate builds.) |
| C-P2-2 | O1 could not be met in plan order | **Closed** | 2.3s now sits in Phase 2, wave 2, lane 2. §8 rows 5-7 amend 2.3i (:186-188), 2.3k (:207-217) and §4 (:240, :249-250). No 2.3 plan text still installs a handler in gwz-core. The install site is misplaced (P3-1), and the reader's mode for server commands is undefined (P2-2). |
| C-P2-3 (ARCH) | Safe points sat inside a held ref transaction | **Closed** | G2 forbids them. §3.5 row 2 lists the five writes under a transaction. I verified each: `merge_prepared.rs:248-249` lock, then `:266`, `:280`, `:339` in the same function; `merge_recovery.rs:143-144`→`:175`; `:197-198`→`:251`. All run to their end. Row 6.9 covers them. |
| C-P3-1 | §8 omitted contradicted clauses; off-by-one cite | **Closed** | §8 rows 9-12 cite MachineOutput `:66-69`, `:142-143`, `:280-287`, `:669-673`; row 14 cites `GwzCoreSessionDesign.md:687`. All verified (MachineOutput is `gwz-cli/docs/MachineOutput.md`). |
| C-P3-2 | Long-phase inventory incomplete | **Closed** | §2 and §3.5 match a grep of `checkout_tree`, `reset(`, `.merge(`, `stash_save`, `stash_apply` and `stash_pop` in non-test gwz-core source. `fixture_native.rs` is test-only. |
| C-P3-3 | The `signal` check's runs and B's entries | **Closed as asked** | The five runs exist: `run_tests.py:552`, `:316-327`, `:472-479`; gwz-cli `ci.yml` job at :26; `test_process_globals.py:11-23`. A new ordering gap at B's 2.3r-1 is P3-2. |
| C-P3-4 | `par_map_per_host` plumbing | **Closed** | `par_map_per_host_gated` is defined. The seven calls in six files match the grep. 3.0d's Owns lists them. No thread-local is used. |
| C-P3-5 | Hand-off wording with B | **Closed on A's side** | No gwz-cli `close()`; refused `Kill` spawns; registry creation before install. B §3.7 still says it relies on "`close()` after the wait (steps 5-6)" and still names `pending_local_work`. Those are B's edits (§4). A wording issue about "first threads" is P3-5. |
| C-P3-6 | OD-C4 marking missing on the forced `--jsonl` path and the CTRL_CLOSE race | **Closed** | §3.6's claim rule (130 only if the interrupt preceded the claim); R13 records the forced `--jsonl` case. |
| C-P3-7 | SIGTERM and SIGHUP mapped to 130 without provenance | **Closed** | §9 O4 states the options. §8 row 8 is marked subject to O4. Its blast-radius list is incomplete (P3-6). |
| C-P3-8 | Notice modes contradicted each other | **Closed** | §3.6 Notices ("every output mode") and §8 row 9 agree. §3.2 no longer has a human-only line. |
| C-P3-9 | Citation and fact errors | **Closed** | R17 is gone. 3.10 `runners.py:44-49, :55-63` verified. `NativeCoreBridge.call` at `bridge.py:318` verified. The f9 path is absolute, under heading "Findings outside scope", item 2 verified. 3.0d "After: Phases 1, 2" matches. The graph keeps `3.0, 3.0b → 3.3`. 3.0e's content is defined. |
| S-P2-1 (ARCH) | Parked network calls contained local writes | **Closed (consistency)** | Cites verified: `smart_protocol.c:782`; `fetch.c:209-226`, where no pack means no shallow write; `indexer.c:874, :1469`; `clone.c:666-673`; `remote.c:3032-3038, :3064-3067`; `transport.c:36, :88-89`; `local.c:354, :426`. The unpark points are consistent with G1. |
| S-P2-2 | `wait()` could block on output or scheduler waits | **Closed** | The writer FIFO; `par_map_per_host_gated` frames per item; the host close is awaited outside any frame. A Force that arrives with no Cancel leaves a text gap (P3-11). |
| S-P2-3 | A closed gate still admitted work | **Closed** | G3; `writing`, `park` and `outside` are refused once closed; `is_closed` is passed as `cancelled`; serial loops get a head safe point. |
| S-P2-4 | The forced envelope misreported completed writes | **Closed** | The claim is the last act in the frame; `wait()` returns after the frame ends; the message was changed. |
| S-P2-5 | OD-C7 (a) tears writes on a rapid triple Ctrl-C | **Closed (Record)** | The operator accepted OD-C7 (a) at plan :1061-1062. The tension is put as O5. Its residual wording is P3-6. |
| S-P2-6 | The gwz-py CLI dropped the result | **Closed** | `gwz_response`; `run` renders and returns 130. The new attribute is visible to library callers but undocumented (P3-10). |
| S-P2-7 | A cancel before registration was lost | **Closed** | `cancel_call` keeps a 64-entry record (`operations.rs:28`). It matches `GwzPyPerOperationTransportDesign.md:104`. |
| S-P3-1 | The handler ran in forked children | **Closed** | §3.1 Unix PID check; row 6.1. |
| S-P3-2 | A deferred Force had no escape | **Closed** | §3.7: step 1 runs at once; row 6.15. |
| S-P3-3 | R5 claimed a notice on Cancel | **Closed** | R5 now names the member's `Cancelled` with the catalog remedy, and the Force notice. |
| S-P3-4 | The read-only list was unchecked | **Closed** | Exhaustive `match`, a membership rule, row 6.14. |
| S-P3-5 | LOGOFF and SHUTDOWN unspecified | **Closed** | §3.1 Windows; R9. |
| S-P3-6 | R17 was avoidable | **Closed** | `Output` plus `check_direct_output.py`. Its scope gap is P3-4. |
| S-P3-7 | `--server` behaviour before C undefined | **Closed** | §3.2 states the interim rule. |

The RemPlan's fix-pass G1-G13 items are reflected in the sections it names. I re-checked G3, G6, G9, G10 and G13 directly. None reopens a round-1 ID.

## 2. New findings

### P2-1: the write gate exists only in candidate builds, so Force exits mid-write in the ordinary build, and nothing records this

- **Not architectural; not a new architectural root cause.** The gate model (frames, parks, safe points, Force order) is untouched. Only the constructor path that carries the gate needs a build-independent route.
- **Root cause.** §3.4 "Carriage" puts the process's `WriteGate` in a field of `ProcessGate`. It is made by `process_safety_with_children()` in `composition.rs`, read by gwz-cli through `ProcessGate::write_gate()`, and given to backends by `with_process_gate`. All of `transport_host`, `composition`, `ProcessGate` and `process_safety` exist only under `cfg(all(gwz_transport_candidate, any(unix, windows)))` (`gwz-core/src/lib.rs:47-51`; `transport_host/mod.rs:12, :32`).
- **Location:**
  - §3.4 "Carriage" and "Where frames open and park" (3.1c's dispatch frame);
  - §1 A3 and A9;
  - §7 R10, which names the ordinary build for network work only;
  - §5 rows 3.0d and 3.1c.
- **Violated invariants:**
  - Decision (i): "Force never exits mid-write" (plan :1056-1059).
  - A3.
  - A9's "one behaviour … unless §3 names a mechanism".
  - Agreement with B, which records the candidate-only composition (B §2 :64; §4 Interim :262; R-B13). A does not.
- **Reproduction.**
  1. Take a non-candidate gwz-cli build after 3.1c, which installs for every command and is not cfg-gated.
  2. Run `gwz switch` on a large checkout, press Ctrl-C once and wait for the bound.
  3. Force begins. Step 3 has no gate to close. Step 5's `wait()` has no gate state. The dispatch's `writing` frame has no gate either, because `dispatch.rs:91-96` uses `Git2Backend::new()` and the gate is reachable only through `ProcessGate`.
  4. Step 8 `_exit`s in the middle of a file.
- **Impact.** A torn checkout file, or `index.lock` if the exit lands in the index write. This is decision (i)'s forbidden outcome, in a build configuration A itself names (R10, §3.2 "No host"). It is neither recorded nor put to the operator.
- **Correction.** Choose one:
  - (a) Make the carriage build-independent: gwz-cli creates the process's `WriteGate` in `run` (it is a value, not a process global). Add an unconditional `Git2Backend::with_write_gate(WriteGate)`. In candidate builds the composition copies the same gate into `ProcessGate`.
  - (b) State that the ordinary build gets no write gate. Add a residual, as B's R-B13 does: "Force in a non-candidate build exits as 1.0.18's interrupt does, tearing a write". Mark it "beyond", and decide whether 3.1c installs Force at all in that build.
- **Closure test.**
  - §3.4 names the route by which the ordinary build's dispatch and backends get a gate, with a cite; or §7 has the residual row.
  - Row 6.9's checkout case is stated to run in both builds (M, L, W), or the ordinary build's leg is marked as the residual.

### P2-2: after 3.1c, the reader's bound, Force and `_exit` also apply to server lifecycle commands, which contradicts 2.3i's shutdown and A's own amended 2.3i red row

- **Not architectural; not a new architectural root cause.** One scoping clause on the reader's mode fixes it.
- **Root cause.**
  - 2.3s installs the module for `server start`, its background copy and `server stdio`, "which takes the events through `Interrupts` (their use is C's)" (§3.1 Install).
  - 3.1c then "brings Cancel's slot, the bound and Force".
  - §3.2 "Wait" says: "Cancel starts `CLEANUP_BOUND` … otherwise Force begins". Force step 8 `_exit`s.
  - §3.6 "Status" and §8 row 8 freeze 130 for every interrupted command.
  - No clause exempts the server lifecycle commands, and none defines the reader's mode for them.
- **Location:** §3.1 Install; §3.2 Wait; §3.4 Force; §3.6 Status; §4 C; §8 rows 5, 6 and 8.
- **Violated invariant.** The hand-off must be satisfiable. A's §8 row 5 rewrites 2.3i's red row to "a second stop request or `force` ends the detached wait". 2.3i's five-step shutdown then continues: host-context shutdown; remove socket, clear record, release lock (2.3 plan :183-185). Under A's reader, the second interrupt is Force. Force's `wait()` returns at once, because the server's code is not a dispatch write frame, and `_exit` follows within about 1 s. The same happens when the shutdown outlasts `CLEANUP_BOUND`, which is 5 s, whereas 2.3i adds "one more bound for detached workers".
- **Reproduction.**
  1. Run `gwz server start --foreground` with a live session and a detached worker.
  2. Press Ctrl-C, then press it again within the detached wait.
  3. A's Force steps 1-8 run, and gwz exits 130 before steps 4-5 of the server's shutdown.
- **Impact.**
  - A socket file and a lock record are left behind. The 2.3k `list` and `start` probes do handle a dead server's record.
  - Detached served workers are cut at `_exit`. Their backends do not carry A's process write gate (B §3.7: `DriverOnly` spawns unregistered until C), so Force never waits for them. Decision (i) breaks on the served path.
  - The exit status of a server stopped by SIGTERM is frozen at 130 by §8 row 8, although §4 gives it to C.
- **Correction.** State, in §3.1 and §3.2, that for a server lifecycle command the reader only delivers Cancel and Force events to `Interrupts`. There is no `HostOwner` slot, no `CLEANUP_BOUND`, no Force steps and no forced exit. Shutdown, and the exit status, are C's (2.3i/2.3k). §8 row 8's MachineOutput row excludes these commands, and R12 is extended or a new row is added.
- **Closure test.**
  - §3.1, §3.2, §3.6 and §8 row 8 agree on the server lifecycle mode.
  - A row, or the 2.3k red row in §8 row 6, shows that a second SIGINT to a foreground server whose detached wait is in progress lets the shutdown remove its socket and record.

### P3-1: 2.3s "installs only for a server lifecycle command", but none exists until 2.3k

- **Location:** §3.1 Install; §5 row 2.3s (Owns `gwz-cli/src/lib.rs`); §8 row 6.
- **Problem.** At gwz-cli `c0ba717` there is no `Server` subcommand: `clirequest/` and `globalargs/parser.rs` have none. 2.3k creates it in wave 4 (2.3 plan :207-210); 2.3s lands in wave 2. 2.3s's edit to `lib.rs` has nothing to install for. Its rows 6.1-6.3 can only run in a harness.
- **Correction.** 2.3s provides `install`, `Interrupts` and a test harness. §8 row 6 says 2.3k calls `install` in its run-own-lifecycle branch. Drop `lib.rs` from 2.3s's Owns unless something else needs it.
- **Closure test.** §5 and §8 name one step that calls `install` for server commands, and that step runs after the command exists.

### P3-2: B's signal calls land in 2.3r-1, not 2.3r-2; the ordering and allowlist hand-off is incomplete

- **Location:** §3.1 Check; §4 point 7; §5 row 2.3s; §8 row 7.
- **Problems:**
  - B's 2.3r-1 builds §3.1-§3.5 in `crates/children`: `pre_exec` `SIG_IGN` and the reaper's `SIG_IGN` set (B §3.2 :122-124, §3.4 :167-168). gwz-core's check scans `crates/*/src/lib.rs` (allowlist roots). So the `libc::signal`/`sigaction` occurrences, and the allowlist entries they need, arrive with 2.3r-1. 2.3r-1 runs in wave 2, lane 3, in parallel with 2.3s, and owns no allowlist.
  - If 2.3r-1 merges first with `env`-kind entries, 2.3s's re-kinding breaks gwz-core's check. 2.3s does not own gwz-core's allowlist.
  - Cross-repository order is unstated. gwz-cli's `signal`-kind entry is checked against gwz-core main (`ci.yml` :26-31), so gwz-core's half of 2.3s must land first.
  - §8 row 7 makes a relative change to the 2.3 plan totals ("one step, 300 and 300 more"), while B §8 row 4 replaces the same sentence (:249-250) with absolute figures. The result depends on which is applied first.
  - 3.0d creates `crates/write-gate/` but omits gwz-core's root `BUILD.bazel` label and `Cargo.lock` from its Owns. B lists both for `crates/children`.
- **Correction:**
  - The edge becomes "2.3s before 2.3r-1", or 2.3s owns gwz-core's allowlist re-kinding.
  - State gwz-core-first landing for 2.3s.
  - Fix the totals as one absolute figure agreed with B.
  - Complete 3.0d's Owns.
- **Closure test.** Whichever wave-2 step merges first, gwz-core's and gwz-cli's checks stay green. One totals sentence results whatever the application order.

### P3-3: the graph lacks `3.0f → 3.1c`, which row 6.13 needs

- **Location:** §8 row 1; §5 row 3.1c "After"; row 6.13.
- **Problem.** Row 6.13 [3.1c], three-member case: "member 1 commits, stdout is the envelope". That needs 3.0f's safe point at each member's head in `handle_commit.rs:179-186`. Without it, Force waits for every member and the main thread holds the claim. 3.1c depends on {3.1, 3.0d, 3.0e} only.
- **Correction.** Add `3.0f → 3.1c`, or move that leg into a 3.0f row.
- **Closure test.** Every [3.1c] row's prerequisites appear in its "After".

### P3-4: `check_direct_output.py` scope does not cover files 2.3k creates or gwz-cli test sources

- **Location:** §3.1 Writer; §5 row 3.1c.
- **Problems:**
  - 2.3k creates `server_exec.rs`, `server_after.rs`, `server_long.rs` and `clirequest/server.rs` before 3.1c. 3.1c's Owns lists explicit files and does not include them.
  - `src/tests/` contains 2 print or handle sites, and no exclusion is stated.
  - The 3.1c file list does otherwise match the current inventory: 12 files with sites outside `src/hook/` and `hook_exec.rs` (verified).
- **Correction.** 2.3k writes through `Output` (§8 row 6), or 3.1c owns those files. State the test-source exclusion.
- **Closure test.** The check passes on the tree 3.1c merges into, with only 3.1c's Owns edited.

### P3-5: install order wording conflicts with B

- **Location:** §3.1 Install ("the reader and writer are gwz's first threads"); §4 point 3.
- **Problems:**
  - B's `create` starts a writer thread and a watch thread (B §3.4 Creation :163-165), and 3.1 puts the registry before A's install. A's threads are therefore not first.
  - For server commands in Phase 2, A installs before any registry. If C gives those commands a registry (B §3.7 To C), B's "before any thread" rule (B §3.1 :101, §3.5) requires it before A's install. A's "right after parsing" does not provide for that.
- **Correction.** Say "the first threads after B's registry". Name the order for server lifecycle commands, as a hand-off point to C.
- **Closure test.** A and B name one order in `run` for every command class.

### P3-6: O4, O5 and the residuals that depend on them are internally inconsistent

- **Location:** §1 A3; §7 R8, R14, R15; §9 O4, O5.
- **Problems:**
  - O4's change list omits:
    - row 6.15's three SIGTERMs;
    - §3.7's "or SIGTERM";
    - row 6.1's source list;
    - the 2.3k red row in §8 row 6 ("SIGINT, SIGTERM and Ctrl-Break").
  - R15's "the Windows events with O4" is ill-posed. O4's options define gwz-cli only, and OD-C1 and 3.2b's test forbid any gwz-py handler. No answer to O4 can change gwz-py.
  - O5's change list names only "row 6.13's 50 ms case". The whole clause "an interrupt during the wait ends gwz by default" changes, and so does B's text about an interrupt restoring the default (B R-B4, OD-B1 :340-341).
  - While O5 is open, A3 and R8 ("only SIGKILL ends a hung write") contradict §3.1 "Giving control back" and R14.
- **Correction.** Complete both lists. R15's Windows cell becomes "decided by OD-C1/OD-C7 (d)'s shape, or put as its own question". A3 and R8 say "only SIGKILL, or (O5 open) a later interrupt".
- **Closure test.** Every clause that changes under either answer is listed. No cell refers to an option that cannot change it.

### P3-7: §7's "Beyond" definition does not match its use

- **Location:** §7 preamble; rows R1-R4, R6, R16.
- **Problem.** The definition reads: "not covered by D-V's SIGKILL, crash and power loss or decision (i)'s hung write". R1-R3 (safe-point residue) and R4, R6, R16 (residue of network ended at once: temporary pack, partial member directory, temporary clone) fit none of those categories, yet are marked "no". The usage implies "a consequence of a D-V-ordered mechanism (resolution (1)'s safe points; decision (i)'s network work at once)". D relies on this column to decide what goes to the operator.
- **Correction.** Restate the definition to include D-V-ordered mechanisms, or mark R4, R6 and R16 "decided (i)". R6 (a partial member directory that blocks re-materialize) deserves an explicit sentence.
- **Closure test.** Each "no" cell names the D-V clause that covers it.

### P3-8: some rows cannot be met as written on Windows

- **Location:** §6 preamble ("each runs on M, L, W unless marked"); rows 6.1, 6.12, 6.15.
- **Problem.** Row 6.1 (SIGHUP and SIGQUIT legs; SIGINT to a group of forked children), row 6.12 (SIGTERM) and row 6.15 (three SIGTERMs) are not marked. Windows has none of those signals. §3.1's Windows-specific rule, that a CTRL_C disabled at creation stays disabled, has no row.
- **Correction.** Mark the Unix legs M, L. Give W analogs: `GenerateConsoleCtrlEvent` CTRL_BREAK for the SIGTERM legs, and a CREATE_NEW_PROCESS_GROUP child for "stays disabled".
- **Closure test.** Every leg of every row is satisfiable on each platform it runs on.

### P3-9: citation precision and provenance

- **Location:** §3.3; §2; §3.8; §0.
- **Items:**
  - §3.3 says "B retains the tree (`owner.rs:298`) in the cleanup domain". `owner.rs:298` retains into the endpoint's `AuthOwner`. The link that makes `HostClose` `Pending` is the adapter's disposal waiting on `pending_cleanup_count()` (`src/git/endpoint/https_adapter/worker.rs:268-340`), which the design should cite.
  - `local-import/src/lib.rs:337-339` is the trait. The poll is at `:509`.
  - The constant for the last 64 records is at `operations.rs:28`.
  - D-V's split names "cancellable entry" as one of A's topics (plan :1050). Revision 2 rightly replaces it with the `HostOwner` slot, but the design body does not say so; only the RemPlan does.
- **Correction.** Fix the cites and add one provenance sentence to §0 or §3.2.

### P3-10: SURFACE items not frozen in §8

- **Location:** §3.8; §8 rows 8, 15, 16.
- **Problems:**
  - `CancelledError.gwz_response` reaches gwz-py library callers ("a library caller still sees `CancelledError`", now carrying the attribute). It is absent from §8 rows 15-16.
  - §8 row 8's "130, whatever the aggregate" over-claims. It does not hold for `hook` and `claude-code-setup` (R12) or for server commands (P2-2). It omits §3.6's condition that the interrupt came before the response was claimed.
- **Correction.** Add the attribute to rows 15 and 16, and qualify row 8.

### P3-11: own-lifecycle commands and a Force with no Cancel

- **Location:** §3.6 The claim; §3.1 Writer.
- **Problems:**
  - `diff` and `log` run outside dispatch (`gwz-cli/src/lib.rs:226-285`), so they take no claim. A Force at the bound during a non-pager `diff` or `log` finds the claim free and writes the forced `--json` envelope after the command's partial stdout.
  - §3.1's "a sender waits on a full queue only until Cancel" leaves senders blocked when Force comes with no Cancel (CTRL_CLOSE, LOGOFF, SHUTDOWN). G1 says that cannot happen.
- **Correction.** State the claim rule for `diff` and `log`: they take it at their first stdout write. Write "until Cancel or Force".

## 3. Verified citations

All were opened at the pins in §0. Each matches unless it is listed under a finding.

- **D-V and plans.**
  - Plan :315, :325, :329-331, :712, :717-720, :721-728, :736-738, :999-1062. Within D-V: OQ2 :1005-1010; OD-C1 to OD-C6 :1030-1031; resolutions :1035-1045; split :1046-1054; round-1 decisions :1055-1062.
  - 2.3 plan :181-188, :202, :207-217, :232-250.
  - Amendment r1 §6 at `9d5a4d4c`; OD-C7 at history :695-702.
- **gwz-cli.** `lib.rs:160-178, :167, :174, :279`; `dispatch.rs:57-89, :82-87, :91-96`; `render_exit.rs:82-95`; `pager.rs:130, :138-145, :146, :155-170`; `forall.rs:260`. The direct-output inventory matches 3.1c's Owns.
- **gwz-transport.** `host.rs:6-10`; `host/owner.rs:21-32, :50-59`; `host/config.rs:22, :36` (`close_bound_ms: 5_000`); `operation.rs:343-344, :414-417`; `host/shared.rs:154-180` (seal cancels bindings); contract :269, :272-275.
- **gwz-core.**
  - Composition and checks: `composition.rs:21-35`; `lib.rs:47-51` (candidate-only `transport_host`, P2-1); `check_process_globals.py:103-104, :540-541`; allowlist roots.
  - Fan-out: `par_map_per_host.rs:35-60` and its seven callers.
  - Git backend: `contract.rs:42`; `backend.rs:26`; `transport.rs:32, :41, :55-59, :73, :368, :718, :740`; `transport_support.rs:89-92, :130`; `repository_support.rs:30-35`; `repository.rs:31, :56, :81, :112`; `refs.rs:33, :134`; `stash.rs:18`; `stash_support.rs:18-26`; `preservation.rs:344`.
  - Merge: `merge_prepared.rs:248-249, :266, :280, :339`; `merge_recovery.rs:143-144, :175, :197-198, :251`.
  - Commit, copies and locks: `handle_commit.rs:179-186`; `commit_log/merge.rs:189`; `handle_local.rs:72`; `family_merge.rs:424`; `local_clone/wait.rs:3-6`; `session_host/served.rs:81`; `copy-contract/lib.rs:39-52`; `refcopy/ordinary.rs:14-17, :477`.
  - Bounds and HTTPS: `session_host/context.rs:80` (`CLEANUP_BOUND`, unconditional module); `https_auth.rs:28, :190`; `owner.rs:253-276, :298`; `https_worker.rs:377`; `runner.rs:226`; `file_worker.rs:91-98`.
- **libgit2.** `smart_protocol.c:782`; `fetch.c:209-226`; `indexer.c:874, :1469`; `clone.c:666-673`; `remote.c:3032-3038, :3064-3067`; `transport.c:36, :88-89`; `local.c:354, :426`; `checkout.c:1886-1891, :2686`; `reset.c:154, :159, :167`; `stash.c:1126, :1149, :1159, :1282-1285`; `refdb_fs.c:1175-1179`.
- **gwz-py and CPython.** `bridge.py:28-35, :298-316, :318`; `cli.py:76, :142-156`; `client_host.rs:226-231`; `shims.rs:79-80`; `operations.rs:199-208`; per-operation design :104; README :225. CPython 3.11 `runners.py:117-124, :150-157`; 3.10 `runners.py:44-49, :55-63`.
- **Documents.** MachineOutput (gwz-cli) :66-69, :142-143, :280-287, :669-673, :954-964; `ErrorCatalog.md:35`; `GwzCoreSessionDesign.md` §5.3 and :687; f9 report "Findings outside scope" item 2.
- **"No handler today."** A grep over the five repositories found no `libc::signal`, `libc::sigaction`, `SetConsoleCtrlHandler`, `ctrlc::`, `tokio::signal` or `signal_hook`. gwz-py's Python has no `signal.signal`, `set_wakeup_fd` or `add_signal_handler`.

### SURFACE (what A freezes)

- **Exit status.** 130 on every platform for SIGINT, and (O4 (a), pending) SIGTERM, SIGHUP and Windows CTRL_C, CTRL_BREAK, CTRL_CLOSE, LOGOFF and SHUTDOWN. It applies when the interrupt comes before the response is claimed. Exceptions: `hook` and `claude-code-setup` die by signal (R12). Server lifecycle commands are undefined (P2-2). The gwz-py CLI exits 130 on SIGINT; on SIGTERM and SIGHUP it dies by signal (R15).
- **JSON (`--json`).** The first own `errors` entry is `{"code":"Cancelled","message":"interrupted",…nulls}`. `aggregate_status` is unchanged, so `Ok` with exit 130 is possible. An interrupted top-level error carries two entries.
- **Forced envelope.** `meta: null`, no members, message `interrupted; forced exit, see stderr for what stopped`. A forced `--jsonl` stream has no final response.
- **stderr, every output mode:**
  - `gwz: interrupted; cancelling (interrupt again to stop now)`
  - `gwz: forced exit; network work ended, waiting for local writes to finish or reach a safe point`
  - `gwz: stopped: <member path> (<phase>)`
  - `gwz: transfer ended: <member path> (fetch|push|clone)[; the remote may have received it]`
  - `gwz: <n> progress lines dropped`
  - `gwz: cleanup still pending`, which replaces the `gwz-alpha` pending-jobs line.
- **`forall`.** `interrupted: <k> of <m> members not run`.
- **gwz-py.** `CancelledError.gwz_response` is visible to library callers; it is not in §8 (P3-10). Cancelling a task that awaits a transport-route call cancels the operation first.

## 4. Risks and next action

- **Highest risk: P2-1.** Decision (i) holds only in candidate builds. The ordinary build's Force tears writes silently. The fix is a build-independent constructor for the gate, or an explicit residual.
- **Second: P2-2.** The reader's universal Force pre-empts 2.3i's server shutdown and the served writes. One scoping clause closes it, and it should be agreed with C's 2.3i/2.3k owners.
- **Hand-off risks on B's side (B's edits, not A's findings):**
  - B §3.7 still relies on "`close()` after the wait (steps 5-6)" while it also says gwz-cli needs no `close()`.
  - B §3.6 still names `pending_local_work` and "A's Cancel token".
  - B §4 lacks the 2.3s edge, and its 2.3r-1 needs allowlist entries (P3-2).
- **Not yet built.** The 3.1 `HostOwner` path (plan 3.1) and the HostClose `Pending` link through the HTTPS adapter's disposal do not exist yet. Rows 6.8 and 6.11 are the guards.
- **Next action.**
  1. Revision 3 resolves P2-1 and P2-2 per their corrections and closes P3-1 to P3-11 in the same pass.
  2. B's owner applies the B-side hand-off edits above.
  3. Re-review on this axis against the new blob. Exit is GO with no new P0-P2.
