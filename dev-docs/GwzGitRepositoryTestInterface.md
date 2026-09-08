# GitRepository test-only extensions

Status: proposed design, 2026-09-08. This document does not implement or enable
new methods. `GitRepository` is the existing interface referred to as
“GitInterface” in the discussion; do not introduce a second trait.

## Filesystem follow-up

The statements below retaining ordinary worktree files and real journals describe
the first Git migration batch. The proposed
[FileSystem interface plan](GwzFileSystemTestInterface.md) supersedes that choice
for subsequent migration: fully isolated tests will select fake Git and an
in-memory filesystem together. The filesystem foundation is now implemented, but complete memory-backed
matrices remain pending; current Git first-batch validation remains historical
evidence. Both factories share the immutable configuration in test_backend.rs.

## Construction and visibility

Keep `make_repository()` and its current compile-time split. Production always
constructs Git2Repository. Unit tests select real or fake once per process with
GWZ_TEST_GIT. No per-test switches, thread-local selection, DI container, or
mutable global mode.

Add fixture methods directly to GitRepository, each under `#[cfg(test)]`.
Their argument/result types live in a `#[cfg(test)]` module alongside the
contract. The real implementation, fake implementation, and factory dispatcher
implement these methods only in test builds. Production cannot name the types,
call the methods, or select the fake. Do not add a Cargo feature that enables
fixture mutation in a production library.

```rust
pub trait GitRepository {
    // Existing production operations remain here.

    #[cfg(test)]
    fn test_create_commit(
        &self,
        repo: &Path,
        spec: &TestCommitSpec,
    ) -> ModelResult<String>;

    #[cfg(test)]
    fn test_set_ref(
        &self,
        repo: &Path,
        name: &str,
        target: Option<&TestRefTarget>,
    ) -> ModelResult<()>;

    // Remaining test-only methods below follow the same pattern.
}
```

No method returns a git2 Repository, Index, Commit, Config, or callback receiving
one. Results are owned values. Ordinary tests use the interface for setup,
execution and observation; they do not acquire a native handle indirectly.

Rust integration-test executables compile the library without cfg(test). These
extensions are therefore initially for the core unit-test harness. Keep external
integration tests real, or move a scenario requiring these fixture methods into
the core unit-test harness. The local-testrepo subcrate cannot depend back on
core; do not introduce a dependency cycle to migrate its own tests. Its native
implementation may supply reusable fixture techniques, not a second fake.

## Initial extension methods

These are fixture operations with explicit semantics, not replacements for the
production operations whose behavior tests are meant to exercise. New fixture
methods have no default successful implementation; the compiler must require
both implementations and the factory dispatcher to handle them.

| Method | Contract |
| --- | --- |
| `test_init_repo(repo, spec)` | Create an empty, isolated repository with explicit initial branch, bare/worktree form and object format. Fail if already initialized. Install deterministic local fixture identity and disable inherited templates, hooks and signing. Use ordinary `create_repo` when testing production repository creation. |
| `test_create_commit(repo, spec) -> String` | Create an object from an explicit tree, ordered parents, message and signatures. Return its object ID. Do not move HEAD or a ref, or change the live index/worktree. |
| `test_read_commit(repo, oid) -> TestCommit` | Return ordered parents, tree ID, message, author and committer. Missing or non-commit objects are errors. |
| `test_set_ref(repo, name, target)` | Set or replace a direct or symbolic ref without production drift checks. None removes it; deleting an absent ref succeeds. Leave HEAD attachment, index and worktree unchanged. A symbolic HEAD naturally resolves through any changed branch ref. |
| `test_set_head(repo, state)` | Set HEAD to an attached branch, an unborn branch, or a detached commit. Does not check out files or change index entries. |
| `test_replace_index(repo, entries)` | Replace the complete logical index, including conflict stages and semantic flags. Entries refer to objects already in the repository. Do not modify the worktree, refs or commits. |
| `test_read_index(repo) -> Vec<TestIndexEntry>` | Read the complete logical index, sorted by raw path then stage. Does not refresh, normalize or rewrite it. |
| `test_set_config(repo, key, values)` | Replace the repository-local values for a key; an empty list removes it. Supports multi-valued keys. Does not modify user/system configuration or process environment. |
| `test_read_config(repo, key) -> Vec<String>` | Read only the repository-local values, in order. Missing key returns an empty list. |

