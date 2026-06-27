# GWZ stash and branch implementation plan

Status: implementation documentation revision checkpoint, 2026-06-25.

This plan was reviewed against:

- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/dev-docs/GwzStashSpec.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/dev-docs/GwzStashPlan.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzBranchPlan.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GWZDesign.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GWZRequirements.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/Reference.md`
- current source under `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git`,
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops`,
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol`,
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol`, and
  `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src`.

No source implementation is part of this document. `GWZDesign.md` remains
authoritative and `GWZRequirements.md` remains the baseline. Wave 0 scope and
layout promotion has landed in the authoritative docs. Branch list/create/delete,
branch switch materialization, branch-sourced snapshots, and coordinated stash
push/list/apply/pop/drop are now implemented. Branch merge B5a/B5b has landed:
the protocol, core handler, CLI parser, renderer, docs, and focused tests now
cover current-attached-branch merge and conflict reporting.

## Review response summary

This revision responds to
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzStashBranch-Review48.md`,
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzStashBranch-Review55.md`, and
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzStashBranch-Review48-2.md`.

Accepted changes:

- Add a Wave 0 authoritative-doc scope gate before implementation.
- Keep stash registry metadata local-only under `.gwz/`, but make that an
  explicit design decision, switch bundle files to `.yaml`, add root exclude
  requirements, and document non-portability.
- Treat the file-backed workspace mutator lock as new cross-process
  infrastructure, shared by stash and branch mutations.
- Add push-lifecycle state distinct from restore state for stash crash recovery.
- Clarify stash member state tuples across participation, push lifecycle, and
  restore state.
- Split branch protocol work into scheduled B4a/B5a protocol slices before
  handler/CLI work.
- Keep merge-only branch result values owned by the B5a merge protocol slice.
- Restrict first branch merge to the current attached branch and defer
  `--into`.
- Remove branch delete from the clean-worktree requirement and defer delete
  `--force`.
- Make documentation revision a first-class pass across dev docs, core docs, and
  CLI docs.

## Current implementation state

Already implemented:

- Taut-generated protocol exists in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`,
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs`, and
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/corpus/`.
- `GitBackend` in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/gitbackend.rs` has status,
  head, ref read, checkout, merge, rebase, reset, stage, commit, tag, fetch, and
  push primitives, plus branch list/create/delete/switch and stash
  push/list/apply/pop/drop primitives. `checkout_branch` remains the
  materialize-restore primitive that can create/check out a branch at a
  specified commit; command branch switching uses create-free `switch_branch`.
- `handle_materialize` in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_materialize.rs`
  rewrites `gwz.conf/gwz.lock.yml` from observed post-mutation state and syncs
  the workspace boundary. It supports lock, snapshot, tag, exact commit, and
  branch switch targets.
- `handle_snapshot` in the same file snapshots observed selected member state
  and supports branch-sourced snapshots.
- `handle_branch` in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_branch.rs`
  implements branch list/create/delete/merge for selected materialized members.
  Create-and-switch rewrites the lock from observed post-switch state. Merge is
  current-attached-branch only for v0; clean merges rewrite the lock from
  observed state, and conflicts return `conflicted` with per-member conflict
  paths while leaving the native Git merge state for the user to resolve.
- `handle_stash` in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stash.rs`
  implements coordinated stash push/list/apply/pop/drop for selected Git
  members using local bundle metadata under `.gwz/stash/bundles/`.
- `.gwz/` is written by core as local runtime state. The stash registry lives
  under `.gwz/stash/bundles/`, the workspace mutator lock lives under
  `.gwz/locks/workspace-mutator.lock`, and `.gwz/` is excluded from root
  repository status by the workspace boundary.
- `handle_tag` in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_tag.rs`
  shows the Git-native ref fan-out pattern and includes the root for local tag
  operations. That root behavior must not be copied into branch switch or stash
  without the explicit root rules below.
- `handle_stage`, `handle_commit`, and `handle_ls` in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/` show the current
  thin operation-handler style, path routing, root-last commit rule, and
  manifest/lock-only listing pattern.
- Status branch summaries exist in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/status/status_member.rs`,
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/status/branch_groups_and_differences.rs`,
  and CLI rendering in
  `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/append_branch_summary.rs`.
- CLI parsing and dispatch are thin wrappers around core requests in
  `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/globalargs.rs`,
  `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs`, and
  `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/main.rs`.
Not implemented:

- Root participation for branch and stash operations.
- Remote branch tracking, branch push/delete, branch rename, stash repair/adopt,
  merge continue/abort helpers, and `gwz branch --merge --into <target>`.

## Resolved stash decisions

- **Root repo participation:** v0 stash applies only to selected workspace
  members. The workspace root is excluded until a separate root stash policy is
  designed around `gwz.conf/`, root first-commit state, and root metadata
  changes. This matches the current stash spec direction.
- **Registry storage:** stash bundle metadata is intentionally local runtime
  state, not versioned workspace intent. Store it under
  `.gwz/stash/bundles/<stash_id>.yaml`, not `gwz.conf/`. The tradeoff is
  explicit: coordinated stash metadata is not portable across clones, which
  matches native Git stash locality. If `.gwz/` is wiped in one clone, bundle
  grouping is lost but per-member native GWZ stash entries remain recoverable as
  orphans surfaced by `gwz stash list`. Wave 0 must update the design docs to
  reconcile the stale directory layout text and document `.gwz/` as local-only.
- **Root exclude boundary:** because `.gwz/` is local runtime state, S1 must add
  `/.gwz/` to the root repository's managed `.git/info/exclude` boundary or an
  equivalent runtime-dir boundary. A stash operation must not make the workspace
  root appear dirty merely by writing bundle metadata.
- **Restore identity:** persist native stash object id as the primary identity.
  Persist `stash@{n}` only as display text. Native stash indices move after each
  stash mutation, so every apply, pop, and drop must re-resolve the current index
  from `git stash list` by object id first and `gwz:<stash_id>:` message prefix
  second.
