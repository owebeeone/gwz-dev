# GwzM5-8 R2-E Capability-Free Amendment — Code axis review (round 1)

**Object.** `GwzM5-8R2E-CapabilityFreeAmendment.md` (DRAFT, 2026-09-02), the
amendment executing the operator ruling at
`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`. This axis verifies the TREE
FACTS the amendment rests on, not the ruling.

**Reviewer.** Fable, Code axis (MECHANISM TRUTH), peer-blind to the State axis.
**Date.** 2026-09-02.

**Method.** Read-only. Every cite in the amendment's §1 table, §1 last
paragraph, §3, §6 and §7 opened at its line on gwz-core `main`
`7f289078b5136a9a7128a742d95d53b05e2a4df2` (`git status` clean; branch `main`).
Routing enumerated from the tree with the system `grep`, not inherited from
`GwzM5-8R2E-E4.4-CharterPrep.md`, which was treated as a claim source to
falsify. No build, no `cargo`, no worktree, no probe volume, no commits, no
file in either repo modified other than this report. Disk untouched.

**Authorities consulted.** The ruling; the prep (§0–§7); the
`SemanticsAmendment-DRAFT.md` §5.2 decision `:909-960`;
`RecordRootAmendment.md` (adopted) §1b/§2/§3/§4/§6/§7;
`R4bR2ConsumerCheckpoint.md` §10 `:272-280`; `E4.1-Review.md` §R2.3
`:690-713`, §R2.5 R2 row, `[P3-C1]` `:1021`;
`scripts/checks/check_checked_artifact_boundaries.py` (`:40-74`, `:228-236`,
`:308`, `:345-350`, `:942-949`, `:952-985`, `:1187-1214`); `docs/OperationModel.md`
`:125-159`; `capability.rs:38-55`; `interface_tests/contracts.rs:149-209`.

---

## 1. VERDICT

# NO-GO (round 1)

**2 P1, 5 P2, 7 P3.** Every finding is curable by TEXT in round 2 — none needs
a build, a probe volume, or a different scoping; the ruling's direction (carve
listed-operation writers, convert only `acquire_activated` arms, scope the abort
sentence from the tree) is exactly what the tree supports. What the tree does
NOT support is which arms the amendment assigns to which class, and the abort
sentence the amendment proposes to ship at three homes. Nothing here returns
to the operator on this axis's account.

The two P1s, in one line each:

- **[P1-1] OVER-CARVE at `:279`.** The v1 forward publication writes
  `.git/info/exclude` at `v1_lifecycle/finalization/execute.rs:51` and that
  arm is reached ONLY under `acquire_activated`. The amendment carves `:279`
  "ENTIRELY (create / init-from-sources only; no fork)". The ruling's "Convert
  only arms already on `acquire_activated`" names precisely this arm.
- **[P1-2] §6's scoped sentence is FALSE in the tree.** "An abort of a
  `--no-ff` merge that has not yet published its result needs no such
  filesystem" is refuted by two pre-publication v1 abort shapes that construct
  `CheckedArtifact`s on the plain lease today: `--abort --preserve` with any
  integrated participant (the bundle door), and plain `--abort` with `@root`
  among the selected targets (the root-metadata doors).

---

## 2. FINDINGS

### [P1-1] Over-carve: `:279` has an `acquire_activated`-only writer the amendment carves as "no fork"

**Amendment text.** §1 table row `:279`: "SOLELY `gwz repo create` and
`init-from-sources` … NO — no fork exists"; §3: "`:279` — CARVED ENTIRELY
(create / init-from-sources only; no fork)".

**Tree.** `publish_workspace_exclude_candidate` (`sync_workspace_boundary.rs:84-86`)
is `crate::artifact::write_atomic(&workspace_exclude_path(root), contents)` on
`<root>/.git/info/exclude` (`:88-90`). Its production callers, enumerated
(`grep -rn "publish_workspace_exclude_candidate(\|ensure_workspace_exclude(\|sync_workspace_boundary(" src`,
non-test, non-definition):

| Site | Operation | Lock / lease |
| --- | --- | --- |
| `v1_lifecycle/finalization/execute.rs:51` (`PublicationPhysicalAction::WriteBoundary`) | **checked v1 forward publication** | **`acquire_activated` ONLY** — see chain below |
| `merge/finalize.rs:251` (`publish_candidate`) | ordinary v0 merge publication | v0 lane's `WorkspaceMutatorLock` (`continue_op/coordinator.rs:13`, or the start guard `dispatch.rs:270`) |
| `merge/abort/evidence.rs:683` (`rollback_evidence`) | v0 abort, evidence rollback | `abort/mod.rs:44` |
| `merge/preserve/artifacts.rs:233` (`restore_root_publication`) | v0 `--abort --preserve` | `abort/mod.rs:44` via `:46` |
| `handle_create_repo.rs:51` (`handle_create_workspace`, fn at `:19`) | workspace create | bare `WorkspaceMutatorLock::acquire` (`:35`) |
| `handle_create_repo.rs:211` (`handle_create_repo`, fn at `:68`) | `gwz repo create` | `guarded_workspace_root(RepoMutate)` (`:90`) — the mutation guard |
| `handle_create_repo.rs:336,426` (`handle_add_existing_repo`, fn at `:232`) | repo add-existing | `guarded_workspace_root(RepoMutate)` (`:244`) |
| `handle_init_from_sources.rs:203` | `init-from-sources` | bare `WorkspaceMutatorLock::acquire` (`:91`) |
| `handle_commit.rs:235,240` | `gwz commit` | mutation guard (`:37`) |
| `handle_materialize.rs:128,404,456` | `gwz materialize` | mutation guard (`:27`, `:100`) |
| `handle_stage.rs:97` | `gwz stage` | mutation guard (`:27`) |
| `handle_branch.rs:234` | `gwz branch` | `guarded_workspace_root(BranchMutate)` (`:33`) |
| `handle_repo_lifecycle.rs:153,229,342` | repo lifecycle | `guarded_workspace_root(RepoMutate)` (`:49`, `:192`) |
| `pull_head_member_preflight.rs:186,492` | pull | `guarded_workspace_root(Pull)` (`:45`) |

**The activated-only chain, driven from the tree.** `finalization/execute.rs:8-64`
`publication()` is called only from `finalization.rs:53` inside
`FinalizationRuntime::execute` (`:35-55`), which is constructed only at
`forward.rs:69` inside `ForwardRuntime::execute`'s
`PhysicalActionKind::Publication` arm (`:68-70`). `ForwardRuntime::new` has
exactly two production sites: `start.rs:89` (before
`service::run(…, ResumeStart, …)` at `:90-96`) and `start.rs:160` (before
`service::run(…, Continue, …)` at `:161`). `service.rs:116-121` takes
`acquire_activated` for exactly `ResumeStart | Continue`. The forward runtime
refuses every reverse/archive action (`forward.rs:71-76`), and the reverse
runtime never carries a `Publication` action (only `ForwardRuntime` matches
it). So `execute.rs:51` — and its siblings `:45` (marker) and `:48` (lock) —
are reached under `acquire_activated` and under nothing else. `forward/execute.rs`
(participant actions) names no raw writer at all (grep: none).

**Consequence.** "No fork exists" and "SOLELY" are both refuted. The
amendment's own exception text ("What remains convertible is the set of
writers reached ONLY under `acquire_activated`") and the ruling's point 2
place `execute.rs:51` in the convertible set; §3 places it in the carved set.
That is the E4.3 contradiction in the opposite direction — a conversion the
ruling orders, foreclosed by the amendment's row text. Cure: `:279` becomes
"partial" like `:277`/`:278`: the listed-operation writers carve; the v1
publication's `WriteBoundary` arm converts under E4.6-B with its
reachability proof (this chain), noting that `entry.rs` has only the
git-directory OBSERVE door (`:92-98` → `:186-192`) and no replace door.

### [P1-2] §6's scoped sentence is false for two pre-publication v1 abort shapes

**Amendment text.** §1 last paragraph and §6: "pre-publication aborts construct
no `CheckedArtifact` (`abort/evidence.rs:39-41`; `rollback_prefix.rs:76-97,155-184`)";
proposed sentence at three homes: "an abort of a `--no-ff` merge that has not
yet published its result needs no such filesystem; after publication the abort
re-verifies …".

