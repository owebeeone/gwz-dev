# R4b-P0.1 interface amendment review — architecture and ownership

Date: 2026-08-10

Reviewer: independent architecture/ownership review

## Verdict

**NO-GO.**

The amended durable handoff is directionally correct, and the latest revision
closes the selected-`@root` ownership, action-free cursor/exhaustion, and
prefix/index compatibility omissions identified by the state review. The
authority-owned `BoundExactObservation` issuance model is also sound in
principle, and the split v0/v1 archive decoder design can preserve the pre-A1
gate.

Five P1 interface blockers remain. The proposed borrowed status view cannot
perform the exact root-evidence check it promises; the new durable v1 field is
not closed through unknown-field/upgrade authority; the production archived-v0
decoder has no normal-build canonical acquisition consumer path; GC's final
absence proof is still allowed to resolve refs; and the 1,100/1,600 correction
is neither reconciled to the controlling slice/aggregate ledger nor backed by
an exact path manifest. There are no P0 findings.

## Review basis

I reviewed the current amendment and directly traced the relevant accepted and
settled code through:

- `model/v0.rs`, `model/v1/{record,canonical}.rs`, and v1 validation;
- `record_wire/{mod,decode,header}.rs`, archive v0/v1 projection/cleanup, and
  checked unknown-field extraction/overlay;
- `status/snapshot.rs`, `publication.rs`, `root/{finalization,abort}.rs`, and
  `acceptance/publication.rs`;
- reverse-entry authority, resolver, reducer, and F's exact publication
  classifier;
- `v1_lifecycle/{reverse,archive}.rs`, checked archive execution, and store
  archive/retention;
- `GitBackend` ref and preservation implementations plus both v0 and v1 GC
  preflight/final-recheck paths; and
- the controlling R4b-P ownership and budget tables in
  `GwzM5-8R4bReverseLifecycleInterface.md` and
  `GwzM5-8ChangeBudget.md`.

This is an interface review, not a settled-tree implementation review. No
production code was changed.

## Findings

### P1-1 — the borrowed status view omits exact root-evidence inputs

Section 5 freezes `MergeStatusRecordView` to `workspace_id`, `merge_id`,
`state`, `baseline`, `selected_targets`, `participants`, `publication`, and
`operation_drift`, while requiring the single observer and its smaller views
to own root evidence and exact root finalization.

That field set cannot implement the existing exact observation. The shared
`observe_root_evidence` verifies the GWZ composition commit with
`composition_message`, whose exact bytes include `source_ref`, `merge_id`, and
`operation_id`. `merge_id` is present in the proposed view, but `source_ref`
and `operation_id` are not. Dropping the message check weakens authority;
reaching back to an owned v0/v1 record defeats the view; and adding a second v1
root-evidence classifier violates the amendment's own no-duplication rule.

The interface must add at least borrowed `operation_id` and `source_ref` (or
freeze a smaller exact root-evidence view that contains them), then demonstrate
that every retargeted publication/root helper consumes only declared borrowed
inputs. The ordinary v0 wrapper and disabled-v1 constructor must build the same
view.

### P1-2 — the new durable field is not closed through unknown-field and upgrade authority

The amendment adds the typed top-level v1 field
`preservation_publication_handoff` and mentions model, canonical, validation,
and the eligible v0 adapter. That is not the complete checked-wire surface.

The accepted checked store extracts every unrecognized v1 top-level key into
an `UnknownFieldManifest`, overlays surviving unknowns on every rewrite, and
maintains an explicit v0-to-v1 top-level collision list. Today those lists know
only the four installed v1 fields (`accepted_workspace`, `recovery_context`,
`pending_rollback`, and `pending_preservation`). Unless the new field is added
to both known-v1 extraction and the v0-to-v1 collision set, a source v1 record
can classify the typed field as an extension, and a v0 extension of the same
name can cross the upgrade boundary and collide with the installed field.
Setting the adapter's typed member to `None` does not retire that unknown-field
authority.

The interface must explicitly own and manifest the unknown-field extractor,
v0-to-v1 collision map, overlay/retirement evidence, and focused tests. The
canonical v1 installed state must also include the field so canonical
comparison cannot ignore it. This is required record authority, not optional
test fallout.

### P1-3 — `decode_archived_v0` does not close the production archived-status API

Section 4 correctly separates a normal-build v0 decoder from test-only v1
decoding. It does not define how ordinary status acquires the archive bytes or
which normal-build owner performs source arbitration and canonical no-follow
path validation.

