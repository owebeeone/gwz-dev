# R2-C0 critical-interface state/lease re-review 2

Date: 2026-08-14

## Verdict

**NO-GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 1 |
| P2 | 1 |
| P3 | 0 |

The corrected checkpoint closes the two findings in the original independent
state/lease review: a returned workspace or Git lease now owns a consumed
target witness that revalidates the target and named final-lock inode, and a
canonical location observed with different bindings is rejected before
phase-one preparation. The workspace compatibility route, common-Git lock
serialization, two-phase ordering, reverse release, no-follow checks, and
preflight/provider association are substantially stronger.

The interface is nevertheless not ready to freeze. A Git request is collapsed
to its common Git directory without retaining the repository/worktree
membership that selected that target, and deduplication can discard additional
request associations. In addition, both batch sorts use Rust's allocating
stable sort, so allocation failure is not represented by the promised typed,
fallible batch boundary. Both are controlling-design violations; R2-C0 remains
NO-GO under the rule that any open P0/P1/P2 blocks acceptance.

## Reviewed state

The review started from the clean, exact committed tuple:

```text
workspace root  67813a79f90d52cbeefa8152594b3d5ddb84039e
gwz-core        d92e4603d427fb1ccfdaca03c666923de1153b98
gwz-cli         3cca145c0b32410f250f640730ed7ca18f1da59f
```

The root correction has parent
`082d0b0c49386f35576c7a8dde871818df7d769f`; the core correction has parent
`27c4adece9d03f31f47662d5e1f36604125e7762`. The complete 17-file core
correction and six-file root correction were reviewed. `AGENTS.md`,
`AGENTS_GWZ.md`, and `gwz-core/AGENTS.md` were reread before review. The
controlling amendment, R2-C0 checkpoint plan, and original state/lease report
were read in full. No implementation, design, plan, lock, or checkpoint file
was modified by this review.

## Findings

### P1-1: Git request/worktree membership is discarded before the returned witness

The amendment says a Git request names a repository or worktree and the owner
derives its common Git directory
(`GwzM5-8R2CCatalogBootstrapAmendment.md:166-170`). It further requires exact
repository-membership binding during canonical-location deduplication, final
set validation, and every later witness edge (lines 186-196 and 206-220).

The implementation binds only the derived common directory:

- `CatalogLeaseTargetRequestV1` initially carries the repository/worktree path
  (`catalog_lease/target.rs:22-45`), but
  `retain_repository_common_git_directory` opens it, derives `commondir`, and
  immediately delegates only that common path to `retain_git_directory`
  (lines 124-135).
- The Git binding then stores the common directory as both the target and the
  `related_git_directory`; both retained handles are opened on that same path
  (lines 110-121). `CatalogTargetBindingV1` has no field for the requesting
  repository/worktree path, its actual Git directory, or their membership
  relation (lines 59-80).
- Git revalidation consequently proves only that the common directory still
  opens as its own actual Git directory (lines 193-236). It cannot revisit the
  repository/worktree which selected it.
- Canonical-location deduplication keeps one `PreparedCatalogTargetV1` and
  drops every later equal binding (`catalog_lease.rs:207-226`). Thus two linked
  worktrees which initially resolve to one common directory lose at least one
  request association. The returned `HeldCatalogTargetV1` and
  `CatalogLeaseTargetWitnessV1` retain only the common target binding; witness
  facts merely copy it (`catalog_lease/witness.rs:37-94`).

A concrete race remains. Let linked worktrees L1 and L2 initially resolve to
common directory C. Initial retention gives both the same current binding, so
deduplication keeps one request. If the discarded worktree's `.git` membership
is changed to a different still-valid common directory D while C and C's lock
entry remain untouched, every retained fact and lock for C still revalidates.
Phase two or a post-return `begin_preflight` can therefore issue authority for
C even though one selected Git request now resolves to D. The same blind spot
exists for a one-request Git lease after return: changing only the external
repository/worktree-to-common association does not change the retained common
target. This is the membership-drift case that amendment line 220 requires to
release read-only.

The passing linked-worktree test proves stable L1/L2 requests share C's lock,
but does not mutate either request association. The passing post-return Git
replacement test renames/replaces C itself, which the retained target detects;
it does not exercise association drift while C stays intact.

