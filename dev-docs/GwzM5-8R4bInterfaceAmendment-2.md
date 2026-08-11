# R4b-P0.2 exact root-preservation interface amendment

Status: both independent reviewers accepted the corrective interface through
§13, and the bounded implementation is present with production v1 still
disabled. The first settled-code architecture review then found an index-only
foreign-marker provenance defect, four uncounted causal-seam export paths,
incomplete evidence rows, and stale control text. The bounded remediation is
implemented and both corrected-code re-reviews are GO with no open P0-P3
finding. P0.2 is accepted locally. The Windows portability remediation is
code-GO. Both independent reviewers accepted §14's non-publishing native
release-platform implementation and exact-SHA run with no open P0-P3 finding.
P1-WR1 is closed and P1 is unpaused against the frozen seam.

This amendment is additive to `GwzM5-8R4bInterfaceAmendment-1.md` and is
controlling where the earlier documents describe root preservation as one
compound `NormalizeRoot` or `RestoreRoot` action.

## 1. Why P0.2 is required

P1 consumption proved that the accepted interface cannot safely implement
root preservation.

The root publication handoff may describe independently different states for:

- the candidate marker worktree leaf;
- the marker leaf's canonical parent directory;
- the workspace lock worktree leaf;
- the corresponding raw index entries; and
- the managed `.git/info/exclude` boundary.

The existing `NormalizeRoot` and `RestoreRoot` phases imply that these objects
can change as one physical action. They cannot. A regular-file replacement and
a Git index lockfile commit are separate durability boundaries. No portable
filesystem primitive atomically commits both leaves plus `.git/index`.

The existing `set_branch_target_checked` also requires a clean attached
checkout. A root at an exact publication handoff can be intentionally dirty
relative to its current commit, so the reset cannot begin until the managed
marker, lock, and index are put into the exact clean form for that commit.

P1 must not hide either problem by:

- treating a compound rewrite as atomic;
- accepting any representable publication prefix after a crash;
- using `--force` or a destructive checkout;
- stashing operation-owned publication changes as user work;
- dropping the exact `Pre` versus `Staged` index distinction; or
- adding unjournaled normalization inside the reset executor.

## 2. Architectural decision

P0.2 uses one durable phase value per independently checked physical step.
It does **not** add a second `root_cursor` field beside the existing phase.
The phase is already the preservation cursor and remains part of the pending
action's serialized identity, authority payload, transition footprint, and
unknown-field lifetime.

P0.2 also leaves the managed boundary byte-for-byte unchanged throughout
preservation. Native stash, checkout, and index operations do not mutate
`.git/info/exclude`, but status/image capture and native stash do consume it to
partition ignored paths from untracked paths. The durable handoff boundary is
therefore the one frozen ignore policy for virtual clean-image projection and
stash creation. Changing it would both change the preserved image and add a
separate physical mutation boundary. Every root observation binds its exact
bytes and reuses its ignored/untracked partition. Boundary drift is ambiguity
and never becomes stashable user work.

The root physical steps are therefore:

1. readiness of the canonical marker parent before a clean form needs a marker;
2. marker worktree leaf;
3. lock worktree leaf;
4. the marker/lock raw index subset; and
5. restoration of the canonical marker parent when native stash removed its
   empty directory.

Parent readiness and marker-leaf publication remain separate in every
direction. When the required parent is missing, the checked parent step stages
and atomically publishes only an empty directory. `NormalizeMarker`,
`PrepareMarker`, and `RestoreMarker` remain the sole phases that can change the
marker leaf in their respective regions.

An exact empty final parent is not accepted merely because it is structurally
the goal. Unix proves the directory entry with an idempotent parent-directory
sync. Windows, where the selected directory-sync API is not a persistence
barrier, proves it with a pinned, handle-bound, write-through round-trip rename
through the deterministic empty staging name. Both use the causal attempt seam
in §8, so a concurrent direct `mkdir` cannot fabricate phase completion.

The existing checked attached-checkout reset remains one journaled Git action.
It is not described as cross-file atomic. As with participant rollback, a
fresh observation after interruption must classify it as exact before, exact
after, or ambiguous. Ambiguity enters typed recovery and never fabricates
completion.

## 3. Durable phase vocabulary

### 3.1 Stash phases

`PreservationStashPhaseV1` becomes:

```rust
enum PreservationStashPhaseV1 {
    NormalizeParent,
    NormalizeMarker,
    NormalizeLock,
    NormalizeIndex,
    CreateStash,
    RestoreIndex,
    RestoreLock,
    RestoreParent,
    RestoreMarker,
    WriteBundle,
    Complete,
}
```

For a root owner with a candidate handoff, the exact graph is:

```text
normalize_parent -> normalize_marker -> normalize_lock -> normalize_index -> create_stash
  -> restore_index -> restore_lock -> restore_parent -> restore_marker
  -> write_bundle -> complete
```

For a non-root owner, or a root owner without a candidate handoff, the graph
remains:

```text
create_stash -> write_bundle -> complete
```

Each normalize/restore phase names the **next** checked object. An exact goal
normally advances without a physical call. The reducer still records that
phase edge; it does not skip cursor states in memory. On every platform,
`AfterNeedsDurability` for a required empty parent executes the platform's
idempotent parent barrier before advancing. An optional or already-established
parent remains proof-only.

`AdvanceStash` may remain in the same conceptual normalize or restore region,
but it advances exactly one listed phase. Only completion of `CreateStash`
may install the exact stash id/object id. Other root-step advances cannot
change stash evidence.

### 3.2 Attached-ref reset phases

`PreservationRefResetPhaseV1` becomes:

```rust
enum PreservationRefResetPhaseV1 {
    PrepareParent,
    PrepareMarker,
    PrepareLock,
    PrepareIndex,
    ResetRef,
    RestoreIndex,
    RestoreLock,
    RestoreParent,
    RestoreMarker,
    Complete,
}
```

For a root owner with a candidate handoff, the exact graph is:

```text
prepare_parent -> prepare_marker -> prepare_lock -> prepare_index -> reset_ref
  -> restore_index -> restore_lock -> restore_parent -> restore_marker
  -> complete
```

For every other owner it remains:

```text
reset_ref -> complete
```

The prepare phases make only the Git-visible managed state exact for the
current attached commit. The boundary remains at the durable handoff. Because
the boundary is outside Git status and the index, it does not prevent the
existing checked reset from requiring an otherwise clean repository.

After `ResetRef`, the attached checkout is at the recorded restore commit.
The restore phases recreate the exact durable handoff relative to that commit.

### 3.3 Root-owner and field rules

The root-bearing owner rule from P0.1 is unchanged:

- `PublicationRoot` and selected participant `@root` use the root graph;
- the selected-root collision still creates only one root owner;
- a non-root owner requires `root_publication_handoff: None`;
- a root action carries the exact candidate handoff only when the durable
  operation handoff is `Candidate { prefix, index }`; and
- `NoCandidate` and `EvidencePending` use the non-root physical graph because
  no candidate publication overlay exists to normalize.

Validation rejects old `normalize_root`, `restore_root`, or compound reset
shapes in a v1 record. V1 is not yet a released writer format, so no installed
v1 compatibility promise is weakened.

