# R4b remediation R1 filesystem/interface settled-tree review

Date: 2026-08-13

Reviewed immutable tuple:

- workspace root `3fa8cdc8c2c299a7a8874cd3580d7379a383daf8`;
- `gwz-core` `081856e72bcee23f5900a7698f770721ae02545d`; and
- `gwz-cli` `3cca145c0b32410f250f640730ed7ca18f1da59f`.

Controlling documents:

- `dev-docs/GwzM5-8R4bP1P2-RemPlan-4.md`; and
- `dev-docs/GwzM5-8R4bR1InterfaceCheckpoint.md`.

## Findings

### [P1-1] A managed-parent plan is not bound to the admitted reservation or even to its schedule

**Where:** `gwz-core/src/checked_artifact/bootstrap.rs:200-213`; the contract
test demonstrates the defect at
`gwz-core/src/checked_artifact/interface_tests/contracts.rs:499-517`.

**Violated criterion:** RemPlan-4 sections 3.2 and 3.6.1 require the complete
managed-parent plan to be capacity-admitted before mutation, require its stable
bootstrap/component/generation ordinals to come from the resident reservation,
and require retry to operate through the reservation-bound immutable intent.
The checkpoint likewise says every required name is derivable from the
reservation and that `ManagedParentBootstrap` requires an admitted action and a
provider-issued plan.

**Evidence and failure scenario:** `preflight` produces the unconstrained
associated `Plan`, while `execute` accepts that plan alongside any
`AdmittedActionV1`. There is no equality/binding check between the plan's specs
and the action's `ActionScheduleV1`, and no plan digest, ordinals, reservation
digest, owner, or action in the type passed to `execute`. The committed test
preflights all four managed purposes, deliberately creates an admitted action
whose schedule has **zero** bootstrap rows, and then successfully executes the
four-purpose plan. Two instances of one provider type can also exchange plans
because the associated type is provider-type-specific, not issuance- or
action-specific.

**Impact:** R2 can mutate managed parents without reserving the required
scratch, active/retired intent, marker, or terminal capacity. A crash then has
no schedule-owned ordinal/name by which to recover the mutation. This defeats
the central restart-closure and no-patchwork purpose of R1.

**Required correction:** split planning from execution explicitly. Preflight
must return a canonical immutable plan whose schedule contribution is used to
construct the capacity reservation. After admission, a private binder must
consume that exact plan plus the admitted action and return an opaque
`BoundManagedParentPlanV1` only if its digest, rows, ranges, purposes, and
missing suffixes equal the resident schedule. `execute` must accept only that
bound value. Add the Taut-defined `ManagedParentBootstrapIntentV1` needed for
the initial/successor/marker-retirement recovery chain.

**Regression tests:** reject an empty-schedule action with a nonempty plan;
reject plan/action, plan/owner, plan/schedule, and cross-provider-instance
substitution; accept the two maximum layouts; and crash/recover every generation
using only names and ordinals from the resident reservation.

### [P1-2] `ActionNamespace` is bypassable and can reserve unscheduled or out-of-range slots

**Where:** `gwz-core/src/checked_artifact/namespace.rs:194-215` exposes the raw
implementation; `namespace.rs:233-273` accepts any `ActionSlotV1`;
`namespace.rs:282-326` exposes the unbound engine trait; and
`gwz-core/src/checked_artifact/protocol/slots.rs:107-172` permits raw indexed
variants without validating their bounds.

**Violated criterion:** RemPlan-4 section 3.6 and checkpoint section 6 make
`ActionNamespace` the only consumer-facing namespace capability. It must derive
destinations from the admitted resident reservation, validate barrier ordinals
against its schedule, and prevent arbitrary path-pair/barrier use.

**Evidence and failure scenario:** a consumer can call `implementation_mut()`
and invoke `NamespaceProtocol::barrier`, `publish_no_replace`, or
`retire_exact` directly. The trait's `ReservationBinding` and `BarrierOrdinal`
are arbitrary associated types; the contract test legitimately implements both
as `u8`. Independently, `reserve_action_slot` and
`reserve_action_retirement_slot` accept such values as
`BarrierIntentActive(255)`, an unreserved bootstrap generation, a cleanup alias
omitted from the schedule, or a base slot in the wrong protocol phase. Only the
separate `scheduled_barrier_ordinal` helper checks a barrier index; callers need
not use it.

