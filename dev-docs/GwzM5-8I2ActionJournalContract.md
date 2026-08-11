# GWZ I2 v1 Action-Journal Contract

Date: 2026-08-04

Status: **accepted; R4a unblocked and R3 remains sequenced after R4a**

Amended 2026-08-11 by
`GwzM5-8R4bInterfaceAmendment-1.md` and
`GwzM5-8R4bInterfaceAmendment-2.md`: the durable publication handoff and the
flattened 11-phase stash/10-phase reset graphs below supersede the original
compound root phases. This document is wire-authoritative only as amended by
those two accepted interfaces.

Amended 2026-08-09 during the R4b architecture review: selected-root metadata
rollback is explicitly legal before acceptance when the selected participant
and exact operation-baseline bytes provide the frozen authority. Exact
rollback and preservation cursor ownership is also clarified below. These are
validator/observation corrections with no v1 wire change.

This slice freezes v1 recovery origin, rollback, preservation, and archived
backup-ref cleanup ownership. The v1 record contains optional
`recovery_context`, `pending_rollback`, and `pending_preservation` fields.

## 1. Exact wire types

```rust
struct RecoveryContextV1 { origin_state: RecoveryOriginStateV1 }

#[serde(rename_all = "snake_case")]
enum RecoveryOriginStateV1 {
    Executing, AwaitingResolution, Halted, Finalizing,
    Preserving, RollingBack,
}

#[serde(tag = "kind", rename_all = "snake_case")]
enum PendingRollbackActionV1 {
    Participant {
        member_id: String,
        action: ParticipantRollbackKindV1,
        terminal_state: ParticipantState,
    },
    PublicationEvidence { next_step: EvidenceRollbackStepV1 },
    SelectedRootMetadata { next_step: RootMetadataRollbackStepV1 },
}

#[serde(rename_all = "snake_case")]
enum ParticipantRollbackKindV1 { AbortConflict, ResetIntegrated }
#[serde(rename_all = "snake_case")]
enum EvidenceRollbackStepV1 {
    EvidenceCommit, Boundary, Lock, Marker, Index, Complete,
}
#[serde(rename_all = "snake_case")]
enum RootMetadataRollbackStepV1 { Manifest, Lock, Complete }

#[serde(tag = "kind", rename_all = "snake_case")]
enum PendingPreservationActionV1 {
    BackupRef {
        owner: PreservationOwnerV1,
        name: String,
        target_commit: String,
    },
    Stash {
        owner: PreservationOwnerV1,
        phase: PreservationStashPhaseV1,
        stash_id: Option<String>,
        stash_object_id: Option<GitObjectIdV1>,
        message: String,
        head_commit: String,
        preimage_sha256: String,
        root_publication_handoff: Option<PreservationPublicationCandidateV1>,
    },
    ResetAttachedRef {
        owner: PreservationOwnerV1,
        branch: String,
        expected_commit: String,
        restore_commit: String,
        phase: PreservationRefResetPhaseV1,
        root_publication_handoff: Option<PreservationPublicationCandidateV1>,
    },
}

#[serde(tag = "kind", rename_all = "snake_case")]
enum PreservationOwnerV1 {
    Participant { member_id: String },
    PublicationRoot,
}
#[serde(rename_all = "snake_case")]
enum PreservationStashPhaseV1 {
    NormalizeParent, NormalizeMarker, NormalizeLock, NormalizeIndex,
    CreateStash, RestoreIndex, RestoreLock, RestoreParent, RestoreMarker,
    WriteBundle, Complete,
}
#[serde(rename_all = "snake_case")]
enum PreservationRefResetPhaseV1 {
    PrepareParent, PrepareMarker, PrepareLock, PrepareIndex, ResetRef,
    RestoreIndex, RestoreLock, RestoreParent, RestoreMarker, Complete,
}
struct GitObjectIdV1 { algorithm: GitObjectAlgorithmV1, digest_hex: String }
#[serde(rename_all = "snake_case")]
enum GitObjectAlgorithmV1 { Sha1, Sha256 }
#[serde(rename_all = "snake_case")]
enum PublicationPrefixV1 { Baseline, Marker, Lock, Boundary }
#[serde(rename_all = "snake_case")]
enum PublicationIndexFormV1 { Pre, Staged }
struct PreservationPublicationCandidateV1 {
    prefix: PublicationPrefixV1,
    index: PublicationIndexFormV1,
}
```

