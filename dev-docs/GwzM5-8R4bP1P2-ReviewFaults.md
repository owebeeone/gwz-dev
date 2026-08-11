# R4b-P P1/P2 settled-tree fault/recovery review

Date: 2026-08-11

Review target:

- workspace commit `06689bb2faf42050223f1d4dc3344d685b30c863`;
- `gwz-core` commit `569135a794df7c5a1fd4ed568df3575a11d2287d`.

Primary lens: adversarial crash/restart behavior, complete global preflight,
ambiguous and foreign live states, preservation-artifact durability, root
ownership, platform/path exposure, recovery exactness, and fault-matrix
closure. I did not read or coordinate with another settled-tree review.

## Verdict

**NO-GO.** There are no P0 findings, but two P1 and two P2 findings remain.
The selected-root entry defect violates the central no-mutation-before-global-
preflight guarantee. The bundle path is not a checked preservation-artifact
boundary and cannot meet the frozen no-overwrite/durability contract. P2 root
artifact mutations retain an observe/path-use race, and the required root and
rollback lifecycle fault matrices are not closed.

The implementation is still production-disabled under `#[cfg(test)]`, so none
of these findings describes a currently released v1 execution path. They are,
however, blockers to accepting P1/P2 and to A1 activation under the frozen
contracts.

## Findings

### [P1-1] Selected-root publication handoff bypasses complete global rollback preflight

**Where**

- `gwz-core/src/workspace_ops/merge/abort/preflight.rs:53-65` skips the selected
  `@root` participant whenever publication evidence remains.
- `gwz-core/src/workspace_ops/merge/abort/preflight.rs:110-124` then checks only
  that the manifest and lock are some regular files; it does not prove their
  exact bytes or the complete root index/worktree.
