# GWZ Merge M1/M2a Second Code Review Remediation Plan

Date: 2026-07-19

Execution status: **implementation and local verification complete; independent
re-review pending** (2026-07-19). R2A through R2E are integrated against the
frozen interfaces. M2b-A2 finalization, M2b-C driver/event completion, and
shared M2b integration remain blocked until the final independent-review exit
criterion in section 9 passes.

Inputs:

- `dev-docs/GwzDevCodeM2a-RemPlan.md`
- `dev-docs/GwzDevCodeM2a-Review56-2.md`
- `dev-docs/GwzDevCodeM2a-ReviewF5-2.md`

## 1. Assessment and gate decision

The first remediation is materially better, but its independent-review gate
is not closed. F5-2 reports no remaining P0/P1/P2 defect; Reviewer 56 reports
one P1 and three P2 defects against the same working-tree snapshot. The latter
findings are confirmed by the current implementation:

1. commit-producing pending actions freeze parents and message but not the
   expected tree, so a different tree can be adopted as `completed_exactly`;
2. core does not apply the open-operation gate to a dry-run merge start;
3. a failed synchronous Python merge emits its finish event but never marks
   the native operation record complete; and
4. Python `merge --jsonl` uses the synchronous response path and emits no live
   merge events.

The dry-run defect was also found by F5-2 as P3-2. This plan accepts Reviewer
56's P2 classification because the central table deliberately classifies all
merge starts as blocked, public core entry points are authoritative, and the
Rust/Python parity contract has no dry-run exception.

These are focused corrections to exact action recovery, one gate call site,
and the Python operation lifecycle. They do not require a redesign of the
merge operation record, status classifier, coordinated abort algorithm, or
event vocabulary delivered by the first remediation.

M2b-A1 marker-model work and isolated M2b-B scoped-root-commit experiments may
continue away from the shared integration path. Nothing may integrate against
the affected recovery or Python event interfaces until this plan passes.

## 2. Finding dispositions

| Re-review finding | Disposition | Required correction |
| --- | --- | --- |
| 56-2 P1: same-parent/same-message commit with a different tree is adopted | Fix first | Freeze the exact content and commit identity for every commit-producing pending action; require it during execution and reconciliation. |
| 56-2 P2 / F5-2 P3-2: dry-run merge start bypasses the core open-operation gate | Fix before M2b | Route every merge start, including dry-run, through the existing guarded workspace-root helper and pin one cross-driver policy. |
| 56-2 P2: failed synchronous Python merge never completes its operation record | Fix before streaming work | Give one owner responsibility for terminal recorder completion and preserve the original structured model error. |
| 56-2 P2: Python `merge --jsonl` emits only a response | Fix in this remediation | Add a submitted merge path, live event subscription, typed final-response lookup, and actual CLI JSONL streaming. |
| F5-2 P3-1: late drift leaves a resting operation labelled `executing` | Defer to M2b-A2 | It is re-enterable and non-destructive. Finalization must decide the truthful resting state when all actions are durable but publication is blocked by late drift. Add it to the M2b-A2 acceptance matrix rather than changing lifecycle policy here. |

F5-2's remaining cosmetic observations are recorded residuals, not exit-gate
defects. They may be addressed only when a required change already owns the
same path and the change does not broaden this remediation.

## 3. Scope and non-negotiable invariants

This remediation must preserve the first remediation's passing contracts:

- the accepted manifest and lock remain frozen while an operation is open;
- status remains read-only;
- continue and abort preflight the whole operation before mutation;
- ambiguous pending actions make both mutation eligibility flags false;
- coordinated abort never discards post-merge user content;
- the authoritative mutation gate uses the request's effective workspace;
- durable outcome and state events follow their verified record writes; and
- no existing protocol enum value is renumbered.

This plan does not implement M2b publication/finalization, M2c `@root`
execution, M3 preservation/history/GC, additional merge strategies, or a
general-purpose background-job framework. The Python native changes are the
smallest reusable operation-completion surface needed to stream merge safely.

