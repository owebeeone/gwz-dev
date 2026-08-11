# R4b-P fourth remediation filesystem-interface re-review 4

Date: 2026-08-12

Reviewed independently:

- fourth-amended `GwzM5-8R4bP1P2-RemPlan-4.md` at SHA-256
  `54efc38dfc3f320393269975960770b0b87f9b6de6ca15689bc63467f95ed1b2`;
- the open P1-1 and P2-1 findings in
  `GwzM5-8R4bP1P2-RemPlan-4-ReviewFS-3.md`; and
- the prior filesystem findings only as needed to check that this amendment
  did not reopen a P0/P1/P2 durability or restart gap.

I re-read the complete amended plan. I did not read or coordinate with the
state-machine reviewer. This was a read-only design review; I did not modify
production or test code.

## Verdict

**NO-GO for R1/R2 implementation.** There is no P0 finding. One P1 ownership/
capacity ambiguity and one P2 bounded-recovery omission remain.

The amendment closes the concrete ReviewFS-3 capacity failure for one
managed-parent bootstrap. Eight component-indexed marker retirements, seventeen
active/retired intent-generation pairs, sixty-four barrier active/retired/
anchor-alias triples, fixed reusable scratches, and terminal whole-directory
retirement require exactly the declared 247 action names. The root and retired
arithmetic also adds up: sixty-four action directories plus ten infrastructure
forms equals seventy-four root entries, while the retired root has sixty-four
child-directory names. The maximum-schedule fault test disables deletion/GC
and exercises every indexed edge.

The exceptional workspace bootstrap collision domain is now complete. The
final prefix plus all three `.gwz` bootstrap forms are scanned in exact,
prefix, index-only, conflict, skip-worktree, tracked, case-equivalent, and
platform-equivalent forms before creation. The active bootstrap record then
has one permanent, counted infrastructure destination inside the catalog, so
its correctness does not depend on deletion.

Two definitions remain ambiguous. First, the seventeen-generation equation is
valid only if one action admission owns at most one `ManagedParentSpec`; the
plan bounds each individual spec but never forbids a second spec in the same
action. Second, the fixed admission staging directory is classified as root
infrastructure, but its children are not explicitly enumerated under the same
247-entry/name/metadata bounds as the final action directory it becomes.

## Findings

### [P1-1] The seventeen-generation schedule does not bind one admission to one managed-parent spec

**Where:** RemPlan-4 lines 176-183, 236-300, and 627-673.

**Ambiguity.** The plan defines
`MAX_BOOTSTRAP_INTENT_GENERATIONS = 1 + 2 * 8 = 17` and proves that equation for
*a* `ManagedParentBootstrap` whose input is *a* `ManagedParentSpec`. It does not
state that an `ActionCapacityReservationV1`/action directory may own exactly
zero or one such spec, must terminally retire that bootstrap action directory
before another spec is admitted, or may not prepare both the merge-store and
preservation sequences named in the inventoried table.

If multiple specs are legal in one admitted action, the correct generation
count is at least:

```text
managed_bootstrap_plan_count + (2 * total_missing_components)
```

before accounting for any plan-bound marker ordinals. Two one-component specs
need six intent generations, not the five provided by `1 + 2 * n` for two total
components. More generally, eight total components spread across two or more
specs exceed the frozen seventeen pairs. Reusing a retired generation or marker
slot would make terminal progress depend on deletion/GC; allocating another
name would violate admission. The ambiguity can therefore strand an admitted
action after managed mutation, the same P1 class as ReviewFS-3.

**Minimal architectural correction.** Freeze one of these two contracts:

1. one admitted managed-bootstrap action binds exactly one
   `ManagedParentSpec`; its last intent and complete action directory reach
   terminal retirement before another spec receives a separate admission, and
   a checked-action authority is never added to that directory; or
2. add a literal maximum bootstrap-plan count, plan-indexed intent and marker
   families, and use
   `plan_count + 2 * total_components` in the generated action-slot/name/
   metadata arithmetic.

The caller-derived schedule equality and maximum fault table must include the
chosen plan-count dimension. Do not infer singular ownership merely from the
singular API parameter.

### [P2-1] Admission staging contents have no explicit action-directory bound

**Where:** RemPlan-4 lines 138-162, 176-234, 252-270, and 327-343.

