# R4b-P P1/P2 settled-tree fault/recovery re-review 3

Date: 2026-08-12

Reviewed exactly:

- workspace commit `1c6bbba13586cb8be50576301072b38f0e0a3463`;
- `gwz-core` commit `7d1a3e6a33cf2e4b5d882db1f7196f16ea7cc87d`;
- `gwz-cli` commit `3cca145c0b32410f250f640730ed7ca18f1da59f`;
- `GwzM5-8R4bP1P2-RemPlan-2.md` and both preceding settled-tree
  re-reviews; and
- the frozen I2 action-journal and R4b reverse-lifecycle/root-handoff
  contracts.

I reviewed this tree independently from the architecture re-review. The tree
was clean and the workspace/core/CLI commits above matched before review.

## Verdict

**NO-GO.** There is no P0 finding. Three P1 checked-artifact defects and two P2
gate defects remain. The checked stash boundary and the result-commit artifact
proof are materially corrected, both rollback kinds are now present, and the
new service tests reach terminal exhaustion. D1, D3, and D4 are nevertheless
not closed, so R4b-G and A1 must remain blocked.

## Findings

### [P1-1] A staged replacement can be rebound to a different parent after restart

**Where:** `gwz-core/src/checked_artifact/transition.rs:85-87,111-141,306-376`;
`gwz-core/src/checked_artifact/observation.rs:74-118`.

**Violated contract:** RemPlan-2 section 3.1 requires the quarantine key to
bind the retained parent identity and requires a moved/replaced parent to be
restored and rejected. Exit criterion 2 forbids overwriting or removing a
foreign source inode or moved parent.

**Evidence.** `action_key` binds only the lossy relative path, operation,
expected bytes, and goal bytes. It does not bind the retained parent identity.
`replace_exact` stages the deterministic goal before the final source/parent
proof. On a new invocation, `acquire` retains whatever directory is now at the
workspace-relative parent, derives the same key, and treats the old staged goal
as recoverable. If the new parent contains the same expected bytes, that new
inode is then detached as though it were the original source and ultimately
discarded.

**Reproduction scenario:** begin an existing-source replacement, inject the
current `BeforeFinalCheck` failure after `stage_goal`, rename the managed parent
away, create a new parent at the same relative path, and create a same-byte new
source there. Reacquire and retry the persisted action. The old `.goal` residue
matches the unbound key, the new source passes the byte proof, and GWZ replaces
and cleans up a foreign inode in the replacement parent. The analogous
missing-source row publishes into the replacement parent even without a leaf.

**Impact:** a crash followed by parent replacement can make a persisted bundle,
publication, selected-root, or root-preservation action mutate user state that
was never part of its original authority. This is the exact-source safety
boundary D1 was meant to close.

**Required correction:** bind the original retained-parent identity in the
action key and every staged/source residue before staging begins. A restart
under a different parent must classify `Ambiguous` and must not detach,
publish, or clean anything. Add service-crossed tests for missing and existing
sources, including a same-byte foreign inode, with a stop after goal staging
and parent replacement before reacquisition.

### [P1-2] Checked removal executes an `Ambiguous` residue and can delete the quarantined source

**Where:** `gwz-core/src/checked_artifact/transition.rs:228-303,306-350`;
`gwz-core/src/checked_artifact/residue.rs:172-194`.

**Violated contract:** RemPlan-2 section 3.1 says a parent move detected after
detachment is restored and rejected, and that ambiguous forms retain both
names rather than becoming destructive.

**Evidence.** `remove_exact` returns early for `After`, but unlike
`replace_exact` it does not branch on or reject `Ambiguous`. The classifier
correctly returns `Ambiguous` when a quarantined source records a parent
identity different from the newly acquired parent. `remove_exact` then
continues, sees that a source residue exists, skips detachment, synchronizes
the new parent, deletes the quarantined source by name, and accepts the new
parent's missing leaf as the durable goal.

**Reproduction scenario:** fault after `AfterDetach` during removal, leaving
the original source in quarantine; before retry, move the original parent and
create an empty replacement parent at the managed path. On retry,
`classify_remove` is `Ambiguous`, but execution continues. The quarantined
original is deleted and the action succeeds against the replacement parent's
absence. If a same-byte foreign leaf exists in the replacement parent, the
original is still deleted before final verification reports an error.

**Impact:** restart can destroy the only retained original source and advance
an action against the wrong directory. Immediate same-process parent-move
tests pass, but the cross-process residue path remains destructive.