- **Preserve index:** default restore should attempt to preserve staged state
  with `--index`/`REINSTATE_INDEX`. If index reinstatement conflicts or cannot be
  represented by the backend, return a typed stash conflict. Add an explicit
  later option only if users need non-index restore as the default.
- **Partial restore metadata:** bundle metadata must have per-member
  `restore_state`: `pending`, `applied`, `popped`, `dropped`, `noop`, `missing`.
  Explicit per-member apply/pop/drop is allowed only with an explicit member
  selection and must update only those member records.
- **Push lifecycle metadata:** restore state is not enough to represent a
  half-written push. Each selected member record also needs a push lifecycle:
  `unattempted`, `saving`, `saved`, `empty`, `failed`. Failed records carry a
  typed error code/message. This is separate from restore state and is required
  for crash recovery and reconciliation.
- **Stash member state tuples:** `participation` is the pre-attempt membership
  classification, `push_lifecycle` is the native stash attempt outcome, and
  `restore_state` is the current restore/drop status. Pin these tuples:
  clean/noop is `participation=empty`, `push_lifecycle=empty`,
  `restore_state=noop`; saved dirty state is `participation=stashed`,
  `push_lifecycle=saved`, `restore_state=pending`; failed, saving, or
  unattempted dirty state is `participation=stashed`,
  `push_lifecycle=failed|saving|unattempted`, `restore_state=missing` until
  reconciliation or repair makes it actionable; post-restore states keep
  `push_lifecycle=saved` and change `restore_state` to `applied`, `popped`,
  `dropped`, or `missing`.
- **Reconciliation:** list and restore preflight scan both directions:
  registry entry with missing native stash payload, and native GWZ-prefixed stash
  payload with no registry bundle. Orphans are surfaced with manual remediation
  guidance; adoption is an explicit later repair command, not automatic v0
  behavior.
- **Operation locking:** stash mutations are sequential inside a workspace
  operation and guarded by a new file-backed cross-process workspace mutator
  lock before the first native stash mutation. Per-member in-process locks alone
  are not sufficient because stash index order and registry writes are
  bundle-wide. The lock is workspace-wide across stash and branch mutations, so
  a branch operation and a stash operation in separate terminals are serialized.
- **Native stash conflict behavior:** git2 stash apply/pop conflicts may return a
  conflict without porcelain-style conflict markers. v0 treats this as an
  unchanged-worktree `stash_conflict`, leaves the member `pending`, and reports
  guidance rather than claiming a partially applied worktree.

## Stash phases

### S1 - Registry model and local runtime I/O

Touchpoints:

- Add `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/stash/`.
- Export it from `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/lib.rs`.
- Use atomic write helpers/patterns from
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/artifact/mod.rs`.
- Add a file-backed cross-process workspace mutator lock in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/operation/`, with lock files
  under `.gwz/locks/`.
- Extend the root boundary handling in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/sync_workspace_boundary.rs`
  or add an equivalent runtime boundary helper so `.gwz/` stays out of root
  status.

Work:

- Define `StashBundle`, `StashBundleMember`, participation, restore state,
  push lifecycle state, dirty summary, and drift/warning structs.
- Store bundle files under `.gwz/stash/bundles/<stash_id>.yaml`, not under
  `gwz.conf/`.
- Add dedicated stash registry list helpers; do not reuse artifact listing
  helpers that only scan `.yaml` by path order unless they are generalized for
  created-at descending order.
- Validate schema `gwz.stash-bundle/v0`, workspace id, stash id, member ids,
  member paths, enum values, object ids when present, and absence of skipped
  members.
- Record `branch_before`, `head_before`, `include_untracked`, `include_ignored`,
  message suffix, full stash message, and dirty summary before push.
- Define lock acquisition/release semantics with OS-level exclusive locks. The
  lock file may remain after process exit, but an unlocked file is not stale; a
  held lock by a dead process must be released by the OS. If the selected locking
  crate cannot provide that guarantee on all targets, or if the workspace root is
  on NFS or another network filesystem with unreliable advisory locking,
  document the stale-lock/network-filesystem policy in this phase.

Tests:

- Round trip pinned `(participation, push_lifecycle, restore_state)` tuples for
  clean/noop, saved dirty, failed dirty, unattempted dirty, and post-restore
  states.
- Round trip push lifecycle states including `unattempted` and `failed` with
  error detail.
- Reject unsupported schema, invalid ids, invalid member paths, duplicate member
  records, and persisted skipped members.
- Atomic write replaces an existing bundle cleanly.
- List returns newest first by `created_at`.
- Two separate OS processes cannot both acquire the workspace mutator lock.
- Writing a stash bundle under `.gwz/` does not make the root repository report
  `.gwz/` as untracked.

### S2 - GitBackend stash primitives

Touchpoints:

- Extend `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/gitbackend.rs`.
- Add focused tests under `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/tests/`.

Work:

- Add `stash_push`, `stash_list`, `stash_apply`, `stash_pop`, and `stash_drop`.
- Model options for tracked-only, include-untracked, include-ignored, and
  preserve-index.
- For git2, account for `stash_save`, `stash_foreach`, and index-only
  apply/pop/drop APIs. Resolve the numeric stash index immediately before each
  mutation by object id first, then GWZ message prefix.
- Document conflict behavior for git2 stash apply/pop: conflict returns a typed
  `stash_conflict`, keeps the native stash pending, and must not claim porcelain
  conflict markers were left behind unless a future backend proves that behavior.
- Map native conflicts and missing stashes to stash-specific model errors once
  protocol error codes exist.

Tests:

- Push tracked-only changes and leave the worktree clean.
- Push with include-untracked and include-ignored.
- List finds GWZ-prefixed stash entries and records object ids.
- Apply restores while keeping the stash entry.
- Pop restores and removes only the matching GWZ stash.
- A conflict returns `stash_conflict` and leaves the worktree unchanged/pending
  for the member.
- Drop removes only the matching GWZ stash.
- Older bundle restore still works after newer stash entries change indices.
- Non-GWZ stashes are never touched.

### S3 - Taut protocol and error shape

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/regen.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/convert.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/corpus/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/tests/protocol.rs`

