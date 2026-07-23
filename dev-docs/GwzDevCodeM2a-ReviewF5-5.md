# GWZ M1/M2a Third-Remediation Correction Re-review — Reviewer F5

Date: 2026-07-23

Reviewer designation: **F5**

## Gate decision

**FAIL — P2: checked conflict-resolution execution can recreate a missing
frozen tree and commit it after the shared read-only validator has rejected the
same durable specification. M2b-A2, M2b-C, and shared M2b integration remain
blocked.**

The correction closes the `*-4.md` clean true-merge missing-tree defect and the
public-driver/native proof gaps. The original tree-A/tree-B and pre-emitter
lifecycle defects also remain closed. However, the equivalent checked
execution boundary for a durable conflict-resolution action still has two
definitions: status uses the new read-only validator, while the mutating
backend path writes the current index tree before checking the frozen tree OID.
I reproduced that divergence with real Git.

Severity counts:

- P0: 0
- P1: 0
- P2: 1
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

I read every required source-of-truth and prior-review input, inspected the
complete root/core/Rust CLI/Python diffs and relevant callers, ran the named
focused tests, independently reproduced the corrected true-merge missing-tree
case, built a separate real-Git resolution-race harness, and ran the complete
RemPlan-3 section 7 gate. I did not read another
`GwzDevCodeM2a-Review*-5.md` report. This report is my only workspace change.

## Verified defect

### P2 — Checked resolution execution recreates missing durable evidence instead of failing closed

**Affected code:**

- `gwz-core/src/git/gitbackend.rs:2210-2238`
- `gwz-core/src/git/gitbackend.rs:2241-2279`
- `gwz-core/src/workspace_ops/merge/continue_op.rs:513-535`

**Violated criteria:**

- RemPlan-3 section 4.1 requires status and checked execution to consume the
  same exact resolved-index definition, requires a missing recorded tree to be
  a typed recovery error, and forbids validation from creating a tree.
- RemPlan-3 R3-A requires missing objects to be ambiguous and zero-mutation,
  and requires exact resolution retries to consume the durable prepared
  action.
- RemPlan-3 section 8 expressly requires status and checked execution to share
  one exact resolved-index definition.
- `REQ-089E/H` requires a checked commit-producing action to consume its frozen
  tree/signatures and reject missing or ambiguous evidence before mutation.
- `GwzMergeDesign.md:1110-1122` says resolution preparation freezes the index
  tree, the checked action consumes it, and missing evidence is ambiguous.
- The correction-review gate requires checked execution to validate before
  every object-writing primitive and commit from the already recorded tree
  rather than recreate it.

`validate_prepared_merge_resolution_state` correctly parses the frozen
signatures, requires the recorded tree to exist as a tree, and compares that
tree read-only with the current index. In contrast,
`commit_prepared_merge_resolution_checked` does not call that validator or an
equivalent shared in-repository helper. After taking the ref lock it calls
`index.write_tree()` at lines 2259-2260, compares the newly written OID with the
recorded text, and only then loads the tree and frozen signatures.

That ordering means a missing recorded tree is silently reconstructed from the
current index. It also means checked execution and status do not share the
promised definition at the final mutation boundary.

**Concrete real-Git reproduction:**

1. Create divergent `main` and `feature/source` commits that conflict in
   `shared.txt`.
2. Prepare and enter the expected native merge, resolve and stage
   `shared.txt`, and call `prepare_merge_resolution_checked` to freeze tree A
   and the signatures.
3. Delete only tree A's loose object, leaving the branch, HEAD, native
   `MERGE_HEAD`, index entries/modes, worktree, before/source commits, and
   signatures otherwise exact.
4. Call `validate_prepared_merge_resolution_state`; it rejects the missing
   tree.
5. Call `commit_prepared_merge_resolution_checked` with that same
   `GitPreparedCommit`.

Observed:

```text
validation_error=GitCommandFailed
tree_exists_before_direct=false
direct_result=Ok(GitCommitResult { commit: "a4f007a4..." })
tree_exists_after_direct=true
head=Some("a4f007a4...")
```

The checked action recreated tree A, created the resolution commit, cleaned
native merge state, and advanced `main`. This is directly reachable from
production continue at `continue_op.rs:528`. A raw Git object prune or deletion
between continue's read-only whole-operation preflight and the checked backend
call produces the same race; the workspace mutator lock does not prevent raw
Git.

**Impact:**

The operation mutates the object store, native merge state, and target ref
after the durable specification has become incomplete. Content addressing
still forces the recreated tree to have the recorded OID, so I did not
reproduce content substitution or user-data loss and classify this as P2
rather than P1. It nevertheless violates the explicit fail-closed recovery
contract and is the resolution analogue of the `*-4.md` P2 that this correction
fixed for clean true merges.

**Recommended correction:**

Extract an in-repository form of the shared resolution validator and invoke it
under the existing target-ref lock before any object-writing primitive. It
should validate native state, branch/ref/HEAD, index/worktree, frozen
signatures, recorded tree syntax/existence/type, and exact tree-to-index
equality once. Then parse and load `prepared.tree_oid` and create the commit
from that already recorded tree. Do not call `index.write_tree()` during
checked execution. Map missing/malformed prepared evidence to the existing
typed recovery mismatch so the caller preserves member context.

**Required regression test:**

Add
`pending_resolution_missing_tree_race_rejects_without_mutation`: persist an
exact durable resolution action, pass status/preflight, delete the recorded
tree immediately before checked execution, and assert:

- direct checked execution and continue both reject with typed recovery error;
- the recorded tree remains absent and no replacement object or commit is
  created;
- ref/HEAD, index, worktree, native merge state, record bytes/events, accepted
  metadata, and pending action are byte-for-byte unchanged;
- neither preparation method is called; and
- the existing exact control still commits from the recorded tree and frozen
  signatures after retry-time identity/configuration/clock changes.

Include malformed tree/signature/timezone rows at the same direct checked
execution boundary.

## Primary correction-gate disposition

The `*-4.md` clean true-merge missing-tree/specification defect is **fixed**.
The shared `validate_prepared_merge_upstream_state` now validates the exact
attached branch/ref/HEAD, before/source commits, result class, frozen
signatures, tree OID syntax/existence/type, and in-memory merge result without
writing an object. Checked true-merge execution invokes the same helper under
the target-ref lock and commits from `repo.find_tree(recorded_oid)` without
`write_tree_to`.

My independent real-Git reproduction used a one-member divergent merge. A
pre-existing `main.lock` left a durable pending true-merge action at the clean
before state; I then deleted only its recorded loose tree object and verified:

```text
git cat-file before status: 128
status pending state: Ambiguous
status continue_eligible: false
status abort_eligible: false
continue: MergeRecoveryRequired
abort: MergeDrift
git cat-file after all checks: 128
workspace file snapshot diff: 0 lines
```

The permanent
`invalid_durable_true_merge_evidence_is_ambiguous_and_blocks_recovery` test
also passed for absent tree, malformed tree OID, invalid author, invalid
committer, and invalid timezone. The valid frozen true-merge control passed.

## Whole-remediation disposition

- **Original tree-A/tree-B defect: fixed.**
  `pending_resolution_tree_change_is_ambiguous_without_mutation` passes;
  status, continue, and abort reject staged tree B without replacing durable
  tree A or re-preparing the action.
- **Original pre-emitter lifecycle defect: fixed.**
  Context-conversion, effective-workspace gate, validation, and dispatch are
  bracketed by one outer `OperationStarted`/`OperationFinished` owner.
  Member events retain one monotonic sequence and terminal native completion
  is published afterward.
- **Clean true-merge missing-tree defect from both `*-4.md` reviews: fixed.**
  The independent and permanent reproductions above pass.
- **Resolution checked-execution boundary: not fixed.**
  The P2 in this report prevents the whole corrected M1/M2a contract from
  passing.
- The public four-state × dry-run/real matrix now covers Rust CLI, Python
  client, and Python CLI from an unrelated cwd with explicit root and
  byte-level non-mutation. Direct core and native synchronous/submitted
  coverage also pass.
- Native backend and malformed-store failures now pass in synchronous and
  submitted modes with original structured errors and exact lifecycle
  completion.
- Multiple waiters, iterator close/replay, native completion, and the retained
  Rust/Python JSONL parity scenarios passed.
- Requirements, design, and plan consistently freeze identity before durable
  intent and forbid retry-time re-resolution.

## Focused and full verification results

Focused permanent tests passed:

- `invalid_durable_true_merge_evidence_is_ambiguous_and_blocks_recovery`
- `pending_resolution_tree_change_is_ambiguous_without_mutation`
- `pending_true_merge_not_started_executes_frozen_spec`
- `pending_resolution_exact_retry_uses_frozen_signatures`
- `every_open_merge_state_and_start_mode_is_bracketed_by_one_outer_lifecycle`

The independent true-merge missing-tree reproduction passed. The independent
resolution missing-tree checked-execution reproduction failed the required
invariant as described in the P2 finding.

The complete RemPlan-3 section 7 gate otherwise passed:

- `cargo fmt --all -- --check`
- `cargo test --workspace --all-targets --no-fail-fast`: 663 Rust pass
  executions, 0 failed, 1 ignored (662 top-level suite passes plus the
  separately spawned lock-contention child pass)
- `cargo clippy --workspace --all-targets --all-features -- -D warnings`
- Python-native Cargo format and strict Clippy checks
- fresh `maturin develop`
- Python protocol regeneration check
- Python suite: **314 passed**, 0 failed
- generated Rust CLI reference check
- strict MkDocs build
- Bazel `//gwz-core:gwz_core` and `//gwz-cli:gwz` build
- `gwz status`, `gwz diff --no-pager`, and all four `git diff --check`
  commands

The green suite does not cover deletion of a frozen resolution tree after
whole-operation preflight but before the checked resolution commit.

## Residual risks and deferred items

The P2 above is the only verified current defect I found. After it is
corrected, the resolution validator should also gain direct malformed-evidence
and post-preflight race rows so the true-merge and resolution checked-action
contracts cannot drift again.

The following remain intentionally deferred and were not reported as defects:

- M2b-A2 late-drift resting-state correction;
- M2b publication/finalization and M2b-C cleanup of
  `GWZ_PY_TEST_EVENT_DELAY_MS`;
- M2c explicit root participation;
- M3 preservation/history/retention/GC; and
- later merge strategies and source forms.

This report **does not** satisfy one of the two required passing
corrected-snapshot reviews because it contains a verified P2. M2b-A2, M2b-C,
and shared M2b integration **may not resume** until checked resolution execution
uses the shared read-only frozen-tree definition, the new race regression
passes, the complete gate remains green, and two fresh independent reviews of
that corrected snapshot report no P0/P1/P2 defect.
