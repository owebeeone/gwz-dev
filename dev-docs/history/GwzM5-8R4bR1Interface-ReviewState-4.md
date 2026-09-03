# R4b R1 checked-artifact state/protocol settled-tree re-review

Date: 2026-08-14

## Reviewed settled tuple

- workspace root: `7e4ccdf80c808367171b1df1a5210c89ac36ec68`
- `gwz-core`: `c239df33ddf567d4eaca182ca8231ed575e95769`
- `gwz-cli`: `3cca145c0b32410f250f640730ed7ca18f1da59f`

The three checked-out commits matched this tuple and the workspace root and
both member worktrees were clean when this review began. The subsequently
created sibling filesystem review is not part of the reviewed tuple and was
not used as evidence for this independent state/protocol determination.

This review re-read the checkpoint, the full remediation plan including
sections 14-16, both `-3` reports, and the controlling R4b design and
remediation documents. It then traced the corrected source and negative tests
rather than accepting the implementation summary or prior test claims.

## Verdict

**GO.**

No P0, P1, P2, or P3 finding remains in this review scope. The state/protocol
half of the exact-tree R1 acceptance gate is satisfied. R2 and R4b-G may
proceed once the independently required filesystem review of this same tuple
also reports no P0/P1/P2 defect.

Production merge-v1 dispatch remains disabled and the public protocol and
merge-dispatcher surfaces remain byte-unchanged from the preceding reviewed
core tuple, so this verdict does not activate the new implementation.

## Closure of the `-3` findings

### Aggregate catalog recovery and mutation order

The catalog provider now performs one owner-private observation of scratch,
active, staging, final, and retired roles. A candidate carries its observed
leaf, retained parent and path, directory identity, marker record ID and
ownership token, infrastructure identity tuple, and optional stored record.
The owner validates staging and final through different branches and can issue
only `ExactStagingInfrastructureV1` or `ExactFinalInfrastructureV1`; their
constructors are not available to consumers.

The complete five-role aggregate is classified before mutation. A missing
staging infrastructure record is written only after the aggregate has selected
`PrepareOrRewriteStaging`; the owner then performs a new aggregate observation
and classification rather than promoting the pre-write facts. Both-present,
substituted-name, final-partial, and rejected missing-record layouts are
covered by zero-mutation tests. The former generic-evidence relabelling and
write-before-aggregate defects are closed.

### Managed forward and restart evidence

`ActionNamespace` now owns four exact paths: forward installation, restart
installation observation, forward marker retirement, and restart retirement
observation. Each binds the provider, action, reservation, intent ID,
bootstrap/component ordinal, target parent identity/path, and scheduled names
before entering the backend. Private post-observation completion additionally
checks the expected ownership marker and the exact installed or retired
physical facts.

Only the resulting private observation can be converted to
`InstalledManagedComponentV1` or `RetiredManagedMarkerV1`. Before returning,
the wrapper proves that the evidence closes the supplied durable intent; the
forward install also proves identity preservation against its retained source.
The production-shaped provider fixture calls all four operations through the
same owner, while the managed tests consume those operations and exercise
provider, request, role, parent, path, mode, object-identity, marker, ordinal,
and restart substitution failures. The former disconnected physical-operation
and evidence seam is closed.

### Authority request transaction

The authority provider's one private fact set now owns the request-owner
binding, expected and goal hashes, artifact root, retained parent identity,
and retained source fingerprint. `CheckedAuthorityObservationOwnerV1::observe`
accepts only the resident reservation and rejects a request-owner mismatch
before issuing opaque authority observation. Two otherwise compatible
reservations are tested in both substitution directions. No consumer-facing
hash argument or raw observation constructor remains.

### Catalog name ownership

`CatalogPrivateNameV1` is the single owner of the final, scratch, active, and
staging catalog names, with `CatalogPrivateRootV1` owning the workspace versus
Git-directory prefix. The bootstrap record, collision domain, and both policy
paths derive from that grammar. A production-source literal search found the
fixed literals only in `catalog_names.rs`; the former P3 duplication is closed.

## Wider state/protocol audit

- Every durable record family still enters through a literal `limit + 1`
  reader before semantic decode and canonical re-encoding. Raw decoders remain
  inside their owner modules.
- The independent 26-vector fixture remains outside regeneration, covers all
  ten durable record kinds, all identity/profile/path variants and closed
  managed phases/purposes, every cleanup alias, and both maximum schedule
  layouts, and bounded-decodes and re-encodes exact literal bytes.
- Admission, reservation, managed-plan, intent/predecessor, role/ordinal,
  retained-root, path/mode, object-identity, marker, and cleanup bindings remain
  exact. No checked-artifact consumer escape hatch for the private backend or
  evidence constructors was found.
- Restart classification remains conservative: unlisted catalog aggregates
  are ambiguous, absent managed post-state cannot issue evidence, and managed
  successor creation requires exact provider-issued evidence.
- The public protocol/generated/conversion files and production merge
  dispatcher/gate have no diff from the preceding reviewed core checkpoint.
  The production decoder still rejects merge-v1 before body decode.
- No privacy widening or new public protocol projection was found. Checked
  state stays internal to `gwz-core`; no new sensitive path or retained-state
  material reaches the Rust or Python CLI surfaces.
- The corrected production modules remain conceptually separated into catalog
  observation/classification, managed request/observation, evidence,
  operations, backend, authority owner, and name grammar. No file approaches
  the 1,000-line split trigger. The 501-line managed bootstrap record is
  already split into codec, validation, and transition children and is not a
  new dumping ground, so no cohesion finding is warranted.

## Independent verification

Run against the exact tuple above:

- `cargo test -p gwz-core checked_artifact::interface_tests`: passed, 77 tests.
- `cargo test -p gwz-core --test protocol`: passed, 29 tests.
- `cargo test -p gwz-core production_decoder_rejects_v1_before_body_decode`:
  passed.
- `cargo fmt --all -- --check`: passed.
- `cargo clippy -p gwz-core --all-targets -- -D warnings`: passed.
- `python protocol/regen.py --check`: passed with `taut-proto` 0.8.1.
- The public protocol/generated/conversion and merge-dispatcher diff from
  `gwz-core` `a2a7125fe6b69f09ca76d83caeb28b0684c3c295` was empty.
- Catalog-name literal ownership and owner/backend privacy searches passed.

An independent full `cargo test -p gwz-core` invocation was also started. It
reported no failure through the ordinary suite and the long reverse
fault/restart matrices, but was stopped while two long-running matrix rows were
still pending so that this requested review could conclude; it is therefore
not counted as an independent full-suite pass. The focused R1 gates above are
complete, and the settled checkpoint separately records a completed 1,252-test
lead-owned full-suite run.

## Gate result

The state/protocol review has no remediation request. Accept this half of the
R1 exact-tree gate and combine it only with the independent filesystem verdict
for the same tuple. If that verdict is also free of P0/P1/P2 findings, R1 is
accepted and R2/R4b-G may resume without another state/protocol remediation
cycle.
