# R2-D Phase 1 settled review — STATE axis (round 1)

Date: 2026-08-22. Axis: **STATE** (durable-transition kernel: crash closure,
restart determinism, composition safety). Adversarial, round 1 of a two-round
cap. Dual #2 of R2-D's three retained duals.

Object: gwz-core commits **`5a7ff0f`** (Steps 1.1+1.2: six acceptance tests,
admission driver, C-2 recheck arms, C-3 observer grammar) **+ `c13f773`**
(Step 1.3: `admission.*` fault activation), i.e.
`git diff c40e712 c13f773 -- src/checked_artifact
scripts/checks/check_checked_artifact_boundaries.py`, judged **as published**
(the recorded early-push incident). `7a97f56` (CRLF pin normalization in
`interface_tests/r2d_seam_freeze.rs`) is inside the surface and is judged with
it. Interleaved `3e60529`/`8c1624a`/`558f834` are the other lane's
workspace_ops M5b work and are outside this object. All file:line positions
below are at `c13f773` unless labelled `c40e712`.

Peer-blind: `GwzM5-8R2DPhase1-ReviewCode.md` was not read.

Verification executed on this host (macOS), working tree at HEAD = `c13f773`
with `git status` **empty under `src/checked_artifact` and `scripts/checks`**
(the object surface is pristine; only the other lane's `src/workspace_ops`
files are modified):

```text
cargo test --lib checked_artifact::admission::
    → ok. 12 passed; 0 failed; 9.26s
      (6 acceptance + 2 driver-kernel + 4 matrix: 19-key interruption/restart/
       convergence on workspace AND git-directory targets, 12-round
       same-boundary crashes on both targets)
python3.13 scripts/checks/check_checked_artifact_boundaries.py
    → failed, three findings, ALL naming src/workspace_ops (the other lane's
      uncommitted files). Zero findings on the object surface: no
      publication-seam, raw-rename, module-rule, catalog-lease-reference, or
      provisional-interface finding, and the pre_catalog tree digest matches
      the committed content.
```

## Verdict

**NO-GO.** 0 × P0, **1 × P1**, 2 × P2, 4 × P3.

The kernel itself — the Idle↔Preparing durable-transition machine, the E4
retire-then-publish composition, and absent≡idle — is **adjudicated sound**
(§1 below is the admitting record). The NO-GO is carried by one capacity
finding outside the transition machine: nothing in the sequence refuses the
65th active action, and the over-publish converts the catalog into a
permanently unobservable state (P1-1). The fix is small and local; the
adjudications in this report survive it unchanged.

---

## 1. Headline adjudication — E4 semantics and absent≡idle

### 1.1 What is actually implemented

`ActionAdmissionEdgeV1` (`protocol/admission.rs:416-433`) decomposes the §7
step 3 / step 7 record transition into three physical edges:

- `WriteAdmissionScratch` — P2 write-through create-or-rewrite + flush of the
  next state into `ActionAdmissionScratch` (`admission_mutation.rs:129-145`,
  `write_durable_record` `:395-433`: truncate, write, `sync_all`, open-file
  verify, named-file verify, parent flush);
- `RetireAdmissionRecord` — **`remove_file`** of `ActionAdmissionActive` plus
  parent flush (`admission_mutation.rs:153-160`);
- `PublishAdmissionRecord` — sealed no-replace publication of the scratch onto
  the active name (P1) with the new destination arm
  `DestinationRecheckV1::AdmissionCatalogInterior { .., absent:
  Infrastructure(ActionAdmissionActive) }`, then a named-file re-verify and a
  parent flush (`admission_mutation.rs:163-201`).

The driver (`admission/driver.rs`) is a pure decision loop: one read-only
observation per iteration, one edge per iteration, every edge derived from the
current durable state only. Its pair-resolver (`resolve`, `:131-149`) is total
over the 4×4 (active, scratch) state space:

```rust
(Missing, Idle | Preparing) => InstallScratch,
(Idle, Preparing) | (Preparing, Idle) => RetireActive,
(Missing | Idle, Missing | Other) => Idle,
(Preparing, Missing | Other) => Preparing,
_ => stop("the durable admission record and its scratch are ambiguous")
```

`(Idle, Idle)`, `(Preparing, Preparing)` and every `(Other, _)` fail closed.
A `Preparing` record for a different action is `Other` (`slot_state`,
`:151-165`), so a foreign in-flight transition **with authority installed**
stops the observer rather than being resumed or revoked.

### 1.2 Ruling on E4: the composition is ADMITTED

**The frozen `FixedReplacementDecisionV1::ReplaceActiveFromScratch` does NOT
require an atomic replacing rename, and the write-scratch → retire →
no-replace-publish composition is accepted.** The checkpoint's contingency
(re-spike) is not triggered. Grounds, which this report enters as the record:

1. **The frozen primitive vocabulary contains no replacing rename.** Amendment
   §4.1 (:267-273): "The final source check and namespace edge use one sealed
   publication primitive … and publishes **no-replace** into the
   already-proved rename domain. Callers cannot perform a separate check
   followed by a raw rename." §8.13 (:656-659) and the checker's raw-rename
   scan fail closed on anything else. Track-P (freeze §4.1) has no
   atomic-replace family, and §4.4's verdict is "New platform primitive
   required: NO". Reading E4's "P2 + P1" as demanding an atomic replacement
   would make the freeze self-contradictory: the row itself names the
   no-replace family as the edge's primitive.

