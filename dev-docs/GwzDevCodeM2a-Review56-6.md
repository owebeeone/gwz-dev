# GWZ M1/M2a Third-Remediation Final Re-review — Reviewer 56

Date: 2026-07-23

Reviewer designation: **56**

## Gate decision

**PASS — I found no P0, P1, or P2 defect in the final corrected M1/M2a
contracts.**

The final correction closes the checked conflict-resolution missing-tree
defect reported in `GwzDevCodeM2a-ReviewF5-5.md`. Clean true-merge and
conflict-resolution actions now both validate their complete durable
specification before mutation and commit from the already recorded tree. The
original tree-A/tree-B and pre-emitter lifecycle defects remain closed.

Severity counts:

- P0: 0
- P1: 0
- P2: 0
- P3: 0

## Reviewed snapshot and method

I reviewed the complete integrated uncommitted tree on top of the required
submitted heads:

| Repository | Submitted head |
| --- | --- |
| workspace root | `5080b441a8003cc39f38c21ce0354bdae37e9549` |
| `gwz-core` | `e152c205bb2e46e401306914e53d7b004f9a3108` |
| `gwz-cli` | `c335d972228e9e6b21658cc14938f756af6533bb` |
| `gwz-py` | `fae628acd91936d386101ee9266e6916ea93d9b7` |

I read the required agent instructions, remediation plan, F5 `*-5.md`
finding, both `*-4.md` reviews, requirements, design, implementation plan,
merge documentation, and machine-output documentation. I inspected the
complete root, core, Rust CLI, and Python diffs and traced the relevant
callers. I did not read another `GwzDevCodeM2a-Review*-6.md` report.

I also ran two independent real-Git checks outside the workspace:

1. a direct-backend conflict-resolution check with the frozen tree deleted;
2. a one-member clean true-merge recovery check with the frozen tree deleted.

Implementation, tests, generated files, and existing documentation were not
modified. This report is my only workspace write.

## Findings

No verified defect or P3 proof gap.

## Primary final recovery-gate disposition

### Conflict-resolution durable action

The F5 `*-5.md` defect is **fixed**.

The resolution path now has one shared read-only validator:

- preparation binds the exact target branch before freezing the index tree
  and signatures at `gwz-core/src/git/gitbackend.rs:2198`;
- public status validation delegates to
  `validate_prepared_merge_resolution_in_repo` at
  `gwz-core/src/git/gitbackend.rs:2226`;
- checked execution locks the durable target ref and invokes that same helper
  before `repo.commit` at `gwz-core/src/git/gitbackend.rs:2247`;
- the helper verifies attached target branch and exact ref/before/source/native
  state, resolved index/worktree, frozen author/committer/timezone, recorded
  tree syntax/existence/type, and tree-to-index equality at
  `gwz-core/src/git/gitbackend.rs:2680` and
  `gwz-core/src/git/gitbackend.rs:2715`; and
- execution passes the existing `validated.tree` directly to `repo.commit`.
  It no longer calls `index.write_tree`.

Status passes the durable target branch into the same public validator at
`gwz-core/src/workspace_ops/merge/status.rs:203`. Continue passes it through
fresh preparation and durable execution at
`gwz-core/src/workspace_ops/merge/continue_op.rs:397` and
`gwz-core/src/workspace_ops/merge/continue_op.rs:529`.

My independent direct-backend check created a real conflicting merge, staged
resolution A, froze its tree/signatures, removed only the loose frozen tree,
and invoked both validation and checked execution with the same prepared
value. Observed:

```text
read_only_validation=rejected tree_before_direct=false
direct_checked=rejected tree_after_direct=false head_changed=false native_merge_open=true
```

Thus the checked action did not recreate the missing tree, did not create a
commit, did not move the target ref, and did not clean native merge state.

The permanent correction tests independently cover:

- absent and malformed trees;
- invalid author, committer, and timezone;
- deletion after whole-operation preflight but before locked execution;
- switching to another branch at the same commit;
- staged tree-A/tree-B drift;
- unchanged exact execution using frozen signatures; and
- zero preparation calls and no post-boundary repository, record, event,
  manifest, or accepted-lock mutation on rejection.

### Clean true-merge durable action

The clean true-merge missing-tree defect from both `*-4.md` reviews remains
**fixed**.

The checked path locks the durable branch ref and calls the shared
`validate_prepared_merge_upstream_in_repo` before any object-writing
primitive. The helper verifies branch/ref/HEAD, before/source commits, result
class, recorded tree and exact in-memory merge result, and frozen signatures.
Execution loads the recorded tree and commits from it without
`write_tree_to`.

My independent CLI reproduction created divergent member branches, forced the
initial checked execution to stop after durable intent using a target-ref lock,
deleted the recorded tree, and ran status, continue, and abort. Observed:

```text
start=1
status=1/Ambiguous
continue=1/MergeRecoveryRequired
abort=1/MergeDrift
tree_recreated=no
repo_unchanged=yes
```

The durable pending action therefore remained fail-closed and no Git file or
object changed.

The exact true-merge and resolution controls also passed: neither retry calls
a preparation method, and later request identity, repository configuration,
or clock changes do not replace the frozen tree, author, committer, timestamps,
or timezone.

