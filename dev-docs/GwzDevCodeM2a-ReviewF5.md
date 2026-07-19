# GWZ Merge M1/M2a — Independent Code Review (F5)

Date: 2026-07-19. Reviewer designation: **F5** (Claude Fable 5), read-only
review. Reviewed as safety-critical Git tooling per the M2a review charter.

Reviewed commits (verified against the live checkouts before review; all four
heads matched the requested checkpoint exactly, working trees clean except the
pre-existing untracked out-of-scope docs and the review prompt itself):

| Repository | Base | Head reviewed | Range |
| --- | --- | --- | --- |
| workspace root | `3d6ce941e7f79eb7aa4e93a1ddb4a865f17f9e90` | `585e44d07f13880fe60072b7409a9d559f64ae53` | `3d6ce941..585e44d` |
| `gwz-core` | `8a0ad1732ed4efc51bb6d264067c18b1f7e3f920` | `cf70d360925563f82d92d93d9522e91d3567a5f3` | `8a0ad173..cf70d36` |
| `gwz-cli` | `f4e0261c3db33e0956331fa9fa576f123ae0f1e1` | `9d634709a4a9be599a0300244eb92d53416b0c91` | `f4e0261c..9d63470` |
| `gwz-py` | `83e058e644cf9e719cc57accd325bd69223168f1` | `b771d98d7b33401fd555a13ad0e16b01c01610c6` | `83e058e6..b771d98` |

Every changed file in each range was read in full, together with the current
implementation of the merge module, the new backend primitives and their
callers, the drivers' gate/render paths, and the authoritative design, plan,
requirements (REQ-089 family), prior M0 review/remediation records, and public
docs. Two suspected defects were additionally reproduced with the freshly
built dev binary and native Python module against synthetic throwaway
workspaces in a scratch directory outside the repositories.

## Executive verdict

**Remediation required before M2b.**

The M1/M2a core is genuinely strong: evidence-before-mutation is enforced and
spy-tested, the atomic store is careful (create-new temp, fsync, rename,
directory sync, byte verification, recursive unknown-field carry), one shared
read-only classifier feeds status, continue, and abort, whole-operation
preflight demonstrably precedes the first mutation on every path, rollback is
reverse-ordered, checked, durable per row, and resumable, and the new backend
primitives verify exact `MERGE_HEAD`, exact parents/messages, and
expected-current refs with real-repository idempotency tests. Cross-driver
parity is pinned byte-exact through shared human and JSON fixtures.

Three findings nevertheless require remediation before (or as the opening
lead-owned checkpoint of) M2b:

1. `recovery_required` is an inescapable open state — continue and abort both
   refuse it, no transition out of it exists, and the central gate then blocks
   every workspace mutator indefinitely (P1, reproduced).
2. The Python driver's central open-merge gate resolves the workspace from the
   process cwd and ignores `meta.workspace.root`, so every gated mutator can
   bypass the gate via the client's documented `root=` parameter (P2,
   reproduced differentially).
3. A conflicted participant that a user restores with raw `git merge --abort`
   — exactly what the current published `merge.md` instructs — permanently
   blocks coordinated abort, because already-restored recognition is keyed to
   the `rolling_back` operation state (P2).

Fixing finding 1 changes the frozen operation-state transition matrix, which
M2b's finalization fault-injection work builds directly against; landing that
correction first avoids freezing more entrances into a trap state.

## Findings

### [P1-1] `recovery_required` is a terminal open state: continue and abort both refuse it, and the gate then blocks the whole workspace with no supported exit

- **Where:**
  - [model.rs:54-84](gwz-core/src/workspace_ops/merge/model.rs:54) —
    `OperationState::transition` permits no transition out of
    `RecoveryRequired` except to itself.
  - [continue_op.rs:27-32](gwz-core/src/workspace_ops/merge/continue_op.rs:27)
    — continue accepts only `Finalizing`/`Executing`/`AwaitingResolution`/
    `Halted`; `RecoveryRequired` returns the `wrong_state` error.
  - [abort.rs:103-111](gwz-core/src/workspace_ops/merge/abort.rs:103) — abort
    explicitly rejects `Completed | RecoveryRequired`.
  - [continue_op.rs:75-90](gwz-core/src/workspace_ops/merge/continue_op.rs:75)
    — the only writer: after all continue actions succeed, a fresh snapshot
    with *any* participant or operation drift persists `RecoveryRequired`.
  - [mod.rs:116-134](gwz-core/src/workspace_ops/merge/mod.rs:116) plus
    [open_merge_gate.rs:83-100](gwz-core/src/operation/open_merge_gate.rs:83)
    — the gate blocks on the presence of any record in `.gwz/merge/`,
    regardless of state.
- **Violated:** severity ladder P1 "unrecoverable … open operation";
  REQ-089G (the gate must block *until recovery*, which must exist); design
  §12/§14 "abort may be safe while continue is not" (the state gate rejects
  abort even when every participant verifies restored); review charter I
  ("current errors do not direct users into unavailable or unsafe recovery")
  — the gate error says "use merge status, merge continue, or merge abort",
  and both recovery commands then refuse.
- **Reproduction (performed, dev binary at `cf70d36`):** a workspace whose
  open record has `state: recovery_required`:
  - `gwz merge --continue` → `MergeRecoveryRequired: merge 'merge_trap' in
    state RecoveryRequired cannot be continued` (exit 1);
  - `gwz merge --abort` → `MergeRecoveryRequired: merge in state
    RecoveryRequired cannot be aborted` (exit 1);
  - `gwz commit -m x` → `OpenOperation: merge 'merge_trap' is open; this
    command is blocked until it is recovered; use merge status, merge
    continue, or merge abort` (exit 1).
  Production entry sequence: `gwz merge --continue` succeeds in all actions,
  and between an action and the post-loop snapshot an untracked file appears
  in any participant (editor swap file, build watcher output, Finder
  `.DS_Store`) → `WorktreeModified` drift → `RecoveryRequired` persisted.
  From then on the operation can never be closed by any GWZ command; the only
  exit is hand-deleting `.gwz/merge/<id>.yaml`, which is undocumented and
  exactly the kind of surgery the durable lifecycle exists to prevent.
