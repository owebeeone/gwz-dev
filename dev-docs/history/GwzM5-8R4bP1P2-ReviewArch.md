# R4b-P P1/P2 Architecture Review

Reviewed fixed workspace commit `06689bb2faf42050223f1d4dc3344d685b30c863`
and `gwz-core` commit `569135a794df7c5a1fd4ed568df3575a11d2287d`.
The review used the frozen I2 action-journal contract, the R4b reverse-lifecycle
interface, amendment 2, and the R4b transition design as the controlling
contracts. I did not read another current review memo.

## Verdict

**NO-GO for accepting P1/P2 or starting A1.** There are two P1 and four P2
contract defects below. There are no P0 findings. The P1/P2 findings are
acceptance blockers; P3-1 is a pre-A1 diagnostics backlog item.

Focused evidence run during this review:

- `cargo test -q workspace_ops::merge::v1_lifecycle::reverse::preservation::tests`
  — 18 passed;
- `cargo test -q workspace_ops::merge::v1_lifecycle::reverse::rollback::tests`
  — 14 passed.

Those passing suites do not exercise the counterexamples below.

## Acceptance blockers

### [P1-1] Reverse artifacts use a path-based, best-effort-durable writer instead of an anchored physical seam

**Evidence.** The P2 path manifest calls for “no-follow root facts”
(`GwzM5-8R4bReverseLifecycleInterface.md:541-546`), and every mutating rollback
row must cover noncanonical parents/leaves and symlinks (`:649-661`). Amendment
2 defines the required containment boundary: retain a no-follow parent handle
through the final source observation and perform handle-relative rename/unlink
plus parent sync (`GwzM5-8R4bInterfaceAmendment-2.md:867-900`). The
implementation instead:

- observes and later reopens rollback artifacts by pathname
  (`gwz-core/src/workspace_ops/merge/root/artifact_facts.rs:14-65`);
- calls that helper for evidence and selected-root rollback mutation
  (`gwz-core/src/workspace_ops/merge/abort/evidence.rs:115-164`,
  `gwz-core/src/workspace_ops/merge/root/abort.rs:317-347`); and
- delegates replacement to `artifact::write_atomic`, which may create the
  pathname parent, renames by pathname, and ignores parent-directory sync
  failure (`gwz-core/src/artifact/mod.rs:418-420,442-474`).

The P1 bundle path has the same architectural escape: bundle observation uses
`symlink_metadata`/`read` on the final pathname and bundle publication calls
the same generic writer (`gwz-core/src/workspace_ops/merge/preserve/artifacts.rs:667-702,721-749`,
`gwz-core/src/stash/mod.rs:247-262`). No retained directory capability binds
observation to mutation.

**Failure scenario.** A rollback `Lock` action observes the exact workspace
lock. Before `write_atomic` stages its replacement, `.gwz` is renamed and
replaced by a symlink to a directory outside the workspace. The temporary and
final paths are then resolved in that outside directory; an unrelated
`lock.yaml` can be replaced without a final expected-source check there. The
equivalent bundle race redirects `.gwz/stash/bundles/stash_<id>.yaml`. A
separate failure mode occurs when rename succeeds and directory sync fails:
the helper reports success, so the journal can advance across a known
non-durable physical boundary.

**Impact.** A journal-owned reverse action can mutate an unowned path outside
the workspace. It can also durably claim progress after a failed durability
barrier, making post-crash evidence disappear. Both violate the no-unowned-
mutation and exact-restart boundaries.

**Required correction.** Introduce/reuse one capability-backed checked-artifact
primitive for bundle, evidence, and selected-root metadata writes/removals. It
must acquire canonical parents no-follow, retain the parent handle, repeat the
exact expected-source observation through that handle, perform handle-relative
same-directory replace/unlink, and propagate parent-sync failure. It must not
call `create_dir_all` for a persisted action. Keep phase policy in the current
owners; share only the physical primitive.

**Regression evidence.** For bundle write, evidence `Lock`/`Marker`, and
selected-root `Manifest`/`Lock`, inject parent and leaf replacement immediately
before the physical linearization point. Assert typed ambiguity/failure, byte-
identical record, no phase advance, and an outside-root sentinel unchanged.
Also inject file-sync, rename/unlink, and parent-sync failure and prove that no
successful attempt/progress write is issued.

### [P1-2] A persisted non-root stash can be executed against a different HEAD than its intent

**Evidence.** I2 requires the complete intent to be persisted and reread before
mutation (`GwzM5-8I2ActionJournalContract.md:186-189`), and the R4b executor is
allowed to create a native stash only after an immediate exact preimage recheck
(`GwzM5-8R4bReverseLifecycleInterface.md:339-354`). A pending stash carries
`head_commit` and `preimage_sha256`, but its `Before` classifier checks only the
image hash/dirt and absence of a matching stash
(`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/phase.rs:152-187`).
Execution then calls the generic stash operation without either expected value
(`gwz-core/src/workspace_ops/merge/v1_lifecycle/reverse/execute/preservation.rs:79-104`),
and the Git implementation checks native state and dirt but not expected HEAD
or preimage (`gwz-core/src/git/gitbackend/preservation.rs:107-184`).

