# GWZ R4b Typed Transition Architecture

Date: 2026-08-09

Amended 2026-08-16 by `GwzM5-8ExactEvidencePlatformAmendment.md`:
recovery-grade rewrite edges (the checked rollback and native-abort
checkouts) are blob-exact (filters disabled), and the checked-artifact
private area is invisible to the preservation-image model and the recovery
cleanliness predicates.

Status: **accepted redesign; corrected R4b-TI/R4b-TR are implemented and
independently accepted; R4b-S and R4b-A are implemented behind the disabled
production boundary and independently accepted; the concrete R4b-F finalizer
and R4b-X participant/continue/recovery service are implemented behind that
boundary and independently accepted; R4b-P P0/P0.1 are independently
accepted and P2-P4 are implemented; P1 consumption exposed the compound
root-preservation gap corrected in `GwzM5-8R4bInterfaceAmendment-2.md`; both
independent reviewers accepted the interface through §13; the
production-disabled P0.2 implementation and bounded settled-code remediation
are accepted by both corrected-code reviews with no open P0-P3 finding; the
Windows portability remediation is code-GO; both §14 settled-implementation
reviews and the exact-SHA native release-platform run are GO; P1-WR1 is closed
and P1 is unpaused**

## 1. Decision

R4b will not extend the v0 lifecycle by passing a mutable
`MergeOperationRecordV1` through existing orchestration and validating the
record before a generic write. It will introduce a separate v1 lifecycle
boundary whose only durable rewrites are closed, typed transitions.

The stable R3/R4a implementation remains the baseline. The abandoned R4b
attempt introduced no committed code and is not an implementation substrate.
R4b is redesigned from the frozen I2 record and action-journal contracts.

This document is normative for R4b. It narrows the R4b package described in
`GwzM5-8Refactor.md`; the I2 contracts, as amended by
`GwzM5-8R4bInterfaceAmendment-1.md` and
`GwzM5-8R4bInterfaceAmendment-2.md`, remain authoritative for wire shapes,
field derivations, phase ordering, observation classifiers, and unknown-field
retirement.

No-wire I2 validation corrections are required before reducer work:

- selected-root baseline metadata rollback before `FreezeAcceptance` is owned
  by the selected root participant plus exact operation-baseline bytes.
  Acceptance is not required; when present it adds an exact cross-check. The
  committed validator currently requires acceptance unconditionally; and
- a v1 record contains at most one forward `pending_action` across all
  participants. It is legal only when the effective state is `Executing` or
  `Halted`; effective `Preserving`, including a `RecoveryRequired` record whose
  origin is `Preserving`, forbids every forward action. The released executor
  is sequential, and the redesigned abort/preserve compounds intentionally
  consume one exact owner rather than inventing multi-owner abandonment; and
- a pending rollback journal must name the exact record-derived current
  reverse cursor owner and legal phase. A pending preservation journal must
  satisfy every record-local owner/phase/order constraint and cannot be
  observed, advanced, or executed until a bound live cursor-prefix proof shows
  every earlier artifact/reset position complete or exactly unnecessary. The
  distinction is required because I2 deliberately does not persist no-op
  preservation skips or a separate reset-completion cursor; and
- rollback/publication phase validators use exact named sets and successors,
  never the existing ordinal `PublicationStep >= ...` shortcut.

R4b-TI must correct and exhaustively pin these existing M4/I2 shapes before
R4b-TR installs reducers. They add no field, phase, skip/partial-success
semantic, or v1 wire change.

## 2. Why the prior direction was rejected

The rejected direction had three architectural defects:

1. lifecycle callers could construct or mutate a complete v1 record and ask a
   general validator whether the result happened to be legal;
2. the store accepted a replacement record without knowing the semantic
   transition that produced it; and
3. persistence, live observation, and physical mutation remained interleaved
   in large call paths, making write-ahead ownership an audit convention
   instead of an enforced interface.

That direction encouraged incremental patches at each v0 persistence call.
It could catch an invalid final shape, but it could not prove that a caller
changed only the fields owned by one transition, retired only the correct
unknown containers, or persisted an action before its physical mutation.

The redesign moves those proofs into type and module boundaries. A complete
record validator remains necessary, but it is the last check, not the
transition API.

## 3. Non-negotiable invariants

R4b satisfies all of the following:

- no lifecycle module receives mutable access to a v1 record;
- no production API accepts an arbitrary replacement v1 record;
- only the transition kernel can construct a persistable v1 rewrite;
- each transition has one exact predecessor set and one frozen field
  footprint;
- every physical Git or filesystem mutation has a persisted typed owner before
  it begins;
- every post-mutation result is accepted only through an opaque proof produced
  by the matching exact observation classifier;
- an ambiguous observation can enter `RecoveryRequired`, but cannot advance a
  journal, create a result, clear an action, or authorize another mutation;
- accepted workspace data is written once, before publication classification,
  and is immutable thereafter;
- v1 unknown fields survive or retire according to the exact transition that
  owns their known container;
- a write is rejected if the source bytes changed after they were checked;
- v0 remains on its released lifecycle and store; v1 does not execute through
  `v0_common_view`;
- R3's v1 writer and migration dispatch remain production-disabled until A1;
  and
- status is read-only, archive is an exact terminal-byte move, and GC never
  rewrites an archive.

Here, an owned physical mutation means a Git, workspace, publication,
preservation, rollback, archive, or GC effect outside the open-record write
that establishes or advances its owner. Open-record persistence is itself
authorized by the prepared transition and governed by the checked-store
protocol in §9; it cannot require a prior copy of the transition it is
persisting.

## 4. Responsibility boundaries

The v1 implementation is divided into five layers.

| Layer | Owns | Must not own |
| --- | --- | --- |
| mutation lease | one retained workspace/root mutation capability for the whole mutating service invocation | record semantics or live-state classification |
| checked record | decoded canonical model, exact raw bytes, unknown manifest, source digest and location | mutation or next-action policy |
| transition kernel | predecessor checks, exact known-field update, immutable-field checks, transition footprint and retirement effect | Git/filesystem I/O or arbitrary YAML writes |
| checked store | source-lineage check, unknown overlay/retirement, atomic write, fsync, reread and equality checks | lifecycle selection or record mutation |
| observers/executors | exact live-state classification and one physical action | record construction or persistence |
| lifecycle service | select the next typed transition, persist intent, call one executor, persist the verified outcome | mutable record access or raw store writes |

The dependency direction is:

```text
                      -> transition kernel -> checked record model
lifecycle service     -> checked store ----> checked record/effect
       |              -> observer/resolver
       +--------------> executor

mutation lease spans open -> dispatch -> observe -> commit -> execute
and every retry of that loop
```

The checked store knows a transition's declared effect, but not why the
lifecycle selected it. An executor knows a prepared action and live state, but
cannot produce a record rewrite.

The expected ownership tree is:

```text
merge/v1_lifecycle/
  mod.rs                         wiring and visibility only
  checked.rs                     StoredV1Record projections
  authority.rs                   opaque action-specific proof vocabulary
  authority/binding.rs           canonical replay binding and sealed issuer
  next_action.rs                 one pure request/state dispatcher
  transition/
    mod.rs                       closed vocabulary and prepare entry point
    effect.rs                    closed typed effect/retirement vocabulary
    footprint.rs                 exact known-diff and conditional footprint verifier
    operation.rs                 operation/recovery/drift reducers
    participant.rs               forward participant reducers
    acceptance_publication.rs    acceptance/publication reducers
    preservation.rs              preservation reducers
    rollback.rs                  rollback reducers
  store/
    mod.rs                       checked-store interface
    rewrite.rs                   atomic rewrite and reread
    unknown.rs                   transition-specific unknown overlay
    archive.rs                   exact terminal archive
  observe/                       opaque exact classifiers/proof tokens
  service/                       thin participant/finalize/reverse consumers
  tests/                         matrix, fault, restart, and privacy owners
```

This is an ownership map, not a requirement to create empty files. Wiring
modules carry no lifecycle policy. The change ledger's general god-file review
trigger is 1,000 lines and allows a small cohesion-justified overrun; when a
split is warranted, the resulting responsibility owners should each be below
500 lines and follow the ownership shown here rather than arbitrary line
chunks. LOC is only a backstop: a smaller file that starts collecting unrelated
concepts is split earlier, and checkpoint planning should establish those
responsibility boundaries before implementation to avoid cleanup churn.

## 5. Opaque checked types

The public surface inside the merge implementation is intentionally small.
Names may change during implementation, but their visibility and authority
must not.

