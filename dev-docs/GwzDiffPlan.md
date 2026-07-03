# GWZ diff implementation plan

Status: design plan, 2026-06-27.

This plan covers a `gwz diff` command that behaves like `git diff` over the
GWZ unified workspace: the root repository plus materialized member Git
repositories rendered as one workspace-relative manifest and assembled patch
output.

This document was reviewed against:

- `/Users/owebeeone/.codex/attachments/39befd09-d37c-4cac-86f9-0f17a9256b5d/pasted-text.txt`
  (`git diff` manual text supplied with this task).
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/artifact/mod.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/gitbackend.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_materialize.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/status/status_member.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stage.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/stage_routing.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/operation/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/lib.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/dispatch/mod.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/operations.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/bridge.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/client.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli.py`
- `/Users/owebeeone/limbo/gwz-dev/taut/docs/Reference.md`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan-Review48.md`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan-Review48-2.md`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan-Review48-3.md`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan-Review48-4.md`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan-Review55.md`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan-Review55-2.md`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan-Review55-5.md`
- Rust `git2 0.21.0`, backed by libgit2 `1.9.4`, from the local Cargo lock.

No implementation is part of this document. The concrete Taut snippets below
are proposals, not applied schema changes.

## Problem statement

`gwz diff` should make a GWZ workspace feel like a single Git repository for
diff review. Across the planned parity surface, a user should be able to run
commands such as:

```text
gwz diff
gwz diff --cached
gwz diff HEAD
gwz diff +start-project
gwz diff +start-project HEAD -- gwz-core/src
gwz diff main...topic -- gwz-core/src gwz-cli/src
gwz diff --name-status
gwz diff --stat
gwz diff --exit-code
```

and see workspace-relative paths:

```text
diff --git a/gwz-core/src/lib.rs b/gwz-core/src/lib.rs
--- a/gwz-core/src/lib.rs
+++ b/gwz-core/src/lib.rs
```

The implementation must preserve GWZ's soon-to-be client/server architecture.
`gwz-core` may run on a server. It must not make terminal-local decisions such
as launching `less`, detecting terminal color policy, or reading `PAGER`.
Those belong in client surfaces. `gwz-cli` owns the primary shipped command-line
UX. `gwz-py` is a Python protocol client with its own CLI and PyO3 bridge; full
`gwz-py` CLI parity for `diff` is still required because it proves the core/Taut
API surface and the PyO3 async/log bridge are complete. That parity requirement
does not justify a shared Rust client crate for pager, parser, and process policy
in v0.

## Current state

- `gwz-core` already depends on `git2 = 0.21`, which exposes libgit2 diff APIs:
  `diff_index_to_workdir`, `diff_tree_to_index`,
  `diff_tree_to_workdir_with_index`, `diff_tree_to_tree`, `Diff::print`,
  `Diff::stats`, and `Diff::find_similar`.
- `gwz-core/src/git/gitbackend.rs` centralizes Git behavior behind
  `GitBackend`. There is no diff primitive today.
- Combined status already solves much of the workspace projection problem:
  `status_member.rs` maps member-local file paths to workspace-relative paths
  and includes root repository status in the combined view.
- `gwz add` has pure workspace path routing in `stage_routing.rs`. It is useful
  starting infrastructure, but it is not full Git pathspec semantics.
- `OperationRuntime` supports accepted responses, lifecycle events, and final
  `operation.result`. Its event buffer is bounded and can emit `reset` on
  overflow. That is acceptable for progress events, but not acceptable as the
  only transport for patch bytes.
- Existing Taut protocol has request/response methods plus
  `events.subscribe` with `shape="log"`. Current GWZ operations do not yet use
  a precise operation-output log for byte-bearing command output; `gwz diff`
  should be the forcing function for that Taut capability.
- `gwz forall` is intentionally CLI-local. `gwz diff` must not follow that
  model because repository object access and revision resolution belong in
  `gwz-core`; terminal presentation belongs in the client.
- Taut's `log` shape is the intended append-only/read-from-offset/tail
  abstraction for whole-operation output feeds. v0 `gwz diff` should use that
  path for patch bytes rather than inventing diff-specific file-patch request
  plumbing.
- `gwz-py` links `gwz-core` through PyO3 and currently exposes `call`,
  `submit`, `subscribe_events`, `wait_events`, `operation_result`, and
  `try_operation_result`. Its current event subscription path is bounded and can
  reset, so D-1 must prove a separate precise Taut log read path through Rust
  and PyO3 before the diff schema freezes.

## Goals

- Match `git diff` semantics where they map cleanly to a unified GWZ workspace.
- Use libgit2 for the core diff engine whenever possible.
- Render one deterministic workspace-relative diff across the root repository
  and selected members.
- Keep `gwz-core` headless and server-safe.
- Return changed-file manifests and stream patch bytes through a precise Taut
  log without forcing the final response to hold a whole-workspace patch.
- Provide `gwz-cli` support that can consume GWZ core protocol, render diff
  output, apply color policy, launch a pager, and compute CLI exit codes.
- Make `gwz-py` diff CLI parity a proof that the core/Taut API surface and PyO3
  async/log bridge are complete, while keeping the implementation independent
  from `gwz-cli`.
- Preserve machine-consumable modes for JSON/JSONL without mixing arbitrary
  patch bytes into response metadata.

## Non-goals for v0

- A synthetic root Git object database that stores all member blobs in one tree.
  GWZ should compose per-repo diffs, not pretend libgit2 has a multi-repo
  repository.
- `git apply` compatibility at the workspace root. The emitted patch should be
  review-friendly and path-stable. Applying it safely would require a later
  `gwz apply` splitter that routes hunks back to owning repositories.
- External diff helpers, textconv filters, word diff, moved-line coloring, and
  full combined merge diff parity in the first release.
- Server-side pagers, terminal color detection, or `less` invocation.

## Architecture decisions

### AD1 - Diff is a read-only core operation

`gwz diff` should be a `gwz-core` operation because revision resolution,
repository object access, pathspec partitioning, and member selection are core
workspace semantics. It should not mutate repositories, locks, manifests, or
workspace artifacts. Short-lived manifest/output-log cache entries are allowed
runtime state, but v0 should not require persistent server-side diff state.

### AD2 - Diff is computed per repository and projected as one manifest

The core operation runs the requested Git diff independently in:

1. The root repository, when included.
2. Each selected materialized Git member, in manifest order.

Each repo produces a normal libgit2 `Diff`. Core projects those per-repo diffs
into a single workspace-relative changed-file manifest and a log-shaped patch
output feed. Patch paths are rewritten relative to the workspace root before
they enter the output log.

This avoids inventing a virtual Git repository while preserving the user model
of one unified workspace.

### AD3 - Core emits canonical uncolored output

`gwz-core` should emit canonical, uncolored diff content plus structured
summary metadata. Color and pager decisions are presentation concerns.

The client library may colorize patch lines when the selected output mode is a
terminal mode. It should not ask core to embed ANSI escapes by default.

### AD4 - v0 diff is manifest plus log-shaped output

Whole-workspace patch output is the v0 backbone, but it must be delivered by a
Taut `shape="log"` feed rather than by stuffing bytes into the final response.
The v0 protocol is:

1. `diff`: plan the workspace diff, resolve targets and operands per repo, and
   return a changed-file manifest, aggregate summary, scoped operand
   classification, and an optional output-log reference.
2. `diff.output`: read the patch output log. The log payload carries exact patch
   bytes, including NUL bytes and binary hunks. The Taut log layer owns cursor or
   byte-offset precision, tailing behavior, EOF, close/cancellation semantics,
   and backpressure rules.

Manifest entries include workspace paths, status, rename pairing, binary flag,
per-file stat counts when available, repo scope, and per-repo operand
classification. Copy pairing is omitted in v0 because `find_copies=true` is
rejected.
Patch output is assembled by core in final display order: root first when
included, then selected members in manifest order.

When the requested mode has no patch output (`--quiet`, `--name-only`,
`--name-status`, `--stat`, `--numstat`, `--shortstat`, `--summary`, or JSON
metadata-only modes), `diff` may omit the output-log reference. Human patch
output, JSONL patch records, and any byte-bearing diff form are read from
`diff.output`.

The log can be backed by a retained per-operation diff/render state or by an
internal bounded spool. The protocol must not require clients to issue one
request per changed file. Worktree races are handled at operation scope: once
`diff` has accepted the request, the output log should represent the same
planned diff. If v0 cannot retain enough state to guarantee that, it must mark
the affected output record as stale rather than silently rendering a different
patch.

### AD5 - Final operation results stay metadata-only

`operation.result` should not carry whole-workspace patch text. It may carry
final status, member outcomes, errors, and small summaries. Patch bytes belong
to the `diff.output` log.

### AD6 - Client behavior lives outside core, and CLOL009 stays in `gwz-cli`

Do not add a `gwz-client` workspace crate for v0. The case for extraction is
real: full `gwz-py` CLI parity means Python repeats CLI parsing and process
behavior, and a shared client API could reduce drift. The decision is still no
for this plan. It would create a new supported API surface and PyO3 binding
burden before the protocol itself is proven.

`gwz-cli` should own the polished CLI implementation. Its diff code may be split
into internal modules, but those modules are not a new public crate unless a
separate future decision reopens CLOL009.

`gwz-cli` responsibilities:

- Parse Git-like CLI diff options into structured request options while leaving
  ambiguous positional operands for core.
- Choose local in-process core or remote core transport.
- Call `diff`, read `diff.output` when patch bytes are requested, and assemble
  human patch output client-side.
- Render stdout/stderr.
- Apply terminal color policy.
- Launch and feed `less` or another pager.
- Implement `--exit-code` and `--quiet` process status.

`gwz-py` responsibilities:

- Use the existing PyO3 request/response bridge for `diff` and add a precise
  Taut log reader for `diff.output`.
- Add Python protocol-generated diff messages.
- Add Python CLI support for `gwz diff` with behavior matching `gwz-cli` for the
  supported Tier 0 surface.
- Cover parser, manifest, output-log, pager/color/process-policy, JSON/JSONL,
  and exit-code parity. Treat parity failures as core/Taut or PyO3 async/log
  bridge gaps unless the difference is explicitly documented as Python-only
  process glue.

### AD7 - Diff uses shared target selection

The default unified diff includes the workspace root repository and active
materialized members by using the shared target-selection resolver with
`CommandDefaultTargets::All` and `RootSelectionPolicy::Allow`. Diff should not
define its own root-selection flag.

Rules:

- No explicit selection: include root plus all active materialized Git members.
- `--all`: include root plus all active materialized Git members. For diff,
  `--all` means the whole unified workspace rather than member-only selection.
- `--target @root`: include the workspace root.
- `--all --no-target @root`: include all active materialized Git members but not
  the root.
- Explicit member selection includes only the selected members unless the
  selection also includes `@root`, `@all`, or a future declared target set that
  expands to root.
- Pathspecs under `gwz.conf/`, `Cargo.toml`, or other root-owned paths route to
  the root repository when root is in the selected candidate set.
- Pathspecs under a member path target that member with the member prefix
  stripped before calling libgit2.

### AD8 - Exit code follows Git only when requested

Like `git diff`, plain `gwz diff` exits 0 even when differences exist.
`--exit-code` exits 1 when differences exist and 0 when there are no
differences. `--quiet` suppresses patch output and implies `--exit-code`.

This should be handled by the client using final `DiffSummary.has_differences`.
The core aggregate status may be `dirty` or `ok`, but CLI exit status is a
client contract.

### AD9 - Core owns positional revision/pathspec disambiguation

