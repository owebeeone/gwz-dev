# R2-C0 third critical-interface code re-review

Date: 2026-08-14

## Verdict

**GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 0 |

The immutable second correction closes both findings that controlled the prior
re-reviews. The two batch ordering passes are allocation-free on the pinned
toolchain and are structurally guarded against regression. Every Git request
now retains its own repository/worktree association through common-target
grouping and through the returned lease witness, with revalidation at every
promised acquisition and authority boundary. No new P0-P3 defect was found in
the corrected association, batching, authority, source-gate, or C0 mutation
surfaces.

## Findings

None.

## Closure of the prior findings

### Prior hidden stable-sort allocation P2: closed

- Canonical-location grouping uses `sort_unstable_by` at
  `bootstrap/runtime/catalog_lease.rs:227-234`; final target ordering uses the
  same operation at lines 103-105. There are exactly two ordering calls and no
  `sort_by` call in the sealed batch module.
- Both comparators are total borrowed comparisons: canonical `PathBuf` order
  for grouping and the frozen canonical byte-vector order key for acquisition.
  Stability is neither required for exact-location rejection/merging nor for
  distinct-target acquisition.
- Rust 1.95's pinned `slice::sort_unstable_by` is documented and implemented as
  an in-place operation that does not allocate
  (`library/core/src/slice/mod.rs:3140-3193`). The former hidden stable-sort
  scratch `Vec::with_capacity` path is gone.
- The explicit batch vectors retain typed `try_reserve_exact` coverage:
  bounded input, prepared targets, per-target request lists, unique targets,
  retained associated targets, held leases, and merged request lists all pass
  through `try_reserve_batch`
  (`catalog_lease.rs:55-76,84-100,140-170,227-247,250-268,307-355`). Allocation
  rejection remains before catalog mutation.
- The maximum-size test passes 4,096 requests through grouping and acquisition
  ordering, and the allocation-failure test still rejects before runtime or
  catalog mutation (`catalog_lease/tests/bounds.rs:5-56`). The interface test
  and production source checker independently require exactly two unstable
  sorts and forbid a stable `sort_by`
  (`interface_tests/capability_permit.rs:89-94`;
  `scripts/checks/check_checked_artifact_boundaries.py:868-876`).

### Prior discarded repository/worktree association P1: closed

- One private `CatalogGitAssociationBindingV1` records separate canonical
  request, optional worktree, actual Git-directory, and common Git-directory
  facts. Every directory binding includes its support profile, durable
  identity, invocation identity, rename domain, and lookup mode
  (`catalog_lease/association.rs:13-41`). No live association data enters a
  durable record.
- Association retention derives the actual/common/worktree relationship from
  `git2::Repository::open(request)`, retains all named directories, and
  immediately revalidates the complete relationship. Later revalidation checks
  every retained handle and reopens the canonical request to reproduce the
  exact optional-worktree, actual, and common paths
  (`association.rs:43-131,140-218`). A symlink request rejects before runtime
  mutation.
- Git target construction derives the common target only from that retained
  association, keeps the association beside the common target, and revalidates
  both before returning (`catalog_lease/target.rs:126-131,191-258`). Callers
  still cannot select actual versus common Git lock scope.
- Initial preparation stores every request with its association binding.
  Canonical-target grouping locks once per common target, requires an exact
  target binding, and merges rather than discards distinct request locations.
  The same request location with different membership rejects before phase-one
  preparation (`catalog_lease.rs:29-42,88-105,227-247,307-335`).
- Each phase re-retains and exactly compares every original request
  association. Group revalidation runs before guard acquisition, after the
  guard is held, after final-slot preparation, during final reacquisition,
  immediately after final locking, and after the complete set is held
  (`catalog_lease.rs:107-170,250-305`;
  `catalog_lease/target.rs:304-412`). The returned
  `HeldCatalogTargetV1` owns the primary and every associated retained target.
- `CatalogMutationLeaseV1::begin_preflight` still consumes the lease into the
  opaque witness. Witness construction, facts access, provider binding, ready
  or missing permit issuance, and each permit-edge revalidation reach
  `HeldCatalogTargetV1::revalidate_held`; consequently every retained
  membership and the still-named target lock are rechecked
  (`catalog_lease.rs:190-216`; `catalog_lease/witness.rs:30-49`;
  `capability/pre_catalog.rs:73-164`). Grouped associations do not leak into a
  caller-selected path/provider seam.
- Negative tests cover membership drift after initial retention in both input
  orders, after preparation, after successful return for one request, and
  after successful common-target grouping in both orders. They keep the common
  target intact and reject without a catalog-role write
  (`catalog_lease/tests/association.rs:171-303`).

## Independent interface audit

### Canonical grouping, revalidation, and fallible boundary

The complete input remains nonempty, stops after maximum-plus-one iterator
items, and has a literal 4,096-request ceiling. Location grouping happens
before preparation. Exact target bindings merge; target binding ambiguity or
same-request membership ambiguity rejects before preparation. Every later
reacquisition compares the frozen target and association bindings, while any
failure after a held-prefix acquisition releases that prefix in reverse.

Preparation and final acquisition remain non-interleaved. Phase one may
converge only the fixed capability-neutral guard/directory/final-lock grammar
and releases its transient guard before advancing. Phase two performs no
preparation and holds the complete canonical target set before any future
catalog operation. No transient guard is entered after a final lock is held.

### Authority and call graph

