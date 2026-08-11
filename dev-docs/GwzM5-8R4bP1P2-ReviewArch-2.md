# R4b-P P1/P2 settled-tree architecture re-review 2

Date: 2026-08-11

Reviewed exactly:

- workspace commit `5e636cf6b288a79a1976322e55cd1d6a822cd60b`;
- `gwz-core` commit `6ffbb7f36cd40837c63b8331fc937ada82eb099e`;
- the original architecture review
  `GwzM5-8R4bP1P2-ReviewArch.md`; and
- the accepted remediation plan `GwzM5-8R4bP1P2-RemPlan.md`.

I did not read or coordinate with the other settled-tree re-review. The frozen
I2 action-journal contract, R4b reverse-lifecycle interfaces, and the accepted
remediation plan are controlling. The review treats the local lack of a
Windows SDK as an evidence fact, not as permission to weaken Windows behavior;
the exact-tree release-platform builds remain the platform gate.

## Verdict

**NO-GO for accepting the R4b-P checkpoint, starting R4b-G, or starting A1.**

There is no P0 finding. One P1 implementation defect and three P2 findings
remain. C3-C6 are substantially corrected in production code, but C1, C2, and
C7 do not meet the accepted plan. The P1/P2 gate requires all C1-C7 regressions
and two reviews with no open P0/P1/P2; this tree does not meet that gate.

## Findings

### [P1-1] A visible checked-artifact goal is accepted as `After` before parent durability is proved

**Where:** `gwz-core/src/checked_artifact.rs:23-28,145-157,170-217,220-251,323-334`;
`gwz-core/src/checked_artifact/tests.rs:88-101,143-165`;
`gwz-core/src/workspace_ops/merge/preserve/artifacts.rs:688-704,743-767`;
`gwz-core/src/git/gitbackend/preservation_root/files.rs:43-49`.

**Contract:** C1 requires parent-directory durability as a required result
(`GwzM5-8R4bP1P2-RemPlan.md:71-80`) and requires directory-sync plus
before/after crash regressions (`:154-164`).

**Evidence.** `replace_exact` performs rename, exposes `AfterMutation`, and only
then syncs the retained parent. `remove_exact` has the same ordering after
unlink. `CheckedArtifactFact` can represent only `Missing`, `Bytes`, or
`Invalid`; it has no causal fact for “goal is visible while the pending action
has not obtained its durability barrier.” Consequently every consumer
classifies exact goal bytes/absence as ordinary `After`.

The focused test encodes the defect: it injects `AfterMutation`, receives an
error before `sync_parent`, and then asserts that observation is already
`Missing`/goal bytes. Neither `BeforeDurability` nor `AfterDurability` is used
by a test anywhere in the tree.

**Failure scenario.** A pending bundle, marker-removal, lock, manifest, or root-
preservation leaf action completes rename/unlink, then parent sync fails (or the
process stops before it). Resume observes the goal, adopts it as `After`, and
writes the durable successor without reissuing the parent barrier. A later
machine failure can retain the successor record but lose the earlier directory
entry update. The journal then says the action completed while the physical
artifact is back at its source form.

**Impact.** This is the known-non-durable advancement C1 was intended to close.
It breaks exact restart and can make a supposedly completed preservation or
rollback action disappear after a crash.

**Required correction.** Preserve the pending action's causal distinction
between “goal visible” and “goal durably completed.” Before any durable
successor is authorized, obtain a successful barrier on the retained canonical
parent, including on recovery after an earlier attempt returned after the
linearization point. This may be an internal checked-artifact result/executor
step; it need not add a wire phase, but read-only byte/absence observation must
not manufacture durability authority.

**Regression evidence required.** For replacement and removal, and through
bundle, publication rollback, selected-root rollback, and root-preservation
service consumers, inject before-parent-sync, parent-sync error, and
after-parent-sync boundaries. Restart from the still-pending action and prove
that the durability barrier is obtained before the successor write, is never
silently skipped, and that an ambiguous/foreign source is never adopted.

### [P2-1] Windows checked removal can strand an unrecognized tombstone

**Where:** `gwz-core/src/checked_artifact.rs:247-250,302-320`;
`gwz-core/src/checked_artifact/platform.rs:50-95,117-149`;
`gwz-core/src/git/gitbackend/preservation_root/parent.rs:196-240`.

**Evidence.** On Windows, removal first write-through renames the managed leaf
to a process/sequence-named `.removed` sibling and then deletes that sibling.
The comment calls a failure after rename recoverable, but neither
`CheckedArtifact` nor its consumers recognize or own that tombstone on a later
process. For a managed marker, the root-preservation parent classifier accepts
only an empty directory or its one expected marker; the tombstone makes that
directory `Invalid`.

