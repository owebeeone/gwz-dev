# R4b-P0.1 interface amendment review — state, durability, and recovery

Date: 2026-08-10

Reviewer: independent state-machine, restart, and recovery review

## Initial verdict (superseded by the 2026-08-10 re-review below)

**NO-GO.** One P0 and two P1 findings remain. No separate P2 or P3 finding was
identified.

The revised separation between preservation and direct rollback is sound in
principle. Direct rollback need not restore its entry prefix: its durable
destination is the operation baseline, provided every full rollback position
has a strict classifier, the action is written before its physical mutation,
and a state exclusive to a future position is rejected. Preserve-then-rollback
correctly retains the preservation handoff and requires a fresh exact equality
check before entering rollback.

The amendment is not yet safe to implement because it excludes the selected
`@root` participant from root publication handoff ownership, and it does not
close two validation/restart uses of the new durable field.

## Review basis

This review inspected the current amendment and traced its new field through
the accepted transition, journal, reverse-lifecycle, and P0 implementation
surfaces:

- `dev-docs/GwzM5-8R4bInterfaceAmendment-1.md`;
- `dev-docs/GwzM5-8R4bTransitionDesign.md`;
- `dev-docs/GwzM5-8I2ActionJournalContract.md`;
- `dev-docs/GwzM5-8R4bReverseLifecycleInterface.md`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/finalization/publication.rs`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/finalization/publication/live.rs`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/transition/reduce/{mod,participant}.rs`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/transition/footprint.rs`;
- `gwz-core/src/workspace_ops/merge/model/v1/validate/{journal,publication}.rs`; and
- the retained v0 preservation plan as characterization evidence in
  `gwz-core/src/workspace_ops/merge/preserve/plan.rs`.

This was an interface review, not a settled implementation review. No
production code was changed and no test result is claimed.

## P0 finding

### P0-1 — A selected `@root` participant is incorrectly forbidden from carrying the publication handoff

The amendment says every participant-owned `Stash` and `ResetAttachedRef`
requires `root_publication_handoff: None`, reserving the candidate handoff for
`PublicationRoot` (`GwzM5-8R4bInterfaceAmendment-1.md:138-155`). That is
incompatible with the frozen root-owner model.

The accepted journal contract explicitly says that **any root-repository
action**, including both `Participant { member_id: "@root" }` and
`PublicationRoot`, may reset the root index/worktree and therefore needs the
recorded publication form for exact restoration
(`GwzM5-8I2ActionJournalContract.md:216-225`). The accepted owner derivation
also deliberately omits `PublicationRoot` when a selected `@root` participant
owns the same root ref (`GwzM5-8R4bTransitionDesign.md:691-702` and
`GwzM5-8R4bReverseLifecycleInterface.md:329-335`). Retained v0 behavior
likewise derives a publication prefix for a participant whose target kind is
root (`gwz-core/src/workspace_ops/merge/preserve/plan.rs:87-128`).

Consequently this is not a harmless overrestriction. In the selected-root
case there is no later `PublicationRoot` owner to repair the omission. A root
participant stash can consume the publication-owned index/worktree form, and a
root participant ref reset can replace it with the anchor checkout. With the
action forced to `None`, no `NormalizeRoot`/`RestoreRoot` phase owns the exact
`Pre` or `Staged` image. A crash/restart can therefore either lose the accepted
handoff or strand preservation before the fresh equality required by
preserve-then-rollback.

Required correction:

- define the condition by repository ownership, not enum variant: a
  `PublicationRoot` action **or** `Participant { member_id: "@root" }` action
  that operates while the durable handoff is `Candidate` must carry and equal
  that exact candidate handoff;
- require `None` only for non-root participant actions (and for root actions
  when no candidate publication handoff exists); and
- cross the phase/restart tests with both root-owner variants and both
  `Pre`/`Staged` forms, including the selected-root collision case where no
  `PublicationRoot` owner is appended.

