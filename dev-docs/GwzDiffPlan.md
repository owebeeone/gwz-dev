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
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/gitbackend.rs`
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
Those belong in client surfaces. `gwz-cli` may share Rust client-side code, but
`gwz-py` is a first-class Python client with its own CLI and PyO3 bridge, so it
must receive equivalent protocol, manifest/file-patch, parser, rendering, and
exit-code work rather than relying on Rust crate reuse.

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
  `events.subscribe`. Current GWZ operations do not expose a generic
  operation-output log payload.
- `gwz forall` is intentionally CLI-local. `gwz diff` must not follow that
  model because repository object access and revision resolution belong in
  `gwz-core`; terminal presentation belongs in the client.
- Taut's `log` shape is the intended append-only/read-from-offset/tail
  abstraction for future whole-operation output feeds, but v0 `gwz diff` should
  not depend on maturing that transport. It can fit existing request/response
  mechanics by splitting diff into manifest discovery and bounded per-file
  patch requests.
- `gwz-py` links `gwz-core` through PyO3 and currently exposes `call`,
  `submit`, `subscribe_events`, `wait_events`, `operation_result`, and
  `try_operation_result`. Its event subscription path is bounded and can reset,
  so v0 diff patch bytes should use ordinary request/response calls instead.

## Goals

- Match `git diff` semantics where they map cleanly to a unified GWZ workspace.
- Use libgit2 for the core diff engine whenever possible.
- Render one deterministic workspace-relative diff across the root repository
  and selected members.
- Keep `gwz-core` headless and server-safe.
- Return changed-file manifests and bounded per-file patch bytes without
  forcing the final response to hold a whole-workspace patch.
- Provide client-side support that can consume GWZ core protocol, render diff
  output, apply color policy, launch a pager, and compute CLI exit codes in
  both `gwz-cli` and `gwz-py`.
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
workspace artifacts. Short-lived manifest/file patch cache entries are allowed
runtime state if a stateful optimization is later added, but v0 should not
require persistent server-side diff state.

### AD2 - Diff is computed per repository and projected as one manifest

The core operation runs the requested Git diff independently in:

1. The root repository, when included.
2. Each selected materialized Git member, in manifest order.

Each repo produces a normal libgit2 `Diff`. Phase 1 projects those per-repo
diffs into a single workspace-relative changed-file manifest. Phase 2 renders
patch bytes for one manifest file, or an explicit small batch of files, with
paths rewritten relative to the workspace root.

This avoids inventing a virtual Git repository while preserving the user model
of one unified workspace.

### AD3 - Core emits canonical uncolored output

`gwz-core` should emit canonical, uncolored diff content plus structured
summary metadata. Color and pager decisions are presentation concerns.

The client library may colorize patch lines when the selected output mode is a
terminal mode. It should not ask core to embed ANSI escapes by default.

### AD4 - v0 diff is two-phase and file-granular

Whole-workspace patch streaming is not the v0 backbone. The v0 protocol is:

1. Phase 1, `diff`: discover the changed-file manifest and aggregate summary.
   This returns file entries with workspace paths, status, rename pairing, copy
   pairing only when copy detection is explicitly enabled/proven, binary flag,
   per-file stat counts when available, repo scope, and per-repo operand
   classification. It returns no patch bytes.
2. Phase 2, `diff.file`: render the patch for one file from the manifest, or
   for one explicit literal file path.
3. Phase 2 batch, `diff.files`: optional bounded batching of several files to
   reduce remote round trips.

When the user explicitly supplies literal file pathspecs, the client may skip
Phase 1 and call `diff.file`/`diff.files` directly after workspace path routing.
Discovery still runs for bare `gwz diff`, directory pathspecs, globs, or any
request where the changed set is unknown.

This avoids requiring a new non-lossy whole-diff log before `gwz diff` can
ship. The only offset-addressed byte path in v0 is for one oversized file patch
at a time, using `DiffFileRequest.from_offset` and `max_bytes`.

Stateless Phase 2 widens the normal worktree race between discovery and
rendering: a user can edit or revert a file after the manifest is returned and
before the file patch is fetched. Core should treat "manifest entry no longer
matches current diff" as a stale-entry condition, not as a fatal protocol error.
Clients should omit that file's patch or show a concise stale notice according
to output mode. A future stateful retained-diff mode can close this snapshot
window for remote or review-UI clients.

### AD5 - Final operation results stay metadata-only

`operation.result` should not carry whole-workspace patch text. It may carry
final status, member outcomes, errors, and small summaries. Patch bytes belong
to bounded `diff.file`/`diff.files` responses.

### AD6 - Client behavior lives outside core, but Rust and Python clients both matter

Add a client-side Rust library, tentatively `gwz-client`, for `gwz-cli` and
future Rust clients. It should depend on protocol types and a transport
abstraction, not on workspace handler internals.

Rust client responsibilities:

- Parse Git-like CLI diff options into structured request options while leaving
  ambiguous positional operands for core.
- Choose local in-process core or remote core transport.
- Call Phase 1 discovery when needed, fetch Phase 2 file patches in manifest
  order, and assemble human patch output client-side.
- Render stdout/stderr.
- Apply terminal color policy.
- Launch and feed `less` or another pager.
- Implement `--exit-code` and `--quiet` process status.

`gwz-py` should not be described as reusing this Rust crate unless a deliberate
new PyO3 wrapper is added for it. Its v0 work is separate:

- Use the existing PyO3 request/response bridge for Phase 1 and Phase 2 calls.
- Add Python protocol-generated diff messages.
- Add Python CLI parser support for `gwz diff`.
- Reimplement pager, color, JSON/JSONL, and exit-code behavior in Python or
  expose those exact client helpers through a new PyO3 surface.

### AD7 - Root participation is explicit but default-on

The default unified diff includes the workspace root repository and active
materialized members. This mirrors combined status, where root metadata changes
are visible alongside member changes.

Rules:

- No explicit selection: include root plus all active materialized Git members.
- `--all`: include root plus all active materialized Git members. For diff,
  `--all` means the whole unified workspace rather than member-only selection.
- Explicit member selection: include selected members. Include root only with a
  root pathspec or an explicit future `--include-root` option.
- Pathspecs under `gwz.conf/`, `Cargo.toml`, or other root-owned paths target
  the root repository.
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

The v0 protocol should add ordinary request/response methods for a two-phase
diff. The exact numbering must be assigned during implementation after checking
the current generated schema and corpus.

### Service methods

```python
service("GwzCore",
    # Existing methods omitted.

    # Phase 1: changed-file manifest and summary. No patch bytes.
    method("diff", role="in",
           params=Params(request=Ref.DiffRequest),
           out=Ref.DiffManifestResponse),

    # Phase 2: patch for one manifest file or one explicit literal file.
    method("diff.file", role="in",
           params=Params(request=Ref.DiffFileRequest),
           out=Ref.DiffFileResponse),

    # Phase 2 batch: optional bounded batch for remote round-trip amortization.
    method("diff.files", role="in",
           params=Params(request=Ref.DiffFilesRequest),
           out=Ref.DiffFilesResponse))
