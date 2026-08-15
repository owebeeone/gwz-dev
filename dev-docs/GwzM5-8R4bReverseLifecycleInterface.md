# R4b-P reverse-lifecycle interface checkpoint

Date: 2026-08-10

Amended 2026-08-16 by `GwzM5-8DurableCursorAmendment.md`: the preservation
cursor prefix is issued from durable completion facts where recorded and
live observation where not; the evidence row's `noop_commit`/`reset_commit`
markers, their write edges, and the reset-edge backfill are normative in
that amendment.

Amended 2026-08-16 by `GwzM5-8ExactEvidencePlatformAmendment.md`: the I2
raw-byte canonical preimage doctrine is unchanged, but its producing edges
are corrected — recovery-grade rewrite edges write blob-exact bytes, and the
checked-artifact private area is excluded from capture/decode and from the
rollback/merge cleanliness predicates. Scope limits and open decisions
(filtered-materialization exposure; foreign-filter policy) are recorded in
that amendment.

Status: **P0 and P0.1 accepted after independent re-review; P2-P4 are
implemented; both independent reviewers accepted amendment 2 through §13 and
the production-disabled P0.2 implementation and bounded settled-code
remediation are accepted by both corrected-code reviews with no open P0-P3
finding; the native release-platform gate is GO; P1 is implemented with local
tests and static analysis green and awaits settled-tree review**

This checkpoint freezes the ownership and consumer interfaces for R4b-P. It
implements preservation, rollback, reverse-origin recovery, read-only open and
archived status, terminal archival, and archive-worklist GC. The normative
behavior remains in `GwzM5-8I2ActionJournalContract.md` and
`GwzM5-8R4bTransitionDesign.md`; this document assigns that behavior to
concrete owners without changing it.

## 1. Decision

R4b-P is one dependency checkpoint with a lead-owned shared prerequisite and
four implementation lanes after this interface is accepted:

1. P0 freezes reverse-entry preview, publication handoff, production proof
   issuance, the reverse router, and the archive-to-projection handoff;
2. P1 owns preservation observation/execution and preserving-origin recovery;
3. P2 owns rollback observation/execution and rolling-back-origin recovery;
4. P3 owns read-only open/archive status and the complete frozen I2 field-10
   projection across core and both drivers; and
5. P4 owns terminal archive orchestration plus immutable archive-worklist GC.

The owners share the accepted `V1LifecycleRequest`, `V1NextAction`,
`ResolvedV1Action`, `V1Transition`, proof-token, checked-store, and service-loop
interfaces. They do not add a lifecycle request, durable phase, journal
variant, transition variant, unallocated protocol shape, generic writer, or
mutable v1 record view. The already frozen I2 protocol projection is addressed
explicitly in §9; it is not a license to invent another response form. R4b-P
remains reachable only through the test-only v1 boundary; A1 alone may activate
production v1 creation, migration, or dispatch.

The lead-owned interface freeze is:

- the closed dispatcher and reducer vocabulary is unchanged;
- production proof issuance is added only inside
  `v1_lifecycle/authority/observe/` and returns the existing sealed proof
  types;
- the transition owner supplies the only anticipated-model preview used by
  reverse entry; observers never recreate reducer logic;
- `CheckedV1Store` keeps its existing open-load, checked-commit, unchanged-
  reload, and exact-byte archive primitives;
- the existing service loop remains the sole interpreter of v1 lifecycle
  actions;
- a reverse runtime implements the existing `ExactObserver` and
  `PhysicalExecutor` traits; and
- status and archive-worklist GC remain outside the mutating open-record loop
  because status is read-only and archived records are immutable history, not
  open lifecycle state.

Any implementation need for a new durable field, phase, transition, physical
action kind, proof constructor visible to a consumer, checked-store mutation
entry point, or v1-to-v0 record conversion stops R4b-P for design review.

## 2. Dependency and authority boundary

R4b-P consumes the accepted R4b-S, R4b-F, and R4b-X interfaces:

- S supplies checked record lineage, transition commits, unchanged reload,
  unknown-field survival/retirement, and the exact terminal-byte archive
  primitive;
- F supplies accepted-workspace, candidate, publication-prefix, root-evidence,
  and finalization recovery observation;
- X supplies participant path validation, pending-forward-action
  reconciliation, checked participant integration execution, and forward
  recovery observation; and
- TR remains the only owner of durable dispatch, observation resolution,
  phase succession, and transition footprints.

R4b-P may extract version-neutral borrowed semantic inputs and checked physical
helpers from the v0 preservation, abort, status, and GC implementations. Such
inputs must be non-serializable and must borrow the exact fields they need.
They may not expose a mutable record, project v1 into `MergeOperationRecord`,
or become a second lifecycle dispatcher. V0 orchestration continues to own v0
persistence; v1 orchestration continues to own only typed transitions through
the checked service.

The existing `GitBackend` does not yet expose the raw, non-UTF-8-safe facts
required by the accepted I2 preimage and exact-checkout contract. This
checkpoint therefore permits one lead-frozen, read-only Git observation seam:

```text
GitPreservationDirtySummary {
  staged: bool, unstaged: bool, untracked: bool
}
GitPreservationImage {
  preimage_sha256: Sha256Digest,
  dirty: GitPreservationDirtySummary
}
GitPreservationStashEvidence {
  object_id: GitObjectId,
  message: String,
  head_commit: GitObjectId,
  image: GitPreservationImage
}

preservation_image(path, include_untracked)
  -> ModelResult<GitPreservationImage>
preservation_stashes(path, merge_id)
  -> ModelResult<Vec<GitPreservationStashEvidence>>
checkout_matches_commit(path, branch, commit)
  -> ModelResult<bool>
```

