# R4b R1 checked-artifact state/protocol interface review

Date: 2026-08-13

## Reviewed settled tuple

- workspace root: `3fa8cdc8c2c299a7a8874cd3580d7379a383daf8`
- `gwz-core`: `081856e72bcee23f5900a7698f770721ae02545d`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

The three checked-out commits matched this tuple at review time. This review is
limited to the committed tree. It focuses on the R1 Taut/state/recovery
interfaces and does not treat the absence of the later platform and catalog
implementations as a defect by itself.

## Verdict

**NO-GO for R1 acceptance and for starting the parallel platform/catalog
implementation.**

There is no P0 or P1 finding. There are six P2 interface defects. They are
pre-activation defects rather than released regressions: the new modules are
private and unconsumed, and production merge-v1 dispatch remains disabled.
They nevertheless defeat the purpose of the lead-owned interface checkpoint:
parallel implementers would either rely on convention for authority/capacity
properties or would have to redesign shared contracts while implementing.

## Findings

### [P2-1] A managed-parent plan is not bound to the admitted schedule

**Evidence:**

- `gwz-core/src/checked_artifact/bootstrap.rs:200-213` gives
  `ManagedParentBootstrap::execute` an opaque implementation-owned `Plan` and
  an `AdmittedActionV1`, but provides no operation that binds the plan's
  purposes, missing components, bootstrap ordinals, generation ranges, or
  marker ordinals to that action's reservation.
- `gwz-core/src/checked_artifact/protocol/schedule.rs:99-116` permits any
  checked-artifact sibling to create a `ManagedBootstrapInputV1` from an
  arbitrary digest and count. It is not derived from `ManagedParentSpec` or
  from the provider-issued plan.
- The checkpoint test demonstrates the hole rather than rejecting it:
  `gwz-core/src/checked_artifact/interface_tests/contracts.rs:499-517`
  preflights all four declared purposes, constructs a reservation whose
  schedule has **zero** bootstrap inputs, and successfully executes that plan
  under the unrelated admitted action.

**Impact:** a future implementation can create managed parents while the
resident reservation contains no corresponding intent generations, marker
slots, or component ordinals. A crash can then require names and retirement
capacity that were never admitted. The `K + 2N` arithmetic is correct for the
schedule that was built, but the mutation interface does not require execution
to use that schedule.

**Recommendation:** make preflight produce a sealed, immutable plan descriptor
that is consumed by schedule construction. After admission, issue a second
sealed scheduled-plan capability only after exact comparison with the resident
reservation (action, owner, schedule digest, purpose/spec digests, component
counts, and assigned ordinal ranges). `execute` should accept only that
capability. Add negative tests for an empty, different, reordered, or partially
matching resident schedule and assert zero managed mutation.

### [P2-2] `AdmittedActionV1` is indirectly forgeable by any checked-artifact sibling

**Evidence:**

- Although `AdmittedActionV1` fields are private,
  `ActionDirectoryObservationV1` and its `Exact` variant are visible throughout
  `checked_artifact` (`protocol/admission.rs:335-354`).
- `ActionDirectoryAdmissionV1::idle` and `admit_observed_action` are also
  visible throughout that module (`protocol/admission.rs:128-130` and
  `436-448`). A sibling can therefore synthesize an idle record plus an exact
  final-directory observation and obtain the nominally opaque authority
  without performing catalog enumeration, bounded child inspection, or a
  durable observation.
- `interface_tests/contracts.rs:343-371` constructs the authority through
  exactly this public-in-module path. This is useful test setup, but it also
  proves that production siblings have the same issuance path.

**Impact:** the private constructor on `AdmittedActionV1` does not enforce the
checkpoint statement that opaque proof constructors do not cross ownership
boundaries. Consumer conversion could accidentally turn self-reported model
facts into mutation authority.

**Recommendation:** move observation construction and admission issuance
behind the catalog/admission owner. Expose an opaque observed-handoff value or
a sealed observer method, not a public enum with constructible variants. Put
synthetic constructors under `cfg(test)` test support. Add compile-fail/privacy
tests showing that a checked-artifact consumer cannot manufacture an admitted
action.

### [P2-3] `ActionNamespace` does not enforce the reservation-bound namespace seam

**Evidence:**

- `NamespaceProtocol` chooses arbitrary associated
  `ReservationBinding` and `BarrierOrdinal` types
  (`namespace.rs:282-326`). Nothing requires `RecordDigestV1` and
  `BarrierOrdinalV1` or connects them to an admitted action.
- The test implementation deliberately uses `u8` for both types and invokes
  the raw protocol directly (`interface_tests/contracts.rs:255-340`). This
  compiles and can issue `DurableNamespace` without an action schedule.
