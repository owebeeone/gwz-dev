# GWZ Merge M1/M2a Code Review Remediation Plan

Date: 2026-07-19  
Execution status: **implementation and verification complete; independent
re-review pending** (2026-07-19). The integrated gate passes 638 Rust tests
(plus 1 ignored platform/process test), 268 Python tests, strict Rust Clippy,
protocol regeneration checks, strict documentation build, and both Bazel
targets. This plan remains formally open only for the independent P0-P2
re-review required by section 9.

Inputs:

- `dev-docs/GwzDevCodeM2a-ReviewF5.md`
- `dev-docs/GwzDevCodeM2a-Review56.md`

## 1. Assessment

Remediation is required before shared M2b finalization, publication, driver,
or event integration begins.

The reviews are complementary. F5 found the inescapable
`recovery_required` state and exact-restoration abort gaps; Reviewer 56 found
that incomplete native Git-state observation can let abort preflight approve
a participant that the checked rollback primitive later rejects, after another
participant has already changed. Both independently reproduced the Python
explicit-root gate bypass. Together they identify four high-fan-out contracts
that must be corrected before M2b builds on them:

1. complete native repository-state observation shared by status and recovery;
2. a lifecycle with a supported exit from every open state and an exact
   post-crash action-reconciliation contract;
3. authoritative gate enforcement against the request's effective workspace,
   under the same mutator lock as the protected operation; and
4. abort/close semantics that recognize verified no-op states and remain
   internally consistent through archive interruption.

The durable store, baseline-lock freeze, exact merge plan, checked Git
primitives, reverse rollback ordering, machine response parity, and M2a
`finalizing` handoff remain useful foundations. This is a focused correction
of shared recovery interfaces, not a rewrite of merge.

M2b-A1 marker-model work and isolated M2b-B scoped-root-commit experiments may
continue on non-integrated branches because they do not consume the defective
interfaces. M2b-A2 finalization, M2b-C driver/event completion, and shared M2b
integration remain blocked until this plan's remediation gate passes.

## 2. Finding dispositions

| Review finding | Disposition | Remediation |
| --- | --- | --- |
| F5 P1-1: `recovery_required` has no exit | Fix before M2b | Permit guarded reclassification into continue or rollback; do not persist this state for routine post-action observation drift. |
| F5 P2-2 / 56 P1: Python and direct-core gate bypass | Fix before M2b | Resolve the request workspace first and enforce one authoritative core policy under the workspace mutator lock. Driver checks become optional early diagnostics. |
| F5 P2-3: raw native abort wedges coordinated abort | Fix before M2b | Recognize an exactly restored conflicted participant as an abort no-op in every abortable open state. Correct the user documentation with the code change. |
| F5 P3-4 / 56 P1: Git success before participant-row write | Fix before M2b | Persist per-action intent before Git mutation and reconcile exact not-started, conflicted, completed, or ambiguous outcomes after restart. |
| F5 P3-5: stale M0 documentation | Fix before public release | Describe the frozen baseline lock, current merge message, coordinated recovery, and actual feature boundary. |
| F5 P3-6 / 56 P2: missing-object and collapsed head drift | Fix before M2b release | Expose distinct diverged and missing-object drift with member context; do not leak an opaque backend error. |
| F5 P3-7 / 56 P2: incomplete merge events | Fix in the remediation integration wave | Emit complete operation, member, outcome, state, and durable-artifact events in durable order. |
| F5 P3-8: ancestor discovery crosses a nested workspace | Fix with central gate | Stop implicit discovery at the nearest workspace boundary while still checking recovery state before parsing that boundary's manifest. |
| F5 P3-9: new work in an already rolled-back participant blocks resume | Fix before release | For a row already durably restored, verify the recorded target ref and skip it without requiring a clean worktree/index that GWZ will not mutate. |
| F5 P3-10: conditional stage gate lacks coverage | Fix | Add real core and driver tests for conflicted, merged, unaffected, and root targets. |
| F5 P3-11: unvalidated paths from the durable record | Fix | Validate every participant path while loading/classifying the record, before observation or mutation. |
| 56 P1: foreign Git sequencer state passes abort preflight | Fix first | Add complete repository-operation state to the shared observation and make preflight and checked actions consume the same classification. |
| 56 P2: terminal abort/archive contradiction | Fix the open-directory inconsistency | Derive `open` from lifecycle plus storage location and make archive retryable. Archived status by id remains M3 scope. |
| 56 P2: resolution identity ignores request attribution | Resolve contract, then fix | A commit uses attribution from the request that creates it; otherwise it uses repository identity. Apply the same rule to clean retries and resolution commits. |

