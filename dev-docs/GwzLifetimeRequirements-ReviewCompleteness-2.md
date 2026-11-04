**Verdict: NO-GO**

# Lifetime requirements revision 3: Completeness axis, re-verdict round 2

## 0 Evidence

- **Object pin.** `git -C /Volumes/projects/limbo/gwz-dev-docs-req hash-object dev-docs/GwzLifetimeRequirements.md`
  returned `ef534435f63eb52d6b0ba17aaf6b86632c267907` at the start and the same hash at the end. The object did not
  change during the review. It is uncommitted in lane `gwz-dev-docs-req` (HEAD `a874eddc`). The round-1 object was
  blob `34680949`, read here with `git show a874eddc:dev-docs/GwzLifetimeRequirements.md`.
- **Peer-blind.** I did not open `GwzLifetimeRequirements-ReviewTestability-2.md`, and it does not exist in the lane.
  I read the round-1 Testability report only through the RemPlan's T rows.
- **Operator input.** The relayed request, "go with (a), canonical path", is the operator's 2.3f round-2 Code P2-1
  decision. I read it at root `gwz-dev` commit `8aa5f800`, `dev-docs/GwzServerCore-Step2.3-Plan.md:154`, together with
  the SCC line that commit adds (§4.2, "One server per directory").
- **Sources re-read at the cited lines:**
  - Plan: :164-181, :278-290 and D-V :1030-1110.
  - A: :93-100, :107, :136-139, :232-237, :350-372 and :384.
  - B: :126-132, :146, :219-221, :271, :326, :340-343 and :361.
  - CT: :668-673, :683 and :704-713.
  - SD: :56, :428-440, :505-514 and :543-548.
  - SCC: :24-25, :81, :101-104, :210-211, :227-241 and :285.
  - SRV (parked, adopted): :1-12, :531-534, :596-616, :803-810, :1059, :1078-1086, :1107, :1112-1118 and :1210-1226.
  - TR18: D5, D14, D18, D24-D26, D31, D32, D42, D44, D45, D47 and D52.
  - IL: :139 and :292-304. CP: :9, :229-231, :266-270 and :326-329. ACD: :628-646.
  - C and D (lane `gwz-dev-docs-cd` at `a977d241`): C :124, :164 and :228-234; D OD-D1 to OD-D18 (:308-325); CS S-P3-4
    (:189).
  - Code: `gwz-core/src/session_host/served.rs:265-298`, `transport_host/connection_host.rs:8-55` and
    `gwz-cli/src/help.rs:40-50`.
- **Mechanical checks** (Python over the object; scripts in the session scratchpad):
  - 148 LR (140 MUST), 28 NG, 34 OQ, 42 E and 7 DEP IDs are defined. Every LR, NG, OQ and DEP reference resolves.
  - Every NG appears in §10, every NG is referenced outside §6, and every OQ is referenced outside §11.
  - Every non-TEST LR ID has a §8 row, with ranges expanded.
  - The PROP scan finds 41 rows carrying "PROP". OQ-16 lists 39 of them. LR-BND-3's hit is a false positive (its text
    mentions "rows marked PROP"). **LR-EXIT-6 is genuinely PROP and missing from OQ-16** (N-6).
  - Row Verify codes against §8 cells: details in N-7.
  - The six process-globals allowlists hold 19 `debt` entries (gwz-core 12, gwz-transport 1, gwz-py 4, gwz-cli 1,
    gwz-git 1). Each is named in OQ-22. The 25 `permanent` entries fall under LR-OWN-2 (d).
  - "M-B9": 0 occurrences. "Clocks keep running": 0 occurrences.

## Prior-finding closure table

Each round-1 check was re-run against blob `ef534435`.