- **Impact:** a routine, non-Git, non-destructive environmental event during a
  successful continue permanently wedges the workspace: all mutators
  (commit, push, pull, stash, tag, branch, capture, materialize, repo, …) are
  gate-blocked forever, and the advertised recovery commands refuse. No user
  work is lost, but the workspace is operationally dead until manual record
  deletion.
- **Recommended correction (one root cause: no exit from the state):**
  - allow `RecoveryRequired → RollingBack` (and arguably
    `RecoveryRequired → Executing`) in the transition matrix, gated on a fresh
    whole-operation preflight passing — i.e., abort/continue treat
    `recovery_required` as "re-classify now; proceed only if the live state
    verifies", which is precisely what their preflights already implement; and
  - stop persisting `RecoveryRequired` for post-action *observation* drift in
    continue: return the observed response with drift reported while leaving
    the durable state at `remaining_state(...)` — the next continue/abort
    preflight already rejects (retryably!) on the same drift. Reserve
    `recovery_required` for genuinely invariant-breaking records.
- **Regression test:** (1) unit: `OperationState::RecoveryRequired
  .transition(RollingBack)` succeeds; (2) service: drive a real continue to
  success, inject an untracked file before the final snapshot (test-only
  observation seam or a file created in a merged member), assert the
  operation does *not* become undischargeable: a subsequent
  `merge --abort` (after removing the file) rolls back and archives. Both
  fail before the correction.

### [P2-2] The Python driver's central gate resolves the workspace from cwd and ignores `meta.workspace.root` — every gated mutator can bypass the gate

- **Where:**
  - [dispatch/mod.rs:29-31,79-84](gwz-py/native/src/dispatch/mod.rs:29) — the
    static per-method gate runs against `current_dir()` with `workspace:
    None`, before the request is even decoded; per-op call sites do the same
    ([dispatch/branch_stash.rs:34-45](gwz-py/native/src/dispatch/branch_stash.rs:34),
    [dispatch/branch_stash.rs:62-73](gwz-py/native/src/dispatch/branch_stash.rs:62),
    [dispatch/materialize.rs:176-184](gwz-py/native/src/dispatch/materialize.rs:176),
    [dispatch/merge.rs:20-31](gwz-py/native/src/dispatch/merge.rs:20)).
  - Core handlers honor the request's explicit root:
    [handle_create_repo.rs:704-713](gwz-core/src/workspace_ops/handle_create_repo.rs:704)
    `resolve_workspace_root` prefers `meta.workspace.root`.
  - The Python client exposes exactly that:
    [client.py:227-233](gwz-py/src/gwz/client.py:227) builds
    `WorkspaceRef(root=…)` from `Client(root=…)`.
  - The Rust CLI, by contrast, passes `meta.workspace` to the gate
    ([globalargs.rs:515-519](gwz-cli/src/globalargs.rs:515),
    [globalargs.rs:746-812](gwz-cli/src/globalargs.rs:746)) — a
    driver-behavior divergence in the gate itself.
- **Violated:** REQ-089G ("A single central pre-dispatch gate MUST block
  unrelated mutating or publishing operations"); design §11 (gate enforced
  once before handler dispatch — for the *resolved* workspace); review
  charter G ("no driver or direct dispatch path bypasses the intended gate")
  and H (Rust/Python parity).
- **Reproduction (performed, differential, current native module):**
  workspace `W` containing an open `awaiting_resolution` record:
  - cwd inside `W`, `Client(root=W).commit("x")` → `OpenOperation: merge
    'merge_gate' is open; this command is blocked …` (gate fires);
  - cwd *outside* `W`, identical call → the gate does **not** fire; the
    request proceeds into the commit handler (which in the synthetic fixture
    then failed on the missing manifest with `IoError`; in a real workspace
    the commit executes).
- **Impact:** from any cwd outside the workspace, the entire blocked-command
  matrix is executable against a workspace with an open merge via the Python
  client: `capture` replaces the baseline lock (continue/abort then reject on
  `baseline_lock_changed`), `commit` in a conflicted participant can complete
  the native merge outside the lifecycle (continue/abort then reject on
  `merge_state_missing`), `push` publishes partial state, etc. Downstream
  drift detection prevents silent data loss, but the operation is left
  blocked in exactly the states the gate exists to prevent. The false-block
  converse also holds (cwd inside a gated workspace, explicit root pointing
  at a clean one).
- **Recommended correction:** thread the decoded request's
  `meta.workspace.as_ref()` into `enforce_workspace_open_merge_gate` in every
  Python dispatch site (for the method-name pre-gate, either move the gate
  after decode or re-check after decode when `meta.workspace.root` is set),
  matching the Rust CLI. While there: make
  `static_open_merge_command`'s `_ => return None` arm an explicit
  exhaustive list or a hard error — today an unclassified *new* method
  silently defaults to ungated, where the Rust CLI's exhaustive `CliRequest`
  match forces a compile-time decision.
- **Regression test:** Python-native test creating a workspace with an open
  record, invoking `commit`/`capture` with `Client(root=W)` from a cwd
  outside `W`, asserting the typed `OpenOperation` rejection (fails today).

### [P2-3] A conflicted participant already restored by a user-run `git merge --abort` permanently blocks coordinated abort — and the published docs instruct users to do exactly that

- **Where:**
  [abort.rs:246-273](gwz-core/src/workspace_ops/merge/abort.rs:246) —
  `already_restored` returns `false` unless
  `operation == OperationState::RollingBack`;
  [status.rs:250-268,304-315](gwz-core/src/workspace_ops/merge/status.rs:250)
  — a conflicted row with no native merge state gets `MergeStateMissing`
  drift and is abort-ineligible; so during `awaiting_resolution` the whole
  abort is rejected at
  [abort.rs:230-242](gwz-core/src/workspace_ops/merge/abort.rs:230).
  [merge.md:33-34](gwz-cli/docs/commands/merge.md:33) currently tells users:
  "A conflict remains in that member's ordinary Git merge state. Resolve or
  abort it with Git commands in the member repository."
