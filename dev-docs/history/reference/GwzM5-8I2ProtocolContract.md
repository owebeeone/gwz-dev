# GWZ I2 v1 Protocol Contract

Date: 2026-08-04

Status: **accepted; R4a unblocked and R3 remains sequenced after R4a**

Amended 2026-08-22 by `GwzM5-8OperatorEscapeAmendment.md` (accepted at
GO/GO): §1's error table appends codes 62-65 and its context-legality
sentences extend to them; §2's enum allocation appends the escape enums and
two `MergeOp` values; §3's `MergeRecordProjection` gains fields 6-8, the new
escape/quarantine messages are appended, `MergeRequest` gains field 8,
`MergeResponse` gains field 11, the field-10 population list extends, and the
record-context range sentence extends to 45–65; §4 gains the escape rendering
rules. Every change is **append-only**: no existing discriminant, field
number, message shape, message name, rendering rule, or error code changes.
Applied per that amendment's §7 item 5. The frozen sentences below are **not
rewritten**: each amended clause carries a dated §-local annotation holding
the amendment's exact replacement text, and that amendment is controlling
wherever the two disagree.

This slice freezes I2 error allocations, structured record context, merge
record/acceptance/recovery projections, and Rust/Python/JSON/JSONL parity.

## 1. Error allocation and context

I2 appends after `merge_record_unreadable = 45`:

| Numeric | Taut/Rust | JSON/JSONL |
| ---: | --- | --- |
| 46 | `unsupported_record_version` / `UnsupportedRecordVersion` | `UnsupportedRecordVersion` |
| 47 | `unsupported_legacy_mode` / `UnsupportedLegacyMode` | `UnsupportedLegacyMode` |
| 48 | `archived_record_unreadable` / `ArchivedRecordUnreadable` | `ArchivedRecordUnreadable` |
| 49 | `unexpected_acceptance_evidence` / `UnexpectedAcceptanceEvidence` | `UnexpectedAcceptanceEvidence` |
| 50 | `acceptance_input_drift` / `AcceptanceInputDrift` | `AcceptanceInputDrift` |
| 51 | `candidate_integrity_mismatch` / `CandidateIntegrityMismatch` | `CandidateIntegrityMismatch` |
| 52 | `ambiguous_evidence_commit` / `AmbiguousEvidenceCommit` | `AmbiguousEvidenceCommit` |
| 53 | `recorded_evidence_drift` / `RecordedEvidenceDrift` | `RecordedEvidenceDrift` |
| 54 | `publication_prefix_mismatch` / `PublicationPrefixMismatch` | `PublicationPrefixMismatch` |
| 55 | `published_candidate_mismatch` / `PublishedCandidateMismatch` | `PublishedCandidateMismatch` |
| 56 | `preservation_evidence_mismatch` / `PreservationEvidenceMismatch` | `PreservationEvidenceMismatch` |
| 57 | `rollback_evidence_mismatch` / `RollbackEvidenceMismatch` | `RollbackEvidenceMismatch` |
| 58 | `unexpected_publication_evidence` / `UnexpectedPublicationEvidence` | `UnexpectedPublicationEvidence` |
| 59 | `terminal_evidence_mismatch` / `TerminalEvidenceMismatch` | `TerminalEvidenceMismatch` |
| 60 | `recovery_evidence_mismatch` / `RecoveryEvidenceMismatch` | `RecoveryEvidenceMismatch` |
| 61 | `terminal_rollback_mismatch` / `TerminalRollbackMismatch` | `TerminalRollbackMismatch` |

`GwzError` appends field 7:

```text
record_context?: MergeRecordCompatibilityContext {
  1 merge_id: string, 2 schema?: string,
  3 record_schema_version?: INT constrained to 0..=u32::MAX,
  4 required_wave?: MergeRecordRequiredWave,
  5 legacy_mode?: string,
}
MergeRecordRequiredWave: a1=0, a2=1, a3=2, a4=3
```

Wave displays are exactly `A1 (v1 integration/acceptance/no-ff)`, `A2 (v2
branch lifecycle)`, `A3 (v3 snapshot source)`, and `A4 (v4 partial
composition)`. Codes 46/47 require applicable context; codes 49–61 require id
and installed pair; code 48 includes every valid field available before the
contradiction. Existing `MergeRecordUnreadable`, `MergeDrift`, and
`MergeRecoveryRequired` also require readable header context on a post-record
command failure; they omit it when the header itself cannot be read. Every
other error forbids context. Codes 46–61 have absent member/detail/target
fields.

