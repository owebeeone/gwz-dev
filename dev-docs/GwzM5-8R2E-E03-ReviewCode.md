# R2-E E0.3 dual #1 — CODE-axis review of the semantics amendment

Reviewer: Code axis, E0.3 dual #1. Date: 2026-08-27. Peer-blind (no State-axis
material read).
Object: `dev-docs/GwzM5-8R2E-SemanticsAmendment-DRAFT.md` (1,396 lines).
Tree: working tree of `gwz-core` under `/Users/owebeeone/limbo/gwz-dev`, at
`d543a50`. Code paths below are relative to `gwz-core/src/checked_artifact/`
unless they begin `workspace_ops/`, `operation/` or `model/` (relative to
`gwz-core/src/`), matching the draft's own convention. dev-docs paths are
relative to `dev-docs/`.

Mandate order followed literally: **§1 below was derived from code before the
draft's §3.5/§4.4/Appendix arguments were read.**

---

## VERDICT

**NO-GO as drafted — revise and re-freeze.** Three [P1] defects would enter
frozen text: one recommended mechanism that the code cannot execute, one taken
DECISION that the code cannot implement, and one OPEN row derived from a
misreading that binds a whole phase to an unprovable condition.

This is a revise verdict, not a start-over. The amendment's spine is sound and
its citation discipline is the best I have audited in this corpus (see §8):
the 38 key semantics are individually defensible, the census invariant holds
against a green fixture, §5's blast-radius decision is correct and
correctly grounded, and §6's two-tier O8 mechanism and its +19-line drift catch
are right. **§5, §6, §7 and the 38 key-semantic rows may freeze as written**
once the corrections below land on §3.5, §4.2, §4.4, §2.2, §2.5 and §8.

Both blocking OPEN rows are **CONFIRMED as real defects**, independently
re-derived. But the drafter's recommendation on B1 is wrong, the drafter's
recommendation to rule them as one package is wrong, and both precondition
packages are three times larger than the draft states.

---

## 1. MANDATE 1 — the two blocking OPEN rows, independently re-derived

Derived from code before reading the draft's argument. Both derivations
terminate at the same place the draft does not name: the **recovery
classifier**, where the failure is not a refusal to retain but a permanent
`Ambiguous`.

### 1.1 The shared spine both derivations run through

```
provider/aggregate.rs:22   outer_aggregate_facts(binding, observed)
  ├─ :72-79   (CatalogRecognizedNameV1::Final, Directory{durable_identity, interior})
  │             → final_raw = RawDirectoryV1::Directory(..)
  └─ :109     directory_fact(CatalogPrivateNameV1::Final, final_raw, expected)
        → interior.rs:156  directory_fact(role, identity, interior, expected)
             :173-179   Final arm:
                        completed_record(..).is_some() ? ExactOwned : Other

catalog/classifier.rs:210-274  classify(...)
  :266-273  the ONLY tuple yielding CatalogBootstrapRecoveryDecisionV1::Complete
            requires  aggregate.final_directory == CatalogDirectoryFactV1::ExactOwned
  :274      _ => CatalogBootstrapRecoveryDecisionV1::Ambiguous

catalog/bootstrap.rs:283-288
  Ambiguous => Err(CheckedFsError::ambiguous(
                 "catalog bootstrap owner",
                 "aggregate catalog facts are ambiguous"))
```

There is **no recovery arm for a completed catalog whose `completed_record` is
`None`.** `recover_or_create` (`catalog/bootstrap.rs:213-229`) loops over
`execute_one`; every other decision returns `Retry`, `Ambiguous` returns `Err`.
So the failure mode is not "this process cannot retain the catalog" — it is
**"no process can ever recover this catalog again."** The draft's phrasing ("a
catalog no process can retain") understates it by one level.

### 1.2 OPEN-B1 — CONFIRMED. The roaming anchor cannot leave home.

`completed_record` (`provider/interior.rs:333-364`):

- `:350` `let roaming = exact_file_identity(interior, Slot::RoamingAnchorHome, ROAMING_ANCHOR_BYTES)?;`
- `exact_file_identity` (`:797-806`) → `file_prefix` (`:763-784`); `file_prefix`
  returns `Missing` when `row(interior, slot)` is `None` (`:768-770`), and
  `exact_file_identity` maps everything but `Exact` to `None` (`:802-805`).
  The `?` on the `Option` therefore returns `None` from `completed_record`.

So while the anchor is out of the catalog root, `completed_record` → `None` →
`ExactOwned` is not produced → `classify` falls to `Ambiguous` → hard `Err`.
**Confirmed.**

Stronger than the draft states, in two ways:

1. **The identity, not just the residency, is bound.** `:352-359` feeds the
   roaming anchor's `DurableObjectIdentityV1` into
   `InfrastructureRecordV1::owner_issue_for_catalog`
   (`protocol/infrastructure_record.rs:86-97`, field at `:28`), whose canonical
   bytes are then compared against the resident `RetiredActionsDescriptor` and
   `CatalogFormat` rows (`interior.rs:360-362`). A *different* file carrying
   the same bytes at the same name does not satisfy the predicate.
2. **Two further, independent gates the draft does not name** (see [P2-1]):
   - `completed.rs:275-285` (`retain_file`, called at `:100-104` for
     `RoamingAnchorHome`) requires a `RegularFile` row at that slot.
   - `completed.rs:363-374` (`require_named_file_identity`, called at
     `:227-233`) requires the row still present at the named slot with the
     retained identity, on **every** `revalidate`.

   Note that `revalidate`'s file-identity loop at `:204-217` does **not** gate:
   it `fstat`s the retained handle, which survives a rename.

In-window blast radius, verified: `publication.rs:225` (`PreRetirementFinal`)
and `:264` (`AdmissionCatalogInterior`) both call
`completed_record(..).is_none()` → refuse. So during a roam window **no
admission edge of any kind can publish**, not merely the barrier's own.

### 1.3 OPEN-T1 — CONFIRMED. The retired root must be empty.

`completed_record` `:349` `let retired = empty_directory_identity(interior, Slot::RetiredActions)?;`

- `empty_directory_identity` (`:808-818`) matches **only**
  `RawCatalogInteriorFactV1::EmptyDirectory`; every other fact → `None`.
- That fact is minted in exactly one place: `observe_slot` `:465-474`, inside
  `if probe_empty_directory { … if entries.next().is_none() { … } }`. A
  directory with any child falls past `:475` to `:489-493` and returns
  `RawCatalogInteriorFactV1::Other(value)`.
- `probe_empty_directory` is `slot == InfrastructureSlotV1::RetiredActions`
  (`:108-113`), so RetiredActions is the only probed slot.

One child in `retired-actions-v1` ⇒ `Other` ⇒ `None` ⇒ `Ambiguous` ⇒ permanent
`Err`. **Confirmed.** Two further independent gates, again unnamed by the draft
([P2-1]): `completed.rs:310` (`retain_directory`, called at `:105-109`) and
`completed.rs:383` (`require_named_directory_identity`, called at `:245-249`)
each require `EmptyDirectory` on their own.

**Against the frozen bounds** — the contradiction is real and the draft's
strongest evidence is correct:

- `protocol/bounds.rs:2` `MAX_RETIRED_ACTION_DIRS: usize = 64`.
- `:62-67` `RETIRED_ROOT_BUDGET_V1 = CatalogBudgetV1::new(64, 64*255, 64*256)`,
  pinned `(64, 16_320, 16_384)` by `interface_tests/bounds_slots.rs:12`. A
  scan budget for 64 entries in a root that must hold zero is incoherent.
- `CatalogOccupancyV1::validate` `:118-142`: `:126-128` `RetiredLimitExceeded`
  when `retired_action_dirs > 64`; `:132-136` the retirement-credit rule
  `retired_action_dirs + outstanding > 64 ⇒ RetirementCreditsExceeded`, where
  `outstanding` is every active action plus a preparing-without-final
  admission. The rule exists **only** to reserve retired-root room for actions
  that will later retire into it.
- `can_admit_new` `:144-148` and `can_resume` `:150-155` both arithmetic over
  `retired_action_dirs`.

Today `RetiredActions` is only ever *created* empty
(`directory_mutation.rs:108-109`, the single `StagingPlanV1::CreateRetiredActions`
arm) and never populated, so T1 is a forward defect — exactly as framed.

### 1.4 What neither derivation found wrong in the draft

`interior.rs:349`, `:350`, `:465-474`, `:808-818`, `:108-113`;
`completed.rs:64-74`; `publication.rs:225`, `:264` — every cite in the draft's
OPEN-B1/OPEN-T1 statements is accurate. The `completed.rs:183-211` cite is
misaimed (the predicate is called at `:250-265`, not in `:183-211`), but the
claim it supports is true.

---

## 2. RULINGS

### RULING B1 — the defect stands; the drafter's recommendation (i) is REFUSED; adopt (ii), in a specific shape.

**(i) — "accept the anchor's absence paired with a resident
`BarrierIntentActive(ordinal)` reconciliation record" — is not implementable.**
Three independent code facts, any one of which is fatal:

