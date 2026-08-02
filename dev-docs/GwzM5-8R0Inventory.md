# GWZ M5–M8 R0 Baseline and Lifecycle Inventory

Status: R0 inventory at the checked implementation baseline. This document is
descriptive, not a new lifecycle contract. `GwzM5-8Refactor.md` remains the
proposal authority. Combinations not demonstrated by production construction,
recovery code, or a named test are recorded as gaps rather than declared legal.

## 1. Baseline identity

The revision baseline was captured with installed `gwz` before R0 production
changes. Every component was on `main`, every lock comparison was `Matches`,
and the workspace was clean at capture time.

| Repository | GWZ id | Revision |
| --- | --- | --- |
| workspace root | `@root` | `3aa24c52914634f99b8957c04257cc7f5f4dd481` |
| `gwz-cli` | `mem_gwz_cli` | `a3fb71e7e6a101b33d5da1ecf1e134a7af31ad55` |
| `gwz-core` | `mem_gwz_core` | `b4d8c63d2c15d0bd9d13288250c7c4303c27177c` |
| `taut` | `mem_taut` | `f00841966663bb0d500974ede446d30e9bfeeac4` |
| `gwz-py` | `mem_gwz_py` | `423d08d572b4fcfd86ac2291847fe81052ecad09` |
| `taut-shape` | `mem_taut_shape` | `389c867010a7cb474c03960ba3420308ed2bd64a` |
| `taut-shape-rs` | `mem_taut_shape_rs` | `7e50457391489d5a2628a60c0462f07f53baf021` |
| `taut-shape-py` | `mem_taut_shape_py` | `7e7d5a7fc27635b152e10815503f1b456ae6a960` |

The core package version is `0.10.2`; its baseline commit is two commits after
tag `v0.10.2`. The root and core baseline commits both have subject
`docs: plan merge lifecycle refactor` and timestamp 2026-07-31 23:30:12/13
+1000.

R0 work started after that capture. Consequently, later `gwz status` output may
show the inventory, characterization tests, compatibility harness, or checking
scripts as dirty while all revision ids above remain unchanged. Those files do
not redefine the baseline.

## 2. Measured module and LOC baseline

Counts below are newline/physical-line counts from the `gwz-core` baseline Git
tree, not from the concurrent R0 worktree. A "module" is one Rust source file.
The production-module footprint includes any test-only blocks embedded in a
production source file; dedicated-test counts do not. This is deliberate: the
method is exact, repeatable, and does not pretend that line classification by
the first `#[cfg(test)]` is valid.

| Scope | Modules | Physical LOC | Classification |
| --- | ---: | ---: | --- |
| `src/workspace_ops/merge/**/*.rs` production-bearing files | 44 | 11,155 | excludes paths under `tests/` and files named `tests.rs`; includes embedded tests |
| merge-local dedicated test modules | 19 | 4,169 | paths under merge `tests/` plus files named `tests.rs` |
| `src/workspace_ops/tests/g23/**/*.rs` lifecycle integration tests | 19 | 5,617 | dedicated tests/fixtures |
| merge Git-backend perimeter | 7 | 2,671 | `contract`, `merge_prepared`, `merge_recovery`, `merge_support`, `preservation`, `scoped_evidence`, `repository` |

There are 179 `#[test]`/`#[rstest]` attributes in the baseline merge subtree
plus `g23`. This is a source test count, not the expanded runtime case count;
several tests iterate matrices.

The largest production-bearing modules at baseline are:

| Module | Physical LOC |
| --- | ---: |
| `src/workspace_ops/merge/mod.rs` | 1,158 |
| `src/workspace_ops/merge/model.rs` | 805 |
| `src/workspace_ops/merge/finalize.rs` | 728 |
| `src/workspace_ops/merge/continue_op/execution.rs` | 481 |
| `src/workspace_ops/merge/preserve/artifacts.rs` | 479 |
| `src/workspace_ops/merge/store.rs` | 476 |
| `src/workspace_ops/merge/plan.rs` | 440 |
| `src/workspace_ops/merge/preserve.rs` | 418 |
| `src/workspace_ops/merge/response.rs` | 377 |
| `src/workspace_ops/merge/preserve/plan.rs` | 375 |

This freezes measurement, not package budgets. Numeric add/delete/move and
file ceilings belong in the R0 change-budget ledger; movement must remain a
separate quantity as required by the proposal.

## 3. Durable v0 vocabulary

The wire identity is exactly:

- `schema: gwz.merge-operation/v0`
- `record_schema_version: 0`

`MergeOperationRecord` durably carries operation identity/state, source and
mode, exact baseline evidence, ordered selected targets, participant records,
optional publication progress, operation drift, and flattened unknown fields.
The exact v0 model is in `workspace_ops/merge/model.rs`.

