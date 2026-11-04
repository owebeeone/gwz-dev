**Verdict: NO-GO**

# Lifetime requirements revision 5: Completeness axis, re-verdict round 4

## 0 Evidence

- **Object pin.** `git -C /Volumes/projects/limbo/gwz-dev-docs-req hash-object dev-docs/GwzLifetimeRequirements.md`
  returned `e9eaa6ab0b3571834e4d3d5549db5550abe9505e` at the start and the same hash at the end. The object is
  uncommitted in lane `gwz-dev-docs-req` (HEAD `4e49eff9`, which holds revision 4 as blob `af4c6079`). I attacked the
  delta with `git diff 4e49eff9` (36 hunks) and read every changed row in full.
- **Peer-blind.** I did not open `GwzLifetimeRequirements-ReviewTestability-4.md` or `-ReviewTestability-3.md`.
- **Operator decision read at the source.** In the lane at Plan:1112-1119 (D-V, bullet "Lifetime requirements
  (operator, 2026-11-04)"), and resolution (1) at Plan:1066-1076.
- **Sources re-read at the cited lines.**
  - A:353-356 (R1 and R2 rows).
  - SCC:206-211 (§3.8).
  - SRV:808 ("Background start" row).
  - `GwzM5-8I2RecordContract.md` §5 (:322-331), :340 and §8 (:454-470).
- **Mechanical checks.** Python over the object; the script is in the session scratchpad.
  - Defined IDs: 148 LR (140 MUST, 8 SHOULD), 28 NG, 35 OQ, 10 DEP and 42 E. Every reference resolves.
  - Every non-TEST LR has a §8 row, ranges ("-3 to -7", "..") expanded, and every §8 ID is defined.
  - PROP scan: 42 rows carry "PROP". LR-BND-3 is the known false positive. The remaining 41 equal OQ-16's list, 25
    wholly and 16 partly PROP with no overlap, and each partly-PROP row names its clause in the row.
  - Verify codes against the §8 cells: five hits (LR-CAN-12, LR-EXIT-8, LR-PROC-7, LR-PROC-13, LR-PROC-15), all
    prose.
  - String counts:

    | String | Hits | Where |
    |---|---|---|
    | `trigger set` | 0 | |
    | `reported at the stop only` | 0 | |
    | `gwz thread` | 0 | |
    | `M-B9` | 0 | |
    | `from the intent` | 0 | |
    | `intent record` | 1 | LR-REC-3 |
    | `told from the user` | 1 | LR-REC-3 |
    | `no operator source` | 1 | the §0 register preamble, unrelated |

## Prior-finding closure table

Each round-3 check was re-run against blob `e9eaa6ab`.

| ID | Sev | Re-run check | Result | Evidence |
|---|---|---|---|---|
| R3-1 | P2 arch. | One rule, derived from recognizability. LR-REC-1, -5, -7, Tier C, E02 and OQ-1/-8/-32 cite it. OQ-32's provenance is corrected. A R1 cut by SIGKILL and stopped by Force both reach a recognizer and LR-REC-7 | **CLOSED** | The rule is stated once, in LR-REC-3 (:422), in the operator's words. Every other site cites "LR-REC-3's record rule" (:154, :174, :197, :213, :226, :256, :259, :277, :301, :420, :424, :426, :432, :696, :900, :907). The carve-out is gone. Checkout (A:355 R1) is in the current enumeration, so both cuts have a record and LR-REC-7 holds; LR-REC-1, -5 and -7 each carry a Force and a SIGKILL row. OQ-32 is closed with its D-V REC source (:931), and OQ-8 is independent (:907). The record rule's interaction with LR-REC-12's closed list is a new finding: R4-1 |
| R3-2 | P2 | LR-CRED-3 and LR-PROC-14 agree with SCC:208-211 and LR-CRED-7. The P row passes on a build that implements SCC:209 | **CLOSED** | LR-CRED-3 (:440) is narrowed to the lifetime roles: the stdio core and the background copy without SCC:208's list and `SSH_AUTH_SOCK` (SCC:209, :211), and a `--foreground` self-scrub (SCC:211). Every other class is deferred to LR-PROC-14. `ssh` keeps the socket and a helper gets its captured environment, matching SCC:209 ("`ssh` keeps `SSH_AUTH_SOCK` for its own login") and LR-CRED-7. LR-PROC-14 (:394) gained the same two clauses. Verify passes the conforming `ssh`, helper and agent signer. No DEP row is needed |
| R3-3 | P3 | Every row whose Source cell scopes PROP to a clause is listed as partly PROP | **CLOSED** | OQ-16 (:915) lists 25 wholly and 16 partly PROP rows, each partly row with its clause. LR-PROC-2, LR-EXIT-8 and LR-LIVE-1 are now partly PROP. Striking acts on the named clause only |
| R3-4 | P3 | A requirement carries SRV:808, with a W P cell | **CLOSED** (residue P3) | LR-PROC-1's D class (:381) requires that the background copy survive the close of a caller's job that permits breakaway, citing SRV:808, DEP-9 and OQ-14. There is a W P row for a permissive job and a kill-on-close job. The §8 row was not updated: R4-4 |
| R3-5 | P3 | (a) the O-clean cell covers a pending K child; (b), (c) E37's thread case | **CLOSED** | §4 :270 reads "or a killed K child pending by its identity (LR-PROC-15)". E37 (:246) no longer has a thread case |
| R3-6 | P3 | (a) NG-9 routes to OQ-35 alone; (b) E20's Tier C fits the definition | **CLOSED** (residue P3) | (a) NG-9 (:552) gives "none (OQ-35)", and OQ-35 (:934) names the pre-composition memory. (b) Tier C (:258) is widened to "or a write that gwz does not own fails". The process-level rows of the C column now misfit that case: R4-3 |

## 1 Findings

R4-1 is P2, non-architectural. R4-2 to R4-4 are P3.

### P2

**R4-1 (P2). LR-REC-12's closed list of recovery mechanisms refuses the records the record rule requires, and
LR-OWN-8 does not name a record that Force leaves in place.**

- **Location:**
  - LR-REC-12 (:431), the clause "A mechanism outside the list is refused." and its S row;
  - LR-REC-3's current enumeration (:422);
  - LR-REC-13 (:432);
  - LR-REC-5 (:424), "a stop leaves it in place";
  - LR-OWN-8 (:306), the entry "Tier C records (next command)";
  - §4 O-forced (:256).
- **What is wrong.** LR-REC-12 (MUST; its closed list is PROP) says that the existing durable recovery mechanisms
  form a closed list, and that "A mechanism outside the list is refused". The list holds:
  - merge records and the action journal;
  - the checked-artifact quarantine;
  - `recover_or_create`;
  - `resume_or_admit`;
  - family `creating`/`disposing` rows;
  - the volatile-filesystem degradation.

  Its S row passes a deletion or rewrite call site only if it "is on the list". LR-REC-3's record rule, the
  operator's decision D-V REC, now requires a durable intent record and its recovery for checkout (new in revision 5;
  revision 4 had kept checkout out). The record rule's current enumeration also covers creation sequences (LR-REC-13,
  pre-existing). Neither is on LR-REC-12's list. LR-REC-2's "one recovery path" is the route by which a mechanism is
  reached; it is not itself a list entry.
