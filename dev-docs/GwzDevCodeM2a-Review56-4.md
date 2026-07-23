# GWZ M1/M2a Third-Remediation Re-review — Reviewer 56

Date: 2026-07-23

Reviewer designation: **56**

## Gate decision

**FAIL — P2: a clean true-merge pending action is classified as exact and
executed when its durably recorded tree object is missing.**

The submitted third remediation closes both concrete F5-3 reproductions:
tree-A/tree-B resolution drift is now ambiguous without mutation, and the
single outer lifecycle owner brackets pre-context and open-operation failures.
It does not satisfy the full exact-durable-action exit criterion. Status does
not include a true-merge `commit_spec.tree_oid` among the required recorded
objects, so it reports the action as `NotStarted` with both continue and abort
eligible. Continue then recreates that absent tree object while recomputing the
merge and commits it.

Severity counts:

- P0: 0
- P1: 0
- P2: 1
- P3: 3

M2b-A2, M2b-C, and shared M2b integration remain blocked.

## Reviewed snapshot and method

I reviewed the complete integrated uncommitted tree on top of the required
submitted heads:

| Repository | Submitted head |
| --- | --- |
| workspace root | `5080b441a8003cc39f38c21ce0354bdae37e9549` |
| `gwz-core` | `e152c205bb2e46e401306914e53d7b004f9a3108` |
| `gwz-cli` | `c335d972228e9e6b21658cc14938f756af6533bb` |
| `gwz-py` | `fae628acd91936d386101ee9266e6916ea93d9b7` |

I read all required source-of-truth and prior-review inputs, inspected the root,
core, Rust CLI, and Python diffs and relevant callers, ran the named focused
tests and retained cross-driver parity suite, performed an independent real-Git
missing-object reproduction, and ran the complete RemPlan-3 section 7 gate.
I did not modify implementation, tests, generated files, or existing
documentation. This report is my only workspace write.

## Verified defect

### P2 — Missing durable true-merge tree is treated as `NotStarted` and recreated during continue

**Affected code:**

- `gwz-core/src/workspace_ops/merge/status.rs:107-134`
- `gwz-core/src/workspace_ops/merge/status.rs:536-577`
- `gwz-core/src/git/gitbackend.rs:1392-1408`

**Violated requirements and remediation criteria:**

- RemPlan-3 section 3 says missing objects make a pending action ambiguous and
  make both continue and abort ineligible.
- RemPlan-3 R3-A task 3 requires a missing tree to be ambiguous.
- RemPlan-3 section 8 requires ambiguity to cause zero Git, record, event, and
  accepted-metadata mutation.
- REQ-089E/H require exact frozen-tree evidence and ambiguous treatment of
  missing evidence.
- `GwzMergeDesign.md:1106-1118` says missing evidence is ambiguous and is
  never made abort-eligible.

`missing_recorded_objects` checks only commit OIDs. It adds the pending before
and source commits but not `pending_action.commit_spec.tree_oid`. The clean
true-merge branch then returns `NotStarted` solely because HEAD is still at
`before`, the branch/ref match, and the worktree is clean.

Checked execution recomputes the merge and calls `write_tree_to` before
comparing the resulting OID with the recorded tree. Consequently, an absent
recorded tree is silently recreated instead of rejected as missing durable
evidence.

**Independent real-Git reproduction:**

1. Create a one-member workspace with divergent `main` and
   `feature/source`.
2. Leave `.git/refs/heads/main.lock` in place for the first start so planning
   and pending-action persistence succeed but checked execution fails at its
   ref lock.
3. Remove the ref lock and inspect the durable pending true-merge action.
4. Verify its recorded tree with
   `git cat-file -e <tree_oid>^{tree}`.
5. Run `gwz merge --status`, then `gwz merge --continue`.

Observed:

```text
recorded tree_oid: 1ee0f2633e7143829bf8e6304ac8708d6c5455a0
git cat-file before continue: exit 128 (object absent)
status pending action: NotStarted
status continue_eligible: true
status abort_eligible: true
continue: exit 0, participant Merged, operation Finalizing
git cat-file after continue: exit 0 (tree recreated)
```

The ref-lock failure is a normal crash-boundary surrogate: exact pending
intent is durable, Git mutation has not begun, and the participant remains at
its clean before commit. The missing tree occurs without editing the operation
record.

**Impact:**

