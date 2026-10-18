# Core test performance — 2026-09-07

Evidence links below point to private campaign archives and require repository access. Historical commands and recorded paths describe the original runs; see the archive README for replay setup.

This is a diagnostic measurement, not a new release gate. No count pins, test
inventory comparison or source instrumentation was added. v1.0.0 remains aborted.

## Where the original CI time went

Source: [core verification 34114642480](https://github.com/owebeeone/gwz-core/actions/runs/34114642480),
core commit `17e13ed2c0748cc40d0cf81bb1fad74fb176cd83`.

| Step within ordinary testing | Linux | Windows |
| --- | ---: | ---: |
| Cargo compilation | 59.96 seconds | 2m 46s |
| Core library tests | 22m 32.28s | 50m 7.42s |
| Linux integration test binaries combined | 0.34 seconds | — |

These are the compilation times within those CI steps, not clean-build benchmarks.
The quoted 23-minute Linux step was overwhelmingly execution, not compilation.
Compiler-mutation gates were separate steps and are not included in this table.

On Linux, library execution began at 11:24:14 UTC. All tests except the two below
had completed by 11:32:36. The ambiguity matrix completed at 11:40:18, and the
fault matrix finally completed at 11:46:46. Those two tests alone extended the
wall-clock tail by 14m 10s after every other test had finished. This is an observed
timeline, not a promise that deleting them saves exactly that much on every host.

## Two test functions hide hundreds of integration scenarios

- `root_fault_matrix::every_root_physical_and_successor_boundary_recovers_without_repeating_mutation`:
  336 serial scenarios, each creating a fresh root/member workspace and driving
  preservation, an interruption, and recovery.
- `root_ambiguity_matrix::every_root_phase_rejects_fresh_ambiguous_work_without_physical_execution`:
  220 serial scenarios, each constructing a workspace, reaching a phase,
  introducing ambiguity, testing refusal, then recovering all the way to terminal.

Together these create 556 workspace fixtures, 1,112 Git repositories and over
4,000 fixture commits before the recovery work itself. Cargo parallelizes test
functions; it does not parallelize the loops inside either function. See
[the source audit](GwzTestSpeedAudit.md) for fixture paths and reduction options.

## Measurement method

Cargo was first run with `test --locked --no-run --message-format=json` to separate
build time. The already-built core library test executable was then run with
Rust's built-in libtest timing output:

```sh
RUSTC_BOOTSTRAP=1 <core-test-executable> \
  --format=json --report-time -Z unstable-options
```

The environment flag enables libtest's experimental output format for this
profiling invocation only. It was not used during compilation, added to CI, or
used to change product code. No nextest installation or custom test instrumentation
was needed. The local run used normal parallelism and did not invoke compiler-
mutation suites. Local macOS timings must not be presented as Linux CI timings.

Two five-second macOS `sample` traces at 10 ms intervals showed filesystem operations
at the top of sampled stacks: file open, fcntl, unlink, rename, mkdir and write.
Root-matrix stacks included fixture construction, real Git operations, journal
serialization and `File::sync_all`. This is a short sample, not a whole-run CPU
percentage attribution. Sampling adds some overhead to the local run.

Evidence: [CI timing excerpts](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/test-performance/runs/test-performance-20260907/ci-timing-excerpts.json)
[stack summary](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/test-performance/runs/test-performance-20260907/stack-sample-summary.txt),
and [tail stack summary](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/test-performance/runs/test-performance-20260907/tail-stack-summary.txt).

## Local measured results

macOS, Rust 1.95.0, debug test profile, 12 logical CPUs/default test parallelism.
Build-only wall time: **22.93 seconds** (incremental). The library run completed
in **780.55 seconds (13m 0.55s)**: 1,866 passed, zero failed, one ignored helper.
Median per-test duration was **0.159 seconds**. No compiler-mutation suite ran.

| Test (short module name) | Duration |
| --- | ---: |
| `root_fault_matrix::every_root_physical_and_successor_boundary_recovers_without_repeating_mutation` | 695.98s |
| `root_ambiguity_matrix::every_root_phase_rejects_fresh_ambiguous_work_without_physical_execution` | 471.36s |
| `root_successor_matrix::every_root_phase_durable_successor_restarts_without_repeating_the_phase` | 161.27s |
| `service_fault_matrix::every_emitted_rollback_physical_and_successor_boundary_recovers_exactly_once` | 159.36s |
| `prefix_drift::completed_participant_drift_blocks_terminal_exhaustion_for_every_request` | 63.38s |
| `prefix_drift::completed_participant_drift_blocks_every_later_owner_and_exhaustion` | 59.98s |
| `service_ambiguity_matrix::every_rollback_action_retains_its_journal_on_fresh_ambiguous_observation` | 58.98s |
| `root_successor_matrix::every_legal_root_handoff_form_exhausts_the_complete_phase_graph` | 57.46s |
| `mutation::every_physical_phase_row_is_before_after_and_restart_safe` | 27.67s |
| `service_durability::checked_rollback_consumers_recover_across_both_durability_sides` | 21.25s |

These test durations overlap because the runner executes test functions in
parallel. Do not add them to calculate wall-clock time. The root fault matrix
alone took 11m 36s, despite the typical test taking a fraction of a second.

Full per-test durations: [CSV](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/test-performance/runs/test-performance-20260907/macos-test-times.csv).
Run metadata and top tests: [JSON](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/test-performance/runs/test-performance-20260907/macos-summary.json).

## Where to make the next change

Start with the root fault and ambiguity matrices, then the root successor and
rollback fault matrices. Keep small named native regression cases in the normal
suite and move exhaustive combinations to the explicit extended suite. For the
ambiguity matrix, avoid completing an entire recovery lifecycle after every
refusal when dedicated recovery tests already exercise that path. This is a
focused workload reduction, not a reason to replace or redesign every test.

This turn measured and documented the workload; it did not change test behavior,
remove the matrices, dispatch CI, or resume a release. The historical CI timings
and new local timings identify the same bottlenecks, but are different hosts and
must not be combined into a promised cross-platform speedup.

## Shared repository implementation — in progress, 2026-09-07

The existing broad GitBackend contract is now named GitRepository, and its
physical implementation Git2Repository. The old names are compatibility aliases,
not additional interfaces. A single cfg(test) FakeGitRepository holds repository
state in memory; ordinary temporary files supply its worktree. Unsupported
operations explicitly fail rather than pretending to succeed.

Four parameterized scenarios currently run with both implementations: index vs
worktree commit snapshots; backup-ref idempotence and drift refusal; checked
reset with dirty-work refusal; and preservation-stash capture, retry, fresh-work
refusal and restoration of staged/unstaged/untracked bytes. These eight runs took
0.08 seconds locally, excluding compilation. This is NOT a new whole-suite timing
and does not establish the projected matrix speedup.

Preservation stash observations now pass through GitRepository, including bundle
construction. The native implementation reaches the same protected read-only
leaf; existing production authority remains sealed. The boundary check was
updated for this deliberate interface change. The compiler mutation probes were
not run.

Remaining implementation checklist:

- Admit the shared fake to the test-only authority boundary and parameterize the
  two large root matrices plus their fixture/runtime wrappers.
- Finish fake scoped candidate commits, repository-state/checkout observations,
  and managed-root/index preparation, observation and mutation transitions.
- Replace native fixture initialization/commit helpers and direct ref edits with
  the shared repository implementation. Keep real temporary directories and
  CheckedV1Store journal/lease/durability behavior: its filesystem journals remain real. Subsequent execution found native Git
  discovery in runtime bootstrap and pre-catalog observations; the first
  migration batch routes those reads through GitRepository. The boundary file .git/info/exclude can exist without
  initializing a Git repository.
- Run the same matrix bodies with either backend, keep a small physical default
  cross-check, and expose exhaustive physical runs explicitly.
- Measure the converted matrices before extending the conversion elsewhere.

The original matrices are still intact and still use real Git. No suite-time
reduction has yet been measured. Nothing has been released or pushed.

### Repository factory — 2026-09-08

`gwz_core::git::make_repository()` has separate production and unit-test
implementations. Production constructs Git2Repository directly and contains no
mode selector. The unit-test build reads GWZ_TEST_GIT once per process using
OnceLock: `fake` (default) or `real`; any other value fails explicitly. All
factory calls, including calls from worker threads, use the same mode. Fake
handles share a mutex-protected repository map so reopening through another
factory call sees the same state.

All four repository contract scenarios use the factory directly. Each scenario
has one behavior-named test function, with no separate fake/physical wrappers:
index versus worktree commits; backup-reference drift and idempotence; checked
reset; and preservation stash capture and restoration.

Run the same four tests in both modes from gwz-core:

```sh
GWZ_TEST_GIT=fake cargo test --locked --lib repository_contract_tests
GWZ_TEST_GIT=real cargo test --locked --lib repository_contract_tests
```

The environment variable only affects unit-test code that uses the factory.
Explicit native/fake constructors elsewhere have not been migrated. Integration
tests compile the library in production mode and always get real Git through
this factory. The large matrix conversion remains separate work; these four
scenarios do not establish a whole-suite speedup.

### Root-matrix migration — 2026-09-08 (first batch verified)

Both root matrices now use the factory and fixture interface, with the same
scenario loops and recovery assertions. The fake supplies Git state; the existing
preservation protocol, filesystem journals and directory identity checks still
execute. A normal test runner selects fake mode for these matrices and native
mode for the remaining suite, with the repository contract tests in both modes.
This supersedes the earlier unmigrated-matrix status above.

The complete fault/recovery and ambiguity matrices passed in both modes, without
truncating their scenario loops. The native bootstrap, linked-worktree and
pre-catalog checks passed (28 tests; one existing subprocess helper remains
ignored). Seven repository contract scenarios and one complete root-preservation
scenario with fresh factory handles pass in both modes. The latter runs routinely
with native Git as the inexpensive cross-check (1.39s fake, 1.78s native in these
runs). The native root-stash round trip also passes. Formatting, production/test
Clippy, the structural boundary check, and release-workflow checks passed.

| Validation run | Result | Test execution time |
| --- | --- | --- |
| Full root fault/recovery + ambiguity matrices, fake Git | Passed | 629.01s (10m 29s) |
| Same full matrices plus the seven small repository contracts, native Git | Passed | 858.11s (14m 18s) |

These were overlapping validation runs on this Mac, with compilation and other
focused checks occurring during the run. They are not an isolated benchmark or
a fresh whole-suite measurement. The runs used migration validation builds;
subsequent cleanup restored the original native stash entrypoint through trait
dispatch and was checked with focused root-preservation tests in both modes.
The timings demonstrate that substantial cost remains; they do not establish a
precise final-build speedup or justify extrapolating a CI suite runtime.

Early fake runs were stopped after finding that the fake walked ignored journal
directories before filtering them. The measured fake run prunes ignored
directories while retaining tracked paths within them.

A one-second stack sample of the active fake run found `sync_all`/`fcntl` calls
from the checked-artifact journal and parent-directory synchronization as the
prominent worker stacks. The fake does not create native Git repositories, but
these tests still perform real durable journal writes throughout every recovery
case. This sample identifies a remaining bottleneck; it is not a quantitative
partition of the whole runtime.

The factory conversion therefore has **not** made these matrices quick. The next
performance decision is to exercise the broad recovery combinations against the
store abstraction, retaining a small physical journal/durability cross-check.
That is separate from adding more Git behavior to the fake. The current batch
preserves the approved real-journal behavior and does not replace filesystem
proofs with successful stubs.

Routine command from gwz-core: `python scripts/run_tests.py`. Full converted-group
comparison: `python scripts/run_tests.py --compare`. Native unconverted filters
need `GWZ_TEST_GIT=real`; integration tests always use the production factory.
No release, tag, push or checkpoint commit was performed in this batch.

## Isolated fake-matrix profile — 2026-09-08

The current fake matrices passed in **459.57s (7m40s)**; external elapsed time
was 459.59s. Compilation was completed beforehand, and neither the native
comparison nor another suite ran concurrently. This supersedes the earlier
629.01s run as the useful current timing: that earlier measurement overlapped
other workloads and used the migration validation build. The difference is
**not a measured optimization gain**. This run includes sampling overhead.

The ambiguity matrix finished at approximately 302.9s (5m03s), observed by a
one-second output poll. The fault matrix determined the final elapsed time.
Process CPU time was 391.23s user + 50.97s system. These are concurrent worker
CPU totals, so subtracting them from elapsed time would not measure I/O waits.

### Where the time goes

macOS `sample` captured 32,908 matrix-worker stacks across 15 successive windows
at a requested 20ms interval (30 seconds per window, final window shorter).
The idle test-harness thread is excluded. A separate initial 10-second,
10ms-interval exploratory sample is retained but excluded from these totals.

| Exclusive sampled worker activity | Fault matrix | Ambiguity matrix |
| --- | ---: | ---: |
| File/directory durability synchronization | 38.0% | 37.9% |
| YAML parsing, serialization, and value processing | 31.5% | 31.0% |
| Remaining work under the fake repository | 8.3% | 5.3% |
| Other kernel operations/waits | 5.0% | 7.4% |
| Remaining user-space work | 17.2% | 18.3% |

These are stack-sampling estimates, not exact invocation counts or directly
additive portions of the suite's wall clock. Nested YAML and synchronization
are assigned to those categories before fake Git. The fake's shared root
preservation implementation still performs actual filesystem operations.

The main synchronization callers are:

- `checked_artifact/platform.rs:678`, `sync_parent`: directory durability for
  checked artifacts (12.6% of all sampled worker stacks).
- `durable_fs.rs:30`, `sync_dir`: chiefly the journal's post-rename directory
  synchronization (10.4%).
- `workspace_ops/merge/v1_lifecycle/store/rewrite.rs:124`: journal temporary-file
  synchronization (9.8%).
- Checked-artifact residue publication, rebarriering, and other writers make
  up the remaining synchronization samples.

`CheckedV1Store` journal commit is on **57.7% of fault-worker stacks and 60.8%
of ambiguity-worker stacks**, including its nested synchronization and CPU
work. This overlaps the table; it is not an extra cost to add.

Source inspection explains the CPU amplification:

1. `store/rewrite.rs:110` decodes and validates the current on-disk record.
2. `:124` writes and synchronizes the replacement; `:131` decodes and validates
   the reread replacement.
3. `:152` renames it; `:159` synchronizes its directory; `:163` decodes and
   validates the published bytes again.
4. At the time of this profile, each `StoredV1Record::from_open_bytes` called
   `decode_production_v1`, whose `decode_v1_body` already validated the record,
   and then validated the same model again. A successful normal journal commit
   therefore performed **three full decodes and six full record validations**,
   before any additional service loads/reloads. The function-level follow-up
   below removes the second validation by retaining the decoder's validated
   wrapper; the commit now performs three decodes and three validations.
5. Record validation reparses embedded lock, manifest and marker YAML and
   recalculates hashes. For example, `model/v1/validate/acceptance.rs` parses
   metadata and accepted lock rows, and `acceptance/publication.rs:210` parses
   candidate metadata. SHA-256 appears in about 14% of worker stacks overall;
   this inclusive metric overlaps validation and fake Git.

Fixture construction accounts for **1.5% of fault-worker samples and 1.8% of
ambiguity-worker samples**. Sampled mutex waiting is 0.13% and 0.32%, respectively.
Neither fixture creation nor fake repository lock contention explains the
remaining minutes.

### Repeated lifecycle work inside the matrices

The ambiguity matrix's sampled worker time divides as follows:

- **32.5%**: execute recovery up to the target phase (`root_ambiguity_matrix.rs:33`).
- **4.3%**: execute the rejection request being checked (`:64`).
- **60.1%**: after removing the ambiguity, execute recovery to completion (`:90`).
- **3.1%**: fixture construction and other test work.

That complete prefix/rejection/suffix sequence is repeated across all 220
combinations. The fault matrix likewise repeatedly executes a prefix up to its
injected fault (35.6%) and the full recovery afterward (61.8%) across its 336
combinations. These are valid end-to-end assertions, but this breadth also
repeats journal persistence and validation hundreds of times.

The next speed work should target:

1. Remove redundant record validation while retaining exact-byte drift checks.
   This is now complete; removing whole decodes requires a separate proof that
   the staged and published bytes retain their exact checked meaning.
2. Reduce repeated full lifecycle traversal in the broad decision matrices;
   retain representative physical journal/recovery coverage and focused checks
   for every required decision. Avoid simply removing durability from the
   production implementation or silently weakening recovery assertions.
3. Measure these changes before expanding another abstraction: Git substitution
   alone leaves the real journal and checked-artifact work measured here.

No production source, test behavior, or durability semantics were changed by
this profiling task. Full results, compressed raw stacks, the summarizer and
methodology are in
[evidence/test-performance-20260908-profile](https://github.com/owebeeone/gwz-core-evidence/blob/main/campaigns/test-performance/runs/test-performance-20260908-profile/README.md).

The proposed next implementation is documented in
[FileSystem interface and test implementation](GwzFileSystemTestInterface.md).
It covers the complete filesystem boundary, including fake Git's worktree,
fixtures, locks and checked artifacts, followed by a separately measured removal
of duplicate record decoding/validation. The filesystem foundation and first
journal file consumers are now implemented; see that plan for focused checks
and the remaining work. The status below supersedes this original projection.

## Post-migration full-suite profile — 2026-09-08

After the legacy checked-artifact entry doors were moved to the shared
`FileSystem` implementation, the serial rollback fault test was divided by
owner and the root fault and ambiguity loops were divided into independently
scheduled tests. Scenario coverage was retained.

| Group | Before | After |
| --- | ---: | ---: |
| Reverse rollback, fake Git/filesystem | 13.53s | 3.42s focused; 3.79s in the final group run |
| Root fault and ambiguity matrices, fake Git/filesystem | 240.34s | 79.82s |

The first corrected full runner took **337.84s**: 74.93s for the migrated fake
phase and 262.15s for the remaining native phase. All 1,899 native library tests
completed in 234.18s when run directly without compilation or integration
binaries. Their summed test durations were 2,428.15s because libtest runs them
concurrently; the median was 0.156s.

The longest native tests were still migrated lifecycle matrices: the root
successor loop took 142.32s, three rollback loops took 57.93–60.01s, and the
split rollback fault targets took 29.51–36.81s. The runner now executes the root
successor and durability matrices and complete rollback suite in fake mode,
retains one explicit native rollback regression, and omits those groups from
the native remainder.

The resulting full runner passed in **245.52s**, a reduction of **92.32s
(27.3%)** from the immediately preceding run. Its migrated phase took 89.47s
for 67 tests; the native remainder and integration binaries took 154.74s.

A symbolicated ten-second macOS `sample` spot check contained many filesystem
leaves. The `fcntl` cases inspected lead through GWZ's `File::sync_all` paths,
and the large `unlinkat` cases lead through recursive temporary-fixture deletion.
The tool's collapsed-leaf report omitted small entries and combined samples from
many concurrent test threads, however, so summing selected leaf names and
dividing by that report's subtotal does not measure their share of the whole
run. The whole-run profiler view puts `fcntl` at roughly 5% of samples. The spot
check therefore establishes that physical synchronization is active, not that
filesystem I/O dominates total runtime.

The exact timings expose the next physical test tier, but do not attribute its
cost. Checked-artifact restart and fault matrices commonly take 10–22s each,
including managed intent, admission, namespace barrier/cleanup, authority record
and catalog bootstrap groups. Moving catalog activation and its retained
pre-catalog provider onto `FileSystem` remains the next migration candidate; its
speed effect must be measured rather than inferred from the spot sample.

## Function-level follow-up — 2026-09-08

Two representative profiles separate the remaining costs.

The native Git-directory managed-intent test took 8.73s in isolation but 22.23s
inside the concurrent native library run. Its retained catalog path repeatedly
enters `recover_or_create`, `preflight_catalog_target`, lease/root revalidation
and `repository_paths`. In one 2,922-sample worker capture, repository discovery
and libgit2 repository/config loading appeared in roughly 800 inclusive stacks.
`File::sync_all` appeared in 784 inclusive stacks through checked-artifact file
and parent-directory barriers. Inclusive stacks overlap and must not be added;
the result identifies repeated repository relationship discovery and durability
barriers as this test's concrete native work. Four managed-intent tests took
28.37s serial and 13.30s with four test threads, so concurrency helps this group
until it competes with the much larger native suite.

The fake root-successor test was a different problem. It took 30.57s alone with
30.18s of user CPU. An eight-second sample put 4,728 of 6,432 active worker
samples under `CheckedV1Store::commit` and 4,677 under
`StoredV1Record::from_open_bytes`; YAML parsing, full record validation and
SHA-256 dominated the leaves. The test serially covered two owners by 22 phase
targets. Exposing each owner/phase and owner/handoff case as its own Rust test
reduced the complete root-successor module from about 39s serial to 6.50s with
12 test threads, without removing a scenario.

A whole fake-phase sample then showed all ten root-ambiguity request tests inside
their 22-phase `assert_ambiguous_request` loops. Across the concurrent capture,
`CheckedV1Store::commit` occupied 42,100 inclusive samples,
`decode_production_v1` 26,473 and `validate_v1_record` 29,487. This phase is CPU
bound even with fake Git and filesystem: the unoptimized run used 615.37 user
CPU-seconds and took 85.72s wall time.

Rust's default test profile leaves this protocol parsing, validation and hashing
unoptimized. Test-profile optimization level 1 preserves debug assertions and
symbols while changing the measured costs as follows:

| Measurement | Unoptimized | Test opt-level 1 |
| --- | ---: | ---: |
| One root-ambiguity request across 22 phases | 13.72s | 2.27s |
| Complete migrated fake phase | 85.72s | 14.02s after single-validation decode |
| Four native managed-intent tests | 13.30s | 12.29s |

The first ordinary runner after changing the profile passed, but took 247.46s
because Cargo spent 76.69s rebuilding the optimized test graph. A direct warm
confirmation then passed in **176.00s**, including 157.50s for the native
remainder. This is 69.52s (28.3%) faster than the preceding 245.52s warm full
runner, and 161.84s (47.9%) faster than the 337.84s runner before migrated groups
were assigned to the fake backends. The remaining speed target is the native
catalog revalidation chain; test-profile optimization intentionally has little
effect on its physical I/O and synchronization.

`DecodedV1Record` now retains the `ValidatedV1Record` produced by
`decode_production_v1`, and `StoredV1Record::from_open_bytes` consumes that proof
instead of calling `validate_v1_record` again. Decoder behavior and exact-byte
checks are unchanged. The representative ambiguity test fell from 2.27s to
1.59s under the optimized profile, and the complete fake phase fell from
16.61–18.01s to **14.02s**.