```rust
pub(in merge::v1_lifecycle) struct StoredV1Record {
    typed: ValidatedV1Record,
    raw: serde_yaml::Value,
    unknown_fields: UnknownFieldManifest,
    source_digest: Sha256Digest,
    location: OpenRecordLocation,
}

pub(in merge::v1_lifecycle) struct PreparedV1Rewrite {
    base_digest: Sha256Digest,
    next: ValidatedV1Record,
    effect: TransitionEffect,
}

pub(in merge::v1_lifecycle) struct V1MutationLease {
    guard: WorkspaceMutatorLock,
    workspace_root: CanonicalWorkspaceRoot,
}
```

The fields and constructors of all three types are private to their owner
modules. `PreparedV1Rewrite` is nameable by the sibling checked-store module,
but only the transition module can construct it or read its fields. The store
consumes it through transition-owned accessors that expose only the base
digest, validated next model, and effect required by the commit protocol.
`StoredV1Record` exposes immutable projections required for next-action
selection. `PreparedV1Rewrite` can be constructed only by the transition
kernel and consumed only by the checked store.

`V1MutationLease` wraps the existing workspace mutator lock and binds it to
one canonical root. Only the mutating v1 service can acquire it. The lease is
retained from before the first open-record read through every observation,
checked write, physical action, result write, archive/GC action, and response.
Every checked-store commit and mutating executor requires `&V1MutationLease`
and rejects a root mismatch. Read-only status does not require a mutation
lease and cannot obtain a persistable rewrite.

The kernel entry point is conceptually:

```rust
fn prepare(
    lease: &V1MutationLease,
    current: &StoredV1Record,
    transition: V1Transition,
) -> ModelResult<PreparedV1Rewrite>;
```

The store entry point is conceptually:

```rust
fn commit(
    &self,
    lease: &V1MutationLease,
    current: &StoredV1Record,
    rewrite: PreparedV1Rewrite,
) -> ModelResult<StoredV1Record>;
```

There is no `write_v1(record)`, `persist_v1(record)`, `update_v1(fn)`, public
`validate_transition(old, new)`, or public constructor for a prepared rewrite.
Initial v1 installation, atomic R3 migration, and archive movement are
separate checked operations; none uses the lifecycle rewrite API.

Install authority is also closed:

```rust
pub(in crate::workspace_ops::merge) struct PreparedBornV1Install { /* private */ }
pub(in crate::workspace_ops::merge) struct PreparedMigratedV1Install {
    /* private R3 source digest/model/unknown manifest */
}

fn install_born_v1(
    &self,
    lease: &V1MutationLease,
    expected_absent: OpenRecordSlot,
    install: PreparedBornV1Install,
) -> ModelResult<StoredV1Record>;

fn install_migrated_v1(
    &self,
    lease: &V1MutationLease,
    current_v0: CheckedV0UpgradeSource,
    install: PreparedMigratedV1Install,
) -> ModelResult<StoredV1Record>;
```

The born constructor belongs to the A1 start planner and proves exact request,
workspace, baseline, selected-target, and all-`Planned` initial shape. It is
unreachable in R4b. The migrated constructor is the existing R3 prepared
upgrade authority and remains bound to the exact checked v0 source bytes,
canonical v1 model, and unknown manifest. The store cannot convert between
the two capabilities, install to an arbitrary path, overwrite a present born
slot, or accept a bare v1 record. A1 changes reachability; it does not add a
third installation API.

The byte-lineage guarantee covers cooperating GWZ mutators because every such
path holds the same retained mutation lock. A non-cooperating process can
ignore an advisory lock and replace a file between the last comparison and
portable atomic rename; the design does not claim an unavailable conditional
filesystem replace. Different bytes observed at any checked boundary reject,
and an intervening same-byte replacement has identical durable authority.

## 6. Closed transition vocabulary

`V1Transition` is a compile-time command vocabulary, not a serialized generic
action bag:

```rust
enum V1Transition {
    Operation(OperationTransition),
    Participant(ParticipantTransition),
    Acceptance(AcceptanceTransition),
    Publication(PublicationTransition),
    Recovery(RecoveryTransition),
    Preservation(PreservationTransition),
    Rollback(RollbackTransition),
    Drift(DriftTransition),
}
```

There is no `SetField`, `SetState`, `SetPublicationStep`, arbitrary patch, or
numeric phase comparison. Every enum is exhaustively matched. Adding a
variant requires an explicit predecessor rule, field footprint, unknown-field
effect, positive test, and cross-owner rejection tests.

Every successful transition updates `writer_version` as store metadata in the
same rewrite. This metadata update is implicit in every footprint and is never
available as a standalone transition.

The sub-enum payload authority is frozen conceptually as follows. Unit
variants derive all data from the current checked record; variants that carry
data accept only opaque bound values, never raw strings/records supplied by a
lifecycle caller:

```rust
enum OperationTransition {
    BeginExecution,
    AwaitResolution,
    Halt,
    EnterFinalizing { proof: VerifiedParticipants },
    BeginPreservation { entry: PreparedPreservationEntry },
    BeginRollback { entry: PreparedRollbackEntry },
    CompleteOperation { proof: VerifiedPublicationCompletion },
    AbortOperation { proof: VerifiedRollbackExhausted },
}

enum ParticipantTransition {
    Prepare { intent: PreparedParticipantAction },
    RecordOutcome { proof: VerifiedParticipantOutcome },
    RecordHaltedOutcomeAndResumeExecution { proof: VerifiedParticipantOutcome },
    RecordHaltedOutcomeAndBeginRollback {
        proof: VerifiedParticipantOutcome,
        entry: PreparedRollbackEntry,
    },
    RecordHaltedOutcomeAndBeginPreservation {
        proof: VerifiedParticipantOutcome,
        entry: PreparedPreservationEntry,
    },
    AbandonNotStartedAndBeginRollback {
        proof: VerifiedParticipantNotStarted,
        entry: PreparedRollbackEntry,
    },
    AbandonNotStartedAndBeginPreservation {
        proof: VerifiedParticipantNotStarted,
        entry: PreparedPreservationEntry,
    },
    RecordPreparationFailureAndHalt { batch: PreparedFailureHaltBatch },
    RecordOwnedRetryFailureAndHalt { batch: BoundOwnedRetryFailureHaltBatch },
    RecordOwnedResolutionFailureAndHalt {
        batch: BoundOwnedResolutionFailureHaltBatch,
    },
    RecordNoMutationAbort { proof: VerifiedNoMutationAbort },
}

enum AcceptanceTransition {
    Freeze { accepted: PreparedAcceptedWorkspace },
}

enum PublicationTransition {
    ClassifyRequired { decision: BoundPublicationDecision },
    ClassifyNone { decision: BoundPublicationDecision },
    BeginMigratedValidation,
    ClassifyMigratedRequired { proof: VerifiedResults },
    ClassifyMigratedNone { proof: VerifiedResults },
    RecordCandidate { candidate: PreparedCandidate },
    BeginEvidence { intent: PreparedEvidenceIntent },
    RecordEvidence { proof: VerifiedEvidenceResult },
    BeginCandidatePublication { intent: PreparedPublicationIntent },
    RecordCandidatePublished { proof: VerifiedCandidatePublicationCompletion },
    RecordPublicationVerified { proof: VerifiedPublicationCompletion },
}

enum RecoveryTransition {
    Enter { ambiguity: BoundAmbiguityEvidence },
    Resume { proof: VerifiedRecoveryOrigin },
}

enum PreservationTransition {
    BeginBackupRef { intent: PreparedBackupRefIntent },
    FinishBackupRef { proof: VerifiedBackupRef },
    BeginStash { intent: PreparedStashIntent },
    AdvanceStash { proof: VerifiedStashPhase },
    FinishStash { proof: VerifiedStashCompletion },
    BeginResetAttachedRef { intent: PreparedRefResetIntent },
    AdvanceResetAttachedRef { proof: VerifiedRefResetPhase },
    FinishResetAttachedRef { proof: VerifiedRefResetCompletion },
}

enum RollbackTransition {
    BeginParticipant { intent: PreparedParticipantRollback },
    FinishParticipant { proof: VerifiedParticipantRollback },
    BeginEvidence { intent: PreparedEvidenceRollback },
    AdvanceEvidence { proof: VerifiedEvidenceRollbackStep },
    FinishEvidence { proof: VerifiedEvidenceRollbackCompletion },
    BeginSelectedRoot { intent: PreparedRootMetadataRollback },
    AdvanceSelectedRoot { proof: VerifiedRootMetadataRollbackStep },
    FinishSelectedRoot { proof: VerifiedRootMetadataRollbackCompletion },
}

enum DriftTransition {
    RecordParticipant { fact: BoundParticipantDrift },
    ClearParticipant { proof: VerifiedParticipantDriftClear },
    RecordOperation { fact: BoundOperationDrift },
    ClearOperation { proof: VerifiedOperationDriftClear },
}
```

