# GWZ M5–M8 Merge-Lifecycle Refactor Proposal — Re-review 6

Date: 2026-07-30

Reviewed:

- `dev-docs/GwzM5-8Refactor.md`, revision with SHA-256
  `fb7740a28f97c12184682336414b2efd2422c48299409cedc844a9c324aaee1d`;
- reviews 1–5;
- `gwz-core/dev-docs/GwzMergePlan.md`; and
- the current merge wire record, archived-record loader/retention/GC,
  publication progress, finalization, preservation, and rollback
  implementation.

## Verdict

Both review-5 findings are substantially resolved.

The proposal now:

- assigns a distinct cumulative durable version to each semantic wave;
- keeps later executable variants out of the A1 wire and canonical model;
- gates v2–v4 writers behind retained older-binary matrices;
- treats `writer_version` as diagnostic rather than semantic authority; and
- gives archived v0 records a separate, evidence-only, archive-byte projection
  with typed availability and corruption outcomes.

**R0, R1, R2a, and M5 remain approved.**

One new P1 contract inconsistency remains before I2/A1: the proposal does not
define the minimum version that a newly installed writer must emit. Its
“lowest sufficient version” rule selects v0 for an ordinary M4/M5 operation,
while A1 explicitly promises to emit only v1 records. The correction is small
but must be frozen because it determines which durable semantics new
operations receive.

## Disposition of review 5

| Review-5 finding | Re-review result |
| --- | --- |
| P1-1: A1 could recognize but not execute later M6–M8 semantics in the same record version | **Resolved.** §15.1.1 allocates cumulative v1–v4 semantic versions, A1 contains only v1 lifecycle state, later bodies arrive with their implementations, and A2–A4 require actual older executables to reject unsupported envelopes before mutation. |
| P2-1: already archived v0 records lacked a separate evidence-only adaptation contract | **Resolved.** §15.3.3 defines an archive-byte-only projection with `Current`, `LegacyComplete`, `LegacyUnavailable`, and `NotAccepted` outcomes; contradictory durable evidence remains `ArchivedRecordUnreadable`; reads never rewrite archives or consult live state; retention/GC is tested independently of historical repositories. |

## Finding

### [P1-1] New-operation version selection has no active-writer floor

**Where:** §15.1.1; §§16.1–16.4; A1; A2–A4; §§22.3–22.5.

The version table correctly says:

- M5 writes v0;
- A1 emits only v1 integration/acceptance records;
- M6 branch operations require v2;
- M7 snapshot operations require v3; and
- M8 partial-composition operations require v4.

However, the general selection rule says immutable operation intent chooses
“the lowest version that covers every requested semantic wave.” Under that
rule, an ordinary direct-ref M4/M5 merge—without `--into`, snapshot source, or
optional participation—continues to select v0 even after A1, because v0 already
covers M4/M5 integration. §16.3 repeats the ambiguity by saying a direct-ref
operation uses the lowest version required by its other features.

That conflicts with A1's explicit requirement to “emit only v1
integration/acceptance records.” It also weakens the purpose of R4b/A1:
new feature-free merges could remain on the legacy record that does not persist
the new accepted-workspace contract, while only pre-existing open v0 records
are upgraded.

Different implementers can therefore derive opposite valid-looking behavior:

1. choose v0 because the request needs only M4/M5 integration; or
2. choose v1 because A1 has activated v1 as the baseline writer.

This is not merely a naming issue. It determines whether a newly created
operation durably carries `AcceptedWorkspace`, born/unborn acceptance, and the
new publication/no-publication recovery contract.

**Required correction before I2/A1:**

Define two independent version decisions:

1. **Active writer floor.** Each release has the minimum version it may create
   for a new operation:
   - M5 and earlier: v0;
   - A1 and every later release: at least v1.
2. **Requested semantic level.** Branch lifecycle, snapshot source, and
   optional/partial participation require v2, v3, and v4 respectively.

The new-operation version should be:

```text
max(active_writer_floor, highest_requested_semantic_version)
```

That yields the unambiguous creation matrix:

| Installed writer | Ordinary direct-ref | M6 branch lifecycle | M7 snapshot source | M8 partial composition |
| --- | ---: | ---: | ---: | ---: |
| M5 | v0 | unsupported | unsupported | unsupported |
| A1 | v1 | unsupported | unsupported | unsupported |
| A2/M6 | v1 | v2 | unsupported | unsupported |
| A3/M7 | v1 | v2 | v3 | unsupported |
| A4/M8 | v1 | v2 | v3 | v4 |

Keep this creation rule separate from handling an existing open record:

- supported v1–v4 records retain their original version and semantics;
- open v0 records use only the separately approved v0-to-v1 migration;
- archived records are never upgraded on read; and
- no later feature can upgrade an already-running lower-version operation.

Update “lowest sufficient version” references to mean the lowest version at or
above the active writer floor. Freeze the floor in I2 and make each activation
gate assert it.

**Required tests:**

- A1 starts an ordinary direct-ref merge as v1, never v0.
- A2, A3, and A4 still start feature-free direct-ref merges as v1.
- A2 starts `--into` as v2 but a direct-ref merge as v1.
- A3 starts a snapshot-source merge as v3, including when it has no M6 branch
  request.
- A4 starts partial composition as v4.
- Existing v1, v2, and v3 records retain their versions when continued by a
  newer binary.
- Open v0 migration and archived-v0 projection remain the only exceptions to
  ordinary new-operation selection.
- The version is persisted before the first feature-specific mutation, as the
  proposal already requires.

## Positive assessment

The review-5 corrections close the two difficult compatibility directions:

- **forward compatibility:** an older executable can reject a later semantic
  wave from the envelope without compiling or projecting its body; and
- **historical compatibility:** an older archive can remain truthful and
  readable without reconstructing history from today's workspace.

The archived-v0 mapping is especially strong. It distinguishes absent
historical evidence from contradictory durable evidence, preserves exact
candidate/baseline bytes when available, prevents legacy projections from
entering mutable lifecycle code, and gives GC an archive-only validation
contract.

The sequencing also remains sound: R4a extracts the current acceptance policy,
R3 implements adapters with production writing disabled, R4b installs complete
acceptance consumption, and A1 activates only the semantics its co-shipped
finalizer can resume.

## Recommended disposition

1. Keep R0, R1, R2a, and M5 approved.
2. Add the active-writer-floor rule and creation matrix before I2 is frozen.
3. Add the feature-free creation cases to A1–A4 activation tests.
4. Re-review the frozen I1/I2 contract before R3, as the proposal already
   requires.

No further architectural change is needed. Once the creation rule distinguishes
the installed writer's baseline from optional feature requirements, the
review-5 compatibility remediation is complete and internally consistent.