- `ActionNamespace` exposes the raw implementation through
  `implementation()`/`implementation_mut()` (`namespace.rs:205-215`) instead
  of owning forwarding methods that accept only its derived capabilities.
- Its slot methods accept any `ActionSlotV1` for either an ordinary destination
  or a retirement destination (`namespace.rs:233-273`). They do not restrict a
  barrier or bootstrap ordinal to the counts in the resident schedule, and
  `reserve_action_retirement_slot` can designate an active/payload slot as a
  retirement slot.
- `BarrierIntentV1::try_new` accepts an arbitrary `AsciiComponent` as
  `reserved_target_leaf` (`protocol/barrier.rs:33-59`) rather than a typed slot
  derived by `ActionNamespace`. Its hash proves consistency of the supplied
  value, not that the value was reserved.

**Impact:** callers can bypass the only-consumer-facing wrapper, select
unreserved ordinals or the wrong slot role, and obtain a durable namespace
proof from a backend whose binding types are unrelated to the capacity
reservation. This leaves the central authority and retry-name guarantee as a
caller convention.

**Recommendation:** keep the raw platform backend private beneath
`ActionNamespace`; remove consumer access to it. Have the wrapper own the only
publish/retire/barrier operations, with concrete reservation and ordinal types.
Use role-specific opaque slots (publish destination, each cleanup retirement
alias, barrier target, bootstrap generation/marker) derived only after checking
the resident schedule. Construct `BarrierIntentV1` from a reserved barrier
target capability rather than from a raw component.

### [P2-4] The frozen durable protocol and catalog grammar are incomplete

**Evidence:**

- `ProtocolRecordKindV1` advertises bootstrap intent, catalog bootstrap,
  infrastructure, and marker records (`protocol/codec.rs:6-17`), but
  `protocol/checked_artifact.taut.py` defines none of those records. There is
  consequently no semantic record, canonical codec, or reservation-binding
  interface for `ManagedParentBootstrapIntentV1`, `CatalogBootstrapV1`, the
  infrastructure records, or ownership markers.
- The controlling design explicitly requires the managed-bootstrap intent
  (`GwzM5-8R4bP1P2-RemPlan-4.md:703`) and catalog-bootstrap record
  (`GwzM5-8R4bP1P2-RemPlan-4.md:618`) and places their contracts in R1
  (`GwzM5-8R4bP1P2-RemPlan-4.md:1031-1046`).
- `RootEntryNameV1::parse` and `ActionSlotV1::parse` return `Option`
  (`protocol/slots.rs:164-172` and `189-203`). A recognized-but-invalid name
  and a genuinely foreign name both become `None`, contrary to the frozen
  grammar's category-specific distinction. A catalog implementation would
  need another parser or prefix heuristic to recover that distinction.

**Impact:** the platform-independent catalog, managed-bootstrap, and first-
catalog agents cannot implement against a frozen shared wire/recovery
contract. Independent implementations would either create a shadow protocol
or modify this checkpoint under one another. The catalog also cannot use the
provided parser to report the required recognized-invalid versus foreign
ambiguity reason.

**Recommendation:** complete the internal Taut schema and semantic adapters for
every advertised durable record before parallel implementation. Bind each
intent/marker to action, owner, schedule, assigned ordinals, retained identity,
and deterministic slot as specified by the plan. Replace `Option` parsing with
a closed classification such as `Valid(T) | RecognizedInvalid(reason) |
Foreign`, and pin every grammar category in independent tests.

### [P2-5] Bounds and reservation trust are optional at the record API

**Evidence:**

- `CanonicalPathIdentityV1` is advertised as a 4 KiB protocol record, but it
  does not implement `BoundedCanonicalRecordV1`. Its only decoder first parses
  the entire CBOR value and allocates the generated component vector, then
  checks the re-encoded size (`capability.rs:119-159`). Thus the documented
  `limit + 1`-before-parse rule is unavailable for this record kind.
- The same module-visible `decode_canonical(&[u8])` pattern is exposed on
  capacity, admission, barrier, and cleanup records. The bounded reader exists,
  but the type system does not stop catalog/recovery code from calling the raw
  semantic decoder on bytes read by another path.
- `CleanupWorklistV1::decode_canonical` returns an untrusted worklist without a
  reservation (`protocol/cleanup.rs:225-257`); reservation validation is a
  separate optional boolean call (`205-217`), and physical classification
  accepts a bare row (`307-324`). `BarrierIntentV1::decode_canonical` similarly
  validates its self-derived hash but does not return a reservation-bound
  recovery capability (`protocol/barrier.rs:105-147`).

