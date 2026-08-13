# R4b R1 interface remediation plan

Date: 2026-08-13

Status: **implemented and locally verified; R2 and R4b-G remain blocked until
the remediated tuple passes two independent settled-tree reviews**.

This plan consolidates the findings in:

- `GwzM5-8R4bR1Interface-ReviewFS.md`; and
- `GwzM5-8R4bR1Interface-ReviewState.md`.

The reviews agree that the first R1 checkpoint is not a safe parallel-work
boundary. The arithmetic and basic Taut encodings are sound, but authority can
still be assembled from unrelated facts, the namespace backend can be
bypassed, managed-parent execution is not schedule-bound, several restart
records are only names in a size table, and the fault vocabulary is not
independently frozen. This remediation replaces those conventions with sealed
capabilities and complete internal recovery records before any consumer or
platform implementation starts.

Public merge-v1 records and dispatch remain unchanged and disabled throughout
this work.

## 1. One root-bound pre-catalog authority

Replace separately composable filesystem-capability, collision, and
non-worktree proofs with one provider-owned pre-catalog transaction.

The transaction observes a retained root and returns an opaque
`PreCatalogPermitV1` containing, but not exposing for reconstruction:

- a closed supported filesystem profile;
- a `DurableObjectIdentityV1` whose variant matches that profile;
- an invocation identity and rename-domain fact;
- a fixed collision-domain version and digest;
- a retained no-follow path-walk capability;
- one `CanonicalPathIdentityV1` whose every component binds spelling, parent
  mode, durable parent identity, invocation identity, and rename domain;
- either the complete lossless workspace index/worktree scan result or the
  same-root non-worktree Git-directory classification; and
- one lease/freshness binding shared by all observations.

Only the pre-catalog owner constructs the permit. A catalog bootstrap receives
the retained capability and cannot accept ambient paths or independently
issued proofs. It revalidates the path chain immediately before mutation.

Required negative gates cover cross-root, stale-incarnation, profile/identity,
path-mode, rename-domain, collision-domain, workspace/Git-directory, and lease
substitution.

## 2. Complete Taut-owned durable record family

Extend `protocol/checked_artifact.taut.py` before adding semantic Rust types.
The internal v1 schema must define every record advertised by
`ProtocolRecordKindV1`:

- checked authority;
- action capacity reservation;
- action-directory admission;
- catalog-bootstrap record;
- infrastructure record;
- barrier intent;
- managed-parent bootstrap intent;
- ownership marker; and
- cleanup worklist.

Catalog-bootstrap and managed-parent records include the exact ownership,
identity, ordinal, cursor, deterministic slot, predecessor/successor, and
retained-parent fields required by the accepted restart tables. Record IDs and
digests are derived from all other binding fields. Initial, successor,
marker-retirement, and terminal relations are closed semantic operations, not
free-form field constructors.

Each record implements the same typed bounded canonical interface. Public raw
decode helpers are removed. Recovery obtains an untrusted decoded value only
through `limit + 1` reading, then produces a sealed bound value after exact
reservation, slot, identity, and predecessor checks. Canonical path identity
uses the same bounded entry point. Independent semantic vectors cover every
closed variant and both maximum schedule layouts in addition to the generated
Taut corpus.

## 3. Plan-derived schedule and admitted managed-parent execution

Managed-parent preflight returns an immutable `ManagedParentPlanV1`, not an
implementation-specific associated value. It contains canonical ordered rows
for the declared purposes, retained existing parents, missing suffixes,
component counts, and stable spec digests. Its digest is derived from the full
plan.

`ActionScheduleV1` is constructed from that plan and assigns all bootstrap,
component, generation, and marker ordinals. The capacity reservation binds the
plan digest and scheduled rows.

After durable admission, the managed-bootstrap owner compares the original
plan with the resident reservation and issues an opaque
`BoundManagedParentPlanV1`. Binding checks action, request/owner, reservation,
schedule, plan digest, purposes, missing suffixes, component counts, and every
assigned range. `execute` accepts only this bound value.

An empty, reordered, different, partial, cross-action, cross-owner,
cross-provider-instance, or stale plan rejects before mutation. Both maximum
layouts remain accepted.

## 4. Sealed admission and role-typed namespace capabilities

Admission observation becomes owner-private. Production siblings cannot
construct observation variants or call a free admission function. The bounded
catalog observer returns an opaque handoff observation and the admission owner
alone issues `AdmittedActionV1`. Synthetic constructors remain under
`cfg(test)`.