2. **`ReplaceActiveFromScratch` is decision vocabulary, not an edge.**
   `classify_fixed_replacement` (`protocol/admission.rs:353-382`) is a pure
   R1 classifier with **zero production callers** on this tree (only
   `interface_tests/catalog_admission.rs:295` and
   `interface_tests/schedule_records.rs:108-144` name it). It states *what
   remains to be achieved* from a static observation — active must come to
   hold `new` — and atomicity is not a property the type can express. The
   driver realizes that goal through admitted primitives. The freeze's
   "second decision surface" prohibition (§3.1) names
   `classify_handoff`/`admit`, and the driver consumes exactly those
   (`driver.rs:61-66`, `:81-99`); it does not bypass them.

3. **The uncovered window is single-valued and convergent, and its safety is
   exactly the absent≡idle ruling (§1.3) — the two adjudications stand or
   fall together.** The composition's durable windows, per direction:

   - *step 3, Idle→Preparing*: W1 `(Idle-or-absent, preparing)` pre-retire;
     W2 `(absent, preparing)` post-retire/pre-publish; W3 `(preparing, –)`
     post-publish. W2 is byte-identical on disk to the first-admission
     virgin state (active never existed, scratch durable), which the executed
     matrix crosses at `admission.preparing_scratch_flush` and converges from
     on both target variants. Restart resolve: `(Missing, Preparing)` →
     `InstallScratch` → publish → continue.
   - *step 7, Preparing→Idle*: W1 `(preparing, idle)` pre-retire; W2
     `(absent, idle)` post-retire/pre-publish; W3 `(idle, –)`. All three are
     reconstructed **on disk** and re-entered as fresh processes by
     `every_admission_record_install_window_converges_to_the_settled_state`
     (`driver/tests.rs:166-242`): rows "retired, scratch idle", "preparing,
     scratch idle", "triad absent" — each converges to the same handoff and
     the same catalog row set, with the scratch consumed.

   A filesystem reordering that persists the publish but not the retire is
   namespace-impossible: both edges target the same name in the same
   directory, and the publish is no-replace — on the closed support table
   (journaled NTFS/ext4/APFS) no crash state can hold both the superseded and
   the published record under one name. Losing the retire alone yields W1;
   losing the publish alone yields W2; both converge.

4. **Cross-process composition is safe.** Authority lives in the ACTIVE slot;
   the scratch is write-ahead intent; the publish is the commit point. A
   successor process under the serialized lease deterministically (a)
   **completes** a pending idle-install regardless of its own action
   (`(Missing, Idle)` → `InstallScratch` fires before any own-action logic),
   (b) **revokes** only an uncommitted preparing write-ahead
   (`(Missing | Idle, Other)` → drive Idle → truncate-rewrite of the scratch)
   — at which point the crashed action owned no staging, no reservation, no
   published row, so nothing durable is lost and its coordinator re-derives
   from idle later — and (c) **stops** whenever a foreign transition has
   authority installed (`(Other, _)`). No interleaving mints or invalidates a
   proof.

**One consequence entered deliberately:** the driver's `resolve` *refines* the
R1 pair vocabulary — `classify_fixed_replacement` calls `(Missing, Exact)`
`Ambiguous`, the driver calls it install-pending. Had the driver been routed
through the generic classifier, the post-retire window would classify
Ambiguous and the composition would not converge. The refinement is confined
to the admission triad, licensed by absent≡idle, and mechanically violates no
contract (the generic classifier keeps its test-pinned meaning and no
production caller). Future phases must not "simplify" the driver back onto
`classify_fixed_replacement` without re-opening this adjudication.

### 1.3 Ruling on absent≡idle: ADMITTED, with its scope stated

**An absent `ActionAdmissionActive` slot ≡ `ActionDirectoryAdmissionV1::idle()`
is durable-state-sound, and the no-re-materialization behavior pinned by the
"triad absent" window row is accepted.** Grounds:

1. **Absence-as-idle is not this package's invention; it is R1+C0's own ground
   state.** The frozen completed-catalog layout contains no admission record
   (RemPlan §8 item 17: C2 "may not invent an unnamed or eleventh child";
   the acceptance test pins that a freshly recovered catalog carries none of
   the triad, `admission/tests.rs:333-347`). The first admission on a virgin
   catalog is legal only because absence already reads as idle. E4's install
   re-enters that same state transiently; it introduces no new reading.

2. **`idle()` carries no fields** (`protocol/admission.rs:135-145` — the Idle
   arm is fieldless; decode refuses an Idle wire record with any Preparing
   field, `:243-256`). Re-materializing it after out-of-band deletion would be
   a durable write with no durable content. The driver performing **no
   mutation** on `(Missing, Missing)` + exact final is the correct reading:
   §7 step 9's "idle" is a *state predicate*, and both encodings satisfy it.

3. **No consumer's proof weakens.** Enumerated consumers of the record on this
   tree: the driver (absent≡idle by design); the C-3 observer
   (`completed_record`/`staging_plan` now ignore the triad — presence and
   absence are both compatible with a complete catalog, and absence always
   was); the within-lease freshness digest (`snapshot.rs:224-233` frames
   infrastructure rows *and* action rows, so an admission record vanishing
   between a permit's observations is caught by revalidation — the
   out-of-band case fails closed *inside* a lease); and `AdmittedActionV1`,
   which is never durable, so no proof outlives a process. §7's reobservation
   steps are satisfied by either encoding, deterministically.

4. **Scope caveat for the record:** the settled logical state is now bistable
   on disk (record present-idle after a completed cycle vs. absent after
   out-of-band deletion or on a virgin catalog). Phase 5's settled-tree
   evidence and any future pin must assert the *state predicate*, never
   record-presence. Out-of-band deletion detection is not claimed and was
   never promised — same-user non-cooperating mutation is outside the
   amendment §4.1 trust boundary.

---

## 2. Nine-step legality — the durable-write map

