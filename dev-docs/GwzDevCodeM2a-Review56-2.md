# GWZ M2a Code Re-Review - Reviewer 56

Date: 2026-07-19

Reviewer designation: 56

Workspace: `/Users/owebeeone/limbo/gwz-dev`

## Reviewed Revisions And Scope

The committed heads exactly match the coordinated M2a checkpoint:

- Workspace root: `3d6ce941e7f79eb7aa4e93a1ddb4a865f17f9e90..585e44d07f13880fe60072b7409a9d559f64ae53`
- `gwz-core`: `8a0ad1732ed4efc51bb6d264067c18b1f7e3f920..cf70d360925563f82d92d93d9522e91d3567a5f3`
- `gwz-cli`: `f4e0261c3db33e0956331fa9fa576f123ae0f1e1..9d634709a4a9be599a0300244eb92d53416b0c91`
- `gwz-py`: `83e058e644cf9e719cc57accd325bd69223168f1..b771d98d7b33401fd555a13ad0e16b01c01610c6`

This re-review also covers the uncommitted M2a remediation working-tree changes present on 2026-07-19. They are not commit-addressable: `gwz-core` has 36 tracked files changed (3,852 insertions, 349 deletions) plus the untracked `protocol/fixtures/cli_parity/merge_event.json`; `gwz-cli` has 11 files changed (324 insertions, 53 deletions); and `gwz-py` has 14 files changed (324 insertions, 79 deletions). The report therefore applies to that exact working-tree snapshot as well as the committed ranges above.

I read the merge design, plan, overall design, REQ-089 requirements, public merge/machine-output/release documentation, the remediation plan, the changed implementation and tests, generated-artifact changes, and relevant callers outside the diff. I did not read another reviewer's `GwzDevCodeM2a-Review*.md` report. No subagents were used. Temporary reproductions were created outside the repositories and removed. No implementation, test, protocol, documentation, or workspace artifact was modified by this review; this report is the only workspace write.

## Executive Verdict

Verdict: **remediation required before M2b**.

Severity counts:

- P0: 0
- P1: 1
- P2: 3
- P3: 0

The remediation substantially improves native-state observation, durable action journaling, coordinated abort, archive retry, central mutation gates, attribution, and core event ordering. The mandatory suite is green. It does not yet meet its own exit gate, however: exact crash reconciliation can adopt and later destroy unrelated work, one core merge-start path bypasses the open-operation gate, and Python's event lifecycle/JSONL integration is incomplete.

## Findings

### P1 - Pending true-merge reconciliation accepts a different tree as `completed_exactly`

Files and lines:

- `gwz-core/src/workspace_ops/merge/model.rs:206-220` records the action kind, branch, parents/source, and message, but no expected tree/index identity.
- `gwz-core/src/git/gitbackend.rs:19-30` defines "exact" matching only in terms of ordered parents and byte-exact message.
- `gwz-core/src/git/gitbackend.rs:849-873` implements that predicate without comparing the commit tree or attribution.
- `gwz-core/src/workspace_ops/merge/status.rs:104-143` classifies a clean true-merge or resolution commit as completed through that predicate.
- `gwz-core/src/workspace_ops/merge/status.rs:334-348` then clears all drift and marks both continue and abort eligible.
- `gwz-core/src/workspace_ops/merge/continue_op.rs:125-187` durably adopts the resulting commit.
- `gwz-core/src/workspace_ops/merge/abort.rs:331-340` treats the same observation as the exact operation result for rollback.

Violated requirement or invariant:

- `gwz-core/dev-docs/GWZRequirements.md:652-664` requires exact durable intent and exact post-interruption classification.
- `gwz-core/dev-docs/GWZRequirements.md:696-709` permits adoption only when branch, ref, parents, source, message, repository state, index, and worktree match durable intent; ambiguous mutation must remain recovery-required.
- `gwz-core/dev-docs/GWZRequirements.md:711-721` forbids abort from destroying post-merge user work.
- The prompt's continue review invariant requires crash-window `planned` rows not to absorb unrelated work.

Concrete reproduction:

I created a real one-member workspace and a true merge, then reconstructed the required crash window with a durable `pending_action`. I created a second two-parent commit with the exact recorded ordered parents and byte-identical message, but with an additional `unrelated.txt` in its tree, moved the recorded branch to it, and requested merge status.

