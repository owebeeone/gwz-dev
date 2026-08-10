# R4b-P0.2 interface amendment review — state, durability, and restart

Date: 2026-08-11

Reviewer: independent state-machine, restart, and recovery review

## Verdict

**HOLD.**

Flattening the root work into one durable phase per independently committed
marker, lock, and index mutation is the correct cursor model. The pending phase
already is the durable cursor; another `root_cursor` would duplicate authority
and create disagreement states. Keeping `.git/info/exclude` at the exact durable
handoff also avoids a fourth mutation boundary. The selected-`@root` collision
rule remains correct, and an interrupted checked ref reset may remain one
journaled action if every mixed result is rejected as ambiguous.

The proposed interface is not restart-closed yet. Exact `Before` and `After`
overlap for permitted no-op rows, the clean form is not demonstrably the form
of the attached commit in all accepted histories, and the durable preimage is
not explicitly rechecked at the final pre-stash boundary. The Git-neutral index
fact is also undefined, and the provisional manifests and budgets omit
mechanically required owners. There are no P0 findings because P0.2 and P1
remain paused. There are five P1 findings and two P2 findings.

This is an interface review, not a settled implementation review. I changed no
production code and claim no test result.

## Review basis

I traced the amendment against P0.1, the reverse-lifecycle checkpoint, the
transition design, and the current implementation. In particular I inspected:

- the durable phase enums and preservation payload in `model/v1/journal.rs`;
- the phase validity and selected-root collision rules in
  `model/v1/validate/preservation.rs`;
- exact adjacency in `v1_lifecycle/transition/reduce/preservation.rs`;
- preservation authority payloads, resolver, and transition footprint;
- the existing preservation image, native stash, and checked reset backend;
- exact YAML, validation, unknown-field identity, and lifecycle fixtures; and
- the controlling R4b-P ownership and budget tables.

The current tree still contains the compound `NormalizeRoot`/`RestoreRoot`
vocabulary. P1 observation and execution remain deliberately unimplemented, so
this review is evaluating whether the new seam is complete enough to implement.

## P0 findings

None.

## P1 findings

### P1-1 — allowed no-op rows satisfy both `Before` and `After`

Section 3 says an object already at its goal advances without a physical call.
Section 8 accordingly requires an exact `After` proof for a no-op. The generic
classifier in §5.4, however, defines `Before` by equality with the phase source
and `After` by equality with the goal. In several allowed handoffs the source
and goal for marker, lock, or index are byte-identical. The same whole-root
state then satisfies both predicates.

This is common rather than theoretical. `Baseline/Pre` can equal the
baseline/pre clean form for all three normalize steps; a composition/staged
checkout can equal the candidate/staged handoff; and the degenerate
`Marker/Staged` row can make individual marker, lock, or boundary facts equal.
An implementation that checks `Before` first can perform a physical mutation
where the contract requires `AlreadyComplete`, while a recovery implementation
that checks in the opposite order reports a different observation for the same
durable bytes.

Required correction: freeze disjoint classification. Evaluate the complete
`After` predicate first. `Before` is legal only when the complete `After`
predicate is false and the named source fact differs from its goal. A
source-equals-goal row must be `After`, never `Before`. The checked executor
must consume the same classifier rather than reproduce this precedence.

Concrete replacement wording:

> For a named object, first evaluate the complete `After` predicate. If it is
> true, return `After`, including when source and goal are equal. Otherwise,
> evaluate `Before` only when the named source fact is not equal to the named
> goal fact and the complete source/prior/later predicate is true. Return
> `Ambiguous` for every remaining physical form. The checked executor calls
> this exact classifier immediately before mutation; it never reimplements the
> predicates.

The amendment should also freeze the full phase table below. Let `H` be the
exact durable handoff managed form, `C0` the exact clean form of the currently
attached expected commit, and `C1` the exact clean form of the restore commit.
Every row additionally requires the unchanged handoff boundary, exact branch/
commit, clean native sequencer state, and the row-specific unrelated-work
guard.

| Durable phase | Exact before | Exact after |
| --- | --- | --- |
| `NormalizeMarker` | marker `H`; lock/index `H` | marker `C0`; lock/index `H` |
| `NormalizeLock` | marker `C0`; lock/index `H` | marker/lock `C0`; index `H` |
| `NormalizeIndex` | marker/lock `C0`; index `H` | marker/lock/index `C0` |
| `CreateStash` | all managed `C0`; no matching stash; normalized image equals durable hash | all managed `C0`; one exact stash whose decoded image equals the durable image; no unrelated live dirt |
| `RestoreIndex` | all managed `C0`; exact stash evidence | index `H`; marker/lock `C0`; exact stash evidence |
| `RestoreLock` | index `H`; marker/lock `C0` | index/lock `H`; marker `C0` |
| `RestoreMarker` | index/lock `H`; marker `C0` | all managed `H` |
| `WriteBundle` | all managed `H`; exact native stash; absent or exact earlier canonical bundle prefix | all managed `H`; exact canonical bundle including the current row |
| stash `Complete` | exact handoff, stash, and bundle evidence | proof-only terminal state; no physical action |
| `PrepareMarker` | marker `H`; lock/index `H` | marker `C0`; lock/index `H` |
| `PrepareLock` | marker `C0`; lock/index `H` | marker/lock `C0`; index `H` |
| `PrepareIndex` | marker/lock `C0`; index `H` | marker/lock/index `C0` |
| `ResetRef` | attached expected branch/commit; all managed `C0` | attached restore branch/commit; all managed `C1` |
| reset `RestoreIndex` | all managed `C1` | index `H`; marker/lock `C1` |
| reset `RestoreLock` | index `H`; marker/lock `C1` | index/lock `H`; marker `C1` |
| reset `RestoreMarker` | index/lock `H`; marker `C1` | all managed `H` |
| reset `Complete` | attached restore branch/commit and exact handoff form | proof-only terminal state; no physical action |

The no-candidate/non-root graphs retain only `CreateStash -> WriteBundle ->
Complete` and `ResetRef -> Complete`; they do not acquire synthetic root facts.
An unexpected advance of a later object or any object outside the two exact row
forms is `Ambiguous`.

### P1-2 — clean-form provenance and descendant history are not closed

Section 4 says authority derives the clean form from the commit currently
attached to the root, but its table covers only no composition, the exact
composition commit attached, and the post-reset restore commit. The accepted
two-pass cursor explicitly permits a live descendant commit different from the
immutable anchor, records it in a backup ref, stashes its work, and later resets
to the anchor. That history has no clean-form row here.

The backend spec then accepts a caller-supplied `clean_form` but never requires
that marker, lock, and managed index entries equal the exact tree of
`attached_commit`. A valid branch/commit pair does not prove the supplied bytes.
Without this check, a stale or fabricated form can be normalized into the live
root and authorized by a proof that binds only the supplied value. Conversely,
blindly requiring baseline/pre or candidate/staged would reject a permitted
descendant whose committed managed paths differ from the anchor.

Required correction:

- define `C0` as the exact marker/lock tree projection of the currently
  attached commit, with the corresponding clean stage-0 index projection;
- define `C1` identically from the recorded restore commit;
- state that the baseline/pre and composition/staged rows are consequences for
  those exact recorded commits, not an exhaustive history table;
- have the backend independently verify each supplied clean form against the
  named commit before issuing `Before`, `After`, or a prepared image; and
- bind both commit ids and both exact physical forms into authority.

Concrete replacement wording:

> A supplied clean form is not authority by construction. Before preparation
> or step classification, the backend resolves the exact, unabbreviated commit
> id named by the spec and reads the marker and lock entries from that commit's
> tree. It derives the clean stage-0 marker/lock index subset from those exact
> tree entries, including exact absence, blob id, regular-file mode, stage, and
> accepted flags. The supplied `C0` form must equal that projection for
> `attached_commit`; the supplied `C1` form must equal the same projection for
> `restore_commit`. A missing commit, non-commit object, mismatch, unsupported
> mode, or noncanonical path is an error and issues no observation or proof.

If committed managed-path divergence on an allowed descendant is instead meant
to reject preservation, that restriction must be added to entry preflight and
the accepted descendant cursor must be narrowed explicitly. It cannot be left
as an implementation choice.

There is a related pre-journal state ambiguity. `prepare_root_preservation_stash`
has no durable phase, object, or goal input, yet §5.3 allows every managed fact
to equal either handoff or clean form “at the current durable phase.” Before
`BeginStash` there is no such phase. Preparation must require the complete exact
handoff, allowing only true byte-equal no-ops, and virtually project `C0`.
Partial mixtures are restart evidence for an unjournaled mutation and must
reject before `BeginStash`.

### P1-3 — the normalized preimage is not rechecked immediately before `CreateStash`

The amendment stores the virtual normalized-image hash at `BeginStash` and
requires re-projection before each normalize mutation. That closes changes
during `NormalizeMarker`, `NormalizeLock`, and `NormalizeIndex`, but the rule
ends before native stash creation. User work can change after
`NormalizeIndex` is durably advanced and before `CreateStash` executes.

The native stash would then preserve an image different from the durable hash.
Post-stash decoding may detect the mismatch, but only after an unauthorized
physical mutation has occurred and the record has become unadvanceable. Exact
post-state checking is not a substitute for pre-mutation authority.

Required correction: the `CreateStash` observer and checked executor must
reproject the complete normalized image immediately before mutation and require
the exact durable `preimage_sha256`. The same check must bind the exact
handoff boundary because `.git/info/exclude` controls whether raw paths are
ignored or untracked. The boundary is not mutated or stashed, but it is consumed
by both status/image classification and native stash selection.

Evidence must change staged, unstaged, untracked, ignored, and raw non-UTF-8
unrelated paths after durable `NormalizeIndex` and before `CreateStash`; every
changed included image must reject without a stash. It must also use a path
whose ignored/untracked classification differs between baseline and candidate
boundary bytes, prove that the durable handoff boundary controls membership,
and reject a boundary change at every partial phase before the next mutation.

### P1-4 — the Git-neutral physical index and handoff forms are undefined

`GitRootManagedForm.index` uses an undefined `GitRootPublicationIndex`, and
`GitRootPublicationHandoff` is said to contain “existing” prefix/index enums.
Those enums are v1 model policy, not Git-layer physical facts. Importing them
into `git` reverses the dependency; copying them creates a second semantic
mapping that can disagree after restart.

