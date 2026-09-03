# R4b-TI architecture and interface re-review

Date: 2026-08-09

Reviewed scope: the settled dirty R4b-TI bytes, the accepted
`GwzM5-8R4bTransitionDesign.md`, and the R4b ceilings in
`GwzM5-8ChangeBudget.md`. This review covers architecture, authority sealing,
effect exactness, version isolation, and budget compliance. It does not review
reducers or dispatcher behavior reserved for R4b-TR.

## Verdict

**GO.** The previous architecture blockers are closed. There are no open
P0, P1, P2, or P3 findings in the reviewed R4b-TI interface. R4b-TR may begin
after the second required independent TI review also records GO; this review
does not authorize later checkpoints or production v1 activation.

## Findings

| Severity | Result |
| --- | --- |
| P0 | None |
| P1 | None |
| P2 | None |
| P3 | None |

## Accepted boundaries

- The validator and authority consume one shared typed `RollbackCursor`. The
  authority-owned observer derives no-mutation participant proofs from that
  cursor rather than from a caller-selected owner.
- `VerifiedRollbackExhausted` now handles the selected-root terminal window
  required by design section 6.6: when the durable cursor identifies selected
  root metadata, the observer rereads both live manifest and lock bytes,
  requires their exact baseline values, and binds the baseline digests into
  the proof. Manifest-only and lock-only drift are independently rejected.
- Production issuance remains owned inside `authority`; lifecycle siblings can
  request the narrow observer classifications but cannot construct an issuer,
  binding, or arbitrary proof. Token wrappers are non-cloneable. Raw
  constructors and payload corruption seams are test-only.
- Proof payload hashing is fallible and canonical. The framed payload includes
  a fixed authority domain, explicit value-type tags, length-delimited values,
  recursive sequence/map framing, and deterministic sorted mapping entries.
  The carried value is therefore bound to the checked record, owner, action,
  phase, and canonical payload bytes.
- Compound preservation/rollback entries bind the publication handoff and
  anticipated model. The preserving rollback entry additionally requires the
  preservation-exhaustion proof; direct and preserving origins cannot be
  interchanged.
- Preservation intent carries a typed `PreservationCursorPosition` and embeds
  the matching owner/position prefix. Owner or position substitution fails
  proof matching.
- `EffectSubject` and `RetiredContainer` are closed typed enums. Participant,
  publication, preservation, and drift identities cannot be supplied through
  generic string paths, and retirement is derived from the effect variant.
- The known-diff verifier requires exact equality between actual and expected
  typed footprints after separately requiring `new.writer_version ==
  crate::VERSION`. Immutable record data and participant structure are pinned.
  Conditional footprints correctly distinguish preservation-owner evidence,
  stash phase evidence, root-only publication prefixes, halted error clearing,
  and resolution-failure non-ownership of the conflict outcome.
- Every transition sub-enum has an exhaustive `effect_kind()` match. The 53
  transition variants map one-to-one onto the 53 effect kinds; adding a variant
  requires updating the exhaustive mapping.
- The v1 lifecycle remains behind the accepted test-reachable,
  production-disabled boundary. No v1 writer, migration, CLI, wire, or protocol
  path was activated.

## Budget

The settled package is within every requested R4b-TI ceiling:

| Measure | Settled | Ceiling | Result |
| --- | ---: | ---: | --- |
| Net production-bearing Rust | 2,096 | 2,100 | Pass |
| Production paths | 22 | 22 | Pass |
| Test/tool/doc LOC | 808 | 1,100 | Pass |
| Test/tool/doc paths | 13 | 14 | Pass |

Production accounting is 1,812 lines in seven new lifecycle owners plus +284
net lines across fifteen existing production-bearing paths. Test/tool/doc
accounting includes the focused tests, validator tests, plan/design/budget
updates, and both TI review memos.

