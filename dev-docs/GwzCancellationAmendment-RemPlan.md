# GwzCancellationAmendment — remediation plan, round 1

Date: 2026-11-01. Status: **remediation plan for the lane owner; it authorizes nothing beyond revision 2 of the
amendment.** Drafted read-only except for this file and the amendment; no git operation was run (the lane owner
commits).

- **Object:** `dev-docs/GwzCancellationAmendment-DRAFT.md`, revision 1, blob `3448c561`, root `9d5a4d4c`.
- **Reviews:** [Consistency](GwzCancellationAmendment-ReviewConsistency.md), NO-GO, 10 P2 and 9 P3;
  [Safety](GwzCancellationAmendment-ReviewSafety.md), NO-GO, 11 P2 and 5 P3. No P0 or P1.
- **Result:** revision 2, edited in place (827 lines after two fix rounds), status "revision 2 (2026-11-01) … for
  round-2 review by fresh reviewers". Line numbers below are revision 2's as it now stands.
- **Fix rounds.** A completeness check of revision 2's first draft found G1 to G15 and I1 to I19 (second table). A
  second check found GAP-1 to GAP-17 and a fresh Safety pass S-R2-1 to S-R2-11; while closing GAP-9 the drafter found
  F1 (third table). All are closed in place in revision 2; the first table's rows point at the closing text.
- **Verification:** every fact revision 2 adds was re-read at root `bd082c89`, gwz-core `b745f1fa`, gwz-cli `c0ba717`,
  gwz-py `2185cc6`, gwz-git `9bc4ea9`, gwz-transport `281a28d`, libgit2 `b172e3d18`, git2-rs `d13951f`, tokio 1.53.1
  (cargo registry), Rust std 1.96 (rustup `stable`, rustc 1.96.0) and CPython 3.11 (local install); every source path
  in revision 2 was resolved by script under its header's path rule (only files the steps create do not exist). No
  reviewer claim was copied unchecked. Python 3.10 is not installed on the host, so §3.7's 3.10 path is stated without
  a line citation and row 19 verifies it. Reviewer facts that did not hold: C-P3-2's two (in its row); Safety's
  parked :809 citation (:809 is the stdio-child row, :808 the background start; no finding rests on it); Safety
  P2-9's "checked-artifact directories have their own recovery" (`verified_write.rs:30-34`: a leftover is operator
  cleanup, no `recover()`); and I17 below. Revision 2's own first draft had one fact that did not hold, F1.

## Blind convergences

The two reviews were peer-blind. They converge on:

1. Consistency P2-2 = Safety P3-5: OD-C1's single installer against 2.3i's gwz-core handler.
2. Consistency P2-5 = Safety P2-7: the forced exit cannot deliver C4 (architectural).
3. Consistency P2-6 = Safety P2-5: gwz-py has no working cancel or reaper path (architectural).
4. Consistency P2-7 = Safety P2-1: the reaper's launch contract (group and signals, start failure, its own death).
5. Consistency P2-8 = Safety P2-2: C2 against `retire` and TR1.6 §3.3; "a group ends" undefined.
6. Consistency P2-9 = Safety P2-10: the `GIT_ELOCKED` message infers staleness from the mutator lock.
7. Consistency P3-2 = Safety P2-9: the temp inventory and the sweep's reach.
8. Consistency P3-3 = Safety P3-2: keeping the helper's stdin open gives no signal.
9. Consistency P3-7 = Safety P3-4: OD-C6 against C5's bound.
10. Partial: Consistency P3-6 and P2-10 with Safety P2-11 (native push abort, and where a cancelled push's possible
    effect appears); Consistency P3-4 with Safety P2-1's closure rows (SIGQUIT, SIGHUP, SIGINT then SIGKILL).
    Consistency's three unscored risks (process-group ID reuse, fork without exec, a panic writing to a closed
    terminal) are Safety P2-2, P2-5 and P2-6.

In fix round 2, GAP-12 (the completeness check) and S-R2-4 (the Safety pass) found the same E2 contradiction
independently.

## Architectural count: three (the operator's, D-V)

`GwzTransportReleaseConvergencePlan.md` §10 D-V (:1032-1045) records three architectural root causes and the
operator's accept-and-remediate decision:

1. **The forced exit** (Safety P2-7, with Consistency P2-5).
2. **gwz-py's reaper and cancel path** (Safety P2-5, with Consistency P2-6).
3. **Step placement against the plan's phases** (Consistency P2-1).

## The operator's resolutions, and where revision 2 gives them their shape

