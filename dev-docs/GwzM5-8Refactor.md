# GWZ M5–M8 Merge-Lifecycle Refactor Proposal

Date: 2026-07-30

Status: **Review 8 and independent F5-2 incorporated; R0, R1, and R2a
approved; M5a custom-message slice approved; `--no-ff` deferred to v1/A1;
I1 approved; broader durable state gated by I2, v1 activation by A1, and later
wave writers by A2–A4**

Review basis: `dev-docs/GwzM5-8Refactor-Review.md`,
`dev-docs/GwzM5-8Refactor-Review-2.md`,
`dev-docs/GwzM5-8Refactor-Review-3.md`,
`dev-docs/GwzM5-8Refactor-Review-4.md`,
`dev-docs/GwzM5-8Refactor-Review-5.md`,
`dev-docs/GwzM5-8Refactor-Review-6.md`,
`dev-docs/GwzM5-8Refactor-Review-7.md`,
`dev-docs/GwzM5-8Refactor-Review-8.md`,
`dev-docs/GwzM5-8Refactor-ReviewF5.md`, and
`dev-docs/GwzM5-8Refactor-ReviewF5-2.md`. This revision accepts all findings
from the eight numbered reviews and both independent F5 reviews. Review 8
confirms the prior immediate approval but requires the executable merge plan
to adopt the safe M5a/M5b/A1 release sequence. F5-2 confirms the earlier
corrections and adds v3-checkpoint, migration-eligibility, and protocol-code
precision.

## 1. Recommendation

GWZ should refactor the merge lifecycle now, but it should not rewrite the
working M4 implementation.

M4 has established sound machinery that should remain the foundation:

- complete selection-wide preflight before mutation;
- exact per-participant commit resolution;
- durable intent written before a Git mutation;
- observation and reconciliation after an interrupted mutation;
- explicit operation and participant transitions;
- checked rollback and preservation;
- root-last finalization;
- typed protocol responses and driver parity; and
- extensive crash, restart, drift, and real-Git tests.

The required change is narrower: evolve the durable lifecycle from “one merge
action on the participant’s current branch” to “typed, restart-safe forward and
reverse actions that produce a deterministic accepted workspace.”

The work must be sequenced around two explicit checkpoints:

1. R0–R1 characterize M4 and centralize current semantics without changing the
   record.
2. A minimal R2a wraps current merge intent/action and freezes message bytes,
   allowing the custom-message M5 slice to ship without speculative M6/M8
   fields. It does not make `mode: no_ff` newly writable in v0.
3. A narrow I1 direction memo then protects the future M6 checkout-evidence
   and M8 lock-domain boundaries without waiting for their full product
   policies.
4. I2 may freeze the v1 integration/acceptance record after that memo; full
   I6, I7, and I8 checkpoints gate v2, v3, and v4 respectively.
5. Extract M4 acceptance semantics first (R4a), implement adapters with the
   writer disabled (R3), make the finalizer consume persisted acceptance (R4b),
   then enable the writer and migration together through the reviewed A1 gate.
6. A1 establishes v1 as the active writer floor and activates `--no-ff`, so
   every new operation from A1 onward receives at least the
   persisted-acceptance contract. I6/M6, I7/M7, and I8/M8 then raise a new
   operation to v2, v3, or v4 when its immutable request needs those semantics.
7. Adapt already archived v0 records through a separate evidence-only
   projection that never treats the current live workspace as historical
   evidence.

The M5 custom-message slice proves the v0-safe integration/message seam.
M5b/A1 proves v1 no-ff. Neither can prove branch ownership, snapshot
provenance, or partial-composition semantics that it does not use; those
dimensions require separate proof gates.

## 2. Scope

### 2.1 In scope

The refactor should:

1. separate immutable participant intent from mutable execution progress;
2. inventory every start, continue, preserve, abort, and cleanup mutation before
   freezing a broader action schema;
3. extend the existing pending-action mechanism to typed forward and rollback
   actions after the I6 M6/v2 checkpoint;
4. record original and target checkout evidence for M6;
5. distinguish immutable participation authorization, runtime participation
   outcome, and integration progress for M8;
6. make a complete post-operation lock plus separate root-publication input an
   explicit, validated input to finalization;
7. retain source provenance separately from the exact per-participant commit
   used for integration;
8. centralize participant lifecycle policy currently repeated as
   `ParticipantState` matches;
9. split finalization by responsibility before selective composition enlarges
   it; and
10. define a version-dispatch and compatibility contract before any open M4
    record is rewritten;
11. version later lifecycle semantics per executable wave rather than exposing
    known-but-unsupported durable variants; and
12. project archived v0 history from archive evidence alone, with typed legacy
    unavailability where newer acceptance detail does not exist;
13. project every supported persisted archive version through its own
    archive-only decoder; and
14. prove every newly writable same-version field, value, enum variant, and
    combination against all materially different released-reader generations;
15. keep the executable merge plan and authoritative merge design synchronized
    with the M5a/M5b/A1 release boundary; and
16. freeze v2–v4 archive projection bodies only at I6, I7, and I8.

### 2.2 Out of scope

This is not authorization to:

- rewrite merge execution or replace the Git backend;
- redesign the public Git backend as capability traits;
- change M4 behavior, output, error codes, ordering, or recovery guarantees;
- weaken fail-closed behavior;
- make skip or partial completion the default;
- introduce a generic workflow engine;
- unify unrelated GWZ operations behind a speculative abstraction;
- mix dead-code cleanup or unrelated source cleanup into the refactor;
- implement unresolved M6 branch-creation policy;
- implement unresolved M8 skippable-cause, exit-status, or ownership policy;
- freeze a v1 record before the I1 M6-checkout/M8-lock-domain direction memo;
- make `mode: no_ff` newly writable in a v0 record;
- predeclare M6–M8 executable wire variants in v1;
- compile or publish speculative v2–v4 archive projection discriminants in A1;
- reconstruct archived history from current live repositories or rewrite
  archives during status;
- infer branch ownership from a matching ref name and commit; or
- treat the workspace root as an ordinary `--into` checkout.

Any cleanup changes should land independently. This refactor needs a clean
behavioral baseline and reviewable commits.

## 3. Why the checkpoint is needed now

The M4 record is a good representation of M4:

- `MergePlan` freezes one source ref, mode, baseline, and ordered participant
  plans;
- each participant freezes `target_branch`, `before_commit`, `source_commit`,
  and `commit_message`;
- `PendingMergeAction` journals an exact merge mutation before it occurs; and
- `ParticipantState` records the resulting integration state.

The model also embeds four assumptions:

1. a participant is already on its target branch;
2. every selected target participates;
3. the source is adequately identified by a display ref plus an exact commit;
4. a participant has at most one kind of mutation: integration.

Those assumptions become architectural pressure in M6–M8:

| Wave | Required behavior | Pressure on M4 assumptions | Refactor urgency |
| --- | --- | --- | --- |
| M5 | Custom messages in v0; `--no-ff` in v1/A1 | Messages reuse existing recovery authority; no-ff needs a fail-closed envelope plus exact prepared commits | Low structurally, but version-sensitive |
| M6 | create/find target branch, switch, integrate, restore, roll back | Requires multiple journaled mutations and two checkout identities | High |
| M7 | `+<snapshot>` with frozen per-member commits | Requires durable source provenance independent of mutable snapshot data | Medium |
| M8 | opt-in partial/skip and selective lock composition | Breaks “selected means attempted and composed” throughout the lifecycle | Very high |

If each wave is added directly to the current control flow, the likely result
is a cross-product of conditions:

```text
participant state
× current/original/target branch
× source kind
× merge mode
× required/optional authorization
× pending/participated/skipped outcome
× normal/continue/abort/status/finalize path
```

That is the source of the future cut-and-paste risk. The present implementation
is not mainly low-quality duplicated code; it is a sound implementation whose
policy decisions are still encoded at several call sites. The refactor should
centralize those decisions before the state space expands.

## 4. Non-negotiable invariants

The following rules remain true throughout the refactor and all M5–M8 work.

### 4.1 Planning and selection

- Resolve the complete selection before mutation.
- Validate every required participant before mutation.
- Freeze every exact source commit before mutation.
- Preserve deterministic participant order.
- Keep the root an explicit target and execute/finalize it in its defined
  root-last position.
- Do not reinterpret a missing source, missing participant, or validation
  failure as a skip unless an explicit M8 policy says that exact cause is
  skippable.

### 4.2 Durability

- Persist exact intent before each Git mutation.
- Assume that an interrupted action may be not started, completed but
  unrecorded, or ambiguous.
- Reconcile live Git state against durable inputs; never infer success merely
  because the process restarted.
- Clear a pending action only after its observed result is durably recorded.
- Make continue and abort idempotent across every persistence boundary.
- Preserve enough evidence to reverse completed mutations in checked reverse
  order.
- Persist a rollback plan/cursor before the first rollback mutation so a
  restart cannot lose reverse ordering.
- Give every mutation family an explicit durable owner: participant
  branch/integration actions, preservation actions, publication progress, or
  cleanup progress.

### 4.3 Composition and publication

- Compute the complete post-operation lock explicitly, including deterministic
  treatment of unselected members.
- Persist the validated complete accepted workspace after participant/outcome
  validation and before deciding whether publication is required.
- Archive that accepted workspace for every successful operation, including
  all-up-to-date and future all-skipped no-publication results.
- Never regenerate its exact lock bytes after restart.
- Keep root integration input separate from the composition/evidence commit
  created by publication.
- Validate the complete accepted workspace before publishing any candidate.
- Persist publication progress before every publication mutation.
- Never silently omit a selected participant from a lock or root
  finalization.
- Retain the current root evidence, candidate verification, and atomic
  publication guarantees.

### 4.4 Interfaces

- Keep protocol evolution append-only.
- Keep Rust, Python, CLI, native, and deprecated-alias behavior in parity.
- Preserve typed errors and stable machine-readable participant reporting.
- Reject records or combinations whose semantics cannot be recovered safely.
- Dispatch durable records by a validated schema/version envelope before
  deserializing a version-specific body.
- Treat additive-v0 compatibility as old-reader behavioral equivalence, not
  merely successful decoding or unknown-field preservation.
- Require complete executable support for an open record's semantic version;
  recognizing its wire variants is insufficient.
- Project archived legacy records only from durable archive evidence, with no
  live reconstruction or read-time rewrite.

## 5. Target lifecycle model

The target model has seven layers:

```text
request and selection
        ↓
frozen operation, participation, and participant intent
        ↓
typed durable forward or rollback action
        ↓
live observation and reconciliation
        ↓
participation/integration outcome and accepted lock state
        ↓
separate root publication
        ↓
response projection
```

Each layer owns one question:

| Layer | Question |
| --- | --- |
| Selection | Which workspace targets did the user address? |
| Intent | What exact result and participation policy were authorized for each target? |
| Action | What is the next mutation, and what evidence proves its outcome? |
| Observation | What actually exists in Git after a run or restart? |
| Outcome/lock | What exact full lock state is accepted, including unselected members? |
| Root publication | What exact born/unborn root base is accepted, and what publication output was created? |
| Projection | How is durable state reported through the protocol and CLI? |

No command path should independently reconstruct these answers from loose
booleans.

The type sketches in §§6–9 describe the eventual cumulative v4 model. They are
not one v1 wire or A1 canonical enum. Each semantic-wave binary implements only
the subset its record versions can execute: v1 specializes participation to
required/participated M4/M5 facts and has no branch action, snapshot source, or
skip/partial variant; v2–v4 add those types with their implementations.

## 6. Immutable participant intent

The current plan and record repeat several intent fields directly. R2a should
first group only the existing M4/custom-message intent; M5b adds no-ff intent
only to v1. The M6 checkout and M8 participation fields shown below are design
inputs; they must not be frozen until their wave checkpoints approve the
policies that give them meaning.

The following is an illustrative eventual internal shape, not a frozen wire
contract:

```rust
struct ParticipantIntent {
    target_id: String,
    target_kind: MergeTargetKind,
    path: String,
    participation: ParticipationIntent,
    source: ParticipantSourceIntent,
    checkout: CheckoutIntent,
    merge: MergeIntent,
}

struct ParticipantSourceIntent {
    provenance: SourceProvenance,
    exact_commit: String,
}

enum SourceProvenance {
    Ref { requested: String },
    Snapshot {
        snapshot_id: String,
        artifact_schema: String,
        workspace_id: String,
        artifact_sha256: String,
    },
}

struct CheckoutIntent {
    original: CheckoutIdentity,
    target: TargetBranchIntent,
}

struct CheckoutIdentity {
    branch: Option<String>,
    head_commit: String,
}

struct TargetBranchIntent {
    name: String,
    before_commit: Option<String>,
    creation: BranchCreationIntent,
}

enum BranchCreationIntent {
    MustExist,
    CreateAt { start_commit: String },
}

struct MergeIntent {
    mode: MergeExecutionMode,
    commit_message: String,
}
```

Important semantics:

- `exact_commit` remains per participant. A shared source name never substitutes
  for the commit actually integrated.
- `SourceProvenance` explains where the exact commit came from. Recovery uses
  the exact commit, not a fresh lookup of the ref or snapshot.
- snapshot provenance identifies the exact artifact bytes accepted during
  preflight, not merely the mutable snapshot ID. Exact per-participant commits
  remain authoritative for execution.
- `original.branch == None` represents a detached checkout without inventing a
  branch name.
- `target.before_commit == None` is valid only when the reviewed M6 policy
  permits branch creation.
- branch creation intent is frozen during complete preflight; it is not decided
  opportunistically during execution.
- message and, from v1 onward, executable mode remain frozen before mutation,
  including recovery identity requirements.
- `ParticipationIntent` records the request authorization that makes a later
  skip legal; it is never overwritten with the runtime outcome.
- R2a needs only existing target/source/message fields and does not make
  `NoFf` writable in v0. M5b adds executable mode to v1. Neither serializes
  placeholder checkout, branch-ownership, or optional-participation values.

For M4-compatible operations, the adapter constructs:

- `SourceProvenance::Ref` from `source_ref`;
- the recorded `source_commit` as `exact_commit`;
- the same recorded target branch as original and target checkout;
- `BranchCreationIntent::MustExist`; and
- `ParticipationIntent::Required`.

The adapter derives participation outcome from the legal M4 lifecycle state:
unfinished records remain undecided, successfully handled participants become
participated, and no v0 participant is synthesized as skipped.

## 7. Typed durable actions

### 7.1 Extend, do not discard, the pending-action pattern

`PendingMergeAction` already implements the most important durability rule:
write exact action intent before mutating Git. The refactor should preserve this
pattern and widen its scope.

Before freezing an enum, R1 must inventory every Git/filesystem mutation in
start, continue, abort, preserve, publication, and GC. Each mutation must name:

- its durable journal/progress owner;
- exact preconditions;
- exact expected result;
- not-started, completed-exactly, and ambiguous observations;
- retry and rollback behavior; and
- the evidence that survives a restart.

The participant branch/integration journal should eventually contain forward
and reverse actions:

```rust
enum PendingParticipantAction {
    Integrate(PendingIntegrationAction),
    CreateBranch(PendingCreateBranchAction),
    SwitchCheckout(PendingSwitchCheckoutAction),
    RollbackIntegration(PendingIntegrationRollbackAction),
    RestoreCheckout(PendingRestoreCheckoutAction),
    DeleteOwnedBranch(PendingBranchDeleteAction),
}
```

`PendingIntegrationAction` should retain the exact information already present
in `PendingMergeAction`, including:

- verification, fast-forward, true-merge, and conflict-resolution kinds;
- target branch and before/source commits;
- exact commit message;
- expected outcome; and
- prepared tree and exact signatures where a deterministic commit may be
  needed.

The non-merge variants must be equally explicit.

`RollbackIntegration` covers checked conflict abort or checked ref movement
back from the exact integrated result to the recorded pre-merge commit. Its
payload must distinguish those mechanisms and their different live
observations. `DeleteOwnedBranch` is a separate checked ref mutation; neither
`RestoreCheckout` nor a forward `Integrate` action may stand in for it.

Preservation does not use `PendingParticipantAction`. It needs a separate,
typed `PendingPreservationAction`/progress owner because it spans participant
backup refs, stashes, and root-publication material. Its mutation inventory
must cover at least backup-ref creation, stash creation, checked ref restore,
publication-prefix restoration, and artifact cleanup. Existing
`PublicationProgress` remains the owner of candidate/evidence publication.
The record checkpoint must prove that these separate journals cover every
mutation; a broad “all Git mutations use `PendingParticipantAction`” claim is
not correct.

### 7.2 Branch creation action

A branch-creation action should record at least:

- repository/participant identity through the owning record;
- branch name;
- expected absence or reviewed alternative precondition;
- exact start commit;
- expected resulting ref target; and
- the chosen branch-ownership proof or conservative no-delete policy.

Reconciliation must distinguish:

- branch still absent: action did not start;
- branch and any operation marker have the exact jointly expected state:
  completed exactly;
- branch exists at a different commit or ownership is uncertain: ambiguous,
  enter recovery-required state.

The branch name and matching commit are not ownership proof. Another process
can create the same branch after intent is persisted, or delete and recreate it
at the same commit (ABA).

The initial safe policy is:

- if creation completed but its completion evidence was not durably recorded,
  automatic deletion is never authorized;
- a matching branch may be adopted for continuation only under an explicitly
  reviewed rule; otherwise recovery fails closed;
- M6 may leave an operation-created branch behind on abort; and
- automatic deletion remains unsupported until an ownership mechanism passes
  concurrent same-commit creation and delete/recreate-at-same-commit tests.