### 3.4 Exact physical phase table

Let:

- `H` be the exact durable handoff managed form;
- `C0` be the exact clean form derived from the currently attached expected
  commit; and
- `C1` be the exact clean form derived from the recorded restore commit.

The marker-parent requirement is derived from its selected marker form: a
present marker requires the real canonical directory; an absent marker makes
that directory optional. Directory contents are validated separately against
the marker leaf expected at the current phase. Thus a complete form with an
absent marker admits either a missing directory or a real empty canonical
directory, while a parent phase may observe the one later source marker leaf
before its corresponding marker phase. A symlink, non-directory, inaccessible
directory, temporary residue, second marker, or foreign child is never exact.
The sole staging exception is §5.4's deterministic empty directory during
a parent phase; it is part of that phase's `Before`, not any managed form.

Every row also requires the unchanged handoff boundary, the exact row-specific
branch and commit, a clean native sequencer state, and the row-specific
unrelated-work guard.

| Durable phase | Exact before | Exact after |
| --- | --- | --- |
| `NormalizeParent` | marker and parent `H`; lock/index `H` | parent `C0`; marker remains `H`; lock/index `H` |
| `NormalizeMarker` | marker `H`; lock/index `H` | marker `C0`; lock/index `H` |
| `NormalizeLock` | marker `C0`; lock/index `H` | marker/lock `C0`; index `H` |
| `NormalizeIndex` | marker/lock `C0`; index `H` | marker/lock/index `C0` |
| `CreateStash` | all managed `C0`; no matching stash; normalized image equals the durable hash | all managed `C0`; one exact stash whose decoded image equals the durable image; no unrelated live dirt |
| `RestoreIndex` | all managed `C0`; exact stash evidence | index `H`; marker/lock `C0`; exact stash evidence |
| `RestoreLock` | index `H`; marker/lock `C0` | index/lock `H`; marker `C0` |
| `RestoreParent` | index/lock `H`; marker and parent `C0` | index/lock `H`; parent `H`; marker remains `C0` |
| `RestoreMarker` | index/lock/parent `H`; marker `C0` | all managed `H` |
| `WriteBundle` | all managed `H`; exact native stash; absent or exact earlier canonical bundle prefix | all managed `H`; exact canonical bundle including the current row |
| stash `Complete` | exact handoff, stash, and bundle evidence | proof-only terminal state; no physical action |
| `PrepareParent` | marker and parent `H`; lock/index `H` | parent `C0`; marker remains `H`; lock/index `H` |
| `PrepareMarker` | marker `H`; lock/index `H` | marker `C0`; lock/index `H` |
| `PrepareLock` | marker `C0`; lock/index `H` | marker/lock `C0`; index `H` |
| `PrepareIndex` | marker/lock `C0`; index `H` | marker/lock/index `C0` |
| `ResetRef` | attached expected branch/commit; all managed `C0` | attached restore branch/commit; all managed `C1` |
| reset `RestoreIndex` | all managed `C1` | index `H`; marker/lock `C1` |
| reset `RestoreLock` | index `H`; marker/lock `C1` | index/lock `H`; marker `C1` |
| reset `RestoreParent` | index/lock `H`; marker and parent `C1` | index/lock `H`; parent `H`; marker remains `C1` |
| reset `RestoreMarker` | index/lock/parent `H`; marker `C1` | all managed `H` |
| reset `Complete` | attached restore branch/commit and exact handoff form | proof-only terminal state; no physical action |

The no-candidate/non-root graphs retain only `CreateStash -> WriteBundle ->
Complete` and `ResetRef -> Complete`; they do not acquire synthetic root
facts. For a named managed object, the classifier evaluates the complete
`After` predicate first. If it is true, the result is `After`, including when
source and goal are equal. The sole causal refinement is a required empty
marker parent whose source marker is absent and goal marker is present: exact
final-only structure is `AfterNeedsDurability`, not completed `After`.
`Before` is considered only when
the complete goal predicate is false, the named source has a permitted
not-yet-goal form, and the whole source/prior/later predicate is exact. Every
other physical form, including an unexpected advance of a later object, is
`Ambiguous`. The checked executor calls this classifier; it does not reproduce
the predicates.

## 4. Exact clean forms versus durable handoff

The clean normalization target is **not always** `Boundary/Staged`, and a
caller-supplied clean form is not authority by construction.

`C0` is the exact marker/lock tree projection of the currently attached
expected commit together with the corresponding clean stage-0 index subset.
`C1` is the same projection of the recorded restore commit. This definition
also closes the permitted descendant history: a live descendant may have
committed managed-path content different from the immutable anchor, so its
`C0` is derived from that exact descendant rather than guessed from the
publication prefix.

Before preparation or step classification, the backend resolves the exact,
unabbreviated commit ids and independently reads the marker and lock entries
from each named commit tree. It derives the matching clean index subset,
including exact absence, raw path, blob id, regular-file mode, stage, and
accepted flags. The supplied `C0` and `C1` must equal those independent
projections. A missing commit, non-commit object, mismatch, unsupported mode,
or noncanonical path is an error and issues no observation or proof.

The familiar rows remain consequences rather than an exhaustive history
table:

| Recorded commit | Consequent exact clean form |
| --- | --- |
| baseline/pre commit | baseline marker/lock worktree plus `Pre` index |
| exact composition-evidence commit | candidate marker/lock worktree plus `Staged` index |
| descendant or recorded restore commit | exact marker/lock/index projection of that commit |

The boundary column is deliberately absent: its handoff bytes remain invariant
and control ignored/untracked classification in every row.

`C0`, `C1`, and `H` are separate authority inputs. One authority-owned
projection converts the validated durable model handoff into the explicit
physical `H`. The model-owned `preservation_handoff_is_compatible` remains the
sole policy for whether the durable pair can exist at the recorded publication
step. The Git backend validates only physical facts, commit-tree provenance,
and canonical paths; it never maps or copies the model's `Baseline`, `Marker`,
`Lock`, `Boundary`, `Pre`, or `Staged` vocabulary.

## 5. Git backend contract

### 5.1 Types

The Git-neutral types describe explicit physical facts. They contain no copy
of the model publication vocabulary:

