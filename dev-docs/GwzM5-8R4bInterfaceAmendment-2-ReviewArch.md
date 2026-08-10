# R4b-P0.2 interface amendment review — architecture and ownership

Date: 2026-08-11

Re-review date: 2026-08-11, after the exact physical contract, manifests, and
package/program ledgers were reconciled.

## Re-review verdict

**GO.**

No P0, P1, P2, or P3 finding remains in the revised amendment or its four
controlling parent documents. The exact physical-step API is implementable
without importing or cloning model publication policy into Git; the checked
stash and reset boundaries close the preimage and authority/mutation gaps; the
index, marker, boundary, restart, and no-op contracts are exact; and every
manifest and charged/unique ceiling reconciles.

This GO accepts the P0.2 architecture and ownership checkpoint within the
frozen 10-production-path and 20-evidence-path manifests and, with both
interface reviews GO, authorizes P0.2 implementation. It does not authorize P1;
P1 remains paused until P0.2 implementation passes both settled-code reviews
and the required quality gates.

### Prior finding closure

| Prior finding | Re-review result |
| --- | --- |
| P1-1 — undefined Git form and duplicated model policy | Closed. `GitRootManagedIndexFact` and `GitRootManagedIndexEntry` define exact physical index facts. `GitRootPreservationSpec` carries explicit commit-derived `C0`/`C1`, authority-derived `H`, and the invariant boundary. Git validates physical facts and commit-tree provenance only; authority owns durable-phase-to-step selection and the sole model-to-`H` projection. |
| P1-2 — incorrect boundary-consumption claim | Closed. The amendment now states that status/image capture and native stash consume `.git/info/exclude` for ignored/untracked partitioning. Every observation binds the exact invariant handoff bytes, and the required matrix crosses a path whose classification differs under alternate boundary bytes. |
| P1-3 — stale preimage window at `CreateStash` | Closed. Both before-observation and checked execution reproject the complete normalized image under the exact handoff boundary after durable `NormalizeIndex` and immediately before native stash mutation. Included staged, unstaged, untracked, and raw-path changes reject; ignored changes remain excluded only under the unchanged boundary. |
| P1-4 — provisional manifest and insufficient ceiling | Closed at the interface-GO checkpoint. The amendment froze exactly 10 production and 20 evidence/tool/document paths, split Git image/root/file/index responsibilities into cohesive owners, and replaced the provisional 950/1,200 budget with 2,100 net production, 450 moved production, and a then-current 5,000 evidence lines. That evidence figure was later superseded by the retained-review ledger below. |
| P2-1 — duplicate dirty output and impossible preparation claim | Closed. `GitPreparedRootStash` returns one normalized image. Read-only preparation accepts only complete exact `H` and virtually projects `C0`; partial physical states belong solely to the step observer. |
| P2-2 — multiple marker identities | Closed. `managed_marker_path` fixes one canonical operation marker. Every marker worktree/index fact must use that path, the lock and boundary paths are backend-fixed, and a second marker rejects before repository inspection. |
| P2-3 — serialized index byte-identity overclaim | Closed. The index contract now preserves and reverifies every unrelated canonical entry tuple and staged content while permitting lockfile reserialization. Unsupported flags or extensions reject before mutation. |

### Checked physical boundary

The flattened phase table is complete and disjoint. Each phase names one
marker, lock, or index mutation; source-equals-goal is classified `After`
before `Before`, so a no-op advances durably without a physical write. The
backend receives a physical object/source/goal step and an explicit
`NormalizedPreimage` or `OtherwiseClean` guard, never a model phase or generic
path callback.

`CreateStash` is now inside the checked boundary: observation reprojects the
durable normalized image, the executor repeats the complete classification,
and post-state requires one exact decoded stash plus no unrelated live dirt.
`ResetAttachedRef` likewise rechecks the exact attached branch/commit, `C0`,
boundary, native state, and unrelated-work guard inside the same checked
backend call before invoking the existing checked reset. Its after-state is
the exact restore commit with `C1`; every partial checkout/ref/index/worktree
state is ambiguity, with no force, adoption, repair, or retry mutation.

The managed index contract is semantic rather than serialized-file identity:
raw path, stage, mode, semantic flags, object identity, and staged content are
preserved for every unrelated entry. Exact absence, SHA-1/SHA-256 object ids,
unknown flags, unsupported extensions, and before/after reverification are all
covered by the required evidence.

### Manifest and ledger reconciliation

Mechanical inventory of the retired exact phase variants and YAML spellings
finds only the three listed model/transition production owners and six listed
existing evidence owners. The remaining seven production paths are the
bounded Git surface and its four new cohesion owners; the seven Git evidence
paths are the registration owner plus six focused root-preservation owners.
The amendment also includes the four required control documents and both
review memos. No unlisted substitution is permitted.

The superseded interface-GO checkpoint allocation was exact and is retained
here as review history, not as the controlling retained-review ledger:

| Charge | Net production | Moved production | Test/tool/doc | Production paths | Evidence paths |
| --- | ---: | ---: | ---: | ---: | ---: |
| P0.2 shared Git | 1,650 | 450 | 2,400 | 7 | 7 |
| P0.2 model/transition | 450 | 0 | 800 | 3 | 6 |
| P0.2 documents/reviews/control | 0 | 0 | 1,800 | 0 | 7 |
| **P0.2 total** | **2,100** | **450** | **5,000** | **10** | **20** |

Only the shared Git row is charged again to P1. Three production paths and
`git/tests/g15.rs` already belonged to P1, so the revised P1 ceiling is exactly
3,000 net production, 1,500 moved production, 4,200 evidence lines, 13 charged
production paths, and 14 charged evidence paths.

At that interface-GO checkpoint, summing every R4b-P slice gave 12,000 net
production, 2,950 moved production, 22,900 evidence lines, 150 charged
production paths, and 146 charged evidence paths. The physical P-package union is 97 production and 102 evidence paths:
P0.2 contributes four new production and ten new P-package evidence paths,
while P1's shared charge contributes no second physical path.

At that interface-GO checkpoint, the program-wide unique-once ceilings were
18,250 production lines, 32,200 evidence lines, 141 production paths, and 145
evidence paths. The shared P1 Git work is counted once, not twice. The program evidence union grows by nine
rather than ten because accepted R4b-TI already charges
`model/v1/validate/tests.rs`.

The ownership split is credible. Current `contract.rs` and `preservation.rs`
are 714 and 788 lines respectively; the amendment caps them below 950 and 850
after extraction. Each of the four new production owners and six new focused
Git evidence owners remains below 500 lines, and any newly discovered owner or
ceiling breach reopens mechanical recount and review.

### Re-review scope

This verdict covers the revised bytes of:

- `GwzM5-8R4bInterfaceAmendment-2.md`;
- `GwzM5-8R4bReverseLifecycleInterface.md`;
- `GwzM5-8R4bTransitionDesign.md`;
- `GwzM5-8ChangeBudget.md`; and
- `GwzM5-8Refactor.md`.

The parent documents consistently authorize P0.2 implementation while keeping
P1 paused for both P0.2 code re-reviews, make amendment 2 controlling for the
exact root-preservation contract, preserve completed P2-P4 work without
authorizing R4b-G integration, and retain the disabled-v1 boundary.

## Initial verdict — superseded by the re-review above

**HOLD.**

The flattened stash and attached-reset phase graphs are the minimum durable
cursor needed for the three independently committed managed objects: marker
worktree leaf, lock worktree leaf, and their raw index subset. A second root
cursor would duplicate the pending phase. The boundary does not need a phase
because P0.2 never mutates it. The existing checked attached-ref reset can also
remain one journaled action: its safe checkout and ref transaction are not
claimed atomic, and an interrupted mixed checkout is rejected as ambiguous
rather than adopted or repaired.

The checkpoint is nevertheless not implementable as written. Three semantic
contracts are incomplete or contradictory, and the provisional manifests and
ceilings are not exact or credible. I find no P0 issue because P1 remains
blocked and no P0.2 mutation is authorized yet. There are four P1 findings and
three P2 findings.

## P0

None.

## P1

### P1-1 — the proposed Git-neutral physical form is undefined and duplicates model policy

Section 5 names `GitRootPublicationIndex` but never defines its representation
or exact equality. It also says `GitRootPublicationHandoff` contains the
existing closed prefix/index enums. Those enums are owned by the v1 model. The
Git module cannot import them without reversing the existing `workspace_ops ->
git` dependency, while cloning them into `gitbackend::contract` creates a
second semantic vocabulary and a second prefix-to-physical-form mapping.

The redundancy is visible in `GitRootPreservationSpec`: it carries a clean
form, a logical handoff, both baseline and candidate files, and both boundary
images so that the backend can derive the other physical form. That derivation
belongs with the model/authority owner that already validates
`preservation_handoff_is_compatible`; the backend should compare and mutate
exact physical facts only.

Required correction:

- define one Git-neutral exact index form, including marker and lock stage-0
  presence/absence, path, blob identity, mode, stage, and accepted flags;
- pass explicit `clean_form` and `handoff_form` physical values plus the exact
  invariant boundary bytes;
- make the authority-owned preservation projection the sole conversion from
  the durable model handoff to those physical values; and
- bind both explicit forms in the authority proof.

The Git backend may validate canonical paths, regular-file shape, raw index
facts, and equality. It must not decide what `Baseline`, `Marker`, `Lock`,
`Boundary`, `Pre`, or `Staged` means or whether a pair is compatible with a
publication step.

One concrete replacement surface is:

```rust
struct GitRootManagedIndexForm {
    marker: Option<GitCandidateFile>,
    lock: GitCandidateFile,
}

struct GitRootManagedForm {
    marker: Option<GitCandidateFile>,
    lock: GitCandidateFile,
    index: GitRootManagedIndexForm,
}

struct GitRootPreservationSpec {
    attached_branch: String,
    attached_commit: String,
    managed_marker_path: String,
    clean_form: GitRootManagedForm,
    handoff_form: GitRootManagedForm,
    handoff_boundary: Vec<u8>,
}

enum GitRootPreservationGuard<'a> {
    NormalizedPreimage { sha256: &'a str },
    OtherwiseClean,
}

struct GitPreparedRootStash {
    normalized_image: GitPreservationImage,
}
```

`managed_marker_path` is validated once; every optional marker and index row
must use exactly that path. `GitRootPreservationGuard` replaces the overloaded
optional hash: normalize and `CreateStash` use `NormalizedPreimage`, while
post-stash restore and reset preparation use `OtherwiseClean`. The authority,
not Git, selects the form, guard, object, and durable phase combination.

### P1-2 — the boundary is safely invariant only after its real Git role is frozen