§7 (:209-224) mapped onto the implementation; every write derived from the
current durable state, restart-deterministic:

| §7 step | Implementation | Durable points |
| --- | --- | --- |
| 1-2 derive plan/schedule read-only | `observe()` (`admission_mutation.rs:45-76`) through the retained completed catalog; `expected` is the coordinator-derived reservation | none — pinned by `admission/tests.rs:333-347` |
| 3 persist Idle→Preparing | `WriteAdmissionScratch(preparing)` [+ `RetireAdmissionRecord` iff an active record is resident] + `PublishAdmissionRecord` | scratch flush+parent flush; unlink+parent flush; rename+verify+parent flush |
| 4 create the one indexed staging directory | `CreateStagingDirectory` (`:206-218`): `create_dir` (no-replace) + no-follow reopen + identity + parent flush | one mkdir |
| 5 write and flush the resident reservation | `WriteResidentReservation` (`:224-246`) into staging under the frozen `ActionSlotV1` name | P2 full write-verify chain |
| 6 publish staging → final, no-replace | `PublishStagingAction` (`:252-309`): staging flush, handle released (Windows sharing precedent `directory_mutation.rs:237-243`), sealed publish with **both C-2 arms**, identity reproof, parent flush | one rename |
| 7 persist Preparing→Idle | same triple as step 3 with `idle()` | as step 3 |
| 8 reobserve the complete catalog | next loop iteration: `CompletedCatalogPermitV1::observe_admission` **revalidates the lease/root binding and the exact retained catalog before every observation and every edge** (`pre_catalog.rs:108-131`) | none |
| 9 handoff only from idle + missing staging + exact final + no extra children | `owner.admit` (`protocol/admission/owner.rs:114-128`), `exact_identity_for` requires `Exact` reservation and `extra_children: 0` | none |

No reordering found. No skipped durability point on the record path (each
scratch write is create/truncate → write → `sync_all` → open-file verify →
named-file verify → parent flush). No instant is readable as both Idle and
Preparing: `resolve` is total and single-valued, the active slot is the sole
authority, and the two-Exact states not on the transition path stop. Retry
reuses names structurally — every name is a compile-time slot constant or the
derived `action-<hex>-v1` (nonce impossible by construction; pinned by
`a_retried_admission_reuses_the_deterministic_names_and_never_chooses_a_nonce`).

Deviations from the *letter* of §7, both already adjudicated: step 3/7 is
three physical edges, not one (§1.2, admitted); the retire sub-edge is skipped
when the active slot is absent (§1.3, admitted — there is nothing to retire).

Liveness: `MAX_ADMISSION_STEPS = 24` (`driver.rs:32`) against a worst legal
path of 10 iterations (9 edges + terminating observation, non-virgin); the
resolve/classify graph has no cycle absent fresh external mutation, and a
persistent I/O failure propagates immediately.

## 3. Crash closure — the 19-key matrix and the adjudicated deviation

**Coverage.** The 19 `admission.*` keys (`fault_v1.rs:78-96`) decompose as: 2
observation boundaries, 3×3 create/write/flush boundaries for the three
durable record writes (preparing scratch, idle scratch, resident reservation),
2×2 publish/reobserve boundaries for the two record installs, and
staging-create / staging-flush / final-publish / final-reobserve.
`reconcile_executed_keys` (`tests_fault_matrix.rs:235-250`) pins matrix rows ==
family inventory, so a 20th key without a row fails the suite. Each row: real
process stop at the boundary, fresh-process restart, convergence to the
settled row set, scratch consumed, and a second settle that mutates nothing —
executed on **both** target variants (workspace, git-directory). Counts held
at 19/165; activation flipped Reserved→Executed as a deliberate fixture edit
(`fault_expected_keys.rs:203-209`, `:442-461`).

**Boundary-without-a-key:** the `RetireAdmissionRecord` unlink+parent-flush is
a durable boundary of the sequence with **no injection site** — the matrix's
own claim that "an interruption is a real process stop across a real durable
edge rather than a reconstructed on-disk state" (`tests_fault_matrix.rs:13-15`)
does not hold for this one edge; its post-state is covered only by the on-disk
window row. Durable-state closure is therefore complete, but the evidence
mechanism and the frozen 19-key census lag the composition. Finding **P2-1**.

**The 12-round selection is correct.** `admission.preparing_scratch_create`
and `admission.staging_flush` (`tests_fault_matrix.rs:336-396`,
`REPEATED_CRASH_ROUNDS = 12` vs. 8 nominal edges/rows) are exactly the two
*mutating* boundaries whose crash state resolves back to the same edge; the
two observation keys are also trivially re-crossable but mutate nothing, so
repeating them proves nothing about slot stability. Both rounds assert a
frozen durable slot census every round (root rows + staging interior), bound
growth to one in-flight row, and require the staging interior to be empty or
exactly the derived reservation row — the R2 stop clause proven under
repetition, on both variants.

**Adjudication of the reported deviation — the scratch-WRITE boundaries are
non-repeatable in this harness: ACCEPTED as durable-state-true.** The harness
kills after the syscall, so the record is complete and restart advances — a
harness property, not a state property. Under real power loss a torn scratch
*does* re-cross the boundary, and that path is closed by construction:

- any torn shape (absent / empty / partial / garbage) decodes to `Other` or
  `Missing`, and every such state routes back to the same truncate-rewrite of
  the same compile-time slot name (`(Missing|Idle, Missing|Other)` → step 3;
  `(Preparing, Missing|Other)` → `classify_handoff` →
  `ReplacePreparingWithIdle` when the final is exact) — the boundary re-enters
  with zero name allocation;
