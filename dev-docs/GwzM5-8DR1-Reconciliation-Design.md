# GwzM5-8 DR-1 E8.3 — the observer/record RECONCILIATION class, and the rest of the round's agenda

Date: 2026-09-03. Author: the implementation lane (E8.3 investigator).
Status: **REVISION 2, 2026-09-03, folds the dual.** Both axes GO-WITH-CONDITIONS
(`GwzM5-8DR1-Reconciliation-ReviewCode.md`, `GwzM5-8DR1-Reconciliation-ReviewState.md`);
every condition is folded in place below. Base re-pointed to gwz-core main
**`d6830cd`** (see the verification notes: no mechanism-cited file differs from
`ffd4f95`, and both reviewers independently re-checked every cite at `d6830cd`). Home: R2-E phase E8 (DR-1's charter
`GwzM5-8DR1-Charter.md:6-10`; the operator's home answer still owed, E8.1 §6 Q1).
Base: gwz-core main **`ffd4f95`** (v0.13.0). Every code claim below is cited
`file:line` **at `ffd4f95`** and was read this session unless marked UNVERIFIED.
Companion, adopted: `GwzM5-8DR1-FilesystemIdentity-Design.md` (E8.1, REVISION 2)
— §4.4 rules that identity tiering does **not** change this class, and §7 item 4
orders this cure *before or with* (c) P3.

This document lands NO production code (charter §1). It proposes; the dual adopts.

---

## 0. The one-paragraph answer

The three windows the phase named separately — the **detach-then-publish absence
window**, the **discovery-dead window**, and the **directional-residue window** —
are one defect with one shape: *the checked boundary's private residue is a
complete, durable, content-sufficient description of an interrupted publication,
and not one shipped observer reads it.* Every observer instead asks the boundary
a **direction-bound** question (`inspect_family(expected, goal)` keys the
authority and staged-object filenames on `action_key`, which hashes `expected`
and `goal` — `authority.rs:204-222`, `residue.rs:114-117`), so a residue minted
by the *other* direction is not merely unmatched, it is classified **foreign**
(`residue.rs:179-181`, `:205-206`) and short-circuits to `Ambiguous`
(`classification.rs:141-143`) *before* the pairing check the record cites ever
runs. The one cure is therefore **a direction-free family survey plus an
observer-side reconciliation that consumes it**: teach the boundary to answer
"what interrupted action does this leaf's own family describe?"
(`family_key` is already direction-free — `authority.rs:196-202` hashes only root
identity and canonical path), widen the four root-artifact doors to an
`Option<&[u8]>` expected fact so a `Missing` expected is expressible (the shape
`replace_merge_preservation_workspace` already ships, `entry.rs:100-116`), and
make `abort/evidence.rs::file_states` (`:269-322`), the forward
`publication/live.rs::snapshot` (`:94-158`) and `root/abort.rs`'s transition
readers (`:123-145`) classify "absent-or-detached leaf + a family residue whose
`goal` names this leaf" as the **reconcilable** state — `Candidate`/`Before`
rather than `Other`/`Ambiguous`. That single change closes all three windows for
all leaves, converts three of the four residual-register rows, un-blocks the
record root's own re-examination, and is expressible as **six steps, each at or
under 450 LOC**, on the capability-free plain lease with **no new probe on any
listed operation**.

**REVISION 2's headline change — the cure has THREE components, not two.** Both
review axes found, independently, that REVISION 1 designed a survey and an
observer and stopped: the observer would report a reconcilable state and then the
executor (`execute_v1_evidence_rollback` → `remove_exact`/`write_checked`) would
re-run the *direction-bound* gate, see the counterpart residue as `foreign`, and
return a typed `Err` — moving the user from `Ok(RecoveryRequired)` to `Err`, not
to `Aborted`. §2.5 adds the third component, a **reconciler**: a pre-pass that
converges the leaf to an endpoint and **retires the counterpart family**, under
the caller's already-held lease. Two corrections make it smaller than either
reviewer assumed: the reconciled direction was **inverted** (for a never-published
leaf the truthful state is `After`, which `classify_table` already answers at
`classification.rs:253` and which short-circuits both writers with `Ok(())` and no
write at `transition.rs:44`/`:119`), so the **marker class needs no reconciler at
all**; and keeping the reconciler as a pre-pass **outside** `classify_table` costs
no new `ExactTransition` variant, no new table row, and no move of the 232-line
pin. The retirement half is load-bearing: without it a stale forward family stays
`foreign` to the *next* merge on a path-stable leaf, converting a one-merge wall
into a permanent one.

**Four corrections to the adopted record, measured this session** (details in
§1.4, §4.5, §4.7, §4.9): the directional-residue **mechanism cite is one layer
off** at five homes; the `gc_archived` family is **under-enumerated a second
time** (12 functions, not 11 — `merge/gc.rs:169-176` `cleanup_error` is missing
from the E4.7-corrected note); the preservation-bundle audit's hazard is **at the
preservation-ROOT leaves, not the bundle** (and the bundle carries a different,
un-named exposure); and the `CatalogOwnerV1` cite `catalog.rs:44-48` is **stale**
(the allow is at `:51-60`, the `use` at `:61-63`). The first is load-bearing:

**The mechanism cite.** The claim carried by `GwzM5-8R2E-E45B-Report.md:35`, `GwzM5-8R2E-E4-Close.md:44`,
`GwzM5-8DR1-Charter.md:117`, E8.1 §4.4 and the in-tree residual comment at
`finalization/execute.rs:71-78` — *"`classification.rs:175-177` sees an authority
record whose `matches_request` was minted for the FORWARD pair"* — names the
**wrong layer**. See §1.4. The observed behaviour is right; the site is one
layer earlier, and the cure lands in a different function because of it.

---

## 1. The leaf × window matrix, measured

### 1.1 The three windows, defined from the mechanism

| # | Window | Mechanism, at `ffd4f95` |
| --- | --- | --- |
| **(i)** | **Detach-then-publish absence** | `CheckedArtifact::replace_exact` detaches the existing leaf into the private area, durable-barriers, then publishes the goal. It detaches **only** when the retained source is `RetainedSource::Existing(_)`; on the `Missing`-source arm it goes straight to publish. `classify_exact`'s operation table (`classification.rs:148-153`) sets `MissingReplace` exactly when `expected == CheckedArtifactFact::Missing`. **So: expected `Bytes(_)` ⇒ window; expected `Missing` ⇒ none.** (Prep `GwzM5-8R2E-E45-6B-CharterPrep.md` §2.0, re-verified.) |
| **(ii)** | **Discovery-dead** | Window (i) on a leaf that is **its own discovery root** — nothing above it names it, so during the absence the lifecycle cannot even find the operation. `RR §1a`: driven at `probe/e4-3-detach-window-evidence` `c9a7303`, `tests/store.rs` row `an_interrupted_checked_rewrite_detaches_the_record_beyond_the_lifecycles_reach`. |
| **(iii)** | **Directional residue** | An interrupted publication leaves a family residue minted for the pair it was executing. Any later observer asking the **counterpart** pair enumerates the same family (the prefix is direction-free) but rejects every member by name, sets `foreign`, and gets `Ambiguous`. Independent of (i): it fires on `MissingReplace` arms too, which is why the marker — window-free under (i) — still walled E4.5-B. |

### 1.2 Where each window is created and where it is refused — the exact sites

**Creation (writer side).**

* `CheckedArtifact::replace_exact` — `transition.rs:38-116`; the fork at
  `:91-106` (`if matches!(authority.retained_source, RetainedSource::Existing(_))
  { self.detach_existing(…) } else { … }`), `publish_goal` at `:329-426` (the
  rename out of the private area at `:391-401`), `detach_existing` at `:179-327`
  (the rename **into** the private area at `:290-301`), `remove_exact` at
  `:118-177` (detach with no publish, `:165` — absence is the goal, so no
  window). **The absence window is asserted, not incidental**: the detach
  refuses unless the leaf is `Missing` afterwards (`:316-317`) and the publish
  refuses unless it is still `Missing` on entry (`:372`).
* The residue family's names: `family_key` = SHA-256 over
  `b"gwz.checked-artifact-family/v1\0"` + **root identity** +
  **canonical path identity** — `authority.rs:196-202`. **No `expected`, no
  `goal`.** `action_key` = SHA-256 over the family key + `expected`'s discriminant
  and digest + `goal`'s discriminant and digest — `authority.rs:204-222`.
  **Direction is entirely inside `action_key`.**
* File names: `authority_name(family, action)` = `ca1-{family}-{action}.authority`
  (`authority.rs:226-228`); `goal_name` = `ca1-{family}-{action}-{id}.goal`
  (`:234-236`); `source_name` = `ca1-{family}-{action}-{id}.source` (`:238-240`);
  `family_prefix(family)` = `ca1-{family}-` (`:230-232`).

**Refusal (observer side).**

* `inspect_family(expected, goal)` — `residue.rs:106-231`. Enumerates by
  `family_prefix` (`:116`, `:127`) — **direction-free** — then binds every
  member to `authority_name(&family, &action)` (`:117`, `:166`) and
  `goal_name`/`source_name` at that same `action` (`:186`, `:196`).
  A `.authority` under the family prefix with **any other** action key:
  **`foreign = true`** (`:179-181`). A `.goal`/`.source` under the family prefix
  with any other action key: falls to the `else` arm, **`foreign = true`**
  (`:205-206`).
* `classify_exact` — `classification.rs:125-199`. `if residue.foreign { return
  Ok(ExactTransition::Ambiguous); }` at **`:141-143`**.
* The pairing gate the record cites — `if residue.authority.is_some() &&
  !authority_current { return Ok(ExactTransition::Ambiguous); }` at
  **`:175-177`**, where `authority_current` is computed at `:155-174` and
  includes `authority.matches_request(self, expected, goal)`
  (`authority.rs:101-119`, which compares `family_key`, `action_key`,
  `canonical_path_identity`, `artifact_root_identity`, `expected`, `goal`,
  `operation`).
* `classify_proof_only` — `classification.rs:103-123`; `foreign || authority ||
  source || goal` ⇒ `Ambiguous` (`:109-115`).

### 1.3 The leaves the checked boundary publishes, or will

Doors, complete, at `checked_artifact/entry.rs`: `observe_merge_root_artifact`
(`:36-41`), `replace_merge_root_artifact` (`:43-51`),
`classify_replace_merge_root_artifact` (`:53-63`), `remove_merge_root_artifact`
(`:65-71`), `classify_remove_merge_root_artifact` (`:73-82`),
`observe_merge_preservation_workspace` (`:84-90`),
`observe_merge_preservation_git_directory` (`:92-98`),
`replace_merge_preservation_workspace` (`:100-107`),
`classify_merge_preservation_workspace` (`:109-116`),
`observe_merge_preservation_bundle` (`:118-126`),
`classify_merge_preservation_bundle` (`:128-137`),
`replace_merge_preservation_bundle` (`:139-148`),
`prepare_merge_store_parents` (`:150-157`), `activate_workspace_catalog`
(`:313-347`), `bootstrap_merge_start_parents` (`:349-373`),
`create_merge_store_record` (`:375-…`). Production namers of the *write* doors,
tests excluded: `dispatch.rs:456` and `v1_lifecycle/checked.rs:165`
(`activate_workspace_catalog`); `checked.rs:182`
(`bootstrap_merge_start_parents`); `v1_lifecycle/store/rewrite.rs:63`
(`create_merge_store_record`); `store/mod.rs:136`
(`prepare_merge_store_parents`); `root/artifact_facts.rs:45`, `:74`
(the root-artifact replace/remove pair); `preserve/checked_bundle.rs:93`
(the bundle); `git/gitbackend/preservation_root/files.rs:43` (the preservation
workspace leaf).

**The matrix.** *Window (i) column reads the expected fact the arm carries or
would carry; (iii) reads whether the SAME leaf carries checked writers on more
than one arm, which is what makes a residue directional at all.*

| # | Leaf | Path on disk | Discovery root? | Checked writer today | (i) detach | (ii) discovery-dead | (iii) directional | Status |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| L1 | **Merge record (record root)** | `.gwz/merge/<id>.yaml` | **YES** — `classify_open_record` / `discover_open` enumerate `.gwz/merge` only (RR §2) | create arm only: `create_merge_store_record`, `rewrite.rs:63`, expected `Missing` | create: **NO**; rewrite (if converted): **YES** | **YES** on the rewrite arm | create-only today ⇒ **no**; with a reverse arm ⇒ yes | rewrite CARVED (RR §2); create LANDED (E4.2) |
| L2 | **Marker** | `gwz.conf/markers/<id>.yaml` (`MARKER_DIR`, `artifact/mod.rs:32`) | On the merge path No — reached via `progress.candidate_marker_path` (`model/v0.rs:181`). **But markers ARE enumerated** for other purposes: `list_artifacts(root.join(MARKER_DIR), …)` at `artifact/mod.rs:464` | forward RAW (`execute.rs:79`); **reverse CHECKED** (`abort/evidence.rs:160-164`, `remove_exact`) | forward expected `Missing` ⇒ **NO** | No, on the merge path | **YES — the only leaf where it is DRIVEN** (E4.5-B) | forward RESIDUAL `:277` |
| L3 | **Lock** | `gwz.conf/gwz.lock.yml` | No for discovery; **yes for observation** (`live.rs:110-112` returns `None` unless the lock digests Regular) | forward RAW (`execute.rs:88`); **reverse CHECKED** (`abort/evidence.rs:154-159`, `write_checked`) | forward expected `Bytes` ⇒ **YES** | No | **YES** (would be, on conversion) | forward RESIDUAL `:278` |
| L4 | **Workspace boundary** | `.git/info/exclude` | No — regenerated from manifest+lock | forward RAW (`execute.rs:98`); **reverse CHECKED** (`abort/evidence.rs:148-153`) | forward expected `Bytes`, except the empty-baseline case ⇒ **YES** | No | **YES** (would be, on conversion) | forward RESIDUAL `:279` |
| L5 | **Root manifest** (`@root` selected abort) | `gwz.conf` (WORKSPACE_MANIFEST) | No | **CHECKED both ways already**: reader `root/abort.rs:123-130`, writer `root/abort.rs:380` (`write_checked`). **A SECOND, EARLIER observer gates it** *(added at Code [P2-2])*: `abort/preflight.rs:110-118` does `artifact_facts::observe` on `WORKSPACE_MANIFEST` and returns `MergeRecoveryRequired` unless the fact is `Bytes(_)` — it runs **before** `root/abort.rs` is reached, so it refuses a detached manifest first, and S3's `root/abort.rs` work alone cannot close L5 | `Bytes → Bytes` ⇒ **YES** | No | **YES — shipped** | shipped, unaudited |
| L6 | **Root lock** (`@root` selected abort) | `gwz.conf/gwz.lock.yml` — **the SAME leaf and the same `family_key` as L3** *(State [P2-2])* | as L3 | **CHECKED both ways already**: reader `root/abort.rs:137-144`, writer `root/abort.rs:380`; **plus the earlier `abort/preflight.rs:110-118` gate**, as L5 | **YES** | No | **YES — shipped** | shipped, unaudited |
| L7 | **Preservation workspace leaves** (managed marker in the worktree; workspace lock) | per `spec.managed_marker_path`; `artifact::LOCK_PATH` | No | **CHECKED both ways already** — one door, `files::replace_relative` (`preservation_root/files.rs:37-49`), called from `mutate_managed`'s `MarkerWorktree` (`preservation_root.rs:477-482`) and `LockWorktree` (`:483-488`) arms with `(source, goal)` taken from a `GitRootManagedTransition` whose direction the caller chooses | `Some(source)` ⇒ **YES**; first-write `None` ⇒ no | No | **YES — shipped, and structurally symmetric** | shipped; **this is flag 9's answer, §4.7** |
| L8 | **Preservation bundle** | `.gwz/stash/bundles/<stash_id>.yaml` (`STASH_BUNDLE_DIR`, `stash/mod.rs:13`; `bundle_relative`, `checked_bundle.rs:129-131`) | **Yes for `gwz stash`** — `list_bundles` enumerates the directory (`stash/mod.rs:266-289`); record-anchored on the merge path | **CHECKED ON EXACTLY ONE ARM — the v1 preserving/reverse arm**: `reverse/execute/preservation.rs:74` → `v1_write_bundle_checked` (`checked_bundle.rs:64-112`) → `replace_merge_preservation_bundle` (`entry.rs:139-148`). Expected = the bundle over `plans[..index]`, goal = over `plans[..=index]` (`:80-83`). **Every other writer is RAW and carved as `:276`**: the v0 `--abort --preserve` forward arm (`preserve/artifacts.rs:357` → `stash::write_bundle`, `stash/mod.rs:261-263`) and six `gwz stash` sites (`handle_stash/commands.rs:68,86,93,122,318,327`) | member 1: `None` ⇒ no; **members ≥ 2: `Bytes → Bytes` ⇒ YES** | No | **NO — one checked arm.** But see §4.7: the raw siblings on the same leaf neither observe nor classify the residue | shipped; detach window OPEN and undocumented |
| L9 | **Catalog's own records** (bootstrap record, scratch) | `.gwz/catalog-final/…` | The bootstrap record is the catalog's root | `activate_workspace_catalog` (`entry.rs:313`), `bootstrap_merge_start_parents` (`:349`) | activation is create-shaped; parents are `prepare_parent` | n/a — guarded by its own `matches_attempt` reconciler (`catalog_bootstrap_record.rs:172-185`) | n/a | LANDED (E4.1); **already has a reconciler** — see §2.5 |
| L10 | **Git-directory boundary** (`<git-dir>/info/exclude` via the preservation door) | `<git-dir>/info/exclude` | No | **observe-only** — `observe_merge_preservation_git_directory` (`entry.rs:92-98`) from `files::observe_boundary` (`:28-35`); `policy.rs:41-45` records that **no production write lands under `<git-dir>/gwz/` through this policy**. **Re-measured and CONFIRMED**: `CheckedArtifactPolicy::git_directory` has exactly one production construction site, `entry.rs:188` inside `preservation_git_directory` (`:186-193`) — *the comment's own cite `entry.rs:182` is stale*, `:182` is now a line of the workspace sibling `preservation_workspace` (`:177-184`) | n/a | n/a | n/a | read-only; relevant only to row `:279`'s cell-2 wording (§4.6) |
| L11 | **The selected-root exact-file set** (a second forward observer of the same class) | per `exact_files` | No | **observe-only, and it has the same defect**: `regular_file_equals` (`authority/observe/finalization.rs:439-458`) maps `NotFound → Ok(false)` at `:442`, so an absence collapses into "not exact" ⇒ root drift | n/a | n/a | inherits (iii) through the residue it cannot see | **newly named here**; folded into S3 |

**Reading the matrix.** Window (i) is a property of *one arm*. Window (iii) is a
property of *a leaf with two arms*. Window (ii) is (i) on a leaf with no outer
namer. **Nine of the ten leaves have at least one window; L9 alone is closed, and
it is closed by the only reconciler the tree owns** (§2.5). Three leaves —
**L5, L6, L7** — carry window (iii) **on shipped code**, not prospectively.

### 1.4 The correction: the directional refusal fires at `classification.rs:141-143`, not `:175-177`

The adopted record says the abort's reverse-pair `classify_remove` returns
`Ambiguous` because *"`classification.rs:175-177` sees an authority record whose
`matches_request` was minted for the FORWARD pair"*
(`GwzM5-8R2E-E45B-Report.md:35`; repeated at `GwzM5-8R2E-E4-Close.md:44`,
`GwzM5-8DR1-Charter.md:117`, E8.1 §4.4, and in-tree at
`finalization/execute.rs:71-78`). Traced at `ffd4f95`:

1. The abort's marker arm calls `artifact_facts::classify_remove(root, marker,
   candidate.marker_yaml)` (`abort/evidence.rs:307-311`) — the **reverse** pair
   `expected = Bytes(marker_yaml)`, `goal = None`.
2. `classify_remove` → `entry::classify_remove_merge_root_artifact`
   (`artifact_facts.rs:83-86` → `entry.rs:73-82`) → `classify_remove` →
   `classify_exact(&Bytes(marker_yaml), None)`.
3. `classify_exact` computes `action_key(Bytes(marker_yaml), None)` — the
   **reverse** action key — and calls `inspect_family` (`classification.rs:140`).
4. `inspect_family` enumerates by `family_prefix` (`residue.rs:127`), which
   **does** match the on-disk forward residue (same `family_key`: same root
   identity, same canonical path). It then tests each name against
   `authority_name(&family, &action_reverse)` (`:166`). The on-disk
   `ca1-{family}-{action_forward}.authority` fails that test and hits
   `if text.ends_with(".authority") { foreign = true; }` at **`:179-181`**. The
   on-disk `ca1-{family}-{action_forward}-{id}.goal` matches neither
   `goal_name(&family, &action_reverse, …)` (`:186`) nor `source_name(…)`
   (`:196`) and hits the `else { foreign = true }` at **`:205-206`**.
5. `inspect_family` therefore returns `FamilyResidue { authority: None, source:
   None, goal: None, foreign: true }` (`residue.rs:26-31`, `:225-230`).
6. `classify_exact` returns `Ambiguous` at **`:141-143`** — *before* the
   `authority_current` computation at `:155-174` and *before* the `:175-177`
   branch, both of which are unreachable in this scenario because
   `residue.authority` is `None`.

`:175-177` is reachable, but for a **different** class: a residue whose file
names *do* match (same family **and** same action, i.e. same direction) but whose
`retained_parent_identity` or `artifact_root_identity` has moved — E8.1's C2/C3
tier class (`GwzM5-8DR1-FilesystemIdentity-Design.md` §2). The two classes need
different cures and the record currently conflates them.

**Consequence for the design.** A cure that widened only `matches_request` or only
the `:175-177` branch would **not** fix the driven wall: the residue is discarded
one frame earlier, inside `inspect_family`, and never reaches the pairing logic.
The cure must reach `residue.rs`'s **naming and enumeration**, which is why §2's
shape is a *survey*, not a widened predicate.

**Independent confirmation.** A second, peer-blind measurement pass over the same
tree reached the identical conclusion without being told it: *"the forward
authority file ends with `.authority` but is not `expected_authority_name`, so it
takes `residue.rs:179-182`, and forward `.goal`/`.source` files fail both name
comparisons at `:188`/`:198` and take `:204-206`. Either way `foreign = true` ⇒
`classify_exact` returns `Ambiguous` at `classification.rs:141-143`, **before**
even reaching `:175-177`."* Two independent readings, one answer.

**A stronger statement of the same fact, worth putting in the record.** A leaf's
forward pair `(A → B, Replace)`, its reverse pair `(B → A, Replace)`, and the
abort's removal `(B → ∅, Remove)` are **three mutually invisible identities over
one leaf**, because `action_key` hashes `expected` and `goal`
(`authority.rs:204-223`) and therefore renames every file in the family. Four
independent comparisons in `matches_request` (`action_key`, `expected`, `goal`,
`operation`) each force `false` on their own. The boundary's recovery model is
per-`(leaf, expected, goal, operation)` and **strictly unidirectional** — that is
the defect, stated at its root.

**And the gate is hiding a correct answer, not merely an unmatched one
(Code [P2-1]).** If the `:141-143` `foreign` gate were bypassed, this scenario
falls through to `classify_table(ExistingRemove, authority = false, Absent,
Absent, Missing)` = **`After`** (`classification.rs:247-253`) — *the reverse
operation is already complete*, which for a never-published marker is the
truthful answer. So the classifier is not missing a rule; it is being prevented
from reaching one it already has. That fact fixes the direction of the whole
cure (§2.4(A), §2.5) and is why the marker class needs no reconciler at all.

**Status of this correction: measured by reading, twice, at `ffd4f95`; NOT
re-driven** (the round is read-only and the RED row is out of tree). Step S1 of
§7 drives it as its first act, and the correction is text at four record homes
plus one in-tree comment (§5.3).

---

## 2. The one cure — a direction-free survey, an observer that reads it, and a reconciler that converges

### 2.1 Shape, in one sentence

Add a **direction-free family survey** to the checked boundary
(`residue.rs` + one `entry.rs` observe-shaped door), widen the four
root-artifact doors so a `Missing` expected fact is expressible, and make the
three observer families classify *"leaf absent-or-unexpected **and** the survey
reports exactly one interrupted action of this leaf's own family whose `goal`
(or `expected`) names the bytes I am asking about"* as the **reconcilable**
state rather than `Ambiguous`/`Other`.

This is charter-prep §2.2 cure (ii) / §7.1 option (i) — *"`abort/evidence.rs::
file_states` and the forward observers learn to read the residue"* — with the
site corrected by §1.4 and one primitive (the survey) added underneath, because
without it the observers have nothing to read: `inspect_family` is the only
residue reader and it refuses to report across a direction change.

### 2.2 The survey primitive (the new thing, and the only new thing)

```
// checked_artifact/residue.rs — sketch, not code to land
pub(super) struct FamilySurvey {
    pub(super) actions: Vec<SurveyedAction>,  // 0 or 1 in every reachable state
    pub(super) unreadable: bool,              // budget blown, or a name we cannot parse
}
pub(super) struct SurveyedAction {
    pub(super) authority: CheckedArtifactAuthority, // carries expected, goal, operation,
                                                    // retained_source, both identities
    pub(super) source_present: bool,
    pub(super) goal_present: bool,
}
pub(super) fn survey_family(&self) -> ModelResult<FamilySurvey>
```

`survey_family` enumerates by `family_prefix(&self.family_key())` exactly as
`inspect_family` does at `residue.rs:116,:127` — **the same prefix, so the same
ownership boundary** — but decodes each `.authority` it finds instead of
demanding one particular `action_key`, and reports the `.source`/`.goal`
companions by matching the decoded authority's own action key rather than the
caller's. It **never writes, never adopts, never repairs**; it answers one
question and returns.

Everything the survey needs is already on disk and already decodable:
`CheckedArtifactAuthority` records `family_key`, `action_key`, `operation`,
`canonical_path_identity`, `artifact_root_identity`, `retained_parent_identity`,
`expected`, `goal`, `retained_source` (`authority.rs:116-144`, `encode`; the
`decode` counterpart is what `residue.rs:175` already calls).

**Why this is the minimal addition.** `inspect_family` cannot be widened in
place: it is the *transition* reader, and its direction-binding is load-bearing —
`replace_exact` must refuse to publish over a residue from a different action.
Widening it would weaken the writer. The survey is a **separate, read-only**
question, which is why it can be answered more liberally without touching the
writer's contract at all.

### 2.3 The classifier widening

Four root-artifact doors hard-code `CheckedArtifactFact::Bytes(expected.to_vec())`
and so cannot express a `Missing` expected fact: `replace_merge_root_artifact`
(`entry.rs:43-51`), `classify_replace_merge_root_artifact` (`:53-63`),
`remove_merge_root_artifact` (`:65-71`), `classify_remove_merge_root_artifact`
(`:73-82`). Their `artifact_facts` wrappers inherit the limit —
`write_checked(… expected: &[u8] …)` (`artifact_facts.rs:38-50`),
`classify_write` (`:52-66`), `remove_exact` (`:68-73`), `classify_remove`
(`:75-86`). This is E4.5-B flag 7 and the charter's "classifier widening" item.

**The shape is already in the tree and needs no invention**: the preservation
doors take `expected: Option<&[u8]>` and map `None → Missing`
(`replace_merge_preservation_workspace` `entry.rs:100-107`,
`classify_merge_preservation_workspace` `:109-116`,
`observe_merge_preservation_bundle` `:118-126`,
`classify_merge_preservation_bundle` `:128-137`,
`replace_merge_preservation_bundle` `:139-148`). The E4.5-B demonstration already
proved this exact widening compiles and passes three of its four rows
(`GwzM5-8R2E-E45B-Report.md:63`: *"widened that existing door's `expected` to
`Option<&[u8]>` … no new door, no new `ENTRY_ITEMS` row"*).

So the widening is: `&[u8] → Option<&[u8]>` on those four doors and their four
wrappers; `RegularFileFact` is unchanged (it already has `Missing`,
`artifact_facts.rs:8-13`). **No new `entry.rs` door ⇒ `ENTRY_REFERENCES` /
`ENTRY_ITEMS` unmoved, `catalog_activation_pin.rs`'s `PRODUCTION_CALLER_COUNT`
unmoved.** **Nine** external call sites move with the signatures *(corrected from
13 at Code [P3-1])*: `abort/evidence.rs:148,:154,:160,:277,:292,:307` and
`root/abort.rs:123,:137,:380`. The other eight —
`abort/evidence.rs:82,:83,:285,:300,:314`, `root/abort.rs:131,:145` and
`abort/preflight.rs:111` — are `artifact_facts::observe` calls with **no
`expected` parameter** and are untouched by the widening. They are, however,
exactly the sites §2.5's reconciler must reach, because an `observe` has no
residue channel at all (State [P2-4](i)).

**One constraint the widening inherits, measured — it is not free.**
`classify_remove_exact` rejects a non-`Bytes` expected outright
(`classification.rs:93-99`, *"checked removal requires exact existing source
bytes"*). So widening the signature is necessary but not sufficient for the
remove arm: the `ExistingRemove` rows of `classify_table`
(`classification.rs:235-277`) must be extended to name the `Missing`-expected
removal, or the remove arm must be routed through the reconcilable state instead
of through a widened `expected`. **S2b is chartered to take the second route** —
it keeps `classify_remove_exact`'s refusal intact and lets S2's survey answer the
question, which is the smaller and safer change. `classify_table` carries a
232-line pinning test (`classification.rs:302-533`) that moves with any row
edit; avoiding that edit is worth the routing.

### 2.4 The transitions table — before and after, per window per leaf

`RFT` = `artifact_facts::RegularFileTransition`. `FS` = `abort::evidence::FileState`.
`PR` = `publication/live.rs::PublicationResolution`.

**(A) Window (iii), the directional residue — abort side, the driven row.**
*Every row names which of the three components (survey / observer / reconciler)
produces the outcome, and what the USER sees* (State [P2-4](iv)).

| Leaf & arm | Today → user | After the cure → user | By which component |
| --- | --- | --- | --- |
| L2 marker, abort **non-pending** (`evidence.rs:313-318`) | `observe` ⇒ `Missing`; `classify_file(…, missing_is_baseline=true)` ⇒ `FS::Baseline` (`:356`) — already tolerant → **`Ok(Aborted)`** on this cell | unchanged — **the non-pending arm was never the wall** | — |
| L2 marker, abort **pending == Marker** (`evidence.rs:307-311`) | `classify_remove` ⇒ `inspect_family` ⇒ `foreign` (`residue.rs:179-181`, `:205-206`) ⇒ `Ambiguous` (`classification.rs:141-143`) ⇒ `transition_file` ⇒ **`FS::Other`** ⇒ shape not exact ⇒ `preflight_v1_evidence` `Err` → user sees **`Ok(RecoveryRequired)`**, then **`Err(RecoveryEvidenceMismatch)`** on the second `--abort` | survey reports one action `(Missing → marker_yaml, Replace)`; the abort asks `(Bytes(marker_yaml) → Missing, Remove)` — **the exact counterpart**. The marker was **never published**, so the reverse operation's *post* state already holds ⇒ **`RFT::After` ⇒ `FS::Baseline`** ⇒ shape exact. `execute_v1_evidence_rollback` then calls `remove_exact`, which classifies `After` and **returns `Ok(())` without writing** (`transition.rs:119-120`) → **`Ok(Aborted)`** | **survey + observer.** No reconciler needed — no detach happened |
| L3 lock, L4 boundary (after S4), abort **pending == Lock/Boundary** (`:277-282`, `:292-297`) | same chain ⇒ `Ambiguous` ⇒ **`FS::Other`** ⇒ `Err` → **`Ok(RecoveryRequired)`** then **`Err`** | **the leaf is genuinely mid-flight**: absent, with the baseline bytes alive only as the detached `.source`. Neither `Before` nor `After` is true. The **reconciler** (§2.5) restores `.source` onto the leaf and retires the counterpart family; the observer then re-classifies on a clean slate and gets the ordinary `FS::Candidate` ⇒ shape exact ⇒ **`Ok(Aborted)`** | **survey + reconciler + observer** |
| L3, L4, abort **pending == None** (`:284-289`, `:299-304`) | `evidence_shape_is_exact` runs `file_states(root, record, None)` **first** (`:390-393`), so this arm is reached before any pending arm; `observe` ⇒ `Missing` ⇒ `classify_file(…, false)` ⇒ **`Other`** ⇒ `preflight_v1_evidence` `Err` (`:34-55`) → **`Ok(RecoveryRequired)`** | **`artifact_facts::observe` returns `RegularFileFact` (`Missing｜Bytes｜Invalid`, `artifact_facts.rs:8-13`) — it has no residue channel, and I9 forbids a new door.** Resolution: the None arms do **not** grow a survey call. Instead `file_states` calls the **reconciler once, up front**, before computing any arm (§2.5); after it returns, the leaf is at an endpoint and the existing `classify_file` is correct unchanged → **`Ok(Aborted)`** | **reconciler (pre-pass), then today's observer verbatim** |
| L5 root manifest, L6 root lock (`root/abort.rs:123-130`, `:137-144`) | `Ambiguous` ⇒ `transition_state` ⇒ refusal → **`Err`** | reconciler pre-pass in the same place, then the existing `Before` handling → **`Ok`** | **reconciler + observer** |
| **`@root` preflight (`abort/preflight.rs:110-118`)** *(added at Code [P2-2])* | `artifact_facts::observe` on `WORKSPACE_MANIFEST` and `LOCK_PATH`; **anything but `Bytes(_)` ⇒ `MergeRecoveryRequired`** — and this runs **before** `root/abort.rs` is ever reached, so a detached L5/L6 is refused here first → **`Ok(RecoveryRequired)`** | the reconciler pre-pass runs here too, as `preflight`'s first act; the existing `Bytes(_)` gate then passes unchanged → **`Ok`** | **reconciler; observer unchanged** |
| L7 preservation-root leaves (`preservation_root.rs:207`) | `observe_transition` ⇒ `Ambiguous` ⇒ refusal `:209-210` → **`Err`** | reconciler pre-pass before `observe_transition`; existing `after`/`pattern_matches` logic then runs → **`Ok`** | **reconciler + observer** |
| **L11 exact-file set (`observe/finalization.rs:439-458`)** *(row added at State [P2-4](iii))* | `regular_file_equals` maps `NotFound → Ok(false)` at `:442` ⇒ reported as root drift → **`Err`/drift** | L11 is **read-only and downstream of the reconciler**: once the pre-pass has converged the leaf, `NotFound` no longer occurs on a reconcilable leaf. **No change to `regular_file_equals` itself** — S3 only has to guarantee the pre-pass runs before it | **reconciler; L11 untouched** |
| **L3/L4, the NEXT merge after a reconciled abort** *(row added at Code [P1-1](c) / State [P1-1](3))* | n/a today — the raw forward writer leaves no residue | **without retirement this is a regression**: a stale forward family on a path-stable leaf is `foreign` to merge #2's `replace_exact` (`residue.rs:179-181`) ⇒ permanent refusal at `transition.rs:44-49`. **The reconciler therefore retires the counterpart family as part of its own act, before it returns** — so merge #2 enumerates nothing → **`Ok`** | **reconciler's retirement half — the load-bearing half** |

**(B) Window (i), the detach absence — forward side.**

| Leaf | Situation | Today | After the cure |
| --- | --- | --- | --- |
| L3 lock (after conversion) | crash inside the detach window; lock **absent** | `regular_digest` ⇒ `FileDigest::Missing`; the `let (FileDigest::Regular(lock), …) else { return Ok(None) }` at `live.rs:110-112` ⇒ `snapshot` `None` ⇒ `resolve_candidate(_, None)` ⇒ `_ => PR::Ambiguous` (`live.rs:90`) ⇒ `ambiguity(current)` ⇒ **RECOVERY origin** | before the `else`-return, `snapshot` surveys the lock's family. One action, `expected = baseline_lock`, `goal = candidate_lock`, `source_present` (the detached original), `goal_present` or not. **⇒ normalise the absent lock to its `expected` digest** and continue: prefix `Marker`/`Baseline` as the other two leaves dictate ⇒ `resolve_candidate` re-issues `WriteLock` ⇒ `replace_exact`'s own `RecoverableDetached` convergence runs ⇒ **publication completes** |
| L4 boundary (after conversion) | crash inside the detach window; boundary **absent**, baseline non-empty | `live.rs:118-122` maps `Missing → None`; `classify_candidate_parts` (`acceptance/publication.rs:81-100`) accepts a `None` boundary as baseline **only when `candidate.baseline_boundary_text.is_empty()`** (`:95`) ⇒ no prefix ⇒ `None` ⇒ `PR::Ambiguous` ⇒ recovery | same normalisation in `snapshot`, **before** `CandidatePublicationObservation::new` at `live.rs:123` ⇒ prefix resolves ⇒ `WriteBoundary` re-issued |
| L2 marker | n/a — expected `Missing`, no detach | — | unchanged |
| L8 bundle, members ≥ 2 | crash inside the detach window; bundle **absent**; the SAME v1 preserving arm retries | `observe_merge_preservation_bundle` ⇒ `false`; `classify_merge_preservation_bundle` ⇒ `inspect_family` **matches** (same direction) ⇒ `Recoverable` ⇒ `checked_bundle.rs:85` treats it as "not yet after" and re-issues `replace_exact`. **This one already converges** | unchanged — L8 is the proof that (i) alone is survivable **when the direction does not change**. It is (iii) that kills, not (i). |
| L8 bundle | crash inside the detach window, then a **raw** sibling writer runs (`stash::write_bundle`, or any `gwz stash` command) | the raw writer neither observes nor classifies the residue; it `write_atomic`s over the leaf, leaving an orphaned staged goal, quarantined source and authority record in `.gwz/checked-artifacts` | **unchanged by S1-S4 — and deliberately so.** The raw siblings are carved (`:276`) and may not be put on a probe. §4.7 records this as a *residue-orphaning* exposure, distinct from the reconciliation class, and routes it to a cleanup sweep rather than a conversion |
| L5/L6/L7 | crash inside the detach window, **same** direction retried | as L8 — `inspect_family` matches, `Recoverable` | unchanged |

> **The sharpest result in this table.** L8 shows window (i) is *already*
> reconciled by the boundary itself whenever the retry asks the **same**
> question. Windows (i) and (ii) are only lethal because the *observer* refuses
> to reach the retry — `live.rs:110-112` bails before `classify_*` is ever
> called, and `abort/evidence.rs`'s non-pending arms use raw `observe` rather
> than `classify_*`. So the whole class reduces to: **the observers must ask the
> boundary the classification question instead of the raw-digest question, and
> the boundary must be able to answer it across a direction change.**

**(C) Window (ii), discovery-dead — the record root.**

| Situation | Today | After the cure |
| --- | --- | --- |
| L1 rewrite interrupted mid-detach (`c9a7303` row) | `.gwz/merge` empty; `load_open` ⇒ `Err(IoError NotFound)`; `classify_open_record` (fn at **`store/mod.rs:216`**, *cite corrected at Code [P3-5]*) enumerates `.gwz/merge/*.yaml` ⇒ **no open merge**; `--abort` cannot find the id; residue durable, invisible, git-status-exempt (`policy.rs:33-45`) | **still no open merge** — because `classify_open_record` does not survey. **The cure does not close (ii) by itself.** It supplies the *primitive* (§2.5) that makes closing it a ~200-LOC follow-on: `classify_open_record` surveys `.gwz/checked-artifacts` for a family whose `canonical_path_identity` resolves under `.gwz/merge`, and reports the record as OPEN-DETACHED with the id read from the surveyed `expected` bytes. **Deliberately a separate step (S5) and a separate decision (§3.3).** |

### 2.5 THE THIRD COMPONENT — the reconciler (added at [P1-1], both axes)

**The defect both reviewers found.** REVISION 1 designed a survey and an observer
and stopped. On the abort side that leaves the ledger's `Aborted` cells with no
execution path: the observer reports a reconcilable state, then
`execute_v1_evidence_rollback` (`abort/evidence.rs:131-172`) calls
`remove_exact` (`:160-164`) or `write_checked` (`:148-159`), whose OWN
`classify_*` gate re-runs the **direction-bound** `inspect_family` that §2.2
deliberately leaves un-widened, sees the counterpart residue as `foreign`, and
returns `Err("removal evidence is ambiguous")` (`transition.rs:121-126`) or
`Err("replacement evidence is ambiguous")` (`transition.rs:44-49`). **The user
would move from `Ok(RecoveryRequired)` to a typed `Err` — not to `Aborted`.**
Both axes reached this independently; it is correct, and it is the single most
important change in this revision.

**Two facts make the fix smaller than either reviewer assumed.**

1. **The direction was inverted, and correcting it dissolves the marker case
   entirely.** For the marker the reverse operation is `Bytes(marker_yaml) →
   Missing` (`ExistingRemove`); an **absent** leaf is that operation's *post*
   state, not its pre state. The truthful reconciliation is **`After`**, not
   `Before`. And `classify_table` already says so: with the `foreign` gate
   bypassed, `(ExistingRemove, authority=false, Absent, Absent, Missing)` yields
   **`After`** at `classification.rs:253` — the row exists today (Code [P2-1],
   recorded in §1.4). `After` then short-circuits **both** writers with
   `Ok(())` and **no write at all**: `replace_exact` at `transition.rs:44`
   (`ProofOnly | After => return Ok(())`) and `remove_exact` at `:119`
   (`After => return Ok(())`). **So for every leaf whose forward arm is
   `MissingReplace` — the marker, and the merge record's create arm — the
   direction correction alone closes the loop. No reconciler, no writer arm, no
   new variant, no `classify_table` edit.**
2. **The reconciler is needed only for leaves that actually detached.** For
   `ExistingReplace`/`ExistingRemove` forward arms (L3, L4, L5, L6, L7) the leaf
   is absent *and* the baseline bytes exist only as the detached `.source` in the
   private area. That state is neither endpoint, and `classify_table` has no row
   for it (`(ExistingReplace, false, Absent, Absent, Missing)` falls to
   `_ => Ambiguous`, `:277`) — correctly, because it is genuinely mid-flight.
   Something must physically move bytes.

**The component: `reconcile_counterpart`, a pre-pass, not a classification.**

```
// checked_artifact/residue.rs or a sibling — sketch, not code to land
impl CheckedArtifact {
    /// Converge this leaf to an endpoint if, and only if, its family holds
    /// exactly one interrupted action that is the EXACT COUNTERPART of the
    /// caller's (expected, goal). Returns what it did; Ok(NothingToDo) is the
    /// overwhelmingly common case. Never partial: converge AND retire, or
    /// neither.
    pub(super) fn reconcile_counterpart(
        &self, expected: &CheckedArtifactFact, goal: Option<&[u8]>,
    ) -> ModelResult<Reconciled>;
}
```

It runs **as the first act of the observer's caller**, under the lease that
caller already holds, on the `CheckedArtifact` it has already acquired (I6 — no
new probe). It does three things and then gets out of the way:

1. **Survey** (§2.2) — direction-free, read-only.
2. **Converge** — restore the detached `.source` back onto the managed leaf
   (`transition.rs:290-301`'s rename, inverted), or, if the `.goal` was already
   published, leave the leaf as it is. This is the one write, and it is a rename
   within a directory the caller already owns.
3. **Retire** — clear the counterpart family with the existing `cleanup.rs`
   primitives (`:87`/`:89` staged goal, `:132`/`:134` quarantined source,
   `:222`/`:224` authority record). **This half is load-bearing**: without it a
   stale forward family stays `foreign` to every later action on a path-stable
   family, so merge #2's `replace_exact` on the lock refuses permanently — the
   design would have converted a one-merge wall into a permanent one (Code
   [P1-1](c), State [P1-1](3)). `cleanup.rs` today retires only the writer's
   **own** family (`:70`, `:117`, `:202`); the reconciler is the first
   foreign-family retirement in the tree, which is exactly why it must be a
   named, reviewed action rather than a side effect.

**Retirement timing, stated (the coordinator's question).** Retirement happens
**inside `reconcile_counterpart`, before it returns** — not deferred to a sweep.
So the next merge's publication never enumerates a counterpart family, whether
or not an abort ran in between: if the user re-runs the merge instead of
aborting, the retry is **same-direction**, `inspect_family` matches, and the
boundary's own `RecoverableDetached` convergence handles it without the
reconciler at all (the L8 precedent, §2.4(B)). The reconciler is only ever
reached by a **counterpart** caller.

**Why a pre-pass and NOT a new `ExactTransition` variant.** To express the
counterpart state *inside* `classify_table` you would have to make
`inspect_family` direction-free — which §2.2 shows would weaken the writer's own
refusal, the very property that makes `replace_exact` safe. Keeping the
counterpart handling **outside** the classification table means:
`ExactTransition` gains **no variant**; `classify_table`
(`classification.rs:235-277`) gains **no row**; and its 232-line pinning test
(`classification.rs:302-533`) **does not move**. After the pre-pass, the leaf is
at an endpoint and every existing classifier is correct verbatim. **This is the
answer to the coordinator's "say which and why": a named retirement action
(option (b) of Code [P1-1]), not a writer-side counterpart arm (option (a)).**

**The residual risk, stated.** If a build discovers the pre-pass cannot converge
without expressing an intermediate state to the classifier, the fallback is
option (a) — a new `ExactTransition` variant, new `classify_table` rows, and a
re-pin of the 232-line test. **That is S2b's stop-trigger** (§7), and the sizes
below carry it as a named contingency rather than a base assumption.

**Who restores the detached `.source`, per leaf** (the coordinator's question):
L3 lock, L4 boundary, L5 root manifest, L6 root lock — the reconciler invoked
from `abort/evidence.rs::file_states`, `abort/preflight.rs:110-118` and
`root/abort.rs`, each under the abort's plain lease. L7 — the reconciler invoked
from `preservation_root.rs:207`, under the preservation transition's lease.
L2 marker and L1's create arm — **nobody**, because nothing detached. L8 — nobody
(same-direction retry, already converges).

---

### 2.5a The precedent: the tree already owns exactly this reconciler, for the catalog

`protocol/catalog_bootstrap_record.rs:172-185` `matches_attempt`, with
`catalog/scratch.rs:19-46` and `catalog/classifier.rs:96-104`, `:301-319`
(cites via E8.1 §2 C10, re-verified in E8.1's verification notes at `ffd4f95`),
is a **record-side reconciler over durable residue**: the catalog's bootstrap
record is matched against an *attempt*, and the scratch object is found by a name
that encodes the attempt, so an interrupted activation is reconciled rather than
refused. That is L9's row being closed in the matrix. The cure proposed here is
that same architecture, applied one layer down, to the artifact families. **It is
not a new idea in this codebase; it is the idea the catalog already shipped, not
yet given to the leaves.**

### 2.6 Invariants — what must hold, and how each is enforced

| # | Invariant | Enforcement |
| --- | --- | --- |
| **I1** | **No silent adoption of foreign residue — stated as an exclusion PROOF, because the token's own boundary demands one.** *(rewritten at State [P2-2], Code [P3-2])* | `bootstrap/managed/provider.rs:58-61` says: *"A later step that uses a resident record to decide adoption of state this action did not create makes determinism load-bearing for exclusion, and must re-litigate the token then."* The reconciler **is** such a step, so "untouched" was the wrong answer. The proof, in three parts: **(a) enumeration bound, enforced at the SURVEY** — only `family_prefix(&self.family_key())` (`residue.rs:116`, `:127`), and `family_key` is SHA-256 over the root durable identity + the canonical path identity (`authority.rs:196-202`), so another workspace, root or path is never enumerated. **(b) The counterpart proof, enforced at the CLASSIFIER, not the survey** — byte-swapped `(expected, goal)` plus the C2/C3 identity comparisons of `matches_request` (`authority.rs:107-110`), retained unchanged (I3). This is the only thing that separates two co-resident writers of one family. **(c) The same-user boundary is out of scope** per the token's own E16 clause (`provider.rs:58-59`). **Two facts this makes explicit: L3 and L6 are ONE leaf** — `gwz.conf/gwz.lock.yml` / `artifact::LOCK_PATH` — so one `family_key` is written by two different observer/writer pairs (merge publication/abort, and selected-root rollback), and **(b) alone separates them**; and **(c) P2's naming rebase replaces (a)'s ground** with a nonce-derived prefix (E8.1 §4.2/§4.4), under which a copied workspace image carrying the same instance id becomes enumerable — so **K1 must carry this invariant forward, not merely the survey's code** (§5.1). |
| **I2** | **The survey never repairs; the RECONCILER repairs, under the caller's already-held lease, and only on an exact counterpart.** *(corrected at [P1-1], both axes)* | `survey_family` returns a value: it opens the private dir with `create = false` (`residue.rs:111`) and performs no rename, no unlink, no barrier. `reconcile_counterpart` (§2.5) is the one component that writes, and it writes only (a) the restore-rename of a detached `.source`, and (b) the retirement of the counterpart family through the existing `cleanup.rs` primitives. It runs on an already-acquired `CheckedArtifact` under the lease its caller already holds — it acquires nothing and takes no lease of its own. It is **all-or-nothing**: converge and retire, or neither. Every *ordinary* action stays exactly where it is today, inside `replace_exact`/`remove_exact`. |
| **I3** | **A reconciled classification is never stronger than an exact one.** | The counterpart rule accepts **only** an exact counterpart: the surveyed action's `expected` equals the caller's `goal` **and** its `goal` equals the caller's `expected` **and** its `family_key` and `canonical_path_identity` equal the caller's **and** `artifact_root_identity` and `retained_parent_identity` match (the C2/C3 checks of `matches_request`, `authority.rs:107-110`, retained unchanged). Anything else ⇒ `Ambiguous`, exactly as today. |
| **I4** | **Exactly one interrupted action, or refuse.** | `FamilySurvey.actions.len() > 1` ⇒ treat as `unreadable` ⇒ `Ambiguous`. Two `.authority` files under one family is already `foreign` today (`residue.rs:170-173`, `:179-181`); the survey must not become the place where that stops being an error. |
| **I5** | **The record root stays the reconciliation root.** | The cure gives *leaves* a reconciler. It does **not** make the merge record recoverable from residue — that is S5, a separate step under a separate decision (§3.3). Until and unless S5 lands, **RR §2's ground stands verbatim**: the record root's detach-then-publish window is closed by no shipped reconciler, so `rewrite.rs::commit` keeps `rename_durable(replace=true)`. |
| **I6** | **Nothing on the capability-free list acquires a probe.** | Measured, and provable at the same three cites the prep used: `abort/evidence.rs::file_states` **already** acquires a `CheckedArtifact` for all three leaves on every **post-publication** v1 abort *(wording corrected at State [P3-1]: `preflight_v1_evidence` returns `Ok(())` when `record.publication` is `None`, `abort/evidence.rs:39-41`, so a pre-publication abort acquires nothing before or after the cure)* — `artifact_facts::observe` at `:285`, `:300`, `:314` → `entry::observe_merge_root_artifact` (`entry.rs:36-41`) → `root_artifact` (`:151-…`) → `CheckedArtifact::acquire` → `durable_identity`. The survey and the reconciler are both methods on an **already-acquired** `CheckedArtifact`; together they add **zero** acquisitions. Likewise `root/abort.rs:131`, `:145` and `preservation_root.rs:207`. **The forward `live.rs::snapshot` is the one exception and needs care** — see I7. |
| **I7** | **The forward observer's new acquisition is on the activated lease only.** | `live.rs::snapshot` today uses `fs::symlink_metadata` + Sha256 and acquires **nothing** (`regular_digest`, `live.rs:166-187`; E8.1 §2 C12: *"none … already T1"*). Surveying requires an acquisition it does not have. Reachability, re-driven by E4.5-B at `f563446` and unchanged in shape at `ffd4f95`: `execute::publication` ← `finalization.rs:53` (sole) ← `FinalizationRuntime::new` ← `forward.rs:69` (sole production) ← `ForwardRuntime::new` ← `start.rs:89`/`:160`, both immediately before `service::run`, and `service.rs:119-124` takes `acquire_activated` at `:121` for exactly those requests. **The forward publication observer runs under `acquire_activated` and under nothing else** — so its new acquisition is inside the checked feature's own capability envelope and touches no listed operation. This must be **written into the step's charter as its RR §4 enumeration**, not assumed. |
| **I8** | **The v0 lane is untouched.** | `acceptance/publication.rs::classify_candidate_parts` (`:81-100`) is shared with v0 through `classify_candidate_publication` (`:43-51`) and `classify_candidate_publication_view` (`:53-69`), and v0 is carved. **The normalisation happens inside `live.rs::snapshot` before `CandidatePublicationObservation::new` (`live.rs:123`)**; `acceptance/publication.rs` is not edited. (Prep §2.2's rule, adopted verbatim.) |
| **I9** | **No new door, no moved pin, no moved census key.** | The widening reshapes four existing `entry.rs` doors' parameters; the survey is one new `pub(super)` method on `CheckedArtifact` plus, if a caller outside `checked_artifact` needs it, **one** observe-shaped `entry.rs` door. **If a door is added, `ENTRY_REFERENCES`/`ENTRY_ITEMS` and the `entry.rs` byte pin move and that is amendment-tier** — S2 is written to avoid it by keeping the survey behind the existing `classify_*` doors' return values (see §7 S2). `PRODUCTION_CALLER_COUNT` stays 1 either way (no new catalog namer). The 165-key fault census is not touched. |
| **I10** | **Failure stays typed and terminal; and the cure's direction is truthful, not optimistic.** *(corrected at [P1-1], both axes)* | A survey that is `unreadable`, or reports a non-counterpart, yields exactly today's `Ambiguous`. The cure converts `Ambiguous` to **whichever endpoint is true**: `After` where the counterpart action never published (the marker class — `classification.rs:253` already answers this), `Before`/`Candidate` where the reconciler has physically restored the detached source. **REVISION 1's "only ever converts `Ambiguous → Before/Recoverable`" was wrong** and is withdrawn: it would have claimed a merge was mid-rollback when in fact the rollback was already complete. The cure never converts a refusal into a *different* refusal, and never converts a true `After` into anything. |

### 2.7 What test rows prove it

| Row | Home | Today | After |
| --- | --- | --- | --- |
| **RED-1** `an_interrupted_checked_marker_publication_converges_and_leaves_abort_working` | `v1_lifecycle/tests/finalization.rs`, in the preserved patch `…/scratchpad/e45b-demonstration-NOT-FOR-LANDING.patch:369` (with `interrupted_inside_marker_publication` at `:439`) | **RED** — `Ok(RecoveryRequired)` then `Err(RecoveryEvidenceMismatch)` | GREEN. **This is the acceptance row of the whole cure.** |
| **RED-2** `an_interrupted_checked_rewrite_detaches_the_record_beyond_the_lifecycles_reach` | `probe/e4-3-detach-window-evidence` `c9a7303`, `v1_lifecycle/tests/store.rs` (RR §1a cites `tests/store.rs:371`; the branch's own added block starts at patch offset `:197`) | **RED for discovery** — `.gwz/merge` empty, `classify_open_record` reports no open merge | **stays RED until S5.** It is the (ii) row and it is *out of scope* for S1-S4 — which is exactly why §3.3 asks the operator whether S5 is wanted. Its value now is as the **negative** pin: S1-S4 must not change its outcome. |
| NEW-1 | S1 | — | Drives §1.4: assert the reverse-pair classify on a forward residue returns `Ambiguous` **and** that `inspect_family` reported `foreign` (i.e. pins the corrected mechanism, so the record can be amended on a driven basis). |
| NEW-2 | S2 | — | Survey unit rows: one interrupted action found; a second `.authority` ⇒ `unreadable`; a foreign-root family **not** enumerated (I1); the survey performs no filesystem mutation (I2). |
| NEW-3 | S3 | — | The three abort arms (boundary/lock/marker), each interrupted on the forward pair, each reconciled on the reverse ⇒ `Ok(Aborted)`; plus a non-counterpart residue ⇒ still `Ambiguous` (I3). |
| NEW-4 | S4 | — | Forward: crash inside the lock's and the boundary's detach window ⇒ resumed run re-issues `WriteLock`/`WriteBoundary` and completes; plus the empty-baseline boundary case (prep §2.3's subtlety). |
| NEW-5 | S4 | — | L5/L6/L7 rows: an interrupted preservation transition, then the counterpart direction ⇒ reconciled (§4.7). |
| NEW-6 | S4 | — | L8 regression: the bundle's member-≥2 detach window still converges (proves the cure did not disturb the one path that already worked). |
| Pins | every step | `catalog_activation_pin.rs` `PRODUCTION_CALLER_COUNT` = 1; `ENTRY_REFERENCES`/`ENTRY_ITEMS`; `CAPABILITY_FREE_RAW_WRITER_INVENTORY` (20 rows, key-set digest `867c580f…`); `V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES` (3 rows) | S1-S3 move **none**. S4 removes three raw writers from `finalization/execute.rs` ⇒ **no inventory row moves** (`execute.rs` is absent from both inventories — E4.5-B verified, `GwzM5-8R2E-E45B-Report.md:55`) but the three residual sentences retire (§3.4). |

---

## 3. What the cure unlocks, and what stays carved

### 3.1 The residual register (`GwzM5-8R2E-E4-Close.md` §2), row by row

| Site | Row | Ground today | After S1-S4 |
| --- | --- | --- | --- |
| `finalization/execute.rs:79` (marker) *(cited `:45` in the record; **cite drift**, §5.3)* | `:277` | DIRECTIONAL-RESIDUE WINDOW | **CONVERTS.** The ground is the exact defect S3 cures; RED-1 is its acceptance row. |
| `finalization/execute.rs:88` (lock) *(cited `:48`)* | `:278` | OBSERVATION-DEAD WINDOW | **CONVERTS.** S4's forward normalisation closes it; NEW-4 is its acceptance row. |
| `finalization/execute.rs:98` (boundary) *(cited `:51`)* | `:279` | OBSERVATION-DEAD WINDOW | **CONVERTS**, together with the cell-2 disposition of §4.6. |
| `store/rewrite.rs::commit` (record root) | `:280` | RECORD-ROOT EXCEPTION: no shipped reconciler closes the discovery-dead window | **DOES NOT CONVERT on S1-S4.** The ground is narrowed, not removed: after S1-S4 a reconciler *exists for leaves*, and RR §2's own words — *"the residue is content- and discovery-sufficient for a reconciler that COULD be built; that is §5's option (a), deliberately deferred"* — become **buildable at ~200 LOC (S5)** rather than hypothetical. §3.3 puts that to the operator. Absent S5, the exception is **re-affirmed**, deliberately, which is precisely what RR §2 says the O14/DR-1 fork must do. |
| The twenty carved raw writers of `:275`-`:279` | `:275`-`:279` | CAPABILITY-FREE LIST STANDS (ruling (D)/(A)) | **STAY CARVED, unconditionally.** Nothing here touches them. |

### 3.2 The line the cure must not cross

The ruling is explicit: *"Ordinary merge / commit / create / abort / GC stay
capability-free. … Do not amend the list to put those operations on the catalog
probe."* (`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md:10`.) The cure honours
it by construction:

* The **abort-side** work (S3) runs where a `CheckedArtifact` is **already**
  acquired on every reachable v1 abort (I6). It adds no acquisition, so the abort's
  capability posture is byte-identical before and after.
* The **forward-side** work (S4) runs only under `acquire_activated` (I7) — the
  checked feature's own envelope, which the list never covered.
* The twenty listed-operation writers (`store/archive.rs`, `archive.rs`,
  `handle_commit.rs:334`, the v0 preserve/stash arms, the mutation-guard
  operations, …) are **not touched by any step**. The
  `CAPABILITY_FREE_RAW_WRITER_INVENTORY`'s shrinkage arm is the machine
  enforcement: if a step tried, the checker fails.

**An ordinary merge, commit, abort or GC gains no probe from any step in §7.**
The step charters must state that with the RR §4 enumeration, and the per-ARM
negative scans already installed by the pins package are the belt.

### 3.3 The record root — what the cure does and does not settle

RR §2's ground has two clauses: *(a)* the boundary's detach-then-publish shape
opens a discovery-dead window, and *(b)* **no shipped reconciler closes it**.
S1-S4 leave *(a)* untouched and make *(b)* **contingent rather than structural**.
That is a real change to the ground's status and RR §2 anticipates it in terms
(*"if converted leaf writes move onto the write authority with a reader-side
residue reconciliation (§5), the record root converts there or the exception is
re-affirmed there, deliberately"*).

The decision is genuinely open and belongs to the operator (§6 Q1). The lane's
reading:

* **S5 is small** (~200 LOC): `classify_open_record` (fn at **`store/mod.rs:216`**, *cite corrected at Code [P3-5]*)
  and `discover_open` gain a survey pass over `.gwz/checked-artifacts` for a family
  whose `canonical_path_identity` resolves under `.gwz/merge`, reporting
  OPEN-DETACHED with the id read from the surveyed `expected` bytes.
* **S5 is disproportionately risky, on TWO independent grounds** *(the second
  added at State [P1-2])*:
  * *Discovery.* It puts *discovery itself* — the entry point of every merge
    command, including the capability-free ones — on a private-area read.
    `recovery_support.rs:360-363` deliberately ignores private residue today. A
    survey there is read-only and needs no probe *if* it is a plain directory
    scan, but routed through `CheckedArtifact::acquire` it would put **discovery**
    on the identity probe — the ruling's red line.
  * *The lease.* **Discovery reconciliation is not sufficient to convert the
    record root, and REVISION 1 was wrong to imply it was.** RR §1b: `commit`
    (`store/rewrite.rs`) is shared by the activated forward arms and the
    **plain-lease reverse arms** (`service.rs:119-124`: `ResumeStart | Continue ⇒
    acquire_activated` at `:121`, everything else ⇒ plain `acquire` at `:123`).
    Converting `commit` therefore puts **every standard-path v1 abort** on the
    identity probe. RR §4's Plain-Lease Probe Clause leaves exactly two live
    routes — split the lease, or prove the reverse reach absent — because the
    third, "amend the capability-free list", is **CLOSED by ruling (A)**. So S5
    is *discovery reconciliation **plus** a lease split of `commit`*, and its
    ~200-LOC figure — already flagged UNVERIFIED — **cannot hold** once the split
    is included.
* **In every outcome, the reverse arm's rewrite stays RAW.** The lease split is
  what makes that explicit rather than accidental: the forward arm may convert,
  the reverse arm may not, and RR §2's exception is **re-affirmed for the reverse
  arm regardless of Q1's answer**.
* **The lane recommends: build S1-S4, then RE-AFFIRM the record-root exception**
  and record S5 as a named, sized, deliberately-declined option — unless the
  operator wants the forward rewrite arm converted, in which case S5 is chartered
  separately with two stop-triggers: *discovery takes no probe*, and *no reverse
  arm moves onto the boundary*.

### 3.4 The abort sentence — precisely what it becomes

Today (settled by CapFree §6, shipped at ten homes; `docs/OperationModel.md:149-155`,
`checked_artifact/capability.rs:50`, `:54-55`, `v1_lifecycle/checked.rs:129-133`,
`runtime/dispatch.rs:437-438`, `:447-448`, `v1_lifecycle/service.rs:112-115`,
`OperationModel.md:131-132`, `:158`, and the release-notes line with no on-disk
home):

> *"an abort that touches no checked artifact needs no such filesystem; aborts
> that must re-verify checked artifacts — preservation bundles, a selected root's
> manifest and lock, or the merge's published evidence, re-verified through the
> checked boundary — need persistent file handles and a mount identity."*

**After S3-S4 the sentence's *scope* does not change and its *ground* does** —
but it must be written against **E8.1 §4.7's TIERED sentence, which retires
CapFree §6's text**, not against CapFree §6 itself. REVISION 1 rewrote the
retiring text and is withdrawn here *(State [P2-3])*. The two compose as
**tiered sentence + scoped addendum**, in that order:

> **[E8.1 §4.7's tiered sentence, adopted, unchanged — it owns the filesystem
> and tier claims.]** An abort that touches no checked artifact needs no such
> filesystem. An abort that must re-verify checked artifacts — preservation
> bundles, a selected root's manifest and lock, or the merge's published
> evidence — recovers exactly at tier 3, and at tier 2 within the session;
> outside that it refuses with `IdentityUnprovable`.
>
> **[This document's addendum, scoped INSIDE that tier.]** *Within the tier at
> which it re-verifies*, such an abort **reconciles** an interrupted publication
> from the merge's own private residue rather than refusing it: an absent or
> detached leaf whose family holds the exact counterpart action is converged to
> that action's endpoint and the counterpart family is retired, so
> `gwz merge --abort` clears a merge interrupted **at any point of its
> publication that the tier can still prove**.

**Two claims REVISION 1 made and this revision drops.** (a) *"clears a merge
interrupted at any point of its publication"* — unqualified, it is **false at T1
and at T2-after-reboot** (E8.1 refuses *before* reconciliation is reached), and it
was **false even at T3** under REVISION 1's own mechanism, since [P1-1] showed the
executor still refused. The addendum is now scoped by tier and is true only
because §2.5 exists. (b) The legacy-probe parenthetical and the
`provider/platform/linux.rs` cite are **dropped entirely** — E8.1 §4.7 replaces
filesystem-naming with tier claims, so repeating a filesystem list here would
re-introduce the text it retires (and the cite has drifted to `:142`/`:132` at
`d6830cd` anyway).

**One coupling this exposes, owed to S2** *(State [P2-3], second half)*: E8.1
§4.4's corrected rule is *"degrade the classification, never make the record
foreign, never make it invisible."* I4 as written (`actions.len() > 1 ⇒
unreadable ⇒ Ambiguous`) collapses a **tier-degraded** family into the same
bucket as a **foreign** one. **S2 must distinguish the two**, or (c) P3
re-litigates S2. Folded into S2's charter and into K1.

Three further consequences at the same ten homes:Three further consequences at the same ten homes:

1. The dated residual CapFree §6 attaches (*"never driven on a non-admitted
   filesystem … cured only by DR-1's (C)"*) **narrows**: after S3, the abort's
   reconciliation path is driven by NEW-3 on ext4, which is a driven basis the
   sentence did not have. The *non-admitted-filesystem* half of the residual is
   untouched — that remains (C)'s, i.e. E8.1's.
2. E8.1 §4.7's **tiered** rewrite of the same sentence and this rewrite touch the
   same ten homes. **They must land as one edit or in a stated order** (§5).
3. The `finalization/execute.rs:45,48,51` cite inside CapFree §6 is **already
   drifted** at `ffd4f95` (the sites are `:79`, `:88`, `:98` after E4.7's residual
   comments) and retires entirely with S4.

---

## 4. The other agenda items — dispositions and sizes

### 4.1 O14's write-authority fork (`authorize_write` / `RetainedWriteAuthorityV1`)

**Measured at `ffd4f95`.** `authorize_write` is an inherent method on
`AdmittedCheckedActionV1` at `checked_artifact/coordinator/execution.rs:318-355`.
**Production callers: ZERO.** Test callers: **6 call sites in 4 tests**, all in
`coordinator/tests_execution.rs` (`:284`, `:296`, `:359`, `:410`, `:430`, `:448`).
`RetainedWriteAuthorityV1`: struct `execution.rs:399-405`, `impl` `:407-432` (five
accessors), **constructed at exactly one site — `execution.rs:348`, inside
`authorize_write` — and consumed nowhere in production.** The file says so itself
at `execution.rs:163-165` (*"`authorize_write`/`RetainedWriteAuthorityV1` still
have no production consumer"*) and `:395-397` (*"nothing in R2-D consumes it
yet"*). A retirement deletes the contiguous block `:276-432` (153 code lines) plus
the family-only helper `require_leaf_digest` (`:358-380`), three now-unused
imports (`:78`, `:90-91`), the module-doc bullet (`:58-69`) and a paragraph at
`:161-170` — **≈172 production lines**, plus ≈211 test lines.
`CheckedAuthorityObservationV1` / `CheckedAuthorityRecordV1` **survive** — they
have a live consumer at
`capability/pre_catalog/provider/authority_record_binding.rs:89-90`, `:432`,
`:476`, `:588`.

**Disposition: AMEND THE FREEZE, do not wire — with one carve.** The charter's
own framing (`GwzM5-8DR1-Charter.md:45-50`) is *"route converted leaf writes
through `authorize_write`/`RetainedWriteAuthorityV1`, **or** amend §8 `:239-240` /
§9 `:264-266` to the landed architecture"*, and E4.2 review flag 6 records zero
production consumers while §9 names path-writing as **not** parent authority.
Two grounds decide it:

1. **The landed architecture already carries the authority the type was to
   carry, in a different place.** `CheckedArtifactAuthority` (`authority.rs`)
   *is* a retained write authority: it records the retained parent identity, the
   retained source, the operation, the exact `expected`/`goal`, and it is
   **durable** (`encode`/`decode`, `:116-144`). `RetainedWriteAuthorityV1` would be
   a second, in-memory, undurable expression of the same fact. The cure in §2
   makes the durable one *readable*, which is the capability the fork wanted.
2. **Wiring it would fight I9.** Every converted seam would take a new parameter,
   which is the surface change the freeze exists to gate.

**Sizing:** amend `§8 :239-240` / `§9 :264-266` to name `CheckedArtifactAuthority`
as the landed write authority and record `authorize_write`'s retirement —
**amendment text plus a ~172-line production deletion (+ ~211 test lines)**,
dual-tier because it is frozen text. **Carve, now measured and real:** the four
tests at `tests_execution.rs:276`, `:312`, `:380`, `:424` pin genuine properties
(an observation may authorize only its own action; the pairing gate refuses each
varied reservation input; a parent-only action takes no write authority). **Those
properties must be re-homed onto `CheckedArtifactAuthority`'s own suite before the
deletion lands** — that is the step's headline risk, not the deletion.

### 4.2 The tier-2 comparable archive sub-surface

**Disposition: KEEP DEFERRED — do not mint at E8.3, ON THE OPERATOR'S WORD
(Q9), not on the lane's.** *(re-grounded at State [P2-5].)* This is a
**re-deferral of something an adopted record assigned to this round**: CapFree §5
says *"DR-1 mints the sub-surface BY AMENDMENT WITH ITS OWN DUAL and names the
execution carrier"*, and the charter's §4 deliverable 3 says the same. A lane
cannot discharge that by declining it. **The disposition is therefore conditional
and carries a dated bracket at two homes** — `GwzM5-8R2E-CapabilityFreeAmendment.md`
§5 and `GwzM5-8DR1-Charter.md` §4 deliverable 3 — recording that DR-1 reached the
item, sized it, and re-deferred it on the operator's answer to Q9. Ruling point 4
("explicitly defer") permits exactly this, in one line. The ruling deferred it
once already (point 4) and CapFree §5 makes minting conditional on a *carrier*.
The carrier question is downstream of two things E8.3 does not settle: whether
the archive conversion is revived at all (§4.5), and (C)'s shape (E8.1's (b)/(c)
choice, §6 Q2 there). Minting a comparable sub-surface for an archive route that
may be **deleted** (§4.5) would be the E4.4 failure mode repeated. **Re-open it
in the same act that decides §4.5**, with the eight byte-preserved-v0-origin
corpus rows' carrier named there. Sizing if minted later: amendment + dual, no
production code. *(The eight machine-enforced carrier strings in
`GwzM5-8I2CompatibilityPredicates.json` already point at DR-1 — CapFree §5 — so
the deferral costs no re-pointing.)*

### 4.3 The record-root re-examination

**Disposition: RE-AFFIRM, with the ground narrowed and S5 named.** See §3.3.
The re-examination RR §2 mandates is **discharged by this document** whichever
way the operator rules: the fork has been reached, the alternative is sized, and
the choice is recorded. Sizing: amendment text only (a dated bracket in RR §2
recording the re-examination and its outcome) unless S5 is taken.

### 4.4 The legacy in-place-writer retirement inside `.gwz/checked-artifacts`

**The writers, enumerated at `ffd4f95`.** The path literal is minted in exactly
one production place — `catalog_names.rs:88` (`LegacyPrivate => b"checked-artifacts"`)
— resolved by `policy.rs:46-54` and stored as `quarantine_parent`
(`observation.rs:93`, `mod.rs:140`). A second, **read-only** copy lives at
`git/gitbackend/preservation_image.rs:8` (`CHECKED_ARTIFACT_PRIVATE_PATH`) for the
git-status dirt exemption and preservation-image blindness. **No production writer
outside `src/checked_artifact/` touches the area**; every other hit on the literal
is test code. Inside it, twelve production writes, all under
`DirentBarrierClass::AnchoredPrivateArea` (`platform.rs:365-369`):

| # | Site | Writes |
| --- | --- | --- |
| 1 | `residue.rs:47-52` → `observation.rs:41`, `:44` | creates `.gwz/` then `.gwz/checked-artifacts/` component-wise, fsyncing each parent (`open_private(create=true)`) |
| 2 | `residue.rs:102` → `platform.rs:688-695` → `platform/anchor.rs:305-317` | **Windows only**: the permanent `.ca1-durability-anchor-<32hex>` leaf. Non-Windows `prepare_private` (`platform.rs:345-352`) is a no-op |
| 3 | `residue.rs:459`, `:472`, `:485` | the authority-record scratch (mode `0o600`) |
| 4 | `residue.rs:508-519`, barrier `:525` | publishes that scratch onto the authority-record name |
| 5 | `residue.rs:363`, `:371`, `:379` | the goal scratch (mode `0o644`) |
| 6 | `residue.rs:400-411`, barriers `:417`, `:428` | publishes it onto the identity-derived staged-goal name |
| 7 | `transition.rs:290-301` | renames the managed leaf **into** the area as the quarantined source |
| 8 | `transition.rs:391-401` | renames the staged goal **out** onto the managed leaf |
| 9 | `cleanup.rs:87`, barrier `:89` | retires the staged goal |
| 10 | `cleanup.rs:132`, barrier `:134` | retires the quarantined source |
| 11 | `cleanup.rs:222`, barrier `:224` | retires the authority record |
| 12 | `residue.rs:582-604` | `rebarrier_exact` — no content write; reopen + `sync_all` + `private_barrier` |

Items 7 and 8 are the only cross-boundary edges: the private area is both the
quarantine destination and the publication source for the managed leaf.

**What must land first, regardless of the enumeration:** the legacy in-place
writer **is** the pre-catalog `CheckedArtifact` every converted path rides
(CapFree §4), and its private area is `CatalogPrivateNameV1::LegacyPrivate`,
deliberately **not** the catalog's Final directory — `policy.rs:33-45`, whose
comment records the R2-F R1.1 split and that *"the leaf bytes are unchanged —
`checked-artifacts` — so the legacy area does not move, its residue is not
orphaned, and its git-status dirt exemption and preservation-image blindness stay
correct where they already are"*. **The survey of §2.2 reads exactly that area.**
Therefore:

> **BINDING ORDER: the legacy area may not be retired, moved, or re-rooted while
> any observer surveys it.** *(Stated exactly, per State [P3-2] — REVISION 1
> overstated this.)* The post-publication abort **already** reads
> `.gwz/checked-artifacts`, through `inspect_family` inside every `classify_*`
> door (`classification.rs:140`); it is not a new *read* dependency. What S1-S4
> change is **what** is read and **that it is now written**: the survey widens
> the read from one action key to the whole family, and §2.5's reconciler makes
> the abort a **writer** of that area for the first time (a restore-rename plus a
> foreign-family retirement). **That second half is the genuinely new
> dependency**, and it is the one the binding order protects. The retirement's
> prerequisite list gains: (1) the survey's area must be a parameter of the
> policy, not a literal; (2) a migration must move residue, not just future
> writes, or an in-flight interrupted publication becomes unreconcilable across
> the upgrade; (3) OPEN-R1's retire-the-area question (`RelocationPlan` §2) is
> **answered NO for the duration of R2-E** on that ground.

**Sizing:** the retirement itself is not chartered here. The order above is one
dated clause in the RelocationPlan and one in CapFree §4 — amendment-tier text.

### 4.5 O8's `gc_archived` route, conditional on (C)

**MEASURED AT `ffd4f95` — and the extent is under-enumerated a SECOND time.**
The record's history: CapFree §5 named 3; E4.7's fold corrected the in-tree note
(`archive.rs:112-126`) to **11 functions + 2 structs + 4 members**, on the E4.7
review's P3-1 measurement. This session's independent pass measures **12
functions + 2 structs + 4 members**. The one the note still misses:

> **`merge/gc.rs:169-176` `cleanup_error`** — 8 lines, three uses (`gc.rs:71`,
> `:88`, `:105`), all three inside `preflight_archived_cleanup`, which is itself
> family-only. It dies with the family and nothing else calls it.

The full set, with spans:

* **`v1_lifecycle/archive.rs` — 8 functions, 160 code lines:** `gc_archived`
  (`:106-151`, allow at `:135-144`, fn at `:145`), `gc_archived_with_hook`
  (`:153-183`), `any_open_record_present` (`:201-222`), `remove_archive`
  (`:224-242`), `require_same_archive` (`:244-260`), `require_real_directory`
  (`:262-272`), `require_regular_file` (`:274-284`), `io_error` (`:290-292`).
  `use std::fs;` (`:1`) and `use crate::durable_fs::sync_dir;` (`:13`) become
  unused. **Correctly excluded** (live): `acquire_archived` `:62`,
  `archive_terminal` `:70` (caller `start.rs:118`), `recovery` `:286`,
  `observe_open` `:185`, `open_record_present` `:194`,
  `CanonicalArchiveAcquisition` `:22`.
* **`merge/gc.rs` — 4 functions + 2 structs, 154 code lines:**
  `preflight_archived_cleanup` (fn at **`:53`**, doc from `:31`; *cite corrected at Code [P3-5]*), `delete_preflighted_backup_refs`
  (`:132-142`), `require_backup_refs_absent` (`:144-167`), **`cleanup_error`
  (`:169-176`)**, `PreparedArchivedCleanup` (`:18-20`), `ArchivedBackupArtifact`
  (`:22-29`). **Correctly excluded** (live, reached from `handle_gc` `:178`):
  `BackupArtifact` `:10-16`, `preflight_backup_artifacts` `:224`,
  `preflight_owner_evidence` `:276`, `attach_member` `:407`, and
  `preflight_direct_ref` `:351` — which is **dual-use** (`:112` family, `:339`
  live) and must NOT be deleted.
* **`v1_lifecycle/archive_result.rs` — 4 family-only members, 15 lines:**
  `source_version()` `:30-35`, `destination_bytes()` `:37-39`,
  `destination_sha256()` `:41-43`, `cleanup()` `:49-51`, plus the fields
  `source_version` `:11` / `destination_sha256` `:13` and their initialisation
  `:20-27`. `projection()` `:45-47` **survives** (live readers `status.rs:116`,
  `gc.rs:213`), as does the struct.

**Production deletion: ≈329 code lines / ≈350 lines of file.** Test side:
`v1_lifecycle/tests/gc.rs` has 7 `#[test]`s of which **6** drive the family
(`:11, :27, :53, :89, :143, :194`); the seventh (`:331`) covers the live
`post_gc_record` and survives.

**Both fail-closed shrinkage arms fire, verified by token count:** `archive.rs`'s
`sync_dir` tokens are `:13` (import) + `:241` (call) = 2, matching O13's
`V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES` (`check_checked_artifact_boundaries.py:413`),
dropping to 0; its `remove_file` token is `:240` only = 1, matching the carve-out
inventory row (`py:527`), dropping to 0. **So the deletion trips both `py:1470-1476`
(O13) and `py:1500-1512` (the carve-out inventory) — the amendment must land with
the deletion, in the same commit.**

**One cite correction owed while here.** The ruling's launch item reads *"GC:
`gc.rs` `decode_production_v0` on archive bytes"*
(`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md:23`). Measured: **there is no
`decode_production_v0` in `merge/gc.rs`.** `gc.rs` calls
`record_wire::decode_archived_common` (`:206`) and `decode_archived` (`:209`) —
which is what the landed GC fix cured. `decode_production_v0` lives at
`record_wire/decode.rs:163` and is very much alive (production callers
`status/snapshot.rs:170`, `store/mod.rs:419`, `store/atomic_upgrade.rs:80`,
`record_wire/checked_owner.rs:79`). The ruling is a verbatim quote and is not
edited; the correction is a dated bracket in the amendment that quotes it.

**Disposition: DELETE — recommended; the decision is the operator's because it
fires O13's shrinkage arm.** The route was built for a converted archive; the
archive is **carved out permanently** by CapFree §3 (every archive writer runs
from a listed operation), and the ruling closed (A). With (C) now being E8.1's
design, the revival condition can be tested: **(C) does not revive it.** E8.1's
(b)/(c) admit *more substrates* to the checked feature; they do not move any
archive writer off the capability-free list, because that list is what carves the
archive, not the filesystem. **The condition the route was kept for cannot be met
by (C) at all** — only by a *reversal of ruling (A)*, which is closed.

* **Delete**: removes 12 fns + 2 structs + 4 members (≈329 production code lines,
  6 tests), retires the two KEPT permanent-pending-DR-1 allowances
  (`GwzM5-8R2E-E4-Close.md:55`), fires **both** shrinkage arms (amendment-tier —
  a shrink is a *pin failure* by design, so the amendment lands in the same
  commit), and deletes the stale inventory row of §4.10 outright.
* **Rebuild against (C)**: no consumer can arrive (above). Not recommended.
* **Keep**: reproduces the "permanent by silence" failure E4.7 exists to stop,
  and now with a *twice*-under-enumerated extent record.

**Record homes, named** *(State [P3-3])*: O13's accepted-residual at
`GwzM5-8R2E-Plan.md` §1.1; **CapFree §3's `:275` inventory row** (`archive.rs`,
`remove_file: 1`); **CapFree §5's O8 paragraph** ("conditional on (C)");
**RR §6's inverted bracket** (the archive files "permanent-documented in all
three rows"); plan `:53` and `:512-513`. The (C)-cannot-revive argument is a
**record** argument, not a code measurement, and stands on ruling (A)'s closure.

**Sizing:** one amendment (O13 shrinkage + the carve-out inventory row + the two
allowance expiries) + one deletion step, **≈350 lines removed, ~60 added** (the
inventory/digest re-pinning). Foundational-independent — it can run in parallel
with S1-S5. **Its one real hazard is `preflight_direct_ref` (`gc.rs:351`), which
is dual-use**; the step's stop-trigger is "any deletion that reddens a live
`handle_gc` path".

### 4.6 Row `:279`'s frozen cell-2 wording vs the shipped reverse writer's workspace door

The frozen cell 2 reads *"checked Git-directory artifact action"*. Measured, and
confirmed by a second independent pass: **every shipped reverse writer takes the
WORKSPACE door.** `execute_v1_evidence_rollback` (`abort/evidence.rs:125-172`),
the exact reverse pair of the forward publication arms, routes all three of its
physical arms — boundary `:148-153`, lock `:154-159`, marker `:160-164` — through
`artifact_facts` → `entry::replace_merge_root_artifact` (`entry.rs:43-51`) or
`remove_merge_root_artifact` (`:65-71`) → `root_artifact`, which builds
`CheckedArtifactPolicy::workspace(root)` at **`entry.rs:161`**. The second reverse
writer (`preservation_root.rs:477`, `:483` → `files::replace_relative` →
`entry::replace_merge_preservation_workspace`) is likewise workspace-rooted
(`entry.rs:179`). The only Git-directory-rooted policy site is `entry.rs:188`,
inside `preservation_git_directory` (`:186-193`), reached solely from
`files::observe_boundary` (`preservation_root/files.rs:28-35`), which **never
mutates** — recorded in-tree at `policy.rs:41-45`: *"no production write lands
under `<git-dir>/gwz/` through this policy"* (whose own `entry.rs:182` cite is
stale for `:188`, §1.3 L10). **Net: zero production writes reach `<git-dir>/gwz/`.**

**Disposition: AMEND THE CELL, do not route the writer.** Cell 2 becomes
*"checked **workspace** artifact action on the repository's boundary file"*, with
a dated bracket recording that the leaf lives under `.git/info/` but is written
through the workspace root because the workspace root is what the boundary's
identity is anchored to. Routing the writer to a new Git-directory replace door
instead would cost a **new `entry.rs` door** (prep §3.3, which also found the
amendment's premise for that door falsified by the tree, §3.2) — moving
`ENTRY_REFERENCES`, `ENTRY_ITEMS` and the `entry.rs` byte pin for no behaviour.
**Record homes, named** *(State [P3-4])*: the FROZEN cell is
`GwzM5-8R4bR2ConsumerCheckpoint.md` §10 row `:279`; **CapFree §3's `:279`
dispositions row** ("E4.6-B converts … through a new git-directory replace door")
needs its own bracket. Both are frozen-tier and **one dual can carry both**.

**Sizing:** frozen-text amendment, own dual, ~1 cell + 2 brackets. **Must land
with or before S4**, since S4 converts the forward arm through the same workspace
door and would otherwise land a writer the frozen cell contradicts.

### 4.7 The preservation-bundle audit (E4.5-B flag 9) — MEASURED, and it splits in two

The flag asked whether the preservation-bundle leaves *"also carry checked
writers on more than one arm"*. Measured at `ffd4f95`, the answer is **different
for the bundle than for the preservation-root leaves**, and the record should say
so:

* **The bundle itself (L8): NO directional-residue hazard — exactly ONE checked
  arm, and it is the *reverse* one.** `replace_merge_preservation_bundle`
  (`entry.rs:139-148`) has one production route:
  `reverse/execute/preservation.rs:74` → `v1_write_bundle_checked`
  (`checked_bundle.rs:64-112`) → the door at `:93`. `entry.rs` has **no** bundle
  remove door. The bundle is only ever *grown*: `before = expected_bundle(record,
  &plans[..index])`, `after = … &plans[..=index]` (`checked_bundle.rs:80-83`).
  It **does** carry a window-(i) detach window for members ≥ 2, and — the useful
  finding — **it already survives it**, because the retry asks the same
  `(expected, goal)` pair, so `inspect_family` matches and the boundary's own
  `Recoverable` convergence runs (`checked_bundle.rs:85`, `:99-105`). **L8 is the
  cure's proof-of-concept, not its problem.**
* **But the bundle has a DIFFERENT, un-named exposure the audit found: residue
  orphaning by its RAW siblings.** The same leaf is written by three raw,
  permanently-carved writers that neither observe nor classify checked residue —
  the v0 `--abort --preserve` forward arm (`preserve/artifacts.rs:357` →
  `stash::write_bundle`, `stash/mod.rs:261-263`, carved as `:276` at
  `stash/mod.rs:260`) and six `gwz stash` sites
  (`handle_stash/commands.rs:68,:86,:93,:122,:318,:327`, carved at `:67`). A
  crash inside `v1_write_bundle_checked` followed by any of those leaves a staged
  goal, a quarantined source and an authority record in `.gwz/checked-artifacts`
  describing a leaf that has since been overwritten by a writer that has never
  heard of them. **This is not the reconciliation class and must not be cured by
  conversion** — the raw siblings are on the capability-free list and may not take
  a probe (§3.2). **Disposition: a cleanup sweep, not a conversion** — the next
  `cleanup.rs` pass retires a family whose authority describes a leaf that no
  longer matches either endpoint. Sized ~120 LOC, independent of S1-S5, and named
  here for the first time.
* **The preservation-ROOT leaves (L7): YES — one door, both directions,
  shipped.** `files::replace_relative` (`preservation_root/files.rs:37-49`) takes
  `source: Option<&GitCandidateFile>, goal: Option<&GitCandidateFile>` and is
  called from `mutate_managed`'s `MarkerWorktree` arm (`preservation_root.rs:477-482`)
  and `LockWorktree` arm (`:483-488`) with `source = form(spec, transition.source)`
  and `goal = form(spec, transition.goal)` — i.e. **the same leaf, the same door,
  with the pair swapped by the transition's direction**. The reader
  `files::observe_transition` (`:51-…`, called at `preservation_root.rs:207`) is
  bound to the same direction, so within one transition the residue matches; the
  hazard is an interrupted transition followed by its **counterpart**, which is
  exactly the E4.5-B wall on shipped code.

**Disposition:** L7's exposure is **already shipped and undated**, and **it has
no adopted home** — E4-Close §2's residual register lists *raw* arms only, and L7
is a *checked* arm *(State [P3-7])*. **Proposed home: a dated bracket at
E4-Close §2** (or CapFree §6, which already carries the preservation abort path).
**Q7 is therefore answered "now"**, not "at S4's landing": an undated shipped
exposure is precisely the state the residual register exists to prevent. It gets
a dated residual line in the `[R2-P3-1]` form, and its cure is
NEW-5 inside S4 — **not a separate class**. The record's flag-9 sentence
(`GwzM5-8R2E-E45B-Report.md:69`, `GwzM5-8DR1-Charter.md:119`) asked about the
*bundle* leaves; **the answer is that the hazard is at the preservation-ROOT
leaves instead**, and the bundle carries a different exposure (residue orphaning)
that the flag did not anticipate. Both belong in the corrected sentence.

**One defect found in passing, worth a one-line fold.**
`preserve/checked_bundle.rs:1` still reads *"Test-gated v1 preservation-bundle
checked adapter."* **That is false at `ffd4f95`**: the module is unconditionally
compiled (`preserve.rs:17`, no `cfg`), its re-exports are unconditional
(`preserve.rs:28-30`), and `v1_write_bundle_checked` has a live production caller
at `reverse/execute/preservation.rs:74`. Same class as E4.7's expired A1-era
wordings; fold it into whichever step touches the file.

### 4.8 The two ownerless A1-era allowances

Both measured by the E4.7 review (`GwzM5-8R2E-E4.7-Review.md` P3-4) and swept
without disposition; DR-1 owns them.

* **`workspace_ops/merge/store/archived.rs:3-9` — MEASURED REDUNDANT** (removal
  rc=0, 0 errors). **Disposition: EXPIRE.** It is `observation.rs:155-158`'s case
  exactly, and that one was expired at E4.7. ~7 lines removed. Fold into any
  landing step; no amendment (no adopted record owns it, which is why it survived).
* **`workspace_ops/merge/store/mod.rs:44-50` — MEASURED LOAD-BEARING** (removal
  rc=101: `method load_archived is never used`). Caller enumeration, done this
  session: the trait default is `store/mod.rs:56-58`, the `FileMergeStore` impl
  `:117-119` (delegating to `archived::load`, `store/archived.rs:14`).
  **Production callers: ZERO — confirmed.** Test callers: **8** —
  `store/tests.rs:176,:298,:313`, `g23/archive_equivalence_v0.rs:266`,
  `g23/characterization_archive_v0.rs:167,:301`,
  `g23/preserve/preserving_abort_gate.rs:99`, `g23/continue_merge.rs:228`.
  **One check owed before the deletion** *(State [P3-5])*: `load_archived` is a
`pub` method on the **`MergeStore` trait**, i.e. a trait *surface*. A grep of
`GwzM5-8R2DInterfaceFreeze.md` returns no hit, but whether that surface is frozen
**elsewhere** is **UNVERIFIED** — settle it before deleting, not after.

**Disposition: RETIRE `load_archived`, do not consume it** — a dead `pub`
  method kept alive by an allow is the shape E4.7 exists to stop, and consuming
  it would invent a caller to justify a method. ~25 LOC removed; **the eight
  test call sites are the cost** and must be re-homed or retired with it, which
  makes this a ~120-LOC step rather than a fold. The asymmetry between the two
  allowances is now explained, not just measured: (a) is redundant because
  `archived::load` has an in-crate caller at `store/mod.rs:118`, so `dead_code`
  never fires; (b) bites because under `not(test)` the *trait method* is
  genuinely uncalled.

### 4.9 The `CatalogOwnerV1` re-export narrowing

**The cite in the record is STALE and should be re-pointed in the same act.**
E4-Close §5 and E4.7's C7(ii) both say `checked_artifact/catalog.rs:44-48`. At
`ffd4f95` those lines are `)]` / `mod enumeration;` / `mod scratch;` / blank /
a comment. **The allow is at `catalog.rs:51-60` and the `use` at `:61-63`**
(`pub(in crate::checked_artifact) use bootstrap::{CatalogOwnerEdgeV1,
CatalogOwnerV1, OpaqueRetainedCatalogV1, recover_or_create};`), with the
explanatory comment at `:48-50`.

Declined at E4.7 as a **surface** change outside that step's "allows, reasons,
comments only" charter (`GwzM5-8R2E-E4.7-Review.md` flag 5: *"DECLINE UPHELD"*),
and named a DR-1 sub-item. **Disposition: TAKE IT.** The decline was on *charter
scope*, not on merit, and the "bites on exactly one name" measurement
**re-verified this session**: every use of `CatalogOwnerV1` outside its own
module is either the re-export itself (`:62`), the allow's own reason text
(`:56`), doc prose (`interface_tests/catalog_activation_pin.rs:24`) or a string
literal in a pin table (`interface_tests/capability_permit.rs:310`, `:335`).
The definition and every real call site are intra-module
(`catalog/bootstrap.rs:17`, `:236`, `:239`, and its tests), resolving through
`bootstrap.rs` and not through the re-export. The other three re-exports are all
consumed. **Sizing:** ~2 lines + one tree-digest re-pin (`catalog.rs` is a
tree-digest root, `py:262-270`; E4.7 already re-pinned it once).
**Parallel-safe; assign it to whichever step touches `catalog.rs` first, or run
it standalone.**

### 4.10 The stale inventory-row description

`scripts/checks/check_checked_artifact_boundaries.py:527` — the
`CAPABILITY_FREE_RAW_WRITER_INVENTORY` row (declared `:510`, 20 rows, closing
`:531`) for `v1_lifecycle/archive.rs`, verbatim at `ffd4f95`:

```python
"workspace_ops/merge/v1_lifecycle/archive.rs": (":275 the DEAD `remove_archive` arm behind the `:108-111` allowance", {"remove_file": 1}),
```

**Re-measured and confirmed.** `archive.rs:108-111` today is the tail of a *doc
comment* (E4.4-6-B's dated bracket), not an allow; the `#[allow(dead_code, …)]`
spans **`archive.rs:135-144`**, above `gc_archived` at `:145`. The Rust side was
already fixed at the E4.7 fold — `archive.rs:238` reads *"behind the `:135-144`
allowance (`:108-111` as adopted; re-measured at the E4.7 landing)"* — and only
the Python copy was left. *(Note: E4.7's review predicted `:127-136` at
`d8c140f`; the landing fold moved it again, to `:135-144`.)*

**The digest hashes keys only — verified at the source** (`py:532-539`:
`hashlib.sha256("\n".join(sorted(CAPABILITY_FREE_RAW_WRITER_INVENTORY)))`, and
`sorted(dict)` iterates keys). The tuple values — the description string and the
`counts` dict — are outside it; `counts` is checked dynamically per row at
`py:1482-1513`, and the description string is used **only** in failure messages
(`:1486`, `:1502`). `v1_lifecycle/archive.rs` is not a direct
`PROTECTED_SOURCE_TREE_DIGESTS` entry (`py:262-270`) — it is covered only through
the `v1_lifecycle/mod.rs` tree root — and the scan at `:1490` runs
`mask_non_code` first. **So the stale cite is invisible to every gate**, which is
exactly why it survived two sweeps. **The fix moves no pin.**

**Disposition: FIX IT — but AFTER §4.5's decision**, because if the deletion
lands the row's subject disappears and the whole row goes with it (which itself
trips the key-set digest, `py:532-539`, and is amendment-tier). ~1 line if the
family is kept; zero if it is deleted.

### 4.11 `protocol/authority_record.rs:50-53` (E4.7 C7(i))

An R2-D-era forward reference to a completed step (*"plan §4 Step 3.3 wires its
production consumer"*; Step 3.3 landed without wiring it). Adjacent to E4.7's
class, routed here so the next sweep does not rediscover it. **Disposition:
RE-REASON to the landed fact** — the seam is installed, no production consumer
arrived, and §4.1 explains why one is not owed. ~2 lines, fold into §4.1's
amendment landing.

---

## 5. Sequencing against E8.1

E8.1's adopted §7 orders: **(a0)** the two gratuitous `require_ext4` deletions
(~30 LOC, unconditional, no dual beyond the landing gate) → **(b)** legacy-parity
admission + gwz-minted instance id (~950-1100 LOC, three steps, Track-W freeze +
dual) → **(c) P1-P2** (evidence type + complete naming rebase) → **the R3 cure
(this document) before, or with, (c) P3** → **(c) P3-P5**. Its §6 Q2 leaves the
operator three options: **(i)** (a0)+(b) now then (c); **(ii)** (c) P1's
`IdentityEvidenceV1` first as (b)'s vehicle; **(iii)** hold everything for (c).

### 5.1 The two hard couplings

**K1 — the naming rebase (c) P2 and the survey are the same surface.** E8.1 §4.2
is a *complete naming rebase*, `family_key` included, and §4.4's corrected design
rule is *"a tier change … must leave the family **enumerable** — which §4.2's
nonce-derived `family_prefix` guarantees"*. The survey of §2.2 enumerates by
`family_prefix` and decodes `.authority` records. **If (c) P2 lands after the
survey, it must carry the survey forward; if it lands before, the survey is
written against the new names.** Either order works; **both at once does not.**

**K1 carries an INVARIANT, not just code** *(State [P2-2])*. I1's exclusion proof
rests in part (a) on `family_key` = SHA-256(root durable identity + canonical
path). (c) P2 **replaces that ground** with a nonce-derived `family_prefix`
(E8.1 §4.2/§4.4), under which a **copied workspace image carrying the same
instance id becomes enumerable** — part (a) weakens, and part (b), the
classifier's counterpart proof, becomes the whole boundary. So (c) P2's charter
must **re-state and re-prove I1**, not merely re-point the survey's names.
**Second K1 obligation** *(State [P2-3])*: S2 must already distinguish a
**tier-degraded** family from a **foreign** one (E8.1 §4.4's rule: *"degrade the
classification, never make the record foreign, never make it invisible"*), or
(c) P3 re-litigates S2.

**K2 — the abort sentence has one set of ten homes, and E8.1 owns the base text.**
*(Corrected at State [P2-3]: REVISION 1 named CapFree §6 as the base; E8.1 §4.7
**retires** that text.)* §3.4 is now written as **E8.1's tiered sentence plus a
scoped addendum**, so the two do not compete: **E8.1 §4.7 lands the sentence, and
this document's addendum lands with S4** — and the addendum is only *true* once
§2.5's reconciler exists. If S4 lands first, the addendum ships against CapFree
§6's text and is re-based at (c) P5; the charters must name which.

There is **no** coupling between the cure and (a0), (b), or (c) P1: tiers do not
change this class (E8.1 §4.4, adopted) and this class does not change tiers.

### 5.2 Placement under each of E8.1 §6 Q2's three options

**Option (i) — (a0)+(b) now, then (c).**

```
(a0)  ──►  S1 ─► S2 ─► S3 ─► S4      [this document, fully parallel to (b)]
      └─►  (b)i ─► (b)ii ─► (b)iii    [E8.1]
                              └─► (c)P1 ─► (c)P2[+K1] ─► (c)P3 …
```
*The cure runs entirely in parallel with (b).* Nothing in S1-S4 touches the
provider tree, the catalog record, or the migration. **K1** resolves by having
(c) P2 carry the survey (the cure lands first). **K2**: §3.4 lands with S4, and
E8.1 §4.7's tiered rewrite folds it at (c) P5. **This option is the best for the
cure** — it is the earliest completion, and it satisfies E8.1 §7 item 4
("before … (c) P3") with the widest margin.

**Option (ii) — (c) P1's `IdentityEvidenceV1` first as (b)'s vehicle.**

```
(a0) ─► (c)P1 ─► (b)-on-P1 ─► (c)P2[+K1] ─► (c)P3 …
   └──► S1 ─► S2 ─► S3 ─► S4        [parallel; must COMPLETE before (c)P2 opens]
```
Same parallelism, but **K1 becomes a scheduling constraint**: (c) P2 is the
naming rebase, so S1-S4 must be *landed* before P2 opens, or P2 must own the
survey's rebase. The lane's recommendation under (ii): **land S1-S3 before P2
opens** (they are the observer work and the survey primitive) and allow S4 (the
three conversions) to land after P2 if P2 slips — S4 depends on the survey, not
on the names.

**Option (iii) — hold everything for (c).**

```
(a0) ─► (c)P1 ─► (c)P2 ─► [S1..S4 HERE, as (c)P2.5] ─► (c)P3 …
```
The cure becomes a phase of (c), written against the rebased names from the
start. **K1 vanishes** (one naming pass). **The cost is that the shipped L5/L6/L7
exposure (§4.7) and the driven L2 wall stay open for the whole of (c) P1-P2** —
and E8.1 §4.4's own note 1 says tiering *widens the population that reaches R3*.
The lane's reading: **(iii) is the worst option for this class specifically**;
if the operator takes (iii) for E8.1's reasons, S1-S3 should still be pulled
forward as a standalone, since they are pure observer work with no naming
dependency at all until (c) P2.

**Under all three options:** §4.5 (the `gc_archived` deletion), §4.6 (the `:279`
cell), §4.8, §4.9, §4.10 and §4.11 are independent of both programs and can run
whenever a builder is free.

### 5.3 What each step amends

| Step | Amends | Tier |
| --- | --- | --- |
| S1 | **Nothing operative.** Corrects the §1.4 mechanism cite at four record homes (`E45B-Report.md:35`, `E4-Close.md:44`, `DR1-Charter.md:117`, E8.1 §4.4) and one in-tree comment (`finalization/execute.rs:71-78`), plus the `:45/:48/:51` → `:79/:88/:98` cite drift at every home. | Dated brackets; **no frozen text**, so lane-owner folds, no dual |
| S2 | Nothing — a new `pub(super)` primitive inside `checked_artifact`. **If it needs an `entry.rs` door, it moves `ENTRY_REFERENCES`/`ENTRY_ITEMS` + the `entry.rs` byte pin ⇒ amendment-tier.** S2 is chartered to avoid that (§7). | none, by design |
| S3 | **CapFree §7's E4.5/6-B disposition** (the `:277` residual retires); **CapFree §6**'s abort-sentence residual narrows (§3.4). | Amendment, own dual |
| S4 | **CapFree §7** (`:278`/`:279` residuals retire); **CapFree §3** row `:279` cell 2 (§4.6, frozen text); **the abort sentence at ten homes** (§3.4); **R2-D Interface Freeze §6 Track-W: NOT touched** — no wire format changes (the survey reads an existing durable record; `CheckedArtifactAuthority`'s `MAGIC`/`VERSION` and `encode` are unchanged). | Amendment + frozen-cell dual |
| S5 (if taken) | **RR §2** (the record-root exception converts) + **RR §6** + the O13 permanent row + the P-2 tripwire. | Amendment, own dual, highest risk |
| §4.1 | Interface Freeze **§8 `:239-240` / §9 `:264-266`**. | Frozen text, own dual |
| §4.5 | **O13 shrinkage arm** + the two KEPT allowances. | Amendment, own dual |
| §4.6 | **CapFree §3** row `:279` cell 2. | Frozen text, own dual (may merge with S4's) |
| **E0.2 §5.2's capability-free list** | **NEVER — no step amends it.** Ruling (A) is closed. | — |

---

## 6. Open questions for the operator — one line each

1. **The record root:** re-affirm the RR §2 exception with S5 named-and-declined (the lane's recommendation), or charter S5 = discovery reconciliation (no probe) **plus a lease split of `commit`** — noting that `commit` is shared with the plain-lease reverse arms (`service.rs:119-124`), that RR §4 leaves only the split since ruling (A) closed the third route, and that **the reverse arm's rewrite stays raw in every outcome**, so RR §2's exception is re-affirmed for the reverse arm regardless?
2. **The `gc_archived` family:** delete it now (fires O13's shrinkage arm, amendment-tier) — the lane's recommendation — or keep it a second phase?
3. **O14:** amend the freeze to name `CheckedArtifactAuthority` as the landed write authority and retire `authorize_write` (the lane's recommendation), or wire it?
4. **Sequencing:** under E8.1 §6 Q2's option (i)/(ii)/(iii), confirm S1-S4 run **in parallel with (b)** rather than inside (c)?
5. **The abort sentence:** land §3.4's reconciliation rewrite with S4 and let E8.1 §4.7's tiered rewrite fold it at (c) P5 — or hold both for one edit?
6. **Row `:279` cell 2:** amend the cell to "checked workspace artifact action" (the lane's recommendation, no new door), or route the writer to a new Git-directory door?
7. **The L7 shipped exposure:** the lane now answers **"now"** (§4.7, State [P3-7]) — it is a *checked* arm, so E4-Close §2's register (raw arms only) has no row for it; confirm the dated bracket goes at E4-Close §2, or name CapFree §6 instead?
8. **The legacy-area order (§4.4):** confirm OPEN-R1's retire-the-area answer is **NO for the duration of R2-E** — on the corrected ground that the abort already *reads* `.gwz/checked-artifacts` and, after §2.5, will *write* it for the first time?
9. **Tier-2:** CapFree §5 and charter §4 deliverable 3 assign the minting to **this round**; confirm the re-deferral until §4.5's delete/rebuild decision (ruling point 4 permits it in one line), so the dated bracket can be written at both homes?
10. **The §1.4 correction:** may S1 land the corrected mechanism at the four record homes *before* it is driven, or must the drive come first (the lane recommends drive-first, per the three-strikes lesson)?
11. **The bundle's residue-orphaning exposure (§4.7, newly named):** charter the ~120-LOC `cleanup.rs` sweep now, or date it as a residual behind S1-S4?
12. **Converting the marker after the cure lands** *(added at State [P2-1])*: ruling (a)'s letter is unqualified — *"Do not convert it"* — and CapFree §7's ruled bracket (`:565-568`) says none of the three arms converts, while the charter's forward-arm line (`:76-77`) promises conversion "only with the reader-side reconciliation" for `:48`/`:51` only; may `execute.rs:79` (the marker, `:277`) convert in S4 once S3 has landed and removed the ground the residual was dated on — and does the same word extend to `:88`/`:98` (`:278`/`:279`)?

---

## 7. The build plan

Phases are milestones; steps are goals with an aspirational **< 500 LOC** budget.
Foundational-first; the parallel column says what may run concurrently.

### Phase R1 — the ground (foundational; nothing else may start until S1 lands)

| Step | Goal | Size | Parallel with |
| --- | --- | --- | --- |
| **S1** | **Drive and re-record the mechanism.** *(Both axes: S1 is GO now and is unaffected by [P1-1] — indeed its RED-1 drive is what would have exposed [P1-1] empirically.)* Apply the preserved patch out of tree, reproduce RED-1, and instrument which frame refuses (`inspect_family` `foreign` vs `classification.rs:175-177`). Land NEW-1 as a permanent pin on the corrected mechanism. Correct the cite at four record homes + one in-tree comment; correct the `:45/:48/:51` → `:79/:88/:98` drift everywhere. **No production behaviour change.** | ~150 LOC (≈40 test, the rest record text) | §4.5, §4.8, §4.9, §4.10, §4.11 |

### Phase R2 — the primitive and the vocabulary (foundational for R3; S2 ∥ S2b)

| Step | Goal | Size | Parallel with |
| --- | --- | --- | --- |
| **S2** | **The direction-free family survey.** `residue.rs::survey_family` + `FamilySurvey`/`SurveyedAction`; the counterpart predicate on `CheckedArtifactAuthority` (I3). **Must distinguish a tier-degraded family from a foreign one** (State [P2-3]; E8.1 §4.4's rule), or (c) P3 re-litigates it. Read-only: no `entry.rs` door (I9). NEW-2. | ~350 LOC | S2b |
| **S2b** | **The reconciler + the classifier widening.** `reconcile_counterpart` (§2.5): survey → restore the detached `.source` → retire the counterpart family through `cleanup.rs`'s existing primitives (`:87/:89`, `:132/:134`, `:222/:224`), all-or-nothing, on the caller's already-held lease. Plus `expected: &[u8] → Option<&[u8]>` on `entry.rs:43-51,:53-63,:65-71,:73-82` and the four `artifact_facts` wrappers, at the **nine** moving call sites of §2.3. NEW-2b: the foreign-family retirement, and its all-or-nothing property. **Stop-trigger: a new `ExactTransition` variant** (see above). | **~450 LOC** *(was 250; the reconciler is the [P1-1] addition)*; **~700 if the stop-trigger fires** and the `classify_table` rows + 232-line pin move | S2 |

### Phase R3 — the observers (the cure lands; S3 ∥ S4 after both R2 steps)

| Step | Goal | Size | Parallel with |
| --- | --- | --- | --- |
| **S3** | **The abort-side reconciliation — now a PRE-PASS, not a widened classifier.** `file_states` (`:269-322`) calls `reconcile_counterpart` **once, up front**, before computing any arm; after it returns the leaf is at an endpoint and `classify_file` (`:343-361`) and `transition_file` (`:325-332`) are correct **unchanged** — which is what resolves the `pending == None` cells that `observe` could never have resolved (State [P2-4](i)). Same pre-pass at **`abort/preflight.rs:110-118`** *(added at Code [P2-2] — it refuses a detached L5/L6 before `root/abort.rs` is reached, so S3 cannot close L5/L6 without it)* and at `root/abort.rs:123-145`. **The direction correction of §2.5 lands here: absent-leaf-never-published ⇒ `After` ⇒ `FS::Baseline`.** L11 (`observe/finalization.rs:439-458`) needs **no edit** — only the guarantee that the pre-pass precedes it. **RED-1 turns GREEN here.** NEW-3. **No new acquisition (I6).** | **~450 LOC** *(was 400)* | S4 |
| **S4** | **The forward-side normalisation + the three conversions.** `publication/live.rs::snapshot` surveys before the `:110-112` bail and before `CandidatePublicationObservation::new` (`:123`); `acceptance/publication.rs` **untouched** (I8). Then convert `execute.rs:79/:88/:98` onto the boundary, expected `Missing`/`Bytes`/`Bytes`-or-`Missing` (prep §2.3's empty-baseline branch). Retire the three residual sentences. NEW-4, NEW-5, NEW-6. **RR §4 enumeration: `acquire_activated` and nothing else (I7).** | ~450 LOC | S3 |

### Phase R4 — the record acts (after R3; all parallel)

| Step | Goal | Size | Parallel with |
| --- | --- | --- | --- |
| **S4a** | The abort sentence at ten homes (§3.4) + CapFree §6's residual narrowing. | ~120 LOC of text | S4b, S4c |
| **S4b** | Row `:279` cell 2 (§4.6) — frozen text, own dual. **Must land with or before S4.** | ~1 cell + bracket | S4a, S4c |
| **S4c** | The record-root re-examination bracket in RR §2 (§4.3), recording re-affirm-or-convert per the operator's Q1. | ~15 lines | S4a, S4b |

### Phase R5 — conditional and independent (any time; no ordering against R1-R4)

| Step | Goal | Size | Depends on |
| --- | --- | --- | --- |
| **S5** | *Conditional on Q1.* The record-root reconciliation, **in two halves**: (a) discovery — `classify_open_record` (`store/mod.rs:216`) + `discover_open` (`:82`) survey `.gwz/checked-artifacts` for a family whose `canonical_path_identity` resolves under `.gwz/merge`; **(b) a LEASE SPLIT of `commit`** (`service.rs:119-124`), without which converting the forward rewrite arm puts every standard-path v1 abort on the probe (RR §1b). **Amends: RR §2 (the exception clause), RR §3 P-1 (shrinkage arm) and P-2 (the tripwire asserting the door absent), RR §6, the O13 permanent row, AND RR §4's plain-lease clause for the split.** Two stop-triggers: *discovery takes no probe*; *no reverse arm moves onto the boundary*. RED-2 turns GREEN. | **~200 LOC for (a) alone — figure does NOT hold once (b) is included; UNVERIFIED** | S2, Q1 |
| **I1** | *Independent.* §4.5 — delete the `gc_archived` family (**12** fns + 2 structs + 4 members), with the O13 shrinkage amendment. *(Aligned to §4.5's measured figures at Code [P3-4]; REVISION 1's "11 / ~450" was internally inconsistent.)* | **≈350 removed / ~60 added** | Q2 |
| **I2** | *Independent.* §4.1 — the O14 freeze amendment + `authorize_write` retirement. | ~40 LOC + amendment | Q3 |
| **I3** | *Independent.* §4.8 (both allowances — note the eight test call sites), §4.9 (`CatalogOwnerV1`, at the **corrected** span `catalog.rs:51-63`), §4.10 (the checker row — **after** I1's decision), §4.11 (`authority_record.rs:50-53`), and the `checked_bundle.rs:1` "Test-gated" doc falsehood (§4.7). | ~160 LOC total | I1 for §4.10 only |
| **I4** | *Independent, newly named (§4.7).* The bundle's **residue-orphaning** sweep: `cleanup.rs` retires a family whose authority describes a leaf that matches neither endpoint. **Trigger site, now stated** *(State [P3-6])*: **inside the v1 reverse arm's existing door** — `v1_write_bundle_checked`'s `finish()` path (`checked_bundle.rs:64-112`) — which is on the plain lease but on an **already-probing** path (CapFree §6 path (ii)), so it adds no capability. **It must NEVER be triggered from `gwz stash` or the v0 preserve arm** (`handle_stash/commands.rs:68,:86,:93,:122,:318,:327`; `preserve/artifacts.rs:357`) — those are carved and may take no probe. **Not a conversion.** Note it does **not** subsume [P1-1]'s retirement: its predicate ("matches neither endpoint") does not fire on a counterpart residue, whose absent leaf *equals* the forward action's `Missing` endpoint (Code [P1-1]). | ~120 LOC | Q11 |

**Critical path:** S1 → (S2 ∥ S2b) → (S3 ∥ S4) → R4. **Four sequential
milestones, six steps, none over 450 LOC** *(S2b carries a named contingency to
~700 if its stop-trigger fires)*. Everything in R5 is off the path.

### The sizing cross-check, and the one thing that could blow it

An independent measurement of the same cure, sized by summing the function bodies
a widened observer vocabulary would touch, gives: forward observers 122 LOC
(`live.rs::snapshot` `:94-150`, `resolve_candidate` `:57-92`, `regular_digest`
`:166-187`, `classify_index_form` `:152-158`) + `abort/evidence.rs` 206 LOC
(`file_states` `:269-323`, `classify_file` `:343-361`, `transition_file`
`:325-332`, the `EvidenceFileStates` types `:174-187` and impl `:189-265`,
`classify_v1_evidence_rollback` `:91-123`) + `artifact_facts.rs` 99 (the whole
module) + `classification.rs` 107 + `residue.rs` ~209 + `authority.rs` 63 ≈
**806 LOC across 6 files**, plus 23 external call sites and 10 downstream
`publication.rs` sites (`:53,:88,:100,:123,:247,:256,:262,:272,:366,:414`). That
fits inside S2+S2b+S3+S4's 1,450-LOC budget with room for the test rows.

**The one thing that could blow it, and the design decision that avoids it:**
adding a third `FileState` variant (e.g. `Detached`) would force a re-audit of
**29 shape literals** *(corrected from 35 at Code [P3-3]: `initial_publication` 9,
`boundary_before` 4, `boundary_after` 5, `lock_before` 2, `lock_after` 3,
`marker_before` 2, `marker_after` 2, `index_before` 1, `index_after` 1)* across
the 11 predicates at `abort/evidence.rs:189-265` — the argument is unchanged by
the count. And an `ExactTransition` variant would drag `classify_table`'s
**232-line pinning test** (`classification.rs:302-533`, confirmed by both axes).

**§2.5's pre-pass design exists precisely to avoid both**, so the stop-triggers
are now two, and the second is the one that matters *(Code [P1-1], condition 1)*:

* **S3:** *"if the cure cannot be expressed without a new `FileState` variant,
  stop and report."* — the reconciler converges the leaf to an endpoint before
  any `FileState` is computed, so the existing three-variant vocabulary suffices.
* **S2b:** *"if `reconcile_counterpart` cannot converge without expressing an
  intermediate state to `classify_table`, stop and report"* — the fallback is
  Code [P1-1]'s option (a): a new `ExactTransition` variant, new `classify_table`
  rows, and a re-pin of the 232-line test. **That contingency is budgeted below,
  not assumed away.**

---

## Verification notes

**Re-read at `ffd4f95` this session** (every line quoted or cited above from
these files was opened): `checked_artifact/entry.rs:36-160`;
`checked_artifact/residue.rs:20-50`, `:100-232`;
`checked_artifact/classification.rs:103-200`;
`checked_artifact/authority.rs:95-145`, `:190-240`;
`checked_artifact/policy.rs:28-50`;
`workspace_ops/merge/root/artifact_facts.rs:1-100`;
`workspace_ops/merge/abort/evidence.rs:260-400` (with `file_states` at `:269-322`,
`transition_file` `:325-332`, `classify_file` `:343-361`, `preflight_v1_evidence`
`:34`, `classify_v1_evidence_rollback` `:91`, `evidence_shape_is_exact` `:390`);
`workspace_ops/merge/v1_lifecycle/authority/observe/finalization/publication/live.rs:55-135`,
`:160-195`; `workspace_ops/merge/v1_lifecycle/finalization/execute.rs:25-115`;
`workspace_ops/merge/preserve/checked_bundle.rs:70-110`;
`git/gitbackend/preservation_root/files.rs:25-60`;
`git/gitbackend/preservation_root.rs:195-215`, `:465-495`;
`workspace_ops/merge/acceptance/publication.rs` (heading map only, `:43`, `:53`,
`:71`, `:81`, `:90`, `:95`, `:99`); `docs/OperationModel.md:120-165`. Production
call-site enumerations for the `entry.rs` write doors and for `artifact_facts::*`
were taken by exhaustive grep over `gwz-core/src`, tests excluded.

**Measured by a second, peer-blind pass over the same tree at `ffd4f95`** (results
folded above, and the §1.4 correction reached independently by both):
`transition.rs:38-116`, `:91-106`, `:118-177`, `:179-327`, `:290-301`, `:316-317`,
`:329-426`, `:372`, `:391-401`; the complete 16-door `entry.rs` inventory;
production call sites for every door; `authority.rs:196-223`, `:226-240`,
`:255-258`, `:271-273`; `residue.rs` write inventory (12 sites); the
`gc_archived` family (12 fns + 2 structs + 4 members, with `cleanup_error`
newly named); `authorize_write`/`RetainedWriteAuthorityV1` consumer counts;
`load_archived` caller counts; the `CatalogOwnerV1` use-site sweep; the checker's
inventory row and its digest code (`py:510-539`, `:1470-1513`); the preservation
bundle's three writer families; `policy.rs:41-45` and its construction sites.

**The dual, folded.** Both axes GO-WITH-CONDITIONS, both peer-blind, both read at
`d6830cd`. **Code axis:** 31 verified, 4 refuted, 2 UNVERIFIED; conditions = the
[P1-1] writer/direction fix, `abort/preflight.rs:110-118` into the matrix and S3,
and the three P3 counts. **State axis:** conditions = [P1-1] the third component,
[P1-2] Q1's lease split, [P2-1] the marker-conversion question, plus text folds.
**Every condition is folded above**; the two carried by the reviewers as their own
UNVERIFIED are inherited below (items 7-8). Both axes independently confirmed
§1.4's load-bearing correction line by line, and both confirmed the three record
corrections (`gc_archived` = 12 with `cleanup_error`; the hazard at L7 not L8;
`catalog.rs:51-63`).

**UNVERIFIED — genuinely open:**
1. **The §1.4 correction is measured by reading, twice — not by driving RED-1.**
   S1 exists to drive it. **It should not be folded into any frozen text until S1
   does.** This is the single most important caveat in the document, because four
   record homes and one in-tree comment currently say something else.
2. All LOC figures are estimates from function spans, not from a spike. The
   independent 806-LOC cross-check (§7) agrees with the step budgets but was
   computed the same way.
3. `merge/store/mod.rs:211-241` (`classify_open_record`) and `discover_open` —
   cited via RR §1a for §2.4(C)/S5; `discover_open` was read at
   `store/mod.rs:82-97` but the S5 sizing is an estimate.
4. The claim that (C) cannot revive O8's archive route (§4.5) is an argument from
   the ruling's structure, not a measurement — it turns on ruling (A) being
   closed, which is a record fact, not a code fact.
5. Whether L7's directional hazard is *reachable* in a real preservation
   transition sequence (as opposed to structurally present) is **not driven**.
   §4.7 states the structure; NEW-5 is what would prove reachability.
6. E8.1's own UNVERIFIED list is inherited wherever this document leans on §4.4
   or §7.
7. *(Code axis, inherited)* Whether `publication/live.rs::snapshot` is reachable
   **only** under `acquire_activated` — the design's chain (`finalization.rs:53`
   → `forward.rs:69` → `start.rs:89`/`:160`) was not re-traced inside the review's
   box. `snapshot` has 14 callers, all inside
   `observe/finalization/publication.rs`. **The fallback holds regardless**: a new
   acquisition in `snapshot` is the *legacy* probe the abort already takes, not
   catalog activation, so the ruling's red line is uncrossed under either lease —
   but I7 should be re-driven at S4's charter rather than assumed.
8. *(State axis, inherited)* L7's directional hazard is **structurally** present;
   whether it is **reachable** in a real preservation transition sequence is not
   driven. NEW-5 is what would prove it.
9. `load_archived`'s trait-surface freeze status outside
   `GwzM5-8R2DInterfaceFreeze.md` (State [P3-5]).
10. S5's sizing once the `commit` lease split is included (State [P1-2]) — the
    ~200-LOC figure covers discovery only.

**Base moved under this document, mid-session — recorded, not chased.** This
document is written against `ffd4f95` as chartered. While it was being written,
gwz-core main advanced two commits: `e15b3a4` (**DR-1 (a0)**: the two gratuitous
ext4 gates removed, the one admission gate made visible) and `d6830cd` (the
aggregate-gate remainder pin, darwin 1119 / linux 1120). `git diff --stat
ffd4f95..d6830cd` touches **four files only**: `docs/OperationModel.md`,
`scripts/checks/check_checked_artifact_boundaries.py` (1 line substituted, no
shift), `scripts/checks/run_r4bg_aggregate_gates.py`, and
`capability/pre_catalog/provider/platform/linux.rs`. **No file this document
takes a mechanism cite from was touched** — `residue.rs`, `classification.rs`,
`authority.rs`, `transition.rs`, `entry.rs`, `artifact_facts.rs`,
`abort/evidence.rs`, `live.rs`, `execute.rs`, `preservation_root*`,
`checked_bundle.rs`, `archive.rs`, `gc.rs`, `policy.rs`, `catalog.rs`,
`execution.rs` are all byte-identical at `d6830cd`. Two **presentational** cites
drift and are re-pointed here:

* `provider/platform/linux.rs:136-151` (`require_ext4`, §3.4's sentence) → at
  `d6830cd` the fn is at **`:142`**, its surviving call at **`:132`**. (a0) did
  exactly what E8.1 §3.0 said it would.
* `docs/OperationModel.md:147-149` / `:149-155` (the *What never refuses*
  paragraph) → at `d6830cd` it begins at **`:151`**; `:131-132` is unmoved.
* `check_checked_artifact_boundaries.py:527` — **unmoved, and still stale**: the
  row still reads `` `:108-111` ``. §4.10 stands verbatim at HEAD.

Every other cite in this document is valid at `d6830cd` as written.

**Not run:** any test suite, `cargo check`, `cargo clippy`, `cargo doc`, or the
boundary checker. **No file in `gwz-core/` was read-locked, modified, staged or
committed; no branch was checked out; no tag was touched; nothing was pushed.**
The only file this session wrote is this document. *(Note for the lane owner: at
the time of writing, `dev-docs/GwxMergeFindingsDraft.md` and
`dev-docs/GwzAiReqDd.md` show as modified in `gwz-dev`. **Not this session's** —
they were untouched here and are left exactly as found.)*
