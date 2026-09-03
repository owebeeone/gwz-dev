# R4b R1 checked-artifact state/protocol interface re-review

Date: 2026-08-14

## Reviewed settled tuple

- workspace root: `07d3bc5ed9ce746fbedbe25ff48b217968ec6bf3`
- `gwz-core`: `1bc07c4b35d6d158aa8daf25f924acf438f20698`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

The three checked-out commits matched this tuple at review time, and the
workspace root and both member worktrees were clean. This is an exact-tuple,
settled-tree re-review of the state/protocol interface against:

- `GwzM5-8R4bR1Interface-ReviewState.md`;
- `GwzM5-8R4bR1Interface-RemPlan.md`; and
- the controlling R4b R1 interface checkpoint and accepted R4b design.

The review focuses on whether the remediated R1 boundary is safe to hand to
parallel R2 implementers. It does not treat the absence of later platform and
catalog implementations as a defect by itself.

## Verdict

**NO-GO.**

There is no P0 or P1 finding. Five P2 findings remain. The original
managed-plan, admission, grammar, bounded-decode, fault-set, and occupancy
findings are otherwise closed.

The remaining P2 findings are interface defects, not released regressions:
the checked-artifact implementation remains private and production merge-v1
dispatch remains disabled. They nevertheless prevent R1 acceptance because
R2 implementers would still have to rely on conventions or redesign a shared
authority, recovery, or namespace boundary.

## Findings

### [P2-1] Pre-catalog authority is still forgeable and immediate revalidation is not structural

`capability/pre_catalog.rs:157-211` exposes an unsealed
`PreCatalogPreflightV1` to every checked-artifact sibling; its default
`preflight` creates the opaque permit, so any sibling can implement the trait
and self-report mutually consistent facts. More directly, `bootstrap.rs:46-52`
gives `CatalogBootstrapV1::recover_or_create` only
`&PreCatalogPermitV1`; it receives neither provider nor root and therefore
cannot structurally call `revalidate` immediately before mutation as RemPlan
section 1 requires. The checkpoint test demonstrates the convention:
`interface_tests/capability_permit.rs:96-104` implements bootstrap without
revalidation, while lines 125-128 call revalidate separately before bootstrap.
A caller may omit it or race between calls.

**Recommended correction:** seal permit issuance behind the capability owner
and make the revalidation/mutation boundary one owner operation (or pass a
one-shot revalidated capability).

### [P2-2] Checked authority remains composable from unrelated facts

`protocol/authority_record.rs:31-51` exposes
`CheckedAuthorityRecordV1::new` to every checked-artifact sibling and accepts
arbitrary reservation, path, retained-parent identity, source fingerprint and
expected/goal hashes. `validate_profiles` at 96-105 checks only common support
profile; it does not prove path/parent/source belong to one retained
observation. `read_and_bind_authority_record` at 182-192 checks only
reservation equality. Thus a same-profile path from A, parent/source from B
and arbitrary goal form a canonical record and a bound value—contrary to
acceptance condition 1.

**Recommended correction:** issue the creation capability only from the owner
that possesses the sealed retained observation/request binding, not raw fact
parameters.

### [P2-3] First-catalog infrastructure ownership is circular, so recovery cannot prove `exact active-owned directory`

`CatalogBootstrapRecordV1` fields (`catalog_bootstrap_record.rs:23-36`) contain
no staging/final directory identity, ownership token, or expected
infrastructure digest. `InfrastructureRecordV1::from_catalog_bootstrap`
(`infrastructure_record.rs:33-56`) accepts an arbitrary post-hoc identity tuple
and binds only the public bootstrap record ID. The recovery classifier
(`catalog_bootstrap_record.rs:215-280`) accepts a caller-supplied
`expected_infrastructure`; after restart, the only durable source of that exact
identity tuple is the observed staging/final record itself. Deriving expected
from observed adopts a foreign/self-consistent insertion; not doing so makes
recovery impossible.

**Recommended correction:** add a non-circular active-record ownership token
or marker or a closed successor record that pins the created staging identity
before its contents can be adopted.

### [P2-4] `ActionNamespace` still does not own all required forwarding operations

`namespace/mod.rs:220-269` forwards only generic base-slot publication, cleanup
retirement, and barrier. It derives barrier/bootstrap
active/retired/scratch/staging/final/marker destinations
(`namespace/mod.rs:147-217`, `namespace/roles.rs:99-240`), but none can be
passed to `publish_no_replace` (accepts only `PublishDestination`) or
`retire_exact` (accepts only `CleanupRetirementDestination`), and there are no
wrapper methods for intent publish/retire, target-alias retirement, component
install, or marker retirement. `RawNamespaceBackend` likewise exposes only
generic publish/retire/barrier (`namespace/backend.rs:384-425`). R2 implementers
must redesign/widen this shared seam, violating RemPlan section 4 and the
interface-freeze purpose.

