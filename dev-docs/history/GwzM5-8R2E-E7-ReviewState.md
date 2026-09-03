# R2-E E7.1 — settled-gate review, STATE axis (dual #2)

**Axis:** STATE (records: freeze, evidence, plan ledger, determinations, pins).
Peer-blind — the Code axis's report not seen, not sought.
**Date:** 2026-08-29.
**Object:** the R2-E record set in `gwz-dev/dev-docs/` as of 2026-08-29, for the
settled tree gwz-core main `be693bd` (tag `v0.11.1`). The tree was read only to
verify records' claims; code soundness is the Code axis's.
**Discipline held:** no commit, push, tag, or edit anywhere except this file.
No cargo probe was needed; every tree check was grep/read against the pristine
checkout at `be693bd` (verified: `git rev-parse HEAD` = `be693bd…`, tag
`v0.11.1` on it, clean).

**Sources examined in full or at the named sections:** `GwzM5-8R2E-Plan.md`
(§0-§5, incl. every §1.1 dated row); `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` +
`-E02b-DRAFT.md` (statuses, §1.5/B-4, §5, §6, §7.6-§7.8, §8 disposition tables,
§10/Appendix B); `GwzM5-8R2DInterfaceFreeze.md` (the three §3.5 activation
records, all eleven E0 annotations, the §3.5 inventory addendum, the §4.4
T-B′ annotation and E3-landing row); `GwzM5-8R4bG-Evidence.md` §12.3/§12.4/
§12.6/§12.7/§12.8/§12.9(c)(d)(e); `GwzM5-8R4bR2ConsumerCheckpoint.md` §10
(:272-280) and §11; the five interior reviews (E1, E2 incl. Round 2, E3 incl.
Round 2, E5, E6) and `GwzConfIntegrity-Review.md`/`-Review-2.md`;
`GwzM5-8R2E-E01ReachTraces.md`; `GwzM5-8R2DSettledTuple.md` §4.8/§11.1-§11.3;
`CurrentProgramCheckpoint.md` R2-E era (:1636-end); gwz-core
`scripts/checks/run_r4bg_aggregate_gates.py` (`_fault_count` and battery
tables), `scripts/checks/check_checked_artifact_boundaries.py` (the three
Python inventories incl. the O13 pin), and
`src/checked_artifact/interface_tests/fault_expected_keys.rs` +
`capability_permit.rs`. Line numbers below are dated conveniences, as of
2026-08-29.

---

## Verdict

**GO-WITH-CONDITIONS.**

The written state of R2-E is coherent, complete at its record-of-record homes,
and truthful; every prescribed ledger close form matches reality as of
`be693bd` — **no O-row's close form fails**. The pin/provenance chain is
internally consistent end to end and the linux DERIVED→MEASURED conversion
E7.2 will perform is sound on the release-verify execution. The conditions are
all record acts E7.2 can perform in the acceptance itself; the one [P2] is a
scope clause the corpus books as recorded but which is written nowhere the
deferred E4 will read.

Finding counts: **0 P0 · 0 P1 · 1 P2 · 7 P3**, plus dispositions of the
state-side E7-dual queue items (§Queue below).

---

## Numbered findings

### [P2 F1] — the [R2-P3-1] "settled ≠ barriered" E4 scope clause is booked as recorded but is written nowhere E4 will read it

The E2 review round 2 (`GwzM5-8R2E-E2-Review.md`, "E7-dual items", "Carries an
**E4 scope clause**: E4's consumer must not rely on the implication"; the
finding body at "a settled barrier ordinal does not prove its target parent's
dirents were ever ordered", :1457-1474 as of 2026-08-29) and the checkpoint's
E2 landing record ("[R2-P3-1] settled-does-not-imply-barriered (an E4 scope
clause + E7-dual item)") both describe the item as carrying an E4 scope
clause. **Zero occurrences of that clause exist in `GwzM5-8R2E-Plan.md`
(§1.1 or the §3 E4 gate note), the freeze §3.5 barrier record, or the
amendment pair** (grep over all four documents for the implication in any
phrasing: no hit outside the review and the checkpoint summary). E4 is
re-scheduled to a post-R2-F resumption whose builder will be briefed from the
plan's E4 section and the freeze — precisely the two homes that do not carry
the clause. The E6 landing's F-7 annotation says the review-debt ledger
empties at E7; it cannot empty over a clause that exists only inside the
review that minted it.

**Condition on the acceptance:** E7.2 writes the clause as a dated annotation
at the plan's §3 E4 gate note (the sanctioned in-place mechanism, beside the
existing E2.2-ordering sentence) — one sentence of the E2 review's own form:
*a settled barrier ordinal does not imply its target parent's dirents were
ordered; E4 consumers must not rely on the implication ([R2-P3-1], E2 review
round 2)* — and the acceptance record carries the dual's disposition of the
underlying contract fork (my determination at §Queue item 1 below).

