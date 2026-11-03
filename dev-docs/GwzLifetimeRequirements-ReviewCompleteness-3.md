**Verdict: NO-GO**

# Lifetime requirements revision 4: Completeness axis, re-verdict round 3

## 0 Evidence

- **Object pin.** `git -C /Volumes/projects/limbo/gwz-dev-docs-req hash-object dev-docs/GwzLifetimeRequirements.md`
  returned `af4c6079428e25c9d8281aab2ac92a1f46661c29` at the start and the same hash at the end, so the object did not
  change during the review. It is uncommitted in lane `gwz-dev-docs-req` (HEAD `a874eddc`). The revision-3 blob
  (`ef534435`) is not in the object store, so I attacked the delta through RemPlan-2's disposition table and by reading
  every changed row in full.
- **Peer-blind.** I did not open `GwzLifetimeRequirements-ReviewTestability-3.md`. I did not read
  `-ReviewTestability-2.md` either; I know its T2 items only from RemPlan-2's rows.
- **Sources re-read at the cited lines.**
  - A: :94-110, :230-240, :350-372 and :384.
  - B: :124-132, :160-166, :340-343 and :361.
  - C (lane `gwz-dev-docs-cd`): :120-132. D: OD-D7 (:314) and R12/R-B9 (:210, :226). DS: S-P2-4 (:105).
  - SRV (parked, adopted): :800-812 and :1105-1118.
  - SCC (lane): :20-25, :78-82, :205-212 and :226-243. Root SCC: :228-232.
  - P23 (lane): :152 and :175. Root P23: :150-156.
  - Plan D-V resolutions: :1064-1076. H: :211-219.
  - Code: `gwz-core/crates/session-host/src/detached.rs:20-30` and
    `gwz-core/src/operation/memberlockmanager.rs:104-115`.
- **Mechanical checks.** Python over the object; the script is in the session scratchpad.
  - Defined IDs: 148 LR (140 MUST), 28 NG, 35 OQ, 42 E and 10 DEP. Every LR, NG, OQ, DEP and E reference resolves.
  - Every NG appears in §10 and is referenced outside §6. Every OQ is referenced outside §11. The only ID range in the
    text, `NG-1 to NG-28` (:727), equals the defined maximum.
  - Every non-TEST LR has a §8 row, and every §8 ID is defined.
  - PROP scan: 43 rows carry "PROP". One of them, LR-BND-3, is a false positive because it only mentions "rows marked
    PROP". The remaining 42 equal OQ-16's list exactly, and the bounds row B-TURN / B-LINGER (:487) is named in OQ-16.
  - Verify codes against the §8 cells: five hits, all regex false positives. Examples: "K children" in LR-EXIT-8 and
    "no P row" in LR-PROC-15.
  - `M-B9`, `Clocks keep running`, `intent record` and `from the intent`: no occurrences.

## Prior-finding closure table

Each round-2 check was re-run against blob `af4c6079`.

