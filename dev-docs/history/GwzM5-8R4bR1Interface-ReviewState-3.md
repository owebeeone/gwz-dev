# R4b R1 checked-artifact state/protocol settled-tree re-review

Date: 2026-08-14

## Reviewed settled tuple

- workspace root: `c51cae969a6549a6986750447b8a648e444aa709`
- `gwz-core`: `a2a7125fe6b69f09ca76d83caeb28b0684c3c295`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

The three checked-out commits matched this tuple at review time, and the
workspace root and both member worktrees were clean before this review artifact
was written. This is a fresh, independent state/protocol review of the exact
settled tuple against:

- `GwzM5-8R4bR1InterfaceCheckpoint.md`;
- `GwzM5-8R4bR1Interface-RemPlan.md`;
- both original R1 filesystem/state reviews and their `-2` re-reviews; and
- the controlling R4b design and accepted amendments.

The review asks whether the remediated R1 interfaces are frozen, structurally
safe, and usable by R2 and R4b-G without widening shared authority, recovery,
or namespace seams.

## Verdict

**NO-GO.**

There is no P0 or P1 finding. Three P2 interface defects remain, plus one P3
literal-ownership observation. R2 and R4b-G must not proceed on this tuple.

The defects remain quarantined inside the unreleased checked-artifact
implementation. Production merge-v1 dispatch is still disabled and the public
wire/dispatcher paths remain unchanged, so these are not released regressions.
They do, however, prevent R1 acceptance: an R2 implementation would have to
trust caller convention or redesign a supposedly frozen seam.

## Findings

### [P2-1] Exact catalog infrastructure evidence is not bound to its observed staging/final location

`RawCatalogInfrastructureObservationV1` carries active marker data, the
retained parent, the staging name, directory identity, infrastructure
identities, and the stored record, but it does not carry the actually observed
directory role or leaf (`protocol/infrastructure_record/owner.rs:34-43`). The
owner verifies those values and returns one cloneable generic
`BoundCatalogInfrastructureObservationV1` (`owner.rs:71-128,132-152`). It does
not inspect or bind whether that exact directory is currently the staging
entry or the final entry.

The recovery classifier then accepts the same generic bound value in either
the caller-constructed `staging` or `final_directory` position
(`protocol/catalog_bootstrap_record.rs:254-280`). Consequently, an exact
directory still present at the staging name can be supplied as
`final_directory = Exact(...)` with `staging = Missing`; the classifier returns
`RetireActive` (`catalog_bootstrap_record.rs:319-329`). This allows the durable
active record to be retired without proving that staging-to-final publication
occurred. The reverse relabelling can incorrectly select `PublishFinal`.

This is narrower than the previous circular-ownership finding: the nonzero
active token, record ID, physical identity tuple, profile, retained parent, and
stored infrastructure record are now checked. What is still missing is an
unforgeable location classification, so the accepted exact restart order is
not structural.

**Required correction:** have the owner observe both fixed names and return a
closed occupancy/recovery observation, or return non-interchangeable
location-bound exact staging and exact final capabilities. The classifier must
consume that owner-issued location result rather than caller-arranged generic
facts. Add negative tests proving that staging evidence cannot drive
`RetireActive`, final evidence cannot drive `PublishFinal`, and both-present or
substituted-name states are ambiguous.

### [P2-2] Namespace installation and marker retirement do not return the evidence required by managed successors

The accepted checkpoint says successful component installation and marker
retirement produce opaque provider-issued evidence that is the only input to
managed successor creation. The sole consumer-facing namespace operations do
not do that:

- `install_bootstrap_component` returns only
  `PublishedIdentity<DurableObjectIdentityV1>`
  (`namespace/operations.rs:126-150`); and
- `retire_bootstrap_marker` returns only
  `RetiredIdentity<DurableObjectIdentityV1>`
  (`namespace/operations.rs:152-169`).

Evidence construction is a separate backend-issuer operation that accepts the
marker, object identities, mode, and path as independent arguments and checks
only provider binding (`namespace/backend.rs:329-377`). The production-shaped
compile fixture likewise exposes separate `issue_installed_component` and
`issue_retired_marker` helpers (`namespace/provider_compile.rs:62-106`), while
its complete forwarding path calls installation and retirement without
obtaining evidence (`provider_compile.rs:159-167`). `ActionNamespace` owns the
backend and exposes no consumer operation that recovers the required evidence.

The tests hide the interface gap by executing neither operation in their
successor helpers: they create a separate issuer and directly synthesize the
evidence from supplied values
(`interface_tests/managed_records.rs:125-194,197-263`). Thus the successor
comparisons are now exact, but an R2 consumer cannot reach those successors
through the frozen namespace seam after a real install or retirement.

**Required correction:** make the owner-controlled namespace operation couple
the successful physical mutation to exact no-follow reobservation and return
`InstalledManagedComponentV1` or `RetiredManagedMarkerV1`. If restart requires
a separate observation, expose an owner-controlled recovery operation bound to
the exact slots and prior intent; do not accept free-form physical facts from
the consumer. Add tests proving evidence cannot be issued before the matching
operation or from substituted post-operation state, and that restart can
recover exact evidence without widening the seam.