Return types in this table use the existing ModelResult convention throughout.
Existing production methods remain the way to read HEAD/refs/status, read a file
at a commit, stage paths, create a normal commit, reset, stash, or query ancestry.
Do not add duplicate `test_head`, `test_status`, or `test_stage` methods.

In particular, use `test_set_ref` to arrange deliberate foreign state; then use
`create_backup_ref_checked` or another production operation to test rejection.
Do not use a fixture setter to perform the operation under test.

## Owned test types

Use existing core types where their semantics match. These definitions describe
the additional information needed; none are protocol payloads.

- **TestRepoSpec:** initial branch; Bare or Worktree; SHA-1 or SHA-256. A normal
  fixture uses fixed local identity and conversion settings. Tests deliberately
  exercising different configuration change it explicitly afterward.
- **TestCommitSpec:** tree source, ordered parent IDs, message, author, committer.
  Tree source is either an explicit complete tree or a snapshot of the current
  index. Capturing an index with unresolved stages fails. Signatures include
  fixed name, email, timestamp and timezone; there is no implicit wall clock.
- **TestTreeEntry:** raw repository-relative path and content: regular bytes,
  executable bytes, symlink target bytes, or gitlink commit ID. An explicit tree
  is complete, not an ambiguous list of patches. No worktree writes are implied.
- **TestCommit:** tree ID, ordered parents, message and signatures. File content
  is inspected through the existing `read_file_at_commit` method.
- **TestRefTarget:** Direct object ID or Symbolic full reference name. Direct
  targets can be commits or other existing objects, for non-commit-ref tests.
- **TestHead:** Attached full branch ref or Detached commit ID. An attached ref
  may be absent, representing an unborn branch.
- **TestIndexEntry:** raw path, object ID, mode, stage 0–3, assume-valid,
  skip-worktree and intent-to-add. Duplicate path/stage pairs and impossible
  stage combinations fail. Logical entries exclude volatile stat-cache fields;
  tests of native serialized index bytes belong to physical tests below.

Use existing string object-ID conventions at this boundary, with validation
against the repository's object format. Do not assume IDs are 40 characters.
Validate repository-relative paths before writing files. Raw path bytes are
needed for existing non-UTF-8 path cases; do not force everything through String.

The fake must retain immutable commits/trees, ordered parent relationships,
refs, HEAD, index entries and local configuration. Mutations affect subsequent
production observations. Object identities must be content-addressed and honor
the declared format; tests that assert Git object hashes require the same object
encoding, using existing hashing utilities rather than invented/truncated IDs.
The current fake needs further work to meet this contract; this is not a claim
that its present simplified object/index model already supports these cases.

Fixture initialization must not read machine credentials or invoke hooks.
Tests of production credential/config resolution still exercise their production
methods with explicitly installed test configuration.

## Later extensions justified by existing call patterns

Add these when converting their consumers, rather than implementing every one
before the two expensive matrices can move:

| Method | Contract and consumers |
| --- | --- |
| `test_write_object(repo, object) -> String` | Write a typed blob, tree, commit or annotated-tag object without making it reachable. Needed for dangling-object, non-commit-ref and protected-object inspection cases. Commit input reuses TestCommitSpec semantics. |
| `test_read_object(repo, oid) -> TestObject` | Return owned typed object contents for those assertions. Never a native handle. |
| `test_replace_reflog(repo, ref_name, entries)` | Install an explicit ordered reflog independently of the current ref. Needed for reflog-only reachability and stash-stack arrangements. |
| `test_read_reflog(repo, ref_name) -> Vec<TestReflogEntry>` | Observe exact target/message/signature entries in documented newest-first order. An absent log is empty. |
| `test_add_worktree(repo, path, spec)` | Create a linked worktree sharing objects/refs while retaining its own HEAD/index. Fake mode must model this relationship; do not treat it as an independent clone. |
| `test_native_layout(repo) -> TestNativeLayout` | Return actual git/common/worktree directory paths for physical filesystem tests. Native-only: fake mode returns UnsupportedOperation explicitly. |