### 3.1 Operation transitions

The table is the exact relation accepted by `OperationState::transition`.
Self-transitions are accepted for every state.

| From | Other accepted next states | Named lifecycle use | §15.3.2 row family | Baseline evidence |
| --- | --- | --- | --- | --- |
| `Executing` | `AwaitingResolution`, `Halted`, `Finalizing`, `Preserving`, `RecoveryRequired` | start/continue execution and pre-acceptance interruption | A; H when preserving; L when recovery-required | `start::durable_recovery_tests`; `g23::continue_merge`; `g23::preserve` |
| `AwaitingResolution` | `Executing`, `Finalizing`, `Preserving`, `RollingBack`, `RecoveryRequired` | unresolved native merge, retry, preserve, abort | A, H, I, L | `g23::continue_merge`; `g23::root_recovery`; `g23::root_abort` |
| `Halted` | `Executing`, `Preserving`, `RollingBack`, `RecoveryRequired` | failed participant plus retry/preserve/abort | A, H, I, L | `start::durable_recovery_tests::real_git_drift_halts_with_durable_rows_and_keeps_baseline_lock`; `g23::continue_merge` |
| `Finalizing` | `Completed`, `Preserving`, `RollingBack`, `RecoveryRequired` | publication state machine | B–G, H, I, L | `g23::finalization`; `g23::abort_recovery` |
| `Preserving` | `RollingBack`, `RecoveryRequired` | preservation artifacts are being reconciled | H, I, L | `g23::preserve`; `g23::preserve::preserving_abort_gate` |
| `RollingBack` | `Aborted`, `RecoveryRequired` | evidence and participant reverse mutations | I, L | `g23::root_abort::root_evidence_abort_resumes_after_each_rollback_mutation`; `merge::abort::tests` |
| `RecoveryRequired` | `Executing`, `RollingBack`, `Preserving` | typed recovery overlay | L over its source row | `merge::start::prepared_recovery_tests`; `merge::abort::tests::lifecycle::recovery_required_can_enter_guarded_rollback` |
| `Completed` | none | terminal open record or archive | J/K, or G before K | `g23::characterization_v0::v0_terminal_completed_before_archive_is_read_only_and_closes_byte_exactly` (R0 worktree) |
| `Aborted` | none | terminal open record or archive | M | `g23::characterization_v0::v0_terminal_aborted_before_archive_is_read_only_and_closes_byte_exactly` (R0 worktree) |

The transition unit test samples this relation but is not exhaustive. An
exhaustive table test for every accepted/rejected pair remains an R0 fixture
gap.

### 3.2 Participant transitions and result shapes

The exact transition relation is:

| From participant state | Other accepted next states | Durable result constraints used by lifecycle code | Row context |
| --- | --- | --- | --- |
| `Planned` | `UpToDate`, `FastForwarded`, `Merged`, `Conflicted`, `Failed`, `Unattempted`, `Aborted` | result commit for clean outcomes; merge-head/conflict evidence for conflict; error for ordinary failure | principally A; B–G after all participants settle; I/M on abort |
| `Unattempted` | `UpToDate`, `FastForwarded`, `Merged`, `Conflicted`, `Failed`, `Aborted` | retry consumes frozen before/source/branch/message | A, I/M |
| `Failed` | `UpToDate`, `FastForwarded`, `Merged`, `Conflicted`, `Failed`, `Aborted` | retry may retain a pending action when mutation outcome is unresolved | A, I/M |
| `UpToDate` | `Aborted` | `resulting_commit == before_commit`; no integration rollback required | A/B–G/J/K or I/M |
| `Conflicted` | `Continued`, `Aborted` | expected merge head and conflict paths; conflict snapshot may be empty only for adopted interrupted mutation | A, then B–G after continue, or I/M |
| `FastForwarded` | `RolledBack` | resulting commit normally equals source commit | A/B–G/K or I/M |
| `Merged` | `RolledBack` | resulting two-parent merge commit | A/B–G/K or I/M |
| `Continued` | `RolledBack` | resulting resolution commit | A/B–G/K or I/M |
| `Aborted` | none | no changed result is owned after rollback | I/M |
| `RolledBack` | none | previously integrated result was checked back to `before_commit` | I/M |

Representative coverage is in `g23::start`, `g23::continue_merge`,
`g23::abort_recovery`, `g23::root_start`, and
`merge::abort::tests::lifecycle::mixed_three_member_abort_unwinds_only_mutated_rows`.
The transition unit test is not a complete pair matrix; that remains a gap.

### 3.3 Pending-action inventory

These are the only v0 pending actions accepted by
`decode_durable_prepared_action`:

| Named shape | Kind | Expected result | Commit spec | Legal participant source state | Possible observation | Row context | Test evidence |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `PA-VERIFY` | `VerifyUpToDate` | absent or `Unchanged` | absent | `Planned`, `Failed`, `Unattempted` | not started or completed exactly | A; L overlay | status pending tests; clean-store failure matrix |
| `PA-FF` | `FastForward` | absent or `FastForward` | absent | `Planned`, `Failed`, `Unattempted` | not started or completed exactly | A; L overlay | `start::durable_recovery_tests::clean_start_store_failures_adopt_exact_results_without_duplicate_git` |
| `PA-MERGE-CONFLICT` | `TrueMerge` | `ExpectedConflict` | absent | `Planned`, `Failed`, `Unattempted` | not started, exact native conflict, or ambiguous | A; L overlay | durable recovery conflict tests; abort pending reconciliation tests |
| `PA-MERGE-COMMIT` | `TrueMerge` | `Commit` | exact tree, author, committer | `Planned`, `Failed`, `Unattempted` | not started, completed exactly, or ambiguous | A; L overlay | `start::prepared_recovery_tests::pending_true_merge_not_started_executes_frozen_spec`; ambiguous-evidence test |
| `PA-RESOLVE` | `ResolveConflict` | `Commit` | exact tree, author, committer | `Conflicted` | not started, completed exactly, or ambiguous | A; L overlay | `pending_resolution_exact_retry_uses_frozen_signatures`; resolution race/validation tests |

On abort, exact completed/conflict observations are first adopted into the
participant row and then rolled back. Ambiguous observations reject before
record or Git mutation. `PendingMergeAction` is not a legal publication,
preservation, rollback, or cleanup journal. A pending action under finalization
or a terminal state has no demonstrated producer and must not be treated as
legal without a fixture and interface decision.

Missing fixture: one named serialization/restart test for every row above in
each of the not-started, completed-exactly, expected-conflict (where
applicable), and ambiguous observation classes. Existing tests cover the
classes in aggregate but do not form a closed action-by-observation table.

### 3.4 Preservation inventory

Preservation is currently represented by operation state plus post-mutation
evidence rather than a pending preservation action.

| Named shape | Durable representation | Legality/recovery rule | Row | Test evidence |
| --- | --- | --- | --- | --- |
| `PR-NONE` | empty participant/root evidence vector | no owned preservation artifact | A/B–G as applicable | ordinary lifecycle tests |
| `PR-REF-RECORDED` | one evidence row with both `backup_ref` and `backup_commit` | ref must use `refs/gwz/merge/<merge-id>/<target>/head` and match commit | H; remains historical in I/M/archive | `preserve_abort_saves_committed_staged_and_untracked_work_before_rollback`; GC tests |
| `PR-REF-INTERRUPTED` | evidence absent, exact operation-owned ref exists | adoptable only while operation is `Preserving` and target equals frozen live commit | H | `preserve_abort_resumes_from_recorded_ref_and_native_stash_without_duplicates` |
| `PR-STASH-RECORDED` | one evidence row with both `stash_id` and `stash_object_id` | exactly one native stash with merge-owned message; matching bundle | H; remains historical in I/M/archive | preserve recovery/failure-window tests |
| `PR-STASH-INTERRUPTED` | evidence absent, one exact merge-owned native stash exists | adoptable only in `Preserving` and only if live status proves stash effect | H | preserve recovery/failure-window tests |
| `PR-ROOT-PREFIX` | publication `preservation_prefix` is `baseline`, `marker`, `lock`, or `boundary` | candidate must exist; observed prefix must equal recorded or the narrowly permitted normalized prefix | H | `preserve_retry_repairs_interrupted_root_publication_normalization`; root retry safety |
| `PR-ROOT-EVIDENCE` | at most one `publication.root_preservation` row | same paired ref/stash shape; owner is publication root rather than selected root participant | H/I/M | `preserve_abort_handles_post_composition_root_work_with_root_bundle_identity` |

More than one evidence row per owner, only one half of either evidence pair, a
wrong ref identity, multiple matching stashes, or changed evidence is rejected
as unreadable/drift. There is no durable pre-action preservation intent. The
proposal correctly treats this as an ownership gap for R1/I2 rather than
claiming the current post-action row is a complete journal.

Missing fixtures: all four root `preservation_prefix` values as independently
interrupted durable shapes; exact restart after stash-bundle write failure; and
archived evidence with stash-only, ref-only, and combined evidence.

## 4. Closed v0 progress inventory mapped to §15.3.2

The following table records producer-observed durable combinations. "Covered"
means a named test constructs the shape; it does not mean the v1 adapter exists.

