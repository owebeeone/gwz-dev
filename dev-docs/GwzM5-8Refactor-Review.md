# Review: GWZ M5–M8 Merge-Lifecycle Refactor Proposal

Date: 2026-07-28

Reviewed document: `dev-docs/GwzM5-8Refactor.md`

Verdict: **request changes before approving R2–R4 or freezing a v1 durable
record**

## Overall assessment

The proposal has the right architectural direction:

- preserve the proven M4 execution and recovery machinery;
- separate immutable intent from mutable progress;
- keep exact source commits authoritative during recovery;
- use typed, persisted mutation intent rather than loose flags;
- centralize participant decisions instead of copying state matches;
- isolate finalization responsibilities; and
- treat open-record compatibility as a designed feature.

R0 and the inventory/characterization portion of R1 should proceed. The
proposal is not yet ready to authorize the canonical model or v1 record,
however. The illustrative types omit mutations that the stated durability rule
requires, the participation model conflates policy with outcome, and the
accepted-composition shape cannot deterministically describe the lock and root
state it is supposed to publish. Those are schema-shaping issues, not details
that can safely be deferred until implementation.

## Findings

### [P1-1] The typed action set omits rollback mutations required by its own invariant

**Where:** §7.1–§7.5, especially the `PendingParticipantAction` enum; §19,
“no Git mutation occurs without a typed durable pending action.”

The proposed action set contains:

```rust
Integrate
CreateBranch
SwitchCheckout
RestoreCheckout
```

It does not contain the checked deletion of an operation-owned branch described
in §7.2, nor an explicit integration rollback action. Both are Git mutations.
For example, aborting after an operation-created target branch has been
integrated and restored may require, in reverse order:

1. rolling the target branch back;
2. restoring the original checkout; and
3. deleting the unchanged operation-owned target branch.

`RestoreCheckout` cannot safely stand in for branch deletion or ref rollback.
If those mutations happen outside `PendingParticipantAction`, the design
violates its central rule. If `Integrate` is intended to cover forward and
reverse ref movement, its name, evidence, and reconciliation states are
underspecified.

**Required correction:**

- inventory every participant Git mutation on start, continue, preserve, and
  abort;
- add explicit typed actions for checked integration rollback and owned-branch
  deletion, or define an equally explicit existing journal that owns them;
- define durable rollback progress/cursor state so reverse ordering survives a
  restart between rollback actions; and
- state whether preservation ref/stash mutations use this action journal or a
  separate typed journal, then narrow §19 accordingly.

The v1 action enum must not be frozen until every M6 forward and reverse
mutation has an exact not-started/completed/ambiguous reconciliation contract.

### [P1-2] `AcceptedComposition` is not sufficient to construct the accepted workspace

**Where:** §9, `AcceptedComposition`; §17 R4; §22 criterion 6.

The proposed composition contains only a map for selected targets, and each
entry contains only a commit or exclusion reason. The actual lock is a complete
workspace artifact. A member lock row includes path, source identity, source
kind, commit, branch, detached state, upstream, dirty state, and materialization
state. Current finalization starts with the complete lock and applies selected
participant results; see
`gwz-core/src/workspace_ops/merge/finalize.rs` and
`gwz-core/src/artifact/mod.rs`.

This becomes correctness-critical in M6:

- integration may occur on a target branch while the successful final checkout
  is restored to the original branch;
- the integrated target ref and the live checkout HEAD may therefore be
  different commits;
- `UseCommit { commit }` cannot say which branch/detached state belongs in the
  lock;
- unselected members still need deterministic treatment in the complete lock;
  and
- `Excluded` does not say whether a selected member is omitted from the lock,
  retained at baseline, or retained with changed materialization metadata.

The root is also not an ordinary lock member. The root integration result is an
input to publication, while the final root composition/evidence commit is
created by publication. Treating both as a generic participant
`UseCommit` creates an input/output cycle.

This conflicts directly with §23, which leaves successful checkout restoration,
lock treatment for skipped members, and root participation unresolved.

**Required correction:**

Define composition as either:

- a complete post-operation workspace state; or
- a frozen baseline lock plus a complete, typed overlay whose application is
  deterministic and accounts for every active/unselected member.

The selected-member entry must distinguish at least:

- the integration result/ref;
- the accepted final checkout/materialization state; and
- the exact resulting lock row or a deterministic rule for producing it.

Root composition must have a separate type that distinguishes the accepted
root integration base from the publication-created composition commit.
`KeepBaseline` versus `Excluded` must not be frozen until M8 decides the lock
semantics of a skip.

### [P1-3] `ParticipationDisposition` conflates immutable policy with runtime outcome

**Where:** §8; §23 M8 decisions.

The proposal correctly states that “should be attempted” and “does it
contribute” are different dimensions, but the proposed type does not preserve
that separation:

```rust
enum ParticipationDisposition {
    Required,
    Skipped(SkipEvidence),
}
```

It cannot represent an optional/skippable participant that successfully
participated. Calling that participant `Required` loses the request policy that
made a skip legal. Calling it `Skipped` is false. This also makes it impossible
to validate after restart whether a recorded skip was authorized by the
original request.

**Required correction:**

Use two durable dimensions, for example:

```rust
enum ParticipationIntent {
    Required,
    Optional(OptionalPolicy),
}

enum ParticipationOutcome {
    Participated,
    Skipped(SkipEvidence),
}
```

The exact names may differ, but immutable skip authorization must be recorded
separately from the eventual outcome. Composition then validates the outcome
against the frozen authorization and execution evidence.

### [P1-4] R2/R3 freeze durable types before the M6 and M8 decisions that define them

**Where:** §2.2, §15, §17 R2/R3, §22, and §23.

The proposal says M6 branch-creation policy and M8 skip/ownership policy are out
of scope, yet R2 introduces their canonical type dimensions and R3 freezes v1
before those product designs are approved. §23 then lists decisions that
directly determine the durable schema:

- branch creation and ownership;
- successful checkout restoration;
- required versus optional selection;
- post-attempt skipping;
- skipped-member lock treatment;
- root composition; and
- preservation/rollback ownership.

The document also leaves the persistence choice open in §15.2—v1 migration is
recommended, but retaining v0 with added fields may still be proposed—while R3
and the acceptance criteria assume v1. That is not a closed checkpoint.

M5 cannot validate the missing dimensions. §16.1 explicitly says M5 must not
use branch actions, skip disposition, or snapshot provenance. It can prove the
integration-intent seam, but it cannot be a go/no-go proof for M6 action
ownership or M8 composition.

**Required correction:**

- decide the durable-version strategy before authorizing R3;
- approve the schema-shaping M6 and M8 interface policies before freezing v1,
  even if their behavior is implemented later; and
- define separate proof gates for integration actions, branch lifecycle, source
  provenance, and partial composition.

If M5 is meant to remain the next user-facing release, implement it after a
minimal behavior-preserving semantic extraction rather than making it wait for
speculative M6/M8 record fields.

### [P2-1] Branch ownership evidence is named but not defined

**Where:** §7.2 and §21.

“Branch exists at the exact start commit” is not proof that GWZ created it.
After the persisted create intent, another process can create the same branch
at the same commit. A delete/recreate ABA sequence can also produce the expected
ref value. The boolean “this operation owns deletion” records a claim, not
evidence.

The proposal correctly says uncertainty must become recovery-required, but it
does not define what evidence can turn the crash window after branch creation
into “completed exactly.” Without such evidence, GWZ must conservatively leave
the branch behind after any interrupted creation, which is a valid policy but
must be explicit.

**Required correction:**

Before the branch-action schema is frozen, choose and test an ownership proof
mechanism, such as an operation-owned ref/marker created with an appropriate
checked ref transaction, or explicitly specify that an unrecorded creation can
never authorize deletion. Include concurrent same-commit creation and
delete/recreate-at-same-commit tests.

### [P2-2] Root `--into` cannot be treated as an ordinary checkout action without a storage policy

**Where:** §4.1 root-last invariant; §6 checkout intent; §16.2; §23 M6
root-specific target-branch behavior.

