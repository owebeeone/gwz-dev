# GWZ I2 v1 Compatibility and Projection Contract

Date: 2026-08-04

Status: **accepted; R4a unblocked and R3 remains sequenced after R4a**

Amended 2026-08-10 by `GwzM5-8R4bInterfaceAmendment-1.md` §4: the normal-build
boundary is split by capability, so the archived-v0 decoder, neutral archive
projection, and canonical record-location acquisition are normal-build code
consumed by ordinary status and GC, while v1 body decoding, open-v1 helpers,
and migration remain test-gated until A1. §9 below describes the superseded
test-only form; this document is boundary-authoritative only as amended by
that accepted interface.

This document freezes v1 envelope dispatch, v0 adaptation/migration,
archive-only projection, protocol allocation, retained-reader evidence, and
activation boundaries. The companion `GwzM5-8I2RecordContract.md` freezes the
v1 body and accepted workspace; `GwzM5-8I2ActionJournalContract.md` freezes
mutation ownership; `GwzM5-8I2ProtocolContract.md` freezes projection; and
`../gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json` is the checked
core-owned migration-whitelist registry.

## 1. Envelope registry and dispatch

Every A1-or-later binary carries this header-only allocation registry;
retained pre-A1 readers keep their released behavior:

| Exact pair | Required implementation |
| --- | --- |
| `gwz.merge-operation/v0` / `0` | M4/M5a v0 |
| `gwz.merge-operation/v1` / `1` | A1 v1 acceptance/no-ff |
| `gwz.merge-operation/v2` / `2` | A2/M6 branch lifecycle |
| `gwz.merge-operation/v3` / `3` | A3/M7 snapshot source |
| `gwz.merge-operation/v4` / `4` | A4/M8 partial composition |

A1 compiles body decoders and canonical lifecycle support only for v0 and v1.
V2–v4 registry entries classify unsupported exact pairs; they do not create a
body type, archive variant, protocol discriminant, or dormant action.

The decoder parses YAML once into a duplicate-aware raw mapping. Before body
deserialization it applies this ordered, disjoint classification:

1. Missing, duplicate, non-string `schema`, non-integer version, negative
   version, or version above `u32::MAX` is location-specific record-unreadable.
2. A recognized schema string paired with any number other than its allocated
   number is location-specific record-unreadable.
3. An exact allocated and installed pair dispatches to that version's decoder.
4. An exact allocated but uninstalled pair is `UnsupportedRecordVersion` and
   reports its required wave.
5. A genuinely unknown schema with any valid `u32` is
   `UnsupportedRecordVersion` with no claimed wave.

Before body decode, duplicate mapping keys at any depth, YAML aliases/anchors,
and `<<` merge keys are rejected as location-specific record-unreadable.
Unknown fields may use ordinary scalar, sequence, and uniquely keyed mapping
values. The body is decoded from the same raw mapping; no second parser,
independent string/number interpretation, fallback decoder, or best-effort
coercion is allowed. Header validation precedes status eligibility, migration,
record rewrite, archive deletion, and all Git/filesystem mutation.

## 2. Version selection and writer floor

`RecordVersion` is a closed core enum. One pure function computes:

```text
max(active_writer_floor, highest_requested_semantic_version)
```

The creation matrix is:

| Installed writer | ordinary/custom | no-ff | branch lifecycle | snapshot source | partial composition |
| --- | ---: | ---: | ---: | ---: | ---: |
| M5a | v0 | unsupported | unsupported | unsupported | unsupported |
| A1 | v1 | v1 | unsupported | unsupported | unsupported |
| A2/M6 | v1 | v1 | v2 | unsupported | unsupported |
| A3/M7 | v1 | v1 | v2 | v3 | unsupported |
| A4/M8 | v1 | v1 | v2 | v3 | v4 |

The core derives the semantic version from immutable typed request intent.
Drivers never choose it. Unsupported requested semantics reject before record
creation. The chosen version is frozen before the first mutation.

> **[Dated annotation, 2026-08-25 — A1 row partial engagement. Frozen text
> above unchanged.]** The shipped A1 activation engages this matrix's A1 row
> partially: the pure function `max(active_writer_floor, semantic)` is
> production and `no-ff → v1` is live end-to-end, but `ACTIVE_WRITER_FLOOR`
> remains `V0` (`gwz-core src/workspace_ops/merge/model/version.rs:39`, doc
> comment states it), so **ordinary/custom starts continue to create v0**
> until the production v1 ordinary-start owner lands (root participants,
> dry-run prediction, drift/conflict response surfaces, the v0 event
> stream). Engaging `V1` at activation was measured breaking every ordinary
> start; the under-flip is the conservative side of this row (no reader can
> regress on a record that was never written). Ruled an ACCEPTED NAMED
> RESIDUAL by the A1 activation review, Safety axis round 2
> (`GwzM5-8A1Activation-ReviewSafety.md` §R2.2), with binding conditions:
> the remainder is owned by milestone **M5c** and the floor raise lands
> with M5c as ONE reviewed change per §9's discipline; the release-time
> retained-reader manifest describes the SHIPPED behaviour (ordinary=v0,
> no-ff=v1), not this row, until M5c lands. Record:
> `GwzM5-8A1ActivationRecord.md`.

