# R4b-P fourth remediation fourth state/authority confirmation

Date: 2026-08-12

Reviewed artifact:

- `GwzM5-8R4bP1P2-RemPlan-4.md` in full at SHA-256
  `54efc38dfc3f320393269975960770b0b87f9b6de6ca15689bc63467f95ed1b2`;
  and
- every finding and required correction in
  `GwzM5-8R4bP1P2-RemPlan-4-ReviewState-3.md`.

This was a read-only design review. I did not change production or test source.

## Verdict

**NO-GO for R1.**

The third amendment closes the central admission self-bootstrap defect. A
durable infrastructure-owned `Preparing` record now precedes directory
creation, owns staging/final alternatives through resident-reservation
publication, and hands ownership back to `Idle` only afterwards. Barrier and
single-bootstrap intent generations, roaming-anchor aliases, marker
retirements, catalog-bootstrap retention, record codecs, catalog budgets, and
physical payload placement are now literal rather than left to implementers.

Three P2 capacity/restart gaps remain. The generation arithmetic does not say
whether a complete action may execute more than one declared managed-parent
spec; if it may, 17 intent pairs and component-only marker ordinals are
insufficient. The infrastructure staging directory is not explicitly scanned
under the action-directory bounds, so its `exact` recovery form does not yet
exclude an unbounded/foreign child set. Finally, the single `CleanupCursor`
slot has no immutable or bounded-replacement transition even though cleanup is
multi-step and the plan forbids the only replacement primitive outside the
admission record.

No prior rollback, checkout, reconciliation, prefix-authority, Linux identity,
or runtime-bootstrap P0/P1/P2 was reopened.

## ReviewState-3 closure

| Prior finding | Current result |
| --- | --- |
| P2-1 admission directory owns its own reservation | **Closed.** Fixed infrastructure `Idle | Preparing`, a fixed scratch/staging name, durable replacement, exact recovery alternatives, resident-reservation proof, and the final `Preparing -> Idle` handoff give directory creation an earlier durable owner. |
| P2-2 fixed slots cannot fund intent history | **Partially closed.** Barriers have 64 indexed active/retired/anchor-alias triples; one managed bootstrap has 17 active/retired generation pairs and eight marker-retirement slots; catalog bootstrap has a permanent infrastructure retirement destination. The singular-bootstrap and cleanup gaps below prevent full closure. |
| P3-1 omitted literal bounds | **Closed for the named catalog and codec surfaces.** Root/action/retired entry, name, and metadata arithmetic and every protocol-record decoder limit are literal and equality-tested. The staging-directory applicability defect below is a classification omission, not a missing constant. |
| P3-2 payload placement | **Closed.** Source and goal payloads physically occupy action-directory slots, remain outside the in-memory catalog model, and are streamed against request length/hash rather than protocol-decoded or materialized. |

The admission record's fixed-slot replacement has the necessary abstract
before/after forms: old active plus missing/partial/exact scratch, or new active
plus missing scratch, with exact active identity reobservation. The
`Preparing` state precedes the first directory mutation and remains authoritative
until the final directory and resident reservation are exact. This closes the
previous unowned gap without making scratch authoritative.

## Findings