- **(1)** The forced exit kills children and network work at once; a local write sequence in progress runs to its
  next safe point; long local phases get file-granular safe points; SIGKILL, a crash or power loss can still tear a
  write, and each residue gets a named recognizer. → C4 (:48-54); §3.3 `Force` (:245-251), the write sequences and
  their parked states (:253-266), the exit points (:267-272), **the inventory of long local phases** (:273-277),
  records (:278-288), the forced notice (:293-304); §3.5 (:431-472); §7 R1, R1a, R10 (:729-734, :754-756). "Network
  work" is a whole `download` or push call, during which gwz writes nothing and libgit2 writes only its temporaries;
  their residue is R1a, attributed to (1)'s own rule. Triggers beyond SIGKILL, crash and power loss are not
  attributed to (1): they are OD-C7, for the operator (§6 :695-702; §7 R1b). The two long phases with no callback
  (`stash push`'s reset, a Git CLI route child) are waited for and stated as R10.
- **(2)** The reaper lives in gwz-core as a process forked when the registry is created; its loop only reads the
  lifeline and calls `kill()`; gwz-cli and gwz-py share it; gwz-py's caller cancellation calls the `Canceller` before
  waiting; fork without exec is a documented constraint. → §3.4 (:305-430; the loop :382-386, using
  `pidfd_send_signal` on Linux for `Process` entries, disclosed as a choice in §6 :709-726); §3.7 (:501-528); §7 R6-R7,
  R9.
- **(3)** A new Phase 2 sub-step before 2.3j builds the registry and reaper; the client interrupt protocol moves from
  2.3k to 3.1. → §4 (2.3r :537, wave 2 lane 3 :543-545; 3.1 :555); §8 rows 6-8, 19-20.
- **OD-C1 to OD-C6, as recommended** → §6 (:689-693).

## Choices revision 2 makes inside those decisions (for round 2 to judge, not operator decisions)

Listed in revision 2 §6 (:709-726): §5.3 unchanged, with pull and materialize stoppable between members at their
existing network crossings; `close_wait` as the served bound; the wire unchanged; residues reported, never removed,
and named at once by the forced notice; `open_operation` for a recorded member; the record's liveness lock; the 1 h
report age; capacity 256; reaperless registries for the old entries and hostless backends until 3.3; the stdio
watchdog's 10 s; the reaper's pidfd on Linux; inherited ignored signals kept, and the inherited dispositions
restored; a second interrupt during an E2 wait makes `Force` pending; the SIGCHLD reset in gwz-cli and the registry's
refusal; the Git CLI route child as a `Writer`; and placement: 2.3r in wave 2 (D-V (3)'s sub-step), **2.3s in wave 4,
which moves OD-C1's gwz-cli module into Phase 2 and is not an operator decision** (D-V (3) names one sub-step), the
new Phase 3 steps 3.0c, 3.0d, 3.0e, 3.1c, 3.2b (3.0f under OD-C8 (a)), and 3.1 after 3.0d.

## Findings and dispositions (round 1)

Dispositions: **Fix** (the design changes), **Fix-text** (the text is corrected; the design stands), **Dispute**
(with evidence), **Record** (kept as a stated residual). "Closed at" is revision 2's section and line. A Fix that
departs from the reviewer's required correction says so and why.

| ID | Finding | Disposition | What revision 2 does | Closure test | Closed at |
|---|---|---|---|---|---|
| C-P2-1 | §4 placements break the plan's phase order and 2.3's ownership (architectural) | Fix | Per D-V (3): 2.3r before 2.3j (wave 2, lane 3, after 2.3e); 2.3s then 2.3k in wave 4; client interrupt in 3.1, after 3.0d (GAP-3); every Phase 2 and 3 step has Owns and Review columns, disjoint within its wave; 3.0d → 3.0e → 3.0c; each §5 row has one owner whose predecessors build it (rows 7, 13, 18a moved by GAP-1, GAP-2); 2.3 plan and plan amendments listed | Each §4 "After" exists earlier in the graph; no two parallel steps own one file or sit on both sides of an unsettled interface; every §5 row's owner's predecessors build what it needs | §4 :530-572; §5 :574-685; §8 rows 6-8, 19-20 :769-771, :782-783 |
| C-P2-2 | OD-C1 contradicts 2.3i's gwz-core handler | Fix | One module in gwz-cli; 2.3i takes the value and installs none; tokio's SIGCHLD handler removed with `tokio::process`; dependency check on tokio's `process` and `signal` features | Row 18a: the `signal` kind's two allow-listed sites; mutations fail; row 7: `tokio::process` refused | §3.1 :166-210; §3.4 :325-331; §8 rows 2, 5, 19 |
| C-P2-3 | Served first-interrupt wait silently 5 s instead of the close bound | Fix | C5 names the bound per form; split doc :242 restated | Row 16 | C5 :55-60; §3.6 :476-481; §8 row 3 |
| C-P2-4 | "Between members" safe points amend §5.3 and allow cross-member partial state | Fix | Option (a): §5.3 unchanged; a mutator-lock W handler with no network request (commit, stage) is all or none; a W handler with idempotent per-member network steps (pull, materialize) can stop only at §5.3's existing crossings, each member whole, `Partial` with `cancelled` members, re-run finishes | Row 11: commit all-or-none under `Cancel` and `Force`, with a CLI-route member too; pull stopped at member 3 leaves 1-2 updated, 3-5 untouched, `Partial` | §3.2 :216-222; §3.3 :253-288 |
| C-P2-5 | The forced path cannot deliver C4/C5 (architectural, with S-P2-7) | Fix | `Force` waits for every W method's run and every N step's local write, unless parked at an exit point, inside a network call or in a member-lock wait; long local phases inventoried; exits running no gwz code are R1 or OD-C7's R1b | Rows 9-11, 13, 29, 30 | C4 :48-54; §3.3 :245-277; §3.5; §7 R1-R1b, R10; §8 row 9 |
| C-P2-6 | gwz-py's interrupt path misdescribed; first interrupt does not cancel (architectural, with S-P2-5) | Fix | §2 corrected with CPython's runner and executor facts; `Canceller` then wait; a second stops waiting; the CLI force-exits through `_force_exit` instead of waiting on the executor | Row 19 on 3.10, 3.11, 3.13: first SIGINT cancels, second exits 130 within 1 s, no child left | §2 :153-163; §3.7 :501-528; §8 rows 14-15 |
| C-P2-7 | The reaper is not fully specified | Fix | Fork in gwz-core; own session and ignored signals; the parent closes its read end at once; pre-exec SIGPIPE ignored around the write (S-R2-11); a failed fork or its death fails closed; residual R9 | Rows 3, 5, 7 | §3.4 :349-398; §7 R9 |
| C-P2-8 | C2's universal scope contradicts TR1.6 §3.3 and OD-C6; "a group ends" undefined | Fix | C2 scoped; E1-E6 named; registration and unregistration defined (by token, S-R2-1); E3 and E6, which narrow D-V, go to the operator as OD-C8 | Row 6; row 25 under OD-C8 (a) | C2 :43-45; §3.4 :349-361, :402-417; §6 :703-708 |
| C-P2-9 | The `GIT_ELOCKED` message claims staleness a concurrent fetch defeats | Fix | The message names path and age, says gwz cannot tell, lists the checks, gives removal commands, draws nothing from the mutator lock | Row 15 | §3.5 :467-470 |
| C-P2-10 | OD-C4 has no field, schema or forced-path output | Fix | No new status; `errors[0].code` `cancelled`; per-mode output after `Cancel` and `Force`; the `Output` sink; the envelope only when nothing was written; the forced notice in every mode (GAP-5); amendments listed | Rows 21, 23 | §3.3 :289-304; §8 rows 12, 13, 18 |
| C-P3-1 | Citations drift or dangle; "gap 11" undefined | Fix-text | One citation tuple; a path rule in the header (GAP-17) under which every source path resolves; every citation re-read; "gap 11" dropped. **Partly disputed:** the fix round's claim that split doc :247 is the test-only hook (I17) | Every cited path resolves under the header's rule and every cited line holds the quoted substance (scripted path check run) | header :3-24; §2 |
| C-P3-2 | Temp facts and sweep inventory incomplete | Fix | Inventory of shapes, writers and directories in the design. **Partly disputed:** `gwz-memory-*.tmp` is test-only (`src/filesystem.rs:41`); the family store's `.tmp.` marker is read only by its tests (`publish.rs:239`) | Row 14: one temp per inventoried shape and directory | §2 :140-148; §3.5 :448-466 |
| C-P3-3 | Keeping the helper's stdin open reaches no helper | Fix | The bullet is removed; TR1.6 §3.3 "Pipes" unchanged | No row depends on it | §2 :113-126 |
| C-P3-4 | Rows not all red-first; signal coverage and platforms incomplete | Fix | Regression legs marked **(reg)** where they stand, the mark applying to the leg it follows (GAP-10); every signal and console event has a row; platforms on every row | Red evidence per named platform for each unmarked leg | §5 :574-685 |
| C-P3-5 | "gwz-core and gwz-py install none" not enforced | Fix | Kind `signal`; exactly two allow-listed sites, gwz-cli's module and gwz-core's children crate (GAP-8); the dependency check | Row 18a | §3.1 :181-189 |
| C-P3-6 | Native-transfer abort overclaimed | Fix | Callbacks unconditional; `download` and `update_tips` split; anonymous pushes; uncovered phases stated | Row 12 | §3.2 :223-231; §7 R2-R3 |
| C-P3-7 | OD-C6's unbounded wait conflicts with C5 | Fix | No bound runs while a foreground program runs; a second interrupt makes `Force` pending, a third ends gwz by default, CTRL_CLOSE does not wait (GAP-12) | Row 20 | C5 :55-60; E2 :404-410 |
| C-P3-8 | The child registry is a new state owner the contract does not account for | Fix | Kill authority; retention stays the retained record; counted once | Row 6's retained-child leg | §3.4 :307-317; §8 row 11 |
| C-P3-9 | Per-step review tiers contradict plan §4 | Fix-text | Tiers follow plan §4 and the 2.3 plan; 2.3r added to the 2.3 plan's dual list; the Phase 3 table has a Review column, with 3.0f dual (secret handling) and 3.0c phase-only with its reason (GAP-7) | §4's tiers match both rules for every step | §4 :530-572 |
| S-P2-1 | The reaper has no launch contract and dies with gwz | Fix | Launch contract; the parent's read end closed; pre-exec SIGPIPE handling; fail closed on fork failure or death | Rows 3, 4a, 5, 7 | §3.4 :349-398 |
| S-P2-2 | Unregistration order undefined against the reap and `retire` | Fix | Pre-exec self-registration; waiter `waitid(WNOWAIT)`, unregister, reap; unregistration on every spawn error (S-R2-1); `retire` unregisters; foreign reaping and never-zombie children stated with R4 and the README | Rows 6, 26, 27 | §3.4 :325-331, :349-369; §7 R4 |
| S-P2-3 | Two registered children lead no group | Fix | Typed entries (`Group`, `Process`, `Writer`, `Stdio`); `Process` ended by `kill(pid)`, through a pidfd on Linux (restored by S-R2-8; macOS keeps `kill(pid)`, R4); `ssh`'s own children stated | Rows 6, 7, 17a | §3.4 :332-348, :382-386 |
| S-P2-4 | Force, reaper and job SIGKILL the stdio core mid-write | Fix | `Stdio` writes no lifeline record, has no job, and is outside every registry kill path (S-R2-5); its own watchdog: `force_exit` at `close_wait`, `_exit` at `close_wait` + 10 s. **Departs from the required correction** (close stdin, SIGTERM, SIGKILL after `close_wait`): after its client's death the core is init's child, so an outside kill by PID can reach a reused PID, and a client that is SIGKILLed cannot send it; the watchdog runs in the core itself | Row 17b | §3.4 :344; §3.6 :485-489; §6 OD-C7 (e), OD-C8 (b) |
| S-P2-5 | gwz-py has no reaper path and no C1 path (architectural) | Fix | D-V (2)'s shared reaper; `Canceller` first; `_force_exit`; SIGTERM and SIGHUP to OD-C7 (d) | Row 19 | §3.4 :370-398; §3.7 :501-528 |
| S-P2-6 | The handler never gives control back; `Force` can block or panic | Fix | The saved dispositions restored when `Force` begins (OD-C7 (a)) and on drop; non-blocking self-pipe; the `Output` sink and its check; `Force` never waits for a stream lock | Rows 8a, 8b, 4b, 21 | §3.1 :194-206; §3.3 :245-251, :289-304 |
| S-P2-7 | C4 and C5 conflict: `Force` lands inside unbounded write sequences (architectural) | Fix | As C-P2-5 | Rows 9-11, 13, 29, 30 | §3.3 :253-277 |
| S-P2-8 | "Never published" admits libgit2's temp packs forever | Fix | The required correction's second option: the `objects` residues are reported by path and `mtime` age, never removed, because no cross-process exclusion covers fetch; named at once by the forced notice (S-R2-10); attributed as R1a (GAP-15) | Row 13 | C4 :48-54; §3.5 table :440; §7 R1a |
| S-P2-9 | The temp sweep reaches files gwz does not own | Fix | No sweep: every residue reported, never removed; exact patterns, regular files by `lstat`, inventoried directories only; root, `.claude/` and `.git/info/` only for their fixed target names. **Departs from the required correction** (remove dead temps under the lock): removal needs an exclusion that proves the writer dead, and only the workspace mutator lock exists while fetch and the artifact writers outside it take none (`src/operation/memberlockmanager.rs:4-15`); a report cannot destroy a live write | Row 14 | §3.5 :431-466 |
| S-P2-10 | The `GIT_ELOCKED` report uses the mutator lock as a staleness test | Fix | As C-P2-9 | Row 15 | §3.5 :467-470 |
| S-P2-11 | A push's possible remote effect has no carrier on native and forced paths | Fix | Push phase per member; native, transport, forced and served paths; the forced notice carries it when no document can (GAP-5). **Departs from the required correction** (`cancelled` plus `effect: possible`): the wire is frozen (§9; OD-C4 adds no status or field), so the effect is the `detail` prefix `possible remote effect:` of the `cancelled` member error, and its own notice line on standard error | Rows 12, 23 | §3.2 :232-238; §3.3 :293-304; §3.6 :476-481 |
| S-P3-1 | On macOS the lifeline's write end can leak into a concurrent spawn | Fix | Lifelines exist only from `child_registry()`, called once per process before any host (gwz-py) or before any thread (gwz-cli); the old entries' registries are reaperless and make none | Row 5's concurrent-spawn leg in the harness | §3.4 :307-317, :370-379; §7 R6, R9 |
| S-P3-2 | Keeping the helper's stdin open gives no signal | Fix | As C-P3-3 | As C-P3-3 | §2 :113-126 |
| S-P3-3 | Only the helper has a job on Windows; CTRL_CLOSE must block | Fix | A job for every `Group`, `Process` and `Writer` entry; CTRL_CLOSE blocks with no state beyond the allow-listed pipe end (GAP-13) | Rows 4a, 4b, 24 | §3.1 :198-202; §3.4 :399-401 |
| S-P3-4 | OD-C6's lifecycle against C2 and C5 incomplete | Fix | As C-P3-7; E2 an exception to C2 | Row 20 | E2 :404-410 |
| S-P3-5 | OD-C1's single owner collides with 2.3i's handler | Fix | As C-P2-2 | Row 18a | §3.1 :207-210; §8 rows 5, 19 |

**Count (round 1):** 35 rows (21 P2, 14 P3): Fix 33, Fix-text 2, Dispute 0, Record 0. Five rows carry a partial
departure inside a Fix, each argued in its row: C-P3-1 and C-P3-2 (reviewer facts disputed), and S-P2-4, S-P2-9 and
S-P2-11 (the required correction replaced by another that meets the same invariant).

## Fix round 1: completeness gaps

| Gap | Problem (short) | Disposition | What revision 2 now does | Closure test | Closed at |
|---|---|---|---|---|---|
| G1 | Rows 8, 12, 17 owned by steps that do not build what they test | Fix | Row 8 split: 8a [2.3s] the module, 8b [3.1c] `Force`; row 12's forced legs moved to row 23 [3.1c]; row 17 split: 17a [2.3j] `ssh`, 17b [3.1c] stdio; row 9 uses `force_exit` from 3.0d itself; rows 4 and 18 split; completed by GAP-1 to GAP-3 | Each row's owner's predecessors build it | §5 :580-685 |
| G2 | "2.3r in any free lane" impossible; 2.3 plan review list, totals, 2.3i row untouched | Fix | Wave 2 lane 3: 2.3e then 2.3r; 2.3s then 2.3k in wave 4 lane 1; §8 rows 19 (2.3i red row, evidence) and 20 (dual list, totals 5,250 + 5,600 across 14 steps) | §8 rows 7, 19, 20 name the exact new text | §4 :543-545; §8 :770, :782-783 |
| G3 | Phase 3 parallel steps share `transport.rs` | Fix | Owns column per step; `transport.rs` is 3.0c's alone; 3.0d → 3.0e → 3.0c; `refs.rs` and `repository.rs` via the Phase 5 lane | No file appears in two steps that can run at once | §4 :547-567 |
| G4 | `Force`'s wait set inconsistent (commit, indexing) | Fix | Wait set = every W run and every N step's local write; parked at exit points, inside network calls and in member-lock waits; indexing is inside `download`; commit waited for to its end | Rows 11, 13, 29 | §3.3 :253-266 |
| G5 | R1 attributed to D-V (1) beyond SIGKILL, crash, power loss | Fix | R1 narrowed to D-V (1); SIGQUIT, post-`Force` interrupts, CTRL_CLOSE's OS limit, gwz-py CLI SIGTERM/SIGHUP and the stdio watchdog go to the operator as OD-C7, held as R1b; R1a attributed to (1)'s own rule (GAP-15) | OD-C7 and R1b list every trigger; C4 attributes only D-V (1)'s | C4 :48-54; §3.1 :203-206; §6 :695-702; §7 :729-735 |
| G6 | E6 narrows D-V without a decision | Fix | OD-C8 (a) with step 3.0f and row 25; alternative recorded | OD-C8 states both options and the cost | §3.4 :414-417; §4 :561-567; §6 :703-708 |
| G7 | tokio installs a SIGCHLD handler and reaps the helper | Fix | §2 states it; the helper leaves `tokio::process`; tokio's `process` feature dropped; source and dependency checks | Rows 7, 18a | §2 :69-78; §3.1 :181-189; §3.4 :325-338 |
| G8 | The reaper would SIGKILL the stdio core | Fix | `Stdio` entries write no lifeline record | Row 17b | §3.4 table :344 |
| G9 | Per-call interim registries fork while other calls spawn; fork vs interrupt thread order | Fix | Old entries use reaperless registries (no lifeline); `child_registry()` once per process; gwz-cli forks before the interrupt module's thread | Row 5 in the harness | §3.4 :307-317, :370-379; §3.1 :171-176; §7 R9 |
| G10 | Member-step crossings contradict "all or none" | Fix | Two handler kinds stated | Row 11 | §3.2 :216-222 |
| G11 | Pack removal by age without cross-process exclusion | Fix | Report only, by `mtime` age | Row 13 | §3.5 :440 |
| G12 | Root, `.claude/`, `.git/info/` temps have no recognizer | Fix | Inventoried by fixed target name and reported | Row 14 | §3.5 :448-466 |
| G13 | `Cancel`'s output can panic | Fix | `Output` sink, never panics; print-macro check | Rows 4b, 8b | §3.3 :289-292 |
| G14 | §8 omits the record artifact, its reports and `open_operation` | Fix | §8 rows 12, 13, 18, 21 | Each named clause has amending text | §8 :775-776, :781, :784 |
| G15 | Missing rows: failed fork, retained child, W `TerminateProcess` and console close with `rev-list` and `ssh`; `ProxyCommand` | Fix | Rows 7, 6, 24, 4b; §3.4 on `ssh`'s children and E5 | The rows exist with their legs | §5; §3.4 :345-348, :412-413 |
| I1 | Reaper death undetectable if gwz keeps the read end; pre-exec SIGPIPE | Fix | Parent closes the read end right after the fork; pre-exec ignores SIGPIPE around its write (S-R2-11); gwz's writes block and consume it | Row 5's legs | §3.4 :349-361, :370-391 |
| I2 | Stdio core has no backstop | Fix | Watchdog (see S-P2-4); OD-C7 (e), OD-C8 (b) | Row 17b | §3.6 :485-489 |
| I3 | `Force` tracks only `WriteSequence` guards | Fix | As G4 | Rows 11, 13 | §3.3 :253-266 |
| I4 | Per-call interim registries | Fix | As G9 | Row 5 | §3.4 :307-317 |
| I5 | Row ownership (rows 4, 8, 9, 12, 17, 18) | Fix | As G1 | As G1 | §5 |
| I6 | Wave placement of 2.3r | Fix | As G2 | As G2 | §4 :543-545 |
| I7 | Served push effect | Fix | The client reports every abandoned push call `possible remote effect` | Row 23 | §3.2 :232-238; §3.6 :476-481 |
| I8 | `ssh`'s children | Fix | As G15 | Row 17a | §3.4 :345-348 |
| I9 | `Cancel`'s output panics; envelope interleaving | Fix | `Output` sink; `Force` writes no envelope after a rendering began and says so in the forced notice | Rows 4b, 8b, 21 | §3.3 :289-304 |
| I10 | E6 and E3 narrow D-V | Fix | Both to the operator as OD-C8; listed in §6 and R5 | OD-C8 | §6 :703-708; §7 R5 |
| I11 | Spawn check scope and E4 allow-list | Fix | Allow-list (E2 sites, E4 launch); scope per repository by step | Row 7 | §3.4 :332-338 |
| I12 | Interrupted record read as interrupted while live | Fix | Liveness by the record's own lock, taken before the rename, held until after removal; Windows share mode for every open (S-R2-9) | Row 9's live-run legs | §3.3 :278-288 |
| I13 | Foreign reaping in gwz-py | Fix | `ECHILD` at the waiter unregisters at once; R4 widened; README | Row 6 trace; §8 row 14 | §3.4 :362-364; §7 R4 |
| I14 | Residual after the reaper's death unlisted | Record | R9 | R9 states it | §7 :751-753 |
| I15 | Overbroad all-or-none claim | Fix | As G10 | Row 11's pull leg | §3.2 :216-222 |
| I16 | "Third interrupt" misstated | Fix-text | "An interrupt after `Force` began: the second when the bound began it, else the third" | Text | §3.1 :203-206 |
| I17 | Split doc :247 said to be the test-only hook | Dispute | `grep -n ServerAliveInterval dev-docs/GwzSplitCommandChannel.md` prints 247: :247 is the `--server ssh://` bullet with the argument vector (`ServerAliveInterval=15`, `ControlMaster=no`); :246 is `--server stdio`; :248 the test-only launch hook; the file is unchanged since `bd082c89` | The command's output | §3.4 death table :418-430 |
| I18 | gwz-py second interrupt | Fix | As C-P2-6 | Row 19 | §3.7 :519-528 |
| I19 | Sweep inventory deferred; pack exclusion | Fix | Inventory in the design; report only; `mtime` age | Rows 13, 14 | §3.5 :431-466 |

**Count (fix round 1):** 34 rows (15 G, 19 I): Fix 31, Fix-text 1, Record 1, Dispute 1.

## Fix round 2: completeness gaps, the Safety pass and F1

| ID | Problem (short) | Disposition | What revision 2 now does | Closure test | Closed at |
|---|---|---|---|---|---|
| GAP-1 | Row 13 [3.0e] needs 3.0c's split and report call | Fix | Row 13 owned by 3.0c, which follows 3.0e and builds the split and the fetch-time call; the recognizer is 3.0e's | Row 13's owner's predecessors build the recognizer and the split | §5 row 13 :630; §4 3.0c :554 |
| GAP-2 | Row 7 [2.3r] tests the dependency check 2.3s builds | Fix | The dependency-check leg moves to row 18a [2.3s] (`process` and `signal`); row 7 keeps the spawn check, which 2.3r builds | Row 7 tests only 2.3r's checks | §5 rows 7 :601, 18a :648 |
| GAP-3 | 3.1's `Force` calls `force_exit`, which 3.0d builds in parallel | Fix | 3.1 "After" adds 3.0d; graph `3.0d → { 3.1 ∥ 3.0e }`; §8 row 8 adds it to the plan | No parallel steps on both sides of `force_exit` (plan :322-323) | §4 :555, :561-567; §8 row 8 |
| GAP-4 | §6 did not list the new steps' placement; 2.3s not disclosed | Fix-text | §6 lists 2.3r as D-V (3)'s sub-step and 2.3s, the Phase 3 steps and 3.1 after 3.0d as this revision's choices, with 2.3s's reason (2.3k needs a producer; OD-C1 forbids one in 2.3i) | §6 names every new step and its provenance | §6 :709-726 |
| GAP-5 | After `Force` with a rendering begun, no carrier for `possible remote effect` | Fix | The forced notice on standard error, in every mode, has one `possible remote effect: <member> (<remote>)` line per push past `upload_started`; §3.2 says where the effect appears in each case | Row 23's `--jsonl` and human legs | §3.2 :232-238; §3.3 :293-304; §5 row 23 :666 |
| GAP-6 | D-V (1)'s "long local phases" not inventoried | Fix | Stage gets file-granular exit points (`add_all`'s callback, the conflict loop); the inventory names checkout, copy loops and stage as covered, and `stash push`'s reset and a CLI-route child as having no callback (R10); every other W phase is bounded per member | Row 30 (stage); the inventory names each W phase | §3.3 :267-277; §7 R10 :754-756; §5 row 30 :681 |
| GAP-7 | Per-step dual uneven; no Review column in Phase 3 | Fix | Phase 3 table has a Review column; 3.0f dual (secret handling); 3.0c phase-only because it changes no credential handling, with a diff test on the callback | Each step's tier follows plan :302-306 with a stated reason | §4 :547-572 |
| GAP-8 | Row 18a's "one site" against gwz-core's `sigaction` and `chdir`; gwz-core allowlist unowned | Fix | `signal` kind's occurrences are two allow-listed entries (gwz-cli module; gwz-core children crate) and the crate's `chdir` one `env` entry; 2.3r owns gwz-core's allowlist (kind `env`), 2.3s re-kinds it `signal`; §2 states the one checker and three allowlists | Row 18a | §2 :69-78; §3.1 :181-189; §4 2.3r :537, 2.3s :540; §5 row 18a :648 |
| GAP-9 | `SpawnGate::spawn`/`output` outside the crate; registry vs gate unstated | Fix | The registry takes the gate as `SpawnExclusion`; `SpawnGate::spawn` and `output` removed, their callers (`rev-list`, the CLI route) on the registry; backends hold a registry as they hold the gate; 2.3r owns `spawn_gate.rs`, `backend.rs`, `commit_tag.rs`, `request.rs` | Row 7's spawn check over gwz-core passes with those sites gone; row 31 | §2 :127-131; §3.4 :318-324; §4 2.3r :537 |
| GAP-10 | Legs that pass before their owner not marked | Fix-text | **(reg)** on row 16's `PeerGone` leg, row 20's `sleep` leg, row 6's survival legs; the mark applies to the leg it follows | Every leg passing before its owner is marked | §5 :574-578, rows 6, 16, 20 |
| GAP-11 | 2.3i's new interrupt module has no owner | Fix | 2.3i owns `src/interrupt.rs` and its `mod` line in `src/lib.rs`, a one-line shared file under the 2.3 plan's rule; §8 row 5 amends that rule | §4 2.3i names the file | §4 :538; §8 row 5 :768 |
| GAP-12 | E2 wait vs second interrupt and CTRL_CLOSE | Fix | A second interrupt during an E2 wait restores the dispositions and makes `Force` pending; a third ends gwz by default; CTRL_CLOSE runs `Force` at once | Row 20's second, third and CTRL_CLOSE legs | C5 :55-60; E2 :404-410; row 20 :658 |
| GAP-13 | The Windows CTRL_CLOSE handler needs unnamed global state | Fix | The handler writes the same notification pipe (its write end, the allow-listed global on both platforms) and then sleeps without end; the process ends by `TerminateProcess` or the OS | C6 and §3.1 name one global | C6 :61-66; §3.1 :198-202 |
| GAP-14 | gwz-py design §2.5's "non-network fails" clause still stands | Fix | Every gwz-py operation is cancellable; §8 row 15 replaces :106's clause | Row 19's local-operation leg | §3.7 :505-507; §8 row 15 :778 |
| GAP-15 | Transfer temporaries Force leaves are an unattributed narrowing | Record | R1a: the residue follows from D-V (1)'s own rule (network work ended at once); reported, never removed, named at once | R1a states it with its recognizers | C4 :48-54; §7 R1a :731-734 |
| GAP-16 | Three RemPlan rows departed from the required correction unannounced | Fix-text | S-P2-4, S-P2-9 and S-P2-11 now state the departure and its reason; the count names all five partial departures | Each departing row says so | This plan, rows S-P2-4, S-P2-9, S-P2-11; the round-1 count |
| GAP-17 | Paths not relative to the workspace root as the header claimed | Fix-text | Header path rule (members from the root; other source paths from `gwz-core/`; `tokio/`, `std/`, `cpython/` roots); every path rewritten to it; a scripted check resolved each | The scripted check finds no unresolved existing-file path | header :13-24; every section |
| S-R2-1 | A spawn whose `exec` fails leaves the child registered after std reaps it | Fix | Records carry a token; the registry writes the token's unregistration on every spawn error; the reaper ignores an unknown token; R4 names the window between std's reap (`std/src/sys/process/unix/unix.rs:141-150`) and that write | Row 26: nonexistent and non-executable programs; 300 of them leave the count at zero | §3.4 :349-361; §7 R4 :740-744; §5 row 26 :673 |
| S-R2-2 | SIGCHLD ignored or `SA_NOCLDWAIT` defeats the zombie pin | Fix | The registry reads SIGCHLD's disposition at creation and each spawn and refuses with `ChildReapingDisabled`; gwz-cli's `run` resets an inherited one first; gwz-py's README states it | Rows 27, 8a | §3.1 :171-176; §3.4 :365-369; §8 row 14 |
| S-R2-3 | Unconditional install overrides inherited `SIG_IGN`; restore puts back `SIG_DFL` | Fix | Signals inherited ignored stay ignored; the module saves and restores what it inherited; Windows' inherited CTRL_C disable stated | Rows 8a, 28 | C1 :38-42; §3.1 :177-180, :203-206 |
| S-R2-4 | Interrupts during the E2 wait undefined | Fix | As GAP-12 | Row 20 | E2 :404-410 |
| S-R2-5 | `Stdio` not excluded from drop, close and cancel kills | Fix | The kinds table gains "Killed by"; `Stdio` is on none of the registry's paths; `Cancel` waits only for `Group`, `Process` and `Writer` children | Row 17b's registry-close and cancel leg | §3.3 :241-244; §3.4 :339-344 |
| S-R2-6 | 2.3s's handler swallows Ctrl-C for ordinary commands until 3.1c | Fix | Install scope by step: 2.3s the server family, 3.1 adds `--server` commands, 3.1c every command; a command that does not install keeps the OS default | Row 8a's `gwz fetch` dies-by-default leg **(reg)** | §3.1 :171-176 |
| S-R2-7 | A W run blocked in a member-lock wait never parks; the started server's forced stop unstated | Fix | A member-lock wait is a parked state (`WriteSequence::waiting`); the started server's second stop and `stop --force` call `force_exit` | Row 29 | §3.3 :253-266; §3.6 :490-493; §5 row 29 :679 |
| S-R2-8 | `pidfd` dropped without a disposition | Fix | Restored on Linux for `Process` entries: the reaper opens a pidfd at the registration, while gwz's zombie pin holds the ID, and signals through it; `kill(pid)` where `pidfd_open` fails and on macOS (R4); disclosed as a form of D-V (2)'s `kill()` (§6) | Row 6's pidfd trace leg | §3.4 :382-386; §7 R4; §6 :709-726 |
| S-R2-9 | Windows readers' opens block the owner's rename and removal | Fix | Every open of a record, owner's and reader's, shares read, write and delete; a gone or delete-pending record is an ended run | Row 9's Windows concurrent-reader leg | §3.3 :278-288; §8 row 21 |
| S-R2-10 | C4's "next command" against §3.5's age gate and scope | Fix | C4 states the age gate and the commands; the forced notice names the transfer temporaries at once; §3.5's `objects` row lists both recognizers | Row 13 and the forced-notice text | C4 :48-54; §3.3 :293-304; §3.5 :440 |
| S-R2-11 | `sigwait` in the post-fork pre-exec step | Fix | The step sets SIGPIPE to `SIG_IGN` with `sigaction` around its write and back to `SIG_DFL`; `sigwait` only on ordinary threads in gwz | Row 5's `EPIPE` leg | §3.4 :349-361 |
| F1 | §2 said gwz-core's commit starts no child; gwz-git's Git CLI route runs `git commit` and `git tag` with hooks and signing through gwz-core's spawn gate (`src/git/gitbackend/commit_tag.rs:31`; `gwz-git/src/native_commit/cli.rs:1-5, :38`; `src/git/spawn_gate.rs:449-454`) | Fix | §2 corrected; a `Writer` kind: own group, lifeline, job, never killed by cancel or `Force` (waited for with its W run, R10); R11 its terminal cost; 2.3r moves it onto the registry | Rows 11 (CLI-route member), 31 | §2 :94-99; §3.2 :216-222; §3.4 :343; §7 R10, R11; §5 row 31 :683 |

**Count (fix round 2):** 29 rows (17 GAP, 11 S-R2, F1): Fix 24, Fix-text 4, Record 1, Dispute 0.

## Applied by the lane owner at acceptance

Revision 2 §8 (:760-785) lists twenty-one clause amendments to other documents, each with its new text: TR1.6 §3.2
and OQ2; the split doc §4.3 (two clauses), §4.4 stdio and its header; the 2.3 plan's 2.3i (handler, red row,
evidence, shared files), 2.3j, 2.3k, the waves, the review list and totals, with new 2.3r and 2.3s; the plan's §4
Phase 3 (3.1 after 3.0d, 3.0f dual) and §10; the session design §16, §5.8, §15 and its header; the contract §6 and
§15; gwz-cli's `MachineOutput.md` and root help; gwz-py's README (fork without exec, foreign reaping, ignored SIGCHLD)
and design §2.5 (:106 replaced); parked :584 and :1222; gwz-core's `ErrorCatalog.md` (`cancelled`, a new
`open_operation` row) and `WorkspaceArtifacts.md`. None is applied by this remediation.

