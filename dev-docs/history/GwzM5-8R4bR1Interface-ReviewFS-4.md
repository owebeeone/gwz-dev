# R4b R1 checked-artifact filesystem/interface exact-tree re-review

Date: 2026-08-14

## Verdict

**GO.** I found no P0, P1, P2, or P3 defect in the reviewed R1
filesystem/interface contracts. From this review's scope, the R1 interface gate
is satisfied and R2/R4b-G may proceed. Final progression remains conditional on
the other independent review required by the plan reaching the same result.

## Settled tuple reviewed

- workspace root: `7e4ccdf80c808367171b1df1a5210c89ac36ec68`
- `gwz-core`: `c239df33ddf567d4eaca182ca8231ed575e95769`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

`gwz status --json` showed this tuple clean before the review. The only file
written by this review is this report.

The review used the R1 interface checkpoint, its remediation plan, both `-3`
review reports, and the controlling R4b architecture, transition, and
remediation documents. It traced the implemented interfaces and their call
graphs rather than relying on checkpoint claims alone.

## Findings

None.

## Closure assessment

### Aggregate catalog recovery

The catalog owner now receives a single aggregate observation covering all five
roles: scratch, active, bootstrap staging, final, and retired. The observation
is role-bound and validated before classification. Staging and final evidence
use distinct types and distinct owner-controlled names; they cannot be
substituted merely because their record payloads look alike.

Recovery classifies the complete observed state before mutation. The sole
permitted repair is the explicitly classified missing staging-record case. It
then re-observes and reclassifies the aggregate state. Ambiguous, substituted,
partial-final, and otherwise rejected combinations return without invoking the
write seam. The interface tests pin both the accepted re-observation path and
zero mutation for rejected aggregate states.

### Managed mutation and restart evidence

Forward installation, installation restart, retirement, and retirement restart
all bind their evidence request to the exact provider, action, reservation,
intent, bootstrap/component slots, schedule-derived names, target parent/rooted
canonical path, and physical observations. The private backend/provider is the
only issuer capable of completing these requests. Consumer code receives opaque
validated evidence and has no free constructor or consumer-supplied identity,
path, mode, marker, or outcome seam.

The installed directory's own mode and the final canonical path component's
parent lookup mode are deliberately separate observations. They need not be
equal: the former describes the installed child and the latter the directory in
which its leaf is resolved. Both are independently carried and checked in their
appropriate transition/restart roles. This is not an evidence substitution
gap.

### Authority binding

The authority fact set includes the request-owner binding together with the
artifact-root, retained-parent, source, expected, and goal hashes. Observation,
issue, and recovery reject a mismatched request binding; callers cannot supply
or replace these hashes after observation. Cross-request substitution is pinned
by the durable-record tests.

### Catalog name ownership

`catalog_names.rs` is the single owner for the checked-artifact catalog private
root and role names. Infrastructure recovery, collision checks, and policy path
derivation consume that owner. No second production literal table or relabeling
adapter exists in the reviewed R1 implementation.

### Privacy, boundedness, and quarantine

- Raw authority, catalog, namespace-backend, and managed-observation seams are
  private to their owning implementation boundaries.
- Evidence completion is reachable from the owner/provider path and test
  support, not from production consumers.
- Durable decoding applies the byte bound before decode and verifies canonical
  re-encoding. Catalog observations carry the infrastructure-record budget to
  the raw provider boundary.
- The production v1 decoder rejection remains in place and passed its focused
  test.
- The R1 changes do not modify the public GWZ protocol schema, conversion layer,
  merge implementation, or CBOR compatibility surface covered by the
  quarantine rule.
- Semantic fixtures remain independent of the regeneration path, and committed
  protocol artifacts pass the regeneration check.

### Cohesion and file size

No new god file or concept-dumping module was introduced. The largest touched
cohesive record module is approximately 500 lines, far below the roughly
1,000-line split trigger, and codec, validation, transition, owner, operation,
and backend responsibilities remain separated. There is no architectural reason
to split one of these modules merely to satisfy a mechanical line target.

## Verification

The following checks passed on the settled tuple:

- `cargo test -p gwz-core checked_artifact::interface_tests -q` — 77 passed
- `cargo test -p gwz-core --test protocol -q` — 29 passed
- `cargo test -p gwz-core production_decoder_rejects_v1_before_body_decode -q`
  — 1 passed
- `cargo fmt --all -- --check`
- `cargo clippy -p gwz-core --all-targets -- -D warnings`
- `python protocol/regen.py --check`

## Gate decision

The four `-3` review failures are closed:

1. aggregate five-role catalog recovery observes and classifies before any
   permitted write, while rejection is zero-mutation;
2. staging and final evidence are non-interchangeable;
3. managed forward/restart evidence is issued through the exact owner-bound
   provider request with no free consumer seam;
4. authority is request-bound and catalog names have one owner.

There is therefore no filesystem/interface P0/P1/P2 blocker to R2 or R4b-G.