`preservation_image` computes the exact I2 canonical binary grammar inside the
Git implementation, before any lossy path conversion, and rejects every
forbidden index flag/form. `preservation_stashes` reconstructs the same exact
preimage facts and dirty summary from each named native stash's trees and
returns every matching stable-id candidate; it does not classify one as
authoritative. The bundle classifier uses that decoded `image` for the current
row and for every completed canonical prefix row after restart; a hash alone
is not reconstruction authority. `checkout_matches_commit`
requires attached HEAD/ref, clean native repository state, and exact index and
worktree equality to the named commit. These methods are read-only facts;
authority-owned observers still decide before/after/ambiguous. No raw
repository handle, mutable index, proof token, or write callback crosses the
seam. Operational acquisition failures retain their existing typed
`ModelError`; they are never collapsed into semantic mismatch or `false`.

## 3. Lead-owned reverse-entry and routing prerequisite

P0 lands before P1-P4 start. It is an interface closure, not a semantic lane.
The following conceptual surface is frozen; exact Rust lifetimes and wrapper
names may vary, but ownership, inputs, and opacity may not:

```text
ReverseEntryPredecessor<'a> =
    ActionFree
  | ParticipantOutcome(&'a VerifiedParticipantOutcome)
  | ParticipantNotStarted(&'a VerifiedParticipantNotStarted)

transition::preview_reverse_entry(current, request, predecessor)
  -> ModelResult<PreparedReverseEntryView>

transition::visit_reverse_entry(
  ReverseEntryInspectionPermit, current, preview,
  visitor: &mut V where V: SealedReverseEntryVisitor)
  -> ModelResult<V::SealedAuthority>

finalization::observe_reverse_publication_handoff(
  backend, context, current, preview)
  -> ModelResult<RecordEvidenceOr<VerifiedPublicationHandoff>>

preservation::preflight_entry(backend, context, current, preview)
  -> ModelResult<VerifiedPreservationEntryPreflight>
rollback::preflight_entry(backend, context, current, preview)
  -> ModelResult<VerifiedRollbackEntryPreflight>

reverse::entry::prepare_preservation_entry(
  current, preview, handoff, VerifiedPreservationEntryPreflight)
  -> ModelResult<PreparedPreservationEntry>
reverse::entry::prepare_direct_rollback_entry(
  current, preview, handoff, VerifiedRollbackEntryPreflight)
  -> ModelResult<PreparedRollbackEntry>
reverse::entry::prepare_exhausted_rollback_entry(
  current, preview, handoff, VerifiedRollbackEntryPreflight,
  VerifiedPreservationExhausted)
  -> ModelResult<PreparedRollbackEntry>
```

`PreparedReverseEntryView` is opaque, non-serializable, and created only by
the transition owner by applying the exact predecessor effect in memory. It
binds the source digest, predecessor identity, exact lifecycle request, exact
anticipated action-free model digest, and request-specific entry kind.
Consumers may inspect only the request, entry kind, and anticipated digest
required for authority matching and error reporting; they cannot obtain or
mutate the anticipated record. The same reducer implementation that will
commit the compound transition verifies this digest. P1/P2 never copy a
participant outcome into a model or predict a transition themselves.

The transition-owned visitor bridge is the sole exception to that consumer
opacity. `ReverseEntryInspectionPermit` is non-cloneable/non-serializable and
constructible only by `AuthorityIssuer`; `SealedReverseEntryVisitor` is sealed
inside the authority-observer subtree and its associated result is bounded by
a second private marker implemented only for the F handoff and the P1/P2
preflight authorities. The bridge validates current lineage and preview
binding, then lends the anticipated model only for the visitor call. The
borrow cannot escape, and the result type is one of those closed sealed
authorities rather than a record, projection, or caller-selected payload.
Runtime/service/physical modules cannot obtain a permit, implement the visitor,
or call the bridge.

The request-neutral finalization observer uses the same bridge through its
sealed publication-handoff visitor and reuses F's accepted participant,
candidate, evidence, publication-prefix, selected-root, and no-publication
checks. One F read returns either the existing evidence result that must be
durably recorded first or a closed `PublicationHandoffFact`: exact
no-candidate/evidence-pending state or the exact candidate file prefix plus
index form. The sealed `VerifiedPublicationHandoff` binds that fact to the
current checked bytes and preview. P1/P2 reobserve the same closed fact while
performing their complete all-member/root/artifact preflight against the
anticipated model and issue only the matching sealed authority. The entry
issuers alone construct the three production entry authorities. The tokens
bind source digest, exact request, entry kind, anticipated-model digest, and
publication fact. No caller-provided eligibility boolean or unsealed result is
accepted. Rollback from preserving additionally consumes the exact
`VerifiedPreservationExhausted`; a direct constructor cannot accept that proof
and the exhausted constructor cannot omit it.

Direct Preserve and Abort previews are legal only from `Executing`,
`AwaitingResolution`, `Halted`, or `Finalizing`. Terminal, `RollingBack`, and
`RecoveryRequired` records issue no preview authority. `Preserving` retains the
already accepted dispatcher behavior: every non-Status request completes the
same preservation cursor and may enter rollback only after fresh exact
preservation-exhaustion proof. Those requests are not interchangeable—the
preview, handoff, preflight, and entry all bind the exact triggering request—
but P0 does not change the command-independent requirement to finish an
already-started preservation sequence safely.

P0 also installs one stable router with private P0-owned concrete delegates to
the preservation, rollback, and archive modules. A pure closed route selector
is testable without permitting consumer-defined lane implementations. P1, P2,
and P4 fill only their predeclared observer/executor functions without changing
the match, `v1_lifecycle/mod.rs`, or another lane's module.
`reverse::observe` delegates only to the selected lane's ordinary or
origin-specific recovery method, and `reverse::execute` delegates only to the
selected lane's physical method. Production reverse-entry issuance remains
unavailable until P1/P2 install the matching sealed preflight issuer. P4
returns this protocol-neutral handoff to P3:

```text
ValidatedArchivedMerge {
  source_version: MergeRecordVersion,
  destination_bytes: ImmutableBytes,
  destination_sha256: Sha256Digest,
  projection: ArchivedMergeProjection,
  cleanup: ArchiveCleanupWorklist
}
```

