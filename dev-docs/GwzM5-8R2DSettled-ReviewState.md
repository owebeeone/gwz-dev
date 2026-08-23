# STATE-AXIS REVIEW — R2-D SETTLED DUAL, aggregate gate over the whole conversion (round 1)

Date: 2026-08-23. Axis: **STATE** (adversarial; round **1 of 2**; program-level
dual gate #3 of R2-D's three — the aggregate settled-tree gate, plan §4 Step
5.2). Object: **gwz-core settled tuple tree `d45458d`** ("Land R2-D Step 5.1",
verified `git rev-parse HEAD`, working tree clean) **together with the frozen
contract `GwzM5-8R2DInterfaceFreeze.md` AS AMENDED** — the full §3.5/§4.3/§4.4
annotation stack (~25 amendments across six landing waves) read end to end, in
order. Claims document: `GwzM5-8R2DSettledTuple.md` (688 lines). Peer-blind:
`GwzM5-8R2DSettled-ReviewCode.md` was not read and does not exist in the tree
at review time. Escalation ledger carried in full (tuple §10 ≡ checkpoint
PHASE 4 SETTLED record, verified identical).

Also consumed: `CurrentProgramCheckpoint.md` RESUMED-section landing records
(:9-684), `GwzM5-8R2D-Plan.md` §4/§5, `GwzM5-8R2DPhase4Closure.md` (§2.7,
§3, §6), `GwzM5-8R2DStep31b-Review.md` and `GwzM5-8R2DStep32-Review.md` (for
riding-item termination only), `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`
§10 annotation block.

---

## VERDICT: **NO-GO (round 1)** — the tree is clean; the frozen map is not.

The settled **tree** survived every adversarial check this review threw at it:
census exact, per-key evidence real at every sampled site, matrices green
re-run, ChangeBudget arithmetic exact to the line against the git log, the E16
atomicity record still true on the final tree, all four §4.4 arm resolutions
mutually consistent and reconciled against the arm-type census in code. What
fails is the **contract half of the object**: the freeze's §3.5 map — the one
document every activation record, the RemPlan §10 annotation, and the plan's
own map clause point at — still declares `durable_leaf.*`, `namespace.*` and
`record.*` **reserved**, six days and six landing waves after all 35 of those
keys were executed. An aggregate settled gate whose mandate is "certify the
freeze as amended coheres with the tree" cannot certify that sentence today.

The remediation is small, census-neutral, document-only, and fully drafted
below (findings [P1-1] and [P2-1] both come with verbatim-liftable freeze
text). One merged remediation at the settle landing should converge this to GO
in round 2 within the two-round cap.

**Finding counts: 1 × P1, 1 × P2, 3 × P3.**

---

## Findings, most severe first

### [P1-1] The freeze §3.5 map was never amended for the three Phase-2 activations — 35 of 165 keys are misdeclared "reserved" by the frozen contract

**The fact.** Freeze §3.5 rows (`GwzM5-8R2DInterfaceFreeze.md:436-438`):

```
| `durable_leaf.*` | 11 | R2-D **Step 2.1**      | reserved |
| `namespace.*`    | 11 | R2-D **Steps 2.2/2.3** | reserved |
| `record.*`       | 13 | R2-D **Step 2.4**      | reserved |
```

No annotation for any of the three exists anywhere in the freeze (verified by
exhaustive grep; the E9 and E16 annotations are §4.3 *edge* records, not
family flips). On the tree, all three families are
`FaultFamilyActivationV1::Executed` (`fault_expected_keys.rs:321/:337/:355`),
all 35 keys have named injection sites and both-variant matrix rows (sampled
and re-run below), and the landing records (checkpoint :140-156, :158-192)
say "durable_leaf 11/11 executed", "namespace 11/11 executed", and record the
2.4 activation. The same staleness extends to §3.5's closing "Injection-site
inventory" paragraph (freeze :849-874): it declares **three** production
sources, "all of them `catalog_bootstrap.*`" (round 2's preamble parenthetical
added the fourth, `admission_mutation.rs`); the settled tree has **nine**
(`FAULT_INJECTION_SOURCES`, 9 `include_str!` rows), spanning six families.

