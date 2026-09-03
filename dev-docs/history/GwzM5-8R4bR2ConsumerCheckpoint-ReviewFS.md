# R4b R2 checked-artifact consumer checkpoint filesystem review

Date: 2026-08-14

## Verdict

**NO-GO for beginning R2 production work.** The checkpoint has the right
package sequence, consumer direction, platform evidence requirement, and
legacy-closure goal, but two already-frozen ownership interfaces cannot enforce
claims on which the production conversion depends. Both should be corrected at
the lead-owned interface checkpoint before platform/provider implementation is
distributed.

## Settled tuple reviewed

- workspace root: `e7e3c1be0286e8d1dddf1f11a7228c020b365e87`
- `gwz-core`: `c239df33ddf567d4eaca182ca8231ed575e95769`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

`gwz status --json` showed the tuple clean before review. I inspected the R2
checkpoint, controlling RemPlan-4 and accepted R1 material, the actual R1
interfaces, and production callers/legacy filesystem paths.

## Findings

### [P2-1] A caller can omit Git collision facts and still reach catalog bootstrap

**Evidence:**

- `gwz-core/src/checked_artifact/capability/pre_catalog.rs:202-217` exposes
  `PreCatalogOwnerV1::recover_or_create` to checked-artifact siblings with
  caller-supplied `&[LosslessIndexEntry]` and `&[TrackedWorktreeEntry]`.
- `gwz-core/src/checked_artifact/capability/pre_catalog/provider.rs:17-25`
  forwards those slices into the raw provider; the provider does not own a Git
  repository/index observation capability from which it must derive a complete
  snapshot.
- `gwz-core/src/checked_artifact/capability/pre_catalog.rs:218-227` puts only
  `domain.version_digest()` into the permit. That digest is computed solely
  from the four fixed private-domain names in
  `capability/collision.rs:126-149`; it does not commit to the observed index or
  tracked-worktree facts.
- `revalidate` receives only `root` and the permit
  (`provider.rs:27-31`), so the interface cannot require a fresh re-observation
  of the Git facts or prove that the supplied slices were complete.

This contradicts checkpoint section 5, which says one provider transaction
owns collision inputs, gathers the complete lossless Git facts, and revalidates
the collision digest immediately before catalog bootstrap. A production
facade—or a later refactor within `checked_artifact`—can pass empty or partial
slices. With an index-only, higher-stage, skip-worktree, gitlink, ancestor, or
equivalent private-path collision on disk, the provider can inspect the
incomplete input and issue a permit. Catalog bootstrap can then mutate the
private namespace despite a collision that must be a zero-mutation rejection.

**Required correction:** make collision acquisition owner/provider-controlled,
not caller-asserted. The production pre-catalog entry point should receive the
retained repository/worktree source (or an opaque lossless Git snapshot issued
by a sealed Git observer) and derive the complete index and tracked-worktree
facts inside the same sealed transaction. Its observation/permit must bind a
digest of the complete observed collision fact set and the repository/index
identity needed to revalidate it. Immediate revalidation must re-observe or
otherwise prove that exact snapshot before calling `CatalogBootstrapV1`.
Workspace and Git-directory root kinds should be separate closed entry paths so
the latter supplies its non-worktree proof rather than an empty workspace scan.

**Regression gate:** a production-shaped test must demonstrate that no API can
provide a hand-selected fact list. For every collision class, insert the
collision in the real index/worktree, request a checked action through the
facade, and assert the private root and all managed parents remain byte-for-byte
absent/unchanged. Add a mutation-boundary test that changes the index after the
first observation and before revalidation and proves bootstrap is not called.

### [P2-2] Managed-parent planning does not enforce the frozen path-ownership and overlap rules

**Evidence:**

- The four purpose paths include the overlapping pair `.gwz/merge` and
  `.gwz/merge/done` and the user-owned prefix `gwz.conf` in
  `gwz-core/src/checked_artifact/bootstrap/managed.rs:21-52`.
- `ManagedParentBootstrapRequest::try_new` validates only count, uniqueness,
  and enum order (`managed.rs:67-89`). It accepts `MergeStore` plus
  `MergeArchive` in the same request without considering their shared missing
  suffix.
- `ManagedParentBootstrapOwnerV1::preflight` accepts any per-purpose provider
  row whose retained count is below the path length
  (`bootstrap/managed/owner.rs:41-87`). It neither rejects overlapping missing
  suffixes nor enforces a purpose-specific minimum retained parent.
- It then counts both rows' missing components independently and schedules both
  (`owner.rs:88-117`). For example, if only `.gwz` exists, accepted observations
  for `MergeStore` at count 1 and `MergeArchive` at count 1 yield missing
  suffixes `merge` and `merge/done`. Two independent bootstraps reserve and try
  to own the same `merge` component. If only the workspace root exists,
  `RootPreservationMarkers` can similarly plan creation of `gwz.conf`, despite
  the architecture forbidding creation of a user-owned parent.

