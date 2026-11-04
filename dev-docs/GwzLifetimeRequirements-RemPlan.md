# GwzLifetimeRequirements: remediation plan, round 1

Date: 2026-11-03. Status: **remediation plan for the lane owner; it authorizes nothing beyond revision 3 of the lifetime
requirements document.** Drafted in the lane `/Volumes/projects/limbo/gwz-dev-docs-req`, writing only this file and the
document; no git or gwz operation that changes anything was run (only read-only `git log` and `git status`; the lane owner commits). Nothing was written in `/Volumes/projects/limbo/gwz-dev`;
the root P23 was read for one citation (below).

- **Object:** `dev-docs/GwzLifetimeRequirements.md`, revision 2, blob `34680949` (lane commit `a874eddc`), the blob both
  round-1 reviews pinned.
- **Reviews (not edited):** [Completeness](GwzLifetimeRequirements-ReviewCompleteness.md), NO-GO, 9 P2 and 6 P3 (`C-1`..`C-15`);
  [Testability](GwzLifetimeRequirements-ReviewTestability.md), NO-GO, 15 P2 and 8 P3 (`T-1`..`T-23`). 38 findings; no P0 or P1.
- **Result:** revision 3, edited in place, 912 lines (target 1,200, hard cap 2,500), status line "DRAFT, revision 3,
  2026-11-03 (round-1 remediation)", blob `ef534435` at the time of writing (a later edit by the lane owner changes it).
  Counts: 148 requirements (140 MUST, 8 SHOULD; revision 2 had 141: 134 and 7), 28 non-goals (was 25), 34 open questions
  (was 29), 42 failure events (was 37), 7 declared departures (new).
- **Dispositions:** fixed in text 38; of those, 10 (C-1, C-3, C-4, C-6, C-7, T-5, T-6, T-9, T-14, T-21) also raise or extend an open question; disputed 0. Five citations in the
  reviews were off by a line and are corrected (§5); none changes a finding.

## 1 Operator input applied

