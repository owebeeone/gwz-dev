# R4b-X participant/continue/recovery state-machine review

Date: 2026-08-10

## Verdict

**GO. P0: none; P1: none; P2: none; P3: none.**

The settled R4b-X implementation closes all six blockers from the prior
review. Participant preparation and execution remain behind exact checked
record authority, recovery verifies the literal recorded origin and every
selected participant, and deterministic `NoFf` fast-forward execution is
restart-safe from the frozen durable action.

This was a read-only implementation review. The reviewer changed no
implementation file; this memo is the only review artifact authored here.

## Reviewed boundary

The direct review covered:

- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/forward.rs`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/forward.rs` and
  `forward/execute.rs`;
- the participant selection, observation resolution, and invocation fences in
  `authority/{dispatcher,resolver}.rs` and
  `authority/dispatcher/invocation.rs`;
- the checked service, lease, reload, and store interfaces used by forward
  execution;
- `status::{observe,pending}` participant path and pending-action
  reconciliation;
- the Git prepared-merge contract, preparation, validation, and execution
  implementation; and
- the focused forward, generic attempt/reconciliation, service sequence, and
  Git prepared-merge tests.

The review applied the current `GwzM5-8R4bTransitionDesign.md`,
`GwzM5-8Refactor.md`, and `GwzM5-8ChangeBudget.md` contracts. It also applied
the revised general source-file policy: 1,000 lines is a cohesion-review
trigger, a small overrun may remain cohesive, and a warranted split should
produce responsibility owners below 500 lines.

## Closed blocker matrix

| Prior blocker | Settled result | Evidence |
| --- | --- | --- |
| Recovery proof was inferred from durable context instead of live origin-specific evidence | Closed. `Executing`, `AwaitingResolution`, and `Halted` use the forward live verifier; `Finalizing` delegates to the finalization verifier; `Preserving` and `RollingBack` are rejected by the forward runtime with `MergePhaseUnsupported`. A pending owner is reconciled exactly, then every other selected participant is also freshly verified. | Concrete finalizing delegation, reverse-origin rejection, exact-owner recovery, cross-member drift rejection, and still-ambiguous rejection tests. |
| `VerifyUpToDate` could enter a `NotStarted`/no-progress execution loop | Closed. Exact `NotStarted` for `VerifyUpToDate` is adopted immediately as a verified `UpToDate` outcome with `resulting_commit = before_commit`; no physical participant action is issued. | `up_to_date_action_is_adopted_without_participant_execution` asserts zero execution calls. |
| Expected-conflict adoption cleared the conflict snapshot | Closed. Expected conflict reconciliation obtains the exact original conflict snapshot and persists each normalized path and SHA-256 in the `Conflicted` outcome. | The real-conflict test pins the single `README.md` path and the exact SHA-256 of the conflict bytes before resolution. |
| Semantic preparation drift was converted into a durable participant failure/halt | Closed. `DirtyMember`, `MergeDrift`, `MergeRecoveryRequired`, and `AcceptanceInputDrift` preparation errors produce bound ambiguity and literal-origin recovery. Operational preparation failure still takes the typed failure/halt path. | `semantic_preparation_drift_enters_executing_recovery_before_owner_or_git_mutation` proves `RecoveryRequired(origin=Executing)`, a still-`Planned` row, no pending owner, unchanged HEAD, and preserved untracked bytes. |
| Pre-owner conflict continuation either crossed an invalid intermediate state or mishandled unresolved work | Closed. `AwaitingResolution + Continue` first observes readiness while the operation remains stopped. Unresolved work returns `MergeValidationFailed` without changing the state or creating an owner. A resolved preparation proof authorizes only `BeginExecution`; the action is then freshly prepared under `Executing` and persisted before mutation. | The real resolved-conflict test reaches `Continued`/`Completed`; the unresolved test remains byte-valid `AwaitingResolution` with no pending action. Reducer and predecessor matrices cover the legal state edges. |
| `NoFf` on a fast-forward graph lacked an exact deterministic primitive and restart classification | Closed. Merge mode is consumed only during read-only preparation. `ForceMergeCommit` freezes a `GitPreparedMerge::Commit` containing the source tree and exact signatures. The durable prepared variant is then the sole validation/execution authority: a `Commit` on a fast-forward graph creates the exact two-parent commit, while a `FastForward` advances directly. Restart validates or matches that frozen commit and adopts it without participant re-execution. | Git prepared-merge coverage and the concrete `no_ff_fast_forward_creates_a_two_parent_merge_commit` and `no_ff_restart_adopts_the_exact_prepared_merge_commit` tests. |

## Recovery and restart mapping

The settled recovery mapping is origin-specific and fail-closed:

| Recorded origin | Forward-runtime verification |
| --- | --- |
| `Executing` | Reconcile the one pending owner when present; verify every remaining selected row from live participant semantics. Without an owner, require every selected row to be exactly ready for its recorded forward position. |
| `AwaitingResolution` | Require an action-free, error-free conflicted durable shape and exact live native-merge/abort eligibility for the conflict plus exact live state for every other selected row. |
| `Halted` | Require a durable halt cause. Reconcile a retained owner when present and verify all other selected rows; completed retained owners are later consumed by the checked halted-outcome compound. |
| `Finalizing` | Delegate to `verify_finalization_recovery_origin`, which verifies either pre-acceptance participant/root inputs or the exact persisted publication prefix. |
| `Preserving` / `RollingBack` | Reject in the forward runtime. These origins belong to the reverse-lifecycle consumer and cannot receive a forward recovery proof. |