Work:

- Add `ActionKind.stash`, `StashOp`, stash request/response messages, bundle
  projection messages, member participation/status records, push lifecycle
  records, restore state records, and drift records.
- Add error codes after existing values, preserving wire compatibility:
  `stash_not_found`, `stash_incomplete`, and `stash_conflict`.
- Do not add shadow Rust protocol types outside generated taut payloads. Core
  internal model structs may exist, but wire shapes must be taut-defined.

Tests:

- `python protocol/regen.py --check` fails before regeneration and passes after.
- CBOR round trip covers stash push, list, apply, pop, and drop.
- Error code wire values are pinned.

### S4 - Core stash push orchestration

Touchpoints:

- Add `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stash.rs`.
- Export from `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/mod.rs`.
- Reuse selection helpers from
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/push_member.rs` and
  locked selection behavior from
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_materialize.rs`.

Work:

- Resolve active selected members. Require selected members to be materialized.
- Generate one `stash_id` per push.
- Preflight all selected member repositories before mutation.
- Write a pending bundle intent before the first native stash mutation.
- Sequentially process members: record clean members with the pinned
  `empty/empty/noop` tuple; stash dirty members with
  `gwz:<stash_id>: <message or "gwz stash">`; update metadata to the pinned
  `stashed/saved/pending` tuple after each successful native mutation.
- If native mutation fails mid-operation, preserve recoverable bundle metadata
  using push lifecycle states (`saved`, `failed`, `unattempted`) and return a
  typed partial result.

Tests:

- Mixed clean/dirty selection creates one bundle.
- Default push ignores untracked files.
- `-u` includes untracked files and `-a` includes ignored files.
- Native stashes share the same message prefix.
- Mid-operation failure leaves recoverable metadata.
- A failed stash attempt records `push_lifecycle: failed` and error detail
  without being confused with clean `empty` participation.
- A process-crash simulation leaves an orphan native GWZ stash detectable by
  reconciliation.

### S5 - Stash list and reconciliation

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/stash/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stash.rs`
- CLI rendering in `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/append_branch_summary.rs`
  or a new listing renderer module wired through
  `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/main.rs`.

Work:

- Load registry bundles first.
- Scan selected materialized members' native stash lists.
- Mark missing payloads and orphan native GWZ stashes.
- Orphan output includes manual remediation instructions until an explicit
  repair/adopt command is added.
- Return combined bundles by default and expanded per-member detail when
  requested.

Tests:

- Combined list sorts newest first.
- Combined output shows total, stashed, clean/noop, and partial counts.
- Expanded output shows per-member ref/message or clean/noop state.
- JSON exposes full taut-defined bundle metadata.
- Drift in both directions is surfaced.

### S6 - Apply, pop, and drop

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/stash/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stash.rs`

Work:

- Resolve newest eligible bundle when no id is supplied.
- Explicit member selection is required for partial apply/pop/drop. Without
  explicit selection, operate on the whole bundle.
- Preflight all targeted pending/applied stashed members for native stash
  presence and clean destination worktree.
- Sequentially apply/pop/drop. Re-resolve the stash index immediately before
  each native mutation.
- `apply` marks members `applied` and keeps native stash payloads.
- `pop` marks members `popped` and removes native stash payloads.
- `drop` marks members `dropped` and removes native stash payloads.
- Delete the bundle file only when all members are terminal:
  `popped`, `dropped`, or `noop`, and no member has push lifecycle
  `unattempted`, `saving`, or `failed`. `missing` is not terminal for automatic
  deletion in v0; users must run explicit drop/repair cleanup once that policy is
  implemented.
- Warn on `branch_before`/`head_before` drift; do not reject drift in v0 unless
  the destination is dirty or native apply reports conflict.

Tests:

- Bundle-wide pop restores all pending members and deletes the complete bundle.
- Bundle-wide apply restores all pending members and keeps native stashes.
- Explicit per-member pop leaves the remaining members pending and visible.
- Missing native stash returns `stash_incomplete`.
- Dirty destination rejects before mutation.
- Clean destination conflict stops immediately, records prior successes, leaves
  the failing and later members pending, returns `stash_conflict`, and verifies
  the failing member's worktree was not silently modified by git2.

