# GWZ diff plan — re-review with mandated restructure (Review48-3)

Reviewed plan:

- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md` (revised)

Prior reviews:

- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan-Review48.md`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan-Review48-2.md`

Re-review status: independent design re-review, 2026-06-27. Contains a
**mandatory architectural directive from the plan manager** (see below). All
prior findings from Review48-2 still stand except where superseded by the
directive.

## Status of the current plan

The revised plan is sound and Review48-2 confirmed it resolved the earlier
architectural risks. The remaining Review48-2 findings (orphaned
`DiffComparison`/`DiffParsedTarget`, empty `DiffOpenResponse.summary`, the
`shape="log"` hard-code, the AD11/spool exclusion coherence) are all real but are
now mostly **dissolved** by the directive below rather than fixed in place.

## MANDATORY DIRECTIVE (plan manager) — two-phase, file-granular diff

The diff protocol **must** be restructured into two phases. This is a
requirement, not an option, and the plan document must be revised to make it the
primary architecture:

1. **Phase 1 — discovery / manifest.** A request/response that returns the
   *list of files that differ* across the root and selected members, with
   per-file metadata (status, rename pairing, stat counts, binary flag, scope),
   plus the aggregate summary. No patch bytes.
2. **Phase 2 — per-file patch.** A separate request/response that returns the
   rendered patch for **one file** (or a small explicit batch), addressed from
   the phase-1 manifest.

Additionally:

3. **When the file(s) to diff are explicitly supplied, phase 1 is skipped.**
   A `gwz diff -- <literal file>` invocation already names its targets, so it is
   routed straight to phase 2 for those exact paths — mapped to owning repos
   exactly like every other GWZ command that takes file arguments. Phase-1
   discovery runs **only** when the changed set is unknown (bare `gwz diff`,
   directory pathspecs, or globs).

### Why this is mandated (short form)

The plan's current backbone is an unbounded patch byte stream, which is the sole
reason the AD4 spool, backpressure, offset addressing, `diff.cancel`, the new
PyO3 streaming binding, and the entire **D-1 transport spike**
(`GwzDiffPlan.md:151`–`179`, `:293`–`307`, `:714`–`747`) exist. A per-file
response is bounded by one file and fits ordinary request/response — the
transport the framework already provides. The two-phase split therefore removes
the single largest unproven risk in the plan and replaces it with mechanics GWZ
already ships (`call`/`submit` request/response, the same per-repo composition in
AD2 at `GwzDiffPlan.md:130`–`141`, and the literal path→repo routing the plan
already intends to extract from `stage_routing.rs`,
`GwzDiffPlan.md:608`–`611`).

It also matches how every programmatic diff consumer (IDEs, web review UIs, and
`gwz-py` as a library) actually wants the data: file list first, patches on
demand.

## Required protocol shape

The following replaces the `diff` / `diff.open` / `diff.output` / `diff.cancel`
method set for v0. Field numbers are illustrative; assign during D0.

### Service methods

```python
service("GwzCore",
    # Phase 1: discovery. Returns the changed-file manifest + summary. No bytes.
    method("diff", role="in",
           params=Params(request=Ref.DiffRequest),
           out=Ref.DiffManifestResponse),

    # Phase 2: per-file patch (bounded request/response).
    method("diff.file", role="in",
           params=Params(request=Ref.DiffFileRequest),
           out=Ref.DiffFileResponse),

    # Phase 2 (batched): optional, for remote round-trip amortization.
    method("diff.files", role="in",
           params=Params(request=Ref.DiffFilesRequest),
           out=Ref.DiffFilesResponse))
```

`diff.open`, `diff.output`, `diff.cancel`, the spool, `DiffStreamKind`,
`DiffOutputChunk` as a stream payload, and `DiffOpenResponse` are **removed from
v0**.

### Phase 1 — manifest

```python
DiffStatus=Enum(
    added=0, modified=1, deleted=2, renamed=3,
    copied=4, type_changed=5, unmerged=6)