TestObject and TestReflogEntry are cfg(test), owned descriptions with explicit
object kinds, identities and ordering. There is no generic “run any Git command”
method, arbitrary script, or `with_native_repository` escape hatch.

The native-layout exception is for tests whose subject is the physical storage:
linked metadata directories, inode replacement, pack files, OS locks, symlinks,
permissions and durability. Run their existing harness in real mode explicitly.
Do not turn UnsupportedOperation into a passing or silently skipped test. A
fake cannot establish an OS filesystem guarantee. Ordinary recovery-policy
matrices retain real journal files, but do not need native object/index storage.

Do not add universal `test_corrupt_anything` or success/failure-result setters.
Model logical foreign state with the specific setters above. Existing service
interruption hooks remain responsible for stopping before/after an action;
fixture methods must not duplicate GWZ's recovery decisions.

## Example: create history, then test a checked mutation

Illustrative Rust using the proposed types:

```rust
#[test]
fn changed_backup_ref_is_rejected() {
    let git = make_repository();
    let temp = tempfile::tempdir().unwrap();
    let repo = temp.path();
    git.test_init_repo(repo, &TestRepoSpec::default()).unwrap();

    let first = git.test_create_commit(repo, &first_spec()).unwrap();
    let second = git.test_create_commit(repo, &child_spec(&first)).unwrap();
    git.test_set_ref(repo, "refs/heads/main", Some(&TestRefTarget::Direct(second.clone()))).unwrap();
    git.test_set_head(repo, &TestHead::Attached("refs/heads/main".into())).unwrap();
    git.test_set_ref(repo, "refs/gwz/merge/case/root/head", Some(&TestRefTarget::Direct(first.clone()))).unwrap();

    let error = git.create_backup_ref_checked(
        repo, "main", &second, "refs/gwz/merge/case/root/head", &second,
    ).unwrap_err();
    assert_eq!(error.code, ErrorCode::PreservationEvidenceMismatch);
    assert_eq!(git.read_ref(repo, "refs/gwz/merge/case/root/head").unwrap(), Some(first));
}
```

first_spec/child_spec merely build TestCommitSpec values; they do not create
repositories or decide real/fake. The same test runs under either factory mode.

## Implementation plan

Implement in the existing codebase. Keep one GitRepository trait, one real
implementation, one fake, and the current process-wide factory selection. No
new DI framework, parallel interface, or separate real/fake scenario bodies.

- [x] **Add the fixture API.** Put the initial nine method declarations in
  `gwz-core/src/git/gitbackend/contract.rs`, with owned types in a sibling
  test-only module. Apply cfg(test) to every added method and type. Implement
  the native methods and forward them through `factory/test_repository.rs`.
  Build the methods in small groups with their first consumers.
- [x] **Implement the same methods in the shared fake.** Extend
  `fake_repository.rs` for explicit commit trees/parents/signatures, refs and
  HEAD, full index entries and local configuration. Keep state shared across
  factory handles and isolated by repository path. Add each behavior assertion
  once, then run it in both modes; preserve exact failure codes and state after
  refusal. Unsupported behavior must fail explicitly.
- [x] **Convert common fixture helpers.** Replace direct native initialization,
  commit_file, ref/HEAD edits, index setup and local config access with the
  interface. Use existing production methods when they already provide the
  operation; use test-only setters for arranging otherwise inaccessible state.
  Ordinary worktree-file writes and journal filesystem operations stay ordinary
  filesystem operations. Retain scenario assertions and remove redundant
  constructor-specific wrappers.
- [x] **Route construction through the factory.** Replace direct backend
  construction in converted production paths and fixture helpers with
  make_repository(). Move native bypasses in those production paths behind
  existing or necessary production GitRepository methods; production code must
  never call a test-only fixture method. Preserve explicit construction inside
  the factory and native adapter implementation. Leave unrelated migrations
  for subsequent batches.
