# GWZ Merge M0 — Independent Code Review (F5)

Date: 2026-07-18. Reviewer: Claude (Fable 5), read-only review.

Scope: the completed M0 implementation of `gwz merge` against
`gwz-core/dev-docs/GwzMergeDesign.md`, `GwzMergePlan.md`, and
`GWZRequirements.md` (REQ-089 family).

Commit ranges reviewed:

| Repo | Range | Content |
| --- | --- | --- |
| gwz-core | `5e15ea6..c315edc` | Git primitives, checked merge seam, planning/preflight, start execution, response invariants |
| gwz-cli | `d45ec92..c1d6c24` | `gwz merge` surface, alias lowering, renderers, docs |
| gwz-py | `5137147..e482c21` | native dispatch, client, CLI, renderers, parity fixtures |
| root | `d3dac00..7f667f2` | lock/marker dogfood commit |

No P0 or P1 findings. One P2 and nine P3 findings follow, most severe first.

---

## Findings

### [P2-1] Preflight failures from `merge_analysis` / `merge_state` do not identify the failing member

- **Where:** [plan.rs:196](gwz-core/src/workspace_ops/merge/plan.rs:196) and
  [plan.rs:215](gwz-core/src/workspace_ops/merge/plan.rs:215) — both propagate
  the raw backend error with `?`, unlike every other rejection in
  `preflight_member`, which goes through `member_error(...)`.
- **Violated:** M0 gate "migrate existing branch-merge behavior" (plan §7,
  design §19 M0 "preserve current preflight … behavior") and REQ-089C's
  same-semantics surface requirement. The legacy `merge_preflight`
  (removed from `handle_branch.rs`) produced
  `"source ref 'x' not found for member 'mem_y'"` — the member was named.
- **Failure scenario:** a ten-member default merge where `feature/x` exists in
  nine members but not the seventh. The whole operation is (correctly)
  rejected, but the response error is
  `GitCommandFailed: revspec 'feature/x' not found; …` with
  `member_id: null`. The same happens for a member with a rebase in progress:
  `"repository has an integration operation in progress: RebaseMerge"` names
  no member (dirty/detached/unborn/lock failures *do* name the member).
- **Impact:** humans and JSON consumers cannot tell which repository blocked
  the batch; this is a diagnosability regression versus the legacy behavior M0
  was chartered to migrate, and it grows with workspace size.
- **Recommended correction:** in `preflight_member`, wrap the
  `merge_state`/`merge_analysis` error paths with member context (message
  prefix like the existing `member_error`, preserving the inner code), and/or
  attach `member_id`/`member_path` to the wire error at the handler boundary
  the way `participant_error` in `start.rs` does for execution errors.
- **Regression test:** real two-member workspace where the source branch
  exists only in the first member: assert the start rejection names the second
  member (message and, if adopted, `member_id`). Repeat with a
  rebase-in-progress member.

### [P3-2] Halted batches still advance the M0 lock for pre-failure clean outcomes; legacy did not

- **Where:** [start.rs:35](gwz-core/src/workspace_ops/merge/start.rs:35) —
  `advance_m0_lock` runs whenever `observed` is non-empty, including after
  `execute_plan` broke on an unexpected failure. The removed legacy
  `merge_branch` returned `partial_response` immediately on
  `Err(error) if mutated`, *before* its lock write, so a halted legacy batch
  never advanced the lock.
- **Violated (strict reading):** design §19 M0 "Preserve current preflight,
  conflict, and partial lock-advance behavior". The looser plan wording
  ("retain M0's documented partial lock advance for clean outcomes") supports
  the new behavior; the two texts diverge and the difference is user-visible.
- **Failure scenario:** member A merges cleanly, member B then hits an
  unexpected failure (e.g. target-ref drift). Response is honest
  (`Halted`/`Failed`, B `failed`, later members `unattempted`), but
  `gwz.lock.yml` now records A's post-merge commit, where pre-M0 it retained
  the stale pre-merge record.
