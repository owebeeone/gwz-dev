**Verdict: GO**

# Lifetime requirements revision 6: Completeness axis, re-verdict round 5

## 0 Evidence

- **Object pin.** `git -C /Volumes/projects/limbo/gwz-dev-docs-req hash-object dev-docs/GwzLifetimeRequirements.md`
  returned `cbb78318e0bf9c622a1e44a92657b0e3bd96f573` at the start and the same hash at the end. That blob matches the one
  RemPlan-4 states. The object is uncommitted in lane `gwz-dev-docs-req` (HEAD `4e49eff9`) and has 939 lines.
- **Delta.** Revision 5 (blob `e9eaa6ab`) was never written to the object store (`git cat-file` fails), so I could not diff
  revision 5 against revision 6. Instead I read every row that RemPlan-4 §1 and §2 say were changed, in full, at its line in
  the object:
  - status (:5-14) and the D-V key (:42);
  - O-forced (:259) and E20 (:229);
  - LR-OWN-8 (:309);
  - LR-REC-3 (:425), LR-REC-5 (:427), LR-REC-7 (:429), LR-REC-12 (:434) and LR-REC-13 (:435);
  - §8 rows :637-639 and :658-660;
  - §9.3 (:732), §10.1 (:746) and OQ-32 (:936).

  I also read the neighbouring rows that the new entries touch: LR-REC-2, LR-REC-4, LR-REC-8, LR-OWN-2, LR-PROC-1, LR-OBS-5
  and OQ-18.
- **Peer-blind.** I did not open `GwzLifetimeRequirements-ReviewTestability-5.md`. I read `-ReviewTestability-4.md` only
  for T4-1's text, because the mandate covers how the T4-1 reading is traced.
- **Sources re-read at the cited lines.**
  - Plan:1112-1119 (D-V REC) and Plan:1121 ("Open:", which ends the D-V block). The plan is unmodified in the lane.
  - A:355-357 (R1, R2, R3) and A:372 (R18).
- **Mechanical checks.** Python over the object. The script is in the session scratchpad and each check is restated here.
  - Defined IDs: 148 LR, 28 NG, 35 OQ, 10 DEP and 42 E. Every reference resolves, and no ID is undefined.
  - Every non-TEST LR has a §8 row, with ranges expanded and the LR-PROC-1 split included. Every §8 ID is defined.
  - The §8 split matches each row's Verify codes:
    - LR-PROC-1: U S, plus a W-only P row;
    - LR-PROC-10: U P;
    - LR-PROC-12: U;
    - LR-PROC-14: U P.
  - PROP scan: 42 rows. LR-BND-3 is the known false positive. The other 41 equal OQ-16's list, with 0 missing and 0 extra.
  - Table integrity: 0 rows with the wrong cell count.
  - String counts:

    | String | Hits |
    |---|---|
    | `Tier C records` | 0 |
    | `trigger set` | 0 |
    | `M-B9` | 0 |
    | `from the intent` | 0 |
    | `power loss is a crash` | 0 (only "Power loss is a crash", in reading (i)) |
    | `intent record` | 2 (LR-REC-3, LR-REC-12) |
    | `told from the user` | 1 (LR-REC-3) |
    | `twin` | 4 (status, LR-REC-3, the §8 note, §10.1) |

## Prior-finding closure table

