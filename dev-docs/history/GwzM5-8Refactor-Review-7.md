# GWZ M5–M8 Merge-Lifecycle Refactor Proposal — Re-review 7

Date: 2026-07-30

Reviewed:

- `dev-docs/GwzM5-8Refactor.md`, revision with SHA-256
  `68512af8c6b84d6fa68f902c721e1a5ab44a8cd5888aad70a1e6a20a6ad501da`;
- reviews 1–6;
- `gwz-core/dev-docs/GwzMergePlan.md`; and
- the current merge wire record, archived-record loader/retention/GC,
  publication progress, finalization, preservation, and rollback
  implementation.

## Verdict

The review-6 finding is resolved. The proposal now has one core-owned,
floor-aware creation rule, an exact release-by-feature creation matrix, and
activation tests that distinguish new-operation selection from existing-record
continuation and the single v0-to-v1 migration exception.

**R0, R1, R2a, and M5 remain approved.**

Two narrower compatibility gaps remain:

- a P2 missing archive projection for supported but non-current v1–v3 records;
  and
- a P3 overlap in the envelope error-classification table.

Neither affects the immediate M5 scope. The archive gap should be closed in
the common record architecture before A2 makes v1 a historical version. The
envelope classification should be made exact before I2 freezes its typed error
contract.

## Disposition of review 6

| Review-6 finding | Re-review result |
| --- | --- |
| P1-1: new-operation version selection had no active-writer floor | **Resolved.** §15.1.1 now defines `max(active_writer_floor, highest_requested_semantic_version)`, gives M5 a v0 floor and A1-and-later a v1 floor, assigns selection to one pure core function, distinguishes existing-record handling, and threads the exact matrix through I2, A1–A4, tests, guardrails, stop conditions, acceptance criteria, and the final decision. |

## Findings

### [P2-1] The archive projection omits supported older v1–v3 records

**Where:** §15.3.3; §15.7; A2–A4; §§18.7–18.8; §§22.4–22.6.

The archive design now explicitly covers:

- legacy v0 archives through `LegacyComplete`, `LegacyUnavailable`, and
  `NotAccepted`; and
- a “current-version” terminal archive with persisted acceptance through
  `Current`.

That is complete for A1, where v0 is legacy and v1 is current. It is no longer
complete after the next activation:

- under A2, completed v1 archives are supported but no longer current;
- under A3, v1 and v2 archives are supported but non-current; and
- under A4, v1–v3 archives are supported but non-current.

Those records are not legacy v0 records: they already contain persisted
`AcceptedWorkspace` evidence and must not be degraded to the v0 availability
adapter. They are also not literally “current-version” records under the
existing AV0-A wording. The proposal requires newer binaries to recover older
open versions, but it does not give the archived loader, status projection,
retention, or GC an equivalent version-by-version contract.

A concrete failure path is:

1. A1 completes and archives a v1 merge.
2. The user upgrades to A2.
3. `gwz merge --status <v1-id>` loads an archive that is neither archived v0
   nor current v2 under the written mapping.

An implementation could reject the valid history, route it through the wrong
legacy adapter, or silently treat “current” as meaning “any supported persisted
version.” Only the last behavior is desirable, but it is not what the frozen
type and table presently say.

This also matters to retention. A newer binary must be able to validate and
classify preservation ownership in supported older archives without consulting
live repositories, rewriting the archive, or weakening the older version's
invariants.

**Required correction:**

Define the archived path for every supported durable version:

1. v0 uses the existing evidence-only legacy adapter.
2. Each supported v1–v4 archive is decoded and validated by its own
   version-specific archive decoder.
3. A supported archive with complete persisted acceptance projects that
   acceptance into the current read-only protocol model without live
   observation or archive rewrite.
4. An unsupported future archive returns `UnsupportedRecordVersion` before
   targeted GC or any other mutation.
5. A structurally contradictory supported archive returns
   `ArchivedRecordUnreadable`.

Consider renaming `Current` to `Persisted` or `SupportedPersisted`, and include
the source record version in the projection. That makes clear that “current”
describes the evidence quality, not equality with the installed writer's
highest version:

```rust
Persisted {
    source_version: RecordVersion,
    workspace: AcceptedWorkspace,
}
```