Each opaque value contains the §8 proof binding. Stable owner/member identity,
action kind, phase, and every derived payload byte come from that value. A raw
member id may be used only for read-only lookup before producing a bound value;
it is never sufficient transition authority.

### 6.1 Operation transitions

| Variant | Exact predecessor | Result and owned fields |
| --- | --- | --- |
| `BeginExecution` | `AwaitingResolution` or `Halted`; no reverse journal | state becomes `Executing` |
| `AwaitResolution` | `Executing`; at least one exact `Conflicted`; every other selected participant successful; no participant error or pending forward action | state becomes `AwaitingResolution` |
| `Halt` | `Executing`; a typed halt reason is already represented by participant outcomes | state becomes `Halted` |
| `EnterFinalizing` | `Executing`; bound `VerifiedParticipants`; every selected participant terminal-success; no pending action or reverse journal | state becomes `Finalizing` |
| `BeginPreservation` | `Executing`, `AwaitingResolution`, `Halted`, or `Finalizing`; no pending forward action or reverse journal; bound `PreparedPreservationEntry` includes the exact global preflight and `VerifiedPublicationHandoff` | state becomes `Preserving` |
| `BeginRollback` | `Executing`, `AwaitingResolution`, `Halted`, `Finalizing`, or `Preserving`; no pending forward or preservation action remains; bound `PreparedRollbackEntry` includes exact global preflight and `VerifiedPublicationHandoff`; the `Preserving` origin additionally requires `VerifiedPreservationExhausted` | state becomes `RollingBack`; the `Executing` edge is the direct valid abort edge and never fabricates an invalid intermediate `Halted` row |
| `CompleteOperation` | `Finalizing`; acceptance present; publication exactly complete; no pending action/journal or unresolved drift | state becomes `Completed` |
| `AbortOperation` | `RollingBack`; no pending action/journal; bound `VerifiedRollbackExhausted` proof covers every participant and exact selected-root baseline | state becomes `Aborted` |

Idempotence is handled by reading the current state and selecting no mutation,
not by allowing same-state transition variants. A predecessor not listed in
the table is rejected.

### 6.2 Participant transitions

| Variant | Exact predecessor | Result and owned fields |
| --- | --- | --- |
| `PrepareParticipantAction` | operation `Executing`; no forward `pending_action` exists anywhere in the record; `Conflicted` only for exact `ResolveConflict`, otherwise `Planned`, `Failed`, or `Unattempted` for its frozen retry action | installs the exact existing `PendingMergeAction` and changes nothing else in the participant |
| `RecordParticipantOutcome` | operation `Executing`, or `Halted` only when the post-outcome aggregate still has another exact halt cause; the same pending action is present | consumes a matching bound outcome proof, clears the pending action and any prior retry error, and writes exactly one of `UpToDate`, `FastForwarded`, `Merged`, `Conflicted`, or `Continued` plus its derived result fields |
| `RecordHaltedOutcomeAndResumeExecution` | request `ResumeStart` or `Continue`; operation `Halted`; same pending action; outcome would remove the final halt cause | atomically records the exact outcome, clears the action, and changes state to `Executing`; this lets restart reconciliation consume a completed retained owner without requiring an otherwise impossible stopped state with no halt cause |
| `RecordHaltedOutcomeAndBeginRollback` | request `Abort`; operation `Halted`; same pending action; bound `PreparedRollbackEntry` covers exact global preflight, publication handoff, and the anticipated post-outcome model | atomically records the exact outcome, clears the action, and changes state to `RollingBack` |
| `RecordHaltedOutcomeAndBeginPreservation` | request `Preserve`; operation `Halted`; same pending action; bound `PreparedPreservationEntry` covers exact global preflight, publication handoff, and the anticipated post-outcome model | atomically records the exact outcome, clears the action, and changes state to `Preserving` |
| `AbandonNotStartedAndBeginRollback` | request `Abort`; operation `Executing` or `Halted`; matching bound `NotStarted` proof; `PreparedRollbackEntry` covers the anticipated action-free model | atomically clears only the never-started pending action, retires that container, and enters `RollingBack`; no integration result is fabricated |
| `AbandonNotStartedAndBeginPreservation` | request `Preserve`; operation `Executing` or `Halted`; matching bound `NotStarted` proof; `PreparedPreservationEntry` covers the anticipated action-free model | atomically clears only the never-started pending action, retires that container, and enters `Preserving`; no integration result is fabricated |
| `RecordPreparationFailureAndHalt` | operation `Executing`; no pending action; failing participant in `Planned`, `Failed`, or `Unattempted`; bound batch names the exact later `Planned` suffix | atomically writes the failing row `Failed`, exact typed error, every later `Planned` row `Unattempted`, and operation `Halted`; no Git result fields |
| `RecordOwnedRetryFailureAndHalt` | operation `Executing`; pending `VerifyUpToDate`, `FastForward`, or `TrueMerge` remains present; bound batch names the exact later `Planned` suffix | atomically writes the failing row error/`Failed`, every later `Planned` row `Unattempted`, and operation `Halted`; pending ownership remains unchanged |
| `RecordOwnedResolutionFailureAndHalt` | operation `Executing`; pending `ResolveConflict` remains present on `Conflicted`; bound batch names the exact later `Planned` suffix | atomically retains `Conflicted`, resulting/conflict/expected-merge-head fields and pending action byte-exact, attaches only the error, writes every later `Planned` row `Unattempted`, and enters `Halted` |
| `RecordNoMutationAbort` | operation `RollingBack`; participant `Planned`, `UpToDate`, `Failed`, or `Unattempted`; no pending reverse action needed | writes `Aborted` and only its legal terminal fields |

One ordinary transition owns one participant. The three failure/halt variants
are the sole multi-row exception: they write one failure/error result and only the
deterministically derived later `Planned -> Unattempted` suffix in the same
rewrite. They never aggregate successful/conflicted outcomes. Integrated and
conflicted participants requiring physical reversal are terminalized only by
rollback transitions.

The three named halted-outcome variants and two named abandonment-entry
variants are the only request-specific entry compounds. The three
failure/halt variants are the only deterministic stop compounds. The outcome
variants exist because a successful or expected-conflict reconciliation can
remove the last fact that makes a `Halted` record structurally valid.
Persisting that participant outcome while leaving the operation halted would
create an invalid durable state; persisting abandonment without the request
state would lose durable intent. The request-specific variants have fixed
union footprints; they are not a generic composition facility.

The rollback/preservation compound entry is prepared against the same source
digest and the exact canonical model anticipated after applying its paired
outcome. The reducer recomputes that anticipated model before accepting the
entry. A handoff/preflight prepared for the pre-outcome participant, a
different outcome, owner, request, or digest cannot authorize the compound.

`PreparedRollbackEntry` has closed origin-specific constructors. Its
`FromPreserving` form is constructible only with a bound
`VerifiedPreservationExhausted` proof covering every owner in both passes,
every required stable evidence row/artifact, exact root restoration, and an
empty preservation journal. Other origin forms cannot be relabeled as
`FromPreserving`.

An ordinary preparation or owned retry failure writes `Halted`, the `Failed`
row, and the entire later `Unattempted` suffix atomically. A resolution failure
instead preserves the authoritative `Conflicted` row and its exact conflict
evidence while attaching the error and writing the same suffix. This prevents both a
crash-visible `Executing + Failed + pending` record whose frozen I2 recovery
origin would be `Halted` and an invalid `Halted` record retaining later
`Planned` rows. The bound batch proves exact selected-target position and
rejects a non-later, non-`Planned`, missing, duplicate, or reordered suffix.

Continue never abandons a not-started action: it moves to `Executing` and uses
the persisted action. Abort/preserve may retire one only with the matching
bound `NotStarted` proof and matching prepared entry, atomically entering
`RollingBack` or `Preserving`. There is no crash-visible action-free
`Executing`/`Halted` abandonment row that a later Continue could re-prepare.
An ambiguous halted pending action has the exact I2 `Halted` origin because
the retained `Failed`/error fact remains unchanged.

The checked-record validator rejects more than one forward pending action
record-wide and rejects any forward pending action in effective `Preserving`.
This applies before dispatch, so the service never chooses an ordering among
multiple durable owners and never enters preservation while another owner is
stranded. These are no-wire corrections to the I2 validity set, not new
transition behavior.

### 6.3 Acceptance transitions

