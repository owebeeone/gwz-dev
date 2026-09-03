# R4b-P P1/P2 second remediation plan

Date: 2026-08-11

Status: **accepted for implementation; R4b-G and A1 remain blocked until the
settled-tree re-review gate in R7 passes**.

## 1. Scope and controlling evidence

This plan responds to the two independent re-reviews of the first remediation
checkpoint:

- workspace commit `5e636cf6b288a79a1976322e55cd1d6a822cd60b`;
- `gwz-core` commit `6ffbb7f36cd40837c63b8331fc937ada82eb099e`;
- `GwzM5-8R4bP1P2-ReviewArch-2.md`; and
- `GwzM5-8R4bP1P2-ReviewFaults-2.md`.

Both reviews are NO-GO. There is no P0 finding. This plan closes their four
unique implementation defects and the incomplete acceptance matrix without
starting R4b-G, changing the v1 wire model, adding a durable lifecycle phase,
or enabling the production v1 decoder/dispatcher.

Where this plan is more specific than
`GwzM5-8R4bP1P2-RemPlan.md`, it supersedes that plan's physical implementation
detail. The I2 action journal and reverse-lifecycle interfaces remain frozen.

## 2. Consolidated blockers

| ID | Defect | Source findings |
| --- | --- | --- |
| D1 | checked-artifact mutation is neither exact-source conditional nor restart-closed across its durability boundary; Windows can strand an unowned tombstone | Arch P1-1/P2-1; Faults P1-1 |
| D2 | checked stash proof becomes stale before libgit2 starts the native mutation | Faults P1-2; Arch C3 evidence gap |
| D3 | rollback entry uses status rather than a complete checkout image and defers exact selected-root result-artifact proof | Arch P2-2; Faults P1-3 |
| D4 | the C7 service matrix is fixture-derived and omits legal owners, handoffs, requests, physical/durability sides, and ambiguous observations | Arch P2-3; Faults P2-1 |

C4, C5, and C6 from the first remediation are retained as closed production
code. Their missing evidence rows are included in D4. The existing P3 backlog
remains pre-A1 work and is not promoted into this P1/P2 gate.

## 3. Frozen architecture

### 3.1 Checked-artifact transition, not a path-based write helper

The physical owner becomes a restartable transition over one managed leaf. A
plain byte observation is insufficient because it loses both source identity
and the distinction between a visible goal and a durably completed goal.

The internal interface is:

```text
acquire(root, relative, policy) -> CheckedArtifact
observe_durable() -> Missing | Bytes | Invalid
classify_replace(expected, goal) -> Before | After | Recoverable | Ambiguous
classify_remove(expected) -> Before | After | Recoverable | Ambiguous
execute_replace(expected, goal)
execute_remove(expected)
```

`observe_durable` obtains the canonical parent durability barrier before it
returns an ordinary leaf fact and revalidates the retained parent and leaf
after the barrier. It is named as a durability operation; callers must not
present it as a side-effect-free filesystem read. A pending action may advance
only from `After`, and `After` is unavailable until that barrier succeeds.

Existing-source replacement and removal use a deterministic, action-bound
quarantine below the workspace runtime directory (or below the retained Git
directory when that directory is itself the artifact root), never a sibling in
a managed worktree directory. The quarantine key binds:

- canonical artifact path;
- operation kind;
- expected bytes;
- replacement goal, if any; and
- retained parent identity.

The source leaf is opened no-follow and its identity and bytes are retained.
After the post-proof/pre-syscall hook, a handle-relative no-replace rename
moves the named leaf to quarantine. The moved file must have the retained
identity and bytes. A different-byte or same-byte/different-inode replacement
is restored with no-replace and rejected; it is never deleted or overwritten.
A parent move detected after detachment is likewise restored and rejected.
If safe restoration is blocked, both names are retained and the transition is
`Ambiguous` rather than destructive.

Replacement then publishes a flushed temporary with no-replace. Removal leaves
the managed leaf absent. The managed parent barrier is mandatory. Quarantine
cleanup and its parent barrier follow; a crash residue remains outside every
managed worktree classifier and is recognized by the same action on restart.
The transition classifier accepts only these closed forms:

| Quarantine | Managed leaf | Result |
| --- | --- | --- |
| absent | exact source | `Before` |
| absent | exact goal | barrier, then `After` |
| exact source | absent | `Recoverable` |
| exact source | exact goal | barrier/cleanup, then `Recoverable` or `After` |
| any other combination | any other form | `Ambiguous` |

For removal, exact goal means absence. For replacement from `Missing`, atomic
no-replace publication and the same barrier/restart rule apply without a
quarantined source.

The private quarantine namespace is covered by the retained workspace mutation
lease and the workspace boundary excludes it from root Git status. It is not
user content and is not part of root-preservation parent classification or the
published workspace. Foreign content there fails closed. Platform code owns
only handle-relative rename, no-replace,
write-through/barrier, and private cleanup primitives; lifecycle policy stays
above it.