**Failure scenario.** Windows stops after the write-through leaf-to-tombstone
rename and before `remove_file`. Resume observes the managed leaf as absent and
can advance the leaf action, but the private tombstone remains. A later parent
normalization/restoration step sees an unexpected directory entry and cannot
classify or clean it, permanently halting exact recovery.

**Impact.** A supported release platform can be stranded by a crash inside the
new common physical primitive. A successful Windows compile/release build does
not exercise this behavioral boundary.

**Required correction.** Use a handle-relative durable deletion whose restart
state has no orphan, or use a deterministic, action-bound tombstone that every
relevant observer can classify and safely finish. Add native Windows crash-
boundary tests. The local host's missing Windows SDK remains a recorded local
limitation; passing exact-tree Windows release evidence is still mandatory.

### [P2-2] Selected-root entry preflight still defers exact result-artifact proof until after rollback begins

**Where:** `gwz-core/src/workspace_ops/merge/abort/preflight.rs:34-65,111-125,129-197`;
`gwz-core/src/workspace_ops/merge/root/abort.rs:112-165,317-395`;
`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rollback.rs:83-123,243-301`;
`gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/entry.rs:69-146`.

**Contract:** C2's prospective proof must establish that selected-root result
manifest/lock bytes and the operation baseline are both derivable and exact
before `BeginRollback` (`GwzM5-8R4bP1P2-RemPlan.md:132-150`). R4 requires the
negative matrix to go through the sealed entry and prove no record/repository/
artifact mutation (`:190-199`).

**Evidence.** `require_virtual_selected_root_after_evidence` verifies the
publication evidence prefix, accepted root branch/result commit relation,
native state, and that live dirt is restricted to candidate paths. It never
reads the manifest or lock from the selected-root `resulting_commit` and never
compares those bytes with `accepted_workspace.metadata_base`.

That proof exists only in the later private
`selected_root_result_artifacts` helper used by selected-root metadata rollback.
The rollback cursor reaches it after publication evidence and the selected-root
participant have been retired. Thus an unreadable/missing result artifact or a
self-consistent record whose accepted metadata differs from the actual result
commit can pass global entry, mutate earlier reverse owners, and fail only at
the metadata cursor.

The new test calls `preflight_v1_rollback` directly and covers only staged,
unstaged, and untracked dirt with selected root alone. It does not exercise the
service/`BeginRollback` boundary, another member, rename/type-change,
unresolved/native-state rows, exact result-artifact mismatch/unavailability, or
the full byte-identical no-mutation assertions required by R4.

**Impact.** Global preflight can still admit a rollback that becomes ineligible
only after earlier reverse mutation, recreating the cross-owner inconsistency
the prospective proof exists to prevent.

**Required correction.** Move/export one read-only exact selected-root result-
artifact verifier into the authority-owned prospective proof. Before issuing
entry authority it must read both canonical artifacts at `resulting_commit`,
validate their exact form/UTF-8 requirements, and compare them with accepted
metadata. Complete the R4 service-level negative matrix and assert no
`BeginRollback`, byte-identical record, and unchanged root/other-member state.

### [P2-3] C7's service fault/restart evidence covers only favored physical phases

**Where:**
`gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_fault_matrix.rs:16-40,74-149,181-203`;
`gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_successor_matrix.rs:16-94,158-206`;
`gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/service_fault_matrix.rs:20-118,127-139`;
`gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/invariants.rs:13-205`;
`gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/phases.rs:68-124`.

**Contract:** R5 requires table-driven service cases for every root
preservation phase and every evidence, participant, and selected-root rollback
step, crossed with both physical sides, durable successor, fresh
`Before`/`After`/ambiguous observations, restart to exhaustion, legal incoming
requests, both root owners, and handoff variants
(`GwzM5-8R4bP1P2-RemPlan.md:201-220`).

**Evidence.** The root physical matrix asserts that its complete emitted action
set is only backup, stash creation, bundle write, and ref reset. It therefore
does not force physical `Before`/`After` cases for the normalize/restore/
prepare marker, lock, parent, and index phases. The separate 22-phase matrix
interrupts only after each durable successor; it does not supply the missing
physical or fresh-fact crosses. Both root matrices allow a selected-root resume
to stop again in `RecoveryRequired`, so they do not prove restart to
exhaustion.

