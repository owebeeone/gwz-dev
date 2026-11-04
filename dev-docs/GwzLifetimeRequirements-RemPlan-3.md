# GwzLifetimeRequirements: remediation plan, round 3

Date: 2026-11-04. Status: **remediation plan for the lane owner; it authorizes nothing beyond revision 5 of the lifetime
requirements document.** Drafted in the lane `/Volumes/projects/limbo/gwz-dev-docs-req`, writing only this file and the
document. No git or gwz operation was run; nothing was written in `/Volumes/projects/limbo/gwz-dev`. This round was permitted
only for non-architectural corrections; the one architectural finding (R3-1) is resolved by the operator's decision, applied
without adding a model structure.

- **Object in:** `dev-docs/GwzLifetimeRequirements.md`, revision 4, blob `af4c6079428e25c9d8281aab2ac92a1f46661c29` (committed at
  lane commit `4e49eff9`; 928 lines). Both round-3 reviews pinned this blob.
- **Reviews (not edited):** [Completeness-3](GwzLifetimeRequirements-ReviewCompleteness-3.md), NO-GO: R3-1 P2 architectural,
  R3-2 P2, R3-3..R3-6 P3. [Testability-3](GwzLifetimeRequirements-ReviewTestability-3.md), NO-GO: T3-1, T3-2 P2, T3-3..T3-5 P3.
  Earlier plans: [RemPlan](GwzLifetimeRequirements-RemPlan.md), [RemPlan-2](GwzLifetimeRequirements-RemPlan-2.md).
- **Result:** revision 5, edited in place, 934 lines (cap 1,200), status line "DRAFT, revision 5, 2026-11-04 (round-3
  remediation; operator decision (a) on R3-1)". Blob `e9eaa6ab0b3571834e4d3d5549db5550abe9505e`, computed from the bytes
  (SHA-1 of `blob <size>\0` plus content, which reproduces `af4c6079` for revision 4), not by running git. A later edit changes it.
- **Counts:** 148 requirements (140 MUST, 8 SHOULD, unchanged), 28 non-goals, 35 open questions (34 open; OQ-32 closed), 42
  failure events, 10 declared departures (no DEP added), 41 PROP requirement rows (was 42: LR-REC-5 is no longer PROP).
- **Dispositions:** 22 of 22 open items fixed in text or dispositioned below (section 2). Disputed 0.

## 1 Operator decision applied, and what it did not decide

Source: `GwzTransportReleaseConvergencePlan.md`, D-V, the bullet "Lifetime requirements (operator, 2026-11-04)", read in this lane
at :1112-1119 (it is not yet in the root). Exact words used (:1116-1119):

> **Decision (a), the derived rule:** a write sequence keeps a durable intent record, written before it starts and cleared when it
> finishes, exactly when one of its cut states (Force at a safe point, crash, SIGKILL) cannot be told from the user's own edits by
> its shape; every other cut state is recognized by shape. This is resolution (1) applied once; no named state is relaxed.

Resolution (1) as read at :1069-1070: "each such residue gets a named recognizer in the next command".

Applied as follows:

- **One statement.** The rule is stated once, in LR-REC-3 ("The record rule"), in the operator's words. Every other site cites it
  as "LR-REC-3's record rule": LR-REC-1, LR-REC-5, LR-REC-7, LR-REC-13, LR-OWN-3, §1 L3, §2 R-TREE, E02, E13, E20, §4 (O-forced,
  Tier C, its C cell), §9 P3, §10.1 and §10.2 R1, OQ-1, OQ-8, the closed OQ-32.