```rust
enum GitRootManagedObject {
    MarkerWorktree,
    LockWorktree,
    Index,
    MarkerParentDirectory,
}

struct GitRootManagedIndexEntry {
    path: Vec<u8>,
    object_id: String,
    mode: u32,
    stage: u8,
    assume_valid: bool,
    skip_worktree: bool,
    intent_to_add: bool,
}

enum GitRootManagedIndexFact {
    Absent { path: Vec<u8> },
    Present(GitRootManagedIndexEntry),
}

struct GitRootManagedIndexForm {
    marker: GitRootManagedIndexFact,
    lock: GitRootManagedIndexFact,
}

struct GitRootManagedForm {
    marker: Option<GitCandidateFile>,
    lock: GitCandidateFile,
    index: GitRootManagedIndexForm,
}

struct GitRootPreservationSpec {
    attached_branch: String,
    attached_commit: String,
    restore_commit: String,
    managed_marker_path: String,
    attached_clean_form: GitRootManagedForm, // C0
    restore_clean_form: GitRootManagedForm,  // C1
    handoff_form: GitRootManagedForm,        // H
    handoff_boundary: Vec<u8>,
}

enum GitRootManagedFormName {
    AttachedClean,
    RestoreClean,
    Handoff,
}

struct GitRootManagedTransition {
    object: GitRootManagedObject,
    source: GitRootManagedFormName,
    goal: GitRootManagedFormName,
}

enum GitRootPreservationPhysicalStep {
    Managed(GitRootManagedTransition),
    CreateStash { merge_id: String },
    ResetAttachedRef,
}

enum GitRootPreservationGuard {
    NormalizedPreimage { sha256: String },
    OtherwiseClean,
}

enum GitRootPreservationStepObservation {
    Before,
    After,
    AfterNeedsDurability,
    Ambiguous,
}

enum GitCheckedPreservationMutation {
    Applied,
    AlreadyComplete,
    StashCreated(GitStashPushResult),
    RefReset(GitUpdateResult),
}

struct GitPreparedRootStash {
    normalized_image: GitPreservationImage,
}
```

Every present index row must be stage zero, regular non-executable mode, and
free of `assume_valid`, `skip_worktree`, `intent_to_add`, or an unknown flag.
The object id is a complete id in the repository's SHA-1 or SHA-256 format.
`managed_marker_path` fixes one operation marker identity: every optional
marker worktree fact and marker index fact must use exactly that normalized
path. The lock and `.git/info/exclude` paths are fixed by the backend. A second
marker path or caller-supplied lock/boundary path rejects before repository
inspection.

`MarkerParentDirectory` has no caller-supplied fact. The backend derives a
`Required` or `Optional` requirement from the selected form's marker. Required
means a real canonical directory; Optional means missing or real canonical
directory. In either case its only permitted entry is the exact marker leaf
required by the whole phase, if any. This keeps parent readiness separate from
marker-leaf publication without introducing a second source of authority.

The three form names are physical selectors only; they are not durable phases.
Authority selects the exact source, goal, step, and guard for a durable phase.
Git validates the spec and executes the selected physical step without owning
phase adjacency or publication compatibility.

### 5.2 Trait additions

```rust
fn prepare_root_preservation_stash(
    &self,
    root: &Path,
    spec: &GitRootPreservationSpec,
) -> ModelResult<GitPreparedRootStash>;

fn observe_root_preservation_step(
    &self,
    root: &Path,
    spec: &GitRootPreservationSpec,
    step: &GitRootPreservationPhysicalStep,
    guard: &GitRootPreservationGuard,
) -> ModelResult<GitRootPreservationStepObservation>;

fn execute_root_preservation_step_checked(
    &self,
    root: &Path,
    spec: &GitRootPreservationSpec,
    step: &GitRootPreservationPhysicalStep,
    guard: &GitRootPreservationGuard,
) -> ModelResult<GitCheckedPreservationMutation>;
```

The three methods are the entire new public-in-core backend surface. The
backend does not receive a model phase or a generic path-writing callback.
`CreateStash` always includes untracked paths and internally uses the existing
native stash implementation. `ResetAttachedRef` internally uses the existing
checked attached-ref reset. This keeps their final pre-mutation revalidation
inside the same checked backend call rather than introducing an authority/
mutation time-of-check gap.

### 5.3 Preparation and preimage binding

`prepare_root_preservation_stash` is read-only. It:

1. validates the spec, independently derives `C0` and `C1` from their exact
   commit trees, and requires equality with the supplied forms;
2. requires the exact attached branch/commit and clean native sequencer state;
3. verifies the complete exact `H`, including its invariant boundary; partial
   `H`/`C0` mixtures reject because no durable phase exists before
   `BeginStash`;
4. projects the full preservation image as if marker, lock, and index were in
   `C0`, without changing the repository;
5. uses the exact `H` boundary to classify raw paths as ignored or untracked,
   classifies only unrelated paths as user dirt, and leaves ignored paths out
   of the image; and
6. rejects a user edit, symlink, directory, executable, mode change, second
   managed path, or forbidden index fact at a managed path.

`BeginStash` stores `normalized_image.preimage_sha256` before the first
normalize phase. Before every normalize mutation, observation reprojects the
same normalized image under the exact `H` boundary and requires that exact
hash. Thus unrelated work cannot change while a partially normalized root is
being completed.

The `CreateStash` before-observation and checked executor perform that complete
projection again after durable `NormalizeIndex` and immediately before native
stash mutation. Changes to staged, unstaged, untracked, ignored, or raw
non-UTF-8 paths, or any boundary drift, cannot enter a stash under a stale
durable preimage. A changed ignored path does not change the image, but the
same exact boundary must still classify it as ignored and the path must remain
untouched.

After stash creation, the decoded stash image must equal the durable preimage
and the live repository must contain no unrelated dirty state. Restore steps
therefore use `OtherwiseClean` and require exact stash evidence plus an
otherwise clean live image at the authority layer.

Reset preparation likewise requires no unrelated dirty state; all user work
has already been preserved. It uses `OtherwiseClean` and exact commit-derived
forms.

### 5.4 Observation and execution

For a managed-object physical step, the observer checks the entire root but
classifies only the named object:

- `Before`: all prior objects equal the goal, the named object equals the
  phase source, all later objects equal the derived source, the boundary is
  the handoff, and unrelated state satisfies the phase contract;
- `After`: the named object and all prior objects equal the goal, later
  objects still equal the source, boundary and unrelated state remain exact;
- `AfterNeedsDurability`: the same structural predicate as `After`, but the
  platform requires one idempotent persistence call before phase completion;
- `Ambiguous`: any third/mixed physical form, opposite index form, wrong
  commit/ref, managed-path user edit, parent/leaf replacement, or unexpected
  later-object advance.

The disjoint goal-first rule in §3.4 is normative. A source-equals-goal row is
ordinary `After`, never `Before`, unless it is the explicitly causal required-
empty-parent case classified `AfterNeedsDurability` below.

The pre-clean order is exactly marker parent, marker leaf, lock leaf, index.
The restoration order is exactly index, lock leaf, marker parent, marker leaf.
For `MarkerParentDirectory`, every classification requires the exact phase
prefix and the marker still at its source form. The exact cases are symmetric
for `H -> C0` preparation and `C0`/`C1 -> H` restoration:

- if the goal marker is absent, a missing parent, a real-empty parent, or the
  real canonical parent containing exactly the expected later source marker is
  ordinary `After`; the phase is proof-only and performs no namespace action;
- if source and goal both have a marker, their already-established
  real parent is ordinary `After`; the phase is proof-only even when the
  marker bytes differ; and
- if the source marker is absent and goal marker is present, final missing with
  no staging directory, or final missing with the one exact real-empty staging
  directory, is `Before`; final real-empty with staging absent is
  `AfterNeedsDurability` on every platform.

Final and staging both present, a foreign child, symlink, non-directory,
unowned/malformed residue, wrong marker advance, or a missing parent after a
durable parent edge is `Ambiguous`; no later phase repairs it.

