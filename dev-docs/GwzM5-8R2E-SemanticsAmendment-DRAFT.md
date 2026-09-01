# R2-E E0.2 — the semantics amendment (DRAFT, 2026-08-27)

Status: **LANDED 2026-08-27 at the E0 close — read WITH the E0.2b addendum
(`GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md`), which is CONTROLLING over this
file wherever they differ.** *(Was: DRAFT for E0.3 dual #1.)* Author: the
implementation lane (E0.2 drafter). Object: `GwzM5-8R2DInterfaceFreeze.md`
§3.5 and §4.3.
Plan: `GwzM5-8R2E-Plan.md` §3 phase E0, step E0.2.
Inputs consumed: `GwzM5-8R2E-E01ReachTraces.md` (both answers),
`GwzM5-8R2DSettledTuple.md` §11.1/§11.3, `GwzM5-8R4bG-Evidence.md`
§12.7-§12.9, `GwzM5-8I2CompatibilityContract.md` §4-§5,
`GwzM5-8A1ActivationRecord.md` §6 (L6).

Traced at the working tree of gwz-core under `/Users/owebeeone/limbo/gwz-dev`
(the v0.11.0 surface; no production source read here differs from the tag per
E0.1's own statement). **Every `file:line` below was opened and read.**

---

## 1. Object and anchors

### 1.1 What this amendment does

It installs **injection semantics** for the **38** fault keys the R2-D freeze
re-reserved to R2-E — `cleanup.*` 11, `barrier.*` 16, `terminal.*` 11 — as
three dated, quote-preserving §3.5 activation records, plus the §4.3 E-table
annotations they imply, plus three decisions the plan routed to E0.2 (the O6
observe-or-refuse resolution, the E0.1(a) blast-radius decision, and the O8
archive-equivalence mechanism decision).

**It binds semantics to keys that already exist. It moves no census.**
`EXPECTED_KEY_COUNT` stays **165** (`fault_expected_keys.rs:174`; the key list
ends `:172`). No key is minted, none retired. The three families' key strings
are already in the vocabulary at `fault_v1.rs:132-147` (barrier),
`:178-188` (cleanup), `:189-199` (terminal), and already in the fixture's
expected list at `fault_expected_keys.rs:151-172`.

### 1.2 The three texts this amendment discharges, quoted verbatim

These are the anchors. They are **left as written**; this amendment is an
annotation beside them, per the map's own sanctioned mechanism.

**(a) `cleanup.*` — freeze `:707`-`:733`.** The row:

> | `cleanup.*` | 11 | R2-D **Phase 4** (step 4.1 legacy leaf edges) | reserved |

The record's semantic sentence (`:712`-`:721`):

> The `cleanup.*` vocabulary names a different edge set: five keys are the
> `CleanupWorklistV1` record's own scratch/publish/reobserve lifecycle
> (`protocol/cleanup.rs`, `BaseActionSlotV1::CleanupWorklist`), and the remaining
> six are the retirement of the three `CleanupAliasV1` aliases — Source, Goal,
> Authority — into their `Retired*Alias` slots with their source/destination
> reobservations and row/completion records (`namespace/mod.rs`
> `cleanup_retirement`, `retire_exact`). None of those edges exists on this tree
> outside the R1 vocabulary: every one requires an `AdmittedActionV1` and a
> scheduled action directory, so their conversion is a consumer conversion, which
> plan §5 items 1 and 2 place outside R2-D.

The re-reservation (`:726`-`:733`):

> Per the map's own carried clause ("any key whose edge genuinely converts later
> is explicitly re-reserved for R2-E/R2-F in the same update"), all eleven
> `cleanup.*` keys are **re-reserved for R2-E**, the lane that gives the cleanup
> worklist its first production consumer; the row above is left as written, since
> this annotation is the sanctioned mechanism and the family's Phase-4 assignment
> was made against the step, not against the edges. RemPlan §10's duty is not
> deferred by this record — it never attached, because Step 4.1 converts no
> `cleanup.*` edge.

**(b) `barrier.*` — freeze `:763`-`:790`.** The row:

> | `barrier.*` | 16 | R2-D **Phase 4** (step 4.2 Windows retirement closure) | reserved |

The record's semantic sentence (`:768`-`:781`):

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

The re-reservation (`:786`-`:790`):

> Per the map's carried clause, all sixteen `barrier.*` keys are **re-reserved for
> R2-E**, the lane that gives the roaming anchor its first admitted action.
> RemPlan §10's duty is not deferred by this record — it never attached, because
> Step 4.2 converts no `barrier.*` edge.

**(c) `terminal.*` — freeze `:791`-`:799`, with `:873`-`:882`.** The row:

> | `terminal.*` | 11 | R2-D **Phase 4** (step 4.2, terminal retirement edges) | reserved |

The record (`:792`-`:799`):

> `terminal.*` non-activation record (2026-08-23, Step 4.2 landing; resolves the
> plan's own conditional, "and `terminal.*` if its edges convert here"): **0 of
> the 11 keys are executed; all 11 stay reserved, re-reserved for R2-E.** The
> conditional resolves to *no*. Every key of the family names the admitted action
> directory's terminal retirement into the catalog's retired root — §4.4's "E7's
> Phase-4 half and the terminal retirement edges" — and each needs an
> `AdmittedActionV1` and a production catalog, which plan §5 item 2 forbids in
> R2-D. No edge of the legacy anchor touches one.

And the §4.4 arm-table resolution it carries (`:873`-`:882`):

> **§4.4's arm table row "any further retirement-destination arm | Phase 4"
> resolves to NO ARM for this step** … On this tree that row is therefore driven
> by E7's Phase-4 half and the terminal retirement edges alone — both
> `terminal.*`'s, re-reserved for R2-E above. The row is left as written, since
> this annotation is the sanctioned mechanism.

### 1.3 The house form these records follow

Calibrated against the **admission** activation record — freeze `:481`:

> | `admission.*` | 19 | R2-D **Phase 1** (R2-C3 admission) | **executed** (Step 1.3, 2026-08-22 — 19 sites in `capability/pre_catalog/provider/admission_mutation.rs`, matrix on both target variants, 12-round same-boundary crash stability; counts held at 19/165, no key minted) |

and against the two records that most resemble what R2-E must write — the
Step-3.1b `managed_bootstrap.*` annotation (freeze `:602`-`:635`) and the
Step-3.2 one (`:637`-`:690`). Their shared shape, which §2-§4 below reproduce:
dated; family-scoped; row left verbatim; per-key site named in an
**owner-private mutation file**; matrix named; both target variants;
repeated-boundary rounds; single-crossing classification declared per row;
counts restated; caller pins restated.

**The driver-holds-zero rule**, which the sites below obey:
`admission/driver.rs:8-9` — "The driver decides; it never mutates" — and the
fixture's own statement at `fault_expected_keys.rs:407-409`:

> `admission/driver.rs` deliberately holds none: it decides and never mutates
> (`admission/driver.rs:8-9`), so every durable admission edge is announced from
> the owner-private mutation file.

The same rule already holds for `namespace/host.rs` (fixture `:444-448`) and
for the managed intent lifecycle (`host.rs:282-283`, in-code: "This owner still
holds no injection site: the boundaries are announced from `managed_mutation.rs`").

### 1.4 Site-naming convention used in §2-§4 (read this before the tables)

The freeze re-reserved these 38 keys **precisely because their edges do not
exist yet**. A table that claimed every key already has a resident mutation
function would be false. So each row names:

- **File** — an owner-private mutation file that **exists today** and is (or
  becomes) a declared entry of `FAULT_INJECTION_SOURCES`
  (`fault_expected_keys.rs:419`+), whose completeness is machine-pinned by the
  anchor at `:414-418`.
- **Function** — with a status marker:
  - `EXISTS` — the function is resident today and the row's boundary is a point
    that function already crosses without announcing it;
  - `NEW@<step>` — the function does not exist; the named E-step adds it beside
    its resident siblings in the same file, on the named precedent.

No row names a file that does not exist. Where a row's edge additionally needs
a surface change outside the mutation file, that is stated in the row and
carried to the OPEN register (§2.4, §3.5, §4.4).

---

## 2. The `cleanup.*` activation record (proposed, 11 keys)

> **`cleanup.*` activation record (DRAFT 2026-08-27, R2-E Step E0.2 semantics
> amendment; annotates the Step-4.1 non-activation record above, which is left as
> written since this annotation is the sanctioned mechanism).** This record binds
> semantics; the flip to `Executed` is R2-E **Phase E1**'s, in the commit that
> converts the edges, per RemPlan §10's duty. Counts: 165 total, unchanged; no
> key minted; `cleanup.*` stays 0/11 until E1.2 lands.

### 2.1 What the family's edges physically are, on this tree

The cleanup worklist has **no driver today, production or test**. A tree-wide
grep of `src/checked_artifact` for `CleanupWorklist` / `cleanup_retirement` /
`CleanupAliasV1` / `read_and_bind_cleanup_worklist` returns, outside
`protocol/cleanup.rs` itself: the codec round-trip in
`interface_tests/schedule_records.rs:197-215`, one slot-list entry in
`admission/tests.rs:258`, the vocabulary in `fault_v1.rs:178-188`, and the
frozen surface — the schedule's alias set
(`protocol/schedule.rs:14`, `:91`, `:267`), the slot
(`protocol/slots.rs:112`/`:129`/`:146`), the publish role
(`namespace/roles.rs:23`/`:37`), and the retirement destination builder
(`namespace/mod.rs:117-148`). That is the whole surface. E1 gives it its first
driver.

Its physical shapes are already fixed by three frozen facts:

1. **The worklist is a bounded canonical protocol record** in one deterministic
   base slot: `BoundedCanonicalRecordV1 for CleanupWorklistV1`
   (`protocol/cleanup.rs:296-306`), kind `CleanupWorklist`
   (`protocol/codec.rs:21`/`:49`, 16 KiB bound `:62`), slot
   `BaseActionSlotV1::CleanupWorklist` (`protocol/slots.rs:112`, suffix
   `"cleanup-worklist"` `:146`), whose name derives from the action digest
   (`ActionSlotV1::name`, `slots.rs:188-202`).
2. **The retirement is a rename between two deterministic slot names of one
   retained action directory**, exactly like E12/E13:
   `ActionNamespace::cleanup_retirement` (`namespace/mod.rs:117-148`) returns a
   `CleanupRetirementDestination` built by `action_destination`
   (`namespace/mod.rs:312-318`) onto `RetiredSourceAlias` / `RetiredGoalAlias` /
   `RetiredAuthorityAlias` (`slots.rs:109-111`, `:143-145`), and it refuses an
   alias the schedule did not reserve (`namespace/mod.rs:126-136`). The move
   itself is `ActionNamespace::retire_exact` (`namespace/mod.rs:261-273`) →
   `HostActionNamespaceV1::retire_exact` (`namespace/host.rs:622-634`) →
   `RetainedActionNamespaceV1::execute_edge` with
   `ActionNamespaceEdgeV1::Retire` (`namespace_mutation.rs:250-305`).
3. **The classification is already written**: `classify_cleanup_row`
   (`protocol/cleanup.rs:383-401`) resolves one row from a
   `(source, destination)` fact pair to `Retire` / `Complete` / `Ambiguous`, and
   `BoundCleanupWorklistV1::classify` (`:323-333`) is its bound entry.

### 2.2 The routing decision this record takes, and its rejected alternative

**DECISION C-1 — the retirement rename keeps announcing `namespace.retire_exact`;
`cleanup.*` announces only the boundaries the family itself owns.**

Grounds: the identical question was settled for the managed intent lifecycle and
the answer is written in code at `managed_mutation.rs:1018-1027`:

> The rename itself is the already-executed `namespace.publish_no_replace` /
> `namespace.retire_exact` boundary of Step 2.2, because this lifecycle
> deliberately routes through the role-validated backend rather than opening the
> sealed primitive again — so these two are the boundaries this family owns, in
> the same shape Step 2.3 used for `staging_directory_publish` and
> `marker_retire`.

Consequently `cleanup.alias_retire` names, like
`managed_bootstrap.prior_generation_retire` (`managed_mutation.rs:1039-1042`),
the **post-edge state** "the retirement rename is durable and nothing has looked
at it yet" — not the rename instruction.

**Rejected alternative:** add a `Cleanup` variant to `ActionNamespaceEdgeV1`
(`namespace_mutation.rs:55-93`) so `faults()` returns cleanup keys for the four
shared boundaries. Rejected because it would **silence**
`namespace.retirement_reserve` / `pre_retire_reobserve` / `retire_exact` /
`retired_reobserve` for these renames, weakening the `namespace.*` family's
already-accepted `Executed` claim (`fault_expected_keys.rs:335-339`) on a tree
where those four keys' matrix rows assert they fire on every retirement.

**DECISION C-2 — the sites live in
`capability/pre_catalog/provider/namespace_mutation.rs`.**
Grounds: every cleanup edge is inside the one retained action directory that
file's `RetainedActionNamespaceV1` owns (`namespace_mutation.rs:122-129`,
retained at `:135-171` through the single identity-proved no-follow hop from the
permit-retained completed catalog, `completed.rs:169-181`); the family needs no
second retained capability, which is the ground on which `managed_mutation.rs`
exists as its own file.
**Rejected alternative:** a new `cleanup_mutation.rs` taking
`&RetainedActionNamespaceV1` (the shape `write_managed_intent_scratch` already
uses, `managed_mutation.rs:1065-1070`). Rejected on cohesion arithmetic:
`namespace_mutation.rs` is **414 lines** today and the cleanup additions are
~120; the settled tuple's §11.3 item 4 records `managed_mutation.rs` at 1,251
lines as "the one R2-D-owned production file over the cohesion trigger", and
R2-E should not mint a second file per family when one owner suffices. If E1
measures the addition above ~250 lines, the alternative is the correct fallback
and the reviewer should say so.

**DECISION C-3 — the worklist's scratch row is the existing shared
`BaseActionSlotV1::RecordScratch`; no slot is minted.**
The three `cleanup.worklist_scratch_*` keys prove by frozen vocabulary that the
worklist *has* a scratch, but `BaseActionSlotV1` (`protocol/slots.rs:98-113`)
carries **no** `CleanupWorklistScratch` — its thirteen entries are
`Reservation`, `Authority`, `SourcePayload`, `GoalPayload`, `AuthorityScratch`,
`GoalScratch`, `RecordScratch`, `BarrierIntentScratch`,
`BootstrapIntentScratch`, the three `Retired*Alias` rows, and `CleanupWorklist`
itself. `RecordScratch` (`:106`/`:140`, publish role `namespace/roles.rs:20`/`:34`)
is the only unspecialized scratch, and a shared deterministic scratch slot is a
pattern already in production: `write_managed_intent_scratch` states it in code —
"The scratch slot is one deterministic base slot shared by every generation of
every row of this action, and the bytes are re-derived identically on every
drive, so a leftover scratch from an interrupted generation is this drive's own
residue and must be converged on rather than wedged against"
(`managed_mutation.rs:1058-1064`) — with the write-or-rewrite discipline that
goes with it (`create_new` fresh, truncate on resume, `:1073-1101`).
**Ordering condition, binding on E1.1 — OPEN-C1.** `RecordScratch` is also where
the authority record's own scratch boundaries are announced (`record.scratch_create`
/ `_write` / `_flush`, sites at `authority_record_binding.rs:508`, `:516`,
`:529`). E1.1 must establish — and E1.2's matrix must drive — that the authority
record's scratch is consumed before the cleanup worklist's is written. The two
are sequential in the action lifecycle (authority install early, cleanup last),
but that must be **proved, not assumed**; an interleaving row is the matrix row
this decision most needs. *Evidence that would settle it:* the schedule's own
ordering of `PublishRoleV1::RecordScratch` and `PublishRoleV1::CleanupWorklist`
consumers once E1.1 writes the first driver — no consumer of either exists today,
so the question cannot be answered by reading the tree.
**Rejected alternative:** mint `BaseActionSlotV1::CleanupWorklistScratch`.
Rejected because it mints a persisted name, which freeze §6 and the §3.1
persisted-home pin forbid, and because it would move `BASE_ACTION_SLOTS` from 13
(`protocol/bounds.rs:9`) and with it the `MAX_ACTION_SLOTS == 261` compile-time
assertion (`bounds.rs:70`) — a frozen-surface edit no activation record may take
silently.

### 2.3 The eleven keys

Site file for all eleven: `capability/pre_catalog/provider/namespace_mutation.rs`
(joins `FAULT_INJECTION_SOURCES` as a cleanup-bearing source; it is already a
declared entry at `fault_expected_keys.rs:449-452`).

| # | Key | Semantic — the durable state the boundary names | Function (status) | Precedent |
| --- | --- | --- | --- | --- |
| 1 | `cleanup.worklist_scratch_create` | the worklist's scratch row is open for write in the shared `BaseActionSlotV1::RecordScratch` slot (DECISION C-3, with OPEN-C1's ordering condition); nothing is written | `write_cleanup_worklist_scratch` (`NEW@E1.1`) | `write_managed_intent_scratch`, hit at `managed_mutation.rs:1092-1095` |
| 2 | `cleanup.worklist_scratch_write` | the canonical bytes of `CleanupWorklistV1::encode_canonical` (`protocol/cleanup.rs:234-238`) are written, not yet flushed | same (`NEW@E1.1`) | `managed_mutation.rs:1104-1107` |
| 3 | `cleanup.worklist_scratch_flush` | `sync_all` returned; the scratch bytes are durable, the dirent may not be | same (`NEW@E1.1`) | `managed_mutation.rs:1110-1113` |
| 4 | `cleanup.worklist_publish` | the no-replace rename onto `BaseActionSlotV1::CleanupWorklist` is durable and unread | `observe_cleanup_worklist_row` (`NEW@E1.1`) | `observe_managed_intent_row`, `managed_mutation.rs:1151-1152` |
| 5 | `cleanup.worklist_reobserve` | the published row has been re-read bounded and proved equal to the bytes written (`read_and_bind_cleanup_worklist`, `protocol/cleanup.rs:356-367`, which additionally refuses a worklist that does not match the resident reservation, `:361-365`) | same (`NEW@E1.1`) | `managed_mutation.rs:1154-1155` |
| 6 | `cleanup.source_reobserve` | one row's **source** alias has been observed into a `CleanupPhysicalFactV1` (`protocol/cleanup.rs:369-374`) — `Exact(fingerprint)` proving identity, length and sha256 (`DurableLeafFingerprintV1`, `:99-129`) | `observe_cleanup_row_facts` (`NEW@E1.1`) | `retain_source` / `NamespaceSourceRetain`, `namespace_mutation.rs:235-244` |
| 7 | `cleanup.destination_reobserve` | the same row's **destination** (`Retired*Alias`) has been observed into its own `CleanupPhysicalFactV1`; the pair is what `classify` consumes (`protocol/cleanup.rs:323-333`) | same (`NEW@E1.1`) | same |
| 8 | `cleanup.alias_retire` | the retirement rename is durable and unread — post-edge, **not** the rename (DECISION C-1) | `observe_cleanup_retirement` (`NEW@E1.1`) | `managed_bootstrap.prior_generation_retire`, `managed_mutation.rs:1039-1042` |
| 9 | `cleanup.retired_alias_reobserve` | the retired row has been re-read and proved to be the exact fingerprint the worklist row named | same (`NEW@E1.1`) | `managed_mutation.rs:1041` twin |
| 10 | `cleanup.row_complete` | the parent flush after the retirement has returned, so a fresh process re-classifies this row as `CleanupResolutionV1::Complete` (`protocol/cleanup.rs:394-398`) rather than `Retire` | same (`NEW@E1.1`), announced after `sync_directory_edge` (the resident twin of the unannounced flush at `namespace_mutation.rs:303`) | `sync_directory_edge` usage, `namespace_mutation.rs:303`; `directory_mutation.rs` flush family (freeze §4.1 P2) |
| 11 | `cleanup.completion_reobserve` | the **whole-worklist** proof: the worklist is re-read and every row classifies `Complete`; this is the state `terminal.cleanup_reobserve` (§4) consumes | `observe_cleanup_completion` (`NEW@E1.1`) | `managed_bootstrap.component_reobserve` (restart observation), fixture `:453-459` |

**No `cleanup.*` key is proposed reserved.** Each of the eleven names a state a
restarting process can physically distinguish. The two the drafter tested
hardest against the Phase-3 settle determination (freeze `:692`-`:706`, which
kept `preflight` and `plan_complete` permanently siteless because they "name
plan-level states … not durable edges") are #10 and #11: #10 survives that test
because the retirement dirent's flush *is* a durable edge whose loss a restart
sees (it re-enters `Retire` rather than `Complete`); #11 survives because it is
an **observation** boundary, and the family map already carries executed
observation keys of exactly that shape.

### 2.4 The `AdmittedActionV1` duty attachment

The re-reservation's clause — "every one requires an `AdmittedActionV1` and a
scheduled action directory" — is structural on this tree, in four places, and
E1's matrix must exercise all four rather than assert them:

1. `ActionNamespace` is constructible **only** `from_admitted`
   (`namespace/mod.rs:85-93`); its `binding()` (`:100-107`) is derived from
   `self.admitted_action.reservation()`.
2. Every cleanup edge passes `validate_operation` (`namespace/mod.rs:297-309`),
   which refuses a binding mismatch and then calls
   `revalidate_action_directory` → `RetainedActionNamespaceV1::revalidate`
   (`namespace_mutation.rs:202-230`), announcing
   `namespace.parent_revalidate` at `:225-228`.
3. `cleanup_retirement` refuses an alias outside
   `schedule().cleanup_aliases()` (`namespace/mod.rs:126-136`;
   `protocol/schedule.rs:267`).
4. `CleanupWorklistV1::try_new` refuses rows whose alias set is not exactly the
   reserved set (`protocol/cleanup.rs:164-181`), and
   `matches_reservation` re-checks four digests plus the alias set on every
   read (`:219-232`).

**Duty, stated for E1.2:** the matrix drives through a real admitted action —
`ScheduledCheckedActionV1::admit` (`coordinator/execution.rs:137-152`) →
`ActionAdmissionOwnerV1::resume_or_admit` — not through a hand-built
reservation. The admission owner's own `resume_or_admit` refuses an action
directory that is not exactly this reservation's
(`coordinator/execution.rs:132-136` records why the coordinator's second
comparison is belt-and-braces), so a matrix that skipped admission would be
proving a different thing.

### 2.5 The convergence obligation (E1.2), stated in the admission matrix's own form

Calibrated on `admission/tests_fault_matrix.rs`:
`ADMISSION_MATRIX: [Fault; 19]` (`:53`), `REPEATED_CRASH_ROUNDS: usize = 12`
(`:80`), `TargetVariantV1::{Workspace, GitDirectory}` (`:86-91`),
`reconcile_executed_keys` (`:235`) reconciling the matrix against the fixture,
and one named test per variant (`:414-421`).

E1.2 owes:

- `CLEANUP_MATRIX: [Fault; 11]` — every key **interrupted, restarted, and
  converged**, on **both** target variants;
- repeated-boundary rows at twelve rounds over a declared subset;
- **single-crossing classification declared per row**, driven by
  `run_single_crossing_probe`
  (`bootstrap/managed/tests_provider.rs:564`) — the instrument the Step-3.2
  record says "caught a real misclassification at once" (freeze `:655`-`:666`).
  The cleanup family's shape makes this load-bearing: the three alias
  retirements share one helper, so on a three-alias row each of keys #6-#10 is
  crossed **once per alias** and is therefore neither repeatable nor
  single-crossing — E1.2 must drive a **one-alias** row for the
  single-crossing half, exactly as the writer matrix drives `gwz.conf/markers`;
- a matrix-to-fixture reconciliation in the `reconcile_executed_keys` shape;
- the `FAULT_FAMILY_ACTIVATION` row edit `Reserved("R2-D phase 4 (legacy leaf
  edge conversion)")` → `Executed(...)` (`fault_expected_keys.rs:380-384`), the
  `FAULT_INJECTION_SOURCES` doc-comment extension (`:397-418`), and the
  §3.5 inventory addendum's file count (freeze `:525-532`, "nine" → ten), all in
  the same commit — RemPlan §10's duty;
- the R4b-G driver's per-OS fault markers (plan E1.2, ritual 5).

---

## 3. The `barrier.*` activation record (proposed, 16 keys) — including the O6 RESOLUTION

> **`barrier.*` activation record (DRAFT 2026-08-27, R2-E Step E0.2; annotates
> the Step-4.2 non-activation record above, left as written).** Counts: 165,
> unchanged; no key minted; `barrier.*` stays 0/16 until E2.3 lands.

### 3.1 The O6 obligation and the precedent, quoted

The obligation, `GwzM5-8R2DSettledTuple.md` §11.1 (`:653`-`:658`):

> `BarrierIntentV1::issue` (`protocol/barrier.rs:39`) accepts caller-asserted
> anchor/home identity facts with no observation check — the named
> caller-restatement class's one remaining seam shape, production-unreachable
> today (sealed witness, zero production callers). **BINDING R2-E
> obligation**: apply the 4.3 observe-or-refuse pattern before the roaming
> anchor gains its first admitted-action caller (Code [P3-1]).

The ordering constraint, restated **verbatim** from E0.1(b) row 1
(`GwzM5-8R2E-E01ReachTraces.md:71-77`):

> 1. **`BarrierIntentV1::issue` (BINDING, ordering constraint):** the
>    observe-or-refuse pattern must land **before the roaming anchor gains its
>    first admitted-action caller**. Plan consequence, now explicit: **E2.2
>    strictly precedes any E4 row that admits actions touching the roaming
>    anchor** — the plan's nominal E1-E3 → E4 order is load-bearing, not
>    stylistic.

The precedent's exact shape — the **Step-4.3 settle-item-8 record**,
`GwzM5-8R2DPhase4Closure.md` §4 (`:292`-`:319`):

> **The defect class.** `CheckedAuthorityObservationV1::owner_issue` sets
> `action_digest: reservation.action_digest()` — it *copies the binding field out
> of the caller's argument*. That is the caller-supplied-restatement class …
>
> **The fix, at the root.** `AuthorityObservationFactsV1` gains `action_digest`;
> `AuthorityFactsIssuerV1::issue` takes it; and
> `CheckedAuthorityObservationOwnerV1::observe` refuses when it disagrees with
> the resident reservation, beside the `request_owner_binding` check already
> there. The production transaction supplies `self.proof.action()` — the digest
> the streaming capability actually read through.
>
> … The obligation — pass the digest you derived your reads from, never one a
> caller handed you — is written on `AuthorityFactsIssuerV1::issue` itself,
> because that signature is the only place a future transaction author will look.

So the precedent has **three parts**: (i) the *owner* — not the protocol type —
makes the observation; (ii) the owner **refuses** on disagreement; (iii) the
*issuer's signature* carries the derivation obligation. §3.2 applies all three
per fact.

### 3.2 O6 RESOLUTION — per fact `BarrierIntentV1::issue` currently asserts

The seam today: `BarrierIntentV1::issue` (`protocol/barrier.rs:39-70`) takes
nine arguments; its **only non-test caller** is
`ActionNamespace::barrier_intent` (`namespace/mod.rs:205-228`) — the one other
reference is the `#[cfg(test)]` twin `test_issue` (`protocol/barrier.rs:193-215`),
which manufactures the sealed authority via `NamespaceBarrierAuthority::test_only`
(`namespace/mod.rs:72-75`) and is the reason the seam is
"production-unreachable today". `barrier_intent` forwards
three of them straight from *its* caller (`catalog_anchor_identity`,
`private_home_parent_identity`, `private_home_name`, `namespace/mod.rs:208-210`
→ `:220-222`) and derives the other three from the retained target
(`slots.target.parent.identity()`, `.path_profile()`, `slots.target.leaf`,
`:224-226`). The ordinal is already checked against the schedule
(`protocol/barrier.rs:50-54`).

| Fact | Source today | Can the callee observe it itself? | **Disposition** |
| --- | --- | --- | --- |
| **anchor identity** (`catalog_anchor_identity`) | caller argument, `namespace/mod.rs:209` | **YES.** `RetainedCompletedCatalogV1` retains `catalog_anchor` — the `catalog-anchor-a-v1` row — as a live handle with its identity (`completed.rs:44`, retained `:95-99`), and revalidates it on every use (`:204-211`). Its bytes are pinned (`CATALOG_ANCHOR_BYTES`, `interior.rs:29`) and `completed_record` refuses a catalog whose anchor row is not exactly those bytes (`interior.rs:351`). | **OBSERVE.** The identity comes from the retained capability, never from a caller. |
| **home identity** (`private_home_parent_identity`) | caller argument, `namespace/mod.rs:209` | **YES.** The roaming anchor's home is the retained completed-catalog Final directory, whose identity the same owner holds (`completed.rs:42`, minted `:85`, revalidated `:194-203`), and which itself retains `roaming_anchor` (`completed.rs:45`, `:100-104`; bytes `ROAMING_ANCHOR_BYTES`, `interior.rs:28`; residency required by `completed_record`, `interior.rs:350`). | **OBSERVE.** |
| **home name** (`private_home_name`) | caller argument, `namespace/mod.rs:210` | **YES — and it is not even an observation.** It is the frozen compile-time constant `InfrastructureSlotV1::RoamingAnchorHome.name()` = `"roaming-anchor-home-v1"` (`protocol/slots.rs:51`, `:79`). | **DERIVE** (drop the parameter). A derived constant cannot be restated wrongly. |
| **target parent identity + path profile** | already derived, `namespace/mod.rs:224-225` | already provider-observed: `BarrierTarget` is minted by `BackendIssuer::barrier_target` from `self.backend.retained_parent()` (`namespace/host.rs:165-171`) | **already observed** — unchanged by this record, but see **OPEN-B2**. |
| **reserved target leaf** | caller argument to `scheduled_barrier_slots` (`namespace/host.rs:152`) → `slots.target.leaf` → `:226` | it is a **reservation**, not an identity assertion: the anchor's outbound publish is no-replace onto exactly that name, so a wrong choice is a typed refusal, not a silent acceptance | **CONSTRAIN** — see **OPEN-B3** on the name grammar. |
| **ordinal** | caller argument | already refused if `>= schedule().barrier_count()` (`protocol/barrier.rs:50-54`) and re-refused on read (`:249`) | already checked. |

**RESOLUTION, stated for the record: OBSERVE both, with one typed REFUSE.**

- **Mechanism.** `BarrierIntentV1::issue` stops taking three bare values and
  takes one **owner-minted witness** — `RoamingAnchorHomeWitnessV1`,
  constructible only inside the pre-catalog provider owner from
  `RetainedCompletedCatalogV1`'s own retained `catalog_anchor` /
  `roaming_anchor` / `final_directory` handles, and carrying the two identities
  plus the derived home name. `ActionNamespace::barrier_intent` receives it
  through `retain_action_namespace`'s existing forwarding seam
  (`completed.rs:169-181`) rather than from its caller. Part (iii) of the
  precedent lands on `issue`'s own signature and doc, in the shape
  `AuthorityFactsIssuerV1::issue` uses: *pass the witness the capability owner
  minted; there is no route from a caller-supplied identity.*
- **The REFUSE arm, typed.** The observation is the **only** source, so where it
  cannot be made the seam refuses. Exactly one reachable state produces that:
  the roaming anchor is **not at home** — out in a target parent from a drive
  that crashed between `anchor_outbound` and `anchor_return`. In that state the
  witness cannot be minted and `issue` refuses typed; the drive must first
  reconcile the stranded anchor through `barrier.anchor_return` /
  `barrier.target_alias_retire` before a new intent may issue. **That refusal is
  currently unreachable for a different and worse reason — see OPEN-B1, which is
  blocking.**
- **Rejected alternative:** keep the three parameters and add a comparison
  inside `barrier_intent` against owner-supplied facts. Rejected on the
  precedent's own ground: a comparison leaves the caller's values in the
  signature, so the next author still has a route to supply them, and the
  Step-4.3 record chose to move the fact into the issued type rather than to
  compare a restatement. It is also strictly weaker here, because two of the
  three values are *derivable* and one is a compile-time constant — comparing a
  constant against itself is ceremony, not evidence.

**Ordering, binding on the plan:** the mechanism above is E2.2, and E2.2
strictly precedes any E4 row that admits actions touching the roaming anchor
(quoted verbatim at §3.1). Because §5 places the first production catalog
activation at E4.1, and the roaming anchor is reachable only from an admitted
action, the constraint is satisfied by the plan's nominal order — but it is now
**load-bearing**, and E4.1's gate must state it.

### 3.3 Routing decisions for the family

**DECISION B-1 — a new owner-private file
`capability/pre_catalog/provider/barrier_mutation.rs`.**
Grounds: unlike cleanup, this family crosses **two** retained parents — the
completed catalog's roaming-anchor home (`completed.rs:100-104`) and a target
parent — which is a distinct capability composition and precisely the ground on
which `managed_mutation.rs` is its own file; and adding sixteen keys' sites to
`namespace_mutation.rs` on top of cleanup's eleven would push that file toward
the cohesion trigger the settled tuple already flagged once (§11.3 item 4).
Adding a declared source is cheap and machine-safe: the completeness anchor
(`fault_expected_keys.rs:414-418`) rescans the production tree and fails if any
source naming `CheckedArtifactFaultKeyV1` is undeclared.
**Rejected alternative:** `namespace_mutation.rs` — rejected on the cohesion
arithmetic above.

**DECISION B-2 — the intent record's five keys follow the managed intent
lifecycle exactly.** `BarrierIntentV1` is a bounded canonical record
(`protocol/barrier.rs:218-228`) moved between `BarrierIntentScratch`
(`slots.rs:107`/`:141`), `BarrierIntentActive(ordinal)` and
`BarrierIntentRetired(ordinal)` (`slots.rs:154-155`, grammar `:303-304`) — the
same shape as `BootstrapIntentScratch`/`Active`/`Retired`. So the three scratch
keys mirror `scratch_faults()` (`managed_mutation.rs:1002-1016`) and the
publish/retire pairs mirror `edge_faults()` (`:1029-1048`).

**DECISION B-3 — `barrier.target_barrier` needs a third `DirentBarrierClass`.**
`DirentBarrierClass` (`platform.rs:363-374`) has two variants;
`AnchoredPrivateArea`'s contract is a directory that "deliberately retains a
permanent `.ca1-durability-anchor-<32hex>` file as product infrastructure
(`finish()` never removes it)" (`:365-368`), and `ExactInterior`'s Windows arm
is a documented no-op precisely because "the round trip below is *unavailable*
to this class, not merely skipped: it renames a resident
`.ca1-durability-anchor-<32hex>` file, and this class of directory may retain
none" (`platform.rs:531-534`). The roaming anchor's whole purpose is to make the
round trip available to a directory that may retain no permanent anchor, by
lending it a **supplied** one for the duration of one barrier. Neither existing
variant states that, so the class must be a third variant (working name
`RoamingAnchoredTarget`) whose Windows arm round-trips the *supplied* anchor
name rather than surveying for a resident one.
Minting a class variant **moves no census** — it is not a fault key.
**The E10/E14 annotation is unaffected**, and this record says so explicitly:
that annotation's claim is about *which arm a caller takes*, and E10/E14 pass
`ExactInterior` at two hardcoded sites (`namespace_mutation.rs:328`;
`host.rs`'s barrier target pin, `namespace/host.rs:741-756`), neither of which
this family touches. `barrier.target_barrier` is a **distinct call site** in a
distinct file.
**Rejected alternative:** pass `AnchoredPrivateArea` at the target parent.
Rejected because on Windows that arm surveys for and, failing to find one,
would *establish* a permanent `.ca1-durability-anchor-*` in the target parent
(`platform/anchor.rs:129` `prepare`, `:216` `establish`, `:284` `survey`) — the
exact-evidence contamination class `platform.rs:536-541` records as diagnosed
and fixed.

### 3.4 The sixteen keys

Site file for all sixteen: `capability/pre_catalog/provider/barrier_mutation.rs`
(`NEW@E2.1`; joins `FAULT_INJECTION_SOURCES`).

| # | Key | Semantic — the durable state the boundary names | Function (status) | Precedent / anchor |
| --- | --- | --- | --- | --- |
| 1 | `barrier.intent_scratch_create` | the intent record's scratch row is open for write in `BaseActionSlotV1::BarrierIntentScratch` | `write_barrier_intent_scratch` (`NEW@E2.1`) | `managed_mutation.rs:1092-1095` |
| 2 | `barrier.intent_scratch_write` | `BarrierIntentV1::encode_canonical` bytes written (`protocol/barrier.rs:114-118`), not flushed | same | `managed_mutation.rs:1104-1107` |
| 3 | `barrier.intent_scratch_flush` | `sync_all` returned; scratch bytes durable | same | `managed_mutation.rs:1110-1113` |
| 4 | `barrier.intent_publish` | the no-replace rename onto `BarrierIntentActive(ordinal)` is durable and unread | `observe_barrier_intent_row` (`NEW@E2.1`) | `managed_mutation.rs:1151-1152` |
| 5 | `barrier.intent_reobserve` | the active intent has been re-read bounded and bound to the resident reservation and ordinal (`read_and_bind_barrier_intent`, `protocol/barrier.rs:238-256`, which refuses on any of five mismatches `:244-254`) | same | `managed_mutation.rs:1154-1155` |
| 6 | `barrier.anchor_outbound` | the roaming anchor's no-replace publish **out of** `RoamingAnchorHome` and **into** the target parent under `reserved_target_leaf` is durable and unread | `execute_anchor_move` (`NEW@E2.1`) | E12's shape, `namespace_mutation.rs:280-290`; **blocked by OPEN-B1** |
| 7 | `barrier.anchor_outbound_reobserve` | the anchor has been re-read at the target under its reserved leaf and proved to be the identity the intent bound | same | `namespace_mutation.rs:292-302` |
| 8 | `barrier.target_barrier` | `private_barrier` at the **target parent** has returned, with the third class (DECISION B-3) — on Windows the supplied-anchor round trip, elsewhere the directory `fsync` (`platform.rs:377-384`) | `barrier_target_parent` (`NEW@E2.1`) | `namespace_mutation.rs:314-336` (the E14 twin); **blocked by OPEN-B1** |
| 9 | `barrier.target_reobserve` | the target parent has been re-observed after the barrier; its identity and the anchor's residency are the ones the intent bound | same | `NamespaceParentRevalidate`'s shape, `namespace_mutation.rs:225-228` |
| 10 | `barrier.anchor_return` | the anchor's no-replace publish **back onto** `RoamingAnchorHome` is durable and unread | `execute_anchor_move` (`NEW@E2.1`) | E12; **blocked by OPEN-B1** |
| 11 | `barrier.anchor_return_reobserve` | the home row has been re-read and proved to be exactly `ROAMING_ANCHOR_BYTES` at the bound identity — i.e. `completed_record`'s own predicate (`interior.rs:350`) holds again | same | `interior.rs:350` |
| 12 | `barrier.target_alias_retire` | a **stranded** anchor alias at the target (a crash between #6 and #10) has been retired onto `RetiredRoamingAnchorAlias(ordinal)` (`slots.rs:156`, grammar `:305`); durable and unread | `execute_stranded_alias_retirement` (`NEW@E2.1`) | E13; and the Step-4.2 ordinal-indexed retirement precedent, freeze `:814`-`:831` |
| 13 | `barrier.target_alias_reobserve` | the retired alias row has been re-read and proved | same | freeze `:830`-`:831` |
| 14 | `barrier.intent_retire` | the intent's rename onto `BarrierIntentRetired(ordinal)` is durable and unread | `observe_barrier_intent_row` (`NEW@E2.1`) | `managed_bootstrap.final_intent_retire`, `managed_mutation.rs:1043-1046` |
| 15 | `barrier.intent_retired_reobserve` | the retired intent has been re-read; it is this barrier's own completion record | same | `managed_mutation.rs:1045` twin |
| 16 | `barrier.completion_reobserve` | the whole barrier ordinal has settled: anchor home, target clean, intent retired — the state a restart uses to skip an already-completed ordinal | `observe_barrier_completion` (`NEW@E2.1`) | `managed_bootstrap.component_reobserve`'s restart-observation role |

**No `barrier.*` key is proposed reserved.** Three of them (#6, #8, #10) are
**ACTIVATE-CONDITIONAL-ON-OPEN-B1** — their edges cannot exist at all until
OPEN-B1 resolves.

### 3.5 OPEN rows this family carries

**OPEN-B1 — BLOCKING for E2.1. The roaming anchor cannot leave home while
`completed_record` requires it resident.**

*The exact question:* `interior::completed_record` requires
`exact_file_identity(interior, Slot::RoamingAnchorHome, ROAMING_ANCHOR_BYTES)`
(`interior.rs:350`). While the anchor is out in a target parent — the whole
window keys #6-#10 name — the home row is **absent**, so `completed_record`
returns `None`, so `retain_completed_catalog` refuses "final catalog does not
have the exact retired layout" (`completed.rs:64-74`) and the same predicate
guards the retained-catalog revalidation (`completed.rs:183-211`) and the
publication destination recheck (`publication.rs:225`, `:264`). **A crash inside
the roaming window would therefore leave a catalog no process can retain** —
including the process that would reconcile it.

*Candidate mechanisms, both stated so the dual can rule:*
(i) a **C-3-style widening of the provider's reading**: `completed_record`'s
roaming-anchor arm accepts *either* the exact resident anchor *or* its absence
paired with a resident `BarrierIntentActive(ordinal)` reconciliation record.
Class 2's own ruling is the precedent and says what its limits are (freeze
`:1443`-`:1450`): "C-3 is an **observer/slot-grammar extension in the same
provider owner** … What Phase 1 must extend is the *provider's reading* of that
vocabulary, not the vocabulary."
(ii) the anchor is **copied, not moved** — a fresh object carrying
`ROAMING_ANCHOR_BYTES` is created at the target, barriered, and retired — which
keeps `completed_record` untouched but makes `catalog_anchor_identity`'s binding
name an object that never travels, and makes keys #6/#10's names ("outbound",
"return") false.

*Evidence that would settle it:* (a) whether the infrastructure descriptor's
bytes move — they do **not**: `InfrastructureRecordV1::owner_issue_for_catalog`
takes the roaming anchor's **durable identity** (`interior.rs:350-359`), and a
rename preserves the object, so a moved-and-returned anchor reproduces
byte-identical `RetiredActionsDescriptor` and `CatalogFormat` rows
(`interior.rs:361-362`); this is strong evidence for (i) and against the need
for any record change; (b) whether the freeze's §6 zero-new-durable-records rule
is engaged — on the reading above it is not, because no record, slot, purpose or
name is minted; (c) a Windows arm decision for `barrier.target_barrier` under
(ii), which would have no anchor identity to round-trip.
**Drafter's recommendation: (i).** It is not taken as a decision here because
`completed_record` is the predicate every catalog consumer depends on, and
widening it is a reviewed edit the dual should authorize explicitly rather than
inherit from a table row.

**OPEN-B2 — the barrier target is pinned to the action directory today.**
`scheduled_barrier_slots` mints its `BarrierTarget` from
`self.backend.retained_parent()` (`namespace/host.rs:165-171`), and
`barrier_namespace` refuses unless `slots.target.parent.identity()` equals the
passed parent's (`namespace/mod.rs:288-293`), which `HostActionNamespaceV1::barrier`
then pins to the retained action directory (`namespace/host.rs:746-753`). So
today the "target parent" *is* the action directory and the anchor has nowhere
to roam. *The question:* does E2.1 extend `scheduled_barrier_slots` to accept a
retained **managed parent** (`RetainedManagedParentV1`, already plumbed at
`namespace/host.rs:184-227`) as the target, or does the roaming anchor serve
only the action directory? *Evidence that would settle it:* the first E4 consumer
that needs a Windows dirent barrier over a directory it may not anchor — the
ConsumerCheckpoint §10 rows for markers and `.git/info/exclude`
(`GwzM5-8R4bR2ConsumerCheckpoint.md:277`, `:279`) are the candidates. Owner: E2.1,
with the answer recorded in the same commit.

**OPEN-B3 — the reserved target leaf's name grammar.**
`reserved_target_leaf` is a caller-chosen `AsciiComponent`
(`namespace/host.rs:152`). If the target parent is user-visible, the anchor's
name appears there for the duration of the barrier. *The question:* is the leaf
constrained to a derived, action-scoped, dotted grammar — the shape Step 4.2
adopted for the private area's own scratch, `.ca1-{family}-{action}-{kind}.scratch`
(freeze `:857`-`:871`) — or left caller-chosen with the no-replace publish as
the only guard? *Evidence that would settle it:* whether any E4 target parent is
inside a preservation-image capture or an `extra_children: 0` grammar; the
freeze's own warning that "landing the anchor inside an admitted action
directory additionally requires teaching admission's `extra_children: 0` grammar
the reserved target leaf" (`:779`-`:781`) is the live case. Owner: E2.1.

### 3.6 The convergence obligation (E2.3)

Same form as §2.5: `BARRIER_MATRIX: [Fault; 16]`, both target variants,
interrupted+restarted+converged per key, twelve-round repeated-boundary rows,
single-crossing classification declared per row, matrix-to-fixture
reconciliation, the `FAULT_FAMILY_ACTIVATION` row edit
(`fault_expected_keys.rs:385-389`), the source-list and inventory-addendum
edits, and the R4b-G per-OS markers — all in the converting commit.
**Additionally, and specific to this family:** the Windows arm of
`barrier.target_barrier` must be *executed natively*, not skipped — the Step-4.2
record's own standard, which proved the retirement rows took the retirement
branch rather than the platform skip
(`hard_link_identity_sharing_is_what_the_retirement_rows_assume`, freeze
`:836`-`:838`).

---

## 4. The `terminal.*` activation record (proposed, 11 keys — the retirement edges)

> **`terminal.*` activation record (DRAFT 2026-08-27, R2-E Step E0.2; annotates
> the Step-4.2 non-activation record above and its §4.4 arm-table resolution,
> both left as written).** Counts: 165, unchanged; no key minted. **Proposed
> outcome: 10 of 11 executed, 1 reserved with a determination** — i.e. the
> family lands as `FaultFamilyActivationV1::PartiallyExecuted`
> (`fault_expected_keys.rs:209`), whose per-key siteless proof keeps the
> `Reserved` arm's guarantee for the eleventh.

### 4.1 What the family's edges physically are

The family names one composite: **the admitted action directory's terminal
retirement into the catalog's retired root** — §4.3 row **E7**, "action
directory retirement into the retired root", `P1 with destination recheck`,
`publication.rs:157-173` (freeze `:1083`).

Three frozen facts fix its shape:

1. **The destination root exists and is retained**: `InfrastructureSlotV1::RetiredActions`
   (`slots.rs:52`, `"retired-actions-v1"` `:80`), retained as a live handle by
   the completed-catalog owner (`completed.rs:46`, `:105-109`).
2. **The bounds already model retired action directories**:
   `MAX_RETIRED_ACTION_DIRS = 64` and `RETIRED_ROOT_BUDGET_V1`
   (`protocol/bounds.rs:2`, `:62-68`), and `CatalogOccupancyV1`
   (`:74-155`) carries `retired_action_dirs` (`:76`), enforces
   `RetiredLimitExceeded` (`:126-128`) and a **retirement-credit** rule
   (`:132-136`) that reserves one retired slot for every outstanding active
   action. `CatalogOccupancyV1::new` (`:101`, `:110`) has **no production caller
   today**: a tree-wide grep of `src/checked_artifact` finds it only in
   `interface_tests/codec_faults.rs` and `interface_tests/catalog_admission.rs`,
   plus the driver's own owed-to-Phase-4 comment at `admission/driver.rs:104-110`,
   which names this exact debt: "Owed to Phase 4: `CatalogOccupancyV1::can_admit_new`
   (`protocol/bounds.rs`) additionally charges the retirement-credit inequality
   against the retired root, whose bounded count this observation does not carry.
   Wiring it belongs to the phase that lands retirement". **R2-E is that phase.**
3. **The destination recheck arm already exists**:
   `DestinationRecheckV1::AdmissionCatalogInterior { durable_identity, expected,
   absent }` (`publication.rs:85-96`), whose doc names "edges E3 and E7" and
   whose semantic is "re-verifies that the catalog is still exactly complete
   *and* that the deterministic destination row is still free".

### 4.2 Decisions this record takes

**DECISION T-A — the retired child's name is the already-derived
`RootEntryNameV1::ActiveAction(digest).name()`, under a different parent.
Nothing is minted.**
Grounds: `RootEntryNameV1::name` is parent-independent
(`slots.rs:372-379`, rendering `action-<hex>-v1`), and
`MAX_RETIRED_ACTION_DIRS == MAX_ACTIVE_ACTION_DIRS == 64`
(`bounds.rs:1-2`) — the frozen bounds already model a one-for-one retired twin
of each active row, which only a same-name-different-parent scheme gives. This
answers the E4-retire activation record's clause head-on; that record's own
sentence (freeze `:1272`-`:1276`) reads:

> Retiring it *by rename* needs a destination name, and `completed_record`
> requires an **empty** `RetiredActions` root, so no in-vocabulary target exists;
> minting one is forbidden by §6 and the §3.1 persisted-home pin.

Its **first** clause ("no in-vocabulary target exists") is true of the
*admission record slot* it was written about and remains so; it is **not** true
of the action directory, which has an in-vocabulary name already. Its **second**
clause — the empty-root requirement — is **OPEN-T1**.
**Rejected alternative:** a `RetiredAction(digest)` grammar arm. Rejected
because it mints vocabulary for a name that already renders correctly, against
§4.4 Class 2's own rule that what a package extends is "the *provider's reading*
of that vocabulary, not the vocabulary" (freeze `:1449`-`:1450`).

**DECISION T-B — the destination arm is a *field generalization* of
`AdmissionCatalogInterior`, not a new `DestinationRecheckV1` variant.**
Grounds: §4.4 Class 1's own criterion distinguishes the two cases (freeze
`:1356`-`:1364`) — a struct's mandatory field generalizes; an enum gets a
variant. `AdmissionCatalogInterior`'s three fields are exactly what the terminal
retirement needs (the catalog is still complete; a named row is still free); what
must generalize is the `absent: RootEntryNameV1` field's **parent**, from the
catalog root to the retired root. This resolves §4.4's last open arm-table row —
"any further retirement-destination arm | E7's Phase-4 half and the terminal
retirement edges | **Phase 4**" (freeze `:1382`) — to **"no new variant; one
field generalization, owned by R2-E E3.1."**
**Rejected alternative:** a fourth variant. Rejected on Class 1's criterion and
because two producers of one arm in two files is the shape the E16 annotation
already declined.

**DECISION T-C — the sites live in
`capability/pre_catalog/provider/admission_mutation.rs`.**
Grounds: the retirement's destination is the catalog root's own interior, and
that file is the owner-private mutation file that already holds every catalog-root
edge of the action lifecycle (`admission_mutation.rs:1-11`, `:79-102`), already
produces the `AdmissionCatalogInterior` arm, and already holds the nineteen
`admission.*` sites. §4.4's arm table pairs E3 and E7 under one owner
(`:1378-1379`); splitting them would put two producers of one arm in two files.
**Rejected alternative:** `namespace_mutation.rs` (the action-directory owner).
Rejected because the action directory is the retirement's **source**, not its
destination, and that file holds no catalog-root capability — its `parent` field
is a clone of the Final directory used only for revalidation
(`namespace_mutation.rs:160-162`, consumed at `:213-216`).

### 4.3 The eleven keys

Site file for all activated keys:
`capability/pre_catalog/provider/admission_mutation.rs` (already a declared
`FAULT_INJECTION_SOURCES` entry, `fault_expected_keys.rs:432-435`).

| # | Key | Semantic — the durable state the boundary names | Function (status) | Anchor |
| --- | --- | --- | --- | --- |
| 1 | `terminal.authority_reobserve` | the action's `Authority` row (`slots.rs:101`/`:135`) has been re-read bounded and proved to be the authority record this action's retirement is entitled to retire | `observe_terminal_preconditions` (`NEW@E3.1`) | `record.*`'s install/retire owner, `authority_record_binding.rs` (fixture `:340-356`) |
| 2 | `terminal.payload_reobserve` | the `SourcePayload` / `GoalPayload` rows (`slots.rs:102-103`) have been re-read and are the ones the action digest was derived over | same | `require_leaf_digest`, `coordinator/execution.rs:296-310` |
| 3 | `terminal.cleanup_reobserve` | the `CleanupWorklist` row has been re-read and **every row classifies `Complete`** — the join to §2 key #11 | same | `protocol/cleanup.rs:323-333`, `:394-398` |
| 4 | `terminal.reservation_reobserve` | the `Reservation` row (`slots.rs:100`/`:134`) has been re-read and equals the admitted reservation's record digest | same | `AdmittedActionV1::reservation`, `protocol/admission.rs:289-291` |
| 5 | `terminal.directory_flush` | the action directory's own flush has returned; every row above is durable before anything moves | `flush_terminal_action_directory` (`NEW@E3.1`) | `sync_directory_edge` family, freeze §4.1 P2 (`:977`) |
| 6 | `terminal.retired_slot_reserve` | the destination row under `RetiredActions` is proved **free**, and the occupancy credit is proved available — `CatalogOccupancyV1::new` (`bounds.rs:101`, `:110`) → `validate` (`:118`) refusing `RetiredLimitExceeded` (`:126-128`) or `RetirementCreditsExceeded` (`:132-136`); this discharges `admission/driver.rs:104-110`'s owed-to-Phase-4 note | `reserve_retired_slot` (`NEW@E3.1`) | `require_absent`, `namespace_mutation.rs:343-352`; `AdmissionCatalogInterior.absent`, `publication.rs:95` |
| 7 | `terminal.action_directory_retire` | the P1 no-replace rename of the whole action **directory** into the retired root, with the generalized destination recheck (DECISION T-B), is durable and unread | `retire_action_directory` (`NEW@E3.1`) | `publish_verified_no_replace`, `publication.rs:132-200`; `PublicationSourceV1::Directory`, `:110-118` |
| 8 | `terminal.retired_directory_reobserve` | the retired row has been re-opened no-follow and its durable identity proved equal to the action directory's | same | `admission_mutation.rs:296-307` (`AdmissionFinalPublish`/`FinalReobserve` pair) |
| 9 | `terminal.catalog_barrier` | the catalog root's dirent barrier has returned, so the retirement's namespace transition is ordered — `ExactInterior` class (`platform.rs:373`), documented no-op on Windows (`:526-567`) | `barrier_catalog_root` (`NEW@E3.1`) | `namespace_mutation.rs:325-336`; the ready-edge prologue precedent, `GwzM5-8R2DPhase4Closure.md:264-269` |
| 10 | `terminal.terminal_revalidate` | the completed-catalog predicate has been **re-proved after** the retirement: `interior::completed_record` is `Some` and the retained-catalog revalidation passes (`completed.rs:183-211`) | same function's tail (`NEW@E3.1`) | `completed.rs:64`, `:183-211`; **blocked by OPEN-T1** |
| 11 | `terminal.authority_release` | — see the reserved-with-reason determination below — | **none** | — |

**Reserved with reason — `terminal.authority_release`.**
It has two possible readings and **neither is a durable edge of its own**:

- *Reading (a), the in-process one.* `RetainedWriteAuthorityV1` is deliberately
  neither `Copy` nor `Clone` (`coordinator/execution.rs:321-327`), so "release"
  is a move-out — a purely in-memory event a restart cannot observe.
- *Reading (b), the catalog one.* Returning the admission record to `Idle` so
  the catalog can admit the next action **is** durable — and it already carries
  five executed keys: `admission.idle_scratch_create` / `_write` / `_flush`,
  `admission.idle_publish`, `admission.idle_reobserve`
  (`fault_v1.rs:92-96`), driven by `write_admission_scratch` /
  `publish_admission_record` (`admission_mutation.rs:87-93`, hits at `:191`,
  `:199`, `:296`, `:307`, `:410-431`).

Binding this key to reading (b) would announce **two keys at one boundary**,
which is the mirror image of the `staging_directory_flush` case the freeze
resolved the other way — one key, two sites, "because both are the state that
key names and minting a second would move the frozen census" (freeze
`:646`-`:653`). One boundary must have one name.
So the determination, in the Phase-3 settle's own form (freeze `:692`-`:706`):
**`terminal.authority_release` never gains a fault boundary.** It names a
state — "this action holds no write authority any more" — whose correct
evidence form is the coordinator suite
(`coordinator/execution.rs:248-285`'s four gates and their tests), as fault
injection is the correct form for an edge. It remains in the vocabulary; census
stays 165/11; the key-by-key siteless proof in `fault_expected_keys.rs` is its
permanent evidence.
**Rejected alternative:** activate it at reading (b)'s boundary anyway.
Rejected on the one-boundary-one-name ground above.

### 4.4 The OPEN row this family carries

**OPEN-T1 — BLOCKING for E3.1. `completed_record` requires an *empty*
`RetiredActions` root, so the first terminal retirement un-completes the
catalog.**

*The exact finding, verified in code:*
`completed_record` calls `empty_directory_identity(interior,
Slot::RetiredActions)` (`interior.rs:349`), which returns `Some` **only** for
`RawCatalogInteriorFactV1::EmptyDirectory` (`interior.rs:808-818`); the
observer produces that fact only when the retired root's first `entries()` item
is `None` (`interior.rs:465-474`, reached because `observe` passes
`slot == InfrastructureSlotV1::RetiredActions` as the `probe_empty_directory`
argument, `:108-113`). So the instant a retired action directory is resident:
`completed_record` → `None` → `retain_completed_catalog` refuses
(`completed.rs:64-74`) → the retained-catalog revalidation refuses
(`completed.rs:183-211`) → the publication destination recheck refuses
(`publication.rs:225`, `:264`). **The catalog would become unusable at its own
first terminal retirement**, and `terminal.terminal_revalidate` (key #10) is by
construction the boundary that cannot pass.

*Why this is a contradiction inside the frozen surface, not a design choice:*
the same protocol already budgets 64 retired action directories and enforces a
retirement-credit rule against them (`bounds.rs:2`, `:60-66`, `:120-142`), and
the catalog-root census already carries a `retired` counter
(`protocol/admission.rs:438`, charged at `:449`). The bounds expect a populated
retired root; the completion predicate forbids one.

*Candidate mechanism:* the **C-3-style widening** — replace the emptiness probe
for `RetiredActions` with a bounded, classified reading of its interior under
`RETIRED_ROOT_BUDGET_V1`, accepting a root whose every child is a valid
`RootEntryNameV1::ActiveAction` row (DECISION T-A's name) and whose count is
within `MAX_RETIRED_ACTION_DIRS`. The freeze's Class 2 ruling authorizes exactly
this shape and states its limits (`:1443`-`:1450`).

*Evidence that would settle it:* (a) whether the descriptor bytes move — they
do **not**: `owner_issue_for_catalog` consumes the retired root's **durable
identity** (`interior.rs:349`, `:352-359`), which a child addition does not
change, so `RetiredActionsDescriptor` and `CatalogFormat`
(`interior.rs:361-362`) stay byte-identical; (b) whether the widening needs its
own enumeration budget — `RETIRED_ROOT_BUDGET_V1` already exists
(`bounds.rs:60-66`) and has no production consumer, which is affirmative
evidence that it was frozen *for* this; (c) whether the census gains a class —
`CatalogRootRowClassV1` is a *root* grammar and the retired root's children are
a different parent, so E3.1 must state whether it reuses `ActionSlotV1::parse`'s
sibling `RootEntryNameV1::parse` (`slots.rs:381-447`) or adds a bounded reader.
**Drafter's recommendation: the widening.** Not taken as a decision here for the
same reason as OPEN-B1: `completed_record` is the predicate every catalog
consumer depends on, and the dual should authorize the edit explicitly.

**Note for the dual:** OPEN-B1 and OPEN-T1 are the same defect class — a
completion predicate written when the catalog had no lifecycle, meeting the
lifecycle. They should be ruled together, and if the dual rules them a single
package, E2.1 and E3.1 share one precondition step rather than two.

### 4.5 The convergence obligation (E3.2)

Same form as §2.5 and §3.6, with `TERMINAL_MATRIX: [Fault; 10]` (ten, not
eleven — §4.3's determination), both target variants, and the
`PartiallyExecuted("...", TERMINAL_EXECUTED_KEYS)` fixture row with its
key-by-key siteless proof for `terminal.authority_release`
(`fault_expected_keys.rs:187-222`, `:390-394`).

---

## 5. The E0.1(a) blast-radius decision, in frozen text

### 5.1 The question, quoted

`GwzM5-8R2E-E01ReachTraces.md:58-64`:

> 2. **The blast radius is an explicit E0.2 amendment decision**: does E4.1 probe
>    at every mutator-lock acquisition (every mutation refuses on such
>    filesystems) or only on the operations that consume the catalog
>    (checked-artifact actions refuse; ordinary merges proceed)? The conservative
>    reading of the §10 table's "runtime bootstrap only" row is the second; the
>    amendment must say so in frozen text either way.

The frozen §10 row (`GwzM5-8R4bR2ConsumerCheckpoint.md:272`):

> | `WorkspaceMutatorLock` | runtime bootstrap only | acquire final lease before open-operation gate and mutation |

### 5.2 DECISION — option (ii): probe only where the catalog is consumed

> **E4.1 blast-radius decision (DRAFT 2026-08-27, R2-E Step E0.2).**
> `WorkspaceMutatorLock::try_acquire` **does not construct a catalog-lease
> witness and executes no `DurableObjectIdentity` probe**, at E4.1 or after. The
> probe stays where it is today: inside the catalog lease's own facts and
> revalidation. On a Linux filesystem that refuses persistent file handles, an
> ORDINARY merge, `gwz repo create`, `init-from-sources`, abort, GC and the
> mutation guard continue to work; a checked-artifact action refuses, typed.

**Ground 1 — the code already separates them, and option (ii) is the reading
that costs nothing.**
`try_acquire` (`bootstrap/runtime/mod.rs:106-149`) performs path resolution,
ambient retention, repository revalidation, the bootstrap guard, the runtime and
locks directories, and the lease file. Its retention and revalidation use
metadata (`dev`/`ino`) identity, not durable identity —
`retain_ambient_directory` / `revalidate_ambient_directory`
(`bootstrap/runtime/paths.rs:69-83`, `:85-100`) compare an `identity(&metadata)`
value; and `revalidate_workspace_catalog_target`
(`bootstrap/runtime/mod.rs:202-235`) is built entirely from those helpers plus
`revalidate_child_directory` / `revalidate_file`. The durable probe lives one
layer down: `CatalogLeaseTargetWitnessV1::facts` calls
`platform.dir_identity(...)` and `platform.rename_domain(...)`
(`catalog_lease/witness.rs:53-54`, `:60`, `:66-67`), and
`CatalogTargetV1::finish`/`revalidate` do the same
(`catalog_lease/target.rs:142-143`, `:154-156`, `:216-217`, `:222`, `:227-228`)
— which reach `name_to_handle_at` (`identity.rs:267`) and the statx `MNT_ID`
rename domain (`identity.rs:304-316`). *[Cite corrected 2026-09-01, E4.1
review flag 1 (confirmed by the reviewer, decides precondition 1's true
module): `dir_identity`/`rename_domain` resolve to the CATALOG's own
providers — `capability/pre_catalog/provider/platform/{linux,macos,windows,
unsupported}.rs` — which implement the same syscall class themselves;
`checked_artifact/identity.rs` is the LEGACY writer's module and is NOT on
the catalog-lease path. E4.1's capability cure was therefore applied to
both modules.]* Notably `witness.rs:31-35`'s `try_new`
calls only `revalidate()`, which for the workspace-runtime source delegates to
`runtime.revalidate_catalog_target()` (`witness.rs:38-41`) — i.e. **even
constructing the witness does not probe**; `facts()` does.

**Ground 2 — availability. Option (i) would cost a Linux user every mutation,
not just the checked ones.**
`WorkspaceMutatorLock::acquire`/`try_acquire`
(`operation/workspace_mutator_lock.rs:14-38`) has **nine** production call
sites *[census moved to TEN at E4.1(c), 2026-09-01: dispatch's
`forward_lifecycle_viability_window` (`runtime/dispatch.rs`) acquires the
lock for the activate-before-upgrade window on the A1 adaptation path —
review round 2 verified deadlock-freedom three ways; the ground's
availability argument is unchanged]*: `workspace_ops/handle_create_repo.rs:35`,
`workspace_ops/handle_init_from_sources.rs:91`,
`workspace_ops/merge/gc.rs:167`, `workspace_ops/merge/abort/mod.rs:44`,
`workspace_ops/merge/runtime/mutation_guard.rs:44`,
`workspace_ops/merge/continue_op/coordinator.rs:13`,
`workspace_ops/merge/v1_lifecycle/archive.rs:97` and `:126`, and
`workspace_ops/merge/v1_lifecycle/checked.rs:129`. Two of them —
repo creation and init-from-sources — have no relation to the catalog at all.
Option (i) turns a filesystem-capability gap into a **total loss of gwz** on
that filesystem; option (ii) turns it into a refusal of the checked-artifact
feature. The ubuntu-22.04 image class E0.1(a) identified (certain
tmpfs/overlay/NFS/older filesystems) is not exotic.

**Ground 3 — fail-closed doctrine, correctly scoped.**
Fail-closed means *refuse rather than proceed without the proof an operation
needs*. It does not mean *refuse operations that need no proof*. The lock's own
proof obligations — advisory exclusivity, ambient revalidation, repository
membership — are met without a durable identity, and are enforced today
(`bootstrap/runtime/mod.rs:110-149`, `:202-235`). Requiring an unrelated
capability at the lock would be **over-refusal**, which is a different failure
mode and one the freeze has consistently rejected: the Step-4.2 remediation
fixed exactly this shape (a permanent typed refusal on a *reachable* state that
"bricked P5 for all seven `AnchoredPrivateArea` callers with no in-code exit",
freeze `:814`-`:825`). Fail-closed binds where the witness is **dereferenced**,
and option (ii) keeps it there.

**Ground 4 — witness freshness gains nothing from an earlier probe.**
What the catalog-lease witness guards is *staleness of the target binding across
the lease's life*: `CatalogTargetV1::revalidate`
(`catalog_lease/target.rs:191-238`) re-compares durable identity, invocation
identity, support profile, parent mode and rename domain for both the target and
the related Git directory, plus the repository/worktree association
(`catalog_lease/association.rs:173-193`), and refuses "stable or live target or
related Git binding changed". Its freshness requirement is **per catalog
consumption**, and it is already met per consumption: `facts()` calls
`revalidate()` before reading anything (`witness.rs:49`). Probing additionally
at lock acquisition adds no freshness — it only adds an earlier failure point
for a proof nothing at that point uses.

**Weighed and rejected: the diagnostic argument for option (i).** An early probe
would fail fast with one clear message instead of failing later inside an
operation. That is a UX property, not an availability or safety one, and it is
obtainable without option (i)'s cost by making the refusal typed and actionable
where it occurs — which §5.3 makes a hard precondition. Buying a better error
message with the loss of ordinary merges on a whole filesystem class is the
wrong trade.

### 5.3 The capability-refusal UX obligation, as E4.1-precondition text

Restating E0.1(a)'s own consequence verbatim (`GwzM5-8R2E-E01ReachTraces.md:52-56`):

> 1. **The capability-refusal UX is an E4.1 hard precondition** (plan O11/E6.3
>    folds into E4.1's gate): a typed, actionable message naming the filesystem
>    capability, a runbook class for it, and a release-notes line — before, not
>    after, the first production caller.

Made concrete by this decision, E4.1 may not land until all five hold:

1. **The refusal is typed and names the capability.** The vocabulary already
   exists — `CheckedFsError::unsupported(PlatformCapability::…)` is the shape
   used for the advisory-lock gap (`bootstrap/runtime/mod.rs:237-243`). The
   durable-identity gap must have its own `PlatformCapability` value, so the
   message names *persistent file handles* / *mount identity* rather than
   surfacing a raw `errno`. Today the failure arrives as
   `std::io::Error::last_os_error()` from `identity.rs:275-277` or the typed
   `Unsupported` at `:279-284` / `:310-315`, and neither reaches the user as an
   actionable sentence.
2. **The refusal is scoped**, i.e. this decision is implemented: no probe at
   `try_acquire`; the refusal appears only on the checked-artifact path.
3. **A runbook class** naming the filesystem families and the workaround
   (run the workspace on a filesystem exposing persistent handles).
4. **A release-notes line** for the first release carrying E4.1.
5. **The `platform/unsupported.rs` Linux-profile claim is swept in the same
   package** — E0.1(b) row 3 explicitly ties it here ("note the adjacency to
   (a): the capability-refusal UX work should sweep this claim"), so it moves
   from O12/E6.2 into E4.1's precondition set rather than staying an E6 nit.

**Frozen-text statement, for §4.3/§10 annotation:** *the `WorkspaceMutatorLock`
row's declared purpose — "runtime bootstrap only" — is load-bearing at E4.1:
the runtime lease is bootstrap-only and identity-probe-free; the catalog lease
is where the durable-identity capability is required and where its absence is
refused.*

---

## 6. The O8 archive-equivalence mechanism decision

### 6.1 The L6 ruling, restated

`GwzM5-8A1ActivationRecord.md` §6 (`:148`-`:166`):

> - **The 18-UNBOUND per-scenario record debt, the archive-equivalence mechanism
>   decision, and the two archive shapes riding [P2-1] option (i)** → **R2-E's
>   archive/GC consumer sub-package** (one named owner; chartered with R2-E's
>   binding-obligation ledger). The gc_archived no-production-caller allowance
>   (`archive.rs:110`, reasoned, route named) is the same owner's.
> - **Cheap closures (i)/(ii) of `GwzM5-8R4bG-Evidence.md` §12.7** (registry rows
>   for the unbound scenarios; one parametric `adapt_open` refusal test over
>   every non-whitelisted scenario): ruled **NOT satisfied by the precheck walk
>   test** — the precheck reads state+mode and never drives `adapt_open`, so the
>   walk test covers a different layer. Both closures are **OWNED, together, by
>   the same R2-E archive/GC consumer sub-package** (the registry rows and the
>   parametric test land as one package).

**Restated as an R2-E obligation:** E5.1 lands the registry rows **and** the one
parametric `adapt_open` refusal test **in one commit**. Neither may land alone;
the precheck walk test does not discharge either.

### 6.2 What the evidence says the mechanism cannot be

`GwzM5-8R4bG-Evidence.md` §12.7 (`:1348`-`:1354`):

> (iii) an archive-equivalence **mechanism decision** for the ten archive shapes
> — the registry's two corpora model open-record adaptation only and there is
> **no registry vocabulary in which an archive shape could be bound**, so what
> O8's archive clause still owes is different and real: proof that operations
> *finishing under v1* produce archives byte-equivalent to v0's for the same
> scenario. No such comparison exists; that is A1-shaped work.

and §12.9(c) (`:1457`-`:1466`) gives the structural reason the corpus cannot be
widened: `valid_unlisted_corpus`'s runtime binding asserts the whitelist's own
closure property and then `assert_ne!(record.state, OperationState::Finalizing)`,
"so widening it to admit these rows would weaken the registry, not extend it."

The Safety axis put the same point as a mechanism constraint
(`GwzM5-8A1Activation-ReviewSafety.md:199-202`):

> 5. **The archive-equivalence mechanism decision** for the ten archive shapes
>    (R4b-G §12.7 closure (iii)) — owed at activation; the registry has no
>    vocabulary for archive shapes, so the decision is a mechanism choice, not a
>    registry edit.

### 6.3 DECISION — a two-tier mechanism, because the frozen contract states two different claims

The frozen contract, `GwzM5-8I2CompatibilityContract.md` §5 (quoted from the
current file at `:178`-`:184`):

> Zero whitelist matches is not an error. Open read-only status leaves bytes
> unchanged and projects v0 source/version with acceptance and recovery absent.
> An existing mutating v0 command remains on the existing v0 lifecycle and may
> write v0 only when that path's existing preflight authorizes it. **Completed
> and aborted v0 records remain v0 and use byte-preserving archival. Archived v0
> uses only the archive decoder/projection below.**

*(Citation-drift note for the dual, verified: R4b-G §12.7 cites this passage as
contract `:159-165` and the whitelist passage as `:117-125`; in the file as it
stands they are at `:178-184` and `:136-144` — a uniform **+19-line** drift.
Every quotation in this amendment uses the current line numbers. E5.2 should
re-verify R4b-G's cites rather than copy them.)*

That sentence states **two** claims, and they need **two** checks:

> **O8 archive-equivalence mechanism (DRAFT 2026-08-27, R2-E Step E0.2).** The
> ten archive shapes are bound by two per-scenario checks, in a **standalone
> archive corpus, not the migration registry**:
>
> **Tier 1 — byte-preservation, by digest, for v0-origin archives.** Per archive
> shape: SHA256 of the archived record bytes equals SHA256 of the open v0 record
> bytes that preceded them. This is precisely what "byte-preserving archival"
> asserts, it is a digest comparison against the archived bytes, and it is
> checkable today: `CanonicalArchiveAcquisition::acquire` retains the exact
> destination bytes (`v1_lifecycle/archive.rs:32-46`) and `decode_archived` is
> the only decoder on the archive path (`archive.rs:41`, `:50`).
>
> **Tier 2 — projection equivalence, by canonical-JSON digest, for archives an
> operation finished under v1.** Per archive shape: the archive
> decoder/projection output of the v1-produced archive equals, on the frozen
> projection surface, that of the v0-produced archive for the same scenario.
> **Byte equality is unavailable by construction here** — a v1-finished
> operation writes a v1 envelope, so the header pair differs — and the contract
> itself never claims it: its byte-preservation clause is about records that
> *remain v0*. The instrument is the one the seven ADAPTED cases already use:
> canonical-JSON SHA256 (`GwzM5-8R4bG-Evidence.md:1381-1387`).
>
> **Rejected alternative — one byte-digest gate over both tiers.** Rejected
> because it is **unsatisfiable for tier 2 by construction**, so adopting it
> would either install a permanently-red gate or invite a lane to satisfy the
> clause by comparing only v0-origin archives and reporting the O8 clause met.
> The PARTIAL statement (`§12.8`) exists to stop exactly that.
>
> **No registry rows for archive shapes.** Per §12.7 and §12.9(c): the archive
> rows live in their own corpus and are cited by clause, in the shape §12.9's
> disposition table uses for the F rows. Cheap closure (i)'s registry rows are
> for the **progress** shapes only.

### 6.4 The two archive shapes' dispositions

The two shapes are `AC-NOPUB-UNBORN` and `AP-PRESERVED`
(`GwzM5-8R4bG-Evidence.md:1356-1365`). They are simultaneously (a) two of C-2's
four unfixtured scenarios and (b) the "two archive shapes riding [P2-1] option
(i)" the Safety axis let ride as a named residual
(`GwzM5-8A1Activation-ReviewSafety.md:574-582`; the package's own
`GwzM5-8A1ActivationPackage-Report.md:90` records "*The two archive shapes ride
as named residual, per the finding.*").

Their reachability is settled and unchanged
(`GwzM5-8A1Activation-ReviewSafety.md:352-355`):

> The two archive shapes (`AC-NOPUB-UNBORN`, `AP-PRESERVED`) gain no new
> reachability: archived v0 never migrates and uses only the archive
> decoder/projection.

> **Disposition (DRAFT 2026-08-27).** Both are **DISPOSITIONED-PROJECTION-ONLY,
> UNFIXTURED**. They take no registry row (§6.3), and they take no adaptation
> disposition, because archived v0 never enters `adapt_open`. Their tier-1 and
> tier-2 rows are **owed and declared PENDING-FIXTURE**, with the carrier named:
> **R2-F's fixtures/native-evidence lane**, per L6's own routing of C-2's four
> scenarios. E5.2 lands their corpus rows in the declared-pending form —
> disposition stated, clause cited, test named as owed — so the per-scenario
> record exists where `check_merge_compatibility_predicates.py` looks, and the
> row flips to executed when R2-F's fixtures land. **E5.2 must not report the O8
> archive clause met on these two rows**, and the PARTIAL statement
> (`GwzM5-8R4bG-Evidence.md:1367-1395`) survives R2-E unchanged for them.

**And the `gc_archived` production route.** `gc_archived`
(`v1_lifecycle/archive.rs:112-118`) carries a reasoned no-production-caller
allowance whose text names the missing route (`:108-111`). It rides **E4.4**
with the terminal-archive consumer, per the plan's O8 row; when it lands, the
`allow(dead_code)` and its reason expire with a dated annotation, exactly as
E4.7 does for the `catalog.rs:10-16` class.

---

## 7. The §11.3 consumption table

Every row of E0.1(b), consumed here or deferred with an owner and terms.

| # | §11.3 row (E0.1(b)) | Disposition | Where / terms |
| --- | --- | --- | --- |
| 1 | `BarrierIntentV1::issue` — BINDING, ordering constraint | **CONSUMED HERE** | §3.1-§3.2: the O6 RESOLUTION (OBSERVE both facts, DERIVE the home name, one typed REFUSE), the owner-minted witness mechanism, and the ordering constraint quoted verbatim and made a load-bearing plan property. Residual: **OPEN-B1** and **OPEN-B2** must resolve before E2.1 can build the edges the resolution governs. |
| 2 | `authority_name` self-checking weigh | **TAKEN HERE** (decision below) | §7.1 |
| 3 | Anchor nits (accepted same-user boundary) | **SPLIT: one consumed, two deferred with terms** | §7.2 |
| 4 | Ten multi-component writer rows' native Windows/ARM ledger debt | **DEFERRED, owner named, terms stated** | §7.3 |
| 5 | The 481-item frozen surface behind the seven blankets | **DEFERRED to E7.2** | §7.4 |

### 7.1 Row 2 — the `authority_name` weigh, TAKEN

The weigh, quoted (`GwzM5-8R2DSettledTuple.md:649-652`):

> `authority_name` embeds no identity digest (the non-self-checking-name design;
> descends from the Step-4.1 review's E21 P2 finding) — R2-E must weigh a
> self-checking rename or a compensating observation when it gives the family its
> next consumer (checkpoint 4.1 record; State [P3-2]).

**The measurement.** `authority_name(family, action)` renders
`"ca1-{family}-{action}.authority"` (`authority.rs:226-228`) — no identity —
while its siblings `goal_name` and `source_name` embed the object's 16-byte
identity in hex (`authority.rs:234-236`, `:238`+). So the legacy authority row's
name is not self-checking, and the legacy `finish()` is the consumer that acts
on it (`cleanup.rs:159-174`).

**DECISION A-1 — the compensating observation is adopted, and it is already
structural on both paths; `authority_name` is NOT renamed.**

- *On the checked path* (the family's genuine next consumer, §2), the name is
  never trusted at all. The retirement retains its source, proves identity and
  bytes, and renames the exact identity-checked handle:
  `PublicationSourceV1::regular_file(&source.encoded_identity, &source.bytes)`
  (`namespace_mutation.rs:280-288`) is verified inside
  `publish_verified_no_replace` by identity (`publication.rs:148-155`) and by
  byte comparison (`:156-179`); the destination is schedule-derived
  (`namespace/mod.rs:137-147`) and proved free first
  (`namespace_mutation.rs:263-265`). A self-checking *name* would add nothing to
  a source-associated *rename*.
- *On the legacy path*, a compensating observation already exists and is
  executed: before the authority removal, `finish()` re-inspects the family and
  requires `rechecked.authority.as_ref() == Some(authority)` with no source and
  no goal residue and the durable destination equal to the goal
  (`cleanup.rs:146-158`), then re-barriers the exact entry through its own
  handle (`residue.rs:570-596`).

**Rejected alternative — rename `authority_name` to embed the authority
record's digest.** Rejected on three grounds: it edits a durable legacy name
grammar that `inspect_family`'s `ca1-{family}-` prefix scan and older gwz
readers classify against (`authority.rs:230-232`); it buys a property the
checked path obtains structurally; and legacy-path removal is R2-F's
(`GwzM5-8R2DSettledTuple.md:793`), so the rename's only beneficiary is a path
scheduled for deletion.

**Condition attached, for E4.7.** This decision rests on the legacy compensating
observation continuing to run wherever the legacy `finish()` still executes.
E4.7 must record, per converted consumer, whether `cleanup.rs`'s `finish()` is
still reachable for it; if any converted consumer bypasses both the checked
retirement and the legacy recheck, this decision reopens.

### 7.2 Row 3 — the anchor nits, split

| Nit | Disposition |
| --- | --- |
| `verify` rides the legacy unbounded `read_to_end` (`platform/anchor.rs:394`) | **DEFERRED to E6.2**, terms: inside the accepted same-user boundary; the fix is a `take(ANCHOR_BYTES.len()+1)` bound, one line, no behaviour change on a well-formed anchor. |
| The retired-ordinal parse accepts non-canonical renderings (`platform/anchor.rs:371-386`) | **DEFERRED to E6.2**, terms: convergence unaffected (`smallest_free_ordinal` reads residency, not text); the fix is a canonical two-digit parse. |
| `platform/unsupported.rs` claims the Linux support profile | **CONSUMED HERE — moved to §5.3 item 5**, i.e. into E4.1's precondition set rather than E6.2, on E0.1(b) row 3's own instruction ("the capability-refusal UX work should sweep this claim"). Ground: the claim and the refusal UX are one story; separating them would ship a message that names a capability the profile table says is present. |

### 7.3 Row 4 — the ten multi-component writer rows' native ledger debt

**DEFERRED, with the owner and the trigger named.** Terms: expected-green; the
entry is owed at the **next matrix dispatch**, which under this plan is the
first E-landing that runs a native matrix — nominally **E1.2**, or whichever of
E1/E2/E3 lands first if they run in parallel. The record goes in the
classification ledger, in the form the Step-4.2 record used
(freeze `:836`-`:838`), and E7.2's three-platform acceptance re-verifies it.
Not consumed here because this amendment executes nothing.

### 7.4 Row 5 — the 481-item frozen surface

**DEFERRED to E7.2**, as the plan already assigns (`GwzM5-8R2E-Plan.md` §3 E7.2,
"the 481-item frozen-surface reconciliation"). Terms, restated so E7.2 inherits
the right denominator: the **481** figure is the blanket-hidden subset across 50
files, and the global measure is **1657 dead-code spans across 85 files**, the
difference being items behind narrower allows — including
`coordinator/execution.rs`'s 26 R2-E-gated items
(`GwzM5-8R2DPhase4Closure.md:344-348`). E7.2 must reconcile against both figures
and must expect the count to *fall* as E4 converts consumers, not merely be
restated.

---

## 8. The §4.3 E-table annotations these records imply

Each row below gains a **dated annotation beside it**; the row itself is left as
written. One line of content each, as the amendment proposes it.

| Target | Annotation content (one line) |
| --- | --- |
| **§4.3 row E7** (`:1083`) | Its Phase-4 half is R2-E's: the eleven `terminal.*` keys bind to this edge (§4), the destination child name is the already-derived `RootEntryNameV1::ActiveAction` name under the retired root (DECISION T-A), and the edge is **blocked on OPEN-T1** until `completed_record`'s empty-retired-root predicate is widened. |
| **§4.3 rows E12 / E13** (`:1105`, `:1106`) | The shared `execute_edge` helper (`namespace_mutation.rs:250-305`) gains cleanup, barrier and terminal callers; the renames keep announcing the four `namespace.*` keys and **no `namespace.*` key is re-keyed or silenced**, per the Step-3.1b routing precedent (`managed_mutation.rs:1018-1027`) — DECISION C-1. |
| **§4.3 row E14** (`:1107`) | `barrier.target_barrier` is a **distinct P5 caller** at a *target* parent with a **supplied** roaming anchor, taking a third `DirentBarrierClass` (DECISION B-3); the E10/E14 annotation's "E10 and E14 both pass `ExactInterior` and can never take the other arm" is untouched, because both of its hardcoded sites (`namespace_mutation.rs:328`; `namespace/host.rs:741-756`) are in a different file from this family's. |
| **The E10/E14 activation annotation** (`:1108`-`:1199`) | Its Windows negative space is unchanged; but its ground — "only the checked-artifact private area is [permitted to retain an anchor]" (`:1116`-`:1118`) — gains the roaming case: a directory may *borrow* an anchor for one barrier without retaining one, which is what the sixteen `barrier.*` keys buy. |
| **§4.1 row P5** (`:980`) | The Windows column becomes **three** arms: the resident-anchor round trip (`AnchoredPrivateArea`), the documented no-op (`ExactInterior`), and the supplied-anchor round trip (`RoamingAnchoredTarget`, DECISION B-3); off Windows all three remain the same directory `fsync` (`platform.rs:377-384`). |
| **§4.4 Class 1 arm table, last row** (`:1382`) | "any further retirement-destination arm | E7's Phase-4 half and the terminal retirement edges | Phase 4" resolves to **no new variant — one field generalization of `AdmissionCatalogInterior`'s `absent` field, owned by R2-E E3.1** (DECISION T-B), on Class 1's own struct-vs-enum criterion (`:1356`-`:1364`). |
| **§4.4 Class 2 (C-3)** (`:1398`-`:1450`) | Its ruling — extend the provider's *reading*, not the vocabulary — is the named precedent for **both** OPEN-B1 and OPEN-T1; R2-E proposes two further readings of the same closed grammar (the roaming-anchor-away home row; the populated retired root) and mints no vocabulary for either. |
| **The E4-retire activation record** (`:1247`-`:1325`) | Its clause "`completed_record` requires an **empty** `RetiredActions` root, so no in-vocabulary target exists" (`:1272`-`:1276`) stays true **of the admission record slot it was written about**; for the *action directory* an in-vocabulary name already exists (DECISION T-A) and the empty-root half is OPEN-T1's object. Its "Open question carried to the re-verdict — the fault key, deliberately not minted" (`:1313`-`:1325`) is **not reopened**: R2-E mints no key. |
| **§4.3 row E22** (`:1245`) and the Step-4.2 record's `:850`-`:856` | The legacy nonce-orphan reclamation the record calls "an optional R2-E item, not a debt of this step" is **DECLINED by R2-E** and stays optional: the count is bounded by past crashes and cannot grow, `legacy_nonce_orphans_are_tolerated_and_block_nothing` pins it, and the natural carrier is R2-F's legacy-path removal. Recorded so the option does not silently lapse. |
| **§3.5 map rows** `:707`, `:763`, `:791` | Each gains its family's activation record — §2, §3, §4 of this amendment — dated, quote-preserving, with the row left as written. |
| **§3.5 injection-site inventory** (`:943`-`:968`) and the **inventory addendum** (`:525`-`:532`) | The declared-and-anchored production source list moves from **nine** files to **eleven** as E1/E2/E3 land: `namespace_mutation.rs` gains the eleven `cleanup.*` sites, a new `barrier_mutation.rs` holds the sixteen `barrier.*` sites, and `admission_mutation.rs` gains the ten activated `terminal.*` sites — each declared in `FAULT_INJECTION_SOURCES` in its own converting commit, with the completeness anchor (`fault_expected_keys.rs:414-418`) proving the list exhaustive. |
| **§3.5's total-165 statement** (`:922`-`:923`) | Unchanged, and restated in each activation record: 165 total, no key minted, no key retired. |

---

## 9. What this amendment does NOT do

1. **No census move.** `EXPECTED_KEY_COUNT` stays 165
   (`fault_expected_keys.rs:174`); no key is minted or retired; the family
   counts 11/16/11 are unchanged. Every decision above was checked against this
   constraint, and two of them (DECISION B-3's third `DirentBarrierClass`,
   DECISION T-B's field generalization) were chosen partly because they move no
   census.
2. **No `FAULT_FAMILY_ACTIVATION` edit.** The three `Reserved(...)` rows
   (`fault_expected_keys.rs:380-394`) stay `Reserved` until E1.2 / E2.3 / E3.2
   flip them **in the commits that convert their edges** — RemPlan §10's duty,
   which this amendment restates rather than discharges.
3. **No production code.** No injection site is added, no function is written,
   no `DirentBarrierClass` variant, no `DestinationRecheckV1` field change, no
   `completed_record` widening. Those are E1-E3, gated on OPEN-B1 and OPEN-T1.
4. **No consumer conversion and no catalog activation.** The §10 table stays
   unexecuted; `recover_or_create` still has zero production callers; the A1
   coexistence gate (`GwzM5-8R2DSettledTuple.md:800-802`) is untouched. §5 is a
   *decision about* E4.1, not E4.1.
5. **No registry rows and no archive tests.** §6 decides the mechanism; E5.1
   lands the rows and the parametric test together (L6), E5.2 lands the
   equivalence rows.
6. **It does not resolve its own OPEN rows.** OPEN-B1, OPEN-B2, OPEN-B3,
   OPEN-C1 and OPEN-T1 are named with their exact questions, their candidate
   mechanisms and the evidence that would settle each. Two of them (B1, T1) are
   **blocking** and the drafter recommends the dual rule them together.
7. **No tree mutation of any kind was performed to produce this draft.** It is
   read-only work over dev-docs and `gwz-core/src`, per the plan's E0 discipline.

---

## Appendix A — OPEN register (for the E0.3 dual)

| Id | Row | Grade | Blocking | Owner |
| --- | --- | --- | --- | --- |
| **OPEN-B1** | The roaming anchor cannot leave home while `completed_record` requires it resident (`interior.rs:350`) | design contradiction inside the frozen surface | **YES — E2.1** | dual #1 to rule; E2.1 to implement |
| **OPEN-T1** | `completed_record` requires an **empty** `RetiredActions` root (`interior.rs:349`, `:808-818`, `:465-474`), so the first terminal retirement un-completes the catalog | design contradiction inside the frozen surface | **YES — E3.1** | dual #1 to rule; E3.1 to implement |
| **OPEN-B2** | The barrier target is pinned to the action directory (`namespace/host.rs:165-171`), so the anchor has nowhere to roam | scope question | no (E2.1 must answer in-commit) | E2.1 |
| **OPEN-B3** | The reserved target leaf is caller-chosen (`namespace/host.rs:152`); grammar unconstrained | hygiene / product-surface question | no | E2.1 |
| **OPEN-C1** | `BaseActionSlotV1::RecordScratch` is shared between the authority record's scratch (`authority_record_binding.rs:508`/`:516`/`:529`) and, under DECISION C-3, the cleanup worklist's; the consumption ordering must be proved, not assumed | ordering condition on an adopted decision | no (E1.1 must prove it in-commit; if unprovable, C-3 reopens) | E1.1 |

## Appendix B — decision register

| Id | Decision | Rejected alternative |
| --- | --- | --- |
| **C-1** | cleanup retirements keep announcing `namespace.retire_exact`; `cleanup.alias_retire` names the post-edge state | a `Cleanup` variant of `ActionNamespaceEdgeV1` (would silence four executed `namespace.*` keys) |
| **C-2** | cleanup sites in `namespace_mutation.rs` | a new `cleanup_mutation.rs` (cohesion arithmetic; fallback if E1 measures > ~250 added lines) |
| **C-3** | the worklist's scratch is the existing shared `BaseActionSlotV1::RecordScratch`; no slot minted — carries ordering condition **OPEN-C1** | mint `CleanupWorklistScratch` (persisted name; would move `BASE_ACTION_SLOTS` and the `MAX_ACTION_SLOTS == 261` assertion) |
| **B-1** | barrier sites in a new `barrier_mutation.rs` | `namespace_mutation.rs` (cohesion) |
| **B-2** | the five intent keys follow `ManagedIntentEdgeV1`'s scratch/edge fault shape | a bespoke shape |
| **B-3** | a third `DirentBarrierClass` for the supplied-anchor target barrier | pass `AnchoredPrivateArea` (would establish a permanent anchor in a target parent) |
| **O6** | OBSERVE anchor identity and home identity from an owner-minted witness; DERIVE the home name; typed REFUSE where the observation cannot be made | keep the three parameters and compare inside `barrier_intent` |
| **T-A** | the retired child's name is the derived `RootEntryNameV1::ActiveAction` name under a different parent | a `RetiredAction(digest)` grammar arm |
| **T-B** | the terminal destination arm is a field generalization of `AdmissionCatalogInterior` | a fourth `DestinationRecheckV1` variant |
| **T-C** | terminal sites in `admission_mutation.rs` | `namespace_mutation.rs` |
| **T-D** | `terminal.authority_release` **reserved with determination** — never gains a boundary | activate it at the return-to-idle edge (would double-name one boundary) |
| **E4.1 blast radius** | option (ii): probe only where the catalog is consumed | option (i): probe at every mutator-lock acquisition (would refuse 9 production call sites incl. repo create / init) |
| **O8** | two-tier archive equivalence — tier 1 byte digest for v0-origin, tier 2 canonical-JSON projection digest for v1-finished; standalone archive corpus, no registry rows | one byte-digest gate over both tiers (unsatisfiable for tier 2 by construction) |
| **O8 shapes** | `AC-NOPUB-UNBORN` / `AP-PRESERVED` — DISPOSITIONED-PROJECTION-ONLY, UNFIXTURED, rows declared PENDING-FIXTURE with R2-F named | claim the clause met by class cite alone |
| **A-1** | the `authority_name` weigh answered by the compensating observation (structural on the checked path, executed on the legacy one); no rename | rename `authority_name` to embed the record digest |

## Appendix C — what the E0.3 dual should attack first

Offered by the drafter, not binding on the reviewers.

**Code axis.**

1. **OPEN-T1 and OPEN-B1, independently re-derived.** Both are load-bearing
   claims about `interior::completed_record` (`:333-364`). Re-walk
   `empty_directory_identity` (`:808-818`), `observe_slot`'s
   `probe_empty_directory` arm (`:465-474`) and `observe`'s call
   (`:108-113`), and the `exact_file_identity(.., RoamingAnchorHome, ..)`
   requirement (`:350`), then the three refusal sites
   (`completed.rs:64-74`, `:183-211`; `publication.rs:225`, `:264`). If either
   finding is wrong, §3 and §4 change shape.
2. **DECISION C-1's silence claim.** Verify that a `Cleanup` variant of
   `ActionNamespaceEdgeV1` really would silence the four `namespace.*`
   retirement keys (`namespace_mutation.rs:79-93`, `:263-302`) and that
   `namespace/tests_fault_matrix.rs` asserts they fire per retirement.
3. **DECISION B-3's Windows arm.** Check whether `platform/anchor.rs`'s closed
   grammar (`ANCHOR_PREFIX` `:76`, `survey` `:284`, `prepare` `:129`) can be
   given a supplied anchor without widening the grammar that
   `legacy_nonce_orphans_are_tolerated_and_block_nothing` pins.
4. **OPEN-C1.** Confirm whether `BaseActionSlotV1::RecordScratch` is in fact the
   authority record's scratch — the `record.scratch_*` sites
   (`authority_record_binding.rs:508`/`:516`/`:529`) do not name the slot, and
   the drafter inferred it from `PublishRoleV1`'s mapping
   (`namespace/roles.rs:20`/`:34`). If it is a different slot, DECISION C-3 gets
   easier, not harder.
5. **The site tables' `EXISTS`/`NEW@` markers**, row by row, against the files
   named — the convention is stated at §1.4 and the whole record's honesty rests
   on it.

**State axis.**

1. **The census invariant**, across all three records: 165, no key minted, no
   key retired, and the three `FAULT_FAMILY_ACTIVATION` rows still `Reserved`
   at the close of E0 (§9 items 1-2).
2. **DECISION T-D** (`terminal.authority_release` reserved with determination)
   against the Phase-3 settle determination's form (freeze `:692`-`:706`) and
   against §11.3 item 5's `staging_directory_flush` precedent (freeze
   `:646`-`:653`) — is one-boundary-one-name the right reading of both?
3. **§5's fail-closed argument.** The decision refuses to probe at the mutator
   lock. Test the doctrine claim directly: is "fail-closed binds where the
   witness is dereferenced" consistent with every prior fail-closed ruling in
   the corpus, or does some ruling bind it at capability acquisition?
4. **§6's two-tier mechanism** against O8's own words
   (`GwzM5-8R4bTransitionDesign.md:1476-1477` via §12.8) — does splitting the
   clause into two checks satisfy it, or narrow it? And is the PARTIAL statement
   preserved for the two unfixtured shapes?
5. **§7's consumption table**: five rows in, five rows accounted for, each
   either consumed with a decision or deferred with an owner **and** terms. Row
   3 is split — check the split is faithful to E0.1(b)'s own instruction.
6. **The citation-drift note in §6.3** — re-verify the +19-line drift between
   R4b-G's contract cites and the current `GwzM5-8I2CompatibilityContract.md`,
   since E5.2 will inherit whichever numbers this record blesses.