Observed result:

```json
{
  "abort_eligible": true,
  "continue_eligible": true,
  "drift_kinds": [],
  "forged_contains_unrelated_file": true,
  "ordered_parents_match": true,
  "pending_state": "completed_exactly"
}
```

This state is realistic after a crash following the intent write: a user can finish a native merge using the prepared message while additional staged content is present. The same problem applies to a resolution commit after a pending-resolution intent is durable.

Impact:

Continue silently adopts a commit containing content that was not part of GWZ's frozen action. More seriously, default abort considers that commit GWZ-owned and can hard-reset it to the recorded before commit, removing the additional tracked file from the worktree. This is a release-blocking, potentially destructive failure of the crash-recovery contract.

Recommended correction:

Persist a content identity for every commit-creating pending action. At minimum, store the exact expected tree OID and require the candidate commit to match it in addition to ordered parents and message. For resolution, freeze the fully preflighted index tree before commit; for a clean true merge, compute/freeze the planned merge tree before the commit-producing action. If author/committer identity is part of the frozen action policy, persist and verify it too. Any mismatch must produce `pending_action_ambiguous`, false eligibility flags, and no durable adoption or rollback.

Regression test:

Add real-Git fault tests for both a clean true merge and a resolution commit. Stop after the pending intent write, create a same-parent/same-message commit whose tree contains one extra staged file, and move the target branch to it. Assert status reports `Ambiguous` plus `PendingActionAmbiguous`, continue and abort are both ineligible, and neither the record, branch, index, nor worktree changes. A control commit with the exact frozen tree must still reconcile as completed exactly.

### P2 - Core dry-run merge start bypasses the open-operation gate

Files and lines:

- `gwz-core/src/operation/open_merge_gate.rs:45-76` classifies every `MergeStart` as blocked while a merge is open.
- `gwz-core/src/workspace_ops/merge/mod.rs:99-116` acquires/enforces the authoritative gate only for non-dry-run start; dry-run resolves the workspace directly.
- `gwz-core/src/workspace_ops/merge/mod.rs:187-204` already provides the intended helper that gates dry-runs without locking and guards real mutations.
- `gwz-cli/src/globalargs.rs:801-807` classifies all Rust CLI starts as `MergeStart`, so the Rust driver's diagnostic gate masks the core omission.
- `gwz-py/native/src/dispatch/merge.rs:18-29` delegates directly to the core merge handler, exposing the omission to Python and direct-core callers.

Violated requirement or invariant:

- `gwz-core/dev-docs/GWZRequirements.md:684-694` requires a single authoritative gate on the request's effective workspace, enforced by public core entry points and not bypassable by a driver.
- The central gate table explicitly classifies `MergeStart` as blocked, with no dry-run exception.
- Rust and Python must lower to the same core behavior under REQ-089C (`GWZRequirements.md:637-642`).

Concrete reproduction:

In a real Python-native workspace I completed a member merge to the intentional open `finalizing` handoff, then invoked a second merge start with `dry_run=True` against the same explicit workspace.

```json
{
  "open": true,
  "open_state": "finalizing",
  "second_start_aggregate": "accepted",
  "second_start_state": "completed"
}
```

The equivalent Rust CLI invocation is rejected by its early driver gate.

Impact:

Python and direct-core callers can plan a new merge against the partial live state of an already open operation while Rust rejects it. Although this path is read-only, it violates the central policy, produces contradictory public behavior, and can present a misleading plan while the accepted lock deliberately remains at the pre-merge baseline.

Recommended correction:

Route merge start through `guarded_workspace_root`, passing the dry-run flag. That helper already enforces the request-root gate without taking the mutator lock for dry-run and retains the lock for a real start. Keep the Rust pre-dispatch check only as an early diagnostic.

Regression test:

Create an open `finalizing` and an open `awaiting_resolution` operation. From a cwd outside the explicit request workspace, assert that Rust CLI, Python client/CLI, and direct core each reject dry-run and non-dry-run merge start with `OpenOperation`, while leaving records, refs, index, worktree, manifest, and lock unchanged.

### P2 - A failed synchronous Python merge never completes its operation record

Files and lines:

- `gwz-py/native/src/shims.rs:24-36` creates an `OperationRecorder`, but `?` returns on a handler error without calling `finish_error`.
- `gwz-py/native/src/dispatch/merge.rs:20-31` finishes the recorder only on a successful merge response.
- `gwz-py/native/src/operations.rs:129-151` defines stream completion solely as `state.result.is_some()`.
- `gwz-py/native/src/operations.rs:156-208` has success/error completion methods, but the synchronous error path invokes neither.
- `gwz-py/src/gwz/bridge.py:178-188` waits until that completion bit becomes true even after yielding all buffered events.

Violated requirement or invariant:

- The prompt's review area H requires event recording to complete exactly once on success and failure.
- `gwz-core/dev-docs/GwzMergeDesign.md:1085-1097` requires every invocation to close and `OperationFinished` on both success and failure.
- The remediation event contract at `dev-docs/GwzDevCodeM2a-RemPlan.md:401-415` says failure must close the invocation event stream.

Concrete reproduction:

I submitted an invalid native Python merge using request id `review56-failure`, caught the expected `GwzBridgeError`, and queried the native operation store by its deterministic id `op_review56-failure`.

```json
{
  "buffered_event_count": 2,
  "exception": "GwzBridgeError",
  "operation_result_present": false
}
```

The two core events are `OperationStarted` and `OperationFinished`, but a Python `client.events(operation_id)` consumer yields them and then continues waiting in repeated timeout cycles because the recorder has no result.

Impact:

Failed merge event consumers can hang indefinitely, and operation-result lookup never converges. This makes Python's failure lifecycle differ from core and from successful operations despite the visible finish event.

Recommended correction:

Make recorder ownership close exactly once around the synchronous handler. On `Err`, set a failed `OperationResult` with the original request/action and structured model error before returning the Python exception; on success, set the normal result. Avoid reducing typed model failures to a generic internal error. A scoped guard or a single dispatch wrapper should make an uncompleted recorder impossible.

Regression test:

Invoke validation, preflight, recovery-required, and store/backend failure paths with known request ids. For each, assert exactly one operation start and finish event, `wait_events(...).complete == true`, a present failed operation result with the correct action/error context, and prompt termination of `client.events()` without a timeout.

### P2 - Python `--jsonl` never emits merge events

Files and lines:

- `gwz-py/src/gwz/cli_merge.py:36-77` always calls the synchronous `Client.merge` method.
- `gwz-py/src/gwz/client.py:712-733` exposes `merge` but no merge streaming method.
- `gwz-py/native/src/dispatch/mod.rs:50-75` omits merge from submitted/streaming operations.
- `gwz-py/src/gwz/cli.py:65-88` treats `--jsonl` as the same final-response rendering mode as `--json`.
- `gwz-py/src/gwz/cli_render.py:21-49` defines the parity event serializer, but the CLI never calls it.
- `gwz-py/src/tests/test_cli_merge.py:99-107` explicitly accepts one JSON object for both `--json` and `--jsonl`; the serializer fixture test at lines 110-151 does not exercise CLI routing.
- `gwz-cli/docs/MachineOutput.md:138-184` publicly claims live event records followed by the response and Rust/Python event parity.

Violated requirement or invariant:

- REQ-089C (`GWZRequirements.md:637-642`) requires Rust, Python, JSON, and JSONL to report the same core semantics.
- `GwzMergeDesign.md:1085-1097` and the public machine-output contract require complete merge event streams.
- The remediation exit criteria at `GwzDevCodeM2a-RemPlan.md:533-534` require operation/member/state/artifact event parity across Rust and Python.

Concrete reproduction:

I ran the native Python CLI with `--jsonl --dry-run merge feature/source` in a real workspace and parsed every stdout line.

```json
{
  "exit_code": 0,
  "line_count": 1,
  "record_kinds": ["response"]
}
```

Core generated the operation events, but the Python CLI printed only the final response. The Rust CLI routes JSONL through its live event sink.

Impact:

Python JSONL consumers cannot observe operation, member, artifact, or state events and cannot satisfy the documented cross-driver protocol. The newly added fixture pins an unused serializer rather than end-to-end behavior.

Recommended correction:

Add merge to native submitted operations, expose a `Client.merge_stream`/result path, and route Python `--jsonl` through it. Serialize each event with `operation_event_json` as it arrives, then emit the final response/result contract in the same order as Rust. Keep `--json` as a single final response.

Regression test:

Run both actual CLIs against equivalent real workspaces for start success, conflict, continue, recovery rejection, abort, and failure. Parse stdout as JSONL and compare semantic record kinds, event sequence/order, structured member outcomes, artifact paths, final response, exit code, and absence of non-JSON stdout. The test must fail if `--jsonl` is parseable as a single response object.

## Design And Invariant Verification (A-J)

| Area | Disposition | Important evidence |
| --- | --- | --- |
| A. Durable record and recovery discovery | Mostly passes; blocked by Finding 1's incomplete exact intent | Initial/open and per-action records precede relevant Git actions; atomic store verification, duplicate/corrupt failure, unknown-field preservation, nearest-workspace recovery discovery, baseline manifest/lock preservation, terminal archive retry, and preservation-owner retention are implemented and tested. The pending record lacks the content identity needed for exact commit adoption. |
| B. Lifecycle, persistence, events, locking | Partial | Core state transitions, write-before-event order, member outcomes, artifact events, full native operation states, and mutator-lock ownership are substantially aligned. No M2a path marks the merge completed; success enters finalizing. Python failure recording does not close (Finding 3). |
| C. Status, drift, eligibility | Fails exact-adoption gate | Status is read-only and distinguishes advanced/rewound/diverged heads, missing objects/repositories, invalid paths, foreign sequencers, expected merge state, and operation drift. Finding 1 proves it clears real tree drift and reports optimistic eligibility. |
| D. Continue and retry | Fails exact-adoption gate | Whole-operation preflight, frozen order/parents/message, stopped-later-action behavior, typed failures, and store-failure reconciliation are covered. Finding 1 allows a crash-window row to absorb unrelated committed content. |
| E. Coordinated abort and interrupted rollback | Passes apart from inherited Finding 1 | Reverse whole-operation preflight, checked refs, exact native abort, externally restored no-op, post-result drift rejection, Git-success/row-failure retry, already durable rollback rows, archive retry, and terminal closing behavior are implemented. The false completed-exact classification can feed unsafe ownership into abort. |
| F. Git backend safety and TOCTOU | Fails merge-result identity | Full repository-state observation, checked merge/resolution/abort/reset primitives, exact parent/message creation, attribution, and idempotent restore behavior are strong. `commit_matches_merge` is too weak because it ignores the tree (Finding 1). |
| G. Central open-operation gate | Partial | Public mutators now generally resolve the request workspace and gate under the retained mutator lock; nested discovery and conditional stage behavior are tested. Core dry-run merge start remains a bypass (Finding 2). |
| H. Rust/Python protocol, events, output | Fails parity gate | Response/protocol fixtures are broad, Rust JSONL is wired, Python serializers match fixture shape, and successful Python event/result lookup works. Python failure completion and actual merge JSONL streaming fail Findings 3 and 4. |
| I. Release boundary and documentation | Partial | Final publication, root participation, preservation/GC, and additional strategies remain correctly deferred; hidden recovery forms remain integration-callable without broad release. The public JSONL claim is currently false for Python (Finding 4). |
| J. Tests and regression quality | Broad but insufficient for the four gaps | The suite has real mixed-Git workflows, late-blocker zero-mutation checks, store faults, invalid metadata recovery, foreign state, rollback retry, attribution, event ordering, and generated parity. Missing negative tests mirror each finding: different-tree same-message adoption, dry-run start while open, Python failed recorder completion, and end-to-end Python JSONL records. |

## Mandatory Adversarial Crash Matrix