| Proposal row | Named current shape(s) | Exact current v0 combination | Evidence/tests | R0 gap |
| --- | --- | --- | --- | --- |
| A | `A-EXECUTING`, `A-AWAITING`, `A-HALTED`, `A-PRE-PRESERVE`, `A-PRE-ROLLBACK` | operation in the named pre-acceptance state; publication/candidate absent; participant rows and optional merge pending action drive recovery | start durable recovery; continue; abort lifecycle; preserve tests | no single table fixture freezes every legal operation/participant/pending combination and response/event ordering |
| B | `B-NO-PUBLICATION-ROW`, `B-NOT-STARTED`, `B-VALIDATING`, `B-PREPARING-EMPTY` | `Finalizing`; publication absent or step through `PreparingCandidate`; candidate absent | `characterization_v0::v0_changed_merge_windows_have_named_exact_durable_shapes` covers absent/validating/preparing (R0 worktree); pre-candidate abort test | durable `NotStarted` post-rename window is not independently injected; born/unborn variants are not complete |
| C | `C-CANDIDATE-PERSISTED` | `Finalizing`; step `PreparingCandidate`; candidate and candidate lock/path hashes present; composition fields absent; root still at accepted base | changed-window characterization and finalization fault matrix | exact candidate cross-field assertions and born-root twin fixture are incomplete |
| D | `D-EVIDENCE-CREATED-UNRECORDED` | durable record remains candidate-bearing `CommittingEvidence` with no composition fields while live root may contain the exact evidence commit | finalization fault matrix; born/unborn evidence-abort recovery | explicit record-plus-live observation fixture should pin parent/message/tree/files/hash checks |
| E | `E-EVIDENCE-RECORDED` | step `CommittingEvidence`; candidate plus composition commit/tree and ordered candidate hashes present; candidate artifacts not published | changed-window characterization; finalization fault matrix; born/unborn evidence-abort recovery | named born and unborn serialized fixtures with exact composition cross-fields |
| F | `F-BASELINE`, `F-MARKER`, `F-LOCK`, `F-BOUNDARY` | step `PublishingCandidate`; candidate/evidence durable; live filesystem/index equals one exact write-order prefix | publication classifier and preservation/rollback tests; `AfterLockPublication` reaches complete boundary prefix | forward interruption fixtures for marker-only and lock-only prefixes are missing; baseline/boundary need explicit index assertions |
| G | `G-VERIFYING`, `G-COMPLETE-PRE-ARCHIVE` | candidate/evidence complete; full candidate published; step `VerifyingPublication` or `Complete`; operation `Finalizing` or `Completed` | before-archive/finalization matrix covers complete; terminal byte-exact close test (R0 worktree) | no direct durable `VerifyingPublication` interruption fixture; born-root complete-before-archive twin missing |
| H | `H-PRESERVING-PRE`, `H-PRESERVING-CANDIDATE`, `H-PRESERVING-PREFIX` | operation `Preserving`; source row evidence retained; participant/root preservation evidence and optional prefix | preserve, review-remediation, preserving-abort-gate, root-retry-safety tests | closed matrix across A/C/E/F source rows and each prefix missing |
| I | `I-PARTICIPANT-ROLLBACK`, `I-EVIDENCE-ROLLBACK` | operation `RollingBack`; reverse participant suffix recorded as `Aborted`/`RolledBack`; evidence rollback may be interrupted before `evidence_rolled_back = true` | abort recovery; root abort mutation matrix; abort unit recovery | there is no durable rollback cursor/typed reverse action; named record shapes for every reverse prefix are missing |
| J | `J-NO-PUBLICATION-BORN`, `J-NO-PUBLICATION-UNBORN` | operation `Completed`; publication `Complete`; candidate, composition, candidate hashes absent; all participants up-to-date; baseline root head/branch frozen | `characterization_v0::v0_no_publication_completion_preserves_born_and_unborn_root_inputs` (R0 worktree); baseline all-up-to-date tests | terminal-open no-publication before archive is not separately frozen byte-exactly |
| K | `K-COMPLETED-CANDIDATE-OPEN`, eventual `K-COMPLETED-NOPUB-OPEN` | terminal `Completed` still under open directory; evidence is source row G or J | candidate-bearing byte-exact status/close test (R0 worktree); abort recovery/finalization tests | no-publication terminal-open byte-exact fixture missing |
| L | `L-RECOVERY-OVERLAY` | `RecoveryRequired` plus the unchanged evidence of an otherwise legal source row | candidate C and no-publication J overlay characterization (R0 worktree); prepared recovery and root drift tests | proposal requires overlays over every applicable source row A–J; current matrix is incomplete |
| M | `M-ABORTED-OPEN` | terminal `Aborted` still in open directory; participant/evidence rollback complete | byte-exact status/close test for pre-acceptance abort (R0 worktree); abort archive-failure unit test | candidate/evidence-era and preservation-bearing terminal-open variants missing |

