# GWZ diff D0 protocol proposal

Status: **FROZEN — D0 committed 2026-07-04 ("Prep for gwz diff impl").**
Maintainer rulings recorded in §12. Schema changes from here follow the
append-only / reserved-tag discipline.

This document is the concrete D0 protocol proposal for `gwz diff`: the full
message/enum sketches (every field, tag, optionality, comment) in
`gwz.taut.py` DSL form, the `diff`/`diff.output` service entries, how
`diff.output` binds to the taut-shape `log` contract, the cache-key design, the
corpus-example catalog, the operand-parsing/translation rules for snapshot
forms, the rename/copy policy, the error taxonomy, and an explicit
**OPEN QUESTIONS** list.

It refines the "Taut protocol proposals" section of
[`GwzDiffPlan.md`](GwzDiffPlan.md) (the authoritative plan) into a
freeze-candidate schema and its acceptance corpus. Where this document and the
plan differ in shape, the plan wins on intent; this document proposes the exact
wire form. No schema changes are applied here — the DSL below is a proposal for
`gwz-core/protocol/gwz.taut.py`, not an edit of it.

Read against:

- [`GwzDiffPlan.md`](GwzDiffPlan.md) — the user's working plan (align, do not
  contradict).
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py` — style +
  existing vocabulary (`RequestMeta`, `ResponseMeta`/`ResponseEnvelope`,
  `ActionKind`, `Selection`, `SourceKind`, `TargetKind`, `GwzErrorCode`,
  `OperationEvent`, the `events.subscribe` `shape="log"` precedent).
- `/Users/owebeeone/limbo/taut-dev/taut-shape/dev-docs/TautClientImplPlan.md`
  — the `log` contract, decisions D1–D23.
- `/Users/owebeeone/limbo/taut-dev/taut-shape/dev-docs/TautShapeArchitecture.md`
  §4 — the `log` delivery-contract design summary.
- `/Users/owebeeone/limbo/taut-dev/taut-shape/ir/shape_log.taut.py` — the
  generated `Log*` wire vocabulary (`LogRecord.payload = BYTES`, `LogReadRequest`,
  `LogReadResponse`, `LogState`, …).

## 0. Design invariants (what D0 must hold)

These follow the plan's architecture decisions (AD1–AD11) and the log contract.
They are the acceptance criteria this schema is measured against.

1. **Two methods only.** One request/response planning method `diff`, one
   `shape="log"` byte-bearing method `diff.output`. No `diff.file`, no
   `diff.files`, no diff-specific read-window/cancel method (plan §"Service
   methods").
2. **Log side is already solved.** `diff.output` is a `shape="log"` method whose
   append type is `DiffOutputRecord`. Patch bytes ride `DiffOutputRecord.data`
   (`BYTES`, NUL-safe). `DiffOutputRecord` is the payload of the generic
   `LogRecord` (taut-shape **D17**). Chunking, cursor precision, tailing, EOF,
   close/cancellation, backpressure, and retention are taut-shape D-rules — this
   schema adds **no** `max_bytes`/cursor/close fields to any diff message. It
   cites the log contract; it does not re-invent it.
3. **Parsed flags are first-class fields, never operand tunnels.** `--cached` /
   `--staged` → `DiffRequest.cached`; `--merge-base` → `DiffRequest.merge_base`.
   `A...B` is parsed from `operands` and lowered to `DiffComparison.merge_base`
   per target. No parsed flag hides in `operands` (plan §"Request messages").
4. **Core owns operand classification.** The client keeps raw operands before
   `--` and explicit pathspecs after `--`; core classifies rev-vs-path per target
   repo (AD9). `+<snapshot_id>` operands are core-resolved and never handed to
   Git.
5. **Scope-addressable, reusable targets.** `DiffParsedTarget` is scoped to
   exactly one root/member repo, carries the resolved comparison, resolved oids,
   and `left_snapshot_id`/`right_snapshot_id`. The output renderer reuses it by
   scope without reclassifying raw operands.
6. **Manifest entries referenceable without path-parsing `file_id`.**
   `DiffFileEntry.file_id` is opaque; identity is
   `scope`/`status`/`old_path`/`new_path`. Output records correlate by
   `scope`+`file_id`(+`entry`), never by parsing `file_id`.
7. **Renames stay renames.** A rename entry carries **both** `old_path` and
   `new_path` plus `similarity`; the renderer must emit rename headers and must
   not degrade a rename into add/delete. `find_copies=true` is a **typed hard
   error** (`unsupported_operation`) in v0.
8. **`any_difference` + `stale_file`.** `DiffManifestMode.any_difference` backs
   `--quiet` (early-exit, no similarity, `files` omitted). Worktree races surface
   as a `stale_file` output record, never a silently-different patch.
9. **Core stays headless.** No pager/color/`less`/`PAGER`/exit-code fields in the
   core protocol (AD3, AD6). No unsupported algorithm values (no `histogram`) in
   v0 — the `git2 0.21` wrapper has no histogram setter (plan §"Libgit2
   coverage").
10. **Metadata-only final result.** `operation.result` never carries
    whole-workspace patch text (AD5); patch bytes live only in the `diff.output`
    log.

---

## 1. Service methods (proposed additions to `service("GwzCore", …)`)

```python
    # Plan the workspace diff: resolve targets/operands/pathspecs per repo and
    # return a changed-file manifest, aggregate + per-repo summary, scoped
    # operand classification (DiffParsedTarget), intentionally excluded targets,
    # and — unless the requested mode has no byte output — a DiffOutputLogRef.
    method("diff", role="in",
           params=Params(request=Ref.DiffRequest),
           out=Ref.DiffManifestResponse),

    # Read exact diff output records. shape="log": the append type is
    # DiffOutputRecord, delivered as the payload of the generic taut-shape
    # LogRecord (D17). log_id comes from DiffManifestResponse.output.log_id.
    # Cursor/tail/EOF/close/backpressure/retention are the shape_log contract
    # (D8/D9/D10/D12/D14/D7) — NOT fields here. `params` is the log_id handle;
    # the generated log shape supplies stream_id/cursor/limits/timeout.
    method("diff.output", role="out", shape="log",
           params=Params(log_id=STR),
           out=Ref.DiffOutputRecord),
