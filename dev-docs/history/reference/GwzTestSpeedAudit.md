# GWZ test speed audit

Date: 2026-09-07. Scope: bounded source audit; no broad test run, release action, or runtime profiling. v1.0.0 release preparation is aborted while the routine test path is simplified.

## Findings

There is concrete duplicated integration coverage we can remove without redesigning the product. Existing narrow interfaces and recording fakes already execute the real decision code. The first move should be to use them consistently, retain focused adapter coverage, and stop repeating the policy matrix through every layer.

The evidence does **not** establish that thousands of tests each create a Git repository, or that Git setup alone explains the reported 23-minute Linux / 53-minute Windows build-and-test steps. Those step times include compilation. A substantial checked-artifact fault matrix exercises real filesystem durability without Git at all. Removing Git fixtures cannot remove that cost.

A lexical source scan found 1,899 Rust `#[test]` declarations under core `src` and 437 under core `crates`; these are not executed counts (platform conditions and macros differ). It found 94 direct `Repository::init*` expressions under core `src`, but a single helper can create many repositories. There are 39 calls to `family_workspace` / `clean_family_workspace` under core `src`, and 28 calls to Python's `create_workspace_with_member`. These are indicators of repeated setup, not timings or a new count gate.

## First conversion: disposal policy duplication

`gwz-core/src/local_clone/tests/dispose.rs` repeats policy cases already exercised by the real disposal implementation in `gwz-core/crates/local-disposal/src/lib.rs`:

| Integration case | Existing narrow implementation test |
| --- | --- |
| `unknown_work_or_history_refuses_and_no_force_name_waives_it` | `unknown_work_or_history_refuses_and_no_force_waives_it` |
| `each_known_hazard_refuses_without_its_name_and_proceeds_with_it` | `each_known_hazard_refuses_until_its_own_name_is_given` |
| Refusal cannot remove a lane | `no_refusal_path_ever_reaches_the_remover` |

The integration force-name case creates a registered two-repository workspace, clones two lanes, commits unique history, writes native merge state, and repeats four waiver combinations. Each refusal also recursively reads whole trees through `tree_bytes`, including Git objects. The library test already drives the real `dispose` function with `RecordingDisposalPorts` and a `FamilySession` implementation (`ScriptedSession`). It observes removal calls and actual session changes; it is not merely comparing fixture values.

**Concrete change:** retain one integration case proving the real Git adapter reports dirty work, open merge state and unpreserved history, plus one successful ordinary disposal/lifecycle regression. Keep the waiver combinations and no-removal decision coverage at the existing disposal interface. Unknown index/layout classification remains a real adapter test, but no longer needs the full force-combination loop and family lifecycle for every classifier case. Preserve the untouched-lane and root-commit-preservation regressions that previously found product defects.

This does not require an in-memory implementation of Git. It removes repeated execution of the same policy through an expensive adapter.

## Second conversion: Python streaming tests

`gwz-py/src/tests/test_native_operations.py` has adjacent tests that create a workspace, a member, a bare remote, configure it and push merely to test event draining. The native round-trip case can remain as the integration check. `test_native_stream_helper_drains_events_and_result` can exercise the real `Client.push_stream` using the established bridge seam (`FakeBridge` in `test_client.py`, submitted-operation bridges in `test_bridge_transport.py`).

`test_native_stream_yields_before_operation_result_is_ready` adds a 500 ms native delay and waits 100 ms to infer ordering. Use explicit async events/barriers to hold completion until the first event has been observed when testing client orchestration. Retain a small native check for the bridge's actual asynchronous boundary; do not substitute a fake for the native behavior that test uniquely owns.

`native_helpers.py` starts eight Git subprocesses just to initialize and commit a standalone repository; `create_workspace_with_member` adds seven explicit Git calls plus core create/capture operations. Avoiding a fixture removes those subprocesses and workspace writes altogether. Changing how the fixture is initialized is a secondary optimization for tests that still need real Git.

## Other opportunities

- **Rust CLI:** `tests/local_family_workflows.rs::family_verbs_reach_core_and_report_its_typed_refusal` runs ten cases in both human and JSON modes after initializing a workspace. Keep a few process-level dispatch/channel checks, and exercise request construction, error rendering and exit-code mapping directly for the combinations. Existing protocol parity and renderer coverage should be checked before adding replacement tests. The goal is fewer subprocesses, not another copy of core semantics.
- **Core planning:** `workspace_ops/merge/plan.rs::PlanningBackend` and its test `FakeBackend` already separate planning from Git. Follow that narrow seam when a handler test is only checking selection, preflight failure or call ordering. Do not build a general fake implementation of the roughly 1,000-line `GitBackend` contract. `workspace_ops/tests/g14.rs` also demonstrates that routing is already isolated; do not refactor areas that are already cheap.
- **Real Git adapter tests:** `src/git/tests` contains 148 textual test declarations. Many exist specifically to verify Git behavior; keep that distinction. Graph ancestry, index interpretation, merge conflicts, object preservation and platform-specific filesystem behavior require real integration evidence somewhere.
- **Filesystem fault tests:** `checked_artifact/namespace/tests_fault_matrix.rs` and related modules repeatedly open/recover durable namespaces for fault permutations. They do not depend on Git repositories. Keep representative real filesystem recovery regressions in normal CI; place exhaustive fault combinations in the explicit extended suite. This is separate from the compiler probes the parent task is moving out now.
- **Fixture overhead:** `crates/local-testrepo/src/repo.rs::TestRepo` opens and closes a repository on each helper operation. Measure this only after removing redundant integration cases. For remaining setup, batch initialization/configuration/commits in one fixture construction or copy an immutable prepared seed into a unique directory. Never share mutable repositories across tests, and avoid cached seeds whose global identity, paths or configuration can contaminate results.
- **Waits:** the overlap fake in `workspace_ops/tests/g01/tracking_backend.rs` can wait two seconds if expected overlap is not reached; Git cancellation tests use a deliberately sleeping child. These may be legitimate tests, but their timeout paths deserve focused timings before attributing all delay to Git.