No implementation commit, root composition update, or push is part of this
plan unless the user requests it separately.

## 4. Lead-owned interface checkpoint

The following decisions must be implemented as compiling interfaces and
focused contract tests before parallel work begins. Agents must not invent
different record, gate, or completion semantics in their individual lanes.

### 4.1 Exact commit-producing action intent

Extend the internal `PendingMergeAction` with an additive `expected_result`
discriminator (`unchanged`, `fast_forward`, `expected_conflict`, or `commit`)
and an optional commit specification. The commit specification is mandatory
when `expected_result` is `commit`, forbidden for the other result classes, and
used by clean `true_merge` and `resolve_conflict` actions. It freezes:

- the exact expected tree OID;
- the ordered first and second parent OIDs already present in the action;
- the byte-exact message already present in the action; and
- the fully resolved Git author and committer signatures: name, email, time,
  and timezone offset.

The resolved signatures, rather than only the request's partial attribution,
are stored because repository identity and implicit timestamps can change
between intent persistence and crash recovery. The checked Git primitive must
use the frozen signatures, not resolve them again after the intent write.

Preparation must occur before the pending-action record is written, without
moving a ref, changing the repository index or worktree, or entering native
merge state:

- for a clean divergent merge, perform an in-memory tree merge and freeze the
  resulting tree and signatures;
- for a divergent merge predicted to conflict, set `expected_result` to
  `expected_conflict` and do not attach a commit specification;
- for a resolved native merge, validate the exact native merge and resolved
  index, write or calculate its tree object, and freeze that tree and the
  signatures; and
- up-to-date and fast-forward actions record `unchanged` or `fast_forward`,
  continue to use their exact existing commit identities, and do not carry a
  commit specification.

Writing an unattached tree object while preparing the intent is permitted; it
must not alter refs, HEAD, the on-disk index, the worktree, or repository
operation state. The expected tree OID remains valid evidence even if that
unreferenced object is later pruned because the candidate commit exposes its
own tree OID.

Execution must revalidate the complete prepared specification under the
existing checked ref boundary before moving the target ref:

- a clean true merge must produce the prepared tree;
- a resolution commit must still have the prepared index tree;
- author, committer, ordered parents, and message must match the frozen values;
  and
- a mismatch fails without moving the branch ref and leaves the pending intent
  available for recovery.

Post-crash reconciliation may classify a commit-producing action as
`completed_exactly` only when branch, target ref, repository state, clean
index/worktree, ordered parents, source, message, tree, author, and committer
all match the durable intent. Any mismatch is
`pending_action_ambiguous`, with continue and abort both ineligible.

An older pending `true_merge` or `resolve_conflict` record without the new
result discriminator and required commit specification is not exact enough to
adopt or roll back. It must be reported as ambiguous with recovery guidance.
Up-to-date and fast-forward records remain classifiable from their existing
kind and commit evidence. Unknown fields continue to round-trip unchanged.

This is an internal durable-record addition. It does not require a public taut
protocol field or enum change.

### 4.2 One merge-start gate policy

All `MergeOp::Start` requests use `guarded_workspace_root`, passing the actual
dry-run flag:

- a real start retains the workspace mutator guard through the operation;
- a dry-run performs the same effective-root discovery and open-merge gate
  check without taking the mutator lock; and
- status, continue, abort, and GC retain their existing recovery discovery.

While a merge is open, both dry-run and real merge starts return the typed
`OpenOperation` error in Rust CLI, Python client/CLI, and direct core calls.
The Rust pre-dispatch check remains an optional early diagnostic, not the
authority.

### 4.3 Exactly-once native operation completion

Refactor native merge dispatch so one completion owner receives the decoded
request metadata, operation recorder, event sink, core result, and final typed
response. It must publish exactly one terminal result:

- success stores the complete `MergeResponse` and a successful
  `OperationResult`;