The client may parse options and the explicit `--` separator, but it must not
pre-classify ambiguous positional operands. Git-compatible classification needs
repository knowledge: `gwz diff foo` may mean a revision in one member and a
path in another. Core receives the raw operands before `--`, plus explicit
pathspecs after `--`, and resolves them per target repository after member/root
planning.

### AD10 - Request cwd is workspace-relative

Remote-ready requests cannot carry the client's absolute terminal path as the
semantic cwd. The client resolves its local cwd against the selected workspace
root and sends a workspace-relative logical cwd such as `""`, `gwz-core`, or
`gwz-core/src`. A local in-process transport may still know absolute paths, but
the protocol contract is workspace-relative.

### AD11 - Read-only root diffs must not depend on mutating excludes

`gwz diff` remains read-only. It must not repair `.git/info/exclude` just to
hide member directories or runtime files from the root diff. Root diff planning
should prefer a GWZ delta post-filter:

- compute the root diff normally;
- drop any root delta whose workspace path is under an active member path,
  `.gwz/`, or `gwz.conf/.tmp/`;
- build manifest entries, stats, and patch selection from the filtered delta
  list.

This is a correctness strategy, not only a fallback. It avoids relying on
negative libgit2 pathspec support, which the current Rust wrapper does not
expose directly. The tradeoff is performance: libgit2 may still walk excluded
paths. A later lower-level skip is an optimization if large root worktrees make
that expensive.

Root diff planning may still choose one of these alternatives if the D2 spike
proves it better:

- apply in-memory pathspec exclusions for every active member path, `.gwz/`, and
  `gwz.conf/.tmp/` when diffing the root repository, or
- validate the managed exclude boundary and fail with a clear diagnostic that
  asks the user to run a boundary-refreshing command.

For default `worktree_vs_index`, member directories and `.gwz/` are often
untracked and therefore absent from plain Git diff output anyway. The exclusion
rule remains required because staged root changes, tree comparisons, explicit
pathspecs, and future options can otherwise surface managed workspace content.

## Taut protocol proposals

The v0 protocol should add one ordinary request/response method to plan the
diff and one Taut `shape="log"` method to read byte-bearing output. The exact
numbering must be assigned during implementation after checking the current
generated schema and corpus.

### Service methods

```python
service("GwzCore",
    # Existing methods omitted.

    # Plan the diff and return manifest/summary plus an optional output log.
    method("diff", role="in",
           params=Params(request=Ref.DiffRequest),
           out=Ref.DiffManifestResponse),

    # Read exact diff output records. The final params should be the generic
    # Taut log-read params or the shape-generated equivalent, not a diff-specific
    # byte-window request.
    method("diff.output", role="out", shape="log",
           params=Params(log_id=STR),
           out=Ref.DiffOutputRecord))
```

Removed from v0: `diff.file`, `diff.files`, and diff-specific cancellation or
read-window methods. If the log reader needs offset, tailing, close, or
backpressure controls, those belong to the Taut log shape, not to a diff-only
service.

Rationale:

- `diff` gives programmatic consumers the data they naturally need first:
  which files changed, how they changed, and aggregate summaries.
- `diff.output` carries exact patch bytes without making the final response
  unbounded.
- `gwz diff | less` remains a client workflow: the client gets the manifest,
  reads the output log, applies color/pager policy, and writes a single
  coherent document.
- A client that quits the pager or hits a broken pipe closes or stops reading
  the Taut log. No domain-specific cancellation method is needed for v0.
- `gwz diff` should mature Taut log semantics for byte- or record-precise
  output rather than routing around them.

### Enum additions

```python
ActionKind=Enum(
    # existing values...
    diff=20)

DiffComparisonKind=Enum(
    # git diff: index -> worktree.
    worktree_vs_index=0,
    # git diff --cached [<commit>]: tree -> index.
    index_vs_tree=1,
    # git diff <commit>: tree -> worktree, using index data for staged deletes.
    worktree_vs_tree=2,
    # git diff <left> <right>, <left>..<right>, <left>...<right>.
    tree_vs_tree=3)

DiffOutputFormat=Enum(
    patch=0,
    raw=1,
    name_only=2,
    name_status=3,
    stat=4,
    numstat=5,
    shortstat=6,
    summary=7,
    patch_with_raw=8,
    patch_with_stat=9,
    no_patch=10)

DiffManifestMode=Enum(
    # Build full file list and stats for output or machine consumers.
    full=0,
    # Return only enough information to answer --quiet/fast any-difference.
    any_difference=1)

DiffAlgorithm=Enum(
    default=0,
    myers=1,
    minimal=2,
    patience=3)

DiffWhitespaceMode=Enum(
    default=0,
    ignore_all=1,
    ignore_change=2,
    ignore_eol=3,
    ignore_blank_lines=4)

DiffStatus=Enum(
    added=0,
    modified=1,
    deleted=2,
    renamed=3,
    copied=4,
    type_changed=5,
    unmerged=6)

DiffChunkEncoding=Enum(
    utf8=0,
    bytes=1)

DiffOutputRecordKind=Enum(
    patch_bytes=0,
    file_started=1,
    file_finished=2,
    stale_file=3,
    diagnostic=4)

DiffTargetExclusionReason=Enum(
    # A snapshot operand does not contain this member; commonly the member was
    # added after the snapshot was captured.
    snapshot_missing=0,
    # The snapshot contains this member but has no Git commit for it.
    snapshot_missing_commit=1,
    # v0 snapshots do not record a workspace-root commit.
    root_not_in_snapshot=2)
```

Do not add `less`, `pager`, or terminal color enums to `gwz-core` protocol.
Those belong to the client library.

### Request messages

```python
DiffComparison=Msg(
    kind=F(1, Ref.DiffComparisonKind),
    # Left side revision/tree/blob token, interpreted inside each target repo.
    left=F(2, STR, optional=True),
    # Right side revision/tree/blob token, interpreted inside each target repo.
    right=F(3, STR, optional=True),
    # Use merge-base(left, HEAD/right) for the old side. Covers --merge-base
    # and A...B forms after core operand resolution.
    merge_base=F(4, BOOL, optional=True))

DiffParsedTarget=Msg(
    # Stable within the manifest and scoped to exactly one root/member repo.
    target_id=F(1, STR),
    scope=F(2, Ref.DiffRepoScope),
    # Resolved by core per repository from DiffRequest operands, request flags,
    # and pathspecs.
    comparison=F(3, Ref.DiffComparison),
    # Repo-relative pathspecs after workspace routing.
    pathspecs=F(4, List(STR)),
    # Resolved object ids where available. Worktree sides may omit an oid.
    left_oid=F(5, STR, optional=True),
    right_oid=F(6, STR, optional=True),
    merge_base_oid=F(7, STR, optional=True),
    # Present when a side came from a GWZ snapshot operand such as +start.
    left_snapshot_id=F(8, STR, optional=True),
    right_snapshot_id=F(9, STR, optional=True))

DiffOptions=Msg(
    output_format=F(1, Ref.DiffOutputFormat, optional=True),
    context_lines=F(2, INT, optional=True),
    interhunk_lines=F(3, INT, optional=True),
    algorithm=F(4, Ref.DiffAlgorithm, optional=True),
    whitespace=F(5, Ref.DiffWhitespaceMode, optional=True),
    find_renames=F(6, BOOL, optional=True),
    # Deferred. v0 rejects this until copy rendering is explicitly implemented.
    find_copies=F(7, BOOL, optional=True),
    rename_threshold=F(8, INT, optional=True),
    rename_limit=F(9, INT, optional=True),
    binary=F(10, BOOL, optional=True),
    text=F(11, BOOL, optional=True),
    full_index=F(12, BOOL, optional=True),
    abbrev=F(13, INT, optional=True),
    reverse=F(14, BOOL, optional=True),
    null_terminated=F(15, BOOL, optional=True),
    src_prefix=F(16, STR, optional=True),
    dst_prefix=F(17, STR, optional=True),
    no_prefix=F(18, BOOL, optional=True),
    line_prefix=F(19, STR, optional=True),
    ignore_submodules=F(20, STR, optional=True),
    diff_filter=F(21, STR, optional=True),
    manifest_mode=F(22, Ref.DiffManifestMode, optional=True))

DiffRequest=Msg(
    meta=F(1, Ref.RequestMeta),
    # Workspace-relative logical cwd used to resolve relative path operands.
    workspace_cwd=F(2, STR, optional=True),
    # Positional tokens before an explicit "--". Core classifies these per
    # target repo because rev/path ambiguity needs repository knowledge.
    # Tokens of the form +<snapshot_id> are GWZ snapshot operands.
    operands=F(3, List(STR)),
    # Pathspecs after an explicit "--"; resolved relative to workspace_cwd.
    explicit_pathspecs=F(4, List(STR)),
    options=F(5, Ref.DiffOptions, optional=True),
    # True for --cached or --staged. Selects index-vs-tree forms.
    cached=F(6, BOOL, optional=True),
    # True for --merge-base. A...B syntax is still parsed from operands.
    merge_base=F(7, BOOL, optional=True))
```

The protocol should be structured. Do not make the core API a raw `git diff`
argv tunnel. The client parser can be Git-compatible for options and the `--`
boundary while the wire contract remains stable and language-neutral. Parsed
operation flags such as `--cached`/`--staged` and `--merge-base` must be
explicit request fields, not hidden in `operands`. Core owns classification of
ambiguous positional operands.

### Manifest, output, and summary messages

```python
DiffRepoScope=Msg(
    # True for the workspace root repository.
    root=F(1, BOOL, optional=True),
    member_id=F(2, STR, optional=True),
    member_path=F(3, STR, optional=True),
    source_kind=F(4, Ref.SourceKind, optional=True))

DiffExcludedTarget=Msg(
    scope=F(1, Ref.DiffRepoScope),
    reason=F(2, Ref.DiffTargetExclusionReason),
    # The snapshot operand that caused the exclusion, without the leading plus.
    snapshot_id=F(3, STR, optional=True),
    message=F(4, STR, optional=True))

DiffFileEntry=Msg(
    # Stable within the manifest, opaque to clients, and not parsed by core in
    # output records. Scope/status/old_path/new_path are the structured identity.
    file_id=F(1, STR),
    scope=F(2, Ref.DiffRepoScope),
    status=F(3, Ref.DiffStatus),
    # Workspace-relative. new_path == old_path except for rename/copy.
    old_path=F(4, STR, optional=True),
    new_path=F(5, STR, optional=True),
    old_mode=F(6, INT, optional=True),
    new_mode=F(7, INT, optional=True),
    similarity=F(8, INT, optional=True),
    insertions=F(9, INT, optional=True),
    deletions=F(10, INT, optional=True),
    is_binary=F(11, BOOL, optional=True))

DiffRepoSummary=Msg(
    scope=F(1, Ref.DiffRepoScope),
    has_differences=F(2, BOOL),
    files_changed=F(3, INT),
    insertions=F(4, INT),
    deletions=F(5, INT),
    files_manifested=F(6, INT))

DiffSummary=Msg(
    has_differences=F(1, BOOL),
    repos_examined=F(2, INT),
    repos_with_differences=F(3, INT),
    files_changed=F(4, INT),
    insertions=F(5, INT),
    deletions=F(6, INT),
    repo_summaries=F(7, List(Ref.DiffRepoSummary)))

DiffManifestResponse=Msg(
    response=F(1, Ref.ResponseEnvelope),
    files=F(2, List(Ref.DiffFileEntry)),
    summary=F(3, Ref.DiffSummary, optional=True),
    # Scope-addressable per-repo operand classification resolved by core.
    targets=F(4, List(Ref.DiffParsedTarget)),
    # Omitted when the requested mode has no patch/byte output.
    output=F(5, Ref.DiffOutputLogRef, optional=True),
    # Candidate targets that were intentionally excluded before diff execution.
    excluded_targets=F(6, List(Ref.DiffExcludedTarget)))

DiffOutputLogRef=Msg(
    # Opaque log handle (taut-shape D3 canonical name; was `output_id`).
    log_id=F(1, STR),
    format=F(2, Ref.DiffOutputFormat),
    encoding=F(3, Ref.DiffChunkEncoding, optional=True))

DiffOutputRecord=Msg(
    kind=F(1, Ref.DiffOutputRecordKind),
    # Optional structured correlation for file-scoped records.
    scope=F(2, Ref.DiffRepoScope, optional=True),
    file_id=F(3, STR, optional=True),
    entry=F(4, Ref.DiffFileEntry, optional=True),
    # Present for patch_bytes records. Taut log delivery owns record/cursor
    # precision; this payload must preserve exact bytes.
    data=F(5, BYTES, optional=True),
    # True when the retained/planned diff could not render this file because the
    # worktree changed before output materialization.
    stale=F(6, BOOL, optional=True),
    diagnostic=F(7, STR, optional=True))
```

