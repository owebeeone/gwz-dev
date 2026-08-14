# R4b R2 consumer-checkpoint remediation — eighth independent filesystem re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

P0: 0. P1: 0. P2: 1. P3: 0.

The private-supertrait `MergeAuthorityBackend` correction is sound in the
reviewed tree. An import alias, a re-export, a fully qualified path, or a
macro-generated implementation cannot make another backend satisfy the
private marker. All present v1 observation and physical-execution routes which
accept a backend require that sealed trait, the checked bundle accepts no
backend, and ordinary non-v1 `GitBackend` test doubles still compile.

One P2 remains in the concrete-observer consumer boundary. The purported
crate-wide inventory reads only files whose names end in `.rs`. Rust can
compile a source file with any suffix through `#[path]` or `include!`. In an
exact-SHA export, I placed a raw writer and a fifth concrete-observer call in
an `.inc` file, attached it to the existing unprotected preservation artifacts
module, and routed the already-used v1 `v1_preservation_image` path through
it. Both the structural checker and exact all-target/all-feature Clippy exited
zero. I reproduced the same result independently with `#[path]` and
`include!`. The positive observer reference inventory therefore does not
cover the source graph the compiler actually builds.

## Exact tuple reviewed

- workspace root: `43760d9e7ec9327fb5564f36669efc17d6d7ed6b`
- `gwz-core`: `3569a3d819c47256f8da326d4e48c27bd96e81a4`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed `gwz status --json` and direct `git rev-parse HEAD` checks agreed on
this exact clean tuple before the review and again immediately before this
report was written. All adversarial changes were made in exact-commit exports
under `/tmp`; reviewed production source was not changed. This report is the
only workspace edit made by this reviewer, and all three reviewed identities
remain unchanged after it.

## Eighth-correction audit

### Backend seal and v1 reachability

`git/gitbackend/authority_backend.rs:5-19` defines the public authority trait
with a private `sealed::Sealed` supertrait and implements both traits only for
`Git2Backend`. The file and its declaring `gitbackend.rs` module are
byte-pinned, and the authority module also has the compiler writer `forbid`.

I appended an import-aliased, macro-generated implementation for the ordinary
`TrackingBackend` test double in an exact export. All-target/all-feature
Clippy failed at type checking:

```text
error[E0277]: the trait bound `TrackingBackend: Sealed` is not satisfied
help: the trait ... `Sealed` is implemented for `Git2Backend`
note: required by a bound in ... `MergeAuthorityBackend`
```

I then replaced that probe with a direct attempt to implement the marker by
its fully qualified internal path. Compilation failed independently:

```text
error[E0603]: module `gitbackend` is private
... trait `Sealed` is not publicly re-exported
```

These compiler results also cover a re-exported spelling: changing the name or
using a macro does not remove the inaccessible supertrait obligation. An
alternate production module cannot name the private marker. Adding an
implementation or a child/module edge inside `authority_backend.rs` changes
its protected exact bytes. A wrapper can coerce to `Git2Backend` only by using
the reviewed concrete implementation; it cannot substitute wrapper callback
semantics.

The complete present non-test-source v1 backend inventory contains no
`GitBackend` identifier. The production runtime adapters are:

- `ForwardRuntime<B: MergeAuthorityBackend>` for observation and execution;
- `ReverseRuntime<B: MergeAuthorityBackend>` for observation and execution;
- `FinalizationRuntime<B: MergeAuthorityBackend>` for observation and
  execution; and
- sealed generic bounds on status, archive, every nested authority observer,
  preservation/rollback execution, and the v1 helpers in preservation plan
  and artifacts.

Internal `ExactObserver`/`PhysicalExecutor` test runtimes remain usable for
fault matrices, but the production adapters which connect a Git backend to
those interfaces all require `MergeAuthorityBackend`. The current
`TrackingBackend: GitBackend` implementation compiled under the exact-tree
all-target check, establishing that ordinary non-v1 backend extensibility was
not accidentally sealed.

### Checked bundle and protected observation path

`workspace_ops/merge/preserve/checked_bundle.rs:19-112` has no backend import,
generic, argument, stored field, callback, or injection point. Its three
visible adapter operations accept only the root, record, plans, and owner and
use the concrete stash observer internally while the exact file digest and
local compiler `forbid` protect the whole helper graph.

`preservation_stashes` remains absent from the open `GitBackend` contract.
`git/gitbackend.rs:163-177` exposes the concrete observer only crate-wide and
terminates directly in the byte-pinned `preservation_image::preservation_stashes`
leaf. Every currently visible `.rs` reference is one of the five expected
definition/caller files, and all approved callers remain under their exact
file or authority-tree source boundary and compiler boundary.

The defect below is that the checker does not enumerate every compiler-loaded
source file, so this current-tree inventory is not fail-closed.

## Finding

### [P2-1] Non-`.rs` Rust module indirection bypasses the concrete-observer reference inventory

**Where:** `gwz-core/scripts/checks/check_checked_artifact_boundaries.py:349-356`
and `:511-520`; RemPlan section 1 and exit criterion 8;
`gwz-core/src/workspace_ops/merge/preserve/artifacts.rs:657-674`; and
`gwz-core/src/git/gitbackend.rs:163-177`.

**Violated contract.** The eighth correction says the source gate freezes the
complete crate-wide concrete-observer reference set so a new wrapper or
consumer fails closed. The checker defines production source as
`source.rglob("*.rs")`. Neither Rust's `#[path = "..."]` module attribute nor
its `include!("...")` macro requires the loaded file to have an `.rs` suffix.
The `.inc` source is compiled but is absent from both the observer inventory
and every protected tree digest outside the already protected authority tree.

