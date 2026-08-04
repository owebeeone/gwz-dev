# GWZ I2 v1 Record and Acceptance Contract

Date: 2026-08-04

Status: **accepted; R4a unblocked and R3 remains sequenced after R4a**

This document freezes the durable v1 body and the canonical acceptance model
authorized by I1. `GwzM5-8Refactor.md` remains the behavioral authority. This
contract resolves its remaining wire-shape choices without adding M6 branch
lifecycle, M7 snapshot provenance, or M8 optional/partial participation.

## 1. Versioned body and record location

The exact v1 envelope is:

```yaml
schema: gwz.merge-operation/v1
record_schema_version: 1
```

`MergeOperationRecordV1` retains every v0 field, field name, enum spelling,
and nested container at the same YAML path. It adds four top-level fields:

```rust
struct MergeOperationRecordV1 {
    // The complete MergeOperationRecordV0 body follows unchanged, except for
    // the v1 envelope and v1 validation of mode: no_ff.
    accepted_workspace: Option<AcceptedWorkspaceV1>,
    recovery_context: Option<RecoveryContextV1>,
    pending_rollback: Option<PendingRollbackActionV1>,
    pending_preservation: Option<PendingPreservationActionV1>,
}
```

Each new field is omitted while absent and is stored in the existing open or done
record at `.gwz/merge/<merge-id>.yaml` or
`.gwz/merge/done/<merge-id>.yaml`. There is no sibling acceptance file,
sidecar journal, cleanup journal, or record self-hash. The pending fields are
the write-ahead owners for one in-flight reverse or preservation action; they
are not generic action bags.

The v1 body does not contain a generic versioned payload, capability list,
future projection, M6 branch action/checkout restoration, M7 snapshot
identity, M8 participation policy/skip outcome, or partial-lock rule.

New A1 operations write all baseline fields produced by the current planner,
including exact `lock_yaml`, `manifest_yaml`, root checkout, and available
committed artifact hashes. A migrated v0 row must also contain exact baseline
lock and manifest bytes before v1 publication. When v0 omitted either field,
the adapter may recover the bytes only from the exact M4 sources in §3,
verify them against every available frozen digest, and persist them in the
staged v1 body. If it cannot, migration is ineligible. This is verified
evidence recovery, not unverified live-state adoption.

## 2. Exact accepted-workspace shape

The v1 durable types are:

```rust
struct AcceptedWorkspaceV1 {
    operation_baseline_lock_sha256: String,
    metadata_base: AcceptedMetadataBaseV1,
    lock: AcceptedLockV1,
    member_audit: BTreeMap<String, MemberAcceptanceV1>,
    root: RootPublicationInputV1,
}

struct AcceptedMetadataBaseV1 {
    source: AcceptedMetadataSourceV1,
    manifest_exact_yaml: String,
    manifest_sha256: String,
    lock_exact_yaml: String,
    lock_sha256: String,
}

#[serde(tag = "kind", rename_all = "snake_case")]
enum AcceptedMetadataSourceV1 {
    OperationBaseline,
    SelectedRootResult { commit: String },
}

struct AcceptedLockV1 {
    exact_yaml: String,
    sha256: String,
}

#[serde(tag = "kind", rename_all = "snake_case")]
enum MemberAcceptanceV1 {
    Selected {
        integration: AcceptedIntegrationRefV1,
        final_checkout: AcceptedAttachedCheckoutV1,
        lock_member: AcceptedLockMemberV1,
    },
    UnselectedPresent {
        lock_member: AcceptedLockMemberV1,
    },
    Absent,
}

struct AcceptedIntegrationRefV1 {
    branch: String,
    before_commit: String,
    resulting_commit: String,
}

struct AcceptedAttachedCheckoutV1 {
    branch: String,
    commit: String,
}

struct AcceptedLockMemberV1 {
    path: String,
    source_id: String,
    source_kind: ArtifactSourceKind,
    commit: Option<String>,
    branch: Option<String>,
    detached: Option<bool>,
    upstream: Option<String>,
    dirty: Option<bool>,
    materialized: Option<bool>,
    #[serde(flatten)]
    extensions: BTreeMap<String, serde_yaml::Value>,
}

struct RootPublicationInputV1 {
    base: AcceptedRootBaseV1,
    publication_branch: Option<String>,
    baseline_artifact_hashes: RootArtifactHashesV1,
}

#[serde(tag = "kind", rename_all = "snake_case")]
enum AcceptedRootBaseV1 {
    BornAttached {
        commit: String,
        symbolic_branch: String,
    },
    BornDetached {
        commit: String,
    },
    UnbornAttached {
        symbolic_branch: String,
    },
}

struct RootArtifactHashesV1 {
    lock_worktree_sha256: String,
    manifest_worktree_sha256: String,
    lock_commit_sha256: Option<String>,
    manifest_commit_sha256: Option<String>,
}
```