For `CreateStash`, `Before` means all managed facts are `C0`, no matching stash
exists, and the freshly projected normalized image under `H`'s boundary equals
the durable hash. `After` means exactly one decoded matching stash equals that
image and no unrelated live dirt remains. For `ResetAttachedRef`, `Before` is
the exact expected branch/commit with `C0`; `After` is the same attached branch
at the exact restore commit with `C1`. Every mixed checkout/ref/index/worktree
state is `Ambiguous`.

Operational read failures remain typed errors; they are not converted to
`Absent` or `Ambiguous`.

The checked executor repeats the complete observation immediately before the
mutation and uses the same classifier. `After` returns `AlreadyComplete`.
`AfterNeedsDurability` performs its named idempotent barrier, reobserves the
exact goal, and then returns `AlreadyComplete`.
`Before` changes only the named object, creates the exact native stash, or
executes the existing checked reset, and returns the corresponding typed
result. `Ambiguous` mutates nothing.

`NormalizeParent`, `PrepareParent`, and `RestoreParent` are the only steps
allowed to stage or publish a directory. Their
staging name is derived from the normalized marker path, source/goal forms, and
exact attached/restore commits, and is not caller supplied. `Before` admits
either no staging entry or that one real empty no-follow directory while final
`markers` is absent. Any other matching-prefix residue, link, child, second
staging entry, or simultaneous final-and-staging form is `Ambiguous`. Exact
staging is operation-owned, not a durable cursor or user work, and may be
reused after interruption; no unjournaled cleanup is required.

The checked executor retains a no-follow, identity-checked `gwz.conf` parent
and reobserves the exact classification. For `Before`, it creates the empty
staging directory handle-relative only if absent and publishes it to absent
`markers` by a no-replace directory rename. Linux uses
`renameat2(RENAME_NOREPLACE)` and macOS `renameatx_np(RENAME_EXCL)` relative to
the retained parent descriptor; unsupported Unix targets reject. Unix syncs
the staging directory before rename and `gwz.conf` afterward.

Every required empty final parent is nevertheless reported as
`AfterNeedsDurability`. With no matching attempt, Unix repeats the idempotent
`gwz.conf` sync. Windows instead performs a pinned, handle-bound,
write-through round trip: no-replace rename exact empty final `markers` to the
absent deterministic staging name, then no-replace rename that exact staging
directory back to `markers`. Both calls use
`SetFileInformationByHandle(FileRenameInfo)` with pinned `gwz.conf` as
`RootDirectory`, relative names, replacement disabled, and a source directory
handle opened with delete access plus `FILE_FLAG_BACKUP_SEMANTICS |
FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_WRITE_THROUGH`. No path call can be
redirected. Starting from `Before` on Windows uses the same write-through
staging-to-final publication for its first execution.

An interruption before first publication leaves exact `Before`; interruption
between the two Windows round-trip renames leaves staging-only `Before`; and
interruption after publication leaves final-only `AfterNeedsDurability`.
Restart therefore repeats a safe publication or barrier. A fresh matching
successful attempt plus final-only exact structure authorizes the durable
phase edge. Concurrent creation of an exact empty final is safely re-barriered
rather than structurally adopted; final plus staging is `Ambiguous`. The
release matrix proves both Windows rename boundaries, the Unix sync boundary,
and this classification on Windows, macOS, Linux x86, and Linux ARM.

The creation/barrier case exists exactly when a parent phase's source marker is
absent and goal marker is present. A present source marker already entails an
established real parent; an absent goal marker makes the parent optional.
Those cases are ordinary proof-only `After` on every platform.

Marker and lock replacement/removal use canonical no-follow parent and leaf
checks plus same-directory atomic replacement. The index step rewrites only
the exact marker/lock raw index entries through Git's lockfile commit. It is
not required to preserve the serialized `.git/index` file byte-for-byte. It
must preserve and reverify every unrelated canonical entry tuple and staged
content—raw path, stage, mode, semantic flags, and object identity—while
rejecting unsupported flags or extensions before mutation.

## 6. Checked ref reset

P0.2 does not widen or weaken `set_branch_target_checked`.

The `PrepareParent`, `PrepareMarker`, `PrepareLock`, and `PrepareIndex` phases
must first prove exact `C0` for `expected_commit`; the invariant `H` boundary
remains in place.
Only then may the checked root-preservation executor's `ResetAttachedRef` step
call the existing primitive. The executor repeats the complete before
classification inside that same checked call.

Fresh observation around `ResetRef` requires:

- before: attached expected branch/commit, clean managed Git form, exact
  handoff boundary, no unrelated dirt or native operation;
- after: attached restore branch/commit, exact clean form for that commit,
  unchanged handoff boundary, no unrelated dirt or native operation; or
- otherwise: ambiguity/recovery.

The existing checked reset may internally touch more than one Git object.
P0.2 makes no false atomicity claim for it. A crash-created mixed checkout is
not automatically repaired or adopted. Test-only fault injection must expose
the boundary between checkout work and the checked ref transaction, or build
every reproducible partial state at that boundary, and prove that each partial
state is classified `Ambiguous` without a retry mutation.

## 7. User-state and path rules

- Unrelated staged, unstaged, and untracked paths enter the normalized
  preimage and native stash exactly.
- Ignored paths are excluded from the image/stash and are never removed.
- A change at marker, lock, their index entries, or the managed boundary is
  never treated as user work; it is a preservation-evidence mismatch.
- Raw non-UTF-8 unrelated paths remain raw bytes and use the existing
  `BTreeMap<Vec<u8>, ...>` ordering.
- Managed paths are fixed UTF-8 paths with regular non-executable leaves or
  exact absence. Their parents are real and no-symlink except for the marker's
  canonical parent: when the selected marker is absent, a missing parent and
  a real empty parent are equivalent exact forms.
- Conflicted entries, multiple stages, intent-to-add, assume-valid,
  skip-worktree, and unknown index flags reject before journaling.
- SHA-1 and SHA-256 object formats remain supported by the existing image and
  object-id vocabulary.

## 8. Reducer and authority rules

- Reducers own the exact phase adjacency graphs in §3.
- The backend owns no durable phase ordering policy.
- Authority maps stash `NormalizeParent` and reset `PrepareParent` to
  `Managed(MarkerParentDirectory, Handoff -> AttachedClean)`, stash
  `RestoreParent` to
  `Managed(MarkerParentDirectory, AttachedClean -> Handoff)`, and reset
  `RestoreParent` to
  `Managed(MarkerParentDirectory, RestoreClean -> Handoff)`.
- Each authority proof binds exact record location/digest, owner, action,
  phase, attached/restore commit ids, `C0`, `C1`, `H`, boundary fact, normalized
  preimage when present, and full preservation cursor prefix.
- A proof for one object, goal, owner, root, or phase cannot authorize another.
- A no-op object still requires an exact `After` proof before the phase can
  advance.
