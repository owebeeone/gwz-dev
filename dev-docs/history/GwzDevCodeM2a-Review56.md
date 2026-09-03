# GWZ M2a Code Review - Reviewer 56

Date: 2026-07-19

Reviewer designation: 56

Workspace: `/Users/owebeeone/limbo/gwz-dev`

Reviewed revisions:

- Root: `3d6ce941e7f79eb7aa4e93a1ddb4a865f17f9e90..585e44d07f13880fe60072b7409a9d559f64ae53`
- `gwz-core`: `8a0ad1732ed4efc51bb6d264067c18b1f7e3f920..cf70d360925563f82d92d93d9522e91d3567a5f3`
- `gwz-cli`: `f4e0261c3db33e0956331fa9fa576f123ae0f1e1..9d634709a4a9be599a0300244eb92d53416b0c91`
- `gwz-py`: `83e058e644cf9e719cc57accd325bd69223168f1..b771d98d7b33401fd555a13ad0e16b01c01610c6`

## Executive Verdict

Verdict: remediation required before M2b.

Severity counts:

- P0: 0
- P1: 3
- P2: 4
- P3: 0

The M2a implementation has substantial good work: durable records are written before ordinary member mutations, the accepted lock stays at the baseline, root publication is not attempted, finalizing is an internal handoff, and the main verification suite is green. However, I found three release-blocking defects in the recovery and gating contracts. Two were reproduced in real temporary workspaces: a foreign Git sequencer state can pass abort preflight and cause partial rollback, and the Python bridge can bypass the open-merge gate when the request targets a workspace different from the process cwd.

## Scope And Method

I reviewed the required design and planning documents, the changed M2a code paths in `gwz-core`, `gwz-cli`, and `gwz-py`, the public CLI/Python docs, and the F5/F5-2 remediation context supplied by the prompt. I did not read any other `GwzDevCodeM2a-Review*.md` reports. No `AGENTS.md` file was present under the workspace. I used no subagents. The only workspace write made by this review is this report.

I inspected the changed implementation paths for start, status, continue, abort, store/archive, open-operation gating, Rust CLI dispatch, Python native dispatch, event emission, protocol conversion, and the real `g23` workflow tests. I also ran focused repros in temporary directories for the highest-risk claims.

Initial and final repository status were checked. The initial checkpoint contained the following untracked files, and they were preserved:

- `dev-docs/AnnounceGwzHn.md`
- `dev-docs/GwzDevCodeM2aReviewPriompt.md`
- `dev-docs/GwzMergeCode-ReviewF5-2.md`
- `dev-docs/HN-AnnounceGWZ.md`

Final status also showed `dev-docs/GwzDevCodeM2a-ReviewF5.md`. I did not read or modify that file.

## Findings

### P1 - Foreign Git sequencer state passes abort preflight and causes partial rollback

Files and lines:

- `gwz-core/src/git/gitbackend.rs:1097-1100` returns `Ok(None)` from `merge_state` for every repository state except `RepositoryState::Merge`.
- `gwz-core/src/workspace_ops/merge/status.rs:107` consumes only `backend.merge_state`.
- `gwz-core/src/workspace_ops/merge/status.rs:270-315` treats a non-conflicted participant as clean/recoverable when `live.merge_state.is_none()` and the index/worktree are clean.
- `gwz-core/src/workspace_ops/merge/abort.rs:213-245` runs whole-operation abort preflight from that status snapshot.
- `gwz-core/src/git/gitbackend.rs:1188` and `gwz-core/src/git/gitbackend.rs:2219-2223` later reject the same participant during checked rollback when libgit2 reports a non-clean state.

Violated requirement or invariant:

- `gwz-core/dev-docs/GWZRequirements.md:589-592`: whole-operation preflight must reject existing merge/rebase state before mutation.
- `gwz-core/dev-docs/GWZRequirements.md:664-667`: status/recovery must report structured drift from a read-only observation snapshot.
- `gwz-core/dev-docs/GWZRequirements.md:689-693`: abort must preflight every required rollback before changing any participant and must reject the entire abort without partial rollback.
- `gwz-core/dev-docs/GwzMergeDesign.md:235-236`: no merge, rebase, cherry-pick, or other incompatible integration state may be in progress.

Concrete reproduction:

