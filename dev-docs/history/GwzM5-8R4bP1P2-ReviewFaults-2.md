# R4b-P P1/P2 settled-tree fault/recovery re-review 2

Date: 2026-08-11

Review target:

- workspace commit `5e636cf6b288a79a1976322e55cd1d6a822cd60b`;
- `gwz-core` commit `6ffbb7f36cd40837c63b8331fc937ada82eb099e`;
- original review `GwzM5-8R4bP1P2-ReviewFaults.md`; and
- accepted remediation plan `GwzM5-8R4bP1P2-RemPlan.md`.

I reviewed this settled tree independently and did not read or coordinate with
the other re-review. The local Windows SDK limitation is accepted as stated in
the remediation plan; release-platform builds remain the Windows/macOS/Linux
x86/Linux arm64 platform gate.

## Verdict

**NO-GO.** There is no P0 finding, but three P1 defects and one P2 acceptance-
evidence defect remain. C4, C5, and C6 are materially closed. C1 and C3 are not
closed at their physical linearization boundaries, C2 still admits Git index
states that hide unrelated work from ordinary status, and C7 does not execute
the complete matrix accepted by the remediation plan.

The implementation remains production-disabled under `#[cfg(test)]`; these
findings therefore block acceptance of P1/P2 and A1 activation rather than
describing a released v1 execution path.

## Findings

### [P1-1] Checked artifact replacement/removal is not bound to the exact source at linearization

**Where**

- `gwz-core/src/checked_artifact.rs:23-28` represents an existing source only
  as bytes; the observed leaf identity is discarded.
- `gwz-core/src/checked_artifact.rs:190-209` runs the deterministic hook, then
  reobserves the parent/leaf, then invokes the mutation in a separate step.
- `gwz-core/src/checked_artifact.rs:235-250` has the same final-observe-then-
  remove window.
- `gwz-core/src/checked_artifact/platform.rs:7-27` uses an unconditional Unix
  rename when the destination was expected to exist; lines 50-95 set
  `ReplaceIfExists = true` on Windows.
- `gwz-core/src/checked_artifact/platform.rs:117-138` removes the named leaf on
  Unix and moves the named leaf to a tombstone on Windows without an atomic
  expected-identity condition.
- `gwz-core/src/checked_artifact/tests.rs:53-86` injects replacement at
  `BeforeFinalCheck`, before the final observation, not after that observation
  and before the syscall.

**Violated contract**

RemPlan section 3.1 requires exact-source checked replacement/removal and says
failure never accepts, overwrites, or removes a foreign source form. The exit
criterion also requires the observed parent and leaf identity/bytes to remain
causally bound to the checked mutation.

**Failure scenarios**

1. During an existing-prefix bundle update, replace the destination after
   `self.observe()` at line 195 returns and before `rename_relative` at line
   202. Unix and Windows both replace the foreign destination unconditionally.
2. During marker removal, replace the leaf after the observation at line 240
   and before `remove_relative` at line 248. The foreign leaf is removed (or
   renamed and then deleted on Windows).
3. Even at the current deterministic hook, replace the expected leaf with a
   different inode containing identical bytes. `CheckedArtifactFact::Bytes`
   compares equal, so the foreign object is accepted and replaced/removed.
4. Move the retained parent after `parent_is_current` returns but before the
   handle-relative mutation. GWZ mutates the retained directory capability
   even though that directory is no longer at the workspace-relative path.

The post-mutation observation cannot recover bytes or an object that GWZ has
already overwritten or deleted.

**Impact**

The shared primitive can destroy a foreign bundle, publication artifact,
selected-root artifact, or root-preservation artifact. This is the same C1
safety boundary the remediation was intended to centralize and close, so the
fact that all consumers now share it amplifies rather than contains the defect.

**Recommended correction**

Do not treat a final path observation followed by an unconditional syscall as
CAS. Carry a source identity in the checked fact and use an actually
identity-conditional physical protocol at the linearization point. If the
supported OS primitives cannot provide that directly, use a recoverable
exchange/quarantine protocol with a closed journaled classifier, or revisit
the frozen contract before activation. Add a hook in the real post-proof/pre-
syscall gap and test different-byte and same-byte inode replacement plus a
parent move at that gap on Unix and Windows.