**How it happened — a process-seam drift, visible in the records.** At Step
1.3 the flip duty was "the memo §3.5 row and source-count note updated as the
activation record" (checkpoint :658-659) and was done. From Step 2.3 onward
the sanctioned mechanism became the dated §3.5 annotation, and every
subsequent flip filed one (2.3, 3.1 deferral, 3.1b, 3.2, Phase-3 settle
determination, 4.1, 4.2 ×2). Steps 2.1, 2.2 and 2.4 landed in the gap between
the two mechanisms and used **neither**: their landing records name "E9
annotation; counts and pins" and the fixture edits, but no freeze-map edit.
Seven of ten family rows are accurate today; the three that fell in the gap
are not.

**What breaks if unaddressed.** (i) The certified contract misstates the
activation state of 35/165 keys — the exact class of incoherence this gate
exists to refuse; every downstream reader (R2-E planning, the A1 activation
review, any future dual) is pointed at this map by RemPlan §10's annotation
and by the map's own "this is the frozen map" sentence. (ii) "Reserved" in
this map's vocabulary is load-bearing: it means *no sites exist and the
converting package owes sites+rows in the same commit* (RemPlan §10 duty, the
reserved-family scan). For these families that is false in both halves, so
the map's carried clauses stop meaning anything if a flip can happen without
the map moving. (iii) The annotation-stack integrity that the other seven
rows' history establishes — and that this dual is asked to certify — is
silently broken for exactly the three single-axis Phase-2 steps.

**Remediation (census-neutral, document-only): lift the following three
annotations into §3.5 at the settle landing, placed under their rows in the
established idiom, plus the one-sentence inventory addendum.** Drafted
verbatim-liftable:

> `durable_leaf.*` activation annotation (2026-08-23, filed at the 5.2
> settled dual's State-axis round 1; records the Step 2.1 flip the landing
> train left out of this map — the row above is left as written, since this
> annotation is the sanctioned mechanism and is its governing statement):
> **all 11 keys are executed** (Step 2.1, landed `d2cf755`, 2026-08-22).
> Sites: `capability/pre_catalog/provider/leaf_observation.rs`, all eleven.
> Matrix: `provider/tests_leaf_fault_matrix.rs`,
> `DURABLE_LEAF_MATRIX: [(Fault, LeafArmV1); 14]` — 14 rows over 11 distinct
> keys, covering both sides of the two-sided proof (Exact and Absent arms;
> three boundaries shared), interruption/restart/convergence on both target
> variants plus 2×2 repeated-boundary rows. The family's Windows arm carries
> the §4.3 E9 activation annotation landed with the same train. Counts held
> at 165; no key minted.
>
> `namespace.*` activation annotation (2026-08-23, same filing): **all 11
> keys are executed** (Step 2.2, landed `d2cf755`, 2026-08-22). Sites:
> `capability/pre_catalog/provider/namespace_mutation.rs`, all eleven — the
> `namespace` owner holds none, which is the rule the later
> `managed_bootstrap.*` annotations cite as precedent. Matrix:
> `namespace/tests_fault_matrix.rs`, `NAMESPACE_MATRIX: [Fault; 11]`,
> both target variants, repeated subset 3. The `parent_barrier` row's
> Windows semantics are governed by the §4.3 E10/E14 activation annotation
> (a process stop across a documented no-op edge there — recorded deviation,
> no count moved). Counts held at 165; the caller pin's 2.2 extension is the
> `CATALOG_PUBLICATION_CALL_COUNTS` namespace_mutation.rs entry, recorded in
> the checker in the converting commit.
>
> `record.*` activation annotation (2026-08-23, same filing): **all 13 keys
> are executed** (Step 2.4, landed `c2564ba`, 2026-08-22). Sites split as
> the family's own shape: the four bounded-parse stages with the R1 parse
> owner (`protocol/authority_record.rs`), the nine install/retire/join
> boundaries with the binding owner
> (`capability/pre_catalog/provider/authority_record_binding.rs`). Matrix:
> `provider/tests_authority_record_matrix.rs`, `RECORD_MATRIX: [Fault; 13]`,
> both target variants, with
> `the_repeatability_taxonomy_accounts_for_every_boundary` proving the 9/4
> repeatable/unrepeatable partition machine-checked. Counts held at 165; the
> caller inventory's move to 13 with the 2.4 file joined is recorded in the
> landing record and the checker.
>
> Inventory addendum (2026-08-23, appended to the §3.5 injection-site
> inventory; the round-2 paragraph above is left as written as the Phase-0
> statement it is): on the settled tree `d45458d` the declared-and-anchored
> production source list is **nine** files — the three `catalog_bootstrap.*`
> sources above, `admission_mutation.rs` (19), `leaf_observation.rs` (11),
> `namespace_mutation.rs` (11), `protocol/authority_record.rs` (4),
> `authority_record_binding.rs` (9), `managed_mutation.rs` (28) — pinned
> tree-complete by the round-2 completeness anchor exactly as designed.