### 3.2 Concurrency boundary for native stash

The workspace mutation lease serializes GWZ and cooperating Git mutations for
the complete service call. It cannot freeze an arbitrary editor or a raw
filesystem writer. That limitation is shared by native `git stash`: no
portable filesystem primitive atomically snapshots and resets an entire Git
index and worktree.

The contract is therefore explicit before activation:

- changes already visible before the checked native-mutation boundary are in
  scope and must reject without mutation if they differ from the persisted
  branch, HEAD, complete preimage, or stash set;
- the deterministic boundary hook is after every status, stash-list,
  signature, repository-state, and eligibility preparation and immediately
  before the single `git_stash_save_with_opts` call;
- GWZ/Git mutators that honor the workspace lease cannot cross that boundary;
- an arbitrary process modifying repository files while a mutating GWZ command
  is executing is outside the concurrency contract, exactly as for a raw
  concurrent `git stash`; and
- post-mutation verification remains mandatory and preserves the native stash
  evidence if an unsupported concurrent writer or backend failure is detected.

This is the contract review explicitly allowed as the alternative to claiming
an impossible portable worktree CAS. It narrows no user-visible safety promise:
the command already owns an exclusive mutating invocation, and documentation
must tell users not to edit or run raw mutating Git concurrently with it.

Implementation splits the legacy orchestration into preparation and one native
mutation. The checked seam never calls the legacy `stash_for_merge_preservation`
routine after its final proof. A stopped-after-intent HEAD/preimage/stash-set
change injected at the exact boundary must reject with byte-identical refs,
index, worktree, record, and native stash list.

### 3.3 Complete checkout authority

Add one backend fact:

```text
checkout_matches_commit_except(path, commit, allowed_paths) -> bool
```

It is built from the canonical preservation-image machinery, not porcelain
status. It:

1. rejects unresolved stages and assume-valid, skip-worktree,
   intent-to-add, or unknown semantic index flags;
2. flattens the expected commit tree;
3. compares every stage-zero index path, mode, and object ID;
4. reads and hashes every tracked worktree regular file, symlink, and gitlink;
5. includes every non-ignored untracked path; and
6. removes only the caller's canonical `allowed_paths` from both live and
   expected maps before exact comparison.

Branch attachment, HEAD, ref target, and native repository state remain
separate explicit caller checks. Ordinary participants pass an empty allowed
set. Selected root passes only the closed publication candidate path set.

The authority-owned selected-root prospective proof also calls one exported
read-only `selected_root_result_artifacts` verifier before `BeginRollback`.
That verifier reads manifest and lock blobs at the recorded result commit,
enforces their exact UTF-8/canonical requirements, and compares them with the
accepted metadata. The later root executor reuses the same verifier; it does
not maintain a second interpretation.

### 3.4 Generated C7 service matrix

The matrix source is the closed phase/action vocabulary plus a legal-form
table, never actions observed from one favoured fixture. Each legal row names:

- root owner encoding (`@root` participant or publication root);
- publication handoff form (`Baseline`, `Marker`, `Lock`, `Boundary`) and legal
  pre/staged index form, including absence where legal;
- incoming request (`Preserve` or `Abort`) where legal;
- pending preservation phase or rollback action;
- physical observation (`Before`, `After`, `Ambiguous`);
- interruption boundary (pre-mutation, post-mutation/pre-barrier,
  pre-barrier, barrier error, post-barrier, post-successor); and
- expected terminal result, exact action count, and exact retained bytes.

Coverage assertions fail when a phase/action/form enum gains a value without a
matrix row. Preservation includes all 22 root phase successors, both handoff
owners, the stopped-after-intent HEAD/preimage rows, and canonical
`mem_z`/`mem_a`/`@root` bundle bytes. Rollback includes evidence steps,
`AbortConflict`, `ResetIntegrated`, selected-root metadata steps, both legal
incoming requests, ambiguous facts, and restart to terminal exhaustion rather
than another `RecoveryRequired` pause.

## 4. Implementation sequence

### R1 — freeze module boundaries and fault vocabulary

1. Split `checked_artifact.rs` by responsibility before adding semantics:
   `mod.rs`, `observation.rs`, `transition.rs`, `platform.rs`, `fault.rs`, and
   focused `tests/` owners. Each child targets below 500 lines.
2. Add explicit post-proof/pre-syscall, post-detach, post-publication,
   pre-parent-barrier, parent-barrier-error, post-parent-barrier, and
   post-quarantine-cleanup boundaries.
3. Freeze quarantine naming/classification and the stash concurrency contract
   in code comments and the controlling interface documents.

Stop for design review if this requires a new wire action/phase, cannot keep a
foreign source recoverable, or lacks a platform implementation for a supported
release target.

### R2 — implement D1 and prove it through real consumers

1. Implement the quarantine-backed replacement/removal transition and durable
   observation.
