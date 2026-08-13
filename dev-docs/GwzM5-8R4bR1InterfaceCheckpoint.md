# R4b remediation R1 interface checkpoint

Date: 2026-08-13

Status: **the third owner-boundary remediation is implemented and locally
verified; its exact settled tuple and two independent re-reviews are still
required before R2 or R4b-G begins**.

This checkpoint turns the accepted replacement architecture in
`GwzM5-8R4bP1P2-RemPlan-4.md` into Rust ownership boundaries. It does not
convert a consumer, create a catalog, change the public merge-v1 service record
model, or enable the v1 dispatcher. It adds private checked-artifact recovery
records under a separate Taut schema. The existing `CheckedArtifact` facade and
`WorkspaceMutatorLock` public API remain stable until R2.

R0-L is complete. GitHub Actions run `31537368340` and retained artifact
SHA-256 `5ebddb1141576816609c2fe1e8e65b163d75ba9a3e4001852e0a6ed7baff313c`
were independently accepted in
`GwzM5-8R4bLinuxIdentityEvidence-Review-2.md` with no P0/P1/P2 finding.

## 1. Ownership and visibility

New contracts remain private to `checked_artifact` unless a data-only input
must be assembled by an existing caller. Opaque proofs and reserved-slot
handles may cross internal module boundaries; their constructors do not.

The checkpoint is split before implementation into these cohesive owners:

- `capability` owns ASCII components, component-wise path equivalence,
  durable-identity provider traits, the closed platform support table, typed
  capability failures, lossless collision inputs, and opaque pre-catalog
  proofs;
- `protocol` owns literal budgets, distinct digests and ordinals, exhaustive
  names/slots, bounded record I/O, schedules, capacity reservations, admission,
  intents, and immutable cleanup records;
- `leaf` owns ordinary and durable leaf proof shapes plus the one retained-
  handle observation seam;
- `namespace` owns reservation-bound publish, retirement, and barrier seams;
- `bootstrap` owns live runtime, first catalog, and declared managed-parent
  bootstrap contracts; and
- `fault_v1` owns one target-independent iterable fault vocabulary. Runtime
  ordinals and labels are instance data, not enum variants.

No new concept is added to `classification.rs` or `residue.rs`. The existing
677-line `identity.rs` is not extended; platform implementation will split it
under `identity/` after this checkpoint is accepted.

Provider, catalog, namespace, leaf, and bootstrap mutation traits use
`pub(in crate::checked_artifact)` or narrower visibility. `CheckedArtifact`,
`CheckedArtifactFact`, `CheckedArtifactTransition`, and
`CheckedArtifactPolicy` retain their present crate-facing signatures. R1 does
not widen visibility merely to avoid dead-code warnings.

## 2. Filesystem capability boundary

`AsciiComponent` is a validated, nonempty, normal ASCII component of at most
255 bytes. It rejects NUL, `/`, `\\`, `.`, and `..`. It has no unchecked public
constructor.

Each retained parent supplies exactly one mode:

```text
PathComponentMode = Sensitive | AsciiCaseFold

CanonicalComponent {
    original_ascii,
    parent_mode,
    canonical_ascii,
    parent_durable_identity,
    parent_invocation_identity,
    rename_domain,
}
```

`CanonicalPathIdentityV1` is nonempty, component-wise, canonically encoded,
and bounded to 4 KiB. Sensitive mode preserves the component; fold mode uses
ASCII folding only. A changed parent mode or identity is a mismatch, not a new
family.

The provider seams are:

```text
PathEquivalenceProvider.parent_mode(retained_parent)
DurableIdentityProvider.directory_identity(retained_directory)
DurableIdentityProvider.file_identity(retained_file)
DurableIdentityProvider.rename_domain(retained_directory)
DurableIdentityProvider.support_profile()
```

The closed initial support profiles are:

- Linux local ext4 with `FS_IOC_GETFSUUID`, a nonzero 16-byte external UUID,
  and a positive 1..=128-byte retained-fd persistent handle;
- macOS persistent object ID plus the proved volume capability; and
- Windows local NTFS with full volume GUID, `FILE_ID_INFO`, per-parent
  `FileCaseSensitiveInfo`, and write-through namespace support.

There is no fallback profile. Unsupported capability maps only to
`UnsupportedOperation`; ordinary failure maps to `IoError`; substitution or
fact mismatch remains an ambiguity/recovery result selected by the caller.

The workspace collision boundary consumes lossless Git path bytes, every index
stage, mode, ordinary and extended flags, and tracked worktree kinds. Its fixed
domain is the complete `.gwz/checked-artifacts` prefix plus the three catalog-
bootstrap scratch/active/staging names. Exact, ancestor, descendant, platform-
equivalent, index-only, higher-stage, skip-worktree, conflict, and tracked
worktree collisions reject before catalog or managed mutation. Git-directory
artifacts require a separately issued non-worktree proof rather than a fake
empty scan.