This inventory does not bless arbitrary enum cross-products. In particular,
candidate fields without a candidate, incomplete composition triples,
publication progress inconsistent with the observed prefix, terminal records
with pending merge actions, and preservation evidence with half a pair are
contradictions in current code or unproven gaps.

## 5. Archived-v0 evidence shapes

Current archive is a durable rename of the exact terminal open YAML into
`.gwz/merge/done`; it does not build a historical projection. The archived
loader requires a regular v0 record with matching file id and terminal
`Completed`/`Aborted` state. Unknown YAML is retained byte-for-byte by the
rename, although the current typed response does not expose a separate legacy
evidence projection.

### 5.1 Evidence fields available in an archive

| Domain | Durable v0 evidence |
| --- | --- |
| identity | schema/version, writer, workspace/merge/operation ids, source, mode, creation time |
| baseline | lock/manifest digests; optional exact lock/manifest YAML; optional commit digests; optional root head and symbolic branch; unknown extensions |
| participants | ordered selection plus keyed identity, target branch, before/source/message, terminal or historical state, result/merge head, conflict paths/snapshot, error, pending action, preservation, drift, unknown extensions |
| candidate | marker/root/actor identity; exact baseline and candidate lock YAML; exact marker and boundary bytes; their hashes; unknown extensions |
| publication | step, candidate lock/path, root merge commit, composition commit/tree, ordered candidate hashes, candidate, evidence rollback flag, root preservation, preservation prefix |
| operation | terminal state and operation drift; flattened unknown extensions |

### 5.2 Observed terminal archive shapes

| Archive id | Terminal/evidence shape | Proposed §15.3.3 projection candidate | Existing evidence | Missing characterization |
| --- | --- | --- | --- | --- |
| `AC-CANDIDATE` | completed, candidate/evidence/full publication retained | AV0-B `LegacyComplete(Candidate)` if complete audit/root validation succeeds | finalization, status-by-id, and before-archive tests | archive-only projection with repositories removed; explicit complete-audit assertion |
| `AC-NOPUB-BORN` | completed, no candidate, exact born baseline fields | AV0-C if exact baseline lock/audit suffice, otherwise AV0-D | R0 no-publication characterization | archive-only projection and optional-baseline variants |
| `AC-NOPUB-UNBORN` | completed, no candidate, root head absent plus exact branch | AV0-C or AV0-D according to evidence completeness | R0 no-publication characterization | archive-only projection with live root later born/missing |
| `AA-PREACCEPTANCE` | aborted before candidate/evidence | AV0-E `NotAccepted` | mixed abort/archive tests | explicit no-live-state projection |
| `AA-CANDIDATE-COMPLETE` | aborted after exact candidate/evidence existed; rollback result is separate | AV0-F `LegacyComplete(Candidate)` | born/unborn evidence-abort tests retain candidate and set `evidence_rolled_back` | archive projection proving acceptance independently of later abort |
| `AA-CANDIDATE-PARTIAL` | aborted with candidate or partial publication but insufficient historical acceptance | AV0-G `LegacyUnavailable` | some rollback-window tests exercise live recovery | named candidate-only and every publication-prefix archive fixture |
| `AP-PRESERVED` | aborted archive retains ref and/or stash evidence | AV0-E/F/G according to acceptance, plus preservation evidence | preserve-abort and GC tests | separate stash-only/ref-only/combined archive fixtures |
| `AL-OPTIONAL-MISSING` | otherwise valid terminal v0 record lacks optional baseline/candidate-era evidence required by new projection | AV0-D/G with closed gaps | no closed fixture set | exact gaps `ExactLockBytes`, `CompleteMemberAudit`, `AcceptedRootInput`, `PublicationEvidence` |
| `AL-UNKNOWN` | valid terminal archive has unknown top-level and nested fields | same AV0 result; raw bytes unchanged | open-record unknown round-trip unit test only | archive read/status/retention byte-stability fixture |
| `AR-C` | nonterminal archive or durable internal contradiction | `ArchivedRecordUnreadable` | store rejects nonterminal archived load; several malformed store tests | digest/identity/cross-field contradiction matrix |

The currently strongest archive tests still run in a live workspace. R0 has not
yet frozen the proposal's proof that history is identical with the root/member
repositories missing, moved, or changed. That is a material fixture gap, not a
claim that present behavior reads live repositories: current id-qualified
archived status loads the record directly and converts it to a response.

## 6. Mutation and durable-owner inventory

Read-only planning, simulation, status observation, hashing, validation, and
event emission are excluded. The table includes record/artifact filesystem
mutations as well as Git mutations.