- **Derivation, not a list.** The trigger-set list is gone. LR-REC-3 now carries a *current enumeration* (creation sequences,
  the sequences whose contracts keep a record, and checkout), marked "a design must confirm it" by a **shape test** that is a U
  row in the same cell: seed each enumerated cut state and a twin that the user's own edits could have made; where the shape
  recognizer cannot give different answers, the sequence must write its record before its first effect. The basis for checkout is
  A:355, "| R1 | a partly checked-out tree; index and HEAD old; a popped stash kept | Force at a checkout safe point | the notice; D |",
  and LR-REC-7's own rationale ("A torn checkout looks like the user's edits"). The operator named checkout only as an example
  ("e.g."); the enumeration is the drafter's reading and is labelled so.
- **No named state relaxed.** "reported at the stop only" is gone (0 hits). LR-REC-5 now says the stop report is "in addition,
  never the only witness". A Force and a SIGKILL at one point leave one state, and LR-REC-1, LR-REC-5 and LR-REC-7 each carry a K
  row for both cuts.
- **OQ-32 closed**, not deleted: the row keeps its ID, says CLOSED, and records the decision and its D-V source; §10.1 has a
  traceability row for the decision. OQ-8 now depends on no other question. §11's preface says OQ-32 is the one decided entry.
- **Not decided, so not invented:** the flush cost of the record route (a design E row; §9 P3 says so), and any sequence beyond
  the three in the enumeration (the shape test finds them).

## 2 Disposition table (every open item of both reviews)

"Rev-5 line" gives the line of the edited text. "Check" names the section 3 check or the manual argument.