**Required correction:** make removal's transition dispatch symmetric with
replacement: only `Before` and `Recoverable` may execute; `Ambiguous` must
reject without cleanup. Revalidate the residue identity and parent binding at
cleanup. Add after-detach restart tests with a moved parent, both an empty new
parent and a same-byte foreign leaf, asserting exact retained bytes/inodes and
no successor write.

### [P1-3] Goal staging has no durable, restart-closed state

**Where:** `gwz-core/src/checked_artifact/residue.rs:113-142`;
`gwz-core/src/checked_artifact/transition.rs:111-120,188-224`;
`gwz-core/src/checked_artifact/fault.rs:5-14`.

**Violated contract:** RemPlan-2 section 3.1 requires a flushed temporary and a
closed residue classifier, and exit criterion 1 forbids advancement without
the necessary durability barriers.

**Evidence.** `stage_goal` creates the final deterministic `.goal` name and
writes directly into it. A crash or write failure can leave a partial file;
`inspect_residue` calls that foreign and the action has no automatic recovery.
More seriously, if the complete bytes are visible but `file.sync_all()` failed
or the process stopped before it completed, the next invocation returns early
at lines 115-116 solely because the bytes match. It never reacquires a file
durability barrier. The later rename and parent-directory sync cannot make the
previously unflushed file contents durable. The fault vocabulary has no
before/after goal-write or goal-file-sync boundary, so current durability tests
cannot expose either form.

**Reproduction scenario:** make the goal write complete, fail/interrupt its
file sync, and retry while the exact bytes remain visible. The retry publishes
the unsynchronized staged file, obtains only the managed-parent barrier, and
can persist the lifecycle successor. A subsequent machine failure may retain
that successor while losing or corrupting the goal contents. If interruption
occurs during the write instead, the deterministic partial `.goal` strands the
action as permanently ambiguous.

**Impact:** ordinary I/O failure or power loss can either authorize a
non-durable artifact or strand exact recovery. This affects every consumer of
the common checked replacement primitive.

**Required correction:** give staging its own restart-closed transition. Never
treat exact visible goal bytes as staged authority until the exact staged file
has been reopened/revalidated and successfully flushed. A partial
GWZ-created staging residue must have a safe deterministic recovery path while
foreign content still fails closed. Add faults before/after create, write,
file sync, staging publication, and its parent barrier, then cross them through
bundle and root-artifact service consumers to terminal exhaustion.

### [P2-1] Selected-root complete-checkout proof excludes non-candidate path trees

**Where:** `gwz-core/src/workspace_ops/merge/abort/preflight.rs:179-192`;
`gwz-core/src/workspace_ops/merge/abort/participants.rs:226-262`;
`gwz-core/src/git/gitbackend/preservation_image.rs:145-178`.

**Violated contract:** RemPlan-2 section 3.3 says selected root passes only the
closed publication candidate path set to the complete checkout comparator.
Exit criterion 5 requires the complete checkout proof before `BeginRollback`.

**Evidence.** Both selected-root callers additionally exclude all of `.gwz`,
`gwz.conf/.tmp`, and every manifest member path. The comparator removes these
prefixes from both the live and expected maps, including tracked index/tree
entries; it does not limit that removal to ignored operational files. No other
entry observer proves tracked content beneath those prefixes.

**Reproduction scenario:** let a selected-root result commit contain a tracked
`gwz.conf/.tmp/sentinel` (or another tracked path below one of the added
prefixes), then modify its index/worktree before rollback entry while keeping
the publication candidate paths exact. The complete comparator erases the
expected and live sentinel rows, so this unrelated selected-root drift can pass
the prospective proof and permit `BeginRollback`.

**Impact:** the new complete-checkout authority can still omit unrelated
tracked/index/worktree state and begin cross-owner rollback on an ineligible
root.

**Required correction:** pass only the exact candidate-owned path set. Handle
runtime/member directories through the already required, exactly verified Git
ignore boundary or explicitly reject tracked entries under reserved paths;
do not erase arbitrary tracked subtrees. Add service-entry rows with tracked,
staged, and worktree drift under each reserved prefix and assert no record,
repository, stash/bundle, or artifact mutation.

### [P2-2] C7 is still a fixture-derived subset rather than the generated legal matrix

**Where:**

- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_fault_matrix.rs:16-32,220-229`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_successor_matrix.rs:16-23,106-170,262-284`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_ambiguity_matrix.rs:14-25,147-190`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/faults.rs:16-80`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/service_fault_matrix.rs:19-47,186-207`; and
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/service_ambiguity_matrix.rs:14-88,126-153`.