| ID | Sev | Re-run check | Result | Evidence |
|---|---|---|---|---|
| N-1 | P2 | `grep "recorded\|intent record\|from the intent"` finds no normative cell that needs a record outside LR-REC-3's trigger set. LR-REC-5, §4, E02, E20 and OQ-1 agree with LR-REC-3 and OQ-32 | **CLOSED** (for the five named sites) | No remaining hit is a record obligation for partial state. LR-REC-5 (:421), §4 O-forced (:254), the C cell (:273-274), E02 (:195), E20 (:224) and OQ-1 (:894) now say "from a record where the trigger set applies, otherwise by shape". The fix added a carve-out for states that are recognizable neither way, and that carve-out reopens the class at the model level: R3-1 |
| N-2 | P2 | Sweep the adopted SRV platform table, SRV §15 and A §3.1/§3.6 against LR-EXIT, LR-PROC and NG; check that each recommendation's reason matches its source | **CLOSED** | DEP-8 matches A:96-97, :235-236 and :384. DEP-9 matches SRV:808 (line verified: "Background start" row). DEP-10 matches SRV:1115 and SCC:209. DEP-3 now cites SRV:809 and :1107, B:162-165 and C:130. OQ-13 says A took (b); OQ-14 now recommends (a), "as SRV:808 asks". The same sweep, extended to LR-CRED, finds a new undeclared departure introduced by the T2-3 fix: R3-2 |
| N-3 | P2 | Every clause of LR-OWN-9 and -10 is in P23:154 or SCC §4.2, or is PROP. R-SRV lists `id-<identity key>.lock` | **CLOSED** | Root P23:154 and root SCC:230 read as quoted. The detached-worker clause is PROP, and its code cite holds: `detached.rs:27`, `type Entry = (String, Weak<()>)`. The start lock is named in R-SRV (:174), R-LOCK, LR-OWN-8, -9, LR-CLEAN-8, LR-CONC-5, LR-TEST-7 and the §10.1 row (:799). LR-OWN-10 cites P23:154 and SCC:230 |
| N-4 | P2 | For E37, LR-PROC-2, LR-OWN-8, §4 and LR-TEST-7 can all hold together | **CLOSED** (residue P3) | LR-PROC-2 (:378) names the pending child as its one added exception. LR-OWN-8 (:302) carries the entry. O-clean and O-forced (:253-254) now except LR-OWN-8's list. LR-PROC-15 has no P row, so the census cannot meet the case. Residue: R3-5 |
| N-5 | P3 | ID ranges equal the defined maxima. E23 equals LR-EMB-3 | **CLOSED** | :727 reads "NG-1 to NG-28". E23 (:229) and LR-EMB-3 (:525) both give B-CLEAN for the `HostOwner` and B-CLOSEW for each pump |
| N-6 | P3 | The PROP scan, bounds table included, equals OQ-16's list | **CLOSED** (residue P3) | 42 of 42, plus the bounds row. OQ-16's "Partly PROP" sentence is incomplete: R3-3 |
| N-7 | P3 | For every row, the Verify codes are a subset of the §8 codes, E and Z excepted | **CLOSED** | No real misses. The five script hits are prose, for example "K and W in it" in LR-PROC-7 and "there is no P row" in LR-PROC-15 |
| C-13 | P3 | partial | **CLOSED** | Closed with N-6 |
| Obs. | - | §11 states who wrote the recommendations | **DONE** | :888-890 |

## 1 Findings

R3-1 and R3-2 are P2; R3-3 to R3-6 are P3.

### P2

**R3-1 (P2, ARCHITECTURAL). The record trigger set is defined independently of the recognition obligation. Under
OQ-32's recommended answer, a shape-indistinguishable state has no recognizer after a crash, which breaks resolution
(1), LR-REC-1, LR-REC-7 and the Tier C definition. OQ-32 states the operator source for the record route wrongly.**

- **Location:**
  - LR-REC-5's PROP clause (:421).
  - LR-REC-3's "Trigger set" (:419).
  - LR-REC-1 (:417).
  - LR-REC-7 (:423).
  - §4's Tier C definition (:256) and its C cell (:273).
  - E02 (:195).
  - OQ-1 (:894), OQ-8 (:901) and OQ-32 (:925).
- **Violated invariants:**
  - Internal coherence of the MUSTs.
  - No misstatement of a source. The operator's resolution (1) (Plan:1069-1070) says "each such residue gets a named
    recognizer in the next command".
- **State sequence:**
  1. LR-REC-5's PROP clause concedes that some stopped states exist that "LR-REC-1's enumeration cannot tell from the
     user's own edits". Under OQ-32 (a), such a state is "reported at the stop only".
  2. A checkout (A R1, A:355: "a partly checked-out tree; index and HEAD old"; recognized by "the notice; D") is neither
     a creation sequence nor a contract-backed one. Under OQ-32 (a) it therefore has no record.
  3. A SIGKILL, instead of a Force, cuts the same checkout at the same file boundary. No stop report exists. That is a
     Tier C residue.
  4. LR-REC-1 (MUST) requires the next command to recognize each enumerated state "from a durable record or from the
     state's own shape". Neither exists, so the requirement cannot be met. The Tier C definition (:256, "either durably
     recorded … or recognizable by its shape") cannot be met either, and resolution (1)'s "named recognizer in the next
     command" goes unmet.
  5. LR-REC-7 (MUST, PROP) requires a later `stage` or `commit` to refuse or warn for a tree "torn by Tier C, or
     stopped at a safe point by Force". That needs the same recognition, which does not exist. Its text carries no
     exception.
  6. E02 (:195) still says the stopped state is "recognized by the next command" without the exception.