## 2. Recovery and legality

`recovery_context` is required exactly while `state: recovery_required` and is
absent otherwise. A v1 transition writes the prior non-recovery state
atomically with `RecoveryRequired`; leaving recovery clears the context in the
same write. Base phase and resume action are derived and cross-checked, not
duplicated. A1 does not migrate v0 `RecoveryRequired` rows; they remain
valid-unlisted v0 and use the existing v0 lifecycle. A v1 recovery row is valid
only when exactly one origin, base, and resume action are proven.

“Prior” is literal durable history, not a state inferred only from participant
facts. If `BeginExecution` produces `Executing` while a retained failed/error
halt cause and pending owner remain, an ambiguous observation first persists
the ordinary `Halted` transition. The owner is then reobserved against those
new bytes before recovery may record `origin_state: halted`. A direct durable
edge from `Executing` to `RecoveryRequired` with a derived `Halted` origin is
invalid.

Only one pending rollback and one pending preservation action may exist, and
they are mutually exclusive:

| Record state | Pending action legality |
| --- | --- |
| `rolling_back` | zero or one rollback action; preservation absent |
| `preserving` | zero or one preservation action; rollback absent |
| recovery from rolling back | prior rollback action remains unchanged |
| recovery from preserving | prior preservation action remains unchanged |
| every other recovery origin | neither pending reverse action is legal |
| direct or terminal state | neither pending reverse action is legal |

An owner/kind must agree with participant, publication, acceptance, and
operation-baseline evidence. Ambiguous observation changes only operation
state/context; it never clears or advances a pending action. Retirement occurs
only with the verified result/progress write.

For rollback, owner/kind agreement includes the exact deterministic current
cursor derived from durable participant terminal states, publication evidence,
selected-root membership, and pending phase; a wrong or later owner is invalid
at decode. Preservation has an additional live condition because this wire
format intentionally persists neither no-op owner skips nor a separate reset
completion bit. Record validation rejects owner/phase/pass contradictions.
Before a pending preservation action is classified, advanced, or executed, an
exact bound observation must also prove that every earlier position in the
two-pass cursor is complete or currently unnecessary. A later pending owner
with any earlier incomplete or ambiguous position authorizes no rewrite or
physical mutation. This clarifies the existing deterministic order; it adds no
field or journal variant.

All free-looking wire strings are validated derivations, never ambient
authority:

- a participant backup ref is exactly
  `refs/gwz/merge/<merge-id>/<member-id>/head`, except a root participant uses
  the key `root`; `PublicationRoot` also uses the key `root`, and the two root
  owners may not coexist or collide;
- a backup target equals the observed attached target ref at preflight, is
  equal to or a descendant of the immutable owner anchor, and the owner anchor
  is the participant result (the composition commit for a selected-root
  participant after composition) or the publication composition commit;
- a stash id is exactly `stash_<merge-id>` and its native message is exactly
  `gwz:stash_<merge-id>: merge preservation`; the owner selects the repository,
  so at most one such stash is legal per repository;
- a participant reset ref is exactly its recorded target branch;
  `PublicationRoot` uses the immutable candidate publication branch; and
- `expected_commit` is the exact recorded backup target and `restore_commit`
  is the immutable owner anchor above.

Every id/ref/branch is validated with the existing canonical path/ref
validators before the journal write. A mismatch, duplicate derived name,
owner collision, non-descendant backup target, or action payload not derivable
from the immutable record and exact preflight observation is typed evidence
mismatch and authorizes no mutation.

## 3. Rollback ownership

Before each reverse action, its complete intent is persisted and reread.
Observation is exactly not-started, completed-exactly, or ambiguous. Exact
completion advances/clears the action atomically with the result; ambiguity
returns the typed rollback/recovery code without further mutation.

Participant inputs come from the named immutable participant row.
`AbortConflict` requires terminal `aborted`; `ResetIntegrated` requires
terminal `rolled_back`. Publication rollback consumes the immutable candidate
and composition owner, advancing only:

```text
evidence_commit -> boundary -> lock -> marker -> index -> complete
```

Selected-root metadata rollback consumes exact operation-baseline bytes and
advances only `manifest -> lock -> complete`. Skipping, reversing, changing
payload, or mutating from operation state alone is forbidden.