```

Removed from v0: `diff.open`, `diff.output`, `diff.cancel`, output spools,
`DiffStreamKind`, `DiffOutputChunk` as a stream payload, `DiffStreamEvent`, and
`DiffOpenResponse`.

Rationale:

- Phase 1 gives programmatic consumers the data they naturally need first:
  which files changed, how they changed, and aggregate summaries.
- Phase 2 keeps patch responses bounded by file or explicit small batch.
- `gwz diff | less` remains a client workflow: the client gets the manifest,
  fetches patches in manifest order, applies color/pager policy, and writes a
  single coherent document.
- A client that quits the pager or hits a broken pipe stops issuing Phase 2
  calls. No domain cancellation method is needed for v0.
- Taut `log` maturation remains valuable for future generic operation output,
  but it is not on the v0 critical path for `gwz diff`.

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
    merge_base_oid=F(7, STR, optional=True))

DiffOptions=Msg(
    output_format=F(1, Ref.DiffOutputFormat, optional=True),
    context_lines=F(2, INT, optional=True),
    interhunk_lines=F(3, INT, optional=True),
    algorithm=F(4, Ref.DiffAlgorithm, optional=True),
    whitespace=F(5, Ref.DiffWhitespaceMode, optional=True),
    find_renames=F(6, BOOL, optional=True),
    # Tier 1 unless D-1 proves faithful Phase 2 copy rendering.
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

### Manifest, file, and summary messages

```python
DiffRepoScope=Msg(
    # True for the workspace root repository.
    root=F(1, BOOL, optional=True),
    member_id=F(2, STR, optional=True),
    member_path=F(3, STR, optional=True),
    source_kind=F(4, Ref.SourceKind, optional=True))

DiffFileEntry=Msg(
    # Stable within the manifest, opaque to clients, and not parsed by core in
    # Phase 2. Scope/status/old_path/new_path are the structured selector.
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
    targets=F(4, List(Ref.DiffParsedTarget)))

