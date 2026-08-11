# R4b-P P1/P2 settled-tree remediation plan

Date: 2026-08-11

Status: **accepted for implementation; R4b-G remains blocked until this plan
has passed its settled-tree re-review**.

## 1. Scope and evidence

This plan remediates the independent reviews of the settled P1/P2 checkpoint:

- workspace commit `06689bb2faf42050223f1d4dc3344d685b30c863`;
- `gwz-core` commit `569135a794df7c5a1fd4ed568df3575a11d2287d`;
- `GwzM5-8R4bP1P2-ReviewArch.md`; and
- `GwzM5-8R4bP1P2-ReviewFaults.md`.

The reviews report no P0, but their seven unique P1/P2 findings make the
checkpoint NO-GO. This plan does not change the frozen v1 wire model, add a
durable phase or request, enable the production v1 decoder/dispatcher, or
start R4b-G. It corrects the implementation beneath the existing I2 action
journal and reverse-lifecycle interfaces.

## 2. Consolidated blockers

| ID | Consolidated defect | Source findings |
| --- | --- | --- |
| C1 | bundle and rollback artifact mutations lack one capability-backed, no-follow, exact-source physical primitive | Arch P1-1; Faults P1-2/P2-1 |
| C2 | selected `@root` can enter rollback without a complete virtual post-evidence checkout proof | Arch P2-4; Faults P1-1 |
| C3 | a persisted non-root stash can execute against a HEAD/preimage other than the recorded action | Arch P1-2 |
| C4 | a persisted backup-ref action can create its ref after the attached branch advances | Arch P2-1 |
| C5 | new work in an earlier skipped owner can be mutated before a later durable owner contradiction is rejected | Arch P2-2 |
| C6 | canonical bundle rows use cursor/manifest order rather than stable owner-ID order | Arch P2-3 |
| C7 | service-level preservation and rollback crash/recovery matrices do not cover every required physical and durable boundary | Faults P2-2 |

The following P3 items remain visible pre-A1 backlog, not P1/P2 exit blockers:

- replace rollback-path validated-record `unwrap`/`expect` calls with typed
  record/recovery errors;
- obtain blob object format from a typed repository fact rather than commit
  string width; and
- retain the original typed rollback physical diagnostic through the resolver.

## 3. Controlling architecture

### 3.1 One checked-artifact owner

Add a crate-private, policy-neutral checked-artifact module. It is the only
physical implementation used by:

- root-preservation managed regular files;
- the v1 preservation bundle;
- publication boundary/lock/marker rollback; and
- selected-root manifest/lock rollback.

Its acquisition returns a retained root and canonical parent capability plus
the parent identity and leaf name. Parent traversal is component-by-component,
no-follow, and distinguishes a genuinely absent component from a symlink,
non-directory, or replaced component. Leaf observation opens no-follow, accepts
only a non-executable regular file, binds opened-file identity to directory
metadata, reads bytes through the opened handle, and revalidates identity.

The public crate-private operations are deliberately small:

```text
acquire(root, relative) -> CheckedArtifact
observe() -> Missing | Bytes(exact bytes) | Invalid
replace_exact(expected Missing|Bytes, goal bytes)
remove_exact(expected Bytes)
```

`replace_exact` creates a unique temporary sibling, writes and flushes it,
runs the deterministic pre-linearization test hook, then revalidates the
retained parent and exact source through the same capability immediately
before a handle-relative rename. An absent destination uses atomic no-replace;
an existing destination uses exact-source checked replacement. `remove_exact`
uses the same final proof before handle-relative unlink. Both operations make
parent-directory durability a required result. Windows uses a relative
write-through rename; Unix flushes the retained parent directory. Failure
removes only GWZ's temporary sibling and never accepts or overwrites a foreign
source form.

The physical layer reports structural/operational facts without selecting a
merge policy error. The preservation and rollback owners map those facts to
`PreservationEvidenceMismatch` and `MergeRecoveryRequired` respectively.

No persisted preservation or rollback action creates its parent hierarchy.
The v1 merge-start/storage invariant prepares the canonical
`.gwz/stash/bundles` hierarchy before an operation can enter preservation; the
preservation entry proof requires that hierarchy to be present and canonical.
The disabled v1 test fixtures establish the same invariant. A missing or
noncanonical hierarchy at action time is an evidence mismatch, not permission
to call `create_dir_all`.

### 3.2 Persisted Git action authority

Add two checked backend seams rather than composing read-only observation with
an older unconditional mutation:

```text
create_backup_ref_checked(path, branch, expected_head, name, target)
stash_for_merge_preservation_checked(
    path, branch, expected_head, expected_preimage_sha256,
    merge_id, include_untracked
)
```

