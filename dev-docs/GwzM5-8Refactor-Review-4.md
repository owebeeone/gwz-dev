# GWZ M5–M8 Merge-Lifecycle Refactor Proposal — Re-review 4

Date: 2026-07-28

Reviewed:

- `dev-docs/GwzM5-8Refactor.md`, revision with SHA-256
  `75946686ae4491c7885deeaf3f0b6e430fed5f4c8e9a95ea2908de31ee7145f3`;
- reviews 1–3;
- `gwz-core/dev-docs/GwzMergePlan.md`; and
- the current v0 merge record, publication candidate, finalization,
  root-finalization, preservation, abort, and store paths.

## Verdict

The third-review findings are resolved. The proposal now correctly represents
born and unborn roots, freezes acceptance before the publication decision,
archives acceptance on successful no-publication paths, and preserves unknown
fields across subsequent current-version rewrites.

**R0, R1, R2a, and M5 remain approved.**

Two additional issues need correction before I2 authorizes a new record writer:

- one P1 sequencing problem between R3 and R4; and
- one P2 gap in adaptation of v0 records already partway through publication.

Both concern the future record/finalization checkpoint. Neither changes or
delays the approved M5 package.

## Disposition of review 3

| Review-3 finding | Re-review result |
| --- | --- |
| P1-1: accepted root could not represent an unborn root | **Resolved.** `AcceptedRootBase` now distinguishes a born commit from an exact attached-unborn symbolic branch, with checked first-commit and rollback requirements. |
| P2-1: no-publication completion could bypass accepted-workspace freeze | **Resolved.** Acceptance now freezes before the publication-required decision and is archived for all successful no-publication outcomes. |
| P3-1: migrated extensions could disappear on a later rewrite | **Resolved.** §15.5 defines current-version rewrite preservation, a container-retirement table, manifest comparison after every write, and a dedicated test matrix. |

## Findings

### [P1-1] R3 is ordered before the accepted-workspace implementation that its upgrade requires

**Where:** §17 R3 and R4; §15.3; I2.

R3 currently:

- implements the new wire record and version adapters;
- atomically upgrades every validated open v0 record before its next Git
  mutation; and
- changes durable state.

R4, which follows R3, currently:

- extracts complete-lock construction;
- constructs and validates `AcceptedWorkspace`;
- handles born/unborn accepted-root evidence;
- persists acceptance before deciding whether publication is required; and
- changes finalization to consume that evidence.

Those packages are not independent in that order. A new-version adapter cannot
safely upgrade every legal open v0 record without the R4 acceptance logic:

1. A v0 record may already be in `finalizing`.
2. Its candidate may already contain the exact accepted `lock_yaml`.
3. Its root evidence commit may already exist and be recorded.
4. Candidate files may already be partially or completely published.
5. An all-up-to-date record may be at the no-publication completion/archive
   boundary.

The new record contract requires acceptance to be persisted before the
publication-required decision and carried through all of those states. If R3
ships or activates its writer first, it has only three unsafe choices:

- write a current-version record without required acceptance;
- duplicate the not-yet-extracted R4 composition/finalization policy inside the
  compatibility adapter; or
- continue the old finalizer after upgrade, allowing a current-version record
  to complete without satisfying its own acceptance invariants.

The first and third violate the proposed schema. The second recreates the
policy duplication this refactor is intended to remove.

**Required correction before implementation sequencing is approved:**

Choose one explicit package structure, for example:

1. **R4a — behavior-preserving acceptance extraction**
   - extract the pure M4 complete-lock, accepted-root, publication-required,
     and validation functions;
   - characterize every v0 publication state;
   - make no wire-format change.
2. **R3 — record/adapter implementation**
   - use the R4a functions to adapt and validate all legal open v0 states;
   - implement the envelope, writer, migration, and rewrite preservation.
3. **R4b — acceptance persistence and finalizer consumption**
   - persist the accepted workspace at the frozen boundary;
   - route both publication and no-publication finalization through it;
   - complete the responsibility-based module split.

An alternative is to implement R3's decoder and writer behind a hard-disabled
publication gate, with no v1 write or v0 upgrade possible until R4 is complete.
If that route is chosen, R3 is not yet the “record implementation and safe
upgrade” package and its acceptance criteria must say so explicitly.

In either structure:

- no package may write a new-version open record that the currently installed
  finalizer cannot consume under all frozen invariants;
- the new writer must remain disabled until acceptance construction,
  persistence, publication, no-publication completion, abort, and preservation
  all understand the new record; and
- activation of the writer and migration path is one reviewed integration
  gate, not an incidental follow-up.

