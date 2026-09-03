# R4b-P0.1 mid-lane interface amendment

Date: 2026-08-10

Status: **accepted after independent state/restart and architecture/ownership
re-reviews returned GO with no open P0-P3 finding; P0.1 implementation and the
affected P1-P4 boundaries are unblocked within the frozen manifests**

This amendment closes interface omissions found when P1-P4 first consumed the
accepted R4b-P0 surface. It does not change the user-visible merge model,
activate v1 records, add a lifecycle phase, or enable a production v1 reader,
writer, migration, or dispatcher.

## 1. Findings

### 1.1 The publication handoff was not durable

P0 correctly classifies a reverse-entry publication handoff as:

```text
NoCandidate
EvidencePending
Candidate { prefix: Baseline|Marker|Lock|Boundary, index: Pre|Staged }
```

The durable preservation model retained only `prefix`. In particular,
`PublishingCandidate + Boundary` permits both `Boundary/Pre` and
`Boundary/Staged`. After root normalization, a restart cannot recover which
index form was the authoritative reverse-entry preimage. Treating either live
form as acceptable would adopt external index drift while restoring the
preserved root.

### 1.2 Lane runtimes had no authority-owned observation binder

`BoundExactObservation::issue` is intentionally visible only inside
`v1_lifecycle::authority`. P1, P2, and P4 were predeclared outside that subtree,
but P0 did not expose closed authority-owned observer entry points. Widening
`issue` would let a runtime turn caller-selected facts into lifecycle authority
and is rejected.

### 1.3 Archived-v0 projection was trapped behind the v1 test gate

Field 10 requires an ordinary released build to project archived v0 records.
The accepted R3 projector and archive decoder were test-reachable only because
their parent modules also contained v1 decoding. Promoting those modules as a
unit would make the pre-A1 reader accept v1 records.

### 1.4 The borrowed status view was described but not exposed

P3 can observe participants, but the complete root/candidate/publication drift
logic still accepts an owned `MergeOperationRecordV0`. Converting v1 to v0 or
duplicating that logic would violate the accepted interface.

### 1.5 GC could not distinguish a direct ref from a symbolic ref

`GitBackend::read_ref` resolves revspecs. A symbolic ref resolving to the
recorded commit can therefore pass all-repository GC preflight, while the later
checked deletion correctly rejects it. If an earlier ref had already been
deleted, this violates the no-early-deletion guarantee.

### 1.6 Generated optional fields have unavoidable constructor fallout

The additive Taut field-10 fields make direct Rust/Python constructors require
the optional `record` value. Four existing parity/constructor fixtures were
missing from the P3 path manifest. They are required schema fallout, not a new
protocol surface.

## 2. Durable preservation publication handoff

### 2.1 Sole v1 durable representation

Add this v1-only model vocabulary:

```text
PublicationIndexFormV1 = Pre | Staged

PreservationPublicationHandoffV1 =
    NoCandidate
  | EvidencePending
  | Candidate {
        prefix: PublicationPrefixV1,
        index: PublicationIndexFormV1,
    }
```

`MergeOperationRecordV1` gains:

```text
preservation_publication_handoff: Option<PreservationPublicationHandoffV1>
```

This is the sole v1 durable preservation-entry handoff. The common v0
`PublicationProgress.preservation_prefix` remains available to retained v0
readers but must be absent in a valid v1 record. The eligible v0-to-v1 upgrade
domain already requires it to be absent. V0 serialization does not change.

### 2.2 Atomic installation

Every transition that first enters `Preserving` installs the exact handoff
from its bound entry authority in the same checked rewrite:

- `BeginPreservation`;
- `RecordHaltedOutcomeAndBeginPreservation`;
- `AbandonNotStartedAndBeginPreservation`.

The handoff is durable before any preservation physical mutation.
The reducer converts only the P0-bound authority fact; it does not reobserve or
accept a caller-provided durable value.

Entering rollback after preservation requires the freshly observed publication
handoff to equal the durable preservation handoff exactly. The rollback entry
retains that field unchanged; it cannot replace or reinterpret it. Direct
rollback does not install the field.

### 2.3 Lifetime and validation

The field is:

- absent before preservation entry, on ordinary forward/completed records,
  and throughout a direct rollback;
- present in `Preserving` and `RecoveryRequired` with a `Preserving` origin;
- retained unchanged through preserve-then-rollback, its rolling-back
  recovery, and the terminal `Aborted` archive; and
