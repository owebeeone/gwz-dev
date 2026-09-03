# GWZ Merge M1/M2a Remediation — Independent Re-Review (F5-2)

Date: 2026-07-19. Reviewer designation: **F5** (Claude Fable 5), read-only
re-review of the M2a remediation executed per
`dev-docs/GwzDevCodeM2a-RemPlan.md` against the findings in
`dev-docs/GwzDevCodeM2a-ReviewF5.md` and `dev-docs/GwzDevCodeM2a-Review56.md`.

## Reviewed state

The remediation is present as **uncommitted working-tree changes** on top of
the unchanged M2a checkpoint heads. Exact reviewed state:

| Repository | HEAD (unchanged from M2a checkpoint) | Working tree | `git diff` sha256 (first 16) |
| --- | --- | --- | --- |
| workspace root | `585e44d07f13880fe60072b7409a9d559f64ae53` | clean (untracked docs only) | `e3b0c44298fc1c14` (empty) |
| `gwz-core` | `cf70d360925563f82d92d93d9522e91d3567a5f3` | 36 files, +3852/−349 | `ef4cdb08bcc6e663` |
| `gwz-cli` | `9d634709a4a9be599a0300244eb92d53416b0c91` | 11 files, +324/−53 | `98c3b870e918a800` |
| `gwz-py` | `b771d98d7b33401fd555a13ad0e16b01c01610c6` | 14 files, +324/−79 | `8387b868e001178b` |

Every changed hunk in all three repositories was read, together with the full
current text of the rewritten files (`status.rs`, `start.rs`,
`continue_op.rs`, `abort.rs`, `mod.rs`, `model.rs`, `store.rs`,
`recovery.rs`, the backend additions, every guarded handler entry point, both
drivers' gate/render paths, the protocol schema and fixtures, and the updated
requirements/design/plan/user documentation). Both original P1/P2
reproductions were re-executed against the rebuilt dev binary and native
Python module. The untracked out-of-scope root files remain untouched.

## Verdict

**Remediation verified complete. No P0, P1, or P2 findings remain in the
corrected M1/M2a contracts. M2b-A2 finalization and M2b-C driver/event work
may begin on these interfaces.** Two P3 observations and a handful of
cosmetic notes follow; none blocks M2b, none weakens a safety invariant.

All fourteen dispositions in the remediation plan's §2 are implemented as
committed, each with focused regression tests that did not exist before, and
the two live reproductions from the original reviews now behave correctly:

- the previously inescapable `recovery_required` record is now discharged by
  `gwz merge --abort` (verified live: state `aborted`, record archived to
  `.gwz/merge/done/`, workspace gate released); and
- `Client(root=W).commit(...)` invoked from a cwd outside `W` is now blocked
  by the typed `OpenOperation` rejection (verified live against the same
  synthetic open-merge workspace that bypassed the gate before).

## New findings (this re-review)

### [P3-1] A continue that finishes its actions but observes late drift leaves the record resting in `executing`

- **Where:**
  [continue_op.rs:89-97](gwz-core/src/workspace_ops/merge/continue_op.rs:89)
  — when the post-action snapshot finds drift, the handler returns the
  observed response without applying `remaining_state(...)`; the durable
  state stays at the `Executing` persisted before the action loop
  ([continue_op.rs:39-45](gwz-core/src/workspace_ops/merge/continue_op.rs:39)).
- **Violated:** none of the frozen invariants — the replacement for the old
  trap deliberately reports drift without persisting `recovery_required`, and
  every recovery path accepts `executing` (continue re-enters it, abort takes
  `Executing → Halted → RollingBack`). This is a truthfulness/diagnosability
  nit: `merge --status` then reports `state: executing` indefinitely for an
  operation that is at rest awaiting user action.
- **Scenario:** continue resolves all conflicts successfully; an untracked
  file appears in a participant before the final snapshot; the command
  returns drift rows with the operation left saying `executing`. Removing the
  file and re-running continue converges to `finalizing` (verified by code
  path; the drift is re-classified fresh each run).
- **Impact:** cosmetic-to-minor; no wedge, no data risk, no gate anomaly.
- **Recommended correction:** apply `remaining_state(&record)` (all of whose
  targets are legal transitions from `Executing`) before returning the
  drift-bearing response, so the resting state matches the participant rows.