DiffFileRequest=Msg(
    meta=F(1, Ref.RequestMeta),
    workspace_cwd=F(2, STR, optional=True),
    scope=F(3, Ref.DiffRepoScope, optional=True),
    # From Phase 1. Recommended for manifest-driven clients. Core validates
    # this entry against the recomputed diff; it does not parse file_id.
    entry=F(4, Ref.DiffFileEntry, optional=True),
    # Optional opaque correlation id echoed in the response.
    file_id=F(5, STR, optional=True),
    # For explicit-file fast path. Workspace-relative after cwd resolution.
    path=F(6, STR, optional=True),
    # Preferred for manifest-driven Phase 2. Reuse the scoped target resolved
    # by Phase 1 instead of reclassifying raw operands.
    target=F(7, Ref.DiffParsedTarget, optional=True),
    # Explicit-file fast path can skip Phase 1 and therefore may send the same
    # unresolved comparison inputs as DiffRequest.
    operands=F(8, List(STR), optional=True),
    cached=F(9, BOOL, optional=True),
    merge_base=F(10, BOOL, optional=True),
    options=F(11, Ref.DiffOptions, optional=True),
    # Oversized-file fallback: read a window of this one file patch.
    from_offset=F(12, INT, optional=True),
    max_bytes=F(13, INT, optional=True))

DiffFileResponse=Msg(
    response=F(1, Ref.ResponseEnvelope),
    # Optional on failure; response carries the error envelope.
    scope=F(2, Ref.DiffRepoScope, optional=True),
    file_id=F(3, STR, optional=True),
    format=F(4, Ref.DiffOutputFormat, optional=True),
    encoding=F(5, Ref.DiffChunkEncoding, optional=True),
    data=F(6, BYTES, optional=True),
    truncated=F(7, BOOL, optional=True),
    total_bytes=F(8, INT, optional=True),
    next_offset=F(9, INT, optional=True),
    # True when the manifest entry no longer matches the current worktree diff.
    stale=F(10, BOOL, optional=True))

DiffFilesRequest=Msg(
    meta=F(1, Ref.RequestMeta),
    files=F(2, List(Ref.DiffFileRequest)),
    max_total_bytes=F(3, INT, optional=True))

DiffFilesResponse=Msg(
    response=F(1, Ref.ResponseEnvelope),
    files=F(2, List(Ref.DiffFileResponse)),
    truncated=F(3, BOOL, optional=True))