The backend needs two explicit Git-neutral physical forms, not a logical
publication handoff: `clean_form` and `handoff_form`. The managed index form
must state exact raw marker/lock path bytes, stage, object id, mode, accepted
semantic flags, and exact absence where required. One authority-owned
projection converts the validated durable handoff to `handoff_form`; Git only
validates canonical physical facts. Proofs bind both forms.

This also closes exact before/after derivation for opposite index forms. A
semantically different `Pre`/`Staged` index cannot be accepted merely because
the worktree leaves happen to match.

### P1-5 — the manifest, wire closure, and ledgers are not restart-auditable

The current lists are expressly provisional even though GO requires exact
mechanical inventory. The phase scan finds two omitted evidence owners:

- `model/v1/validate/tests.rs` matches removed `RestoreRoot`; and
- `v1_lifecycle/tests/fixtures.rs` matches removed `NormalizeRoot`.

The exact YAML owner must accept every new snake-case spelling and reject
`normalize_root`/`restore_root`. Validation must close phase/ids/handoff
requirements for every new row. The unknown-field fixture literally embeds
`phase: normalize_root`; although phase is intentionally excluded from the
pending-action unknown identity, the fixture must use a valid current shape and
prove that phase-only progress preserves unknown descendants while an owner,
kind, or handoff identity change retires them. Constructor helpers must produce
both root graphs and the no-root graphs. `prefixed_preservation.rs`, not the
generic `reducer.rs`, is the current exact graph owner.

The 1,200-line evidence ceiling is impossible to audit: the amendment itself is
458 lines, two reviews and control-document reconciliation consume additional
evidence lines, and the required real-Git/fault matrix cannot fit the remaining
space in one sub-500-line file. The controlling reverse-lifecycle and budget
tables still contain only the P0.1 8,250/15,500 and 136/120 package ceilings.
P0.2 and its shared P1 charge are absent.

The mechanically defensible replacement manifests and ceilings are recorded
below. A different split requires a fresh inventory before any edit.

## P2 findings

### P2-1 — `GitPreparedRootStash` duplicates dirty authority

`GitPreservationImage` already contains `dirty`. Returning a second
`user_dirty` value permits disagreement without defining which copy is
authoritative. Return only the normalized image, or freeze and enforce literal
equality. The single-value form is smaller and sufficient.

### P2-2 — reset interruption evidence needs a concrete fault boundary

The reset rule itself is sound: exact expected-ref/`C0` is before, exact
restore-ref/`C1` is after, and any checkout/ref mixture is ambiguous. The
required evidence nevertheless says “fault injection” without naming a seam in
the existing checked reset. Before implementation, the test owner must expose a
test-only interruption point between checkout work and the checked ref
transaction, or enumerate reproducible repository states for every reachable
partial outcome and prove none is classified `After`.

This does not justify splitting `ResetRef` into speculative subphases. It is an
evidence requirement for the deliberately compound existing primitive.

## P3 findings

None.

## Accepted state boundaries

Subject to the findings above:

- the flattened phase enum is the sole durable root cursor;
- reducers own adjacency, while Git observes only a named object and explicit
  physical source/goal forms;
- the durable boundary remains byte-exact and immutable through handoff,
  normalization, stash, reset, restoration, pass boundaries, and exhaustion;
- both `PublicationRoot` and selected participant `@root` are root owners, and
  the current validator correctly rejects a duplicate `PublicationRoot` when
  `@root` is selected;
- `NoCandidate` and `EvidencePending` correctly use the simple physical graph;
- re-projecting one normalized image through every partial normalization is the
  right protection once `CreateStash` is included in the guard; and
- `ResetRef` may remain one checked action because interruption never fabricates
  completion or authorizes repair.

## Mechanically required replacement manifest

### Production — 10 paths

```text
gwz-core/src/git/gitbackend.rs
gwz-core/src/git/gitbackend/contract.rs
gwz-core/src/git/gitbackend/preservation.rs
gwz-core/src/git/gitbackend/preservation_image.rs
gwz-core/src/git/gitbackend/preservation_root.rs
gwz-core/src/git/gitbackend/preservation_root/files.rs
gwz-core/src/git/gitbackend/preservation_root/index.rs
gwz-core/src/workspace_ops/merge/model/v1/journal.rs
gwz-core/src/workspace_ops/merge/model/v1/validate/preservation.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/transition/reduce/preservation.rs
```

`preservation_image.rs` owns extraction of the existing raw image grammar and
virtual managed substitution. `preservation_root.rs` owns spec validation and
whole-step orchestration. Its subowners perform canonical leaf and raw-index
observation/mutation. No Git owner receives model phases or publication policy.

### Evidence/tool/document — 20 paths

```text
gwz-core/src/git/tests/g15.rs
gwz-core/src/git/tests/g15/root_preservation.rs
gwz-core/src/git/tests/g15/root_preservation/support.rs
gwz-core/src/git/tests/g15/root_preservation/observation.rs
gwz-core/src/git/tests/g15/root_preservation/mutation.rs
gwz-core/src/git/tests/g15/root_preservation/stash.rs
gwz-core/src/git/tests/g15/root_preservation/faults.rs
gwz-core/src/workspace_ops/merge/model/v1/tests.rs
gwz-core/src/workspace_ops/merge/model/v1/validate/preservation_tests.rs
gwz-core/src/workspace_ops/merge/model/v1/validate/tests.rs
gwz-core/src/workspace_ops/merge/record_wire/unknown_fields/tests.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/fixtures.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/prefixed_preservation.rs
dev-docs/GwzM5-8R4bInterfaceAmendment-2.md
dev-docs/GwzM5-8R4bInterfaceAmendment-2-ReviewState.md
dev-docs/GwzM5-8R4bInterfaceAmendment-2-ReviewArch.md
dev-docs/GwzM5-8R4bReverseLifecycleInterface.md
dev-docs/GwzM5-8R4bTransitionDesign.md
dev-docs/GwzM5-8ChangeBudget.md
dev-docs/GwzM5-8Refactor.md
```

Current generic footprint, resolver, journal-vocabulary, and reducer tests do
not match a removed phase and need no P0.2 edit. If implementation changes one,
it must be added to the reviewed manifest before editing.

## Credible stop ceilings

| Charge | Stop ceiling |
| --- | ---: |
| new/changed production | 2,100 lines |
| moved production | 450 lines |
| test/tool/document | 4,000 lines |
| production paths | 10 |
| evidence/tool/document paths | 20 |

These are stop ceilings, not targets. Each new production and Git evidence
cohesion owner remains below 500 lines. `contract.rs` should remain below 950,
the post-extraction `preservation.rs` below 850, and no existing owner may cross
1,000 without renewed cohesion review.

The ledger must charge shared Git paths/lines to P1 as well as P0.2, then
recompute P1, R4b-P charged and unique unions, and the program union. The present
documents do not contain enough attribution to derive honest aggregate totals;
inventing them here would not satisfy the mechanical gate.

## GO conditions

State/restart GO requires the controlling amendment and ledgers to:

1. make no-op classification disjoint and freeze the complete phase table;
2. derive and independently verify clean forms from exact current/restore
   commits, including the permitted descendant history;
3. restrict pre-journal preparation to the exact full handoff;
4. recheck the durable normalized image and invariant boundary immediately
   before native stash creation;
5. replace logical Git publication types with exact physical forms;
6. close YAML, validation, constructor, unknown-field, root-owner/collision,
   action-free cursor, and exhaustion evidence;
7. demonstrate reset interruption as before/after/ambiguous with no fabricated
   completion; and
8. freeze the exact manifests, credible ceilings, shared P1 attribution, and
   package/program unions.

Until those corrections are independently re-reviewed, P0.2 and P1 remain
paused.

## Re-review — 2026-08-11

### Verdict

**HOLD.** The revised amendment closes every prior semantic P1 and P2 finding,
but the reconciled evidence budget is already exceeded by its own listed
documents. One new P1 accounting finding remains. There is no P0, P2, or P3
finding in this re-review.

This re-review inspected the complete revised amendment and the reconciled
`GwzM5-8R4bReverseLifecycleInterface.md`,
`GwzM5-8R4bTransitionDesign.md`, `GwzM5-8ChangeBudget.md`, and
`GwzM5-8Refactor.md`. No implementation or controlling document was changed.

### Prior semantic findings closed

The revised interface now closes the state/restart defects from the initial
review:

- Amendment §3.4 freezes every stash and reset phase as an exact `H`, `C0`, or
  `C1` before/after row. It evaluates the complete `After` predicate first and
  permits `Before` only when the named source differs from its goal. A
  source-equals-goal row is therefore only `After` and cannot authorize a
  physical write.
- Amendment §4 defines `C0` from the exact currently attached commit and `C1`
  from the exact restore commit, expressly including a permitted descendant
  with committed managed-path differences. The backend independently resolves
  both exact commit objects, derives their tree/index projections, and rejects
  a supplied mismatch before observation or proof.
- Amendment §5.3 requires pre-journal preparation to start from complete exact
  `H`; an unjournaled partial `H`/`C0` mixture rejects. It also reprojects the
  complete normalized image under the exact invariant `H` boundary both after
  durable `NormalizeIndex` and immediately inside the checked `CreateStash`
  execution call.
- Amendments §§5.4 and 6 retain `ResetRef` as one honest checked action while
  classifying only exact expected-ref/`C0` as before and exact
  restore-ref/`C1` as after. Every mixed checkout/ref/index/worktree result is
  ambiguous, with a required concrete internal fault boundary or exhaustive
  reproducible partial-state evidence and no automatic retry mutation.
- Amendment §5.1 replaces model publication vocabulary in Git with explicit
  physical worktree and raw index facts. It freezes path, complete object id,
  mode, stage, accepted flags, exact absence, one marker identity, and
  rejection of unknown flags/extensions. Section 5.4 correctly preserves
  unrelated semantic entry tuples rather than claiming serialized index-file
  identity.
- Amendment §9 and the exact six-path model/transition evidence allocation
  close current phase YAML, validation, constructor, unknown-field identity,
  root-owner collision, action-free cursor/exhaustion, fixture, and adjacency
  fallout. The generic resolver, footprint, journal-vocabulary, and reducer
  tests do not match a retired phase and remain outside the manifest.