Protocol contracts:

- `diff` runs the full per-repo `Diff`, including rename similarity, and
  returns manifest entries in the final display order. Copy similarity is
  deferred: v0 rejects `find_copies=true`.
- `DiffRepoSummary.files_changed` counts changed files before GWZ root/member
  filtering where libgit2 can report that cheaply; `files_manifested` counts
  entries actually surfaced after pathspec, selection, and root-exclusion
  filtering.
- Manifest data alone satisfies `--name-only`, `--name-status`, `--stat`,
  `--numstat`, `--shortstat`, `--summary`, `--quiet`, JSON metadata modes, and
  the exit-code decision. `--exit-code` without `--quiet` still reads and prints
  the patch output log.
- For `--quiet`, core may use `DiffManifestMode.any_difference`: stop at the
  first detected difference, skip similarity detection, omit `files`, and return
  only enough summary data for exit-code behavior. Do not use this mode for
  JSON summaries, name/status/stat output, or patch assembly.
- The output log renders patch bytes in manifest order and may include
  file-scoped boundary records for machine consumers. The client must not need
  to parse patch text to correlate records with manifest entries.
- The log implementation should use retained/cache-backed output state to avoid
  client-visible per-file recompute races. The cache key combines normalized
  request data with per-target input fingerprints and must change when any input
  file, index, or object that can affect output changes. If v0 uses lazy
  rendering and a mutable input changes before a file can be rendered, emit a
  `stale_file` record and mark the final operation result partial/diagnostic
  rather than silently emitting a patch for different content.
- For ordinary add/modify/delete/type-change entries, output rendering may
  narrow internal work to the relevant path. For rename entries, rendering must
  include both `old_path` and `new_path`, preserve similarity detection, and
  emit matching rename headers. Copy entries are not part of v0.
  `find_copies=true` is rejected until a later copy-rendering project can
  reproduce the source set, including unmodified copy sources when copy
  detection allows them.
- The Taut log layer provides chunking, read offsets/cursors, tailing, EOF,
  close/cancellation, and backpressure. Do not add diff-only byte-window fields
  to the protocol.
- Literal explicit files may still use the same `diff` planning request with a
  narrowly routed pathspec. Directory pathspecs, globs, and bare `gwz diff` also
  use `diff`; the difference is only how many files enter the manifest and
  output log.
- Rename/copy detection for explicit single-file requests is scoped to that
  filtered path set, matching `git diff -- <path>` behavior. Full rename/copy
  pairing is a manifest discovery feature.

### Taut extension notes

`gwz diff` v0 depends on maturing Taut `shape="log"` for precise output. Do not
grow diff-specific substitutes for generic Taut concerns.

Required Taut log properties:

- Byte- or record-precise cursor semantics. A client must be able to resume from
  a known position without duplicated or skipped patch bytes.
- Exact `BYTES` payload support through Rust, PyO3, and Python, including NUL
  bytes and binary patch hunks.
- Explicit tailing and EOF behavior. A client can distinguish "no records yet"
  from "the output log is complete".
- Explicit close/cancellation semantics. Pager quit, broken pipe, or client
  disconnect should let core stop rendering and release retained diff state.
- Clear backpressure and retention bounds. If a client is too slow or abandons
  the output, core must have a defined cleanup path.
- Log records must not flow through the bounded operation-event buffer. Progress
  and lifecycle events can reset; patch output cannot.

Diff may define its own log record payload (`DiffOutputRecord`), but not its own
cursor, tail, close, or cancellation model.

## Diff semantics

### Comparison forms

Core maps common Git forms as follows after resolving raw operands per target
repo:

| CLI form | Comparison kind | libgit2 call |
| --- | --- | --- |
| `gwz diff` | `worktree_vs_index` | `repo.diff_index_to_workdir(None, opts)` |
| `gwz diff --cached` | `index_vs_tree` with `left=HEAD` | `repo.diff_tree_to_index(head_tree, None, opts)` |
| `gwz diff --staged` | same as `--cached` | same |
| `gwz diff <commit>` | `worktree_vs_tree` with `left=<commit>` | `repo.diff_tree_to_workdir_with_index(tree, opts)` |
| `gwz diff +<snapshot>` | `worktree_vs_tree` with `left=<snapshot member commit>` | same after snapshot resolution |
| `gwz diff <a> <b>` | `tree_vs_tree` | `repo.diff_tree_to_tree(a_tree, b_tree, opts)` |
| `gwz diff <a>..<b>` | `tree_vs_tree` | same after resolving omitted sides as `HEAD` |
| `gwz diff <a>...<b>` | `tree_vs_tree` with merge base old side | `merge_base(a,b)` then `diff_tree_to_tree` |
| `gwz diff --merge-base <commit>` | `worktree_vs_tree` with merge base old side | `merge_base(commit, HEAD)` then `diff_tree_to_workdir_with_index` |
| `gwz diff --cached <commit>` | `index_vs_tree` | `diff_tree_to_index(commit_tree, None, opts)` |
| `gwz diff --cached --merge-base <commit>` | `index_vs_tree` with merge base old side | `merge_base(commit, HEAD)` then `diff_tree_to_index` |

The client sends parsed comparison flags (`cached` for `--cached`/`--staged`,
`merge_base` for `--merge-base`), rendering/filter options, `operands` before
`--`, and `explicit_pathspecs` after `--`. Revision strings are resolved
independently inside each target repository. A branch name such as `main` means
`main` in each member. A missing ref in one member is a member-scoped error;
global `--partial` decides whether other members continue. If a token before
`--` is ambiguous, core follows Git's disambiguation rules per repo and returns
a member-scoped diagnostic when the token cannot be classified safely.

### GWZ snapshot operands

`gwz diff` should support GWZ snapshot operands as a first-class GWZ extension.
An operand before `--` with the form `+<snapshot_id>` is a snapshot reference,
where the stored snapshot id is `<snapshot_id>` without the leading plus. Core
resolves these operands before calling libgit2; it never passes the `+...` token
to Git.

Snapshot operand rules:

- `+<snapshot_id>` reads `gwz.conf/snapshots/<snapshot_id>.yaml` and validates
  the artifact schema and workspace id.
- Snapshot operands can appear anywhere a commit/tree operand can appear:
  `gwz diff +start`, `gwz diff +base +tip`, `gwz diff +base..HEAD`, and
  `gwz diff +base...main`.
- Range parsing happens before endpoint resolution, so `+base..+tip` and
  `+base...+tip` are parsed as two snapshot endpoints rather than one snapshot
  id containing dots.
- For a member target, `+snapshot` resolves to that member's recorded
  `members.<member_id>.commit`. The recorded source kind must be Git and the
  commit must be present.
- Snapshot artifacts do not currently record the workspace root repository. When
  a snapshot operand is present, the default candidate set is narrowed to members
  covered by every snapshot operand in the comparison. Explicit `--target @root`
  with a snapshot operand is a typed error.
- Targets excluded by snapshot narrowing are reported in
  `DiffManifestResponse.excluded_targets`. This is required for members added
  after the snapshot was captured: the target is not part of the diff, but the
  response must explain that the snapshot did not contain it.
- If an explicitly selected member is absent from a referenced snapshot, or its
  snapshot entry has no commit, core returns a member-scoped typed error. Global
  `--partial` controls whether other selected members continue.
- A path after `--` is always a pathspec, not a snapshot operand. For example,
  `gwz diff -- +notes.txt` routes a workspace path literally named
  `+notes.txt`.

This keeps snapshot resolution a core operation: the client preserves raw
operands, and core translates snapshot endpoints to per-target commit ids before
libgit2 revision lookup and diff generation.

### Unborn repositories

Rules should match Git where possible:

- `--cached` with no `HEAD`: compare an empty tree to the index.
- `gwz diff` with no `HEAD`: compare index to worktree, as usual.
- `<commit>` forms in an unborn repo reject that member with a typed error.

### Untracked files

Plain `git diff` does not show untracked files. `gwz diff` should not include
untracked content by default even though `gwz status` reports untracked files.

If a later `--include-untracked` extension is added, it should map to
`DiffOptions::show_untracked_content(true)` and should be documented as a GWZ
extension unless Git adds equivalent porcelain behavior.

### Root and member ordering

Output order:

1. Root repository diff, if included and if it has output.
2. Member diffs in manifest order.

Within each repo, preserve libgit2/Git diff ordering. Across repos, do not sort
paths globally because that would reorder the manifest, complicate client-side
patch assembly, and require extra buffering.

The root repo must exclude active member directories, `.gwz/`, and
`gwz.conf/.tmp/` in memory when computing root diffs. This protects read-only
diff from stale `.git/info/exclude` state and prevents runtime/temp files from
surfacing in root output.

### Workspace path rendering

For each member repo:

- Patch headers should use workspace paths, not member-relative paths.
- Default prefixes are `a/<member_path>/` and `b/<member_path>/`.
- `--src-prefix=<p>` becomes `<p><member_path>/`.
- `--dst-prefix=<p>` becomes `<p><member_path>/`.
- `--no-prefix` still keeps `<member_path>/` so output remains unified.
- Root output uses Git defaults: `a/`, `b/`, or the requested custom prefixes.

Implementation must test rename/copy extended headers. Git patch format prints
some extended paths without `a/` and `b/` prefixes. If libgit2 does not prefix
those paths as needed, GWZ must render patch headers from `DiffDelta` and
`DiffLine` callbacks instead of blindly forwarding `Diff::print` output.

### Pathspec routing

Start by extracting the path-to-repo partitioning logic from `stage_routing.rs`
into shared workspace pathspec routing. Do not reuse its output ordering
directly: `stage_routing.rs` sorts with a `BTreeMap`, while diff output must use
root first and then the manifest member vector order.

Selection and pathspec intersection:

1. Build candidate repositories from GWZ selection:
   - Resolve `RequestMeta.selection` with `CommandDefaultTargets::All` and
     `RootSelectionPolicy::Allow`.
   - No explicit selection and `--all` both include root plus all active
     materialized Git members.
   - Explicit selectors are honored literally: `--target mem_app` selects that
     member, `--target @root` selects root, and `--all --no-target @root`
     selects all active materialized Git members except root.
2. Route pathspecs relative to `DiffRequest.workspace_cwd` and intersect them
   with the candidate list:
   - No pathspec means keep the candidate list unchanged.
   - A member pathspec keeps only that member if it is in the candidate list.
   - A pathspec at or above member boundaries fans out only to candidate
     members.
   - A root-owned pathspec keeps root only if root is in the candidate list and
     removes members that cannot match it.
   - Pathspecs never add root or a member that was excluded by explicit target
     selection.