Recovery accepts incomplete durable evidence and mutates the Git object store
to repair it. The content-addressed comparison prevents this particular
reproduction from adopting a different tree, so this is P2 rather than P1, but
it defeats the explicit fail-closed contract and permits finalization to
publish a result derived from an action specification that status had not
proved complete.

**Recommended correction:**

Make the shared durable-action validator verify the complete commit
specification, including valid frozen signatures and existence/type of the
recorded tree object, before classifying a true merge as `NotStarted`. Status
and checked execution should use the same read-only validation. Checked
execution must prove the recorded tree exists before any `write_tree_to` call;
then it may recompute and compare the live merge tree under the ref lock.
Missing or malformed evidence must return the existing structured
pending-action ambiguity, with both eligibility flags false and no mutation.

**Required regression test:**

Add
`pending_true_merge_missing_tree_is_ambiguous_without_mutation`: persist a
clean true-merge pending action at the exact before state, remove its recorded
tree object, and assert:

- status reports `Ambiguous`;
- continue and abort are both ineligible;
- rejected status/continue/abort leave record bytes, refs/HEAD, object-store
  inventory, index, worktree, native state, events, and accepted metadata
  unchanged; and
- the preparation counter remains zero.

Add a companion malformed-signature row so the durable decoder cannot label an
unusable commit specification `NotStarted`.

## P3 proof and documentation gaps

### P3-1 — The public-driver open-state matrix is still not the required cross-product

**Affected tests:**

- `gwz-core/src/workspace_ops/merge/mod.rs:625-680`
- `gwz-cli/tests/local_workflows.rs:376-455`
- `gwz-py/src/tests/test_native_merge_operations.py:233-280`
- `gwz-py/src/tests/test_native_branch_stash.py:67-124`

**Violated criterion:** RemPlan-3 R3-C task 6 requires all four open states,
dry-run and real start, explicit root from an unrelated current directory, and
every Rust CLI, Python client, Python CLI, native, and direct-core surface.

Direct core and native synchronous/submitted tests now cover all four states
and both modes. The Rust CLI row exercises the naturally produced
`finalizing` state, and the Python client/CLI rows exercise
`awaiting_resolution`; they do not pin `halted` or `recovery_required` through
those public drivers.

**Failure scenario and impact:** a later driver dispatch or root-resolution
change could special-case one unrepresented state while the direct-core and
native matrices stay green. Current inspection confirms the drivers share the
correct outer owner, so this is a proof gap rather than a reproduced defect.

**Recommended correction and regression test:** reuse the four frozen open
records in one table-driven public-surface harness covering state × mode ×
Rust CLI/Python client/Python CLI, always from an unrelated cwd with explicit
root. Assert start/finish/error order and byte-level non-mutation.

### P3-2 — Backend and malformed-store native failures are tested only through submitted execution

**Affected test:**
`gwz-py/src/tests/test_native_merge_operations.py:283-325`.

**Violated criterion:** required focus C and R3-C require synchronous and
submitted completion evidence across backend/store failures, with typed errors
and byte-level non-mutation.

**Failure scenario and impact:** a future change in synchronous `call` setup or
terminal publication could diverge from `submit` only for a backend or
malformed-record error. The shared `run` implementation currently makes that
unlikely, and no divergence was reproduced.

**Recommended correction and regression test:** parameterize both failure rows
over synchronous/submitted execution and snapshot workspace bytes before and
after. Assert the exact structured error, start/finish order, completion, and
no successful merge response in both modes.

### P3-3 — Continue prose still understates the frozen-signature invariant

**Affected documentation:**
`gwz-core/dev-docs/GwzMergeDesign.md:752-755`.

**Violated criterion:** RemPlan-3 R3-D requires requirements, design, plan, and
code to say consistently that identity is resolved before durable intent and
the complete signatures are frozen.

This passage says the exact parents are “the merge invariant,” then says
identity comes from the request or repository and that the same rule applies
to a true-merge retry. Read alone, it can be taken to authorize identity
resolution again during a retry, despite the corrected frozen-signature rules
at lines 545-553 and 1106-1118.

**Failure scenario and impact:** an M2b contributor following the continue
section could use the retry request or current repository identity for an
already durable action. Current code and focused tests use the frozen
signatures, so this is documentation-driven regression risk only.

**Recommended correction and regression check:** clarify that request-or-repo
identity is resolved only while preparing a new action; an existing pending
action always consumes its recorded signatures. Pin that statement in the
authoritative documentation consistency check.