**Impact:** the two security properties that matter after restart—bounded
parsing and binding the recovered record to the resident reservation—can be
skipped by ordinary module-visible calls. Canonical self-consistency is not
proof that a record belongs to the currently admitted action.

**Recommendation:** make unbounded/raw semantic decode helpers owner-private.
Expose typed bounded decode entry points for every record kind, including the
canonical path identity, and return an untrusted decoded value that cannot
drive action until `bind_to(&reservation, observed slot/identity)` produces a
sealed bound capability. Make cleanup/barrier recovery consume that capability,
not a raw record or row. Test limit and limit-plus-one, unknown field,
noncanonical form, wrong reservation/action/owner/schedule, and changed slot
identity.

### [P2-6] The fault vocabulary is neither complete nor independently pinned

**Evidence:**

- `fault_v1.rs:46-179` stores boundary names in ten string slices. Several
  restart edges named by the controlling design have no distinct key. Examples
  include bootstrap-guard lock acquisition/reobservation/release and the
  separate `.gwz` creation row; catalog-bootstrap active-record publication;
  and managed-bootstrap marker write, staging publish/reopen, successor
  scratch create/write/flush, prior-generation retirement, and final intent
  retirement. Broader labels such as `component_create` or
  `successor_publish` cannot identify crashes on both sides of all those
  edges.
- The only equality gate asserts `len() > 100`, uniqueness, and four sample
  names (`interface_tests/codec_faults.rs:103-119`). It does not compare the
  production vocabulary with an independently declared expected key set, as
  required by `GwzM5-8R4bR1InterfaceCheckpoint.md:305-313`.

**Impact:** deleting or failing to add a recovery boundary still passes the
interface suite. Later restart matrices cannot prove coverage of the complete
runtime/catalog/admission/barrier/bootstrap/cleanup protocol if the key space
already collapses distinct edges.

**Recommendation:** enumerate every durable edge as a closed typed key and
keep ordinals/labels as instance data. Declare the expected stable-key set in a
separate test module that does not derive from `all()`, compare exact equality,
and require each key to map to its production injection point before claiming
the later C7/platform gate.

## P3 observations

### [P3-1] Occupancy state conflates idle with Preparing-after-publication

`CatalogOccupancyV1` retains only `preparing_without_final`
(`protocol/bounds.rs:74-125`). Once the final directory exists but the durable
admission record is still `Preparing`, the caller must pass `false`, which is
indistinguishable from `Idle`; `can_admit_new()` can then return true. The
single admission record prevents a legitimate new admission until recovery has
replaced `Preparing` with `Idle`. Model this as a closed admission occupancy
state (`Idle`, `PreparingWithoutFinal`, `PreparingWithFinal`) so the capacity
API itself cannot give an affirmative new-admission answer during recovery.

### [P3-2] Frozen literals are repeated instead of derived from their owners

Examples include the hard-coded staging/final names in
`protocol/admission.rs:141-143`, `rows.len() > 3` in
`protocol/cleanup.rs:181`, and the `1..=8` bootstrap request bound in
`bootstrap.rs:149-155`. Derive these from `InfrastructureSlotV1`,
`RootEntryNameV1`, `MAX_CLEANUP_ROWS`, and
`MAX_MANAGED_PARENT_BOOTSTRAPS`. This is not currently incorrect, but it makes
an otherwise reviewed protocol-version change capable of drifting across
owners.

## Positive findings

- The arithmetic constants and both `K=1,N=8` and `K=8,N=8` layouts agree with
  the accepted `K + 2N` model.
- The 10/13/261 name inventories are deterministic and unique; the literal
  root/action/retired budgets match the plan.
- Capacity, admission, barrier, and cleanup encodings are Taut-generated at the
  field/tag layer, canonically re-encoded, and do not introduce a separate wire
  serializer.
- Fixed-record replacement, directory handoff, and cleanup physical resolution
  are conservative for the rows they represent.
- The focused R1 interface suite passes: 28 passed, zero failed.
- The checked commit changes no public `protocol/gwz.taut.py`, generated public
  protocol file, merge record model, or merge dispatcher source. The production
  decoder gate
  `production_decoder_rejects_v1_before_body_decode` also passes. Public
  merge-v1 dispatch behavior therefore remains unchanged and disabled.

## Acceptance condition

Remediate P2-1 through P2-6, add the corresponding negative/privacy and exact-
equality gates, and submit one new settled tuple for two independent reviews.
R1 is acceptable only when those reviews report no P0/P1/P2. R4b-G and consumer
conversion remain blocked in the meantime.