DiffFileEntry=Msg(
    # Stable id used by phase 2 to address this exact delta.
    file_id=F(1, STR),
    scope=F(2, Ref.DiffRepoScope),
    status=F(3, Ref.DiffStatus),
    # Workspace-relative. new_path == old_path except for rename/copy.
    old_path=F(4, STR, optional=True),
    new_path=F(5, STR, optional=True),
    old_mode=F(6, INT, optional=True),
    new_mode=F(7, INT, optional=True),
    similarity=F(8, INT, optional=True),   # rename/copy %
    insertions=F(9, INT, optional=True),
    deletions=F(10, INT, optional=True),
    is_binary=F(11, BOOL, optional=True))

DiffManifestResponse=Msg(
    response=F(1, Ref.ResponseEnvelope),
    files=F(2, List(Ref.DiffFileEntry)),
    summary=F(3, Ref.DiffSummary, optional=True),
    # Per-repo operand classification — this is where the previously orphaned
    # DiffParsedTarget belongs (see Review48-2 finding 1).
    targets=F(4, List(Ref.DiffParsedTarget)))
```

Phase 1 satisfies, with **no phase-2 calls at all**: `--name-only`,
`--name-status`, `--stat`, `--numstat`, `--shortstat`, `--summary`, `--quiet`,
`--exit-code`, and all `--json` metadata (`GwzDiffPlan.md:343`–`354`, `:977`).
`summary_only` (`GwzDiffPlan.md:428`) is no longer a special flag — it is simply
"phase 1, do not call phase 2."

Note: phase 1 must run the full per-repo `Diff` including `find_similar`, because
rename/copy pairing is a whole-set operation, not a per-file one. The expensive
work (diff + similarity + stats) happens once here; phase 2 is cheap rendering.

### Phase 2 — per-file patch

```python
DiffFileRequest=Msg(
    meta=F(1, Ref.RequestMeta),
    # Comparison context. Either echo the phase-1 operation id (stateful reuse)
    # or re-send the resolved comparison for stateless recompute. The plan must
    # pick one; stateless recompute is the recommended default.
    workspace_cwd=F(2, STR, optional=True),
    scope=F(3, Ref.DiffRepoScope),
    file_id=F(4, STR, optional=True),     # from the manifest
    path=F(5, STR, optional=True),        # for the explicit-file fast path
    options=F(6, Ref.DiffOptions, optional=True),
    operands=F(7, List(STR)),             # comparison operands, as in DiffRequest
    # Oversized-file fallback: read a byte window of this one file's patch.
    from_offset=F(8, INT, optional=True),
    max_bytes=F(9, INT, optional=True))

DiffFileResponse=Msg(
    response=F(1, Ref.ResponseEnvelope),
    scope=F(2, Ref.DiffRepoScope),
    file_id=F(3, STR, optional=True),
    format=F(4, Ref.DiffOutputFormat),
    encoding=F(5, Ref.DiffChunkEncoding),
    data=F(6, BYTES),                     # this file's patch, workspace-relative
    truncated=F(7, BOOL, optional=True),  # true if total exceeds the cap
    total_bytes=F(8, INT, optional=True), # for resuming via from_offset
    next_offset=F(9, INT, optional=True))