There is a second interface dead end at `namespace.rs:26,58,106,140,159,172,277`:
the retained-capability and successful-result constructors are private to the
`namespace` module. A real platform engine in a sibling module cannot issue a
retained directory/object or return `PublishedIdentity`, `RetiredIdentity`, or
`DurableNamespace`; only the `cfg(test)` support module can do so. R2 therefore
cannot implement the frozen seam without changing it.

**Impact:** widening the constructors makes the first half exploitable; leaving
them private makes production implementation impossible. In either case this is
not a safe frozen boundary for R2. Unscheduled publication can consume names
outside admission accounting, while a raw barrier can reuse or invent an
ordinal with no durable `BarrierIntentV1` ownership.

**Required correction:** keep a lower-level engine private behind the wrapper
and remove consumer access to it. Give the engine module provider-owned issuer
methods for retained handles and successful result proofs. Expose only
reservation-aware operations on `ActionNamespace`; those methods should accept
opaque scheduled-slot values derived from the resident schedule, not raw
`ActionSlotV1` or an engine-specific ordinal. Validate the retained action
parent/identity and reservation digest before every operation.

**Regression tests:** compile-fail tests for raw engine access and proof
construction outside the owning module; exhaustive rejection of ordinal 64/24/8
and of every schedule-omitted slot; cross-action/cross-parent binding rejection;
and a production-shaped non-test engine that can implement the interface
without widening constructors.

### [P1-3] Capability and collision proofs can be combined across different roots

**Where:** `gwz-core/src/checked_artifact/capability.rs:300-360` binds a root
identity only in `FilesystemCapabilityProof`;
`gwz-core/src/checked_artifact/capability/collision.rs:142-186` omits the root
identity and retained-root capability from `PrivateNamespaceCollisionProof`;
and `gwz-core/src/checked_artifact/bootstrap.rs:68-99` combines the two proofs
without checking even their path-profile equality.

**Violated criterion:** RemPlan-4 section 3.1 requires one ordered, read-only
capability/path/collision preflight on the complete selected root before private
mutation. Checkpoint section 2 explicitly says consumers cannot assemble a
permit from unrelated observations.

**Failure scenario:** scan workspace B (or an old incarnation of workspace A)
with an empty index/worktree and obtain a collision proof. Acquire the capability
proof for workspace A, which has a tracked `.gwz/checked-artifacts` collision.
`CatalogBootstrapPermit::workspace(&capability_a, &collision_b)` succeeds. The
collision proof has only a freely supplied domain and path profile; two roots
with the same ASCII mode vector are indistinguishable. Even two proofs with
different path profiles are accepted by the constructor.

**Impact:** catalog bootstrap may create the private namespace on a root whose
tracked/index-visible collision domain was never scanned, overwriting the
precondition that prevents private GWZ state from aliasing user-controlled Git
state.

**Required correction:** issue one opaque aggregate pre-catalog permit from one
provider-owned transaction over a retained root. At minimum, the collision
proof must bind the same durable and invocation root identities, canonical path
walk, support profile, fixed domain version/digest, and freshness/lease token as
the capability proof, and `CatalogBootstrapPermit` must be constructible only
after exact equality is checked. Do the equivalent for the non-worktree
Git-directory proof.

**Regression tests:** cross-root, stale-root-incarnation, mismatched-path-mode,
mismatched-domain, and workspace-vs-Git-directory proof substitution must all
be unconstructible or reject before any bootstrap call.

### [P1-4] The frozen schema omits the durable records that make catalog and managed-parent bootstrap restart-closed

**Where:** `gwz-core/protocol/checked_artifact.taut.py:28-96` defines path,
identity, schedule, reservation, admission, barrier, and cleanup messages only;
`gwz-core/src/checked_artifact/protocol/codec.rs:6-17` lists `Authority`,
`BootstrapIntent`, `CatalogBootstrap`, `Infrastructure`, and `Marker` merely as
size labels. There is no `ManagedParentBootstrapIntentV1`,
`CatalogBootstrapV1` recovery record, infrastructure record, marker record, or
canonical checked authority record in the private schema or semantic adapters.

