# R2-C0 corrected critical-interface code re-review

Date: 2026-08-14

## Verdict

**NO-GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 1 |
| P3 | 0 |

The corrected checkpoint closes the original target-authority counterexample,
the unbounded-input and lossy-alias counterexamples, and the
duplicate-location preparation race. The lease-owned witness, provider-root
binding, common-Git derivation, closed permit issuers, source gates, and C0
mutation boundary are suitable seams for C1/C2.

One allocation counterexample still violates the controlling requirement that
the bounded batch use fallible reserved allocation throughout. Both batch
ordering passes call Rust's stable slice sort, whose pinned 1.95 implementation
allocates an auxiliary `Vec` with the aborting global-allocation API. A large
valid batch can therefore abort rather than return the typed allocation error.
Under the review rule, this P2 keeps C0 at NO-GO.

## Finding

### P2-1: stable batch sorting bypasses the fallible allocation envelope

The amendment freezes a maximum of 4,096 targets and says the batch “uses
fallible reserved allocation throughout” and rejects allocation failure before
mutation (`GwzM5-8R2CCatalogBootstrapAmendment.md:186-196`). The corrected
batch explicitly uses `try_reserve_exact` for collection, prepared, unique, and
held vectors (`catalog_lease.rs:44-64,73-85,122-124,207-247`). It nevertheless
uses stable `sort_by` in both batch ordering passes:

- canonical-location grouping sorts `PreparedCatalogTargetV1` values at
  `catalog_lease.rs:207-214`;
- final canonical acquisition ordering sorts the distinct prepared targets at
  `catalog_lease.rs:85-86`.

On the pinned Rust 1.95 toolchain, the standard-library documentation states
that stable slice sort allocates `len` elements for medium inputs and `len / 2`
for larger inputs (`alloc/src/slice.rs:152-161`). Its implementation routes
`sort_by` through `stable_sort` and instantiates the scratch buffer with
`Vec::with_capacity` (`alloc/src/slice.rs:188-197,854-867`). That allocation is
not covered by `try_reserve_batch`, its error cannot become
`CheckedFsError::Unsupported`, and allocation failure follows Rust's global OOM
path. The existing injected failure test only trips the explicit reserve at
the beginning of `try_acquire` (`catalog_lease/tests/bounds.rs:28-40`), so all
focused and full checkpoint tests can pass without probing either sort.

Both sorts occur before the phase-one runtime-slot preparation loop, so this is
not an observed catalog corruption or partial-mutation path. It is P2 because
the frozen bounded interface still fails to provide its promised typed,
fallible resource boundary and can terminate the checked command on a valid
maximum-size input.

**Required correction text:** Replace both stable sorting calls with an
in-place, non-allocating ordering operation such as `sort_unstable_by`; neither
comparison needs stability because canonical-location equality is handled by
the following full-binding deduplication and the final acquisition comparator
is the frozen total key. Alternatively, explicitly acquire a fallibly reserved
scratch buffer and use only a sort implementation that consumes that buffer
without any hidden allocation. Add a source-level or allocator-backed test
that exercises both ordering stages at a nontrivial batch length and proves no
infallible allocation occurs outside the typed reserve path. Preserve the
existing rule that every allocation rejection precedes phase-one preparation.

## Original findings and requested interface audit

### Original target-authority finding: closed

- `CatalogMutationLeaseV1::begin_preflight(self)` consumes the lease into the
  opaque `CatalogLeaseTargetWitnessV1` (`catalog_lease.rs:170-196`; witness
  fields are private at `catalog_lease/witness.rs:11-13`). There is no longer a
  production pathname-plus-lease entry seam.
- Witness creation and every later `facts` call revalidate the retained target.
  A lease-set witness calls `HeldCatalogTargetV1::revalidate_held`, which checks
  the target/repository association, retained runtime/locks directories when
  applicable, and the still-named lock entry against the held file
  (`witness.rs:30-50`; `catalog_lease/target.rs:341-384`). The workspace
  compatibility witness calls the corresponding retained-root, repository,
  runtime, locks, and named-lock revalidation
  (`bootstrap/runtime/mod.rs:47-81,201-235`). This closes post-return target and
  named-slot substitution for both lease sources.
