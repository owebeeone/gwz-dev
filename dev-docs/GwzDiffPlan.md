# GWZ diff implementation plan

Status: design plan, 2026-06-27.

This plan covers a `gwz diff` command that behaves like `git diff` over the
GWZ unified workspace: the root repository plus materialized member Git
repositories rendered as one workspace-relative change stream.

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
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan-Review55.md`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan-Review55-2.md`
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
must receive equivalent protocol, streaming, parser, rendering, and exit-code
work rather than relying on Rust crate reuse.

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
  `events.subscribe`. There is no generic operation-output log payload today.
- `gwz forall` is intentionally CLI-local. `gwz diff` must not follow that
  model because repository object access and revision resolution belong in
  `gwz-core`; terminal presentation belongs in the client.
- Taut's `log` shape is the intended append-only/read-from-offset/tail
  abstraction for diff output. The current GWZ `events.subscribe`
  implementation uses a bounded buffer and can emit `reset` after dropping
  history, so that implementation must be matured or re-shaped before it can be
  treated as the model for patch-byte transport.
- `gwz-py` links `gwz-core` through PyO3 and currently exposes `call`,
  `submit`, `subscribe_events`, `wait_events`, `operation_result`, and
  `try_operation_result`. Its streaming path is the same lossy
  operation-event path that `gwz diff` must avoid.

## Goals

- Match `git diff` semantics where they map cleanly to a unified GWZ workspace.
- Use libgit2 for the core diff engine whenever possible.
- Render one deterministic workspace-relative stream across the root repository
  and selected members.
- Keep `gwz-core` headless and server-safe.
- Add protocol support for streaming large responses without forcing the final
  response to hold patch text.
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
workspace artifacts. Ephemeral operation records and output spools are allowed
runtime state, but they must not change Git state or tracked GWZ metadata.

### AD2 - Diff is computed per repository and rendered as one stream

The core operation runs the requested Git diff independently in:

1. The root repository, when included.
2. Each selected materialized Git member, in manifest order.

Each repo produces a normal libgit2 `Diff`. GWZ rewrites or renders paths so
every output path is relative to the workspace root.

This avoids inventing a virtual Git repository while preserving the user model
of one unified workspace.

### AD3 - Core emits canonical uncolored output

`gwz-core` should emit canonical, uncolored diff content plus structured
summary metadata. Color and pager decisions are presentation concerns.

The client library may colorize patch lines when the selected output mode is a
terminal mode. It should not ask core to embed ANSI escapes by default.

### AD4 - Patch bytes require a non-lossy Taut log

The existing `events.subscribe` stream is lifecycle/progress oriented. Its
bounded buffer can reset and lose history. Patch data cannot use that as its
only transport.

The protocol must provide a Taut `log` where diff chunks are delivered in
order. A slow consumer must not silently lose bytes. The v0 transport design
should mature Taut's `log` implementation before the diff schema is frozen.

The preferred v0 mechanism is an operation output log, separate from
`OperationRuntime` events:

- Core appends ordered byte chunks to an operation-scoped output log with a
  monotonic byte offset and sequence.
- Consumers read from offset and may tail until a final event is available.
- The spool may be memory-backed for small output and spill to `.gwz/` or a
  server-local temp area for large output. Root diffs must exclude the runtime
  spool path in memory so spool files never appear in diff output even when
  `.git/info/exclude` is stale.
- If configured spool limits are reached, core must either block production
  until consumers drain or fail the operation with a typed output-capacity
  error. It must not drop already-acknowledged bytes.
- The log has an explicit cancel path so a pager quit, broken pipe, or
  remote client disconnect can stop diff generation and clean up the spool.
- The same log contract must be available through the PyO3 bridge for
  `gwz-py`; it cannot go through `subscribe_events`.

If implementation chooses a pure blocking sink instead of a spool, it must still
provide an equivalent PyO3 and remote-client story and must document what
happens on disconnect.

### AD5 - Final operation results stay metadata-only

