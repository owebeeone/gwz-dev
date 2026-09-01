# GwzM5-8 R2-E E4.5/6-B — charter prep (read-only investigation)

Date: 2026-09-02. Author: a read-only investigation session for the
implementation lane. **Status: INPUT TO E4.5/6-B's CHARTER. Not an
amendment, not a plan change, not a build.** No file in `gwz-core` was
modified; no build, no test, no commit was run. Every claim below carries a
`file:line` citation opened and read in this session.

Trees read: `gwz-core` at main `0dae0d5` ("E4.3-B landing nit: the
`production_rust_files` cite re-pointed to `:982-989`"; `git status` clean)
and `gwz-dev/dev-docs` at the same tip (`3351895`).

Authorities consumed, and treated as FALSIFIABLE throughout:
`GwzM5-8R2E-CapabilityFreeAmendment.md` (ADOPTED) §1's table, §2, §3's cell
table, §6, §7's "convertible forward arms" paragraph;
`GwzM5-8R2E-CapabilityFreeAmendment-ReviewCode.md` [P1-1] (the reachability
chain) and [P2-4] (the door census); `GwzM5-8R2E-RecordRootAmendment.md`
§1a (the record-root analysis duty), §2 (the exception), §4 (the PLAIN-LEASE
PROBE CLAUSE); `GwzM5-8R4bR2ConsumerCheckpoint.md` §10 rows `:277`–`:279`
and the 2026-09-02 dated bracket; `GwzM5-8R2E-E4.2-Review.md` flag-6 /
C1(iii) (the O14 interim gate-statement duty) and its §7.5-quoted E4.4-prep
findings; `GwzM5-8R2E-E4.4-CharterPrep.md` §2/§3.

---

## 0. Headline answers

1. **Reachability RE-VERIFIED, and the amendment's last column is CORRECT
   for all three sites.** `finalization/execute.rs:45`, `:48` and `:51` are
   reached under `V1MutationLease::acquire_activated` and under nothing
   else. No reverse path, no plain-lease path, no listed operation. The
   chain is closed at four independent joins, each re-driven from the tree
   in this session (§1). Nothing in item 1 falsifies the amendment.

2. **The record-root analysis finds a WALL on two of the three leaves, and
   the wall is NOT the amendment's or the prep's.** `:45` (marker) is
   window-free: the publication machine issues `WriteMarker` only when the
   marker is ABSENT, so the checked expected fact is `Missing`,
   `replace_exact` never detaches (`transition.rs:91-92`), and a missing
   marker is already a first-class observed state on both the forward
   (`acceptance/publication.rs:90,99-100`) and the abort
   (`abort/evidence.rs:313-318`) sides. `:48` (lock) and `:51` (boundary)
   are `Bytes → Bytes` replacements: `replace_exact` DETACHES the leaf
   (`transition.rs:91-92` → `detach_existing`), and during that window the
   leaf is ABSENT — a state the shipped forward observer **and** the shipped
   abort preflight both classify as unclassifiable. A crash there drives
   the record to `RecoveryRequired` and `gwz merge --abort` then REFUSES at
   preflight. Driven-by-reading in §2. **This is a stop-and-report trigger
   under RR §4 / RR §1a.**

3. **The `:279` "git-directory replace door" is NOT required, and the
   amendment's premise for it is falsified by the shipped tree.** The v1
   REVERSE arm already writes `.git/info/exclude` through the **workspace**
   door — `abort/evidence.rs:148-153` → `artifact_facts::write_checked(root,
   boundary_relative(root), …)` → `entry::replace_merge_root_artifact` →
   `CheckedArtifactPolicy::workspace(root)` — with `boundary_relative`
   (`abort/evidence.rs:334-341`) yielding the workspace-relative
   `".git/info/exclude"`. So a checked replace of that leaf exists in
   production today, on the plain lease, and it is not a Git-directory
   action. The forward conversion's cheapest and most symmetric route is the
   same seam (§3). A genuine Git-directory replace door is buildable — the
   root kind exists, `CheckedArtifact::acquire` supports it, and
   `parent_is_canonical` is exactly the "observe existing Git-owned parent;
   never bootstrap" predicate — but it would be the FIRST production write
   under `<git-dir>/gwz/`, which falsifies `policy.rs:41-45` and opens a
   residue-location question of its own. Estimate and cost in §3.

4. **Recommended shape: SPLIT, not one step, and one arm does not start.**
   E4.5-B = `:277` only (`execute.rs:45`), ~180–250 LOC, clean.
   E4.6-B = `:278`/`:279` — **STOP AND REPORT before building**: its cure is
   a change to the publication OBSERVATION vocabulary (v1-only, but frozen
   surface), which the operator's ruling did not authorize. Three routed
   options in §7.

---

## 1. THE THREE SITES, AND THE REACHABILITY RE-VERIFICATION

### 1.1 What each site writes

`src/workspace_ops/merge/v1_lifecycle/finalization/execute.rs` is 64 lines.
`publication()` (`:8-64`) is its only function. Five `PublicationPhysicalAction`
arms; two are git-backend seams, three are raw durable file writers:

| Site | Action arm | Raw primitive | Absolute path written | Parent | Bytes source |
| --- | --- | --- | --- | --- | --- |
| `:45` | `WriteMarker` (`:38-46`) | `artifact::write_atomic` (`artifact/mod.rs:492-495`) | `<root>/gwz.conf/markers/<marker_id>.yaml` — `progress.candidate_marker_path` (`:39-44`), built at `acceptance/v1_candidate.rs:103` / `finalize_dispatch.rs:232-236` from `MARKER_DIR` (`artifact/mod.rs:32`) | `gwz.conf/markers` = `ManagedParentPurpose::RootPreservationMarkers` (`bootstrap/managed.rs:92`) | `candidate.marker_yaml` (`model/v0.rs:207`) |
| `:48` | `WriteLock` (`:47-49`) | `artifact::write_atomic` | `<root>/gwz.conf/gwz.lock.yml` — `LOCK_PATH` (`artifact/mod.rs:30`) | `gwz.conf` — not a managed-parent purpose | `candidate.lock_yaml` (`model/v0.rs:206`) |
| `:51` | `WriteBoundary` (`:50-52`) | `publish_workspace_exclude_candidate` (`sync_workspace_boundary.rs:84-86`) → `artifact::write_atomic` on `workspace_exclude_path(root)` (`:88-90`) | `<root>/.git/info/exclude` (a hard-coded `.git` join, NOT `repo.path()`) | `.git/info` — Git-owned | `candidate.boundary_text` (`model/v0.rs:209`) |

Both remaining arms are git seams, not file writers: `:31`
`backend.commit_gwz_paths_checked` (the `_checked` git seam the amendment's
negative-scan needle must exclude, §3 of CapFree) and `:60`
`backend.stage_paths`. **`finalization/execute.rs` therefore contains
exactly the three convertible durable-writer sites and no fourth.**

`write_atomic` → `stage_durably` (`artifact/mod.rs:519-537`) →
`fs::create_dir_all(parent)` at `:521`, then a temp write + `sync_all`
(`:528-530`), then `publish_staged` — an ATOMIC rename onto the destination.
**The raw path never leaves the destination absent.** That fact is the
baseline against which §2's detach window must be judged.

### 1.2 The reachability chain, re-driven

Each join was re-derived by grep + read in this session, not taken from the
amendment:

1. `execute::publication` has ONE caller in the whole tree:
   `finalization.rs:53` (`grep -rn "execute::publication" src/` → one hit).
2. `finalization.rs:53` sits inside `FinalizationRuntime::execute`
   (`:34-55`), guarded by a non-publication refusal (`:41-46`) and a
   lease-coverage refusal (`:47-52`).
3. `FinalizationRuntime::new` has exactly ONE production site:
   `forward.rs:69`, inside `ForwardRuntime::execute`'s
   `PhysicalActionKind::Publication(_)` arm (`:68-70`). Every other
   `FinalizationRuntime::new` in the tree is under `tests/` —
   `tests/finalization.rs:131,200,736,779,826,863`,
   `tests/finalization_inputs.rs:77,123,318,367,406`,
   `tests/finalization_root.rs:21`.
4. `ForwardRuntime::new` has exactly TWO production sites: `start.rs:89`
   (before `service::run(…, ResumeStart, …)` at `:90-97`) and `start.rs:160`
   (before `service::run(…, Continue, …)` at `:161`). All other sites are
   under `tests/`.
5. `service.rs:116-121` forks the lease: `ResumeStart | Continue` →
   `V1MutationLease::acquire_activated(root)` (`:118`); everything else →
   `V1MutationLease::acquire(root)` (`:120`). `acquire_activated`
   (`checked.rs:158-164`) is `acquire` plus
   `entry::activate_workspace_catalog` — the probe.
6. **No other runtime can carry a Publication action.**
   `ReverseRuntime::execute` refuses `PhysicalActionKind::Publication(_)`
   explicitly (`reverse.rs:78-82`; the `Publication(_)` pattern is `:79`).
   `ForwardRuntime::execute` symmetrically refuses
   `Preservation/Rollback/Archive` (`forward.rs:71-76`).
7. **No other production `service::run` exists** beyond `start.rs:90`,
   `start.rs:161`, `start.rs:165` and `archive.rs:79`. `start.rs:165` builds
   a `ReverseRuntime` (`:164`) for `Abort`/`Preserve`; `archive.rs:78-85`
   builds a `ReverseRuntime` for `V1LifecycleRequest::Archive`. Both take
   the plain lease at `service.rs:120`.
8. Adversarial check on the sealed trait: `FinalizationRuntime` is a
   `sealed::ProductionRuntime` (`service.rs:44`), so `service::run` COULD in
   principle be called with one directly — **no production caller does**;
   the only `FinalizationRuntime` construction outside tests is
   `forward.rs:69`, which is already inside the activated fork.

**Verdict on item 1: the amendment's last column is CORRECT for all three
sites; not one of the three is reachable from anything but
`acquire_activated`.** No arm is disqualified from conversion on
reachability grounds. (This also re-confirms the ReviewCode [P1-1] correction
that `:279`'s "SOLELY create/init … no fork" was wrong.)

### 1.3 Two cite corrections owed at the landing

* `policy.rs:42` names the git-directory acquire site as `entry.rs:182`. At
  `0dae0d5` it is `entry.rs:188` (inside `preservation_git_directory`,
  `:186-193`). Stale by one landing; the charter's builder should re-point
  it if it touches the file.
* The amendment's §1 chain cites `service.rs:117-119` for
  `acquire_activated`; the exact call is `service.rs:118` inside the
  `:116-121` block. Cosmetic; recorded so a later reader is not misled.

---

## 2. RECORD-ROOT ANALYSIS PER LEAF (RR §1a's duty, discharged BEFORE a build)

The question RR §1a makes mandatory: *is this leaf its own discovery root —
would a detach-then-publish window strand anything the lifecycle cannot
reconcile?*

### 2.0 The mechanism, restated exactly

`CheckedArtifact::replace_exact` (`transition.rs:38-116`) detaches the
existing leaf **only** when the authority's retained source is
`RetainedSource::Existing(_)` (`:91-92`); on the `Missing`-source arm it
goes straight to `publish_goal` (`:93-104`). `classify_exact`'s operation
table (`classification.rs:148-154`) sets `MissingReplace` exactly when
`expected == Missing`. So:

> **A converted arm whose expected fact is `Missing` has NO detach window.
> A converted arm whose expected fact is `Bytes(_)` HAS one.**

### 2.1 `:45` — the marker (`gwz.conf/markers/<id>.yaml`), row `:277`

**Its own discovery root? NO.** The marker is discovered from the merge
record: `progress.candidate_marker_path` (`model/v0.rs:181`),
`candidate.marker_yaml`/`marker_sha256` (`model/v0.rs:207,211`). Both the
forward snapshot (`live.rs:100-108`) and the abort's `file_states`
(`abort/evidence.rs:306-319`) reach it only through the record.

**Detach window? NONE.** `resolve_candidate` issues `WriteMarker` only from
`(CandidatePublicationPrefix::Baseline, IndexForm::Pre)` (`live.rs:64-66`),
and `Baseline` requires `marker_absent` (`acceptance/publication.rs:90,
99-100`). The converted expected fact is therefore `Missing` — the
`create_merge_store_record` shape (`entry.rs:390`) exactly — and
`replace_exact` never detaches.

**What reconciles it today on restart?** `restart_after(WriteMarker)`
(`tests/finalization.rs:602-645`) drives it: the resumed run re-observes,
sees prefix `Marker`, and continues at `WriteLock`. Missing-marker is
`Baseline` → re-issue `WriteMarker`. On the abort side `classify_file(…,
missing_is_baseline = true)` (`abort/evidence.rs:313-318`, `:356`) makes an
absent marker `FileState::Baseline` — the rollback's "after" state. Both
directions already tolerate absence.

**Can `replace_exact` serve it?** YES, and it is the `MissingReplace` arm
RR §1a itself blesses ("E4.2's create side was safe: `MissingReplace` never
detaches"). **No E4.3 wall.**

### 2.2 `:48` — the lock (`gwz.conf/gwz.lock.yml`), row `:278`

**Its own discovery root? NO for discovery, YES for observation.** The merge
record survives, so `--abort` can still FIND the merge. But every observer
of the publication prefix requires the lock to be present AND to digest to
either the baseline or the candidate:

* Forward: `live.rs:107` reads it, and `:110-112` returns `Ok(None)` unless
  the lock is `FileDigest::Regular`. `None` → `resolve_candidate(_, None)`
  → `_ => PublicationResolution::Ambiguous` (`live.rs:90`) →
  `publication.rs:441,455` → `ambiguity(current)`
  (`observe/finalization.rs:464-…`), which issues `BoundAmbiguityEvidence`
  and drives the record into the RECOVERY origin.
* Abort: `preflight_v1_evidence` (`abort/evidence.rs:34-52`) fires whenever
  candidate + composition_commit are set and evidence is not rolled back
  (`:42-45`) — true throughout publication. It calls
  `evidence_shape_is_exact` (`:390-404`) → `file_states(root, record,
  None)` → for the lock, `classify_file(observe(LOCK_PATH), candidate,
  baseline, /*missing_is_baseline=*/ false)` (`:299-304`). `Missing` with
  that flag false → `FileState::Other` (`:356-359`). `Other` matches no
  shape (`:204-219`), so `classify_v1_evidence_rollback` returns
  `Ambiguous`, `evidence_shape_is_exact` is false, and `preflight_v1_evidence`
  returns `Err("publication evidence is not at an exact rollback-representable
  state")`. That `?` propagates from
  `observe/reverse/rollback.rs:311`. **`gwz merge --abort` refuses.**

**So: a crash inside the lock's detach window leaves the record in
RecoveryRequired with `--abort` refusing.** The raw `write_atomic` path has
no such state — its rename is atomic.

**Is it recoverable in principle?** Yes, and this is the one respect in
which it is *better* than RR §1a's record root: the `(expected, goal)` pair
— `candidate.baseline_lock_yaml` → `candidate.lock_yaml` (`model/v0.rs:205-206`)
— lives in the surviving record, so the boundary's own
`ExactTransition::RecoverableDetached` convergence IS reachable if the same
`replace_exact` is re-issued. **The wall is in the OBSERVERS, not the
writer.** Nothing re-issues, because both observers refuse first.

**Cure shape (for §7's estimate).** Two v1-only observer changes:
(i) `live.rs::snapshot` must, on a Missing lock, consult
`artifact_facts::classify_write(root, LOCK_PATH, baseline, candidate)` and
treat `Recoverable` as "still at the pre-write prefix"; (ii)
`abort/evidence.rs::file_states` must do the same in its `pending == None`
arm. `acceptance/publication.rs::classify_candidate_parts` must NOT change —
it is shared with the v0 lane through `classify_candidate_publication`
(`:43-51`) and `classify_candidate_publication_view` (`:53-69`), and v0 is
carved. The cure must therefore normalise inside `live.rs` before the
classifier is called.

### 2.3 `:51` — the boundary (`.git/info/exclude`), row `:279`

**Its own discovery root? NO, and it is the most benign of the three by
content**: the file is local, never committed, and REGENERATED from the
manifest + lock on every run (`sync_workspace_boundary.rs:30-45,48-82`).
Its loss costs only that members stop being hidden from `git status`.

**But its OBSERVATION is as strict as the lock's.** `live.rs:109` digests
it; `:118-122` maps Missing to `None`; `classify_candidate_parts:93-95`
accepts a `None` boundary as `baseline_boundary` **only when
`candidate.baseline_boundary_text.is_empty()`**. For the ordinary case (a
`git init`-created exclude file with template comments, so a non-empty
baseline) an absent boundary classifies as no prefix → `None` → `Ambiguous`
→ recovery. On the abort side, `classify_file(…, missing_is_baseline=false)`
(`abort/evidence.rs:284-289`) → `Other` → the same preflight refusal as the
lock. **Same wall.**

**One conversion subtlety the charter must name.** The converted expected
fact is `Bytes(baseline_boundary_text)` in the normal case but must be
`Missing` when the baseline text is empty *and the file is absent* (the
second disjunct of `:93-95`) — because `Bytes(b"")` and `Missing` are
different facts to `classify_exact`. `boundary_text` itself is never empty
(the BEGIN/END block is unconditional, `sync_workspace_boundary.rs:65-72`),
so only the expected side branches.

### 2.4 The abort-side residue interaction, precisely (feeds §6)

`classification.rs:155-177` is decisive: an authority record present whose
`matches_request(self, expected, goal)` disagrees with the caller's pair
returns `Ambiguous` outright (`:175-177`). The abort's `file_states` uses
the REVERSE pair (`classify_write(root, …, expected = candidate, goal =
baseline)`, `abort/evidence.rs:277-282,292-297`), while a forward detach
window's residue was minted for the FORWARD pair. So even the abort's
`pending == Some(step)` arm — which does map `Recoverable → Candidate`
(`transition_file`, `:325-332`) — cannot rescue a forward-pair residue: it
sees `Ambiguous` → `FileState::Other`. The residue vocabulary is
directional, and the charter cannot assume the abort's existing
`Recoverable` tolerance covers the new window.

### 2.5 Summary table

| Leaf | Own discovery root? | Detach window under `replace_exact`? | Forward observer on the window | Abort observer on the window | Verdict |
| --- | --- | --- | --- | --- | --- |
| `:45` marker | No (record-derived) | **None** — expected `Missing` | absent = `Baseline` → re-issue | absent = `Baseline` (`:356`) | **CLEAR** |
| `:48` lock | No for discovery; yes for observation | **Yes** | `None` → `Ambiguous` → recovery | `Other` → preflight `Err` | **WALL** |
| `:51` boundary | No (regenerable) | **Yes** (except empty-baseline case) | `None` → `Ambiguous` → recovery | `Other` → preflight `Err` | **WALL** |

---

## 3. THE `:279` DOOR — feasibility, necessity, and size

### 3.1 What `entry.rs` has at `0dae0d5`

Doors, complete (`src/checked_artifact/entry.rs`):

| Door | Root kind | Verb |
| --- | --- | --- |
| `observe_merge_root_artifact` `:36-41` | workspace (`root_artifact` `:159-166`) | observe |
| `replace_merge_root_artifact` `:43-51` | workspace | `replace_exact(Bytes(expected), goal)` |
| `classify_replace_merge_root_artifact` `:53-63` | workspace | classify |
| `remove_merge_root_artifact` `:65-71` | workspace | `remove_exact` |
| `classify_remove_merge_root_artifact` `:73-82` | workspace | classify |
| `observe_merge_preservation_workspace` `:84-90` | workspace (`:177-184`) | observe |
| **`observe_merge_preservation_git_directory` `:92-98`** | **git directory (`:186-193`)** | **observe ONLY** |
| `replace_merge_preservation_workspace` `:100-107` | workspace | replace-or-remove (Option/Option) |
| `classify_merge_preservation_workspace` `:109-116` | workspace | classify |
| bundle trio `:118-148` | workspace | observe/classify/replace + canonical-parent gate |
| `prepare_merge_store_parents` `:150-157` | — | `CheckedArtifact::prepare_parent` |
| `create_merge_store_record` `:371-391` | workspace | parent gate + `replace_exact(Missing, goal)` |

Confirmed by `grep -n "git_directory" src/checked_artifact/entry.rs` →
`:92`, `:186`, `:188`, `:336`. **There is no git-directory replace door.**
The amendment's `:279` NOTE is factually right about `entry.rs`.

### 3.2 The amendment's PREMISE for that door is falsified by the tree

`abort/evidence.rs:148-153` — the shipped v1 evidence rollback, reached in
production from `v1_lifecycle/reverse/execute/rollback.rs:62` on the PLAIN
lease — executes:

```
EvidenceRollbackStepV1::Boundary => artifact_facts::write_checked(
    root, &boundary_relative(root)?,
    candidate.boundary_text.as_bytes(),        // expected
    candidate.baseline_boundary_text.as_bytes(), // goal
)
```

with `boundary_relative` (`:334-341`) = `workspace_exclude_path(root)`
stripped of `root` = the workspace-relative string `".git/info/exclude"`,
and `artifact_facts::write_checked` (`root/artifact_facts.rs:39-51`) →
`entry::replace_merge_root_artifact` → `root_artifact` →
`CheckedArtifactPolicy::workspace(root)` (`entry.rs:159-166`).

**So `.git/info/exclude` is ALREADY written through the checked boundary in
production, on the WORKSPACE root kind, today.** Consequences the charter
must record:

* Row `:279`'s cell 2 ("checked **Git-directory** artifact action") is
  already not what the shipped converted writer does. Nobody has recorded
  that. It is a pre-existing, unlogged discrepancy — not something E4.6-B
  introduces.
* Row `:279`'s cell 3 ("observe existing Git-owned parent; never bootstrap
  `.git/info`") is nonetheless SATISFIED by that writer, structurally:
  `replace_exact` refuses when `ParentState` is not `Open`
  (`transition.rs:61-67`), `classify_exact` returns `Ambiguous` for the same
  (`classification.rs:130-139`), and nothing in the `CheckedArtifact` write
  path calls `prepare_parent`. The boundary CANNOT bootstrap `.git/info`.
* Therefore the *cheapest correct* `:279` conversion is
  `artifact_facts::write_checked(root, &boundary_relative(root)?,
  baseline_boundary_text, boundary_text)` — the exact inverse of the shipped
  reverse arm — **and it needs no new door at all.**

### 3.3 If a real Git-directory replace door is nonetheless wanted

It is **feasible**. Everything it needs exists:

* Root kind: `CheckedArtifactPolicy::GitDirectoryArtifact` (`policy.rs:10`,
  constructor `:20-24`), private area `CatalogPrivateNameV1::LegacyPrivate`
  over `CatalogPrivateRootV1::GitDirectory` = `gwz/checked-artifacts` under
  the git dir (`policy.rs:51-53`, `catalog_names.rs:12-17`).
* `CheckedArtifact::acquire` is root-kind agnostic (`observation.rs:82-121`)
  — it takes `policy.artifact_root()` and probes it (`:96`) and the parent
  (`:105`). The git-directory arm is proven live in production today by
  `observe_merge_preservation_git_directory` ← `preservation_root/files.rs:28-35`,
  which passes `git2::Repository::open(root)?.path()` as the artifact root
  and `Path::new("info/exclude")` as the relative path — i.e. **the exact
  target `:279` would need, already acquired in production.**
* The "observe existing Git-owned parent; never bootstrap" clause maps
  ONE-TO-ONE onto `CheckedArtifact::parent_is_canonical`
  (`observation.rs:159-164`). That predicate is a pure filesystem check —
  parent traversable, a real directory, not a symlink, identity stable
  (`traverse` + `parent_is_current`, `:166-176`) — **it does not require a
  catalog-MANAGED parent.** So a Git-owned parent passes it, and no
  managed-parent session, no `ManagedParentPurpose`, no `create_dir_all` is
  involved. E4.2's `create_merge_store_record` (`entry.rs:371-391`) supplies
  the door shape verbatim: acquire → `if !artifact.parent_is_canonical()? {
  return Err(...) }` → `replace_exact`, with the refusal message saying the
  clause out loud rather than leaving the generic ambiguity.

It carries **three costs the workspace route does not**:

1. It would be the **first production write under `<git-dir>/gwz/`**,
   falsifying `policy.rs:41-45` verbatim ("no production write lands under
   `<git-dir>/gwz/` through this policy ([P3-7])") and requiring that
   comment, and the R2-F `RelocationPlan.md` §1 reasoning it cites, to be
   re-argued. The git-status dirt exemption
   (`repository_support.rs:118-151`) covers only the two workspace-relative
   private paths (`:141-151`) — a `<git-dir>/gwz/` residue needs none (it is
   inside `.git`), but it also gets no `preservation_image` treatment and no
   cleanup coverage. That analysis is charter-tier on its own.
2. **Two different path resolutions would coexist for one leaf**: the raw
   family and the v1 reverse arm both resolve `<root>/.git/info/exclude`
   (`sync_workspace_boundary.rs:88-90`), while the git-directory door
   resolves `repo.path()/info/exclude`. For a LINKED WORKTREE these differ
   (`<root>/.git` is a file there), so the forward and reverse arms would
   write different paths. `workspace_mutator_lock.rs:189`
   (`linked_worktree_uses_its_actual_git_directory_for_the_guard`) shows the
   tree does contemplate linked worktrees.
3. New `ENTRY_REFERENCES` and `ENTRY_ITEMS` rows in the checker
   (`check_checked_artifact_boundaries.py:392-459`, `:461+`), plus the
   `entry.rs` byte-digest refresh.

**Size estimate.** Workspace route (recommended): **0 LOC of new door**;
the call site changes from one line to ~8 in `execute.rs` plus the expected-fact
branch of §2.3. Git-directory-door route: **~60–90 LOC production**
(`entry.rs` door + a `sync_workspace_boundary`/`preservation_root`-side path
resolver + the git-dir acquisition in the caller), **~150–250 LOC of driven
rows** (door fault matrix, parent-missing refusal, linked-worktree row), plus
the `policy.rs` / RelocationPlan re-argument and the residue-location
analysis — which is not LOC-bounded and is the real reason to decline it.

---

## 4. THE CELLS OWED (CapFree §3's last column)

### 4.1 `:277` — "all ordinary preflight first; parent durable before member/root mutation"

**What the converted marker write must prove about its parent.**
`gwz.conf/markers` IS a declared managed purpose —
`ManagedParentPurpose::RootPreservationMarkers`, components `[gwz.conf,
markers]`, minimum retained parent count 1 (`bootstrap/managed.rs:63-68,
87-94, 96-101`).

**Who bootstraps it on the v1 forward path today? NOBODY.** E4.2's
merge-start session is sealed to `[MergeStore, PreservationBundles]`
(`ManagedParentBootstrapRequest::for_merge_start`, `managed.rs:119-128`;
enforced at `:190-196`), reached from
`V1MutationLease::acquire_for_merge_start` (`checked.rs:175-181`) ←
`start.rs:83-85`. `RootPreservationMarkers` is NOT in it. The only
production creator of `gwz.conf/markers` in the whole tree is
`handle_commit.rs:334`'s raw `fs::create_dir_all` (the row's own
accepted-unmet "when missing" clause) — plus the implicit creation every
`write_atomic` performs at `artifact/mod.rs:521`.

**So E4.5-B has exactly three routes, and must pick one in its charter:**

* **(a) Refuse when missing** — the `create_merge_store_record` shape
  (`entry.rs:384-389`): `parent_is_canonical()` false → typed refusal. This
  is a **behaviour change**: a workspace that has never run `gwz commit` and
  whose `gwz.conf/markers` does not yet exist would fail its first `--no-ff`
  publication where today `write_atomic` silently creates the directory.
  The charter must drive whether that topology is reachable; nothing in the
  tree guarantees `gwz.conf/markers` exists before the first marker write.
* **(b) Bootstrap through the sealed DurableMerge authority.**
  `ManagedParentRequestAuthorityV1::DurableMerge` exists and permits exactly
  `{PreservationBundles, RootPreservationMarkers}`
  (`managed.rs:132-147`, `:197-203`), and
  `CheckedManagedActionV1::for_durable_merge` (`coordinator/identity.rs:262-266`)
  builds it — but **it has ZERO production callers today** and no `entry.rs`
  door. Taking (b) means minting a `bootstrap_durable_merge_parents`-class
  door with the two-lease admission/execution session
  (`coordinator/execution.rs:130-143`, `:161-172`), new `ENTRY_REFERENCES`
  rows, and a placement decision (the marker parent would have to be
  bootstrapped at merge start, i.e. in `acquire_for_merge_start`, since the
  publication lease is `acquire_activated` and creates no parent by design —
  `checked.rs:166-174`). **That is a second, larger step, not a clause of
  this one.**
* **(c) Bootstrap it in E4.2's session** by widening `for_merge_start`'s
  sealed purpose set. That changes a FROZEN sealed request (`managed.rs:119-128`
  is explicitly "the one reproducible pre-record request. Its fixed purpose
  set prevents callers from deriving a different resident action after a
  crash") and moves row `:273`. **Amendment-tier; out of scope.**

Recommendation in §7: **(a)**, with a driven row proving the refusal is
typed and a driven row proving the ordinary topology has the parent —
because (b)/(c) each exceed the step's cap and (a) is what the frozen cell
literally asks for once "when missing" is read as row `:277`'s *raw* arm's
accepted-unmet clause, which CapFree §3 already dispositions as staying raw.

The "all ordinary preflight first" half is SATISFIED by position:
`WriteMarker` is issued only after `EvidenceCommit`, from the `Baseline`
prefix (`live.rs:64-66`), i.e. after acceptance, candidate preparation and
the composition commit.

### 4.2 `:278` — "exact source/goal and existing managed parent"

**Exact source/goal**: satisfied by construction once converted — expected
`Bytes(candidate.baseline_lock_yaml)`, goal `Bytes(candidate.lock_yaml)`
(`model/v0.rs:205-206`), the exact inverse of the shipped reverse arm
(`abort/evidence.rs:154-159`). The prefix that issues `WriteLock` requires
`baseline_lock` (`live.rs:67-69` + `acceptance/publication.rs:101-102`), so
the expected fact is *proved* by the observation that selected the action —
not assumed.

**Existing managed parent**: `gwz.conf` is NOT a `ManagedParentPurpose` —
the four are `MergeStore`, `MergeArchive`, `PreservationBundles`,
`RootPreservationMarkers` (`managed.rs:63-68`). So "managed parent" cannot
be read literally here. The available predicate is `parent_is_canonical`
(§3.3), which the shipped reverse arm already relies on implicitly
(`transition.rs:61-67`). The charter should say so explicitly and record
that the cell's "managed" is discharged as "canonical, existing, never
bootstrapped" — the same reading `:279` gets — or the cell is unmeetable.

### 4.3 `:279` — as §3

Cell 3 is met by the workspace route structurally (no bootstrap is possible
through the boundary). Cell 2's "Git-directory" wording is already at odds
with the shipped reverse arm; §7 routes that.

### 4.4 Which tests pin these cells for the RAW arms today

The raw arms are pinned **behaviourally, not by a boundary assertion**:

* `tests/finalization.rs:15`
  `concrete_finalizer_freezes_acceptance_and_publishes_exact_candidate` —
  pins the exact five-action sequence `[EvidenceCommit, WriteMarker,
  WriteLock, WriteBoundary, StageIndex]` (`:78-90`) and the resulting index
  state.
* `tests/finalization.rs:157`
  `restart_reconciles_every_owned_publication_mutation_prefix` →
  `restart_after(target)` (`:602-645`), which crashes after each action and
  asserts the resumed run replays exactly the remaining suffix (`:630-644`).
  **This is the family the conversion must extend**, because today its fault
  points are all *between* actions; the conversion creates a new fault
  surface *inside* one (`fail_next_checked_artifact_at(BeforeManagedPublication)`,
  the E4.3 idiom).
* `tests/finalization.rs:170`
  `tampered_owned_publication_prefix_enters_recovery_without_overwrite`.
* `tests/finalization.rs:222,323` — the reverse-handoff phase classifiers,
  which read the same three leaves.
* `tests/predecessor_matrix.rs:152`,
  `tests/publication_attempt_sequence.rs:212`,
  `tests/dispatcher_attempt_matrix.rs:340` — action-name pins only.

**Driven rows the conversion owes** (new, per arm):

1. E4.5-B: marker published through the boundary; residue family present and
   converged; `restart_after(WriteMarker)` still exact.
2. E4.5-B: the parent-missing refusal is typed and does NOT create
   `gwz.conf/markers` (a negative filesystem assertion after the refusal).
3. E4.5-B: an interrupted checked marker publication
   (`BeforeManagedPublication`) converges on re-issue AND leaves
   `gwz merge --abort` working — the RR §1a-style transcript row, run as
   evidence rather than argued.
4. E4.6-B (if taken): the same three for lock and boundary, PLUS the
   detach-window rows: crash inside `replace_exact`, then (i) resume
   converges, (ii) `--abort` succeeds — both of which fail today (§2).
5. E4.6-B: the empty-baseline boundary case (`expected == Missing`).

---

## 5. TRIPWIRE / PIN IMPACT

### 5.1 A `recover_or_create` namer? **NO.**

`interface_tests/catalog_activation_pin.rs:45` pins
`PRODUCTION_CALLER_COUNT = 1` — production FILES outside the owner naming
`recover_or_create`. The only two callers are inside `entry.rs`
(`activate_workspace_catalog` `:310`, `bootstrap_merge_start_parents`
`:353,:359`), and the file set says nothing inside the owner's own files
(`:18-20`). **Consumers consume the retained catalog through the doors, not
through `recover_or_create`.** Verified door pattern:
`root/artifact_facts.rs:23-89` names five `entry::` doors and no catalog
owner; `preservation_root/files.rs:13-63` the same. A conversion routed
through `artifact_facts` or through a new `entry.rs` door adds NO namer and
the pin stays at 1.

### 5.2 O13 / capability-free inventory rows: **NONE change today.**

`V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES`
(`check_checked_artifact_boundaries.py:369-373`) has exactly three rows —
`v1_lifecycle/archive.rs`, `v1_lifecycle/store/archive.rs`,
`v1_lifecycle/store/rewrite.rs` — and its token vocabulary is
`("rename_durable", "rename_noreplace", "sync_dir")` (`:374`).
`finalization/execute.rs` names **none** of those tokens, so it is not a row
and cannot move one. `V1_LIFECYCLE_PERMANENT_WRITER_EXCEPTIONS` (`:382-384`)
holds only the record root.

**But there is an ORDERING HAZARD the charter must state.** CapFree §3
generalises the map with a wider vocabulary that explicitly includes
`write_atomic` and `publish_workspace_exclude_candidate`, and the ReviewCode
[P2-4] door census lists `finalization/execute.rs` with "4 (convertible, not
carved)". If the pins package (E4.4-6-B) enters `finalization/execute.rs`
into the carved-file inventory, E4.5/6-B's conversion trips the SHRINKAGE
direction ("a conversion of a carved arm may not land without revising this
amendment"). **The charter must require the builder to verify, before
touching code, that `finalization/execute.rs` is absent from the landed
inventory's carved-file list — and to say so in the delivery.** CapFree §3's
exception text already agrees ("A v1 FORWARD arm that stays raw … is NOT
covered by this ground"), so the correct landed state is: not carved.

### 5.3 Shared files with carved arms: **NONE.**

`grep -rn "publish_workspace_exclude_candidate\|write_atomic"
src/workspace_ops/merge/v1_lifecycle/` (non-test) returns exactly four
lines, all in `finalization/execute.rs` (`:6` the `use`, `:45`, `:48`,
`:51`). The carved v0 siblings live in DIFFERENT files —
`merge/finalize.rs:245,248,251`, `handle_commit.rs:200,213,235,240,334`,
`abort/evidence.rs:683,689,696`, `abort/preflight.rs:409-410`,
`preserve/artifacts.rs:226,233` — so the per-ARM negative-scan problem
[P2-4] raised for `abort/evidence.rs` and `abort/preflight.rs` does NOT
arise here: `finalization/execute.rs` is single-arm. Its one `_checked`
occurrence is the git seam at `:31`, which the amendment's needle already
excludes.

### 5.4 Aggregate-gate partitions

`run_r4bg_aggregate_gates.py:426-454` — the fault battery's four disjoint
partitions:

1. `workspace_ops::merge::v1_lifecycle::` minus `root_fault_matrix` — **262
   passed** (`:428-430`). **New E4.5/6-B rows land here** (`tests/finalization.rs`,
   `tests/finalization_inputs.rs`).
2. `root_fault_matrix`, release profile — 1 passed (`:431-432`).
3. `checked_artifact::` (165-key census) — 457/467 (`:448-449`). Moves ONLY
   if a new `entry.rs` door is minted (the git-directory route).
4. lib remainder — 1110/1111 (`:450-453`). Moves if rows are added under
   `workspace_ops::merge::` outside `v1_lifecycle::` — which the abort-side
   `file_states` cure of §2.2 would require.

The charter must budget a re-measure of partition 1 always, partition 4 if
E4.6-B is taken, partition 3 only on the git-directory route. Both linux and
darwin counts carry the LINUX-COUNT-OWED note (`:433-435`).

---

## 6. THE ABORT / PLAIN-LEASE INTERACTION AFTER CONVERSION

**Does the reverse arm gain a new probe? NO — none, and this is provable at
three cites.** `abort/evidence.rs::file_states` ALREADY acquires a
`CheckedArtifact` for all three leaves on every reachable v1 abort, through
`artifact_facts::observe` at `:285` (boundary), `:300` (lock) and `:314`
(marker) — each `→ entry::observe_merge_root_artifact` (`entry.rs:36-41`)
`→ root_artifact` (`:159-166`) `→ CheckedArtifact::acquire` `→
durable_identity` (`observation.rs:96,105`). That is the probe, on the plain
lease, shipped with A1. The forward conversion adds **zero** new abort-side
acquisitions.

**Does it change what the reverse arm must reconcile? YES, in one respect
only: TIMING, not families.** The residue families for exactly these three
leaves already exist post-abort — the reverse arm mints them itself via
`write_checked` on boundary and lock (`abort/evidence.rs:148-159`) and
`remove_exact` on the marker (`:160-164`). After conversion the same
families can be present *before* the abort begins, produced by the forward
arm. Two consequences:

1. **Benign in the completed case.** `replace_exact` runs
   `finish_replace` before returning (`transition.rs:106`) and re-verifies
   `After` (`:107-115`), so a *completed* forward publication leaves no
   pending residue for the abort to trip on. `classify_proof_only`
   (`classification.rs:103-123`) is the one place a leftover family is read
   as `Ambiguous` outright, and it is only reached when expected == goal —
   not a shape any of the three arms produces.
2. **Not benign in the interrupted case, and directionally so** (§2.4): a
   forward-pair residue makes the abort's reverse-pair classify return
   `Ambiguous` (`classification.rs:175-177`), because
   `authority.matches_request` compares the caller's `(expected, goal)`
   against the residue's. **This is the abort-side face of the same §2 wall,
   not a separate one.** No new probe; a new *state* the existing probes
   cannot classify.

**New abort-side probes owed: NONE.** New abort-side OBSERVER work owed if
E4.6-B is taken: the `pending == None` arm of `file_states` must learn the
detached-recoverable state (§2.2 cure (ii)) — a change on the PLAIN lease
that RR §4 requires the charter to dispose of explicitly. It adds no
capability requirement (the acquisition is already there), so RR §4's clause
is satisfiable by the "reverse-arm reach is proven structurally already
probing" route rather than by a lease split — but it must be *written*, not
assumed.

---

## 7. RECOMMENDED CHARTER SHAPE

### 7.1 Recommendation: SPLIT into E4.5-B (build) and E4.6-B (report first)

CapFree §7 charters "ONE small step (E4.5/6-B)" for all three sites on the
premise that they share one reachability proof and differ only in door.
The reachability half of that premise is TRUE (§1). The *durability* half is
not: `:45` has no detach window and `:48`/`:51` do (§2), and the cure for
the latter is an observer change the ruling did not contemplate. One step
cannot carry both without either exceeding its cap or landing the wall
silently.

**E4.5-B — `:277`, the marker. BUILD.**
Scope: `execute.rs:45` → the checked boundary, expected fact `Missing`,
parent refused-not-bootstrapped (§4.1 route (a)). Vacuous for `:276` as
CapFree §7 says (no forward bundle writer exists — `forward.rs:71-76`
refuses `Preservation`, re-verified this session).
Route: reuse `artifact_facts` (`root/artifact_facts.rs:39-51` /
`:69-75`) or mint nothing; **no new `entry.rs` door**, so
`catalog_activation_pin.rs:45` and `ENTRY_REFERENCES` are untouched.
Cap: **~250 LOC** (≈30 production, the rest driven rows and doc sentences).
Driven rows: §4.4 items 1–3.
Partitions to re-measure: 1 (and 4 only if a `workspace_ops::merge::` row
is added).

**E4.6-B — `:278`/`:279`. STOP AND REPORT; do not open a build.**
The §2 wall is exactly the class RR §4's PLAIN-LEASE PROBE CLAUSE and RR
§1a's record-root duty exist to surface at charter time, and it was found
here before a line was written — which is the clause working. Three routed
options, for the lane owner / operator to choose:

* **(i) Convert with the observer cure.** v1-only changes in `live.rs` and
  `abort/evidence.rs::file_states`; `acceptance/publication.rs` untouched
  (v0-shared). Estimated **450–600 LOC** with the new fault rows — over a
  "small step" cap, and it touches the publication observation vocabulary,
  which no adopted record authorises.
* **(ii) Convert `:279` only, via the workspace seam, and leave `:278`
  raw.** `:279`'s leaf is regenerable and its loss is cosmetic, but its
  detach window produces the SAME `Ambiguous` → RecoveryRequired →
  abort-refuses outcome as the lock (§2.3). So this option does not avoid
  the wall; it only halves its surface. **Not recommended.**
* **(iii) Leave both raw as a DATED RESIDUAL.** CapFree §3's exception text
  already provides the landing place: "A v1 FORWARD arm that stays raw …
  is NOT covered by this ground — it already runs behind
  `acquire_activated` — and needs its own dated residual record in the pins
  package (Code axis [R2-P3-1])", and CapFree §7 already anticipates the
  failure mode ("should E4.5/6-B fail its own loop terminally, the arms fall
  to the [R2-P3-1] residual record and O1's text is re-annotated — not
  silently"). The residual's GROUND would be new and stronger than "not
  taken": *the boundary's detach-then-publish shape opens an
  observation-dead window on two leaves whose absence the shipped forward
  and reverse observers both refuse to classify; the raw `write_atomic`
  rename is atomic and opens none.* That is RR §2's record-root ground,
  generalised — and it is the honest one.

**The investigator's recommendation is (iii) for `:278`/`:279`, with
`:279`'s cell-2 discrepancy (§3.2) recorded at the same time**, and E4.5-B
built. If the operator wants the leaves converted anyway, (i) is the only
sound route and it is a step of its own, sized like E4.3-B, with the
observer change as its headline rather than a clause.

### 7.2 The reachability proof text to write INTO the charter

> **Reachability (RR §4, enumerated before the build, re-driven at main
> `0dae0d5`).** `finalization/execute.rs::publication` has one caller,
> `finalization.rs:53`. `FinalizationRuntime::new` has one production site,
> `forward.rs:69`, in `ForwardRuntime::execute`'s `Publication(_)` arm
> (`:68-70`); every other construction is under `tests/`.
> `ForwardRuntime::new` has two production sites, `start.rs:89` and
> `start.rs:160`, each immediately preceding `service::run` with
> `V1LifecycleRequest::ResumeStart` (`:90-97`) or `Continue` (`:161`).
> `service.rs:116-121` takes `V1MutationLease::acquire_activated` for
> exactly those two requests (`:118`) and the plain `acquire` for every
> other (`:120`). `ReverseRuntime::execute` refuses `Publication(_)`
> (`reverse.rs:78-82`) and `ForwardRuntime::execute` refuses every reverse
> and archive action (`forward.rs:71-76`); the only other production
> `service::run` calls are `start.rs:165` and `archive.rs:79`, both with a
> `ReverseRuntime` on the plain lease. `FinalizationRuntime` is a sealed
> `ProductionRuntime` (`service.rs:44`) but no production `service::run`
> passes one. **This arm is reached under `acquire_activated` and under
> nothing else. It touches no carved arm: `finalization/execute.rs` is the
> only file under `v1_lifecycle/` naming `write_atomic` or
> `publish_workspace_exclude_candidate`, and it holds no carved row
> (`check_checked_artifact_boundaries.py:369-373`).**

### 7.3 STOP-AND-REPORT triggers for whoever builds E4.5-B

1. **If `gwz.conf/markers` is found NOT to be resident in an ordinary
   pre-publication topology** — i.e. §4.1 route (a)'s refusal would fire on
   a normal first merge — STOP: route (b) or (c) is a different step and
   neither is chartered.
2. **If the landed pins package (E4.4-6-B) lists `finalization/execute.rs`
   in the carved-file inventory**, STOP: the conversion would trip the
   inventory's shrinkage direction and the amendment must be revised first
   (§5.2).
3. **If the marker arm's expected fact turns out not to be `Missing` on any
   reachable prefix** — i.e. if `resolve_candidate` can issue `WriteMarker`
   over a present marker — STOP: the arm has a detach window after all and
   joins `:278`/`:279`.
4. **If any interrupted-publication transcript shows `gwz merge --abort`
   refusing after a converted marker write**, STOP and report the transcript
   — that is the RR §1a signature and it must not be landed through.
5. For E4.6-B if it is ever chartered: **if the git-directory acquire is
   found unsupported for `<git-dir>/info/exclude`**, STOP — but note this
   session found the opposite (`preservation_root/files.rs:28-35` acquires
   exactly that target in production today), so the likelier trigger is the
   inverse: **if the workspace route is refused on cell-2 grounds, the
   `policy.rs:41-45` / R2-F residue-location question must go back to the
   lane owner before any door is minted.**

### 7.4 The O14 interim gate statement, owed at each seam

E4.2's review C1(iii), scope-corrected to E4.3–E4.6, requires each step to
state at its own execution seam which gate it opens and which it leaves
closed. The model sentence lives at
`coordinator/execution.rs:154-160`. E4.5-B's seam is the converted
`execute.rs` arm, and its sentence is:

> **Which gate this opens.** The DURABILITY gate only: the marker's
> publication is an exact checked replacement over a canonical, already-
> resident parent, proved by `parent_is_canonical` and by the boundary's own
> post-publication re-verification. The AUTHORITY gate stays closed —
> `AdmittedCheckedActionV1::authorize_write` / `RetainedWriteAuthorityV1`
> still have no production consumer, and this leaf is addressed by path,
> which ConsumerCheckpoint §9 `:264-266` names as NOT parent authority. Per
> E4.2's review flag-6 and CapFree §5, that conversion is DR-1's; **do not
> design an `authorize_write` consumer here.**

---

## 8. Falsification log — what this session checked against the authorities

| Authority claim | Verdict | Cite |
| --- | --- | --- |
| CapFree §1 last column: all three sites `acquire_activated`-only | **CONFIRMED** | §1.2 |
| CapFree §1 `:279` NOTE: `entry.rs` has only the git-directory OBSERVE door | **CONFIRMED** | `entry.rs:92-98,186-193`; grep → `:92,:186,:188,:336` |
| CapFree §7: `:279` "needing a git-directory replace door" | **FALSIFIED as a necessity** — the shipped v1 reverse arm writes that leaf through the WORKSPACE door | `abort/evidence.rs:148-153,334-341`; `root/artifact_facts.rs:39-51`; `entry.rs:159-166` |
| CapFree §7: "Three sites, one file, one reachability proof — ONE small step" | **HALF-FALSIFIED**: one reachability proof yes; one durability profile no | §2 |
| CapFree §3 `:277` "E4.5-B converts `execute.rs:45` and owes the ordering there" | **CONFIRMED and completed**: the parent is a real managed purpose that nothing bootstraps on this path | `managed.rs:92,119-128,190-196`; `handle_commit.rs:334` |
| CapFree §3 `:278` "exact source/goal and existing managed parent" | **PARTLY UNMEETABLE as worded**: `gwz.conf` is not a managed-parent purpose | `managed.rs:63-68` |
| CapFree §6: "the forward path wrote it raw (`execute.rs:45,48,51`)" | **CONFIRMED** | §1.1 |
| RR §1a: "the boundary's detach-then-publish shape cannot serve an artifact that is its own discovery root" | **CONFIRMED, and the failure mode GENERALISES beyond discovery roots** to leaves whose ABSENCE the observers cannot classify | §2.2, §2.3 |
| RR §1a: "MissingReplace never detaches" | **CONFIRMED** and is what makes `:277` clean | `transition.rs:91-92`; `classification.rs:148-154` |
| ReviewCode [P2-4] census: `finalization/execute.rs` 1 door namer, 4 raw writers, "convertible, not carved" | **CONFIRMED** | §5.3 |
| `policy.rs:41-45` "[P3-7] no production write lands under `<git-dir>/gwz/`" | **TRUE today**; would be falsified by a git-directory replace door | `policy.rs:41-45`; §3.3 |
| `policy.rs:42` cite "`entry.rs:182`" | **STALE** — the site is `entry.rs:188` | §1.3 |
| E4.4 prep §7.5: "`:279` … solely from `gwz repo create` and `init-from-sources`" | already corrected by ReviewCode [P1-1]; **re-confirmed wrong** | §1.2 |
