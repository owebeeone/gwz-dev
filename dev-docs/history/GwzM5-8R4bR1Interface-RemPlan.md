# R4b R1 interface remediation plan

Date: 2026-08-13

Status: **complete and independently accepted at the exact settled tuple; R2
may proceed, while R4b-G remains gated by RemPlan-4 R2-R6**.

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

## 9. First-remediation review result

The first implementation was committed at this exact settled tuple:

- workspace root `07d3bc5ed9ce746fbedbe25ff48b217968ec6bf3`;
- `gwz-core` `1bc07c4b35d6d158aa8daf25f924acf438f20698`; and
- `gwz-cli` `3cca145c0b32410f250f640730ed7ca18f1da59f`.

The two independent reviews are recorded in:

- `GwzM5-8R4bR1Interface-ReviewFS-2.md`; and
- `GwzM5-8R4bR1Interface-ReviewState-2.md`.

Both returned **NO-GO**. They confirmed that the plan/schedule binding, closed
record family, catalog grammar, bounded decoding, admission privacy, raw
namespace-backend privacy, occupancy accounting, and exact fault vocabulary
were materially corrected. The remaining findings expose six related holes in
the ownership and recovery architecture rather than isolated implementation
mistakes.

## 10. Second-remediation architecture

### 10.1 One sealed pre-catalog transaction

The raw filesystem/collision provider becomes a sealed capability-owner
implementation detail. A checked-artifact sibling cannot implement it and
cannot call a permit-producing method.

The public-internal catalog entry point performs one indivisible logical
sequence:

1. observe capability, retained path, collision domain, root kind, and lease;
2. construct the opaque `PreCatalogPermitV1` inside the owner;
3. immediately revalidate the retained root and every path component; and
4. pass a non-constructible, lifetime-bound `RevalidatedPreCatalogPermitV1`
   directly to catalog bootstrap.

`CatalogBootstrapV1::recover_or_create` accepts only the revalidated value.
There is no API on which a caller can omit revalidation or insert an unrelated
operation between revalidation and the bootstrap call. A production-shaped
provider compile test proves future platform providers can implement the raw
seam without exposing it.

### 10.2 Authority issued from one opaque observation

`CheckedAuthorityRecordV1` no longer accepts a path, parent identity, source
fingerprint, expected digest, and goal digest as independently supplied
arguments. The authority owner obtains those facts from one sealed retained
observation/request transaction and returns an opaque
`CheckedAuthorityObservationV1`.

Record creation accepts only that observation plus the resident reservation.
Recovery binds the decoded record to both the reservation and the exact
observation, not merely to a common support profile. Synthetic observation
construction is test-only. This makes cross-root, cross-parent, cross-source,
and cross-request composition unavailable to production siblings.

### 10.3 Non-circular first-catalog ownership

The initial catalog-bootstrap record carries a nonzero, randomly issued
bootstrap ownership token. Every staging infrastructure record carries the
same token and bootstrap record ID. The exact staging/final classification is
therefore rooted in the already durable active bootstrap record, rather than
in a post-hoc tuple derived from whatever directory was observed.

Infrastructure construction is owner-controlled. The owner issues an opaque
observation only after verifying the ownership marker/token and the exact
staging directory identity, infrastructure identities, fixed names, profile,
and retained-parent binding. Recovery accepts `Exact` only for that bound
observation. A foreign or self-consistent but unowned directory is `Other` and
cannot become the expected value merely because it was observed after restart.

### 10.4 Complete role-typed namespace operations

`ActionNamespace` remains the sole consumer-facing namespace seam and gains
operations for every derived role:

- publish and retire a barrier intent;
- retire the exact roaming-anchor target alias;
- publish bootstrap scratch and active generations;
- retire the prior bootstrap generation;
- publish a managed staging directory to its final component; and
- retire the exact ownership marker to its scheduled marker slot.

