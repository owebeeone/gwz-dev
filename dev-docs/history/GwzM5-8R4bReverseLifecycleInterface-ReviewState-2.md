# R4b-P reverse-lifecycle interface re-review — state, restart, and ownership

Date: 2026-08-10

Reviewer: independent state-machine/interface re-review

Verdict: **GO**

No P0, P1, P2, or P3 finding remains. Every finding from the first review is
closed in the amended interface and parent plan. The new P0 prerequisite is
constructible without exposing or duplicating reducer state, the four consumer
lanes have non-overlapping semantic ownership, and the executable evidence fits
an explicit reconciled budget.

This verdict accepts the interface and permits the P0 prerequisite to begin
once the other required independent re-review also reports GO. It does not
permit P1-P4 to start before P0 is implemented, focused-tested, and accepted by
the same two reviewers.

## 1. Review basis

This re-review checked the amended:

- `GwzM5-8R4bReverseLifecycleInterface.md`;
- `GwzM5-8R4bTransitionDesign.md`;
- `GwzM5-8Refactor.md`;
- `GwzM5-8ChangeBudget.md`; and
- `GwzM5-8R4bReverseLifecycleInterface-ReviewState.md`.

The review re-ran every prior P1/P2 finding as a closure test, then checked the
new P0 signatures, ownership handoffs, executable matrices, path manifest, and
ledger arithmetic for additional P0-P3 defects.

## 2. Prior-finding closure

| Prior finding | Result | Decisive amendment |
| --- | --- | --- |
| P1-1 retained forward owner was not state-exact | **Closed** | §5 now separates `Executing` from `Halted`: completed `Executing` uses ordinary `RecordParticipantOutcome` with `EntryFact::None` and a fresh next-digest preflight; only the frozen `Halted` compounds combine outcome and entry. Not-started and ambiguity rows are also state-exact. |
| P1-2 stash evidence was not restart-complete | **Closed** | The Git seam is error-bearing and `GitPreservationStashEvidence` carries a decoded `GitPreservationImage`, including the exact dirty summary reconstructed from stash trees. Bundle-prefix verification uses that image after restart rather than treating a hash as reconstruction authority. |
| P1-3 field-10 ownership was contradictory and unbudgeted | **Closed** | P3 explicitly owns the complete frozen I2 field-10 schema, generated bindings, one v0/v1 projector, response population, Rust/Python JSON/JSONL/human rendering, and parity fixtures before A1. The parent plan and ledger authorize that append-only durable-record projection while keeping v1 creation, migration, and mutation dispatch disabled until A1. |
| P1-4 archive bytes lacked no-follow deletion provenance | **Closed** | Archive/status/GC acquisition now requires canonical real parents and a regular non-symlink named leaf. GC performs the same acquisition and decode initially and before deletion, proves byte identity and an identical immutable projection/worklist, then proves all owned refs absent. |
| P2-1 status lacked exact source arbitration and lineage | **Closed** | §9 freezes every open/archive location combination, including identical and contradictory both-copy shapes. Open status uses checked load, complete live observation, unchanged-source reread, and one whole-read retry or a typed contention/recovery error. |
| P2-2 root facts permitted byte-only or followed reads | **Closed** | Every manifest, lock, boundary, marker, candidate, and index observation now requires canonical real parents, a regular non-symlink leaf, exact bytes, and the accepted mode rule. Operational errors remain errors; symlink, directory, mode, and mixed forms issue no proof. |
| P2-3 reverse recovery had a cross-lane classifier dependency | **Closed** | P1 owns preserving-origin recovery through its preservation classifier/prefix proof; P2 owns rolling-back-origin recovery through its rollback cursor/classifier. P0 owns only the stable sealed router. |

## 3. P0 interface closure

The amended P0 surface supplies the missing production reverse-entry chain:

1. the transition owner creates a request-specific, digest-bound
   `PreparedReverseEntryView` by applying the exact predecessor effect in
   memory;
2. the F authority observer obtains the publication handoff through the sealed
   transition visitor or returns the evidence result that must be recorded
   first;
3. P1/P2 authority observers perform complete read-only global preflight over
   the anticipated action-free model and issue request/kind/digest-bound sealed
   preflight tokens; and
4. the P0 entry constructors consume the preview, F handoff, lane token, and,
   for preserving-to-rollback entry, `VerifiedPreservationExhausted`.

The anticipated model remains implementably accessible without becoming a
consumer-owned mutable projection. `transition::visit_reverse_entry` requires a
non-cloneable authority-issued inspection permit and a visitor sealed to F,
P1, or P2; it lends the model only for the call and permits only a sealed
authority result. Runtime, service, and physical modules cannot call the
bridge or obtain, serialize, or mutate the model. Thus no lane predicts a
reducer result and no caller supplies an eligibility boolean.

The ordering contract is also exact. Mandatory retained-owner outcome or
publication-evidence reconciliation may occur first. After that reconciliation,
global preflight failure occurs before the reverse entry transition and before
any reverse physical mutation. This matches the frozen
`Executing -> RecordParticipantOutcome -> fresh entry preflight` sequence while
preserving the all-member no-partial-reverse-mutation guarantee.

