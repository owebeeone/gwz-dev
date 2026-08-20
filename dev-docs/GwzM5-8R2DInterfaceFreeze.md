# R2-D interface freeze — admission, namespace backend, leaf, and managed-parent seams

Date: 2026-08-16

Status: **DRAFT — pending mandated dual review (interface-freeze tier).**
This is R2-D Phase 0 Step 0.1 of the adopted plan, landing together with the
Step 0.2 scaffolding under **one** dual peer-blind review (adopted plan §4
Step 0.2 "Rides Step 0.1's review"; `GwzFasterProposal.md` §3 Step C.2).
Nothing physical mutates in this package.

Verified tree: gwz-core `90d3f8a23d27b1dc00c6921b2526acd09190c0e6`, workspace
root `d6cd2cb`. Every file:line in this memo was re-verified against that tree;
the plan's own citations were taken at `f36d20d` and several have drifted, so
§7 lists each correction. Working tree at drafting time carries only this
package's changes (§8).

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
primitive per platform". §4 discharges that condition: every R2-D physical edge
names an admitted primitive with a macOS arm and a Windows arm, and the
verdict is **no new platform primitive**. The delta is therefore implemented in
this package, and the freeze is recorded as **contingent on two facts that the
dual review must confirm**:

- **(C-1)** the Track-P table at §4.3 is per-platform-complete and correct, and
- **(C-2)** the one bounded extension identified at §4.4 — an admission arm on
  the sealed primitive's *source-interior* and *destination* recheck enums — is
  accepted as an in-seam extension owned by Phase 1, not as a new primitive and
  not as a bypass.

If the review rejects either, the trait delta reverts to paper-freeze and the
Phase 1 step re-spikes first; the delta is confined to
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

Body today: a typed refusal in house style
(`CheckedFsError::ambiguous("action admission", "physical admission driver is
implemented in R2-D phase 1")`, `admission/mod.rs:59-64`) — the same idiom the
removed backend defaults used. Phase 1.2 replaces it with the nine-step durable
sequence of ConsumerCheckpoint §7 (:209-224). The skeleton is compile-checkable
and boundary-legal: the boundary checker's module rules, raw-rename scan,
catalog-lease reference sets, and provisional-interface scan all pass over it
unchanged (§10 gate 4).

### 3.2 Seam 2 — `RawNamespaceBackend` managed-operation delta

`gwz-core/src/checked_artifact/namespace/backend.rs:375-457`. Before this
package the four managed operations carried
`Err(managed_operation_unavailable())` default bodies at `:411-451` with the
helper at `:460-465`. After it they are **required**:

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

| Implementation | Site | Before | After |
| --- | --- | --- | --- |
| `ProductionShapedBackend` (always-compiled provider shape) | `namespace/provider_compile.rs:153-281` | states all four (`:204`, `:219`, `:239`, `:254`) | unchanged |
| `RecordingNamespaceBackend` (test backend) | `namespace/test_support.rs:412-554` | states all four (`:471`, `:488`, `:508`, `:525`) | unchanged |
| blanket legacy adapter `impl<Protocol: NamespaceProtocol>` | `namespace/test_support.rs:137-188` | inherited the four defaults | states the four explicitly, returning the identical `CheckedFsError::ambiguous("managed namespace operation", "provider does not implement exact post-observation")` from `test_support.rs:243-248` |

The sentinel constructor moved from the production trait file into the
test-only adapter (`test_support.rs:243-248`), byte-identical in variant, fact
string, and detail string. The only type reaching the blanket adapter is
`RecordingNamespace` (`interface_tests/contracts.rs:236`), which never calls a
managed operation; the full suite is unchanged (§10 gate 3).

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
| `catalog_bootstrap.*` | 25 | R2-C2 physical first-catalog owner | **executed** |
| `admission.*` | 19 | R2-D **Phase 1** (R2-C3 admission) | reserved |
| `durable_leaf.*` | 11 | R2-D **Step 2.1** | reserved |
| `namespace.*` | 11 | R2-D **Steps 2.2/2.3** | reserved |
| `record.*` | 13 | R2-D **Step 2.4** | reserved |
| `managed_bootstrap.*` | 30 | R2-D **Phase 3** | reserved |
| `cleanup.*` | 11 | R2-D **Phase 4** (step 4.1 legacy leaf edges) | reserved |
| `barrier.*` | 16 | R2-D **Phase 4** (step 4.2 Windows retirement closure) | reserved |
| `terminal.*` | 11 | R2-D **Phase 4** (step 4.2, terminal retirement edges) | reserved |