Every method accepts the corresponding opaque `BarrierSlots`,
`BootstrapGenerationSlots`, or `BootstrapComponentSlots`, revalidates the
admitted action directory and resident reservation, validates source parent,
leaf, kind, provider, action, and ordinal bindings, and only then forwards to
the private backend. No indexed transition accepts a raw slot or leaf.

### 10.5 Exact managed-successor evidence

Each managed component row retains the installed directory identity, mode,
canonical path, and ownership-marker object identity once installation
completes. Installation evidence is issued only when:

- the prior retained path is an exact prefix;
- the appended component names the scheduled final leaf;
- the appended component's parent identity and mode equal the intent's exact
  retained parent identity and mode; and
- the ownership marker content and retained marker-object identity match the
  current intent/component.

Marker-retirement evidence carries and checks the retired marker identity plus
the exact installed parent identity, mode, and path stored for that component.
Successor creation rejects any same-profile substitution, changed case mode,
wrong parent, wrong marker identity, or merely suffix-matching path.

### 10.6 Independent semantic byte vectors

Add a hand-authored semantic-vector fixture that is outside Taut's generated
corpus and is never regenerated by `protocol/regen.py`. Literal canonical bytes
must decode through the bounded semantic adapters and re-encode byte-for-byte.
The fixture covers every durable record family, all closed enum variants, all
three durable-identity variants, both path modes, both root kinds, all managed
phases and purposes, every cleanup alias, and the `K=1,N=8` and `K=8,N=8`
maximum schedule layouts.

The test also proves the generated shape corpus is not being used as the
semantic fixture. Changing a semantic tag, binding, canonical encoding, closed
variant, or maximum layout requires an explicit reviewed fixture edit.

### 10.7 Literal ownership cleanup

Protocol limits and fixed catalog names are derived from their enum/grammar
owners. The remaining hard-coded cleanup row limit and duplicated bootstrap,
admission, and anchor names are removed while the affected modules are open.

## 11. Second-remediation test-first sequence

1. Commit the two `-2` review reports with the eventual remediation; do not
   mistake the reports themselves for an accepted checkpoint.
2. Add failing tests proving a caller cannot bootstrap from an unrevalidated
   permit, cannot construct an authority from raw unrelated facts, cannot
   adopt an unowned first-catalog directory, and can execute every scheduled
   namespace role only through `ActionNamespace`.
3. Add failing substitution tests for installed-parent identity/mode/path and
   retired marker/parent identity/mode/path.
4. Extend the Taut schema for bootstrap ownership and complete stored managed
   component evidence, regenerate, then update semantic adapters.
5. Implement the sealed owners, complete namespace forwarding seam, and exact
   transition checks; remove superseded constructors and conventions.
6. Add and exact-pin the independent semantic vectors and derive duplicated
   literals from their owners.
7. Run focused interface/protocol tests, the full core suite, clippy, fmt,
   regeneration, public-protocol/dispatcher quarantine, privacy/call-graph
   searches, and LOC/cohesion checks.
8. Commit one new exact tuple with installed `gwz` and run two fresh
   independent settled-tree reviews. R2 and R4b-G remain blocked until both
   report no P0/P1/P2.

## 12. Second-remediation local result

The implementation now provides:

- an owner-private pre-catalog provider with structurally adjacent immediate
  revalidation and a lifetime-bound bootstrap handoff;
- checked authority issued and recovered only from one opaque retained
  observation;
- non-circular first-catalog ownership through an active-record token and a
  later infrastructure record whose stored identities are checked against the
  physical marker and directory before an opaque exact observation is issued;
- all seven missing role-specific namespace forwarding operations;
- durable exact component identity, mode, path, and marker-object evidence for
  managed installation and retirement; and
- a non-regenerated 26-vector semantic-byte fixture covering every record
  family and closed variant required by section 10.6.

The lead-owned gates pass on the settled worktree:

- 1,247 full-core tests passed, one ignored, zero failed;
- 72 checked-artifact interface tests passed;
- 29 public protocol tests passed;
- strict all-target clippy, formatting, regeneration, and diff checks passed;
- the production v1 decoder remains disabled; and
- public `gwz` protocol/schema/conversion and merge-dispatch files are
  byte-identical to the first-remediation base tuple.

This is local evidence only. The exact commit tuple and both required review
verdicts must be recorded before the status can advance to accepted.

## 13. Second-remediation settled-tree review result

The second remediation was committed and reviewed at this exact tuple:

- workspace root `c51cae969a6549a6986750447b8a648e444aa709`;
- `gwz-core` `a2a7125fe6b69f09ca76d83caeb28b0684c3c295`; and
- `gwz-cli` `3cca145c0b32410f250f640730ed7ca18f1da59f`.

The independent reviews are recorded in:

- `GwzM5-8R4bR1Interface-ReviewFS-3.md`; and
- `GwzM5-8R4bR1Interface-ReviewState-3.md`.

Both returned **NO-GO** with no P0/P1 and the same three P2 conclusions. They
also agree that the pre-catalog transaction, seven indexed namespace roles,
exact managed-successor comparisons, independent semantic vectors, bounded
record codecs, public-wire quarantine, and prior literal cleanup are closed.
The remaining defects are all ownership-boundary defects:

1. catalog infrastructure evidence is not bound to staging versus final, and
   record creation can precede aggregate two-location classification;
2. managed install/marker-retirement mutations are disconnected from the
   opaque evidence their durable successors require; and
3. authority expected/goal hashes remain bare consumer inputs rather than
   facts from the sealed request/retained observation.

Both reviews also retain one nonblocking P3: the catalog/private-domain names
still have three independent literal owners.

## 14. Third-remediation architecture

### 14.1 Catalog owner returns one closed two-location observation

Replace the single caller-selected infrastructure observation with one
owner-private provider operation that observes both fixed catalog directory
names in the same transaction. The provider reports each name as missing,
partial expected content, an owned physical candidate, or other; a candidate
includes its actually observed leaf, retained parent/path, durable directory
identity, marker/token, infrastructure identities, and optional stored
infrastructure record.

The catalog owner validates both names and returns one opaque
`CatalogBootstrapRecoveryObservationV1`. Its staging and final exact values are
role-bound and non-interchangeable. The public-in-`checked_artifact` consumer
obtains only the closed recovery decision and any role-specific capability
needed by the selected action; it cannot arrange generic exact evidence into
separate staging/final parameters.

The owner classifies the complete scratch/active/staging/final/retired shape
before any write. A missing infrastructure record may be created only when the
closed classification has selected the exact staging prepare/rewrite state.
After the write, the owner reobserves both names and reclassifies; a write does
not itself fabricate exact evidence. Both-present, substituted-name,
wrong-role, final-partial, or otherwise unlisted states return ambiguity with
zero mutation.

### 14.2 Managed namespace operations own success and restart evidence

`ActionNamespace` remains the only consumer-facing physical seam. Its managed
component install and marker-retirement operations consume the exact bound
intent/slots and return `InstalledManagedComponentV1` and
`RetiredManagedMarkerV1` respectively, only after the private provider has
performed no-follow post-operation observation of the final directory,
ownership marker, canonical path, mode, and durable object identities.

The same owner exposes role-specific recovery observations for restart when
the physical mutation completed before its result was durably recorded. Those
operations consume the exact prior intent and scheduled slots; they do not
accept caller-supplied identity, mode, path, marker, action, or ordinal facts.
The backend issuer may assemble evidence only inside the private provider
subtree, and its constructors bind the provider, intent, reservation, and
scheduled role before returning the opaque result.

The production-shaped compile fixture must drive both forward mutation and
restart observation through these evidence-returning operations. Test helpers
may seed physical observations, but managed-record tests must consume the same
`ActionNamespace` result rather than manufacture successor evidence directly.