1. **`completed_record` cannot see an action-directory record.** Its input is
   `RawCatalogInteriorObservationV1` (`capability/pre_catalog/provider.rs:186-200`),
   whose `action_rows` is `Vec<ActionDigestV1>` (`:196`) — *bare digests, no
   interiors*, by the deliberate C-3 design recorded at `:190-195` ("Only the
   row's identity is retained here — the admission owner re-verifies each
   action directory's interior through its own bounded observation"). A
   `BarrierIntentActive(ordinal)` record lives inside an action directory.
   Seeing it means `interior::observe` descends into up to 64 action
   directories × up to 261 slots (`bounds.rs:70` `MAX_ACTION_SLOTS == 261`) on
   a function that runs on every catalog recovery **and inside every
   publication acquisition window** (`publication.rs:224`, `:239`).
2. **The intent record does not carry the fact needed.** `BarrierIntentV1`
   (`protocol/barrier.rs:19-32`) binds `catalog_anchor_identity` and
   `private_home_parent_identity` — there is **no roaming-anchor identity
   field**. `owner_issue_for_catalog` requires one
   (`infrastructure_record.rs:86-97`). The only other durable source is the
   `RetiredActionsDescriptor` bytes themselves, which converts
   `completed_record` from *derive-and-compare* into *decode-and-trust* — it
   would stop proving that the descriptor names the resident anchor.
3. **Any move-based scheme is fatal at key #12 regardless of (i).** See
   [P2-2]: the amendment's own `barrier.target_alias_retire` semantic destroys
   the catalog permanently.

**(ii) copy-not-move is correct, and the right concrete shape is a hard link.**

The draft's stated cost for (ii) is that it "makes keys #6/#10's names
('outbound','return') false". That cost is nominal: the key **strings** are
frozen, and their **semantics are precisely what this amendment installs**
(§1.1: "It binds semantics to keys that already exist"). E0.2 is free to define
#6 as "an anchor bearing `ROAMING_ANCHOR_BYTES` is resident in the target
parent under the reserved leaf" and #10 as "the target parent holds no anchor".

Affirmative frozen evidence for a per-barrier anchor rather than one travelling
object: `bounds.rs:5` `MAX_BARRIER_INVOCATIONS_PER_ACTION = 64`, `:10-11`
`BARRIER_ACTION_SLOTS = 3 * 64`, and `ActionSlotV1::all()`
(`protocol/slots.rs:169-175`) pushes **three** slots per ordinal —
`BarrierIntentActive`, `BarrierIntentRetired`, **`RetiredRoamingAnchorAlias`**.
One anchor-retirement destination is budgeted for *every* barrier ordinal.
That is the shape of a scheme where every barrier ends in an anchor retirement.
*(Counter-reading acknowledged: one could budget all 64 because any of them
could strand. So this is strong evidence, not proof — but it is the only
frozen evidence that speaks to the question at all, and it points at (ii).)*

**The hard-link shape, which E2.1 should take:** create the target-parent alias
as a hard link off the resident `roaming-anchor-home-v1` object rather than a
byte copy. Then:

- the home row **never leaves** — `completed_record` `:350`,
  `completed.rs:275-285` and `completed.rs:363-374` are all untouched, and
  **OPEN-B1's precondition package collapses to zero catalog-owner changes**;
- the alias shares the home object's durable identity (Linux
  `name_to_handle_at` resolves the inode, `platform/linux.rs:181`; Windows file
  IDs are shared across hard links), so #7/#11's "proved to be the identity the
  intent bound" stays a real check;
- the Windows P5 round trip still works: the barrier needs a rename of a
  *resident* file within the target parent, and a hard link is resident;
- key #12 becomes non-fatal — retiring a stranded alias onto
  `RetiredRoamingAnchorAlias(ordinal)` leaves the home row intact;
- the legacy protocol's hard-link hazard does **not** transfer: `anchor.rs`
  refuses a re-homing link because its names are *identity-derived*
  (`anchor.rs:325`, `:408-410`, and the test
  `a_foreign_link_that_re_homes_the_anchor_is_refused_without_mutation`,
  `anchor/tests.rs:515-538`). The catalog's roaming anchor is addressed by the
  **fixed constant** `roaming-anchor-home-v1` (`protocol/slots.rs:79`), so
  re-homing is not expressible.

E2.1 must state the fallback where hard links are unavailable (fresh copy), and
must state which it chose, **because that choice decides whether key #11 is a
real boundary or a tautology** ([P2-6]).

### RULING T1 — the defect stands; the C-3-style widening is CORRECT and should be authorized, with the package corrected.

Authorize it. It is squarely the shape freeze `:1443-1450` sanctions (verified
verbatim): extend the *provider's reading* of the closed grammar, mint nothing.
The children are already in-vocabulary (`RootEntryNameV1::ActiveAction`,
DECISION T-A, which I verify as correct — `RootEntryNameV1::parse`
`slots.rs:434-445` classifies `action-<hex>-v1` `Valid` at `:444`, parent-independently), and
the enumeration budget already exists unused.

Two corrections to the package before E3.1 implements it:

- it is **three** gates, not one ([P2-1]);
- the `census.retired` supporting sentence must be struck ([P2-5]) — it is a
  misreading, and the bounds evidence is sufficient without it.

### RULING on "one precondition package" — REFUSED. Rule them separately.

The draft's §4.4 note asks the dual to rule B1 and T1 as one package "the same
defect class — a completion predicate written when the catalog had no
lifecycle, meeting the lifecycle."

They are **not** the same class:

