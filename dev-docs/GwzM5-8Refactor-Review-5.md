# GWZ M5–M8 Merge-Lifecycle Refactor Proposal — Re-review 5

Date: 2026-07-29

Reviewed:

- `dev-docs/GwzM5-8Refactor.md`, revision with SHA-256
  `9ea3dd7aee783c8d2641d8230a3554543a0f66a5454ddaae1f59e03ac1546dd3`;
- reviews 1–4;
- `gwz-core/dev-docs/GwzMergePlan.md`; and
- the current merge wire record, archived-record loader/GC, publication
  progress, finalization, preservation, and rollback implementation.

## Verdict

The fourth-review findings are resolved. The proposal now:

- extracts M4 acceptance semantics before implementing the adapter;
- keeps the R3 production writer and migration dispatch unreachable;
- makes R4b consume persisted acceptance before activation;
- enables writing and migration only through a reviewed A1 gate; and
- defines a closed, progress-aware mapping for open v0 records.

**R0, R1, R2a, and M5 remain approved.**

Two additional compatibility issues remain before I2/A1:

- a P1 same-schema downgrade gap once later M6–M8 semantics are written; and
- a P2 missing contract for already archived v0 records.

Neither affects the immediate M5 implementation.

## Disposition of review 4

| Review-4 finding | Re-review result |
| --- | --- |
| P1-1: R3 preceded the acceptance implementation its writer required | **Resolved.** The sequence is now R4a semantic extraction, disabled-writer R3, R4b persistence/consumption, then reviewed A1 activation. |
| P2-1: v0 adaptation was not publication-progress aware | **Resolved for open records.** §15.3.2 now covers pre-acceptance, candidate, interrupted/recorded evidence, partial publication, preservation, rollback, no-publication, recovery-required, and terminal-before-archive states with exact evidence and next-action rules. |

## Findings

### [P1-1] A1 can read the schema but not the later M6–M8 semantics that use it

**Where:** §§15.1–15.3; A1; M6–M8; §§22.3–22.5.

The proposal deliberately freezes the schema-shaping M6/M8 dimensions at I2,
activates the new record through A1, and implements M6, M7, and M8 afterward.
That makes the A1 binary the first reader/writer of a record version whose full
semantic state space is not yet executable by that binary.

For example, a later binary may write the same schema/version with:

- a pending `CreateBranch`, `SwitchCheckout`, `RestoreCheckout`, or
  `DeleteOwnedBranch` action;
- snapshot provenance and its source-specific planning state; or
- optional participation, a skipped outcome, and partial-composition
  acceptance.

An A1-era binary may know the wire enum definitions because R3 implemented the
frozen schema, so the schema/version pair and deserialization both succeed. It
still lacks the M6–M8 lifecycle implementation needed to continue, preserve,
abort, roll back, finalize, or report those states correctly.

The existing protections do not close this case:

- v0 readers reject v1 by schema mismatch;
- §15.1 covers later *additive fields* that retain a version; but
- these are already-known variants intentionally frozen into the initial new
  schema, not unknown additive fields.

If the A1 binary treats a recognized but unsupported branch/skip action as an
ordinary canonical record, it can choose M4/M5 recovery behavior against a
state it cannot safely own. A generic `writer_version` string is not by itself
a semantic compatibility contract.

**Required correction before I2/A1:**

Choose and freeze one of these strategies:

1. **Version per semantic wave.** A1 writes only the M4/M5 acceptance version;
   M6, M7, or M8 increments the record version whenever it first writes a state
   an older reader cannot execute.
2. **Durable required-capability profile.** Every record contains a closed,
   immutable set or level describing the lifecycle semantics required to
   operate on it. Each binary declares the profiles it fully supports and fails
   closed before mutation when any required capability is unsupported.
3. **Delay A1.** Do not activate the broad record until every semantic variant
   it can express is implemented in the same installed binary.

If capabilities are used, they must be semantic and closed—not arbitrary
strings—and must cover at least:

- integration/acceptance-only records;
- branch lifecycle;
- snapshot source provenance/resolution;
- optional/partial participation and composition; and
- any future preservation/publication behavior that changes recovery.

The capability/profile must be frozen before the first action that requires it
is persisted. It must be validated before status-derived mutation eligibility,
continue, abort, preserve, rollback, finalization, GC that rewrites state, or
record migration. A read-only status path may report the unsupported profile,
but it must not adapt it to a weaker semantic model.

Do not rely solely on matching `writer_version`, because a newer maintenance
writer can remain semantically compatible while an older writer may already
support a capability introduced by another build. If a minimum-compatible
reader version is chosen instead, define its ordering and backport policy
explicitly.

**Required tests:**

- Run the actual A1 binary against open M6 branch-action records, M7 snapshot
  records, and M8 optional/skipped/partial records.
- Cover every start/continue/status/abort/preserve/rollback/finalize state that
  the later feature can persist.
