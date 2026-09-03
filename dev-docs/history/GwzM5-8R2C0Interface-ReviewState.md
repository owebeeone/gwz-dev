# R2-C0 critical-interface state and lease review

Date: 2026-08-14

## Verdict

**NO-GO.** The settled C0 tree passes its existing focused, full, protocol,
format, and structural gates, and C0 does not create any catalog role. Two
interface defects remain, however: one P1 target-association defect and one P2
duplicate-target race defect. Under the review rule that any P0/P1/P2 finding
is blocking, R2-C0 cannot be accepted yet.

| Severity | Count |
| --- | ---: |
| P0 | 0 |
| P1 | 1 |
| P2 | 1 |
| P3 | 0 |

## Reviewed state

The review began from a clean exact tuple:

- workspace root: `082d0b0c49386f35576c7a8dde871818df7d769f`;
- `gwz-core`: `27c4adece9d03f31f47662d5e1f36604125e7762`;
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`.

The root C0 range is exactly one commit from
`aca9d12d18e1f1d903e909c2a34742b42bee03d4`; the core C0 range is exactly one
commit from `dba602ae1d3e7f0c27a321f15a791d3ec7d98845`. The complete core range is 46
files, `+2024/-1201`. The root range updates the C0 checkpoint, core pin, and
integration marker. I reviewed both ranges against
`GwzM5-8R2CCatalogBootstrapAmendment.md` and R2-C0 in
`GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`.

## Findings

### [P1-1] The frozen lease cannot prove that the later provider root and named lock slot are still the locked target

The amendment requires `CatalogMutationLeaseV1` to retain and bind the exact
workspace or Git-directory target, requires `CatalogOwnerV1` to derive its
target and retained handles from that opaque lease, and forbids an independent
path beside the lease. It also requires wrong-target, substituted-target, and
post-lock drift to reject without catalog mutation
(`GwzM5-8R2CCatalogBootstrapAmendment.md:166-179,190-205,474-483`). The
execution plan likewise says that no production entry accepts caller-selected
root kinds or paths and that the lease binds retained target handles
(`GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md:351-370`).

The acquired lease internally retains the target and lock, and acquisition
does revalidate both before returning. That protection is not carried through
the frozen consumer interface:

- `CatalogMutationLeaseV1` exposes only `root_kind()` and
  `canonical_target_path()`
  (`gwz-core/src/checked_artifact/bootstrap/runtime/catalog_lease.rs:124-160`).
  It exposes no opaque target binding, comparison with a provider observation,
  or revalidation of the target plus named final slot.
- The complete target revalidation exists only on the private
  `RetainedCatalogTargetV1`
  (`gwz-core/src/checked_artifact/bootstrap/runtime/catalog_lease/target.rs:155-193`),
  and named-slot revalidation is executed only inside `acquire_final()` before
  `try_acquire` returns (`target.rs:239-321`).
- `CatalogPermitV1` merely has independent fields for the lease and a provider
  `RetainedPlatformRoot` (`gwz-core/src/checked_artifact/capability/pre_catalog.rs:49-63`).
  There is no C0 issuer or constructor that proves those two capabilities name
  the same live object. Carrying both values does not itself establish that
  relation.
- The retained provider still inspects and revalidates a supplied `&Path`
  (`gwz-core/src/checked_artifact/capability/pre_catalog/provider.rs:34-55`).
  The only lease value available to such an issuer is the canonical path, so a
  fresh open can retain a replacement object rather than the object locked by
  the lease.
- The only catalog lease fault points are preparation, final-file open, and
  final-file lock. Existing substitution tests fire before acquisition
  returns. There is no post-return target or slot substitution test
  (`gwz-core/src/checked_artifact/bootstrap/runtime/fault.rs:3-10` and
  `bootstrap/runtime/catalog_lease/tests.rs:142-212`).

A concrete counterexample is therefore representable:

1. Acquire a lease on target directory A and its final lock file.
2. After `CatalogLeaseSetV1::try_acquire` returns, rename A away and create a
   valid replacement repository/target B at the same canonical path. The
   retained A handles and advisory lock remain valid but no longer name the
   ambient path.
3. A C1 preflight issuer can use only `lease.canonical_target_path()` and opens
   B. The frozen permit shape can then carry the lease for A and retained root
   B without an exact-binding check.
4. Another process can acquire B's newly named lock slot while the first
   process still holds A's old lock. C2 would then have two live authorities
   for the apparent same target. Replacing only the named lock entry after
   return creates the equivalent split-lock case.

The same gap affects the borrowed `WorkspaceRuntimeLease`: it preserves old
retained handles, but its catalog view exposes only the workspace path. Thus
workspace compatibility borrowing is proven at acquisition time, not at
preflight issuance or immediately before a catalog edge.

The Git-target request surface also leaves common-Git selection to the caller:
`CatalogLeaseTargetRequestV1::workspace(path)` and
`git_directory(path)` accept raw paths and choose the root kind
(`target.rs:21-40`). The linked-worktree test passes `repository.commondir()`
and `linked.commondir()` explicitly (`catalog_lease/tests.rs:52-75`), so it
proves contention only after the caller has already selected the same common
directory. Both a linked worktree's actual Git directory and its common Git
directory satisfy the current Git-directory request validation; the frozen
interface does not derive which target is required or reject an actual/common
misassociation. This is part of the same missing target-association proof, not
a separate count.

Required correction:

- Make the only production preflight/owner route consume the lease and an
  owner-private, lease-derived retained target witness; do not accept an
  independently selected root kind or path at that boundary.
- Give the sealed owner a nonforgeable operation that compares the provider
  root with the lease's exact root kind, support profile, canonical location,
  durable identity, invocation identity, lookup mode, rename domain, and
  repository/common/actual-Git relationship.
- Revalidate that binding and the still-named final lock file against the held
  file immediately before permit issuance and before every physical catalog
  edge. The workspace compatibility source must implement the same proof.
- Derive actual-versus-common Git target selection from a sealed retained
  repository purpose rather than a raw `git_directory(path)` choice.
- Add post-return target replacement, post-return final-slot replacement,
  cross-target lease/provider pairing, workspace-borrow substitution, and
  linked actual/common mismatch tests. Each must reject before catalog
  mutation, and a competing acquisition of the replacement slot must expose
  the split-lock counterexample before the fix.

This is P1 because C0 is explicitly freezing the authority interface that C2
will trust. C0 itself has no catalog writer, so the defect is not classified as
an already-active P0 corruption path, but C1/C2 cannot safely consume the
current frozen surface.

### [P2-1] Duplicate-path/different-identity observations can reach preparation mutation before ambiguity is detected

The amendment requires a duplicate path with a different identity to release
the whole set read-only
(`GwzM5-8R2CCatalogBootstrapAmendment.md:181-201`). The implementation first
retains every request, sorts by `binding.order_key`, and then deduplicates
(`gwz-core/src/checked_artifact/bootstrap/runtime/catalog_lease.rs:32-42`).
`deduplicate_exact_targets` compares only adjacent entries whose order keys are
equal (`catalog_lease.rs:181-195`). The order key contains support profile,
durable identity, and root kind, but no canonical path
(`catalog_lease/target.rs:360-378`).

If two requests name the same canonical path and a concurrent replacement
changes A to B between their initial retains, their durable identities and
therefore order keys differ. Both entries survive deduplication. If B sorts
first, phase one can re-retain B successfully and create/converge its final
runtime slot (`catalog_lease.rs:43-70`) before the later A entry fails its
binding comparison. The rejection has then mutated capability-neutral runtime
grammar rather than being read-only. Which identity sorts first must not decide
whether the same semantic ambiguity mutates disk.

Required correction:

- Before phase-one preparation, detect duplicate canonical target locations
  independently of the identity-based acquisition order. Exact full bindings
  may deduplicate; one location with different durable/live/repository binding
  must fail before any guard or final-slot creation.
- Preserve the existing canonical acquisition key for distinct targets after
  this location-ambiguity pass.
- Add a deterministic initial-retention race hook/test that observes one path
  first as A and then as B in both possible identity sort orders, proving no
  guard, runtime directory, or final lock file is created.

This is P2 because the current path does not create a catalog role and later
phase revalidation does reject the mismatched target, but it violates the
frozen read-only failure contract and leaves the required dedupe race unproved.

## Confirmed behavior outside the findings

- The new `DurablePathV1` contains only original/canonical ASCII, parent mode,
  and parent durable identity. Live invocation identity and rename domain stay
  in `CanonicalPathIdentityV1`. Durable decoding rejects empty, mixed-profile,
  oversized, noncanonical, and non-exactly-reencoded values.
- All five affected durable-record roots use `CheckedDurablePathV1`; the
  catalog bootstrap record carries the separately typed durable-target and
  historical-collision digests and no live lease, invocation, or rename-domain
  field.
- `CatalogPreflightV1` is closed to boxed `MissingGitPrivateParent` and `Ready`
  states. The missing permit has only lease, retained root, and missing-parent
  digest fields; it has no ready digest or raw-role material. The ready permit
  has the lease, retained root, live path material, raw roles, and the three
  distinct typed digests. No generic production `CatalogBootstrapV1` callback
  remains.
- Within acquisition, the two phases are non-interleaved: preparation drops
  each transient guard before visiting the next target; final acquisition uses
  existing files only and revalidates targets and slots. Prefix failure and
  `Drop` release held leases in reverse order.
- Existing no-follow, wrong-kind, lock replacement, target substitution,
  preparation/final ordering, workspace-lock compatibility, and reverse-prefix
  release tests pass. Linked worktrees contend when both requests are already
  directed to the same common Git target; P1-1 concerns the missing derivation
  and later association proof.
- A production-source search found no catalog role name, scratch/staging/active
  operation, write, rename, remove, or catalog directory creation in the C0
  lease/preflight production files. Their mutation calls are limited to the
  fixed bootstrap guard, `.gwz/locks` grammar, workspace final lock, and Git
  final lock. Thus C0 remains capability-neutral and C2 remains the first
  planned catalog writer.

## Checks run

All commands were read-only with respect to tracked implementation and design
files. Results:

- `git diff --check dba602ae1d3e7f0c27a321f15a791d3ec7d98845..27c4adece9d03f31f47662d5e1f36604125e7762`
  — passed.
- `cargo test --lib checked_artifact::bootstrap::runtime::catalog_lease::tests:: -- --nocapture`
  — 11 passed, 0 failed.
- `cargo test --lib` — 1,265 passed, 0 failed, 1 ignored; 787.71 s.
- `python3 scripts/checks/check_checked_artifact_boundaries.py` — passed, 15
  visible entries and 5 classified modules.
- `python3 -m unittest scripts.checks.test_check_checked_artifact_boundaries`
  — 56 passed; 560.051 s.
- `cargo fmt --all --check` from the workspace root — passed.
- `python3 protocol/regen.py --check` — passed; committed protocol bindings and
  both corpora are current under `taut-proto` 0.8.1.
- Focused `rg` inventories of the C0 lease/preflight production files found no
  catalog-role or catalog-mutation operation; the only creation/open calls are
  the fixed runtime guard, directories, and final lock slots described above.

## Final tuple verification

After writing this report, the exact commit tuple remained:

- workspace root: `082d0b0c49386f35576c7a8dde871818df7d769f`;
- `gwz-core`: `27c4adece9d03f31f47662d5e1f36604125e7762`;
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`.

`gwz status --json` reported every member clean and every member lock pin
matching. The root had no staged or tracked modification. Its only untracked
paths were the independently authorized review outputs
`dev-docs/GwzM5-8R2C0Interface-ReviewCode.md` and this state report; this review
did not create or modify the code-review output.

## Disposition

R2-C0 remains **NO-GO** until P1-1 and P2-1 are corrected, their negative race
tests pass, the complete gates are rerun on a new settled tuple, and the
independent critical-interface reviews report no open P0/P1/P2 finding.