**Gap.** The catalog promises to enumerate every recognized active action
directory under the 247-entry, 62,985-name-byte, and 63,232-metadata-byte
bounds. The global admission `.staging` directory is instead one infrastructure
entry at the root. `Preparing` owns and populates it before its no-replace
rename to an active action directory, and recovery classifies it as
missing/exact/other, but the plan never assigns its children to
`MAX_ACTION_SLOTS`, `MAX_ACTION_NAME_BYTES`, and
`MAX_ACTION_METADATA_BYTES`.

**Failure scenario.** Recovery finds a valid `Preparing` record and its named
staging directory after a crash. The directory contains the exact resident
reservation plus more than 247 foreign entries or excess aggregate name/
metadata bytes. The root catalog count sees only one infrastructure name. To
decide whether staging is exact or other, recovery must either traverse it
without the promised action bound, ignore extra contents and publish them into
an active action directory, or apply an unstated limit. None is a frozen
bounded-recovery contract.

**Minimal architectural correction.** State that admission staging is a
prospective action directory and is enumerated before decoding or adoption
under the exact same action entry/name/metadata limits and closed child grammar
as its final name. At the pre-publication stage its only valid populated child
is the exact resident `ActionCapacityReservationV1`; every other child or
budget overflow is `other`/foreign ambiguity. Creation, crash recovery, and
no-replace publication must preserve that bound. Add exact-limit and
limit-plus-one staging rows for entry count, aggregate name bytes, metadata
bytes, wrong kind, and an otherwise exact reservation plus one foreign child.

## ReviewFS-3 closure

| ReviewFS-3 item | Fourth-review result |
| --- | --- |
| Eight-component marker retirement | **Closed for one bootstrap plan.** Eight indexed marker destinations remain occupied safely with deletion disabled. |
| Immutable bootstrap successors | **Conditional NO-GO (P1-1).** Seventeen active/retired pairs are exact for one plan with eight components, but plan cardinality is not frozen. |
| Sixty-four barrier invocations | **Closed.** Every ordinal has an active intent, retired intent, and retired roaming-anchor-alias destination; retry reuses the same ordinal. |
| Persistent admission ownership | **Structurally closed, subject to P2-1.** `Preparing` durably precedes staging/final creation, the resident reservation takes ownership before `Idle`, and the fixed scratch is reused. Staging-child enumeration is not bounded yet. |
| Root/action/retired arithmetic | **Numerically correct for the listed grammar.** `13 + 3*64 + 2*17 + 8 = 247`, `64 + 10 = 74`, and the name/metadata products are correct. P1-1 determines whether the listed action grammar is complete. |
| Repeated-crash/no-GC proof | **Closed for the listed one-plan schedule.** It crosses all 64 barriers and eight components, fills non-reserved retired capacity, and requires constant cardinality without deletion. Add the chosen plan-count dimension. |
| Exceptional `.gwz` collision domain | **Closed.** The prefix and all three bootstrap forms are scanned before creation, including index-only and equivalent names. |
| Permanent catalog-bootstrap retirement | **Closed.** The active exceptional record moves to one fixed, counted catalog infrastructure destination and is not GC-dependent. |

## Prior-interface regression check

No earlier P0/P1/P2 was reopened outside the two findings above:

- deterministic scratch, reserved barrier aliases, action-directory terminal
  retirement, and persistent admission prevent fault-driven name growth;
- `WorkspaceRuntimeBootstrapV1` remains a fixed-name, capability-neutral,
  explicitly non-durable live-lock protocol with one final lease object;
- the Linux provider still obtains identity from the retained no-follow
  descriptor using empty-path `name_to_handle_at(..., AT_EMPTY_PATH)` and
  forbids pathname relookup;
- bounded protocol decoders, streamed payloads, exact Windows flush rights,
  write-through namespace edges, catalog/barrier intent, and roaming-anchor
  recovery remain frozen; and
- unsupported and collision decisions still precede private/catalog/managed
  mutation.

The Linux R0-L release-architecture evidence remains a mandatory execution
gate and is not a design finding in this review.

## Exit decision

Do not begin R1/R2 production edits. Freeze whether one action admission owns
one managed-parent spec or a bounded set and update the generated schedule
accordingly. Apply the action-directory entry/name/metadata limits to admission
staging contents before classification or adoption. Then repeat the focused
filesystem confirmation. No v1 wire change or merge lifecycle phase is needed
for either correction.