I created a temporary two-member workspace. `app` was successfully fast-forwarded by the merge. `docs` was left conflicted. Before abort, I wrote a valid `app/.git/CHERRY_PICK_HEAD` equal to the source commit, leaving porcelain clean and no `MERGE_HEAD` in `app`.

Observed preflight state:

```text
preflight-observation {
  'operation': 'awaiting_resolution',
  'app_abort_eligible': True,
  'app_drift': [],
  'app_porcelain_entries': 0,
  'docs_merge_head_before': True
}
```

Observed abort result:

```text
abort-result MergeDrift: rollback found integration state CherryPick
post-abort-state {
  'docs_merge_head': False,
  'docs_head_restored': True,
  'app_head_still_merged': True,
  'record_has_rolling_back': True
}
```

The operation partially rolled back `docs` before failing on `app`, despite the prompt and REQ-089I requiring zero participant mutation when any selected rollback is not eligible.

Impact:

Default abort can mutate one member and then stop on another member that should have been rejected up front. This breaks the core safety claim for coordinated abort and can leave users with a mixed workspace requiring manual recovery.

Recommended correction:

Expose the complete native repository operation state in the backend observation, not only `MERGE_HEAD` state. The shared status classifier should distinguish expected native merge state from foreign sequencer states such as cherry-pick, revert, rebase, bisect, and any other libgit2 non-clean integration state. Abort/continue preflight and checked rollback must consume the same observation semantics so a state accepted by preflight cannot be rejected only after earlier participants have been mutated.

Regression test:

Add a real-repository M2a abort test with two selected members in this order: conflicted `docs`, fast-forwarded `app`. Inject `CHERRY_PICK_HEAD` or equivalent libgit2 cherry-pick state into `app`, assert `merge status` reports structured drift and `abort_eligible=false`, then assert `merge --abort` fails before `docs` loses `MERGE_HEAD` or moves its head.

### P1 - The open-merge gate is not central, and Python gates the wrong workspace

Files and lines:

- `gwz-core/src/workspace_ops/merge/mod.rs:116-134` implements `enforce_workspace_open_merge_gate`, but it is called by drivers rather than by a single core dispatch path after request workspace resolution.
- `gwz-core/src/workspace_ops/mod.rs:27` publicly exports `handle_branch`.
- `gwz-core/src/workspace_ops/handle_branch.rs:10-44` resolves the request workspace and mutates branch state without enforcing the open-merge gate.
- `gwz-cli/src/globalargs.rs:515-518` runs a Rust CLI pre-dispatch gate before calling handlers.
- `gwz-cli/src/globalargs.rs:670-672` then calls the ungated core `handle_branch`.
- `gwz-py/native/src/dispatch/mod.rs:29-31` applies a static gate before request decoding, using `current_dir()`.
- `gwz-py/native/src/dispatch/mod.rs:78-83` passes `workspace=None` to the core gate.
- `gwz-py/native/src/dispatch/branch_stash.rs:33-43` decodes `BranchRequest` with its own workspace metadata but still gates only the process cwd.
- `gwz-py/native/src/dispatch/merge.rs:19-39` follows the same cwd-gated pattern for merge requests.

Violated requirement or invariant:

- `gwz-core/dev-docs/GWZRequirements.md:671-677`: a single central pre-dispatch gate must block unrelated mutating or publishing operations while recovery remains discoverable.
- `gwz-core/dev-docs/GwzMergePlan.md:94-102`: open-operation gate behavior is a high-fan-out contract that must be frozen before parallel M2b work.

Concrete reproduction:

I created workspace A as the process cwd with no open merge, and workspace B with an open merge in `finalizing`. From cwd A, I used the Python client to send a `branch create` request whose `meta.workspace.root` pointed at B.

Observed result:

```text
merge=finalizing open=True branch=ok ref=<OID>
```

The branch mutation succeeded inside workspace B even though B had an open merge. The gate checked workspace A because Python dispatch gated `env::current_dir()` before using the explicit request root.

Impact:

Python clients can mutate a workspace with an open merge whenever the process cwd is outside that workspace. Direct Rust/core API callers can also bypass the gate by calling exported handlers. This violates the single-gate safety model and can invalidate M2a's promise that the accepted composition stays frozen while a merge is open.