- **Contradictions:**
  - OQ-32's recommendation says that under (a) such a state "is reported at the stop only (LR-REC-5, LR-REC-7)". This
    cites LR-REC-7 as accommodating a case its text forbids.
  - OQ-32 justifies (a) by saying that (b) and (c) "are a per-sequence journal with no operator source". For a sequence
    whose crash states shape cannot recognize, resolution (1) together with LR-REC-1 is exactly that source. The
    recommendation therefore misstates the decision it departs from, and the departure is not in the §0 register.
  - OQ-8 recommends confirming LR-REC-7 as a MUST, and OQ-32 recommends an answer under which LR-REC-7 cannot hold.
    The operator is invited to confirm both "as a set" (OQ-16).
- **Why architectural.** This is the third consecutive round in which the record and recognition relation broke:
  - round 1, C-7: an untraced per-sequence intent mandate;
  - round 2, N-1: the narrowed trigger set against five "recorded" obligations;
  - now: the carve-out against LR-REC-1, LR-REC-7, Tier C and resolution (1).

  The cause is in the requirement model, not the wording. LR-REC-3 defines the trigger set as a list of sequence kinds.
  The property that actually decides whether a record is needed, recognizability by shape of each enumerated state, is
  a separate obligation in LR-REC-1, LR-REC-7 and Tier C. With two independently stated knobs that must agree, every
  local edit to one breaks the other.
- **Required correction:**
  - Derive the trigger set from the obligation. A sequence uses the record route when any of its enumerated Tier C or
    Force states cannot be recognized by shape. This rule follows from resolution (1) and LR-REC-1.
  - Reframe OQ-32 as the remaining real choices:
    - (a) the derived set;
    - (b) a wider set (every multi-file or temp-creating sequence);
    - (c) an explicit operator relaxation of resolution (1) for named states. These would be listed by name with a DEP
      row, and LR-REC-1, LR-REC-7 and Tier C would carry the matching exception.
  - Correct OQ-32's provenance sentence.
  - Make OQ-8 and OQ-32 say how they depend on each other.
  - Whatever is chosen, LR-REC-1, LR-REC-5, LR-REC-7, E02, §4 Tier C and OQ-1 must state one rule, defined once and
    cited everywhere else.
- **Regression check.**
  - For each recommended answer to OQ-32, LR-REC-1, LR-REC-7, §4 Tier C and resolution (1) must all hold for A R1 cut
    by SIGKILL and for A R1 stopped by Force.
  - No §11 recommendation may claim "no operator source" where resolution (1) applies.

**R3-2 (P2). LR-CRED-3's new environment clause covers "every child class". That contradicts accepted SCC:209,
LR-CRED-7 and LR-PROC-14, and the register does not declare it.**

- **Location:** LR-CRED-3 (:436), its Requirement and Verify cells; LR-TEST-7 (:579), which inherits the tag route.
- **Text.** "A process gwz execs (the stdio core, the background server copy, **every child class**) gets a constructed
  environment with no credential variable and **no agent socket**." The Verify cell starts each exec'd role, "every
  child class" included, with `SSH_AUTH_SOCK` and a credential variable set, and inspects the exec-time environment for
  their absence.