- The ten production and twenty evidence paths are exactly enumerated. The
  shared-Git, P1 duplicate-charge, charged-path, unique-path, and program-union
  arithmetic is otherwise correct: `10 = 7 + 3`, `20 = 7 + 6 + 7`,
  `12,000 = 8,250 + 2,100 + 1,650`, and
  `21,900 = 15,500 + 4,000 + 2,400`. The 150/146 charged and 97/102 unique
  package paths, and 141/145 program paths, follow from the stated overlaps.
  The reverse-lifecycle, transition, budget, and refactor controls consistently
  keep P0.2/P1 paused and make amendment 2 controlling over the historical
  compound-phase text.

### P1-R1 — the 800-line document row is already overrun

The P0.2 allocation reserves only 800 test/tool/document lines for all seven
documents and reviews. The three P0.2 documents alone currently contain:

| Evidence document | Current lines before this re-review append |
| --- | ---: |
| `GwzM5-8R4bInterfaceAmendment-2.md` | 687 |
| `GwzM5-8R4bInterfaceAmendment-2-ReviewState.md` | 381 |
| `GwzM5-8R4bInterfaceAmendment-2-ReviewArch.md` | 337 |
| **Subtotal** | **1,405** |

That subtotal excludes this dated re-review, the architecture re-review, and
the P0.2 reconciliation lines added to the four control documents. All seven
paths are expressly charged by amendment §10. Because the files are new P0.2
evidence, their original review text cannot be treated as a previously accepted
package charge. The 800-line component ceiling is therefore exceeded before
implementation starts.

The component sum is arithmetically `2,400 + 800 + 800 = 4,000`, but it is not
an auditable stop budget when one component has already consumed at least
1,405 lines. Silently borrowing from the Git evidence row would also invalidate
the reviewed 2,400-line reservation for the broad real-Git and fault matrix.

Required correction: replace the document/review/control row with a measured
ceiling of at least 1,800 lines and raise the P0.2 evidence total to 5,000 while
leaving the 2,400 shared-Git and 800 model/transition allocations intact. On
the current overlap rules this changes only these totals:

| Ledger value | Corrected ceiling |
| --- | ---: |
| P0.2 test/tool/document | 5,000 |
| R4b-P charged test/tool/document | 22,900 |
| program unique-once test/tool/document | 32,200 |

The 20 evidence paths, P1's 4,200 duplicate shared-Git test charge, production/
moved ceilings, and all charged/unique path counts remain unchanged. If the
lead chooses a different evidence allocation, it must publish the measured
seven-document charge and preserve credible Git/model test reservations; the
present 800/4,000 values cannot receive GO.

### Re-review gate

State/restart GO requires only P1-R1 to be corrected consistently in the
amendment, reverse-lifecycle ledger, change budget, and top-level R4b totals.
The semantic interface itself is accepted by this re-review. P0.2 and P1 remain
paused until the accounting correction and both independent verdicts are
settled.

## Final re-review — 2026-08-11

### Verdict

**GO.** P1-R1 is closed. No P0, P1, P2, or P3 finding remains at the
pre-implementation state/restart boundary.

The settled amendment and both controlling ledgers now reserve 1,800 lines for
the seven document/review/control paths and 5,000 total P0.2 evidence lines.
Measured against the pre-P0.2 control-document lengths, the seven settled paths
consume 1,751/1,800 lines before this two-line measurement note.
The component and overlap arithmetic is exact:

- `5,000 = 2,400 shared Git + 800 model/transition + 1,800 documents`;
- R4b-P charged evidence is `22,900` after adding the 5,000 P0.2 charge and
  P1's separate 2,400 shared-Git charge to the accepted 15,500 baseline; and
- the unique-once program evidence ceiling is `32,200`, with shared Git counted
  once.

The 20 evidence paths, 10 production paths, 12,000/2,950 production/movement
ceilings, 150/146 charged paths, 97/102 P-package unique paths, and 141/145
program paths remain reconciled. No stale current 4,000, 21,900, or 31,200
P0.2 total remains in the settled controls. The settled controls now authorize
P0.2 implementation; P1 awaits both P0.2 code re-reviews.

This GO accepts the corrected interface and accounting for implementation. It
does not accept unimplemented code, activate v1, or replace the required
settled-tree implementation re-review and test gates.

## P0.2 settled-code state/restart review — 2026-08-11

### Verdict

**NON-GO.** P0.2 has four P1 findings. There is no P0, P2, or P3 finding.
P1 remains blocked until these findings are remediated and both independent
settled-code reviewers return GO.

This review inspected exactly the ten production and twenty
evidence/tool/document paths frozen by amendment §10. The exact-phase model,
validator, reducer, YAML, and unknown-field work is coherent: the new
snake-case phases are exhaustive in the frozen enum, the retired compound
spellings reject, ids/handoff/owner/root-collision rules follow the phase
graph, and the reducer advances only the literal root and non-root successors.
The Git classifier also gets the central shape right: it observes `After`
first, keeps `Before` disjoint when source equals goal, derives `C0`/`C1` from
the exact commits, requires exact full `H` before preparation, rechecks the
normalized preimage and boundary immediately before `CreateStash`, verifies
the native stash post-state, uses marker/lock/index forward and
index/lock/marker reverse order, and classifies the top-level reset states as
exact before, exact after, or ambiguous. It does not automatically repair an
ambiguous top-level state.

### P1-SR1 — managed-file mutation is still vulnerable to path-replacement races

`preservation_root/files.rs:38-64,67-77,80-105,137-150,160-197` performs
`symlink_metadata` parent/leaf checks and then opens, renames, removes, or
creates by pathname. A parent can be exchanged for a symlink after the final
check at line 103 and before `rename` at line 104; removal has the same
check/use gap. `create_dir_all` also follows a newly exchanged path. There is
no directory-handle-relative `openat`/`renameat`/`unlinkat` equivalent, nor a
closed retry/revalidation protocol that prevents mutation outside the root.
This does not satisfy amendment §5's canonical no-follow guarantee, and the
seven physical tests contain no parent/leaf replacement race at all.

Required remedy: make every observe/create/replace/remove operation relative
to verified directory handles with no-follow semantics (or prove an equivalent
race-closed primitive), then cover symlink, regular-file, directory,
executable, missing-parent, and parent/leaf replacement races without any
outside-root mutation.

### P1-SR2 — the promised index/object-format boundary is not implemented

`preservation_root/index.rs:100-154,185-215` validates only entry flags.
`validate_supported` has no observation or rejection of index extensions, yet
`rewrite` serializes through libgit2 and can therefore discard or reinterpret
an unsupported extension before the unrelated-entry post-check notices it.
The same owner accepts 64-hex ids in `validate_fact` at lines 218-235, while
commit/blob parsing and derivation use `git2::Oid` at lines 46-52, 87-97, and
157-181; the evidence constructs only SHA-1 repositories. Thus the stated
SHA-256 support is not demonstrated and is not a usable checked boundary.

Required remedy: inspect the raw index format before journaling and reject
every unsupported extension/flag without mutation, preserve and reverify the
complete unrelated canonical tuple promised by §5, and either implement real
SHA-256-repository support or narrow the accepted contract and all controls
before retrying review. Add real conflict/multiple-stage, assume-valid,
skip-worktree, intent-to-add, unknown-flag, extension, raw-path, SHA-1, and
SHA-256 cases.

### P1-SR3 — the required restart/fault evidence matrix is not closed

`preservation_root.rs:47-93,174-199,374-405` exposes faults only before and
after an entire physical step. It cannot interrupt between reset checkout work
and the checked ref transaction. `root_preservation/faults.rs:4-50` exercises
only those two outer boundaries for `NormalizeMarker`; `mutation.rs:34-76`
exercises only an uninterrupted reset. No test constructs a mixed
checkout/ref/index/worktree reset state and proves `Ambiguous`, no retry
mutation, and no automatic repair as amendment §6 requires.

More broadly, the seven Git tests do not close amendment §9's minimum matrix.
Missing cases include every handoff/form cross-product; fabricated, stale,
missing, non-commit, unsupported-mode, descendant, and second-marker clean
forms; each persisted row's before/after/restart whole-root state; every
degenerate no-op; skipped/reordered/opposite/mixed phases and foreign native
state; late staged, unstaged, ignored, and raw non-UTF-8 changes; a boundary
change that flips a raw path between ignored and untracked; marker/lock/index/
boundary edits at all partial phases; the forbidden-index and extension cases
above; every atomic leaf/index fault boundary; and action-free physical
cursor/exhaustion. The old helper test in `g15.rs` proves only byte sorting for
a synthetic raw path, not a real non-UTF-8 preservation/restart flow.

Required remedy: add the complete real-Git matrix within the frozen evidence
owners, including an internal reset seam or every reproducible partial reset
state. Each case must assert the durable phase on both sides, the complete
root, typed classification, and mutation/no-mutation result; happy-path-only
coverage is insufficient.

### P1-SR4 — settled accounting is arithmetically right but not reconciled in every review control

The current amendment, reverse-lifecycle interface, and change budget agree on
the revised ceilings: `6,400 = 2,400 + 800 + 3,200` for P0.2 evidence;
`24,300 = 15,500 + 6,400 + 2,400` for the charged R4b-P evidence program; and
`33,600 = 27,200 + 6,400` unique-once program evidence. The P0.2
2,100/450 production/movement charge, P1 3,000/1,500/4,200 duplicate shared
charge, R4b-P 12,000/2,950 totals, 150/146 charged paths, 97/102 P-package
unique paths, and 141/145 program paths also reconcile. The 3,200 document row
properly leaves review/remediation capacity beyond the 1,753-line accepted
interface set.

However, `GwzM5-8R4bInterfaceAmendment-2-ReviewArch.md:32,77-92` still calls
5,000/22,900/32,200 current, and its later budget snapshot still records 4,000.
The immediately preceding state-review GO likewise records the superseded
1,800/5,000/22,900/32,200 values. This appended review supersedes that state
tail, but it cannot reconcile the independent architecture memo.

Required remedy: correct the architecture review's current accounting and
append a measured charge for both settled-code reviews/remediation under the
3,200 document ceiling. Do not borrow that reserve for production or tests.

### Gates and re-review boundary

The focused physical suite passed 7/7. `cargo test -p gwz-core` passed 1007
unit tests with one ignored, then 10, 28, 4, and 2 integration tests;
`cargo fmt --all -- --check` and strict all-target/all-feature core Clippy also
passed. The core run includes the retained v0 characterization and protocol
tests, and no inspected P0.2 path activates the disabled v1 runtime boundary.
This review did not rerun the external retained-reader platform matrix.