### 14.3 Authority provider owns the request binding and both hashes

The one private authority observation fact set gains the request-owner binding,
expected SHA-256, and goal SHA-256. `CheckedAuthorityObservationOwnerV1` accepts
only the resident reservation; it accepts no bare hashes. It rejects when the
sealed fact set's request-owner binding differs from the reservation before it
issues the opaque observation.

A production provider is constructed for one retained artifact and one
request, so root/path/source facts and request hashes are observed as one
transaction. Cross-request tests use two otherwise-compatible reservations and
prove that an owner for one request cannot issue against the other.

### 14.4 One catalog/private-domain name owner

Define the fixed catalog bootstrap scratch, active, staging, and final names in
the catalog grammar owner. Derive the bootstrap record fields, collision-domain
paths, and workspace/Git-directory policy paths from that owner. No consumer
retains a byte or path literal for those names.

## 15. Third-remediation execution and gate

1. Add failing role-substitution and zero-mutation catalog tests, then replace
   the single-directory owner with the aggregate owner and post-write
   reobservation.
2. Add failing managed-operation and restart evidence tests, then couple both
   managed mutations and both recovery observations to `ActionNamespace`.
3. Add the cross-request authority substitution test, then move request
   binding and hashes into the sealed provider fact set.
4. Replace the remaining catalog-name literals with the one grammar owner.
5. Run focused checked-artifact and protocol tests, full core tests, strict
   clippy, formatting, regeneration, public-protocol/dispatcher quarantine,
   privacy/call-graph searches, diff checks, and LOC/cohesion checks.
6. Commit one new exact tuple with installed `gwz` and repeat both independent
   settled-tree reviews. R2 and R4b-G remain blocked until both report no
   P0/P1/P2.

## 16. Third-remediation local result

The implementation now provides:

- one aggregate catalog owner that observes scratch, active, staging, final,
  and retired roles before mutation, returns distinct exact staging/final
  evidence, permits a missing staging infrastructure-record write only from
  the accepted prepare state, and reobserves the aggregate after that write;
- managed install and marker-retirement operations, plus their restart forms,
  that return only provider-, intent-, reservation-, schedule-, root-, and
  physical-observation-bound successor evidence through `ActionNamespace`;
- one authority observation transaction that owns the request binding and both
  hashes as well as the retained root/path/source facts; and
- one catalog/private-domain name owner used by the bootstrap record,
  collision domain, and workspace/Git-directory policy.

Lead-owned validation on the combined tree passed:

- `cargo test -p gwz-core`: 1,252 passed, one ignored, zero failed (1,207 unit
  plus 45 integration tests);
- `cargo test -p gwz-core checked_artifact::interface_tests`: 77 passed;
- `cargo test -p gwz-core --test protocol`: 29 passed;
- strict all-target Clippy, formatting, protocol regeneration, and diff checks;
- the production merge-v1 decoder rejection and public protocol/dispatcher
  byte-equivalence quarantine; and
- privacy/call-graph, catalog-literal ownership, and LOC/cohesion audits.

All changed production modules are below 500 LOC.

## 17. Acceptance result

The implementation was committed at workspace/core/CLI tuple
`7e4ccdf80c808367171b1df1a5210c89ac36ec68` /
`c239df33ddf567d4eaca182ca8231ed575e95769` /
`3cca145c0b32410f250f640730ed7ca18f1da59f`.

The independent filesystem/interface and state/protocol re-reviews are
recorded in `GwzM5-8R4bR1Interface-ReviewFS-4.md` and
`GwzM5-8R4bR1Interface-ReviewState-4.md`. Both report **GO** with no P0-P3
finding. The plan's R1 acceptance gate is closed, and R2 is unblocked.
RemPlan-4 R3-R6 still precede R4b-G. The production merge-v1 decoder and
dispatcher remain disabled.