Sections 2 and 4 say native Git stash, checkout, and index operations do not
consume `.git/info/exclude`. The mutation claim is correct; the consumption
claim is not. Current preservation capture configures status with
`include_ignored(false)`, and native stash uses `include_ignored: false`.
Status and stash therefore consult the managed boundary to partition untracked
from ignored paths even though they never write the file.

Leaving the boundary byte-for-byte at the durable handoff is still the smaller
and safer design, but for the opposite reason: it freezes the ignore policy
used by normalized-image projection and stash creation. The amendment must say
that explicitly. The full-root observer must bind the same boundary bytes to
the ignored/untracked classification before every normalize step and again at
`CreateStash`.

Required evidence must cross a path whose classification differs between the
baseline and candidate boundary. It must prove that the durable handoff form,
not the attached commit's alternate boundary, controls image/stash membership;
that ignored work survives; and that a boundary change at any partial phase is
ambiguity before the next mutation.

### P1-3 — durable preimage protection has a gap at native stash creation

Section 5.3 requires the normalized preimage hash before every normalize
mutation, then starts its next rule after stash creation. It does not require a
fresh normalized-image projection at the `CreateStash` before state. User work
can change after `NormalizeIndex` is durably completed and before the native
stash call. Checking only the earlier normalization mutations does not bind
the stash to the durable preimage.

The `CreateStash` observer must reproject the complete normalized image under
the exact invariant boundary and require the durable `preimage_sha256`
immediately before it can issue `Before`. Post-stash observation must retain
the stated decoded-image equality and otherwise-clean live check. The stale
preimage matrix must include changes in staged, unstaged, untracked, ignored,
and raw non-UTF-8 unrelated paths in the window after `NormalizeIndex`.

### P1-4 — the provisional manifest and 950/1,200 ceilings cannot satisfy the gate

The document calls its lists provisional even though §10 and the exit gate
require an exact mechanically confirmed manifest before GO. Mechanical phase
inventory finds omitted constructor and fixture owners:

- `model/v1/validate/tests.rs` matches the removed `RestoreRoot` phase;
- `v1_lifecycle/tests/fixtures.rs` matches the removed `NormalizeRoot` phase;
- exact phase serialization changes `model/v1/tests.rs`;
- the exact root graph changes `v1_lifecycle/tests/prefixed_preservation.rs`;
  and
- the current unknown-field fixture literally contains `phase:
  normalize_root`.

The single proposed `<500`-line Git owner is also expected to implement
canonical no-follow leaf acquisition and replacement, virtual normalized-image
projection, raw index subset observation/rewrite, dirt partitioning, phase
classification, and checked mutation. Current `preservation.rs` is already 788
lines and `contract.rs` is 714 lines. Adding all shared mechanics there risks
both crossing the 1,000-line review trigger and recreating a Git preservation
god file. One `<500` production owner and one `<500` evidence owner cannot
honestly cover the required matrix.

The replacement manifest and ceilings below are the minimum scope I can defend.
Any different file split requires another mechanical recount before GO.

## P2

### P2-1 — stash preparation has redundant output and an unrepresentable phase claim

`GitPreparedRootStash` contains both `normalized_image:
GitPreservationImage` and `user_dirty: GitPreservationDirtySummary`, although
the existing image already contains `dirty`. Two copies can disagree and add
no authority. Return the image once.

The text also says `prepare_root_preservation_stash` accepts the exact
handoff/clean mixture expected at the current durable phase, but the method
receives no phase, object, or goal. Before `BeginStash` there is no pending
phase: preparation should require the complete exact durable handoff (allowing
only true byte-equal no-ops) and virtually project the clean image. Later
partial states belong exclusively to the phase-specific observer.

### P2-2 — one managed marker identity is not closed

The spec carries `clean_form.marker`, `candidate_marker`, and a logical
handoff, but only says marker paths must be normalized files below the fixed
marker directory. It does not require every form to describe the same managed
candidate-marker path. Without that equality, a caller could classify one
marker as managed while a change at the actual operation marker falls into
unrelated dirt.

Freeze one marker path derived from the durable candidate. Every clean or
handoff form must describe that exact path as present or absent. The backend
must reject a second path before reading or mutating the repository.

### P2-3 — “byte-for-byte unchanged” overstates what an index lockfile commit preserves

Section 5.4 says the index step leaves every unrelated staged entry
byte-for-byte unchanged. Rewriting `.git/index` through a lockfile may change
file-level encoding, stat cache fields, or extensions while preserving the
accepted semantic entry set. The accepted preservation image compares raw
path bytes, stage, mode, semantic flags, and object identity; it does not claim
serialized index-file identity.

The contract should require every unrelated canonical entry tuple and staged
content to remain exact, reject unsupported flags/extensions before mutation,
and verify the same tuple set after commit. If literal index-file byte identity
is intended, the proposed implementation and ceiling are not sufficient.

## Accepted architectural decisions

### Flattened phases are cohesive and minimal

Each new phase names the next independently checkable mutation, and every no-op
still crosses a durable edge from an exact `After` proof. Separate normalize,
prepare, and restore names are justified because they use different authority
contexts even when the target physical form is equal. Adding a second durable
root cursor would make phase/action identity ambiguous rather than simpler.

### The boundary should remain invariant

There is no reason to rewrite `.git/info/exclude`: it is outside the tracked
checkout and index, stash/reset do not mutate it, and every rewrite would add
another crash boundary. Its bytes must nevertheless remain part of every full
observation because they define ignored/untracked classification as described
in P1-2.

### The existing checked reset is honestly bounded

`set_branch_target_checked` requires a clean attached branch, uses a safe
checkout, moves the checked ref through a transaction, and verifies the final
branch/checkout. It can crash after checkout work and before ref commit, so it
is not atomic. P0.2 accurately treats any such mixed ref/index/worktree state
as ambiguity and neither force-repairs nor adopts it. Splitting the reset would
increase durable vocabulary without providing an existing independently safe
primitive for each internal Git write.

The settled-tree review must confirm that P0.2 wraps rather than weakens this
primitive: no `--force`, no cleanup of ignored/untracked work, exact full
before/after forms, and no retry from a mixed checkout.

## Mechanically required replacement manifest

This is the minimum exact P0.2 implementation manifest after applying P1-1.
The split keeps every new cohesion owner below 500 lines and prevents the
existing 788-line preservation module from absorbing unrelated policy.

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

`preservation_image.rs` owns the existing canonical raw image grammar and the
new virtual managed substitution. `preservation_root.rs` owns only spec
validation and step orchestration. Its `files.rs` and `index.rs` children own
canonical leaf and raw index mutation respectively. No Git file owns phase
adjacency or publication compatibility.

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

The existing no-prefix `journal_vocabulary.rs`, generic transition footprint,
resolver, and common constructor tests do not match a removed phase and need no
edit for this checkpoint. If implementation changes them, they must be added
before the edit rather than silently substituted.

## Credible stop ceilings

Historical initial-review proposal only: the provisional 950 production /
1,200 evidence ceilings were proposed to be replaced with the following.
These figures were superseded first by the interface-GO checkpoint above and
then by the controlling retained-review ledger below:

| Charge | Stop ceiling |
| --- | ---: |
| new/changed production | 2,100 lines |
| moved production | 450 lines |
| test/tool/document | 4,000 lines |
| production paths | 10 |
| evidence/tool/document paths | 20 |

All four new production cohesion owners and each new Git evidence owner remain
below 500 lines. `contract.rs` must remain below 950, the post-extraction
`preservation.rs` below 850, and no existing owner may cross 1,000 without a
new review. These are stop ceilings, not targets.

The ledger must separately add the shared Git charges to P1, recompute P1 and
R4b-P charged/unique unions, and update the program union before implementation.
No numeric package/program union can be accepted from the current provisional
document because it supplies none.

## Exit conditions for architecture GO

Architecture GO requires all of the following in the controlling amendment
and ledgers:

1. replace the logical Git handoff duplication with defined exact physical
   clean/handoff forms and one authority-owned projection;
2. state that the invariant boundary controls ignored/untracked classification
   and add the cross-boundary evidence;
3. require exact durable-preimage re-projection at `CreateStash`;
4. remove the duplicate dirty summary and close the single marker identity;
5. replace literal index-file byte identity with the exact supported semantic
   tuple contract, or specify and budget a literal-byte implementation;
6. freeze the exact manifests, ceilings, shared P1 charges, and package/program
   unions; and
7. retain the accepted flattened graphs and strict ambiguous-reset behavior.

Until those corrections are independently re-reviewed, P0.2 and P1 remain
paused.

## P0.2 settled-code architecture review — 2026-08-11

### Verdict

**HOLD.** No P0 finding exists because the v1 mutation route remains
production-disabled and P1 is still blocked. Three P1 findings prevent
settled-code architecture GO. There is no separate P2 or P3 finding.

The implementation preserves the accepted high-level ownership direction. Git
receives only explicit physical `C0`/`C1`/`H` forms and a physical step/guard;
no Git owner imports the model phase, publication-prefix, or publication-index
vocabulary. The public-in-core addition is exactly the frozen three methods,
and only the production backend and focused Git tests currently consume it.
The model still owns durable phase validity and reducer adjacency, so no normal
build v1 writer, migration, dispatcher, or P1 consumer was activated.

### P1-C1 — the claimed SHA-256 and index-extension contract is not implemented

`GitRootManagedIndexEntry` accepts both 40- and 64-hex object ids, and the
amendment requires both SHA-1 and SHA-256 repositories. The executing code is
nevertheless SHA-1-only:

- `preservation_root/index.rs::exact_commit` calls `git2::Oid::from_str`;
- `add_entry` calls the same SHA-1-default parser; and
- clean-form blob ids use `git2::Oid::hash_object`, also the default format.

The workspace enables `git2` only with `https` and `ssh`, not its unstable
SHA-256 support. The dependency's documented `Oid::from_str` limit is 40 hex
characters. A 64-hex fact therefore passes `validate_fact` but cannot be
derived or executed, and a SHA-256 commit cannot produce `C0` or `C1`.

The same index owner promises to reject unsupported index extensions before
mutation, but `validate_supported` inspects only per-entry flags. Nothing reads
or classifies index extensions before `Index::write`, and the evidence suite
contains no extension-survival or extension-rejection row. This is not enough
to establish that a lockfile rewrite cannot discard an unsupported optional
extension.

Required correction: use repository-format-aware object-id parsing, hashing,
tree derivation, and index mutation with real SHA-1/SHA-256 fixtures; inspect
and reject every unsupported index extension before writing, and prove the
accepted extensions plus every unrelated canonical entry tuple survive.

