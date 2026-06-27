# GWZ diff plan — re-review (Review48-2)

Reviewed plan:

- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md` (revised)

Prior review: `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan-Review48.md`

Re-review status: independent design re-review, 2026-06-27. New claims were
re-validated against the source tree and `git2 0.21.0`. Citations are to verified
file:line locations.

## Verdict

The revision resolves essentially every finding from the first pass, and does so
without weakening the design's good decisions. Streaming stays a v0 requirement
but is now correctly sequenced behind a dedicated transport spike; the
remote-safety, operand-classification, ordering, exclude, and `gwz-py` gaps are
all addressed with concrete mechanisms rather than assertions. The plan is now
implementation-ready in its broad shape.

What remains is a small set of internal-consistency issues in the protocol
proposal and a couple of edge cases the transport spike should be told to prove.
None are blocking; they are cleanups to do during D-1/D0 before the corpus is
frozen.

## Resolved since Review48 (and Review55)

- **Streaming kept as v0, sequenced correctly.** New phase **D-1**
  (`GwzDiffPlan.md:714`) prototypes the non-lossy transport and its PyO3 binding
  *before* D0 freezes schema/corpus (`GwzDiffPlan.md:275`–`277`). AD4 now
  specifies an operation-scoped append-only spool with read-from-offset,
  blocking-or-typed-failure on capacity, and a cancel path
  (`GwzDiffPlan.md:161`–`179`). This directly answers the keystone risk.
- **Cancellation added.** `diff.cancel`, a `cancelled` stream kind, and
  pager-quit/broken-pipe → cancel wiring (`GwzDiffPlan.md:305`, `:376`, `:702`).
- **cwd is workspace-relative.** AD10 + `DiffRequest.workspace_cwd`
  (`GwzDiffPlan.md:248`, `:433`).
- **Core owns operand disambiguation.** AD9 + raw `operands`/`explicit_pathspecs`
  split (`GwzDiffPlan.md:239`, `:436`–`438`).
- **Manifest order separated from `stage_routing` sort** (`GwzDiffPlan.md:608`,
  `:611`, `:818`).
- **Read-only root exclude via in-memory pathspec exclusion** (AD11,
  `GwzDiffPlan.md:256`–`267`, `:587`).
- **`gwz-py` is first-class**, with its own protocol/PyO3/parser/render/exit-code
  work and parity tests (`GwzDiffPlan.md:204`–`211`, `:685`–`693`, `:1051`).
- **`exit_code`/`quiet` corrected:** wire carries only `summary_only`
  (`GwzDiffPlan.md:428`); `histogram` removed from `DiffAlgorithm`
  (`GwzDiffPlan.md:356`–`360`, confirmed absent in `git2 0.21`); `blob_vs_blob`
  and `no_index` dropped from `DiffComparisonKind` (`GwzDiffPlan.md:333`–`341`).
- **`--include-root`** replaces the colliding `--root` (`GwzDiffPlan.md:223`).
- **`foreach` patch renderer** demoted to its own subplan with the real cost
  called out (`GwzDiffPlan.md:879`–`882`).

## Findings

### [P2] `DiffComparison` and `DiffParsedTarget` are defined in the wire schema but referenced by nothing

`DiffComparison` (`GwzDiffPlan.md:389`) and `DiffParsedTarget`
(`GwzDiffPlan.md:399`) are declared as Taut messages, but after the AD9 rewrite
the request carries raw `operands` + `explicit_pathspecs` instead
(`GwzDiffPlan.md:430`–`439`), and no `DiffResponse` / `DiffStreamEvent` /
`DiffSummary` field references either type. They are orphaned: the request no
longer sends a `DiffComparison`, and the per-repo `DiffParsedTarget` that core
"resolves per repository" (`GwzDiffPlan.md:400`) is never returned.

Decide one of two things and make the schema reflect it:

- If these are **core-internal** resolution structs, drop them from the Taut
  proposal (they belong in D1's internal model, `GwzDiffPlan.md:786`), or
- **surface the resolved classification** by putting `DiffParsedTarget` into
  `DiffRepoSummary` (`GwzDiffPlan.md:466`). Given AD9 makes per-repo rev/path
  classification the subtlest behavior — `foo` can be a rev in one member and a
  path in another — echoing back how each repo classified each operand would be
  valuable for diagnostics and for the parity tests the plan already wants
  (`GwzDiffPlan.md:557`, `:1029`). I recommend this option.

### [P3] AD11's in-memory root exclusion list is narrower than the managed boundary it replaces

AD11 and D2 say root diff applies in-memory exclusions for "active member
path[s]" (`GwzDiffPlan.md:261`, `:825`). But the on-disk boundary it is standing
in for excludes more than members: `ensure_workspace_exclude` writes `/.gwz/`
(the `RUNTIME_DIR`) and `/gwz.conf/.tmp/` as well as every member path
(`gwz-core/src/workspace_ops/sync_workspace_boundary.rs:38`–`39`;
`RUNTIME_DIR = ".gwz"`, `WORKSPACE_DIR = "gwz.conf"` at
`gwz-core/src/workspace/mod.rs:6`–`8`). The in-memory exclusion used for the
root diff should cover the same set, not just members.

This is low severity for v0 plain diff (libgit2 does not emit untracked content
by default, so `.gwz/` would not show), but it becomes load-bearing the moment a
future `--include-untracked` (`GwzDiffPlan.md:572`) or any status-style listing
is added — and it interacts with the next finding.

### [P3] The output spool spills into `.gwz/`, the very directory whose visibility AD11 says not to trust

AD4 spills large output to `.gwz/` (`GwzDiffPlan.md:168`), which is correct as
ephemeral runtime state under AD1 (`GwzDiffPlan.md:127`) and is correctly named
— `RUNTIME_DIR = ".gwz"` (`gwz-core/src/workspace/mod.rs:8`). Its
git-invisibility, however, depends on the same managed `.git/info/exclude` block
that AD11 explicitly declares unreliable (`GwzDiffPlan.md:266`). So the plan
trusts the on-disk exclude for the spool while distrusting it for member
directories. Make this coherent: have the root-diff in-memory exclusion include
`RUNTIME_DIR` (per the finding above) so spool files can never surface in a root
diff regardless of exclude freshness. Worth one explicit sentence in AD4/AD11.

### [P3] `DiffOpenResponse.summary` cannot be populated at open time

`diff.open` is `role="in"` returning `DiffOpenResponse` immediately so the client
can begin tailing `diff.output` (`GwzDiffPlan.md:293`, `:484`). But the summary
(`has_differences`, file/line counts) is only known after all repos are diffed,
which is *after* open returns. So `DiffOpenResponse.summary`
(`GwzDiffPlan.md:489`) is structurally always empty. The real summary arrives in
the `finished` event's `DiffResponse` (`GwzDiffPlan.md:520`) or from the
non-streaming `diff` method. Either drop the field from `DiffOpenResponse` or
document that it is populated only when core fully buffers small output before
returning. As written it invites a client to read a summary that isn't there.

### [P3] `diff.cancel` carries no request envelope; confirm that is acceptable for a control op

`diff.cancel` uses inline scalar params `Params(operation_id=STR)` with no
`RequestMeta` (`GwzDiffPlan.md:305`). There is precedent for envelope-free inline
scalars — `events.subscribe` and `operation.result`
(`gwz.taut.py:85`–`91`) — but those are `role="out"` read/lookup methods. Cancel
is `role="in"` and actually stops work and frees server resources, so it is the
one new method where attribution/auth might matter (especially for remote core).
Confirm the transport's auth model covers an envelope-free control call, or give
`diff.cancel` a small request message with `meta`. Minor, but decide it
deliberately rather than by copy-paste from the read methods.

### [P3] The D0 snippet still hard-codes `shape="log"` for `diff.output`

The protocol proposal pins `diff.output` to `role="out", shape="log"`
(`GwzDiffPlan.md:300`) while the prose correctly says the concrete shape depends
on the D-1 spike and that a Taut `log` is acceptable "only if the implementation
supports read-from-offset/tail without losing bytes" (`GwzDiffPlan.md:297`–`299`,
`:736`). Since Taut `log` today is append-only with no flow control
(`taut/docs/Reference.md`) and the existing log-backed event path drops on
overflow (`gwz-core/src/operation/push_event.rs:437`–`460`), the snippet's
`shape="log"` should be marked provisional-pending-D-1 in the snippet itself, not
only in surrounding prose, so it is not mistaken for a settled decision when D0
work begins.

## Open questions (carried / new)

- Does the D-1 spike explicitly test the **blocking-producer deadlock** case? If
  core blocks production on a full spool while the same runtime must service the
  consumer's `diff.output` read, producer and reader must not share a thread.
  Worth an acceptance bullet under D-1 (`GwzDiffPlan.md:739`).
- Should per-repo operand classification (`DiffParsedTarget`) be returned for
  diagnostics? (See first finding.)
- `--merge-base` / `A...B`: the plan resolves these per repo, but the
  old-vs-new side mapping for the worktree-bearing `--merge-base <commit>` form
  is not spelled out. A one-line note in the comparison table
  (`GwzDiffPlan.md:540`) would remove the ambiguity.

## Source verification

Every external citation in this re-review (and the load-bearing ones carried
from Review48) was confirmed by reading the source directly, not via summary:

- `RUNTIME_DIR = ".gwz"`, `WORKSPACE_DIR = "gwz.conf"` — confirmed at
  `gwz-core/src/workspace/mod.rs:6` and `:8`.
- The managed exclude writes `/{RUNTIME_DIR}/`, `/{WORKSPACE_DIR}/.tmp/`, and
  every member path, and is mutating (`fs::write`) and "regenerated on every
  run" — confirmed at `gwz-core/src/workspace_ops/sync_workspace_boundary.rs:36`–`41`,
  `:54`–`57`, doc comment `:14`–`18`. A read-only diff does not call
  `sync_workspace_boundary`, so AD11's "exclude may be stale" premise holds.
- Inline-scalar `Params(operation_id=STR)` with no `meta` exists on
  `events.subscribe` and `operation.result`, and **both are `role="out"`** —
  confirmed at `gwz-core/protocol/gwz.taut.py:85`–`91`. This is the exact basis
  for the `diff.cancel` (`role="in"`) caveat above.
- The operation event path is lossy: on `events.len() >= event_capacity` it
  runs `state.events.clear()` then pushes a `Reset` ("event buffer overflow;
  history incomplete") — confirmed at
  `gwz-core/src/operation/push_event.rs:438`–`459`.
- Taut `log` is `whole · append-only`, ops `append / read-from-offset / tail`,
  with **no flow-control column** — confirmed at `taut/docs/Reference.md:180`.
- `git2 0.21` exposes `patience` (`diff.rs:864`) and `minimal` (`diff.rs:869`)
  but **`histogram` appears nowhere in the crate** (`grep -rni histogram` over
  the vendored source returns nothing) — so removing it from the v0 enum is
  correct. `old_prefix`/`new_prefix` (`diff.rs:924`/`:932`), `show_binary`,
  `force_text`, `reverse`, `context_lines`, `interhunk_lines`, and
  `show_untracked_content` all exist as claimed.

One small correction to Review48: `gwz-py/native/src/lib.rs` registers **eight**
`#[pyfunction]`s, not six — the six operational ones (`call`, `submit`,
`subscribe_events`, `wait_events`, `operation_result`, `try_operation_result`)
plus `health` and `version` (`gwz-py/native/src/lib.rs:99`–`108`). The substance
is unchanged: the only streaming path is `subscribe_events`/`wait_events`
(`:56`, `:64`), both of which drain the same bounded `OperationEvent` store, and
there is no byte-stream primitive — which is exactly why AD4/D-1 require a new
PyO3 binding for the diff output stream.

## Residual risk

Low and well-contained. The architectural risks from the first review are
closed; the spool/transport remains the one genuinely unproven mechanism, but
the plan now isolates it in D-1 with concrete acceptance criteria and forbids
freezing the corpus until it is proven across both Rust and PyO3 consumers. The
remaining findings are schema hygiene (orphaned messages, the open-time summary
field) and exclusion-completeness coherence — all cheap to fix during D-1/D0 and
none capable of derailing the per-repo libgit2 composition that the design rests
on.