### [P2-1] The 17-generation bootstrap schedule assumes one invocation but the action contract does not

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:176-183`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:239-250`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:282-300`; and
- `GwzM5-8R4bP1P2-RemPlan-4.md:627-673`.

**Violated contract.** The resident capacity reservation must cover the complete
caller-derived physical schedule before any managed mutation, with stable
ordinals that cannot collide during recovery.

**Evidence.** The plan bounds a `ManagedParentSpec` to eight missing components,
but the declared spec table is plural: merge-store and preservation parent
sequences. It never freezes at most one `ManagedParentBootstrap` invocation per
action. The arithmetic nevertheless assumes exactly one initial generation:

```text
MAX_BOOTSTRAP_INTENT_GENERATIONS = 1 + 2n
```

For `k` separate bootstrap plans containing `N` total installed components,
the minimum is `k + 2N`, because every plan needs its own generation zero and
one successor per installation and marker retirement. The names are also only
`BootstrapIntentActive(g)`, `BootstrapIntentRetired(g)`, and
`RetiredBootstrapMarker(component_index)`. A second plan starting at generation
or component zero collides with the first plan's no-replace retired slots.

**Failure scenario.** One checked action needs a missing merge-store suffix and
a missing preservation suffix. Even if their total missing component count is
no more than eight, completing the first bootstrap occupies generation zero and
its component-indexed marker retirement. Starting the second either reuses
those occupied names, exceeds the `1 + 2N` generation schedule, or invents an
unreserved ordinal. Deletion is disabled by the accepted maximum-schedule test,
so retry cannot make those names reusable.

**Minimal architectural correction.** Choose and freeze one of:

1. `MAX_MANAGED_PARENT_BOOTSTRAPS_PER_ACTION = 1`, with a generated complete
   caller inventory and a pre-admission rejection proving no action can request
   two specs; or
2. a literal `K`-invocation schedule with a bound on both total invocations and
   total missing components, generation count `K + 2N`, and names indexed by
   `(bootstrap_ordinal, generation)` and `(bootstrap_ordinal,
   component_ordinal)`.

Update `MAX_ACTION_SLOTS`, name/metadata arithmetic, the capacity schedule
digest, planner rejection, and the deletion-disabled maximum test from the same
declared formula. Do not infer the singular rule from current call sites.

### [P2-2] Admission staging has no explicit bounded child grammar

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:138-160`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:185-229`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:252-270`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:310-343`; and
- `GwzM5-8R4bP1P2-RemPlan-4.md:405-412`.

**Violated contract.** Every private directory must be classified with bounded
entry/name/metadata work before an `exact` recovery form can own or publish it.
Malformed/foreign state must remain visible rather than ride inside a valid
handoff.

**Evidence.** The catalog enumerates the root and every recognized **active
action directory**. The admission staging directory is instead an
infrastructure entry. Recovery crosses `missing/exact/other staging` and the
resident reservation, but the document does not say that staging children are
enumerated under `MAX_ACTION_SLOTS`, `MAX_ACTION_NAME_BYTES`, and
`MAX_ACTION_METADATA_BYTES`. Root capacity counts the staging directory itself;
that does not bound its contents.

The intended step creates only the resident reservation before renaming staging
to the final action directory. An uncooperative or injected staging directory
can therefore contain that exact resident record plus arbitrarily many foreign
children. Looking up only the resident record incorrectly classifies it as
exact; enumerating all children without the action bound violates bounded
parsing. The ambiguity exists specifically before the name is recognized as an
active action directory.

**Minimal architectural correction.** State that from its first appearance the
admission staging directory is scanned with the same action entry/name/metadata
limits that will govern it after rename. Its only legal pre-publication child
forms are the missing/partial/exact resident reservation forms owned by the
matching `Preparing` record; every additional entry, wrong kind, over-limit
entry, or over-limit aggregate is `other`/global ambiguity. The same reserved
action-directory credit must cover staging and final rather than being counted
twice. Add limit-plus-one entry/name/metadata cases and `exact reservation plus
foreign child` to the admission table.

### [P2-3] `CleanupCursor` has no restart-safe update or immutable interpretation

**Where:**

- `GwzM5-8R4bP1P2-RemPlan-4.md:176-183`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:201-211`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:272-308`;
- `GwzM5-8R4bP1P2-RemPlan-4.md:327-341`; and
- `GwzM5-8R4bP1P2-RemPlan-4.md:994-1013`.

**Violated contract.** Every step in terminal cleanup must have a durable owner
and successor, and its admitted action must finish with deletion/GC disabled
using only pre-reserved names.

**Evidence.** Cleanup is part of the worst-case schedule and has one base
`CleanupCursor` slot, but no cursor scratch, active/retired generations, or
transition semantics are defined. Terminal cleanup can retire several source,
goal, authority, and other exact aliases before moving the action directory.
A progress cursor normally changes after each durable retirement.

The only bounded replacement primitive is explicitly restricted to the
infrastructure admission record. Action authority and intents use immutable
no-replace publication. Updating `CleanupCursor` in place risks a torn record;
retiring and republishing it has no reserved successor/retirement slots;
deleting and recreating it creates an unowned gap and contradicts the
deletion-disabled terminal test. Merely listing a cleanup fault boundary does
not select one of those protocols.

**Minimal architectural correction.** Define one of two closed models:

- make `CleanupCursor` an immutable worklist published once and state that
  progress is derived solely from exact source/retired-slot observations, so
  the cursor bytes never change; or
- declare a maximum cleanup-step count and allocate indexed active/retired
  cursor generations plus one scratch, using the same immutable-successor
  handoff proven for bootstrap intents.

In either model, bind every cleanup source/destination and ordinal in the
resident capacity schedule, add the physical before/after/restart table, and
include the resulting slots in generated arithmetic. A mutable single file is
not an acceptable implicit implementation.

### [P3-1] The frozen high-level order obscures the required pre-admission catalog scan

**Where:** `GwzM5-8R4bP1P2-RemPlan-4.md:104-113` and
`GwzM5-8R4bP1P2-RemPlan-4.md:132-183`.

Section 3.1 says to admit capacity and run/recover managed bootstrap before it
says to enumerate the catalog. Section 3.2 necessarily scans/classifies the
catalog before choosing a family or computing current entries and outstanding
reservations. Clarify that admission starts with a full bounded catalog scan,
global ambiguity check, infrastructure-intent recovery, and existing-family
selection. If step 6 is a second post-bootstrap observation before authority
issuance, call it a fresh re-enumeration. The catalog section makes the safe
interpretation available, so this is ordering precision rather than another
P2.

## Confirmed closed surfaces

- Retained-fd Linux identity still requires `AT_EMPTY_PATH`; no pathname or
  `f_fsid` fallback was reintroduced.
- `WorkspaceRuntimeBootstrapV1` remains a capability-neutral live lock and is
  not coupled to checked identity or catalog durability.
- Collision preflight covers the complete private prefix and exceptional
  bootstrap names before catalog creation.
- Action-bound intents now live consistently inside action directories; root
  infrastructure and retired-root budgets are separate.
- Every logical barrier has an indexed active/retired/anchor-alias triple and a
  stable retry ordinal.
- The exceptional catalog-bootstrap record has a fixed permanent
  infrastructure retirement destination.
- Literal codec, name, and metadata constants and streamed in-directory payload
  placement close both prior P3 findings.
- Split checkout authority, selected-root handoff, terminal order, fresh
  ambiguity precedence, stage-typed C7, and private rollback-prefix issuance
  remain unchanged and coherent.

## Required amendments before GO

1. Freeze one managed-bootstrap invocation per action or budget/index every
   invocation explicitly.
2. Apply the action-directory bounds and exact child grammar to admission
   staging before it becomes an active action directory.
3. Define cleanup as an immutable physical-state-derived worklist or reserve a
   bounded cursor-successor protocol.
4. Clarify the full pre-admission catalog scan and the post-bootstrap fresh
   enumeration in the frozen order.

One further focused confirmation should be sufficient if those amendments do
not change the already accepted identity, checkout, aggregate, or authority
interfaces.