### [P2-1] `runtime.*` — 18 keys declared Executed at a standard below every other executed family's, adjudication filed to this dual — **RULING: RESTATE the frozen claim** (drafted below)

**The fact, verified on the tree.** All 18 `runtime.*` keys: zero
`CheckedArtifactFaultKeyV1` sites, zero matrix rows (grep: 0 hits outside
`fault_v1.rs`). The declared mechanism is real but disjoint: the six-variant
`RuntimeBootstrapFault` enum (`bootstrap/runtime/fault.rs:4-11`) produced at
six production sites (`bootstrap/runtime/mod.rs:140/:145`,
`catalog_lease.rs:102/:136`, `catalog_lease/target.rs:347/:352` — all
verified) and driven by eight named substitution/drift-rejection tests (all
eight verified present by name). No per-key correspondence exists between the
18 keys and the 6 variants; the tests are not interruption/restart/convergence
matrices and are not variant-paired. `fault_expected_keys.rs` hard-codes the
exemption (`if *family != "runtime"` inside
`reserved_fault_families_have_no_injection_sites_before_their_package`). The
freeze §3.5 closing paragraph declares the exception — accurately as far as it
goes — but the row's bare **executed** reads as the same claim the other five
executed families earn with 107 per-key tables, and it is not.

**Ruling: restate, do not re-reserve.** Grounds: "reserved" in this map means
*edges not yet converted; the converting package owes sites and rows*
(RemPlan §10). `runtime.*`'s edges were converted and accepted in R2-A/R2-B,
before the stable key census existed, and run in production on this tree.
Re-reserving would (i) plant in the map a falsehood of exactly [P1-1]'s class,
pointing the other direction; (ii) mint an activation duty **no planned
package owns** — the `cleanup.*`/`barrier.*`/`terminal.*` records each name
the R2-E consumer conversion that genuinely owns their edges, and no such
owner exists for edges already converted; an owner-less register entry is how
debts rot. Restatement makes the frozen claim exactly true at the cost of
admitting the evidence class is weaker — which is what the record actually
is. Census-neutral; no code change; the exemption's justification becomes
accurate instead of embarrassing.

**Drafted freeze-edit text, verbatim-liftable** (placed as a blockquote
annotation directly under the `runtime.*` row in §3.5):

