# R4b-TI transition implementation final re-review

Date: 2026-08-10

Baseline: `gwz-core` `cec3d5bf800b44e6db85a51edb3ede057b330851`

Reviewed scope: the settled uncommitted R4b-TI implementation, the amended
`GwzM5-8R4bTransitionDesign.md`, and the R4b-TI row in
`GwzM5-8ChangeBudget.md`. This final re-review checked the dirty workspace
bytes directly after the prior state/interface findings were corrected. No
production or test code was changed by the reviewer.

## Verdict

**GO.** No P0, P1, P2, or P3 finding remains. R4b-TI satisfies its frozen
state/interface contract, owner-size rule, numeric change budget, production-
disabled v1 boundary, and required quality gates. R4b-TR may begin in the
ledger's dependency order and remains bound to the accepted TI interfaces.

The implementation now supplies one closed typed transition/effect vocabulary,
sealed and record-bound authority proofs, exact preservation and rollback
cursors, and executable exact-effect verification without introducing a v1
writer or changing released v0 wire behavior.

## Findings

### P0

None.

### P1

None.

### P2

None.

### P3

None.

## Accepted state/interface

The final review verified all previously open and corrected boundaries:

- `RollbackCursor` is one shared pure type exported by the v1 validator. Its
  `NoMutationParticipant` form carries the exact reverse-order `member_id`, and
  pending rollback validation and authority observation consume the same
  derivation.
- production `authority::observe::no_mutation_abort` issues an opaque proof
  only for that exact cursor member. `rollback_exhausted` issues for pure
  `Complete` or for `SelectedRootMetadata` only after reading both live
  workspace-manifest and lock bytes and matching them exactly to the operation
  baseline. Independent manifest-only and lock-only corruption assertions
  reject.
- proof bindings cover source digest, workspace, merge, operation, owner,
  action, phase, and a canonical domain-framed payload hash. Payload tampering
  and stale-record reuse reject.
- `PreparedRollbackEntry::FromPreserving` structurally carries a same-record
  `VerifiedPreservationExhausted`; direct entries reject the exhaustion member,
  and the private representation cannot be relabeled by a caller.
- every preservation intent/result embeds `VerifiedPreservationCursorPrefix`.
  Both sides carry a typed `PreservationCursorPosition` covering backup, exact
  stash phase, or exact reset phase, and matching requires identical owner and
  position.
- transition effects use a closed typed subject enum for operation,
  participant, failure batch, preservation owner, participant drift kind, and
  operation drift kind. Drift retirement carries the protocol enum identity,
  not a caller-supplied string.
- known semantic verification requires exact set equality after treating
  `WriterVersion` as the one implicit transition write, and the public verifier
  separately requires the resulting value to equal `crate::VERSION`.
  Immutable record, baseline, participant-identity, selected-target, extension,
  and arbitrary writer-version changes reject.
- resolution failure requires primary error, operation state, and exact later
  outcomes without allowing mutation of the authoritative primary conflict
  outcome.
- halted outcome compounds conditionally include participant error only for an
  existing-error-to-none clear; adding or replacing an error rejects.
- root publication-prefix changes are required only when the new stash/reset
  action carries a prefix and only for `PublicationRoot` or selected `@root`.
  A non-root owner cannot use the publication-preservation footprint.
- backup completion requires both pending-journal retirement and the exact
  owner's preservation evidence. Advancing stash from `CreateStash` requires
  the same evidence field. `FinishStash` owns only pending-journal retirement
  and rejects an evidence-row mutation.
- all 53 transition variants have compiler-exhaustive `effect_kind()` mapping
  through their eight typed sub-enums. The former count-only relationship is no
  longer the mapping authority.
- the Rust source tree contains no `v0_common_view` symbol and no owned v1 to
  `MergeOperationRecordV0` conversion.
- the pure v1 candidate/publication semantic helper lives in the existing
  `acceptance/publication.rs` policy owner. Its v1 imports, functions, and
  exports remain `#[cfg(test)]`, preserving the production-disabled v1
  boundary.

## Budget measurement

The measurement follows the ledger convention: production-bearing Rust
excludes `tests/` paths and `*_tests.rs`; test/tool/doc includes the focused
Rust tests and the five dirty R4b documents.

| Measure | Current | Ceiling | Result |
| --- | ---: | ---: | --- |
| Net production-bearing Rust | 2,096 | 2,100 | within by 4 |
| Moved production Rust | at most 107 deleted lines | 150 | within |
| Production paths | 22 | 22 | at ceiling |
| Net test/tool/doc LOC | 834 | 1,100 | within by 266 |
| Test/tool/doc paths | 13 | 14 | within |

The production total is 1,812 lines in seven new production files plus 284
net lines in fifteen existing production files. Focused/core test Rust is 529
net lines across eight paths; the remaining allowance is used by the five
dirty R4b documents.

Every affected responsibility owner is below the 500-line review trigger:

| Owner | LOC |
| --- | ---: |
| `transition/footprint.rs` | 496 |
| `authority.rs` | 479 |
| `model/v1/validate/acceptance.rs` | 464 |
| `acceptance/publication.rs` | 302 |
| `transition/effect.rs` | 288 |
| `transition/mod.rs` | 267 |
| `authority/binding.rs` | 163 |
| `checked.rs` | 109 |
| `v1_lifecycle/mod.rs` | 10 |

The former 523-line acceptance-validator P2 is closed by moving its cohesive
pure candidate/publication semantics into the existing publication-policy
owner. The final forwarding surface is a scoped re-export in the already
touched validator owner, so the package remains at 22 production paths without
dense line packing or a new policy owner.