| Variant | Exact predecessor | Result and owned fields |
| --- | --- | --- |
| `FreezeAcceptance` | state `Finalizing`; all selected outcomes exactly verified; no pending action/journal; acceptance absent; publication absent | writes the complete `AcceptedWorkspaceV1` and no publication decision |

The acceptance payload is constructed by one shared, pure acceptance builder.
The v1 finalizer and the R3 open-v0 adapter must call that same builder or a
byte-equivalence wrapper around it. No later transition may clear, replace, or
modify acceptance or any unknown descendant beneath it.

### 6.4 Publication transitions

Publication steps are exact action owners, not ordinal progress markers.

| Variant | Exact predecessor | Result and owned fields |
| --- | --- | --- |
| `ClassifyPublicationRequired` | `Finalizing`; acceptance present; publication absent; pure R4a decision is required | installs empty required progress at `PreparingCandidate` |
| `ClassifyNoPublication` | same, but pure R4a decision is no publication | installs `Complete` with no candidate, evidence, output hash, or preservation data |
| `BeginMigratedResultValidation` | R3-installed adapted record at `NotStarted`; acceptance present; no candidate or output evidence | advances exactly to `ValidatingResults` |
| `ClassifyMigratedPublicationRequired` | R3-installed adapted record at `ValidatingResults`; exact result-validation proof; pure R4a decision is required | advances exactly to `PreparingCandidate` |
| `ClassifyMigratedNoPublication` | same, but pure R4a decision is no publication | advances exactly to `Complete` with no candidate, evidence, output hash, or preservation data |
| `RecordCandidate` | `PreparingCandidate`; candidate absent; opaque `PreparedCandidate` from the pure builder | records candidate bytes/hashes derived from immutable acceptance; remains `PreparingCandidate` |
| `BeginEvidence` | `PreparingCandidate`; exact candidate present; evidence absent; bound preflight verifies the baseline form and derives the evidence intent | advances to `CommittingEvidence` before the authoritative action classification or Git evidence mutation |
| `RecordEvidence` | `CommittingEvidence`; evidence absent; matching evidence proof | writes exact composition commit/tree/root fields and hashes; remains `CommittingEvidence` |
| `BeginCandidatePublication` | `CommittingEvidence`; exact evidence present; bound preflight verifies the baseline form and derives publication intent | advances to `PublishingCandidate` before the authoritative prefix classification or artifact/index mutation |
| `RecordCandidatePublished` | `PublishingCandidate`; bound `VerifiedCandidatePublicationCompletion` proves terminal file prefix and exact staged candidate index | advances to `VerifyingPublication`; candidate/evidence remain immutable |
| `RecordPublicationVerified` | `VerifyingPublication`; exact verification proof | advances to `Complete` |

The `PublishingCandidate` owner spans marker, lock, boundary, and index staging.
The R4a file classifier has the closed exact prefix set `Baseline`, `Marker`,
`Lock`, and `Boundary`, but file prefix alone is not completion authority:
`Boundary` exists both immediately before and immediately after `stage_paths`.
The v1 publication observer therefore combines the exact file prefix with an
exact raw index/worktree classification:

| Files | Index/worktree | Resolution |
| --- | --- | --- |
| `Baseline` | exact baseline evidence form | execute marker write |
| `Marker`, candidate lock differs from baseline | exact pre-stage candidate index | execute lock write |
| `Marker`, candidate lock equals baseline but candidate boundary differs | exact pre-stage candidate index | execute boundary write; the lock write is already byte-complete |
| `Marker`, candidate lock and boundary both equal their baselines | exact pre-stage candidate index | execute the owned index-stage action; both remaining file writes are already byte-complete |
| `Marker`, candidate lock and boundary both equal their baselines | exact candidate paths staged and worktree-aligned | construct `VerifiedCandidatePublicationCompletion` |
| `Lock` | exact pre-stage candidate index | execute boundary write |
| `Boundary` | exact pre-stage candidate index | execute the owned index-stage action |
| `Boundary` | exact candidate paths staged and worktree-aligned | construct `VerifiedCandidatePublicationCompletion` |
| any mixed/third form | any | bound ambiguity; no mutation |

Only the two explicit completion rows may authorize
`RecordCandidatePublished`.
`VerifiedPublicationPrefix` can select an intermediate file action but cannot
advance to `VerifyingPublication`. If implementation needs another durable
prefix or cannot distinguish exact pre/post-stage index forms, that is an I2
design change; it cannot be simulated by an in-memory cursor.

The two `Marker` terminal rows are the frozen R4a degenerate case, not a new
prefix: candidate lock bytes already equal baseline lock bytes and candidate
boundary bytes already equal baseline boundary bytes. Restart and every fault
boundary use those equality predicates plus exact index alignment; `Marker`
alone is never terminal evidence.

Candidate and evidence fields are immutable once present. Completion is the
separate `CompleteOperation` transition so publication proof cannot silently
change operation state.

Born-v1 finalization does not create `NotStarted` or `ValidatingResults`.
Those two shapes are retained solely so an exact R3-adapted v0 record can
resume without rewriting history. No lifecycle transition can create either
shape; after restart their validated persisted shape is sufficient authority
to select the two closed compatibility transitions. Every other R3 publication
phase enters the same matching reducer shown above and has no migration-only
writer.

Abort/preserve cannot bypass a publication action. The dispatcher first
obtains a bound `VerifiedPublicationHandoff`. If `CommittingEvidence` has an
exact live composition result not yet in the record, `RecordEvidence` is the
only next action. If `PublishingCandidate` has an exact partial prefix, the
handoff binds that prefix so the reverse/preservation journal can own it; it
does not force forward publication. Any mixed evidence/prefix form enters the
representable `Finalizing` recovery origin. A no-publication or pre-publication
shape receives an exact no-effect handoff bound to the same record digest.

### 6.5 Recovery transitions

| Variant | Exact predecessor | Result and owned fields |
| --- | --- | --- |
| `EnterRecovery` | `Executing`, `AwaitingResolution`, `Halted`, or `Finalizing` with exact I2-derived origin; `Preserving` only with the matching pending-preservation action; `RollingBack` only with the matching pending-rollback action | consumes a bound ambiguity proof and writes only `state: RecoveryRequired`, exact `recovery_context.origin_state`, and writer metadata |
| `ResumeFromRecovery` | `RecoveryRequired`; exact unchanged recovery context; matching exact-state proof | restores only the recorded origin state, clears recovery context, and applies no action progress |

Recovery dispatch first matches the exact origin and any pending journal.
It does not route through a general continue/finalize function. An ambiguous
proof cannot construct `ResumeFromRecovery`.

Entering recovery never clears or advances a pending forward, rollback, or
preservation action. If the originating state is not representable by the
frozen v1 recovery context, the transition is rejected rather than inventing
a fallback origin.

The recorded origin is literally the immediately prior durable non-recovery
state. In particular, `BeginExecution` may leave an `Executing` record that
still contains a retained `Failed`/error halt cause and its forward owner. If
the fresh owner observation is ambiguous, `resolve_observation` must first
return the ordinary `Halt` transition, commit `Halted`, discard the now-stale
ambiguity binding, and reobserve. Only the new bound ambiguity proof may then
enter `RecoveryRequired { origin_state: Halted }`. It may not write a direct
`Executing -> RecoveryRequired(origin=Halted)` compound, because that would
make `origin_state` derived history rather than the prior durable state.

Ambiguity found between preservation/rollback actions, when no matching
pending journal exists, is deliberately not persisted as `RecoveryRequired`:
I2 has no valid `Preserving`/`RollingBack` recovery origin for that shape. It
returns a typed drift/recovery error with no record rewrite and no mutation.
Once an action owner is persisted, ambiguity enters recovery with that exact
journal retained.

### 6.6 Rollback transitions

The prospective reverse owner is derived from immutable record state in this
order:

1. publication evidence;
2. participants in reverse selected-target order; and
3. selected-root metadata.

Only the derived current owner can be started or advanced. The reverse cursor
is fully derivable from participant terminal states, publication rollback
evidence, selected-root membership, and the exact pending phase. Checked-open
validation rejects a pending action whose owner, kind, terminal state, or
phase is not that exact cursor. Before a new reverse action, a bound read-only
preflight derives its complete payload; the transition persists and rereads
that action before the action classifier is allowed to label the live state
`NotStarted`, `Completed`, or `Ambiguous`.