- `gwz-core/src/workspace_ops/merge/abort/evidence.rs:356-405` classifies the
  publication-evidence shape from HEAD and the scoped candidate files. Its
  `exact_evidence_head` checks native repository state but not dirty status or
  unrelated root paths.
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rollback.rs:83-122`
  runs publication evidence before participants and selected-root metadata.
- `gwz-core/src/git/gitbackend/scoped_evidence.rs:157-213` can therefore move
  the root ref before the skipped root participant is classified.

**Violated contract**

`GwzM5-8R4bReverseLifecycleInterface.md:308-316` requires entry to be a
complete read-only preflight over every selected participant, selected root,
publication handoff, and rollback artifact, with failure before the entry
transition or any reverse mutation. The required evidence at lines 612-613
also demands that a global-preflight failure leave every earlier repository,
record, ref, index, worktree, root file, stash, and bundle unchanged.

**Failure scenario**

Use an operation with selected `@root`, an exact outstanding publication
candidate/composition commit, and another selected member. Before `Abort`,
edit a tracked root path outside the publication candidate set, for example
the workspace manifest. The root repository remains in native Git state
`Clean`, the publication HEAD/candidate/marker/lock/boundary/index shape can
remain exact, and both root artifacts remain regular files. The entry
preflight therefore skips the root participant and succeeds. Rollback may
durably begin and roll back the publication commit and its files. Only when
the cursor reaches the root participant does the dirty full-checkout
classifier reject the live state.

**Impact**

An entry-invalid selected root is discovered after root mutation and possibly
after other participant mutation. This recreates the cross-repository partial
rollback condition that P0/P2 global preflight was designed to prevent.

**Recommended correction**

Do not substitute the two `RegularFileFact::Bytes(_)` checks for selected-root
preflight. Add one authority-owned, read-only prospective classifier for the
publication-handoff-plus-selected-root composition. It must prove the entire
current root index/worktree and native state, including paths outside the
scoped candidate set, and prove that completing the already-validated
publication rollback yields an exact before/after form for the immutable root
participant and selected-root metadata owners. The `@root` skip may remain
only if that stronger proof is issued and bound to the entry preview.

**Regression test**

Create selected `@root` plus a second member with outstanding publication
evidence, introduce staged, unstaged, and untracked drift outside the
publication candidate paths one case at a time, and invoke rollback entry.
Each case must fail before `BeginRollback`; assert byte-identical record,
unchanged root/member refs, index and worktrees, and unchanged marker, lock,
boundary, and manifest.

### [P1-2] Bundle publication is unanchored, replace-existing, and not durably closed

**Where**

- `gwz-core/src/workspace_ops/merge/preserve/artifacts.rs:832-858` checks only
  the final bundle leaf with `symlink_metadata` and then reopens it by path. It
  never validates or pins the `.gwz/stash/bundles` parent chain.
- `gwz-core/src/workspace_ops/merge/preserve/artifacts.rs:721-756` classifies a
  before state and then calls the generic bundle writer across an unprotected
  check/use window.
- `gwz-core/src/stash/mod.rs:260-261` delegates to the generic
  replace-existing atomic writer.
- `gwz-core/src/artifact/mod.rs:444-473` follows ambient parents, creates them,
  publishes with replacement, and ignores directory-sync failure.
- `gwz-core/src/workspace_ops/merge/preserve/artifacts.rs:686-700` accepts a
  missing bundle as `Before` only when the expected earlier prefix is empty;
  once one owner is durable, a missing directory entry is unrecoverable even
  though all canonical rows remain reconstructible from native stashes and
  durable evidence.

**Violated contract**

`GwzM5-8R4bReverseLifecycleInterface.md:359-363` requires atomic bundle writes
that never overwrite a noncanonical or differently populated bundle and says
bundles survive abort/archive/GC. `GwzM5-8I2ActionJournalContract.md:302-312`
defines the closed no-bundle/prior-prefix/after-prefix crash classifier and
forbids overwriting every third form.

**Failure scenarios**

1. Make `.gwz/stash/bundles` a symlink to another directory before preserve.
   With no destination leaf, `read_bundle_bytes` reports `None`, entry and the
   write phase accept it, and `write_bundle` creates the preservation file
   outside the workspace.
2. Insert or replace a foreign bundle after the checked read but before the
   rename. The generic writer replaces it; post-write verification sees only
   GWZ's bytes and cannot detect the overwritten object.
3. On Unix, lose the bundle-directory rename across a crash after record
   advancement, or surface a directory-sync failure. The sync error was
   ignored, and a later durable owner prefix rejects the missing bundle rather
   than reaching a defined reconstructible before state.

**Impact**

Preserve can write outside its workspace, destroy a foreign artifact, or
strand an otherwise exactly recoverable operation after a crash. This is the
artifact whose durability is meant to make preserved user work discoverable;
the failure is therefore safety-critical, not merely diagnostic.

**Recommended correction**

Introduce a bundle-specific checked artifact owner. Acquire and retain a
canonical no-follow parent capability; reject symlink/non-directory parents
and nonregular leaves; publish with a no-overwrite/CAS protocol bound to the
exact observed absent or prior-prefix identity; make directory durability a
required result rather than best effort; and define/reconcile the missing-
prefix restart row consistently with the frozen classifier. Do not route v1
preservation through the generic replace-existing `write_bundle` seam.

**Regression test**

Cover a symlinked `stash` parent, symlinked `bundles` parent, symlink leaf,
parent and leaf replacement after observation, foreign insertion immediately
before publish, exact prior-prefix update, directory-sync failure, and crash
before/after publication and before/after the durable phase rewrite. Assert no
outside write, no foreign-byte replacement, and deterministic before/after/
ambiguous recovery.

### [P2-1] P2 checked workspace-artifact mutations do not retain the observed leaf or parent

**Where**

- `gwz-core/src/workspace_ops/merge/root/artifact_facts.rs:14-31` observes a
  path with separate metadata and path-based read operations.
- `gwz-core/src/workspace_ops/merge/root/artifact_facts.rs:34-65` rechecks and
  then performs a generic replace or path-based remove after the observation
  capability has been discarded.
- `gwz-core/src/workspace_ops/merge/root/artifact_facts.rs:68-86` validates
  parent components but does not retain an anchored directory handle or parent
  identity across the mutation.
- These helpers execute publication evidence rollback at
  `gwz-core/src/workspace_ops/merge/abort/evidence.rs:139-163` and selected-root
  rollback at `gwz-core/src/workspace_ops/merge/root/abort.rs:317-347`.

**Violated contract**

`GwzM5-8R4bReverseLifecycleInterface.md:391-402` requires one checked mutation
per persisted step and canonical real parents, regular non-symlink leaves,
exact bytes, and no proof for a replaced/mixed form. Its executable rollback
matrix at lines 659-661 explicitly crosses noncanonical parent/leaf, symlink,
operational error, and crash boundaries.

**Failure scenario**

After `write_checked` verifies candidate lock bytes, replace `gwz.conf` with a
symlink or replace the lock leaf before `artifact::write_atomic` publishes.
The write can follow the new parent and replace a file that was never the
observed candidate. Likewise, after `remove_exact` reads the marker, replace
the leaf before `remove_file`; the executor removes the replacement.

**Impact**

An action described as checked can mutate outside the workspace or discard a
foreign edit. A later ambiguous observation cannot recover the overwritten or
deleted bytes.

**Recommended correction**

Move these rollback artifacts onto the same anchored, no-follow, stable-
identity leaf API required for root preservation. The exact observed parent
and leaf identity/bytes must remain causally bound to checked replace/remove;
replacement races must fail without touching either object. Preserve the
existing per-phase write-ahead journal and post-observation rules.

**Regression test**

Add deterministic hooks after the exact read and before replace/remove. Swap
the parent for a symlink, replace the leaf with a regular foreign file, and
change its bytes/mode. Cross publication boundary/lock/marker and selected-
root manifest/lock. Assert the foreign and outside files survive byte-for-byte,
the record/journal is unchanged, and fresh observation is ambiguous.

### [P2-2] The accepted lifecycle fault/recovery matrix is materially incomplete

**Where**

- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/faults.rs:17-80`
  crashes only four non-root physical actions.
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/real_git.rs:115-225`
  and `:228-310` assert one uninterrupted root phase sequence; they do not
  restart on both sides of each physical and durable root boundary.
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/recovery.rs:69-114`
  reaches only the first non-root stash/reset action classes, not every root
  phase.
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/faults.rs:14-118`
  covers only participant execution/re-observation. Evidence and selected-root
  steps have direct before/after tests, but no service-level crash/durable-
  rewrite matrix.
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/recovery.rs:9-30`
  covers only a participant recovery journal, not every evidence and root
  pending phase.

