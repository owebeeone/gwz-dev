# Independent GWZ Merge M1/M2a Code Review Prompt

You are conducting an independent, read-only code review of the completed GWZ
merge M1 and M2a implementation. Review it as safety-critical Git tooling:
prioritize data-loss risks, incomplete whole-operation preflight, ambiguous
crash recovery, unsafe ref/worktree mutation, state-machine errors, and
Rust/Python behavioral drift over style or refactoring preferences.

Replace `XX` with your reviewer designation and write your completed report to:

```text
gwz-dev/dev-docs/GwzDevCodeM2a-ReviewXX.md
```

For example, designation `F5` writes
`gwz-dev/dev-docs/GwzDevCodeM2a-ReviewF5.md`.

Do not modify implementation code, tests, protocol files, existing
documentation, workspace artifacts, or another reviewer's report. The review
report above is the only file you may create or edit. Do not stage, commit, or
push anything.

Two reviewers are being engaged. Preserve independence: do not read any other
`GwzDevCodeM2a-Review*.md` report before completing your own analysis. You may
read the older M0 review and remediation records because they define regression
history, but form your own conclusions from the current code and design.

## Repository and checkpoint

Work from:

```text
/Users/owebeeone/limbo/gwz-dev
```

The coordinated M2a checkpoint is:

| Repository | M1-0 base | M2a checkpoint | Primary review range |
| --- | --- | --- | --- |
| workspace root | `3d6ce941e7f79eb7aa4e93a1ddb4a865f17f9e90` | `585e44d07f13880fe60072b7409a9d559f64ae53` | `3d6ce941..585e44d` |
| `gwz-core` | `8a0ad1732ed4efc51bb6d264067c18b1f7e3f920` | `cf70d360925563f82d92d93d9522e91d3567a5f3` | `8a0ad173..cf70d36` |
| `gwz-cli` | `f4e0261c3db33e0956331fa9fa576f123ae0f1e1` | `9d634709a4a9be599a0300244eb92d53416b0c91` | `f4e0261c..9d63470` |
| `gwz-py` | `83e058e644cf9e719cc57accd325bd69223168f1` | `b771d98d7b33401fd555a13ad0e16b01c01610c6` | `83e058e6..b771d98` |

Confirm the actual heads before reviewing. If they differ, record the exact
heads and whether the difference affects the review rather than silently
reviewing a different range.

The ranges identify the M1/M2a implementation, but do not limit the review to
diff reading. Read the current implementation and every relevant caller,
model, serializer, test, and design contract needed to verify behavior.

The following unrelated root files are outside scope and must remain untouched:

```text
dev-docs/AnnounceGwzHn.md
dev-docs/GwzMergeCode-ReviewF5-2.md
dev-docs/HN-AnnounceGWZ.md
```

## Sources of truth

Read these before judging the implementation:

1. `gwz-core/dev-docs/GwzMergeDesign.md` — merge behavioral authority.
2. `gwz-core/dev-docs/GwzMergePlan.md` — sequencing, ownership, milestone, and
   release-gate authority.
3. `gwz-core/dev-docs/GWZDesign.md` — overall workspace model.
4. `gwz-core/dev-docs/GWZRequirements.md` — baseline requirements, especially
   the REQ-089 family.
5. `dev-docs/GwzMergeCode-ReviewF5.md`,
   `dev-docs/GwzMergeCodeRemPlan-1.md`, and
   `dev-docs/GwzMergeCode-ReviewF5-2.md` — prior M0 findings, dispositions, and
   regression risks.
6. `gwz-cli/docs/commands/merge.md`, `gwz-cli/docs/MachineOutput.md`, and
   `gwz-cli/docs/Releases.md` — current public claims and release boundary.

When documents disagree, identify the conflict explicitly. Do not choose the
implementation merely because it is current.

## Intended milestone boundary

M1/M2a is an internal checkpoint, not the public member-merge release gate.
The following are intentionally deferred and are not findings merely because
they are absent:

- M2b: final lock publication, scoped composition evidence, durable
  finalization, and public status/continue/abort release.
