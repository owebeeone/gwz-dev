# R2-C0 critical-interface state/lease re-review 3

Date: 2026-08-14

## Verdict

**GO**

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 0 |
| P2 | 0 |
| P3 | 0 |

The second correction closes both findings from the prior immutable
checkpoint. Every Git request now retains its own repository/worktree
membership while requests sharing one common Git target share only the final
lock target. Canonical-target grouping preserves all distinct association
witnesses, and the held lease revalidates them at every subsequent authority
transition. Both ordering passes now use allocation-free unstable sorting and
the source/interface gates forbid a return to stable allocating sort.

Independent source tracing, race-counterexample review, and proportional
exact-tree checks found no open state, lease, allocation, ordering, authority,
or C0 mutation-boundary defect. All P0-P3 counts are zero, so the corrected
R2-C0 checkpoint is GO under the requested acceptance rule.

## Reviewed immutable state

The review began from the clean exact tuple:

```text
workspace root  093857634e931696a2178f12970a7743ebe20e5d
gwz-core        1c5e42db9851c96b801e5aa7e6996207ecd6c514
gwz-cli         3cca145c0b32410f250f640730ed7ca18f1da59f
```

`gwz status --json` reported the workspace and all members clean, every lock
pin matching, and the requested core/CLI heads. The root correction is the
single commit after `67813a79f90d52cbeefa8152594b3d5ddb84039e`; the core
correction is the single commit after
`d92e4603d427fb1ccfdaca03c666923de1153b98`.

The complete eight-file core correction and six-file root correction were
reviewed. `AGENTS.md`, `AGENTS_GWZ.md`, and `gwz-core/AGENTS.md` were reread.
The complete controlling amendment, checkpoint plan, second code review, and
second state/lease review were read before auditing the implementation. This
review did not modify implementation, design, plan, lock, marker, or any other
committed file.

## Prior findings closed

### Prior P1: repository/worktree membership survives common-target deduplication

The correction represents target and request association separately:

- `CatalogGitAssociationBindingV1` binds the canonical request directory,
  optional worktree, actual Git directory, and common Git directory. Every
  directory binding includes support profile, durable identity, invocation
  identity, rename domain, and lookup mode
  (`catalog_lease/association.rs:13-41`). Retention obtains no-follow canonical
  directories and retained handles, then immediately revalidates the complete
  relationship (`association.rs:43-78,93-130,140-217`).
- A Git target is derived only from the association's common directory and
  carries the retained association beside the common target
  (`catalog_lease/target.rs:126-131`). Target revalidation repeats the common
  target facts and the complete association (`target.rs:191-238`). A caller
  still cannot choose actual versus common lock scope.
- Initial preparation stores one `PreparedCatalogRequestV1` per request,
  including its association binding (`catalog_lease.rs:29-42,88-103`). The
  canonical-location pass merges request lists rather than discarding the
  later target. Same-location target bindings must still match exactly; an
  exact duplicate request location deduplicates only if its complete
  association also matches (`catalog_lease.rs:227-247,307-335`).
- `RetainedCatalogTargetGroupV1` re-retains every preserved request and
  compares both the common target binding and exact request association before
  proceeding (`catalog_lease.rs:250-305`). It revalidates every member before
  phase-one work, before and after final-slot preparation, again before final
  acquisition, and after the complete final lock set is acquired
  (`catalog_lease.rs:109-169,250-289`).
- Final acquisition transfers every associated retained target into
  `HeldCatalogTargetV1`; `revalidate_held` checks the primary, every associated
  request, retained runtime/locks directories where applicable, and the named
  final-lock inode (`catalog_lease/target.rs:304-361,365-410`). The association
  witnesses therefore survive the local preparation vectors and remain live
  for the complete lease lifetime.

The prior counterexample—L and main initially resolve to common directory C,
then L is repointed while C and C's named lock remain intact—is closed in both
input orders. Drift after initial retention is detected before the bootstrap
guard or final slot is created. Drift after phase-one preparation is detected
before final acquisition and can leave only authorized capability-neutral
runtime lock grammar. Drift after return is detected for a single linked
request and for both deduplicated request orders at `begin_preflight`. The 27
lease tests exercise all of those cases and assert catalog roles remain absent
(`catalog_lease/tests/association.rs:171-282`).