**Violated contract:** RemPlan-2 sections 3.4/R5 and exit criterion 6 require a
generated legal-form matrix covering physical/durable/form/request rows with
exact counts/bytes and terminal exhaustion, rather than actions discovered
from favored fixtures.

**Evidence.** The root physical cases run all five handoffs only to discover a
union of emitted targets and then deduplicate each target to one handoff. The
separate five-handoff test records phase names, not the physical/no-op count,
artifact bytes, or both sides of every legal phase/form pair. Physical root
faults start only with `Preserve`, and each boundary is resumed with only one
of the two legal requests. The non-root matrix is still four ordinal
after-success crashes under `Preserve` only. Most root “ambiguity” rows install
one unrelated untracked file rather than the target's before/after/residue
third form.

Rollback now commendably includes `AbortConflict`, `ResetIntegrated`, both
requests, evidence, selected root, and terminal completion. It still discovers
actions by running one fixture per lane. Its service ambiguity matrix replaces
the production observer with a synthetic `BoundAmbiguityEvidence`, so it does
not prove that actual ambiguous Git/artifact facts compose through the service.
The matrices count only executions of the selected target and generally do not
pin exact successor record bytes or intermediate artifact bytes.

**Failure scenario:** change a root phase's physical/no-op decision for one of
the four handoffs not selected for that target, regress a non-root
before-physical/ambiguous resume under `Abort`, or make a real evidence/root
observer misclassify its third form. The current “complete” service matrices
remain green because the omitted combination is never instantiated or the
observer is bypassed.

**Impact:** D4 and the explicit C7 acceptance claim remain unproved. The three
P1 defects above are examples of physical residue/durability rows that this
matrix does not generate.

**Required correction:** define an explicit legal-form table and generate the
cross-product directly: handoff absent plus all five present forms, both root
owners, all 22 root phases, every actual physical/no-op row, both legal
requests, `Before`/`After`/real `Ambiguous`, every checked-artifact physical and
durability boundary, both rollback kinds, all evidence/root steps, and
non-root rows. Assert exact total and per-target calls, exact successor and
artifact/bundle bytes, retained bytes on ambiguity, and terminal exhaustion.

## D1-D4 closure audit

| Item | Assessment |
| --- | --- |
| D1 checked-artifact durability/source identity/Windows residue | **Open.** Managed-parent tombstones are gone and visible goals are re-barriered, but P1-1 through P1-3 leave parent identity, removal recovery, and staged-file durability open. |
| D2 checked native stash boundary | **Closed in code and focused evidence.** The exact hook is followed by final HEAD, native-state, full stash-set, and complete-preimage checks immediately before the single native stash call; boundary HEAD/staged/unstaged/untracked and matching/foreign stash rows exist. The documented arbitrary-writer limitation is honest. |
| D3 complete rollback-entry authority | **Partially open (P2-1).** Semantic index flags and exact selected-root result blobs are proved before entry, including multi-owner no-mutation tests, but the selected-root comparator erases more than the candidate-owned paths. |
| D4 generated C7 matrix | **Open (P2-2).** Both rollback kinds, both requests, five handoffs, all 22 phase names, durability consumers, and terminal exhaustion now appear, but not as the required generated legal physical/form/request/real-ambiguity cross-product with exact bytes/counts. |

## Verification performed

On the exact reviewed tree:

- installed `gwz status --json`: workspace and all members clean; exact
  workspace/core/CLI commits matched the review request;
- all 13 focused `checked_artifact::tests` passed;
- checked rollback consumer durability on both sides passed;
- the complete emitted rollback physical/successor matrix passed;
- `cargo fmt --all -- --check` passed; and
- workspace and `gwz-core` `git diff --check` passed.

These green checks are consistent with the findings: checked-artifact tests do
not restart after goal staging with a changed parent, do not execute removal
after an ambiguous cross-process parent change, and do not fault goal-file
write/sync; the service matrices omit or synthesize the stated C7 rows.

The exact-tree Windows/macOS/Linux x86/Linux arm64 release builds remain the
accepted platform evidence. They do not close the platform-independent
transition defects above.

## Exit decision

Do not resume R4b-G. Correct P1-1 through P2-2 in one reviewed remediation,
run the full local/static/document/platform gates on one settled tree, and
return that exact workspace/core pair to two independent reviewers. This gate
is GO only when both report no open P0/P1/P2 defect.
