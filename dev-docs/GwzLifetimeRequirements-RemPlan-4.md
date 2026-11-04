# GwzLifetimeRequirements: remediation plan, round 4

Date: 2026-11-04. Status: **remediation plan for the lane owner; it authorizes nothing beyond revision 6 of the lifetime
requirements document.** Drafted in the lane `/Volumes/projects/limbo/gwz-dev-docs-req`, writing only this file and the
document. No git or gwz operation was run (the blob below is computed from the bytes); nothing was written in
`/Volumes/projects/limbo/gwz-dev`. This round was authorized by the operator on 2026-11-04 and confined to exactly two fixes,
R4-1 and T4-1, on which both reviewers pre-committed to GO, plus the P3s that are one-line local corrections.

- **Object in:** `dev-docs/GwzLifetimeRequirements.md`, revision 5, blob `e9eaa6ab0b3571834e4d3d5549db5550abe9505e` (uncommitted
  in the lane on top of `4e49eff9`; 934 lines). Both round-4 reviews pinned this blob.
- **Reviews (not edited):** [Completeness-4](GwzLifetimeRequirements-ReviewCompleteness-4.md), NO-GO: R4-1 P2, R4-2..R4-4 P3.
  [Testability-4](GwzLifetimeRequirements-ReviewTestability-4.md), NO-GO: T4-1 P2, T4-2..T4-4 P3. Earlier plans:
  [RemPlan](GwzLifetimeRequirements-RemPlan.md), [-2](GwzLifetimeRequirements-RemPlan-2.md),
  [-3](GwzLifetimeRequirements-RemPlan-3.md).
- **Result:** revision 6, edited in place, 939 lines (cap 1,200), status line "DRAFT, revision 6, 2026-11-04 (round-4
  remediation, operator-authorized)". Blob `cbb78318e0bf9c622a1e44a92657b0e3bd96f573`, computed as SHA-1 of `blob <size>\0` plus
  the bytes (the method that reproduces `af4c6079` for revision 4); a later edit changes it.
- **Counts:** 148 requirements (140 MUST, 8 SHOULD, unchanged), 28 non-goals, 35 open questions, 42 failure events, 10 declared
  departures (none added), 42 PROP requirement rows by the scan (LR-BND-3 the known false positive, so 41, unchanged).
- **Dispositions:** 2 of 2 authorized fixes done; 7 one-line P3 corrections done; the remaining items carried (section 3). Disputed 0.

## 1 The two authorized fixes

### Fix 1: R4-1 (P2), the record rule's records were outside LR-REC-12's closed list

Source read: D-V REC (`GwzTransportReleaseConvergencePlan.md:1112-1119`, in this lane) and the Completeness-4 finding.

- **LR-REC-12 (l.434).** The closed list gains, after the family rows: "the record rule's intent records (LR-REC-3: its current
  enumeration and any sequence the shape test adds), reached through LR-REC-2's point". The rest of the row is unchanged, so the
  S row (every deletion or rewrite call site is in a recovery function that is on the list) now passes the recovery that retires
  a checkout's record or a creation sequence's record. The Source cell adds "LR-REC-3's record rule (D-V REC) for the added
  entry". The K row adds "including the recovery of a checkout's record and of a creation sequence's record".