All object identifiers, branches, paths, workspace/member/source identifiers,
and SHA-256 strings use the validators already applied to their v0 source
fields. SHA-256 values are lowercase 64-character hexadecimal strings.
Integration, checkout, lock-row, symbolic, and publication branches are
canonical short local names such as `main`, never `refs/heads/main`; internal
Git calls construct the full ref without changing persisted/protocol meaning.

The tagged `UnselectedPresent` and `Absent` variants deliberately contain no
`final_checkout`. This is the wire representation for “no selected checkout
evidence”; it cannot be confused with a selected participant whose checkout
field was accidentally omitted. An unselected row's complete lock member is
the only checkout-related evidence claimed for that row.

`Absent` serializes only `kind: absent`. It carries no lock row, checkout,
integration, participation, skip, or reason field.

## 3. Accepted lock and audit invariants

Acceptance validates all of these conditions before its first write and after
every decode:

1. `operation_baseline_lock_sha256 == record.baseline.lock_sha256`.
2. Both metadata-base byte strings match their SHA-256 values and parse as a
   valid `ManifestArtifact`/`LockArtifact` for `record.workspace_id`.
3. With no selected `@root`, metadata source is `OperationBaseline` and its
   exact bytes/hashes equal the persisted operation baseline. With selected
   `@root`, source is `SelectedRootResult`, its commit equals the verified root
   participant result, and both byte strings are read and verified from that
   exact commit before acceptance.
4. `sha256(lock.exact_yaml.as_bytes()) == lock.sha256`.
5. `lock.exact_yaml` parses as one complete, valid `LockArtifact` for
   `record.workspace_id`; it is never regenerated after being persisted.
6. The audit key set is exactly the union of:
   - member keys in the parsed metadata-base lock;
   - active member ids in the parsed metadata-base manifest;
   - selected non-root participant ids.
7. Every selected non-root target has exactly one `Selected` row. Its branch,
   before commit, and resulting commit equal the durable participant; its final
   checkout is the same branch/result; and its `lock_member` equals the parsed
   accepted-lock row.
8. Every unselected metadata-base member has exactly one `UnselectedPresent` row,
   even when inactive or unmaterialized. Its row equals the unchanged complete
   metadata-base row and the parsed accepted-lock row.
9. Every remaining audit-domain member is `Absent` and has no accepted-lock
   row.
10. The accepted lock's keys equal exactly the audit keys whose variant is
   `Selected` or `UnselectedPresent`.
11. Each selected lock row has the verified result commit, exact target branch,
   `detached: false`, `dirty: false`, and `materialized: true`; its path/source
   identity equals the frozen selected-participant identity and, when that
   member exists in the metadata-base manifest, the two identities agree.
12. Unknown fields inside `lock.exact_yaml` remain authoritative bytes. V1
    parses each raw member mapping into `AcceptedLockMemberV1`, retaining its
    flattened extensions; the duplicated audit row must equal that complete
    semantic mapping, including every unknown path and value.

The operation-baseline bytes needed by items 1–3 are persisted in
`baseline.lock_yaml` and `baseline.manifest_yaml`. For an
eligible legacy v0 row that lacks baseline lock or manifest bytes, the adapter
recovers them before staging v1 from this closed source order:

1. the selected root participant's exact `before_commit` tree, checked against
   `lock_commit_sha256`/`manifest_commit_sha256` and the worktree digests;
2. for an unselected root, candidate `baseline_lock_yaml` when its hash equals
   `baseline.lock_sha256`, plus the unchanged live manifest only when its hash
   equals `baseline.manifest_sha256`; or
3. before any root evidence/publication mutation, the live lock and manifest
   only when both hashes equal the frozen worktree digests and the accepted
   root checkout is exact.