## P1 findings

### P1-1 — Action-free preservation cursor and exhaustion proofs are not explicitly bound to the exact handoff

The amendment binds the exact handoff to a pending root action and names the
`NormalizeRoot`/`RestoreRoot` classifiers
(`GwzM5-8R4bInterfaceAmendment-1.md:125-155`). It does not require the same
exact `{prefix,index}` comparison when no preservation action is pending:
while issuing `VerifiedPreservationCursorPrefix`, while preparing the next
root action, or while issuing `VerifiedPreservationExhausted`.

Those action-free boundaries are real crash boundaries. `FinishStash` and
`FinishResetAttachedRef` clear the pending journal, after which the service
loop restarts from the checked record. The accepted contract requires every
earlier position in the two-pass cursor to be proven exact before a later
position is classified, advanced, or executed
(`GwzM5-8I2ActionJournalContract.md:128-139`), and requires exact root
restoration as part of exhaustion before `Preserving -> RollingBack`
(`GwzM5-8R4bTransitionDesign.md:474-479`). The executable preservation matrix
also requires restart on both sides of every physical/result boundary and an
exact prefix/index at root reset completion
(`GwzM5-8R4bReverseLifecycleInterface.md:617-634`).

Without an explicit binding, the old prefix-only cursor check can still accept
the opposite index form after a crash that occurs just after a pending action
is cleared. The later fresh rollback-entry handoff comparison may prevent a
wrong rollback transition, but only after preservation has already accepted an
incorrect cursor/exhaustion fact; that is not the promised no-adoption rule.

Required correction: state that every preservation observation involving the
root—pending action classification, earlier-prefix proof, next-action
preparation, and final preservation exhaustion—must compare the complete live
root to `record.preservation_publication_handoff`, including exact index form.
The action-free opposite-index form must be ambiguous/error without a rewrite,
not a valid cursor prefix or exhaustion proof.

### P1-2 — Checked validation does not close the durable variant/record compatibility matrix

The amendment requires presence by lifecycle and checks a `Candidate` handoff
for required fields and an “impossible” prefix/index pair
(`GwzM5-8R4bInterfaceAmendment-1.md:114-136`). It does not state the structural
compatibility rules for `NoCandidate` and `EvidencePending`, nor does it freeze
the exact candidate pair matrix (including the degenerate `Marker/Staged`
case).

P0's authority classifier has materially different durable-state domains:
no-candidate phases, evidence-pending `CommittingEvidence`, pre-publication
`Baseline/Pre`, partial publishing prefixes, and completed staged forms
(`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/finalization/publication.rs:20-129`). The exact candidate
matrix is not merely `prefix x index`: `Marker/Staged` is complete only when
both lock and boundary equal their baseline bytes, while `Baseline/Staged` and
`Lock/Staged` are never valid
(`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/finalization/publication/live.rs:43-77`).

As written, a checked preserving record can satisfy the listed rules with, for
example, `NoCandidate` attached to candidate-bearing publishing state or
`EvidencePending` outside the exact pre-evidence shape. Such a value cannot
have been issued by the entry authority, but checked-open validation is the
boundary that must reject altered or malformed durable bytes. It can otherwise
skip required root ownership or become permanently unequal at the later
preserve-to-rollback gate.

Required correction: define one pure durable compatibility projection of the
closed F handoff classifier and require validation to use it:

- `NoCandidate` only for the exact durable publication shapes from which F can
  issue `NoCandidate`;
- `EvidencePending` only for its exact candidate-bearing, composition-absent
  `CommittingEvidence` shape; and
- `Candidate { prefix, index }` only for the exact step/candidate/composition
  and candidate-equality-dependent pair matrix.

The transition footprint still separately proves historical installation and
immutability; structural validation must prove that the stored value is a
possible authority result for the stored record.

## P2 findings

None separate from the two P1 closure requirements above.