All new and materially changed responsibility owners remain below 500 lines.
The largest is `transition/footprint.rs` at 496 lines; other close owners are
`authority.rs` at 479, `record_wire/archive/v0_evidence.rs` at 478, and
`model/v1/validate/acceptance.rs` at 464. These margins are small, so R4b-TR
must add its responsibilities in its own owners rather than growing TI owners
through the review trigger.

## Verification

The following checks passed on the settled bytes:

- `cargo fmt --all -- --check`;
- `cargo clippy -p gwz-core --all-targets --all-features -- -D warnings`;
- `cargo test -p gwz-core v1_lifecycle --lib`: 12 passed;
- `cargo test -p gwz-core model::v1::validate --lib`: 40 passed; and
- `cargo test -p gwz-core --lib`: 814 passed, 0 failed, 1 ignored.

## Gate conclusion

Accept R4b-TI as the frozen validity, isolation, authority, transition, and
effect interface. R4b-TR must preserve these sealed constructors, typed
subjects, exact effect verification, shared cursors, version isolation, and
budget-owner boundaries. Any later widening of proof issuance, raw record
replacement, transition vocabulary, durable shape, or production dispatch
reopens TI interface review.

## R4b-TR architecture/reducer review

**NO-GO.** R4b-S/A remain blocked by two P1 findings and one P2 budget finding.

- **P1 — resolver authority is open:** `BoundExactObservation` carries a caller/observer-selected `V1Transition`, observation binding omits `V1LifecycleRequest`, `resolve_observation` ignores its request, and generic `ObservationKind` becomes generic `BoundPhysicalAction`; Abort/Preserve can therefore execute a not-started forward owner instead of selecting the required abandonment compound.
- **P1 — required matrices are absent:** 28 focused tests hit all 53 positive reducer arms, but do not provide §13’s exhaustive request/state/owner/phase/observation, predecessor, per-variant footprint, root-prefix/cross-owner cursor, or executor-classification rejection matrices.
- **P2 — path ceiling:** TR is +2,163/2,200 production LOC but touches 11/10 production paths; test code is +2,178 before this review, within 2,200 only with the bounded memo allowance.
- The 53 exhaustive reducer arms, exact known-diff/retirement checks, writer-version update, and phase-successor reducers otherwise match the frozen vocabulary; no separate reducer P0–P3 defect was found.
- All owners remain below 500 lines (production maximum 499); focused 28/28, format, and strict all-target/all-feature Clippy pass.
- Close TR only after request-bound typed observations and exact physical actions are resolver-mapped, the missing matrices pass, and production paths return to ≤10.

## R4b-TR architecture/reducer re-review

**NO-GO.** P0: none; P1: three; P2: two; P3: none. R4b-S/A remain blocked.
- **P1 — resolver authority is still optional:** a bound Abort/Preserve `NotStarted(Participant)` reaches `not_started`, which ignores the request and authorizes the forward action; observer-selected `Abandon` is a fourth classification, not the resolver's mandatory mapping of the same not-started fact.
- **P1 — physical/replay proof is incomplete:** preservation `NotStarted` carries no `VerifiedPreservationCursorPrefix`, completed/ambiguous attempt matching skips exact action equality, and every non-evidence publication action is accepted from the same coarse durable phase.
- **P1 — the required matrices remain absent:** 35 focused tests cover representative positives, but not the exhaustive request/state/owner/phase/observation, all-transition predecessor/footprint, publication prefix/index, cross-owner live cursor, or executor-error cross-products; `Preserve + Exhausted` incorrectly rejects instead of entering rollback.
- **P2 — strict gate failure:** all-target/all-feature Clippy rejects `PhysicalActionKind` and `ResolvedV1Action` for large enum variants; focused 35/35, core 837/837 with one ignored, format, and diff checks otherwise pass.
- **P2 — revised accounting fails:** production is +2,489/2,550 LOC across 12/12 paths and owners remain ≤499, but prior settled test/doc 2,200/10 plus 968 remediation LOC/3 paths, the 9-line budget revision/1 path, and 30 re-review lines yields 3,207/3,200 across 14/12 paths.
- Semantic facts now replace observer-supplied transitions, requests are replay-bound, reducer/effect vocabulary remains 53-for-53, and source inspection still finds no reachable production v1 writer; these positives do not close the blockers above.