The current released status path calls `MergeStore::load_archived`, which
returns an owned `MergeOperationRecordV0` through the legacy archived store
reader. The canonical no-follow acquisition that returns a
`ValidatedArchivedMerge` currently lives in the test-gated v1 lifecycle archive
owner. Merely exposing `decode_archived_v0(bytes, expected_merge_id)` leaves P3
with three unacceptable choices: keep the legacy owned-v0 response path and
never use the neutral projection, duplicate canonical acquisition in status,
or promote the v1 lifecycle archive module into a normal build.

The amendment must freeze a normal-build, version-neutral canonical archive
acquisition API (including its exact owner and path), return the same-byte
`ValidatedArchivedRecord`/neutral projection to P3, and state how the existing
open/destination arbitration table reaches it. Its normal build must reject v1
at header classification without compiling the v1 body or cleanup decoder.
Without this bridge, acceptance item 4 is not implementable within the listed
ownership boundary.

### P1-4 — exact direct-ref authority is not required at the final all-absent boundary

Section 6 fixes the initial all-repository preflight and the immediate
per-delete recheck, but says nothing about the final proof immediately before
archive unlink. The current final v1 GC check uses resolving `read_ref`. A
broken symbolic ref can therefore be installed after its direct ref was
deleted; resolving it returns absent, allowing the immutable archive to be
removed while the named non-direct ref still exists. A symref resolving to an
object can similarly change the fact being checked from the named ref to its
referent.

The original GC contract requires every owned ref itself to be absent before
archive deletion. `require_backup_refs_absent` and any v0 equivalent must use
`observe_direct_ref`: only `Absent` succeeds; `Direct` of any target,
`NonDirect`, or an unavailable repository retains the archive. Tests must cover
a resolving symref and a broken symref both before preflight and after a
successful deletion but before the final archive check.

### P1-5 — the 1,100/1,600 correction and path manifest are not auditable

Section 7 provides responsibility categories, not the fixed exact path
manifest required by the controlling R4b-P checkpoint. It omits at least the
unknown-field paths required by P1-2 and does not identify the normal-build
archive-acquisition owner required by P1-3. “Authority observer parents,” “v1
model/canonical/validation,” and “existing publication/root consumers” are not
path-bounded ownership.

The numeric correction is also unreconciled:

- P0 already closed at 1,180/1,200 production lines and 4,121/4,200
  test/tool/doc lines, so this cannot fit as an implicit P0 allowance.
- The controlling P table remains 4,800 production, 2,050 moved, 13,300
  test/tool/doc, 76 charged/63 unique production paths, and 70 charged/69
  unique test paths. The amendment does not say whether 1,100/1,600 replaces
  slice ceilings, increases the package ceiling, or is charged within existing
  P1-P4 headroom.
- The four newly admitted P3 fixture paths are not reflected in a revised P3
  13-path charge or aggregate charged/unique counts.
- No measured rebaseline, per-slice allocation, moved-LOC treatment, generated
  path count, or sub-500-line owner allocation supports the figures.

Before implementation resumes, the amendment must list every exact production
and test/tool/doc path, mark shared charges by slice, publish old ceiling,
measured current charge, correction allowance, and new ceiling for every
affected row, and reconcile the package and aggregate totals. The correction
may be numerically reasonable, but the current document supplies no evidence
from which an independent reviewer can determine that.

## Non-blocking observations

### P2-1 — archive observation opacity needs an explicit constructor/lineage owner

The `CheckedArchiveObservation` direction is sound: P4 acquires exact physical
facts and only the authority subtree issues `BoundExactObservation`. The final
interface should nevertheless name the type's file, constructor visibility,
and exact bound identity (source digest/location, workspace/merge identity,
destination state, and request-relevant terminal state). “Non-constructible”
cannot literally apply to its P4 producer, and a bare enum-shaped API must not
become a consumer-selectable fact. A focused stale-source and cross-merge test
matrix should accompany the path manifest.

### P2-2 — direct rollback's strict physical matrix remains referenced, not frozen

The new preservation compatibility matrix is now explicit and has one
model-owned reuse point, including the degenerate `Marker/Staged` row. That
closes the earlier handoff-matrix concern. Section 2.7 still says the direct
rollback matrix “derives” the allowed complete shape at each evidence step
without listing those file/index before and after rows or naming their single
pure owner. This is weaker than the new preservation treatment. Freeze the
baseline destination matrix or cite one exact already-frozen table/function
that both observation and recovery must use; prohibit separate observer and
recovery matches.