### P1-C2 — the managed-file owner is not an anchored no-follow single step

`preservation_root/files.rs` checks parents with `symlink_metadata`, then uses
ordinary path-based `create_dir_all`, temporary-file creation, `rename`, and
`remove_file`. Two contract violations follow:

- `require_real_parents` creates a missing marker parent. A phase that names
  only the marker leaf can therefore commit directory creation and leaf
  replacement at separate crash boundaries. A crash can leave the unmodelled
  empty directory while the durable phase is unchanged.
- the second parent check occurs before path-based `rename`; a concurrent
  parent replacement after that check can redirect the rename outside the
  already validated directory. There is no directory-handle-relative
  no-follow mutation or equivalent anchored primitive.

The focused tests do not replace a parent or leaf during the checked mutation.
Required correction: either require the canonical parent to exist and reject
absence, or journal its creation as a separate physical step; perform leaf
create/replace/remove relative to an already opened, verified directory with
no-follow semantics, and exercise parent/leaf replacement at the final
pre-mutation boundary.

### P1-C3 — the mandatory physical and restart evidence matrix is absent

The root-preservation filter runs only seven focused Git tests plus one model
collision test. The Git tests use one SHA-1 fixture, one marker identity, one
handoff form, and before/after faults around a marker mutation. They do not
close amendment §9's required matrix for:

- all allowed handoff pairs, both root owners, descendant `C0`, restore `C1`,
  fabricated/stale clean forms, non-commit objects, and partial `H` mixtures;
- every normalize/restore/prepare/index row, all degenerate no-ops, wrong or
  reordered steps, later-object advance, detached/wrong refs, and foreign
  native state;
- cross-boundary ignored/untracked classification, raw non-UTF-8 live paths,
  SHA-1/SHA-256 repositories, forbidden flags/extensions, executable/
  symlink/directory shapes, and parent/leaf races; or
- multi-owner preflight no-mutation and the required interruption boundary
  inside the checked reset between checkout work and the ref transaction.

The only fault seam is `Before`/`After` around the entire selected mutation;
the reset test executes a successful reset and never constructs or injects an
internal partial checkout. Passing unit tests therefore does not prove the
frozen ambiguous-reset claim or the full physical contract. These rows must be
added within the already reserved six focused Git evidence owners; discovering
that a new owner is necessary reopens the manifest before editing.

### Manifest, budget, and cohesion measurements

The implementation itself stays inside the numeric and ownership envelopes:

- exact manifest: 10/10 production paths and 20/20 evidence paths, with no
  P0.2 symbol or phase-spelling owner found outside the frozen set;
- production: 1,243/2,100 net lines and 410/450 moved lines, reconciled from
  the pre-implementation file-size snapshot; shared Git is 1,100 net plus 410
  moved, and model/transition integration is 143 net;
- focused code evidence before retained reviews: 544 Git lines and 236
  model/transition lines, within the separate 2,400 and 800 reservations; and
- retained documents measured 1,753/3,200 at interface GO; settled-code reviews
  and this reconciliation add 258 lines: 133 architecture, including this
  reconciliation, and 125 state. The measured combined retained-document charge
  is 2,011/3,200, without borrowing test scope.

Current production owner sizes are: `contract.rs` 835/950,
`preservation.rs` 378/850, `preservation_image.rs` 491/500,
`preservation_root.rs` 405/500, `files.rs` 234/500, and `index.rs` 252/500.
The existing model/transition owners are also below 500 lines. No god-file or
path-count exception is required.

The final controlling retained-review ledger is arithmetically consistent:
documents/reviews/control is 3,200, P0.2 evidence is 6,400, R4b-P charged
evidence is 24,300, and the unique-once program evidence ceiling is 33,600.
Production/movement and path ceilings remain 12,000/2,950, 150/146 charged,
97/102 P-package unique, and 141/145 program unique. These correct budgets do
not substitute for the missing contract and evidence.

### Read-only verification

- `cargo test -p gwz-core root_preservation --lib`: 8 passed;
- `cargo test -p gwz-core --lib`: 1,008 passed;
- `cargo fmt --all -- --check`: passed; and
- `cargo clippy -p gwz-core --all-targets --all-features -- -D warnings`:
  passed.

P0.2 and P1 remain blocked until P1-C1 through P1-C3 are corrected and both
independent settled-code re-reviews return GO.

## Corrective-interface architecture re-review — 2026-08-11

### Verdict

**GO.** No P0, P1, P2, or P3 finding remains in the corrective interface.
Amendment 2 §12 and the reconciled reverse-lifecycle, transition, change-budget,
and refactor controls are sufficient to authorize the bounded P0.2 remediation.
This does not close the settled-code HOLD, authorize P1, or accept code that has
not yet implemented and passed the complete corrective evidence matrix.

This verdict covers the final §12 bytes, including the added `sha1` dependency,
full SHA-1/SHA-256 trailer recomputation, cache-tree framing and invalidation
rules, and the 350-line `index_format.rs` cohesion cap. It supersedes the
6,400/24,300/33,600 retained-review checkpoint above; that earlier paragraph
remains review history rather than the controlling corrective ledger.

### Capability and race boundary

The reviewed same-major `cap-std`/`cap-fs-ext` 4.0.2 API line supplies the
required capability-directory, `open_dir_nofollow`, no-follow metadata/open,
handle-relative rename/unlink, cross-platform directory identity, and parent
sync building blocks. Traversing one component at a time and retaining the
verified parent handle closes pathname redirection; the final same-handle
source/identity observation makes every pre-linearization parent or leaf
replacement a no-mutation rejection.

The contract is deliberately capability containment plus one filesystem
linearization point, not destination CAS. It does not falsely promise to
preserve an adversarial writer's simultaneous post-observation replacement.
The outside-root sentinel and injected parent/leaf replacement rows make that
narrower contract observable. Missing, symlinked, replaced, or non-directory
parents are ambiguity, never an implicit `mkdir`; the required real-Git rows
must also prove native stash leaves the already-existing marker parent live in
both object formats. A different capability major or stronger CAS requirement
reopens interface review.

### Object-format and index boundary

`git2` 0.21 exposes `Repository::object_format`, `Oid::from_str_ext`, and
`Oid::hash_object_ext` behind `unstable-sha256`; the control correctly accepts
the upstream experimental dependency explicitly and requires real SHA-1 and
SHA-256 preparation, index, stash, and reset rows. Exact lowercase 40/64-byte
validation before the format-aware APIs closes their abbreviation acceptance.

The new raw-index owner is cohesive: it validates `DIRC` v2-v4 framing,
format-dependent OID width, entry semantics, extensions, and an independently
recomputed SHA-1/SHA-256 trailer before proof and after rewrite. Supporting only
`TREE` is honest because it is cache rather than preservation authority. Exact
node framing plus mandatory invalidation of the root and every managed-path
ancestor prevents a post-write cache from claiming the pre-write tree; all
other named, lowercase-mandatory, unknown, malformed, or checksum-invalid
extensions fail before mutation. Unrelated canonical entry tuples remain the
preserved authority.

### Manifest, cohesion, and ledger reconciliation

The corrective manifest adds exactly two production paths—`gwz-core/Cargo.toml`
and `preservation_root/index_format.rs`—to the accepted ten, and exactly the
workspace and member `Cargo.lock` files to the accepted twenty evidence paths.
Both locks are necessary because the integrated workspace and independently
released core resolve separately. No additional lifecycle, model, recovery, or
test owner is implied.

The arithmetic is exact:

- P0.2 is 6,700 evidence lines across 12 production and 22 evidence paths;
- the duplicate shared-Git charge makes P1 4,500 evidence and 15/16 paths;
- R4b-P is 24,900 evidence with 154/150 charged and 99/104 unique paths; and
- the unique-once program is 33,900 evidence with 143/147 paths.

The 300-line evidence increase is counted once in P0.2, again in P1's charged
shared row, and once program-wide. The at-most 540 net-production and 1,900
test/tool remediation estimates fit the retained 2,100/2,700 component stops.
Current `files.rs`/`index.rs` sizes of 234/252 leave their sub-450 limits
credible, the format parser has a focused sub-350 owner, and the existing test
owners have enough aggregate sub-500 capacity for the closed table-driven
matrix. Any owner, path, lockfile, or ceiling breach stops for recount and
review rather than borrowing the 3,200-line document reserve.

Remediation may now proceed inside §12. P1 remains paused until the corrected
settled tree passes both independent code re-reviews and every required
formatting, lint, real-Git, fault/restart, protocol, and retained-reader gate.

## Durable parent corrective re-review — 2026-08-11

### Verdict

**NON-GO.** There is no P0, P2, or P3 finding. Two P1 interface/ownership
findings remain. This verdict supersedes the preceding corrective-interface GO
for the newly revised durable-parent seam; the preceding section remains review
history.

The revised root graphs and backend boundary otherwise pass architecture
review. `RestoreParent` is correctly placed between `RestoreLock` and
`RestoreMarker` in both graphs, has one derived `MarkerParentDirectory` object,
and maps to the exact stash/reset source and `Handoff` goal forms. Required and
optional parent forms are derived rather than caller supplied. For an absent
`H` marker, missing and real-empty parents are correctly equivalent and the one
expected later source marker leaf is permitted without creating or removing a
directory. Foreign children, a second marker, temporary residue, symlinks,
non-directories, inaccessible entries, and reordered later objects fail closed.

The mutation boundary also contains no concealed `mkdir`: only required-parent
`RestoreParent`, only for a present `H` marker with an absent parent and absent
clean-source marker, may perform one handle-relative `create_dir("markers")`
through the retained no-follow `gwz.conf` capability. It never uses
`create_dir_all`. The reobservation, no-follow open, identity/emptiness checks,
and child/parent ordering are compatible with the accepted capability-
containment contract and make no false destination-CAS or compound-atomicity
claim.

### P1-DP1 — barrier completion is not representable in the closed lifecycle

Amendment 2 lines 117–119, 506–523, and 596–599 correctly state the intended
rule: raw structural `After` is insufficient for a required parent; the checked
executor must repeat both persistence barriers before a completion proof may
advance the phase. The accepted service protocol cannot currently express that
causal rule. Its observer has only `Before`, `After`, and `Ambiguous`; the
generic lifecycle maps a completed observation directly to completion, while
`BoundExecutionAttempt` is diagnostic rather than outcome authority. The
controlling transition design likewise says `Completed` records progress and
the executor cannot create an outcome proof.

