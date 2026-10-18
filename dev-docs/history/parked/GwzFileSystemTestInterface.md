# FileSystem interface and test implementation

Evidence links below point to private campaign archives and require repository access. Historical commands and recorded paths describe the original runs; see the archive README for replay setup.

Status: first memory-backed root-matrix milestone completed, 2026-09-08. The
filesystem boundary, retained-handle operations, fake Git worktree and both
root-preservation matrices are implemented; whole-core migration remains open.

Companion to [GitRepository test extensions](GwzGitRepositoryTestInterface.md).
The [measured profile](GwzTestPerformance.md#isolated-fake-matrix-profile--2026-09-08)
found about 38% of sampled worker time in durability synchronization and 31% in
YAML processing. Fake Git still accesses real worktree files, and the recovery
matrices still use real journals, retained directories and OS locks.

## Decision and scope

Introduce **one `FileSystem` trait**, **one `NativeFileSystem` implementation**,
and **one `FakeFileSystem` implementation compiled only for tests**. Construct
it through **`make_filesystem()`**, following `make_repository()`.

The same production operations, checked-artifact rules, journal formats and
recovery decisions execute against either implementation. Fake mode stores
filesystem state in memory: no temporary directories, native files, host
canonicalization, directory scans, disk flushes, native locks or subprocesses
for the converted scenario. This includes fixture setup, fake Git's worktree,
execution, assertions and cleanup. A fake journal over real files, or a fake
filesystem that delegates unsupported operations to the host, does not satisfy
this design.

The first complete migration is the existing two root-preservation matrices.
The eventual scope is filesystem access throughout core, including other
workspace operations. Native Git's own object/index I/O stays inside the native
Git implementation; use fake Git with fake filesystems for isolated tests.
CLI/Python process and OS integration tests remain explicitly native initially.

Keep a small physical contract suite and tests whose subject actually is OS
behavior. A memory model tests GWZ's response to filesystem behavior; it does
not establish that a particular OS, filesystem or device provides that behavior.

## Construction, selection and lifetime

```rust
#[cfg(not(test))]
pub(crate) fn make_filesystem() -> impl FileSystem {
    NativeFileSystem::new()
}

#[cfg(test)]
pub(crate) fn make_filesystem() -> TestFileSystem {
    // Forward every operation to the process-selected implementation.
    // Fresh factory calls share the same filesystem namespace.
    test_factory::make_filesystem()
}
```

Production has no fake variant, test controls or environment selector. There
is no Cargo feature that enables the fake in a production library. Keep the
interface crate-private initially; do not expand the public API unnecessarily.

Use the existing process-wide factory pattern. Read `GWZ_TEST_GIT` and the new
`GWZ_TEST_FS` once into one immutable test-backend configuration shared by both
factories. No thread-local mode, per-test mode switch, mutable global selector
or DI container. Invalid combinations fail before fixture setup.

| Git | Filesystem | Use |
| --- | --- | --- |
| fake | fake | Fully migrated unit tests; target routine mode. |
| fake | real | Temporary migration mode for existing fake-Git tests with physical files. |
| real | real | Native contracts, OS integration and explicitly selected comparisons. |
| real | fake | Reject: native Git cannot open an in-memory worktree. |

During migration, an unset `GWZ_TEST_FS` retains real filesystem behavior so
existing tests do not change underneath this work. The runner explicitly sets
both variables for each group. Once routine unit groups are migrated, make the
default pair fake/fake; native groups remain explicitly real/real. The two
variables configure one process, not independently mutable services.

Keep native construction inside the factory and native adapters. Use a small
closed `TestFileSystem` forwarding enum, as with `GitTestRepository`; this enum
is dispatch, not another filesystem implementation.

Every fixture has a unique namespace and an RAII workspace guard. A real guard
owns a temporary directory; a fake guard owns only an in-memory root and its
absolute `PathBuf`. The common fixture helper cleans up both filesystem and
fake-Git state for that namespace. Do not clear a process-wide registry while
other tests run. Namespace generations prevent stale handles or reused paths
from referring to another fixture. Fresh factory calls can reopen the same
fixture, and child threads see the same state. Selection is global; filesystem
contents and fault scripts are isolated per fixture.

Rust integration tests compile core without `cfg(test)`. Keep those native or
move a scenario needing the fake into core's unit-test harness. Do not expose
production fake selection merely to make an integration-test binary convenient.

## One trait, opaque retained handles

A path-only `read/write/exists` wrapper is insufficient. Existing code holds
`cap_std::fs::Dir` and `File` handles to survive renames and verify identities;
locks also have lifetimes. Preserve those semantics.

Use opaque owned `FsDirectory`, `FsFile`, and `FsLockGuard` types with private
native representation and a test-only memory representation. They retain their
provider/namespace identity and rights. They are handles, not additional traits
or additional fakes. Closing/dropping them releases the corresponding resource;
lock-guard drop releases the lock even during unwinding. Explicit synchronization
returns errors; drop must not silently make data durable.

No method returns `std::fs::File`, `cap_std::fs::Dir`, a raw fd/HANDLE, or a
callback receiving one. Native conversion stays private to the native adapter.
Do not require every consumer to become generic over another collection of
handle traits. Clone a handle only through an explicit fallible operation when
a consumer needs it; reopening by path is not equivalent to retaining an object.

Illustrative signatures (the tables define the initial contract):

```rust
pub(crate) trait FileSystem: Send + Sync {
    fn open_directory(&self, path: &Path, options: ResolveOptions)
        -> FsResult<FsDirectory>;
    fn open_directory_at(&self, parent: &FsDirectory, name: &OsStr)
        -> FsResult<FsDirectory>;
    fn open_file_at(&self, parent: &FsDirectory, name: &OsStr,
        options: FileOpenOptions) -> FsResult<FsFile>;
    fn read_at(&self, file: &FsFile, offset: u64, bytes: &mut [u8])
        -> FsResult<usize>;
    fn write_at(&self, file: &FsFile, offset: u64, bytes: &[u8])
        -> FsResult<usize>;
    fn sync_file(&self, file: &FsFile) -> FsResult<()>;
    fn sync_directory(&self, directory: &FsDirectory) -> FsResult<()>;

    #[cfg(test)]
    fn test_workspace(&self, spec: &TestWorkspaceSpec)
        -> FsResult<TestFsWorkspace>;
}
```

`FsResult` preserves underlying `io::Error` information and explicit unsupported
capabilities. Existing layers still map failures to their existing ModelError
or CheckedFsError codes. Do not invent new user-visible error or wire formats.
All required trait operations must be implemented by both backends and forwarded
by the dispatcher; no default success, empty result or native fallback.

## Initial operations and semantics

Add operations with their first consumers, rather than implementing an entire
POSIX/Win32 simulator before migrating one scenario.

| Operations | Contract |
| --- | --- |
| `canonicalize(path)`, `open_directory(path, options)` | Perform backend namespace resolution. Resolution options explicitly distinguish ordinary link-following from the existing checked no-follow policy. Memory mode never calls `Path::canonicalize`. |
| `open_directory_at(parent, name)`, `open_file_at(parent, name, options)` | Resolve a single validated component relative to a retained parent. File options specify read/write rights, existing/create-new/open-or-create, truncation, no-follow and the sharing/durable-open semantics needed by existing callers. Never follow a final symlink implicitly. Preserve native platform limitations explicitly. |
| `clone_directory(handle)`, `clone_file(handle)` | Retain the same object and rights, not whatever currently occupies its old name. Wrong-backend, closed or crash-invalidated handles fail. |
| `metadata_at(parent, name)`, `file_metadata(file)`, `directory_metadata(dir)` | Owned kind, length, relevant permissions/attributes and identity facts. Named metadata does not follow the final link. A missing object is an error distinguishable from permission and other failures. |
| `read_directory(dir)` | Owned names and entry-kind observations, preserving native names. Ordering is unspecified; callers sort when needed. Never an iterator holding an unexposed native resource. |
| `read_at(file, offset, buffer)`, `write_at(file, offset, bytes)`, `set_len(file, length)` | Real byte operations, including short reads/writes and errors. Writes do not silently truncate. Shared `read_all`/`write_all` helpers implement bounds and progress/error handling through these methods; no second native convenience path. |
| `create_directory_at(parent, name)` | Create one directory; existing names are distinguishable. Shared recursive-directory helpers retain the required ancestor/durability ordering. |
| `remove_file_at(parent, name)`, `remove_directory_at(parent, name)` | Unlink a name; directory removal requires emptiness. An open handle still identifies the retained object where the declared platform profile permits that lifetime. Tree cleanup is a shared traversal, not an unchecked host `remove_dir_all`. |
| `rename_at(source_parent, source_name, destination_parent, destination_name, mode)` | Explicit replace/no-replace, correct type/collision and cross-volume errors. Same-volume namespace atomicity and platform publication/durability behavior are preserved. No copy/delete fallback for a cross-volume rename. |
| `rename_opened(source_handle, source_parent, source_name, destination_parent, destination_name, mode)` | Preserve the existing retained-source platform operation. Do not promise a new atomic identity-conditional rename that native POSIX cannot supply; existing checked observation/revalidation remains responsible for its current race protections. |
| `read_link_at(parent, name)`, `create_symlink_at(parent, name, target)` | Preserve target spelling and explicit link behavior. Add hard-link creation with its first actual consumer; shared identity must then be modeled. |
| `set_file_attributes(file, attributes)` | Only the semantic permissions/executable/read-only attributes current consumers use. Keep the platform's actual error/support behavior. |
| `sync_file(file)`, `sync_directory(dir)` | Explicit data/metadata and namespace durability barriers, not unconditional no-ops in memory. Native mode keeps existing flush ordering and platform guarantees; unsupported directory barriers are handled through the existing platform alternative, not falsely reported as successful. |
| `try_lock_exclusive(file) -> FsLockGuard` | Nonblocking lock with explicit contention, correct lifetime and unwind release. Model ownership by open-handle/lock identity, not thread ID. Never a fake always-success lock. |
| `identity(file_or_directory)`, `path_profile(dir)`, `capabilities(dir)` | Durable and invocation identity, rename-domain/mount facts, name-equivalence behavior and supported durability primitives used by the existing checked-artifact adapters. Reuse/relocate existing internal fact types where appropriate. |

Common path-based conveniences (`read`, `write`, `read_to_string`, `create_dir_all`,
`remove_tree`) are shared helpers built from the trait's primitives. Checked code
keeps directory-relative retained handles throughout; convenience wrappers must
not replace its no-follow or revalidation logic with ambient path reopening.

Use `Path`/`PathBuf` and `OsStr`/`OsString`, not UTF-8-only String paths. Validate
relative components before directory-relative operations: absolute components,
separators, `.` and `..` must not escape the retained parent. Symlink resolution
has explicit traversal limits. Case sensitivity, alias comparison, volume
boundaries and platform restrictions come from the declared filesystem profile.
Only implement profiles justified by existing consumers/tests; unsupported
behavior fails explicitly rather than impersonating another platform.

Native file sharing/access rights, write-through publication and retained-source
behavior may need typed options beyond this sketch. Lift the existing operations
without weakening them; do not flatten them into flags that lose their meaning.

## How this fits existing checked-artifact interfaces

`PlatformProviderV1` currently supplies identity/path facts over concrete native
`Dir`/`File` types. `DurableIdentityProvider`, `PathEquivalenceProvider` and the
private namespace interfaces express checked-artifact contracts; they do not
cover all filesystem I/O and do not make the current files virtual.

Replace their native handle dependencies with the opaque filesystem handles and
make their host adapters delegate to `FileSystem`. Keep domain-specific provider,
authority, lease and protocol interfaces where they express existing rules;
do not build a separate filesystem fake for each of them. Factor shared low-level
identity facts into a neutral module if necessary to avoid a dependency cycle.

Checked artifact admission, retained-object verification, publication ordering,
lease rules and recovery decisions remain above the filesystem trait. The fake
must not return a preselected `Allowed`, `RecoveryRequired` or terminal state.
Move raw syscall code from those adapters into `NativeFileSystem`, retaining
platform-specific branches there or in its private modules. Preserve existing
serialized identity representations and protocol bytes: synthetic fixture
identities exercise existing encodings, with no new production “fake” wire tag.

## Memory implementation and test controls

Use one shared implementation with per-workspace state:

- Directories map native names to node identities; files own bytes and attributes;
  symlinks own targets. Replacement allocates a new identity. Renaming a node
  preserves its identity. Retained handles refer to nodes, not pathname lookups.
- Handles retain access rights, workspace generation and platform sharing rules.
  Locks and aliases refer to object identities. Operations on unrelated fixtures
  cannot conflict; do not hold a global registry lock through a whole scenario.
- Keep separate current and durable state for file content/metadata and directory
  entries. A successful file flush establishes the relevant file state; it does
  not make the parent name durable. Namespace changes require their prescribed
  parent barriers. Cross-directory renames retain both parent obligations.
- A completed filesystem operation is visible to subsequent reads and fresh
  factory handles. Reopening a factory is not a crash and does not discard data.
  Existing service panic hooks still simulate interruption at their current
  protocol boundaries; they do not become power-loss simulations automatically.

Add narrowly scoped `#[cfg(test)]` methods on the same trait, with their types
under `cfg(test)`, following the Git fixture extensions:

| Method | Purpose |
| --- | --- |
| `test_workspace(spec) -> TestFsWorkspace` | Isolated root with explicit capability/name profile. Native allocates a real temporary root, fake allocates memory only. A requested unsupported native profile fails explicitly. |
| `test_set_faults(workspace, script)` | Inject failure before an operation, short write with a stated byte count, or error after its effect but before reporting success. Match a semantic operation and fixture-relative object, optionally an occurrence. Keep state already changed by partial/after-effect failures. Implement via the same test-only dispatch hook in both modes where reproducible. |
| `test_operations(workspace) -> Vec<TestFsOperation>` | Opt-in semantic trace for specific ordering and refusal assertions. Bound/reset it per fixture. No routine full trace, exact global operation-count assertions or release recording gate. |
| `test_crash(workspace, outcome)` | Memory-only: discard explicitly unpersisted changes according to the selected crash outcome, invalidate open handles and release runtime locks in that workspace. Return Unsupported in native mode; never crash the machine. |

A fake crash model is a documented deterministic durability model, not a claim
to enumerate every filesystem's possible power-loss outcome. Start with the
persisted-state outcome; add another outcome only for a concrete recovery case.
File creation/removal, parent durability, rename dependencies and identity
retention must be covered before calling those cases supported. Ordinary matrix
interruption tests need valid barriers and error handling, but do not require a
complete simulated operating system.

Use normal filesystem methods to arrange files, replacements, deletions and
symlinks. Test controls do not provide a universal “make this GWZ action succeed”
setter or fabricate a recovery result. Physical power-loss, kernel races,
process-lock and storage guarantees stay in explicit physical tests. Unsupported
simulation must fail or be deliberately classified before execution; never turn
it into a passing or silently skipped scenario.

## Migration order

- [x] **Introduce the boundary and native adapter.** Establish the source/lint
  enforcement described below for the new modules from the start. Add `src/filesystem.rs` and
  private `contract`, `types`, `native`, `factory`, test-only `fake`, and contract
  test modules. Start with open/read/write/metadata/rename/flush operations used
  by `v1_lifecycle/store/rewrite.rs`. Move existing native logic with unchanged
  error mapping and durability semantics; expand with each dependent caller.
- [ ] **Implement memory state and shared contract cases.** Establish contents,
  identity, retained handles, rename/refusal, barriers, fault effects and locks
  with focused tests. Run the same ordinary contract bodies in separate native
  and fake processes; model-only crash assertions are explicitly separate.
- [x] **Unify fixtures and fake Git's worktree.** Replace matrix `TempDir`,
  `fs::write/read/create_dir_all`, `Path::canonicalize`, cleanup and assertions.
  Change `fake_repository.rs`, `fake_repository/fixture.rs` and `root.rs` to use
  `make_filesystem()` for their worktree and exclude-file access. Both factories
  must see identical files. Git history/index/ref data stays in FakeGitRepository;
  file contents belong to FileSystem, not a second shadow worktree.
- [x] **Move journal I/O and locking.** Convert `v1_lifecycle/store/{rewrite,archive}.rs`,
  `checked.rs`, `durable_fs.rs`, `WorkspaceMutatorLock`, and the runtime lease's
  retained paths/advisory lock. Convert all reachable canonicalization and
  validation reads, including creation and drop paths. Keep CheckedV1Store and
  its serialization/recovery logic shared.
- [x] **Move checked-artifact and root-preservation I/O.** Convert retained
  directories/files, platform identity observations, residue publication,
  namespace mutations, parent barriers and `gitbackend/preservation_root/parent.rs`.
  Include common artifact/stash read/write helpers reached by the matrices.
  Other artifact/stash callers remain in the final core-caller migration below.
  Preserve existing fault boundaries and seals while replacing the native
  leaf operations. This is required for an entirely memory-backed matrix;
  converting the journal alone is an intermediate step.
- [x] **Run one complete root recovery scenario in both modes.** Prove that fresh
  factories reopen the same state, dirty user work survives, identities detect
  foreign replacement, and interrupted execution resumes correctly. Verify the
  fake scenario performs no host workspace I/O, including fixture teardown.
- [x] **Convert and measure both complete matrices.** Use fake/fake routinely;
  retain a small real/real cross-check and make full native comparison explicit.
  Keep the current scenario assertions and loops for this substitution so its
  performance/correctness effect can be measured separately from test reduction.
- [x] **Remove duplicate decoding/validation as a separate change.** Preserve
  exact-byte drift checks and unknown-field behavior while avoiding revalidation
  of an already validated identical record. Run focused drift/round-trip tests
  and remeasure. The filesystem fake alone does not remove the profiled YAML CPU
  cost. Any later reduction of repeated matrix traversal is a separate change.
- [ ] **Migrate remaining core callers by operation/helper.** Route direct host
  filesystem access through this same boundary. Keep explicit native Git,
  transport, process and OS-specific tests in their appropriate native groups;
  document remaining bypasses until converted. Extend this trait only when an
  actual consumer needs an operation.

Use the corresponding groups in `scripts/run_tests.py`; retain one scenario body
per behavior. Example commands once the new selector is implemented:

```sh
GWZ_TEST_GIT=fake GWZ_TEST_FS=fake cargo test --locked --lib filesystem_contract_tests
GWZ_TEST_GIT=real GWZ_TEST_FS=real cargo test --locked --lib filesystem_contract_tests
GWZ_TEST_GIT=fake GWZ_TEST_FS=fake cargo test --locked --lib -- root_fault_matrix:: root_ambiguity_matrix::
```

The filesystem contract commands run the implemented operation set. Both root
matrices now run with `GWZ_TEST_FS=fake`, including catalog activation through
the shared implementation. Remaining core callers still need migration.

## CLI and Python compatibility is part of completion

The migration must also validate **gwz-cli and gwz-py against the changed local
core**, not just pass core's own unit tests. Both currently depend on
`../gwz-core` by path. CLI tests include real-binary workflows, while Python
builds the `_gwz_core` extension and has native-bridge and cross-driver tests.
Filesystem fixtures in those suites are real and must stay compatible with the
native adapter.

When core is compiled as their dependency, its `cfg(test)` is **off**, even if
the consuming crate is being tested. Consequently their core factory continues
to select NativeFileSystem and native Git. Do not propagate a fake-build feature
into those products, expose fixture types to bindings, or rewrite Python/CLI
integration fixtures to an in-memory namespace their native processes cannot
access. Core's internal opaque handles must not leak through public functions,
protocol results or the Python extension API.

Add these checks to the migration batches:

- [x] **Compile both consumers early.** After introducing the boundary and after
  changes to public signatures, trait bounds or handle ownership, check CLI
  including its tests and the Python Rust extension against the local core.
  Catch visibility, Send/Sync and lifetime regressions before migrating more
  consumers. Preserve the existing public API unless a necessary coordinated
  change is explicitly included in the batch.
- [x] **Run focused native workflows after each relevant batch.** Select actual
  existing CLI and Python tests for the paths changed: workspace initialization,
  artifact reads/writes, merge/recovery and preservation, local clone/disposal,
  and error reporting. Check persisted files, refusal/error codes and preserved
  user work as well as returned values. Keep driver semantics in core.
- [x] **Rebuild the Python extension before testing it.** An already installed
  editable extension must not satisfy the check. Use the configured Python venv
  and `maturin develop`; verify import succeeds before pytest so an
  `importorskip` cannot disguise a missing native module as success.
- [x] **Use the newly built CLI for Python parity tests.** Build the local CLI
  and pass its actual artifact path through `GWZ_RUST_BIN`. Resolve the target
  directory from Cargo metadata/build output rather than assuming
  `gwz-cli/target`: this checkout's CLI belongs to the outer workspace, while
  Python and core have separate Cargo workspaces. Do not accidentally compare
  against an older installed executable or an extension built from released
  core instead of the changed checkout.
- [x] **Check selector isolation.** A small consumer smoke test run with the
  test-selector environment variables set to fake must still use real files:
  the compiled production dependency contains no test selector. Ordinary
  consumer runs can clear these variables for clarity; environment cleanup is
  not the mechanism that makes production safe.
- [x] **Run both ordinary consumer suites at the first complete matrix milestone**
  and again at final core migration completion. Use the existing runners; run
  relevant macOS/Linux/Windows jobs for native behavior changed by the adapter.
  Keep compiler mutation probes out of routine validation and do not run release
  scripts merely to test compatibility. Run the existing Python package smoke
  if packaging/import behavior changes.

Example local checks, from the outer `gwz-dev` directory:

```sh
cargo check --locked -p gwz --tests
cargo check --locked --manifest-path gwz-py/Cargo.toml
cargo test --locked -p gwz --test local_family_workflows
```

Use the appropriate existing CLI test target for other affected paths. For a
focused Python run, first build the local CLI, then from `gwz-py`, in its venv:

```sh
python -m maturin develop
python -c 'import gwz._gwz_core'
python -m pytest src/tests/test_native_bridge.py src/tests/test_native_merge_operations.py -q
```

Supply `GWZ_RUST_BIN` with the verified local CLI path when a selected test uses
it. At a coherent milestone, `cargo test --locked -p gwz` runs the CLI suite;
`python run_tests.py` from `gwz-py` rebuilds the extension, checks the protocol
and runs its tests, with the same verified `GWZ_RUST_BIN`. These are validation
steps for implementation, not claims that these suites have already run.

A migration batch is not complete if its relevant downstream checks fail.
Report core, CLI and Python results separately, including any platform not yet
checked. Full migration acceptance includes all three products working with
native production filesystem behavior.

## Enforce the filesystem boundary automatically

**Core application logic must not access the host filesystem directly.** All
such access belongs behind `FileSystem`; production still selects its native
implementation. The rule includes test fixtures and assertions. `#[cfg(test)]`
is not an exemption from filesystem isolation.

Add a fast, mandatory architecture check alongside the existing boundary checks.
Use Rust's existing Clippy checks for resolved types/methods, supplemented by a
small source-policy check. A search for `use std::fs` alone is insufficient:
`std::fs::read(path)`, renamed imports, `Path::exists()` and temporary-directory
helpers can all touch disk without that import spelling.

### Compiler-resolved restrictions

Configure `clippy::disallowed_methods` and `clippy::disallowed_types` as errors
for the application modules. Maintain a reviewed list of host I/O entry points
in the Clippy configuration, not an inventory of their current call sites:

- `std::fs` entry points and native file/directory/metadata types, including
  reads, writes, enumeration, creation, deletion, links, rename and permissions.
- Filesystem-observing `Path` methods: `exists`, `try_exists`, `is_file`, `is_dir`,
  `is_symlink`, `metadata`, `symlink_metadata`, `canonicalize`, `read_dir` and
  `read_link`. Pure path operations such as `join`, `parent`, `components` and
  `file_name` remain allowed.
- `cap_std` file/directory types and native-opening entry points; native file
  handle access/conversion APIs used to bypass these restrictions.
- `tempfile`, native fixture helpers, filesystem-walking libraries, and direct
  libc/Windows filesystem entry points used by core.

Resolved method/type checks catch aliases and fully qualified calls; the
implementation must verify that with small checker fixtures. Native handles
are private to the adapter, so ordinary callers cannot acquire a file and then
hide I/O behind `Read`/`Write` methods. `std::io::Read` and `Write` themselves
remain allowed for in-memory readers/writers.

Reuse the normal Clippy invocation; do not create a compiler-mutating test or
recompile the entire workspace once for each forbidden spelling. Add tiny
compile-fail/pass fixtures for the rule in one checker invocation, run when the
rule changes. Routine checking scans/lints the real source once. Existing OS
jobs check their platform-specific code; a Mac-only check must not be claimed
to cover code excluded by Windows/Linux cfg attributes.

### Source policy and exceptions

The source-policy check also rejects direct imports/re-exports of host filesystem
modules outside the approved adapter locations, and rejects new local
`allow`/`expect` attributes suppressing the filesystem restriction. Handle grouped
and renamed imports, rather than matching only a literal line. Comments and
string literals must not trigger diagnostics. Report file, line, forbidden API
or import, and the replacement (`make_filesystem()` / `FileSystem`).

Keep exemptions narrow and explicit:

- The private `filesystem/native` implementation and native handle internals.
- Native Git's actual storage adapter where libgit2 requires host files/handles.
- Explicit physical adapter/OS contract tests whose subject requires native I/O.

Do not exempt all of `git`, `checked_artifact`, `tests`, an entire platform cfg,
or any module merely because it currently contains a direct filesystem call.
FakeFileSystem and FakeGitRepository never receive a native-I/O exemption.
A new host-I/O dependency, native FFI call or subprocess escape must be reviewed
at the boundary; prohibit ad-hoc file commands in ordinary application modules
rather than letting a shell command bypass the trait.

Clippy plus a source guard enforces the known boundary; it is not a sandbox or a
proof about arbitrary third-party code, macro expansion or dynamically invoked
FFI. Keep the focused runtime no-host-I/O verification for converted scenarios
as complementary evidence.

### Rollout and acceptance

- [x] Add the rule and its focused examples: direct import, grouped/renamed
  import, fully qualified call, aliased function call, `Path` observation,
  temporary-directory helper, forbidden exemption, and an allowed pure path or
  in-memory I/O operation. Failures must identify the offending source location.
- [x] Enforce it on each migrated module immediately. List remaining migration
  scope explicitly; do not pretend current native callers are already clean.
  Temporary exclusions identify legacy modules with a removal step, not accepted
  violation counts or hashes. New modules are subject to the rule by default.
- [x] Invoke the fast check from the normal test runner and existing structural
  CI checks, so a developer does not need to remember a separate command.
- [ ] At whole-core migration completion, remove temporary exclusions. Only the
  reviewed native boundaries and explicit physical tests may touch host files.

A newly introduced direct filesystem dependency in protected application code
must fail ordinary validation, even if its behavioral tests happen to pass.

## Verification and completion

Use focused failing tests before each behavior, then run the same contract in
both modes where the contract is physically observable. Cover create-new
collision, short writes, replace/no-replace, symlink refusal, retained identity
across rename/replacement, lock contention/release, durability ordering and
state retained after failed operations. Cross-check OS-specific contracts on
their supported platform; do not demand identical unsupported capabilities.

For migrated fake scenarios, verify the whole reachable path uses the interface:
include aliases of `std::fs`, path metadata/exists/canonicalize methods,
`cap_std` operations, libc/Windows calls, temporary-directory helpers and external
process calls. Use a focused no-host-I/O guard/trace for fixture roots to catch
accidental fallbacks; exclude executable/runtime loading and test-harness output.
A passing test whose files happened to land in the OS temporary directory is
not completion. Keep any source boundary check narrow and architectural, not a
pinned inventory of every call or a compiler-mutation probe.

Production checks must confirm fake types, fixture controls and selectors are
absent. Update existing boundary rules to describe the intentional adapter move,
without weakening checked-artifact authority rules. Preserve current disk
formats, native error behavior and crash-recovery guarantees. Run relevant
checks once per coherent batch, not the full suite after every helper edit.

The first milestone is complete when both root matrices run their unchanged
recovery algorithms and assertions entirely against memory state, the native
contract cross-checks and CLI/Python compatibility suites pass, and isolated
before/after timings are recorded.
Do not promise a seconds-long runtime from removing disk waits alone: the
profile also found substantial repeated parsing and validation. Subsequent
whole-core migration remains explicitly unfinished until its callers move.

## Implementation progress — first slice, 2026-09-08

Implemented in core:

- `filesystem.rs` defines one FileSystem trait and opaque directory/file handles;
  `filesystem/native.rs`, `fake.rs` and `factory.rs` provide native, memory and
  test dispatch respectively. Production construction is unconditionally native.
- The first operations cover retained directory/file opens, create-new files,
  byte reads/writes, file flush, rename with replacement, directories, unlink,
  canonical paths, file kind and executable-bit observations. Memory nodes retain
  contents across factory reopening and preserve open objects across rename and
  replacement. Each fixture owns an isolated virtual root; no native fallback.
- `test_backend.rs` holds the one immutable Git/filesystem mode selection. The
  existing Git factory uses it too. Filesystem default remains real during
  migration, and native Git plus memory filesystem is rejected.
- Journal `store/rewrite.rs` now uses FileSystem for reading, canonicalization,
  temporary-file creation, writing/flushing, unlink and parent creation. Its
  existing atomic publication and parent barrier still use `durable_fs`.
  The journal reader has a shared native/memory test.
- Preservation-image reads and selected finalization/entry observations now use
  owned filesystem metadata and file reads through the same interface. Their
  existing error-code and executable/symlink decisions remain in their owners.
- `check_filesystem_boundary.py` protects the migrated source scope. The test
  runner and structural CI invoke it. Its focused tests exercise grouped/aliased
  imports, qualified calls, path observations, subprocess bypass, forbidden lint
  suppression and ignoring comments/literals. The runner explicitly sets both
  backend modes so inherited environment cannot switch legacy groups.
- The broader compiler-resolved policy is in
  `scripts/checks/filesystem_lints/clippy.toml`. Structural CI uses this configuration
  for core with `--no-deps`, replacing its existing core Clippy invocation. The
  original configuration remains for the separately validated, unmigrated native
  storage libraries. Migrated modules deny the relevant lints; native adapter
  and physical source/protocol checks have explicit scoped exemptions. This does
  not claim that all core modules are already protected or migrated. FileSystem
  mutators are also on the checked-reader forbidden-method list, so adding the
  abstraction does not create an escape around existing mutation authority.

Current focused verification on macOS:

| Check | Result |
| --- | --- |
| Native filesystem, journal/store and Git contracts | 30 passed, 1.60s test execution |
| Native shared root-preservation/stash checks | 2 passed, 1.54s |
| Memory filesystem and journal reader | 5 passed, under 0.01s reported test execution |
| CLI init/status/snapshot/materialize and merge-start workflows | 2 passed, 2.27s |
| Rebuilt Python extension: native bridge and merge operations | 37 passed, 2.95s |
| Source guard, runner-mode and existing release-boundary unit checks | 12 passed |
| Existing workflow integration checks | 9 passed |
| Expanded core Clippy, formatting and checked-artifact boundary | Passed |

The Python checks ran with both selector variables set to fake, yet exercised
real temporary workspaces through the production dependency. The current CLI
also successfully inspected the actual workspace under those variables. This
checks that production consumers do not acquire core's test-only selection.
The Python extension was rebuilt with maturin; its import was checked explicitly.
The release-boundary unit checks use mocks; no release/tag/push was performed.

Build caches exhausted the local disk during verification. Only the three
rebuildable `target/debug/incremental` directories were removed. Subsequent
checks used `CARGO_INCREMENTAL=0`; source and workspace data were preserved.

Still required before the first full memory-matrix milestone:

- Directory durability, identity facts, locks, no-replace/publication semantics,
  symlinks/attributes, explicit fault scripts and the documented crash model.
  The initial fake records file-flushed bytes but is not yet a crash simulator.
- Conversion of checked-artifact retained handles and platform operations,
  runtime leases, root parent operations, common fixtures and fake Git's physical
  worktree access. Both matrices still depend on real files today.
- Removal of duplicate decoding/validation, separately measured after the
  filesystem migration; no performance claim for the complete matrices yet.
- Remaining core callers, full consumer suites at the milestone, and native
  Linux/Windows validation. The checks above are focused macOS checks only.

Manual expanded lint command, from core:

```sh
CLIPPY_CONF_DIR="$PWD/scripts/checks/filesystem_lints" \
  cargo clippy --locked --no-deps --lib --tests -- -D warnings
```

Focused logs: [filesystem-interface-20260908](https://github.com/owebeeone/gwz-core-evidence/tree/main/campaigns/filesystem-interface/runs/filesystem-interface-20260908).

## Implementation progress — publication and fake-Git slice, 2026-09-08

The publication seam and fake Git worktree are now migrated:

- `FileSystem` now owns atomic replace/no-replace rename, directory barriers,
  directory enumeration and empty-directory removal. Seven shared contract
  tests pass against both native and memory backends. The no-replace contract
  proves that a collision preserves both source and destination.
- The native adapter now contains the Unix/macOS/Linux/Windows publication
  implementation. `durable_fs` is a compatibility facade for unmigrated
  callers and no longer performs host I/O itself.
- Merge journal rewrite and terminal archive use the interface for their full
  file lifecycle, including publication and parent barriers. The existing 18
  store tests pass on the native backend.
- The neutral fallback record writer and the ordinary artifact atomic writer
  now stage, sync, publish, barrier and reread through `FileSystem`.
- Fake Git repository creation, worktree enumeration, stage/read, reset,
  stash apply, exclude parsing and checkout writes now share the memory
  filesystem. Its seven repository contract tests pass fake/fake in 0.02s and
  create no physical repository.
- The common core `TempDir` fixture now acquires its workspace from the selected
  filesystem. Production and real-mode tests continue to receive a native
  temporary directory.
- The checked-artifact source checker no longer pins private item/call counts
  or raw primitive spellings for migrated paths. It continues to pin the
  visible checked-entry API and the single neutral fallback writer caller.

At the end of that slice, the reverse-preservation matrix reached the retained
marker-parent implementation before touching native storage. Its
factory-reopening case passed real/real, while fake/fake stopped because
`preservation_root/parent.rs` still owned `cap_std` handles. The following
section records the completed retained-handle migration.

## Implementation progress — memory root matrices, 2026-09-08

The retained-handle slice and the first complete matrix milestone are now
implemented:

- `FileSystem` now provides retained directory enumeration and synchronization,
  replace/no-replace relative rename, directory-entry identity checks and owned
  file/directory identities. The native implementation keeps platform-specific
  publication inside `filesystem/native.rs`; the memory implementation compares
  stable node identities rather than paths.
- `preservation_root/parent.rs`, preservation index reads and candidate-file
  observations use `FileSystem`. Fake Git calls the shared production root
  preservation algorithm rather than maintaining a separate test algorithm.
- The shared Git test fixture is selected by `make_repository()`. Its test-only
  `test_force_checkout` operation supplies the one setup capability needed by
  these cases without exposing a fake selector in production.
- Merge-root artifact observation/replacement, canonical record locations and
  selected-root rollback observation use the same retained-handle algorithm in
  both filesystem modes. A fake-mode mutation lease currently bypasses catalog
  activation because those catalog internals have not moved behind the trait
  yet. This is a narrow intermediate test seam, not production behavior.
- The correct test selector is `GWZ_TEST_FS`. Both matrices use paths under
  `/__gwz_memory__`; accessing those paths through host filesystem APIs would
  fail, providing practical isolation evidence for the reachable matrix paths.

After the matrix milestone, canonical open/archive record observation and
selected-root baseline observation were also moved completely onto `FileSystem`;
their separate native branches and platform identity helpers were removed. The
new `file_entry_matches` operation binds an opened file handle back to its named
parent slot before and after reading, preserving same-name replacement detection
for both native and memory backends. These two modules are now covered by the
filesystem source guard.

`FileSystem` now also owns nonblocking advisory file locks through an opaque
`FsLockGuard`. The native implementation holds the OS lock for the guard's
lifetime; the memory implementation records contention by file identity and
releases it on drop. Fake v1 mutation leases now take this real memory lock at
the ordinary workspace lock path instead of using an empty guard. A shared
contention/drop contract runs against both implementations.

Current focused verification on macOS:

| Check | Result |
| --- | --- |
| Filesystem contract, fake/fake | 14 passed, 0.01s |
| Filesystem contract, real/real | 14 passed, 0.04s |
| Root-preservation factory reopen, real/real | 1 passed, 1.43s |
| Root-preservation entry, recovery and phase groups, fake/fake | 12 passed, 0.14s combined |
| Root successor matrix, fake/fake | 4 passed, 29.13s after the retained record-location migration |
| Root fault and ambiguity matrices after splitting serial loops, fake/fake | 22 passed, 79.82s combined |
| Reverse-rollback tests after splitting the serial fault loop, fake/fake | 36 passed, 3.79s |
| Reverse-rollback tests, real/real | 28 passed in 64.14s; the sole failure was a temporary diagnostic assertion, and its corrected focused native case passed in 0.17s |
| CLI full suite after rollback migration | 253 passed |
| Rebuilt Python extension, protocol check and full suite | 798 passed in 71.20s |

The remaining fake-matrix cost is primarily repeated lifecycle traversal and
YAML serialization/validation. Splitting the old serial test functions reduced
the root pair from 240.34s to 79.82s and reverse rollback from 13.53s to 3.42s
in focused warm runs without deleting scenario rows. The root tests now contend
on shared fake state when all migrated groups run together, so adding more test
functions alone is unlikely to reduce the remaining wall time.

Linux and Windows Rust targets are installed locally, but cross-checking from
this Mac stops in third-party native dependencies before compiling gwz: Linux
lacks an `x86_64-linux-gnu` compiler/sysroot and target OpenSSL/zlib, while the
Windows target lacks the MSVC SDK/vcpkg headers. Native validation on those
platforms remains required.

The next filesystem work is to move checked-artifact catalog activation and its
retained paths behind `FileSystem`, remove the temporary fake lease seam,
then protect the catalog modules completely. Record-location and the complete
reverse-rollback test directory are already guarded. After that, migrate
remaining direct core callers by helper and finally remove the source guard's
legacy exclusions.

That catalog slice must preserve one algorithm across both backends. Its order
is now fixed:

1. Add filesystem-owned facts for support profile, durable object identity,
   invocation identity, component lookup mode, rename domain and volume
   description. These values must contain no checked-artifact types.
2. Change runtime bootstrap retention and advisory locking from `cap_std`
   handles to `FsDirectory`, `FsFile` and `FsLockGuard`.
3. Make the checked-artifact platform provider translate the filesystem facts
   into catalog facts. Platform-specific probing moves into
   `filesystem/native`; the memory backend supplies stable facts for its node
   identities.
4. Convert pre-catalog retained roots, index retention, alias enumeration and
   publication to the same retained filesystem handles.
5. Delete the `V1MutationLease::acquire_activated` memory return and every
   `entry.rs` fake checked-artifact branch, then rerun catalog fault tests and
   lifecycle matrices in both modes.

The order prevents a second fake catalog algorithm from becoming permanent and
keeps production builds unable to select the memory backend.

Step 2 is now complete. Runtime path resolution, ambient and child retention,
file creation/revalidation, alias enumeration, catalog-target association and
advisory locking all use `FileSystem`. Checked-artifact runtime code no longer
contains its own `flock`/`LockFileEx` implementation. Catalog association also
uses `make_repository()` for worktree, Git-directory and common-directory
membership. The production runtime files are registered in the filesystem
source guard. The native runtime suite passes all 31 focused tests.

`V1MutationLease::acquire` now uses the same `WorkspaceMutatorLock` in both test
and production builds; the separate memory lock construction is gone. The two
catalog-specific fake returns remain in `acquire_activated` and
`acquire_for_merge_start`. A focused attempt to remove the activation return
reached `recover_or_create` and stopped at the next unmigrated boundary:
pre-catalog retained-directory observation still opens a host directory. This
confirms step 4, beginning with `provider/retained.rs`, is the prerequisite for
removing those final returns. The high-level fake artifact branches and their
duplicate helpers in `entry.rs` have been removed; all legacy artifact entry
doors now execute the shared retained-handle implementation.

The ordinary runner now selects fake Git and filesystem for the root fault,
ambiguity, successor and durability matrices and the complete reverse-rollback
suite. It retains the native rollback regression as a separate cross-check and
skips the migrated groups in the native remainder. A warm full run passed in
245.52s, compared with 337.84s immediately before the runner change (92.32s,
27.3% less wall time). The migrated phase took 89.47s and the native remainder,
including integration binaries, took 154.74s.

A subsequent function-level profile separated fake CPU cost from native I/O.
The large fake matrices spend most of their time reparsing and validating the
durable YAML record on every journal commit. The independent root-successor
owner/phase and owner/handoff cases are now separate Rust tests, reducing that
module to 6.50s under the 12-thread harness. `Cargo.toml` now uses test-profile
optimization level 1; the complete migrated fake phase fell from 85.72s to
16.61–18.01s. A native managed-intent group changed only from 13.30s to 12.29s,
confirming that catalog retention, repository relationship discovery and
durability barriers remain the next filesystem migration target. The first full
run after the profile change passed and included a 76.69s one-time rebuild. A
direct warm full run then passed in 176.00s, 69.52s (28.3%) below the preceding
245.52s warm run.

The decoder now carries its `ValidatedV1Record` into the checked store rather
than validating the same decoded model a second time. This keeps all strict YAML,
typed validation and exact-byte checks while reducing the optimized fake phase
again, from 16.61–18.01s to 14.02s. The corresponding one-request ambiguity
case fell from 2.27s to 1.59s.

The first three read-only entry doors have now crossed that boundary:
`observe_merge_root_artifact`, `observe_merge_preservation_workspace` and
`observe_merge_preservation_git_directory` use one retained-handle traversal
for native and memory filesystems. The traversal checks every parent entry,
the opened leaf before and after reading, the retained parent after reading and
the ambient root after reading. `FileSystem` gained no-follow retained metadata
and retained-directory cloning to support that algorithm, including root-level
artifacts whose parent path is empty. Contract tests run the new operations on
both backends. Mutation, classification and durable bundle observation still
use the legacy checked-artifact implementation and remain the next conversion.

The same 29-test fake/fake reverse-rollback slice took 13.80s after this change,
compared with the preceding 14.25s sample. The roughly 3% difference is too
small to claim as a stable speed improvement; it confirms that removing host
I/O alone does not address the repeated lifecycle/fault traversal cost.

The reverse-rollback fixtures now use the selected repository and filesystem
factories throughout. The only raw Git/filesystem fixture in that directory is
the deliberately native conflict test. Test-only Git operations now cover mixed
reset, repository-state seeding, exact unresolved-merge seeding and parent-tree
commit edits. Their native and memory implementations sit behind the same
`GitRepository` contract. The fake status implementation also models
assume-valid and skip-worktree suppression while exact checkout verification
still rejects those semantic index flags; this matches the two distinct native
observations exercised by rollback entry.

## Implementation progress — catalog and durable publication, 2026-09-08

The checked-artifact production tree now uses `FileSystem` throughout. Platform
probes live in `filesystem/native/facts` and `filesystem/native/legacy_identity`;
the catalog provider translates neutral filesystem facts into validated catalog
identities. Legacy and catalog identity encodings are preserved separately so
this refactor does not change existing persisted records.

Retained catalog roots, files, index reads, bounded directory-name enumeration,
mutation, publication and the Windows durability anchor use `FsDirectory` and
`FsFile`. Stream operations and directory conveniences delegate to the same
filesystem interface. Native handle extraction is private to the native adapter.
The Windows implementation retains source-handle rename and write-through file
opens, including reopening an existing scratch file.

`V1MutationLease::acquire_activated` and `acquire_for_merge_start` no longer have
fake early returns. A shared factory test verifies initial catalog creation,
identity stability after reopening, and merge-start directory preparation. A
shared filesystem contract verifies persistent facts across name replacement;
Windows additionally checks publication of a retained source after replacement
of its original name. Existing catalog and durability fault scenarios remain.

The source guard covers every production module under `checked_artifact`, in
addition to the existing migrated scopes. Test-only items are excluded without
hiding production platform branches such as `cfg(any(windows, test))`. Native
syscalls remain permitted only inside the filesystem adapter in this scope.

This closes the catalog slice described above, not the whole-core migration.
Direct I/O remains in workspace discovery/bootstrap, ordinary artifact/stash
helpers, some merge consumers, local-clone adapters and Git transport helpers.
Convert those by operation, preserving follow/no-follow, error and durability
semantics; extend the guard as each scope is converted. Native Git and process
integration tests remain intentional physical consumers.

The next design step is [operation context ownership](GwzOperationContextPlan.md)
before the next large Git fixture batch. The context plan is not implemented by
this filesystem change.

Validation for this batch:

- The ordinary macOS core runner passed, with compiler mutation tests excluded.
  Its phases totalled 285.64s including rebuilds while cross-platform fixes were
  being made. The fake scenario phase itself took 14.55s, close to the preceding
  14.02s; this is not evidence of a new speed improvement.
- The complete CLI suite passed (253 tests). Python's full suite passed 798
  tests and initially failed its exact CLI/extension provenance comparison:
  concurrent builds had captured different source snapshots. Both artifacts
  were rebuilt together; all 37 focused bridge/merge checks then passed,
  including provenance. Running that recheck with both test selectors set to
  fake still exercised real files through the production dependency.
- The Raspberry Pi passed 479 native checked-artifact/filesystem/factory cases
  on ext4 in 87.75s and the shared memory contracts in 0.20s. Its default `/tmp`
  is volatile, so native disk-durability fixtures used an ext4 `TMPDIR`.
- Windows passed 454 native checked-artifact/filesystem/factory cases in 191.12s
  and all 20 shared memory contracts in 0.22s, including retained-source
  replacement and catalog activation. Native checks caught and corrected path-only
  metadata being used as though it contained handle identity. Windows metadata
  now comes from a no-follow handle. The existing shared directory adapter
  permits rename, so its stale name-pinning test now checks identity refusal
  and preservation of both the original and substituted contents.
  Retained no-replace rename now shares the working source-handle publication
  primitive instead of maintaining a second Windows implementation.
- macOS Clippy with warnings denied and the filesystem/checked-artifact source
  guards passed. Raw logs and source archives stay outside the repositories.