## Accepted portions of the amendment

Subject to the blockers above, these boundaries are suitable:

- `BoundExactObservation::issue` remains authority-private; lane runtimes
  delegate closed observation and never bind facts themselves.
- The durable handoff is installed atomically by all three preservation-entry
  compounds, remains immutable through preserve-then-rollback, and is absent
  on direct rollback.
- The updated owner rule correctly covers both `PublicationRoot` and selected
  participant `@root`, preserving the collision rule.
- The updated action-free proof rule binds cursor-prefix, next-root-action,
  whole-prefix, and exhaustion authority to the exact durable prefix/index.
- The model-owned compatibility matrix closes ordinary and degenerate
  prefix/index pairs without claiming live physical truth.
- A neutral archive projection plus separately gated v0/test-v1 decoder bodies
  can make archived v0 status production-reachable without enabling a pre-A1
  v1 reader.
- Keeping `PublicationProgress.preservation_prefix` unchanged for v0 while
  forbidding it in valid v1 is the correct serialization boundary, once the
  unknown-field/upgrade collision is closed and retained v0 bytes are tested.

## GO conditions

The architecture review can return GO after the amendment itself, before
implementation, does all of the following:

1. supplies the missing exact status-view inputs;
2. closes the new field through canonical and unknown-field/upgrade authority;
3. defines a normal-build canonical archived-v0 acquisition-to-projection API;
4. requires exact named-ref absence at the final archive deletion boundary;
5. freezes the exact path manifest and reconciled slice/package/aggregate
   budgets; and
6. names the archive-observation constructor/lineage owner and the single
   direct-rollback physical matrix owner.

After those contract changes receive both independent GO reviews,
implementation can resume within the revised manifest. The same reviewers
should then inspect normal-build reachability, v0 byte compatibility,
authority constructor visibility, restart matrices, exact ref races, and final
LOC/path accounting on the settled tree before R4b-G integration.

---

# Corrected-amendment architecture re-review

Date: 2026-08-10

## Re-review verdict

**GO.**

The corrected amendment closes all five P1 findings and both P2 observations
from the first architecture review. I find no remaining P0, P1, P2, or P3
interface finding in the durable handoff, authority ownership, production
gating, archive lineage, borrowed status observation, rollback/GC closure,
exact manifests, or reconciled budgets.

This decision approves implementation only within the corrected interface and
manifests. It is not a settled-tree implementation approval and does not enable
a production v1 reader, writer, migration, or dispatcher before A1.

## Prior-finding closure

### P1-1 — borrowed status inputs: closed

`MergeStatusRecordView` now also borrows `operation_id` and `source_ref`.
Section 5 explicitly requires the shared root-evidence observer to reconstruct
the exact composition message from `source_ref`, `merge_id`, and
`operation_id`; it may neither drop that check nor reach back to an owned v0 or
v1 record. Both record versions construct the same non-serializable view, and
all root/publication classifiers have one borrowed implementation.

### P1-2 — canonical and unknown-field authority: closed

The new top-level field and nested journal value are now included in canonical
v1 state, known-field extraction, nested journal extraction/identity, checked
overlay evidence, and the open-v0 adapter boundary. Most importantly, the
field is frozen as the fifth v0-to-v1 top-level collision: a conflicting v0
extension makes migration ineligible. It is rejected, never silently retired.
The adapter writes typed `None` only after absence is proven, and the dedicated
`unknown_fields/tests/v0.rs` path is in the evidence manifest for exhaustive
five-collision rejection.

### P1-3 — normal-build archived-v0 API closure: closed

`record_wire/location.rs` is the sole version-neutral physical acquisition
owner. Its opaque leaf/path vocabulary binds canonical open/archive location,
same-read bytes, and digest without importing `model::v1`. The closed source
arbitration table covers absent, open-only, archive-only terminal v0,
byte-identical terminal copies, nonterminal contradictions, malformed or
unsupported sources, and byte-different copies.

Normal builds use the released v0 open decoder plus `decode_archived_v0`, with
`PRODUCTION_R3` classification and no compiled v1 body or v1 cleanup decoder.
The disabled v1 path reuses only location authority and substitutes the
test-gated decoder. Optimistic status re-acquires the complete location/source
identity, discards stale live facts, retries once, and returns the typed
contention/recovery result rather than leaking an incidental filesystem error.

### P1-4 — final exact direct-ref exhaustion: closed

