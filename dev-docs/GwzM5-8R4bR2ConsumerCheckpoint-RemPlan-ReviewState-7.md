# R4b R2 consumer-checkpoint remediation — seventh independent state/release/consumer-boundary re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

R2-I8 closes the State-6 direct-caller defect. Every current caller of the
concrete stash observer is covered by a complete positive source boundary and
a compiler boundary, including the complete nested authority-observer source
tree.

One P2 remains in the same authority decision, demonstrated by two independent
counterexamples. First, the protected callers still invoke other pre-existing
open `GitBackend` observation callbacks immediately before or after the
concrete stash read. A public, import-aliased production backend can override
one of those callbacks with a raw writer while leaving every protected byte
unchanged. Second, the checker does not inventory references to the concrete
observer itself: an unprotected test-gated helper can write and then invoke it.
The structural checker and exact all-target/all-feature Clippy accept both
compiled changes. Freezing the current callers therefore neither freezes the
implementation which supplies the rest of the authority fact nor closes the
concrete observer's consumer set.

P0: 0. P1: 0. P2: 1. P3: 0.

## Exact tuple reviewed

- workspace root: `7a600da21e47c4e1d4777efe34c0c695463d9910`
- `gwz-core`: `ef3e2c2ec9d19f56b47173a29c89d37a48a1af00`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed workspace status and direct `git rev-parse HEAD` checks agreed on
this clean tuple at dispatch. The three tracked identities were unchanged
after the executable probes; the only later root status entries were this
report and an independent filesystem State-7 report, both untracked documents
which changed no reviewed source or identity. Each counterexample was compiled
in a detached worktree of the exact core SHA. This report is my only workspace
edit.

## R2-I8 caller closure

### Complete caller enumeration

There are four production or test-gated production call sites for
`observe_preservation_stashes_read_only`:

| Caller | Positive source boundary | Compiler boundary |
| --- | --- | --- |
| `workspace_ops/merge/preserve/checked_bundle.rs` | exact file SHA-256 | local module `forbid` |
| `workspace_ops/merge/preserve/plan.rs` | exact file SHA-256 | local module `forbid` |
| `workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/phase.rs` | aggregate authority-observer tree digest | inherited root-module `forbid` |
| `workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/phase/evidence.rs` | aggregate authority-observer tree digest | inherited root-module `forbid` |

No import alias or function-pointer call to the concrete observer exists
elsewhere. Remaining `preservation_stashes` method calls are Git tests using
the ordinary inherent `Git2Backend` method. Remaining internal stash decodes
are inside the concrete protected Git implementation.

This is a complete inventory of the settled tree, but not an enforced
allowlist. The P2 counterexample below adds a fifth caller in an unprotected
test-gated production module without changing any of these four protected
files.

### Aggregate path-and-byte proof

I independently reimplemented the aggregate calculation rather than importing
the checker. The manifest covers the root
`workspace_ops/merge/v1_lifecycle/authority/observe.rs` plus all 18 descendant
Rust files. Its recomputed value is exactly:

```text
41d5d4db9f53b3275113d2f44caf36053c527824a5f0222270d759d4fde2e8e8
```

For every file, the preimage contains:

```text
u64-be(relative-path-byte-length) || UTF-8 POSIX relative path ||
u64-be(content-byte-length) || exact content bytes
```

Files are sorted by that complete relative path. This is unambiguous without a
separate row count: every row is length framed and parsing ends at the digest
preimage boundary. The path is included as well as the bytes, so addition,
removal, rename, movement, or content substitution changes the aggregate.
The root is included explicitly; the descendant scan includes unwired `.rs`
files as well as modules currently named by `mod` declarations.

The current tree contains no `#[path]` or `include!` indirection. Independent
probes adding either an external `#[path]` module or an external `include!`
target changed the protected root bytes and were rejected as a protected-tree
change. A new nested helper file was also rejected even before it was wired
into the module graph.

### Inherited compiler boundary

`observe.rs` has an executable inner
`#![forbid(clippy::disallowed_methods)]`. Rust lint levels inherit through its
file-backed descendants, and `forbid` cannot be lowered by a child `allow`.
The preservation-plan caller has its own identical inner guard. The checker
masks comments and strings before looking for the guard, so a commented-out
attribute does not satisfy it.

The committed adversarial suite and my source probes establish:

- direct raw writes in `plan.rs` and nested authority observation reject;
- imported/crate-local aliases and same-name function pointers change the
  positive manifest, while compiler-resolved listed writers are also denied;
- a new local helper or a changed nested helper changes the aggregate;
- a newly named backend call changes the protected caller bytes;
- `#[path]` and `include!` indirection changes the protected root bytes; and
- removed, disabled, or commented guards reject.

The complete CPython 3.11 boundary suite passed 40/40, including compiler
probes that reject `std::fs::write` in `plan.rs` and a nested authority
descendant. This closes the exact State-6 raw-writer counterexample.

## Prior contract regression audit

### Concrete stash observer