### [P3 F2] — [R2-P3-3]'s one-word wording cure was never folded, and the fold window it was assigned has since passed

`platform.rs:489` (as of `be693bd`) still reads "the survey is two
`symlink_metadata` calls that always answer the …" — the cost claim the E2
review found imprecise (`leaf_is_resident` is a full bounded observation) with
the disposition "fold it into whichever commit next touches `platform.rs`".
E6.2b then moved the `checked_artifact/platform.rs` protected tree (the
second dated platform.rs digest re-pin) without folding it. Not a truth
defect in any ledger row; it is an un-executed landing-optional cure whose
assigned carrier ("next commit touching platform.rs") has fired once without
it. **Disposition (state axis):** close it at E7.2 as a named accepted
residual with the carrier restated — the next production train that moves the
platform.rs tree (E4 resumption or the R2-F lane) folds the one-word fix and
its digest re-pin; do NOT execute it inside E7.2 (a comment-only digest move
at the acceptance would be a worse trade than the residual).

### [P3 F3] — the superseded "10 registry rows" denominator stands un-bracketed in the controlling addendum; the correction lives at the resolution homes

`GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` §8's [P2-2] cure (":1433 as of
2026-08-29", "**E5.1 owes 10 registry rows**") carries no E5-landing bracket,
while the same document takes dated in-place brackets elsewhere (B-4 at §1.5;
§6.2's FIVE-inventory correction). The 9+1 partition is recorded at: plan
§1.1's O8 row (dated E5-landing bracket, "9 registry rows + 1 clause-cited
disposition … ratified by the E5 interior review (adjudication A)"); Evidence
§12.9(e) bullet 1, which names the addendum statement explicitly ("E0.2b §8
[P2-2]'s '10 registry rows' is therefore right as a shape count and off by
one as a registry-row count, for (c)'s own reason"); and the E5 review
(adjudication A, verdict "RATIFIED (9 registry rows + 1 clause-cited
disposition)"). Every denominator statement at a record-of-record home now
agrees; only the addendum is silent about its own supersession. Acceptable
under the corpus's resolution-index discipline (the same pattern as the
citation-drift note: dated records left as written, resolved at the index).
**Condition-shaped ask:** the O8 close in the acceptance cites **Evidence
§12.9(e)** as the denominator authority, so no future reader takes the
addendum's §8 figure as the executed form.

### [P3 F4] — OPEN-B2/B3/B7/B8's register rows carry no closure marks; their answers are scattered across the E2 review, one B-4 bracket, and a CI record

The addendum's §10 and Appendix B still row all four as open, owner E2.1. The
answers exist and are executed: OPEN-B8 answered positive on the conjunction
(the dated B-4 bracket at addendum §1.5, "[CORRECTED 2026-08-27, E2 review
ratified at the lane-owner escalation …]", which consumes OPEN-B8's own
evidence row; E2 review "OPEN-B8 — answered positive"); OPEN-B2 "no widening
— PASS" (E2 review, with the expiry condition attached to the
caller-count no-move confirmation); OPEN-B3 "PASS on placement" with the
honest limit filed as E2 [P3-1]; OPEN-B7 discharged by the first native
Windows execution of the roaming arm (checkpoint E2 CI closure, Windows
matrix 33044834867 green, "WINDOWS-ARM-OWED discharged"). **Condition-shaped
ask:** E7.2's acceptance record closes the four rows by citation, one line
each, so the addendum's register is not the last word.