**Executable counterexample.** I exported exact core SHA
`3569a3d819c47256f8da326d4e48c27bd96e81a4` and added this unprotected module
edge to `workspace_ops/merge/preserve/artifacts.rs`:

```rust
#[path = "unreviewed_observer.inc"]
mod unreviewed_observer;
```

The new `unreviewed_observer.inc` contained:

```rust
#[allow(dead_code)]
pub(super) fn observe_after_write(
    path: &std::path::Path,
) -> crate::model::ModelResult<Vec<crate::git::GitPreservationStashEvidence>> {
    let _ = std::fs::write(path.join("fs8-unreviewed-writer"), b"bypass");
    crate::git::observe_preservation_stashes_read_only(path, "merge_probe")
}
```

I changed the existing test-gated production
`v1_preservation_image` function to invoke
`unreviewed_observer::observe_after_write(&plan.path)?` before its existing
authority observation. That function is already reached by the selected v1
preservation authority graph. No protected v1 caller or sealed-backend source
changed.

Both mandatory gates incorrectly accepted the compiled mutation:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
Checking gwz-core v0.10.3 (/private/tmp/gwz-fs8-path-probe.W8MhSL)
Finished `dev` profile ...
```

I then replaced the module edge with:

```rust
include!("unreviewed_observer.inc");
```

and called the included function directly. The same checker and exact Clippy
command again exited zero. Direct `.rs` aliases, wrappers, re-exports, and new
files are caught by the current textual inventory or protected digests, but
these two standard Rust source-loading forms hide the observer identifier and
the raw writer in a compiler-loaded non-`.rs` file.

**Impact.** The sealed backend closes the FS-7 alternative-backend attack, but
this counterexample enters an already-used v1 authority helper without any
alternative backend. It can mutate the repository immediately before the
reviewed authority observation and then return the concrete observer's result.
PR/push, release CI, and local release all execute the same two gates which
accepted the mutation. Consequently the concrete observer's claimed fixed
consumer set and selected read-only authority path are not enforced.

**Required correction.** Make the source inventory cover the Rust source graph
the compiler can load. A complete correction should either:

1. reject every unapproved `#[path]` and `include!` edge in crate source and
   positively pin the small approved set, including macro-produced spellings;
   or
2. resolve and inventory every such edge, require the target to be a regular
   in-tree file, and scan/pin its exact path and bytes regardless of suffix.

Add compiling negative regressions for both forms above, with the helper
called from the existing `v1_preservation_image` path. They must fail the
structural gate independently of compiler writer lint coverage. Retain the
sealed backend and all existing exact source/tree/forbid protections.

## Prior-boundary regression evidence

- **Archive semantics:** the contradictory terminal-labelled v0 case and the
  exact terminal-open/absent-destination case each passed 1/1. The complete
  96-test checked-artifact interface matrix also passed, including terminal
  v1 and archived-source negatives.
- **Capability privacy and source/compiler boundaries:** the committed
  boundary suite passed 38/38. It covers visible-entry classification,
  re-export/capability escape, checked helper call graphs, exact source/tree
  manifests, compiler `forbid`, direct `.rs` wrappers, alternative-backend
  compilation, and open-trait regression.
- **CI:** `.github/workflows/checked-artifact-boundary.yml:6-31` still runs on
  PR and main push with Python 3.11, the checker, all checker/release tests, and
  exact all-target/all-feature Clippy. The release workflow runs the same
  boundary and Clippy.
- **Local release:** all 6 release-boundary tests and all 7 publish-workflow
  tests passed. `release.py:372-410` still checks exact SHA and cleanliness
  before and after the boundary, converges new-tag paths on the finalizer,
  gates existing-tag retry, and tags/pushes explicit verified SHA refspecs.
- **Static quality:** reviewed-tree source checker, `cargo fmt --all -- --check`,
  `git diff --check`, and exact all-target/all-feature Clippy passed.

## Focused command evidence

Reviewed-tree commands included:

```text
gwz status --json
git rev-parse HEAD
git -C gwz-core rev-parse HEAD
git -C gwz-cli rev-parse HEAD

../.venv/bin/python scripts/checks/check_checked_artifact_boundaries.py
../.venv/bin/python -m unittest scripts/checks/test_check_checked_artifact_boundaries.py -v
../.venv/bin/python -m unittest scripts/checks/test_release_boundary.py -v
cargo test checked_artifact::interface_tests
cargo test archive_authority_rejects_a_terminal_label_with_a_contradictory_v0_envelope
cargo test archive_authority_requires_one_terminal_open_source_and_an_absent_destination
cargo test --test publish_workflow
CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings
cargo fmt --all -- --check
git diff --check ef3e2c2ec9d19f56b47173a29c89d37a48a1af00..3569a3d819c47256f8da326d4e48c27bd96e81a4
```

Results:

- structural/compiling boundary suite: 38/38 passed;
- checked-artifact interface matrix: 96/96 passed;
- release-boundary suite: 6/6 passed;
- publish-workflow suite: 7/7 passed;
- focused archive tests: 1/1 and 1/1 passed;
- exact-tree checker, format, diff check, and all-target/all-feature Clippy:
  passed;
- import-alias/macro authority implementation: compiler rejected;
- direct private-marker implementation: compiler rejected;
- reachable `#[path]` non-`.rs` writer/observer consumer: checker and Clippy
  incorrectly passed; and
- reachable `include!` non-`.rs` writer/observer consumer: checker and Clippy
  incorrectly passed.

The parent had already run the complete core suite; this review used the
requested focused evidence rather than repeating that long run.

## Gate decision

Do not begin R2 production conversion on this tuple. Preserve the sealed v1
backend correction, but close the compiler-loaded source inventory gap and
repeat both independent settled-tree reviews. This review returns **NO-GO**.