- the worst torn shape **is** exercised twelve times: the
  `preparing_scratch_create` rounds leave a present-but-empty scratch each
  round, and the census stays frozen;
- decode-ambiguity is foreclosed: a valid `ActionDirectoryAdmissionV1` must
  carry a self-digest **and** re-encode byte-identically
  (`protocol/admission.rs:276-281`), so no proper prefix or corruption can
  alias a different valid record;
- torn *reservation* shapes classify as `PartialExpectedPrefix` under
  `classify_expected_prefix` and route to `WriteOrRewriteReservation`
  (empirically: the `reservation_create` matrix row leaves an empty resident
  reservation and converges on both variants).

No matrix row was owed. A single-shot non-empty-undecodable-scratch row would
complete the shape set cosmetically — P3-4, informational.

## 4. C-3 widening soundness

- **Refusals retained, byte-identical.** `exact_row` (`interior.rs:370-394`)
  keeps the platform-equivalent alias refusal ("platform-equivalent
  infrastructure alias is noncanonical", `:379-383` — case twins on
  insensitive and sensitive filesystems both die here or below) and the
  unowned-child refusal ("catalog directory contains an unowned child",
  `:390-393`). The only newly admitted child is a byte-canonical
  `RootEntryNameV1::ActiveAction` row; `MalformedRecognized` (e.g.
  `action-00-v1`, `catalog-format-v2`) and `Foreign` still refuse at
  observation. Duplicate-slot and duplicate-action-row ambiguity refusals
  added/kept (`:111-123`). Uppercase twin of an action row on a sensitive
  filesystem → not byte-canonical → unowned-child refusal; on an insensitive
  filesystem it cannot coexist.
- **The five classes are the frozen five.** `CatalogRootRowClassV1`
  (`slots.rs:459-466`) = Infrastructure + exactly the plan Step 1.2 five for
  non-infrastructure rows {ActiveAction, ScheduledScratch, Retired,
  MalformedRecognized, Foreign}; classification is derived from
  `RootEntryNameV1::parse` alone (no name minted);
  `interface_tests/bounds_slots.rs` pins the vocabulary case-by-case.
- **Zero-headroom accounting:** `MAX_INTERIOR_ENTRIES = MAX_ROOT_ENTRIES`
  (= 74, `interior.rs:40`, `bounds.rs:21-23`) with the partition enforced
  per-family — 65th action row refuses (`:85-87`), 11th infrastructure row
  refuses (`:95-99`), 75th entry refuses (`:79-81`) — pinned by
  `the_widened_interior_bound_is_the_frozen_root_entry_budget`. **However the
  mandate's clause "cannot be over-filled by admission rows (capacity refusal
  fires first)" FAILS: no capacity refusal exists anywhere in the admission
  path, and the observation bound fires only after the over-publish — finding
  P1-1.**
- **`completed_record` drop is exactly right; `staging_plan` drop is wider
  than the need.** `completed_record` (`interior.rs:309-340`) drops exactly
  the triad and keeps the `CatalogAnchorB` refusal — required for both
  recovery and future retirement of a used catalog, and admission-slot
  *validity* correctly remains the driver's judgment (a wrong-kind or garbage
  triad row leaves the catalog complete and stops admission as `Other`/
  ambiguous — fail-closed at the right layer). `staging_plan`
  (`interior.rs:177-191`) also drops the triad (at `c40e712` both functions
  refused on `[CatalogBootstrapRetired | CatalogAnchorB, ActionAdmissionActive,
  ActionAdmissionScratch, ActionAdmissionStaging]`, old `:128-138`/`:262-272`)
  — but no cooperating history ever places admission slots inside a
  *bootstrap-staging* interior. Finding P2-2.
- **Census:** six counters matching the §6 grammar
  (`protocol/admission.rs:438-467`); action rows join the within-lease
  freshness digest (`snapshot.rs:224-233`) so row substitution is caught like
  infrastructure substitution. The driver's `has_unowned_row()` stop
  (`driver.rs:45-48`) is structurally unreachable because `exact_row` refuses
  first — P3-2, defensive dead guard, posture unchanged.

## 5. §3.1 home pin — held

