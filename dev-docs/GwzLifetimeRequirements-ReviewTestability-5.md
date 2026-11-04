**Verdict: GO**

# Lifetime requirements r6: Testability-axis review, re-verdict round 5

## 0 Evidence

- **Object:** `/Volumes/projects/limbo/gwz-dev-docs-req/dev-docs/GwzLifetimeRequirements.md`, revision 6 ("DRAFT, revision 6,
  2026-11-04 (round-4 remediation, operator-authorized)"), 939 lines, uncommitted in the lane on top of `4e49eff9`.
- **Pin:** `git hash-object` = `cbb78318e0bf9c622a1e44a92657b0e3bd96f573` at start and at end. It matches RemPlan-4's stated
  blob. The revision 5 blob `e9eaa6ab` is not in the object store, so I located the delta from RemPlan-4 sections 1-2 and from
  greps, and read each changed row in full.
- **Read:** my reports -1 to -4 (T4-1 and T4-2 again in full); `-RemPlan-4.md`; the changed rows: LR-REC-3 (:425), LR-REC-12
  (:434), LR-OWN-8 (:309), the D-V key (:42), the §8 rows LR-PROC-1, -10/-14, -12 and LR-REC-3 (:637-639, :660), §9.3 (:732),
  §10.1 (:746).
- **Context rows re-read:** LR-REC-1, -4, -5, -7, -10, -13 and LR-CLEAN-5; LR-TEST-6; §10.2 R1, R3, R18 (:828-845); D-V REC at
  lane `GwzTransportReleaseConvergencePlan.md:1112-1119`; the I2 record and action-journal contracts (no fetch coverage, 0 hits).
- **Peer-blind:** `-ReviewCompleteness-5.md` was not opened.
- Nothing was modified except this report. No git mutation and no build.

## Prior-finding closure (round 4)

| ID | Sev | Status | Re-run evidence (revision 6) |
|---|---|---|---|
| T4-1 | P2 | **CLOSED** | Each element of the mandate is checked below |
| T4-2 | P3 | PARTLY CLOSED (carried, RemPlan-4 §3) | The gloss now sits in reading (i), marked as the drafter's. The shape test's U row covers "the power-loss state at its cut point", which is one deterministic state per cut point under LR-TEST-6's "drop every unsynced write". LR-REC-1's own enumeration clause is unchanged |
| T4-3 | P3 | OPEN (carried) | Unchanged, as authorized |
| T4-4 | P3 | OPEN (carried) | Unchanged, as authorized |

### How T4-1 closes against the mandate

- **Rule verbatim.** The rule sentence in LR-REC-3 now matches Plan:1116-1119 word for word: "(Force at a safe point, crash,
  SIGKILL)", with no gloss inside it. The readings follow it under a label that says who owns each. The "stated once, as
  written" property, which I named as a risk in round 4, holds.
- **Domain defined and decidable** (reading (ii)). The twin domain covers ordinary user edits of tracked and untracked content
  and ordinary Git commands. Interrupted commands are in it: the text's own A R18 example is an interrupted `git fetch`. The
  domain excludes LR-CLEAN-5's reserved shapes and gwz's record locations. The twin matches the cut state in every observable the
  shape recognizer reads. My round-4 reproduction needed three readers:
  - Reader A counted `git add` and `git fetch`. They are now in the domain by text.
  - Reader B counted working-tree edits only. That reading is now excluded.
  - The "a user can create any file" reader made shape recognition impossible. Reserved shapes are now excluded, so that
    reading is gone too.
- **One verdict for A R3 and A R18** (reading (iii) and the U rows). A record is required only where the cut state's required
  handling differs from its twin's. The verdict for these two states is "no record" in both cases that remain:
  - If a reader decides no twin exists, shape recognizes the state, so no record is needed.
  - If a twin exists, the handling is the same: `gc`, or the next fetch completes. Again no record is needed.
  - The regression row fixes both outcomes, "no record" for A R3 and A R18 and "record" for A R1, so the verdict no longer
    depends on the reader.
- **No per-fetch barrier.** The barrier is stated in the text ("there is no per-fetch durable barrier"). LR-REC-3's durability
  order applies only "where a sequence keeps a record". The U row asserts that fetch and stage write none. The I2 contracts have
  no fetch coverage, so no existing record conflicts with this.
- **Twin-side assertion exists.** The row requires that the next command neither reports, recovers nor refuses a twin, and that
  `stage` and `commit` do not refuse it. A recognizer that flags every working-tree difference fails, and so does one that flags
  the A R1 twin.
- **No false GO through the recognizer's observables.**
  - In a U fake filesystem the test can construct a twin that matches any observable, timestamps included. So a recognizer that
    reads extra heuristic observables cannot escape the record requirement.
  - A build that writes no record for A R1 still fails, because the seeded state without a record has a twin and the handling
    differs.

### Changed MUSTs: still deterministic to verify

- **LR-REC-3.** The new U rows run on seeded states in a fake filesystem, with a step-point assertion that the record is written
  before the first effect. There is no sleep and no chance cut.