No candidate lock for a selected-root operation substitutes for the original
baseline lock. An unavailable, mutated, ambiguous, or digest-disagreeing
source makes migration ineligible. The verified bytes are persisted in the
staged v1 baseline so restart never repeats live recovery.

## 4. Root invariants

`RootPublicationInputV1` is input to publication, never publication output:

- a successful selected `@root` produces `BornAttached` whose commit and
  branch equal its verified result and target branch;
- an unselected root reproduces its exact frozen born-attached,
  born-detached, or attached-unborn checkout;
- `BornDetached` is invalid when `@root` was selected or when publication is
  required;
- `publication_branch` is `Some(symbolic_branch)` for both attached variants
  and `None` only for `BornDetached`;
- a publication step that creates evidence or candidate files requires an
  attached base and the exact `publication_branch`;
- evidence uses `Some(commit)` as parent for `BornAttached`, no parent for
  `UnbornAttached`, and is forbidden for `BornDetached`; and
- composition commit/tree, candidate bytes/hashes, marker, boundary, and
  publication prefix remain solely in `PublicationProgress`.

`RootArtifactHashesV1` is copied exactly from the frozen baseline. Worktree
hashes are always present. Commit hashes are present only when v0/M4 captured
them; a missing hash is not synthesized during migration. Selected-root
acceptance requires both committed hashes because its existing preflight
requires the root's baseline manifest and lock at the before commit.

## 5. Recovery and write-ahead action owners

V1 closes the v0 mutation-owner gaps through the exact recovery, rollback,
preservation, and cleanup types in `GwzM5-8I2ActionJournalContract.md`.

Those fields are validated as part of every v1 decode/rewrite. Their action
legality, ordered steps, canonical non-UTF-8-safe stash preimage framing, exact
restart observations, and archive-worklist cleanup rules are normative in the
companion contract. No mutation is authorized merely by operation state.

## 6. Acceptance lifetime and state legality

`accepted_workspace` is absent before complete participant validation. The
first accepted-workspace write is its own durable write and happens before
the publication-required decision or any evidence/candidate mutation.

Once present, the complete known value and every unknown field beneath it are
immutable through continue, preservation, rollback, abort, status,
finalization, completion, archival, and GC. No path may clear it, replace its
exact lock bytes, recompute its audit, or derive a new root input.

The closed v1 legality rules are:

| Lifecycle condition | Acceptance |
| --- | --- |
| executing, awaiting resolution, or halted | absent |
| preserving/rolling back/recovery-required before acceptance | absent |
| finalizing before the acceptance write | absent |
| finalizing after complete verification | present |
| any candidate/evidence/publication prefix exists | present |
| completed, including all-up-to-date/no-publication | present |
| aborted after accepted publication/preservation/rollback work | present |
| aborted before acceptance | absent |

`publication: None` with acceptance present means publication need has not yet
been durably classified. `publication.step: complete` with no candidate,
composition, or publication hashes means deterministic no-publication
completion. A candidate or publication-created output with acceptance absent
is invalid. A crash after the acceptance write resumes by consuming that exact
value and deriving only the next publication action.

Whenever `publication.candidate` exists in v1,
`candidate.lock_yaml == accepted_workspace.lock.exact_yaml` byte-for-byte,
`candidate_lock_sha256 == accepted_workspace.lock.sha256`, and the parsed
candidate lock, audit rows, and root branch agree with acceptance. Candidate
`baseline_lock_yaml` equals `metadata_base.lock_exact_yaml` byte-for-byte.
Candidate
marker members, sorted candidate hashes, evidence parent, composition
message/tree, `root_merge_commit`, publication branch, boundary bytes, and
marker/boundary hashes retain the existing M4 cross-field checks. A mismatch
is `CandidateIntegrityMismatch` before publication, record rewrite, or Git
mutation.

## 7. Closed v1 integration action semantics

V1 retains `PendingMergeAction`, `PendingMergeExpectedResult`,
`PendingCommitSpec`, and exact commit-message fields at their v0 YAML paths.
No new generic participant-action enum is added.

The v1 mode/action matrix is:

