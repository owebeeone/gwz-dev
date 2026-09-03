# R4b-P fourth remediation third state-machine confirmation

Date: 2026-08-12

Reviewed in full:

- `GwzM5-8R4bP1P2-RemPlan-4.md` at the second-amended design checkpoint;
- `GwzM5-8R4bLinuxCapabilitySpike.md`; and
- every open item and required amendment in
  `GwzM5-8R4bP1P2-RemPlan-4-ReviewState-2.md`.

This is a design confirmation. I did not read or coordinate with the filesystem
reviewer, and I changed no production or test source.

## Verdict

**NO-GO for R1.**

Every P2/P3 reported in `ReviewState-2` is now closed in design: Linux uses the
retained-descriptor `AT_EMPTY_PATH` query; checkout authority is split between
index and worktree domains; C7 has independent stage alphabets rather than an
inherited 20-cell claim; `WorkspaceRuntimeBootstrapV1` is literal,
capability-neutral, and live-process-only; capability/collision checks precede
private-catalog mutation; terminal cursor wording is correct in both
orientations; and prefix issuance is owned by a prefix-specific private issuer.

The newly added deterministic capacity architecture is not yet restart-closed.
Two P2 defects remain in that interface. The capacity reservation is stored
inside the action directory it is said to create, with no durable admission
owner for the intervening directory-creation window. Separately, the literal
16-slot table cannot represent the repeated immutable-successor and
write-through-retirement history promised for barriers and multi-component
bootstrap. That second defect is compounded by inconsistent placement of
intents and by the pre-capacity catalog-bootstrap record having no named
retirement slot.

## ReviewState-2 closure

| Prior finding | Current result |
| --- | --- |
| P2-1 retained-handle Linux tuple | **Closed.** Both documents freeze `name_to_handle_at(retained_fd, "", ..., AT_EMPTY_PATH)`, forbid pathname fallback and incompatible flags, and require a same-invocation substitution test plus the remount probe. |
| P2-2 inseparable index/worktree status | **Closed.** `CheckoutPathFact` now has distinct index and worktree values/status, and overlays are `Index(path)` or `Worktree(path)` with exact side-specific replacement. The selected-root positive and independent-drift tests are named. |
| P2-3 inherited five-by-four C7 audit | **Closed.** Section 3.10 expressly removes the 20-cell/five-by-four claim and declares service, binding, invocation, proof-only, action-free, and platform domains independently. |
| P2-4 undefined/circular runtime bootstrap | **Closed.** `WorkspaceRuntimeBootstrapV1` has a fixed four-name grammar, live/crash/concurrency rows, no durability claim, retained no-follow identities, and no dependency on checked identity, catalog, namespace barrier, or Windows anchor. Ordinary mutators remain capability-neutral. |
| P2-5 catalog before collision guard | **Closed.** The frozen order is lease, read-only capability/path and lossless Git collision scan, no-mutation rejection, then catalog bootstrap and capacity. R2 installs that gate before private mutation. |
| P3-1 terminal cursor orientation | **Closed.** The cursor is now explicitly before the suffix in forward order and after the prefix in reverse order. |
| P3-2 generic issuer privacy wording | **Closed.** The plan names a private `RollbackPrefixIssuer`, preserves legitimate generic `AuthorityIssuer` uses, and separates the compile-fail privacy gate from the semantic executor call-graph gate. |

The selected-root ownership phases remain coherent after the status split:
pending physical `After` gives evidence only lock/marker worktree facts, retains
all index authority with the participant, and activates root-metadata ownership
only at the durable participant successor.

## New findings

