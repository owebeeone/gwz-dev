# R4b R2 consumer-checkpoint remediation — eleventh independent state/type re-review

Date: 2026-08-14

## Verdict

**GO for the corrected R2 remediation checkpoint.**

P0: 0. P1: 0. P2: 0. P3: 0.

No correctness or regression finding was identified in the two-commit
correction. The production v1 service admits exactly `ForwardRuntime`,
`ReverseRuntime`, and `FinalizationRuntime`, with each admission retaining the
sealed `MergeAuthorityBackend` requirement. Arbitrary observer/executor test
doubles enter only through the separately named `#[cfg(test)]` runner, and
both entries use the same unchanged private state-machine body.

The compiler route is now anchored from `Cargo.toml` through `src/lib.rs`,
`src/workspace_ops/mod.rs`, and `src/workspace_ops/merge/mod.rs` into the
complete protected v1 tree. The intended root is also positively referenced
by a compiled sentinel. The tenth-review alternate-root case and redirects at
each higher edge reject. Preservation observation, terminal archive behavior,
and release/static gate wiring remain intact. Full, focused, adversarial,
release, document, format, and Clippy checks all pass.

Under the review rule that any P0/P1/P2 requires NO-GO, the zero-finding
result is **GO**.

## Exact settled tuple reviewed

- workspace root: `b5773f3a4f5fee7c456df1975ed8a2e11c66dc5c`
- `gwz-core`: `bbbfd2833fdbb2191fabbc4f4aa9d8eaf85ad8b2`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Initial direct `git rev-parse HEAD` checks and `gwz status --json` agreed with
this tuple. `gwz status --json` reported every tracked member clean and
matching its lock entry. The final direct commit verification, performed after
creating this report, again matched all three SHAs; the report was the only
workspace file written by this review.

The relevant workspace/core commits are:

- `1f1bdf0b055da3306c8bd57714e96151f07d7bbb` /
  `b93e26aa3c5fc12ced8a057612ffbb64c2b5423e`: seal the production v1
  runtime and split the generic test runner; and
- `b5773f3a4f5fee7c456df1975ed8a2e11c66dc5c` /
  `bbbfd2833fdbb2191fabbc4f4aa9d8eaf85ad8b2`: anchor the compiler root
  and add the positive v1 sentinel.

`gwz-cli` did not move during these two workspace corrections.

## Type and state review

### Production admission is exactly closed

`gwz-core/src/workspace_ops/merge/v1_lifecycle/service.rs:38-52` defines the
private `sealed::ProductionRuntime` marker and implements it only for:

- `ForwardRuntime<'_, B>` where `B: MergeAuthorityBackend`;
- `ReverseRuntime<'_, B>` where `B: MergeAuthorityBackend`; and
- `FinalizationRuntime<'_, B>` where `B: MergeAuthorityBackend`.

`V1Runtime` requires that private marker in addition to `ExactObserver` and
`PhysicalExecutor`. Because the marker module is private to `service`, a
sibling module cannot add a fourth implementation. The three concrete runtime
modules independently implement both service traits only for the same sealed
backend bound.

The production entry at `service.rs:69-77` accepts only `R: V1Runtime`. The
generic entry at `:79-88` accepts test doubles but is explicitly
`#[cfg(test)]`. Both are one-line delegates to `run_with_runtime` at `:90`, so
the split did not fork transition, observation, lease, execution, recovery, or
response semantics. The production terminal archive caller remains on
`service::run` with a `ReverseRuntime` at
`v1_lifecycle/archive.rs:68-85`; mechanical test call sites moved to
`run_test`.

The compiler-negative regression at
`scripts/checks/test_check_checked_artifact_boundaries.py:884-918` adds an
arbitrary fourth observer/executor runtime and proves it cannot satisfy
`V1Runtime`. It passed. Focused tests also exercised the shared body through
forward, finalization, reverse preservation, service, sequence, and archive
paths, and the complete suite passed.

