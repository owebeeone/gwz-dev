# GWZ diff plan independent review 55-2

Reviewed plan:

- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md`

Review status: independent design review, 2026-06-27. This review targets the
updated plan after earlier Review48 and Review55 feedback was incorporated.

## Findings

### [P1] `diff.output` is still shaped like a Taut subscription, not an offset-addressable read API

The plan proposes
`method("diff.output", role="out", shape="log", params=Params(output_id=STR, from_offset=INT, max_bytes=INT), out=Ref.DiffStreamEvent)`
in `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:300`, while also
requiring resumable byte offsets and bounded reads in
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:527`. Current Taut
`log` is described as append/read/tail conceptually, but generated clients
treat streaming methods as subscribe-once plus unsubscribe, with no per-read
`max_bytes`, no ack, and no backpressure path: see
`/Users/owebeeone/limbo/gwz-dev/taut/src/taut/gen/runtime/typescript/taut_client.ts:69`
and `/Users/owebeeone/limbo/gwz-dev/taut/src/taut/gen/scaffold.py:117`.
D-1 should not leave this as `shape="log"` unless the transport
implementation actually grows read-from-offset semantics; otherwise use
explicit unary `output.read` plus a separate tail/subscribe method, or add a
new implemented Taut shape.

### [P1] Root member exclusion is specified, but the current `git2` wrapper does not expose the clean implementation hook

The plan requires read-only in-memory exclusion of member directories for root
diffs in `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:256` and
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:587`. Current
`git2::DiffOptions::pathspec` is additive path filtering, not a clear
"all except these dirs" mechanism, and the Rust wrapper explicitly leaves
`notify_cb` unexposed in
`/Users/owebeeone/.cargo/registry/src/index.crates.io-1949cf8c6b5b557f/git2-0.21.0/src/diff.rs:965`.
`Diff::print` / `foreach` callbacks terminate on `false`; they do not provide
a normal skip-delta filter in the wrapper
(`/Users/owebeeone/.cargo/registry/src/index.crates.io-1949cf8c6b5b557f/git2-0.21.0/src/diff.rs:169`).
Without a concrete filtered-diff strategy, stale `.git/info/exclude` can still
leak materialized member directories into root output or summaries.

### [P2] Selection plus pathspec intersection is still underspecified

AD7 defines no-selection and explicit-member-selection defaults in
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:221`, and pathspec
routing defines fan-out in
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:613`, but the plan
does not define cases such as `gwz --member A diff -- B/file`,
`gwz --all diff -- member/path`, or selected member plus root pathspec.
Current status selection returns selected members only and preserves explicit
selection order
(`/Users/owebeeone/limbo/gwz-dev/gwz-core/src/status/status_member.rs:107`);
stage routing has separate fan-out behavior and a special narrowed `--all`
path
(`/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stage.rs:31`).
Diff needs an explicit rule: candidate repos from selection, then pathspec
intersection, with defined empty-match/error behavior.

### [P2] Stats formats are in the v0 protocol before workspace path rewriting is proven

`DiffOutputFormat` includes `stat`, `numstat`, and `shortstat` in
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:343`, and D3 says to
stream stats formats in
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:855`. But
`git2::DiffStats::to_buf` exposes only formatted output over the repo-local
diff
(`/Users/owebeeone/.cargo/registry/src/index.crates.io-1949cf8c6b5b557f/git2-0.21.0/src/diff.rs:1214`),
with no path-prefix callback. The plan's own open decision at
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:1076` should be
resolved before D0 freezes these as v0 formats, or they should be explicitly
Tier 1 until a GWZ stats renderer exists.

### [P2] The PyO3 implementation touchpoints are under-scoped

D-1 and D5 mention `gwz-py/native/src/lib.rs` and Python client files in
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:718` and
`/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzDiffPlan.md:901`, but current
native method availability is gated in
`/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/dispatch/mod.rs:12`, async
event/result storage is in
`/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/operations.rs:16`, and the
bridge protocol has only call/event/result methods
(`/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/bridge.py:14`). Generated
messages plus `lib.rs` exports will not make `diff.open`, `diff.output`, or
cancellation callable from Python.

## Open questions

- Should `--all` count as explicit member selection for root inclusion, or
  should it mean root plus all materialized members for diff?
- What is the exact empty-match behavior when selection and pathspecs do not
  intersect?
- Will root exclusion be implemented through lower-level libgit2 raw options, a
  filtered renderer/stat path, or a fail-fast validation of the managed exclude
  boundary?
- Is the final output stream a generic operation-output transport or a
  diff-only API?

## Residual risk

The updated plan is directionally sound on client/server boundaries, PyO3
parity, and avoiding lossy operation events. The remaining risk is mostly
implementability: the current Taut and `git2` surfaces do not yet provide the
exact stream and root-filter primitives the plan assumes. Resolve those in
D-1/D2 before freezing protocol corpus or promising Tier 0 output parity.
