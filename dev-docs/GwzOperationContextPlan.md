# GWZ operation context

Status as of 2026-09-09: V1 merge lifecycle and catalog acquisition carry owned
services; several other workspace entry points now share their backend's
services too. Remaining downstream wrappers and older fixtures still need
closure. The exact current scope is in
[the post-v1.0.8 inventory](GwzDebtRecoveryScope-2026-09-09.md): 14 production
filesystem-factory calls in six files and 146 test/support factory calls in
39 files, plus indirect helper/constructor callers. Earlier dated sections below
are implementation history, not the current file list.

## Decision

Use one explicitly constructed `OperationContext` containing the Git and
filesystem dependencies used by an operation. Construct it at the core entry
boundary and borrow it down the call tree. Keep one `GitRepository` interface
and one `FileSystem` interface, with their existing native and test-only memory
implementations. No dependency-injection framework is needed.

The useful change is ownership and lifetime. Merely putting `make_repository()`
and `make_filesystem()` behind context methods would leave the global state and
hidden construction in place.

## Starting constraints

- `src/git/gitbackend/factory.rs` and `src/filesystem/factory.rs` construct
  adapters wherever callers need them. Production selection is native only;
  test selection is fixed once per process in `src/test_backend.rs`.
- `git/gitbackend/fake_repository.rs::shared` owns a process-wide repository
  map. `filesystem/fake.rs::registry` locates memory workspaces through another
  process-wide map. Fake Git reads its worktree through `make_filesystem()`.
- `filesystem/retained.rs` implements directory conveniences and standard
  `Read`/`Write`/`Seek` operations by calling the filesystem factory. Those
  handles must retain their owning filesystem when the global factory goes.
- Many workspace handlers already receive a generic `B: GitBackend`, which is
  the compatibility name for `GitRepository`. Reuse that existing dependency
  path instead of adding a second Git dependency beside it.
- `GitRepository::with_transport` returns `Self` and requires `Self: Sized`.
  Configure the concrete repository for an invocation before exposing it as a
  trait object. Credential selection and transport observations must not become
  mutable application-wide state.

## Ownership and API shape

Use a private, ordinary Rust struct with typed fields. The conceptual shape is:

```rust
pub(crate) struct OperationContext {
    filesystem: Arc<dyn FileSystem>,
    repository: Arc<dyn GitRepository + Send + Sync>,
}
```

This shape is now implemented in `gwz-core/src/operation_context.rs`; the concrete
repository implementations satisfy the thread bounds. Existing
handlers that only borrow Git can accept `&(impl GitRepository + ?Sized)`;
orchestration that needs both dependencies receives `&OperationContext`.
Pure model functions receive neither.

A core constructor configures native Git with the request's transport options,
then constructs the context. Public CLI/Python entry signatures remain valid:
their compatibility wrappers construct a native context and call internal
context-aware operations. Existing callers supplying a repository must retain
that repository and its configured authority; never replace it inside a wrapper.
Add a borrowed context view where necessary to preserve those public generic
entry points without cloning or extending the supplied repository's lifetime.

An application may reuse the native filesystem service across calls. Each
invocation owns its transport selection, observations, mutation leases and
request state. Do not keep workspace locks or open mutation transactions on a
long-lived application object. Cloning an `Arc` shares a service; it does not
mean holding a mutex across the whole operation.

Opaque filesystem handles retain a reference to the filesystem that created
them. Their convenience methods and stream traits dispatch through that owner.
The adapter's raw handle payload must not hold a reference back to the public
wrapper, avoiding an ownership cycle. Reject operations combining handles from
different memory worlds, including cross-context rename.

## Test construction

Introduce a test-only `TestWorld` that owns the repository map and filesystem
namespace. It constructs a compatible pair: native Git/native filesystem,
fake Git/native filesystem where needed during migration, or fake Git/fake
filesystem. Native Git/fake filesystem remains invalid.

Each test normally owns a new world. Reopening an operation within that test
uses a new context borrowing or sharing the same world, so recovery sees the
same files and Git state. A different world remains isolated even if it uses
the same path spelling. Fake Git receives that world's filesystem explicitly;
it must not look up the process factory from inside its implementation.

The runner can continue selecting a backend through its existing environment
variables. It reads that choice when creating test worlds, rather than changing
global production behavior. The same contract test body runs under each
supported mode. Fake types and constructors stay under `cfg(test)`; there is no
Cargo feature or product option that enables memory storage.

## Implementation sequence