- **T1 is an observer-reading defect.** `completed_record` reads the retired
  root too narrowly. The remedy is a strictly-widening read plus two `EmptyDirectory`
  gates. Nothing else in the predicate changes; the identity it derives
  (`empty_directory_identity` → the retired root's *own* durable identity) is
  unaffected by a child addition, so descriptor/format bytes stay byte-identical
  (the draft's evidence (a) is correct here).
- **B1 is not an observer defect at all.** `completed_record`'s roaming arm is
  a *derivation*: the anchor's identity is an **input** to the record whose
  bytes are compared against two resident files. Under the correct mechanism
  (ii) `completed_record` is **untouched**. There is nothing to authorize.

Bundling them would hand E2.1 a standing authorization to widen
`completed_record` — the predicate every catalog consumer depends on — for a
defect whose correct fix does not touch it. That is precisely the risk the
drafter cites as the reason not to take the decision unilaterally.

**Ruling: authorize the T1 widening (three gates, corrected evidence) as its
own precondition step owned by E3.1. Do not authorize any `completed_record`
edit for B1; require E2.1 to land mechanism (ii) with `completed_record`
unchanged, and record that as B1's resolution.**

---

## 3. [P1] findings

### [P1-1] OPEN-B1's recommended candidate (i) is not implementable; it must not be blessed.

Grounds in RULING B1 above. `capability/pre_catalog/provider.rs:196`
(`action_rows: Vec<ActionDigestV1>`), `protocol/barrier.rs:19-32` (no roaming
identity field), `protocol/infrastructure_record.rs:86-97` (the identity is a
required argument). The draft's "**Drafter's recommendation: (i)**"
(`:626-629`) and the §3.5 candidate text (`:602-609`) must be replaced.

### [P1-2] DECISION T-B is not implementable as a field generalization, and §8 would freeze that determination.

The draft resolves freeze §4.4's last open arm-table row (`:1382`) to "**no new
variant — one field generalization of `AdmissionCatalogInterior`'s `absent`
field, owned by R2-E E3.1**" (§4.2 DECISION T-B; §8 row 6), on the ground that
only the `absent` field's *parent* must move.

The code refutes it. `publication.rs:234-275`:

```
DestinationRecheckV1::AdmissionCatalogInterior { durable_identity, expected, absent } => {
    let fresh = interior::observe(destination_dir, &HostPlatform)?;   // :239 — ONE observation
    ... match absent { … }                                           // :240-248
    ... interior::completed_record(durable_identity, &fresh, expected).is_none()  // :264
```

A **single** observation of `destination_dir` serves both the `absent` check
*and* the completed-catalog proof. Both existing producers pass the catalog
root as `destination_dir` (`admission_mutation.rs:177-189` and `:276-294`, both
passing `final_directory` for source and destination). A terminal retirement's
`destination_dir` is the **retired root**; `interior::observe` of the retired
root can never yield a `completed_record`-satisfying observation — it holds no
`CatalogFormat`, `CatalogAnchorA`, `RoamingAnchorHome`, `RetiredActions` or
`RetiredActionsDescriptor` row. Moving `absent`'s parent does not help, because
the *observation input* is what must differ.

Nor can the destination be expressed as a two-component leaf: `rename_open_source`
(`platform.rs:86-93`, `:106-111`) takes `destination: &OsStr` and the whole
owner is single-component no-follow throughout.

So the terminal arm needs a **second directory handle** (catalog root alongside
retired root) or the recheck must move out of `publish_verified_no_replace`.
Either is a new observation input, which by §4.4 Class 1's own struct-vs-enum
criterion (freeze `:1356-1364`) is **variant-shaped, not field-shaped** — i.e.
the draft's own rejected alternative is the right one, or something larger.

Consequence: §4.2's DECISION T-B, §4.3 row #7's anchor, and §8's row-6
annotation all need rework before freezing. Freezing "no new variant" resolves
a frozen open row the wrong way.

### [P1-3] OPEN-C1 is REFUTED at the source; DECISION C-3's premise is wrong and the OPEN row must be struck.

The drafter flagged this for attack (Appendix C item 4) and was right to.

`BaseActionSlotV1::RecordScratch` is **not** the authority record's scratch.
The `record.scratch_*` boundaries fire inside `write_authority_scratch`
(`capability/pre_catalog/provider/authority_record_binding.rs:501-531`, hits at
`:508`, `:516`, `:529`), which is called from `install_authority_record`
(`:473-495`) with:

```
authority_record_binding.rs:486
    let scratch = slot_name(action, BaseActionSlotV1::AuthorityScratch)?;
```

— **`AuthorityScratch`**, not `RecordScratch`. The keys are named after the
`record.*` fault **family** (the durable-record lifecycle: `record.bounded_read`,
`record.decode`, `record.active_publish`, `record.retire_exact`, …
`fault_v1.rs:97-107`), not after the slot.

A tree-wide grep for `BaseActionSlotV1::RecordScratch` returns exactly two
non-definition hits — `namespace/roles.rs:20` and `:34` (the `PublishRoleV1`
routing) — and **zero write paths**. `PublishRoleV1` carries `AuthorityScratch`
(`roles.rs:18`) and `RecordScratch` (`:20`) as *distinct* roles, which is
itself the disproof of the inference.

Consequences:

- **DECISION C-3 is safe and gets easier**, exactly as the drafter predicted:
  `RecordScratch` is an entirely unconsumed base slot, so the cleanup worklist
  is its first and only user. No sharing, no ordering.
- **OPEN-C1 must be struck.** As written it is an "*Ordering condition, binding
  on E1.1*" that E1.1 "must establish — and E1.2's matrix must drive", and
  whose failure "reopens" C-3. It binds a phase to prove an interleaving that
  cannot occur, and its stated settling evidence ("no consumer of either exists
  today, so the question cannot be answered by reading the tree") is exactly
  backwards — the tree answers it in one grep.
- §2.3 row #1's parenthetical "(DECISION C-3, with OPEN-C1's ordering
  condition)" and Appendix A's OPEN-C1 row go with it.

---

## 4. [P2] findings

### [P2-1] Both blocking precondition packages are three gates, not one.

The draft names only `completed_record` as the thing to widen. Each defect has
**two further independent gates inside `completed.rs`** that a `completed_record`
widening alone leaves standing:

| | gate | called from | requires |
| --- | --- | --- | --- |
| T1 | `completed.rs:305-331` `retain_directory` | `:105-109` | `EmptyDirectory` at `RetiredActions` |
| T1 | `completed.rs:378-397` `require_named_directory_identity` | `:245-249` (every `revalidate`) | `EmptyDirectory` at `RetiredActions` |
| B1 | `completed.rs:270-303` `retain_file` | `:100-104` | `RegularFile` + exact bytes at `RoamingAnchorHome` |
| B1 | `completed.rs:358-376` `require_named_file_identity` | `:227-233` (every `revalidate`) | row present at the named slot, retained identity |

E3.1 implementing only `interior.rs:349` would still fail at
`retain_completed_catalog` and at every `revalidate`. Both OPEN rows and both
§8 annotations must state the full package. (Under RULING B1 the B1 rows become
moot — that is the point.)

Also worth noting for T1: `staging_plan` (`interior.rs:216-237`) matches
`EmptyDirectory` for `RetiredActions` too. It should keep refusing a populated
retired root during bootstrap; if E3.1 reshapes the fact type, that arm must be
preserved deliberately.

### [P2-2] A move-based roam is fatal beyond the crash window: the amendment's own key #12 destroys the catalog permanently.

§3.4 row #12 defines `barrier.target_alias_retire` as "a **stranded** anchor
alias at the target … has been retired onto `RetiredRoamingAnchorAlias(ordinal)`".

Under any move-based scheme that is unrecoverable. Once the anchor object is
retired into an action directory, restoring the home row requires creating a
**new** file — a new inode, hence a new `DurableObjectIdentityV1`. The
`RetiredActionsDescriptor` and `CatalogFormat` rows bind the *old* identity
(`interior.rs:352-362` via `infrastructure_record.rs:86-97`), so
`completed_record` can never match again. There is no descriptor-rewrite path
in a completed catalog.

So the draft's own reconciliation key is catalog-fatal, and OPEN-B1's framing
("a crash inside the roam window") is too narrow: the *designed* stranded-alias
path is equally fatal. This is decisive for RULING B1.

### [P2-3] The O6 resolution applies parts (i) and (iii) of the Step-4.3 precedent but not part (ii); the restatement class survives restart.

The draft's §3.1 correctly enumerates the precedent's three parts, including
"(ii) the owner **refuses** on disagreement". §3.2's mechanism implements only
the mint side.

`read_and_bind_barrier_intent` (`protocol/barrier.rs:238-256`) re-checks five
facts on read — `action_digest`, `request_owner_binding`, `reservation_digest`,
`schedule_digest`, `ordinal` (`:244-249`) — and **none** of
`catalog_anchor_identity`, `private_home_parent_identity`, `private_home_name`.
`decode_canonical` (`:120-159`) rebuilds via `from_bound_fields`, bypassing
`issue` entirely. So a drive resuming after a crash reads caller-asserted
identities off disk and acts on them, however tight `issue` becomes.

I verified the precedent does put the refusal on the consumer: the Step-4.3
record's own text (quoted at draft `:428-429`) is
"`CheckedAuthorityObservationOwnerV1::observe` refuses when it disagrees with
the resident reservation", and `AuthorityFactsIssuerV1::issue`'s own doc
(`protocol/authority_record/owner/host.rs:53-73`) is explicit that the seam's
protection "is the provenance obligation documented above, **not** the argument
shape". An issuer alone buys nothing.

**Required:** the O6 resolution must add the read-side refusal — the owner
compares the resident intent's three facts against the witness it can mint, and
refuses on disagreement — and key #5 (`barrier.intent_reobserve`) must name it,
since §3.4 currently documents the five-check read as sufficient.

One related caution on the mechanism's wording: §3.2 says the witness lands
"in the shape `AuthorityFactsIssuerV1::issue` uses". `AuthorityFactsIssuerV1` is
handed **to the caller**, which then supplies the facts (`host.rs:104-119`). If
E2.2 copies that shape literally, the caller still supplies the identities and
nothing is gained. The correct shape is the *inverse* and already exists in the
same owner: `RetainedCompletedCatalogV1::observe_admission` (`completed.rs:142-147`)
and `retain_action_namespace` (`:169-181`), whose in-code contract is "The
caller receives the typed observation only — never a handle". Say that
explicitly, or E2.2 will build the weaker thing.

### [P2-4] §2.4's E1.2 duty is not executable, and the tree already says so.

§2.4 binds E1.2: "the matrix drives through a real admitted action —
`ScheduledCheckedActionV1::admit` (`coordinator/execution.rs:137-152`) →
`ActionAdmissionOwnerV1::resume_or_admit` — **not** through a hand-built
reservation… a matrix that skipped admission would be proving a different
thing."

`namespace/tests_fault_matrix.rs:20-35` records why that is impossible for the
restart half:

> once a namespace edge has published its first row the action directory is no
> longer *exact* (`protocol/admission/owner.rs:29-38`), which is precisely the
> state a second admission must refuse. Resuming that handoff from durable
> state is **not owned by any landed step**… the only route to reconstruct an
> `AdmittedActionV1` in that state is `protocol/admission/test_support.rs`

Verified at the source: `has_exact` (`protocol/admission/owner.rs:29-38`)
requires `extra_children: 0`. Every cleanup edge publishes a row into the action
directory, so from the first durable edge onward the restart cannot re-admit.

E1.2 can honour the duty for the *first* attempt only; every restart must
rebuild the handoff through the test-only issuer, inheriting
`namespace/tests_fault_matrix.rs`'s documented deviation. The amendment should
say so, or E1.2 burns a cycle rediscovering it. (The `admission/tests_fault_matrix.rs`
precedent §2.5 calibrates on does not transfer: there the action directory is
the *subject* of admission, not a post-admission mutation target.)

### [P2-5] OPEN-T1's `census.retired` evidence is a misreading.

§4.4: "the catalog-root census already carries a `retired` counter
(`protocol/admission.rs:438`, charged at `:449`)" offered as evidence that
"the bounds expect a populated retired root".

`CatalogRootRowCensusV1::retired` (`protocol/admission.rs:438`, charged `:449`)
counts `CatalogRootRowClassV1::Retired(slot)` rows, and that class covers
exactly two names (`protocol/slots.rs:475-478`):
`InfrastructureSlotV1::RetiredActions` and
`InfrastructureSlotV1::CatalogBootstrapRetired`. The census is charged over the
**catalog root's** children (`interior.rs:82-83`); the retired root's children
are never enumerated by it. In a completed catalog `census.retired == 2`, a
constant.

Strike the sentence. The bounds/occupancy evidence carries OPEN-T1 on its own.

*Related, and in the draft's favour:* §4.1 quotes `admission/driver.rs:104-110`
but truncates before its last clause — "unlike the active bound, exhausting
retirement credit cannot make the catalog unobservable." That clause is the
R2-D author's belief that the retired root is observationally benign, i.e.
exactly the belief OPEN-T1 refutes. Quoting it in full **strengthens** the row.

### [P2-6] The O6 REFUSE arm's "exactly one reachable state" is wrong, and its reachability is coupled to the B1 ruling.

§3.2: "Exactly one reachable state produces that: the roaming anchor is **not
at home**."

At least two further states block the witness mint, both already typed refusals
in the callee's own owner:

- the catalog anchor's identity changed — `completed.rs:211` "retained catalog
  file identity changed" (and `:363-374` at the named slot);
- the final directory is no longer the named catalog — `completed.rs:195-203`.

And under RULING B1's mechanism (ii) the anchor never leaves home, so
anchor-stranded is reachable in **zero** states and the promised typed REFUSE
has no trigger at all. §3.2's REFUSE arm must be restated once B1 is decided.

### [P2-7] DECISION T-C splits the terminal family across a capability boundary it cannot cross.

`admission_mutation.rs::execute` (`:79-84`) takes only
`final_directory: &Dir` — the catalog root. It holds neither the action
directory nor the retired root.

But of §4.3's ten activated keys:

- #1-#4 (`terminal.authority_reobserve`, `payload_reobserve`,
  `cleanup_reobserve`, `reservation_reobserve`) and #5
  (`terminal.directory_flush`) are reads/flushes of the **action directory** —
  the capability `namespace_mutation.rs`'s `RetainedActionNamespaceV1` owns
  (`:122-129`, retained `:135-171`);
- #6-#8 need the **retired root** handle, which is private to
  `RetainedCompletedCatalogV1.retired_actions` (`completed.rs:46`, retained
  `:105-109`) and never forwarded anywhere.

T-C's stated ground for rejecting `namespace_mutation.rs` — "that file holds no
catalog-root capability" — applies symmetrically against
`admission_mutation.rs`. E3.1 needs two new capability forwards either way;
the decision should say which owner mints them, and #1-#5 arguably belong in
`namespace_mutation.rs` regardless.

### [P2-8] The declared-source file count is wrong in both places the amendment states it, and both are RemPlan §10 duties.

Verified today: **nine** declared sources (nine `include_str!` entries in
`FAULT_INJECTION_SOURCES`, `interface_tests/fault_expected_keys.rs:419`+),
matching freeze `:525-532` and the green completeness anchor.

- **§2.5** owes E1.2 "the §3.5 inventory addendum's file count (freeze
  `:525-532`, 'nine' → ten)". Wrong: E1.2 adds cleanup sites to
  `namespace_mutation.rs`, which the draft itself notes at §2.3 is *already*
  declared (`fault_expected_keys.rs:449-452`). E1.2 changes the count to
  **nine**.