3. A valid selection/pathspec intersection that is empty is a clean no-diff
   result, not an error. Example: explicit member `A` plus pathspec `B/file`
   returns no differences when both references are valid but non-overlapping.
4. Pathspecs outside the workspace, explicit pathspecs into inactive or
   unmaterialized members, and ambiguous paths that cannot be routed safely are
   typed request errors.

For v0:

- Support normal literal file and directory pathspecs.
- Support pathspecs at or above member boundaries by fan-out.
- Strip the member prefix before passing pathspecs to libgit2.
- Reject pathspecs outside the workspace.
- Resolve relative path operands against `DiffRequest.workspace_cwd`, not a
  client absolute cwd.
- Preserve manifest order after partitioning and deduplication.
- Treat complex Git pathspec magic as unsupported unless tests prove routing is
  correct.

Later work should add a real workspace pathspec layer for Git pathspec magic,
glob semantics, exclusions, and `:(top)` behavior.

## Libgit2 coverage

Direct support available through Rust `git2 0.21`:

- Patch/raw/name-only/name-status/patch-id output via `DiffFormat`.
- Context lines and interhunk context.
- Minimal and patience algorithms.
- Whitespace ignore modes: all whitespace, whitespace change, EOL whitespace,
  blank lines.
- Binary patch output via `show_binary`.
- Force text or binary.
- Pathspec filtering.
- Prefix customization.
- Reverse diff.
- Rename/copy detection via `DiffFindOptions` and `Diff::find_similar`.
- Per-repo diff stats via `Diff::stats`; per-file counts likely need a GWZ pass
  over deltas/patch lines so manifest rendering can produce workspace-correct
  stats.

Likely needs GWZ/client rendering or deferred support:

- Histogram algorithm. The Rust `git2 0.21` wrapper exposes minimal and
  patience controls but no histogram setter, so v0 must not advertise
  histogram in the protocol.
- Color, moved-line coloring, and word diff.
- External diff helpers.
- Textconv.
- `--check` whitespace/conflict-marker diagnostics.
- Pickaxe options `-S`, `-G`, `--find-object`.
- Order files, skip/rotate, and advanced diffcore transforms.
- Combined merge diffs.
- `--no-index`, unless implemented as a source-to-source diff with a local
  filesystem adapter.
- `--patch-with-stat` until a GWZ patch+stat combiner exists.

## CLI plan

Implement polished command-line behavior in `gwz-cli` internal modules. Do not
extract a `gwz-client` crate in v0. Add parallel `gwz-py` work for full diff CLI
parity; parity proves the core/Taut API surface and PyO3 async/log bridge are
complete, even though the client process code is not shared.

Suggested `gwz-cli` modules:

- `transport`: trait for local in-process core and future remote core clients.
- `diff_args`: Git-compatible parser for options and `--`; ambiguous operands
  remain raw for core.
- `diff_manifest`: obtains and formats manifests and summaries.
- `diff_output`: reads `diff.output` log records and assembles patch output in
  manifest order.
- `diff_render`: applies color, line prefix, and machine output formatting.
- `pager`: launches `less` or configured pager and writes assembled bytes.
- `exit_code`: maps `DiffSummary` and options to process status.

`gwz-cli` should become a thin shell:

1. Parse global GWZ options.
2. Delegate diff argument parsing to `gwz-cli` diff modules.
3. Open local or remote GWZ core transport.
4. Run `diff` discovery unless all requested pathspecs are explicit literal
   files.
   - For human `--quiet` without JSON/JSONL metadata, request
     `DiffManifestMode.any_difference`.
5. Read `diff.output` when the response includes an output log reference.
6. Pipe assembled output to stdout or pager.
7. Exit according to `--exit-code`/`--quiet` from the manifest summary.

`gwz-py` should add parity-oriented Python-side pieces:

1. Add generated protocol classes for `DiffRequest`, `DiffManifestResponse`,
   `DiffOutputLogRef`, `DiffOutputRecord`, and summaries.
2. Use the existing PyO3 `call` bridge for `diff` and add a precise Taut log
   reader for `diff.output`.
3. Add a Python argparse command for `gwz diff` covering the same supported
   Tier 0 argument surface as `gwz-cli`.
4. Implement Python manifest formatting, patch assembly, pager/color behavior,
   JSON/JSONL, and exit-code behavior to match `gwz-cli` for the supported
   surface.
5. Keep Python CLI parity tests alongside existing `gwz-py` CLI parity tests.

Pager policy:

- Default to pager only for human patch output on a TTY.
- Never page `--json`, `--jsonl`, `--quiet`, or NUL-terminated output.
- Honor `--no-pager`, `GIT_PAGER`, `PAGER`, and a future GWZ config key.
- Pager failure is a client-side `external_tool_missing` or I/O error, not a
  core diff failure.
- Pager quit or stdout broken pipe closes or abandons the `diff.output` log
  through Taut log semantics so core can stop rendering and release retained
  state.

Color policy:

- Default `auto`: color only when writing human patch output to a terminal or
  a pager that expects ANSI.
- `--color=always` and `--color=never` are client rendering decisions.
- The core output log remains canonical uncolored bytes.

## Implementation phases

### D-1 - Taut log transport spike

Design spike prompt:

```text
We need the minimum generic Taut shape="log" design required to unblock
gwz diff. Do not design a diff-specific file or byte-window API.

Context:
- gwz diff will call a request/response method, diff, which returns metadata
  and a DiffOutputLogRef.
- Patch bytes are then read from diff.output, a Taut shape="log" method whose
  payload is DiffOutputRecord with BYTES support.
- gwz-core may run remotely; gwz-cli and gwz-py must both read the log. The
  PyO3 async/log bridge must support this path.
- Pager quit, stdout broken pipe, and client disconnect must let core stop
  rendering and release retained output state.

Deliver the smallest design that answers:

1. What is the generic log identity?
   - Is log_id opaque?
   - Is it scoped to an operation id/session?
   - What does a client need to pass to read it?

2. What is the cursor model?
   - Record sequence, byte offset, or both?
   - Is a cursor always returned with every batch?
   - Can clients resume without duplicated or skipped BYTES?

3. What does a read request look like?
   - max_records and/or max_bytes.
   - tail/non-tail behavior.
   - close/cancel flag or separate generic close operation.

4. What does a read response look like?
   - records.
   - next cursor.
   - eof / would_block / closed / expired states.
   - error behavior for unknown, expired, or canceled logs.

5. What are retention and backpressure rules?
   - Minimum retention until EOF or close.
   - What happens when a client is slow?
   - What cleanup is required after EOF, close, broken pipe, or disconnect?

6. How are exact bytes represented through Rust, PyO3, and Python?
   - BYTES payloads must preserve NUL bytes and binary patch data.
   - JSON/JSONL representation must be explicit, likely base64.

7. What is the minimum generated-protocol/corpus proof?
   - Rust can write/read DiffOutputRecord.data as BYTES.
   - PyO3 can expose async/tailing log reads to Python.
   - Python can read a binary/NUL-containing output record without using the
     bounded events.subscribe buffer.
   - Close/cancel releases retained server state.

Acceptance:
- A short Taut protocol proposal with message/shape sketches.
- A Rust/PyO3/Python test plan.
- A decision on cursor semantics, EOF/would_block, close/cancel, retention, and
  backpressure.
- No diff-specific replacement for generic log mechanics.
```

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/lib.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/dispatch/mod.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/bridge.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/client.py`

Work:

- Confirm a Taut `shape="log"` method can carry `BYTES`, including NUL bytes,
  through Rust, PyO3, and Python without using the bounded operation-event
  buffer.
- Define the Taut log read contract for cursor/offset precision, tailing, EOF,
  close/cancellation, backpressure, and retention cleanup.
- Use retained/cache-backed output state for v0. The `diff` call creates an
  operation-scoped output cache keyed by normalized request plus per-target input
  fingerprints. The key must change when any input that can affect output
  changes.
  - Request component: normalized operands, resolved target set, repo-local
    pathspecs, output format, diff options, renderer version, and libgit2 version.
  - Immutable Git component: resolved tree/blob/object ids for commit/tree/blob
    sides and snapshot-derived sides.
  - Index component: a content fingerprint of the Git index, preferably the
    index checksum or a hash of the index file bytes, not only mtime.
  - Worktree component: a fingerprint of every candidate path that can affect
    the manifest or output, including file type, executable bit, size, and a
    content hash or Git blob id for changed files. Stat-only fingerprints are a
    performance optimization only if paired with race detection.
  - Output component: output-log format, byte encoding, line prefix/prefix
    options, and any renderer choices that change bytes.
  - Scope: v0 may keep this cache operation-scoped and short-lived. Cross-command
    reuse is not required to unblock `gwz diff`.
- Prove `cached`/`merge_base` request fields cover `--cached`, `--staged`,
  `--merge-base`, and `A...B` without tunneling parsed flags through operands.
- Prove GWZ snapshot operand parsing and per-target commit translation for
  `+snap`, `+base +tip`, `+base..HEAD`, and `+base...main`.
- Prove rename output rendering: manifest-driven renames must include both old
  and new paths and preserve similarity headers. Copy detection is deferred; v0
  rejects `find_copies=true`.
- Define log chunk sizing and retention bounds. These are Taut log/backpressure
  concerns, not diff-specific `max_bytes` fields.
- Define literal-file fast-path detection: files can use a narrow manifest;
  directories, globs, pathspec magic, and ambiguous operands must use full
  manifest discovery.
- Define `DiffManifestMode.any_difference` behavior for `--quiet` and
  `stale_file` output behavior for worktree races.

Acceptance:

- Rust, PyO3, and Python tests can round-trip `DiffOutputRecord.data`
  containing binary bytes and NULs over a Taut log method.
- A `diff.output` sketch can render patch bytes from the planned manifest in
  final output order without requiring one client request per changed file.
- `--cached`, `--staged`, `--merge-base`, and `A...B` have concrete request and
  corpus examples before D0 freezes schema.
- Snapshot operands have concrete request, resolution, and missing-member
  examples before D0 freezes schema.
- Rename manifest entries do not degrade into add/delete patches during output
  rendering.
- `find_copies=true` is rejected with a typed unsupported-option error before D0.
- Log chunking, retention, close/cancellation, and EOF behavior are specified
  before D0 freezes schema.
- Retained/cache-backed output state has a concrete cache-key design, including
  request normalization, per-target input fingerprints, and invalidation rules
  for index/worktree changes.

### D0 - Protocol design and corpus

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/corpus/`

Work:

- Add Taut enums and messages for diff request/options, manifest entries,
  manifest responses, output-log references, output records, and summaries.
- Add `ActionKind.diff`.
- Add service entries for `diff` and `diff.output` with `shape="log"`.
- Regenerate Rust and Python protocol outputs and golden corpus.
- Add compatibility tests for default values and unknown optional fields.
- Add corpus examples for `DiffOutputLogRef`, `DiffOutputRecord` with `BYTES`,
  scoped `DiffParsedTarget`, `DiffManifestMode.any_difference`, `stale_file`
  output records, and comparison forms using `cached`, `merge_base`, and
  `A...B`.
- Add corpus examples for `+snapshot` operands, including `left_snapshot_id` and
  `right_snapshot_id` on scoped `DiffParsedTarget` entries and
  `DiffExcludedTarget` records for targets absent from the snapshot.
- Keep client-only process status out of the core request; do not include
  unsupported algorithm values such as histogram in v0.

Acceptance:

- Protocol generation is deterministic.
- Existing protocol corpus still passes.
- A minimal `DiffRequest` round trips through Taut.
- `DiffOutputRecord` can carry arbitrary bytes, including NUL bytes.
- Manifest entries can be referenced by output records without forcing clients
  to parse `file_id`.