`operation.result` should not carry patch text. It may carry final status,
member outcomes, errors, and small summaries. Large output belongs to the diff
output log.

### AD6 - Client behavior lives outside core, but Rust and Python clients both matter

Add a client-side Rust library, tentatively `gwz-client`, for `gwz-cli` and
future Rust clients. It should depend on protocol types and a transport
abstraction, not on workspace handler internals.

Rust client responsibilities:

- Parse Git-like CLI diff options into structured request options while leaving
  ambiguous positional operands for core.
- Choose local in-process core or remote core transport.
- Consume the non-lossy output log.
- Render stdout/stderr.
- Apply terminal color policy.
- Launch and feed `less` or another pager.
- Implement `--exit-code` and `--quiet` process status.

`gwz-py` should not be described as reusing this Rust crate unless a deliberate
new PyO3 wrapper is added for it. Its v0 work is separate:

- Add PyO3 bindings for the same non-lossy diff output log.
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
should either:

- apply in-memory pathspec exclusions for every active member path, `.gwz/`, and
  `gwz.conf/.tmp/` when diffing the root repository, or
- validate the managed exclude boundary and fail with a clear diagnostic that
  asks the user to run a boundary-refreshing command.

The preferred v0 path is in-memory exclusion so read-only diff works even if the
local root boundary is stale. D-1/D2 must prove the concrete implementation
strategy before this becomes a protocol promise: lower-level libgit2 filtering,
a GWZ filtered renderer/stat path, or fail-fast validation of the managed
exclude boundary. The current Rust `git2` wrapper's pathspec API is additive,
so the plan must not assume "all except these paths" exists until the spike
demonstrates it.

## Taut protocol proposals

The protocol should add metadata methods plus a log-shaped output method. The
exact numbering must be assigned during implementation after checking the
current generated schema and corpus.

These snippets assume the transport phase below has already matured Taut `log`
support for precise read-from-offset/tail behavior. Do not freeze the corpus
until that log shape has Rust and PyO3 consumers.

### Service methods

Recommended first shape:

```python
service("GwzCore",
    # Existing methods omitted.

    # Diff metadata and summary. This does not return patch bytes.
    method("diff", role="in",
           params=Params(request=Ref.DiffRequest),
           out=Ref.DiffResponse),

    # Start diff output production and return operation/output log ids.
    method("diff.open", role="in",
           params=Params(request=Ref.DiffRequest),
           out=Ref.DiffOpenResponse),

    # Taut log of ordered patch/raw/name output. Taut log runtime supplies
    # read-from-offset, bounded replay, and tail behavior for this shape.
    method("diff.output", role="out", shape="log",
           params=Params(output_id=STR, from_offset=INT, max_bytes=INT,
                         tail=BOOL),
           out=Ref.DiffStreamEvent),

    # Cancel an in-flight diff/output log.
    method("diff.cancel", role="ctl",
           params=Params(request=Ref.DiffCancelRequest),
           out=Ref.CancelOperationResponse))
```

Rationale:

- `diff.open` plus an offset-addressable output log avoids the race where a
  client receives an `operation_id` and subscribes after output has already
  been produced.
- `diff` remains useful for `--quiet`, summary-only API callers, JSON metadata,
  and tests.
- The general `events.subscribe` method can still publish lifecycle events, but
  patch bytes use the non-lossy output log.
- `diff.cancel` gives pager quit, broken pipe, and remote disconnect a defined
  cleanup path.

If GWZ later wants generic stdout/stderr streaming for multiple operations,
promote `DiffStreamEvent` into a generic `OperationOutputEvent` and make the
diff output log a thin specialization.

### Taut log extensions required by diff

`gwz diff` should push Taut forward rather than work around it. The correct
shape for diff bytes is `log`: append-only history with read-from-offset and
tail. D-1 must therefore mature the Taut log implementation and generated
clients, not invent a private GWZ read protocol.

Required Taut behavior:

- A `shape="log"` method must expose record-precise replay from a caller-supplied
  offset and live tail from that same offset without a subscribe-after-open race.