| ID | Sev | Re-run check | Result | Evidence |
|---|---|---|---|---|
| C-1 | P2 | No "M-B9". LR-PROC-7, OQ-2 and OQ-27 agree with B:128-129, B21 and B:343 | **CLOSED** | LR-PROC-7 (:379) puts K and W in a job and S in none, citing B:128-129 and B21 (B:326). OQ-2 (:880) covers Unix only. OQ-27 (:905) confirms B:343 ("Windows stays (a) under every choice"). B:343 reads as cited |
| C-2 | P2 | LR-BND-2 names CT:707-708's two transitions and TR18 D42/D45. No clause says clocks run through a helper prompt | **CLOSED** | LR-BND-2 (:445) lists four transitions: SetupClock, D19, CP's local-wait pause plus CP:9, and producer-busy. B-PRODUCER (600 s) is at :476. The agent confirmation runs in Network: D5 (b) (TR18:39) names `ssh-add -c` as the same case. CT:707-708 read as cited |
| C-3 | P2 | Every departure (a)-(g) is in a register, with an authority or an OQ | **CLOSED** (for (a)-(g)) | DEP-1 to DEP-7 (:88-96). The register is new, so its completeness was attacked separately: N-2 finds four more undeclared departures |
| C-4 | P2 | Run the S check against the allowlists. Every entry is allowed by a decision or listed in OQ-22 | **CLOSED** | LR-OWN-2 (d) admits SD §5.7's `permanent` class. My own count of the six allowlists matches OQ-22's 19 `debt` entries name by name. `Hub` is cited to CP:270 |
| C-5 | P2 | SD's admission thread appears in §3, in LR-BND-1 or §6, and in LR-OWN-8 | **CLOSED** | E36 (:239), NG-26 (:561), LR-OWN-8 (:298), LR-BND-1's §6 pointer (:444), R-THR (:173) and DEP-6 |
| C-6 | P2 | §10.1 contains IL decision 4. LR-LIVE-1's sources support each clause | **CLOSED** | The no-polling clause is PROP (:359). NG-27 (:562) and the §10.1 row (:782) are present. OQ-17 is extended. IL:299-300 and IL:139 read as cited |
| C-7 | P2 | The intent clause traces to a source, or is PROP and in §11 | **CLOSED** for LR-OWN-3 and L3 | LR-OWN-3 (:293) and L3 (:148) now defer to LR-REC-3's trigger set, and OQ-32 asks about it. The narrowed trigger set now contradicts five places that still require "recorded" partial state: N-1 |
| C-8 | P2 | No normative clause rests only on C, D, AM or review sources without a PROP mark | **CLOSED** for the five named clauses | LR-EXIT-8, -9, -10, LR-PROC-2 (S clause) and LR-LIVE-1 are marked PROP. LR-OWN-9 has a new unmarked clause attributed to an operator decision that does not contain it: N-3 |
| C-9 | P2 | §4's Report row and LR-OBS-1 are achievable for E02 under `--server`. E18's tier cites NG-23 | **CLOSED** | LR-OBS-10 (:500); §4 Report row (:272); E05 (:194), E17b (:212) and E18-core (:214) cite NG-23 |
| C-10 | P3 | The §10.1 row matches SD §13 | **CLOSED** | Source key :56 and the §10.1 row (:796) say "capability bit" (SD:548). LR-CAN-13 was added |
| C-11 | P3 | §1, §2 and §3 list the pump, eviction and the three events | **CLOSED** | PMP (:125); R-SESS eviction (:163); E37-E40 (:240-243) |
| C-12 | P3 | Every TR18 spawn, inheritance, clock or close decision is in §10.1 | **CLOSED** | D24, D42/D45, D47 and D52 at :775-779. SBC Q5 at :784. TR18 lines match |
| C-13 | P3 | Every NG and OQ is referenced outside §11, and OQ-16 lists the PROP IDs | **PARTIAL** | The NG and OQ references pass. OQ-16 omits PROP row LR-EXIT-6 and the PROP bound B-TURN / B-LINGER (:483). §0 says each PROP "is listed in §11": N-6 |
| C-14 | P3 | Every production spawn site maps to a class or a named exemption | **CLOSED** | The cover process is in LR-OWN-8 and R-PROC. The `gwz hook` exemption is in LR-PROC-1 (:373). `help.rs:46` is a clap `Command`, not a spawn |
| C-15 | P3 | R8 appears in §10.1 | **CLOSED** | §10.1 :793. LR-TEST-11 adds the two-machine run (:579) |