- **§8** (proposed frozen text) says the list "moves from **nine** files to
  **eleven** as E1/E2/E3 land". Wrong on its own sentence: only
  `barrier_mutation.rs` is new; `admission_mutation.rs` is already declared
  (`:432-435`). Nine → **ten**.

Both must be corrected; §8's is the more serious, since it enters the freeze.

---

## 5. MANDATE 2 — the injection-site rows

**Convention finding, first:** §1.4 defines a two-valued marker (`EXISTS` /
`NEW@<step>`), but **no row in §2.3, §3.4 or §4.3 carries `EXISTS`.** All 37
activated rows are `NEW@E1.1` / `NEW@E2.1` / `NEW@E3.1`. So the mandate's
"verify each EXISTS marker" has an empty domain. That is honest — it is the
correct consequence of "the freeze re-reserved these 38 keys precisely because
their edges do not exist yet" — but §1.4 should say so, since a reader will
otherwise hunt for a distinction the tables never draw. [P3]

What I checked instead, row by row:

| Check | Result |
| --- | --- |
| Every named **site file** exists | ✔ `namespace_mutation.rs` (414 lines), `admission_mutation.rs` (475), `barrier_mutation.rs` correctly marked `NEW@E2.1` |
| Named files are declared `FAULT_INJECTION_SOURCES` entries at the cited lines | ✔ `:449-452` (namespace_mutation), `:432-435` (admission_mutation) — both EXACT |
| Completeness anchor forces the new file's declaration | ✔ `fault_expected_keys.rs:414-418`; test `the_declared_injection_sources_are_every_production_source_holding_sites` passes |
| Driver-holds-zero consistency | ✔ all 37 sites land in owner-private mutation files under `capability/pre_catalog/provider/`; the deciders (`ActionNamespace`/`namespace/host.rs`, `admission/driver.rs`) keep zero, matching `fault_expected_keys.rs:407-409` and `:444-448` |
| Every **precedent** cite resolves | ✔ all 21 `managed_mutation.rs` / `namespace_mutation.rs` / `admission_mutation.rs` precedent cites verified exact (§8 below) |
| Owner capability actually reachable at each site | ✖ for §4.3 keys #1-#8 — see [P2-7] |

Site-placement judgment: C-2 (cleanup → `namespace_mutation.rs`) and B-1
(barrier → new `barrier_mutation.rs`) are both correct, and C-2's cohesion
arithmetic is accurate (`namespace_mutation.rs` = 414, `managed_mutation.rs` =
1,251 — both EXACT). T-C is not ([P2-7]).

---

## 6. MANDATE 3 — the O6 resolution's observation claims

| Claim | Verdict | Evidence |
| --- | --- | --- |
| anchor identity is held & revalidated by the callee's own owner | **TRUE** | held `completed.rs:44`, retained `:95-99`; revalidated at `:204-217` (handle fstat) **and** `:227-244` via `require_named_file_identity` for `CatalogAnchorA` (`:228-229`); bytes pinned `interior.rs:29`, refused by `completed_record` `:351` |
| home identity is held & revalidated by the same owner | **TRUE** | `completed.rs:42`, minted `:85`, revalidated `:190-203` — reopened by name **and** re-fstat'd on the retained handle, both compared to the stored identity. The home *is* the Final directory (the anchor lives at `<final>/roaming-anchor-home-v1`) ✔ |
| `private_home_name` is the compile-time constant | **TRUE** | `protocol/slots.rs:51` (`RoamingAnchorHome`), `:79` (`"roaming-anchor-home-v1"`) — both EXACT. `DERIVE` is the right disposition |
| the owner-minted witness matches the Step-4.3 `AuthorityFactsIssuerV1::issue` precedent's shape | **MISLEADING** | see [P2-3]: the precedent hands the issuer *to the caller*; copying it literally preserves the defect. Use the `observe_admission` / `retain_action_namespace` shape instead |
| anchor-stranded is the one state where observation cannot be made | **FALSE** | see [P2-6] |

The three dispositions themselves (OBSERVE / OBSERVE / DERIVE) are **correct**
and well grounded. The mechanism's framing is what needs work.

---

## 7. MANDATE 4 — C-1, B-3, C-1's silence claim, OPEN-C1

### DECISION C-1's silence claim — **VERIFIED, correct.**

`ActionNamespaceEdgeV1` has exactly two variants (`namespace_mutation.rs:55-60`);
`faults()` (`:78-93`) returns the four `namespace.*` keys per variant;
`execute_edge` (`:250-305`) hits them at `:265`, `:278`, `:290`, `:302`.
A `Cleanup` variant would replace those four hits for cleanup renames — i.e.
silence `namespace.retirement_reserve` / `pre_retire_reobserve` / `retire_exact`
/ `retired_reobserve` for that class. The `namespace` family is `Executed`
(`fault_expected_keys.rs:335-339`, EXACT) with all eleven keys in
`NAMESPACE_MATRIX` (`namespace/tests_fault_matrix.rs:83-95`). The quoted code
precedent at `managed_mutation.rs:1018-1027` is **verbatim exact**. C-1 stands.

*Adjacent observation, not a defect in C-1:* `namespace/tests_fault_matrix.rs`
is silent about barrier-intent *records* — its `drive` (`:296-327`) writes the
"barrier intent" scratch with a raw `fs::write` of the **reservation's** bytes
(`write_scratch`, `:335-343`), never a real `BarrierIntentV1`. So there is today
**zero executed coverage of the intent record's binding**, which E2.3's matrix
is the first thing to provide.

### DECISION B-3's Windows arm — **the concern is real; the draft's framing is right but understated.**

`platform/anchor.rs`'s grammar cannot take a supplied anchor without widening:

- `verify` (`:394-406`) hard-requires bytes `== ANCHOR_BYTES`
  (`b"GWZ-CHECKED-ARTIFACT-DURABILITY-ANCHOR-V1\n"`, `:74`). The catalog's
  roaming anchor carries `ROAMING_ANCHOR_BYTES`
  (`b"GWZ-ROAMING-ANCHOR-V1\n"`, `interior.rs:28`) — different.
- `survey` (`:284-361`) accepts a name only if it equals
  `anchor_name(&identity.name_digest())` or its `.roundtrip` twin
  (`:323-339`); the name is **derived from the object's own identity**
  (`:408-410`). A supplied, caller-named anchor can never be `Ready`.
- `round_trip` (`:183-204`) calls `prepare(dir, false, …)`, which for a
  directory with no `.ca1-` anchor returns `Ok(())` (`:162-165`), then
  demands `Ready` and errors "private durability anchor is not ready"
  (`:185-191`).

So B-3's conclusion — a **third** `DirentBarrierClass` whose Windows arm
round-trips a supplied anchor rather than surveying for a resident one — is
correct, and its rejected alternative is correctly rejected (`AnchoredPrivateArea`
would `establish` a permanent `.ca1-durability-anchor-*` in the target parent,
`:129`/`:216`/`:284`).

Does it widen what `legacy_nonce_orphans_are_tolerated_and_block_nothing`
(`platform/anchor/tests.rs:492-508`) pins? **Not that test directly** — it
pins that a `.ca1-anchor-scratch-<32hex>` orphan falls through all four survey
arms and is ignored (it starts `.ca1-`, not `ca1-`, so it does not even set
`family_state`, `:307-309`). A supplied-anchor path that does **not** route
through `survey` leaves that test byte-identical. What it *does* touch is
`assert_closed_grammar` (`tests.rs:115-125`), which asserts every name in a
protocol-created tree is one of four shapes — so the new class must keep the
supplied anchor out of any directory that helper covers.