| Variant | Exact predecessor | Result and owned fields |
| --- | --- | --- |
| `BeginParticipantRollback` | `RollingBack`; no pending rollback; named participant is current reverse owner | installs exact `Participant` action |
| `FinishParticipantRollback` | same participant action; matching completed proof | clears action and writes only `Aborted` or `RolledBack` and its exact terminal fields |
| `BeginEvidenceRollback` | `RollingBack`; no pending rollback; evidence is current owner | installs `PublicationEvidence { EvidenceCommit }` |
| `AdvanceEvidenceRollback` | exact evidence step; matching completed proof | advances exactly one edge: `EvidenceCommit -> Boundary -> Lock -> Marker -> Index -> Complete` |
| `FinishEvidenceRollback` | evidence action at `Complete`; complete proof | clears action and writes `evidence_rolled_back: true` |
| `BeginSelectedRootRollback` | `RollingBack`; no pending rollback; selected-root metadata is current owner | installs `SelectedRootMetadata { Manifest }` |
| `AdvanceSelectedRootRollback` | exact root step; matching completed proof | advances exactly one edge: `Manifest -> Lock -> Complete` |
| `FinishSelectedRootRollback` | root action at `Complete`; complete proof | clears the action; immutable baseline/acceptance remain unchanged |

Skip, reverse, same-step, and wrong-owner advances are rejected. Safe
no-Git participant terminalization uses `RecordNoMutationAbort`; it never
creates a fake reverse journal.

I2 intentionally has no separate selected-root-metadata completion bit. After
`FinishSelectedRootRollback` clears the complete journal, the dispatcher must
observe the exact baseline manifest and lock again. Exact completion contributes
to `VerifiedRollbackExhausted`; exact not-started installs a new action only
when the record still has work; a mixed form is an error without rewrite
because no action is pending. `AbortOperation` is the durable completion marker.
Thus a crash after clearing the selected-root action and before `Aborted` does
not repeat forever or treat participant terminal state alone as metadata
completion.

### 6.7 Preservation transitions

Only the derived current preservation owner can be started or advanced. One
stable evidence row exists per owner and survives as fields fill.

| Variant | Exact predecessor | Result and owned fields |
| --- | --- | --- |
| `BeginBackupRef` | `Preserving`; no pending preservation; owner is current | installs exact `BackupRef` before ref creation |
| `FinishBackupRef` | same action; exact-ref proof | clears action and fills only the owner's backup ref/commit evidence |
| `BeginStash` | `Preserving`; no pending preservation; owner current | installs exact `Stash`; initial phase is `NormalizeParent` with a root handoff, otherwise `CreateStash`; for a root handoff, the same write also records the matching immutable publication handoff before mutation |
| `AdvanceStash` | exact stash phase; matching proof | advances only the frozen I2 edge and fills stash ids exactly when `CreateStash` completes |
| `FinishStash` | stash at `Complete`; exact proof | clears action; retained evidence row is complete |
| `BeginResetAttachedRef` | `Preserving`; no pending preservation; owner current | installs exact `ResetAttachedRef { phase: PrepareParent }` before root preparation, or `ResetRef` without a root handoff; a root row installs or verifies the same immutable publication handoff even when no stash ran |
| `AdvanceResetAttachedRef` | exact reset phase; matching proof | advances only the amendment-2 graph and changes one physical phase; the no-root graph remains `ResetRef -> Complete` |
| `FinishResetAttachedRef` | reset at `Complete`; exact proof | clears action; retained evidence remains immutable |

The stash phase graph is exactly:

```text
with root handoff: normalize_parent -> normalize_marker -> normalize_lock -> normalize_index
                   -> create_stash -> restore_index -> restore_lock
                   -> restore_parent -> restore_marker -> write_bundle
                   -> complete
without handoff:   create_stash -> write_bundle -> complete
```

The root reset graph is exactly:

```text
with root handoff: prepare_parent -> prepare_marker -> prepare_lock -> prepare_index
                   -> reset_ref -> restore_index -> restore_lock
                   -> restore_parent -> restore_marker -> complete
without handoff:   reset_ref -> complete
```

`GwzM5-8R4bInterfaceAmendment-2.md` §3.4 owns the complete before/after table,
disjoint no-op classification, and explicit `C0`/`C1`/`H` physical forms.
Its §8 also owns the causal exception for a required empty marker parent:
structural completion is `PreservationDurabilityPending` on every platform and
advances only when a fresh exact goal is paired with the matching successful
attempt. Unix executes an idempotent parent sync; Windows executes a pinned,
handle-bound, write-through final-to-staging-to-final rename barrier. Optional
and already-established parent no-ops remain ordinary completed observations.

Stash ids and the stable evidence row are written in the same transition that
records successful `CreateStash`. A bundle write consumes the already-recorded
object id. No unowned `preservation_prefix` may be introduced.

The cross-owner preservation cursor is deterministic and preserves the
released two-pass behavior:

1. derive owners in `selected_targets` order; append `PublicationRoot` only
   when composition evidence exists and no selected `@root` owns the same
   root ref;
2. artifact pass, owner by owner: create/verify the backup ref when the live
   descendant commit differs from the immutable anchor, then create/verify the
   stash and bundle when the exact preimage is dirty;
3. verify every required owner artifact and stable evidence row; and
4. reset pass in the same owner order: reset only owners with a recorded
   backup target different from the anchor, including exact root restoration.

At each cursor position, bound preflight facts may prepare the next journal
payload but cannot classify or authorize the action. The action is first
persisted and reread; only its matching observer can advance it. An owner is
skipped only by an exact proof that its artifact/reset work is unnecessary or
already complete. No later owner starts while an earlier owner has an
unfinished or ambiguous action.

I2 does not persist a no-op row when an owner needs no artifact, nor a separate
completion bit after a reset journal clears. Consequently, a pure decoder
cannot distinguish a legitimately later pending owner from the same bytes
paired with a live earlier owner that has become incomplete. R4b does not fake
that distinction. Checked-open validation rejects every contradiction that is
derivable from record bytes: an owner outside the frozen list, an action/phase
inconsistent with its evidence, a later durable evidence row that contradicts
the claimed cursor, or an artifact/reset pass mismatch.

Before classifying, advancing, or executing any persisted preservation action,
the observer must additionally produce `VerifiedPreservationCursorPrefix`.
It binds the record digest, exact two-pass cursor position, and an exact live
observation proving every earlier position complete or unnecessary. The
matching action proof embeds that prefix proof. A legitimate restart after
earlier no-op skips or cleared resets therefore continues; a wrong/later owner,
new earlier work, or ambiguous earlier repository state returns typed
`PreservationEvidenceMismatch` without journal rewrite or physical mutation.
The resolver may not process the named pending owner merely because its local
payload is individually legal.

### 6.8 Drift transitions

Drift is diagnostic evidence, never mutation authority.

| Variant | Exact predecessor | Result and owned fields |
| --- | --- | --- |
| `RecordParticipantDrift` | exact new ordered drift observation for one participant | inserts/replaces only the matching drift identity |
| `ClearParticipantDrift` | exact recorded drift identity and exact-clear proof | retires only that identity |
| `RecordOperationDrift` | exact new unique operation-drift kind | inserts/replaces only that kind |
| `ClearOperationDrift` | exact recorded kind and exact-clear proof | retires only that kind |

Callers cannot compose a drift change with another transition in one write.
If a future invariant requires an atomic compound transition, it must be a new
closed enum variant with an independently reviewed footprint; it is not a
generic composition facility.

### 6.9 Physical mutation ownership matrix

The persisted owner checked immediately before each physical mutation is
closed:

| Physical mutation | Required persisted owner |
| --- | --- |
| participant fast-forward, true merge, conflict creation, or resolved commit | that participant's exact `PendingMergeAction` |
| composition/evidence commit | publication progress at `CommittingEvidence` plus exact candidate |
| marker, lock, boundary, or root index publication | publication progress at `PublishingCandidate` plus exact candidate/evidence; the R4a prefix classifier selects only the next write |
| backup-ref creation | matching `PendingPreservationActionV1::BackupRef` |
| root marker-parent, marker, lock, or index normalization before stash | matching `Stash` at the exact `NormalizeParent`, `NormalizeMarker`, `NormalizeLock`, or `NormalizeIndex` phase and recorded matching publication handoff |
| native stash creation | matching `Stash { phase: CreateStash }` |
| post-stash root index, lock, marker-parent, or marker restoration | matching `Stash` at the exact `RestoreIndex`, `RestoreLock`, `RestoreParent`, or `RestoreMarker` phase |
| preservation-bundle write | matching `Stash { phase: WriteBundle }` with durable exact stash ids |
| pre-reset root marker-parent, marker, lock, or index preparation | matching `ResetAttachedRef` at the exact `PrepareParent`, `PrepareMarker`, `PrepareLock`, or `PrepareIndex` phase |
| preservation attached-ref reset | matching `ResetAttachedRef { phase: ResetRef }` |
| post-reset root index, lock, marker-parent, or marker restoration | matching `ResetAttachedRef` at the exact `RestoreIndex`, `RestoreLock`, `RestoreParent`, or `RestoreMarker` phase |
| participant native merge abort or integrated-ref reset | matching `PendingRollbackActionV1::Participant` |
| publication evidence, boundary, lock, marker, or index rollback | matching `PublicationEvidence` at the exact named step |
| selected-root manifest or lock rollback | matching `SelectedRootMetadata` at the exact named step |
| open-record archive rename | validated terminal checked record; destination reconciliation requires exact source-byte identity |
| merge-owned backup-ref GC deletion | immutable validated archive row with exact derived private ref name and exact observed target |
| terminal archive deletion | immutable validated archive after every owned backup ref is observed absent |