**Failure scenario.** `CreateStash` is persisted for `head_commit=C1` and a
dirty untracked preimage, then the process stops. The user makes an empty commit
`C2`, leaving that dirty image unchanged. Resume classifies the persisted action
as `Before` and creates the native stash at `C2`. Post-observation rejects it
because the stash HEAD is not `C1`, leaving the checkout cleaned and the record
stuck on an action that cannot adopt the newly created stash.

**Impact.** GWZ physically moves user work under an identity not authorized by
the journal and then cannot complete or exactly replay the action. This is a
preservation/recovery integrity failure, not just a conservative rejection.

**Required correction.** Make non-root stash classification require attached
HEAD/ref at the persisted `head_commit`. Add a checked Git-backend stash seam
that takes the persisted expected HEAD and preimage and revalidates both
immediately before native stash creation. Third/mixed state must return
`PreservationEvidenceMismatch` without mutation.

**Regression evidence.** Stop after `BeginStash`, advance the attached branch
with an empty commit while retaining an identical dirty preimage, and resume.
Assert typed mismatch and byte-identical record, HEAD/ref, index/worktree,
native stash list, backup ref, and bundle.

### [P2-1] A persisted backup-ref action is not rebound to its recorded live checkout

**Evidence.** I2 says the backup target is the attached target observed at
preflight and every action payload must remain derivable from immutable record
plus exact preflight facts (`GwzM5-8I2ActionJournalContract.md:159-182`). With no
durable backup evidence, owner planning makes the *current* live commit the
protected commit (`gwz-core/src/workspace_ops/merge/preserve/plan.rs:627-676`).
For a pending backup, an absent ref is nevertheless accepted without comparing
the pending `target_commit` to that new live/protected commit (`:485-516`). The
pending classifier likewise treats ref absence alone as `Before`
(`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/phase.rs:29-40`),
and `create_backup_ref` checks the destination ref but not the attached checkout
(`gwz-core/src/git/gitbackend/preservation.rs:6-44`).

**Failure scenario.** A backup action targeting `C1` is persisted and the
process stops. The user advances the attached branch to descendant `C2`.
Resume accepts `C2` as the new protected commit, then creates the private ref at
stale `C1`. Once that result is recorded, the next plan rejects live `C2`
against durable backup `C1`, stranding preservation after a mutation.

**Impact.** The executor can perform an action whose payload no longer follows
from the live bound facts, violating exact intent replay and creating an
internally inconsistent recovery state.

**Required correction.** Pending backup observation and the immediate checked
executor must require attached HEAD/ref to equal persisted `target_commit`
before absent-ref means `Before`. Do not recompute pending-action authority from
the ambient live commit.

**Regression evidence.** Stop after `BeginBackupRef`, advance the branch, and
resume. Assert `PreservationEvidenceMismatch`, no private ref, and byte-identical
record, branch, checkout, stash list, and bundle.

### [P2-2] New work in an earlier skipped owner is stashed before a later durable bundle row is rejected

**Evidence.** The frozen rule is explicit: new earlier work must return
`PreservationEvidenceMismatch` without journal rewrite or physical mutation
(`GwzM5-8R4bTransitionDesign.md:736-754`; I2 contract `:152-156`). In the
action-free cursor, however, the implementation first verifies the bundle
against *all durable evidence*, then scans owners from the beginning and begins
the first newly needed stash (`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation/cursor.rs:14-42,135-155,232-259,316-365`).
Only after that native stash completes does its `WriteBundle` classifier compare
the bundle with slices ending at the earlier owner
(`gwz-core/src/workspace_ops/merge/preserve/artifacts.rs:667-702`).

**Failure scenario.** `mem_a` was clean and skipped; later `mem_b` completed its
stash and durable bundle row; the process stops immediately after that durable
result. The user creates work in `mem_a`. Resume accepts the existing `mem_b`
bundle as the exact all-evidence bundle, persists and executes a stash for
`mem_a`, then reaches `WriteBundle`. At that point the existing later `mem_b`
row is neither the empty before form nor the `mem_a`-only after form, so the
operation becomes ambiguous only after `mem_a` has been mutated.

**Impact.** The implementation violates the cross-owner proof boundary and can
create a native stash/journal action before discovering a durable later-owner
contradiction.

**Required correction.** Before issuing `BeginStash` (or any earlier regressed
position) from an action-free cursor, bind the candidate position to a proof
that no later durable evidence/bundle row contradicts it. New work in a
previously skipped earlier owner must reject before record or repository
mutation; do not solve this by silently reordering or adopting later evidence.