## Verification

The reviewed dirty bytes pass:

- 12 focused R4b-TI lifecycle tests;
- all 40 v1 validator tests;
- the complete `gwz-core` suite: 814 passed and 1 ignored library test, plus
  integration groups of 10, 27, 4, and 2 passing tests;
- `cargo fmt --all -- --check`;
- `cargo clippy --all-targets --all-features -- -D warnings`;
- `git diff --check`; and
- the protocol integration group, confirming no protocol-schema drift.

## Closeout

R4b-TI is accepted and may close. Dependent work may consume these interfaces
but may not widen authority constructors, add a transition/effect variant,
introduce a raw v1 writer, or change the phase graph without the lead-owned
interface checkpoint and independent review required by the ledger.

## R4b-TR state-machine/matrix review

**NO-GO.** P0: none; P1: two; P2: one; P3: none.
- **P1 — resolver authority inversion:** `BoundExactObservation::Completed` carries
  an arbitrary `V1Transition` outside the bound payload; `resolve_observation`
  ignores its request and forwards that transition, so request/owner matching,
  compounds, and between-action ambiguity rejection are not closed by §7.
- **P1 — required matrix absent:** 28 focused tests pass, but no exhaustive
  request/state/pending-owner × observation/attempt matrix exists; illegal-
  predecessor, literal-origin, prefixed-preservation, and footprint gaps remain.
- **P2 — budget:** +2,163 production LOC fits 2,200, but 11 production paths
  exceed 10; test/tool/doc is 2,190/2,200 LOC across 9/12 paths.

## R4b-TR state-machine/matrix re-review — 2026-08-10

**NO-GO.** P0: none; P1: four; P2: two; P3: none. R4b-S/A remain blocked.
- **P1 — halted completion:** Halted+Continue always selects the resume compound;
  when another halt cause remains, the required ordinary outcome is rejected.
- **P1 — request/phase ordering:** Finalizing Abort/Preserve bypasses a live but
  unrecorded evidence result, and exhausted Preserving rejects its retained Preserve
  request instead of entering FromPreserving rollback.
- **P1 — exact attempt replay:** Completed/Ambiguous observations omit the physical
  action key, so a same-owner attempt for another exact action is accepted.
- **P1 — matrices remain incomplete:** the 35 passing focused tests do not exhaust
  request/state/owner/phase/observation, illegal predecessors, per-variant footprints,
  or cross-owner and reset-prefix cursors as required by §13.
- **P2 — owner mismatch:** PreparationFailed does not match its batch member to the
  requested participant, allowing a same-record cross-participant failure rewrite.
- **P2 — quality gate:** strict all-target/all-feature Clippy fails on two large enums.
- Verified: literal origins, Halt-before-recovery, abandonment compounds, named phase
  successors, format, and 837/837 non-ignored core tests. Budget is 2,489/2,550
  production LOC on 12/12 paths; Rust tests are 3,146 LOC before review memos.

## R4b-TR final state-machine/matrix re-review — 2026-08-10

**GO.** P0: none; P1: none; P2: none; P3: none. R4b-TR may close and
R4b-S/A may proceed in the ledger's dependency order.

- Halted aggregate completion, evidence-before-entry ordering, request-independent
  FromPreserving exhaustion, preparation-owner equality, request-bound abandonment,
  literal recovery origins, and exact post-attempt action equality are closed.
- The executable request/state, operation-predecessor, owner/attempt, publication-
  action, rollback, no-prefix/prefixed stash, and reset matrices cover the frozen TR
  authority; all 53 reducer/effect variants retain exact typed predecessors and fields.
- Live publication prefix/index classification, cross-owner filesystem observation,
  and runtime fault/restart matrices consume these opaque interfaces in R4b-F/P/G;
  they do not expose a missing TR transition, proof, owner, or physical action.
- Focused lifecycle tests pass 43/43; full core passes 845 with one ignored; format,
  strict all-target/all-feature Clippy, diff hygiene, and forbidden-writer scans pass.
- Final accounting is 2,721/2,750 production LOC on 12/12 paths and 3,995 focused
  test LOC before review memos within 4,400; test/tool/doc paths are 16/16, and every
  responsibility owner remains below 500 lines (maximum 499).

## R4b-TR settled-byte evidence re-review — 2026-08-10

**GO.** P0: none; P1: none; P2: none; P3: none. The evidence-only remediation
introduces no regression to the accepted TR state machine or authority boundary.

- Successful `prepare` calls are captured only after reducer application and exact
  known-diff verification; the observed set equals all 53 declared effect variants.
- Ambiguity overrides both Success and Failed attempts for ordinary participant,
  ResolveConflict, publication, preservation, and rollback owners; exact action and
  cursor bindings remain mandatory.
- All 30 wrong recovery-origin cells reject. Prefixed and no-prefix stash/reset plus
  rollback and publication matrices accept only their exact successor/action phases.
- The sole production increment is a two-line `#[cfg(test)]` capture hook; a normal
  library check and forbidden-writer scan confirm production runtime isolation.
- Focused lifecycle tests pass 46/46; full core passes 848 with one ignored; protocol
  passes 27/27; format, strict Clippy, check, and diff hygiene pass.
- Accounting passes at 2,723/2,750 production and 4,277/4,400 focused-test LOC,
  with 12/12 and 16/16 paths respectively; every owner remains below 500 (max 499).

Correction: focused-test accounting is 4,277/4,400, not 4,276/4,400.
**GO remains unchanged:** P0: none; P1: none; P2: none; P3: none.