### S7 - CLI, help, and release checks

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/globalargs.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/main.rs`
- New help modules under `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/tests/local_workflows.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/tests/`

Work:

- Add `gwz stash push [-u|-a] [-m <message>]`, `gwz stash list
  [--expanded]`, `gwz stash apply [stash-id]`, `gwz stash pop [stash-id]`,
  and `gwz stash drop <stash-id>`.
- CLI parses flags and renders human/JSON output only. It must not inspect
  `.gwz/stash/` or call Git directly.
- Reject invalid flag combinations in parser tests: `-u` with `-a`, partial
  restore selection without explicit stash id unless newest partial semantics are
  later specified, and JSON/combined option conflicts.

Verification:

- `cargo test --test rename` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- `cargo test --test rename` in `/Users/owebeeone/limbo/gwz-dev/gwz-cli`
- `cargo test --test protocol` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- `python protocol/regen.py --check` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- Focused core and CLI workflow tests for each phase.

## Resolved branch decisions

- **Root repo participation:** member branch operations are default. Root
  participation is opt-in and delayed until root reload semantics are
  implemented. Root list/create/delete can be added before root switch/merge, but
  the first implementation should keep root out of mutation paths unless the
  root reload rule is in the same phase.
- **Switch/materialize semantics:** `gwz materialize --switch <branch>` is the
  checkout/switch surface. It switches selected materialized members to an
  existing local branch, then observes each repo and rewrites
  `gwz.conf/gwz.lock.yml` from the observed state. It does not create branches,
  fetch, detach, or move branch refs.
- **Snapshot-from-branch semantics:** `gwz snapshot <id> --branch` captures the
  HEAD of the current attached branch for all selected members and rejects mixed
  branch names by default. `gwz snapshot <id> --branch <name>` resolves
  `refs/heads/<name>` in every selected member without switching worktrees. Both
  write normal snapshot artifacts; snapshots do not follow moving branches.
- **Snapshot error taxonomy:** current-branch snapshots reject detached HEAD,
  unborn HEAD, and mixed attached branch names as distinct typed errors. A
  single-member selection trivially satisfies the shared-branch rule if it is
  attached and born.
- **Dirty worktree policy:** switch, create-and-switch, and merge require clean
  selected member worktrees by default. Non-current branch delete does not touch
  the worktree and does not require cleanliness; delete only rejects when the
  target branch is currently checked out or missing.
- **Merge conflicts:** first merge support is current-attached-branch only.
  `--into <target>` is deferred until switch-before-merge is implemented and
  tested. Conflicts are expected Git outcomes. A conflicted member remains in
  normal Git merge state and is reported as `conflicted` with conflict paths.
  Successful prior members are not rolled back. Cleanly merged members are
  re-observed and written to the lock. Conflicted members are captured after the
  developer resolves and runs `gwz capture` or `gwz commit`.
- **Branch protocol shape:** `MaterializeTargetKind.branch` covers switch.
  `SnapshotRequest` gets a taut-defined `SnapshotSource`. Git-native branch
  create/list/delete/merge use a separate `BranchRequest`/`BranchResponse`.
  Branch merge response shape must include per-repo conflict paths and explicit
  aggregate rules before B5 handler work starts. Root results use either an
  explicit root repo summary or a synthetic repo id; do not force root through
  member selection.
- **Branch mutation locking and atomicity:** branch switch/create/delete/merge
  share the file-backed workspace mutator lock with stash. Create attempts
  rollback for branches it created earlier if a later member fails. Delete is
  best-effort after full preflight and reports partial honestly if a postflight
  failure occurs; it must not claim transactional all-or-nothing.
- **Existing checkout primitive:** `checkout_branch` remains the source of truth
  for "branch exists at target or can be created at target" materialize restore.
  `switch_branch` must be create-free, and `branch_create` must factor shared ref
  resolution/divergence checks rather than duplicate them.

## Branch phases

### B1 - Protocol for switch and branch-sourced snapshots

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/corpus/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/convert.rs`

Work:

- Add `MaterializeTargetKind.branch`.
- Add `SnapshotSourceKind` and `SnapshotSource { kind, branch? }`, then add
  `source` to `SnapshotRequest`.
- Add typed branch snapshot errors after existing error codes:
  `branch_detached_head`, `branch_unborn_head`, and `branch_mixed`.
- Add planned action/status values only when needed by wire responses, preserving
  existing numeric values.

Tests:

- Protocol generation and corpus round trips for branch materialize target and
  snapshot sources.
- Existing protocol tests remain green.

### B2 - Backend branch primitives

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/gitbackend.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/tests/`

Work:

- Add `branch_list`, `branch_create`, `branch_delete`, and `switch_branch`.
- Factor shared ref existence/divergence checks from existing
  `checkout_branch`; do not regress materialize branch restore behavior.
- `switch_branch` checks out an existing branch without moving it and verifies
  HEAD is attached to that branch.
- `branch_create` resolves the start ref per repo. Existing branch at the same
  commit is noop; existing branch at a different commit is rejected unless a
  future destructive policy explicitly allows moving it.
- `branch_delete` refuses to delete the current branch. `--force` is deferred
  from CLI/protocol until exact porcelain-equivalent semantics are specified.

Tests:

- Switch existing branch and verify HEAD attachment.
- Missing branch rejects without mutation.
- Dirty worktree causes safe checkout rejection.
- Create from HEAD and named ref.
- Existing same-commit branch is noop.
- Existing different-commit branch rejects.
- Delete non-current branch and reject current branch.
- Existing materialize branch-restore tests remain green after refactoring
  `checkout_branch` support code.

### B3 - `materialize --switch` and `snapshot --branch`

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_materialize.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_snapshot`
  behavior inside `handle_materialize.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/materialize_preflight.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/mod.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/globalargs.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/materialize_long.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/materialize_after.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/snapshot_long.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/snapshot_after.rs`

Work:

- Build branch target states by resolving `refs/heads/<branch>` in every
  selected materialized member.
- Preflight: materialized repo, branch exists, worktree clean, selection in lock.
- Switch with `GitBackend::switch_branch`.
- Re-observe with existing `backend.head`/`backend.status`, update lock, and run
  `sync_workspace_boundary`.
- For `snapshot --branch`, resolve current attached branch or named branch heads
  without switching and write a normal snapshot.
- Add a named-branch snapshot builder that uses
  `backend.read_ref("refs/heads/<name>")` and writes `branch=<name>`,
  `detached=false`, and `dirty=false` based on the ref, not the live worktree.
- For current-branch snapshots, preflight all selected members and reject
  detached, unborn, or mixed attached branches with the typed B1 errors.

Tests:

- Switch rewrites the lock from observed branch/commit.
- Switch does not create missing branches.
- Switch rejects dirty members before any mutation.
- Snapshot current branch rejects detached HEAD and mixed branch names.
- Snapshot current branch rejects unborn HEAD distinctly from detached HEAD.
- Snapshot named branch does not change worktrees.
- Snapshot named branch ignores unrelated worktree dirtiness and records the ref
  commit.
- Existing materialize lock/snapshot/tag tests remain green.