Re-review requires source remedies for P1-SR1 and P1-SR2, the complete
state/restart evidence required by P1-SR3, and exact control reconciliation for
P1-SR4. The same strict formatting, Clippy, focused, full-core, protocol, and
retained-reader gates must then pass on the final settled tree. P1 may not
resume merely because the present incomplete suite is green.

## Corrective-interface state/restart re-review — 2026-08-11

### Verdict

**GO.** No P0, P1, P2, or P3 finding remains in amendment 2 §12 or the
reconciled controls. This verdict authorizes only the bounded P0.2 corrective
implementation. It does not clear the settled-code NON-GO above, activate v1,
or authorize P1 before both corrected-code reviewers return GO.

### Restart and filesystem boundary

The durable graph and the exact `Before`/`After`/`Ambiguous` predicates remain
unchanged. The corrective boundary adds no compound mutation or hidden repair.
Its reset evidence constructs both checkout/index-versus-ref orderings and
separately mixed index/worktree states, requires `Ambiguous`, rejects checked
execution, and compares ref/index/worktree bytes after rejection. That is an
adequate directly constructed substitute for an internal reset crash hook and
proves no automatic adoption or repair.

The capability design closes P1-SR1 without inventing a parent phase. Every
managed parent is opened no-follow, identity-checked, and retained through the
operation; observation and mutation are relative to that handle. Missing,
replaced, symlinked, or non-directory parents are ambiguity and never cause an
unjournaled `mkdir`. Exact source re-observation through the anchored parent is
the pre-mutation gate, and handle-relative rename/unlink is the declared
linearization point. A post-observation adversarial leaf exchange is expressly
ordered around that operation rather than falsely promised destination-CAS
semantics. Outside-root sentinels and pre-linearization replacement injection
make containment and no-mutation rejection testable. The SHA-1/SHA-256
real-Git rows must also prove native stash leaves the already-existing marker
parent available, closing the no-create liveness concern.

### Object-format and raw-index boundary

The selected `git2` 0.21 feature exposes the named repository-format-aware
parse/hash APIs, and the contract now requires exact lowercase 40/64-character
facts plus complete preparation, managed-index, stash, and reset execution in
real SHA-1 and SHA-256 repositories. Unsupported or cross-format facts reject
before mutation.

The raw-index owner now has a complete authority boundary: `DIRC` versions
2–4, format-dependent OID widths, entry flags/stages, extension framing, and
full SHA-1/SHA-256 trailer recomputation and equality are checked before proof
and after rewrite. `TREE` is accepted only as a parsed cache. After an actual
managed rewrite, the root and every cache-tree ancestor of either managed path
must be invalidated with no cached OID; unaffected nodes may remain. This
matches Git's requirement that an updated path be removed from the valid tree
cache and avoids treating cache payload as preservation authority. Every named
unsupported, lowercase-mandatory, unknown, malformed, checksum-invalid, or
unsupported-version index rejects before mutation, while unrelated canonical
entry tuples remain exact.

### Evidence, manifest, and accounting

Section 12.4 now operationalizes all single-repository §9 rows: owner/handoff
and `C0`/`C1` forms, partial/fabricated commit evidence, every directional and
no-op phase, wrong ordering/ref/native states, every late user/boundary class,
raw non-UTF-8 paths, both object formats, index versions/flags/extensions,
filesystem shapes and races, every leaf/index fault boundary, complete-root
restart classification, cursor/exhaustion, and the direct reset partials. The
multi-owner/global-preflight row remains explicitly owned by P1 integration;
it is not inferred from backend tests and P1 remains paused, so there is no
false P0.2 completion claim.

The corrective manifest is exactly 12 production paths and 22 evidence paths:
the accepted 10/20 plus `gwz-core/Cargo.toml`,
`preservation_root/index_format.rs`, and the workspace/member lockfiles. The
charged and unique arithmetic reconciles:

- P0.2: 2,100/450/6,700 with 12/22 paths;
- duplicate shared P1: 3,000/1,500/4,500 with 15/16 paths;
- R4b-P: 12,000/2,950/24,900 with 154/150 charged and 99/104 unique paths; and
- program unique-once: 18,250/33,900 with 143/147 paths.

The 300-line evidence increase is charged in P0.2 and duplicate P1 accounting
but once in the program union. The 540 production and 1,900 test/tool
corrective estimates fit the retained component ceilings; the 350-line
`index_format.rs` cap is credible for the now-explicit checksum/cache parser.
The corrective architecture appendix supersedes its historical earlier
figures with the same controlling totals. Any extra owner, unsupported
dependency/version, line/path overrun, or inability to execute a required
matrix row stops implementation for recount and review.

P0.2 remediation may proceed. Re-review still requires the complete focused
matrix, strict formatting and Clippy, full core and protocol parity, retained
v0/disabled-v1 behavior, and retained-reader gates on the final settled tree.

## Durable parent corrective re-review — 2026-08-11

### Verdict

**NON-GO.** The revised parent phase closes the state-machine and restart
ambiguities, but its supported-Windows durability contract has one P1 gap.
There is no P0, P2, or P3 finding. P0.2 implementation remains paused; this
appendix supersedes the preceding corrective-interface GO only for the newly
added durable-parent seam and does not alter the settled-code findings above.

### Accepted state and restart closure

`RestoreParent` is correctly adjacent between `RestoreLock` and
`RestoreMarker` in both root-bearing graphs, while both no-root graphs remain
unchanged and reject that phase. The parent requirement is derived from the
selected marker form rather than caller input. All four clean/handoff marker
presence pairs are now exact: an absent handoff marker admits missing and
real-empty parents as one optional `After` form, including the one expected
later clean marker leaf; a present handoff marker requires a real canonical
parent. `After` is evaluated first, and optional-parent equality remains
proof-only.

The required-parent exception also closes the retry hole created by structural
`After`: the checked executor must run both persistence barriers before the
durable edge whether it created the parent, adopted an exactly empty concurrent
creation, or observed an already-present required parent. Thus interruption
before creation remains `Before`; interruption after creation, after the child
barrier, or after the parent barrier retries both barriers from structural
`After`. After the durable edge, a missing parent is `Ambiguous` and
`RestoreMarker` cannot repair it. Symlinks, non-directories, inaccessible
parents, temporary residue, second markers, foreign children, wrong-phase
marker advances, and pre-linearization parent/leaf replacement all reject.
Only `RestoreParent` may perform the one-level anchored creation.

The YAML enum, validator/constructor, unknown identity, reducer adjacency,
cursor/exhaustion, and root/no-root closure are expressly in scope. Section
12.4 also names every `RestoreParent` row and no-op, all four marker-presence
pairs, parent shapes and races, faults on both sides of creation and each
barrier, complete-root restart classification, and action-free exhaustion.
The revised 13/22 P0.2 manifest, component allocations, duplicate P1 charge,
R4b-P totals, and unique-once program totals reconcile with the current
controls.

### P1-DP1 — Windows parent creation has no persistence barrier

Amendment §5.4 requires a newly created directory entry to cross child and
parent persistence barriers, and requires every required-parent structural
`After` retry to repeat both. Section 12.1 says Windows satisfies this through
the workspace's existing `durable_fs` directory policy while retaining Windows
as a supported release/evidence platform. That policy does not provide the
declared operation. `gwz-core/src/durable_fs.rs:24-35` implements Windows
`sync_dir` as unconditional `Ok(())`. Its comment instead attributes Windows
persistence to `MOVEFILE_WRITE_THROUGH`, which is used only by the rename path
at lines 147-175. `RestoreParent` performs handle-relative one-level directory
creation, not that write-through rename. Consequently the two promised calls
can both succeed without issuing any Windows persistence operation, and a
durable `RestoreParent` edge can be recorded without the physical parent entry
having crossed the contract's barrier.

Required remedy: define and implement a reviewed Windows protocol that makes
the new directory entry durable before the phase edge, with restart evidence
at every real barrier boundary. If Windows cannot support that contract, remove
it from this seam's supported platforms and reject the operation there, then
reconcile every platform, evidence, release, and compatibility control. Any new
primitive, dependency, owner, or production path must be added to the exact
manifest and all charged/unique budgets before implementation resumes; a
temporary-directory/rename workaround must also specify and classify its own
residue, race, and crash states rather than hiding a second mutation.

### Re-review boundary

State/restart GO now requires only P1-DP1 to be closed in the amendment and
reconciled controls. Re-review must confirm that required-parent structural
`After` invokes two effective platform barriers on every supported platform,
that faults around those barriers retain the exact retry behavior above, and
that the final manifest and ceilings include every affected path.

## Durable parent causal-barrier final re-review — 2026-08-11

### Verdict

**GO.** P1-DP1 is closed by the current Amendment 2 and reconciled controls.
No P0, P1, P2, or P3 finding remains in the durable-parent corrective
interface. This appendix supersedes the immediately preceding durable-parent
NON-GO, which remains history for the rejected direct-create protocol.

This verdict authorizes only the bounded P0.2 corrective implementation. It
does not accept unimplemented code, clear the settled-code NON-GO above,
activate v1, or authorize P1 before both corrected-code reviewers and the
complete required evidence matrix return GO.

### Exact forms and graph closure

`RestoreParent` remains exactly between `RestoreLock` and `RestoreMarker` in
both root-bearing graphs. The no-root stash and reset graphs remain unchanged,
and validation rejects `RestoreParent` there. The phase owns only the marker
parent namespace; `RestoreMarker` remains the sole marker-leaf mutation.

All four clean/handoff marker-presence directions are now explicit and
disjoint:

- absent `H`, absent clean marker: missing and real-empty parents are ordinary
  proof-only `After`;
- absent `H`, present clean marker: the real canonical parent containing only
  that expected later `C0`/`C1` marker is ordinary proof-only `After`;
- present `H`, present clean marker: the established real parent is ordinary
  proof-only `After`, whether or not the marker bytes already match; and
- present `H`, absent clean marker: missing final with absent or one exact
  deterministic empty staging directory is `Before`; exact empty final with
  staging absent is `AfterNeedsDurability` on every platform.