## P3 findings

None.

## Boundaries accepted by this review

Subject to the findings above, the following revised decisions are coherent:

- only the three preservation-entry transitions install the field atomically;
  the halted-outcome and not-started compounds must include it in their fixed
  union footprints as well as changing the participant and operation state
  (`GwzM5-8R4bInterfaceAmendment-1.md:96-107`;
  `GwzM5-8R4bTransitionDesign.md:437-472`);
- recovery retains the field without reinterpretation: it is required for a
  `Preserving` origin, optional for a `RollingBack` origin according to direct
  versus preserve-then-rollback history, and unchanged across enter/resume;
- preserve-then-rollback's fresh exact equality and unchanged retention close
  the transition boundary (`GwzM5-8R4bInterfaceAmendment-1.md:109-123`);
- direct rollback intentionally has no preservation handoff. Its safety comes
  from a complete full-shape matrix, write-ahead action installation, exact
  before/after classification, and rejection of future-step shapes
  (`GwzM5-8R4bInterfaceAmendment-1.md:157-171`), consistent with the journal's
  action-before-mutation rule (`GwzM5-8I2ActionJournalContract.md:166-171`);
  the existing broad “rollback representable” preflight is therefore not an
  acceptable implementation of the amendment; and
- retaining the preservation handoff on terminal `Aborted` archives is
  consistent with immutable history, while direct-abort archives keep it
  absent.

## Gate condition

The amendment may proceed only after P0-1 and both P1 findings are resolved in
the normative text. The settled implementation review must then exercise:

- both root owner variants (`PublicationRoot`, selected participant `@root`);
- every F handoff variant and every valid/invalid prefix-index pair;
- crash/restart immediately before and after each preservation journal clear,
  action install, physical mutation, and progress write;
- action-free preservation cursor and exhaustion checks with opposite-index
  drift;
- direct and preserve-then-rollback recovery origins with exact field
  retention; and
- direct rollback's complete full-shape per-position matrix, especially
  rejection of a future-step shape before its owning action is durable.

## Re-review — 2026-08-10

### Verdict

**GO.** The current amendment closes P0-1, P1-1, and P1-2. No P0, P1, P2, or
P3 finding remains at the interface boundary. This verdict authorizes the
paused P1-P4 implementation work under the amendment's stated sequencing; it
does not activate production v1 records or replace the required settled-tree
code re-review.

### Corrected-boundary verification

#### P0-1 closed — both root owner variants carry the exact handoff

Section 2.5 now defines publication restoration by repository ownership. Both
`PublicationRoot` and `Participant { member_id: "@root" }` carry and exactly
match the durable candidate handoff, while only non-root participants require
`None` (`GwzM5-8R4bInterfaceAmendment-1.md:176-201`). It also retains the
collision rule: a selected `@root` participant is the sole normalization and
restoration owner because no duplicate `PublicationRoot` is appended
(`GwzM5-8R4bInterfaceAmendment-1.md:192-195`).

This matches the accepted action-journal rule that either root owner can reset
the root index/worktree and must restore the publication form
(`GwzM5-8I2ActionJournalContract.md:216-225`), and the deterministic owner set
(`GwzM5-8R4bTransitionDesign.md:691-702`). The amendment explicitly crosses
both owner variants with `Pre` and `Staged` restart tests
(`GwzM5-8R4bInterfaceAmendment-1.md:216-221`). P0-1 is closed.

#### P1-1 closed — action-free cursor, pass boundary, and exhaustion bind the full handoff

Section 2.6 now requires every preservation observation capable of authorizing
progress to reobserve the complete root against the operation-level handoff.
The list expressly includes pending-action classification,
`VerifiedPreservationCursorPrefix`, next-root-action preparation after a prior
journal clears, whole-prefix verification between passes, and
`VerifiedPreservationExhausted` after the final clear
(`GwzM5-8R4bInterfaceAmendment-1.md:203-214`).