### [P2-3] Authority expected/goal hashes are still composable with an unrelated retained observation

The authority provider now owns the retained root/path/source observation, but
its sealed fact set contains only `artifact_root`, `retained_parent_identity`,
and `source` (`protocol/authority_record/owner.rs:9-29`). The consumer-facing
`observe` method still accepts bare `expected_sha256` and `goal_sha256`
arguments and combines them with whatever retained facts the provider returns
(`owner.rs:52-66`). The resulting opaque object copies those values into the
request/reservation binding (`protocol/authority_record.rs:57-85`), and
recovery can only prove equality with that already-composed object.

A consumer can therefore bind retained artifact A to expected/goal values from
request B. This leaves the cross-request composition explicitly prohibited by
RemPlan section 10.2 available, even though cross-root, cross-parent, and
cross-source composition are now sealed.

**Required correction:** the private authority provider must obtain the
expected and goal hashes from the same sealed retained observation/request
transaction, or consume an opaque request capability that derives and proves
them against the reservation's request-owner binding. The consumer-facing
issuance seam must not accept bare request hashes. Add substitution tests in
which path/source facts from one request cannot be combined with hashes from
another request sharing the same profile and otherwise-valid reservation.

## P3 observation

### [P3-1] Catalog fixed-name ownership is still duplicated

The cleanup count and admission/slot literals from the prior observation are
now derived from their owners. Catalog names remain separately hard-coded in
`protocol/catalog_bootstrap_record.rs:17-18` and as full private-domain paths
in `capability/collision.rs:128-133`; the final artifact root is also repeated
in `policy.rs:33-34`. A change to one owner can therefore make collision
admission disagree with bootstrap or policy. Define one closed catalog-name or
private-domain grammar owner and derive all three uses from it.

This P3 does not independently block R1, but it means the prior P3 is only
partly closed.

## Closure of the second-review findings

- **State `-2` P2-1 / filesystem `-2` P1-1, pre-catalog authority and
  revalidation:** closed. The raw provider is owner-private; permit creation,
  retained-root revalidation, and the lifetime-bound bootstrap call are
  adjacent in one owner operation. Rejected revalidation cannot call
  bootstrap.
- **State `-2` P2-2, authority observation:** partly closed. Retained
  root/path/source facts are owner-issued and record recovery binds their exact
  opaque observation. Current P2-3 records the remaining independently
  supplied expected/goal request facts.
- **State `-2` P2-3, catalog ownership:** partly closed. Active-token,
  bootstrap-record, parent, profile, physical-identity, and stored-record
  checks remove circular adoption. Current P2-1 records the remaining ability
  to relabel generic exact evidence as staging or final.
- **State `-2` P2-4 / filesystem `-2` P1-2, role forwarding:** closed for all
  seven indexed operations and their parent/leaf/kind/provider/action/ordinal
  checks. Current P2-2 is the distinct missing result/recovery-evidence seam.
- **Filesystem `-2` P1-3, exact managed successor comparisons:** closed at the
  protocol transition layer. Exact marker, parent identity, mode, path,
  component, action, schedule, and reservation substitutions are rejected.
  Current P2-2 concerns how a real operation can obtain that exact evidence.
- **State `-2` P2-5 / filesystem `-2` P3-1, independent semantic vectors:**
  closed. The committed, hand-authored 26-vector fixture is outside
  regeneration, bounded-decodes, canonical re-encodes byte-for-byte, covers
  every record family and closed variant, and includes both maximum schedule
  layouts.
- **State `-2` P3-1, literal ownership:** partly closed as described by current
  P3-1.

The previously accepted managed-plan/reservation binding, admission sealing,
closed grammar, exhaustive fault set, bounded canonical readers, unknown-field
rejection, and occupancy model remain closed.

## Public behavior, bounds, and wire quarantine

All production record reads remain bounded before semantic parse and require
canonical re-encoding. The semantic vectors exercise the complete current
record family, identity variants, modes, root kinds, aliases, phases, purposes,
and maximum schedules. No unbounded checked-artifact production decoder or
new public-v0 decoder path was found.

The public protocol/generated/convert files and merge dispatcher/gate are
unchanged from the prior reviewed `gwz-core` tuple. The production decoder
continues to reject merge-v1 before body decode. The R1 work therefore remains
quarantined from released public behavior.

## Verification

Verification was run against the exact tuple above:

- `cargo test -p gwz-core`: passed; 1,247 tests passed and one was ignored
  (1,202 unit tests plus 45 integration tests), with zero failures.
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

Remediate P2-1 through P2-3 with structural negative tests, then submit one new
exact settled tuple for fresh independent filesystem and state/protocol
re-review. R2 and R4b-G remain blocked until both reviews report no P0/P1/P2
finding.
