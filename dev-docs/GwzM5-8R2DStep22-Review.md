# R2-D Step 2.2 review — retained-handle namespace backend (State axis, round 1)

Date: 2026-08-22
Gate: single-axis adversarial review, **State-axis lead** (durable-edge
semantics), per the adopted plan's §4 Phase-2 gate note ("single-axis,
alternating Code/State per §4.2, **auto-escalation on any P0-P2**";
State-axis first recommended for 2.2/2.3). Round 1.

Object: the uncommitted working-tree Step 2.2 package over base `c13f773`
("Park R2-D Phase 1 Step 1.3") —

- **New production:**
  `gwz-core/src/checked_artifact/capability/pre_catalog/provider/namespace_mutation.rs`
  (393 lines, 287 code) and `gwz-core/src/checked_artifact/namespace/host.rs`
  (383 lines, 292 code).
- **New tests:** `namespace/tests_fault_matrix.rs` (523),
  `namespace/tests_backend.rs` (207).
- **Wiring edits:** `capability/pre_catalog.rs` (permit forwarder),
  `capability/pre_catalog/provider.rs` (mod + re-export),
  `capability/pre_catalog/provider/completed.rs` (the one no-follow hop),
  `catalog/bootstrap.rs` (opaque-catalog forwarder), `namespace/mod.rs`
  (host module + re-export).
- **Pin/fixture edits:** `interface_tests/capability_permit.rs` caller pin
  8→9; the `namespace` region of `interface_tests/fault_expected_keys.rs`.

Tree caution honoured: the working tree co-carries four other lanes
(Step 2.1 `leaf_observation.*`, the admission remediation edits in
`publication.rs`/`interior.rs`/`admission/*`/`platform.rs`, the D3 cursor and
M5b edits under `workspace_ops/merge/*`). Only the object above is judged;
base reads via `git -C gwz-core show c13f773:<path>`.