The last direction is the sole causal refinement to goal-first classification.
Final plus staging, malformed/linked/non-empty/duplicate staging, a foreign
child, symlink, non-directory, wrong later marker advance, or any mixed root
fact is `Ambiguous`. After the durable edge, a missing parent is `Ambiguous`
and no later phase may recreate or repair it.

### Publication, barriers, races, and restart

Only `RestoreParent` may create the deterministic empty staging directory. It
does so relative to the retained no-follow `gwz.conf` capability, reuses only
the one exact empty staging form, and publishes by no-replace rename. Linux
uses descriptor-relative `renameat2(RENAME_NOREPLACE)` and macOS uses
descriptor-relative `renameatx_np(RENAME_EXCL)`; unsupported Unix targets
reject. Unix syncs staging before publication and the retained parent after
publication.

Windows pins the root and `gwz.conf`, opens the exact source directory
no-follow with delete access, backup semantics, open-reparse-point, and
write-through, and renames it relative to the pinned `gwz.conf` handle through
`SetFileInformationByHandle(FileRenameInfo)` with replacement disabled. Its
creation path publishes staging to final through that barrier. A final-only
causal retry performs the same handle-bound, write-through, no-replace
final-to-staging-to-final round trip. It neither calls the no-op Windows
`sync_dir` nor relies on a path-based rename.

The complete creation-needed namespace table is closed:

- final absent, staging absent: `Before`; create staging and publish;
- final absent, one exact staging directory: `Before`; publish it;
- exact empty final, staging absent: `AfterNeedsDurability`; Unix resyncs the
  parent and Windows performs the write-through round trip; and
- final plus staging: `Ambiguous`; neither name is cleaned or adopted.

Interruption during staging returns to either `Before` form. Unix interruption
after rename but before parent sync leaves final-only pending. Windows
interruption between the round-trip renames leaves staging-only `Before`;
interruption after publication leaves final-only pending. A crash after a
successful barrier but before record rewrite loses the in-memory attempt and
therefore repeats the barrier. A concurrent final appearing before staging is
re-barriered; one appearing after staging produces the both-name ambiguity.
No-replace collision, source exchange, parent replacement, and symlink races
cannot redirect mutation outside the retained parent.

### Causal lifecycle and model closure

`AfterNeedsDurability` maps to the non-durable, fully bound
`PreservationDurabilityPending` fact. With no attempt it selects the exact
persisted preservation action. Only a fresh matching `Success` plus a fresh
final-only pending goal may advance the stash/reset phase once. A matching
failure paired with fresh `Before`, pending, or `Ambiguous` physical binding
returns its exact typed diagnostic without transition or same-invocation
retry. This includes failure after the first Windows round-trip rename and a
second-rename collision. Success without the exact pending goal, and every
stale, mismatched, or second attempt, rejects.

The pending fact remains executor output only in conjunction with fresh
observation; it adds no durable field or wire shape. The named resolver,
observation, execution, reverse-preservation observer, and reverse executor
owners cover construction, dispatch, result matching, and consumption. The
existing service reload/observe loop needs no new owner.

The YAML vocabulary accepts exact `restore_parent` in both phase enums and
rejects retired compound spellings. Constructor and validator closure,
unknown-field identity, selected-root collision, exact reducer adjacency,
cursor prefix, action-free cursor, exhaustion, and root/no-root rejection are
all required. The controlling Transition Design and Reverse Lifecycle
Interface carry the same every-platform causal-pending rule.

### Evidence, manifest, and accounting

The corrective matrix covers all four marker directions, missing/empty parent
equivalence, both root owners, stash/reset phases and no-ops, exact/malformed/
duplicate/both-name staging, foreign contents, parent and leaf replacement,
outside-root containment, Unix exclusive rename and sync boundaries, Windows
staging publication and both round-trip rename boundaries, staging-only and
both-name failures, pending without attempt, every matching attempt result,
crash before rewrite, stale/mismatched/second attempts, complete-root restart,
and action-free exhaustion. The previously required SHA-1/SHA-256, raw-index,
reset-partial, protocol, retained-v0, and platform release evidence remains in
force.

The frozen manifest is exactly 18 production and 25 evidence paths: the
accepted 10/20, three shared Git/dependency owners, five lifecycle owners, two
lockfiles, and three existing lifecycle evidence owners. Platform publication
belongs in the already-listed `preservation_root/files.rs`; required Windows
features belong in `gwz-core/Cargo.toml`. No `durable_fs.rs`, service, model,
recovery, or new evidence owner is required.

The amendment, Change Budget, Reverse Lifecycle Interface, and Transition
Design reconcile exactly:

- P0.2: 2,650/450/8,200 with 18/25 production/evidence paths;
- duplicate shared P1: 3,700/1,500/6,000 with 19/19 charged paths;
- R4b-P: 13,250/2,950/27,900 with 164/156 charged and 103/104 unique paths;
  and
- program unique-once: 18,800/35,400 with unchanged 144/147 paths.

Any extra owner, unsupported platform/filesystem, inability to implement the
handle-bound primitives, or line/path ceiling breach stops remediation for a
new manifest and interface review. P0.2 remediation may now proceed within
this boundary.

## Direct-Unix primitive disclosure re-review — 2026-08-11

### Verdict

**GO.** The §12.1 `rustix` disclosure introduces no new state, restart, path,
or accounting finding and does not change the preceding causal-barrier GO.

The direct dependency is target-scoped to Unix at reviewed major version 1
and requests only the `fs` functional feature. Its
`renameat_with(..., RenameFlags::NOREPLACE)` accepts the retained capability
`Dir` as an `AsFd`; it therefore resolves both source and destination against
the already-open parent rather than reopening a pathname. On Linux the flag is
`RENAME_NOREPLACE`; on Apple it is `RENAME_EXCL`. The implementation is
compiled only for the supported Linux and macOS paths, while other Unix
targets return the already-specified unsupported-operation error.

This is an implementation primitive for the already-reviewed no-replace
linearization point. It does not add a physical mutation or cursor state. The
accepted table remains exactly: absent final/absent staging `Before`; absent
final/exact staging `Before`; exact empty final/staging absent
`AfterNeedsDurability`; both names `Ambiguous`. All interruption, attempt,
race, and later-phase predicates remain unchanged.

`gwz-core/Cargo.toml` is already one of the 18 production paths, and both
lockfiles are already among the 25 evidence paths. The dependency fits the
shared-Git allocation without adding an owner or changing the accepted
2,650/450/8,200 P0.2, R4b-P, or program-union ceilings.

## Owner-only manifest re-review — 2026-08-11

### Verdict

**GO.** The two focused owners are a cohesion split only. They introduce no
physical behavior, state/restart predicate, cursor, wire, unknown-field, or LOC
ceiling change and do not alter either preceding GO.

`preservation_root/parent.rs` receives the already-approved deterministic
staging, anchored parent classification, Unix publication/sync, and Windows
handle-bound publication/round-trip operations from the broader physical
owner. Whole-root classification and durable phase policy remain in
`preservation_root.rs`; managed leaves remain in `files.rs`. The accepted
A/B/C/D table, attempt binding, crash boundaries, race outcomes, and prohibition
on later repair are byte-for-byte the same contract.

`record_wire/unknown_fields/tests/preservation.rs` receives only the existing
preservation-specific unknown-field rows from the mixed 552-line test owner.
It adds no production decoder, field, phase spelling, constructor, or expected
wire result. Unknown-field byte identity and cursor/phase validation remain
unchanged; the split merely keeps both evidence owners below the frozen
cohesion ceiling.

The current manifest and ledgers reconcile. P0.2 becomes 19 production and 26
evidence paths; duplicate P1 becomes 20/19. Because `parent.rs` is charged in
both P0.2 and P1, R4b-P gains two production path charges, not one, and is
therefore 166/157 charged with 104/105 unique paths. The two new unique files
make the program union 145/148. Production, movement, and evidence ceilings
remain 2,650/450/8,200 for P0.2, 13,250/2,950/27,900 for R4b-P, and
18,800/35,400 unique-once program lines.

## Failure-diagnostic contract clarification re-review — 2026-08-11

### Verdict

**GO.** The §8 clarification matches the existing resolver/service boundary
and changes no state, restart, wire, or protocol contract.

A bound failed execution attempt retains the complete internal
`ExecutionDiagnostic::Failed { code, message, detail }`, so optional `detail`
continues to participate in exact attempt identity and stale/mismatched/second-
attempt rejection. The returned `ModelError` projects only the existing exact
`ErrorCode` and message. It has no `detail` field, and the resolver neither
concatenates internal detail into the message nor fabricates a new public
field.

The transition result remains identical: a fresh matching failure over
`Before`, durability-pending, or `Ambiguous` physical state returns that error,
performs no record transition, and permits no same-invocation retry. A later
invocation has no retained in-memory attempt and reclassifies the same durable
filesystem state under the accepted A/B/C/D machine. Success authority,
crash-before-rewrite repetition, cursor identity, and all ambiguity barriers
remain unchanged.

## §13 pre-clean parent state/restart review — 2026-08-11

### Verdict

**GO.** No P0, P1, P2, or P3 finding remains in §13 or its reconciled
Transition Design, Reverse Lifecycle, Change Budget, and Refactor controls.
This verdict accepts only the two pre-clean parent phases and their revised
ceilings. It does not accept implementation, activate v1, clear any earlier
settled-code finding, or release P1 before the remaining review gates pass.

### Vocabulary, initial states, and adjacency

The durable vocabularies add exactly `NormalizeParent`/`normalize_parent` to
the stash enum and `PrepareParent`/`prepare_parent` to the attached-reset enum.
`BeginStash` starts a root action with a candidate handoff at
`NormalizeParent`; `BeginResetAttachedRef` starts its corresponding root action
at `PrepareParent`. The only new reducer edges are
`NormalizeParent -> NormalizeMarker` and
`PrepareParent -> PrepareMarker`. Every later normalize, stash, reset,
restore, bundle, and completion edge remains unchanged.

Both phases are root-handoff-only. Non-root owners and root owners with
`NoCandidate` or `EvidencePending` retain the exact short graphs
`CreateStash -> WriteBundle -> Complete` and `ResetRef -> Complete`; validation
must reject either parent phase there. No second cursor or durable field is
introduced, and the retained v0 graph and bytes remain unchanged.

### Directional physical classification