- **Violated:** REQ-089I resumability intent and design §14.1's own row
  "already `aborted`/`rolled_back`: treat as an idempotent no-op after
  verifying the restored before state" — the *verifiable restored before
  state* is exactly what a user-performed native abort produces (branch at
  `before`, clean, no merge state), yet it is only recognized after GWZ
  itself entered `rolling_back`. Design M10 rejects participants that
  "cannot roll back safely"; this participant requires no rollback at all.
- **State sequence:** three-member merge → `docs` conflicted,
  `lib` merged, operation `awaiting_resolution`. User follows `merge.md` and
  runs `git merge --abort` in `docs` (restoring it bit-for-bit to the
  recorded before state). `gwz merge --abort` now rejects with `MergeDrift`
  on `docs` (`merge_state_missing`) *before* touching `lib`; continue also
  rejects (`MergeStateMissing`). The operation can never be discharged by
  GWZ; all mutators remain gate-blocked (same terminal effect as P1-1,
  reached through documented user behavior instead of a crash).
- **Impact:** the most natural Git-user reaction to a conflict — the one the
  current docs prescribe — wedges the coordinated operation permanently.
- **Recommended correction:** extend `already_restored` (or the classifier's
  abort eligibility) to accept a conflicted participant whose live state
  verifies exactly restored-before (branch attached at recorded branch, HEAD
  and target ref at `before_commit`, clean, no integration state) in the
  pre-`rolling_back` open states as a verified no-op row; `abort_merge`'s
  clean-state verification path
  ([gitbackend.rs:1145-1149](gwz-core/src/git/gitbackend.rs:1145)) already
  implements the required check. (The docs half of this hazard is finding
  P3-5.)
- **Regression test:** real-Git mixed workspace; run raw `git merge --abort`
  in the conflicted member; assert `gwz merge --abort` rolls back the merged
  member, marks the conflicted row aborted, verifies the baseline, and
  archives. Fails today with `MergeDrift` and zero rollback.

### [P3-4] Crash between a resolution/retry commit and its participant row write leaves both continue and abort blocked, with guidance that cannot be followed

- **Where:** the durable write follows the Git action:
  [continue_op.rs:44-56](gwz-core/src/workspace_ops/merge/continue_op.rs:44);
  after `commit_merge_resolution_checked` publishes the resolution commit
  (or `merge_upstream_checked` completes a retry) and before
  `store.write_open`, the row still says `conflicted`/`planned`. On restart
  the classifier reports `MergeStateMissing` + `HeadAdvanced` +
  `TargetRefChanged`; continue is ineligible and abort is ineligible
  (conflicted row without native state), so the operation is stuck; the
  drift guidance says "restore the recorded native merge state or follow
  abort recovery guidance"
  ([status.rs:252-254](gwz-core/src/workspace_ops/merge/status.rs:252)),
  which is not practically restorable. Rollback has the symmetric
  recognition (`already_restored`,
  [abort.rs:246-273](gwz-core/src/workspace_ops/merge/abort.rs:246));
  resolution has none.
- **Violated:** crash-matrix boundary 2 ("blocked with a precise typed
  recovery state" is met only nominally — the typed state's guidance is not
  executable); REQ-089H's spirit (a classified exact point should be
  recognizable).
- **State sequence:** continue commits `docs`' resolution (parents =
  recorded `before` + `MERGE_HEAD`, exact message — all verified by the
  backend), process dies before the row write. Restart: continue and abort
  both reject; the resolution work survives only as the branch head, but no
  automated path can adopt or unwind it.
- **Impact:** low-likelihood (power loss/kill in a millisecond window), but
  the outcome is another manual-surgery wedge; the operation record can no
  longer describe a state it in fact created.
- **Recommended correction:** during continue preflight, recognize the
  already-resolved shape for a `conflicted` row: live HEAD's commit has
  exactly two parents equal to the recorded `before_commit` and expected
  `MERGE_HEAD` and the exact recorded message, branch/target ref at that
  commit, clean, no native state → adopt as `continued` idempotently
  (mirror of `already_restored`).
- **Regression test:** fake-store test failing the row write after a real
  `commit_merge_resolution_checked`, then asserting a re-run continue
  completes and records `continued` without a second commit. Fails today.

### [P3-5] User-facing docs still describe the discarded M0 semantics — including conflict guidance that now wedges the workspace

- **Where:** [merge.md:26-45](gwz-cli/docs/commands/merge.md:26) — "Status,
  continue, and coordinated abort will ship together …" (accurate), but
  also: "Resolve or abort it with Git commands in the member repository"
  (now produces finding P2-3's wedge); "the workspace lock reflects clean
  member outcomes" and "the lock still records earlier outcomes" (false
  since M1 — the baseline freeze is implemented and test-asserted,
  [start.rs:1012-1020](gwz-core/src/workspace_ops/merge/start.rs:1012));
  "True merge commits use the message `Merge <source> into <target-branch>`
  without quoting or GWZ operation trailers" (false since M1 —
  [start.rs:115-127](gwz-core/src/workspace_ops/merge/start.rs:115) writes
  the quoted + trailer form, asserted at
  [g23.rs:265](gwz-core/src/workspace_ops/tests/g23.rs:265)).
  [Releases.md:22-27](gwz-cli/docs/Releases.md:22) repeats the lock-advance
  and unquoted-message claims as "current".