| # | Boundary | Re-review result |
| --- | --- | --- |
| 1 | Durable record created, before first Git action | Safe at the exact frozen before state. Status discovers the record; continue re-preflights and abort can no-op/restore. Drift blocks rather than disappearing. |
| 2 | Git start/continue action succeeds, before participant outcome write | Fast-forward and exact native-conflict cases reconcile idempotently. Commit-producing cases are unsafe under Finding 1: a same-parent/same-message different-tree commit is silently adopted as exact and becomes abort eligible. |
| 3 | Participant write succeeds, before its event | Git-safe and idempotent. Durable state wins; retry does not repeat the action. The lost process-local event is not reconstructed, but status explains the durable outcome. |
| 4 | Transition to `executing`, before first continue action | Safe. A fresh continue or abort re-observes and preflights the whole operation before mutation; exact untouched rows remain retryable and drift blocks. |
| 5 | Transition to `rolling_back`, before first rollback | Safe. Retry reconstructs the complete reverse plan from durable rows and re-preflights before the first mutation. |
| 6 | Git rollback succeeds, before rollback row write | Safe and idempotent. Exact restored ref/native state is recognized as an already-restored no-op, then persisted without repeating destructive Git work. |
| 7 | Rollback row written, before next rollback | Safe. The durable row is verified/skipped; later unrelated work in that already-complete participant is reported but preserved because GWZ will not mutate it. |
| 8 | Transition to `aborted`, before archive | Safe and discoverable. The terminal record remains in open storage, the gate/status treat storage as closing/open, and retry archives without repeating repository rollback. |
| 9 | Archive publication succeeds, before command return | Safely closed. The verified done record remains retained; explicit-id retry is idempotent and does not duplicate the archive. Default status returns idle; historical status remains assigned to M3. |
| 10 | All member success reaches `finalizing`, before M2b publication | Intentional safe handoff: the accepted manifest/lock remain at baseline, the record stays open/discoverable, unrelated mutations are gated, and abort remains available. M2b should not build on this handoff until Findings 1-4 are remediated. |

## Verification Commands And Results

All mandatory commands were run against the reviewed working tree. None changed a tracked file.

| Command | Result |
| --- | --- |
| `cargo fmt --all -- --check` | Pass |
| `cargo test --workspace --all-targets --no-fail-fast` | Pass: 638 tests passed, 1 ignored, 0 failed across the reported Rust targets |
| `cargo clippy --workspace --all-targets --all-features -- -D warnings` | Pass |
| `cargo fmt --manifest-path gwz-py/Cargo.toml -- --check` | Pass |
| `cargo clippy --manifest-path gwz-py/Cargo.toml --all-targets --all-features -- -D warnings` | Pass |
| `gwz-py/.venv/bin/python -m maturin develop --manifest-path gwz-py/Cargo.toml` | Pass; editable native package built and installed |
| `cd gwz-py && .venv/bin/python scripts/regen_protocol.py --check` | Pass; generated Python API and IR are current |
| `cd gwz-py && .venv/bin/python -m pytest -q` | Pass: 268 passed |
| `cd gwz-cli && python3 scripts/generate_cli_reference.py --check` | Pass |
| `cd gwz-cli && uvx --from 'mkdocs-material>=9.5,<10' mkdocs build --strict --site-dir /tmp/gwz-m2a-review-56-docs` | Pass; strict documentation build completed. `uvx` emitted its upstream MkDocs 2.0 advisory only. |
| `bazel build //gwz-core:gwz_core //gwz-cli:gwz` | Pass: 2 targets |
| Root, `gwz-core`, `gwz-cli`, and `gwz-py` `git diff --check` | Pass |

Focused real-workspace reproductions also passed as tests of the review claims:

- a different-tree same-parent/message commit was classified `completed_exactly`, with no drift and both recovery actions eligible;
- a Python dry-run merge start was accepted while an earlier merge remained open in `finalizing`;
- a failed Python merge buffered two lifecycle events but had no operation result; and
- Python `--jsonl` emitted one `response` line and no event lines.

## Residual Risks And Missing Coverage

- This verification ran on macOS/arm64. Linux/Windows CI definitions and portable tests exist, but this review did not execute platform-specific ref-lock, path, rename, or directory-sync behavior on those platforms.
- Fault-injection tests exercise atomic-write and post-Git store failures, but no test can prove power-loss durability of every filesystem/drive combination.
- Most raw-Git races are checked at the action seam, but there is no stress harness repeatedly racing external Git processes against every preflight/action pair.
- The re-review target includes a large uncommitted remediation diff. Any subsequent edit is a new snapshot and should be rechecked before using this verdict.
- M2b publication, M2c root participation, and M3 preservation/history/GC remain intentionally deferred; their absence is not a finding here.

## Recommended Next Action

Do not begin parallel M2b implementation on this snapshot. First:

1. extend pending commit actions with exact tree/content identity and make mismatches ambiguous;
2. route dry-run merge start through the authoritative core gate;
3. close Python operation recording exactly once on synchronous failure; and
4. wire Python merge JSONL through a real event stream and final-result path.

Add the specified negative tests, rerun the full mandatory suite, and perform another focused re-review. M2b can safely begin once there are no P0/P1/P2 findings in these M2a safety and parity gates.