```

Phase contracts:

- Phase 1 runs the full per-repo `Diff`, including rename similarity, and
  returns manifest entries in the final display order. Copy similarity is Tier 1:
  v0 must reject `find_copies=true` unless D-1 proves a faithful Phase 2 copy
  strategy before D0 freezes the schema.
- `DiffRepoSummary.files_changed` counts changed files before GWZ root/member
  filtering where libgit2 can report that cheaply; `files_manifested` counts
  entries actually surfaced after pathspec, selection, and root-exclusion
  filtering.
- Phase 1 alone satisfies `--name-only`, `--name-status`, `--stat`,
  `--numstat`, `--shortstat`, `--summary`, `--quiet`, JSON metadata modes, and
  the exit-code decision. `--exit-code` without `--quiet` still fetches and
  prints patches.
- For `--quiet`, core may use `DiffManifestMode.any_difference`: stop at the
  first detected difference, skip similarity detection, omit `files`, and return
  only enough summary data for exit-code behavior. Do not use this mode for
  JSON summaries, name/status/stat output, or patch assembly.
- Phase 2 is stateless by default: the client sends the same comparison
  target plus the `DiffFileEntry` from the manifest, or unresolved explicit-file
  comparison inputs plus an explicit path. Core recomputes the relevant repo
  diff and renders the requested bounded response. When a scoped
  `DiffParsedTarget` contains resolved object ids, Phase 2 uses those ids
  instead of re-resolving revision tokens.
- `DiffFileRequest` must contain either `entry` for manifest-driven requests or
  `path` for the explicit-file fast path. Manifest-driven requests should carry
  the matching scoped `DiffParsedTarget`; explicit-file requests may carry
  `operands`, `cached`, and `merge_base` because they intentionally skipped
  Phase 1. `DiffOptions.manifest_mode` applies only to Phase 1; Phase 2 ignores
  or rejects `any_difference`.
- For ordinary add/modify/delete/type-change entries, Phase 2 may narrow the
  recompute to the relevant path. For manifest-driven rename entries, Phase 2
  must include both `old_path` and `new_path`, rerun similarity detection, and
  extract the matching delta. Manifest-driven copy entries are allowed only
  after the implementation can reproduce the Phase 1 source set, including
  unmodified copy sources when copy detection allows them, or after a retained
  diff/blob-id strategy exists; until then `find_copies=true` is rejected.
- If a stateless Phase 2 recompute no longer finds the requested manifest entry
  because the worktree changed, return a stale empty `DiffFileResponse` rather
  than a fatal error.
- `diff.files` batches several Phase 2 calls with a total byte cap. If a batch
  exceeds the cap, the client falls back to smaller batches or single-file
  calls.
- `diff.files` may contain per-file successes and failures because each
  `DiffFileResponse` has its own `ResponseEnvelope`; on failure, patch payload
  fields such as `scope`, `format`, `encoding`, and `data` may be absent.
  Clients must handle partial batch results.
- For a pathologically large file patch, `DiffFileResponse.truncated` plus
  `from_offset`/`max_bytes` provides one-file windowing. This is an edge case,
  not the whole-diff transport.
- Literal explicit files may skip Phase 1. Directory pathspecs, globs, and bare
  `gwz diff` must run Phase 1 because the changed set is unknown.
- Rename/copy detection for explicit single-file requests is scoped to that
  filtered path set, matching `git diff -- <path>` behavior. Full rename/copy
  pairing is a Phase 1 discovery feature.

### Taut extension notes

The two-phase v0 protocol should not grow diff-specific substitutes for generic
Taut concerns. If later GWZ operations need long-running output feeds, Taut
should mature a generic `log`-shaped facility with precise byte or record
offsets, explicit tailing semantics, and clear cancellation/close behavior.

For `gwz diff` v0:

- Do not add a diff-only cancellation method. Stopping consumption means the
  client stops issuing Phase 2 requests.
- Do not add a diff-only whole-operation output log. File-granular patch
  responses keep diff shippable without blocking on generic Taut output work.
- Keep any future `log` semantics byte- or record-precise enough for patch
  bytes, binary hunks, and JSONL records. Patch bytes must not flow through the
  bounded operation-event buffer.

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
   - No explicit selection or `--all`: root plus all active materialized Git
     members.
   - Explicit member selection: selected active materialized Git members. Root
     is added only when a root-owned pathspec is present or a future
     `--include-root` option is used.
2. Route pathspecs relative to `DiffRequest.workspace_cwd` and intersect them
   with the candidate list:
   - No pathspec means keep the candidate list unchanged.
   - A member pathspec keeps only that member if it is in the candidate list.
   - A pathspec at or above member boundaries fans out only to candidate
     members.
   - A root-owned pathspec keeps root and removes members that cannot match it.
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
  over deltas/patch lines so Phase 1 can render workspace-correct stats.

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
- `--no-index`, unless implemented as client-local filesystem diff.
- `--patch-with-stat` until a GWZ patch+stat combiner exists.

## CLI and client library plan

Add a client-side Rust library, tentatively `gwz-client`, for `gwz-cli` and
future Rust clients. Add parallel Python client work in `gwz-py`; do not assume
Python can directly reuse the Rust crate.

Suggested modules:

- `transport`: trait for local in-process core and future remote core clients.
- `diff_args`: Git-compatible parser for options and `--`; ambiguous operands
  remain raw for core.
- `diff_manifest`: obtains and formats Phase 1 manifests and summaries.
- `diff_patch`: fetches Phase 2 per-file/batch patches and assembles them in
  manifest order.
- `diff_render`: applies color, line prefix, and machine output formatting.
- `pager`: launches `less` or configured pager and writes assembled bytes.
- `exit_code`: maps `DiffSummary` and options to process status.

`gwz-cli` should become a thin shell:

1. Parse global GWZ options.
2. Delegate diff argument parsing to `gwz-client`.
3. Open local or remote GWZ core transport.
4. Run `diff` discovery unless all requested pathspecs are explicit literal
   files.
   - For human `--quiet` without JSON/JSONL metadata, request
     `DiffManifestMode.any_difference`.
5. Fetch `diff.file`/`diff.files` patch responses in manifest or requested path
   order.
6. Pipe assembled output to stdout or pager.
7. Exit according to `--exit-code`/`--quiet` from the manifest summary.

`gwz-py` should add equivalent Python-side pieces:

1. Add generated protocol classes for `DiffRequest`, `DiffManifestResponse`,
   `DiffFileRequest`, `DiffFileResponse`, and summaries.
2. Use the existing PyO3 `call` bridge for discovery and per-file patch calls.
3. Add a Python argparse command for `gwz diff`.
4. Implement Python manifest formatting, patch assembly, pager, JSON/JSONL, and
   exit-code behavior.
5. Keep Python CLI parity tests alongside existing `gwz-py` CLI parity tests.

Pager policy:

- Default to pager only for human patch output on a TTY.
- Never page `--json`, `--jsonl`, `--quiet`, or NUL-terminated output.
- Honor `--no-pager`, `GIT_PAGER`, `PAGER`, and a future GWZ config key.
- Pager failure is a client-side `external_tool_missing` or I/O error, not a
  core diff failure.
- Pager quit or stdout broken pipe stops issuing Phase 2 requests. No server
  cancellation call is required for stateless v0.

Color policy:

- Default `auto`: color only when writing human patch output to a terminal or
  a pager that expects ANSI.
- `--color=always` and `--color=never` are client rendering decisions.
- The core patch responses remain canonical uncolored bytes.

## Implementation phases

### D-1 - Two-phase request/response spike

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/lib.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/dispatch/mod.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/bridge.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/client.py`