- **Contradictions:**
  - **SCC:209 (accepted).** "the client starts `ssh` with an allow-listed environment only: … `SSH_AUTH_SOCK` …";
    "`ssh` keeps `SSH_AUTH_SOCK` for its own login". `ssh` is a K child (LR-PROC-1). SCC:209 removes the scrub list
    only "from every `sudo` and local stdio child".
  - **LR-CRED-7 (MUST; R4, H:211-219).** "A helper runs with its own operation's captured environment". A credential
    helper (K) is the child that needs the `GCM_*` and `GIT_ASKPASS` families, which SCC:208 calls "the helper's own
    variables".
  - **LR-PROC-14 (MUST).** "No credential environment reaches a child **that does not need it**". This presupposes
    children that do need it.
  - **W children.** `git commit` with `ssh-keygen -Y` signing uses the agent socket. LR-CAN-11 and decision (ii)
    require that write to finish, and an agent-less environment fails it.
- **Impact.**
  - A correct build fails the LR-CRED-3 P row for `ssh`, helpers and agent-signed commits.
  - A build that passes it breaks SSH login, credential helpers and commit signing.
  - The clause overrides accepted SCC:209 without a DEP row, which is the defect class N-2 closed.
  - The clause was introduced by RemPlan-2's T2-3 fix; RemPlan-2's N-2 sweep (check 14) did not cover LR-CRED.
- **Required correction:**
  - Restrict the clause to the lifetime roles the row is about: the stdio core and the background server copy get a
    constructed environment with SCC:208's list and `SSH_AUTH_SOCK` removed.
  - For child classes, defer to LR-PROC-14's per-class scrub, citing SCC:209: `ssh` keeps `SSH_AUTH_SOCK`, and helpers
    get their operation's captured environment.
  - Narrow the Verify cell to match: assert absence for the lifetime roles, and for each child class the class's own
    allow-list.
- **Regression check.** Every clause of LR-CRED-3 and LR-PROC-14 is consistent with SCC:208-211 and LR-CRED-7, or has
  a DEP row. The P row passes on a build that implements SCC:209 as written.

### P3

**R3-3 (P3). OQ-16 calls twelve partly PROP rows wholly PROP, LR-PROC-2 among them.**

- **Location:** OQ-16 (:909).
- **What is wrong.** OQ-16 says its list is "sourced only by review or code evidence". Its "Partly PROP, for the named
  clause only" sentence names four rows: LR-OWN-9, LR-EXIT-6, LR-REC-5 and LR-OBS-5. By their Source cells, these
  rows are also only partly PROP:
  - LR-CAN-13 (the notice);
  - LR-EXIT-8 (the "at any step" clause);
  - LR-EXIT-9 (two clauses);
  - LR-EXIT-10 (the never-kill clause);
  - LR-LIVE-1 (the no-polling clause);
  - LR-PROC-2 (the S clause);
  - LR-PROC-13 (the W post-exit clause);
  - LR-REC-2 (the read/write split);
  - LR-REC-12 (the closed list);
  - LR-CONC-5 (the code-only part);
  - LR-TEST-6 (the power-loss model and seeded states);
  - LR-TEST-7 (the census mechanism).
- **Consequence.** The recommendation is "confirm as a set, then strike any the operator rejects". Striking
  "LR-PROC-2" as listed would strike the operator's own OQ2 no-orphan rule (sources OQ2, BR1, AM C2). The same applies
  to LR-EXIT-8's SRV-sourced shutdown order and LR-LIVE-1's OQ2 death detection.
- **Correction.** Move these twelve rows into the "Partly PROP" sentence with their clauses, or state that the per-row
  Source cell names the PROP clause and that striking acts on that clause only.
- **Check.** Every row whose Source cell scopes PROP to a clause is listed as partly PROP.

**R3-4 (P3). No requirement carries SRV:808's positive obligation.**

- **What is wrong.** DEP-9 says "the text follows SRV:808", and OQ-14 now recommends (a). But SRV:808 ("the copy
  leaves the caller's job object where the job allows it") appears only in DEP-9, NG-2 and OQ-14. No LR row requires
  the W background copy to break away, and no test checks it.
- **Consequence.** A W build that never attempts breakaway, so that its server dies with a permissive caller job,
  satisfies every MUST. NG-12 and LR-OWN-8, which give the background server's terminal as stop, idle exit or a signal,
  are contradicted only implicitly.
- **Correction.** Add the clause to LR-PROC-1's D class or to an LR-PROC row, with a W P cell (a permissive job and a
  kill-on-close job), pending OQ-14.