As amended 2026-08-22 by `GwzM5-8OperatorEscapeAmendment.md` (accepted at
GO/GO), §1's error table appends codes 62-65 after
`terminal_rollback_mismatch = 61` (the verified current maximum; 0..61
contiguous), with the message bodies of that amendment's §2.5.4 and the same
registered-reason requirement the table already imposes on codes 49-61:

| Numeric | Taut/Rust | Message body after `merge record '<id>' ` |
| ---: | --- | --- |
| 62 | `operator_override_invalid` / `OperatorOverrideInvalid` | `operator override is invalid: <reason>` |
| 63 | `escape_consent_required` / `EscapeConsentRequired` | `escape requires explicit consent: <reason>` |
| 64 | `escape_not_applicable` / `EscapeNotApplicable` | `escape is not applicable: <reason>` |
| 65 | `quarantine_state_invalid` / `QuarantineStateInvalid` | `quarantine state is invalid: <reason>` |

All four require record context and carry absent member/detail/target fields.
The context-legality sentences above therefore extend by exact edit, so the
appended codes fall inside the require-context set rather than under "every
other error forbids context": **"codes 49–61 require id and installed pair"
becomes "codes 49–65 require id and installed pair", and "Codes 46–61 have
absent member/detail/target fields." becomes "Codes 46–65 have absent
member/detail/target fields."** The sentence "Every other error forbids
context." is **unchanged** and stays true.

Messages for 46–48 are:

- `merge record '<id>' uses schema '<schema>' version <n>, which requires <wave>; use a compatible newer GWZ`
- `merge record '<id>' uses unrecognized schema '<schema>' version <n>; use a compatible newer GWZ`
- `merge record '<id>' uses ambiguous legacy mode 'no_ff' and cannot be migrated or resumed safely`
- `archived merge record '<id>' is unreadable: <reason>`, where `<reason>` is
  one exact archived-contradiction reason registered in
  `../gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json`

Codes 49–61 use `merge record '<id>' ` followed respectively by:

```text
has unexpected acceptance evidence: <reason>
acceptance input changed: <reason>
candidate integrity check failed: <reason>
has ambiguous evidence-commit state: <reason>
recorded evidence changed: <reason>
publication prefix is invalid: <reason>
published candidate changed: <reason>
preservation evidence is invalid: <reason>
rollback evidence is invalid: <reason>
has unexpected publication evidence: <reason>
terminal evidence is invalid: <reason>
recovery evidence is invalid: <reason>
terminal rollback evidence is invalid: <reason>
```

Allowed exact reasons are the matching lists in
`../gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json`; they are not
free-form diagnostics.

## 2. Enum allocation

```text
MergeRecordVersion: v0=0, v1=1
MergeTerminalOutcome: completed=0, aborted=1
MergeAcceptanceKind:
  supported_persisted=0, legacy_complete=1,
  legacy_unavailable=2, not_accepted=3
MergeInstalledAcceptedWorkspaceKind: v1=0
MergeLegacyAcceptanceSource: candidate=0, baseline_no_publication=1
MergeLegacyAcceptanceGap:
  exact_lock_bytes=0, complete_member_audit=1,
  accepted_root_input=2, publication_evidence=3
MergeAcceptedMemberKind: selected=0, unselected_present=1, absent=2
MergeAcceptedRootKind: born_attached=0, born_detached=1, unborn_attached=2
MergeAcceptedMetadataSource: operation_baseline=0, selected_root_result=1
MergeRecoveryOriginState:
  executing=0, awaiting_resolution=1, halted=2, finalizing=3,
  preserving=4, rolling_back=5
MergeCompatibilityBasePhase:
  pre_acceptance=0, pre_candidate=1, candidate_persisted=2,
  evidence_unrecorded=3, evidence_recorded=4, publishing_prefix=5,
  published=6, no_publication_complete=7
MergeCompatibilityNextAction:
  reconcile_pending_participant=0, execute_next_participant=1,
  await_resolution=2, validate_results=3, persist_acceptance=4,
  prepare_candidate=5, create_or_adopt_evidence=6,
  publish_candidate=7, verify_publication=8,
  complete_no_publication=9, resume_preservation=10,
  resume_rollback=11, archive_completed=12, archive_aborted=13,
  report_recovery_required=14
```

No v2–v4 body/discriminant is allocated at I2.

