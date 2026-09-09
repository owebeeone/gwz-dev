# GWZ remaining debt: scoped inventory, 2026-09-09

Campaign evidence and experiment runners referenced here moved to the private [GWZ archive](https://github.com/owebeeone/gwz-core-evidence) on 2026-09-10. Recorded paths and commands remain historical; see the archive README and REPLAY.md for current locations and fresh-run setup.

This is a decision aid after v1.0.8, not a new implementation authorization or a
claim that every item below must go into the next release. The release passed;
these are consistency, maintenance and test-cost follow-ups. No production code
or tests were changed for this inventory.

## Baseline and counting rules

- Core checkout and v1.0.8: `ec6d1a5749c379fbd78d510c160b372f79bdda83`.
- CLI main: `70dfe7e8fc3a75047417d59e853addc29660747a`; release tag:
  `4f31f0b0ae042caed506a7d2564b753f5f657049`.
- Python main: `9822766ee7d79dbc505281ba780a2194ea6094f7`; release tag:
  `31148763e92851b9f9a29791067c34c139065808`.
- Source inventory uses tracked Rust files in the current checkouts: 785 core,
  120 CLI and 16 Python files. Core includes its local-clone support crates.
  Paths below are relative to `gwz-dev`; line numbers belong to this snapshot.
- Factory counts are lexical **call sites**, not imports, definitions, test
  counts or runtime executions. Comments/literals are ignored using the existing
  filesystem guard's tokenizer. Test-only items and external test modules are
  classified separately; platform-disabled production branches remain visible.
- This is not Rust name resolution. The additional wrapper-caller table filters
  known unrelated standard-library/native names and test modules; alias-only,
  function-pointer and transitive callers can still extend that migration.
- Conditional counts are source sites with `cfg`/`cfg_attr`, including test-only
  and inactive platform code. They are scoping data, **not a pinned-count gate**.
  The inventory does not claim that all conditional attributes are violations.
- The C-style scope is tracked handwritten code in these repos and the workspace
  helper. No claim is made about unrelated projects or all languages on this Mac.

## Work that is already done

`gwz-core/src/operation_context.rs` provides owned `OperationServices`, borrowed
`BorrowedOperationContext`, and test-only `TestWorld`. `for_merge(backend)` shares
the admitted backend's services; it should not be mechanically replaced with a
new independent context. `existing()` and `native()` here are already test-only.
The similarly named request/event context is a different concern.

V1 merge lifecycle/catalog operations carry services through the operation.
Several other entry points have also moved since the older plan was written:
`workspace_ops/handle_branch.rs`, `handle_commit.rs`, `handle_stage.rs`,
`handle_stash.rs`, `handle_tag.rs`, `handle_repo_lifecycle.rs`,
`handle_create_repo.rs`, `handle_materialize.rs`, `handle_init_from_sources.rs`,
`handle_remote_identity.rs`, and `workspace_bootstrap.rs` obtain backend-owned
services. This does **not** prove their entire downstream call chains are closed:
materialize, for example, still calls ambient artifact wrappers listed below.

Git/filesystem contracts in `src/git/gitbackend/repository_contract_tests.rs` and
`src/filesystem/filesystem_contract_tests.rs`, plus
`src/operation_context/preservation_tests.rs`, already demonstrate owned worlds.
Use those as patterns. Native adapter construction at the operation boundary and
inside an explicitly native test world is intentional.

Core, CLI and Python **v1.0.8** release workflows passed. v1.0.6 exposed the fake
Windows volume-ID type mismatch; v1.0.7 exposed two slash-dependent assertions.
Those are fixed. The two orphaned `OsStr` import attributes that broke v1.0.5 are
also fixed. None belongs on the remaining implementation list.

## Choices for the next batch

| Batch | Measured scope | What closes it | Relative size / dependencies |
| --- | --- | --- | --- |
| A. Conditional imports in CLI | 3 sites: `src/globalargs.rs:8`, `src/globalargs/invocation.rs:1`, `src/globalargs/parser.rs:1` | Imports are enclosed, semantics and visibility preserved | Small independent pilot; needs the same rule as core |
| B. Conditional imports in core | 105 sites in 55 files | Enclose platform/test import sections; preserve import visibility and module paths | Mechanical but spread out; no context migration dependency |
| C. C++ helper braces | 83 unbraced control-flow bodies in one handwritten file | Brace every body; preserve behavior; syntax-check on Linux | Small surface, privileged helper: do not combine with functional/security changes |
| D. Reverse-rollback fixtures | 120 sites across 12 files: 73 direct factory calls plus 47 helper calls | One owned world per fixture; every helper/reopen uses that world | Medium, coherent first context batch; shared fixture setup is the key |
| E. Reverse-preservation fixtures | 21 direct factory calls in 5 files | Pass owned services through fixture construction and matrix helpers | Medium; keep deliberate native/fake contract comparisons |
| F. Remaining context cleanup | Overall 14 production factory calls in 6 files; 146 test/support calls in 39 files | Close wrappers and their callers; migrate tests; remove ambient compatibility state last | Larger; six wrapper files alone are not the full call-chain scope |
| G. Conditional-scope enforcement | New fast checker + fixtures + validation wiring | Report file/line violations across inactive branches, without count pins or compiler mutation | Bounded tooling task; define accepted forms and generator treatment first |
| H. Windows test cost | Main native test pass: 1,828 passing tests, 1 ignored, 1,386.96 seconds | Profile that pass, then choose measured hot groups for conversion | High potential benefit, not yet a quantified speedup promise |

Start with A or C for a small mechanical change. D is a coherent next ownership
migration. G prevents recurrence but must account for the existing inventory;
introducing a guard does not automatically make the legacy source compliant.
The figures overlap where stated and must not be added as independent file totals.

## Context migration: exact remaining sites

### Production factories: 14 calls, all `make_filesystem()`, in 6 files

There are zero direct production `make_repository()` calls in this scan. Its
production factory/export still exists, so absence of in-tree calls is not proof
that removing a public symbol is API-compatible.

CLI and Python have zero direct calls to either factory in their scanned Rust
source. Recheck their consumers when changing public core wrappers; they do not
need an invented factory-replacement batch of their own.

The six files and wrappers are:

- `gwz-core/src/artifact/conf_integrity.rs`: `inspect_conf_integrity`,
  `refresh_conf_integrity_marker`, `canonical_conf_integrity_marker`.
- `gwz-core/src/artifact/mod.rs`: `read_manifest`, `write_manifest`, `read_lock`,
  `write_atomic`, `write_manifest_and_lock`, private `read_to_string`.
- `gwz-core/src/durable_fs.rs`: `rename_noreplace`, `sync_dir`.
- `gwz-core/src/workspace_ops/handle_create_repo.rs`: `read_lock_or_empty`.
- `gwz-core/src/workspace_ops/merge/record_wire/location.rs`:
  `acquire_canonical_merge_locations`.
- `gwz-core/src/workspace_ops/sync_workspace_boundary.rs`:
  `ensure_workspace_exclude`.

Most already have an `_in` sibling accepting the filesystem. Route callers to
that sibling with their existing services. For public wrappers, decide whether
an explicit native convenience API remains supported; do not silently invent a
fake/global mode for public calls. For unused private wrappers, first establish
that they have no callers, then remove them rather than carrying dead compatibility.

| File | Calls | Exact sites (`line: function`) |
| --- | ---: | --- |
| `gwz-core/src/artifact/conf_integrity.rs` | 3 | `127: make_filesystem`, `187: make_filesystem`, `204: make_filesystem` |
| `gwz-core/src/artifact/mod.rs` | 6 | `379: make_filesystem`, `396: make_filesystem`, `416: make_filesystem`, `527: make_filesystem`, `549: make_filesystem`, `634: make_filesystem` |
| `gwz-core/src/durable_fs.rs` | 2 | `12: make_filesystem`, `20: make_filesystem` |
| `gwz-core/src/workspace_ops/handle_create_repo.rs` | 1 | `951: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/record_wire/location.rs` | 1 | `109: make_filesystem` |
| `gwz-core/src/workspace_ops/sync_workspace_boundary.rs` | 1 | `52: make_filesystem` |

### Production caller follow-through: 64 identified sites in 26 files

These call the named wrappers above, including wrapper-to-wrapper calls inside
`artifact/mod.rs`. They are a practical next search list, not 64 independent
operations. They demonstrate why counting only six factory-bearing files would
understate the work. In addition, inspect the non-factory artifact wrappers that
call these functions (for example lock, snapshot and marker publication).
Existing `for_merge` at an entry point does not close these downstream calls.

| File | Calls | Exact sites (`line: function`) |
| --- | ---: | --- |
| `gwz-core/src/artifact/mod.rs` | 6 | `425: write_atomic`, `426: refresh_conf_integrity_marker`, `451: write_atomic`, `486: read_to_string`, `490: write_atomic`, `517: read_to_string` |
| `gwz-core/src/diff/handle_diff.rs` | 1 | `73: read_manifest` |
| `gwz-core/src/local_clone/adapters/disposal.rs` | 1 | `172: read_manifest` |
| `gwz-core/src/local_clone/adapters/generated_marker.rs` | 1 | `130: canonical_conf_integrity_marker` |
| `gwz-core/src/local_clone/adapters/git_config.rs` | 2 | `152: read_lock_or_empty`, `157: ensure_workspace_exclude` |
| `gwz-core/src/local_clone/adapters/install.rs` | 6 | `100: read_manifest`, `119: inspect_conf_integrity`, `550: read_lock`, `573: write_atomic`, `581: refresh_conf_integrity_marker`, `588: inspect_conf_integrity` |
| `gwz-core/src/local_clone/adapters/inventory.rs` | 1 | `70: read_manifest` |
| `gwz-core/src/local_clone/create.rs` | 1 | `200: read_manifest` |
| `gwz-core/src/local_clone/family_merge.rs` | 3 | `277: read_manifest`, `279: read_lock`, `302: read_lock` |
| `gwz-core/src/operation/commit_log/request.rs` | 2 | `65: read_manifest`, `108: read_lock` |
| `gwz-core/src/stash/mod.rs` | 1 | `262: write_atomic` |
| `gwz-core/src/status/status_member.rs` | 2 | `22: read_manifest`, `115: read_lock` |
| `gwz-core/src/workspace_ops/handle_list_snapshots.rs` | 1 | `16: read_manifest` |
| `gwz-core/src/workspace_ops/handle_materialize.rs` | 12 | `45: read_manifest`, `47: read_lock`, `113: read_manifest`, `115: read_lock`, `129: read_lock_or_empty`, `181: read_manifest`, `422: read_lock_or_empty`, `431: read_lock`, `454: read_lock`, `484: read_lock_or_empty`, `641: read_manifest`, `1022: read_lock` |
| `gwz-core/src/workspace_ops/handle_stash/commands.rs` | 1 | `15: read_lock` |
| `gwz-core/src/workspace_ops/merge/gc.rs` | 1 | `205: acquire_canonical_merge_locations` |
| `gwz-core/src/workspace_ops/merge/plan.rs` | 2 | `23: read_manifest`, `25: read_lock` |
| `gwz-core/src/workspace_ops/merge/start/record.rs` | 1 | `59: read_manifest` |
| `gwz-core/src/workspace_ops/merge/status/snapshot.rs` | 1 | `70: acquire_canonical_merge_locations` |
| `gwz-core/src/workspace_ops/merge/store/gc.rs` | 1 | `21: sync_dir` |
| `gwz-core/src/workspace_ops/merge/store/retention.rs` | 1 | `54: sync_dir` |
| `gwz-core/src/workspace_ops/pull_head_member_preflight.rs` | 10 | `77: read_manifest`, `79: read_lock`, `141: read_manifest`, `147: read_lock`, `290: read_manifest`, `291: read_lock_or_empty`, `495: read_manifest`, `502: read_lock`, `529: read_manifest`, `536: read_lock` |
| `gwz-core/src/workspace_ops/push_member.rs` | 1 | `68: read_manifest` |
| `gwz-core/src/workspace_ops/sync_workspace_boundary.rs` | 1 | `26: ensure_workspace_exclude` |
| `gwz-core/src/workspace_ops/target_listing.rs` | 2 | `15: read_manifest`, `20: read_lock` |
| `gwz-core/src/workspace_ops/workspace_bootstrap/conf_gate.rs` | 2 | `94: inspect_conf_integrity`, `108: refresh_conf_integrity_marker` |

### Test and support factories: 146 calls in 39 files

132 call `make_filesystem()` and 14 call `make_repository()`. These include
compatibility implementation code as well as fixtures; they are not 146 tests.
Some files also have production calls, so the production/test file counts overlap.

For a migrated fixture, retain its `TestWorld`/`OperationServices`, pass the same
repository/filesystem through arrange, execute, observe, reopen and cleanup,
and keep the backend choice at world construction. Creating a fresh world on
reopen, or using the old global helpers only during arrangement, defeats the
migration even when the test happens to pass on native storage.

| File | Calls | Exact sites (`line: function`) |
| --- | ---: | --- |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/leaf_observation.rs` | 1 | `628: make_filesystem` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/managed_mutation.rs` | 1 | `399: make_filesystem` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/production_tests.rs` | 1 | `64: make_filesystem` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/tests_admission_spike.rs` | 5 | `84: make_filesystem`, `87: make_filesystem`, `122: make_filesystem`, `172: make_filesystem`, `191: make_filesystem` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/tests_leaf_observation.rs` | 1 | `203: make_filesystem` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/tests_retired_root.rs` | 6 | `297: make_filesystem`, `322: make_filesystem`, `328: make_filesystem`, `331: make_filesystem`, `344: make_filesystem`, `347: make_filesystem` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/tests_terminal_fault_matrix.rs` | 1 | `319: make_filesystem` |
| `gwz-core/src/checked_artifact/catalog/bootstrap/tests.rs` | 1 | `608: make_filesystem` |
| `gwz-core/src/checked_artifact/entry.rs` | 1 | `1067: make_filesystem` |
| `gwz-core/src/checked_artifact/observation.rs` | 1 | `117: make_filesystem` |
| `gwz-core/src/checked_artifact/platform.rs` | 6 | `512: make_filesystem`, `575: make_filesystem`, `667: make_filesystem`, `668: make_filesystem`, `786: make_filesystem`, `787: make_filesystem` |
| `gwz-core/src/checked_artifact/platform/anchor/tests.rs` | 2 | `69: make_filesystem`, `267: make_filesystem` |
| `gwz-core/src/checked_artifact/tests/leaf_publication.rs` | 1 | `368: make_filesystem` |
| `gwz-core/src/filesystem.rs` | 3 | `38: make_filesystem`, `46: make_filesystem`, `69: make_filesystem` |
| `gwz-core/src/git/gitbackend/factory.rs` | 1 | `14: make_repository` |
| `gwz-core/src/git/gitbackend/fake_repository.rs` | 2 | `20: make_filesystem`, `38: make_filesystem` |
| `gwz-core/src/verified_write.rs` | 1 | `108: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/preserve.rs` | 1 | `64: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/record_wire/location.rs` | 5 | `440: make_filesystem`, `447: make_filesystem`, `454: make_filesystem`, `457: make_filesystem`, `469: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/checked.rs` | 2 | `20: make_filesystem`, `20: make_repository` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/store/rewrite.rs` | 1 | `298: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/durable_cursor.rs` | 1 | `354: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/factory_contract.rs` | 9 | `19: make_repository`, `23: make_filesystem`, `25: make_filesystem`, `26: make_filesystem`, `27: make_filesystem`, `32: make_filesystem`, `35: make_filesystem`, `43: make_filesystem`, `47: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/mod.rs` | 9 | `36: make_filesystem`, `43: make_filesystem`, `55: make_filesystem`, `64: make_filesystem`, `68: make_filesystem`, `72: make_filesystem`, `157: make_repository`, `376: make_repository`, `834: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_ambiguity_matrix.rs` | 1 | `251: make_repository` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_fault_matrix.rs` | 1 | `207: make_repository` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/entry.rs` | 10 | `39: make_filesystem`, `119: make_filesystem`, `130: make_filesystem`, `134: make_filesystem`, `147: make_filesystem`, `154: make_filesystem`, `178: make_filesystem`, `202: make_filesystem`, `224: make_filesystem`, `279: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/entry_service.rs` | 9 | `146: make_filesystem`, `167: make_filesystem`, `207: make_filesystem`, `237: make_filesystem`, `247: make_filesystem`, `296: make_filesystem`, `298: make_filesystem`, `436: make_filesystem`, `482: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/entry_service_drift.rs` | 4 | `87: make_filesystem`, `96: make_filesystem`, `99: make_filesystem`, `110: make_repository` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/mod.rs` | 5 | `34: make_repository`, `89: make_repository`, `96: make_filesystem`, `130: make_filesystem`, `151: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/phases.rs` | 10 | `101: make_filesystem`, `121: make_filesystem`, `132: make_filesystem`, `141: make_filesystem`, `154: make_filesystem`, `178: make_filesystem`, `190: make_filesystem`, `219: make_filesystem`, `286: make_filesystem`, `299: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/prefix_drift.rs` | 5 | `36: make_filesystem`, `62: make_filesystem`, `107: make_filesystem`, `134: make_filesystem`, `171: make_repository` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/root_artifacts.rs` | 20 | `17: make_repository`, `21: make_filesystem`, `82: make_filesystem`, `93: make_filesystem`, `103: make_filesystem`, `112: make_filesystem`, `123: make_filesystem`, `133: make_filesystem`, `142: make_filesystem`, `158: make_filesystem`, `161: make_filesystem`, `166: make_filesystem`, `189: make_filesystem`, `198: make_filesystem`, `203: make_filesystem`, `224: make_filesystem`, `239: make_filesystem`, `249: make_filesystem`, `265: make_filesystem`, `276: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/service_durability.rs` | 5 | `101: make_filesystem`, `112: make_filesystem`, `120: make_filesystem`, `133: make_filesystem`, `142: make_filesystem` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/service_fault_matrix/fixtures.rs` | 5 | `58: make_repository`, `132: make_repository`, `137: make_filesystem`, `140: make_filesystem`, `211: make_filesystem` |
| `gwz-core/src/workspace_ops/sync_workspace_boundary.rs` | 1 | `113: make_filesystem` |
| `gwz-core/src/workspace_ops/tests/g11.rs` | 1 | `9: make_filesystem` |
| `gwz-core/src/workspace_ops/tests/g23/crash_recovery.rs` | 5 | `835: make_filesystem`, `842: make_filesystem`, `849: make_filesystem`, `857: make_filesystem`, `865: make_filesystem` |
| `gwz-core/src/workspace_ops/workspace_bootstrap/claude_settings.rs` | 1 | `93: make_filesystem` |

### Indirect test helpers: another 51 calls in 13 files

`write_for_test`, `write_atomic_for_test` and `remove_file_for_test` in
`gwz-core/src/filesystem.rs` call the ambient factory. Their callers must migrate
too. These 51 caller sites are additional to the direct-factory sites above;
the three factory calls inside their definitions are already counted there.
The table does not expand local aliases or wrappers transitively.

| File | Calls | Exact sites (`line: function`) |
| --- | ---: | --- |
| `gwz-core/src/checked_artifact/entry.rs` | 1 | `1069: write_for_test` |
| `gwz-core/src/workspace_ops/merge/record_wire/location.rs` | 3 | `451: write_atomic_for_test`, `464: write_atomic_for_test`, `472: write_atomic_for_test` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/entry.rs` | 7 | `35: write_for_test`, `78: write_for_test`, `100: write_for_test`, `107: write_for_test`, `114: write_for_test`, `176: write_for_test`, `196: write_for_test` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/entry_service.rs` | 5 | `27: write_for_test`, `439: write_for_test`, `444: write_for_test`, `461: write_for_test`, `483: write_for_test` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/entry_service_drift.rs` | 5 | `65: write_for_test`, `74: write_for_test`, `81: write_for_test`, `84: write_for_test`, `102: write_for_test` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/mod.rs` | 4 | `98: write_for_test`, `211: write_for_test`, `212: write_for_test`, `235: write_for_test` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/phases.rs` | 5 | `40: write_for_test`, `214: write_for_test`, `251: write_for_test`, `269: write_for_test`, `278: write_for_test` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/prefix_drift.rs` | 3 | `286: write_for_test`, `289: write_for_test`, `296: write_for_test` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/real_git.rs` | 1 | `108: write_for_test` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/recovery.rs` | 1 | `24: write_for_test` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/root_artifacts.rs` | 7 | `59: write_for_test`, `156: remove_file_for_test`, `185: remove_file_for_test`, `186: write_for_test`, `217: write_for_test`, `246: write_for_test`, `273: write_for_test` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/service_ambiguity_matrix.rs` | 7 | `111: write_for_test`, `140: write_for_test`, `145: write_for_test`, `158: write_for_test`, `172: write_for_test`, `183: write_for_test`, `188: write_for_test` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/service_fault_matrix/fixtures.rs` | 2 | `139: write_for_test`, `212: write_for_test` |

### Compatibility constructors and shared registries: 15 calls in 10 files

All listed calls are test-only. `OperationServices::existing()` always selects
native services; it does not honor a memory world. `factory_test_services()`
constructs services using the ambient factories. Replace callers with explicit
owned-world setup, preserving intentional real-backend tests.

| File | Calls | Exact sites (`line: function`) |
| --- | ---: | --- |
| `gwz-core/src/checked_artifact/bootstrap/runtime/catalog_lease.rs` | 1 | `94: OperationServices::existing` |
| `gwz-core/src/checked_artifact/bootstrap/runtime/mod.rs` | 1 | `92: OperationServices::existing` |
| `gwz-core/src/checked_artifact/bootstrap/runtime/paths.rs` | 1 | `330: OperationServices::existing` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/filesystem.rs` | 2 | `85: OperationServices::existing`, `108: OperationServices::existing` |
| `gwz-core/src/checked_artifact/entry.rs` | 1 | `662: OperationServices::existing` |
| `gwz-core/src/filesystem/factory.rs` | 1 | `11: FakeFileSystem::shared` |
| `gwz-core/src/git/gitbackend/factory/test_repository.rs` | 1 | `20: FakeGitRepository::shared` |
| `gwz-core/src/operation/workspace_mutator_lock.rs` | 1 | `62: OperationServices::existing` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/checked.rs` | 5 | `81: factory_test_services`, `125: factory_test_services`, `181: factory_test_services`, `214: factory_test_services`, `239: factory_test_services` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/store/mod.rs` | 1 | `33: checked::factory_test_services` |

Remove compatibility infrastructure **after** its fixture and wrapper callers:

- `gwz-core/src/filesystem/factory.rs`: test backend selection and
  `FakeFileSystem::shared()` lookup; keep the real adapter available.
- `gwz-core/src/filesystem/fake.rs`: shared `OnceLock` state in `shared()`.
- `gwz-core/src/git/gitbackend/factory.rs` and `factory/test_repository.rs`:
  factory selector/delegation and `FakeGitRepository::shared()` lookup.
- `gwz-core/src/git/gitbackend/fake_repository.rs`: ambient `Default` and
  `shared()` construction; preserve `with_filesystem` for owned worlds.
- Exports/helpers in `gwz-core/src/filesystem.rs` and
  `src/git/gitbackend.rs`; obsolete compatibility constructors in
  `src/operation_context.rs` and `workspace_ops/merge/v1_lifecycle/checked.rs`.

Keep `src/test_backend.rs` selection for `TestWorld::selected()` and the runner's
separate native/memory processes. Removing shared registries is not a request to
remove backend comparison or expose a fake switch in the production binary.

Completion checks: affected tests in memory and native modes, an isolated-world
reopen/cleanup case, and an expanded context boundary guard covering the migrated
consumers. Existing guard: `gwz-core/scripts/checks/check_filesystem_boundary.py`
(`CONTEXT_PROTECTED`, `context_violations`, `production_source`); its own tests:
`gwz-core/scripts/checks/test_check_filesystem_boundary.py` and
`gwz-core/scripts/checks/test_run_tests_filesystem_mode.py`. Do not pin these counts.

## Conditional-compilation and compound-statement rule

The rule is already written in workspace `AGENTS.md` and the user's global agent
instructions. Adoption is complete; old-code migration and automated enforcement
are not. Use bounded `cfg_if!` sections or platform modules, keeping unconditional
imports outside. Preserve paths, visibility, API and platform selection.

### Rust declaration candidates: 258 sites in 114 files

| Construct | Core sites | CLI sites | Python sites | Total |
| --- | ---: | ---: | ---: | ---: |
| Conditional `use` (including grouped imports) | 105 | 3 | 0 | 108 |
| External `mod name;` declarations | 114 | 1 | 0 | 115 |
| Conditional `let` | 24 | 0 | 0 | 24 |
| Conditional non-function `const` | 4 | 0 | 0 | 4 |
| Conditional `type` | 6 | 0 | 0 | 6 |
| Conditional `static` | 1 | 0 | 0 | 1 |

The 108 imports occupy 58 files: 55 core and 3 CLI. All 258 declaration
candidates are outside generated Rust paths in this snapshot. A grouped
`use { ... };` is still an import declaration, not an enclosing platform scope.
External test modules are included: moving them must preserve Rust's module-file
resolution. This is why import-only conversion is the simpler first batch.

The tokenizer observed 1,097 conditional attribute sites overall. Braced
functions, structs, enums, implementations and modules are not blanket defects.
Do not mechanically replace all 1,097 attributes. Besides the declaration table,
231 expression/field/variant/macro sites are listed separately for grammar-aware
review; some already have safe boundaries or require preserving layout/API.
This inventory is not a substitute for the proposed syntax-aware rule checker.

| File | Sites | Attribute lines and following construct |
| --- | ---: | --- |
| `gwz-cli/src/globalargs.rs` | 1 | `8: use` |
| `gwz-cli/src/globalargs/invocation.rs` | 1 | `1: use` |
| `gwz-cli/src/globalargs/parser.rs` | 1 | `1: use` |
| `gwz-cli/src/lib.rs` | 1 | `73: mod` |
| `gwz-core/crates/copy-contract/src/lib.rs` | 1 | `36: mod` |
| `gwz-core/crates/family-store-contract/src/lib.rs` | 1 | `35: mod` |
| `gwz-core/crates/family-store/src/lib.rs` | 2 | `39: mod`, `41: mod` |
| `gwz-core/crates/history-check/src/lib.rs` | 1 | `93: mod` |
| `gwz-core/crates/local-disposal/src/lib.rs` | 1 | `48: mod` |
| `gwz-core/crates/local-import/src/lib.rs` | 2 | `30: mod`, `33: mod` |
| `gwz-core/crates/local-testrepo/src/lib.rs` | 2 | `83: mod`, `93: use` |
| `gwz-core/crates/refcopy/src/lib.rs` | 1 | `147: mod` |
| `gwz-core/crates/refcopy/src/native.rs` | 6 | `45: use`, `47: use`, `55: use`, `57: use`, `281: use`, `1042: mod` |
| `gwz-core/crates/refcopy/src/ordinary.rs` | 2 | `734: let`, `832: mod` |
| `gwz-core/crates/repo-contract/src/lib.rs` | 1 | `33: mod` |
| `gwz-core/crates/repo-factory/src/lib.rs` | 2 | `48: mod`, `51: mod` |
| `gwz-core/crates/repo-inspect/src/lib.rs` | 2 | `61: mod`, `63: mod` |
| `gwz-core/crates/workspace-install/src/lib.rs` | 2 | `53: mod`, `56: mod` |
| `gwz-core/src/checked_artifact/admission/driver.rs` | 1 | `229: mod` |
| `gwz-core/src/checked_artifact/admission/mod.rs` | 3 | `57: mod`, `59: mod`, `61: mod` |
| `gwz-core/src/checked_artifact/bootstrap.rs` | 1 | `16: use` |
| `gwz-core/src/checked_artifact/bootstrap/managed.rs` | 4 | `34: mod`, `41: mod`, `43: mod`, `45: mod` |
| `gwz-core/src/checked_artifact/bootstrap/runtime/catalog_lease.rs` | 4 | `4: use`, `18: use`, `25: use`, `400: mod` |
| `gwz-core/src/checked_artifact/bootstrap/runtime/mod.rs` | 2 | `15: mod`, `17: mod` |
| `gwz-core/src/checked_artifact/capability.rs` | 3 | `25: use`, `27: use`, `34: use` |
| `gwz-core/src/checked_artifact/capability/pre_catalog.rs` | 5 | `16: use`, `25: use`, `27: use`, `29: use`, `55: use` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider.rs` | 15 | `112: use`, `159: use`, `161: use`, `167: use`, `347: mod`, `349: mod`, `351: mod`, `353: mod`, `355: mod`, `357: mod`, `359: mod`, `361: mod`, `363: mod`, `366: mod`, `369: mod` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/admission_mutation.rs` | 3 | `32: use`, `183: let`, `637: let` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/authority_record_binding.rs` | 1 | `81: use` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/barrier_mutation.rs` | 1 | `170: use` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/directory_mutation.rs` | 1 | `38: type` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/filesystem.rs` | 2 | `4: use`, `6: use` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/index.rs` | 1 | `3: use` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/leaf_observation.rs` | 1 | `44: use` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/managed_mutation.rs` | 2 | `38: use`, `59: use` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/mutation.rs` | 1 | `28: type` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/namespace_mutation.rs` | 2 | `52: use`, `280: let` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/platform.rs` | 1 | `7: use` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/production_tests.rs` | 1 | `13: use` |
| `gwz-core/src/checked_artifact/catalog/bootstrap.rs` | 1 | `338: mod` |
| `gwz-core/src/checked_artifact/coordinator/mod.rs` | 1 | `53: mod` |
| `gwz-core/src/checked_artifact/fault.rs` | 4 | `1: use`, `83: type`, `86: type`, `139: let` |
| `gwz-core/src/checked_artifact/fault_v1.rs` | 1 | `202: type` |
| `gwz-core/src/checked_artifact/mod.rs` | 5 | `97: use`, `106: use`, `113: use`, `166: mod`, `170: mod` |
| `gwz-core/src/checked_artifact/namespace/mod.rs` | 9 | `54: mod`, `56: mod`, `59: mod`, `62: mod`, `64: mod`, `66: mod`, `68: mod`, `92: use`, `419: use` |
| `gwz-core/src/checked_artifact/platform.rs` | 1 | `77: mod` |
| `gwz-core/src/checked_artifact/platform/anchor.rs` | 1 | `564: mod` |
| `gwz-core/src/checked_artifact/protocol/admission.rs` | 2 | `16: mod`, `19: use` |
| `gwz-core/src/checked_artifact/protocol/authority_record.rs` | 2 | `37: use`, `47: use` |
| `gwz-core/src/checked_artifact/protocol/infrastructure_record/owner.rs` | 2 | `13: mod`, `16: use` |
| `gwz-core/src/checked_artifact/protocol/managed_bootstrap_record/codec.rs` | 1 | `12: use` |
| `gwz-core/src/checked_artifact/tests/recovery_protocol.rs` | 3 | `41: let`, `212: let`, `247: let` |
| `gwz-core/src/checked_artifact/transition.rs` | 2 | `1: use`, `12: static` |
| `gwz-core/src/diff/mod.rs` | 2 | `22: mod`, `40: use` |
| `gwz-core/src/diff/render/mod.rs` | 1 | `64: mod` |
| `gwz-core/src/diff/tests/t_mode.rs` | 1 | `4: use` |
| `gwz-core/src/filesystem.rs` | 2 | `15: mod`, `17: mod` |
| `gwz-core/src/filesystem/fake.rs` | 4 | `829: let`, `834: let`, `840: let`, `850: let` |
| `gwz-core/src/filesystem/native.rs` | 8 | `11: use`, `14: use`, `66: let`, `68: let`, `226: let`, `229: let`, `261: let`, `1001: let` |
| `gwz-core/src/filesystem/native/facts.rs` | 8 | `1: mod`, `3: use`, `5: mod`, `7: use`, `9: mod`, `11: use`, `13: mod`, `15: use` |
| `gwz-core/src/filesystem/native/legacy_identity.rs` | 2 | `24: const`, `26: const` |
| `gwz-core/src/git/gitbackend.rs` | 16 | `4: use`, `41: use`, `43: use`, `55: use`, `57: use`, `59: use`, `61: use`, `63: use`, `65: use`, `392: mod`, `395: mod`, `397: use`, `400: mod`, `402: use`, `404: mod`, `407: use` |
| `gwz-core/src/git/gitbackend/backend.rs` | 1 | `1: use` |
| `gwz-core/src/git/gitbackend/factory.rs` | 2 | `17: mod`, `20: use` |
| `gwz-core/src/git/gitbackend/preservation.rs` | 1 | `630: type` |
| `gwz-core/src/git/gitbackend/preservation_image.rs` | 2 | `551: let`, `553: let` |
| `gwz-core/src/git/gitbackend/preservation_root.rs` | 1 | `11: use` |
| `gwz-core/src/git/mod.rs` | 1 | `5: mod` |
| `gwz-core/src/git/tests/g15/root_preservation.rs` | 1 | `5: mod` |
| `gwz-core/src/git/tests/g15/root_preservation/faults.rs` | 2 | `116: let`, `180: let` |
| `gwz-core/src/git/tests/g15/root_preservation/stash.rs` | 1 | `360: let` |
| `gwz-core/src/lib.rs` | 2 | `32: mod`, `66: mod` |
| `gwz-core/src/local_clone/mod.rs` | 1 | `56: mod` |
| `gwz-core/src/model/mod.rs` | 1 | `623: mod` |
| `gwz-core/src/operation/commit_log/handler.rs` | 1 | `8: use` |
| `gwz-core/src/operation/commit_log/mod.rs` | 6 | `20: use`, `26: use`, `318: let`, `507: mod`, `509: mod`, `511: mod` |
| `gwz-core/src/operation/operation_runtime.rs` | 2 | `4: use`, `6: use` |
| `gwz-core/src/operation_context.rs` | 4 | `2: use`, `4: use`, `7: use`, `292: mod` |
| `gwz-core/src/status/workspace_path.rs` | 1 | `1: use` |
| `gwz-core/src/workspace_ops/merge/acceptance/v1_candidate.rs` | 1 | `11: use` |
| `gwz-core/src/workspace_ops/merge/mod.rs` | 4 | `28: use`, `31: use`, `41: use`, `43: use` |
| `gwz-core/src/workspace_ops/merge/model/mod.rs` | 1 | `17: mod` |
| `gwz-core/src/workspace_ops/merge/model/v1/mod.rs` | 2 | `11: use`, `15: mod` |
| `gwz-core/src/workspace_ops/merge/model/v1/validate/mod.rs` | 10 | `48: mod`, `50: mod`, `52: mod`, `54: mod`, `56: mod`, `58: mod`, `60: mod`, `62: mod`, `64: mod`, `66: mod` |
| `gwz-core/src/workspace_ops/merge/participant_semantics/continue_eligibility.rs` | 1 | `142: mod` |
| `gwz-core/src/workspace_ops/merge/participant_semantics/result.rs` | 2 | `1: use`, `139: mod` |
| `gwz-core/src/workspace_ops/merge/participant_semantics/rollback.rs` | 1 | `212: mod` |
| `gwz-core/src/workspace_ops/merge/participant_semantics/status.rs` | 1 | `486: mod` |
| `gwz-core/src/workspace_ops/merge/plan.rs` | 1 | `421: mod` |
| `gwz-core/src/workspace_ops/merge/preserve.rs` | 1 | `20: use` |
| `gwz-core/src/workspace_ops/merge/record_wire/archive/mod.rs` | 1 | `261: mod` |
| `gwz-core/src/workspace_ops/merge/record_wire/mod.rs` | 4 | `31: use`, `33: use`, `41: use`, `65: mod` |
| `gwz-core/src/workspace_ops/merge/record_wire/unknown_fields/mod.rs` | 1 | `215: mod` |
| `gwz-core/src/workspace_ops/merge/runtime/mod.rs` | 1 | `4: mod` |
| `gwz-core/src/workspace_ops/merge/status/mod.rs` | 1 | `6: mod` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/archive.rs` | 2 | `325: mod`, `329: mod` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority.rs` | 2 | `731: use`, `737: use` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe.rs` | 1 | `25: use` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse.rs` | 3 | `11: use`, `19: use`, `23: use` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation.rs` | 1 | `17: use` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/checked.rs` | 1 | `6: use` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/finalization.rs` | 3 | `66: mod`, `70: mod`, `74: mod` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/mod.rs` | 1 | `22: mod` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/reverse.rs` | 1 | `17: use` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/reverse/preservation.rs` | 1 | `14: mod` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/reverse/rollback.rs` | 1 | `13: mod` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/service.rs` | 2 | `271: mod`, `275: mod` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/status.rs` | 1 | `140: mod` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/transition/effect.rs` | 2 | `3: use`, `346: const` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/transition/mod.rs` | 3 | `6: use`, `9: use`, `16: const` |
| `gwz-core/src/workspace_ops/merge/v1_rollback/mod.rs` | 1 | `25: mod` |
| `gwz-core/src/workspace_ops/mod.rs` | 3 | `31: mod`, `51: use`, `65: use` |
| `gwz-core/src/workspace_ops/pull_head_barrier.rs` | 1 | `3: use` |
| `gwz-core/src/workspace_ops/workspace_bootstrap/claude_settings.rs` | 1 | `19: use` |

### Additional Rust forms to classify before broad enforcement

This table lists the other 231 sites by file and following token. Examples
include test fault-injection calls, conditional enum variants/fields, match arms,
`thread_local!` and generated delegate invocations. Some are braced macro bodies;
others are unbraced statements. Treat them as a review queue, not 231 confirmed
violations. The new checker must make these distinctions explicitly and must
also recognize declaration-only functions where there is no body.

| File | Sites | Attribute lines and following construct |
| --- | ---: | --- |
| `gwz-core/crates/copy-contract/src/contract_tests.rs` | 4 | `334: a_read_only_destination_is_unwritable_and_keeps_an_accurate_partial`, `337: never_hardlinks_source_files`, `339: symlinks_remain_links_with_their_target`, `490: source` |
| `gwz-core/crates/refcopy/src/native.rs` | 4 | `119: ScriptedUnsupported`, `122: ScriptedFailure`, `145: Self`, `149: Self` |
| `gwz-core/src/checked_artifact/bootstrap/managed.rs` | 4 | `211: ManagedParentRequestAuthorityV1`, `247: Unrestricted`, `256: Unrestricted`, `272: ManagedParentRequestAuthorityV1` |
| `gwz-core/src/checked_artifact/bootstrap/runtime/catalog_lease.rs` | 3 | `51: thread_local`, `114: super`, `148: super` |
| `gwz-core/src/checked_artifact/bootstrap/runtime/catalog_lease/target.rs` | 2 | `438: super`, `443: super` |
| `gwz-core/src/checked_artifact/bootstrap/runtime/mod.rs` | 2 | `162: fault`, `167: fault` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/admission_mutation.rs` | 16 | `50: crate`, `78: crate`, `202: crate`, `210: crate`, `223: crate`, `279: crate`, `307: crate`, `318: crate`, `386: crate`, `489: crate`, `504: crate`, `534: crate`, `545: crate`, `643: crate`, `651: crate`, `664: crate` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/aggregate.rs` | 1 | `26: crate` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/authority_record_binding.rs` | 9 | `460: crate`, `506: crate`, `514: crate`, `527: crate`, `555: crate`, `572: crate`, `626: crate`, `639: crate`, `657: crate` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/barrier_mutation.rs` | 12 | `380: crate`, `390: crate`, `394: crate`, `429: crate`, `449: crate`, `547: crate`, `550: crate`, `595: crate`, `603: crate`, `626: crate`, `630: crate`, `666: crate` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/completed.rs` | 1 | `85: super` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/directory_mutation.rs` | 22 | `41: thread_local`, `89: crate`, `102: run_fault`, `160: crate`, `165: crate`, `213: crate`, `217: run_fault`, `242: run_fault`, `259: crate`, `267: crate`, `276: crate`, `326: run_fault`, `337: run_fault`, `351: crate`, `361: crate`, `608: crate`, `613: crate`, `617: run_fault`, `629: crate`, `634: run_fault`, `645: crate`, `650: run_fault` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/filesystem.rs` | 3 | `54: hook`, `86: hook`, `349: hook` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/leaf_observation.rs` | 8 | `171: crate`, `182: crate`, `201: crate`, `217: crate`, `271: crate`, `349: crate`, `359: crate`, `505: crate` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/managed_mutation.rs` | 16 | `444: crate`, `506: crate`, `534: crate`, `631: crate`, `651: crate`, `685: crate`, `734: crate`, `804: crate`, `810: crate`, `860: crate`, `939: crate`, `951: crate`, `957: crate`, `966: crate`, `1168: crate`, `1171: crate` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/mutation.rs` | 12 | `31: thread_local`, `69: crate`, `81: crate`, `127: run_fault`, `142: run_fault`, `157: crate`, `163: crate`, `191: crate`, `237: run_fault`, `248: crate`, `258: crate`, `276: crate` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/namespace_mutation.rs` | 23 | `246: crate`, `262: crate`, `285: crate`, `298: crate`, `310: crate`, `322: crate`, `368: crate`, `413: crate`, `425: crate`, `449: crate`, `464: crate`, `655: crate`, `811: crate`, `821: crate`, `825: crate`, `855: crate`, `858: crate`, `875: crate`, `878: crate`, `898: crate`, `906: crate`, `909: crate`, `950: crate` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/platform.rs` | 2 | `39: thread_local`, `86: thread_local` |
| `gwz-core/src/checked_artifact/capability/pre_catalog/provider/tests_leaf_observation.rs` | 2 | `664: std`, `668: cases` |
| `gwz-core/src/checked_artifact/coordinator/identity.rs` | 1 | `126: (` |
| `gwz-core/src/checked_artifact/fault.rs` | 12 | `31: AfterDestinationPathDerivation`, `40: BeforeAnchorScratchCreate`, `42: AfterAnchorScratchWrite`, `44: AfterAnchorScratchFlush`, `46: AfterAnchorPublication`, `48: BeforeAnchorAliasRetirement`, `50: AfterAnchorAliasRetirement`, `52: BeforeAnchorRoundTrip`, `54: AfterAnchorOutboundRename`, `56: AfterAnchorReturnRename`, `58: AfterAnchorReobservation`, `89: thread_local` |
| `gwz-core/src/checked_artifact/fault_v1.rs` | 1 | `205: thread_local` |
| `gwz-core/src/checked_artifact/platform.rs` | 1 | `60: super` |
| `gwz-core/src/checked_artifact/protocol/authority_record.rs` | 4 | `358: crate`, `364: crate`, `376: crate`, `387: crate` |
| `gwz-core/src/checked_artifact/residue.rs` | 1 | `597: filesystem` |
| `gwz-core/src/checked_artifact/tests/recovery_protocol.rs` | 2 | `50: assert_eq`, `254: assert_eq` |
| `gwz-core/src/filesystem.rs` | 3 | `95: Memory`, `100: Memory`, `108: Memory` |
| `gwz-core/src/filesystem/fake.rs` | 9 | `158: return`, `160: return`, `164: return`, `172: Err`, `255: self`, `276: return`, `278: return`, `280: FsSupportProfile`, `871: return` |
| `gwz-core/src/filesystem/native.rs` | 5 | `30: _`, `47: _`, `489: directory`, `603: unsafe`, `662: unsafe` |
| `gwz-core/src/git/gitbackend.rs` | 14 | `143: delegate`, `145: delegate`, `147: delegate`, `149: delegate`, `151: delegate`, `153: delegate`, `155: delegate`, `157: delegate`, `159: delegate`, `161: delegate`, `163: delegate`, `165: delegate`, `167: delegate`, `169: delegate` |
| `gwz-core/src/git/gitbackend/backend.rs` | 1 | `104: thread_local` |
| `gwz-core/src/git/gitbackend/preservation.rs` | 7 | `616: BeforeUnixParentSync`, `618: AfterUnixParentSync`, `620: BeforeWindowsFirstBarrierRename`, `622: AfterWindowsFirstBarrierRename`, `624: BeforeWindowsSecondBarrierRename`, `626: AfterWindowsSecondBarrierRename`, `633: thread_local` |
| `gwz-core/src/git/gitbackend/preservation_root/parent.rs` | 1 | `102: filesystem` |
| `gwz-core/src/git/tests/g15/root_preservation/faults.rs` | 2 | `135: cases`, `199: cases` |
| `gwz-core/src/git/tests/g15/root_preservation/stash.rs` | 2 | `11: RawNonUtf8`, `391: LateImageChange` |
| `gwz-core/src/operation/commit_log/handler.rs` | 14 | `51: fail_append`, `53: fail_seal`, `58: fail_append`, `58: fail_seal`, `63: fail_append`, `65: fail_seal`, `235: fail_next_spool`, `237: fail_next_append`, `239: fail_next_seal`, `248: fail_next_spool`, `250: fail_next_append`, `252: fail_next_seal`, `301: self`, `303: self` |
| `gwz-core/src/workspace_ops/merge/record_wire/location.rs` | 2 | `192: inject_location_fault`, `396: thread_local` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority.rs` | 1 | `292: reverse_entry_preflight_fixture` |
| `gwz-core/src/workspace_ops/merge/v1_lifecycle/transition/mod.rs` | 1 | `58: super` |
| `gwz-core/src/workspace_ops/pull_head_barrier.rs` | 1 | `12: thread_local` |

### C++ helper: one handwritten file, 83 control-flow bodies

`scripts/fs-bench-helper/gwz-fs-bench.cpp` has 82 unbraced `if` bodies and one
unbraced `else` body in a token scan. Includes single-line destructors and
multiline conditions. This is a lexical inventory, not a completed C++ AST lint.
Brace-only changes must preserve the privileged helper's checks, cleanup and
exit behavior. Validate with the helper's normal Linux build; there is no reason
to run the full GWZ test suite for a braces-only edit.

Exact control-flow keyword lines:

`61`, `73`, `78`, `84`, `85`, `88`, `90`, `95`, `104`, `108`, `113`, `119`, `121`, `128`, `131`, `136`, `137`, `138`, `141`, `142`, `146`, `147`, `149`, `150`, `153`, `154`, `156`, `158`, `166`, `168`, `172`, `179`, `184`, `190`, `196`, `205`, `207`, `213`, `215`, `219`, `225`, `226`, `235`, `238`, `240`, `244`, `247`, `248`, `249`, `257`, `260`, `261`, `262`, `269`, `270`, `274`, `278`, `280`, `290`, `291`, `297`, `299`, `303`, `305`, `320`, `323`, `325`, `330`, `334`, `342`, `345`, `347`, `350`, `352`, `355`, `357`, `359`, `363`, `366`, `368`, `394`, `395`, `396`.

No tracked C/C++/Java/JS/TS/C# files were found in the three package repos.
Workspace `trial/cpp/generated/corpus.hpp` and `trial/cpp/generated/types.hpp`
are generated; review the generator rather than hand-editing their output if
those experiments remain in validation scope. This does not count generator
output as migrated or extend the scan to the separate taut repositories.

### Enforcement task and acceptance

Add a fast syntax-aware rule check and tiny source fixtures, then wire it into
normal validation in core, CLI and Python as applicable. Reuse language lints
where they enforce the rule; do not implement it as a whitespace regex. The
exact implementation/parser is still a decision, not already landed tooling.

Required cases: conditional imports with stacked/multiline attributes;
`cfg_attr`; inactive Windows/test branches; comments and string literals;
bounded accepted alternatives; external-module path preservation; unbraced
C-style bodies; deleting the last declaration in a conditional section without
reattaching its condition to the next unconditional declaration. Keep normal
source-guard tests passing when new `cfg_if!` forms are introduced. Any temporary
migration scope must be explicit; no numeric allowlist/count-pin workaround.

## Test-cost debt: measured release baseline

Evidence: [core v1.0.8 Windows job](https://github.com/owebeeone/gwz-core/actions/runs/34293953652/job/102286363714).
This is one hosted Windows run, not a cross-filesystem benchmark or per-test
profile. No new full suite was run to create this document.

| `gwz-core/scripts/run_tests.py` phase | Result | Cargo/process wall time | Test harness time |
| --- | --- | ---: | ---: |
| Filesystem contracts, fake Git/fake FS | 19 pass | 314.78 s | 0.00 s |
| Migrated contracts/matrices, fake Git/fake FS | 128 pass | 45.43 s | 34.49 s |
| Deliberate native rollback crosscheck | 1 pass | 11.33 s | 0.32 s |
| Remaining native pass, including integration/doc runs | main library 1,828 pass, 1 ignored; integrations 9 + 37 + 9 + 2 pass | 1,464.44 s | main library 1,386.96 s |

The first phase includes compilation/setup; calling its 314.78 seconds a test
execution cost would be wrong. Conversely, the native main library alone took
23m06.96s, so compile caching alone cannot remove that delay. These logs do not
identify the hot functions or prove how many tests need native Git/filesystem.
Compiler-mutation suites remain out of the normal release path.

Next measurement scope: the native main-library phase in
`gwz-core/scripts/run_tests.py`, with the same five exclusions from `MATRICES` and
`MIGRATED_GROUPS`. Capture per-test/group timing and a CPU/I/O profile on Windows,
then select the expensive groups. Do not infer a 1,828-test migration from this
number, and do not promise a speedup from the 146 factory edits alone. A context
migration improves ownership; running a test in native mode can still perform
real I/O after its factories disappear. CLI and Python workflows should be timed
separately before changing their test partition.

## Other open items retained from the debt plan

These are not included in the factory/conditional counts above:

- Local-family merge `--dry-run`: currently explicitly unsupported. The scope is
  a read-only pairing/import/merge preflight, not suppressing writes in the live
  engine. Start from `gwz-core/src/local_clone/family_merge.rs` and the existing
  local-family merge tests; see the main debt plan's deferred preflight contract.
- Encrypted SSH identity/agent selection follow-up: keep the acceptance work in
  `GwzRemoteAuthProposal.md` distinct from the released verbose-output fixes.
- Broader native-I/O/test-fixture migration and deferred file splitting remain
  separate tasks. This audit is not a fresh census of every direct `git2`,
  `std::fs`, native-adapter or subprocess call in the workspace.

Use this snapshot to choose a batch. Update its baseline and affected rows after
that batch; do not leave the old plan's historical 'first slice not started'
text as the current status.