Both phases map to the already-reviewed
`Managed(MarkerParentDirectory, Handoff -> AttachedClean)` transition. Parent
readiness remains separate from marker publication: the parent executor may
publish only an empty directory, and the immediately following marker phase
is still the sole leaf writer.

The generalized classifier is exact in all four marker directions:

- absent source, present goal: final missing with staging absent or one exact
  empty staging directory is `Before`; exact empty final with staging absent
  is `AfterNeedsDurability` on every platform;
- present source, absent goal: the real parent containing only the expected
  later source marker is ordinary proof-only `After`;
- absent source, absent goal: missing and real-empty parents are equivalent
  ordinary proof-only `After`; and
- present source, present goal: the established real parent is ordinary
  proof-only `After`, including differing source/goal marker bytes.

Thus source-equals-goal remains goal-first ordinary `After` with no physical
call except the single accepted causal refinement: absent source marker,
present goal marker, and exact empty final. Both names, malformed/duplicate
staging, a foreign child, symlink, non-directory, parent replacement, wrong
marker position, or a missing parent after the durable edge is `Ambiguous`.
No marker phase or later phase may repair it.

### Attempts, crashes, and restart

The existing A/B/C/D namespace machine applies without a new state. A crash
before publication leaves either `Before` form. A crash after publication but
before its durability proof leaves final-only `AfterNeedsDurability`. Unix
repeats the retained-parent sync; Windows repeats the pinned, handle-bound,
write-through final-to-staging-to-final barrier. An interruption between the
Windows renames leaves staging-only `Before`. Final plus staging remains
ambiguity rather than cleanup or adoption.

`PreservationDurabilityPending` continues to bind the exact record, owner,
action, new phase, cursor prefix, forms, commits, and physical goal. With no
attempt it executes the bound barrier. Only a fresh matching success plus a
fresh final-only goal advances to the marker phase. A matching failure over
`Before`, pending, or `Ambiguous` returns the existing typed diagnostic with no
transition or same-invocation retry; stale, mismatched, second, or successful-
without-goal attempts reject. A crash after the barrier but before record
rewrite loses the in-memory attempt and repeats the barrier on restart.

### Wire, cursor, evidence, and accounting

The two prerelease enum spellings are included in exact YAML acceptance and
retired-spelling rejection. Root-only construction, unknown-field byte
identity, serialized action identity, full preservation prefix, exact
predecessor/successor, wrong owner, action-free misuse, cursor exhaustion, and
short-graph rejection are all assigned to existing owners. The required
matrix also covers every marker-presence direction, missing/empty equivalence,
source-equals-goal, staging-only and final-only restart, both-name collision,
foreign residue, causal success/failure, wrong phase, unchanged non-root
graphs, and retained v0 characterization. No production or evidence path is
added.

The §13 arithmetic reconciles:

- P0.2: 3,100/450/11,100 with unchanged 19/26 paths;
- duplicate P1: 4,000/1,500/6,600 with unchanged 20/19 paths;
- R4b-P: 14,000/2,950/31,400 with 166/157 charged and 104/105 unique paths;
  and
- program unique-once: 19,250/38,300 with unchanged 145/148 paths.

The increases from the accepted §12 baseline are 450 P0.2 production lines
and 2,900 P0.2 evidence lines, plus the 300/600 shared-Git duplicate charge in
P1; the program union counts that shared work once. The unchanged path and
movement ceilings, existing ownership rows, and enlarged document reserve are
consistent across all controlling documents. Any new owner, graph state,
durable field, or ceiling overrun stops implementation for renewed review.

## §13 settled-code review — 2026-08-11

### Verdict

**GO.** The settled shared tree implements the reviewed P0.2 durable-parent
interface without an unreviewed state, transition, authority path, production
owner, or wire change. P0 findings: none. P1 findings: none. P2 findings:
none. P3 findings: none. Multi-owner/global preflight remains explicitly
deferred to the P1 gate; this verdict does not authorize that work.

This review read the complete current Amendment 2, reverse lifecycle
interface, transition design, change budget, and R4b refactor controls, then
inspected the affected model, reducer, validator, classifier, filesystem
backend, lifecycle resolver, authority tables, protocol evidence, tests, and
working-tree diff. The settled implementation and §12.4/§13 evidence agree.

### Stored state, graphs, and wire closure

The durable spellings are exactly `normalize_parent` and `prepare_parent`.
Root stash begins `NormalizeParent -> NormalizeMarker`; root reset begins
`PrepareParent -> PrepareMarker`. Their predecessor and successor tables,
wrong-successor rejection matrices, owner validation, action-bearing rows,
cursor prefixes, exhaustion behavior, transition effects, and physical
footprints are closed over those exact graphs. Root-only construction and
handoff behavior are enforced. The non-root stash and reset graphs retain
their old marker-first starts, and the evidence-pending/non-candidate short
graphs remain unchanged.

Exact YAML accepts the two new snake-case values and rejects retired compound
spellings. Unknown-field round trips preserve the new child phase and handoff
identity byte-for-byte. The serialized pending action contains the new phase
through the existing preservation binding; no new durable field or protocol
discriminant was introduced. The retained v0 preservation characterization
also remains unchanged.

### Directional classification and causal pending state

The classifier uses the reviewed source-marker/goal-marker direction in both
forward and restore parent phases. With a present source marker and absent
goal marker, a missing or real-empty parent is `Before`. With an absent source
marker and present goal marker, an exact empty final is the sole
`AfterNeedsDurability` form. A present source marker and absent goal marker
with the expected later source marker, absent/absent with missing or empty
parent, and present/present with the established real parent are ordinary
proof-only `After`. Source-equals-goal is goal-first `After`.

Staging-only is restartable `Before`; final-only after the durable edge is
causal pending where required. Both names, foreign residue, malformed or
duplicate staging, symlinks, non-directories, replacement races, and wrong
marker positions are ambiguity. The parent mutation publishes only an empty
directory and never writes marker bytes. Unix uses no-replace rename followed
by retained-parent sync; Windows uses the pinned handle-bound write-through
round trip. Containment and replacement checks remain fail closed.

`PreservationDurabilityPending` binds the exact record, owner, phase, action,
cursor prefix, forms, commits, and physical goal. No-attempt input executes
only that barrier. A fresh matching success advances only after reobservation
of the exact goal. A matching failure returns the original typed code and
message without transition or retry. Stale, mismatched, duplicate, and
success-without-goal attempts reject. Crash before publication, after
publication, during the Windows round trip, and after barrier success but
before record rewrite all converge to the documented prefix-safe restart
behavior.

### Authority and activation boundary

The root-owner matrix, transition-effect matrix, physical-footprint matrix,
dispatcher attempt matrix, and reconciliation matrix cover the new phases.
The production observer and executor remain deliberate
`MergePhaseUnsupported` stubs, and the router reaches preservation work only
through those stubs. No alternate authority or execution bypass was found.
Activating production observation/execution, or introducing multi-owner or
global preflight, remains P1 work and requires its own accepted gate.

### Evidence, gates, and settled accounting

The focused state/restart evidence covers all four marker-presence
directions, missing/empty equivalence, source-equals-goal, staging/final/both
and foreign forms, forward and restore parent faults, causal success/failure,
attempt identity, cursor prefix/exhaustion, exact successor rejection,
root-only/short-graph behavior, effects/footprints, unknown-field identity,
and retained v0 behavior. Local macOS verification passed:

- the complete `gwz-core` suite;
- focused root-preservation, prefix, causal-pending, exact-YAML, root-phase,
  protocol, unknown-field, and retained-v0 tests;
- formatting and all-target/all-feature clippy with warnings denied;
- merge-document consistency, compatibility validation and its unit tests;
  and
- retained-reader validation, gate-ready checks, and checked macOS evidence.

The P0.2 production manifest remains exactly 19 paths. Measured against its
frozen baseline, physical production growth is 3,165 lines, comprising the
frozen 450-line movement allowance plus 2,715 net semantic production lines:
**2,715/3,100 production and 450/450 movement**. No production symbol escaped
the manifest. The evidence manifest remains exactly 26 paths. Before this
appended settled-code review, conservative added evidence was 8,751 lines
against the 11,100-line ceiling, leaving 2,349 lines for this appendix and any
final evidence-only reconciliation.

All reviewed P0.2 production owners stay within their frozen ceilings. The
1216-line hand-written `gwz-core/tests/protocol.rs` owner is accepted as a
cohesive P3 integration conformance owner: its modest general-threshold
overrun is a review trigger, not an automatic split, and splitting it would
add an evidence path without creating a meaningful authority boundary. No
split stop is required. The duplicate P1, R4b-P, and program-unique arithmetic
remains as recorded in §13; this GO applies only to the settled P0.2 slice.

Final conservative evidence growth including this 112-line review is
**8,863/11,100 lines across 26/26 paths**, leaving 2,237 evidence lines of
reserve. No new evidence owner was introduced.

## §13 corrected-tree state/restart/causal-authority re-review — 2026-08-11

### Verdict

**GO.** The corrected production-disabled P0.2 slice closes every defect from
the first settled-code architecture review. P0 findings: none. P1 findings:
none. P2 findings: none. P3 findings: none. This local settled-code verdict
does not activate P1 or release: the declared native release-platform matrix
must still pass on the committed and pushed corrected tree.

This re-review explicitly revisited P1-SC1 through P2-SC4 rather than relying
on the earlier state review. It inspected the corrected raw-index parser and
namespace guard, physical classifier and mutators, the three parent-phase
causal-authority rows, unknown-field identity logic, production router and
stubs, complete focused evidence, corrected control texts, manifests, and
ledger arithmetic.

### Raw-index provenance and pre-mutation rejection

The exact raw index is parsed and semantically validated before the selected
marker and lock facts can authorize preparation, observation, or execution.
Stage conflicts, assume-valid, skip-worktree, intent-to-add, and any unknown
extended flag reject. The marker namespace check now rejects every index entry
equal to or below `gwz.conf/markers` except the exact selected marker path,
including the formerly accepted index-only second marker whose worktree leaf
is absent. The same namespace check runs before and after managed index
rewrite.

The focused regression stages a second marker, removes its worktree leaf, and
then proves that preparation, first-step observation, and checked execution
cannot authorize mutation. Its exact snapshot retains HEAD, loose and packed
refs, raw index bytes, marker, lock, and boundary bytes; the test separately
confirms the absent foreign worktree leaf and no native stash. The four named
semantic-flag rows use the same rejection assertion, with intent-to-add work
also checked byte-exact. P1-SC1 is closed.