As amended 2026-08-22 by `GwzM5-8OperatorEscapeAmendment.md` (accepted at
GO/GO), §2's enum allocation appends `MergeRecordSource`, `MergeEscapeOwner`,
`MergeEscapeSide`, `MergeSideDispositionKind`, `MergeConsentCollapseBasis`,
`MergeRetiredActionKind`, `MergeObservedFactKind`, and the two `MergeOp`
values `quarantine=5` and `restore=6`. Their exact discriminants are that
amendment's §2.5.1/§2.5.2, which are controlling; each takes the next free
slot and no existing discriminant is renumbered. Force-abandon is **not** a
new op — it is a flag on `abort`, so the frozen mutual-exclusion rule is
unchanged. No v2–v4 allocation is added.

## 3. Message fields

```text
MergeRecordProjection {
  1 source_version: MergeRecordVersion, 2 archived: bool,
  3 terminal_outcome?: MergeTerminalOutcome,
  4 acceptance?: MergeAcceptanceProjection,
  5 recovery?: MergeRecoveryProjection,
}
MergeAcceptanceProjection {
  1 kind: MergeAcceptanceKind,
  2 supported_persisted?: MergeInstalledAcceptedWorkspaceProjection,
  3 legacy_complete?: MergeLegacyAcceptedWorkspace,
  4 legacy_source?: MergeLegacyAcceptanceSource,
  5 legacy_evidence?: MergeLegacyAcceptanceEvidence,
  6 missing_gaps: [MergeLegacyAcceptanceGap],
}
MergeInstalledAcceptedWorkspaceProjection {
  1 kind: MergeInstalledAcceptedWorkspaceKind,
  2 v1?: MergeAcceptedWorkspaceV1Projection,
}
MergeRecoveryProjection {
  1 origin_state: MergeRecoveryOriginState,
  2 base_phase: MergeCompatibilityBasePhase,
  3 next_action: MergeCompatibilityNextAction,
  4 resume_action: MergeCompatibilityNextAction,
}
MergeAcceptedWorkspaceV1Projection {
  1 operation_baseline_lock_sha256: string,
  2 metadata_base: MergeAcceptedMetadataBaseProjection,
  3 lock_yaml: string, 4 lock_sha256: string,
  5 members: [MergeAcceptedMemberV1Projection],
  6 root: MergeAcceptedRootProjection,
}
MergeAcceptedMetadataBaseProjection {
  1 source: MergeAcceptedMetadataSource, 2 source_commit?: string,
  3 manifest_yaml: string, 4 manifest_sha256: string,
  5 lock_yaml: string, 6 lock_sha256: string,
}
MergeAcceptedMemberV1Projection {
  1 member_id: string, 2 kind: MergeAcceptedMemberKind,
  3 integration?: MergeAcceptedIntegrationProjection,
  4 final_checkout?: MergeAcceptedCheckoutProjection,
  5 lock_member?: MergeAcceptedLockMemberProjection,
}
MergeAcceptedIntegrationProjection {
  1 branch: string, 2 before_commit: string, 3 resulting_commit: string,
}
MergeAcceptedCheckoutProjection { 1 branch: string, 2 commit: string }
MergeAcceptedLockMemberProjection {
  1 path: string, 2 source_id: string, 3 source_kind: SourceKind,
  4 commit?: string, 5 branch?: string, 6 detached?: bool,
  7 upstream?: string, 8 dirty?: bool, 9 materialized?: bool,
}
MergeAcceptedRootProjection {
  1 kind: MergeAcceptedRootKind, 2 commit?: string,
  3 symbolic_branch?: string, 4 publication_branch?: string,
  5 lock_worktree_sha256: string, 6 manifest_worktree_sha256: string,
  7 lock_commit_sha256?: string, 8 manifest_commit_sha256?: string,
}
MergeLegacyAcceptedWorkspace {
  1 baseline_lock_sha256: string, 2 lock_yaml: string,
  3 lock_sha256: string, 4 members: [MergeAcceptedMemberV1Projection],
  5 root: MergeAcceptedRootProjection,
}
MergeLegacyAcceptanceEvidence {
  1 lock_yaml?: string, 2 lock_sha256?: string,
  3 members: [MergeLegacyMemberEvidence],
  4 root?: MergeAcceptedRootProjection,
  5 composition_commit?: string, 6 composition_tree?: string,
  7 candidate_hashes: [MergeAcceptedCandidateHashProjection],
}
MergeLegacyMemberEvidence {
  1 member_id: string, 2 selected: bool,
  3 state?: MergeParticipantState,
  4 integration?: MergeAcceptedIntegrationProjection,
  5 lock_member?: MergeAcceptedLockMemberProjection,
}
MergeAcceptedCandidateHashProjection { 1 path: string, 2 sha256: string }
```

