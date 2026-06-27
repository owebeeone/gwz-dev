# GWZ diff plan independent review

Reviewed plan:

- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md`

Review status: independent design review, 2026-06-27.

## Findings

### [P1] Rev/pathspec parsing is assigned to the client, but Git-compatible disambiguation needs repo knowledge

The plan says revision resolution belongs in core
(`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:107`), but also makes
the client parse Git-like args into `DiffRequest`
(`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:157`) and sends
already-split `comparison`, `revisions`, and `pathspecs`
(`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:334`). `git diff foo`
cannot be classified correctly without checking whether `foo` is a ref/path in
the target repo, and in GWZ it may differ per member. Core should own
positional rev/pathspec disambiguation or the client must query core before
building the comparison.

### [P1] `DiffRequest.cwd` is not server-safe as specified

The plan says `gwz-core` may run on a server
(`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:48`), but proposes an
absolute terminal cwd in the request
(`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:336`). That copies
today's local-only `StageRequest.cwd` contract
(`/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py:947`,
`/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs:738`). A remote core
cannot safely resolve `/Users/...` from the client. Use a workspace-relative
logical cwd, or require a transport-level cwd mapping.

### [P1] The proposed `diff.stream` contract promises backpressure, but neither Taut `log` nor current GWZ runtime provides it

The plan proposes `role="out", shape="log"` for chunks
(`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:214`) and says chunks
must not drop (`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:417`).
Taut documents `log` as append-only history
(`/Users/owebeeone/limbo/gwz-dev/taut/docs/Reference.md:176`). Existing
operation events are explicitly bounded and reset on overflow
(`/Users/owebeeone/limbo/gwz-dev/gwz-core/src/operation/operation_runtime.rs:20`,
`/Users/owebeeone/limbo/gwz-dev/gwz-core/src/operation/push_event.rs:437`). The
plan needs a concrete blocking stream sink, spool/offset design, or transport
requirement.

### [P2] Root/member boundary correctness depends on mutable `.git/info/exclude`, but diff is declared read-only

The root repo hides member repos through a managed local exclude block
(`/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/sync_workspace_boundary.rs:14`,
`/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/sync_workspace_boundary.rs:31`).
The plan says diff must not mutate state beyond operation records
(`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:105`). If the exclude
block is stale or missing, root diff may see member directories; if diff
refreshes it, AD1 is violated. The plan should specify validation or a
read-only root pathspec exclusion strategy.

### [P2] Reusing `stage_routing.rs` naively violates planned manifest-order streaming

The plan requires root first, then members in manifest order
(`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:461`), but current
stage routing stores targets in a `BTreeMap` and returns them sorted by member
path (`/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/stage_routing.rs:24`,
`/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/stage_routing.rs:101`).
It also receives all manifest member paths in `handle_stage`
(`/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stage.rs:29`).
Diff planning needs a new target ordering layer over active/materialized Git
members.

### [P2] The protocol proposal includes unsupported or client-only fields too early

`DiffOptions.exit_code` is in the core request
(`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:331`) even though the
plan says exit status is a client contract
(`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:182`). The wire enum
also includes `histogram`
(`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:266`) while the plan
later says wrapper support must be verified
(`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:523`); local `git2`
exposes `patience`/`minimal`, but no histogram option
(`/Users/owebeeone/.cargo/registry/src/index.crates.io-1949cf8c6b5b557f/git2-0.21.0/src/diff.rs:863`).
Keep unsupported values out of v0 or require explicit hard-error tests.

## Open questions

- Should core receive raw positional diff operands plus `--` boundary and
  resolve them per target repo, instead of receiving pre-classified
  `comparison`/`pathspecs`?
- Is `diff.stream` meant to be a Taut `log`, a live `stream`, or a
  GWZ-specific operation-output transport with explicit cancellation and
  backpressure?
- What is the remote-core representation of cwd: workspace-relative path,
  server absolute path, or transport-mapped client path?
- What should root inclusion be called? The proposed future `--root` conflicts
  with the existing global `--root` workspace option
  (`/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/globalargs.rs:32`).

## Residual risk

The plan is directionally sound on keeping terminal behavior out of core and
composing per-repo diffs, but the highest-risk areas are still protocol/runtime
shape and Git argument semantics. Without resolving those, an implementation
can look correct for `gwz diff -- path` cases while failing ambiguous Git-like
invocations, remote-core use, or large streamed patches.