Total 165 keys, equal to `EXPECTED_KEY_COUNT`
(`interface_tests/fault_expected_keys.rs:172`).

Two clauses the map carries forward verbatim from the plan:

- "any key whose edge genuinely converts later is explicitly re-reserved for
  R2-E/R2-F in the same update" (plan §4-end);
- RemPlan §10's duty (:1033-1035): when a family's edges convert, "that family
  must gain injection sites and matrix rows in the same package that converts
  its edges".

`runtime.*` is recorded as executed on a different mechanism from the rest: its
edges are driven by the separate `RuntimeBootstrapFault` enum
(`bootstrap/runtime/fault.rs:19`) exercised from `bootstrap/runtime/tests.rs:21/:37/:73`
and `bootstrap/runtime/catalog_lease/tests.rs:163/:189/:212` plus
`catalog_lease/tests/association.rs:198/:272`, not by
`CheckedArtifactFaultKeyV1::Runtime*` references. The Step 0.2 fixture encodes
that exception explicitly rather than papering over it
(`interface_tests/fault_expected_keys.rs:334-346`).

The only `CheckedArtifactFaultKeyV1` injection sites on the tree today are
`catalog_bootstrap.*` in `capability/pre_catalog/provider/mutation.rs` and
`capability/pre_catalog/provider/directory_mutation.rs`; every reserved family
has zero, by design, which the fixture now asserts (§8, D3).

## 4. Track P — physical primitive inventory, per platform

### 4.1 The already-proven primitive families

| # | Primitive family | Definition | macOS / Linux arm | Windows arm |
| --- | --- | --- | --- | --- |
| P1 | Sealed source-associated publication (`publish_verified_no_replace`) | `capability/pre_catalog/provider/publication.rs:82-183` | `platform::open_rename_source` `platform.rs:23-46` (`cfg(not(windows))`, no-follow, `maybe_dir`) then `platform::rename_open_source` `platform.rs:78-96` — retains the identity-checked handle across a relative no-replace rename | `platform::open_rename_source` `platform.rs:48-76`, `platform::rename_open_source` `platform.rs:98-157` — renames the **exact identity-checked handle** via `SetFileInformationByHandle`; absolute destination derived from the retained destination handle at `platform.rs:196-259` |
| P2 | Write-through open + handle flush + parent flush | `durable_write_options` `mutation.rs:377-394` and `directory_mutation.rs:703-727`; `file.sync_all()` `mutation.rs:163/:236`, `directory_mutation.rs:561` | `platform::sync_parent` `platform.rs:367-370` (`cfg(not(windows))`, directory fsync) | `platform::sync_parent` `platform.rs:372-377` is a documented no-op; Windows durability is the write-through open plus the anchor barrier in P5 |
| P3 | No-follow open + durable identity compare | `platform::open_dir_share_delete` `platform.rs:159-163` / `:165-194`; `HostPlatform` identity at `capability/pre_catalog/provider/platform.rs:23-33`; encoding at `provider/retained.rs:417-427` | per-OS children `provider/platform/linux.rs`, `provider/platform/macos.rs` selected by the `#[path]` edges at `provider/platform.rs:8-13` (macOS: `getattrlist` persistent object/volume identity, `fpathconf(_PC_CASE_SENSITIVE)`) | `provider/platform/windows.rs` via `provider/platform.rs:14-16`; `open_dir_share_delete` `platform.rs:165-194` adds the `FILE_SHARE_DELETE` recipe so the directory open does not collide with the retained rename-source handle |
| P4 | Bounded interior / global enumeration | `provider/interior.rs:50-91` with `MAX_INTERIOR_ENTRIES = 10` (`interior.rs:27`); global catalog grammar budgets `catalog/enumeration.rs:8-12` (4 096 parent entries, 255 native name units, 510 encoded bytes) | portable; identical code path | portable; identical code path |
| P5 | Dirent barrier | `platform::private_barrier`; catalog root-flush callers at `mutation.rs:396/:412/:426` and `directory_mutation.rs:729` | `platform.rs:271-274` (`cfg(not(windows))`, directory handle fsync) | `platform.rs:458-514` — the durability-anchor round trip (`ANCHOR_BYTES` `:380`, `ANCHOR_PREFIX` `:383`, `prepare_private` `:386`, `anchor_state` `:517`, `anchor_roundtrip_name` `:575`, `verify_anchor` `:582`) |

### 4.2 Spike executed for this freeze