`MergeResponse` appends optional field 10 `record`. A successful response tied
to a durable record includes it: start after creation, open/archived status,
continue, preserve-abort, abort, and id-qualified GC. Dry-run, idle status,
pre-record failure, and unqualified GC omit it.

A post-record command failure is returned through the existing top-level error
envelope, not a `MergeResponse`; it therefore cannot carry field 10. Its
`GwzError.record_context` must carry the merge id and exact schema/version pair
whenever the header is readable and the code is one of 45–61,
`MergeDrift`, or `MergeRecoveryRequired`. Callers needing the full durable
projection then issue id-qualified status. Renderers never synthesize a
partial record in an error payload.

As amended 2026-08-22 by `GwzM5-8OperatorEscapeAmendment.md` (accepted at
GO/GO), §3 appends, and changes nothing existing:

- `MergeRecordProjection` gains fields **6-8** — `6 source?:
  MergeRecordSource`, `7 operator_override?:
  MergeOperatorOverrideProjection`, `8 quarantine?:
  MergeQuarantineProjection`. Fields 1-5 are unchanged; `archived` (field 2)
  stays authoritative, with the pinned equivalence
  `archived == (source == archived)`;
- the new messages of that amendment's §2.5.2 are appended
  (`MergeOperatorOverrideProjection`, `MergeOperatorConsentProjection`,
  `MergeOwnerDispositionProjection`, `MergeSideDispositionProjection`,
  `MergeObservedFact`, `MergeQuarantineProjection`);
- `MergeRequest` gains field **8** `escape?: MergeEscapeRequest`, and
  `MergeResponse` gains field **11** `escape`, carrying the structured
  destruction manifest of the no-op first pass;
- the field-10 population list gains "quarantine, restore, and force-abandon
  successes".

The record-context range sentence above carries this exact edit: **"the code
is one of 45–61" becomes "the code is one of 45–65"**, so the four appended
codes carry `record_context` on exactly the frozen readable-header terms.

## 4. Discriminants and rendering

`supported_persisted` requires only the installed wrapper; A1 accepts exactly
kind v1 plus its v1 payload. Legacy complete requires complete payload/source
with no gaps/evidence-only payload. Legacy unavailable requires evidence plus
a sorted nonempty unique gap list. Not-accepted has no payload/source/gaps and
is legal only for an aborted pre-acceptance archive. Terminal outcome is
present exactly for archives and is independent of acceptance kind.

Nested shapes are exact:

| Kind | Required | Forbidden |
| --- | --- | --- |
| metadata operation baseline | bytes/hashes | source commit |
| metadata selected-root result | source commit, bytes/hashes | none |
| selected member | integration, checkout, lock member | none |
| unselected-present member | lock member | integration, checkout |
| absent member | none | all payloads |
| born-attached root | commit, symbolic/equal publication branch, hashes | none |
| born-detached root | commit, worktree hashes | symbolic/publication branch |
| unborn-attached root | symbolic/equal publication branch, worktree hashes | commit/commit hashes |

Commit hashes are both present or absent; selected-root input requires both.
For a supported current-version record, recovery appears exactly when the
record contains a valid unique `recovery_context`. Its next action is
`report_recovery_required`, its resume action is the unique guarded post-repair
action, and it authorizes no automatic mutation. Valid-unlisted v0 records are
projected through the frozen legacy path with acceptance and recovery absent;
A1 does not synthesize v1 recovery context for them.

Rows sort by member id; hashes by path; gaps are sorted/unique. Duplicate
identities reject. Taut omits absent optionals. JSON/JSONL emit every projection
key with `null` for absent optional scalar/message and `[]` for repeated fields.
Human output never invents absent data. Generated Rust/Python, corpus, both
JSON renderers, JSONL, and retained readers must agree exactly.

As amended 2026-08-22 by `GwzM5-8OperatorEscapeAmendment.md` (accepted at
GO/GO), §4 gains that amendment's §2.5.3 rendering rules:

- a record whose `source` is `quarantined` renders `archived=false` and
  `MergeResponse.open=false`; the pinned equivalence
  `archived == (source == archived)` is fixture-enforced;
- an overridden abort is **never** rendered as a plain abort in any renderer
  (human, JSON, JSONL, both drivers): the terminal line carries
  `aborted (operator override)` and the per-owner rows carry
  `abandoned:preservation` / `abandoned:rollback` / `abandoned:both`;
- open status of a record with dispositions but no `overridden_abort` renders
  `force-abandon in progress` plus the marked-side count;
- human output never invents absent data; JSON/JSONL emit every projection
  key with `null`/`[]` per the frozen rule above — unchanged, and restated
  here because the escape projections are optional throughout.