The value is constructible only from a non-cloneable P4-owned
`CanonicalArchiveAcquisition` after canonical no-follow destination acquisition
and successful R3 decode of those exact bytes. It retains R3's single
`ValidatedArchivedRecord` and `ArchivedCleanupWorklist`; it never copies or
accepts an independently supplied projection/worklist. P4 owns the private
capability constructor and lifecycle/archive semantics; P3 alone maps the
immutable projection into field 10. The lead owns
`transition/reverse_entry.rs`, the entry issuers, the finalization handoff, the
router, and `v1_lifecycle/archive_result.rs`. A lane that needs to widen any of
them stops for lead review.

## 4. Reverse runtime interface

`ReverseRuntime<'a, B>` is the sole reverse-lane implementation of the existing
service traits. It holds only a borrowed `GitBackend` and `OperationContext`.
It owns no durable state and cannot persist a record. Archive routing remains
the separate P0/P4 wrapper frozen in §3.

Its observation dispatch is closed:

| Observation kind | Owner |
| --- | --- |
| participant action under `Abort` or `Preserve` | reverse compound observer |
| preservation entry or cursor | preservation observer |
| rollback entry or cursor | rollback observer |
| recovery with `Preserving` origin | P1 preserving-origin verifier |
| recovery with `RollingBack` origin | P2 rolling-back-origin verifier |
| recovery with a forward/finalizing origin | accepted X/F verifier delegation |
| archive | terminal archive observer |
| any forward preparation, forward-only publication, or other kind | typed phase rejection |

Its physical dispatch is also closed:

| Physical action | Owner |
| --- | --- |
| `Preservation(action)` | preservation executor |
| `Rollback(action)` | rollback executor |
| `Archive` | existing service/store special case; never executed by the runtime |
| participant or forward publication action | typed phase rejection |

Every executor first checks the retained `V1MutationLease`, then re-verifies
the complete persisted action and its immediate live precondition. An executor
diagnostic remains non-authoritative: only the fresh post-action observation
can complete, advance, or enter recovery.

## 5. Abort/preserve entry and retained forward owners

Abort and preserve must never execute a retained forward participant action.
The reverse compound observer therefore owns
`ObservationKind::ParticipantAction` for those two requests and follows this
operation-state-exact matrix:

| Durable state | Exact live class | Required fact and transition |
| --- | --- | --- |
| `Executing` | completed outcome or exact expected conflict | `VerifiedParticipantOutcome + EntryFact::None`; ordinary `RecordParticipantOutcome`; the next checked digest performs fresh global entry preflight |
| `Executing` | not started | `VerifiedParticipantNotStarted` plus matching preview/entry; atomic `AbandonNotStartedAndBegin*` |
| `Executing` without retained halt cause | ambiguous | `EnterRecovery(origin=Executing)` with owner retained |
| `Executing` with retained halt cause | ambiguous | persist `Halt`, discard the stale proof, and reobserve; only then may `RecoveryRequired(origin=Halted)` be entered |
| `Halted` | completed outcome or exact expected conflict | `VerifiedParticipantOutcome` plus matching preview/entry; atomic `RecordHaltedOutcomeAndBegin*` |
| `Halted` | not started | `VerifiedParticipantNotStarted` plus matching preview/entry; atomic `AbandonNotStartedAndBegin*` |
| `Halted` | ambiguous | `EnterRecovery(origin=Halted)` with owner retained |
| either | wrong owner, foreign conflict, mixed or third form | typed rejection/ambiguity; no entry proof and no owner retirement |

Every row is crossed with Abort and Preserve. The resolver selects only the
already-frozen transitions. The no-two-write rule applies to not-started
abandonment and to completed owners while `Halted`; it does not prohibit the
frozen `Executing -> RecordParticipantOutcome -> fresh entry preflight`
sequence.

Entry preparation is a complete read-only global preflight over every selected
participant, selected root, publication handoff, and relevant preservation or
rollback artifact. It is bound to the current checked digest and to the exact
anticipated post-outcome/action-free model. Failure in any member returns
before the entry transition or any reverse physical mutation. Mandatory
retained-owner outcome reconciliation and publication-evidence recording may
precede it under the frozen dispatcher. This preserves the multi-repository
guarantee for the reverse operation: no easy member starts reverse mutation
before a later member makes the entry ineligible.

`BeginRollback` from `Preserving` is available only after the preservation
observer has verified the complete two-pass cursor and issued the existing
`VerifiedPreservationExhausted` proof. An incoming abort cannot abandon a
partially completed preservation sequence.

## 6. Preservation owner

The preservation observer implements the exact I2 two-pass cursor:

1. artifact pass in frozen owner order: backup ref, then stash/bundle;
2. whole-prefix verification; and
3. reset pass in the same owner order.

The owner set is the selected targets in durable order plus
`PublicationRoot` only when composition evidence exists and no selected root
participant owns the same root ref. Every owner path and derived private ref is
validated before repository access. Before any pending action is classified,
advanced, or executed, the observer issues the existing
`VerifiedPreservationCursorPrefix` only after every earlier position is
exactly complete or exactly unnecessary — durably where recorded, live
where not.

The observer alone derives the existing pending payloads from immutable record
evidence and exact live facts. It enforces the I2 raw-byte canonical preimage
grammar, including non-UTF-8 Git path ordering and rejection of assume-valid,
skip-worktree, intent-to-add, duplicate path/stage, or unknown semantic flag
forms. YAML, JSON, display paths, or lossy strings are never hash input.

The physical executor performs only the mutation named by the persisted phase.
For a root handoff, `GwzM5-8R4bInterfaceAmendment-2.md` is controlling and
replaces each compound normalization/restoration step with independent marker,
lock, and raw-index phases:

- exact idempotent backup-ref creation;
- root marker, lock, and index normalization to exact commit-derived clean
  forms;
- native stash creation after immediate exact preimage recheck;
- root index, lock, and marker restoration to the exact durable handoff;
- canonical bundle creation from the already durable stash ids; or
- checked attached-ref reset followed, when required, by exact handoff
  restoration.

Each phase uses the exact I2 before/after classifier. A matching after state
advances without re-execution; a matching before state authorizes one physical
action; every third or mixed state is ambiguous. Bundle writes are atomic and
never overwrite a noncanonical or differently populated bundle. Native stashes
and bundles survive abort, archive, and GC.