### [P2-1] The v0 adapter needs a publication-progress-aware acceptance mapping

**Where:** §15.3; R0, R3, and R4; §18.

The v0 adapter now maps baseline and selected-root state to
`AcceptedRootBase::Born` or `Unborn`, but that is only the initial root input.
Legal open v0 records can contain later durable publication evidence:

- `PublicationCandidate.lock_yaml` and its recorded digest;
- the publication branch and root-participation result;
- `composition_commit` and `composition_tree`;
- sorted candidate hashes;
- a partially or fully published candidate prefix;
- preservation evidence or an evidence rollback; and
- a `Complete` publication step immediately before terminal archival.

The live root and filesystem expectations depend on that progress. In
particular:

- before first evidence creation, an accepted `Unborn` root must still be
  attached and unborn;
- after a checked first evidence commit is recorded, that same accepted
  `Unborn` input legitimately has a born live HEAD at the recorded
  `composition_commit`;
- after candidate publication starts, the live lock/boundary may match a
  recorded partial or complete candidate rather than the baseline; and
- when a candidate already exists, its exact persisted `lock_yaml` must become
  the accepted lock bytes. Rebuilding those bytes with the new binary's
  serializer would violate both M4 recovery and the proposal's
  no-regeneration rule.

The statement that unborn adaptation requires an “accepted live symbolic
branch” is therefore necessary but insufficient. A validator that always
requires the live root to remain unborn would reject a legitimate
post-evidence v0 record. A validator that simply accepts a born root would risk
adopting an unrelated commit.

**Required correction before I2 freezes the adapter:**

Define a closed mapping table for every legal v0 operation/publication state.
For each row, freeze:

- whether `AcceptedWorkspace` is absent, can be constructed now, or must be
  recovered from already persisted candidate evidence;
- the authoritative accepted-lock byte source;
- the born/unborn root input;
- the exact allowed live root/ref and candidate-prefix observation;
- the recorded publication outputs that justify a changed live state;
- whether the next action is acceptance freeze, evidence creation, candidate
  publication, verification, no-publication completion, rollback, or archive;
  and
- the typed rejection for contradictory or incomplete evidence.

At minimum, the fixture matrix must include:

1. executing, awaiting-resolution, halted, preserving, and rolling-back records
   before acceptance;
2. finalizing before candidate preparation;
3. candidate persisted but no evidence commit;
4. first evidence commit created but its record write interrupted;
5. evidence commit recorded;
6. each candidate-publication prefix;
7. publication verified/complete before archive;
8. evidence-preserved and evidence-rolled-back states;
9. all-up-to-date no-publication completion; and
10. born and unborn root variants wherever legal.

Tests must prove that:

- an existing candidate's exact lock bytes/hash are adopted and never
  reserialized;
- a born live root is accepted for an originally unborn input only when the
  exact recorded first evidence commit verifies against `parent = None`,
  candidate files, message, tree, branch, and hashes;
- partial publication is reconciled against recorded candidate bytes;
- upgrade followed by restart selects the same next action as the original v0
  implementation; and
- contradictory progress fails before any record or Git mutation.

This mapping belongs in the shared acceptance/compatibility seam from P1-1,
not as a second implementation of finalization policy inside the wire decoder.

## Positive assessment

The architecture remains sound after these corrections:

- the immediate M5 seam stays deliberately small and v0-compatible;
- action, observation, outcome, acceptance, and publication have clear owners;
- branch deletion remains conservative until ownership/ABA proof exists;
- root `--into` remains safely rejected pending an independent authority
  design;
- optional authorization remains distinct from runtime skip outcome;
- exact accepted lock bytes and audit rows are closed durable evidence;
- no-publication completion now uses the same acceptance contract;
- born/unborn root semantics match current checked Git behavior;
- snapshot provenance is immutable without becoming execution authority;
- additive schema reuse has a real previous-reader gate; and
- unknown-field migration, subsequent rewrites, and container retirement are
  explicitly testable.

## Recommended disposition

1. Keep R0, R1, R2a, and M5 approved.
2. Split or gate R3/R4 so acceptance logic exists before any new-version record
   is written.
3. Add the v0 publication-state adaptation table to I2.
4. Make the adapter reuse the extracted acceptance/finalization semantics.
5. Re-review the frozen I1/I2 contracts and the writer-activation gate before
   R3 publishes or upgrades any record.

The proposal does not need another architectural rewrite. It needs the record
and finalization packages ordered so that a durable writer can never get ahead
of the lifecycle semantics required to interpret what it writes.