### B4a - Branch command protocol

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/corpus/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/tests/protocol.rs`

Work:

- Add `ActionKind.branch`, `BranchOp`, `BranchRequest`, `BranchResponse`, and a
  taut-defined branch repo summary.
- Include branch action result values (`created`, `exists`, `deleted`,
  `switched`, `noop`, `skipped`) in generated payloads, not shadow CLI/core
  types. Merge-only result values are owned by B5a.
- Omit delete `force` until semantics are specified.

Tests:

- Protocol corpus and CBOR round trips cover list/create/delete requests and
  responses.
- `python protocol/regen.py --check` and `cargo test --test protocol` pass after
  regeneration.

### B4b - Branch list/create/delete command

Touchpoints:

- Add `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_branch.rs`.
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/mod.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/globalargs.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/main.rs`
- New CLI help modules under `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/`.

Work:

- Implement `gwz branch`, `gwz branch --list`, `gwz branch --create <name>
  [--from <ref>] [--switch]`, and `gwz branch --delete <name>`.
- List is read-only and can reuse status branch label/grouping ideas.
- Create/delete preflight every selected materialized member before mutating.
- Create uses rollback for branches already created by this operation if a later
  selected member fails unexpectedly.
- Delete preflights all selected members but reports post-preflight failures as
  partial rather than claiming rollback.
- Create-and-switch composes create preflight with the B3 switch semantics.

Tests:

- Parser builds taut requests and rejects invalid flag combinations.
- List aggregates branch names and current branch labels.
- Create rejects missing start ref and different existing branch before mutation.
- Create rollback removes earlier branches when a later create fails.
- Delete rejects before mutation when any selected repo has the branch checked
  out; post-preflight delete failure is reported partial.
- Delete a non-current branch does not require a clean worktree.
- Create-and-switch leaves lock matching observed post-switch state.

### B5a - Branch merge protocol

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/corpus/`

Work:

- Add merge result fields to the branch response shape before handler work:
  source ref, target branch, resulting commit, conflict paths, and per-repo
  action result.
- Append merge-only branch action result values `merged` and `conflicted` here,
  not in B4a, so each protocol slice owns the vocabulary it uses.
- Define aggregate rules: any conflict yields aggregate `conflicted` when no
  member failed unexpectedly; clean members plus conflicted members are still
  `conflicted`, not `partial`; unexpected post-preflight failures with prior
  mutations are `partial`.

Tests:

- Protocol round trips include clean merge and conflicted merge branch
  responses.
- Error/result wire values remain append-only, and B4a corpus fixtures do not
  require merge-only values before B5a lands.

### B5b - Branch merge handler and CLI

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_branch.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/gitbackend.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/append_branch_summary.rs` or a
  branch-specific renderer.

Work:

- Implement `gwz branch --merge <source>` into the current attached target
  branch across selected members.
- Defer `--into <target>` until a later phase. The first handler must assert the
  supplied or inferred target branch equals each member's current attached HEAD
  before calling `merge_upstream`.
- Preflight: selected repos materialized and clean, source ref resolves, target
  branch attached and current, no in-progress merge/rebase state is present.
- Use existing `merge_upstream` initially. Its `branch` argument must be the
  current branch. Report clean merges as `ok` and conflicts as `conflicted` with
  conflict paths.
- Rewrite lock from observed state for cleanly merged members. Do not claim lock
  matches for conflicted members.
- Status or branch preflight must surface/block an existing `MERGE_HEAD` before
  re-running branch merge on that member.

Tests:

- Fast-forward merge updates lock.
- True merge commit updates lock.
- Conflict leaves repo in merge state and reports conflict paths.
- A conflict after earlier clean merges returns aggregate `conflicted` and does
  not roll back earlier members.
- Existing in-progress merge state blocks a new branch merge.
- Dirty member rejects before any mutation.

### B6 - Root participation and remote polish

Touchpoints:

- Root-aware branch handling in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_branch.rs`.
- Workspace reload through existing artifact readers in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/artifact/mod.rs`.

Work:

- Add explicit `--root` or `--all-repos` only after member behavior is stable.
- Root switch rule: preflight current root and selected members, switch root
  first, reload `gwz.conf/gwz.yml` and `gwz.conf/gwz.lock.yml`, re-resolve
  selected members against the new workspace, then switch/materialize members.
- Before switching root, inspect the target root branch's `gwz.conf/` files
  without mutating when possible. If the target manifest drops, renames, or
  repaths selected members, reject before root mutation. If the backend cannot
  inspect safely, record prior root branch/head and define rollback behavior.
- Remote branch tracking, branch push/delete, worktrees, rename, and destructive
  branch moves remain out of scope.

Tests:

- Root switch reloads the manifest and lock before member mutation.
- Root switch rejects dirty root before mutation.
- Root switch rejects target manifests that cannot resolve the selected members
  before mutating root, or rolls root back if a post-switch reload fails.
- Root list/create/delete behavior is explicit and never implied by member
  selection.

## Sequencing recommendation

Start with Wave 0 documentation/scope promotion. After that, run B1, S1, and
the branch/stash backend sub-slices in parallel with strict shared-file ordering.

Rationale:

- B1-B3 unlock a narrow branch workflow (`materialize --switch` and
  `snapshot --branch`) by reusing observed-state lock rewrite and boundary sync,
  while adding new target resolution and preflight logic.
- S1 is independent of branch protocol and most branch handler work. It reduces
  stash risk early by pinning the registry shape, atomic write behavior,
  `.gwz/` boundary behavior, and cross-process operation locking.
- Do not start S4-S6 until S2-S3 are complete. Stash correctness depends on
  native stash object identity, index re-resolution, and taut-defined payloads.
- Do not start B5b until B3, B4b, and B5a are stable. Merge conflict reporting
  has the widest partial-outcome surface.

Parallelizable slices:

- S1 registry model/tests can run beside B1 protocol and B2/S2 backend work once
  Wave 0 completes.
- CLI help/parser tests for stash and branch can be drafted after protocol names
  are accepted, but dispatch must wait for core handlers.