- The log reader must support bounded reads or bounded replay windows so a
  client can cap memory. For diff, the bound should be expressible in bytes,
  records, or both; `max_bytes` in the proposal is the GWZ-facing initial
  requirement.
- A log transport must not silently drop records. If retention or spool capacity
  is exceeded, the source must block, backpressure, or fail with a typed
  capacity/retention error.
- Log cursors must be stable enough for reconnect/resume. Diff events carry
  `byte_offset` and `sequence`; Taut log runtime may also maintain its own
  cursor token if that is the better cross-language API.
- Tailing a log must have teardown semantics. A client closing a subscription,
  pager quit, broken pipe, or remote disconnect must release the reader, and it
  must be possible to propagate cancellation to the producing operation when the
  operation is no longer wanted.
- Generated Rust, Python/PyO3, and TypeScript clients should expose log-shaped
  APIs as log APIs: replay/read, tail, and cancel/close. They should not expose
  every streaming shape as an undifferentiated subscribe callback.
- Existing lossy `events.subscribe` behavior should be audited. If a method is
  declared `shape="log"`, its implementation should satisfy log semantics or be
  renamed/re-shaped so the schema does not overpromise.

`diff.cancel` is a domain control method for stopping the producer. Generic log
reader teardown should live in Taut runtime/codegen so every log-shaped method
gets it.

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
    no_patch=4)

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

DiffStreamKind=Enum(
    started=0,
    repo_started=1,
    chunk=2,
    repo_finished=3,
    finished=4,
    error=5,
    cancelled=6)

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
    # Resolved by core per repository from DiffRequest.operands/pathspecs.
    comparison=F(1, Ref.DiffComparison),
    # Repo-relative pathspecs after workspace routing.
    pathspecs=F(2, List(STR)))

DiffOptions=Msg(
    output_format=F(1, Ref.DiffOutputFormat, optional=True),
    context_lines=F(2, INT, optional=True),
    interhunk_lines=F(3, INT, optional=True),
    algorithm=F(4, Ref.DiffAlgorithm, optional=True),
    whitespace=F(5, Ref.DiffWhitespaceMode, optional=True),
    find_renames=F(6, BOOL, optional=True),
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
    # Core-side output suppression for --quiet and metadata-only callers.
    summary_only=F(22, BOOL, optional=True))

DiffRequest=Msg(
    meta=F(1, Ref.RequestMeta),
    # Workspace-relative logical cwd used to resolve relative path operands.
    workspace_cwd=F(2, STR, optional=True),
    # Positional tokens before an explicit "--". Core classifies these per
    # target repo because rev/path ambiguity needs repository knowledge.
    operands=F(3, List(STR)),
    # Pathspecs after an explicit "--"; resolved relative to workspace_cwd.
    explicit_pathspecs=F(4, List(STR)),
    options=F(5, Ref.DiffOptions, optional=True))
```

The protocol should be structured. Do not make the core API a raw `git diff`
argv tunnel. The client parser can be Git-compatible for options and the `--`
boundary while the wire contract remains stable and language-neutral. Core owns
classification of ambiguous positional operands.

### Log and summary messages

```python
DiffRepoScope=Msg(
    # True for the workspace root repository.
    root=F(1, BOOL, optional=True),
    member_id=F(2, STR, optional=True),
    member_path=F(3, STR, optional=True),
    source_kind=F(4, Ref.SourceKind, optional=True))

DiffOutputChunk=Msg(
    format=F(1, Ref.DiffOutputFormat),
    encoding=F(2, Ref.DiffChunkEncoding),
    # Patch/raw/name bytes. UTF-8 text is still carried as bytes so the stream
    # can preserve NUL-terminated records and binary patch data.
    data=F(3, BYTES),
    byte_offset=F(4, INT, optional=True),
    line_count=F(5, INT, optional=True))