Reviewer 56's proposed foreign-state regression ordering needs one correction:
the poisoned participant must be earlier in execution order so reverse rollback
reaches it only after a later participant would have changed. The required test
below uses that order.

The request to support general archived-status lookup by merge id remains in
M3. This remediation covers only truthful open/closed projection, idempotent
archive completion, and the fault points needed by abort and M2b finalization.

## 3. Lead-owned interface checkpoint

The lead must freeze the following decisions in `GwzMergeDesign.md`,
`GWZRequirements.md`, and `GwzMergePlan.md` before parallel implementation.
The checkpoint includes compiling stubs and focused contract tests, not the
complete feature implementations.

### 3.1 Complete native repository state

Add a backend observation that distinguishes a clean repository, the expected
native merge state, and every other Git integration/sequencer state exposed by
the backend. A native merge detail object remains responsible for exact
`MERGE_HEAD` and conflict paths; it is not used as the absence/presence test
for all other repository states.

The shared participant snapshot becomes the only authority consumed by status,
continue, abort, and checked recovery actions:

- the expected recorded merge is eligible only when repository state is
  `merge` and exact merge details match;
- a non-conflicted participant is eligible only when repository state is
  clean, unless its recorded state is a verified no-op that requires no Git
  mutation;
- cherry-pick, revert, rebase, mailbox/apply, bisect, unknown, or any other
  non-clean state is structured foreign-integration drift;
- a participant accepted by whole-operation preflight cannot be rejected by a
  checked action solely because the two paths interpreted repository state
  differently.

Append protocol enum values where necessary; do not renumber existing values.

### 3.2 Durable action intent and exact reconciliation

Extend the internal participant record with an optional, additive
`pending_action` object. Preserve it through older-reader unknown-field
round-trips. It records, before a Git action:

- action kind: up-to-date verification, fast-forward, true merge, native
  conflict attempt, conflict-resolution commit, or retry;
- recorded before/source commits, target branch, and exact message already
  frozen by the operation;
- expected analysis/result class needed to distinguish a completed action from
  unrelated drift; and
- an attempt identifier if needed to make repeated writes unambiguous.

The write order is:

1. emit `MemberStarted` for the actionable participant;
2. write and verify `pending_action`;
3. emit `ArtifactWritten` for the intent;
4. execute the checked Git action;
5. atomically write the participant outcome and clear `pending_action`;
6. emit `ArtifactWritten`, then `MemberFinished` with the durable outcome.

After restart, the shared classifier recognizes exactly one of:

- **not started:** before ref, branch, clean index/worktree, and clean
  repository state still match;
- **expected conflict:** before ref and exact expected native merge state
  match;
- **completed exactly:** fast-forward at the exact source, or a clean merge /
  resolution commit with the exact recorded parents, message, target ref, and
  clean repository state;
- **ambiguous:** anything else, reported as structured recovery-required
  drift without mutation.

Exact outcomes may be adopted durably; ambiguous outcomes never are. This
contract applies to start, failed/unattempted retry, and resolution commit
paths. It is preferable to special-casing only resolution commits because the
same Git-success/store-failure boundary exists for every mutating action.