**Regression evidence.** Use two owners, complete only the later owner's stash
and bundle, stop action-free, add dirty work to the earlier owner, and resume.
Assert typed mismatch plus byte-identical record, both repositories, refs,
stash lists, and bundle.

### [P2-3] Canonical bundle rows follow manifest order, not stable owner-id order

**Evidence.** I2 requires selected owners and member rows to sort by stable
owner id (`GwzM5-8I2ActionJournalContract.md:302-312`). Selection deliberately
freezes manifest order; the existing plan test proves `mem_z, mem_a`
(`gwz-core/src/workspace_ops/merge/plan/tests.rs:184-199`). `expected_bundle`
iterates that plan order and appends both arrays without sorting
(`gwz-core/src/workspace_ops/merge/preserve/artifacts.rs:760-829`).

**Failure scenario.** A manifest selects `mem_z` before `mem_a` and both owners
are stashed. The generated YAML contains `selected_members` and member rows in
`mem_z, mem_a` order, while the canonical I2 bytes require `mem_a, mem_z`.

**Impact.** Settled code emits a noncanonical durable artifact. Equivalent
owner sets can have manifest-dependent bytes, weakening exact restart and
cross-driver/tool interpretation.

**Required correction.** Build bundle rows from durable owner evidence and sort
both selected-owner and member-row representations by the canonical stable
owner key before serialization. Preserve frozen cursor order only for action
scheduling.

**Regression evidence.** Exercise at least `mem_z, mem_a` plus a root owner;
assert canonical owner sorting, byte-identical restart observation, and the
before/after `WriteBundle` matrix under a cursor order different from sort
order.

### [P2-4] Rollback entry skips the selected-root clean-check while publication evidence owns the root

**Evidence.** Entry preparation must globally preflight every selected
participant before any reverse mutation (`GwzM5-8R4bReverseLifecycleInterface.md:308-316`).
When publication evidence is live, P2 preflight explicitly skips the selected
`@root` participant (`gwz-core/src/workspace_ops/merge/abort/preflight.rs:46-65`)
and later checks only that manifest/lock are regular files (`:110-124`). The
evidence preflight validates publication-owned files/index and HEAD but not
unrelated worktree dirt (`gwz-core/src/workspace_ops/merge/abort/evidence.rs:38-55,266-369,372-405`).
The publication snapshot deliberately ignores worktree changes outside its
candidate paths (`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/finalization/publication/live.rs:117-131`).
After evidence rollback, the selected-root participant classifier does require
an entirely clean checkout (`gwz-core/src/workspace_ops/merge/abort/participants.rs:93-120,190-207`).

**Failure scenario.** A successful selected-root participant has pending
publication evidence. The user creates or modifies an unrelated root file.
Rollback entry passes and begins rolling back evidence. Only after that owner is
durably retired does selected-root participant observation reject the dirty
checkout, leaving a partially rolled-back workspace that the entry preflight
was required to prevent.

**Impact.** A later selected participant can make rollback ineligible after an
earlier reverse owner has already mutated, recreating the cross-repository
inconsistency the global preflight contract exists to exclude.

**Required correction.** When publication evidence temporarily masks selected
`@root`, entry preflight must prove the virtual post-evidence root-participant
checkout: exact branch/commit/candidate overlay and no unrelated staged,
unstaged, untracked, unresolved, or native-state dirt. Perform this proof before
`BeginRollback`; publication-owned candidate differences remain the only
allowed exceptions.

**Regression evidence.** With selected `@root` and another member plus live
publication evidence, add unrelated root dirt and request abort. Assert rejection
before `BeginRollback` and byte-identical record, evidence commit/ref, member
repositories, index/worktree, and root artifacts.

## Pre-A1 backlog

### [P3-1] Rollback physical failures lose their typed diagnostic at the resolver boundary

**Evidence.** `no_progress` preserves the executor's code/message for
preservation but replaces every other non-participant failure, including
rollback, with generic “owned non-participant action made no progress”
(`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver/execution.rs:3-20`).

**Impact.** Recovery remains conservative, but an operator loses the actual
filesystem/Git failure needed to remediate an exact rollback action.

**Correction/test.** Preserve the original typed error and detail for rollback
as for preservation, adding the durable owner context where available. Inject a
distinct backend code/message for each rollback owner and assert that it reaches
the API unchanged while the record and physical state do not advance.

## Acceptance gate

P1/P2 is ready only after all six blockers have fixes with the stated negative
and restart tests, the complete focused P1/P2 suites pass, and an independent
re-review reports **no open P0/P1/P2 defect**. Until then the architecture does
not satisfy its frozen preservation, rollback, containment, and global-
preflight contracts.