## 7. Rollback owner

The rollback observer derives the sole current owner from the existing pure
cursor in this exact order:

1. publication evidence;
2. selected participants in reverse durable order; and
3. selected-root manifest/lock metadata.

No-mutation participants use the existing `RecordNoMutationAbort` transition
and never acquire a fake journal. Every mutating owner is prepared, committed,
reread, observed, and only then executed.

Participant rollback consumes only the immutable participant row:

- `AbortConflict` accepts only the exact recorded native merge as before and
  the exact clean `before_commit` checkout as after;
- `ResetIntegrated` accepts only the exact clean recorded result as before and
  the exact clean `before_commit` checkout as after.

Publication evidence advances exactly
`EvidenceCommit -> Boundary -> Lock -> Marker -> Index -> Complete`.
Selected-root metadata advances exactly `Manifest -> Lock -> Complete` and
uses operation-baseline manifest/lock bytes even before acceptance exists;
persisted acceptance, when present, is an additional exact cross-check.

The executor maps each persisted step to one checked Git/filesystem mutation.
It cannot collapse steps or perform cleanup belonging to a later step. After a
selected-root `Complete` journal clears, the observer rereads exact baseline
manifest and lock bytes before issuing `VerifiedRollbackExhausted`; the absence
of a separate completion bit never permits a blind repeat or early abort.

Every root manifest, lock, boundary, marker, candidate, and index observation
uses canonical real parents, a regular non-symlink leaf, exact bytes, and the
accepted executable/mode rule where applicable. An operational read failure is
an error, not `false`. A symlink, directory, executable/noncanonical form, or
mixed prefix issues no proof. This replaces the current byte-only placeholder
reads for all P rollback/preservation issuers and final rollback exhaustion.

## 8. Reverse-origin recovery

`RecoveryRequired(origin=Preserving)` is legal only with the exact retained
preservation journal; `origin=RollingBack` is legal only with the exact retained
rollback journal. P1's preserving-origin verifier reuses P1's preservation
phase classifier and earlier-prefix proof. P2's rolling-back-origin verifier
reuses P2's pure rollback cursor and phase classifier. Only an exact
representable state issues the existing `VerifiedRecoveryOrigin` token. The
shared router is integrated only after both verifier interfaces settle;
neither lane implements a compatibility classifier for the other.

Between-action ambiguity in `Preserving` or `RollingBack` remains
unrepresentable by v1 and returns the typed evidence/recovery error without a
rewrite. Recovery never fabricates an owner, completes an action from the
executor result, or changes the recorded literal origin.

Forward and finalizing recovery continue to delegate to the accepted X/F
verifiers. The reverse runtime may route to them but may not duplicate their
state policy.

## 9. Read-only status

Status has two sources and no mutation path:

- an open v1 record is loaded through `CheckedV1Store`, then projected from the
  checked typed record plus read-only live observations; and
- a named archived v0/v1 record is decoded solely from archive bytes through
  the R3 archive decoder and projected without consulting live repositories.

Source arbitration is exact:

| Open source | Archive destination | Status result |
| --- | --- | --- |
| absent | absent | named not-found or unqualified idle |
| valid nonterminal | absent | checked open projection plus live observation |
| exact terminal | absent | open projection with `archived=false` |
| absent | valid terminal | archive-only projection with `archived=true`; no live access |
| exact terminal | identical terminal bytes | open projection with `archived=false`; status does not reconcile |
| any source | different or malformed destination | typed contradiction; no mutation |
| nonterminal source | any present destination | typed contradiction; no mutation |
| noncanonical/symlink parent or leaf | any | typed unreadable; no live access |

Open status reports the exact operation state, recovery origin, pending
forward/reverse owner and phase, participant outcomes, persisted drift,
read-only live drift, continue/abort eligibility, preservation evidence,
publication step, and acceptance availability. It never calls `service::run`
merely to obtain a status response and never acquires a mutation lease.
Version-neutral participant observation is shared through a borrowed status
view; there is no v1-to-v0 record conversion.

Because status intentionally holds no mutation lease, an open status load has
optimistic lineage: load checked source, collect the complete live snapshot,
then perform an unchanged-source reread. If bytes changed, discard every live
fact and retry the whole read once or return the typed contention/recovery
error. A response never combines one durable digest's lifecycle with another
digest's observations.

Archived status reports the R3 `ArchivedMergeProjection`, including source
version, sibling terminal outcome and acceptance variant. It never reconstructs
history from current repositories, upgrades an archive, or rewrites bytes.
Unsupported future envelopes fail at header classification and remain
untouched.

P3 implements the user-facing projection exactly as the accepted I2 protocol
contract:
`MergeResponse` optional field 10 `record`, the pinned `MergeRecordProjection`
and nested acceptance/recovery messages, and only the v0/v1 enum
discriminants allocated there. A durable-record success includes field 10;
idle/dry-run/pre-record/unqualified-GC responses omit it. Rust, Python, JSON,
JSONL, human rendering, fixtures, and generated bindings must agree. P3 owns
the Taut append, generated Rust/Python bindings, the one semantic projection
owner, response population, renderers, and parity fixtures before A1. It
projects open v0, open v1, archived v0, and archived v1 and populates field 10
for start after creation, open/archived status, continue, preserve-abort,
abort, and id-qualified GC successes. A1 does not finish this contract; it
only activates v1 creation/migration/dispatch after the matrix passes.

## 10. Terminal archive service

The archive consumer is an outer orchestration wrapper around the existing
checked archive physical action. Source-only and both-copy paths first obtain
the existing bound `Archive` observation, resolve it, invoke S's exact-byte
primitive, and then reopen the destination. Destination-only uses only the
final reopen/decode path. The wrapper handles the three restart shapes:

| Filesystem shape | Result |
| --- | --- |
| exact terminal open source only | run the checked v1 archive action, then decode the destination |
| exact identical open and destination copies | run checked reconciliation, removing only the source, then decode the destination |
| destination only | decode and return the immutable archived projection; do not require an open record |