- immutable after first installation.

Checked validation rejects:

- a preserving state without the handoff;
- a forward/completed or direct-rollback transition that introduces it;
- a v1 record using legacy `publication.preservation_prefix`;
- a candidate handoff without the candidate/publication fields required for
  that prefix;
- an impossible prefix/index pair for the recorded publication step;
- a pending root preservation action that disagrees with the durable handoff;
  or
- a transition footprint that introduces, changes, or clears the handoff
  outside the exact entry rules.

### 2.4 Closed durable compatibility matrix

One pure model-owned compatibility projection defines which handoff values
could have been issued by F for the stored publication shape. Checked-open
validation and preservation-entry reducers both use it; they do not maintain
parallel matches.

The exact domain is:

| Durable publication shape | Compatible preservation handoff |
| --- | --- |
| no `publication` | `NoCandidate` |
| `NotStarted` or `ValidatingResults` | `NoCandidate` |
| `PreparingCandidate` without candidate | `NoCandidate` |
| `PreparingCandidate` with candidate | `Candidate { Baseline, Pre }` |
| `CommittingEvidence` without composition result | `EvidencePending` |
| `CommittingEvidence` with complete recorded composition result | `Candidate { Baseline, Pre }` |
| `PublishingCandidate` | only a non-ambiguous row from the closed pair matrix below |
| `VerifyingPublication` or `Complete` without candidate | `NoCandidate` |
| `VerifyingPublication` or `Complete` with candidate | only a `Complete` row from the closed pair matrix below |

The candidate pair matrix is:

| Prefix/index | Publishing | Verifying/Complete |
| --- | --- | --- |
| `Baseline/Pre` | allowed | rejected |
| `Marker/Pre` | allowed | rejected |
| `Lock/Pre` | allowed | rejected |
| `Boundary/Pre` | allowed | rejected |
| `Marker/Staged` | allowed only when candidate lock and boundary bytes both equal their baselines | same degenerate condition |
| `Boundary/Staged` | allowed | allowed |
| `Baseline/Staged`, `Lock/Staged`, or any other pair | rejected | rejected |

The existing candidate, composition, hash, and step validators remain
prerequisites. The compatibility projection does not claim that the live root
matches; it proves only that the stored handoff is a possible F result for the
stored record. Live observers independently require the exact physical form.

### 2.5 Preservation journal binding

The root-bearing fields in `Stash` and `ResetAttachedRef` become:

```text
root_publication_handoff:
  Option<{ prefix: PublicationPrefixV1, index: PublicationIndexFormV1 }>
```

The rule follows the repository owner, not only the enum variant. A
`PublicationRoot` action or `Participant { member_id: "@root" }` action carries
the candidate value from `record.preservation_publication_handoff` and must
equal it exactly. A non-root participant action requires `None`. A root action
also requires `None` when the durable handoff is `NoCandidate` or
`EvidencePending`.

The selected-root collision rule remains unchanged: when selected `@root` owns
the root ref, no additional `PublicationRoot` owner is appended. The selected
root participant's stash/reset journal is therefore the sole owner of root
normalization and restoration.

`BeginStash` and `BeginResetAttachedRef` no longer create a weaker publication
prefix checkpoint. They copy the already durable candidate handoff into the
write-ahead action. Normalize/restore classifiers require the exact prefix
bytes and exact `Pre` or `Staged` index image from that action. A matching
opposite index form is `Ambiguous`, not an idempotent success.

### 2.6 Action-free cursor and exhaustion binding

The exact durable handoff is not only a pending-action input. Every
preservation observation that can authorize progress reobserves the complete
root and compares it with `record.preservation_publication_handoff`:

- pending action before/after classification;
- `VerifiedPreservationCursorPrefix` issuance for every position whose prefix
  includes a root owner;
- preparation of the next root action after a prior journal clears;
- whole-prefix verification between the artifact and reset passes; and
- `VerifiedPreservationExhausted` after the final journal clears.

The corresponding sealed proof payloads bind the source digest and the exact
durable handoff, including index form. After any restart, an opposite index
form is ambiguity/error without rewrite. It cannot issue a cursor-prefix,
next-action, or exhaustion proof. Tests cross both root owner variants
(`PublicationRoot` and selected participant `@root`) with `Pre` and `Staged`,
including the collision case in which no `PublicationRoot` owner exists.

### 2.7 Direct rollback is intentionally different