This contradicts checkpoint section 9: overlapping missing suffixes are not
independently requested; `MergeArchive` is legal only after `.gwz/merge`
exists; and writers cannot turn an `exists()` result into parent authority. It
also contradicts the controlling rule that existing user-managed parents are
never created by checked bootstrap. The current schedule can admit impossible
or unauthorized work and spend capacity on duplicate physical components.

**Required correction:** freeze a purpose policy owned by the managed-parent
owner, not by the platform provider. It must specify the minimum retained
prefix and permitted creator for each purpose. At minimum:

- `MergeArchive` requires an exact retained `.gwz/merge` proven from the
  validated terminal/source record before planning only `done`;
- `RootPreservationMarkers` requires exact retained user-owned `gwz.conf` and
  may plan only `markers`;
- overlapping missing physical components across rows are rejected before
  schedule/reservation derivation (or deliberately coalesced into one owner row
  with one set of component/generation slots); and
- the provider's returned retained count/path/identity is checked against that
  policy and against all other rows.

The validated archive source dependency must be an opaque prerequisite bound
to the request/owner, rather than a consumer boolean.

**Regression gate:** add owner-level and production-shaped tests for
`MergeStore + MergeArchive` with missing `merge`, archive with both source and
destination absent, archive with destination-only state, a missing
`gwz.conf`, and every pair of purpose paths. Assert rejection occurs before
admission/private/managed mutation and before any Git mutation. Pin the legal
source-only archive path to one `done` component and a non-overlapping schedule.

## Confirmed direction outside the blockers

The following checkpoint decisions are physically coherent and need no
separate finding:

- Runtime bootstrap has a fixed additive grammar and correctly requires the
  actual Git directory, including linked-worktree resolution, as an explicit
  input. Converting the current recursive `WorkspaceMutatorLock` creator into
  this retained, no-follow compatibility wrapper is feasible. Create/init
  ordering after repository creation is accounted for.
- Catalog bootstrap uses the accepted five-role aggregate recovery model;
  staging/final evidence is role-distinct and rejection is zero-mutation. A
  real retained catalog/global enumerator is a missing R2 implementation, not a
  new protocol phase.
- Admission types have deterministic names, reservation-bound schedules,
  explicit occupancy limits, and a private `AdmittedActionV1` issuer. The
  proposed persistent `Idle -> Preparing -> Idle` executor and global
  re-enumeration fit those contracts once collision and managed-plan issuance
  are repaired.
- `ActionNamespace` now has owner-bound forward and restart observations for
  managed install/marker retirement and no free consumer evidence constructor.
  A real platform backend and `LeafObserver` can be implemented beneath the
  private seams.
- The conversion table covers the principal managed creators and checked
  mutations: runtime lock, merge store/archive, ordinary and merge stash
  bundles, ordinary and merge root markers, workspace lock/marker artifacts,
  `.git/info/exclude`, and test-gated v1 store/root/bundle paths. Actual source
  inspection confirms the corresponding legacy creators still exist, so R2-F
  must retain a source-level bypass inventory/gate rather than relying only on
  functional tests.
- The plan properly requires native macOS, Linux ext4, and Windows NTFS
  behavior evidence; release builds are not treated as platform evidence.
- Proposed package ownership is cohesive. No planned owner needs to become a
  god file; the existing large identity implementation is explicitly not the
  destination for new platform-provider concepts.

## Verification performed

This was an interface/design checkpoint review, so no production code was
changed and no test result is used to excuse the two ownership defects. I
verified the exact clean tuple and traced:

- all production `WorkspaceMutatorLock` acquisition sites;
- all production `CheckedArtifact::acquire` sites;
- merge-store, archive, stash-bundle, root marker/lock, manifest, and
  `.git/info/exclude` writers;
- remaining `create_dir_all`, `prepare_parent`, random scratch, deletion, and
  legacy checked-artifact paths;
- pre-catalog permit issuance and revalidation;
- catalog recovery/admission issuance;
- managed plan/schedule binding and namespace backend call graphs; and
- public/private module visibility and current file ownership.

## Gate decision

R2-A runtime implementation could be reasoned about independently, but the
checkpoint explicitly gates all R2 production conversion on both critical
reviews. Accordingly, **do not begin R2 production work yet**. Amend the R1/R2
interfaces for P2-1 and P2-2, add the focused compile/behavior gates, commit a
new settled tuple, and re-review that checkpoint. No public GWZ protocol or
merge lifecycle change is required to resolve either issue.
