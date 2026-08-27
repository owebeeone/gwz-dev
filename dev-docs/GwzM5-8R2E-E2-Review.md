# GwzM5-8 R2-E Phase E2 — interior review (single-axis, peer-blind)

**Verdict: GO CONDITIONAL on [P2-1]. ESCALATE.**

**ESCALATION (one P2, at the top as the charter requires):** the Windows-only
`<leaf>.roundtrip` residual that commit `ae4228f` exists to state is stated
**wrongly**. Its recovery mechanism — *"The next barrier over that directory
returns it (the converge step below), and every executed row proves that"* — is
**unreachable from the drive**. The drive re-creates the alias before it
barriers, which skips the converge arm, produces a state two landed records call
unproducible, costs one refused attempt, and leaves the outbound name as a
**permanent** orphan rather than a returned one. Full statement at **[P2-1]**.

Everything else in the train verifies. B-5 is complied with exactly; O6 is
closed on both sides with wire compatibility proved byte-for-byte; the sixteen
keys match the addendum's rewritten rows; the companions and per-OS markers are
honest; every gate is green on this host.

- **Object:** branch `e2-barrier`, three commits over base `94da3e5` —
  `9a6b737` (E2.2), `8245297` (E2.1+E2.3), `ae4228f` (anchor header).
  Diffstat 19 files, +2206 / −69.
- **Charter:** `GwzM5-8R2E-Plan.md` §1/§1.1/§3 Phase E2;
  `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` §3;
  `GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` §1/§5/§6.1-§6.3/§10 —
  **addendum controlling**; `GwzM5-8R2DInterfaceFreeze.md` §3.5/§6.
- **Discipline:** read-only on the object; every code claim below was opened and
  read in this worktree; no tracked file was edited and no git operation was run.
- **Host:** darwin (macOS 25.5.0). Every count in §10 is measured here except
  where marked.

---

## 0. Findings register

| Id | Grade | Axis | One line |
| --- | --- | --- | --- |
| **[P2-1]** | **P2** | 7 | The `<leaf>.roundtrip` residual's converge arm is unreachable from the drive; the drive produces the "unproducible" both-names window, refuses one attempt, and orphans the name permanently |
| [P3-1] | P3 | 5 | OPEN-B3's gate admits action-slot names another family is *using*; the fixture's own `GoalScratch`/`AuthorityScratch` choice demonstrates it |
| [P3-2] | P3 | 7 | The "two `ExactInterior` call sites" citation is wrong (there is one) and was copied forward from E0.2 §3.3 rather than checked |
| [P3-3] | P3 | 5/8 | The §3.6 "Windows arm executed natively" obligation is discharged by construction but carries no machine-visible OWED marker, unlike the linux count |
| [P3-4] | P3 | 5 | OPEN-B7's "long-resident" arm is a one-mutation proxy; the label overstates what the fixture builds (the honest-limit paragraph itself is exact) |
| [P3-5] | P3 | 7 | `mod host`'s pre-existing `dead_code` allow, whose reason says "entry-point reachability is R2-E", now covers six more methods dead until E4 — and E4 moved behind R2-F at the E0 close |
| [P3-6] | P3 | 3 | Row #9's code adds an alias-residency proof beyond §1.5's rewritten row (strictly stronger, non-divergent) — recorded so the two are not read as conflicting |
| [P3-7] | P3 | 2 | The three O6 read-side refusals are three independent perturbations inside one `#[test]`, so the first failure masks the other two (matches the file's existing style) |
| **[B-4]** | P3 | 4 | **RULING: grounds CORRECTION, not a reopening.** B-4 stands; §1.5's clause "a restart distinguishes them from the intent record alone" is false and the lane owner owes a one-line dated correction |

No P0, no P1. One P2.

---

## 1. Axis 1 — B-5 compliance. **PASS, decisively.**

This was the axis with the power to fail the train, and it is clean.

**The complete diff of the two predicate files, walked line by line.**

`git diff --numstat 94da3e5..HEAD` gives:

```
5	1	src/checked_artifact/capability/pre_catalog/provider/interior.rs
22	0	src/checked_artifact/capability/pre_catalog/provider/completed.rs
```

**`interior.rs` — one hunk, one removed line.** The only removed line in the
entire file across all three commits is:

```
-const ROAMING_ANCHOR_BYTES: &[u8] = b"GWZ-ROAMING-ANCHOR-V1\n";
```

replaced by the identical constant at `pub(super)` plus a three-line doc. The
constant's *value* is unchanged. `completed_record`, `exact_file_identity`,
`exact_row`, `owner_issue_for_catalog`'s call site, `observe`, the
`extra_children` counters, the recovery classifier's inputs — none appear in the
diff at any hunk. **No predicate semantics moved.**

**`completed.rs` — one hunk, zero removed lines.** Twenty-two added lines, all
inside `impl RetainedCompletedCatalogV1`, all one new method:

```rust
pub(in crate::checked_artifact::capability::pre_catalog) fn observe_roaming_anchor_home(
    &self,
) -> super::RoamingAnchorHomeWitnessV1 {
    super::RoamingAnchorHomeWitnessV1::owner_mint(
        self.catalog_anchor.identity.durable().clone(),
        self.final_directory.identity.durable().clone(),
    )
}
```

`retain_file`, `require_named_file_identity`, `revalidate`, `observe_admission`,
`retain_action_namespace`, the `:227-244` named-slot loop — all untouched. The
addendum's `DECISION B-5` sentence, *"E2.1 receives no authorization to edit any
of them, and needs none"*, is honoured exactly.

**The alias is a fresh copy, not a link and not a rename.** `create_target_anchor_alias`
(`barrier_mutation.rs:464-488`):

```rust
let options = super::directory_mutation::durable_write_options(true);   // create_new = true
let mut file = directory.open_with(&name, &options)?;
file.write_all(ROAMING_ANCHOR_BYTES)?;
file.sync_all()?;
drop(file);
sync_directory_edge(directory, "flush roaming anchor alias row")?;
```

That is the P2 family — write-through, `sync_all`, parent flush — under the
schedule-derived reserved leaf, exactly as row #6 specifies. There is **no**
`hard_link` anywhere in the tree's new code (`grep -rn "hard_link" src/` finds
only the pre-existing macOS platform fixture and its pinning test), and no
`rename` of the home row: the only renames the family reaches are
`RetainedActionNamespaceV1::execute_edge`'s same-directory ones, via
`namespace/operations.rs`'s pre-existing `publish_barrier_intent`,
`retire_barrier_intent` and `retire_barrier_target_alias` — all inside the
action directory.