If M6 requires automatic deletion, its design must specify stronger evidence,
such as an operation-owned marker created with the branch in one checked,
atomic ref transaction. The design must still explain how it detects an ABA
replacement after that transaction. A boolean ownership claim is insufficient.

### 7.3 Checkout switch action

A checkout-switch action should record:

- exact expected current checkout identity;
- exact target branch;
- expected target ref commit;
- whether detached HEAD is expected;
- worktree/index cleanliness preconditions; and
- the expected checkout identity after the switch.

Reconciliation must classify exact source checkout, exact target checkout, and
all other states separately. “Some branch is checked out” is not sufficient
evidence.

### 7.4 Checkout restoration action

Restoration is a separate action rather than a boolean on abort. It should
record:

- the exact current state from which restoration is allowed;
- the original branch or detached commit;
- the expected post-restore identity; and
- any target branch that remains owned by the operation.

Restoration occurs in checked reverse mutation order. A failed restore must
remain visible as a pending or ambiguous action; it must not be reported as a
completed abort.

Branch deletion, when supported, occurs only after the original checkout is
restored and the target branch is no longer checked out. It uses its own
pending action and exact checked deletion preconditions.

### 7.5 One pending action, durable completed evidence

The record does not need a general event-sourcing framework. Keep one active
pending action per participant and store the durable result of completed
actions in purpose-specific progress/evidence:

```rust
struct ParticipantExecution {
    phase: ParticipantPhase,
    pending_action: Option<PendingParticipantAction>,
    branch_progress: BranchLifecycleProgress,
    integration: IntegrationProgress,
    rollback: Option<RollbackProgress>,
}

struct RollbackProgress {
    steps: Vec<RollbackStep>,
    next_step: usize,
}
```

Before the first reverse mutation, GWZ derives, validates, and persists the
complete typed rollback plan in reverse order. For a created target branch the
plan may be:

1. roll back integration or abort conflict state;
2. restore the original checkout; and
3. delete the operation-owned branch, only if the approved ownership proof
   permits it.

`next_step` advances only after exact reconciliation of the current pending
action. This makes a restart between rollback actions resume the same order.
The plan is immutable once rollback begins.

The mutation protocol is always:

1. derive the next legal action from durable intent and progress;
2. validate its preconditions;
3. persist it as the pending action;
4. perform exactly that action;
5. observe the repository;
6. reconcile the observation against the pending action;
7. persist completed evidence and the new phase; and
8. clear the pending action in the same durable record update.

Completed evidence should be monotonic. Do not rely on an unbounded generic
action log, string action names, or arbitrary maps to drive recovery.

### 7.6 Root target branches

The root worktree is not an ordinary participant checkout. It contains or
selects the manifest, lock, exclusion boundary, workspace identity, and
`.gwz/merge` recovery records. Switching it can change the metadata needed to
discover and recover the operation itself.

The first M6 release should reject `--into` when `@root` is selected. This is a
typed selection-wide preflight error and occurs before any member mutation.

Root `--into` may be enabled only after a separate storage/authority design
defines:

- a recovery-record location independent of the checked-out root tree;
- which manifest, lock, workspace identity, and member paths remain
  authoritative while switched;
- how nested invocations discover the open operation;
- which metadata is restored before abort and publication;
- how tracked/untracked `.gwz` path collisions are prevented; and
- restart behavior when the target branch removes, changes, or conflicts with
  GWZ metadata.

## 8. Separate participation authorization, outcome, and execution

M8 must not add `Skipped` to the existing flat `ParticipantState` and then
teach every match statement what it means. “Was selected,” “was a skip
authorized,” “what eventually happened,” “what integration mutation happened,”
and “what enters the final workspace” are separate dimensions.

Record immutable request policy separately from runtime outcome:

```rust
enum ParticipationIntent {
    Required,
    Optional(OptionalPolicy),
}

enum ParticipationOutcome {
    Undecided,
    Participated,
    Skipped(SkipEvidence),
}

struct SkipEvidence {
    reason: SkipReason,
    decided_at: SkipDecisionPoint,
    detail: Option<String>,
}
```

`OptionalPolicy` must freeze the exact authorization needed to validate a later
skip after restart. An optional participant that succeeds remains:

```text
intent = Optional(...)
outcome = Participated
```

It must not be rewritten as required, because that would erase why a skip was
legal. A required participant may never have a skipped outcome. `Undecided`
covers planned or still-recoverable participants; completed operations may not
retain it.

The exact `OptionalPolicy`, `SkipReason`, and legal decision points are M8
interface decisions and must be a closed typed set before the record
checkpoint. Free-form error codes must not silently become skippable.

A participant with a skipped outcome:

- stays in `selected_targets`, the record, and machine reporting;
- has durable reason, decision point, and the original authorization;
- has no integration mutation unless an explicitly reviewed transition permits
  a post-attempt skip;
- is excluded from continue/abort mutation scope when no owned mutation exists;
- is included in preservation or rollback scope if it does own a mutation;
- cannot be treated as success merely because it was skipped; and
- affects operation exit status according to one centralized M8 policy.

Execution progress remains independently durable. If M8 permits skipping after
an attempted mutation, the record retains the attempted integration outcome,
rollback/preservation ownership, and later participation outcome. Composition
validates all three dimensions rather than accepting contradictory labels such
as “skipped and merged” without evidence.

## 9. Derive a complete accepted workspace

Current M4 finalization starts from the complete baseline lock and updates
selected participant rows. A selected-target-to-commit map is therefore not a
sufficient composition model. The accepted result must deterministically
describe the full lock, including unselected members, and keep root publication
inputs separate from publication outputs.

After complete participant/outcome validation—and before deciding whether any
candidate or publication work is required—construct, validate, and persist the
complete accepted workspace. Once persisted it is immutable for the operation:

```rust
struct AcceptedWorkspace {
    baseline_lock_sha256: String,
    lock: AcceptedLock,
    member_audit: BTreeMap<String, MemberAcceptance>,
    root: RootPublicationInput,
}

struct AcceptedLock {
    exact_yaml: String,
    sha256: String,
}

struct MemberAcceptance {
    selection: MemberSelection,
    participation_intent: Option<ParticipationIntent>,
    participation_outcome: Option<ParticipationOutcome>,
    integration: Option<AcceptedIntegrationRef>,
    final_checkout: AcceptedCheckout,
    lock_membership: LockMembership,
}

struct AcceptedIntegrationRef {
    branch: String,
    before_commit: String,
    resulting_commit: String,
}

enum LockMembership {
    Present(ResolvedMemberArtifact),
    Absent,
}

struct RootPublicationInput {
    base: AcceptedRootBase,
    publication_branch: Option<String>,
    baseline_artifact_hashes: RootArtifactHashes,
}

enum AcceptedRootBase {
    BornAttached { commit: String, symbolic_branch: String },
    BornDetached { commit: String },
    UnbornAttached { symbolic_branch: String },
}
```

This is the cumulative target. The v1 wire/canonical acceptance type is a
closed M4/M5 specialization: every selected member is required, no member is
skipped, integration occurs on the same existing attached, born local branch
and finishes attached to that exact ref, and source provenance is direct-ref.
Selected-detached integration remains unsupported. It does not serialize
dormant M6–M8 enum variants. Later adapters lift v1 acceptance into their
richer installed model without rewriting the original record version.

In v1 every selected member is `Present` with its verified complete result row,
and every unselected baseline-present member remains `Present` with its
unchanged authoritative baseline row, whether materialized or not. An audit
member with no baseline lock row and no selected result is `Absent` without
invented checkout or lock evidence. The complete lock's member keys equal
exactly the `Present` audit keys; `Absent` is not a v1 skip or partial outcome.

`LockArtifact` is not stored a second time inside `AcceptedLock`. It is the
validated in-memory parse of `exact_yaml`. This removes one redundant durable
representation; the exact bytes and their digest are the lock evidence.

The names and exact record location remain subject to the record checkpoint.
The durability contract and semantic requirements do not:

- `exact_yaml` is the authoritative complete post-operation lock, not a partial
  selected map or a value regenerated after restart.
- Parsing `exact_yaml` yields the complete `LockArtifact`.
- Its `ResolvedMemberArtifact` rows carry path, source identity, source kind,
  commit, branch, detached state, upstream, dirty state, and materialization
  state.
- `sha256(exact_yaml)` must equal the recorded `sha256`.
- `member_audit` accounts for the union of baseline, active, selected, and
  intentionally absent members, including unselected members retained from the
  baseline.
- A selected member records the integration result/ref separately from its
  accepted final checkout. M6 may integrate a target branch and later restore
  a different original checkout.
- `lock_membership` says exactly whether the member is present and, if present,
  with which full row. It does not ask finalization to interpret `Skipped`.
- every `Present` audit row equals the complete parsed lock row byte-for-model,
  and every `Absent` audit member is absent from the lock;
- the audit and lock domains satisfy the approved M8 rules for baseline,
  active, selected, unselected, and intentionally absent members;
- each member entry is consistent with frozen participation authorization,
  runtime outcome, integration/rollback evidence, and verified live state.

The I8 M8/v4 checkpoint must decide which participation outcomes produce
`Present` baseline rows, changed rows, or `Absent`. The record must not freeze
`KeepBaseline` versus `Excluded` before that decision.

The root is not a `LockArtifact` member. `RootPublicationInput` records the
accepted checkout and metadata authority used to decide and, when possible,
begin publication.

`AcceptedRootBase` has no empty, sentinel, or synthetic object ID:

- `BornAttached.commit` equals the durable accepted root result when `@root`
  participated, or the frozen attached baseline root head when it did not;
  `symbolic_branch` is the exact attached local branch.
- `BornDetached.commit` preserves the already legal all-up-to-date,
  no-publication baseline case without inventing a branch. It is invalid when
  `@root` participated or when the accepted workspace requires publication.
- `UnbornAttached.symbolic_branch` is legal only when the frozen baseline and
  accepted live precondition both identify that exact attached unborn branch.
- an explicitly selected `@root` produces `BornAttached` after successful
  integration; current root integration requires a real commit and an existing
  attached local branch.
- `publication_branch` is `Some` with the exact accepted symbolic branch for
  attached inputs and `None` for `BornDetached`. Publication-required
  acceptance rejects unless that value is present.
- evidence creation uses `Some(commit)` for `BornAttached` and `None` for
  `UnbornAttached`, preserving the checked first-commit and rollback behavior
  already released in M4. `BornDetached` never creates evidence.

The composition/evidence commit created by publication remains an output in
`PublicationProgress`. This avoids treating the publication-created root
commit as its own input. Every root input field comes from pre-publication
durable evidence; none may be derived from a publication output.

Finalization consumes the persisted `AcceptedWorkspace`; it must not
independently match participant states, rediscover lock rows, re-read a
snapshot, or regenerate the accepted YAML with a later serializer. Status and
protocol responses project the same durable decisions.

On restart after acceptance, GWZ validates the persisted cross-field
invariants and the live preconditions needed for publication, then uses the
exact recorded YAML bytes. If the live repository no longer matches accepted
evidence, it reports typed drift and publishes nothing; it never derives a
different candidate. In particular, an accepted `Unborn` root that becomes
born before evidence publication is drift; GWZ does not adopt the new commit as
the publication parent.

The record write that freezes `AcceptedWorkspace` occurs before the
publication-required decision and has an immediate fault-injection point.
After that point, restart either consumes the exact accepted bytes or fails
closed.

“Publication not required” is a deterministic projection of the accepted
workspace, not a bypass around it. Every successful operation archives the
exact accepted workspace, including all-up-to-date member-only, explicit-root,
and mixed selections and future all-validly-skipped operations. A no-change
operation freezes acceptance, records that no candidate is required, then
performs the terminal transition.

This is the containment boundary for M8: policy persists one complete accepted
workspace, while lock/root publication only validates and applies it when
required.

## 10. Central participant semantics

Lifecycle policy is currently expressed through matches in several modules.
Before adding new states, centralize it in pure functions over intent and
durable progress.

The semantic API should answer:

```rust
fn next_action(record: &ParticipantRecord) -> ModelResult<Option<PlannedAction>>;
fn reconcile(action: &PendingParticipantAction, live: &Observation)
    -> ActionReconciliation;
fn continue_eligibility(record: &ParticipantRecord, live: &Observation)
    -> RetryEligibility;
fn rollback_plan(record: &ParticipantRecord, live: &Observation)
    -> ModelResult<Vec<PlannedAction>>;
fn member_acceptance(record: &ParticipantRecord, live: &Observation)
    -> ModelResult<MemberAcceptance>;
fn accepted_workspace(record: &OperationRecord, live: &WorkspaceObservation)
    -> ModelResult<AcceptedWorkspace>;
fn participant_projection(record: &ParticipantRecord, live: &Observation)
    -> ParticipantResponse;
```

These functions need not share one trait or one file. The important rule is
that start, status, continue, abort, and finalization call the same semantic
functions instead of carrying parallel match matrices.

Prefer exhaustive typed matches. Do not replace them with:

- `can_continue`, `needs_restore`, `should_compose`, and similar boolean fields
  that can become contradictory;
- stringly action/state names;
- a generic transition table loaded at runtime; or
- driver-specific lifecycle policy.

Drivers should lower requests and render typed results. Core owns lifecycle
semantics.

## 11. Operation state

The existing `OperationState` is adequate through M5. Do not expand it merely
to mirror every participant phase.

For M6–M8, review it against these operation-level facts:

- execution can contain branch preparation, integration, and restoration;
- an operation may be workspace-acceptance-ready even when some selected
  participants are explicitly skipped;
- rollback may need to undo integration, restore checkouts, and, when ownership
  is provable, delete an owned branch;
- publication begins only after complete accepted-workspace validation;
- no-publication completion is decided only after the same validation/freeze;
- recovery-required remains the fail-closed state for ambiguous live evidence.

If an additional operation state is necessary, it should identify a distinct
user-visible recovery contract, not an internal loop position. Fine-grained
work belongs in participant phases and publication progress.

## 12. Source provenance for M7

M7 should reuse the intent model rather than introduce snapshot branches
through execution code.

Snapshot resolution is a planning concern:

1. parse `+<snapshot>` as a typed source selector;
2. read the exact snapshot artifact bytes once during complete preflight;
3. parse and validate its schema, workspace identity, snapshot ID, and content;
4. hash the exact bytes read;
5. resolve one exact commit for every selected participant;
6. validate object availability in the relevant repository;
7. freeze snapshot ID, schema, workspace identity, artifact hash, and every
   exact commit in the operation record;
8. stop using mutable snapshot data for that operation; and
9. execute the normal integration lifecycle against the frozen commits.

After planning, integration should not care whether the exact commit came from
a ref or snapshot. The exact per-participant commits are authoritative
execution evidence. The recorded snapshot identity/hash is authoritative audit
evidence for which artifact supplied them. Recovery never re-reads the snapshot
to choose commits.

Status and audit output display the recorded snapshot ID and artifact hash
without implying that a current file under the same ID still has those bytes.
A read-only observation may separately report that the current artifact differs
or is missing, but it must not replace the frozen evidence or block otherwise
safe execution recovery.

Typed selection-wide errors are required for:

- the snapshot not existing;
- a selected participant missing from the snapshot;
- a recorded commit missing or unavailable in its repository;
- malformed snapshot identity or content; and
- root selection not covered by the reviewed snapshot policy.

None of those become skips unless M8 explicitly classifies the exact cause and
the user opted into partial behavior.

Tests must replace snapshot contents under the same ID after preflight. Continue
and restart must use the frozen commits, while status/audit output must retain
the originally accepted artifact hash and report any current-file observation
as separate, non-authoritative information.

## 13. Finalization decomposition

`merge/finalize.rs` is already a concentration point and will otherwise absorb
M8 policy. Split it by behavior, not by arbitrary line ranges:

```text
merge/finalization/
    mod.rs                 orchestration only
    composition.rs         build and validate AcceptedWorkspace
    candidate.rs           construct deterministic lock/root candidate
    evidence.rs            root evidence commit and rollback evidence
    publish.rs             atomic candidate publication
    verify.rs              post-publication verification
```

Rules for the split:

- perform R4a's behavior-preserving acceptance extraction before any record
  adapter or M8 logic;
- route both the existing v0 finalizer and later compatibility adapters through
  those shared pure semantics before changing the durable record;
- perform R4b's acceptance persistence/finalizer-consumption change only after
  the disabled-writer R3 implementation proves every v0 publication mapping;
- keep orchestration small enough to read as the publication state machine;
- move tests with the semantic module they exercise;
- do not broaden visibility pre-emptively;
- keep candidate generation pure where possible;
- use one accepted-workspace validator from start/resume finalization paths;
- feed candidate construction the persisted exact lock YAML rather than
  reserializing a parsed lock after restart;
- freeze/archive acceptance before the all-up-to-date/all-skipped
  no-publication short circuit; and
- retain the existing durable `PublicationProgress` checkpoints.

A practical target is no production module over 500 lines and orchestration
under approximately 300 lines. These are review limits, not reasons to split a
cohesive function mechanically.

`merge/mod.rs` is the larger cross-cutting concentration point (approximately
1,158 lines at the reviewed baseline) and receives an equally explicit
behavior-preserving ownership split:

| Current responsibility | Target owner |
| --- | --- |
| `handle_merge` / `handle_merge_with_events` public dispatch | thin `merge/mod.rs` plus `runtime/dispatch.rs` |
| `enforce_workspace_open_merge_gate` | `runtime/open_gate.rs` |
| workspace mutation guard | `runtime/mutation_guard.rs` |
| `MergeStore` seam | `store/mod.rs` |
| record writes and transition helpers such as `persist_operation_transition` / `persist_merge_record` | `store/persistence.rs` |

