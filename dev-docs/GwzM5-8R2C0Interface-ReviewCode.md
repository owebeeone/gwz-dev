# R2-C0 critical-interface code review

Date: 2026-08-14

## Verdict

**NO-GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 1 |
| P2 | 1 |
| P3 | 0 |

R2-C0 correctly installs the restart-stable wire schema, generated and
independent semantic coverage, closed preflight shapes, pre-return two-phase
lease acquisition, structural compiler/source boundary, and the promised
absence of catalog-role mutation. It is not yet a safe frozen interface for
R2-C1/C2. A returned mutation lease cannot revalidate or bind a later provider
observation to the retained target and named lock that it actually holds. The
lease-set boundary also accepts and enumerates unbounded inputs with
infallible allocation. Both defects must be corrected before C1 begins.

## Findings

### P1-1: a returned lease cannot prove that C1 observes the target whose lock it holds

The controlling amendment requires `CatalogOwnerV1` to derive both the target
path and retained handles from the opaque lease, never pair a target from one
lease with another, and revalidate the target/membership binding before
mutation. The frozen interface cannot express that proof:

- `CatalogMutationLeaseV1` retains either `WorkspaceRuntimeLease` or
  `HeldCatalogTargetV1`, but its only owner-visible target operations are
  `root_kind()` and `canonical_target_path()`
  (`bootstrap/runtime/catalog_lease.rs:124-160`). There is no post-return
  revalidation operation and no opaque retained-target proof.
- `RetainedCatalogTargetV1::revalidate()` and the named-final-lock checks do
  exist, but run only inside acquisition before `CatalogLeaseSetV1` returns
  (`catalog_lease/target.rs:155-193, 239-321`). They are not reachable through
  `CatalogMutationLeaseV1`. The workspace compatibility source has the same
  problem: its retained handles and `revalidate_runtime_tree` are private to
  initial runtime acquisition.
- The future permit shape stores `_catalog_target_lease` and the provider's
  separately retained `_retained_root` side by side
  (`capability/pre_catalog.rs:49-70`). The current provider seam still inspects
  and revalidates a separately supplied root (`provider.rs:34-55`). No common
  opaque identity is available with which the C1 issuer could prove that these
  two values designate the same retained object.

A concrete counterexample remains after all current tests have returned:

1. acquire target A and its named final lock;
2. rename/replace the target path with valid target B, or unlink and replace
   the named lock file while the process still holds the old inode;
3. have C1 inspect the unchanged pathname, which now retains B (or have a
   second process lock the replacement lock inode); and
4. construct the frozen permit from A's still-live lease plus B's retained
   provider facts.

The path strings and root-kind tag still agree, but the catalog operation is
not serialized by B's lock. The existing substitution tests do not close this
window: their fault hooks run inside `acquire_final()` before the lease is
returned (`catalog_lease/tests.rs:142-211`). There is no test that substitutes
the target or named slot after successful acquisition and before preflight or
an owner edge. The source comment that carrying the lease inside the permit
makes substitution impossible (`pre_catalog.rs:49-50`) is therefore stronger
than the type invariant.

**Required correction text:** Replace the pathname accessor as the C1 entry
seam with one sealed lease-owned operation, for example
`CatalogMutationLeaseV1::begin_preflight`, that revalidates the originally
retained target, repository relationship, runtime directories, and named
final-lock entry/inode and then derives or binds the provider's retained root
from that same opaque target capability. Ready and missing-parent permit
constructors must accept only this lease-derived bound observation, never an
independently reopened path/root. Preserve an owner-private revalidation
operation on the resulting permit and invoke it immediately before every C2
physical edge. Add workspace and actual-Git-directory tests that successfully
acquire first, then substitute (a) the target and (b) the named lock slot;
preflight/edge execution must return typed read-only ambiguity and another
process must not obtain concurrent mutation authority.

### P2-1: the lease-set and equivalent-alias proofs have no finite, fallible envelope

The new security boundary has two unbounded paths:

- `CatalogLeaseSetV1::try_acquire` accepts an arbitrary `IntoIterator`, drains
  it into `Vec::new()` with `push`, and later uses `Vec::with_capacity`
  (`catalog_lease.rs:29-41, 77, 181-194`). There is no literal maximum, no
  exact bounded batch type, and no `try_reserve`. An internal caller can pass
  `std::iter::repeat(request)`, causing nontermination/resource exhaustion
  before deduplication; a sufficiently large finite set can take the
  infallible allocation failure path.
- `reject_equivalent_alias` reads an arbitrary parent directory to EOF with no
  entry, native-name-unit, or aggregate-byte budget
  (`catalog_lease/target.rs:381-415`). It is applied to actual Git directories
  and lock directories during preparation/acquisition. These parents contain
  caller-controlled ordinary names and are not structurally finite.

This falls short of the amendment's bounded/fail-closed namespace discipline
and leaves C1/C2 dependent on caller convention rather than a safe frozen
lease interface. It is also not covered by the 11 lease tests: there is no
maximum/maximum-plus-one batch or parent-enumeration case.

**Required correction text:** Introduce a sealed, nonempty
`CatalogLeaseTargetBatchV1` derived by the checked coordinator from its complete
read-only target selection, freeze a justified literal maximum target count,
reject maximum-plus-one before preparation, and use checked arithmetic plus
`try_reserve_exact` for every batch allocation. Make equivalent-alias proof
finite as well: on a sensitive parent avoid a distinct-alias scan; on an
ASCII-case-fold parent charge every native name losslessly against literal
entry, native-unit, and aggregate-byte budgets and return typed capacity
ambiguity before final-lock acquisition. Add maximum/maximum-plus-one,
repeating-iterator, allocator-failure, large-parent, and Unix non-Unicode/native
Windows-name tests. No over-budget case may create a catalog role or return a
lease.

