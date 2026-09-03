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

---
---

# Round 2 — the remediation, 2026-08-27 (final round under the two-round cap)

**Verdict: GO. Land it.**

**[P2-1] is cured, and cured better than I proposed.** I recommended a
refuse-on-resident guard; the builder declined it on a stronger bar than mine —
*no reachable crash window may leave a name no later drive returns* — and
observed correctly that my shape leaves the orphan permanent and merely makes
the refusal tidier. What it built instead moves the entry decision to the owner
of both names. That is the right cure, and I withdraw my recommendation in
favour of it.

**All seven P3s are taken**, several of them beyond what I asked. Two further
tightenings were volunteered, both real. Three new P3s below, **none a landing
blocker**: two are E7-dual items, one is landing-optional wording.

- **Object:** `9ba6e24` appended to `e2-barrier` (12 files, +712/−121).
- **Round 1 is left as written above**, including the [P2-1] statement the
  remediation falsifies — that is the record of what was found, and this section
  is the record of what happened to it.
- **Scope note the coordinator supplied and I have honoured:** E1 and E3 are
  landed on `main` (`1d50e59`); this train lands **squashed** onto that tip with
  pins re-executed by the lane owner, so per-commit pin staleness inside the
  branch is not a finding. I gate the **content**. Everything below was measured
  on `9ba6e24` over base `94da3e5`, and §R2.8 lists what the rebase itself owes.

---

## R2.0 Round-2 findings register

| Id | Grade | Disposition |
| --- | --- | --- |
| **[P2-1]** | **CLOSED** | Cured at the entry decision, disclosed truthfully, driven on both target variants |
| [P3-1] … [P3-7] | **ALL TAKEN** | Spot-checked one by one at §R2.4 |
| **[R2-P3-1]** | P3 | "Ordinal settled" does **not** imply "its physical barrier ran" — sharpened by the new row three. Charter-compliant; **E7-dual item + E4 scope clause** |
| **[R2-P3-2]** | P3 | Row three's return rename is a durable edge in the barrier family that **no fault key announces**, on a path where no P5 key is announced at all. Forced by the frozen 16-key record. **E7-dual item** |
| **[R2-P3-3]** | P3 | `prepare_roaming_target`'s cost claim "two `symlink_metadata` calls" understates it — `leaf_is_resident` is a full bounded observation (stat + no-follow open + identity re-check + read). **Landing-optional wording** |

No new P0/P1/P2. No regression found in the +712/−121.

---

## R2.1 [P2-1] re-verified — against the code and by execution

### The cure, traced

`platform::prepare_roaming_target` (`platform.rs:481`) surveys **both** names and
answers create-or-resume; `barrier_mutation::converge_target_anchor_alias` is its
owner-side entry; `ActionNamespace::converge_target_anchor_alias` its consumer
seam. `drive_ordinal`'s branch is now:

```rust
let entry = match namespace.converge_target_anchor_alias(&slots)? {
    TargetAnchorAliasStateV1::Stranded => AliasRetirementEntryV1::Stranded,
    TargetAnchorAliasStateV1::Absent => {
        namespace.create_target_anchor_alias(&slots)?;
        namespace.barrier_target_parent(&slots, &bound)?;
        AliasRetirementEntryV1::OwnDrive
    }
};
```

The old `scheduled_row_is_resident(&reserved)` test is gone from the alias phase.
**The wrong question is no longer askable**: `TargetAnchorAliasStateV1` is the
only thing the seam returns, and it is produced only by the owner of both names.
`round_trip_supplied` calls `prepare_roaming_target` too, so a caller entering
the arm directly is equally covered — verified in the diff and by the surviving
`an_outbound_roaming_alias_is_returned_when_the_arm_is_entered_directly` row.

### Is the enumeration total?

Yes, and structurally so. The match is on `(alias_resident, outbound_resident)`:

```rust
(false, false) => Absent
(true,  _)     => Resident
(false, true)  => { verify → publish_verified_leaf_no_replace → verify } Resident
```

Three arms over a `(bool, bool)` domain with a wildcard covering the two
`true`-first cases: **exhaustive by the compiler**, not by inspection. There is
no `_ =>` catch-all hiding an unconsidered state, and no `Option`/`Result`
smuggling a fifth outcome.