Only one opaque `PreCatalogPermitV1` issued by a provider-owned transaction may
enter catalog bootstrap. It binds one retained root, support profile, durable
and invocation identities, rename domain, path walk, collision domain,
workspace or Git-directory classification, freshness lease, and root kind.
Consumers cannot assemble it from unrelated observations.

## 3. Exhaustive namespace grammar

Digest and ordinal domains are non-interchangeable private-field newtypes:

```text
ActionDigestV1                     [u8; 32]
RequestOwnerBindingV1              [u8; 32]
ScheduleDigestV1                   [u8; 32]
RecordDigestV1                     [u8; 32]
BarrierOrdinalV1                   0..64
BootstrapOrdinalV1                 0..8
BootstrapGenerationV1              0..24
BootstrapComponentOrdinalV1        0..8
CleanupOrdinalV1                   0..3
```

The root grammar has exactly ten infrastructure slots and at most 64 active
action directories. An action directory has exactly thirteen base slot kinds,
64 barrier triples, 24 bootstrap intent pairs, and eight retired-marker slots:
261 possible deterministic slots. Every scratch name contains the action
digest and kind/ordinal. No nonce or random-name API exists.

The literal budgets are:

| Scope | Entries | Aggregate name bytes | Metadata bytes |
| --- | ---: | ---: | ---: |
| Root | 74 | 18,870 | 18,944 |
| Action or prospective staging | 261 | 66,555 | 66,816 |
| Retired-action root | 64 | 16,320 | 16,384 |

Every entry charges count, native/raw filename bytes, and 256 metadata bytes
before classification, open, or decode. A recognized invalid name is distinct
from a foreign name; neither is silently inert.

## 4. Schedule, reservation, and admission

One canonical `ActionScheduleV1` is derived from an immutable
`ManagedParentPlanV1` and carries its complete digest, stable barrier ordinals,
ordered managed-parent specs, contiguous global bootstrap-generation and
component-ordinal ranges, and the cleanup alias set. Managed bootstrap
invocation rejects empty input, duplicate purposes, `K > 8`, `N > 8`, or a
zero-component spec. For `K` specs and `N` total missing components:

```text
generation_count = K + 2N <= 24
```

The generic schedule grammar has maximum shapes `K=1,N=8` with 17 generations
and `K=8,N=8` with 24. The initial product inventory declares four managed-
parent purposes; the wider grammar reserves the accepted bound without adding
new product purposes. Barrier count is at most 64. Every name required by the
schedule is derivable from the reservation; retry allocates no new name.

`ActionCapacityReservationV1` binds action digest, request/owner binding, and
the full schedule. Its canonical schedule and record digests are derived, not
separately supplied fields.

Admission reserves terminal retired-directory capacity as well as current
root/action capacity:

```text
retired directories
+ distinct active or Preparing action credits
+ one prospective action credit
<= 64
```

Recovery of an already-owned `Preparing` action does not add the prospective
credit again. This prevents failed retired-object GC from stranding an admitted
action.

The persistent record is exactly:

```text
ActionDirectoryAdmissionV1 = Idle | Preparing {
    action_digest,
    request_owner_binding,
    capacity_schedule_sha256,
    staging_name,
    final_action_name,
    resident_reservation_sha256,
}
```

Only a capacity reservation constructs `Preparing`. Fixed-slot replacement
accepts old active plus missing/expected-prefix scratch, old active plus exact
new scratch, or new active plus missing scratch according to its closed table;
all other combinations are ambiguous. A partial scratch is a byte-exact prefix
of the one expected canonical record.

The directory handoff distinguishes missing, exact bounded directory, and
other for staging and final. While `Preparing`, staging may contain only the
matching resident reservation but is charged against the complete action
budget. Both directories present, wrong identity/kind/reservation, extra child,
or an orphan staging/final directory is ambiguity. Authority is unavailable
until the observed admission is `Idle` and the exact final resident record owns
the action directory. Observation and issuance are private to the catalog
admission owner; a checked-artifact sibling cannot construct the opaque
`AdmittedActionV1`.

## 5. Bounded records and payload separation

The shared record interface provides canonical encode/decode plus a bounded
reader that reads at most `limit + 1`, rejects oversize before semantic parse,
and requires canonical re-encoding equality.

| Record | Limit |
| --- | ---: |
| Authority, capacity, admission | 16 KiB each |
| Barrier or managed-bootstrap intent | 16 KiB each |
| Catalog bootstrap | 16 KiB |
| Infrastructure | 8 KiB |
| Marker | 4 KiB |
| Cleanup worklist | 16 KiB |
| Canonical path identity | 4 KiB |