**Required correction:** Retain an exact per-request association witness for
the named repository/worktree, including the canonical actual Git directory,
the derived common directory, and their stable/live relationship. Group and
lock once per common target without discarding any association witness.
Revalidate every original association before/after preparation, after final
acquisition, at `begin_preflight`, and at each permit edge. Add negative tests
for linked-worktree membership substitution after initial observation, after
deduplication in both input orders, and after successful return; all must
reject without catalog mutation while the original common directory and its
named lock remain unchanged.

### P2-1: canonical batch sorting bypasses the fallible allocation boundary

The amendment requires the bounded batch to use "fallible reserved allocation
throughout" and to reject allocation failure (`...Amendment.md:186-196`). The
explicit `try_reserve_exact` wrappers cover the visible batch vectors, but two
operations bypass them:

```text
catalog_lease.rs:86       prepared.sort_by(...order_key...)
catalog_lease.rs:210-214  targets.sort_by(...canonical_path...)
```

In the pinned Rust 1.95.0 standard library, `slice::sort_by` is the stable
driftsort entry point. Its own documentation states that medium inputs allocate
`self.len()` auxiliary elements and larger inputs allocate up to
`self.len() / 2`; the API is compiled only with global OOM handling and has no
`Result` allocation-failure path
(`library/alloc/src/slice.rs:78-101, 154-199`). Therefore a valid batch of at
most 4,096 targets can still take an infallible OOM/abort path rather than the
typed read-only failure promised by C0. The allocation-injection test can only
fail `try_reserve_batch`; it cannot reach or control either hidden sort-buffer
allocation.

Both sorts happen before catalog mutation, so this is not a partial-catalog
write defect. It remains a frozen-interface availability/fail-closed defect
because allocation failure is not returned through the checked boundary.

**Required correction:** Use an allocation-free in-place ordering operation
for both passes (for example, `sort_unstable_by` with the same total
comparators), or supply explicitly reserved fallible scratch storage to a
sorting implementation which cannot allocate behind the boundary. Add a
structural regression that rejects allocating stable sorts in the sealed
batch path, plus a focused maximum-batch ordering test. Preserve the exact
canonical acquisition key and pre-preparation ambiguity pass.

## Original findings reproduced and closed

- **Original P1, post-return target/slot association:** closed for the retained
  targets that the implementation actually records. `CatalogMutationLeaseV1`
  now exposes only consumed `begin_preflight`; the witness revalidates a lease-
  set target through `HeldCatalogTargetV1::revalidate_held` and a borrowed
  workspace target through `WorkspaceRuntimeLease::revalidate_catalog_target`.
  The Git target/lock and workspace root/slot post-return substitution tests
  all pass. Provider observation is derived from the witness, cross-target
  pairing rejects, and a substituted related-Git provider capability rejects.
- **Original P2, duplicate path/different binding reaching preparation:**
  closed. The canonical-location pass runs before phase one, exact duplicates
  deduplicate, and any full stored-binding mismatch returns ambiguity. Both
  identity orders and live target/related-Git mismatches pass their negative
  tests with guard/final/catalog roles absent.

P1-1 above is narrower than the original pathname/lock-substitution defect:
the common target and its lock remain valid, but the repository/worktree
association used to select that common target is no longer retained. P2-1 is
the remaining hidden allocation edge in the bounded-batch correction.

## Other confirmed properties

- The workspace compatibility lease cannot fabricate a Git target and
  revalidates its canonical workspace root, actual workspace Git directory,
  runtime directory, locks directory, and named final-lock inode before later
  authority. A batch workspace request contends on the same existing slot.
- While worktree membership remains stable, main and linked-worktree requests
  derive the same common Git target and serialize on one final lock. Callers no
  longer choose actual versus common Git-directory lock scope.
- Preparation and final acquisition are non-interleaved. No final target lock
  is held during phase one; final slots are acquired in canonical-key order,
  and the held prefix is released in reverse on contention or error.