- **Regression test:** fake-store continue test injecting an untracked file
  between the last outcome write and the final snapshot, asserting the
  response carries drift and the durable state equals the remaining-state
  projection rather than `executing`.

### [P3-2] Dry-run merge start while a merge is open diverges between drivers

- **Where:**
  [mod.rs:99-116](gwz-core/src/workspace_ops/merge/mod.rs:99) — the
  authoritative guard is acquired only for non-dry-run starts; a dry-run
  start performs no open-merge gate check in core. The Rust CLI's retained
  advisory gate classifies every `MergeOp::Start` as `MergeStart` regardless
  of `--dry-run` ([globalargs.rs:789-797](gwz-cli/src/globalargs.rs:789)) and
  therefore rejects with `OpenOperation`, while the Python driver (which now
  correctly has no advisory gate) reaches core and returns either an advisory
  plan (all-clean members) or a typed per-member preflight rejection
  (dirty/conflicted members).
- **Violated:** driver-parity expectation (REQ-089C) on a read-only path.
  No mutation is possible on either driver: dry-run planning holds no lock
  and mutates nothing, and any participant carrying live merge state fails
  member preflight with a typed error.
- **Impact:** minor: identical requests produce `OpenOperation` on one driver
  and a plan/typed-preflight result on the other while a merge is open.
- **Recommended correction:** pick one semantics deliberately — either gate
  dry-run start in `handle_merge_with_events` (matching the Rust CLI), or
  drop `MergeStart` gating for dry-run in the CLI's advisory check — and pin
  it with a two-driver test.
- **Regression test:** open-merge workspace; `merge <src> --dry-run` through
  both drivers asserting the chosen common behavior.

## Disposition verification (all fourteen remediation rows)

