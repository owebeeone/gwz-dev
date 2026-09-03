# GWZ Merge M4 Independent Review Remediation Plan

Date: 2026-07-26

Execution status: **complete; local, independent re-review, and
release-platform gates green**

Inputs:

- the independent M4 code review completed on 2026-07-26;
- `gwz-core/dev-docs/GwzMergeDesign.md`;
- `gwz-core/dev-docs/GwzMergePlan.md`; and
- the uncommitted M4 implementation in `gwz-core`, `gwz-cli`, and `gwz-py`.

## 1. Gate decision

The initial M4 implementation passed its complete local technical gate, but
the independent review found two P1 defects, one P2 response-completeness
defect, and one P3 release-matrix gap. At that point M4 could not be accepted,
committed as complete, or used to begin M5 until this plan was implemented and
independently re-reviewed.

The passing test baseline does not invalidate the findings. Both P1 scenarios
fall outside the current fixtures:

1. a conflicted in-memory index can be reported as clean when its only path is
   not valid UTF-8; and
2. pull simulates one exact source commit but later resolves and integrates a
   mutable remote-tracking ref.

Both can make a non-partial `pull --sync merge` begin local mutation after a
false-clean preflight, violating the central M4 safety guarantee.

## 2. Finding dispositions

| Finding | Disposition | Required correction |
| --- | --- | --- |
| P1: non-UTF-8 conflict path is dropped and an empty list is treated as clean | **Fix first** | Use the merge index's conflict state as the sole clean/conflicted truth. Render every raw conflict path with one deterministic byte-safe representation. |
| P1: pull simulates an exact commit but executes a mutable remote ref | **Fix before any release-matrix expansion** | Freeze expected local and exact fetched source commits, freeze the prepared clean result, revalidate the complete selection, and execute only the checked prepared action. |
| P2: rename conflicts project only one stage path | **Fix with the path correction** | Collect paths independently from ancestor, ours, and theirs, then sort and deduplicate the rendered values. |
| P3: mixed root/member M4 and Python `--ff-only` routing proof is incomplete | **Close as required regression coverage** | Add real mixed-selection prediction and `--ff-only` rows plus an explicit Python request-lowering assertion. |

No P0 was reported.

## 3. Why a non-UTF-8 Git path is possible

Git tree and index paths are byte strings. They are not defined as Unicode.
POSIX APIs generally treat a Unix filename as bytes other than NUL and `/`,
although a particular platform or filesystem may impose additional
constraints. Git objects created on one platform may also be cloned or
inspected on another.
For example, the raw path bytes below are legal Git data even though they are
not valid UTF-8:

```text
config-\xFF.toml
```

libgit2 exposes each conflict-stage path as raw bytes. The current helper uses
`std::str::from_utf8` and adds the path only on success. An invalid path is
therefore silently omitted. The current simulation then asks whether the
rendered path vector is empty instead of asking whether the in-memory index
has conflicts. If the invalid path is the only conflict, a conflicted index is
misclassified as `Clean`.

The correction separates two concerns:

- `index.has_conflicts()` determines merge truth; and
- a byte-safe formatter determines how every conflicted path is reported.

Diagnostic rendering must never decide whether a merge is safe to execute.
The shipped release matrix is Windows, macOS, Linux x86, and Linux arm64.
The correction must therefore not depend on the host filesystem being able to
create an invalid-UTF-8 filename. Byte-rendering fixtures must be constructed
directly from raw bytes and produce identical protocol output on every release
target.

## 4. Scope and non-negotiable invariants

This remediation is limited to:

- conflict-state classification and conflict-path projection;
- the M4 `pull --sync merge` prediction-to-execution handoff;
- exact pull-plan revalidation before the first local mutation;
- mixed member/root and driver regression coverage;
- documentation and milestone status; and
- an independent re-review.

It does not add `--no-ff`, custom merge messages, `--into`, snapshot sources,
new partial-merge policy, or a durable coordinated lifecycle for pull.

The corrected implementation must preserve these invariants:

- merge simulation never writes a ref, HEAD, index, worktree, native
  integration state, accepted lock, marker, or merge-operation record;
- a conflicted index is never reported as clean, even when no path can be
  represented normally;
