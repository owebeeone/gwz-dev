# R4b R2 consumer-checkpoint remediation — tenth code review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

P0: 0. P1: 0. P2: 1. P3: 0.

The committed service implementation itself admits only the intended
`ForwardRuntime`, `ReverseRuntime`, and `FinalizationRuntime` production
adapters, all three remain constrained by `MergeAuthorityBackend`, and the
generic runner is available only as the `#[cfg(test)]` `run_test` entry. The
mechanical test call-site changes compile and the focused service behavior is
unchanged. The committed v1 tree digest also matches every current descendant
file and byte, and the mixed `preserve/artifacts.rs` file matches its exact
source digest and retains local compiler writer bans on all v1 helper
functions.

The completeness gate is nevertheless bypassable. The parent module
declaration which selects the v1 root is outside the byte manifest. An
always-true `cfg_attr(path = ...)` can redirect that declaration to a sibling
root while the original, fully hashed `v1_lifecycle/mod.rs` tree remains
untouched. The direct path scanner still does not recognize `cfg_attr`, so both
the structural checker and exact all-target/all-feature Clippy accepted an
alternate root containing a blanket, unsealed `V1Runtime` and a fourth runtime.
This violates the correction's central production-runtime and compiler-loaded
source-closure guarantees.

## Exact tuple reviewed

- workspace root: `1f1bdf0b055da3306c8bd57714e96151f07d7bbb`
- `gwz-core`: `b93e26aa3c5fc12ced8a057612ffbb64c2b5423e`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Direct `git rev-parse HEAD` checks agreed with this tuple before and after the
review. The adversarial probe used an exact-core-SHA export below `/tmp`; no
reviewed production or test source was modified. This report is the reviewer's
only workspace write.

## Review of the committed correction

### Runtime admission and test-only access

`src/workspace_ops/merge/v1_lifecycle/service.rs:38-52` declares a private
`sealed::ProductionRuntime` and implements it only for:

- `ForwardRuntime<'_, B>` where `B: MergeAuthorityBackend`;
- `ReverseRuntime<'_, B>` where `B: MergeAuthorityBackend`; and
- `FinalizationRuntime<'_, B>` where `B: MergeAuthorityBackend`.

The production `run` entry at `:69-77` requires `V1Runtime`. The generic
`ExactObserver + PhysicalExecutor` entry at `:79-88` is explicitly
`#[cfg(test)]`, and both entries delegate to the same private implementation,
so the split introduces no behavioral fork. Crate-wide call-site inspection
found only the production `archive.rs` call using `service::run`; arbitrary
test doubles use `run_test`, and every changed call site is loaded from a
test-only module.

### Source and compiler manifests

`PROTECTED_SOURCE_TREE_DIGESTS` now includes
`workspace_ops/merge/v1_lifecycle/mod.rs`. `source_tree_digest` treats a
`mod.rs` root's parent directory as the descendant root and hashes the sorted
relative path and exact bytes of every regular descendant file, regardless of
suffix. Independent recomputation matched the committed
`21ab2154e58ebe091a62bfb5bdf36afdbe98c30b9181d0ba08e29415aa6af00d`
digest. Thus every current file below `v1_lifecycle/`, including its tests and
the mechanical call-site edits, is covered.

`workspace_ops/merge/preserve/artifacts.rs` independently matched its committed
`489006a9a7550503fd051b2d6136f8a08728ab9355e6281ede684047732f6da5`
digest. Its v1 root-spec, clean-form, managed-fact, blob-id, preservation-image,
attachment, and error helpers each retain a function-local
`#[forbid(clippy::disallowed_methods)]`. The dedicated compiler regression for
a raw writer in `v1_preservation_image` passes.

Those local properties are sound, but they do not anchor the compiler's parent
edge into the hashed v1 root, which causes the finding below.

## Finding

### [P2-1] The unhashed parent module edge can redirect compilation away from the protected v1 tree

**Where:** `gwz-core/src/workspace_ops/merge/mod.rs:23-24`;
`gwz-core/scripts/checks/check_checked_artifact_boundaries.py:118-121`,
`:409-415`, `:507-523`, and `:549-596`; RemPlan sections 1 and 7,
implementation item 11, and exit criterion 8.