The proof payloads bind both source digest and exact durable handoff, including
index form; an opposite form after restart issues neither prefix, next-action,
nor exhaustion authority and performs no rewrite
(`GwzM5-8R4bInterfaceAmendment-1.md:216-221`). This covers the action-free crash
windows required by the accepted cursor-prefix and exhaustion contracts
(`GwzM5-8I2ActionJournalContract.md:128-139`;
`GwzM5-8R4bTransitionDesign.md:474-479`). P1-1 is closed.

#### P1-2 closed — one pure compatibility projection owns the complete durable matrix

Section 2.4 now assigns checked-open validation and preservation-entry reducers
to one model-owned compatibility projection rather than parallel matches
(`GwzM5-8R4bInterfaceAmendment-1.md:138-143`). Its durable-shape table closes
`NoCandidate`, `EvidencePending`, pre-evidence and post-evidence candidate
states, publishing, and terminal/no-publication cases
(`GwzM5-8R4bInterfaceAmendment-1.md:145-157`).

The candidate matrix exactly matches the accepted F resolution:

- all four `Pre` prefixes are non-ambiguous only while publishing;
- `Boundary/Staged` is complete;
- `Marker/Staged` is allowed only in the degenerate case where candidate lock
  and boundary bytes both equal their baselines; and
- `Baseline/Staged`, `Lock/Staged`, and every other pair reject
  (`GwzM5-8R4bInterfaceAmendment-1.md:159-169`;
  `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/finalization/publication/live.rs:43-77`).

The text correctly separates this pure stored-record possibility check from
the live observer's independent exact physical check
(`GwzM5-8R4bInterfaceAmendment-1.md:171-174`). P1-2 is closed.

#### Direct rollback remains correctly distinct

Section 2.7 retains baseline—not entry prefix—as direct rollback's immutable
destination, so direct rollback does not install the preservation handoff. Its
safety boundary is instead explicit: persist each action first, classify only
strict complete before/after shapes for that position, accept external exact
completion only after the owning action is durable, and reject mixed or
future-step states (`GwzM5-8R4bInterfaceAmendment-1.md:223-237`).

That is consistent with the accepted action-before-mutation rule
(`GwzM5-8I2ActionJournalContract.md:166-171`). It also expressly disallows the
existing broad “any representable publication prefix” predicate as rollback
progress authority. Preserve-then-rollback remains separately closed by fresh
equality with, and unchanged retention of, the durable preservation handoff
(`GwzM5-8R4bInterfaceAmendment-1.md:109-123`).

### Re-review findings

#### P0

None.

#### P1

None.

#### P2

None.

#### P3

None.

### Implementation evidence still required

The GO depends on implementing the amendment literally. The settled-tree
review must reject any implementation that:

- gives the handoff only to `PublicationRoot` and not selected `@root`;
- checks only prefix, or checks the exact index only while an action is
  pending;
- duplicates or weakens the model-owned durable compatibility matrix;
- uses a broad rollback-representable predicate in place of a full physical
  before/after shape for the current persisted rollback step; or
- changes recovery origin, clears the handoff, or advances progress from an
  executor diagnostic rather than a fresh exact bound observation.

Required tests remain those listed in the initial gate condition, with both
root owners, all compatibility rows, both index forms, action-free journal
clear windows, both reverse recovery origins, and every direct-rollback
physical/durable crash boundary.

## Second re-review — 2026-08-10

### Verdict

**GO.** The prior preservation/recovery GO still holds, the new canonical
acquisition and final direct-ref exhaustion rules are state-safe, and both
controlling-document reconciliations are closed. The direct-rollback matrix now
covers all ordinary, single-no-op, both-no-op, staged, and valid baseline-entry
forms while rejecting nonterminal future-only shapes. No P0, P1, P2, or P3
finding remains.

### P0 challenges found and closed