**The cites are true of the EVIDENCE chain only.** `preflight_v1_evidence`
returns `Ok(())` when `publication` is `None` (`:39-41`) and probes (via
`evidence_shape_is_exact` → `file_states` → `artifact_facts::observe/classify_*`)
only when candidate + composition_commit + `!evidence_rolled_back` (`:42-45`);
`rollback_prefix.rs:76-81` gates `observe_v1_evidence_rollback` on the same
triple plus `evidence_rolled_back`; `:155-184` is git-side. Verified. But the
tree has two OTHER plain-lease abort paths that construct `CheckedArtifact`s
with no publication gate at all:

**(a) `--abort --preserve` — the bundle door, at observation and at execution.**
`handle_v1_command` maps `(Abort, Some(true))` → `V1LifecycleRequest::Preserve`
(`start.rs:154`) → `service.rs:120` plain lease. Preservation entry
(`authority/observe/reverse/preservation/entry.rs:87-110`) calls
`preflight_v1_evidence` (`:87-91`, passes trivially pre-publication), then
`v1_preservation_owners` (`:95`), then for EVERY plan
`v1_bundle_observation` (`:102-104`) → `checked_bundle.rs:19-46` →
`entry::classify_merge_preservation_bundle` (`entry.rs:128-137`) →
`preservation_bundle()` (`:168-175`) → `CheckedArtifact::acquire` →
`durable_identity` (`observation.rs:96`, `:105`) → `UnsupportedOperation`
(`:363-365`, `:376-381`). Execution: `reverse/execute/preservation.rs:74`
`v1_write_bundle_checked` → `checked_bundle.rs:64-112` → `:78` classify,
`:93` `replace_merge_preservation_bundle` (`entry.rs:139-148`), `:99`
classify — three acquisitions. The owner set is non-empty whenever any
selected target has an integrated result (`preserve/plan.rs:407-446`,
`is_integrated_result` at `:415`) — a state every participant reaches during
forward execution, BEFORE the finalization/publication phase. Publication is
consulted only for the `@root` anchor (`:421-426`) and the
`PublicationRoot` owner (`:448-453`). So every v1 `--abort --preserve` with at
least one merged/fast-forwarded participant probes, published or not.

**(b) plain `--abort` with `@root` selected — the root-metadata doors.**
`model/v1/validate/journal.rs:214-262` `rollback_cursor`: publication
evidence first (`:217-222`, only if a candidate + composition commit exist);
then the participants (`:224-252`); then, with every participant terminal,
`SelectedRootMetadata` iff `"@root"` is in `selected_targets` (`:253-258`) —
no publication condition. That cursor is observed at
`authority/observe/reverse/rollback.rs:121-127` →
`root::observe_v1_root_metadata_rollback` (`root/abort.rs:109-145`) →
`artifact_facts::observe(root, WORKSPACE_MANIFEST)` (`:131`) and
`artifact_facts::observe(root, LOCK_PATH)` (`:145`) → `entry::observe_merge_root_artifact`
(`entry.rs:36-41`) → `root_artifact()` (`:159-166`) → `acquire` → probe. Its
execution (`reverse/execute/rollback.rs:69-76` → `root/abort.rs:333-386`)
writes through `artifact_facts::write_checked` at `:380` after re-observing
at `:339`; `rollback_prefix.rs:99-113` observes it again whenever the `@root`
participant's own rollback is complete; `journal.rs:104-121` mints the pending
`SelectedRootMetadata { Manifest }` step. Driven rows exist
(`v1_lifecycle/tests/reverse_rollback/root_artifacts.rs`,
`workspace_ops/tests/g23/root_abort.rs`).

**Consequence.** The amendment's "pre-publication aborts construct no
`CheckedArtifact`" is a universal that the tree refutes twice; the proposed
sentence would ship a NEW over-claim at `capability.rs:52-55`,
`docs/OperationModel.md` and the release-notes line. Worse, the shipped
`docs/OperationModel.md:147-148` — "**What never refuses:** `gwz merge
--abort`, with or without `--preserve`, on a record of either version" — is
already false today for (a), (b) and the post-publication chain, and §6 does
not name that sentence's `--preserve` clause as a home to scope.
`RecordRootAmendment` §1b carried the same universal; it was adopted on the
evidence chain's cites and this amendment inherits it — the inheritance is
where the error entered, and this amendment is the object that would make it
user-facing. The honest scoping is by PATH, not by publication state: a v1
abort is capability-free exactly when it takes no checked door — no
`--preserve` with an integrated participant, no selected `@root`, and no
published evidence — and the sentence must say so or say less (e.g. "an abort
that touches no checked artifact needs no such filesystem; aborts that must
re-verify checked artifacts — preservation bundles, a selected root's
manifest/lock, or published evidence — need persistent file handles").

### [P2-1] "needs the same filesystem the merge needed" is not mechanism-exact; "the merge's own checked evidence" is a misnomer

Two identity modules, two admissions. The merge's `acquire_activated`
(`checked.rs:158-164`) activates the catalog, whose provider
(`capability/pre_catalog/provider/platform/linux.rs`) enforces
`require_ext4` (`:136-151`, a filesystem-magic check), `FS_IOC_GETFSUUID`
(`:153-171`) and `name_to_handle_at` (`:173-203`). The abort's doors go through
`CheckedArtifact::acquire` → `identity::object_identity` (`identity.rs:154-156`)
→ the LEGACY Linux probe (`identity.rs:312-346`: `name_to_handle_at` with a
non-empty handle, `fstatfs` `f_fsid` `:279-290`) and `rename_domain`
(`:356-367`, `statx` `MNT_ID`) — NO ext4 magic, NO uuid ioctl (grep `ext4`
across `src/`: only the provider's profile names and one comment). On btrfs,
xfs or zfs the abort's probe SUCCEEDS where the merge's `require_ext4`
REFUSES; the two coincide only on substrates that lack `name_to_handle_at`
(the FAT32 class E4.1's PR-3 drove). So the post-publication abort needs a
STRICTLY WEAKER filesystem than the merge, and the amendment's own "Linux
non-ext4 drive optional" would, if ever exercised, drive the sentence false.
Separately, the evidence the abort re-verifies was written RAW by the forward
path (`finalization/execute.rs:45,48,51`, `write_atomic`) — "the merge's own
checked evidence" should read "the merge's published evidence, re-verified
through the checked boundary". Cure: wording — "needs persistent file handles
and a mount identity" (the remedy's own vocabulary, `capability.rs:52-55`),
not "the same filesystem the merge needed".

### [P2-2] `:275`'s GC writer is a dead arm; the live GC deletion writer is elsewhere and the decode fix has a second site

**Amendment text.** Row `:275`: "`remove_archive` via GC — under NO v1 lease
(`archive.rs:207`, `WorkspaceMutatorLock` only)"; §3: "`archive.rs::remove_archive`
(site A, `sync_dir` ×2): CARVED".

**Tree.** `remove_archive` (`archive.rs:191-208`) is called only from
`gc_archived_with_hook:148`, which is called only from `gc_archived:117`,
which carries `#[allow(dead_code, reason = "… this family has no production
caller until that route lands")]` (`:108-111`); its callers are
`tests/gc.rs` only. The lease fact is verified (`:126`,
`WorkspaceMutatorLock::acquire`, no `V1MutationLease`) — but "via GC" is not a
production reach. Production `gwz merge --gc` is `gc::handle_gc`
(`dispatch.rs:361` → `gc.rs:160-205`), which deletes through
`store.gc(root, Some(merge_id))` (`:203`) → `FileMergeStore::gc`
(`store/mod.rs:156-157`) → `store/gc.rs::collect` (`:3-18`):
`fs::remove_file(&path)` at `:16` and `sync_dir(DONE_DIR)` at `:17`; id-less
GC deletes through `store/retention.rs:36-41` (`fs::remove_file` `:37`,
`sync_dir` `:41`). Neither file is named by the amendment's carved set, the
O13 map (`:345-349`, `v1_lifecycle/` only), `PROTECTED_SOURCE_DIGESTS` or
`PROTECTED_SOURCE_TREE_DIGESTS`. The inventory that "covers every carved
site" for row `:275`'s GC writer therefore pins a dead arm and misses the live
one — and the live one is the writer that will delete v1 archives once the GC
decode fix lands.