### Parent state, causal authority, and restart

`NormalizeParent`, `PrepareParent`, and the retained `RestoreParent` use the
same bound `MarkerParentDirectory` physical action while retaining their exact
stash/reset phase and cursor identities. Their causal matrix covers pending
and `Before` with no attempt, exact matching success, matching failure over
pending/`Before`/ambiguity, crash after barrier but before record rewrite,
stale record digest, mismatched and second attempts, wrong prefix, wrong
action/phase, generic ambiguity, and success without the exact pending goal.
Failure returns the original typed code and message without transition or
same-invocation retry; only a fresh exact goal plus its bound success advances
to the exact marker successor.

The SHA-1/SHA-256 physical matrix runs normalization/preparation, managed
marker/lock/index mutation, stash restoration, checked reset, both restore
directions, restart observation, and idempotent replay. Both formats also run
the checked normalized-image/native-stash/managed-index round trip. The
source-equals-goal matrix proves every allowed managed no-op is ordinary
write-free `After`. Forward and restore parent fault evidence covers missing,
staging-only, final-only, both-name, residue, collision, publication, Unix
parent-sync, and exact replay forms; the Windows-only evidence pins both
write-through rename boundaries for its native lane. Reset partials remain
ambiguous and preserve exact ref/index/worktree bytes.

The directional classifier remains closed: absent-source/present-goal exact
empty final is the sole `AfterNeedsDurability` form; source-present/goal-
absent with the expected later source marker, absent/absent missing or empty,
present/present established parent, and source-equals-goal are ordinary
proof-only `After`. Both names, foreign residue or child, replacement, wrong
marker position, and missing parent after the durable edge reject as
ambiguity. The parent action publishes only an empty directory; the following
marker action remains the sole writer of marker bytes.

### Unknown identity and authority boundary

The focused unknown-field owner now crosses both `normalize_parent ->
normalize_marker` and `prepare_parent -> prepare_marker`. Unknown action and
handoff children survive only when the full typed action and handoff identity
remain exact; changing either identity retires both rather than rebinding
them. Exact YAML, root-only validation, successor/predecessor closure, action
identity, cursor/exhaustion, effects/footprints, wrong-owner rejection, short
graphs, and retained-v0 characterization remain closed without a new durable
field or released-wire migration.

The production preservation observer still returns
`MergePhaseUnsupported("preservation observation is not implemented")`, and
the production executor still returns the corresponding not-implemented
diagnostic. The reverse router delegates only through those closed stubs. No
alternate production-v1 authority, direct backend call, hidden parent
creation, parent-plus-marker action, or mutation bypass was found.

### Manifests, Python fixture, and cohesion

The corrected P0.2 manifest is **23 production and 26 evidence paths**. The
four newly charged causal export surfaces were already present and P0-owned;
they add P0.2/P1 charged appearances, not new semantic LOC or a unique path.
The reconciled ledgers are P1 **24/19**, R4b-P **174/158 charged** and
**104/106 unique**, and program **145/149**. The program evidence increment is
P3's existing `gwz-py/src/tests/test_client.py`: its required optional
`MergeResponse.record: None` fixture is present and its 25 focused tests pass.
It is correctly excluded from the P0.2 evidence manifest and line ceiling.

Against the accepted pause-point measurement, the only corrective production
growth is 23 semantic lines in `preservation_root/index.rs`, which grows from
352 to 375 lines. The settled charge is therefore **2,738/3,100 semantic
production lines and 450/450 moved lines**. Conservative evidence additions
before this appendix are **9,431/11,100** across the frozen 26 paths.

The focused production owners remain cohesive and below their reviewed caps:
`contract.rs` 837, `preservation.rs` 455, `preservation_image.rs` 491,
`preservation_root.rs` 499, `files.rs` 297, `index.rs` 375,
`index_format.rs` 341, and `parent.rs` 351 lines. Focused evidence owners are
also below 500 lines, including authority 498, attempt matrix 498,
reconciliation 483, prefix matrix 487, unknown preservation 158, and the G15
owners at 438-498 lines. Their responsibilities remain singular enough that
no new cohesion owner is required.

The hand-written 1,216-line `gwz-core/tests/protocol.rs` crosses the general
1,000-line judgment trigger but is accepted without a split finding. Its 28
tests remain one public wire-conformance owner for round trips, pinned
discriminants, generator freshness, and shared protocol fixtures. It contains
no P0.2 filesystem or lifecycle policy, and splitting it would add evidence
paths without creating a clearer authority boundary.

### Verification and remaining external gate

Local corrected-tree verification passed: 34 focused root-preservation tests;
the causal-parent and unknown-identity tests; the complete core suite (1,078
passed, one ignored), including 28 protocol tests; all 330 Python tests, including
the 25 fixture-focused tests; formatting; strict all-target/all-feature
clippy; diff whitespace checks; and merge-document consistency (87
assertions). These runs execute the current macOS lane and inspect, but do not
claim execution of, the native Windows/Linux release lanes.

The external native release matrix remains a required P1/release gate for
Windows x86-64, both macOS architectures, Linux x86-64, and Linux ARM64 on the
committed and pushed corrected tree. Its pending status is not a local P0.2
code defect, but P1 consumption and release remain blocked until it passes.

Final conservative evidence growth including this 139-line appendix is
**9,570/11,100 lines across 26/26 paths**, leaving 1,530 evidence lines of
reserve. No new evidence owner was introduced.

## Windows platform-remediation review — 2026-08-11

### Verdict

**GO for code; platform evidence pending.** Independent review of committed
`gwz-core` range `71ffb641..064f06e571334c3935233a7d253effc73fcc4b75`
found no P0, P1, P2, or P3 code defect. The reviewed head is exactly
`origin/main` at `064f06e`; the workspace was clean before this review memo
append. This verdict is separate from the still-running native release gate.

### Windows identity and containment

`record_wire/location.rs` now obtains Windows identity from each opened handle
with the stable `windows-sys` `GetFileInformationByHandle` API. Identity is a
non-optional volume serial plus 64-bit file index, eliminating the previous
`None == None` false-equivalence risk. These are the same non-UWP identity
fields that Rust derives for ordinary Windows file metadata, without relying
on the unstable optional `MetadataExt` accessors.

Every Windows named open uses read access, delete sharing,
`FILE_FLAG_BACKUP_SEMANTICS`, and `FILE_FLAG_OPEN_REPARSE_POINT`. It can open
directories, observes rename/delete races rather than masking them with a
share violation, and obtains identity for the named reparse object instead of
following a symlink or junction. The preceding and following
`symlink_metadata` type checks reject non-real parents and nonregular leaves.

For each record leaf, the pre-open named identity, the byte-reading handle
identity, and the post-read named identity must all match. The bytes, length,
SHA-256 digest, opaque path, and returned identity therefore belong to the
same checked file handle; byte-identical replacement is still contention, not
adoption. Root, `.gwz`, merge, and optional archive-parent identities are
reacquired and compared, so real-directory replacement or archive-parent
appearance/disappearance rejects. The second complete leaf read must equal
the first, closing leaf appearance, replacement, and content races before any
authority is returned. Acquisition remains read-only and fail-closed.

### State, ownership, and accounting

No model, durable record, lifecycle phase, reducer, observer, executor,
transition, error code, or production-v1 activation changed. Callers retain
the same `CanonicalMergeLocations` state and optimistic retry/recovery rules;
the remediation strengthens only physical location identity.

`record_wire/location.rs` is the already accepted version-neutral canonical
record-location owner from the broader R4b/P0.1 interface, not a P0.2 owner.
The commit adds 92 and removes 40 lines there, for +52 net; the owner remains
cohesive at 478 lines under its focused 500-line threshold and adds no path.
`preservation_root/files.rs` is a frozen P0.2 owner, but its three-add/three-
delete change only places `OsMetadataExt` behind `cfg(unix)`. It is behavior-
neutral, remains 297 lines, and changes neither P0.2 semantic LOC, movement,
path counts, authority, nor physical mutation behavior.

### Verification and exact remaining gate

Local macOS verification passed the four canonical-location contention tests,
the byte-identical status retry, the complete core suite (1,078 passed, one
ignored), formatting, strict all-target/all-feature clippy, and range
whitespace checks. The Windows implementation was also checked against the
stable Rust/windows-sys signatures and flag semantics; native CI results are
not claimed by this code verdict.

The gate is not satisfied merely by the current `gwz-py` Windows/macOS/Linux
source-build matrix plus the retained-reader five-platform runs. The remaining
condition is the exact committed-and-pushed-tree `gwz-cli` Cargo Dist build for
all five declared targets: `x86_64-pc-windows-msvc`,
`aarch64-apple-darwin`, `x86_64-apple-darwin`,
`aarch64-unknown-linux-gnu`, and `x86_64-unknown-linux-gnu`. P1 consumption
and release remain blocked until that exact matrix succeeds.

This 73-line review raises conservative P0.2 document evidence to
**9,643/11,100 lines across 26/26 paths**, leaving 1,457 lines of reserve; it
introduces no production or evidence path.

## §14 native platform-gate interface review — 2026-08-11

### Verdict

**NON-GO.** P0 findings: none. P1 findings: one. P2 findings: none. P3
findings: none. The proposed gate is otherwise a bounded, non-publishing
evidence owner, but its successful run is not yet cryptographically bound to
the reviewed workflow definition that produced the evidence.

### P1-G1 — the dispatch workflow revision is not bound to the CLI input SHA

Section 14 requires exact 40-hex `gwz-cli` and `gwz-core` inputs, checks out
both commits into sibling directories, verifies their checked-out `HEAD`
values, and writes both inputs into job summaries. It does not require the
GitHub Actions workflow-definition SHA (`github.workflow_sha`, together with
the expected workflow repository/path) to equal the reviewed `gwz-cli` input
SHA or otherwise bind that third execution identity into the retained
evidence.

`workflow_dispatch` may select a ref whose workflow body differs from the
checked-out CLI commit. Without this binding, an older or unreviewed workflow
definition could omit a target, weaken the self-check, or gain publishing
behavior while still checking out and reporting the requested product SHAs.
The two product `HEAD` checks therefore do not by themselves prove that the
reviewed gate implementation generated the run.

