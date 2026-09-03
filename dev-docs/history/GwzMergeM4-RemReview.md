# GWZ Merge M4 Remediation Independent Re-review

Date: 2026-07-26

Scope: the complete stable, uncommitted M4 remediation tree, including the
final simulated/native parity regression, test-support split, and milestone
evidence updates.

## Findings

No P0, P1, P2, or M4-specific P3 finding remains.

## Final P3 closure

| Prior P3 | Result | Evidence |
| --- | --- | --- |
| Same-fixture simulated/native conflict-path agreement was not tested | Closed | The object-database rename/rename fixture prepares an attached target and source (`gwz-core/src/git/tests/g17.rs:81-97`), then runs `merge_simulate` and native `merge_upstream` in the same repository and compares their sorted, deduplicated paths and stored native merge state (`gwz-core/src/git/tests/g17.rs:150-179`). |
| M4 compatibility logic enlarged an existing oversized test file | Closed | `gwz-core/src/workspace_ops/tests/g01.rs` is now 426 lines and delegates its coherent tracking fake through `mod tracking_backend` (`gwz-core/src/workspace_ops/tests/g01.rs:1-11`). The extracted `gwz-core/src/workspace_ops/tests/g01/tracking_backend.rs` is 355 lines. Both are below the 500-line remediation ceiling. |
| Milestone evidence recorded a stale core-test count | Closed | Both evidence statements now record 596 passing core tests and one intentional ignore (`dev-docs/GwzMergeM4-RemPlan.md:383-393`; `gwz-core/dev-docs/GwzMergePlan.md:1471-1477`). They also record the parity regression and 426/355-line split. |

## Original finding closure

| Original finding | Result | Evidence |
| --- | --- | --- |
| Non-UTF-8 conflicts can be misclassified as `Clean` | Closed | `merge_simulate` branches on `Index::has_conflicts()` before path projection (`gwz-core/src/git/gitbackend/merge_prepared.rs:329-349`). Projection consumes raw bytes and returns a typed error if a conflicted index exposes no paths (`gwz-core/src/git/gitbackend/merge_support.rs:151-249`). Portable byte-table and object-database tests pin escaping and prove an invalid-byte conflict is not `Clean` (`gwz-core/src/git/tests/g17.rs:99-148`). |
| Pull can simulate one source and execute a moved ref; root/member barrier races were insufficiently closed | Closed | Executable pull plans freeze the remote ref, expected local commit, exact source commit, and prepared result (`gwz-core/src/workspace_ops/pull_head_plan.rs:12-46`). The complete selection passes a read-only barrier before root or member application (`gwz-core/src/workspace_ops/pull_head_member_preflight.rs:141-158`; `gwz-core/src/workspace_ops/pull_head_barrier.rs:38-89`). Root and member merge-sync actions use checked prepared execution against exact commits (`gwz-core/src/workspace_ops/pull_head_merge_preflight.rs:255-339`; `gwz-core/src/workspace_ops/pull_head_plan.rs:123-183`). Pre-barrier rejection, post-barrier exact-source execution, mixed root/member execution, and checked target-race behavior are covered by `gwz-core/src/workspace_ops/tests/g25.rs:121-447`. |
| Rename conflicts omit ancestor/ours/theirs paths | Closed | The shared helper collects all three index stages independently before sorting and deduplicating (`gwz-core/src/git/gitbackend/merge_support.rs:223-249`). Synthetic all-stage coverage and the same-repository simulated/native rename test pin all three paths (`gwz-core/src/git/tests/g17.rs:114-124`; `150-179`). |
| Mixed root/member prediction and selection-wide `--ff-only`, including Python lowering, lacked proof | Closed | Mixed prediction, rejection-before-mutation, and success rows are covered in `gwz-core/src/workspace_ops/tests/g23/m4_matrix.rs:50-187`. Python lowers standalone `--ff-only` to `MergeMode.ff_only` in the existing routing row (`gwz-py/src/tests/test_cli_merge.py:66-72`). |

## Additional audit

- Cross-platform path handling is byte-oriented. The invalid-byte fixture is
  constructed below the filesystem boundary in Git objects, while the native
  parity fixture uses portable ordinary names. No host-dependent filename
  creation or lossy UTF-8 conversion controls conflict truth.
- Non-partial pull performs the complete final barrier before applying root or
  member actions. Checked prepared execution remains mandatory after the
  barrier.
- Partial `PredictedConflict` rows remain non-executable skips in validation
  and application (`gwz-core/src/workspace_ops/pull_head_barrier.rs:61-65`;
  `gwz-core/src/workspace_ops/pull_head_plan.rs:162-166`).
- Barrier and execution failures retain member id/path context, and root errors
  retain `@root`/`.` context
  (`gwz-core/src/workspace_ops/pull_head_barrier.rs:45-52`;
  `gwz-core/src/workspace_ops/pull_head_member_preflight.rs:150-158`;
  `gwz-core/src/workspace_ops/pull_head_merge_preflight.rs:401-410`).
- New and materially enlarged implementation, focused tests, and the extracted
  tracking support module are within the 500-line ceiling. The existing
  504-line Python merge test did not grow beyond the remediation-plan snapshot;
  its `--ff-only` assertion was folded into an existing routing row.
- User and protocol documentation consistently describe byte escaping,
  prediction completeness, selection-wide `--ff-only`, exact prepared-source
  execution, and remote-tracking-ref movement.

## Verification evidence

The complete post-fix stable gate is green:

- `cargo test --workspace`: 596 core tests passed, 1 intentionally ignored;
  all other Rust workspace suites passed;
- strict Clippy and formatting checks passed;
- protocol and generated CLI-reference freshness checks passed;
- the rebuilt Python/native suite passed 320/320;
- Bazel build for `//gwz-core/...` and `//gwz-cli/...` passed; and
- cross-repository diff hygiene passed.

The final independent snapshot additionally reran:

- all five `git::tests::g17` tests, including same-repository
  simulated/native parity;
- `workspace_ops::tests::g01::pull_head_fetches_selected_members_in_parallel`;
  and
- `cargo fmt --all -- --check`.

All passed.

## Release-platform evidence

The independent local code review ran on macOS arm64. After that review, the
completed release builds were confirmed for Windows, macOS, Linux x86, and
Linux arm64. Those builds are the accepted release-platform evidence.

The invalid-byte test remains platform-independent: it constructs Git objects
below the filesystem boundary and pins byte rendering directly, so the matrix
does not rely on every host filesystem accepting an invalid-UTF-8 filename.

## Final verdict

**M4 independent remediation re-review gate: PASS.**

All four original findings and all three follow-up P3 findings are explicitly
closed. No P0/P1/P2 defect and no M4-specific P3 coverage gap remains.
Remediation exit criterion 11 passes, and the M4 code-review release gate is
accepted. The completed release builds also satisfy the platform-evidence
condition.