> `runtime.*` evidence restatement (2026-08-23, ruled by the 5.2 settled
> dual's State axis; adjudicates the open item Step 5.1 filed at
> `GwzM5-8R2DSettledTuple.md` §4.6/§11.3(3). The row is left as written,
> since this annotation is the sanctioned mechanism, and it is the governing
> statement of the row's **executed** cell and of the closing `runtime.*`
> paragraph below the map). **The family's "executed" status is a
> family-level claim on a disjoint mechanism, and it claims no per-key
> evidence.** What the tree proves, and all it proves: the six-variant
> `RuntimeBootstrapFault` enum (`bootstrap/runtime/fault.rs`) is produced at
> six production sites — `bootstrap/runtime/mod.rs` (final lease open, final
> lease lock), `catalog_lease.rs` (initial-retention-complete, preparation),
> `catalog_lease/target.rs` (catalog final lease open, catalog final lease
> lock) — and driven by eight named substitution/drift-rejection tests in
> `bootstrap/runtime/tests.rs`, `catalog_lease/tests.rs` and
> `catalog_lease/tests/association.rs`. No correspondence exists or is
> claimed between the 18 stable keys and the six variants; none of the 18
> keys has a `CheckedArtifactFaultKeyV1` injection site or matrix row
> (verified per key at the settled tuple, `d45458d`); the six tests are
> substitution-rejection tests, not interruption/restart/convergence
> matrices, and are not variant-paired. The per-key L1-16/L2-14 table the
> settled tuple produces for the R2-D families (§4.1-§4.5 there) therefore
> does not exist for this family, and the `fault_expected_keys.rs` runtime
> exemption is the machine form of this restatement, not a debt of the R2-D
> lane.
>
> **Why restated rather than re-reserved.** "Reserved" in this map means the
> family's edges are not yet converted and the converting package owes sites
> and rows in the same commit (RemPlan §10). `runtime.*`'s edges were
> converted and accepted in R2-A/R2-B, before this key census existed, and
> run in production on this tree; re-reserving would restate the map's
> falsehood in the other direction and would mint an activation duty no
> planned package owns — the `cleanup.*`/`barrier.*`/`terminal.*` records
> each name the R2-E consumer conversion that genuinely owns their edges,
> and no such owner exists for edges already converted. The 18 keys remain
> in the vocabulary as the family's names. Any future package that wants
> per-key runtime evidence — the natural candidate is the R2-F native fault
> evidence release gate (plan §5 item 4) — takes the standard upgrade path:
> `CheckedArtifactFaultKeyV1` injection sites, both-variant matrix rows, and
> deletion of the `fault_expected_keys.rs` runtime exemption, all in the
> same package. Until that package exists, no duty is open and none is
> deferred by this record.
>
> **Counts.** 165 total, unchanged; no key minted, none retired; 18/165
> unchanged. Census-neutral by construction: this annotation moves no
> activation state — it states the evidence class of one that has stood
> since R2-B.

### [P3-1] The freeze §4.2/§10.3 tracked acceptance item was never discharged on record and is absent from the open register

"Both `tests_admission_spike` cases green on the next full Windows matrix
run" (freeze :925-929, :1701-1702; checkpoint :68). Factually satisfied —
runs 18-21 are fully-green full-matrix Windows runs (1472-1528 passed, 0
failed) and both cases are ordinary lib tests inside them — but no record
marks the item discharged (contrast the anchor package's twin item,
explicitly "DISCHARGED" at checkpoint :383), and the tuple's §11 register
does not carry it. An owed item that is neither discharged nor open is a
ledger leak. One-line discharge note at the settle landing.

### [P3-2] The `authority_name` design note "carries to R2-E" but the R2-E register does not carry it

Checkpoint 4.1 landing record (:473-474): "the authority_name
non-self-checking-name design note carries to R2-E". Tuple §11.1 (the R2-E
register the next lane reads) has no such row. Add it at the settle landing
(one row, citing the Step-4.1 review and the E21 P2 finding it descends
from).

### [P3-3] Exactness bundle — five small self-report drifts, none load-bearing

