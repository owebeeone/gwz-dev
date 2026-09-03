# GWZ M1/M2a Third-Remediation Re-review — F5

Date: 2026-07-23

Reviewer designation: **F5**

## Gate decision

**FAIL — one P2 exact-recovery defect remains, so M2b-A2, M2b-C, and shared
M2b integration remain blocked.**

The third remediation closes both defects accepted from F5-3 at their original
reproduced boundaries:

- an existing resolution intent for tree A is no longer discarded or replaced
  by later staged tree B; and
- invalid attribution and open-operation rejection now pass through one outer
  lifecycle owner and complete only after `OperationFinished`.

However, the same not-started true-merge path does not verify that the tree
object named by the durable commit specification still exists. Status reports
the damaged intent as exact, enables both continue and abort, and continue
recreates the missing object and completes the merge. This violates the
third-remediation requirement that a missing recorded object is ambiguous and
zero-mutation.

Severity counts:

- P0: 0
- P1: 0
- P2: 1
- P3: 2

## Reviewed snapshot and method

I reviewed the complete integrated uncommitted tree on these submitted heads:

| Repository | Reviewed base head |
| --- | --- |
| workspace root | `5080b441a8003cc39f38c21ce0354bdae37e9549` |
| `gwz-core` | `e152c205bb2e46e401306914e53d7b004f9a3108` |
| `gwz-cli` | `c335d972228e9e6b21658cc14938f756af6533bb` |
| `gwz-py` | `fae628acd91936d386101ee9266e6916ea93d9b7` |

The heads matched the review prompt. I read every required input, inspected the
workspace-wide diff and the untracked pending-action decoder, followed the
affected core, Rust CLI, native Python, Python client, and Python CLI callers,
ran the complete RemPlan-3 section 7 gate, and performed two independent
real-workspace adversarial reproductions. I did not read another
`GwzDevCodeM2a-Review*-4.md` report and used no subagents.

No implementation, test, generated artifact, or existing documentation was
changed. Temporary reproductions used system temporary directories. This report
is my only workspace write.

## Verified defect

### P2 — A missing durable true-merge tree is classified as not started and is recreated during continue

**Affected files and lines:**

- `gwz-core/src/workspace_ops/merge/status.rs:92-134`
- `gwz-core/src/workspace_ops/merge/status.rs:536-577`
- `gwz-core/src/workspace_ops/merge/pending.rs:13-57`
- `gwz-core/src/workspace_ops/merge/continue_op.rs:379-394`
- `gwz-core/src/git/gitbackend.rs:1392-1408`

**Violated requirement and plan criteria:**

- RemPlan-3 section 4.1 requires a missing recorded tree to be a typed recovery
  error and requires status and execution to share the exact durable-state
  definition.
- RemPlan-3 R3-A item 3 and the re-review prompt require a missing object to be
  `PendingActionAmbiguous`, with continue and abort both ineligible and no Git,
  record, event, or accepted-metadata mutation.
- `GWZRequirements.md` REQ-089E/H requires exact durable intent to govern
  restart recovery and makes missing or mismatched evidence mutation
  ineligible.

`decode_durable_prepared_action` checks the kind/result/spec shape but only
copies the tree OID and signatures. For a clean true-merge pending action at
the before commit, `reconcile_pending_action_from_live` returns `NotStarted`
without asking the backend to validate that commit specification. The
`missing_recorded_objects` scan includes only commit objects and never the
pending tree.

Continue therefore accepts the decoded action. The checked true-merge executor
recomputes the merge and calls `write_tree_to` before comparing the resulting
OID. If the durable tree object was missing, this silently recreates it and
then creates the merge commit.

**Concrete real-Git reproduction:**

1. Create a one-member workspace with a clean true-merge graph.
2. Run merge through its normal finalizing result and use that exact result to
   reconstruct the documented crash-before-execution record: participant
   `failed`, operation `recovery_required`, and a pending true-merge commit
   specification containing the exact tree and signatures.
3. Reset the member to the recorded before commit.
4. Delete the loose tree object named by `commit_spec.tree_oid`.
5. Run merge status and then continue through the current rebuilt native
   extension.

Observed:

```text
tree_exists_before_status False
aggregate AggregateStatus.failed
pending_state MergePendingActionState.not_started
continue_eligible True
abort_eligible True
drift []
record_unchanged True
tree_recreated_by_status False
continued_state MergeOperationState.finalizing
continued_repo_state MergeParticipantState.merged
tree_recreated_by_continue True
```

Status itself remained read-only, but it approved a damaged durable
specification. Continue then performed the mutation that the contract requires
preflight to reject.