R1 moves these responsibilities by behavior with characterization coverage;
R2a does not add new policy to `merge/mod.rs` while the move is pending. The
target root module contains public wiring/re-exports and readable orchestration
only, under the same approximate 300-line orchestration trigger.

## 14. Suggested module boundaries

Do not perform another workspace-wide mechanical split. Add boundaries only
where the new lifecycle needs ownership.

```text
merge/
    mod.rs                 public wiring/re-exports only
    runtime/
        dispatch.rs
        open_gate.rs
        mutation_guard.rs
    store/
        mod.rs
        persistence.rs
    model/
        mod.rs
        intent.rs
        action.rs
        progress.rs
        record.rs
        compatibility.rs
        archive_projection.rs
    participant_semantics/
        mod.rs
        next_action.rs
        reconciliation.rs
        eligibility.rs
        rollback.rs
        composition.rs
    branch_lifecycle/
        plan.rs
        execute.rs
        observe.rs
    source/
        ref_source.rs
        snapshot_source.rs
    finalization/
        ...
```

This is a target ownership map, not a requirement to create every file
immediately. In particular:

- retain the existing start/status/continue/abort orchestration modules;
- extract model types when their first consumer migration is ready;
- keep backend Git primitives in the backend semantic modules;
- do not make model modules aware of CLI or Python types;
- do not allow branch lifecycle to generate protocol responses;
- make `model/compatibility.rs` translate wire evidence and delegate acceptance
  decisions to the shared finalization semantics rather than owning a second
  policy implementation;
- keep `model/archive_projection.rs` read-only and free of backend/live
  workspace dependencies;
- keep `MergeStore` and durable persistence mechanics outside lifecycle model
  and driver code;
- prevent `merge/mod.rs` from becoming the replacement policy owner while
  finalization is split; and
- keep new-version record publication and migration entry points inaccessible
  until the reviewed A1 activation gate.

## 15. Record compatibility and version decision

M4 records use `gwz.merge-operation/v0` and schema version `0`. The current
store first deserializes directly into `MergeOperationRecord`, then validates
the constants, and recursively carries unknown fields forward on a rewrite.
Open operations may exist when a newer binary first runs. A replacement must
preserve those guarantees deliberately.

No new writer is approved before I2/A1. This proposal does choose the
compatibility strategy that I2 must freeze: v0 remains the M4 record plus the
old-reader-safe custom-message slice of M5, and each later semantic wave
receives a monotonically cumulative record version. The first new version, v1,
adds executable `--no-ff` integration plus the accepted-workspace/finalization
contract installed by R4b. M6, M7, and M8 do not predeclare executable variants
inside v1.

R2a and the custom-message M5 slice avoid forcing v1. `--no-ff` does not. R3
remains blocked until the directional interface memo and record checkpoint
approve the exact v1 fields, but not the version-per-semantic-wave rule.

### 15.1 Same-version newly writable shape gate

Unknown-field preservation prevents data loss, but it does not make a new
same-version semantic safe. The gate applies to every newly writable shape:
field, field value, enum variant, nested action, or combination—even when the
type was predeclared in an older binary.

A same-version change is eligible only when previous-reader behavior is
proven:

1. The release gate names the selected baseline for each distributed reader
   surface and any older reader explicitly retained for a distinct downgrade
   boundary. Within that supported window, it labels every materially different
   decode behavior relevant to the newly writable shape.
2. For every new open-record shape, each named binary either rejects the record
   before any Git/record mutation or selects exactly the mutations, transitions,
   recovery result, and projection required by the newly writable semantics.
3. The compatibility suite exercises status, continue, abort, preserve, and GC
   whenever the record state permits that command.
4. Equivalent behavior includes mutation selection, legal transitions,
   rollback/preservation scope, accepted workspace, publication, exit status,
   emitted machine state, and record rewrite behavior.
5. No newly writable value or combination that changes any of those semantics
   may remain invisible or inert to an old reader.

Run these tests with actual released executables through the retained-binary
harness in §15.8, not only current code configured to decode an old version or
only the immediately previous release. Unknown fields must remain preserved,
but preservation alone does not satisfy behavioral equivalence.

The M5 split is explicit:

- custom messages remain v0-safe because the already-recorded
  `participant.commit_message` is the recovery authority used by pending
  action reconciliation and execution;
- `mode: no_ff` is not v0-safe even though `MergeExecutionMode::NoFf` is
  predeclared: the selected durable-v0 baseline decodes it but recovery can
  re-derive a fast-forward action. V0.9.2 and earlier predate the durable merge
  record entirely and may ignore an open v0 operation; they are a separate
  downgrade safety boundary, not a v0 decode generation; and
- therefore M5 ships custom messages only in v0, while `--no-ff` first becomes
  writable under v1/A1.

The dormant v0 `NoFf` variant is a compatibility liability, not grandfathered
proof. The gate includes a v0 record with `mode: no_ff`, one fast-forwardable
participant, and no pending action. It runs status, continue, and abort with the
actual retained durable-v0 baseline. V0.9.2 is tested separately by invoking
its available one-shot commands in a workspace containing an open v0 record.
Typed rejection or exact no-ff behavior is required; current released behavior
fails, which is why v0 activation is forbidden.

If any supported old reader accepts a new shape and can make a different
mutation or lifecycle decision, the writer must select a new schema/version
pair.

The same behavioral gate applies to all later same-version changes: test every
supported decode generation of that version or increment the version.

#### 15.1.1 Version per semantic wave

The durable version denotes the complete lifecycle semantics required to
operate on the record, not merely the set of wire fields its decoder recognizes.
Versions are cumulative:

| Version | Required executable semantics | First writer |
| --- | --- | --- |
| v0 | Released M4 integration plus M5 custom messages using existing commit-message recovery authority | M4/M5a custom-message slice |
| v1 | v0 semantics plus executable `--no-ff`, persisted `AcceptedWorkspace`, born/unborn acceptance, and publication/no-publication consumption | A1 after R4b |
| v2 | v1 plus M6 create/switch/integrate/restore/delete branch lifecycle and ownership-aware rollback | M6 activation |
| v3 | v2 plus M7 snapshot-source provenance, resolution, and reporting | M7 activation |
| v4 | v3 plus M8 required/optional participation, skipped outcomes, partial composition, and its recovery/reporting semantics | M8 activation |

Any future preservation, publication, cleanup, or recovery behavior that an
older reader cannot execute safely receives a new cumulative version before
the first state requiring it is persisted. Mere maintenance changes,
diagnostics, or a different `writer_version` do not bump the record version
when lifecycle semantics are unchanged.

New-operation selection has two independent inputs:

1. the installed release's **active writer floor**—v0 for M5a and earlier, and
   v1 for A1 and every later release; and
2. the immutable request's **highest required semantic version**—v0 for an
   ordinary direct-ref M4/custom-message merge, v1 for `--no-ff`, v2 for M6
   branch lifecycle, v3 for M7 snapshot source, and v4 for M8 optional/partial
   composition.

The creation rule is:

```text
new_operation_version =
    max(active_writer_floor, highest_requested_semantic_version)
```

One pure core function owns this calculation. It takes a closed
`RecordVersion` floor plus the semantic level derived from immutable typed
request intent; CLI/Python drivers do not select versions. If the installed
binary does not implement the requested semantic level, start rejects the
request before creating a record rather than clamping it to the floor.

| Installed writer | Ordinary/custom message | `--no-ff` | M6 branch lifecycle | M7 snapshot source | M8 partial composition |
| --- | ---: | ---: | ---: | ---: | ---: |
| M5a | v0 | unsupported | unsupported | unsupported | unsupported |
| A1 | v1 | v1 | unsupported | unsupported | unsupported |
| A2/M6 | v1 | v1 | v2 | unsupported | unsupported |
| A3/M7 | v1 | v1 | v2 | v3 | unsupported |
| A4/M8 | v1 | v1 | v2 | v3 | v4 |

Combined features select the maximum requested semantic version, then apply
the floor. The chosen version is persisted with immutable operation intent
before any feature-specific action or mutation and remains fixed for that
operation.

This creation rule does not rewrite existing records:

- supported v1–v4 records retain their original version and semantics when a
  newer binary continues, preserves, aborts, rolls back, finalizes, reports, or
  archives them;
- open v0 records use only the separately approved atomic v0-to-v1 A1
  migration before their next mutation;
- an open v0 record carrying `mode: no_ff` is rejected as
  `UnsupportedLegacyMode` before migration or mutation because the envelope
  cannot prove whether dormant-v0 or executable-v1 semantics created it;
- archived records are never upgraded on read; and
- a later feature cannot be added by upgrading an already-running
  lower-version operation.

Thus “lowest sufficient version” means the lowest requested version at or above
the active writer floor, never the legacy v0 minimum after A1. Raising the
active writer floor beyond v1 would itself require a reviewed compatibility
decision and release matrix.

Every binary declares the exact record versions for which it implements the
complete lifecycle. Header validation occurs before status-derived mutation
eligibility, continue, abort, preserve, rollback, finalization, mutating GC, or
migration. Recognizing an enum or field is not proof of version support. An
unsupported version produces a typed `UnsupportedRecordVersion` result before
record, ref, index, worktree, candidate, preservation, or publication mutation.
Read-only status may report the schema/version and minimum supporting semantic
wave, but may not adapt it to an older canonical record.

This proposal deliberately chooses versioning over a required-capability set.
It avoids an A1 binary deserializing dormant M6–M8 variants that it cannot
execute and makes downgrade behavior testable with actual released binaries.

### 15.2 Wire envelope and decoder dispatch

If a new version is chosen, read a small YAML envelope before decoding a
version-specific body:

```rust
struct MergeRecordHeader {
    schema: String,
    record_schema_version: u32,
}

enum MergeRecordWire {
    V0(MergeOperationRecordV0),
    V1(MergeOperationRecordV1),
    // V2, V3, and V4 are added only by their semantic-wave releases.
}
```

The decoder sequence is:

1. parse once into a duplicate-aware raw mapping;
2. reject missing, duplicate, non-scalar, wrong-type, negative, or out-of-range
   `schema`/`record_schema_version` values as typed record-unreadable;
3. classify the pair through the ordered registry below;
4. only for an exact pair supported by this binary, deserialize the same raw
   value into that version’s wire type;
5. validate all version-specific invariants;
6. adapt to one canonical internal model; and
7. retain the raw/unknown-field representation needed for lossless rewrite.

Every binary contains a header-only allocation registry independent of its
compiled body decoders:

| Exact allocated pair | Required wave |
| --- | --- |
| `gwz.merge-operation/v0` / `0` | M4/M5 custom-message |
| `gwz.merge-operation/v1` / `1` | A1 integration/acceptance/no-ff |
| `gwz.merge-operation/v2` / `2` | M6 branch lifecycle |
| `gwz.merge-operation/v3` / `3` | M7 snapshot source |
| `gwz.merge-operation/v4` / `4` | M8 partial composition |

The classification is ordered and disjoint:

| Priority | Header class | Result |
| ---: | --- | --- |
| 1 | Malformed fields: missing, duplicate, non-scalar, wrong type, negative, or out of `u32` range | typed record-unreadable |
| 2 | Recognized schema string paired with any number other than its one assigned number | typed record-unreadable |
| 3 | Exact allocated pair whose body decoder/lifecycle is supported by this binary | decode that version-specific body |
| 4 | Exact allocated pair whose body/lifecycle is not supported by this binary | `UnsupportedRecordVersion` with raw pair and registry wave |
| 5 | Genuinely unknown schema string with any valid `u32` value | `UnsupportedRecordVersion` with raw pair and no claimed minimum wave |

A pair such as `gwz.merge-operation/v1` / `99` therefore matches only priority
2, never the future-version result. An unknown schema string paired with a
known or unknown valid number matches only priority 5. Future exact pairs
v2–v4 are recognized by A1 through the header-only registry without compiling
their bodies. Unallocated future pairs report their raw values without
inventing a supporting wave.

The redundant string/number pair is never interpreted independently and no
best-effort body decoding is allowed.

I2 owns the machine-error allocation for this boundary. The internal
`UnsupportedRecordVersion`, `UnsupportedLegacyMode`, and
`ArchivedRecordUnreadable` results map to append-only taut `GwzErrorCode`
values `unsupported_record_version`, `unsupported_legacy_mode`, and
`archived_record_unreadable`. I2 pins their numeric values and exact
human/JSON/JSONL projection; generated Rust and Python enums, corpus fixtures,
and driver parity must agree. No path may expose one of these as an
unregistered string code or reuse the broader `record_unreadable` code.

### 15.3 Version adapter

If v1 is approved, use versioned wire records and one canonical internal model
for the versions the installed binary fully implements:

```text
v0 wire record ──adapter──┐
                         ├── canonical lifecycle record
v1 wire record ──adapter──┘
```

Later releases add v2/v3/v4 wire adapters and canonical variants only with
their corresponding executable lifecycle package. The A1 binary's canonical
model intentionally has no branch-lifecycle, snapshot-source, or
partial-composition state.

The v0 adapter may synthesize only facts guaranteed by M4:

- direct-ref provenance;
- original checkout equal to the recorded target checkout;
- no branch creation;
- required participation intent;
- outcome derived only from legal M4 lifecycle state, never skipped;
- existing pending merge action mapped to typed integration action; and
- the exact complete M4 lock/root publication semantics, mapping a selected
  root result to `BornAttached`, an unselected born baseline to
  `BornAttached` or `BornDetached` according to its exact checkout, and an
  attached `baseline.root_head = None` plus exact `root_branch` to
  `UnbornAttached`.

It rejects a v0 record when those facts cannot be established safely.
In particular, unborn adaptation requires the frozen accepted symbolic branch;
it does not invent a branch or commit. Whether the live root must still be
unborn or may be the exact publication-created commit is determined by the
progress-aware mapping below.

#### 15.3.1 Shared acceptance/compatibility seam

The wire decoder does not implement finalization policy. It parses and validates
v0 evidence, then calls the same pure acceptance functions used by current
finalization:

```text
v0 evidence ──compatibility classification──┐
                                            ├── shared M4 acceptance semantics
live observation ───────────────────────────┘
```

Those functions are extracted behavior-preservingly in R4a before the adapter
or any new writer. They own complete-lock construction, accepted root-checkout
derivation, publication-required classification, candidate/prefix
reconciliation, and the next legal lifecycle action.

The adapter may select a version-specific wire representation, but it may not
duplicate those rules.

#### 15.3.2 Closed v0 publication-progress mapping

I2 freezes a closed mapping for every legal v0 operation/publication state.
The minimum mapping is:

| Row | Legal v0 state/evidence | `AcceptedWorkspace` and lock-byte source | Accepted root input | Exact allowed observation | Next action | Typed contradiction |
| --- | --- | --- | --- | --- | --- | --- |
| A | `Executing`, `AwaitingResolution`, or `Halted`; pre-acceptance `Preserving`/`RollingBack`; no candidate/evidence | Absent | Not yet derived | Existing v0 participant, preservation, and rollback observation | Resume the same v0 lifecycle; construct no acceptance yet | `UnexpectedAcceptanceEvidence` |
| B | `Finalizing`; publication absent or `NotStarted`/`ValidatingResults`/`PreparingCandidate`; candidate absent | Construct now through R4a from the recorded baseline and verified participant results | Derive `BornAttached` from the selected-root result/attached baseline, `BornDetached` from the exact detached born baseline, or `UnbornAttached` from the exact attached unborn baseline; detached is legal only if publication is not required | Live participants/root exactly match the pre-evidence accepted checkout | Persist acceptance, then derive publication-required; reject detached if publication is required | `AcceptanceInputDrift` |
| C | Candidate persisted; no evidence commit attempted | Recover from exact `candidate.lock_yaml`; verify its recorded digest; never reserialize | Same immutable R4a base as B, cross-checked with candidate publication branch and root-participation result | Candidate metadata valid; live root exactly matches accepted pre-evidence base; no candidate prefix published | Persist recovered acceptance, then create evidence | `CandidateIntegrityMismatch` |
| D | Candidate persisted; `composition_commit` absent; live root may contain an interrupted first/evidence commit | Recover from exact candidate bytes/hash | Same immutable accepted base as C; any evidence commit is publication output, not a replacement input | Adopt a born live root only when the exact scoped evidence commit verifies against the accepted parent (`None` for unborn), branch, message, files, tree, and candidate hashes | Persist recovered acceptance and exact evidence output | `AmbiguousEvidenceCommit` |
| E | `composition_commit`/`composition_tree` recorded; publication not started | Recover from exact candidate bytes/hash | Same immutable accepted base as C; recorded composition remains separate output | Recorded evidence commit verifies exactly and live root is that commit | Publish or reconcile the first candidate prefix | `RecordedEvidenceDrift` |
| F | `PublishingCandidate` with any legal observed partial candidate-publication prefix | Recover from exact candidate bytes/hash | Same immutable accepted base as C; recorded composition remains separate output | Root evidence is exact; the live marker/lock/boundary/staging state equals one prefix of the M4 write order using the recorded candidate bytes/hashes | Reconcile the observed prefix and resume publication | `PublicationPrefixMismatch` |
| G | Candidate fully published; `VerifyingPublication` or publication `Complete` before archive | Recover from exact candidate bytes/hash | Same immutable accepted base as C; recorded composition remains separate output | Root evidence and every candidate file/hash verify exactly | Verify/complete, then archive | `PublishedCandidateMismatch` |
| H | `Preserving` with candidate, evidence, root-preservation, or publication-prefix state | Recover from exact candidate when present; otherwise remain pre-acceptance as row A | Same accepted base as its source row, or absent for pre-acceptance A | Backup/stash evidence, root evidence, and preserved prefix match the recorded preservation state | Resume preservation or guarded abort | `PreservationEvidenceMismatch` |
| I | `RollingBack` or evidence-rollback state | Retain recovered acceptance when it had been frozen/recoverable; otherwise remain pre-acceptance | Same accepted base as its source row; rollback never rewrites it | Each reversed prefix is exact; recorded evidence rollback returns root to the original accepted checkout | Resume checked reverse rollback, then abort/archive | `RollbackEvidenceMismatch` |
| J | All-up-to-date no-publication `Complete`; candidate/evidence absent | Construct through R4a from the recorded baseline and complete verified unchanged audit | Derive the same exact attached-born, detached-born, or attached-unborn baseline input as B; no publication output exists | Root matches its accepted checkout; no candidate prefix exists | Persist acceptance, record publication-not-required, then archive | `UnexpectedPublicationEvidence` |
| K | Terminal `Completed` record still open before archive | Recover by C–G when candidate-bearing, or J when no-publication | Same immutable accepted base as the source row | Corresponding candidate/no-publication evidence is exact | Archive without Git mutation | `TerminalEvidenceMismatch` |
| L | `RecoveryRequired` with any publication state | Classify by the exact source row A–J; do not invent missing acceptance | Same accepted base as the source row, if one exists | Source-row evidence plus the durable drift/recovery reason are mutually consistent | Remain recovery-required and report the typed recovery action; make no automatic mutation | `RecoveryEvidenceMismatch` |
| M | Terminal `Aborted` record still open before archive | Retain recovered acceptance only if its source row had one | Same immutable accepted base as the source row | Publication/evidence rollback is complete and the live root is the original accepted checkout | Archive without Git mutation | `TerminalRollbackMismatch` |