**The home row is never opened for write, linked, renamed or removed by any
barrier edge.** Verified three ways: (i) `barrier_mutation.rs` never names
`InfrastructureSlotV1::RoamingAnchorHome` outside the witness's *name*
derivation; (ii) its only directory handles are `action.handle()` /
`target.handle()`, both the retained action directory; (iii) executed —
`assert_catalog_home_untouched` (`tests_barrier_matrix.rs:616-631`) reads
`<catalog root>/roaming-anchor-home-v1` after two complete barriers, asserts the
frozen bytes, and then re-retains the catalog through
`observe_roaming_anchor_home`, which is the strongest available statement that
no completion predicate broke — it re-runs the whole `revalidate` chain.

**The macOS hazard the addendum §1.2 raised is not present.** Nothing creates a
second link to the home object, so `ATTR_CMN_OBJPERMANENTID` is never
re-allocated and `anchor/tests.rs:308-324`'s `!shared` assertion remains a
statement about a shape this train refused rather than one it took.

**Verdict on axis 1: PASS.** Exactly the claimed surface moved — one visibility
widening and one accessor — and nothing else.

---

## 2. Axis 2 — the O6 mechanism. **PASS.**

### 2.1 The signature drops the three bare facts (part iii)

`BarrierIntentV1::issue` before/after:

```
-        catalog_anchor_identity: DurableObjectIdentityV1,
-        private_home_parent_identity: DurableObjectIdentityV1,
-        private_home_name: AsciiComponent,
+        home: &RoamingAnchorHomeWitnessV1,
```

with the derivation obligation written on the signature's own doc — *"Pass the
witness the capability owner minted; there is no route from a caller-supplied
identity."* The same substitution lands on the `cfg(test)` twin `test_issue` and
on `ActionNamespace::barrier_intent`, whose now-three-argument form let the
`#[allow(clippy::too_many_arguments)]` be dropped there (correctly retained on
`issue`, still seven arguments).

`ActionNamespace::barrier_intent` no longer forwards anything from *its* caller:
the three home facts come from the witness, and the target's identity and path
profile were already provider-observed via `BackendIssuer::barrier_target`. The
in-code claim *"after this change no field of `BarrierIntentV1` is a caller
restatement"* is true on the eleven fields: four digests derived from the
reservation, the ordinal schedule-checked, three home facts owner-minted, two
target facts provider-observed, the reserved leaf a schedule-gated reservation
(see [P3-1]), and `intent_id` derived.

### 2.2 The witness is constructible only inside the owner (part i)

```rust
pub(in crate::checked_artifact) struct RoamingAnchorHomeWitnessV1 {
    catalog_anchor_identity: DurableObjectIdentityV1,
    private_home_parent_identity: DurableObjectIdentityV1,
    private_home_name: AsciiComponent,
}
```

Fields private to `barrier_mutation`. No `Clone`, no `Default`, no `From`.
Accessors return `&`. Two constructors:

- `owner_mint`, `pub(in crate::checked_artifact::capability::pre_catalog)` —
  reachable only from the pre-catalog provider owner. `grep -rn "owner_mint"`
  finds exactly one call site: `completed.rs:180`. This is the
  `observe_admission` **inverse** shape §5.3 demanded, not the
  `AuthorityFactsIssuerV1` argument shape §5.3 warned would rebuild the defect —
  verified against `completed.rs`'s own in-code contract *"the caller receives
  the typed observation only — never a handle"*.
- `test_only`, `#[cfg(test)]` — the `NamespaceBarrierAuthority::test_only`
  precedent, cited in its own doc. `cfg(test)` is set only when the crate itself
  is compiled under test, so `tests/` integration targets cannot reach it and no
  production build compiles it. Its two callers are both in
  `interface_tests/` (`schedule_records.rs:25`, `namespace_roles.rs:142`).

**No production path can fabricate one.** The home *name* is not even a
parameter of `owner_mint`: it is derived from
`InfrastructureSlotV1::RoamingAnchorHome.name()`, so the `DERIVE` disposition of
E0.2 §3.2 is honoured rather than converted into a comparison.

*(Observation, not a finding: `owner_mint`'s visibility is the whole
`pre_catalog` owner rather than `completed` alone, so a sibling owner-private
file could in principle mint from arbitrary identities. That is exactly the
boundary §5.3 names — "inside the pre-catalog provider owner" — so the train is
compliant; a future tightening to `pub(in …provider::completed)` would cost
nothing and is worth considering at E4.)*

### 2.3 The read side re-mints per resume and refuses typed (part ii)

`read_and_bind_barrier_intent` gains a required `home: &RoamingAnchorHomeWitnessV1`
and, **after** its five existing checks and in a separate block with its own
message:

```rust
if &value.catalog_anchor_identity != home.catalog_anchor_identity()
    || &value.private_home_parent_identity != home.private_home_parent_identity()
    || &value.private_home_name != home.private_home_name()
{
    return Err(ProtocolCodecErrorV1::Invalid(
        "barrier intent does not match the observed roaming anchor home",
    ));
}
```

Requiring the witness rather than accepting an `Option` is what closes the class
— there is no route to bind a resident intent without the owner's observation.

**Re-minted per resume, executed.** `attempt_ordinals`
(`tests_barrier_matrix.rs:232-241`) opens the catalog fresh and calls
`catalog.observe_roaming_anchor_home()` on **every** attempt, before retaining
the namespace; the witness is never carried across a restart. The interruption
matrix runs one such attempt per fault key per variant, so the read-side
comparison is exercised on every one of the 16 × 2 restarts.

**Three refusals, independently perturbed** (`schedule_records.rs`):

```rust
for disagreeing in [
    home_witness(4, 2, b'h'),   // catalog anchor identity
    home_witness(1, 4, b'h'),   // private home parent identity
    home_witness(1, 2, b'i'),   // private home name
] { assert!(read_and_bind_barrier_intent(…, &disagreeing).is_err()); }
```

Each varies exactly one fact against the same encoded record. See [P3-7] for the
one-test-function caveat.

### 2.4 `decode_canonical` stays a pure codec; the REFUSE arm mints nothing

`decode_canonical` does not appear in the diff at any hunk (proved
byte-identical in §2.5). The comparison lives with the owner, beside the five
checks, exactly as §5.2 requires.

The REFUSE arm is `CompletedCatalogPermitV1::observe_roaming_anchor_home`:

```rust
self.revalidate()?;
Ok(self.completed.observe_roaming_anchor_home())
```

and `CompletedCatalogPermitV1::revalidate` (`pre_catalog.rs:144-147`) is
`revalidate_lease_root_binding` + `completed.revalidate(&self.retained_root)` —
i.e. the full completion-predicate chain. So a changed catalog-anchor identity,
a final directory that is no longer the named catalog, or a catalog whose
completion predicate no longer holds all propagate through `?` and **mint
nothing**, which is §5.4's restated arm verbatim. The prologue-first shape
matches `observe_admission`'s `ready_edge_prologue` discipline line for line.