The lower G15 suite supplies valuable physical root leaf/index fault coverage,
but it cannot prove the lifecycle journal, reducer, service retry, incoming
request, and durable record boundaries above that backend.

**Violated contract**

`GwzM5-8R4bReverseLifecycleInterface.md:600-613` requires crash/restart and
incoming-abort tests between every preservation action, rollback coverage for
every evidence/root step, recovery for every pending phase, and whole-state
global-preflight assertions. Lines 625-628 require every physical action to
cross success/failure with fresh not-started/completed/ambiguous observation.
Lines 640-647 require restart on both sides of every root physical/result
boundary; lines 659-661 require crash on both sides of every rollback physical
and durable boundary. Amendment 2 §9.4 and §9.13 impose the same root restart
matrix.

**Failure scenario**

A wrong lifecycle phase mapping, reducer successor, attempt reconciliation,
or persisted-result boundary for any of the 19 root stash/reset phases, five
publication-evidence mutations, or two selected-root mutations can regress
while all existing direct backend and uninterrupted-sequence tests remain
green. Incoming `Abort` during a partially completed root preservation graph
is also not demonstrated to finish preservation before rollback entry.

**Impact**

The current green suite does not establish the acceptance claim for the most
failure-sensitive P1/P2 rows. This is a mandatory exit-evidence gap, not a
request for exhaustive incidental testing.

**Recommended correction**

Build table-driven service-level harnesses from the durable phase enums. For
each physical phase, inject: failure before mutation; crash after mutation
before result observation/write; crash after durable successor write; fresh
before/after/third observation; and restart to exhaustion. Cross both root
owner encodings, handoff-present/absent where legal, and incoming Preserve/
Abort behavior. Do the corresponding table for evidence, participant, and
selected-root rollback, including recovery-origin entry for every pending
phase.

**Regression test**

The generated matrix itself is the regression test. It should assert action
counts (no repeat), exact journal retention/advancement, byte-identical record
on rejected rows, exact refs/index/worktrees/artifacts, and preservation
exhaustion before rollback.

## Pre-A1 backlog (non-blocking for P1/P2 acceptance if tracked)

### [P3-1] P2 still relies on panic-style validated-record invariants

`gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rollback.rs:364,420,435,480,495`,
`gwz-core/src/workspace_ops/merge/abort/evidence.rs:129-161`, and
`gwz-core/src/workspace_ops/merge/root/abort.rs:337-343` unwrap pending actions,
successors, and optional wire fields. Current checked-record validation appears
to make these unreachable from an accepted stored record, so I do not classify
them as a settled P1/P2 safety blocker. Before A1, replace them with typed
`MergeRecordUnreadable`/`RecoveryEvidenceMismatch` errors and add malformed-
model unit tests so a future validator/constructor drift cannot turn a record
error into a process abort.

### [P3-2] Root managed blob algorithm is inferred from commit-string width

`gwz-core/src/workspace_ops/merge/preserve/artifacts.rs:633-645` selects SHA-1
or SHA-256 from `commit.len()` and synthesizes blob ids locally. Real SHA-1 and
SHA-256 repositories are covered and this is fail-closed for other widths, so
it is not an open acceptance blocker. Before activation, obtain the repository
object format as a typed backend fact (or have the backend derive the exact
blob object id) rather than treating an unvalidated string width as algorithm
authority.

## Exposure assessment

- Raw non-UTF-8 preservation paths are encoded before lossy display conversion
  and have G15 coverage. No P1 lossy-path defect was found.
- Conflict snapshots reject a non-UTF-8 conflict path with typed manual-
  recovery evidence at `gwz-core/src/git/gitbackend/merge_recovery.rs:88-103`;
  they do not silently drop it. That is safe-failing, although broader raw-path
  conflict support remains a future feature.
- Both SHA-1 and real SHA-256 root-preservation rows exist in the backend
  evidence. The P3 typed-algorithm issue above remains worth removing before
  activation.
- Windows-specific parent durability has backend/platform evidence, but the
  missing service-level lifecycle rows in P2-2 still apply independently of
  platform compilation.

## Checks run

- `cargo test -q preservation`: **86 passed**.
- `cargo test -q rollback`: **53 passed**.
- `cargo test -q reverse_`: **18 passed**.
- Workspace and core commits matched the requested review targets before the
  memo was created.

These green checks confirm the existing positive paths; they do not exercise
the P1/P2 scenarios above.

## Gate decision

Under the required rule **“no open P0/P1/P2”**, R4b-P P1/P2 is **NO-GO**.
Remediate P1-1, P1-2, P2-1, and P2-2, rerun the focused/full/static gates, and
obtain a settled-tree re-review before resuming R4b-G or A1 activation.