### Cargo-to-v1 compiler path is anchored and positively linked

The structural checker semantically requires exactly
`[lib] path = "src/lib.rs"` and rejects a symlinked `Cargo.toml`. It then
checks exact SHA-256 manifests for all three compiler parents:

- `src/lib.rs`:
  `e035f8a53ddb589362972c85593cc0dff4b590129de38fe0fdb72ca1880f544e`;
- `src/workspace_ops/mod.rs`:
  `663b228d1f3fddc74853d3e26f9623a0d7d2009f172f53640697de35042a8124`;
- `src/workspace_ops/merge/mod.rs`:
  `d7b2662b8c18fa7d5b83e951a65be73498edcdc29565ca65e3f6d5cd069c1356`.

Independent recomputation matched all three. The pinned chain selects
`workspace_ops` at `src/lib.rs:30`, `merge` at
`src/workspace_ops/mod.rs:15`, and `v1_lifecycle` at
`src/workspace_ops/merge/mod.rs:23-24`. The pinned parent then requires
`v1_lifecycle::COMPILER_ROOT_SENTINEL` at `merge/mod.rs:25-26`, while the
intended root defines it with `module_path!()` at
`v1_lifecycle/mod.rs:5`. Thus the gate proves both the parent edges and that
the compiler-selected child exports the intended symbol.

The complete v1 tree digest independently recomputed as
`56c96f601db9f709ace68b5a1959bf2fc657481741c131d0da4c02d8d3fb8d27`,
matching `PROTECTED_SOURCE_TREE_DIGESTS`. `git ls-tree` showed ordinary
`100644` blobs throughout the current v1 tree. The adversarial suite passed
all of the following:

- Cargo lib-target redirection rejects;
- `lib.rs` workspace-module redirection rejects;
- `workspace_ops/mod.rs` merge-module redirection rejects;
- the always-true `cfg_attr(path)` alternate v1 root rejects even though that
  replacement compiles on its own; and
- the positive parent/root sentinel is present and compiled by the exact
  all-target gate.

The checker still allows unrelated Cargo metadata such as a version change,
so the anchor is scoped to compiler identity rather than freezing release
metadata.

### Preservation observation and archive semantics are unchanged

The runtime-seal commit added only function-local
`#[forbid(clippy::disallowed_methods)]` attributes to the v1 helpers in
`preserve/artifacts.rs`; it did not alter their bodies. The file independently
matched its recorded digest
`489006a9a7550503fd051b2d6136f8a08728ab9355e6281ede684047732f6da5`.
The root-spec, clean-form, managed-fact, blob-id, preservation-image,
attachment, and error helpers all retain their local compiler writer bans and
their `MergeAuthorityBackend` bounds. The compiler probe which injects a raw
write into `v1_preservation_image` rejected as intended.

The static checker still proves that the concrete preservation observer ends
in the protected leaf, remains absent from the open `GitBackend` trait, and
has exactly the approved caller set. The former v1 backend-name spelling scan
was removed only after the exact complete-tree manifest and sealed production
runtime made it redundant: adding an open backend use now changes the
protected tree digest, and an alternative production runtime/backend also
fails at compile time. The corresponding regression passed.

Archive production code was not changed by either core commit. The only
production service call continues to use `ReverseRuntime`; exact destination
validation, source/destination reconciliation, rejection without deletion,
and symlink containment are unchanged. The three focused archive tests and
the full archive/GC/status matrices passed.

### Release and static gates retain their effective boundary

No file under `.github`, `scripts/release.py`,
`scripts/checks/test_release_boundary.py`, or `tests/publish_workflow.rs`
changed between `da57559` and `bbbfd283`. The boundary checker itself gained
the compiler-root manifests and regressions required by this correction; its
release invocation did not move or become optional.