The backup-ref seam validates and locks the attached HEAD/branch and
destination ref, rechecks that the branch equals the persisted target, and
creates only an absent destination or accepts the exact already-complete ref.
The stash seam rechecks native state, attached HEAD/branch, branch target, the
complete canonical preimage digest, and absence of a matching stash immediately
before invoking stash creation. `--force` has no role in either seam.

Pending-action observers classify `Before` only from those same persisted
`branch`, `target/head_commit`, and `preimage_sha256` values. Ambient live HEAD
may describe an `After` state, but never supplies authority for a `Before`
state or replaces a persisted action value.

### 3.3 Cursor and bundle authority

Cursor order remains the frozen selected-owner order. Before issuing any
regressed action for owner N, the action-free observer proves that no later
owner has durable preservation evidence or a native/bundle artifact. Thus an
owner previously skipped as clean cannot acquire new work and be stashed in
front of an already durable later owner.

Canonical artifact order is separate from execution order. Bundle
`selected_members` and `members` rows are sorted by stable owner ID before YAML
serialization. Root uses its stable `@root` ID; duplicate IDs or a row/owner
mismatch are evidence errors. Cursor scheduling is unchanged.

### 3.4 Selected-root prospective rollback proof

Rollback entry keeps evidence-first execution, but it may skip direct
selected-root participant observation only after issuing one read-only
prospective proof for the composition:

1. publication evidence is at an exact rollback-representable prefix;
2. its immutable parent is exactly the selected-root participant result form;
3. HEAD is attached to the recorded branch and native Git state is clean;
4. the complete index/worktree has no unresolved entry and no staged,
   unstaged, renamed, deleted, type-changed, or untracked path outside the
   publication-owned candidate set; and
5. every candidate-set difference is already accepted by the closed
   publication evidence classifier, while selected-root result manifest/lock
   bytes and the operation baseline are both derivable and exact.

This proves the checkout that will exist after evidence retirement before
`BeginRollback` is written. Any unrelated root dirt rejects entry with a typed
root error and byte-identical record, refs, index, worktree, and artifacts.

## 4. Implementation sequence

### R1 — freeze the physical interface

1. Add the checked-artifact module and deterministic fault hooks.
2. Add focused no-follow, replaced-parent, replaced-leaf, foreign insertion,
   exact-prefix replacement, unlink, directory-sync, and before/after crash
   tests.
3. Move root-preservation regular-file operations onto the new module without
   changing their public backend contract or physical classifier.
4. Move v1 bundle and P2 rollback artifacts onto the same module.
5. Run G15 root-preservation, bundle, and root-artifact matrices before any
   policy correction continues.

R1 is an interface checkpoint. If it requires a new wire phase, a new action,
or cannot provide the existing Windows/macOS/Linux durability semantics, stop
for design review rather than adding a local compatibility path.

### R2 — bind persisted Git actions

1. Add and implement the two checked backend seams.
2. Make the pending backup observer require persisted attached HEAD/ref before
   absent destination means `Before`.
3. Make the non-root stash observer and executor bind persisted HEAD and
   preimage immediately before mutation.
4. Add stopped-after-begin tests that advance HEAD or alter staged, unstaged,
   and untracked content; each must reject with no ref/stash/record mutation.

### R3 — close cursor and bundle ordering

1. Separate scheduling order from canonical artifact order.
2. Reject a candidate earlier action when any later durable/native/bundle
   evidence makes that position stale.
3. Sort both bundle owner representations by stable owner ID.
4. Add the two-owner skipped-then-dirty regression and a `mem_z`, `mem_a`,
   `@root` canonical-byte fixture whose cursor order differs from artifact
   order.

### R4 — complete selected-root entry preflight

1. Add the authority-owned prospective selected-root classifier.
2. Bind it into the sealed rollback entry proof; do not duplicate it in the
   executor.
3. Test staged, unstaged, untracked, rename/type-change, unresolved/native
   state, and allowed publication-candidate differences with selected root plus
   another member.
4. For every rejected row assert no `BeginRollback`, byte-identical record,
   and unchanged repositories and root artifacts.

### R5 — close service-level fault/recovery evidence

Generate table-driven cases from the durable phase enums rather than manually
listing a favored subset. For every root preservation phase and every
publication-evidence, participant, and selected-root rollback step, cross:

- failure before physical mutation;
- crash after physical mutation and before result observation/write;
- crash after the durable successor write;
- fresh `Before`, `After`, and third/ambiguous observation;
- restart to exhaustion;
- incoming `Preserve` and `Abort` where legal;
- selected-root participant and publication-root owner encodings; and
- root handoff present/absent where the phase graph permits it.