1. Introduce owned test worlds and the context types. Add a focused isolation
   test for two worlds using identical paths, plus a reopen test that shares one
   world across two contexts. Use existing adapter contract bodies for backend
   parity; avoid duplicating them into fake/native tests.
2. Give filesystem handles their owner and inject the filesystem into Fake Git.
   Move memory registries into `TestWorld`. Convert retained-handle conveniences
   and cleanup/drop paths, which otherwise silently restore global lookup.
3. Convert one complete operation: V1 lease activation and root-preservation
   recovery, including catalog bootstrap, repository relationship discovery,
   journal observation, durable publication and fixture teardown. Pass one
   context from entry to cleanup. Keep the current scenario assertions.
4. Move the remaining operation families in coherent batches. Preserve public
   wrappers during migration. Once a family's callers are converted, prohibit
   factory use in that family with the existing source-boundary checker.
5. Remove internal global factory lookup and global fake registries when their
   last callers are gone. Keep production construction at entry boundaries.
   Recheck CLI and the rebuilt Python extension against the changed local core.

Do not mix this with the outstanding Git fixture conversion, encrypted SSH
agent selection, file splitting, or protocol changes. The next implementation
batch should be steps 1–3, followed by review of the actual plumbing cost.

## Completion evidence

The first batch is complete when the selected operation passes unchanged
behavioral scenarios with both backend pairs, shared-world reopening works,
separate worlds cannot see one another, and its source contains no global
factory lookup. Native consumer checks must still pass with test-selector
environment variables present. Measure ordinary runner time once the batch is
stable; this architecture is primarily for explicit ownership and isolation,
not a promised speed improvement.

Use ordinary targeted tests and the routine runners. Compiler mutation probes,
expected test counts and repeated full-suite gates are outside this work.

## First implementation batch — 2026-09-08

Implemented:

- `operation_context.rs` owns the two services. `TestWorld` constructs an
  isolated compatible pair and reopens contexts sharing that pair. The existing
  `operation::OperationContext` still carries request metadata; this batch does
  not change its public fields or CLI/Python request signatures.
- `BorrowedOperationContext` preserves a caller-supplied Git implementation.
  The shared root-preservation protocol receives that view; the native and fake
  Git adapters supply their own filesystem. Native transport configuration
  retains that filesystem when constructing its configured repository.
- `FsDirectory` and `FsFile` retain their filesystem owner. Reads, writes,
  seeks, identity probes, locks, publication, checked residue and cleanup use
  that owner. Memory adapters reject handles from another world. Windows fake
  durable volume identities now distinguish worlds too.
- The context-taking V1 lease acquires runtime locks, discovers and revalidates
  Git relationships, activates/reopens the catalog and bootstraps merge parents.
  Retained catalog witnesses carry the same context into provider callbacks.
- Stored V1 records retain their context. Journal creation, rewrite, reload,
  archive and temporary cleanup use supplied resources. The below-capability
  publication and parent-bootstrap paths accept the filesystem explicitly too.
- `scripts/checks/check_filesystem_boundary.py` rejects ambient factory lookup
  in the migrated helper modules. `scripts/run_tests.py` includes the new
  context contracts in its fake pair and native comparison runs.

The contracts exercise independent histories, files and locks at identical
paths in two memory worlds; handles surviving context drop; catalog and journal
reopening; archive reconciliation; and root-preservation reopening between
physical steps, including an interruption after source retirement and an
idempotent stash retry. They share test bodies across backend selections.

The remaining call-chain closure is explicit: whole merge-service and
preservation-bundle orchestration still enter through compatibility constructors.
Catalog batch acquisition and other workspace operation families also retain
their old entry path. `FakeGitRepository::shared` and `FakeFileSystem::shared`
remain solely to support those existing callers. Do not describe the entire
core, or implementation step 3's complete service call chain, as migrated yet.
The follow-up below closes that orchestration. Remove the remaining compatibility
constructors as the other families migrate.

This batch demonstrates the actual plumbing cost: retained handles eliminate
most downstream parameters, while path-based acquisition and Git relationship
checks require an explicit context. It makes no test-speed improvement claim.

Validation on macOS:

- The ordinary library runner exercised 18 filesystem contracts, 126 migrated
  cases and 1,863 native cases (plus the runner's native crosscheck). One native
  source assertion still required the old factory spelling; it was updated and
  passed its targeted rerun. No behavioral failure was found. The four runner
  phases totaled 224.56 seconds, including recompilation and concurrent consumer
  builds; this is not a controlled before/after performance comparison.
- All four context contracts pass with fake Git/fake filesystem, fake Git/native
  filesystem and native Git/native filesystem. The final native preservation
  rerun passed 51 tests after removing the remaining native image-reader factory
  calls. Core's four integration targets passed 58 tests.
- CLI: 253 tests passed. The rebuilt Python extension and cross-driver checks:
  56 tests passed. Both consumer runs used `GWZ_TEST_GIT=fake GWZ_TEST_FS=fake`
  while their production core remained native.
- `cargo clippy --all-targets -- -D warnings`, filesystem/context and
  checked-artifact source guards, seven Python guard/runner tests, formatting
  and whitespace checks passed. Compiler mutation tests were not run.

## Merge and catalog call-chain completion — 2026-09-08

The first batch was committed as core `cfa14b8`, workspace `39dab61`.

V1 start/continue/abort/status entry points now construct services from the
supplied merge backend. The existing sealed backend interface has a private
service-sharing method: native Git clones its configured adapter, retaining its
filesystem, credential selection and shared transport observations; test Git
clones the same world. This adds no requirement to the open `GitRepository`
interface or public CLI/Python request signatures. The borrowed context remains
appropriate for adapter helpers already receiving borrowed dependencies.

`CheckedV1Store` owns a context. Its default constructor is test-only. The service
reuses that context for initial reads, leases, rechecks and journal commits.
Checked records retain it for downstream observers. This avoids adding both a
store and a separate context parameter throughout the service loop.

Converted paths include:

- Catalog batch preparation, common-Git/worktree association, deduplication,
  final lock acquisition, revalidation and retained provider callbacks.
- Crash-recovery admission, catalog-free and catalog-backed merge creation,
  the lifecycle loop, optimistic status reads and archive reconciliation.
- Root artifact observation, evidence rollback, root metadata rollback,
  preservation-bundle observation/publication and finalization observations.
- Raw marker/lock/boundary publication through the supplied filesystem.
  These remain raw atomic replacements; dependency ownership does not introduce
  a catalog requirement or change their existing recovery semantics.

New contracts cover catalog batch contention and reopening; a service transition
persisted into an isolated world; and the existing full dirty-root preservation
scenario using an isolated `TestWorld`, followed by archive publication and a
second destination-only archive acquisition. The same scenario runs with
memory or native storage. The filesystem source guard now prohibits ambient
factory lookup throughout the V1 lifecycle, reverse artifact helpers and catalog
target/association helpers.

The GitRepository and FileSystem contract suites also construct their own
`TestWorld` now; reopening explicitly reuses that world instead of a fresh
ambient factory call.

Validation:

- Ordinary library runner: 18 filesystem tests, 128 migrated cases, the native
  crosscheck and 1,865 native tests passed; one existing test remains ignored.
  The four phases took 196.14 seconds, including concurrent consumer work.
- The final contract-fixture conversion passed 25 Git/filesystem contracts
  with memory storage. The same contracts plus the full preservation/archive
  scenario passed with native storage (26 cases); the mixed Git-memory/native-FS
  run passed all 31 selected contracts and lifecycle cases.
- CLI: 253 passed. Rebuilt Python bridge/merge/cross-driver checks: 56 passed.
- Clippy with warnings denied, both source-boundary checks, seven Python
  checker/runner tests, formatting and whitespace checks passed.
- Compiler mutation tests were excluded. No release or push was performed.

The build exhausted local disk during this work. `cargo clean -p gwz-core`
removed rebuildable package outputs, leaving dependencies available; subsequent
verification disabled incremental compilation to keep its cache from regrowing.

Remaining scope, corrected against the 2026-09-09 checkout:

- Several bootstrap/branch/commit/stage/repo/stash/materialize entry points now
  obtain `OperationServices::for_merge(backend)`. Do not redo that composition.
  Some downstream calls still use ambient wrappers, so entry-point conversion
  alone is not proof of a completely closed call chain.
- Production factory calls remain in `src/artifact/{mod,conf_integrity}.rs`,
  `src/durable_fs.rs`, `src/workspace_ops/handle_create_repo.rs`,
  `src/workspace_ops/merge/record_wire/location.rs`, and
  `src/workspace_ops/sync_workspace_boundary.rs`. The inventory names each
  function and its identified callers.
- Older fixture helpers and factory contracts still use shared fake state.
  Migrate their arrangement, observations, reopen and cleanup to the same owned
  world before deleting `shared()` and the compatibility factories.
- `OperationServices::existing()` and `native()` are already test-only.
  Preserve deliberate native tests and native adapter composition; eliminate
  hidden compatibility lookups, not the real implementations or backend matrix.