DiffRepoSummary=Msg(
    scope=F(1, Ref.DiffRepoScope),
    has_differences=F(2, BOOL),
    files_changed=F(3, INT),
    insertions=F(4, INT),
    deletions=F(5, INT),
    bytes_streamed=F(6, INT),
    # Echoes core's per-repo operand/pathspec classification for diagnostics
    # and parity tests. Omitted only when no diff planning occurred.
    parsed_target=F(7, Ref.DiffParsedTarget, optional=True))

DiffSummary=Msg(
    has_differences=F(1, BOOL),
    repos_examined=F(2, INT),
    repos_with_differences=F(3, INT),
    files_changed=F(4, INT),
    insertions=F(5, INT),
    deletions=F(6, INT),
    bytes_streamed=F(7, INT),
    repo_summaries=F(8, List(Ref.DiffRepoSummary)))

DiffOpenResponse=Msg(
    response=F(1, Ref.ResponseEnvelope),
    # Operation id is also present in response.meta.operation_id; output_id
    # identifies the non-lossy Taut log for diff.output.
    output_id=F(2, STR))

DiffCancelRequest=Msg(
    meta=F(1, Ref.RequestMeta),
    operation_id=F(2, STR),
    output_id=F(3, STR, optional=True))

DiffStreamEvent=Msg(
    operation_id=F(1, STR),
    request_id=F(2, STR),
    sequence=F(3, INT),
    timestamp_ms=F(4, INT),
    kind=F(5, Ref.DiffStreamKind),
    severity=F(6, Ref.Severity),
    output_id=F(7, STR),
    scope=F(8, Ref.DiffRepoScope, optional=True),
    chunk=F(9, Ref.DiffOutputChunk, optional=True),
    response=F(10, Ref.DiffResponse, optional=True),
    error=F(11, Ref.GwzError, optional=True),
    attribution=F(12, Ref.OperationAttribution, optional=True))

DiffResponse=Msg(
    response=F(1, Ref.ResponseEnvelope),
    summary=F(2, Ref.DiffSummary, optional=True))

CancelOperationResponse=Msg(
    response=F(1, Ref.ResponseEnvelope))
