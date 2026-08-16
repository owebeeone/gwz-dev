# R2-D execution plan — managed namespace, leaf, and managed-parent recovery on `CheckedDurablePathV1`

Date: 2026-08-16

Status: **ADOPTED 2026-08-16 by the lane owner** — adoption record and
the §7 decision dispositions are in §9; issued DRAFT earlier the same
day. This is the scoping
deliverable forecast by `CurrentProgramCheckpoint.md:118` ("Scoping in
flight: `GwzM5-8R2D-Plan.md`"). It extracts the binding R2-D specification,
verifies the unblock conditions on today's tree, and proposes the phased
execution plan. It changes no interface and accepts nothing; adoption,
tier recording, and the §7 owner decisions belong to the lane owner and
operator. Verified tree: gwz-core `f36d20dda7af631f6d9a5657affd481022bcebed`
(the accepted-green Windows-matrix commit, `CurrentProgramCheckpoint.md:14`),
workspace clean at plan time.

## 1. Controlling documents

- `GwzM5-8R4bP1P2-RemPlan-4.md` §4 — the R2–R6 chain that gates R4b-G
  (scope correction recorded at `CurrentProgramCheckpoint.md:108-111`).
- `GwzM5-8R4bR2ConsumerCheckpoint.md` — R2 package definitions (§11) and
  R2-D's one-line charter (:299-300).
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md` — controls where it differs
  (§8 items 15-19 define the C0/C1/C2/C3 sub-lane; :605 sequences R2-D;
  §10 scopes the fault vocabulary).
- `GwzM5-8R2CCatalogBootstrapAmendment.md` — the accepted amendment whose
  §5 schema correction and §8 exit gates bind R2-D directly.
- Review/audit routing sources: `GwzM5-8R2C2PublicationAudit.md`,
  `GwzM5-8R2C2OwnerInterface-ReviewState-2/-3.md`, `-ReviewCode-3/-4.md`,
  `GwzM5-8R2C2DirentBarrier-ReviewState.md`,
  `GwzM5-8R2CCatalogBootstrapAmendment-ReviewState.md`.
- Process authority: `AgentProcessRules.md` as amended plus
  `GwzProcessOptimization.md` (tiering §4.2, two-round cap §4.1,
  cross-model §4.3, two-track freeze §3.1) —
  `CurrentProgramCheckpoint.md:34-42`.

## 2. What R2-D is

### 2.1 Charter and position in the lane

`GwzM5-8R4bR2ConsumerCheckpoint.md:299-300`:

> **R2-D — managed namespace.** Real `ActionNamespace` backend and
> managed-parent provider/state machine.

Sequencing, two authorities in agreement:

- `GwzM5-8R4bR2ConsumerCheckpoint.md:309`: "R2-D begins after R2-C
  admission settles."
- `GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md:605`: "R2-D through R2-F
  follow accepted R2-C."

The RemPlan executes R2-C as **four** bounded packages (§8 items 15-18):
C0 durable schema/lease, C1 aggregate grammar/classifier, C2 physical
first-catalog owner, and **C3 "admission, reservation, and handoff"**
(:597-600). C0/C1/C2 are accepted (`CurrentProgramCheckpoint.md:47-51`).
**C3 is not implemented**: `protocol/admission/owner.rs:75-125` is the
R1 classifier (`CatalogAdmissionOwnerV1::classify_handoff/admit`) with no
physical driver; the expected cohesive owner `checked_artifact/admission/`
(`GwzM5-8R4bR2ConsumerCheckpoint.md:317`, amendment §7) does not exist in
`gwz-core/src/checked_artifact/`; and `recover_or_create` still has no
production caller (`catalog.rs:16` sealed re-export only — re-verified
today), so the retained catalog that C3 must consume is reachable only
from tests. This plan therefore folds C3 in as its Phase 1, explicitly
badged as R2-C's tail executed on this lane (owner decision §7.1).

### 2.2 Amendment clauses that bind R2-D

1. **Schema floor.** `GwzM5-8R2CCatalogBootstrapAmendment.md:360-366`: the
   mixed-purpose `CheckedCanonicalPathIdentityV1` "is therefore replaced
   in every durable record by `CheckedDurablePathV1`. `CanonicalPathIdentityV1`
   remains the live retained-transaction type and is never serialized.
   **R2-D may not build managed recovery on the old mixed-purpose shape.**"
   The exact affected durable fields (:370-375): "`CheckedAuthorityV1.artifact_root`,
   `CheckedCatalogBootstrapV1.retained_parent_path`,
   `CheckedBarrierIntentV1.target_path_profile`,
   `CheckedManagedBootstrapComponentV1.installed_path`, and
   `CheckedManagedParentBootstrapIntentV1.retained_parent_path`. No second
   path encoding remains reachable from a durable-record root."
   Status: the conversion itself **landed in C0** — all five fields are
   `CheckedDurablePathV1` in `protocol/generated.rs:359/:404/:507/:558/:607`,
   the schema gate executes (`interface_tests/durable_path_schema.rs:14-36`),
   and production has zero references to the old type (grep, today). R2-D's
   duty is to build the managed/admission recovery **on** that shape and on
   its single semantic envelope — ":406-413: one semantic envelope
   everywhere it is used … No durable-record adapter may weaken or
   duplicate this envelope."
2. **Unblock gate.** `:665-667`: "R2-D remains blocked until these tests,
   the real macOS implementation, the structural boundary suite,
   formatting, and all-target/all-feature Clippy pass. Linux and Windows
   native fault evidence remains an R2-F release gate." ("These tests" =
   the §8 TDD items 1-13 plus the schema gate at :661-663.) The prior
   state review said the same, `GwzM5-8R2CCatalogBootstrapAmendment-ReviewState.md:20-22`,
   quoted in full: "The current code deliberately still has the
   pre-amendment shapes, and the amendment correctly keeps R2-D blocked
   until its TDD, physical implementation, structural, format, Clippy,
   and native-platform gates pass." (Its "native-platform" clause is the
   real-macOS-implementation requirement; the amendment's own :667
   assigns Linux/Windows native fault evidence to R2-F.)
3. **Sealed publication seam.** §4.1 (:267-316): every namespace
   publication family routes through the one sealed source-associated
   primitive; direct raw provider renames are structurally rejected
   (§8.13, :656-659). R2-D inherits this for every new edge it adds and
   for every legacy edge it converts.
4. **Fault-vocabulary duty.** RemPlan §10 (:1025-1038): the reserved
   families "`namespace.*`, `record.*`, `admission.*`, `barrier.*`,
   `managed_bootstrap.*`, `cleanup.*`, `terminal.*`, `durable_leaf.*`,
   and `runtime.*` are declared-reserved for the R2-D+ conversion … at
   which point that family must gain injection sites and matrix rows in
   the same package that converts its edges." (Basis: audit P3-1.)
5. **Owner-cohesion consequences.** Amendment §7 (:569-576): owners are
   `checked_artifact/catalog/bootstrap/`, `checked_artifact/catalog/enumeration/`,
   `checked_artifact/admission/`, and the retained provider exposing
   owner-private operations "without returning raw handles or mutation
   capability to callers." R2-D's new backends must keep that boundary.

### 2.3 RemPlan-4 §4 R2 items 1-9 — quoted, with today's dispositions

Quoted from `GwzM5-8R4bP1P2-RemPlan-4.md:1058-1080`; stop clause at
:1082-1085 binds every phase below.

1. "Convert workspace-lock/runtime-root preparation to
   `WorkspaceRuntimeBootstrapV1`; remove its ad hoc `create_dir_all`
   path." — **DONE (R2-A)**. `operation/workspace_mutator_lock.rs:35-38`
   routes `try_acquire` through
   `crate::checked_artifact::try_acquire_workspace_runtime`; the concrete
   bootstrap is `bootstrap/runtime/mod.rs:103`; the only `create_dir_all`
   in that file are inside its `#[cfg(test)]` module (:61, :179, :309).
   Checkpointed at RemPlan `:609-626`.
2. "Install read-only capability/path-mode and workspace tracked/index
   collision preflight before any private-catalog or managed-path
   mutation." — **DONE (R2-B/C0/C1)**. Retained provider + collision
   snapshot under `capability/pre_catalog/provider/` (RemPlan `:628-659`);
   permit issuance re-proof accepted through the C0/C1 reviews.
3. "Convert first-catalog preparation to `CatalogBootstrapV1`, then
   replace family-prefix-only inspection with the bounded global
   catalog." — **SPLIT**. First half done as the sealed
   `CatalogOwnerV1::recover_or_create` (`catalog/bootstrap.rs:145`, C2
   accepted) with bounded parent/interior enumeration
   (`catalog/enumeration.rs:8-13`); but it has **no production caller**,
   and family-prefix-only inspection is still the live engine
   (`classification.rs:108/:140` → `residue.rs:106-121`,
   `family_prefix`). The second half completes through C3 (global
   classification of action rows, Phase 1) and R2-E (consumers).
4. "Run the persistent `ActionDirectoryAdmissionV1` handoff, publish the
   resident `ActionCapacityReservationV1`, and return the admission
   record to `Idle` before authority/intent/managed mutation; use only
   deterministic indexed scratch and pre-reserved generation/retirement
   slots." — **NOT DONE → Phase 1 (R2-C3)**. Classifier only at
   `protocol/admission/owner.rs:75-125`.
5. "Separate authority parsing from streamed source/goal proof." —
   **NOT DONE → Phase 2**. Legacy `authority.rs:1-30` is one 16 KiB
   record (`GWZCAUTH`) carrying `canonical_path_identity` bytes; the R1
   bounded records exist (`protocol/authority_record.rs`) but no
   production streamed leaf proof exists (`LeafObserver` trait
   `leaf.rs:74`; sole impls in `interface_tests/contracts.rs:183/:236`).
6. "Route authority, goal staging, detach, publish, duplicate repair,
   cleanup, and proof-only observation through the durable
   leaf/namespace interfaces." — **NOT DONE → Phases 2 + 4**.
   `NamespaceProtocol` (`namespace/mod.rs:305`) has no production impl;
   the four managed backend operations are `Err(managed_operation_unavailable())`
   defaults (`namespace/backend.rs:411-460`).
7. "Replace Windows deletion with durable retirement and implement
   closed catalog-anchor/barrier-intent/roaming-anchor recovery." —
   **NOT DONE at action level → Phase 4** (native execution R2-F).
   First-catalog anchor recovery landed in C2; the live engine still
   runs the legacy Windows durability anchor with a **random** anchor
   scratch name (`platform.rs:372-505`, scratch at :409) — the exact
   shape the R2 stop clause forbids ("allocates a fresh retry scratch
   name", RemPlan-4 :1082-1084).
8. "Route every inventoried managed-parent creator through
   `ManagedParentBootstrap`; remove ad hoc checked-path parent
   creation." — **NOT DONE → Phase 3** (consumer rewiring R2-E). Trait
   and owner state machine exist (`bootstrap/managed/owner.rs:16/:34`)
   with test-only providers (`interface_tests/managed_plan_binding.rs:81`,
   `coordinator_contract.rs:513`); production still calls
   `CheckedArtifact::prepare_parent` via `entry.rs:144-145` from
   `workspace_ops/merge/store/mod.rs:135`.
9. "Convert every workspace/Git-dir consumer and run payloads above one
   MiB, parent/mode replacement, alias, partial protocol, same/different
   identity, repeated same-boundary scratch crashes past nominal
   capacity, the one-deep-chain and eight-plan forms of the complete
   eight-component/64-barrier schedule with deletion disabled, immutable
   cleanup recovery, full retired capacity, and every fault/restart
   boundary." — **LATER → R2-E (conversion) + R2-F (closure matrices)**.
   R2-D builds the machinery these matrices exercise; the full-schedule
   and consumer-parity forms belong to the conversion/closure packages.

### 2.4 Review-routed tracked items ("R2-D lane"), status on today's tree

| Item | Source | Status 2026-08-16 | R2-D duty |
| --- | --- | --- | --- |
| Checker bare-token/alias hardening | `GwzM5-8R2C2OwnerInterface-ReviewState-3.md:54-61` required "extend the scan to bare-token references … or digest-pin the remaining legacy interior files until R2-D retires their allowlisted call sites"; same shape Code-4 [P3-1] | **LANDED** at `641f03c` (checkpoint :207-210): the checker counts masked-source bare identifiers with definitions excluded, subsystem-wide, comment citing "rounds Code-4/State-3 [P3-1]" (`scripts/checks/check_checked_artifact_boundaries.py:160-170`), with alias/fn-pointer probe tests; checker green today | Retire the four allowlisted legacy call sites and shrink `RAW_RENAME_CALL_ALLOWLIST` (`:171-184`) — Phase 4.1 |
| 55 non-digest-pinned files | `ReviewState-3:139` | **Recounted today: still exactly 55** production `checked_artifact` files outside both digest maps (99 production files; 44 covered by `PROTECTED_SOURCE_DIGESTS` ∪ `PROTECTED_SOURCE_TREE_DIGESTS`); raw-rename evasion is closed by the bare-token counter, but the legacy interior (`cleanup.rs`, `coordinator/`, `namespace/`, `protocol/`, `transition.rs`, `residue.rs`, …) stays un-pinned for general drift (Code-4 :149) | Convert-or-pin as each Phase 2-4 package takes ownership of a file; Phase 4.1 closes `transition.rs`/`residue.rs` |
| Audit P3-1 reserved fault families | `GwzM5-8R2C2PublicationAudit.md:56-70` | Rescope note **filed** as RemPlan §10; §6 parent edge **keyed** and Git-target matrix executed (checkpoint :162-173; `fault_v1.rs` `catalog_bootstrap.*` = 24 keys; inventory 164 at `interface_tests/fault_expected_keys.rs:171`) | Per RemPlan §10: each converted family gains injection sites + executed matrix rows **in the converting package** — mapped per phase in §4 below |
| Audit P3-2 sealing perimeter | `PublicationAudit.md:71-79` | **CLOSED**: subsystem-wide raw-rename caller inventory landed in the round-3 remediation and was verified by Code-4 (:49-55 "CLOSED"), then hardened to bare-identifier at `641f03c` | None residual beyond allowlist shrink (Phase 4.1) |
| Audit P3-3 legacy/catalog coexistence | `PublicationAudit.md:80-89`: legacy leaf writers' private root **is** the catalog Final directory (`policy.rs:33-42` → `.gwz/checked-artifacts`); four legacy rename edges + the Windows durability anchor mutate inside it unsealed; "Disposition: R2-D conversion scope; record as an explicit R2-D acceptance criterion" | **OPEN**; fail-closed only because `recover_or_create` has no production caller (re-verified; State-2 concurrence at `ReviewState-2:183-189`) | Explicit R2-D acceptance criterion — Phase 4.3 |
| State-2 [P3-1] Git-parent dirent barrier | `ReviewState-2:67+` | **CLOSED** at `660f46c` (checkpoint :222-234) | New residual only (next row) |
| Dirent-barrier resume-window residual | `GwzM5-8R2C2DirentBarrier-ReviewState.md:126` ("[P3-1] Resume paths that re-enter after the scratch bytes are complete skip the root barrier…"), routing :393-395: "the next matrix/durability package or a bounded §6-style erratum, alongside the R2-F power-loss item" | **OPEN, routed** | Phase 4.3 disposition point (fix or erratum + R2-F pin) |
| State-2 [P3-2] Windows destination binding | `ReviewState-3:50` | Comment/erratum corrections landed; residual **is R2-F by design** | None (defer-out §5) |
| Legacy interior digest-pinning or conversion | `CurrentProgramCheckpoint.md:259-261` "Also open — legacy interior digest-pinning or conversion at R2-D" | Same substance as rows 1-2/5 | Phases 2-4 |

## 3. Unblock verification on today's tree

Tree: gwz-core `f36d20d`, clean. Commands executed for this plan are in §8.

| # | Condition (amendment :665-667) | Verdict | Evidence |
| --- | --- | --- | --- |
| 1 | §8 TDD items 1-13 | **PASS** | Acceptance chain: C0 GO/GO zero findings (`GwzM5-8R2C0Interface-ReviewCode-3/-ReviewState-3`, RemPlan :799-805); C1 GO/GO (RemPlan :893-907); C2 round-4 Code-4 GO / State-3 GO, accepted at root `f7ba323` / gwz-core `0d8382e` (checkpoint :188-193). Item 9's matrix executed as interruption/restart/convergence on both target variants, 24 keys (checkpoint :162-173). Item 13 audited (`PublicationAudit.md:13-52`) with the six-call-site pin (`interface_tests/capability_permit.rs:110`). Today: `cargo test --lib checked_artifact::` → **244 passed / 0 failed** (9.39 s, macOS host) |
| 2 | Schema gate (:661-663) | **PASS** | `interface_tests/durable_path_schema.rs:14-36` asserts the five durable messages use `Ref.CheckedDurablePathV1` and never `Ref.CheckedCanonicalPathIdentityV1`; fields verified in `protocol/generated.rs:359/:404/:507/:558/:607`; zero production references to the retired type |
| 3 | Real macOS implementation | **PASS** | `capability/pre_catalog/provider/platform/macos.rs` (195 lines) is a real provider: `getattrlist` persistent object/volume identity, `fpathconf(_PC_CASE_SENSITIVE)` per-parent lookup mode, typed unsupported/noncanonical rejections — not a stub; R2-B checkpoint RemPlan :628-659 |
| 4 | Structural boundary suite | **PASS** | Today: `python3.13 scripts/checks/check_checked_artifact_boundaries.py` → "ok (15 visible entries, 5 classified modules)"; CI workflow "Checked-artifact boundary" (checker + 63-test checker suite + release-boundary suite + per-commit lane gate `check_lane_commits.sh`) **SUCCESS at `f36d20d`** |
| 5 | Formatting | **PASS** | Today: `cargo fmt --all -- --check` clean |
| 6 | All-target/all-feature Clippy | **PASS** | Today (macOS host): `cargo clippy --all-targets --all-features -- -D warnings` exit 0; CI runs the identical command on ubuntu-24.04 / Rust 1.95.0 inside the boundary workflow — green at `f36d20d` |
| 7 | (Not an R2-D precondition) Linux/Windows native fault evidence | R2-F by :667 | Strengthening fact anyway: the Windows matrix is **GREEN at `f36d20d`** — 1306/0/1 + 29/0, campaign accepted (checkpoint :100-107) |
| 8 | Sequencing precondition — "R2-D through R2-F follow accepted R2-C" (RemPlan :605); "R2-D begins after R2-C admission settles" (ConsumerCheckpoint :309) | **FAIL — C3 open** | No physical admission driver (§2.1 evidence); `recover_or_create` unreachable from production |

**Verdict: the amendment's own R2-D gate is fully satisfied on `f36d20d`;
the only unmet condition is lane sequencing — R2-C3 (admission,
reservation, handoff) must land first.** This plan makes C3 its Phase 1
rather than declaring R2-D blocked, because the same lane, discipline,
and reviewers execute both and the checkpoint's position statement
already treats the remainder as one stretch ("R2-D managed recovery on
`CheckedDurablePathV1` and further conversion items remain, TDD-gated
per the amendment", `CurrentProgramCheckpoint.md:112-115`).

## 4. Phased plan

House rules: phases are shippable milestones; steps are single goals at
an **aspirational <500 LOC** each (target, not a hard limit; the
ChangeBudget ledger stop-rules in `GwzM5-8ChangeBudget.md:16-22` still
apply). Foundational-first; steps minimize cross-coupling so parallel
agents *can* take them (`GwzProcessOptimization.md` §4.4 pipeline lanes).
Review tiers follow §4.2: **dual peer-blind** at interface freezes,
durable-transition kernels, and settled-tree gates; **single-axis with
automatic escalation** for interior steps; tiers are recorded in
`CurrentProgramCheckpoint.md` at the Phase 0 freeze, not chosen mid-lane.
Cross-model where available (§4.3); two-round remediation cap (§4.1).
TDD is mandatory: every step lands its failing production-shaped tests
before physical code (RemPlan §8 pattern; ConsumerCheckpoint §12).
The ConsumerCheckpoint §14 stop conditions and the RemPlan-4 §4 R2 stop
clause (:1082-1085) bind every phase.

**Wire/records rule (two-track freeze, Track W):** R2-D expects **zero
new durable records** — the R1+C0 record set (`protocol/generated.rs`)
is the frozen floor. Any discovered need for a new/changed durable
record, slot, purpose, or phase stops the step for a Track-W freeze +
dual review before any implementation (ConsumerCheckpoint §14 :378-380;
`GwzProcessOptimization.md` §3.1).

### Phase 0 — Interface freeze and test-first scaffolding (the gate phase)

Milestone: the complete R2-D provider surface is frozen, the physical
primitive inventory is proven-by-reference, and the failing test
skeletons exist. Nothing physical mutates.

- **Step 0.1 — freeze memo + Track-P primitive inventory** (docs +
  frozen signatures; ~200 LOC of signature/test stubs). One document
  (`GwzM5-8R2DInterfaceFreeze.md`) freezing: the physical admission
  driver seam (consumes `OpaqueRetainedCatalogV1` only); the
  `ActionNamespace` backend contract change that turns the four
  `managed_operation_unavailable` defaults (`namespace/backend.rs:411-460`)
  into required methods; the `LeafObserver` provider seam (`leaf.rs:74`);
  the `ManagedParentBootstrap` provider seam (`bootstrap/managed/owner.rs:16-32`);
  and the per-phase fault-family activation map (§4 table below, updating
  RemPlan §10). Track P: map **every** R2-D physical edge onto the
  already-proven primitive family — `publish_verified_no_replace`
  (`provider/publication.rs:48`), write-through + handle flush, no-follow
  open + identity compare, bounded interior enumeration, dirent barrier —
  and declare that any edge needing a *new* platform primitive triggers a
  platform spike **before** this freeze completes
  (`GwzProcessOptimization.md` §3.1). Records the §7 owner decisions.
  **Gate: dual review (mandated tier — interface freeze), cross-model.**
- **Step 0.2 — failing-test scaffolding** (tests only; <500 LOC).
  Compile-boundary tests pinning the frozen seams (no raw handle/lease
  bytes/synthetic observation constructible by production callers —
  amendment §8.10 pattern); expected-fault fixtures extended so each
  reserved family, once activated, must show injection sites and matrix
  rows (`interface_tests/fault_expected_keys.rs` pattern, RemPlan §10).
  Rides Step 0.1's review.

### Phase 1 — R2-C3: admission, reservation, and handoff (formally R2-C's tail)

Milestone: `AdmittedActionV1` is physically obtainable from a retained
catalog on a real filesystem, restart-closed, with the `admission.*`
family executed. Unlocks R2-D proper on both sequencing authorities.
Spec: RemPlan §8 item 18 (:597-600); ConsumerCheckpoint §7 (:203-224)
nine-step durable sequence; amendment §7 owner boundaries.

- **Step 1.1 — failing production-shaped tests** (<500 test LOC): global
  catalog lookup resumes an exact existing action and stops on
  conflict/ambiguity; the exact durable sequence Idle→Preparing→staging
  dir→resident reservation→no-replace publish→Preparing→Idle→reobserve;
  `AdmittedActionV1` only from idle + missing staging + exact final
  reservation with no extra children; retry reuses names/capacity,
  never a nonce; capacity includes barrier/generation/marker/cleanup/
  terminal-retirement slots before first action mutation.
- **Step 1.2 — physical admission driver** (<500 LOC) in a new
  `checked_artifact/admission/` owner: consumes the retained catalog
  through `begin_preflight` revalidation only; bounded global
  classification of interior action rows (active / scheduled scratch /
  retired / malformed-recognized / foreign — ConsumerCheckpoint §6
  :199-201); deterministic indexed slots; all namespace edges through
  the sealed publication primitive (extend the six-site pin at
  `interface_tests/capability_permit.rs:110` and
  `CATALOG_PUBLICATION_CALL_COUNTS` deliberately, same-commit digest
  refresh per the adopted lane rule, checkpoint :186-204).
- **Step 1.3 — `admission.*` fault activation** (<500 test LOC):
  injection sites for the family; interruption/restart/convergence
  matrix on both target variants (workspace and Git-directory, the
  `catalog/bootstrap/tests.rs:326-383` pattern); repeated same-boundary
  crashes past nominal capacity with stable slots (ConsumerCheckpoint
  §12).

**Gate: dual settled review** — RemPlan §8 item 19 mandates "settled
implementation reviews after C2/C3", and admission's Idle↔Preparing is a
durable-transition kernel (mandated dual under §4.2). Parallelism: Phase
2 steps 2.1/2.2 may start behind the Phase 0 freeze while Phase 1 is in
implementation or review (different file owners; §4.4).

### Phase 2 — Durable leaf observer and real `ActionNamespace` backend

Milestone: every namespace/leaf operation the R1 interfaces promise has
a real retained-handle implementation with its fault families executed;
"default 'unavailable' managed operations are not reachable in
production" (ConsumerCheckpoint §8 :228-231).

- **Step 2.1 — `LeafObserver` production implementation** (<500 LOC):
  bounded/streamed read through one retained handle, fingerprint, flush,
  same-parent reobserve, identity/content/length stability; two-sided
  durable-absence proof; payload size never conflated with
  protocol-record size (ConsumerCheckpoint §8 :232-237). Activates
  `durable_leaf.*` with rows. Independent of 2.2.
- **Step 2.2 — backend publish/retire/barrier** (<500 LOC): the
  retained-handle `NamespaceProtocol`/`ActionNamespace` implementation
  for `publish_exact`/`retire_exact`/`barrier` over scheduled namespace
  roles (`namespace/roles.rs`), routed through the sealed primitive
  family; provenance from the permit-retained root only (audit
  provenance pattern, `PublicationAudit.md:39-44`). Activates
  `namespace.*` with rows. Independent of 2.1.
- **Step 2.3 — managed backend operations become real** (<500 LOC): the
  four defaults at `namespace/backend.rs:411-460`
  (`install_managed_component`, `observe_installed_managed_component`,
  `retire_managed_marker`, `observe_retired_managed_marker`) become
  required and implemented — forward and restart observations for
  component installation and marker retirement. Depends on 2.2.
- **Step 2.4 — authority parsing / streamed proof separation** (<500
  LOC): bounded authority-record parsing (R1 `protocol/authority_record`
  owner) split from source/goal proof, which now streams through 2.1;
  the legacy single-record `authority.rs` shape stays behind the
  retained-reader harness until Phase 4 conversion. Activates `record.*`
  with rows. Depends on 2.1.

**Gates: single-axis, alternating Code/State per §4.2, auto-escalation
on any P0-P2.** Recommend State-axis first for 2.2/2.3 (durable-edge
semantics) and Code-axis first for 2.1/2.4. Each step converts-or-pins
the files it takes ownership of (§2.4 row 2).

### Phase 3 — Managed-parent provider and execution binding

Milestone: all four frozen purposes bootstrap physically and
restart-closed through `ManagedParentBootstrap`; execution consumes only
admitted actions. Spec: ConsumerCheckpoint §9 (:249-266).

- **Step 3.1 — real provider** (<500 LOC): `observe_preflight` /
  `revalidate_plan` / `execute_bound` on the retained provider; per
  missing component: staged directory, ownership marker, installed
  observation, durable successor, prior-generation retirement, marker
  retirement, final reproof; restart consumes the resident intent and
  scheduled slots, never replans a partially completed live path.
  Depends on 2.2/2.3 shapes.
- **Step 3.2 — `managed_bootstrap.*` activation + purpose policy
  matrix** (<500 test LOC): all four purposes
  (`MergeStore`/`MergeArchive`/`PreservationBundles`/`RootPreservationMarkers`),
  overlap rejection (a missing `.gwz/merge` is `MergeStore`;
  `MergeArchive` legal only with an existing `.gwz/merge` —
  ConsumerCheckpoint §9 :253-256), every component/generation/marker
  boundary, repeated-crash slot stability.
- **Step 3.3 — coordinator execution glue** (<500 LOC): schedule +
  `AdmittedActionV1` binding so that "replacement/removal executes only
  after an admitted action and an owner-private coherent authority
  observation" (ConsumerCheckpoint §8 :239-240); writers receive only
  the opaque retained-parent proof (:264-266). Depends on Phase 1 and
  3.1. **This step wires machinery; it does not convert consumers**
  (that is R2-E).

**Gate: dual at the phase settle** (the managed intent state machine is
a durable-transition kernel); 3.1/3.2 interior iterations single-axis.

### Phase 4 — Legacy-edge conversion and Windows retirement closure

Milestone: no production write path mutates the reserved private
namespace outside the sealed primitive/backend; the coexistence
acceptance criterion (audit P3-3) is closed; behavior of live merge/
stash flows is unchanged (L2-04 retained-reader harness green).

- **Step 4.1 — convert the four legacy rename edges** (<500 LOC):
  `transition.rs:275/:368` and `residue.rs:393/:486` route through the
  sealed source-associated family (or the 2.2 backend) with identical
  external behavior; shrink `RAW_RENAME_CALL_ALLOWLIST`
  (`check_checked_artifact_boundaries.py:171-184`) accordingly and
  digest-pin or split what remains of the files; activate `cleanup.*`
  rows for the cleanup edges this converts. The checker's own comment
  already declares these "the four legacy leaf edges, retired by R2-D"
  (`:166-169`).
- **Step 4.2 — retire the legacy Windows durability anchor** (<500
  LOC): replace `.ca1-durability-anchor-*` + random `.ca1-anchor-scratch-*`
  (`platform.rs:372-505`, random name at :409 — a standing violation of
  the R2 stop clause the moment it is on a successful converted path)
  with the closed barrier/anchor protocol via the 2.2 barrier ops;
  correctness-critical Windows removal becomes durable retirement;
  activate `barrier.*` (and `terminal.*` if its edges convert here)
  with rows. Native Windows execution of these tests stays R2-F.
- **Step 4.3 — coexistence and residual dispositions** (docs + small
  code; <300 LOC): (a) close the audit P3-3 acceptance criterion —
  after this phase, either the legacy leaf grammar inside
  `.gwz/checked-artifacts` is admission-owned/grammar-legal or a
  reviewed quarantine/relocation decision is recorded **before any
  production catalog activation** (interacts with the MAX_PATH
  relocation candidate, checkpoint :144-147 — owner decision §7.2);
  (b) disposition of the dirent-barrier resume-window residual
  (`DirentBarrier-ReviewState.md:126`): fix in-line here or file the
  bounded §6-style erratum and pin the power-loss aspect to R2-F, as
  its review allows (:393-395).

**Gate: dual** — this phase rewrites the durable kernel under live
production flows; retained-reader harness (L2-04) is a hard gate in
addition to review.

### Phase 5 — R2-D settled gate

Milestone: one settled tuple carrying all of the above, with executed
per-key evidence, ready for the R2-E lanes.

- **Step 5.1 — full local gate train + evidence** (<300 LOC of gate
  wiring/ledger): focused and full `gwz-core` suites, integration
  binaries, boundary checker + its unit tests + release-boundary suite,
  retained-reader manifest, protocol regeneration/currentness, merge-doc
  and compatibility gates, `cargo fmt`, all-target/all-feature Clippy,
  LOC/cohesion audit + ChangeBudget row; per-key executed fault evidence
  in the L1-16/L2-14 form (every activated family, both target
  variants); settled tuple committed with installed `gwz`.
- **Step 5.2 — dual settled-tree re-review** (aggregate settled-tree
  gate — mandated dual, cross-model), two-round cap in force. R2-E may
  start its interface work behind this gate per §4.4.

Fault-family activation map (Step 0.1 freezes this; updates RemPlan §10):
`admission.*` → Phase 1; `durable_leaf.*` → 2.1; `namespace.*` →
2.2/2.3; `record.*` → 2.4; `managed_bootstrap.*` → Phase 3; `cleanup.*`,
`barrier.*`, `terminal.*` → Phase 4 (any key whose edge genuinely
converts later is explicitly re-reserved for R2-E/R2-F in the same
update); `runtime.*` is already executed (`bootstrap/runtime/tests.rs`,
`catalog_lease/tests.rs` — per `ReviewState-2:160-169`).

File ownership (ConsumerCheckpoint §11 :312-326): new owners
`checked_artifact/admission/`, `checked_artifact/namespace/backend/`
(with OS children if platform code splits), `checked_artifact/bootstrap/managed/provider/`;
1,000-line cohesion trigger, sub-500 targets (RemPlan-4 §5 :1178-1181).

## 5. What R2-D is NOT (defer-out)

Scope cannot creep past this list without a lane-owner scope decision:

1. **No consumer conversion (R2-E).** None of the §10 conversion-table
   rows (`GwzM5-8R4bR2ConsumerCheckpoint.md:268-286`) flips; merge
   store/archive, stash bundles, markers, `.git/info/exclude`, and the
   checked workspace/Git-dir flows keep their current call graphs.
   "R2-E consumer lanes begin only after the facade and opaque
   retained-parent result are lead-owned and tested" (:309-310).
2. **No production catalog activation.** `recover_or_create` gains no
   production caller in R2-D; first activation is an R2-E/A1-adjacent
   decision behind the Phase 4.3 coexistence criterion. R2-D "does not …
   enable merge-v1, or add a merge lifecycle phase" (amendment :32-33;
   ConsumerCheckpoint :25-33 unchanged).
3. **No legacy-path removal (R2-F).** Removal of legacy successful
   paths and dead-code allowances is R2-F closure (ConsumerCheckpoint
   :304-305; acceptance gate :395). R2-D converts edges in place;
   deletion waits for consumer parity.
4. **No native-platform evidence obligations.** "Linux and Windows
   native fault evidence remains an R2-F release gate" (amendment
   :667); the native Windows destination-window/object-binding tests
   (§4.1 erratum :314-316; checkpoint :260-261), the real-Windows
   exact-evidence satisfiability tripwire (checkpoint :138-142), and
   R5's three-platform evidence jobs (RemPlan-4 :1131-1138) all stay
   where they are. Existing CI lanes must merely stay green per push.
5. **Not R3-R6.** Complete checkout/aggregate handoff, reconciliation/
   executable C7, the settled implementation gate, and the independent
   re-reviews are the later chain (RemPlan-4 :1087-1145).
6. **No new durable records, slots, purposes, actions, or phases, and
   no public GWZ protocol change** (ConsumerCheckpoint §14 :375-386;
   amendment :610-614). Discovery of a needed record change is a
   Track-W stop, not an R2-D deliverable.
7. **Not the parallel program debts:** pre-A1 checklist items (operator
   escape runbook, panic-invariant conversion packages, preservation-
   cursor decision — checkpoint :71-85), the exact-evidence amendment's
   OPEN DECISIONS (foreign-filter policy — checkpoint :138-141), MAX_PATH
   relocation execution (checkpoint :144-147, R2-F candidate; Phase 4.3
   only *decides* compatibly), resolver Publication-arm diagnosability
   (checkpoint :143-144), and rulebook Phase-2 doc tooling (checkpoint
   :86-87). They stay on their own lanes.

## 6. Estimate honesty

Program history says nominal budgets here underrun reality by 2-10x:

- R2-C0 needed three review rounds (two corrections) before GO/GO
  (RemPlan :710-805); C1 two rounds (:852-907); C2 four rounds **plus**
  a §4.1 conformance audit, a dirent-barrier closure package, two
  matrix-extension packages, and a checker-hardening package before the
  lane moved on (checkpoint :149-234). The R2 consumer RemPlan itself
  ran **eleven** review rounds
  (`GwzM5-8R4bR2ConsumerCheckpoint-RemPlan-ReviewCode-11.md`), and the
  optimization plan's own evidence section records "eleven rounds on
  R2; … rounds became the architecture process"
  (`GwzProcessOptimization.md:96-99`). The Windows matrix took an
  eleven-run campaign, 126 → 0 (checkpoint :89-107).
- The aspirational sum below is ~5.5-7k LOC across 15 steps. Expect the
  landed diff mass at 1.5-3x that (tests and matrices dominate, as in
  C0-C2), and wall-clock at 2-10x any single-session estimate.
- Therefore: schedule in **gates and review rounds**, not LOC. Nominal:
  4 dual gates (0.1, Phase 1, Phase 3/4 settles, 5.2) × ≤2 rounds under
  the cap, plus 4-6 single-axis interior reviews. A third architectural
  root cause on any one object is a stop-the-lane redesign signal, not
  another guard round (`GwzProcessOptimization.md` §4.1).
- Rough sizes: Phase 0 ≈ 0.2-0.5k; Phase 1 ≈ 1.2-1.8k; Phase 2 ≈
  1.5-2.2k; Phase 3 ≈ 1.0-1.6k; Phase 4 ≈ 1.0-1.6k; Phase 5 ≈ 0.2-0.5k
  (prod+test, before the historical multiplier).

## 7. Owner decisions required at adoption

1. **C3 badging:** execute admission as Phase 1 of this lane (this
   plan's assumption) or as a separately checkpointed R2-C closure
   package first. Either way it precedes R2-D proper (RemPlan :605).
2. **Coexistence option for Phase 4.3:** convert-in-place (legacy leaf
   grammar becomes admission-owned) vs quarantine/relocation of the
   legacy private area (which would also retire the MAX_PATH exposure —
   checkpoint :144-147). Decision needed before any catalog activation.
3. **Backend trait surface:** confirm the Step 0.1 interface delta that
   removes the four `managed_operation_unavailable` defaults (an
   interface change to a frozen R1 trait — dual-reviewed at the freeze).
4. **Fault-family activation map** (§4 end): confirm, and file the
   matching RemPlan §10 update in the Phase 0 docs commit.
5. **Dirent-barrier resume-window:** in-line fix at 4.2/4.3 vs bounded
   §6-style erratum + R2-F pin (`DirentBarrier-ReviewState.md:393-395`).
6. **Tier recording:** enter the §4 gate tiers into
   `CurrentProgramCheckpoint.md` at the Phase 0 freeze (§4.2 requires
   recording at freeze time).

## 8. Verification appendix — commands executed for this plan (2026-08-16)

All from `gwz-core/` at `f36d20dda7af631f6d9a5657affd481022bcebed`, clean
worktree; read-only plus build/test artifacts only.

```text
git rev-parse HEAD                          → f36d20dda7af631f6d9a5657affd481022bcebed
python3.13 scripts/checks/check_checked_artifact_boundaries.py
                                            → checked-artifact boundary: ok
                                              (15 visible entries, 5 classified modules)
cargo fmt --all -- --check                  → clean
CLIPPY_CONF_DIR=$PWD cargo clippy --all-targets --all-features -- -D warnings
                                            → Finished, exit 0 (macOS host)
cargo test --lib checked_artifact::         → ok. 244 passed; 0 failed; 1106 filtered (9.39 s)
gh run list (checked-artifact-boundary / retained-readers / windows-matrix)
                                            → all three SUCCESS at f36d20d
digest-coverage recount (production_rust_files ∖ (flat ∪ tree pins))
                                            → 99 production files, 44 pinned, 55 unpinned
                                              (same 55-file surface as ReviewState-3:139)
grep Ref.CheckedCanonicalPathIdentityV1 (src/, protocol/)
                                            → only the two negative assertions in
                                              interface_tests/durable_path_schema.rs:18/:34
grep recover_or_create (production)         → definition + sealed re-export only
fault inventory                             → 164 keys (fault_expected_keys.rs:171),
                                              catalog_bootstrap.* = 24
```

## 9. Adoption record (lane owner, 2026-08-16)

Adopted after a full read on gwz-core `90d3f8a` (four trains after the
plan's verified `f36d20d`: platform-matrix workflow, D1+D2 filter
policy + R2-F missing tests, P1-P3 panic conversions + F-1 v0 gate,
run-12 residue fixes). None of those trains touch the admission/
namespace/managed surfaces or the §3 gate conditions; the boundary
workflow was green on every intervening push and the Windows matrix is
GREEN at `90d3f8a` (run 13, 1322/0/1; ledger). Line citations into
`platform.rs` may have drifted by the two added `windows_tests`; Step
0.1 re-verifies at its own tuple. Two nits recorded, neither worth a
round: §6 says "15 steps" — the §4 phase list enumerates 17; the §6
size sum brackets 5.1-8.2k, not 5.5-7k. Neither changes any gate.

§7 decisions, as adopted:

1. **C3 badging — option (a):** admission executes as Phase 1 of this
   lane, badged as R2-C's tail. RemPlan §8 item 19's "settled
   implementation reviews after C2/C3" is preserved as the Phase 1
   dual settled gate; no separate R2-C closure checkpoint.
2. **Coexistence (Phase 4.3) — direction: quarantine/relocation
   preferred**, because it also retires the MAX_PATH exposure
   (checkpoint :144-147); convert-in-place remains live as fallback.
   The binding reviewed decision is made at Phase 4.3 per the plan
   text — in any case before any production catalog activation — and
   Step 0.1's freeze memo carries the Track-P analysis for both
   options so the 4.3 decision is evidence-based, not re-scoped.
3. **Backend trait surface — confirmed:** Step 0.1 freezes the delta
   that removes the four `managed_operation_unavailable` defaults
   (`namespace/backend.rs:411-460`) and makes the methods required;
   the 0.1 dual review is the gate for changing the frozen R1 trait.
4. **Fault-family activation map — confirmed** as tabled at §4 end;
   the matching RemPlan §10 update files in the Phase 0 docs commit.
5. **Dirent-barrier resume-window — conditional:** in-line fix at
   4.2/4.3 if it fits the step budget, else the bounded §6-style
   erratum + R2-F power-loss pin (`DirentBarrier-ReviewState.md:393-395`
   allows either); ratified in the Phase 0 freeze memo.
6. **Tier recording — confirmed:** the §4 gate tiers enter
   `CurrentProgramCheckpoint.md` in the Phase 0 freeze commit.

Cross-lane compatibility noted at adoption: the operator-escape
amendment (in drafting) and the accepted D3 durable-cursor amendment
change the **I2 merge record**, not the checked-artifact R1+C0 durable
record set — no Track-W collision with this plan's zero-new-records
rule; D3's implementation package runs on `workspace_ops/merge` files,
file-disjoint from this lane's `checked_artifact` owners.

Execution begins with Phase 0 (Step 0.1 freeze memo + Step 0.2
scaffolding riding one dual review), per the plan.