### [P2-1] Capacity admission has no durable owner for creation of the directory containing its reservation

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:132-183`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:205-218`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:230-237`; and
- `GwzM5-8R4bP1P2-RemPlan-4.md:976-980`.

**Violated contract.** Capacity must be durably reserved before private
mutation, every GWZ-created transient must have a named recovery owner, and an
admitted action must remain recoverable without scratch or retired-object GC.

**Evidence.** The only pre-reservation object is one global admission
**scratch**, which the plan explicitly treats as inert. The reservation is then
published into a per-action reservation slot, but that slot is inside the
deterministic active action directory that the reservation itself is said to
create. Those operations cannot occur in that order:

1. publishing the reservation first is impossible because its parent action
   directory does not exist; or
2. creating the action directory first mutates the catalog before a durable
   reservation exists.

In the second order, a crash after directory creation and before reservation
publication leaves an empty or partial active directory. The catalog grammar
classifies orphan active residue as global ambiguity. The global scratch cannot
authorize adoption because unpublished scratch is explicitly non-authoritative
and is reused after a pre-publication crash. No atomic filesystem primitive
creates a populated directory and its child reservation record as one
namespace edge.

This is not solved by deterministic naming or arithmetic reservation: both
describe what should exist after admission but do not provide a durable owner
for the physical handoff into that state.

**Required correction.** Add a closed, catalog-infrastructure-owned admission
protocol before the per-action directory. For example:

1. permanently reserve distinct fixed global scratch and active
   `ActionDirectoryAdmissionV1` slots;
2. encode/flush scratch, publish the active admission record through the
   catalog anchor, and bind the capacity arithmetic, action identity,
   deterministic directory/staging names, and final reservation payload;
3. create and durably publish or recover the deterministic action directory
   only under that active record;
4. publish and prove the resident `ActionCapacityReservationV1`;
5. durably retire the global admission record only after the resident record
   owns the exact directory; and
6. forbid the next admission until every scratch/active/directory/resident
   combination in this handoff has been recovered or rejected.

The exact implementation need not use those names, but it must provide the two
durable ownership states. Add the admission record, its capacity, its
publication/retirement destinations, every fault boundary, and repeated-crash
rows to the literal grammar and C7 platform alphabet. An exact final directory
may be adopted only through the matching durable admission record; an orphan
remains ambiguity.

### [P2-2] The fixed slot table cannot fund the intent/successor/retirement history it promises

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:132-145`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:158-211`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:379-383`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:403-427`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:435-445`; and
- `GwzM5-8R4bP1P2-RemPlan-4.md:505-537`.

**Violated contract.** Admission must reserve the worst-case slots for the
complete action, and failure to garbage-collect retired state must not consume
a slot required for that admitted action to reach terminal recovery.

**Evidence.** The plan promises capacity for every possible barrier/bootstrap
intent successor and retirement, but the literal `MAX_ACTION_SLOTS = 16` table
contains only one `BarrierIntent`, one `BootstrapIntent`, and one
`IntentSuccessorScratch`. It contains no retired-intent slot. A managed-parent
bootstrap can publish multiple immutable successor intents while descending
multiple components. One checked action can also invoke the roaming barrier
protocol repeatedly. Each Windows intent is durably retired with no-replace
rename rather than correctness-critical deletion.

After the first successor or retired intent occupies the only apparent slot,
the next one has no destination. Reusing it requires deletion, overwrite, or a
new unreserved name: deletion may fail and is expressly non-critical;
replacement is not part of the frozen no-replace protocol; and a fresh name
violates deterministic bounded capacity. The same issue applies when several
managed-component ownership markers must be retired through one
`RetiredBootstrapMarker` slot.

The namespace location is also contradictory. The root grammar says active
internal intents live in the scanned retained private root, while the
per-action `ActionSlotKind` table places `BarrierIntent` and `BootstrapIntent`
inside the action directory. Root-entry admission, action-slot admission,
enumeration, privacy, and retirement arithmetic differ depending on which is
normative.

Finally, `CatalogBootstrapV1` retires its pre-catalog record *into* the catalog
before ordinary action capacity is admitted. The exhaustive infrastructure
grammar names no fixed catalog-bootstrap retirement destination; the only
bootstrap retirement slot is per action, but no action directory/reservation
exists at that point.