```

Notes on the binding:

- The `params` of `diff.output` is only the opaque `log_id`. Everything a reader
  additionally passes — `stream_id`, `cursor`, `max_records`/`max_bytes`,
  `timeout_ms` — is the `shape_log` `LogReadRequest` surface (taut-shape §3.2,
  **D3/D4/D5/D8/D10/D14**), supplied by the generated log shape, not by this
  schema. This mirrors the existing `events.subscribe` `shape="log"` precedent
  in `gwz.taut.py` (which passes only `operation_id`).
- `out=Ref.DiffOutputRecord` is the method's append/out binding: it declares what
  `LogRecord.payload` decodes to (**D17**, "the method's `out=` binding declares
  what the payload decodes to"). Core CBOR-encodes each `DiffOutputRecord` and
  `LogPush`es it; the reader decodes `LogRecord.payload` back into a
  `DiffOutputRecord`.
- `ActionKind.diff` is added (see §2). The `diff` method's `ResponseMeta.action`
  is `ActionKind.diff`.

---

## 2. Enum additions

Appended after the existing `list_snapshots=20` value in `ActionKind`, then new
diff enums. Tag/value discipline follows the file: enums start at `0`, values are
dense and append-only.

```python
    # ActionKind gains diff. Existing values 0..20 unchanged; diff appended.
    # (Shown as the single added member; it is inserted into the existing
    #  ActionKind=Enum(...) block, not a new enum.)
    #     diff=21

    # Which two sides libgit2 compares, resolved per target repo from operands
    # and the cached/merge_base request flags.
    DiffComparisonKind=Enum(
         # git diff: index -> worktree. diff_index_to_workdir.
         worktree_vs_index=0,
         # git diff --cached [<commit>]: tree -> index. diff_tree_to_index.
         index_vs_tree=1,
         # git diff <commit>: tree -> worktree (index data for staged deletes).
         # diff_tree_to_workdir_with_index.
         worktree_vs_tree=2,
         # git diff <a> <b> / <a>..<b> / <a>...<b>: tree -> tree.
         # diff_tree_to_tree.
         tree_vs_tree=3),

    # Requested rendering of the diff. Metadata-only forms are answered from the
    # manifest with no diff.output read (name_only..summary, no_patch). Byte
    # forms are read from diff.output. Histogram is deliberately absent (the
    # git2 0.21 wrapper has no setter); adding it is an unsupported_operation.
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
         # Manifest/summary only; used by --quiet and JSON metadata modes.
         no_patch=10),

    # How much manifest work core does. full builds the whole file list + stats;
    # any_difference backs --quiet: stop at the first delta, skip similarity,
    # omit files, return only enough summary for the exit-code decision (AD8).
    DiffManifestMode=Enum(
         full=0,
         any_difference=1),

    # Diff algorithm. NO histogram in v0 (unsupported by the wrapper). Requesting
    # an unsupported algorithm is a typed unsupported_operation error, not a
    # silent downgrade.
    DiffAlgorithm=Enum(
         default=0,
         myers=1,
         minimal=2,
         patience=3),

    # Whitespace handling, mapped to libgit2 ignore_whitespace* / ignore_blank_lines.
    DiffWhitespaceMode=Enum(
         default=0,
         ignore_all=1,
         ignore_change=2,
         ignore_eol=3,
         ignore_blank_lines=4),

    # Per-file change classification. copied is reserved for a later copy project;
    # v0 never emits it (find_copies=true is rejected) but the value is defined so
    # the enum need not change when copy support lands.
    DiffStatus=Enum(
         added=0,
         modified=1,
         deleted=2,
         renamed=3,
         copied=4,
         type_changed=5,
         unmerged=6),

    # Byte encoding advertised on the output log. utf8 for text-only outputs;
    # bytes for patch/binary. JSON/JSONL transports base64-expand BYTES (D-1
    # spike; documented in D5), which is a transport concern, not a schema field.
    DiffChunkEncoding=Enum(
         utf8=0,
         bytes=1),

    # The DiffOutputRecord discriminator. patch_bytes carries data; the boundary
    # kinds (file_started/file_finished) are ALWAYS emitted, in every output
    # format (ruling #4, §12), so machine consumers can frame per-file output
    # without parsing patch text; stale_file marks a worktree race (D8 invariant
    # below); diagnostic carries a non-fatal message.
    DiffOutputRecordKind=Enum(
         patch_bytes=0,
         file_started=1,
         file_finished=2,
         stale_file=3,
         diagnostic=4),

    # Why a candidate target was excluded before diff execution (snapshot
    # narrowing). Reported in DiffManifestResponse.excluded_targets so a member
    # added after a snapshot was captured is explained rather than silently
    # dropped.
    DiffTargetExclusionReason=Enum(
         # Snapshot operand does not contain this member (commonly: member added
         # after the snapshot was captured).
         snapshot_missing=0,
         # Snapshot contains this member but records no Git commit for it.
         snapshot_missing_commit=1,
         # v0 snapshots do not record a workspace-root commit.
         root_not_in_snapshot=2),