### "Both renames are atomic, so every crash lands in row two or three" — TRUE

I checked the actual sequence rather than the claim.

**The round trip's order of operations** (`round_trip_supplied`, post-remediation):

```
prepare_roaming_target(…)            ← converge first
guard: alias resident && !outbound
verify_leaf_bytes(alias)
publish_bytes(alias → roundtrip)     ← rename #1
verify_leaf_bytes(roundtrip)
publish_bytes(roundtrip → alias)     ← rename #2
verify_leaf_bytes(alias)
identity equality across the trip
```

So the reachable name-states are: before #1 → *(resident, absent)* = row two;
between #1 and #2 → *(absent, resident)* = row three; after #2 → *(resident,
absent)* = row two. **Rows one and four are never produced by this sequence.**

**Both renames are genuinely atomic.** Each is
`publish_verified_leaf_no_replace` → `rename_open_source` →, on Windows,
a single `SetFileInformationByHandle(FILE_RENAME_INFO, ReplaceIfExists=false)`
(`platform.rs:105-150`); off Windows a `rename_relative`. One kernel rename: at
every instant exactly one of the two names exists. There is no copy-then-unlink
anywhere on the path — I grepped the whole new roaming region for
`remove_file`/`remove_dir`/`unlink` and it is **empty**. So no crash can land in
row one (neither) or row four (both). The claim holds.

**And the converge itself is crash-safe under the same property.** Row three's
return is one atomic rename: a crash during it leaves either row three again
(the next drive converges again) or row two (settled). Idempotent, self-
converging.

### Does the seeded state match what a real crash leaves?