## Whole-remediation disposition

- **Original tree-A/tree-B replacement — fixed.** Changed staged resolution
  content is ambiguous; continue and abort are ineligible; the pending action
  remains intact; exact execution reuses tree A.
- **Original pre-emitter lifecycle defect — fixed.** The one outer owner at
  `gwz-core/src/workspace_ops/merge/mod.rs:315` emits start before context
  conversion/open-operation gating/validation/dispatch and finish after the
  last inner event on every accepted invocation.
- **Clean true-merge missing-tree/specification defect — fixed.** Status and
  locked execution share the complete read-only validator and do not recreate
  missing evidence.
- **Conflict-resolution checked-execution defect — fixed.** Status and locked
  execution share the complete branch-aware read-only validator and commit
  from the existing recorded tree.
- **Open-operation surface matrix — complete.** All four open states in
  dry-run and real modes are covered through direct core, native synchronous
  and submitted paths, Rust CLI, Python client, and Python CLI. Public drivers
  use an explicit root from an unrelated current directory and assert
  byte-level non-mutation.
- **Native completion and errors — correct.** Backend and malformed-store
  failures retain the original typed/member-scoped error in synchronous and
  submitted modes. `OperationFinished` precedes terminal visibility; multiple
  waiters wake consistently; iterator close/cancellation does not own or cancel
  the operation; replay remains complete.
- **Driver parity — current.** The retained eight-scenario Rust/Python JSONL
  matrix, merge response/event fixtures, generated Python protocol, and Rust
  CLI reference all pass.
- **Documentation — consistent.** Requirements, design, and plan agree that
  target branch, parents, result/tree, complete signatures, and pending action
  are frozen before execution and are not re-resolved on retry.

## Focused and adversarial verification

Each named Rust test below passed with one executed test:

- `checked_resolution_invalid_evidence_rejects_without_mutation`
- `pending_resolution_missing_tree_race_rejects_without_mutation`
- `pending_resolution_same_commit_branch_switch_race_rejects_without_mutation`
- `pending_resolution_tree_change_is_ambiguous_without_mutation`
- `pending_resolution_exact_retry_uses_frozen_signatures`
- `pending_true_merge_not_started_executes_frozen_spec`
- `invalid_durable_true_merge_evidence_is_ambiguous_and_blocks_recovery`
- `durable_resolution_race_preserves_pending_intent_without_failed_outcome`
- `invalid_attribution_is_bracketed_by_one_outer_lifecycle`
- `every_open_merge_state_and_start_mode_is_bracketed_by_one_outer_lifecycle`
- Rust CLI
  `merge_dry_run_alias_and_first_class_start_work_end_to_end`

The permanent Python suite also exercised the complete public/native matrix,
typed backend/store failures, multiple waiters, iterator close/replay, and
cross-driver JSONL scenarios.

## Full verification gate

Every RemPlan-3 section 7 command passed:

- `cargo fmt --all -- --check`
- `cargo test --workspace --all-targets --no-fail-fast`
- `cargo clippy --workspace --all-targets --all-features -- -D warnings`
- Python-native Cargo format and strict Clippy checks
- fresh `maturin develop`
- Python protocol regeneration check
- full Python `pytest`
- generated Rust CLI-reference check
- strict MkDocs build
- Bazel `//gwz-core:gwz_core` and `//gwz-cli:gwz` build
- `gwz status`
- `gwz diff --no-pager`
- root, core, CLI, and Python `git diff --check`

Exact counts:

- **666 Rust pass executions, 0 failed, 1 ignored.** Cargo's top-level test
  summaries contain 665 passes and 1 ignored test; the lock-contention test
  also launches and passes one child test process, matching the plan's 666
  execution baseline.
- **314 Python tests passed, 0 failed.**

The strict MkDocs command emitted only its upstream MkDocs 2.0 advisory; the
strict build itself completed successfully.

## Residual risks and deferred work

Raw Git processes do not honor GWZ's workspace lock. The corrected checked
actions therefore continue to rely on durable target-ref locking, immediate
in-lock revalidation, content-addressed recorded trees, and postcondition
verification. The adversarial hooks cover interference immediately before
that locked validation boundary; no contract failure was observed.

The following remain intentionally deferred and are not defects in this
snapshot:

- M2b finalization/publication and the M2b-A2 late-drift resting state;
- M2b-C removal or test-only gating of `GWZ_PY_TEST_EVENT_DELAY_MS`;
- M2c explicit root participation;
- M3 preservation, history, retention, and GC; and
- later merge strategies and source forms.

The existing macOS PyO3 unit-harness linker limitation also remains unchanged;
strict native Clippy, a fresh extension build, and the full Python native tests
cover the modified native paths.

## Review-unblock statement

This report **satisfies one of the two required passing final-snapshot
reviews**. It contains no P0/P1/P2 finding and identifies no technical reason
to keep M2b-A2 or M2b-C shared integration blocked.

The two-review process gate is satisfied only when the other independent final
review also passes. Subject to that second passing report, shared M2b
integration **may resume**.