| Mode and analysis | Durable pending action |
| --- | --- |
| normal/ff-only, up to date | verify-up-to-date / unchanged / no commit spec |
| normal/ff-only, fast-forward | fast-forward / fast-forward / no commit spec |
| normal, clean true merge | true-merge / commit / exact commit spec |
| normal, conflicting true merge | true-merge / expected-conflict / no commit spec |
| no-ff, up to date | verify-up-to-date / unchanged / no commit spec |
| no-ff, fast-forwardable or clean true merge | true-merge / commit / exact two-parent commit spec |
| no-ff, conflicting true merge | true-merge / expected-conflict / no commit spec |
| resolved conflict in normal or no-ff | resolve-conflict / commit / exact commit spec |

`mode: no_ff` forbids a durable fast-forward action. Its commit action freezes
the existing intent fields, exact tree, author, committer, message, and the
two parents implied in order by `before_commit` and `source_commit`. Preparation,
restart, execution, reconciliation, and rollback all validate that pair and do
not reclassify it as a fast-forward.

V0 `mode: no_ff` remains invalid and is rejected before adaptation, migration,
record rewrite, or Git mutation.

## 8. Unknown fields and container retirement

Migration first records every v0 unknown YAML path and raw value. The v1 body
keeps all surviving v0 containers at the same path, so their unknown fields
remain at that exact path and value. A v0 unknown top-level field named
`accepted_workspace`, `recovery_context`, `pending_rollback`, or
`pending_preservation` collides with a v1 known field and makes migration
ineligible; it is never adopted, overwritten, or moved.

Current-version rewrites use this closed retirement table:

| Container | Rule |
| --- | --- |
| record, baseline, participant | survive for the record lifetime; unknown descendants survive |
| accepted workspace and descendants | immutable once present; unknown descendants survive |
| recovery context | exists only with `RecoveryRequired`; retires on the checked transition out of recovery |
| pending action and commit spec | survive while the same action is pending; retire only when its exact result is durably reconciled |
| pending rollback/preservation action | survives while its exact mutation is pending; retires only with the verified result/progress write |
| conflict paths/snapshot | survive while conflict evidence remains authoritative; retire when resolution or checked abort completes |
| participant/publication-root preservation evidence | one stable row per owner; the same row survives as ref/stash fields fill and survives archival |
| publication progress, candidate, hashes, root preservation | survive once created; never retired before archival/GC |
| participant or operation drift entry | survives while the same ordered durable drift fact remains; retires only when that fact is explicitly cleared |
| participant error | identity is code plus member/target fields and stable detail (diagnostic message text is not identity); unknowns survive only while that exact identity remains. Clear or replacement with a different identity retires the old container before the new typed error is written; unknowns never rebind across replacement |

An unknown field below a retired container retires with that container. No v1
retirement has a typed destination elsewhere. Reordering a sequence may not
attach an extension to a different semantic entry. Sequence rewrites match
unknown descendants by these exact identities before preserving their new
positions: conflict evidence `(path, sha256)`; preservation evidence by its
stable owner (`participant:<id>` or `publication-root`, with at most one row);
candidate hash `path`; participant drift by `(kind, every expected/live
field, occurrence index among identical keys)`; and operation drift by its
unique `kind`. Diagnostic message changes do not change drift identity.
Duplicate identities where uniqueness is required make the rewrite invalid
before publication. Unknown descendants of a removed identity retire only
under the table's corresponding legal transition.

Every write atomically publishes the known replacement plus the expected
surviving unknown manifest, rereads the exact bytes through the v1 decoder,
and compares both the canonical model and unknown manifest. A mismatch is a
pre-mutation recovery error.

## 9. Record-contract exit tests

R3 may begin only after independent review accepts this contract and the
implementation plan names tests for:

- duplicate-aware v1 envelope decoding and exact v1 round trips;
- every member-audit variant and all cross-field/domain contradictions;
- selected attached-only checkout and explicit unselected/no-checkout shapes;
- all three root bases, detached no-publication, and detached publication
  rejection;
- acceptance-first crashes, immutable restart consumption, and successful
  all-up-to-date archival;
- the complete normal/ff-only/no-ff action matrix and two-parent restart
  reconciliation;
- every recovery origin and every rollback/preservation action step with
  before/after-mutation faults and exact/ambiguous observation;
- partial multi-ref cleanup proving the immutable archive worklist is an
  idempotent owner and the archive remains until every ref is absent;
- unknown fields in every surviving and retiring container, including all
  four top-level v1 collisions; and
- atomic rewrite verification proving either complete old bytes or complete
  verified new bytes after every injected fault.