Existing v1+ records never change version. Open v0 records may use only the
explicit A1 migration path. Archived records never migrate. `writer_version`
means the last binary that successfully wrote the record. Every mutating
migration or current-version rewrite sets it to the current binary version in
the same atomic record write. V0-to-v1 migration therefore sets the A1 binary
version. Read-only status and archive projection leave it byte-exact, and it
never affects dispatch, compatibility, or migration eligibility.

## 3. Pinned error allocations and projections

`GwzM5-8I2ProtocolContract.md` freezes codes 46–61, `GwzError` field 7
structured record context, exact reason/message corpus, location-aware malformed
record projection, all taut fields/discriminants, and Rust/Python/JSON/JSONL
parity. A terminal archived v0 `mode: no_ff` is decoded read-only from actual
durable result evidence; only open migration/resume uses
`UnsupportedLegacyMode`.

## 4. Open v0 compatibility result

Structural validity and migration eligibility are separate. The v0 decoder
first validates the complete v0 row. A malformed or contradictory row receives
its typed error. A structurally valid row then follows exactly one of two paths:

- exact migration-whitelist match; or
- valid-unlisted legacy-v0 handling.

The core-owned registry is an equality matcher over a recursively closed,
relation-normalized descriptor. Non-root ids are alpha-normalized in selection
order, while duplicate/collision checks and all record-to-live relations are
verified before normalization. Fixture address and classification are excluded
from semantic identity. Each rule has one canonical descriptor hash and one
live Rust producer binding; the checker enforces one classification per
descriptor and bidirectional rule/corpus coverage.

A1 deliberately whitelists only seven one-member-workspace,
`Finalizing`, normal-mode shapes: before publication progress, validating,
candidate persisted, evidence created but unrecorded, evidence recorded,
exact boundary/index publication prefix, and no-publication complete before
the terminal state write. Every selected result, baseline byte/hash,
candidate/composition relation, participant HEAD/ref/index/worktree, and root
observation is exact. Marker/lock-only prefixes, multi-member workspaces, selected
root, born root, drift, pending actions, recovery, preservation, rollback, and
terminal rows are not A1 migration rules.

Durable phase selection is record-only. Live Git is a separate observation and
cannot select an earlier durable phase. In particular, evidence-unrecorded is
the durable candidate-at-committing-evidence shape with no persisted
composition; exact base versus one exact unrecorded evidence commit belongs
only to observation/matching.

Migration constructs or recovers `AcceptedWorkspace` in the migration write.
It is not a later lifecycle action. The first v1 restart therefore retains the
v0-equivalent next action: validate results, create/adopt evidence, publish the
remaining candidate prefix, or complete the no-publication terminal write.
Persisted candidate bytes are never reserialized.

## 5. Migration eligibility and atomic boundary

An open v0 record is eligible only when all are true:

- it is structurally valid and its normalized descriptor equals exactly one
  migration-whitelist rule;
- its mode is not `no_ff`;
- the v1 canonical model preserves the identical lifecycle state, participant
  state, drift/recovery result, evidence, and next action; and
- every unknown field has the exact surviving path required by the record
  contract.

No v0 `RecoveryRequired`, `Preserving`, or `RollingBack` row is A1
migration-eligible. The adapter never invents a recovery origin or reverse
owner from an unjournaled v0 window.

Migration is representation-only: it performs no Git, ref, index, worktree,
candidate, publication, preservation, archive, or lifecycle action. Read-only
status never migrates.

Zero whitelist matches is not an error. Open read-only status leaves bytes
unchanged and projects v0 source/version with acceptance and recovery absent.
An existing mutating v0 command remains on the existing v0 lifecycle and may
write v0 only when that path's existing preflight authorizes it. Completed and
aborted v0 records remain v0 and use byte-preserving archival. Archived v0 uses
only the archive decoder/projection below. A v0 `mode: no_ff` open row remains
`UnsupportedLegacyMode` before resume/mutation.

An unreadable/contradictory row, unknown-field collision, or exact whitelist
row without evidence needed for its lossless canonical mapping rejects before
staging. Multiple whitelist matches are a build/registry defect; the checker
forbids them.

The migration sequence is validate/adapt, serialize a unique staged file,
fsync it, decode and compare its canonical model/unknown manifest, compute its
ephemeral byte hash, atomically replace the open record, fsync the directory,
reread exact bytes, compare the hash, decode again, and compare the same model
and unknown manifest. The rename plus directory fsync is the durable boundary.

Fault injection proves restart sees either complete valid v0 or complete valid
v1. A crash after rename and before same-process reread resumes as an ordinary
v1 record without an upgrade journal. Temporary files are never discovered as
open records.

A2–A4 retain the same v0 decoder and this same eligible v0-to-v1 migration;
they never jump v0 directly to their current maximum version. Existing v1+
records never auto-upgrade.

## 6. Archive and open-record protocol projection