## R4b-TR final architecture/reducer re-review — 2026-08-10

**NO-GO.** P0: none; P1: one; P2: none; P3: none. R4b-S/A remain blocked.

- **P1 — the frozen TR executable-matrix contract is still incomplete:** §13 requires listed successes and every unlisted rejection. All current ambiguous-observation tests pass `attempt: None`, so exact-action replay and diagnostic suppression for `Ambiguous + BoundExecutionAttempt` are not exercised for any owner. The no-prefix stash/reset test follows only each happy edge while only the prefixed variants enumerate wrong successors; the recovery-origin matrix tests only the six matching diagonals; and there is still no independent expected-footprint/predecessor table across all 53 variants. Positive reductions do run the production footprint verifier, but that is not the required exhaustive rejection evidence.
- The prior authority defects are closed: resolver mappings are request-bound; Abort/Preserve cannot authorize a fresh forward participant action; preservation NotStarted/ambiguity carries an exact cursor-prefix proof; attempts compare exact physical actions; publication actions carry action-specific phases; halted/evidence/preparation reconciliation is closed; and every mutating request maps preservation exhaustion to `FromPreserving` rollback.
- Live filesystem/index publication classification, cross-owner preservation observation, and service fault/restart suites belong to R4b-F/P/G and are not counted as missing TR evidence. The finding above concerns only frozen reducer/resolver interfaces owned by TR.
- Format, strict all-target/all-feature Clippy, document consistency, focused 43/43, and core 845/845 with one ignored all pass. Source inspection finds no production writer and `v1_lifecycle` remains `#[cfg(test)]`.
- Final scope-reviewed accounting passes: +2,721/2,750 production LOC on 12/12 paths, 3,995 focused test LOC before review memos within 4,400, test/tool/doc paths 16/16, and every responsibility owner below 500 lines (maximum 499).

## R4b-TR final executable-matrix re-review — 2026-08-10

**GO.** P0: none; P1: none; P2: none; P3: none. The final §13 evidence remediation closes the architecture review blocker, so R4b-S/A may proceed in dependency order.

- The test-only capture records an effect only after the real reducer returns and `verify_known_diff` succeeds. The resulting set equals the explicit complete 53-variant `EffectKind` vocabulary, proving a successful actual `prepare` path for every declared footprint.
- `Ambiguous + BoundExecutionAttempt` now crosses Success and Failed diagnostics for ordinary retry, ResolveConflict, publication, preservation, and rollback. Resolution first requires the same request and exact `PhysicalActionKind`; the existing same-owner/different-publication-action test confirms mismatch rejection before diagnostic suppression.
- The recovery matrix accepts all six matching literal origins and rejects all 30 off-diagonal cells through the checked reducer. No-prefix stash and reset now try every non-successor from each applicable advance phase, while the retained happy paths prove the exact successors; prefixed, rollback, and publication phase/action matrices remain green.
- The only production-bearing increment is a two-line `#[cfg(test)]` capture hook inside the already test-only `v1_lifecycle` module. A normal library check passes, no raw persistence writer is reachable, and no v1 runtime/CLI/wire behavior is activated.
- Format, strict all-target/all-feature Clippy, document consistency, focused 46/46, and core 848/848 with one ignored pass; both root and `gwz-core` diff hygiene checks are clean.
- Reproducible accounting passes: 4,535 current production lines minus the accepted 1,812-line TI base is 2,723/2,750; 4,766 current focused-test lines minus the accepted 489-line TI base is 4,277/4,400. Paths remain 12/12 production and 16/16 test/tool/doc, and every responsibility owner remains below 500 lines (maximum 499).