- `DiffParsedTarget` is scope-addressable and can be reused by the output
  renderer without reclassifying raw operands; snapshot-derived sides preserve
  their snapshot ids.
- `DiffManifestResponse.excluded_targets` round trips and can identify a member
  skipped because a snapshot lacks that member.
- Python generated protocol classes round trip the same diff messages.

### D1 - Core diff model and Git backend primitive

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/gitbackend.rs`
- New `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/diff/` or
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_diff.rs`

Work:

- Add core internal structs for `DiffComparison`, `DiffOptions`,
  `DiffRepoScope`, `DiffParsedTarget`, `DiffFileEntry`, `DiffSummary`, and
  patch render requests if generated types are too wire-oriented for internal
  use.
- Add `GitBackend::diff_manifest` and output-rendering primitives that produce
  manifest entries, stats, and patch byte records.
- Implement `Git2Backend` with libgit2 calls mapped from comparison kind.
- Resolve commits/trees/blobs, GWZ snapshot operands, and ambiguous operands per
  target repo.
- Map libgit2 errors to typed `GwzErrorCode` values. Add a new code such as
  `diff_target_not_found` only if existing `git_command_failed`,
  `invalid_request`, and `member_not_found` are too coarse.

Acceptance:

- Backend tests compare `git2` patch output to `git diff` for simple tracked
  modification, add, delete, mode change, rename, and binary file cases.
- `--cached`, `HEAD`, and two-tree cases are covered.
- Unborn `--cached` uses an empty tree.

### D2 - Workspace diff planning

Touchpoints:

- Existing status selection helpers in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/status/status_member.rs`
- Existing stage routing in
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/stage_routing.rs`

Work:

- Reuse `resolve_workspace_root`, manifest validation, lock optional reads, and
  `workspace_ops::resolve_targets` for explicit selection handling.
- Extract path-to-repo partitioning from stage routing into shared workspace
  pathspec routing, while keeping manifest order separate from partitioning.
- Resolve request `workspace_cwd` relative to the workspace root.
- Decide root inclusion through shared target selection first, then apply
  pathspec-intersection semantics. Pathspecs narrow candidates; they do not add
  excluded targets back.
- When snapshot operands are present and selection is implicit, narrow candidate
  targets to materialized active Git members covered by every referenced
  snapshot and record omitted materialized active members in
  `excluded_targets` with `snapshot_missing`, `snapshot_missing_commit`, or
  `root_not_in_snapshot` as appropriate. If selection is explicit, missing
  snapshot records are member-scoped errors that respect partial policy.
- Skip unmaterialized fan-out members, but error on explicit unmaterialized
  member pathspecs.
- Reject unsupported source kinds unless policy says to skip.
- Produce root-first, manifest-order target list.
- Implement root exclusion with the preferred delta post-filter: after root
  diff generation, drop deltas under active member paths, `.gwz/`, and
  `gwz.conf/.tmp/` before manifest/stat/patch selection. Treat libgit2-level
  pruning as a performance optimization only if D2 proves it.

Acceptance:

- No pathspec selects root plus all materialized active members.
- `--target @root` selects only the root repository.
- `--all --no-target @root` selects all materialized active members and excludes
  root.
- `gwz diff +snap` omits root, diffs only members recorded in `snap`, and
  reports root plus any members missing from the snapshot in `excluded_targets`.
- `gwz --target @root diff +snap` is a typed error because v0 snapshots do not
  record a root commit.
- `gwz diff -- gwz-core/src/lib.rs` targets only `gwz-core` with
  `src/lib.rs`.
- `gwz diff -- gwz.conf/gwz.yml` targets only root.
- `gwz diff -- .` at workspace root targets root plus members.
- Parent-relative pathspecs from subdirectories behave like Git.
- Explicit member `A` plus pathspec `B/file` returns a clean empty result when
  both names are valid but non-overlapping.
- Root diff output does not include active member paths, `.gwz/`, or
  `gwz.conf/.tmp/` even when `.git/info/exclude` is stale.

### D3 - Core manifest and output-log handlers

Touchpoints:

- New diff handler module.

Work:

- Implement `handle_diff` for the `diff` planning request.
- Implement the `diff.output` log producer/reader integration.
- `diff` computes manifest entries, per-file stats, rename pairing, binary
  flags, per-repo summaries, and aggregate summary. v0 rejects copy pairing and
  `find_copies=true`.
- The output log renders patch bytes in manifest order and may emit structured
  `file_started`, `file_finished`, `stale_file`, and `diagnostic` records around
  byte records when useful for machine consumers.
- The output renderer validates file identity using `DiffFileEntry`
  scope/status/old_path/new_path plus the scoped `DiffParsedTarget`. `file_id`
  is opaque and is never parsed as a path container.
- Output rendering preserves manifest rename semantics. Rename patches are
  rendered with both paths and similarity enabled. Copy rendering is deferred
  with a clear unsupported-option error for `find_copies=true`.
- Implement `--name-only`, `--name-status`, `--stat`, `--numstat`,
  `--shortstat`, `--summary`, and `--quiet` from the manifest and summary
  without patch-byte log reads. Implement the `--exit-code` process decision
  from the manifest summary while still reading patches when output is not
  quiet.
- Implement `DiffManifestMode.any_difference` for `--quiet` so core can
  short-circuit without full manifest construction or similarity detection.
- Implement Taut log chunk sizing, EOF, close/cancellation, and retention
  cleanup for large outputs.
- Return `stale_file` output records when a worktree race makes a planned
  manifest entry impossible to render.

Acceptance:

- Manifest file order is root first, then members in manifest order.
- Per-file manifest metadata is correct for add, modify, delete, rename, type
  change, binary, and mode change cases. Copy cases are deferred and
  `find_copies=true` is rejected.
- `diff.output` returns workspace-relative patch bytes in final manifest order.
- `diff.output` returns rename headers that agree with the manifest entry.
- Large patch output is resumable according to Taut log cursor/offset semantics.
- Stale manifest entries are reported as non-fatal `stale_file` output records.
- Final summary is correct when the manifest contains multiple repos.
- Errors in one member are member-scoped and respect partial policy.
- `operation.result` remains small and metadata-only.

### D4 - Patch rendering and path rewriting

D4 can run in parallel with D-1/D0 once `DiffOutputRecord` and the generic Taut
log shape are sketched. It does not gate the Taut log design or core comparison
planning, but it gates declaring v0 human patch output complete. If the spike
proves libgit2 cannot rewrite every required header safely, create the
`gwz-diff-render` workspace crate before expanding into word diff or combined
diff work.

Touchpoints:

- `git2::Diff::print`
- `git2::Diff::foreach`

Work:

- First attempt: use libgit2 prefix options for patch, raw, name-only, and
  name-status.
- Build `--stat`, `--numstat`, `--shortstat`, `--summary`, `--name-only`, and
  `--name-status` from manifest entries rather than forwarding
  repo-local formatted stats.
- Spike and document whether libgit2 prefix options cover extended headers for
  rename/copy/mode changes.
- If extended headers are not workspace-relative, split a GWZ patch renderer
  into its own subplan. A hand renderer must reproduce mode lines,
  `/dev/null`, similarity indexes, rename/copy headers, binary patch literals,
  and NUL records; it is not a small fallback.
- Implement line-prefix support after workspace path rendering.
- Preserve NUL records for `-z` modes.

Acceptance:

- Patch output paths are workspace-relative in `diff --git`, `---`, `+++`,
  rename/copy headers, raw output, name-only, and name-status.
- Stat and summary output paths are workspace-relative and match manifest order.
- `--src-prefix`, `--dst-prefix`, and `--no-prefix` keep unified paths.
- Binary patch output remains byte-correct.

### D5 - Client integration for `gwz-cli` and `gwz-py`

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/globalargs.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs`
- New or existing `gwz-cli` diff modules.
- CLI docs under `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/lib.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/dispatch/mod.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/operations.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/bridge.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/client.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/tests/`

Work:

- Add `CommandArgs::Diff`.
- Parse Git-like diff options and revision/pathspec separator `--`; send raw
  ambiguous operands to core while setting structured request fields for parsed
  comparison flags such as `cached` and `merge_base`.
- Preserve leading-plus snapshot operands before `--` for core classification;
  after `--`, `+name` is a literal pathspec.
- Keep unsupported Git options as clear `invalid_request` errors.
- Implement manifest discovery, `diff.output` log reading, and client-side patch
  assembly.
- Treat `stale_file` output records as a non-fatal worktree race: omit the stale
  file in normal human output, and expose the stale marker in machine output.
- Handle log close/cancellation cleanly when the pager exits, stdout breaks, or
  the client no longer needs patch bytes.
- Implement client-side color.
- Implement `--exit-code` and `--quiet`.
- Ensure the PyO3 native dispatch registry and Python bridge can call `diff` and
  read `diff.output` through Taut log semantics.
- Add JSON/JSONL behavior:
  - `--json`: emit `DiffManifestResponse` metadata and summary, not patch bytes.
  - `--jsonl`: emit manifest entries and optional output records only after
    verifying how Taut JSON serializes `BYTES`; document base64 expansion if
    that is the encoding.
- Add `gwz-py` client and CLI support with parity tests against `gwz-cli`; do
  not add PyO3 wrappers for `gwz-cli` parser, pager, or process policy.

Acceptance:

- `gwz diff` writes assembled patch text to stdout.
- `gwz diff | cat` does not launch a pager.
- `gwz diff` on a TTY uses pager unless disabled.
- `gwz diff --quiet --exit-code` emits no patch and exits 1 on differences.
- `gwz --json diff --quiet` returns summary metadata.
- Python `gwz diff` can discover manifests, read/assemble patches, render
  JSON/JSONL, and match `gwz-cli` exit status for covered cases.

### D6 - Documentation

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/CLI.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/commands/diff.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/MachineOutput.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/Protocol.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/Reference.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/README.md`

Work:

- Document supported `git diff` forms and option tiers.
- Document `+<snapshot_id>` operands as a GWZ extension, including root omission,
  missing-member behavior, and machine-readable excluded targets.
- Document path rewriting and root/member selection behavior.
- Document manifest/output-log protocol and JSON/JSONL behavior.
- Document unsupported/deferred Git options.
- Document Python CLI/API parity and any intentional differences.
- Add troubleshooting entries for missing refs in some members and
  unmaterialized selected members.

Acceptance:

- Generated CLI reference includes `diff`.
- Machine output docs include `DiffRequest`, `DiffManifestResponse`,
  `DiffOutputLogRef`, and `DiffOutputRecord` examples.
- Protocol docs explain that `diff` returns metadata and patch bytes are read
  from the Taut `diff.output` log.

## Option parity tiers

Use these tables to split the diff project into independent work packages. "Base
libgit2" means the Rust `git2 0.21` wrapper exposes a direct primitive or option
over the local libgit2 `1.9.4` build; "GWZ work" means core, protocol, client, or
renderer code that still has to be built around that primitive. `CLOL` means
"candidate library or local" feature package; the normalized catalog below is
the source of truth for where a feature lives and how clients access it.

