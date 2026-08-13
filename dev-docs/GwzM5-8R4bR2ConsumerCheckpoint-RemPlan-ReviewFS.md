# R4b R2 consumer-checkpoint remediation — independent filesystem re-review

Date: 2026-08-14

## Verdict

**NO-GO for R2 production implementation or consumer conversion.**

The remediation closes both filesystem findings from
`GwzM5-8R4bR2ConsumerCheckpoint-ReviewFS.md`. I found no recurrence of either
defect. One new P2 interface defect remains: the coordinator does not bind a
managed-parent request's owner class and purpose set to the exact action and
plan that it admits.

P0: 0. P1: 0. P2: 1. P3: 0.

## Exact tuple reviewed

- workspace root: `0efe97a93a186eab1ed27b7b9a136ca4df8a8884`
- `gwz-core`: `cddd2388223b719bd41b5f5b94d14b982887a5a5`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

`gwz status --json` showed this exact tuple clean before review. The commit
identities remained exact throughout source inspection and verification. A
separate independent review document appeared as an untracked workspace-root
file while the final test run was in progress; it changed no reviewed source
or commit identity and is not an input to this review.

## Closure of the prior filesystem findings

### Prior P2-1 — caller-selected collision facts: closed

`PreCatalogOwnerV1` now exposes distinct workspace and actual-Git-directory
entry points in
`gwz-core/src/checked_artifact/capability/pre_catalog.rs:197-233`. Neither
accepts a root-kind enum, collision domain, index entry, worktree entry, or
caller-computed snapshot. The raw observation/provider contract is private to
the `pre_catalog` subtree
(`capability/pre_catalog/provider.rs:8-40`), and owner construction from a raw
provider is private (`pre_catalog.rs:188-195`).

The issued permit binds `collision_snapshot_digest` and the complete retained
root/path transaction (`pre_catalog.rs:24-92`). The owner selects the matching
root-kind-specific revalidation and invokes catalog bootstrap immediately
afterward, without returning a permit or invoking an intervening caller
callback (`pre_catalog.rs:235-270`). Focused tests prove both entry paths,
immediate ordering, revalidation rejection, and changed-snapshot rejection
before bootstrap.

The real platform observer and real collision matrix remain R2 implementation
work, as intended; the frozen interface no longer permits a consumer to omit
facts or substitute an empty workspace scan for Git-directory proof.

### Prior P2-2 — unmanaged or overlapping managed parents: closed

The managed owner now enforces the fixed minimum retained prefixes:
`MergeStore=1`, `MergeArchive=2`, `PreservationBundles=1`, and
`RootPreservationMarkers=1`
(`gwz-core/src/checked_artifact/bootstrap/managed.rs:45-67` and
`bootstrap/managed/owner.rs:70-105`). It compares the retained path with the
exact fixed purpose prefix, omits fully existing parents as proof-only rows,
and rejects duplicate or ancestor/descendant missing physical edges before
schedule derivation (`bootstrap/managed/owner.rs:88-119,233-264`).

Purpose-specific constructors restrict merge-start, durable-merge, and archive
request shapes (`bootstrap/managed.rs:74-176`). Archive creation requires an
opaque source prerequisite bound to its request owner, and the accepted archive
shape can plan only `done` below an already retained `.gwz/merge`
(`bootstrap/managed.rs:108-120,181-220`). The plan digest binds declared
purposes, observations, retained facts, and physical rows
(`bootstrap/managed/owner.rs:274-337`). Focused tests cover forbidden prefixes,
the overlapping merge-store/archive pair, archive prerequisite substitution,
and the legal one-component archive plan.

## Finding

### [P2-1] Managed owner class, action purposes, and physical plan can be mixed

**Where:**

- `gwz-core/src/checked_artifact/coordinator/identity.rs:191-207` constructs a
  parent-only action from any `CheckedActionOwnerV1` and any
  `ManagedParentBootstrapRequest`; it does not validate their owner classes.
- `gwz-core/src/checked_artifact/bootstrap/managed.rs:181-194` validates owner
  binding only for archive prerequisites. `MergeStart` and `DurableMerge`
  request authorities accept any supplied owner binding.
- `gwz-core/src/checked_artifact/bootstrap/managed/owner.rs:41-47` preflights a
  managed request against separately supplied raw `ActionDigestV1` and
  `RequestOwnerBindingV1`. It does not receive the action request whose digest
  and purpose set it is meant to implement.