Recommended correction:

Move gate enforcement into a central core request dispatch path after the effective workspace root has been resolved from `request.meta.workspace`. Driver-side checks may remain as early diagnostics, but they must not be the authoritative policy. Direct public handlers should either route through the gate or become internal helpers that cannot be called as an ungated mutation path.

Regression test:

Add a matrix covering Rust CLI, Python bridge/client with an explicit root different from cwd, and direct core handler/API entry. For each gated mutator, create an open merge in the request workspace, run the operation from a different cwd, and assert the request fails before any branch, stash, publish, materialize, or other mutation occurs.

### P1 - A crash or store failure after Git success but before the participant row can strand an open operation

Files and lines:

- `gwz-core/src/workspace_ops/merge/start.rs:185-199` runs `execute_one` first, then applies the participant row and writes the durable record.
- `gwz-core/src/workspace_ops/merge/continue_op.rs:44-55` runs resolve/retry Git work first, then writes the outcome row.
- `gwz-core/src/workspace_ops/merge/continue_op.rs:212-229` commits a resolved native merge before updating the participant record.
- `gwz-core/src/workspace_ops/merge/status.rs:233-248` classifies a head that moved away from the recorded expected head as drift.
- `gwz-core/src/workspace_ops/merge/status.rs:251-315` requires matching native merge state and no drift for continue/abort eligibility.
- `gwz-core/src/workspace_ops/merge/abort.rs:246-273` only has "already restored" tolerance for `OperationState::RollingBack`, not for stale `Executing` or `AwaitingResolution` records after a completed Git action.

Violated requirement or invariant:

- `gwz-core/dev-docs/GWZRequirements.md:651-656`: every subsequent participant and operation transition must be atomically recorded.
- `gwz-core/dev-docs/GWZRequirements.md:681-685`: resume may retry only from a classified unchanged retry point and must reject ambiguous mutation.
- `gwz-core/dev-docs/GWZRequirements.md:689-694`: interrupted abort must be resumable from durable participant state.
- Prompt crash boundary 2 requires reasoning after a Git start/continue action and before its participant outcome write.

Concrete state sequence:

1. Start or continue writes the operation record.
2. A participant Git action succeeds: fast-forward, true merge commit, or conflict-resolution commit.
3. The process crashes or `store.write_open` fails before the participant row is updated.
4. On restart, the durable record still says the participant is `Planned`, `Conflicted`, `Failed`, or `Unattempted`, but the repository has already moved, and in the resolution case the native merge state is gone.
5. Status reports drift instead of adopting the exact completed action. Continue and abort cannot prove the intended exact outcome from the record and can block with the workspace-wide gate still active.

The existing test at `gwz-core/src/workspace_ops/merge/start.rs:883-933` verifies "Git then write then event" ordering, but it does not inject failure after Git and before the row. It proves durable event order, not crash-safe adoption of an already-completed Git action.

Impact:

A common crash point can leave an open operation in a state where the only accurate recovery evidence is in Git, not in the durable operation record. The user can be stuck behind the open-operation gate with no safe continue/abort path except manual record editing or manual Git surgery.

Recommended correction:

Persist a per-action intent record before executing any Git action that can move refs, commit, or alter native merge state. The post-crash classifier should be able to prove one of these exact outcomes: action not started, action completed exactly as intended, action conflicted exactly as intended, or action is ambiguous and requires typed manual recovery. For resolution commits, record enough expected commit identity information before mutation to adopt the exact two-parent commit or reject safely.

Regression test:

Add fault-injection tests for start and continue where the store fails immediately after each successful Git action and before the row write. In a fresh process, assert `merge status` explains the completed-but-unrecorded action, and assert `merge --continue` or `merge --abort` converges without duplicating commits, losing user work, or requiring manual edits.

### P2 - Merge event protocol is incomplete and omits durable write events

Files and lines:

- `gwz-core/src/workspace_ops/merge/start.rs:195-210` emits only `MemberFinished` after row writes; it does not emit `OperationStarted`, `MemberStarted`, `OperationFinished`, or `ArtifactWritten`.
- `gwz-core/src/workspace_ops/merge/continue_op.rs:44-61` emits only `MemberFinished`.
- `gwz-core/src/workspace_ops/merge/abort.rs:157` and `gwz-core/src/workspace_ops/merge/abort.rs:200` emit member start/finish for rollback actions, but operation start/finish and artifact writes are still absent.
- `gwz-core/src/operation/push_event.rs:403-481` exposes operation/member event APIs, but `member_finished` has no structured outcome payload.
- `gwz-core/src/workspace_ops/merge/start.rs:918-928` locks in a trace that lacks member start and artifact written events.