**Two hazards the draft does not name, which E2.1 must handle:**

1. **The two closed grammars collide.** If a supplied anchor were ever placed
   in a catalog-owned directory, `interior::exact_row` (`:394-418`) classifies
   `.ca1-*` as `Foreign` (`protocol/slots.rs:434-435`) and refuses the whole
   interior with "catalog directory contains an unowned child". The
   `reserved_target_leaf` grammar (OPEN-B3) must therefore be *catalog-legal*
   wherever the target is catalog-owned — which is a stronger constraint than
   OPEN-B3 states.
2. Under RULING B1's hard-link shape, the alias in the target parent is a
   *second name for the home object*. B-3's Windows arm must round-trip **that
   name**, and E2.1 must confirm the Windows rename-within-directory of a hard
   link delivers the same dirent ordering the resident-anchor arm relies on.

### OPEN-C1 — **REFUTED.** See [P1-3].

---

## 8. MANDATE 5 — the §5 blast-radius decision's code grounds

Every claim verified; the decision is **correct and correctly grounded**.

| Claim | Verdict | Evidence |
| --- | --- | --- |
| `witness.rs:31-41`'s `try_new` revalidates without probing | **TRUE** | `try_new` `:31-35` calls `revalidate()`; `revalidate` `:37-44` dispatches to `runtime.revalidate_catalog_target()` / `held.revalidate_held()` |
| `facts()` at `:46-95` is where probes fire | **TRUE** | `:53-54` `platform.dir_identity(…)`, `:57` `support_profile()`, `:60`/`:66-67` `rename_domain(…)`, `:61`/`:68` `parent_mode(…)`. `dir_identity` reaches `libc::name_to_handle_at` (`platform/linux.rs:181`) |
| the runtime lease uses dev/ino identity (`paths.rs:69-100`) | **TRUE** | `InvocationIdentity { device, inode }` `paths.rs:10-13`; `identity(metadata)` `:315-319` = `metadata.dev()` / `metadata.ino()`; `retain_ambient_directory` `:69-83`, `revalidate_ambient_directory` `:85-105` — cited as `:69-83`/`:85-100`, both EXACT |
| **NINE** production call sites | **TRUE — counted independently** | `WorkspaceMutatorLock::acquire` has exactly ten call sites tree-wide; the tenth (`workspace_ops/tests/g02.rs:281`) is a test. The nine production sites are exactly the nine the draft lists, in the order it lists them, incl. `workspace_ops/handle_create_repo.rs:35` and `workspace_ops/handle_init_from_sources.rs:91` — both verified as `let _guard = WorkspaceMutatorLock::acquire(&root)?;` |
| `try_acquire` does not reach `facts()` today | **TRUE (stronger than stated)** | `operation/workspace_mutator_lock.rs:35-38` → `try_acquire_workspace_runtime`; `catalog_mutation_lease()` (`:48-52`) carries `#[allow(dead_code)]` — no production caller at all |
| advisory-lock refusal is the shape to copy | **TRUE** | `bootstrap/runtime/mod.rs:237-243` `CheckedFsError::unsupported(PlatformCapability::RuntimeAdvisoryLock, …)` — EXACT |

One note on §5.2's Ground 2 arithmetic: `workspace_ops/merge/v1_lifecycle/archive.rs`
supplies **two** of the nine (`:97` and `:126`), so it is nine sites across
eight files. The draft's list is correct; a reader counting files will get
eight. Worth one clause. [P3]

---

## 9. MANDATE 6 — internal-consistency sweep of the 38 rows

