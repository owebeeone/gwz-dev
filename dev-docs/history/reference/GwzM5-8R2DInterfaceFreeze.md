# R2-D interface freeze — admission, namespace backend, leaf, and managed-parent seams

Date: 2026-08-16; **revised 2026-08-22 (round 2)**.

Status: **FROZEN — ACCEPTED 2026-08-22 at GO/GO** (dual #1 of R2-D's
three retained duals). Code axis: round-2 GO after the [P1-3] pin
refresh at `c40e712`, verified on a pristine extraction. State axis:
round-2 GO — all round-1 findings resolved; C-3 ruled real, material,
Phase-1-owned, and not Track-W; the §3.1 persisted-home pin verified
machine-enforced; one informational [P3-5] cleaned in this acceptance
pass. History: round-1 NO-GO/NO-GO (2 P1 + 1 P1/3 P2), one merged
remediation at `2ed2435`, the pin fix at `c40e712` — within the
two-round cap. Tracked to the next full Windows matrix run: both
Track-P spike cases green natively. This is R2-D Phase 0 Step 0.1 of
the adopted plan, landed together with the Step 0.2 scaffolding under
**one** dual peer-blind review (adopted plan §4 Step 0.2 "Rides Step
0.1's review"; `GwzFasterProposal.md` §3 Step C.2). Nothing physical
mutates in this package. The five frozen seams, the two extension
classes (C-2 recheck arms, C-3 observer grammar) with their per-phase
assignments, and the §3.1 persisted-home pin are now binding on
Phases 1-5.

Round-1 verdicts were NO-GO on both axes
(`GwzM5-8R2DInterfaceFreeze-ReviewCode.md`: 2 × P1, 8 × P3;
`GwzM5-8R2DInterfaceFreeze-ReviewState.md`: 1 × P1, 3 × P2, 4 × P3). This
revision addresses **every** finding on both axes:

- **Code P1-1 / State P2-2** — the Windows arm of the Step C.1 spike is now
  *executed*, not substituted; recorded at §4.2 with the run identity, plus a
  tracked acceptance item for the next full matrix run.
- **Code P1-2 / State P2-1** — the injection-site inventory is corrected to
  three production sources (§3.5), the fixture declares the third
  (`FAULT_INJECTION_SOURCES` :257), and a completeness **anchor** now pins the
  declared set against a scan of the production tree (§8 D3), so the
  "machine-checked" claim in the RemPlan §10 annotation is delivered rather
  than qualified. (Activation update 2026-08-22: Step 1.3 added the fourth
  production source, `provider/admission_mutation.rs` with 19 sites — the
  fixture's declaration moved to :271 and the runtime-exception block to
  :356-364 by that insertion; the anchor verified the fourth source the
  designed way.)
- **State P1-1 / State P2-3** — §4.4 no longer claims "one bounded extension".
  It defines the recheck-arm **class** once, gives its true shape, and assigns
  the arms per phase; §4.3 rows E3/E7/E15/E16/E17 carry the dependency.
- **New in this revision (lane owner)** — a **third** bounded extension class,
  the pre-catalog interior observer/slot grammar, is named as **C-3** (§2,
  §4.4), and §3.1 pins the admission record's persisted home to the three
  already-frozen `ActionAdmission*` infrastructure slots.
- **Code P3-1..P3-8, State P3-1..P3-4** — all corrected in place. Code
  P3-4's one-line citation fix inside `tests_admission_spike.rs` LANDED
  in the round-2 commit `2ed2435` (with the matching pre_catalog tree
  pin refreshed at `c40e712` after the Code round-2 [P1-3] catch);
  Code P3-8 is an environment note about another lane's working-tree
  files, no memo change possible. (State round-2 [P3-5] cleanup.)

Verified tree: gwz-core `d32b2c9cb67a0d25ddb2dd047e1c5d1a70d0c39b` (the parked
package commit), workspace root `13b98df`. **Every file:line in this memo is
stated at that tuple**, including this package's own files — the round-1
baseline mixed pre-package (`90d3f8a`) and post-package positions in §3.2,
which is Code P3-7 and is fixed by the re-baseline. Positions that are quoted
*as they were before the package* are labelled `90d3f8a` explicitly. The plan's
own citations were taken at `f36d20d` and several have drifted, so §7 lists
each correction. Round-2 exception: `interface_tests/fault_expected_keys.rs`
positions are those of this revision's working tree, which the lane owner
commits on top of `d32b2c9` (§8 D3, §10).

## 1. Controlling documents

- `GwzM5-8R2D-Plan.md` — **ADOPTED** 2026-08-16. §4 Phase 0 is this package's
  specification; §4-end is the fault-family activation map; §9 is the adoption
  record whose six §7 dispositions are transcribed at §5 below; §2.2 lists the
  amendment clauses that bind every phase.
- `GwzM5-8R2CCatalogBootstrapAmendment.md` — §4.1 sealed publication seam
  (:267-316 plus the 2026-08-15 erratum :298-316); §7 owner cohesion
  (:569-577); §8 TDD/exit gates, in particular §8.10 (:643-645) and §8.13
  (:656-659); the R2-D unblock gate at :665-667.
- `GwzM5-8R4bR2ConsumerCheckpoint.md` — §6 bounded global enumeration
  (:184-201), §7 admission and capacity (:203-224), §8 namespace/leaf/authority
  execution (:226-245), §9 managed-parent bootstrap (:247-266), §11 file
  ownership (:288-326), §12 TDD and fault closure (:328-353), §14 stop
  conditions (:373-386).
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` — §8 items 15-19 (:572-603),
  §10 fault-key vocabulary scope (:1025-1038).
- `GwzFasterProposal.md` — operator instruction, 2026-08-16. §2 caps R2-D at
  three dual reviews and requires Track-P evidence per platform *before* this
  freeze is reviewed; §3 Step C is this package's order of work. Quoted into
  `CurrentProgramCheckpoint.md:44-81` ("Operator decision 2026-08-16 — thin
  A1"), which is the authority cited by §9 below.
- `GwzM5-8ThinA1Amendment.md` — the bounded gate-chain amendment filed by the
  lane owner under Step B. This memo does not restate or extend it.
- Process authority: `AgentProcessRules.md` as amended, `GwzProcessOptimization.md`
  §3.1 (two-track freeze / platform spike), §4.1 (two-round cap), §4.2
  (tiering), §4.3 (cross-model), §4.4 (pipeline lanes).

## 2. What this freeze does and does not do

Freezes, as they exist on the tree after this package:

1. the physical admission driver seam (new owner `checked_artifact/admission/`);
2. the `ActionNamespace` raw-backend delta that turns the four
   `managed_operation_unavailable` defaults into required methods;
3. the `LeafObserver` provider seam that Phase 2.1 implements;
4. the `ManagedParentBootstrap` provider seam that Phase 3.1 implements;
5. the per-phase fault-family activation map.

It does **not** add a durable record, a production caller for
`recover_or_create`, a consumer conversion, or any filesystem edge. The
defer-out list at plan §5 items 1-7 is unchanged and is reaffirmed by the thin-A1
decision, which orders that residual accepted through A1
(`CurrentProgramCheckpoint.md:56-62`).

**Freeze contingency (operator instruction).** `GwzFasterProposal.md` §2
"Track P before the freeze is reviewed" forbids freezing the four managed
defaults into required methods "until each new physical edge names an admitted
primitive per platform", and §3 Step C.1 orders the admission publish/retire
path spiked "on macOS **and** Windows" before the freeze goes to review. That
instruction has two legs and §4 now discharges **both**:

- *naming* — every R2-D physical edge names an admitted primitive with a macOS
  arm and a Windows arm (§4.3), verdict **no new platform primitive** (§4.4);
- *execution* — the two spike cases ran green on macOS **and on Windows**
  (§4.2). Round 1 recorded only the macOS arm and offered run-13 as a
  substitute; the Windows arm has since been executed directly, so no
  substitution and no operator waiver is needed (Code P1-1, State P2-2 both
  discharged by their "execute on Windows" option, not by their
  "record a disposition" option).

The delta is therefore implemented in this package, and the freeze is recorded
as **contingent on three facts that the review must confirm**:

- **(C-1)** the Track-P table at §4.3 is per-platform-complete and correct;
- **(C-2)** the **recheck-arm extension class** defined at §4.4 — arms on the
  sealed primitive's *source-interior* expectation and *destination* recheck
  surface — is accepted as an in-seam extension class whose **admission** arms
  are owned by Phase 1, not as a new primitive and not as a bypass. Round 1
  called this "the one bounded extension"; that was false (State P1-1) — the
  managed edges need arms of the same class in Phases 2.3/3, and §4.4 now
  defines the class once and assigns every arm;
- **(C-3)** the **observer/slot-grammar extension class** at §4.4 — the
  pre-catalog interior observer at
  `capability/pre_catalog/provider/interior.rs` admits only the ten
  `InfrastructureSlotV1::ALL` children and treats a present `ActionAdmission*`
  slot as *not complete*, so Phase 1's first admission breaks the next
  `recover_or_create` reobservation — is accepted as an in-seam extension in
  the same provider owner, owned by Phase 1, on the same rules as the recheck
  arms.

If the review rejects any of the three, the trait delta reverts to paper-freeze
and the Phase 1 step re-spikes first; the delta is confined to
`namespace/backend.rs` and `namespace/test_support.rs` precisely so that
reverting it is a two-file operation.

## 3. The five frozen seams

### 3.1 Seam 1 — physical admission driver (`checked_artifact/admission/`)

New cohesive owner, `gwz-core/src/checked_artifact/admission/mod.rs:28-64`,
declared at `checked_artifact/mod.rs:8-12`. Frozen signature:

```rust
pub(in crate::checked_artifact) struct ActionAdmissionOwnerV1<'lease> {
    catalog: OpaqueRetainedCatalogV1<'lease>,
}

impl<'lease> ActionAdmissionOwnerV1<'lease> {
    pub(in crate::checked_artifact) const fn from_retained_catalog(
        catalog: OpaqueRetainedCatalogV1<'lease>,
    ) -> Self;