### 2.5 Wire compatibility — verified by byte-comparison, not by eye

Zero diff on the three wire files:

```
git diff --stat 94da3e5..HEAD -- protocol/generated.rs protocol/slots.rs protocol/bounds.rs
  → (empty)
```

And, extracting each brace-balanced section from both revisions and hashing:

```
struct BarrierIntentV1     base=b6e85a0274a5 head=b6e85a0274a5 IDENTICAL=True (14 lines, 11 fields)
digest_material            base=ce35f88b9d4e head=ce35f88b9d4e IDENTICAL=True
decode_canonical           base=dc3f47dbc125 head=dc3f47dbc125 IDENTICAL=True (40 lines)
encode_canonical           base=f94a244e0d1b head=f94a244e0d1b IDENTICAL=True
from_bound_fields          base=20ad42900d10 head=20ad42900d10 IDENTICAL=True (30 lines)
```

**No record, slot, purpose or name is minted** — `ActionSlotV1::RetiredRoamingAnchorAlias`,
`BarrierIntentActive/Retired` and `BarrierIntentScratch` all pre-exist in the
frozen `slots.rs`, and `BarrierSlots::retired_anchor_alias` pre-exists in
`roles.rs` (zero diff). Freeze §6's zero-new-durable-records rule and the
E4-retire record's no-key rule are both satisfied.

**Verdict on axis 2: PASS.**

---

## 3. Axis 3 — the sixteen keys against §1.5's rewritten rows. **PASS.**

Row by row, against the **addendum's** rows (not E0.2 §3.4's):