The root worktree contains the workspace manifest, lock, exclusion boundary,
and `.gwz/merge` recovery record. Switching the root checkout may expose a
different manifest/lock, a different workspace identity, or tracked paths that
collide with runtime recovery data. Current M4 recovery deliberately discovers
the open record before trusting a live manifest, but the proposal does not
state what happens when GWZ itself changes the root checkout.

A generic `SwitchCheckout` variant does not answer:

- which manifest and member paths remain authoritative while the root is on
  the target branch;
- whether the merge record location is guaranteed to survive checkout;
- how nested invocation discovers the operation after the switch;
- which root metadata is restored before abort/finalization; or
- whether selecting `@root` with `--into` is initially unsupported.

**Required correction:**

Make root target-branch handling an interface prerequisite for R2/R3. Either
disallow root `--into` in the first M6 release or define a stable recovery
storage and metadata authority that does not depend on the checked-out root
tree. Add restart tests where the target root branch has missing, changed, or
conflicting GWZ metadata.

### [P2-3] The v1 decoder and migration preservation contract are incomplete

**Where:** §15 and §17 R3.

The current store deserializes directly into `MergeOperationRecord` and checks
the schema constants afterward. It also deliberately carries unknown nested
fields forward when rewriting a record. A real v0/v1 adapter requires a
different decode boundary.

The proposal does not yet require:

- reading and validating the schema/version header before deserializing a
  version-specific body;
- an allowed matrix for the redundant schema string and numeric version;
- preservation or explicit rejection of unknown v0 extension fields during
  v0-to-v1 rewrite;
- verification of the exact upgraded bytes/model after atomic replacement;
- behavior when an older binary encounters an open v1 record; or
- downgrade/retry messaging after the first v1 write.

Losing unknown fields during migration would regress an explicit M4 store
guarantee.

**Required correction:**

Specify a small wire envelope/header dispatcher and version-specific decoders.
Require nested unknown-field preservation across migration, or reject the
upgrade if preservation cannot be proven. Add fixtures for mismatched
schema/version pairs, unknown nested v0 fields, every open publication and
preservation state, interrupted upgrade publication, and old-binary
fail-closed behavior.

## Recommended sequencing

A safer sequence is:

1. **R0:** freeze the clean M4 baseline and complete the fixture inventory.
2. **R1:** extract current pure participant semantics without changing the
   serialized record.
3. **R2a:** wrap the existing integration pending action and immutable merge
   intent only; avoid speculative branch/skip fields.
4. **M5:** implement and release `--no-ff` and custom messages through that
   integration seam.
5. **Interface checkpoint:** approve M6 branch ownership/root/restore policy
   and M8 required/optional/lock/root policy.
6. **Record checkpoint:** freeze the complete action inventory, participation
   dimensions, accepted-composition model, and v0/v1 migration contract.
7. **R3/R4:** implement migration, behavior-preserving finalization extraction,
   and deterministic composition consumption.
8. Proceed with M6–M8 behind their individual proof gates.

If avoiding a second durable-record redesign is a hard requirement, the M6 and
M8 product-interface decisions must move earlier. If those decisions should
remain deferred, the architecture must explicitly permit a later record
version instead of pretending v1 can already be frozen.

## Non-blocking comments

- The `SourceProvenance`/exact-commit separation is sound and should be kept.
- The finalization split follows the existing responsibility boundaries and is
  justified by the current size of `merge/finalize.rs`.
- One pending action per participant remains a good constraint if completed
  forward and rollback progress is sufficient to derive the next legal action.
- R0 should enumerate legal record states rather than generating a blind
  cross-product of enum values.
- The M5 interface checkpoint should freeze the exact final commit-message
  bytes—including placement and normalization of mandatory GWZ recovery
  identity—because those bytes participate in prepared-commit recovery.

## Approval recommendation

Approve R0 and R1 now. Approve the general architectural direction. Do not
approve R2–R4 or the v1 schema until P1-1 through P1-4 are resolved and the
three P2 recovery contracts are made explicit.