- Assert a typed unsupported-capability/version result before record, ref,
  index, worktree, candidate, preservation, or publication mutation.
- Verify newer binaries continue to read and recover A1 records.
- Verify a later record whose required profile is still supported remains
  readable across a mere writer-version change.
- Include the same-profile older-reader test in every future record-changing
  release gate, even when the schema/version pair is unchanged.

Add a stop condition: no binary may accept an open record merely because it
recognizes the wire variants; it must prove support for the complete durable
semantic profile required by that record.

### [P2-1] Archived v0 records need a separate evidence-only adaptation contract

**Where:** §§15.3, 15.6, and 15.7; R3; §§18.6 and 22.3.

The new §15.3.2 table is deliberately recovery-oriented and covers terminal
records **still open before archive**. It validates live root, participant, and
candidate-prefix observations to choose the next safe action.

The store also retains already archived v0 records under `.gwz/merge/done`.
The proposal says those records remain readable for status/reporting and are
never rewritten merely because they were read, but it does not define their
canonical adaptation.

Archived records differ materially from open recovery:

- the workspace may have advanced through many later operations;
- member repositories or candidate artifacts may no longer be present;
- the root may have moved, been recloned, or become detached;
- an old all-up-to-date archive may not contain every byte needed to construct
  the new complete `AcceptedWorkspace`; and
- an aborted archive may correctly have no accepted workspace at all.

Applying open rows K/M to an archive and consulting live Git would turn normal
historical drift into a false corruption/recovery error. Reconstructing missing
acceptance from the current workspace would fabricate historical evidence.
Rejecting every older archive that lacks new acceptance fields would regress
released `merge --status <id>`/historical reporting. GC and retention also need
to identify terminal archived records without requiring the historical
workspace state to remain live.

**Required correction before R3/A1:**

Define a separate archived-v0 adapter with these rules:

1. It is evidence-only and performs no live Git, worktree, manifest, lock, or
   snapshot observation to establish historical facts.
2. It validates the schema, terminal lifecycle state, internal record
   invariants, and whatever candidate/publication evidence is durably present.
3. It never rewrites or upgrades an archived record on read.
4. It constructs accepted lock/root/audit history only from exact durable bytes
   and evidence already in the archive.
5. When an old archive lacks enough evidence for a new field, it reports that
   field as legacy/unavailable through a typed canonical representation or
   projection; it does not fabricate a value and does not make the whole valid
   archive unreadable merely for incompleteness introduced by the newer model.
6. An aborted archive may truthfully retain no accepted workspace.
7. Explicit GC may delete a structurally valid terminal archive without first
   proving that its historical live Git state still exists.

The canonical model therefore needs to distinguish at least:

- complete persisted current acceptance;
- complete acceptance recoverable from durable legacy candidate/baseline
  bytes;
- legacy terminal evidence for which new acceptance detail is unavailable; and
- no acceptance because the operation aborted before acceptance.

This distinction is for read-only historical projection. It must never make an
open record with missing required acceptance eligible for mutation.

**Required fixtures:**

- completed candidate-bearing v0 archives;
- completed all-up-to-date/no-publication v0 archives;
- aborted archives before and after candidate/evidence work;
- born and unborn root archives;
- archives with optional legacy baseline/candidate bytes absent;
- unknown top-level and nested fields;
- archives after the live root and members have advanced;
- archives after member paths or repositories are unavailable;
- read-only status/projection without byte changes; and
- explicit GC/retention without live-repository validation.

Run those fixtures through Rust core, Rust CLI, Python/native status, JSON, and
JSONL projections. Current-version archives should still expose their complete
accepted workspace; only legacy evidence gaps should project as unavailable.

## Positive assessment

The proposal now has a strong implementation boundary:

- R4a owns acceptance and next-action semantics;
- R3 can exercise complete migration without exposing a production writer;
- R4b installs finalizer consumption before activation;
- A1 is small and independently reviewable;
- open v0 publication recovery has a concrete evidence table;
- persisted candidate bytes remain authoritative;
- born/unborn root progress is stage-aware;
- no-publication completion follows the same acceptance path;
- unknown fields survive migration and later rewrites; and
- later M6/M8 policy cannot shape the record before I1/I2.

The remaining corrections extend that discipline in two directions: forward to
same-schema feature readers, and backward to historical archives.

## Recommended disposition

1. Keep R0, R1, R2a, and M5 approved.
2. Add a record-version or required-capability rule before A1 is approved.
3. Add a separate archived-v0 adaptation/projection table to I2 and R3.
4. Test actual older binaries against later same-schema semantic profiles.
5. Re-review the frozen I1/I2 compatibility contract before R3, as already
   required.

The core architecture does not need to change. The record compatibility model
needs to define both readers that are too old for later semantics and archives
that are too old to contain later evidence.