## Disposition of the accepted F5 defects

| Accepted F5-3 defect | Disposition |
| --- | --- |
| P1 tree-A/tree-B durable resolution replacement | **The concrete defect is closed.** A changed resolved index is ambiguous, status is read-only, continue/abort are ineligible, existing durable resolution and true-merge actions do not call preparation again, exact retries retain the frozen tree/signatures, and an execution race preserves intent without a false failed outcome. The new P2 shows that the broader “all missing evidence is ambiguous” exit criterion is not yet closed. |
| P2 pre-emitter terminal completion | **Closed.** One outer owner is created from protocol-safe request metadata before context conversion and the start gate. Invalid attribution, all open states/modes, backend/store errors, success, and member failure retain one monotonic lifecycle, with finish last and native completion afterward. The original structured model error survives. |

## Focused and adversarial results

The following permanent focused tests passed:

- `pending_resolution_tree_change_is_ambiguous_without_mutation`;
- `pending_true_merge_not_started_executes_frozen_spec`;
- `pending_resolution_exact_retry_uses_frozen_signatures`;
- `durable_resolution_race_preserves_pending_intent_without_failed_outcome`;
- `invalid_attribution_is_bracketed_by_one_outer_lifecycle`;
- `every_open_merge_state_and_start_mode_is_bracketed_by_one_outer_lifecycle`;
- `member_scoped_failure_keeps_intermediate_events_and_finishes_last_once`.

The native merge, native branch/stash gate, and actual Rust/Python merge JSONL
parity files passed **42 tests**, including the retained eight-scenario parity
matrix.

The independent missing-tree reproduction above failed the required invariant:
it is the basis of P2. The original F5 tree-A/tree-B case was also exercised
through its real-Git regression and remained ambiguous without mutation.

One exploratory pytest command named a nonexistent file
`test_actual_merge_jsonl_parity.py` and exited 4 before running tests. I
corrected the filename to `test_merge_cli_cross_driver.py`; the intended three
focused files then passed 42/42. The initial Rust focused commands used
`--exact` with unqualified names and selected zero tests; I reran every named
test without `--exact`, and each selected and passed one test.

## Full verification gate

Every required RemPlan-3 section 7 gate command passed:

| Gate | Result |
| --- | --- |
| root Rust format | pass |
| Rust workspace tests | **662 passed, 0 failed, 1 ignored** across the reported test-result invocations |
| root Rust Clippy, all targets/features, warnings denied | pass |
| native Rust format | pass |
| native Rust Clippy, all targets/features, warnings denied | pass |
| `maturin develop` | pass; current editable native extension rebuilt |
| generated Python protocol check | pass |
| Python suite | **308 passed, 0 failed** |
| generated Rust CLI reference check | pass |
| strict MkDocs build | pass; upstream MkDocs 2.0 advisory only |
| Bazel `gwz_core` and `gwz` build | pass |
| `gwz status`, `gwz diff --no-pager` | pass; expected integrated uncommitted changes reviewed |
| root/core/CLI/Python `git diff --check` | pass |

The Rust output includes one reported filtered one-test invocation in addition
to the ordinary test binaries; summing the gate's reported result lines gives
the lead baseline of 662 passes and one ignored test.

## Residual risks and deferred items

- `GWZ_PY_TEST_EVENT_DELAY_MS` remains the accepted M2b-C residual; I found no
  production effect beyond delaying after `OperationStarted` when explicitly
  set.
- The M2b-A2 late-drift resting-state correction remains deferred and visible.
- M2b final publication, M2c root participation, M3 preservation/history/GC,
  and new strategies remain outside this review.
- Verification ran on macOS arm64. Linux/Windows filesystem and ref-lock
  behavior, power-loss durability, and broad raw-Git race stress remain
  residual risks.
- Direct `cargo test` of the macOS PyO3 unit harness retains the previously
  documented undefined-Python-symbol limitation; the required native Clippy,
  rebuilt extension, and Python tests passed.

## Independent-review requirement

This review **does not satisfy** one of the two independent-review requirements
because it reports a P2 defect in the corrected exact-recovery contract.
M2b-A2, M2b-C, and shared M2b integration must not resume until the missing-tree
path is corrected, its zero-mutation regression passes, the complete gate is
green, and two fresh independent reviews of that corrected snapshot report no
P0/P1/P2 finding.