- The witness facts bind root kind, support profile, canonical target path,
  target durable and invocation identities, target mode and rename domain, and
  the same live/durable/mode/rename tuple for the related Git directory
  (`catalog_lease/witness.rs:15-28,46-94`).
- `inspect_bound_catalog_target` consumes only that witness, derives the
  provider path from its facts, compares the complete target and related-Git
  association, and revalidates both sides before returning the bound
  observation (`provider/filesystem.rs:77-88,155-216`). Permit-edge
  revalidation repeats retained-root and repository relationship checks before
  and after the exact association comparison (`provider/filesystem.rs:218-256`).
- A Git request accepts a repository/worktree purpose and privately derives
  `Repository::commondir`; the former direct actual/common Git selector is
  absent (`catalog_lease/target.rs:22-45,82-89,124-135`). The Git binding and
  provider revalidation require target, actual, common, and related-Git paths to
  converge on that common directory (`provider/filesystem.rs:227-237`). Linked
  worktrees therefore share the one target and fixed final slot.
- The only ready and missing-parent issuers are private constructors taking
  `LeaseBoundPreCatalogObservationV1`. Each revalidates before construction;
  ready requires a retained private parent, while missing requires a Git target
  with no private parent (`pre_catalog.rs:92-164`). The closed enum has only
  `MissingGitPrivateParent` and `Ready`; the missing permit has none of the
  ready role/digest material (`pre_catalog.rs:43-71`). Both permit forms expose
  the same owner-private target-binding revalidation seam for future physical
  edges (`pre_catalog.rs:73-79,130-136`).

The corrected post-return workspace/Git target, named-slot, cross-target, and
related-Git substitution tests all pass. The former split-lock/provider-root
pairing counterexample is no longer representable through the frozen
production surface.

### Original boundedness/alias finding: original counterexamples closed

- `CatalogLeaseTargetBatchV1::try_new` is sealed, nonempty, stops after the
  maximum-plus-one item, rejects more than 4,096 items (including an infinite
  iterator), and uses the typed fallible reserve helper
  (`catalog_lease.rs:32-66`). The original unbounded-drain counterexample is
  closed.
- Case-fold alias proof charges every native entry against literal per-parent,
  per-name, and aggregate budgets. Unix names use raw bytes; Windows names use
  native UTF-16 units; unsupported platforms fail closed; sensitive parents do
  not enumerate aliases (`catalog_lease/alias.rs:11-135`). The original
  unbounded and lossy alias counterexamples are closed.
- Before any preparation, targets are grouped by canonical location. Exact
  full bindings deduplicate, and the same location with any different target
  or repository binding rejects (`catalog_lease.rs:85,207-227`). Tests exercise
  both identity orders and full live/repository binding differences
  (`catalog_lease/tests/association.rs:108-172`). The prior identity-sort-order
  preparation race is closed.

P2-1 is a distinct hidden allocation inside the now-bounded ordering operation;
it does not reopen the old infinite-input, alias, or duplicate-location
counterexamples.

### Other audited properties: confirmed

- The durable taut schema and generated semantic corpus are unchanged by the
  correction and remain current. Durable records use
  `CheckedDurablePathV1`; invocation identities, rename domains, leases, and
  live witnesses remain only in live capabilities/permits.
- The complete catalog-lease and pre-catalog module trees are protected by
  exact path-and-byte source-tree manifests. Exact reference sets freeze the
  batch, request, set, lease, witness, `begin_preflight`, lease-bound provider,
  and edge-revalidation seams; the checker also rejects reintroduction of the
  direct Git-directory selector and provisional callback interfaces
  (`scripts/checks/check_checked_artifact_boundaries.py:147-156,458-524,
  838-867`). The direct checker and all 57 adversarial checker tests pass.