## 1 Findings

Every finding below is new. N-1 to N-4 are P2; N-5 to N-7 are P3.

### P2

**N-1 (P2). The narrowed record trigger set contradicts the requirements that Force's partial state be "recorded".**
- **Location:**
  - LR-REC-3 "Trigger set" (:414).
  - LR-REC-5 (:416).
  - §4, the O-forced definition (:250) and the "Partial across members" row, C column (:270).
  - E02 R-TREE (:191) and E20 (:220).
  - OQ-1, option (b) (:879).
- **Violated invariant.** The document must be internally coherent. The C-7 remediation must remove the untraced
  per-sequence record mandate everywhere, not only in LR-OWN-3 and L3.
- **State sequence.**
  1. LR-REC-3 now says: "Records are required for creation sequences (LR-REC-13) and for the sequences whose
     existing contracts keep one … No other sequence is required to use the record route" (OQ-32 recommends exactly
     that).
  2. LR-REC-5 (MUST, not PROP) says partial state left by Force "is recorded and handled by the same recovery path".
  3. §4 O-forced allows partial state "only as **recorded** partial state".
  4. E02 says "That state is recorded". E20 says "a stopped multi-file sequence is recorded".
  5. §4's C column says partial across members is "recognized via the intent record".
  6. OQ-1 (b) completes or rolls back "from the intent".
  7. Take A's R1, a checkout stopped at a Force safe point. A recognizes it by "the notice; D" (A:355), with no
     record. A checkout is neither a creation sequence nor a contract-backed one, so under LR-REC-3 it needs no record,
     and under LR-REC-5 it must be recorded. A cross-member pull is the same case.
- **Impact.**
  - A design cannot satisfy both rows. Either it builds the per-sequence journal that OQ-32 says has no operator
    source (the C-7 defect back again), or it violates LR-REC-5 and §4.
  - OQ-1 and OQ-32 present the operator with two questions that assume different answers to the same question.
  - The RemPlan's check 8 counted :191 and :220 as consistent.
- **Required correction:**
  - Restate LR-REC-5, §4 O-forced, E02 and E20 as "recognized by the next command (from a record where LR-REC-3's
    trigger set applies, otherwise by shape) and reported", or mark them PROP and tie them to OQ-32.
  - Change §4 :270's C cell to "recognized per member (LR-REC-1)".
  - Reword OQ-1 (b) to "from a record or shape".
  - Source LR-REC-5's "recorded" clause, or drop it: resolution (1) and A R1-R3 say "recognizer", not "record".
- **Regression check.**
  - `grep -n "recorded\|intent record\|from the intent"` returns no normative cell that requires a record outside
    LR-REC-3's trigger set.
  - LR-REC-5, §4 and OQ-1 agree with LR-REC-3 and OQ-32.

**N-2 (P2). The departures register is incomplete. Two new recommendations misstate the accepted text they depart
from.**
- **Location:** §0 register (:85-96); LR-EXIT-1 (:328); LR-EXIT-4 (:331); OQ-13 (:891); NG-2 (:537); OQ-14 (:892);
  LR-PROC-14 (:386); LR-EXIT-9 (:336); E14 (:208).
- **Violated invariant.** §0: "Where a requirement differs from the text of an accepted document, the difference is
  declared here with its authority." No misstatement of a source.
