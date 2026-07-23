# GWZ Merge M1/M2a Third Code Review Remediation Plan

Date: 2026-07-22

Execution status: **complete; full local gate and two independent final
re-reviews passed; M2b shared integration unblocked** (2026-07-23).

Inputs:

- `dev-docs/GwzDevCodeM2a-RemPlan-2.md`
- `dev-docs/GwzDevCodeM2a-ReviewF5-3.md`
- `dev-docs/GwzDevCodeM2a-Review56-3.md`
- `dev-docs/GwzDevCodeM2a-ReviewF5-4.md`
- `dev-docs/GwzDevCodeM2a-Review56-4.md`
- `dev-docs/GwzDevCodeM2a-ReviewF5-5.md`
- `dev-docs/GwzDevCodeM2a-ReviewF5-6.md`
- `dev-docs/GwzDevCodeM2a-Review56-6.md`
- `gwz-core/dev-docs/GWZRequirements.md`
- `gwz-core/dev-docs/GwzMergeDesign.md`
- `gwz-core/dev-docs/GwzMergePlan.md`

## 1. Gate decision

The third remediation is required before M2b-A2, M2b-C, or shared M2b
integration resumes.

Reviewer 56 passed the submitted snapshot with three P3 proof/documentation
gaps. Reviewer F5 independently reproduced two defects outside the existing
green matrix:

1. a P1 recovery path can discard an exact durable conflict-resolution intent,
   freeze later staged content, and commit that replacement; and
2. a P2 lifecycle path can publish terminal native completion for a request
   rejected before the core merge emitter exists, with no operation start or
   finish event.

The green baseline of 649 Rust tests (plus 1 ignored) and 286 Python tests does
not invalidate either reproduction. Both defects violate explicit RemPlan-2
exit criteria and are accepted as gate blockers. Where the reviews disagree,
the concrete real-Git/native reproductions and the stricter fail-closed
contract govern this plan.

## 2. Finding dispositions

| Finding | Disposition | Required correction |
| --- | --- | --- |
| F5-3 P1: not-started recovery replaces durable tree/signatures | **Fix first; P1** | Preserve the pending action, validate live state against its exact durable specification, and execute that specification without re-preparing it. Any mismatch is ambiguous and zero-mutation. |
| F5-3 P2: pre-emitter failure completes without lifecycle events | **Fix before native completion publication; P2** | Create one non-fallible outer event owner before context conversion, request gating, validation, and dispatch. It emits exactly one start and finish event before native terminal completion. |
| F5-3 P3 / 56-3 P3-1: design contradicts frozen signatures | **Fix with code** | Remove the obsolete commit-time identity rule and state the durable frozen-signature rule once, consistently. |
| 56-3 P3-2: open-start driver/state matrix is incomplete | **Close as regression hardening** | Add a table-driven state × dry-run/real × surface/effective-root matrix without duplicating the core policy implementation. |
| 56-3 P3-3: native failure/waiter/cancellation matrix is incomplete | **Close around the P2 owner change** | Add focused synchronous/submitted failure rows, multiple waiters, and cancel-then-resubscribe coverage. Use test-only fault injection where a real failure cannot be induced deterministically. |
| 56-3 residual: production-visible event-delay environment hook | **Remove from normal builds if the owned test path permits; otherwise record for M2b-C** | Prefer a test-feature or injected barrier. This is not a blocker for the two accepted defects and must not expand the production protocol. |

The previously deferred late-drift resting-state correction remains an M2b-A2
acceptance item. This remediation must neither implement it opportunistically
nor remove it from the main merge plan.

## 3. Scope and invariants

This remediation is limited to:

- exact recovery of already durable participant actions;
- lifecycle-event ownership for every accepted merge invocation;
- focused core/native/driver regression coverage;
- correction of the authoritative merge documentation and plan status; and
- a fresh independent re-review gate.

It does not implement M2b publication/finalization, M2c root participation, M3
preservation/history/GC, new merge strategies, or a general background-job
framework.