**Recommended correction:** add role-specific ActionNamespace operations
returning the existing sealed evidence.

### [P2-5] The required independent semantic vectors are absent

The only committed corpus is explicitly generated
(`protocol/checked_artifact-corpus/rust/vectors.rs:1-25`) from generic generator
values and is not referenced by `src/checked_artifact`; its embedded parity
test is therefore not compiled. Rust tests construct semantic values and
round-trip their self-produced bytes (`interface_tests/durable_records.rs:69-115`)
and test schedule counts (`schedule_records.rs:29-43`), but there is no
independently authored literal semantic-byte fixture covering closed variants
and both maximum layouts. This misses RemPlan section 2 and acceptance
condition 3.

## P3 observation

### [P3-1] Original literal-drift observation is only partly remediated

`protocol/cleanup.rs:188` still hard-codes `3` instead of
`MAX_CLEANUP_ROWS`; catalog bootstrap names at
`catalog_bootstrap_record.rs:17-20` and admission/anchor names at
`infrastructure_record.rs:15-17` duplicate the grammar/collision owners. Derive
from owner constants/enums.

## Closed findings

The following findings from `GwzM5-8R4bR1Interface-ReviewState.md` are closed
on this tuple, except where the new findings above identify a narrower residual
defect:

- **Original P2-1, managed plan not bound to admitted schedule:** closed. The
  managed-plan owner builds the immutable canonical plan, assigns its schedule,
  reproduces the resident reservation, and issues the bound plan only after
  exact comparison. Negative plan/schedule substitution cases are pinned.
- **Original P2-2, indirectly forgeable admitted action:** closed. Admission
  observation and issuance moved behind the owner; synthetic construction is
  test-only and privacy/call-graph gates prevent production siblings from
  manufacturing admission authority.
- **Original P2-3, raw namespace bypass and untyped slots:** closed as to raw
  backend access, binding types, schedule-derived role capabilities, and
  consumer construction. New P2-4 records the remaining incomplete forwarding
  surface that still prevents the seam from being frozen.
- **Original P2-4, incomplete durable protocol and grammar:** closed as to the
  Taut record family and closed `Valid | RecognizedInvalid | Foreign` grammar.
  New P2-3 records the distinct non-circular ownership defect in the completed
  catalog-bootstrap record family.
- **Original P2-5, optional bounds and reservation trust:** closed. Production
  recovery uses typed bounded reads and sealed binding before records can drive
  action; limit, limit-plus-one, canonical, unknown-field, reservation, slot,
  and identity cases are exercised.
- **Original P2-6, incomplete and self-derived fault vocabulary:** closed. The
  complete 161-key production set is compared with an independently authored
  expected set.
- **Original P3-1, occupancy conflated idle with Preparing-after-publication:**
  closed. Occupancy is modeled as `Idle`, `PreparingWithoutFinal`, or
  `PreparingWithFinal`, and neither Preparing state may admit a new action.
- **Original P3-2, repeated frozen literals:** partly closed. The remaining
  instances are recorded as P3-1 above.

## Public behavior and wire quarantine

Public protocol and merge-dispatcher paths are byte-identical to the previous
reviewed tuple. Production merge-v1 dispatch remains disabled, and
`production_decoder_rejects_v1_before_body_decode` passes. The findings above
therefore do not alter released public behavior or wire compatibility.

## Verification

Verification was run against the exact settled tuple above:

- `cargo test -p gwz-core`: passed; 1,235 tests passed and one was ignored
  (1,190 unit tests plus 45 integration tests).
- `cargo test -p gwz-core checked_artifact::interface_tests`: passed; 61 tests.
- `cargo test -p gwz-core --test protocol`: passed; 29 tests.
- `cargo clippy -p gwz-core --all-targets -- -D warnings`: passed.
- `cargo fmt --all -- --check`: passed.
- `python protocol/regen.py --check`: passed with `taut-proto` 0.8.1.
- `production_decoder_rejects_v1_before_body_decode`: passed.
- The public protocol and merge-dispatcher diff from the prior tuple was empty.

## Acceptance condition

Remediate P2-1 through P2-5, add their structural and negative gates, and
submit one new exact settled tuple for independent re-review. R1 remains
blocked, and R2/R4b-G must not begin, until the settled-tree reviews report no
P0/P1/P2 finding.