Source and goal payloads are not protocol records and have no one-MiB family
sum. They are streamed against request-supplied length and SHA-256. The
ordinary byte-returning observer remains bounded and fallible.

## 6. Leaf and namespace proof boundary

The ordinary proof is `Missing`, `Exact { identity, length, sha256, bytes }`,
or typed `Other`. The durable proof is `MissingDurable`,
`ExactDurable { identity, length, sha256 }`, or typed `Other`.

The durable observer opens one leaf no-follow, proves exact content and
identity, flushes that same retained handle, executes the parent namespace
barrier, then revalidates parent mapping, name/handle identity, length, and
content. Durable absence is missing/barrier/missing. Operational failure is not
`Other`, and `Other` is not collapsed to missing.

Namespace methods do not accept arbitrary path pairs:

```text
publish_no_replace(retained_source, reserved_destination)
retire_exact(retained_source, reserved_retirement_slot)
barrier(retained_parent, precomputed_barrier_ordinal)
```

The source is handle/identity-bound. The destination belongs to the admitted
action reservation. `ActionNamespace` is the only consumer-facing namespace
capability: it is constructed from an opaque admitted action, derives slots
from the exhaustive grammar, binds them to the resident reservation digest,
and validates barrier ordinals against the resident schedule. Platform
implementations and their raw issuer remain private implementation values
inside this wrapper. Successful component installation and marker retirement
produce opaque provider-issued evidence; managed-intent successors accept only
that evidence.

`BarrierIntentV1` binds caller/action, schedule, ordinal, anchor identity,
private home parent/name, target durable parent/path profile, and the derived
reserved target leaf. Its intent ID is derived from every other field and must
verify on decode.

## 7. Bootstrap boundaries

`WorkspaceRuntimeBootstrapV1` is capability-neutral. It owns only the fixed
bootstrap guard, `.gwz`, `.gwz/locks`, final lease file, retained handles, and
advisory-lock convergence. It cannot call durable providers, catalog, namespace
barriers, or checked authority. The public `WorkspaceMutatorLock` remains a
compatibility wrapper in R2.

The pre-catalog owner performs retained-root observation, permit issuance, and
immediate revalidation as one structurally ordered operation. It passes a
lifetime-bound `RevalidatedPreCatalogPermitV1` directly to
`CatalogBootstrapV1`; no consumer can issue or retain a revalidated permit or
interpose work between revalidation and bootstrap.

`CatalogBootstrapV1` owns only the fixed scratch/active/staging/final grammar
and retained catalog/anchor result. Its active record carries a nonzero random
ownership token but does not claim the identity of a staging directory that
has not yet been created. The later infrastructure record carries that token,
the active bootstrap record ID, and the observed staging identity. A private
owner compares the physical marker, retained parent/path, fixed staging name,
physical identity tuple, support profile, and any stored record before issuing
the opaque exact observation accepted by recovery. The recovery classifier
does not accept a caller-supplied expected infrastructure value.

The closed recovery table accepts only missing or exact-prefix scratch, exact
active record, owner-bound exact staging/final infrastructure, and exact
retired-record progress; every unlisted combination is ambiguous.

`ManagedParentBootstrap` requires a `BoundManagedParentPlanV1` issued only after
the immutable plan exactly matches the resident admitted reservation. Execution
accepts that bound value, not an independently supplied plan and action. The
only initial purposes are merge store, merge archive, preservation bundles, and
root-preservation markers. There is no raw arbitrary-path request or arbitrary
caller-token seam.

## 8. Private recovery-record model

`protocol/checked_artifact.taut.py` is the sole field/tag definition for the
new internal v1 records. Generated Rust and the generated shape corpus are
committed and checked by `protocol/regen.py --check`. A separate, hand-authored
26-vector semantic fixture is outside regeneration and is compiled into the
test suite. Its literal canonical bytes must bounded-decode and re-encode
byte-for-byte. Semantic adapters enforce closed variants, literal sizes,
derived digests, reservation binding, and canonical re-encoding; they do not
define a second wire format.

The schema owns canonical path and durable-object identities, physical action
schedules, capacity reservations, admission, checked authority, catalog
bootstrap, infrastructure, barrier intent, managed-parent bootstrap intent,
ownership markers, and cleanup worklists. Every record has a bounded canonical
adapter. Recovery first performs the `limit + 1` read and then binds the decoded
record to its resident permit, reservation, plan, expected ordinal, predecessor,
identity, or slot before it can drive work. Raw decoders remain owner-private.

## 9. Fault and proof gates