| Check | Result |
| --- | --- |
| No key mints census; 165 intact | ✔ `EXPECTED_KEY_COUNT = 165` at `fault_expected_keys.rs:174` (EXACT); key list ends `:172` (EXACT); cleanup `:151-161`, terminal `:162-172` (the draft's `:151-172` EXACT); barrier `fault_v1.rs:132-147`, cleanup `:178-188`, terminal `:189-199` — **all three EXACT** |
| Fixture green on the tree | ✔ ran `cargo test -p gwz-core --lib checked_artifact::interface_tests::fault_expected_keys` (scratch target dir): **6 passed, 0 failed**, incl. `reserved_fault_families_have_no_injection_sites_before_their_package` and `the_declared_injection_sources_are_every_production_source_holding_sites` |
| Three families still `Reserved` | ✔ `fault_expected_keys.rs:380-384` / `:385-389` / `:390-394` — **all three EXACT**, and the cleanup row's reason string quoted verbatim |
| `PartiallyExecuted` exists for the terminal 10/1 split | ✔ `fault_expected_keys.rs:209` (EXACT), with the per-key siteless proof it describes at `:205-208` |
| Key names match the vocabulary one-for-one | ✔ all 38 row labels in §2.3/§3.4/§4.3 match `fault_v1.rs:132-199` exactly, in order |
| No row contradicts a frozen determination | ✖ **T-B contradicts §4.4 Class 1's own criterion** once the code is read ([P1-2]). T-A does **not** contradict the E4-retire record — I checked the disputed clause's scope and the draft's reading is right: freeze `:1256-1276` is about `remove_file` of the `ActionAdmissionActive` *record slot*, and no in-vocabulary retired-root target exists for a file even after T1's widening (which admits only `ActiveAction` rows). T-A stands. |
| Every convergence obligation executable | ✖ §2.4/§2.5's "real admitted action" duty ([P2-4]); §2.5 and §8's file counts ([P2-8]); §3.6's "executed natively, not skipped" Windows duty names no carrier — the plan puts three-platform acceptance at E7.2 and §7.3 puts the native ledger entry at "nominally E1.2". [P3] |
| E-step ids referenced all exist | ✔ E1.1, E1.2, E2.1, E2.2, E2.3, E3.1, E3.2, E4.1, E4.4, E4.7, E5.1, E5.2, E6.2, E6.3, E7.2 all present in `GwzM5-8R2E-Plan.md:98-181` |
| O6 ordering consistent with the plan | ✔ mechanism = E2.2 (plan `:111`); constraint is "E2.2 before any **E4** row", so E2.1-before-E2.2 is permitted. Minor churn: §3.4 rows #1-#3 build the intent-scratch writer at E2.1 against a signature E2.2 changes. [P3] |
| T-D (`terminal.authority_release` reserved) | ✔ code-side sound. `RetainedWriteAuthorityV1` is neither `Copy` nor `Clone` (`coordinator/execution.rs:321-327`, EXACT); the return-to-idle boundary genuinely already carries five executed keys. (Its *doctrinal* half is the State axis's.) |

**Census verdict: intact.** No row in the amendment mints, retires or re-keys
anything. §9 items 1-2 are true.

---

## 10. [P3] findings — citation drift

The draft claims "**Every `file:line` below was opened and read**" (`:13`). I
sampled ~110 cites across code and dev-docs. The hit rate is exceptional —
every `managed_mutation.rs`, `fault_expected_keys.rs`, `protocol/cleanup.rs`,
`platform.rs`, `authority.rs`, `namespace/host.rs`, `coordinator/execution.rs`
and `archive.rs` cite I checked was exact, as were the long verbatim quotes
from freeze `:712-721`, `:726-733`, `:768-781`, `:792-799`, `:1443-1450` and
`managed_mutation.rs:1018-1027`. The drift below is all off-by-one on span
starts, plus three substantive-adjacent slips.

| # | Cite | Actual | Note |
| --- | --- | --- | --- |
| a | §3.2 "anchor identity … `namespace/mod.rs:209`" | `:208` | `:209` is `private_home_parent_identity` |
| b | §3.2 "forwards three … `:208-210` → `:220-222`" | `→ :221-223` | `:220` is `slots.ordinal` |
| c | §1.2(b) barrier re-reservation `:786-790` | `:787-790` | `:786` is the tail of the previous sentence |
| d | §4.2 E4-retire clause `:1272-1276` | `:1273-1276` | quote itself verbatim ✔ |
| e | §5.3 / §3.1 `E01ReachTraces.md:52-56` and `:71-77` | `:53-57` and `:72-77` | both quotes verbatim ✔ |
| f | OPEN-B1/T1 "revalidation (`completed.rs:183-211`)" | predicate is at `:250-265` | `:183-211` is the head of `revalidate`; obscures the separate gates at `:227-249` ([P2-1]) |
| g | §4.1 `RETIRED_ROOT_BUDGET_V1` `:62-68` vs §4.4 `:60-66` | `:62-67` | the two sections disagree with each other |
| h | §4.4 `CatalogOccupancyV1` `validate` `:120-142` | `:118-142` | |
| i | §4.3 T-D "hits at `:191`, `:199`, `:296`, `:307`, `:410-431`" | `:191`/`:199`/`:410`/`:418`/`:431` are the five idle keys; **`:296`/`:307` are `AdmissionFinalPublish`/`FinalReobserve`** — a different edge | pollutes a determination's evidence |
| j | §7 table header "§11.3 row" | rows 1-3 and 5 are §11.1 (`SettledTuple.md:636-775`); only the A1-gate quotes are §11.3 (`:798+`) | inherited from E0.1(b); §3.1's own "§11.1 (`:653-658`)" is **correct** |
| k | §1.4 defines `EXISTS` but no row uses it | — | see §5 above |
| l | §5.2 "nine production call sites" | nine sites, **eight files** | `archive.rs` supplies two |

**Verified correct and worth keeping:** §6.3's citation-drift note. R4b-G cites
the contract passages at `:159-165` and `:117-125`; they are at `:178-184` and
`:136-144` in `GwzM5-8I2CompatibilityContract.md` today — a uniform **+19**.
Confirmed exactly.

---

## 11. What should freeze as written

- **§5** (E4.1 blast radius, option (ii)) — every ground verified; freeze it,
  fixing [P3-l].
- **§6** (O8 two-tier mechanism, the two shapes' PENDING-FIXTURE disposition,
  the `gc_archived` route) — code-side claims verified (`archive.rs:32-46`,
  `:41`, `:50`, `:108-111`, `:112-118`); the drift note is right.
- **§7** — A-1's compensating-observation measurement verified at
  `authority.rs:226-228` / `:230-232` / `:234-236` / `:238`, and its checked-path
  ground at `namespace_mutation.rs:280-288` → `publication.rs:148-179`. Sound.
- **DECISION C-1, C-2, B-1, B-2, B-3, T-A, T-D** — all sound.
- **DECISION C-3** — sound, and *stronger* than drafted once OPEN-C1 is struck.
- **The 38 key-semantic rows** — individually defensible; #6/#10/#11 need
  re-wording once B1 is ruled, and #5 needs [P2-3]'s read-side refusal.

## 12. Required before re-freeze

1. Replace OPEN-B1's recommendation: refuse (i), adopt (ii) in the hard-link
   shape, record B1's resolution as "`completed_record` unchanged". [P1-1]
2. Rework DECISION T-B and §8's row-6 annotation; the terminal arm needs a
   second observation input. [P1-2]
3. Strike OPEN-C1 and its `RecordScratch` premise from DECISION C-3 and
   Appendix A. [P1-3]
4. State the full three-gate package for T1 (and drop B1's, per 1). [P2-1]
5. Record that a move-based roam is fatal at key #12. [P2-2]
6. Add the read-side refusal to the O6 resolution and to key #5. [P2-3]
7. Correct §2.4/§2.5's admitted-action duty to the documented deviation. [P2-4]
8. Strike the `census.retired` sentence; quote `driver.rs:104-110` in full. [P2-5]
9. Restate the REFUSE arm's reachable states. [P2-6]
10. Re-place §4.3 keys #1-#5 and name the two capability forwards #6-#8 need. [P2-7]
11. Fix both file counts: E1.2 → nine; the E1/E2/E3 arc → nine to ten. [P2-8]
12. Sweep the [P3] cite drift, especially (f) and (i).

---

**VERDICT: NO-GO as drafted (revise and re-freeze). OPEN-B1 CONFIRMED —
recommendation (i) REFUSED, adopt copy-not-move (hard link), `completed_record`
untouched. OPEN-T1 CONFIRMED — the C-3-style widening AUTHORIZED, corrected to
three gates. RULE THEM SEPARATELY: they are not one defect class and not one
precondition package.**

*(Round 1 closed. The hard-link sub-shape above is superseded — see Round 2 §R1.)*

---

# Round 2 — re-verdict on the pair (draft + addendum), 2026-08-27

Object: `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` **+**
`GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` (1,327 lines), the addendum
controlling where they differ. Peer-blind holds — no State-axis material read;
where the addendum reports the State axis's rulings I treat them as the
addendum's own claims and judge them on code.

## VERDICT (round 2)

**GO — conditional on one [P2] fold-in.**

All three round-1 [P1]s are cured at the source, not papered over. All eight
[P2]s are dispositioned acceptably, two of them better than I asked. The three
new open rows are well-formed. The addendum's override of my own ruling is
**correct and I ratify it** (§R1).

One new **[P2]** blocks nothing but will red every converging commit: a
third machine-enforced inventory — the sealed-publication caller registry and
the protected source-tree digests — is absent from all three convergence
obligations (§R6). Fold it into §2.5 / §3.6 / §4.5 and the pair may freeze.

Three new **[P3]**s in §R7. No [P0]. No [P1].

---

## R1. THE OVERRIDE — re-derived independently, and RATIFIED

The addendum §1.2 refuses my round-1 hard-link sub-shape as macOS-fatal. I
re-derived it from the tree before reading the addendum's argument, as in round
1. **The finding holds. My sub-shape is refuted. I ratify DECISION B-5.**

### R1.1 The derivation

The macOS `DurableObjectIdentityV1` **is** the per-hard-link attribute:

```
capability/pre_catalog/provider/platform/macos.rs:88
        commonattr: libc::ATTR_CMN_OBJPERMANENTID,
macos.rs:83-105   object_attributes(fd) -> fgetattrlist(.., ATTR_CMN_OBJPERMANENTID, ..)
macos.rs:77       DurableObjectIdentityV1::mac(volume.volume_uuid, object.persistent_object_id)
```

and the tree carries an **executed, measured** statement of the platform fact —
`platform/anchor/tests.rs:278-284`, verbatim:

> Linux does (`name_to_handle_at` encodes the inode, which hard links share) and
> so does Windows (`FILE_ID_INFO.FileId` is per file). **macOS does not**:
> `ATTR_CMN_OBJPERMANENTID` is allocated per hard link, and creating the second
> link also re-homes the first onto an indirect node with a fresh id. Measured,
> not assumed — the fixture above asks the platform rather than reading a `cfg`.

pinned by `hard_link_identity_sharing_is_what_the_retirement_rows_assume`
(`anchor/tests.rs:308-324`), which asserts `!shared` on macOS and `shared`
elsewhere, over the probe `hard_links_share_durable_identity`
(`:293-302`). The contrast is real in the providers: `linux.rs:181`
`libc::name_to_handle_at` (inode-derived, shared across links);
`windows.rs:75-97` `FILE_ID_INFO.FileId` (per file, shared across links);
`macos.rs:88` per **link**.

**The consequence for my sub-shape, traced.** Hard-linking
`roaming-anchor-home-v1` re-homes the **home** object onto a fresh
`ATTR_CMN_OBJPERMANENTID`. Then, at the very next observation:

- `completed_record` `interior.rs:350` re-derives the roaming anchor's identity,
  feeds it to `owner_issue_for_catalog` (`:352-359`), and compares the canonical
  bytes against the resident `RetiredActionsDescriptor` and `CatalogFormat` rows
  (`:360-362`). Derived ≠ resident ⇒ `None` ⇒ `ExactOwned` not produced ⇒
  `classifier.rs:274` `Ambiguous` ⇒ `bootstrap.rs:283-288` permanent `Err`.
- Independently, `require_named_file_identity` (`completed.rs:358-376`, called
  for `RoamingAnchorHome` at `:230-233`) fails on every `revalidate`.
- And `revalidate`'s handle-identity loop at `completed.rs:204-217` fails too:
  `fgetattrlist` reads the attribute live from the vnode, so even the retained
  open handle reports the new id.

So the hard-link shape converts B1 from *a crash-window defect* into *a
deterministic first-barrier catastrophe on macOS* — the exact failure mode my
own round-1 ruling existed to prevent, self-inflicted, on a first-class target.
**It is strictly worse than the defect it cures.**

### R1.2 What survives, and what dies

**Survives — the essential ruling.** What made hard-link attractive was that
`completed_record`, `retain_file` and `require_named_file_identity` stay
untouched. The addendum is right that this property belongs to **copy-not-move
as such**, not to the sub-shape: a freshly created independent file in the
target parent touches the home object on no platform. `interior.rs:350`'s input
is `row(interior, RoamingAnchorHome)` — the home row's own observed fact — and
creating an unrelated file elsewhere cannot change it. **B1's resolution
"`completed_record` UNCHANGED" (addendum §1.3, §1.6) stands, and E2.1 correctly
receives no authorization to edit any catalog predicate.**

**Dies — the identity refinement.** Rows #7 and #11 lose their identity proofs.
The addendum states this honestly (§1.3, "What copy-not-move costs") and adds
the observation that closes it: `BarrierIntentV1` has **no alias-identity
field** (`protocol/barrier.rs:19-32` — I re-enumerated all eleven), so **no
restart could ever have re-derived the alias's identity from durable state, on
any shape, including hard link.** My round-1 claim that hard link "keeps #7/#11
a real check" was therefore over-stated even on Linux and Windows: the check
would have had no durable referent. That is a second, independent reason the
sub-shape was wrong, and the addendum found it.

**Adjudication: the addendum's override is correct on both counts. I ratify
DECISION B-5 (fresh independent copy) without reservation.** Rows #7/#11 as
rewritten — residency-and-bytes, explicitly labelled "not an identity proof"
— are the honest form. Row #9 correctly keeps its identity check, because
`target_parent_identity` **does** exist (`protocol/barrier.rs:28`); I verified
that asymmetry is real and not editorial.

*Process note, recorded against myself:* my round-1 hard-link recommendation
cited Linux and Windows identity semantics and did not check macOS. The tree
had a measured, executed answer sitting in the same file I had already read for
DECISION B-3. The lesson generalises to the addendum's own §4 lesson: **a
platform claim in this corpus must be checked against the probe fixtures, not
against general knowledge of the syscall.**

## R2. DECISION B-6 (unconditional alias) — sustained, with its cost correctly bounded

I initially suspected B-6 of making the freeze's `extra_children: 0`
contamination class (freeze `:778-781`) unconditional on all platforms. **The
code refutes my suspicion and I withdraw it.** The barrier-time path is
`validate_operation` → `RetainedActionNamespaceV1::revalidate`
(`namespace_mutation.rs:202-230`), which checks binding and durable identity
only — **it counts no children**. The `extra_children: 0` gate lives solely in
the admission classifier (`protocol/admission/owner.rs:29-38` `has_exact`), i.e.
at `resume_or_admit`, which is already unreachable after the first published row
(`namespace/tests_fault_matrix.rs:20-35`). So the alias adds one child to an
already-non-exact directory and breaks nothing new. B-6's grounds (uniform
cross-platform matrix evidence for six keys, against one 22-byte file) are
sound. **Sustained.** See [P3-R3] for the one line it should carry.

## R3. Appendix C Code item 2 — T-B′'s shape

**(a) A `&Dir` field in `DestinationRecheckV1` crosses no enforced boundary —
CONFIRMED.** I read the checker
(`gwz-core/scripts/checks/check_checked_artifact_boundaries.py`, 1,254 lines).
It enforces compiler-root manifests, source and source-tree digests, the
publication seam's shape and caller set, a raw-rename allowlist, and forbidden
provisional interfaces. **There is no rule about `cap_std`, `Dir`, or
handle-carrying types anywhere in it.** The publication seam check
(`:913-960`) constrains only: exactly one `fn publish_verified_no_replace`, one
`open_rename_source`, one `rename_open_source`, no `rename_relative`, and the
per-file caller counts. A new enum variant disturbs none of those. The
addendum's supporting argument is also correct on the code: the type is
`pub(super)` (`publication.rs:79`), `publish_verified_no_replace` already takes
two `&Dir` arguments (`:132-140`), and the arms are documented as borrowed
read-only holders (`publication.rs:50-53`).

**(b) `interior::observe` over a ≤64-child retired root returns without
refusing — CONFIRMED**, but by a different constant than the addendum names.
Walked: `:79-81` caps at `MAX_INTERIOR_ENTRIES = MAX_ROOT_ENTRIES = 74`
(`:40`); `:84-91` routes `ActiveAction` children into `action_rows` under a cap
of **`MAX_ACTIVE_ACTION_DIRS`** (`:85-87`), which is `64` (`bounds.rs:1`) and
numerically equal to `MAX_RETIRED_ACTION_DIRS` (`bounds.rs:2`); the name budget
is `CatalogNameBudgetV1::new()` (`:68`), which charges against
`MAX_CATALOG_PARENT_ENTRIES_V1 = 4_096` (`catalog/enumeration.rs:8`, `:248`).
So 64 children pass and 65 refuse. See [P3-R1] — the bound is right today by
coincidence of two equal constants, not by construction.

## R4. Appendix C Code item 3 — T-C′ needs exactly ONE forward. CONFIRMED.

Keys #6-#10 in `admission_mutation.rs`, which holds `final_directory: &Dir`
(`:79-84`) — the catalog root.

| key | needs | held? |
| --- | --- | --- |
| #6 `retired_slot_reserve` | retired root (prove child free) | **forward #1** |
| #7 `action_directory_retire` | **source_dir = catalog root** (the action dir is a *child* of it, `completed.rs:169-181` hops `final_directory.handle` → `ActiveAction(..).name()`); **destination_dir = retired root** | catalog root ✔ held; retired root = forward #1 |
| #8 `retired_directory_reobserve` | retired root | forward #1 |
| #9 `catalog_barrier` | catalog root | ✔ held |
| #10 `terminal_revalidate` | catalog root (`completed_record` re-proof) | ✔ held |

The retired root is the only capability not already in the file, and it appears
in three keys — **one forward, reused**. The rejected alternative (keep all ten
and forward the action directory as well) would indeed mint two. **T-C′'s claim
is exact.** Keys #1-#5 are action-directory reads and a flush, which
`RetainedActionNamespaceV1` already owns (`namespace_mutation.rs:122-129`,
`:135-171`) — no forward. My round-1 [P2-7] is cured better than I framed it: I
said "two forwards either way"; the split makes it one.

## R5. Appendix C Code items 4 and 5

**Item 4 — the read-side comparison belongs with the owner. CONFIRMED, and
provably so.** `protocol/barrier.rs`'s complete import set (`:1-16`) is `sha2`,
`std::io::Read`, and *value* types (`AsciiComponent`, `CanonicalPathIdentityV1`,
`DurableObjectIdentityV1`, `DurablePathV1`). A tree-wide grep shows
**`protocol/` never imports `cap_std` or `Dir` in any production file**, and
`read_and_bind_barrier_intent` takes `reader: impl Read` (`:239`) — it holds no
handle and can re-mint nothing. Putting the comparison there would drag a
capability into the pure codec layer. The addendum's placement — with the owner,
beside the five existing checks, not inside `decode_canonical` — is right, and
so is its §5.3 correction that the witness must take the
`observe_admission` / `retain_action_namespace` inverse shape
(`completed.rs:142-147`, `:169-181`) rather than `AuthorityFactsIssuerV1`'s
(`protocol/authority_record/owner/host.rs:104-119`, whose own doc at `:53-73`
disclaims argument shape as the protection). That was my round-1 [P2-3] second
half and it is fully cured.

**Item 5 — rows #6-#13 against the primitive families. CONFIRMED; no row
silently needs a recheck arm.**

| row | family | recheck arm? |
| --- | --- | --- |
| #6 create alias | **P2 only** — write-through + `sync_all` + parent flush; no rename, so no P1 at all | none needed ✔ |
| #7, #9, #11, #13 | reads | n/a ✔ |
| #8 target barrier | **P5** (`platform.rs:377-384` / `:519-567`) | n/a ✔ |
| #10, #12 retire alias | **P1** | `DestinationRecheckV1::None`, exactly as E13 does today (`namespace_mutation.rs:286`) ✔ |

Row #10's note "E13's shape; post-edge key per DECISION C-1" is load-bearing and
correct: under OPEN-B2's current pin the target parent **is** the action
directory, so the retirement is a same-directory rename onto
`RetiredRoamingAnchorAlias(ordinal)` — precisely what
`RetainedActionNamespaceV1::execute_edge` already performs, passing
`&self.handle` for both source and destination (`namespace_mutation.rs:280-283`).
So no new recheck arm, and no new sealed-primitive call site at E2. **Verified.**

**DECISION B-4** (#10 and #12 are two boundaries sharing one helper) is sound on
the code, and its self-cancelling clause — "if E2.1 finds the two
indistinguishable, #12 becomes the restart observation and #10 the edge, and it
reopens this decision rather than being taken silently" — is the right shape for
a decision resting on OPEN-B8.

## R6. [P2-R1] — a third machine-enforced inventory is missing from every convergence obligation

The addendum devotes §6 to getting one inventory right (`FAULT_INJECTION_SOURCES`,
nine → ten, once, at E2 — which I verified: nine `include_str!` entries, and the
arithmetic is correct including T-C′'s split across two already-declared files).
**There are two more, both machine-enforced, and neither document mentions
either.**

**(a) `CATALOG_PUBLICATION_CALL_COUNTS`** (`check_checked_artifact_boundaries.py:726-747`)
pins the sealed primitive's callers by file **and exact count**. Verified live —
the dict matches the tree exactly:

```
mutation.rs 1 · directory_mutation.rs 5 · admission_mutation.rs 2
namespace_mutation.rs 1 · managed_mutation.rs 2 · authority_record_binding.rs 2
```

and `check()` (`:913-960`) additionally asserts set equality between the dict's
keys and *every* production file under `provider/` that calls the primitive, so
a new caller file fails closed with "catalog publication seam changed". This is
not a routine digest bump: the dict carries four dated comments each recording a
deliberate extension by a named step, quoting the freeze's own rule — *"The
caller count stays at six production sites until a phase deliberately extends
it"* (`:711-716`, `:733-735`, `:737-741`, `:743-746`).

- **E2: no move** — barrier retirements route through `execute_edge` (§R5). Good
  news the amendment should bank explicitly, since DECISION B-1 mints a new file
  under `provider/` and a reader will assume it moves.
- **E3: MOVES.** T-B′'s terminal retirement is catalog root → **retired root**,
  and `execute_edge` is same-directory only (`&self.handle` twice,
  `namespace_mutation.rs:280-283`). It therefore needs its own
  `publish_verified_no_replace` call in `admission_mutation.rs`: **2 → 3**,
  making it the **fourteenth** production call site and the seventh deliberate
  extension. §4.5 owes that edit and its justifying comment.

**(b) `PROTECTED_SOURCE_TREE_DIGESTS`** (`:156-165`) pins whole module trees by
path-and-byte manifest — the comment is explicit that "a nested helper, **a new
source file**, or a changed module edge fails closed" (`:153-155`). I
reimplemented `source_tree_digest` (`:832-848`) and **reproduced both pinned
digests exactly** against the live tree, which confirms the coverage:

- `checked_artifact/capability/pre_catalog.rs` = `4e06f84d…` — its
  `descendant_root` is `pre_catalog/`, covering `provider/interior.rs`,
  `completed.rs`, `publication.rs`, `namespace_mutation.rs`,
  `admission_mutation.rs` and any new `barrier_mutation.rs`. **Trips at E1, E2
  and E3 — every converging commit.**
- `checked_artifact/platform.rs` = `febdc28b…` — **trips at E2**, because
  DECISION B-3 adds the third `DirentBarrierClass` variant to that file
  (`platform.rs:364-374`).

> **[P2-R1] Required fold-in.** §2.5, §3.6 and §4.5 each gain the pins their
> commit must update, beside the `FAULT_FAMILY_ACTIVATION` and
> `FAULT_INJECTION_SOURCES` edits already listed:
> - **E1.2** — `PROTECTED_SOURCE_TREE_DIGESTS["…/pre_catalog.rs"]`.
> - **E2.3** — the same, **plus** `PROTECTED_SOURCE_TREE_DIGESTS["…/platform.rs"]`
>   (B-3's variant), **plus** a stated confirmation that
>   `CATALOG_PUBLICATION_CALL_COUNTS` does **not** move because the alias
>   retirements route through `execute_edge`.
> - **E3.2** — the same tree digest, **plus**
>   `CATALOG_PUBLICATION_CALL_COUNTS["…/admission_mutation.rs"] 2 → 3` with the
>   deliberate-extension comment the freeze's caller-count rule requires.
>
> Also worth one line in §5: `operation/workspace_mutator_lock.rs` is itself
> byte-pinned (`PROTECTED_SOURCE_DIGESTS`, `:56` `0d9b034e…`), which is a further
> — and previously unstated — argument for DECISION §5.2's option (ii): probing
> at the lock would edit a pinned file for a proof nothing there uses.

**Grade.** [P2], not [P1]: it fails loudly at the first CI run and corrupts no
frozen semantics. **Parity note for the lane, stated because it is not mine to
decide:** this is the same defect class as the injection-source omission the
addendum reports the State axis graded [P1-1] — an inventory duty missing from
the convergence obligations. If that grading stands, this deserves it too.

## R7. Round-2 [P3]s

- **[P3-R1] The retired root's bound would be the *active* constant.** T-B′ §3.2
  says the arm observes the retired root "bounded under `RETIRED_ROOT_BUDGET_V1`",
  and ground 3 proposes reusing `interior::observe`. Reused, the effective caps
  are `MAX_ACTIVE_ACTION_DIRS` (`interior.rs:85-87`) and
  `MAX_CATALOG_PARENT_ENTRIES_V1 = 4_096` (`enumeration.rs:8`) — **not**
  `MAX_RETIRED_ACTION_DIRS` and not `RETIRED_ROOT_BUDGET_V1`'s `(64, 16_320,
  16_384)`. It is numerically safe **only because `bounds.rs:1` and `:2` are both
  64**, which silently couples the retired-root bound to the active one. E3.1
  should add an explicit `MAX_RETIRED_ACTION_DIRS` count check and say the arm
  does not inherit its bound. *(I raised this at [P2] on a first pass and
  downgraded it after walking `:85-87` — recorded because the addendum's own
  ground 3 cites 74 > 64 as the reason it is safe, which is the wrong constant
  twice over.)*
- **[P3-R2] T-B′'s arm text should repeat the non-`ActiveAction` refusal.**
  §2.4 item 2 has it for T1 ("accepting only children that classify
  `RootEntryNameV1::ActiveAction`"), but §3.2's shape description says only
  "proving the child row free and the root within its bound".
  `interior::observe` would happily classify an infrastructure-slot name in the
  retired root into `rows` rather than refusing it (`interior.rs:99-115`), so the
  arm must require `rows.is_empty()` too. Both halves are E3.1's; say it once in
  each place.
- **[P3-R3] B-6 should carry one line on why its cost is bounded.** Sustained at
  §R2, but the reason is non-obvious and worth recording: barrier-time
  revalidation counts no children (`namespace_mutation.rs:202-230`), so the alias
  touches only re-admission, which is already unreachable and docketed
  (`namespace/tests_fault_matrix.rs:20-35`). Without that line a reader meets
  freeze `:778-781` and concludes B-6 reopens the contamination class.
- **[P3-R4] Cross-parent retirement is latent behind OPEN-B2.** #10/#12 are
  same-directory today only because the target parent *is* the action directory
  (`namespace/host.rs:165-171`, `:746-753`). If OPEN-B2 widens the target,
  #10/#12 become cross-parent renames, `execute_edge` no longer serves them,
  and both §R6(a)'s caller inventory and the rename-domain question
  (`namespace_mutation.rs:158`) open. Worth one clause on OPEN-B2 so E2.1 sees
  the coupling before it chooses.

## R8. Round-1 dispositions — confirmation sweep

| Round-1 finding | Claimed | Verified |
| --- | --- | --- |
| [P1-1] B1 candidate (i) | FIXED §1.1 | ✔ all three facts re-derived correctly, incl. the `action_rows: Vec<ActionDigestV1>` reading (`provider.rs:196`) and the eleven-field enumeration of `BarrierIntentV1` |
| [P1-2] T-B unimplementable | FIXED §3 (T-B′) | ✔ §3.1's `publication.rs:239` single-observation argument is exact; and **T-B′ lands in the right freeze class** — freeze `:1362-1364` says in terms "`DestinationRecheckV1` … **is** an enum …, so its arm genuinely is a new variant", which is the criterion I applied blind. Class 1 is correct; Class 2 is T1's. The two are properly separated at §3.2's closing paragraph |
| [P1-3] OPEN-C1 | FIXED §4 | ✔ closure text accurate at every cite: `authority_record_binding.rs:486` (`AuthorityScratch`), hits `:508`/`:516`/`:529`, `roles.rs:18`/`:20`, zero write paths. The struck-item list is complete |
| [P2-1] three gates | FIXED §2.2 (T1); B1's struck as moot | ✔ the gate table is exact (`:310`, `:383`, call sites `:105-109`, `:245-249`), the `staging_plan` preservation note (`interior.rs:216-237`) is a correct addition of mine restated, and the "not a gate" note on `:204-217`/`:218-225` is right |
| [P2-2] key #12 fatal | FIXED §1.1 fact 3 + row #12 | ✔ and row #12 now states why it is no longer fatal |
| [P2-3] O6 part (ii) | FIXED §5.2/§5.3 | ✔ see §R5 item 4 |
| [P2-4] admitted-action duty | FIXED | ✔ **and improved on my finding**: the same in-code note names the owner I did not report — "**item 6 of the Phase 3 settle docket**" (`namespace/tests_fault_matrix.rs:27-33`) — so R2-E inherits and cites rather than minting an owner. Correct |
| [P2-5] `census.retired` | FIXED §2.3 | ✔ struck; `slots.rs:475-478` and `interior.rs:82-83` cited correctly; `driver.rs` now quoted through its last clause |
| [P2-6] REFUSE arm | FIXED §5.4 | ✔ both corrections present, including that B-5 makes "anchor away" reachable in zero states |
| [P2-7] T-C capability split | FIXED (T-C′) | ✔ see §R4 |
| [P2-8] source count | FIXED §6 | ✔ arithmetic verified against the live nine entries |
| [P3] a–l | FOLDED | ✔ all twelve accepted with correct replacements; (i)'s T-D evidence correction and (k)'s empty-`EXISTS`-domain sentence are both right |

**New open rows — all three well-formed** (question, evidence that would settle
it, owner, grade, blocking status): **OPEN-B7** (Windows round trip over a
freshly created alias) correctly demands a *measured* probe in the
`hard_link_identity_sharing_is_what_the_retirement_rows_assume` shape rather
than a `cfg` reading — the right lesson drawn from §R1; **OPEN-B8** (#10/#12
distinguishability) correctly reopens B-4 if negative; **OPEN-R1** (whether
relocation must move resident residue) is correctly graded blocking-for-the-
relocation-package rather than for R2-E, and its Windows ground is real —
`interior::exact_row` refuses any `.ca1-*` child (`interior.rs:414-417`, via
`slots.rs:434-436` returning `Foreign`), which I verified in round 1 as the
two-closed-grammars collision and which the addendum correctly promotes into
OPEN-B3's strengthening and §7.4's gate analysis.

*Outside my axis, flagged not adjudicated:* §7.8's option (b) is a charter
change and the addendum correctly declines to take it. Its ground 4 (the
O3/relocation circularity) is the one I would test hardest, but it is the State
axis's item.

## R9. What round 2 requires before freeze

1. **[P2-R1]** — fold the publication-caller inventory and the protected
   source-tree digests into §2.5 / §3.6 / §4.5, per §R6's three-line list.
2. **[P3-R1]**–**[P3-R4]** — four clauses, no structural change.

Nothing else. The pair is otherwise ready.

---

**VERDICT (round 2): GO, conditional on [P2-R1]. OVERRIDE ADJUDICATION — the
addendum's §1.2 macOS finding is CORRECT, independently re-derived at
`platform/anchor/tests.rs:278-284`, `:293-302`, `:308-324` and
`provider/platform/macos.rs:77`/`:88`: `ATTR_CMN_OBJPERMANENTID` is allocated
per hard link and creating the second re-homes the first, so hard-linking
`roaming-anchor-home-v1` would break `completed_record:350` and
`require_named_file_identity` on macOS deterministically. My round-1 hard-link
sub-shape is REFUTED and WITHDRAWN; DECISION B-5 (fresh independent copy) is
RATIFIED, and it is doubly right because `BarrierIntentV1` binds no
alias-identity field, so rows #7/#11 never had a durable referent to prove
under any shape. My essential ruling survives unchanged: `completed_record`,
`retain_file` and `require_named_file_identity` stay UNTOUCHED for B1, and
E2.1 receives no authorization to edit them.**
