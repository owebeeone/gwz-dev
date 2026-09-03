# R4b R2 consumer-checkpoint remediation — tenth independent state/release/consumer-boundary re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

P0: 0. P1: 0. P2: 1. P3: 0.

The committed R2-I11 implementation correctly seals the production service
inside the currently compiled v1 module: only `ForwardRuntime`,
`ReverseRuntime`, and `FinalizationRuntime` receive the private production
marker, all three retain `MergeAuthorityBackend`, and generic doubles use the
separate `#[cfg(test)]` runner. The mixed preservation-artifact adapter is
byte-pinned and its v1 observation functions retain compiler-resolved writer
bans. Focused archive, checked-artifact, release, document, format, and static
checks pass.

One P2 remains in the completeness proof. The v1 byte manifest begins at
`v1_lifecycle/mod.rs`, but the compiler-loading declaration for that root is
the unprotected `mod v1_lifecycle;` in the parent `merge/mod.rs`. The path-edge
scanner still does not recognize `cfg_attr(..., path = ...)`. In an exact-SHA
export, a `cfg_attr` on that parent declaration redirected the compiler to a
sibling module containing an unsealed replacement service and runtime. The
original pinned tree and every recorded digest remained unchanged. The source
checker and exact all-target/all-feature Clippy both passed, the alternative
runtime test ran, and the original v1 lifecycle test graph was no longer
compiled. Thus the manifest proves bytes for a resident directory, not that
the compiler uses that directory as the v1 service.

## Exact tuple reviewed

- workspace root: `1f1bdf0b055da3306c8bd57714e96151f07d7bbb`
- `gwz-core`: `b93e26aa3c5fc12ced8a057612ffbb64c2b5423e`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Initial direct `git rev-parse HEAD` checks and `gwz status --json` agreed on
this exact clean tuple. The executable counterexample was built only in an
archive of the exact core SHA under `/tmp`; it did not modify the reviewed
checkout. Immediately before this report was written, `gwz status --json`
again reported the exact tuple clean with both tracked members matching their
lock entries. This report is my only workspace edit.

## R2-I11 state/type audit

### Production service seal and test-only route

`v1_lifecycle/service.rs:38-52` declares a private
`sealed::ProductionRuntime`, implements it only for the three intended runtime
families, and requires `MergeAuthorityBackend` in every implementation.
`V1Runtime` combines that private marker with `ExactObserver` and
`PhysicalExecutor`. The production `run` at `:69-77` requires `V1Runtime`; the
generic `run_test` at `:79-88` is separately `#[cfg(test)]`; both share only the
private state-machine body.

The three runtime implementations themselves retain the sealed backend bound:

- `forward.rs` implements both service traits only for
  `B: MergeAuthorityBackend`;
- `reverse.rs` does the same for reverse observation and execution; and
- `finalization.rs` does the same for publication observation and execution.

No current alias, wrapper, or fourth implementation of the private production
marker exists. The only current direct production-run call is
`v1_lifecycle/archive.rs:79`, which constructs `ReverseRuntime`. The test-only
runner has 48 references across 23 v1 source modules, covering service
sequencing, finalization, forward operation, preservation, rollback,
ambiguity, durability, and fault matrices. The full current unit-test binary
contains 1,227 tests and the focused service/lifecycle tests encountered in
the full run were green before that long run was stopped.

The boundary suite's direct unsealed-runtime compiler probe also rejects as
expected. That probe adds the runtime inside the already manifested v1 tree;
it does not cover replacement of the tree's parent loader, which is the P2
below.

### Preservation observation remains read-only on the reviewed route

The exact SHA-256 of
`workspace_ops/merge/preserve/artifacts.rs` recomputes to
`489006a9a7550503fd051b2d6136f8a08728ab9355e6281ede684047732f6da5`,
matching `PROTECTED_SOURCE_DIGESTS`. Its v1 observation/specification helper
functions have function-local `#[forbid(clippy::disallowed_methods)]`
attributes, and the committed compiler probe which inserts
`std::fs::write` into `v1_preservation_image` rejects. The adapter still
requires `MergeAuthorityBackend` and therefore reaches only `Git2Backend`.

The concrete stash observer remains outside the open `GitBackend` contract,
terminates in the protected `preservation_image` leaf, and only validates the
merge ID and decodes native stash objects. Its reference inventory remains the
five approved files. `prepare_root_preservation_stash` validates the exact
handoff and constructs an in-memory normalized image through the protected
capture leaf; mutation is reserved for the separately named checked execution
step. I found no current write in the reviewed observation route.