The following existing invariants remain non-negotiable:

- status is read-only and never clears, rewrites, or advances an operation;
- the accepted manifest and lock remain the last complete baseline while a
  merge is open;
- continue and abort preflight the complete operation before the first Git
  mutation;
- exact action intent precedes mutation and exact outcome precedes
  `MemberFinished`;
- no staged or committed post-intent user content is silently adopted as
  GWZ-owned;
- an ambiguous pending action makes both continue and abort ineligible;
- checked Git actions retain their ref-lock/revalidation boundaries;
- terminal native completion is published exactly once, after the final event;
- original structured model errors retain code, message, member id/path,
  target kind, and detail; and
- no protocol enum value or existing wire field is renumbered or repurposed.

## 4. Lead-owned interface checkpoint R3-0

Freeze and compile the following interfaces before parallel implementation
begins. The checkpoint contains additive seams and focused contract tests, not
the completed remedies.

### 4.1 Exact resolved-index validation

Add one read-only `GitBackend` primitive for validating an in-progress resolved
merge against an already frozen `GitPreparedCommit`. Its contract must cover:

- attached target branch and exact `before` ref;
- exact native `MERGE_HEAD` and no foreign repository operation;
- fully resolved index with no unstaged or untracked work;
- exact equality between the current index entries/modes and the recorded tree;
  and
- no ref, HEAD, index, worktree, native-state, operation-record, or accepted
  metadata mutation.

The real backend should compare the index with the already recorded tree
without creating a new tree as an observation side effect. A missing recorded
tree is a typed recovery error. The default trait implementation remains typed
`UnsupportedOperation` for non-upgraded backends.

The tentative interface is:

```text
validate_prepared_merge_resolution_state(
    repo,
    expected_before,
    expected_merge_head,
    prepared_commit,
) -> Result
```

The final name may follow existing backend conventions, but status and checked
execution must consume the same definition of a valid resolved state.

### 4.2 Durable prepared-action decoder

Create one internal decoder from `PendingMergeAction` to the exact executable
prepared value:

```text
DurablePreparedAction =
    Unchanged
  | FastForward
  | ExpectedConflict
  | TrueMerge(GitPreparedCommit)
  | Resolution(GitPreparedCommit)
```

It validates the kind/result/spec matrix and reconstructs complete author and
committer signatures without consulting current request attribution,
repository configuration, or the clock. Status, continue, and tests must use
this shared decoder rather than maintain separate weaker interpretations.

Old commit-producing records without the complete tree/signature
specification remain ambiguous. Legacy non-commit up-to-date and fast-forward
records keep their current conservative compatibility.

### 4.3 One outer merge invocation event owner

Add a non-fallible event-emitter construction path using only the request's
protocol-safe envelope fields: operation id, request id, and the supplied
protocol attribution payload. Constructing this emitter must not validate or
accept an invalid Git identity.

One outer merge invocation wrapper must own this sequence:

```text
construct event owner
emit OperationStarted
convert/validate OperationContext
apply effective-workspace open-operation gate
validate request and dispatch
emit OperationFinished
return success or original ModelError
native recorder publishes terminal result
```

`OperationFinished` is emitted exactly once on every return path. Inner merge
handlers must not create a second lifecycle owner. Requests rejected before
decoding, current-directory resolution, or native operation registration do not
have an accepted operation id and remain outside this contract.

## 5. Work packages

After R3-0 is frozen, R3-A and R3-B may proceed in parallel. R3-C follows the
event interface from R3-B. R3-D may proceed alongside the code after the exact
wording and named tests are known.

### R3-A — Exact pending-action recovery

Owned paths:

- `gwz-core/src/git/gitbackend.rs` and focused Git tests;
- `gwz-core/src/workspace_ops/merge/status.rs`;
- `gwz-core/src/workspace_ops/merge/continue_op.rs`;
- shared pending-action decoder in the lead-allocated merge module; and
- focused real-Git recovery tests.

Required behavior:

1. Reconciliation must not clear a `NotStarted` pending action.
2. A pending resolution is `NotStarted` only when the native merge, resolved
   index tree, worktree, branch/ref, and recorded objects exactly match the
   durable prepared specification.
3. A changed staged tree, changed index mode, unresolved entry, unstaged file,
   untracked file, foreign native state, missing object, or ref/head difference
   is `PendingActionAmbiguous`; continue and abort are both false.
4. Continue constructs its action directly from the durable pending action.
   It must not call either preparation method again for that action.
5. A clean true-merge pending action at the exact before state is executed with
   its original recorded result class, tree, author, and committer. The checked
   backend recomputes and compares live merge content under its existing ref
   lock before mutation.
6. Existing non-commit pending actions are likewise consumed rather than
   cleared and recreated, so the retry model has one rule.
7. Fresh participant work still prepares first, persists the pending action,
   verifies that write, and then executes it.
8. `pending_action` is cleared only by persisting a verified completed,
   expected-conflict, or failed outcome. It is never cleared merely because Git
   has not started.
9. Reconciliation must not emit an unmatched `MemberStarted`. Durable adopted
   outcomes retain the existing write-before-finish event order; an exact
   not-started action is bracketed by the later execution events.

Required focused tests:

- `pending_resolution_tree_change_is_ambiguous_without_mutation`: persist tree
  A, stage tree B, run status/continue/abort, and compare record, ref, index,
  worktree, native state, and accepted metadata before/after;
- `pending_resolution_exact_retry_uses_frozen_signatures`: retain tree A,
  change repository identity and clock inputs, and prove the resulting commit
  uses the original durable author and committer signatures;
- `pending_true_merge_not_started_executes_frozen_spec`: crash after intent but
  before Git, change identity/time, retry, and prove the original exact commit
  is created without rewriting intent;
- exact negative rows for index mode, unstaged/untracked content, missing tree,
  native state, ref, and source-object drift;
- exact controls for unchanged, fast-forward, expected conflict, clean true
  merge, and resolution; and
- a spy/fault test proving no preparation method is called for an existing
  durable action and no record write occurs before an ambiguous rejection.

### R3-B — Core lifecycle-event ownership

Owned paths:

- `gwz-core/src/operation/eventemitter.rs` and `push_event.rs`;
- `gwz-core/src/workspace_ops/merge/mod.rs`; and
- focused core lifecycle tests.

Required behavior:

1. Create the emitter and send `OperationStarted` before fallible attribution
   conversion, the merge-start gate, request validation, root/recovery
   resolution, or dispatch.
2. Send `OperationFinished` after the last inner event and before returning the
   result or original `ModelError` to the caller.
3. Preserve one monotonically increasing sequence across the whole invocation.
4. Do not synthesize a success response for failures and do not flatten the
   model error into event text.
5. Keep the lifecycle pair on direct core, Rust CLI, native Python, Python
   client, and Python CLI paths by fixing the shared core boundary rather than
   adding driver-specific substitutes.
6. Prove that direct dependency-injected handlers use the same outer owner or
   are explicitly internal dispatch functions that cannot be called as a
   public invocation without it.

Required focused tests:

- invalid attribution emits exactly start/finish and returns the original
  `InvalidRequest`;
- each open state (`awaiting_resolution`, `halted`, `recovery_required`, and
  `finalizing`) rejects dry-run and real start with start/finish and
  `OpenOperation`;
- validation, recovery-root, backend, and injected store errors end with one
  finish event;
- success and member-scoped failure preserve existing intermediate events and
  finish last; and
- no return path emits either lifecycle event twice.

### R3-C — Native completion and public-surface matrices

Owned paths:

- `gwz-py/native/src/dispatch/merge.rs`;
- `gwz-py/native/src/operations.rs` only where needed for tests/ordering;
- Python native/client/CLI merge tests; and
- Rust CLI merge gate tests.

Required behavior and evidence:

1. Native completion remains owned exactly once by `run`: core returns only
   after the finish event is stored, then `finish_merge` or
   `finish_model_error` atomically publishes the terminal result.
2. Test synchronous and submitted invalid-attribution failure and dry-run/real
   open-operation rejection. Assert start/finish, typed error, completion, and
   finish-before-complete ordering.
3. Start multiple event and result waiters before failure; every waiter must
   wake and observe the same first result and complete event history.
4. Cancel/close one event iterator, then resubscribe and obtain the intact
   operation result and full buffered events. Subscriber lifetime must not own
   the operation.
5. Deterministically exercise backend and post-recorder store failure through a
   test-only seam, without adding a production protocol option.
6. Cover all four open states through direct core. Across public drivers, use a
   table-driven matrix that covers both dry-run and real start, explicit root
   from an unrelated current directory, and every Rust CLI/Python
   client/Python CLI/native surface. Avoid an unnecessarily multiplicative
   fixture by reusing each frozen open record across read-only rejection rows.
7. Compare workspace bytes before and after every gate rejection.
8. Re-run the existing eight-scenario actual Rust/Python JSONL semantic parity
   test and retain exactly one terminal response/error after all events.

If the production-visible `GWZ_PY_TEST_EVENT_DELAY_MS` hook can be replaced
within these owned paths, gate it behind an explicit test-only build feature or
an injected native barrier. Release builds must ignore it. If that cannot be
done without changing distribution behavior, document the deferral to M2b-C
and do not let it delay the P1/P2 fixes.

### R3-D — Documentation, plan state, and review handoff

Owned paths:

- `gwz-core/dev-docs/GwzMergeDesign.md`;
- `gwz-core/dev-docs/GWZRequirements.md` only if clarification is needed;
- `gwz-core/dev-docs/GwzMergePlan.md`;
- both remediation plans' execution-status notes; and
- a new third-remediation re-review prompt.

Required updates:

1. Replace the obsolete rule that identity comes from the repository at commit
   time. State that request-or-repository identity is resolved before durable
   intent and the complete author/committer signatures are frozen for immediate
   and recovered execution.
2. State explicitly that `NotStarted` retains and consumes the existing durable
   specification; it does not authorize re-preparation.
3. State that every accepted merge invocation has one lifecycle owner covering
   context conversion and open-operation gating as well as dispatch.
4. Keep the M2b-A2 late-drift item visible.
5. Mark this remediation implemented only after the full local gate passes.
   Keep the M2a checkpoint blocked until the independent re-review criterion
   below passes.
6. Update exact Rust/Python counts and list the new named adversarial tests.

As a documentation consistency gate, fail if the authoritative design still
contains the obsolete commit-time identity statement or gives status/continue
different rules for a durable pending action.

## 6. Integration order

1. Commit or otherwise freeze R3-0 interfaces and their contract tests.
2. Implement R3-A and R3-B independently from that checkpoint.
3. Integrate R3-A and run all core recovery/status/continue/abort tests.
4. Integrate R3-B and verify exact event order in direct core tests.
5. Implement/integrate R3-C against the frozen outer lifecycle owner.
6. Integrate R3-D and regenerate only artifacts affected by actual source
   changes.
7. Run the focused adversarial matrices first, then the complete gate.
8. Inspect the unified `gwz diff`; use `gwz add` and `gwz commit` only when the
   user separately requests submission.
9. Obtain two fresh independent re-reviews of the final integrated snapshot.
10. Only when both contain no P0/P1/P2 finding, mark the M2a remediation
    checkpoint complete and begin M2b-A1/M2b-B integration work.

## 7. Verification gate

Run from `/Users/owebeeone/limbo/gwz-dev` unless noted:

```text
cargo fmt --all -- --check
cargo test --workspace --all-targets --no-fail-fast
cargo clippy --workspace --all-targets --all-features -- -D warnings

cargo fmt --manifest-path gwz-py/Cargo.toml -- --check
cargo clippy --manifest-path gwz-py/Cargo.toml \
  --all-targets --all-features -- -D warnings
gwz-py/.venv/bin/python -m maturin develop \
  --manifest-path gwz-py/Cargo.toml

cd gwz-py
.venv/bin/python scripts/regen_protocol.py --check
.venv/bin/python -m pytest -q
cd ..

cd gwz-cli
python3 scripts/generate_cli_reference.py --check
uvx --from 'mkdocs-material>=9.5,<10' mkdocs build --strict \
  --site-dir /tmp/gwz-m2a-remediation-3-docs
cd ..

bazel build //gwz-core:gwz_core //gwz-cli:gwz

gwz status
gwz diff --no-pager
git diff --check
git -C gwz-core diff --check
git -C gwz-cli diff --check
git -C gwz-py diff --check
```

Use GWZ for workspace status, staging, and commits. The direct `git diff
--check` calls above are read-only whitespace validation for the repositories
that own changed files; they are not a substitute for GWZ workspace operations.

Record exact Rust and Python test counts. Passing only the pre-existing suite
is insufficient: every named R3-A/R3-B/R3-C row must be green and the F5 live
reproductions must be converted into permanent tests.

### 7.1 Local execution evidence

The integrated working tree passed the full local gate on 2026-07-23:

- the Rust workspace suite passed **666 tests**, with **0 failed** and **1
  ignored**;
- the Python suite passed **314 tests**, with **0 failed**;
- both Rust format checks and both strict Clippy invocations passed;
- the native extension rebuilt successfully with `maturin develop`;
- generated Python protocol and Rust CLI-reference checks passed;
- the strict MkDocs build passed;
- the Bazel `gwz_core` and `gwz` targets built successfully; and
- all root/member `git diff --check` invocations passed.

Permanent adversarial evidence includes:

- `pending_resolution_tree_change_is_ambiguous_without_mutation`;
- `pending_true_merge_not_started_executes_frozen_spec`;
- `invalid_durable_true_merge_evidence_is_ambiguous_and_blocks_recovery`;
- `checked_resolution_invalid_evidence_rejects_without_mutation`;
- `pending_resolution_missing_tree_race_rejects_without_mutation`;
- `pending_resolution_same_commit_branch_switch_race_rejects_without_mutation`;
- `pending_resolution_exact_retry_uses_frozen_signatures`;
- `durable_resolution_race_preserves_pending_intent_without_failed_outcome`;
- `invalid_attribution_is_bracketed_by_one_outer_lifecycle`;
- `every_open_merge_state_and_start_mode_is_bracketed_by_one_outer_lifecycle`;
- `successful_member_events_share_the_outer_sequence_and_finish_last`;
- `member_scoped_failure_keeps_intermediate_events_and_finishes_last_once`;
- `test_open_operation_failure_completes_after_one_lifecycle_without_mutation`;
- `test_open_merge_gate_public_surface_matrix_from_unrelated_cwd`;
- `test_backend_and_store_failures_complete_with_structured_errors_without_mutation`;
- `test_submitted_failure_wakes_multiple_event_and_result_waiters`; and
- `test_closing_one_event_iterator_does_not_own_submitted_merge`.

The missing/malformed-evidence regression covers an absent tree, malformed
tree OID, invalid author, invalid committer, and invalid timezone, and asserts
status, direct checked execution, continue, and abort are zero-mutation and
perform no re-preparation. The public-surface matrix covers all four open
states in dry-run and real modes through Rust CLI, Python client, and Python
CLI from an unrelated current directory with an explicit root. Native
backend/store failures cover synchronous and submitted execution. The retained
eight-scenario Rust/Python JSONL parity matrix also passes.

The production-visible `GWZ_PY_TEST_EVENT_DELAY_MS` hook is unchanged and
remains an explicit M2b-C cleanup item: the documented native test build does
not enable a test-only Cargo feature, so gating the hook now would remove the
live-streaming test coverage. Direct `cargo test` of the macOS PyO3 unit
harness also retains its existing undefined-Python-symbol linker limitation;
strict native Clippy plus the rebuilt-extension Python tests cover the changed
native behavior.

