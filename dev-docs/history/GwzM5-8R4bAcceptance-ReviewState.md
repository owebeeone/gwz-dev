# GWZ M5-8 R4b-A Acceptance Interface Review (State)

Date: 2026-08-10

Status: **GO; no P0-P3 finding**

## Scope

This was an independent, read-only review of the current uncommitted R4b-A
acceptance checkpoint. The implementation was not modified. The review used
the accepted I1 direction, the I2 record/acceptance contract, the accepted R4a
seams, and `GwzM5-8R4bTransitionDesign.md` sections 6.3-6.4 and 10-13 as the
normative basis.

The reviewed implementation surface was:

- `gwz-core/src/workspace_ops/merge/acceptance/v1.rs` and `v1/support.rs`;
- the R3 open-v0 adapter delegation in
  `record_wire/open_v0/adapter.rs`;
- the v1 acceptance validator and publication decision seam;
- the acceptance/store/adapter compatibility tests; and
- the production-disabled module and call-graph boundary.

## Findings

No P0, P1, P2, or P3 finding remains. R4b-F and R4b-X may depend on this
interface, subject to their own required reviews.

## Contract review

### Shared builder and version isolation

- `build_v1_acceptance` is the sole R4b-A complete-acceptance builder. Its
  `V1AcceptanceRecord` is a narrow borrowed view over v0 or v1, not an owned
  projection.
- The R3 open-v0 adapter calls that builder directly and has deleted its former
  local acceptance/audit implementation. The v1 test seam calls the same
  builder through `V1AcceptanceRecord::V1`.
- The Rust source contains no `v0_common_view`, owned v1-to-v0 conversion, or
  v1 call into a v0 serializer/mutator. The adapter constructs v1 directly.
- The adapter checks the builder's publication result against R4a's released
  `publication_required` decision. The seven registered compatibility cases
  also retain R4a next-action parity; persisted candidates keep lock YAML
  byte-for-byte.
- `acceptance::v1`, `model::v1`, `record_wire::open_v0`, and the v1 lifecycle
  remain behind `#[cfg(test)]`. A normal library build therefore contains no
  v1 writer, upgrader, or lifecycle dispatch.

### Complete accepted workspace

- The audit domain is exactly metadata-base lock members, active
  metadata-base manifest members, and selected non-root members. Selected
  rows are explicit `Selected`, unchanged metadata-base rows are
  `UnselectedPresent`, and active manifest-only rows are explicit `Absent`.
  Accepted-lock keys consequently equal the selected/unselected-present audit
  keys.
- Selected member integration and final checkout are distinct, attached to
  the exact target branch/result, and the selected complete lock row freezes
  result commit, branch, `detached: false`, `dirty: false`, and
  `materialized: true`.
- For selected-root metadata, selected member identity is anchored first in
  the operation baseline manifest or lock. Result manifest/lock identities,
  when present, must match it. A member removed by the selected-root result is
  therefore reconstructed from the frozen baseline identity rather than
  dropped or invented.
- Operation-baseline and metadata-base bytes remain separate. Baseline bytes
  are digest-checked before use. Selected-root metadata preserves the supplied
  exact result manifest/lock bytes and records their digests and exact result
  commit. Accepted candidate lock bytes are reused exactly when present.
- Without a persisted candidate, `render_complete_lock` edits only the nine
  known selected-row fields in the raw metadata/base YAML. Unknown member
  descendants remain at their approved destination and are duplicated in the
  flattened audit row. The rendered bytes are parsed back and required to
  equal the completed typed lock before acceptance is returned.
- The independent v1 validator recomputes baseline and metadata digests,
  metadata source, workspace identity, exact audit domain, selected and
  unselected row equality including extensions, lock membership, root input,
  and candidate byte/digest agreement after every decode.

### Root and publication ordering

- Selected `@root` produces only `BornAttached` at its exact verified result
  and branch and requires both committed baseline artifact hashes.
- An unselected root preserves exactly `BornAttached`, `BornDetached`, or
  `UnbornAttached` from the baseline. Attached variants carry their exact
  symbolic publication branch; detached carries none. Detached acceptance is
  rejected whenever publication is required.
- The acceptance reducer permits the sole freeze only in `Finalizing`, after
  all selected outcomes are successful, with no acceptance, publication, or
  pending owner already present. The publication reducer independently
  requires persisted acceptance before either required or no-publication
  classification.
- Later transition footprints treat acceptance as immutable. Store tests also
  prove the first freeze preserves derived lock-member extensions.
- `classify_frozen_v1_publication` starts from persisted
  `accepted_workspace`: member changes come from frozen selected audit
  integration, and selected-root change is cross-checked against the durable
  selected-root source/result and baseline participant evidence. It performs
  no live repository read. The pre-freeze builder decision is consumed only by
  the R3 adapter's R4a parity guard; it is not the v1 lifecycle classification
  authority.

## Budget and ownership

The R4b-A accounting matches the accepted ledger:

| Measure | Current | Ceiling | Result |
| --- | ---: | ---: | --- |
| Net production-bearing Rust | +620 | 650 | within by 30 |
| Moved/de-duplicated production Rust | at most 118 | 400 | within |
| Focused Rust test LOC | 202 | 900 | within |
| Production paths | 4 | 10 | within |
| Test/tool/doc paths, including this review | 5 | 8 | within |

The production total is 730 new lines in the two builder owners, minus 118
net lines from the duplicated adapter builder, plus eight net module-wiring
lines. The relevant responsibility owners remain below 500 lines:

| Owner | LOC |
| --- | ---: |
| `acceptance/v1.rs` | 498 |
| `model/v1/validate/acceptance.rs` | 464 |
| `acceptance/publication.rs` | 302 |
| `record_wire/open_v0/adapter.rs` | 242 |
| `acceptance/workspace.rs` | 235 |
| `acceptance/v1/support.rs` | 232 |
| `v1_lifecycle/tests/acceptance.rs` | 202 |

## Verification

The latest reviewed tree passed:

- focused R4b-A acceptance tests: 4/4;
- focused v1 acceptance-validator tests: 7/7;
- G23 compatibility/recovery suite, including all seven adapted-v0 registry
  cases: 107/107;
- full `gwz-core`: 862 passed, 1 ignored, plus integration groups 10/10,
  27/27, 4/4, and 2/2;
- production-only `cargo check -p gwz-core --lib`;
- strict all-target/all-feature Clippy with `-D warnings`;
- `cargo fmt --all -- --check` and `git diff --check`;
- merge-document consistency; and
- compatibility predicate validation: seven migration rules and seven runtime
  bindings.

## Verdict

**GO.** The checkpoint supplies one shared acceptance builder, preserves the
closed I1/I2 v1 meanings and exact evidence, classifies only after the durable
freeze on the v1 transition path, remains byte/semantically compatible with
the R3 adapter domain, stays production-disabled, and is within every R4b-A
budget and ownership ceiling.