#### P0-1 — The explicit direct-rollback matrix strands valid no-op-component and degenerate staged publications

Section 2.7 defines a physical shape as `[Boundary, Lock, Marker, Index]`, with
`B` for baseline and `C` for candidate. It correctly says that a candidate lock
or boundary byte-equal to baseline is a true no-op represented as `B`
(`GwzM5-8R4bInterfaceAmendment-1.md:239-249`). The first explicit table
inspected during this re-review, before its in-review corrections, gave
`EvidenceCommit` only `BBBB`, `BBCB`, `BCCB`, `CCCB`, and `CCCC`, and gave
`Boundary` only `CCCB`/`CCCC` before and
`BBBB`/`BBCB`/`BCCB`/`BCCC` after. Those sets were incomplete.

The accepted F resolver expressly supports each single unchanged candidate
component and the both-unchanged degenerate terminal marker
(`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/finalization/publication/live.rs:235-268`). Combined with the closed handoff matrix already
accepted in amendment §2.4, the exact valid physical publication set is:

```text
BBBB  Baseline/Pre
BBCB  Marker/Pre
BCCB  Lock-or-Boundary/Pre when boundary is a no-op
CBCB  Boundary/Pre when lock is a no-op
CCCB  Boundary/Pre when neither is a no-op
BBCC  degenerate Marker/Staged when both are no-ops
BCCC  Boundary/Staged when boundary is a no-op
CBCC  Boundary/Staged when lock is a no-op
CCCC  Boundary/Staged when neither is a no-op
```

The intermediate `EvidenceCommit` row therefore rejected four legal
entry/restart states: `CBCB`, `BBCC`, `BCCC`, and `CBCC`. Its `Boundary` row
also rejected `CBCB`/`CBCC` as exact before states when only lock is a no-op,
and rejected `BBCC` as an exact after state for the already accepted degenerate
`Marker/Staged` publication. That intermediate table would have made direct
abort unrepresentable before its first physical mutation or at its boundary
step.

Required correction:

- make the `EvidenceCommit` before/after set exactly
  `BBBB, BBCB, BCCB, CBCB, CCCB, BBCC, BCCC, CBCC, CCCC`;
- make `Boundary` before exactly `CBCB, CCCB, CBCC, CCCC` when boundary is not
  a no-op;
- make `Boundary` after exactly `BBBB, BBCB, BCCB, BBCC, BCCC`; and
- add an executable cross-product of lock-same/different, boundary-same/
  different, `Pre`/`Staged`, every durable step, and before/after/ambiguous.

The later `Lock`, `Marker`, `Index`, and `Complete` sets are coherent once the
correct boundary states reach them.

The amendment now includes all nine shapes at `EvidenceCommit`, all four
candidate-boundary shapes at `Boundary` before, and the missing no-op/staged
forms at later steps. A subsequent stricter edit removed `BBBB` from both
`Boundary` and `Lock` after. That intermediate edit was not total over the
accepted entry domain.

`CommittingEvidence` with complete recorded composition is explicitly
compatible with `Candidate { Baseline, Pre }`
(`GwzM5-8R4bInterfaceAmendment-1.md:145-155`). Direct rollback from that state
starts publication-evidence rollback at physical `BBBB`. `EvidenceCommit`
changes only HEAD, so after its durable advance the `Boundary` step sees exact
baseline HEAD plus unchanged `BBBB`. With `BBBB` absent from `Boundary` after,
neither before nor after matches and rollback becomes permanently ambiguous.
The same destination-equivalent shape must remain `After` through `Lock`; no
durable direct-entry handoff exists from which either step could distinguish
legitimate initial `BBBB` from exact external completion.

Restore `BBBB` to `Boundary` and `Lock` after. The prose should reject shapes
belonging exclusively to a future step, while allowing the exact immutable
baseline destination when it is also a legal initial direct-entry shape.
Alternatively, the design would have to persist new direct-entry authority,
which would reopen the reviewed durable model.