(a) Checkpoint 4.2 record says "net +1061"; measured (and the tuple's own
table) is +1062 (370 prod + 682 test + 10 scripts). (b) Tuple §5.3's
subsystem denominator "54,956 lines" measures 54,962 on the settled tree —
the 6-line gap is exactly its own §2.1 repair's net, i.e. the denominator was
taken pre-repair; 63.6% holds either way. (c) Tuple §5.1's dev-docs row
("+26,797/−61 across 57 files" for "the matching range") is not reproducible
as stated — no range endpoints are named; a naive since-2026-08-16
reproduction at parent HEAD gives 46 files +19,972/−172; plausible under a
wider range, but pin the endpoints if the number is to be citable. (d)
Checkpoint 3.1b record's "publication seam unchanged (21 sites, permit 13)" —
"21 sites" reconciles against nothing in the freeze or the checker (every
governing count is 13/6-callers-then-13); ambiguous prose, freeze governs.
(e) Freeze-internal: the §3.5 closing runtime paragraph cites the fixture
exception at ":348-355" while the same revision's preamble says the block
moved to ":356-364" — a same-document, same-tuple inconsistency at round-2
time (positions have long since drifted anyway; the freeze declares its
positions at `d32b2c9`).

---

## The mandated second-axis scrutiny — results

**1. Step 2.3's E16 cross-parent atomicity record vs the final tree: STILL
TRUE.** `managed_mutation.rs` is byte-untouched from `3a45619` through
`d45458d` (empty diff), and the record is carried verbatim at the edge
(`retire_marker`, :721-766): rename as commit point, two
`sync_directory_edge` calls ordering observation only, the three post-crash
states each named to their matrix rows, the no-key window between the
flushes, EXDEV as a typed pre-mutation refusal, foreign removal outside the
boundary, off-table wedge refused typed by `observe_installed`'s interior
check. Phase 4's surgery in the same neighborhood was checked directly:
`platform.rs`'s P1 pair (`open_rename_source`/`rename_open_source`) bodies
are unchanged across `c2564ba..d45458d` — the two hunks are the additive
`publish_verified_leaf_no_replace` composition (4.1) and the anchor-block
extraction to `platform/anchor.rs` (4.2). The three rows
(`marker_retire`, `marker_retired_reobserve`, `final_identity_reobserve`)
re-ran green in this review's 400/0 focused run.