Direct rollback has one immutable destination: the operation baseline. It does
not restore the reverse-entry prefix and therefore does not persist that
prefix/index as durable authority. Instead, each rollback step is installed
write-ahead and accepts only its strict exact before or exact after state.
External exact completion after a persisted step is intentionally
restart-equivalent to crash completion. Mixed states and nonterminal shapes
that belong only to a future step are rejected. The exact immutable baseline
destination may be an explicit `After` at more than one step because a valid
direct rollback can enter with its files/index already at that destination;
this is destination equivalence, not adoption of a different handoff.

The rollback matrix derives the allowed complete physical shape for every
`EvidenceCommit -> Boundary -> Lock -> Marker -> Index -> Complete` position.
It may not use a broad “any representable publication prefix” test. This keeps
direct rollback restart-safe without conflating its baseline destination with
preservation's obligation to restore an exact original handoff.

`abort/evidence.rs` is the single pure owner of that matrix. Its
`classify_v1_evidence_rollback` projection consumes the durable step, exact
head relation, and one full `[Boundary, Lock, Marker, Index]` physical shape.
Preflight, ordinary observation, rolling-back recovery, and the executor's
immediate recheck must all call it; none may carry a parallel match. In the
table below `B` is the immutable operation baseline and `C` is the immutable
candidate. A candidate lock or boundary whose bytes equal its baseline is a
true no-op and is treated as `B`; no other equivalence is permitted.

| Durable step | Exact before file/index shapes | Exact after file/index shapes | Head |
| --- | --- | --- | --- |
| `EvidenceCommit` | `BBBB`, `BBCB`, `BCCB`, `CBCB`, `CCCB`, `BBCC`, `BCCC`, `CBCC`, `CCCC` | same shapes | before is exact composition commit; after is exact baseline parent |
| `Boundary` | `CBCB`, `CCCB`, `CBCC`, `CCCC`; only when boundary is not a no-op | `BBBB`, `BBCB`, `BCCB`, `BBCC`, `BCCC` | exact baseline parent |
| `Lock` | `BCCB`, `BCCC`; only when lock is not a no-op | `BBBB`, `BBCB`, `BBCC` | exact baseline parent |
| `Marker` | `BBCB`, `BBCC` | `BBBB`, `BBBC` | exact baseline parent |
| `Index` | `BBBC` | `BBBB` | exact baseline parent |
| `Complete` | none | `BBBB` | exact baseline parent |

For `EvidenceCommit`, “after” means the same file/index shape observed with the
baseline head. A nonterminal shape that is exact only for a future step, mixes
rows, has an unrecognized file/index value, or makes both before and after
false is `Ambiguous`. `BBBB` is the sole cross-step exception: with the exact
baseline head it is the immutable destination and can already be present after
an `EvidenceCommit` entered from `Baseline/Pre`. Boundary and Lock therefore
classify it as `After`; they do not mutate it. A mutating executor runs only
from exact `Before`; exact `After` records only durable completion.

The selected-root `Complete` exhaustion fact is likewise not byte-only.
`root/abort.rs` owns one canonical no-follow selected-root observer that checks
the full expected baseline shape, named paths, parent directories, regular
non-symlink leaves, and source record identity. The authority rollback observer
must call it in the same operation that issues `VerifiedRollbackExhausted`.
Plain `fs::read`, a resolving path read, or a fact captured before the final
authority call cannot authorize `AbortOperation`.

### 2.8 Unknown-field, canonical, and upgrade closure

The typed `preservation_publication_handoff` field is installed record
authority, not an extension. The same change therefore updates all of these
owners together:

- `record_wire/unknown_fields/extract/mod.rs` recognizes the top-level key as
  typed v1 state and does not place it in `UnknownFieldManifest`;
- `record_wire/unknown_fields/extract/journals.rs` recognizes every nested
  `root_publication_handoff` key and its exact enum/object shape;
- `record_wire/unknown_fields/identity.rs` gives nested candidate fields stable
  semantic identity across overlay and rewrite;
- `record_wire/unknown_fields/mod.rs` adds the top-level field as the fifth
  v0-to-v1 collision, so a v0 extension of that name makes migration
  ineligible and cannot survive into an installed typed field;
- `model/v1/canonical.rs` includes the top-level and journal fields in the
  installed canonical state; and
- `record_wire/open_v0/adapter.rs` writes typed `None` only after the collision
  check has proved that the v0 extension is absent. A collision is rejection,
  never retirement or silent discard.