### [P1-2] The checked non-root stash mutates after its persisted preimage proof has gone stale

**Where**

- `gwz-core/src/git/gitbackend/preservation.rs:268-299` decodes stashes,
  captures the complete preimage, and performs the final attached-HEAD check.
- Line 300 then delegates to `stash_for_merge_preservation`.
- `gwz-core/src/git/gitbackend/preservation.rs:175-220` performs another
  status query, stash-list query, and eligibility calculation inside that
  legacy routine before the actual `stash_push` at lines 221-230.
- `gwz-core/src/git/gitbackend/preservation.rs:301-314` detects an unexpected
  stash only after the checkout and native stash have already been mutated.
- `gwz-core/src/git/tests/g15.rs:198-249` changes the preimage before calling
  the checked seam; it does not inject a change between the final proof and
  native stash creation.

**Violated contract**

RemPlan sections 3.2 and R2 require the pending stash executor to bind the
persisted branch, HEAD, and complete `preimage_sha256` immediately before the
native mutation, and require altered staged, unstaged, or untracked content to
reject with no stash mutation.

**Failure scenario**

After the preimage capture at line 269, an editor or another Git process changes
a tracked, staged, or untracked path while the legacy status/stash-list work is
running. The legacy function still sees preservable dirt and stashes the new
image. The post-check then rejects because the native stash preimage differs
from the pending journal. The worktree is already cleaned and the operation is
left with an unrecorded native stash that `v1_preservation_owners` correctly
rejects on restart as foreign to the pending action.

An attached-branch advance in the same interval has the analogous result: the
native mutation can occur against a HEAD other than the persisted one, and only
the post-check reports it.

**Impact**

The user data remains potentially discoverable in the native stash, but GWZ
has performed an unauthorized worktree/index mutation and stranded the durable
journal in a state it cannot reconcile. That is a preservation/recovery safety
failure, not merely a narrower race in diagnostics.

**Recommended correction**

Do not call the older unconditional orchestration seam after the final proof.
Move all pre-mutation status/list work before the persisted preimage check and
put the actual native stash creation immediately behind one checked backend
boundary. Add a deterministic hook at that exact boundary. Because arbitrary
worktree writers are not governed by a Git ref transaction, also document and
prove the physical protocol that prevents or safely reconciles a write during
stash creation; another check with an unprotected gap is not sufficient.

### [P1-3] Global rollback preflight trusts status states that deliberately hide worktree drift

**Where**

- `gwz-core/src/workspace_ops/merge/abort/preflight.rs:169-195` implements the
  selected-root prospective proof with native state plus `backend.status`.
- `gwz-core/src/workspace_ops/merge/abort/participants.rs:190-207` uses the same
  status-based `clean_checkout` proof for ordinary integrated and no-mutation
  participants.
- `gwz-core/src/git/gitbackend/repository.rs:155-193` constructs ordinary
  libgit2 status without inspecting semantic index flags or hashing every
  tracked worktree path.