This rollback is legal before accepted workspace exists. Its authority is the
selected `@root` participant, exact persisted operation-baseline manifest/lock
bytes and hashes, and completion of any prior publication-evidence rollback.
When acceptance is present, its selected-root metadata source must agree and
is an additional cross-check; acceptance is not the source of the baseline
rollback bytes. Requiring acceptance unconditionally would strand a selected
root when abort begins after participant integration but before
`FreezeAcceptance`.

Each participant variant has a closed repository observation. For
`ResetIntegrated`, not-started is the exact attached target ref/HEAD at the
recorded result with the clean result index/worktree; completed is that same
ref/HEAD at `before_commit` with the exact clean before index/worktree. For
`AbortConflict`, not-started is the exact recorded native merge state,
MERGE_HEAD, conflict index, and worktree; completed is attached target
ref/HEAD at `before_commit`, no native integration state, and exact clean
before index/worktree. Any dirty, mixed, foreign, detached, or third state is
ambiguous and authorizes no reset/abort or terminal-row write.

## 4. Preservation ownership

Backup-ref creation is owned by exact owner/name/target. Absent means
not-started, exact target means completed, and any other ref is ambiguous.

Checked reset of the same existing attached integration ref advances only:

```text
with a root handoff:
prepare_parent -> prepare_marker -> prepare_lock -> prepare_index
-> reset_ref -> restore_index -> restore_lock -> restore_parent
-> restore_marker -> complete

without a root handoff:
reset_ref -> complete
```

The extended graph is used only when `root_publication_handoff` is present.
Before reset, the exact ref must equal `expected_commit`; after reset it must
equal `restore_commit`. Any root-repository action with a handoff, including
`Participant { member_id: "@root" }` and `PublicationRoot`, may have reset the
root index/worktree, so the recorded prefix and immutable candidate/baseline
data own exact restoration before completion. Each marker-parent, marker,
lock, and raw-index step is an independent durable phase. A crash after checked
ref reset but before its phase write is recognized only by the exact restored
ref plus the exact phase-local root form; a foreign ref or any mixed root state
is ambiguous. The action clears only in the same write that records complete
checked root restoration.

For every repository, `ResetRef` not-started additionally requires exact
attached HEAD/ref at `expected_commit` and the clean post-stash index/worktree;
completed requires exact attached HEAD/ref at `restore_commit` and the clean
anchor index/worktree. Root publication restoration is an additional root-only
phase, not a substitute for this repository classifier. A dirty or mixed
index/worktree is ambiguous even when the ref target happens to match.

Stash phases advance only:

```text
with a root handoff:
normalize_parent -> normalize_marker -> normalize_lock -> normalize_index
-> create_stash -> restore_index -> restore_lock -> restore_parent
-> restore_marker -> write_bundle -> complete

without a root handoff:
create_stash -> write_bundle -> complete
```

The root-only phases are omitted only when no root handoff exists. The exact
owned stash id/object id, message, and decoded stash trees must agree with HEAD
and the preimage fingerprint; missing, duplicate, foreign, or disagreeing
stashes are ambiguous, never adopted. Before a `Stash` action is written,
preflight rejects any index entry with assume-valid, skip-worktree, or
intent-to-add set: native stash objects do not retain those flags, so they
cannot be losslessly adopted after a crash. `CreateStash` is persisted with
absent stash ids before mutation. Its result, including both ids, is persisted
and reread atomically with advancement to `RestoreIndex` or `WriteBundle`. A crash
between creation and that write may adopt only one exact message/HEAD/preimage
match. Not-started also requires the current index/worktree still equal the
recorded preimage. Completed-exactly requires unchanged attached HEAD/ref plus
the exact clean post-stash index/worktree (or the exact normalized root form);
matching stash identity with residual or new work is ambiguous. `WriteBundle`
is illegal until both ids are persisted and verified; the
bundle is generated only from that recorded object id. `Complete` retains both
ids. SHA-1 ids contain 40 lowercase hex characters and SHA-256 ids contain 64.

Phase cross-fields are exact. Every normalize phase is legal only with a root
handoff and both stash ids absent. `CreateStash` has both ids absent. Every
restore phase, `WriteBundle`, and `Complete` requires both ids present and
equal to verified native evidence; restore phases are legal only with a root
handoff. Normalize steps move one managed object at a time from the exact
handoff form to the exact attached-clean form; restore steps reverse one
managed object at a time after the stash result is durable. Reset preparation
uses the same handoff-to-attached-clean direction, and post-reset restoration
moves from the exact restore-clean form back to the handoff. Exact before means
not started, exact after means completed, the required empty-parent causal case
is `AfterNeedsDurability`, and every mixed/third form is ambiguous and
authorizes no mutation.