The six release-boundary tests passed, including exact-SHA reacquisition,
before/after drift rejection, mandatory finalization before tag/push, atomic
SHA-sourced branch/tag push, and absence of a compiler-skip option. The seven
publish-workflow tests passed, confirming the checked-artifact gate remains on
PR/main and release paths and precedes local release publication. Exact
all-target/all-feature Clippy passed with warnings denied.

## Test and inspection evidence

The following commands were run against the settled tuple. Cargo build output
was directed to `/tmp/gwz-core-review-state-11-target`; adversarial tests made
their mutations only in temporary copies.

```text
gwz status --json

PYTHONDONTWRITEBYTECODE=1 \
  python3 scripts/checks/check_checked_artifact_boundaries.py
PYTHONDONTWRITEBYTECODE=1 \
  python3 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py -v
PYTHONDONTWRITEBYTECODE=1 \
  python3 -m unittest scripts/checks/test_release_boundary.py -v

CARGO_TARGET_DIR=/tmp/gwz-core-review-state-11-target \
  cargo +1.95.0 test --lib \
  'workspace_ops::merge::v1_lifecycle::service::tests::'
CARGO_TARGET_DIR=/tmp/gwz-core-review-state-11-target \
  cargo +1.95.0 test --lib \
  'workspace_ops::merge::v1_lifecycle::service::sequence_tests::'
CARGO_TARGET_DIR=/tmp/gwz-core-review-state-11-target \
  cargo +1.95.0 test \
  'workspace_ops::merge::v1_lifecycle::archive::archive_tests::'
CARGO_TARGET_DIR=/tmp/gwz-core-review-state-11-target \
  cargo +1.95.0 test --lib \
  'workspace_ops::merge::v1_lifecycle::tests::forward::'
CARGO_TARGET_DIR=/tmp/gwz-core-review-state-11-target \
  cargo +1.95.0 test --lib \
  'workspace_ops::merge::v1_lifecycle::finalization::'
CARGO_TARGET_DIR=/tmp/gwz-core-review-state-11-target \
  cargo +1.95.0 test --lib \
  'workspace_ops::merge::v1_lifecycle::reverse::preservation::tests::real_git::'
CARGO_TARGET_DIR=/tmp/gwz-core-review-state-11-target \
  cargo +1.95.0 test

CARGO_TARGET_DIR=/tmp/gwz-core-review-state-11-target \
CLIPPY_CONF_DIR="$PWD" \
  cargo +1.95.0 clippy --all-targets --all-features -- -D warnings
cargo +1.95.0 fmt --all -- --check

PYTHONDONTWRITEBYTECODE=1 python3 scripts/checks/check_merge_docs.py
PYTHONDONTWRITEBYTECODE=1 \
  python3 -m unittest scripts/checks/test_check_merge_docs.py -v
```

Results:

- checked-artifact structural checker: passed;
- checked-artifact/adversarial suite: 52/52 passed;
- release-boundary suite: 6/6 passed;
- focused service and sequence: 5/5 and 4/4 passed;
- focused archive: 3/3 passed;
- focused forward, finalization, and real-Git preservation: 16/16, 19/19,
  and 3/3 passed;
- full library suite: 1,226 passed, 0 failed, 1 pre-existing ignored;
- integration suites: diff render 10/10, protocol 29/29, publish workflow
  7/7, rename 2/2, and doc tests passed;
- exact all-target/all-feature Clippy with `-D warnings`: passed;
- Rust formatting check: passed;
- merge-document consistency: 10 sources / 133 assertions, plus 3/3
  checker tests passed; and
- compiler-parent, complete-v1-tree, and preservation-adapter digest
  recomputation: exact matches.

## Gate decision

The two commits close the unsealed runtime and unloaded-protected-tree risks
without changing the v1 state machine, preservation observation, archive
semantics, or release/static execution boundary. There are no P0, P1, P2, or
P3 findings. This independent settled-tree review is **GO**.