The final table restores `BBBB` to both after sets and explicitly defines it as
the sole cross-step exception: exact baseline HEAD plus `BBBB` is immutable
destination-equivalent and can arise legitimately after `EvidenceCommit`
entered from `Baseline/Pre`; all nonterminal future-only shapes remain
ambiguous (`GwzM5-8R4bInterfaceAmendment-1.md:251-267`). This makes the phase
graph total without adding direct-entry durable authority. P0-1 is closed.

### P1 findings

None. The exact preservation handoff, selected-`@root` ownership,
action-free cursor/exhaustion binding, compatibility projection, compound
footprints, recovery retention, and preserve-then-rollback equality accepted
in the first re-review remain unchanged and correct.

### P2 challenges found and closed

#### P2-1 — The controlling refactor document prematurely unblocks P1-P4

The amendment remains paused pending architecture re-review
(`GwzM5-8R4bInterfaceAmendment-1.md:5-6`), as do the transition design, reverse
interface, and budget ledger (`GwzM5-8R4bTransitionDesign.md:9-14`;
`GwzM5-8R4bReverseLifecycleInterface.md:733-739`;
`GwzM5-8ChangeBudget.md:745-750`). The intermediate refactor text instead said
P1-P4 were already unblocked. Its status and R4b-P checkpoint now agree that
the lanes exposed P0.1 omissions and remain paused until both independent
amendment reviews return GO (`GwzM5-8Refactor.md:10-15` and
`GwzM5-8Refactor.md:2319-2329`). P2-1 is closed.

#### P2-2 — The intermediate P0.1 document charge was two lines below the checked tree

The intermediate ledger declared 1,355 from a stale 1,295/60 split. Before the
second-review appends, the lead recounted the then-current three amendment/
review files at 1,306 lines and the four controlling-document corrections at a
net 66, for an exact 1,372 pause-point charge
(`GwzM5-8R4bInterfaceAmendment-1.md:681-684` and :716-719;
`GwzM5-8ChangeBudget.md:745-750`). The text now correctly identifies 1,306 as
the combined three-file count, not the amendment alone. Both review files have
grown during their final re-reviews, and the amendment explicitly requires
those append deltas to be recorded before implementation begins. P2-2 is
closed subject to that already stated final recount.

### P3 findings

None.

### Newly expanded boundaries accepted

The following additions are correct and do not reopen the earlier GO:

- canonical/unknown-field closure treats the v1 handoff as typed authority and
  makes all five v0 extension collisions migration-ineligible rather than
  retiring or silently discarding them
  (`GwzM5-8R4bInterfaceAmendment-1.md:277-304`);
- the authority-owned archive constructor binds source path/digest,
  workspace/merge/operation identity, request, terminal state, and exact
  destination state without exporting a caller-constructible fact
  (`GwzM5-8R4bInterfaceAmendment-1.md:331-351`);
- the single canonical location acquisition returns path, immutable bytes, and
  digest together, rejects noncanonical/nonregular/symlink/changed-during-read
  leaves, and the optimistic status reread retries the whole arbitration while
  normalizing source races to the typed contention result
  (`GwzM5-8R4bInterfaceAmendment-1.md:404-463`);
- status now includes `operation_id` and `source_ref`, preserving exact
  composition-message evidence in the borrowed observer
  (`GwzM5-8R4bInterfaceAmendment-1.md:465-497`);
- GC uses exact direct-ref observation at global preflight, immediately before
  every deletion, and again for final all-absent exhaustion; any direct,
  non-direct, broken symbolic, or unavailable row retains the archive
  (`GwzM5-8R4bInterfaceAmendment-1.md:504-541`); and