Violated requirement or invariant:

- `gwz-core/dev-docs/GwzMergeDesign.md:1065-1072`: each merge command invocation emits `OperationStarted`/`OperationFinished`; each actionable target emits `MemberStarted`/`MemberFinished` with structured outcome; durable record/evidence writes emit `ArtifactWritten`; event order follows durable order.
- `gwz-core/dev-docs/GWZRequirements.md:636-639`: Rust, Python, human, JSON, and JSONL surfaces must lower to and report the same core semantics.

Concrete reproduction or state sequence:

For merge start and continue, the only member event emitted by core is `MemberFinished`, after a successful or failed row write. There is no operation lifecycle envelope, no target start event for start/continue, no artifact event for durable record writes, and no structured per-member outcome in the finish event. JSONL consumers therefore cannot reconstruct the required command lifecycle from events.

Impact:

Machine consumers receive an incomplete event stream, and future M2b finalization work will have to change event behavior across core, Rust CLI, and Python at the same time. Missing `ArtifactWritten` events also weakens the durable-order observability required by the design.

Recommended correction:

Wrap merge start/status/continue/abort/gc in a lifecycle emitter that always emits operation start and finish. Emit member start before each actionable Git step, emit member finish with a structured outcome after the durable row succeeds, and emit `ArtifactWritten` after every verified durable record/evidence write. Keep outcome/state events after successful writes only.

Regression test:

Add Rust and Python JSONL parity tests for start success, start conflict, continue success, abort success, and failure paths. Assert exact event order: operation start, member start, durable artifact write, member finish with outcome, operation state change where applicable, operation finish.

### P2 - Terminal abort/archive crash points produce contradictory or undiscoverable state

Files and lines:

- `gwz-core/src/workspace_ops/merge/model.rs:49-52` reports `Aborted` as not open.
- `gwz-core/src/workspace_ops/merge/abort.rs:203-211` writes `OperationState::Aborted` and then separately archives the record.
- `gwz-core/src/workspace_ops/merge/store.rs:22-34` discovers any record file under `.gwz/merge` as the open operation.
- `gwz-core/src/workspace_ops/merge/store.rs:83-124` archives by rename plus sync/verify/retention, with multiple failure points after the open record has become terminal or has moved.
- `gwz-core/src/workspace_ops/merge/response.rs:72` derives response `open` from `state.is_open()`.
- `gwz-core/src/workspace_ops/merge/validate.rs:33-36` rejects status by merge id.

Violated requirement or invariant:

- `gwz-core/dev-docs/GWZRequirements.md:660-668`: lifecycle state and status must be structured and read-only.
- Prompt crash boundaries 8 and 9 require clear behavior after terminal aborted write and after archive publication.
- `gwz-core/dev-docs/GwzMergePlan.md:950-960` requires finalization/archive fault points to remain explainable and idempotent for M2b.

Concrete state sequence:

Boundary 8: if the process crashes after the record is written as `aborted` but before archive, the open directory still contains the record, so the gate can still discover and block on it. The response projection for that same record says `open=false` because `OperationState::Aborted` is not open.

Boundary 9: if archive rename succeeds but a later sync, verification, retention, or response path fails, the open record is gone. Default status without an open operation reports idle, and status by merge id is deliberately unsupported. The store can load from `done`, but the user-facing status/abort path cannot explain the just-archived operation by id.

Impact:

Users can see contradictory state (`open=false` while the gate still treats an open-dir record as active), or they can lose user-visible discoverability of an operation after archive movement but before a command result is observed. This is lower risk than the preflight bugs, but it is a real recovery diagnosability defect at a required crash boundary.

Recommended correction:

Represent terminal archival as an explicit durable closing state, for example `archive_pending`, and derive `open` consistently from both lifecycle and storage location. Support idempotent status/retry by merge id across open and done records, at least for recently archived terminal operations. Archive should be a resumable operation with tested fault points before rename, after rename, after directory sync, after verification, and during retention.