```

Log contract:

- `started`: first event; carries `DiffResponse.response` with accepted-style
  metadata and the `operation_id`.
- `repo_started`: emitted before a target repository is diffed.
- `chunk`: carries ordered output bytes.
- `repo_finished`: carries a per-repo summary.
- `finished`: final event; carries `DiffResponse` with final summary and
  aggregate status.
- `error`: carries a typed `GwzError`; the stream then finishes with failed or
  partial status.
- `cancelled`: emitted after client cancellation when core stops producing
  output and releases log resources.

The stream sequence is per diff log and must be strictly increasing. The
transport must not drop `chunk` events. `diff.output` is a Taut log: clients can
read bounded history from `from_offset` and tail from the same cursor. Clients
can resume from the last observed byte offset after reconnect. If the spool
cannot accept more data and no blocking/backpressure path exists, core must
fail the operation instead of discarding chunks.

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

The client sends option flags, `operands` before `--`, and
`explicit_pathspecs` after `--`. Revision strings are resolved independently
inside each target repository. A branch name such as `main` means `main` in
each member. A missing ref in one member is a member-scoped error; global
`--partial` decides whether other members continue. If a token before `--` is
ambiguous, core follows Git's disambiguation rules per repo and returns a
member-scoped diagnostic when the token cannot be classified safely.

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
paths globally because that would require buffering all output and would weaken
streaming.

The root repo must exclude active member directories, `.gwz/`, and
`gwz.conf/.tmp/` in memory when computing root diffs. This protects read-only
diff from stale `.git/info/exclude` state and prevents operation spool files
from surfacing in root output.

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
- Per-repo diff stats via `Diff::stats`. Formatted `DiffStats::to_buf` output
  is repo-local, so it is not enough by itself for workspace-correct `--stat`.

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
- Workspace-correct `--stat`, `--numstat`, `--shortstat`, and
  `--patch-with-stat` until a GWZ stats renderer exists.

## CLI and client library plan

Add a client-side Rust library, tentatively `gwz-client`, for `gwz-cli` and
future Rust clients. Add parallel Python client work in `gwz-py`; do not assume
Python can directly reuse the Rust crate.

Suggested modules:

- `transport`: trait for local in-process core and future remote core clients.
- `diff_args`: Git-compatible parser for options and `--`; ambiguous operands
  remain raw for core.
- `diff_stream`: consumes the non-lossy output log and exposes bytes plus
  summary.
- `diff_render`: applies color, line prefix, and machine output formatting.
- `pager`: launches `less` or configured pager and writes streamed bytes.
- `exit_code`: maps `DiffSummary` and options to process status.

`gwz-cli` should become a thin shell:

1. Parse global GWZ options.
2. Delegate diff argument parsing to `gwz-client`.
3. Open local or remote GWZ core transport.
4. Start `diff.open` and read or tail the `diff.output` log.
5. Pipe output to stdout or pager.
6. Wait for final summary and exit according to `--exit-code`/`--quiet`.

`gwz-py` should add equivalent Python-side pieces:

1. Add generated protocol classes for `DiffRequest`, output log events, and
   summaries.
2. Add PyO3 bindings for opening, reading, waiting on, and cancelling diff
   output logs.
3. Add a Python argparse command for `gwz diff`.
4. Implement Python rendering, pager, JSON/JSONL, and exit-code behavior.
5. Keep Python CLI parity tests alongside existing `gwz-py` CLI parity tests.

Pager policy:

- Default to pager only for human patch output on a TTY.
- Never page `--json`, `--jsonl`, `--quiet`, or NUL-terminated output.
- Honor `--no-pager`, `GIT_PAGER`, `PAGER`, and a future GWZ config key.
- Pager failure is a client-side `external_tool_missing` or I/O error, not a
  core diff failure.
- Pager quit or stdout broken pipe must call the cancel path when the transport
  is remote or spooled.

Color policy:

- Default `auto`: color only when writing human patch output to a terminal or
  a pager that expects ANSI.
- `--color=always` and `--color=never` are client rendering decisions.
- The core stream remains canonical uncolored bytes.

## Implementation phases

### D-1 - Taut log transport and PyO3 bridge spike

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/taut/src/taut/ir/shapes.py`
- `/Users/owebeeone/limbo/gwz-dev/taut/src/taut/gen/`
- `/Users/owebeeone/limbo/gwz-dev/taut/src/taut/gen/runtime/`
- `/Users/owebeeone/limbo/gwz-dev/taut/docs/Reference.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/operation/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/lib.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/dispatch/mod.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/operations.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/bridge.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/client.py`

Work:

- Mature Taut `log` before freezing diff schema. The target is an
  operation-scoped append-only log with read-from-offset, bounded replay,
  tailing, explicit final event, and cancellation/teardown.
- Update generated client surfaces so `shape="log"` is not exposed as a generic
  subscribe-only stream. It should have log-specific read/replay, tail, and
  cancel/close affordances.
- Prove the log is independent from bounded `OperationRuntime` events and
  never emits `reset` for patch bytes.
- Define spool limits, cleanup, disconnect behavior, and cancellation semantics.
- Add or sketch Rust APIs for opening, reading, tailing, waiting on, and
  cancelling an output log.
- Add or sketch PyO3 bindings and bridge methods that expose the same ordered
  chunks to Python without routing through `subscribe_events`.
- Decide whether this is a diff-specific log or a generic operation-output log
  reusable by later commands.

Acceptance:

- A local Rust test can produce more chunks than the event buffer capacity and
  read every byte back through the Taut log cursor.
- A PyO3/Python test can read/tail ordered chunks and cancel early.
- Generated TypeScript and Python client stubs expose log-specific operations,
  not only a subscribe callback.
- A disconnected or slow consumer either resumes by offset, blocks production,
  or receives a typed capacity failure; bytes are not silently dropped.
- Blocking production on a full spool cannot deadlock the read path; producer
  and reader service must not require the same blocked execution slot.
- The final diff protocol proposal can be written against a proven transport
  log shape.