- `gwz-core/src/checked_artifact/coordinator/schedule.rs:18-28` compares only the
  plan's action digest and owner binding with the action request. It does not
  compare the action's declared purpose set with the plan's
  `declared_purposes`.

**Violated contract.** RemPlan sections 2 and 6 make owner/purpose authority
closed: the deterministic `ManagedParentsV1` owner may perform only the fixed
pre-record merge-parent work, while `RootPreservationMarkers` requires a
durable merge-record owner. Section 3 makes the purpose bitset part of the exact
action identity, and section 4 requires the coordinator alone to derive the
physical schedule from that request. The current interfaces validate each
piece locally but do not validate the tuple.

**Failure scenarios.** A checked-artifact sibling can pair
`CheckedActionOwnerV1::for_merge_start` with
`ManagedParentBootstrapRequest::try_for_durable_merge([RootPreservationMarkers])`.
`for_managed_parents`, managed preflight, and reservation derivation all accept
the combination, allowing the reproducible pre-record owner to name and admit
work reserved for an already durable merge record.

The same loose boundary permits a subtler substitution with one owner binding:
construct an action whose purpose bitset says `RootPreservationMarkers`, then
preflight a `PreservationBundles` request while supplying that action's digest
and owner binding. The resulting plan retains the supplied digest but schedules
the bundle-parent row. `derive_new_reservation` accepts it because it checks the
digest and binding, not that the plan's declared purposes equal the action's
purpose bitset. The durable catalog identity can therefore describe one
purpose while its physical schedule executes another.

**Impact.** This reopens the authority and restart ambiguity that the
lead-owned checkpoint is intended to eliminate. Parallel R2 consumers can
compile while assigning a managed path to the wrong durable owner, and recovery
cannot infer the real physical purpose from the action identity alone. Because
these interfaces are being frozen specifically to prevent independent R2 lanes
from making that choice, this is a P2 checkpoint blocker even though no
production provider is installed yet.

**Required correction.** Make owner class, managed request, action identity,
and managed plan one coordinator-issued typed chain:

1. Expose an owner-class check inside `ManagedParentBootstrapRequest` (or encode
   it in distinct request types), and reject every owner/request combination
   outside the closed policy table. In particular, a deterministic merge-start
   owner must never authorize archive or root-marker purposes, and a durable
   request must require a `MergeRecordV0` or `MergeRecordV1` owner derived from
   exact durable bytes.
2. Replace managed preflight's loose `(request, action_digest,
   request_owner_binding)` inputs with an opaque coordinator-issued managed
   action containing the exact owner and purpose set, or have preflight receive
   the complete private `CheckedActionRequestV1` and derive both values from it.
3. Before schedule derivation, require exact equality between the action
   purpose bitset and `ManagedParentPlanV1::declared_purposes`, in addition to
   the existing digest and owner checks. Prefer making a mismatched tuple
   unrepresentable; retain an explicit defensive check at the durable boundary.

**Regression gate.** Add an exhaustive owner-variant by request-authority
matrix covering merge-start, durable-merge, and archive requests. Add a test
that deliberately uses one action digest/owner with a different managed
request's purpose set and prove rejection before reservation/admission. For
each rejection, assert zero provider execution and zero private, managed, or
Git mutation. Pin the legal deterministic two-purpose merge-start row and each
legal durable-record purpose as positive controls.

## Verification

On the reviewed core commit:

- `cargo test checked_artifact::interface_tests`: 88 passed;
- `cargo test -p gwz-core`: 1,218 unit tests passed, one ignored, plus all 45
  integration tests passed;
- `python3 scripts/checks/test_check_checked_artifact_boundaries.py`: passed;
- `python3 scripts/checks/check_checked_artifact_boundaries.py`: passed;
- `cargo fmt --all -- --check`: passed; and
- `cargo clippy --all-targets --all-features -- -D warnings`: passed.

The green suite does not cover the cross-owner or same-owner/different-purpose
substitutions above. The current constructor test checks which purposes each
request constructor accepts in isolation; it never pairs those requests with
the wrong owner or action.

## Gate decision

Do not begin R2 production conversion on this tuple. Correct the single typed
binding defect, add the substitution matrix, commit a new settled workspace /
core / CLI tuple, and repeat both independent re-reviews. No public GWZ
protocol, merge lifecycle, or additional durability phase is required.