`preservation_stashes` remains absent from the open `GitBackend` trait. All
four callers use the crate-qualified concrete observer. Its byte-pinned
implementation still terminates directly in the byte-pinned
`preservation_image` leaf, which validates the merge ID and only enumerates and
decodes stash commits, parents, trees, and blobs using local read-only helpers.
A qualified, import-aliased, or macro-generated backend cannot override that
function itself.

The P2 below concerns other open callbacks which remain part of the same
authority calculation, not a regression in the concrete stash function.

### Archive semantics, identity, policy, and capability privacy

The R2-I8 source changes do not alter archive decoding, canonical-location
acquisition, owner/action encoding, managed policy, reservation derivation, or
general checked capability visibility.

Focused exact-tree evidence passed:

- all 96 `checked_artifact::interface_tests`, covering collision snapshots,
  immediate revalidation, managed prefix/overlap policy, owner/action framing,
  schedules, reservations, durable records, and capability roles; and
- all 24 archive tests, including terminal-envelope contradictions, canonical
  open/archive replacement, cleanup ownership, unknown fields, and v0/v1
  dispatch behavior.

The source inventory still reports no general `CheckedArtifact`, fact, policy,
or transition escape and keeps ordinary command call sites outside the checked
entry boundary.

### Release and CI gates

Release source is unchanged from the accepted State-6 correction. I nevertheless
re-executed all six `main` paths:

- existing tag, with and without push;
- already-versioned new tag, with and without push; and
- version-commit new tag, with and without push.

Every existing-tag push passed through the exact gate. Every new tag converged
through the finalizer in the order `exact gate -> explicit-SHA tag -> optional
explicit-SHA push`. The version-commit branch used the new commit SHA; the
other branches used the captured existing SHA.

Independent clean-tree probes showed:

- wrong pre-gate SHA and pre-gate dirt reject before the boundary suite;
- post-gate SHA drift and post-gate dirt reject after one suite execution; and
- an unchanged clean tree passes both reacquisitions.

The captured atomic push sources were exactly:

```text
<gated-sha>:refs/heads/main
<gated-sha>:refs/tags/v1.2.3
```

The PR/main-push workflow remains pinned to Python 3.11 and runs the source
checker, its tests, release tests, and all-target/all-feature Clippy. The exact
suite passed under CPython 3.11.14. Seven static publish-workflow tests, format,
and exact current-tree Clippy also passed.

## Finding

### [P2-1] The authority boundary neither seals adjacent callbacks nor closes the concrete observer's consumer set

**Where:** remediation section 7 and exit criterion 8
(`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`);
`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/phase.rs:160-219`;
`gwz-core/src/workspace_ops/merge/preserve/plan.rs:480-522`;
`gwz-core/src/workspace_ops/merge/preserve/artifacts.rs`;
`gwz-core/src/git/gitbackend/contract.rs:462-477`; and
`gwz-core/scripts/checks/check_checked_artifact_boundaries.py:15-43,220-233,455-500`.

**Violated contract.** Exit criterion 8 requires no selected checked-path raw
bypass, including an alternative backend observer override. R2-I8 freezes the
callers' source but does not seal the implementations of open callbacks already
present in those frozen callers.

The same criterion also requires the concrete observer's caller set to be
closed. The checker permits the function name in selected guarded callers but
does not enumerate all references to that crate-visible function. Its global
scan searches for the standalone identifier `preservation_stashes`; because
underscore is a word character, that expression does not match the longer
`observe_preservation_stashes_read_only` identifier.

The clearest sequence is `observe_plain_stash`:

```text
concrete observe_preservation_stashes_read_only(...)
backend.preservation_image(...)
backend.repository_state(...)
backend.head(...)
backend.read_ref(...)
classify the preservation authority state
```

`plan.rs` similarly calls the open `observe_direct_ref` callback before its
concrete stash read. These are not hypothetical added call sites; they are
already part of the protected digest.

In a detached export of exact core SHA
`ef3e2c2ec9d19f56b47173a29c89d37a48a1af00`, I added a public production
`AlternativeBackend` in unprotected `git/gitbackend/backend.rs`, imported
`crate::git::GitBackend as AlternativeGitBackend`, implemented the required
trait surface by forwarding to `Git2Backend`, and overrode the already-used
`preservation_image` callback as follows:

```rust
fn preservation_image(
    &self,
    path: &Path,
    include_untracked: bool,
) -> ModelResult<GitPreservationImage> {
    std::fs::write(path.join("alternative-backend-write"), b"mutated")
        .map_err(crate::git::io_error)?;
    Git2Backend::default().preservation_image(path, include_untracked)
}
```