Focused tests cover known-field extraction, a v1 typed field that must not
reappear as unknown, exhaustive rejection of all five v0 top-level collisions,
nested journal identity, overlay survival of unrelated unknowns, attempted
typed-field resurrection through overlay, and canonical comparison with each
handoff variant. A checked
rewrite cannot ignore, duplicate, clear, or resurrect this field through raw
YAML overlay.

## 3. Authority-owned reverse and archive observations

`BoundExactObservation::issue` remains private to `authority`. No runtime,
executor, archive helper, or production consumer gains a constructor.

The authority subtree owns these closed entry points:

```text
authority::observe_preservation(
  backend, context, current, request)
  -> ModelResult<BoundExactObservation>

authority::observe_rollback(
  backend, context, current, request)
  -> ModelResult<BoundExactObservation>

authority::observe_archive(
  current, request)
  -> ModelResult<BoundExactObservation>
```

The preservation and rollback functions own the complete route-by-kind and
origin-specific recovery match. Internal helpers may return closed facts or
sealed tokens, but only these authority functions bind the observation.

`v1_lifecycle/authority/observe/archive.rs` owns the private
`CheckedArchiveObservation` type and its private `acquire(current, request)`
constructor. The constructor uses the version-neutral canonical location
reader from §4.3 and is the only code that can create this fact. It binds the
checked source path and digest, workspace/merge/operation identities, exact
request and terminal state, and destination path/digest state. It has only two
accepted physical results:

```text
DestinationAbsent
ExactTerminalCopy
```

Malformed, noncanonical, differently identified, or byte-different
destinations are rejected before issuance. The authority checks that the
observation still matches `current`, the exact `Archive` request, and a
terminal open record, then maps it to existing `NotStarted(Archive)` or
`Completed(Archive)`. The checked fact never leaves the authority subtree. A
stale-source, cross-merge, cross-operation, wrong-request, or reloaded-record
fact cannot be issued or rebound. Focused tests cross each of those identities
with both accepted destination states.

The reverse runtime delegates to these authority functions. It never calls
`BoundExactObservation::issue` and never reconstructs their facts.

Every authority token, observation request, physical action, and execution
attempt is universally bound by `authority/binding.rs::ProofBinding` to the
checked source digest **and** exact `OpenRecordLocation` (canonical workspace
root plus canonical open-record path), in addition to workspace/merge/operation
identity and payload. Identical bytes and IDs loaded at another workspace root
do not match. Archive-specific facts add destination identity but do not replace
this universal source-location binding.

The merge-root `abort` and `root` modules may provide `cfg(test)`, merge-scoped
re-exports of closed v1 physical observation helpers. This is visibility only:
child modules remain private, normal builds gain no v1 lifecycle reachability,
and no mutable v0 orchestration API is exposed.

## 4. Production archived-v0 boundary

The normal-build boundary is split by capability rather than by promoting the
test-only R3 module wholesale.

### 4.1 Neutral projection model

`model::archive_projection` is normal-build code. Its source discriminator is
a neutral `ArchiveSourceVersion { V0, V1 }` owned by that module; it does not
import `model::v1`. V0 and test-only v1 projectors map into this type.

`model::record_projection::project_archived` and helpers that consume only the
neutral archive projection are normal-build code. Open-v1, recovery, and
direct accepted-v1 helpers remain `cfg(test)` until A1.

### 4.2 Two decoder entries

The archive wire owner exposes:

```text
decode_archived_v0(bytes, expected_merge_id)
  -> ModelResult<ValidatedArchivedRecord>

#[cfg(test)]
decode_archived_for_r3_tests(bytes, expected_merge_id)
  -> ModelResult<ValidatedArchivedRecord>
```

`decode_archived_v0` classifies with
`InstalledMergeRecordVersions::PRODUCTION_R3`, accepts only v0, and compiles no
v1 body decoder or v1 cleanup reader into the normal build.

The test entry uses `V0_AND_V1_FOR_R3_TESTS` and retains the accepted R3
cross-version fixtures. `archive::v1`, typed v1 decoding and validation, and
`cleanup::from_v1` remain `cfg(test)`.

P3 ordinary archived status consumes only `decode_archived_v0`. P4's test-only
v1 lifecycle consumes `decode_archived_for_r3_tests`. Allocated v1 records
continue returning `UnsupportedRecordVersion` in normal builds.