If version-specific canonical accepted-workspace types differ, the adapter must
define the lossless read-only projection for each version. It must not invent a
later feature's branch, snapshot, or participation facts. A supported older
archive may expose its complete older semantic shape plus explicit
version-appropriate absence; it must not be treated as incomplete merely
because a later version added inapplicable semantics.

Add this rule to A2, A3, and A4 activation gates, not only to the eventual final
acceptance criteria.

**Required tests:**

- A2 reads, renders, retains, and explicitly GCs valid v1 archives.
- A3 does the same for v1 and v2 archives.
- A4 does the same for v1, v2, and v3 archives.
- Core, Rust CLI, Python/native, JSON, and JSONL agree on the source version,
  persisted acceptance, operation outcome, and version-appropriate fields.
- Reads perform no Git, manifest, lock, snapshot, member-repository, or
  filesystem-artifact observation and leave archive bytes unchanged.
- A supported older archive with contradictory durable evidence is unreadable,
  not legacy-unavailable.
- An unsupported future archive is not deleted or rewritten by an older
  binary's status, retention, or targeted GC path.

### [P3-1] The envelope table has overlapping mismatch classifications

**Where:** §15.2; §15.7; §§18.8–18.9.

The allowed envelope matrix contains both:

- “known string with wrong number” → typed record-unreadable; and
- “unknown string or future number” → `UnsupportedRecordVersion`.

A record with:

```yaml
schema: gwz.merge-operation/v1
record_schema_version: 99
```

matches both descriptions: the schema string is known and paired with the
wrong number, while the number is also future. The same ambiguity exists for a
future number paired with any already recognized schema string.

Both outcomes fail closed, so the mutation-safety property remains intact.
However, they produce different machine codes and recovery guidance:
`UnsupportedRecordVersion` tells the user to use a newer binary, while
record-unreadable identifies a contradictory/corrupt envelope. The existing
table cannot support deterministic core/driver fixtures for that distinction.

**Required correction:**

Replace the overlapping rows with an ordered, disjoint classification such as:

1. missing, duplicate, non-scalar, or wrong-type header fields →
   record-unreadable;
2. an exact schema/version pair supported by this binary → decode that body;
3. an exact recognized future pair not supported by this binary →
   `UnsupportedRecordVersion`;
4. a recognized schema string paired with any number other than its assigned
   number → record-unreadable; and
5. a genuinely unknown schema/version pair → the explicitly chosen unknown
   version result, normally `UnsupportedRecordVersion`.

State how the dispatcher recognizes future exact pairs without compiling their
bodies, as the proposal already requires for A1's v2–v4 rejection. For future
pairs not yet allocated, reporting the raw pair with no claimed minimum
supporting wave is more accurate than inventing one.

Add a cross-product fixture covering:

- every supported exact pair;
- every recognized-but-unsupported exact pair;
- each known schema string with every wrong known/future number;
- unknown strings with known and unknown numbers; and
- missing, duplicate, non-scalar, negative, and out-of-range version values.

Assert identical typed classification in core, Rust CLI, Python/native, JSON,
and JSONL, with no body adaptation or mutation on every rejection.

## Positive assessment

The writer-floor remediation is complete rather than local. It now prevents
both failure modes:

- A1-and-later cannot accidentally create legacy v0 operations for simple
  requests; and
- newer binaries cannot opportunistically raise an existing operation to the
  newest version they understand.

The formula, pure core ownership, rejection rather than clamping, combined
feature handling, durable pre-action freeze, creation matrix, and fault test
form a coherent contract.

The wider architecture also remains strong:

- semantic-wave versions prevent older binaries from accepting lifecycle state
  they cannot execute;
- R4a/R3/R4b/A1 sequencing keeps the writer behind the installed finalizer;
- open v0 recovery remains progress-aware and next-action equivalent;
- archived v0 history remains evidence-only;
- accepted lock bytes are never regenerated after their authority boundary;
  and
- unknown fields survive migration and ordinary supported-version rewrites.

## Recommended disposition

1. Keep R0, R1, R2a, and M5 approved.
2. Define supported older-version archive projection and add it to A2–A4
   activation matrices.
3. Make the schema/version envelope classification disjoint before I2 freezes
   the decoder contract.
4. Re-review the frozen I1/I2 contract before R3, as already required.

The review-6 version-selection correction needs no further change. The next
revision should generalize historical projection across v1–v4 and remove the
last ambiguity from the otherwise sound fail-closed envelope dispatcher.