- **LR-OWN-8 (l.309).** The entry "Tier C records (next command)" becomes "the record rule's records left in place by a Force
  stop or a cut (LR-REC-3, LR-REC-5; retired by the next command's recovery, LR-REC-12)". A record that Force leaves (O-forced) is
  now an LR-OWN-8 entry by name, which is what §4's O-forced "except the entries of LR-OWN-8's closed list" needs. `Tier C records`:
  0 hits.
- **Regression (read):** a checkout's record and a workspace-root `clone`'s record are retired by recovery reached through
  LR-REC-2's point, which is a list entry; a Force-stopped checkout's record is a named LR-OWN-8 entry.

### Fix 2: T4-1 (P2), the shape test's comparison domain (the operator's chosen reading)

Source read: Testability-4 T4-1 (a) to (c); A:353-357 and A:372 (R1, R3, R18 as accepted); §10.2 rows R3 and R18 ("harmless").

- **Kept verbatim.** The operator's rule sentence in LR-REC-3 (l.425) now reads exactly as D-V REC has it: "exactly when one of
  its cut states (Force at a safe point, crash, SIGKILL) cannot be told from the user's own edits by its shape". Two glosses that
  had crept into the sentence ("at least", "a crash", "; power loss is a crash, E13") are out of it.
- **Definitions after the rule, labelled.** "**Readings of the rule** (after it, not part of it; (i) is the drafter's, (ii) the
  operator's)":
  - (i) power loss is a crash (E13), so a power-loss cut state is a cut state of the rule (this is also T4-2's gloss, section 2);
  - (ii) the **twin domain**: "the user's own edits" are the states reachable by ordinary user edits of tracked and untracked
    content and by ordinary Git commands, excluding names in gwz's reserved shapes (LR-CLEAN-5) and gwz's record locations; the
    **twin** of a cut state is the member of that domain that matches it in every observable the shape recognizer reads. This is
    the reading the operator chose on 2026-11-04. It is not in D-V's text; the text and the Source cell say so;
  - (iii) a consequence of (ii): a cut state cannot be told from its twin, in the rule's sense, only where the state's required
    handling (recovery, a report, or LR-REC-7's refusal) differs from its twin's. Where the handling is the same whatever made
    the state, no distinction is needed and no record is.
- **A R3 and A R18 named as needing no record.** A R3 (unreachable loose objects after a cut stage; A:357 "none needed (`gc`)")
  and A R18 (a complete pack and `.git/shallow`, refs old, after a cut fetch; A:372 "the next fetch completes"): a user's own
  `git add` or interrupted `git fetch` leaves the same state and the handling is the same. This agrees with §10.2 ("harmless")
  and accepted A R3, and there is no per-fetch durable barrier on R1's path (the cost OQ-32's rejected wider set would have
  carried). A R1 needs a record: its twin, the user's own half-finished edits, must be left alone (LR-REC-4), while the torn
  checkout must be reported and must block an unseen commit (LR-REC-7). The current enumeration ends "a sequence the test finds
  joins it; A R3 and A R18 do not (the twin reading above)".
- **Verify cell, three U rows.**
  - The shape test seeds each enumerated cut state (the power-loss state at its cut point included) and its twin, matched on every
    observable the recognizer reads. Where the handling differs and shape cannot give two answers, a step-point row asserts the
    record is written before the first effect; where the handling is the same (A R3, A R18) it asserts that no record is written.
  - The twin side: the next command neither reports, recovers nor refuses a twin, and `stage` and `commit` do not refuse it
    (LR-REC-4, LR-REC-7); a recognizer that flags every working-tree difference as a torn checkout fails the row.
  - Regression: the test yields "no record" for A R3 and A R18 and "record" for A R1, and a recognizer that flags the A R1 twin fails.
- **Traceability.** The Source cell and the §10.1 row for the decision record that the twin reading is the operator's of
  2026-11-04, from this plan. It is not added to D-V's text.

## 2 One-line P3 corrections made

| Item | Sev | Change (revision 6 line) |
|---|---|---|
| T4-2 gloss | P3 | "power loss is a crash, E13" moved out of the rule's sentence into reading (i), marked as the drafter's (l.425). The shape test's Verify row now covers the power-loss state at its cut point. LR-REC-1's own enumeration clause is not changed (section 3) |
| R4-4 D-V key | P3 | "Plan:1030-1110" becomes "Plan:1030-1121" (the block runs to the "Open:" line at :1121; REC is :1112-1119) (l.42) |
| R4-4 LR-REC-7 | P3 | "(a checkout does)" becomes "(checkout is in the current enumeration)" (l.429) |
| R4-4 §9.3 | P3 | R-D1..R-D3 "temporaries named in intents" becomes "a temporary is named in a record only where LR-REC-3's record rule keeps one; elsewhere it disappears on the condition of the next row, shape recognition plus proof of ownership, LR-CLEAN-5" |
| R4-4 §8 split | P3 | "LR-PROC-1, -10, -12, -14 | U P S" split into LR-PROC-1 (M U S, L U S, W U P S; "W: job breakaway (pending OQ-14)"), LR-PROC-10 and -14 (U P everywhere) and LR-PROC-12 (U), matching each row's Verify codes |
| R4-4 §8 LR-REC-3 note | P3 | the note now names the shape test with its twin side |
| R4-4 / D-V traceability | P3 | §10.1's decision row says the twin-domain reading is the operator's of 2026-11-04 (this plan) |

## 3 Items carried, not fixed this round

The authorization was confined to R4-1 and T4-1, so everything not a one-line local correction is carried to the lane owner's
decision.

| Item | Sev | What is carried |
|---|---|---|
| R4-2 | P3 | The enumeration counts the existing contracts' records as the rule's records, but the rule says "cleared when it finishes" and "exactly when", while the I2 record contract keeps a merge record past completion (:340, :465). Suggested: one sentence that an existing contract's record keeps its own trigger and lifetime, the rule decides only which sequences must keep an intent record, and a passing shape test never withdraws an accepted record; or a DEP row and OQ. Not applied here because it changes what the enumeration means beside the new twin clause, and the lane owner should decide |
| R4-3 (a) | P3 | E20's gwz-owned writes are O-clean ("nothing is left behind") yet LR-REC-5 requires a safe-point report and next-command recognition. Suggested: an O-clean exception for a multi-file sequence stopped by a write failure at a safe point, or assign it the O-forced column |
| R4-3 (b) | P3 | The widened Tier C column's three process-level rows ("no code runs", "none from this process", "the OS's") are false for a non-gwz write failure where gwz runs on. Suggested: scope them "for a death of gwz" |
| T4-2 (rest) | P3 | LR-REC-1's own clause "enumerated exhaustively from step points" does not mention the power-loss states of LR-TEST-6's model. Only the shape test's row was extended this round |
| T4-3 | P3 | LR-PROC-14's "a child that does not need it" has no per-class environment rule for the contested classes (K `git fetch` fallback, W `git commit` hooks and signer, F `forall`). Suggested: each class declares an allow-list or scrub list at its spawn site, and an S row checks it; or put the change from 1.0.18 to the operator |
| T4-4 | P3 | (a) no row seeds a live checkout record then a user edit before the next command (LR-REC-4 Verify); (b) LR-TEST-10's O row lacks breakaway semantics (`BREAKAWAY_OK`, `SILENT_BREAKAWAY_OK`, a nested non-breakaway job); (c) LR-TEST-7's census wording reads as a tautology |
| Completeness risk | - | Sweep every other closed list and "refused" clause against LR-REC-3: LR-OWN-2, LR-PROC-1 (the OQ-14 reading). LR-REC-12 and LR-OWN-8 are done; LR-OWN-2 and the LR-PROC-1 undeclared-spawn refusal were not re-read this round |
| Both reviews | - | Unspiked (ext) facts in gating cells: the PEB live block, `KERN_PROCARGS2` and `/proc` reads, kill-on-close at the last handle, `IsProcessInJob`, breakaway. Do not freeze the matrix before the LR-TEST-10 spike |
| Citations | - | Root P23:154 and SCC:230 (`8aa5f800`) and the lane-only Plan:1112-1119 and :1121 (commit `4e49eff9`): re-check at merge |

A correction to the previous plan: RemPlan-3 section 1 said the operator named checkout "only as an example ('e.g.')". D-V REC at
Plan:1112-1119 has no "e.g." (Completeness-4 risk). The document never repeated it; do not carry the claim forward. Checkout is in
the enumeration as the drafter's reading, to be confirmed by the shape test.

## 4 Mechanical checks and results

Python over `GwzLifetimeRequirements.md` (939 lines, blob `cbb78318`); the script lives in the session scratchpad, which is wiped on
restart, so each check is restated. Numbering follows RemPlan-3 section 3.

| # | Check | Result |
|---|---|---|
| 1 | Every `LR-`, `NG-`, `OQ-`, `DEP-` and `E` ID referenced is defined (148 LR, 28 NG, 35 OQ, 10 DEP, 42 E including E17b and E27b) | pass: none undefined |
| 2 | Every NG appears in §10 | pass |
| 3 | Every NG is referenced outside §6 | pass |
| 4 | Every OQ is referenced outside §11 | pass |
| 5 | Every non-TEST requirement appears in a §8 row, ranges expanded (the LR-PROC-1 split included) | pass: none missing |
| 6 | Every §8 ID is defined | pass |
| 8 | PROP scan (requirement rows) equals OQ-16's list | pass: 42 rows, LR-BND-3 the known false positive, 41 of 41 listed, 0 extra. The OQ-16 list is unchanged because LR-REC-12's closed list was already PROP and LR-REC-3 already partly PROP |
| 11 | Table integrity: every row has its header's cell count (the three new §8 rows included) | pass: 0 problems |
| 13 | `M-B9`, `Clocks keep running`, `from the intent`, `reported at the stop only`, `trigger set`, `gwz thread`, `Tier C records`: absent. `told from the user`: exactly 1 (LR-REC-3). `intent record`: 2 hits, one in LR-REC-3 (the rule's own words) and one in LR-REC-12 (the closed-list entry Fix 1 requires, "the record rule's intent records"); no third | pass (the expected count is now 2, by design) |
| 14 | §9.3's range reads `NG-1 to NG-28`, the defined maximum | pass |
| 16 | The rule is stated once and cited: LR-REC-1, -5, -7, -13 and LR-OWN-3 each cite the rule or LR-REC-3; the rule's own words occur once | pass |
| 17 | `relax` occurs only in OQ-3, LR-REC-3 ("No named state is relaxed") and the closed OQ-32 row | pass: lines 425, 907, 936 |
| 18 | Status line carries "DRAFT, revision 6, 2026-11-04 (round-4 remediation, operator-authorized)" | pass |
| 20 | Size against the 1,200 cap | pass: 939 lines |
| 21 | New: `power loss is a crash` absent from the rule's sentence (now in reading (i)); `twin` appears at the status paragraph, LR-REC-3, the §8 note and the §10.1 row | pass |

Not run: every P, K, F, U row (the document authorizes no code), including the new twin-side and regression U rows. Checks 7, 9, 10,
12, 15 and 19 of earlier plans were not re-run in full: none of the edited rows adds a Verify code outside §0's set (the new rows
use U only), a bound, or a lock-file name.

## 5 For the lane owner

- **Next review:** pin blob `cbb78318` and read the document, not this plan. Most likely to draw a finding: (a) the twin domain is
  recorded as the operator's chosen reading but it is not in D-V's text, so the lane owner may want to add it to the plan's D-V
  bullet or to an OQ so the source is a document and not this plan; (b) R4-2, which the twin clause sits beside; (c) the §8
  split of LR-PROC-10, -12 and -14, which follows each row's Verify codes and not the earlier merged row.
- **State:** the document and this plan are uncommitted in the lane beside the round-4 reviews. No review copy, build folder or
  cargo build was made this round, so there is nothing to free under `build-targets`.