2. Move root-preservation files, preservation bundles, evidence rollback, and
   selected-root rollback to transition-aware classification/execution.
3. Test different-byte replacement, same-byte/different-inode replacement,
   parent movement, foreign quarantine, each durability boundary, and restart
   from every closed residue for replacement and removal.
4. Cross those cases through bundle, publication rollback, selected-root
   rollback, and root-preservation service consumers.

### R3 — implement D2 under the explicit concurrency contract

1. Split native stash preparation from mutation.
2. Run all read-only/native preparation before the complete preimage and final
   attached-HEAD proof.
3. Add the deterministic exact-boundary hook and invoke native stash creation
   directly, once.
4. Add stopped-after-intent changes to HEAD, staged, unstaged, untracked, and
   matching/foreign stash sets. Each in-contract row rejects before mutation.
5. Retain exact post-mutation verification and a typed recovery diagnostic for
   unsupported concurrent mutation/backend failure.

### R4 — implement D3 before any rollback-entry mutation

1. Extract canonical checkout comparison beside `preservation_image` and add
   the backend contract/delegation.
2. Replace both ordinary-participant and selected-root status-based clean
   proofs.
3. Export and reuse the exact selected-root result-artifact verifier from the
   authority-owned preflight.
4. Add service-entry negative rows for semantic flags, staged/unstaged/
   untracked/rename/type-change/unresolved/native drift, result blob missing,
   non-UTF-8, and accepted-metadata mismatch with selected root plus another
   participant.
5. Assert no `BeginRollback` and byte-identical record, refs, index, worktree,
   stash/bundle, and workspace artifacts for every rejected row.

### R5 — implement D4 and retained C3/C6 evidence

1. Generate the legal preservation and rollback cross-products in separate,
   concept-owned test modules.
2. Require terminal exhaustion, not merely a second recovery response.
3. Assert complete enum/action coverage, exact physical call counts, exact
   successor bytes, and no repeated mutation.
4. Add the root-inclusive canonical bundle fixture and stopped-after-intent
   stash HEAD/preimage rows.

### R6 — local gates and settled checkpoint

Run:

- all new focused regression matrices;
- all preservation, rollback, reverse-lifecycle, and G15 tests;
- `cargo test --all-targets --no-fail-fast`;
- `cargo fmt --check`;
- `cargo clippy --workspace --all-targets -- -D warnings`;
- protocol regeneration checks;
- merge documentation consistency and checker unit tests;
- compatibility-predicate validation and unit tests; and
- `git diff --check`.

Use installed `gwz` for workspace status, staging, and the one settled
remediation commit. Record exact workspace and `gwz-core` commit IDs.

The exact-tree Windows, macOS, Linux x86, and Linux ARM64 release builds remain
the platform evidence. The local macOS host's missing Windows SDK is recorded
as a limitation, not converted into a weaker cross-compile requirement.

### R7 — independent acceptance gate

Return the exact settled workspace/core commit pair and both first re-reviews
to two independent reviewers. R4b-G resumes only when both report no open
P0/P1/P2 defect. Any finding is folded into one reviewed remediation design;
no ad hoc patch chain is accepted.

## 5. Path and cohesion manifest

Expected production owners:

- `gwz-core/src/checked_artifact/{mod,observation,transition,platform,fault}.rs`;
- `gwz-core/src/git/gitbackend/{contract,preservation,preservation_image}.rs`;
- existing bundle/root-preservation/evidence/root-rollback consumers; and
- existing abort preflight and participant observers.

Expected test owners:

- `gwz-core/src/checked_artifact/tests/` split by observation, exact-source,
  durability, and recovery;
- G15 checked-stash and root-preservation boundary owners;
- reverse-preservation root physical/successor/form matrices; and
- reverse-rollback entry, service-fault, and root-artifact matrices.

The 1,000-line threshold is a cohesion review trigger, not a hard rule. A file
that is becoming a concept dump is split earlier; when a split is warranted,
children target below 500 lines. This manifest is amended before introducing a
new responsibility owner.

## 6. Exit criteria

The second remediation is accepted only when:

1. no visible goal authorizes a durable successor without a successful
   canonical-parent barrier, including restart after every injected boundary;
2. no foreign source inode or moved parent is overwritten or removed by a
   checked-artifact action;
3. Windows removal has no unmanaged sibling tombstone and exact-tree platform
   evidence covers the implementation;
4. every in-contract stash preimage/HEAD/stash-set change at the exact boundary
   rejects before native mutation;
5. rollback entry uses complete checkout and exact selected-root result blobs
   before `BeginRollback`;
6. the generated C7 matrix proves every legal physical/durable/form/request
   row to terminal exhaustion;
7. all local/static/document/platform gates pass on one settled tree; and
8. two independent re-reviews report no open P0/P1/P2 defect.

Only then does step 4 of the controlling six-step sequence—R4b-G—begin.