`GwzFasterProposal.md` §3 Step C.1 requires the admission publish/retire path to
be spiked against the sealed family before the freeze is reviewed. Executed as
two `#[cfg(test)]` cases in
`gwz-core/src/checked_artifact/capability/pre_catalog/provider/tests_admission_spike.rs`:

- `admission_publish_and_retire_edges_use_the_sealed_publication_primitive`
  (`:72-149`) — deterministic indexed staging name, write + flush, no-replace
  publication into a catalog-interior destination through
  `publish_verified_no_replace`, reobservation through the same retained parent
  (staging name consumed, identical object identity, identical bytes),
  a no-replace collision proof that leaves the published object untouched, then
  a cross-parent retirement edge with the same reobservation.
- `admission_publish_rejects_a_substituted_source_before_the_namespace_edge`
  (`:154-192`) — same name, different object: the primitive's identity compare
  rejects before the edge and performs zero namespace mutation (amendment §4.1
  drift-rejection paragraph).

Both are runnable standalone by name and were executed green on this macOS host
(§10 gate 5). **Windows arm status:** these two cases have not been executed on
Windows by this package — no native Windows runner is available in this lane, and
the plan assigns native fault evidence to R2-F (amendment :667). They will
execute on Windows the next time the platform matrix runs. The Windows evidence
that P1 carries the *same* physical edge shape today is the accepted run-13
matrix (1322/0/1, GREEN at `90d3f8a`, `CurrentProgramCheckpoint.md` ledger) over
the six sealed publication moves, which already include a directory
staging→final publish and an active→retired cross-parent retirement through
`rename_open_source`'s `SetFileInformationByHandle` arm.

**Seam discipline of the spike.** The boundary checker pins the production
caller set of `publish_verified_no_replace` to exactly `mutation.rs` (1 call)
and `directory_mutation.rs` (5 calls)
(`scripts/checks/check_checked_artifact_boundaries.py:816-856`;
`CATALOG_PUBLICATION_CALL_COUNTS` at `:648-651`; mirrored by the six-site pin at
`interface_tests/capability_permit.rs:110`). A spike naming the primitive from
any ordinary file under `provider/` would join `publication_callers` and fail
that rule. The spike therefore lives in a `tests`-prefixed file, which
`production_rust_files` (`:663-670`) excludes, so the production caller
inventory is unchanged and the checker's publication-seam rule still passes
(§10 gate 4). Step 0.2 pins that arrangement
(`interface_tests/r2d_seam_freeze.rs:218-239`).

### 4.3 Every R2-D physical edge mapped onto a proven primitive

Platform columns record which arm serves the edge: **mac** = macOS,
**win** = Windows, **lin** = Linux. All five families are portable across the
three; the table records where the arms differ in code rather than repeating
the same cell.

| # | R2-D physical edge | Phase | Primitives | mac | win | lin | New primitive? |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E1 | admission staging action directory create | 1 | P2 + P3 | `platform.rs:367` fsync | write-through open; `sync_parent` no-op `platform.rs:372` | as mac | no |
| E2 | resident reservation write + flush | 1 | P2 | `mutation.rs:377`, `sync_all` `:163` | same, `directory_mutation.rs:703` options | as mac | no |
| E3 | admission staging → final action publish, no-replace, catalog interior | 1 | P1 | `platform.rs:23/:78` | `platform.rs:48/:98` exact-handle | as mac | no — spiked §4.2 |
| E4 | admission record `Idle`/`Preparing` scratch → publish | 1 | P2 + P1 | as E2/E3 | as E2/E3 | as E2/E3 | no |
| E5 | post-publish reobservation of identity and exact record | 1 | P3 + P4 | `provider/platform/macos.rs` | `provider/platform/windows.rs` | `provider/platform/linux.rs` | no |
| E6 | bounded global classification of interior action rows | 1 | P4 | `interior.rs:50`, `enumeration.rs:8-12` | identical | identical | no |
| E7 | action directory retirement into the retired root | 1/4 | P1 with destination recheck | `publication.rs:157-173` | same + `windows_destination_path` `platform.rs:196` | as mac | no — spiked §4.2 |
| E8 | bounded/streamed leaf read through one retained handle | 2.1 | P3 + P2 | `platform.rs:159` no-follow | `platform.rs:165` share-delete | as mac | no |
| E9 | leaf flush | 2.1 | P2 | `sync_all` | `sync_all` | `sync_all` | no |
| E10 | leaf namespace barrier | 2.1 | P5 | `platform.rs:271` | `platform.rs:458` anchor | as mac | no |
| E11 | same-parent reobserve; two-sided durable-absence proof | 2.1 | P3 + P4 | identical | identical | identical | no |
| E12 | backend `publish_exact` over scheduled roles | 2.2 | P1 | `platform.rs:23/:78` | `platform.rs:48/:98` | as mac | no |
| E13 | backend `retire_exact` over scheduled roles | 2.2 | P1 | as E12 | as E12 | as E12 | no |
| E14 | backend `barrier` | 2.2 | P5 | `platform.rs:271` | `platform.rs:458` | as mac | no |
| E15 | managed component install (staged dir → final) | 2.3/3 | P1 + P2 + P3 | as E3 | as E3 | as E3 | no |
| E16 | managed ownership-marker retirement | 2.3/3 | P1 | as E13 | as E13 | as E13 | no |
| E17 | managed successor publish, prior-generation retirement, final reproof | 3 | P1 + P2 + P3 | as E3/E13 | as E3/E13 | as E3/E13 | no |
| E18 | legacy leaf edge `transition.rs:275` | 4.1 | P1 (replaces `platform::rename_relative` `platform.rs:276`) | `platform.rs:276` today | `platform.rs:324` today | `platform.rs:276` today | no |
| E19 | legacy leaf edge `transition.rs:368` | 4.1 | P1 (same) | as E18 | as E18 | as E18 | no |
| E20 | legacy leaf edge `residue.rs:393` | 4.1 | P1 (same) | as E18 | as E18 | as E18 | no |
| E21 | legacy leaf edge `residue.rs:486` | 4.1 | P1 (same) | as E18 | as E18 | as E18 | no |
| E22 | legacy Windows durability anchor retirement | 4.2 | P1 + P5 | n/a (`cfg(windows)`) | `platform.rs:380-604`; random scratch at `:420` is the R2 stop-clause violation this step removes | n/a | no |