| Mutation family | Current mutation/API and boundary | Current durable owner/evidence | Target owner from proposal | Coverage and ownership gap |
| --- | --- | --- | --- | --- |
| workspace mutual exclusion | `WorkspaceMutatorLock::acquire` creates/owns the workspace mutation lock; drop releases it | lock file/owner itself, outside merge record | `runtime/mutation_guard.rs` | open-operation and concurrent mutation guard tests; not lifecycle history |
| open record create/rewrite | temp create/write/fsync, durable rename, directory sync, reread verification | `MergeStore::write_open`; entire record is the next durable state | `store/persistence.rs` | store tests and injected store failures; remains store-owned |
| participant up-to-date verification | prepared `VerifyUpToDate` executes with no intended Git mutation | `PendingMergeAction` | `PendingParticipantAction::Integrate` | retain as action/reconciliation case even though exact result is unchanged |
| participant fast-forward | checked prepared execution moves branch/HEAD and updates index/worktree | `PendingMergeAction::FastForward` written first; result row afterward | `PendingParticipantAction::Integrate` | clean store-failure adoption test |
| participant true merge | writes merge commit or native conflict index/worktree/sequencer state | `PendingMergeAction::TrueMerge` plus exact commit spec/result class | `PendingParticipantAction::Integrate` | prepared recovery, conflict, signature tests |
| conflict-resolution commit | checked commit writes commit, moves branch, clears native merge state | `PendingMergeAction::ResolveConflict` plus exact tree/signatures | `PendingParticipantAction::Integrate` | resolution retry/race/validation tests |
| native conflict abort | `abort_merge` restores pre-merge index/worktree and removes merge state | operation `RollingBack`, participant conflict evidence; no rollback pending action | `PendingParticipantAction::RollbackIntegration` plus immutable rollback plan/cursor | exact interrupted-action adoption exists; pre-action rollback journal is absent |
| integrated branch reset | checked `set_branch_target_checked(result,before)` restores branch/HEAD/index/worktree | operation `RollingBack`, participant result; participant state changes after mutation | `PendingParticipantAction::RollbackIntegration` plus cursor | rollback-store-failure recovery test; no pre-action rollback journal |
| root evidence commit | `commit_gwz_paths_checked` writes scoped evidence commit and moves/creates root branch | `PublicationStep::CommittingEvidence` plus candidate; composition fields recorded after mutation | `PublicationProgress` / `finalization/evidence.rs` | born/unborn record-window recovery exists |
| candidate marker write | atomic marker file publication | `PublicationStep::PublishingCandidate`, candidate bytes/hash; live prefix is reconcilable | `PublicationProgress` / `finalization/publish.rs` | no marker-only forward failure fixture |
| candidate lock write | atomic `gwz.lock` publication | same | same | no lock-only forward failure fixture |
| boundary write | `publish_workspace_exclude_candidate` changes `.git/info/exclude` | same candidate baseline/new boundary bytes and hashes | same | rollback fault tests exist; forward prefix fixture incomplete |
| candidate index stage | stages candidate lock/marker | same; exact index checked during preservation/recovery | same | finalization and preservation tests; forward post-stage/pre-record fixture implicit only |
| root evidence commit rollback | `rollback_gwz_paths_commit_checked` restores born/unborn root base | operation `RollingBack`, candidate/composition; `evidence_rolled_back` set only after later artifact restoration | typed reverse action owned by publication rollback plan | born/unborn abort recovery exists; no discrete durable action/cursor |
| rollback boundary restore | writes baseline boundary | operation `RollingBack`; exact candidate baseline bytes; final flag after whole sequence | publication rollback action/progress | fault injected after boundary |
| rollback lock restore | writes baseline lock | same | same | fault injected after lock |
| rollback marker removal | removes candidate marker | same | same | fault injected after marker |
| rollback candidate staging | stages restored lock/marker absence | same | same | fault injected after staging; sequence has no durable substep cursor |
| selected-root baseline restore | atomic writes of exact baseline manifest and lock after participant rollback | operation `RollingBack`, terminal participant states, optional exact baseline YAML/digests | typed rollback/preservation action; exact owner must be frozen at I2 | root abort tests; no per-file pre-action journal |
| preservation backup ref creation | `create_backup_ref` | operation `Preserving`; evidence written after mutation; exact unrecorded ref may be adopted | `PendingPreservationAction` | interruption/retry tests; no durable pre-action intent |
| preservation native stash | `stash_for_merge_preservation` mutates stash ref/index/worktree | operation `Preserving`; evidence written after mutation; exact unrecorded stash may be adopted | `PendingPreservationAction` | failure-window/retry tests; no durable pre-action intent |
| root preparation for stash | rewrites candidate files and stages them before stashing | persisted `preservation_prefix`, candidate bytes | `PendingPreservationAction` | normalization retry tests; multi-mutation substeps lack a cursor |
| root publication restoration after stash | restores baseline/candidate lock, marker, boundary and stages | persisted `preservation_prefix`, candidate bytes | `PendingPreservationAction` | normalization safety tests; no typed sub-action |
| preserved branch restore | checked branch reset from backed-up live commit to merge anchor | recorded backup-ref/commit evidence; participant state is not advanced here | `PendingPreservationAction` or typed rollback step, to be fixed at I2 | retry converges, but no pre-action journal |
| stash bundle write | `stash::write_bundle` creates/rewrites `.gwz/stash` bundle | stash evidence already in merge record plus bundle identity | `PendingPreservationAction` / preservation artifact progress | bundle-specific post-write failure fixture missing |
| terminal archive | durable open-to-done rename, duplicate verification/removal, directory sync | terminal record itself; archive is byte-exact | `store/persistence.rs` | archive failure retry tests; remains store-owned |
| ordinary retention deletion | deletes oldest valid unpreserved archives over 20 | valid terminal archive plus mtime ordering; unreadable/preserved records skipped | store retention owner; future header/archive decoder must gate deletion | store retention test exists; future-version protection is not implemented |
| checked backup-ref GC | preflights all archive evidence, then `delete_backup_ref_checked` per owner | immutable terminal archive preservation evidence; no cleanup cursor | proposal assigns artifact cleanup to `PendingPreservationAction`/progress, but concrete post-archive owner is unresolved | checked GC tests; partial multi-ref crash cursor is a design gap |
| targeted archive GC | removes one validated terminal archive and syncs done directory | target archive is authorization and disappears at completion | `store/gc` | status/GC tests; must follow future version/header classification |