Regression test:

Inject failures after the `Aborted` write and at each archive step. Assert status and the gate report the same open/closed truth, retrying abort completes archive idempotently, and status by id can explain the terminal record after the record moves to `done`.

### P2 - Status collapses diverged and missing-object drift into generic advanced/rewound errors

Files and lines:

- `gwz-core/src/workspace_ops/merge/status.rs:95-100` internally distinguishes `Equal`, `Advanced`, `Rewound`, `Diverged`, and `Missing`.
- `gwz-core/src/workspace_ops/merge/status.rs:97-98` uses `backend.is_ancestor(...)?`; missing object errors escape as backend errors rather than becoming structured participant drift.
- `gwz-core/src/workspace_ops/merge/status.rs:233-248` maps `Diverged` to `HeadAdvanced` and `Missing` to `HeadRewound`.
- `gwz-core/src/workspace_ops/merge/model.rs:257-268` has no durable/protocol drift kind for `HeadDiverged` or `ObjectMissing`.
- `gwz-core/src/workspace_ops/merge/response.rs:234-235` exposes only `HeadAdvanced` and `HeadRewound` for head drift.

Violated requirement or invariant:

- `gwz-core/dev-docs/GWZRequirements.md:664-667`: status/recovery must report structured participant drift rather than silently adopting live state or collapsing materially different states.
- Prompt review area D explicitly requires distinct advanced, rewound, diverged, and missing-object handling.

Concrete reproduction or state sequence:

If a participant head is neither ancestor nor descendant of the recorded expected head, the classifier detects `HeadRelation::Diverged` internally but reports `ParticipantDriftKind::HeadAdvanced`. If the recorded expected object is missing from the repository, the ancestry check can fail out as a backend error before a participant drift response is built. In both cases the status protocol loses the distinction needed for safe recovery guidance.

Impact:

Users and automated recovery tooling cannot tell whether a participant merely advanced, rewound, diverged, or lost an expected object. The recommended action differs materially: preserving post-merge work, restoring an expected object, or rebuilding a repository are not equivalent recovery steps.

Recommended correction:

Add protocol-visible drift kinds for `head_diverged` and `object_missing` or equivalent names. Convert missing-object ancestry failures into structured participant observations carrying expected/live commit fields and typed guidance. Keep backend errors for true backend failures, not expected recovery-classified object absence.

Regression test:

Add status tests for a participant with a diverged head and for a participant where the recorded expected commit object is absent. Assert JSON/human/Python expose distinct drift kinds and that continue/abort eligibility blocks with precise guidance.

### P2 - Conflict-resolution commits ignore request-provided Git identity

Files and lines:

- `gwz-core/src/workspace_ops/merge/continue_op.rs:223-228` calls `commit_merge_resolution_checked` without passing request attribution.
- `gwz-core/src/git/gitbackend.rs:1799-1820` defines `commit_merge_resolution_checked` without an attribution parameter and creates the commit using `merge_signature(&repo)`.
- `gwz-core/src/git/gitbackend.rs:2474-2480` resolves `merge_signature` from repository config or the gwz fallback.
- `gwz-core/src/git/gitbackend.rs:2483-2498` has request-attribution-aware `merge_signatures`, used by start-time true merges but not by continue resolution commits.

Violated requirement or invariant:

- `gwz-core/dev-docs/GWZRequirements.md:599-600`: merge operations that create Git commits must use request-provided Git identity when present, subject to driver policy.

Concrete reproduction or state sequence:

Start a merge that conflicts, then continue it with explicit request author/committer identity. The resolution commit is created by `commit_merge_resolution_checked`, which has no way to receive that identity and instead uses the member repository identity or fallback. A start-time true merge does use the attribution path, so identity behavior differs depending on whether the true merge conflicts.

Impact:

Two merge commits created by the same first-class merge operation can have different attribution semantics. This breaks protocol expectations for clients that supply identity explicitly and can make audit trails inconsistent.

Recommended correction:

Extend the checked resolution commit path to accept operation attribution and use the same `merge_signatures` behavior as start-time true merges. If the intended product behavior is that continue always uses repository identity, reconcile `REQ-089` and the protocol docs before release and test that policy explicitly.