- Target, directory, and final-slot wrong-kind, symlink/no-follow, open/lock
  substitution, and reacquisition races fail. Alias observation is skipped on
  sensitive parents and charged losslessly against the literal limits on
  case-fold parents. The nonempty 4,096-target/max-plus-one iterator boundary
  terminates an infinite iterator.
- `CatalogPreflightV1` remains closed and its missing-parent and ready states
  are disjoint. The sole production provider entry consumes the lease witness;
  permit revalidation compares the retained provider root with target and
  related-Git facts.
- The reviewed production writer calls create only the fixed runtime bootstrap
  guard, workspace runtime/locks directories, workspace final lock, or Git
  final lock. The workspace runtime directory is the pre-existing
  capability-neutral `.gwz` parent; no catalog scratch, active, staging,
  final, durable record, or Git `gwz` private parent is created by C0.
  Provider inspection/revalidation remains read-only.
- The direct checked-artifact boundary checker passes and still freezes the
  catalog-lease/witness/pre-catalog source tree and production reference sets.

## Commands and evidence

```text
git rev-parse HEAD
git -C gwz-core rev-parse HEAD
git -C gwz-cli rev-parse HEAD
git status --short
git -C gwz-core status --short
git -C gwz-cli status --short
gwz status --json
  exact requested tuple; all three trees initially clean; workspace clean

git rev-parse HEAD^
git -C gwz-core rev-parse HEAD^
  exact requested root/core parents; each correction is one direct commit

git diff --stat 082d0b0...67813a7
  6 files changed, 640 insertions(+), 7 deletions(-)
git -C gwz-core diff --stat 27c4ade...d92e460
  17 files changed, 1313 insertions(+), 200 deletions(-)
git diff --check 082d0b0...67813a7
git -C gwz-core diff --check 27c4ade...d92e460
  passed

cargo +1.95.0 test --lib \
  checked_artifact::bootstrap::runtime::catalog_lease::tests -- --nocapture
  21 passed; 0 failed

cargo +1.95.0 test --lib \
  checked_artifact::capability::pre_catalog::provider::production_tests \
  -- --nocapture
  19 passed; 0 failed

cargo +1.95.0 test --lib checked_artifact::interface_tests -- --nocapture
  97 passed; 0 failed

python3 scripts/checks/check_checked_artifact_boundaries.py
  checked-artifact boundary: ok (15 visible entries, 5 classified modules)

python3 -m unittest scripts.checks.test_check_checked_artifact_boundaries
  57 passed; 0 failed

cargo +1.95.0 fmt --all -- --check
  passed

rustc +1.95.0 --version --verbose
rg -n "pub fn sort_by|allocation behavior" \
  $(rustc +1.95.0 --print sysroot)/lib/rustlib/src/rust/library/alloc/src/slice.rs
  rustc 1.95.0; local pinned-library documentation confirms allocating stable
  sort behavior

rg -n "sort_by\\(" \
  gwz-core/src/checked_artifact/bootstrap/runtime/catalog_lease.rs \
  gwz-core/src/checked_artifact/bootstrap/runtime/catalog_lease
  two allocating stable-sort call sites in the sealed batch path

rg -n "open_or_create_file|ensure_child_directory|create_dir|write\\(" \
  <reviewed production catalog-lease/pre-catalog paths>
  only capability-neutral runtime guard/directory/final-lock writers; provider
  route contains no catalog writer
```

The full Rust suite was not repeated: the checkpoint already records a clean
1,279-test pre-review run, and this independent re-review ran the proportional
21 lease, 19 production-provider, and 97 structural/interface tests plus the
57-case adversarial source-boundary suite, direct source boundary, and
formatting/diff checks.

## Final tuple verification

Final verification after writing this report:

```text
workspace root  67813a79f90d52cbeefa8152594b3d5ddb84039e
gwz-core        d92e4603d427fb1ccfdaca03c666923de1153b98
gwz-cli         3cca145c0b32410f250f640730ed7ca18f1da59f
```

The root has this required untracked state-review report plus the concurrent,
independently produced untracked `GwzM5-8R2C0Interface-ReviewCode-2.md`;
`gwz-core` and `gwz-cli` remain clean. This review wrote only
`GwzM5-8R2C0Interface-ReviewState-2.md`, and no reviewed committed file changed
during the review.
