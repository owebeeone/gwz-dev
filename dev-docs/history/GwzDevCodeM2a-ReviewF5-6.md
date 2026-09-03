# GWZ M1/M2a Third-Remediation Final Re-review — Reviewer F5

Date: 2026-07-23

Reviewer designation: **F5**

## Gate decision

**PASS — I found no P0, P1, or P2 defect in the final corrected M1/M2a
snapshot.**

The second correction closes the conflict-resolution missing-tree defect from
`GwzDevCodeM2a-ReviewF5-5.md`. Clean true-merge and conflict-resolution
recovery now both validate the durable specification under the target-ref lock
before an object or ref write, and checked execution consumes the recorded
tree and signatures without re-preparing or reconstructing missing evidence.
The original tree-A/tree-B and pre-emitter lifecycle defects remain closed.

## Reviewed snapshot and method

I reviewed the complete integrated uncommitted tree on top of the required
submitted heads:

| Repository | Submitted head |
| --- | --- |
| workspace root | `5080b441a8003cc39f38c21ce0354bdae37e9549` |
| `gwz-core` | `e152c205bb2e46e401306914e53d7b004f9a3108` |
| `gwz-cli` | `c335d972228e9e6b21658cc14938f756af6533bb` |
| `gwz-py` | `fae628acd91936d386101ee9266e6916ea93d9b7` |

I read the required source-of-truth, remediation, earlier-review, user
documentation, and workspace-instruction inputs; inspected the complete
root/core/Rust CLI/Python diffs and relevant callers; ran the named focused
tests; and ran the complete RemPlan-3 section 7 gate. I did not read another
`GwzDevCodeM2a-Review*-6.md` report before completing this analysis.

For independent evidence, I built a small temporary Rust harness outside the
workspace and exercised real Git repositories for both a clean true merge and
a resolved conflict. In each case I froze the prepared action, deleted only
the recorded loose tree object, then invoked the public read-only validator
and public checked execution directly.

## Findings

No verified defect.

## Primary final recovery-gate disposition

### Conflict-resolution durable action

The resolution path now has one shared read-only definition:

- preparation requires the exact attached target branch, target ref,
  before/source commits, native merge state, resolved index, and safe worktree,
  then freezes the index tree and complete author/committer signatures
  (`gwz-core/src/git/gitbackend.rs:2198-2223`);
- status calls `validate_prepared_merge_resolution_state` with the durable
  target branch (`gwz-core/src/workspace_ops/merge/status.rs:203-213`);
- public validation and checked execution both call
  `validate_prepared_merge_resolution_in_repo`
  (`gwz-core/src/git/gitbackend.rs:2226-2245`,
  `gwz-core/src/git/gitbackend.rs:2247-2269`);
- the shared validator requires the exact attached branch and ref, native
  merge parents, resolved index/worktree, valid frozen signatures, an existing
  recorded tree object, and exact tree-to-index equality without writing a
  tree (`gwz-core/src/git/gitbackend.rs:2685-2757`); and
- checked execution locks the durable target ref first, validates, and creates
  the commit from `validated.tree`. It no longer calls `index.write_tree`
  (`gwz-core/src/git/gitbackend.rs:2256-2281`).

My independent real-Git missing-tree check observed:

```text
validation=MergeRecoveryRequired
checked execution=MergeRecoveryRequired
tree exists afterward=false
HEAD unchanged=true
native merge state unchanged=true
```

The direct permanent matrix additionally covers malformed tree OID, invalid
author, invalid committer, and invalid timezone. The post-preflight
missing-tree and same-commit branch-switch races preserve the durable pending
action, record-write count, repository snapshot, native state, lock,
manifest, and accepted metadata, and perform no re-preparation
(`gwz-core/src/workspace_ops/merge/start.rs:1973-2352`).

The valid control confirms that recovered execution uses the frozen tree,
author, committer, timestamps, and timezone despite later request identity,
repository configuration, and clock changes
(`gwz-core/src/workspace_ops/merge/start.rs:1861-1971`).

### Clean true-merge durable action

The true-merge correction from the `*-4.md` reviews remains intact:

- status and checked execution share
  `validate_prepared_merge_upstream_in_repo`;
- checked execution acquires the target-ref lock before validation
  (`gwz-core/src/git/gitbackend.rs:1315-1344`);