Work:

- Confirm ordinary request/response can carry `BYTES`, including NUL bytes,
  through Rust and the PyO3 bridge.
- Choose the Phase 2 state model. Recommended v0: stateless recompute for
  `diff.file`, using the scoped resolved `DiffParsedTarget` plus the manifest
  `DiffFileEntry`, or explicit-file comparison inputs plus an explicit path.
  Defer stateful retained `Diff` caches until profiling demands them.
- Prove `cached`/`merge_base` request fields cover `--cached`, `--staged`,
  `--merge-base`, and `A...B` without tunneling parsed flags through operands.
- Prove the rename Phase 2 strategy: manifest-driven renames must include both
  old and new paths during recompute. Copy detection remains rejected unless D-1
  proves Phase 2 can reproduce the Phase 1 source set or can use a
  retained-diff/blob-id strategy.
- Define `diff.files` batch limits: maximum files per batch and maximum total
  response bytes.
- Define oversized-file behavior: default byte cap, `truncated` response, and
  whether human clients transparently continue with offset windows or show a
  truncation marker.
- Define literal-file fast-path detection: files can skip Phase 1; directories,
  globs, pathspec magic, and ambiguous operands must run Phase 1.
- Define `DiffManifestMode.any_difference` behavior for `--quiet` and stale
  `DiffFileResponse` behavior for worktree races.

Acceptance:

- Rust and PyO3 tests can round-trip `DiffFileResponse.data` containing binary
  bytes and NULs.
- A stateless `diff.file` sketch can recompute a single-file patch from
  scoped `DiffParsedTarget` plus `DiffFileEntry`, or from explicit-file request
  fields.
- `--cached`, `--staged`, `--merge-base`, and `A...B` have concrete request and
  corpus examples before D0 freezes schema.
- Rename manifest entries do not degrade into add/delete patches during Phase 2.
  Copy detection is either proven faithful or rejected before D0.
- Batch overflow and oversized-file window behavior are specified before D0
  freezes schema.