### D0 - Protocol design and corpus

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/corpus/`

Work:

- Add Taut enums and messages for diff request, options, output log events,
  open/log/cancel responses, and summary, using the D-1 Taut log result.
- Add `ActionKind.diff`.
- Add service entries for `diff`, `diff.open`, log-shaped `diff.output`, and
  cancel.
- Regenerate Rust and Python protocol outputs and golden corpus.
- Add compatibility tests for default values and unknown optional fields.
- Keep client-only process status out of the core request; do not include
  unsupported algorithm values such as histogram in v0.

Acceptance:

- Protocol generation is deterministic.
- Existing protocol corpus still passes.
- A minimal `DiffRequest` round trips through Taut.
- Diff output events can carry arbitrary bytes, including NUL bytes.
- Python generated protocol classes round trip the same diff messages.

### D1 - Core diff model and Git backend primitive

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/gitbackend.rs`
- New `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/diff/` or
  `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_diff.rs`

Work:

- Add core internal structs for `DiffComparison`, `DiffOptions`,
  `DiffRepoScope`, `DiffSummary`, and stream sink callbacks if generated types
  are too wire-oriented for internal use.
- Add `GitBackend::diff` or narrower primitives that produce a stream of
  chunks plus stats.
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
- Prove and implement the root exclusion strategy. Preferred: in-memory
  exclusions for active member paths, `.gwz/`, and `gwz.conf/.tmp/` when diffing
  the root repository. Fallback: fail fast when the managed exclude boundary is
  stale or cannot be validated safely.

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
  `gwz.conf/.tmp/` even when `.git/info/exclude` is stale, or else returns the
  documented boundary validation error.