There remains one v0 projector, one neutral archived projection, and one v0
cleanup implementation. No temporary duplicate decoder is allowed.

### 4.3 Canonical location acquisition and source arbitration

`record_wire/location.rs` is the sole normal-build physical acquisition owner.
It exposes package-private, read-only vocabulary:

```text
CanonicalRecordLeaf =
    Absent
  | Exact {
        path: CanonicalRecordPath,
        bytes: ImmutableBytes,
        digest: Sha256Digest,
    }

CanonicalMergeLocations {
  open: CanonicalRecordLeaf,
  archived: CanonicalRecordLeaf,
}

acquire_canonical_merge_locations(root, merge_id)
  -> ModelResult<CanonicalMergeLocations>
```

`CanonicalRecordPath` is opaque and distinguishes the open
`.gwz/merge/<merge-id>.yaml` leaf from the archived
`.gwz/merge/done/<merge-id>.yaml` leaf. Acquisition validates `merge_id`
before path construction, canonicalizes the workspace root, walks every
existing parent with `symlink_metadata`, requires real directories, and reads
only regular non-symlink named leaves. A missing leaf is `Absent`; a missing
archive `done` directory means the archive leaf is absent. An unreadable,
nonregular, symlinked, escaped, or changed-during-read leaf is an error rather
than `Absent`. Exact bytes and their digest are returned together; consumers
do not reopen through a weaker path helper.

The production status acquisition owner applies one closed arbitration table:

| Open | Archived | Result |
| --- | --- | --- |
| absent | absent | operation not found |
| exact valid record | absent | project the open record with `archived=false` |
| absent | exact valid terminal v0 | project the archived record with `archived=true` |
| exact valid terminal record | exact byte-identical terminal record | the open source remains authoritative and projects with `archived=false` |
| nonterminal open plus any archive | any | contradictory/recovery-required |
| malformed/unsupported source or destination | any | its typed decode error; no fallback |
| two exact but byte-different copies | exact | contradictory/recovery-required |

In a normal build the open decoder remains the released v0 decoder and the
archive decoder is `decode_archived_v0`; neither path compiles a v1 body
decoder. P4's disabled v1 lifecycle reuses the same physical acquisition and
substitutes only the test-gated decoder after location authority is established.

Optimistic status observation binds to the complete acquired source identity,
including which leaf was authoritative and its digest. After collecting all
live Git/root facts, it reruns the same canonical acquisition and requires the
same source and arbitration result. Source disappearance, appearance,
replacement, or open/archive role change during that reread is normalized to
the existing typed contention/recovery result, never leaked as an incidental
`IoError`. The implementation discards all live facts and retries the complete
acquisition/observation once; a second change returns the typed error. P3 does
not duplicate path acquisition or weaken this lineage check.

## 5. Version-neutral borrowed status observation

Add a non-serializable, immutable `MergeStatusRecordView<'a>` borrowing only
the common status inputs:

```text
workspace_id, merge_id, state, baseline,
operation_id, source_ref,
selected_targets, participants, publication, operation_drift
```

It has explicit constructors from v0 and, behind the disabled v1 boundary,
v1. It owns no data, exposes no extensions or pending v1 journals, and cannot
be converted back to either record.

Extract one `observe_status_view(backend, root, view)` implementation that
returns owned live facts: participant observations, ordered operation drift,
root-finalization exactness, and interrupted-evidence-rollback normalization.
The released v0 `snapshot_status` becomes a wrapper that combines those facts
with its original owned v0 record. P3 combines the same facts with the checked
v1 record projection.

`operation_id` and `source_ref` are mandatory because exact root-evidence
observation reconstructs `composition_message(source_ref, merge_id,
operation_id)`. Neither constructor may omit them, and root evidence may not
drop the exact composition-message check or reach back to an owned record.

Candidate-prefix classification, prefix allowance, root evidence, exact root
finalization, and interrupted evidence rollback consume the borrowed view (or
smaller borrowed parts derived from it). Existing v0 entry points remain thin
wrappers over those single implementations. There is no v1-to-v0 conversion,
no second root/publication classifier, and no protocol type in the observation
layer.

Optimistic lineage remains: load checked v1 source, collect the complete live
view, reread unchanged source, and only then project. A changed source discards
all live facts and retries the whole observation once or returns the existing
typed contention/recovery error.

## 6. Exact direct-ref GC preflight

Extend the read-only Git fact seam with:

```text
GitDirectRefObservation =
    Absent
  | Direct { target: GitObjectId }
  | NonDirect

observe_direct_ref(path, name)
  -> ModelResult<GitDirectRefObservation>
```

The implementation inspects the named ref itself; it does not use a resolving
revspec as evidence of directness. Symrefs and any other non-direct form return
`NonDirect`, even when they resolve to the recorded commit.

P4 performs the complete all-repository preflight before deletion:

- `Absent`: already complete;
- `Direct(recorded target)`: eligible;
- `Direct(other target)`, `NonDirect`, or unavailable repository: reject while
  every ref and the archive remain untouched.

`delete_backup_ref_checked` immediately rechecks the same exact direct-ref fact
before each deletion, closing the preflight/execution race. A change after
preflight may leave an earlier proper subset deleted, but it cannot delete the
changed ref; restart treats absent refs as complete and repeats full preflight
from the unchanged archive.

Immediately before archive unlink, `require_backup_refs_absent` rereads every
named ref through `observe_direct_ref`. Only `Absent` satisfies final
exhaustion. `Direct` with any target, `NonDirect` (including resolving and
broken symbolic refs), or an unavailable repository retains the archive. The
same rule applies to the retained v0 GC wrapper. Tests install both a resolving
and broken symbolic ref during initial preflight and after a successful
deletion but before the final archive check.

## 7. Manifest reconciliation

### 7.1 Exact P0.1 production manifest

The shared correction is charged for exactly these 60 production paths. A
path may be removed if unused; adding or substituting one requires lead review
and a ledger update before the edit.

```text
gwz-core/src/workspace_ops/merge/model/v1/journal.rs
gwz-core/src/workspace_ops/merge/model/v1/record.rs
gwz-core/src/workspace_ops/merge/model/v1/canonical.rs
gwz-core/src/workspace_ops/merge/model/v1/validate/common.rs
gwz-core/src/workspace_ops/merge/model/v1/validate/preservation.rs
gwz-core/src/workspace_ops/merge/model/v1/validate/publication.rs
gwz-core/src/workspace_ops/merge/record_wire/open_v0/adapter.rs
gwz-core/src/workspace_ops/merge/record_wire/open_v0/structural.rs
gwz-core/src/workspace_ops/merge/record_wire/archive/v0_evidence.rs
gwz-core/src/workspace_ops/merge/record_wire/unknown_fields/mod.rs
gwz-core/src/workspace_ops/merge/record_wire/unknown_fields/extract/mod.rs
gwz-core/src/workspace_ops/merge/record_wire/unknown_fields/extract/journals.rs
gwz-core/src/workspace_ops/merge/record_wire/unknown_fields/identity.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/transition/reduce/mod.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/transition/reduce/participant.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/transition/reduce/preservation.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/transition/footprint.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/binding.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/dispatcher.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/archive.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preserving_recovery.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rollback.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/reverse/preservation.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/reverse/rollback.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/reverse/execute/preservation.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/archive.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/store/mod.rs
gwz-core/src/workspace_ops/merge/abort/mod.rs
gwz-core/src/workspace_ops/merge/abort/preflight.rs
gwz-core/src/workspace_ops/merge/root.rs
gwz-core/src/workspace_ops/merge/abort/evidence.rs
gwz-core/src/workspace_ops/merge/root/abort.rs
gwz-core/src/workspace_ops/merge/model/mod.rs
gwz-core/src/workspace_ops/merge/model/archive_projection.rs
gwz-core/src/workspace_ops/merge/model/record_projection.rs
gwz-core/src/workspace_ops/merge/record_wire/mod.rs
gwz-core/src/workspace_ops/merge/record_wire/location.rs
gwz-core/src/workspace_ops/merge/record_wire/archive/mod.rs
gwz-core/src/workspace_ops/merge/record_wire/archive/cleanup.rs
gwz-core/src/workspace_ops/merge/record_wire/archive/v0.rs
gwz-core/src/workspace_ops/merge/record_wire/archive/v1.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/archive_result.rs
gwz-core/src/workspace_ops/merge/status/mod.rs
gwz-core/src/workspace_ops/merge/status/view.rs
gwz-core/src/workspace_ops/merge/status/snapshot.rs
gwz-core/src/workspace_ops/merge/publication.rs
gwz-core/src/workspace_ops/merge/acceptance/mod.rs
gwz-core/src/workspace_ops/merge/acceptance/publication.rs
gwz-core/src/workspace_ops/merge/root/finalization.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/status.rs
gwz-core/src/git/gitbackend.rs
gwz-core/src/git/gitbackend/contract.rs
gwz-core/src/git/gitbackend/preservation.rs
gwz-core/src/workspace_ops/merge/gc.rs
gwz-core/src/workspace_ops/merge/store/gc.rs
```