```

No `less`/`pager`/color enums are added to core (AD3/AD6).

---

## 3. Request messages

```python
    # One resolved comparison for one target repo. kind + resolved endpoints.
    # left/right are the raw revision tokens as classified for THIS repo; the
    # resolved object ids live on DiffParsedTarget. merge_base covers both
    # --merge-base and the A...B form after per-repo operand resolution.
    DiffComparison=Msg(
        kind=F(1, Ref.DiffComparisonKind),
        # Left/old-side revision token, interpreted inside each target repo.
        left=F(2, STR, optional=True),
        # Right/new-side revision token, interpreted inside each target repo.
        right=F(3, STR, optional=True),
        # Use merge-base(left, HEAD-or-right) as the old side. Set for
        # --merge-base and for A...B after core lowers the range per repo.
        merge_base=F(4, BOOL, optional=True)),

    # git2 diff option knobs. All optional; absent = libgit2/GWZ default. These
    # affect which bytes core emits, so every client must agree on them (they are
    # protocol, not client-local). Pager/color/exit-code are deliberately NOT here.
    DiffOptions=Msg(
        output_format=F(1, Ref.DiffOutputFormat, optional=True),
        context_lines=F(2, INT, optional=True),
        interhunk_lines=F(3, INT, optional=True),
        algorithm=F(4, Ref.DiffAlgorithm, optional=True),
        whitespace=F(5, Ref.DiffWhitespaceMode, optional=True),
        find_renames=F(6, BOOL, optional=True),
        # Deferred. v0 REJECTS this with unsupported_operation until a copy
        # project can reproduce copy source sets and render copy headers.
        find_copies=F(7, BOOL, optional=True),
        rename_threshold=F(8, INT, optional=True),
        rename_limit=F(9, INT, optional=True),
        binary=F(10, BOOL, optional=True),
        text=F(11, BOOL, optional=True),
        full_index=F(12, BOOL, optional=True),
        abbrev=F(13, INT, optional=True),
        reverse=F(14, BOOL, optional=True),
        # -z / NUL-terminated name output. Byte fidelity is a diff.output/log
        # concern (D11 NUL-safety); this flag only selects the format.
        null_terminated=F(15, BOOL, optional=True),
        src_prefix=F(16, STR, optional=True),
        dst_prefix=F(17, STR, optional=True),
        no_prefix=F(18, BOOL, optional=True),
        line_prefix=F(19, STR, optional=True),
        ignore_submodules=F(20, STR, optional=True),
        diff_filter=F(21, STR, optional=True),
        # full vs any_difference (--quiet). Absent = full.
        manifest_mode=F(22, Ref.DiffManifestMode, optional=True),
        # Opt-in echo of each changed file's manifest entry on the matching
        # DiffOutputRecord (DiffOutputRecord.entry). Absent/false = correlate by
        # scope+file_id only; true = core populates entry so a streaming consumer
        # need not hold the whole manifest. Default off (ruling #3, §12).
        echo_manifest_entries=F(23, BOOL, optional=True)),

    # The diff planning request. Parsed comparison flags are FIRST-CLASS fields
    # (cached, merge_base); raw ambiguous operands stay in operands for per-repo
    # core classification; explicit pathspecs after `--` stay separate.
    DiffRequest=Msg(
        meta=F(1, Ref.RequestMeta),
        # Workspace-relative logical cwd (AD10): "", "gwz-core", "gwz-core/src".
        # Relative path operands resolve against this, NOT a client abspath.
        workspace_cwd=F(2, STR, optional=True),
        # Positional tokens before `--`. Core classifies rev-vs-path per target
        # repo (AD9). A token of the form +<snapshot_id> is a GWZ snapshot
        # operand, resolved by core, never passed to Git.
        operands=F(3, List(STR)),
        # Pathspecs after `--`; resolved relative to workspace_cwd. A leading
        # `+` here is a literal path, never a snapshot operand.
        explicit_pathspecs=F(4, List(STR)),
        options=F(5, Ref.DiffOptions, optional=True),
        # --cached / --staged. Selects index-vs-tree forms. First-class, not an
        # operand tunnel.
        cached=F(6, BOOL, optional=True),
        # --merge-base. First-class. The A...B syntax is still parsed from
        # operands and lowered to DiffComparison.merge_base per repo.
        merge_base=F(7, BOOL, optional=True)),
