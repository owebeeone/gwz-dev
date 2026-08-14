# R4b R2 consumer-checkpoint remediation — seventh independent filesystem re-review

Date: 2026-08-14

## Verdict

**GO for the R2 production packages controlled by this checkpoint.**

P0: 0. P1: 0. P2: 0. P3: 0.

The R2-I8 correction closes FS-6 P2-1. The separate preservation-plan caller
is protected as one exact source file, and the v1 authority-observation root
now protects its complete current module tree through both an inherited
compiler `forbid` and a deterministic path-and-byte aggregate. Independent
new-file, nested-helper, out-of-tree `#[path]` helper, alias, lint-downgrade,
backend-callback, and guard probes all failed closed. I found no independent
P0/P1/P2 defect in the changed code or the full RemPlan exit boundary.

## Exact tuple reviewed

- workspace root: `7a600da21e47c4e1d4777efe34c0c695463d9910`
- `gwz-core`: `ef3e2c2ec9d19f56b47173a29c89d37a48a1af00`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Installed `gwz status --json` showed this exact tuple clean before review. All
adversarial mutations were made in exact-commit exports under `/tmp`; no
reviewed production source was changed.

## FS-6 P2 closure

### Separate preservation-plan caller

`workspace_ops/merge/preserve/plan.rs` now begins with
`#![forbid(clippy::disallowed_methods)]` and its exact bytes are listed in
`PROTECTED_SOURCE_DIGESTS` (`plan.rs:1`,
`check_checked_artifact_boundaries.py:28-35`). A direct writer, imported alias,
same-name function pointer, local wrapper, changed helper edge, commented or
removed guard, or alternative callback therefore changes the file digest. A
writer also independently reaches the non-overridable compiler lint.

The committed source and compiler counterexamples for a writer inserted before
the concrete preservation observer both passed by correctly rejecting their
mutated trees.

### Complete v1 authority-observation tree

`workspace_ops/merge/v1_lifecycle/authority/observe.rs` now establishes
`#![forbid(clippy::disallowed_methods)]` above all of its child modules
(`observe.rs:1-11`). Rust lint inheritance applies that forbid to every inline
and file-backed descendant; a descendant cannot lower a `forbid` level.

The structural gate separately computes one SHA-256 aggregate over:

1. the root `observe.rs` path and exact bytes;
2. the sorted relative path of every `*.rs` descendant under `observe/`; and
3. each descendant's exact byte length and bytes.

Lengths frame both paths and contents, so neither concatenation ambiguity nor
path/content substitution preserves the aggregate
(`check_checked_artifact_boundaries.py:38-43,423-437,455-470`). I independently
recomputed the reviewed value:

```text
41d5d4db9f53b3275113d2f44caf36053c527824a5f0222270d759d4fde2e8e8
```

The manifest contained the root plus all 18 current descendant Rust files.
There are no current `#[path]`, `include!`, symlink, or external-module edges
inside this tree; every current module edge resolves within that set.

## Independent adversarial evidence

### Nested new helper and attempted lint downgrade

In an exact export I added a new nested module/file whose helper imported
`std::fs::write` under the accepted observer-like local name. The child also
attempted `#![allow(clippy::disallowed_methods)]`.

The source gate independently rejected the changed path set and bytes:

```text
checked-artifact boundary: failed
- protected source tree changed: workspace_ops/merge/v1_lifecycle/authority/observe.rs
```

The compiler independently rejected the attempted lint downgrade:

```text
error[E0453]: allow(clippy::disallowed_methods) incompatible with previous forbid
... overruled by previous forbid at authority/observe.rs:1
```

### Moved/path-included helper outside the descendant directory

In a second exact export I added an out-of-tree sibling helper and attached it
as a child with `#[path = "unreviewed_authority_helper.rs"]`. Its same-name
function pointer called `std::fs::write`.

Changing the root module edge caused the tree aggregate to fail even though
the helper file itself was physically outside `observe/`:

```text
checked-artifact boundary: failed
- protected source tree changed: workspace_ops/merge/v1_lifecycle/authority/observe.rs
```

Because it remained a semantic child module, the inherited compiler boundary
also reached it:

```text
error: use of a disallowed method `std::fs::write`
note: the lint level is defined at authority/observe.rs:1
```

These probes establish independent source-gate and compiler-gate enforcement,
including the moved/path-included case. The committed suite additionally
rejects a new unreferenced helper file, nested writer helper, differently named
backend callback, imported and unlisted aliases, same-name function pointers,
direct raw writers, and commented/removed root guards. Since every exact byte
is pinned, a new local wrapper using an unlisted crate writer is rejected by
the source gate even when the compiler denylist has no name for it.

## Prior boundaries rechecked

- **Archive semantics:** the focused contradictory-terminal-v0 test and the
  exact terminal-open/absent-destination test each passed 1/1. Complete archive
  validation therefore remains required before authority issuance.
- **Concrete observer:** `preservation_stashes` remains absent from the open
  `GitBackend` trait. The crate-private concrete observer remains byte-pinned
  and calls the protected preservation-image leaf without trait dispatch
  (`gitbackend.rs:161-175`). Authority-sensitive call sites still name only
  this observer.
- **Capability privacy:** the source inventory still rejects visible-entry
  growth, re-exports, private capability escape, external helper re-entry, and
  unclassified users. Those committed probes passed.
- **CI and release:** PR/push still selects Python 3.11, runs the source checker,
  its tests, release-boundary tests, then exact all-target/all-feature Clippy.
  Local release still has no compiler skip, verifies SHA and cleanliness both
  before and after the gate, converges all new-tag paths on the finalizer,
  gates existing-tag retries, and atomically pushes both branch and tag from
  the explicit gated SHA (`release.py:322-413`).
- **Explicit SHA binding:** the six release-boundary tests and seven publish
  workflow tests passed, including pre/post SHA drift, finalizer ordering, and
  explicit SHA source refspecs.

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
cargo test --test publish_workflow
CLIPPY_CONF_DIR="$PWD" cargo clippy --all-targets --all-features -- -D warnings

cargo test archive_authority_rejects_a_terminal_label_with_a_contradictory_v0_envelope
cargo test archive_authority_requires_one_terminal_open_source_and_an_absent_destination
```

Results:

- structural/compiling boundary suite: 34/34 passed;
- release-boundary suite: 6/6 passed;
- publish-workflow suite: 7/7 passed;
- archive authority tests: 1/1 and 1/1 passed;
- reviewed-tree source checker and all-target/all-feature Clippy: passed;
- independent nested and path-included mutations: rejected by both source and
  compiler gates as shown above.

The parent had already run the complete core suite; this re-review used the
requested focused evidence rather than repeating the long full run.

## Exit decision

All filesystem/authority-boundary P0/P1/P2 findings are closed on the exact
tuple. This independent review returns **GO**. R2 may proceed only after the
other required settled-tree review also returns GO, as required by the RemPlan.