```

`diff.files` is the same with `List` request/response for round-trip
amortization on a remote core.

### The only residual streaming surface

A single pathologically large file (e.g. a vendored/minified/generated blob)
still cannot fit one response. That is handled by `DiffFileResponse.truncated`
plus the `from_offset`/`max_bytes` window on `diff.file` — i.e. **offset reads
survive for one file at a time, not for the whole diff**. This is a small,
optional path, not the foundation. Streaming is thereby demoted from
"backbone" to "edge case," which is the entire point of the directive.

### Explicit-file fast path

When `explicit_pathspecs` (`GwzDiffPlan.md:438`) are **literal files** (not
directories or globs), the client routes them straight to `diff.file` per path,
using the same workspace path→repo partitioning the plan already extracts from
`stage_routing.rs` (`GwzDiffPlan.md:608`–`611`). No manifest is computed. This
mirrors how `gwz add` and the other path-taking commands already work.

Caveat to document: rename/copy detection is inherently scoped to the filtered
set, so an explicit single target path reports its literal status without
cross-set rename pairing — which is consistent with `git diff -- <path>`
behavior. Discovery (phase 1) remains the path that yields full rename pairing.

## Obligations this directive creates (the plan author must still solve these)

The directive removes one hard problem and introduces a few smaller, ordinary
ones. The revised plan must address each explicitly:

1. **State vs. recompute for phase 2.** Either keep the phase-1 `Diff` alive
   keyed by operation id (stateful; needs a lifetime + cleanup, much lighter
   than a byte spool) or recompute a single-file diff per `diff.file` call
   (stateless; N+1 diff setups). **Recommend stateless recompute as the
   default**; revisit only if profiling shows the per-call setup dominates.
2. **Remote round-trip count.** N changed files can mean N phase-2 calls. Use
   `diff.files` batching to amortize; document a batch byte cap and fall back to
   per-file (and then to the offset window) when a batch would be too large.
3. **Client-side patch assembly.** `gwz diff | less` and `gwz diff > out.diff`
   want one coherent document. The client (both `gwz-cli` and `gwz-py`)
   concatenates phase-2 results in manifest order. Order and per-file bytes both
   come from core, so divergence risk is bounded — but the two clients must
   assemble identically; add a parity test. (`git apply` compatibility remains a
   v0 non-goal, `GwzDiffPlan.md:113`–`115`, so byte-for-byte stream identity with
   `git diff` is not required.)
4. **Oversized-file threshold and behavior.** Define the cap that triggers
   `truncated`, and whether default human output truncates-with-marker or
   transparently windows via `from_offset`.

## Effect on existing phases

- **AD4** (`GwzDiffPlan.md:151`–`179`): rewrite from "non-lossy spool stream" to
  "bounded per-file request/response; offset window only for an oversized single
  file." The non-lossy/backpressure language is no longer load-bearing.
- **D-1** (`GwzDiffPlan.md:714`–`747`): shrinks dramatically. It is no longer a
  spool/backpressure/PyO3-streaming spike. Its remaining job is to decide
  state-vs-recompute and the batching/oversized-file policy, and to confirm
  ordinary request/response carries `BYTES` (including NUL) across the PyO3
  bridge — which the existing `call` path already does.
- **D3** (`GwzDiffPlan.md:837`): becomes "phase-1 manifest handler + phase-2
  file handler," not a streaming handler.
- **D5/tests**: replace stream/cancel/offset-resume tests with manifest
  correctness, per-file patch correctness, explicit-file fast-path routing,
  batch behavior, oversized-file truncation/window, and the client-assembly
  parity test.

## How this resolves prior findings

- **Review48-2 #1 (orphaned messages):** `DiffParsedTarget` now has a home —
  `DiffManifestResponse.targets`. `DiffComparison` becomes a core-internal /
  manifest detail rather than dead wire schema.
- **Review48-2 #4 (empty `DiffOpenResponse.summary`):** `DiffOpenResponse` is
  gone; the summary lives in `DiffManifestResponse` where it is genuinely known.
- **Review48-2 #5 (`diff.cancel` envelope):** `diff.cancel` is gone; cancellation
  is "stop calling phase 2."
- **Review48-2 #6 (`shape="log"` hard-code):** no `log` method remains in v0.
- **Review48 (PyO3 streaming binding):** no new streaming binding needed; phase 1
  and phase 2 are ordinary `call` round trips that `gwz-py` already supports.

## Residual risk

Low, and lower than the current plan. The mandated model trades one genuinely
unproven mechanism (a non-lossy backpressured spool across Rust + PyO3) for three
ordinary, well-understood ones (stateless per-file recompute, request batching,
and identical client-side assembly). The one consumer it mildly disadvantages —
a raw `gwz diff` byte pipe that must be reassembled client-side — is already
outside v0 goals. The plan author must still get rename-detection-in-phase-1,
the state-vs-recompute choice, and the oversized-file fallback right, but none of
these gate the per-repo libgit2 composition the design rests on. The directive
should be folded into `GwzDiffPlan.md` as the primary architecture before D0
freezes any schema or corpus.