Catalog occupancy becomes a closed state:

- `Idle`;
- `PreparingWithoutFinal`; or
- `PreparingWithFinal`.

Neither Preparing form may admit a new action. Preparing recovery does not
double-charge the already-owned prospective credit.

The raw platform namespace backend moves into a private child module. Checked
consumers cannot obtain it, call it, select its binding type, or construct
success proofs. Provider modules receive owner-private issuer helpers so a real
non-test backend can be implemented without widening those constructors.

`ActionNamespace` owns all forwarding operations. It validates the admitted
action directory and resident reservation before each operation and derives
only role-specific opaque capabilities:

- publish destination;
- source/goal/authority cleanup retirement destination;
- barrier active/retired/target slots;
- bootstrap active/retired/scratch/staging/final slots; and
- bootstrap marker retirement slot.

The wrapper rejects omitted cleanup aliases and barrier/bootstrap/component
ordinals outside the resident schedule. No method accepts a raw
`ActionSlotV1`, raw leaf name, raw binding, or backend-specific ordinal.
`BarrierIntentV1` is constructed from the reserved barrier capability.

Compile/privacy and production-shaped backend tests prove that consumers
cannot call the backend or forge retained, reserved, published, retired,
durable, observed-admission, or admitted-action capabilities.

## 5. Closed catalog grammar and capacity surface

Root and action parsers return a closed classification:

```text
Valid(value) | RecognizedInvalid(reason) | Foreign
```

All fixed prefixes, hexadecimal widths, ordinal ranges, and slot roles are
validated by the owner. Recognized invalid protocol names are never treated as
foreign or inert. Constants and deterministic names are derived from their
enum/grammar owners rather than repeated literals.

All record-kind limits have an implementing semantic type. Bounds use the
shared constants. Capacity and directory admission are expressed through
closed states, so a caller cannot accidentally treat a partially recovered
action as idle.

## 6. Exact fault vocabulary

Replace axis-plus-free-string construction with one closed typed boundary key
space. It includes every distinct before/after durable edge for runtime
bootstrap, catalog bootstrap, admission, record publication, namespace,
durable leaf, barrier outbound/return, every managed-parent initial/successor/
marker-retirement step, cleanup, and terminal closure. Ordinals and diagnostic
labels remain instance data.

An independently authored fixture lists every stable key and compares exact set
equality and cardinality with production `all()`. Missing, extra, renamed, and
duplicate keys fail. Later implementation must map every key to a production
injection point before the executable restart gate is accepted.

## 7. Test-first implementation order

1. Commit the two review reports and this plan as review evidence only if a
   checkpoint is required; do not treat that as R1 acceptance.
2. Add failing tests for cross-root permits, profile/identity mismatch,
   intermediate-parent replacement, recognized-invalid grammar, forgeable
   admission, namespace bypass/slot roles, unbound managed plans, optional
   record binding, every missing record codec, and exact fault equality.
3. Extend the Taut schema, regenerate Rust/corpus, and implement the complete
   bounded semantic record layer.
4. Implement the unified pre-catalog permit and retained path-walk contract.
5. Implement plan-derived scheduling, sealed admission, role-typed namespace,
   and bound managed-parent execution.
6. Remove superseded constructors and optional/raw paths; run privacy and
   call-graph searches.
7. Run focused interface/protocol tests, full core tests, clippy, fmt, protocol
   regeneration, public-protocol/dispatcher quarantine checks, and LOC/cohesion
   audit.
8. Commit one new settled tuple with installed `gwz`.
9. Run two independent reviews of that exact tuple. R2 begins only after both
   report no P0/P1/P2 finding.

## 8. Acceptance conditions

R1 is accepted only when all of the following are true:

- no checked-artifact sibling can compose authority from unrelated observed
  facts;
- every persisted identity is compatible with its claimed support profile and
  every retained path component is identity/mode/rename-domain bound;
- every durable record in the accepted architecture is Taut-defined, bounded,
  canonical, reservation-bound, and independently vectored;
- managed-parent mutation is impossible without its exact resident scheduled
  plan;
- admitted action issuance and raw namespace operations are owner-private;
- all namespace destinations and ordinals are role-typed and schedule-derived;
- catalog grammar distinguishes valid, recognized-invalid, and foreign names;
- the fault key set is complete and independently equal-pinned;
- public merge-v1 protocol and disabled dispatcher behavior are unchanged;
- all local verification gates pass; and
- both independent settled-tree re-reviews report no P0/P1/P2.