- **Impact:** consumers comparing lock behavior across the M0 boundary see an
  undocumented change; the behavior is also untested (the fake-backend halt
  test does not check the lock and g23 has no halt scenario).
- **Recommended correction:** keep the new behavior (recording verified clean
  outcomes is more honest than legacy's stale lock) but say so: document the
  halt case in `merge.md` / design §4 alongside the conflict case, and pin it
  with a test. If strict preservation was intended instead, gate the advance
  on `execution.errors.is_empty()`.
- **Regression test:** real two-member workspace; force drift in member B
  between planning and execution (commit via raw git on B after acquiring the
  plan is hard to time — simplest is a fake at the workspace seam, or move B's
  branch from a pre-created hook); assert `Halted`, B `failed`, and the lock
  advanced for A only (or unchanged, per the chosen semantics).

### [P3-3] True-merge commit message omits the design's quoting and GWZ trailers

- **Where:** [plan.rs:242](gwz-core/src/workspace_ops/merge/plan.rs:242) —
  `format!("Merge {source} into {branch}")`.
- **Violated:** design §10: default message
  `Merge '<source-ref>' into '<target-branch>'` plus `GWZ-Merge-ID` and
  `GWZ-Operation-ID` trailers, recorded at start.
- **Scenario/impact:** every M0 true merge commits `Merge feature/x into main`
  (legacy format, no quotes, no trailers). Full compliance is impossible in M0
  (no merge id exists), but nothing records the divergence, and changing the
  message later is itself a user-visible change that will invalidate the
  message assertions in `g23.rs`.
- **Recommended correction:** reconcile deliberately: either amend design §10
  to state the trailer/quoted form begins at M1 (when the record and merge id
  exist), or adopt `Merge '<source>' into '<branch>'` + `GWZ-Operation-ID` now
  and add `GWZ-Merge-ID` at M1 so the format changes once.
- **Regression test:** the existing
  `first_class_true_merge_uses_request_git_identities_and_planned_message`
  assertion, updated together with whichever format is chosen.

### [P3-4] No real-repository test for the halt path

- **Where:** [g23.rs](gwz-core/src/workspace_ops/tests/g23.rs) — covers
  preflight rejection, dry-run, FF, identity, conflict-continues-batch; the
  `Failed → Unattempted → Halted` composition (and its lock effect) exists
  only in fake-backend unit tests
  ([start.rs:464](gwz-core/src/workspace_ops/merge/start.rs:464)).
- **Violated:** plan §14.3 (real Git backend tests for behavior fakes could
  conceal) and the design test-matrix row "unexpected failure after an earlier
  mutation".
- **Impact:** the most safety-relevant batch semantics (stop-on-unexpected
  failure, honest partial state, lock behavior under halt) are not proven
  against real Git.
- **Recommended correction / test:** same scenario as P3-2; a mid-batch
  `MergeDrift` from a raw-git branch move is the most realistic reproducible
  trigger.

### [P3-5] Cross-driver parity fixture is enforced only from the Python side

- **Where:**
  [merge_response.json](gwz-py/src/tests/fixtures/cli_parity/merge_response.json)
  is asserted by `test_cli_merge.py` only; no gwz-cli test consumes it
  (searched `gwz-cli/src` for `cli_parity`/fixture references: none). The Rust
  side pins only spot fields in
  [g02.rs](gwz-cli/src/tests/g02.rs).
- **Violated:** plan M0-C2 "cross-driver machine-output parity fixtures" — as
  delivered, the fixture *documents* Rust's shape rather than *verifying* it.
- **Failure scenario:** a future Rust renderer edit (key rename, enum label
  change in [merge_render.rs:58](gwz-cli/src/merge_render.rs:58)) passes all
  Rust tests; Python still matches the now-stale fixture; drivers silently
  diverge.
- **Impact:** the exact drift class the parity fixtures exist to prevent.
- **Recommended correction:** add a Rust test that builds the same synthetic
  `MergeResponse` and byte-compares `response_json` output with the shared
  fixture file (or generate the fixture from the Rust CLI in CI).
- **Regression test:** the Rust-side fixture comparison itself.

### [P3-6] Machine output silently drops protocol fields that M1 will populate

- **Where:** [merge_render.rs:58](gwz-cli/src/merge_render.rs:58) and
  [cli_render.py:435](gwz-py/src/gwz/cli_render.py:435) — neither emits
  `operation_drift`, `preservation`, or `publication_step`, and
  `participant_counts` omits `continued`/`aborted`/`rolled_back`.
- **Violated:** nothing in M0 (the fields are always empty/None); design §20
  requires drift/preservation in JSON from M1 on.
- **Failure scenario:** M1 lands, core populates `operation_drift`, and JSON
  consumers still see nothing until both renderers and the fixture are
  extended in lockstep — an easy omission because nothing fails today.
- **Impact:** foreseeable cross-driver/output-completeness trap at the next
  milestone.
- **Recommended correction:** emit the fields now as empty arrays / null
  (cheap, keeps the parity fixture shape stable across M1) or carry an
  explicit M1 task for both renderers plus fixture.
- **Regression test:** extend the shared fixture with the emitted empty
  fields.

### [P3-7] Merges of unrelated histories are now rejected — including via `pull --sync merge` — without a pinned test

- **Where:** [gitbackend.rs:1884](gwz-core/src/git/gitbackend.rs:1884)
  `classify_merge` returns `GitCommandFailed: "target and source do not share
  a merge base"`; the reimplemented `merge_upstream`
  ([gitbackend.rs:884](gwz-core/src/git/gitbackend.rs:884)) routes the pull
  merge path through it. Legacy `merge_upstream` used libgit2
  `merge_analysis` + `merge`, which merges unrelated histories against an
  empty base.
- **Violated:** no requirement — the new behavior matches git porcelain's
  "refusing to merge unrelated histories" and is arguably a fix — but it is an
  unannounced behavior change to a *pre-existing* surface (`pull --sync
  merge`) made inside the merge milestone, with no test on either the merge or
  pull path.
- **Failure scenario:** a member whose local branch was recreated as an orphan
  (or whose remote was force-replaced) previously pull-merged (possibly with
  conflicts); it now fails with a backend error.
- **Impact:** low-probability but surprising pull failure; nothing pins the
  intended semantics.
- **Recommended correction:** keep the porcelain-faithful rejection; add a
  real-repo test for both `gwz merge` and `pull --sync merge` against
  unrelated histories, and mention the change in release notes.
- **Regression test:** as above.

### [P3-8] git2-rejectable identities pass request validation and can halt mid-batch after earlier members mutated

- **Where:** [model/mod.rs:305](gwz-core/src/model/mod.rs:305)
  `GitObjectIdentity::validate` checks only non-empty name/email and timezone
  range; [gitbackend.rs:2135](gwz-core/src/git/gitbackend.rs:2135)
  `signature_from_identity` then calls `git2::Signature::new`, which rejects
  `<`, `>`, and control characters at execution time.
- **Violated:** design §7 preflight item "Git identity is available if the
  planned merge may create a commit" — availability is guaranteed by the
  `merge_signature` fallback and whole-request `attribution.validate()` runs
  before mutation in `OperationContext::from_meta`, but git2-invalid *values*
  slip through and only fail per-participant.
- **Failure scenario:** request author name `"Alice <work>"`; member 1's plan
  is fast-forward (publishes its ref, no signature needed), member 2 needs a
  true merge and fails constructing the signature → batch halts with member 1
  mutated, though complete preflight could have rejected the request before
  any mutation. (Ordering inside the seam is correct: `merge_signatures` runs
  before `repo.merge`, so the *failing* member itself is untouched.)
- **Impact:** avoidable partial state from a request-shape defect;
  low likelihood since drivers construct these values.
- **Recommended correction:** extend `GitObjectIdentity::validate` to reject
  angle brackets and control characters (mirroring git's signature rules); the
  existing context-construction call site then rejects the whole request
  before mutation with no new plumbing.
- **Regression test:** `validate()` rejects `"A <b>"`; `handle_merge` with
  such attribution fails before any member mutates.

### [P3-9] Docs and diagnostics around rejected global options

- **Where:** the generated merge section of
  [docs/CLI.md](gwz-cli/docs/CLI.md) lists `--sync`, `--remote`, `--jobs`,
  `--max-per-host`, `--progress-interval`, `--partial`, `--force` under
  `gwz merge`, all of which core rejects for merge
  ([validate.rs:54-71](gwz-core/src/workspace_ops/merge/validate.rs:54));
  the catch-all rejection at
  [validate.rs:70](gwz-core/src/workspace_ops/merge/validate.rs:70)
  ("merge request contains an unrelated operation policy field") does not name
  the offending field; and
  [clirequest.rs](gwz-cli/src/clirequest.rs) `merge_meta` silently strips the
  *defaulted* `--progress-interval` while an *explicit* one is rejected —
  correct, but the asymmetry plus unnamed-field error makes the rejection hard
  to interpret.
- **Violated:** design §16 "typed protocol-validation errors, not silently
  ignored input" is honored; this is a diagnosability/doc-accuracy gap only.
  ([merge.md](gwz-cli/docs/commands/merge.md) does document `--partial`/
  `--force`/`@root` rejection — good.)
- **Impact:** a user following the generated reference passes `--sync merge`
  or `--jobs 4` and gets a message that does not say which option to remove.
- **Recommended correction:** name the field in each rejection message; if the
  reference generator ever supports per-command notes, mark the inapplicable
  globals on the merge page.
- **Regression test:** validation-message assertions naming the field.

### [P3-10] Module-wide `#![allow(dead_code)]` on the merge module

- **Where:** [merge/mod.rs:1](gwz-core/src/workspace_ops/merge/mod.rs:1).
- **Violated:** maintainability only. The allow exists for the frozen I0/M1
  seams (`MergeStore`, `MergeOperationRecord`, `response.rs`), but it blankets
  the live M0 code in `plan.rs`/`start.rs`/`validate.rs`, so genuinely dead M0
  code would not be flagged.
- **Recommended correction:** move `#[allow(dead_code)]` onto the specific
  seam items and delete it as M1 consumes them.
- **Regression test:** none (compiler lint).

---

## Review-area verification summary

All seven requested areas were verified; findings above are the only
deviations observed.

1. **Selection and validation** — default selection is active members in
   manifest order with root excluded (`resolve_targets` +
   `CommandDefaultTargets::Members`); explicit `@root` (in `targets`,
   `member_ids`, or `paths`) returns `RootMergeNotYetSupported`; exclusions
   are applied before the root-participation decision (`@root` selected and
   excluded → members-only, no error; test pinned); `@all` selects members
   only, per REQ-089D's "only when explicitly selected as `@root`".
   `--partial`, `--force`, skip policy, `ff-only`/`no-ff`, `-m`,
   `preserve`, and misplaced lifecycle fields all return stable typed errors
   from the core-owned matrix (`validate.rs`), for both drivers and the alias.
   Both CLIs lower `branch --merge` to `MergeRequest(start)` before any
   `BranchRequest` exists; protocol `BranchOp.merge` is intercepted at the top
   of `handle_branch` with `DeprecatedOperation` (wire value 37, `BranchOp`
   merge=3 retained), so the `unreachable!` in its dispatch arm is safe and no
   second merge implementation is reachable (legacy `merge_branch`,
   `merge_preflight`, and summaries were actually deleted).
2. **Preflight and frozen planning** — every selected member is preflighted
   (short-circuit on first failure, before any mutation; g23 proves an
   earlier member is untouched when a later one is dirty); order is manifest
   order regardless of selection order; members must be Git-kind, locked,
   materialized, clean (`is_dirty` covers staged/unstaged/untracked/
   unresolved), attached, born, free of native merge state
   (`merge_state`) and of rebase/cherry-pick/etc.
   (`ensure_no_integration_in_progress` inside `merge_analysis`); sources
   resolve per-repository via `revparse` + `peel_to_commit` (tree/blob/missing
   rejected, g12); the plan freezes branch, before OID, source OID, analysis
   kind, and message, and execution consumes only frozen OIDs. Dry-run takes
   the read-only path before `WorkspaceMutatorLock::acquire`, and g23 proves
   HEAD, lock bytes, and merge state are unchanged.
3. **Git mutation safety** — `merge_upstream_checked` locks
   `refs/heads/<branch>` via a git2 transaction, then revalidates state,
   HEAD attachment, and the exact expected-before OID under the lock;
   up-to-date releases the lock untouched; fast-forward does a safe
   checkout then `set_target` under the lock; true merge resolves
   author/committer (independent fallbacks: request identity → repo config →
   `gwz <gwz@localhost>`, g12-tested) *before* mutating, creates a refless
   commit, verifies parents/message/author/committer against the plan, then
   publishes under the lock and `cleanup_state`s; every path ends in
   `verify_merge_result` (branch, HEAD, clean status, no native state) or, for
   conflicts, verifies persisted `MERGE_HEAD == source`, unchanged HEAD, and
   matching conflict paths. Failure paths never publish an unintended ref and
   never report unverified success; the residual failure windows (checkout
   done / ref not yet set; commit created / publish failed) leave a *reported*
   `failed` member with a mutated worktree — inherent to M0's no-rollback
   scope (see residual risks).
4. **Batch execution semantics** — expected conflicts continue the batch
   (g23: conflict in member 1, clean FF in member 2, lock advanced for the
   clean member only); unexpected errors stop the batch, mark the failed
   member `failed` and the remainder `unattempted` (fake-backend tests);
   execution uses frozen source OIDs, never ref names; lock/boundary write
   failures are appended to `errors` and force `Halted`/`Failed` rather than
   being swallowed. Deviation from legacy in the halt case is P3-2.
5. **Response correctness** — action is `Merge` in meta for start, dry-run,
   and the alias (core, CLI, native, and workflow tests); aggregate/state/open
   agree (`Accepted+Completed+closed` dry-run; `Ok/Noop+Completed+closed`
   clean; `Conflicted+AwaitingResolution+open`; `Failed+Halted+open`);
   `Halted` wins over `AwaitingResolution` when both occur; failed/unattempted
   rows carry `live_commit: null`; conflicted rows carry `live=before`,
   `resulting=null`, retained sorted conflict paths, and member identity;
   participant errors carry `member_id`/`member_path`/`target_kind`; counts
   are derived from the same rows they accompany.
6. **Driver and machine-output parity** — Rust and Python expose identical
   start/dry-run/alias surfaces, identical hidden reserved flags with local
   `InvalidRequest` for ambiguous combinations, identical JSON envelope and
   merge payload key sets (meta keys, `error_json` five-key shape, Debug-style
   labels), and matching exit codes for merge flows (0 clean/noop/accepted,
   1 conflicted/halted/core errors, 2 semantic parse errors). Python preserves
   structured conflicted/halted `MergeResponse` values through
   `GwzOperationError` and renders them in human/JSON/JSONL (real-repo native
   test). M0 conflict guidance in both drivers is exactly the design's honest
   text and never mentions `--continue`/`--abort`/`--status`. Protocol
   artifacts and the generated CLI reference are current (`regen.py --check`,
   `generate_cli_reference.py --check` both clean). Gaps: P3-5, P3-6.
7. **Regression and maintainability** — legacy merge code is removed;
   `pull --sync merge` still flows through `merge_upstream`, whose new
   internal dirty/attached checks are consistent with pull's pre-existing
   `pull_dirty_guard` (which already rejected untracked files for integrating
   modes), and whose conflicted-path contract (MERGE_HEAD content, returned
   conflict list) is unchanged for `resolve_repairable_root_conflicts`; the
   `merge_upstream_checked` trait default is a typed
   `UnsupportedOperation` error, so no unchecked production path can appear by
   default; real-repository coverage exists for every integration kind,
   drift rejection, identity handling, and conflict persistence. Gaps: P3-4,
   P3-7, P3-10.

---

## Commands run and results

From `/Users/owebeeone/limbo/gwz-dev` (macOS arm64, Darwin 25.5.0):

| Command | Result |
| --- | --- |
| `git status --short` | only two pre-existing untracked docs (`dev-docs/AnnounceGwzHn.md`, `dev-docs/HN-AnnounceGWZ.md`); working trees otherwise clean |
| `git -C gwz-core diff 5e15ea6..c315edc` (and cli/py ranges) | inspected in full |
| `cargo fmt --all -- --check` | pass |
| `cargo test -p gwz-core -p gwz` | pass — 554 tests, 0 failures (exit 0 verified directly, not through a pipe) |
| `cargo clippy --workspace --all-targets -- -D warnings` | pass (exit 0) |
| `gwz-core: protocol/regen.py --check` | "OK -- committed protocol artifacts are current." (taut-proto 0.8.1) |
| `gwz-cli: scripts/generate_cli_reference.py --check` | pass (exit 0) |
| `gwz-py: python -m maturin develop` | built and installed editable wheel |
| `gwz-py: run_tests.py` | pass — protocol regen check OK, 261 pytest tests passed |

## Areas that could not be verified

- **Platform coverage:** everything ran on macOS arm64 only; no Linux or
  Windows execution (ref-transaction, path, and filesystem-case behavior
  unverified there).
- **True cross-driver byte parity:** no harness executes the Rust binary and
  the Python CLI against the same live workspace and diffs their machine
  output; parity rests on the shared hand-curated fixture (P3-5) and
  key-by-key inspection performed in this review.
- **Concurrency/failure injection:** the `lock_ref` contention path,
  `set_target`/`transaction.commit` failure windows (disk full, permission
  loss), and racing raw-git writers are not exercisable with the present test
  harness; they were verified by code inspection only.
- **Bazel/razel builds:** not exercised (cargo only, per the suggested
  verification commands).

## Residual risks inherent to documented M0 scope (not defects)

- **No durable record:** a crash mid-batch leaves only raw Git state as
  evidence; conflicted/halted responses report `open: true` with
  `merge_id: null` and there is no `--status`, `--continue`, or coordinated
  abort until M1/M2. JSON consumers must treat `open` as "work remains", not
  "a recoverable operation exists".
- **Failure windows leave mutated members:** a fast-forward whose ref
  publication fails leaves the worktree/index at the source tree with the
  branch at `before`; a true merge whose commit/publication fails leaves
  staged merge state (and `MERGE_HEAD`). Both are reported as `failed` with
  no invented `live_commit`, and recovery is ordinary Git — but M0 performs no
  rollback and the response does not describe the worktree residue.
- **Partial lock advancement:** the accepted lock advances for verified clean
  outcomes while other members are conflicted (or, per P3-2, halted); the
  lock-freeze semantics arrive at M1 and are already flagged in the design as
  a release-note-worthy change.
- **Dry-run is advisory:** it holds no mutator lock and can be stale by
  execution time; the real start repeats preflight under the lock (documented,
  and by design its plan cannot promise a true merge is conflict-free —
  `prediction_complete: false`).
- **Worktree races:** the checked seam pins the target ref under a ref lock,
  but worktree/index mutations by concurrent raw-git users between
  revalidation and mutation can still surface as backend failures; this is
  inherent to Git and fails closed.