`authority/observe/archive.rs`, `record_wire/location.rs`, and
`status/view.rs` are the three planned new cohesion owners. They target fewer
than 500 lines. The amendment does not turn any existing sub-500-line owner
into a policy dumping ground; any such concentration triggers another path and
budget review even below the general 1,000-line threshold.

### 7.2 Exact P0.1 test/tool/doc manifest

The shared correction is charged for exactly these 46 evidence paths:

```text
gwz-core/src/workspace_ops/merge/model/v1/validate/preservation_tests.rs
gwz-core/src/workspace_ops/merge/model/v1/validate/common_tests.rs
gwz-core/src/workspace_ops/merge/model/v1/validate/journal_tests.rs
gwz-core/src/workspace_ops/merge/model/v1/validate/canonical_tests.rs
gwz-core/src/workspace_ops/merge/model/v1/tests.rs
gwz-core/src/workspace_ops/merge/record_wire/unknown_fields/tests.rs
gwz-core/src/workspace_ops/merge/record_wire/unknown_fields/tests/overlay_regressions.rs
gwz-core/src/workspace_ops/merge/record_wire/unknown_fields/tests/v0.rs
gwz-core/src/workspace_ops/merge/record_wire/unknown_fields/tests/nulls.rs
gwz-core/src/workspace_ops/merge/record_wire/archive/tests.rs
gwz-core/src/workspace_ops/merge/record_wire/archive/tests/fixtures.rs
gwz-core/src/workspace_ops/merge/record_wire/archive/tests/v0.rs
gwz-core/src/workspace_ops/merge/record_wire/archive/tests/v1.rs
gwz-core/src/workspace_ops/merge/record_wire/tests/header_dispatch.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/store.rs
gwz-core/src/workspace_ops/merge/status/tests/support.rs
gwz-core/src/git/tests/g15.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reducer.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/authority.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/effect.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/prefixed_preservation.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/fixtures.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_entry.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_router.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/mod.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/entry.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/phases.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/recovery.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/real_git.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/faults.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/mod.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/phases.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/recovery.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/root_artifacts.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/status.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/archive.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/archive_result.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/gc.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/service_sequence.rs
dev-docs/GwzM5-8R4bInterfaceAmendment-1.md
dev-docs/GwzM5-8R4bInterfaceAmendment-ReviewState.md
dev-docs/GwzM5-8R4bInterfaceAmendment-ReviewArch.md
dev-docs/GwzM5-8ChangeBudget.md
dev-docs/GwzM5-8R4bReverseLifecycleInterface.md
dev-docs/GwzM5-8R4bTransitionDesign.md
dev-docs/GwzM5-8Refactor.md
```

P3 additionally owns these four unavoidable constructor/parity evidence paths,
which raise its test/tool/doc path charge from 13 to 17 without raising its
2,500-line ceiling:

```text
gwz-py/src/tests/test_cli_merge.py
gwz-py/src/tests/test_codec.py
gwz-py/src/tests/test_bridge_transport.py
gwz-cli/tests/fixtures/merge_status_human.txt
```

### 7.3 Reconciled slice and aggregate ceilings

These are stop ceilings, not targets. Generated Rust/Python LOC remains
excluded, but generated paths remain counted. Shared paths are charged to each
slice that changes them; the unique ceiling is the set union.

| Slice | Old production ceiling | Measured current charge at pause | New production ceiling | Moved ceiling | Test/tool/doc current | Test/tool/doc ceiling | Production paths | Test/tool/doc paths |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| P0 closed prerequisite | 1,200 | 1,180 | 1,200 | 0 | 4,121 | 4,200 | 29 | 21 |
| P0.1 shared correction | 0 | 36 direct-ref seam lines | 1,400 | 0 | 1,818 docs plus shared direct-ref tests | 2,200 | 60 | 46 |
| P1 preservation/recovery | 950 | 508 | 1,350 | 1,050 | 100 | 1,800 | 9 | 8 |
| P2 rollback/recovery | 900 | conservative HEAD delta 1,941 | 2,200 | 650 | 842 | 1,800 | 9 | 8 |
| P3 status/protocol | 900 | 1,096 | 1,250 | 250 | 801 | 2,500 | 18 | 17 |
| P4 archive/GC | 750 | 508 | 750 | 100 | 532 | 1,500 | 8 | 8 |
| aggregate wiring/fault/docs | 100 | not started | 100 | 0 | not started | 1,500 | 3 | 12 |
| **Charged package ceiling** | **4,800** |  | **8,250** | **2,050** |  | **15,500** | **136** | **120** |

