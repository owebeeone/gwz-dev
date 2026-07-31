# GWZ M5–M8 Merge-Lifecycle Refactor Proposal — Re-review

Date: 2026-07-28

Reviewed:

- `dev-docs/GwzM5-8Refactor.md`, revised proposal;
- `dev-docs/GwzM5-8Refactor-Review.md`, first review;
- `gwz-core/dev-docs/GwzMergePlan.md`, especially the accepted M4 state and
  planned M5–M8 waves; and
- the current v0 merge-record model and store in
  `gwz-core/src/workspace_ops/merge/model.rs` and
  `gwz-core/src/workspace_ops/merge/store.rs`.

## Verdict

**Approve the proposal for its stated immediate scope: R0, R1, the minimal R2a
integration seam, and M5.**

The revision materially resolves all seven prioritized findings from the first
review. It no longer authorizes a speculative v1 record, correctly separates
M5 from the schema-shaping M6/M8 work, and installs explicit interface and
record checkpoints before broader durable-state changes.

There are no P0 or P1 findings in the work now authorized by the proposal.

Before I2 authorizes R3 or any broader record-changing implementation, resolve
the two P2 checkpoint findings below. They do not block R0–R2a or M5 because
those packages preserve the v0 record and existing recovery semantics. The two
P3 comments should be closed while the corresponding interface is being
frozen.

## Disposition of the first review

| Prior finding | Re-review result | Revised proposal |
| --- | --- | --- |
| P1-1: rollback and cleanup mutations missing from the action model | **Resolved** | §7.1 now includes typed integration rollback, checkout restoration, and owned-branch deletion actions; preservation and publication retain separately named durable owners. |
| P1-2: accepted composition did not describe the complete M4 result | **Resolved** | §9 now requires a complete `LockArtifact`, exact YAML/hash, audit entries for selected and unselected members, distinct integration and final-checkout facts, and a separate root-publication input. |
| P1-3: request authorization and runtime skip outcome were conflated | **Resolved** | §8 separates immutable `ParticipationIntent` from `ParticipationOutcome`, retains undecided state, and requires validation of every skip against its frozen authorization. |
| P1-4: R2/R3 froze speculative M6/M8 types too early | **Resolved** | §§1, 15, and 17 split out R2a, permit M5 to ship on v0, and block broader record work on I1 and I2. §16.5 defines independent proof gates. |
| P2-1: branch ownership evidence was undefined | **Resolved** | §7.2 adopts the conservative no-delete rule after an interrupted, unrecorded creation and requires concurrency/ABA proof before automatic deletion can be enabled. |
| P2-2: root `--into` lacked stable storage and metadata authority | **Resolved** | §7.6 rejects `--into` selection-wide when `@root` is selected in the first M6 release and enumerates the design requirements for enabling it later. |
| P2-3: record decoder/migration compatibility was incomplete | **Resolved, subject to the new P2-1 below** | §15 now defines envelope-first version dispatch, an exact schema/version matrix, version-specific adapters, recursive unknown-field preservation or rejection, staged and published verification, and downgrade behavior. |

## Findings

### [P2-1] “Strictly additive v0” needs an old-reader safety gate

**Where:** §15, especially lines 833–841; §15.5; I2 and R3.

The proposal correctly defers the choice between additive v0 and a new record
version. It does not yet define the criterion under which additive v0 is safe.
“Strictly additive” and “semantically unambiguous” are not sufficient durable
compatibility rules.

The current v0 reader:

- deserializes directly into the v0 `MergeOperationRecord`;
- accepts the record when the existing schema string and numeric version still
  match; and
- deliberately captures or carries unknown fields.

Consequently, an older binary can successfully open a newer record carrying
unknown additive fields. If those fields affect action selection,
participation, checkout ownership, rollback, or composition, the old binary may
ignore them and continue, abort, preserve, or rewrite the operation according
to obsolete M4 semantics. Unknown-field preservation prevents data loss, but it
does not prevent unsafe execution.

This differs from the proposed v1 behavior: an old v0-only binary sees the v1
header mismatch and fails closed. §15.5 specifies that safe case but does not
cover an additive record that still advertises itself as v0.

**Required correction before I2:**

Define additive-v0 eligibility as an old-reader behavioral property:

1. every supported released v0 reader must either reject the new record before
   mutation or behave exactly as it did for the equivalent old record;
2. no additive field may alter mutation selection, legal transitions,
   rollback/preservation scope, workspace acceptance, or publication while
   remaining invisible to an old reader;
3. compatibility fixtures must run the previous release binary against every
   new open-record shape and exercise status, continue, abort, preserve, and GC
   as applicable; and
4. if those properties cannot be proved, the record version must change.

In practice, R2a/M5 can remain v0 because the proposal requires them to reuse
the existing integration record. M6/M8 fields that change durable lifecycle
meaning should be presumed to require a new version unless the old-reader
tests prove otherwise.

### [P2-2] I2 must decide whether `AcceptedWorkspace` is durable evidence or a canonical derived view

**Where:** §9 lines 587–600; §17 R4; I2; §22.3.