- B4b branch list/create/delete can start after B2 and B4a, even if root
  participation B6 remains deferred.

## Multi-agent breakdown schedule

Use named agents with explicit ownership. Some files are unavoidable chokepoints:
`gitbackend.rs`, generated protocol files, and `gwz-cli/src/clirequest.rs` must
land in the order below. Agents are not alone in the codebase: each must check
current status before editing, avoid reverting others' work, and adapt to any
already-landed changes. Atlas owns protocol numbering, merge order, and final
gate runs.

| Agent | Ownership | Primary phases | May run in parallel with | Must wait for |
| --- | --- | --- | --- | --- |
| Atlas, coordinator | Cross-repo sequencing, merge order, release gates, final conflict resolution | All waves | All agents | None |
| Archivist, docs agent | Scope promotion, docs revision pass, active/history moves, implemented API docs | Wave 0, D1-D4, final docs wave | Feature implementation only after docs gate clears | Atlas-approved scope and implemented behavior |
| Prisma, protocol agent | `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`, generated protocol, corpus, protocol tests | B1, S3, B4a, B5a | S1, B2, S2 | Atlas approval of final wire names and enum values |
| Ledger, stash registry agent | `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/stash/`, stash bundle YAML I/O, stash lock helper, `.gwz/` runtime boundary | S1 | B1, B2, S2 | Wave 0 |
| Forge-Branch, git backend agent | Branch primitives in `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/gitbackend.rs`, factored `checkout_branch` helper signature, `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/tests/` | B2 | S1, B1 | Wave 0 |
| Forge-Stash, git backend agent | Stash primitives in `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/gitbackend.rs`, `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/tests/` | S2 | S1, B1 | Wave 0; waits for Forge-Branch's landed helper signature before touching overlapping helper code |
| Switchboard, branch materialize agent | `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_materialize.rs`, branch-sourced snapshot behavior, shared switch helper | B3 | S1, S2 | B1 and B2 |
| Curator, branch command agent | `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_branch.rs`, branch list/create/delete orchestration | B4b | S4 planning, S5 renderer planning | B2 and B4a |
| Mergekeeper, branch merge agent | Branch merge orchestration, conflict reporting, lock behavior for clean/conflicted members | B5b | S6 after stash restore starts, docs updates | B3, B4b, and B5a |
| Vault, stash orchestration agent | `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stash.rs`, push/list/apply/pop/drop core handlers | S4, S5, S6 | B4b, B5b, CLI help once protocol is stable | S1, S2, S3 |
| Lens, CLI/UX agent | `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/globalargs.rs`, `clirequest.rs`, `main.rs`, renderers, help modules, parser tests | S7, B3 CLI, B4b CLI, B5b CLI | Core handler work once taut names are stable | Relevant protocol fields and handler signatures; central `clirequest.rs` edits land in Atlas-approved order |
| Sentinel, test/review agent | Adversarial review, risk checks, targeted workflow tests, final gate matrix | Review, phase gates | All implementation agents | Updated plan or candidate changes |

### Wave schedule

Wave 0 - scope and docs gate:

- Atlas freezes taut naming decisions, phase boundaries, root-repo deferrals, and
  the local-only stash registry decision.
- Archivist updates `/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GWZRequirements.md`
  and `/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GWZDesign.md` to promote
  branch/stash from deferred v0 operations into the accepted post-v0 feature
  track, or marks all later phases cannot-merge until that happens.
- Archivist reconciles the stale directory-layout text in `GWZDesign.md` with
  actual `gwz.conf/` paths and documents `.gwz/` as local runtime state.
- Sentinel performs adversarial review before implementation begins.
- No implementation source edits beyond protocol planning.

Gate W0:

- `cargo test --test rename` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- `cargo test --test rename` in `/Users/owebeeone/limbo/gwz-dev/gwz-cli`
- Review confirms `GWZRequirements.md` no longer requires branch/stash to be
  hidden/rejected for the implementation target.

Wave 1 - parallel foundations:

- Prisma works B1 protocol for `MaterializeTargetKind.branch` and snapshot
  source.
- Ledger works S1 stash registry model, YAML I/O, validation, `.gwz/` boundary,
  and file-backed workspace mutator lock.
- Forge-Branch works B2 branch backend primitives, factoring shared
  `checkout_branch` logic without regressing materialize.
- Forge-Stash works S2 stash backend primitives after Forge-Branch lands the
  factored helper signature for any overlapping `gitbackend.rs` code.

Gate W1:

- `python protocol/regen.py --check` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- `cargo test --test protocol` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- `cargo test git::tests` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- Materialize branch-restore regression test remains green.
- Stash registry tests pass without native Git stash behavior.
- Cross-process lock contention test passes.

Wave 2 - branch switch plus stash and branch command protocol:

- Switchboard implements B3 core branch switch and branch-sourced snapshots.
- Lens wires B3 CLI flags after Switchboard exposes handler behavior.
- Prisma works S3 stash protocol after stash registry field names are stable.
- Prisma works B4a branch command protocol before Curator starts B4b.

Gate W2:

- `python protocol/regen.py --check` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- `cargo test --test protocol` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- `cargo test workspace_ops::tests` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
  with targeted materialize/snapshot branch filters once tests are named.
- B3 tests prove clean switch, dirty rejection, mixed-branch snapshot rejection,
  unborn/detached errors, and named-branch no-switch behavior.
- CLI parser tests cover `materialize --switch` and `snapshot --branch`.

Wave 3 - branch commands plus stash push/list:

- Curator implements B4b branch list/create/delete handler.
- Lens wires B4b CLI parsing/rendering.
- Vault implements S4 stash push after S1-S3 are in place.
- Vault starts S5 stash list/reconciliation after push metadata exists.
- Prisma works B5a merge protocol before Mergekeeper starts B5b.

Gate W3:

- `cargo test workspace_ops::tests` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
  with targeted branch list/create/delete and stash push/list filters once tests
  are named.