- every conflict stage contributes its path to diagnostics;
- non-partial `pull --sync merge` completes fetch, simulation, preparation,
  and selection-wide revalidation before the first local branch, index,
  worktree, or workspace-lock mutation;
- fetch and remote-tracking-ref updates remain the documented preflight
  exception;
- execution integrates the exact source commit that was simulated;
- a changed target branch, remote-tracking ref, prepared tree, result class,
  or native Git state returns a structured target-scoped drift/recovery error
  before that repository is mutated;
- an explicitly selected or default-selected root remains ordered before
  member execution for pull, but it is not applied until the complete
  selection passes the final barrier;
- `gwz merge --ff-only` remains selection-wide and durable;
- pull remains outside the coordinated `gwz merge` status/continue/abort
  lifecycle; and
- no public protocol field or enum value is renumbered or repurposed.

## 5. Lead-owned interface checkpoint R4-0

Freeze these internal interfaces and focused contract tests before the two P1
work packages proceed.

### 5.1 Conflict truth and byte-safe path projection

Retain `GitMergeSimulation::Clean | Conflicts(Vec<String>)`; no protocol
change is required.

Define one shared conflict-path projection contract:

1. Clean/conflicted classification comes from `Index::has_conflicts()`.
2. For a conflicted index, inspect ancestor, our, and their entries
   independently.