## Implementation order

1. Finish removing compiler probes and redundant suite invocations from ordinary release paths; retain an explicit manual entry point.
2. Convert the disposal waiver duplication and Python stream-helper setup above. Run only those focused suites and the retained affected integration cases while editing.
3. Separate compilation time from runtime once on the resulting normal path. Obtain per-test durations with a standard runner (for Python, `pytest --durations=20`; for Rust, an existing nextest installation if available, or time the small suspected groups). Do not build another inventory/reconciliation system or block progress waiting for a complete profile.
4. Use measured offenders to shrink redundant CLI/native and filesystem fault matrices. Keep normal coverage on relevant operating systems in parallel. Make the extended suite manual and clearly named, rather than silently skipping it.
5. Compare the same focused groups before and after, then one normal run on the slowest platform. Set an ordinary-test time budget based on measured compilation and execution separately; an under-five-minute target is useful, but is not a prediction from this audit.

No exact minutes-saved estimate is justified yet. The immediate benefit is known avoided work: extra repository creation/cloning, full-tree rereads, redundant native pushes, subprocess launches and fixed waits. A broad architecture rewrite is not a prerequisite for these first improvements.

## Follow-up: the two long-running root matrices

The parent task extracted the historical core CI timing: compilation took 59.96 seconds; the library test harness took 1,352.28 seconds. All tests except the two below had completed by 11:32:36; root ambiguity finished at 11:40:18 and root fault at 11:46:46. These timestamps establish a long tail, not the isolated duration of every scenario. A fresh timed run is being collected separately.

The source explains why two test names hide so much work:

| Single Rust test | Serial scenarios inside it |
| --- | --- |
| `reverse_preservation/root_fault_matrix.rs::every_root_physical_and_successor_boundary_recovers_without_repeating_mutation` | (54 publication-owner cases + 58 selected-owner cases) × 3 interruption boundaries = **336** |
| `reverse_preservation/root_ambiguity_matrix.rs::every_root_phase_rejects_fresh_ambiguous_work_without_physical_execution` | 2 owners × 22 phases × 5 requests = **220** |

Each scenario constructs a new `dirty_root_handoff_fixture_with_owner` in `reverse_preservation/mod.rs`, starting with `integrated_fixture`. This creates a root repository and member repository; writes three member commits; writes two root baseline commits, a composition commit and a protected root commit; and, for the selected-root owner, writes another root commit. It then creates staged, unstaged and untracked work. Across the two test bodies this is **556 independent fixtures, 1,112 repository creations and over 4,000 fixture commits**, before the behavior under test runs.

The expensive work continues after setup. Each scenario runs the real `ReverseRuntime<Git2Backend>` and `CheckedV1Store` from the beginning of preservation up to its chosen interruption point. It then resumes to a terminal aborted state. Fault cases permit eight additional recovery attempts; ambiguity cases first test the requested refusal, remove the ambiguity, then resume with up to seven additional attempts. These are upper retry limits, not evidence that every case uses every retry. Store rewrites use durable filesystem operations, including `sync_all` in `store/rewrite.rs`; runtime profiling is needed to separate this from Git and fixture costs.

There are no explicit sleeps in these two matrix files. Their scenario loops are serial inside two `#[test]` functions, so Cargo test parallelism cannot distribute the 556 individual cases. Splitting cases would help scheduling, but does not reduce the underlying workload and could increase filesystem contention.

### Concrete reductions for these tests

1. Move these exhaustive Cartesian matrices to the manual extended suite immediately. Keep focused native preservation/durability tests on the ordinary path. Nearby `root_durability.rs` already has targeted marker durability cases, and `root_successor_matrix.rs` separately exercises durable successor restart behavior; check that overlap before adding another replacement matrix.
2. Keep a small explicit routine set covering both root owners, a before-mutation refusal, an after-mutation restart, and an after-durable-successor restart, including the special parent-durability behavior. The manual matrix retains all combinations. Select named cases rather than silently truncating a loop.
3. For ambiguity checks, stop completing the entire lifecycle after *every* refused request. The assertion under test is no physical execution plus retained pending phase. Preserve one real ambiguity-removal-and-recovery case for each distinct ambiguity mechanism (conflicting backup reference, foreign stash bundle, foreign work), with targeted owner coverage. The full suffix recovery is already exercised repeatedly in fault/successor tests.
4. If the extended suite itself needs improvement, construct each stopped phase once and restore an isolated checkpoint for request variants, or invoke the actual phase/recovery logic through its existing observer/executor seam. Do not substitute a copied decision algorithm. A checkpoint must retain the real repository and journal relationships; avoid a shared mutable fixture. Measure this against straightforward independent cases before introducing a fixture cache.

This finding changes priority: the two giant matrices, rather than the earlier small disposal/Python examples, are the first runtime target. Moving their exhaustive permutations out of routine CI addresses the observed long tail immediately; the other conversions remain useful but should not delay that change.