    pub(in crate::checked_artifact) fn resume_or_admit(
        &mut self,
        expected: &ActionCapacityReservationV1,
    ) -> Result<AdmittedActionV1, CheckedFsError>;
}
```

Properties this shape makes structural rather than advisory:

- **Consumes `OpaqueRetainedCatalogV1` only.** That type is constructible only
  by the sealed catalog owner's completed recovery
  (`catalog/bootstrap.rs:128-137`, produced at `:210-214`; sole production
  entry `recover_or_create` at `catalog/bootstrap.rs:145-149`, which itself
  takes a target-bound lease and revalidates through `begin_preflight` at
  `:155`). Admission therefore cannot run against a caller-supplied root, lease
  bytes, token bytes, raw role rows, an expected record, a bootstrap callback,
  or a synthetic observation — amendment §8.10 (:643-645), ConsumerCheckpoint
  §14 first bullet (:377-378).
- **Returns `AdmittedActionV1` only** — an opaque handoff of
  `{reservation, directory_identity}` with accessor-only reads
  (`protocol/admission.rs:281-294`). No raw handle and no mutation capability
  crosses the seam, per amendment §7 (:576-577).
- **Takes the coordinator-derived reservation as its expectation**, matching the
  R1 classifier already in place
  (`CatalogAdmissionOwnerV1::classify_handoff`/`admit`,
  `protocol/admission/owner.rs:82-124`, both keyed on
  `expected: &ActionCapacityReservationV1`). The physical driver is the missing
  half of that classifier, not a second decision surface.

**The admission record's persisted home is already frozen; Phase 1 invents no
name.** The three infrastructure slots the physical driver writes through exist
in the R1+C0 slot vocabulary today, as the active/scratch/staging triad of the
catalog-bootstrap pattern:

- `InfrastructureSlotV1::ActionAdmissionActive` / `ActionAdmissionScratch` /
  `ActionAdmissionStaging` — declared in the enum at
  `protocol/slots.rs:47-58` (the ten-slot catalog-root grammar), listed in
  `InfrastructureSlotV1::ALL` at `:61-72`, and named
  `"action-admission-active-v1"` / `"action-admission-scratch-v1"` /
  `"action-admission-staging-v1"` at `:83-85`;
- carried in the durable infrastructure record: issued into
  `admission_active_name` / `admission_scratch_name` / `admission_staging_name`
  at `protocol/infrastructure_record.rs:102-104`, and re-checked against the
  same three slot components on decode at `:181-193`, which refuses any other
  value ("infrastructure record binding mismatch", `:191`);
- already named by the admission state record itself —
  `ActionDirectoryAdmissionV1::preparing` sets
  `staging_name: InfrastructureSlotV1::ActionAdmissionStaging.name()` at
  `protocol/admission.rs:153-155`.

So Phase 1 adds **no new slot, no new record, and no new name**: it drives the
existing triad. This is the concrete reason §6's zero-new-durable-records rule
is not merely unbroken but structurally unreachable for admission — and it is
also why C-3 is a *grammar* obligation rather than a vocabulary one (§4.4).

Body today: a typed refusal in house style
(`CheckedFsError::ambiguous("action admission", "physical admission driver is
implemented in R2-D phase 1")`, `admission/mod.rs:59-64`) — the same idiom the
removed backend defaults used. Phase 1.2 replaces it with the nine-step durable
sequence of ConsumerCheckpoint §7 (:209-224). The skeleton is compile-checkable
and boundary-legal: the boundary checker's module rules, raw-rename scan,
catalog-lease reference sets, and provisional-interface scan all pass over it
unchanged (§10.1, §10.2).

### 3.2 Seam 2 — `RawNamespaceBackend` managed-operation delta

`gwz-core/src/checked_artifact/namespace/backend.rs:375-457` (`d32b2c9`;
`SealedActionNamespace` follows at `:459`). Before this package the four managed
operations carried `Err(managed_operation_unavailable())` default bodies at
`90d3f8a`:`411-451` with the helper at `90d3f8a`:`460-465`. After it they are
**required**:

```rust
pub(in crate::checked_artifact) trait RawNamespaceBackend {
    type DirectoryHandle;
    type ObjectHandle;
    type Identity: Clone + Eq;
    type PathProfile;

    fn provider_binding(&self) -> ProviderBinding;

    fn revalidate_action_directory(
        &mut self,
        expected_identity: &DurableObjectIdentityV1,
        expected_reservation: RecordDigestV1,
    ) -> Result<(), CheckedFsError>;

    fn publish_no_replace(
        &mut self,
        source: &RetainedNamespaceObject<
            Self::DirectoryHandle, Self::ObjectHandle, Self::Identity, Self::PathProfile,
        >,
        destination: &ActionDestination,
    ) -> Result<PublishedIdentity<Self::Identity>, CheckedFsError>;

    fn retire_exact(
        &mut self,
        source: &RetainedNamespaceObject<
            Self::DirectoryHandle, Self::ObjectHandle, Self::Identity, Self::PathProfile,
        >,
        destination: &ActionDestination,
    ) -> Result<RetiredIdentity<Self::Identity>, CheckedFsError>;

    // required from this freeze forward (was: four defaulted methods)
    fn install_managed_component(
        &mut self,
        source: &RetainedNamespaceObject<
            Self::DirectoryHandle, Self::ObjectHandle, Self::Identity, Self::PathProfile,
        >,
        destination: &ActionDestination,
        request: &ManagedInstallRequestV1,
    ) -> Result<ManagedInstallObservationV1, CheckedFsError>;

    fn observe_installed_managed_component(
        &mut self,
        request: &ManagedInstallRequestV1,
    ) -> Result<ManagedInstallObservationV1, CheckedFsError>;

    fn retire_managed_marker(
        &mut self,
        source: &RetainedNamespaceObject<
            Self::DirectoryHandle, Self::ObjectHandle, Self::Identity, Self::PathProfile,
        >,
        destination: &ActionDestination,
        request: &ManagedMarkerRetirementRequestV1,
    ) -> Result<ManagedMarkerRetirementObservationV1, CheckedFsError>;

    fn observe_retired_managed_marker(
        &mut self,
        request: &ManagedMarkerRetirementRequestV1,
    ) -> Result<ManagedMarkerRetirementObservationV1, CheckedFsError>;

    fn barrier(
        &mut self,
        parent: &RetainedDirectory<Self::DirectoryHandle, Self::Identity, Self::PathProfile>,
        ordinal: BarrierOrdinalV1,
    ) -> Result<DurableNamespace, CheckedFsError>;
}
```

**Behaviour is unchanged by construction.** The complete implementation set on
this tree is three impls, and two of them already stated all four operations
before the delta:

**All positions in this table are at `d32b2c9`** (post-package); the one
pre-package position that matters is given inline and labelled `90d3f8a`.

| Implementation | Site (`d32b2c9`) | Before | After |
| --- | --- | --- | --- |
| `ProductionShapedBackend` (always-compiled provider shape) | `namespace/provider_compile.rs:153-281` | states all four (`:204`, `:219`, `:239`, `:254`) — unmoved by this package | unchanged |
| `RecordingNamespaceBackend` (test backend) | `namespace/test_support.rs:465-607` | states all four; at `90d3f8a` the impl was `:412-554` with the ops at `:471`/`:488`/`:508`/`:525` | unchanged in behaviour; the ops now sit at `:524`, `:541`, `:561`, `:578`, shifted only by the `+53` lines this package adds above them |
| blanket legacy adapter `impl<Protocol: NamespaceProtocol>` | `namespace/test_support.rs:137-234` | inherited the four defaults | states the four explicitly (`:185`, `:199`, `:206`, `:220`), returning the identical `CheckedFsError::ambiguous("managed namespace operation", "provider does not implement exact post-observation")` from the relocated helper at `test_support.rs:243-248` |

The sentinel constructor moved from the production trait file into the
test-only adapter (`test_support.rs:243-248`), byte-identical in variant, fact
string, and detail string. The only type reaching the blanket adapter is
`RecordingNamespace` (`interface_tests/contracts.rs:236`), which never calls a
managed operation; the full suite is unchanged (§10.1).

This discharges ConsumerCheckpoint §8 (:230-231) — "Default 'unavailable'
managed operations are not reachable in production" — structurally rather than
by convention, and executes adopted decision §9.3 (§5.3 below).

### 3.3 Seam 3 — `LeafObserver` provider contract (Phase 2.1)

`gwz-core/src/checked_artifact/leaf.rs:70-101`, unchanged by this package and
frozen as-is:

```rust
pub(super) trait LeafObserver {
    type DirectoryHandle;
    type Identity: Clone + Eq;
    type PathProfile;

    fn observe_bounded(
        &self,
        parent: &RetainedDirectory<Self::DirectoryHandle, Self::Identity, Self::PathProfile>,
        leaf: &AsciiComponent,
        max_bytes: usize,
    ) -> Result<LeafProof<Self::Identity>, CheckedFsError>;

    fn observe_durable<Content, Protocol>(
        &self,
        parent: &RetainedDirectory<Self::DirectoryHandle, Self::Identity, Self::PathProfile>,
        leaf: &AsciiComponent,
        expected: DurableLeafExpectation<'_, Content>,
        namespace: &mut Protocol,
        barrier_ordinal: Protocol::BarrierOrdinal,
    ) -> Result<DurableLeafProof<Self::Identity>, CheckedFsError>
    where
        Content: ExpectedLeafContent + ?Sized,
        Protocol: NamespaceProtocol<
                DirectoryHandle = Self::DirectoryHandle,
                Identity = Self::Identity,
                PathProfile = Self::PathProfile,
            >;
}
```

Phase 2.1's production implementation must satisfy, against this signature:
bounded/fallible allocation keyed to `max_bytes` (never an unbounded read —
ConsumerCheckpoint §14 third bullet); one retained handle owned across exact
proof, flush, namespace barrier, and exact reobservation (the doc contract at
`leaf.rs:70-73`); two-sided durable absence through `DurableLeafProof::MissingDurable`
(`leaf.rs:38-46`); and payload size never conflated with protocol-record size
(ConsumerCheckpoint §8 :236-237). `ExpectedLeafContent` (`leaf.rs:57-63`)
already forbids assuming one reader can be rewound between the two
comparisons.

**Correction to the plan's premise.** Plan §2.3 item 5 and §4 Step 0.1 state
the "sole impls" are at `interface_tests/contracts.rs:183/:236`. On this tree
`LeafObserver` has **zero** implementations anywhere (`grep -rn LeafObserver`
returns only the trait declaration); `contracts.rs:183` implements
`ExpectedLeafContent for Bytes` and `contracts.rs:236` implements the test-only
`NamespaceProtocol for RecordingNamespace`. Phase 2.1 therefore writes the
first implementation of this trait, and there is no existing test
implementation to keep in parity. See §7.

### 3.4 Seam 4 — `ManagedParentBootstrap` provider contract (Phase 3.1)

`gwz-core/src/checked_artifact/bootstrap/managed/owner.rs:13-32`, unchanged by
this package and frozen as-is:

```rust
pub(in crate::checked_artifact) trait ManagedParentBootstrap {
    type RetainedParents;

    fn provider_instance_id(&self) -> [u8; 32];

    fn observe_preflight(
        &self,
        request: &ManagedParentBootstrapRequest,
    ) -> Result<Vec<ManagedParentObservationV1>, CheckedFsError>;

    fn revalidate_plan(&self, plan: &ManagedParentPlanV1) -> Result<bool, CheckedFsError>;

    fn execute_bound(
        &self,
        plan: &BoundManagedParentPlanV1,
    ) -> Result<Self::RetainedParents, CheckedFsError>;
}
```

The owner that consumes it, `ManagedParentBootstrapOwnerV1`
(`bootstrap/managed/owner.rs:34-41`), already enforces the properties Phase 3.1
must not weaken: authority validation before any observation
(`:69`), provider-instance binding (`:70-71`), exact plan cardinality
(`:73-77`), declared-purpose ordering and mask (`:79-91`), fallible reservation
of the row vector (`:94-99`), and per-row rejection of a reordered, extended,
or policy-violating retained prefix (`:100-117`). `Self::RetainedParents` is the
opaque retained-parent proof of ConsumerCheckpoint §9 (:264-266) — writers
receive it, never a path string. Existing test providers are
`interface_tests/managed_plan_binding.rs:81` and
`interface_tests/coordinator_contract.rs:513`; production still calls
`CheckedArtifact::prepare_parent` via `entry.rs:144-145`, and that conversion is
R2-E, not R2-D (plan §5 item 1).

### 3.5 Seam 5 — per-phase fault-family activation map

Transcribed from adopted plan §4-end and confirmed as adopted decision §9.4.
This is the frozen map; RemPlan §10 is annotated to point here (§8, D4).

| Family | Keys | Owning package | Status today |
| --- | --- | --- | --- |
| `runtime.*` | 18 | R2-A/R2-B runtime bootstrap and catalog lease | **executed** |
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
| `catalog_bootstrap.*` | 25 | R2-C2 physical first-catalog owner | **executed** |
| `admission.*` | 19 | R2-D **Phase 1** (R2-C3 admission) | **executed** (Step 1.3, 2026-08-22 — 19 sites in `capability/pre_catalog/provider/admission_mutation.rs`, matrix on both target variants, 12-round same-boundary crash stability; counts held at 19/165, no key minted) |
| `durable_leaf.*` | 11 | R2-D **Step 2.1** | reserved |
| `namespace.*` | 11 | R2-D **Steps 2.2/2.3** | reserved |
| `record.*` | 13 | R2-D **Step 2.4** | reserved |
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
> production source list is **ten** files *(nine until the R2-E E2 landing,
> 2026-08-27 — this count is edited in place per the E0.2 amendment pair's
> §6.3 E2.3 duty, the single R2-E count move)* — the three
> `catalog_bootstrap.*` sources above, `admission_mutation.rs` (19),
> `leaf_observation.rs` (11), `namespace_mutation.rs` (11 + the eleven
> `cleanup.*` sites since E1), `protocol/authority_record.rs` (4),
> `authority_record_binding.rs` (9), `managed_mutation.rs` (28), and since
> the E2 landing `barrier_mutation.rs` (16) — pinned tree-complete by the
> round-2 completeness anchor exactly as designed, with the declared-list
> length machine-pinned at ten (`fault_expected_keys.rs`, the `len() == 10`
> assertion landed at E2.3).
| `managed_bootstrap.*` | 30 | R2-D **Phase 3** | **partially executed** (see annotation) |

> `managed_bootstrap.*` activation annotation (2026-08-22, Step 2.3 landing;
> discharges the obligation `GwzM5-8R2DStep22-Review.md` routed to Step 2.3's
> gate, and the Step-2.3 State review's [P2-1]): **8 of the 30 keys are
> executed**, the remaining **22 stay reserved for Phase 3**. The family's owning
> package is still Phase 3; what moved is per *edge*, because RemPlan §10's duty
> — "that family must gain injection sites and matrix rows in the same package
> that converts its edges" — follows the edge, and §4.3 assigns rows **E15** and
> **E16** to Step 2.3. The executed subset is exactly the boundaries those two
> edges and their two restart observations cross: `parent_revalidate`,
> `staging_directory_publish`, `final_directory_reopen`,
> `final_directory_reobserve`, `component_reobserve`, `marker_retire`,
> `marker_retired_reobserve`, `final_identity_reobserve`. All eight have
> injection sites in
> `capability/pre_catalog/provider/managed_mutation.rs` (the `namespace` owner
> holds none, as with `namespace.*`) and executed
> interruption/restart/convergence rows on both target variants in
> `namespace/tests_managed_matrix.rs`, plus 4×2 repeated-boundary rows. The 22
> reserved keys are Phase 3's writer, intent-record and successor halves; they
> are proved siteless **key by key**, not merely family-wide, by the
> `FaultFamilyActivationV1::PartiallyExecuted` arm added for this flip
> (`interface_tests/fault_expected_keys.rs`). Counts held at **165**, no key
> minted; the caller pin moved 9 → 11 with its `CATALOG_PUBLICATION_CALL_COUNTS`
> companion in the same package.
>
> Deferral record (2026-08-22, Step 3.1 landing; discharges the Step-3.1
> review's [P2-1]): Step 3.1's `execute_bound` converts the *edges* of five of
> the 22 reserved keys — `staging_directory_create`, the three
> `ownership_marker_*`, and `staging_directory_flush` — **without** injection
> sites or matrix rows, deliberately: the plan assigns activation to Step 3.2
> by name, both steps sit inside the family's owning package (Phase 3), and
> RemPlan §10's duty binds per package with the phase settle as its gate.
> Step 3.2 therefore owes: injection sites for the five in
> `managed_mutation.rs`, their interruption/restart/convergence and
> repeated-boundary rows, and the `PartiallyExecuted` list edit (8 → 13
> executed). The five stay counted among the 22 reserved until 3.2's flip;
> counts stay at 165 throughout. Until 3.2 lands, this record is the durable
> statement that the edge conversion and the activation are intentionally
> split, not drifted.
>
> *(Superseded in part at the Step 3.1b landing, 2026-08-22, per the
> Step-3.1b review's [P2-1]: Step 3.1b's own flip below moved the family to
> 23 executed / 7 reserved, so the five writer keys now sit among the **7**
> reserved and the list edit Step 3.2 owes is **23 → 28**. The duty itself —
> five sites, their rows, the list edit — is unchanged.)*
>
> `managed_bootstrap.*` activation annotation (2026-08-22, Step 3.1b landing;
> discharges `GwzM5-8R2DStep31-Review.md` [P2-1] and records Step 3.1b's own
> conversion): **23 of the 30 keys are executed**, the remaining **7 stay
> reserved**. Two steps moved edges since the Step-2.3 annotation above.
>
> **Step 3.1 (landed `e72e376`) converted five edges without activating them.**
> `stage_component` and `write_or_rewrite_marker`
> (`capability/pre_catalog/provider/managed_mutation.rs`) cross
> `staging_directory_create`, `ownership_marker_create`, `ownership_marker_write`,
> `ownership_marker_flush` and `staging_directory_flush`. Their **activation
> remains Step 3.2's** by plan assignment, and RemPlan §10's duty is satisfied at
> the *package* level: both the converting and the activating step sit inside
> `managed_bootstrap.*`'s own owning package, Phase 3, whose gate is the phase
> settle — the condition that forced Step 2.3's early flip (a different phase)
> does not hold here. Nothing unactivated is reachable in the interim:
> `mod provider` carries `allow(dead_code)` and plan §5 item 2 forbids production
> catalog activation. Step 3.2 owes these five sites, their matrix rows on both
> target variants, and this list's edit; its Git-directory arm must drive through
> the Step-2.3 door (`retain_managed_parent_at_for_test`) or place its managed
> prefix under that target's own retained root, because a Git-directory catalog
> has no `.gwz` ancestor (review [P3-3]).
>
> **Step 3.1b converts row E17 and activates its fifteen keys same-commit:**
> `initial_intent_scratch_create`, `initial_intent_scratch_write`,
> `initial_intent_scratch_flush`, `initial_intent_publish`,
> `initial_intent_reobserve`, `successor_scratch_create`, `successor_scratch_write`,
> `successor_scratch_flush`, `successor_scratch_reobserve`, `successor_publish`,
> `successor_reobserve`, `prior_generation_retire`, `prior_generation_reobserve`,
> `final_intent_retire`, `final_intent_retired_reobserve` — the managed intent
> record's durable lifecycle. All fifteen have injection sites in
> `capability/pre_catalog/provider/managed_mutation.rs` (the `namespace` owner
> holds none, as with `namespace.*`) and executed interruption/restart/convergence
> rows on both target variants in `bootstrap/managed/tests_intent_matrix.rs`, plus
> 13×2 repeated-boundary rows at twelve rounds each. This closes the
> partial-retirement window review [P1-1] recorded: a row with two or more missing
> components interrupted inside marker retirement now resumes from its resident
> record instead of refusing for ever.
>
> **E17's two §4.4 Class 1 arms resolve to none**, by the same
> conditional-resolution mechanism §4.3's E16 annotation used. Every move of this
> lifecycle is a *regular-file protocol record* travelling between two
> deterministic slots of one retained action directory, through Step 2.2's
> role-validated `publish_bootstrap_generation` / `retire_bootstrap_generation`,
> carrying `PublicationSourceV1::regular_file` and `DestinationRecheckV1::None`
> exactly as E12/E13 do. No directory is published, so no managed source-interior
> arm exists to add; no retirement destination is re-checked, so no destination arm
> is either. `PublicationSourceV1` and `DestinationRecheckV1` are unchanged by this
> step. Consequently §4.4's arm table rows "managed source-interior | E15, E17" and
> "managed destination | E16, E17" are, on this tree, driven by E15 and by neither
> respectively; the rows are left as written, since this annotation is the
> sanctioned mechanism.
>
> **Counts.** 165 total, unchanged; no key minted. Executed 8 → 23, reserved 22 →
> 7. The `capability_permit.rs` caller inventory holds at **13** and
> `CATALOG_PUBLICATION_CALL_COUNTS` is unchanged: this step adds zero
> `publish_verified_no_replace` call sites.
>
> `managed_bootstrap.*` activation annotation (2026-08-22, Step 3.2 landing;
> discharges the deferral record above, as superseded at the Step-3.1b landing):
> **28 of the 30 keys are executed**, the remaining **2 stay reserved**.
>
> **Step 3.2 activates the five staged-component writer keys** whose edges Step
> 3.1 converted on the record: `staging_directory_create`,
> `ownership_marker_create`, `ownership_marker_write`, `ownership_marker_flush`
> and `staging_directory_flush`. All five have injection sites in
> `capability/pre_catalog/provider/managed_mutation.rs` (the `namespace` owner
> still holds none, as with `namespace.*`) — `staging_directory_flush` at both of
> the writer's directory flushes, the staged interior's and the managed parent's,
> because both are the state that key names and minting a second would move the
> frozen census. Executed interruption/restart/convergence rows on both target
> variants are in `bootstrap/managed/tests_writer_matrix.rs`, with one
> repeated-boundary row at twelve rounds and four named single-crossing
> exclusions. This closes the split the deferral record above declared
> intentional; nothing about it drifted.
>
> **The single-crossing half of the partition is now machine-checked**, in this
> matrix and retroactively in Step 3.1b's. `run_single_crossing_probe`
> (`tests_provider.rs`'s shared `matrix` harness) crashes a boundary, re-arms it,
> and requires the next drive to settle without firing it. It caught a real
> misclassification at once: every boundary inside `stage_component` is crossed
> *once per component*, so on a two-component row a crash at the first
> component's crossing leaves the second's ahead and the boundary is neither
> repeatable nor single-crossing. The writer matrix therefore drives a
> **one-component** row (`gwz.conf/markers`) — the shape the criterion is written
> for — while the intent matrix keeps its two-component row, which it needs to
> reach a mid-retirement interruption. Row shape is now a declared property of
> each matrix rather than an accident of its fixture.
>
> **The purpose policy matrix** (plan §4 Step 3.2's other half) is
> `bootstrap/managed/tests_purpose_policy.rs`: all four purposes bootstrapped on
> both target variants through production request constructors only, and
> ConsumerCheckpoint §9 (:253-256) driven on real durable state in both
> directions — `MergeArchive` refused over a missing `.gwz/merge` (retained count
> one, below the purpose's minimum of two) and bootstrapping exactly its own
> `done` component over a resident one, so a missing `.gwz/merge` can only ever
> be `MergeStore`'s to create. A fully present purpose set plans no row. Its
> Git-directory arm takes route (b) — the managed prefix under that target's own
> retained root — because route (a), the Step-2.3 door, stages with `fs::write`
> and can never reach the writer boundaries; the managed parent's home on that
> variant is fixture-placed, with follow-up 3 (the Git-directory catalog's
> workspace-root binding) attached as the owner decision it awaits.
>
> **Counts.** 165 total, unchanged; no key minted. Executed 23 → 28, reserved 7 →
> 2. The two that remain are `preflight` and `plan_complete`: every edge-bearing
> key of the family is now executed, no step has given these two a boundary, and
> the plan's Step-3.2 clause ("every component/generation/marker boundary") does
> not reach them — their disposition is the Phase 3 settle determination the
> Step-3.1b review's [P3-1] docketed, not this step's. The
> `capability_permit.rs` caller inventory holds at **13** and
> `CATALOG_PUBLICATION_CALL_COUNTS` is unchanged: this step adds zero
> `publish_verified_no_replace` call sites.
>
> Phase 3 settle determination (2026-08-23, lane owner, at the Step 3.3
> landing `3a45619`; discharges the Step-3.1b review's [P3-1] docket item):
> **`preflight` and `plan_complete` never gain fault boundaries.** They name
> plan-level states of the request lifecycle — a preflight observation and a
> completed plan — not durable edges: no create, write, flush, publish,
> retire, or reobserve occurs at either, and every edge-bearing key of the
> family (28 of 30) is executed. The states they name are proven by the
> purpose-policy matrix's ProofOnly/Scheduled arms and the coordinator suite
> (`tests_purpose_policy.rs`, `coordinator/tests_execution.rs`), which is the
> correct evidence form for a state, as fault injection is for an edge. They
> remain in the vocabulary — census stays 165/30, no key retired — with the
> key-by-key siteless proof (`fault_expected_keys.rs`) as their permanent
> evidence. Step 5.1's per-key executed-evidence duty accordingly reads:
> edge-bearing keys carry executed interruption rows; these two carry this
> determination and their state proofs.
| `cleanup.*` | 11 | R2-D **Phase 4** (step 4.1 legacy leaf edges) | reserved |
> `cleanup.*` non-activation record (2026-08-23, Step 4.1 landing; records the
> disposition of the plan's "activate `cleanup.*` rows for the cleanup edges this
> converts" clause): **0 of the 11 keys are executed; all 11 stay reserved.**
> Step 4.1 converts §4.3 rows **E18-E21** — the four legacy *rename* edges. The
> `cleanup.*` vocabulary names a different edge set: five keys are the
> `CleanupWorklistV1` record's own scratch/publish/reobserve lifecycle
> (`protocol/cleanup.rs`, `BaseActionSlotV1::CleanupWorklist`), and the remaining
> six are the retirement of the three `CleanupAliasV1` aliases — Source, Goal,
> Authority — into their `Retired*Alias` slots with their source/destination
> reobservations and row/completion records (`namespace/mod.rs`
> `cleanup_retirement`, `retire_exact`). None of those edges exists on this tree
> outside the R1 vocabulary: every one requires an `AdmittedActionV1` and a
> scheduled action directory, so their conversion is a consumer conversion, which
> plan §5 items 1 and 2 place outside R2-D. The legacy twin of that work is
> `cleanup.rs`'s three `remove_file` calls, which are removals, not renames — the
> shape the E4-retire activation record above already establishes no admitted
> family expresses — and Step 4.1's scope names only the four rename sites.
>
> Per the map's own carried clause ("any key whose edge genuinely converts later
> is explicitly re-reserved for R2-E/R2-F in the same update"), all eleven
> `cleanup.*` keys are **re-reserved for R2-E**, the lane that gives the cleanup
> worklist its first production consumer; the row above is left as written, since
> this annotation is the sanctioned mechanism and the family's Phase-4 assignment
> was made against the step, not against the edges. RemPlan §10's duty is not
> deferred by this record — it never attached, because Step 4.1 converts no
> `cleanup.*` edge.
>
> **What Step 4.1 did convert, and where its evidence is.** E18-E21 route through
> one sealed source-associated composition of §4.1's **P1** arms,
> `platform::publish_verified_leaf_no_replace`, which the frozen §4.3 rows already
> specify ("P1 (replaces `platform::rename_relative`)") and which §5 decision 2
> already calls converting E18-E21 "in situ through P1". It is P1's *arms* —
> `open_rename_source` then `rename_open_source` — and not a call into
> `publish_verified_no_replace`, because that function's identity compare is
> `HostPlatform`-bound and `HostPlatform` admits only the closed support table
> (ext4 + `FS_IOC_GETFSUUID`, `ATTR_CMN_OBJPERMANENTIDS` with `MNT_LOCAL`, NTFS
> `FileId128`), while the legacy leaf writer is live on every filesystem carrying
> a persistent file handle; routing these edges through it would narrow
> production merge and stash flows to that table, against the step's own "with
> identical external behavior". The composition therefore carries the legacy
> family's durable identity vocabulary — the one its authority record already
> commits to. Evidence: `tests/leaf_publication.rs` drives each of the four edges
> to the front of a real flow and substitutes its source inside the closed
> window; all four are red against the raw rename (verified by reverting the four
> sites in scratch: 1 passed, 4 failed) and green against the composition. The
> new `BeforeSealedLeafPublication` boundary is a legacy `CheckedArtifactFault`,
> not a `CheckedArtifactFaultKeyV1`, so it mints no key; it is driven on all
> three legacy restart/convergence matrices. `RAW_RENAME_CALL_ALLOWLIST` lost its
> `transition.rs` and `residue.rs` entries and `platform.rs`'s pair counts moved
> 5 → 6, leaving the subsystem's whole raw-rename surface in two sealed files.
>
> **Counts.** 165 total, unchanged; no key minted. `cleanup.*` 0 executed / 11
> reserved. The `capability_permit.rs` caller inventory holds at **13** and
> `CATALOG_PUBLICATION_CALL_COUNTS` is unchanged: this step adds zero
> `publish_verified_no_replace` call sites.
>
> **`cleanup.*` activation record (2026-08-27, R2-E Step E0.2 semantics
> amendment as amended at E0.2b; annotates the Step-4.1 non-activation record
> above, which is left as written since this annotation is the sanctioned
> mechanism).** This record binds semantics; the flip to `Executed` is R2-E
> **Phase E1**'s, in the commit that converts the edges, per RemPlan §10's duty.
> **Counts. 165 total, unchanged; no key minted, none retired; `cleanup.*` stays
> 0/11 until E1.2 lands.**
> *(Filed at the E0 landing, 2026-08-27: the full record — the eleven key
> semantics, DECISIONS C-1/C-2/C-3 and the E1.2 convergence obligation — is
> `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §2 as amended by
> `GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` §4/§6.3/§8 (addendum
> controlling; OPEN-C1 struck there — no interleaving proof is owed); this
> header is the §3.5-entering string per the addendum's §8 [P2-4] cure.)*
| `barrier.*` | 16 | R2-D **Phase 4** (step 4.2 Windows retirement closure) | reserved |
> `barrier.*` non-activation record (2026-08-23, Step 4.2 landing; records the
> disposition of the plan's "activate `barrier.*` … with rows" clause):
> **0 of the 16 keys are executed; all 16 stay reserved.** Step 4.2 converts §4.3
> row **E22**, the legacy Windows durability anchor. The `barrier.*` vocabulary
> names a different protocol: the frozen **action-scoped roaming anchor**, whose
> `BarrierIntentV1` (`protocol/barrier.rs`) binds a catalog anchor identity, a
> private home, a target parent and path profile, a reserved target leaf and an
> ordinal, and whose `issue` refuses without a `NamespaceBarrierAuthority`, an
> `ActionCapacityReservationV1` and an ordinal inside `schedule().barrier_count()`.
> Its five intent keys move a record between `BarrierIntentScratch`,
> `BarrierIntentActive(ordinal)` and `BarrierIntentRetired(ordinal)`; its target
> and alias keys move the anchor into a target parent and retire a stranded alias
> onto `RetiredRoamingAnchorAlias(ordinal)`. None of that exists without an
> `AdmittedActionV1`, so its conversion is a consumer conversion, which plan §5
> items 1 and 2 place outside R2-D — and landing the anchor inside an admitted
> action directory additionally requires teaching admission's `extra_children: 0`
> grammar the reserved target leaf, which is the very contamination class the
> E10/E14 annotation records as diagnosed and fixed.
>
> The legacy anchor is that protocol's **ancestor**, not an instance: source and
> destination are one directory, there is no intent record, no ordinal and no
> target leaf. §4.3's E22 row already says which primitives serve it — "P1 + P5",
> win cell `platform.rs`, "New primitive? no" — and that is what this step used.
> Per the map's carried clause, all sixteen `barrier.*` keys are **re-reserved for
> R2-E**, the lane that gives the roaming anchor its first admitted action.
> RemPlan §10's duty is not deferred by this record — it never attached, because
> Step 4.2 converts no `barrier.*` edge.
>
> **`barrier.*` activation record (2026-08-27, R2-E Step E0.2 as amended at
> E0.2b; annotates the Step-4.2 non-activation record above, left as written).**
> This record binds semantics; the flip to `Executed` is R2-E **Phase E2**'s, in
> the commit that converts the edges. **Counts. 165 total, unchanged; no key
> minted, none retired; `barrier.*` stays 0/16 until E2.3 lands.**
> *(Filed at the E0 landing, 2026-08-27: the full record — the sixteen key
> semantics as rewritten at E0.2b §1.5, the O6 RESOLUTION completed with the
> read-side identity refusal (E0.2b §5), DECISIONS B-1 through B-6, and the
> E2.3 convergence obligation — is `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §3
> as amended by `GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` §1/§5/§6.3
> (addendum controlling), which also records the OPPOSITE of a predicate
> authorization: E2.1 must land the alias lifecycle with `completed_record`,
> `retain_file` and `require_named_file_identity` **unchanged** (DECISION B-5,
> fresh-copy alias).)*
>
> **Named-exception annotation (2026-08-29, E7.2 — discharging dual #2's
> condition C2; [R2-P3-2]'s disposition, both peer-blind axes convergent).**
> One durable edge on the barrier drive path is announced by no `barrier.*`
> key, deliberately: **primitive family P5's roaming recovery return rename**
> — the converge of a stranded outbound alias (`prepare_roaming_target` row
> three), which runs *before* the family's drive proper and, on the
> `Stranded` path, on a drive where `private_barrier` is never called. It is
> one atomic no-replace rename, idempotent and self-converging (a crash
> before it re-presents row three; a crash after it presents row two), and
> the seeded-state row already asserts census-exactly everything an
> interruption row could assert, on both target variants. A seventeenth key
> would move the frozen sixteen-key record and the 165 census to buy zero
> new distinguishable crash-window evidence — the 17th key is refused on
> substance. "Every durable barrier edge is announced" is true of the
> family's own boundaries and carries this named exception for P5-recovery
> convergence; the `barrier_mutation.rs` module header's universal reads as
> "all `barrier.*` announcements live in this file" (Code axis F1/Q2; State
> axis queue item 3; the optional in-code qualifying clause rides the C1
> carrier train, declined at E7.2 to avoid a comment-only protected-tree
> digest move).
| `terminal.*` | 11 | R2-D **Phase 4** (step 4.2, terminal retirement edges) | reserved |
> `terminal.*` non-activation record (2026-08-23, Step 4.2 landing; resolves the
> plan's own conditional, "and `terminal.*` if its edges convert here"): **0 of
> the 11 keys are executed; all 11 stay reserved, re-reserved for R2-E.** The
> conditional resolves to *no*. Every key of the family names the admitted action
> directory's terminal retirement into the catalog's retired root — §4.4's "E7's
> Phase-4 half and the terminal retirement edges" — and each needs an
> `AdmittedActionV1` and a production catalog, which plan §5 item 2 forbids in
> R2-D. No edge of the legacy anchor touches one.
>
> **What Step 4.2 did convert, and where its evidence is.** E22's machinery is
> retired in place on P1 + P5. The random `.ca1-anchor-scratch-<16 bytes>` — the
> R2 stop clause's nonce, whose orphans the legacy survey could not even
> classify, one per crash and unreclaimable — is replaced by the single
> deterministic `.ca1-anchor-scratch-v1`, resumed onto and rewritten in place. The
> correctness-critical `remove_file` on the stranded outbound alias is replaced by
> a durable retirement onto ordinal-indexed retired names through the sealed P1
> composition. All four surviving anchor renames publish through that same
> composition, so `platform/anchor.rs` names no raw rename and the subsystem's
> whole `rename_relative` surface is now **one** reference —
> `rename_open_source`'s own non-Windows delegation
> (`RAW_RENAME_CALL_ALLOWLIST`, `platform.rs` 5 → 1).
>
> **Amendment (2026-08-23, round-2 remediation of the Step-4.2 review's [P2-2]).**
> The retirement destination is **ordinal-indexed, not singular**. Round 1
> retired the stranded alias onto one fixed `.ca1-anchor-retired-v1` and refused
> a second retirement while it stood; that refusal was permanent on a *reachable*
> state — two foreign strandings, each interrupted at one window — and it bricked
> P5 for all seven `AnchoredPrivateArea` callers with no in-code exit. E16's
> standard admits a permanent typed refusal only where the wedge state is
> unreachable or off the supported table, and this one was neither, so it is
> fixed mechanically rather than derived away. `RETIRED_PREFIX` +
> `smallest_free_ordinal` now read the ordinal off the ordinals actually resident
> and publish onto the smallest free one; smallest-free rather than count or
> max+1, because count has a fixed point on a gap and would have wedged inside
> the fix itself. Retired objects are bounded by **actual crash occurrences** —
> one per foreign stranding a crash interrupted — and deliberately uncapped,
> since a cap would restore a wedge at the cap. Every reachable post-crash state
> has a typed exit, and the recurrence windows are executed rather than asserted
> (`a_recurring_stranding_retires_onto_the_next_free_ordinal_and_converges`,
> `the_retirement_ordinal_is_the_smallest_free_one_observed`).
>
> **[P2-1] closure.** `BOUNDARIES` carries all **ten** boundaries the protocol
> announces, and the matrix builds the stranded-alias state before driving the
> two retirement ones. Round 1 announced and injected them and drove neither.
> They execute natively on Windows (probe `32612243125`, 82/0), where
> `hard_link_identity_sharing_is_what_the_retirement_rows_assume` proves the rows
> took the retirement branch rather than the platform skip.
>
> **A platform fact, recorded because a reader would otherwise mis-read the
> evidence.** `NeedsRetireAlias` — the state the retired removal reconciled —
> requires one object under two names with one durable identity. On macOS
> `ATTR_CMN_OBJPERMANENTID` is allocated per hard link and creating the second
> link re-homes the first, so **the state, and the legacy removal, have always
> been unreachable there**; it is reachable on Linux and on Windows, the platform
> the anchor serves. The rows assert the retirement where the state is producible
> and the refusal where it is not, and the pinning row measures the platform
> rather than reading a `cfg`, so the coverage cannot silently rot.
>
> **Legacy nonce orphans are tolerated, never reclaimed.** The closed grammar is
> forward-looking. A `.ca1-anchor-scratch-<32hex>` left by a pre-4.2 Windows
> crash matches none of the closed names; `survey` ignores it, so it refuses
> nothing and blocks nothing, but no drive reclaims it either. The count is
> bounded by past crashes and cannot grow, because nothing mints that shape any
> more. Reclamation is an optional R2-E item, not a debt of this step;
> `legacy_nonce_orphans_are_tolerated_and_block_nothing` pins the upgraded tree.
>
> **The same nonce, one file away, closed in the same package.** Until this
> remediation the private area was not nonce-free: `authority::scratch_name`
> minted `.ca1-scratch-<kind>-<16 random bytes>` per attempt on E20
> (`ensure_goal`) and E21 (`publish_scratch`) — the very edges Step 4.1 put on a
> successful converted path, which is plan §4 Step 4.2's own trigger condition.
> It is now `.ca1-{family}-{action}-{kind}.scratch`, derived from the same
> observed durable state as the published names beside it, with the same resume
> discipline the anchor uses (`create_new` fresh, truncate on resume). It is
> **action-scoped**, so determinism needs no serialization argument: two drives
> collide only on the same family and action, a case that already ended in a
> typed refusal. It stays dotted, so `inspect_family`'s `ca1-{family}-` grammar
> is unchanged and no older gwz classifies it as foreign. Legacy random orphans
> get the disposition above exactly: tolerated, invisible, never reclaimed,
> bounded by past crashes.
>
> **§4.4's arm table row "any further retirement-destination arm | Phase 4"
> resolves to NO ARM for this step**, by the same conditional-resolution
> mechanism E16's and E17's annotations used. The anchor's retirement
> destination is a *regular file* published through P1 with the legacy family's
> own identity vocabulary; it re-checks no destination interior, so there is no
> `DestinationRecheckV1` arm to add and `DestinationRecheckV1` is unchanged by
> Step 4.2. On this tree that row is therefore driven by E7's Phase-4 half and
> the terminal retirement edges alone — both `terminal.*`'s, re-reserved for
> R2-E above. The row is left as written, since this annotation is the
> sanctioned mechanism.
>
> **Two behaviour deltas, declared.** (a) **Truncate at a predictable staging
> name.** Both the anchor's `.ca1-anchor-scratch-v1` and the family's
> `.ca1-{family}-{action}-{kind}.scratch` are now derivable, so a *foreign*
> object planted there is truncated and reused where the legacy random name
> would never have touched it. It is inside the amendment's cooperating-same-user
> boundary (the actor needs write access to the private area, which already lets
> it delete or rewrite anything there), and it cannot promote foreign content:
> the resume writes our own bytes, takes the identity from its own write handle,
> and the sealed publication re-verifies identity and bytes through the handle
> it renames before anything durable moves. What it removes is the pre-4.2
> alternative — allocating beside the foreign object and leaking an orphan for
> ever. (b) **The `NeedsReturn` pre-verify.** The return arm gains a `verify`
> the legacy arm did not have. It can only reject, and it converts
> rename-then-fail into refuse-without-mutation. Both are improvements; both are
> deltas, not nulls.
>
> **The P5 row and the E10/E14 annotation stay true, with two amendments of
> record.** (1) §4.1's P5 row cited the anchor internals in `platform.rs`; they
> now live in `platform/anchor.rs`, and `private_barrier`,
> `DirentBarrierClass` and the `ExactInterior` documented no-op — everything the
> E10/E14 annotation and `authority_record_binding.rs` cite by name — stay in
> `platform.rs` unchanged. (2) The annotation's subject is which arm a caller
> takes on Windows; that is untouched, and `ExactInterior` still takes the
> no-op. Both amendments are relocations, not substantive edits: no caller
> changed class, and the seven `AnchoredPrivateArea` call sites are
> byte-identical.
>
> **Counts.** 165 total, unchanged; no key minted. `barrier.*` 0/16,
> `terminal.*` 0/11. The `capability_permit.rs` caller inventory holds at **13**
> and `CATALOG_PUBLICATION_CALL_COUNTS` is unchanged: this step adds zero
> `publish_verified_no_replace` call sites.
>
> *(Step-4.1 [P3-4], discharged in this package.)* `BeforeSealedLeafPublication`
> is split into four per-edge variants — `BeforeAuthorityPublication`,
> `BeforeGoalPublication`, `BeforeDetachPublication`,
> `BeforeManagedPublication`. The three legacy restart matrices carry 9 rows in
> place of 3, each flow driving exactly the sealed leaf edges it crosses.
>
> **`terminal.*` activation record (2026-08-27, R2-E Step E0.2 as amended at
> E0.2b; annotates the Step-4.2 non-activation record above and its §4.4
> arm-table resolution, both left as written).** Proposed outcome: **10 of 11
> executed, 1 reserved with a determination** — the family lands as
> `FaultFamilyActivationV1::PartiallyExecuted`
> (`interface_tests/fault_expected_keys.rs:209`), whose per-key siteless proof
> keeps the `Reserved` arm's guarantee for the eleventh. **Counts. 165 total,
> unchanged; no key minted, none retired; `terminal.*` stays 0/11 until E3.2
> lands.**
> *(Filed at the E0 landing, 2026-08-27: the full record — the ten activated
> key semantics, the `terminal.authority_release` determination (T-D as
> re-grounded at E0.2b §8 on reading (a) alone, its demotion of reading (b)
> closed by the non-activation record's own scope sentence above), DECISIONS
> T-A / T-B′ / T-C′, the OPEN-T1 widening authorization at three gates, and
> the E3.2 convergence obligation — is
> `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §4 as amended by
> `GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` §2/§3/§6.3/§8 (addendum
> controlling).)*
> *(E3 landing determination, 2026-08-27, per the E3 review round 2,
> condition (b): the amendment's §4.3 row #2 semantic is REPLACED —
> `terminal.payload_reobserve` names: each payload row
> (`SourcePayload`/`GoalPayload`) is resident in exactly ONE of its two
> scheduled homes and is a canonical regular file, READ NOT AT ALL; a
> payload's length is never a protocol-record bound (ConsumerCheckpoint §8
> :236-237), and the derived-over digest relation the drafted row named is
> the authority record's, requiring a `CheckedAuthorityObservationV1` no
> terminal retirement holds. Row #3 lands STRENGTHENED over its remediated
> code: the worklist re-read bounded, bound to the resident reservation,
> and every row classifying `Complete` over facts observed through the
> production identity provider.)*

Total 165 keys, equal to `EXPECTED_KEY_COUNT`
(`interface_tests/fault_expected_keys.rs:174`; the key list ends `:172`).

> R2-E E0 annotation (2026-08-27, per the E0.2 amendment pair, addendum
> controlling; the statement above is left as written): unchanged by the R2-E
> semantics amendment, and restated in each of the three activation records
> above — 165 total, no key minted, no key retired; the three families stay
> `Reserved` (0/11, 0/16, 0/11) until E1.2 / E2.3 / E3.2 flip them in their
> converting commits.

Two clauses the map carries forward verbatim from the plan:

- "any key whose edge genuinely converts later is explicitly re-reserved for
  R2-E/R2-F in the same update" (plan §4-end);
- RemPlan §10's duty (:1033-1035): when a family's edges convert, "that family
  must gain injection sites and matrix rows in the same package that converts
  its edges".

`runtime.*` is recorded as executed on a different mechanism from the rest: its
edges are driven by the separate `RuntimeBootstrapFault` enum
(`bootstrap/runtime/fault.rs:4-11`; its injection entry point `run_next_at` is
at `:19`) exercised from `bootstrap/runtime/tests.rs:21/:37/:73`
and `bootstrap/runtime/catalog_lease/tests.rs:163/:189/:212` plus
`catalog_lease/tests/association.rs:198/:272`, not by
`CheckedArtifactFaultKeyV1::Runtime*` references. The Step 0.2 fixture encodes
that exception explicitly rather than papering over it
(`interface_tests/fault_expected_keys.rs:348-355`).

**Injection-site inventory (corrected in round 2 — Code P1-2, State P2-1).**
There are **three** production sources holding `CheckedArtifactFaultKeyV1`
injection sites on this tree, all of them `catalog_bootstrap.*`:

| Production source | `fault_v1::hit` sites |
| --- | --- |
| `capability/pre_catalog/provider/directory_mutation.rs` | 15 |
| `capability/pre_catalog/provider/mutation.rs` | 9 |
| `capability/pre_catalog/provider/aggregate.rs` (`:26-29`, `CatalogBootstrapCatalogEnumerate`) | 1 |

25 sites, matching the family's 25 keys. Round 1 of this memo named only the
first two, and the Step 0.2 fixture declared only those two in
`FAULT_INJECTION_SOURCES` — so the *design* claim (every reserved family has
zero sites anywhere) was true, but the recorded inventory was false and the
reserved-family scan was porous: a reserved-family site added in `aggregate.rs`
or any other undeclared production file would not have tripped it. Both are now
fixed in the fixture: the third source is declared (`:257`), and a completeness
**anchor** (`:418`) rescans the production tree and fails if any production
source naming `CheckedArtifactFaultKeyV1` is missing from the declared list
(§8, D3). Because `CheckedArtifactFaultKeyV1` is `pub(super)` on
`checked_artifact::fault_v1` (`fault_v1.rs:10`, declared `mod fault_v1;` at
`checked_artifact/mod.rs:51`), no injection site can exist outside
`src/checked_artifact/`, so the anchor's scan of that subtree is exhaustive and
the "machine-checked" property claimed in the RemPlan §10 annotation is
delivered, not merely asserted. Every reserved family still has zero sites, by
design, which the fixture asserts (§8, D3).

> R2-E E0 annotation (2026-08-27, per the E0.2 amendment pair as corrected at
> E0.2b §6.1; this inventory and the round-2 inventory addendum at the
> `record.*` row above are left as written): the declared-and-anchored
> production source list moves **nine → ten**, the single move landing at
> **E2** with `barrier_mutation.rs`; E1 and E3 add sites to already-declared
> files (`namespace_mutation.rs`, `admission_mutation.rs`) and move no count.
> The inventory addendum's "nine" is edited to **ten** in E2.3's own converting
> commit, not before. The completeness anchor
> (`fault_expected_keys.rs:414-418`) proves the list exhaustive at each
> landing.

## 4. Track P — physical primitive inventory, per platform

### 4.1 The already-proven primitive families

| # | Primitive family | Definition | macOS / Linux arm | Windows arm |
| --- | --- | --- | --- | --- |
| P1 | Sealed source-associated publication (`publish_verified_no_replace`) | `capability/pre_catalog/provider/publication.rs:82-183` | `platform::open_rename_source` `platform.rs:23-46` (`cfg(not(windows))`, no-follow, `maybe_dir`) then `platform::rename_open_source` `platform.rs:78-96` — retains the identity-checked handle across a relative no-replace rename | `platform::open_rename_source` `platform.rs:48-76`, `platform::rename_open_source` `platform.rs:98-157` — renames the **exact identity-checked handle** via `SetFileInformationByHandle`; absolute destination derived from the retained destination handle at `platform.rs:196-259` |
| P2 | Write-through open + handle flush + parent flush | `durable_write_options` `mutation.rs:377-394` and `directory_mutation.rs:703-727`; `file.sync_all()` `mutation.rs:163/:236`, `directory_mutation.rs:561`; the catalog root-flush callers `mutation.rs:396/:412/:426` and `directory_mutation.rs:729` call `platform::sync_parent` and so belong to **this** family, not to P5 (Code P3-6) | `platform::sync_parent` `platform.rs:367-370` (`cfg(not(windows))`, directory fsync) | `platform::sync_parent` `platform.rs:372-377` is a documented no-op; Windows durability is the write-through open plus the anchor barrier in P5 |
| P3 | No-follow open + durable identity compare | `platform::open_dir_share_delete` `platform.rs:159-163` / `:165-194`; `HostPlatform` identity at `capability/pre_catalog/provider/platform.rs:23-33`; encoding at `provider/retained.rs:417-427` | per-OS children `provider/platform/linux.rs`, `provider/platform/macos.rs` selected by the `#[path]` edges at `provider/platform.rs:8-13` (macOS: `getattrlist` persistent object/volume identity, `fpathconf(_PC_CASE_SENSITIVE)`) | `provider/platform/windows.rs` via `provider/platform.rs:14-16`; `open_dir_share_delete` `platform.rs:165-194` adds the `FILE_SHARE_DELETE` recipe so the directory open does not collide with the retained rename-source handle |
| P4 | Bounded interior / global enumeration | `provider/interior.rs:50-91` with `MAX_INTERIOR_ENTRIES = 10` (`interior.rs:27`); global catalog grammar budgets `catalog/enumeration.rs:8-12` (4 096 parent entries, 255 native name units, 510 encoded bytes) | portable; identical code path | portable; identical code path |
| P5 | Dirent barrier | `platform::private_barrier`; production callers today are the legacy `cleanup.rs:89/:129/:159`, `residue.rs:407/:500/:529`, `transition.rs:427`, plus the Windows anchor internals, owned since Step 4.2 by `platform/anchor.rs` | `platform.rs:271-274` (`cfg(not(windows))`, directory handle fsync) | two arms, selected by the caller's `DirentBarrierClass` (`platform.rs:364`): the durability-anchor round trip, owned since Step 4.2 by `platform/anchor.rs` (`round_trip` `:183`, `ANCHOR_BYTES` `:74`, `ANCHOR_PREFIX` `:76`, `prepare` `:129`, `survey` `:284`, `roundtrip_name` `:388`, `verify` `:394`; retirement `RETIRED_PREFIX` `:100`), for `AnchoredPrivateArea` callers, and the documented no-op `platform.rs:526` for `ExactInterior` callers (see the E10/E14 activation annotation) |

> R2-E E0 annotation (2026-08-27, per the E0.2 amendment pair; the row above is
> left as written): the Windows column becomes **three** arms at E2.1 — the
> resident-anchor round trip (`AnchoredPrivateArea`), the documented no-op
> (`ExactInterior`), and the supplied-anchor round trip
> (`RoamingAnchoredTarget`, DECISION B-3), whose Windows arm round-trips the
> **supplied** alias by its reserved leaf rather than surveying for a resident
> anchor; off Windows all three remain the same directory `fsync`
> (`platform.rs:377-384`).

### 4.2 Spike executed for this freeze

`GwzFasterProposal.md` §3 Step C.1 requires the admission publish/retire path to
be spiked against the sealed family before the freeze is reviewed. Executed as
two `#[cfg(test)]` cases in
`gwz-core/src/checked_artifact/capability/pre_catalog/provider/tests_admission_spike.rs`:

- `admission_publish_and_retire_edges_use_the_sealed_publication_primitive`
  (`:72-152`) — deterministic indexed staging name, write + flush, no-replace
  publication into a catalog-interior destination through
  `publish_verified_no_replace`, reobservation through the same retained parent
  (staging name consumed, identical object identity, identical bytes),
  a no-replace collision proof that leaves the published object untouched, then
  a cross-parent retirement edge with the same reobservation.
- `admission_publish_rejects_a_substituted_source_before_the_namespace_edge`
  (`:157-191`) — same name, different object: the primitive's identity compare
  rejects before the edge and performs zero namespace mutation (amendment §4.1
  drift-rejection paragraph).

Both are runnable standalone by name. **Step C.1 requires both platforms, and
both have now executed green:**

| Arm | Where | Result |
| --- | --- | --- |
| macOS | this lane's host, at `d32b2c9` | `ok. 2 passed; 0 failed` (§10.1, re-run green in §10.2) |
| **Windows** | GitHub Actions run **32542469665**, workflow "Windows matrix", branch `probe/track-p-spike`, commit `e448d7f` (= `d32b2c9` plus a workflow trim only — no `src/` change), 2026-08-22 | `test result: ok. 2 passed; 0 failed` (0.04 s, 1332 filtered) for `checked_artifact::capability::pre_catalog::provider::tests_admission_spike::{admission_publish_and_retire_edges_use_the_sealed_publication_primitive, admission_publish_rejects_a_substituted_source_before_the_namespace_edge}` |

Round 1 of this memo recorded the Windows arm as *not executed* and offered the
run-13 matrix as substitute evidence, citing amendment `:667` ("Linux and
Windows native **fault evidence** remains an R2-F release gate"). Both reviews
were right to reject that: `:667` defers fault-injection evidence, not Track-P
spike execution, and the operator's instruction says "apply it to this freeze".
The spike is now executed on Windows against the parked object's source, so the
condition is met on its own terms — **no substitution, no waiver, and the
run-13 argument is withdrawn as the discharging evidence** (it remains true and
is retained below only as corroboration of the same edge shape).

**Tracked acceptance item:** *both* `tests_admission_spike` cases must come back
green on the **next full Windows matrix run** (the run above is a
branch-scoped workflow-trimmed run, so the full matrix is still owed). This item
travels with the package to acceptance; the lane owner records it in
`CurrentProgramCheckpoint.md` with the tier statement of §9.

*(Discharged 2026-08-23 at the 5.2 settled dual's State-axis [P3-1]: runs 18-21 — 32573548540, 32607411300, 32613728318, 32616954369 — are fully-green full-matrix Windows runs, 1472-1528 passed / 0 failed, and both spike cases are ordinary lib tests inside each. Recorded here so the owed item is neither silently satisfied nor open.)*

Corroborating Windows evidence that P1 carries the *same* physical edge shape
today: the accepted run-13 matrix (1322/0/1, GREEN at `90d3f8a`,
`CurrentProgramCheckpoint.md` ledger) over the six sealed publication moves,
which already include a directory staging→final publish and an active→retired
cross-parent retirement through `rename_open_source`'s
`SetFileInformationByHandle` arm.

**Seam discipline of the spike.** The boundary checker pins the production
caller set of `publish_verified_no_replace` to exactly `mutation.rs` (1 call)
and `directory_mutation.rs` (5 calls)
(`scripts/checks/check_checked_artifact_boundaries.py:816-856`;
`CATALOG_PUBLICATION_CALL_COUNTS` at **`:657-660`** — round 1 said `:648-651`,
which is inside an unrelated list (State P3-1); mirrored by the six-site pin at
`interface_tests/capability_permit.rs:110`). A spike naming the primitive from
any ordinary file under `provider/` would join `publication_callers` and fail
that rule. The spike therefore lives in a `tests`-prefixed file, which
`production_rust_files` (`:663-670`) excludes, so the production caller
inventory is unchanged and the checker's publication-seam rule still passes
(§10.1, §10.2). Step 0.2 pins that arrangement
(`interface_tests/r2d_seam_freeze.rs:218-240`).

**One stale citation inside the spike file, owed to its owner (Code P3-4).**
`tests_admission_spike.rs:84` cites the R2 stop clause as
"`RemPlan-4 §4 R2 stop clause :1082-1085`". The clause's content is quoted
accurately (it does forbid allocating a fresh retry scratch name, which is why
the spike uses deterministic indexed names), but in
`GwzM5-8R4bP1P2-RemPlan-4.md` it now sits at **`:1089-1092`**; the citation was
inherited verbatim from `GwzFasterProposal.md:160` and was already stale when
the package was parked. The one-line comment edit LANDED with the round-2
commit `2ed2435` (`tests_admission_spike.rs:84` now cites `:1089-1092`), and
the pre_catalog tree pin it disturbed was refreshed at `c40e712` per the Code
round-2 [P1-3] finding. (Sentence updated at acceptance per State round-2
[P3-5].)

### 4.3 Every R2-D physical edge mapped onto a proven primitive

Platform columns record which arm serves the edge: **mac** = macOS,
**win** = Windows, **lin** = Linux. All five families are portable across the
three; the table records where the arms differ in code rather than repeating
the same cell.

The last column also carries each edge's **recheck-arm dependency** where it has
one, so the table alone states the §4.4 obligation: rows **E3, E7, E15, E16,
E17** publish or retire a *directory* through P1 and therefore need an arm of
the §4.4 recheck class before they can be implemented. "New primitive?" stays
**no** for all of them — an arm is a protocol-typed extension of an admitted
primitive, not a new one.

| # | R2-D physical edge | Phase | Primitives | mac | win | lin | New primitive? |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E1 | admission staging action directory create | 1 | P2 + P3 | `platform.rs:367` fsync | write-through open; `sync_parent` no-op `platform.rs:372` | as mac | no |
| E2 | resident reservation write + flush | 1 | P2 | `mutation.rs:377`, `sync_all` `:163` | same, `directory_mutation.rs:703` options | as mac | no |
| E3 | admission staging → final action publish, no-replace, catalog interior | 1 | P1 | `platform.rs:23/:78` | `platform.rs:48/:98` exact-handle | as mac | no — spiked §4.2 (mac+win); **needs the admission source-interior arm, §4.4 class, Phase 1 (C-2)** |
| E4 | admission record `Idle`/`Preparing` scratch → publish | 1 | P2 + P1 | as E2/E3 | as E2/E3 | as E2/E3 | no |
| E5 | post-publish reobservation of identity and exact record | 1 | P3 + P4 | `provider/platform/macos.rs` | `provider/platform/windows.rs` | `provider/platform/linux.rs` | no |
| E6 | bounded global classification of interior action rows | 1 | P4 | `interior.rs:50`, `enumeration.rs:8-12` | identical | identical | no |
| E7 | action directory retirement into the retired root | 1/4 | P1 with destination recheck | `publication.rs:157-173` | same + `windows_destination_path` `platform.rs:196` | as mac | no — spiked §4.2 (mac+win); **needs the admission destination arm, §4.4 class, Phase 1 (C-2)** |
> R2-E E0 annotation (2026-08-27, per the E0.2 amendment pair; the row is left
> as written): its Phase-4 half is R2-E's — the eleven `terminal.*` keys bind
> to this edge (amendment §4: ten activated + one determined), the destination
> child name is the already-derived `RootEntryNameV1::ActiveAction` name under
> the retired root (DECISION T-A), and the edge is blocked on OPEN-T1 until
> `completed_record`'s empty-retired-root predicate is widened — a widening
> **AUTHORIZED at E0.2b §2** at three gates (`interior.rs:349`;
> `completed.rs:310`; `completed.rs:383`), owned by E3.1 as its own reviewed
> precondition step, with `staging_plan`'s bootstrap-refusal arm preserved by
> name.
| E8 | bounded/streamed leaf read through one retained handle | 2.1 | P3 + P2 | `platform.rs:159` no-follow | `platform.rs:165` share-delete | as mac | no |
| E9 | leaf flush | 2.1 | P2 | `sync_all` | documented no-op (see annotation) | `sync_all` | no |

> E9 activation annotation (2026-08-22, Step 2.1 landing; discharges the
> Step-2.1 State escalation [P2-1] and Code round-1 settle duty): the
> Windows arm of `flush_observed_leaf` performs no handle flush —
> `FlushFileBuffers` requires `GENERIC_WRITE` and the observation handle
> is read-only by design; widening the open would make read-only
> artifacts unobservable. The durability property substituting for it is
> writer-class-conditional and consumer-facing: every gwz writer reaches
> observed leaves through write-through or write-handle `sync_all`
> (verified across `mutation.rs`/`directory_mutation.rs`/
> `admission_mutation.rs`/`residue.rs`), so for gwz-written leaves
> `ExactDurable` means the same on every platform; for FOREIGN-written
> leaves `ExactDurable` is strictly weaker on Windows (namespace ordering
> via the E10/P5 anchor round-trip only, no byte-flush claim). Step 2.4's
> production caller binding must carry this condition. Same annotation,
> negative space of E11's proof: `MissingDurable` is a two-sided absence
> proof; it does not assert continuous absence across the barrier window.
| E10 | leaf namespace barrier | 2.1 | P5 | `platform.rs:271` | `platform.rs:526` documented no-op (see the E10/E14 activation annotation) | as mac | no |
| E11 | same-parent reobserve; two-sided durable-absence proof | 2.1 | P3 + P4 | identical | identical | identical | no |
| E12 | backend `publish_exact` over scheduled roles | 2.2 | P1 | `platform.rs:23/:78` | `platform.rs:48/:98` | as mac | no |
| E13 | backend `retire_exact` over scheduled roles | 2.2 | P1 | as E12 | as E12 | as E12 | no |
> R2-E E0 annotation (2026-08-27, E12/E13 jointly, per the E0.2 amendment pair;
> the rows are left as written): the shared `execute_edge` helper
> (`namespace_mutation.rs:250-305`) gains cleanup, barrier and terminal
> callers; the renames keep announcing the four `namespace.*` keys and **no
> `namespace.*` key is re-keyed or silenced**, per the Step-3.1b routing
> precedent (`managed_mutation.rs:1018-1027`) — DECISION C-1.
| E14 | backend `barrier` | 2.2 | P5 | `platform.rs:271` | `platform.rs:526` documented no-op (see the E10/E14 activation annotation) | as mac | no |
> R2-E E0 annotation (2026-08-27, per the E0.2 amendment pair; the row is left
> as written): `barrier.target_barrier` is a **distinct P5 caller** at a
> *target* parent with a **supplied** roaming anchor, taking a third
> `DirentBarrierClass` (DECISION B-3); the E10/E14 annotation's "E10 and E14
> both pass `ExactInterior` and can never take the other arm" is untouched,
> because both of its hardcoded sites (`namespace_mutation.rs:328`;
> `namespace/host.rs:741-756`) are in a different file from this family's.

> **E10/E14 activation annotation** (2026-08-22, the Windows anchor-readiness
> repair over the Step 2.2 and Step 2.3 landings, gwz-core `6b8b76e`;
> discharges the `GwzWindowsMatrix-Classification.md` run-16 class — all four
> `checked_artifact::namespace::tests_fault_matrix` tests plus the managed
> matrix that entry pre-attributes to it — and the Code review's [P2-1],
> [P2-2] and [P2-4]): **P5's Windows column is two arms, and which one an
> edge takes is a property of the caller, not of the directory.**
>
> The round trip renames a resident `.ca1-durability-anchor-<32hex>` file, so
> it presupposes a directory that is *permitted to retain one*. Only the
> checked-artifact private area is: the anchor is permanent by design and
> `finish()` never removes it. An action directory is not, and the refusal is
> structural rather than stylistic — admission requires `extra_children: 0`
> (`protocol/admission/owner.rs:29-38`), so a resident anchor would make the
> action permanently inadmissible, and the catalog root (`.gwz` / `gwz`) is
> outside the private path the preservation-image model excludes, so it would
> also re-enter exact captures. The barrier is therefore given its writer
> class explicitly (`platform::DirentBarrierClass`, `platform.rs:364`):
> `AnchoredPrivateArea` keeps the round trip unchanged, and `ExactInterior`
> takes a documented no-op (`platform.rs:526`). E10 and E14 both pass
> `ExactInterior` and can never take the other arm — `host.rs:667-681` pins
> the barrier target to the retained action directory and
> `namespace_mutation.rs:328` hardcodes the class — which is why both win
> cells now name the no-op. Off Windows the class selects nothing: both arms
> are the same directory `fsync` (`platform.rs:377`), so no non-Windows
> behaviour of any P5 caller moves.
>
> The substituting property is the P2 family's own, and it is
> writer-class-conditional in exactly E9's sense: every row of an exact
> interior is gwz-written through `durable_write_options`
> (`FILE_FLAG_WRITE_THROUGH`) and moved by the sealed exact-handle rename,
> and no portable directory flush exists on Windows to add to that — the
> same ground `sync_parent`'s Windows no-op and `sync_directory_edge`'s
> already stand on. Carriage: the condition is carried by
> `authority_record_binding.rs` — the file E9's annotation already
> designates as the carrier — at `:34-45` (the doc contract), `:321`
> (`FOREIGN_EXACT_DURABLE_IS_WEAKER`) and `:340`
> (`require_authority_strength`), and by this package's own arm comment at
> `platform.rs:526`. No new carrier is created and no other consumer is
> asked to carry anything.
>
> **This annotation supersedes one clause of E9's.** E9 records that for
> FOREIGN-written leaves `ExactDurable` is "strictly weaker on Windows
> (namespace ordering via the E10/P5 anchor round-trip only, no byte-flush
> claim)". That residual no longer exists: E10 has no anchor path, so on
> Windows a foreign-written leaf's `ExactDurable` carries **no byte-flush
> claim and no namespace-ordering claim — the residual is empty, not
> weaker**. Nothing rests on it: the class was always refused rather than
> accepted at reduced strength (`FOREIGN_AUTHORITY_REFUSAL`, whose text moves
> with this annotation to "a foreign-written leaf carries no durable proof on
> this platform"), and an exact interior admits no foreign row by
> construction. E9's own text above is left verbatim; this annotation is the
> governing statement of that clause.
>
> **Negative space, stated directly rather than by reference** (E9's residual
> clause is the record this arm falsifies, so it cannot serve as the
> citation). Three things this arm does *not* prove, in the form of the
> E4-retire activation record:
>
> 1. **The `DurableNamespace` witness is still issued where no physical edge
>    occurred.** `host.rs:681` returns `Ok(self.issuer().durable())`
>    unconditionally, so on Windows the witness is minted after a no-op. It
>    proves what the schedule proves — that a barrier the admitted action
>    reserved was reached at its ordinal, with the retained parent
>    revalidated — and on Windows it proves *nothing physical beyond that*.
>    It is not a dirent-flush receipt there. Nothing dereferences it for
>    durability today; the one consumer class that could is refused before it
>    reaches the witness (`require_authority_strength`). Any future consumer
>    that wants a physical claim from it must state the platform condition
>    itself rather than infer it from the type's name.
> 2. **The `namespace.parent_barrier` matrix row is a process stop across a
>    no-op edge on Windows.** The key (`fault_v1.rs:119`) is live and its row
>    executes on every platform — its `hit` is at `namespace_mutation.rs:334`,
>    outside `private_barrier`, so the 165-key census and the 11-row matrix
>    are untouched (`tests_fault_matrix.rs:85`, `:464`). But the matrix's
>    claim that every row is "a real process stop across a real durable edge"
>    does not hold for this row there. The convergence it proves on Windows is
>    the restart's re-derivation, not the edge's durability. Recorded as an
>    accepted deviation rather than left implicit; it mints no key and changes
>    no count.
> 3. **`MissingDurable` is unchanged and still two-sided.** E9's other
>    negative space survives this annotation intact: it does not assert
>    continuous absence across the barrier window, and an arm that performs no
>    physical edge does not widen that window's claim.
>
> **Discharged at this landing by the lane owner:** the two win cells and the
> §4.1 P5 cell (edited with this annotation); the two protected-source pins
> (`platform.rs` flat and the `pre_catalog.rs` tree, recomputed at gwz-core
> `6b8b76e`); the tracked acceptance item (recorded in the classification
> ledger): the next full Windows matrix run must return the legacy
> `AnchoredPrivateArea` suites green, since the trimmed probe run that
> validated this arm (`32569434565`) executed none of them.

> R2-E E0 annotation (2026-08-27, on the E10/E14 activation annotation above,
> left as written): its Windows negative space is unchanged; but its ground —
> only the checked-artifact private area is permitted to *retain* an anchor —
> gains the roaming case: a directory may **borrow** an anchor for one barrier
> without retaining one, which is what the sixteen `barrier.*` keys buy. Under
> DECISION B-5 (E0.2b §1) the borrowed object is a **fresh independent copy**
> carrying `ROAMING_ANCHOR_BYTES` — never a hard link and never a rename of
> the catalog's `roaming-anchor-home-v1` row; the per-hard-link macOS identity
> facts recorded in the Step-4.2 platform-fact paragraph (§3.5 above,
> `anchor/tests.rs:308-324`) are exactly why.
| E15 | managed component install (staged dir → final) | 2.3/3 | P1 + P2 + P3 | as E3 | as E3 | as E3 | no — but **needs a managed source-interior arm, §4.4 class, Phase 2.3/3** |
| E16 | managed ownership-marker retirement | 2.3/3 | P1 | as E13 | as E13 | as E13 | no — leaf-shaped today; **needs a destination arm only if the marker retires as a directory, §4.4 class, Phase 2.3/3** |
> E16 activation annotation (2026-08-22, Step 2.3 landing; discharges the
> Step-2.3 State review's [P2-3], and resolves this row's conditional). **The
> conditional resolves to no arm.** The marker retires as a *regular file*, not a
> directory: `managed_marker_name()` (`protocol/managed_bootstrap_record.rs`) is
> the frozen leaf `gwz-bootstrap-owner-v1`, and `namespace/operations.rs`'s
> pre-existing R1 role validator already pins the retirement source to
> `NamespaceObjectKind::RegularFile`. So E16 publishes through
> `PublicationSourceV1::regular_file` with `DestinationRecheckV1::None`, and
> `DestinationRecheckV1` is unchanged by Step 2.3. Consequently §4.4's arm table
> row "managed destination (marker/generation retirement) | E16, E17 | Phase 2.3
> / Phase 3" is, on this tree, driven by **E17 alone and owned by Phase 3
> alone**; that row is left as written rather than edited, since this annotation
> is the sanctioned mechanism and §4.3's conditional already carried the
> substance.
>
> **Cross-parent atomicity, recorded because the recovery path rests on it.**
> E16 is the lane's first cross-*directory* durable edge — every prior edge in
> the provider's namespace family renames within one retained handle — so it owes
> the cross-parent twin of the E4 record above. The rename is the commit point;
> the two `sync_directory_edge` calls that follow order the *observation*, not
> the atomicity. On the closed support table a rename spanning two parents of one
> filesystem is a single metadata transaction, replayed or discarded whole, so
> the reachable post-crash states are only: rename discarded (the drive re-enters
> the edge and re-derives the same scheduled row), or rename durable with either
> or both parents unflushed (the retirement row is resident and the drive
> short-circuits on it). Rows `marker_retire`, `marker_retired_reobserve` and
> `final_identity_reobserve` execute those states on both target variants; the
> window *between* the two flushes carries no namespace transition and therefore
> no key of its own, which is deliberate — minting one would move the frozen
> 165-key census. The state that would wedge a restart — marker absent from the
> component *and* the retirement row absent — is **not producible** on that
> table; a cross-filesystem attempt fails `EXDEV` before touching either parent
> (typed refusal, nothing durable changed, idempotent retry), and foreign removal
> is outside the accepted same-user namespace boundary. Off the supported table
> that state would be refused permanently and by design, typed rather than
> silent, by the installed-component interior check. Full derivation at the edge
> in `capability/pre_catalog/provider/managed_mutation.rs` `retire_marker`.

| E17 | managed successor publish, prior-generation retirement, final reproof | 3 | P1 + P2 + P3 | as E3/E13 | as E3/E13 | as E3/E13 | no — but **needs managed source-interior *and* destination arms, §4.4 class, Phase 3** |
| E18 | legacy leaf edge `transition.rs:275` | 4.1 | P1 (replaces `platform::rename_relative` `platform.rs:276`) | `platform.rs:276` today | `platform.rs:324` today | `platform.rs:276` today | no |
| E19 | legacy leaf edge `transition.rs:368` | 4.1 | P1 (same) | as E18 | as E18 | as E18 | no |
| E20 | legacy leaf edge `residue.rs:393` | 4.1 | P1 (same) | as E18 | as E18 | as E18 | no |
| E21 | legacy leaf edge `residue.rs:486` | 4.1 | P1 (same) | as E18 | as E18 | as E18 | no |
| E22 | legacy Windows durability anchor retirement | 4.2 | P1 + P5 | n/a (`cfg(windows)`) | `platform.rs:380-604`; random scratch at `:420` is the R2 stop-clause violation this step removes | n/a | no |

> R2-E E0 annotation (2026-08-27, per the E0.2 amendment pair; the row and the
> Step-4.2 record's optional-item clause in §3.5 above are left as written):
> the legacy nonce-orphan reclamation the Step-4.2 record calls "an optional
> R2-E item, not a debt of this step" is **DECLINED by R2-E** and stays
> optional — the count is bounded by past crashes and cannot grow,
> `legacy_nonce_orphans_are_tolerated_and_block_nothing` pins it, and the
> natural carrier is R2-F's legacy-path removal. Recorded so the option does
> not silently lapse.

**Activation record 2026-08-22 — E4's retire half, a first-of-kind removal
edge.** Recorded in the §3.5 activation-record form rather than by reshaping the
frozen table above, and entered on both settled review axes' finding (Code
[P2-1] ≡ State [P2-1]).

E4's row reads "P2 + P1", and that is accurate for the two halves the table was
written to describe: the write-ahead scratch (P2) and the no-replace publication
onto the active name (P1). Phase 1 implemented the composition as **write-ahead
scratch → retire the superseded active record → no-replace publish**, and the
middle step is a physical shape no admitted family expresses:

| Edge | Physical shape | Where |
| --- | --- | --- |
| E4-retire | `cap_std::fs::Dir::remove_file` of the frozen `ActionAdmissionActive` slot name, then the P2 parent flush (`platform::sync_parent`) | `capability/pre_catalog/provider/admission_mutation.rs`, `retire_admission_record` |

The record, stated once:

- **Why no admitted family covers it.** §4.1's five families are P1 sealed
  rename-publication, P2 write-through + flush, P3 identity, P4 bounded
  enumeration, P5 barrier. None is a removal. The §4.2 spike exercised no unlink
  on either platform. The publish half *is* over the sealed primitive; the
  checkpoint's shorthand "retire-then-publish over the no-replace primitive"
  overstates by covering both halves with the one phrase, and is corrected here.
- **Why the alternatives were unavailable rather than unattractive.** The
  publication family is no-replace by construction (§4.1, §8.13, and the
  checker's raw-rename scan), so the superseded record cannot be displaced by a
  replacing rename. Retiring it *by rename* needs a destination name, and
  `completed_record` requires an **empty** `RetiredActions` root, so no
  in-vocabulary target exists; minting one is forbidden by §6 and the §3.1
  persisted-home pin. Removal is what remains.
- **Write-ahead protection.** The next durable state is written, `sync_all`-ed,
  open-file verified, named-file verified and parent-flushed into
  `ActionAdmissionScratch` **before** the active name is touched. Authority
  lives in the active slot and the publish is the commit point, so the retire
  never destroys the only copy of a committed state — it removes a record whose
  successor is already durable.
- **Idempotence.** The edge is derived from the current durable state, targets a
  compile-time slot constant, and allocates nothing. A lost retire re-presents
  the pre-retire window and the driver retires again; a retire whose flush is
  lost is indistinguishable from one that never happened. Its durability is
  therefore not correctness-critical, which is also why the Windows
  delete-pending wrinkle degrades to a typed refusal plus restart convergence
  (a lingering name fails either the no-replace publish or the delete-pending
  open; both stop, and the state remains a covered window) rather than to
  corruption. ConsumerCheckpoint §8 (:242-243) is not violated.
- **Ordering against the publish is namespace-enforced, not timing-enforced.**
  Both edges target the same name in the same directory and the publish is
  no-replace, so on the closed support table (journaled NTFS/ext4/APFS) no crash
  state can hold both the superseded and the published record under one name.
  Losing the retire yields the pre-retire window; losing the publish yields the
  post-retire window; both converge.
- **Convergence evidence.** Both post-retire windows are now reconstructed on
  disk and re-entered as fresh processes by
  `every_admission_record_install_window_converges_to_the_settled_state`
  (`admission/driver/tests.rs`): the `idle` variant (step 7) as
  "retired, scratch idle", and — added in this round-2 remediation — the
  `preparing` variant (step 3, action row not yet published) as
  "post-retire, scratch preparing". The pre-retire windows are covered in both
  directions by "preparing, scratch idle" and "idle, scratch preparing". Under
  real interruption the four scratch publish/reobserve keys crash and restart
  green on both target variants.
- **Not a Track-P event.** `remove_file` is not a new platform capability: it is
  already production-used at `cleanup.rs:87/:127/:157`, `platform.rs:408` and
  `transition.rs:234`. §4.4's "New platform primitive required: **NO**" verdict
  stands unchanged; what was wrong was the *inventory*, not the verdict.

**Open question carried to the re-verdict — the fault key, deliberately not
minted.** No `admission.*` key sits between the unlink and the publish, so the
matrix's own claim that every row is "a real process stop across a real durable
edge" (`admission/tests_fault_matrix.rs`) does not hold for this one edge; its
post-state is covered by the on-disk window rows above instead. The State axis
offered two closures: (i) this record **plus** minting `admission.record_retire`
with its own matrix row, or (ii) this record alone, recording the accepted
no-key deviation with the window rows cited as covering evidence. **Route (ii)
is implemented.** Route (i) is not refused on the merits — it is stronger — but
minting a key moves the frozen census from **19/165 to 20/166** (§3.5,
`EXPECTED_KEY_COUNT`), which is a deliberate freeze edit and not a remediation
the implementation lane may take silently. The re-verdict owns the decision: if
the settle wants route (i), the count change is the reviewed edit it asks for.

> R2-E E0 annotation (2026-08-27, on the activation record above, left as
> written): its clause "`completed_record` requires an **empty**
> `RetiredActions` root, so no in-vocabulary target exists" stays true **of
> the admission record slot it was written about**; for the *action directory*
> an in-vocabulary name already exists (DECISION T-A) and the empty-root half
> was OPEN-T1's object — now an authorized widening (E0.2b §2, owner E3.1).
> Its "Open question carried to the re-verdict — the fault key, deliberately
> not minted" is **not reopened**: R2-E mints no key.

### 4.4 Verdict, and the two in-seam extension classes the R2-D phases own

**New platform primitive required: NO.** Every edge E1-E22 names an admitted
primitive with a macOS arm and a Windows arm (Linux shares the macOS arm on
P1/P2/P3/P5 and has its own identity child). (Drift annotation 2026-08-22,
ARM64 EBADF package: Linux's P2/P3 arms now route descriptor-consuming
operations through a same-object reopen — `GwzArm64EbadfDiagnosis.md`; an
intra-family repair, not a new primitive, per `GwzArm64Ebadf-Review.md`.) No platform spike is triggered, and
this freeze is not stopped (`GwzProcessOptimization.md` §3.1; brief's stop
condition).

What *is* required is bounded in-seam extension, in **two classes**. Round 1 of
this memo named one of them and called it "the one bounded extension"; both
statements were wrong and are corrected here. Neither class is a new platform
capability, a new durable record, or a bypass; both are owned inside the
existing pre-catalog provider owner (amendment §7 :569-577).

#### Class 1 — the recheck-arm class (contingency **C-2**)

**Definition, stated once.** The `PublicationSourceV1` interior/destination
recheck surface is the protocol-typed re-verification layer of the sealed
primitive; each converting package adds its family's arm inside the provider
owner under the same rules — no platform surface touched; acquisition window,
identity compare, retained handle, and no-replace rename unchanged; caller-pin
and digest refresh extended deliberately in the same commit.

**The surface today, and its true shape** (State P2-3 corrects round 1's "a
variant on two enums inside `publication.rs`"):

- `PublicationSourceV1::Directory { expected_identity, interior }`
  (`publication.rs:17-26`) carries a **struct**, not an enum —
  `DirectoryInteriorRecheckV1 { durable_identity, expected: &CatalogBootstrapRecordV1 }`
  (`:34-37`). The `expected` field is **mandatory and catalog-typed**, so the
  admission arm is a *generalization of that struct or its field type*, not the
  addition of a variant.
- `DestinationRecheckV1` (`:41-47`) **is** an enum (`None` |
  `PreRetirementFinal { durable_identity, expected: &CatalogBootstrapRecordV1 }`),
  so its arm genuinely is a new variant.
- The verification each one drives lives in a **different file of the same
  owner**: `interior::staging_plan(..) == StagingPlanV1::Complete(_)`
  (`publication.rs:145-155` → `interior.rs:122`) and
  `interior::completed_record(..)` plus the `CatalogBootstrapRetired` row check
  (`publication.rs:157-173` → `interior.rs:256`). So the extension necessarily
  reaches beyond `publication.rs` into `provider/interior.rs` — still inside the
  sealed provider owner, but "added inside `publication.rs`" was too narrow a
  boundary to check "bounded" against.

**Every arm, assigned** (this is what round 1 omitted — State P1-1):

| Arm | Driving edges | Owner |
| --- | --- | --- |
| admission source-interior (interior is a resident `ActionCapacityReservationV1`, not a `CatalogBootstrapRecordV1`) | E3 | **Phase 1** (C-2 as drafted) |
| admission destination | E7 | **Phase 1** (C-2 as drafted) |
| managed source-interior (a staged managed component's interior is neither record type) | E15, E17 | **Phase 2.3 / Phase 3** |
| managed destination (marker/generation retirement) | E16, E17 | **Phase 2.3 / Phase 3** |
| any further retirement-destination arm | E7's Phase-4 half and the terminal retirement edges | **Phase 4** |

> R2-E E0 annotation (2026-08-27, per the E0.2 amendment pair as corrected at
> E0.2b §3; the row is left as written): the last row resolves to **a new
> `DestinationRecheckV1` variant carrying a second observation input**
> (`TerminalRetiredRoot` — the catalog root's retained handle alongside the
> retired root arriving as `destination_dir`), owned by **R2-E E3.1**, on this
> class's own struct-vs-enum criterion above (`DestinationRecheckV1` **is** an
> enum, "so its arm genuinely is a new variant"); the field-generalization
> reading is refuted by `publication.rs:239`'s single observation of
> `destination_dir` (DECISION T-B′, replacing the E0.2 draft's T-B). The arm
> must NOT inherit `interior::observe`'s bounds — E3.1 owes an explicit
> `MAX_RETIRED_ACTION_DIRS` count check inside the arm, stated as its own
> sentence.

> R2-E E3 landing annotation (2026-08-27; the table is left as written): the
> arm table gains a row it did not assign, found STRUCTURALLY FORCED at E3.1
> — a terminal retirement's source is a directory, and this class's own
> closing paragraph states `PublicationSourceV1::Directory` has no
> "no interior recheck" form — and ruled owed by the E3 interior review (F2,
> ratified at the lane-owner escalation): *terminal source-interior (the
> retiring action directory's interior is a lived-in action directory, not a
> staged one) | E7's Phase-4 half | R2-E E3.1*. Shape as landed:
> `DirectoryInteriorExpectationV1::TerminalActionDirectory` — the admission
> arm's own bounded reader with the same `Exact` resident-reservation
> requirement, dropping only `extra_children == 0` (true of a freshly staged
> directory, never of one that has run an action). Second-axis scrutiny
> rides dual #2 (E7.1) per the deferred-escalation design.

The managed rows are not optional: `PublicationSourceV1::Directory` has no
"no interior recheck" form, amendment §8.13 (:656-659) structurally rejects raw
provider renames, and the checker's bare-identifier scan plus
`RAW_RENAME_CALL_ALLOWLIST` (`check_checked_artifact_boundaries.py:160-184`)
fail closed on any rename outside the sealed primitive. So a managed directory
publish has exactly one route, and that route demands an arm. Had this freeze
kept the "one extension" wording, Phase 2.3 would have hit a mid-lane
freeze-level stop the first time it published a staged component directory.

The caller count stays at six production sites until a phase deliberately
extends it (plan Step 1.2: "extend the six-site pin at
`interface_tests/capability_permit.rs:110` and `CATALOG_PUBLICATION_CALL_COUNTS`
deliberately, same-commit digest refresh").

#### Class 2 — the observer / slot-grammar class (contingency **C-3**)

Recorded for the first time in this revision. The pre-catalog **interior
observer** at `capability/pre_catalog/provider/interior.rs` is a closed grammar
over the catalog root, and admission is the first thing that writes into that
root. Three verified facts at `d32b2c9`:

1. **`exact_slot()` refuses any catalog-root child outside
   `InfrastructureSlotV1::ALL`** (`interior.rs:309-329`): it walks `ALL`
   (`:313`) and otherwise returns
   `CheckedFsError::ambiguous("catalog interior", "catalog directory contains
   an unowned child")` (`:325-328`). `ALL` is the ten infrastructure slots
   (`protocol/slots.rs:61-72`) — but the catalog root's *protocol* grammar is
   already wider: `RootEntryNameV1` (`slots.rs:366-369`) is
   `Infrastructure(slot) | ActiveAction(digest)`, one grammar whose
   `Infrastructure` arm is exactly this observer's ten slot names
   (`slots.rs:374`) and whose `ActiveAction` arm renders `action-<hex>-v1`
   (`:375-377`). The durable admission record already commits to that second
   arm — `ActionDirectoryAdmissionV1::preparing` sets
   `final_action_name: RootEntryNameV1::ActiveAction(action).name()`
   (`protocol/admission.rs:156`) — so the observer does not yet admit the very
   row E3 publishes. (`RootEntryNameV1::parse` has no production caller today;
   Phase 1 writes the first.)
2. **`completed_record()` and `staging_plan()` un-complete when any of the three
   `ActionAdmission*` slots is present**: `staging_plan` returns
   `StagingPlanV1::Other` (`interior.rs:127-138`) and `completed_record` returns
   `None` (`interior.rs:261-272`) if `any_present(..)` finds
   `ActionAdmissionActive`, `ActionAdmissionScratch`, or `ActionAdmissionStaging`.
3. **`MAX_INTERIOR_ENTRIES = 10` caps the root** (`interior.rs:27`, enforced at
   `:64-69` with "catalog interior exceeds the ten-slot bound"). Ten is exactly
   `|InfrastructureSlotV1::ALL|`, so a fully-populated catalog root has **zero
   headroom**: the cap and the grammar have to move together.

**Consequence.** A completed admission breaks the *next* catalog reobservation —
ConsumerCheckpoint §7 step 8 ("reobserve the complete catalog"), which is the
step that gates step 9's `AdmittedActionV1`. The chain is exact:
`catalog/bootstrap.rs:145-149` `recover_or_create` →
`capability/pre_catalog.rs:297-308` `execute_owner_complete` →
`provider/completed.rs:49` `retain_completed_catalog`, which refuses unless
`interior::completed_record(..)` is `Some` (`completed.rs:61`); the same
predicate guards the retained-catalog revalidation at `completed.rs:178-216`.
With an `ActionAdmissionActive` slot resident — the steady state after a
successful admission — that predicate is `None` by fact 2, and with an
`action-<hex>-v1` child present the observation fails even earlier, at fact 1.

**Ruling and assignment.** C-3 is an **observer/slot-grammar extension in the
same provider owner**, owned by **Phase 1**, under exactly the rules of the
recheck arms above: in-memory verification only, no platform surface, and a
deliberate caller-pin/digest extension in the converting commit. It is not a
Track-W event and needs no new durable record, slot, purpose, action, or phase —
the admission triad is already in the frozen vocabulary and the record binding
already pins it (§3.1). What Phase 1 must extend is the *provider's reading* of
that vocabulary, not the vocabulary.

> R2-E E0 annotation (2026-08-27, per the E0.2 amendment pair; the ruling above
> is left as written): its rule — extend the provider's *reading*, not the
> vocabulary — is the named precedent for **OPEN-T1's authorized widening**
> (E0.2b §2: three gates — `interior.rs:349`, `completed.rs:310`,
> `completed.rs:383` — owner E3.1, census unchanged), one further reading of
> the same closed grammar (the populated retired root), minting no vocabulary.
> OPEN-B1's cure takes **no** Class-2 sanction: DECISION B-5 (fresh-copy
> alias, E0.2b §1) touches no catalog predicate, and E2.1 holds no
> authorization to edit `completed_record`, `retain_file` or
> `require_named_file_identity`.

#### Why this is recorded rather than executed here

Step 0.1 adds no physical code. The §4.2 spike deliberately exercises the
*regular-file* arm, which needs no extension, so the freeze rests on executed
evidence for the mechanics (open, identity compare, no-replace publish,
cross-parent retire, substitution rejection — now on macOS **and** Windows) and
on named, assigned, bounded obligations for the typed arms and the observer
grammar that do. Both classes are contingencies **(C-2)** and **(C-3)** at §2.

## 5. The six §7 owner decisions, as adopted

Transcribed from `GwzM5-8R2D-Plan.md` §9 (adoption record, lane owner,
2026-08-16). This memo records them; it re-decides nothing.

1. **C3 badging — option (a):** "admission executes as Phase 1 of this lane,
   badged as R2-C's tail. RemPlan §8 item 19's 'settled implementation reviews
   after C2/C3' is preserved as the Phase 1 dual settled gate; no separate R2-C
   closure checkpoint."
2. **Coexistence (Phase 4.3) — direction: quarantine/relocation preferred**,
   "because it also retires the MAX_PATH exposure (checkpoint :144-147);
   convert-in-place remains live as fallback. The binding reviewed decision is
   made at Phase 4.3 per the plan text — in any case before any production
   catalog activation — and Step 0.1's freeze memo carries the Track-P analysis
   for both options so the 4.3 decision is evidence-based, not re-scoped."
   Discharged by §4.3: both options run on the same primitive set. Convert-in-place
   converts E18-E21 in situ through P1; quarantine/relocation moves the legacy
   private area first and then converts the same four edges through the same P1
   arms — *the move itself executes on the R2-F-candidate relocation package, on
   the same P1 family, not inside R2-D*, since plan §5 item 7 keeps MAX_PATH and
   relocation execution out of this lane ("Phase 4.3 only *decides*
   compatibly"), which is why the move is not an E-row above (State P3-4).
   Neither option needs a primitive the other does not, so the 4.3 choice
   is a coexistence and MAX_PATH question, not a platform question.
3. **Backend trait surface — confirmed:** "Step 0.1 freezes the delta that
   removes the four `managed_operation_unavailable` defaults
   (`namespace/backend.rs:411-460`) and makes the methods required; the 0.1
   dual review is the gate for changing the frozen R1 trait." Executed at §3.2,
   subject to the §2 contingency the operator instruction adds on top.
4. **Fault-family activation map — confirmed** "as tabled at §4 end; the
   matching RemPlan §10 update files in the Phase 0 docs commit." Executed at
   §3.5 and at the RemPlan §10 annotation (§8, D4).
5. **Dirent-barrier resume-window — conditional:** "in-line fix at 4.2/4.3 if it
   fits the step budget, else the bounded §6-style erratum + R2-F power-loss pin
   (`DirentBarrier-ReviewState.md:393-395` allows either); ratified in the Phase
   0 freeze memo." **Ratified as written**: the conditional stands unchanged,
   the decision point is Phase 4.2/4.3, and this freeze adds no fact that
   forces either branch — the residual is a resume-window property of the
   barrier family (P5), and no E1-E22 edge above depends on which branch is
   taken.
6. **Tier recording — confirmed:** "the §4 gate tiers enter
   `CurrentProgramCheckpoint.md` in the Phase 0 freeze commit." The tiers to
   record are those in §9 below, which are the operator's three duals, not the
   four the plan §6 nominally scheduled.

## 6. Wire and records rule — zero new durable records

R2-D expects **zero new durable records**; the R1+C0 record set in
`protocol/generated.rs` is the frozen floor (plan §4 "Wire/records rule",
ConsumerCheckpoint §14 :378-380, amendment :610-614). This package adds no
record, no slot, no purpose, no action, and no durability phase; it adds no
field to `protocol/generated.rs` and touches no `.taut`/protocol source. The
schema gate at `interface_tests/durable_path_schema.rs:14-36` is unchanged and
green.

Any discovered need for a new or changed durable record, slot, purpose, or
phase stops the step for a Track-W freeze plus dual review before any
implementation. **Neither §4.4 extension class is such a need**, and round 1's
one-sentence description of the first class was imprecise (State P2-3); stated
correctly:

- **C-2, the recheck-arm class** — a variant on the `DestinationRecheckV1`
  **enum** and a generalization of the `DirectoryInteriorRecheckV1`
  **struct**'s `expected` field, with matching interior-verification support in
  the same provider owner (`provider/interior.rs`). Both types are
  `pub(super)`, lifetime-parameterized reference holders
  (`publication.rs:34-37`, `:41-47`) with no encode path; they are constructed
  in `mutation.rs`/`directory_mutation.rs`, consumed inside
  `publish_verified_no_replace` (`publication.rs:82-183`) as read-only
  pre-rename verification, and dropped. Nothing is serialized and nothing is
  reachable from a durable-record root.
- **C-3, the observer/slot-grammar class** — a widening of what
  `provider/interior.rs` *accepts and calls complete*. It reads the durable
  vocabulary that already exists (§3.1: the `ActionAdmission*` triad and
  `RootEntryNameV1::ActiveAction`, both frozen at R1+C0) and mints nothing. The
  ten-slot cap `MAX_INTERIOR_ENTRIES` is a provider-side bound, not a durable
  record.

Every durable item either class touches is already in the frozen floor, so both
are ordinary in-seam work for their assigned phases, not Track-W stops.

## 7. Line-reference drift corrected against the plan

The plan's citations were verified at `f36d20d`; this memo is verified at
`d32b2c9` (four trains later, plus the two docs trains since, plus this
package). The "Actual" column is stated at `d32b2c9`; where the package itself
moved a line, the pre-package position is given as well. Corrections:

| Plan citation | Actual on `d32b2c9` | Note |
| --- | --- | --- |
| `provider/publication.rs:48` for `publish_verified_no_replace` | `publication.rs:82` | `:48-80` is now the `PublicationSourceV1` impl block |
| `namespace/backend.rs:411-460` for the four defaults | at `90d3f8a`: `:411-451` defaults, helper at `:460-465`; **both are gone at `d32b2c9`** and the trait is `:375-457` | matched closely; the range included the helper |
| `bootstrap/managed/owner.rs:16-32` | `:16-32` | correct |
| `namespace/mod.rs:305` for `NamespaceProtocol` | `:305` (`cfg(not(test))`); test builds use `test_support::NamespaceProtocol` re-exported at `:349` | the plan does not note the cfg split |
| `leaf.rs:74` for `LeafObserver` | `:74` | correct |
| "sole impls in `interface_tests/contracts.rs:183/:236`" | **no `LeafObserver` impls exist**; `:183` is `impl ExpectedLeafContent for Bytes`, `:236` is `impl NamespaceProtocol for RecordingNamespace` | material correction — see §3.3 |
| fault inventory "164 keys (`fault_expected_keys.rs:171`)" | **165 keys**, `EXPECTED_KEY_COUNT` at `:174`, list ends `:172` (post-round-2 positions) | +1 since the plan |
| "`catalog_bootstrap.*` = 24 keys" | **25** | +1 since the plan; RemPlan §10 says 21, older still |
| `cargo test --lib checked_artifact::` "244 passed" | **245 before this package**, 256 after round 1, **257 after the round-2 completeness anchor** | +1 since the plan; §10 |
| `platform.rs:372-505` Windows anchor, random scratch at `:409` | anchor block `:379-604`, `ANCHOR_PREFIX` `:383`, `prepare_private` `:386`, random scratch at **`:420`**, `private_barrier` `:458` | the adoption record predicted this drift ("may have drifted by the two added `windows_tests`") |
| `catalog/bootstrap.rs:145` `recover_or_create` | `:145` | correct |
| `interface_tests/capability_permit.rs:110` six-site pin | `:110` | correct |
| `transition.rs:275/:368`, `residue.rs:393/:486` | all four confirmed | correct |

## 8. What Step 0.2 landed (scaffolding inventory)

All tests pass at landing. They are freeze pins, not red tests: each fails only
if a later package widens or reshapes a frozen seam without saying so.

**D2 — signature stubs.**

- `namespace/backend.rs` — four defaults removed, methods required, sentinel
  helper deleted from the production trait file (**+19/−27 = net −8 lines**;
  round 1 said "net -1", which was wrong — Code P3-1; §3.2).
- `namespace/test_support.rs` — blanket legacy adapter states the four
  operations, with the byte-identical refusal (+53).
- `admission/mod.rs` — new owner skeleton, frozen seam, typed refusal (+64 new).
- `checked_artifact/mod.rs` — `mod admission;` with the R2-D dead-code reason
  (+5).

**D3 — failing-test scaffolding (green-at-landing).**

- `interface_tests/r2d_seam_freeze.rs` (new, **240** lines, 6 tests; round 1
  said 239 — Code P3-2):
  `managed_backend_operations_are_required_not_defaulted` (`:17`) pins the
  required declarations and the absence of both default bodies and the
  sentinel; `every_namespace_backend_implementation_states_the_managed_operations`
  (`:64`) pins the complete impl inventory;
  `admission_owner_consumes_only_the_opaque_retained_catalog` (`:111`) is the
  amendment §8.10 pattern for the new seam — required items plus nine forbidden
  caller-authority tokens plus an exact count of exposed items;
  `leaf_observer_provider_seam_is_frozen` (`:155`);
  `managed_parent_bootstrap_provider_seam_is_frozen` (`:187`);
  `the_track_p_publication_spike_is_test_only` (`:218`).
- `interface_tests/fault_expected_keys.rs` (**+276** across rounds 1-2; +190 at
  round 1, +86 net in the round-2 correction): the activation map
  (`FaultFamilyActivationV1` at `:187`, `FAULT_FAMILY_ACTIVATION` at `:193`,
  `FAULT_INJECTION_SOURCES` at `:257`) and **four** tests —
  `every_fault_family_declares_its_owning_activation_package` (`:293`) makes the
  map total against the key fixture and pins each family's key count;
  `reserved_fault_families_have_no_injection_sites_before_their_package`
  (`:333`) fails when a reserved family gains an injection site *in a declared
  source* without its activation row being flipped in the same package;
  `the_declared_injection_sources_are_every_production_source_holding_sites`
  (`:418`, **new in round 2**) is the completeness anchor that makes the
  previous test tree-complete — it walks `src/checked_artifact/` under the same
  production-file rule the boundary checker uses
  (`check_checked_artifact_boundaries.py:663-670`: no `tests`/`interface_tests`
  component, no `tests`-prefixed file name) and fails if any production source
  naming `CheckedArtifactFaultKeyV1` is absent from `FAULT_INJECTION_SOURCES`;
  `only_the_runtime_and_catalog_bootstrap_families_are_executed_today` (`:435`)
  makes any activation-state change a deliberate, reviewed edit.

  The anchor was probed adversarially: with the `aggregate.rs` row removed it
  fails and names the missing file
  (`left: {aggregate.rs, directory_mutation.rs, mutation.rs}` vs
  `right: {directory_mutation.rs, mutation.rs}`); with the row present it
  passes. Its scan is a runtime source-tree walk rather than `include_str!`,
  because an `include_str!` inventory cannot, by construction, notice a file
  nobody registered; the same idiom is already in the crate at
  `workspace_ops/merge/v1_lifecycle/tests/reverse_entry.rs:514-546`.
- `capability/pre_catalog/provider/tests_admission_spike.rs` (new, **191** lines,
  2 tests; round 1 said 192 — Code P3-2) — the Track-P spike of §4.2, wired at
  `provider.rs:199-200`.

**D4 — RemPlan §10 annotation.** A dated, append-only block at the end of
`GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` §10 recording that the activation
map is frozen here, with the per-phase assignments in one list. No existing line
was modified. Round 2 appends a second dated correction paragraph to that same
block — still append-only, nothing above it touched — recording the third
injection source, the completeness anchor, and the DRAFT status of the freeze
that the block cites (State P2-1, State P3-3).

## 9. Gate — review tier for this package

**This memo and the Step 0.2 scaffolding land under ONE mandated dual
peer-blind review (interface-freeze tier), cross-model where available**
(plan §4 Phase 0 Step 0.1 gate; Step 0.2 "Rides Step 0.1's review";
`GwzProcessOptimization.md` §4.2/§4.3). **Two-round remediation cap** (§4.1): a
third new architectural root cause on the same object is redesign-or-accept,
not another rem-plan series.

**R2-D carries three duals in total, not four.** Per `GwzFasterProposal.md` §2,
quoted at `CurrentProgramCheckpoint.md:72-81`:

| # | Gate | Tier |
| --- | --- | --- |
| 1 | **Phase 0 interface freeze** — this package | mandated dual peer-blind, cross-model |
| 2 | **Phase 1 admission kernel** — `Idle`↔`Preparing` treated as a durable-transition kernel | mandated dual peer-blind |
| 3 | **Phase 5 settled-tree gate** | mandated dual peer-blind, cross-model |

Everything else is **single-axis with automatic escalation on any P0/P1/P2**:
Phase 2 steps 2.1-2.4, Phase 3 steps 3.1-3.3 **including the Phase 3 settle**,
and Phase 4 steps 4.1-4.3 **including the Phase 4 settle**. This supersedes the
plan §4 gate lines that assign a dual at the Phase 3 and Phase 4 settles; the
plan §6 "4 dual gates" estimate is correspondingly 3. P3 findings file and
continue; they do not become packages and do not enlarge R2-D. The L2-04
retained-reader harness remains a hard gate at Phase 4 in addition to review.

Recording duty: these tiers enter `CurrentProgramCheckpoint.md` in the Phase 0
freeze commit (adopted decision §9.6; `GwzFasterProposal.md` §3 Step C.3 —
"when the freeze lands, not mid-lane"). That edit is the lane owner's, not this
package's.

Parallelism permitted behind this gate: Phase 1 failing tests may start while
this freeze is in review (`GwzFasterProposal.md` §2 final line;
`GwzProcessOptimization.md` §4.4).

## 10. Verification appendix — commands executed

### 10.1 Round 1 (2026-08-16), at the parked object

All from `gwz-core/`, foreground, on this macOS host, with the working tree
carrying only this package's changes.

```text
git rev-parse HEAD            → 90d3f8a23d27b1dc00c6921b2526acd09190c0e6
cargo fmt --all -- --check    → clean (exit 0)
cargo clippy --all-targets --all-features -- -D warnings
                              → Finished, exit 0
cargo test --lib checked_artifact::
                              → ok. 256 passed; 0 failed; 0 ignored;
                                1115 filtered out; 10.26s
                                (245 before this package: +6 seam-freeze,
                                 +3 fault-activation, +2 Track-P spike)
cargo test --lib checked_artifact::capability::pre_catalog::provider::tests_admission_spike
                              → ok. 2 passed; 0 failed
python3.13 scripts/checks/check_checked_artifact_boundaries.py
                              → failed, exactly two digest pins, both for
                                pinned files this package edited:
                                - protected source allowlist changed:
                                  checked_artifact/mod.rs
                                - protected source tree changed:
                                  checked_artifact/capability/pre_catalog.rs
                                No allowlist-count, bare-token, module-rule,
                                publication-seam, catalog-lease-reference, or
                                provisional-interface finding.
```

The two digest findings were expected and were refreshed by the lane owner in
the same commit as the code, per the adopted lane rule
(`CurrentProgramCheckpoint.md:483` and `:495-497` at root `13b98df` —
"same-commit digest refresh", "digest refresh as the literal last pre-commit
step"; round 1 of this memo cited `:186-204`, which is not that rule —
State P3-1): `checked_artifact/mod.rs` gained `mod admission;`, and the
`checked_artifact/capability/pre_catalog.rs` tree gained
`provider/tests_admission_spike.rs` plus its `#[cfg(test)]` module edge in
`provider.rs`. No other pinned file was touched, and the checker itself was not
edited. Both re-verifications confirm the checker is **GREEN at `d32b2c9`**.

**Full-suite status.** The whole `cargo test --lib` run exceeds this host's
foreground budget and was executed as an exact partition of all **1 371** lib
tests at `d32b2c9` — not 1 367, which round 1 stated and which its own focused
line (256 passed + 1 115 filtered) contradicts (Code P3-3, State P3-2). Result:
**1 370 pass, 0 fail, 1 unexecuted**. The unexecuted test is
`root_fault_matrix::every_root_physical_and_successor_boundary_recovers_without_repeating_mutation`
(`workspace_ops`, >585 s alone) — pre-existing, outside this package's file set,
and green in CI at `90d3f8a` (run 13); confirm on the next matrix run. This is
recorded in the pause checkpoint (`CurrentProgramCheckpoint.md:67-72`) and is
carried here so §10 states it too.

### 10.2 Round 2 (2026-08-22), remediation gates

The round-2 edit touches three files:
`dev-docs/GwzM5-8R2DInterfaceFreeze.md` (this memo),
`gwz-core/src/checked_artifact/interface_tests/fault_expected_keys.rs`, and the
package's own dated block in `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` §10.

Gate context: the gwz-core working tree also carries **another lane's**
uncommitted files — `src/checked_artifact/admission/tests.rs` (6 tests, red by
design, the Phase-1 failing-test start permitted by §9) plus modifications under
`src/workspace_ops/`. Those are outside this package and are attributed below,
not fixed here (Code P3-8).

```text
cargo test --lib checked_artifact::
      → test result: FAILED. 257 passed; 6 failed; 0 ignored; 0 measured;
        1136 filtered out; finished in 21.01s
        The 6 failures are all checked_artifact::admission::tests::* — the
        other lane's red-by-design Phase-1 tests. The package's own count
        rises 256 → 257: +1 for the round-2 completeness anchor, so the
        decomposition is now 245 + 6 seam-freeze + 4 fault-activation
        + 2 Track-P spike = 257.

cargo test --lib checked_artifact::interface_tests::fault_expected_keys
      → test result: ok. 6 passed; 0 failed; 0 ignored; 0 measured;
        1393 filtered out; finished in 0.01s

cargo test --lib checked_artifact::capability::pre_catalog::provider::tests_admission_spike
      → test result: ok. 2 passed; 0 failed; 0 ignored; 0 measured;
        1397 filtered out; finished in 0.03s

cargo fmt -- --check src/checked_artifact/interface_tests/fault_expected_keys.rs
      → exit 0 (clean). Scoped to this revision's own file; the tree-wide
        run would flag the other lane's files.

python3.13 scripts/checks/check_checked_artifact_boundaries.py
      → checked-artifact boundary: failed
        - protected source tree changed: workspace_ops/merge/v1_lifecycle/mod.rs
        exit 1
```

Checker findings at round-2 commit time, corrected per ReviewCode round-2
[P1-3]: the remediation's own files owed no digest refresh (the checker
excludes `interface_tests` from every rule it applies,
`check_checked_artifact_boundaries.py:668`, so
`interface_tests/fault_expected_keys.rs` is not a pinned file), **but** the
round-2 commit also carried the ReviewCode P3-4 one-line citation fix in
`tests_admission_spike.rs`, which sits inside the
`checked_artifact/capability/pre_catalog.rs` **tree** digest — that pin was
not refreshed in the same commit (a literal-last ritual miss by the lane
owner, caught by the round-2 re-verdict) and is refreshed to
`877e9d18…b3a3e3c1` in the P1-3 fix commit that carries this sentence. The
remaining working-tree finding (`workspace_ops/merge/v1_lifecycle/mod.rs`)
belongs to the other lane's uncommitted files, as both round-1 reviews
recorded. No allowlist-count, bare-token, module-rule, publication-seam,
catalog-lease-reference, or provisional-interface finding appeared in any
round.

`cargo clippy` was not re-run in round 2: the other lane's working-tree files
break it independently of this package (round 1 recorded exit 0 on a clean
extraction of the object, and both reviews reproduced that).

### 10.3 Windows arm of the Track-P spike (2026-08-22)

GitHub Actions run **32542469665**, workflow "Windows matrix", branch
`probe/track-p-spike`, commit `e448d7f` (`d32b2c9` plus a workflow trim only —
no `src/` change):

```text
checked_artifact::capability::pre_catalog::provider::tests_admission_spike::
  admission_publish_and_retire_edges_use_the_sealed_publication_primitive ... ok
  admission_publish_rejects_a_substituted_source_before_the_namespace_edge ... ok
test result: ok. 2 passed; 0 failed; 0.04s; 1332 filtered out
```

This is the Windows arm of the Step C.1 spike (§4.2). Tracked acceptance item:
**both spike cases green on the next full Windows matrix run.**