### 7.2 First third-remediation re-review disposition

The first two independent third-remediation reviews both confirmed that the
original tree-A/tree-B replacement and pre-emitter lifecycle defects are
closed, but both reproduced the same remaining P2: a clean not-started
true-merge action did not validate the recorded tree object or frozen
signatures before status made it eligible, and continue could recreate a
missing tree object.

The corrective pass therefore adds one shared read-only complete-specification
validator for clean true-merge recovery and uses it in both status and checked
execution before any object-writing primitive. It also closes the reviewers'
agreed P3 proof gaps by completing the public-driver open-state matrix,
covering native backend/store failures in synchronous and submitted modes, and
clarifying the retry-time identity rule in the authoritative design. The two
`*-4.md` reports are evidence for this failed review snapshot; two new passing
reviews of the corrected snapshot are still required.

### 7.3 First correction re-review disposition

The first correction closed the clean true-merge missing-tree defect and all
agreed proof/documentation gaps. The F5 `*-5.md` review then reproduced its
resolution analogue: the read-only status validator rejected a missing frozen
resolution tree, but `commit_prepared_merge_resolution_checked` wrote the
current index tree before comparing its object ID and could therefore recreate
the evidence and advance HEAD.

The second corrective pass now makes public validation and checked resolution
execution consume one shared in-repository read-only validator under the
durable target-branch ref lock. Preparation, status, and execution all require
the exact attached target branch. Checked execution commits from the already
recorded tree and does not call `index.write_tree`. Direct malformed-evidence,
post-preflight missing-tree, and same-commit branch-switch regressions pass,
along with the complete local gate. At that checkpoint, two fresh passing
independent reviews remained required.

### 7.4 Final independent review gate

The final branch-safe snapshot passed two fresh independent reviews:

- `GwzDevCodeM2a-Review56-6.md`: **PASS**, with zero P0/P1/P2/P3 findings;
- `GwzDevCodeM2a-ReviewF5-6.md`: **PASS**, with zero P0/P1/P2/P3 findings.

Both reviewers independently exercised clean true-merge and
conflict-resolution missing-tree recovery, confirmed fail-closed zero-mutation
behavior, and passed the complete gate at 666 Rust test executions (plus 1
ignored) and 314 Python tests. Neither reviewer read the other's `*-6.md`
report before completing their own. All remediation exit criteria are
satisfied, so M2b-A2, M2b-C, and shared M2b integration may resume.

## 8. Exit criteria and M2b unblock

This remediation is complete only when:

- an existing durable pending action is never cleared or re-prepared merely
  because Git has not started;
- changed post-intent staged content is ambiguous, blocks continue and abort,
  and produces zero Git/record/accepted-metadata mutation;
- exact not-started true-merge and resolution retries consume the original
  frozen tree and signatures regardless of later identity/time changes;
- status and checked execution share one exact resolved-index definition;
- every accepted merge invocation emits exactly one `OperationStarted` and one
  final `OperationFinished`, including context and open-gate failures;
- native completion becomes visible only after the finish event and retains
  the original structured result/error exactly once;
- multiple waiters and cancelled subscribers cannot lose, replace, or cancel
  the operation result or buffered events;
- the open-state/public-surface matrix and existing JSONL parity matrix pass
  without workspace mutation or output regression;
- the authoritative requirements, design, and plan contain no contradictory
  signature or pending-action rules;
- the full verification gate passes with generated artifacts current;
- exact test counts and named evidence are recorded in this plan; and
- two fresh independent re-reviews of the final integrated snapshot report no
  P0/P1/P2 defect in the corrected M1/M2a contracts.

Only then may `GwzMergePlan.md` mark the M2a remediation checkpoint complete
and unblock M2b-A2, M2b-C, and shared M2b integration. M2b-A1 marker-model work
and isolated M2b-B scoped-root-commit experiments may remain separate, but
must not integrate against the affected recovery or event interfaces before
this gate closes.