3. Valid, safe UTF-8 paths retain their ordinary spelling.
4. A path requiring byte representation is double-quoted; `"` and `\` are
   escaped, familiar control bytes use their conventional escapes, and every
   other invalid byte uses uppercase `\xNN`.
5. Sort and deduplicate only after rendering all stage paths.
6. If the index reports conflicts but iteration or projection cannot produce
   a trustworthy result, return a typed Git error. Never return `Clean`.

The same helper remains authoritative for simulated conflicts and native merge
results so dry-run, pull, status, human output, JSON, and JSONL do not disagree.
The escaped value is diagnostic text, not a promise that it can be passed back
as a UTF-8 path selector.

### 5.2 Exact prepared pull actions

Every mutating pull plan records:

```text
target branch
expected local commit
exact fetched source commit
remote-tracking ref used during planning
planned action/result class
```

For fast-forward and merge actions used by `--sync merge`, also retain the
existing `GitPreparedMerge` value produced during preflight:

- behind becomes `FastForward`;
- divergent clean becomes `Commit` with its exact prepared tree and
  signatures; and
- divergent conflict never becomes an executable action.

Preparation may create an unattached tree object, as the established merge
backend already permits, but it must not change observable repository state.

Execution uses `execute_prepared_merge_upstream_checked` with the frozen local
and source commits. It must not call `merge_upstream` or resolve the remote ref
again. A prepared-clean mismatch returns `MergeRecoveryRequired` without
entering native merge state.

Root fast-forward and clean root merge use the same prepared checked path.
This removes the current check-then-call gap around root fast-forward and
prevents root and member execution from implementing different M4 guarantees.

Rebase and reset retain their existing policy semantics, but their plan must
pass the frozen source commit rather than re-resolve the remote ref during
application. A general durable or atomic rebase/reset redesign remains outside
M4.

### 5.3 Selection-wide final revalidation barrier

After every selected repository has fetched, simulated, and prepared, and
immediately before root or member application:

- verify each target is still on the frozen attached branch and local commit;
- verify no foreign native integration state or newly disallowed dirt exists;
- verify each planned remote-tracking ref still equals its frozen source
  commit;
- validate every prepared merge against the exact current repository state;
  and
- attach member id/path or `@root`/`.` to every error.

The barrier is read-only. Any failure rejects before GWZ applies the root or
any member. Checked prepared execution remains mandatory after the barrier so
a raw-Git race after revalidation fails closed at the individual ref lock.

Introduce a test-only hook or backend wrapper that can move a remote-tracking
ref after simulation but before this barrier. Do not add a production
environment-variable race hook.

## 6. Work packages

After R4-0 compiles, R4-A and R4-B have disjoint ownership and may proceed in
parallel. R4-C consumes both corrections. The lead alone integrates shared
module registration, fixtures, and milestone documents.

### R4-A — Conflict classification and complete path projection

Owned paths:

- `gwz-core/src/git/gitbackend/merge_support.rs`;
- `gwz-core/src/git/gitbackend/merge_prepared.rs`; and
- a new focused Git test module rather than growing an existing test file past
  its ceiling.

Required work:

1. Add the byte-safe path renderer with table-driven byte tests.
2. Collect all ancestor/our/their paths for each conflict.
3. Make `merge_simulate` branch on `index.has_conflicts()`.
4. Assert that a conflicted index cannot fall through to `Clean`.
5. Preserve sorted, deduplicated output for ordinary UTF-8 content conflicts.
6. Confirm native merge conflict reporting consumes the same helper.

Required regressions:

- an ordinary content conflict remains `Conflicts(["path"])`;
- a Unix invalid-UTF-8 conflict is not `Clean` and has pinned escaped output;
- a rename/rename conflict reports the distinct ancestor, our, and their
  paths;
- duplicate stage paths collapse to one row;
- simulated and native conflict projection agree; and
- HEAD, refs, index, worktree, and native merge state remain byte-for-byte
  unchanged after simulation.

### R4-B — Exact pull plan, barrier, and checked execution

Owned paths:

- `gwz-core/src/workspace_ops/pull_head_plan.rs`;
- `gwz-core/src/workspace_ops/pull_head_member_preflight.rs`;
- `gwz-core/src/workspace_ops/pull_head_merge_preflight.rs`; and
- a new pull race/regression test module. Do not grow the existing 400-line
  `g24.rs`.

Required work:

1. Freeze expected local, exact source, remote ref, and prepared result in
   executable pull actions.
2. Prepare clean merge-sync actions during preflight.
3. Add one selection-wide revalidation barrier after all plans exist.
4. Apply root only after the barrier succeeds.
5. Execute root and member fast-forward/merge through the prepared checked
   backend.
6. Pass exact source commits to rebase/reset application without otherwise
   redesigning those modes.
7. Preserve partial-pull `PredictedConflict` skips as non-executable rows.
8. Preserve structured root/member errors and current aggregate-status rules.

Required regressions:

- move a member remote-tracking ref from clean R1 to conflicting R2 after
  simulation; assert no root/member local mutation and no native merge state;
- move the target branch after preparation; assert target-scoped drift and no
  GWZ mutation;
- change the prepared clean result before execution; assert
  `MergeRecoveryRequired`, not native conflict;
- prove successful member, root, and mixed pulls integrate the exact simulated
  source commit even if the remote ref moves after the final barrier;
- prove a later member failure detected by the barrier leaves an earlier
  fast-forwardable root and member untouched;
- retain the existing non-partial, partial-skip, root-conflict, root
  fast-forward, and clean-root-merge tests; and
- compare HEAD, index, worktree, lock, and native state before and after every
  rejection.

### R4-C — Release matrix, drivers, and documentation

Owned paths:

- new focused M4 scenario modules under `gwz-core/src/workspace_ops/tests/`;
- existing Rust/Python request and canonical parity tests without creating a
  new god file;
- `gwz-cli/docs/commands/merge.md`;
- `gwz-cli/docs/commands/pull.md`;
- `gwz-core/docs/Protocol.md` if byte-path rendering is documented there; and
- generated CLI documentation.

Required coverage:

- run the portable byte-rendering and conflict-classification tests across the
  Windows, macOS, Linux x86, and Linux arm64 release matrix;
- construct invalid-byte fixtures below the filesystem boundary so Windows or
  macOS path restrictions cannot silently skip the regression case;
- mixed member-plus-root dry-run with clean and conflicted predictions,
  stable paths, and complete non-mutation evidence;
- mixed `--ff-only` rejection where an earlier participant could
  fast-forward and a later root/member requires a true merge;
- mixed `--ff-only` success across changing and up-to-date rows;
- Rust request lowering for `--ff-only`;
- Python request lowering proving `MergeMode.ff_only` reaches the client when
  used alone;
- Rust/Python human, JSON, and JSONL parity for predicted paths; and
- documentation that pull executes the exact fetched/preflighted source and
  that remote-tracking refs may still change during fetch.

The existing 504-line `gwz-py/src/tests/test_cli_merge.py` must not grow merely
for the routing assertion. Replace or parameterize an existing routing row, or
move a coherent M4 test group into a focused module.

### R4-D — Integration and independent re-review

The lead:

1. reviews both P1 fixes against the frozen R4-0 contracts;
2. confirms no executable pull merge retains only a mutable ref;
3. confirms no clean/conflict decision depends on a rendered path vector;
4. runs the full verification gate;
5. updates `GwzMergePlan.md` with exact current evidence; and
6. requests an independent re-review using the original findings as explicit
   exit criteria.

No remediation implementation commit or push is included unless requested
separately.

## 7. Sequencing and ownership

```text
R4-0 interface checkpoint
        |
        +--> R4-A conflict truth/path projection --+
        |                                          |
        +--> R4-B exact pull handoff/barrier -------+--> R4-C matrix/docs
                                                       |
                                                       +--> R4-D re-review