| Item | Sev | Disposition and where | Check | Result |
|---|---|---|---|---|
| R3-1 = T3-2 | P2 arch. | FIXED by the decision of section 1. LR-REC-3 (l.422) states the rule, the enumeration and the shape test; LR-REC-1 (l.420), LR-REC-5 (l.424), LR-REC-7 (l.426) cite it; carve-out deleted; OQ-32 (l.931) closed; OQ-1 (l.900) and OQ-8 (l.907) rewritten; §4 O-forced, Tier C and its C cell, E02, E13, E20, R-TREE, L3, LR-OWN-3, §9 P3 cite it. Regression: A R1 cut by SIGKILL and stopped by Force both reach a recognizer (record) and LR-REC-7's refusal; `stage`/`commit` K rows for both cuts | checks 13, 18, 19 | pass |
| R3-1 provenance | - | FIXED. OQ-32's "no operator source" sentence is gone with the question; the closed row cites D-V REC. No §11 recommendation says "no operator source" where resolution (1) applies | grep `no operator source`: 1 hit, the §0 register preamble (l.89; unrelated) | pass |
| R3-2 = T3-1 | P2 | FIXED. LR-CRED-3 (l.440) narrowed to the sources: the stdio core and the background server copy are exec'd with SCC:208's scrub list and `SSH_AUTH_SOCK` removed (SCC:209, :211; LR-PROC-14), a `--foreground` server scrubs itself (SCC:211); every other class is judged by LR-PROC-14, not this row: `ssh` keeps `SSH_AUTH_SOCK` (SCC:209), a helper runs with its operation's captured environment (LR-CRED-7), no variable reaches "a child that does not need it". LR-PROC-14 (l.394) gained the same two clauses. Verify rewritten: lifetime roles asserted clean; each child class against LR-PROC-14; a conforming `ssh` child, helper and agent signer pass, a leak to a child that does not need the variable fails; spawn-port record of each exec's environment added (U) | departure sweep (section 4); manual | pass |
| R3-2 DEP row | - | NOT NEEDED. After narrowing, no clause departs from SCC:208-211, LR-CRED-7 or LR-PROC-14 (section 4), so no DEP is added | section 4 | pass |
| T3-1 second defect | P2 | FIXED, option 2 (drop the assertion). The text says the cover "execs nothing and starts nothing, so it passes no environment on", and requires no clearing of its live `environ`, because no source asks for it (OQ-35 option (b) would). The Verify cell no longer asserts the cover's live environment; the cover is judged by the credential-source ordering row and the memory scan. OQ-35 (l.934) option (a) says the same. A self-scrubbing server keeps its live-environment seam query (SCC:211 requires the scrub) | manual | pass |
| R3-3 | P3 | FIXED. OQ-16 (l.915) regenerated: 25 wholly PROP rows, 16 partly PROP rows each with its clause (the twelve the review listed, plus LR-OWN-9, LR-EXIT-6, LR-OBS-5 and now LR-REC-3), and a sentence that a row's Source cell names the clause and striking acts on that clause only. LR-PROC-2 and the other eleven are no longer listed as wholly PROP | check 8 | pass |
| R3-4 | P3 | FIXED without a new ID. LR-PROC-1 (l.381), D class: on W the background copy survives the close of a caller's job that permits breakaway (SRV:808; DEP-9, OQ-14); Verify adds a W P row (a permissive job and a kill-on-close job), pending OQ-14. SRV:808 as read: "The copy leaves the caller's job object where the job allows it." DEP-9 and OQ-14 name LR-PROC-1 | manual; check 1 | pass |
| R3-5 (a) | P3 | FIXED. §4 O-clean "K and S children" cell (l.270) now reads "ended and reaped by gwz, or a killed K child pending by its identity (LR-PROC-15)" | read | pass |
| R3-5 (b), (c) | P3 | FIXED, option "drop". E37 (l.246) no longer has "or a gwz thread". A process cannot finish exiting while one of its threads is in uninterruptible sleep; that is a platform fact, not a gwz guarantee, so no requirement is written (see section 5) | `grep "gwz thread"`: 0 hits | pass |
| R3-6 (a) | P3 | FIXED. NG-9's Operator cell routes the pre-composition part to OQ-35 alone; OQ-35 now names the embedding application's pre-composition memory in the forked cover. Because OQ-16 lost that reference, §0's provenance rule now cites OQ-16 | check 4 | pass |
| R3-6 (b) | P3 | FIXED. The Tier C definition (l.258) reads "No gwz code runs at the moment of death, or a write that gwz does not own fails (a libgit2 or Git leftover, E20)" | read | pass |
| R3-5 residue of N-4, R3-3 residue of N-6 | - | closed by the two rows above | checks 8, manual | pass |
| T3-3 | P3 | FIXED. LR-TEST-7 (l.583): the census waits for each tagged process by its own exit notification, never a sleep; "expected" becomes "persistent" (the background server, an F program); a tagged process that never exits (R-B20) fails at the hang guard with the recorded process tree. Code X (l.74) drops "or job" | read | pass |
| T3-4 | P3 | FIXED. LR-TEST-7 states the route by which the tag reaches the remote core under the localhost `sshd` (test `sshd_config` accepts it and the test build's `ssh` allow-list sends it, or a test-only remote command wrapper sets it) and the census self-test seeds a fifth leak, a remote core that outlives its session. SCC:209 allows `SendEnv` only of allow-listed names, so the allow-list route is the one SCC permits | read | pass |
| T3-5 first | P3 | FIXED. Matrix LR-REC-5 note (l.651) now reads "F: ENOSPC with every further write refused; K: SIGKILL at the Force point; U: seeded stopped states" | read | pass |
| T3-5 second | P3 | FIXED. LR-TEST-7 and LR-CRED-3 name the W read as the PEB's environment block, "which is the live block"; LR-TEST-10's process-state O rows gain "the W PEB environment pointer follows the live block"; LR-CRED-3 adds a spawn-port record of each exec's environment (U) so a W role that scrubs itself after receiving a variable still fails | read | pass |

Risks and observations in the two reviews:

| Item | Disposition |
|---|---|
| Completeness: recurrence of the record/recognition seam | Closed structurally: one quantity (recognizability by shape) now decides the record; no second list remains (`trigger set`: 0 hits) |
| Completeness: fix-induced departures; sweep every touched row | Done for every row this round touched (section 4) |
| Completeness and Testability: citation drift of root P23:154 and SCC:230 (`8aa5f800`) | Open for the lane owner; and Plan:1112-1119 is a new lane-only cite (section 5) |
| Testability: churn on the environment row; check against SCC §3.8 line by line | Done: LR-CRED-3 was rewritten against SCC:208-211 clause by clause (section 4) |
| Testability: OQ-32, OQ-1, OQ-8 and LR-REC-7 are one decision | Closed: OQ-32 is decided, OQ-1 and OQ-8 cite the rule, LR-REC-7 cites it |
| Testability: unverified (ext) facts in gating cells; do not freeze the matrix before a spike | Already required by LR-TEST-10's spike clause (unchanged); no new (ext) fact is added except the W PEB live block, which has its O row now |

## 3 Mechanical checks and results

Python over `GwzLifetimeRequirements.md` (934 lines, blob above); the script lives in the session scratchpad, which is wiped on
restart, so each check is restated here. Checks 1-12 are round 2's; 13-19 are new or changed.

| # | Check | Result |
|---|---|---|
| 1 | Every `LR-`, `NG-`, `OQ-`, `DEP-` and `E` ID referenced is defined (148 LR, 28 NG, 35 OQ, 10 DEP, 42 E) | pass: none undefined |
| 2 | Every NG appears in §10 | pass |
| 3 | Every NG is referenced outside §6 | pass |
| 4 | Every OQ is referenced outside §11 | pass (OQ-16 after the §0 cite; OQ-32 from §10.1) |
| 5 | Every non-TEST requirement appears in a §8 row, ranges expanded | pass |
| 6 | Every §8 ID is defined | pass |
| 8 | PROP scan (requirement rows; LR-BND-3's mention excluded) equals OQ-16's list; the bounds row B-TURN / B-LINGER is named | pass: 41 of 41, 0 extra (25 wholly, 16 partly) |
| 9 | Verify codes are a subset of the §8 codes, E and Z excepted | pass: 5 hits, all prose (`K children` in LR-CAN-12, LR-EXIT-8, LR-PROC-13; `K and W`, `S not in it` in LR-PROC-7; `no P row` in LR-PROC-15) |
| 10 | Every code in §8 is defined in §0 | pass |
| 11 | Table integrity: every row has its header's cell count | pass: 0 problems |
| 12 | Every `B-` bound used is in the bounds table | pass |
| 13 | `M-B9`, `Clocks keep running`, `from the intent`, `reported at the stop only`, `trigger set`: absent. `intent record` and `told from the user` (the rule's own words): exactly 1 hit each, in LR-REC-3 | pass |
| 14 | §9.3's range reads `NG-1 to NG-28`, the defined maximum | pass |
| 15 | The start lock `id-<identity key>.lock` is named in R-SRV, LR-OWN-8, -9, LR-CLEAN-8, LR-CONC-5, LR-TEST-7 (LR-OWN-10 cites it through LR-OWN-9) | pass: 6 of 6 rows that name it by file |
| 16 | The derived rule is stated once and cited by every listed site: LR-REC-1, LR-REC-5, LR-REC-7, E02 (R-TREE row), E20, §4 Tier C and its C cell, OQ-1, OQ-8, OQ-32 each contain "record rule" or the LR-REC-3 cite; plus LR-REC-13, LR-OWN-3, L3, R-TREE, E13, O-forced, §9 P3 | pass: 17 of 17 sites cite; the rule's wording occurs once |
| 17 | No remaining relaxation of resolution (1): the string `relax` occurs only in OQ-3 (native-route helpers, unrelated), LR-REC-3 ("No named state is relaxed") and the closed OQ-32 row (which says it was not chosen) | pass |
| 18 | Status line carries "DRAFT, revision 5, 2026-11-04 (round-3 remediation; operator decision (a) on R3-1)" | pass |
| 19 | PROP scan vs OQ-16 after LR-REC-5 lost its PROP clause and LR-REC-3 became partly PROP | pass: included in check 8 |
| 20 | Size against the 1,200 cap | pass: 934 lines |

Not run: every P, K, F, U row (the document authorizes no code), including the new shape test and the census self-test's fifth
leak. Platform fact newly leaned on: the W PEB environment pointer tracks the live block (ext), now with an LR-TEST-10 O row.

## 4 Departure sweep over every row this round touched

Rule from the Completeness risk: sweep the rows a remediation touches, not only the rows a finding names. Sources re-read at
the cited lines.

| Row touched | Source read | Finding |
|---|---|---|
| LR-CRED-3, LR-PROC-14 | SCC:208 scrub list; SCC:209 "the client starts `ssh` with an allow-listed environment only: `PATH`, `HOME`, `USER`, `LOGNAME`, `SSH_AUTH_SOCK`, ..." and "It removes the list from every `sudo` and local stdio child it starts, and for the local stdio child and the test-only separate-user launch it also removes `SSH_AUTH_SOCK`. `ssh` keeps `SSH_AUTH_SOCK` for its own login"; SCC:210 `gwz server stdio` refuses; SCC:211 "it launches its background server with the list and `SSH_AUTH_SOCK` removed, and a `--foreground` server removes them from its own environment before any thread starts" | No departure: the stdio core and background copy lose both; `ssh` keeps the socket; a `--foreground` server scrubs itself. The "every child class, no agent socket" sentence of revision 4 is gone. LR-CRED-7 (helper, captured environment) and LR-PROC-14 agree |
| LR-PROC-1 (D class) | SRV:808 "The copy leaves the caller's job object where the job allows it." | Follows SRV:808; DEP-9 and NG-2 already say so; OQ-14 (b) remains the departure |
| LR-REC-1, -3, -5, -7, -13 | Plan:1069-1070 (resolution (1)); Plan:1116-1119 (decision (a)); A:355 | Follow both; the carve-out that contradicted resolution (1) is removed; no clause relaxes a named state |
| LR-TEST-7, LR-TEST-10 | SCC:209 (`SendEnv` only of allow-listed names) | The tag route through the test `ssh` allow-list is permitted by SCC; the alternative is a test-only wrapper |
| §4 Tier C definition | Plan:1069 ("each such residue") | A libgit2 or Git leftover (E20) is a residue of the same kind; widening agrees with LR-REC-6 |

## 5 For the lane owner

- **Next review:** pin the new blob and read the document, not this plan. Most likely to draw a finding: (a) the current
  enumeration in LR-REC-3 (checkout is the drafter's reading of the operator's "e.g."; the shape test is what makes it
  checkable; stash pop is named in A:355 and not separately enumerated); (b) LR-REC-5 losing its PROP mark (its remaining
  content is resolution (1) and decision (a) read together; the review may prefer to keep a PROP on "no record is written at
  stop time"); (c) dropping the cover's live-environment assertion rather than requiring a clear (OQ-35 carries the other
  option); (d) the SRV:808 clause in LR-PROC-1 being a MUST while OQ-14 is open (it follows the adopted text, as LR-EXIT-1
  follows A pending OQ-13).
- **Citations to re-check at merge:** root P23:154 and root SCC:230 (commit `8aa5f800`), as before; and Plan:1112-1119, which
  exists in this lane (commit `4e49eff9`) and not in the root. Lane P23 lines after :154 are offset by 2 from the root.
- **Out-of-scope observations (not acted on):** (1) a gwz thread in uninterruptible sleep delays the OS's completion of gwz's
  own exit; this is a platform fact and no source asks gwz to guarantee otherwise, so E37 no longer claims it. NG-26 covers the
  one thread class (workspace resolution) that has an accepted source. (2) The record route adds a durable barrier before and
  after each sequence in the enumeration, checkout included; its cost on R1's and R2's path is a design E row, not bounded here.
  (3) The shape test may add sequences beyond the three listed; each is the design's finding, to be recorded against LR-REC-3.
- **State:** the document and this plan are uncommitted in the lane beside the two round-3 reviews.