### 3.3 Recoverable operation states

`recovery_required` means that automatic action is currently blocked pending
manual restoration or a newly supported classifier. It must not mean that the
record can never be discharged.

- allow `RecoveryRequired -> Executing` after a fresh complete continue
  preflight proves an exact retry/adoption point;
- allow `RecoveryRequired -> RollingBack` after a fresh complete abort
  preflight proves every remaining rollback or no-op;
- reserve the future `RecoveryRequired -> Preserving` path for M3;
- leave the record in `RecoveryRequired` when reclassification remains
  ambiguous; and
- for routine drift found only by the post-action observation, report the drift
  without converting an otherwise meaningful lifecycle state into a trap.

Every open state must have a tested GWZ path to completion, safe abort, or a
specific manual correction after which one of those commands succeeds.

### 3.4 Authoritative workspace gate

Introduce one core guard API that:

1. resolves the effective workspace from `request.meta.workspace.root` when
   present, otherwise from the nearest workspace boundary;
2. acquires the workspace mutator lock for gated mutating operations;
3. discovers the open merge for that exact workspace;
4. applies the central gate table; and
5. retains the guard through the protected mutation.

Public mutating core handlers must use this guard or become internal helpers
behind an entry point that does. A caller must not be able to bypass the gate
by invoking an exported handler. Rust and Python may retain early checks for
faster diagnostics, but those checks are not authoritative.

For implicit recovery discovery, inspect a directory's `.gwz/merge` before
parsing its manifest, then stop at the first directory containing the workspace
manifest. This preserves recovery through a conflicted/invalid manifest while
preventing a valid nested workspace from being captured by an enclosing
workspace's operation.

The conditional stage path uses the same resolved root and lock, then applies
its recorded-conflicted-participant restriction.

### 3.5 Abort no-op and interrupted-close semantics

Abort classification distinguishes participants GWZ must mutate from verified
no-ops:

- a recorded conflicted row at the exact before ref, clean index/worktree, and
  clean repository state is an already-restored no-op even before
  `rolling_back`;
- a durably `aborted` or `rolled_back` row is skipped after verifying its
  recorded target ref remains at the before commit; later unrelated
  worktree/index content is reported but does not block rollback elsewhere;
- a row that still requires mutation retains the strict clean/exact checks;
  and
- foreign integration state in any row that must be changed blocks the whole
  abort before the first mutation.

An `Aborted` or future `Completed` record still located in the open-record
directory is in closing/archive-pending storage, even if its lifecycle state is
terminal. Status reports it as still requiring close completion, the gate and
response agree on that fact, and retry completes archive idempotently. Once it
is verified in `done/`, it is closed. General historical lookup remains M3.

### 3.6 Drift, path, identity, and event contracts

- append distinct drift values for `head_diverged`, `object_missing`, and
  `foreign_integration_state`;
- attach member id/path and expected/live evidence to every such drift;
- validate durable participant paths as workspace-relative member paths before
  filesystem access;
- use author/committer attribution supplied on the request that creates a Git
  commit, otherwise resolve identity from the repository, for both retry and
  conflict-resolution commits; and
- implement the existing event design literally: operation start/finish,
  member start/finish with structured outcome, operation-state changes, and
  artifact-written events after verified durable writes.

## 4. Parallel implementation wave

After the checkpoint in section 3 is committed, R1 through R4 may proceed in
parallel. The lead allocates any unavoidable shared-file edits before agents
start. Agents do not independently alter the frozen record, drift, gate, event,
or backend trait contracts.

### R1 — Native state and structured classifier

Owner: Git/status agent. Suggested budget: at most 500 handwritten changed
lines.

Primary ownership:

- `gwz-core/src/git/gitbackend.rs` native observation implementation;
- focused backend tests under `gwz-core/src/git/tests/`;
- `gwz-core/src/workspace_ops/merge/status.rs`;
- status/drift-focused tests.

Tasks:

1. Implement the complete repository-state observation frozen in section 3.1.
2. Make the shared classifier produce distinct foreign-state, diverged-head,
   and missing-object observations without returning a memberless backend
   error.
3. Add exact completed-action recognition helpers consumed by R2 without
   writing the record from status.
4. Validate participant paths before repository observation.
5. Prove status remains strictly read-only.

Required tests:

- every backend repository-state variant available on the platform;
- missing expected and resulting commit objects;
- advanced, rewound, diverged, detached, and missing-head cases;
- traversal and absolute paths in a durable record;
- exact true-merge and resolution commit recognition;
- foreign state produces `abort_eligible=false` for a row requiring mutation.

### R2 — Action journal, continue, and lifecycle recovery

Owner: lifecycle/continue agent. Suggested budget: at most 550 handwritten
changed lines.

Primary ownership:

- a new `merge/reconcile.rs` if useful;
- `merge/start.rs` action write ordering;
- `merge/continue_op.rs`;
- lifecycle/action fault-injection tests.

Tasks:

1. Write `pending_action` durably before every mutating start/continue action.
2. Clear it only in the same durable participant-outcome write.
3. Adopt exact post-crash outcomes using R1's read-only classifier.
4. Permit guarded continue from `RecoveryRequired` and preserve that state on
   remaining ambiguity.
5. Stop persisting `RecoveryRequired` merely because routine drift appeared
   between the last outcome write and final observation.
6. Pass request attribution through the checked resolution-commit path and
   use the same identity policy as retry.

Required fault tests inject store failure after Git success and before the row
write for:

- fast-forward start;
- clean true-merge start;
- conflicted start;
- failed/unattempted retry producing each of those outcomes; and
- conflict-resolution commit.

After a fresh-process-style reload, status must explain the exact pending
action and continue or abort must converge without duplicate commits or manual
record editing.

### R3 — Coordinated abort and archive recovery

Owner: abort/store agent. Suggested budget: at most 500 handwritten changed
lines.

Primary ownership:

- `merge/abort.rs`;
- abort-specific additions to `merge/store.rs`;
- open/closed response projection allocated by the lead;
- abort/archive fault tests.

Tasks:

1. Consume R1's full repository-state classifier during whole-operation
   preflight.
2. Recognize exact externally restored conflicts as no-op rows.
3. Skip already durable rollback rows without treating later worktree changes
   as work GWZ must discard.
4. Permit guarded abort from `RecoveryRequired`.
5. Make terminal-record archive completion retryable and make response `open`
   agree with storage location.
6. Preserve reverse order, expected-current ref updates, baseline verification,
   and zero mutation on any required-action blocker.

The foreign-state regression uses execution order:

```text
app   fast-forwarded, then given foreign cherry-pick state
docs  conflicted
```

Reverse rollback would touch `docs` before `app`; the test asserts preflight
rejects first, leaving `docs`' `MERGE_HEAD`, both refs, and the record unchanged.

Additional tests cover raw `git merge --abort` before coordinated abort,
post-rollback untracked/staged work, record-write interruption between rollback
rows, terminal-write interruption before archive, and failures at every archive
publication step.

### R4 — Core and driver gate correction

Owner: dispatch/gate agent. Suggested budget: at most 600 handwritten changed
lines because this is deliberately a high-fan-out audit.

Primary ownership:

- `gwz-core/src/operation/open_merge_gate.rs`;
- new shared core guard wiring allocated at the checkpoint;
- `merge/recovery.rs` workspace-boundary discovery;
- affected mutating handler entry points;
- Rust and Python dispatch gate wiring and tests.

Tasks:

1. Route every mutating public core entry through the authoritative guard.
2. Decode Python requests before resolving/gating their workspace.
3. Use explicit request root when supplied and nearest workspace otherwise.
4. Hold the mutator lock across the final gate check and mutation to close the
   gate-check/start race.
