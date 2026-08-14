# R4b R2 consumer-checkpoint remediation — eleventh code review

Date: 2026-08-14

## Verdict

**GO for the R2-I12 compiler-root correction.**

P0: 0. P1: 0. P2: 0. P3: 1.

The committed correction closes the tenth-review alternate-root bypass. Cargo
semantically selects `src/lib.rs`; exact byte manifests then bind
`src/lib.rs` to `src/workspace_ops/mod.rs`, that parent to
`src/workspace_ops/merge/mod.rs`, and that parent to the protected
`v1_lifecycle/mod.rs` tree. The pinned merge parent references a symbol that is
defined in the intended v1 root, and exact all-target/all-feature Clippy plus
the v1 tests compile that route. A resident but unselected protected v1 tree is
therefore no longer sufficient to pass the current gate.

The boundary checker, all 52 boundary regressions, release-boundary tests,
publish-workflow tests, focused service tests, exact Clippy, and formatting all
pass. Ordinary package metadata and version changes remain allowed because the
checker semantically constrains only the `[lib]` table. PR/push CI, release CI,
and the local exact-SHA release finalizer still run the source and compiler
checks. The sole finding is a P3 test-maintenance issue: the positive version
change regression will become a no-op after the next version bump. It does not
affect the current implementation or gate decision.

This is one independent GO re-review. The plan's separate two-GO requirement
still determines when R2 production conversion may begin.

## Exact tuple reviewed

- workspace root: `b5773f3a4f5fee7c456df1975ed8a2e11c66dc5c`
- `gwz-core`: `bbbfd2833fdbb2191fabbc4f4aa9d8eaf85ad8b2`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Direct `git rev-parse HEAD` checks agreed with this tuple before inspection.
The same three IDs were rechecked after this report was written. The checkout
was clean before review, and this report is my only workspace write.

The core correction is the single commit from
`b93e26aa3c5fc12ced8a057612ffbb64c2b5423e` to the reviewed core SHA. Its only
changes are the boundary checker, the boundary regression suite, the merge
parent sentinel reference, and the v1 root sentinel definition. The latest
workspace-root commit updates the settled plan/review evidence and pins that
core SHA in `gwz.lock.yml`; it does not change the root Cargo workspace or
release machinery. Both latest-commit diffs pass `git diff --check`.

## R2-I12 review

### Cargo selects the protected crate root

`gwz-core/Cargo.toml` contains exactly:

```toml
[lib]
path = "src/lib.rs"
```

`check_checked_artifact_boundaries.py:552-565` parses Cargo TOML with
`tomllib` and requires that semantic `[lib]` value. It rejects a missing,
malformed, symlinked, redirected, or augmented library target table. Independent
`cargo +1.95.0 metadata --no-deps --format-version 1` output identifies the
core library `src_path` as the reviewed `gwz-core/src/lib.rs`.

The comparison is deliberately scoped to `manifest["lib"]`. Package version,
description, repository, and other unrelated package metadata are not byte
pinned. The committed version-change regression currently changes `0.10.3` to
`99.0.0` and passes the checker. Changes within `[lib]` are not treated as
ordinary package metadata because they can change the compiler target.

### Every parent edge through `merge/mod.rs` is fixed

`PROTECTED_COMPILER_ROOT_DIGESTS` pins these exact regular files:

- `src/lib.rs`:
  `e035f8a53ddb589362972c85593cc0dff4b590129de38fe0fdb72ca1880f544e`;
- `src/workspace_ops/mod.rs`:
  `663b228d1f3fddc74853d3e26f9623a0d7d2009f172f53640697de35042a8124`;
- `src/workspace_ops/merge/mod.rs`:
  `d7b2662b8c18fa7d5b83e951a65be73498edcdc29565ca65e3f6d5cd069c1356`.

Independent SHA-256 recomputation matched all three values.
`src/lib.rs:30` declares `pub mod workspace_ops;`,
`src/workspace_ops/mod.rs:15` declares `mod merge;`, and
`src/workspace_ops/merge/mod.rs:23-26` declares the test-gated
`mod v1_lifecycle;` and references its compiler-root sentinel. An alternate
`#[path]`, `cfg_attr(path = ...)`, macro, sibling root, or higher-level module
redirect now changes one of these exact manifests before it can displace the
protected tree.

The new regression matrix covers:

- Cargo `[lib]` redirection;
- `src/lib.rs` redirection of `workspace_ops`;
- `workspace_ops/mod.rs` redirection of `merge`;
- the tenth-review alternate v1 root, including a blanket unsealed runtime;
- a positive parent/root sentinel; and
- a non-target version change.

The executable alternate-root fixture first proves that its replacement root
can compile, then the structural checker rejects the changed compiler-root
manifest. All 52 boundary tests pass.

### The intended v1 tree is selected

The exact merge parent contains:

```rust
#[cfg(test)]
mod v1_lifecycle;
#[cfg(test)]
const _: &str = v1_lifecycle::COMPILER_ROOT_SENTINEL;
```

The protected `v1_lifecycle/mod.rs` defines:

```rust
pub(super) const COMPILER_ROOT_SENTINEL: &str = module_path!();
```