Status, observation, response projection, and preservation-worklist generation
are read-only and have no mutation owner. Native stashes and preservation
bundles are retained by GC as required by I2. Candidate construction is also
read-only: its opaque pure-builder result becomes durable only through
`RecordCandidate`.

## 7. Closed next-action dispatcher

Typed writes do not by themselves prevent orchestration policy from becoming
scattered. V1 therefore has one pure durable-state dispatcher:

```rust
fn next_action(
    current: &StoredV1Record,
    request: V1LifecycleRequest,
) -> ModelResult<V1NextAction>;
```

`V1LifecycleRequest` is the closed internal form of resume-start, continue,
abort, preserve, status, and archive work. `V1NextAction` is closed:

```rust
enum V1NextAction {
    Observe(BoundObservationRequest),
    Apply(V1Transition),
    Respond(V1ResponseDisposition),
    Reject(ModelError),
}

enum ResolvedV1Action {
    Apply(V1Transition),
    Execute(BoundPhysicalAction),
    Respond(V1ResponseDisposition),
    Reject(ModelError),
}

fn resolve_observation(
    current: &StoredV1Record,
    request: V1LifecycleRequest,
    observation_request: BoundObservationRequest,
    observation: BoundExactObservation,
    attempt: Option<BoundExecutionAttempt>,
) -> ModelResult<ResolvedV1Action>;

struct V1Invocation {
    prepared_participant_owners: OrderedSet<MemberId>,
    attempted_physical_actions: OrderedMap<ActionIdentity, ExecutionDiagnostic>,
}
```

Neither enum contains a function pointer or arbitrary callback. The dispatcher
is the only constructor of `BoundObservationRequest`. The observation resolver
is the only constructor of `BoundPhysicalAction` and the only place that maps
`NotStarted`, `Completed`, or `Ambiguous` to a transition, one owned execution,
or an error. Read-only preparation/preflight success or failure is also a bound
observation payload resolved here; a caller does not branch on it. Thin
lifecycle consumers contain no state, phase, observation, preparation, or
request-result branching.

`V1Invocation` is an authority-owned, non-durable guard around the pure
dispatcher; it is not consumer policy and it never supplies completion
authority. Its history is discarded at the end of one service call. It may
refine a pure next action only in these closed cases:

- reject a second preparation or physical execution of the same owner/action
  within one invocation;
- after a `Continue` retry produces `Conflicted`, skip that newly stopped
  owner for the remainder of the invocation, continue later frozen targets in
  order, and synthesize `AwaitResolution` only after no later eligible target
  remains;
- return the newly committed stopped disposition after an invocation-owned
  failure, after the preceding conflict sequence commits
  `AwaitingResolution`, or immediately after `EnterRecovery`; and
- allow `ResumeStart` to consume a completed retained participant owner with
  the same halted-outcome compound used by `Continue`, so a crash between the
  failed physical attempt and response cannot strand a valid halted record.

The guard cannot skip an owner on a later invocation, create proof tokens,
classify live state, change immutable target order, or authorize a physical or
durable mutation. A restart therefore forgets invocation history and resumes
solely from the checked record and fresh exact observations.

The dispatcher applies this precedence exhaustively:

1. validate the checked record and honor read-only status without mutation;
2. dispatch `RecoveryRequired` by exact recorded origin and pending journal;
3. reconcile any persisted publication, rollback, preservation, or forward action before
   selecting a new physical action, using the request-specific halted compound
   transitions where a participant-only rewrite would be structurally invalid;
4. handle a terminal record through status or exact-byte archive only;
5. apply request legality to the exact operation state;
6. derive the current participant, finalization, preservation, or rollback
   owner from immutable record order and evidence; and
7. return exactly one next action.

No lifecycle caller may bypass a returned reconciliation action to honor a
new abort or preservation request. Ambiguity under a persisted owner returns
`EnterRecovery`; unrepresentable between-action preservation/rollback
ambiguity rejects without rewrite. Neither is represented as a best-effort
next action. R4b-T freezes and tests the full `V1LifecycleRequest x operation
state x pending owner x base phase x observation class` matrix. R4b-F, R4b-X,
and R4b-P are thin consumers of these two dispatchers and may not carry
independent state/phase matches.

When the current state is `Preserving`, the dispatcher completes the exact
two-pass preservation cursor even between actions, regardless of an incoming
abort request. It may return `BeginRollback` only after a fresh bound
`VerifiedPreservationExhausted` proof. There is no implicit abandonment of a
partially completed preservation plan.

The service loop restarts after every step:

```text
open checked record under the retained mutation lease
invocation.next_action(current, request)
  Observe -> observe -> resolve_observation
  Apply   -> prepare -> checked commit -> invocation stop check -> loop
  Execute -> execute exactly once -> retain bound attempt diagnostic,
             reopen the same source bytes/observe/resolve;
             never treat executor output as outcome
  Respond/Reject -> finish
```

After an owner-creating transition, the loop must use the checked record
returned by the store before observation. After a physical action, it retains
only a `BoundExecutionAttempt`, reopens the same record under the lease,
rejects any different source bytes before another dispatch, and observes
again. The attempt records the exact action binding and whether the executor
returned success or a diagnostic error, but is not completion authority. The
resolver combines it with the fresh matching observation:

- `Completed` records the observed outcome/progress, regardless of a late
  executor diagnostic;
- `Ambiguous` enters representable recovery and never records the diagnostic
  as a result;
- `NotStarted` plus a participant executor error constructs the bound owned
  failure/halt batch; and
- `NotStarted` with the same exact physical action returns a typed
  no-progress/error result with the persisted owner unchanged, while a
  different action under the same record-bound owner may execute only when
  the sealed observer proves the intervening exact physical prefix. This
  exception is required by the marker/lock/boundary/index publication owner;
  callers and executor diagnostics cannot select the successor action.

The executor cannot create an outcome proof or result transition. After every
result/progress write, the loop dispatches from the new persisted bytes. This
prevents consumers from carrying stale state across durable boundaries or
retrying an executor indefinitely.

## 8. Proof tokens and I/O discipline

Observers return closed classifications:

```rust
enum ExactObservation<P> {
    NotStarted(P),
    Completed(P),
    Ambiguous(AmbiguityEvidence),
}

struct ProofBinding {
    source_digest: Sha256Digest,
    location: OpenRecordLocation,
    workspace_id: WorkspaceId,
    merge_id: MergeId,
    operation_id: OperationId,
    owner: StableOwnerIdentity,
    action: ActionIdentity,
    phase: ExactPhaseIdentity,
    payload_sha256: Sha256Digest,
}

struct BoundExecutionAttempt {
    binding: ProofBinding,
    diagnostic: ExecutionDiagnostic,
}
```

That generic sketch remains the vocabulary for ordinary actions. Amendment 2
adds the closed preservation-only
`ExactObservationFact::PreservationDurabilityPending` case for the required
empty marker parent; it is neither ordinary `Completed` nor ambiguity and is
resolved only by the causal attempt rule in §6.7.

The payload types are opaque and action-specific, for example:

- `PreparedCandidate`;
- `VerifiedParticipantOutcome`;
- `VerifiedParticipantRollback`;
- `VerifiedEvidenceResult`;
- `VerifiedPublicationHandoff`;
- `VerifiedPublicationPrefix`;
- `VerifiedCandidatePublicationCompletion`;
- `VerifiedPublicationCompletion`;
- `VerifiedPreservationResult`; and
- `VerifiedPreservationCursorPrefix`;
- `VerifiedPreservationExhausted`;
- `VerifiedRootMetadataRollback`.

Their fields and constructors are private to the matching observer or pure
builder. Reducers accept these tokens, not ambient repository state, raw
strings, or booleans. Tests use test-only checked fixtures; production
constructors are not widened for test convenience.