- Branch create rollback and delete partial-report tests pass.
- Stash push/list tests pass for mixed clean/dirty, untracked/ignored policy,
  and reconciliation drift.
- CLI parser tests cover branch and stash flag conflicts.
- `python protocol/regen.py --check` and `cargo test --test protocol` pass after
  B5a.

Wave 4 - restore, merge, and conflict semantics:

- Vault implements S6 stash apply/pop/drop and partial restore metadata.
- Mergekeeper implements B5b branch merge and conflict reporting.
- Lens wires restore and merge renderers after core response shapes stabilize.
- Sentinel reviews partial/conflict behavior before root participation is
  attempted.

Gate W4:

- `cargo test workspace_ops::tests` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
  with targeted stash restore and branch merge filters once tests are named.
- Stash restore tests cover preserve-index, missing payload, dirty destination,
  clean conflict, and partial bundle visibility.
- Branch merge tests cover fast-forward, merge commit, conflict paths, aggregate
  status, lock writes for clean members only, and existing merge-state rejection.
- CLI workflow tests cover human/JSON rendering for stash restore and branch
  merge conflict responses.

Wave 5 - root/remote polish and docs:

- B6 root branch support is opt-in and starts only after member behavior is
  stable.
- Archivist moves superseded planning docs to history only after Atlas confirms
  this combined plan is accepted.
- Archivist executes the documentation revision pass below for implemented
  behavior.

Gate W5:

- Root switch reload tests pass if root support lands.
- `cargo test --locked` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- `cargo test --locked` in `/Users/owebeeone/limbo/gwz-dev/gwz-cli`
- `cargo fmt --check` in both repos
- `python protocol/regen.py --check` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- Targeted CLI workflow tests pass.

## Risk and test matrix

| Feature | Risk | Mitigation | Required tests/gates |
| --- | --- | --- | --- |
| Scope gate | Requirements could drift back toward hiding branch/stash. | Wave 0 promoted the features in authoritative docs; future revisions must preserve the implemented scope or explicitly record deferrals. | Review of `GWZRequirements.md`/`GWZDesign.md`; rename tests. |
| Stash registry | Bundle metadata drifts from native Git stash state or is lost with local runtime state. | Registry is local-only membership metadata; native payload existence is reconciled on list/restore; list shows manual remediation for orphans, including `.gwz/` wipe recovery. | Registry round trips, missing native payload drift, orphan native payload drift, local-only docs. |
| Runtime boundary | Writing `.gwz/stash` dirties the root repo. | Add `.gwz/` to managed root exclude/runtime boundary. | Root remains clean after stash bundle write. |
| Stash identity | `stash@{n}` shifts after any stash mutation. | Persist object id; treat `stash@{n}` as display-only; re-resolve index before each mutation. | Older-bundle pop after newer stash changes indices; non-GWZ stash untouched. |
| Stash push | Native mutation can succeed before registry update or fail mid-batch. | Pending intent before mutation; push lifecycle records `unattempted/saving/saved/empty/failed`; partial metadata retained. | Mid-operation failure fixture; failed push lifecycle fixture; orphan discovery fixture. |
| Stash restore | Clean destination can still conflict, especially with preserve-index, and git2 may leave no conflict markers. | Preflight dirty destinations; attempt preserve-index; on native conflict leave member pending and verify worktree was not silently modified. | Dirty reject before mutation; clean conflict unchanged-worktree result; staged-state restore. |
| Stash partial restore | User accidentally restores only part of a workspace bundle. | Require explicit member selection for partial apply/pop/drop; list shows partial states. | Per-member pop/apply/drop and subsequent bundle-wide restore. |
| Workspace mutator locking | Two processes mutate stash or branch state concurrently; advisory locking may be unreliable on network filesystems. | File-backed workspace mutator lock acquired before first native mutation and shared by stash/branch; document NFS/network caveat. | Two-process lock contention test; docs describe workspace-wide serialization. |
| Branch switch | Switching branch can move worktrees while lock still records old state. | Re-observe after switch and rewrite lock from observed state. | Switch rewrites lock from observed state and syncs boundary. |
| Branch dirty policy | Git checkout/merge can overwrite or conflict with local changes. | Require clean selected repos for switch/create-and-switch/merge; delete non-current branch ignores dirtiness. | Dirty switch and dirty merge reject before mutation; dirty non-current delete allowed. |
| Branch snapshot | Current selected members can be on mixed branches, detached HEADs, or unborn HEADs. | Typed preflight errors; named branch resolves refs without switching or reading worktree dirty state. | Mixed branch, detached HEAD, unborn HEAD, missing named branch, no-worktree-switch tests. |
| Branch create/delete | Cross-repo TOCTOU can violate transactional all-or-nothing. | Shared workspace mutator lock; create rollback for branches created by this op; delete reports partial honestly after full preflight. | Existing different-commit reject, missing start reject, create rollback, current branch delete reject, delete partial-report. |
| Branch merge | Conflicts are expected and cannot be preflighted away; merge primitive operates on current HEAD. | Current-attached-branch-only v0; protocol carries conflict paths; rewrite lock only for clean members. | Fast-forward, merge commit, conflict paths, aggregate status, lock behavior, existing merge-state rejection. |
| Root branch support | Switching root can replace `gwz.conf/` while operation uses old metadata. | Defer root switch until explicit root reload phase with target manifest inspection or rollback. | Root reload/rollback tests in B6 only. |
| Protocol | Handwritten/shadow payloads diverge from taut authority. | Define request/response/status/error shapes in `protocol/gwz.taut.py`; regenerate and test corpus. | `python protocol/regen.py --check`; `cargo test --test protocol`. |
| CLI | CLI grows workspace semantics or direct Git behavior. | CLI only parses flags and renders taut responses; all semantics stay in core. | Parser unit tests plus workflow tests that dispatch to core handlers. |
| Documentation | Large doc set diverges from implemented branch/stash behavior. | Dedicated documentation revision pass with active/history disposition and command/API docs updated only when behavior lands. | Docs checklist, rename tests, protocol/docs catalog checks where applicable. |