The workflow request for this round reads "go with (a), canonical path". It answers the 2.3f round-2 Code P2-1 question
that the root plan records as the operator's decision of 2026-11-03: `ws-<key>` stays the first 16 hex digits of the SHA-256
of the canonical workspace root, and filesystem identity keys only the member lock and a per-directory start lock (root
`GwzServerCore-Step2.3-Plan.md:154`, commit `8aa5f800`; this lane's copy of P23 predates it). It resolves C-3 (d) and
LR-OWN-9: the server address is canonical-path keyed, identity keys the member lock and the one-server-per-directory
refusal, and a grant never follows an inode. The document cites it as an operator decision (source key l.44, l.299, DEP-4 l.93,
§10.1 l.788). No other operator decision was invented: every item below that needs one is an open question with options
and a recommendation (§11 now has a Recommendation column for all 34 rows).

## 2 Disposition table

"Where" gives revision-3 line numbers. Every row is FIXED unless it says otherwise; "+OQ" means the fix also raises or
extends an open question the operator decides.

| ID | Sev | Finding | Disposition and where | Check | Result |
|---|---|---|---|---|---|
| C-1 | P2 | LR-PROC-7 and OQ-27 misstate B; "M-B9" does not exist | FIXED +OQ. LR-PROC-7 (l.379): jobs for K and W only, S in none (B:128-129, :326 B21), W child ended by the job (B:343); "M-B9" deleted; LR-PROC-2 (l.374) W clause; E09 W row (l.200), §4 W row (l.263), L1 row (l.346), LR-OWN-8; OQ-2 (l.880) now Unix only; OQ-27 (l.905) rewritten to W only, confirming B:343 | No "M-B9"; LR-PROC-7, LR-PROC-2, OQ-2, OQ-27 agree with B:129, B21, B:343 (quotes re-read at the cited lines) | pass |
| C-2 | P2 | LR-BND-2 contradicts CT and TR1.6; omits D42/D45 | FIXED. LR-BND-2 (l.445) rewritten: only four authorized charging transitions (setup interaction pause; D19 helper-Basic; connect-clock local-wait pause and no stall charge for backpressure; producer-busy), agent confirmation stays in Network; B-PRODUCER 600 s (l.476); cites CT:683, :704-713 | LR-BND-2 names CT:707-708's two transitions and TR18 D42/D45; `grep "Clocks keep running"` = 0 | pass |
| C-3 | P2 | No register of departures from accepted documents | FIXED +OQ. New §0 register, DEP-1..DEP-7 (l.88-96). (a) DEP-1 -> OQ-30 (l.908); (b) DEP-2 yields, LR-EMB-3 (l.521); (c) DEP-3 -> OQ-5 (l.883), LR-EXIT-9/-10 PROP; (d) DEP-4 resolved by the operator decision of §1; (e) DEP-5 -> OQ-31 (l.909), LR-EXIT-8 PROP; (f) DEP-6 -> NG-26; (g) DEP-7 -> LR-OWN-2 (d) | Items (a)-(g) each appear in the table with an authority or an OQ | pass |
| C-4 | P2 | LR-OWN-2 allow-list contradicts SD §5.7; S check infeasible | FIXED +OQ. LR-OWN-2 (l.292) gains (d), SD's `permanent` class (SD:89, :345-370), and states the check as "fails on any global not in its allowlist; every `debt` entry it passes is in OQ-22"; `Hub` per CP:270; OQ-22 (l.900) inventories the 19 `debt` entries of the six allowlists | Script over the six `process_globals_allowlist*.json`: 19 `debt` entries, all named in OQ-22 (`Cred::credential_helper` matched by name); 24 `permanent` entries fall under (d) | pass |
| C-5 | P2 | SD's hung admission thread missing from failure model and LR-BND-1/OWN-8 | FIXED. E36 (l.239), NG-26 (l.561), LR-OWN-8 entry (l.298), LR-BND-1 list (l.444), R-THR, DEP-6 | `grep`: the admission thread appears in §3, in §6 and in LR-OWN-8 | pass |
| C-6 | P2 | "No polling" has no operator source; IL decision 4 unmapped | FIXED +OQ. LR-LIVE-1 (l.359): clause PROP, CT:335-340 replaced by Plan:1039 and IL decision 4; NG-27 (l.562) records the 2 ms HTTPS step and SSH bounded park as interim; §10.1 IL decision 4 row (l.782); §9.1 P5; OQ-17 (l.895) extended | §10.1 contains "IL decision 4"; each LR-LIVE-1 source clause supports its text | pass |
| C-7 | P2 | Untraceable "durable intent" mandate is §9's design | FIXED +OQ. Clause removed from LR-OWN-3 (l.293) and phase L3 (l.148); LR-REC-3 (l.414) holds the one trigger set (creation sequences, existing contracts' records); OQ-32 (l.910) asks whether more sequences must use the record route; §9 P3 cites it | The intent clause traces to a source or to OQ-32; LR-REC-1 still allows record or shape | pass |
| C-8 | P2 | Clauses resting on C/D/AM/review not marked PROP | FIXED. PROP marks added: LR-EXIT-8 ("any step"), LR-EXIT-9 (safe-point clause), LR-EXIT-10 (never kills), LR-PROC-2 (S clause), LR-LIVE-1 (no polling); OQ-16 (l.894) now lists all 39 PROP rows by ID | Script C6: PROP rows missing from OQ-16 = none; OQ-16 IDs that are not PROP = none; provenance scan: every non-PROP row has an accepted-source marker (LR-PROC-11, LR-CONC-8 cite existing accepted designs) | pass |
| C-9 | P2 | Served reporting infeasible; E18 core side tier | FIXED. LR-OBS-10 (l.500) new; LR-OBS-1 (l.491), §4 Report row (l.272); E05, E17b, E18-core qualified with NG-23 (l.194, l.212, l.214); event assignment (l.275) | §4 Report row and LR-OBS-1 achievable for E02 under `--server` through LR-OBS-10; E18 row cites NG-23 | pass |
| C-10 | P3 | SD-F8 mis-mapped | FIXED. Source key (l.56) and §10.1 row (l.796) say capability bit (SD:548); LR-CAN-13 (l.322, SHOULD, PROP for the notice) | §10.1 row matches SD:548 | pass |
| C-11 | P3 | Bridge pump, eviction and three failure events missing | FIXED. PMP actor (l.125); R-SESS eviction and result view (l.163); E37 uninterruptible sleep, E38 limit exhaustion, E39 directory removed or unmounted, E40 pump death (l.240-243) with tiers and requirements (LR-PROC-15, LR-PROC-10, LR-REC-6, LR-OWN-14) | §1, §2 and §3 list them | pass |
| C-12 | P3 | TR18 D24/D42/D45/D47/D52 and SBC Q5 unmapped | FIXED. §10.1 rows (l.775-779, l.784); R-GATE (l.175); LR-OBS-9 (l.499); LR-TEST-8 (l.576); NG-16 (l.551) | Every TR18 decision about spawn, inheritance, clocks or close is in §10.1: D5, D14, D18, D24, D25, D26, D31, D32, D42, D44, D45, D47, D52 (grep 13 of 13). The remaining decisions (D1-D4, D6-D13, D15-D17, D19-D23, D27-D30, D33-D41, D43, D46, D48-D51) concern proxies, trust, words, builds and evidence hosts; D19's helper-Basic pause is cited through CT:707-708 | pass |
| C-13 | P3 | NG-9, NG-25, OQ-14 unreferenced; OQ-16 has no list | FIXED. §10.1 rows for H A1 residuals (NG-9) and the no-decision items (NG-25, NG-28), l.790-791; OQ-14 cited in NG-2 (l.537); OQ-15 in LR-BND-8; OQ-17/OQ-18 in LR-LIVE-1 and LR-OWN-8; OQ-16 enumerates | Script C2, C3, C3b: no NG missing from §10; no OQ or NG unreferenced outside its own section | pass |
| C-14 | P3 | Closed lists omit the cover process; `gwz hook` spawns have no class | FIXED. LR-OWN-8 and R-PROC (l.161) list the cover after a Tier C death (B:219-221); LR-PROC-1 (l.373) exempts `gwz hook`/`claude-code-setup` spawns until OQ-13 (B:361, R-B9) | Allowlisted spawn sites map to a class or the exemption: gwz-core 3 (K, K, NG-15), gwz-cli 10 (`forall` and pager F; hook utilities and fallback exempt; `help` is not a spawn), gwz-git 1 (W). A full census is LR-PROC-1's own S check | pass |
| C-15 | P3 | Acceptance row R8 omitted | FIXED. §10.1 R7, R8, R9 rows (l.792-794); LR-TEST-11 (l.579) adds the two-machine run per direction for L5, E17b, LR-CAN-9, LR-EXIT-9 (remote); LR-TEST-13 | R8 appears in §10.1 and LR-TEST-11 | pass |
| T-1 | P2 | LR-LIVE-1 cannot hold for L6, L9, L2/W | FIXED. LR-LIVE-1 (l.359) limited to cross-process links L1-L5, L7, L8, L10; LR-LIVE-9 (l.367) new for L6 (owned join or channel handle) and L9 (atexit); L2 W cell "no instance" (l.347); §5.4 intro | Every remaining LR-LIVE-1 cell is killable by a P test (L1-L5 by process kill, L7 by peer close, L8 by helper kill, L10 by pty hang-up) | pass |
| T-2 | P2 | "Within B-COVER" has no timer and needs a sleep | FIXED. LR-PROC-2 (l.374) event-ordered, verified by waiting on each K child's pinned exit under a hang guard; LR-PROC-16 (l.388, SHOULD, E) holds the latency; B-COVER moved to measured figures (l.485); LR-BND-3 (l.446); E09, §4 | Script C5: no MUST verify cell cites B-COVER or "within B-x after" | pass |
| T-3 | P2 | File-event crash cuts for libgit2/Git are not deterministic | FIXED. LR-REC-1 (l.412) verifies libgit2/Git writes by seeded state; LR-TEST-6 (l.574) reclassifies file-event kills as Z supplement; matrix note (l.641) | Every libgit2/Git sequence has a seeded-state row per enumerated state; `grep` shows "file event" only in Z-supplement wording | pass |
| T-4 | P2 | LR-PROC-5 conflicts with ssh dispositions | FIXED. LR-PROC-5 (l.377) excepts class-declared dispositions (Unix `ssh`: SIGINT and SIGQUIT ignored); verify reads dispositions per class | LR-PROC-5 and LR-PROC-9 give no contradictory disposition for one spawn | pass |
| T-5 | P2 | E06 tiered O-clean but ssh gets the hang-up | FIXED +OQ. E06 (l.195) retiered for the `ssh` carrier as E05 (pending OQ-24), citing OpenSSH as PF ext; E08 (l.197) adds ssh SIGTSTP; OQ-24 (l.902) bears on E06, E08, option (b) extended; LR-PROC-9 (l.381) gains the pty hang-up row over real `ssh`; LR-TEST-10 O row for OpenSSH signals | P (pty) row hangs up during a served fetch over real `ssh` (LR-PROC-9, LR-TEST-13) | pass |
| T-6 | P2 | Silent idle loss has no OS event | FIXED +OQ. E17 (l.211) and LR-LIVE-2 (l.360) narrowed to loss the peer signals; NG-28 (l.563) states the silent case (typed `Stalled`, no replay, exposure B-IDLE); LR-LIVE-7 (l.365); F row with a silently dropping network port; OQ-33 (l.911) | The F cell asserts NG-28's outcome (LR-LIVE-2 verify, matrix l.625) | pass |
| T-7 | P2 | One B-CLEAN contradicts close_wait and stop bound | FIXED. LR-CLEAN-2 (l.395): one named bound per owner kind (host B-CLEAN, session B-CLOSEW then detach, server shutdown, carrier, registry); §4 exit property: server forms within B-STOP, U-waits excluded (l.261). SRV:586-598 step 4 uses "the close bound", so the server's host-context step is not B-CLEAN | Every owner kind of §0 Terms maps to one bound | pass |
| T-8 | P2 | LR-OWN-11 forbids concurrent connections | FIXED. LR-OWN-11 (l.301): creations at most min(CAP, concurrent demand), peak at most CAP, later phases create none, replacements excepted; oracle is R1's pass rule (Plan:280) | The U oracle equals R1's pass rule, quoted | pass |
| T-9 | P2 | Wrong Windows durability fact | FIXED +OQ. LR-REC-3 (l.414) states W's write-through create and rename; LR-REC-11 (l.422) drops W; NG-3 (l.538); OQ-7 (l.885) rewritten; LR-TEST-10 O row; U row with a fake filesystem recording the barrier per platform; matrix (l.643, l.647) | Code re-read: `durable_fs.rs:17`, `filesystem/native.rs:134-145`, `native/handles.rs:92`, `native/publication.rs:29`, `residue.rs:591-595` all say write-through; LR-REC-11's U row on W/NTFS emits no warning | pass |
| T-10 | P2 | LR-BND-7 claims more than a monotonic clock gives | FIXED. LR-BND-7 (l.450) restated as CT:670-672 states it; B-SUSPEND (l.477, the idle interval, `connection_host.rs:11-13, :45-47`); E28 (l.231); matrix F U O (l.654); LR-TEST-10 O row | Bounds table names the threshold (script C9) | pass |
| T-11 | P2 | R7 P cell, R8, ssh route missing | FIXED. LR-OWN-6 (l.296) and matrix (l.595) add the R7 P row (Plan:286); §10.1 R7-R9; LR-CAN-1 (l.310) adds `ssh`; LR-TEST-13 (l.581) | §10.1 lists R7, R8, R9; every `ssh`-form MUST has a real-ssh or O-backed cell (LR-TEST-13) | pass |
| T-12 | P2 | No row tests the shipped binary | FIXED. Code I (§0); LR-TEST-14 (l.582); LR-TEST-4 (l.572) restated (gwz's own roles re-exec, external programs real); every LR-EXIT matrix row gains I | Script: LR-EXIT-1..11 each have an I cell on every platform that applies | pass |
| T-13 | P2 | LR-CRED-3 memory check infeasible on macOS | FIXED. LR-CRED-3 (l.432): ordering by U, environments by `KERN_PROCARGS2`/`/proc`/PEB, memory scan L and W only or a self-scan in the cover role; matrix split (l.649) | Every M cell names an unprivileged mechanism | pass |
| T-14 | P2 | Intent trigger sets inconsistent; §9 widens | FIXED +OQ. One trigger set in LR-REC-3 (l.414); §1 L3, LR-OWN-3 and §9 P3 cite it; OQ-32 (l.910) carries the wider options with the E-row cost | `grep multi-file`: lines 191 (E02), 220 (E20), 910 (OQ-32); no other trigger definition | pass |
| T-15 | P2 | LR-CAN-12 demoted to SHOULD | FIXED. LR-CAN-12 (l.321) MUST with B-CANCEL fixed from the Phase 0 baseline (Plan:282, :178-179); B-CANCEL (l.485, OQ-28); R5's numeric column added to LR-CAN-9's verify (l.318) | R1-R7 numeric columns: R1 creations -> LR-OWN-11; R2 -> LR-EMB-7; R3 -> LR-CAN-12; R5 -> LR-CAN-9 E row; R6 -> LR-CLEAN-11; R7 count -> LR-OWN-6. R1/R2 warm-time columns are speed rows, now said in §10.1 | pass |
| T-16 | P3 | Verify cells contradict LR-TEST-2/-5/-3 | FIXED. LR-LIVE-5 (l.363) wake counter; LR-TEST-5 (l.573) admits F; LR-TEST-3 (l.571) and code E cite Plan:289ff | Read | pass |
| T-17 | P3 | Some S checks cannot verify | FIXED. LR-OWN-3 order by U recording port (§0 code S too); LR-REC-12 (l.423) defines "recovery entry" by marker; LR-CRED-2 (l.431) adds the secret-types S check | Read | pass |
| T-18 | P3 | Windows analogues n/a or missing | FIXED. LR-PROC-5 W clause and matrix W = P (l.630); LR-CONC-7 (l.512) adds `GenerateConsoleCtrlEvent` to group 0 | Read | pass |
| T-19 | P3 | Census marker can be scrubbed | FIXED. LR-TEST-7 (l.575): session or process group (Unix), job (W); an env tag only on the test-build scrub allow-list | Read | pass |
| T-20 | P3 | LR-LIVE-4 reads as forbidding NG-7 | FIXED. LR-LIVE-4 (l.362) says "the cover's own fork", NG-7 for the application's; M CLOEXEC cell pending OQ-12 (l.621) | Read | pass |
| T-21 | P3 | LR-OWN-9 matrix code; Linux floor | FIXED +OQ. Matrix LR-OWN-9 is U P O (l.597); kernel floor and `SO_PEERPIDFD` fallback to OQ-34 (l.912) with LR-TEST-11 pointing at it | Read | pass |
| T-22 | P3 | One LR-OBS-7 remedy cannot fit cmd and PowerShell | FIXED. LR-OBS-7 (l.497): one remedy per default shell or a `gwz` subcommand remedy; U oracle per named shell | Read | pass |
| T-23 | P3 | §9.3 R-B15 and macOS R-B3 "disappear" presumes OQ-12 | FIXED. Moved to "Disappear if the operator chooses" with NG-16 and application `popen` noted | Read | pass |

## 3 New and changed open questions

Each row carries options and a recommendation (new Recommendation column, l.874-912). The recommendations are the
drafter's, grounded in the cited source; none is an operator decision.

| OQ | Origin | Question in one line | Recommendation |
|---|---|---|---|
| OQ-30 (new) | C-3 (a) | First interrupt in the served form: B-CLEAN or SCC:242's close bound | (a) B-CLEAN, then Force |
| OQ-31 (new) | C-3 (e), C-8 | Does a server Force at any step end every wait, or only step 3 | (a) any step (decision (i)) |
| OQ-32 (new) | C-7, T-14 | Which sequences must use the durable record route | (a) creation sequences and existing contracts' records only |
| OQ-33 (new) | T-6 | Accept silent idle loss as NG-28 | (a) accept |
| OQ-34 (new) | T-21 | Linux kernel floor of the suites | (a) 5.3 floor with tested fallback below 6.5 |
| OQ-2 | C-1 | Narrowed to Unix | (b) SIGTERM then SIGCONT (B and D) |
| OQ-5 | C-3 (c) | Adds: client kill of a stdio core at B-STDIO (SRV:1116) | (a) never kill |
| OQ-7 | T-9 | Rewritten around W write-through | accept W as full if the O row holds |
| OQ-16 | C-8, C-13 | Lists the 39 PROP IDs | confirm as a set |
| OQ-17 | C-6 | Adds NG-27's two interim polls | confirm |
| OQ-22 | C-4 | Adds (d) and the 19-entry `debt` inventory | accept (b), (c), (d); keep debt with owners |
| OQ-24 | T-5 | Bears on E06, E08; option (b) extended | R-C4 (a), R-C2 (a) |
| OQ-27 | C-1 | Rewritten: confirm B:343 for W | (a) confirm |
| OQ-28 | T-2, T-15 | Timers (B-SETTLE, B-FACTORY, B-EOF) vs measured figures (B-CANCEL, B-COVER, B-RSS) | fix from an E baseline |

## 4 IDs added

LR-CAN-13 (SHOULD), LR-LIVE-9, LR-PROC-15, LR-PROC-16 (SHOULD), LR-OBS-10, LR-TEST-13, LR-TEST-14; E36-E40; NG-26-NG-28;
DEP-1-DEP-7; bounds B-PRODUCER, B-SUSPEND, B-CANCEL; verification code I; actor PMP; OQ-30-OQ-34. LR-CAN-12 changed from
SHOULD to MUST. New PROP rows: LR-CAN-13, LR-LIVE-9, LR-PROC-15, LR-PROC-16, LR-TEST-13, LR-TEST-14, and the five clauses
marked in C-8; all are in OQ-16.

## 5 Disputed findings and citation corrections

Disputed: none. Every finding was accepted. Five citations in the reviews were off and are corrected in the document;
the substance of each finding stands.

- C-3 (e) cites SRV:590 for "a second signal or `stop --force` ends only step 3"; step 3 is SRV:589, and SCC:239 states
  it. The document cites SRV:586-598 and SCC:239 (DEP-5).
- T-11 cites Plan:285 for R7's pass rule; it is Plan:286 (Plan:285 is R6). Used: Plan:286.
- C-4 cites CP:269 for the `Hub` decision; it is CP:270 (CP:269 is the connect-clock pause). Used: CP:270.
- T-4 cites B:313 for `Process { ignore_interrupts }`; B:313 is test row B8. The mechanism is at B:107 and :146. Used: B:146.
- T-16 cites Plan:288ff for the rounds-and-pairing method; it starts at Plan:289. Used: Plan:289ff.

## 6 Mechanical checks and results

Run over `GwzLifetimeRequirements.md` (912 lines, blob `ef534435`) with a Python script that parses the tables (kept in the
session scratchpad, which is wiped on restart; each check is restated here so it can be re-run).

| # | Check (review "Check" line it implements) | Result |
|---|---|---|
| 1 | Every `LR-`, `OQ-`, `NG-` and `E` ID referenced is defined (C-13, T-14 format). Defined: 148 LR, 34 OQ, 28 NG, 42 E | pass: none undefined |
| 2 | Every NG appears in §10 (C-13). Revision 2 failed on NG-9, NG-25 | pass: none missing |
| 3 | Every OQ is referenced outside §11, every NG outside §6 (C-13). Revision 2 failed on OQ-14, -15, -17, -18 | pass: none unreferenced |
| 4 | Every non-TEST requirement appears in a §8 row, ranges and shorthand expanded; no §8 ID is undefined | pass: none missing |
| 5 | No MUST verify cell cites a bound that no timer owns, or "within B-x after" (T-2). Revision 2 failed on LR-PROC-2 | pass: none |
| 6 | Every PROP row is listed in OQ-16 and OQ-16 lists only PROP rows (C-8, C-13) | pass: 39 of 39 |
| 7 | "M-B9" absent (C-1) | pass |
| 8 | One intent-trigger definition: `multi-file` occurs at l.191, l.220, l.910 only (T-14) | pass |
| 9 | Every B-bound used is in the bounds table (T-10) | pass |
| 10 | Every verification code in §8 is defined in §0 (new code I included) | pass |
| 11 | Table integrity: every row of every table has its header's cell count | pass: 0 problems |
| 12 | Every LR-EXIT MUST has an I cell on each applicable platform (T-12) | pass |
| 13 | Every OQ row has a non-empty recommendation | pass: 34 of 34 |
| 14 | Quotes and citations re-read at the cited lines (±3 lines for wrapped text): 50 checks across Plan, SCC, SD, SRV, B, CT, CP, IL, TR18, SBC, TR1.6 and five code files | pass: 0 misses |
| 15 | OQ-22 inventory against the six allowlists | pass: 19 `debt` entries, all named |
| 16 | `grep "Clocks keep running"` (C-2) | pass: 0 |
| 17 | Provenance scan: non-PROP rows whose Source cell has no accepted-source marker | LR-PROC-11, LR-CONC-8 (existing accepted designs); LR-EXIT-6 is a parser artifact (its cell contains `\|`; it carries PROP) |
| 18 | Size: 912 lines against the 1,200 target and the 2,500 cap | pass |

Manual checks, with no script: "every remaining LR-LIVE-1 cell can be killed by a P test" (T-1), "no two rows give one spawn
contradictory dispositions" (T-4), "every M cell names an unprivileged mechanism" (T-13), "R1-R7 numeric columns map to
a MUST" (T-15), "every owner kind maps to one bound" (T-7). Each is argued in the table above.

## 7 For the lane owner

- The round-1 reviews were pinned to blob `34680949`; the next review must pin the new blob and read the document, not
  this plan. Both reviews' pre-commitments (resolve C-1..C-9 and T-1..T-15 with no new P0-P2) are met by the rows above.
- Revision 3 adds recommendations for all 34 open questions, which the operator can accept in one pass; OQ-16 asks for
  the 39 PROP rows as a set.
- The document cites the root P23:154 (commit `8aa5f800`) for the operator decision; this lane's P23 copy does not have
  that paragraph yet. The citation resolves once the lane merges past that commit.
- Out-of-scope observations (not acted on): the allowlists record ID counters (`NEXT_WORKER`, `SERIAL`, `NEXT_POOL`) as
  `debt` while SD §5.7 says they stay `permanent`; the reviewers' line cites drift as plan files move (§5). Both are in
  OQ-22 or the citation list and need no separate follow-up.