The type is production compiled and publicly reachable through the already
pinned wildcard backend re-export. No protected caller, concrete stash
observer, or leaf byte changed. Both mandatory gates exited zero:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
CLIPPY_CONF_DIR=. cargo clippy --all-targets --all-features -- -D warnings
Finished `dev` profile ...
```

The crate-level lint deliberately allows raw writers outside selected guarded
modules, and the checker no longer proves a sole production `GitBackend`
implementation. Fully qualified and macro-generated implementations have the
same property; the import-aliased form is the compiled counterexample.

I separately reset another detached export to the same exact core SHA and
added this helper to the unprotected
`workspace_ops/merge/preserve/artifacts.rs` module:

```rust
#[cfg(test)]
#[allow(dead_code)]
fn unprotected_observer_helper(
    root: &Path,
) -> ModelResult<Vec<crate::git::GitPreservationStashEvidence>> {
    std::fs::write(root.join("unprotected-observer-write"), b"mutated")
        .map_err(crate::git::io_error)?;
    crate::git::observe_preservation_stashes_read_only(root, "merge_1")
}
```

This creates a fifth, direct authority-observer consumer and compiles it in
the all-target test configuration. It changes no protected manifest entry and
is outside every compiler `forbid` boundary. Both mandatory gates again
exited zero:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
CLIPPY_CONF_DIR=. cargo clippy --all-targets --all-features -- -D warnings
Finished `dev` profile ...
```

Import aliasing or assigning the concrete observer to a same-signature
function pointer in this unprotected helper does not improve containment; the
raw writer remains outside the compiler boundary and no complete observer
reference inventory rejects the new consumer.

**Impact.** The concrete stash read can succeed and then an alternative
backend can mutate the repository or another artifact before returning the
image used in the same authority classification. It can also fabricate that
image. An existing pre-read callback can perform the symmetric mutation before
the concrete observation. The source/tree manifests remain exact because the
raw writer lives behind an already-approved trait call. Thus PR, push, local
release, and release CI all remain green while a selected preservation
authority path has a raw successful bypass. Independently, new test-gated
authority code can acquire the concrete observer through an unprotected
write-capable helper, so even the observer's supposedly fixed consumer set is
not enforced.

**Required correction.** Close the implementation boundary, not only the
caller spelling. Viable designs include:

- make the complete preservation authority observation concrete, returning one
  opaque fact that combines stash, image, attached HEAD/ref, and repository
  state from the pinned Git implementation; or
- seal the production authority-observer trait at the type level so only the
  pinned `Git2Backend` implementation can satisfy it, with a separate
  test-only abstraction for fault injection.

In either design, make the complete concrete authority operation private to a
protected facade and enforce a positive inventory of every reference to that
facade, including `cfg(test)` code. A direct, aliased, or function-pointer
reference from any unprotected merge module must fail the source boundary,
and test-only fault injection must cross a capability-limited interface that
cannot perform repository or artifact writes.

Merely adding another implementation-name regex or pinning `backend.rs` is not
complete: an implementation can use a qualified/imported/macro trait path or
live in another production module. Add a compile-negative regression in which
a public alternative backend overrides each approved authority-read callback
with a writer; it must be impossible to pass that type into the authority
entry. Also add a source-negative regression which places a direct, aliased,
and function-pointer concrete-observer call behind a raw writer in an
unprotected `cfg(test)` helper; no form may pass. Preserve the R2-I8
source-tree manifest and inherited compiler guard as defense against
caller-local mutations.

## RemPlan exit-gate disposition

| Exit criterion | Disposition |
| --- | --- |
| 1-3 collision ownership, revalidation, policy and overlap | focused interface matrix passed; no regression found |
| 4-6 pre-record owner, durable-record identity, literal vectors | focused interface matrix passed; no regression found |
| 7 ordinary commands outside checked provider | source inventory passed; no regression found |
| 8 no raw/alias/alternative-backend bypass | **not met: P2-1** |
| 9 complete archive semantic validation | 24/24 archive tests and authority negatives passed |
| 10 exact source/compiler release gates and Python runtime | release-flow, SHA/dirt, push-refspec, Python 3.11 and static gates passed |
| 11 focused/full/static settled-tree checks | focused and static review checks passed; full-suite duplication was unnecessary after P2-1 established NO-GO |
| 12 two independent GO re-reviews | not met by this review |

## Focused verification summary

- independent exact-file digest recomputation: 7/7 matched;
- independent aggregate tree digest: matched across 19 framed paths/files;
- concrete observer callers: four found, four source-covered, four
  compiler-covered;
- CPython 3.11 structural/release suite: 40/40 passed;
- checked-artifact interface matrix: 96/96 passed;
- archive semantic matrix: 24/24 passed;
- static publish-workflow suite: 7/7 passed;
- format and exact current-tree all-target/all-feature Clippy: passed;
- external `#[path]` and `include!` probes: source gate rejected both;
- all six release branches plus pre/post SHA and dirt probes: passed expected
  ordering/rejection;
- explicit gated-SHA atomic refspec probe: passed;
- import-aliased alternative-backend writer probe: structural and compiler
  gates incorrectly passed; and
- unprotected test-gated writer plus direct concrete-observer call: structural
  and compiler gates incorrectly passed.

## Gate decision

Do not begin R2 production conversion on this tuple. Preserve R2-I8's complete
caller source manifest and inherited compiler guard, but extend the concrete or
sealed boundary through every backend observation which contributes to the
preservation authority fact and close the observer's complete consumer set.
Then repeat both independent settled-tree re-reviews.