## Documentation revision pass

Documentation is a named workstream, not a cleanup footnote. GWZ now has enough
dev docs, core reference docs, command docs, and machine-output docs that branch
and stash must land with a deliberate doc update plan.

### D0 - Authoritative scope and layout correction

Owner: Archivist, before implementation source edits.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GWZRequirements.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GWZDesign.md`

Work:

- Move branch, merge, and stash out of "must be hidden/rejected for v0" language
  for the implementation target, or clearly mark this plan as cannot-merge until
  that promotion lands.
- Reconcile the stale directory layout text with actual `gwz.conf/` artifact
  paths.
- Document `.gwz/` as local runtime state, including `.gwz/stash/` and
  `.gwz/locks/`.

Gate:

- `cargo test --test rename` in both repos.

### D1 - Protocol and API reference docs

Owner: Archivist with Prisma, after each protocol slice lands.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/Protocol.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/MessageCatalog.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/ErrorCatalog.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/EventCatalog.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/Reference.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/RustApi.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/GitBackend.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/OperationModel.md`

Work:

- Add taut request/response/error catalog entries for stash, branch, branch
  snapshot sources, and merge conflict payloads as they are implemented.
- Update `Reference.md` only after handlers exist.
- Update `GitBackend.md` with branch/stash primitive contracts and the
  `checkout_branch` factoring rule.
- Update `OperationModel.md` with the file-backed workspace mutator lock and
  partial/conflicted aggregate behavior.

Gate:

- `python protocol/regen.py --check`
- `cargo test --test protocol`

### D2 - CLI command documentation

Owner: Archivist with Lens, after CLI command surfaces land.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/commands/materialize.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/commands/snapshot.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/commands/status.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/commands/branch.md` (new)
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/commands/stash.md` (new)
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/CLI.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/Workflows.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/Concepts.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/MachineOutput.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/Troubleshooting.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/RootWorkspace.md`

Work:

- Add branch and stash command pages only after commands exist.
- Update materialize/snapshot docs for `--switch` and `--branch` after B3.
- Update workflows with safe multi-repo branch/stash examples.
- Update machine-output examples for stash bundle and branch conflict payloads
  after JSON shapes stabilize.
- Document local-only stash registry behavior and orphan remediation guidance.

Gate:

- CLI parser/help tests pass.
- `cargo test --test rename` in `/Users/owebeeone/limbo/gwz-dev/gwz-cli`.

### D3 - Workspace artifact and operational docs

Owner: Archivist with Ledger/Vault/Switchboard.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/WorkspaceArtifacts.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/Embedding.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/MemberListing.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/AgentBootstrap.md`

Work:

- Document `.gwz/stash/` and `.gwz/locks/` as local runtime paths, not tracked
  artifacts.
- Document the root `.git/info/exclude` boundary for `.gwz/`.
- Update embedding guidance for the mutator lock and stash/branch handler calls.
- Document that the mutator lock is workspace-wide across stash and branch
  operations, and call out the advisory-lock caveat for NFS/network-mounted
  workspace roots.
- Keep member listing docs unchanged unless branch/stash selection behavior
  changes list semantics.

Gate:

- Docs reflect actual code paths, not historical `workspace/` or
  `.gwz/snapshots` layouts.

### D4 - History and gap tracker cleanup

Owner: Archivist, after this combined plan is accepted and/or features land.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/dev-docs/GwzGaps.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/dev-docs/GwzStashPlan.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzBranchPlan.md`
- Relevant `dev-docs/history/` directories in both repos.

Work:

- Update `GwzGaps.md` so it does not point active readers at a moved/superseded
  stash plan.
- Move superseded stash/branch plans to history only after this combined plan is
  accepted.
- After implementation, remove branch/stash from active gaps and replace with
  any explicit residual deferrals such as root stash, branch remote tracking,
  branch rename, stash repair/adopt, or merge continue/abort helpers.

Gate:

- No active doc links point to moved history files as the active plan.

## Documentation disposition

Remain active after this update:

- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzStashBranchPlan.md` - active
  implementation/documentation checkpoint for the combined work, with B6 root
  participation and post-v0 polish still deferred.
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzStashBranch-Review48.md`,
  `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzStashBranch-Review55.md`, and
  `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzStashBranch-Review48-2.md` -
  active review inputs until the second-review nits are accepted.
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/dev-docs/GwzStashSpec.md` - active
  command behavior spec until its behavioral content is moved into core docs or
  this combined plan.
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/dev-docs/GwzGaps.md` - active gap
  tracker until D4 records the residual gaps: root stash, root branch
  participation, remote branch tracking, branch rename, stash repair/adopt,
  merge continue/abort helpers, and branch merge `--into`.
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GWZDesign.md` - authoritative
  architecture/design; D0 scope/layout reconciliation has landed.
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GWZRequirements.md` -
  baseline requirements and deferred-operation record; D0 scope promotion has
  landed.
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/Reference.md` - active
  implemented API reference; update only when handlers/protocol actually land.

Move to history after the team accepts this combined plan:

- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/dev-docs/GwzStashPlan.md` - superseded
  by the stash phases and decisions in this document.
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzBranchPlan.md` - superseded
  by the branch phases and decisions in this document.

Updated during this documentation revision pass:

- User-facing command docs under `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/`
  for implemented branch/stash/snapshot/materialize behavior, including
  current-attached-branch merge and conflict reporting.
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/MessageCatalog.md`,
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/ErrorCatalog.md`, and core
  API/artifact docs for landed taut and handler shapes.

## Planning-change verification

For this documentation-only update, run:

- `cargo test --test rename` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- `cargo test --test rename` in `/Users/owebeeone/limbo/gwz-dev/gwz-cli`

Implementation phases add their own gates listed above.