- **LR-REC-12.** The added entry is checked by the existing S row (the recovery is reached through LR-REC-2's point). The K row
  now kills during the recovery of a checkout's record and of a creation sequence's record. Both are gwz step points.
- **LR-OWN-8.** The new named entry falls under the row's "U per entry".
- **§8 split.** Each §8 row now matches its row's Verify codes:
  - LR-PROC-1: U S on M and L, U P S on W;
  - LR-PROC-10: U P;
  - LR-PROC-14: U P;
  - LR-PROC-12: U.

  The W P row on LR-PROC-1 still rests on unspiked breakaway semantics (T4-4, carried).

## 1 New findings (delta only)

No P0, P1 or P2 findings. Three P3s, none architectural.

### N5-1 (P3) The twin-side row and LR-REC-1's U row assert on the same seeded bytes for a state that is its own twin

- **Location.**
  - LR-REC-3's twin-side U row (:425): "the next command neither reports, recovers nor refuses a twin".
  - LR-REC-1 (:423): every enumerated state is recognized, and the next command "recovers the state, or reports it with a
    remedy". Its U row seeds "pack without refs" and asserts "recognition and a remedy that succeeds".
- **Problem.**
  - A R3 and A R18 are byte-identical to their twins, so the twin-side row also binds the cut states themselves.
  - Suppose a reader reads LR-REC-1's "recognition" for a harmless state as an observable report, for example `gwz status`
    naming the pack without refs. That reader fails the twin-side row on the same bytes.
  - The consistent reading resolves it: recognition of a harmless state means needing no action, and the remedy is the
    ordinary next command (`gc`, the next fetch). Under that reading a correct build passes both rows, so this is not a P2.
  - The text never says so, however. The delta turned a latent looseness in LR-REC-1 into a pair of opposed-looking assertions.
- **Correction.** Add one clause to reading (iii) or to the twin-side row: a state whose handling equals its twin's is its own
  twin. LR-REC-1's recognition of it is "no action", its remedy is the ordinary next command, and the twin-side row applies to
  it as written.

### N5-2 (P3) "The sequence writes no record" does not say whose record

- **Location.** The LR-REC-3 shape-test U row: "where the required handling is the same (A R3, A R18), the row asserts that the
  sequence writes no record".
- **Problem.**
  - A fetch inside a workspace-root `clone` runs under the clone's creation record (LR-REC-13).
  - A stage inside a merge runs under that merge's I2 record, if one applies.
  - A strict tester could apply the A R18 or A R3 row to those fetches and stages, and so fail a correct clone or merge.
  - This sits beside the carried R4-2 (which records an existing contract keeps and for how long).
- **Correction.** Reword to "keeps no record of its own; a record of an enclosing sequence (LR-REC-13, LR-REC-12's list) is
  unaffected".

### N5-3 (P3) Two wording points in the shape test's condition

- **Redundant clause.**
  - The clause "where ... the shape recognizer cannot give the two different answers" is always true once a twin exists. By
    definition (ii), the twin matches the cut state in every observable the recognizer reads.
  - The clause is harmless. The condition that decides is (twin exists, handling differs).
- **"Shape recognizer" left undefined.**
  - The term does not say that it reads no record. The domain excludes record locations, so a recognizer that consulted the
    record would make every twin impossible, and the test's condition would become circular.
  - No false GO follows: the seeded state of a build without a record still has a twin, and the A R1 regression row asserts
    "record".
  - Fix: one phrase, "a shape recognizer reads no record (LR-REC-3's record locations)".

## 2 Invariant analysis

- **Deterministic verification per platform.** The round-4 non-determinism (T4-1) is gone. No changed MUST adds a sleep, a chance
  cut or a host-dependent row. The one host-dependent row in scope is LR-PROC-1's W P row, which is unchanged in substance and
  goes through the LR-TEST-10 spike.
- **Record rule testable without chance.** The rule is now testable without chance:
  - seeded cut states, including the power-loss state per cut point;
  - Force and SIGKILL at one gwz step point, recognized alike;
  - the barrier order (F) and the primitives (U);
  - the twin domain with both sides asserted;
  - fixed regression outcomes for A R1, A R3 and A R18.
- **Stuck, orphan and partial state against operator decisions.** D-V REC is applied verbatim with no relaxation. The twin reading
  narrows nothing that the operator's rule required: A R3 and A R18 were already "harmless" in §10.2 and accepted A R3. The
  twin-side row now guards the over-broad recognizer risk that round 4 raised, a recognizer that refuses user edits.
- **Architectural root causes:** none.

## 3 Risks

- **Source of the twin reading.** LR-REC-3 and §10.1 attribute it to the operator on 2026-11-04 but record it only in
  RemPlan-4, which is a remediation plan, not the operator's text. RemPlan-4 §5 (a) raises this itself. Before merge, put it in
  D-V's bullet or in an OQ, so that a design cites an operator document.
- **Carried P3s** (R4-2, R4-3, T4-2's LR-REC-1 clause, T4-3, T4-4). N5-1 and N5-2 touch the same sentences as R4-2 and should be
  fixed together with it.
- **Unspiked (ext) facts in gating cells.** The PEB live block, `KERN_PROCARGS2` and `/proc` reads, kill-on-close at the last
  handle, `IsProcessInJob`, and breakaway are all still unspiked. Do not freeze the matrix before the LR-TEST-10 spike.
- **Citation drift.** Plan:1112-1121 is lane-only (`4e49eff9`). Root P23:154 and SCC:230 (`8aa5f800`) must be re-checked at
  merge.

Pre-commitment honoured: T4-1 is closed and the delta adds no P0-P2. GO.