- **Violated:** design §4 / plan M1 ("Replace the interim M0 lock behavior
  … and update human/JSON consumer documentation together"; M1 gate:
  "First-release user and machine-output documentation describes only the
  durable baseline-lock behavior") — M1 is marked complete but this
  obligation was not honored in-tree; review charter I ("current errors do
  not direct users into unavailable or unsafe recovery" — the *docs* direct
  users into unsafe recovery).
- **Impact:** the hosted site builds from release tags, so released users
  see docs matching their binary; but anyone building `main` (the entire
  M2a→M2b audience) gets guidance that actively creates the P2-3 wedge and
  describes lock/message behavior the tree no longer has.
- **Recommended correction:** rewrite the `merge.md` "Current behavior"
  bullets and the two `Releases.md` compatibility bullets to describe the
  durable baseline-lock lifecycle, the quoted/trailer message, and honest
  interim conflict guidance that does not prescribe raw-git aborts of
  coordinated participants (this is M2b-C scope; it must not slip past the
  release gate).
- **Regression test:** the existing docs checks are generative only; add the
  M2b-C doc review to the release-gate checklist (behavioral tests already
  pin the code side).

### [P3-6] A missing recorded commit object makes status, continue, and abort fail with an opaque, memberless backend error instead of structured drift

- **Where:**
  [status.rs:95-101](gwz-core/src/workspace_ops/merge/status.rs:95) — the
  head-relation probe calls
  [gitbackend.rs:1971-1977](gwz-core/src/git/gitbackend.rs:1971)
  `is_ancestor` → `graph_descendant_of`, which errors when the recorded
  `resulting_commit`/`before_commit` object no longer exists; the `?`
  propagates out of `observe_participant`, so `handle_status` (and the
  continue/abort preflights that call `snapshot_status`) return a raw
  `GitCommandFailed` without member context, rather than a
  drift-classified row. A missing *repository* is handled structurally
  ([status.rs:87-89,358-391](gwz-core/src/workspace_ops/merge/status.rs:87));
  a missing *object* is not.
- **Violated:** review charter C ("missing repository, and missing object
  drift"); design §12 structured-drift contract.
- **Failure scenario:** while a merge is open, a participant checkout is
  re-cloned from the remote (repo exists, recorded commits absent) or an
  aggressive `git gc --prune` follows a manual reset. `gwz merge --status`
  now exits with `GitCommandFailed` naming no member; abort/continue emit
  the same opaque error. Fail-closed, but undiagnosable from the output.
- **Recommended correction:** guard the relation probe with `commit_exists`
  (already on the trait,
  [gitbackend.rs:764](gwz-core/src/git/gitbackend.rs:764)) and classify a
  missing expected object as structured drift (e.g. `head_rewound` with an
  explanatory message, or a dedicated missing-object drift kind at the next
  protocol revision), keeping status a *reporting* command.
- **Regression test:** real-repo test deleting the merged member's
  resulting commit (branch moved + pruned), asserting status returns a
  response with structured drift for that participant and that abort's
  rejection names the member.

### [P3-7] The merge event stream does not implement the design's event conventions

- **Where:** design §16 requires `OperationStarted`/`OperationFinished` per
  invocation, `MemberStarted` + `MemberFinished` with the structured outcome
  per actionable target. The merge module emits: no
  `OperationStarted`/`OperationFinished` at all (other handlers do, e.g.
  [handle_materialize.rs:153-169](gwz-core/src/workspace_ops/handle_materialize.rs:153));
  `MemberStarted` only on the abort path
  ([abort.rs:157](gwz-core/src/workspace_ops/merge/abort.rs:157)), not in
  start ([start.rs:194-217](gwz-core/src/workspace_ops/merge/start.rs:194))
  or continue
  ([continue_op.rs:44-72](gwz-core/src/workspace_ops/merge/continue_op.rs:44));
  and `member_finished` carries no outcome payload
  ([push_event.rs:473-482](gwz-core/src/operation/push_event.rs:473) emits
  `member: None, error: None`).
- **Violated:** design §16 event conventions; H-parity between operations
  (start/continue vs abort emit different member-event shapes for the same
  lifecycle).
- **Impact:** JSONL/event consumers cannot see per-member begin/end pairs or
  outcomes for start/continue; durable-order guarantees (which *are*
  honored: every transition/outcome event follows its record write,
  spy-tested at
  [start.rs:883-933](gwz-core/src/workspace_ops/merge/start.rs:883)) apply
  to an incomplete stream.
- **Recommended correction:** emit `MemberStarted` before each participant
  action in start/continue, attach the participant outcome to
  `MemberFinished`, and emit `OperationStarted`/`OperationFinished` from the
  merge dispatch (or document the deviation in the design before M2b-C locks
  driver event parity).
- **Regression test:** extend the durable-order spy test to assert the full
  §16 sequence.

### [P3-8] The gate's ancestor discovery crosses workspace boundaries, so an unrelated nested workspace is falsely gated by an enclosing workspace's open merge

- **Where:**
  [recovery.rs:17-39](gwz-core/src/workspace_ops/merge/recovery.rs:17) —
  `discover_open_before_manifest` pops to the filesystem root and stops only
  at the first *open record*; workspace resolution stops at the nearest
  manifest ([workspace/mod.rs:77-95](gwz-core/src/workspace/mod.rs:77)). With
  `meta.workspace` unset (always, for the Python driver; default for the
  Rust CLI), a command in nested workspace `O/inner` (no merge ever run, no
  open record) walks past `inner` and finds enclosing `O`'s open merge:
  handlers operate on `inner`, the gate blocks on `O`.
- **Violated:** design §11 / charter G ("new-workspace operations are not
  incorrectly captured by an enclosing workspace's open merge" — init/clone
  are `NotGated`, but every subsequent command in the established nested
  workspace *is* captured).
- **Impact:** false `OpenOperation` blocks for all mutators in the nested
  workspace until the outer merge closes. Fail-closed, unusual layout,
  confusing.
- **Recommended correction:** stop the gate walk at the first ancestor that
  is a workspace root (manifest present), i.e. resolve the same root the
  handler will use, then check only that root's `.gwz/merge`.
- **Regression test:** nested-workspace fixture with an outer open record;
  assert `commit` inside the inner workspace is not gated.

### [P3-9] Post-rollback user work in an already-`rolled_back`/`aborted` participant blocks resuming the remaining rollback

- **Where:** for recorded `Aborted`/`RolledBack` rows the classifier requires
  a fully pristine restored state (`drift.is_empty() && !is_dirty`,
  [status.rs:309-315](gwz-core/src/workspace_ops/merge/status.rs:309), with
  untracked files always counted as drift at
  [status.rs:282-287](gwz-core/src/workspace_ops/merge/status.rs:282)), and
  `already_restored` accepts only the
  `TargetRefChanged`/`HeadRewound`/`MergeStateMissing` shapes
  ([abort.rs:246-273](gwz-core/src/workspace_ops/merge/abort.rs:246)).
- **Violated:** design §14.4 ("a later `--abort` resumes the remaining
  idempotent plan") and §14.1's idempotent-no-op row: a participant whose
  rollback is already durably recorded needs *no further mutation*, so new
  user work in it cannot make the remaining rollback unsafe.
- **State sequence:** abort interrupted after `docs` and `lib` are recorded
  `rolled_back`; user edits (or an editor drops an untracked file in) `lib`;
  re-run `gwz merge --abort` → whole abort rejected on `lib`'s
  `WorktreeModified`, though only `app` still needs rollback.
- **Impact:** rollback resumability degrades to "hold everything pristine
  until the abort fully completes"; fail-closed but needlessly sticky.
- **Recommended correction:** for rows already recorded
  `Aborted`/`RolledBack`, verify only the ref position (target branch at
  `before_commit`) and skip; ignore worktree/untracked state.
- **Regression test:** fake-runtime (or real) interrupted-abort resume with
  an untracked file added to an already-rolled-back member; resume must
  complete. Fails today.

### [P3-10] The conditional stage/add gate has no test coverage

- **Where:**
  [mod.rs:136-167](gwz-core/src/workspace_ops/merge/mod.rs:136)
  `enforce_open_merge_stage_targets`, wired at
  [handle_stage.rs:69](gwz-core/src/workspace_ops/handle_stage.rs:69).
  Repository-wide grep finds no test referencing the function, its error
  text, or the add-while-merge-open scenario, in core or either driver.
- **Violated:** design §20 test-matrix rows "`add` accepts paths only in
  recorded conflicted members" / "rejects a merged or unaffected root
  target"; plan §14.4 open-operation-gate coverage. This is the single
  `Conditional` row of the gate — the one row a table test cannot cover.
- **Impact:** regressions in the only path-carrying gate exception (e.g. an
  inverted filter staging into a *merged* member, creating index drift that
  then blocks continue/abort) would land silently. Note also that an empty
  target set vacuously passes (`iter().all` on empty), which is currently
  harmless but unpinned.
- **Recommended correction / test:** real-workspace test with a mixed open
  merge asserting: staging a conflict path in the conflicted member
  succeeds; staging in the merged member and in a non-participant is
  rejected with the typed `OpenOperation` error naming the merge id; a
  root-target stage is rejected.

### [P3-11] Recorded participant paths are joined unvalidated in status and continue; only abort parses them defensively

- **Where:** abort validates
  `MemberPath::parse(&participant.path)?`
  ([abort.rs:149](gwz-core/src/workspace_ops/merge/abort.rs:149)); status
  joins the raw record value
  ([status.rs:87](gwz-core/src/workspace_ops/merge/status.rs:87)), as does
  continue via its snapshot/actions
  ([continue_op.rs:166](gwz-core/src/workspace_ops/merge/continue_op.rs:166)).
- **Violated:** defense-in-depth consistency for a file parsed from disk
  (`.gwz/merge/*.yaml` is local runtime state, but it is read back after
  arbitrary crashes and is hand-editable).
- **Impact:** a corrupted/tampered record with `path: ../../elsewhere` makes
  read-only status observe, and continue potentially *mutate*, a repository
  outside the workspace. Requires local write access to `.gwz/`, so this is
  a robustness inconsistency rather than a security boundary break.
- **Recommended correction:** apply the same `MemberPath::parse` in
  `observe_participant` and the continue action path (one helper on the
  record).
- **Regression test:** record with a traversal path → status/continue return
  the typed path-validation error rather than probing outside the root.

## Design and invariant verification (areas A–J)

**A. Durable record and recovery discovery — sound; no further findings.**
The record is written before the first mutation under the workspace mutator
lock ([start.rs:43-59](gwz-core/src/workspace_ops/merge/start.rs:43)) and the
ordering is spy-tested (record write → state event → git action → row write →
member event,
[start.rs:883-933](gwz-core/src/workspace_ops/merge/start.rs:883)). Atomic
writes stage a `create_new` temp in the destination directory, `sync_all`,
rename, sync the directory, and re-read for byte equality
([store.rs:226-262](gwz-core/src/workspace_ops/merge/store.rs:226));
`write_open` additionally verifies decoded equality and refuses when another
record exists. Unreadable, duplicate, non-regular-file, schema-mismatched, or
misnamed records fail closed as `MergeRecordUnreadable`/`MergeRecoveryRequired`
([store.rs:22-48,157-199](gwz-core/src/workspace_ops/merge/store.rs:22)) and
are never downgraded to "no open merge". Unknown fields survive
read-modify-write at every mapping level via the recursive raw-overlay
([store.rs:271-295](gwz-core/src/workspace_ops/merge/store.rs:271)), tested
including nested `publication` extras. Discovery precedes manifest parsing for
status/continue/abort
([mod.rs:260-277](gwz-core/src/workspace_ops/merge/mod.rs:260),
[recovery.rs:17-39](gwz-core/src/workspace_ops/merge/recovery.rs:17)), with a
test proving recovery with an unparsable manifest. Archive refuses open-state
records, verifies destination equality, and retention (newest 20 by mtime)
skips preservation owners and unreadable archives
([store.rs:83-125,297-322](gwz-core/src/workspace_ops/merge/store.rs:83)).
Baseline digests are exact file bytes recorded at plan time with
`root_head: None` in M2a ([plan.rs:41-47](gwz-core/src/workspace_ops/merge/plan.rs:41)).

**B. Lifecycle state machine, persistence, events, locking — sound except
P1-1 and P3-7.** Transitions are validated on every write (operation:
[model.rs:54-84](gwz-core/src/workspace_ops/merge/model.rs:54); participant:
[model.rs:86-114](gwz-core/src/workspace_ops/merge/model.rs:86)); every
outcome/state write precedes its event
([mod.rs:169-181](gwz-core/src/workspace_ops/merge/mod.rs:169)); the mutator
lock covers start, continue, and abort
([start.rs:43](gwz-core/src/workspace_ops/merge/start.rs:43),
[continue_op.rs:22](gwz-core/src/workspace_ops/merge/continue_op.rs:22),
[abort.rs:26](gwz-core/src/workspace_ops/merge/abort.rs:26)); status holds no
lock and is read-only. Wrong-id requests reject with `MergeIdMismatch` before
mutation; repeated continue after completion returns the archived response;
abort from `executing` takes the legal two-step `Halted → RollingBack` path
([abort.rs:123-140](gwz-core/src/workspace_ops/merge/abort.rs:123)). No path
can reach `completed`: full success stops at the durable `finalizing` handoff
([mod.rs:184-191](gwz-core/src/workspace_ops/merge/mod.rs:184)), verified
end-to-end from Python (`state is finalizing`, `open is True`).

**C. Status, drift, and eligibility — strong, with P3-6 and the P2-3/P3-9
eligibility conservatisms.** One classifier
([status.rs:203-338](gwz-core/src/workspace_ops/merge/status.rs:203)) serves
status, continue, and abort; it distinguishes branch changes, detached HEAD
(branch `None`), target-ref changes, advanced/rewound/diverged/unborn HEAD,
missing repositories, exact native merge state and single-line `MERGE_HEAD`,
unresolved/unstaged/staged/untracked distinctions, and — for conflicted rows —
an index-level comparison against the recomputed expected auto-merge result
([gitbackend.rs:2127-2177](gwz-core/src/git/gitbackend.rs:2127)), not
aggregate dirty counts. `planned` crash-window rows fail closed for both
continue and abort when drift exists; `up_to_date`/`unattempted` rows never
block abort. Operation-level baseline lock/manifest digests are compared
read-only on every snapshot. Continue and abort each re-run this classifier
under the mutator lock over the complete operation before any mutation, so an
optimistic status can never mutate an earlier repository: proven by
[g23.rs:675-781](gwz-core/src/workspace_ops/tests/g23.rs:675) (untracked file
in the *last* member blocks the retry of the first, which is asserted
untouched) and the abort fake proving zero calls and zero record writes on a
blocked preflight.

**D. Continue and retry — sound except P3-4.** Whole-operation preflight
precedes any action; actions run in the frozen `selected_targets` order;
resolution commits are created with exactly the recorded parents and message
and verified post-hoc
([gitbackend.rs:1799-1855](gwz-core/src/git/gitbackend.rs:1799)); retries
re-derive the analysis kind from the frozen before/source commits and verify
the resulting commit against it
([continue_op.rs:252-282](gwz-core/src/workspace_ops/merge/continue_op.rs:252));
`failed`/`unattempted` retries require the classified exact-before clean
point; new conflicts are recorded and execution continues while an unexpected
failure stops later actions, marks crash-window `planned` rows `unattempted`,
and halts durably. Already-successful rows are verified, never re-executed
(g23 asserts `lib`'s commit is bit-identical after continue). Post-action
observation refuses to adopt drift — but via the P1-1 trap state. Only a
fully successful set reaches `finalizing`, and the accepted lock provably
remains at baseline through continue
([g23.rs:565-568](gwz-core/src/workspace_ops/tests/g23.rs:565)).

**E. Coordinated abort and interrupted rollback — sound except P2-3/P3-9.**
The full rollback plan is computed and checked before the first mutation
(zero-mutation rejection test); post-merge commits/edits/branch moves/native
drift reject the whole abort naming the member, before any other participant
changes ([g23.rs:638-672](gwz-core/src/workspace_ops/tests/g23.rs:638));
rollback is reverse frozen order; conflicted rows use the checked native
abort, merged/fast-forwarded/continued rows use the expected-current checked
ref update, `up_to_date`/`failed`/`unattempted`/`planned` rows are verified
no-ops marked `aborted`; each row write is durable before the next action;
Git-rollback-then-row-write-failure is recognized (`already_restored`) and the
re-run is idempotent at both the flow level (fake fault-injection test) and
the primitive level (real-Git repeat tests); the exact baseline manifest and
lock bytes are verified before `aborted` is persisted and the record archived
([abort.rs:202-211](gwz-core/src/workspace_ops/merge/abort.rs:202)); a crash
after `aborted` but before archive is handled by the re-run's
verify-and-archive branch
([abort.rs:118-122](gwz-core/src/workspace_ops/merge/abort.rs:118)). Any
root evidence in the record fails closed as `RootMergeNotYetSupported`.

**F. Git backend safety and TOCTOU — strong; no findings beyond P3-6.**
`validate_expected_native_merge` requires an attached branch at the exact
before commit and a single exact `MERGE_HEAD` line
([gitbackend.rs:2095-2125](gwz-core/src/git/gitbackend.rs:2095)). All three
mutating primitives lock the target ref for their whole
validate-mutate-verify sequence, revalidating under the lock (raw-git
concurrency between preflight and action fails closed on the expected-current
check). `abort_merge` preserves untracked/ignored files on its forced
checkout, orders worktree → index → `cleanup_state`, and verifies restoration;
its clean-state entry makes re-runs idempotent.
`set_branch_target_checked` treats already-at-target as verified success
(`updated: false`) and rejects unexpected current OIDs without mutation.
`commit_merge_resolution_checked` verifies the created commit's two parents
and message bytes before publishing the ref, then cleans state and verifies
the postcondition. All of this is pinned by real-repository tests including
repeat-call idempotency
([g12.rs:393-485](gwz-core/src/git/tests/g12.rs:393)). Residual in-primitive
crash windows (e.g. checkout done / ref not yet set) leave fail-closed,
detectable-but-manual states — see residual risks.

**G. Central open-operation gate — correct table and Rust wiring; Python
defect P2-2, boundary defect P3-8, coverage gap P3-10.** The decision table
matches design §11 row-for-row with a complete table test
([open_merge_gate.rs:45-143](gwz-core/src/operation/open_merge_gate.rs:45));
the Rust CLI classifies via an exhaustive `CliRequest` match (compile-forced
for new commands) ahead of every handler; merge start is double-guarded (gate
+ in-handler open check under the lock); read-only rows remain available;
stage/add is restricted to conflicted member participants with root targets
rejected in M2a; recovery commands reach discovery before manifest parsing.
The blocked error names the open merge id and all three recovery commands.

**H. Rust/Python protocol, events, output parity — strong except P3-7 and the
P2-2 gate divergence.** Both drivers submit the same `MergeRequest` forms,
keep `--status`/`--continue`/`--abort` hidden from help while callable
(tested in both), render idle without fabricating an operation (pinned
byte-exact in both), and render the full lifecycle (state, merge id,
open/closed, counts including reserved ones, publication step, operation
drift, per-participant recorded/live commits, eligibility, conflicts, drift,
errors) byte-identically against one shared human fixture
([merge_status_human.txt](gwz-cli/tests/fixtures/merge_status_human.txt),
consumed by [g02.rs](gwz-cli/src/tests/g02.rs:90) and
[test_cli_merge.py](gwz-py/src/tests/test_cli_merge.py:66)) and one canonical
JSON fixture that now pins an error-bearing row with `target_kind` and
`detail`, envelope errors, drift, eligibility, and publication step (the two
M0-review follow-ups — CLI top-level error member fields and the error
sub-shape — were closed in `d325fc1`/`d379658` before this range). Typed
participant errors retain code and member context through the record
([response.rs:162-169](gwz-core/src/workspace_ops/merge/response.rs:162)).
Neither driver's human output ever advertises `gwz merge --continue`/
`--abort` (asserted); the *core* gate/open errors do name them, which is
defensible as recovery-necessary but should be revisited alongside P1-1.

**I. Release boundary and documentation accuracy — P3-5.** No release is cut
at M2a; hidden forms return typed errors for unavailable features
(`preserve`, modes, message, id-qualified status, gc, `@root`); internal
milestone names do not appear in user-facing diagnostics (checked in
renderers and error strings); `GwzMergePlan.md`'s completion claims for M1-0,
M1-A/B/C1/C2, the M1 gate, the M2a checkpoint, lanes A/B/C, and the M2a gate
are each supported by the implementation and tests reviewed here, with two
qualifications: the M1 documentation-update obligation was not met in-tree
(P3-5), and the M2a-gate bullet "an interrupted rollback resumes without
repeating unsafe mutations" is proven at the fake-flow and real-primitive
levels but not by an end-to-end real-Git interrupted abort (residual risks).

**J. Tests and regression quality — good, with the listed gaps.** Present
and convincing: real-Git mixed `up_to_date`/clean/conflict for start,
continue, and abort with exact ref/lock/manifest byte assertions; a
later-member blocker proving zero earlier mutation (both continue preflight
and abort preflight); failed/unattempted retry with real repositories;
post-merge-commit abort rejection proving the conflicted member untouched;
record-write-failure rollback recovery (fake) plus real-Git primitive
idempotency; exact resolution parents/message; record-before-mutation and
event-after-write spies; unknown-field round-trips including nested maps;
unreadable/corrupt record fail-closed at the store; shared cross-driver
fixtures; temp directories keyed by name+pid (+sequence in the store tests)
— low collision risk. Gaps: no end-to-end real interrupted-rollback resume;
no service/driver-level unreadable-record status test (store-level only); no
stage-gate tests (P3-10); no missing-object drift test (P3-6); gate matrix
is table-tested plus two representative end-to-end rows rather than one
assertion per command row; prior M0 findings were re-verified as still fixed
(member-context preflight errors, halt-lock behavior now *replaced* by the
frozen baseline as designed, unrelated-history rejection, identity
validation, narrow dead-code allows).

## Crash-matrix results

1. **After durable record creation, before the first Git action** — safe.
   All rows `planned`, state `executing`. Continue re-preflights and re-runs
   the plan; abort verifies no-mutation rows and closes idempotently
   (participant `Planned → Aborted` is legal; baseline verification passes).
2. **After a Git start/continue action, before its participant outcome
   write** — fail-closed, two legs. Start leg: the row is `planned` with
   drift (e.g. branch advanced by the completed clean merge); both continue
   and abort correctly refuse automatic action; status guidance ("restore …
   before commit … or abort") is accurate and loses nothing (the created
   merge commit is reconstructible). Continue-resolution leg: **blocked with
   impractical guidance and no idempotent adoption — finding P3-4.**
3. **After a participant write, before its event** — safe. Events are
   per-invocation, derived from durable state; the next command re-observes;
   nothing replays or is lost from the record.
4. **After transition to `executing`, before the first continue action** —
   safe. Continue accepts `executing` and re-preflights; abort takes the
   legal `Executing → Halted → RollingBack` path, both durably.
5. **After transition to `rolling_back`, before the first rollback** — safe.
   Re-run abort skips the redundant transition and executes the verified
   plan.
6. **After Git rollback succeeds, before the rollback row write** — safe and
   verified. `already_restored` recognizes the exact stale shapes
   (conflicted → `merge_state_missing`; merged/FF/continued →
   `target_ref_changed`+`head_rewound` at `before`); the re-run's primitive
   calls are individually idempotent (real-Git tested). Residual: crash
   *inside* a primitive (e.g. between restore and `cleanup_state`) can
   require one raw-git command before automation resumes — see residual
   risks.
7. **After a rollback row write, before the next rollback** — safe. Recorded
   `aborted`/`rolled_back` rows are skipped; the remainder executes in
   order. (New user work in a completed row over-blocks — finding P3-9.)
8. **After transition to `aborted`, before archive** — safe. The record
   (closed state, open directory) blocks mutators fail-closed; re-running
   abort verifies the baseline, archives, and returns the aborted response.
9. **After archive publication, before the command returns** — safe but
   scruffy. The response is lost with the process; a repeated
   `merge --abort` returns `OperationNotFound: no coordinated merge is
   open` rather than an idempotent aborted response, and
   `merge --continue <id>` on the archived aborted record returns the
   `wrong_state` error; `--status` reports idle. Typed and stable; noted as
   residual polish.
10. **After all member success and transition to `finalizing` (M2b absent)**
    — by-design open handoff. Status renders `finalizing`; continue is a
    typed no-op returning the record; abort fully unwinds
    (`Finalizing → RollingBack` legal) — the designed M2a escape hatch. The
    workspace remains gate-blocked until M2b supplies publication, which is
    the frozen handoff working as specified, and is safe *given* the P1-1 /
    P2-3 corrections so the escape hatch cannot itself become unreachable.

## Verification commands and results

Run from `/Users/owebeeone/limbo/gwz-dev` (macOS arm64, Darwin 25.5.0) at the
exact reviewed heads. Working trees were byte-identically clean before and
after the suite (pre/post `git status --porcelain` captured and diffed: no
tracked or untracked changes in any repository).

| Command | Result |
| --- | --- |
| `cargo fmt --all -- --check` | pass (exit 0) |
| `cargo test --workspace --all-targets --no-fail-fast` | pass — **597 passed, 0 failed, 1 ignored** across 14 test binaries (gwz-core lib 425; cli unit 72; integration suites 100) |
| `cargo clippy --workspace --all-targets --all-features -- -D warnings` | pass (exit 0) |
| `cargo fmt --manifest-path gwz-py/Cargo.toml -- --check` | pass (exit 0) |
| `cargo clippy --manifest-path gwz-py/Cargo.toml --all-targets --all-features -- -D warnings` | pass (exit 0) |
| `gwz-py/.venv/bin/python -m maturin develop --manifest-path gwz-py/Cargo.toml` | pass — editable wheel built and installed |
| `gwz-py: scripts/regen_protocol.py --check` | pass — `regen_protocol: OK` |
| `gwz-py: .venv/bin/python -m pytest -q` | pass — **265 passed** |
| `gwz-cli: python3 scripts/generate_cli_reference.py --check` | pass (exit 0) |
| `uvx --from 'mkdocs-material>=9.5,<10' mkdocs build --strict --site-dir /tmp/gwz-m2a-review-F5-docs` | pass — built in 0.38s |
| `bazel build //gwz-core:gwz_core //gwz-cli:gwz` | pass — 2 targets, build successful |
| `git diff --check` (root, gwz-core, gwz-cli, gwz-py) | all clean (exit 0) |

Additional reproductions (all artifacts confined to the session scratch
directory outside the repositories):

- **P1-1:** synthetic workspace with a `recovery_required` open record; the
  dev binary (`target/debug/gwz` at `cf70d36`) rejects both `--continue` and
  `--abort` with `MergeRecoveryRequired`, renders status, and gate-blocks
  `commit` with the `OpenOperation` message naming the refusing commands.
- **P2-2:** synthetic workspace with an `awaiting_resolution` open record;
  `Client(root=W).commit(...)` through the freshly built native module is
  gate-blocked (`OpenOperation`) when cwd is inside `W` and **not**
  gate-blocked when cwd is outside `W` (the request reached the commit
  handler, failing only on the fixture's missing manifest).

## Residual risks and missing coverage

- **Platform:** everything verified on macOS arm64 only; ref-transaction,
  rename/fsync, and path-case behavior on Linux/Windows unexercised. The
  store's byte-exact fixtures and `MERGE_HEAD` file reads are
  platform-sensitive by design; no platform-conditional code was observed.
- **In-primitive crash sub-windows:** a crash inside `abort_merge` between
  index restore and `cleanup_state`, or inside `set_branch_target_checked`
  between checkout and the ref transaction commit, leaves a fail-closed
  state whose recovery needs one raw-git command (`git merge --abort` /
  `git reset --hard <recorded>`), after which the automated resume works.
  Millisecond windows; detectable via status; acceptable but worth a
  documented recovery note at M2b.
- **GWZ-vs-GWZ gate race:** the gate check runs before the handler acquires
  the mutator lock, so a mutator whose gate check passed can complete after
  a concurrent merge start opens a record. All such interleavings surface
  later as classified drift/baseline rejection rather than silent damage;
  closing it would require re-checking under the lock.
- **No end-to-end real-Git interrupted-rollback test:** the flow is proven
  with a fake runtime plus real-Git primitive idempotency tests; a single
  real interrupted-and-resumed abort (kill between rows) would close the
  composition gap the M2a gate bullet claims.
- **No service-level unreadable-record test:** store-level corruption tests
  exist; nothing pins that `merge --status` (and both drivers) surface
  `MergeRecordUnreadable` rather than idle for a corrupt open record.
- **Crash-boundary 9 polish:** re-running abort after a post-archive crash
  yields `OperationNotFound` rather than an idempotent aborted response.
- **Leftover atomic-write temporaries:** crashed writes leave
  `*.tmp` files in `.gwz/merge/` that are ignored by discovery but never
  cleaned; unbounded only in pathological crash loops.
- **`carry_unknown` sequence pairing:** unknown fields inside sequences are
  re-attached positionally; if a future writer stores unknown per-item data
  in reordered lists, an older binary's rewrite could mis-pair them. No
  M2a-visible effect.
- **`aborted`-state record in the open directory** (crash boundary 8) blocks
  all mutators with "merge … is open" until abort is re-run; the message's
  "open" wording is slightly misleading for a closed-state record.
- **Python event surface:** the py operation recorder wraps the core event
  stream; once P3-7 lands, driver event parity should be re-pinned.
- **Empty stage-target sets** vacuously pass the stage gate (currently
  harmless; unpinned behavior).

## Recommended next action

**Remediate before opening the M2b wave; M2b lanes not touching the state
machine may start in parallel.** Concretely:

1. Land a lead-owned interface correction (the change-control path in plan
   §17, since it alters frozen transition semantics) covering **P1-1**:
   `RecoveryRequired` exits guarded by fresh whole-operation preflight, and
   continue no longer persisting the trap for post-action observation
   drift. Doing this *before* M2b-A2 matters because the finalization
   state machine will otherwise wire additional failure paths into an
   inescapable state and freeze fault-injection tests against it.
2. Fix **P2-2** (Python gate uses `meta.workspace.root`; default-open
   method classification made exhaustive) — small, driver-local, no frozen
   contracts.
3. Fix **P2-3** (recognize a verifiably restored conflicted participant
   outside `rolling_back`) together with the **P3-5** documentation rewrite
   so the published conflict guidance stops manufacturing that state; at
   minimum both must be complete before the M2b release gate.

The P3 items (idempotent already-resolved adoption, event conventions,
missing-object drift, nested-workspace gate boundary, post-rollback
over-blocking, stage-gate tests, path validation) are well-bounded
follow-ups suitable for the M2b wave; none blocks starting M2b-A1 (marker
model) or M2b-B (scoped root commit primitive) immediately.

Everything else reviewed — the durable store, the shared classifier, the
whole-operation preflights, the checked backend primitives, rollback
resumability, machine-output parity, and the M1/M2a milestone claims in
`GwzMergePlan.md` — holds up under adversarial reading and real-repository
verification, and the M2b handoff seam (`finalizing`, publication progress,
archive discipline) is genuinely ready to build on once the state-machine
correction lands.