Regression test:

Create a conflicted merge with explicit request author and committer identities, resolve and continue it, then inspect the resulting commit author/committer. The test should fail today because the continued commit uses repo/fallback identity rather than the request identity.

## Design And Invariant Verification

A. Lifecycle and durability: Partially satisfied. The operation record is written before ordinary start mutations, unknown fields are preserved, store writes use temp/rename/sync/verify, and finalizing is represented as open state. Defects remain at the post-Git/pre-row crash boundary and terminal archive boundary.

B. Start/continue: Partially satisfied. M2a correctly enters `finalizing` after all member success (`gwz-core/src/workspace_ops/merge/mod.rs:182-190`) and `continue` is idempotent while already finalizing (`gwz-core/src/workspace_ops/merge/continue_op.rs:27-29`). The post-Git/pre-row crash gap and continue identity gap remain.

C. Abort: Partially satisfied. Abort acquires the workspace mutator lock, preflights all selected participants, unwinds in reverse order, and uses checked ref updates. The foreign-sequencer finding proves that preflight does not observe all native integration states, allowing partial rollback.

D. Status/drift: Partially satisfied. Status is read-only and reports live versus recorded state, conflict paths, drift, and eligibility. It does not expose full native integration state, collapses diverged/missing-object cases, and therefore can mark unsafe participants eligible.

E. Crash/idempotence: Not yet sufficient. Boundaries 1, 3, 4, 5, 6, 7, and 10 have plausible idempotent behavior. Boundaries 2, 8, and 9 are defective or ambiguous.

F. Gating/locking: Partially satisfied. Start/continue/abort acquire the workspace mutator lock. The open-operation gate is not authoritative in core and can be bypassed through Python explicit-root requests or direct handler calls.

G. Routing surfaces: Partially satisfied. Rust CLI invokes the gate before dispatch and docs keep status/continue/abort hidden until M2b. Python routing gates cwd rather than the decoded request workspace. Public core handlers remain ungated.

H. Events: Not sufficient. Operation lifecycle, member-start for start/continue, structured member outcomes, and durable `ArtifactWritten` events are missing for merge.

I. Output/protocol parity: Mostly satisfied for the current hidden M2a surfaces and fixtures, with the drift-kind gap noted above. Public release remains appropriately deferred until M2b.

J. Testing/docs/release: Verification is green and real workflow coverage in `g23` is useful: finalizing, identity for initial true merge, dry-run, mixed conflict continue/abort, post-merge drift, failed/unattempted preflight, and unrelated staged work. Missing coverage includes foreign Git sequencer states, cross-surface gate bypasses, post-Git/pre-record fault injection, archive fault points, status missing-object cases, and continue identity.

Positive observations:

- M2a does not publish composition evidence, does not advance the accepted root lock, and does not report `completed` before M2b.
- The baseline lock remains frozen while the merge is open.
- The file store is stronger than a simple write: same-directory temp file, file and directory sync, rename, read-back verification, unknown-field preservation, duplicate/unreadable fail-closed behavior, and retention safeguards are present.
- Root merge participation is rejected/deferred as required for M2a.
- Whole-operation abort preflight exists in structure; the defect is incomplete observation, not missing preflight.
- Human and machine status rendering include useful participant detail for the states currently represented.

## Crash-Matrix Results

1. After durable record creation, before the first Git action: safe/idempotent. Status can discover the open record. Abort can mark planned participants aborted without Git rollback. No Git mutation has occurred.

2. After a Git start/continue action, before its participant outcome write: defective. A successful ref move, native merge, or resolution commit can be present in Git but absent from the record, leaving status/continue/abort unable to prove exact recovery. See P1 finding 3.

3. After a participant write, before its event: safe for durable recovery, lossy for observability. The durable row is authoritative and retry/status can proceed, but the event stream can miss required member/artifact events. See P2 finding 4.

4. After transition to `executing`, before the first continue action: safe/idempotent. `handle_continue` accepts `Executing` and reruns preflight before actions. If no Git action started, retry should converge.

5. After transition to `rolling_back`, before the first rollback: safe/idempotent. The operation remains open and retrying abort resumes rollback from durable state before any rollback mutation.