- Git `AfterNeedsDurability` maps to a bound
  `ExactObservationFact::PreservationDurabilityPending` carrying the same
  phase-completion fact, cursor prefix, and physical action. With no execution
  attempt, the resolver returns `Execute` for that exact persisted action.
  With a fresh matching `Success` attempt, the resolver consumes the pending
  fact as the normal stash/reset phase completion and advances once. A matching
  failure paired with a fresh matching `Before`, pending, or `Ambiguous`
  physical binding returns the existing service diagnostic—exact
  `ErrorCode` and message—without a transition or same-invocation retry; this
  covers a Windows first-rename failure that leaves staging-only `Before` and a
  collision that leaves both names `Ambiguous`. The bound execution attempt
  retains its optional internal `detail` for attempt identity, but `ModelError`
  has no `detail` field and P0.2 neither appends it to the message nor widens the
  public model/protocol to expose it.
  Success without the fresh exact final-only pending goal, and every stale,
  mismatched, or second attempt, rejects.
- The service reloads the unchanged record and reobserves after execution, so
  the successful attempt is only causal authority when combined with a fresh
  exact physical goal. If the process stops after the barrier but before the
  record rewrite, the next invocation repeats it. Optional-parent and
  already-established-parent no-ops remain ordinary proof-only completions.
- Only `CreateStash` completion installs stash ids; only `WriteBundle`
  completion permits stash completion; only `ResetRef` can move the branch.
- The durable handoff remains unchanged through preservation and rollback.

## 9. Required evidence

The minimum closed matrix is:

1. every allowed handoff pair, including all four clean/handoff marker-presence
   combinations, crossed with both root owners and commit-derived
   baseline/pre, composition/staged, descendant `C0`, and restore `C1` forms;
2. independent commit-tree derivation rejects a fabricated/stale `C0` or `C1`,
   a missing/non-commit object, unsupported tree mode, and a second marker;
3. pre-journal preparation accepts only exact full `H`; every partial `H`/`C0`
   mixture rejects before `BeginStash`;
4. before/after/restart for every row in §3.4, including both pre-clean parent
   phases, with the durable phase persisted
   on both sides and the entire root exact;
5. no-op lock/marker/parent/index steps, including all degenerate allowed pairs,
   prove source-equals-goal is ordinary `After` and performs no physical write,
   except the required empty-parent causal case explicitly re-barriered on
   both Unix and Windows;
6. wrong phase, skipped/reordered object, unexpected later-object advance,
   opposite index, mixed prefix, stale preimage, wrong ref/commit, detached
   HEAD, and foreign native state;
7. changes to staged, unstaged, untracked, ignored, and raw non-UTF-8 unrelated
   paths after durable `NormalizeIndex` and before `CreateStash`; every included
   image change rejects without a stash, while an ignored path remains
   untouched under the same boundary classification;
8. a raw path whose ignored/untracked classification differs between baseline
   and candidate boundary bytes proves that exact `H`, not the alternate
   committed boundary, controls image and stash membership;
9. unrelated staged/unstaged/untracked work survives; ignored work survives
   and is absent from image/bundle;
10. user edits at marker, lock, index, and boundary each reject before
    mutation, including boundary drift at every partial phase;
11. raw non-UTF-8 ordering, SHA-1/SHA-256 object ids, forbidden index flags,
    unsupported extensions, and exact preservation of every unrelated
    canonical index entry tuple and staged content;
12. symlink/file/directory/executable and parent/leaf replacement races,
    absent-versus-empty parent equivalence in both physical directions,
    deterministic exact staging
    restart, malformed/duplicate residue, and foreign-child rejection;
13. fault injection before and after each atomic leaf/index mutation, plus a
    concrete interruption boundary inside the existing checked reset producing
    only exact before, exact after, or ambiguity without fabricated success or
    automatic repair;
14. multi-owner/global-preflight failure mutates no earlier repository,
    record, stash, bundle, ref, index, worktree, or boundary;
15. `AfterNeedsDurability` with no attempt, matching success/failure, crash
    before rewrite, stale/mismatched/second attempt, Unix parent sync, both
    Windows round-trip rename boundaries, and optional/established-parent
    ordinary `After`; exact YAML accepts `restore_parent` and every other new snake-case phase
    and rejects the retired
    compound spellings; validation, constructors, unknown-field identity,
    root collision, action-free cursor, exhaustion, and reducer adjacency are
    closed for root and non-root graphs; and
16. all retained v0 characterization remains byte- and behavior-exact.

## 10. Accepted implementation ownership and pre-remediation budget

This section records the interface-GO allocation used by the first
implementation. Section 12.5 supersedes its path counts and evidence totals
for the corrective slice; the original production ceilings and ownership
separation remain controlling.

The mechanically confirmed production manifest is exactly 10 paths:

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
whole-step orchestration. Its `files.rs` and `index.rs` children own canonical
leaf and raw-index observation/mutation. No Git owner owns phase adjacency or
publication compatibility.

The exact evidence/tool/document manifest is 20 paths:

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

The generic footprint, resolver, journal-vocabulary, and reducer tests do not
match a retired phase and need no P0.2 edit. No unlisted path may be edited. A
newly discovered owner stops implementation for a mechanical recount and
review rather than silently substituting paths.

The stop ceilings are:

| P0.2 component | Net production | Moved production | Test/tool/doc | Production paths | Evidence paths |
| --- | ---: | ---: | ---: | ---: | ---: |
| shared Git implementation/testing | 1,650 | 450 | 2,400 | 7 | 7 |
| model/transition integration | 450 | 0 | 800 | 3 | 6 |
| documents/reviews/control | 0 | 0 | 3,200 | 0 | 7 |
| **P0.2 total** | **2,100** | **450** | **6,400** | **10** | **20** |

Only the shared Git row is also charged to P1. Three of its seven production
paths and one of its seven evidence paths were already in P1, so P1 becomes
3,000 net production, 1,500 moved production, 4,200 test/tool/doc, 13 charged
production paths, and 14 charged evidence paths.

That pre-remediation snapshot produced the superseded 12,000/2,950/24,300
R4b-P charged ledger. Section 12.5 is the sole current numeric authority.

These are stop ceilings, not targets. Every new production and Git evidence
cohesion owner remains below 500 lines. `contract.rs` remains below 950, the
post-extraction `preservation.rs` below 850, and no existing owner may cross
1,000 without renewed cohesion review.

The seven document/review/control paths measured 1,753 lines at interface GO.
Their 3,200-line ceiling retains bounded code-review and remediation/re-review
appendices in the same two review memos without adding an evidence path. It is
not available to Git or model tests and does not enlarge production scope.

## 11. Exit gate

P1 was paused until:

1. architecture/ownership and state/restart reviewers independently return
   GO on this exact design;
2. all physical path manifests and charged/unique counts are reconciled in
   this document, `GwzM5-8R4bReverseLifecycleInterface.md`, and
   `GwzM5-8ChangeBudget.md`;
3. the reviewers agree that the phase graph covers every physical mutation
   without a false compound-atomic claim;
4. the backend/model interface is implemented and independently re-reviewed;
5. strict Clippy, formatting, focused matrices, full core, protocol parity,
   and retained-reader gates pass; and
6. only then does P1 resume observation/execution against the frozen seam.

## 12. Settled-code remediation boundary