Concretely, `authority/resolver.rs:81-84` calls `completed(...)` for every
`ExactObservationFact::Completed` regardless of whether a matching successful
attempt exists. A structurally present required parent therefore has the same
fresh observation before and after the barriers. Either the first observation
can advance without executing them, or the interface needs a special causal
resolution rule that has not been specified. Merely requiring the backend's
`AlreadyComplete` path to run barriers does not cause the service to select
`Execute` for an already-`After` object.

The corrective manifest compounds the gap. Section 12.5 retains only the ten
original production owners plus `Cargo.toml`, `repository_support.rs`, and
`index_format.rs`, and explicitly adds no lifecycle owner. None of the exact
authority observer/resolver/execution owners needed to select, bind, and
resolve this exceptional attempt is present; the generic resolver evidence is
also excluded. The correction must define one exact closed protocol—for
example, required-parent structural `After` resolves to the same checked
physical action until a fresh, matching successful barrier attempt is bound,
after which the fresh `After` may issue the phase proof—while optional-parent
`After` remains ordinary proof-only. It must reconcile that exception with the
transition design, name every production and evidence owner, and recount the
manifest before implementation is authorized.

### P1-DP2 — the Windows direct-create durability claim has no barrier

Section 12.1 says the direct directory creation applies two platform
persistence barriers and that the Windows build uses the existing
`durable_fs` policy. That policy does not provide a persistence operation for
this mutation. `durable_fs.rs:24-35` implements Windows `sync_dir` as `Ok(())`;
its documented Windows barrier is `MOVEFILE_WRITE_THROUGH`, used by the durable
rename implementation. `RestoreParent` instead performs direct
`create_dir("markers")`, so neither the child nor parent call supplies that
barrier on Windows. Platform tests cannot turn the two no-ops into the durable
publication claimed by the interface.

The amendment must either specify a real reviewed Windows publication primitive
for this one-level directory creation, with its failure/restart semantics, or
revise the physical protocol to use an operation for which the existing policy
actually supplies a barrier. Any necessary `durable_fs.rs` or platform-helper
owner and focused Windows evidence must be added to the exact manifests and
recounted. The current manifest contains neither owner.

### Ledger and gate

The stated arithmetic is mechanically consistent for the currently listed
paths: P0.2 is 2,100/450/7,000 with 13/22 paths; P1 is
3,150/1,500/4,800 with 16/16; R4b-P is 12,150/2,950/25,500 with 156/150 charged
and 100/104 unique paths; and the program union is 18,250/34,200 with 144/147
paths. The reverse-lifecycle and change-budget controls carry those same
figures. They are not yet controlling ceilings for remediation because both P1
findings require newly named owners or an equally explicit proof that no such
owner changes are needed; either outcome requires a mechanical manifest and
ledger reconciliation.

macOS/Linux child-plus-parent handle flushing, the restart/fault matrix, all
four marker-presence combinations, optional-parent no-op behavior, and
race/replacement rows are otherwise adequate evidence requirements. P0.2
remediation remains unauthorized, and P1 remains paused, until P1-DP1 and
P1-DP2 are corrected and both independent interface reviewers return GO on the
same final bytes and reconciled controls.

## Durable parent rename-publish corrective re-review — 2026-08-11

### Verdict

**GO.** The current Amendment 2 and reconciled controls close P1-DP1 and
P1-DP2. No P0, P1, P2, or P3 finding remains in the durable parent corrective
interface. This section supersedes the immediately preceding NON-GO; that
section remains review history for the rejected direct-create protocol.

This verdict authorizes only the bounded P0.2 remediation. It does not accept
unimplemented code, close the settled-code review, activate v1, or release P1
before both independent corrected-code re-reviews and the complete required
matrix pass.

### Physical ownership and containment

`RestoreParent` remains the one durable phase between `RestoreLock` and
`RestoreMarker` in both root graphs. It is the only step allowed to stage or
publish a directory. The final `markers` directory is never created directly:
the checked executor creates or reuses one deterministic, real, no-follow,
empty staging directory under the retained `gwz.conf` capability, then performs
one no-replace rename. No other step may create a parent and no path uses
`create_dir_all`.

The staging name is derived from the normalized marker path, exact source/goal
forms, and attached/restore commits rather than caller input. Missing staging
and the one exact empty staging directory are the two `Before` substates.
Malformed, linked, non-empty, duplicate, matching-prefix, foreign, or
simultaneous final-and-staging forms are `Ambiguous`; no unjournaled cleanup is
permitted. This is sufficient physical ownership without introducing another
durable cursor.

Linux publication is descriptor-relative `renameat2(RENAME_NOREPLACE)` and
macOS publication is descriptor-relative `renameatx_np(RENAME_EXCL)` against
the retained parent. Unsupported Unix targets reject. The temporary is synced
before publication and `gwz.conf` afterward, so neither source nor destination
resolution can escape the capability boundary and destination replacement is
not silently permitted.

Windows now binds both ends of publication rather than relying on a pathname
rename. The checked call pins the root and `gwz.conf`, opens the exact temporary
directory no-follow with delete access, backup semantics, open-reparse-point,
and write-through flags, and renames that opened object with
`SetFileInformationByHandle(FileRenameInfo)`. `RootDirectory` is the pinned
`gwz.conf` handle, the target is the single relative name `markers`, and
replacement is false. Microsoft's `FILE_RENAME_INFO` contract makes an
existing target an error and defines the relative target against the supplied
directory handle; its file-caching contract states that a write-through request
flushes metadata changes such as rename. The source handle also removes the
remaining source-leaf exchange window. The unchanged Windows `sync_dir` no-op
and path-based `MoveFileExW` helper are not used by this protocol, so no
`durable_fs.rs` edit is implied.

This remains the accepted containment-plus-linearization contract, not a false
portable destination CAS. A concurrent exact final directory observed before
staging is ordered before GWZ and may be adopted only with no staging residue.
A destination race after staging leaves both names and is `Ambiguous`; a
pre-publication parent/source replacement either cannot occur while pinned or
fails exact reobservation. Post-linearization external interference remains
outside the stronger CAS guarantee already rejected by the interface.

### Durability, restart, and causal authority

On Unix, final presence in the creation-needed direction is
`AfterNeedsDurability`, including after a crash between rename and parent sync.
It maps to a bound
`ExactObservationFact::PreservationDurabilityPending`, not ordinary
`Completed`. With no attempt, the resolver selects the exact persisted
physical action. The checked executor repeats the idempotent parent sync and
reobserves the complete goal. Only a fresh matching pending fact paired with
that exact successful attempt may issue the stash/reset phase-completion proof.
A matching failure returns its typed error without transition, while stale,
mismatched, or second attempts reject. A crash after the barrier but before the
record rewrite therefore repeats the barrier on the next invocation rather
than inferring durability from structure.

Windows has one synchronous write-through metadata operation, so a successful
handle-bound rename yields ordinary structural `After`; a crash before it
leaves missing or exact staging `Before`, and a crash after successful return
leaves the published goal. Optional parents and source-equals-goal established
parents remain ordinary proof-only `After` on every platform. Missing parent
after the durable edge, a wrong marker advance, a foreign child, or any mixed
form is `Ambiguous` and cannot be repaired by `RestoreMarker`.

The new pending fact is a closed, non-durable authority value bound to the same
record digest/location, owner, action, phase, cursor prefix, and physical goal
as the existing preservation proofs. It reconciles with the observe–execute–
reobserve service loop without letting executor output alone become outcome
authority. The five exact lifecycle production owners—resolver,
resolver/observation, resolver/execution, reverse preservation observation,
and reverse preservation execution—cover construction, selection, matching,
and consumption. The three named existing authority/attempt/reconciliation
test owners cover the causal matrix. There is no missing service, reducer,
model, recovery, or evidence-file owner.

### Evidence, manifests, and arithmetic

The required evidence closes the four marker-presence combinations,
missing/empty equivalence, deterministic staging restart, malformed/duplicate/
both-name residue, foreign children, outside-root sentinels, Linux/macOS
exclusive rename, Windows pinned handle and write-through behavior, faults
before and after staging/rename/Unix parent sync, pending with no attempt,
matching success/failure, crash before rewrite, and stale/mismatched/second
attempts. The existing real-Git SHA-1/SHA-256, index, reset-partial, platform
release, strict lint/format, full-core, protocol, and retained-reader gates
remain required.

The exact corrected manifest is 18 production and 25 evidence paths: the
accepted 10/20, three shared-Git/dependency owners, five lifecycle owners, two
lockfiles, and three existing lifecycle evidence owners. Low-level Unix and
Windows publication belongs in the already-listed `preservation_root/files.rs`;
required build features belong in the listed `gwz-core/Cargo.toml`. No hidden
production or evidence path is required by the reviewed protocol.

The reconciled arithmetic is exact:

- P0.2 is 2,650/450/8,200 with 18/25 production/evidence paths;
- P1 is 3,700/1,500/6,000 with 19/19 charged paths;
- R4b-P is 13,250/2,950/27,900 with 164/156 charged and 103/104 unique paths;
  and
- the program union is 18,800/35,400 with unchanged 144/147 paths.

The program path union remains unchanged because the resolver and lifecycle
evidence owners were already present in earlier checkpoints; the shared Git and
lifecycle rows are charged again to P1 but counted once program-wide. Amendment
2, the reverse-lifecycle control, and the change-budget ledger carry the same
figures. Any additional owner, unsupported platform/filesystem, API-contract
failure, or ceiling breach stops implementation for renewed manifest and
interface review rather than borrowing scope.

## Branchless durable parent corrective re-review — 2026-08-11

### Verdict

**GO.** No P0, P1, P2, or P3 finding remains in the current exact Amendment 2
or its reconciled controls. The branchless Windows correction closes the last
provenance gap in the preceding rename-publish review: a structurally exact
required empty parent is now causal durability-pending on every platform and
cannot advance without a fresh matching successful barrier attempt.

This section supersedes the preceding GO only where that review treated a
Windows final-only form as ordinary `After`. Earlier sections remain historical
records of rejected protocols. This GO authorizes the bounded corrective
implementation; it does not accept code, activate v1, release P1, or replace
the required independent settled-code re-reviews.

### One phase owns every physical restart form

`RestoreParent` remains the sole phase that may stage or publish a directory,
and remains exactly between `RestoreLock` and `RestoreMarker` in both root
graphs. It owns one deterministic empty staging name derived from sealed
operation facts. Final missing with staging absent or with that one exact real-
empty staging directory is `Before`. Final-only real-empty is
`AfterNeedsDurability`. Final plus staging, malformed or duplicate residue,
links, non-directories, children, foreign entries, and matching-prefix residue
are `Ambiguous`. No later phase cleans or repairs any of those forms.

