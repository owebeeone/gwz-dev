# R4b-P P1/P2 settled-tree architecture re-review 3

Date: 2026-08-12

Reviewed exactly:

- workspace commit `1c6bbba13586cb8be50576301072b38f0e0a3463`;
- `gwz-core` commit `7d1a3e6a33cf2e4b5d882db1f7196f16ea7cc87d`;
- `gwz-cli` commit `3cca145c0b32410f250f640730ed7ca18f1da59f`;
- `GwzM5-8R4bP1P2-ReviewArch-2.md`;
- `GwzM5-8R4bP1P2-ReviewFaults-2.md`; and
- the accepted second remediation plan
  `GwzM5-8R4bP1P2-RemPlan-2.md`.

I did not read or coordinate with the other reviewer assigned to this settled
tree. That review file appeared as an untracked workspace file during this
review and was left unread and untouched. The I2 journal and R4b transition
design remain controlling. The production v1 decoder/dispatcher remains
disabled, so the findings block acceptance and activation rather than describe
an enabled release path.

## Verdict

**NO-GO for accepting the R4b-P P1/P2 checkpoint or starting R4b-G.**

There is no P0 finding. One P1 implementation defect and three P2 findings
remain. D2 and D3's rollback-entry correction are materially closed, and the
durability barrier added for D1 is real. D1 is nevertheless not restart-closed,
rollback completion can be certified without live proof of the completed
participant prefix, and D4 still does not provide the generated C7 evidence
required by the accepted plan.

## Findings

### [P1-1] Restarting checked removal can delete the quarantined source after the managed parent changes

**Where:**

- `gwz-core/src/checked_artifact/transition.rs:228-303`;
- `gwz-core/src/checked_artifact/transition.rs:306-350`;
- `gwz-core/src/checked_artifact/transition.rs:357-376`;
- `gwz-core/src/checked_artifact/residue.rs:56-110`;
- `gwz-core/src/checked_artifact/residue.rs:172-194`; and
- `gwz-core/src/checked_artifact/residue.rs:202-215`.

**Violated contract.** RemPlan-2 section 3.1 requires the action key to bind the
retained parent identity, requires a moved parent or foreign source inode to be
`Ambiguous`, and says neither may be deleted. Its closed table permits source
residue only with the managed leaf absent or at the exact goal. Exit criterion
2 requires that no foreign source inode or moved parent be removed.

**Evidence.** `replace_exact` exhaustively rejects an initial `Ambiguous`
classification, but `remove_exact` only special-cases `After` at lines 228-231
and proceeds for `Before`, `Recoverable`, and `Ambiguous`. On a restart it then
skips detachment whenever any apparently matching source residue exists,
syncs the currently opened managed parent, and calls `cleanup_source`.
`cleanup_source` re-parses the residue and unconditionally removes its name.

The deterministic action key contains path, expected bytes, and goal, but not
the retained parent identity. The source name contains only the original
parent identity. The original detached source inode is checked during the
first process, but its identity is not retained in the name or other durable
metadata. On restart `inspect_residue` accepts whichever current inode has the
expected bytes under that name. The classifier also labels source residue with
the managed leaf still equal to the original source as `Recoverable`, although
that combination is outside the plan's closed table.

**Failure scenario.** Start checked removal of an exact source and stop at
`AfterDetach`, leaving the original in quarantine and the managed leaf absent.
Move the managed parent aside and create a new empty parent at its canonical
path. A fresh `CheckedArtifact` retains the new parent. Its classifier returns
`Ambiguous` because the source residue names the old parent, but
`remove_exact` ignores that result. Since a residue source exists, it performs
no new detach; it syncs the new empty parent, deletes the quarantined original,
observes the managed leaf as missing, and returns success. The user's only
original source bytes are gone.

A second restart case replaces the deterministic quarantine source with a
same-byte, different-inode file. The restart accepts and deletes that foreign
inode because the original source identity is no longer available. A stale
`.goal` is likewise not scoped by retained parent identity.

**Impact.** This is destructive data loss in the shared physical primitive
used by root-preservation files and rollback artifacts. It defeats the central
exact-source guarantee of D1 and is a P1 blocker even though the immediate
post-proof parent-move and same-byte replacement tests pass in one process.

**Required correction.** Make `remove_exact` exhaustively reject
`Ambiguous` before opening or cleaning quarantine, as replacement already
does. Persistently bind the whole residue namespace to the retained parent and
bind a detached source to its exact source identity, then revalidate both
before cleanup. Make cleanup an exact-identity conditional operation that
fails closed. Implement the plan's closed residue/managed-leaf table literally;
in particular, source residue plus exact original source is not recoverable.

**Regression evidence required.** Add removal restart cases for:

- stop after detach, replace the parent, then resume without deleting either
  the quarantined source or content in the new parent;
- replace the quarantined source with same bytes on a new inode, then resume
  without deleting it;
- source residue plus a managed exact-source leaf returning `Ambiguous`;
- stale goal/source residue from a different retained parent; and
- each real removal consumer, including selected-root/publication rollback
  and root preservation.