The first settled-code reviews and subsequent real-Git evidence found four
implementation gaps:

- managed leaf mutation was path-based rather than anchored against parent
  replacement;
- repository object format and raw index extensions were not closed; and
- the physical/restart evidence implemented only a small happy-path subset of
  §9; and
- native `stash -u` removes an empty canonical marker parent, so restoration
  requires its own journaled physical step.

This section controls the corrective slice. It adds only `RestoreParent`, its
two adjacent edges, and §8's non-durable observation fact; it does not add a
durable field, activate v1, or authorize P1.

### 12.1 Anchored managed-leaf operations

The Git implementation adds `cap-std` and `cap-fs-ext` at the same reviewed
major version. On Unix it also declares direct target dependency `rustix` 1
with only `fs`, used to issue no-replace rename against the retained capability
directory descriptor; it does not reopen a pathname. It opens the workspace
root and Git directory as capability directories, traverses every managed
parent with explicit no-follow opens, verifies the opened directory identity,
and retains the parent handle through observation and mutation. A missing,
symlinked, replaced, or non-directory parent is `Ambiguous` except for the
exact marker-parent forms in §3.4.

`preservation_root/parent.rs` is the cohesive marker-parent namespace owner. It
owns deterministic staging-name derivation, anchored parent observation,
Unix no-replace publication/sync, and Windows handle-bound publication and
round-trip barriers. `files.rs` remains the managed-leaf owner; neither file
duplicates whole-root classification or durable phase policy from
`preservation_root.rs`.

Leaf observation uses no-follow metadata and an opened-handle identity check.
Replacement creates and synchronizes a unique temporary file relative to the
already-open parent, repeats the exact expected-source observation through
that same handle, then performs one handle-relative atomic rename and parent
sync. Removal similarly repeats the exact source observation and performs one
handle-relative unlink and parent sync. A parent-path replacement cannot
redirect either operation outside the anchored directory.

The exact race contract is containment plus a filesystem linearization point,
not a portable destination compare-and-swap that the selected APIs do not
provide. A parent or leaf replacement injected before the final anchored
source observation rejects without mutation. A genuinely concurrent leaf
replacement after that observation is ordered immediately before or after the
atomic rename/unlink; GWZ never follows it outside the parent, but does not
claim to preserve an adversarial writer's simultaneous replacement. If that
stronger CAS contract is required later, implementation stops for explicit
quarantine phases or platform-specific primitives rather than hiding another
physical mutation here.

Native stash is allowed to remove the empty canonical marker parent. No stash,
index, lock, reset, or marker-leaf step recreates it. Only the explicit
`NormalizeParent`, `PrepareParent`, or `RestoreParent` phase may stage an exact
empty directory and publish it as `markers` under anchored real `gwz.conf`,
using §5.4's deterministic-residue and no-replace-rename protocol. It never
directly creates the final directory or calls `create_dir_all`.

Directory-entry persistence is platform-exact. Unix uses handle-relative
rename and parent sync, with `AfterNeedsDurability` closing the crash window.
Windows does not call its no-op `sync_dir`; it uses §5.4's pinned-handle,
no-replace, write-through staging publication and final-to-staging-to-final
barrier. Unsupported filesystems or a failed pin/rename/sync produce a
typed error and no phase transition. Windows, macOS, Linux x86, and Linux ARM
release builds plus restart/fault evidence are mandatory.

Per the release policy, platform-build evidence is the existing `gwz-cli`
Cargo Dist release matrix, whose configured targets include Windows x86-64,
both macOS architectures, Linux x86-64, and Linux ARM64 and whose executable
links the exact `gwz-core` revision. Native/focused tests provide the restart
and fault evidence; a host-side cross-check that stops in a foreign C SDK is
not a substitute. The matrix must run on the committed corrected tree before
P1 is unblocked or the change is released.

### 12.2 Repository object format

The `git2` dependency enables its `unstable-sha256` feature, and the raw-index
owner adds the `sha1` digest crate alongside the existing `sha2` dependency.
Every commit and blob parse/hash uses `Repository::object_format()`,
`Oid::from_str_ext`, and `Oid::hash_object_ext`. SHA-1 facts must contain
exactly 40 lowercase hex
characters and SHA-256 facts exactly 64; a cross-format, abbreviated,
uppercase, missing, or non-object id rejects before mutation.

The dependency feature is explicitly experimental upstream. Accepting it here
is deliberate: the existing contract already requires real SHA-1 and SHA-256
repositories, and pretending that a 64-character type check supplied that
support was incorrect. Both formats must execute the complete preparation,
managed-index, stash, and reset evidence rows on supported release platforms.
The existing checked reset reaches `repository_support::parse_existing_commit`;
that shared parser must use the same repository-format-aware exact parse.
Duplicating checked-reset logic inside the preservation owner is forbidden.

### 12.3 Raw index boundary

A new cohesive `preservation_root/index_format.rs` owner parses the on-disk
`DIRC` header, versions 2, 3, and 4 entries, repository-format-dependent OID
width, entry stages/flags, extension frames, and trailing checksum before any
journal or mutation proof and again after an index rewrite. It recomputes
SHA-1 or SHA-256 over every byte preceding the trailer and requires exact
digest equality; merely locating or trusting the stored checksum is not
validation.

`TREE` is the only supported extension. It is an invalidatable cache, not
preservation authority. The parser validates every cache-tree node's path,
entry count, subtree count, optional repository-format OID, framing, nesting,
and exhaustion. After an actual managed-index rewrite, every `TREE` node whose
path is the root or an ancestor of either managed path must be invalidated
(`entry_count == -1` with no cached OID), or the checked mutation fails
post-verification. Unaffected cache nodes may retain their prior semantic
payload. The extension may be absent before and after the rewrite, but a
present post-write extension cannot claim a valid cached tree across the
changed marker or lock path. `REUC`, `NAME`, `UNTR`, `FSMN`,
`link`, `sdir`, `EOIE`, `IEOT`, every lowercase mandatory extension, every
unknown signature, malformed framing, checksum mismatch, and unsupported
index version reject before mutation. The post-write parse must have an exact
recomputed checksum, remain structurally valid, contain no unsupported
extension, satisfy the cache-tree invalidation rule, prove the exact managed
facts, and preserve every
unrelated raw path/stage/mode/accepted-semantic-flag/object-id tuple. No claim
is made that `TREE` bytes or the serialized index file remain identical.

### 12.4 Required corrective evidence

The existing focused Git and lifecycle evidence owners expand in place; no new
test file is introduced. The settled re-review requires table-driven coverage
for every row already frozen in §9, including:

1. both physical root-owner forms, every allowed handoff pair and all four
   clean/handoff marker-presence combinations, descendant
   `C0`, restore `C1`, fabricated/stale/missing/non-commit objects, unsupported
   tree modes, second markers, and every partial pre-journal form;
2. every normalize/prepare/restore row, including `RestoreParent`, allowed
   no-op, reordered/skipped/
   opposite/later-advanced form, wrong/detached ref, and foreign native state;
3. late staged, unstaged, untracked, ignored, raw non-UTF-8, managed-path, and
   boundary changes, including a boundary-dependent ignored/untracked flip;