The Windows barrier's two no-replace renames do not introduce a hidden durable
branch. The first successful rename leaves staging-only, which is already an
exact `Before` restart substate; the second leaves final-only, which is still
pending rather than completed. A crash before the first rename resumes normal
publication, a crash between renames resumes staging-to-final publication, and
a crash after the second rename repeats the round-trip barrier before the
record edge. No intermediate state needs a new field or cursor, and no
unjournaled cleanup is permitted.

The same ownership covers races. A staging collision before the first rename
leaves both names and fails closed. A concurrent final creation while staging
exists likewise leaves both names and is `Ambiguous`. An exact concurrent
final-only creation is re-barriered instead of being adopted as durable. A
matching execution failure paired with fresh `Before`, pending, or
`Ambiguous` state returns its bound diagnostic without transition or a second
same-invocation execution. Success with anything except fresh final-only
pending state rejects.

### Capability and platform durability boundary

Linux and macOS retain descriptor-relative exclusive rename under the opened,
identity-checked `gwz.conf` parent, using `renameat2(RENAME_NOREPLACE)` and
`renameatx_np(RENAME_EXCL)` respectively. The exact empty staging directory is
synced before initial publication. Final-only required-empty state then invokes
the idempotent retained-parent sync on every retry. Unsupported Unix platforms
reject rather than substituting a weaker pathname operation.

Windows pins the root and `gwz.conf`, opens the exact source directory no-follow
with delete access, backup semantics, open-reparse-point, and write-through,
and calls `SetFileInformationByHandle(FileRenameInfo)` with pinned `gwz.conf` as
`RootDirectory`, one relative name, and replacement disabled. Initial
staging-to-final publication uses that operation. The barrier reopens final by
exact handle and performs final-to-staging-to-final through the same absent
deterministic name. Both source identity and destination resolution are bound;
neither rename can be redirected through a replaced pathname.

The Windows API contract is sufficient for the declared boundary:
`FILE_RENAME_INFO` makes a present target an error and resolves a relative name
against the supplied directory handle, while `FILE_FLAG_WRITE_THROUGH` flushes
rename metadata generated through the opened handle. The existing no-op Windows
`sync_dir` and path-based `MoveFileExW` helper are not part of this protocol, so
`durable_fs.rs` correctly remains outside the manifest. Low-level platform code
belongs to the already-listed `preservation_root/files.rs`, and any needed
Windows feature selection belongs to the listed `gwz-core/Cargo.toml`.

The design still promises capability containment plus each platform's named
linearization point, not cross-platform destination CAS or compound atomicity.
Two Windows namespace mutations are disclosed and restart-classified; the
interface does not pretend that the round trip is one atomic rename.

### Closed causal authority without durable mode

Git `AfterNeedsDurability` maps to the preservation-only bound
`ExactObservationFact::PreservationDurabilityPending`. With no attempt, the
resolver issues the exact persisted `RestoreParent` physical action. The
executor reobserves before mutation, performs the Unix sync or Windows round
trip, and the service reloads the unchanged record and observes again. Only the
conjunction of a fresh exact final-only pending proof and its matching
`Success` attempt constructs the ordinary stash/reset phase-completion proof.

The attempt remains causal input rather than outcome authority: it is bound to
the same source digest/location, owner, action, phase, full cursor prefix, and
physical goal; failure does not transition; stale, mismatched, success-without-
goal, and second attempts reject. If the process stops after a successful
barrier but before the record write, invocation history disappears and the
next fresh pending observation executes the barrier again. Optional-parent and
already-established-parent cases remain ordinary proof-only `After` and never
enter this seam.

The five lifecycle owners in §12.5 are the exact closed production surface:
resolver, resolver observation, resolver execution, reverse preservation
observation, and reverse preservation execution. They cover construction,
physical-action selection, execution, attempt matching, diagnostic handling,
and completion-proof issuance. The three named existing lifecycle evidence
owners cover the causal attempt matrix. No reducer, durable model field,
service, recovery, or new test-file owner is concealed.

### Cross-document ownership and ledger

Amendment §§3.4, 5.4, 8, 12.1, 12.4, and 12.5 now agree on every-platform
pending classification, branchless restart forms, causal completion, platform
operations, evidence, and owners. The transition design explicitly identifies
`PreservationDurabilityPending` as the closed preservation-only exception to
its generic three-way sketch. The reverse-lifecycle control names the Unix
parent sync and Windows round-trip barrier. The refactor status retains the
correct P0.2/P1 gate, and the change-budget and reverse-lifecycle ledgers carry
the same exact values.

The manifests and arithmetic remain exact:

- P0.2 is 2,650/450/8,200 with 18/25 production/evidence paths;
- P1 is 3,700/1,500/6,000 with 19/19 charged paths;
- R4b-P is 13,250/2,950/27,900 with 164/156 charged and 103/104 unique paths;
  and
- the program union is 18,800/35,400 with 144/147 paths.

The existing Git filesystem/fault owners must prove initial publication, both
Windows round-trip rename boundaries, staging-only restart, second-rename
failure, both-name collision, Unix parent-sync retry, outside-root containment,
and all four marker-presence combinations. The three lifecycle owners must
prove pending with no attempt, matching success/failure over each fresh
physical class, crash before rewrite, and stale/mismatched/second attempts. All
existing object-format, raw-index, reset-partial, platform release, strict
format/lint, full-core, protocol, and retained-reader gates remain mandatory.
Any extra path, unsupported filesystem/API behavior, or ceiling breach stops
for renewed manifest and interface review.

## Unix `rustix` dependency/API re-review — 2026-08-11

**GO.** No P0, P1, P2, or P3 finding is introduced by the explicit target-Unix
`rustix` 1 dependency. With the `fs` API feature, rustix exposes
`fs::renameat_with` over `AsFd` directory arguments and
`RenameFlags::NOREPLACE`. Its Linux backend issues
`renameat2(RENAME_NOREPLACE)` and its Apple backend issues
`renameatx_np(RENAME_EXCL)`; an Apple runtime without the flagged API returns
`NOSYS`, which matches the already-approved fail-closed unsupported-platform
rule.

Passing the retained capability `Dir` descriptor as both source and destination
directory authority preserves the reviewed containment boundary: only the two
relative staging/final leaf names are resolved, no ancestor pathname is
reopened, replacement remains forbidden, and the same descriptor remains
available for the post-rename parent sync. This is an implementation mechanism
for the existing linearization point, not another mutation, owner, phase, or
durability claim.

“Only `fs`” is accepted as the only explicitly requested rustix API feature;
the crate's ordinary `std` default and Cargo feature unification already induced
by `cap-std`/`cap-fs-ext` are not represented as new preservation scope. The
direct dependency lives in the already-approved `gwz-core/Cargo.toml`, and both
already-manifested lockfiles remain the required resolution evidence. No
production/evidence path or line/path ceiling changes, so the controlling
18/25 P0.2 manifest and all previously reconciled budgets remain exact.

## Owner-only manifest corrective re-review — 2026-08-11

**GO.** No P0, P1, P2, or P3 finding remains in the owner-only amendment or
reconciled controls. The new `preservation_root/parent.rs` is a justified
production cohesion owner, not behavioral scope: deterministic staging-name
derivation, anchored marker-parent observation, Unix exclusive publication and
sync, and Windows handle-bound publication and round-trip barriers form one
marker-parent namespace responsibility. `preservation_root.rs` retains whole-
step classification/orchestration and durable-phase neutrality; `files.rs`
retains ordinary managed-leaf mechanics. The split therefore avoids both
platform duplication and a second phase-policy owner.

This section supersedes the 18/25 path count in the immediately preceding
dependency review; that review's dependency/API GO remains unchanged.

The projected post-split sizes are credible and restore the frozen limits:
`parent.rs` remains below 300 lines, `files.rs` falls from 485 to about 365, and
`preservation_root.rs` from 565 to about 475. The direct Unix `rustix` use and
Windows bindings move with the platform namespace responsibility; dependency
declarations remain in the already-approved `gwz-core/Cargo.toml`, so no
additional dependency or build owner is needed.

The evidence split is equally cohesive. Moving roughly 113 preservation-only
unknown-field rows from the 552-line mixed `tests.rs` into
`unknown_fields/tests/preservation.rs` leaves both focused owners below 500
without adding behavior or evidence lines. Its single module-registration edge
does not introduce another fixture, production surface, or policy authority.

The exact manifests become 19 production and 26 evidence paths. The focused
parent owner is shared and therefore charged once in P0.2 and once again in P1;
the focused unknown-field child is charged only in P0.2's model evidence row.
The mechanically reconciled path arithmetic is:

- P0.2: 19/26;
- P1: 20/19;
- R4b-P charged: 166/157;
- R4b-P unique: 104/105; and
- program unique: 145/148.

The production, movement, and evidence line ceilings remain
13,250/2,950/27,900 for R4b-P and 18,800/35,400 program-wide. Amendment 2, the
reverse-lifecycle control, and the change-budget ledger now carry the same
double-charged 166 production-path total. No stale owner, hidden path, line
budget increase, or cohesion exception remains. Implementation still must
demonstrate the projected file sizes and exact test extraction; any overrun or
new owner stops for renewed review.

## Preservation failure-diagnostic contract re-review — 2026-08-11

**GO.** The §8 clarification preserves rather than weakens the accepted
failure contract. `ExecutionDiagnostic::Failed` carries `ErrorCode`, message,
and optional internal detail so the complete attempt remains bound and
identity-distinct. The existing public `ModelError` has no detail field, and
both established preservation no-progress resolution and the shared
`attempt_failure` helper construct it from the exact diagnostic code and
message while deliberately ignoring detail.

Returning that same `ModelError` for a matching failed durability attempt is
therefore the existing service surface: no transition is written, no
same-invocation retry is authorized, and the original code/message pair is not
rewritten or decorated with internal text. Detail remains available only inside
the bound attempt for matching, replay rejection, and diagnostics; it does not
become durable record data or public protocol output.

This closes the previously implicit conversion without adding a model field,
message convention, protocol member, production owner, or evidence path. The
existing causal-attempt tests can assert exact code/message preservation and
detail non-disclosure within their current owners and ceilings. No manifest or
budget change is required.

## §13 pre-clean parent phase architecture re-review — 2026-08-11

### Verdict

**GO.** No P0, P1, P2, or P3 finding remains in Amendment 2 §13 or its
reconciled controls. `NormalizeParent` and `PrepareParent` are the minimal
durable repair for the newly proved direction. This section supersedes earlier
P0.2 line ceilings in this memo; the accepted §12 physical, dependency,
ownership, and diagnostic contracts remain unchanged.

### Minimal phase boundary and four-way closure