```

Rationale echoes the plan: the core API is structured, not a `git diff` argv
tunnel; the client parser can be Git-compatible for options and the `--` boundary
while the wire stays stable and language-neutral (plan §"Request messages").

---

## 4. Manifest, summary, and output messages

```python
    # Which repo an entry/target/record belongs to. root xor (member_id +
    # member_path). source_kind reuses the existing SourceKind enum.
    DiffRepoScope=Msg(
        # True for the workspace root repository.
        root=F(1, BOOL, optional=True),
        member_id=F(2, STR, optional=True),
        member_path=F(3, STR, optional=True),
        source_kind=F(4, Ref.SourceKind, optional=True)),

    # A candidate target intentionally excluded before diff execution, with the
    # reason and (for snapshot narrowing) the snapshot id that caused it. Required
    # so a member absent from a referenced snapshot is explained, not dropped.
    DiffExcludedTarget=Msg(
        scope=F(1, Ref.DiffRepoScope),
        reason=F(2, Ref.DiffTargetExclusionReason),
        # Snapshot operand that caused the exclusion, without the leading `+`.
        snapshot_id=F(3, STR, optional=True),
        message=F(4, STR, optional=True)),

    # A scope-addressable, reusable per-repo classification of the request.
    # The output renderer reuses this by scope/target_id WITHOUT reclassifying
    # raw operands (design invariant 5). Resolved oids are populated where a side
    # has one; worktree sides omit an oid. Snapshot-derived sides preserve their
    # snapshot ids.
    DiffParsedTarget=Msg(
        # Stable within the manifest; scoped to exactly one root/member repo.
        target_id=F(1, STR),
        scope=F(2, Ref.DiffRepoScope),
        # Resolved by core per repository from operands, cached/merge_base, and
        # pathspecs.
        comparison=F(3, Ref.DiffComparison),
        # Repo-relative pathspecs after workspace routing (member prefix stripped).
        pathspecs=F(4, List(STR)),
        # Resolved object ids where available. Worktree sides may omit an oid.
        left_oid=F(5, STR, optional=True),
        right_oid=F(6, STR, optional=True),
        merge_base_oid=F(7, STR, optional=True),
        # Present when a side came from a GWZ snapshot operand such as +start.
        left_snapshot_id=F(8, STR, optional=True),
        right_snapshot_id=F(9, STR, optional=True)),

    # One changed file, workspace-relative. file_id is opaque (never parsed as a
    # path); scope/status/old_path/new_path are the structured identity. Rename
    # entries carry BOTH paths + similarity and must not degrade to add/delete.
    DiffFileEntry=Msg(
        file_id=F(1, STR),
        scope=F(2, Ref.DiffRepoScope),
        status=F(3, Ref.DiffStatus),
        # Workspace-relative. new_path == old_path except for rename/copy.
        old_path=F(4, STR, optional=True),
        new_path=F(5, STR, optional=True),
        old_mode=F(6, INT, optional=True),
        new_mode=F(7, INT, optional=True),
        # 0..100 similarity for rename/copy entries.
        similarity=F(8, INT, optional=True),
        insertions=F(9, INT, optional=True),
        deletions=F(10, INT, optional=True),
        is_binary=F(11, BOOL, optional=True)),

    # Per-repo rollup. files_changed counts changes before GWZ root/member
    # filtering where libgit2 reports it cheaply; files_manifested counts entries
    # actually surfaced after pathspec/selection/root-exclusion filtering.
    DiffRepoSummary=Msg(
        scope=F(1, Ref.DiffRepoScope),
        has_differences=F(2, BOOL),
        files_changed=F(3, INT),
        insertions=F(4, INT),
        deletions=F(5, INT),
        files_manifested=F(6, INT)),

    # Workspace aggregate. has_differences drives the client --exit-code/--quiet
    # decision (AD8); it is a client contract, not the core aggregate_status.
    DiffSummary=Msg(
        has_differences=F(1, BOOL),
        repos_examined=F(2, INT),
        repos_with_differences=F(3, INT),
        files_changed=F(4, INT),
        insertions=F(5, INT),
        deletions=F(6, INT),
        repo_summaries=F(7, List(Ref.DiffRepoSummary))),

    # The opaque handle + advertised shape of the byte output log. log_id is the
    # taut-shape log handle (D3): holding it is the authority to read diff.output.
    # format/encoding tell the client what the bytes are. NO cursor/close/max_*
    # fields — those are the shape_log LogReadRequest surface (D8/D10/D14).
    DiffOutputLogRef=Msg(
        log_id=F(1, STR),
        format=F(2, Ref.DiffOutputFormat),
        encoding=F(3, Ref.DiffChunkEncoding, optional=True)),

    # The `diff` response. Metadata + manifest + scoped targets + excluded
    # targets + an optional output-log ref (omitted for no-byte modes). Reuses
    # the existing ResponseEnvelope (meta/members/errors).
    DiffManifestResponse=Msg(
        response=F(1, Ref.ResponseEnvelope),
        files=F(2, List(Ref.DiffFileEntry)),
        summary=F(3, Ref.DiffSummary, optional=True),
        # Scope-addressable per-repo operand classification resolved by core.
        targets=F(4, List(Ref.DiffParsedTarget)),
        # Omitted when the requested mode has no patch/byte output.
        output=F(5, Ref.DiffOutputLogRef, optional=True),
        # Candidates intentionally excluded before diff execution (snapshot
        # narrowing). Absent-from-snapshot members appear here.
        excluded_targets=F(6, List(Ref.DiffExcludedTarget))),

    # The append/out type of the diff.output log: the payload of the generic
    # taut-shape LogRecord (D17). Patch bytes ride `data` (BYTES, NUL-safe).
    # Boundary/stale/diagnostic kinds let machine consumers frame per-file output
    # without parsing patch text. Correlation is scope+file_id(+entry); file_id
    # is NEVER parsed as a path. There is NO cursor/EOF/close field here — the log
    # engine owns all of that (D8/D9/D12/D14).
    DiffOutputRecord=Msg(
        kind=F(1, Ref.DiffOutputRecordKind),
        # Structured correlation for file-scoped records (file_started/
        # file_finished/patch_bytes/stale_file).
        scope=F(2, Ref.DiffRepoScope, optional=True),
        file_id=F(3, STR, optional=True),
        # Echo of the manifest entry so a streaming consumer need not hold the
        # whole manifest to interpret a record. Populated only when the request
        # sets DiffOptions.echo_manifest_entries (ruling #3, §12); absent by
        # default (correlate by scope+file_id).
        entry=F(4, Ref.DiffFileEntry, optional=True),
        # patch_bytes payload. Exact bytes, including NULs and binary hunks.
        data=F(5, BYTES, optional=True),
        # True on a stale_file record: the planned entry could not be rendered
        # because the worktree changed before output materialization.
        stale=F(6, BOOL, optional=True),
        diagnostic=F(7, STR, optional=True)),