`WriteBundle` also has a closed crash classifier. Before is either no bundle or
the exact canonical bundle containing only already completed owner rows. After
is the exact canonical bundle with this owner/object row added. An existing
bundle with any other bytes, semantic field, owner row, or order is ambiguous
and is never overwritten. Bundle identity is derived from workspace id,
`stash_<merge-id>`, record creation time, fixed message suffix `merge
preservation`, `include_untracked: true`, and `include_ignored: false`.
Selected owners and member rows sort by stable owner id; every row path,
branch/head, full message, object id, lifecycle value, and dirty summary is
derived from the journal owner and decoded canonical preimage. The bundle is
written atomically and the phase advances only with an exact reread.

`preimage_sha256` is SHA-256 over this canonical binary grammar. All counts and
variable-byte lengths are unsigned `u64` big-endian; all fields appear in the
listed order and no alignment/padding bytes exist:

```text
frame = ASCII("gwz.merge-preservation-preimage/v1") 0x00
        u64(entry_count) entry*
entry = u64(path_len) raw_path entry_kind u64(index_count) index* worktree
index = stage u32(mode) u32(semantic_flags) object_id
object_id = algorithm digest_len raw_digest
worktree = presence [worktree_kind executable payload]
```

- entries sort by raw Git path bytes, with no UTF-8 decoding/normalization;
  equal paths reject;
- `entry_kind` is `0` for index-present (`index_count > 0`) and `1` for
  worktree-only/untracked (`index_count == 0` and worktree present); no other
  value or combination is legal;
- index rows sort by unique stage; `stage` is one byte `0..=3`; mode is `u32`
  big-endian; semantic flags are `u32` big-endian and must be zero. Bits 0, 1,
  and 2 identify assume-valid, skip-worktree, and intent-to-add for preflight
  rejection, and any other set bit also rejects;
- `object_id` uses algorithm byte `1` for SHA-1 or `2` for SHA-256, followed by
  digest-length byte `20` or `32` respectively and exactly those raw digest
  bytes; every other tag/length pairing rejects;
- worktree `presence` is `0` for absent, with no following bytes, or `1` for
  present. A present `worktree_kind` is `0` regular, `1` symlink, or `2`
  gitlink. `executable` is one byte `0` or `1`; symlink/gitlink require `0`;
- regular payload is `u64(byte_len)` then the 32 raw bytes of SHA-256 over file
  bytes; symlink payload is `u64(target_len)` then the 32 raw bytes of SHA-256
  over the raw symlink target; gitlink payload is one `object_id`. No payload
  fields belonging to another kind are emitted; and
- included untracked directories are flattened and ignored files are absent.

The empty frame hex is
`67777a2e6d657267652d707265736572766174696f6e2d707265696d6167652f7631000000000000000000`
and its SHA-256 is
`72334dbbc68c9cf295c86199f84184b8c5f03c8d7cc28aaad3207f363a131e34`.
R3 adds golden byte vectors and hashes for every entry/worktree/object-id tag,
stage, executable value, and non-UTF-8 ordering case. YAML/JSON/string rendering
is forbidden as hash input.

## 5. Archived cleanup ownership

The immutable validated archive is the cleanup worklist; terminal history is
not rewritten. For every recorded backup ref:

- before observing or deleting it, derive and require the exact private name
  `refs/gwz/merge/<merge-id>/<owner-key>/head` from the archive merge id and
  stable owner (`root` for either root owner), and reject duplicate names,
  duplicate owners, and selected-root/publication-root collisions;

- absent is already complete;
- the exact target authorizes checked deletion; and
- a different target or unavailable repository stops cleanup and retains the
  archive.

The archive may be deleted only after every recorded ref is observed absent.
Native stashes and stash bundles are never deleted by merge GC. An archive
with no backup refs may be deleted after archive-only validation.

## 6. Exit tests

R3 remains blocked until the implementation plan names tests for every origin,
legality cell, action/step, and before/after-mutation fault; exact and ambiguous
stash/ref/root observations including non-UTF-8 paths; and partial multi-ref
cleanup proving the archive remains until all refs are absent.