The failing row cannot be repaired in `NormalizeMarker` or `PrepareMarker`:
anchored leaf publication correctly refuses a missing parent, and allowing that
leaf step to create one would hide a namespace mutation and a crash boundary.
Nor may preparation reject the row, because a missing parent and a real-empty
parent are already equivalent exact forms when the source marker is absent.
One parent phase immediately before each action-specific marker phase is
therefore the smallest honest graph change. Separate stash/reset enum variants
are necessary because their durable action and cursor types are distinct; they
reuse one physical parent primitive rather than duplicate behavior.

The final classifier covers all four marker-presence directions:

- absent source to present goal accepts missing or real-empty source state,
  publishes only an empty parent when missing, and treats exact empty final as
  `AfterNeedsDurability`;
- absent source to absent goal preserves missing/real-empty equivalence as
  ordinary proof-only `After`;
- present source to absent goal accepts the real canonical parent containing
  exactly the expected later source marker, as well as the goal's missing or
  empty optional forms, without removing the parent; and
- present source to present goal accepts the already-established exact parent
  as ordinary proof-only `After`.

Thus no direction narrows absent-versus-empty equivalence, and no parent phase
writes marker bytes. The immediately following marker phase remains the sole
leaf writer. Both-name state, malformed/duplicate staging residue, a foreign
child, wrong marker position, replacement, link, or non-directory remains
`Ambiguous`; no cleanup or repair is smuggled into either new phase.

### Restart and causal authority

The creation direction reuses the already-reviewed deterministic full-form-
bound staging name, retained `gwz.conf` capability, Unix exclusive rename and
parent sync, Windows handle-bound write-through publication/round trip, and
`PreservationDurabilityPending` authority. Missing final with absent or exact
empty staging is `Before`; final-only exact empty is pending. A crash before
publication resumes `Before`, after publication resumes pending, and after the
record edge enters the marker phase with its required parent exact.

Only a fresh pending observation paired with the same bound successful attempt
may advance. Failure returns the existing exact code/message surface without a
transition or same-invocation retry; stale, mismatched, success-without-goal,
and second attempts reject. The proof already binds action, phase, record
digest/location, full cursor prefix, source/goal forms, commits, and handoff, so
the two new enum values need no durable mode bit or widened proof type.

Non-root, `NoCandidate`, `EvidencePending`, action-free, and short graphs remain
unchanged and reject the new phases. A root stash/reset writes its respective
parent phase as the initial pending action before physical mutation. Reducer
successors are exactly parent-to-marker; no compound parent-plus-marker edge or
skipped in-memory cursor is introduced.

### Ownership, evidence, and provisional reserve

No new path is required. `journal.rs` owns the two serialized values;
preservation validation and reducer owners own graph admission and adjacency;
the existing authority observer/executor/resolver owners bind and consume the
direction-neutral parent action; `preservation_root.rs` owns whole-root
direction classification; and the focused `parent.rs` continues to own only
anchored parent namespace mechanics. This preserves the accepted separation
between phase policy, whole-step classification, parent mutation, and marker
leaf mutation.

The existing model, validation, unknown-field, prefixed-preservation, G15
observation/mutation/fault, and causal reconciliation evidence owners are the
right places for the added rows. Their required matrix includes both spellings,
root-only admission, exact predecessors/successors, all four presence
directions, missing and empty source forms, staging-only restart, pending
success/failure/restart, collisions/residue, wrong-phase authority, and
unchanged short-graph/v0 behavior. No protocol generator, migration, recovery,
or new evidence owner is implied. Implementers must refactor table-driven rows
in place where an existing focused file is near 500 lines; inability to retain
the frozen cohesion caps stops for a new owner review rather than silently
overrunning them.

The provisional line reserve is conservative but honest: it increases shared
Git production/evidence for the physical four-way matrix, model/transition
production/evidence for two durable enum/graph rows, and document reserve for
the renewed interface and settled-code history. It adds no path and does not
borrow document capacity for code or tests. The arithmetic is exact:

- P0.2: 3,100/450/11,100 with 19/26 paths;
- P1: 4,000/1,500/6,600 with 20/19 charged paths;
- R4b-P: 14,000/2,950/31,400 with 166/157 charged and 104/105 unique paths; and
- program union: 19,250/38,300 with 145/148 paths.

The duplicate P1 charge includes only the shared Git and lifecycle rows; the
program union counts them once. Amendment §13, the transition design,
reverse-lifecycle interface, change-budget ledger, and refactor status agree on
the new phases, pause gate, paths, and figures. These remain stop ceilings, not
permission to exceed owner caps or to resume P1 before both §13 interface
reviews, implementation, complete evidence, and independent settled-code
re-reviews are GO.

## §13 settled-code review — 2026-08-11

### Verdict

**NON-GO.** Three P1 findings and one P2 control finding remain. There is no
P0 finding because the preservation observer/executor still return the
production-disabled `MergePhaseUnsupported`/not-implemented results, and there
is no P3 finding. The implemented parent state machine, capability containment,
raw-index parser, and reducer graph are otherwise directionally consistent with
the accepted interface, but passing tests do not close the defects below.

### P1-SC1 — an index-only unowned marker is accepted as user work

The exact-handoff predicate does not reject a second marker that exists only in
the raw index. `preservation_root/index.rs` lines 173-203 filters the index only
for the selected marker and lock facts. `preservation_root.rs` lines 383-412
therefore calls that result an exact full form when the selected worktree marker,
lock, parent, and boundary match. `preservation_image.rs` lines 63-85 excludes
only those same two selected paths from user dirt, so an additional staged
`gwz.conf/markers/other.yaml` with its worktree leaf absent is captured as
ordinary unrelated work instead of rejected.

That state is neither the exact `H` nor an allowed user path under Amendment 2's
single-marker authority. The current second-marker evidence in
`root_preservation/observation.rs` lines 400-498 proves only invalid committed
tree entries; the parent tests prove visible worktree children. Neither reaches
the raw-index-only form.

Required correction: raw-index validation must reject every marker-directory
entry other than the one exact selected marker path before preparation or
mutation, and focused evidence must construct the index-only second marker,
prove preparation/observation rejection, and prove identical index/worktree/ref
bytes afterward.

### P1-SC2 — the causal seam exceeds the frozen 19-path production manifest

The five listed lifecycle owners contain the causal implementation, but the
settled surface also exports P0.2-only symbols through four unlisted production
paths:

- `v1_lifecycle/authority/observe/reverse.rs` line 10 exports
  `preservation_durability_fact`;
- `v1_lifecycle/authority/observe.rs` lines 18-21 exports it again;
- `v1_lifecycle/authority.rs` lines 615-623 exports it from the authority
  surface; and
- `v1_lifecycle/reverse.rs` line 17 exports
  `preservation_durability_diagnostic`.

Those names implement and expose the new
`PreservationDurabilityPending`/parent-barrier seam; they are not incidental
generated output. Amendment 2 §§10, 12.5, and 13.2 say that no unlisted path may
be edited and retain an exact 19-path production manifest. The actual P0.2
surface is therefore at least 23 production paths unless the exports are moved
inside already-approved owners. Numeric headroom cannot waive this path stop.
The manifest and all duplicated P1/R4b-P/program path counts must be recounted
and independently accepted if the four paths remain.

### P1-SC3 — mandatory object-format, flag, causal-phase, and platform evidence is incomplete

The focused suite has a real SHA-256 repository test, but
`root_preservation/observation.rs` lines 113-120 proves only preparation of an
exact handoff. Amendment §§12.2 and 12.4 require both SHA-1 and SHA-256 to run
the complete preparation, managed-index mutation, stash, and checked-reset
rows. No SHA-256 test executes those later operations.

The raw-index parser rejects stage conflicts, assume-valid, extended flags,
skip-worktree, intent-to-add, unknown extensions, malformed framing, and bad
checksums. Evidence covers versions 2-4, one stage conflict, the named extension
signatures, a checksum mismatch, and `TREE` invalidation, but no focused test
constructs assume-valid, skip-worktree, intent-to-add, or an unknown extended
flag. Thus Amendment §9.11 and §12.4 row 4 remain unproved.

The lifecycle causal matrix in `dispatcher_reconciliation.rs` lines 245-283
and its helpers at lines 294-349 hard-code stash `RestoreParent`. It does not
cross the two new durable identities `NormalizeParent` and `PrepareParent`
through no-attempt execution, matching success/failure, stale/mismatched/second
attempts, crash-before-rewrite replay, and wrong-phase authority as §13.2
requires. Unknown-field evidence likewise exercises `normalize_parent` but not
the reset action's `prepare_parent` identity.

Finally, the checked-in release workflow runs Ubuntu x86 and Windows, while
§12.1 requires Windows, macOS, Linux x86, and Linux ARM release builds and
restart/fault evidence. The Windows-only round-trip test exists at
`root_preservation/faults.rs` lines 453-492, and the generic publication faults
also compile on Windows, but no current artifact proves the full mandated
platform matrix. A local `x86_64-pc-windows-msvc` check could not substitute:
it stopped in `libz-sys` because this macOS environment has no Windows C SDK.

Required correction: extend the already-approved focused owners with the
missing SHA-256 end-to-end, forbidden-flag, both-new-phase causal, unknown-field,
and release-platform rows. If platform evidence or readable matrices require a
new file, stop for another manifest review rather than exceeding the frozen
owner/path limits.

### P2-SC4 — controlling status text still says §13 is unreviewed and unimplemented

Both independent §13 interface memos now record GO and the settled code is
present, but the controlling status text still says the interface review is
pending and implementation is paused in Amendment 2 lines 3-7,
ReverseLifecycleInterface lines 5-8 and 813-815, TransitionDesign lines
1187-1192, ChangeBudget lines 5-12 and 800-803, and Refactor lines 2328-2338.
The document checker passes because it does not assert these lifecycle-status
sentences. Reconcile them to the actual checkpoint history while retaining the
correct rule that P0.2/P1 remain paused until both settled-code re-reviews are
GO.

### Accepted mechanics, ownership, and ceilings

The settled implementation adds only the two §13 enum values and exact
parent-to-marker reducer edges; it adds no cursor field, compound action, or
protocol field. Non-root/no-candidate short graphs remain unchanged. The
phase-neutral Git classifier at `preservation_root.rs` lines 294-346 implements
all four source/goal marker-presence directions and uses goal-first ordinary
`After` except for absent-to-present final-only
`AfterNeedsDurability`. Missing/staging-only, final-only, both-name, residue,
foreign-child, and wrong-marker forms have the accepted classifications in the
inspected tests.