The remaining authority edges are closed by the same retained witness rather
than a parallel association check. `CatalogLeaseTargetWitnessV1::try_new` and
`facts` call `HeldCatalogTargetV1::revalidate_held`
(`catalog_lease/witness.rs:30-49`). Provider observation and permit issuance
revalidate that witness before and after the exact provider-root comparison
(`provider/filesystem.rs:155-255`; `pre_catalog.rs:92-127,139-163`). Both ready
and missing-parent permit edge methods call
`revalidate_lease_root_binding`, whose first and last actions revalidate the
witness (`pre_catalog.rs:73-79,130-136`; `provider/filesystem.rs:218-255`).
Thus a membership drift after provider observation, permit issuance, or any
later C2 edge attempt reaches the same complete association rejection.

### Prior P2: both batch sorts are allocation-free

The two former stable `sort_by` calls are gone. Canonical-location grouping
uses `sort_unstable_by` at `catalog_lease.rs:230`, and final acquisition-key
ordering uses `sort_unstable_by` at line 105. Both comparators are total byte/
path orders and unstable sorting allocates no auxiliary heap buffer.

Every batch-owned vector introduced or grown by the correction uses the typed
`try_reserve_batch` path: the bounded input, prepared targets, per-target
request list, unique targets, merged request witnesses, retained associated
targets, and held lease prefix. Allocation failure therefore returns a typed
error; before phase one it is fully read-only, and after preparation it can
leave only the explicitly allowed runtime lock grammar. No catalog mutation is
reachable.

The maximum 4,096-entry test runs both ordering sites and successfully
deduplicates to one lease. The interface test and source checker independently
require exactly two `.sort_unstable_by(` calls and reject any `.sort_by(` call
in the sealed batch path (`interface_tests/capability_permit.rs:89-94`;
`scripts/checks/check_checked_artifact_boundaries.py:868-878`). This closes the
hidden standard-library stable-sort allocation path identified by both prior
reviews.

## Independent state/lease audit

- **Target and named-slot authority:** workspace compatibility still borrows
  only its exact retained workspace lease. Lease-set targets retain the common
  Git directory, final-lock file, runtime/locks handles where applicable, and
  every request association. Post-return target, workspace root, and both
  named-slot replacement tests reject, while a replacement process can lock
  the substituted slot; this proves the old inode cannot authorize the new
  path.
- **Linked-worktree/common-Git serialization:** main, linked-worktree root,
  and actual-Git-directory requests derive the common Git directory privately.
  Stable associations deduplicate to one target lease, and a second request
  contends on that one final slot. Distinct request witnesses remain attached
  to the held target even though `CatalogLeaseSetV1::len()` is one.
- **Phase and race ordering:** phase one retains and validates the complete
  request group, holds no final lease, uses only the transient bootstrap guard,
  and releases that guard before visiting the next target. Phase two never
  re-enters a guard, re-retains every group, acquires final locks in canonical
  key order, and performs a final whole-set validation. Membership, target,
  lock-open, lock-acquisition, and reacquisition fault points all fail before
  catalog authority.
- **Canonical order and reverse release:** distinct target groups are ordered
  by the frozen support-profile/durable-identity/root-kind key. Later-target
  contention and error paths call `release_reverse`; the focused test proves
  every earlier slot can immediately be reacquired after the failure.
- **Capacity and alias behavior:** the batch is sealed, nonempty, capped at
  4,096 requests, consumes at most maximum-plus-one iterator items, and
  terminates an infinite iterator. Case-fold alias enumeration remains bounded
  and lossless; sensitive parents do not enumerate, and Unix non-Unicode names
  are charged rather than skipped.
- **Read-only/capability-neutral failure:** duplicate location with different
  target identity, different related-Git facts, changed association, symlinked
  request, target/slot substitution, wrong kind, alias ambiguity, contention,
  or allocation failure returns no catalog lease/permit. A failure after
  preparation may leave only the fixed guard/runtime/locks/final-lock grammar
  authorized by C0.
- **No catalog mutation:** the new production association module contains no
  writer. The only writer added by this correction is the test-only `.git`
  membership fault injection. Production C0 writers remain limited to the
  transient bootstrap guard, workspace runtime/locks directories, workspace
  final lock, and Git final lock. No Git `gwz` private parent, catalog scratch,
  active record, staging directory, final catalog, or durable catalog record
  is created. Provider and permit paths remain observation/revalidation only.