| Finding | Disposition claimed | Verified outcome |
| --- | --- | --- |
| **F5 P1-1** `recovery_required` trap | Guarded exits; no trap for routine observation drift | **Fixed and reproduced.** `RecoveryRequired → Executing/RollingBack/Preserving` added to the matrix with tests ([model.rs:79-83](gwz-core/src/workspace_ops/merge/model.rs:79)); continue accepts the state and re-preflights ([continue_op.rs:28-35](gwz-core/src/workspace_ops/merge/continue_op.rs:28)); abort rejects only `Completed` ([abort.rs:104-109](gwz-core/src/workspace_ops/merge/abort.rs:104)); continue's post-action drift path no longer persists the state (finding P3-1 is the residue of that change, in the safe direction); ambiguity persists `RecoveryRequired` but remains re-enterable. Live re-run: a `recovery_required` record with a matching baseline was aborted, archived, and the gate released. Tests: `recovery_required_can_enter_guarded_rollback`, `recovery_required_retry_store_failure_adopts_without_repeating_git`. |
| **F5 P2-2 / 56 P1** gate bypass | One authoritative core guard under the mutator lock | **Fixed and reproduced.** `acquire_workspace_mutation_guard` resolves the effective workspace from `meta.workspace.root` (else nearest boundary), acquires the mutator lock, discovers the open merge at that exact root, applies the table, and holds the lock through the mutation ([mod.rs:171-205](gwz-core/src/workspace_ops/merge/mod.rs:171)). Every public mutating handler routes through it: branch, tag, stash, commit, stage, snapshot, capture, materialize, pull (head+snapshot), push, init-update, repo create/add/sync/clone/detach/attach — verified individually. Merge start acquires the same guard; dry-runs use gate-only checks (except P3-2 above). Python driver checks are removed entirely (core authoritative); the Rust CLI keeps advisory checks plus a real held guard for `forall`, which has no core handler ([globalargs.rs:515-533](gwz-cli/src/globalargs.rs:515)). Tests: `direct_core_mutator_cannot_bypass_open_merge_gate` (direct core call), `test_native_gate_uses_explicit_root_outside_cwd_and_stage_is_conditional` (the exact bypass), plus my live differential re-run now blocking with `OpenOperation`. The gate-check/start race is closed because the check happens under the same lock as the protected mutation. |
| **F5 P2-3** raw native abort wedges coordinated abort | Recognize exact restoration in every abortable state | **Fixed.** The classifier now grants abort eligibility to `externally_restored_conflict` (conflicted row at exact before ref, clean index/worktree, clean repository state, no missing objects) in any open state ([status.rs:769-787](gwz-core/src/workspace_ops/merge/status.rs:769)); abort treats it as a verified no-op row and persists `aborted` without a second Git abort ([abort.rs:434-438](gwz-core/src/workspace_ops/merge/abort.rs:434)). Tests: `externally_restored_conflict_is_abort_eligible` (classifier), `externally_restored_conflict_is_persisted_without_a_second_git_abort` (abort), and the end-to-end scenario below. `merge.md` now explicitly warns against substituting raw `git merge --abort`. |
| **F5 P3-4 / 56 P1** Git success before row write | Durable pending intent + exact reconciliation | **Fixed.** `PendingMergeAction` (kind, frozen branch/before/source/message, unknown-field-retaining) is written and fsync-verified before every mutating start/continue action and cleared only in the same durable write as the outcome ([start.rs:186-216](gwz-core/src/workspace_ops/merge/start.rs:186), [continue_op.rs:47-63](gwz-core/src/workspace_ops/merge/continue_op.rs:47)); failures preserve the intent for later reconciliation. The read-only reconciler proves exactly one of not-started / expected-conflict / completed-exactly / ambiguous, using an exact two-parent + byte-exact-message commit matcher ([status.rs:48-198](gwz-core/src/workspace_ops/merge/status.rs:48), [gitbackend.rs:849-874](gwz-core/src/git/gitbackend.rs:849)); continue and abort adopt exact outcomes durably and reject ambiguity with `pending_action_ambiguous` drift before any mutation. Fault tests cover clean/FF/conflict/resolution store failures with fresh-process reloads and assert convergence without duplicate commits (`clean_start_store_failures_adopt_exact_results_without_duplicate_git`, `conflict_and_resolution_store_failures_reconcile_after_reload`, `completed_pending_action_is_adopted_then_rolled_back`, `expected_pending_conflict_is_adopted_then_aborted`, `ambiguous_pending_action_blocks_before_record_or_git_mutation`). |
| **F5 P3-5** stale M0 docs | Rewrite with the durable contract | **Fixed.** `merge.md` now describes the frozen baseline lock, quoted message with GWZ trailers, pending-action reconciliation, hidden-but-existing recovery commands, the open-`finalizing` development caveat, and forbids raw `git merge --abort`; `Releases.md` replaces both stale claims and adds the identity policy; `MachineOutput.md` documents the complete response shape including `pending_action` and the extended drift taxonomy. No internal milestone names in any user-facing text. |
| **F5 P3-6 / 56 P2** missing-object / collapsed drift | Distinct, member-scoped, structured | **Fixed.** New protocol drift kinds `head_diverged`, `object_missing`, `foreign_integration_state`, `pending_action_ambiguous` appended at wire values 10–13 (append-only verified against the schema diff and regenerated corpus). `read_live_participant` probes every recorded oid with `commit_exists` and classifies absence as structured `ObjectMissing` observations with role and oid instead of erroring; `is_ancestor` runs only when both objects exist; every backend probe error now carries member context via `member_result` ([status.rs:375-536](gwz-core/src/workspace_ops/merge/status.rs:375)). Diverged heads map to their own kind. Tests: `divergent_head_has_its_own_structured_drift`, `missing_expected_and_resulting_commits_are_member_scoped_object_drift`, `rewound_detached_and_missing_heads_have_distinct_evidence`. |
| **F5 P3-7 / 56 P2** incomplete events | Full §16 stream in durable order | **Fixed.** `handle_merge_with_dependencies` wraps every merge op in `OperationStarted`/`OperationFinished` (finished emitted on failure too, [mod.rs:310-329](gwz-core/src/workspace_ops/merge/mod.rs:310)); `MemberStarted` precedes each actionable step in start, continue, and abort; `MemberFinished` carries the full structured `MergeRepoSummary` (new appended `OperationEvent.merge_member` field, emitted only after the durable row write); every record write and archive emits `ArtifactWritten` with the workspace-relative path (appended `artifact_path` field) after the verified write ([mod.rs:253-296](gwz-core/src/workspace_ops/merge/mod.rs:253)). A shared canonical event fixture (`gwz-core/protocol/fixtures/cli_parity/merge_event.json`) is byte-pinned by both drivers (`merge_event_json_keeps_state_outcome_and_artifact_fields`, `test_merge_event_json_matches_rust_canonical_fixture`), and the Python native stream closes exactly once (`test_native_merge_status_closes_one_event_stream`). |
| **F5 P3-8** nested-workspace capture | Stop at nearest boundary, recovery first | **Fixed.** `discover_open_before_manifest` inspects each directory's `.gwz/merge` before its manifest, then stops at the first directory containing the workspace manifest instead of walking into enclosing workspaces ([recovery.rs:26-50](gwz-core/src/workspace_ops/merge/recovery.rs:26)), preserving recovery-through-invalid-manifest. Test: `recovery_discovery_stops_at_nearest_nested_workspace_boundary`. |
| **F5 P3-9** post-rollback work blocks resume | Ref-only verification for durable rows | **Fixed.** `durable_restore_verified` grants abort eligibility to recorded `aborted`/`rolled_back` rows on the target-ref check alone; later worktree/index/untracked content is reported but not blocking, and the rollback loop skips those rows without Git calls ([status.rs:780-788](gwz-core/src/workspace_ops/merge/status.rs:780), [abort.rs:167-171](gwz-core/src/workspace_ops/merge/abort.rs:167)). Tests: `durably_restored_row_ignores_later_worktree_dirt_for_abort` (classifier) and `durable_rollback_row_ignores_later_worktree_changes` (abort). |
| **F5 P3-10** stage gate untested | Core + driver tests | **Fixed.** `handle_stage` now runs under the authoritative guard with the narrow conflicted-participant check under the same held lock; tests cover conflicted-member allow, merged/unaffected/root reject in core (`conditional_stage_allows_only_recorded_conflicted_participants`), the Rust CLI (`rust_cli_stage_gate_allows_conflicted_member_and_rejects_root`), and Python (the combined native gate/stage test). |
| **F5 P3-11** unvalidated record paths | Validate before observation | **Fixed.** `validated_participant_path` parses every durable path as a workspace-relative member path (root participants must be `.`), applied to the entire participant set before the first repository access in `snapshot_status` and in each observation/reconciliation entry ([status.rs:200-209,456-478](gwz-core/src/workspace_ops/merge/status.rs:200)). Test: `invalid_record_path_is_rejected_before_repository_observation`. |
| **56 P1** foreign sequencer passes abort preflight | Complete repository-state observation | **Fixed.** New `repository_state` backend observation maps every libgit2 state ([gitbackend.rs:653-703,1218-1222](gwz-core/src/git/gitbackend.rs:653)); the shared classifier consumes it first (native merge detail only when the state is `merge`), emits `ForeignIntegrationState` drift for any other non-clean state, and makes rows requiring mutation ineligible; abort's `verified_no_op` explicitly cannot override an ineligible observation ([abort.rs:412-421](gwz-core/src/workspace_ops/merge/abort.rs:412)); preflight and checked primitives now agree by construction. The regression uses the plan's corrected ordering — poisoned participant earlier in execution order, so reverse rollback would reach it only after mutating the later one — and asserts zero runtime calls and zero record writes (`foreign_state_in_earlier_app_rejects_before_later_docs_rollback`), plus a real-`CHERRY_PICK_HEAD` classifier test and the real-Git end-to-end scenario which asserts the record bytes are unchanged through the rejected abort. |
| **56 P2** terminal abort/archive contradiction | Truthful open projection, retryable archive | **Fixed.** Snapshot-based responses force `open = true` for any record still in the open directory regardless of terminal lifecycle state, so gate and response agree ([response.rs:85-93](gwz-core/src/workspace_ops/merge/response.rs:85)); an `aborted` record found open-side archives idempotently and then reports closed; abort with the merge id after the rename reloads from `done/` and completes archive verification/retention idempotently instead of `OperationNotFound` ([abort.rs:111-118,456-483](gwz-core/src/workspace_ops/merge/abort.rs:111)); the store accepts destination-only and both-copies retry states (`archive_retry_accepts_destination_only_after_publish`, `archive_retry_removes_matching_open_copy_after_destination_publish`, `terminal_archive_failure_is_retryable_without_reobserving_repositories`, `retry_by_id_succeeds_when_archive_moved_before_reporting_failure`). General archived-status-by-id correctly remains M3 (`validate.rs` unchanged). |
| **56 P2** resolution identity ignores request attribution | One attribution policy | **Fixed.** `commit_merge_resolution_checked` now takes the operation attribution and resolves author/committer through the same `merge_signatures` path as start-time true merges ([gitbackend.rs:1901-1949](gwz-core/src/git/gitbackend.rs:1901), [continue_op.rs:397-404](gwz-core/src/workspace_ops/merge/continue_op.rs:397)); retries pass it too; REQ-089 now states the request-supplied-else-repository rule for immediate, retried, and resolution commits. Test: `retry_true_merge_uses_request_author_and_committer` plus resolution-identity assertions in the fault tests. |