`parent.rs` retains and identity-checks `gwz.conf`, creates only the exact empty
staging directory, uses a no-replace handle-relative rename, and never creates
the final parent directly. Unix syncs the staging and retained parent; Windows
uses a no-follow, delete-capable, backup-semantics, open-reparse-point,
write-through source handle and a pinned `RootDirectory` for both branchless
round-trip renames. The only production `create_dir` in the root-preservation
owners is the staging creation at `parent.rs` line 96; there is no
`create_dir_all`, hidden marker-parent creation, or parent-plus-leaf mutation.
The leaf owner repeats source and parent identity through the retained
capability, and the index owner uses a lockfile write followed by raw semantic
verification.

The object-format-aware exact OID code and `DIRC` v2-v4 parser recompute SHA-1
or SHA-256 trailers, reject unsupported extensions, validate `TREE`, and
reverify unrelated tuples after mutation. Reset partials cover ref/index/
worktree mixtures and reject without retry mutation. The generic causal
resolver correctly requires a fresh pending goal plus its bound success,
projects failed attempts to exact code/message without leaking detail, and
rejects success without a pending goal. The P1 observer and executor remain
explicitly disabled.

No actual owner reaches the 1,000-line cohesion trigger. The principal sizes
are `contract.rs` 837, `preservation.rs` 455,
`preservation_image.rs` 491, `preservation_root.rs` 499, `files.rs` 297,
`index.rs` 352, `index_format.rs` 341, and the new `parent.rs` 351. Every new
focused production and evidence owner is below 500 lines; the existing
536-line resolver remains cohesive and below the general trigger. Generated
protocol artifacts are unrelated to P0.2 and contain none of the new phase or
causal symbols. The 3,100/450/11,100 numeric ceilings and duplicated
4,000/1,500/6,600 P1, 14,000/2,950/31,400 R4b-P, and
19,250/38,300 program ceilings reconcile on paper, and the retained document
delta remains under its 5,200-line reserve. The unlisted production exports,
however, prevent accepting the frozen 19/26 path report or closing the package
accounting.

### Verification

- `cargo test -p gwz-core root_preservation --lib`: 33 passed;
- `cargo test -p gwz-core --lib`: 1,033 passed, 0 failed, 1 ignored;
- `cargo fmt --all -- --check`: passed;
- `cargo clippy -p gwz-core --all-targets --all-features -- -D warnings`:
  passed;
- `python3 gwz-core/scripts/checks/check_merge_docs.py`: passed, 87 assertions;
  and
- `cargo check -p gwz-core --target x86_64-pc-windows-msvc --lib`: not run to
  Rust completion because `libz-sys` could not find the Windows C SDK headers.

P0.2 and P1 remain blocked. Settled-code architecture GO requires closing
P1-SC1 through P1-SC3, reconciling P2-SC4, rerunning the complete platform and
quality gates, and obtaining both independent settled-code reviews with no
open P0-P2 finding.

## §13 corrected-tree settled-code architecture re-review — 2026-08-11

### Verdict

**GO.** The corrected tree closes P1-SC1 through P2-SC4. No P0, P1, P2, or P3
finding remains in the local production-disabled P0.2 slice. This verdict does
not activate P1: the declared native release matrix remains a separate
committed-and-pushed-tree gate before P1 consumption or release.

### Closure of P1-SC1 — raw index-only foreign marker

`preservation_root/index.rs` lines 173-198 now validates the complete raw index
marker namespace before comparing the selected marker and lock. Any entry
equal to `gwz.conf/markers` or below that directory, other than the exact
selected marker path, returns `PreservationEvidenceMismatch`. The same
validation runs before an index rewrite and again on the verified post-write
index, so the check is not confined to initial preparation.

`root_preservation/stash.rs` lines 74-151 constructs the formerly missing
case: it stages `gwz.conf/markers/other.yaml`, removes the worktree leaf, and
then proves that preparation rejects, observation cannot authorize the first
parent step, checked execution rejects, no stash exists, and exact
ref/index/managed-worktree/boundary bytes remain unchanged. The neighboring
rows exercise assume-valid, skip-worktree, intent-to-add, and an unknown
extended flag through the same pre-mutation assertion. P1-SC1 is closed.

### Closure of P1-SC2 — manifest and accounting

Amendment 2 §12.5 and §13.3 now charge all four causal export surfaces rather
than treating them as incidental P0 ownership. The frozen P0.2 production
manifest is 23 paths and its evidence manifest is 26 paths. The reverse
lifecycle ownership table enumerates the same 23 P0.2 paths and 24 P1 paths,
including `authority.rs`, `authority/observe.rs`,
`authority/observe/reverse.rs`, and `reverse.rs`.

The duplicated and unique ledgers reconcile to P0.2 **23/26**, P1 **24/19**,
R4b-P **174/158 charged** and **104/106 unique**, and program **145/149**.
The program evidence increment is explicitly P3's
`gwz-py/src/tests/test_client.py`; its corrected `MergeResponse.record: None`
fixture is present and its focused 25-test suite passes. It is not
misrepresented as a P0.2 evidence owner. P1-SC2 is closed.

### Closure of P1-SC3 — object format, index semantics, causal replay, and wire evidence

The physical matrix in `root_preservation/mutation.rs` lines 89-168 now runs
both SHA-1 and SHA-256 through managed parent/marker/lock/index mutation,
restore, checked ref reset, restart observation, and idempotent replay. The
checked-stash matrix in `root_preservation/stash.rs` lines 153-294 also runs
both object formats through normalized-image verification, native stash,
managed-index verification, and restoration. Raw-index evidence covers DIRC
versions 2-4, conflicts, every named/unknown rejected extension, checksum and
cross-format rejection, semantic flags, `TREE` invalidation, and preservation
of unrelated index/worktree semantics. Reset partials remain fail-closed with
identical ref/index/worktree bytes.

`dispatcher_reconciliation.rs` lines 188-340 crosses `NormalizeParent`,
`PrepareParent`, and `RestoreParent` through no-attempt execution, exact
success, matching failure over pending/`Before`/ambiguity, crash-before-record-
rewrite retry, stale and second attempts, mismatched prefixes/actions, and
success without an exact goal. The focused unknown-field child lines 77-148
covers both `normalize_parent` and `prepare_parent`, preserving unknowns only
across the exact handoff/action identity. Forward and restore parent fault
matrices retain staging-only/final-only/both-name classification and the Unix
sync and Windows two-rename restart boundaries. P1-SC3 is closed locally.

The remaining platform requirement is appropriately a P1/release gate rather
than a local P0.2 defect. `gwz-cli/dist-workspace.toml` declares Windows
x86-64, both macOS architectures, Linux x86-64, and Linux ARM64, and the Cargo
Dist release workflow builds the executable that links this `gwz-core`. That
matrix must run on the committed and pushed corrected tree. Until it passes,
P1 and release stay blocked; local P0.2 acceptance neither claims that result
nor weakens the gate. The production preservation observer and executor still
return `MergePhaseUnsupported`/not-implemented results, so no unverified
platform path is active.

### Closure of P2-SC4 — current controls

Amendment 2, ReverseLifecycleInterface, TransitionDesign, ChangeBudget, and
Refactor now state that both §13 interface reviews are GO, the bounded
production-disabled implementation and remediation are present, corrected-
code reviews are in progress, and P1 remains paused. They no longer describe
§13 as awaiting interface review or implementation. The same controls carry
the corrected manifests and ledgers above. P2-SC4 is closed.

### Ownership and cohesion judgment

The focused physical owners remain cohesive and below their stop thresholds:
`contract.rs` 837, `preservation.rs` 455, `preservation_image.rs` 491,
`preservation_root.rs` 499, `files.rs` 297, `index.rs` 375,
`index_format.rs` 341, and `parent.rs` 351 lines. The new and expanded focused
evidence owners remain below 500 lines; the 536-line lifecycle resolver and
631-line authority owner retain single binding/resolution responsibilities.
No hidden parent creation, compound parent-plus-marker mutation, new durable
field, or alternate production authority was introduced.

The hand-written `gwz-core/tests/protocol.rs` is 1,216 lines and therefore
does trigger the user-directed 1,000-line cohesion review. It is accepted
without a split finding: its 28 tests form one public wire-conformance owner
covering message round trips, pinned discriminants, generated-source
freshness, and shared protocol fixtures. It contains no P0.2 lifecycle or
filesystem policy, and splitting it would add evidence paths without creating
a clearer authority boundary. The threshold is a judgment trigger, not an
automatic defect.

### Verification

- `cargo test -p gwz-core git::tests::g15::root_preservation --lib`: 32
  passed;
- focused parent causal reconciliation and unknown-field identity tests:
  passed;
- `cargo test -p gwz-core --lib`: 1,035 unit tests, comprising 1,034 passed,
  0 failed, and 1 ignored;
- `cargo test -p gwz-core --test protocol`: 28 passed;
- `python3 -m pytest gwz-py/src/tests/test_client.py -q`: 25 passed;
- `cargo fmt --all -- --check`: passed;
- `cargo clippy -p gwz-core --all-targets --all-features -- -D warnings`:
  passed; and
- `python3 gwz-core/scripts/checks/check_merge_docs.py`: passed, 87 assertions.

P0.2 is accepted by this architecture/ownership re-review. P1 remains paused
until the other independent corrected-code review is GO and the committed-tree
native release-platform gate passes.

## Windows platform-remediation review — 2026-08-11

### Verdict

**NON-GO for P1 unblocking; GO for the two-file remediation itself.** Independent
review of committed `gwz-core` range
`71ffb641..064f06e571334c3935233a7d253effc73fcc4b75` finds no P0, P1, P2, or
P3 defect in `record_wire/location.rs` or the `cfg(unix)` import correction in
`preservation_root/files.rs`. P1-WR1 below is a gate-operability finding, not a
defect in those owners: the required native Cargo Dist matrix cannot currently
be run against the untagged corrected commit without entering the publishing
workflow.

### Remediation judgment

`location.rs` lines 302-350 uses stable Rust 1.95 Windows APIs and matching
`windows-sys` 0.61 types. `OpenOptionsExt` applies read access, delete-capable
sharing, `FILE_FLAG_BACKUP_SEMANTICS`, and
`FILE_FLAG_OPEN_REPARSE_POINT`; the final named component is therefore opened
without following a reparse-point target. `symlink_metadata` type checks reject
non-regular leaves and non-real directories, while each intermediate directory
is observed and identity-checked separately. The remaining check/open intervals
have the same fail-closed path-plus-identity contract as Unix; the code does not
claim atomic namespace pinning.