The exact enum combinations for each row are frozen from the R0 legal-state
inventory. Unknown or contradictory combinations are not coerced into the
nearest row.

The authoritative accepted lock source is:

- existing `PublicationCandidate.lock_yaml` whenever a candidate exists;
- otherwise the exact bytes returned by R4a's M4 complete-lock/candidate
  construction from record-frozen inputs and verified results; and
- never a reserialization of an already persisted candidate by the new binary.

Candidate recovery verifies the recorded lock digest, publication branch,
root-participation result, marker/boundary bytes and hashes, canonical sorted
candidate hashes, evidence message/tree/files, and their cross-field
relationships before it can supply acceptance.

The accepted root base is derived once from the selected root result or frozen
baseline. Its allowed live observation then depends on publication progress:

- before evidence creation, live root must equal the exact `BornAttached`
  commit/branch, exact detached `BornDetached` commit, or exact attached
  `UnbornAttached.symbolic_branch`;
- a `BornDetached` input is legal only for deterministic no-publication and is
  never eligible for an evidence or candidate-publication step;
- an unrecorded born live HEAD for an accepted unborn base is adopted only when
  it verifies as the exact first scoped evidence commit with `parent = None`;
- after `composition_commit` is recorded, the born live root is valid only when
  that exact commit, tree, branch, message, candidate files, and hashes verify;
- during partial publication, filesystem/index state must match exactly one
  legal M4 write-order prefix justified by the durable step and recorded
  candidate bytes/hashes; and
- after evidence rollback, live root must again equal the original accepted
  attached-born or attached-unborn base.

For every row, the compatibility result freezes:

- acceptance absent/construct/recover classification;
- authoritative lock bytes/hash;
- accepted root base;
- exact allowed live root/ref and candidate prefix;
- publication outputs justifying any changed live state;
- next lifecycle action; and
- typed rejection for missing, contradictory, or ambiguous evidence.

Adaptation and the following restart must select the same next action as the
v0 implementation. Contradictory progress rejects before any record or Git
mutation.

#### 15.3.3 Archived record decoding and evidence-only projection

Already archived records under `.gwz/merge/done` do not use the open-record
adapter. Their separate read-only projection uses exact archive bytes alone and
performs no Git, ref, index, worktree, filesystem-artifact, manifest, live-lock,
member-repository, or snapshot observation to establish historical facts.

Terminal outcome and acceptance availability are orthogonal:

```rust
struct ArchivedMergeProjection {
    source_version: RecordVersion,
    terminal_outcome: ArchivedTerminalOutcome,
    acceptance: ArchivedAcceptanceProjection,
}

enum ArchivedTerminalOutcome {
    Completed,
    Aborted,
}

enum ArchivedAcceptanceProjection {
    SupportedPersisted {
        workspace: InstalledAcceptedWorkspaceProjection,
    },
    LegacyComplete {
        workspace: ArchivedAcceptedWorkspace,
        source: LegacyAcceptanceSource,
    },
    LegacyUnavailable {
        available: LegacyAcceptanceEvidence,
        missing: BTreeSet<LegacyAcceptanceGap>,
    },
    NotAccepted,
}

// Eventual cumulative A4 shape only; A1 compiles/publishes V1 alone.
enum InstalledAcceptedWorkspaceProjection {
    V1(AcceptedWorkspaceV1Projection),
    V2(AcceptedWorkspaceV2Projection),
    V3(AcceptedWorkspaceV3Projection),
    V4(AcceptedWorkspaceV4Projection),
}
```

This makes AV0-F and AV0-G representable: an aborted terminal outcome can
coexist with complete or incomplete historical acceptance. It also makes
“persisted” an evidence-quality statement rather than an assertion that the
archive version equals the installed writer's highest version.

The four-variant enum above is the eventual cumulative A4 shape, not an A1
compiled type or I2 protocol freeze. The installed enum is closed at each
activation:

- A1 contains and publishes only `V1`;
- I6/M6 freezes and appends `V2` with the v2 archive decoder;
- I7/M7 freezes and appends `V3` with the v3 archive decoder; and
- I8/M8 freezes and appends `V4` with the v4 archive decoder.

Each addition is append-only in the taut protocol: it receives a new pinned
discriminant and only version-appropriate fields, while all earlier
discriminants and field meanings remain unchanged. An unsupported record stops
at the header classifier and cannot instantiate or project a future variant.
The A1 canonical model and generated Rust/Python protocol must therefore
contain no V2–V4 projection body or discriminant, even though the header-only
registry recognizes their exact envelope pairs for fail-closed rejection.

Each installed binary has a version-specific archive decoder for every durable
version it supports. A2 therefore decodes v1/v2 archives, A3 decodes v1–v3,
and A4 decodes v1–v4. Each decoder validates that version's complete persisted
acceptance and maps it losslessly to its matching read-only projection variant.
It never invents later branch, snapshot, participation, or composition facts.
A field introduced by a later semantic wave is version-inapplicable for an
older persisted archive, not `LegacyUnavailable`.

`ArchivedAcceptedWorkspace` is an immutable v0 historical value containing the
exact accepted lock bytes/hash, complete legacy member audit, and accepted root
input derived from archive evidence. It is deliberately different from the
mutable lifecycle's `AcceptedWorkspace`.

`LegacyAcceptanceEvidence` carries only internally validated facts actually
present in a v0 archive, such as an exact lock digest or candidate bytes,
terminal participant results, born/unborn baseline fields, and recorded
composition evidence. The gap enum remains closed and protocol-projected:
`ExactLockBytes`, `CompleteMemberAudit`, `AcceptedRootInput`, and
`PublicationEvidence`.

The archive mapping is:

| Row | Durable archived evidence | Projection/result | Validation and reporting |
| --- | --- | --- | --- |
| AR-P | Terminal v1–v4 archive whose version is supported by the installed binary and whose persisted acceptance is complete | `SupportedPersisted` with exact `source_version`, sibling terminal outcome, and matching V1–V4 workspace projection | Decode and validate with that version's archive decoder; expose exact older semantics with later fields version-inapplicable |
| AV0-B | Completed v0 archive with exact candidate bytes and sufficient participant/root/audit evidence | `LegacyComplete { source: Candidate }` with `Completed` outcome | Adopt exact `candidate.lock_yaml`; validate its digest and all durable candidate/composition cross-fields without live observation |
| AV0-C | Completed all-up-to-date/no-publication v0 archive with exact baseline lock bytes and sufficient participant/root/audit evidence | `LegacyComplete { source: BaselineNoPublication }` with `Completed` outcome | Adopt exact `baseline.lock_yaml`; validate its digest and derive only unchanged audit/root facts proven by the archive |
| AV0-D | Completed v0 archive missing exact bytes, root input, or complete audit evidence required by the new projection | `LegacyUnavailable` with `Completed` outcome | Report every closed missing field plus durable evidence that remains; keep the otherwise valid archive readable |
| AV0-E | Aborted v0 archive with no candidate or other durable acceptance evidence | `NotAccepted` with `Aborted` outcome | Report that the operation ended before durable acceptance; do not synthesize a workspace |
| AV0-F | Aborted v0 archive with complete candidate-era acceptance | `LegacyComplete { source: Candidate }` with `Aborted` outcome | Report exact historical acceptance and later abort as separate typed facts |
| AV0-G | Aborted v0 archive with partial candidate/evidence history insufficient for complete acceptance | `LegacyUnavailable` with `Aborted` outcome | Report present evidence and closed gaps; never convert rollback success into fabricated acceptance |
| AR-C | Any supported v0–v4 archive with a nonterminal state or contradictory schema, identity, digest, acceptance, candidate, publication, or terminal evidence | typed `ArchivedRecordUnreadable` | Reject as structurally invalid based only on archive bytes; never downgrade contradiction to legacy unavailability |
| AR-U | Exact allocated future pair or genuinely unknown pair unsupported by this binary | `UnsupportedRecordVersion` | Report the raw pair through §15.2; status, retention, and targeted GC do not rewrite or delete it |

Born and unborn v0 root history is derived only from durable baseline,
selected-root result, candidate publication branch, and recorded composition
fields. A later live born HEAD never upgrades an archived unborn input, and a
missing repository never removes valid historical evidence.

The v0 adapter validates its schema/version, terminal state, identity,
participant terminal invariants, and whatever baseline/candidate/publication
evidence is present. Missing optional legacy evidence produces
`LegacyUnavailable`; an internal contradiction produces
`ArchivedRecordUnreadable`. Unknown top-level and nested fields remain in the
untouched archive bytes and cannot trigger a read rewrite.

Historical status, Rust CLI, Python/native, JSON, and JSONL project the same
source version, terminal outcome, acceptance variant, and version-appropriate
fields. Supported persisted archives always expose their complete
version-specific acceptance. Only v0 evidence gaps render as legacy
unavailable, with machine-stable gap values.

An explicit retention/GC command may delete a structurally valid terminal v0
legacy or supported v1–v4 archive after its archive-only decoder succeeds; it
does not need historical repositories or live workspace state. An unsupported
future archive cannot be deleted or rewritten by the older binary's status,
retention, or targeted GC path.

This projection is read-only. `LegacyUnavailable`, `NotAccepted`, and every
versioned persisted archive projection can never be converted to the mutable
open canonical record or used to authorize continue, abort, preserve,
rollback, finalization, migration, or any other lifecycle mutation.

### 15.4 Unknown-field preservation

Unknown top-level and nested v0 fields are an explicit compatibility contract.
Migration must recursively preserve their values and placement when a
corresponding container survives in v1. Before upgrade, construct an
unknown-field manifest containing the YAML path and value for every unknown
field.

The upgrade must either:

- map and verify every unknown field in the v1 raw value; or
- reject the upgrade before writing if a field has no lossless destination.

Silently dropping, coercing, or moving an unknown field is forbidden. Tests
must include unknown fields in the operation, baseline, participant, pending
action, prepared commit, preservation, publication, candidate, and drift
containers.

### 15.5 Current-version rewrite preservation

Unknown-field preservation continues after migration. Every ordinary rewrite
of a current-version record recursively carries unknown fields whose semantic
container survives the transition. This applies equally to:

- extensions migrated from v0;
- fields written by a future/newer writer but unknown to the current reader;
- top-level and nested fields introduced through additive compatibility.

I2 must freeze a container-retirement table. The default contract is:

- if the semantic container survives, every unknown field at and below that
  container retains the same YAML path and value;
- if a legal lifecycle transition removes the container itself, its nested
  unknown fields retire with it unless the table names a typed durable
  destination; and
- movement to a durable destination is explicit and verified—never inferred by
  field name or silently placed in a generic map.

Transient containers such as pending action, prepared commit, preservation
progress, publication candidate, and drift require an explicit survive/retire
rule for every legal transition before the version is frozen.

The store keeps the current raw/unknown manifest alongside the canonical model,
merges it into each replacement value, writes atomically, rereads the result,
and compares the expected surviving unknown manifest after every rewrite.
Migration is only the first such rewrite, not a special one-time preservation
exception.

### 15.6 Upgrade publication and verification

Migration eligibility is closed rather than inferred. Any structurally valid
open v0 record that maps to one legal §15.3.2 row A–M is eligible, including a
`RecoveryRequired` row or operation-level drift, provided it does not contain
`mode: no_ff` and every unknown field has a lossless v1 destination. The
atomic migration is a representation change, not an automatic lifecycle
action: it preserves the exact state, drift/recovery result, evidence, and next
action and performs no Git, ref, index, worktree, candidate, publication,
preservation, or archive mutation. An unreadable/contradictory record, a v0
no-ff record, or a record whose extensions cannot be preserved is ineligible
and returns its existing typed error without staging an upgrade. Read-only
status never migrates.

For an approved v1 migration:

- route archived v0 records through §15.3.3 for status/reporting;
- never pass an archived record to the open-record adapter or migration path;
- never rewrite an archived record merely because it was read or projected;
- validate the complete adapted open v0 record before staging an upgrade;
- preserve operation/merge identity, timestamps, lifecycle state, and all known
  and unknown evidence;
- serialize and fsync a unique staged file;
- parse the staged bytes through the envelope dispatcher;
- compare the canonical model and unknown-field manifest with the expected
  upgrade;
- compute the staged-byte hash in memory for the same-process I/O check;
- atomically replace the open record and fsync its directory;
- reread the published bytes, verify the same byte hash, decode them again, and
  compare the canonical model and unknown-field manifest; and
- perform no Git mutation until that verification succeeds.

R3 may implement this algorithm and exercise it through `cfg(test)` or an
equivalent non-production harness, but production dispatch must remain
hard-disabled. This is a call-graph boundary, not a runtime flag: the production
store continues to emit only its approved old version and has no callable
open-record migration path. No open record may be upgraded or written in the
new version until R4b has made the installed finalizer consume every
accepted-workspace, root, publication-prefix, preservation, rollback, and
no-publication invariant. A1 enables the new-version writer and v0 migration
together as one reviewed integration change; there is no intermediate release
in which a writer can create an open record that its installed finalizer cannot
safely resume.

The fully validated and fsynced staged file is the durable proof. The atomic
rename plus directory fsync publishes those exact bytes and is the durable
upgrade boundary. The staged-byte hash is ephemeral comparison state; it is not
stored inside the record and the record does not hash its own complete bytes.

The same-process post-rename reread is an additional I/O check, not a durable
migration phase. If the process crashes after rename and before that reread, a
restart treats the published file as an ordinary current-version record:
envelope-dispatch, decode, and validate it before any mutation. No external
upgrade transaction or resumable comparison journal is required.

Fault injection must prove that an interrupted upgrade leaves either the
complete valid v0 record or the complete verified new record. Stale temporary
files are never treated as open operations. The explicit post-rename fault must
resume from the new record without requiring the lost in-memory hash/model
comparison.

The A1 fixture set includes `RecoveryRequired` and operation-drift v0 records.
Migration must leave each in the identical recovery/drift classification with
the identical next action and no non-record mutation; status alone leaves the
original v0 bytes untouched.

### 15.7 Older binaries and downgrade behavior

An older binary encountering an open v1 record must fail closed before any Git
or record mutation. Already released M4 binaries cannot gain new wording, so
their actual schema-mismatch behavior must be captured as a compatibility
fixture. Before any new-version writer ships, the last v0-only reader release
should provide a clear error stating that the operation was written by a newer
GWZ record schema and must be resumed or aborted with a compatible/newer
binary. It must not suggest retrying with the old binary, rewriting the record,
or deleting it.

After the first new-version write, user-facing recovery output should make the
minimum compatible GWZ requirement clear. Downgrade is unsupported unless a
separately designed, lossless reverse adapter exists.

The same rule applies between semantic waves. The actual A1 binary must reject
v2, v3, and v4 before mutation; the actual v2/M6 binary must reject v3/v4; and
the actual v3/M7 binary must reject v4. Newer binaries must continue to decode,
recover, and preserve older supported versions under those versions' exact
semantics.

`writer_version` remains diagnostic metadata, not a compatibility gate. An
older maintenance binary that fully supports a record's semantic version must
continue to read it when a newer maintenance build rewrote it without changing
required lifecycle semantics. Conversely, recognizing fields or enum
discriminants from a later semantic version never authorizes operation.

An additive same-version writer receives no protection from schema matching.
Its old-reader behavior is governed by §15.1 and must be proven before release.
Every record-changing release runs the actual previous-reader matrix even when
the proposed schema/version pair is unchanged.

Required fixtures include:

- every legal open v0 participant, pending-action, preservation, and
  publication state;
