# GWZ diff plan independent review (Review48)

Reviewed plan:

- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md`

Review status: independent design review, 2026-06-27. Findings were validated
against the actual source tree, the vendored `git2 0.21.0` crate, and the Taut
docs. Citations below are to verified file:line locations.

## Verdict

The plan is directionally correct on the two decisions that matter most:
composing per-repo libgit2 diffs instead of inventing a virtual multi-repo
object store (AD2), and keeping pager/color/terminal policy out of `gwz-core`
(AD3, AD6). Those are the right calls and the reuse of existing status/routing
infrastructure is mechanically plausible.

The plan's risk is concentrated in one place: non-lossy streaming of patch
bytes is a hard v0 requirement (AD4, Definition of Done), but the required
transport semantics (ordered, non-lossy, backpressured) are *not* provided by
anything in the framework today — the only Taut streaming shape is append-only
`log` with no flow control, and the existing operation event buffer drops
history on overflow. The plan correctly identifies that a new transport is
needed but never shows it is achievable, yet Phase D0 still freezes the wire
schema and golden corpus that depend on it. The transport must be designed and
proven *first*; the schema freeze should follow it, not precede it.

A second blind spot, surfaced after the first review pass: the plan treats
`gwz-cli` as the only real client and `gwz-py` as an afterthought ("available to
gwz-py integration", AD6). But `gwz-py` is a *first-class in-process consumer of
`gwz-core`* with its own complete Python CLI, and it consumes operations through
a PyO3 bridge whose only streaming primitive today is `operation_id` →
`subscribe_events` — i.e. the same lossy event path the diff stream is trying to
escape. Any transport design that satisfies only `gwz-cli` will need rework to
serve `gwz-py`. The two findings below cover this.

## Findings

### [P1] D0 freezes a wire schema and corpus for a stream whose transport is unsolved

The plan's first phase commits the full diff protocol — request, options,
stream events, summary — *and regenerates golden corpus*
(`GwzDiffPlan.md:583`, `GwzDiffPlan.md:590`). But the central streaming
requirement is unmet by the framework:

- The only `shape="log"` method today is `events.subscribe`
  (`gwz-core/protocol/gwz.taut.py:85`). Taut documents `log` as
  *whole · append-only* with no flow-control primitive
  (`taut/docs/Reference.md:180`).
- The existing operation event buffer is bounded (default small) and, on
  overflow, **clears all buffered events and emits a `Reset`** —
  `state.events.clear()` then a synthetic reset event
  (`gwz-core/src/operation/push_event.rs:437`–`460`), backing the
  `VecDeque` in `OperationState` (`gwz-core/src/operation/operation_runtime.rs:21`).

So AD4's promise — "transport must not drop `chunk` events… must apply
backpressure" (`GwzDiffPlan.md:417`) — is a *requirement the current stack
cannot satisfy*, and the plan punts it to "implementation must use an
append-only spool or reject streaming" (`GwzDiffPlan.md:419`). That is a
research spike, not an implementation detail. Committing the schema and golden
corpus in D0 before this is resolved means the goldens will churn when the
transport shape settles (the plan itself flags `diff.stream` vs. a generic
output stream as an open decision, `GwzDiffPlan.md:860`). **Recommendation:**
add a transport phase *before* D0 that designs and prototypes the byte-stream
end to end — the blocking/backpressured sink, the spool with read-from-offset,
and cancellation — then freeze the `diff.stream` schema and corpus on top of the
proven shape. Streaming stays a v0 deliverable; it is just sequenced after the
thing it depends on.

### [P1] The non-lossy/backpressure requirement needs a concrete transport design, not an assertion

AD4 and the Definition of Done make non-lossy patch streaming a v0 gate
(`GwzDiffPlan.md:417`, `GwzDiffPlan.md:881`), but the plan never says *how* the
stream achieves it. The gap is real and verified:

- Taut `log` is *whole · append-only* with no documented flow-control
  (`taut/docs/Reference.md:180`); `events.subscribe` is the only log method
  (`gwz-core/protocol/gwz.taut.py:85`).
- The operation runtime's event path is the opposite of non-lossy: bounded
  `VecDeque` that **clears and resets on overflow**
  (`gwz-core/src/operation/push_event.rs:437`–`460`,
  `gwz-core/src/operation/operation_runtime.rs:21`).

The plan proposes `diff.stream` as `role="out", shape="log"`
(`GwzDiffPlan.md:214`) — i.e. the same family as the lossy path — then
*requires* it not drop chunks. Those are in tension. To make streaming work in
v0, the plan must pin down one mechanism and write it into the design:

- a **blocking sink** where core stalls production until the consumer drains
  (true backpressure), and/or
- an **append-only spool** on the core side with `read-from-offset`/`tail` so a
  slow or reconnecting consumer never loses bytes,
- with the chosen mechanism's relationship to (or replacement of) the bounded
  event runtime spelled out.

Note `gwz forall` is *not* a precedent here: it streams by spawning child
processes with inherited stdio from the CLI (`gwz-cli/src/forall.rs`), so the
bytes never traverse core or the protocol. Diff's bytes originate in core, so it
genuinely needs the transport the plan is calling for — which is exactly why it
must be designed before the schema is frozen.

### [P1] `DiffRequest.cwd` repeats the local-only absolute-path contract that breaks remote core

`DiffRequest.cwd` is an "absolute current directory" (`GwzDiffPlan.md:336`).
That copies today's `StageRequest.cwd=F(2, STR)`
(`gwz-core/protocol/gwz.taut.py:951`), which the CLI fills with the literal
local absolute path (`gwz-cli/src/clirequest.rs:732`–`744`,
`cwd.to_string_lossy().into_owned()`). A remote core cannot resolve a client's
`/Users/...` path. Since the plan's whole rationale for the protocol is remote
readiness (`GwzDiffPlan.md:48`), the request should carry a **workspace-relative
logical cwd**, with the client resolving the local absolute path before the
call. Otherwise the streaming protocol is remote-shaped but its primary input is
local-only.

### [P1] The transport must cross the PyO3 FFI boundary too — `gwz-py`'s only stream primitive is the lossy event path

`gwz-py` is not an "integration" bolted on later; it is an in-process consumer
that links `gwz-core` directly as a PyO3 cdylib
(`gwz-py/native/Cargo.toml`, `gwz-core = { path = "../gwz-core" }`;
`module-name = "gwz._gwz_core"`). Its entire client surface is six PyO3
functions — `call`, `submit`, `subscribe_events`, `wait_events`,
`operation_result`, `try_operation_result` (`gwz-py/native/src/lib.rs`) — and
its streaming pattern is `submit()` → take `operation_id` →
`subscribe_events(operation_id)` (`gwz-py/src/gwz/client.py`, `_stream_call`).

That is precisely the `operation_id`-then-subscribe model the plan calls a race
to avoid (`GwzDiffPlan.md:218`–`221`), and it rides the bounded, drop-on-overflow
event runtime (`gwz-core/src/operation/push_event.rs:437`–`460`). There is **no
byte-stream primitive across the FFI today**. So the transport-design phase (the
first finding) cannot target `gwz-cli` alone: whatever non-lossy mechanism is
chosen (blocking sink / spool) must also be expressible as a PyO3 binding that
yields ordered diff chunks to Python without going through `subscribe_events`.
If the transport is designed only for the Rust in-process path, `gwz-py` either
gets a lossy diff or forces a second transport redesign. This belongs in the v0
transport spike, not a follow-up.

### [P2] AD6's "reusable by gwz-py" overstates what a Rust `gwz-client` crate can share

AD6 proposes a Rust `gwz-client` crate owning arg parsing, color, pager, and
exit-code logic, "available to gwz-py integration" (`GwzDiffPlan.md:149`–`164`).
But `gwz-py` already reimplements the *entire* CLI in Python — its own argparse
command registry, its own render path, no code shared with `gwz-cli`
(`gwz-py/src/gwz/cli.py`, `pyproject.toml` `gwz = "gwz.cli:main"`). A Rust crate
cannot be called from Python without yet another PyO3 layer, so the diff arg
parser, color policy, pager launch, and exit-code mapping would all have to be
reimplemented Python-side regardless. The only thing that genuinely crosses is
the **generated protocol types** (`DiffRequest`/`DiffResponse`/
`DiffStreamEvent`), which Taut already emits for both Rust and Python. The plan
should say this plainly: `gwz-client` is reusable by `gwz-cli` and future *Rust*
clients; `gwz-py` reuses the protocol contract, not the client crate. Otherwise
the design implies a sharing that the FFI boundary does not deliver, and the
pager/color/exit-code work is silently doubled. (`gwz-py` is also a separate
Cargo workspace, not a member of the root `Cargo.toml`, so even path-level
crate reuse would need deliberate wiring.)

### [P2] Positional `<rev>` vs `<pathspec>` disambiguation needs per-repo knowledge but is assigned to the client

The plan puts revision resolution in core (`GwzDiffPlan.md:107`) yet has the
client parse args into a pre-split `comparison` / `revisions` / `pathspecs`
`DiffRequest` (`GwzDiffPlan.md:157`, `GwzDiffPlan.md:334`). `git diff foo`
cannot be classified without knowing whether `foo` is a ref or a path in the
target — and in GWZ that answer can differ per member. The client cannot decide
this before `--`. Either core receives the raw operands plus the `--` boundary
and disambiguates per target repo, or the client must round-trip to core to
resolve. The structured request is right; the *classification* should not be a
client responsibility.

### [P2] "Manifest order" conflicts with the routing helper the plan proposes to reuse

AD/output-ordering requires root first then members in **manifest order**
(`GwzDiffPlan.md:461`, `GwzDiffPlan.md:649`). But the plan proposes
generalizing `stage_routing.rs` for pathspec routing
(`GwzDiffPlan.md:490`), and that module keys targets in a `BTreeMap`
(`gwz-core/src/workspace_ops/stage_routing.rs:33`) returning them root-first
then **sorted by member path** (`stage_routing.rs:23`). The manifest-order
guarantee lives in the *other* reuse target, status, which iterates
`manifest.members` filtered by `active` with no sort
(`gwz-core/src/status/status_member.rs:112`–`116`). The two proposed reuse
sources have different ordering semantics. The plan should be explicit:
`stage_routing` provides path→repo *partitioning*, while iteration order must
come from the manifest member vector — and decouple dedup/sort from emission
order.

### [P2] Read-only diff vs. the mutable `.git/info/exclude` boundary

AD1 forbids mutation beyond operation records (`GwzDiffPlan.md:105`). But the
root repo hides member directories via a managed block in
`.git/info/exclude`, rebuilt from the lock and **written when it differs**
(`gwz-core/src/workspace_ops/sync_workspace_boundary.rs:14`–`26`,
`:49`–`57`). A read-only `gwz diff` will not refresh that block; if it is stale
or missing, root `diff_index_to_workdir` can surface member directories as
untracked/changed. The plan must specify how root diff excludes members without
writing — e.g. validate the block and fail/warn, or apply an in-memory pathspec
exclusion for member paths rather than relying on on-disk exclude state.

### [P2] No cancellation in the stream contract

The stream lifecycle is started → repo_started → chunk → repo_finished →
finished/error (`GwzDiffPlan.md:405`–`416`) with no client→core cancel signal.
The dominant interactive case is `gwz diff` piped to `less`, where the user
quits after one screen. For a local core that is a harmless `SIGPIPE`; for the
remote core the plan is designed around, an uncancellable stream means core
keeps diffing (and a spool keeps filling) after the consumer is gone. Add
cancellation/early-termination to the contract — it also interacts directly with
the backpressure question in the first finding.

### [P3] `exit_code` belongs to the client; `quiet` legitimately belongs to core

`DiffOptions` carries both `exit_code` and `quiet`
(`GwzDiffPlan.md:331`–`332`) although AD8 says exit status is a client contract
(`GwzDiffPlan.md:182`). These are not the same:

- `exit_code` is pure client process-status mapping — drop it from the wire.
- `quiet` is genuinely a core optimization: the D3 work says `--quiet` should
  "compute summaries without emitting patch chunks" (`GwzDiffPlan.md:672`),
  which only core can do. Keep a core-side "summary only / suppress chunks"
  flag, but name it for what core does, not for the CLI's `--quiet`.

### [P3] `histogram` is not available in `git2 0.21` — remove it from the v0 enum

`DiffAlgorithm` includes `histogram=4` (`GwzDiffPlan.md:271`). The vendored
crate exposes only `patience` (`git2-0.21.0/src/diff.rs:864`) and `minimal`
(`diff.rs:869`); there is no histogram setter. The plan elsewhere admits this
needs verification (`GwzDiffPlan.md:523`). Keep unsupported wire values out of
v0, or the protocol advertises a capability the backend will reject. (For
reference: `ActionKind.diff=20` in the plan is correct — current max is
`clone_workspace=19` at `gwz.taut.py:115`.)

### [P3] The `Diff::foreach` fallback for path rewriting is a hidden large-cost step

D4 plans to fall back to a GWZ patch renderer over `DiffDelta`/`DiffHunk`/
`DiffLine` callbacks if libgit2 prefixing is insufficient for extended headers
(`GwzDiffPlan.md:694`, `git2-0.21.0/src/diff.rs:190`). Both `old_prefix` /
`new_prefix` exist (`diff.rs:924`/`932`), so simple +++/--- cases are cheap —
but a hand-rolled renderer must reproduce git's *full* patch grammar (mode
lines, `/dev/null`, `similarity index`, `rename from`/`rename to`, binary
patch literals, `-z` NUL records). That is effectively re-implementing
libgit2's patch printer and will not fit the aspirational <500 LOC step budget.
The plan should call this out as its own multi-step risk and, ideally, prove via
a spike whether `old_prefix`/`new_prefix` already cover rename/copy headers
before committing to the fallback.

### [P3] `--root` name collides with the existing global option; `--jsonl` bytes need a serialization decision

- The future root-inclusion flag is called `--root` (`GwzDiffPlan.md:178`), but
  `gwz` already has a global `--root` for the workspace path
  (`gwz-cli/src/globalargs.rs:32`). Pick a non-colliding name
  (e.g. `--include-root`).
- `--jsonl` is to emit stream events "including base64 or escaped bytes
  according to the generated serializer" (`GwzDiffPlan.md:726`). Whether Taut's
  JSON serialization base64-encodes `BYTES` is unverified; confirm it before
  promising JSONL patch streaming, and note that base64'd patch text roughly
  doubles output size.

## Open questions

- What concrete transport gives ordered + non-lossy + backpressured byte
  delivery: a blocking sink, an append-only on-disk spool with read-from-offset,
  or a new Taut shape? This must be answered before D0 freezes the schema.
- How does that transport surface across the PyO3 bridge so `gwz-py` consumes
  diff chunks without the lossy `subscribe_events` path? What new binding
  function replaces/augments the current six?
- Does AD6's `gwz-client` cover `gwz-py` at all, or only Rust clients? If the
  pager/color/exit-code logic must be reimplemented in Python, the plan should
  budget that as separate work.
- Does core resolve raw positional operands per target repo, or does the client
  pre-classify? (Per-repo resolution is the only correct option.)
- What is the remote-core representation of `cwd` — workspace-relative logical
  path, or transport-mapped?
- How does read-only root diff exclude member directories without writing
  `.git/info/exclude`?

## Residual risk

Non-lossy patch streaming is a v0 requirement and the plan is right to keep it.
The risk is one of *sequencing within v0*: D0 as written freezes the
`diff.stream` wire schema and golden corpus before the transport that gives it
its load-bearing property (non-lossy backpressured byte delivery) has been
designed or proven on this stack. If the transport shape changes after the
corpus exists, the goldens churn. Add a transport-design phase ahead of D0 so
the schema is frozen on a proven mechanism, then let D1 (per-repo libgit2
engine) and D2 (planning/ordering) proceed in parallel against it. The per-repo
composition, the streaming requirement itself, and the terminal-policy-in-client
decisions are all sound and should be kept.