`GetFileInformationByHandle` is called with a live `AsRawHandle`, propagates
`last_os_error` on failure, and combines the volume serial number with the high
and low file-index halves in the documented order. That is also the identity
tuple used by Rust 1.95's non-UWP Windows `File::metadata` implementation.
Parent identities are rechecked after both leaf observations. Each leaf is
compared before open, on the reading handle, and after read, including identity,
file type, and length; the second complete observation also detects byte-identical
replacement and absent/present races. Windows consequently has replacement and
race-detection parity with the Unix implementation's identity model. Native
matrix execution remains necessary to validate the supported Windows runner and
filesystem in practice.

Unreadable open-record paths map to `MergeRecordUnreadable`, unreadable archive
paths map to `ArchivedRecordUnreadable`, and identity/contention changes map to
`MergeRecoveryRequired`. These distinctions are preserved through directory,
leaf, and Win32-call failures. The `files.rs` change only places
`cap_fs_ext::OsMetadataExt` behind `cfg(unix)`; it removes an unavailable Windows
trait import without changing the non-Unix executable-file policy or any
preservation mutation behavior.

Both owners remain cohesive: `location.rs` is 478 lines with one canonical
location/identity acquisition responsibility, and `files.rs` is 297 lines with
one capability-relative managed-file responsibility. No split finding is
warranted.

### P1-WR1 — native platform gate is not operable pre-publication

`gwz-cli/.github/workflows/release.yml` accepts only a published release or a
manual existing semver tag, verifies that tag, fixes `publishing` to `true`, and
conditions its Cargo Dist build matrix on that value. The corrected tree has no
release tag. Thus it cannot supply the declared pre-P1 committed-tree platform
gate without first creating release state and taking publishing/upload paths.

The preferred remedy is a separate, non-publishing `workflow_dispatch`
platform-gate workflow. It should require and verify an exact commit/ref, check
out that revision (and the exact matching `gwz-core` revision), run Cargo Dist
plan/build across the frozen five targets, retain diagnostic artifacts only,
and have no release creation, hosting, attestation, or release-asset upload
authority. A clearly separated `publish: false` dispatch mode in the existing
workflow is also architecturally acceptable, but only if every host, announce,
attestation, and release-upload step is positively gated; sharing the release
workflow makes that alternative more condition-sensitive. Merely relaxing the
tag check while leaving `publishing: "true"` is not acceptable.

No workflow edit is authorized by this review. Before either remedy is edited,
its selected path must be added explicitly to the P0.2/P1 evidence ownership
manifests and the charged and unique unions mechanically recounted. If one new
workflow path is charged to both packages, the minimum charged deltas are P0.2
evidence **26 to 27**, P1 evidence **19 to 20**, and R4b-P charged evidence
**158 to 160**. R4b-P unique and program-unique evidence increase by one only
if that selected path is not already present in the respective union. The
workflow change then requires its own focused settled-code re-review. The
committed Windows remediation itself changes none of the frozen counts:
`files.rs` is already a P0.2 path, while `location.rs` was introduced as a
broader R4b location owner and is not added retroactively to the P0.2 physical
manifest.

### Verification and evidence state

- Rust 1.95 isolated `x86_64-pc-windows-msvc` type checks passed for the exact
  Win32 identity call and the no-follow/share-mode `OpenOptions` construction;
- `cargo +1.95.0 test -p gwz-core canonical_location --lib`: 4 passed;
- `cargo +1.95.0 test -p gwz-core
  open_status_retries_after_byte_identical_leaf_replacement --lib`: 1 passed;
- `cargo +1.95.0 fmt --all -- --check`: passed;
- `cargo +1.95.0 clippy -p gwz-core --all-targets --all-features -- -D
  warnings`: passed;
- `python3 gwz-core/scripts/checks/check_merge_docs.py`: passed, 87 assertions;
  and
- the full local MSVC-target check reached Windows Rust dependencies but could
  not pass `libz-sys` on the macOS host because the Windows C SDK headers/tooling
  were unavailable.

The pushed `gwz-core` retained-reader run 31440069226 completed successfully on
Windows x86-64, both macOS architectures, Linux x86-64, and Linux ARM64 during
this review. It is useful CI evidence, but it is not the required Cargo Dist
current-code release-platform build. That native platform-gate evidence has not
started and remains the sole blocker recorded by this review.

## §14 non-publishing native platform-gate interface review — 2026-08-11

### Verdict

**NON-GO.** The separate one-path, read-only workflow is the correct architecture,
and its SHA, target, and accounting boundaries reconcile. Its exact Cargo Dist
invocation is not yet executable as written. There are no P0 or P3 findings,
one P1 finding, and one P2 control finding. No workflow implementation is
authorized by this review.

### Accepted boundary

The proposed `gwz-cli/.github/workflows/platform-gate.yml` is correctly isolated
from the publishing workflow. Manual dispatch only, top-level `contents: read`,
no release trigger, and the express prohibition on host, announce, attestation,
package publication, GitHub Release, and release-asset upload authority leave
only ordinary diagnostic Actions artifacts. The static matrix is exactly the
five-target set in `dist-workspace.toml`: Windows x86-64, both macOS
architectures, and Linux x86-64/ARM64. Rust 1.95, Cargo Dist 0.31, native runners,
all-target success, and retained per-target manifests are appropriate gates.

The embedded self-check is acceptable as evidence in the same workflow path;
it is a regression alarm, not a substitute for the required settled-code
review. No second test owner is needed.

The corrected SHA contract is accepted. Before checkout it binds
`github.repository`, the path in `github.workflow_ref`, and
`github.workflow_sha` to the exact supplied `gwz-cli` SHA. It then verifies both
fixed-repository sibling checkout `HEAD` values. Every target's retained identity
artifact and summary binds the executing workflow repository/path/SHA, both
supplied and observed checkout SHAs, and its target. Because the exact reviewed
CLI manifest selects `gwz-core` only through `../gwz-core`, this closes the
workflow-definition and sibling-source substitution cases.

### P1-14A — Cargo Dist's exact non-publishing invocation is underspecified

Cargo Dist 0.31 requires an announcement selector for build planning and, when
`--tag` is absent, attempts to infer one from package versions. Section 14 both
forbids tag inference and omits the alternative, so a conforming command is not
defined. It also says only “local artifacts” without freezing the CI-safe flags
that distinguish those artifacts from Cargo Dist's deliberately fuzzy host
mode.

The interface must either freeze an explicit non-release Cargo Dist
announcement selector derived from the exact CLI package version, while
stating that it is not and must not create or resolve a Git ref, or narrow the
prohibition to Git release-tag inference and explicitly permit Cargo Dist's
package-version selector. Each row must run the equivalent of
`dist build --artifacts=local --target=<exact-row-target>
--output-format=json`, verify Cargo Dist 0.31.0 and Rust 1.95 before building,
and reject unless the emitted manifest names that one exact target. The early
self-check must compare the unique static matrix set for exact equality with
the checked CLI SHA's `dist-workspace.toml`, not merely search for five known
strings. This closes drift, duplicate-row, fuzzy-host, and accidental
sixth-target cases.

### P2-14B — two controls claim review before it is complete

`GwzM5-8R4bTransitionDesign.md` and `GwzM5-8Refactor.md` describe §14 evidence
as “independently reviewed” while both §14 interface reviews are still in
progress and this review is NON-GO. They must say that independent interface
review is pending/required until both reviews return GO. Amendment 2, the
change budget, and the reverse-lifecycle interface correctly retain the pause
through interface acceptance, implementation, focused settled-code review, and
the successful exact-SHA run.

### Ownership, accounting, and verification

The one-path accounting is correct. Charging the new evidence path once to
P0.2 and once to P1 changes P0.2 **23/26 to 23/27**, P1 **24/19 to 24/20**, and
R4b-P charged paths **174/158 to 174/160**. Because the physical path is new to
both unions, R4b-P unique evidence changes **106 to 107** and program-unique
evidence **149 to 150**; production counts and all numeric line ceilings remain
unchanged. Reporting 23/26 implemented paths within the frozen 23/27 manifest
before workflow implementation is also correct.

`python3 gwz-core/scripts/checks/check_merge_docs.py` passed all 87 assertions,
and the changed control documents pass `git diff --check`. Closing P1-14A and
P2-14B in the interface and controls requires another independent §14 interface
re-review before the workflow is authored.

## §14 corrected-interface architecture re-review — 2026-08-11

### Verdict

**GO.** P1-14A and P2-14B are closed. The corrected §14 is a complete
pre-implementation contract for the one-path non-publishing native platform
gate. No P0, P1, P2, or P3 finding remains. This verdict authorizes only the
bounded workflow implementation; P1 remains paused through its focused
settled-code review and successful exact-SHA five-target run.

### Closure of P1-14A

One contract job now owns the single JSON matrix, rejects duplicate, missing,
or extra targets and runner drift, and passes that same matrix to all build
jobs. Every row freezes the CI-safe Cargo Dist invocation to
`--artifacts=local`, its one exact `--target`, explicit
`--tag=v0.2.0-dev`, `--print=linkage`, and JSON output. The selector matches the
frozen CLI package version, is explicitly non-implicit, and neither resolves
nor creates a Git tag or release.

The post-build contract pins Cargo Dist 0.31.0, the explicit announcement
selector, and the row's exact executable archive and checksum through
`dist print-upload-files-from-manifest`; output for any other target rejects.
Together with the already-accepted workflow repository/path/SHA binding,
sibling checkout `HEAD` checks, per-row identity records, native runners, and
all-row success rule, this is exact five-target Cargo Dist parity rather than
fuzzy host evidence.

The locally installed Cargo Dist 0.31.0 `plan` output confirmed the specified
`dist_version`, `announcement_tag`, and `announcement_tag_is_implicit` fields
and the target-specific archive/checksum schema. A direct local `dist build`
attempt stopped before compilation because this development checkout is nested
in the outer `gwz-dev` Cargo workspace, whose root does not define the member's
`dist` profile. That host-layout limitation does not apply to §14's specified
clean sibling checkouts and is not claimed as platform evidence.

### Closure of P2-14B and retained boundary

`GwzM5-8R4bTransitionDesign.md` and `GwzM5-8Refactor.md` now call §14 proposed
evidence pending interface review rather than claiming completed review. The
amendment, change budget, and reverse-lifecycle interface consistently retain
the pause through both reviews, implementation, settled-code review, and the
exact pushed-commit run.

The security and ownership judgments are unchanged: the manual workflow has
only `contents: read`, diagnostic Actions-artifact authority, and explicit
prohibitions on release, host, announce, attestation, publication, and release-
asset operations. The one physical path is charged twice but counted once per
union: P0.2 remains **23/27**, P1 **24/20**, R4b-P charged **174/160**, R4b-P
unique **104/107**, and program-unique **145/150**, with no production or line-
ceiling change.