**Impact:**

This path does not substitute different content: the checked executor still
requires the recomputed tree OID to equal the frozen OID and uses the frozen
signatures. It nevertheless breaks the fail-closed recovery boundary.
Deletion, pruning, or corruption of durable commit evidence is hidden from
status; automation receives no drift, both eligibility flags are true, and
continue mutates the object database and ref instead of requiring recovery.
The same early `NotStarted` return also means malformed tree/signature fields
in a true-merge specification are not rejected by status.

**Recommended correction:**

Add one read-only validation path for a not-started prepared true merge. Before
returning `NotStarted`, it must validate the complete prepared specification,
including OID syntax, signature representability, source/before objects, and
existence of the recorded tree object, without recomputing or writing the
tree. Alternatively, add a narrow backend object/tree-existence primitive and
combine it with shared prepared-signature validation. Status, continue, and
abort must consume that same result. Any failure must project
`PendingActionAmbiguous`, disable both actions, and leave all state unchanged.

**Regression test:**

Add
`pending_true_merge_missing_tree_is_ambiguous_without_mutation`: persist an
exact prepared true-merge action, remove its otherwise-unreferenced tree,
snapshot record/ref/index/worktree/native state/accepted metadata, and assert
status plus rejected continue and abort leave every snapshot byte unchanged.
Event assertions should allow only each invocation's required outer
start/finish pair and reject member, artifact, or state-change events. Add
malformed OID and malformed author/committer signature rows to the same table.
The existing exact-tree control must continue to prove zero preparation calls
and use of the frozen signatures.

## P3 proof and documentation gaps

### P3-1 — The required open-state cross-driver matrix is still partial

**Affected files and lines:**

- `gwz-core/src/workspace_ops/merge/mod.rs:621-681` covers all four states and
  both start modes through direct core.
- `gwz-py/src/tests/test_native_merge_operations.py:233-280` covers all four
  states and both modes through synchronous/submitted native dispatch.
- `gwz-cli/tests/local_workflows.rs:430-454` covers the Rust CLI from an
  unrelated cwd, but only for the naturally produced `finalizing` state.
- `gwz-py/src/tests/test_native_branch_stash.py:70-124` covers Python
  client/CLI behavior from an unrelated cwd, but only for
  `awaiting_resolution`.

**Violated criterion:** RemPlan-3 R3-C item 6 requires the open-state ×
dry/real × public-surface matrix, with explicit root from an unrelated current
directory. This was the accepted 56-3 P3-2 proof gap that R3-C said to close.

**Concrete failure scenario:** a later driver routing change affects only
`halted` or `recovery_required`. Direct core/native rows stay green, while the
Rust or Python CLI bypasses or renders the rejection differently.

**Impact:** this is not a reproduced current policy bypass. Inspection shows
both drivers now route first-class merge directly to
`handle_merge_with_events`, and the covered public rows pass. The missing
cross-product weakens regression proof.

**Recommended correction and regression test:** extend a table-driven
real-workspace harness so Rust CLI, Python client, and Python CLI each exercise
all four states in dry-run and real mode from an unrelated cwd with explicit
root, asserting exact start/finish/error order and byte-level non-mutation.

### P3-2 — Continue documentation still leaves retry-time identity ambiguous

**Affected file and lines:**
`gwz-core/dev-docs/GwzMergeDesign.md:752-755`.

**Violated criterion:** RemPlan-3 R3-D requires the design to say consistently
that identity is resolved before durable intent and that a not-started retry
consumes the already frozen signatures.

**Concrete failure scenario:** section 13 says identity comes from “the request
that creates the resolution commit” and applies the same rule to a true-merge
retry. A later implementer can read that as the recovery invocation and
re-resolve attribution after an intent-persistence crash, despite the precise
frozen rule later in section 16.

**Impact:** the current implementation and section 16 use the correct frozen
values; this is documentation-driven regression risk, not a current runtime
defect.

**Recommended correction:** state at section 13 that current request or
repository identity is consulted only while preparing a new action. Once
intent exists, every immediate or recovered execution consumes the recorded
author and committer signatures.

**Regression check:** extend the existing authoritative-doc consistency check
to reject “request that creates the resolution commit” wording unless the same
paragraph explicitly distinguishes new preparation from existing durable
intent.

## Disposition of the accepted F5-3 defects

### F5-3 P1 — tree-A/tree-B replacement: closed

I independently recreated a real conflicted member, staged resolution tree A,
persisted the exact resolution action, then staged tree B. Current behavior was:

```text
tree_changed True
pending_state MergePendingActionState.ambiguous
continue_eligible False
abort_eligible False
status_zero_mutation True
reject_zero_mutation True
rejections [('continue', 'GwzBridgeError', 'MergeRecoveryRequired'),
            ('abort', 'GwzBridgeError', 'MergeDrift')]
```

The pending action remained unchanged, neither preparation method was used by
the permanent exact-retry controls, and changed post-intent resolution content
was not adopted. The new P2 is a different missing-evidence boundary and does
not recur to tree-B substitution.

### F5-3 P2 — pre-emitter completion: closed

The public core entry constructs `EventEmitter::from_request_meta` and emits
start before context conversion, root/gate resolution, validation, and
dispatch. Rust no longer applies an earlier merge-specific driver gate; native
Python calls the same public core entry and publishes its terminal result only
after core returns.

Focused direct-core tests proved invalid attribution, all four open states in
both modes, backend failure, store failure, successful member events, and
member failure have one monotonically increasing lifecycle with finish last.
Native synchronous/submitted tests proved the original typed error,
finish-before-completion, multiple waiter wake-up, replay, and subscriber
cancellation behavior. The complete Python and cross-driver suites passed.

## Focused and adversarial verification

Each of these focused Rust tests passed with one executed test:

- `pending_resolution_tree_change_is_ambiguous_without_mutation`
- `pending_true_merge_not_started_executes_frozen_spec`
- `pending_resolution_exact_retry_uses_frozen_signatures`
- `durable_resolution_race_preserves_pending_intent_without_failed_outcome`
- `invalid_attribution_is_bracketed_by_one_outer_lifecycle`
- `every_open_merge_state_and_start_mode_is_bracketed_by_one_outer_lifecycle`

The permanent native matrices, multiple-waiter/cancellation tests, Rust CLI
open-gate JSONL test, and retained eight-scenario actual Rust/Python JSONL
parity test all passed inside the full suites.

One first draft of the external missing-tree reproduction exited before
status because the Python environment does not include PyYAML. The reproduction
was rerun using Ruby's standard YAML library and then completed with the
results recorded in P2. This was not a verification-gate failure.

## Full verification gate

Every required RemPlan-3 section 7 command passed:

| Command | Result |
| --- | --- |
| `cargo fmt --all -- --check` | pass |
| `cargo test --workspace --all-targets --no-fail-fast` | pass — **662 passed, 0 failed, 1 ignored** across emitted parent/child test summaries |
| `cargo clippy --workspace --all-targets --all-features -- -D warnings` | pass |
| `cargo fmt --manifest-path gwz-py/Cargo.toml -- --check` | pass |
| `cargo clippy --manifest-path gwz-py/Cargo.toml --all-targets --all-features -- -D warnings` | pass |
| `gwz-py/.venv/bin/python -m maturin develop --manifest-path gwz-py/Cargo.toml` | pass; current extension rebuilt and installed editable |
| `gwz-py: .venv/bin/python scripts/regen_protocol.py --check` | pass; `regen_protocol: OK` |
| `gwz-py: .venv/bin/python -m pytest -q` | pass — **308 passed** |
| `gwz-cli: python3 scripts/generate_cli_reference.py --check` | pass |
| strict MkDocs build | pass; upstream MkDocs 2.0 advisory only |
| `bazel build //gwz-core:gwz_core //gwz-cli:gwz` | pass; both targets built |
| `gwz status` and `gwz diff --no-pager` | integrated uncommitted snapshot inspected |
| root/core/CLI/Python `git diff --check` | pass in all four repositories |

The Rust and Python counts match the lead's recorded baseline.

## Residual risks and deferred items

- The M2b-A2 late-drift resting-state correction remains deliberately
  deferred and visible in the main plan.
- `GWZ_PY_TEST_EVENT_DELAY_MS` remains the documented M2b-C residual; I found
  no production semantic effect beyond delaying an operation-start event when
  that environment variable is explicitly set.
- Verification was on macOS arm64. Platform-specific filesystem durability,
  directory sync, and ref-lock behavior was not exercised on Linux or Windows.
- M2b publication/finalization, M2c explicit-root participation, M3
  preservation/history/GC, and new strategies remain outside this snapshot.

## Independent-review criterion

This is a fresh independent third-remediation review, but it **does not
satisfy** one of the two required passing reviews because it reports one P2
defect in the corrected M1/M2a recovery contract. M2b-A2, M2b-C, and shared M2b
integration may not resume until the missing-tree/specification boundary is
fixed, its focused regression and the complete gate pass, and two fresh
independent reviews report no P0/P1/P2 defect.