### 4.4 Verdict, and the one in-seam extension Phase 1 owns

**New platform primitive required: NO.** Every edge E1-E22 names an admitted
primitive with a macOS arm and a Windows arm (Linux shares the macOS arm on
P1/P2/P3/P5 and has its own identity child). No platform spike is triggered, and
this freeze is not stopped (`GwzProcessOptimization.md` §3.1; brief's stop
condition).

**One bounded extension is required inside the sealed primitive, and Phase 1
owns it.** P1's *directory* source arm and its retirement *destination* arm are
currently typed to the catalog-bootstrap record rather than to publication in
general:

- `DirectoryInteriorRecheckV1 { durable_identity, expected: &CatalogBootstrapRecordV1 }`
  (`publication.rs:34-37`), verified through `interior::staging_plan(...) ==
  StagingPlanV1::Complete` (`publication.rs:145-155`);
- `DestinationRecheckV1::PreRetirementFinal` keyed to `interior::completed_record`
  and `InfrastructureSlotV1::CatalogBootstrapRetired` (`publication.rs:41-47`,
  `:157-173`).

An admission staging directory's interior is a resident
`ActionCapacityReservationV1`, not a `CatalogBootstrapRecordV1`, so E3's
*directory* form and E7's destination recheck need an **admission arm on those
two enums**, added inside `publication.rs` under the existing seam. That is a
protocol-shaped extension of an already-sealed primitive, not a new platform
capability and not a bypass: the acquisition window, identity compare, retained
handle, and no-replace rename are untouched, and the caller count stays at six
production sites until Phase 1 deliberately extends it (plan Step 1.2:
"extend the six-site pin at `interface_tests/capability_permit.rs:110` and
`CATALOG_PUBLICATION_CALL_COUNTS` deliberately, same-commit digest refresh").

This is contingency **(C-2)** at §2. It is recorded here rather than executed
here because Step 0.1 adds no physical code. The §4.2 spike deliberately
exercises the *regular-file* arm, which needs no extension, so the freeze rests
on executed evidence for the mechanics (open, identity compare, no-replace
publish, cross-parent retire, substitution rejection) and on a named, bounded
obligation for the one typed arm that does.

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
   arms. Neither option needs a primitive the other does not, so the 4.3 choice
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
implementation. The §4.4 extension is explicitly **not** such a need: it adds a
variant to two `pub(super)` in-memory recheck enums inside `publication.rs`,
neither of which is serialized or reachable from a durable-record root.

## 7. Line-reference drift corrected against the plan

The plan's citations were verified at `f36d20d`; this memo is verified at
`90d3f8a` (four trains later, plus the two docs trains since). Corrections:

| Plan citation | Actual on `90d3f8a` | Note |
| --- | --- | --- |
| `provider/publication.rs:48` for `publish_verified_no_replace` | `publication.rs:82` | `:48-80` is now the `PublicationSourceV1` impl block |
| `namespace/backend.rs:411-460` for the four defaults | `:411-451` defaults, helper at `:460-465` | matched closely; the range included the helper |
| `bootstrap/managed/owner.rs:16-32` | `:16-32` | correct |
| `namespace/mod.rs:305` for `NamespaceProtocol` | `:305` (`cfg(not(test))`); test builds use `test_support::NamespaceProtocol` re-exported at `:349` | the plan does not note the cfg split |
| `leaf.rs:74` for `LeafObserver` | `:74` | correct |
| "sole impls in `interface_tests/contracts.rs:183/:236`" | **no `LeafObserver` impls exist**; `:183` is `impl ExpectedLeafContent for Bytes`, `:236` is `impl NamespaceProtocol for RecordingNamespace` | material correction — see §3.3 |
| fault inventory "164 keys (`fault_expected_keys.rs:171`)" | **165 keys**, `EXPECTED_KEY_COUNT` at `:172`, list ends `:171` | +1 since the plan |
| "`catalog_bootstrap.*` = 24 keys" | **25** | +1 since the plan; RemPlan §10 says 21, older still |
| `cargo test --lib checked_artifact::` "244 passed" | **245 before this package**, 256 after | +1 since the plan |
| `platform.rs:372-505` Windows anchor, random scratch at `:409` | anchor block `:379-604`, `ANCHOR_PREFIX` `:383`, `prepare_private` `:386`, random scratch at **`:420`**, `private_barrier` `:458` | the adoption record predicted this drift ("may have drifted by the two added `windows_tests`") |
| `catalog/bootstrap.rs:145` `recover_or_create` | `:145` | correct |
| `interface_tests/capability_permit.rs:110` six-site pin | `:110` | correct |
| `transition.rs:275/:368`, `residue.rs:393/:486` | all four confirmed | correct |

## 8. What Step 0.2 landed (scaffolding inventory)

All tests pass at landing. They are freeze pins, not red tests: each fails only
if a later package widens or reshapes a frozen seam without saying so.

**D2 — signature stubs.**

- `namespace/backend.rs` — four defaults removed, methods required, sentinel
  helper deleted from the production trait file (net -1 line; §3.2).
- `namespace/test_support.rs` — blanket legacy adapter states the four
  operations, with the byte-identical refusal (+53).
- `admission/mod.rs` — new owner skeleton, frozen seam, typed refusal (+64 new).
- `checked_artifact/mod.rs` — `mod admission;` with the R2-D dead-code reason
  (+5).

**D3 — failing-test scaffolding (green-at-landing).**

- `interface_tests/r2d_seam_freeze.rs` (new, 239 lines, 6 tests):
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
- `interface_tests/fault_expected_keys.rs` (+190): the activation map
  (`FaultFamilyActivationV1` at `:185`, `FAULT_FAMILY_ACTIVATION` at `:191`,
  `FAULT_INJECTION_SOURCES` at `:248`) and three tests —
  `every_fault_family_declares_its_owning_activation_package` (`:280`) makes the
  map total against the key fixture and pins each family's key count;
  `reserved_fault_families_have_no_injection_sites_before_their_package`
  (`:320`) fails the moment a reserved family gains an injection site without
  its activation row being flipped in the same package;
  `only_the_runtime_and_catalog_bootstrap_families_are_executed_today` (`:349`)
  makes any activation-state change a deliberate, reviewed edit.
- `capability/pre_catalog/provider/tests_admission_spike.rs` (new, 192 lines,
  2 tests) — the Track-P spike of §4.2, wired at `provider.rs:199-200`.

**D4 — RemPlan §10 annotation.** A dated, append-only block at the end of
`GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` §10 recording that the activation
map is frozen here, with the per-phase assignments in one list. No existing line
was modified.

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

## 10. Verification appendix — commands executed (2026-08-16)

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

The two digest findings are expected and are refreshed by the lane owner in the
same commit as the code, per the adopted lane rule
(`CurrentProgramCheckpoint.md:186-204`): `checked_artifact/mod.rs` gained
`mod admission;`, and the `checked_artifact/capability/pre_catalog.rs` tree
gained `provider/tests_admission_spike.rs` plus its `#[cfg(test)]` module edge
in `provider.rs`. No other pinned file was touched, and the checker itself was
not edited.

Full-suite status is reported with the package (the whole `cargo test --lib`
run exceeds this host's foreground budget and was executed as an exact
partition of all 1 367 tests; see the package report).