Different source/destination bytes, a nonterminal source, a malformed archive,
a noncanonical path, or a symlinked destination directory rejects without
deleting either copy. The returned response is derived by reopening the
destination, not from a stale pre-rename in-memory model. No archive
reserializer exists. Every source and destination acquisition proves canonical
real parent directories and a regular non-symlink named leaf before decoding;
P4 returns `ValidatedArchivedMerge` rather than a response, and P3 alone maps
that value to field 10.

## 11. Archive-worklist GC

Targeted GC acquires the workspace mutator lock, proves that no open merge is
present, and acquires the named archive through the same canonical no-follow
reader: the workspace root and `.gwz/merge/done` parent chain are real
canonical directories, the validated merge id selects exactly its `.yaml`
leaf, and that leaf is a regular non-symlink file. Only those authority bytes
may produce the validated R3 projection and immutable cleanup worklist. Before
deleting any ref it validates every owner path/name/target and preflights every
repository:

- absent ref: already complete;
- exact recorded target: eligible for checked deletion;
- different target or unavailable repository: stop and retain the archive.

After a successful full preflight, each eligible ref is deleted through
`delete_backup_ref_checked`. A crash may leave a proper subset absent; restart
treats those rows as complete and continues from the same unchanged archive.
Before deleting the archive, GC rereads and revalidates the exact same archive
through the same no-follow acquisition, proves byte identity and the same
projection/worklist, and observes every owned ref absent. Only then may it
remove the terminal file and fsync the archive directory. A symlinked parent or
leaf, nonregular leaf, path mismatch, or replacement retains the archive and
every remaining ref.

Archives with no backup refs may be deleted immediately after archive-only
validation. Native stashes and preservation bundles are never deleted.
Ordinary retention exempts any archive whose validated worklist owns backup
refs and retains any unreadable or unsupported-future archive. GC never clears
evidence by rewriting terminal history.

## 12. Planned implementation ownership

The path manifest is fixed before lane work. A path may be removed when an
implementation proves unnecessary, but a lane may not add another production
path or edit another lane's path without lead review:

| Owner | Production path manifest | Responsibility |
| --- | --- | --- |
| P0 lead prerequisite (29) | `v1_lifecycle/{mod,reverse,archive_result,status,archive}.rs`; `v1_lifecycle/reverse/{preservation,rollback,execute}.rs`; `v1_lifecycle/reverse/execute/{preservation,rollback}.rs`; `transition/{mod,reverse_entry}.rs`; `transition/reduce/{mod,participant}.rs`; `authority.rs`; `authority/{dispatcher,observe}.rs`; `authority/observe/{finalization,reverse}.rs`; `authority/observe/finalization/{handoff,publication}.rs`; `authority/observe/reverse/{entry,preservation,preserving_recovery,rollback,rolling_back_recovery}.rs`; `record_wire/mod.rs`; `record_wire/archive/{mod,cleanup}.rs` | request/preview/handoff/tokens, closed publication fact, concrete router delegates, compiling lane skeletons, R3-backed archive result; reducer paths expose existing predecessor helpers without policy duplication |
| P0.2 root preservation (23) | `Cargo.toml`; `git/gitbackend.rs`; `git/gitbackend/{contract,preservation,preservation_image,preservation_root,repository_support}.rs`; `git/gitbackend/preservation_root/{files,index,index_format,parent}.rs`; `merge/model/v1/journal.rs`; `merge/model/v1/validate/preservation.rs`; `merge/v1_lifecycle/transition/reduce/preservation.rs`; `v1_lifecycle/{authority,reverse}.rs`; `v1_lifecycle/authority/{observe,resolver,resolver/observation,resolver/execution}.rs`; `v1_lifecycle/authority/observe/reverse.rs`; `v1_lifecycle/authority/observe/reverse/preservation.rs`; `v1_lifecycle/reverse/execute/preservation.rs` | exact physical forms, checked root steps, flattened durable phases, validation, adjacency, capability filesystem dependencies, cohesive parent namespace owner, repository-format support, and causal post-barrier resolution; the four parent export surfaces are charged even though P0 already owns them |
| P1 preservation (29) | `Cargo.toml`; `git/gitbackend.rs`; `git/gitbackend/{contract,preservation,preservation_image,preservation_root,repository_support}.rs`; `git/gitbackend/preservation_root/{files,index,index_format,parent}.rs`; `merge/preserve/{plan,artifacts}.rs`; `v1_lifecycle/{authority,reverse}.rs`; `authority/{observe,resolver,resolver/observation,resolver/execution}.rs`; `authority/observe/reverse.rs`; `authority/observe/reverse/{preservation,preserving_recovery}.rs`; `authority/observe/reverse/preservation/{cursor,entry,phase}.rs`; `authority/observe/reverse/preservation/phase/{evidence,steps}.rs`; `v1_lifecycle/reverse/preservation.rs`; `v1_lifecycle/reverse/execute/preservation.rs` | raw facts, sealed entry preflight, causal durability resolution and its parent export surfaces, two-pass observer/executor, preserving verifier; the five child owners are the lead-approved cohesion split of the retained preservation router |
| P2 rollback (9) | `merge/abort/{preflight,evidence,participants}.rs`; `merge/root/{abort,artifact_facts}.rs`; `authority/observe/reverse/{rollback,rolling_back_recovery}.rs`; `v1_lifecycle/reverse/rollback.rs`; `v1_lifecycle/reverse/execute/rollback.rs` | pure cursor, no-follow root facts, sealed entry preflight, observer/executor, rolling-back verifier |
| P3 status/protocol (18) | `protocol/gwz.taut.py`; generated `src/protocol/generated.rs`, `gwz-py/src/gwz/protocol/generated/{api.py,gwz.ir.json}`; `src/protocol/convert.rs`; `merge/model/{mod,record_projection}.rs`; `merge/{response,status/snapshot}.rs`; `merge/start/response.rs`; `v1_lifecycle/status.rs`; `gwz-cli/src/{git_status_json,merge_render}.rs`; `gwz-py/src/gwz/{cli_render.py,cli_merge.py,cli_render_parts/{machine,merge}.py}`; `gwz-py/native/src/dispatch/merge.rs` | one v0/v1 semantic projector, optimistic status, field 10 and renderers |
| P4 archive/GC (8) | `v1_lifecycle/archive.rs`; `v1_lifecycle/store/{mod,rewrite}.rs`; `record_wire/archive/{mod,cleanup}.rs`; `merge/gc.rs`; `merge/store/{gc,retention}.rs` | canonical archived result, source/destination arbitration, immutable-worklist retention and GC |