- In contrast, `gwz-core/src/git/gitbackend/preservation_image.rs:45-61` walks
  every index entry and lines 406-412 deliberately reject assume-valid,
  skip-worktree, intent-to-add, and unknown semantic flags.
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/entry.rs:69-125`
  covers staged, unstaged, and untracked dirt only.

**Violated contract**

RemPlan section 3.4 requires a complete index/worktree proof before
`BeginRollback`, including all unrelated paths, and requires every invalid row
to fail before record or repository mutation.

**Failure scenario**

For a selected root with outstanding publication evidence, or for an ordinary
integrated participant, mark an unrelated tracked path assume-unchanged (or
skip-worktree) and then change its worktree bytes. Libgit2's workdir/index diff
classifies those flags as unmodified, so `backend.status` can remain clean.
The exact HEAD/ref tests pass and selected-root publication evidence still
matches because it owns only the candidate paths. Rollback entry is accepted
and earlier publication/participant mutation can occur. A later safe checkout
may reject after that partial mutation or may act according to the semantic
flag; either outcome violates the global no-mutation-before-complete-preflight
guarantee.

**Impact**

The original selected-root partial-rollback failure remains reachable through
a valid Git index state, and the same incomplete clean-checkout primitive also
affects non-root participants.

**Recommended correction**

Base rollback entry on a complete canonical checkout image, not porcelain-like
status counts. Inspect every index entry and reject semantic/unknown flags (or
model them explicitly), compare the full index to the expected commit, and
verify every tracked worktree path plus untracked paths. Add whole-state
preflight tests for assume-valid and skip-worktree drift on both selected root
and a later non-root participant; assert no `BeginRollback` and byte-identical
records, refs, index, worktrees, and root artifacts.

### [P2-1] C7's service-level fault/recovery matrix is still a favored-fixture subset

**Where**

- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_fault_matrix.rs:17-29`
  derives the physical set from one fixture and asserts only backup,
  `CreateStash`, `WriteBundle`, and `ResetRef`.
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/mod.rs:372-377`
  fixes that fixture to the `Boundary`/`Staged` publication handoff.
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_successor_matrix.rs:114-140`
  enumerates all 22 durable phase successors, but uses the same handoff fixture;
  phases already classified complete do not exercise both sides of a physical
  mutation.
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/service_fault_matrix.rs:127-138`
  includes only `ResetIntegrated` for the participant lane and omits
  `AbortConflict`.
- Ambiguous root/evidence rows are valuable direct observer tests in
  `reverse_rollback/{phases,root_artifacts}.rs`, but they are not recovery-
  service cases for every retained pending phase.

**Violated criterion**

RemPlan R5 and the C7 exit criterion require every root preservation phase and
every participant/evidence/selected-root rollback step across physical failure,
post-physical crash, post-successor crash, fresh before/after/ambiguous
observation, restart, legal incoming requests, both root owner encodings, and
every handoff-present/absent form permitted by the phase graph.

**Impact**

The new matrices substantially improve evidence and all focused cases pass,
but they do not prove the accepted cross-product. A phase that mutates under a
`Baseline`, `Marker`, `Lock`, or pre-index handoff, a conflicted-participant
rollback, or an ambiguous retained journal can regress while the current
service matrices remain green.

**Recommended correction**

Generate cases from the phase/action enums and the legal publication
prefix/index forms rather than from the actions emitted by one fixture. Force
each phase to genuine Before, After, and Ambiguous physical forms; include
handoff absence where legal, both root owner encodings, `AbortConflict` and
`ResetIntegrated`, both legal incoming requests, exact action counts, exact
journal retention/advancement, and exact artifact/bundle bytes.

## Closure assessment

| Remediation item | Assessment |
| --- | --- |
| C1 checked artifact owner | **Open (P1-1).** Capability traversal and required durability are useful, but exact-source identity is not atomically bound to mutation. |
| C2 selected-root prospective proof | **Open (P1-3).** Ordinary staged/unstaged/untracked cases close; semantic index flags still bypass the complete checkout proof. |
| C3 persisted non-root stash binding | **Open (P1-2).** Persisted values reach the seam, but validation precedes additional legacy work and the actual mutation. |
| C4 checked backup-ref action | **Closed.** `HEAD`, the attached branch, and destination ref are locked in one transaction and validated against the persisted target. |
| C5 skipped-owner/later contradiction | **Closed.** The owner reconstruction rejects foreign native refs/stashes, the bundle prefix is checked, and later durable evidence is rejected before a regressed action. |
| C6 canonical bundle ordering | **Closed.** Both `members` and `selected_members` are sorted by stable owner ID before serialization. |
| C7 complete service matrices | **Open (P2-1).** Durable-successor coverage is broad, but the accepted physical/form/ambiguous cross-product is incomplete. |

## Verification performed

The exact requested commit pair was rechecked before and after review. The
following focused tests passed on the local macOS host:

- all seven `checked_artifact::tests`;
- foreign-bundle insertion before publication;
- root physical/successor boundary recovery;
- all 22 root durable phase successor restarts for both owner encodings;
- emitted rollback service fault boundaries; and
- selected-root staged/unstaged/untracked entry rejection.

`cargo fmt --check --all` and both workspace/core `git diff --check` passed.
These green tests are consistent with the findings: the artifact tests inject
before the final observation, the stash tests inject before the checked seam,
and the service generators do not instantiate the missing rows described
above.

R4b-G should remain blocked until P1-1 through P1-3 are corrected, the full C7
matrix is executable, the complete local/static/document gates pass, release-
platform builds provide the platform evidence, and two settled-tree re-reviews
report no P0/P1/P2 defect.