- M2c: explicit workspace-root (`@root`) merge participation and recovery.
- M3: preserve-abort, archived-status/retention completion, and garbage
  collection.
- M4: additional strategies and source forms.

However, report any M1/M2a behavior that makes those phases unsafe, violates
the frozen handoff, exposes a deferred feature prematurely, loses recovery
information needed by a later phase, or claims deferred behavior already
works.

## Required review method

1. Inspect repository status and exact commits without changing them.
2. Read the authoritative design and plan completely for the reviewed phases.
3. Read every changed file in each range, then follow relevant call paths
   outside the range.
4. Construct adversarial state and crash scenarios before relying on tests.
5. Run focused reproductions for suspected defects when practical. Temporary
   files must be outside the repositories and removed or left outside the
   workspace.
6. Run the verification suite below. If a command cannot run, record the
   precise blocker and continue with the remaining review.
7. Write only the designated review report. Do not fix findings.

Do not infer correctness solely from passing tests. Conversely, do not report
speculative concerns without a concrete violated invariant and credible
failure path.

## Required review areas

### A. Durable record and recovery discovery

Verify:

- the open record exists durably before the first Git mutation;
- atomic write ordering, flushing, rename, verification, and cleanup fail
  closed;
- unreadable, duplicate, mismatched, or future-field records cannot disappear
  as if no operation were open;
- unknown fields survive read-modify-write;
- open-state discovery precedes parsing possibly conflicted/invalid workspace
  metadata;
- archive and retention behavior cannot destroy an open or preservation-owning
  record;
- the accepted manifest and lock remain the exact baseline while the operation
  is open; and
- recovery remains possible after process restart from member subdirectories.

### B. Lifecycle state machine, persistence, events, and locking

Verify:

- every legal and illegal operation/participant transition;
- every outcome/state write occurs before its corresponding event;
- no event or response claims a state that was not durably recorded;
- the workspace mutator lock covers every mutating start, continue, and abort
  path;
- repeated calls and closed/open ID mismatches have stable typed behavior;
- no operation can become `completed` before M2b publication;
- successful member-only work enters only the M2b `finalizing` handoff; and
- failure while persisting a transition leaves a state that status and
  recovery can explain safely.

### C. Status, drift, and eligibility

Verify status is read-only and accurately distinguishes:

- branch, attached/detached HEAD, target-ref, advanced/rewound/diverged HEAD,
  missing repository, and missing object drift;
- exact native merge state and `MERGE_HEAD`;
- unresolved conflicts, expected auto-merge index entries, conflict-path
  resolution changes, unrelated staged/unstaged/untracked work, and other
  integration state;
- operation-level manifest/lock drift;
- `planned`, `failed`, and `unattempted` crash/retry ambiguity;
- successful-result drift versus an exact recorded result;
- no-action abort states such as `up_to_date` and genuinely unattempted rows;
  and
- separate continue and abort eligibility where abort may be safe while
  continue is not.

Confirm that the classifier consumed by continue and abort cannot report an
optimistic eligibility that is rejected only after an earlier repository has
already mutated.

### D. Continue and retry

Verify:

- the complete operation is preflighted before any resolution commit or retry;
- a blocker in the last participant prevents mutation of the first;
- frozen target branch, before commit, source commit, merge head, message, and
  original execution order are honored;
- resolution commits have exactly the recorded first and second parents and
  exact recorded message;
- `failed` retries occur only from a classified exact-before clean point;
- `unattempted` and crash-window `planned` rows cannot absorb unrelated work;
- new content conflicts are recorded and execution continues, while unexpected
  backend/host failure stops later actions and records them honestly;
- every result is persisted before an event or later action;
- already-successful participants are preserved exactly;
- post-action observation cannot silently adopt drift; and
- only a fully successful participant set reaches `finalizing`.

### E. Coordinated abort and interrupted rollback

Verify:

- the entire rollback plan passes before the first rollback mutation;
- post-merge commits, edits, staged work, branch moves, and native-state drift
  reject the whole abort without touching another participant;