### D0 - Protocol design and corpus

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/corpus/`

Work:

- Add Taut enums and messages for diff request/options, manifest entries,
  manifest responses, file patch requests/responses, batch patch
  requests/responses, and summaries.
- Add `ActionKind.diff`.
- Add service entries for `diff`, `diff.file`, and optional `diff.files`.
- Regenerate Rust and Python protocol outputs and golden corpus.
- Add compatibility tests for default values and unknown optional fields.
- Add corpus examples for manifest-driven `DiffFileRequest.entry`,
  scoped `DiffParsedTarget`, `DiffManifestMode.any_difference`, stale
  `DiffFileResponse`, partial `DiffFilesResponse`, and comparison forms using
  `cached`, `merge_base`, and `A...B`.
- Keep client-only process status out of the core request; do not include
  unsupported algorithm values such as histogram in v0.

Acceptance:

- Protocol generation is deterministic.
- Existing protocol corpus still passes.
- A minimal `DiffRequest` round trips through Taut.
- `DiffFileResponse` can carry arbitrary bytes, including NUL bytes.
- Manifest entries can be echoed through `DiffFileRequest.entry` without
  forcing clients to parse `file_id`.
- `DiffParsedTarget` is scope-addressable and can be reused by Phase 2 without
  reclassifying raw operands.
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
- Add `GitBackend::diff_manifest` and `GitBackend::diff_file_patch` or narrower
  primitives that produce manifest entries, stats, and one-file patch bytes.
- Implement `Git2Backend` with libgit2 calls mapped from comparison kind.
- Resolve commits/trees/blobs and ambiguous operands per target repo.
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

- Reuse or extract `resolve_workspace_root`, manifest validation, lock optional
  reads, and explicit selection handling.
- Extract path-to-repo partitioning from stage routing into shared workspace
  pathspec routing, while keeping manifest order separate from partitioning.
- Resolve request `workspace_cwd` relative to the workspace root.
- Decide root inclusion from selection and pathspecs using candidate-list then
  pathspec-intersection semantics.
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
- `gwz diff -- gwz-core/src/lib.rs` targets only `gwz-core` with
  `src/lib.rs`.
- `gwz diff -- gwz.conf/gwz.yml` targets only root.
- `gwz diff -- .` at workspace root targets root plus members.
- Parent-relative pathspecs from subdirectories behave like Git.
- Explicit member `A` plus pathspec `B/file` returns a clean empty result when
  both names are valid but non-overlapping.
- Root diff output does not include active member paths, `.gwz/`, or
  `gwz.conf/.tmp/` even when `.git/info/exclude` is stale.

### D3 - Core manifest and file patch handlers

Touchpoints:

- New diff handler module.

Work:

- Implement `handle_diff_manifest` for `diff`.
- Implement `handle_diff_file` for `diff.file`.
- Implement optional `handle_diff_files` for bounded batch patch responses.
- Phase 1 computes manifest entries, per-file stats, rename pairing, binary
  flags, per-repo summaries, and aggregate summary. Copy pairing is computed
  only after the Phase 2 copy strategy is proven and `find_copies` is enabled.
- Phase 2 recomputes or filters the relevant repo diff and renders only the
  requested file patch.
- Phase 2 validates manifest-driven selectors using `DiffFileEntry`
  scope/status/old_path/new_path plus the scoped `DiffParsedTarget`. `file_id`
  is opaque and is never parsed as a path container.
- Phase 2 preserves manifest rename/copy semantics. Rename patches are
  recomputed with both paths and similarity enabled. Copy patches are enabled
  only when the recompute strategy can include the Phase 1 copy source set;
  otherwise copy rendering remains deferred with a clear unsupported-option
  error.
- Implement `--name-only`, `--name-status`, `--stat`, `--numstat`,
  `--shortstat`, `--summary`, and `--quiet` from the manifest and summary
  without Phase 2 patch calls. Implement the `--exit-code` process decision
  from the manifest summary while still fetching patches when output is not
  quiet.
- Implement `DiffManifestMode.any_difference` for `--quiet` so core can
  short-circuit without full manifest construction or similarity detection.
- Implement oversized-file caps and `from_offset`/`max_bytes` windowing for one
  file patch.
- Return stale empty file responses when a worktree race makes a manifest entry
  disappear before Phase 2.
- For `diff.files`, allow failed per-file responses to omit patch payload fields
  while preserving successful file responses in the same batch.

Acceptance:

- Manifest file order is root first, then members in manifest order.
- Per-file manifest metadata is correct for add, modify, delete, rename, type
  change, binary, and mode change cases; copy cases are covered only if copy
  detection is enabled after the D-1 proof.
- `diff.file` returns a workspace-relative patch for exactly one file.
- `diff.file` returns rename headers that agree with the manifest entry. Copy
  headers are required only after copy support is proven and enabled.
- `diff.files` respects the batch byte cap and reports truncation/fallback
  cleanly.
- `diff.files` partial failures do not require dummy `data` bytes or dummy
  patch metadata.
- Oversized single-file patches are resumable by offset.
- Stale manifest entries are reported as non-fatal stale file responses.
- Final summary is correct when the manifest contains multiple repos.
- Errors in one member are member-scoped and respect partial policy.
- `operation.result` remains small and metadata-only.

### D4 - Patch rendering and path rewriting

Touchpoints:

- `git2::Diff::print`
- `git2::Diff::foreach`

Work:

- First attempt: use libgit2 prefix options for patch, raw, name-only, and
  name-status.
- Build `--stat`, `--numstat`, `--shortstat`, `--summary`, `--name-only`, and
  `--name-status` from Phase 1 manifest entries rather than forwarding
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

- New `gwz-client` crate or equivalent client-side module.
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/globalargs.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs`
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
- Keep unsupported Git options as clear `invalid_request` errors.
- Implement manifest discovery, per-file/batch patch fetching, and client-side
  patch assembly.
