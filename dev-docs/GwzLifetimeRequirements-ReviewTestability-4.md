**Verdict: NO-GO**

# Lifetime requirements r5: Testability-axis review, re-verdict round 4

## 0 Evidence

- **Object:** `/Volumes/projects/limbo/gwz-dev-docs-req/dev-docs/GwzLifetimeRequirements.md`, revision 5 ("DRAFT, revision 5,
  2026-11-04 (round-3 remediation; operator decision (a) on R3-1)"), uncommitted in the lane on top of `4e49eff9`.
- **Pin:** `git hash-object` = `e9eaa6ab0b3571834e4d3d5549db5550abe9505e` at start and at end (unchanged during review; matches
  RemPlan-3's stated blob). Revision 4 = `af4c6079` (`git show HEAD:`), used for the comparisons below.
- **Read:** the object in full (:1-934); my `-ReviewTestability-3.md` (and -1, -2 as lineage); `-RemPlan-3.md`; the delta
  (`git diff`, 50+/44-). Peer-blind: `-ReviewCompleteness-4.md` not opened.
- **Sources re-read:** D-V REC, lane `GwzTransportReleaseConvergencePlan.md:1112-1119` (the operator's decision (a)); lane SCC
  §3.8 (:206-212); SRV:808 (parked server design, W background start row); A:353-372 (R1, R3, R18); CT:470-473.
- **Mechanical re-runs (grep, rev5 vs rev4):** `reported at the stop only` 0/2; `trigger set` 0/7; `every child class` 0/1;
  `no agent socket` 0/1; `lacks the variable` 0/1; `intent record` 1/0 (LR-REC-3 only); `power loss is a crash` 1/0 (new);
  `class-to-environment` 0/0; `twin` 1/0 (new, LR-REC-3).
- Nothing modified except this report. No git mutation, no build.

## Prior-finding closure table (round 3)

| ID | Sev | Status | Re-run evidence (revision 5) |
|---|---|---|---|
| T3-1 | P2 | CLOSED (residue: T4-3, P3) | LR-CRED-3 (:440) is scoped to the lifetime roles: stdio core and background server copy are exec'd with SCC:208's list and `SSH_AUTH_SOCK` removed, `--foreground` scrubs itself (SCC:211, re-read). Every other class is "judged by LR-PROC-14, not by this row"; `ssh` keeps `SSH_AUTH_SOCK` (SCC:209: allow-list includes it; `GH_TOKEN` is on the :208 scrub list and not on the allow-list). Re-run of the round-3 reproduction: the P row run over the served `ssh://` form now asserts the `ssh` child holds `SSH_AUTH_SOCK` and not `GH_TOKEN` (LR-PROC-14 :394 Verify), so a conforming carrier passes and a carrier given `GH_TOKEN` fails. Helper: "holds its operation's captured environment" (LR-CRED-7). Lifetime roles: both variables absent, plus an injected spawn-port record of each exec's environment (U), which catches a W role that receives a variable and scrubs itself (PEB live block). Second defect: the cover live-environment assertion is dropped and the cover is judged by the ordering row and memory scan (OQ-35 (a)); text and test now agree. Matrix :634, :662 match |
| T3-2 | P2 | CLOSED for the contradiction; the replacement rule's test opens T4-1 | The carve-out is gone (0 hits). LR-REC-1 (:420), LR-REC-5 (:424), LR-REC-7 (:426) each cite LR-REC-3's record rule, and each carries both cuts: LR-REC-1 "stop each sequence with Force at each safe point ... and assert the next command recognizes that state as it does the one a kill at the same point leaves"; LR-REC-5 P (Force mid-checkout, recognized from the record) and K (SIGKILL at the same boundary); LR-REC-7 K, P (refuse or warn in both). Re-run of the round-3 reproduction (Force vs SIGKILL at one checkout file boundary): one state, one recognizer (the record), no MUST pair left that no build can satisfy. OQ-1 (a) and OQ-8 now cite the rule; OQ-32 closed with the decision. The checkout boundary is a gwz step point (LR-CAN-5 "checkout parks between files"), so the K row is deterministic |
| T3-3 | P3 | CLOSED | LR-TEST-7 (:583): waits on each tagged process's own exit notification, never a sleep; "persistent" (background server, F program) replaces "expected"; a never-exiting tagged process (R-B20) fails at the hang guard with the recorded tree. Code X (:74) no longer says "or job" |
| T3-4 | P3 | CLOSED | LR-TEST-7 states the route (test `sshd_config` `AcceptEnv` plus the test build's `ssh` allow-list `SendEnv`, which SCC:209 permits only for allow-listed names, or a test-only remote wrapper) and adds the remote core to the census self-test |
| T3-5 | P3 | CLOSED | Matrix LR-REC-5 (:651) reads "F: ENOSPC with every further write refused". LR-TEST-7 and LR-CRED-3 name the W PEB read as the live block; LR-TEST-10 (:586) lists "the W PEB environment pointer follows the live block" as an O row |

**Operator decision (a), as written and stated once.** LR-REC-3 (:422) carries the rule once ("**The record rule** (stated here
once; every other site cites it)"); `intent record` occurs once; every other site cites "LR-REC-3's record rule" (§1 L3, R-TREE,
E02, E13, E20, §4 O-forced and Tier C, LR-OWN-3, LR-REC-1, -5, -7, -13, OQ-1, OQ-8, OQ-32, §9 P3). The wording matches
Plan:1116-1119 with one unmarked gloss inside the rule's own sentence: "(Force at a safe point, a crash, SIGKILL; **power loss is
a crash, E13**)". The operator's text has "(Force at a safe point, crash, SIGKILL)". The reading is defensible (E13 is Tier C),
but it widens the cut-state set the shape test must enumerate and is not marked as the drafter's reading (T4-2). No named state is
relaxed. OQ-32 is closed and cites D-V REC.

Summary: all five round-3 findings close against their own reproductions. The new shape test that makes the derived rule
checkable has no defined comparison domain. Applied as written to two states that §10.2 itself maps as harmless and that the rule
does not enumerate, it returns opposite verdicts depending on the reader: T4-1, P2. There are also four P3s.

## 1 Findings

### T4-1 (P2) The shape test has no defined "user's own edits" domain and no rule for states that need no distinction, so its verdict on A R3 and A R18 depends on the reader and can fail a correct build

- **Location:** LR-REC-3 Verify (:422): "U (the shape test): for each enumerated cut state of each write sequence, seed that state
  and a twin that the user's own edits could have made; where the shape recognizer cannot give the two different answers, a
  step-point row asserts that the sequence writes its record before its first effect". Current enumeration (same cell): creation
  sequences, the existing contracts' records, checkout; "a sequence the test finds joins it".
- **Conflicts with:**
  - §10.2 (:825) "R3 unreachable loose objects | LR-REC-5 (harmless; `gc`)";
  - §10.2 (:840) "R18 complete pack and `shallow`, refs old | LR-REC-5, LR-REC-10 (enumerated harmless state)";
  - A:355-357 (accepted): R3 "none needed (`gc`)", covered by (1); A R18 (:372): "the next fetch completes".
- **Violated invariants:**
  - A gating verification must give one verdict for one build, independent of who writes the test.
  - It must pass a correct implementation.
- **Reproduction (two readers, one build).** Take a build that follows the drafter's enumeration: no record for stage or fetch.
  1. Stage cut at a safe point (A R3): unreachable loose objects. Twin: the user's own `git add` of the same content leaves the
     same loose objects, byte for byte. No recognizer can tell them apart. Reader A counts `git add` as "the user's own edits"
     and finds that the twin exists. The test then requires a record before stage, and the build fails.
  2. Fetch cut after transfer (A R18): a complete pack and `.git/shallow`, refs old. A user's own interrupted `git fetch` leaves
     the same state. Reader A again requires a record before every member's fetch. This is the per-member, per-fetch durable
     barrier on R1's path that OQ-32's rejected option (c) priced.
  3. Reader B reads "edits" as working-tree content changes. That reader builds no twin for loose objects or packs and requires
     no record, so the same build passes.
  4. The text supports both readings. The literal one is arguably the stronger, because "could have made" sets no bound. It also
     has no exemption for a state whose correct handling is the same whatever made it: a harmless state needs no "two different
     answers". On the literal reading the shape test pulls stage and fetch into the record route. That contradicts §10.2 and A
     R3, and it adds an R1 cost the operator did not choose. Taken fully literally, "could have made" also covers files in gwz's
     own name shapes, since a user can create any file. Then no state is shape-recognizable, and the test contradicts LR-CLEAN-5
     and the rule's own "every other cut state is recognized by shape".
- **A second gap in the same cell:**
  - The test asserts only the cut state's side. It never asserts that the twin is *not* reported, recovered or refused as
    residue.
  - Without that assertion, a recognizer that flags every working-tree difference as a torn checkout passes. That recognizer
    would trip LR-REC-7's refusal on ordinary user edits, and LR-REC-4 forbids touching user work.
- **Impact:** the mandate's core check, that the derived rule is testable without chance, is not met. The set of sequences that
  must keep a record, and so the R1/R2 barrier cost, is decided by the test author's reading, not by the requirement.
- **Not architectural.** The operator's rule stands unchanged. What is missing is the comparison domain and the pass condition
  of the rule's own test, which is a definition inside the existing model.
- **Required correction:**
  - (a) Define the twin domain for the test. The domain is the states reachable by the user's ordinary edits of tracked and
    untracked content and by ordinary Git commands. It excludes names in gwz's reserved shapes (LR-CLEAN-5) and gwz's record
    locations. The twin is the member of that domain that matches the cut state in every observable a shape recognizer may
    read. If the operator's word "edits" is meant more narrowly (for example, Git commands excluded), say so as the drafter's
    reading, or ask in an OQ.
  - (b) A cut state needs distinguishing only where its required handling (recovery, report, LR-REC-7 refusal) differs from the
    twin's. Name A R3 and A R18 as the examples that need no record under this clause, or put them in the enumeration with
    their cost.
  - (c) Add the twin-side assertion: the recognizer neither reports nor recovers the twin, and `stage`/`commit` do not refuse it.
- **Regression test:**
  - U: the shape test applied to A R3 and A R18 yields "no record", with the stated reason.
  - U: applied to A R1 (checkout), it yields "record".
  - U: a recognizer that flags the R1 twin fails the row.

### T4-2 (P3) The rule's unmarked "power loss is a crash" gloss adds cut states that LR-REC-1's step-point enumeration does not produce

- **Location:** LR-REC-3 (:422), the rule's own parenthesis; LR-REC-1 (:420): states are enumerated "exhaustively from step points
  for gwz-owned writes".
- **Problem:**
  - Plan:1117-1118 lists "Force at a safe point, crash, SIGKILL". The added "power loss is a crash, E13" sits inside the
    sentence the document calls the operator's rule, unmarked.
  - A power-loss cut is the synced prefix at a cut point, which is not the same as a SIGKILL state at that point for writes
    without barriers. The shape test quantifies over "each enumerated cut state", yet no enumeration method produces
    power-loss states.
  - LR-REC-3's F row checks gwz's barrier ordering only for sequences that keep a record. A shape-recognized sequence whose
    power-loss state is shape-indistinguishable is therefore never tested.
- **Correction:**
  - Mark the gloss as the drafter's reading. It belongs in the Source cell or after the quoted rule.
  - Add to LR-REC-1 that the enumeration includes, for each gwz-owned sequence, the power-loss model's state at each cut point
    (LR-TEST-6's "drop every unsynced write"), and that the shape test covers those states too.

### T4-3 (P3) LR-PROC-14's "a child that does not need it" has no per-class oracle, so the child half of LR-CRED-3's P row is decided by the tester

- **Location:**
  - LR-PROC-14 (:394): "scrubbed by class. No credential environment reaches a child that does not need it"; Verify: "a child
    that needs neither holds neither".
  - LR-CRED-3 (:440): "a credential variable or agent socket in a child that does not need it fails".
- **Problem:**
  - The clear cases are now determinate: the lifetime roles, `ssh`, a helper, and `git rev-list` (which needs neither).
  - The contested classes are not:
    - the K `git fetch` fallback, whose credential helper (for example `gh auth git-credential`) reads `GH_TOKEN`;
    - a W `git commit`, whose user hooks may read anything and whose signer needs `SSH_AUTH_SOCK`;
    - F `forall` commands, which are the user's own programs.
  - SCC §3.8 scrubs only `sudo` and local stdio children (re-read). The "need" sets for every other class come from no source
    and are not declared.
  - Round 3's regression step asked for a class-to-environment table under an S row. The table was not added (0 hits).
- **Correction:**
  - Each class declares its environment rule (an allow-list or a scrub list) at the spawn site. The S row checks that every
    class has one, and the P row checks each child against its class's declared rule.
  - Where a class keeps the user's environment for parity (F, and W hooks), the row says so. Otherwise the stripping is put to
    the operator as a behaviour change from 1.0.18.

### T4-4 (P3) Smaller testability gaps in the delta

- **Record-route recovery against later user edits.**
  - Gap: checkout now recovers from a record (LR-REC-3, -5, -7), but no row seeds a live checkout record followed by a user
    edit made before the next gwz command.
  - Risk: a recovery that "completes" the checkout from the record would overwrite that edit.
  - Fix: LR-REC-4's Verify gains this seeded state, and asserts that the edit survives and that the state is reported.
- **W breakaway feasibility (LR-PROC-1 :381, D class).**
  - Gap: LR-TEST-10's O list (:586) names kill-on-close and `IsProcessInJob`, but not breakaway semantics: `BREAKAWAY_OK`
    versus `SILENT_BREAKAWAY_OK`, and a nested job (a CI runner's job) that does not permit breakaway.
  - Risk: the new W P row ("the server runs on") can fail for host reasons.
  - Fix: add breakaway to the O row and to the spike's named-gap clause.
- **LR-TEST-7 census wording (:583).** "no process that gwz created ... is still alive once every process the census tags has
  exited" reads as a tautology. The intended check is "every tagged, non-persistent process exits before the hang guard". State
  it that way.

## 2 Invariant analysis

- **Deterministic verification per platform.**
  - The round-3 infeasibilities are gone:
    - LR-CRED-3's every-class P row;
    - the LR-REC-1/-5/-7 triple.
  - The one new non-determinism is the shape test's domain (T4-1).
  - The new W P row on LR-PROC-1 depends on host job configuration (T4-4) and goes through LR-TEST-10's spike.
- **Derived record rule testable without chance.**
  - What is now testable:
    - seeded cut states (LR-REC-1 U and K-recovery rows);
    - Force and SIGKILL at one gwz checkout step point, recognized alike;
    - barrier order (F) and primitives (U);
    - ENOSPC with the record pre-written (LR-REC-5 F).
  - No sleep and no chance cut: file-event kills are Z-only.
  - What remains: the shape test's verdict is reader-dependent (T4-1), and power-loss cut states are not enumerated (T4-2).
- **LR-CRED-3.**
  - It passes a conforming `ssh` carrier (`SSH_AUTH_SOCK` kept, `GH_TOKEN` absent, per SCC:208-209), a helper (its captured
    environment) and an agent signer (beneath a W child that keeps the socket).
  - It fails a real leak in a lifetime role (exec-time or live block, plus the spawn-port U record) and in `ssh`.
  - For contested child classes, the leak verdict depends on an undeclared need set (T4-3, P3). This is not a false GO on the
    lifetime roles, which are the row's own scope.
- **Stuck, orphan and partial state against operator decisions.**
  - D-V REC is applied with no relaxation.
  - LR-REC-5 now makes the next-command recognizer the witness for every stopped state, which fits resolution (1).
  - E37 no longer claims a gwz-thread case.
  - The §4 O-clean K/S cell admits only the LR-PROC-15 pending child.
  - The delta adds no stuck state.
  - The new partial-state risk is T4-1's: an over-broad recognizer refusing user edits.
- **Architectural root causes:** none. T4-1 is a missing definition and pass condition for a test row. It leaves the operator's
  rule as written.

## 3 Risks

- **Fix-induced drift on the rule's wording.** T4-1 (a) and T4-2 touch the sentence the document calls the operator's rule.
  Keep the operator's words verbatim and put every definition and gloss after it, marked as the drafter's reading. Otherwise the
  "stated once, as written" property is lost.
- **Cost exposure.** If the operator's "edits" is read broadly, fetch joins the record route, and R1's acceptance figures carry
  a durable barrier per member. Surface this explicitly rather than let a design discover it.
- **Unspiked (ext) facts in gating cells:** the PEB live block, `KERN_PROCARGS2`/`/proc` reads, kill-on-close at the last
  handle, `IsProcessInJob`, and now breakaway. They are unchanged in status. The matrix should not freeze before the spike.
- **Citation drift:** Plan:1112-1119 is lane-only (commit `4e49eff9`). Root P23:154 and SCC:230 cite root `8aa5f800`. Re-check
  both at merge.

Pre-commitment: GO on a revision that resolves T4-1 with no new P0-P2. T4-2 to T4-4 do not block.