Contract register: `GwzM5-8R2DInterfaceFreeze.md` §3.2 (frozen
`RawNamespaceBackend`, four managed methods required and stated), §3.5
(`namespace.*` = 11 keys, owning package "R2-D Steps 2.2/2.3", RemPlan §10
same-package duty), §4.3 rows E12/E13/E14 (P1/P1/P5, **no recheck arm**),
§4.4 (Class-1 arm assignment; caller-pin and digest refresh "extended
deliberately in the same commit"); `GwzM5-8R2D-Plan.md` §4 Step 2.2
(sealed-primitive routing; provenance per `GwzM5-8R2C2PublicationAudit.md`
:39-44; "Activates `namespace.*` with rows");
`GwzM5-8R4bR2ConsumerCheckpoint.md` §8 :228-231.

## 0. Evidence executed for this review

All runs foreground on this tree, `CARGO_TARGET_DIR` per lane rule:

- `cargo test -p gwz-core --lib 'checked_artifact::namespace::'` — **10/10
  green** in 5.9s: both interruption/restart/convergence matrices (workspace +
  git-directory), both 12-round repeated-crash suites, all five
  `tests_backend.rs` refusal cases, plus the pre-existing
  `provider_compile` seam test.
- `cargo test -p gwz-core --lib 'checked_artifact::interface_tests::'` —
  **124/124 green**, including the 165-key census, the activation-literal
  test renamed to `only_the_families_with_executed_matrices_are_executed_today`,
  the injection-source completeness anchor, and the 9-caller permit pin.
- `cargo fmt -p gwz-core --check` — clean (exit 0).
- `scripts/checks/check_checked_artifact_boundaries.py` — **FAILED** (exit 1);
  attribution dissected in finding [P2-1].
- `git -C gwz-core diff d32b2c9 -- src/checked_artifact/namespace/backend.rs`
  — **empty**: the frozen trait file is byte-identical to the freeze object,
  discharging §3.2's "byte-unchanged" obligation structurally.
  `roles.rs` likewise unchanged vs base.

## 1. Attack item 1 — edge legality: crash windows and the retention discipline

**Pipeline as built** (`namespace_mutation.rs:239-294`, `execute_edge`): the
deterministic destination is checked absent (`require_absent`, :252, read-only
`symlink_metadata`), the source is re-observed **inside the strictest window**
and compared on identity + encoded identity + bytes (:256-265, refusal
"retained namespace source changed before the edge"), the move runs through
the sealed primitive (`publish_verified_no_replace(&self.handle, …,
PublicationSourceV1::regular_file(&source.encoded_identity, &source.bytes),
DestinationRecheckV1::None, …)`, :269-277), the published row is re-observed
(:281-289), and the one shared parent is flushed (`sync_directory_edge`,
:292). Source and destination live under the **same retained Dir**, so no
rename-domain crossing exists. The four `namespace.*` keys per edge bracket
exactly these boundaries (:78-94), matching the audit's pre-edge-in-strictest-
window + before/after form.

**Window enumeration** (each restart outcome single-valued per observed
state; the matrix executes each bracket on both target variants):

| Crash window | Durable state on restart | Resume (drive residency logic, `tests_fault_matrix.rs:305-320`) |
| --- | --- | --- |
| before/at `destination_reserve` | unchanged (checks are read-only) | re-enter the same edge |
| at `pre_publish_reobserve` / `pre_retire_reobserve` | unchanged | re-enter the same edge |
| inside primitive, pre-rename | unchanged (typed refusals fail closed) | re-enter |
| post-rename (`publish_no_replace` / `retire_exact`), pre-flush | row moved, flush pending | residency advances the sequence; the moved row cannot re-cross its rename (source gone) |
| at `published_reobserve` / `retired_reobserve` | as post-rename | as post-rename |
| after `sync_directory_edge` | settled | no-op (second `settle` asserts zero mutation) |

A power-loss revert of a not-yet-flushed rename re-presents the pre-edge
window, which is itself a matrix row (and 12-round covered by durable-state
class) — the same model scoping the admission family recorded at E4. The
process-stop model is stated in the module header ("a real process stop
across a real durable namespace edge").

**Retention discipline** (`host.rs:219-260`): an edge refuses before any
physical work when (a) no source is retained ("namespace edge has no retained
source capability", :230-234), (b) the proof differs from the held retention
in leaf, identity, handle, parent identity/path-profile, or kind ("namespace
source is not the capability this backend retained", :235-245), or (c) the
destination's reservation is not the admitted one (:246-250). Retention is
cleared only after the edge succeeds (:258).

**Interleavings attempted, all closed:**

- *Displaced proof:* retain A, retain B, publish(A) — refused by (b);
  executed as a test (`tests_backend.rs:94-102`).
- *Consumed proof replay:* publish(P), publish(P) — refused by (a); executed
  (`tests_backend.rs:106-114`).
- *Superseded source, no re-retain:* retain P, source replaced externally,
  publish(P) — host comparisons pass (both stale) but the strictest-window
  re-observation refuses on identity/bytes **before** the rename
  (`namespace_mutation.rs:256-265`); a swap after that check is caught by the
  sealed primitive's own in-window byte/identity re-verification
  (base `publication.rs` RegularFile arm: "publication source bytes
  changed"). Two independent layers; no mutation precedes either.
- *Racing occupant:* destination occupied after `require_absent` — the
  primitive's hardcoded `replace=false` makes the move atomic against the
  race; a pre-resident row is the typed refusal "namespace destination row is
  already occupied" with both rows proven untouched
  (`tests_backend.rs:124-160`).
- *Two backends over one admitted action* (ProviderBinding is
  reservation-digest-derived, not nonce'd): the second publish fails the
  occupied-destination check and the absent-source observation; no
  double-publish is constructible. Cross-action proofs fail
  `validate_operation` (binding mismatch, `namespace/mod.rs:293-305`).
- *Restart double-publish:* after a post-rename crash, re-retaining the
  scratch row fails (row gone, typed io refusal); the resume must proceed by
  residency, which the matrix's driver does and the settled-state second
  `settle` proves mutation-free.

**Verdict on item 1: sound.** No hole found in the discipline; every window's
restart outcome is single-valued.

## 2. Attack item 2 — provenance

Chain as built: `catalog/bootstrap.rs:166-172`
(`OpaqueRetainedCatalogV1::retain_action_namespace` → permit) →
`capability/pre_catalog.rs:143-149` (**`self.revalidate()?` immediately
before the hop**, same prologue as `execute_admission_edge` at :134-137) →
`completed.rs` `retain_action_namespace` (leaf name derived from the frozen
grammar `RootEntryNameV1::ActiveAction(admitted.reservation().action_digest()).name()`;
expected identity = `admitted.directory_identity()`) →
`namespace_mutation.rs:135-171`: **one** `open_dir_nofollow` hop from the
permit-retained completed-catalog Dir, identity-proved against the handoff
("admitted action directory identity changed", :146-151). This is exactly the
audit pattern (`GwzM5-8R2C2PublicationAudit.md` :39-44). Thereafter
`revalidate()` (:191-219) re-opens the name no-follow and requires both the
named identity and the retained handle's identity to equal the acquisition
identity, and the wrapper runs it **before every edge**
(`validate_operation`, `namespace/mod.rs:293-305`).

- **No path/Dir escape.** The two `Dir`s live in private fields of
  `RetainedActionNamespaceV1`, itself a private field of
  `HostActionNamespaceV1`; no accessor returns either. Consumers receive
  `ActionNamespaceHandleV1([u8; 32])` (action-digest-derived, `host.rs:53-58`),
  `DurableObjectIdentityV1`, and `CanonicalPathIdentityV1` — the last is an
  identity profile (frozen leaf component + parent durable identity + mode +
  rename domain, `namespace_mutation.rs:153-159`), not an ambient path.
  Proof types are mintable only via `BackendIssuer::new` / 
  `ProviderBinding::owner_new` / `ActionDestination::new`, all `pub(super)`
  inside `namespace` (`backend.rs:18/:174/:195`). The pre-existing
  `provider_compile` test and the byte-frozen `backend.rs` hold the seam.
- **Foreign-identity retain is genuinely tested.**
  `tests_backend.rs:193-207` builds **two real fixtures with the same
  reservation** (hence the same deterministic action leaf), guards the
  premise with `assert_ne!(own_identity, foreign_identity)`, retains over
  fixture A's catalog with a handoff carrying fixture B's identity, and
  requires the refusal "identity changed" — i.e., it drives the exact
  comparison at `namespace_mutation.rs:146-151`, not a stand-in.

**Verdict on item 2: sound.**

## 3. Attack item 3 — repeatability taxonomy vs actual durability

Their statement (`tests_fault_matrix.rs:431-438`): "A boundary is repeatable
only when the durable state its crash leaves resolves back to the same edge.
The four boundaries *after* a rename — `publish_no_replace`,
`published_reobserve`, `retire_exact` and `retired_reobserve` — are crossed
at most once per action by construction … The three crossed here are the
repeatable ones, one per Step 2.2 edge".

- **At-most-once for the four post-rename keys: verified**, in the stated
  process-stop model. After the rename the source row is gone; the resume
  advances by residency (`drive`, :305-320) and the moved row cannot re-cross
  its own rename. Under power loss an unflushed rename can revert, but that
  restart state is byte-identical to the covered pre-edge window, so the
  claim is model-scoped exactly as the admission family's E4 record scoped
  it. Convergence for all four is proven by the matrix on both variants.
- **`destination_reserve` and `retirement_reserve` are NOT unrepeatable.**
  Both fire after a read-only absence check (`require_absent`,
  `namespace_mutation.rs:252`, :322-331); a crash there mutates nothing and
  every restart re-enters the same edge and re-crosses the same boundary. The
  same holds for `pre_publish_reobserve` and `parent_revalidate`. By the
  comment's own criterion the physically repeatable set is **seven** keys
  ({source_retain, parent_revalidate, destination_reserve,
  pre_publish_reobserve, parent_barrier, retirement_reserve,
  pre_retire_reobserve}), not three. See finding [P3-1].
- **The evidence itself is not weakened.** The three 12-round keys were
  chosen one-per-edge where "a retry could be tempted to re-name", and their
  crash states cover the complete durable-state classes of all seven
  repeatable boundaries: {scratch resident} (`source_retain`; also the crash
  state of `parent_revalidate`(1st), `destination_reserve`,
  `pre_publish_reobserve`), {active resident, barrier unwitnessed}
  (`parent_barrier`), {active resident} (`pre_retire_reobserve`; also
  `retirement_reserve`). The four untested-repeatable keys are read-only
  boundaries; 12 rounds on them could not allocate a name or grow the slot
  set by construction. Slot-set stability and 12-round convergence are green
  on both variants.

**Verdict on item 3: taxonomy sentence miswritten ([P3-1]); durable evidence
sound and class-complete.**

## 4. Attack item 4 — the 11/11-executed ruling, adjudicated

Their ruling (`tests_fault_matrix.rs:72-77`, repeated in the fixture's
activation comment): "No key is reserved for Step 2.3. The four managed
operations are physically the same publish and retire edges plus a managed
*observation*, so when 2.3 makes them real they reuse these eleven boundaries
through the same shared edge helper; the observation boundaries they add
belong to the `managed_bootstrap.*` family (`fault_v1.rs:148-177`, Phase 3),
not to this one."

**Ruling: SOUND — the 11/11-now claim does not over-execute the family.**
Grounds:

1. **The frozen key vocabulary already assigns the 2.3 physical boundaries
   elsewhere.** `fault_v1.rs:148-177` (their citation is line-exact; 30 keys)
   contains `managed_bootstrap.staging_directory_publish`,
   `.final_directory_reopen`, `.final_directory_reobserve`,
   `.marker_retire`, `.marker_retired_reobserve`, `.prior_generation_retire`,
   `.successor_publish`, … — i.e., E15's arm-bearing staged-directory publish
   and E16's marker retirement crash boundaries are **vocabulary-assigned to
   `managed_bootstrap.*`**, not to `namespace.*`. No 2.3-specific
   `namespace.*` boundary exists in the frozen census; the 11 keys
   (`fault_v1.rs:110-120`) name precisely the scheduled-role
   publish/retire/barrier brackets this step executed.
2. **E16's leaf-shaped retirement can literally reuse the shared helper**
   (a marker retiring as a regular file crosses the same eleven announced
   sites); E15's directory publish is the Phase-2.3/3 arm-bearing route by
   §4.3/§4.4 and never belonged to this family's keys.
3. **The census discipline fails closed either way.** If 2.3 extends the
   shared helper, the existing `#[cfg(test)] fault_v1::hit` sites announce
   the new shapes under the same keys; if 2.3 adds a separate production
   file, the fixture's completeness anchor
   (`fault_expected_keys.rs`, `the_declared_injection_sources_are_every_
   production_source_holding_sites`) fails until the source is deliberately
   declared. `reconcile_executed_keys` (`tests_fault_matrix.rs:344-359`)
   additionally pins matrix-rows ↔ family-inventory set-equality.
4. §3.5's owning-package column ("Steps 2.2/2.3") names the step **range**
   permitted to execute the family; 2.2 ⊂ {2.2, 2.3} executing all eleven
   does not contradict it, and the counts stay 11/165 with no key minted.

**Routed (not a defect of this object):** two obligations land on **Step
2.3's gate**: (i) if 2.3's managed operations physically cross
`managed_bootstrap.*` boundaries (`staging_directory_publish`,
`marker_retire`, the observation reopen/reobserve keys), RemPlan §10's
same-package duty follows the edge — 2.3 must activate that subset with rows
and a deliberate §3.5 map annotation (the map currently reserves the family
for Phase 3), or demonstrate the crossed edges are not durable edges;
(ii) 2.3 must add its own matrix rows for any new shape it pushes through the
shared helper (directory-source windows under the same keys). Separately, the
**Phase-2 settle** should annotate §3.5's frozen row to record that 2.2 alone
executed the family, so the map and the fixture literal stay reconciled.

## 5. Attack item 5 — the matrix restart token

The deviation is declared in the module header (`tests_fault_matrix.rs:20-29`)
and is **sound for this step**, on the same shape as the settled 2.1
adjudication:

- The rebuilt element is the **authority token only**. `handoff()` (:248-262)
  reconstructs `AdmittedActionV1` through `admit_observed_action`, which is
  `#[cfg(test)]`-gated at both the module and the re-export
  (`protocol/admission.rs:16-20`) — compile-time absent from production. The
  production issuer remains `CatalogAdmissionOwnerV1::admit`
  (`protocol/admission/owner.rs:113-127`), which requires an idle admission,
  no staging, and an **exact** final directory.
- The justification is a real §7-exactness property, not convenience: after
  the first published row the action directory is no longer exact, which is
  "precisely the state a second admission must refuse" — re-running
  `resume_or_admit` per restart would demand the durable handoff resume that
  plan §4 assigns to **Step 3.3**.
- Everything durable is re-acquired for real per restart: the target lease,
  `recover_or_create` through the sole sealed owner, the permit's
  `revalidate()`, and the identity-proved no-follow hop — and the identity
  fed to the rebuilt token is the original **real** admission's
  (`Fixture::admit` drives `resume_or_admit` once, :174-186), with
  `retain_action_namespace` failing closed on any mismatch
  (proven by `tests_backend.rs:193-207`).
- Consequence: the crash evidence over the **namespace edges** — this step's
  object — is not hollowed; what is deliberately not evidenced here is the
  resume of the handoff itself, which is Step 3.3's and must arrive with its
  own resume-path rows. Same ruling as 2.1's deviation.

## 6. Attack item 6 — ownership deviations

- **The four wiring files mirror the admission forwarder pattern exactly.**
  `pre_catalog.rs:143-149`: `self.revalidate()?; self.completed.…` — the
  literal shape of `execute_admission_edge` (:134-137), same
  `pub(in crate::checked_artifact)` visibility. `catalog/bootstrap.rs:166-172`:
  plain forward through the permit, exactly as its `execute_admission_edge`
  forwarder; the catalog stays opaque. `completed.rs`: the hop is
  `pub(in …::pre_catalog)` (owner-private), derives the leaf from the frozen
  grammar, returns the retained capability only. `provider.rs`: module
  declaration plus a re-export whose doc restates amendment §7 :576-577; no
  raw surface exported. **No authority leak found**: the returned type
  carries no path and no Dir accessor, and every proof constructor stays
  namespace-private.
- **Caller pin 8→9** (`capability_permit.rs`): extended deliberately in the
  object with the §4.4 citation and the one-file/one-site rationale;
  `assert_eq!(callers.matches("publish_verified_no_replace(").count(), 9)`
  green; the no-raw-rename asserts hold over the enlarged caller set. The
  **Python-side twin of this pin was missed** — see [P2-1].
- **The 2.3 stubs** (four, not two: the install pair and the retirement pair,
  `host.rs:319-359`): each is an unconditional
  `CheckedFsError::ambiguous("managed namespace operation", "managed
  component operations land in R2-D step 2.3")` before touching anything —
  genuinely fail-closed, no partial mutation possible. Production
  reachability today: **none** — a tree-wide sweep finds zero production
  consumers of `HostActionNamespaceV1`/`retain_action_namespace` outside the
  namespace module and its tests (`namespace/mod.rs` carries the explicit
  `#[allow(dead_code, reason = "… plan §4 Step 3.3 wires its production
  consumer")]`), and the plan's dependency order (3.1/3.3 depend on 2.2/2.3)
  replaces the stubs before any consumer exists. Even if reached, the failure
  is typed and safe. ConsumerCheckpoint §8 :230-231 stays discharged: the
  trait has no default bodies (frozen §3.2), and no production path reaches
  an unavailable managed operation.

## 7. Attack item 7 — LOC

Independent count (non-blank, non-comment): `namespace_mutation.rs` **287** +
`host.rs` **292** + wiring adds across the four shared files and
`namespace/mod.rs` **~52** ≈ **631** — consistent with the lane's ~629
(±ambiguity in counting attribute lines). Against the **aspirational** <500:
~26% over, recorded not excused. Both structural drivers verified:

1. **The `pub(super)` seam genuinely forces the two-owner split.** The sealed
   primitive surface (`publish_verified_no_replace`, `sync_directory_edge`,
   `encode_identity`, `HostPlatform`) is provider-private, so the physical
   half must live in `provider/namespace_mutation.rs`; the proof issuers
   (`BackendIssuer::new`, `ProviderBinding::owner_new`,
   `ActionDestination::new`) are `pub(super)` inside `namespace`, so the
   authority half must live in `namespace/host.rs`. Widening either
   visibility to merge the halves would be a seam change (freeze §3.2 /
   amendment §7 owner rules) that a step lane may not take — confirmed as
   drafted.
2. The freeze itself imposes the second block: the four managed operations
   must be **stated** on the production impl (~45 lines of required
   signatures/stubs), and the role/barrier-slot wrapper surface
   (`host.rs:94-161`) is the R1-shaped consumer API the plan names.

## Findings

### [P2-1] The freeze-mandated checker companion of the caller-pin extension is missing; the enforced seam gate fails attributably to this object

- **Evidence.** `scripts/checks/check_checked_artifact_boundaries.py` (run
  with Python 3.13) fails: "`catalog publication seam changed: all six
  physical moves must use the single source-associated publication
  primitive`". Root cause: `CATALOG_PUBLICATION_CALL_COUNTS`
  (`check_checked_artifact_boundaries.py:663-667`) still lists only
  `mutation.rs: 1`, `directory_mutation.rs: 5`, `admission_mutation.rs: 2`,
  while the object adds a fourth production caller
  (`namespace_mutation.rs:269`, one site). The check computes
  `publication_callers` from the production tree and requires set-equality
  with the table (:824-864), so the failure is deterministic and solely this
  package's: the co-resident 2.1 observer has zero
  `publish_verified_no_replace` references. Additionally the
  `PROTECTED_SOURCE_TREE_DIGESTS` pin for `checked_artifact/catalog.rs`
  (:155) is un-refreshed while the only in-tree change under that root is
  this object's `bootstrap.rs` forwarder ("`protected source tree changed:
  checked_artifact/catalog.rs`").
- **Contract.** Freeze §4.4 Class 1: "caller-pin and digest refresh extended
  deliberately **in the same commit**"; the plan's pin discipline names
  `CATALOG_PUBLICATION_CALL_COUNTS` explicitly alongside the
  `capability_permit.rs` pin. The lane extended the Rust-side pin (8→9,
  green) but not the machine-enforced Python-side inventory, and the package
  as parked leaves the enforced seam gate red with two failure lines
  attributable to this object. (The remaining checker lines — `platform.rs`,
  `workspace_ops/*`, and the shared `pre_catalog.rs` tree digest, which
  spans the co-resident 2.1 files — belong to the other lanes and serialize
  at their commits.)
- **Impact.** No durable-state or physical-seam defect: no raw rename exists
  in the object (asserted twice), the single primitive call site is real,
  and the Rust pin holds. The defect is gate integrity: landing this package
  without the companion edit either blocks the serialized commit train red
  or normalizes a red checker across five lanes — exactly the alarm-fatigue
  state the completeness anchor was built to prevent.
- **Minimal remediation** (mechanical, in the package's own landing commit):
  add `"checked_artifact/capability/pre_catalog/provider/namespace_mutation.rs": 1,`
  to `CATALOG_PUBLICATION_CALL_COUNTS`; refresh the
  `checked_artifact/catalog.rs` tree digest; refresh the shared
  `checked_artifact/capability/pre_catalog.rs` tree digest in commit-serial
  order with the co-resident lanes; re-run the checker to green before park.

### [P3-1] The repeatability-taxonomy comment misclassifies four boundaries by implication

- **Evidence.** `tests_fault_matrix.rs:437`: "The three crossed here are the
  repeatable ones, one per Step 2.2 edge". By the comment's own criterion
  (:431-432) the physically repeatable set has seven members —
  `parent_revalidate`, `destination_reserve`, `pre_publish_reobserve`, and
  `retirement_reserve` are read-only boundaries whose crash states resolve
  back to the same edge (`namespace_mutation.rs:252-267`, :191-219).
- **Impact.** None on the executed evidence (the durable-state classes of all
  seven are covered by the three 12-round keys; the four omitted are
  read-only and cannot allocate names or grow slots), but the sentence as
  written will mislead the 2.3/3.3 reader who inherits this taxonomy.
- **Minimal remediation.** One-sentence comment fix: "three representatives,
  one per edge, chosen where a retry could be tempted to re-name; the four
  remaining pre-rename boundaries are equally repeatable but crash into the
  same durable states as these."

### [P3-2] The resume path never completes a pending parent flush for an already-crossed edge

- **Evidence.** A crash at `publish_no_replace`/`published_reobserve`/
  `retire_exact`/`retired_reobserve` stops after the rename but before
  `sync_directory_edge` (`namespace_mutation.rs:279-292`). The restart
  advances by residency (`tests_fault_matrix.rs:305-320`) — for the retire
  windows it returns at :305-307 — so no code path re-issues the parent
  flush for the completed move; durability rides on the OS or a later edge.
- **Impact.** Safe: on power loss the unflushed rename reverts to the covered
  pre-edge window and the next restart re-crosses and flushes; each observed
  state has a single-valued resume; this matches the E4-record doctrine. But
  the decision "does a coordinator resume issue a completing barrier when it
  detects an already-crossed edge?" is exactly Step 3.3's restart-resolution
  surface and should be made deliberately there, not inherited silently.
- **Minimal remediation.** Route to Step 3.3 (carry in the checkpoint's
  step-2.2 row): the coordinator's resume design must state whether a
  detected already-crossed namespace edge gets a completing
  `barrier`/parent-flush before the action proceeds.

## Scope and non-findings

- E12/E13's **no-recheck-arm refusal is fail-closed, not lossy**: a directory
  source is refused read-only at observation (`observe_regular_file`,
  `namespace_mutation.rs:344-352`, "namespace object is not a canonical
  regular file") before any physical work, at both retain and the pre-edge
  window; executed as a real test with a real directory planted in a
  scheduled slot (`tests_backend.rs:46-66`). The record-bound read caps at
  the frozen kind bound, never the payload's length (:360-378).
- The generic wrapper (`namespace/mod.rs` `publish_no_replace`) does not gate
  the source leaf against the destination's role; the evidence-typed
  operations do (`operations.rs:21-43` requires the barrier-intent scratch
  leaf for publish, the active leaf for retire). Pre-existing R1 surface
  shape, in-crate-only exposure, discipline present at the consumer-facing
  layer — no finding; 3.3's coordinator should consume the evidence-typed
  surface.
- `barrier` (E14) routes through the admitted P5 family entry
  (`platform::private_barrier`, base `platform.rs:271`), ordinal-gated by
  schedule (`scheduled_barrier`), binding-checked at three layers; unscheduled
  ordinals refuse (`tests_backend.rs:165-187`).
- The fixture edits keep the frozen census: 165 keys, no key minted, 11/11
  set-equality double-enforced; `FAULT_INJECTION_SOURCES` declares
  `namespace_mutation.rs` with the driver/mutation split rationale mirroring
  `admission/driver.rs`; the executed-set literal is a two-place deliberate
  edit as designed.
- Windows execution of these suites rides the program-level Windows matrix
  runner, per the Step 1.3 precedent; the object's physical arms (P1/P5 win
  columns) are the already-spiked shared primitives.
- Not judged (other lanes, co-resident): `leaf_observation.rs` + its tests
  and fixture region, `publication.rs`/`interior.rs` admission-budget edits,
  `platform.rs`, `admission/*`, `workspace_ops/merge/*`,
  `protocol/admission.rs` deletion hunk.

## Verdict

**GO** — zero P0, zero P1; one **P2** and two P3.

**Escalation: TRIGGERED.** Per the adopted plan §4 Phase-2 gate rule
("auto-escalation on any P0-P2"), finding [P2-1] escalates this package to
the mandated escalation pass; [P2-1]'s remediation is mechanical (checker
inventory + two digest refreshes, same-commit) and does not touch the
physical seam, the evidence, or any frozen surface.