- Treat stale `DiffFileResponse` values as a non-fatal worktree race: omit the
  stale file in normal human output, and expose the stale marker in machine
  output.
- Handle partial `diff.files` results by preserving successful file responses
  and retrying or reporting failed file responses individually.
- Implement client-side color.
- Implement `--exit-code` and `--quiet`.
- Ensure the PyO3 native dispatch registry and Python bridge can call `diff`,
  `diff.file`, and optional `diff.files` through ordinary request/response.
- Add JSON/JSONL behavior:
  - `--json`: emit `DiffManifestResponse` metadata and summary, not patch bytes.
  - `--jsonl`: emit manifest entries and optional file patch records only after
    verifying how Taut JSON serializes `BYTES`; document base64 expansion if
    that is the encoding.
- Add equivalent `gwz-py` client and CLI support instead of assuming Rust
  client crate reuse.

Acceptance:

- `gwz diff` writes assembled patch text to stdout.
- `gwz diff | cat` does not launch a pager.
- `gwz diff` on a TTY uses pager unless disabled.
- `gwz diff --quiet --exit-code` emits no patch and exits 1 on differences.
- `gwz --json diff --quiet` returns summary metadata.
- Python `gwz diff` can discover manifests, fetch/assemble patches, render
  JSON/JSONL, and return the same exit status as the Rust CLI for covered
  cases.

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
- Document path rewriting and root/member selection behavior.
- Document manifest/file-patch protocol and JSON/JSONL behavior.
- Document unsupported/deferred Git options.
- Document Python CLI/API parity and any intentional differences.
- Add troubleshooting entries for missing refs in some members and
  unmaterialized selected members.

Acceptance:

- Generated CLI reference includes `diff`.
- Machine output docs include `DiffRequest`, `DiffManifestResponse`,
  `DiffFileRequest`, `DiffFileResponse`, and `DiffFilesResponse` examples.
- Protocol docs explain that Phase 1 is metadata-only and patch bytes are
  returned by bounded Phase 2 file responses.

## Option parity tiers

### Tier 0 - v0 must have

- Default patch output.
- `--cached` and `--staged`.
- One commit/tree argument.
- Two commit/tree arguments.
- `A..B` and `A...B` parsing.
- `--merge-base` where it maps to Git's documented forms.
- Pathspec separator `--`.
- `-U<n>` and `--unified=<n>`.
- `--inter-hunk-context=<n>`.
- `--name-only`.
- `--name-status`.
- `--raw`.
- `--stat`, `--numstat`, `--shortstat`, and `--summary` from the Phase 1
  manifest.
- `--binary`.
- `--text`.
- `--find-renames`, `--no-renames`, and a rename threshold.
- `-w`, `-b`, `--ignore-space-at-eol`, and `--ignore-blank-lines`.
- `--src-prefix`, `--dst-prefix`, `--no-prefix`, and `--line-prefix`.
- `--exit-code`.
- `--quiet`.

### Tier 1 - near follow-up

- `--diff-filter`.
- `--patch-with-raw`.
- `--patch-with-stat`.
- `--full-index` and `--abbrev`.
- Copy detection `-C`.
- Rename/copy limits.
- `--ignore-submodules`.
- `--check` and whitespace error summaries.
- Better Git pathspec magic.

### Tier 2 - explicit deferral

- Word diff and color words.
- Moved-line coloring.
- External diff helpers.
- Textconv.
- Pickaxe `-S`/`-G`.
- Order file and rotate/skip.
- Combined merge diff.
- Blob-vs-blob if cross-repo object routing proves awkward.
- `--no-index` unless implemented as a client-local filesystem diff.

## Test strategy

Core backend parity tests:

- Compare libgit2 output with `git diff` for one isolated repository.
- Cover modification, add, delete, rename, type change, executable bit change,
  binary file, and pathspec filtering.
- Cover `--cached`, `HEAD`, two-tree, and unborn staged changes.
- Cover request schema mapping for `--cached`, `--staged`, `--merge-base`, and
  `A...B`.

Workspace integration tests:

- Root-only diff.
- Member-only diff.
- Root plus multiple members in deterministic order.
- Selection by member id and member path.
- Cwd-relative pathspecs from root, member, and nested subdirectory.
- Missing ref in one member with and without partial policy.
- Two members resolving the same operand differently, with scoped
  `DiffParsedTarget` entries for each member.
- Unmaterialized fan-out member skipped; explicit unmaterialized member errors.
- Literal explicit file pathspecs skip Phase 1 and route directly to
  `diff.file`.
- Directory pathspecs, globs, and ambiguous operands use Phase 1 discovery.