The proposal currently says the accepted workspace may be “persist[ed] or
deterministically reconstruct[ed],” while R4 similarly permits it to be
persisted or deterministically validated. Those are different recovery
contracts and can produce different record schemas.

`AcceptedLock` contains three representations of the same result:

- parsed `LockArtifact`;
- exact YAML bytes; and
- a SHA-256 digest.

`member_audit` then supplies another projection of every lock row. If the
accepted workspace is persisted, the record needs closed cross-field
invariants and migration rules. If it is derived, the proposal must identify
only durable inputs, freeze the derivation/canonical-serialization version, and
prove that no mutable live Git or snapshot state can change the result after a
restart or binary upgrade.

Without that decision, a crash between participant acceptance and candidate
publication can be resumed under a different serializer or derivation rule,
changing the supposedly exact lock YAML/hash. Re-reading live checkouts could
also turn a previously accepted result into a different candidate rather than
typed drift.

**Required correction before I2:**

Choose one of these contracts:

- persist the complete validated `AcceptedWorkspace` before the first
  publication mutation; or
- define it as a versioned canonical function of explicitly enumerated,
  immutable durable inputs and persist the resulting exact lock bytes/hash at
  the publication boundary.

In either form, require the validator to prove:

1. `sha256(exact_yaml) == sha256`;
2. parsing `exact_yaml` yields exactly `artifact`;
3. every `Present` audit row is identical to its complete lock row and every
   `Absent` row is absent;
4. the audit and lock domains satisfy the approved M8 selected, unselected,
   baseline, active, and intentionally absent rules;
5. root input is not derived from publication-created output; and
6. restart after acceptance either reproduces the exact accepted bytes or
   reports typed drift without publishing.

Add a fault point immediately after accepted-workspace persistence/freeze and
before the first candidate/publication mutation.

### [P3-1] Clarify the durable meaning of post-rename upgrade verification

**Where:** §15.4.

The upgrade sequence says to record the staged-byte hash, atomically replace
the open record, reread the published bytes, and compare them with the expected
model and unknown-field manifest. It does not say where the expected hash,
model, and manifest live across the explicit crash point after rename but
before post-publication verification.

This can be a documentation issue rather than a need for another journal. If
the fully verified staged file is the durable proof and atomic rename publishes
those exact bytes, say so. Then a restart should envelope-decode and validate
the published v1 record as an ordinary current record; the same-process
post-rename reread is an additional I/O check, not a durable migration phase.

If post-rename comparison is instead required before the upgrade is considered
complete, persist enough upgrade-transaction evidence outside the replaced
record to resume that comparison. Do not leave “record the staged-byte hash”
ambiguous or imply that a record can contain a hash of its own complete bytes.

### [P3-2] Snapshot provenance should say whether the snapshot ID is evidence or display metadata

**Where:** §6 `SourceProvenance`; §12; M7.

`SourceProvenance::Snapshot` currently stores only `snapshot_id`. Snapshot
artifacts can be replaced under the same ID, so the ID alone does not identify
the artifact read during preflight.

The exact per-participant commits already make execution restart-safe, and the
proposal correctly forbids rereading the snapshot for recovery. The remaining
question is audit semantics:

- if provenance is display metadata, state that the exact commits—not the
  mutable snapshot ID—are the authoritative evidence; or
- if provenance is intended to prove which snapshot artifact was accepted,
  freeze at least its content hash, schema/workspace identity, and snapshot ID.

Whichever contract is chosen, add a test that replaces the snapshot contents
under the same ID after preflight and confirms both safe recovery and truthful
status/audit output.

## Positive observations

- The proposal now distinguishes immediate authorization from eventual
  architecture. This is the most important improvement in the revision.
- The conservative branch no-delete default is appropriate. It avoids claiming
  that a matching name and commit prove ownership.
- Separating participant actions, preservation actions, and publication
  progress accurately reflects the current implementation rather than forcing
  unrelated mutations into one generic journal.
- The participation-intent/outcome split is sufficient to validate optional
  behavior after restart without erasing the user's original authorization.
- The complete-lock and root-input/output split gives M8 a viable containment
  boundary: policy constructs one accepted result and publication applies it.
- Envelope-first dispatch and recursive unknown-field accounting preserve the
  important v0 store guarantees.
- The independent proof gates correctly prevent M5 from being treated as proof
  of branch ownership, snapshot provenance, or partial composition.
- The proposed module boundaries are responsibility-driven and avoid another
  mechanical god-file split.

## Recommended disposition

1. Accept this revision as the working refactor plan.
2. Proceed with R0, R1, R2a, and M5.
3. During I1, close the M6 and M8 product decisions already listed by the
   proposal.
4. Before approving I2, incorporate P2-1 and P2-2 into the record-checkpoint
   exit criteria.
5. Resolve P3-1 while specifying R3 and P3-2 while specifying M7.
6. Re-review the frozen I1/I2 contracts before any broader record writer ships.

The proposal is now appropriately phased. The remaining issues are located at
the future durable-record boundary, not in the next implementation package.