| | real crash between the two renames | what `assert_mid_round_trip_residue_converges` seeds |
| --- | --- | --- |
| reserved leaf | absent | absent (`prepared()` strands **ordinal 1** at `AuthorityScratch`; ordinal 0's `GoalScratch` is untouched) |
| `<leaf>.roundtrip` | resident | resident, via `place_bytes(…, &outbound)` |
| which object carries the anchor bytes | the outbound one | the outbound one — `place_bytes` writes `ROAMING_ANCHOR_BYTES` |
| durable identity | the alias's original inode | a fresh inode |

**Faithful on names, bytes and which object carries them.** The identity
difference is immaterial by construction: DECISION B-5's stated cost is that the
intent binds **no** alias identity, so nothing on any path compares it — the two
alias reobservations are residency-and-bytes proofs, which is exactly what round
1 verified. The seed reproduces every fact the code actually consumes.

The outbound name is derived in the fixture as `format!("{leaf}.roundtrip")`,
matching `platform::roundtrip_name`'s `push(ROUNDTRIP_SUFFIX)` with
`ROUNDTRIP_SUFFIX = ".roundtrip"`. Duplicating the derivation in the test rather
than importing it is right here — a test that imported the production
derivation could not catch the production derivation changing.

### Does anything persist after row three's return? Census-exact — yes, really

```rust
let rows = fixture.action_children(variant, action);
assert_eq!(rows, settled_census(action), "…did not settle to the ordinary census");
assert!(!rows.contains(&outbound), "…the outbound name survived the converge");
```

`action_children` is the **full sorted child list** of the action directory and
`settled_census` is exactly five names. So the first assertion alone is
census-exact — an orphan of any name would fail it. The second is a redundant,
better-messaged restatement. And the attempt is run **once**, unwrapped: if the
pre-remediation behaviour survived, that unwrap would panic on the refusal. **The
row is its own discriminator.**

### Row four's toleration does not mask a foreign-bytes state

The refusal still fires, in both the places it should:

- **Inside the arm** —
  `a_roaming_alias_resident_under_both_names_is_refused_inside_the_arm`
  (`platform/anchor/tests.rs`): places both names, calls `round_trip_supplied`,
  asserts the refusal and `names(&root.0).len() == 2` ("a refused barrier mutates
  nothing"). Body unchanged from round 1; only the name and doc moved. **Executed
  green.**
- **Foreign bytes under the reserved leaf** —
  `foreign_bytes_under_the_reserved_leaf_are_refused_before_the_edge`, unchanged
  and still green. I re-traced it against the new code: row two returns
  `Resident` without inspecting bytes, the guard passes, and then
  `verify_leaf_bytes(alias)` refuses with **"roaming anchor alias bytes are
  invalid"** — which still satisfies the test's `contains("bytes are invalid")`.
- **Foreign bytes under the outbound name in row three** — refused before the
  return, by `verify_leaf_bytes(&outbound, …)` *preceding* the publish. So an
  object under this protocol's own outbound name carrying something else is
  **never adopted**.
- **Foreign bytes at the reserved leaf on the drive path** — `require_alias_resident`
  on the retirement row refuses after the no-replace rename, preserving the
  object as evidence, which is B-4's own stated reason for retirement over
  removal.

**Row four does not verify either object's bytes**, and the disclosure does not
claim it does — it scopes the foreign-bytes refusal to "the third row", verbatim.
That scoping is precise and I checked it word by word. The principle underneath
is sound and worth stating: **verification is exactly where adoption happens.**
Row three adopts the outbound object (returns it and then lends it as the
anchor), so it must prove it. Row four never touches, reads or renames the
outbound object — it is inert. Verifying an object you will never use would be
ceremony, and refusing on it is the wedge §R2.2 rules out.

---

## R2.2 Grading the wedge-class argument for row four

**Ruling: not-refusing is SOUND. The argument holds, and the tolerated object
cannot be misread by a later ordinal.**

**(1) The wedge argument itself is correct on its own terms.** Row four is
reachable only on a tree a pre-remediation binary wrote. Refusing there would be
a *permanent typed refusal on a reachable state*, and the only convergence
available is a removal — which Step 4.2 deliberately replaced with durable
retirement, and for which there is exactly one `RetiredRoamingAnchorAlias`
retirement slot per ordinal, already spoken for by the alias itself. That is the
`Ambiguous`-dead-end class the whole E0.2b analysis is built to avoid ("a wrong
reading here bricks a user's catalog permanently, with no in-code exit"). Choosing
toleration over a wedge is the same call the corpus has made every time it has
faced this shape.

**(2) The reserved-leaf grammar interaction — I checked it, and it closes the
misreading question completely.** `<leaf>.roundtrip` **cannot** be a reserved
leaf and **cannot** be read as a slot:

- `ActionSlotV1::parse` requires `strip_suffix(PROTOCOL_VERSION_SUFFIX)` where
  `PROTOCOL_VERSION_SUFFIX = "-v1"` (`protocol/slots.rs:9`). A name ending
  `.roundtrip` fails that strip and returns `RecognizedInvalid(UnsupportedVersion)`
  — never `Valid(_)`.
- OPEN-B3's gate (`require_reserved_target_leaf`) admits **only** `Valid(_)`. So
  **no ordinal of any action can ever reserve a leaf equal to a `.roundtrip`
  name.** Collision is impossible by grammar, not by convention.
- Every other lookup in the family is on a derived, `Valid` name
  (`active`/`retired`/`retired_anchor_alias` from `action_destination`), so no
  slot lookup can resolve to it.

**(3) It cannot trip a bound or a predicate.** `observe_action_interior`
(`interior.rs:553-594`) counts an unparseable child as `extra_children += 1` and
refuses only on `seen > MAX_ACTION_SLOTS` (261) or the name budget — one extra
~35-byte child is far inside both. And the `extra_children: 0` gates are
admission-only, which round 1 verified independently (`protocol/admission/owner.rs:101-107`
and `publication.rs:204`'s `AdmissionStaging` directory-source recheck), neither
reachable from a barrier edge.

**(4) It is driven, not asserted.**
`a_legacy_both_names_tree_settles_with_a_tolerated_orphan_*` seeds both names,
asserts the ordinal settles, asserts the census is **exactly** the settled five
**plus** the orphan and nothing else, and then re-settles to assert a later drive
neither removes it nor trips over it. Both target variants. Executed green.

So the toleration is bounded, ungrowable on this tree, grammatically unreachable
as a slot, invisible to every predicate that could refuse, and behaviourally
pinned. **Sound.**

---

## R2.3 The disclosure rewrite — states exactly what the code does

I read the rewritten paragraphs against the code clause by clause. **No residual
false claim.** Specifically:

| Claim | Verified |
| --- | --- |
| "the converging caller is `prepare_roaming_target`, **not** this function" | ✓ — the converge block was removed from `round_trip_supplied` and replaced by a call |
| "One attempt was lost, and the outbound name was then left **permanently**" | ✓ — this is round 1's trace, restated correctly, including *why* (a settled ordinal is never barriered again) |
| "unreachable on this tree, because the entry decision runs before anything is created" | ✓ — `converge_target_anchor_alias` precedes `create_target_anchor_alias` in the only branch that creates |
| "a rename, never a removal, and nothing is left behind" | ✓ — `publish_verified_leaf_no_replace`; zero removal calls in the region |
| "exactly one thing persists, and only from the past: a `<leaf>.roundtrip` written by a **pre-remediation** binary" | ✓ — no post-remediation path produces it and survives |
| "it parses as no scheduled action slot" | ✓ — `RecognizedInvalid(UnsupportedVersion)`, §R2.2(2) |
| "this refusal … covers the case it is actually for: a foreign object appearing after that decision" | ✓ — the only way to reach the arm's both-names guard now |
| "Off Windows … the survey is two `symlink_metadata` calls" | **✗ imprecise — [R2-P3-3]** |

**The renamed tests are honest, and the renames are the right ones.**

- `an_outbound_roaming_alias_is_returned_by_the_next_barrier` →
  `..._when_the_arm_is_entered_directly`. The old name asserted the very thing
  round 1 found false; the new one is exactly what the body proves, and the doc
  now says the drive does *not* reach the arm in that state and points at the row
  that does. Body unchanged.
- `a_roaming_alias_resident_under_both_names_is_refused` →
  `..._is_refused_inside_the_arm`. The old doc's justification ("not a window
  this protocol can produce") is not merely deleted — it is **quoted, attributed
  to the review, and replaced** with the case the refusal is now for, plus a
  cross-reference to the drive-level row. Body unchanged.

That is the correct disposition for a falsified justification: fix it in place
with its provenance, rather than quietly restating.

---

## R2.4 The seven P3 dispositions, the two volunteered tightenings, and a regression audit

| Id | Taken? | Spot-check |
| --- | --- | --- |
| [P3-1] | **Yes, well** | `require_reserved_target_leaf`'s doc now says the gate is *action-scoped, not family-scoped*, names the exact hazard (base slots the authority record / payload writers / cleanup worklist own), names the fixture's own `GoalScratch`/`AuthorityScratch` as the demonstration, and **binds E4** to reserve a leaf no other family writes *and to say which it picked*. The matrix header carries the matching warning ("a real consumer must not copy the choice"). Stronger than I asked. |
| [P3-2] | **Yes** | "both `ExactInterior` call sites" → "the tree's **one** … site", with the miscount's provenance recorded beside it (inherited from E0.2 §3.3 rather than opened and read) and the substance re-verified. I re-confirmed: `grep -rn DirentBarrierClass` finds exactly one `ExactInterior` construction, at `namespace_mutation.rs:328`. |
| [P3-3] | **Yes** | `WINDOWS-ARM-OWED (R2-E E2)` sits beside `LINUX-COUNT-OWED` in `run_r4bg_aggregate_gates.py`, naming what discharges it by construction, why darwin cannot prove it, and that the Windows leg is the **first Windows compile of that code**. |
| [P3-4] | **Yes** | Label `"long-resident"` → `"aged-directory"`; the doc now says what it builds (one sibling written and unlinked), calls it a proxy, and adds "a true long-residency shape is not constructible in a unit test". |
| [P3-5] | **Yes** | The allow's reason string itself is edited to "entry-point reachability is **Phase E4, itself sequenced behind R2-F's relocation**", with a doc block re-anchoring the expiry and handing E7 a dated re-owning duty if E4 has not landed. Editing the machine-visible `reason=` string, not only the prose, is the right half to change. |
| [P3-6] | **Yes** | Recorded on `barrier_target_parent`, with the distinction I drew (the row dropped a clause about the **home** anchor, which does not travel; this proves the **alias**, which does) and one added justification of its own — it turns "a barrier that silently lost its lent object" into a typed refusal. |
| [P3-7] | **Yes, exceeded** | The loop becomes three named `#[test]`s — `a_resident_barrier_intent_is_refused_when_the_{catalog_anchor_identity,home_parent_identity,home_name}_disagrees` — **plus a positive control**, `a_resident_barrier_intent_binds_against_the_witness_the_owner_re_minted`. The control is the part I did not ask for and is the part that matters: it proves the three refusals are about *disagreement* and not about the witness being universally rejected. |

**Volunteered tightening 1 — the refused state named beside the converged ones.**
The disclosure now states the one state that is **not** a row of the table and
**does** block: foreign bytes under either name. Correct and correctly scoped
(§R2.1). The reasoning offered — *"no reachable crash window leaves a name a
later drive does not return" is only honest beside "and this one state blocks
until it is cleared"* — is exactly the standard round 1 applied to the original
paragraph. Good.

**Volunteered tightening 2 — `outbound_residue` dropped.** `RoamingTargetStateV1`
is now a two-variant enum with no unread field. I checked: nothing reads or
constructs a third state, and the toleration is pinned behaviourally by the
legacy row's census assertion, which is strictly stronger than a flag no caller
inspects. Removing it is right, and the stated reason ("an unread field on a new
seam is exactly the kind of drift this train was graded for") is the correct
reading of round 1.

### Regression audit of the +712/−121

**The `anchor.rs` → `platform.rs` moves are clean, and one of them is a
restoration.** Extracting each brace-balanced function from `94da3e5` and from
`HEAD` and hashing:

```
verify        IDENTICAL=True   (13 lines)   ← restored byte-for-byte to base
round_trip    IDENTICAL=True   (22 lines)
prepare       IDENTICAL=True   (51 lines)
establish     IDENTICAL=True   (36 lines)
survey        IDENTICAL=True   (78 lines)
publish       differs          (delegates to publish_bytes; behaviour-preserving)
```

**`verify`'s semantics did not change — they were put back.** Round 1's landing
had `verify` delegating to a generic `verify_bytes`; the remediation deletes
`verify_bytes` from `anchor` and restores `verify` to its own single-purpose
body, **byte-identical to base**, including both of its error strings
("private durability anchor bytes are invalid", "private durability anchor lacks
identity"). The resident anchor protocol's observable surface is now closer to
base than it was after round 1. The roaming arm gets its own
`platform::verify_leaf_bytes` with its own messages ("roaming anchor alias bytes
are invalid" / "… lacks identity"), so the two arms no longer share a message —
an improvement over round 1, and it keeps
`foreign_bytes_under_the_reserved_leaf_are_refused_before_the_edge`'s
`contains("bytes are invalid")` satisfied.

`anchor::is_resident` → `platform::leaf_is_resident`: body identical, moved.
`anchor::roundtrip_name` → `platform::roundtrip_name`: body identical, moved.
The move is **necessary**, not stylistic — `prepare_roaming_target` is portable
and `mod anchor` is `#[cfg(any(windows, test))]` (`platform.rs:11`), so a
non-test non-Windows build could not call a derivation that stayed in `anchor`.
The stated reason ("the residue has to be classifiable on every platform") is
accurate: `prepare_roaming_target` *is* that classifier and it runs everywhere.

**The byte-pinned callers are untouched.** `residue.rs`, `transition.rs`,
`cleanup.rs` and `namespace_mutation.rs` appear in **no** hunk of `9ba6e24`; all
eight existing `DirentBarrierClass` call sites remain textually unchanged; and
`PROTECTED_SOURCE_DIGESTS["checked_artifact/residue.rs"]` /
`["…/transition.rs"]` are not in the diff. The passing checker is the proof
against live bytes. `ROUNDTRIP_SUFFIX` has no consumer outside `platform.rs`, so
moving it broke no pinned reader.

**Full removed-line audit of `9ba6e24` — 117 non-header removed lines**: 2
digests, 7 driver docstring/marker lines, 5 import re-wraps, 20 the
`schedule_records` loop (→ four named tests), 1 `dead_code` reason string, 6
`drive_ordinal`'s leaf-only branch (→ the converge match), 9 the inline census
`vec` (→ the shared `settled_census()` helper), 4 the corrected `ExactInterior`
doc, 12 the false residual paragraph, ~19 the three moved `anchor` helpers, 8
`round_trip_supplied`'s old converge block, ~10 `anchor/tests.rs` renames and
helper re-pointing.

**Zero assertions deleted or weakened.** Every apparent removal is re-added
stronger: the census `assert_eq!(rows, want)` becomes
`assert_eq!(rows, settled_census(action))` over the identical five names, now
shared with two new rows; the three looped refusal assertions become three named
tests plus a control; the two renamed anchor tests keep their bodies verbatim.

---

## R2.5 The transient checker-suite failure — nailed down

**Authoritative evidence, run once by me on the frozen worktree tree at
`9ba6e24`:**

```
$ python3.13 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py
.....................................................................
----------------------------------------------------------------------
Ran 69 tests in 548.138s

OK
[exited with code 0]
```

**69/69 OK, exit 0, 9m08s.** The builder's `failures=3` does **not** reproduce on
the frozen tree. It was a mid-edit artifact, exactly as reported.

**What I can and cannot say about the three.** I cannot name them from a run I
did not observe, and I will not guess three specific names into the record. What
I *can* state from the suite's structure is the mechanism: a large fraction of
these 69 tests materialise the **live source tree** — copying it, hashing module
trees, and reclassifying the source inventory (e.g.
`test_current_source_inventory_is_classified` at `:77`, and the tree-digest
rejection tests at `:260`, `:284`, `:439`, `:458`, `:473`). Those are precisely
the tests that fail if a file changes underneath a 9-minute run, and they fail
*closed* — which is the design. A concurrent edit producing exactly three such
failures and none after freezing is fully consistent with that mechanism and
with nothing else in the suite.

**Conclusion: no finding.** The suite is green on the object being landed, which
is what the gate is for.

---

## R2.6 Round-2 gates — all green. Verbatim tails.

```
$ cargo fmt --all -- --check
(no output)
FMT_EXIT=0
```

```
$ cargo clippy --all-targets --all-features
    Checking gwz-core v0.11.0 (…/e2-worktree)
    Finished `dev` profile [unoptimized + debuginfo] target(s) in 14.18s
```

```
$ cargo test --lib -p gwz-core namespace::tests_barrier_matrix
test result: ok. 12 passed; 0 failed; 0 ignored; 0 measured; 1599 filtered out; finished in 8.39s
```

```
$ cargo test --lib -p gwz-core platform::anchor
test result: ok. 22 passed; 0 failed; 0 ignored; 0 measured; 1589 filtered out; finished in 0.09s
```

```
$ cargo test --lib -p gwz-core interface_tests::fault_expected_keys
test result: ok. 6 passed; 0 failed; 0 ignored; 0 measured; 1605 filtered out; finished in 0.03s
```

```
$ cargo test --lib -p gwz-core interface_tests::schedule_records
test result: ok. 11 passed; 0 failed; 0 ignored; 0 measured; 1600 filtered out; finished in 0.00s
```

```
$ cargo test --lib -p gwz-core checked_artifact::
test result: ok. 421 passed; 0 failed; 0 ignored; 0 measured; 1190 filtered out; finished in 43.28s
```

```
$ python3.13 scripts/checks/check_checked_artifact_boundaries.py
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
```

The four remediation rows, run by name:

```
$ cargo test --lib -p gwz-core -- a_mid_round_trip_roaming_residue_converges \
      a_legacy_both_names_tree_settles_with_a_tolerated_orphan \
      an_outbound_roaming_alias_is_returned_when_the_arm_is_entered_directly \
      a_roaming_alias_resident_under_both_names_is_refused_inside_the_arm --nocapture
running 6 tests
test …::a_roaming_alias_resident_under_both_names_is_refused_inside_the_arm ... ok
test …::an_outbound_roaming_alias_is_returned_when_the_arm_is_entered_directly ... ok
test …::a_mid_round_trip_roaming_residue_converges_on_a_workspace_target ... ok
test …::a_legacy_both_names_tree_settles_with_a_tolerated_orphan_on_a_workspace_target ... ok
test …::a_mid_round_trip_roaming_residue_converges_on_a_git_directory_target ... ok
test …::a_legacy_both_names_tree_settles_with_a_tolerated_orphan_on_a_git_directory_target ... ok

test result: ok. 6 passed; 0 failed; 0 ignored; 0 measured; 1605 filtered out; finished in 0.42s
```

**The 421 count reconciles.** Round 1 measured 413; the remediation adds eight
rows — four in `tests_barrier_matrix` (8 → 12) and four in `schedule_records`
(7 → 11) — and 413 + 8 = **421 measured**, matching the driver's darwin pin
exactly. The linux 431 stays DERIVED and marked OWED. The driver additionally
states, correctly, that both numbers are base-relative and must be **re-measured
at the rebase**, not added up on paper.

---

## R2.7 New findings

**[R2-P3-1] — "the ordinal settled" does not imply "its physical barrier ran".
E7-dual item + an E4 scope clause.**

After any crash in the alias window, the restart takes `Stranded`, which retires
the alias and never calls `barrier_target_parent` — so key #8 is never announced
for that ordinal and, on Windows, the dirent barrier never happens. Row three
sharpens this: a mid-round-trip crash is now *converged* — the drive proves the
returned object carries the frozen bytes and holds it at the reserved leaf — and
then retires it without barriering, even though at that moment it could barrier.

This is **charter-compliant**: §1.5's row #12 designs the restart path as "retire
the stranded alias", full stop, and the analogous property has existed since the
first landing. It is not a defect in this train. But the consequence is a
statement about the family's guarantee that no document currently makes: **a
settled barrier ordinal does not prove its target parent's dirents were ever
ordered.** E4's first consumer must not assume otherwise, and the E7 dual should
decide whether the family's contract says so explicitly or whether the restart
path should re-barrier a converged alias.

**[R2-P3-2] — row three's return rename is a durable edge that no fault key
announces, on a path where no P5 key is announced at all. E7-dual item.**

`converge_target_anchor_alias` performs a real, durable, no-replace rename from
inside `barrier_mutation.rs` — the file whose own header states *"every durable
barrier edge is announced from here"* — and announces nothing. The builder's
defence is the resident protocol's `AnchorState::NeedsReturn`, which also
announces no key. **The precedent is real but imperfect**: `NeedsReturn`'s return
happens *inside* `private_barrier`, under the umbrella of the key that names
"`private_barrier` has returned"; this one runs *before* the barrier and, on the
`Stranded` path, on a drive where `private_barrier` is never called at all — so
no P5 key covers it, even loosely.

Mitigations I verified, which are why this is P3 and not higher: the rename is
atomic (§R2.1) and idempotent, so an interruption row would assert exactly what
the seeded-state row already asserts; the resulting state is census-pinned on
both variants; `grep -c "fault_v1::hit"` is **0** in both `anchor.rs` and
`platform.rs`, so this is the module's uniform convention rather than an
exception carved for this edge; and minting a seventeenth `barrier.*` key is
outside E2's authorization — the frozen §3.5 record is sixteen and the census is
165. The builder could not have fixed this without moving a frozen count.

**Owed at E7:** either the §3.5 barrier record gains a key for P5's roaming
recovery rename at a later phase, or it states in terms that the roaming arm's
recovery rename is a named, deliberate exception to "every durable barrier edge
is announced".

**[R2-P3-3] — the cost claim understates the survey. Landing-optional.**

`prepare_roaming_target`'s doc says: *"Off Windows nothing renames the alias, so
the survey is two `symlink_metadata` calls that always answer the first two
rows."* `leaf_is_resident` calls `observe_leaf_exact`
(`observation.rs:193-275`), which does `symlink_metadata`, then a no-follow
open, then an opened-vs-named identity comparison, then `read_to_end`, then a
re-`symlink_metadata` and a five-way consistency check. For a 22-byte alias the
real cost is still negligible and the *conclusion* ("always answer the first two
rows") is correct — but this is a cost claim written in the same commit that
fixed a doc-accuracy P2, and the round's own standard is that every claim is
opened and read. One-word fix: "two bounded leaf observations".

*(Inherited, not a finding against this train: `observe_leaf_exact`'s
`read_to_end` is unbounded. That is pre-existing and shared with `transition.rs`,
`classification.rs` and the resident anchor protocol, which use it on the same
class of names; the target parent here is an owner-private action directory. I
record it only so a future reader does not attribute it to E2.)*

---

## R2.8 Verdict, and what the landing owes

**GO. Land it.**

[P2-1] is closed: the defect is cured at the only place it could be cured — the
entry decision — the fix is total over the state space and atomic under crash,
the disclosure now says what the code does, the falsified test justification is
corrected with its provenance, and both the converged and the legacy states are
**driven on both target variants**. All seven P3s are taken, two of them beyond
what I asked, and two further tightenings were volunteered and are right. No
regression in the +712/−121; the resident anchor protocol is closer to base than
it was after round 1. Every gate is green, and the checker suite is 69/69 OK on
the frozen tree.

### Landing conditions — the lane owner owes these at the squash

1. **Re-measure, do not add up.** `421` (darwin `checked_artifact::`) and `431`
   (linux, derived) are relative to base `94da3e5`. E1 and E3 have landed on
   `1d50e59` and both add lib tests under that partition, so **both values are
   wrong on the tip**. The driver says so; the landing must execute the two
   partitions on the rebased tip rather than arithmetic.
2. **Three predictable rebase conflicts, all of which must be resolved by
   *adding*, never by taking a side.**
   - `FAULT_FAMILY_ACTIVATION`'s executed-family literal
     (`fault_expected_keys.rs:800-815`) is currently seven entries including
     `"barrier"`; E1 adds `"cleanup"` and E3 adds `"terminal"`. The landed
     literal must be **nine**.
   - `run_r4bg_aggregate_gates.py`'s two `_fault_count` pins and their docstring
     — three steps edit the same lines.
   - `PROTECTED_SOURCE_TREE_DIGESTS["checked_artifact/capability/pre_catalog.rs"]`
     — E1, E2 and E3 all trip this tree (E0.2b §6.2(b)), so the digest must be
     **re-executed** post-rebase, never merged.
   `FAULT_INJECTION_SOURCES` and its `declared.len() == 10` pin do **not** move:
   §6.1's routing puts E1's and E3's sites in already-declared files, so ten
   stays ten across the rebase. I checked this rather than assuming it.
3. **The freeze §3.5 inventory-addendum edit, "nine" → "ten"** — still owed, in
   gwz-dev, correctly not attempted from the gwz-core worktree.
4. **`LINUX-COUNT-OWED` and `WINDOWS-ARM-OWED` discharged at the three-platform
   dispatch.** The Windows leg is the first Windows compile of
   `round_trip_supplied`, `prepare_roaming_target` and the third
   `DirentBarrierClass`; treat a Windows failure there as expected work, not as
   a surprise.
5. **The B-4 grounds correction** to `GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md`
   §1.5, per round 1 §4.3 — one dated line, plus OPEN-B8 closed *answered
   positive on the conjunction, durable half struck*.

### E7-dual items — not landing blockers

- **[R2-P3-1]** the family's contract on "settled ⇒ barriered", and whether the
  restart path should re-barrier a converged alias. Carries an **E4 scope
  clause**: E4's consumer must not rely on the implication.
- **[R2-P3-2]** the unannounced P5 roaming recovery rename — a key at a later
  phase, or a named exception written into the §3.5 record.
- **[R2-P3-3]** the one-word cost-claim fix, landing-optional; fold it into
  whichever commit next touches `platform.rs` if it is not worth a rebase.

### What this round did not do

It did not run the Windows or Linux legs; it did not review E1 or E3 as landed
on `1d50e59`, and therefore did not verify this branch **against that tip** —
only against its own base. It edited no tracked file in the object and ran no git
operation.

---

*Round 2 filed 2026-08-27 by the R2-E Phase E2 interior reviewer. Object:
`e2-barrier` @ `9ba6e24` over base `94da3e5`. Round 1 above is left as written.*