Manifest and patch tests:

- Phase 1 manifest reports status, old/new paths, modes, rename similarity,
  binary flag, per-file stats, repo scope, and aggregate summary. Copy
  similarity is covered only if copy detection is proven and enabled.
- `DiffRepoSummary.files_changed` and `files_manifested` are stable and
  documented when root/member filtering removes deltas.
- Manifest order is root first, then members in manifest order.
- `--name-only`, `--name-status`, `--stat`, `--numstat`, `--shortstat`,
  `--summary`, and `--quiet` need no Phase 2 patch calls.
- `--quiet` uses an early-exit any-difference manifest mode and does not run
  rename/copy similarity.
- `diff.file` returns exactly one workspace-relative patch.
- Manifest-driven rename `diff.file` output preserves `similarity index`,
  `rename from`, and `rename to` headers.
- Copy detection is either faithfully reproduced in Phase 2 or rejected as an
  unsupported Tier 1 option; it is never silently rendered as an add.
- `diff.files` preserves requested order and respects batch byte limits.
- `diff.files` partial failures are visible per file and do not discard
  successful file responses.
- Oversized file patch returns `truncated`, `total_bytes`, and `next_offset`;
  follow-up offset reads reconstruct the full one-file patch.
- A worktree edit between manifest and file fetch returns a stale file response
  rather than a fatal error.
- Patch bytes survive NUL records and binary patch data.
- Final `DiffSummary` matches accumulated manifest entries.

CLI/client tests:

- Pager is not launched for pipes, JSON, JSONL, quiet, or no-pager.
- Pager is launched for TTY human patch output.
- Color auto/always/never is client-side.
- `--exit-code` returns 1 on differences and 0 when clean.
- Plain `gwz diff` returns 0 with differences.
- Broken pipe or pager quit stops further Phase 2 requests.
- Stale file responses are omitted or surfaced consistently according to
  output mode.
- Rust and Python clients assemble manifest-ordered patches identically.

Python client tests:

- PyO3 `call` path returns `DiffManifestResponse` and `DiffFileResponse`
  including BYTES payloads without using `subscribe_events`.
- Python `gwz diff` handles the same Tier 0 parser cases as Rust for covered
  options.
- Python and Rust clients agree on `--exit-code`, `--quiet`, JSON summary, and
  JSONL manifest/file behavior.

Golden output tests:

- Stabilize a small fixture workspace with root plus two members.
- Compare selected `gwz diff` outputs against checked-in goldens.
- Keep goldens focused on path headers and sequencing rather than every hunk
  detail from libgit2 if libgit2 version drift can legitimately change hunk
  boundaries.

## Open decisions

- Should Phase 2 remain stateless recompute forever, or should a later release
  retain Phase 1 `Diff` objects behind operation ids for high-latency remote
  clients? This plan says stateless recompute for v0.
- What exact byte caps should govern `diff.file` truncation and `diff.files`
  batching?
- Should human clients transparently fetch oversized-file windows, or show a
  truncation marker unless the user asks for the full patch?
- Should copy rendering use stateless repo-scope recompute, retained Phase 1
  diffs, or blob-id based rendering? Until this is proven, copy detection stays
  Tier 1 and must fail clearly rather than degrading to add output.
- Should root exclusion later add lower-level libgit2 pruning for performance?
  Correctness uses the GWZ delta post-filter in v0.
- Should unsupported Git options be hard errors or warnings? For Git parity,
  they should be hard errors until implemented.
- Should `gwz-client` be a new workspace crate or a module inside `gwz-cli`
  first? The client/server direction argues for a separate crate early, but a
  module can be a temporary stepping stone if crate churn is too high.
- Should `gwz-py` reimplement pager/color/parser behavior in Python, or should
  a new PyO3 helper expose selected Rust client helpers? The plan budgets Python
  work either way.

## Definition of done

`gwz diff` is ready when:

- It handles the Tier 0 forms above across root and member repositories.
- Output paths are workspace-relative and deterministic.
- Phase 1 returns a complete changed-file manifest and summary without patch
  bytes.
- Phase 2 returns bounded per-file or batch patch bytes, including oversized
  file windowing.
- Core contains no pager or terminal presentation logic.
- `gwz-cli` consumes the client-side API for pager, color, and exit status.
- `gwz-py` has equivalent protocol, parser, rendering, assembly, and exit-code
  behavior for the supported surface.
- JSON/JSONL behavior is documented and tested.
- The protocol corpus includes diff manifest, file patch, batch patch, and
  summary examples.