- failure stores a failed `OperationResult` constructed from the original
  `ModelError`, preserving code, message, member id/path, target kind, and
  detail; and
- a second completion attempt is rejected or is a verified identical no-op;
  it must never overwrite the first result.

The completion flag observed by `wait_events` is set only after:

1. all core events, including `OperationFinished`, have been delivered; and
2. on success, the final typed merge response is available for lookup.

This ordering prevents an event subscriber from observing `complete=true` and
racing a missing final response. A failed operation has no successful merge
response; consumers obtain its structured failure from `OperationResult`.

Synchronous `Client.merge` and submitted merge execution must call the same
core runner and the same terminal-completion function. They must not create
nested recorders for one operation id.

### 4.4 Submitted merge and Python streaming surface

Add merge to the native submitted-operation dispatcher. The submission returns
an accepted `MergeResponse` containing the deterministic operation id, starts
one background execution, and makes these operations available:

- subscribe to events from a sequence cursor;
- await the generic terminal `OperationResult`; and
- retrieve the stored successful final `MergeResponse`.

Do not extend the generated `OperationResult` protocol solely to carry the
merge-specific response. Retain the typed final response in the native
operation store and expose a narrow response lookup through the bridge.

Expose `MergeOperationHandle` from
`Client.merge_stream(...) -> MergeOperationHandle` with these semantics:

```text
handle = await client.merge_stream(...)
async for event in handle.events():
    ...
response = await handle.result()
```

The handle exposes its `operation_id`; `events()` returns the existing
sequence-aware async event iterator; and `result()` returns the complete
successful `MergeResponse`. On failure, `result()` raises from the structured
terminal `OperationResult`. The native bridge exposes the successful response
as `merge_operation_response(operation_id)`. Cancellation of a subscriber does
not cancel or corrupt the merge operation.

Python CLI `merge --jsonl` must use this surface and write records to stdout as
they become available:

```text
event
event
...
response
```

Each event uses `operation_event_json`; the final line uses the same complete
response serializer as `--json`. On failure, already emitted events remain,
followed by exactly one structured machine error response. Stdout contains no
non-JSON text. Human and `--json` modes remain synchronous and unchanged.

## 5. Implementation lanes

After section 4 compiles, R2A, R2B, and R2C may run in parallel. R2D depends on
R2C. Shared-file edits not listed below remain lead-owned.

### R2A — Exact tree and commit reconciliation

Owner: core recovery/backend agent. Suggested budget: at most 550 handwritten
changed lines, excluding tests.

Primary ownership:

- `gwz-core/src/workspace_ops/merge/model.rs` pending commit specification;
- `gwz-core/src/git/gitbackend.rs` preparation, checked execution, and exact
  matcher primitives;
- `gwz-core/src/workspace_ops/merge/start.rs` intent preparation/write order;
- `gwz-core/src/workspace_ops/merge/continue_op.rs` resolution/retry intent;
- `gwz-core/src/workspace_ops/merge/status.rs` reconciliation; and
- focused backend, store-fault, status, continue, and abort tests.

Tasks:

1. Add the additive internal result discriminator, commit specification, and
   unknown-field tests.
2. Prepare clean-merge or conflict intent without changing observable
   repository state.
3. Prepare the resolved-index tree and exact signatures before persisting a
   resolution intent.
4. Pass the prepared specification into checked execution and verify it before
   ref publication.
5. Replace the parent/message-only matcher with the complete matcher.
6. Make missing commit specifications conservative for old commit-producing
   pending records.
7. Prove continue and abort never adopt or roll back a different-tree commit.

The implementation must not clear a pending intent merely because execution
failed after the intent write. Recovery owns classification of that boundary.

### R2B — Dry-run merge-start gate parity

Owner: core gate agent. Suggested budget: at most 100 handwritten changed
lines, excluding tests.

Primary ownership:

- `gwz-core/src/workspace_ops/merge/mod.rs` start-root/guard selection;
- focused open-gate tests in core; and
- Rust/Python driver parity tests limited to this behavior.

Tasks:

1. Route dry-run and real start through the frozen section 4.2 helper.
2. Preserve no-lock/no-mutation behavior for dry-run.
3. Test explicit request roots from an unrelated cwd.
4. Assert all rejected forms leave record bytes, refs, index, worktree,
   manifest, and lock unchanged.

Do not change the central gate table or weaken the Rust diagnostic to make the
drivers agree.

### R2C — Native failure completion and response retention

Owner: Python native bridge agent. Suggested budget: at most 300 handwritten
changed lines, excluding tests.

Primary ownership:

- `gwz-py/native/src/operations.rs` terminal completion state;
- `gwz-py/native/src/shims.rs` shared execution wrapper;
- `gwz-py/native/src/dispatch/merge.rs` single merge runner;
- merge submission wiring in `gwz-py/native/src/dispatch/mod.rs`; and
- native bridge exports in `gwz-py/native/src/lib.rs`.

Tasks:

1. Make operation completion atomic, exactly once, and first-result preserving.
2. Convert `ModelError` directly to the generated structured `GwzError`.
3. Store the successful typed merge response before publishing completion.
4. Use one recorder for synchronous and submitted execution.
5. Ensure both success and failure wake every waiting event/result consumer.
6. Add native tests for duplicate completion, failure completion, response
   publication ordering, and multiple concurrent readers.

Do not flatten an error to `err.to_string()` before terminal result creation.

### R2D — Python client and actual JSONL streaming

Owner: Python client/CLI agent after R2C. Suggested budget: at most 350
handwritten changed lines, excluding tests.

Primary ownership:

- `gwz-py/src/gwz/bridge.py` final merge-response lookup;
- `gwz-py/src/gwz/client.py` merge stream handle/result API;
- `gwz-py/src/gwz/cli_merge.py` JSONL execution path;
- the narrow dispatch/result handling needed in `gwz-py/src/gwz/cli.py`;
- `gwz-py/src/gwz/cli_render.py` only if parity requires a serializer fix; and
- Python unit, native, and subprocess CLI tests.

Tasks:

1. Submit merge and obtain its operation id without a call/subscribe race.
2. Stream each event once in sequence order.
3. Await the terminal result and retrieve the successful full response.
4. Print exactly one final response or structured error after the events.
5. Keep `--json` a single non-streaming response.
6. Prove output is genuinely live rather than buffered until completion.
7. Compare Python and Rust JSONL semantic record sequences.

### R2E — Lead integration, documentation, and deferred-P3 handoff

Owner: lead after R2A-R2D integrate. Suggested budget: at most 150
handwritten changed lines, excluding documentation and fixtures.

Tasks:

1. Reconcile shared call sites without weakening the frozen interfaces.
2. Update `GWZRequirements.md` and `GwzMergeDesign.md` so exact completed
   actions explicitly include tree and frozen signature identity.
3. Update `GwzMergePlan.md` and this plan with verified status and counts.
4. Correct `MachineOutput.md` only where needed to describe the actual event,
   final-response, and failure ordering implemented by both drivers.
5. Add F5-2 P3-1 to the M2b-A2 finalization acceptance matrix.
6. Run the combined adversarial scenario and the full gate.

## 6. Integration order and change control

1. Land the section 4 additive interfaces and contract tests locally.
2. Run R2A, R2B, and R2C from that exact checkpoint.
3. Integrate R2A and R2B, then rerun core recovery and gate tests.
4. Integrate R2C and verify synchronous success/failure completion before R2D.
5. Run R2D against the integrated native completion surface.
6. Integrate R2D, then complete R2E documentation and cross-driver tests.
7. Run the focused matrix in section 7 and the full gate in section 8.
8. Obtain a new independent re-review of the integrated snapshot.
9. Resume M2b-A2 and M2b-C only after section 9 is satisfied.