The new direct-ref observation is required at all three authority boundaries:
the complete all-repository preflight, each immediate checked deletion, and
the final all-absent proof adjacent to archive unlink. Final exhaustion accepts
only `Absent`; any direct target, resolving or broken symbolic ref, other
non-direct form, or unavailable repository retains the archive. The same rule
is explicit for the v0 wrapper, with before-preflight and post-deletion race
tests for both symbolic-ref forms.

### P1-5 — exact manifest and budget reconciliation: closed

The correction now freezes exactly 54 production paths and 43 test/tool/doc
paths. The increase from the initially reviewed 42-path list is necessary and
correct: `record_wire/unknown_fields/tests/v0.rs` owns the exhaustive migration
collision test. P3 separately gains the four named constructor/parity paths.

The arithmetic is internally consistent:

- production: `1,200 + 1,400 + 1,350 + 2,200 + 1,250 + 750 + 100 = 8,250`;
- test/tool/doc: `4,200 + 2,200 + 1,800 + 1,800 + 2,500 + 1,500 + 1,500 = 15,500`;
- charged paths: 130 production and 117 test/tool/doc; and
- conservative R4b-P unions: 89 production and 89 test/tool/doc paths.

The program-wide ceilings are correspondingly reconciled to 16,150 production
lines, 27,200 test/tool/doc lines, and conservative unique unions of 137 and
136. The extra v0 collision-test charge raises the P package union but not the
program union because accepted R3 already charges that physical path. The
pause-point lane measurements, remaining headroom, generated-path treatment,
and shared-path attribution are explicit. The pre-append documentation charge
was corrected to 1,372/2,200. The ledger must mechanically recount the amended
interface and both final review appends before implementation; this memo does
not freeze that later figure.

### P2-1 — opaque archive-observation identity: closed

`authority/observe/archive.rs` now owns both the private
`CheckedArchiveObservation` and its private acquisition constructor. The fact
never leaves the authority subtree and binds source path/digest,
workspace/merge/operation identities, request, terminal state, and destination
path/digest state. Only that owner maps `DestinationAbsent` or
`ExactTerminalCopy` into `BoundExactObservation`; runtimes and P4 consumers
receive no fact or proof constructor. Cross-source, cross-operation,
wrong-request, and reloaded-record cases are explicitly required tests.

### P2-2 — strict direct-rollback matrix and exhaustion: closed

`abort/evidence.rs::classify_v1_evidence_rollback` is the single pure owner of
the full `[Boundary, Lock, Marker, Index]` and exact-head matrix. The amendment
freezes every before/after row for `EvidenceCommit`, `Boundary`, `Lock`,
`Marker`, `Index`, and `Complete`, including the only permitted lock/boundary
no-op equivalence. Preflight, ordinary observation, recovery, and immediate
executor recheck must reuse it; parallel matches are forbidden.

Final selected-root exhaustion is separately bound to one canonical no-follow
observer in `root/abort.rs`. The authority calls it in the same operation that
issues `VerifiedRollbackExhausted`; a prior byte fact, plain `fs::read`, or a
resolving path observation cannot authorize terminal abort.

## Implementation re-review conditions

The settled-tree review must verify the promised semantics rather than only
the signatures. In particular it must inspect normal-build dependency
reachability, handle/path no-follow behavior and race tests, exact unknown-field
survival/collision behavior, authority constructor visibility, both root-owner
handoff restart matrices, reuse of the sole rollback classifier, final
direct-ref absence races, unchanged v0 serialized bytes/retained readers, and
actual per-slice and unique-path accounting. Any manifest substitution,
ceiling breach, consumer-visible proof constructor, or production v1 decode
path reopens this checkpoint.

## Terminal-baseline destination-equivalence re-review

**GO remains.** The temporary removal of `BBBB` from the `Boundary` and `Lock`
after sets was incorrect. A valid direct rollback can enter from
`Candidate { Baseline, Pre }`: its files and index are already `BBBB`, while
its head is still the composition commit. `EvidenceCommit` changes only that
head to the exact baseline parent. The resulting baseline-head plus `BBBB`
state is already the immutable rollback destination. `Boundary` and `Lock`
must therefore classify it as exact `After` and record durable completion;
neither has a legitimate mutation to perform from that state.

The corrected exception is narrow and does not weaken strict step ownership:

- it applies only to terminal `BBBB` with the exact baseline head;
- `BBBB` is not a `Before` state for either step, so neither mutator runs;
- every nonterminal shape belonging only to a future step remains
  `Ambiguous`; and