| ID | Sev | Re-run check | Result | Evidence |
|---|---|---|---|---|
| R4-1 | P2 | For a checkout and a workspace-root `clone`, the recovery that retires the intent record is on LR-REC-12's list. A Force-stopped checkout's record is an LR-OWN-8 entry by name | **CLOSED** | LR-REC-12 (:434) lists "the record rule's intent records (LR-REC-3: its current enumeration and any sequence the shape test adds), reached through LR-REC-2's point". Its S row ("is on the list") therefore passes the recovery function for a checkout's record and for a creation sequence's record. The K row now kills that recovery and reruns it. The Source cell attributes the entry to D-V REC, and the list's closure stays **PROP**. LR-OWN-8 (:309) names "the record rule's records left in place by a Force stop or a cut (LR-REC-3, LR-REC-5; retired by the next command's recovery, LR-REC-12)", so O-forced's "except the entries of LR-OWN-8's closed list" (:259) now admits that record. The drafter narrowed my wording, "a stop", to "a Force stop", which leaves a P3 residue: R5-1 |
| R4-2 | P3 | The contracts' records against the rule's lifetime and its "exactly when" | **OPEN, carried** (RemPlan-4 §3) | Unchanged. It does not block, per my pre-commitment |
| R4-3 | P3 | (a) O-clean against E20; (b) the process rows of the widened Tier C | **OPEN, carried** | Unchanged (:255, :258, :277, :284). R5-1 is the same seam, at the record level |
| R4-4 | P3 | D-V key; LR-REC-7; §9.3; §8 split; §8 LR-REC-3 note | **CLOSED** | Key: "Plan:1030-1121", and :1121 is the "Open:" line. LR-REC-7 (:429) reads "(checkout is in the current enumeration)". §9.3 (:732) puts a temporary in a record only where the rule keeps one, and otherwise applies the shape-plus-ownership condition. §8 is split as checked above. The §8 note (:660) names the shape test and its twin side |

## 1 Findings

There are no P0-P2 findings and nothing architectural. R5-1 to R5-3 are P3, and none of them blocks.

### P3

**R5-1 (P3). LR-OWN-8's new entry covers a record left by "a Force stop or a cut", but not a record left by a failed
gwz-owned write.**

- **Location:** LR-OWN-8 (:309); E20 (:229); LR-REC-5 (:427); O-clean (:255).
- **What is wrong.**
  - E20 and LR-REC-5 cover a gwz-owned multi-file sequence that a write failure stops, for example a workspace-root `clone`
    (a creation sequence, so it is in the enumeration) that hits ENOSPC. The next command must recognize that state "from a
    record where LR-REC-3's record rule requires one (written before the sequence began, so a full disk cannot defeat it)".
  - The record therefore outlives the operation. That operation is O-clean (event assignment, :284), and O-clean leaves
    "Nothing … except the entries of LR-OWN-8's closed list".
  - The new entry names only "a Force stop or a cut". A write failure is neither of these: E20 is Tier O, and E20's
    gwz-owned writes are not the "write that gwz does not own" that Tier C now covers.
- **Why P3.**
  - This is the record-level face of R4-3 (a), which I rated P3 in round 4. It is pre-existing, and that rating holds here.
  - My round-4 correction said "a stop or a cut". The regression check I set (a Force-stopped checkout's record is named)
    passes.
- **Correction:** write "left in place by a stop (Force, or a write failure, LR-REC-5) or a cut". Fix this together with
  R4-3 (a).

**R5-2 (P3). Reading (iii) is labelled "the consequence of (ii)", but it is a separate reading, and its provenance is not
labelled.**

- **Location:** LR-REC-3 (:425): "**Readings of the rule** (… (i) is the drafter's, (ii) the operator's) … (iii) (the
  consequence of (ii))".
- **(iii) does not follow from (ii).**
  - Reading (ii) defines the twin domain. Under that domain, A R3's unreachable loose objects and A R18's pack and
    `.git/shallow` match their twins in every observable. So under the operator's sentence read alone, these states "cannot
    be told from the user's own edits by [their] shape", and they would keep a record.
  - Reading (iii) is what exempts them. It narrows "cannot be told from" to "where the required handling differs". That is
    T4-1 correction (b), a reading distinct from T4-1 (a). It decides whether stage and fetch keep a durable record on R1's
    path.
  - The key labels (i) as the drafter's and (ii) as the operator's. It does not label (iii), and calling (iii) a
    "consequence" hides that a choice was made.
- **Why only P3.** The outcome is not invented. Accepted A:357 says R3 needs "none needed (`gc`)", and A:372 says R18
  "the next fetch completes". §10.2 classes both as harmless. So the result is sourced, and only the label is wrong.
- **Correction:** label (iii) as the operator's, if the 2026-11-04 choice covered all of T4-1's correction, or as the
  drafter's reading that accepted A R3 and A R18 support. Drop "consequence".

**R5-3 (P3). The traceability of the twin reading, and stale citation lists after R4-1.**

- **The twin reading.**
  - The only written record that the operator chose the twin reading on 2026-11-04 is `GwzLifetimeRequirements-RemPlan-4.md`,
    the drafter's own plan. D-V REC (Plan:1112-1119) is unchanged.
  - The document states this honestly ("it is not in D-V's text", :425, :746), so no source is misstated.
  - However, a remediation plan is not an operator decision record. RemPlan-4 §5 (a) also flags this.
  - Correction: at merge, add the reading to D-V's REC bullet, or to the record where the operator's decisions are kept, and
    cite that record.
- **§10.1's decision row (:746) and OQ-32 (:936).** Their "cited by" lists omit LR-REC-12 and LR-OWN-8, which now cite the
  rule.
- **LR-OBS-5 (:506).** It sorts LR-OWN-8's entries into those with a user surface, those visible to tests, and those that need
  no surface. It does not place the new entry. That entry's surface is the next command's recognition report (LR-REC-2), so
  say so. This list was already incomplete for other entries, such as the cover process and a pending K child.
- **OQ-18 (:922).** It confirms LR-OWN-8's list but names only the entries added in revision 2. Add the record-rule entry.

## 2 Invariant analysis

| Invariant | State in revision 6 |
|---|---|
| R4-1 closed: LR-REC-12's list and LR-OWN-8's entry | **Holds.** Both regression checks pass. R5-1 is a P3 residue that shares R4-3 (a)'s seam |
| Every operator decision traced, the T4-1 reading included | **Holds, with a weak record** (R5-3). The rule's sentence is now verbatim from D-V REC. I compared it word for word with Plan:1116-1119. "No named state is relaxed" is also verbatim. The glosses are moved after the rule and labelled. Reading (ii) is attributed to the operator and its record is named. Reading (iii)'s label is wrong (R5-2) |
| No misstated source | **Holds.** A:355, :357 and :372 match the R1, R3 and R18 claims. The D-V range and REC sub-key match the plan. §10.2's "harmless" matches |
| No invented decision | **Holds.** Reading (iii)'s outcome is backed by accepted A R3 and R18 (R5-2 concerns only its label). The new LR-REC-12 entry is sourced to D-V REC, and the list's closure stays PROP |
| IDs, tables and matrix | **Pass** on every mechanical check, and the §8 split matches the Verify cells |

**Closed-list sweep (my round-4 risk).** I re-read LR-OWN-2 and LR-PROC-1 against the record rule.

- **LR-OWN-2** concerns in-process reachability (globals and ambient lookups). The rule's records are durable files reached
  through LR-REC-2's point, so the two do not conflict.
- **LR-PROC-1** refuses an undeclared spawn. The rule's record is written by gwz "before it hands the sequence to libgit2 or a
  Git child", and that child keeps its declared class. There is no conflict here either.
- **LR-REC-12's S row.** The S row admits the clearing of a record when its sequence finishes through its allow-list ("why it
  is not recovery"). The rule therefore needs no further entry.

## 3 Risks

- **Citation drift.**
  - Root P23:154 and SCC:230 (`8aa5f800`).
  - The lane-only Plan:1112-1121, plus the twin reading's record if it is added there (R5-3).
  - Re-check all of these at merge.
- **Unspiked platform facts in gating cells.** As RemPlan-4 §3 says, do not freeze the matrix before the LR-TEST-10 spike.
  This was not re-reviewed on this axis this round.
- **Carried P3s.** R4-2 and R4-3 (with R5-1) are the remaining coherence seams around the record rule. Fix them before a design
  cites LR-REC-3's enumeration as authority for the existing contracts' records.

**Pre-commitment honoured.** R4-1 is fixed as I stated it, and there is no new P0-P2 on this axis. Verdict: GO.