### Archive semantics and checked state contracts

The R2-I11 source changes do not alter canonical archive acquisition,
version-specific archived-record validation, source/destination identity,
managed-purpose policy, owner/action encoding, reservation derivation, or
general capability visibility.

All 96 `checked_artifact::interface_tests` passed. That matrix includes the
contradictory terminal-labelled v0 rejection, the exact terminal-open plus
absent-destination success, v1 archive support, closed owner/action encoding,
managed prefix and overlap policy, reservation binding, collision snapshot
revalidation, and private capability roles. The two archive authority tests
were also run separately and passed 1/1 each. During the later full run, the
v1 archive restart, mismatch, symlink, and GC tests completed successfully
before the run was stopped.

### Release and check wiring

The correction does not change `scripts/release.py` or either release-boundary
workflow. Re-execution showed:

- the structural boundary suite passed 46/46;
- the exact-release suite passed 6/6, including pre/post gate SHA drift,
  unconditional new-tag finalization, explicit-SHA tag/push, and absence of a
  compiler skip;
- the static publish-workflow suite passed 7/7;
- exact `CLIPPY_CONF_DIR="$PWD" cargo +1.95.0 clippy --all-targets
  --all-features -- -D warnings` passed;
- `cargo +1.95.0 fmt --all -- --check` passed; and
- the correction diff passed `git diff --check`.

These checks prove that the current release/check paths still converge and are
non-skippable. They do not close the P2 because the same mandatory source and
compiler gates accepted its exact-SHA counterexample.

## Finding

### [P2-1] The v1 manifest does not bind the parent module loader, so an unsealed replacement service can displace the pinned tree

**Where:** `gwz-core/src/workspace_ops/merge/mod.rs:23-24`;
`gwz-core/scripts/checks/check_checked_artifact_boundaries.py:115-121,
507-523,541-596`; and
`gwz-core/scripts/checks/test_check_checked_artifact_boundaries.py:347-382,
760-794`; RemPlan sections 1 and 7, implementation item 11, and exit criteria
8 and 11.

**Violated contract.** R2-I11 says the complete v1 lifecycle tree is
byte-pinned so a loader alias or a new runtime requires an explicit reviewed
manifest update, and that generic tests retain their lifecycle coverage only
through the test runner. The digest rooted at
`workspace_ops/merge/v1_lifecycle/mod.rs` includes that file and everything
below its directory, but it excludes the parent declaration which decides
whether Rust loads that root at all:

```rust
#[cfg(test)]
mod v1_lifecycle;
```

The scanner at `check_checked_artifact_boundaries.py:563-583` still recognizes
only a literal direct `#[path = "..."]`. It does not interpret
`#[cfg_attr(test, path = "...")]`. It also inventories files resident below
`src`, not the effective compiler module graph.

The committed `cfg_attr`, include-alias, and outside-source regressions modify
the already byte-pinned `preserve/artifacts.rs`, so they fail because that
adapter's bytes change even though the loader spelling remains unrecognized.
The committed unsealed-runtime probe modifies `v1_lifecycle/status.rs`, so it
is rejected inside the existing tree. No regression changes the unprotected
parent root edge while leaving the pinned tree untouched.

**Executable counterexample.** I exported exact core SHA
`b93e26aa3c5fc12ced8a057612ffbb64c2b5423e` under
`/tmp/gwz-state10-root-loader.PYTsqH`. I left every file below
`src/workspace_ops/merge/v1_lifecycle/` and the mixed artifact adapter
unchanged, then:

1. added `#[cfg_attr(test, path = "unreviewed_v1.rs")]` to the parent
   declaration in `merge/mod.rs`;
2. added a sibling `unreviewed_v1.rs` with open `ExactObserver` and
   `PhysicalExecutor` traits, a blanket unsealed `V1Runtime`, `run`, and an
   `UnreviewedRuntime` test; and
3. allowed test-only unused/dead-code warnings in unprotected crate/parent
   source, because displacing the original v1 module otherwise leaves sibling
   v1 helpers unused. The protected `forbid(clippy::disallowed_methods)`
   boundaries remained intact.

The mandatory gates accepted the replacement:

```text
checked-artifact boundary: ok (15 visible entries, 5 classified modules)
CLIPPY_CONF_DIR="$PWD" cargo +1.95.0 clippy --all-targets --all-features -- -D warnings
Finished `dev` profile ...
```

The alternative runtime then compiled and entered its replacement service:

```text
cargo +1.95.0 test unreviewed_runtime_enters_replacement_service
running 1 test
test workspace_ops::merge::v1_lifecycle::unreviewed_runtime_enters_replacement_service ... ok
test result: ok. 1 passed; ... 1003 filtered out
```

The reviewed current test binary contains 1,227 unit tests; the replacement
binary contained only 1,004. Thus 223 net unit tests disappeared while the
mandatory source checker and Clippy remained green. The original v1 manifest
digest still matched because its complete directory remained resident but was
not compiler-loaded.

**Impact.** A checked-in loader substitution can replace the compiler-sealed
service with one admitting arbitrary runtimes and can remove the entire
test-only lifecycle coverage without changing either exact manifest. PR,
push, release CI, and local release accept that tree. This is the same
authority boundary R2-I11 is intended to make fail closed, so a P2 requires a
NO-GO verdict even though the current literal service implementation is sound.

**Required correction.** Bind the compiler edge which selects the v1 root as
part of the positive manifest. At minimum, byte-pin `merge/mod.rs` or an even
narrower dedicated loader module and require the exact canonical gated module
declaration; reject `cfg_attr(path = ...)`, inline replacement modules,
renamed/imported `include`, and any alternate root form independently of the
resident v1 directory digest. Add a compiling negative regression which
redirects the parent edge to a complete unsealed replacement service while
leaving the original v1 tree and artifact adapter unchanged. The regression
must make the structural gate fail before relying on incidental unused-code
warnings, and a positive sentinel should prove the intended lifecycle test
root is actually compiler-loaded.

## Command evidence

Reviewed-tree commands and results:

```text
git rev-parse HEAD
git -C gwz-core rev-parse HEAD
git -C gwz-cli rev-parse HEAD
gwz status --json
```

Matched the exact tuple and reported a clean workspace before review and
before report creation.

```text
./.venv/bin/python gwz-core/scripts/checks/check_checked_artifact_boundaries.py
./.venv/bin/python -m unittest \
  gwz-core/scripts/checks/test_check_checked_artifact_boundaries.py -v
./.venv/bin/python -m unittest \
  gwz-core/scripts/checks/test_release_boundary.py -v
```

Passed: current checker; boundary suite 46/46; release-boundary suite 6/6.

```text
./.venv/bin/python gwz-core/scripts/checks/check_merge_docs.py --workspace-root .
./.venv/bin/python -m unittest \
  gwz-core/scripts/checks/test_check_merge_docs.py -v
./.venv/bin/python \
  gwz-core/scripts/checks/check_merge_compatibility_predicates.py \
  gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json --core gwz-core
./.venv/bin/python -m unittest \
  gwz-core/scripts/checks/test_merge_compatibility_predicates.py -v
```

Passed: document consistency 133 assertions; document tests 3/3;
compatibility registry 7 rules and 7 bindings; compatibility tests 14/14. An
initial invocation mistakenly supplied unsupported `--core` to
`check_merge_docs.py` and exited 2; the documented `--workspace-root .`
invocation above was then run and passed.

```text
cargo +1.95.0 test checked_artifact::interface_tests
cargo +1.95.0 test \
  archive_authority_rejects_a_terminal_label_with_a_contradictory_v0_envelope
cargo +1.95.0 test \
  archive_authority_requires_one_terminal_open_source_and_an_absent_destination
cargo +1.95.0 test --test publish_workflow
```

Passed: 96/96; 1/1; 1/1; and 7/7 respectively.

```text
TAUT_PYTHON=... cargo +1.95.0 test --all-features
```

The full run reached the long v1 preservation/rollback fault matrices with no
reported failure and confirmed the 1,227-test current unit inventory, but was
stopped after the independently reproduced P2 made further duplicate runtime
unnecessary. It is intentionally not reported as a completed pass.

```text
CLIPPY_CONF_DIR="$PWD" cargo +1.95.0 clippy \
  --all-targets --all-features -- -D warnings
cargo +1.95.0 fmt --all -- --check
git diff --check \
  da57559f9ce17d12fec72239f4012cf737589140..b93e26aa3c5fc12ced8a057612ffbb64c2b5423e
```

All passed on the reviewed tree.

## Gate decision

Do not begin R2 production conversion on this tuple. Preserve the current
private production-runtime seal, separate test runner, sealed backend,
read-only preservation observation route, semantic archive authority, and
exact release convergence, but extend the positive manifest through the
parent compiler-loading edge and pin a regression for whole-root substitution.
Repeat both independent settled-tree reviews after correction. This review
returns **NO-GO**.