Stop parallel work and return the decision to the lead if a lane proposes:

- adopting a commit without an exact tree match;
- resolving author or committer identity again after intent persistence;
- treating a missing new commit specification as compatible exact evidence;
- allowing a driver-only open-operation check to be authoritative;
- marking an operation complete before its final event/result data is visible;
- buffering Python JSONL until the merge finishes;
- extending this fix into publication, root participation, preservation, GC,
  or strategy behavior; or
- renumbering an existing protocol enum value.

## 7. Required focused and adversarial matrix

### 7.1 Pending-action content identity

For both a clean true merge and a resolution commit:

1. persist the prepared pending intent;
2. stop before the participant outcome write;
3. create or move the target to a two-parent commit with the recorded parents
   and byte-identical message but one additional tracked file in its tree;
4. assert status reports `pending_action_ambiguous`;
5. assert continue and abort eligibility are both false; and
6. assert status, rejected continue, and rejected abort do not change the
   record, branch, index, worktree, or additional file.

Controls must prove that the exact prepared tree and signatures still reconcile
as `completed_exactly`, adopt durably, and remain safely abortable. Additional
negative cases vary author, committer, parent order, message bytes, target ref,
repository state, index, and worktree one at a time.

Preparation tests assert ref, HEAD, index checksum, worktree status, and native
repository state are unchanged before the pending intent is persisted.

### 7.2 Merge-start gate

Test open `awaiting_resolution`, `halted`, `recovery_required`, and `finalizing`
records. From both the workspace and an unrelated cwd using an explicit root,
assert dry-run and real starts return `OpenOperation` through:

- the direct core handler;
- the Rust CLI;
- Python `Client`;
- the Python CLI; and
- the native bridge call.

Every rejection must be zero-mutation. With no open operation, dry-run remains
read-only and real start retains the mutator lock through execution.

### 7.3 Native operation completion

With deterministic request ids, exercise:

- successful merge start and status;
- validation failure;
- second-member preflight failure;
- recovery-required or drift rejection;
- backend failure;
- store failure after event recorder creation; and
- submitted and synchronous forms of the same request.

For each failure, assert one operation start event, one operation finish event,
`wait_events(...).complete == true`, a present failed `OperationResult` with
the original typed/member-scoped error, and prompt termination of every event
iterator. For success, assert the final `MergeResponse` is retrievable whenever
completion is visible. Assert no path completes twice or overwrites its result.

### 7.4 Rust/Python JSONL parity

Run the actual CLIs against equivalent real workspaces for:

- dry-run start;
- clean start success;
- expected conflict;
- status;
- successful continue;
- recovery/drift rejection;
- coordinated abort; and
- top-level validation or preflight failure.

Parse every stdout line as one JSON object. Compare semantic record kinds,
event order, member identity/outcomes, merge-state transitions, artifact paths,
final response/error shape, aggregate status, exit code, and absence of
non-JSON stdout. Normalize only timestamps, generated operation/request ids,
and temporary filesystem paths.

At least one subprocess test must pause execution after an early event and
prove the Python CLI makes that event readable before the operation completes.
A one-line response from `--jsonl` must fail this test. `--json` must continue
to emit exactly one response and no event records.

## 8. Verification gate

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
  --site-dir /tmp/gwz-m2a-remediation-2-docs
cd ..

bazel build //gwz-core:gwz_core //gwz-cli:gwz