Assertions include exact action counts, no repeated physical mutation, exact
journal retention/advancement, exact bundle/artifact bytes, and preservation
exhaustion before rollback. Test owners remain concept-specific and under 500
lines; split generated matrices by preservation, evidence rollback, and
selected-root rollback rather than creating one lifecycle test god-file.

### R6 — verification and settled-tree review

Run:

- all new focused regression matrices;
- all preservation, rollback, reverse, lifecycle, and G15 tests;
- the full `gwz-core` suite;
- `cargo fmt --check`;
- `cargo clippy --all-targets -- -D warnings`;
- merge documentation manifest/tripwire checks; and
- the repository's release-platform evidence gate where locally available.

Commit the complete remediation as one settled checkpoint with installed GWZ.
Return that exact workspace/core commit pair to two independent reviewers. R4b-G
may resume only when the re-review has no open P0/P1/P2 finding.

## 5. Path and cohesion amendment

The existing P1/P2 manifest is amended only for the shared physical owner and
focused evidence required by these findings:

- production: `gwz-core/src/checked_artifact.rs`,
  `gwz-core/src/checked_artifact/{fault,platform}.rs`, plus the existing P0.2/P1/P2
  consumers named in the reverse-lifecycle interface;
- backend contract/delegation: existing
  `git/gitbackend/{contract,preservation}.rs` and `git/gitbackend.rs`;
- tests: `gwz-core/src/checked_artifact/tests.rs`,
  `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/invariants.rs`,
  `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_fault_matrix.rs`,
  `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_successor_matrix.rs`,
  `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/service_fault_matrix.rs`,
  and the sub-500-line G15 boundary owner
  `gwz-core/src/git/tests/g15/root_preservation/fault_boundaries.rs`; and
- docs: this plan, both settled-tree reviews, and their re-review memos.

The 1,000-line threshold remains a cohesion review trigger, not a hard rule.
Any file that is becoming a concept dump is split earlier; any warranted split
targets children below 500 lines. New paths must be recorded here before use,
and code movement is reported separately from net semantic growth.

## 6. Implementation evidence

The remediation candidate closes C1-C7 on the local settled tree:

- C1: root preservation, preservation bundles, and rollback workspace
  artifacts share the capability-backed checked-artifact primitive and its
  deterministic replacement/removal fault tests;
- C2: selected-root publication handoff preflight rejects unrelated staged,
  unstaged, untracked, rename/type-change, unresolved, and native-state drift
  before `BeginRollback`;
- C3/C4: persisted stash and backup-ref actions use checked backend seams that
  bind the recorded branch, HEAD, target, and complete preimage;
- C5: an earlier skipped owner is rejected before mutation when a later owner
  already has durable evidence;
- C6: bundle owner rows and selected IDs are serialized in stable ID order;
  and
- C7: generated service matrices pin the exact emitted rollback and root
  mutation sets, cross both sides of every physical/durable boundary, enter
  recovery from every pending successor, and separately restart after every
  one of the 22 root preservation phase successors for both owner encodings.

Verification on 2026-08-11:

- `cargo test --all-targets --no-fail-fast`: 1,113 passed, 1 ignored;
- `cargo fmt --check`: passed;
- `cargo clippy --all-targets -- -D warnings`: passed;
- merge document consistency: 10 sources and 133 assertions passed;
- merge document checker unit tests: 3 passed; and
- `git diff --check`: passed.

The local macOS host cannot compile the Windows dependency stack because its
Windows C/zlib SDK is absent. Per the project release policy, the exact-tree
Windows, macOS, Linux x86, and Linux arm64 release builds remain the platform
evidence gate before activation/release; this local environment limitation is
recorded for the settled-tree reviewers rather than replaced with an
unrepresentative cross-check.

## 7. Exit criteria

P1/P2 is accepted only when all of the following are true:

- C1-C7 each has an executable regression test and passes;
- no v1 persisted action uses ambient `create_dir_all`, generic
  replace-existing artifact publication, or path-based unlink;
- no pending Git action derives its expected commit or preimage from ambient
  live state;
- selected-root global preflight rejects every unrelated-dirt row before
  record or repository mutation;
- bundle bytes are canonical independent of cursor/manifest order;
- the complete generated service fault/recovery matrices pass;
- full/static/doc/platform gates pass; and
- two independent settled-tree re-reviews report no P0/P1/P2 defect.

Only then does the main sequence continue with R4b-G followed by the two final
full-tree reviews.