- the validator checks the exact clean branch/ref/HEAD, before/source commits,
  result class, frozen signatures, recorded tree syntax/existence/type, and
  the exact in-memory merge result without creating a tree
  (`gwz-core/src/git/gitbackend.rs:2872-2934`); and
- true-merge execution commits from the already loaded recorded tree.

My independent real-Git missing-tree check observed:

```text
validation=MergeRecoveryRequired
checked execution=MergeRecoveryRequired
tree exists afterward=false
HEAD unchanged=true
```

The permanent missing/malformed-evidence and valid frozen-spec controls also
passed. Existing durable actions are decoded and reused rather than sent
through either preparation method
(`gwz-core/src/workspace_ops/merge/continue_op.rs:379-394`).

## Whole-remediation disposition

- **Original tree-A/tree-B replacement defect: fixed.**
  Changed staged resolution content is ambiguous; status, continue, and abort
  do not replace the durable tree or prepare a second action.
- **Original pre-emitter lifecycle defect: fixed.**
  Context conversion, effective-root resolution, open-operation gating,
  validation, and dispatch have one outer lifecycle owner. All accepted
  success and failure paths retain exactly one `OperationStarted` and one
  final `OperationFinished`, with native completion published afterward.
- **Clean true-merge missing-tree defect from both `*-4.md` reviews: fixed.**
  Both the permanent regression and my independent real-Git check reject
  without recreating the tree or advancing the ref.
- **Conflict-resolution missing-tree defect from `ReviewF5-5.md`: fixed.**
  Checked execution uses the same read-only validator as status and commits
  from the recorded tree.
- The four open states × dry-run/real public-surface matrix covers direct core,
  native, Rust CLI, Python client, and Python CLI, including explicit root
  resolution from unrelated working directories and byte-level
  non-mutation.
- Native synchronous/submitted backend and store failures retain their typed
  errors and complete exactly once. Multiple waiters, event replay, iterator
  cancellation, and final-event-before-completion behavior remain covered.
- Rust/Python JSONL parity and generated protocol/CLI artifacts are current.
- Requirements, design, plan, and implementation consistently require frozen
  signatures, durable pending actions, the exact target branch, and one
  lifecycle owner.

## Focused and full verification results

The following focused permanent tests passed individually:

- `checked_resolution_invalid_evidence_rejects_without_mutation`
- `pending_resolution_missing_tree_race_rejects_without_mutation`
- `pending_resolution_same_commit_branch_switch_race_rejects_without_mutation`
- `invalid_durable_true_merge_evidence_is_ambiguous_and_blocks_recovery`
- `pending_resolution_tree_change_is_ambiguous_without_mutation`
- `pending_true_merge_not_started_executes_frozen_spec`
- `pending_resolution_exact_retry_uses_frozen_signatures`

The complete RemPlan-3 section 7 gate passed:

- Rust workspace suite: **666 pass executions**, 0 failed, 1 ignored (665
  top-level suite passes plus the separately spawned lock-contention child
  pass);
- Python suite: **314 passed**, 0 failed;
- both Rust format checks and both strict Clippy invocations;
- fresh native-extension rebuild with `maturin develop`;
- Python protocol regeneration and Rust CLI-reference checks;
- strict MkDocs build;
- Bazel `//gwz-core:gwz_core` and `//gwz-cli:gwz` targets;
- `gwz status` and `gwz diff --no-pager`; and
- root, core, Rust CLI, and Python `git diff --check`.

The counts exactly match the lead baseline.

## Residual risks and deferred work

Raw Git can still race after a checked validation boundary in ways that no
workspace lock can prevent. The target-ref transaction and final
self-verification materially constrain that window, and the reviewed contract
does not claim to serialize arbitrary direct manipulation of `.git`.

The following remain intentionally deferred and are not defects in this
snapshot:

- M2b-A2 late-drift resting-state correction;
- M2b publication/finalization and M2b-C removal of
  `GWZ_PY_TEST_EVENT_DELAY_MS`;
- M2c explicit root participation;
- M3 preservation/history/retention/GC; and
- later merge strategies and source forms.

## Final severity and unblock statement

- P0: 0
- P1: 0
- P2: 0
- P3: 0

This report **satisfies one of the two required passing final-snapshot
reviews**. It finds no defect that blocks the M2a remediation checkpoint.
Shared M2b integration may resume only when the other independent final review
also passes and the lead records both passing dispositions; subject to that
second pass, this snapshot is ready to unblock.