The platform-neutral `CheckedArtifactFaultKeyV1` is composed from closed runtime,
catalog-bootstrap, admission, record, namespace, durable-leaf, barrier,
managed-bootstrap, cleanup, and terminal axes. `all()` returns the same key set
on every target. Ordinals and labels live in `FaultInstanceV1`.

Independent equality tests must pin:

- the 10/13/261 slot sets, deterministic names, uniqueness, and parse/render;
- every literal budget and exact/limit-plus-one behavior;
- all valid schedule arithmetic, both maximum layouts, and invalid K/N/barrier
  cases;
- retired-directory capacity credit for new admission and Preparing recovery;
- fixed-record replacement and directory-handoff tables;
- immutable cleanup ordering and exact physical resolution;
- every record limit, unknown/trailing/noncanonical bytes, and exact-prefix
  scratch classification;
- every binding field in barrier intent identity;
- coherent checked-authority issue and recovery binding, with raw observation
  and provider construction sealed behind its owner;
- every role-specific barrier and managed-bootstrap namespace operation,
  including provider/action/reservation, source parent/leaf/kind, and ordinal
  substitution;
- exact installed component identity/mode/path and marker-object identity
  across installation and retirement successors;
- all durable semantic record families and closed variants through the
  independent literal-byte fixture;
- the independently declared fault-key set; and
- capability/path/collision/leaf/namespace/bootstrap privacy and typed-error
  boundaries.

Critical interface reviewers must report no P0/P1/P2 defect before Linux,
Windows, or platform-independent catalog implementation is assigned. Consumer
conversion remains R2 and is not part of this checkpoint.

Local second-remediation evidence on 2026-08-14:

- `cargo test -p gwz-core`: 1,247 passed, one ignored (1,202 unit plus 45
  integration tests) on the final tree;
- `cargo test -p gwz-core checked_artifact::interface_tests`: 72 passed;
- `cargo test -p gwz-core --test protocol`: 29 passed;
- `cargo clippy -p gwz-core --all-targets -- -D warnings`: passed;
- `cargo fmt --all -- --check`: passed; and
- `python protocol/regen.py --check`: passed with `taut-proto 0.8.1`.

Public `protocol/gwz.taut.py`, generated public protocol, conversion code, and
merge dispatcher are byte-unchanged. New production modules are below 500 LOC;
the generated Taut adapter is the only new-file exception.

## 10. Exact-tree review disposition

The second remediation was reviewed at workspace/core/CLI tuple
`c51cae969a6549a6986750447b8a648e444aa709` /
`a2a7125fe6b69f09ca76d83caeb28b0684c3c295` /
`3cca145c0b32410f250f640730ed7ca18f1da59f`.

`GwzM5-8R4bR1Interface-ReviewFS-3.md` and
`GwzM5-8R4bR1Interface-ReviewState-3.md` independently found no P0/P1 but the
same three P2 defects:

- catalog occupancy evidence is not location-bound and may mutate before the
  complete staging/final shape is accepted;
- managed namespace mutations do not return or recover the opaque exact
  evidence required by their durable successors; and
- authority request hashes are still composable with an unrelated retained
  observation.

The reports also retain the nonblocking duplicated catalog-name P3. The
third-remediation architecture and execution gate are frozen in RemPlan
sections 14-15. This checkpoint is not accepted until that implementation is
committed and two fresh exact-tree reviews report no P0/P1/P2.

## 11. Third-remediation implementation checkpoint

The three P2 boundaries and the remaining catalog-name P3 are structurally
corrected on the local tree:

- catalog recovery is one owner-controlled five-role observation and
  classification before mutation, with non-interchangeable staging/final
  evidence and mandatory aggregate reobservation after an allowed staging
  record write;
- managed install, managed marker retirement, and both restart observations
  return the exact opaque evidence required by their durable successors
  through `ActionNamespace`; and
- the authority provider owns the request binding and both request hashes in
  the same fact set as the retained root/path/source observation.

The bootstrap record, collision scan, and workspace/Git-directory policy now
derive their fixed catalog paths from one closed name owner.

Local third-remediation evidence on 2026-08-14:

- `cargo test -p gwz-core`: 1,252 passed, one ignored (1,207 unit plus 45
  integration tests), zero failed;
- `cargo test -p gwz-core checked_artifact::interface_tests`: 77 passed;
- `cargo test -p gwz-core --test protocol`: 29 passed;
- strict all-target Clippy, formatting, protocol regeneration, and diff checks
  passed;
- the production merge-v1 decoder remains disabled and public
  protocol/generated/conversion/dispatcher files are byte-unchanged from the
  preceding reviewed core tuple; and
- all changed production modules remain below 500 LOC.

This evidence does not accept R1 by itself. The implementation must be
committed as one exact settled tuple and both independent re-reviews must
report no P0/P1/P2.