| Tier | Feature | Base libgit2 coverage | GWZ work / project boundary | CLOL refs |
| --- | --- | --- | --- | --- |
| T0 | Default patch output | `DiffFormat::Patch`, diff tree/index/workdir APIs, `Diff::print` and callbacks. | Core comparison planner, workspace target routing, root/member path rewriting, `diff.output` log producer. | CLOL001, CLOL005, CLOL007 |
| T0 | `--cached`, `--staged` | `diff_tree_to_index`; empty-tree handling available through normal tree diff setup. | Parser maps both flags to `cached`; core resolves `HEAD` or empty tree per repo. | CLOL001, CLOL002, CLOL006 |
| T0 | One commit/tree argument | Tree-to-workdir-with-index and tree-to-index primitives exist. | Core per-target revision resolution and unborn/missing-ref diagnostics. | CLOL001, CLOL002 |
| T0 | Two commit/tree arguments | `diff_tree_to_tree`. | Core parser keeps raw operands and resolves them per selected repo. | CLOL001, CLOL002 |
| T0 | `A..B`, `A...B` parsing | libgit2 has object lookup and merge-base primitives, but not GWZ CLI operand parsing. | Operand parser splits `..`/`...` before endpoint resolution; core maps `...` to merge-base-old-side per repo. | CLOL002, CLOL003 |
| T0 | GWZ `+<snapshot_id>` operands | Not a Git/libgit2 feature. | Core reads snapshot artifacts, maps each selected member to its recorded commit, reports excluded targets, preserves snapshot provenance in `DiffParsedTarget`. | CLOL003, CLOL004 |
| T0 | `--merge-base` | merge-base primitive plus normal diff calls. | Parser and comparison mapper; reject unsupported combinations with typed errors. | CLOL002, CLOL003 |
| T0 | Pathspec separator `--` | libgit2 supports repo-local pathspecs. | Client/parser separates operands from pathspecs; core routes workspace paths to root/members, strips member prefixes, rejects escapes. | CLOL005 |
| T0 | `-U<n>`, `--unified=<n>` | `DiffOptions::context_lines`. | CLI/parser/protocol option wiring. | CLOL006 |
| T0 | `--inter-hunk-context=<n>` | `DiffOptions::interhunk_lines`. | CLI/parser/protocol option wiring. | CLOL006 |
| T0 | `--name-only` | `DiffFormat::NameOnly`. | Prefer manifest rendering so paths are workspace-relative and ordered root-first/manifest-order. | CLOL007 |
| T0 | `--name-status` | `DiffFormat::NameStatus`. | Prefer manifest rendering for workspace paths, rename/copy headers, and NUL modes. | CLOL007 |
| T0 | `--raw` | `DiffFormat::Raw`. | Output renderer must rewrite paths and preserve workspace ordering; may use libgit2 raw records as a spike. | CLOL007 |
| T0 | `--stat`, `--numstat`, `--shortstat`, `--summary` | `DiffStatsFormat::{FULL, NUMBER, SHORT, INCLUDE_SUMMARY}`. | Per-repo stats are available, but workspace output needs aggregate summaries, root/member filtering, and rewritten paths. | CLOL007 |
| T0 | `--binary` | `DiffOptions::show_binary`. | Wire option and verify `diff.output` preserves exact bytes. | CLOL001, CLOL006 |
| T0 | `--text` | `DiffOptions::force_text`. | Wire option and test binary/text override behavior. | CLOL006 |
| T0 | `--find-renames`, `--no-renames`, rename threshold | `Diff::find_similar`; `DiffFindOptions::renames`, `rename_threshold`. | Core option wiring, manifest similarity metadata, output renderer validation for rename headers. | CLOL008 |
| T0 | `-w`, `-b`, `--ignore-space-at-eol`, `--ignore-blank-lines` | `DiffOptions::ignore_whitespace*`, `ignore_blank_lines`. | CLI/protocol mapping and parity tests. | CLOL006 |
| T0 | `--src-prefix`, `--dst-prefix`, `--no-prefix`, `--line-prefix` | `old_prefix`/`new_prefix` cover src/dst prefix only. | Workspace-aware prefix composition; `--no-prefix` still keeps member path; `--line-prefix` is a renderer/log transform. | CLOL006, CLOL007 |
| T0 | `--exit-code` | Not a diff primitive. | Client maps `DiffSummary.has_differences` to process status. | CLOL009 |
| T0 | `--quiet` | libgit2 can cheaply detect whether deltas exist. | Core `any_difference` mode; client suppresses output and applies exit code. | CLOL001, CLOL009 |
| T1 | `--diff-filter` | Delta status is exposed on `DiffDelta`. | Filter manifest/output records after diff generation, before summaries/output. | CLOL010 |
| T1 | `--patch-with-raw` | Patch and raw formats exist separately. | Canonical output composer emits patch plus raw in Git-compatible order across repos. | CLOL011 |
| T1 | `--patch-with-stat` | Patch and stats exist separately. | Canonical output composer emits patch plus workspace stat summary. | CLOL011 |
| T1 | `--full-index`, `--abbrev` | `DiffOptions::id_abbrev`; full index can be represented by full oid length. | Option mapping plus renderer tests for headers and binary patches. | CLOL006, CLOL007 |
| T1 | Copy detection `-C` | `DiffFindOptions::copies`, `copies_from_unmodified`, `copy_threshold`. | Deferred. v0 rejects `find_copies=true`; later work must preserve manifest/render source sets and output semantics. | CLOL008 |
| T1 | Rename/copy limits | `DiffFindOptions::rename_limit`. | CLI/protocol mapping and policy for excessive rename/copy work. | CLOL008 |
| T1 | `--ignore-submodules` | `DiffOptions::ignore_submodules`. | Wire for real Git submodules; document that GWZ members are not Git submodules and root member exclusion still applies. | CLOL006 |
| T1 | `--check` whitespace/conflict-marker diagnostics | No direct libgit2 porcelain equivalent. | Independent scanner over changed lines or rendered patch records; report whitespace and conflict-marker diagnostics without pager/color concerns. | CLOL012, CLOL022 |
| T1 | Better Git pathspec magic | libgit2 has pathspec support, but GWZ needs workspace-level routing before repo-local matching. | Independent workspace pathspec package: parse magic, route to root/members, then pass repo-local specs to libgit2. | CLOL013, CLOL023 |
| T2 | Word diff and color words | No direct libgit2 word-diff renderer. | Independent presentation project over patch lines/blob pairs; should live outside core canonical bytes unless output mode requests it. | CLOL014, CLOL024 |
| T2 | Moved-line coloring | No direct libgit2 moved-line coloring. | Independent output-decoration project that indexes removed/added lines after canonical diff generation. | CLOL015, CLOL024 |
| T2 | External diff helpers | libgit2 does not run Git external diff helpers. | Client-side or explicitly sandboxed adapter only; core/server must remain safe by default. | CLOL016 |
| T2 | Textconv | libgit2 does not apply Git textconv filters for diff output. | Separate filter pipeline with cache/safety policy before diff rendering. | CLOL016 |
| T2 | Pickaxe `-S`, `-G`, `--find-object` | No direct libgit2 diffcore pickaxe. | Independent diffcore filter over blobs, deltas, and regex/string searches. | CLOL017, CLOL022 |
| T2 | Order file and rotate/skip | Not a libgit2 diff primitive. | Manifest/output ordering transform after root/member planning. | CLOL018, CLOL025 |
| T2 | Combined merge diff | libgit2 exposes merge primitives but not Git combined diff porcelain output. | Separate merge-output renderer comparing multiple parents and reproducing combined headers/hunks. | CLOL019 |
| T2 | Blob-vs-blob | `git2::Patch::from_blobs`, `from_blob_and_buffer`, and `from_buffers` exist. | GWZ needs operand classification and cross-repo object routing; can be independent once tree/worktree forms work. | CLOL020 |
| T2 | `--no-index` | Not a repository diff operation. | General source-to-source diff engine with adapters; local filesystem is one adapter, provider-backed remote GWZ content is another future adapter. | CLOL021, CLOL024 |

### CLOL catalog

CLOL rows are feature-package boundaries, not release commitments. For
git2-backed rows, `gwz-cli` and `gwz-py` access the behavior through the Taut
protocol unless the row explicitly says the feature is client-local. "Boundary
decision" should name one of: existing core/client module, new workspace crate,
external dependency behind an adapter, or separate `gwz-*` repository. A
separate repository is the exception and needs an ownership/release reason; most
"build" items should start as a module or workspace crate.