**The decode fix has two sites, not one.** §7: "`gc.rs:184-192` decodes
archived records with the v0 decoder … the fix is read-side". `gc.rs:188-190`
`decode_production_v0` → `archived_record_unreadable` — verified, and a v1
envelope fails it: `decode.rs:119-121` classifies the header against
`InstalledMergeRecordVersions::V0_ONLY` → `header.rs:159-163`
`Some(V1) if installed.v1` does not match → `Err(unsupported(…))`
(`HeaderClassificationError::Unsupported`) → `RecordDecodeError::Header`. (The
`decode.rs:128-133` "the v0 decoder received a v1 record" arm the prep cites is
the UNREACHABLE typed twin, per its own comment `:123-127`.) But after that
site is fixed, `store.gc(root, Some(id))` at `gc.rs:203` → `store/gc.rs:15`
`read_record(&path, RecordLocation::Archived)` → `store/mod.rs:394`
`decode_production_v0(&bytes)` fails the same v1 envelope, so the archive
still cannot be deleted; `retention.rs:8` is a third site on the id-less
path (it falls to `validated_future_cleanup` on `Err`, whose v1 behaviour the
GC brief must check). "Adds no probe" — VERIFIED: `gc.rs`, `store/gc.rs`,
`store/retention.rs` name no `CheckedArtifact`/`entry::`/`artifact_facts`/
`durable_identity` (grep: none); `handle_gc` holds only
`WorkspaceMutatorLock::acquire` (`:167`). "Keeps GC capability-free" holds as
long as the fix routes deletion through `store/gc.rs`, not through the
dead `gc_archived` family (which is `remove_archive`'s only route and
carries no probe either).

### [P2-3] The listed-operation raw-writer set is under-enumerated, and row `:276` conflates a converted writer with a raw one

For an inventory to be fail-closed "in BOTH directions", the carved set must be
exhaustive. The §1 table's cells are not:

| Row | Listed-operation raw writer NOT in the table | Operation |
| --- | --- | --- |
| `:277` marker | `abort/evidence.rs:696` `fs::remove_file(&marker_path)` (`rollback_evidence`) | v0 abort (`abort/mod.rs:44`) |
| `:277` marker | `preserve/artifacts.rs:226` `write_atomic(&marker_path, …)` (`restore_root_publication`) | v0 `--abort --preserve` |
| `:278` lock | `abort/evidence.rs:689-692` `write_atomic(LOCK_PATH, baseline_lock_yaml)` | v0 abort |
| `:279` boundary | `abort/evidence.rs:683`, `preserve/artifacts.rs:233`, `finalize.rs:251`, `handle_commit.rs:235,240`, and the mutation-guard family in [P1-1]'s table | v0 abort, v0 abort --preserve, ORDINARY merge, `gwz commit`, guard |

And row `:276`:

- "ordinary merge preserve (`preserve/artifacts.rs:353`)" — `:353` is
  `crate::stash::write_bundle` inside `persist_stash_bundle` (`:292-354`),
  whose only caller is `preserve.rs:145` inside `preserve_then_abort`
  (`:64-194`), whose only caller is `abort/mod.rs:46` under `request.preserve
  == Some(true)`. It is the v0 `--abort --preserve` writer; no ordinary
  (non-abort) merge reaches it (the prep's §7.7(3) doubt, now settled: NOT
  reachable from an ordinary merge). Still a listed operation — the carve
  holds — but the label is wrong.
- "`abort --preserve` (`v1_lifecycle/reverse/execute/preservation.rs:74`)" —
  `:74` is `v1_write_bundle_checked`, the CONVERTED writer
  (`checked_bundle.rs:93` `entry::replace_merge_preservation_bundle`). It
  has no "raw `durable_fs` / `std::fs` publication primitive" to retain; the
  exception's operative sentence has no referent at that cite. §3 then says
  the same path "is §6's already-shipped narrowing" — but §6 covers only the
  post-publication evidence chain ([P1-2]). The row must separate the v0
  raw writer (carved) from the v1 converted writer (already-probing, dated
  under a §6 that names it).

The `gwz stash` writer (`stash/mod.rs:260-262` → `write_atomic`) is a further
raw writer of the `:276` leaf under the mutation guard (`handle_stash.rs:43`),
carved only under the guard's broad reading; the amendment lists `gwz commit`
by name (the ruling names commit) but leaves materialize/stage/branch/pull/
repo-lifecycle/stash to "the mutation guard" — the inventory must name their
sites explicitly or the negative direction is unsound.

### [P2-4] The per-file negative scan is unsound for mixed files; the map generalization needs three named changes

**Existing shape, verified** (`check_checked_artifact_boundaries.py:1187-1214`):
scan `production_rust_files(source / V1_LIFECYCLE_TREE)` (`:1188`;
`V1_LIFECYCLE_TREE = "workspace_ops/merge/v1_lifecycle"` `:308`;
`production_rust_files` `:942-949` excludes `tests`/`interface_tests`
parts and `tests*` names); populate a file only if masked source matches
`\bdurable_fs\b` (`:1191`); count bare identifiers of the three tokens
(`:350`, `:1194-1195`) on `mask_non_code` output (`:952-985`, comments and
string contents blanked — `use` lines count); three findings: gained file
(`:1198-1202`), lost file (`:1203-1207`), count moved (`:1208-1214`). Count
per primitive per file, both directions — as the amendment says.

**Generalization is implementable in that shape, but only with:** (i) a scan
root beyond `v1_lifecycle/` — an explicit carved-file list under `src/`; (ii)
dropping the `\bdurable_fs\b` population gate — `handle_commit.rs`,
`finalize.rs`, `abort/preflight.rs`, `abort/evidence.rs`, `preserve/artifacts.rs`,
`sync_workspace_boundary.rs`, `handle_create_repo.rs`,
`handle_init_from_sources.rs`, `store/gc.rs`, `store/retention.rs` never name
it; (iii) a wider token vocabulary. Every `std::fs`-direct and seam spelling
at the carved sites IS countable by the bare-identifier idiom
(`\bcreate_dir_all\b` matches `fs::create_dir_all`; likewise `remove_file`,
`write_atomic`, `write_marker`, `write_lock`, `write_manifest_and_lock`,
`write_bundle`, `publish_workspace_exclude_candidate`, `sync_workspace_boundary`,
`ensure_workspace_exclude`, `rename_noreplace`, `sync_dir`). The amendment's
"enforced by the same map" is true only after (i)–(iii), which it should name.

**Per-file negative scan.** Door census on the candidate carved files
(`grep -c "entry::\|artifact_facts\|checked_artifact::\|_checked("`):

| File | door namers | raw writers |
| --- | --- | --- |
| `abort/evidence.rs` | **27** (`artifact_facts::observe/classify_write/classify_remove/write_checked/remove_exact` at `:82-83`, `:148-164`, `:277-315`) | 3 (`:683`, `:689`, `:696`) |
| `abort/preflight.rs` | **3** (`use … artifact_facts` `:24`; `artifact_facts::observe` `:111-112`) | 2 (`:409-410`) |
| `finalize.rs` | 1 (`commit_gwz_paths_checked` `:193` — a git `_checked` seam, not a `CheckedArtifact` door) | 4 |
| `finalization/execute.rs` | 1 (same seam, `:31`) | 4 (convertible, not carved) |
| `handle_commit.rs`, `preserve/artifacts.rs`, `sync_workspace_boundary.rs`, `handle_create_repo.rs`, `handle_init_from_sources.rs`, `store/archive.rs`, `archive.rs`, `store/gc.rs`, `store/retention.rs` | 0 | 5 / 6 / 4 / 8 / 2 / 11 / 3 / 2 / 2 |

A "file names no checked boundary door" pin is FALSE on day one for
`abort/evidence.rs` and `abort/preflight.rs` — the same file carries the
carved v0 arm and the converted v1 arm. For those two the pin must be
per-ARM (region-scoped to `rollback_evidence` `:664-…` and `restore_baseline`
`:346-411`, in the CRLF-normalized region idiom P-2 already anticipates), and
the needle must exclude the git `_checked` seam or `finalize.rs` also fails.

### [P2-5] The "refreshable tree digests as the property-free backstop" does not exist for 12 of 13 carved files