- schema/version mismatches;
- unknown top-level and nested v0 fields;
- interrupted upgrade before staging, before rename, and after rename;
- exact published-byte/model verification;
- actual A1/v2/v3 binaries against every later semantic-wave state;
- typed fail-closed behavior for unsupported future versions before every
  mutable command;
- newer-reader recovery of every earlier version;
- same-version records rewritten by a newer `writer_version`;
- old-binary fail-closed behavior for new versions;
- supported v1–v4 and legacy-v0 archive projection through §15.3.3 without
  live observation or rewrite;
- the disjoint §15.2 envelope cross-product; and
- actual previous-release behavior for every proposed same-version shape.

Protocol response versioning is separate from durable-record versioning.
Response additions remain append-only even if the durable record changes.

### 15.8 Retained-binary compatibility harness

Actual-reader proofs are release gates, not ad hoc local experiments. R0
establishes the harness before the custom-message M5a release; I2 freezes its
long-term manifest and A1–A4 extend it.

Milestone documents specify the reader-selection rule, not a permanent release
tag. At each checkpoint, the retained durable-record baseline is the latest
successfully published pre-change release that supports that record. The
machine-readable manifest alone pins the exact tag, artifacts, and digests for
reproducibility. A superseded release with failed required builds does not
become a permanent compatibility requirement merely because an earlier plan
named it.

For every selected retained release, the supported downgrade contract includes
both distributed reader surfaces: the Rust `gwz` CLI and the released `gwz-py`
wheel/native package. The repository owns a machine-readable compatibility
manifest containing, for each surface/release/platform tuple:

- exact release tag and decode-generation label;
- reader surface and invocation entry point;
- immutable release artifact name and source URL;
- platform/architecture;
- for `gwz-py`, Python version/ABI, wheel tag, native-extension identity, and
  isolated runtime/bootstrap command;
- SHA-256 digest;
- supported record versions and expected envelope behavior; and
- commands/projections available in that release.

Use immutable tagged release artifacts, never `latest` or a locally rebuilt
approximation. CI verifies the digest, caches artifacts by digest, and fails
the gate if a required artifact or runtime is missing or cannot execute. If a
release distributed `gwz-py`, its retained wheel/native artifact is required;
it cannot be replaced by current Python code calling an old Rust executable.
If a historical release did not distribute a surface/platform combination,
the manifest carries an explicit unsupported entry, reason, and substitute
evidence rather than omitting the tuple.

The complete behavioral downgrade matrix runs on Linux x86_64 and Windows
x86_64 for every required Rust/Python reader tuple available on those lanes; a
host-native lane may supplement but not replace them. This is behavioral
sampling, not a redefinition of supported distribution targets. Release-build
and packaging evidence remains required for every supported target in the
release manifest, including macOS and Linux arm64. Those additional targets
run artifact-integrity, launch/import, generated-protocol, and focused smoke
checks; they need not duplicate every semantic fixture unless their decode or
filesystem behavior differs.

For this checkpoint the manifest pins v0.10.2 as the selected durable-v0
baseline; its `NoFf` recovery semantics remain dormant. V0.9.2 remains a
separate pre-record downgrade lane that must prove how an older command surface
behaves when an open v0 record exists; it must not be described as decoding
that record. M5a, A1, A2, and A3 Rust and distributed Python artifacts join the
manifest when published.

The harness:

1. creates an isolated temporary workspace/record fixture;
2. records hashes/bytes for the operation, archives, refs, index, worktree,
   candidate artifacts, preservation state, and relevant protocol output;
3. invokes the retained Rust executable or isolated Python/native entry point
   non-interactively with a bounded timeout and no network dependency after
   artifact acquisition;
4. captures exit code, stdout/stderr, JSON/JSONL where supported, and all
   post-command state;
5. compares typed result and mutation/no-mutation expectations; and
6. destroys only the isolated fixture.

Linux and Windows run the same fixture IDs and expectation manifest for each
retained surface. A surface- or platform-specific command absence is declared
in the manifest and covered by the nearest available public entry point; it is
not silently skipped. Current cross-driver tests supplement retained artifacts
but never prove old Python packaging/rendering compatibility by themselves.
Release retention keeps every binary, wheel, native extension, and runtime
descriptor needed by the supported downgrade window. Changing that window is
a reviewed compatibility/product decision with release notes, not a test
configuration edit.

## 16. Wave-by-wave use of the architecture

### 16.1 M5 custom messages and v1 no-ff

The immediate M5 slice proves only the old-reader-safe message seam:

- up-to-date participants remain explicit no-ops;
- custom messages live in immutable merge intent;
- mandatory GWZ recovery identity remains present;
- R2a alone freezes the exact final message bytes, including recovery-identity
  placement, separators, and normalization, before the M5 release;
- continue, abort, preservation, finalization, and rendering use the same
  centralized semantics.

`--no-ff` remains an integration action architecturally, but activation moves
to v1/A1. An otherwise fast-forwardable participant freezes a deterministic
two-parent commit specification before mutation; v1 persists enough typed
intent/action evidence that a v0 reader cannot silently reinterpret it. No
v0 writer may emit `mode: no_ff`.

Neither slice requires a branch action, participation outcome, or snapshot
provenance. M5 custom messages remain v0; A1 both raises the active floor to v1
and activates no-ff.

### 16.2 M6 — add branch lifecycle actions

After its separate product design is approved, M6 supplies:

- `CheckoutIntent`;
- typed create/switch/integrate/rollback/restore/delete actions;
- branch-specific observation;
- a persisted reverse-order rollback plan/cursor;
- the approved branch ownership or conservative no-delete policy;
- a typed rejection of root `--into` unless stable root recovery storage is
  separately approved; and
- fault injection after every create, switch, integration, rollback, restore,
  and delete mutation.

The normal merge integration action remains unchanged once the target checkout
is proven. An operation requesting M6 branch lifecycle is created as v2 before
its first branch intent is persisted; M6 does not add branch-action variants to
v1 or upgrade an already-running v1 operation. An A2/M6 direct-ref operation
without branch lifecycle is still created as v1 because the active floor
remains v1.

### 16.3 M7 — add a planning source

M7 supplies a new `SourceProvenance` variant and resolver. The provenance
freezes snapshot ID, schema, workspace identity, and the SHA-256 of the exact
artifact bytes read. It should not add a new execution engine or re-resolve the
snapshot during recovery.

Its proof gate is independent of M5/M6: restart must continue from frozen exact
commits after the source snapshot is changed or deleted, and every missing
participant/object/root case must remain typed and selection-wide.
An operation planned from a snapshot is created as v3 before its source intent
is persisted. An A3/M7 direct-ref operation is created at
`max(v1, other_requested_semantics)`, never v0.

### 16.4 M8 — add explicit participation and workspace policy

After its separate product design is approved, M8 supplies:

- closed skip reasons;
- legal decision points;
- immutable required/optional authorization;
- participated/skipped runtime outcomes;
- operation result/exit-status rules;
- complete post-operation lock behavior, including unselected members;
- separate root integration/publication behavior;
- continue, abort, preservation, and root scope; and
- complete protocol projection.

Finalization consumes the already validated accepted workspace and does not
contain skip-policy branches. An operation authorizing optional/partial
participation is created as v4 before that authorization is persisted. An
A4/M8 feature-free direct-ref operation remains v1; branch and snapshot
requests select v2 and v3 respectively.

### 16.5 Separate proof gates

The waves do not substitute for one another:

| Gate | Must prove |
| --- | --- |
| Integration | Exact frozen intent, prepared commit/message bytes, restart reconciliation, checked rollback, and v0/v1 compatibility |
| Branch lifecycle | Complete forward/reverse mutation inventory, branch ownership/ABA behavior, persisted rollback order, detached behavior, root policy, and v2 downgrade rejection |
| Source provenance | One frozen commit per participant, exact snapshot artifact identity/hash for audit, no recovery dependence on mutable snapshot data, and v3 downgrade rejection |
| Partial composition | Authorization/outcome separation, complete deterministic lock, born/unborn root input/output separation, no-publication acceptance, exit status, lifecycle scope, and v4 downgrade rejection |

I2 freezes the cumulative semantic-version allocation and the v1
integration/no-ff/acceptance wire contract after the integration proof and
I1's branch/partial direction memo. It does not wait for or freeze dormant
M6–M8 wire variants. I6, I7, and I8 freeze and activate only their own next
wire version after the full interface and
actual-older-binary proof gate passes.

## 17. Implementation work packages

Each package should be independently reviewable and leave the full test suite
green.

R0 also creates a change-budget ledger before R1 begins. For every package the
ledger records planned production LOC added/deleted/moved, production and test
files touched/created, and expected wire/protocol delta. Movement is reported
separately so it cannot hide semantic growth. The initial constraints are:

- R0 changes no production behavior and adds only characterization,
  compatibility-harness, and documentation material;
- R1 has zero intentional behavior, wire, or protocol delta and touches only
  responsibilities named in the §13–§14 ownership map;
- R2a has one integration/message seam, no serialized-record delta, and no
  checkout/participation/source/composition scope; and
- M5a contains only custom-message implementation, driver projection, and
  tests.

R0 freezes numeric LOC/file ceilings for those packages from its measured
baseline and later checkpoints do the same before their implementation
packages. Exceeding a ceiling by more than 20%, adding an unlisted production
module, or changing the declared wire/protocol delta stops the package for
scope review. The budget is a review guard, not an incentive to compress
cohesive code.

### R0 — baseline and characterization

- Record exact workspace and component revisions.
- Finish or revert unrelated cleanup before the baseline.
- Capture production LOC/module counts for affected merge modules.
- Enumerate and fixture every legal v0 operation, participant, pending-action,
  preservation, and publication state. Do not generate a blind cross-product
  of enum values.
- Classify every legal v0 publication state against §15.3.2, including
  pre-acceptance phases, pre-candidate finalization, candidate creation,
  interrupted and recorded first evidence, each publication prefix,
  verification/completion before archive, preservation, rollback, and
  no-publication completion on both born and unborn roots.
- Inventory every completed/aborted archived-v0 shape and the exact durable
  baseline, participant, candidate, root, publication, and unknown-field
  evidence it may or may not contain; freeze §15.3.3 fixtures without reading
  live workspace state.
- Add characterization tests for exact M4 records, responses, events, exit
  status, ordering, and mutation boundaries.
- Characterize born/unborn root evidence creation/rollback and the existing
  all-up-to-date no-publication completion path.
- Establish §15.8's retained-binary manifest/harness on Linux and Windows,
  including the selected durable-v0 baseline plus the v0.9.2 pre-record
  downgrade lane.
- Prove custom-message v0 recovery equivalence and capture the failing
  `mode: no_ff`/fast-forwardable/no-pending-action status/continue/abort
  fixture that forbids v0 no-ff activation.
- Confirm existing Rust, Python/native, protocol freshness, Clippy, formatting,
  Bazel, and cross-driver gates.

No architecture code moves in R0.

### R1 — pure semantic policy extraction

- Inventory `ParticipantState` decisions in start, status, continue, abort,
  preservation, finalization, and response code.
- Inventory every Git/filesystem mutation and assign its current and target
  durable progress owner.
- Extract common pure eligibility, rollback, and result-projection functions.
- Move `merge/mod.rs` dispatch, open-gate, mutation-guard, store seam, and
  persistence helpers to the §13–§14 owners without changing visibility,
  ordering, record bytes, or events.
- Replace duplicate matches call site by call site.
- Keep serialized types, behavior, and test output unchanged.
- Add exhaustive table tests for every current participant state.

This package proves policy can have one owner before new dimensions exist.

### R2a — v0-safe integration intent/message seam

- Group only the existing M4/custom-message intent needed for exact source,
  target branch, exact before commit, message, and current prepared commit.
- Wrap the existing merge pending action as the integration action variant.
- Preserve exact prepared-commit recovery.
- Freeze exact M5 commit-message bytes, including mandatory recovery identity,
  placement, separators, and normalization.
- Add compile-time exhaustive handling for existing integration variants.
- Keep M4 execution behavior unchanged.
- Keep the v0 serialized record unchanged and do not make `mode: no_ff`
  writable. Any newly writable v0 shape must pass §15.1 across every retained
  decode generation.

R2a does not add checkout intent, branch ownership, participation policy,
snapshot provenance, or speculative composition fields.

### M5a — custom-message implementation and release

- Implement custom messages through R2a.
- Run restart, continue, abort, preservation, root-finalization, exact
  message-byte, driver-parity, and retained-reader gates.
- Release the custom-message slice without waiting for v1 or speculative
  M6/M8 record fields.
- If M5a is the last v0-only reader, ship the forward-schema fail-closed
  diagnostic before any new-version writer.
- Keep the `--no-ff` CLI/protocol activation unavailable in this release.

M5a proves only the v0-safe message/integration gate.

### I1 — v1 directional interface memo

Status: **accepted; I2 unblocked** (2026-08-04). Two independent interface
reviews found no remaining P0–P3 issue after the root-checkout and
lock-membership clarifications.

- Freeze the M6 direction needed by v1: member audit separates integration ref
  from final checkout, but v1 permits only the existing checkout and contains
  no create/switch/restore/delete action or ownership claim.
- Freeze the M8 direction needed by v1: acceptance covers the complete baseline
  lock/audit domain, but every selected v1 participant is
  required/participated and no skip/partial variant exists.
- Keep root integration input separate from publication output and retain the
  attached-born/detached-born/attached-unborn checkout matrix.
- Confirm the v1 preservation/publication ownership seams can be extended by
  later versions without predeclaring their policies.

I1 is a short directional memo, not full M6/M8 product approval. Full M6
branch/ownership policy freezes at I6 before v2; full M8 skip/composition
policy freezes at I8 before v4. No durable schema is frozen during I1.

### I2 — v1 durable record checkpoint

- Freeze §15.8's actual-reader manifest/support window, Rust CLI and
  distributed `gwz-py` reader surfaces, behavioral lanes, full
  release-platform evidence, and explicit unsupported tuples. Keep v0 limited
  to custom-message shapes that pass §15.1 and record `mode: no_ff` as a known
  failing v0 shape.
- Freeze §15.1.1's cumulative allocation: v1 acceptance, v2 branch lifecycle,
  v3 snapshot source, and v4 partial composition. Freeze only the v1 wire body
  now; later bodies belong to their implementation waves.
- Freeze the active writer floor and creation matrix: M5a creates v0; A1 and
  every later release create at least v1; requested M6/M7/M8 semantics raise a
  new operation to v2/v3/v4; combined features use the maximum; existing
  records never use this creation calculation.
- Freeze only the v1 integration/no-ff action inventory, accepted lock, root
  publication input, current preservation ownership, and rollback evidence.
- Exclude M6 branch ownership/actions, M7 snapshot source, and M8
  optional/skipped outcomes from the v1 wire/canonical model.
- Freeze the attached-born/detached-born/attached-unborn accepted-root model,
  detached no-publication rule, and checked first-evidence commit/rollback
  invariants.
- Freeze `AcceptedWorkspace` as immutable persisted evidence before the
  publication-required decision, including its record location and all
  lock/audit/root cross-field invariants.
- Freeze the disjoint schema/version registry/classifier and v0/v1 body
  decoders.
- Allocate and pin the append-only taut `GwzErrorCode` values and exact
  machine projections for unsupported record version, unsupported legacy mode,
  and archived-record unreadability.
- Freeze the closed v0 publication-progress mapping, exact candidate-lock byte
  authority, stage-dependent accepted-root-checkout rules, typed contradiction
  results, and the requirement that adaptation preserve the v0 next action.
- Freeze the general per-supported-version archive rule plus only the concrete
  legacy-v0 and persisted-v1 projection bodies: sibling terminal
  outcome/acceptance, archive-only structural validation, cross-driver parity,
  no-read-rewrite, and live-independent GC. V2–V4 projection bodies and
  discriminants remain absent until I6/I7/I8.
- Freeze unknown-field migration and subsequent-rewrite preservation, the
  container-retirement table, atomic upgrade verification, and old-binary
  behavior.
- Freeze the closed migration-eligibility set from §15.6, including
  representation-only migration of `RecoveryRequired` and operation-drift
  rows with identical next-action classification.
- Freeze the writer/migration activation policy: R3 production writes remain
  disabled until R4b is installed and A1 passes as one reviewed gate.
- Approve the directional memos and independent future proof-gate structure for
  branch, source, and partial composition.

I2 authorizes v1 record work only. Its frozen contract receives explicit
re-review before R3. I6, I7, and I8 each receive their own mandatory re-review
before v2, v3, or v4 work; this deliberate decoupling prevents
persisted acceptance, no-publication archival, and v1 no-ff safety from waiting
on the hardest future product policies.

### R4a — behavior-preserving acceptance extraction

- Extract complete-lock construction and accepted-root-checkout derivation,
  publication-required classification, candidate-prefix reconciliation, and
  next-action selection as pure functions with exact M4 behavior.
- Characterize every legal v0 publication state through the closed §15.3.2
  mapping, including interrupted first evidence, preservation, rollback,
  no-publication completion, and terminal-before-archive states.
- Route the existing v0 finalizer through those functions without changing its
  record shape, mutation order, output bytes, events, or next action.
- Give compatibility code only validated wire evidence and observations; it
  calls these functions rather than copying their policy.
- Prove byte-equivalent lock/candidate/root results and exact next-action
  equivalence for the complete v0 fixture corpus.

R4a is a semantic extraction, not durable-state work. It introduces no new
writer, migration, or accepted-workspace persistence.

### R3 — record and adapter implementation, writer disabled

- Implement the approved v0/v1 wire strategy and envelope dispatcher; do not
  compile dormant v2–v4 lifecycle variants into the A1 canonical model.