- [x] **Convert the two slow root matrices first.** Update
  `v1_lifecycle/tests/reverse_preservation/mod.rs`, `root_fault_matrix.rs` and
  `root_ambiguity_matrix.rs` to use the factory/interface for fixture creation
  and deliberate foreign state. Complete the fake's existing
  commit_gwz_paths_checked, repository_state, checkout observations and root
  preservation preparation/observation/execution methods. Admit the shared
  factory test implementation through the existing authority marker only under
  cfg(test); keep the production boundary sealed. Preserve CheckedV1Store and
  the actual recovery logic, including interruption and no-repeat assertions.
- [x] **Verify and measure that batch.** Run the same converted scenarios in
  fake and real processes, then compare their results and elapsed times with
  the recorded baseline. Keep routine fake coverage and a small explicit real
  cross-check; make the complete real matrix available as an explicit run.
  Do not silently truncate scenarios, skip unsupported cases, or report a
  projected speedup as a measurement.
- [ ] **Repeat for remaining callers.** Work through the audited test/fixture
  bypasses by helper and behavior, adding the later object/reflog/worktree
  methods only when needed. Native storage, OS lock and durability tests remain
  explicitly physical. Migrate external integration tests only where their
  build arrangement can honor the test-only visibility described above.

### Checks for each batch

Start with a focused failing test, implement, and run that same test in both
modes. Once a converted group passes, proceed; do not rerun the full suite after
every helper edit. For the current contract scenarios, run from gwz-core:

```sh
GWZ_TEST_GIT=fake cargo test --locked --lib repository_contract_tests
GWZ_TEST_GIT=real cargo test --locked --lib repository_contract_tests
```

Use the corresponding test filter for each matrix or later converted group.
Check the production build and normal relevant lint checks after interface
changes. Update existing boundary rules only where the deliberate refactor
changes the boundary they describe. Do not run compiler mutation probes, add
pass-count pins, or introduce inventory/reconciliation gates.

### Completion criteria

The converted tests use one factory-selected scenario body and do not open
native Git handles. Their assertions pass in both modes, including fresh-handle
reopening, fixture isolation and preservation of user work. Production builds
contain neither fixture methods/types nor the fake selector. Physical-only
coverage remains explicit. The two matrices have measured before/after timings,
and any remaining unmigrated callers are identified rather than claimed done.

The audit's 290 test/fixture call sites are migration candidates, not 290 API
methods and not proof that every site can be simulated. The initial nine methods
cover recurring setup needs; the remaining physical cases retain real Git.

## First migration batch — 2026-09-08

The nine fixture methods are now declared under `cfg(test)` and implemented by
both backends. The first consumers use ordinary files, explicit commits and
parents, ref/HEAD changes, index entries, and local config. The fake rejects
unsupported bare repositories, non-regular commit trees, non-UTF-8 index
paths and unimplemented pathspec behavior explicitly; later fixtures must add those behaviors before
using them. It does not invoke Git or create a native object database.

The common root-preservation fixtures use the fixture API. Both large matrices
select their backend with `make_repository()`, including deliberate foreign ref
edits. They retain their complete scenario loops and real CheckedV1Store journals.
Other consumers of the common helper remain explicitly native until migrated.

The first matrix exposed production bypasses as well as fixture bypasses:
workspace runtime startup and pre-catalog observations opened Git directly to
resolve storage paths and read the index. These now obtain owned repository
paths and lossless index facts through the production GitRepository interface.
Directory retention, identity checks, filesystem observations and journals stay
in their existing implementations. Root preservation now uses the same protocol
implementation in both modes, with Git index and normalized-image operations
routed through that same interface. These production reads and mutations are
separate from the nine test-only fixture setters; production never calls a
fixture setter.

The temporary mixed migration is run as separate processes, each with one
immutable global mode. `gwz-core/scripts/run_tests.py` runs the migrated group in
fake mode and all remaining tests in native mode, retaining the small repository
contracts in both. `--compare` runs the complete converted group in both modes.
Release and full-matrix workflows use this runner; targeted native workflows
explicitly select real mode. Direct unconverted unit-test filters likewise need
`GWZ_TEST_GIT=real` until their fixtures have been migrated.

Validation and matrix timings are recorded in GwzTestPerformance.md. Remaining
callers, broader fixture semantics, and explicitly physical tests are subsequent
batches; this batch does not claim to have converted all audited bypasses.