The record persists **only** through the three frozen `ActionAdmission*`
slots and the derived `ActiveAction` row: every durable name in
`admission_mutation.rs` is `InfrastructureSlotV1::ActionAdmission{Active,
Scratch,Staging}.name()`, `RootEntryNameV1::ActiveAction(digest).name()`, or
the frozen `ActionSlotV1` reservation name. `protocol/generated.rs` is
untouched by the diff; no new slot, record, purpose, phase, or fault key
(19/165 held; `EXPECTED_KEY_COUNT` unchanged). All new types
(`ActionAdmissionEdgeV1`, `CatalogRootRowCensusV1`, `CatalogRootRowClassV1`,
`ActionAdmissionObservationV1`, `DirectoryInteriorExpectationV1`,
`AdmissionRecordRowV1`) are in-memory, lifetime-bound or Copy, with no encode
path and no reachability from a durable-record root. Track-W is not
triggered. The C-2 arms landed as the freeze §4.4 prescribed: the
source-interior arm as a generalization of the struct's `expected` field
(`publication.rs:36-56`), the destination arm as a new enum variant
(`:76-88`), verification in `interior.rs` of the same owner
(`observe_action_directory`/`observe_action_interior`, `:458-544`), caller
pins and digests extended deliberately in the same commit
(`CATALOG_PUBLICATION_CALL_COUNTS` +`admission_mutation.rs: 2`;
`capability_permit.rs` 6→8 with the freeze-clause comment). Conformance note:
the Phase-1 destination arm serves E3/E4; E7's retirement-destination variant
remains owed with its edge (freeze §4.4 "any further retirement-destination
arm", Phase 4).

## 6. Composition

- **The freeze-recorded C-3 breakage chain is closed and exercised.**
  `recover_or_create` → `execute_owner_complete` → `retain_completed_catalog`
  → widened `completed_record`; every `attempt()` in every suite re-runs
  `recover_or_create` against a catalog carrying settled admission state
  (resume test, second-action test, every matrix restart) — 12/12 green.
- **The R1 classifier is consumed, not duplicated:** `classify_handoff` and
  `admit` are the sole deciders for staging/final and for issuance
  (`driver.rs:61-66`, `:81-105`); §7 step 9's predicate is enforced inside
  `admit` (`owner.rs:114-128`).
- **Visibility widening blast radius:** `ObservedActionDirectoryV1{,::exact}`
  and `CatalogAdmissionOwnerV1::{new,classify_handoff,admit}` widened
  `pub(super)` → `pub(in crate::checked_artifact)` (`owner.rs` diff). A
  checked_artifact-internal sibling could now synthesize observations and
  mint an `AdmittedActionV1` without touching a filesystem. No handle or
  mutation capability crosses (amendment §7 boundary intact), the widening is
  the minimum Rust visibility that lets the physical half consume the
  classifier, and today's production namers are exactly `admission/driver.rs`
  and `provider/interior.rs` — but nothing pins that set. P3-3.
- **Seam discipline held elsewhere:** driver holds zero injection sites and
  zero mutation (decides only); all 19 sites live in the one owner-private
  mutation file; edges execute behind
  `CompletedCatalogPermitV1::{observe_admission,execute_admission_edge}` with
  `revalidate()` first (`pre_catalog.rs:108-131`) — the ready-edge prologue
  applied to the completed permit.

---

## Findings

### [P1-1] Nothing refuses the 65th active action; the over-publish permanently bricks the catalog

**Where:** `admission/driver.rs:58-80` (Idle arm — no census consult before
`WriteAdmissionScratch(&preparing)`); `publication.rs:215-235`
(`AdmissionCatalogInterior` recheck — verifies row-absence, completed record,
retired record, but not the action-count bound);
`protocol/bounds.rs:144-155` — `CatalogOccupancyV1::can_admit_new` /
`can_resume`, the R1 capacity vocabulary for exactly this refusal, has **zero
production callers** on this tree; `admission_mutation.rs:68` — the
`admission.capacity_check` key names a boundary at which no check exists.

**Sequence (no crash required, all steps legal through the frozen seam):**
64 distinct actions admitted and not yet retired (retirement is Phase 4;
nothing in Phase 1 removes an active row). The 65th `resume_or_admit`:
observation passes (`action_rows.len()` reaches exactly 64; the `:85-87` cap
fires only when *pushing* a 65th row), drive Idle, final Missing → the full
sequence executes; at `PublishStagingAction` the destination recheck
re-observes a root still holding 64 action rows → passes → the rename lands
the **65th** `ActiveAction` row. The same loop's next observation (step 8)
now errors `interior_bound_exceeded` — and so does **every** subsequent
`interior::observe`, including `recover_or_create` →
`retain_completed_catalog`'s. The catalog is durably unrecoverable through
every sealed path; no driver edge can remove an action row. Resume of
existing actions, bootstrap recovery, and retirement (when it lands) are all
behind the failing observation.

**Why P1 and not P0:** reachable only after 64 accumulated live admissions;
`resume_or_admit` has no production caller at this tree (R2-E not landed, A1
off), no false authority is minted and no payload corrupted. But it is a
durable dead-end reachable by legal use of the frozen seam, it falsifies the
zero-headroom design premise the C-3 widening rests on ("capacity refusal
fires first"), and the settle gate cannot accept a kernel that can wedge its
own catalog. Blocks settle.

**Minimal remediation (~10 LOC + tests, inside the Phase-1 owners):**
(a) in the driver's Idle arm, after the failed `admit` and the
final-conflict stop, refuse a **new** sequence when
`observed.census.active_actions >= MAX_ACTIVE_ACTION_DIRS` (typed stop;
resume-at-64 stays legal because `admit` runs first); (b) one line in the
`AdmissionCatalogInterior` arm: when `absent` is an `ActiveAction` row,
require `fresh.action_rows.len() < MAX_ACTIVE_ACTION_DIRS` — this is the
race-free structural guarantee at the commit point and also closes the
resumed-in-flight path (a Preparing admission resumed after other admissions
filled the root); (c) a 64-full acceptance case (bulk-place 64 grammar-legal
action rows on disk, assert the 65th admission stops typed, assert resume of
an existing action still succeeds, assert `recover_or_create` still
recovers). Wiring the full `CatalogOccupancyV1` retirement-credit inequality
requires a bounded retired-root count the current observation does not carry;
record it as owed to the phase that lands retirement (Phase 4) — it cannot
brick the catalog the way the active bound can.

### [P2-1] The retire edge is outside the frozen physical-edge inventory and has no fault key

**Where:** `admission_mutation.rs:153-160` — `retire_admission_record` is
`final_directory.remove_file(ActionAdmissionActive) + sync_directory_edge`.
This is the **only** namespace removal in the sealed provider tree
(`git grep remove_file` over `provider/` + `catalog/` at `c13f773`: one hit),
i.e. a first-of-its-kind physical edge shape. The freeze's §4.3 E-table has no
removal row (E4 is "P2 + P1"; E7 is retirement-by-rename), the Track-P family
table P1-P5 has no removal family, and the checkpoint's own framing
("retire-then-publish **over the no-replace primitive**") oversells — only
the publish is over the primitive; the retire is a raw unlink. Separately, no
`admission.*` key sits between the unlink and the publish, so the matrix's
"real process stop across a real durable edge" property
(`tests_fault_matrix.rs:13-15`) does not hold for this edge; its window is
covered only by the on-disk reconstruction (`driver/tests.rs:187-194`), and
§3.5's 19-key census was frozen before the composition added this boundary.

**Durable-state assessment (why this is P2, not P1):** the edge is
crash-closed — same-name exclusion orders it against the publish on every
supported filesystem (§1.2 point 3), a lost unlink re-presents W1, and the
Windows delete-pending wrinkle degrades to a typed refusal plus restart
convergence, never to corruption (a lingering name fails the no-replace
publish or the delete-pending open; both stop; the state remains W1/W2).
The defect is inventory/closure discipline, not soundness.

**Minimal remediation (either closes it):** (i) a one-row freeze erratum
(E4b: "admission active-record retire — unlink + parent flush", per-platform
cells incl. the Windows delete-pending disposition) **plus** minting
`admission.record_retire` in a deliberate activation-map amendment (19→20,
165→166 — the fixture is built to make this a reviewed edit) with its matrix
row (non-repeatable class: crash leaves the active absent and restart
advances via `InstallScratch`); or (ii) the same erratum recording the
accepted no-key deviation with the on-disk window row cited as the covering
evidence. Option (i) is stronger and cheap.

### [P2-2] `staging_plan`'s C-3 drop widens the bootstrap's adoption grammar with no flow that needs it

**Where:** `interior.rs:183-191` — the widening comment covers both
functions, but only `completed_record` had the recorded breakage chain. At
`c40e712`, `staging_plan` refused any staging interior carrying
`ActionAdmission{Active,Scratch,Staging}` (old `:128-138`); now only
`CatalogBootstrapRetired` refuses. Admission never writes into a
bootstrap-staging interior (it runs only against a complete catalog's root),
so the dropped refusal guarded a state only out-of-band mutation can produce
— and the R1 posture refused it anyway (fail-closed beyond the trust
boundary). **Concrete sequence now accepted that was refused before:** a
staging directory planted with a stray-but-valid admission record alongside
the six exact roles now classifies `Complete(_)`, passes the CatalogStaging
source-interior recheck (`publication.rs:176-186`), and publishes as a live
catalog carrying an unexplained admission row.

**Minimal remediation:** restore the triad to `staging_plan`'s refusal list
(one line); zero cooperating history changes classification;
`completed_record` keeps the drop it actually needs.

### [P3-1] The `(Idle, preparing)` durable state is never restart-entered

The pre-retire window of a **second** admission's step 3 (active idle
resident, preparing scratch durable) is crossed only inside the uninterrupted
`a_second_distinct_action_admits_...` settle. The matrix is virgin-per-key,
so its `preparing_scratch_flush` row leaves `(Missing, preparing)`, and the
window suite covers only the step-7 flavors. The resolve arm is shared
(`(Idle, Preparing) | (Preparing, Idle)`) and the `(Preparing, Idle)` side is
matrix-restarted, so convergence follows; a direct row (one more
`InstallWindowV1` with `active: Some(&idle), scratch: Some(&preparing)` —
note it must assert continuation to full settle, not just install) closes the
gap cheaply.

### [P3-2] The driver's census stop is structurally unreachable

`driver.rs:45-48` stops on `census.has_unowned_row()`, but `exact_row` errors
on every malformed/foreign child before the census can be charged
(`interior.rs:82-94` — the `.expect("a classified non-action catalog row owns
an infrastructure slot")` also relies on that refusal to keep
`Foreign`/`MalformedRecognized` out of the non-action path). Harmless
defense-in-depth; keep it, but comment the invariant (and that the `expect`
is guarded by `exact_row`'s refusal), or assert it in a unit test, so a
future `exact_row` widening cannot silently turn the panic path live.

### [P3-3] The classifier visibility widening has no namer pin

`pub(in crate::checked_artifact)` on `ObservedActionDirectoryV1::exact` and
`CatalogAdmissionOwnerV1::{classify_handoff,admit}` lets any internal module
synthesize observations and mint `AdmittedActionV1` (judgment call (i),
accepted — it is the minimum visibility that connects the frozen halves, and
reservations were already internally constructible). Add an
`interface_tests` pin (the `FAULT_INJECTION_SOURCES` idiom) enumerating the
production files allowed to name `CatalogAdmissionOwnerV1` and
`ObservedActionDirectoryV1::exact` — today `admission/driver.rs`,
`provider/interior.rs`, and the protocol owner — so a new namer is a
deliberate, reviewed edit.

### [P3-4] (informational) Torn-scratch shape set

The write-boundary non-repeatability deviation is adjudicated true (§3). The
empty-file torn shape is 12-round-proven; a single-shot row with a non-empty
undecodable scratch (e.g. truncated canonical bytes) would make the torn-shape
set visibly exhaustive. No behavior change expected (`Other` → rewrite).

---

## Dispositions of the three queued judgment calls

1. **Visibility widening** — ACCEPTED; P3-3 asks for a namer pin.
2. **E4 as retire-then-publish** — ADMITTED, no re-spike; §1.2 is the
   admitting record; P2-1 requires the edge-inventory/key closure for the
   retire half.
3. **Absent-active ≡ idle** — ADMITTED with the §1.3 scope caveat
   (state-predicate, never record-presence, in all future pins/evidence).

LOC disposition (~768 vs the aspirational <500): noted as recorded by the
lane owner with the structural cause (both extension classes assigned to
Phase 1 by the round-2 freeze); no state-axis objection — the budget is
aspirational by its own definition and no §16-22 stop rule fires.

## Verdict

**NO-GO** — 1 × P1 (capacity refusal absent; over-fill bricks the catalog),
2 × P2, 4 × P3, 0 × P0. The transition kernel itself is adjudicated sound on
all three queued judgment calls; remediation of P1-1 (and the P2 pair) is
round-2 scope, within the two-round cap. The E4 and absent≡idle admitting
arguments in §1 are entered as the durable record and survive the
remediation unchanged.

---

# Focused re-verdict (round 2)

Date: 2026-08-22. Axis: STATE. Object of this re-verdict: gwz-core
**`bf438ed`** ("Apply the Phase 1 settle round-2 remediation", local,
UNPUSHED; base `c13f773` + the remediation files only:
`admission/driver.rs`, `admission/mod.rs`, `admission/tests.rs`,
`admission/driver/tests.rs`, `admission/tests_fault_matrix.rs`,
`admission/tests_namer_pin.rs` (new), `provider/interior.rs`,
`provider/publication.rs`, `protocol/admission.rs` (stale
`#[allow(unused_imports)]` removed only), the checker's pre_catalog tree
digest), plus the dated freeze append committed at workspace root
**`a99b77e`** (`GwzM5-8R2DInterfaceFreeze.md` §4.3 activation record).
Peer-blind vs the Code report maintained. All file:line positions in this
section are at `bf438ed`.

The gwz-core working tree now carries four other lanes' uncommitted files
**inside `src/checked_artifact/`** (provider.rs, completed.rs, namespace/*,
leaf files), so in-tree runs are not evidence for the committed object.
Verification therefore executed on a **pristine extraction** (root `20f1654`
archive + gwz-core `bf438ed` archive + sibling member archives, fresh target
directory):

```text
python3.13 scripts/checks/check_checked_artifact_boundaries.py
    → checked-artifact boundary: ok (15 visible entries, 5 classified
      modules); exit 0 — fully green on pristine (round 1's in-tree noise was
      the other lanes', as recorded)
cargo test --lib checked_artifact::admission::
    → ok. 15 passed; 0 failed (12 round-1 suites + the 2 capacity cases +
      the namer pin; matrix and 12-round suites on both target variants)
cargo test --lib checked_artifact::
    → ok. 274 passed; 0 failed — reconciles exactly: 271 at c13f773
      + 2 capacity + 1 namer pin
```

## Per-finding disposition

### [P1-1] — RESOLVED

Both refusal paths landed per the round-1 sketch, plus the tests:

- **(a) driver gate** (`admission/driver.rs:111-114`): in the Idle arm,
  placed **after** `owner.admit` and after the final-conflict stop and
  **before** the first durable write, refusing a new admission when
  `observed.census.active_actions >= MAX_ACTIVE_ACTION_DIRS` with the exact
  typed stop "the catalog root already holds the frozen active-action
  budget". Resume-of-existing at full capacity is untouched by construction
  (`admit` returns first — pinned by the test below).
- **(b) commit-point recheck** (`provider/publication.rs:222-244`): the
  `AdmissionCatalogInterior` arm computes `full` **only for `ActiveAction`
  destinations** (`fresh.action_rows.len() >= MAX_ACTIVE_ACTION_DIRS`) and
  refuses before the rename with "publication would exceed the frozen
  active-action bound". Two subtleties verified correct: Infrastructure
  destinations (the E4 record publish) never trip it — blocking record
  installs at 64 actives would have wedged step-3/7 settles at capacity —
  and the refusal fires inside the acquisition window, race-free, with the
  staging directory left intact for a later retry.
- **(c) tests** (`admission/tests.rs:496-663`):
  `a_full_catalog_root_refuses_a_new_admission_and_still_resumes_and_recovers`
  fills the root to exactly 64 grammar-legal rows, asserts the 65th refuses
  with the exact typed message and **zero mutation**, asserts
  `recover_or_create` still recovers and the resident action still resumes
  byte-identically at full capacity, then frees one row and asserts the
  refused admission completes and settles idle.
  `a_resumed_in_flight_admission_refuses_at_the_publish_when_the_root_filled`
  reconstructs the Preparing-installed in-flight state on disk, fills the
  root, and asserts the refusal comes from the destination recheck (exact
  fact/detail), root unchanged, staging intact. Both use the new
  `stopped_with` so neither can pass on an unrelated ambiguity.

**Ruling on the recorded limitation (`can_admit_new` still unwired): NOT a
gap — the deferral is the only coherent choice at this phase.** The
retirement-credit inequality needs a bounded retired-root count, and
`completed_record` requires the `RetiredActions` root **empty**
(`provider/interior.rs:349` inside `completed_record` `:333`), so no catalog
this kernel can observe carries a nonzero retired count: the inequality is
vacuous until the phase that lands retirement widens `completed_record`,
which is exactly where the driver comment records the debt
(`driver.rs:85-110`) — matching this report's own round-1 disposition
("record it as owed to the phase that lands retirement"). Unlike the active
bound, exhausting retirement credit cannot make the catalog unobservable.
Consulting `census.active_actions` directly is complete for Phase 1.

The round-1 P1 sequence is now closed at both ends: the 65th admission
refuses before its first durable write, and the one path that bypasses the
new-admission gate (a resumed in-flight publish) refuses at the commit
point. The catalog can no longer reach the unobservable state through any
admission edge.

### [P2-1] — RESOLVED via route (ii), which this re-verdict RATIFIES; no key mint owed

The dated activation record ("Activation record 2026-08-22 — E4's retire
half, a first-of-kind removal edge", freeze §4.3, committed at root
`a99b77e`) is append-only (the frozen E-table is not reshaped), names both
axes' finding, states the physical shape and location of E4-retire, and
carries the argument chain — **every leg of which was re-verified here**:

- *no admitted family covers removal* — true (P1-P5 inventory, §4.2 spike
  exercised no unlink);
- *no in-vocabulary rename destination exists* — **verified**:
  `completed_record` requires an empty `RetiredActions` root
  (`interior.rs:349`), so retiring the record *by rename* would un-complete
  every catalog; minting a destination is barred by freeze §6/§3.1. Removal
  is what remains;
- *write-ahead protection, commit-at-publish, idempotence,
  namespace-enforced ordering against the publish, Windows delete-pending
  degrading to typed refusal + restart convergence* — all match this
  report's own §1.2/§P2-1 analysis, and the record corrects the
  checkpoint's overstated "retire-then-publish over the no-replace
  primitive" shorthand explicitly;
- *convergence evidence* — now covers **both record-kind variants of the
  post-retire window on disk** ("retired, scratch idle" and the new
  "post-retire, scratch preparing" with the action row not yet published)
  plus both pre-retire windows ("preparing, scratch idle", new
  "idle, scratch preparing"), each re-entered as a fresh process
  (`driver/tests.rs`, 15/15 green on pristine).

**Ruling (i) vs (ii): route (ii) is sufficient and is ratified; the
`admission.record_retire` mint is NOT owed for the settle.** What a minted
key would add over the delivered evidence is only the kill mechanics at a
boundary whose post-state is already directly re-entered from disk in both
variants — the production retire edge itself executes on every step-7
crossing throughout the matrix and suites, and its two loss modes (lost
unlink → pre-retire window; lost publish → post-retire window) are both
covered states. The frozen census stands at **19/165**; no freeze edit is
requested. Route (i) remains open to the lane as *optional* hardening if a
later phase wants the matrix property restated as universal, but nothing is
owed. The matrix's own 12-round doc block now also names its exclusions
honestly (`tests_fault_matrix.rs:323-350`) and the 12-round set was widened
to a third boundary (`admission.reservation_create`, the empty-resident-
reservation `PartialExpectedPrefix` loop) on both variants — beyond what
round 1 asked.

### [P2-2] — RESOLVED

`staging_plan`'s triad refusal is restored **verbatim** — the refusal list
is again `[CatalogBootstrapRetired, ActionAdmissionActive,
ActionAdmissionScratch, ActionAdmissionStaging]` (`interior.rs:203-211`),
byte-equal in effect to `c40e712`'s list — with a comment that records why
the C-3 widening is deliberately not applied to the bootstrap-staging
adoption grammar. `completed_record` keeps the drop it needs. Regression:
the full 274-test partition (including every catalog_bootstrap suite) is
green, so no previously-accepted interior changed classification.

### [P3-1] — RESOLVED

The `(Idle, preparing)` durable state is now restart-entered directly:
window row "idle, scratch preparing" (`driver/tests.rs:210-225`) with
`installs: true` demanding continuation to full settle. The remediation
additionally added the unpublished-final flavor ("post-retire, scratch
preparing", `:263-330`), converting round 1's virgin-state-identity
argument into direct evidence.

### [P3-2] — RESOLVED

The invariant is commented at both ends, each naming the other and the
future-widening obligation: the `exact_row`-refusal guarantee at the
`interior.rs:92-98` `expect` site, and the deliberately-unreachable census
stop at `driver.rs:46-51`.

### [P3-3] — RESOLVED

`admission/tests_namer_pin.rs` (new, wired at `admission/mod.rs:61-62`) pins
the namer set in the completeness-anchor idiom — a **rescan** of
`src/checked_artifact/` under the production-file rule, not a trusted list —
so a new namer of `CatalogAdmissionOwnerV1`/`ObservedActionDirectoryV1` is a
reviewed edit; substring over-catching fails closed. The declared set is
round 1's three (driver.rs, provider/interior.rs, protocol/admission.rs)
plus the classifier's own module tree (`protocol/admission/owner.rs`,
`protocol/admission/test_support.rs`) — legitimate: the definition site and
its `#[cfg(test)]` fixture could name the items at `pub(super)` and are
unaffected by the widening. Green on pristine. The location outside
`interface_tests/` is an ownership artifact, recorded honestly in the file
header with relocation named a lane-owner decision — the property is
mechanical and identical from either home; relocation is file-and-continue,
not a finding.

### [P3-4] — RESOLVED

The non-empty undecodable torn-scratch row ("scratch undecodable",
truncated canonical `preparing` bytes, `driver/tests.rs:271-283` and the
loop below) converges through the truncate-rewrite of the same compile-time
slot name, completing the torn-shape set alongside the 12-round empty-file
shape.

## New findings introduced by the remediation

None. Sweep performed: the `>=` gate direction; census counting of
wrong-kind action-named entries (counted toward capacity by name — the
fail-closed direction); the `(occupied, full)` split sparing Infrastructure
destinations (required — see P1-1(b)); the wedged-in-flight-at-capacity
liveness shape (an out-of-band-only state; fail-closed stop semantics
pre-date this package and were accepted in round 1; resolves when headroom
returns); the namer-pin scan rule mirroring `production_rust_files`; the
checker digest refresh verified against committed content on pristine
(round 1's incident lesson held); the `protocol/admission.rs` edit is the
stale allow's removal only.

## Re-verdict

**GO** — all seven round-1 findings resolved (1 × P1, 2 × P2, 4 × P3 →
closed); route (ii) ratified on the key-mint question with the 19/165
census standing; zero new findings; 0 × P0/P1/P2/P3 open on the STATE axis.
The §1 admitting arguments (E4 composition, absent≡idle) carry forward
unchanged as the durable record. Within the two-round cap: this axis's
settle gate is satisfied at `bf438ed` + root `a99b77e`.