4. real SHA-1 and SHA-256 repositories; index versions, stages, forbidden
   flags, supported `TREE`, and every named/unknown rejected extension;
5. file, symlink, directory, executable, missing/empty parent, exact/malformed/
   duplicate staging residue, foreign child, parent-replacement, and
   leaf-replacement cases with an outside-root sentinel proving containment;
6. faults before/after staging, rename, and Unix parent sync; Windows pinned-
   chain, staging-to-final publication, and both write-through round-trip rename
   boundaries, including second-rename failure with staging-only and collision
   with both names; lifecycle pending-without-attempt, matching success/failure
   over pending/`Before`/`Ambiguous`, crash-before-rewrite,
   stale/mismatched/second attempt; every leaf/index fault; complete-root restart classification,
   source-equals-goal parent no-op, and action-free cursor/exhaustion; and
7. directly constructed reset partials with checkout/index at `C1` and ref at
   `C0`, ref at `C1` and checkout/index at `C0`, and separately mixed index or
   worktree facts. Each must classify `Ambiguous`, reject checked execution,
   and retain identical ref/index/worktree bytes after that rejection.

The multi-owner/global-preflight no-mutation row remains a P1 integration gate
and cannot be inferred from these single-repository backend tests.

### 12.5 Corrective manifest and ceilings

The ten accepted production paths remain. The corrective slice adds exactly:

```text
gwz-core/Cargo.toml
gwz-core/src/git/gitbackend/repository_support.rs
gwz-core/src/git/gitbackend/preservation_root/parent.rs
gwz-core/src/git/gitbackend/preservation_root/index_format.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver/observation.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/resolver/execution.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/authority/observe/reverse/preservation.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/reverse.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/reverse/execute/preservation.rs
```

The twenty accepted evidence/tool/document paths remain. Dependency locking
and the causal lifecycle matrix add exactly:

```text
Cargo.lock
gwz-core/Cargo.lock
gwz-core/src/workspace_ops/merge/record_wire/unknown_fields/tests/preservation.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/authority.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/dispatcher_attempt_matrix.rs
gwz-core/src/workspace_ops/merge/v1_lifecycle/tests/dispatcher_reconciliation.rs
```

Both lockfiles are required: the workspace root drives integrated development,
while the member lockfile drives the independently released `gwz-core` repo.
`repository_support.rs` changes only its existing complete-commit parser from
`Oid::from_str` to `Oid::from_str_ext(value, repo.object_format())`; it adds no
preservation policy. The five defining lifecycle owners and four parent export
surfaces implement only the causal `PreservationDurabilityPending`
observation/execution/resolution seam in §8; the three existing lifecycle
evidence owners pin it. The focused unknown-field
child only moves the preservation-specific rows out of the existing 552-line
mixed-concept test owner; it adds no behavior or evidence budget. No
`merge_recovery.rs`, new service owner, or `durable_fs.rs` edit is added.

The corrected interface adds an explicit causal lifecycle row and raises the
stop ceiling rather than hiding it in P1. Evidence rises for the durable-parent
and attempt/restart matrices; the documents row retains its review reserve.
This is the accepted pre-§13 baseline; §13.3 replaces it only after §13 GO:

| P0.2 component | Net production | Moved production | Test/tool/doc | Production paths | Evidence paths |
| --- | ---: | ---: | ---: | ---: | ---: |
| shared Git implementation/testing | 1,900 | 450 | 3,200 | 11 | 9 |
| model/transition integration | 300 | 0 | 800 | 3 | 7 |
| lifecycle causal seam | 450 | 0 | 1,000 | 9 | 3 |
| documents/reviews/control | 0 | 0 | 3,200 | 0 | 7 |
| **P0.2 total** | **2,650** | **450** | **8,200** | **23** | **26** |

The shared Git and lifecycle rows are also charged to P1, which becomes 3,700
net production, 1,500 moved production, 6,000 test/tool/doc, and 24/19 charged
production/evidence paths. R4b-P becomes 13,250/2,950/27,900 lines with 174/158
charged paths and 104/106 unique production/evidence paths. The program
unique-once ceilings become 18,800 production and 35,400 evidence lines while
becoming 145/149 paths: the three resolver and three lifecycle evidence owners
already belong to earlier program checkpoints, while `parent.rs` and the
focused unknown-field child are new unique paths. Shared work is counted once.

Measured before remediation, P0.2 is 1,243 net production, 410 moved, and 780
code-test evidence lines. The revised corrective estimate is at most 1,050 net
production and 3,000 test/tool lines, leaving the package under every stop
ceiling. `files.rs` and `index.rs` remain below 450 lines; `parent.rs`,
`preservation_root.rs`, and every focused evidence owner remain below 500;
`index_format.rs` remains below 350 and the existing `preservation_image.rs`
remains unchanged at 491 lines.

The physical/state protocol, dependency disclosure, owner-only manifest
amendment, and reconciled control ledgers through §12 are GO. The later matrix
gap and its renewed pause are controlled by §13.

## 13. Pre-clean parent phase correction

The §12.4 four-way real-Git matrix found one state that the accepted graph
could observe but could not execute safely:

```text
source H marker absent + source parent missing + clean goal marker present
```

Preparation correctly accepted the missing and real-empty source parent as
equivalent exact forms. The first `NormalizeMarker` or `PrepareMarker` then
failed because anchored leaf replacement correctly refuses to create a
missing parent. Hiding `mkdir` in the marker step would create an unjournaled
physical edge and a crash-created mixed state. Requiring an empty source
parent would contradict the accepted absent-versus-empty equivalence. The
correct repair is therefore an explicit durable parent phase before each
pre-clean marker phase.

### 13.1 Exact new edges

The root candidate graphs gain only these phases and adjacencies:

```text
stash: normalize_parent -> normalize_marker
reset: prepare_parent -> prepare_marker
```

`BeginStash` now installs `NormalizeParent` as the first root stash phase, and
`BeginResetAttachedRef` installs `PrepareParent` as the first root reset phase.
Non-root, `NoCandidate`, and `EvidencePending` graphs remain byte-for-byte unchanged.
There is no extra durable field and no compound parent-plus-leaf action.

Both new phases map to:

```text
Managed(MarkerParentDirectory, Handoff -> AttachedClean)
```

They use the same checked parent primitive, deterministic full-form-bound
staging name, pinned `gwz.conf` handle, and causal attempt protocol as
`RestoreParent`. The physical classifier is direction-independent:

- source absent, goal present: missing final plus absent/exact-empty staging is
  `Before`; exact empty final plus no staging is `AfterNeedsDurability`;
- goal absent: missing final, exact-empty final, or the real canonical parent
  containing exactly the expected later source marker is ordinary proof-only
  `After`;
- source and goal present: the already-established exact parent is ordinary
  proof-only `After`; and
- both names, foreign residue/child, replacement, wrong marker position, or a
  missing parent after the durable edge is `Ambiguous`.