### [P3 F5] — the ten-writer-rows native-execution "ledger entry" has no entry at its prescribed home; the discharge facts live only in the checkpoint's CI-closure lines

E0.2 §7.3 deferred §11.3 row 4 with terms "the entry is owed at the next
matrix dispatch … the record goes in the classification ledger, in the form
the Step-4.2 record used (freeze `:836-838`), and E7.2's three-platform
acceptance re-verifies it." No dated entry in the Step-4.2 form exists in the
freeze or the settled tuple (the tuple row still reads "ledger entry owed at
the next matrix dispatch", :665 as of 2026-08-29); no document named "the
classification ledger" exists beyond these three cross-references. The
discharge is real and recorded — E1-tip CI 4/4 green (Windows 33039208104,
Platform 33039209950, after the kill-race rerun), re-confirmed at 1d50e59 and
c11c5ef — but only in checkpoint prose. **Condition on the acceptance:** E7.2
writes the consolidated dated entry (the run ids above, the Step-4.2 record's
form) into the acceptance record and marks the tuple row resolved by that
citation, discharging §7.3's own "E7.2 re-verifies" clause at the same time.

### [P3 F6] — two E1-review residuals must ride the acceptance's residual register by name

(a) E1 F1 (the 16 KiB alias-retirement bound stated twice with a prose-only
coupling) is still prose-only at `be693bd`
(`namespace_mutation.rs:983` names `CleanupRetirementDestination::
source_bound` in a doc comment; no parameter threading landed in E2/E3/E5/E6
— none of them touched that seam). Its queued carrier is "the next train
touching those files", and the 16 KiB bound is separately named an E4 input
by the E1 landing record — so the acceptance must carry it with the E4
resumption, or it will be re-discovered mid-E4. (b) E1 F2's corrected
cohesion ranking ("fifth largest", not third) was to be "carried into the
E1.3 landing record"; the landing record repeats no false ranking but never
carries the correction — close by citing the review's §2.1 correction in the
acceptance. Both are record acts only.

### [P3 F7] — the corpus tier-2 carrier strings name one of the two encumbrances

All eight `byte-preserved-v0-origin` rows' tier-2 carrier strings
(`gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json` archive_corpus)
name the E5-landing ratification and the R2-F relocation dependency —
verbatim, consistently, all eight — and Evidence §12.4's Table B header says
"tier 2 is owed on all ten, carrier E4.4, R2-F-encumbered." Neither names the
**second** encumbrance: tier 2 as written in amendment §6.3 is NOT
SATISFIABLE and a comparable sub-surface must be minted **by amendment with
dual review** before E4.4 executes it. That precondition is recorded at the
carrier's charter home (plan §3 E4.4, determination (2)), in the E5 review
(adjudication G(iii)), and in the checkpoint — which is where E4.4's
implementer will be briefed from, so nothing contradicts and nothing is
false. **Condition-shaped ask:** the O8 close states the two-part encumbrance
in one clause (R2-F-gated AND sub-surface-by-amendment-first), so the corpus
string's single-encumbrance reading cannot be taken as complete.

### [P3 F8] — O3's prescribed close wording is future-facted; the acceptance should use the tense the fallback's own text supports

Plan §1.1's O3 row prescribes "E7 records O3 as *discharged by R2-F's
relocation, re-owned there*". At close time the relocation has **not**
landed, so nothing has discharged O3 yet; the §7.8(a) text carries the
precise form — "O3 is **discharged by R2-F's package and re-owned**, not
stalled", i.e. the package discharges it **on landing**. The acceptance's O3
row should read: *RE-OWNED to R2-F's quarantine/relocation package, which
discharges O3 directly on landing (§7.6.2's single-owner fact:
`policy.rs:33-42` defines the legacy private parent, `observation.rs:93` its
only consumer); E4.7 remains this lane's stronger full-retirement mechanism
at the E4 resumption; OPEN-R1 rides with the package (blocking for its
owner).* Wording only; the verdict class (re-owned, not discharged) is
unambiguous in every source.

---

## The E7-dual queue — state-side determinations

1. **[R2-P3-1] settled ≠ barriered** (E2 review round 2). The recording
   question is [P2 F1] above: the clause is NOT written where E4 will find
   it; E7.2 writes it. On the contract fork the review leaves open (state the
   non-implication explicitly vs make the restart path re-barrier a converged
   alias): **state it explicitly.** Re-barriering a converged alias is a
   semantics move on a frozen 16-key record — §1.5 row #12's charter text is
   "retire the stranded alias", full stop, and E2.1 was explicitly "not
   authorized to edit any catalog predicate" (OPEN-B1's closure); widening
   restart behaviour would need an E0.2b-class amendment with review, for a
   property no current consumer needs (E4 has no consumers yet). The dated
   clause plus the E4 scope sentence is the whole cure. (The Code axis may
   weigh the re-barrier option's mechanics; this axis finds the record answer
   sufficient and cheaper by an amendment.)

2. **[R2-P3-3]** — disposed at [P3 F2]: accepted residual, carrier restated
   (next platform.rs-tree production train), not executed at E7.2.

3. **[R2-P3-2] the unannounced P5 roaming recovery rename** (E2 review:
   either a later-phase key mint or a named exception written into the §3.5
   barrier record). State-side input to the dual's merged disposition: the
   **named-exception route** is the one consistent with the frozen census
   discipline — a 17th `barrier.*` key moves the frozen 165/16 census for an
   idempotent, atomic edge whose interruption row would assert what the
   seeded-state row already asserts (the review's own mitigations). One dated
   annotation on the freeze §3.5 barrier activation record naming the roaming
   recovery rename as the deliberate exception to "every durable barrier edge
   is announced" closes it census-neutrally. Whichever way the dual merges,
   the disposition must land as a dated freeze annotation, not stay in the
   review.

4. **E6.3 VOID — the dated no-work record E7.2 owes.** Content requirements,
   verified against the sources: the record must state **both branches
   empty** with citations — (i) reach answered NEGATIVE at E0.1
   (`GwzM5-8R2E-E01ReachTraces.md` §(a): no v0.11.0 production path executes
   the `name_to_handle_at` probe; `recover_or_create` zero production
   callers; the 22.04 failure fixture-only), so the "if reach is real" branch
   never opened; (ii) the capability-refusal UX moved into **E4.1's
   precondition set** (E0.2 §5.3 as amended — seven items: §5.3's five, with
   the `unsupported.rs` claim-sweep as item 5 per §7.2(c), plus item 0 the
   coexistence gate and item 6 refusal-ordering/restart legality; plan §1.1
   O11 row), so no UX work was E6.3's; (iii) dated, citing plan §1.1's "E6.3
   — VOID" row and E0.2b [P3-5]; and (iv) the E6 review's E6-10 row as the
   executed emptiness check (nothing in the E6 diff touches UX/runbook/notes
   surfaces). All four ingredients exist; the record is assemblable as
   prescribed.

5. **Anchor nit 1** (E6 F-6 hand-off): the re-routing record is in order —
   the deferral's ":394 one-line take() cure" no longer describes the tree;
   the unbounded read is the shared `observe_leaf_exact`
   (`observation.rs:249`, twenty-one call sites, six modules), and the
   in-tree bounded-read template travels with it (`platform.rs:219-234`).
   The design choice (reuse the template at a new bounded observation entry
   vs bound the shared reader) is the Code axis's to weigh; state-side, the
   acceptance must either take the dual's decision or re-own the nit with a
   named carrier and its dated terms updated — silence would revive the
   stale ":394" terms a third time.

6. **E5 [P2-1] K/J(ii) same-object determination — second-axis scrutiny
   executed, SUSTAINED.** §12.9(e) records it coherently: the sanctioned
   one-object-two-R0-rows pattern with the §12.6-form arithmetic line ("22
   rows bind 22 shape labels; the durable-object count is lower"), the
   ground correction to §12.9(c) (its Finalizing-inexpressibility argument
   reaches the three F rows + G-VERIFYING — **four** Finalizing rows — and
   never grounded J(ii)), and both records' different ledger questions
   stated. Cross-checked for surviving contradictions: Table A's K row
   carries the same-object note; §12.6's E5 bracket carries the arithmetic;
   §12.9(c) is a dated 2026-08-24 record corrected forward by (e) — the
   house mechanism; the one un-bracketed superseded figure is [P3 F3]'s.
   Nothing else in the corpus still contradicts the determination.

7. **Tier-2 comparable sub-surface** — deliberately unminted, and rightly:
   amendment §6.3's own rejected-alternative warning (against narrowing a
   comparison to make a clause look met) is the reason a sub-surface must
   arrive by amendment with dual review, not from the implementing step. The
   plan's E4.4 entry, the corpus, and Evidence agree it is unexecuted
   everywhere (§12.8 stays PARTIAL; §12.6's E5 bracket: "tier 2 is executed
   nowhere"). Encumbrance-naming completeness is [P3 F7].

8. **Citation-drift rule spot-check — PASS.** The R2-E-era records citing
   the compatibility contract do so content-anchored with dated line
   conveniences: the archive-corpus rows (e.g. `§5, "Completed and aborted
   v0 records remain v0 and use byte-preserving archival" (:178-184 as of
   2026-08-28)`), Table B's clause column, and the E5 review's §5
   re-verification of all five citations against the live contract. No new
   bare line-anchored contract cite found in the R2-E era. The drift note
   itself is filed at the checkpoint as the resolution index, with the
   six stale-cite documents left as written, per its own rule.

9. **E3 F7 (optional)** — the corpus-wide convergence idiom (a converging
   process trusting another process's unflushed rename) was offered as an
   optional E7-dual item, correctly disclosed and out of E3's scope. It is a
   code-semantics question for the Code axis; state-side the acceptance may
   decline it with one line naming it out-of-scope for R2-E and leaving it
   to the R2-F/native-evidence neighbourhood where power-loss semantics
   already live (plan §1's NOT-R2-E list).

10. **E6 F-3's authorization reading** — the lane owner's ruling that the
    anchor-guard widening is "the step's own authorization, NOT a semantics
    move needing E0.2b" is recorded (checkpoint E6 landing, [F-3]) and is
    consistent with the freeze's Step-4.2 nit scope ("inside the accepted
    same-user boundary") and the E6 review's own analysis (the refusal class
    existed at base; the guard widens the poisonable subset, recoverable by
    removing the foreign name). Sustained; no further record act owed.

---

## Pins and provenance

**The `_fault_count` chain is internally consistent end to end.** Every move
re-derived and re-summed from the docstring's own dated blocks:

| Move | checked_artifact (dar/lin) | remainder (dar/lin) | provenance as stated |
| --- | --- | --- | --- |
| v0.11.0 baseline | 400 / 410 | 932 / 933 | darwin executed at tag; linux run 32954473899; remainders = lib totals minus partitions (1589−657=932; 1600−667=933), marked FIRST-DISPATCH-EXPECTED (E6 F-4 wording cure in place) |
| E2 landing (squash) | 446 / 456 | — | darwin EXECUTED; linux DERIVED +46 (E1+8, E3+17, E2+21, cfg-independent) |
| E5.1 / E5.2 | — | 933→935 / 934→936 | darwin MEASURED each step; linux DERIVED +1/+2 |
| E6.1 | — | 935→937 / 936→938 | darwin MEASURED; linux DERIVED +2 — first non-cfg-independent delta (cfg(unix) pair; Windows moves by zero), stated in the block AND at the battery comment |
| E6.2b | 446→447 / 456→457 | — | darwin MEASURED; linux DERIVED +1 |
| conf-integrity | — | 937→979 / 938→980 | darwin MEASURED (+42); linux DERIVED, cfg-independent |

Sum-confirmations recorded at the CI closures hold to the digit: E5 linux
`1649+1` = 456+936+256+1; E6 linux `1652+1` = 457+938+256+1; darwin twins
likewise (1638/1641). The darwin-vs-linux remainder offset (935/936 era) is
recorded as a pre-R2-E baseline platform delta with the one bare-`#[ignore]`
test premise-checked — the right honesty note, in place.

**Converting the linux markers to MEASURED at E7.2 is SOUND.** The release
verify's ubuntu-24.04 leg executed the fault battery per-partition against
the pinned counts at the settled tree itself — checked_artifact **457**, lib
remainder **980**, v1_lifecycle **256**, root_fault_matrix **1** — at tag
`v0.11.1` = `be693bd` (checkpoint release record; "E7 should cite run
33196576270"). That is direct per-partition linux execution, strictly
stronger than the standing sum-confirmations, at the exact tree under
settlement, with no lib-test delta since. E7.2's rewrite should: (i) cite the
workflow run and leg as the checkpoint and lane memory record them
(33196574973 → leg 33196576270, 2026-08-29, at v0.11.1/`be693bd`); (ii) touch
only the linux provenance wording in `_fault_count` (a scripts-only doc edit;
no protected source-tree digest covers `scripts/checks/`, no count moves);
(iii) leave the FIRST-DISPATCH-EXPECTED convention text intact for future
moves. The Windows arm correctly has no pin (the driver refuses non-darwin/
linux hosts loudly; E6's cfg(unix) note pre-records that a future Windows pin
moves by zero for the O9 pair).

**Census statements — all verified against `be693bd`:**

- `EXPECTED_KEY_COUNT` **165** (`fault_expected_keys.rs:174`), equal to the
  tuple §4.8 arithmetic (18+25+19+11+11+13+30+11+16+11).
- **38/38 re-reserved keys accounted:** cleanup `Executed` ("R2-E phase E1"),
  barrier `Executed` ("R2-E phase E2"), terminal `PartiallyExecuted` with
  `TERMINAL_EXECUTED_KEYS` ten and the eleventh "DECISION T-D's
  determination, not an unconverted edge" — 11+16+10 executed + 1 determined.
- **Reserved arm unconstructed-retained:** `#[allow(dead_code, reason =
  "the frozen activation vocabulary's reserved arm; unconstructed since the
  R2-E E1-E3 landings, retained for the map's carried re-reservation
  clause")]` with the dated doc block — exactly the E2-landing record's
  claim.
- **Freeze inventory count ten:** the §3.5 inventory addendum edited in
  place with dated provenance ("*nine until the R2-E E2 landing, 2026-08-27…
  the single R2-E count move*", `barrier_mutation.rs (16)` listed), and the
  machine pin real — `assert_eq!(declared.len(), 10, …)` inside the
  completeness anchor that also set-equalizes declared-vs-scanned sources.
- **"Five machine-enforced inventories"** — correct **within its stated
  scope** (addendum §6's object: inventories the converging E1-E3 commits
  must move — FAULT_INJECTION_SOURCES; CATALOG_PUBLICATION_CALL_COUNTS,
  admission_mutation.rs now 3 with the fifth-dated-extension comment;
  PROTECTED_SOURCE_TREE_DIGESTS pre_catalog + platform; the catalog.rs
  digest; the capability_permit Rust twin, asserting 14). E7.2's census
  statement should keep that scoping, since the R2-E-era machine-enforced
  pin family is larger (the O13 raw-writer inventory, the driver pins, the
  g23/compat/map markers).
- **O13 pin present and shaped as recorded:**
  `V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES` = exactly
  {`archive.rs`, `store/archive.rs`, `store/rewrite.rs`}, with the dated
  comment carrying the accepted-residual terms ("the set may only SHRINK…
  E4.2/E4.3 retire entries to empty deliberately, in their own commits").
- **Wire zero-diff held for the whole span:** `git diff 94da3e5..be693bd`
  over both `protocol/generated.rs` files is empty — plan §4's non-goal
  honoured through E1-E6 + conf-integrity + release.

---

## Ledger-close table (E7.2's spine)

Verdict column = this axis's finding on whether the row CAN close exactly as
prescribed at `be693bd`. **All fourteen rows: YES.** Conditions from the
findings attach where named.

| Row | Prescribed close form | Verdict | Carrying citations |
| --- | --- | --- | --- |
| **O1** | RE-OWNED with named carrier: R2-F quarantine/relocation package → this lane's E4 resumption; the close **additionally carries §10 row `:280` via O13** | **CAN CLOSE as prescribed.** Carrier well-formed (both halves named: the blocking package AND the resuming owner); row `:280` verified as the ninth §10 row (`ConsumerCheckpoint:272-280`), its second clause pinned (O13 below) | Plan §1.1 cross-lane row + §7.8 fallback (iv) with the dated RULED line (operator "a", 2026-08-27); addendum §7.6 nine-row map |
| **O2** | RE-OWNED, same carrier (first production catalog activation rides E4.1 behind the coexistence gate) | **CAN CLOSE.** Gate text verified live (tuple §11.3 item 1); no record anywhere claims a production `recover_or_create` caller appeared since E0.1's zero-caller trace; E4 never opened | Plan §1.1; tuple §11.3:800-802 + §11.2 "Pinned to R2-F" (:776, relocation row ~:790); E01ReachTraces §(a) |
| **O3** | "Discharged by R2-F's relocation, re-owned there" (E4.7 did not land) | **CAN CLOSE — with [P3 F8]'s tense fix:** RE-OWNED to the relocation package, which discharges it directly on landing; E4.7 stays the stronger mechanism at the E4 resumption; OPEN-R1 rides | Addendum §7.6.2 (single-owner fact) + §7.8(a); plan §1.1 O3 row + OPEN-R1 row |
| **O4** | DISCHARGED — cleanup 11/11 matrix-green both variants | **CAN CLOSE.** | E1 review GO (six P3, none blocking; C-2-vs-§6.1 ruled for the train); freeze cleanup activation record + `Executed` fixture row; E1-tip CI 4/4 green |
| **O5** | DISCHARGED — barrier 16/16 + the restatement class extinct | **CAN CLOSE.** | E2 round-2 GO; freeze barrier record + nine→ten inventory move in place; c11c5ef CI all four legs green incl. first native Windows execution (WINDOWS-ARM-OWED discharged) |
| **O6** | DISCHARGED — observe-or-refuse RESOLVED (mint side E0.2 §3; read side completed E0.2b §5) + the E2 witness | **CAN CLOSE.** Wire surface zero-diff re-verified by this review over the whole span | E0.2b §5; E2 review (O6 read-side refusals split into named rows beside a positive control); freeze barrier record's filed footnote |
| **O7** | DISCHARGED — terminal 10/11 + T-D | **CAN CLOSE.** Announced-vs-performed gaps closed by dated determinations at every home (see Determinations note below) | E3 round-2 GO; freeze terminal record incl. the E3-landing determination (row #2 REPLACED, row #3 STRENGTHENED); fixture `PartiallyExecuted` + T-D docstring |
| **O8** | Split close: 9+1 registry partition + eight tier-1 + two PENDING-FIXTURE (carrier R2-F) DISCHARGED; tier-2 RE-OWNED, carrier E4.4-under-R2-F, sub-surface by amendment first | **CAN CLOSE** — citing §12.9(e) as denominator authority ([P3 F3]) and stating the two-part tier-2 encumbrance ([P3 F7]); the close must not cite byte-equivalence as green (§12.8 stays PARTIAL) | Plan §1.1 O8 row (E5 bracket); Evidence §12.6 E5 bracket, §12.9(e), Tables A/B; E5 review adjudications A and G(iii); corpus machine-enforcement (8 executed / exactly the AC-NOPUB-UNBORN + AP-PRESERVED pair) |
| **O9** | DISCHARGED — the composed-path upgrade-failure test + the moved-pin coverage restoration | **CAN CLOSE.** Both halves MET on the review's own ledger (E6-1, E6-2) | E6 review §3 (genuine ENAMETOOLONG at the upgrade's own open(2); control arm; isolation guard); A1 record §14/L14 |
| **O10** | DISCHARGED with the F-5 wording, verbatim: "cfg(test)-gated variants — a third shape, strictly stronger than both named options (production compiles no constructor at all; negative compile probe verified twice)" | **CAN CLOSE.** The wording exists verbatim in the E6 landing record; E7.2 reuses it | E6 review §4 + F-5; checkpoint E6 landing [F-5] |
| **O11** | CLOSED NEGATIVE at E0.1; UX folded into E4.1's seven-item precondition set | **CAN CLOSE.** The seven-item enumeration is coherent (5.3's five incl. the `unsupported.rs` sweep as item 5, + item 0, + item 6) and travels with the E4 resumption | E01ReachTraces §(a); plan §1.1 O11 row; E0.2 §5.3 as amended; addendum §7.2(c) |
| **O12** | Split close: riders 1-3 as executed/routed; the 481-item reconciliation EXECUTES AT E7.2 | **CAN CLOSE — only WITH E7.2's own acts:** the 481-item reconciliation against BOTH denominators (481 blanket-hidden across 50 files; 1657 spans across 85 files; expectation of a falling count transfers to the E4 resumption since E4 converted nothing), and the dual's nit-1 disposition. Rider 1 (abort-bound tie) E6-4 MET; rider 2 (A-1 taken — with its E4.7 reopen condition traveling to the resumption); rider 3 nit 2 executed (E6-5), nit 1 → dual (item 5 above), unsupported.rs → E4.1 | E0.2 §7.1/§7.2/§7.4; E6 review E6-4/E6-5/E6-6; plan §1.1 O12 row |
| **O13** | Split close: "no legacy raw writer" pin DISCHARGED (landed `8597d32`); substantive half RE-OWNED with E4.2/E4.3; the dated accepted-residual record covering A1→E4.2/E4.3 | **CAN CLOSE.** Pin verified live with both-directions set equality and the dated retirement protocol; the accepted-residual record exists at BOTH homes (plan §1.1 and the pin's own comment), bounded, growth-pinned, expiry named; E4.2/E4.3 scope clauses present in the plan incl. the §7.7 duties (door disposition + workspace-root binding) | Addendum §7.6.1; plan §1.1 O13 row + E4.2/E4.3 entries; `check_checked_artifact_boundaries.py` O13 block; E0-landing CI (33032497893/33032497890 green) |
| **E6.3** | VOID — dated no-work record at E7.2 | **CAN CLOSE** with the four-ingredient content at §Queue item 4 | Plan §1.1 E6.3 row + §3 E6.3 bracket; E0.2b [P3-5]; E6 review E6-10 |

**Determinations note (mandate 2(c)/(d), verified):** the terminal family's
announced-vs-performed integrity holds at every home — freeze §4.3's E7
annotation says "ten activated + one determined"; the terminal activation
record carries T-D "re-grounded at E0.2b §8 on reading (a) alone" with frozen
text closing reading (b)'s demotion, plus the E3-landing determination for
rows #2/#3; the fixture's docstring repeats T-D's ground and the ten-key
list; the plan's O7 row matches. No home still announces fingerprint-depth
semantics the code does not perform.

---

## Completeness sweep — what else the acceptance record must carry

Verified present and needing only citation (no defects): the E3 landing
range's four permanently-red intermediates with the minted squash lesson
(ritual 7's arm, validated at the E2 squash and used again at conf-integrity);
E5's a6ce8a8 latent-red J-7 catch fixed at fc0bb22; the conf_gate Windows
first-dispatch red fixed forward at cc7c625 (fixture-only); the operator
agents-template commit `8c59521` riding the E6 train per standing ruling;
v0.11.1 complete across all three channels with the PyPI runbook gap closed
at source (`11bca66`); conf-integrity's NF-3/NF-4 + two merge-lane follow-ups
recorded BACKLOG (standalone lane — not R2-E ledger rows, but resident on the
settled tree and correctly registered).

Two framing sentences the acceptance should include so "R2-E accepted" is
true as written: (i) R2-E closes against its **§1.1-amended object** — the §0
object statement ("the §10 conversion table is executed…") is deliberately
not met, by the operator's ruling (a), and §1.1 is the authority that
re-scopes the close; (ii) the E6 milestone annotation (F-7) means the
review-debt ledger empties **at E7.2's own acts**, so each act in this
report's conditions must be visible in the acceptance for that claim to
stand.

Nothing else was found. No unclosed interior-review condition beyond those
named in the findings; no contradiction between the checkpoint's R2-E-era
records and the documents they summarize (every checkpoint claim I tested —
counts, digests moved, inventory states, CI run dispositions, freeze edit
lists — matched the underlying document or the tree); no landing condition
recorded as discharged that is not discharged.

---

*Filed 2026-08-29 by the E7.1 STATE axis (dual #2), peer-blind. The lane
owner commits this file; no other write was made.*