## Confirmed properties

- The taut schema has the exact corrected durable path component tags 1-4,
  path tag 1, and catalog-bootstrap tags 1-12. All five durable-record roots
  now reference `CheckedDurablePathV1`; no durable root references the former
  mixed live path shape. The adapter enforces nonempty, component canonical,
  one-profile, canonical-reencoding, and 4-KiB bounds.
- `CheckedCatalogBootstrapV1` contains durable target and historical collision
  digests and restart-stable retained-parent identity/path. Invocation
  identities, rename domains, and durable lease material are absent. Generated
  Rust/corpus output is current, and the independent 26-vector semantic suite
  covers all three durable identity kinds, both lookup modes, both catalog root
  kinds, and all three filesystem profiles.
- `CatalogPreflightV1` has only boxed `MissingGitPrivateParent` and `Ready`
  variants. The missing permit has only lease, retained root, and the missing
  observation digest; it has none of the ready raw roles or three ready
  digests. The former callback, path-plus-lease bootstrap entry, and durable
  lease binding names are absent. Digest domains are distinct private types.
- Before it returns, `CatalogLeaseSetV1` retains exact targets, sorts by the
  stable `(support profile, durable identity, root kind)` key, rejects
  mismatched duplicate bindings, performs all preparation without final
  leases, releases every transient guard, acquires final leases in order, and
  releases the held prefix in reverse on failure/contention. Workspace lock
  compatibility and the shared linked-worktree Git-directory slot are green.
- The C0 production delta creates only the allowed runtime guard/directories
  and workspace/Git final lock files. Searches of the added production writer
  calls found no catalog scratch, active, staging, final, record, or catalog
  directory creation. Those role names occur in the new lease tree only in
  the test assertion that they remain absent. C1 can remain classifier-only
  after the two interface corrections; C2 remains the first catalog mutator.
- The source checker semantically requires Cargo's exact `[lib]` target
  `src/lib.rs`, hashes `lib.rs`, `workspace_ops/mod.rs`, and
  `workspace_ops/merge/mod.rs`, retains the intended v1 positive sentinel, and
  freezes the complete durable-path, pre-catalog, and catalog-lease trees,
  lease reference sets, source-loading edges, and removed provisional names.
  The adversarial suite passes all 56 cases, including crate/parent redirect
  probes and allowed non-target Cargo metadata change. CI/release integration
  is unchanged by this C0 delta.
- The root checkpoint updates only the remediation claims, workspace lock, and
  checkpoint marker. The lock and marker select core
  `27c4adece9d03f31f47662d5e1f36604125e7762`; CLI remains
  `3cca145c0b32410f250f640730ed7ca18f1da59f`.

## Commands and results

Initial status was clean in the workspace root, `gwz-core`, and `gwz-cli`.

```text
git rev-parse HEAD
  082d0b0c49386f35576c7a8dde871818df7d769f
git -C gwz-core rev-parse HEAD
  27c4adece9d03f31f47662d5e1f36604125e7762
git -C gwz-cli rev-parse HEAD
  3cca145c0b32410f250f640730ed7ca18f1da59f

git -C gwz-core diff --name-status <core-parent>..<core-head>
  46 changed/added core paths; complete C0 delta inspected
git diff --name-status <root-parent>..<root-head>
  remediation plan, workspace lock, and one checkpoint marker

cargo +1.95.0 test --lib checked_artifact::bootstrap::runtime::catalog_lease::tests -- --nocapture
  11 passed; 0 failed
cargo +1.95.0 test --lib checked_artifact::interface_tests -- --nocapture
  97 passed; 0 failed
cargo +1.95.0 test --lib checked_artifact::capability::pre_catalog::provider::production_tests -- --nocapture
  15 passed; 0 failed
cargo +1.95.0 test --test protocol generated_checked_artifact_protocol_is_current -- --nocapture
  1 passed; 0 failed; generated bindings and corpus current

python3 scripts/checks/check_checked_artifact_boundaries.py
  checked-artifact boundary: ok (15 visible entries, 5 classified modules)
python3 -m unittest scripts.checks.test_check_checked_artifact_boundaries
  56 passed in 506.309s

cargo +1.95.0 fmt --all -- --check
  passed
cargo +1.95.0 clippy -p gwz-core --all-targets --all-features -- -D warnings
  passed
git -C gwz-core diff --check <core-parent>..<core-head>
  passed
git diff --check <root-parent>..<root-head>
  passed
```

Here `<core-parent>` is
`dba602ae1d3e7f0c27a321f15a791d3ec7d98845`, `<core-head>` is
`27c4adece9d03f31f47662d5e1f36604125e7762`, `<root-parent>` is
`aca9d12d18e1f1d903e909c2a34742b42bee03d4`, and `<root-head>` is
`082d0b0c49386f35576c7a8dde871818df7d769f`.

The full 1,265-test Rust matrix was not redundantly rerun; the committed
checkpoint records that settled green run, while this review independently ran
the complete C0-focused Rust and adversarial structural gates plus all-target,
all-feature Clippy.

The tuple was rechecked immediately before completion:

```text
workspace root 082d0b0c49386f35576c7a8dde871818df7d769f
gwz-core       27c4adece9d03f31f47662d5e1f36604125e7762
gwz-cli        3cca145c0b32410f250f640730ed7ca18f1da59f
```

Both subrepositories remained clean. Final root status contained only the
untracked `dev-docs/GwzM5-8R2C0Interface-ReviewCode.md` review report.

Only this review report was written. No implementation, amendment, plan,
workspace lock, marker, or other file was modified.