Every observation request, observation payload, prepared intent,
`PreparedCandidate`, and physical-action capability carries a private
`ProofBinding`. Its payload hash covers every authority-bearing input in
canonical binary framing, including raw non-UTF-8 path bytes where applicable.
The resolver, reducer, and executor each require exact equality with the
current checked record digest, workspace/merge/operation identity, stable
owner, action kind, phase, derived payload, and exact checked
`OpenRecordLocation` (canonical workspace root plus canonical open-record
path). Identical bytes and IDs cloned beneath another root do not match. A
proof from another member, phase, record rewrite, operation, workspace,
location, or earlier digest is unusable.
The binding is reissued only by a fresh observer/builder after each checked
write. Opaque type names without this replay binding are insufficient and are
forbidden.

`BoundExecutionAttempt` has the same binding and is consumed at most once by
the immediate post-execution resolver call. Executor error text/code is
diagnostic input only. It cannot by itself clear/advance an owner, write a
participant failure, or enter recovery. The matching bound retry- or
resolution-failure halt batch is constructible only from the conjunction of a
matching attempt error, a fresh matching `NotStarted` proof, the exact pending
action kind/state, and the exact derived later-target suffix.

`PreparedCandidate` is the exception to the I/O wording: it comes from the
pure candidate builder after acceptance and verified participant facts are
frozen. It authorizes only the record write and no external mutation.

The lifecycle algorithm for every new physical action is fixed:

```text
open and validate exact record bytes under the mutation lease
derive a bound read-only preflight intent
persist the complete typed action owner and reread it
observe/classify the persisted action
if completed: persist verified outcome/progress and loop
if ambiguous: persist representable recovery origin, otherwise reject, and stop
if not started: resolve one bound physical action, execute it, reopen, and loop
```

When a publication step or pending journal is already the owner, the preflight
and owner-persistence lines are omitted; the exact persisted owner is observed.
Forward participant preparation and publication phase transitions follow the
same write-before-classify rule. Read-only preflight may derive a complete
intent, but cannot return `NotStarted`/`Completed` authority. No mutation
follows a failed persistence or reread.

## 9. Checked-store protocol

`commit` performs these steps in order:

1. require a live `V1MutationLease` bound to the same canonical workspace root;
2. require `rewrite.base_digest == current.source_digest`;
3. reread the source path and require its byte digest still equals that value;
4. require the source still names the same workspace, merge, operation, and
   open-record location;
5. recompute the known semantic diff and require it equals the transition's
   closed `TransitionEffect` footprint;
6. apply the transition-specific unknown-field survival and retirement rules;
7. serialize to a unique same-directory temporary;
8. fsync the file, decode it, and compare canonical model plus unknown
   manifest with the prepared expectations;
9. atomically replace the source and fsync the directory; and
10. reread the published bytes and repeat digest, model, and unknown-manifest
   checks before returning the next `StoredV1Record`.

The store cannot infer a retirement from an arbitrary old/new diff. The
transition effect names the identities it may retire. The store rejects an
effect that does not match the actual known diff or attempts to retire an
identity outside the transition's frozen footprint.

Initial born-v1 installation and R3's atomic migrated-v1 installation use
their own validated install types and source rules. Archive accepts only a
validated terminal checked record and atomically moves its exact bytes. A
destination-only or source-and-destination crash is reconciled by exact byte
identity; archive never decodes and reserializes terminal content.

## 10. Unknown-field handling

The I2 retirement table remains normative. The kernel makes it enforceable by
returning a private closed `TransitionEffect` variant corresponding one-to-one
with the transition variant. It is not a caller-constructed set of paths or a
generic diff allow-list. Its exhaustive methods derive:

- exact known field paths that may change;
- stable sequence/container identities that may be added or removed;
- exact retired-container identities; and
- immutable subtree assertions.

Conceptually:

```rust
enum TransitionEffect {
    BeginExecution,
    ParticipantPrepared { owner: StableOwnerIdentity },
    ParticipantOutcome { owner: StableOwnerIdentity, result: ResultIdentity },
    // one named effect for every transition variant; no Custom/Paths case
}

impl TransitionEffect {
    fn verify_known_diff(&self, old: &CanonicalV1, new: &CanonicalV1) -> Result<()>;
    fn retirement(&self, old: &UnknownManifest) -> Result<ExactRetirement>;
}
```

Only the reducer constructs an effect. `ExactRetirement` is likewise private
and identity-typed; it cannot name a path that the corresponding effect does
not own. The store reruns both exhaustive methods and never accepts a raw path
list from transition or lifecycle code.

Every transition test compares the actual semantic diff with this footprint.
Unknown fields beneath record, baseline, participants, acceptance,
publication evidence, and surviving stable rows remain byte-value exact.
Unknown descendants retire only when the matching typed container is legally
retired. They never move to a replacement error, action, drift identity, or
sequence row.

The first acceptance write may preserve exact derived accepted-lock/audit
extensions at their approved destinations. No later acceptance transition
exists, so no descendant can be retired. Recovery context and pending action
unknowns retire only through their corresponding exact resume/result
transition.

## 11. Runtime and version isolation

R4b introduces a v1 lifecycle service beside the released v0 service.

- v0 opens, mutates, and archives through the existing v0 paths;
- v1 opens through the versioned decoder and enters only the v1 service;
- the owned, serializable `MergeOperationRecordV1::v0_common_view` conversion
  is removed before lifecycle work. Shared R4a logic receives a narrow
  borrowed, non-serializable semantics projection or explicit pure input
  structs, never a `MergeOperationRecordV0` created from v1;
- shared R4a pure acceptance/publication semantics and physical Git executors
  may be reused behind typed adapters;
- no v1 lifecycle code calls `persist_merge_record`,
  `persist_operation_transition`, or another raw v0 writer; and
- A1 is the sole production dispatch change that makes v1 creation/migration
  reachable.

R4b tests may enter the v1 service through a test-only harness. A production
call-graph/source gate across the entire merge implementation—not only the new
v1 directory—must prove that no v1-to-v0 owned-record conversion exists, no v1
value reaches a v0 serializer/mutator, and the normal build still has no
reachable v1 writer or upgrade path. `v0_common_view` itself is a forbidden
  source symbol after R4b-TI.

## 12. Implementation packages

R4b is not one implementation package. It is delivered in these dependency
ordered checkpoints:

| Checkpoint | Deliverable | May proceed in parallel |
| --- | --- | --- |
| R4b-TI | validator/isolation corrections, opaque checked types, sealed proof composition, transition/effect enums and executable exact-effect verification | R4b-TR starts only after two independent TI interface reviews |
| R4b-TR | reducers, next-action and observation dispatch, bound execution attempts, retirement, and exhaustive pure matrices | nothing that writes v1 starts before two independent TR reviews |
| R4b-S | strict checked store, lineage checks, unknown overlay/retirement, exact terminal-byte archive primitive | R4b-A after T interface acceptance |
| R4b-A | shared acceptance builder and frozen publication classification inputs | R4b-S |
| R4b-F | acceptance-consuming candidate/evidence/publication finalizer | R4b-X after T/S/A interfaces settle |
| R4b-X | participant execution/continue and exact recovery-origin service | R4b-F after T/S/A interfaces settle |
| R4b-P0 | transition-owned reverse-entry preview, F-owned publication handoff, production entry issuers, stable reverse/archive routing, protocol-neutral archived result | lead-owned prerequisite after S/F/X; two re-reviews before consumer lanes |
| R4b-P0.1/P0.2 | shared physical/authority closure discovered by consumer implementation | affected consumer lanes pause until two independent amendment re-reviews return GO |
| R4b-P1/P2 | preservation plus preserving recovery; rollback plus rolling-back recovery | parallel after P0 acceptance |
| R4b-P3/P4 | read-only status plus frozen I2 field-10 projection; terminal archive plus archive-worklist GC | parallel after P0 acceptance; P4 returns only the frozen handoff consumed by P3 |
| R4b-G | aggregate fault, equivalence, compatibility, call-graph, and settled-tree gates | no feature work |

Each checkpoint receives an independent interface review before dependants use
its API. R4b-TI and R4b-TR each require two independent reviews because they
jointly form the transition authority boundary. The lead owns the transition vocabulary, checked-store API, proof
token visibility, and acceptance-builder interface. Parallel agents may own
separate consumers only after those interfaces are accepted; they may not add
transition variants or widen store/proof constructors independently.