**Violated contract.** The tenth correction says the exact v1 lifecycle tree
is the completeness proof, a loader alias outside that manifest must fail, and
only three sealed runtime adapters may enter the production service. The tree
digest begins at `v1_lifecycle/mod.rs`, but the compiler selects that root from
the separate declaration in `merge/mod.rs`, which is neither individually
byte-pinned nor included in the tree digest. The spelling scanner recognizes
only a direct `#[path = "..."]`, not a path produced by `cfg_attr`.

**Executable counterexample.** In an exact
`b93e26aa3c5fc12ced8a057612ffbb64c2b5423e` export, I changed the existing
parent declaration to:

```rust
#[cfg(test)]
#[cfg_attr(all(), path = "v1_lifecycle.rs")]
mod v1_lifecycle;
```

`cfg(all())` is always true. I added the sibling `v1_lifecycle.rs` with a
minimal service that declared `ExactObserver`, `PhysicalExecutor`, restored the
blanket `impl<T: ExactObserver + PhysicalExecutor> V1Runtime for T`, and proved
at compile time that a new `UnreviewedRuntime` satisfied `V1Runtime`. The
original `v1_lifecycle/mod.rs` and every descendant remained byte-identical, so
the protected digest still matched even though none of that tree was the
selected v1 module.

Redirecting away from the original test tree makes other existing
`#[cfg(test)]` support imports unused. To exercise the same exact compiler gate,
the probe also added a test-only crate-level `allow(dead_code,
unused_imports)` outside the protected manifests. The checker accepted that
change as well. Both mandatory commands exited zero:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
Finished `dev` profile ...
```

The commands were:

```text
python3 scripts/checks/check_checked_artifact_boundaries.py
CLIPPY_CONF_DIR="$PWD" cargo +1.95.0 clippy --all-targets --all-features -- -D warnings
```

**Impact.** A checked-in change can substitute an arbitrary v1 root, omit the
protected v1 tests, restore the blanket runtime admission, and define a fourth
runtime while the source and compiler gates report success. The current three
marker implementations are correct, but the release boundary does not prove
that they are the implementations the compiler selected. The same root
redirection remains applicable when the existing test gate is later removed
for production activation because the `cfg(all())` condition is unconditional.

**Required correction.** Anchor the positive manifest at the compiler edge.
At minimum, byte-pin `workspace_ops/merge/mod.rs` so the exact declaration of
`mod v1_lifecycle` cannot change independently of review. Prefer one manifest
which binds the parent source path and exact module declaration, the canonical
regular-file identity of `v1_lifecycle/mod.rs`, and every descendant path and
byte. Add a compiling negative regression for the always-true `cfg_attr` root
redirection above, leaving the original protected directory untouched; the
structural gate must reject it before Clippy. Preserve the current service
seal, complete descendant digest, artifact-file digest, and local compiler
bans.

## Test and inspection evidence

Reviewed-tree results:

- checked-artifact structural checker: passed;
- checked-artifact boundary unit suite: 46/46 passed, including the new loader,
  unsealed-runtime, and v1 artifact-writer regressions;
- focused service tests: 5/5 passed;
- focused service-sequence tests: 4/4 passed;
- release-boundary Python tests: 6/6 passed;
- publish-workflow tests: 7/7 passed;
- exact all-target/all-feature Clippy with `-D warnings`: passed;
- `cargo +1.95.0 fmt --all -- --check`: passed;
- core, root, and CLI commit diff checks: passed; and
- current tree and individual artifact digest recomputation: passed.

A full `cargo +1.95.0 test` run launched all 1,227 library tests. All emitted
results were passing, including the long service fault, successor, and
ambiguity matrices. After the independent P2 counterexample was reproduced,
the remaining long-running duplicate test was stopped at the coordinating
reviewer's direction, so this review does not claim a final full-suite pass.

Adversarial exact-export result:

- always-true parent `cfg_attr(path)` plus alternate unsealed v1 root:
  structural checker incorrectly passed;
- the same alternate root with a fourth compile-proven runtime: exact
  all-target/all-feature Clippy incorrectly passed.

## Gate decision

Do not begin R2 production conversion on this tuple. The committed runtime
seal and byte manifests should be retained, but the v1 manifest must be bound
to the parent module-selection edge before it can establish compiler-loaded
source closure. Because this review has one P2 finding, the verdict is
**NO-GO**.