- Implement the open-v0-to-v1 adapter, delegating acceptance and publication
  classification to R4a.
- Reject open v0 `mode: no_ff` as `UnsupportedLegacyMode` before adaptation,
  migration, or mutation.
- Implement the legacy-v0 and persisted-v1 archive decoders plus read-only
  `ArchivedMergeProjection` seam from §15.3.3 without live dependencies;
  later waves add only their own decoder.
- Implement the atomic upgrade algorithm behind `cfg(test)` or an equivalent
  non-production harness; do not add a production runtime flag.
- Preserve or reject unknown nested fields exactly as specified.
- Verify/project v1 and legacy-v0 archives without rewriting or upgrading them;
  reject v2–v4/unknown archives before targeted GC.
- Add mismatch, corruption, unknown-version, unknown-extension,
  partial-upgrade, old-binary, previous-release same-version, and restart
  tests.
- Add the complete ordered §15.2 envelope cross-product through every driver.
- Exercise every §15.3.2 v0 row and born/unborn variant, proving exact
  candidate-byte adoption, exact publication-prefix reconciliation, identical
  next action after adaptation/restart, and contradiction rejection before
  mutation.
- Exercise every §15.3.3 archive row through core, CLI, Python/native, JSON,
  JSONL, retention, and GC paths with byte-for-byte no-change assertions for
  reads.
- Drive staged migrated records through repeated legal continue, abort,
  preservation, publication, and archival rewrites, checking the surviving
  unknown-field manifest after every write.

R3 requires explicit lead approval because it implements durable-state
machinery. Its production new-version writer and open-v0 migration dispatch
remain hard-disabled: no package or release at this point may publish a new
open-record shape.

### R4b — acceptance persistence and finalizer consumption

- Add member audit entries separating integration ref, final checkout, and
  exact lock membership/row.
- Separate accepted root integration input from publication-created root
  output.
- Represent born and attached-unborn accepted roots without sentinels and
  retain M4 checked first-commit/rollback behavior.
- Validate and persist the complete immutable accepted workspace before
  deciding whether candidate publication is required.
- Derive the no-publication result from that frozen workspace and archive it
  for all-up-to-date and future all-skipped successful operations.
- Consume only its exact lock YAML bytes after restart; never regenerate them
  from live Git, snapshots, or a later serializer.
- Make continue, finalization, preservation, abort, evidence rollback, and
  archival consume the persisted acceptance and the same R4a semantics.
- Split candidate, evidence, publish, and verification responsibilities.
- Prove byte-equivalent lock/candidate/root output and exact restart actions
  for every M4 and adapted-v0 scenario.

No skip behavior is introduced in R4b; selected and unselected M4 handling must
remain exact.

### M5b — v1 no-ff implementation, activation disabled

- Add v1 `NoFf` integration intent without adding it to writable v0.
- For every otherwise fast-forwardable participant, freeze the deterministic
  two-parent commit message/tree/signatures and typed prepared action before
  its mutation can begin.
- Reconcile continue/abort/preservation from that exact v1 action rather than
  re-deriving merge mode from live analysis.
- Prove normal/divergent/up-to-date behavior remains exact and prove the
  fast-forwardable result is a deterministic two-parent commit.
- Keep CLI/protocol start activation and the v1 production writer unreachable
  until A1.

M5b may merge behind the same hard publication boundary as R3/R4b. It is not a
v0 release.

### A1 — new-writer and migration activation

- Verify that the installed finalizer understands every accepted-workspace,
  born/unborn root, candidate, evidence, publication-prefix, preservation,
  rollback, no-publication, and archival invariant emitted by the new record.
- Run the full §15.3.2 mapping and fault matrix through the production dispatch
  path, proving adaptation and restart choose the same next action as v0.
- Enable the new-version writer and validated open-v0 migration together in one
  small, explicitly reviewed integration change.
- Emit only v1 integration/acceptance records. The A1 binary contains no
  executable v2 branch, v3 snapshot, or v4 partial-composition state.
- Activate `--no-ff` only through v1 and verify no start path can serialize it
  under v0.
- Assert that every ordinary direct-ref operation created by A1 starts as v1
  and never v0; open-v0 migration is the only mutable v0 exception.
- Confirm the last v0-only binary fails closed on the new version and that no
  older package in the supported upgrade path can publish it.
- Retain the exact A1 release executable for the M6–M8 downgrade matrices, and
  prove it rejects representative v2/v3/v4 envelopes before decoding their
  bodies or deriving mutation eligibility.

A1 is the only v1 production activation point. If its review or tests fail, R3
and R4b may remain merged with the v1 writer and migration dispatch disabled.

### I6 — M6/v2 full interface and record checkpoint

- Approve the complete forward/reverse action inventory.
- Decide branch existence/creation, restoration, ownership, cleanup, detached
  HEAD, same-commit concurrency, and ABA behavior.
- Reject root `--into` or approve stable recovery storage and metadata
  authority.
- Freeze v2 wire fields, rollback plan/cursor, unknown-field retirement,
  archive projection, and actual A1 downgrade fixtures.
- Re-review the I1 direction memo and prove v2 maps v1 acceptance without
  changing v1 semantics.

No v2 implementation or writer activation proceeds before I6.

### M6 — branch lifecycle implementation

- Implement approved checkout/branch intent and forward/reverse actions.
- Implement the v2 wire/adapter behind a disabled production writer; choose v2
  before persisting an operation's first branch-lifecycle intent.
- Add the v2 persisted-archive decoder/projection without changing v1.
- Persist and reconcile the rollback plan/cursor.
- Apply the approved ownership/no-delete and root policy.
- Add deterministic fault injection at every mutation boundary.
- Keep integration execution isolated from checkout
  preparation/restoration/cleanup.

### A2 — M6/v2 activation

- Run the retained actual A1 binary against every v2 branch action and each
  lifecycle state/command that can contain it, requiring typed
  `UnsupportedRecordVersion` before mutation.
- Prove the v2 binary continues and recovers v0/v1 records and accepts
  same-v2 records written by a newer maintenance `writer_version`.
- Prove A2's version-specific archive decoders read/render/retain/GC valid v1
  and v2 archives without live observation or byte changes on reads.
- Assert A2 creates feature-free direct-ref operations as v1 and M6 branch
  operations as v2.
- Enable v2 creation only after the branch package and downgrade matrix pass
  explicit review.

### I7 — M7/v3 source, record, and archive checkpoint

- Approve the snapshot-source identity, schema, workspace identity, exact
  artifact-byte hash, per-participant commit, missing-participant/object, and
  explicit-root contracts.
- Freeze the v3 wire fields and adapter, the append-only `V3` archive
  projection body/discriminant, and the v3 unknown-field container-retirement
  entries without altering v1/v2.
- Freeze the actual A1/v2 retained Rust/Python reader fixtures for every v3
  planning, recovery, reporting, preservation, rollback, finalization, archive,
  and GC state M7 can persist.
- Re-review the version-independent execution boundary: exact persisted commits
  remain authoritative after the snapshot changes or disappears.
- Prove the v3 body and archive projection extend rather than reinterpret v1/v2
  acceptance and protocol fields.

No v3 implementation or writer activation proceeds before I7.

### M7 — snapshot source implementation

- Add typed source selection and snapshot resolver.
- Implement the v3 wire/adapter behind a disabled production writer; choose v3
  before persisting snapshot-source intent.
- Add the v3 persisted-archive decoder/projection without changing v1/v2.
- Freeze snapshot ID, schema, workspace identity, exact artifact-byte hash, and
  exact per-participant commits.
- Add changed/deleted-snapshot restart tests proving no later snapshot read.
- Replace a snapshot under the same ID and prove truthful recorded-hash
  status/audit output.
- Consume the I7-frozen source, wire, archive, unknown-field, and downgrade
  contracts without redefining them.

### A3 — M7/v3 activation

- Run the retained actual A1 and v2 binaries against every v3 snapshot-source
  planning/recovery/reporting state and applicable lifecycle command, requiring
  typed pre-mutation rejection.
- Prove the v3 binary recovers v0–v2 and accepts semantically unchanged v3
  records across maintenance `writer_version` changes.
- Prove A3 reads/renders/retains/GCs valid v1–v3 archives through their own
  decoders and refuses unsupported future archives without deletion.
- Assert A3 creates feature-free direct-ref operations as v1, M6 branch
  operations as v2, and snapshot-source operations as v3 even without M6.
- Enable v3 creation only after the source-provenance and downgrade matrices
  pass explicit review.

### I8 — M8/v4 full interface and record checkpoint

- Approve required/optional authorization, closed skip causes/decision points,
  post-attempt behavior, exit status, continue/abort/preservation scope, exact
  complete-lock treatment, and separate root semantics.
- Freeze the preservation journal's M8 scope/ownership.
- Freeze v4 wire fields, legal cross-field combinations, archive projection,
  unknown-field retirement, and actual A1/v2/v3 downgrade fixtures.
- Re-review the I1 lock-domain direction and prove v4 extends rather than
  reinterprets v1–v3 acceptance.

No v4 implementation or writer activation proceeds before I8.

### M8 — partial/skip implementation

- Implement the already approved participation intent/outcome policy.
- Implement the v4 wire/adapter behind a disabled production writer; choose v4
  before persisting optional/partial authorization.
- Add the v4 persisted-archive decoder/projection without changing v1–v3.
- Produce the already approved complete lock and root input.
- Add exhaustive cross-product tests for skip reason, decision point,
  participant outcome, root/member kind, and lifecycle command.
- Prove no unselected or silently omitted lock row and no skip without frozen
  authorization.

### A4 — M8/v4 activation

- Run the retained actual A1, v2, and v3 binaries against every v4
  authorization, skipped/participated outcome, partial acceptance,
  preservation, rollback, finalization, and reporting state, requiring typed
  pre-mutation rejection.
- Prove the v4 binary recovers v0–v3 and accepts semantically unchanged v4
  records across maintenance `writer_version` changes.
- Prove A4 reads/renders/retains/GCs valid v1–v4 archives through their own
  decoders and refuses unsupported future archives without deletion.
- Assert A4 creates feature-free, branch, snapshot, and partial-composition
  operations as v1, v2, v3, and v4 respectively.
- Enable v4 creation only after the partial-composition and downgrade matrices
  pass explicit review.

## 18. Test strategy

### 18.1 Characterization gates

Before changing types, enumerate and freeze legal states rather than a blind
enum cross-product:

- serialized M4 open and archived records;
- each legal operation and participant state;
- all four current pending merge action kinds;
- not-started, completed-exactly, expected-conflict, and ambiguous action
  observations;
- status, continue, and abort decisions;
- reverse rollback order;
- conflict preservation;
- preservation backup-ref/stash and root-publication progress;
- root-only, member-only, and mixed finalization;
- protocol response/event ordering; and
- CLI and Python rendering.

### 18.2 Pure model tests

Use exhaustive tables for:

- legal intent/progress transitions;
- next action by phase;
- reconciliation of every typed action against not-started, exact-completion,
  and ambiguous live states;
- continue and abort eligibility;
- persisted rollback action ordering/cursor;
- participation authorization-to-outcome legality;
- outcome/integration/final-checkout-to-lock-row mapping;
- every selected and unselected member receiving deterministic lock treatment;
- accepted-lock hash equals the exact YAML hash;
- parsing exact accepted YAML yields the expected complete `LockArtifact`;
- every present/absent audit entry exactly matches lock membership;
- born accepted-root commit selection for participating and non-participating
  roots;
- attached-unborn legality only when baseline/live branch evidence agrees;
- rejection of unborn accepted roots after successful explicit-root
  integration;
- publication-branch agreement with the accepted attached branch whenever
  publication is required, and explicit rejection of detached publication;
- publication-required versus no-publication derivation only from a frozen
  accepted workspace;
- separate root publication input/output legality; and
- rejection of contradictory records.

These tests should not require real repositories.

### 18.3 Real Git tests

Add real-repository tests for:

- exact prepared no-ff commits;
- attached and detached checkout switching;
- target branch present, absent, moved, and concurrently created;
- concurrent creation at the exact expected commit;
- delete/recreate-at-the-same-commit branch ABA;
- restart immediately before and after branch creation;
- restart immediately before and after checkout switch;
- restart immediately before and after integration;
- restart immediately before and after restoration;
- checked deletion of an operation-owned unchanged branch, if enabled by the
  approved ownership design;
- refusal to delete or move an ambiguous branch;
- typed root `--into` rejection before member mutation;
- future root-`--into` tests, if enabled, where the target root branch removes,
  changes, or conflicts with GWZ metadata;
- changed/deleted snapshot after preflight;
- replaced snapshot contents under the same ID after preflight, retaining the
  original recorded artifact hash and exact commits;
- changed member finalization from an attached unborn root;
- restart through first root evidence-commit creation and recording;
- abort before and after recording a first evidence commit on an unborn root;
- typed drift when a frozen unborn root becomes born before publication;
- all-up-to-date member-only selections on born and unborn roots;
- all-up-to-date explicit-root and root/member mixed selections on born roots;
- all-up-to-date multi-member selections without `@root` on an unborn root;
- every no-publication case archiving its accepted workspace without a
  candidate;
- skipped member/root composition according to the approved M8 policy; and
- final complete-lock/root verification from the accepted workspace.

When M8 lands, add all-validly-skipped and mixed skipped/up-to-date
no-publication cases with the same archived-acceptance requirement.

### 18.4 Fault injection

M6 needs a named fault point around every durable boundary:

```text
persist create intent
create branch
persist create result
persist switch intent
switch checkout
persist switch result
persist integration intent
integrate
persist integration result
persist rollback plan
persist rollback intent
roll back integration
persist rollback result/cursor
persist restore intent
restore checkout
persist restore result
persist delete intent
delete owned branch
persist delete result
```

The delete fault points apply only if the approved M6 ownership design enables
automatic deletion.

Every fault test restarts through status, continue, and abort as applicable.
The expected result must be exact completion, safe retry, or typed
recovery-required. “Best effort” is not acceptable.

The R3 non-production upgrade harness, and later A1's production path, have
separate fault points before staged write, after staged fsync, before atomic
rename, and after rename/before verification. The post-rename restart validates
the new record normally; it does not depend on lost in-memory staged hashes.

Finalization has fault points:

- immediately after the durable `AcceptedWorkspace` freeze and before the
  publication-required decision, on both born and unborn roots;
- after that freeze and before a no-publication terminal transition;
- after an unborn-root first evidence commit and before recording it; and
- after recording that first evidence commit and before later publication.

Restart must use the exact recorded lock/root evidence or report typed drift
without publishing. The all-up-to-date/no-publication path must resume to the
same terminal result without bypassing accepted-workspace validation.

### 18.5 Current-version rewrite compatibility

Start from one migrated open record containing unknown fields in:

- operation and baseline;
- participant;
- pending action and prepared commit;
- preservation;
- publication and candidate; and
- drift.

Clone that seed as needed to drive every legal branch through multiple
current-version writes:

- pending-action reconciliation and continue;
- abort and rollback;
- preservation creation/reconciliation;
- candidate preparation and each publication step;
- no-publication completion; and
- terminal archive/status.

After every write, compute and compare the unknown-field manifest. Fields below
surviving containers remain at the same path/value. Fields below a legitimately
retired container follow the frozen I2 retirement table exactly. Repeat with
fields written by a simulated future current-schema writer, not only fields
migrated from v0.

### 18.6 Open-v0 publication adaptation

Build named legal fixtures, not a generated enum cross-product, for:

1. `Executing`, `AwaitingResolution`, `Halted`, `Preserving`, and
   `RollingBack` before acceptance;
2. `Finalizing` before candidate creation;
3. a persisted candidate with no evidence;
4. an exact first evidence commit created but not recorded;
5. recorded composition commit/tree evidence;
6. every legal observed candidate-publication prefix;
7. verified/publication-complete records before archive;
8. evidence and publication state being preserved or rolled back;
9. all-up-to-date no-publication completion;
10. `RecoveryRequired` over every otherwise legal source row;
11. terminal `Completed` and `Aborted` records interrupted before archive; and
12. every legal born and attached-unborn variant of those states.

For each fixture, run the original v0 recovery classifier, the R4a shared
classifier, the version adapter, and restart from the adapted record. Assert:

- a persisted candidate contributes its exact `lock_yaml` bytes and hash with
  no serialization step;
- an accepted unborn root may have a born live HEAD only when it is the exact
  verified first evidence commit with `parent = None`;
- recorded evidence and each partial publication prefix match the exact
  recorded commit/tree/files/hashes;
- preserved and rolled-back evidence accepts only the exact stage allowed by
  its durable progress;
- the adapter and adapted restart select the same next action as v0; and
- every missing, contradictory, or ambiguous combination returns its specified
  typed error before record, filesystem, ref, index, or worktree mutation.

Run the same fixtures once through R3's non-production harness around the
proposed implementation and again through the production dispatch proposed for
A1. The second run is a release blocker, not permission for R3 to enable the
writer early.

Add the predeclared-mode liability fixture separately: an open v0 record with
`mode: no_ff`, a fast-forwardable participant, and no pending action. Status,
continue, and abort runs with the actual retained durable-v0 baseline document
its unsafe behavior; v0.9.2 separately proves the pre-record open-operation
downgrade behavior. The A1 adapter must return `UnsupportedLegacyMode` before
migration or mutation.

Run A1 migration separately against a `RecoveryRequired` overlay on every
applicable source row and against operation-level drift. Assert an atomic
v0-to-v1 representation change, identical typed recovery/drift and next-action
projection, and no Git/artifact mutation. The same fixtures through status
alone must remain byte-identical v0.