## Adversarial and crash-matrix results (remediation plan §7)

Every boundary in the plan's required matrix was verified against the
implementation and its tests:

1. **Record created before first action intent** — status discovers it;
   continue re-plans, abort verifies no-ops and closes. Unchanged, still
   sound.
2. **Pending action written before Git** — reconciliation proves not-started
   (intent cleared, retry proceeds) with the frozen inputs cross-checked
   against the participant record; mismatched intent is `Ambiguous`, blocking
   both mutations with typed drift.
3. **Git succeeds before participant outcome write** — reload reconciles the
   exact outcome (FF at source; merge/resolution commit with exact parents,
   message, ref, clean state; conflict at exact expected native state) and
   adopts durably with no duplicate commit; anything inexact is ambiguous and
   mutation-blocked. Fault-injected for all five action shapes.
4. **Outcome written before event** — durable state wins; events are
   per-invocation and never replay mutations.
5. **Continue enters `executing` before first action** — continue and abort
   both re-preflight safely (abort via `Executing → Halted → RollingBack`).
6. **`recovery_required` after manual correction** — fresh preflight permits
   guarded continue or abort; verified live and by test. Remaining ambiguity
   stays `recovery_required` without becoming a trap.
7. **Abort enters `rolling_back` before first action** — retry resumes the
   same complete reverse plan.