The table exposes four current ownership deficiencies that later interface
checkpoints must close rather than conceal: reverse participant mutation,
multi-step evidence rollback, preservation actions, and multi-ref cleanup. All
forward participant integration mutations already have a pre-action durable
owner.

## 7. Consolidated fixture gap register

These gaps are sufficient to block a claim that R0 is complete, but they do not
block using this inventory as the starting artifact:

1. exhaustive operation and participant transition-pair tables;
2. closed action-by-observation coverage for all five pending shapes;
3. forward publication failure windows at marker-only, lock-only, boundary,
   and post-stage/pre-record points;
4. a durable `VerifyingPublication` fixture;
5. born/unborn twins for every applicable B–G state, not only evidence rollback
   and no-publication completion;
6. H preservation overlays across each source row and all four recorded root
   prefixes;
7. named reverse-prefix record fixtures for participant and evidence rollback;
8. L `RecoveryRequired` overlays over every applicable A–J source row;
9. K no-publication and M candidate/preservation terminal-open byte-exact
   fixtures;
10. archive-only AV0-B–G fixtures with live roots/members absent or changed;
11. optional legacy evidence-gap fixtures for all four closed gap values;
12. archived unknown-field and raw-byte stability across status/retention;
13. archive contradiction fixtures separating missing optional evidence from
    internal corruption; and
14. exact restart evidence for stash-bundle failure and partial multi-ref GC.

The in-flight `g23/characterization_v0.rs` closes parts of items 3–9, as marked
in the tables, but does not close the remaining cases by implication.

### 7.1 R0 closure addendum

The inventory above is the baseline discovery record. Subsequent bounded R0
slices produced the following evidence; this addendum records disposition
without rewriting what was unknown when the inventory was captured.

| Gap | R0 disposition |
| ---: | --- |
| 1 | Closed by exhaustive 9×9 operation and 10×10 participant transition tables in `merge/tests/transition_matrix_v0.rs`. |
| 2 | Closed by the 40-cell pending-action decode table and real-Git observation matrix in the same module. Two observation cells are structurally impossible and are asserted as such. |
| 3 | Closed for every actual mutation boundary. `characterization_publication_v0.rs` covers the durable publication windows; `characterization_publication_prefix_v0.rs` uses test-only hooks after marker, lock, boundary, and staging mutations and proves restart convergence. |
| 4 | Closed by the born/unborn durable `VerifyingPublication` cases in `characterization_publication_v0.rs`. |
| 5 | Closed for every producer-observed B–G fault window by born/unborn twins. Unproduced enum cross-products remain unblessed. |
| 6 | Closed for all four recorded preservation prefixes plus interrupted ref/stash evidence and producer-observed source rows by `characterization_preservation_v0.rs` and the existing `g23::preserve` corpus. A blind A/C/E/F cross-product is deliberately not declared legal. |
| 7 | Closed by participant reverse-suffix and evidence Boundary/Lock/Marker/Staging restart matrices. |
| 8 | Closed for every producer-constructible publication row plus the pre-acceptance awaiting-resolution row. Other arbitrary `RecoveryRequired` combinations remain invalid until I2 freezes an adapter mapping. |
| 9 | Closed by byte-exact no-publication `Completed` and candidate-bearing `Aborted` open-record close cases. |
| 10 | Closed by AV0-B–G archive-only status with live root/member repositories absent and then independently changed. |
| 11 | Closed for the four proposed optional evidence-gap classes; all remain readable and byte-stable under the v0 loader. |
| 12 | Closed by nested unknown-field/raw-byte stability through archived status and retention scanning. |
| 13 | Current v0 behavior is frozen for schema, identity, terminal-state, and field-type contradictions. Digest/candidate semantic validation and typed `LegacyAcceptanceGap` projection do not exist in v0 and remain I2/R3 work rather than invented R0 behavior. |
| 14 | Closed at the exact restart-state boundary: missing stash bundles and partially deleted multi-ref GC converge idempotently. Direct I/O-failure hooks are unnecessary to the durable-state proof because the materialized states are byte-identical to the post-mutation crash windows. |