The new association module is private to `catalog_lease`; its binding and
retained handles cannot be fabricated by a checked-artifact consumer. The
existing closed route remains:

```text
sealed request batch -> CatalogLeaseSetV1 -> borrowed CatalogMutationLeaseV1
  -> consumed CatalogLeaseTargetWitnessV1
  -> lease-bound provider observation
  -> Ready or MissingGitPrivateParent permit
```

There is no production callback, direct Git-directory selector,
pathname-plus-lease constructor, synthetic witness, or independent provider
root seam. Ready and missing-parent permit constructors remain private and
accept only the lease-bound observation. Both permits retain the witness and
expose the same owner-private revalidation operation for C2 edges.

### C0 mutation boundary

The new `association.rs` path is read-only: it opens repositories and retained
directories, canonicalizes paths, reads identities/modes/domains, and compares
facts. The production writer inventory in the complete lease/pre-catalog path
is unchanged: it can create only the transient bootstrap guard, workspace
runtime/locks directories, workspace final lock, or common-Git final lock.

No Git `gwz` private parent, catalog record, scratch, active, staging, final,
or other catalog role is created, written, renamed, or removed by C0. C1 can
remain classifier-only and C2 remains the first catalog-role mutator.

### Structural and durable boundaries

- The exact path-and-byte tree manifest for `catalog_lease.rs` includes the new
  association descendant and fails on a changed file set, module edge, or
  byte. Existing exact reference sets continue to freeze batch, request, set,
  lease, witness, provider-ingress, and edge-revalidation consumers.
- The source checker additionally requires common-Git derivation and exactly
  two allocation-free unstable sorts. The direct checker and all 57
  adversarial cases pass.
- The correction does not change the taut durable schema or generated corpus.
  Protocol regeneration and the generated checked-artifact currentness test
  pass. Live request/worktree associations, invocation identities, rename
  domains, and lease handles remain outside durable records.

## Commands and results

The review began from a clean workspace and exact lock-matched member state:

```text
git rev-parse HEAD
  093857634e931696a2178f12970a7743ebe20e5d
git -C gwz-core rev-parse HEAD
  1c5e42db9851c96b801e5aa7e6996207ecd6c514
git -C gwz-cli rev-parse HEAD
  3cca145c0b32410f250f640730ed7ca18f1da59f
gwz status --json
  clean; every member Ok; every lock pin Matches

git -C gwz-core diff --stat d92e4603d427fb1ccfdaca03c666923de1153b98..1c5e42db9851c96b801e5aa7e6996207ecd6c514
  8 files changed, 593 insertions, 46 deletions
git diff --stat 67813a79f90d52cbeefa8152594b3d5ddb84039e..093857634e931696a2178f12970a7743ebe20e5d
  6 files changed, 672 insertions, 12 deletions

cargo +1.95.0 test --lib checked_artifact::bootstrap::runtime::catalog_lease::tests -- --nocapture
  27 passed; 0 failed
cargo +1.95.0 test --lib checked_artifact::capability::pre_catalog::provider::production_tests -- --nocapture
  19 passed; 0 failed
cargo +1.95.0 test --lib checked_artifact::interface_tests -- --nocapture
  98 passed; 0 failed

python3 scripts/checks/check_checked_artifact_boundaries.py
  checked-artifact boundary: ok (15 visible entries, 5 classified modules)
python3 -m unittest scripts.checks.test_check_checked_artifact_boundaries
  57 passed in 327.332s

python3 protocol/regen.py --check
  passed with taut-proto 0.8.1; generated bindings and corpora are current
cargo +1.95.0 test --test protocol generated_checked_artifact_protocol_is_current -- --nocapture
  1 passed; 0 failed

cargo +1.95.0 fmt --all -- --check
  passed
cargo +1.95.0 clippy -p gwz-core --all-targets --all-features -- -D warnings
  passed
git -C gwz-core diff --check d92e4603d427fb1ccfdaca03c666923de1153b98..1c5e42db9851c96b801e5aa7e6996207ecd6c514
  passed
git diff --check 67813a79f90d52cbeefa8152594b3d5ddb84039e..093857634e931696a2178f12970a7743ebe20e5d
  passed

rg -n "\\.sort_by\\s*\\(|\\.sort_unstable_by\\s*\\(" \
  gwz-core/src/checked_artifact/bootstrap/runtime/catalog_lease.rs
  exactly two sort_unstable_by calls; no sort_by call
rg -n "open_or_create_file|ensure_child_directory|create_dir|write\\(|rename\\(|remove_" \
  <catalog-lease and pre-catalog production paths>
  only the fixed capability-neutral runtime guard/directory/final-lock writers
```

The full 1,286-passing-test library and integration matrix was not redundantly
repeated; the committed checkpoint records that settled green run. This review
ran the complete corrected lease, provider, interface, adversarial structural,
protocol-currentness, formatting, lint, and diff checks proportional to the
eight-file core correction.

## Final tuple and status verification

Immediately before completion, the tuple remained:

```text
workspace root 093857634e931696a2178f12970a7743ebe20e5d
gwz-core       1c5e42db9851c96b801e5aa7e6996207ecd6c514
gwz-cli        3cca145c0b32410f250f640730ed7ca18f1da59f
```

`gwz status --json` reported every member clean and every lock pin matching.
The root had no staged or tracked modification; its only untracked files were
the independently authorized third state review and this third code review.
Only this code-review report was written by this review. No implementation,
design, plan, workspace lock, marker, or other file was modified.