```

---

## 5. How `diff.output` binds to the taut-shape `log` contract

This is the load-bearing "do not re-invent the log" section. Everything
transport-ish is cited to a D-rule and lives in `shape_log`, not in diff.

### 5.1 The binding

- `diff.output` is `role="out", shape="log"` with `out=Ref.DiffOutputRecord`.
- Per taut-shape **D17**: `LogRecord.payload = BYTES` carries the method's
  append-slot message **already taut-encoded**. So each diff output record is a
  `DiffOutputRecord`, CBOR-encoded, wrapped as `LogRecord{seq, payload}`. The
  method's `out=` binding declares that `payload` decodes to `DiffOutputRecord`.
- Patch bytes ride `DiffOutputRecord.data` (`BYTES`). This is the doubly-nested
  byte path — patch bytes inside a `DiffOutputRecord` inside a `LogRecord.payload`
  — and it is NUL-safe end to end (CBOR major-type-2, **D11**, proven across
  Rust/Py/TS in taut-shape).
- `DiffOutputLogRef.log_id` is the opaque log handle (**D3**): "holding the ref is
  the authority to read." The client passes it as the `log_id` param of
  `diff.output`; the generated log shape supplies `stream_id`, `cursor`,
  `max_records`/`max_bytes`, `timeout_ms` (the `LogReadRequest` surface, §3.2).

### 5.2 What diff does NOT define (cited, not re-invented)

| Concern | Owned by shape_log | D-rule |
|---|---|---|
| Cursor model, resume without dup/skip | `LogCursor{seq}`, `next_cursor` always present; first record `seq=1`, `START={seq:0}` | D8 |
| Read sizing / backpressure | `LogReadRequest.max_records` / `max_bytes` (payload bytes only; ≥1-record forward-progress) | D10 |
| Tail vs probe vs hold | `LogReadRequest.timeout_ms` (absent=hold, `0`=probe→`would_block`, `>0`=hold+timer) | D14 |
| EOF vs "no records yet" | `LogState.eof` (sealed+drained) vs `would_block` | D12/D13 |
| Close / cancellation | `LogEndStream` (adapter injects on transport death); `LogClose` → `closed`/`failed` | D4/D12 |
| Producer-stop (pager quit / broken pipe / disconnect halts render) | `LogProducerStop{reason}` on `Close` and last-stream-end (`stop_when=last_reader`) | D6 |
| Retention / eviction | consumer-driven `LogEvict`; engine tracks per-stream watermarks | D7 |
| Invalid/expired cursor | `LogState.expired` (a state, never an error) with earliest-resumable `next_cursor` | D9 |
| Late push after terminal | dropped + `LogDiagnostic{warn, push_after_terminal}` | D18/D19 |

Diff contributes **only** `DiffOutputRecord` as the log record payload. It adds
no `max_bytes`-style field, no cursor field, no close flag. The plan's
§"Taut extension notes" required list is satisfied entirely by the shape_log
contract above.

### 5.3 Production model (diff side)

- `diff` plans the render and mints the `log_id`. Core `LogPush`es
  `DiffOutputRecord`s in **manifest order** (root first, then members in manifest
  order — plan §"Root and member ordering"), then `LogSeal`s at end of output so
  drained readers see `eof` (**D12**).
- Per-file framing is **always emitted, in every output format** (ruling #4, §12):
  `file_started` … `patch_bytes`* … `file_finished` per manifest entry, so machine
  consumers frame without parsing patch text (design invariant 6). It is not gated
  on `--jsonl` or a request flag.
- Worktree race: if a planned entry cannot be rendered because its input changed
  after `diff` accepted, core emits a `stale_file` `DiffOutputRecord`
  (`kind=stale_file, stale=true`) instead of a different patch (AD4, design
  invariant 8). The operation result is marked partial/diagnostic; the log itself
  is not `failed`.
- Backing store is the fingerprint-keyed cache of §6 (taut-shape architecture §5
  "Illustration (gwz)": render into a keyed cache and replay into per-operation
  logs). If bytes should not sit in the engine window, the designed additive
  `ScanRequest`/`ScanResult` extension (taut-shape §9) applies — not needed for
  v0.

---

## 6. Cache-key design (retained / cache-backed output, operation-scoped v0)

The `diff.output` log is backed by retained render state so re-reads and resumes
do not recompute per file, and so a worktree race is *detectable* rather than
silently rendered. The cache key is the whole reason a race can be caught: if any
input that can change bytes changes, the key changes. The key has five
components; v0 keeps the cache **operation-scoped and short-lived** (cross-command
reuse is not required to unblock `gwz diff`).

**1. Normalized request component.** The request reduced to a canonical form:
   - normalized `operands` (post-classification, per-target),
   - resolved target set (root + selected members, in manifest order),
   - repo-local pathspecs (after workspace routing, member prefix stripped),
   - `output_format`,
   - all `DiffOptions` that affect bytes (context/interhunk/algorithm/whitespace/
     rename settings/binary/text/full_index/abbrev/reverse/prefixes/line_prefix/
     ignore_submodules/diff_filter),
   - renderer version,
   - libgit2 version (hunk boundaries can drift across libgit2 builds).

**2. Immutable Git component.** Resolved tree/blob/object ids for every
   commit/tree/blob side, including snapshot-derived sides (from
   `DiffParsedTarget.left_oid`/`right_oid`/`merge_base_oid`). These are immutable
   once resolved, so they anchor the tree-vs-tree and `--cached`-vs-tree cases.

**3. Index component.** A **content fingerprint of the Git index** — the index
   checksum or a hash of the index file bytes — **NOT mtime-only**. Covers
   index-vs-tree and worktree-vs-index. mtime alone races on same-second edits.

**4. Worktree component.** Per candidate path that can affect the manifest or
   output: **file type, executable bit, size, and a content hash or Git blob id**.
   Stat-only fingerprints (type/mode/size/mtime) are permitted **only** as a
   performance optimization **paired with race detection** — a stat-match that
   later fails content verification during render becomes a `stale_file` record.

**5. Output component.** Output-log `format`, byte `encoding`, `src_prefix`/
   `dst_prefix`/`no_prefix`/`line_prefix`, and any renderer choice that changes
   emitted bytes.

Invalidation rule: the key MUST change when any file, index entry, or object that
can affect the output changes. A key hit means the retained render is still valid;
a miss means re-plan. During lazy render, a mutated input discovered after
planning yields a `stale_file` record (§5.3), never a silently-different patch.

Open cache questions are in §12.

---

## 7. Operand parsing & translation rules (snapshot + range forms)

The client keeps raw operands before `--` and explicit pathspecs after `--`; core
does all rev/path/snapshot classification per target repo (AD9). This section is
the normative translation table.

### 7.1 Comparison-form lowering (per target repo)

| CLI form | Request fields | Resolved `DiffComparison` |
|---|---|---|
| `gwz diff` | (none) | `worktree_vs_index` |
| `gwz diff --cached` / `--staged` | `cached=true` | `index_vs_tree`, `left=HEAD` (empty tree if unborn) |
| `gwz diff <commit>` | `operands=[<commit>]` | `worktree_vs_tree`, `left=<commit>` |
| `gwz diff <a> <b>` | `operands=[<a>,<b>]` | `tree_vs_tree`, `left=<a>`, `right=<b>` |
| `gwz diff <a>..<b>` | `operands=[<a>..<b>]` | `tree_vs_tree` (omitted side → `HEAD`) |
| `gwz diff <a>...<b>` | `operands=[<a>...<b>]` | `tree_vs_tree`, `merge_base=true` |
| `gwz diff --merge-base <commit>` | `merge_base=true`, `operands=[<commit>]` | `worktree_vs_tree`, `merge_base=true` |
| `gwz diff --cached <commit>` | `cached=true`, `operands=[<commit>]` | `index_vs_tree`, `left=<commit>` |
| `gwz diff --cached --merge-base <commit>` | `cached=true`, `merge_base=true`, `operands=[<commit>]` | `index_vs_tree`, `merge_base=true` |

Range parsing (`..`/`...`) happens **before** endpoint resolution, so `+base..+tip`
splits into two snapshot endpoints, never one id containing dots.

### 7.2 GWZ snapshot operands (`+<snapshot_id>`)

- A token before `--` of the form `+<snapshot_id>` is a snapshot reference; the
  stored id is the token without the leading `+`. Core reads
  `gwz.conf/snapshots/<snapshot_id>.yaml`, validates the artifact schema and
  workspace id, and resolves per member to that member's recorded
  `members.<member_id>.commit`. The recorded source kind must be Git and the
  commit must be present. The `+…` token is **never** passed to Git.
- Snapshot operands appear anywhere a commit/tree operand can: `+start`,
  `+base +tip`, `+base..HEAD`, `+base...main`.
- Provenance survives on `DiffParsedTarget.left_snapshot_id` /
  `right_snapshot_id` alongside the resolved `left_oid`/`right_oid`.
- **Root omission.** v0 snapshots do not record the workspace root. When a
  snapshot operand is present and selection is implicit, the candidate set narrows
  to members covered by **every** snapshot operand in the comparison. Root is
  excluded and reported in `excluded_targets` with `root_not_in_snapshot`.
  Explicit `--target @root` with a snapshot operand is a **typed error**
  (`invalid_request`).
- **Missing members.** A member absent from a referenced snapshot (e.g. added
  after capture) is not diffed; it is reported in `excluded_targets` with
  `snapshot_missing` (or `snapshot_missing_commit` if present-but-commitless). If
  the member was **explicitly** selected, that is a member-scoped typed error and
  global `--partial` decides whether other members continue.
- After `--`, a `+name` token is always a literal pathspec, never a snapshot
  operand.

### 7.3 Pathspec routing (summary; full rules in the plan)

Build candidates from GWZ selection (`CommandDefaultTargets::All`,
`RootSelectionPolicy::Allow`), then intersect pathspecs relative to
`workspace_cwd`: a member pathspec keeps only that member; a pathspec at/above a
member boundary fans out to candidate members; a root-owned pathspec keeps root
only if root is a candidate. Pathspecs **never add** a target excluded by explicit
selection. An empty-but-valid intersection is a clean no-diff result, not an
error. Member prefixes are stripped before libgit2. Escapes and pathspecs into
inactive/unmaterialized members are typed request errors.

---

## 8. Rename / copy policy

- **Renames are first-class and irreducible.** A rename manifest entry carries
  `status=renamed`, **both** `old_path` and `new_path`, and `similarity`. Output
  rendering must include both paths, preserve similarity detection, and emit
  matching rename headers (`similarity index`, `rename from`, `rename to`). A
  rename must **not** degrade into an add + delete pair during rendering (design
  invariant 7).
- Rename detection is `find_renames` + `rename_threshold`/`rename_limit` in
  `DiffOptions`, mapped to `Diff::find_similar` / `DiffFindOptions`.
- For explicit single-file requests, rename/copy detection is scoped to the
  filtered path set, matching `git diff -- <path>`.
- **Copies are rejected in v0.** `find_copies=true` is a **typed hard error**
  (`unsupported_operation`, with `GwzError.detail` naming the offending option
  `find_copies` — ruling #10, §9/§12), never a silent downgrade to add.
  `DiffStatus.copied` is defined for forward-compatibility but never emitted in v0.
  A later copy project must reproduce copy source sets (including unmodified
  sources when copy detection allows) before the value is used.

---

## 9. Error taxonomy

Reuse the existing `GwzError`/`GwzErrorCode` vocabulary; add no diff-only codes
unless the existing ones prove too coarse (plan D1: only add e.g.
`diff_target_not_found` if `git_command_failed`/`invalid_request`/
`member_not_found` are insufficient — deferred, not proposed here).

**Unsupported-option rejections reuse `unsupported_operation` with structured
detail (ruling #10, §12).** `find_copies=true` and unsupported algorithm values
are **not** given a new diff-specific code; they reuse
`GwzErrorCode.unsupported_operation`, and the returned `GwzError.detail` carries a
structured value that **names the offending option** (e.g. `find_copies`, the
requested algorithm) so a client can tell which knob was rejected.

| Condition | Code | Scope |
|---|---|---|
| `find_copies=true` (v0 rejects copy detection) | `unsupported_operation` (detail names `find_copies`) | request |
| Unsupported algorithm (e.g. histogram) requested | `unsupported_operation` (detail names the algorithm) | request |
| `--target @root` with a snapshot operand | `invalid_request` | request |
| Pathspec outside workspace / escape | `path_escape` | request |
| Explicit pathspec into inactive/unmaterialized member | `member_inactive` / `member_not_found` | member |
| Snapshot artifact not found for `+<id>` | `snapshot_not_found` | request/member |
| Explicit member missing from referenced snapshot | `member_not_found` (member-scoped) | member, honors `--partial` |
| Missing ref in a member (`<commit>` unresolved) | `git_command_failed` (member-scoped) | member, honors `--partial` |
| Unborn-repo `<commit>` form | `git_command_failed` (member-scoped) | member |
| libgit2 diff failure | `git_command_failed` | member |

**Log-side errors defer to shape_log.** Unknown `log_id` → `LogErrorCode.unknown_log`
(service-level, taut-shape §3.1); expired cursor → `LogState.expired` (a state,
never an error, **D9**); producer error → `LogState.failed` + `LogError`
(**D12**). Diff defines none of these; it cites them. There is no diff-specific
cancellation error: client cancel is `LogEndStream` (**D4**), which needs no
response.

Non-fatal render conditions ride the log, not the error list: worktree race →
`DiffOutputRecord{kind=stale_file}` + partial operation result; per-file
diagnostics → `DiffOutputRecord{kind=diagnostic}`.

---

## 10. Corpus-example catalog (required before D0 freezes)

Each example is a named corpus entry added to `gwz-core/protocol/corpus/` (the
`golden.json` byte-parity form + a Rust round-trip), proving one contract.
Generation must be deterministic and the **existing corpus must still pass**.

| # | Example name | Message(s) | What it proves |
|---|---|---|---|
| C1 | `DiffRequest_minimal` | `DiffRequest` | Minimal request (meta + empty operands) round-trips; defaults absent. |
| C2 | `DiffRequest_cached` | `DiffRequest` | `cached=true` is a first-class field (`--cached`/`--staged`), not an operand. |
| C3 | `DiffRequest_merge_base` | `DiffRequest` | `merge_base=true` (`--merge-base`) is a first-class field. |
| C4 | `DiffRequest_range_A_dotdotdot_B` | `DiffRequest` | `A...B` is parsed into `operands` and later lowered to `merge_base` — no parsed flag tunneled through operands. |
| C5 | `DiffOptions_full` | `DiffOptions` | Every option tag round-trips (incl. `echo_manifest_entries`); unknown-optional-field tolerance for future additions. |
| C6 | `DiffParsedTarget_scoped_member` | `DiffParsedTarget` | Scope-addressable target with resolved `left_oid`/`right_oid`, reusable without reclassifying operands. |
| C7 | `DiffParsedTarget_snapshot` | `DiffParsedTarget` | `left_snapshot_id`/`right_snapshot_id` preserved alongside resolved oids. |
| C8 | `DiffFileEntry_rename` | `DiffFileEntry` | Rename carries BOTH `old_path` and `new_path` + `similarity`; does not degrade to add/delete. |
| C9 | `DiffFileEntry_binary` | `DiffFileEntry` | `is_binary=true`, mode fields round-trip. |
| C10 | `DiffManifestResponse_full` | `DiffManifestResponse` | Manifest + summary + targets + `output` log ref, root-first ordering. |
| C11 | `DiffManifestResponse_any_difference` | `DiffManifestResponse` | `any_difference`/`--quiet` shape: `files` omitted, summary-only, `output` omitted. |
| C12 | `DiffManifestResponse_excluded_snapshot_member` | `DiffManifestResponse` + `DiffExcludedTarget` | A member absent from a snapshot is reported (`snapshot_missing`), not dropped. |
| C13 | `DiffExcludedTarget_root_not_in_snapshot` | `DiffExcludedTarget` | Root omission under a snapshot operand (`root_not_in_snapshot`). |
| C14 | `DiffOutputLogRef_basic` | `DiffOutputLogRef` | Opaque `log_id` + `format` + `encoding`; no cursor/close/max_* fields present. |
| C15 | `DiffOutputRecord_patch_bytes_with_nul` | `DiffOutputRecord` | `data` carries BYTES **including NUL bytes** and binary hunks (D11 NUL-safety). |
| C16 | `DiffOutputRecord_file_boundary` | `DiffOutputRecord` | `file_started`/`file_finished` (always-emitted, ruling #4) correlate by `scope`+`file_id` without parsing patch text; `entry` absent (echo opt-in, off). |
| C17 | `DiffOutputRecord_stale_file` | `DiffOutputRecord` | `kind=stale_file, stale=true` worktree-race record. |
| C18 | `DiffSummary_multi_repo` | `DiffSummary` | Aggregate + per-repo summaries across root + members; `has_differences` for exit-code. |
| C19 | `compat_defaults` | `DiffRequest`/`DiffOptions` | All-defaults instances decode to defaults (default-value compat). |
| C20 | `compat_unknown_optional_field` | `DiffOptions` | An instance with a higher unknown optional tag still decodes (forward compat). |
| C21 | `DiffOutputRecord_entry_echo` | `DiffOutputRecord` + `DiffFileEntry` | Opt-in echo (ruling #3): when `DiffOptions.echo_manifest_entries` is requested, `DiffOutputRecord.entry` carries the full manifest entry and round-trips. |

Plus, per plan D0 acceptance: a **minimal `DiffRequest` round-trips through Taut**
(C1); **Python generated classes round-trip** the same diff messages (each C-row
also exercised through the Python codegen).

---

## 11. Alignment / deltas vs `GwzDiffPlan.md`

- **Tag numbers:** all message tags match the plan's sketches exactly. The one
  numeric assignment the plan left open is `ActionKind.diff`; this document
  proposes `diff=21` (next dense value after `list_snapshots=20`). Final numbering
  is confirmed against the generated schema at implementation time (plan
  §"Taut protocol proposals").
- **`diff.output` params:** the plan sketched `params=Params(output_id=STR)` and
  `DiffOutputLogRef.output_id`. This document renames the handle to **`log_id`**
  to match the taut-shape `log` contract's canonical name (**D3**;
  `DiffOutputLogRef{log_id}` is the exact term used in TautShapeArchitecture §4).
  This is a naming alignment, not a semantic change — **CONFIRMED by ruling #1
  (§12); the plan's `output_id` is superseded** (that plan file is left unedited).
- Everything else (enums, request/manifest/output messages, semantics, ordering,
  routing, rename/copy policy) tracks the plan's proposals directly.

---

## 12. Resolved rulings

The 10 questions previously open here were **ruled by the maintainer on
2026-07-04**. Each ruling is final for D0; the body of this document has been
updated to match (the load-bearing changes are noted per ruling). The questions
are preserved for provenance, each followed by its ruling.

1. **`log_id` vs `output_id` naming.** *Q: The plan sketches `output_id`; this D0
   proposal uses `log_id` to match the taut-shape `log` contract's canonical
   handle name (D3). Confirm the rename, or keep `output_id`?*
   **Ruling (2026-07-04): `log_id` — CONFIRMED as implemented.** The taut-shape
   canonical handle name wins. `GwzDiffPlan.md`'s `output_id` is **superseded** by
   `log_id` (that plan file is left unedited; this ruling records the supersession).
   No schema change — the file already ships `log_id` on `DiffOutputLogRef` and as
   the `diff.output` param. §11's "flagged as an OPEN QUESTION" note is now closed.
2. **`ActionKind.diff` value.** *Q: Proposed `diff=21`. Confirm no reserved gap is
   intended in `ActionKind`.*
   **Ruling (2026-07-04): `diff=21` — CONFIRMED.** Dense append after
   `list_snapshots=20`, as already implemented in the schema.
3. **`DiffOutputRecord.entry` echo.** *Q: Keep the echo always, make it opt-in via
   a request flag, or drop it and force `scope`+`file_id` correlation only?*
   **Ruling (2026-07-04): OPT-IN via a request option, default off.** A new
   optional `BOOL` field **`DiffOptions.echo_manifest_entries`** is added
   (tag 23, append-only). When absent/false, `DiffOutputRecord.entry` is omitted
   and consumers correlate by `scope`+`file_id`; when true, core populates
   `entry`. The `DiffOutputRecord.entry` comment now reads "populated only when
   `DiffOptions.echo_manifest_entries` requested." (Body updated: §3 `DiffOptions`,
   §4 `DiffOutputRecord`, corpus catalog C-row for the echo case.)
4. **Boundary records default.** *Q: Emit `file_started`/`file_finished` always,
   only in machine (`--jsonl`) modes, or only on request?*
   **Ruling (2026-07-04): ALWAYS emitted, in every output format.** Boundary
   records are unconditional (not gated on `--jsonl` or a request flag), so machine
   consumers can always frame per-file output without parsing patch text. (Body
   updated: §5.3 production model and the `DiffOutputRecordKind` enum comment.)
5. **Cache key: stat-only worktree fingerprint.** *Q: Is stat+race acceptable for
   v0, or must v0 always content-hash candidate paths?*
   **Ruling (2026-07-04): stat + race-detection accepted for v0 — CONFIRMED.** A
   stat-only worktree fingerprint paired with render-time race detection (→
   `stale_file`) is sufficient for v0; content-hashing every candidate path is not
   required. (Matches §6 as written.)
6. **Cache scope.** *Q: Confirm no cross-command reuse is required for D0.*
   **Ruling (2026-07-04): operation-scoped / short-lived for D0 — CONFIRMED.** No
   cross-command cache reuse is required to unblock `gwz diff`. The §6 key already
   supports a future shared cache; TTL/eviction policy stays unspecified for D0.
7. **`diff.output` retention/chunk defaults.** *Q: Confirm chunk size / retention /
   backpressure *defaults* belong in D3 implementation, not the D0 schema.*
   **Ruling (2026-07-04): D3 implementation, never the D0 schema — CONFIRMED.**
   Chunk size, retention window, backpressure, and the diff-side engine defaults
   (e.g. `stop_when=last_reader`, evict-after-drain) are a D3 deployment choice.
   The D0 schema adds no such field.
8. **`diff_filter` as `STR`.** *Q: Typed set/enum, or validated raw string for v0?*
   **Ruling (2026-07-04): validated raw `STR` for v0 — CONFIRMED; typed revisit at
   D1.** `DiffOptions.diff_filter` stays a raw `STR` (Git's `AMDRCT…` letters),
   validated by core; a typed representation is revisited at D1, not D0.
9. **Snapshot root capture (future).** *Q: Confirm member-only snapshots forcing
   `root_not_in_snapshot` is acceptable for D0.*
   **Ruling (2026-07-04): member-only snapshots ⇒ `root_not_in_snapshot` —
   CONFIRMED.** v0 snapshots do not record the workspace root; root is excluded
   with `root_not_in_snapshot` (§2, §7.2). A future snapshot-schema change to
   capture the root commit is out of scope for D0.
10. **`unsupported_operation` reuse.** *Q: Reuse the existing
    `GwzErrorCode.unsupported_operation` for `find_copies=true`/unsupported
    algorithms, or add a diff-specific code?*
    **Ruling (2026-07-04): reuse `unsupported_operation` WITH structured detail
    naming the offending option — CONFIRMED; no new code.** No diff-specific
    `unsupported_option` code is added. The rejection carries a structured
    `GwzError.detail` that names the offending option (e.g. `find_copies`, the
    requested algorithm). (Body updated: §9 error taxonomy.)