Pending participant observations map as follows:

- exact `Completed` adopts the observed result even when the executor returned
  a late error;
- exact `Ambiguous` overrides either executor diagnostic and enters
  representable recovery, first committing `Halted` when an executing retained
  halt cause requires that literal predecessor;
- exact `NotStarted` after a failed participant execution creates the matching
  owned failure/halt batch;
- exact `NotStarted` after reported success is no progress and cannot execute
  the action again in the invocation; and
- `VerifyUpToDate` is the intentional exception that is directly adopted as a
  no-mutation result before the physical-action resolver.

The service retains one canonical-root `V1MutationLease`, reauthorizes the
exact persisted participant action immediately before Git mutation, and uses
the checked Git primitive to revalidate branch, HEAD, clean state, source
object, and frozen prepared result under the ref lock. After every physical
action it reloads the same record source and rejects changed bytes before the
attempt can reach another dispatch. Invocation-local preparation and execution
fences prevent same-call retry loops and are discarded on restart.

## Path and symlink safety

Participant member paths are parsed as normalized `MemberPath` values and
walked component by component with `symlink_metadata`. Every existing
component must be a real directory; a symlink or non-directory fails with
`PathEscape`. The checked record and mutation lease canonicalize the workspace
root, and path validation is repeated on the immediate execution path before
the checked Git mutation.

The concrete symlink regression replaces the final member directory with a
symlink to an outside repository and proves `PathEscape`, a durable halt, and
an unchanged outside HEAD. The component walk applies the same rejection to
intermediate symlinks. No fail-open participant path was found.

## Halt, error, and ambiguity precedence

The generic authority matrices and concrete forward tests agree on the
required precedence:

1. fresh completion evidence owns a result, regardless of a late diagnostic;
2. fresh ambiguity owns recovery and cannot be converted into a participant
   failure by an executor error;
3. an executing retained halt cause commits `Halted`, is reobserved under the
   new digest, and only then records recovery with literal origin `Halted`;
4. only matching participant `NotStarted` plus a failed diagnostic constructs
   the owned failure/halt batch; and
5. unresolved pre-owner conflict work returns its validation error before any
   invalid state or pending owner can be persisted.

No caller branch, executor return value, or durable context alone can mint a
completion or recovery proof.

## Budget and cohesion review

The current frozen R4b-X ceiling is 900 net production-bearing lines, 1,100
moved production lines, 1,500 test/tool/doc lines, 14 production paths, and 12
test/tool/doc paths.

The conservative settled charge is approximately **958 net production lines
across exactly 14 production paths**:

- 798 current lines in the three dedicated X owners
  (`610 + 87 + 101`);
- 123 net lines in the four Git prepared-merge paths and the shared participant
  path validator; and
- approximately 37 lines of shared lifecycle module wiring and the
  pre-owner dispatcher/resolver/invocation correction.

No production movement is required for the implementation. Focused Rust test
charge is **903 lines** (`826` forward lifecycle lines plus `77` Git prepared-
merge lines); the small design/plan/budget and shared-matrix deltas remain
comfortably below the 1,500-line and 12-path ceilings.

The measured production charge is about 6.4% over the old 900-line stop
ceiling, caused by the exact live recovery verifier, fail-closed component path
check, and deterministic prepared `NoFf` primitive required to close the
reviewed interface. It adds no wire field, phase, transition, normal-build
activation, or second policy implementation.

**Recommendation:** revise only the R4b-X net production ceiling from **900 to
1,050 lines**. Retain moved production at 1,100, test/tool/doc at 1,500,
production paths at 14, and test/tool/doc paths at 12.

No source split is warranted. The largest dedicated production owner is 610
lines and the focused test owner is 826 lines. Both are below the current
1,000-line judgment trigger, have coherent single responsibilities, and keep
policy/test support separated. The below-500 target does not apply unless a
split is architecturally warranted; splitting these files solely for size
would not improve ownership.

## Verification

All commands were run on the final settled bytes:

- `cargo test -p gwz-core workspace_ops::merge::v1_lifecycle::tests::forward -- --nocapture`
  — **16 passed**;
- `cargo test -p gwz-core workspace_ops::merge::v1_lifecycle -- --nocapture`
  — **118 passed**;
- `cargo test -p gwz-core --lib --quiet` — **925 passed, 1 ignored**;
- `cargo clippy -p gwz-core --all-targets --all-features -- -D warnings` —
  passed;
- `cargo fmt --all -- --check` — passed;
- root and `gwz-core` `git diff --check` — passed;
- `python3 scripts/checks/check_merge_docs.py` — **8 sources / 87 assertions**;
  and
- `python3 -m unittest scripts/checks/test_check_merge_docs.py` — **3 passed**.

Final decision: **GO for R4b-X with the narrow production-ceiling correction
above. Finding count: P0 0 / P1 0 / P2 0 / P3 0.**