| CLOL | Package / feature | Git2 API shape | Runtime owner and client access | Boundary decision | Uniformity and analysis notes |
| --- | --- | --- | --- | --- | --- |
| CLOL001 | Core git2 diff generation | Embedded diff APIs: `Repository::diff_tree_to_tree`, `diff_tree_to_index`, `diff_index_to_workdir`, `diff_tree_to_workdir_with_index`, plus `Diff::print`, `Diff::foreach`, `Diff::stats`, `DiffDelta`, and `DiffLine`. | `gwz-core` only. Clients access results through `diff` metadata and `diff.output`; `gwz-cli`/`gwz-py` must not call git2 directly for workspace diff. | Existing `gwz-core` backend trait and `Git2Backend`; no new repo. | Centralizes repository access, remote-core compatibility, and byte fidelity. |
| CLOL002 | Git object and graph resolution | Separate repository APIs, not embedded in diff: `revparse`, `revparse_single`, `find_object`, `find_commit`, `merge_base`, `merge_bases`, and prefix object lookup. | `gwz-core` only; resolved ids are surfaced in `DiffParsedTarget`. | Existing `gwz-core` diff/operand module. | Keeps ambiguous refs and per-member resolution server-side; clients send raw operands. |
| CLOL003 | GWZ diff operand parser | Local parser for `A..B`, `A...B`, `+snapshot`, and raw endpoint classification before git2 lookup. | `gwz-core` owns semantic parsing; clients only separate options and `--` pathspecs. | Start as `gwz-core/src/diff/operands.rs`; split to crate only if reused by other commands. | Prevents Rust/Python parser drift for snapshot/range behavior. |
| CLOL004 | GWZ snapshot operand resolver | Not git2. Uses `artifact::read_snapshot`, manifest ids, and snapshot member commits. | `gwz-core`; exposed through `DiffParsedTarget.left_snapshot_id`, `right_snapshot_id`, and `excluded_targets`. | Existing artifact module plus diff planner. | Required because each selected member resolves the same `+name` to a different commit. |
| CLOL005 | Workspace target and path routing | libgit2 pathspecs apply only after routing; GWZ must route workspace paths first. | `gwz-core`; clients pass `RequestMeta.selection`, `workspace_cwd`, and raw pathspecs. | Shared core routing module, likely extracted from stage routing. | Enforces `@root`, `@all`, member selection, root exclusion, and path escape policy uniformly. |
| CLOL006 | Git2 diff option wiring | Embedded `DiffOptions` / `DiffFindOptions` setters: context, interhunk, text/binary, whitespace, abbrev, prefixes, ignore-submodules, etc. | `gwz-core`; clients send structured `DiffOptions` over Taut. | Existing protocol/options mapper in `gwz-core`. | Option values belong in protocol so Rust/Python clients stay aligned. |
| CLOL007 | Canonical GWZ manifest and output renderer | Uses `DiffDelta`, `DiffLine`, `DiffStats`, and maybe libgit2 print formats as inputs, but workspace paths/order are GWZ-owned. | `gwz-core`; clients read canonical uncolored bytes from `diff.output`. | Start in `gwz-core/src/diff/render.rs`; consider `gwz-diff-render` crate if word/source-diff work later needs it. | Avoids duplicating path rewriting and stat aggregation in Python. |
| CLOL008 | Similarity detection | Embedded post-process: `Diff::find_similar` with `DiffFindOptions::{renames,copies,copies_from_unmodified,rename_threshold,copy_threshold,rename_limit}`. | `gwz-core`; clients send structured options. | Existing `gwz-core` backend and render validation. | Copy support remains gated until render/source-set fidelity is proven. |
| CLOL009 | Client process behavior | No git2 API. | Client-side: pager, broken pipe, stdout/stderr, `--exit-code`, `--quiet` process status. | No new crate. Implement polished behavior inside `gwz-cli`; implement matching behavior independently in `gwz-py` as an API and bridge completeness proof. | Extraction case: shared parser/process policy could reduce Rust/Python drift. Decision: reject for v0 because it creates a supported API/PyO3 burden before the core/Taut API and PyO3 async/log bridge are proven. |
| CLOL010 | Diff status filter | Uses `DiffDelta.status()` / manifest statuses; no separate git2 filter API required. | Prefer `gwz-core` so JSON, patch, and summaries agree. | Small local core filter module. | Must run before aggregate summaries and output-log rendering. |
| CLOL011 | Combined canonical output forms | Builds on CLOL007 plus raw/stat renderers. | Prefer `gwz-core` output modes so `gwz-cli` and `gwz-py` consume identical bytes. | Extend renderer module; no new repo. | Keeps `--patch-with-raw` and `--patch-with-stat` deterministic across clients. |
| CLOL012 | `--check` diagnostics scanner | No direct git2 porcelain API. Scanner consumes changed lines or rendered patch records. | Prefer `gwz-core` for uniform machine diagnostics; clients only render. | Independent crate candidate `gwz-diff-check` if it grows beyond a small module. | Needs byte-safe scanning, conflict-marker checks, whitespace policy, and machine-output schema. |
| CLOL013 | Workspace Git pathspec magic | libgit2 supports repo-local pathspec matching, but not GWZ workspace routing. | `gwz-core`; clients pass raw pathspecs. | Independent crate candidate `gwz-pathspec` if Git magic coverage becomes broad. | Needs escaping, exclusions, `:(top)`, cwd semantics, and target intersection tests. |
| CLOL014 | Word diff / color words | No direct libgit2 word-diff renderer. | Prefer shared renderer package. Core can produce non-colored word-diff output; clients apply terminal color if requested. | Independent crate candidate `gwz-diff-words`, or adapter over CLOL024. | Requires a uniform Rust/Python access path: Taut output from core or PyO3 wrapper, not two separate algorithms. |
| CLOL015 | Moved-line coloring | No direct libgit2 moved-line coloring. | Client presentation unless a future core output mode emits moved-line annotations. | Extend `gwz-diff-render` / `gwz-diff-words` if created. | Needs explicit color-policy split: detection can be shared, ANSI emission stays client-side. |
| CLOL016 | External diff and textconv adapters | libgit2 does not execute Git external diff/textconv helpers. | Client/local only by default; remote `gwz-core` should not run arbitrary commands without an explicit capability and sandbox policy. | Separate adapter crate if ever enabled; no v0 support. | Security, determinism, caching, and remote execution policy are the blocking dimensions. |
| CLOL017 | Pickaxe filters | No direct libgit2 diffcore pickaxe API. Uses blobs/deltas plus string/regex matching. | Prefer `gwz-core` so target filtering, JSON, and output agree. | Independent crate candidate `gwz-diff-pickaxe` if more than simple `-S` search. | Needs blob loading cost limits, regex safety, binary handling, and `--pickaxe-all` policy later. |
| CLOL018 | Orderfile / rotate / skip | Not a libgit2 primitive. | `gwz-core` manifest/output ordering transform after target planning. | Small local module; crate only if orderfile glob semantics become large. | Must preserve root/member grouping unless Git parity explicitly overrides it. |
| CLOL019 | Combined merge diff renderer | libgit2 has merge/object primitives, not combined diff porcelain output. | `gwz-core` if supported, because output is canonical. | Separate renderer module or future `gwz-diff-combined` crate. | Large independent project: headers, multi-parent hunks, stats, and JSON shape. |
| CLOL020 | Blob-vs-blob diff | Separate patch APIs: `git2::Patch::from_blobs`, `from_blob_and_buffer`, `from_buffers`, and `Patch::print`. | `gwz-core`; expose as an explicit comparison kind if operand routing supports it. | Existing backend plus operand classifier; no new repo. | Underlying primitive exists; hard part is cross-repo object ownership and UX. |
| CLOL021 | Source-to-source content diff, including `--no-index` local paths | Not repository/libgit2 diff. | Adapter-driven. The diff engine consumes a source provider; local filesystem is only one provider. Cross-machine/provider coordination is a separate project and is assumed to expose file data through Taut messages/protocols. | Future workspace crate `gwz-content-diff`, not `gwz-fsdiff`. | Needs source manifests, stable source snapshots, lazy blob/range reads, output-log streaming, provider capability checks, and Git `--no-index` goldens for the local adapter. |
| CLOL022 | Byte scanning helper crates | Optional helper dependencies, not features: `bstr`, `memchr`, `regex`. | Used inside core scanner/filter crates when needed. | Library dependencies only. | Add only when they reduce complexity and pass license/supply-chain review. |
| CLOL023 | Git pathspec library candidate | Optional dependency: `gix-pathspec`. | Used by CLOL013 if it can parse enough Git pathspec magic without fighting GWZ routing. | Library dependency or adapter crate. | Must be proven against Git pathspec goldens before adoption. |
| CLOL024 | Text-diff library candidates | Optional dependencies: `similar`, `dissimilar`, `imara-diff`. | Used by CLOL014/CLOL015/CLOL021 if selected. | Library dependency behind a GWZ adapter crate. | Evaluate output fidelity, streaming behavior, performance, Unicode/byte behavior, and license. |
| CLOL025 | Pattern/order helper candidate | Optional dependency: `globset` or local matcher. | Used by CLOL018 for orderfile patterns if Git-compatible matching is needed. | Library dependency only unless ordering grows. | Must not reorder root/member groups accidentally. |

Additional dimensions to analyze for each CLOL package:

- Execution authority: core/server, client/local, or shared helper.
- Wire shape: Taut request metadata, `diff.output` bytes, machine diagnostics, or client-only behavior.
- Cross-client uniformity: generated protocol, shared Rust crate with PyO3 binding, or independent Rust/Python implementations with golden parity tests.
- Security boundary: arbitrary command execution, filesystem reads outside the workspace, regex/pathspec denial-of-service, and remote-core policy.
- Byte fidelity: binary data, NUL bytes, encoding assumptions, JSON/JSONL representation, and ANSI/color ownership.
- Determinism: output ordering, libgit2-version drift, locale/case behavior, and platform path rules.
- Performance/state: blob loading cost, rename/copy limits, output-log retention, backpressure, and cancellation.
- Packaging: existing `gwz-core` module, new workspace crate, or a separate `gwz-*` repository only when ownership/reuse justifies it.

### Recommended CLOL resolution proposal

The recommendation is to make repository semantics centralized and keep client
process behavior scoped to `gwz-cli` for v0. Do not create separate project
repositories for v0 diff. Start with modules and workspace crates inside
`gwz-dev`; split a crate into its own `gwz-*` repository only after it has
independent consumers, release cadence, or ownership.

Core rule: if a feature affects which repository objects are compared, which
paths are included, which files appear in JSON, or which bytes appear in the
canonical patch, it belongs in `gwz-core` and is reached by clients through Taut.
Client rule: if a feature controls local process behavior, terminal color, or
pager interaction, it belongs in `gwz-cli` for the primary implementation and is
reimplemented in `gwz-py` for parity. Arbitrary local filesystem paths are not
the general abstraction; they are one adapter for a broader source-to-source diff
engine. Python uses Taut for core behavior and independently matches the
supported `gwz-cli diff` surface; this plan does not create a shared Rust/Python
client-behavior API.

Recommended package boundaries:

| Boundary | CLOL coverage | Recommendation | Taut/protocol burden |
| --- | --- | --- | --- |
| `gwz-core/src/diff/` modules | CLOL001, CLOL002, CLOL003, CLOL004, CLOL005, CLOL006, CLOL008, CLOL010, CLOL011, CLOL018, CLOL020 | Implement v0 and near-follow-up repository diff behavior inside core. Keep operand parsing, snapshot resolution, target routing, git2 option mapping, manifest filtering, ordering, blob-vs-blob routing, and output composition server-side. | Add structured `DiffRequest`, `DiffOptions`, `DiffManifestResponse`, `DiffParsedTarget`, `DiffExcludedTarget`, `DiffOutputLogRef`, and `DiffOutputRecord`; use `diff` plus `diff.output` only. |
| `gwz-core/src/diff/render.rs`, promoted later to `gwz-diff-render` workspace crate if needed | CLOL007, CLOL011, CLOL014, CLOL015, CLOL019 | Start patch/stat/raw/name renderers in core. Promote to a workspace crate only if D4 proves a hand renderer is required or if word/moved/combined diff need the same rendering model. Do not make this a separate repository in v0. | No extra service methods. Add output-format enum values only when a format is supported. Patch bytes still flow through the Taut log. |
| `gwz-cli` internal diff modules plus independent `gwz-py` parity | CLOL009 | Keep Git-like argument parsing, output-mode selection, pager/color policy, stdout/broken-pipe handling, and exit-code policy inside `gwz-cli`. Do not extract `gwz-client` in v0. `gwz-py` implements matching supported behavior independently to prove the API surface and PyO3 async/log bridge. | No core schema for pager/color/exit code. The only protocol interaction is calling `diff`, reading `diff.output`, and closing/abandoning the Taut log when output is no longer needed. Parity failures should usually drive core/Taut or PyO3 bridge fixes, not private client workarounds. |
| New `gwz-diff-check` workspace crate when `--check` leaves spike stage | CLOL012, CLOL022 | Implement as a reusable scanner over canonical diff records or changed lines. Core invokes it so Rust and Python clients receive identical diagnostics. Start as a core module if the first version is small. | Add structured diagnostics either as `DiffOutputRecordKind.diagnostic` for log-associated output or a future `DiffDiagnostic` list on the manifest for metadata-only `--check`. |
| New `gwz-pathspec` workspace crate when Git pathspec magic expands | CLOL013, CLOL023 | Keep simple literal routing in core for v0. When adding broad Git pathspec magic, extract workspace pathspec parsing/routing into a crate used by core. Evaluate `gix-pathspec` behind an adapter instead of leaking that dependency into command code. | No new service method. Existing `explicit_pathspecs`, `workspace_cwd`, and `RequestMeta.selection` remain the wire shape. |
| New `gwz-diff-words` workspace crate for word diff and moved-line detection | CLOL014, CLOL015, CLOL024 | Treat this as a deferred presentation engine. It should consume canonical core diff data and return deterministic word/moved annotations. Prefer `similar`, `dissimilar`, or `imara-diff` behind a GWZ adapter after goldens. Color emission stays client-side. | If word diff output becomes a canonical output mode, add explicit `DiffOutputFormat` values and stream through `diff.output`. If it is only color decoration, no core schema change beyond optional annotations. |
| New `gwz-diff-pickaxe` workspace crate if `-S`/`-G` grows beyond a small filter | CLOL017, CLOL022 | Core-side diffcore filter over blobs and deltas. Start small inside core for `-S`; extract once regex, binary handling, or `--pickaxe-all` appear. | Add structured fields to `DiffOptions` for string/regex/object search when implemented. Do not tunnel raw `git diff` argv. |
| Future `gwz-diff-combined` workspace crate | CLOL019 | Separate deferred renderer for combined merge diff. It is large enough to be independent once prioritized, but it should still be called by core because output and JSON shape must be canonical. | Add comparison kind and output schema only when implemented. Do not overload v0 tree-vs-tree fields for multi-parent cases. |
| Future `gwz-content-diff` workspace crate | CLOL021, CLOL024 | Adapter-driven diff over two named content sources. Local filesystem `--no-index` is one adapter; local GWZ workspace, remote GWZ core, snapshot/artifact, archive, object-store, or coordinator-backed providers can be additional adapters. The engine compares source manifests and lazily reads content streams or ranges rather than assuming paths are local. | Define the source-provider protocol the diff engine consumes: source manifests, stable source ids/generation ids, content hashes, range/blob reads, provider capabilities, and a log-shaped diff output. The coordinator/provider implementation is a separate project. |
| Future `gwz-diff-ext` adapter crate, disabled by default | CLOL016 | External diff helpers and textconv are local-command execution features. Keep them out of remote core unless a sandboxed capability model is added. | No v0 protocol. Any future remote support needs explicit capability discovery, sandbox policy, and audit-friendly diagnostics. |
| Third-party helper dependencies only | CLOL022, CLOL023, CLOL024, CLOL025 | Use `bstr`, `memchr`, `regex`, `gix-pathspec`, `similar`, `dissimilar`, `imara-diff`, or `globset` only behind GWZ-owned adapters. The adapter owns byte/Unicode policy, license review, performance limits, and goldens. | No direct wire exposure of third-party library concepts. |