- candidate boundary or lock byte equality remains the only component-level
  no-op normalization.

This also resolves the apparent tension with the future-step rule: terminal
destination equivalence is possible at multiple durable positions, while a
nonterminal future-step shape still cannot be adopted. The settled-tree review
must include the `Candidate { Baseline, Pre }` trace through every persisted
step and prove that the shared classifier accepts only this terminal
cross-step exception. No architecture finding is opened by the clarification.

## Implementation-start manifest correction

**GO remains.** The earlier 54-path count omitted two normal-build v0
constructors: `record_wire/open_v0/structural.rs` and
`record_wire/archive/v0_evidence.rs`. Each constructs a temporary
`MergeOperationRecordV1` solely to apply shared structural/evidence validation.
Because v0 has no durable publication-handoff source, each constructor must
initialize the new v1-only field to typed `None`.

Charging those one-line initializers does not enable a v1 decoder, writer,
migration, or dispatcher and does not change any semantic or line ceiling. The
correct P0.1 production manifest has 56 distinct paths; R4b-P has 132 charged
and 91 unique production paths. The program-wide union remains 137 because
accepted R3 already charges both physical paths. These figures supersede the
54/130/89 package figures in the earlier closure paragraph. No architecture
finding is opened by this manifest correction.

The parent-visibility audit additionally charges `acceptance/mod.rs` for two
package-private reexports of the existing publication-view classifiers. It
creates no new policy owner or wider visibility. The final production figures
are therefore 57 P0.1 paths and 133 charged/92 unique R4b-P paths; the program
union remains 137 because R4b-A already charges the parent.

The evidence manifest likewise adds `model/v1/validate/common_tests.rs` for its
one-value journal-field constructor update and `unknown_fields/tests/nulls.rs`
for its one-key known-null fixture update. The corrected counts are 45 P0.1
evidence paths and 119 charged/91 unique R4b-P test/tool/doc paths. The program
union remains 136 because accepted R3/TI already charge both files. These
mechanical additions change no semantic or line ceiling and supersede the
earlier 43/117/89 evidence figures.

## Canonical source-location binding correction

**GO remains.** Digest plus workspace/merge/operation IDs is not a complete
authority identity: identical record bytes can exist beneath a different
workspace root. The correction properly makes the opaque, checked
`OpenRecordLocation` part of the single `authority/binding.rs::ProofBinding`.
Exact canonical root and canonical open-record path equality is therefore
required whenever a request, observation/token, physical action, or execution
attempt is consumed.

The central binder is the correct ownership point: callers cannot supply or
rewrite the location, archive destination identity remains an additional fact,
and cloning identical bytes and IDs under another root cannot replay authority.
The normative TransitionDesign definition and equality wording now match the
amendment. The settled-tree review must verify all four authority-bearing
categories reject cross-root reuse through this one binder.

The exact additions are `authority/binding.rs` and existing
`tests/authority.rs`, yielding 58/46 P0.1 paths and R4b-P charged/unique counts
of 134/93 production and 120/92 evidence. Program unions remain 137/136 because
the physical paths are already charged by accepted TI/P0. No semantic or line
ceiling changes, and no architecture finding remains.

### Test-only abort-preflight parent charge

**GO remains.** `abort/preflight.rs` imports the single canonical root-artifact
observer only inside its `cfg(test)` v1 rollback preflight. It neither duplicates
artifact policy nor adds normal-build v1 reachability. Charging the existing
path raises the exact P0.1 production manifest to 59 and the R4b-P production
charge to 135; the package unique union remains 93 because another P slice
already charges the path. No semantic, visibility, or line ceiling changes.

### Internal observation-key layout correction

**GO remains.** `authority/dispatcher.rs` already owns `ObservationKey` and the
P0.1 lifecycle getter. The implemented layout boxes its private
`ObservationKind` payload and then boxes the private `BoundValue<ObservationKey>`
inside the existing `BoundObservationRequest` wrapper. `Box<ObservationKind>`
delegates serialization to the enum, while the outer box is outside the
serialized binding payload, so canonical authority bytes and the
`ProofBinding` hash are unchanged. `V1NextAction::Observe`, destructuring,
visibility, and the borrowed `kind()` API are unchanged; no caller receives box
ownership and no lint suppression or binding weakness is introduced. Focused
all-target Clippy with `clippy::large_enum_variant` denied passes. Charging the
existing path raises P0.1 to 60 production paths and R4b-P to 136 charged
production paths; its 93-path unique union and every line/test ceiling remain
unchanged.