- the 54-production/43-evidence P0.1 manifest and revised package/program
  ceilings are internally coherent, with the pre-append document charge
  corrected as described above: R4b-P is 8,250/2,050/15,500 with 130/117
  charged and 89/89 unique paths, while the conservative program ceiling is
  16,150/27,200 and 137/136 unique paths
  (`GwzM5-8R4bInterfaceAmendment-1.md:543-723`;
  `GwzM5-8ChangeBudget.md:481-517`).

### Second re-review gate

State/restart **GO is restored**. Before implementation resumes, the ledger
must perform its already promised mechanical recount of both final review
appends. The settled-tree implementation review must prove that the corrected
matrix comes from one pure owner reused by preflight, ordinary observation,
rolling-back recovery, and executor recheck, with no broad fallback predicate.

## Implementation-start manifest correction — 2026-08-10

**GO remains.** The added `record_wire/open_v0/structural.rs` and
`record_wire/archive/v0_evidence.rs` entries are necessary: both contain direct
`MergeOperationRecordV1` common-view constructors, and each must initialize the
new v1-only preservation-publication handoff to `None`. That preserves v0
semantics; any non-`None` value would invent preservation authority. The
implementation must keep handoff-presence validation version-aware so these
valid v0 projections do not acquire a v1 lifecycle obligation.

The corrected counts supersede the 54/130/89 figures above: P0.1 has **56**
production paths, R4b-P has **132 charged / 91 unique** production paths, and
the program-wide production-path ceiling remains **137** because accepted R3
already charges both paths. The two one-line initializers fit the existing
P0.1 production ceiling, so no state or LOC ceiling changes.

## Abort-preflight manifest correction — 2026-08-10

The `abort/preflight.rs` charge is valid: its test-only v1 preflight imports the
single canonical root-artifact observer, so this is visibility/lint fallout and
does not add policy, normal-build reachability, or a lifecycle transition.
P0.1 is therefore 59 production paths and R4b-P is 135 charged / 93 unique;
the 137-path program union and every line and evidence ceiling remain unchanged.

Restart **GO is held only for ledger coherence**: the amendment and budget have
the corrected figures, but `GwzM5-8R4bReverseLifecycleInterface.md` still says
58 at lines 746 and 755 and 134 at line 761. Mechanically changing those three
values to 59, 59, and 135 closes the state/restart concern; there is no semantic
or code objection.

The reverse-lifecycle checkpoint now contains 59, 59, and 135 at those three
locations. The ledger is coherent and final state/restart **GO is restored**.

## Authority-dispatcher manifest correction — 2026-08-10

Charging `authority/dispatcher.rs` is correct, and the 60-path P0.1 / 136
charged / 93 unique R4b-P production accounting is coherent without changing
the 137-path program union or any line/evidence ceiling. Boxing
`ObservationKey.kind` is also semantically transparent: `Box<T>` preserves the
serialized value, value equality, proof payload hash, and borrowed `kind()` API.

Restart is nevertheless **HOLD** because that indirection does not clear the
actual lint. `cargo clippy -p gwz-core --all-targets -- -D warnings` still
reports `V1NextAction::Observe(BoundObservationRequest)` at at least 296 bytes
versus the 88-byte next-largest variant. The remedy must move indirection to a
level that materially shrinks the bound request (for example the enum field or
the internal `ProofBinding`) while retaining exact location equality; lint
suppression or removal of location authority remains unacceptable.

The settled implementation additionally boxes the private
`BoundValue<ObservationKey>` inside the unchanged `BoundObservationRequest`.
That materially compacts `V1NextAction::Observe` while preserving the enum
variant, borrowed lifecycle/kind APIs, value equality, serialized observation
key, payload hash, and exact location binding. Strict Clippy no longer reports
`large_enum_variant`; with the unrelated `abort/participants.rs`
`items_after_test_module` lint isolated, the all-targets Clippy run passes.
The 60/136/93 ledger is aligned across all three control documents, so
state/restart **GO is restored for this correction**. The unrelated lint remains
a package-gate item, not a state/restart or dispatcher finding.