## Commands and results

```text
git rev-parse HEAD
  093857634e931696a2178f12970a7743ebe20e5d
git -C gwz-core rev-parse HEAD
  1c5e42db9851c96b801e5aa7e6996207ecd6c514
git -C gwz-cli rev-parse HEAD
  3cca145c0b32410f250f640730ed7ca18f1da59f
git status --short
git -C gwz-core status --short
git -C gwz-cli status --short
gwz status --json
  exact tuple; root and all members initially clean; all lock pins match

git rev-parse HEAD^
  67813a79f90d52cbeefa8152594b3d5ddb84039e
git -C gwz-core rev-parse HEAD^
  d92e4603d427fb1ccfdaca03c666923de1153b98
git diff --stat HEAD^..HEAD
  6 files changed, 672 insertions(+), 12 deletions(-)
git -C gwz-core diff --stat HEAD^..HEAD
  8 files changed, 593 insertions(+), 46 deletions(-)
git diff --check HEAD^..HEAD
git -C gwz-core diff --check HEAD^..HEAD
  passed

cargo +1.95.0 test --lib \
  checked_artifact::bootstrap::runtime::catalog_lease::tests -- --nocapture
  27 passed; 0 failed

cargo +1.95.0 test --lib \
  checked_artifact::capability::pre_catalog::provider::production_tests \
  -- --nocapture
  19 passed; 0 failed

cargo +1.95.0 test --lib checked_artifact::interface_tests -- --nocapture
  98 passed; 0 failed

python3 scripts/checks/check_checked_artifact_boundaries.py
  checked-artifact boundary: ok (15 visible entries, 5 classified modules)

python3 -m unittest -v \
  scripts.checks.test_check_checked_artifact_boundaries.CheckedArtifactBoundaryTest.test_provisional_catalog_callback_interface_cannot_return \
  scripts.checks.test_check_checked_artifact_boundaries.CheckedArtifactBoundaryTest.test_catalog_mutation_lease_cannot_escape_its_exact_reference_set \
  scripts.checks.test_check_checked_artifact_boundaries.CheckedArtifactBoundaryTest.test_catalog_lease_tree_rejects_an_unreviewed_target_helper \
  scripts.checks.test_check_checked_artifact_boundaries.CheckedArtifactBoundaryTest.test_git_lease_target_cannot_return_to_a_caller_selected_directory
  4 passed; 0 failed

cargo +1.95.0 fmt --all -- --check
  passed

rg -n "\\.sort_by\\s*\\(|\\.sort_unstable_by\\s*\\(" \
  gwz-core/src/checked_artifact/bootstrap/runtime/catalog_lease.rs
  exactly two sort_unstable_by calls; zero sort_by calls

rg -n "open_or_create_file|ensure_child_directory|create_dir|create_new|write\\(" \
  <reviewed production catalog-lease and pre-catalog paths>
git -C gwz-core diff d92e460...1c5e42d -- src/checked_artifact | \
  rg '^\\+.*(create_dir|create_new|write\\(|open_or_create_file|ensure_child_directory)'
  production writers remain capability-neutral runtime grammar; the only new
  writer is test-only linked-worktree membership fault injection
```

The checkpoint records the clean 1,286-test library run, all integration
binaries, the complete 57-case adversarial boundary suite, Clippy, protocol,
document, and release gates on this exact settled tree. This independent
review did not redundantly repeat those full matrices; it ran the complete
27-test lease/race suite, 19-test production-provider suite, 98-test interface
suite, direct checker, four catalog-specific adversarial probes, and focused
format/diff/static checks proportional to the correction.

## Final tuple and status verification

Immediately before finalizing this report, the committed tuple remained:

```text
workspace root  093857634e931696a2178f12970a7743ebe20e5d
gwz-core        1c5e42db9851c96b801e5aa7e6996207ecd6c514
gwz-cli         3cca145c0b32410f250f640730ed7ca18f1da59f
```

`gwz-core` and `gwz-cli` remained clean. The root contained only the authorized
untracked review output described by the final status command. This review
wrote only `dev-docs/GwzM5-8R2C0Interface-ReviewState-3.md`; no reviewed
committed file changed.
