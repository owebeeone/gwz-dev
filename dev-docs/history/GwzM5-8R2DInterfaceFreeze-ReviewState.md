# R2-D Phase 0 interface freeze — STATE-axis review, round 1

Date: 2026-08-22
Axis: **State / durable-state** (adversarial, peer-blind; round 1 of the
two-round cap)
Object: gwz-core parked commit `d32b2c9cb67a0d25ddb2dd047e1c5d1a70d0c39b`
(diff `90d3f8a..d32b2c9`, 10 files, +767/−29);
`dev-docs/GwzM5-8R2DInterfaceFreeze.md` (DRAFT, 692 lines, committed at root
`a995c6d`); the §10 append (+30 lines, append-only) in
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`. Root HEAD at review
time: `6a458aa` (thin-A1 acceptance lane; neither object file touched since
`a995c6d`).
Peer-blind: the Code-axis report was not read or searched for.
Grounding re-read for this review: `GwzM5-8R2D-Plan.md` (§2.2, §4, §5, §9),
`GwzM5-8R2CCatalogBootstrapAmendment.md` (§4.1 + erratum, §5, §7, §8,
:665-667), `GwzM5-8R4bR2ConsumerCheckpoint.md` §6-§9/§11/§12/§14, its RemPlan
§8 items 15-19 and §10, `GwzFasterProposal.md` §2/§3, the thin-A1 quote at
checkpoint (d6cd2cb) :44-81, and the code at `d32b2c9`/`90d3f8a`.

Tree note: the gwz-core working tree carries one uncommitted file from the
parallel Phase-1 lane (`src/workspace_ops/merge/v1_lifecycle/tests/forward.rs`);
every code fact below was taken from the committed object
(`git show d32b2c9:<path>` / `90d3f8a:<path>`), and the re-executed gates are
attributed accordingly (§4).

## 1. VERDICT: **NO-GO** (1 P1; 0 P0). GO requires zero P0/P1.

The freeze's central durable-state adjudication is **correct** (§2 below): C-2
is not a Track-W event, and booking it as an ordinary Phase-1 in-seam
obligation is sound. The NO-GO is narrower: the freeze's own completeness
claim — that the admission arm is the **one** bounded extension the sealed
primitive needs across R2-D — is falsified by the memo's own E15/E17 rows,
leaving the managed-phase directory publishes with an unrecorded C-2-class
decision (P1-1). The remediation is text-only and fits round 2.

Finding counts: **P0 0 · P1 1 · P2 3 · P3 4.**

## 2. The central question — C-2 vs Track-W: RULED NOT TRACK-W

The mandate: adjudicate whether `DirectoryInteriorRecheckV1` /
`DestinationRecheckV1` are durable records or in-memory protocol/recheck
types, tracing serialization reality in code before ruling, because the
zero-new-durable-records rule (plan §4 Track-W; ConsumerCheckpoint §14
:373-386 "a new protocol record, slot, purpose, action, or durability phase")
makes any durable change a STOP requiring freeze+dual before implementation.

Trace, at `d32b2c9`:

1. **Both types are in-memory by construction.**
   `publication.rs:34-37`: `pub(super) struct DirectoryInteriorRecheckV1<'a>
   { durable_identity: &'a DurableObjectIdentityV1, expected: &'a
   CatalogBootstrapRecordV1 }`. `publication.rs:41-47`:
   `pub(super) enum DestinationRecheckV1<'a> { None, PreRetirementFinal
   { durable_identity: &'a ..., expected: &'a ... } }`. Lifetime-parameterized
   reference holders cannot be serialized records; neither has any encode
   path.
2. **No serialization or durable-root reachability.** Exhaustive usage sweep
   (`git grep` at `d32b2c9`): the two types appear only in
   `provider/publication.rs` (definition and match), `provider/mutation.rs`
   and `provider/directory_mutation.rs` (construction of transient
   expectations), and the test spike. They are consumed and dropped inside
   `publish_verified_no_replace` (`publication.rs:82-183`) as **read-only
   pre-rename verification**; a read-only check cannot alter durable state.
3. **Every durable item the admission arms would reference already exists in
   the frozen R1+C0 floor.** The resident reservation's wire form is
   `generated::CheckedActionCapacityReservationV1` (`protocol/generated.rs:290`,
   encoded via `ActionCapacityReservationV1::encode_canonical`,
   `protocol/admission.rs:61-64`); the admission record and its state enum are
   `CheckedActionDirectoryAdmissionV1` / `CheckedAdmissionState`
   (`generated.rs:316` / `:43`); the admission name grammar already carries
   `ActionAdmissionActive` / `ActionAdmissionScratch` / `ActionAdmissionStaging`
   plus `RetiredActions(Descriptor)` (`protocol/slots.rs:47-58`); action-side
   slots (reservation, scratches, retirement aliases) are in
   `BaseActionSlotV1`. No new record, slot, purpose, action, or phase is
   implied by the arms.
4. **The package itself is durable-clean.** The committed diff touches no
   `protocol/` file, no `generated.rs`, no `.taut` source; the schema gate
   (`interface_tests/durable_path_schema.rs:14-36`) is unchanged and green in
   the re-run.

Ruling: the memo's §6 sentence — "it adds a variant to two `pub(super)`
in-memory recheck enums inside `publication.rs`, neither of which is
serialized or reachable from a durable-record root" — is **substantively
TRUE** (its shape imprecision is P2-3), and C-2 is correctly booked as
ordinary Phase-1 in-seam work, not a Track-W stop. **No P0.**

Contingency-structure logic (mandate item 6): the operator instruction
("Do not freeze the four defaults into required methods until each new
physical edge names an admitted primitive per platform",
`GwzFasterProposal.md:68-71`) is honored in substance: every edge names an
admitted platform primitive (§3 below); the delta is implemented but the
commit is **parked and unpushed** with the checkpoint recording "push only
with a review GO/GO train or by operator instruction", the revert trigger is
stated, and the revert is verifiably a two-file operation (the delta is
confined to `namespace/backend.rs` + `namespace/test_support.rs` — confirmed
against the diff). The dual review is therefore still the binding gate, and
recording the freeze as contingent on C-1/C-2 is coherent — **except that
C-2's "the one bounded extension" cannot be confirmed verbatim** (P1-1); this
review confirms the admission arm itself as in-seam, Phase-1-owned, not a new
primitive and not a bypass.

## 3. Findings

### [P1-1] The "one bounded extension" claim is false: the managed-phase directory publishes (E15/E17) need un-named arms of the same recheck class

Quote (memo §4.4): "**One bounded extension is required inside the sealed
primitive, and Phase 1 owns it.**" and (§2 C-2) "the **one** bounded extension
identified at §4.4 — an admission arm on the sealed primitive's
*source-interior* and *destination* recheck enums — is accepted as an in-seam
extension owned by Phase 1".

Evidence:

- The sealed primitive's directory source arm **requires** an interior
  recheck typed to the catalog record: `PublicationSourceV1::Directory
  { expected_identity, interior: DirectoryInteriorRecheckV1<'a> }` with
  `expected: &'a CatalogBootstrapRecordV1` (`publication.rs:17-37` at
  `d32b2c9`), verified via the catalog-typed
  `interior::staging_plan(..) == StagingPlanV1::Complete`
  (`publication.rs:139-155`). The field is not optional.
- The memo's own table routes the managed edges through that arm: E15
  "managed component install (**staged dir** → final) | 2.3/3 | P1 + P2 + P3 |
  as E3" and E17 "managed successor publish, prior-generation retirement,
  final reproof | 3 | P1 + P2 + P3 | as E3/E13" (§4.3).
- ConsumerCheckpoint §9 (:258-261) confirms those are directory publishes:
  "Each missing component has a **staged directory**, ownership marker,
  installed observation, **durable successor**, prior-generation retirement,
  marker retirement, and final reproof."
- No alternative route exists: amendment §8.13 (:656-659) structurally
  rejects raw provider renames, and the checker's bare-identifier scan +
  `RAW_RENAME_CALL_ALLOWLIST` (checker `:160-184`) fail closed on any rename
  outside the sealed primitive.
- A managed component's staged interior is neither a
  `CatalogBootstrapRecordV1` nor an `ActionCapacityReservationV1`, so the
  admission arm C-2 defines does not serve it. Phases 2.3/3 therefore need at
  least one further arm on the same recheck surface (and the marker/generation
  retirement edges E16/E17 raise the matching destination-recheck question),
  which the freeze neither names nor assigns.

Concrete scenario forcing the mid-lane stop: Phase 2.3 implements
`install_managed_component`; the staged component directory must publish
through `publish_verified_no_replace`; the implementer finds the mandatory
interior recheck cannot be constructed for a managed interior; the freeze on
record says the admission arm was "the one" extension and says nothing about
who may extend the sealed enums again or under what rules — the exact
C-2-class question this freeze existed to settle, now unrecorded, on a sealed
object whose last extension needed a dual-review contingency. That is a
material gap forcing a mid-lane freeze-level clarification.

Durable-state impact of the missing arms: none (same class as C-2 — in-memory,
read-only verification; §2 trace applies unchanged). The gap is contractual,
not physical — which is why this is P1, not P0.

Minimal remediation (text-only, round 2): amend §4.4 (and the §2 C-2 wording)
to define the recheck-arm **class** once — "the `PublicationSourceV1`
interior/destination recheck surface is the protocol-typed re-verification
layer of the sealed primitive; each converting package adds its family's arm
inside the provider owner under the same rules (no platform surface touched;
acquisition window, identity compare, retained handle, and no-replace rename
unchanged; caller-pin and digest refresh extended deliberately in the same
commit)" — and assign: admission arms → Phase 1 (C-2 as drafted); managed
interior/destination arms → Phases 2.3/3; any further retirement-destination
arms → Phase 4. Annotate table rows E3/E7/E15/E16/E17 with their arm
dependency so the table alone carries the caveat.

### [P2-1] The injection-site inventory in the frozen map is false, and the pin's advertised enforcement is overstated

Quote (memo §3.5): "The only `CheckedArtifactFaultKeyV1` injection sites on
the tree today are `catalog_bootstrap.*` in
`capability/pre_catalog/provider/mutation.rs` and
`capability/pre_catalog/provider/directory_mutation.rs`". Quote (memo §8 D3):
"`reserved_fault_families_have_no_injection_sites_before_their_package`
(`:320`) fails the moment a reserved family gains an injection site without
its activation row being flipped in the same package". The RemPlan §10 append
repeats the latter ("fails if a family … gains an injection site while still
declared reserved").

Evidence:

- `provider/aggregate.rs:26-29` at `d32b2c9` is a third production injection
  site — `#[cfg(test)] fault_v1::hit(CheckedArtifactFaultKeyV1::
  CatalogBootstrapCatalogEnumerate)` — structurally identical to the
  mutation.rs sites. `fault_v1::hit` counts at `d32b2c9`: mutation.rs 9,
  directory_mutation.rs 15, aggregate.rs 1 (= 25, matching the 25
  `catalog_bootstrap.*` keys).
- The fixture's `FAULT_INJECTION_SOURCES` doc claims "The complete set of
  production sources that hold `CheckedArtifactFaultKeyV1` injection sites
  today" yet declares only the two files
  (`interface_tests/fault_expected_keys.rs`, the `include_str!` pair) —
  provably incomplete on the day it landed.
- Because the pin scans only declared sources, an injection site added in any
  undeclared file never trips it; the structural direction that does hold is
  the converse (flipping a row to `Executed` fails unless sites exist in
  declared sources, forcing the converting package to register its files).

The substantive frozen fact survives: a tree-wide sweep confirms **zero**
reserved-family sites anywhere (aggregate.rs's site belongs to the executed
`catalog_bootstrap.*`), and the activation-state pin
(`only_the_runtime_and_catalog_bootstrap_families_are_executed_today`) still
makes every flip a deliberate reviewed edit.

Minimal remediation: add `aggregate.rs` to `FAULT_INJECTION_SOURCES`; correct
the §3.5 sentence; re-word the two "fails the moment / fails if … gains an
injection site" claims to state the real guarantee (declared-source scan +
mandatory source registration on flip) — or make the claim true by moving the
reserved-family token scan into the boundary checker, which walks the whole
production tree.

### [P2-2] The operator-ordered Windows spike execution was substituted, under a mis-cited authority

Quote (operator, `GwzFasterProposal.md:65-68`): "Before
`GwzM5-8R2DInterfaceFreeze.md` goes to dual review, **spike the admission
publish/retire path on macOS and Windows** against the already-sealed
`publish_verified_no_replace` family." Quote (memo §4.2): "these two cases
have not been executed on Windows by this package — no native Windows runner
is available in this lane, and the plan assigns native fault evidence to
R2-F (amendment :667)."

Evidence: amendment :667 reads "Linux and Windows native **fault evidence**
remains an R2-F release gate" — it defers fault-injection evidence, not
Track-P spike execution, so it does not cover this deviation. The memo's
substantive substitute is real and disclosed: run-13 (1322/0/1 GREEN at
`90d3f8a`, checkpoint ledger) executes the same primitive over the six sealed
moves including a directory staging→final publish and an active→retired
cross-parent retirement through the `SetFileInformationByHandle` exact-handle
arm, and the two spike tests are ordinary `#[cfg(test)]` cases that will run
in the next Windows matrix. The platform information content of the
substitution is materially equivalent; the process condition, as written by
the operator, was not met and its stated authority is the wrong clause.

Minimal remediation: either execute the two spike cases on the Windows matrix
at `d32b2c9` before acceptance, or record in §4.2 (and the checkpoint) the
explicit acceptance of run-13 as the Windows arm of the Step C.1 spike, citing
the operator's review of this deviation rather than amendment :667 — plus a
tracked acceptance item "both `tests_admission_spike` cases green on the next
Windows matrix run".

### [P2-3] C-2's object is misdescribed: one of the "two enums" is a struct, and the extension is not confined to `publication.rs`

Quote (memo §6): "it adds a variant to **two** `pub(super)` in-memory recheck
**enums** inside `publication.rs`"; (§4.4) "an **admission arm on those two
enums**, added **inside `publication.rs`** under the existing seam."

Evidence: `DirectoryInteriorRecheckV1` is a **struct** with a mandatory
`expected: &CatalogBootstrapRecordV1` field (`publication.rs:34-37`), so the
admission arm is a reshape of that struct or its field type, not "a variant";
and the catalog-typed verification it drives (`interior::staging_plan`,
`interior::completed_record`) lives in `provider/interior.rs`, so the bounded
extension necessarily reaches beyond `publication.rs` (still inside the same
provider owner). The checkpoint pause note already concedes the shape
("admission arms for `DirectoryInteriorRecheckV1.expected` /
`DestinationRecheckV1`"). Durable-state impact: none — §2's ruling is
unaffected. But a freeze that will be quoted to bound Phase 1's change must
describe the change's true shape, or "bounded" is not checkable against it.

Minimal remediation: one corrected sentence in §4.4 and §6 ("a variant on the
`DestinationRecheckV1` enum and a generalization of the
`DirectoryInteriorRecheckV1` expectation, with matching interior-verification
support in the same provider owner").

### [P3-1] Three citations are stale or mislabeled despite the header's re-verification claim

Quote (memo header): "Every file:line in this memo was re-verified against
that tree". Evidence, all at the memo's declared tuple
(`90d3f8a` / root `d6cd2cb`):

- §4.2: "`CATALOG_PUBLICATION_CALL_COUNTS` at `:648-651`" — actually
  `check_checked_artifact_boundaries.py:657-660` (`:648` is inside an
  unrelated list).
- §3.5: "the separate `RuntimeBootstrapFault` enum
  (`bootstrap/runtime/fault.rs:19`)" — the enum is at `:4`; `:19` is the
  injection entry point `run_next_at`.
- §10: "per the adopted lane rule (`CurrentProgramCheckpoint.md:186-204`)" —
  at `d6cd2cb` that range holds operator-escape narrative; the digest-refresh
  lane rule sits near `:364`/`:377`. (Inherited from the plan's earlier
  checkpoint state.)

None is load-bearing. Remediation: fix the three pointers in the same
remediation pass.

### [P3-2] §10's full-suite arithmetic is internally inconsistent

Quote (memo §10): "executed as an exact partition of all **1 367** tests".
Evidence: the memo's own focused-run line (256 passed + 1115 filtered)
implies 1 371 lib tests at `d32b2c9`, and the pause checkpoint records
"1370/1371 pass, 0 fail" with the one unexecuted test (>585 s, pre-existing,
outside the package's file set) named there but not in memo §10. An "exact
partition" claim needs the correct total to be checkable. Remediation:
correct the number and carry the one-unexecuted-test disclosure into §10.

### [P3-3] The RemPlan §10 append asserts "now frozen" while the freeze is DRAFT

Quote (append): "The per-family activation map is now **frozen** by
`GwzM5-8R2DInterfaceFreeze.md` §3.5". The memo is DRAFT pending this mandated
dual review, and the parked-commit discipline elsewhere is explicit about
that. Harmless once the review accepts; until then the RemPlan overstates.
Remediation: either accept as-is on GO, or add "(pending its Step 0.1 dual
review)" if a further docs pass happens first. The append is otherwise clean:
verified append-only (+30 lines after :1038, no existing line modified),
counts match the fixture (19/11/11/13/30/11/16/11 + 25 + 18 = 165), the 21→25
`catalog_bootstrap.*` drift is explained, and the re-reserve clause is
carried.

### [P3-4] §5.2's quarantine analysis should say the relocation move's execution is out-of-lane

Quote (memo §5.2): "quarantine/relocation **moves the legacy private area
first** and then converts the same four edges through the same P1 arms."
Plan §5 item 7 keeps MAX_PATH/relocation **execution** out of R2-D ("Phase
4.3 only *decides* compatibly"), and the relocation move is accordingly not
an E-row. The sentence as written can be read as scheduling the move inside
R2-D. One clarifying clause ("the move itself executes on the R2-F-candidate
relocation package, on the same P1 family") removes the ambiguity. The
substantive discharge of owner decision §9.2 — both options draw on the same
primitive set, so 4.3 is a coexistence/MAX_PATH question, not a platform
question — is sound.

## 4. Mandate items verified clean (no finding)

1. **Nine-step admission sequence vs the §3.1 seam** (ConsumerCheckpoint §7
   :203-224): steps 1-2 are the coordinator's (R1 classifier
   `CatalogAdmissionOwnerV1::classify_handoff/admit`,
   `protocol/admission/owner.rs:82-124`, keyed on
   `expected: &ActionCapacityReservationV1` — verified); steps 3-9 including
   resume and the final reobservation live behind
   `resume_or_admit(&mut self, expected) -> Result<AdmittedActionV1, _>`.
   Internal helpers do not cross the seam; the exposed-item pin (count = 3)
   does not constrain private additions; fault injection is ambient
   (`#[cfg(test)] fault_v1::hit(..)` at the site), so activating
   `admission.*` needs no signature change. The seam is sufficient.
2. **Restart observations**: the four now-required methods include the
   `observe_installed_managed_component` / `observe_retired_managed_marker`
   restart pair — ConsumerCheckpoint §8 (:228-231) is discharged structurally;
   verified against the committed trait, the three-impl inventory
   (`provider_compile.rs:153`, `test_support.rs:137`/`:465`), the
   byte-identical relocated refusal, and the compiler now enforcing the
   property for any future impl.
3. **LeafObserver two-sided absence**: `DurableLeafExpectation::Missing` +
   `DurableLeafProof::MissingDurable` (leaf.rs) carry the two-sided durable
   absence proof inside the frozen signature; the memo's zero-impls
   correction to the plan is verified (tree-wide, only the trait declaration
   exists), a genuinely material catch.
4. **ManagedParentBootstrap**: the four purposes, overlap rule, cardinality,
   ordering/mask, fallible reservation, and per-row prefix rejection are
   owner-enforced (`bootstrap/managed/owner.rs:34+`, spot-verified
   `:69-77`); the provider seam needs no purpose knowledge; writers receive
   `Self::RetainedParents` only. Sufficient for Phase 3.1.
5. **Activation map**: 165 keys total and all ten per-family counts verified
   arithmetically from the committed fixture; map totality, per-family size
   pins, and the sum==`EXPECTED_KEY_COUNT` assertion are in the tests;
   `runtime.*` is correctly recorded executed on the separate
   `RuntimeBootstrapFault` mechanism (enum `fault.rs:4`, driver `:19`,
   exercised at the cited test lines — all verified) with the fixture
   exception explicit. The 25-key `catalog_bootstrap.*` figure is confirmed
   (25 hit-sites, 25 keys).
6. **Owner decisions §5**: all six transcriptions checked word-against-word
   with plan §9 — no re-deciding, no drift; the quarantine **direction**
   wording ("quarantine/relocation preferred … convert-in-place remains live
   as fallback") and the dirent-barrier **conditional** ("ratified as
   written", decision point unchanged at 4.2/4.3) are exact.
7. **Three-dual tier statement (§9)**: grounded verbatim in operator
   authority (`GwzFasterProposal.md` §2, quoted at checkpoint d6cd2cb:72-81);
   the supersession of the plan's Phase 3/4 settle duals is the operator's,
   not the memo's.
8. **Track-P table platform columns**: every Windows arm cites real code at
   the declared tuple (exact-handle rename `:48-76`/`:98-157`, absolute
   destination `:196-259`, share-delete open `:165-194`, no-op `sync_parent`
   `:372-377` with the P5 anchor `:458-514`), all exercised green in the
   run-13 Windows matrix; E18-E21 legacy sites and the `:420` random-scratch
   stop-clause violation verified; the allowlist comment naming
   transition.rs/residue.rs as "the four legacy leaf edges, retired by R2-D"
   confirmed. **No new platform primitive** is the right verdict at the
   platform level.
9. **Gates re-executed on the parked object**: boundary checker — the only
   failure is the other lane's dirty `v1_lifecycle` tree digest; zero
   checked-artifact findings, confirming the committed digests are correct
   and the publication-seam/module/bare-token/provisional rules pass.
   `cargo test --lib checked_artifact::` → **256 passed / 0 failed**,
   reproducing memo §10 exactly. The commit's checker delta is two digest
   lines only, per the adopted lane rule.
10. **Zero-new-durable-records (§6)**: verified — the diff adds no record,
    slot, purpose, action, or phase and touches no protocol source; the
    schema gate is unchanged and green.

## 5. Round-2 instruction

All remediations are documentation/fixture edits: P1-1 (§4.4/C-2 class
definition + table annotations), P2-1 (fixture source registration + two
sentence corrections), P2-2 (Windows-spike acceptance recording or
execution), P2-3 (shape sentence), P3-1..P3-4 (pointers, arithmetic, status
clause, one clarifying clause). None requires touching the frozen seams or
the parked code beyond `fault_expected_keys.rs`'s `FAULT_INJECTION_SOURCES`
entry. With those landed, this axis expects to confirm C-1 (as annotated) and
C-2 (as generalized) and issue GO in round 2.

---

# Focused re-verdict (round 2) — 2026-08-22

Axis: **State / durable-state** (focused re-verdict on this axis's round-1
findings plus the round-2 new material, per the coordinator's instruction;
round 2 of the two-round cap). Peer-blind: the Code-axis report remains
unread.
Object: gwz-core `2ed2435` (round-2 remediation) + `c40e712` (ReviewCode
round-2 P1-3 pin fix) on top of the parked `d32b2c9`; root `7ecc1a7` +
`85f9fe6` (memo revised 2026-08-22, now 1,069 lines; RemPlan §10 correction
block, +27/−0).

**Verification basis.** Committed state only (`git show` at the commits
above). Unlike round 1, no gate was re-executed here: the gwz-core working
tree now carries the Phase-1 lane's uncommitted files (`admission/tests.rs`,
a modified `admission/mod.rs`, a modified checker), so any tree run would
test that lane, not this object. The basis is: committed-state reading and
static verification, this axis's round-1 executions (256/0, checker
object-clean), the lane's attributed round-2 runs recorded at memo §10.2, and
independent GitHub corroboration of the Windows spike run (below).

## 6. Round-1 findings — per-finding status

**[P1-1] RESOLVED.** §4.4 is rewritten as "the two in-seam extension
classes"; Class 1 defines the recheck-arm class once, in substance the class
rule this review drafted (in-seam; no platform surface; acquisition window,
identity compare, retained handle, no-replace rename untouched; caller-pin +
same-commit digest discipline), and assigns **every** arm with an owner:
admission source-interior/destination → Phase 1 (C-2), managed
source-interior → 2.3/3 (E15, E17), managed destination → 2.3/3 (E16
conditional — honest, since the marker is leaf-shaped today — and E17),
further retirement-destination → Phase 4. The §4.3 table now carries the
dependency on rows E3/E7/E15/E16/E17 itself, with a preamble stating that an
arm is an extension of an admitted primitive, not a new one. §2's C-2 is
reworded as the class contingency with the admission arms Phase-1-owned, and
names round 1's "one bounded extension" as false. The "managed rows are not
optional" paragraph carries the structural argument this review's finding
rested on (mandatory `interior` field; §8.13 + checker fail-closed).
Verified against the committed memo text.

**[P2-1] RESOLVED.** Verified in the committed code at `c40e712`:
`aggregate.rs` is declared in `FAULT_INJECTION_SOURCES`, and the new
completeness anchor
(`the_declared_injection_sources_are_every_production_source_holding_sites`)
rescans `src/checked_artifact/` at test time under the checker's own
production-file rule and asserts set equality both ways. Soundness checks
done here: `fault_v1.rs` contains no `CheckedArtifactFaultKeyV1::` token (no
self-trip); a tree-wide token sweep at `c40e712` finds exactly the three
declared production files (plus two `interface_tests` files the walk
correctly excludes), so the anchor passes as committed and fails on any
unregistered file; the `pub(super)` visibility argument making the subtree
scan exhaustive is correct. §3.5 now states the true inventory (3 sources,
15+9+1 = 25 sites = 25 keys — matching this review's counts) and scopes the
reserved-family scan claim honestly ("in a declared source") with the anchor
supplying tree-completeness; §8 D3 matches; the RemPlan §10 correction block
is append-only (+27/−0, verified) and restates both corrections.

**[P2-2] RESOLVED — on the stronger option (executed, not waived).**
Independently corroborated here: `gh run view 32542469665` reports
conclusion **success**, workflow "Windows matrix", branch
`probe/track-p-spike`, headSha `e448d7f1c989548e57316da11d76321f0a4a2cf1`;
the local object `e448d7f` differs from `d32b2c9` by a 2-line
`.github/workflows/windows-matrix.yml` trim **only** (verified by diff), so
the run executed the parked object's spike source on native Windows; memo
§10.3 records the per-test lines ("ok. 2 passed; 0 failed"). §4.2 now
discharges both legs of Step C.1 (naming and execution), explicitly names and
retracts round 1's amendment-`:667` mis-citation (fault evidence ≠ spike
execution — the exact category error this review flagged), demotes run-13 to
corroboration, and files the tracked acceptance item "both spike cases green
on the next full Windows matrix run".

**[P2-3] RESOLVED.** §4.4 Class 1 "the surface today, and its true shape"
and §6's C-2 bullet now state the code-accurate shape: a variant on the
`DestinationRecheckV1` enum, a generalization of the
`DirectoryInteriorRecheckV1` struct's mandatory catalog-typed `expected`
field, and verification reach into `provider/interior.rs` within the same
owner. Matches the code verified in round 1.

**[P3-1] RESOLVED.** All three pointers fixed: checker dict cited at
`:657-660` (§4.2, with the round-1 error named); `RuntimeBootstrapFault` enum
at `fault.rs:4-11` with `run_next_at` at `:19` (§3.5); lane rule re-cited to
`CurrentProgramCheckpoint.md:483`/`:495-497` at root `13b98df` (§10.1) — both
lines verified verbatim here ("digest refresh as the literal last pre-commit
step").

**[P3-2] RESOLVED.** §10.1 corrects the partition total to **1,371** lib
tests (naming round 1's self-contradiction) and carries the
one-unexecuted-test disclosure (>585 s `workspace_ops` matrix test,
pre-existing, green in CI at run 13) into §10 itself.

**[P3-3] RESOLVED.** RemPlan correction ¶2 adds the "frozen … on acceptance"
reading, append-only, original sentence untouched.

**[P3-4] RESOLVED.** §5.2 now states the relocation move executes on the
R2-F-candidate package, not inside R2-D, and why it is therefore not an
E-row.

## 7. New material — C-3 and the §3.1 persisted-home pin

**C-3's code facts: all verified here against the committed object**
(`d32b2c9`; `interior.rs` untouched by the round-2 commits):

1. `exact_slot` walks `InfrastructureSlotV1::ALL` and refuses any other
   catalog-root child ("catalog directory contains an unowned child") — so
   the `action-<hex>-v1` row that `RootEntryNameV1::ActiveAction(action)
   .name()` renders, and that the **durable admission record commits to**
   (`ActionDirectoryAdmissionV1::preparing`, `final_action_name` at
   `protocol/admission.rs:156`), is refused by today's observer.
   `RootEntryNameV1::parse` has no production caller (verified: test callers
   only).
2. `staging_plan` returns `Other` and `completed_record` returns `None`
   whenever any `ActionAdmission{Active,Scratch,Staging}` slot is present
   (both `any_present` guards read directly in the committed code).
3. `MAX_INTERIOR_ENTRIES = 10` is enforced during `observe` ("catalog
   interior exceeds the ten-slot bound") and equals `|ALL|` — zero headroom.
4. The consequence chain is exactly as the memo prints it and was re-traced
   here: `recover_or_create` (`catalog/bootstrap.rs:145-149`) →
   `execute_owner_complete` (`capability/pre_catalog.rs:297-308`) →
   `retain_completed_catalog` (`completed.rs:49`), which refuses when
   `interior::completed_record(..)` is `None` (`:61`); the retained-catalog
   revalidation re-runs `interior::observe` on the Final directory
   (`completed.rs:178+`), so both the fresh-recovery and revalidation paths
   break. One sharpening this review adds: the nine-step sequence persists
   `Idle → Preparing` (step 3) **before** publishing, so the admission
   active/scratch slot files exist from the first admission **attempt** — the
   breakage does not wait for a completed admission. The memo's "Phase 1's
   first admission breaks the next `recover_or_create` reobservation" (§2
   C-3) is therefore accurate and, if anything, conservative.

**Ruling on C-3: REAL, MATERIAL, and correctly handled.** Without it, Phase
1's first write would permanently brick every subsequent catalog
reobservation — the exact durable-state unsoundness this axis exists to
catch, and a gap of the same kind as round-1 P1-1 (an unrecorded in-seam
extension a later step must make). Folding it into the freeze as a third
named contingency with Phase-1 ownership is the correct freeze-level
treatment, and its placement in round 2 is legitimate scope: it is
lane-disclosed new material (found by the §9-permitted Phase-1 test
drafting), adds no code to the package, reshapes none of the five frozen
seams (the extension sits behind `OpaqueRetainedCatalogV1` inside the
provider owner), and is put before this review to confirm — not smuggled past
it. **Track-W adjudication confirmed NOT Track-W**: the extension widens what
the provider observer *accepts and calls complete*; every durable item it
reads — the `ActionAdmission*` triad, the `RootEntryNameV1` two-arm grammar,
the admission record fields — is already in the frozen R1+C0 vocabulary
(verified), and `MAX_INTERIOR_ENTRIES` is a provider-side constant, not a
durable record. Class 2's definition, ownership, and rules (same discipline
as the recheck arms; "the cap and the grammar have to move together") are
adequate for Phase 1 to build against without a further freeze round.

**§3.1 persisted-home pin: confirmed, code-true.** The three
`ActionAdmission*` slots exist in the enum, in `ALL`, and as rendered names
(`protocol/slots.rs`, verified round 1); the durable infrastructure record
already **issues** the triad (`infrastructure_record.rs:100-104`) and its
decoder **refuses any other value** ("infrastructure record binding
mismatch", `:181-193` — verified here); the admission record itself names the
staging slot (`admission.rs:153-155`). So the pin is not merely a memo
assertion — the binding is machine-enforced by the record codec, which is
stronger than a test pin. It grounds §6's zero-new-durable-records rule
structurally for admission ("Phase 1 adds no new slot, no new record, and no
new name") and correctly makes C-3 a grammar obligation, not a vocabulary
one.

**Contingency confirmations (this axis):** **C-1 confirmed** — the §4.3
table is per-platform-complete and correct, and now carries its own arm
dependencies; **C-2 confirmed as generalized** — the recheck-arm class is an
in-seam extension class, not a new primitive and not a bypass, with the
admission arms Phase-1-owned; **C-3 confirmed** — an in-seam observer/slot-
grammar extension in the same provider owner, Phase-1-owned, not a Track-W
event.

## 8. New finding this round

**[P3-5, round 2] Two stale "owed to the lane owner" sentences contradict
the commits that carry them.** Memo §4.2's closing paragraph ("the one-line
comment edit is owed to the lane owner at the acceptance commit") and the
header's Code-P3-4 bullet still describe the `tests_admission_spike.rs:84`
citation fix as pending — but twin commit `2ed2435` carries exactly that edit
(`:1082-1085` → `:1089-1092`, verified in the diff), `c40e712` refreshed its
tree pin, and §10.2 (rewritten in the pin-fix commit) records the true
sequence. Two sentences to clean at the acceptance/docs pass; no frozen fact
is affected. Files-and-continues per the operator cap; not a gate.

## 9. VERDICT (round 2): **GO** — State axis

All round-1 findings on this axis are resolved (P1-1, P2-1, P2-2, P2-3,
P3-1..P3-4); the round-2 new material (C-3, §3.1 pin) is verified and
adequate; the only new finding is P3-5, informational. Zero P0/P1/P2 open on
this axis.

Items traveling with the package to acceptance (already recorded in the memo
or filed here): (a) both `tests_admission_spike` cases green on the next
**full** Windows matrix run (§4.2/§10.3); (b) the P3-5 sentence cleanup;
(c) the pre-existing >585 s `workspace_ops` matrix test confirmed on the next
matrix run (§10.1, checkpoint-tracked); (d) the §9 tier-recording duty at the
freeze commit (lane owner).
