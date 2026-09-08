# GWZ operation context

Status: proposed next step after the checked-artifact filesystem migration.
This document designs the change; the factories remain in production today.

## Decision

Use one explicitly constructed `OperationContext` containing the Git and
filesystem dependencies used by an operation. Construct it at the core entry
boundary and borrow it down the call tree. Keep one `GitRepository` interface
and one `FileSystem` interface, with their existing native and test-only memory
implementations. No dependency-injection framework is needed.

The useful change is ownership and lifetime. Merely putting `make_repository()`
and `make_filesystem()` behind context methods would leave the global state and
hidden construction in place.

## Current constraints in this code

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

This is an ownership sketch, not an existing API. Audit the concrete repository
implementations' thread bounds before adopting the exact declaration. Existing
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