| # | Key | Rewritten row requires | Code | ✓ |
| --- | --- | --- | --- | --- |
| 1-3 | `intent_scratch_{create,write,flush}` | P2 family on the deterministic scratch slot | `write_barrier_intent_scratch`, hits after `open_with`, after `write_all`, after `sync_all`; write-or-rewrite with `set_len(0)`+rewind, bound-checked against `ProtocolRecordKindV1::BarrierIntent.max_bytes()`, read-back compared | ✓ |
| 4 | `intent_publish` | post-edge, durable and unread | `observe_barrier_intent_row` hits `row.faults()[0]` **before** `read_bounded` | ✓ |
| 5 | `intent_reobserve` | five checks **plus** the three identity facts vs the re-minted witness (§5.2's extension) | `read_and_bind_barrier_intent(…, home)` — five then three | ✓ |
| 6 | `anchor_outbound` | **freshly created** regular file, exactly `ROAMING_ANCHOR_BYTES`, written through and flushed, parent flushed, durable **and unread**; home row untouched | `create_new`, `write_all`, `sync_all`, `sync_directory_edge`, **then** the fault hit, **then** the read-back | ✓ |
| 7 | `anchor_outbound_reobserve` | **residency-and-bytes**, explicitly *not* identity | `require_alias_resident` — no-follow bounded read, byte-equality, no identity probe; the doc says so | ✓ |
| 8 | `target_barrier` | third `DirentBarrierClass`, Windows arm round-trips the **supplied** alias by its reserved leaf | `RoamingAnchoredTarget { alias: &name, bytes: ROAMING_ANCHOR_BYTES }` → `anchor::round_trip_supplied`, which surveys nothing | ✓ |
| 9 | `target_reobserve` | **real identity check** against `target_parent_identity` | `dir_identity(directory).durable() != expected_identity` where `expected_identity` comes from `intent.value().target_parent_identity()` — from the resident record, not the drive's expectation | ✓ (+[P3-6]) |
| 10 | `anchor_return` | the alias **this drive created**, retired onto `RetiredRoamingAnchorAlias(ordinal)`, durable and unread | `AliasRetirementEntryV1::OwnDrive`; fault at helper entry, before any read; `retired_leaf` derived from `slots.retired_anchor_alias` | ✓ |
| 11 | `anchor_return_reobserve` | **two-sided** — retired row re-read **and** reserved leaf holds nothing | `require_alias_resident(retired)` then `require_absent(reserved)` | ✓ |
| 12 | `target_alias_retire` | **stranded** alias, restart entry, same helper | `AliasRetirementEntryV1::Stranded`; fixture pre-places ordinal 1's alias with `std::fs::write` so no drive created it | ✓ |
| 13 | `target_alias_reobserve` | restart path's post-read proof | same two-sided proof, then `entry.faults()[1]` | ✓ (stronger) |
| 14-15 | `intent_retire`, `intent_retired_reobserve` | the managed-intent-lifecycle twins | `BarrierIntentRowV1::Retired`'s pair, same helper as #4/#5 | ✓ |
| 16 | `completion_reobserve` | settled-ordinal restart observation | reserved leaf absent + retired alias resident with bytes + retired intent a canonical regular file | ✓ |

**The B-4 split (#10/#12) is built the way §1.5 requires.** Two ordinals in the
fixture (`SCHEDULED_ORDINALS = [0, 1]`), and the **stranded state is built
before driving** — `strand_ordinal_one_alias` runs inside `prepared()`, i.e.
before any attempt, which is precisely the state key #12 names ("found at the
target by a drive that did not create it") and the Step-4.2 technique the module
header cites. Ordinal 0 then takes #6-#11 and ordinal 1 takes #12-#13, so one
virgin attempt crosses all sixteen. Executed, both variants.

**Reconciliation is machine-checked:** `reconcile_executed_keys` sorts
`BARRIER_MATRIX`'s stable keys against `Fault::all()` filtered on the `barrier.`
prefix, asserts set equality *and* `len() == 16`.

---

## 4. Axis 4 — **RULING ON THE B-4 GROUNDS CORRECTION**

### 4.1 The correction is factually right

`drive_ordinal` publishes the intent (key #4) **before** any alias work and
retires it (key #14) **after** the alias is gone. So `BarrierIntentActive(ordinal)`
is resident on both entries into the retirement helper, and
`BarrierIntentRetired(ordinal)` on neither. B-4's clause

> *A restart distinguishes them from the intent record alone.*

is **false**. The train says so in three places — `AliasRetirementEntryV1`'s doc
(`barrier_mutation.rs:304-310`), the matrix header, and an inline comment at the
branch (`tests_barrier_matrix.rs:283-286`) — and does not paper over it.

### 4.2 The ruling: **grounds CORRECTION. B-4 stands. Not reopened. [P3].**

Five grounds, in order of weight.

**(1) OPEN-B8's own evidence test admits the in-memory fact.** The row that
governs whether B-4 reopens states its test as: *"whether the intent record's
resident row (`BarrierIntentActive(ordinal)` vs `BarrierIntentRetired(ordinal)`)
**plus the in-memory write-handle identity** partition the two entries."* That is
a **conjunction**, and the conjunction holds: the drive that took the `else`
branch created the alias and holds that fact; the drive that took the `if`
branch found an alias it did not create. The durable half alone fails; the test
the charter set does not ask the durable half alone. OPEN-B8 therefore answers
**positive**, and its negative branch ("#12 becomes the restart observation and
#10 the edge") is not triggered.

**(2) The negative branch is not even expressible under DECISION C-1.** Both #10
and #12 are already **post-edge** keys under C-1's routing rule: the *edge* is
`namespace.retire_exact`, an R2-D-executed key reached through
`RetainedActionNamespaceV1::execute_edge`. Neither #10 nor #12 is an edge key
today, so "make #10 the edge" would require re-routing the family off C-1 —
a far larger move than the "one line" the fallback describes. The fallback was
written against an assumption C-1 has since displaced.

**(3) What a restart actually needs to do at each entry — the test the charter
asked me to apply — is identical, and that is what B-4 authorized.**
`retire_target_anchor_alias` performs the same work on both entries: prove the
retirement row carries exactly `ROAMING_ANCHOR_BYTES`, prove the reserved leaf
holds nothing. Only the announced key pair and the label differ. B-4's own text
authorizes exactly this ("They share one helper") and explicitly **rejected**
giving #10 a *different* action (the alias's removal), on the ground that
"retirement preserves evidence where a foreign object may sit at the reserved
leaf". That property is real in the code and is the same on both entries: the
rename is no-replace, and a foreign object is renamed into the ordinal's
retirement slot and *then* refused by `require_alias_resident` — preserved, not
destroyed. So the behavioural identity of the two entries is B-4's design, not
evidence against it.

**(4) Nothing consumes the announced key, and the settled state is provably the
same on both paths.** No predicate reads which fault key was announced;
`observe_barrier_completion` is byte-identical on both entries. Executed:
`assert_settled_census` shows ordinal 0 (OwnDrive) and ordinal 1 (Stranded)
leaving the *same shape* — `RetiredRoamingAnchorAlias(0)` and `(1)` alongside
`BarrierIntentRetired(0)` and `(1)` and the reservation, five rows, nothing at
either reserved leaf. What a durable partition would have bought — a
memory-less fresh process choosing between two names — is not needed by any
consumer.

**(5) Exactly-one-entry-per-ordinal is enforced by durable state even though
which-entry is not.** `RetiredRoamingAnchorAlias(ordinal)` is filled by whichever
entry ran, and a filled retirement row routes every later drive past both
(`if !namespace.scheduled_row_is_resident(&retired_alias)`). So the durable
record does carry the safety-relevant fact (*this ordinal's alias is disposed
of*); what it does not carry is the audit-relevant fact (*by whom*). The former
is what correctness needs.

**Conclusion.** The in-memory partition **is** sufficient for B-4's substance —
two boundaries, one helper — so this is a grounds **CORRECTION**, honestly
recorded, and **not** a reopening. The clause the charter feared ("If E2.1 finds
the two indistinguishable in code … it reopens this decision rather than being
taken silently") does not fire, because they are *not* indistinguishable and
because nothing was taken silently.

### 4.3 What the ruling still owes

The correction currently lives **only in gwz-core source comments**. B-4's text
in `GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` §1.5 still carries the false
clause, and that document is the lane owner's. **The lane owner owes a one-line
dated correction** to §1.5's B-4 paragraph, replacing

> *A restart distinguishes them from the intent record alone.*

with the in-memory partition, and an OPEN-B8 close marked **answered positive at
E2.1 on the conjunction, with the durable half struck**. Graded P3, owner: lane
owner (dev-docs); not this train's to edit and correctly not attempted.

---

## 5. Axis 5 — the four OPEN rows

### OPEN-B2 — no widening. **PASS.**

Answered "the target parent stays action-directory-pinned", with three grounds
in `barrier_mutation.rs`'s header (the coupling clause; the settling evidence is
an E4 consumer that cannot exist under the operator's ruling (a); and
`RetainedManagedParentV1`'s only caller today is the `cfg(test)` door). Enforced,
not merely recorded — `validate_barrier_slots` (`host.rs:415-432`) re-proves on
**every** one of the six new entry points that
`slots.target.parent` is the retained action directory by handle, identity **and**
path profile.

**The coupling clause's three consequences, each verified not to fire:**

1. *`CATALOG_PUBLICATION_CALL_COUNTS` would move.* It does not. The negative is
   **banked explicitly** in the dict itself, in the dict's own dated-comment
   form, and — the part that matters — its wording **carries the condition**:
   *"This holds only while OPEN-B2's answer holds. E2.1 answered it 'the target
   parent stays action-directory-pinned'; if a later step widens the target to a
   retained managed parent, those retirements become cross-parent renames,
   `execute_edge` no longer serves them, and this dict moves with a new
   `barrier_mutation.rs` row."* That is the right wording: it states the
   negative, its ground, and its expiry condition, so the set-equality check is
   not the first thing a future step hears.
2. *A new sealed-primitive call site would appear.* It does not:
   `grep -n "publish_verified" barrier_mutation.rs` → empty. The file's only
   write primitives are `durable_write_options` (×2) and `sync_directory_edge`.
3. *The rename-domain question would apply to a pair of parents.* It does not —
   both retirements are same-directory through `execute_edge`. Named in the
   file's header, though not in the checker comment (which names 1 and 2 only);
   acceptable, since the header is where the answer lives.

### OPEN-B3 — the reserved-leaf gate. **PASS on placement; see [P3-1] on the honest limit.**

`require_reserved_target_leaf` (`host.rs:938-948`) requires
`ActionSlotV1::parse(action, leaf) == Valid(_)`.

**Is it on every path including restart re-binding?** Yes. `BarrierSlots` is
constructed at exactly one site — `namespace/mod.rs:188`, inside
`ActionNamespace::barrier_slots` — and `barrier_slots` has exactly one
production caller, `scheduled_barrier_slots` (`host.rs:159`), which calls the
gate at `:175` **before** minting the target. The other `barrier_slots` callers
are `interface_tests/namespace_roles.rs` and `namespace/test_support.rs`, both
test-only. A restart re-binds through the same `scheduled_barrier_slots`,
reading the leaf from the durable record via the new
`BarrierIntentV1::reserved_target_leaf()` accessor, so first drive and restart
pass through **one** gate. Verified.

**Is the honest-limit statement accurate?** Substantially yes, and unusually
frank: *"What the gate does not do, stated honestly: no frozen slot names the
live alias… E2.1 holds no authorization to mint a slot — that would move
`BASE_ACTION_SLOTS` and the `MAX_ACTION_SLOTS == 261` compile-time assertion."*
Both halves check out. The three physical guards it names are real
(`create_new`; no-replace retirement; the leaf bound durably so a restart reads
rather than re-chooses). One gap, graded at [P3-1].

### OPEN-B7 — the probe. **PASS on honesty; see [P3-3] and [P3-4].**

`the_supplied_roaming_round_trip_is_measured_on_both_alias_shapes` calls the real
`round_trip_supplied` on the real platform (no `cfg` read), on two alias shapes,
and asserts identity equality before/after plus a clean directory. That is the
`hard_link_identity_sharing_is_what_the_retirement_rows_assume` shape.

**It says what it cannot**, in terms:

> *What it does not prove, stated so a green run is not over-read: the dirent
> ordering the Windows round trip exists to deliver is not observable from
> inside the process. This row proves the mechanism is available and
> identity-preserving on both shapes; the native Windows leg runs at the
> three-platform dispatch.*

That is exact and is the correct disposition. Four further rows fence the arm:
empty target refused with nothing planted; outbound-name convergence; both-names
refused; foreign bytes refused before any rename.

### OPEN-B8 — answered positive; see the B-4 ruling at §4.

---

## 6. Axis 6 — DECISION B-6. **PASS.**

**The alias is created on every platform.** `create_target_anchor_alias` carries
no `cfg`; `drive_ordinal` calls it unconditionally; the matrix's eight tests run
on this darwin host and cross all sixteen keys, so none of #6-#13 is a vacuous
non-Windows row. Rows #6/#7/#10-#13 are real process stops across real durable
edges here — the property B-6 bought the extra file for.

**The bounded-cost claim, verified against the tree:**

- *"revalidate counts no children"* — `RetainedActionNamespaceV1::revalidate`
  (`namespace_mutation.rs:202-230`) compares the binding, reopens the named
  directory no-follow, and compares `dir_identity` on both handles. There is no
  `read_dir`, no counter, no interior observation. **True.**
- *"`extra_children: 0` lives solely in the admission classifier … at
  `resume_or_admit`"* — the gate appears at `protocol/admission/owner.rs:34/45/62`
  (`has_exact`, `has_rewritable_reservation`, `exact_identity_for`), consumed only
  at `owner.rs:101-107`; and at `interior.rs:548` via
  `ActionInteriorObservationV1::is_exact`, consumed only at `publication.rs:204`
  under `DirectoryInteriorExpectationV1::AdmissionStaging` — i.e. the
  publication-source recheck for a **directory** source, which is the admission
  publish. **Substantially true**: there is a second consumer the addendum's
  sentence does not name, but it is also an admission path and is unreachable
  from the barrier family, whose publications are all `PublicationSourceV1::Leaf`
  through `execute_edge`. So the alias adds one child to an already-non-exact
  action directory and breaks nothing — verified, not inherited.

Independently, `DestinationRecheckV1::PreRetirementFinal` re-runs
`interior::completed_record` on the **catalog root**, which an alias inside an
action directory cannot perturb, and which under B-5 keeps holding because the
home row never moves. Executed by `assert_catalog_home_untouched`.

---

## 7. Axis 7 — the deviations

### 7.1 The `DirentBarrierClass` lifetime parameter. **Clean.**

`pub(super) enum DirentBarrierClass<'alias>` with the new
`RoamingAnchoredTarget { alias: &'alias OsStr, bytes: &'alias [u8] }`.

**Are the existing call sites textually unchanged?** Yes — *all eight*, not
seven, and **all in files with zero diff**:

```
cleanup.rs:91, :136, :171            AnchoredPrivateArea
transition.rs:455                    AnchoredPrivateArea
residue.rs:411, :519, :593           AnchoredPrivateArea
namespace_mutation.rs:328            ExactInterior
```

None of those four files appears in the diffstat at all. Unit variants need no
lifetime argument, so the parameter is invisible to them; only the two
`private_barrier` signatures inside `platform.rs` gained `<'_>`.

**Did the byte pins move?** No. `PROTECTED_SOURCE_DIGESTS["checked_artifact/residue.rs"]`
= `717c2417…` and `["checked_artifact/transition.rs"]` = `13b483bc…` are
unchanged in the diff, and the checker passes against the live bytes — which is
the proof, since a moved byte would have failed it closed.

*(Note: the charter's "seven" appears to be an undercount; the tree holds eight.
No consequence — all eight are untouched.)*

### 7.2 The per-ordinal single-crossing classification. **The ground is real.**

**Does a two-ordinal drive legitimately cross `intent_retire` twice?** Yes.
`drive_ordinal` is invoked once per ordinal; each ordinal owns its own
`BarrierIntentActive/Retired(ordinal)` rows, its own alias and its own
retirement row, so `observe_barrier_intent_row(…, Retired, …)` — which announces
#14 and #15 — runs once per ordinal. A whole-drive probe would fire twice and
would be measuring the schedule's *width*, not the boundary's routing. The
module says exactly that, and it is correct.

**Is it a weakening of the single-crossing discipline?** No — it is the **same**
discipline the charter already applied at E1, whose §2.5 obligation reads
"single-crossing classification declared per row, driven by
`run_single_crossing_probe` …, **on a one-alias row**". One ordinal here is the
exact analogue of one alias there. And the probe does not hide behind the
subset: after the one-ordinal re-arm survives unfired, it runs the **full
two-ordinal** `attempt` and asserts the whole drive settles
(`tests_barrier_matrix.rs:502-503`). The 6/10 partition is machine-checked by
`assert_boundary_partition`'s per-key XOR plus the length sum. Both
classifications check out against `drive_ordinal`'s resident-row short-circuits.

### 7.3 The Windows-only tolerated orphan `<leaf>.roundtrip` — **[P2-1]**

**[P2-1] The residual's stated recovery mechanism is unreachable from the drive;
the drive produces the state two records call unproducible, refuses one attempt,
and orphans the name permanently.**

The claim under review, added by `ae4228f` (`platform/anchor.rs` module header):

> *A crash between this arm's two renames leaves the alias under `<reserved
> leaf>.roundtrip`. **The next barrier over that directory returns it (the
> converge step below), and every executed row proves that.** If no later barrier
> runs, the name persists as a tolerated orphan — the same disposition the legacy
> nonce orphans have: bounded by past crashes, refusing nothing and blocking
> nothing…*

**The converge arm fires only when the reserved leaf is empty:**

```rust
if !is_resident(dir, alias, code, label)? && is_resident(dir, &roundtrip, code, label)? {
    let stranded = verify_bytes(dir, &roundtrip, bytes, code, label)?;
    publish_bytes(dir, &roundtrip, alias, &stranded, bytes, code, label)?;
}
if !is_resident(dir, alias, code, label)? || is_resident(dir, &roundtrip, code, label)? {
    return Err(error(code, label,
        "supplied roaming anchor alias is not exactly resident at its reserved leaf"));
}
```

**The drive never presents that state.** `drive_ordinal` branches on the
*reserved leaf*, and after a mid-round-trip crash the reserved leaf is **empty**
(the alias is out under `.roundtrip`):

```rust
let entry = if namespace.scheduled_row_is_resident(&reserved) {
    AliasRetirementEntryV1::Stranded
} else {
    namespace.create_target_anchor_alias(&slots)?;   // create_new SUCCEEDS on the empty leaf
    namespace.barrier_target_parent(&slots, &bound)?;
    AliasRetirementEntryV1::OwnDrive
};
```

So the restart takes `OwnDrive`, creates a **second** object at the reserved
leaf, and only then calls the barrier — by which time `alias` is resident, the
converge arm is skipped, and the guard trips on `is_resident(roundtrip)`. **The
attempt refuses.**

**Consequences, traced:**

1. The claim *"the next barrier over that directory returns it"* is false on the
   drive path. The converge arm is structurally unreachable from above, because
   the only caller that could present the "alias absent, `.roundtrip` present"
   state would have to know the `.roundtrip` derivation, which is
   `anchor`-module-private and not exposed.
2. The sibling test's justification is falsified too.
   `a_roaming_alias_resident_under_both_names_is_refused` states: *"Both names
   resident at once is **not a window this protocol can produce** — it renames
   one object and never copies."* Under `drive_ordinal` it copies (`create_new`
   after the crash) and produces exactly that window.
3. The orphan is **permanent**, not merely un-returned. The attempt after the
   refused one finds the reserved leaf resident → `Stranded` → settles; the
   ordinal is then settled, so every later drive short-circuits at
   `observe_barrier_completion` and no barrier over that ordinal ever runs
   again. Per-ordinal reserved leaves mean no other ordinal of the action uses
   that name either.
4. **The green run does not contradict any of this**, because there is no fault
   key inside the round trip: `grep -n "fault_v1::hit" platform/anchor.rs` →
   empty. Key #8 fires *after* `private_barrier` returns. The matrix therefore
   cannot produce a mid-round-trip crash, and the residual's drive-level
   behaviour is entirely unexercised.

**What survives of the disposition:** *bounded by crashes* — yes, one per
crash-in-window per reserved leaf. *Blocks nothing* — yes, verified: the
`extra_children` gates are admission-only (§6), and the ordinal does settle. So
the residual's **class** is right; its **mechanism sentence** and its **cost**
are wrong.

**Why P2 and not P3.** Commit `ae4228f`'s entire stated purpose is to state this
residual honestly; the sentence that states it is false in its mechanism, and
the falsity is not editorial — it has a behavioural tail (one refused attempt,
plus a permanent rather than converged orphan) on a crash-recovery path. A
landed record asserting a recovery mechanism the code cannot reach is the same
class this program has graded before ("the recorded inventory was false").

**Why not P1.** Windows-only in production (`private_barrier`'s non-Windows arm
ignores the class entirely); the ordinal still settles on the following attempt;
the orphan is tolerated by every predicate that reads an action directory; and
the family has zero production consumers today.

**Honest adjustments (lane owner's choice, all small):**

- **(a) Restate.** Replace the mechanism sentence with what the code does: *"a
  mid-round-trip crash costs one refused attempt; the attempt after it settles
  the ordinal through the `Stranded` entry, and the outbound name persists as a
  permanent tolerated orphan."* Correct
  `a_roaming_alias_resident_under_both_names_is_refused`'s doc in the same
  breath, and re-title it as the **drive-reachable** ambiguity it actually is.
- **(b) Make it reachable.** Have `create_target_anchor_alias` refuse when the
  outbound name is resident (needs `anchor::roundtrip_name` exposed to the
  provider), turning the case into one typed refusal at a named boundary with
  the same orphan and no second object.
- **(c) Converge properly.** Let the arm prefer the outbound object when both
  names are resident. Rejected here on sight: it needs a *removal*, which
  Step 4.2 deliberately replaced with durable retirement.

Recommendation: **(a) plus (b)** — (a) makes the record true, (b) removes the
second object and stops the drive from manufacturing an "unproducible" state.

### 7.4 The two visibility widenings. **Both minimal and correct.**

- `interior.rs`: `const ROAMING_ANCHOR_BYTES` → `pub(super)`. Reaches only
  `provider/*`. The alternative — duplicating the 22 bytes into
  `barrier_mutation.rs` — would have created a second source of a frozen
  constant. Right call. Note the *test* file `tests_barrier_matrix.rs:170` and
  `anchor/tests.rs` do each re-declare the literal, which is unavoidable (they
  are outside `provider/`) and is the corpus's existing habit.
- `tests_fault_matrix.rs`: `Fixture::action_children` → `pub(super)`, so the
  barrier matrix converges on the same census helper rather than writing a
  second one. No production visibility moved.

**Full removed-line audit (all 69 removed lines across the range, categorised):**
3 tree digests re-pinned · 1 count pin · 1 constant re-declared at wider
visibility · 1 `FAULT_FAMILY_ACTIVATION` row Reserved→Executed · 1 doc line
extended · ~14 import-list re-wraps · 4 the now-unneeded
`#[allow(too_many_arguments)]` on `barrier_intent` · 27 the three-fact parameter
triples at `issue` / `test_issue` / `barrier_intent` and their three call sites ·
1 `action_children` visibility · 5 `DirentBarrierClass` lifetime and doc ·
5 the `publish`→`publish_bytes` / `verify`→`verify_bytes` delegation · 6 the
`bound_reservation()` factoring in `schedule_records.rs`.

**Zero `assert!`/`assert_eq!` removed. Zero assertions weakened.** The one
mutated assertion — `Cursor::new(bytes)` → `Cursor::new(bytes.clone())` — is a
borrow fix so `bytes` survives for the three added refusal rows; the assertion
itself is unchanged. The `publish`/`verify` delegation is behaviour-preserving:
both keep `ANCHOR_BYTES` and simply forward.

---

## 8. Axis 8 — matrix honesty. **PASS.**

- **`BARRIER_MATRIX: [Fault; 16]`**, in virgin-drive order, both variants,
  four test functions × two variants = eight tests, all green.
- **Real process stops.** `run_next_barrier_fault(key, || panic!(…))` +
  `catch_unwind` + `assert!(interrupted.is_err(), "fault point was not reached")`
  — so a key that never fires fails the row rather than passing silently. Each
  attempt re-opens the catalog (`with_catalog`), re-mints the witness, and
  re-retains a **fresh** namespace capability, so a restart is a genuine
  fresh-process resume and not a continuation.
- **Convergence is settled, not merely reached.** Each row asserts
  `resumed_rows == settled_rows` **and then** runs a second settle asserting
  `again_rows == settled_rows` — "the resume mutated the settled action
  directory" is a real failure mode the matrix closes.
- **6/10 XOR partition machine-checked** — `assert_boundary_partition`, per key
  plus the length sum, called at the top of both interruption-matrix tests.
- **Twelve rounds over ALL six repeatable boundaries** — `REPEATED_CRASH_ROUNDS
  = 12`, `for key in REPEATED_BOUNDARIES` (not a sub-selection), with a
  census-stability assertion every round and a final convergence assertion. The
  stated ground ("the virgin drive settles two ordinals in six durable edges and
  leaves five rows, so twelve crashes cross the nominal capacity several times
  over") is checkable and holds.
- **Reconciliation** — `reconcile_executed_keys`, set equality against the
  vocabulary plus `len() == 16`.
- **Settled census pinned by name, not count** — five names asserted
  explicitly, plus a byte check that both retired alias rows carry
  `ROAMING_ANCHOR_BYTES`, plus `assert_catalog_home_untouched`.
- **The Git-directory route is stated**, as §11.3 item 2 requires: both variants
  drive the same target (the retained action directory reached through the
  permit's no-follow hop), neither takes the Step-2.3 `cfg(test)` managed-parent
  door, and the door's disposition is left to E4.2. Honest — it does not claim a
  distinct physical target it does not have.
- **The classifications' stated grounds are true of the code.** #4/#5 repeatable
  because the publish is residency-guarded but the observation is unconditional
  (`if !scheduled_row_is_resident(&active) { … }` then an unconditional
  `observe_barrier_intent_row`); #16 repeatable because a settled ordinal
  short-circuits into it; #6-#9 single-crossing because a crash leaves an alias
  the restart cannot prove is its own, routing it to `Stranded`; #10-#15
  single-crossing on the resident-row short-circuits. All four verified by
  reading `drive_ordinal`.

One residual gap, already graded: no fault key exists inside the round trip, so
key #8's *interior* window is unexercised — see [P2-1].

---

## 9. Axis 9 — companions. **PASS, with the inventory gap noted.**

- **`FAULT_INJECTION_SOURCES` 9 → 10**, one file (`barrier_mutation.rs`), with
  the doc-comment entry, **and the `declared.len() == 10` pin** added to
  `the_declared_injection_sources_are_every_production_source_holding_sites` —
  the two-place deliberate edit §6.1 demands, with the failure message naming
  the freeze's §3.5 addendum as the co-owner of the move. Correct: this is the
  **only** count move across E1-E3.
- **`FAULT_FAMILY_ACTIVATION`** `barrier` row `Reserved` → `Executed("R2-E phase
  E2 (roaming anchor barrier)")`, count 16 unchanged; `barrier` added to the
  executed-family literal in `only_the_families_with_executed_matrices_are_executed_today`.
  The comment records that the three ACTIVATE-CONDITIONAL-ON-OPEN-B1 keys
  activate with the rest because OPEN-B1 closed at E0.2b — accurate.
- **The freeze inventory-addendum edit correctly NOT attempted.**
  `GwzM5-8R2DInterfaceFreeze.md` does not exist in this worktree's `dev-docs/`
  (it lives in the gwz-dev repo). The commit could not have made that edit, and
  did not fake it. Right call; the "nine → ten" §3.5 addendum edit is **owed by
  the lane owner** at landing.
- **`CATALOG_PUBLICATION_CALL_COUNTS` negative banked explicitly**, in the
  dict's own dated-comment form, with the OPEN-B2 expiry condition attached —
  see §5. Verified against the tree: `barrier_mutation.rs` opens no sealed
  primitive.
- **All three tree digests verified against actual bytes** — by the checker,
  which passes and would fail closed on any mismatch:
  - `checked_artifact/capability/pre_catalog.rs` → `46103ca5…` (gains
    `provider/barrier_mutation.rs`) — forecast by §6.2(b);
  - `checked_artifact/platform.rs` → `8dfe3101…` (third `DirentBarrierClass`) —
    forecast by §6.2(b);
  - **`checked_artifact/catalog.rs` → `cfbace35…` — NOT in §6.2(b)'s inventory.**
    This is a **§6.2(b) inventory gap**, not a train defect: O6's witness reaches
    the barrier owner through `OpaqueRetainedCatalogV1`, whose forwarder lives in
    `catalog/bootstrap.rs` under that tree. The train **recorded it rather than
    absorbing it**, in a comment naming the gap and its cause. That is the right
    handling and is worth the lane owner folding back into §6.2(b) so E3 does
    not rediscover it.
- **Per-OS markers.** `_fault_count("413 passed", "423 passed")`, with a
  docstring that states the thirteen added lib tests (eight
  `tests_barrier_matrix` + five `platform::anchor::tests`), marks darwin
  **MEASURED** and linux **DERIVED (410 + 13), not measured, and therefore
  OWED**, plus a `LINUX-COUNT-OWED (R2-E E2)` comment at the row itself. The
  remainder row is left at `("932 passed", "933 passed")` with the docstring
  asserting it re-measured unchanged. **All three darwin values reproduce here:
  413 / 932 / 256.** This is the honest form — every other value in that
  function was executed before it was written, and the docstring says this one
  was not.

---

## 10. Axis 10 — gates re-run. **All green. Verbatim tails.**

```
$ cargo fmt --all -- --check
(no output)
FMT_EXIT=0
```

```
$ cargo check --all-targets
    Checking gwz-core v0.11.0 (…/e2-worktree)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 10.90s
CHECK_EXIT=0
```

```
$ cargo clippy --all-targets --all-features
    Checking gwz-core v0.11.0 (…/e2-worktree)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 14.35s
```

```
$ RUSTFLAGS="--cfg e2_review_probe" cargo clippy --lib -p gwz-core     # forced fresh, non-test cfg
    Checking gwz-core v0.11.0 (…/e2-worktree)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 17.09s
```

```
$ cargo test --lib -p gwz-core namespace::tests_barrier_matrix -- --nocapture
test result: ok. 8 passed; 0 failed; 0 ignored; 0 measured; 1595 filtered out; finished in 9.65s
```

```
$ cargo test --lib -p gwz-core interface_tests::fault_expected_keys
test result: ok. 6 passed; 0 failed; 0 ignored; 0 measured; 1597 filtered out; finished in 0.03s
```

```
$ cargo test --lib -p gwz-core interface_tests::schedule_records
test result: ok. 7 passed; 0 failed; 0 ignored; 0 measured; 1596 filtered out; finished in 0.00s
```

```
$ cargo test --lib -p gwz-core platform::anchor
test result: ok. 22 passed; 0 failed; 0 ignored; 0 measured; 1581 filtered out; finished in 0.14s
```

```
$ python3.13 scripts/checks/check_checked_artifact_boundaries.py
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
```

**The three R4b-G count-pinned partitions, re-measured on this darwin host:**

```
$ cargo test --lib -p gwz-core checked_artifact::
test result: ok. 413 passed; 0 failed; 0 ignored; 0 measured; 1190 filtered out; finished in 55.20s

$ cargo test --lib -p gwz-core -- --skip checked_artifact:: --skip workspace_ops::merge::v1_lifecycle::
test result: ok. 932 passed; 0 failed; 1 ignored; 0 measured; 670 filtered out; finished in 90.12s

$ cargo test --lib -p gwz-core workspace_ops::merge::v1_lifecycle:: -- --skip root_fault_matrix
test result: ok. 256 passed; 0 failed; 0 ignored; 0 measured; 1347 filtered out; finished in 503.73s
```

All three darwin pins in `run_r4bg_aggregate_gates.py` reproduce exactly.
The linux 423 remains **OWED** and is marked as such.

Sample matrix evidence line (both variants print one per key):

```
barrier.target_alias_retire | workspace | interrupted=yes | restart=settled | rows=5 | resume=no-mutation
barrier.completion_reobserve | git-directory | rounds=12 | slots-stable=yes | rows=[…4 names…] | converged=yes
```

---

## 11. The remaining P3 findings, stated

**[P3-1] OPEN-B3's gate admits action-slot names another family is *using*.**
`require_reserved_target_leaf` accepts any `Valid(_)` `ActionSlotV1` of this
action, which includes base slots other families own. The matrix's own choice
demonstrates it: `RESERVED_LEAVES = [GoalScratch, AuthorityScratch]`. The
module header says the fixture "picks two legal action-scoped slots **the
barrier family** does not otherwise use" — true of the barrier family, not of
the action. A live collision is a typed refusal (`create_new`), so this is
hygiene rather than corruption, but the honest-limit paragraph covers only "no
frozen slot names the live alias" and not "the gate admits a slot another family
is holding". **Owed:** one clause on `require_reserved_target_leaf`'s doc, and a
warning that E4 must not reserve a leaf another family writes.

**[P3-2] The "two `ExactInterior` call sites" citation is wrong and was copied
forward.** `platform.rs`'s new `RoamingAnchoredTarget` doc asserts "both
`ExactInterior` call sites (`namespace_mutation.rs`'s `barrier`, and the
barrier-target pin in `namespace/host.rs`)". The tree holds **one**
`ExactInterior` construction site (`namespace_mutation.rs:328`); base
`host.rs:741-756`'s `barrier` is an identity pin that delegates to
`self.retained.barrier()` and names no class. The error originates in E0.2 §3.3
and was inherited rather than checked, against the lane's own house rule that
every code claim is opened and read. **The substance is unaffected and
independently verified**: this family's call site is a distinct one in a distinct
file, so the §4.3 E10/E14 annotation genuinely does not move.

**[P3-3] The §3.6 "Windows arm executed natively, not skipped" obligation carries
no machine-visible OWED marker.** It is discharged *by construction* — the five
`round_trip_supplied` rows are portable, ask the platform rather than a `cfg`,
and will execute natively on the Windows leg, as will key #8 end-to-end through
`private_barrier`'s Windows arm. But on this host `private_barrier` never
reaches the arm, so nothing here proves it; the only record is a Rust doc
sentence. **Owed:** a `WINDOWS-ARM-OWED (R2-E E2)` comment beside
`LINUX-COUNT-OWED` in `run_r4bg_aggregate_gates.py`, so the landing dispatch
cannot forget it the way the linux count cannot.

**[P3-4] OPEN-B7's "long-resident" arm is a one-mutation proxy.** The loop's
second shape writes a `noise` file and unlinks it, then round-trips an alias
created in the same breath. The test's own prose is accurate ("resident across
other directory mutations"); the label `"long-resident"` overstates what was
built. Cheap to fix by renaming the label to `"aged-directory"`. The
honest-limit paragraph is exact and is the reason this is P3 and not higher.

**[P3-5] `mod host`'s `dead_code` allow now covers six more methods.** The allow
is **pre-existing** (`namespace/mod.rs:9-13`, from R2-D Step 2.2) and does not
appear in this diff, so it is not this train's defect. But its stated reason —
*"entry-point reachability is R2-E"* — has drifted: E2 adds six more methods with
no production caller (`grep` confirms only `tests_barrier_matrix` calls them),
and Phase E4, which was to supply the entry points, moved behind R2-F's
relocation at the E0 close under operator ruling (a). **Owed:** a dated note so
the allow's expiry is re-anchored to E4's new position rather than quietly
becoming permanent.

**[P3-6] Row #9's code proves one thing more than §1.5's rewritten row.**
`barrier_target_parent` calls `require_alias_resident` at `:526` in addition to
the identity comparison. §1.5 deliberately *dropped* a residency clause from row
#9 because under B-5 "the home anchor never travels". The added proof is about
the **alias**, a different object that genuinely is resident at that point, so it
does not resurrect the dropped state and is strictly stronger. Recorded so the
row and the code are not later read as divergent.

**[P3-7] The three O6 read-side refusals share one `#[test]`.** They are three
independent perturbations, but inside one `for` loop in
`barrier_intent_id_binds_every_persisted_field`, against one combined `if` with a
single message — so the first failure masks the other two and no per-fact
refusal name exists. This matches the file's existing treatment of the five
reservation/ordinal checks, so it is consistency rather than regression.
Recorded, not required.

---

## 12. What this review did not do

- It did not run the Windows or Linux legs; every count and every executed row
  above is darwin. The linux census (423) and the native Windows arm are owed at
  the three-platform dispatch — see [P3-3] and §9.
- It did not review E1 (`cleanup.*`) or E3 (`terminal.*`), nor the parts of the
  E0.2/E0.2b amendments outside §1/§3/§5/§6/§10.
- It edited no tracked file in the object and ran no git operation. This report
  is the only file written.

---

*Filed 2026-08-27 by the R2-E Phase E2 interior reviewer (single-axis,
peer-blind). Object: `e2-barrier` @ `ae4228f` over base `94da3e5`.*