6. After Git rollback succeeds, before the rollback row write: mostly safe for the represented states. The `already_restored` logic handles a `RollingBack` record whose live commit is already back at `before_commit` and whose drift is limited to expected rollback effects. This still depends on the same incomplete native-state classifier noted in finding 1.

7. After a rollback row write, before the next rollback: safe/idempotent. Retrying abort skips rows already marked `Aborted` or `RolledBack` and continues later selected targets.

8. After transition to `aborted`, before archive: defective/contradictory. The record remains under the open directory and can still gate the workspace, while response projection says `open=false`. See P2 finding 5.

9. After archive publication, before the command returns: ambiguous for user-visible recovery. If the record moved to `done`, default status reports idle and status by merge id is not available, even if the command failed or the user never observed the terminal response. See P2 finding 5.

10. After all member success and transition to `finalizing`, before M2b exists to publish composition evidence: expected internal handoff. The operation remains open, continue is idempotent, publication is not attempted, and public release is correctly deferred until M2b implements finalization.

## Verification Commands And Results

All required verification commands completed successfully.

- `cargo fmt --all -- --check`: PASS.
- `cargo test --workspace --all-targets --no-fail-fast`: PASS. Useful observed counts include `gwz-cli` lib 72 passed, `diff_workflows` 25 passed, `local_workflows` 25 passed, `publish` 4 passed, `release_script` 1 passed, `gwz-core` lib 425 passed and 1 ignored, `diff_render_spike` 10 passed, `protocol` 26 passed, and `publish_workflow` 4 passed.
- `cargo clippy --workspace --all-targets --all-features -- -D warnings`: PASS, with the existing non-fatal profile warning.
- `cargo fmt --manifest-path gwz-py/Cargo.toml -- --check`: PASS.
- `cargo clippy --manifest-path gwz-py/Cargo.toml --all-targets --all-features -- -D warnings`: PASS.
- `gwz-py/.venv/bin/python -m maturin develop --manifest-path gwz-py/Cargo.toml`: PASS; installed editable `gwz 0.0.0`.
- From `gwz-py`: `.venv/bin/python scripts/regen_protocol.py --check`: PASS (`regen_protocol: OK`).
- From `gwz-py`: `.venv/bin/python -m pytest -q`: PASS (`265 passed in 6.42s`).
- From `gwz-cli`: `python3 scripts/generate_cli_reference.py --check`: PASS.
- From `gwz-cli`: `uvx --from 'mkdocs-material>=9.5,<10' mkdocs build --strict --site-dir /tmp/gwz-m2a-review-56-docs`: PASS; docs built successfully with a MkDocs future warning only.
- `bazel build //gwz-core:gwz_core //gwz-cli:gwz`: PASS; both requested targets built, cache hit.
- `git diff --check`: PASS.
- `git -C gwz-core diff --check`: PASS.
- `git -C gwz-cli diff --check`: PASS.
- `git -C gwz-py diff --check`: PASS.

## Residual Risks And Missing Coverage

- Review and focused repros were run on macOS only. Linux and Windows filesystem, path, lock, rename, and Git-state behavior were not independently exercised.
- I did not physically power-fail the machine or inject filesystem-level torn writes; durability conclusions are based on code inspection and in-process fault reasoning.
- Concurrency coverage remains limited. Simultaneous CLI/Python mutators, direct core API callers, and external Git commands racing recovery need stronger tests.
- Property/fuzz coverage for durable record schema preservation, malformed records, duplicate records, and protocol roundtrips is not exhaustive.
- Archived status, preservation, retention, root participation, and GC are intentionally deferred in the plan, but M2a archive behavior still affects required abort recovery boundaries.
- The docs contain some tension around merge commit identity: `REQ-089` requires request-provided identity when present, while design prose also says some resolution identities are resolved by Git at commit time. This should be reconciled before public release.

## Recommended Next Action

Do not begin shared M2b integration work on top of these interfaces. Fix the three P1 items first because they affect high-fan-out contracts: native state classification, central gate placement, and post-Git/pre-row crash recovery. Then fix or explicitly scope the P2 event/archive/drift/identity issues before the M2b release gate.

Isolated M2b experiments that do not consume or expand the current gate/status/recovery contracts can continue off the integration path, but the M2b finalization, publication, driver, and event work should wait until these M2a recovery contracts are corrected and covered by regression tests.