The Rust characterization side of R0 is therefore closed over current legal
v0 producers and durable mutation boundaries. R0 as a whole is not accepted
until the checksum-pinned retained-reader cases run on their required
Linux/Windows behavioral lanes and supported-platform smoke evidence is
recorded. Metadata readiness or a local-only run is not a substitute.

### 7.2 Retained-reader local checkpoint

The retained-reader inventory declares the exact 24-reader/platform cross
product and freezes each tuple as required or explicitly unsupported. Its six
canonical fixtures and 13 command cases cover v0.9.2's pre-record downgrade
boundary and the selected durable-v0 baseline's field-known reader behavior.
For this checkpoint the reproducible manifest pins v0.10.2. Status, continue,
abort, post-mutation reconciliation/rollback, fail-closed wrong-message handling, preserve,
archive-only status, explicit GC, and the dormant v0 `no_ff` incompatibility
all run through actual pinned executables.

The independent R0 interface reviews found false-positive paths in tuple
completeness, input validation, fixture/provenance identity, JSON projection
checks, mutation bounds, durable-content identity, exact result-set checking,
and derived runtime reuse. Their remediation is represented by 80 passing
unit/adversarial tests. The 13 cases now contain 59 explicit postconditions,
including 17 content-semantic checks for lock/member data, publication state,
Git indexes, markers, and complete archived records. A fresh local macOS arm64
run produced 26 passing executable/case results and zero failures; a second run
matched the checked portable semantic projection. Checked evidence contains 26
exact result rows and 14 complete evaluator-source digests. It remains self-reference-free,
while CI emits a separate evidence-digest attestation carrying its commit and
workflow-run identity.

This is the local implementation checkpoint, not R0 acceptance. The committed
workflow must still produce passing attested evidence on Linux x86_64, Windows
x86_64, Linux arm64, macOS x86_64, and macOS arm64 before R1 dispatch.

### 7.3 R0 acceptance

R0 was accepted on 2026-08-03 after retained-reader workflow
[`30765936845`](https://github.com/owebeeone/gwz-core/actions/runs/30765936845)
passed against exact core commit
`b7b49b4d05009e71a3606d2566febacd88d3a767`. Both Linux and Windows harness
jobs passed, followed by all five required behavioral lanes: Linux x86_64,
Linux arm64, Windows x86_64, macOS x86_64, and macOS arm64.

Each uploaded evidence artifact contains exactly 26 passing rows and no failed
or unsupported row. Each execution attestation binds the artifact digest to
the exact commit, workflow run, and platform; all five bindings were verified
against the downloaded artifact bytes. This completes the platform gate left
open by §7.2 and unblocks R1 under `GwzM5-8R1Ownership.md`.

## 8. Reproduction commands

Revision/status capture:

```sh
gwz status --json
git -C gwz-core show -s --format='%H%n%ci%n%s' \
  b4d8c63d2c15d0bd9d13288250c7c4303c27177c
git show -s --format='%H%n%ci%n%s' \
  3aa24c52914634f99b8957c04257cc7f5f4dd481
```

Baseline production-bearing module count and physical LOC:

```sh
git -C gwz-core ls-tree -r --name-only \
  b4d8c63d2c15d0bd9d13288250c7c4303c27177c \
  src/workspace_ops/merge \
  | rg '\.rs$' \
  | rg -v '/tests/|/tests\.rs$'
```

For each emitted path, count the output of:

```sh
git -C gwz-core show \
  b4d8c63d2c15d0bd9d13288250c7c4303c27177c:<path> \
  | wc -l
```

Inventory/test references:

```sh
rg -n 'enum (OperationState|ParticipantState|PendingMergeActionKind|PublicationStep)' \
  gwz-core/src/workspace_ops/merge/model.rs
rg -n '^fn v0_' \
  gwz-core/src/workspace_ops/tests/g23/characterization_v0.rs
rg -n '^fn ' \
  gwz-core/src/workspace_ops/tests/g23/{finalization,abort_recovery,root_abort,preserve,gc}.rs
```