P0 additionally freezes separate sealed preservation, rollback, and archive
lane traits, compiling lane skeletons, the router, and the protocol-neutral
`ValidatedArchivedMerge` handoff. P1/P2/P4 can later implement their assigned
traits without editing the router or central lifecycle match.

The router's added crate-v1-visible accessor in
`authority/dispatcher.rs` exposes only the read-only lifecycle request already
bound into `BoundObservationRequest`. It lets the stable router distinguish the
frozen Abort/Preserve routes without accepting an unbound caller value,
constructing authority, or exposing record/reducer state. This is a necessary
P0 binding accessor, not a second dispatcher or a widened proof surface.

## 4. State, restart, and mutation assessment

The final interface retains the required restart discipline:

- every physical phase has exact before/after/third-state classification;
- a persisted owner is reread and its complete prefix is proved before action;
- executor return remains diagnostic only, while fresh observation alone
  advances or enters recovery;
- preservation performs artifact pass, whole-prefix proof, then reset pass in
  the same durable owner order;
- an incoming Abort cannot bypass partial preservation and needs exact
  preservation exhaustion before rollback entry;
- rollback remains publication evidence, reverse durable participants, then
  selected-root metadata, with one persisted phase per physical action;
- no-mutation participants use the frozen transition without a fabricated
  journal;
- between-action reverse ambiguity remains unrepresentable and is not rewritten
  into a fabricated recovery owner;
- archive restart handles source-only, identical both-copy, and destination-only
  forms and always derives the result from a final destination reopen; and
- GC performs full preflight before deleting the first ref, safely resumes an
  absent prefix, retains immutable archive bytes until all refs are absent, and
  never deletes native stashes or preservation bundles.

The preservation, rollback, recovery, status/archive, and GC tables in §§14-15
are executable matrices rather than prose-only intentions. They cover phase
boundaries, process restarts, source-digest changes, operational errors,
noncanonical/symlink forms, executor success/failure, fresh observation
outcomes, archive replacement, and partial-GC restart.

## 5. Protocol and archive ownership

P3 is the sole response owner. It maps checked open v0/v1 records and R3-decoded
archived v0/v1 projections into the frozen optional field 10 and owns the full
presence/omission and cross-driver parity matrix. P4 never constructs a
response; it returns `ValidatedArchivedMerge` containing immutable destination
bytes, digest, R3 projection, and cleanup worklist. This keeps protocol mapping
out of archive mutation and prevents P3 from acquiring open-record mutation
authority.

The parent documents now agree with that split: R4b-P installs the already
accepted I2 projection before A1, while A1 only activates v1 writer, migration,
and lifecycle dispatch. No new durable field, phase, transition, action,
observation, or production v1 mutation route is authorized.

## 6. Budget and manifest reconciliation

The revised stop ceiling reconciles exactly:

| Slice | Production LOC | Moved LOC | Test/tool/doc LOC | Production paths | Test/tool/doc paths |
| --- | ---: | ---: | ---: | ---: | ---: |
| P0 | 900 | 0 | 3,000 | 23 | 8 |
| P1 | 950 | 1,050 | 1,800 | 9 | 8 |
| P2 | 900 | 650 | 1,800 | 9 | 8 |
| P3 | 900 | 250 | 2,500 | 18 | 13 |
| P4 | 750 | 100 | 1,500 | 8 | 8 |
| Aggregate | 100 | 0 | 1,500 | 3 | 12 |
| **Charged ceiling** | **4,500** | **2,050** | **12,100** | **70** | **57** |

The enumerated overlaps reduce 70 charged production paths to 59 unique paths:
P0 contributes 23; P1 and P2 add five each beyond their four P0 skeletons; P3
adds 17 beyond P0 status; P4 adds seven beyond P0 archive; and aggregate wiring
adds two beyond P0's lifecycle module. The exact test/tool/doc lists total 57
charged and unique paths.

The parent ledger matches those numbers. Replacing the withdrawn P allowance
produces aggregate ceilings of 12,400 production lines, 23,800 test/tool/doc
lines, 111 unique production paths, and 113 unique test/tool/doc paths.
Generated binding LOC remains excluded, while the schema and all three generated
output paths are counted. No matrix is left without a named owner or path
allowance.

## 7. Parallel-start decision

This interface is ready for the staged implementation order it declares:

1. P0 may begin after both independent interface re-reviews are GO.
2. P0 must then be implemented and focused-tested and must pass the same two
   code/interface re-reviews.
3. Only then may P1 preservation/recovery, P2 rollback/recovery, P3
   status/protocol, and P4 archive/GC proceed concurrently.
4. Shared router integration waits for P1 and P2; archive-response integration
   uses only the frozen P4-to-P3 handoff; R4b-G waits for all four settled lanes.

No compatibility shim, local reducer prediction, cross-lane policy duplicate,
or unreviewed path/interface widening is permitted. On the reviewed documents,
there is no remaining P0-P3 reason to block P0.