- **Instances:**
  - **(a) `hook` and `claude-code-setup`.**
    - Accepted A installs the interrupt module "for every other command but `hook` and `claude-code-setup` (R12)"
      (A:97). Its exit status excludes them (A:235-236; A §8 row 8, :384).
    - LR-EXIT-1 and LR-EXIT-4 cover "every command, subject to OQ-13". This is a departure from accepted A, and the
      register does not declare it.
    - OQ-13's new recommendation says "A and DS/DC dispute D's premise for (b)". A does not dispute it: A took (b).
      The operator is told that the accepted design supports the opposite of what it says.
  - **(b) W background server and the caller's job.**
    - The adopted server design says "The copy leaves the caller's job object where the job allows it" (SRV:807,
      the background-start row). SCC does not amend this.
    - NG-2 makes the server die with the caller's kill-on-close job. OQ-14 recommends (b), "no source asks for it".
      SRV:807 asks for (a).
    - CS S-P3-4 (:189) is the review that flagged C for dropping this rule without recording it. The requirements
      document repeats the same undeclared drop.
  - **(c) The local stdio child's environment.**
    - SRV:1113: "the child gets the client's environment". SCC:210 amends this only by refusing credential variables.
    - LR-PROC-14: "The served core and its children never receive the client's environment." Read literally, that
      forces a constructed environment for the local stdio core. The register does not declare it.
  - **(d) The stdio child and CTRL_BREAK.**
    - SRV:809 and :1107: the W stdio child "also ignores `CTRL_BREAK_EVENT`".
    - LR-EXIT-9 and E14 make a CTRL_BREAK that reaches it carrier loss, exiting 130. The clause is PROP (C evidence;
      C :231-232 proposes the amendment), but DEP-3 cites only SRV:1116 and :1082, so the SRV:809/:1107 departure is
      undeclared.
- **Impact.**
  - The operator may approve OQ-13 and OQ-14 "in one pass" (RemPlan §7) on recommendations that misstate the accepted
    or adopted text.
  - Three accepted or adopted clauses are overridden silently. The round-1 C-3 defect class survives beyond the seven
    instances I listed.
- **Required correction:**
  - Add DEP-8 (A:97, :235, row 8 against LR-EXIT-1 and -4, routed to OQ-13).
  - Add DEP-9 (SRV:807 against NG-2, routed to OQ-14).
  - Add DEP-10 (SRV:1113 against LR-PROC-14). Either restate LR-PROC-14 as "never over the carrier; a local stdio
    child inherits the client's process environment with SCC §3.8's list and `SSH_AUTH_SOCK` removed", or route it.
  - Extend DEP-3 with SRV:809 and :1107.
  - Correct OQ-13's recommendation ("A excludes them, as (b)").
  - Correct OQ-14's recommendation ("SRV:807 asks for (a)").
- **Regression check.**
  - A sweep of SRV's adopted platform table (:803-810), §6 and §15, and of A §3.1 and §3.6, against LR-EXIT, LR-PROC
    and NG: every difference has a DEP row.
  - No recommendation's stated reason contradicts the text it cites.

