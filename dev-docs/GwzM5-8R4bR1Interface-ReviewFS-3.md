# R4b R1 checked-artifact filesystem/interface settled-tree re-review

Date: 2026-08-14

## Reviewed settled tuple

- workspace root: `c51cae969a6549a6986750447b8a648e444aa709`
- `gwz-core`: `a2a7125fe6b69f09ca76d83caeb28b0684c3c295`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

The three checked-out commits matched this tuple, and the workspace root and
both reviewed member worktrees were clean before this review artifact was
written. This is a fresh filesystem/interface review against:

- `GwzM5-8R4bR1InterfaceCheckpoint.md`;
- `GwzM5-8R4bR1Interface-RemPlan.md`;
- the preceding filesystem and state/protocol reviews, including both `-2`
  reviews; and
- the controlling R4b design and accepted amendments.

The review concentrated on cross-root/provider/record substitution,
revalidation ordering, catalog restart ownership, provider boundaries,
namespace-role forwarding, managed-successor evidence, independent semantic
vectors, public quarantine, privacy/call-graph gates, and module cohesion.

## Verdict

**NO-GO.**

There is no P0 or P1 finding. Three P2 interface defects remain, together with
one P3 literal-ownership issue. R2 and R4b-G must not proceed on this tuple.

These defects remain quarantined in the unreleased checked-artifact tree:
merge-v1 production dispatch remains disabled and no public-v0 behavior was
changed. They nevertheless block the R1 interface freeze because implementing
R2 would require caller convention, or alteration of a supposedly frozen
owner boundary, to preserve the accepted safety invariants.

The three P2 conclusions independently agree with the state/protocol `-3`
review. The catalog finding below additionally identifies mutation ordering at
the filesystem owner boundary as part of the same missing location/occupancy
contract.

## Findings

### [P2-1] Catalog evidence is location-interchangeable, and record creation can precede global occupancy classification

The catalog infrastructure provider observes one caller-selected directory.
Its raw observation includes the active marker, retained parent, fixed staging
name, directory identity, infrastructure identities, and optional stored
record, but it does not include an owner-issued staging/final role or a closed
observation of both fixed names
(`protocol/infrastructure_record/owner.rs:34-52`).

`CatalogInfrastructureOwnerV1::recover_or_create` validates that single
observation and returns the same cloneable generic
`BoundCatalogInfrastructureObservationV1` for either catalog role
(`owner.rs:71-152`). The recovery classifier then relies on the caller to place
that generic value in either the `staging` or `final_directory` argument
(`protocol/catalog_bootstrap_record.rs:255-335`). A directory actually observed
at the staging name can therefore be relabelled as final and, with staging
reported missing, select `RetireActive`; the reverse substitution can select a
publication path. The interface tests themselves can pass the same exact value
in either role (`interface_tests/durable_records.rs:501-575`), so the type
boundary does not prevent this substitution.

There is a second filesystem consequence of the same incomplete owner
boundary. When the single observed directory lacks its infrastructure record,
`recover_or_create` writes the record immediately
(`infrastructure_record/owner.rs:111-128`). At that point the owner has not
observed the other fixed catalog name and has not classified the aggregate
occupancy. Thus an ambiguous both-present or otherwise rejected global state
can be mutated before the recovery classifier rejects it. That contradicts the
accepted rule that unmatched catalog combinations fail without bootstrap or
private-domain mutation.

**Required correction:** make one catalog owner observe both fixed names,
their kinds and identities, and the applicable budget before returning a
closed occupancy/recovery result. Exact staging and final evidence must be
non-interchangeable, either through role-specific types or an opaque aggregate
observation. Perform any infrastructure-record creation only after that
owner-controlled classification has selected an allowed prepare/rewrite state.
Add negative tests for staging-as-final, final-as-staging, both-present,
substituted-name, and rejected-state zero-mutation cases.

### [P2-2] Physical namespace operations are disconnected from managed-successor evidence

The seven indexed role-specific namespace operations now exist and correctly
revalidate directory, provider, binding, source parent/path/leaf/kind, action,
and ordinal. The remaining gap is at their success boundary:

- `install_bootstrap_component` returns only
  `PublishedIdentity<DurableObjectIdentityV1>`
  (`namespace/operations.rs:126-150`); and
- `retire_bootstrap_marker` returns only
  `RetiredIdentity<DurableObjectIdentityV1>`
  (`namespace/operations.rs:152-169`).

The opaque evidence consumed by managed successor creation is issued by
separate `BackendIssuer` methods. Those methods accept the marker, object
identities, mode, and path as separately supplied facts and verify only the
provider binding (`namespace/backend.rs:329-377`). The production-shaped
compile fixture likewise separates physical forwarding from evidence issuance:
the full forwarding route installs and retires but does not obtain successor
evidence, while independent helper methods manufacture that evidence from
arguments (`namespace/provider_compile.rs:62-106,159-167`). The managed-record
tests bridge the gap with test helpers rather than through the successful
physical operation (`interface_tests/managed_records.rs:125-263`).

The transition records now compare all promised identities, modes, paths,
actions, and schedule data exactly. However, R2 has no frozen owner-controlled
route that proves those values were reobserved after the corresponding real
mutation. A caller can compose evidence facts independently of the operation
that supposedly established them.

