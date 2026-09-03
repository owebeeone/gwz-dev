# R4b R2 consumer remediation checkpoint — independent state re-review

Date: 2026-08-14

Reviewed exact immutable tuple:

- workspace: `0efe97a93a186eab1ed27b7b9a136ca4df8a8884`
- `gwz-core`: `cddd2388223b719bd41b5f5b94d14b982887a5a5`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

Primary remediation target:
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-RemPlan.md`.

Prior state review:
`dev-docs/GwzM5-8R4bR2ConsumerCheckpoint-ReviewState.md`.

## Verdict

**NO-GO for R2 consumer conversion.**

P0: 0. P1: 0. P2: 3. P3: 2.

The tuple materially improves the rejected checkpoint. It excludes ordinary
stash and commit-marker identities from checked ownership, defines a
reproducible pre-record merge-parent owner, implements a framed owner/action
encoding with literal digest fixtures, fixes the coordinator's leaf schedule
table at 64 barriers and the expected cleanup masks, and replaces caller-owned
filesystem fact slices with provider-owned snapshots. The managed-parent path,
minimum-prefix, archive-prerequisite, and physical-overlap checks are also
present.

The remaining P2 findings are interface failures, not requests for later R2
consumer implementation. The current types permit an owner class to authorize
another class's managed purposes, permit a managed plan for one purpose set to
be scheduled under another request's action digest, and do not implement the
claimed exhaustive checked/ordinary call-graph gate. Parallel R2 lanes would
have to make or work around those architectural choices, so the remediation
exit gate is not closed.

## Prior-finding closure

| Prior finding | Result at this tuple |
| --- | --- |
| Recoverable durable owner | **Partially closed.** Ordinary generated stash/marker IDs are correctly left ordinary and merge start has a reproducible owner. Owner-to-authority coupling and exact-record owner issuance remain open in P2-1. |
| Byte-exact owner/action identity | **Substantially closed.** The versioned, length-framed encoding and literal substitutions are implemented. The prefixed-ID edge cases remain P3-1. |
| Coordinator-owned immutable schedule | **Partially closed.** Leaf rules, masks, barrier count, and proof-only behavior are frozen. The action-to-plan purpose binding remains open in P2-2, and the promised parent schedule vectors remain P3-2. |
| Checked-versus-ordinary boundary | **Documented but not enforced.** The prose classification is now clear; the executable gate remains shallow and non-exhaustive in P2-3. |

## Findings

### [P2-1] Owner variants are not coupled to managed authority, and durable-record owner issuance is not closed

**Where:** remediation lines 55-81 and exit criteria 335-337;
`gwz-core/src/checked_artifact/coordinator/identity.rs:43-78,190-208,407-430`;
and `gwz-core/src/checked_artifact/bootstrap/managed.rs:75-114,181-208`.

**Violated contract.** The remediation gives `ManagedParentsV1` exactly one
pre-record authority: the fixed `MergeStore + PreservationBundles` parent-only
action. It gives record-owned bundle, marker, and archive work only to an owner
issued from an exact bounded durable record read.

The types do not enforce either boundary:

- `CheckedActionRequestV1::for_managed_parents` accepts every
  `CheckedActionOwnerV1` with every `ManagedParentBootstrapRequest`; it derives
  the purpose bitset but never checks the owner's variant.
- `ManagedParentBootstrapRequest::validate_authority` checks only the archive
  prerequisite. `MergeStart` and `DurableMerge` authority are not compared with
  the supplied owner binding or owner class.
- The only non-test owner constructor exported from `identity.rs` is
  `for_merge_start`. The record constructors exposed to the rest of the crate
  are test-only. The private common constructor accepts workspace, merge, and
  operation IDs independently from an arbitrary nonempty source byte slice;
  it does not consume one opaque, bounded, verified record observation from
  which all fields are derived.

**Failure scenario.** At the current internal API, a caller can create a
`ManagedParentsV1` owner with `for_merge_start("ws_1")`, create a
`DurableMerge` request containing `RootPreservationMarkers`, and successfully
construct the parent-only action. The supposedly pre-record owner can therefore
name a purpose that the remediation expressly reserves for a durable record.
The future record-owned consumer has the opposite problem: there is no frozen
production issuer that proves its IDs came from the exact bytes being hashed,
so R2-E would have to add or change this interface.

**Required correction.** Make owner class and managed authority one sealed
construction decision. A merge-start constructor must be able to construct
only its fixed two-purpose request; durable v0/v1 constructors must accept an
opaque bounded verified-record observation and derive workspace ID, merge ID,
operation ID, version, and source hash from that one value. Do not accept those
IDs or a digest separately. Reject every cross-owner/cross-authority
composition in type-level regression tests, including merge-start plus root
markers/archive and record-owner plus merge-store.

### [P2-2] A managed plan can be issued for one purpose set and reserved under another action digest

**Where:** remediation lines 149-194 and I3 at 313-315;
`gwz-core/src/checked_artifact/bootstrap/managed/owner.rs:41-61,120-145`;
`gwz-core/src/checked_artifact/bootstrap/managed/plan.rs:94-137`; and
`gwz-core/src/checked_artifact/coordinator/schedule.rs:18-67`.

**Violated contract.** The action digest is supposed to bind the exact declared
purpose set, while the plan and schedule bind the missing suffix observed for
that same request. Consumers are not supposed to compose digests, bindings,
plans, or schedules.

`ManagedParentBootstrapOwnerV1::preflight` nevertheless accepts a request plus
raw `ActionDigestV1` and `RequestOwnerBindingV1` values. It records the request's
actual `declared_purposes`, but nothing proves that those purposes are the ones
encoded by the supplied action digest. `derive_new_reservation` then compares
only the plan's copied digest and owner binding with the checked request. It
does not compare the plan's `declared_purposes` with the checked request's
purpose bitset.

**Failure scenario.** Construct checked request A for one legal purpose set.
Preflight request B for a different legal purpose set while supplying A's
action digest and owner binding. The plan contains B's rows and declared
purposes but A's digest and binding. `derive_new_reservation(A, Some(plan_B))`
accepts it and schedules B's managed mutations under a catalog action whose
identity claims A. The same substitution can use two subsets of the durable
merge purpose class; it does not depend on the P2-1 merge-start mismatch.

This breaks resident lookup, authorization, schedule identity, and retry
meaning even though every individual digest comparison succeeds.

**Required correction.** Introduce one opaque coordinator-issued action value
that carries the sealed owner, action request, and exact managed request.
Managed preflight must accept that value, not caller-supplied digest/binding
scalars. Plan issuance must derive its bindings internally. Reservation must
also verify exact declared-purpose equality between the action and plan before
using schedule inputs. Add negative tests for every same-owner purpose-set
substitution, reordered/reduced/expanded sets, and a plan from a different
owner/action. The corrected public-in-module surface must make the invalid
composition unrepresentable rather than relying on each future consumer to
repeat comparisons.

### [P2-3] The checked/ordinary boundary gate is neither exhaustive nor fail-closed

**Where:** remediation lines 275-303, I4 at 316-317, and exit criteria 339-340;
`gwz-core/scripts/checks/check_checked_artifact_boundaries.py:19-50,53-87`;
`gwz-core/scripts/checks/test_check_checked_artifact_boundaries.py:12-21`;
and `gwz-core/src/workspace_ops/handle_stash/commands.rs`.

**Violated contract.** The remediation requires call-graph gates that enumerate
every ordinary command, prove that none reaches checked authority, prove that
every selected merge path reaches its exact owner, and fail when a new call
site is unclassified.

The implemented checker is explicitly a small source inventory rather than a
call-graph gate. It scans eleven named top-level files for five text tokens.
That list includes `handle_stash.rs` but not its `handle_stash/commands.rs`,
`projection.rs`, or `shared.rs` implementation modules. It cannot see a checked
call placed in a transitive helper. Across the complete production tree it
inventories only the two legacy strings `CheckedArtifact::acquire` and
`CheckedArtifact::prepare_parent`; a new coordinator/facade entry name is not
classified. There is no complementary check that selected merge preservation,
publication, rollback, and recovery paths reach their required owner. The unit
test only asserts that the unchanged current tree passes. No negative mutation
fixtures prove failure, and the checker is not wired into a discovered CI or
release gate.

**Failure scenario.** An ordinary stash implementation can call a new checked
facade from `handle_stash/commands.rs`, or a shared ordinary helper can call it
transitively, and the checker still reports success. Conversely, a converted
merge path can retain a raw successful bypass and the checker has no assertion
to fail. This is the exact compatibility drift the gate was intended to stop.

**Required correction.** Enforce one architectural checked-entry module whose
complete production caller set can be enumerated, or build a full production
source/call inventory covering entry modules, submodules, and shared helpers.
Classify every checked entry symbol, not only legacy names, and fail on an
unknown entry or call site. Add the complementary exact-owner reachability gate
for every selected merge path. Add negative fixtures that inject a checked call
into an ordinary submodule/helper, add an unclassified facade, and preserve a
raw merge bypass. Wire the gate into the mandatory local/CI check used for this
checkpoint.

## Lower-priority findings

### [P3-1] Prefixed ID validation accepts the forbidden `.` and `..` suffixes

**Where:** remediation lines 109-112; and
`gwz-core/src/checked_artifact/coordinator/identity.rs:373-400,407-414`.

The frozen encoding says workspace and operation IDs exclude the `.`/`..`
suffix/slug. `validate_slug_id` enforces this for merge IDs, but
`validate_prefixed_id` checks only prefix, length, and character set. It accepts
`ws_.`, `ws_..`, `op_.`, and `op_..`. Validate the suffix after the required
prefix and add boundary literals for both prefixed ID classes.

### [P3-2] Coordinator fixtures do not pin the promised managed-parent schedule vectors

**Where:** remediation lines 191-194 and exit criterion 338; and
`gwz-core/src/checked_artifact/interface_tests/coordinator_contract.rs:440-576`.

The coordinator contract test pins schedule/reservation digests for three leaf
mutation shapes and tests only a fully present, proof-only parent plan. It does
not derive and pin a reservation for a missing one-, two-, or four-purpose plan,
the first-merge two-purpose plan, partial bootstrap, or combined parent-plus-
leaf work. Generic R1 schedule tests do not prove the coordinator's purpose and
missing-suffix mapping. Add literal schedule digest, reservation digest,
ordinal/range, cleanup-mask, and capacity assertions for the exact vector set
promised by the remediation.

## Checks observed

The following passed on the exact core SHA:

- `cargo test -p gwz-core checked_artifact::interface_tests`: 88 passed;
- `cargo test -p gwz-core --test protocol`: 29 passed;
- `cargo fmt --all -- --check`;
- `cargo clippy -p gwz-core --all-targets -- -D warnings`;
- `python3 scripts/checks/check_checked_artifact_boundaries.py`; and
- `python3 -m unittest scripts/checks/test_check_checked_artifact_boundaries.py`.

The passing boundary check is not evidence against P2-3: its present behavior
is the subject of that finding. Likewise, the focused tests exercise valid
compositions but do not attempt the cross-purpose substitutions in P2-1/P2-2.

## Gate

The exact reviewed tuple is **NO-GO**. R2 consumer conversion should remain
blocked until P2-1 through P2-3 are corrected on one new settled tuple and two
independent re-reviews of that tuple report no open P0/P1/P2 defect. P3-1 and
P3-2 should be corrected in the same interface checkpoint because both freeze
inputs used by durable identity and restart behavior; they should not be left
for parallel consumers to interpret independently.