8. **Git rollback succeeds before row write** — recognized idempotently
   (classifier `restored_mutation` plus checked-primitive already-at-target
   fast paths, real-Git tested).
9. **Rollback row written before next action** — completed rows are skipped;
   later unrelated work there no longer blocks (P3-9 fix).
10. **Terminal state written before archive** — status, response, and gate
    now agree the operation is still closing (`open = true` from storage
    location); retrying abort completes archive.
11. **Archive rename succeeds before return** — abort by id reloads the done
    record, completes verification/retention idempotently, and returns the
    terminal response; no duplicate archive.
12. **Foreign sequencer in the last reverse-preflight participant** — the
    entire abort rejects with zero mutation and byte-identical record
    (fake-ordering test plus the real end-to-end scenario).
13. **Explicit Python root differs from cwd** — the gate applies to the
    explicit root (native test plus live re-run).
14. **Nested workspace inside an open outer workspace** — inner commands
    resolve and gate at the inner boundary (discovery test; inner mutators
    are guarded by their own workspace's state only).

## Verification commands and results

Run from `/Users/owebeeone/limbo/gwz-dev` (macOS arm64, Darwin 25.5.0)
against the exact reviewed working tree; pre/post `git status --porcelain`
snapshots were byte-identical (the suite changed no tracked or untracked
files).

| Command | Result |
| --- | --- |
| `cargo fmt --all -- --check` | pass (exit 0) |
| `cargo test --workspace --all-targets --no-fail-fast` | pass — **639 passed, 0 failed, 1 ignored** across 14 test binaries (remediation plan's own gate recorded 638; the ±1 is a harness-binary counting artifact, with zero failures under both counts) |
| `cargo clippy --workspace --all-targets --all-features -- -D warnings` | pass (exit 0) |
| `cargo fmt --manifest-path gwz-py/Cargo.toml -- --check` | pass (exit 0) |
| `cargo clippy --manifest-path gwz-py/Cargo.toml --all-targets --all-features -- -D warnings` | pass (exit 0) |
| `gwz-py/.venv/bin/python -m maturin develop --manifest-path gwz-py/Cargo.toml` | pass — wheel rebuilt and installed |
| `gwz-py: scripts/regen_protocol.py --check` | pass — `regen_protocol: OK` (append-only schema changes regenerate cleanly) |
| `gwz-py: .venv/bin/python -m pytest -q` | pass — **268 passed** |
| `gwz-cli: python3 scripts/generate_cli_reference.py --check` | pass (exit 0) |
| `uvx --from 'mkdocs-material>=9.5,<10' mkdocs build --strict --site-dir /tmp/gwz-m2a-review-F5-2-docs` | pass |
| `bazel build //gwz-core:gwz_core //gwz-cli:gwz` | pass — build successful |
| `git diff --check` (root, gwz-core, gwz-cli, gwz-py) | all clean (exit 0) |

Live reproductions re-run against the rebuilt artifacts (scratch-directory
fixtures outside the repositories, as in the original review):

- **P1-1 discharge:** synthetic workspace, open record in `recovery_required`
  with baseline digests matching its `gwz.conf` bytes → `gwz merge --status`
  reports the state; `gwz merge --abort` exits 0 with `state: aborted`,
  `merge_trap (closed)`; the record is in `.gwz/merge/done/`; a subsequent
  mutator is no longer gate-blocked. (Original binary: both commands rejected
  with `MergeRecoveryRequired` and the gate held forever.)
- **P2-2 block:** same open-merge fixture as the original bypass;
  `Client(root=W).commit("x")` from a cwd outside `W` now fails with
  `OpenOperation: merge 'merge_gate' is open; …`. (Original module: the gate
  was silently skipped and the handler ran.)

## Residual notes (non-blocking)

- **Crashed-continue staged-resolution window:** a pending
  `resolve_conflict` action reconciled as not-started deliberately makes
  ordinary abort reject ("the staged resolution cannot be discarded by
  ordinary abort", [abort.rs:307-324](gwz-core/src/workspace_ops/merge/abort.rs:307),
  pinned by `pending_resolution_not_started_still_requires_abort_eligible_index`).
  In this one window "abort may be safe while continue is not" inverts to
  continue-first; the message is typed and honest and continue always
  converges, after which abort is available. Deliberate, tested, acceptable —
  worth a line in the M2b-C recovery docs.
- **Event-stream cosmetics:** rows adopted during abort's pending
  reconciliation receive `MemberStarted` twice in one invocation (once for
  reconciliation, once in the rollback loop), and a participant whose pending
  action is ambiguous gets `MemberStarted` with no matching `MemberFinished`
  before the typed rejection. Consumers ordering on durable outcomes are
  unaffected.
- **Boundary-stop failure mode:** `discover_open_before_manifest` treats an
  unreadable ancestor manifest probe as a boundary (`try_exists().unwrap_or(true)`),
  which stops implicit discovery early. The authoritative guard resolves
  explicit roots independently, so this only affects implicit recovery-root
  resolution under unreadable ancestor directories — exotic and fail-safe in
  the direction of not capturing unrelated directories.
- **`restored_mutation` recognition requires all recorded objects present:**
  a user-restored merged member whose orphaned merge commit was pruned shows
  `object_missing` drift and blocks abort; recovery guidance is accurate.
  Conservative and acceptable.
- **Prior residuals unchanged:** macOS-only verification; in-primitive crash
  sub-windows needing one raw-git command before automation resumes;
  `.tmp` accumulation in `.gwz/merge/`; `carry_unknown` positional sequence
  pairing. None regressed; none is release-gating for M2b to start.

## Recommended next action

**Proceed.** The remediation gate is genuinely green: all fourteen
dispositions are implemented with regression tests, both original P1/P2
defects are dead by live reproduction, the corrected contracts
(repository-state observation, pending-action journal, guarded
`recovery_required` exits, authoritative locked gate, no-op abort
recognition, truthful open/close projection, event and identity policy) are
documented in `GWZRequirements.md`/`GwzMergeDesign.md`/`GwzMergePlan.md`, and
the full verification gate passes with no generated-artifact drift.

Specifically:

1. Commit the remediation as the superseding M2a gate (the working tree is
   currently the only holder of ~4.5k reviewed lines).
2. M2b-A2 finalization and M2b-C driver/event completion may begin against
   these interfaces; the finalization fault matrix should reuse the
   pending-action reconciliation pattern rather than inventing a second
   adoption mechanism.
3. Fold the two P3 observations (resting `executing` after late drift;
   dry-run-start gate parity) and the staged-resolution abort note into the
   M2b wave at convenience; none blocks integration.