**R3-5 (P3). Residue of N-4.**

- **(a) §4's "K and S children" O-clean cell.** It says "ended and reaped by gwz" (:264). E37 is assigned O-clean
  (:278), yet its K child is pending and unreaped. The cell should read "ended and reaped, or pending by identity
  (LR-PROC-15)".
- **(b) E37's thread case.** E37's trigger includes "or a gwz thread" in uninterruptible sleep, but its required
  outcome and LR-PROC-15 cover only a killed child. No requirement states the outcome for the thread case.
- **(c) "gwz's exit is not held".** For a stuck thread this cannot be a gwz guarantee: a process does not finish
  exiting while one of its own threads is in uninterruptible sleep. That is a platform fact, so (ext) with an O row.
- **Correction.** Either drop the thread case from E37, or give it its own outcome (reported where possible, exit
  delayed by the OS, NG or OQ routing).

**R3-6 (P3). Minor traceability drift.**

- **NG-9's Operator cell.** It routes the pre-composition part to "OQ-16" (:548), but OQ-16 lists only LR rows. Route
  it to OQ-35 alone, or add NG-9 to a question that asks about it.
- **E20's Tier C assignment.** E20's libgit2 or Git leftover is assigned Tier C (:224, :280), but §4 defines Tier C as
  "no gwz code runs at the moment of death", and in E20 gwz is running. Either widen the Tier C definition
  ("or a write gwz does not own fails") or name that residue class in O-clean's exception list.

## 2 Invariant analysis

| Invariant | State in revision 4 |
|---|---|
| Every operator decision, standing rule, accepted clause and residual traced to an LR, NG or OQ | Holds for the D-A to D-V decisions, OD-C, OD-B, OD-D, TR1.6, TR18, IL, ACD, SBC, CP, the 2.3f decision and SD-F8. SRV:808 traces only to NG and OQ, with no positive requirement (R3-4) |
| No misstatement of a source | Fails. OQ-32's "no operator source" ignores resolution (1) (R3-1). OQ-16's "sourced only by review or code evidence" is wrong for 12 rows (R3-3) |
| No invented decision | Fails for LR-CRED-3's "every child class … no agent socket", which has no source and contradicts SCC:209 (R3-2). Everything else is PROP or OQ |
| Every departure from accepted text declared | Fails. LR-CRED-3 departs from SCC:209 with no DEP row (R3-2). DEP-1 to DEP-10 are otherwise verified at their cited lines |
| Internal coherence | Fails. The LR-REC-5 carve-out conflicts with LR-REC-1, LR-REC-7, Tier C, E02, and OQ-8 against OQ-32 (R3-1). LR-CRED-3 conflicts with LR-CRED-7 and LR-PROC-14 (R3-2). There is minor drift in §4 and E37 (R3-5) |
| IDs, tables and matrix | Pass. All references resolve, the matrix covers every ID, Verify codes are a subset of the §8 codes, NG and OQ references are complete, and the PROP list is 42 of 42 |

## 3 Risks

- **Recurrence.** R3-1 is the same seam, broken for a third time. A fourth local patch, such as adding an exception to
  LR-REC-7 only, would move the contradiction again. The fix has to derive one quantity from the other.
- **Fix-induced departures.** R3-2 came from a Testability fix (T2-3) applied to a Completeness-sensitive row. The
  departure sweep should run over every row a remediation touches, not only over the rows a finding names.
- **Citation drift.** Root P23:154 and root SCC:230 (`8aa5f800`) are not yet in this lane, and lane P23 lines after
  :154 are offset by 2 from the root. Re-check both at merge.

**Pre-commitment.** I will return GO on a revision that:
- resolves R3-1 by stating one recognition and record rule that LR-REC-1, LR-REC-5, LR-REC-7, Tier C, E02, OQ-1,
  OQ-8 and OQ-32 all cite, with OQ-32's provenance corrected;
- resolves R3-2 as stated;
- introduces no new P0-P2 on this axis.

R3-3 to R3-6 are recommended but do not block.