**Required correction:** couple successful install and marker-retirement
operations to exact no-follow post-operation observation and return the opaque
`InstalledManagedComponentV1` or `RetiredManagedMarkerV1` evidence. If restart
requires a separate route, expose an owner-controlled recovery observation
bound to the exact prior intent, namespace slots, and provider. Do not expose a
consumer seam that accepts free-form physical facts. Add tests proving that
evidence cannot be issued before the matching operation, from substituted
state, or from a different provider/root, and that valid restart recovery does
not widen the raw backend boundary.

### [P2-3] Authority request hashes can still be combined with an unrelated retained observation

The private authority provider now seals the artifact root, retained parent
identity, and source path (`protocol/authority_record/owner.rs:9-35`). But the
consumer-facing owner method still accepts `expected_sha256` and `goal_sha256`
as bare arguments and combines them with whichever retained observation the
provider returns (`owner.rs:52-66`). The resulting opaque observation merely
records that already-composed tuple; later recovery can prove equality with it
but cannot prove that the hashes came from the same request transaction
(`protocol/authority_record.rs:57-85,146-160`).

A consumer can therefore bind retained artifact A to expected/goal hashes from
request B. Cross-root, cross-parent, and cross-source substitution are closed,
but the cross-request composition prohibited by RemPlan section 10.2 remains
available.

**Required correction:** make the private provider obtain the expected and
goal hashes from the same sealed retained/request observation, or require an
opaque request capability whose owner binding is checked against the
reservation. The issuance seam must not accept bare request hashes. Add a
negative test using two requests with the same profile and otherwise-valid
reservation to prove that retained facts from one cannot be combined with
hashes from the other.

## P3 observation

### [P3-1] Catalog fixed-name ownership remains duplicated

The remediation now derives cleanup counts and the previously duplicated
admission/slot values from their owners. Catalog names remain hard-coded in
`protocol/catalog_bootstrap_record.rs:17-18`, repeated as full private-domain
paths in `capability/collision.rs:128-133`, and the final artifact-root form is
also repeated in `policy.rs:33-34`. A future edit can therefore make collision
admission, bootstrap recovery, and policy disagree. Define one closed
catalog-name/private-domain grammar owner and derive all three uses from it.

This P3 does not independently block R1.

## Prior-finding closure

- **Pre-catalog issuance and revalidation:** closed. The raw provider is behind
  an owner-private module; the owner performs observation, issues the permit,
  revalidates it, and immediately invokes bootstrap with a lifetime-bound
  revalidated value. The consumer cannot obtain or retain that value, and a
  failed revalidation cannot call bootstrap.
- **Authority observation ownership:** partly closed. Artifact root, retained
  parent, and source are now one provider-issued fact set. P2-3 is the remaining
  independently supplied request-hash seam.
- **Catalog circular adoption:** partly closed. Active-token, bootstrap-record,
  retained-parent, profile, physical-identity, and stored-record comparisons
  are now exact. P2-1 is the remaining location/occupancy and mutation-order
  defect.
- **Indexed namespace roles:** closed. All seven required operation classes are
  represented and forward through the private backend after the required
  structural checks.
- **Exact managed-successor comparison:** closed at the protocol transition
  layer. P2-2 is the distinct inability to obtain that exact evidence from the
  real namespace operation/recovery seam.
- **Independent semantic vectors:** closed. The committed hand-authored
  26-vector fixture is not regenerated; every vector bounded-decodes and
  canonical re-encodes byte-for-byte, and the set covers every record family,
  closed variant, and both maximum schedule layouts.
- **Literal ownership:** partly closed, with the catalog-name remainder stated
  in P3-1.

The previously accepted plan/reservation binding, admission sealing, closed
grammar, fault vocabulary, bounded canonical record readers, unknown-field
rejection, provider quarantine, and occupancy model remain closed.

## Public quarantine, privacy, and cohesion

No raw checked-artifact backend or provider became public. Production record
reads remain bounded before semantic parsing and require canonical
re-encoding. The new semantic fixture is compiled into tests and is independent
of protocol regeneration.

No change was found in the public protocol/generated/convert surfaces or the
production merge dispatcher/gate relative to the previously reviewed
`gwz-core` baseline. The production decoder still rejects merge-v1 before body
decode, so the R1 work remains quarantined from released behavior.

The newly introduced production modules remain cohesive and below 500 lines,
apart from generated code. No new sub-500-line dumping-ground module or new
file at the 1,000-line split threshold was found. Existing larger files were
not materially enlarged by this checkpoint.

## Verification

Run against the reviewed tuple:

- `cargo test -p gwz-core checked_artifact::interface_tests`: passed; 72 tests.
- `cargo test -p gwz-core --test protocol`: passed; 29 tests.
- `cargo test -p gwz-core production_decoder_rejects_v1_before_body_decode`:
  passed.
- `cargo fmt --all -- --check`: passed.
- `cargo clippy -p gwz-core --all-targets -- -D warnings`: passed.
- `python protocol/regen.py --check`: passed with `taut-proto` 0.8.1.
- The public protocol and merge-dispatcher diff from the prior reviewed
  `gwz-core` tuple was empty.

## Acceptance condition

Remediate P2-1 through P2-3 with owner-bound positive and substitution/failure
tests, then submit one new exact settled tuple for fresh independent filesystem
and state/protocol re-review. R2 and R4b-G remain blocked until both reviews
report no P0/P1/P2 finding.