`GwzM5-8I2ProtocolContract.md` freezes optional `MergeResponse.record` field
10, installed-v1/legacy acceptance wrappers, accepted metadata/member/root
payloads, recovery origin/base/current/resume action, exact field numbers and
variant legality, ordering, omission/null rules, and response-presence rules.
No v2–v4 body or discriminant is allocated at I2.

## 7. Archive decoder and GC rules

Archive projection reads only the exact done-record bytes. It performs no Git,
manifest, lock-file, member-repository, root, snapshot, candidate-destination,
or worktree observation and never rewrites the archive.

The v0 decoder first rejects structural contradictions, then chooses candidate,
baseline-no-publication, or pre-acceptance history, then computes every gap.
`legacy_complete` is legal only when the gap set is empty:

- candidate history requires exact candidate lock bytes/hash, a complete audit
  domain, exact root input, and evidence/publication relationships appropriate
  to the terminal outcome;
- baseline-no-publication requires exact baseline lock and manifest bytes,
  complete verified unchanged participants, and an exact born/detached/unborn
  root input; and
- aborted history with no candidate/acceptance evidence is `not_accepted`;
  partial but internally consistent candidate history is `legacy_unavailable`.

For an unselected root, audit domain uses exact baseline manifest/lock. For a
selected root, it requires the exact result-commit manifest/lock or equivalent
durable domain proof. V0 candidate bytes alone do not contain that manifest;
such an archive therefore includes `complete_member_audit` unless its own
bytes independently prove the full domain. Operation-baseline manifest is
never substituted. Missing optional historical evidence produces every
applicable closed gap. Contradictory identity, terminal state, digest,
participant, candidate, publication, rollback, or acceptance evidence produces
`ArchivedRecordUnreadable`, not a gap.

Archive decoding/status remains live-independent. Deletion has a separate
ownership gate. An archive with no backup refs may be deleted after validation.
For each recorded ref, absent is already complete, its exact recorded target
authorizes checked deletion, and a mismatch or unavailable repository retains
the archive. Delete the archive only after all owned refs are observed absent.
Native stashes and stash bundles are retained. Ordinary retention remains
archive-only for classification and exempts records owning backup refs.
Unsupported or corrupt archives are never deleted or rewritten.

Rust core, Rust CLI, Python/native, human, JSON, and JSONL must project the
same record, terminal outcome, acceptance discriminant, gaps, and fields.

## 8. Retained-reader support window

The machine-readable manifest under
`gwz-core/scripts/retained_readers/manifest.json` is the reproducible
authority for exact tags, URLs, digests, ABI/runtime, platform support, command
availability, and unsupported tuples. I2 freezes these selection rules:

- retain the latest successfully published pre-change release for each
  materially distinct durable decode generation;
- retain both distributed Rust CLI and `gwz-py` wheel/native surfaces;
- keep v0.9.2 as the separate pre-record lane, never as a v0 decoder;
- currently pin v0.10.2 as the durable-v0 baseline, including its known
  Windows evidence, until the manifest advances under review;
- add the published M5a/A1 generation rather than naming an unbuilt local
  substitute;
- run the full behavioral matrix on Linux x86_64 and Windows x86_64; and
- require release artifact/import/protocol/smoke evidence for macOS and Linux
  arm64, with explicit unsupported tuples rather than silent omission.

Artifacts are immutable release downloads verified by SHA-256 and cached by
digest. Missing artifacts, runtimes, commands, timeouts, unexpected results,
or failed evidence checks fail the gate; they never skip. Current-code parity
tests supplement but do not replace retained artifacts.

The fixture matrix covers status, continue, abort, preserve, and GC wherever
the retained command exists; exact exit/stdout/stderr, JSON/JSONL, record
bytes, refs, index, worktree, candidate, preservation, and post-state are
compared. A same-version newly writable shape must behave exactly or reject
before mutation on every retained decode generation.

The manifest machine-encodes the complete envelope matrix for each retained
reader. V0.9.2 has no record dispatcher and supports no pair. V0.10.2 supports
exactly `gwz.merge-operation/v0`/`0` and must reject v1, v2, v3, v4, a
recognized schema with the wrong number, and one genuinely unknown valid
future pair. A1 supports v0 through its adapter and installed v1; A2 adds v2;
A3 adds v3; A4 adds v4. Every unsupported pair has an exact fixture and
expected exit/stdout/stderr/mutation result for each available record-touching
command and both distributed surfaces.

## 9. Disabled implementation and activation

R3 may implement v0/v1 decoders, adapters, archive projection, atomic
migration, and v1 serialization behind test-only entry points. Production
creation remains v0 and production migration has no call path. This is a
compile/call-graph boundary, not a runtime flag.

R4b must first make every finalization, restart, preservation, rollback,
abort, no-publication, completion, and archive path consume persisted
acceptance. M5b then installs deterministic no-ff while start remains
unreachable. A1 alone activates the v1 writer floor, eligible v0 migration,
and public no-ff surface in one reviewed change.

I2 is complete only after two independent reviews find no P0–P3 issue and the
record/protocol contracts, R0 fixtures, retained-reader manifest, and merge
plan agree. The accepted I1/I2 contracts receive another explicit review
before R3 begins.