**Violated criterion:** RemPlan-4 sections 3.2, 3.6, and 3.6.1 specify the exact
durable catalog-bootstrap and managed-parent intent ownership/restart protocols.
R1 freezes the bounded multi-bootstrap transition schedule, bounded record
codecs, `BarrierIntentV1`, and `ManagedParentBootstrap` before parallel R2
implementation. The repository rule also requires protocol payloads to be
Taut-defined rather than introduced later as a shadow protocol.

**Failure scenario:** an R2 platform/catalog implementor reaches the first
catalog staging crash or first managed-component successor. No frozen type can
record the retained parent binding, full component plan, local/global ordinals,
cursor, staging/final names, ownership token, marker binding, or successor
relationship described by the accepted architecture. The implementor must
invent those fields and the transition contract while implementing, after the
claimed interface freeze.

**Impact:** Linux, Windows, and catalog work cannot safely proceed in parallel;
each either blocks or creates incompatible local recovery state. This is exactly
the architectural patchwork the lead-owned checkpoint is meant to prevent.

**Required correction:** add the missing Taut records and closed semantic
adapters before R2, including all binding fields and derived IDs/digests from
the accepted restart tables. Replace size-only record-kind placeholders with
actual `BoundedCanonicalRecordV1` implementations. Freeze their construction,
successor, adoption, and retirement relations with independent canonical
vectors.

**Regression tests:** canonical round trips plus field-mutation rejection for
every binding; initial/successor/final managed intent chains; catalog-bootstrap
missing/partial/exact staging rows; marker adoption/retirement; unknown and
noncanonical field rejection; and maximum-size/limit-plus-one cases.

### [P2-1] Canonical path identity cannot detect replacement of an intermediate parent

**Where:** `gwz-core/src/checked_artifact/capability.rs:52-185` persists only
component spelling, case mode, and folded spelling; `capability.rs:300-360`
adds one root identity but no identity/rename-domain vector for walked parents.
`CatalogBootstrapV1::recover_or_create` at
`gwz-core/src/checked_artifact/bootstrap.rs:103-109` receives no retained root or
parent handle at all.

**Violated criterion:** RemPlan-4 section 3.1 requires the complete managed path
to be walked with retained no-follow handles and says parent replacement or mode
change is ambiguity. Checkpoint section 2 repeats that a changed parent mode or
identity is a mismatch.

**Failure scenario:** after preflight, replace an intermediate `.gwz` directory
with another directory on the same supported filesystem and with the same case
mode. The root identity and encoded `CanonicalPathIdentityV1` remain equal. A
retry or a path-based catalog implementation resolves the same component bytes
to the replacement and cannot detect that the scanned/accepted parent was lost.
Because the permit carries no retained directory handle, the catalog trait
encourages ambient/path state to fill the gap.

**Impact:** checked state or a roaming-anchor/bootstrap mutation can be applied
under a substituted namespace even though the architecture promises
identity-bound, no-follow mutation and fail-closed recovery.

**Required correction:** make path acquisition return one retained path-walk
capability whose persisted fact includes the durable parent identity and rename
domain (or an equivalently strong identity chain) at every component. Bind the
permit and catalog bootstrap to the retained final parent handle and invocation
identities, then revalidate the chain immediately before mutation and on retry.

**Regression tests:** replace each intermediate parent with same-mode and
different-mode directories, symlinks/reparse points, and another directory on
the same rename domain; every case must reject before private or managed
mutation. Exercise the race between collision acceptance and catalog bootstrap.

### [P2-2] The support-profile claim is not structurally tied to the durable identity provider

**Where:** `gwz-core/src/checked_artifact/capability.rs:276-298` permits an
arbitrary `DurableIdentity` associated type and returns the support profile as a
separate value; `capability.rs:335-360` defines a second preflight trait whose
`inspect` can return any profile, root identity, and path value. The contract
test at `interface_tests/contracts.rs:168-190` uses an invocation-local
`Identity(u8)` while claiming the Linux ext4 durable profile.