No file is assigned both lifecycle policy and physical implementation. The
1,000-line general threshold is a cohesion review trigger, not an automatic
split. These boundaries are planned now so an owner that becomes a policy
dumping ground is split before the threshold; when a split is warranted, the
resulting responsibility owners target fewer than 500 lines.

## 13. Parallel lanes and merge order

P0 is implemented and focused-tested first, then returned to the same two
reviewers. Only after they accept its signatures may work proceed in these
lanes:

- P1: shared preservation facts, preservation observer/executor, and
  preserving-origin recovery;
- P2: rollback observer/executor and rolling-back-origin recovery;
- P3: read-only status and the complete frozen cross-driver field-10
  projection;
- P4: archive wrapper and archive-worklist GC.

P1 and P2 may share only P0's entry/handoff/router types. P3 is read-only and
may not import the reverse runtime. P4 resolves open `Archive` observation only
through P0, calls S for mutation, and returns `ValidatedArchivedMerge`; it may
not map a response or import open-record reducers. P3 alone consumes that
handoff for response projection.

P1-P4 initially proceeded concurrently after P0/P0.1. P2-P4 are now complete.
Both corrective-interface reviews accepted the durable restoration-parent step
and bounded parent/test owner amendment through §12, then accepted §13's
pre-clean parent edges. The production-disabled P0.2 implementation and bounded
remediation are accepted by both corrected-code reviews with no open P0-P3
finding. The native release-platform gate is GO and P1 is unpaused.
Integration lands the shared router only after accepted P1 and P2, then P3/P4's
archive handoff, then the aggregate service/restart and cross-driver matrices.
A lane that needs to widen a shared interface pauses while independent lanes
continue; it does not land a local compatibility shim.

The only aggregate production paths are `v1_lifecycle/mod.rs`,
`merge/runtime/dispatch.rs`, and `merge/mod.rs`, all lead-owned and integrated
after lane files settle. A P0 skeleton transfers to its named lane after the P0
code re-review; this is an explicit ownership handoff, not concurrent editing.

## 14. Required focused evidence

The R4b-P checkpoint must add at least:

- P0 reverse-entry state/request closure, cross-request/digest/stale-authority
  rejection, a source gate for the sealed visitor, every exact F publication
  prefix/index plus evidence-first and mixed-shape handling, closed observation
  and physical routing, and real v0/v1 R3-decoded archive-result provenance;

- preservation before/after/ambiguous tests for every I2 phase, root-prefix
  form, non-UTF-8 preimage vector, special index flag, bundle form, and cursor
  prefix;
- crash/restart and incoming-abort tests between every preservation action,
  proving preservation exhaustion precedes rollback entry;
- rollback before/after/ambiguous tests for both participant actions, every
  evidence step, every selected-root step, no-mutation participants, and final
  exact exhaustion;
- retained-forward-owner compounds crossed with Abort/Preserve and
  completed/not-started/conflicted/ambiguous observation;
- reverse recovery tests for every pending phase and rejection of
  between-action ambiguity without rewrite;
- multi-member global-preflight failures proving no earlier member, record,
  ref, index, worktree, root file, stash, or bundle changes;
- open status byte-identity tests across every state, pending owner, and
  recovery origin, plus archive-only status with repositories absent;
- the complete frozen I2 field-10 Rust/Python/JSON/JSONL/human parity matrix,
  including every open/archived v0/v1 source and every I2 presence/omission
  case;
- archive restart tests for source-only, both-copy, destination-only,
  mismatch, nonterminal, and symlink forms; and
- archive-worklist GC tests for no refs, all absent, exact refs, partial crash,
  mismatch, missing repository, stash/bundle retention, unreadable archive,
  and unsupported future archive.

Every physical action crosses executor `Success` and `Failed` with fresh
`NotStarted`, `Completed`, and `Ambiguous` observations. Tests assert that
diagnostics alone never advance a journal and that one invocation never
reexecutes the same physical action.

The executable non-root preservation matrix is:

| Position | Exact before | Exact after | Third/mixed |
| --- | --- | --- | --- |
| backup ref | absent | exact derived ref/target | different target or unreadable |
| `CreateStash` | no matching stash; current image equals preimage | one matching typed object/message/HEAD/decoded image; clean post-stash | duplicate/foreign/disagreeing stash or residual/new work |
| `WriteBundle` | absent or exact earlier-row canonical prefix | exact prefix plus current decoded-image row | any other bytes/row/order/identity |
| stash complete | exact bundle plus native evidence | proof only; no action | missing/changed evidence |
| reset ref | exact attached expected commit and clean post-stash checkout | exact attached restore commit and clean anchor checkout | detached, dirty, native state, foreign ref/index/worktree |

For a root handoff, the complete marker/lock/index/stash/reset phase table and
disjoint `After`-first classifier in amendment 2 §3.4 replace the removed
`NormalizeRoot`/`RestoreRoot` rows. Every position requires
`VerifiedPreservationCursorPrefix`; reset requires a whole-artifact-prefix
proof and exhaustion requires both passes complete. Run the matrices for root
handoff absent/present, participant versus publication root, SHA-1/SHA-256,
non-UTF-8 paths, forbidden index forms, and restart on both sides of physical/
result boundaries.

The executable rollback matrix is:

| Cursor owner | Exact order | Terminal rule |
| --- | --- | --- |
| publication evidence | `EvidenceCommit -> Boundary -> Lock -> Marker -> Index -> Complete` | one checked mutation per persisted phase; completion alone sets evidence rolled back |
| participant, reverse durable order | `AbortConflict` or `ResetIntegrated` from immutable row | exact clean before/after checkout; finish writes only the frozen result fields |
| no-mutation participant | `Planned`, `UpToDate`, `Failed`, or `Unattempted` | `RecordNoMutationAbort`; no fabricated journal/Git action |
| selected-root metadata | `Manifest -> Lock -> Complete` | operation baseline is authority; acceptance only cross-checks |
| exhausted | no pending journal and pure cursor complete | canonical exact selected-root reread, then proof and `AbortOperation` |

Every mutating row crosses exact before/after, dirty/mixed, foreign ref/HEAD,
native state, noncanonical parent/leaf, symlink, operational error, and crash
on both sides of each physical and durable boundary.

The executable recovery matrix is:

| Literal origin | Required owner/verifier | Result |
| --- | --- | --- |
| `Preserving` | exact pending preservation action; P1 classifier and prefix | exact representable before/after issues proof; ambiguity/error leaves row unchanged |
| `RollingBack` | exact pending rollback action; P2 cursor and classifier | exact representable before/after issues proof; ambiguity/error leaves row unchanged |
| forward origin | exact forward owner/selected set; accepted X verifier | delegate without P policy |
| `Finalizing` | exact finalization shape; accepted F verifier | delegate without P policy |
| reverse between actions | no pending reverse journal | typed error; no rewrite or fabricated owner |
| any mismatch | wrong/missing owner or resume action | reject before proof or mutation |

The executable GC matrix includes unreadable/unsupported/noncanonical archive,
invalid or colliding owner rows, unavailable repositories, different targets,
all refs absent, exact refs, a proper absent prefix after crash, archive change
after preflight, ref reappearance, no-ref stash/bundle-only archives, and
unlink-success/directory-fsync-failure. Every rejected row retains the archive
and all not-already-absent refs; successful partial progress retains identical
archive bytes until the final all-absent deletion boundary. Run v0/v1 and
SHA-1/SHA-256 rows, including symlinked `done` parent and archive leaf.

## 15. Budget allocation and stop gate

The first 1,450/1,600/2,600 budget is withdrawn. It predated the production
entry closure and treated the already frozen field-10 contract as optional.
The measured rebaseline uses the current 1,276-line v0 preservation owners,
accepted F at 1,989 production/1,681 focused tests, accepted X at 987/1,426,
and the concrete path manifest above. Generated LOC remains excluded, but the
schema and generated paths are counted. The reviewed stop ceilings are:

| Slice | Net production | Moved production | Test/tool/doc | Production path charges | Test/tool/doc path charges |
| --- | ---: | ---: | ---: | ---: | ---: |
| P0 shared prerequisite | 1,200 | 0 | 4,200 | 29 | 21 |
| P1 preservation/recovery | 950 | 1,050 | 1,800 | 9 | 8 |
| P2 rollback/recovery | 900 | 650 | 1,800 | 9 | 8 |
| P3 status/protocol | 900 | 250 | 2,500 | 18 | 13 |
| P4 archive/GC | 750 | 100 | 1,500 | 8 | 8 |
| aggregate wiring, fault matrix, and checkpoint docs | 100 | 0 | 1,500 | 3 | 12 |
| **Charged ceiling** | **4,800** | **2,050** | **13,300** | **76** | **70** |

The conservative unique manifest is at most 63 production-bearing paths and
69 test/tool/doc paths; shared paths are charged to every slice that changes
them. The P3 production count includes the schema and three generated outputs;
its LOC ceiling excludes mechanical generated LOC. Its test/tool/doc manifest
is limited to the protocol golden/corpus/parity fixtures, core protocol and
merge projection tests, Rust CLI merge-render tests, Python protocol/native/
renderer tests, and `MachineOutput.md`. P1/P2 test manifests are limited to
separate sub-500-line entry, phase, recovery, real-Git, and fault owners. P4 is
limited to archive location, no-follow, retention, targeted-GC, and fault
owners. The exact test/tool/doc manifest is:

- P0 (21): this interface, both first interface reviews, both interface `-2`
  re-reviews, both first P0 code reviews, both P0 code `-2` re-reviews,
  `v1_lifecycle/tests/{mod,authority,dispatcher_attempt_matrix,dispatcher_reconciliation,predecessor_matrix,reducer}.rs`,
  `v1_lifecycle/tests/{reverse_entry,reverse_router,archive_result,finalization}.rs`,
  `record_wire/archive/tests.rs`, and
  `record_wire/archive/tests/fixtures.rs` (shared with P4);
- P1 (14): `v1_lifecycle/tests/reverse_preservation/{mod,entry,phases,recovery,real_git,faults}.rs`,
  `git/tests/g15.rs`, `git/tests/g15/root_preservation.rs`,
  `git/tests/g15/root_preservation/{support,observation,mutation,stash,faults}.rs`,
  and `workspace_ops/tests/g23/characterization_preservation_v0.rs`;
- P2 (8): `v1_lifecycle/tests/reverse_rollback/{mod,entry,phases,recovery,real_git,faults,root_artifacts}.rs`
  and `merge/abort/tests/recovery.rs`;
- P3 (14): `protocol/{corpus/golden.json,corpus/rust/vectors.rs,fixtures/cli_parity/merge_response.json}`,
  `tests/protocol.rs`, `v1_lifecycle/tests/status.rs`,
  `merge/status/tests/{operation,pending}.rs`,
  `gwz-cli/src/tests/{g02,g02/shared_rendering}.rs`,
  `gwz-py/src/tests/{test_client,test_protocol,test_merge_cli_cross_driver,test_native_merge_operations}.py`,
  and `gwz-cli/docs/MachineOutput.md`;
- P4 (8): `v1_lifecycle/tests/{archive,gc}.rs`,
  `record_wire/archive/tests/{v0,v1,cleanup,fixtures}.rs`,
  `workspace_ops/tests/g23/characterization_archive_v0.rs`, and
  `merge/store/tests.rs`; and