The rollback service matrix covers only participant `ResetIntegrated`, not
`AbortConflict`, and uses only incoming `Abort`. No checked-artifact service
case reaches `BeforeDurability` or `AfterDurability`. Focused phase/unit tests
are useful, but do not prove the service's observe/persist/execute/restart
composition at the required boundaries.

R2/R3 evidence is also narrower than accepted: stash regression changes a
preimage but does not advance HEAD after `BeginStash`, and the canonical bundle
fixture covers `mem_z`/`mem_a` but omits the required `@root` owner. Production
checks for C3 and sorting for C6 look correct; the missing cases are nevertheless
part of the explicit executable exit gate.

**Impact.** The test claim in the plan that every physical/durable boundary is
covered is false. P1-1 is an example of the exact class that the incomplete
matrix passes while encoding the wrong restart authority.

**Required correction.** Generate cases from the complete phase enums and make
each otherwise-degenerate phase physically `Before`, `After`, and ambiguous.
Require terminal exhaustion, exact action counts/bytes, and no repeated
mutation. Add `AbortConflict`, legal `Preserve`/`Abort` crosses, handoff variants,
the stopped-after-intent stash HEAD row, and the `mem_z`/`mem_a`/`@root` bundle
fixture. Integrate the durability cases from P1-1 rather than treating a visible
goal as a completed physical action.

## C1-C7 closure audit

| Item | Result | Review conclusion |
| --- | --- | --- |
| C1 | **Open** | The common no-follow checked primitive is adopted, but it loses mutation/durability causality (P1-1) and has an orphaned Windows removal state (P2-1). |
| C2 | **Open** | Unrelated dirt is checked, but exact result-commit manifest/lock proof is still deferred (P2-2). |
| C3 | Code closed; evidence incomplete | Checked stash execution binds persisted branch, HEAD, and preimage; the required stopped-after-intent HEAD service row remains missing under P2-3. |
| C4 | Closed | Checked backup-ref execution locks/rechecks attached HEAD, branch, and destination, with a service stale-HEAD regression. |
| C5 | Closed | The cursor rejects an earlier regressed owner before issuing mutation when later durable evidence exists. |
| C6 | Code closed; evidence incomplete | Both bundle representations sort by owner ID; the required root-inclusive canonical-byte fixture remains missing under P2-3. |
| C7 | **Open** | The matrices do not cover the complete physical/durable/fresh-fact/restart cross-product (P2-3). |

The remediation diff does not alter the frozen v1 wire/model vocabulary or
enable the production v1 decoder/dispatcher. The checked Git seams, later-owner
guard, and stable bundle sorting are cohesive with the accepted interfaces.

## P3 backlog

No new P3 finding is needed to reach the verdict. The accepted pre-A1 backlog
remains open and unchanged:

- retain the original typed rollback physical diagnostic through
  `v1_lifecycle/authority/resolver/execution.rs`;
- replace rollback-path validated-record `unwrap`/`expect` calls with typed
  record/recovery errors; and
- obtain blob object format from a typed repository fact rather than commit
  string width.

These are not substitutes for the P1/P2 corrections above.

## Verification

On the exact reviewed tree:

- installed `gwz status --json`: workspace and all seven members clean and lock
  matched; exact workspace/core commits matched the review request;
- `cargo test --all-targets --no-fail-fast`: **1,113 passed, 1 ignored, 0
  failed** (1,069 library + 44 integration tests);
- `cargo fmt --check`: passed;
- `cargo clippy --workspace --all-targets -- -D warnings`: passed;
- `python3 protocol/regen.py --check`: passed;
- merge document consistency: **10 sources, 133 assertions** passed;
- merge document checker unit tests: **3 passed**;
- compatibility-predicate validator: **7 migration rules and 7 runtime
  bindings** passed;
- compatibility-predicate unit tests: **14 passed**; and
- `git diff --check`: passed.

The retained-reader suite was not used as positive evidence: an ancillary local
run was stopped while a fixture `git fsck` did not return. It is not part of
the R6 command list and has no bearing on the concrete findings above.

Per the accepted plan, the local macOS host cannot supply Windows behavioral
evidence because the Windows C/zlib SDK is absent. The exact-tree Windows,
macOS, Linux x86, and Linux arm64 release builds remain required, but build
success alone cannot replace the Windows crash-boundary regression in P2-1.

## Exit decision

Do not resume R4b-G. Correct P1-1 and P2-1 through P2-3, complete the required
regressions, rerun full/static/doc/platform gates, and return one new exact
settled workspace/core commit pair to two independent reviewers. The gate is
GO only when both reviews report no open P0/P1/P2.