### 18.7 Archived record projection

Create byte-stable archives for:

1. completed candidate-bearing v0 records;
2. completed all-up-to-date/no-publication records with and without optional
   baseline lock bytes;
3. terminal archives whose optional legacy baseline or candidate bytes are
   absent where the v0 schema permits that absence;
4. aborted records before candidate preparation, after candidate preparation,
   after evidence creation, and after evidence rollback;
5. born and attached-unborn root inputs with and without recorded publication
   output;
6. complete and incomplete member audit evidence;
7. unknown top-level and nested baseline, participant, candidate, publication,
   preservation, and drift fields;
8. structurally contradictory terminal evidence;
9. a live root and members advanced through later operations;
10. missing member paths, deleted repositories, detached/recloned roots, and
   absent candidate files outside the archive;
11. v1, v2, v3, and v4 archives with complete persisted acceptance;
12. each supported older-version archive under A2, A3, and A4; and
13. exact allocated-future and genuinely unknown archive envelopes.

Run each valid fixture through the core archive loader, Rust CLI,
Python/native status, JSON, and JSONL. Assert identical
source version, sibling terminal outcome,
`SupportedPersisted`/`LegacyComplete`/`LegacyUnavailable`/`NotAccepted`
classification, version-appropriate fields, and closed gap values across
projections; exact candidate/baseline byte adoption; no live Git,
filesystem-artifact, manifest, lock, repository, or snapshot lookup; and
byte-for-byte unchanged archive contents after every read.

Missing optional legacy evidence produces `LegacyUnavailable`, not corruption.
Contradictory durable evidence in any supported version produces
`ArchivedRecordUnreadable`, not an unavailable field. An aborted
pre-acceptance v0 archive produces `NotAccepted`. No historical projection can
enter mutable lifecycle code.

Run explicit retention/GC over every structurally valid terminal fixture after
removing or advancing all live repositories. GC must identify/delete the
archive using its bytes alone. Read-only fixtures remain intact; GC deletion is
the only authorized mutation and does not first upgrade or rewrite the record.
An older binary returns `UnsupportedRecordVersion` for an unsupported future
archive and does not delete or rewrite it through status, retention, or
targeted GC.

Activation matrices additionally require:

- A1's compiled model and generated protocol contain only the concrete V1
  persisted projection while its header registry rejects exact v2–v4 pairs;
- A2 reads/renders/retains/GCs valid v1 and v2 archives;
- A3 does so for v1–v3; and
- A4 does so for v1–v4.

### 18.8 Semantic-wave downgrade compatibility

Retain actual Rust CLI and distributed Python/native readers for v0, A1/v1,
M6/v2, and M7/v3. At each later wave, run every older reader surface against
every new durable action, source, authorization/outcome, preservation,
rollback, finalization, publication, terminal-before-archive, and
recovery-required state that wave can persist.

Separately exercise the §15.1.1 new-operation matrix through real start paths:

- M5a creates an ordinary direct-ref operation as v0;
- M5a rejects `--no-ff` before record creation;
- A1, A2, A3, and A4 each create a feature-free direct-ref operation as v1,
  never v0;
- A1 and later create `--no-ff` operations as v1;
- A2 creates an M6 branch-lifecycle operation as v2 while retaining v1 for a
  direct-ref operation;
- A3 creates a snapshot-source operation as v3 even without an M6 branch
  request;
- A4 creates optional/partial composition as v4;
- combined requests select the maximum requested version above the v1 floor;
- newer binaries continue existing v1, v2, and v3 records without changing
  their version;
- open-v0 atomic migration and archived-v0 evidence projection remain the only
  exceptions to ordinary new-operation selection; and
- a fault immediately after record creation proves the chosen version and
  immutable intent are durable before the first feature-specific mutation.

For each applicable start/open-operation, status, continue, abort, preserve,
rollback, finalize, mutating-GC, and migration entry point, assert:

- a typed `UnsupportedRecordVersion` is selected from the envelope before body
  adaptation or mutation eligibility;
- no record, ref, index, worktree, candidate, preservation, publication, or
  archive byte changes;
- read-only status reports the unsupported semantic wave without projecting a
  weaker canonical lifecycle;
- the newer binary continues and recovers every earlier supported version;
- a record stays readable when only `writer_version` changes within the same
  semantic version; and
- a same-version additive proposal either passes the complete actual-reader
  behavioral matrix or increments the semantic version.

For `start`, the old binary rejects a later-wave request/protocol shape before
creating or rewriting any record. For commands targeting an existing
later-version record, envelope rejection occurs before body deserialization.

A2, A3, and A4 cannot activate their writers until their complete retained
binary matrix passes.

### 18.9 Envelope classification

Run a complete header cross-product through core, Rust CLI, Python/native,
JSON, and JSONL:

- every exact pair supported by the tested binary;
- every exact allocated-but-unsupported pair;
- each recognized schema string paired with every wrong known and future
  number;
- unknown schema strings paired with known and unknown valid numbers; and
- missing, duplicate, non-scalar, wrong-type, negative, and out-of-range
  version values.

Assert the ordered §15.2 classification, identical typed codes/guidance, and
raw-pair reporting. Every rejection occurs before body adaptation, archive
deletion, or any record/ref/index/worktree/candidate/preservation/publication
mutation. A recognized string with a wrong number is always unreadable; a
genuinely unknown valid pair is unsupported with no invented minimum wave.
Protocol fixtures also pin the append-only numeric `GwzErrorCode` allocation
and human/JSON/JSONL projection for `unsupported_record_version`,
`unsupported_legacy_mode`, and `archived_record_unreadable`.

### 18.10 Full release gates

Every package runs:

- Rust formatting;
- strict Clippy;
- focused and full Rust tests;
- Python/native tests;
- generated protocol/reference freshness;
- Rust/Python request and response parity;
- Bazel tests/builds;
- release builds and packaging evidence for every supported target, including
  macOS and Linux arm64;
- root-only, member-only, and mixed driver scenarios; and
- diff hygiene proving no unrelated cleanup entered the package.

Every record-changing release also runs §15.8's checksum-pinned Linux and
Windows retained Rust/Python readers across every newly writable
field/value/variant/combination and applicable status, continue, abort,
preserve, rollback, finalize, migration, and GC command. This applies when the
version changes and when a proposal claims same-version compatibility. A
missing required reader/runtime, behavioral lane, or full-platform
build/package artifact fails the release; it never becomes a skip.

R0 adds a document-consistency check over this proposal,
`gwz-core/dev-docs/GwzMergePlan.md`, `GwzMergeDesign.md`, and current public
capability/deferred-feature documentation. The gate fails if any source makes
`--no-ff` writable or releasable in v0, combines it into the M5a release, omits
the M5b/A1 activation boundary, or assigns a different I6/I7/I8 version
sequence. It runs at every M5a–A4 integration gate.

## 19. Review limits and code-quality guardrails

To avoid recreating the same issues:

- one module owns each lifecycle decision;
- no driver contains core lifecycle policy;
- no new action is represented by a string or a set of independent booleans;
- no participant branch/integration mutation occurs without a typed durable
  `PendingParticipantAction`;
- preservation, publication, and cleanup mutations occur only through their
  separately named durable progress/journal owner;
- no rollback begins without a persisted typed reverse-order plan/cursor;
- no branch deletion relies only on a name, boolean ownership claim, and
  matching commit;
- no additive same-version field or variant is accepted merely because unknown
  fields round-trip; actual supported old readers must pass the behavioral
  gate;
- no newly writable field, value, enum variant, or combination is treated as
  compatible without testing every materially different retained decode
  generation;
- no v0 writer emits `mode: no_ff`, and no v0 no-ff record is migrated;
- no required retained Rust/Python artifact, runtime, behavioral lane, or
  full-platform release artifact is silently skipped or replaced with a local
  rebuild;
- no A1 model or generated protocol contains a V2–V4 archive-projection body
  or discriminant;
- no new typed compatibility result lacks one pinned append-only
  `GwzErrorCode` and cross-driver machine projection;
- no publication-required decision occurs before the complete accepted
  workspace is durably frozen;
- no successful no-publication completion bypasses accepted-workspace freeze,
  validation, or archival;
- no restart regenerates accepted lock bytes from live Git, snapshot data, or a
  newer serializer;
- no adapter reserializes `PublicationCandidate.lock_yaml` or substitutes
  baseline bytes once a candidate exists;
- no compatibility adapter owns acceptance, root, publication-prefix, or
  next-action policy separately from R4a;
- no new-version writer or open-record migration dispatch is reachable before
  R4b is installed and A1 passes;
- no package can write an open record whose complete invariant set is not
  understood by the finalizer shipped in that same package;
- no binary accepts an open record merely because its decoder recognizes the
  wire fields or enum variants; it must implement that record version's
  complete durable semantic wave;
- no v1 wire/canonical type contains dormant M6 branch, M7 snapshot, or M8
  partial-composition variants;
- no new operation bypasses
  `max(active_writer_floor, highest_requested_semantic_version)`;
- no A1-or-later binary creates a new v0 operation, even when the request is an
  ordinary direct-ref merge;
- no feature-specific intent is persisted before the operation's selected
  semantic-wave version is frozen;
- no newer binary recomputes or raises an existing operation's version, except
  for the separately approved open-v0-to-v1 migration;
- no `writer_version` comparison substitutes for record-version compatibility;
- no envelope condition can match both record-unreadable and
  `UnsupportedRecordVersion`;
- no archived-v0 projection reads live Git, worktree, manifest, lock, snapshot,
  filesystem artifact, or member repository state to fill historical gaps;
- no archived record is rewritten/upgraded by status or historical projection;
- no supported older v1–v4 archive is routed through the legacy-v0 adapter or
  rejected merely because it is not the installed writer's highest version;
- no unsupported future archive is deleted by retention or targeted GC;
- no `LegacyUnavailable` or `NotAccepted` archive projection can enter mutable
  lifecycle code;
- no unborn root is represented by an empty, sentinel, or synthetic commit;
- no current-version rewrite drops unknown fields from a surviving semantic
  container;
- no new `expect` or `unwrap` is allowed on recovery-controlled data;
- impossible internal states use constructors and exhaustive types where
  practical, with typed errors at persistence and Git boundaries;
- no copied participant-state match is accepted when a semantic function can
  own it;
- no new dispatch, gate, guard, store, or persistence policy accumulates in
  `merge/mod.rs` outside the §13–§14 owner;
- no module exceeds 500 production lines without a written cohesion reason;
- orchestration functions should read as lifecycle steps, not contain Git
  mechanics;
- record compatibility code remains isolated from current lifecycle policy;
- generated files are never hand-edited; and
- movement-only refactors and behavior changes land separately.

The LOC limit is a review trigger, not a target. Several tiny files with shared
mutable assumptions are worse than one cohesive module.

## 20. Risks and mitigations

| Risk | Mitigation |
| --- | --- |
| Refactor changes proven M4 behavior | Characterization fixtures, pure extraction first, feature work later |
| Record migration makes an open M4 merge unrecoverable | Limit A1 to v1 acceptance semantics, envelope dispatch, exact extension preservation, atomic verified upgrade, legal-state fixtures |
| New record writer outruns finalizer semantics | Sequence R4a, disabled-writer R3, R4b consumption, then one reviewed A1 writer/migration activation |
| V0 partial publication is adapted to the wrong restart action | Closed progress mapping, exact candidate bytes/evidence/prefix checks, born/unborn fixtures, and v0-versus-adapted next-action equivalence |
| A released reader silently reinterprets v0 `mode: no_ff` | Ship only custom messages in v0; reject legacy v0 no-ff; activate deterministic no-ff under v1/A1 |
| Executable plan reintroduces v0 no-ff | Keep proposal, plan, design, and public capability text under the R0 document-consistency gate |
| Gate checks fields but misses new values/predeclared variants | Define newly writable shapes broadly and test the selected durable-record baseline's field-known and dormant-variant behavior; retain v0.9.2 as a separate pre-record downgrade lane |
| A1 recognizes but cannot execute later M6–M8 states | Cumulative v1–v4 semantic-wave versions, floor-aware selection, retained actual-binary downgrade matrices, and A2/A3/A4 activation gates |
| A1 predeclares unresolved archive projection types | Compile/publish only V1 at A1; append V2/V3/V4 at I6/I7/I8 |
| `RecoveryRequired` migration behavior is guessed | Closed §15.6 eligibility; representation-only migration preserves recovery/drift and next action |
| Typed compatibility errors drift across drivers | Pin append-only taut codes and human/JSON/JSONL projections at I2 |
| Feature-free operations remain on legacy v0 after A1 | Freeze `max(active_writer_floor, requested_semantics)`; A1 and all later writers have a v1 floor; test the complete creation matrix |
| `writer_version` is mistaken for lifecycle compatibility | Gate on the closed semantic record version; prove same-version maintenance rewrites remain readable |
| Archived v0 history is reconstructed from today's workspace | Separate evidence-only archive projection, closed availability/gap types, no live observations, and byte-stable read tests |
| A valid legacy archive is rejected for lacking newer acceptance detail | Project `LegacyUnavailable` or `NotAccepted`; reserve `ArchivedRecordUnreadable` for durable internal contradiction |
| A supported non-current v1–v3 archive falls between current and legacy paths | Version-specific archive decoders, `SupportedPersisted` with source version, version-appropriate projection, and A2–A4 archive matrices |
| Older GC deletes an unsupported future archive | Classify the header first and forbid status/retention/targeted-GC mutation on `UnsupportedRecordVersion` |
| Envelope mismatch yields inconsistent machine codes | Ordered disjoint header classifier plus full cross-driver pair/malformed-field cross-product |
| Actual-reader gates become ad hoc or silently skipped | Tagged Rust/Python checksum manifest, declared runtimes/unsupported tuples, Linux/Windows behavioral lanes, full-platform release evidence, content-addressed cache, bounded invocation, and fail-not-skip policy |
| A same-version addition lets an old reader ignore new lifecycle meaning | Previous-release executable matrix; require rejection or exact old-record behavior, otherwise bump version |
| Restart regenerates a different accepted lock | Persist exact accepted workspace/YAML/hash before the publication decision and consume it immutably |
| Accepted root requires a fabricated parent on an unborn workspace | Typed born/unborn root base with exact symbolic-branch validation and checked first-commit rollback |
| No-op completion bypasses accepted workspace | Freeze acceptance before deriving publication necessity and archive it for every successful operation |
| Migrated extensions disappear on the next ordinary rewrite | Preserve unknown manifests on every write and freeze container-retirement rules |
| Typed actions become a generic workflow framework | Limit variants to concrete merge-lifecycle mutations and keep one pending action |
| Rollback mutations escape the action journal | Complete mutation inventory, typed rollback/delete actions, persisted reverse cursor |
| M6 policy leaks into Git mechanics | Approve checkout/branch/ownership/root policy at I6 before v2 freeze and execute typed actions only |
| Branch deletion destroys a foreign ABA replacement | Conservative no-delete default until an ownership design passes concurrency/ABA tests |
| Root checkout hides recovery metadata | Reject root `--into` until stable storage and metadata authority are approved |
| M8 creates contradictory state combinations | Separate authorization, outcome, integration progress, and exact lock state |
| Snapshot ID points to replaced audit content | I7 freezes exact artifact-byte hash plus schema/workspace identity; exact commits remain execution authority |
| Finalization becomes the new policy god file | Build the complete accepted workspace before finalization and split publication responsibilities |
| Drivers diverge | Keep lifecycle in core and retain corpus/parity gates |
| Mechanical file movement obscures semantic changes | Separate pure moves, model changes, record upgrade, and features |
| `merge/mod.rs` becomes the next god file | Assign dispatch/gate/guard/store/persistence owners in R1 and keep root orchestration under the review trigger |
| Full M6/M8 design delays v1 durability/no-ff | Freeze only the I1 direction memo for v1; require I6/I7/I8 review before v2/v3/v4 |
| Refactor delays low-risk M5 | Ship v0-safe custom messages after R2a; defer no-ff to v1 and future feature fields to their wave checkpoints |

## 21. Stop conditions

Stop the current package for design review if:

- an M4 record cannot be adapted without guessing;
- a Git mutation lacks enough evidence for exact restart reconciliation;
- any start/continue/preserve/abort/publication/cleanup mutation has no explicit
  durable progress owner;
- rollback order cannot be reconstructed from persisted state alone;
- branch ownership cannot be proven before rollback deletion;
- concurrent same-commit creation or same-commit ABA cannot be distinguished
  under a proposed delete policy;
- root `--into` depends on metadata in the tree being switched;
- skip behavior would remove a selected participant from durable reporting;
- optional authorization and runtime skip outcome are stored in one field;
- finalization must inspect ad hoc skip flags or reconstruct lock rows instead
  of consuming the complete accepted workspace;
- a successful no-publication path can complete/archive without a frozen
  accepted workspace;
- root publication input and its publication-created output form a cycle;
- an attached unborn root requires a fabricated commit value or is rejected;
- an unborn root can become born before publication without typed drift;
- a feature requires driver-specific lifecycle logic;
- the refactor changes protocol values or M4 output before an approved interface
  change;
- v1 is frozen before the I1 checkout-evidence/lock-domain memo, or v2/v3/v4
  is frozen before I6/I7/I8 approval;
- R3's new-version writer or open-v0 migration can be reached before R4b is
  installed and A1 is explicitly reviewed;
- any shipped writer can create an open record whose invariants the co-shipped
  finalizer cannot consume on continue, preserve, abort, rollback, publish,
  no-publication completion, and archive paths;