- aggregate (12): `v1_lifecycle/tests/{reverse_integration,service_sequence,service}.rs`,
  `workspace_ops/tests/g23/{m4_matrix,compatibility_v0}.rs`, the three amended
  parent documents, two final settled-tree review memos,
  `scripts/checks/merge_docs_manifest.json`, and
  `scripts/checks/check_merge_docs.py`.

These are stop ceilings, not targets. Shared paths are charged to every slice
that changes them. Extracted lines are reported as movement rather than
semantic deletion. If exact I2 implementation cannot fit a slice, the owner
reports the concrete omitted contract and measured scope before changing code;
the lead and independent reviewers decide whether to revise the ledger.

Both independent interface re-reviews and both remediated P0 code re-reviews
report GO with no open P0-P3 finding. The first P0 code reviews remain as
evidence. P0 closes at 29 production paths and net 1,180/1,200 production LOC,
plus 21 test/tool/doc paths and 4,121/4,200 added lines. P1-P4 were initially
unblocked by that checkpoint; their first consumption exposed the bounded
P0.1 omissions below. Both amended-interface re-reviews now report GO, so the
shared correction and affected lanes are unblocked within the revised limits.

## 16. P0.1 mid-lane correction

`GwzM5-8R4bInterfaceAmendment-1.md` is controlling for the exact durable
publication handoff, authority-owned observation wrappers, normal-build
canonical archived-v0 acquisition, borrowed status view, strict direct
rollback matrix, and direct-ref GC facts. It freezes an exact 60-path
production and 46-path test/tool/doc shared manifest. P3 additionally gains
four exact constructor/parity evidence paths without a test-line increase.

The reconciled stop ceilings are:

| Slice | Net production | Moved production | Test/tool/doc | Production path charges | Test/tool/doc path charges |
| --- | ---: | ---: | ---: | ---: | ---: |
| P0 closed prerequisite | 1,200 | 0 | 4,200 | 29 | 21 |
| P0.1 shared correction | 1,400 | 0 | 2,200 | 60 | 46 |
| P1 preservation/recovery | 1,350 | 1,050 | 1,800 | 9 | 8 |
| P2 rollback/recovery | 2,200 | 650 | 1,800 | 9 | 8 |
| P3 status/protocol | 1,250 | 250 | 2,500 | 18 | 17 |
| P4 archive/GC | 750 | 100 | 1,500 | 8 | 8 |
| aggregate wiring, fault matrix, and checkpoint docs | 100 | 0 | 1,500 | 3 | 12 |
| **Charged ceiling** | **8,250** | **2,050** | **15,500** | **136** | **120** |

The conservative unique ceilings are 93 production and 92 test/tool/doc
paths. The amendment contains the exact path manifests, measured pause-point
charges, and attribution notes. These figures supersede §15 only for R4b-P;
all earlier accepted package budgets remain unchanged. No implementation may
use the correction to enable a production v1 decoder, writer, migration, or
dispatcher before A1.

## 17. P0.2 exact root-preservation correction

P1 implementation proved that the compound root `NormalizeRoot` and
`RestoreRoot` phases could not honestly journal independent marker, lock, and
raw-index durability boundaries. `GwzM5-8R4bInterfaceAmendment-2.md` is
controlling for the flattened phase graphs, exact physical `C0`/`C1`/`H`
forms, invariant boundary classification, normalized-preimage guard, durable
marker-parent preparation/restoration, checked stash/reset execution, path
manifests, and evidence matrix. A required structurally empty final parent is causally pending
on every platform: Unix synchronizes the retained parent, while Windows uses
the reviewed pinned write-through round-trip rename before the phase edge.

The reconciled stop ceilings supersede §§15-16 for R4b-P:

| Slice | Net production | Moved production | Test/tool/doc | Production path charges | Test/tool/doc path charges |
| --- | ---: | ---: | ---: | ---: | ---: |
| P0 closed prerequisite | 1,200 | 0 | 4,200 | 29 | 21 |
| P0.1 shared correction | 1,400 | 0 | 2,200 | 60 | 46 |
| P0.2 root-preservation correction | 3,100 | 450 | 11,100 | 23 | 27 |
| P1 preservation/recovery | 4,000 | 1,500 | 6,600 | 29 | 20 |
| P2 rollback/recovery | 2,200 | 650 | 1,800 | 9 | 8 |
| P3 status/protocol | 1,250 | 250 | 2,500 | 18 | 18 |
| P4 archive/GC | 750 | 100 | 1,500 | 8 | 8 |
| aggregate wiring, fault matrix, and checkpoint docs | 100 | 0 | 1,500 | 3 | 12 |
| **Charged ceiling** | **14,000** | **2,950** | **31,400** | **179** | **160** |

The conservative unique ceilings are 109 production and 107 test/tool/doc
paths. The program-wide unique production-path ceiling is 150. The five-path
increase is solely the cohesion split of
`authority/observe/reverse/preservation.rs` into the `cursor`, `entry`,
`phase`, `phase/evidence`, and `phase/steps` owners listed in §12. The parent
router remains, and every resulting owner is below 500 lines. The six
`reverse_preservation` evidence owners were already reserved in the P1
manifest, so the evidence path and all line ceilings are unchanged. This
lead-reviewed recount supersedes amendment 2's provisional 24-path P1 figure
without changing its wire, phase, action, or authority contracts.

P0.2's 2,200/450/3,800 shared Git and 450/0/1,000 lifecycle portions
are charged again to P1;
P0.2 reserves 5,200 document lines for the accepted interface set plus bounded
code-review/remediation appendices in the same memos. The aggregate program
counts shared physical paths once. Both interface reviews are GO through §13.
The first settled-code architecture review found index-only provenance,
manifest, evidence, and stale-control defects. Their bounded remediation is
accepted by both corrected-code re-reviews. P0.2 is accepted locally. Both
amendment 2 §14 settled-implementation reviews and the exact-SHA run are GO;
P1-WR1 is closed. P1 is implemented against the frozen seam, its local test
and static-analysis gates are green, and settled-tree review remains required
before acceptance.
P2-P4 retain
completed work but cannot authorize aggregate R4b-G integration while this
gate is open.