### [P2-1] Rollback skips live proof of already completed participants and can certify a false terminal rollback

**Where:**

- `gwz-core/src/workspace_ops/merge/model/v1/validate/journal.rs:214-262`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rollback.rs:75-124`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rollback.rs:348-390`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rollback.rs:513-517`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe.rs:93-121`; and
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/rolling_back_recovery.rs:6-70`.

**Violated contract.** The frozen transition design requires
`VerifiedRollbackExhausted` to cover every participant and the exact selected-
root baseline (`GwzM5-8R4bTransitionDesign.md:434`). It also requires the
selected-root baseline to be reobserved after its journal clears
(`:658-665`). The same live-prefix principle must apply to ordinary
participants: a terminal row is retained history, not current physical proof.

**Evidence.** `rollback_cursor` skips every `Aborted` or `RolledBack`
participant based solely on the record. `observe_cursor` live-observes only the
current nonterminal participant or pending action. At `Complete`,
`rollback_exhausted` checks only the pure record cursor; when selected root is
present it additionally reads manifest/lock baselines, but it never observes
ordinary terminal participants. Recovery-origin verification likewise checks
only the current `pending_rollback` owner.

**Failure scenario.** In a multi-member rollback, let `mem_z` finish
`ResetIntegrated` and durably record `RolledBack`, then stop. Before resume,
change `mem_z`'s branch, HEAD, index, worktree, native state, or a semantic index
flag. Resume skips `mem_z`, can mutate `mem_a`, and can ultimately issue
`AbortOperation` because the record says the earlier row is terminal. The
operation reports an atomic abort even though `mem_z` no longer equals its
exact pre-operation checkout. If drift is introduced before a later owner is
processed, the service can also mutate that later owner before discovering
nothing, because it never rechecks the completed prefix.

**Impact.** Restart no longer fails closed over the complete rollback prefix.
The terminal `Aborted` state can make a false whole-workspace claim, and a
later repository can be mutated after an earlier completed repository has
drifted.

**Required correction.** Add an authority-owned live rollback-prefix proof.
Before observing or executing the next owner, and again before issuing
`VerifiedRollbackExhausted`, verify every record-terminal participant against
its exact rollback result using the complete checkout/ref/native-state facts,
not status counts. Keep the selected-root exact-baseline proof in the same
closed aggregate. Any mismatch must enter typed recovery without issuing a
later physical action or durable successor.

**Regression evidence required.** Stop after a participant's durable rollback
successor, drift that completed participant, and resume with both `Abort` and
`Preserve`. Cross ordinary staged/unstaged/untracked drift, branch/HEAD/native
drift, semantic index flags, and both `Aborted` and `RolledBack` participant
forms. Assert zero later-owner physical calls, byte-identical record, and no
terminal `AbortOperation`. Add the equivalent terminal-exhaustion rows.

### [P2-2] Quarantine selection is Git-discovery based rather than artifact-root based

**Where:**

- `gwz-core/src/checked_artifact/observation.rs:74-118`;
- `gwz-core/src/checked_artifact/platform.rs:7-31`;
- `gwz-core/src/checked_artifact/platform.rs:55-101`; and
- `gwz-core/src/git/gitbackend/preservation_root/files.rs:66-81`.

**Violated contract.** RemPlan-2 section 3.1 requires quarantine below the
workspace runtime directory, using the retained Git directory only when that
directory itself is the artifact root. It must never be selected merely
because a worktree-root artifact happens to live in a discoverable Git
repository.

**Evidence.** `CheckedArtifact::acquire` calls `git2::Repository::open(root)`
and, whenever that succeeds, chooses `repo.path()/gwz/checked-artifacts` as the
private root. Root-preservation callers pass the worktree root and a worktree-
relative artifact path. Therefore ordinary worktree artifacts quarantine in
the Git directory even though the Git directory is not their artifact root.
The platform operation is one atomic rename between the managed parent handle
and quarantine handle; it has no cross-filesystem protocol.

**Failure scenario.** Use a supported linked-worktree/separate-git-dir layout
whose worktree and Git directory are on different filesystems. Checked removal
or replacement of a root-preservation artifact tries to rename from the
worktree filesystem into the Git-dir filesystem and fails with `EXDEV` (or the
Windows equivalent) before it can implement the frozen transition. This is a
valid Git layout and can affect any worktree artifact requiring preservation.

**Impact.** The shared primitive is unavailable for a supported repository
layout, and its namespace placement contradicts the reviewed architecture.
Build-only platform evidence will not exercise this behavior.

**Required correction.** Make quarantine policy explicit at each artifact
root. Worktree-root artifacts use a private workspace runtime directory on the
same artifact filesystem; Git-dir artifacts use the retained Git directory.
Do not infer policy from repository discovery. Add separate-git-dir and linked-
worktree tests, including a cross-device boundary where the test environment
supports it, plus an assertion that quarantine is outside the managed
classifier but on the source filesystem.

### [P2-3] The C7 matrices are still fixture-derived subsets rather than the accepted legal cross-product

**Where:**

- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_fault_matrix.rs:16-158`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_fault_matrix.rs:186-281`;
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_preservation/root_successor_matrix.rs:218-259`; and
- `gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/reverse_rollback/service_ambiguity_matrix.rs:14-153`.

