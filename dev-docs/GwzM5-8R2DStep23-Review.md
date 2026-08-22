# R2-D Step 2.3 — the four managed backend operations: single-axis State review

Date: 2026-08-22. **Single-axis State-lead** (adopted plan's Phase-2 note —
durable-edge semantics; `GwzM5-8R2DInterfaceFreeze.md` §9: "Everything else is
**single-axis with automatic escalation on any P0/P1/P2**"). Round 1 of a
two-round cap. Escalation on any P0/P1/P2 is RECORDED in the verdict line for
the lane owner to route; no second axis is assumed.

Object: the gwz-core working tree over origin `6c7c8f3` ("Open interior slot
directories with delete sharing on Windows (matrix os-32 residue)"). Repo HEAD
at review time `d2cf755`. The Step-2.3 file set was last written at
**17:13:20 AEST**; every gate below ran after that instant, so the gates cover
the object as reviewed.

**Tree caution, honoured.** Two other lanes' uncommitted files coexist in this
tree and are **attributed, not judged**: the Step-2.4 lane
(`provider/authority_record_binding.rs`, `provider/tests_authority_record*.rs`,
the `record` rows in `interface_tests/fault_expected_keys.rs`, the
`authority_record_binding.rs: 2` entry in
`scripts/checks/check_checked_artifact_boundaries.py`) and the D3 lane under
`src/workspace_ops/`. The 2.4 lane was **actively editing during this review**
— `authority_record_binding.rs` was rewritten at 17:29:07, 25 seconds before a
gate call — which is recorded in §9 because it moved one gate from green to red
mid-review.

---

## 1. Object inventory, as counted

Independent count, non-blank and non-comment (`//`, `///`, `//!` all excluded):

| File | Disposition | Code lines |
| --- | --- | --- |
| `capability/pre_catalog/provider/managed_mutation.rs` | NEW (579 raw) | **433** |
| `namespace/host.rs` | modified (+319/−19 raw) | **+266** |
| `capability/pre_catalog/provider/interior.rs` | modified | **+78** |
| `namespace/managed.rs` | modified | **+38** |
| `capability/pre_catalog/provider.rs` | modified | **+17** |
| `capability/pre_catalog.rs` | modified | **+10** |
| `capability/pre_catalog/provider/publication.rs` | modified | **+12** |
| `capability/pre_catalog/provider/namespace_mutation.rs` | modified | **+3** |
| `namespace/mod.rs` | modified | **+4** |
| `interface_tests/capability_permit.rs` | modified | **+3** |
| **Production/wiring subtotal** | | **864** |
| `namespace/tests_managed.rs` | NEW (438 raw) | 317 |
| `namespace/tests_managed_matrix.rs` | NEW (232 raw) | 141 |
| `interface_tests/fault_expected_keys.rs` | modified (2.3 half) | +104 |
| `namespace/tests_fault_matrix.rs` | modified | +4 |
| **Test subtotal** | | **566** |
| **Total** | | **1 430** |

The lane's "~865" is the **production/wiring** slice and my count of **864**
confirms it to within one line (the residual is attribute-line counting, the
same ±ambiguity the Step-2.2 review recorded at its ~629/631). See §8 for the
structural argument.

---

## 2. Contract check — freeze §3.2, the four frozen signatures

Verified against `GwzM5-8R2DInterfaceFreeze.md` §3.2 (:273-299) line by line.
All four methods keep their frozen shapes exactly:

- `install_managed_component(&mut self, source: &RetainedNamespaceObject<..>,
  destination: &ActionDestination, request: &ManagedInstallRequestV1)
  -> Result<ManagedInstallObservationV1, CheckedFsError>` — `host.rs:567`;
- `observe_installed_managed_component(&mut self, request:
  &ManagedInstallRequestV1) -> Result<ManagedInstallObservationV1, _>` —
  `host.rs:601`;
- `retire_managed_marker(&mut self, source, destination, request:
  &ManagedMarkerRetirementRequestV1)
  -> Result<ManagedMarkerRetirementObservationV1, _>` — `host.rs:615`;
- `observe_retired_managed_marker(&mut self, request)
  -> Result<ManagedMarkerRetirementObservationV1, _>` — `host.rs:647`.

**No reshape.** The only textual delta on each is `_source`→`source`,
`_destination`→`destination`, `_request`→`request`; parameter order, types,
lifetimes, receiver mutability and return types are byte-identical to the
freeze. No parameter was added, and in particular the intent was **not**
threaded across the seam (see §6).

**The sentinel is gone.** `HostActionNamespaceV1::managed_unavailable` is
deleted; nothing in `src/checked_artifact/namespace/host.rs` now produces
`CheckedFsError::ambiguous("managed namespace operation", "managed component
operations land in R2-D step 2.3")`. The byte-identical twin in
`namespace/test_support.rs:243-248` (the blanket legacy adapter's refusal, the
one the freeze §3.2 table relocated there) is untouched and still stands, which
is correct: it is the test-only adapter's sentinel, not the production one.
`RecordingNamespaceBackend` and `ProductionShapedBackend` are unchanged, so the
freeze's "complete implementation set is three impls" inventory still holds and
`interface_tests/r2d_seam_freeze.rs` passes (§9).

This discharges ConsumerCheckpoint §8 (:230-231) — "Default 'unavailable'
managed operations are not reachable in production" — for the *production*
backend, which is what §3.2 promised structurally.

---

## 3. Headline adjudication 1 — E16 resolves to **no destination arm**: RULED SOUND

### 3.1 The shape claim, verified in the frozen protocol

The implementer's claim is that §4.3 row E16's conditional ("needs a
destination arm **only if the marker retires as a directory**") has a false
antecedent, because the marker is the frozen regular-file leaf. Both citations
are exact and the property is enforced at three independent layers:

1. **The name is a compile-time constant with no caller input.**
   `protocol/managed_bootstrap_record.rs:508`:
   ```rust
   pub(in crate::checked_artifact) fn managed_marker_name() -> AsciiComponent {
       decode_ascii(b"gwz-bootstrap-owner-v1").expect("fixed managed marker name is valid")
   }
   ```
   It is also what the durable component record itself carries
   (`marker_name: managed_marker_name()`, same file `:216`).

2. **The kind is pinned by the R1 role validator that predates this object.**
   `namespace/operations.rs:361-362`, inside `validate_marker_source_role`:
   ```rust
       || source.leaf() != &crate::checked_artifact::protocol::managed_marker_name()
       || source.kind() != NamespaceObjectKind::RegularFile
   ```
   returning `binding_error("bootstrap marker source role mismatch")`. This is
   on the **sole** production entry to E16 — `retire_bootstrap_marker` calls it
   at `operations.rs:214`, after `validate_operation` and **before**
   `backend.retire_managed_marker` at `:222-224`. The citation `:361-362` is
   line-exact.

3. **The object re-proves it twice more.**
   `host.rs:625-629` re-checks the capability's kind through
   `take_managed_source(source, &managed_marker_name(),
   NamespaceObjectKind::RegularFile)`; and the provider refuses a non-file on
   disk at `managed_mutation.rs:531-536`
   (`"managed object is not a canonical regular file"`).

So a directory can never occupy the marker role, on any path.

### 3.2 Does the annotation genuinely resolve to no-arm?

Yes, and the resolution is grounded in §4.4's own stated mechanism rather than
only in §4.3's wording:

- §4.4 gives the *reason* an arm is forced: "`PublicationSourceV1::Directory`
  has no 'no interior recheck' form" (:773-775). E16's source is a regular
  file, so the primitive takes the `PublicationSourceV1::regular_file`
  path (`managed_mutation.rs:388`), which verifies by retained-handle identity
  **and byte-exact content** (`publication.rs:157-179`) and needs no arm by
  construction. The forcing condition is absent.
- A `DestinationRecheckV1` arm re-verifies the *destination directory's
  interior grammar*; the only existing arm, `PreRetirementFinal`, exists
  because E7's destination is the **catalog root**, whose grammar gates the
  edge. E16's destination is the **action directory**, whose interior is
  governed by the scheduled slot vocabulary and not by any record type the
  recheck surface can express. There is no in-vocabulary destination
  expectation to assert.
- What `PreRetirementFinal` supplies for E7 is supplied here by three other
  in-seam facts, all verified present: `require_absent_in(action.handle(),
  &destination_name, label)` before the edge
  (`managed_mutation.rs:370`, :501-514); the no-replace rename itself; and
  `revalidate_action_directory` — reached on every managed operation through
  `operations.rs` `validate_operation` (`namespace/mod.rs:305-308` →
  `host.rs:523-529` → `RetainedActionNamespaceV1::revalidate`), which re-proves
  the destination directory's identity *and* reservation immediately before the
  edge.

**Ruling: the E16 annotation genuinely resolves to no destination arm.** The
residual is a map-reconciliation item only, filed as [P3-4]: §4.4's arm table
(:771) still lists "managed destination (marker/generation retirement) | E16,
E17"; on this tree that row is driven by **E17 alone**, and it survives for
Phase 3 on that basis.

---

## 4. Headline adjudication 2 — the 8-executed / 22-reserved `managed_bootstrap.*` split

The Step-2.2 review routed this obligation here verbatim
(`GwzM5-8R2DStep22-Review.md`, "Routed (not a defect of this object)"). I
adjudicated it key by key against `fault_v1.rs:148-177` (30 keys, line-exact).

### 4.1 Each of the 8 is genuinely crossed by the four operations

Every one of the eight has a real production site inside
`capability/pre_catalog/provider/managed_mutation.rs`, and I traced the crossing
in a virgin drive:

| Key | Site | Crossed by |
| --- | --- | --- |
| `parent_revalidate` | `:204-207` in `revalidate()` | **all four** ops, via `HostActionNamespaceV1::managed()` (`host.rs:419-437`) |
| `staging_directory_publish` | `:300-303` | `install_managed_component` (E15's rename) |
| `final_directory_reopen` | `:466-469` in `open_installed()` | all four (every route to the component) |
| `final_directory_reobserve` | `:343-346` in `observe_installed()` | `install_managed_component`, `observe_installed_managed_component` |
| `component_reobserve` | `:320-323` in `observe_installed_on_restart()` | `observe_installed_managed_component` (the restart entry) |
| `marker_retire` | `:392-395` | `retire_managed_marker` (E16's rename) |
| `marker_retired_reobserve` | `:415-418` | `retire_managed_marker`, `observe_retired_managed_marker` |
| `final_identity_reobserve` | `:421-424` | `retire_managed_marker`, `observe_retired_managed_marker` |

All eight sites live in one production file. `namespace/host.rs` holds **zero**
`CheckedArtifactFaultKeyV1` references (verified: `grep -c` returns 0), which is
the correct posture — and is also what makes [P3-1] a real inconsistency.

Note also that the managed operations reuse an already-executed boundary rather
than minting one: `validate_operation` → `revalidate_action_directory` crosses
`namespace.parent_revalidate` (`namespace_mutation.rs:226-227`), a key Step 2.2
already executed with rows. No key was minted; `EXPECTED_KEY_COUNT` stays 165.

### 4.2 Each of the 22 genuinely belongs to Phase 3's writer/intent/successor half

Enumerated exhaustively against `fault_v1.rs:148-177`:

- **Intent lifecycle (7)** — `preflight`, `initial_intent_scratch_create`,
  `initial_intent_scratch_write`, `initial_intent_scratch_flush`,
  `initial_intent_publish`, `initial_intent_reobserve`, `plan_complete`. Step
  2.3 writes no intent record and runs no preflight; the intent arrives
  pre-built through `ManagedParentBootstrapIntentV1`.
- **Staged-component and marker construction (5)** —
  `staging_directory_create`, `ownership_marker_create`,
  `ownership_marker_write`, `ownership_marker_flush`,
  `staging_directory_flush`. Step 2.3 **consumes** a staged directory it never
  creates; the fixture places it explicitly and says so
  (`tests_managed.rs:119-123`: "This is Phase 3.1's writer half").
- **Durable successor (4)** — `successor_scratch_create`,
  `successor_scratch_write`, `successor_scratch_flush`,
  `successor_scratch_reobserve`. Not written here.
- **Successor publish / prior-generation retirement (4)** —
  `prior_generation_retire`, `prior_generation_reobserve`,
  `successor_publish`, `successor_reobserve`. These are freeze §4.3 row **E17**
  (Phase 3), not E15/E16.
- **Final intent retirement (2)** — `final_intent_retire`,
  `final_intent_retired_reobserve`. Not written here.

22 accounted for, none of them crossed by any of the four operations. **The
split is correct, exhaustive, and edge-faithful.**

### 4.3 The RemPlan §10 duty — sites and rows discharged, the map annotation is not

RemPlan §10 (:1033-1035) requires "injection sites **and matrix rows** in the
same package that converts its edges". Sites: yes (8, above). Rows: yes — the
8×2 matrix plus 3×2 repeated rows in `tests_managed_matrix.rs`, executed green
(§9). The Step-2.2 review's routed item (i) additionally required "a deliberate
§3.5 map annotation"; that half is **not filed** — see [P2-1]. Routed item (ii)
("matrix rows for any new shape pushed through the shared helper") is
discharged by non-applicability: the managed edges run through a separate
production file with `managed_bootstrap.*` keys, not through
`namespace_mutation.rs`'s shared helper.

---

## 5. Attack item 1 — crash windows, retained handles, and the restart taxonomy

### 5.1 E15 (staged dir → final), cross-checked by the marker-exact interior observation

The composition is `require_absent` → sealed publish with the Class-1 managed
source-interior arm → `staging_directory_publish` → parent flush → durable
reobservation (`managed_mutation.rs:283-305`). The interior is proved **three
times**: at retention (`:246`), inside the primitive's acquisition window
(`publication.rs:206-208` → `interior.rs:624`), and post-edge (`:337`). The
acquisition-window proof is the arm proper and is exercised adversarially by
`managed_staged_interior_drift_is_refused_*` (`tests_managed.rs:378-398`).

Windows walked, all convergent:

- *before the rename* — staged resident, final absent; restart re-stages
  idempotently (`tests_managed.rs:207-209`, `:130` guards on `exists()`) and
  re-enters E15 on the **same deterministic staging name**
  (`intent.components()[0].staging_name()`), so no retry name is allocated
  (RemPlan-4 §4 R2 stop clause);
- *rename landed, parent flush lost* — the rename is a single intra-directory
  transaction; either state is a covered window, and no crash state holds both
  names because the publish is no-replace over a `require_absent`ed
  destination;
- *after flush, before the observation* — restart takes
  `recover_installed_bootstrap_component` and reproduces identical evidence;
  proved directly by `managed_install_restart_reobserves_*`
  (`tests_managed.rs:310-332`, asserting identity, marker identity and path
  equality against the forward run).

The interior bound is genuinely bounded: `MAX_MANAGED_COMPONENT_ENTRIES = 2`
(`interior.rs:599`) with the refusal at `:646`, plus `CatalogNameBudgetV1`
charging every child name. Marker-plus-one-extra is a *typed* refusal via
`extra_children`; marker-plus-two is the unsupported bound. Correct for every
shape this owner accepts. **Phase-3 caution, not a finding:** that bound is
exact for a *bootstrapping* component; a component that has since been
populated by writers would exceed it, and Phase 3.1's `observe_preflight` must
keep such components out of plans (ConsumerCheckpoint §9: plan only for a
missing suffix).

### 5.2 E16 — the cross-directory retire, and the share-delete doctrine

The sibling owner's new handle **does stay sealed**. `RetainedActionNamespaceV1::handle()`
is `pub(super)` (`namespace_mutation.rs:186-194`), i.e. visible only inside
`capability::pre_catalog::provider`; `managed_mutation` is a sibling module of
that owner, not a consumer. Nothing widened: the `Dir` is not re-exported by
`provider.rs` or `pre_catalog.rs`, and `pre_catalog.rs`'s new exports are
capabilities and fact structs only (`ManagedInstalledFactsV1`,
`ManagedRetiredFactsV1`, `ObservedManagedObjectV1`, `RetainedManagedParentV1`,
`retain_managed_parent`). The "no real `Dir` leaves the provider owner" rule is
unweakened. **Verified sound.**

Retained handles across the edge: the marker's own read handle is opened and
**dropped** inside `observe_regular_file` (`:539-541`) before the primitive's
DELETE-access reopen, which is the documented discipline
(`ObservedManagedObjectV1`, `:54-59`). That is the handle that actually
mattered, and it is handled correctly. The two `open_dir_share_delete` claims
at `:242-244` and `:462-465` are *conservative and behaviour-neutral* — on
macOS/Linux the helper is byte-identically `open_dir_nofollow` — but their
stated reasons do not survive inspection, filed as [P3-2].

Crash windows for E16, walked:

- *before the rename* — marker resident, retirement row absent; restart
  re-observes the installed component (marker still inside, so the interior
  proof still passes) and re-enters E16 on the schedule-derived
  `RetiredBootstrapMarker` row; no name allocated;
- *rename landed, either or both parent flushes lost* — the retirement row is
  the settling state; a restart that sees it short-circuits
  (`tests_managed.rs:212-214`), and a restart that does not sees the pre-rename
  window;
- *between the two flushes* (`:396` source parent, `:397` destination parent) —
  see [P2-3]: on the closed support table this is the same covered window, but
  the argument is nowhere written and the restart path has **no** recovery from
  a split.

The fault-key placement is right for durability: `marker_retire` fires
*after* the publish and *before* both flushes (`:392-397`), so the
"renamed-but-unflushed" window is a matrix row rather than an unwitnessed gap.
Same for `staging_directory_publish` on E15 (`:300-304`). This is a genuine
improvement over the 2.2 shape, where `faults[3]` fires before the single flush.

### 5.3 Single-valued restarts — the 8×2 matrix and the 3×2 repeated rows

The 8×2 matrix is sound: every activated key gets an interruption, a restart to
the settled census, and a second settle proving the resume mutated nothing
(`tests_managed_matrix.rs:124-136`), over **both** directories the sequence
writes into (`tests_managed.rs:157-166`) — which is the right census, because
this is the lane's first two-directory edge. `reconcile_executed_keys`
(`:66-86`) pins matrix rows ↔ the activated subset, and
`only_the_families_with_executed_matrices_are_executed_today` (`:695-709`) pins
the subset itself in a second place. Both fail closed.

**The read-only-boundary taxonomy does not hold as written** — [P2-2]. The
excluded five are *not* "at or after edge E16's rename": `staging_directory_publish`
is E15's rename and `final_directory_reobserve` is E15's post-edge proof. More
than a wording slip, `final_directory_reobserve` satisfies the file's own
inclusion criterion and is nonetheless excluded.

---

## 6. Attack item 3 — the intent/cursor storage in `ManagedMarkerRetirementRequestV1`

**No authority leaks.** The new fields (`managed.rs:47-48`) are private to
`managed.rs`; `ManagedMarkerRetirementRequestV1` derives nothing (no `Debug`,
no `Clone` — the `#[derive(Clone, Debug, Eq, PartialEq)]` at `:15` is on
`ManagedBindingV1`), and there is no accessor returning the intent. `host.rs`,
though a sibling module, cannot read the field. The only consumer is
`bind_marker_bytes` (`:221-232`).

**The binding is doubly validated, and against durable bytes.** The bytes come
from the *retired row on disk* (`observe_regular_file` on
`action.handle()`, `managed_mutation.rs:410-414`), bounded twice — once by
`ProtocolRecordKindV1::Marker.max_bytes()` at read (`:543-561`) and again by
`read_bounded_record_inner` inside
`read_and_bind_ownership_marker`. That function
(`protocol/ownership_marker_record.rs:244-266`) rejects any marker that is not
this intent's component; and `complete` then re-checks
`marker.marker_id() != self.expected_marker_id`, where the expectation was
derived from `intent.components()[intent.cursor()]` at bind time
(`managed.rs:176-181`). A substituted or replayed marker is refused twice.
`local_component: intent.cursor()` is consistent with the pre-existing
`bind` body, not a new indexing decision. **Sound.**

**On "the alternative really was a seam change" — partly.** Moving the decode
into the provider *would* be a seam change: `managed_mutation` receives only
`AsciiComponent`s and the retained action capability, so binding there needs
either the request or the intent to cross into the provider owner (amendment §7
owner cohesion) or a widened `retire_marker` signature. But a third option
existed entirely inside the `namespace` owner — have `complete` take the raw
durable bytes and bind at `retired_marker_evidence`, which already holds the
intent (`operations.rs:262-274`). That is an intra-owner change, not a seam
change. It is nevertheless **strictly worse**: it would put unvalidated durable
bytes inside the sealed evidence constructor, which is precisely the surface
whose job is to compare two independently established facts. So the choice
stands on its merits; only the "forced" framing overstates. Not filed.

---

## 7. Attack items 2 and 5 — the fixture mechanism change and the two self-caught defects

### 7.1 `PartiallyExecuted` + `names_variant`: the false-positive class **is** closed

`names_variant` (`fault_expected_keys.rs:495-503`) anchors the head with
`CheckedArtifactFaultKeyV1::` and requires the character after the variant name
to be neither ASCII-alphanumeric nor `_`. That closes the nesting case the
comment names — `ManagedBootstrapMarkerRetire` ⊂
`ManagedBootstrapMarkerRetiredReobserve` — in the direction that matters: a
site for the longer, activated key no longer reports the shorter key as sited.
I checked the whole 30-key family for nesting pairs and this is the only one;
the check is sufficient, and `is_none_or` handles end-of-file correctly.

**The reserved guarantee still fails closed on a stray site**, by two
independent mechanisms:

1. a stray site for a reserved key *inside a declared source* is caught by the
   per-key loop at `:557-575` (`sites_naming(&variant)` must be empty);
2. a stray site in an *undeclared* production file is caught by the
   completeness anchor `the_declared_injection_sources_are_every_production_source_holding_sites`,
   which re-walks `src/checked_artifact/` at runtime and demands set-equality
   with `FAULT_INJECTION_SOURCES`. Its scan is exhaustive because
   `CheckedArtifactFaultKeyV1` is `pub(super)` on `checked_artifact::fault_v1`.
   The anchor is unmodified by this object and green.

Both were exercised implicitly by the green run; the mechanism is intact.

### 7.2 The two self-caught defects

- **Reservation cardinality** (`tests_managed.rs:54-70`): `managed_reservation`
  reserves exactly one managed component because
  `tests_fault_matrix::reservation` reserves two and an intent must cover its
  schedule's whole component count before reaching `RetireMarkers`. Verified
  correct and correctly reasoned; the fix is real, not cosmetic.
  **No second instance found**: `retain_managed_component_slots(retained, 0, 0,
  …)` and `ActionSlotV1::RetiredBootstrapMarker(0)` are consistent with the
  one-component schedule.
- **Substring probe** (`tests_managed.rs:168-179`): `marker_retired_row_exists`
  derives the whole slot name rather than probing a substring that "would
  silently never match". Verified correct. One residual of the same *shape* —
  the row name is derived independently of `slots.marker_retirement_leaf()`
  rather than from it — but it fails **loudly** if they ever diverge (the
  retirement re-enters and trips `require_absent_in`), so it is not a silent
  probe and I do not file it.
  **A second instance of the probe class does exist one function above the
  fix** — the family-level `contains` at `:487-495` — filed as [P3-3].

---

## 8. Attack items 4, 6, 7 — provenance door, LOC, and the permit literal

**Deferred provenance gap — verified closed against production.**
`retain_managed_parent_at_for_test` is `#[cfg(test)]` at its definition
(`managed_mutation.rs:154-163`) **and** at both re-export hops
(`provider.rs:66-67`, `pre_catalog.rs:25-26`), so no production path can reach
it and it does not exist in a release build. It is the only place in the owner
that opens an ambient path, and it says so (`:146-153`). The production
constructor `retain_managed_parent` (`:110-144`) takes an already-retained
`&Dir` and does one identity-proved no-follow hop, mirroring
`namespace_mutation::retain_action_namespace`. Its production caller is Phase
3.1, and the allow-comment records exactly that: `#[allow(unused_imports,
reason = "Step 2.3 lands the managed capability; plan §4 Step 3.1 wires its
production caller")]` (`pre_catalog.rs:31-34`). **Same posture as Step 2.2,
verified.** (Nit, not filed: the allow covers a five-item `use` group of which
only `retain_managed_parent` is unused.)

**LOC — arithmetic confirmed, structural argument holds.** 864 production
lines against the aspirational <500 is **73% over**; counting tests, 1 430.
Recorded, not excused. The two structural drivers verified for 2.2 apply
unchanged and account for the bulk: the `pub(super)` seam splits the work into
a provider half (`managed_mutation.rs`, 433 — it may name
`publish_verified_no_replace`, `sync_directory_edge`, `encode_identity`,
`HostPlatform`, `interior::*`) and a `namespace` half (`host.rs`, +266 — it may
name `BackendIssuer`, `ProviderBinding::owner_new`, the request/observation
constructors), and merging them would be a seam change a step lane may not
take. A third driver is specific to 2.3 and genuine: this step implements
**four** trait methods, not three, and each carries a distinct forward/restart
pair, so the required-signature and dispatch surface alone is ~150 lines of
`host.rs`. The overrun is structural rather than discretionary; I record it
without a finding, consistent with the 2.2 precedent.

**The `capability_permit.rs` literal 11 is correct for this object alone.**
Counted directly: `mutation.rs` 1 + `directory_mutation.rs` 5 +
`admission_mutation.rs` 2 + `namespace_mutation.rs` 1 + `managed_mutation.rs` 2
= **11**, and the `callers` string at `:126` concatenates exactly those five.
The pin is right. **Flag, not a finding:** the landing must reconcile to **13**
with the Step-2.4 lane, which adds `authority_record_binding.rs` (2 sites) and
is not yet in this test's `callers` string.

---

## 9. Gates executed

All from `gwz-core/`, foreground, macOS host,
`CARGO_TARGET_DIR=…/scratchpad/rev23-target`, after the object's last write
(17:13:20).

```text
cargo test --lib checked_artifact::
      → test result: ok. 327 passed; 0 failed; 0 ignored; 1169 filtered out;
        41.74s                                   (matches the reported 327/0)

cargo test --lib checked_artifact::namespace::tests_managed
      → test result: ok. 12 passed; 0 failed; 1484 filtered out; 6.84s
        8 × tests_managed (install / restart-reobserve / marker-retire /
        interior-drift, each on both target variants)
        4 × tests_managed_matrix (8×2 interruption matrix, 3×2 repeated-crash)

cargo clippy --all-targets --all-features -- -D warnings
      → Finished `dev` profile, exit 0            (at ~17:2x, tree as reviewed)
      → re-run at 17:29:32: exit 101, six E0061 errors, ALL in
        capability/pre_catalog/provider/tests_authority_record.rs, against
        signatures in authority_record_binding.rs which the Step-2.4 lane
        rewrote at 17:29:07 — 25 s earlier. Attributed to that lane; no
        Step-2.3 file is implicated and no Step-2.3 file changed after 17:13.

cargo fmt -- --check  (scoped to this object's nine files)
      → exit 0 (clean)

python3.13 scripts/checks/check_checked_artifact_boundaries.py
      → checked-artifact boundary: failed, exit 1
        - protected source allowlist changed: workspace_ops/merge/preserve/artifacts.rs      [D3 lane]
        - protected source tree changed: checked_artifact/capability/pre_catalog.rs          [cross-lane, see below]
        - protected source tree changed: workspace_ops/merge/v1_lifecycle/authority/observe.rs [D3 lane]
        - protected source tree changed: workspace_ops/merge/v1_lifecycle/mod.rs             [D3 lane]
        - catalog publication seam changed: all six physical moves must use the
          single source-associated publication primitive                                    [THIS OBJECT — P1-1]
      (reproduced identically at 17:36:35)
```

On the `checked_artifact/capability/pre_catalog.rs` tree digest: the pin **was**
refreshed by this lane (`52644519…` → `9aed2e9b…`), so that half of the ritual
was performed. It nonetheless fails, because `source_tree_digest`
(`check_checked_artifact_boundaries.py:763-779`) hashes every file under
`pre_catalog/`, and the Step-2.4 lane's files live there and were rewritten
mid-review. **Not attributable to Step 2.3**; it is a same-commit refresh the
combined landing owes.

No allowlist-count, bare-token, module-rule, catalog-lease-reference or
provisional-interface finding appeared. No gate was run on an extracted copy:
the live tree's misbehaviour was isolated to the two other lanes and did not
obscure any Step-2.3 result.

**Tracked, not a finding:** the 8×2 and 3×2 managed matrices are macOS-only
here. Windows evidence for E15/E16 is owed to the next full Windows matrix run,
on the standing deferral of amendment `:665-667`.

---

## Findings

### [P1-1] The freeze-mandated checker companion of the caller-pin extension is missing; the enforced publication-seam gate fails attributably to this object — and this is a repeat of Step 2.2's settled [P2-1]

- **Evidence.** `python3.13 scripts/checks/check_checked_artifact_boundaries.py`
  fails deterministically with `catalog publication seam changed: all six
  physical moves must use the single source-associated publication primitive`.
  Root cause: `CATALOG_PUBLICATION_CALL_COUNTS`
  (`check_checked_artifact_boundaries.py:663-677`) lists
  `mutation.rs: 1`, `directory_mutation.rs: 5`, `admission_mutation.rs: 2`,
  `namespace_mutation.rs: 1`, `authority_record_binding.rs: 2` — and **not**
  `managed_mutation.rs`, which this object adds with **2** production sites
  (`managed_mutation.rs:285`, `:383`). The rule at `:833-870` computes
  `publication_callers` from the production tree and requires
  `publication_callers == set(CATALOG_PUBLICATION_CALL_COUNTS)`, so the failure
  is set-equality and unconditional.
- **Attribution is exact.** Counting `^\s*publish_verified_no_replace\s*\(` per
  production provider file: `admission_mutation.rs` 2,
  `authority_record_binding.rs` 2 (2.4 lane, **declared**),
  `directory_mutation.rs` 5, `managed_mutation.rs` 2 (**undeclared**),
  `mutation.rs` 1, `namespace_mutation.rs` 1. The 2.4 lane declared its entry;
  only this object's is absent. `tests_admission_spike.rs` is excluded by
  `production_rust_files`.
- **Contract.** Freeze §4.4 Class 1 (:741): "caller-pin and digest refresh
  extended deliberately **in the same commit**"; and §4.4 (:784-786) quoting
  plan Step 1.2 — "extend the six-site pin at
  `interface_tests/capability_permit.rs:110` **and**
  `CATALOG_PUBLICATION_CALL_COUNTS` deliberately, same-commit digest refresh".
  The object performed two of the three: it extended
  `capability_permit.rs` (9 → 11, `:133`, with a nine-line justification
  comment at `:115-124`) and it refreshed the `pre_catalog.rs` tree digest. It
  omitted the third, which is the machine-enforced one.
- **Why P1 and not P2.** `GwzM5-8R2DStep22-Review.md` [P2-1] filed the
  *identical* defect against the *immediately preceding* step, in the same
  dictionary of the same file, and it was remediated (`namespace_mutation.rs: 1`
  is present today). This object had explicit, recent, same-file notice and
  demonstrably knew the two-place rule — the `capability_permit.rs` comment
  says "extends the caller inventory deliberately" — yet executed only one
  place. A repeat of a settled finding on a machine-enforced freeze gate is
  worse than a first occurrence, and the practical consequence is that the
  object **cannot land**: the gate is red and attributable. A GO verdict would
  assert the opposite.
- **Remediation (one line).** Add to `CATALOG_PUBLICATION_CALL_COUNTS`, in the
  established annotated form:
  ```python
      # R2-D Step 2.3 extends this inventory deliberately (freeze §4.4 Class 1):
      # edge E15 publishes the staged component inside the managed parent and
      # edge E16 retires the ownership marker out of the installed component
      # into the action directory's scheduled row — two no-replace moves through
      # the one primitive.
      "checked_artifact/capability/pre_catalog/provider/managed_mutation.rs": 2,
  ```
  then re-run the checker and refresh the `pre_catalog.rs` tree digest as the
  literal last pre-commit step.

### [P2-1] The routed §3.5 map annotation for the partial `managed_bootstrap.*` activation is not filed; the frozen map and the code now disagree

- **Evidence.** `GwzM5-8R2DInterfaceFreeze.md` §3.5 (:439) still reads
  `| managed_bootstrap.* | 30 | R2-D **Phase 3** | reserved |`, and
  `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` §10's dated blocks carry no
  Step-2.3 correction. The code says otherwise:
  `fault_expected_keys.rs:320-323` declares
  `FaultFamilyActivationV1::PartiallyExecuted("R2-D phase 3 (managed-parent
  provider); rows E15/E16 by phase 2 step 2.3", MANAGED_BOOTSTRAP_STEP_2_3_KEYS)`
  over eight keys with sites and rows. The implementer records the divergence
  in the fixture itself — `:315` "(the map currently reserves the family for
  Phase 3)" — and leaves it unreconciled.
- **Contract.** `GwzM5-8R2DStep22-Review.md`, "Routed (not a defect of this
  object)": "two obligations land on **Step 2.3's gate**: (i) … RemPlan §10's
  same-package duty follows the edge — **2.3 must activate that subset with rows
  and a deliberate §3.5 map annotation** (the map currently reserves the family
  for Phase 3), or demonstrate the crossed edges are not durable edges". The
  edges *are* durable, so the escape clause does not apply. The precedent form
  is in the freeze already: the `admission.*` row was annotated in place at
  §3.5 (:435) by its converting step.
- **Severity.** No physical defect and the code-side pins are two-place and
  fail closed, so the state is discoverable. But the controlling frozen
  document now misstates the tree, and a routed obligation is half-discharged —
  which is exactly the class the routing existed to prevent.
- **Remediation.** One dated annotation under §3.5's `managed_bootstrap.*` row
  in the `admission.*` form: "**partially executed** (Step 2.3, 2026-08-22 — 8
  of 30 keys for §4.3 rows E15/E16, sites in
  `capability/pre_catalog/provider/managed_mutation.rs`, matrix on both target
  variants in `namespace/tests_managed_matrix.rs`; the remaining 22 stay
  reserved for Phase 3 and are proved siteless key-by-key); counts held at
  8/165, no key minted", plus the matching append-only RemPlan §10 paragraph.

### [P2-2] The repeated-crash taxonomy is false as written, and the repeated set is under-covered by one boundary that meets its own inclusion criterion

- **Evidence.** `namespace/tests_managed_matrix.rs:151-166`:
  > "A boundary is repeatable only when the durable state its crash leaves
  > resolves back to the same boundary. The five boundaries at or after edge
  > E16's rename are crossed at most once per action by construction — the
  > retired row they produce is exactly the state that settles the sequence …
  > The three crossed here are the read-only ones whose crash re-enters the
  > same boundary".

  The five excluded are `staging_directory_publish`,
  `final_directory_reobserve`, `marker_retire`, `marker_retired_reobserve`,
  `final_identity_reobserve`. Two of them are **not** at or after E16's rename:
  `staging_directory_publish` is **E15's** rename (`managed_mutation.rs:300-303`)
  and `final_directory_reobserve` is **E15's** post-edge proof (`:343-346`).
  Neither produces a retired row, so the stated ground fails for both.
- **The substantive half.** `final_directory_reobserve` **is** read-only and
  **is** re-enterable, so it satisfies the file's own criterion and belongs in
  the repeated set. The crash sequence is exact:
  1. Arm `ManagedBootstrapFinalDirectoryReobserve`. `drive_managed_sequence`
     stages, enters `install_bootstrap_component`, publishes, flushes, and
     panics inside `observe_installed` at `managed_mutation.rs:343-346`.
     Durable state: component installed, marker inside, no retirement row.
  2. Restart. `installed_path().exists()` is true, so no re-stage;
     `staged` is false, so E15 is skipped; `marker_retired_row_exists()` is
     false, so the settled short-circuit (`tests_managed.rs:212-214`) does not
     fire. Control reaches `recover_installed_bootstrap_component`, which the
     file itself states is crossed on **every** drive (`tests_managed.rs:199-204`:
     "The install's restart observation is crossed on *every* drive, not only
     on a resumed one"). That path runs `component_reobserve` →
     `open_installed` → `final_directory_reobserve` → panics again, on the
     same durable state.
  3. Repeat to 12 rounds: census invariant, no name allocated, then settle.
  So `run_repeated_managed_crashes` should carry **four** keys, not three, and
  the ConsumerCheckpoint §12 / RemPlan-4 R2 stop-clause proof is currently one
  boundary short of its own stated class.
- **Severity.** No durable defect — the omitted boundary is read-only, so
  neither name allocation nor slot growth is physically possible there. What is
  wrong is the *evidence class* and the sentence that defines it, and this
  taxonomy is inherited verbatim by Steps 3.1/3.3, which own the same family.
  This is the class the Step-2.2 review filed as [P3-1] ("taxonomy sentence
  miswritten; durable evidence sound and **class-complete**") — here the
  evidence is *not* class-complete, which is why it lands one tier higher.
- **Remediation.** Add `Fault::ManagedBootstrapFinalDirectoryReobserve` to the
  list at `tests_managed_matrix.rs:173-177` (making the rows 4×2), and rewrite
  `:152-157` to state the true criterion — a boundary is crossed at most once
  per action when the durable state its crash leaves routes the next drive
  *past* it, which is true of the two rename boundaries (their completion
  switches the branch) and of the three post-E16 boundaries (the retirement row
  short-circuits the drive), and false of `final_directory_reobserve`.

### [P2-3] E16 is the lane's first cross-directory durable edge and lands without the cross-parent atomicity record; the restart path has no recovery from a split rename

- **Evidence.** `managed_mutation.rs:383-397` renames the marker from the
  installed component into the action directory and then flushes **two**
  parents:
  ```rust
  publish_verified_no_replace(&installed, &marker_name, action.handle(), &destination_name, …)?;
  #[cfg(test)] fault_v1::hit(CheckedArtifactFaultKeyV1::ManagedBootstrapMarkerRetire);
  sync_directory_edge(&installed, "flush managed marker retirement")?;
  sync_directory_edge(action.handle(), "flush managed marker retirement row")?;
  ```
  Every prior edge in this owner family is same-directory: Step 2.2's shared
  helper passes `&self.handle` as **both** source and destination
  (`namespace_mutation.rs:280-288`) and flushes once (`:303`). So the two-parent
  shape is new here, and it carries no comment.
- **Why it matters.** The recovery logic depends on rename atomicity across
  the two parents. If a crash could expose a *split* — marker gone from the
  installed component, retirement row absent from the action directory — the
  restart is **stuck**, not merely delayed: `marker_retired_row_exists()` is
  false so the settled short-circuit does not fire; `installed_path().exists()`
  is true so E15 is skipped; and `recover_installed_bootstrap_component` reaches
  `observe_installed`, whose interior check
  (`managed_mutation.rs:337-341`) refuses with "installed managed component
  interior is not the exact ownership marker" on every subsequent attempt. There
  is no path out.
- **Why it is not a live bug.** On the closed support table (journaled
  NTFS/ext4/APFS) a rename is one metadata transaction spanning both parents, so
  a split is not a producible crash state; and on Windows both
  `sync_directory_edge` calls are the documented no-op
  (`directory_mutation.rs:733-742`), matching freeze §4.1 P2. The defect is that
  the object's recoverability now **rests** on that fact and nowhere says so,
  and no matrix row sits between the two flushes.
- **Precedent.** The freeze's own E4 activation record (§4.3, :637-701) is
  exactly this: a first-of-kind physical shape gets a written record covering
  why the alternatives were unavailable, what the write-ahead protection is,
  what idempotence means, and — the closest analogue — "Ordering against the
  publish is namespace-enforced, not timing-enforced … on the closed support
  table … no crash state can hold both". E16 owes the cross-parent twin of that
  paragraph.
- **Remediation.** A short activation-record comment at
  `managed_mutation.rs:396-397` (or a §4.3 E16 annotation alongside [P2-1]'s
  §3.5 one) stating: the retirement is a single cross-directory rename; on the
  closed support table it is one journaled transaction, so no crash state holds
  the marker absent from both parents; the two flushes order the *observation*,
  not the atomicity; and a split rename is outside the supported table and would
  be an unrecoverable state, which is why no recovery branch exists for it.

### [P3-1] The fixture's `managed_bootstrap` activation comment claims an injection site in `namespace/host.rs`; the code has none and the sibling comment says so

`fault_expected_keys.rs:312-314` reads "have injection sites in
`capability/pre_catalog/provider/managed_mutation.rs` (plus the one
restart-entry boundary in `namespace/host.rs`)". `:399-404` says the opposite —
"the provider exposes a distinct restart entry point rather than letting the
`namespace` owner announce it. `namespace/host.rs` therefore still holds no
injection site" — and the code agrees: `grep -c CheckedArtifactFaultKeyV1
src/checked_artifact/namespace/host.rs` returns **0**; all eight sites are in
`managed_mutation.rs`, `component_reobserve` among them
(`managed_mutation.rs:320-323`). Machine-checked declarations are correct and
anchored; only the prose is wrong. Delete the parenthetical. Worth fixing
because this fixture's recorded inventory being false is precisely the freeze's
own round-1 Code [P1-2] / State [P2-1].

### [P3-2] The share-delete rationales misdescribe the Windows mechanism, in the direction that will mislead Phase 3

Both new opens claim `open_dir_share_delete` for reasons that do not hold on
their paths:

- `managed_mutation.rs:237-241`: "The staged component is the rename *source*
  of edge E15, and the sealed primitive reopens it with DELETE access. A plain
  directory open does not request DELETE sharing and collides (os error 32)
  with that handle on Windows". No collision is possible: the handle opened at
  `:242-244` is a local dropped when `retain_staging_source` returns, and only
  identity and bytes survive into `ObservedManagedObjectV1`, so it is never
  simultaneously live with the primitive's DELETE-access open. (The genuine
  instance of that collision is *inside* the primitive, `publication.rs:141`
  vs `:184-188`, and is correctly documented at `interior.rs:438-460`.)
- `managed_mutation.rs:459-461`: "edge E16 renames the marker *out of* this
  directory while this handle is live, so it must not hold a
  non-DELETE-sharing open across the edge on Windows". The live handle is the
  **parent** of the renamed object, not the object; a directory handle does not
  constrain DELETE access to its children. Moreover `FILE_SHARE_DELETE` on this
  open *permits* others to delete or rename the installed component while it is
  held — it is a relaxation, not a protection, so the sentence has the direction
  of the guarantee backwards.

Behaviour is unaffected (on macOS/Linux the helper is byte-identically
`open_dir_nofollow`; on Windows the choice is harmless conservatism), and no
crash sequence changes. Restate both comments as deliberate conservatism, since
Phase 3.1 inherits this doctrine and will otherwise over- or under-apply it.

### [P3-3] The substring-probe class is closed at the key level but left open one function above

`names_variant` (`fault_expected_keys.rs:495-503`) fixes the boundary check for
the per-key scan, but the family-level scan in the same test still uses a bare
`contains`:

```rust
        let sites = FAULT_INJECTION_SOURCES
            .iter()
            .filter(|(_, source)| {
                source.contains(&format!(
                    "CheckedArtifactFaultKeyV1::{}",
                    variant_prefix(family)
                ))
            })
```
(`:487-495`). It is safe today only because no family's variant prefix is a
prefix of another's — I checked all ten (`Runtime`, `CatalogBootstrap`,
`Admission`, `DurableLeaf`, `Namespace`, `Record`, `ManagedBootstrap`,
`Cleanup`, `Barrier`, `Terminal`) — but that invariant is unstated and nothing
enforces it, and a future family such as `record_wire` or `barrier_intent`
would silently break the `Reserved` arm's guarantee. Route the family-level
scan through the same boundary-aware helper.

### [P3-4] §4.4's arm table still names E16 as a driver of the managed-destination arm

`GwzM5-8R2DInterfaceFreeze.md` §4.4 (:771) lists
"managed destination (marker/generation retirement) | **E16, E17** | Phase 2.3
/ Phase 3". Per §3 above, E16 correctly resolves to no destination arm, so on
this tree the row is driven by **E17 alone** and its owner is Phase 3 alone. No
one routed this reconciliation (§4.3's conditional already covers the
substance), so it is informational — but it should ride the same dated
annotation as [P2-1] so the two freeze sections stop disagreeing.

### [P3-5] The E15 restart observation is phase-scoped, and its doc-comment claims otherwise

`managed_mutation.rs:327-329`: "The restart half of edge E15 …: the same
observation, reached without an edge, so a fresh process that finds the
component already installed reproduces the identical evidence." That is true
only in the window between the install and the marker retirement: after E16 the
marker has left the component, so `observe_managed_component_interior(…).is_exact()`
is false and `observe_installed` refuses at `:337-341` — for a component that
*is* installed. The design is right (the intent's phase, which
ConsumerCheckpoint §9 requires a restart to consume, selects
`observe_retired_managed_marker` instead, and that path deliberately omits the
interior check at `:404-432`), but the comment overstates and Phase 3.1's
restart driver reads it. Qualify it with the phase window.

### [P3-6] `Fixture::root_path` duplicates the existing accessor rather than widening it

`namespace/tests_fault_matrix.rs:139-148` now has both
`fn path(&self) -> &Path { &self.root }` (private, pre-existing) and
`pub(super) fn root_path(&self) -> &Path { &self.root }` (added by this
object) — identical bodies under two names, where widening `path`'s visibility
would have done. Test-fixture-only; no behaviour.

---

## Round-1 verdict — **superseded by §"Round 2" below**

**NO-GO** — one P1. The freeze's own machine-enforced publication-seam gate is
red and the failure is attributable to this object alone; the step cannot land
until `CATALOG_PUBLICATION_CALL_COUNTS` gains its `managed_mutation.rs: 2`
entry. The remediation is one dictionary line plus a digest refresh, and the
three P2s are one annotation block, one matrix key, and one comment — all well
inside a single round.

Set against that: both headline adjudications came out **sound**. E16 genuinely
resolves to no destination arm, on a marker shape pinned at three layers
including the pre-existing R1 role validator; and the 8/22 `managed_bootstrap.*`
split is exact, edge-faithful and exhaustive on both sides, discharging the
obligation the Step-2.2 review routed here. The frozen §3.2 signatures are
unreshaped, the sentinel is gone, the sibling owner's new `handle()` stays
sealed, the intent/cursor storage leaks no authority and is doubly validated,
the cfg(test) provenance door is unreachable from production, the permit
literal 11 is right for this object, and all 327 `checked_artifact::` tests
(the 12 managed cases among them), clippy and scoped fmt are green.

**Escalation trigger: RECORDED.** This review carries 1 × P0/P1 and 3 × P2
(`[P1-1]`, `[P2-1]`, `[P2-2]`, `[P2-3]`), so under `GwzM5-8R2DInterfaceFreeze.md`
§9 ("single-axis with automatic escalation on any P0/P1/P2") the lane owner
routes this object to escalation. The escalation is sharpened by [P1-1] being a
**repeat of a settled finding** — Step 2.2's [P2-1], same dictionary, same
file, one step earlier — which is a lane-discipline signal rather than a
one-off oversight, and which the routing decision should weigh alongside the
two-round cap.

Counts: **1 × P1, 3 × P2, 6 × P3.**

---

# Round 2 (2026-08-22) — remediation verification and re-verdict

Round 2 of 2 under the cap. Same axis, same protocol: attributable scratch,
`CARGO_TARGET_DIR=…/p23-target`, `python3.13` for the checker. **Verified
against the tree, not against the implementer's note** — every claim below was
re-derived from the source. Remediation writes land 17:50:14 – 17:55:17; all
gates ran after 17:55:17.

## Per-finding closure

| Finding | Round-1 grade | Status | Verified how |
| --- | --- | --- | --- |
| [P1-1] checker companion missing | P1 | **CLOSED** | `managed_mutation.rs: 2` present in `CATALOG_PUBLICATION_CALL_COUNTS` with the annotated form, placed above the 2.4 entry; the 2.4 entry byte-unchanged. Checker output no longer contains "catalog publication seam changed". Anchored re-count of `^\s*publish_verified_no_replace\s*\(` in `managed_mutation.rs` = **2**, matching the declaration. |
| [P2-1] §3.5 map annotation unfiled | P2 | **CLOSED** | §3.5 row now `**partially executed** (see annotation)` + dated blockquote in the `admission.*` form. I checked the eight named keys against `MANAGED_BOOTSTRAP_STEP_2_3_KEYS` and against my own round-1 §4.1 table — exact match, all eight. "22 reserved, proved siteless key by key", "counts held at 165, no key minted", "caller pin 9 → 11 with its companion" all verified true on the tree. The fixture's dangling "(the map currently reserves the family for Phase 3)" note is gone. |
| [P2-2] false taxonomy + missing boundary | P2 | **CLOSED, and empirically confirmed** | See below. |
| [P2-3] E16 cross-parent record | P2 | **CLOSED — Branch A ruled correct on the merits** | See below. |
| [P3-1] false `host.rs` site claim | P3 | **CLOSED** | `fault_expected_keys.rs:315-316` now reads "all eight, the restart-entry boundary included"; the sibling comment at `:405` ("`namespace/host.rs` therefore still holds no injection site") is now consistent rather than contradictory. |
| [P3-2] share-delete rationales backwards | P3 | **CLOSED, fully** | Both restated. The second is now exactly right: "a directory handle never constrains DELETE access to its children, so the sharing mode is not what makes edge E16 legal. It is a relaxation — it permits others to rename or delete this component while the handle is held — not a protection", with "Phase 3.1 inherits this doctrine: apply it for uniformity of the owner's opens, never as an interlock argument." The first names where the genuine collision lives (inside the primitive). |
| [P3-3] family scan still bare `contains` | P3 | **CLOSED, better than asked** | New `family_sites()` (`:524-538`) enumerates the family's *keys* and routes each through `sites_naming` → `names_variant`, used at `:545`. `grep 'source.contains(&format!'` returns nothing. The unstated no-prefix-collision invariant is **removed** rather than documented — the stronger of the two fixes I offered. |
| [P3-4] §4.4 arm table names E16 | P3 | **CLOSED** | Resolved inside the §4.3 E16 annotation, which states the row is "driven by **E17 alone and owned by Phase 3 alone**" on this tree and says why §4.4 is left as written. Correct call: the sanctioned-records-only constraint is honoured and the substance is recorded where §4.3's conditional already lived. |
| [P3-5] restart-observation comment overstates | P3 | **CLOSED** | `managed_mutation.rs:333-346` now carries "**Phase-scoped, deliberately.**", names the window, explains why refusing post-retirement is correct (the intent's phase selects `observe_retired_managed_marker`, which omits the interior check), and adds a do-not-widen warning. |
| [P3-6] duplicate accessor | P3 | **CLOSED** | `root_path()` deleted; the pre-existing `path()` widened to `pub(super)` (`tests_fault_matrix.rs:142`); the four `tests_managed.rs` call sites updated. |

**Ten of ten closed. No finding partially discharged, and no remediation
introduced a new defect that I could find on re-read of the changed regions.**

## [P2-2] — the partition genuinely pins, and the missing boundary was real

The restructure is stronger than the one-line fix I asked for:

- the criterion is restated correctly and generally — "A boundary is
  *single-crossing* when the durable state its crash leaves routes the next
  drive **past** it" (`tests_managed_matrix.rs:62-64`), which is the routing
  formulation, not the rename formulation that failed;
- the error is **recorded rather than quietly overwritten** (`:71-77`), naming
  both halves: that the old sentence was "false twice over" for
  `staging_directory_publish` and `final_directory_reobserve`, and that it
  wrongly excluded the latter;
- `REPEATED_BOUNDARIES` (4) and `SINGLE_CROSSING_BOUNDARIES` (4) each carry a
  per-boundary routing reason. I checked all four exclusions against my own
  round-1 trace and each is correct: E15's rename consumes the staging name so
  the next drive computes `staged == false`; the three post-E16 boundaries are
  skipped by the `marker_retired_row_exists()` short-circuit;
- `assert_boundary_partition()` (`:140-162`) is a real pin, not a comment: it
  sorts the union, asserts `union == deduped` (no boundary in both classes) and
  `union == matrix` (none in neither), and is called from **both** entry points
  (`:182`, `:237`), hence from all four matrix tests.

**Empirically confirmed.** Running the repeated suite with `--nocapture`
returns eight evidence lines, and the boundary my finding said was missing is
among them:

```text
managed_bootstrap.parent_revalidate        | workspace     | rounds=12 | slots-stable=yes | converged=yes
managed_bootstrap.parent_revalidate        | git-directory | rounds=12 | slots-stable=yes | converged=yes
managed_bootstrap.final_directory_reopen   | workspace     | rounds=12 | slots-stable=yes | converged=yes
managed_bootstrap.final_directory_reopen   | git-directory | rounds=12 | slots-stable=yes | converged=yes
managed_bootstrap.component_reobserve      | workspace     | rounds=12 | slots-stable=yes | converged=yes
managed_bootstrap.component_reobserve      | git-directory | rounds=12 | slots-stable=yes | converged=yes
managed_bootstrap.final_directory_reobserve| workspace     | rounds=12 | slots-stable=yes | converged=yes
managed_bootstrap.final_directory_reobserve| git-directory | rounds=12 | slots-stable=yes | converged=yes
```

`final_directory_reobserve` sustaining 12 rounds with a stable slot set and
converging is the direct experimental confirmation of the round-1 reading: the
boundary *is* re-entered on every restart, so it belonged in the class, and the
stop-clause proof was genuinely one boundary short. 4×2 rows now.

## [P2-3] — RULING: Branch A is not merely acceptable, it is the correct branch

The record is filed in both places: a derivation comment at the edge
(`managed_mutation.rs`, inside `retire_marker`, between the `marker_retire`
fault site and the two flushes) and a §4.3 E16 annotation in the E9 blockquote
format, which also carries the [P3-4] reconciliation. I rule on both questions
the coordinator posed.

**Is the three-state analysis sound on the support table? Yes, and it is
complete.** The drive's branching depends on exactly two durable booleans —
`installed_path().exists()` and `marker_retired_row_exists()` — so the
observable state space after the rename instruction has four cells, and the
record accounts for all four:

- *rename discarded* (row absent, component present) → the next drive skips
  E15 (`staged == false`), does not short-circuit, and re-enters E16 on the
  same schedule-derived row. The cited rows `component_reobserve` and
  `final_directory_reobserve` do settle from precisely this state — I verified
  that independently in round 1 when tracing the same crash points;
- *rename durable, neither parent flushed* (row present) → short-circuit. The
  cited `marker_retire` boundary "sits precisely here", which is true: the hit
  fires after the publish and before both flushes;
- *rename durable, both flushed* → same short-circuit, one boundary later
  (`marker_retired_reobserve`, `final_identity_reobserve`).

The apparent fourth reachable state — *first parent flushed, second not* — is
correctly folded in rather than omitted: the flushes push already-committed
metadata and cannot make half a rename durable, so it is
namespace-indistinguishable from the two above. That is why "nothing durable
changes *between* the two flushes" is the right claim, and why the window is
**bracketed** by two existing green rows rather than uncovered. **The no-key
decision is also right**: there is no namespace transition inside that window
to announce, and minting a key would move the frozen 165 census — a freeze edit,
not something an implementation lane may take silently. That disposes of the
"no matrix row sits between the two flushes" half of my finding on the correct
ground.

**Is "typed deliberate permanent refusal for off-table states" an acceptable
resolution of my "refusing permanently with no branch out" concern? Yes —
and it is the only sound resolution.** My concern was specifically a *silent
wedge on reachable states*. Both premises are now removed:

- *Reachability.* The wedge state (marker absent from the component **and**
  retirement row absent) requires a split rename, and the two candidate routes
  fail closed rather than wedging. Cross-filesystem: `renameat` returns `EXDEV`
  at the syscall, which the sealed primitive surfaces as a typed refusal
  **before either parent is touched** — I verified the ordering, the refusal
  lands after `require_absent_in` and after the identity/bytes compare but
  before any namespace mutation, so nothing durable has changed and the retry
  re-enters idempotently at `open_installed`. Foreign removal is outside the
  accepted same-user namespace boundary the design already declines to defend
  (§4.4's drift-rejection paragraph). Note also that nothing in the code
  *enforces* one-filesystem placement — it relies on EXDEV to fail closed if
  the managed parent and the catalog are ever on different volumes — and the
  record says so, which is the honest form.
- *Silence.* The state is now a documented, deliberate, typed refusal from
  `observe_installed`'s interior check, with the safety rationale stated: "a
  component whose ownership marker cannot be accounted for must not be
  adopted."

I go one step further than the coordinator asked, because it settles the branch
choice rather than merely permitting it: **a recovery arm would have been
unsound.** In the wedge state the retirement row is absent by hypothesis, and
the intent is durable before the edge in both the "retired by us, row lost" and
the "destroyed by a foreign actor" cases — so the two are indistinguishable
from durable evidence. Any recovery arm would have to *guess*, and adopting a
component whose marker cannot be accounted for is exactly the ambiguity this
codebase's typed refusals exist to prevent. Branch A is therefore the correct
engineering answer, not a convenient one. **[P2-3] is closed on the merits.**

## Gates (round 2)

All from `gwz-core/`, foreground, macOS host,
`CARGO_TARGET_DIR=…/scratchpad/p23-target`, after the last remediation write.

```text
cargo test --lib checked_artifact::
      → ok. 330 passed; 0 failed; 0 ignored; 1169 filtered out; 39.18s
        (327 at round 1; +3 is the 2.4 lane — the implementer's own
         test-function delta is 0, the 4th repeated key being a new row
         inside an existing test, which the run confirms)

cargo test --lib checked_artifact::namespace::tests_managed
      → ok. 12 passed; 0 failed; 1487 filtered out; 6.45s

cargo test …tests_managed_matrix::repeated -- --nocapture
      → ok. 2 passed; 0 failed — eight evidence lines, quoted above

cargo clippy --all-targets --all-features -- -D warnings
      → Finished `dev` profile in 11.78s, exit 0, zero findings.
        The fault_expected_keys.rs:699 `bool::then` finding another lane saw
        is gone (current code is filter().map()).

cargo fmt -- --check  (scoped to all 14 object files)
      → exit 0

python3.13 scripts/checks/check_checked_artifact_boundaries.py
      → failed, exit 1, with exactly four findings and NO publication-seam
        finding:
        - protected source tree changed: checked_artifact/capability/pre_catalog.rs   [owed, shared]
        - protected source allowlist changed: workspace_ops/merge/preserve/artifacts.rs [D3 lane]
        - protected source tree changed: workspace_ops/…/authority/observe.rs           [D3 lane]
        - protected source tree changed: workspace_ops/…/v1_lifecycle/mod.rs            [D3 lane]
```

The remaining `pre_catalog.rs` tree digest is **not a finding**: the digest
covers a tree shared with the Step-2.4 lane, so its value moves whenever either
lane writes, and under the lane owner's resolution (below) tree-digest pins are
the lane owner's land-time duty. It must be **recomputed at landing, not pinned
blind**. The three `workspace_ops` findings are the D3 lane's, untouched here.

Unchanged from round 1 and still owed at landing: the `capability_permit.rs`
literal must reconcile **11 → 13** with the 2.4 lane, which adds
`authority_record_binding.rs` to the `callers` string. 11 remains correct for
this object alone (re-verified: 1+5+2+1+2). And Windows evidence for E15/E16 is
owed to the next full Windows matrix run, on the standing deferral of amendment
`:665-667`.

## Freeze diff scope — verified

`git diff dev-docs/GwzM5-8R2DInterfaceFreeze.md` is **63 insertions, 1
deletion**, and I read it in full. The single deletion is the
`managed_bootstrap.*` table cell, replaced by its annotated form. The
insertions are exactly two blockquotes: the §3.5 activation annotation and the
§4.3 E16 annotation. **No other line of the freeze is touched** — no other row,
no §4.4 edit, no §6, no §9. The sanctioned-records-only constraint is honoured
exactly.

One cosmetic observation, recorded not filed: both annotations are blockquotes
inserted *between* table rows, which splits each table in two at render. That
is the established in-document precedent (the E9 annotation at §4.3 has done
the same since Step 2.1 and passed review), so it is consistent rather than new
— but §3.5's own prior precedent, the `admission.*` row, used an inline
parenthetical. Either form is fine; noting it only so a future settle does not
read the split table as damage.

## Process note — the brief conflict behind the P1 (recorded, not re-litigated)

The [P1-1] root cause was a genuine instruction conflict, not a missed step:
the implementer's launch brief reserved `scripts/checks/` to the lane owner
("report count/pin deltas, never edit"), while freeze §4.4 Class 1 treats the
checker companion as the converting package's **same-commit** duty. The
implementer filed the entry as owed in the landing report rather than writing
it. The same conflict produced Step 2.2's identical [P2-1] one step earlier,
which is the mechanism behind what round 1 read as a discipline regression —
that reading stands as to *effect* (the gate was red and attributable) but the
*cause* is now established as structural. The lane owner has resolved it going
forward: **counts/allowlist companions are the converting package's same-commit
duty; tree-digest pins remain the lane owner's land-time duty.** With that
resolution in place the recurrence risk at Step 2.4 is closed. The round-1 §9
escalation-trigger record stands unchanged, and routing to the Phase 5 dual is
already decided.

## Round-2 verdict

**GO.**

All ten round-1 findings are closed and verified against the tree rather than
the note: the P1's machine-enforced publication-seam gate is green with the
declared count anchored at the true value; the §3.5 activation annotation is
filed in the sanctioned form with its eight keys matching the fixture
constant exactly; the taxonomy is rebuilt on the correct routing criterion with
a real `assert_boundary_partition()` pin and the missing boundary now carrying
12 stable, converged rounds on both target variants — direct experimental
confirmation that the round-1 reading was right; and [P2-3]'s Branch A is ruled
correct on the merits, since the three-state analysis is complete over the
observable state space, the inter-flush window is bracketed by existing green
rows and rightly gets no key, and a recovery arm for the off-table wedge would
have been unsound because that state is undecidable from durable evidence. The
freeze carries only the two sanctioned records at 63/1. Tests 330/0 and 12/12,
clippy clean under `-D warnings`, scoped fmt clean; the only checker residue is
the shared `pre_catalog.rs` tree digest, which is the lane owner's land-time
recompute, plus the D3 lane's three pins. Landing conditions, both carried from
round 1 and neither a finding: recompute the shared tree digest as the literal
last pre-commit step, and reconcile the `capability_permit.rs` literal 11 → 13
with the Step-2.4 lane.

Round-2 counts: **0 × P0, 0 × P1, 0 × P2, 0 × P3.** Closure from round 1:
**1 P1, 3 P2, 6 P3 — all closed.**