§3 leans on the digests for the `std::fs`-direct class where the map does not
enumerate it. At main, `PROTECTED_SOURCE_DIGESTS` (`:40-74`) names, among the
files in play, only `preserve/artifacts.rs` (`:69`), `preserve/checked_bundle.rs`,
`preserve/plan.rs`, `root/artifact_facts.rs`, `preservation_root/files.rs`,
`capability.rs`, `entry.rs`; `PROTECTED_SOURCE_TREE_DIGESTS` (`:228-236`)
names seven files, none carved. NOT digest-pinned: `store/archive.rs`,
`v1_lifecycle/archive.rs`, `handle_commit.rs`, `finalize.rs`, `abort/preflight.rs`,
`abort/evidence.rs`, `sync_workspace_boundary.rs`, `handle_create_repo.rs`,
`handle_init_from_sources.rs`, `store/gc.rs`, `store/retention.rs` — nor,
for the record, `store/rewrite.rs`, so the same backstop sentence inherited
from `RecordRootAmendment` §3 P-1 is also false at main for the record root
(its only pin there is the `create_merge_store_record` entry-reference row,
`:377-379`, and the O13 counts). Either the pins package adds the carved files
to a digest map (a refresh cost on every legitimate edit) or the `std::fs`
class is put in the count map ([P2-4] (iii)) — the amendment must pick one
rather than cite a backstop that is absent.

### [P3-1] `:275`'s origin list omits the completed forward dispositions

Row `:275` says the archive runs "from abort/preserve". `respond()`
(`start.rs:107-125`) is also called at `:98` after `ResumeStart` and at `:169`
after `Continue`, so a COMPLETED `--no-ff` start or resume archives through the
same `Archive` request (`archive.rs:79-85`) on the same plain lease
(`service.rs:120`). Harmless to a lease-keyed carve; the sentence should say
"from every terminal disposition, abort included".

### [P3-2] `:279`'s cites attribute repo-add writers to "gwz repo create"

`handle_create_repo.rs:211` is in `handle_create_repo` (fn `:68`, guard `:90`)
and `:336,426` in `handle_add_existing_repo` (fn `:232`, guard `:244`) — both
under `guarded_workspace_root(RepoMutate)`, i.e. the mutation guard, not the
bare lock; only `:51` (`handle_create_workspace`, fn `:19`, lock `:35`) is the
bare-lock site E0.2 §5.2 Ground 2 cites. The operations are still on the list
(guard, broad reading), but the cell's "both named verbatim" attaches to the
wrong three of its four cites.

### [P3-3] The carved `.git/info/exclude` writer bootstraps `.git/info` by construction

`write_atomic` → `stage_durably` → `fs::create_dir_all(parent)`
(`artifact/mod.rs:492-495`, `:519-522`). So the carved boundary writer
creates `.git/info` when missing — the exact thing row `:279`'s own third
cell forbids ("never bootstrap `.git/info`"). The exception reads row
`:280`'s cells through itself but says nothing about `:279`'s (or `:277`'s
"all ordinary preflight first") frozen orderings, which the carved raw paths
do not all satisfy. Mechanism fact recorded for the State axis.

### [P3-4] The decoder cite the pins/GC brief will inherit names the unreachable arm

See [P2-2]: the v1 refusal fires at `decode.rs:119-121` → `header.rs:159-163`;
`decode.rs:128-133` is the typed twin that cannot be reached with `V0_ONLY`.
The prep cites the twin; the amendment cites neither. The GC brief should cite
the live one.

### [P3-5] Tree doctrine that "E4.7 does not retire the carved writers" leaves false or stale

- `catalog_names.rs:41-44`: `checked-artifacts` is "a directory that is still
  live until E4.7". Under §4/§5 (O14 → DR-1; E4.7 = allowance expiry + records)
  E4.7 retires nothing; the legacy area stays live until DR-1 at the earliest.
  Becomes false.
- `catalog.rs:19-22`, `:27-29`, `:34-36`, `:46-47`: dead surface "whose first
  production consumers are the E4.2-E4.6 conversions". With E4.4 converting
  nothing and E4.5-B/E4.6-B at most three arms, E4.7's "allowances expire with
  dated annotations" would expire them WITHOUT the promised consumers arriving.
- `bootstrap/managed.rs:45-46` "the unconsumed imports shrink as E4.3-E4.6
  convert" (E4.3-B's to reword per `RecordRootAmendment` §7) — still promises
  E4.4.