**Failure scenario.** Admit an action requiring a multi-component managed
parent and more than one Windows namespace barrier. Crash after the first
intent successor or retirement, leave its retired destination present, then
resume to the next component/barrier. The next no-replace retirement collides.
If recovery deletes or overwrites the old entry, an injected deletion failure
strands an action whose reservation claimed to include all cleanup capacity.

**Required correction.** Freeze one physical layout and derive its literal
capacity from the bounded transition schedule:

- state whether action-bound intents are direct root entries or action-directory
  slots, and make the catalog grammar, budgets, scanner, privacy boundary, and
  reservation arithmetic agree;
- reserve a distinct deterministic generation/successor/retirement slot for
  every intent and marker instance that a complete action can produce, including
  the maximum declared managed-parent component count and barrier count; or
- specify a reviewed bounded rotation/compaction primitive whose durable
  replacement and restart proof permits slot reuse without depending on
  deletion or best-effort GC;
- add a fixed infrastructure retirement destination for the exceptional
  `CatalogBootstrapV1` record; and
- generate `MAX_ACTION_SLOTS` and all aggregate budgets from that maximum
  schedule, then test exhaustion with every old retirement destination still
  present.

The existing enum equality test is useful only after the enum can represent
the promised physical history.

### [P3-1] The claimed literal budget still omits several literal bounds

**Where:** `GwzM5-8R4bP1P2-RemPlan-4.md:127-145`,
`GwzM5-8R4bP1P2-RemPlan-4.md:158-203`, and
`GwzM5-8R4bP1P2-RemPlan-4.md:284-304`.

The catalog says every entry consumes a metadata budget and the retired child
has separate count/name/metadata budgets. The “literal and versioned” table
does not name aggregate retired-name or metadata limits. Section 3.4 gives a
16 KiB authority bound but no literal decoder bounds for capacity,
barrier-intent, managed-bootstrap-intent, admission, anchor, or infrastructure
records. Freeze these constants and include them in generated equality and
oversize-before-allocation tests. Counts and the per-component 255-byte limit
make some aggregate name limits derivable, but the document should either state
that derivation as normative or name the constants rather than claiming an
unstated separate budget.

This is P3 because the architecture already requires bounded codecs and exact
equality tests; it is a specification-completeness issue once the two physical
capacity P2 findings are corrected.

### [P3-2] Payload placement uses two incompatible descriptions

**Where:** `GwzM5-8R4bP1P2-RemPlan-4.md:132-138`,
`GwzM5-8R4bP1P2-RemPlan-4.md:197-205`, and
`GwzM5-8R4bP1P2-RemPlan-4.md:295-302`.

The active action directory and `ActionSlotKind` include `SourcePayload` and
`GoalPayload`, while section 3.4 says recognized payloads “are not copied into
the catalog.” If “catalog” there means the in-memory decoded catalog model,
say that payload files remain in reserved action-directory slots but are
streamed and never materialized by catalog enumeration. If payloads are
physically external, remove the slot kinds and add their external location,
identity, capacity, retirement, and recovery ownership. The intended streaming
rule is clear enough that this is wording/layout precision rather than a
separate P2.

## Required amendments before GO

1. Add a durable, fixed-slot admission handoff that owns action-directory
   creation before the resident capacity reservation can own it.
2. Make intent location and capacity arithmetic consistent, and reserve or
   safely rotate every successor/retirement/marker slot needed by the complete
   bounded action without GC.
3. Give the exceptional catalog-bootstrap record its own fixed infrastructure
   retirement destination.
4. Freeze the remaining literal decoder/name/metadata bounds and clarify
   physical payload placement.

After those amendments, one focused capacity/restart confirmation should be
enough. The seven `ReviewState-2` issues do not need to be reopened unless the
capacity correction changes their interfaces.