- conflict abort, fast-forward rollback, true-merge rollback, continued-result
  rollback, and no-action states are all handled deliberately;
- rollback order is the reverse of the frozen execution order;
- checked ref updates cannot overwrite an unexpected current value;
- worktree/index restoration preserves unrelated and untracked work or rejects
  before mutation;
- a crash or record-write failure after Git successfully rolled back but before
  the participant row persisted is recognized and safely verified on retry;
- idempotent backend verification does not repeat an unsafe mutation;
- already persisted `rolled_back`/`aborted` rows are verified and skipped
  safely;
- the exact baseline manifest and lock are verified before archive; and
- archive failure or process interruption leaves a safely repeatable open
  operation.

### F. Git backend safety and TOCTOU boundaries

Review the checked Git primitives independently of their callers. Verify:

- exact object parsing and merge-base/unrelated-history behavior;
- native merge-state validation, including a single exact `MERGE_HEAD`;
- index comparison against the expected merge result, not merely aggregate
  dirty counts;
- exact-parent resolution commit creation and cleanup;
- ref transaction/locking behavior and checked current-object updates;
- ordering of checkout, index, ref, and repository-state cleanup;
- postconditions after success and failure;
- idempotent already-restored behavior;
- raw-Git concurrency between whole-operation preflight and checked action;
- behavior for attached/detached branches, missing refs/objects, hooks/signing
  expectations, identity, filesystem errors, and untracked/ignored files; and
- whether any error can occur after a destructive side effect while leaving an
  unrecoverably stale durable record.

### G. Central open-operation gate

Verify the full command matrix in both Rust and Python drivers:

- mutators are blocked while a merge is open;
- read-only commands remain available;
- stage/add is restricted to the open merge's conflicted participants;
- merge status, continue, and abort reach recovery even with invalid live
  manifest metadata;
- new-workspace operations are not incorrectly captured by an enclosing
  workspace's open merge;
- tag, branch, stash, repo, init, snapshot, pull, push, materialize, capture,
  commit, diff, status, list, and arbitrary-command forms match the design;
- no driver or direct dispatch path bypasses the intended gate; and
- conditional/allowed classifications are followed by any required narrower
  handler check.

### H. Rust/Python protocol, events, and output parity

Verify:

- both drivers construct the same merge operations and use the same core
  semantics;
- top-level and participant errors retain typed codes and structured member
  context;
- JSON and JSONL include the complete current merge response shape;
- human status contains lifecycle, counts, recorded/live commits, conflicts,
  drift, eligibility, and publication state without inventing unavailable
  behavior;
- the shared parity fixture pins error-bearing and non-empty nested shapes;
- event recording is completed exactly once on success/failure; and
- hidden M1/M2a command forms remain callable for integration tests without
  being advertised as released before M2b.

### I. Release boundary and documentation accuracy

Confirm:

- start/dry-run documentation describes the durable baseline-lock behavior;
- M1/M2a internal names are not used as user-facing capability descriptions;
- status/continue/coordinated abort are not publicly released before M2b;
- M2b finalization, M2c root participation, M3 preservation/GC, and M4
  strategies are neither silently implemented nor falsely promised;
- current errors do not direct users into unavailable or unsafe recovery; and
- `GwzMergePlan.md` completion claims match the code and tests.

### J. Tests and regression quality

Assess whether tests prove behavior rather than mirror implementation. Look
especially for:

- real-Git mixed `up_to_date`/clean-merge/conflict scenarios;
- a blocker in a later repository proving zero earlier mutation;
- failed/unattempted retry behavior;
- post-merge work rejecting abort before another rollback;
- Git-success/record-write-failure recovery;
- exact resolution parents/message and exact rollback ref checks;
- recovery with invalid manifest bytes;
- event ordering and record-before-mutation spies;
- shared Rust/Python machine-output fixtures;
- parallel-test temporary-directory collisions;
- platform-sensitive ref, path, flush, and rename assumptions; and
- regression of the earlier M0 review/remediation findings.

## Mandatory adversarial crash matrix