- `archive.rs:108-111` "no production caller until that route lands" — the
  O8 route (`gc_archived`) is not landed by the GC decode fix as §7 describes
  it; the allowance becomes indefinite, and plan `:512-513` ("+ the O8
  `gc_archived` production route lands with the archive consumer") is left
  without a carrier.
- Checker `:326-344` ("E4.2/E4.3 retire", "`archive.rs` and `store/archive.rs`
  are … E4.4's") — re-annotation is the pins package's.
- ADOPTED text this amendment supersedes without saying so:
  `RecordRootAmendment` §6 ("`store/archive.rs` and `archive.rs` keep their
  accepted-residual, retire-on-conversion marker … until E4.4 converts them —
  E4.4's charter author may not cite this section against that duty") and
  `E7-Acceptance.md:180` ("the archive rows keep retire-on-conversion until
  E4.4").

### [P3-6] The "permanent-row shape" is not on main

§3: "made a permanent-row shape by E4.3-B's P-1". At `7f28907` the map's
comment (`:326-344`) still reads accepted-residual/retire-on-conversion;
E4.3-B is on `e4/e4-3b-record-root`, not main. True as a description of the
post-E4.3-B baseline the pins package will start from; not a tree fact today.

### [P3-7] FAT32-free — verified; the "never driven" claim is by absence

Nothing in the amendment requires a probe volume; this review used none. §6's
"never driven on a non-admitted filesystem" is confirmed only by absence in
E4.1's evidence: PR-2/PR-4 and R7(iii)
(`a_v1_resume_refuses_without_mutation_and_abort_still_clears_the_record`)
abort a record whose resume was REFUSED, i.e. never published; PR-3 drove the
plain lease on FAT32 (`E4.1-Review.md` §R2.5 R2 row). The "Linux non-ext4
drive optional" clause is inert for this axis — except that, per [P2-1], such a
drive would show the abort doors PASSING (btrfs/xfs/zfs answer
`name_to_handle_at`), not failing.

---

## 3. MANDATE ANSWERS

**1. §1 table, row by row — over-/under-carves.**
*Over-carve found (P1):* `finalization/execute.rs:51` (row `:279`), reached
only under `acquire_activated` ([P1-1]). Its siblings `:45` (`:277`) and `:48`
(`:278`) the amendment correctly calls convertible.
*Under-carves (the E4.3 contradiction reborn) — none in the strict sense:* no
writer the amendment calls CONVERTIBLE is also reached from a listed
operation. The three convertible sites are reached from `ForwardRuntime` only.
*But the carved set is under-ENUMERATED* ([P2-3]) and mis-cited at `:275`'s GC
writer ([P2-2]) and `:276`'s v1 cell ([P2-3]).
*Lease facts verified:* `service.rs:116-121` is the only fork; `Archive`/
`Abort`/`Preserve` → `acquire` (`checked.rs:134-141`: `WorkspaceMutatorLock`
and nothing else); `ResumeStart`/`Continue` → `acquire_activated`
(`:158-164`: `acquire` + `activate_workspace_catalog`); the creation lease
`acquire_for_merge_start` (`:175-183`) is E4.2's, taken at `start.rs:84`
only. `service::run` production callers: `start.rs:90` (ResumeStart), `:161`
(Continue), `:165` (Abort/Preserve), `archive.rs:79` (Archive). Bare
`WorkspaceMutatorLock` (no v1 lease): `abort/mod.rs:44` (v0 abort, incl.
`--preserve` via `:46`), `gc.rs:167`, `archive.rs:97` (destination-only
archive read), `:126` (dead `gc_archived`), `handle_create_repo.rs:35`,
`handle_init_from_sources.rs:91`, `continue_op/coordinator.rs:13`,
`mutation_guard.rs:44`, `dispatch.rs:452`. The v0/v1 fork is by record
version at `dispatch.rs:328-334` (v1 → `handle_v1_command`; v0 → the v0
handlers at `:345-367`).

**2. The already-probing abort.** The chain `abort/evidence.rs:148-165` →
`root/artifact_facts.rs:39-89` → `entry.rs:159-166` → `observation.rs:96,105`
is VERIFIED end to end and reaches a durable-identity probe on a
post-publication v1 abort (execution via `reverse/execute/rollback.rs:61-68`;
observation via `abort/evidence.rs:54-75` → `file_states` `:269-323`, gated at
`:61-63`/`rollback_prefix.rs:76-81`). Pre-publication, the EVIDENCE chain
constructs nothing (`:39-45`; `rollback_prefix.rs:76-97,155-184`) — VERIFIED.
§6's scoped sentence is NOT true of the tree as written: (i) two other
plain-lease abort paths probe with no publication gate — `--preserve`'s bundle
door and the selected-root metadata doors ([P1-2]); (ii) "the same filesystem
the merge needed" over-states — the abort's legacy probe admits btrfs/xfs/zfs
where the catalog's `require_ext4` refuses ([P2-1]); (iii) "the merge's own
checked evidence" was written raw ([P2-1]). The prep's "converted on the PLAIN
lease" reverse arms for `:276`/`:277`/`:278` are NOT the same chain: `:276`'s is
the bundle door (a), `:277`'s root-preserve arm (`preservation_root.rs:477` →
`files.rs:37-45` → `entry.rs:100-107`) IS publication-gated (`v1_root_preservation_spec`
returns `None` without `plan.root_handoff`, `preserve/artifacts.rs:499-513`),
and `:278`'s reverse half is the evidence chain plus the root-metadata doors
(b). §6 must cover (a) and (b); it covers neither.

**3. The convertible remainder — it exists, and it is exactly three sites in one file.**
`v1_lifecycle/finalization/execute.rs:45` (`WriteMarker`, `write_atomic` →
rows `:277`/`:278`), `:48` (`WriteLock`, `write_atomic` → `:278`), `:51`
(`WriteBoundary`, `publish_workspace_exclude_candidate` → `:279`). Reach:
`PhysicalActionKind::Publication` → `ForwardRuntime::execute` (`forward.rs:68-70`)
→ `FinalizationRuntime::execute` (`finalization.rs:35-55`) → `execute::publication`
(`:53`); `ForwardRuntime::new` at `start.rs:89`/`:160` only, both immediately
before `service::run` with `ResumeStart`/`Continue` → `acquire_activated`
(`service.rs:117-119`). `forward/execute.rs` has no raw writer. Row `:276`
has NO forward writer (`ForwardRuntime` refuses `Preservation`,
`forward.rs:71-76`; preservation is reverse-only) and row `:275` has none
(`Archive` always takes the plain arm). So E4.5-B is real for `:277` and
vacuous for `:276`; E4.6-B is real for `:278` AND `:279` (the latter needing a
new git-directory replace door beside `entry.rs:92-98`/`:186-192`).

**4. The inventory.** Implementable in the checker's existing count-map shape,
both directions — VERIFIED — but only with the three generalizations in
[P2-4] (scan root, population gate, vocabulary); the `std::fs`-direct class is
countable by the bare-identifier idiom without a new primitive vocabulary
concept, only new tokens. Per-file negative scanning is UNSOUND for
`abort/evidence.rs` and `abort/preflight.rs` (mixed carved/converted arms) and
must be per-ARM there ([P2-4]). The digest backstop the amendment cites for the
`std::fs` class is absent for 12 of 13 carved files ([P2-5]).

**5. The GC gap.** `gc.rs:188-190` decodes with `decode_production_v0` —
VERIFIED; a v1 envelope fails it at `decode.rs:119-121` → `header.rs:159-163`
(`Unsupported`), not at the `:128-133` twin. GC touches no `CheckedArtifact`
today — VERIFIED (grep of `gc.rs`, `store/gc.rs`, `store/retention.rs`: none;
`handle_gc` holds only `WorkspaceMutatorLock`, `:167`). A read-side fix adds no
probe — VERIFIED — but must cover the SECOND v0-only decode on the deletion
path (`store/gc.rs:15` → `store/mod.rs:394`) and check `retention.rs:8`
([P2-2]); and the live deletion writer `store/gc.rs:16-17` is the `:275` GC
writer the carve must name, not the dead `remove_archive`.

**6. §4's E4.7 / O3 claim.** "Legacy writers mutate in their OWN area" —
VERIFIED at `policy.rs:33-54`: `private_parent()` is
`CatalogPrivateNameV1::LegacyPrivate` (`checked-artifacts`), pinned by R2-F
R1.1, not the catalog's `Final`; the corpus records O3 discharged on landing
(`E7-Acceptance.md:170`, plan `:175-181`). "E4.7 does not retire the carved
writers" leaves the promises in [P3-5] false or stale — `catalog_names.rs:44`
("live until E4.7") is the one that becomes affirmatively false; the
`catalog.rs` and `managed.rs` allowance reasons promise E4.2-E4.6 consumers
that no longer arrive; `RecordRootAmendment` §6 and `E7-Acceptance.md:180` are
adopted text silently superseded.

**7. FAT32-free.** Nothing requires a probe volume — VERIFIED; the optional
non-ext4 clause is inert for this axis ([P3-7]), with the [P2-1] caveat.

---

## 4. PER-CLAIM TABLE

| # | Claim (amendment §, cite) | Status | Where verified |
| --- | --- | --- | --- |
| 1 | §1 `CheckedArtifact::acquire` takes a durable identity at `observation.rs:96,105` and refuses `UnsupportedOperation` | VERIFIED | `observation.rs:82-120`, `:363-365`, `:376-381`; `identity.rs:154-156` |
| 2 | `:275` v1 archive via `respond()` from abort/preserve, PLAIN lease only, all of `store/archive.rs` (`service.rs:120`) | VERIFIED (lease); INCOMPLETE (origins, [P3-1]) | `service.rs:116-121`; `archive.rs:77-92`; `start.rs:98,118,169`; `store/archive.rs:11-86` |
| 3 | `:275` `remove_archive` via GC, no v1 lease (`archive.rs:207`, lock only) | lease VERIFIED (`:126`); reach REFUTED ([P2-2]) | `archive.rs:108-118,148,191-208`; `gc.rs:160-205`; `store/gc.rs:3-18` |
| 4 | `:276` ordinary merge preserve (`preserve/artifacts.rs:353`) | cite VERIFIED; label REFUTED — v0 `--abort --preserve` only ([P2-3]) | `preserve/artifacts.rs:292-354`; `preserve.rs:145`; `abort/mod.rs:44-49` |
| 5 | `:276` `abort --preserve` (`v1_lifecycle/reverse/execute/preservation.rs:74`) as a raw writer | REFUTED — converted, already-probing ([P2-3], [P1-2]) | `preservation.rs:71-74`; `checked_bundle.rs:64-112`; `entry.rs:139-148,168-175` |
| 6 | `:277` ordinary merge marker (`finalize.rs:245`) | VERIFIED | `finalize.rs:227-269`; `finalize_dispatch.rs:138,277`; `coordinator.rs:13` |
| 7 | `:277` `gwz commit` incl. bootstrap (`handle_commit.rs:334`) | VERIFIED | `handle_commit.rs:37,213,326-340`; `mutation_guard.rs:44` |
| 8 | `:278` `finalize.rs:245,248`; `handle_commit.rs:200,213`; `abort/preflight.rs:409-410` | VERIFIED | as cited; `preflight.rs:346` `restore_baseline` ← `abort/mod.rs:180` |
| 9 | `:278` "both aborts" | v0 VERIFIED; v1 reverse is converted; v0 abort's `evidence.rs:683-696` UNCITED ([P2-3]) | `abort/evidence.rs:664-705` |
| 10 | `:279` SOLELY create/init (`handle_create_repo.rs:51,211,336,426`; `init_from_sources.rs:203`), no fork | cites VERIFIED as writers; SOLELY REFUTED; "no fork" REFUTED ([P1-1]); attribution [P3-2] | [P1-1] table; `forward.rs:68-70`; `start.rs:89,160` |
| 11 | §1/§6 post-publication abort chain `evidence.rs:148-165` → `artifact_facts.rs:39-89` → `entry.rs:159-166` → `observation.rs:96,105` | VERIFIED | as cited; `reverse/execute/rollback.rs:61-68` |
| 12 | §6 pre-publication aborts construct no `CheckedArtifact` (`evidence.rs:39-41`; `rollback_prefix.rs:76-97,155-184`) | cites VERIFIED for the evidence chain; universal REFUTED ([P1-2]) | `preservation/entry.rs:95-104`; `plan.rs:407-446`; `validate/journal.rs:214-262`; `rollback.rs:121-127`; `root/abort.rs:109-145,380` |
| 13 | §6 "needs the same filesystem the merge needed" | REFUTED as exact ([P2-1]) | `identity.rs:312-367` vs `provider/platform/linux.rs:136-203` |
| 14 | §6 "the merge's own checked evidence" | wording REFUTED ([P2-1]) | `finalization/execute.rs:45,48,51` |
| 15 | §6 three homes: `capability.rs:53-57`, `OperationModel.md`, release-notes line | `capability.rs:52-55` VERIFIED; `OperationModel.md:147-148,157-159` VERIFIED (and `:147-148` already false, un-named); release-notes home NOT FOUND in gwz-core (`RELEASE.md` has no such line; only `capability.rs:55` names "needs no such filesystem") — the lane must locate it | grep of `*.md` |
| 16 | §6 remedy pin still satisfied by a scoped sentence | VERIFIED — `contracts.rs:156-163` requires only the substrings `persistent file handles`, `mount identity`, `--abort`, `--no-ff`; `:204` requires `--abort` in the rendered catalog refusal | `contracts.rs:149-209` |
| 17 | §3 O13 map = count-per-primitive per file, both directions | VERIFIED | checker `:345-350`, `:1187-1214` |
| 18 | §3 "enforced by the same map" for `std::fs` sites | CONDITIONAL — needs [P2-4] (i)–(iii) | checker `:308`, `:1188-1191`, `:942-949` |
| 19 | §3 tree digests as property-free backstop | REFUTED for 12/13 carved files ([P2-5]) | checker `:40-74`, `:228-236` |
| 20 | §3 per-file "names no checked door" scan | UNSOUND for `abort/evidence.rs`, `abort/preflight.rs` ([P2-4]) | door census |
| 21 | §3 "made a permanent-row shape by E4.3-B's P-1" | NOT ON MAIN ([P3-6]) | checker `:326-344`; prep `:10-12` |
| 22 | §3 `:275` sites B–H + four raw `std::fs` incl. `create_dir_all` at `:61` | VERIFIED (B `:55`, C `:56`, D `:63`, E `:65`, F `:66`, G `:78`, H `:79`; `remove_file` `:54`,`:77`; `create_dir_all` `:61`; A `archive.rs:206-207`) | `store/archive.rs`, `archive.rs` |
| 23 | §4 O3 discharged by R2-F; legacy writers in their own area | VERIFIED | `policy.rs:33-54`; `E7-Acceptance.md:170`; plan `:175-181` |
| 24 | §4/§7 "E4.7 does not retire the carved writers" leaves doctrine true | REFUTED for `catalog_names.rs:44`; stale elsewhere ([P3-5]) | `catalog.rs:17-50`; `managed.rs:43-47`; `archive.rs:108-111` |
| 25 | §7 `gc.rs:184-192` v0 decoder; every completed `--no-ff` since v0.11.0 un-GC-able | VERIFIED (`:188-190`); second decode site UNCITED ([P2-2]) | `decode.rs:114-136`; `header.rs:140-164`; `store/mod.rs:394`; `store/gc.rs:15` |
| 26 | §7 GC fix read-side, adds no probe, capability-free | VERIFIED (conditional on routing through `store/gc.rs`) | grep; `gc.rs:167` |
| 27 | §7 "no E4.4 conversion candidate exists" | VERIFIED — `V1LifecycleRequest::Archive` production issuer is `archive.rs:83` only; no branch or door for it in `entry.rs` | grep |
| 28 | §2/§6 FAT32-free; no probe volume required | VERIFIED | this review's method |
| 29 | §1 "`WorkspaceMutatorLock`'s nine (now ten) production sites" | VERIFIED as the §5.2 census; this review's own bare-lock census in mandate answer 1 | `SemanticsAmendment-DRAFT.md:948-960` |
| 30 | §1 "E4.3 was the first symptom … the shared `commit` could not satisfy both" | VERIFIED — `service.rs:139-140,180-181` `store.commit` under either lease | `service.rs` |

30 claims: 19 verified, 2 conditional, 9 refuted or refuted-in-part.

---

## 5. GREP TRANSCRIPTS (the load-bearing ones, verbatim)

```
$ grep -rn "service::run(" src --include='*.rs' | grep -v "/tests/"
src/workspace_ops/merge/v1_lifecycle/start.rs:90:    let disposition = super::service::run(
src/workspace_ops/merge/v1_lifecycle/start.rs:161:                    super::service::run(&store, root, merge_id, lifecycle, &mut runtime)?
src/workspace_ops/merge/v1_lifecycle/start.rs:165:                    super::service::run(&store, root, merge_id, lifecycle, &mut runtime)?
src/workspace_ops/merge/v1_lifecycle/archive.rs:79:        let response = service::run(

$ grep -rn "ForwardRuntime::new" src --include='*.rs' | grep -v "/tests/\|tests\.rs"
src/workspace_ops/merge/v1_lifecycle/start.rs:89:    let mut runtime = ForwardRuntime::new(backend, context);
src/workspace_ops/merge/v1_lifecycle/start.rs:160:                    let mut runtime = ForwardRuntime::new(backend, context);

$ grep -rn "FinalizationRuntime::new\|finalization::publication\|execute::publication" src --include='*.rs' | grep -v "/tests/"
src/workspace_ops/merge/v1_lifecycle/finalization.rs:53:        execute::publication(self.backend, current, *action)
src/workspace_ops/merge/v1_lifecycle/forward.rs:69:                FinalizationRuntime::new(self.backend, self.context).execute(lease, current, action)

$ grep -n "write_atomic\|fs::\|write_marker\|write_lock\|write_bundle\|publish_workspace_exclude\|sync_dir\|rename" src/workspace_ops/merge/v1_lifecycle/forward/execute.rs
(none)

$ grep -rn "persist_stash_bundle" src --include='*.rs'
src/workspace_ops/merge/preserve.rs:145:            persist_stash_bundle(root, &record, plan, &result.message, &result.object_id)?;
src/workspace_ops/merge/preserve/artifacts.rs:292:pub(super) fn persist_stash_bundle(
$ grep -rn "preserve_then_abort" src --include='*.rs' | grep -v "fn "
src/workspace_ops/merge/abort/mod.rs:46:        return super::preserve::preserve_then_abort(

$ grep -n "CheckedArtifact\|checked_artifact\|entry::\|artifact_facts\|durable_identity" src/workspace_ops/merge/gc.rs src/workspace_ops/merge/store/gc.rs
(none)

$ grep -rn -i "ext4" src --include='*.rs'
src/checked_artifact/bootstrap/runtime/catalog_lease/target.rs:451:        SupportedFilesystemProfile::LinuxExt4FsIocGetFsUuidV1 => 1,
src/checked_artifact/capability/pre_catalog/provider/production_tests.rs:55,69,86: (tests)
src/checked_artifact/capability/pre_catalog/provider/managed_mutation.rs:737: (comment)
$ grep -n "fn \|name_to_handle_at" src/checked_artifact/capability/pre_catalog/provider/platform/linux.rs | grep -n "require_ext4\|filesystem_uuid\|persistent_handle"
136:fn require_ext4(fd: RawFd) -> Result<(), CheckedFsError> {
153:fn filesystem_uuid(fd: RawFd) -> Result<[u8; 16], CheckedFsError> {
173:fn persistent_handle(fd: RawFd) -> Result<(i32, Vec<u8>), CheckedFsError> {
(identity.rs:312-367 has name_to_handle_at, fstatfs f_fsid, statx MNT_ID — no ext4 check)
```

No cargo invocation, no probe volume, no worktree: all of the above is
read/grep on the checked-out `main` at `7f28907`.

---

## 6. WHAT ROUND 2 NEEDS (text folds; no build)

1. Row `:279` → "partial"; `finalization/execute.rs:51` joins the convertible
   set (E4.6-B) with the [P1-1] chain as its reachability proof; "SOLELY"
   replaced by the [P1-1] writer table (or a pointer to the inventory that
   will carry it).
2. §6 re-scoped by PATH, not publication state, naming (a) `--preserve`'s
   bundle door and (b) the selected-root metadata doors alongside the
   evidence chain; `OperationModel.md:147-148`'s "with or without `--preserve`
   … either version" named as a home; "same filesystem" → "persistent file
   handles and a mount identity"; "checked evidence" → "published evidence,
   re-verified through the checked boundary"; the release-notes home located.
3. Row `:275`: GC writer = `store/gc.rs:16-17` (+ `retention.rs:37,41`), with
   `remove_archive` recorded as a dead arm awaiting the O8 route; §7's GC fix
   names both decode sites (`gc.rs:188`; `store/mod.rs:394` via `store/gc.rs:15`)
   and the `retention.rs:8` check.
4. Rows `:276`–`:279`: the [P2-3] sites enumerated; `:276`'s two cells
   separated into the v0 raw writer (carved) and the v1 converted writer
   (already-probing, dated under §6).
5. §3: the three generalizations of [P2-4] named; the negative scan made
   per-ARM for `abort/evidence.rs` and `abort/preflight.rs` with a needle that
   excludes the git `_checked` seam; the digest backstop sentence replaced by
   an explicit choice ([P2-5]).
6. §4/§5: the [P3-5] texts routed (E4.7's expiry list; `catalog_names.rs:44`;
   the superseded `RecordRootAmendment` §6 / `E7-Acceptance:180` sentences;
   the O8 route's carrier).

On those folds this axis expects GO in round 2 on a text-diff confirmation
against the cites above; no new tree reads are needed unless the folds cite
new lines.

---

## ROUND 2 — Code axis confirmation

**Object.** The folded v2 at `GwzM5-8R2E-CapabilityFreeAmendment.md` (480
lines; §8 records both round-1 verdicts), diffed against the round-1 draft
preserved at session scratch `CapFree-v1-draft.md` (264 lines, identical to
the object reviewed above). **Date.** 2026-09-02. **Method.** Text-diff
confirmation of every §6 "what round 2 needs" item, plus a mechanical
set-difference of every `file:line` cite in v2 minus v1 minus this report
(89 cites in v2; the residue is listed below) with each genuinely new TREE
cite opened at its line on gwz-core `main` `7f28907` (still byte-clean; no
build, no probe volume, no worktree, nothing modified but this report).

# GO (terminal)

Every round-1 condition is folded exactly, no fold introduced a tree claim
this axis cannot stand behind, and the two residual notes below are P3
labels, not mechanisms — recorded, not blocking.

### Item-by-item confirmation (folded sentence quoted)

**(1) `:279` → partial; `execute.rs:51` convertible with the [P1-1] chain;
"SOLELY" replaced; guard-vs-bare-lock attribution.** CONFIRMED. §1 row
`:279`, last column: *"`finalization/execute.rs:51` (`PublicationPhysicalAction::WriteBoundary`
→ the same family) — same chain, `acquire_activated` ONLY. NOTE: `entry.rs`
has only the git-directory OBSERVE door (`:92-98` → `:186-192`), no replace
door — E4.6-B mints one"*; the chain is spelled at row `:277`
(*"`finalization.rs:53` ← `forward.rs:68-70` ← `ForwardRuntime::new` at
`start.rs:89`/`:160` ← `service::run(ResumeStart|Continue)` → `service.rs:117-119`
`acquire_activated`; the reverse runtime never carries `Publication`"*) and
inherited by `:278`/`:279` as "same chain". The writer family table replaces
"SOLELY" in full — fourteen sites — with the attribution: *"`handle_create_workspace`
(`handle_create_repo.rs:51`, bare lock `:35`); `gwz repo create` and
add-existing (`:211`, `:336,426` — under `guarded_workspace_root(RepoMutate)`,
the mutation guard, not the bare lock); `init-from-sources`
(`handle_init_from_sources.rs:203`, bare lock `:91`)"*. §7: *"E4.6-B is
real for `:278` (`execute.rs:48`) and `:279` (`execute.rs:51`, needing a
git-directory replace door in `entry.rs`)"*. The former "Split escape?"
column is gone; the two-column shape (listed writers | convertible forward
arm) carries the same fact more precisely.

**(2) §6 scoped BY PATH; the three doors named; `OperationModel.md:147-149`
a home; "persistent file handles and a mount identity"; the strictly-weaker
legacy-probe fact; "published evidence, re-verified through the checked
boundary"; release-notes home status.** CONFIRMED. §6: *"a v1 abort takes a
checked door — and therefore the durable-identity probe — whenever it must
re-verify a checked artifact: published evidence (post-publication,
`abort/evidence.rs:148-165`), a preservation bundle (`--preserve` with an
integrated participant, `preservation/entry.rs:102-104`), or a selected
root's manifest/lock (`@root` selected, `validate/journal.rs:253-258` →
`root/abort.rs:131,145,380`). None of these is gated on publication alone;
the "pre-publication aborts need no such filesystem" universal — inherited
from RR §1b … — is false for paths (ii) and (iii), and
`OperationModel.md:147-149` is false today."* The disposition sentence:
*"an abort that touches no checked artifact needs no such filesystem;
aborts that must re-verify checked artifacts — preservation bundles, a
selected root's manifest and lock, or the merge's published evidence,
re-verified through the checked boundary — need persistent file handles and
a mount identity."* The weaker-probe fact: *"the abort's doors go through
the LEGACY identity probe (`identity.rs:312-367`, `name_to_handle_at` +
`statx` mount id, no ext4 magic), which ADMITS btrfs/xfs/zfs where the
catalog's `require_ext4` (`provider/platform/linux.rs:136-151`) refuses —
the post-publication abort needs a STRICTLY WEAKER filesystem than the
merge, coinciding only on substrates without `name_to_handle_at`."* Homes:
`capability.rs:50`, `:54-55`; `OperationModel.md:131-132`, `:147-149`, `:158`;
`checked.rs:129-133`; `dispatch.rs:437-438,447-448`; `service.rs:112-115`;
*"the E4.1 release-notes line, which has NO on-disk home (E4.1 [P3-1]: it
lives in a commit message; carrier the release train)"* — consistent with
round 1's grep (only `capability.rs:55` names "needs no such filesystem").

**(3) `:275` GC writer; `remove_archive` dead; three decode sites with the
live refusal cite.** CONFIRMED. §1 row `:275`: *"The LIVE GC deletion writer
is `store/gc.rs:16-17` (`fs::remove_file`, `sync_dir`) and
`store/retention.rs:37,41`, under `WorkspaceMutatorLock` only (`gc.rs:167`).
`archive.rs::remove_archive` (`:191-208`, pin `sync_dir` 2) is a DEAD arm:
its only route is the test-only `gc_archived` family behind the
`archive.rs:108-111` allowance."* §7: *"three v0-only decode sites on the
live GC path — `gc.rs:188` (`decode_production_v0` for
`preflight_backup_artifacts`), `store/gc.rs:15` → `store/mod.rs:394`, and
`store/retention.rs:8` (id-less path) — fail a v1 envelope at
`decode.rs:119-121` → `header.rs:159-163` (the `:128-133` arm is the
unreachable typed twin)"*; §5 dispositions the dead family and keeps the fix
off it.

**(4) [P2-3] sites per row; `:276` split; `gwz stash`; the third primitive
class.** CONFIRMED. `:277` adds *"v0 abort (`abort/evidence.rs:696`
`fs::remove_file(marker)`); v0 `--abort --preserve` (`preserve/artifacts.rs:226`
`write_atomic(marker)`)"*; `:278` adds *"v0 abort (`abort/evidence.rs:689-692`
`write_atomic(LOCK_PATH)`)"*; `:279` the family table. `:276`: *"v0 `--abort
--preserve` (`preserve/artifacts.rs:353` `stash::write_bundle`, reached only
via `preserve.rs:145` ← `abort/mod.rs:46` — NOT an ordinary merge); `gwz
stash` (`stash/mod.rs:260-262` `write_atomic`, mutation guard
`handle_stash.rs:43`). The v1 preserve writer (`reverse/execute/preservation.rs:74`
→ `checked_bundle.rs:64-112` → `entry::replace_merge_preservation_bundle`) is
CONVERTED and already probing on the PLAIN lease — §6's class, not a carved
writer."* §1's closing paragraph names the three classes: *"`durable_fs` …,
`std::fs`-direct (`create_dir_all` / `remove_file`), and the
`artifact::write_atomic` family (`write_atomic` / `write_marker` /
`write_lock` / `write_manifest_and_lock` / `stash::write_bundle` /
`publish_workspace_exclude_candidate` / `sync_workspace_boundary` /
`ensure_workspace_exclude`, itself over `durable_fs`)"*.

**(5) §3's three checker changes; per-ARM scans with the `_checked` seam
excluded; the digest sentence replaced; RR §3 flagged.** CONFIRMED. §3:
*"three named changes to the checker's existing mechanism
(`check_checked_artifact_boundaries.py:1187-1214`, verified: count per
primitive per file on `mask_non_code` output, three findings …): (i) an
EXPLICIT carved-file list under `src/` replaces the `v1_lifecycle/` scan
root; (ii) the `\bdurable_fs\b` population gate is dropped …; (iii) the
token vocabulary widens to all three primitive classes"*; *"The `std::fs`
and `write_atomic` classes go INTO the count map — this amendment chooses
the map over expanding the digest tables, which at main pin only
`preserve/artifacts.rs` of the carved files (the "refreshable tree digest
backstop" sentence of RR §3 P-1 is likewise false at main for
`store/rewrite.rs` … — RR §3 gains the dated correction; the pins are the
backstop)"*; *"per-ARM, region-scoped, for the two mixed files
`abort/evidence.rs` (carved v0 `rollback_evidence` beside 27 converted-arm
door namers) and `abort/preflight.rs` (`restore_baseline` beside its
`artifact_facts` observe), with a needle that excludes the git `_checked`
seam (`finalize.rs:193`, `execute.rs:31`) or `finalize.rs` fails on day
one."*

**(6) [P3-5] doctrine routed; [P3-1]/[P3-3]/[P3-6]/[P3-7].** CONFIRMED. §4
names the allowance class by file and line (`workspace_mutator_lock.rs:44-47`,
`catalog.rs:19-22,27-29,34-36,46`, `coordinator/mod.rs:16`,
`entry.rs:299-300`, `bootstrap/managed.rs:45-46`,
`checked_artifact/mod.rs:12,18,29-31,37,44,50,59`, checker `:358`) with
*"each EXPIRES or is RE-REASONED AS PERMANENT"*; `catalog_names.rs:44`
("still live until E4.7") gains its bracket; RR §6 `:218-227` is *"now
inverted — no duty remains for E4.4's charter author to be protected from"*
and E7-Acceptance `:180` is bracketed; §5 dispositions the O8 route
(*"RE-OWNS to DR-1, conditional on (C)"*). [P3-1]: *"reached via `respond()`
from EVERY terminal disposition — completed start/resume (`start.rs:98,:169`),
abort, preserve"*. [P3-3]: §1 *"`write_atomic` → `stage_durably` →
`fs::create_dir_all(parent)` (`artifact/mod.rs:492-495, 519-522`), so the
carved `.git/info/exclude` writer bootstraps `.git/info` by construction"*
and §3's `:279` cell *"NOT MET by the raw family … ACCEPTED as part of this
exception, the fact recorded"*. [P3-6]: *"as of E4.3-B's landing a
permanent-row shape — at main `7f28907` its comment still reads
retire-on-conversion"*. [P3-7]: *"'never driven on a non-admitted
filesystem' holds by absence (E4.1's PR-3 drove the lease, not these doors …
a Linux non-ext4 drive, if ever available, would show these doors PASSING,
not failing)"*.