Because the parent bytes are fixed, this reference cannot be moved to an
alternate module without an explicit manifest update. Because the root and all
descendants are covered by the protected tree digest, the symbol cannot be
silently supplied by a changed intended root. The current tree digest
independently recomputes to
`56c96f601db9f709ace68b5a1959bf2fc657481741c131d0da4c02d8d3fb8d27`.
The nested authority-observer tree also independently matches its existing
`ff6574fc1bde70c81dc72bd58373eaa50ef7d1b26fc6468412f9e041a1e90788`
digest.

Exact all-target/all-feature Clippy compiles the test-gated v1 route with the
sentinel reference. The focused service tests pass 5/5 and the four service
sequence tests pass 4/4, further confirming that Cargo/rustc selected the
intended v1 module and its test graph.

### CI and release wiring remains mandatory

The core correction does not modify either workflow, `scripts/release.py`, or
their static tests. Inspection and execution confirm:

- `.github/workflows/checked-artifact-boundary.yml` runs on pull requests and
  pushes to `main`, installs Python 3.11 and Rust 1.95.0, then runs the
  structural checker, its unit suite, the release-boundary suite, and exact
  all-target/all-feature Clippy;
- `.github/workflows/release.yml` runs the checker and both Python suites on
  Linux, full `cargo test --locked` on its release matrix, and exact Clippy on
  Linux; and
- `scripts/release.py` runs the structural checker, 52-test suite,
  release-boundary suite, and exact Clippy inside the non-skippable exact-SHA
  gate, verifies the tree before and after, and only then tags or pushes that
  SHA.

The release-boundary suite passes 6/6, including pre/post SHA drift,
unconditional new-tag gating, explicit-SHA atomic tag/push, and the absence of
a compiler skip. The publish-workflow suite passes 7/7.

## Finding

### [P3-1] The non-target version regression becomes vacuous after the next version bump

**Where:**
`gwz-core/scripts/checks/test_check_checked_artifact_boundaries.py:408-419`.

`test_compiler_root_manifest_allows_non_target_metadata_change` replaces the
literal current line `version = "0.10.3"` with `version = "99.0.0"`, but it
does not assert that the replacement changed the copied manifest. Once a
release changes the real package version, this replacement will do nothing and
the test will continue to pass against an unchanged manifest. It could then
fail to detect a future regression that accidentally byte-pins `Cargo.toml` or
rejects ordinary package version changes.

This is not a current boundary bypass. At the reviewed SHA the replacement
does occur and passes, and the checker implementation reads the TOML and
compares only the `[lib]` table. The production compiler-root chain, current
release bump, and current GO decision are unaffected.

**Recommended correction:** parse the copied manifest's current version and
replace it with a guaranteed-distinct valid version, or add a harmless package
metadata key. Assert that the before and after bytes differ before invoking the
checker. Keeping a separate redirect negative for `[lib].path` preserves the
target/non-target distinction.

## Commands and results

All commands were run read-only against the reviewed checkout except for test
fixtures created under temporary directories and this report.

- `git status --short`; `git rev-parse HEAD`; `git -C gwz-core rev-parse HEAD`;
  `git -C gwz-cli rev-parse HEAD`: initially clean and exactly matched the
  requested tuple.
- `git -C gwz-core diff --check HEAD^..HEAD`; root
  `git diff --check HEAD^..HEAD`: passed.
- `python3 scripts/checks/check_checked_artifact_boundaries.py`: passed,
  reporting `ok (15 visible entries, 5 classified modules)`.
- `python3 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py -v`:
  passed 52/52 in 330.902 seconds.
- `python3 -m unittest scripts/checks/test_release_boundary.py -v`: passed 6/6.
- `cargo +1.95.0 test --test publish_workflow`: passed 7/7.
- `cargo +1.95.0 metadata --no-deps --format-version 1`: identified the core
  library target as `gwz-core/src/lib.rs`.
- `sha256sum` over the three compiler-root source files: matched every
  committed compiler-root digest.
- independent calls to `source_tree_digest` for the v1 and authority observer
  roots: matched both committed tree digests.
- `cargo +1.95.0 test --lib workspace_ops::merge::v1_lifecycle::service::tests`:
  passed 5/5.
- four exact service-sequence test-name filters: passed 4/4. An earlier broad
  `service_sequence` filter matched zero tests; it was corrected to the exact
  names and is not counted as evidence.
- `CLIPPY_CONF_DIR="$PWD" cargo +1.95.0 clippy --all-targets --all-features -- -D warnings`:
  passed.
- `cargo +1.95.0 fmt --all -- --check`: passed.
- `TAUT_PYTHON="$(command -v python3)" cargo +1.95.0 test --locked`: launched
  all 1,227 library tests. Every emitted result was passing, including the
  service, archive, finalization, preservation, rollback, and workspace
  integration cases. The remaining duplicate exhaustive run was stopped with
  exit 130 at the coordinating reviewer's direction because another
  independent reviewer was already running it; this review does not claim a
  completed full-suite pass.

## Gate decision

R2-I12 satisfies the compiler-loaded source-closure correction requested by
the tenth reviews. There are no P0, P1, or P2 findings. The P3 regression
maintenance issue should be fixed opportunistically but does not reopen the
architectural gate. This review is **GO**.