5. Keep the command classification exhaustive so a new operation cannot
   silently default to ungated.
6. Stop ancestor discovery at a nested workspace boundary.
7. Test the conditional stage exception through both drivers.

The test matrix covers Rust CLI, Python `Client(root=W)` invoked from outside
`W`, and direct core handler/API calls for branch, stash, capture, commit,
stage, materialize, repo mutation, snapshot, pull, push, tag, and merge start.
Read-only/list/recovery rows remain available. Every blocked case asserts no
ref, index, worktree, lock, manifest, or operation-record change.

## 5. Integration completion wave

R5 begins only after R1-R4 are individually green and integrated. It may edit
their operation paths because no parallel production-code ownership remains.

### R5 — Events, parity, documentation, and release-gate tests

Owner: lead plus driver/parity agent. Suggested budget: at most 450 handwritten
changed lines, excluding fixtures and documentation.

Tasks:

1. Add complete event emission to start, status, continue, abort, and archive
   completion without emitting a durable outcome before its write succeeds.
2. Include structured participant outcomes in `MemberFinished` and durable
   artifact identity/path in `ArtifactWritten`.
3. Pin Rust/Python JSONL event parity for start success, conflict, continue,
   recovery-required rejection, abort success, and failure paths.
4. Add identity tests that inspect author and committer on immediate clean
   merges, continue retries, and resolution commits.
5. Update `gwz-cli/docs/commands/merge.md`, `MachineOutput.md`, and
   `Releases.md` to remove M0 behavior and unsafe raw-Git guidance.
6. Update `GwzMergeDesign.md`, `GWZRequirements.md`, and `GwzMergePlan.md` with
   the final accepted contracts and mark the prior M2a gate superseded by this
   remediation gate.
7. Add one end-to-end real-Git scenario combining clean, conflicted, restored,
   foreign-state, crash-reload, continue, and abort behavior.

Expected event order for an actionable member is:

```text
OperationStarted
MemberStarted
ArtifactWritten        # pending action
ArtifactWritten        # durable outcome
MemberFinished         # carries that outcome
OperationStateChanged  # after durable state transition, when applicable
OperationFinished
```

The Git action occurs between the two artifact events. On failure,
`OperationFinished` still closes the invocation event stream. The
non-negotiable rule is that result/state events follow their durable write.

## 6. Integration order and change control

1. Lead lands the section 3 design/requirements/plan decisions and compiling
   additive interfaces.
2. Branch R1, R2, R3, and R4 from that exact checkpoint.
3. Integrate R1 first because its observation semantics are consumed by R2 and
   R3.
4. Integrate R2, then R3; resolve only call-site adaptations to the frozen R1
   interface.
5. Integrate R4 after auditing every public mutating entry point.
6. Run the focused remediation suite before opening R5.
7. Complete R5 events, parity, docs, and end-to-end tests.
8. Run the full gate in section 8.
9. Obtain an independent re-review of the integrated remediation before M2b-A2
   begins.
10. Use `gwz add`, `gwz commit`, and the root composition workflow only after
    the user requests integration commits. Do not push unless separately
    requested.

Stop parallel feature work and return to lead-owned interface work if any lane
requires:

- renumbering an existing protocol enum;
- weakening whole-operation preflight or expected-current updates;
- adopting an outcome that is not byte/parent/ref/message exact;
- allowing a driver-only gate to be authoritative;
- discarding worktree/index state in a participant GWZ must mutate;
- hand-editing an operation record as a supported recovery path; or
- broadening this remediation into preserve, `@root`, strategy, or archived
  history features assigned to later milestones.

## 7. Required crash and adversarial matrix

The integrated implementation must state and test the result at each boundary:

| Boundary | Required result |
| --- | --- |
| Record created before first action intent | Status discovers it; continue or abort is safe. |
| Pending action written before Git | Retry proves not-started or later adopts an exact outcome. |
| Git succeeds before participant outcome write | Reload reconciles exact outcome; no duplicate commit and no permanent gate trap. |
| Participant outcome written before event | Durable state wins; replay does not repeat mutation. |
| Continue enters executing before first action | Continue/abort re-preflight safely. |
| Recovery-required manual correction completed | Fresh preflight permits supported continue or abort. |
| Abort enters rolling-back before first action | Retry resumes the same complete reverse plan. |
| Git rollback succeeds before row write | Exact restored state is recognized idempotently. |
| Rollback row written before next action | Completed row is skipped; later user work there is preserved. |
| Terminal state written before archive | Status/gate agree it is closing; retry archives. |
| Archive rename succeeds before command return | The verified done record remains retained and the operation is safely closed; no duplicate archive is created. |
| Foreign sequencer appears in the last reverse-preflight action | Entire abort rejects before any earlier reverse action mutates. |
| Explicit Python root differs from cwd | Gate applies to the explicit root only. |
| Nested workspace lies inside an open outer workspace | Inner commands use the inner boundary and are not falsely gated. |

## 8. Verification gate

Run from `/Users/owebeeone/limbo/gwz-dev` unless noted:

```text
cargo fmt --all -- --check
cargo test --workspace --all-targets --no-fail-fast
cargo clippy --workspace --all-targets --all-features -- -D warnings

cargo fmt --manifest-path gwz-py/Cargo.toml -- --check
cargo clippy --manifest-path gwz-py/Cargo.toml \
  --all-targets --all-features -- -D warnings
gwz-py/.venv/bin/python -m maturin develop \
  --manifest-path gwz-py/Cargo.toml

cd gwz-py
.venv/bin/python scripts/regen_protocol.py --check
.venv/bin/python -m pytest -q
cd ..

cd gwz-cli
python3 scripts/generate_cli_reference.py --check
uvx --from 'mkdocs-material>=9.5,<10' mkdocs build --strict \
  --site-dir /tmp/gwz-m2a-remediation-docs
cd ..

bazel build //gwz-core:gwz_core //gwz-cli:gwz

git diff --check
git -C gwz-core diff --check
git -C gwz-cli diff --check
git -C gwz-py diff --check
```

Record exact test counts. Passing the existing suite without the new focused
fault and gate tests is not sufficient; both reviews passed the existing 597
Rust tests and 265 Python tests while the defects remained.

## 9. Remediation exit criteria

Remediation is complete only when:

- every native non-clean repository state is observed consistently before
  abort or continue mutation;
- the poisoned-late-abort test proves zero mutation in all participants;
- `RecoveryRequired` can be discharged after exact manual restoration and no
  normal observation-drift path creates a permanent trap;
- every mutating Git action has durable pre-action evidence and exact
  post-crash reconciliation;
- start, retry, and resolution store-failure tests converge without duplicate
  commits, lost work, or record editing;
- Rust CLI, Python explicit-root, nested-workspace, and direct-core gate tests
  all enforce the same policy under the mutator lock;
- raw native abort of a conflicted participant is recognized as an exact abort
  no-op;
- an interrupted rollback remains resumable when an already restored member
  later contains unrelated work;
- diverged, missing-object, foreign-state, and invalid-path cases are typed,
  member-scoped, and read-only in status;
- pre-archive abort/close fault points remain discoverable and idempotent, a
  verified post-rename record remains safely retained, and historical lookup
  remains clearly assigned to M3;
- immediate, retry, and resolution commits follow one documented attribution
  policy;
- operation, member, state, and artifact events are complete and byte/semantic
  parity is pinned across Rust and Python;
- user-facing documentation contains no internal milestone labels, stale M0
  lock/message claims, or guidance that creates an unrecoverable coordinated
  state;
- all commands in section 8 pass with generated artifacts clean; and
- an independent re-review reports no P0/P1/P2 defect in the corrected M1/M2a
  contracts before M2b-A2 and M2b-C resume.