The production ceiling increases by 1,400 for P0.1, 400 for the measured P1
correction, 1,300 for P2, and 350 for P3. Test/tool/doc increases only by
P0.1's 2,200; the four P3 paths fit its existing 2,500-line ceiling. The
conservative union is at most 93 production paths and 92 test/tool/doc paths,
compared with the prior 63 and 69. The final settled ledger reports actual net
additions, movement, generated-path charges, and unique paths from the
repository diff.

The two added common-view constructor paths raise the R4b-P production charge
and unique union by two. They do not raise the program-wide 137-path production
ceiling because accepted R3 already charges both physical paths. The added
`acceptance/mod.rs` parent re-export raises the package production union by one
but not the program union because accepted R4b-A already charges it. The added
`authority/binding.rs` source-location correction and existing authority test
raise the package unions by one each but not the program unions because
accepted R4b-TI/P0 already charge them. The added
`unknown_fields/tests/{v0,nulls}.rs` and `validate/common_tests.rs` charges
raise the R4b-P test charge and unique union by three. They do not raise the
program-wide 136-path conservative test/tool/doc ceiling because those same
paths are already charged by the accepted R3/TI model packages. Shared package
charging is additive; the aggregate set union counts each physical path once.

P2's conservative 1,941-line HEAD delta includes lines replacing accepted P0
skeletons because no trustworthy pre-lane byte snapshot exists; the ledger
deliberately does not fabricate a smaller attribution. Its 2,200 ceiling leaves
259 lines for the authority wrapper, final canonical exhaustion repair, and
review corrections. P3's 1,250 ceiling leaves 154 lines for the borrowed-view
and archive integration. P1's 1,350 ceiling leaves 878 lines for its six
remaining lifecycle owners; retaining 950 after its reviewed Git fact seam had
already consumed 472 would force concept collapse rather than control scope.
P4 remains within its original ceiling at 508 production and 532 test lines,
with five of eight production and four of eight test paths touched. The final
pre-implementation document charge is 1,778 lines: the exact
1,706-line three-file amendment/review count plus the 72-line net correction to
the four controlling documents. Both independent re-reviews are included. The
first landed implementation seam adds 36 shared production lines and 40 shared
test lines for exact direct-ref observation, producing the 1,818 current
test/tool/doc charge above. The ledger conservatively includes those deltas in
both P0.1 and P1 because the paths are shared.

Any new lifecycle variant, durable phase, public protocol field, normal-build
v1 decoder, consumer-visible proof constructor, path outside these manifests,
or ceiling breach requires another stop and independent review.

## 8. Acceptance gate

Implementation resumes only after two independent reviewers both confirm:

1. the durable preservation handoff prevents `Pre`/`Staged` adoption across
   every restart;
2. preserve-then-rollback retains that immutable handoff while direct rollback
   uses a strict write-ahead per-step baseline matrix;
3. observation binding remains authority-owned;
4. the new durable field is closed through canonical state, unknown-field
   extraction/identity/overlay, and rejection of the fifth v0-to-v1 collision;
5. normal builds acquire and accept canonical archived v0 but reject v1 before
   A1 without compiling a v1 body decoder;
6. open v0/v1 status shares one borrowed root/publication observer including
   the exact operation/source inputs, and optimistic reread normalizes source
   races to the typed contention result;
7. GC full preflight, every immediate deletion recheck, and final exhaustion
   reject non-direct refs;
8. v0 serialization and retained-reader behavior are unchanged; and
9. the exact manifest and reconciled slice/package/unique budgets are
   sufficient.

After implementation, the same two reviewers re-review the settled tree before
R4b-G integration.

Both pre-implementation re-reviews now report GO with no open P0-P3 finding,
so this gate is satisfied. The measured 1,778/2,200 pre-implementation document
charge and revised manifests/ceilings were frozen before production work. The
implementation-start audit then added the two exact common-view constructor
paths above; both reviewers must confirm that bounded manifest correction
before their edits land.