The concrete R4b-F candidate consumes only checked v1 records and frozen
acceptance. Its exact observer verifies selected participant outcomes and the
accepted root base before issuing proofs. Publication persists candidate and
phase ownership before creating the scoped evidence commit or crossing the
marker, lock, workspace-boundary, and index prefixes. The physical executor
rechecks the exact action under the retained lease immediately before each
mutation. Restart classifies every owned prefix from exact filesystem bytes,
raw stage-0 index entries, and recorded evidence; mixed or tampered forms enter
the representable `Finalizing` recovery origin without overwrite. The raw-index
classifier deliberately does not inspect worktree bytes, because marker and
lock publication precede the final index update; filesystem digests and file
types are verified independently.

F does not activate the v1 writer or migration path. Two independent reviews
accepted its finalization, selected-root, restart, and package-budget
boundaries with no P0-P3 findings. It is therefore an accepted dependency in
the exact DAG below. The P0 prerequisite has also passed both independent code
re-reviews. Their first P1-P4 consumption exposed the bounded P0.1 omissions,
which were corrected and accepted. P1 then exposed the compound
root-preservation gap addressed by P0.2. Both corrective-interface reviews
accepted its durable restoration-parent step and bounded owner manifest through
§12, then independently accepted §13's two pre-clean parent phases. The
production-disabled P0.2 implementation is present; its first settled-code
architecture review found bounded provenance, manifest, evidence, and control
defects whose remediation is accepted by both corrected-code reviews. P0.2 is
accepted locally, both amendment 2 §14 settled-implementation reviews and the
exact-SHA native release-platform run are GO, and P1 is unpaused. P2-P4 retain
completed work; R4b-G awaits accepted P1, so the aggregate gate remains
pending.

The concrete R4b-X service persists an exact participant owner before every
Git mutation and treats the durable prepared variant as the sole
validation/execution authority. It directly adopts exact up-to-date results,
records exact conflict path/hash evidence, classifies unresolved continue
before changing operation state, executes checked fast-forward and
deterministic two-parent no-ff actions, and reconciles completed actions after
restart without re-execution. Recovery verifies a pending owner and every
other selected participant, delegates `Finalizing` exactness to F, and rejects
the P-owned preservation/rollback origins. Two independent reviews accepted X
with no P0-P3 findings and approved its reviewed 1,050-line production
ceiling. X does not activate the v1 writer, migration, or production dispatch.

The dependency DAG is exact:

```text
R4b-TI -> R4b-TR
R4b-TI + R4b-TR -> R4b-S
R4b-TI + R4b-TR -> R4b-A
R4b-TI + R4b-TR + R4b-S + R4b-A -> R4b-F
R4b-TI + R4b-TR + R4b-S + R4b-A -> R4b-X
R4b-S + R4b-F + R4b-X -> R4b-P0
R4b-P0 -> R4b-P0.1
R4b-P0.1 -> R4b-P0.2 -> R4b-P1
R4b-P0.1 -> R4b-P2
R4b-P0.1 -> R4b-P3
R4b-P0.1 -> R4b-P4
R4b-P1 + R4b-P2 + R4b-P3 + R4b-P4 -> R4b-G
```

R4b-S owns only the checked atomic terminal-byte move/reconciliation primitive.
R4b-P0 freezes request/state dispatch into that primitive and the immutable
validated-archive handoff. P4 owns dispatch/reopen and GC consumption of R3's
immutable `ArchivedMergeProjection` cleanup worklist; P3 alone maps the
handoff into the already allocated field 10. Neither package adds an archive
reserializer or rewrites terminal history. The complete P interface and path
budget are frozen in `GwzM5-8R4bReverseLifecycleInterface.md`.

## 13. Required matrices and tests

R4b-TR must contain an executable predecessor matrix covering every operation
state, participant state, publication shape, pending action kind, rollback
phase, preservation phase, and recovery origin against every applicable
transition. It must prove both listed successes and every unlisted rejection.

The complete gate includes:

- exact phase-successor tests rejecting skips, reverse edges, repeated edges,
  wrong initial phases, and wrong owners;
- one footprint test per variant proving the actual known semantic diff equals
  only its declared fields;
- immutable acceptance/candidate/evidence tests for every later transition;
- outcome-without-action, result-for-wrong-action, action-for-wrong-member,
  rollback-result-before-complete, and preservation-evidence-without-owner
  negative tests;
- checked-record rejection tests for two or more forward pending actions and
  for any forward pending action in effective `Preserving`, including the
  corresponding `RecoveryRequired` origin; preparation tests prove a second
  owner cannot be installed;
- rollback decoder tests reject every wrong/later owner, kind, terminal state,
  and phase against the pure durable reverse cursor; preservation decoder
  tests reject every record-local owner/phase/pass contradiction, while bound
  cursor tests reject execution/advance of a later pending owner when any
  earlier live position is incomplete or ambiguous and accept the same durable
  shape when every earlier position is exactly complete or unnecessary;
- stale/wrong outcome-plus-entry tests for the rollback/preservation compound
  variants, including mismatched anticipated post-outcome model and publication
  handoff;
- executor-error crosses fresh `NotStarted`/`Completed`/`Ambiguous` tests for
  ordinary retry, resolve-conflict, publication, preservation, and rollback,
  proving an error alone never rewrites and no same-invocation retry loops;
- an invocation-history matrix crosses `ResumeStart` and `Continue` with
  preparation failure, physical-action failure, a newly conflicted retry,
  retained halted owners, and recovery entry; it proves one preparation and
  one execution per owner/action, later frozen targets continue after the new
  conflict, `AwaitingResolution` and `RecoveryRequired` return without
  redispatch, and a new invocation forgets every local visit fence;
- a service restart test for `ResumeStart` over `Halted` with a retained
  completed participant owner proves the halted-outcome compound records the
  outcome and resumes `Executing` without an invalid intermediate state;
- a fault/restart matrix immediately after `BeginExecution` with a retained
  failed/error row and pending owner proves ambiguous observation writes
  `Halted` first, reobserves under the new digest, and only then enters recovery
  with literal origin `Halted`; direct recovery from `Executing` with that
  origin is rejected;
- crash/restart and incoming-abort tests between every pair of preservation
  actions, proving the remaining artifact/reset cursor completes before a
  `FromPreserving` rollback entry can be constructed;
- proof-token provenance tests and ambiguity tests proving no ambiguous token
  can construct a result transition;
- source-lineage/contention tests at every checked-store boundary, including
  acceptance that an intervening same-byte replacement has identical durable
  authority while any different-byte replacement rejects; this includes the
  reopen immediately after every physical execution and before the bound
  attempt reaches another dispatch;
- unknown survivor and retirement-manifest checks after every transition;
- fault injection immediately before and after every durable owner, physical
  mutation, result, publication-prefix, terminal, and archive boundary;
- restart tests from every persisted publication, rollback, preservation, and
  recovery phase;
- publication restart/fault tests for every file prefix crossed with exact
  pre-stage, exact staged, and mixed index state, including the terminal
  `Marker` case where both candidate lock and boundary equal their baselines;
- byte-equivalent accepted lock, candidate, root output, and archive results
  across all M4 and all seven R3 adapted-v0 scenarios;
- a privacy/compile test proving lifecycle code cannot construct
  `PreparedV1Rewrite`, proof tokens, or call a raw v1 writer;
- a source/call-graph check proving v1 lifecycle modules contain no raw v0
  persistence call;
- the retained-reader, protocol, CLI/Python parity, document-consistency,
  Clippy, format, and platform release gates already required by the parent
  plan; and
- two independent settled-tree reviews with no open P0/P1/P2 finding before
  A1.

## 14. Stop conditions

Implementation stops for design review if any checkpoint requires:

- a generic record rewrite or public prepared-rewrite constructor;
- a physical mutation without a pre-existing typed persisted owner;
- a new durable phase or observation not representable by I2;
- regeneration of accepted lock or audit data from live state;
- mutation of accepted workspace, candidate, or evidence after its freeze
  point;
- a caller-provided raw proof payload;
- a same-state, skipped, reverse, or ordinal (`>=`) durable phase transition;
- routing v1 lifecycle mutation through a v0 projection;
- unknown-field movement not covered by the I2 identity/retirement rules; or
- activation of any production v1 writer or migration path before A1.

Those are architecture changes, not local implementation exceptions.

## 15. R4b exit condition

R4b is complete only when every v1 durable rewrite reachable in the test
harness is produced by the typed kernel, committed by the checked store, and
covered by the predecessor, footprint, unknown-field, restart, and fault
matrices. All finalization and reverse paths must consume the persisted
accepted workspace and exact lock bytes. The production v1 writer and R3
migration dispatch must still be unreachable.

Passing R4b does not activate v1. A1 remains a separate, small, reviewed
runtime-dispatch change after R4b and M5b meet their gates.