**Violated contract.** RemPlan-2 section 3.4 and R5 require generation from the
closed phase/action vocabulary plus a legal-form table. Each legal row crosses
root owner, handoff including absence, request, phase/action, physical
`Before`/`After`/`Ambiguous`, interruption/durability boundary, exact action
count/bytes, and terminal exhaustion. Exit criterion 6 makes this executable
evidence part of acceptance.

**Evidence.** `physical_cases` traces actions emitted by five chosen handoff
fixtures and then deduplicates them by target, so each target is tested under
only its first observed handoff rather than every legal target/form pair.
There is no handoff-absence form. Its `expected_targets` list omits both
`Stash(RestoreParent)` and `Reset(RestoreParent)`, although the complete
successor vocabulary in `root_successor_matrix` includes those phases. Thus
those physical phases have no pre-/post-mutation fault row.

The rollback ambiguity loop enumerates actions and both requests, but
`AmbiguousRuntime` fabricates `BoundAmbiguityEvidence` directly. It does not
place each real observer in a fresh physically ambiguous state, so it tests
service reduction of an ambiguity fact rather than the production
observe/classify/service composition. The newer rollback fault matrix is
materially broader, but these missing root-form and real-observer crosses mean
the accepted generated gate is still not present.

**Impact.** Enum coverage and all green focused tests can coexist with an
untested physical phase/form or a production classifier that never emits the
expected ambiguity. P1-1 is a concrete restart/classifier defect that the
current matrix does not expose.

**Required correction.** Define a declarative legal-form table and generate
the full cases from enum vocabularies without target deduplication. Include
handoff absence where legal, both `RestoreParent` physical phases, every legal
target/handoff/owner/request combination, real fresh `Before`/`After`/
`Ambiguous` observer states, all durability boundaries, exact calls/bytes, and
terminal exhaustion. Coverage assertions must fail when either an enum value
or legal form lacks a row.

## D1-D4 closure audit

| Item | Result | Review conclusion |
| --- | --- | --- |
| D1 checked-artifact transition | **Open** | Canonical-parent rebarriering and the one-process exact-source checks are improvements, but removal ignores an ambiguous restart and can delete the original source (P1-1). Quarantine placement is not artifact-root scoped (P2-2). |
| D2 checked native stash | **Closed** | Read-only preparation precedes the final branch/HEAD/preimage/stash-set proof, the deterministic boundary is immediately before the single native mutation, post-verification remains, and the documented concurrency contract matches the plan. |
| D3 complete rollback entry | **Entry correction closed; lifecycle prefix open** | Complete checkout authority and the shared selected-root result-artifact verifier are used before `BeginRollback`. A separate later defect allows completed rollback participants to escape live proof (P2-1). |
| D4 generated C7 matrix | **Open** | Coverage is broader, including both rollback requests and lanes, but the root physical/form cross-product and real-observer ambiguity rows remain incomplete (P2-3). |

Retained C4 (checked backup ref), C5 (preservation completed-prefix guard), and
C6 (canonical owner ordering) remain closed. The root-inclusive canonical
bundle and stopped-after-intent stash cases requested by RemPlan-2 are present.
No v1 wire/model phase was added and production activation remains off.

## P3 backlog

No new P3 finding is required for this verdict. The existing pre-A1 backlog
remains applicable:

- preserve the original typed rollback physical diagnostic through the
  resolver;
- replace rollback-path validated-record `unwrap`/`expect` calls with typed
  record/recovery errors; and
- derive blob object format from a typed repository fact rather than commit
  string width.

These are separate from the P1/P2 blockers above.

## Verification

On the exact reviewed commits:

- workspace, `gwz-core`, and `gwz-cli` commit IDs matched the requested
  checkpoint; the reviewed tree was clean at review start;
- `cargo test --all-targets --no-fail-fast -q`: **1,276 passed, 1 ignored, 0
  failed** across the workspace;
- focused checked-artifact tests: **13 passed**;
- focused rollback service tests: passed;
- `cargo fmt --all -- --check`: passed; and
- `git diff --check` for workspace, `gwz-core`, and `gwz-cli`: passed.

The full test gate is useful regression evidence, but it cannot supersede the
explicit closed-form and generated-matrix acceptance criteria. Exact-tree
Windows, macOS, Linux x86, and Linux ARM64 release builds remain required
platform evidence; this review did not rerun those external builds.

## Exit decision

Do not accept P1/P2 and do not start R4b-G. Remediate P1-1 and P2-1 through
P2-3 as one reviewed architecture correction, rerun the full local/static/
document/platform gates on one settled tree, and repeat the two independent
acceptance reviews. R4b-G remains blocked until both report no P0/P1/P2.