## What revision 2 does not resolve

- **OD-C7 and OD-C8 are the operator's.** OD-C7: the exits that can tear a write beyond D-V (1)'s SIGKILL, crash and
  power loss (post-`Force` interrupts, SIGQUIT, CTRL_CLOSE's OS limit, gwz-py CLI SIGTERM/SIGHUP, the stdio watchdog).
  OD-C8: E6 (native-route helpers; recommended: remove with step 3.0f) and E3 (the stdio core's bounded tail;
  recommended: accept). Revision 2 states each with options and a recommendation; it cannot decide them.
- **A native push's upload stays unabortable** (§7 R2): git2-rs's `push_transfer_progress` returns nothing
  (`git2-rs/src/remote_callbacks.rs:86, :528-546`). Lifting it needs a change to the gwz-git2 fork, outside scope.
- **Two long local phases have no callback** (§7 R10): `stash push`'s reset inside libgit2's `stash_save` and a Git
  CLI route child with its hooks. Giving them file-granular points needs a libgit2 stash API with checkout options or
  a hook protocol, outside scope; `Force` waits for them.
- **macOS has no pidfd,** so a `Process` entry's `kill(pid)` keeps R4's reuse window there. A PID loop on the host
  (macOS 26) printed a lower PID after higher ones within one second, so prompt reuse is not ruled out.
- **The Writer's terminal cost** (§7 R11): a hook or signer that reads the terminal itself is stopped in its own
  group. Keeping it in gwz's group instead would let the terminal's interrupt reach `git` mid-write, which C4 and
  §3.2's all-or-none commit forbid. Round 2 may judge the trade.
- **Hostless backends** (`Git2Backend::new`, used in production by `src/operation_context.rs:80, :135`) keep a
  reaperless registry; whether 3.3 can remove them is recorded for 3.3 (R9), not decided here.
- **Python 3.10's path** (§3.7) is from CPython's documented behaviour, not a local read: no 3.10 interpreter is on
  the host. Row 19 runs on 3.10.
- **Length.** Revision 2 is 827 lines against the ~500 target (cap 2,500). Each fix round added closures the gaps
  required: the Owns and Review columns, split rows, the temp inventory, OD-C7 and OD-C8, the path rule, the
  `Writer` kind and seven new rows (26 to 31 and the legs of 8a, 9, 20).