- The corrected C0 production delta creates only capability-neutral runtime
  grammar: the transient bootstrap guard, `.gwz/locks` directories, workspace
  final lock, and Git final lock. No catalog private parent, record, scratch,
  active, staging, or final catalog role is created. C1 remains classifier-only
  and C2 remains the first catalog-role mutator.
- The correction is implementable for C1/C2 after P2-1: the sole provider
  ingress consumes a target-bound witness, closed issuers consume only the
  bound observation, and both permit variants retain an exact per-edge
  revalidation operation.

## Commands and results

The review began with the exact requested tuple. `gwz status --json` reported
all members clean and all lock pins matching; the workspace root was clean at
that time.

```text
git rev-parse HEAD
  67813a79f90d52cbeefa8152594b3d5ddb84039e
git -C gwz-core rev-parse HEAD
  d92e4603d427fb1ccfdaca03c666923de1153b98
git -C gwz-cli rev-parse HEAD
  3cca145c0b32410f250f640730ed7ca18f1da59f

git -C gwz-core diff --stat 27c4adece9d03f31f47662d5e1f36604125e7762..d92e4603d427fb1ccfdaca03c666923de1153b98
  17 files changed, 1,313 insertions, 200 deletions
git diff --stat 082d0b0c49386f35576c7a8dde871818df7d769f..67813a79f90d52cbeefa8152594b3d5ddb84039e
  6 files changed, 640 insertions, 7 deletions

cargo +1.95.0 test --lib checked_artifact::bootstrap::runtime::catalog_lease::tests -- --nocapture
  21 passed; 0 failed
cargo +1.95.0 test --lib checked_artifact::capability::pre_catalog::provider::production_tests -- --nocapture
  19 passed; 0 failed
cargo +1.95.0 test --lib checked_artifact::interface_tests -- --nocapture
  97 passed; 0 failed

python3 scripts/checks/check_checked_artifact_boundaries.py
  checked-artifact boundary: ok (15 visible entries, 5 classified modules)
python3 -m unittest scripts.checks.test_check_checked_artifact_boundaries
  57 passed in 362.966s
python3 protocol/regen.py --check
  passed; taut-proto 0.8.1 generated bindings and both corpora are current

cargo +1.95.0 fmt --all -- --check
  passed
cargo +1.95.0 clippy -p gwz-core --all-targets --all-features -- -D warnings
  passed
git -C gwz-core diff --check 27c4adece9d03f31f47662d5e1f36604125e7762..d92e4603d427fb1ccfdaca03c666923de1153b98
  passed
git diff --check 082d0b0c49386f35576c7a8dde871818df7d769f..67813a79f90d52cbeefa8152594b3d5ddb84039e
  passed

rustc +1.95.0 --version
  rustc 1.95.0 (59807616e 2026-04-14)
rg -n "\\.sort_by\\(" gwz-core/src/checked_artifact/bootstrap/runtime/catalog_lease.rs
  86: prepared.sort_by(...)
  210: targets.sort_by(...)
rg -n "Vec::with_capacity\\(capacity\\)|medium sized slices allocate|clamps at" \
  ~/.rustup/toolchains/1.95.0-aarch64-apple-darwin/lib/rustlib/src/rust/library/alloc/src/slice.rs
  stable-sort documentation reports auxiliary allocation; BufGuard uses
  Vec::with_capacity(capacity)
```

The 1,279-test library and integration matrix was not redundantly repeated;
the committed checkpoint records that settled green run. This re-review ran
the complete corrected lease, provider, interface, source-boundary,
format/lint, and protocol-currentness checks needed to evaluate the correction.

## Final tuple and workspace verification

Immediately before this report was finalized:

```text
workspace root 67813a79f90d52cbeefa8152594b3d5ddb84039e
gwz-core       d92e4603d427fb1ccfdaca03c666923de1153b98
gwz-cli        3cca145c0b32410f250f640730ed7ca18f1da59f
```

`gwz status --json` again reported every member clean and every member lock pin
matching. The root had no staged or tracked modification. Its only untracked
files were the independently authorized state-review output and this code
review. Only this code-review report was written by this review; no
implementation, design, plan, workspace lock, marker, or other file was
modified.