- any binary accepts an open record solely because it recognizes the
  schema/version pair's fields or enum variants without proving complete
  executable support for that semantic version;
- v1 contains M6 branch, M7 snapshot, or M8 partial-composition wire variants
  that A1 cannot execute;
- A1 compiles or publishes a V2–V4 archive-projection body/discriminant rather
  than stopping at its header-only registry;
- any v0 writer makes `mode: no_ff` newly writable;
- a same-version gate considers only new fields and omits new values, enum
  variants, combinations, or a materially different decode generation;
- a required retained Rust/Python artifact or runtime, checksum,
  Linux/Windows behavioral lane, full-platform release artifact, or fixture is
  missing and the release gate skips rather than fails;
- an A1-or-later binary creates an ordinary new operation as v0;
- new-operation selection does not take the maximum of the active writer floor
  and requested semantic level;
- a branch, snapshot, optional/partial, or future recovery semantic is written
  without selecting its sufficient version before the first requiring action;
- a newer binary recalculates/upgrades an existing v1–v4 operation's version,
  or upgrades v0 through any path other than the approved atomic v0-to-v1
  migration;
- an A1–A4 activation lacks its exact feature-free and feature-specific
  creation-matrix assertions;
- compatibility depends solely on `writer_version`;
- a new compatibility/recovery result has no pinned append-only
  `GwzErrorCode` and cross-driver machine projection;
- an envelope can satisfy more than one typed classification, or an exact
  allocated future pair requires compiling its body to reject it;
- an A2/A3/A4 activation lacks the retained actual-older-binary matrix for
  every durable state and lifecycle command the new wave can persist;
- an archive adapter consults live workspace/repository state, rewrites an
  archive during read, or fabricates missing acceptance;
- open-v0 migration eligibility is inferred at a call site rather than selected
  by §15.6's closed rule;
- a supported older v1–v4 archive is treated as legacy-unavailable or rejected
  solely because it is not the current highest version;
- an unsupported future archive can be deleted or rewritten by status,
  retention, or targeted GC;
- terminal outcome is nested inside only one acceptance-availability variant
  instead of being its sibling;
- a valid terminal legacy archive becomes unreadable only because newer
  acceptance detail is unavailable;
- an unavailable/not-accepted historical projection can authorize a mutable
  lifecycle path;
- GC requires historical live repositories in order to delete a structurally
  valid terminal archive;
- a compatibility adapter duplicates acceptance, root, publication-prefix, or
  next-action policy instead of calling R4a;
- the legal v0 publication inventory cannot be expressed as a closed
  progress-aware §15.3.2 row;
- a persisted candidate's accepted lock is reconstructed or reserialized
  instead of adopting its exact `lock_yaml` bytes;
- an accepted unborn root with a born live HEAD is accepted without verifying
  the exact first evidence commit, branch, files, tree, hashes, and
  `parent = None`;
- adaptation or restart selects a different next action from the original v0
  state;
- any same-version shape is proposed without actual supported
  previous-release binaries proving rejection or exact equivalent behavior;
- migration cannot preserve and verify every unknown nested field;
- a subsequent current-version rewrite can drop an unknown field whose
  container survives;
- a removed container has no frozen unknown-extension retirement rule;
- post-rename recovery depends on an in-memory hash/model lost in the crash;
- accepted lock YAML/hash/parsed-artifact/audit invariants disagree;
- accepted workspace bytes can be regenerated after their publication boundary;
- snapshot audit provenance identifies only a mutable snapshot ID;
- the canonical model needs arbitrary maps or string action names to proceed;
- `merge/mod.rs` gains cross-cutting lifecycle/store policy without an assigned
  §13–§14 owner;
- the proposal, executable merge plan, authoritative merge design, or public
  capability/deferred-feature text disagrees on M5a/M5b/A1 or I6/I7/I8;
- behavior and mechanical movement can no longer be reviewed separately; or
- an ambiguous live state is being converted into success or safe retry.

## 22. Architecture acceptance criteria

Approval is incremental.

### 22.1 R0–R1 acceptance

R0–R1 may proceed now and are complete when:

1. all M4 behavior and release gates remain green;
2. every legal M4 record state and mutation boundary has characterization
   coverage;
3. every legal terminal archived-v0 shape has evidence-only characterization
   coverage independent of live workspace state;
4. §15.8 runs the selected retained durable-v0 Rust and Python readers on the
   required Linux/Windows behavioral lanes, retains full supported-platform
   release evidence, freezes the
   custom-message success plus v0 no-ff failure fixtures, and runs v0.9.2 only
   as the explicit pre-record downgrade lane;
5. every mutation has an inventoried durable progress owner;
6. `merge/mod.rs` cross-cutting responsibilities have the §13–§14 target
   owners;
7. the change-budget ledger has numeric LOC/file ceilings and declared
   wire/protocol deltas for R1, R2a, and M5a;
8. the cross-document consistency gate enforces the M5a/M5b/A1 and
   I6/I7/I8 sequence; and
9. current participant policy has one semantic owner per decision.

### 22.2 R2a/M5a acceptance

R2a/M5a are complete when:

1. existing merge pending actions run through the minimal integration seam;
2. M4 v0 serialization and recovery remain compatible;
3. R2a is the single owner of exact final custom-message bytes and their tests;
4. the selected retained durable-v0 Rust and Python readers prove
   custom-message recovery equivalence, with undistributed platform tuples
   explicit rather than substituted;
5. M5a rejects `--no-ff` before record creation and no v0 path writes
   `mode: no_ff`; and
6. M5a lands without speculative checkout/skip/composition fields or
   duplicated lifecycle policy.

### 22.3 Interface and record checkpoint acceptance

Record-changing work remains blocked until:

1. I1 freezes the M6 direction that v1 separates integration ref from final
   checkout while permitting only the existing checkout and no branch action;
2. I1 freezes the M8 direction that v1 acceptance covers the complete
   baseline/audit domain while permitting only required/participated outcomes;
3. the schedule explicitly defers full M6 and M8 policy to I6/I8 because v1
   serializes none of those variants, with mandatory re-review before v2/v4;
4. the complete v1 post-operation `LockArtifact` and member audit rules are
   deterministic for selected and unselected members;
5. accepted root input distinguishes attached-born, detached-born, and exact
   attached-unborn checkouts without sentinels; detached-born is legal only
   for no-publication, while attached inputs retain M4 checked first-commit and
   rollback behavior;
6. root integration input is distinct from publication-created output;
7. `AcceptedWorkspace` is frozen as immutable persisted evidence before the
   publication-required decision, with exact
   YAML/hash/parsed-artifact/audit/root invariants and a restart contract that
   never regenerates accepted bytes;
8. every successful v1 no-publication result archives that same accepted
   workspace, including all-up-to-date results;
9. v0 remains limited to M4/custom-message-compatible semantics, v1 adds
   no-ff/acceptance, and the cumulative
   v1-acceptance/v2-branch/v3-snapshot/v4-partial version allocation,
   `max(active_writer_floor, highest_requested_semantic_version)` creation
   rule, M5a-v0/A1-and-later-v1 floor, exact creation matrix, pre-intent freeze,
   and existing-record non-upgrade rule are approved;
10. v1 contains only integration/no-ff/acceptance semantics executable by A1
    and no dormant M6–M8 lifecycle variants;
11. the ordered disjoint envelope registry/classifier, pinned append-only
   `GwzErrorCode` allocations/projections, adapters, unknown-field migration
   and subsequent
   current-version rewrite rules, container-retirement table, atomic-rename
   boundary, restart semantics, and old-binary behavior are frozen;
12. the closed open-v0 publication-progress mapping covers every legal
    pre-acceptance, candidate, evidence, partial-publication, preservation,
    rollback, no-publication, and terminal-before-archive state, with exact
    lock-byte/root/prefix evidence, typed contradictions, and v0-equivalent
    next actions;
13. open v0 `mode: no_ff` is rejected before adaptation/migration, the actual
    retained durable-v0 baseline failing fixture is frozen, the v0.9.2
    pre-record downgrade behavior is frozen separately, and every other legal v0 row's migration
    eligibility—including `RecoveryRequired` and operation drift—is explicit
    and fixture-proven;
14. archive decoding freezes the general per-supported-version rule plus only
    concrete v0/v1 projection bodies at I2; sibling terminal
    outcome/acceptance, unsupported-future non-deletion, cross-driver parity,
    byte-stable reads, and live-independent GC are complete, while V2–V4
    bodies/discriminants remain owned by I6/I7/I8;
15. §15.8 freezes tagged Rust/Python artifact sources, runtimes, digests,
    behavioral Linux/Windows lanes, full supported-platform release evidence,
    supported decode generations, explicit unsupported tuples, caching,
    invocation, and fail-not-skip behavior;
16. the production v1 writer and migration dispatch are specified as
    disabled through R3 and enabled together only by A1 after R4b;
17. actual older-binary matrices, same-version `writer_version` compatibility,
    and A2/A3/A4 activation rules are frozen for every later semantic wave;
18. I7 owns the v3 source, wire, archive-projection, unknown-retirement, and
   A1/v2 downgrade-fixture freeze, with exact artifact
   hash/schema/workspace identity separate from authoritative per-participant
   execution commits;
19. branch, source-provenance, and partial-composition proof-gate ownership is
    assigned to I6/I7/I8; and
20. the frozen I1/I2 contracts receive explicit re-review before R3 begins.

### 22.4 Writer-activation acceptance

A1 remains blocked until:

1. R4a is the single semantic owner for complete-lock construction, accepted
   root checkouts, publication-required classification, candidate-prefix
   reconciliation, and next-action selection;
2. R3's production writer and migration paths are demonstrably unreachable;
3. R4b makes every finalization, continuation, preservation, abort, rollback,
   no-publication, and archive path consume the persisted acceptance;
4. M5b's deterministic no-ff prepared action is installed but unreachable
   until the activation change;
5. every §15.3.2 fixture passes through production dispatch with exact
   candidate bytes, root/evidence/prefix checks, typed pre-mutation rejection,
   and the same next action as the original v0 record;
6. every §15.3.3 v0/v1 archive fixture projects identically across core, CLI,
   Python/native, JSON, JSONL, retention, and GC without live observation or
   read rewrite;
7. the complete §15.2 envelope cross-product is identical across drivers;
8. the installed binary can resume every v1 open-record state it can write and
   contains no executable v2–v4 state or V2–V4 archive-projection
   body/discriminant;
9. A1 creates ordinary/custom-message and no-ff operations as v1, never v0,
   while §15.6-eligible open v0 uses only the approved atomic migration,
   `RecoveryRequired`/operation-drift migration remains representation-only,
   and v0 no-ff is rejected;
10. retained v0 readers fail closed on v1 while the A1 binary rejects
    representative v2/v3/v4 envelopes before body adaptation or mutation
    eligibility; and
11. the small activation change enabling the v1 writer, migration, and no-ff
    start path together receives explicit review.

### 22.5 Later semantic-wave activation acceptance

A2, A3, and A4 each remain blocked until:

1. the wave's wire body contains only that cumulative semantic level and the
   binary implements every lifecycle path it can express;
2. I6 has approved v2 before M6/A2, I7 has approved v3 before M7/A3, and I8
   has approved v4 before M8/A4;
3. immutable intent selects and persists
   `max(v1 active floor, highest requested semantic version)` before the first
   feature-specific action;
4. the release passes the exact creation matrix: feature-free direct-ref and
   no-ff v1,
   M6 branch v2, M7 snapshot v3, and M8 optional/partial v4 wherever supported;
5. every actual older supported binary rejects every new durable state and
   applicable start, status, continue, abort, preserve, rollback, finalize,
   migration, or GC entry point before mutation;
6. the new binary continues and recovers every earlier supported record
   version without changing that existing record's version;
7. A2 validates/projects v1–v2 archives, A3 v1–v3, and A4 v1–v4, while every
   unsupported future archive is protected from status/retention/targeted-GC
   mutation;
8. same-version records remain readable across maintenance-only
   `writer_version` changes;
9. any newly writable same-version shape passes every retained decode
   generation's complete behavioral matrix; and
10. the small writer activation receives explicit review after the feature and
   compatibility matrices pass.

### 22.6 Final lifecycle acceptance

The broader refactor is complete when:

1. intent, progress, pending action, rollback progress, participation
   authorization/outcome, complete lock, and root publication are separate
   concepts;
2. every forward and reverse mutation is restart-reconciled through its named
   durable owner;
3. every legal old open state has a tested safe adaptation or typed rejection
   under the approved version strategy;
4. v1 writer/migration activation occurred only after §22.4 passed;
5. every new operation used the active-writer-floor creation rule and every
   existing operation retained its version except approved v0 migration;
6. every current-version rewrite preserves the expected surviving unknown-field
   manifest;
7. archived v0 history projects only durable evidence and every supported
   v1–v4 archive projects through its version-specific decoder, with no live
   reconstruction or read rewrite;
8. finalization consumes and archives the complete accepted workspace on both
   publication and no-publication paths;
9. born and unborn root finalization/abort retain exact checked behavior;
10. finalization responsibilities remain below agreed review limits;
11. I6-approved M6/v2 branch lifecycle does not change v1 integration
    semantics;
12. I7-approved M7/v3 snapshot provenance does not change recovery execution;
13. I8-approved M8/v4 policy does not leak into publication mechanics; and
14. every later semantic writer activated only after §22.5 passed.

## 23. Interface decisions required before later record freezes

The following full product semantics may remain open through M5a and v1/A1
because I1 freezes only their directional boundaries and v1 contains none of
their executable variants. They may not remain open when I6 freezes v2 or I8
freezes v4. I7 separately freezes the complete v3 source/provenance,
archive-projection, unknown-retirement, and downgrade-fixture contract before
M7:

### M6

- whether every selected target branch must already exist;
- when branch creation is allowed and at which exact start commit;
- whether branch policy must be uniform across the selection;
- detached-HEAD start and restoration behavior;
- whether successful completion restores the original checkout or leaves the
  target checked out;
- ownership and cleanup of newly created branches;
- behavior after an interrupted, unrecorded creation;
- same-commit concurrency and ABA policy;
- exact rollback action order; and
- whether root `--into` remains unsupported or gains a stable storage and
  metadata-authority design.

### M8

- the closed list of skippable causes;
- the exact required/optional request authorization;
- whether skipping may occur only in preflight or also after an attempted
  integration;
- partial-success exit status;
- how required and optional participants are requested;
- exact complete-lock rows/membership for participated, skipped, and unselected
  members;
- root skip/participation rules;
- continue and abort scope;
- preservation ownership; and
- machine and human reporting terminology.

I6 and I8 encode the reviewed branch/participation answers explicitly while v1
remains unchanged. I7 freezes the source contract. M6, M7, and M8 implement v2,
v3, and v4 respectively only after their checkpoints and older-reader gates
pass; v1 does not claim to execute those later waves.

## 24. Final decision

Proceed with R0, R1, the minimal R2a integration/message seam, and the
custom-message M5a slice. The eight numbered reviews and both independent F5
reviews support that immediate scope after one correction: do not activate
`--no-ff` in v0. Actual v0.10.x readers can silently execute different
recovery semantics, while v0.9.2 can ignore the durable operation entirely;
no-ff therefore moves to M5b/v1/A1.

`gwz-core/dev-docs/GwzMergePlan.md` and `GwzMergeDesign.md` must use that same
sequence before M5 work is dispatched. The R0 document-consistency gate keeps
the plan, design, proposal, and public capability/deferred-feature text from
reintroducing a v0 no-ff release path.

Do not approve v1 work until I1 records the M6 checkout-evidence and M8
lock-domain directions and I2 passes the newly-writable-shape reader gate,
retained-binary harness, disjoint envelope classification, semantic-wave/
writer-floor versioning, open-v0 and all-version archive adaptation,
accepted-root-checkout, no-publication acceptance, and rewrite-preservation
gates.
Full M6/M8 product policy intentionally waits for I6/I8 because v1 serializes
none of those executable variants.

After that checkpoint, sequence the work as R4a behavior-preserving acceptance
extraction, R3 record/adapter implementation with its production writer and
migration hard-disabled, R4b persisted-acceptance/finalizer consumption, M5b
no-ff implementation behind the same boundary, and only then the separately
reviewed A1 activation. No release may create a new-version open record until
its installed finalizer understands and can resume every invariant and
publication state that record can express.

A1 activates only v1 integration/no-ff/acceptance semantics. I6/M6, I7/M7,
and I8/M8 add and activate cumulative v2, v3, and v4 records through A2, A3,
and A4 after actual checksum-pinned older Rust/Python readers prove typed
pre-mutation rejection. `writer_version` is diagnostic, not the compatibility
contract.

For new operations, M5a has a v0 writer floor and A1 plus every later release
has a v1 floor. No-ff requires v1; requested M6/M7/M8 semantics raise that
floor to v2/v3/v4 via
`max(active_writer_floor, highest_requested_semantic_version)`. Existing
records keep their version; only the closed §15.6 open-v0 eligibility set is
upgraded, including representation-only `RecoveryRequired`/operation-drift
migration, and v0 no-ff is rejected.

Archived v0 records use the evidence-only legacy projection; every supported
v1–v4 archive uses its own persisted-acceptance decoder/projection. Terminal
outcome is separate from acceptance availability. Archives are never
reconstructed from the live workspace or rewritten on read; supported durable
contradiction is unreadable, and unsupported future archives are protected
from older status/retention/targeted-GC mutation.

The desired outcome is not less merge code at any cost. It is a smaller number
of places where merge policy can be expressed, with every mutation and every
workspace-acceptance decision backed by exact durable evidence.