- **State sequence.**
  1. A design implements the checkout intent record, as LR-REC-3 requires.
  2. A later command's recovery retires that record and recovers or reports the tree.
  3. LR-REC-12's S check finds that the deletion or rewrite site is in a recovery function that is not on the closed
     list, so the build fails a MUST.
  4. A design that obeys LR-REC-12 cannot meet the operator's rule for checkout, or for creation sequences.
- **Second, related gap.** LR-REC-5 says that the rule's record "exists before the sequence started … and a stop
  leaves it in place". O-forced (:256) allows nothing to be left except LR-OWN-8's entries. LR-OWN-8's only matching
  entry is "Tier C records (next command)", but a Force stop is O-forced, not Tier C. The record that Force leaves has
  no entry by name.
- **Why P2.** Two MUSTs conflict, and the S check would fail a build that conforms to the operator's decision. The
  record rule's own enumeration cites LR-REC-12's list, so the two lists also disagree about which records exist.
  That is the "one rule for records" mandate failing at a second site.
- **Provenance and history.** The creation-sequence half predates this round; I missed it in rounds 1-3. The delta
  extended it to checkout.
- **Why not architectural.** The derived rule is the right model. LR-REC-12 needs to reference that rule, not hold a
  second list.
- **Required correction:**
  - Add to LR-REC-12's list: "the record rule's intent records (LR-REC-3: its current enumeration and any sequence
    the shape test adds), reached through LR-REC-2's point".
  - Rename LR-OWN-8's entry "Tier C records" to "the record rule's records left by a stop or a cut (LR-REC-3,
    LR-REC-5; retired by the next command's recovery)".
- **Regression check.** For checkout and for a workspace-root `clone`, the recovery that retires the intent record is
  on LR-REC-12's list. A Force-stopped checkout's record is an LR-OWN-8 entry by name.

### P3

**R4-2 (P3). The enumeration counts the existing contracts' records as the rule's records, but the rule's lifetime
and its "exactly when" do not match them.**

- **Location:** LR-REC-3's current enumeration (:422), "the sequences whose existing contracts keep a record
  (LR-REC-12's list)".
- **Lifetime mismatch.** The rule's record is "cleared when it finishes". The accepted I2 record contract keeps a
  merge record past completion: the accepted workspace cannot be cleared "through finalization, completion,
  archival, and GC" (:340), and the publication fields are "never retired before archival/GC" (§8, :465). Only the
  pending action retires when its result is reconciled (:461).
- **"Exactly when" mismatch.** These sequences are in the enumeration because a contract keeps a record, not because
  the shape test placed them there. The rule says a sequence keeps a record "exactly when" shape fails. If a design's
  shape test passes for one of them, the rule as written withdraws an accepted contract's record without a DEP row.
  The Verify cell tests only the "shape fails, so a record is needed" direction, so no build fails a row. That is why
  this is P3.
- **Correction.** Add one sentence:
  - an existing contract's record (LR-REC-12) keeps its own trigger and lifetime and serves as the rule's record for
    its sequence;
  - the rule decides only which sequences must keep an intent record;
  - a shape test that passes never withdraws an accepted contract's record.

  Alternatively, declare a DEP row and an OQ.

**R4-3 (P3). Tier coherence for E20.**

- **(a) O-clean versus E20 (pre-existing).** E20's gwz-owned writes are assigned O-clean (:284). O-clean says that
  "Nothing is left behind" (:255), and its column says "Multi-file partial state within a member: none" (:277). Yet
  E20 and LR-REC-5 require that a multi-file sequence stopped by ENOSPC be reported with its safe point and recognized
  by the next command. Correction, either of:
  - an O-clean exception: "a multi-file sequence stopped by a write failure, at a safe point (LR-REC-5)";
  - assign that case the O-forced column.
- **(b) The widened Tier C (R3-6 residue).** Tier C now includes "a write that gwz does not own fails" (:258). The C
  column's process-level rows describe a death, so they are false for E20's libgit2 or Git leftover, where gwz runs
  on, reports `Failed`, and exits normally:
  - "gwz process exit: no code runs";
  - "Report: none from this process";
  - "Exit status: the OS's".

  Correction: scope those three rows "for a death of gwz", or say that a non-gwz write failure takes the O row for
  the process and the C row only for its residue.

**R4-4 (P3). Traceability and table drift.**

- **The D-V key.** It gives "Plan:1030-1110" (:39), but its own REC sub-key is at :1112-1119. The block runs to
  :1121.
- **LR-REC-7 (:426).** Its parenthesis "(a checkout does)" states as settled what LR-REC-3 says "a design must
  confirm". Write "(checkout is in the current enumeration)".
- **§9.3 (:727).** It says R-D1 and R-D2 disappear because temporaries are "named in intents". Under the record rule,
  only the enumerated sequences keep intents, so other sequences' temporaries are not named in one. They disappear
  only if shape recognition plus proof of ownership (LR-CLEAN-5) suffices, the same condition as R4's row. This part
  is non-normative and stale since the trigger narrowing.
- **§8, row "LR-PROC-1, -10, -12, -14" (U P S everywhere).** By §8's own rule it covers IDs only when every cell is
  identical. LR-PROC-1 now has a W-only P row (pending OQ-14), and LR-PROC-12 has U only. Split LR-PROC-1 into its
  own row (M: U S, L: U S, W: U P S, note "W: job breakaway, pending OQ-14").
- **§8 LR-REC-3 note (:664).** It does not mention the shape test, which is the rule's U row. The codes already cover
  it.

## 2 Invariant analysis

| Invariant | State in revision 5 |
|---|---|
| Every operator decision traced, the new D-V REC included | **Holds.** D-V REC is in §0's key, §10.1 (:741), LR-REC-3's Source, and OQ-32 (closed). The decision is applied as written: the wording is the operator's. "Power loss is a crash, E13" is a gloss that resolution (1) supports (Plan:1069, "SIGKILL, a crash or power loss"). "No named state is relaxed" is kept, and the rule is stated once. Standing rules, accepted clauses and residuals: unchanged since round 3, and the SRV:808 gap is closed |
| No misstatement of a source | **Holds**, apart from the D-V key range (R4-4). LR-CRED-3 and LR-PROC-14 now read SCC:209 correctly. OQ-14 and DEP-9 match SRV:808. The provenance claim in OQ-32 is gone |
| No invented decision | **Holds.** The enumeration is labelled provisional and must be confirmed by the shape test. LR-REC-5's "No record is written at stop time" is now unmarked; it reads as a consequence of the rule's "written before it starts", and I do not raise it |
| One rule for records and recognition | **Fails at one site.** LR-REC-12's closed list and LR-OWN-8 do not admit the rule's records (R4-1). The contract records are not reconciled with the rule's lifetime (R4-2) |
| IDs, tables and matrix | **Pass on the mechanical checks**, with matrix drift on LR-PROC-1 (R4-4) |

## 3 Risks

- **Closed lists elsewhere.** The record rule creates records that LR-REC-12 and LR-OWN-8 did not anticipate. Before
  the next revision, sweep every "closed list" and "refused" clause against LR-REC-3: LR-OWN-2, LR-OWN-8, LR-PROC-1
  and LR-REC-12.
- **RemPlan-3 §1 claim.** It says the operator named checkout "only as an example ('e.g.')". No "e.g." occurs at
  Plan:1112-1119. The object does not repeat the claim, so this is no finding against the document. The lane owner
  should not carry it forward.
- **Citation drift.** As before: root P23:154 and root SCC:230 (`8aa5f800`), plus the lane-only Plan:1112-1119.
  Re-check at merge.

**Pre-commitment.** I will return GO on a revision that:
- fixes R4-1 as stated;
- introduces no new P0-P2 on this axis.

R4-2 to R4-4 are recommended but do not block.