git diff --check
git -C gwz-core diff --check
git -C gwz-cli diff --check
git -C gwz-py diff --check
```

Record exact Rust and Python test counts. Passing only the pre-existing suite
is insufficient; every focused matrix row in section 7 must be named and green.

### 8.1 Local execution evidence

The integrated 2026-07-19 snapshot passes the complete section 8 gate:

- Rust workspace: **648 passed, 1 ignored**;
- Python: **286 passed**;
- Rust and native Python formatting and strict clippy: passed;
- protocol regeneration and CLI reference checks: passed;
- strict MkDocs build: passed;
- Bazel `gwz_core` and `gwz` targets: passed; and
- root, core, CLI, and Python `git diff --check`: passed.

The focused/adversarial rows are pinned by these named tests:

| Matrix | Named evidence |
| --- | --- |
| Exact prepared content and zero-mutation preparation | `prepared_clean_merge_freezes_exact_content_without_observable_mutation`; `prepared_conflict_prediction_does_not_enter_native_merge_state`; `checked_resolution_binds_parents_and_rejects_unsafe_index_states` |
| Exact/negative reconciliation and legacy records | `pending_true_merge_completion_is_exact_and_read_only`; `different_tree_or_signature_commit_is_ambiguous_and_status_is_read_only`; `resolution_candidate_with_different_tree_is_never_adopted_or_rollback_eligible`; `exact_merge_commit_matcher_checks_ordered_parents_and_message`; `old_commit_producing_pending_record_is_ambiguous_but_old_fast_forward_is_classifiable` |
| Open-operation gate states and driver surfaces | `open_awaiting_resolution_blocks_dry_run_and_real_starts_from_an_explicit_root`; `open_halted_blocks_dry_run_and_real_starts_from_an_explicit_root`; `open_recovery_required_blocks_dry_run_and_real_starts_from_an_explicit_root`; `open_finalizing_blocks_dry_run_and_real_starts_from_an_explicit_root`; Rust `merge_dry_run_alias_and_first_class_start_work_end_to_end`; Python `test_native_gate_uses_explicit_root_outside_cwd_and_stage_is_conditional` |
| Exactly-once native completion and structured failure | `test_submitted_merge_retains_response_before_completion_is_visible`; `test_failed_merge_completes_once_with_the_original_structured_error`; `test_duplicate_merge_operation_id_is_rejected_without_overwriting_result`; `test_submitted_preflight_failure_retains_member_context` |
| Actual Rust/Python JSONL parity | `test_actual_rust_and_python_merge_jsonl_are_semantically_equivalent`, parameterized for dry-run start, clean start, conflict, status, continue, recovery rejection, abort, and preflight failure |
| Live Python delivery and terminal ordering | `test_native_merge_jsonl_subprocess_flushes_before_completion`; `test_merge_jsonl_failure_ends_with_structured_terminal_error`; `test_merge_jsonl_streams_events_then_one_final_response` |

The local implementation and verification conditions below are satisfied. The
fresh independent re-review remains deliberately outstanding, so M2b shared
integration is still blocked.

## 9. Exit criteria and M2b unblock

The second remediation is complete only when:

- every commit-producing pending action durably freezes its exact tree and
  resolved author/committer signatures before the Git action;
- checked execution and post-crash reconciliation consume the same frozen
  specification;
- different-tree, different-signature, parent, message, ref, index, worktree,
  and repository-state negatives are ambiguous and zero-mutation;
- exact control commits still reconcile, adopt, continue, and abort safely;
- old commit-producing pending records without the new specification fail
  conservatively rather than being silently adopted;
- all core and driver merge starts, including dry-run, enforce one
  effective-workspace open-operation policy;
- every synchronous and submitted Python merge completes its native operation
  record exactly once on success and failure with structured errors intact;
- Python `merge --jsonl` emits live event records followed by exactly one final
  response or structured error and matches Rust semantically;
- human and `--json` behavior do not regress;
- the full section 8 gate and every section 7 test pass with generated
  artifacts clean; and
- a fresh independent re-review of the final integrated snapshot reports no
  P0/P1/P2 defect in the corrected M1/M2a contracts.

Only then may `GwzMergePlan.md` mark the M2a remediation checkpoint complete
and unblock M2b-A2 finalization, M2b-C driver/event completion, and shared M2b
integration. The deferred late-drift resting-state P3 remains an explicit
M2b-A2 acceptance item and does not disappear when this gate closes.