```

R4-A owns the shared conflict helper while R4-B consumes its result. R4-B must
not independently encode paths. R4-C begins mixed behavioral fixtures only
after both P1 corrections compile so its tests pin the corrected contract.

New or materially enlarged implementation and focused test files remain at or
below 500 lines. Existing oversized files must not grow to absorb remediation
logic.

## 8. Verification gate

Focused red-before-green evidence is mandatory for both P1 findings.

Then run:

```text
cd <workspace-root>
cargo fmt --all -- --check
cargo test -p gwz-core
cargo test -p gwz
cargo test --workspace
cargo clippy --workspace --all-targets -- -D warnings

cd gwz-core
python protocol/regen.py --check

cd ../gwz-cli
python scripts/generate_cli_reference.py --check

cd ../gwz-py
.venv/bin/python -m maturin develop
.venv/bin/python run_tests.py

cd ..
bazel build //gwz-core/... //gwz-cli/...
gwz forall --all --no-banner -- git diff --check
gwz status
```

The current baseline is 583 passing core tests with one intentional ignore and
320 passing Python/native tests. The remediation must not reduce either count.
The configured Bazel labels currently have build targets but no test targets;
record that fact rather than treating `bazel test`'s “no test targets” exit as
a code failure.

Implementation evidence on 2026-07-26:

- 596 core tests passed with one intentional ignore;
- the complete Rust workspace and `gwz` package suites passed;
- 320 Python/native tests passed after rebuilding the extension;
- strict Clippy, formatting, protocol regeneration, CLI-reference generation,
  Bazel build, and cross-repository diff hygiene passed;
- portable raw-byte fixtures and a same-repository simulated/native
  rename/rename conflict-path parity fixture passed;
- the pre-existing oversized `workspace_ops/tests/g01.rs` was reduced to 426
  lines, with its 355-line tracking backend isolated in a focused support
  module; and
- pre-barrier ref/head drift, post-barrier ref movement, and mixed
  member/root prediction and `--ff-only` regressions passed.

The completed release build matrix covers Windows, macOS, Linux x86, and Linux
arm64. Those release builds are the platform evidence for exit criterion 7;
the portable object-database and byte-table tests ensure that evidence does
not depend on creating an invalid-UTF-8 filename on the host filesystem.

The final independent re-review reports no P0/P1/P2 defect and no remaining
M4-specific P3 coverage gap. The completed release builds supply the
cross-platform evidence, so the M4 remediation gate is fully accepted.

## 9. Exit criteria

M4 remediation is complete only when:

1. a conflicted index can never produce `GitMergeSimulation::Clean`;
2. invalid-byte and rename-stage paths have deterministic complete output;
3. non-partial pull never applies a source commit other than the exact
   simulated/prepared commit;
4. ref/head/prepared-result drift before the final barrier leaves every local
   selected repository unchanged;
5. a post-barrier race is stopped by checked prepared execution without
   entering native conflict state;
6. member-only, root-only, and mixed prediction and `--ff-only` matrices pass;
7. the byte-path contract passes on Windows, macOS, Linux x86, and Linux
   arm64 without relying on native creation of an invalid-UTF-8 filename;
8. Rust and Python routing and rendering parity pass;
9. the full verification gate is green;
10. all four independent-review findings are explicitly closed; and
11. an independent re-review reports no P0/P1/P2 defect and no remaining
    M4-specific P3 coverage gap.

Only then may the main plan mark the M4 release gate accepted and allow M5 to
begin.