The executor may publish only the empty parent. The following marker phase is
the sole writer of marker bytes. A crash before publication leaves `Before`; a
crash after publication leaves `AfterNeedsDurability`; and a crash after the
record rewrite starts the marker phase with the durable parent exact. Windows
uses the already-reviewed write-through round trip and Unix uses the retained
parent sync. The §8 success/failure/prefix rules apply unchanged.

The serialized spellings are exactly `normalize_parent` and `prepare_parent`.
Validation, unknown-field identity, action identity, cursor/exhaustion,
reducer adjacency, and retired-spelling rejection must cover both. No released
v1 writer exists, so this closes the prerelease graph rather than migrating a
released record.

### 13.2 Ownership and evidence

No production or evidence path is added. The existing accepted model,
transition, authority, backend, and focused-test owners absorb the two enum
values, deterministic mappings, and table rows. In particular:

- `journal.rs` owns the enum values;
- preservation validation and reducer owners own root-only graph admission and
  exact successors;
- the existing authority observation/execution owners map the phases to the
  parent physical action;
- `preservation_root.rs` generalizes the parent classifier from restore-only
  to either direction without changing `parent.rs`; and
- the existing model, prefixed-preservation, unknown-field, G15 observation,
  mutation, fault, and causal-reconciliation evidence owners close the matrix.

Required new evidence is: both new phase spellings and rejection of their
non-root/action-free misuse; every predecessor/successor and owner form; all
four source/goal marker-presence combinations with missing and empty source
parents; Before, staging-only, `AfterNeedsDurability`, success/failure/restart,
source-equals-goal, foreign residue, both-name collision, and wrong-phase
authority; and unchanged short-graph/v0 characterization.

### 13.3 Provisional stop ceilings

The path counts and moved-production ceiling do not change. The added state
rows and the now-measured evidence expansion replace §12.5's numeric ceilings:

| P0.2 component | Net production | Moved production | Test/tool/doc | Production paths | Evidence paths |
| --- | ---: | ---: | ---: | ---: | ---: |
| shared Git implementation/testing | 2,200 | 450 | 3,800 | 11 | 9 |
| model/transition integration | 450 | 0 | 1,100 | 3 | 7 |
| lifecycle causal seam | 450 | 0 | 1,000 | 9 | 3 |
| documents/reviews/control/platform gate | 0 | 0 | 5,200 | 0 | 8 |
| **P0.2 total** | **3,100** | **450** | **11,100** | **23** | **27** |

Shared Git and lifecycle work is also charged to P1, whose ceiling becomes
4,000 net production, 1,500 moved production, and 6,600 test/tool/doc with
29/20 charged path counts. The five additional production paths are the
lead-approved cohesion split of the existing preservation observer into
`cursor`, `entry`, `phase`, `phase/evidence`, and `phase/steps` owners; the
parent remains a router and every resulting owner is below 500 lines. The six
dedicated preservation test owners were already reserved, so evidence counts
do not change. R4b-P becomes
14,000/2,950/31,400 lines with 179/160 charged paths. Program
unique-once ceilings become 19,250 production and 38,300 evidence lines with
150/150 paths. The conservative R4b-P unique path ceiling becomes 109/107.

The four export surfaces are already P0-owned and therefore add eight charged
P0.2/P1 appearances without increasing either production union. The one-path
evidence-union increase is P3's `gwz-py/src/tests/test_client.py`: the full
Python gate exposed its missing required-optional `MergeResponse.record`
fixture. It is not a P0.2 evidence owner and changes no line ceiling.

These remain stop ceilings. Both pre-implementation §13 interface reviews and
both corrected-tree settled-code reviews returned GO. With §14's allocated
workflow implementation present, the measured actual is
2,738/3,100 semantic production lines, 450/450 moved production lines, and
10,648/11,100 evidence lines across 23/27 implemented paths within the frozen
23/27 manifest. P0.2 is accepted locally; both §14 interface reviews are GO.
Both §14 settled-implementation reviews and the exact pushed-tree run are GO;
P1-WR1 is closed and P1 may resume against the frozen seam.

## 14. Non-publishing native platform gate

P1-WR1 found that `gwz-cli/.github/workflows/release.yml` can run the Cargo
Dist matrix only for an existing release tag and retains publishing authority.
Creating a tag or release merely to test a prerelease checkpoint is not an
acceptable gate. This section allocates exactly one new evidence/control path:

```text
gwz-cli/.github/workflows/platform-gate.yml
```

The workflow is manual `workflow_dispatch` only. It requires exact 40-hex
`gwz-cli` and `gwz-core` commit inputs and fails before checkout unless
`github.repository`, the path in `github.workflow_ref`, and
`github.workflow_sha` identify
`owebeeone/gwz-cli/.github/workflows/platform-gate.yml` at exactly the supplied
`gwz-cli` commit. It then checks out the two supplied commits as sibling
directories, verifies both checked-out `HEAD` values, and builds the CLI's
local Cargo Dist artifacts for the five targets already frozen in
`gwz-cli/dist-workspace.toml`:

```text
x86_64-pc-windows-msvc
aarch64-apple-darwin
x86_64-apple-darwin
aarch64-unknown-linux-gnu
x86_64-unknown-linux-gnu
```

It uses Rust 1.95 and Cargo Dist 0.31, runs each target on its native GitHub
runner, and may upload diagnostic workflow artifacts only. A contract job owns
one JSON matrix, rejects duplicate/missing/extra targets or runner drift, and
passes that exact matrix to the build jobs. Every row executes exactly:

```text
dist build --artifacts=local --target=<row-target> \
  --tag=v0.2.0-dev --print=linkage --output-format=json
```

The explicit `v0.2.0-dev` value is Cargo Dist's non-implicit announcement
selector matching the frozen CLI package version; it is not created as a Git
tag or release. The post-build check requires `dist_version: 0.31.0`,
`announcement_tag: v0.2.0-dev`, `announcement_tag_is_implicit: false`, and
exactly the row target's executable archive plus checksum from
`dist print-upload-files-from-manifest`; any other target output fails the row.

The workflow's permissions are `contents: read`; it has no release event, tag
creation, host/create/upload/release/announce invocation, attestation, GitHub
Release, package publication, or release-asset upload step. Failure or
cancellation of the contract job or any target fails the gate.

The workflow source owns its static five-row matrix and an early self-check
that rejects loss of the exact inputs, workflow repository/path/SHA binding,
read-only permissions, five targets, or the presence of release/publishing
commands. No second test path is allocated. Every target writes a gate-identity
artifact and job summary containing the workflow repository, workflow path,
`github.workflow_sha`, both supplied and observed checkout SHAs, and target.
The actual successful run retains those identities plus each target's Cargo
Dist manifest/artifacts.

This path is charged once to P0.2 and once again to P1. It raises P0.2 evidence
paths from 26 to 27, P1 charged evidence paths from 19 to 20, and R4b-P charged
evidence paths from 158 to 160. Because it is new to both unions, R4b-P unique
evidence paths rise from 106 to 107 and program-unique evidence paths from 149
to 150. Numeric line ceilings do not change. The workflow was not authored
until both independent interface reviewers accepted this section, and P1 was
paused until the implemented workflow received focused settled-code reviews
and its exact-SHA five-target run succeeded; all of those gates are now GO.