**2. Step 3.1b chain semantics and the four §4.4 arm-table resolutions:
MUTUALLY CONSISTENT, and reconciled against the code's arm census.** The
resolution chain — E16 (destination conditional → none; row then driven by
E17 alone), E17 (both arms → none; source-interior row driven by E15 alone,
destination row by neither), 4.2 (retirement-destination row → no arm for
E22; driven by `terminal.*`'s edges alone, re-reserved) — is sequential,
each annotation naming the mechanism and stating the row's driver set on its
tree; no concealed contradiction. Decisive cross-check against the tree:
`DirectoryInteriorExpectationV1 = {CatalogStaging, AdmissionStaging,
ManagedStaging}` (the Phase-1 C-2 admission arm and the 2.3 managed arm,
exactly the two the table assigns) and `DestinationRecheckV1 = {None,
PreRetirementFinal, AdmissionCatalogInterior}` (the Phase-1 admission
destination arm; **no** managed-destination variant, **no** further
retirement variant) — exactly what the four resolutions jointly predict.
`resume_intent` (provider.rs:189) exists; the chain-linkage adversarial
verification is the 3.1b review's, and the 15-key intent matrix re-ran green
here.

**3. Annotation-stack coherence audit: PASSES except [P1-1].** Read in
order: round-2 preamble; admission inline activation; managed 2.3 → 3.1
deferral → 3.1b partial supersession (explicit italic clause) → 3.1b flip →
3.2 flip (citing the deferral "as superseded") → Phase-3 settle
determination; cleanup/barrier/terminal non-activation records with the
re-reserve clause invoked; E4-retire record (route (ii), no
`admission.record_retire` key — verified 0 hits); E9 → E10/E14 (explicit
supersession of one clause, frozen text preserved) → 4.2's two relocation
amendments (anchor cites verified at the stated `anchor.rs` lines:
`ANCHOR_BYTES` :74, `ANCHOR_PREFIX` :76, `RETIRED_PREFIX` :100, `prepare`
:129, `round_trip` :183, `survey` :284; `ExactInterior` no-op at
`platform.rs:526`; `FOREIGN_AUTHORITY_REFUSAL` text is the moved text
verbatim at `authority_record_binding.rs:325-326`, carriers :321/:340
exact). Every owed duty is discharged or open except the two P3 register
leaks. **Census arithmetic at every stage: exact.** 165 = 18+25+19+11+11+13+
30+11+16+11 (fault_v1.rs counted); managed 8 → (8+15)=23 → (23+5)=28 with
2 remaining as the determination pair; cleanup 0/11, barrier 0/16, terminal
0/11; tuple §4.8's 107+2+38+18=165 with 107 = 19+11+11+13+28+25; permit
callers 13 = checker counts 1+5+2+1+2+2; `EXPECTED_KEY_COUNT = 165` at
`fault_expected_keys.rs:174` (citation still exact).

**4. §3.5 map vs the tree — 16 of 107 evidence rows sampled across all five
families: ALL REAL.** Sites verified at the tuple's exact lines
(admission `:51/:68/:268/:354→:410`; durable_leaf `:174/:274/:387`;
namespace `:90→:290/:242/:334`; record `authority_record.rs:359`,
binding `:456/:552`; managed `:433/:519+:950/:923/:1044-:1045`); all seven
matrix constants present at the declared arities (19/14-over-11/11/13/8/15/5);
`reconcile_executed_keys` consumed by all eight matrix files; every named
both-variant test present and **re-run green in this review's own 400/0**
(37.2s, matching the tuple's tail). The 2 determinations: both keys 0 hits
outside `fault_v1.rs`; determination filed in the freeze; evidence homes
exist and ran (`tests_purpose_policy.rs` — including the [P3-3]-hardened
exact-refusal pin — and `coordinator/tests_execution.rs`). Reserved keys: 5
of 38 sampled (`CleanupWorklistPublish`, `CleanupAliasRetire`,
`BarrierIntentPublish`, `BarrierTargetAliasRetire`,
`TerminalActionDirectoryRetire`) — 0 hits outside `fault_v1.rs`, matching
the fixture's Reserved scan.

**5. runtime.\* adjudication: ruled at [P2-1]** — restate, with the drafted
annotation above. The six variants, six production sites, and eight driving
tests were verified individually on the tree before ruling.

## Plan §4 milestones on the tree

- **Phase 0** ✓ — freeze ACCEPTED GO/GO; scaffolding pins live and green.
- **Phase 1** ✓ — `resume_or_admit` real (driver delegation verified);
  19/19 executed both variants; kernel settled GO/GO at `bf438ed`.
- **Phase 2** ✓ on the tree (three families 11/11, 11/11, 13/13 executed;
  managed defaults required and unreachable-in-production structurally) —
  but the milestone's own contract record is [P1-1]'s subject.
- **Phase 3** ✓ — four purposes both variants through production
  constructors; 28/30 + the determination pair; coordinator glue landed with
  the 3.3 round-2 hardening; all 8 settle dispositions verified carried.
- **Phase 4** ✓ — raw-rename surface is one delegation reference
  (`RAW_RENAME_CALL_ALLOWLIST`: `platform.rs` `rename_relative: 1`,
  verified against the tree's single call at `platform.rs:94`); coexistence
  decision recorded (`Phase4Closure.md` §2.7, variant A) with the A1 gate
  fail-closed in code (`recover_or_create`: no production caller, verified);
  L2-04 harness 86/0 recorded at 5.1 and `validate`/`gate-ready` re-run
  green here (24 tuples).
- **Phase 5** — the tuple exists, is honest, and carries executed per-key
  evidence for every R2-D family; the runtime.* reading is closed by this
  review's ruling. The milestone completes when the settle landing lifts
  [P1-1]'s and [P2-1]'s texts.

## Settle caveats and register — carried correctly

All seven §11.3 caveats verified present and accurate. Independently
re-verified: **L2-05** — no workflow under `.github/workflows/` (all six
files) references `check_merge_docs.py`/`test_check_merge_docs.py` while
`AgentProcessRules.md` §4 names them hard gates; both pass on this host only
because the sibling `gwz-cli` checkout exists. The CI-wiring item is real
and correctly filed. Twin-seam docket ✓ (§11.2, Step-4.1 [P3-1] → R2-F).
Test-door dependency ✓ (§11.3(2), settle disposition 2/3). Operator policy
question ✓ (§11.3(6)). Register leaks: [P3-1], [P3-2] above. (The ARM
template-mode operator question is checkpoint-carried, program-level, not
this register's to hold — no finding.)

## ChangeBudget audit

All 17 per-step rows recomputed from `git show --numstat` under the tuple's
own bucketing rule: **every row and all three totals match exactly**
(prod +9,445 / test +11,525 / scripts +49); ratios 1.22:1 as bucketed and
1.55:1 quasi-reattributed both reproduce; D3's +2192/−103 reconciles as
stated; 4.2's composition sums to +1062 (see [P3-3](a)).

## Verification appendix (this review's own runs, tree `d45458d`, clean; CARGO_TARGET_DIR isolated per mandate)

```
git -C gwz-core rev-parse HEAD → d45458d820093c841257c62ea8ee92e752e835f3; status clean
cargo test --lib checked_artifact::interface_tests::fault_expected_keys → 6/0 (1564 filtered → 1570 collected, consistent with 1569+1 ignored)
cargo test --lib checked_artifact:: → ok. 400 passed; 0 failed; 1170 filtered; 37.21s  (both 5.1 multi-component writer tests observed green by name)
cargo fmt --all -- --check → clean
cargo test --test {diff_render_spike,protocol,publish_workflow,rename} → 10/0, 29/0, 9/0, 2/0 (= 50/0)
cargo test --test protocol generated_ → 3/0 by name
cargo clippy -p gwz-core --all-targets --all-features -- -D warnings → zero diagnostics
python3.13 check_checked_artifact_boundaries.py → ok (15 visible entries, 5 classified modules)
unittest -k test_windows_exact_handle_publication_is_source_protected → OK (the §2.3-repaired companion, re-run alone)
unittest test_release_boundary.py → OK
check_merge_docs.py → ok (11 sources, 147 assertions); its suite → OK (3)
check_merge_compatibility_predicates.py → 7 rules / 7 bindings; its suite → OK (14)
retained_reader_harness.py validate / gate-ready → valid / manifest-ready, 24 tuples
grep sweeps: runtime/reserved/determination keys 0 sites; rename_relative 1 call; recover_or_create 0 production callers; blanket coverage recomputed 103 files / 34,941 / 63.6%
```

Not re-executed here (accepted as recorded, with corroboration): the full
`cargo test --lib` (700s; corroborated by the 400/0 focused run, the 1570
collected count, and clippy compiling all targets), the full 65-test boundary
unit suite (~440s; the one repaired member re-run green alone), the 86-test
retained-reader suite (validate/gate-ready re-run), the CI platform matrix
(runs 19-21 cited identically by tuple §9 and the checkpoint; nine of nine
green as recorded).

— State axis, round 1 of 2. NO-GO; remediation drafted liftable in full
above; expected one merged round to GO.