**Violated criterion:** RemPlan-4 section 3.3 defines a closed, profile-specific
durable identity tuple and forbids syscall-success/fallback admission.
Checkpoint section 2 says the closed support profile and durable identity are
proved together by the provider.

**Failure scenario:** a provider reports `LinuxExt4FsIocGetFsUuidV1` but uses an
inode, mount-local ID, or `u8` as its durable root identity, or reports the NTFS
profile while returning an identity with no full volume GUID. All current
traits and the capability-proof constructor accept the mismatch.

**Impact:** the type crossing the catalog/recovery boundary does not carry the
reboot-stable promise named by the profile. R2 tests can accidentally validate
only invocation identity while production records claim a durable supported
profile.

**Required correction:** merge these seams under the closed provider. The
durable identity crossing the proof boundary should be
`DurableObjectIdentityV1` (or a sealed profile-indexed type), and issuance must
verify that its variant matches the support profile. Separate invocation IDs
may remain generic but must not substitute for the persisted identity.

**Regression tests:** reject every profile/identity-variant mismatch and a
mount-local/invocation-only identity; prove all three supported tuples and the
complete unsupported-profile table.

### [P2-3] The fault vocabulary is not independently pinned as required

**Where:** `gwz-core/src/checked_artifact/fault_v1.rs:17-179` declares the
vocabulary; `gwz-core/src/checked_artifact/interface_tests/codec_faults.rs:103-119`
asserts only that there are more than 100 unique keys and spot-checks four.

**Violated criterion:** RemPlan-4 section 3.6.1 says adding an enum variant
changes an independently pinned test key set. Checkpoint section 9 requires the
independently declared fault-key set to be pinned.

**Failure scenario:** delete or rename any unasserted boundary, or replace it
with another unique spelling. The test still passes as long as the total remains
above 100 and the four sampled keys survive. An implementation can therefore
omit a crash boundary while retaining a green interface gate.

**Impact:** parallel platform implementations and the later executable C7 gate
have no stable, independently reviewed key contract. Restart coverage can drift
silently.

**Required correction:** place the expected stable-key list in an independent
fixture/module that does not derive from `FaultAxisV1::boundaries`, compare exact
set equality and exact cardinality, and require a deliberate fixture change for
every vocabulary edit.

**Regression test:** the exact independent expected set is sufficient; add a
meta-test proving duplicate, missing, and extra keys fail the comparison.

### [P3-1] The generated corpus proves Taut shape drift, not valid semantic variants

**Where:** `gwz-core/protocol/checked_artifact-corpus/golden.json` uses generic
generator values; for example its durable-identity message selects the macOS
kind while populating unrelated optional fields. The semantic decoder correctly
rejects that shape, but no independent semantic corpus is committed.

**Impact:** generator drift is pinned, but the cross-platform canonical bytes
of valid Linux, macOS, Windows, idle/preparing admission, maximum schedule,
barrier, and cleanup records are tested only through Rust-constructed unit
values. Another implementation cannot use this corpus as interoperability
evidence for the actual closed semantic protocol.

**Recommended correction:** retain the generated corpus and add a small
independent semantic-vector fixture containing every closed variant and both
maximum schedule forms. Compare decode/re-encode bytes and expected rejection
vectors from Rust and any future platform/runtime adapter.

## Public behavior and wire review

The R1 commit does **not** change `protocol/gwz.taut.py`, the public generated
protocol, public `GwzCore` service methods, the existing `CheckedArtifact`
facade signatures, or dispatcher activation. The new schema is private under
`checked_artifact`, and the new modules are not exported from `gwz-core`.
Therefore I found no public merge-v1 behavior or wire delta in this checkpoint.

That absence does not mitigate the private durability-interface blockers above.

## Acceptance verdict

**NO-GO for R2 platform/catalog implementation.** There is no P0 finding, but
four P1 and three P2 findings remain. In particular, the committed contract
test proves that an unreserved managed-parent plan can execute, the namespace
wrapper is bypassable, pre-catalog proofs are cross-root composable, and the
durable records needed by R2 have not been frozen.

Remediate all P1/P2 findings and repeat two settled-tree critical reviews before
assigning Linux, Windows, or platform-independent catalog implementation. The
public merge-v1 wire/behavior quarantine may remain unchanged while doing so.