**N-3 (P2). LR-OWN-9 attributes to the operator's 2.3f decision a clause the decision does not contain, and the
transcription omits the decision's new start lock.**
- **Location:** LR-OWN-9 (:299); LR-OWN-10 (:300); §2 R-SRV (:170); LR-OWN-8 (:298); LR-CLEAN-8 (:401).
- **Violated invariant.** No untraced or misattributed decision. Every operator-decided resource appears in §2.
- **Evidence.**
  - Root P23:154 (`8aa5f800`): "filesystem identity keys **only the member lock**". The server "also holds an
    exclusive start lock keyed by `workspace_identity` in the per-user directory for its whole life"
    (`id-<identity key>.lock`, holding the server's address).
  - LR-OWN-9 says the identity function "keys the member lock **and the detached-worker entries**". Its source is the
    decision plus SCC:25, and only the "case clause" is PROP.
  - No source keys the detached-worker record by identity. SCC:24 and :104 name the record but not its key. Revision
    2 had this clause under a whole-row PROP derived from code (`served.rs:269-296`). Revision 3 moved it under the
    operator's name.
  - The start lock (`id-<identity key>.lock`) is a new lifetime resource. It is held for the server's life, persists
    as a file, carries an address record, and is not in R-SRV, LR-CLEAN-8 or LR-OBS-5.
  - LR-OWN-10's source cell omits the decision it now transcribes ("a second spelling … refuses as held").
- **Impact.**
  - A clause the operator did not decide reaches the design as decided, against the standing scope rule.
  - A crash leaves an `id-*.lock` file holding a stale address that no row says is "not stale" or recognizes. The
    census (LR-TEST-7) has no expected-set entry for it.
- **Required correction:**
  - Mark the detached-worker keying clause PROP (code evidence `served.rs:269-296`), or drop it.
  - Name the start lock in LR-OWN-9 and in R-SRV, with Tier C: "advisory lock released by the OS; the file stays
    and is not stale; its recorded address is never trusted without the stale-socket proof".
  - Cite root P23:154 in LR-OWN-10.
- **Regression check.**
  - Every clause of LR-OWN-9 and LR-OWN-10 is either in P23:154 or the SCC §4.2 line, or marked PROP.
  - R-SRV lists `id-<identity key>.lock`.

**N-4 (P2). The new E37 / LR-PROC-15 outcome contradicts LR-PROC-2's closed exceptions, LR-OWN-8's closed list and
§4's O-clean.**
- **Location:** E37 (:240); §4 event assignment (:278, E37 assigned O-clean); LR-PROC-15 (:387); LR-PROC-2 (:374);
  LR-OWN-8 (:298); §4 O-clean (:249); LR-TEST-7 (:575).
- **State sequence.**
  1. A K child is in uninterruptible sleep, and gwz sends SIGKILL.
  2. The reap does not complete.
  3. LR-PROC-15: at B-CLEAN the child is retained pending, and "gwz's exit is not held".
  4. gwz exits while the child process still exists.
- **Contradictions:**
  - LR-PROC-2 (MUST) requires each K child to end "by gwz before it exits, or after it by the cover". Its "only
    exceptions are NG-1, NG-7, NG-15, NG-17 and NG-18". E37 is not one of them.
  - LR-OWN-8's closed list of things that may outlive their scope has no entry for this child. "Retained cleanup …
    until confirmed or process exit" ends at gwz's exit, while the child does not.
  - E37 is O-clean, whose definition says "Nothing is left behind".
  - LR-TEST-7's census after the E37 test would find the marker process, so the specified test fails against its own
    requirement.
- **Impact.** Three MUSTs cannot hold together for a case the revision itself added.
- **Required correction.** Do all of the following:
  - add the killed-but-unreaped child to LR-OWN-8 (owner: the OS; terminal: the kill completes when the call
    returns; surface: the pending report);
  - add E37 as an exception in LR-PROC-2, or as an NG row with OQ routing (LR-PROC-15 is PROP);
  - move E37 out of O-clean (O-forced, or "O with a pending residue");
  - give the census an exemption for an E37 row.
- **Regression check.** For E37, LR-PROC-2, LR-OWN-8, §4 and LR-TEST-7 are jointly satisfiable.

### P3

**N-5 (P3). Stale cross-references after renumbering.**
- §9.3's "Remain as non-goals | NG-1 to NG-25" (:716) omits NG-26, NG-27 and NG-28.
- E23 (:225) still says "close owners within B-CLEAN", the revision-2 text that DEP-2 retired. LR-EMB-3 now joins
  each bridge pump within B-CLOSEW, and E23 does not say so.
- **Consequence.** A reader of §3 or §9 gets the revision-2 bound and an incomplete NG list.
- **Correction.** "NG-1 to NG-28". E23: "close the `HostOwner` within B-CLEAN and join each pump within B-CLOSEW
  (LR-EMB-3)".
- **Check.** A grep for ID ranges matches the defined maxima. E23 equals LR-EMB-3.

**N-6 (P3). OQ-16 omits two PROP items (C-13 partial).**
- LR-EXIT-6 carries "B-OUT behaviour **PROP**" (:333). The RemPlan's check 17 acknowledges that it carries PROP, but
  it is not in OQ-16's list, so RemPlan check 6 ("39 of 39") is wrong.
- The bounds row B-TURN / B-LINGER (:483) is PROP, and LR-BND-3 makes it non-normative until confirmed, but no §11
  row asks for it.
- **Consequence.** §0's "Each PROP … is listed in §11" is false, and the operator is never asked about these two
  items.
- **Correction.** Add LR-EXIT-6 and B-TURN / B-LINGER to OQ-16.
- **Check.** The PROP scan, which also covers the bounds table, equals OQ-16's list.

**N-7 (P3). Verify cells added in this revision are missing from the §8 matrix.**
- **Instances.** In each case the row's Verify cell names a code that its §8 cells lack:
  - LR-CLEAN-2 (S; matrix U).
  - LR-CRED-2 (new secret-types S; matrix U).
  - LR-OWN-9 (S, "no second keying site"; matrix U P O).
  - LR-REC-12 (S, recovery-entry marker check; matrix K P).
- **Earlier instances, for the record:**
  - LR-OWN-7 (X; matrix P).
  - LR-PROC-10 and LR-PROC-14 (P; matrix U S).
  - LR-CONC-3 (U; matrix P).
- **Consequence.** LR-TEST-1's matrix audit, which maps each MUST to tests per platform from §8, would not schedule
  these S checks.
- **Correction.** Add the codes to §8.
- **Check.** For every row, the Verify codes are a subset of the §8 codes, apart from E and Z, which are supplements.

**Observation (not a finding).** §11's new Recommendation column states no provenance in the document itself; only
the RemPlan says the recommendations are the drafter's. N-2 shows two of them misstate their sources, and OQ-16
("confirm as a set") sits beside OQ-11 ("decide one by one … D's set bundles accepts"). A one-line note under the §11
heading ("recommendations are the drafter's; none is an operator decision") would close it.

No finding is an architectural root cause. N-1 is the residue of a question already routed (OQ-32), not a new
structural defect.

## 2 Invariant analysis

| Invariant | State in revision 3 |
|---|---|
| Every operator decision traced to a requirement, NG or OQ | Holds for D-A to D-V, OD-C1 to C9, OD-B1 to B4, OD-D1 to D18, TR1.6, TR18 (13 lifetime decisions), IL 1-6, ACD, SBC, CP and the 2.3f decision. The 2.3f transcription over-claims (N-3) |
| No misstatement of a source | Fails. OQ-13 misstates A (N-2 (a)); OQ-14 misstates SRV:807 (N-2 (b)); LR-OWN-9 attributes a clause to P23:154 (N-3) |
| No invented decision | Holds in the PROP and OQ machinery, except N-3's misattribution. Recommendations are not marked as the drafter's (observation) |
| Every departure from accepted text declared | Fails. Four more undeclared departures (N-2) |
| Internal coherence of requirements | Fails. The record trigger set against "recorded" partial state (N-1); E37 against LR-PROC-2, LR-OWN-8 and O-clean (N-4) |
| IDs, tables and matrix | IDs resolve; NG and OQ references are complete; §8 covers every ID; minor drift (N-5, N-7) |
| PROP rows listed in §11 | 39 of 41 (N-6) |

## 3 Risks

- **Citation drift.** The document cites root P23:154 at `8aa5f800`, which this lane does not have yet. If the root
  plan moves again before the lane merges, that citation drifts. Re-check it at merge.
- **Recommendation-driven approval.** The RemPlan invites the operator to accept 34 recommendations in one pass.
  Any recommendation that misstates its source (N-2) becomes a de facto override of accepted text. I would require a
  source re-check of every recommendation before it is put to the operator, not only those in N-2.
- **OQ-32 versus OQ-1 coupling.** Until N-1 is fixed, an operator answer to OQ-32 (a) silently empties LR-REC-5 and
  OQ-1 (b) of meaning.
- **Unstable evidence base.** C and D are still r1 NO-GO. Several PROP clauses (LR-EXIT-9's CTRL_BREAK clause, the
  LR-EXIT-8 "any step" clause) rest on C's proposed amendments of SRV and SCC.

Pre-commitment: I will return GO on a revision that resolves N-1 to N-4 as stated and introduces no new P0-P2 on this
axis. N-5 to N-7 are recommended, not blocking.