Required correction: the interface must require an early fail-closed check
that the executing workflow repository/path and `github.workflow_sha` are the
reviewed `gwz-cli` repository/path and exact `gwz_cli_sha` input. The workflow
SHA must also appear in every target summary and retained evidence. This uses
the already allocated workflow path and needs no new state, target, owner, or
line ceiling. Return the corrected §14 text for re-review before authoring the
workflow.

### Accepted interface boundaries

The remaining contract is accepted. The matrix contains exactly the five
Cargo Dist targets frozen in `dist-workspace.toml`: Windows MSVC x86-64, both
macOS architectures, and GNU Linux ARM64/x86-64. Each target runs natively on
Rust 1.95 with Cargo Dist 0.31; failure or cancellation of any row fails the
gate.

The workflow is `workflow_dispatch` only with `contents: read` and no release
event, tag inference/creation, hosting, announcement, attestation, GitHub
Release, package publication, or release-asset upload authority. Local Cargo
Dist output may be retained only as diagnostic Actions artifacts. The static
self-check and the ordinary workflow failure semantics are appropriately
fail-closed, and each target must retain its Cargo Dist manifest/artifacts and
exact commit summaries.

The single new path `gwz-cli/.github/workflows/platform-gate.yml` is correctly
charged once to P0.2 and once to P1: P0.2 evidence becomes 27 paths, P1 becomes
20, R4b-P becomes 174/160 charged and 104/107 unique, and program union becomes
145/150. It introduces no production path and leaves the 11,100-line evidence
ceiling unchanged. Before implementation, the reconciled actual remains
2,738/3,100 semantic production, 450/450 moved, and 9,782/11,100 evidence
lines across 23/26 implemented paths within the frozen 23/27 manifest.

The separate `gwz-py` source-build matrix and five-platform retained-reader
runs remain useful supplemental evidence but cannot satisfy this exact
`gwz-cli` Cargo Dist gate. No workflow or control document is authorized for
implementation until P1-G1 is corrected and both interface reviews are GO.

This 67-line review raises conservative document evidence to
**9,849/11,100 lines across 26 implemented paths**, leaving 1,251 lines for
the allocated workflow and bounded re-review without adding another path.

## §14 workflow-identity corrective re-review — 2026-08-11

### Verdict

**GO.** P1-G1 is closed. P0 findings: none. P1 findings: none. P2 findings:
none. P3 findings: none. This verdict accepts the corrected interface only; it
does not authorize P1 before workflow implementation, focused settled-code
review, and a successful exact-SHA five-target run.

### P1-G1 closure

The workflow must now fail before checkout unless all three execution-source
facts are exact: `github.repository` is `owebeeone/gwz-cli`, the path in
`github.workflow_ref` is
`.github/workflows/platform-gate.yml`, and `github.workflow_sha` equals the
supplied 40-hex `gwz_cli` commit. The executing workflow body is therefore the
reviewed gate definition contained in the same CLI commit whose product source
is built; selecting another dispatch ref cannot manufacture accepted evidence.

After that pre-check, both supplied commits are checked out as siblings and
their observed `HEAD` values must match. Every target's job summary and
gate-identity artifact binds the workflow repository/path/SHA, both supplied
and observed checkout SHAs, and the target. The retained Cargo Dist manifest
and artifacts are consequently attributable to one exact workflow definition,
one exact CLI/core pair, and one exact matrix row.

The previously accepted boundaries remain unchanged: manual dispatch only,
`contents: read`, no tag/release/hosting/announcement/attestation/package or
release-asset publication path, exact static five-target native matrix, and
failure or cancellation of any target failing the gate. The existing single
workflow path and 23/27 P0.2, 24/20 P1, 174/160 charged, 104/107 unique, and
145/150 program path ledgers need no change.

This 37-line re-review raises conservative document evidence to
**9,886/11,100 lines across 26 implemented paths**, leaving 1,214 lines for
the allocated workflow and settled-code review without adding another path.

## §14 Cargo Dist contract corrective re-review — 2026-08-11

### Verdict

**GO.** Architecture findings P1-14A and P2-14B are closed. P0 findings:
none. P1 findings: none. P2 findings: none. P3 findings: none. Section 14 now
defines a complete pre-implementation interface for the single non-publishing
native platform-gate workflow. This verdict accepts the interface only; P1
remains paused through workflow implementation, focused settled-code review,
and a successful exact-SHA five-target run.

### P1-14A closure

The contract job owns one JSON matrix, rejects duplicate, missing, or extra
targets and target-to-runner drift, and supplies that same exact matrix to the
build jobs. Its required set is exactly equal to the five targets frozen in
the checked `gwz-cli/dist-workspace.toml`; no build row may introduce an
implicit sixth target or substitute a runner.

Every row now executes the exact CI-safe command
`dist build --artifacts=local --target=<row-target> --tag=v0.2.0-dev
--print=linkage --output-format=json`. The explicit selector matches the
checked CLI package's exact `0.2.0-dev` version and is neither an inferred
selector nor a Git-ref creation or release operation. Post-build acceptance
requires Cargo Dist `0.31.0`, `announcement_tag: v0.2.0-dev`,
`announcement_tag_is_implicit: false`, and exactly that row target's
executable archive plus checksum as reported by
`dist print-upload-files-from-manifest`; output for any other target fails the
row. These requirements close implicit-version, tag, fuzzy-host, matrix-drift,
and cross-target-output cases.

The earlier accepted identity and authority boundaries remain intact: the
workflow repository/path/SHA is bound to the exact CLI input before checkout,
both sibling checkout `HEAD` values are verified, every row retains complete
identity and Cargo Dist evidence, permissions are `contents: read`, and no
release, publishing, hosting, announcement, attestation, tag-creation, or
release-asset operation is permitted.

### P2-14B closure and accounting

`GwzM5-8R4bTransitionDesign.md` and `GwzM5-8Refactor.md` now describe §14 as
proposed evidence pending interface review instead of claiming that review is
already complete. The remaining controls consistently retain the pause
through implementation, settled-code review, and the exact committed and
pushed run.

The accepted one-path accounting is unchanged: P0.2 remains **23/27**, P1
**24/20**, R4b-P charged **174/160**, R4b-P unique **104/107**, and
program-unique **145/150**. No production path or numeric line ceiling changes,
and no workflow implementation is claimed by this review.

## §14 settled workflow and exact-run state/evidence review — 2026-08-11

### Verdict

**GO to unpause P1.** P1-WR1 is closed. P0 findings: none. P1 findings: none.
P2 findings: none. P3 findings: none. The pushed one-path workflow implements
the accepted §14 interface, and successful run `31442644461` supplies complete
non-publishing native Cargo Dist evidence for exact `gwz-cli` commit
`038fa68e6bd537d9d89249dbf24620fc48e5fd87` and exact `gwz-core` commit
`064f06e571334c3935233a7d253effc73fcc4b75`.

### Executing identity and matrix

The completed run is attempt 1 of `.github/workflows/platform-gate.yml`, was
manually dispatched from `main`, and has `head_sha` equal to the supplied CLI
SHA. Its contract job passed the pre-check binding `github.repository`,
`github.workflow_ref`, and `github.workflow_sha`, checked that exact CLI tree,
and emitted the only matrix. The implementation rejects duplicate or drifting
rows and compares the target set exactly with the checked
`dist-workspace.toml`.

GitHub's job records and all five retained identities agree on the complete
native mapping: Windows x86-64 / `windows-2022`, Apple ARM64 / `macos-14`,
Apple x86-64 / `macos-15-intel`, GNU Linux ARM64 / `ubuntu-22.04-arm`, and GNU
Linux x86-64 / `ubuntu-22.04`. Each identity records the expected repository
and workflow path, workflow/CLI SHA, supplied and observed CLI SHA, supplied
and observed core SHA, target, and runner. Each row's successful identity step
writes that same dictionary to its job summary, so the retained JSON and
summary source have one state owner rather than parallel identity logic.

The exact CLI manifest depends on `gwz-core` only through sibling path
`../gwz-core`; every row's pre-build `HEAD` checks passed. Contract and all five
rows completed successfully with `fail-fast: false`; no target was cancelled,
skipped, or inferred.

### Cargo Dist evidence and the absolute-path correction

Every row ran Cargo Dist `0.31.0` on Rust `1.95.0` with exactly
`dist build --artifacts=local --target="$TARGET" --tag=v0.2.0-dev
--print=linkage --output-format=json`. All five downloaded manifests report
`dist_version: 0.31.0`, `announcement_tag: v0.2.0-dev`, and
`announcement_tag_is_implicit: false`. Each names only its exact executable
archive and checksum, and both artifact records carry only that row's target;
the archive metadata names its checksum. The five downloaded checksum files
verify. Archive inspection finds the corresponding ARM64/x86-64 Mach-O, ELF,
or PE executable in every package.

Failed run `31442318333` used preceding CLI/workflow commit `ec906511...`.
All five builds completed but all five rows failed closed before identity or
artifact upload because Cargo Dist 0.31.0 returned absolute paths. Commit
`038fa68...` changes only the path acceptance/output handling: it resolves
`target/distrib` and each reported path and requires every resolved file's
parent to equal that exact directory. Nested and sibling paths still reject;
a wrong-target direct child still fails the unchanged exact two-name upload
set, manifest upload set, `[target]` metadata, checksum linkage, and existence
checks. Thus the correction accepts absolute or relative spellings of direct
row files without weakening target-exact validation.

### Authority, accounting, and closure

The workflow is `workflow_dispatch` only with top-level `contents: read`,
credential persistence disabled, and no publishing job or step. Its only
retained output is five ordinary diagnostic Actions artifacts. Run and source
inspection found no host/create/upload/release/announce Cargo Dist operation,
GitHub Release, tag creation, attestation, package publication, or release-
asset upload; GitHub has no `v0.2.0-dev` tag or release.

The workflow is one cohesive 366-line evidence owner, below the 500-line new-
owner boundary. It adds no production path. The reconciled control baseline is
2,738/3,100 semantic production, 450/450 moved, and 10,490/11,100 evidence
lines across the fully implemented 23/27 manifest. The accepted charges remain
P0.2 **23/27**, P1 **24/20**, R4b-P **174/160** charged and **104/107** unique,
and program **145/150**. The exact pre-publication native platform gate that
P1-WR1 required is now implemented, reviewed here, and successful on the exact
pushed CLI/core pair; no state/evidence condition remains to keep P1 paused.
