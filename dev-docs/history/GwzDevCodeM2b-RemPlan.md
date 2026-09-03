# GWZ Merge M2b Code Review Remediation Plan

Date: 2026-07-24

Execution status: **complete; independent re-review passed**

Inputs:

- `gwz-core/dev-docs/GwzMergeDesign.md`
- `gwz-core/dev-docs/GwzMergePlan.md`
- independent review of the committed M2b snapshot:
  - `gwz-core` `5cd67c2d9329eb749497636cd7f2d5baa3a1886c`
  - `gwz-cli` `0cd9742cf36517c09f8c11d79262e55eb4c172c0`
  - `gwz-py` `6d60ee2f3e0ec7d0108610bc0da3a622b2d6b2ca`

## 1. Gate decision

M2b remains implemented but its release gate is reopened. The independent
review found no P0/P1 issue, but five P2 recovery/driver defects and one P3
event-contract gap. All six findings are accepted. M2c and M3 remain blocked
until this plan is complete and the corrected snapshot passes independent
re-review.

## 2. Finding dispositions

| Finding | Disposition | Required correction |
| --- | --- | --- |
| P2-1 born-root evidence abort is blocked by intentional candidate-path dirt | **Fix** | Add an exact scoped-evidence rollback primitive that moves the checked root ref without checkout or a generic clean-worktree requirement, preserves unrelated root state, and recognizes an exact evidence commit whose record write was interrupted. |
| P2-2 post-candidate operation drift is ignored | **Fix** | Make every finalization mutation consume complete live operation drift, including manifest drift, and fail closed until exact repair. |
| P2-3 repaired root-candidate drift remains wedged | **Fix** | Reclassify root candidate state on each retry and clear only the durable root-candidate drift after exact read-only verification. |
| P2-4 status omits marker and boundary drift | **Fix** | Share one read-only ordered publication-prefix classifier across status, continue/finalization, and abort. |
| P2-5 Python recovery guidance names the Rust executable | **Fix** | Render `gwz-py merge ...` on the Python CLI surface and update its tests. |
| P3-1 final evidence publication is absent from events | **Fix** | Emit stable `ArtifactWritten` events after verified publication for the root evidence commit, marker, accepted lock, and local boundary. |

## 3. Required invariants

- Status remains strictly read-only.
- Continue and abort perform complete whole-operation preflight before their
  first Git or accepted-artifact mutation.
- A recovered evidence commit must match the exact parent, tree, candidate
  paths and bytes, message, attached branch, and valid publication prefix.
- Evidence rollback changes only the checked root ref and candidate-owned
  lock/marker/boundary paths. Unrelated staged, dirty, and untracked root work
  is byte-for-byte and index-entry-for-index-entry preserved.
- Only `RootCandidateStateChanged` may be cleared after exact repair; manifest,
  lock, participant, and other durable drift remain blockers.
- Candidate publication accepts only the ordered baseline, marker, lock, and
  boundary prefixes. Deletion or alteration after a later step is drift.
- Terminal completion and archive occur only after participant, manifest,
  root evidence, and complete candidate publication verification.
- Existing protocol values and response fields are not renumbered or
  repurposed.

## 4. Work packages

### R1 — Shared publication observation

Extract a pure read-only candidate-prefix classifier and exact evidence
candidate helpers. Use them in status, finalization, and abort. Status must
independently report invalid marker, lock, boundary, ordering, branch, HEAD, or
manifest state.

### R2 — Exact evidence recovery and abort

Add a checked Git backend operation that verifies the scoped evidence commit
under the attached branch/ref lock and restores the recorded parent (or unborn
branch) without checkout and without treating expected candidate-path
differences as unrelated dirt. Recover exact evidence identity from `HEAD`
when the ref moved before `composition_commit` was persisted.

### R3 — Finalization retry safety

Use the complete status snapshot before each mutating finalization phase.
Revalidate the manifest and ordered publication prefix, adopt an exact
unrecorded evidence commit, and make exact repair of root branch/prefix drift
retryable without clearing unrelated drift.

### R4 — Driver and event closure

Correct Python recovery command names. Emit and document final evidence
artifact events with deterministic ordering before the terminal transition.

## 5. Required regression tests

- born-root abort immediately before and immediately after durable
  `composition_commit` persistence;
- both born-root abort rows preserve unrelated staged, dirty, and untracked
  root work;
- post-candidate manifest drift blocks status and continue without mutation,
  and exact restoration permits one evidence commit;
- same-commit root branch drift blocks, exact branch restoration resumes, and
  no duplicate evidence commit is created;
- repaired candidate-prefix drift resumes;
- status directly detects marker and boundary alteration before continue or
  abort is invoked;
- finalization JSONL/event collection includes evidence commit, marker, lock,
  and boundary artifact events in deterministic order;
- Python human recovery guidance uses `gwz-py`;
- all existing six finalization interruption points still resume
  idempotently.

## 6. Exit gate

Completion requires:

1. all focused recovery and publication tests pass;
2. `cargo fmt --all -- --check` passes;
3. `cargo clippy --workspace --all-targets -- -D warnings` passes;
4. `cargo test --workspace` passes;
5. `gwz-py/.venv/bin/python run_tests.py` passes with current native code and
   cross-driver parity;
6. generated protocol/document checks and `git diff --check` pass;
7. `gwz status` reports only the intended remediation files; and
8. an independent re-review reports no P0/P1/P2 defect in the corrected M2b
   contracts.

## 7. Implementation evidence

The initial corrected implementation completed R1-R4. Its independent
re-review verified all six original findings but found two additional P2 abort
recovery gaps and one P3 orchestration-test gap:

- abort rejected valid early-finalization records that had no candidate yet;
- evidence artifact restoration did not proceed in reverse publication order,
  leaving one interrupted state outside the accepted prefixes; and
- unborn-root evidence abort was covered only at the Git primitive layer.

The second correction accepts evidence-free pre-candidate abort, restores
boundary then lock then marker, fault-injects after every restoration mutation,
and runs both evidence-record persistence windows end-to-end for born and
unborn roots while preserving unrelated root state.

The release gate now passes:

- 691 Rust test executions: 690 passed and one ignored;
- 315 Python/native tests;
- workspace formatting and strict warnings-as-errors Clippy;
- generated protocol freshness and Rust/Python cross-driver parity; and
- workspace diff hygiene.

The final independent re-review passed with no P0/P1/P2/P3 findings. It
independently reran all 32 focused `g23` lifecycle tests and both scoped
evidence rollback backend tests. All eight exit criteria are satisfied.