Reason through and report the outcome at each boundary, even if no defect is
found:

1. after durable record creation, before the first Git action;
2. after a Git start/continue action, before its participant outcome write;
3. after a participant write, before its event;
4. after transition to `executing`, before the first continue action;
5. after transition to `rolling_back`, before the first rollback;
6. after Git rollback succeeds, before the rollback row write;
7. after a rollback row write, before the next rollback;
8. after transition to `aborted`, before archive;
9. after archive publication, before the command returns; and
10. after all member success and transition to `finalizing`, before M2b exists
    to publish composition evidence.

For each, decide whether retry/status/abort is safe, idempotent, blocked with a
precise typed recovery state, or ambiguous. Flag any boundary that can lose or
overwrite user work, silently adopt drift, or make the operation undiscoverable.

## Severity and finding standard

Use these priorities:

- **P0:** active data loss, repository corruption, credential exposure, or an
  operation that can irreversibly publish a false workspace composition in
  ordinary use.
- **P1:** likely destructive behavior, unrecoverable/undiscoverable open
  operation, whole-operation preflight violation with material mutation, or a
  release-blocking correctness failure.
- **P2:** concrete correctness, recovery, protocol parity, or diagnosability
  defect with a realistic failure scenario but lower likelihood/impact.
- **P3:** bounded robustness, coverage, maintainability, or documentation gap
  that has a concrete consequence. Do not report cosmetic style preferences.

Every finding must contain:

1. a concise severity-labelled title;
2. exact file and line references;
3. the violated design requirement or safety invariant;
4. a concrete reproduction or state sequence;
5. impact;
6. recommended correction; and
7. a regression test that would fail before the correction.

Keep one root cause per finding. If evidence is insufficient, put the concern
under residual risks rather than presenting it as a defect. Explicitly state
when there are no P0/P1/P2 findings.

## Verification commands

Run from `/Users/owebeeone/limbo/gwz-dev` unless noted:

```sh
cargo fmt --all -- --check
cargo test --workspace --all-targets --no-fail-fast
cargo clippy --workspace --all-targets --all-features -- -D warnings

cargo fmt --manifest-path gwz-py/Cargo.toml -- --check
cargo clippy --manifest-path gwz-py/Cargo.toml \
  --all-targets --all-features -- -D warnings
gwz-py/.venv/bin/python -m maturin develop \
  --manifest-path gwz-py/Cargo.toml

cd gwz-py
.venv/bin/python scripts/regen_protocol.py --check
.venv/bin/python -m pytest -q
cd ..

cd gwz-cli
python3 scripts/generate_cli_reference.py --check
uvx --from 'mkdocs-material>=9.5,<10' mkdocs build --strict \
  --site-dir /tmp/gwz-m2a-review-XX-docs
cd ..

bazel build //gwz-core:gwz_core //gwz-cli:gwz

git diff --check
git -C gwz-core diff --check
git -C gwz-cli diff --check
git -C gwz-py diff --check
```

Replace `XX` in the temporary documentation path with your designation. Report
the exact pass/fail/skip result and useful test counts. A verification command
that changes tracked files is itself evidence of a generated-artifact problem;
do not restore or commit it—record it and stop mutating the workspace.

## Required report structure

Write `dev-docs/GwzDevCodeM2a-ReviewXX.md` with:

1. **Title, date, reviewer designation, exact reviewed commits/ranges.**
2. **Executive verdict:** one of `ready for M2b`, `ready for M2b with
   non-blocking follow-up`, or `remediation required before M2b`.
3. **Findings:** ordered most severe first, using the required finding format.
4. **Design and invariant verification:** concise disposition of review areas
   A–J, including important no-finding evidence.
5. **Crash-matrix results:** all ten mandatory boundaries.
6. **Verification commands and results:** exact outcomes and counts.
7. **Residual risks and missing coverage:** including platform limitations.
8. **Recommended next action:** whether M2b parallel work can safely begin and
   what, if anything, must be remediated first.

The report must stand on its own. Do not assume the reader has access to your
tool output or private reasoning.