### D3 - Core streaming handler

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/operation/`
- New diff handler module.

Work:

- Use the D-1 non-lossy Taut log; do not write patch bytes to
  `events.subscribe`.
- Implement `handle_diff_summary` for `diff`.
- Implement `diff.open`, `diff.output` log replay/tail, and `diff.cancel`
  handlers according to the Taut log contract.
- Emit `started`, `repo_started`, `chunk`, `repo_finished`, and `finished`
  events.
- Count streamed bytes and aggregate `DiffStats` across repos.
- For `summary_only`, compute summaries without emitting patch chunks.
- For `--name-only`, `--name-status`, and `--raw`, stream only the requested
  format. Keep stats formats out of Tier 0 until D4 proves a workspace-correct
  stats renderer.

Acceptance:

- A slow log consumer cannot lose patch chunks.
- Cancelling a log tail or producer stops diff generation and releases spool
  resources.
- Final summary is correct when the stream contains multiple repos.
- Errors in one member are member-scoped and respect partial policy.
- `operation.result` remains small and metadata-only.

### D4 - Patch rendering and path rewriting

Touchpoints:

- `git2::Diff::print`
- `git2::Diff::foreach`

Work:

- First attempt: use libgit2 prefix options for patch, raw, name-only, and
  name-status.
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
  ambiguous operands to core.
- Keep unsupported Git options as clear `invalid_request` errors.
- Implement stdout/pager streaming through the non-lossy output log.
- Implement client-side color.
- Implement `--exit-code` and `--quiet`.
- Update the PyO3 native dispatch registry, native operation/output-log
  storage, and Python bridge protocol so `diff.open`, `diff.output`, and
  cancellation are callable from Python.
- Add JSON/JSONL behavior:
  - `--json`: emit final `DiffResponse` metadata and summary, not patch bytes.
  - `--jsonl`: emit stream events as JSONL only after verifying how Taut JSON
    serializes `BYTES`; document base64 expansion if that is the encoding.
- Add equivalent `gwz-py` client and CLI support instead of assuming Rust
  client crate reuse.

Acceptance:

- `gwz diff` streams patch text to stdout.
- `gwz diff | cat` does not launch a pager.
- `gwz diff` on a TTY uses pager unless disabled.
- `gwz diff --quiet --exit-code` emits no patch and exits 1 on differences.
- `gwz --json diff --quiet` returns summary metadata.
- Python `gwz diff` can stream, cancel, render JSON/JSONL, and return the same
  exit status as the Rust CLI for covered cases.

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
- Document streaming protocol and JSON/JSONL behavior.
- Document unsupported/deferred Git options.
- Document Python CLI/API parity and any intentional differences.
- Add troubleshooting entries for missing refs in some members and
  unmaterialized selected members.

Acceptance:

- Generated CLI reference includes `diff`.
- Machine output docs include `DiffRequest`, `DiffResponse`, and stream event
  examples.
- Protocol docs explain that patch bytes are streamed and final results are
  metadata-only.

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
- `--binary`.
- `--text`.
- `--find-renames`, `--no-renames`, and a rename threshold.
- `-w`, `-b`, `--ignore-space-at-eol`, and `--ignore-blank-lines`.
- `--src-prefix`, `--dst-prefix`, `--no-prefix`, and `--line-prefix`.
- `--exit-code`.
- `--quiet`.

### Tier 1 - near follow-up

- `--diff-filter`.
- `--stat`, `--numstat`, and `--shortstat` after a GWZ stats renderer or proven
  path-rewrite strategy exists.
- `--summary`.
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

Workspace integration tests:

- Root-only diff.
- Member-only diff.
- Root plus multiple members in deterministic order.
- Selection by member id and member path.
- Cwd-relative pathspecs from root, member, and nested subdirectory.
- Missing ref in one member with and without partial policy.
- Unmaterialized fan-out member skipped; explicit unmaterialized member errors.

Log/streaming tests:

- Large patch emits multiple ordered chunks.
- Chunk bytes survive NUL records.
- Slow consumer does not lose chunks.
- Consumer can resume from a byte offset after reconnect.
- Cancellation stops production and cleans up log resources.
- Final `DiffSummary` matches accumulated repo summaries.
- `--quiet` emits no chunk events but still returns correct summary.

CLI/client tests:

- Pager is not launched for pipes, JSON, JSONL, quiet, or no-pager.
- Pager is launched for TTY human patch output.
- Color auto/always/never is client-side.
- `--exit-code` returns 1 on differences and 0 when clean.
- Plain `gwz diff` returns 0 with differences.
- Broken pipe or pager quit cancels a remote/spooled log.

Python client tests:

- PyO3 diff log binding returns ordered chunks without using
  `subscribe_events`.
- Python `gwz diff` handles the same Tier 0 parser cases as Rust for covered
  options.
- Python and Rust clients agree on `--exit-code`, `--quiet`, JSON summary, and
  JSONL stream behavior.

Golden output tests:

- Stabilize a small fixture workspace with root plus two members.
- Compare selected `gwz diff` outputs against checked-in goldens.
- Keep goldens focused on path headers and sequencing rather than every hunk
  detail from libgit2 if libgit2 version drift can legitimately change hunk
  boundaries.

## Open decisions

- Should the final implemented log be diff-specific (`diff.output`) or a
  generic operation output log? This must be decided in D-1 before D0 freezes
  schema and corpus.
- Should root exclusion be implemented through lower-level libgit2 filtering, a
  filtered GWZ renderer/stat path, or fail-fast validation of the managed
  exclude boundary? D2 must prove one safe strategy.
- For Tier 1, should `gwz diff --stat` use libgit2's stat renderer plus path
  prefixing, or a GWZ stat renderer over per-file stats? This depends on
  whether libgit2 exposes enough path detail for correct workspace path output.
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
- Patch bytes use the Taut log shape without lossy buffering.
- The log has cancellation and a PyO3/Python consumer path that does not use
  lossy operation events.
- Core contains no pager or terminal presentation logic.
- `gwz-cli` consumes the client-side API for pager, color, and exit status.
- `gwz-py` has equivalent protocol, stream, parser, rendering, and exit-code
  behavior for the supported surface.
- JSON/JSONL behavior is documented and tested.
- The protocol corpus includes diff request, stream event, and final response
  examples.