Future remote source diff use case:

Consider two hosts, each running `gwz-core` and monitoring one or more
workspaces. A user should eventually be able to compare workspace A on host 1
with workspace B on host 2 without mounting either filesystem locally. That is
not `fsdiff`; it is source-to-source diff over protocol-backed content sources.
The coordinator/orchestrator that connects those machines is a separate project.
This diff plan should assume a source provider exists and exposes file data
through Taut messages/protocols.

The recommended shape is:

1. A source provider exposes two source adapters to the diff engine.
2. Each source adapter exposes a stable source manifest: logical path, type, mode,
   size, content hash when known, source generation id, and an opaque content
   token.
3. The diff engine compares manifests first and reads content lazily only for
   paths that need hunk generation, rename detection, word diff, or binary
   classification.
4. Remote/provider-backed adapters read content through Taut APIs that support
   range or chunked reads and source-generation validation. A stale generation
   is a typed source race, not silent content drift.
5. The diff response uses the same log-shaped output model as repository diff:
   metadata first, byte-bearing records through a Taut log, and close/cancel
   semantics when the consumer stops reading.

This future work needs a source-provider protocol, not just a local file API.
Likely source endpoints:

- Local filesystem source for Git-compatible `--no-index`.
- Local `gwz-core` workspace source.
- Remote `gwz-core` workspace source over Taut.
- GWZ snapshot/artifact source.
- Archive/object-store source if later useful.
- Coordinator-backed source provider from the separate orchestration project.

Out of scope here: deciding where the coordinator runs, how it discovers hosts,
how credentials are delegated, or how remote authorization and audit are
implemented. The diff engine should consume source adapters rather than local
paths or coordinator internals.

Recommended Taut burden:

- Keep exactly one planning method, `diff`, and exactly one byte-bearing log
  method, `diff.output`, for repository diff output.
- Mature Taut `shape="log"` generically with record sequence or byte cursor,
  `max_records` or `max_bytes`, EOF state, tailing, close/cancellation, and
  retention/backpressure rules. These are Taut concepts, not diff-specific
  request fields.
- Extend the diff schema only for semantic choices that every client must agree
  on: comparison form, target selection, options that affect libgit2/core
  output, output format, diagnostics, and unsupported-feature errors.
- Keep pager, terminal color, and process status outside `gwz-core` protocol.
  Treat `--no-index` as a local-filesystem adapter for the future generic
  source-to-source diff engine, not as the whole engine.
- Add new output formats and option fields only when the corresponding CLOL
  package is implemented. Unsupported options are typed hard errors, not silent
  client fallbacks.
- For future source-to-source diff, add source-provider APIs deliberately
  instead of overloading repository diff: source manifest discovery,
  content-token reads, range/chunk reads, generation validation, source
  capability discovery, and log-shaped output. Those APIs should work for local
  filesystem, local core, and provider/coordinator-backed adapters.

If Taut `shape="log"` needs explicit reusable messages, define them once for all
log-shaped methods rather than adding `DiffOutputReadRequest` fields:

```python
TautLogCursor=Msg(
    sequence=F(1, INT, optional=True),
    byte_offset=F(2, INT, optional=True))

TautLogReadRequest=Msg(
    log_id=F(1, STR),
    cursor=F(2, Ref.TautLogCursor, optional=True),
    max_records=F(3, INT, optional=True),
    max_bytes=F(4, INT, optional=True),
    tail=F(5, BOOL, optional=True),
    close=F(6, BOOL, optional=True))

TautLogReadState=Enum(
    records=0,
    eof=1,
    would_block=2,
    closed=3,
    expired=4)
```

The exact names can follow Taut conventions, but the important boundary is that
cursor precision, read sizing, tailing, EOF, close/cancellation, and expiry are
defined on the generic log shape. Diff contributes only `DiffOutputRecord` as
the log record payload.

Recommended sequencing:

1. Keep CLOL009 in `gwz-cli` internal modules plus independent `gwz-py` parity
   work. Do not build `gwz-client` or PyO3 wrappers for CLI parser/process
   policy in v0.
2. Keep v0 repository diff in `gwz-core/src/diff/`. That includes all git2 calls,
   operand parsing, snapshot resolution, routing, filtering, and canonical
   output bytes.
3. During D4, decide whether path rewriting can safely use libgit2 formatting.
   If not, create `gwz-diff-render` as a workspace crate before adding word diff,
   moved-line coloring, or combined diff.
4. Treat `gwz-diff-check`, `gwz-pathspec`, `gwz-diff-pickaxe`,
   `gwz-diff-words`, `gwz-diff-combined`, and `gwz-content-diff` as follow-up
   workspace crates with explicit entry criteria. None should become a separate
   project repository until it has independent release pressure.
5. For every new CLOL crate, add a parity contract first: core caller, client or
   PyO3 access path, fixture goldens against Git where applicable, byte/Unicode
   policy, and performance limits.

## Test strategy

Core backend parity tests:

- Compare libgit2 output with `git diff` for one isolated repository.
- Cover modification, add, delete, rename, type change, executable bit change,
  binary file, and pathspec filtering.
- Cover `--cached`, `HEAD`, two-tree, and unborn staged changes.
- Cover request schema mapping for `--cached`, `--staged`, `--merge-base`, and
  `A...B`.
- Cover `+snapshot` endpoint resolution separately from libgit2 revision
  parsing.

Workspace integration tests:

- Root-only diff.
- Member-only diff.
- Root plus multiple members in deterministic order.
- Selection by member id and member path.
- Snapshot operand diff against current worktree: `gwz diff +snap`.
- Snapshot-to-snapshot and snapshot-to-ref forms: `+base +tip`, `+base..HEAD`,
  and `+base...main`.
- Snapshot operands narrow implicit target sets to covered members and reject
  explicit missing members or explicit `@root`.
- Snapshot operands report implicit exclusions for members absent from the
  snapshot, including members added after the snapshot was captured.
- Cwd-relative pathspecs from root, member, and nested subdirectory.
- Missing ref in one member with and without partial policy.
- Two members resolving the same operand differently, with scoped
  `DiffParsedTarget` entries for each member.
- Unmaterialized fan-out member skipped; explicit unmaterialized member errors.
- Literal explicit file pathspecs route to a narrow manifest and output log.
- Directory pathspecs, globs, and ambiguous operands use full manifest
  discovery.

Manifest and patch tests:

- The manifest reports status, old/new paths, modes, rename similarity,
  binary flag, per-file stats, repo scope, and aggregate summary. Copy detection
  is covered by an unsupported-option test for `find_copies=true`.
- `DiffRepoSummary.files_changed` and `files_manifested` are stable and
  documented when root/member filtering removes deltas.
- Manifest order is root first, then members in manifest order.
- `--name-only`, `--name-status`, `--stat`, `--numstat`, `--shortstat`,
  `--summary`, and `--quiet` need no patch-byte log reads.
- `--quiet` uses an early-exit any-difference manifest mode and does not run
  rename similarity.
- `diff.output` returns workspace-relative patch bytes in manifest order.
- Manifest-driven rename output preserves `similarity index`, `rename from`,
  and `rename to` headers.
- Copy detection is rejected as an unsupported Tier 1 option; it is never
  silently rendered as an add.
- `diff.output` preserves requested order and respects Taut log chunking and
  retention bounds.
- A worktree edit before lazy output rendering returns a `stale_file` output
  record rather than a fatal error or silently different patch.
- Patch bytes survive NUL records and binary patch data.
- Final `DiffSummary` matches accumulated manifest entries.

CLI/client tests:

- Pager is not launched for pipes, JSON, JSONL, quiet, or no-pager.
- Pager is launched for TTY human patch output.
- Color auto/always/never is client-side.
- `--exit-code` returns 1 on differences and 0 when clean.
- Plain `gwz diff` returns 0 with differences.
- Broken pipe or pager quit closes or abandons the Taut output log.
- Stale file output records are omitted or surfaced consistently according to
  output mode.
- `gwz-cli` assembles manifest-ordered patches deterministically.
- `gwz-cli` parser preserves `+snapshot` before `--` and literal `+path` after
  `--`.

Python client tests:

- PyO3/Python can read `DiffOutputRecord` values including BYTES payloads over
  `diff.output` without using the bounded `subscribe_events` buffer.
- Python `gwz diff` covers the same supported Tier 0 parser cases as `gwz-cli`.
- Python and Rust clients agree on generated protocol messages, `diff.output`
  log reading, human output bytes where color is disabled, JSON summary shape,
  JSONL manifest/output records, `--quiet`, and `--exit-code` for covered
  fixtures. These tests prove both the Taut surface and the PyO3 async/log
  interface framework.

Golden output tests:

- Stabilize a small fixture workspace with root plus two members.
- Compare selected `gwz diff` outputs against checked-in goldens.
- Keep goldens focused on path headers and sequencing rather than every hunk
  detail from libgit2 if libgit2 version drift can legitimately change hunk
  boundaries.

## Open decisions

- What exact Taut log chunk size, retention window, and backpressure policy
  should govern `diff.output`?
- What exact retained/cache-backed output key should v0 use for mutable inputs?
  It must include normalized request data and per-target input fingerprints, and
  it must change when any file/index/object that can affect output changes.
- Should root exclusion later add lower-level libgit2 pruning for performance?
  Correctness uses the GWZ delta post-filter in v0.
- Should a future snapshot schema capture the workspace root commit so
  `+snapshot` can address `@root` as well as members? v0 snapshots are
  member-only.
- For future source-to-source diff, what exact source-provider Taut messages
  should `gwz-content-diff` consume? The coordinator/orchestrator is a separate
  project; this plan only needs the provider contract.
- If maintaining independent `gwz-cli`/`gwz-py` CLOL009 parity becomes too
  expensive later, should CLOL009 be reopened as a shared client API? v0 says no.

## Definition of done

`gwz diff` is ready when:

- It handles the Tier 0 forms above across root and member repositories.
- `+<snapshot_id>` operands resolve to per-member snapshot commits before
  libgit2 diff generation.
- Output paths are workspace-relative and deterministic.
- `diff` returns a complete changed-file manifest and summary without patch
  bytes.
- `diff.output` returns exact patch bytes through Taut log records with precise
  cursor/offset, EOF, and close/cancellation behavior.
- Core contains no pager or terminal presentation logic.
- `gwz-cli` implements pager, color, parser, output assembly, and exit status.
- `gwz-py` matches `gwz-cli diff` behavior for the supported surface, proving the
  protocol, manifest, output-log, JSON/JSONL, pager/color policy, exit-code paths,
  and PyO3 async/log bridge can be implemented from the public API.
- JSON/JSONL behavior is documented and tested.
- The protocol corpus includes diff manifest, output-log, output-record, and
  summary examples.