### Adversarial glance — cites in v2 not in v1 and not in this report

The mechanical residue (v2 − v1 − report) is 35 strings; 26 are regex
fragments or re-spellings of cites verified in round 1 (`evidence.rs:39-45`,
`finalization/execute.rs:45|48`, `finalize.rs:193|248`, `start.rs:98`,
`store/archive.rs:38-45`, `root/abort.rs:131,145,380`, `entry.rs:299-300`,
`capability.rs:50`, `sync_workspace_boundary.rs:84-90`,
`preservation/entry.rs:102-104`, `validate/journal.rs:253-258`,
`provider/platform/linux.rs:136-151`, `store/retention.rs:8|:37,41`,
`managed.rs:44-47`, `catalog.rs:19-22,…`, `checked.rs:129-133`,
`service.rs:112-115`, `OperationModel.md:131-132|147-149`). The genuinely
NEW tree cites, each opened now:

| New cite in v2 | Status | What the line says |
| --- | --- | --- |
| `handle_stash.rs:43` | VERIFIED | `guarded_workspace_root(… OpenMergeCommand::StashMutate …)` `:43-48` — the mutation guard |
| `store/mod.rs:131` | VERIFIED | `crate::checked_artifact::entry::prepare_merge_store_parents(root)?` under `if !path_exists(&path)?` `:130-132` — the v0 store's permanent caller; `ConsumerCheckpoint:284-286`'s "removed after parity tests pass" is indeed unreachable while the v0 store lives |
| `workspace_mutator_lock.rs:44-47` | VERIFIED | `#[allow(dead_code, reason = "R2-C0 freezes the checked catalog borrow before the C1 owner consumes it")]` on `catalog_mutation_lease` — stale: consumed at `checked.rs:161,179-180` |
| `coordinator/mod.rs:16` | VERIFIED | reason `"… conversion arrives consumer by consumer across E4.2-E4.6"` |
| `checked_artifact/mod.rs:12,18,29-31,37,44,50,59` | VERIFIED | `:12,18,37,44,50,59` "frozen interface awaiting R2-E consumer conversion (plan §5 item 1)"; `:29-31` "the checked entry-point inventory is consumed by the legacy leaf writers …" |
| `runtime/dispatch.rs:437-438, 447-448` | VERIFIED | `:437-438` "abort routes to the reverse service and its capability-free lease … an abort never creates a catalog"; `:447-448` "Not a wedge: `gwz merge --abort` is capability-free and clears it" — two more homes of the unconditional class, correctly listed |
| "the reverse runtime never carries `Publication`" (§1 `:277` cell) | VERIFIED | `reverse.rs:78-80`: `Participant { .. } \| Publication(_) \| Archive => failure(route_error(…))` |
| eight carrier strings, `gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json` | VERIFIED | exactly 8 occurrences of "R2-E E4.4 terminal-archive consumer, ratified by lane-owner determination at the E5 landing …" |
| `handle_commit.rs:334` "unsynced `create_dir_all`" | VERIFIED | `:334-339`, no `sync_dir` follows |
| `:275` "call sites 1 + 6" | VERIFIED | `rename_noreplace` `:63`; `sync_dir` `:55,:56,:65,:66,:78,:79`; the `use` at `:3` supplies the +1 each |
| "in practice `git init` always creates it" (`.git/info`) | NOT A TREE CLAIM | general git behaviour (template dir); true by default, false under an empty `--template=`; hedged by "in practice" — acceptable |
| `SemanticsAmendment-DRAFT.md:917|913-917|1170-1175|1235-1239`, plan/E7-Acceptance/RelocationPlan/R4bG-Evidence line cites | NOT TREE CLAIMS | dev-docs record homes — the State axis's; `:914-917` re-read (the decision sentence), the rest not opened here |

**No new tree claim is unverified; none is refuted.**

### Two residual notes (P3, recorded, not blocking)

- **[R2-P3-1] §3's exception reads row `:280` through itself for "any v1
  forward arm that stays raw".** The exception's ground — converting would
  place a LISTED operation on the probe — does not hold for
  `finalization/execute.rs:45,48,51`: those arms already sit behind
  `acquire_activated` (the probe), and no listed operation reaches them
  (§1's own last column). If E4.5/6-B is "not taken" (§7: "if taken"), the
  forward arms need an accepted-residual record of their own, not this
  exception's ground. One clause to cut or re-ground; mechanism otherwise
  intact.
- **[R2-P3-2] §7 names [P3-C1]'s guard row "the package's PRE-PUBLICATION
  capability-free pin".** That label re-imports the publication-state
  framing §6 just abandoned: the R7(iii)-shaped row aborts a record that
  touches no checked door, and is a pin for exactly that ("an abort that
  touches no checked artifact"), not for "pre-publication". Rename the pin
  to §6's by-path vocabulary so it cannot later be cited for the universal
  round 1 refuted.

Neither note changes a tree fact stated in v2; both are labels. GO stands
with them recorded; folding them is the lane owner's option inside the
pins package.

**Attestation.** gwz-core `main` `7f28907`, `git status --porcelain` empty
after this round; no build, no probe volume, no worktree, no commits.
