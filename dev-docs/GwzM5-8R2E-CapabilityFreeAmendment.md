# GwzM5-8 R2-E Capability-Free Amendment — the remaining §10 rows carve out; the list stands

Date: 2026-09-02. Author: the implementation lane (Fable, lane owner).
Status: **ADOPTED 2026-09-02 — dual complete, both axes GO (terminal)
in round 2; the State axis's one mandatory landing fold (the §5.2
quote made verbatim) and its precision notes are in this text** (round
1: Code axis NO-GO, 2 P1 / 5 P2 / 7 P3, all text folds, "the ruling's
scoping is intact and nothing returns to the operator on this axis";
State axis GO-WITH-CONDITIONS, 0 P0 / 1 P1 / 7 P2 / 8 P3, §6 ruled NOT
list-amendment-by-stealth, DR-1's minting ruled within the lane's
routing authority with conditions; reports verbatim at
`-ReviewCode.md` / `-ReviewState.md`; round-1 draft preserved in the
session scratch as `CapFree-v1-draft.md`). Tier: peer-blind Code+State,
Fable×2, on the refined tier policy's amendment-tier line (checkpoint
2026-08-22, "Fable ONLY at the program-level dual gates — … this
amendment-tier work — plus escalations"); this dual sits outside plan §2's
"Two duals maximum" on that line's authority, the same footing as
`GwzM5-8R2E-RecordRootAmendment.md` (RR) — plan §2's home gains the dated
bracket recording that the line now carries three further amendment-tier
duals (RR, this, DR-1). Operator ruling authorizing this object, verbatim on
disk: `GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`. Its operative lines
(elisions marked "[…]"; nothing paraphrased here widens or narrows it):

> Closed. (A) — Ground 2 stands. Ordinary merge / commit / create /
> abort / GC stay capability-free. Ext4-only was for the checked
> feature, not "gwz dies on Fedora." Do not amend the list to put those
> operations on the catalog probe. […] One dual-tier amendment, not four
> more conversion deliveries: 1. Record the tension […] 2. Capability-
> free list stands. Rows `:275–:279` whose writers are on that list are
> carved out — raw durable writers stay, documented and pinned
> (generalize the E4.3-B / O13 inventory shape). Convert only arms
> already on `acquire_activated`. 3. Re-scope O1, the R2-D milestone,
> and E4.7 […] E4.7 does not retire those writers. 4. Mint or
> explicitly defer the tier-2 archive sub-surface (do not invent it in
> E4.4). 5. Route (C) […], reader-side record reconciliation, and O14
> […] to one phase-end design round. […] Do not start (C) inside E4.
> FAT32. Not a supported filesystem. […] Do not start E4.4–E4.6 as
> originally chartered. […] Terminal NO-GO on a different scoping comes
> back to me; do not pick (A) or start (C) to unblock a step.

Object: (1) the recorded tension; (2) the dated carve-out of every §10
row `:275`–`:279` writer that runs from a capability-free operation, with
the generalized fail-closed inventory that pins it and the per-row,
per-cell disposition of the rows' own frozen clauses; (3) the re-scopes
of O1, the Phase-E4 milestone, plan §0's object and E4.7 — E4.7 narrowed
exactly as the ruling says and no further; (4) the explicit deferral of
the tier-2 archive sub-surface and the disposition of O8's checked-archive
route; (5) the minting of the phase-end design round DR-1 with its agenda
recorded as ruling items plus lane-routed items; (6) the abort-sentence
correction, scoped BY PATH from the tree; (7) what the remaining E4 work
becomes, with the pins package's complete must-cure list.

## 1. The tension, recorded

Two adopted records contradict each other for every remaining §10 row,
and no ADOPTED record said so (the E4.4 charter prep §7.5 and RR §1b said
it for their own rows the same day):

- **R2-D's premise** (`GwzM5-8R4bR2ConsumerCheckpoint.md` §10, the
  conversion table; `GwzM5-8R2E-Plan.md` Phase E4's heading, "milestone:
  production writes go through the boundary"; plan §0's object, "the §10
  consumer conversion table is executed"): every production durable
  writer converts onto the checked artifact boundary.
- **R2-E E0.2 §5.2's decision** (`GwzM5-8R2E-SemanticsAmendment-DRAFT.md`
  :909-967, Ground 2): the durable-identity capability is required only
  where the catalog is consumed. Its decision sentence, verbatim
  (:915-917): *"an ORDINARY merge, `gwz repo create`, `init-from-sources`,
  abort, GC and the mutation guard continue to work; a checked-artifact
  action refuses, typed."* Those operations stay capability-free because *"Option (i) would cost a Linux user
  every mutation, not just the checked ones … turns a filesystem-
  capability gap into a total loss of gwz on that filesystem."*

The checked boundary's `CheckedArtifact::acquire` takes a durable object
identity (`observation.rs:96,105`) and refuses `UnsupportedOperation`
without it. Converting a writer that runs from a listed operation
therefore places that operation on the probe. The reach, row by row —
the prep's table (`GwzM5-8R2E-E4.4-CharterPrep.md` §1/§7) as CORRECTED by
the Code axis at every cite:

| Row | Listed-operation writers today (tree cites) | Convertible forward arm (reached ONLY under `acquire_activated`) |
| --- | --- | --- |
| `:275` terminal archive | ALL of `store/archive.rs` (pin-reference counts `rename_noreplace` 2, `sync_dir` 7 — call sites 1 + 6 — plus four raw `std::fs` mutations incl. the `create_dir_all` at `:61`, the row's "when missing" bootstrap), reached via `respond()` from EVERY terminal disposition — completed start/resume (`start.rs:98,:169`), abort, preserve — always as `V1LifecycleRequest::Archive` on the PLAIN lease (`service.rs:120`). The LIVE GC deletion writer is `store/gc.rs:16-17` (`fs::remove_file`, `sync_dir`) and `store/retention.rs:37,41`, under `WorkspaceMutatorLock` only (`gc.rs:167`). `archive.rs::remove_archive` (`:191-208`, pin `sync_dir` 2) is a DEAD arm: its only route is the test-only `gc_archived` family behind the `archive.rs:108-111` allowance. | NONE — the forward runtime refuses archive actions (`forward.rs:71-76`); `Archive` is always plain |
| `:276` stash bundles | v0 `--abort --preserve` (`preserve/artifacts.rs:353` `stash::write_bundle`, reached only via `preserve.rs:145` ← `abort/mod.rs:46` — NOT an ordinary merge); `gwz stash` (`stash/mod.rs:260-262` `write_atomic`, mutation guard `handle_stash.rs:43`). The v1 preserve writer (`reverse/execute/preservation.rs:74` → `checked_bundle.rs:64-112` → `entry::replace_merge_preservation_bundle`) is CONVERTED and already probing on the PLAIN lease — §6's class, not a carved writer. | NONE — no v1 forward bundle writer exists (`forward.rs:71-76` refuses preservation) |
| `:277` markers | `gwz commit` (`handle_commit.rs:334` `create_dir_all`, the row's own "when missing" bootstrap, unsynced; marker writes under the guard `:37`); ordinary v0 merge publication (`finalize.rs:245`); v0 abort (`abort/evidence.rs:696` `fs::remove_file(marker)`); v0 `--abort --preserve` (`preserve/artifacts.rs:226` `write_atomic(marker)`) | `finalization/execute.rs:45` (v1 forward marker) — chain: `finalization.rs:53` ← `forward.rs:68-70` ← `ForwardRuntime::new` at `start.rs:89`/`:160` ← `service::run(ResumeStart|Continue)` → `service.rs:117-119` `acquire_activated`; the reverse runtime never carries `Publication` |
| `:278` workspace marker/lock | ordinary v0 merge (`finalize.rs:248`); `gwz commit` (`handle_commit.rs:200,213`); v0 abort (`abort/evidence.rs:689-692` `write_atomic(LOCK_PATH)`); the abort preflight (`abort/preflight.rs:409-410`). The v1 reverse rollback of a selected `@root` is CONVERTED (`root/abort.rs:380` `artifact_facts::write_checked`) on the PLAIN lease — §6's class. | `finalization/execute.rs:48` (v1 forward lock) — same chain |
| `:279` `.git/info/exclude` | the `publish_workspace_exclude_candidate` family (`sync_workspace_boundary.rs:84-90`, `write_atomic` on the leaf): `handle_create_workspace` (`handle_create_repo.rs:51`, bare lock `:35`); `gwz repo create` and add-existing (`:211`, `:336,426` — under `guarded_workspace_root(RepoMutate)`, the mutation guard, not the bare lock); `init-from-sources` (`handle_init_from_sources.rs:203`, bare lock `:91`); ordinary v0 merge (`finalize.rs:251`); v0 abort (`abort/evidence.rs:683`); v0 `--abort --preserve` (`preserve/artifacts.rs:233`); `gwz commit` (`handle_commit.rs:235,240`); materialize / stage / branch / repo-lifecycle / pull preflight under their guards (`handle_materialize.rs:128,404,456`; `handle_stage.rs:97`; `handle_branch.rs:234`; `handle_repo_lifecycle.rs:153,229,342`; `pull_head_member_preflight.rs:186,492`) | `finalization/execute.rs:51` (`PublicationPhysicalAction::WriteBoundary` → the same family) — same chain, `acquire_activated` ONLY. NOTE: `entry.rs` has only the git-directory OBSERVE door (`:92-98` → `:186-192`), no replace door — E4.6-B mints one |

Three mechanism facts the rows depend on: the mutation guard's listed
operations are read BROADLY — the operations under it (commit, stage,
materialize, branch, repo lifecycle, pull, stash), not merely the lock
acquisition — because the ruling names `commit` by name and §5.2 names
none of them (§2 records the reading at §5.2's home); `write_atomic` →
`stage_durably` → `fs::create_dir_all(parent)` (`artifact/mod.rs:492-495,
519-522`), so the carved `.git/info/exclude` writer bootstraps `.git/info`
by construction when it is missing (in practice `git init` always creates
it — the cell disposition in §3 records the fact); and the carved rows'
writers publish through THREE primitive classes — `durable_fs`
(`rename_noreplace` / `rename_durable` / `sync_dir`), `std::fs`-direct
(`create_dir_all` / `remove_file`), and the `artifact::write_atomic` family
(`write_atomic` / `write_marker` / `write_lock` / `write_manifest_and_lock`
/ `stash::write_bundle` / `publish_workspace_exclude_candidate` /
`sync_workspace_boundary` / `ensure_workspace_exclude`, itself over
`durable_fs`) — an inventory that counted only `durable_fs` names would
read ZERO at most carved files (§3 fixes the vocabulary).

E4.3 was the first symptom: its charter carried "abort/plain-lease paths
untouched" and "ride the boundary" together, and the shared `commit` could
not satisfy both (RR §1c). E4.4's charter prep, run BEFORE a build per RR
§4, found the wall on every remaining row — which is what the clause
exists to do — and this dual then corrected the prep's own enumeration in
four places (`:279`'s "solely" and its activated-only arm; `:276`'s
conflated writers; `:275`'s dead GC arm; the abort paths of §6).

**Already narrower than its text.** Three v1 abort paths on the PLAIN
lease consume checked artifacts today, all shipped with A1's v1 reverse
path, none gated on publication in the way §5.2's readers assume: (i) the
post-publication evidence re-verification (`abort/evidence.rs:148-165` →
`root/artifact_facts.rs:39-89` → `entry.rs:159-166` → `observation.rs:96,105`,
gated at `evidence.rs:39-45` on candidate + composition commit); (ii)
`--abort --preserve` with any integrated participant — the bundle door at
observation and execution (`preservation/entry.rs:102-104` →
`checked_bundle.rs:19-46` → `entry::classify_merge_preservation_bundle`;
owners from `preserve/plan.rs:407-446`, integrated results reached BEFORE
publication); (iii) plain `--abort` with `@root` selected — the root
metadata doors (`validate/journal.rs:253-258` → `rollback.rs:121-127` →
`root/abort.rs:131,145,380`), no publication condition. §6 dispositions
all three.

## 2. The list stands; (A) is closed

E0.2 §5.2's capability-free list is NOT amended. The operator closed
option (A) with Ground 2 standing: ext4-only is the CHECKED feature's
posture (ratified 2026-09-01), not gwz's. Binding on every later charter
in the phase: no step may put a listed operation on the probe to complete
a row. FAT32 is out of product and out of the lab: no row, no dispatch, no
probe volume. Two READINGS of §5.2 this amendment relies on are recorded
as one dated bracket at §5.2's own home
(`GwzM5-8R2E-SemanticsAmendment-DRAFT.md:917`, after the decision
blockquote), citing the ruling and this amendment, so a reader at the
list sees them: (a) "the mutation guard" is read BROADLY — the operations
executed under it — the operator's reading, resolved by naming `commit`;
(b) §6's narrowing — a v1 abort's re-verification of checked artifacts it
must reconcile is "a checked-artifact action [that] refuses, typed" under
§5.2's own second clause, not an operation placed on the probe. Neither
moves an operative word of the list.

## 3. The carve-outs, row by row — the cells dispositioned — and the inventory that pins them

For each row, the writers reached from a listed operation KEEP their raw
publication primitive, permanently, as a dated exception; only the
forward arms reached ONLY under `acquire_activated` (§1's last column)
convert, each in its own reviewed step with RR §4's lease-reachability
enumeration in its charter.

> **CAPABILITY-FREE EXCEPTION (2026-09-02, this amendment, operator-
> authorized).** For §10 rows `:275`–`:279`, every durable writer that
> executes from `gwz repo create`, `init-from-sources`, an ordinary
> merge, `gwz commit`, merge abort (either form, either record version),
> GC, or an operation under the mutation guard (`commit`, `stage`,
> `materialize`, `branch`, repo lifecycle, `pull`, `stash` — the broad
> reading, §2) retains its raw publication primitive of any of the three
> classes named in §1. Ground: E0.2 §5.2's capability-free list stands
> (operator ruling 2026-09-02); the checked boundary's acquisition
> requires the durable-identity capability, so a converted writer would
> place its operation on the probe, which the list forbids. These arms
> are a DATED EXCEPTION, not unfinished work: their conversion is not
> owed by any E4 step, and E4.7 does not retire them. Row `:280`'s second
> and third cells ("artifact actions"; "no legacy raw writer") are read
> through this exception for exactly the V1 arms it carves —
> `store/archive.rs` and the dead `archive.rs::remove_archive` — as RR
> §2 reads them for the record root. A v1 FORWARD arm that stays raw
> (the three `finalization/execute.rs` sites, if E4.5/6-B is not taken)
> is NOT covered by this ground — it already runs behind
> `acquire_activated` — and needs its own dated residual record in the
> pins package (Code axis [R2-P3-1]);
> the non-v1 arms were never `:280` paths and are governed by their own
> rows' cells, dispositioned below. What remains convertible is the set
> of writers reached ONLY under `acquire_activated`; each converts only
> with its own charter-time reachability proof.

**The rows' own cells, dispositioned** (the ConsumerCheckpoint §10 bracket
points here):

| Row | Cell 2 (purpose) | Cell 3 (frozen ordering) — raw-arm status today | Owed by the convertible arm |
| --- | --- | --- | --- |
| `:275` | `MergeArchive` when missing | "terminal/source validation before bootstrap; no creation for both-absent/destination-only" — SATISFIED by the raw path (`store/archive.rs:38-45`); the archive move is atomic (`rename_noreplace`, `:63`) so no record-root window exists (prep §2) | nothing — no forward arm |
| `:276` | `PreservationBundles` when missing | "parent durable before first bundle/native stash mutation" — met by the v1 converted preserve; ACCEPTED-UNMET for the v0 preserve and `gwz stash` raw writers as part of this exception | nothing — no forward arm |
| `:277` | `RootPreservationMarkers` when missing | "all ordinary preflight first; parent durable before member/root mutation" — ACCEPTED-UNMET at `handle_commit.rs:334` (unsynced `create_dir_all`) and the v0 arms, as part of this exception | E4.5-B converts `execute.rs:45` and owes the ordering there |
| `:278` | checked workspace artifact action | "exact source/goal and existing managed parent" — the v1 reverse rollback satisfies it (converted); NOT MET by the raw forward/ordinary/commit/abort-preflight arms, ACCEPTED as part of this exception | E4.6-B converts `execute.rs:48` and owes it there |
| `:279` | checked Git-directory artifact action | "observe existing Git-owned parent; never bootstrap `.git/info`" — NOT MET by the raw family: `write_atomic` bootstraps the parent by construction (`artifact/mod.rs:492-495`), inert only because `git init` always creates `.git/info`; ACCEPTED as part of this exception, the fact recorded | E4.6-B converts `execute.rs:51` through a new git-directory replace door and owes both clauses there |

**The generalized inventory (the pin).** The O13 per-file count map
(`V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES`; as of E4.3-B's landing a
permanent-row shape — at main `7f28907` its comment still reads
retire-on-conversion) generalizes into a CAPABILITY-FREE RAW WRITER
INVENTORY with three named changes to the checker's existing mechanism
(`check_checked_artifact_boundaries.py:1187-1214`, verified: count per
primitive per file on `mask_non_code` output, three findings — gained
file, lost file, count moved): (i) an EXPLICIT carved-file list under
`src/` replaces the `v1_lifecycle/` scan root; (ii) the `\bdurable_fs\b`
population gate is dropped — most carved files never name it; (iii) the
token vocabulary widens to all three primitive classes of §1, every
spelling countable by the bare-identifier idiom (`\bcreate_dir_all\b`
matches `fs::create_dir_all`). Per carved file the map records the
primitives and their counts (pin-reference counts, `use` lines included,
labeled as such), each row marked with its exception authority (this
amendment; the record root's row with RR), fail-closed in BOTH directions:
growth fires (a new raw writer is not blessed), shrinkage fires (a
conversion of a carved arm may not land without revising this amendment
— P-1's message class). The `std::fs` and `write_atomic` classes go INTO
the count map — this amendment chooses the map over expanding the digest
tables. *[Corrected 2026-09-02, post-adoption, on the E4.3-B review's
[P1-1] proof: tree-digest roots named `mod.rs` cover their whole parent
subtree, so `workspace_ops/merge/v1_lifecycle/mod.rs`'s digest ALREADY
backstops the three carved files under `v1_lifecycle/` — `store/rewrite.rs`,
`store/archive.rs`, `archive.rs`; the Code axis's [P2-5] sentence ("absent
for 12 of 13 carved files … nor `store/rewrite.rs`") and the bracket it
caused in RR §3 were wrong for those three and are withdrawn there. NOT
so for `store/gc.rs` and `store/retention.rs`: they live in the v0 store
(`src/workspace_ops/merge/store/`), outside `v1_lifecycle/`, and are
neither flat- nor tree-pinned (E4.3-B review round 2, [P3-10] — the lane
owner's first correction over-reached by the same path conflation it
corrected, pointing the other way). What remains true: the per-file `PROTECTED_SOURCE_DIGESTS` table
names only `preserve/artifacts.rs` of the carved files, and the NON-v1
carved files' tree coverage is enumerated by the pins package per file
(flat-pinned / tree-pinned by which root / unpinned). The count-map choice
stands regardless: a digest is a refresh-only byte backstop, the map is the
property pin.]* Per carved file a negative
scan in E4.3-B's P-2 idiom (CRLF-normalized at the read; self-excluding
needle) asserts the carved ARM names no checked boundary door — per-ARM,
region-scoped, for the two mixed files `abort/evidence.rs` (carved v0
`rollback_evidence` beside 27 converted-arm door namers) and
`abort/preflight.rs` (`restore_baseline` beside its `artifact_facts`
observe), with a needle that excludes the git `_checked` seam
(`finalize.rs:193`, `execute.rs:31`) or `finalize.rs` fails on day one.
Anti-vacuity throughout: a blinded scan root or a vanished file fails
loudly.

## 4. The re-scopes — E4.7 narrowed exactly as ruled

- **O1's obligation text and close form.** Plan §1 O1's text ("The §10
  consumer conversion table, executed: …") is re-scoped by a dated
  bracket at its own row (plan :46) — the SECOND re-scope, after the
  operator's ruling "a" (E7-Acceptance framing (i)) — to "executed as far
  as E0.2 §5.2 permits: every row dispositioned by name — converted
  (E4.1, E4.2, the convertible forward arms), carved by dated exception
  (this amendment; RR), or already-converted-and-dated (§6)". SEQUENCING,
  chosen: **O1 closes DISCHARGED at E4.7 against the re-scoped text,
  citing O14 as RE-OWNED to DR-1** (not awaiting DR-1's disposition, which
  is post-phase); brackets at plan :145 (O14's "O1's close must cite O14's
  disposition"), RR §6 :226-227, E7-Acceptance :168 (extended) and a
  pointer beside framing (i) :15-20; O1's close cites this amendment, RR,
  O13 and O14.
- **The Phase-E4 milestone** (plan :331 heading; :333-340 preamble
  universal; plan :15-16 §0 object): re-scoped by dated annotation to
  "CHECKED-FEATURE writes go through the boundary; capability-free arms
  are a dated exception." The ConsumerCheckpoint §10 gains per-row
  brackets at `:275`–`:279` pointing at §3's cell table, one at `:280`
  (the scoped device), and one at `:282-286` recording that
  `CheckedArtifact::prepare_parent`'s removal promise is unreachable — it
  keeps its v0 store caller permanently (`store/mod.rs:131` →
  `entry::prepare_merge_store_parents`).
- **E4.7 — narrowed by exactly the ruling's sentence, and no further.**
  The ruling re-scopes E4.7 by one sentence: it does not retire the
  capability-free arms. Everything else E4.7 owned SURVIVES: the A1-era
  allowance expiry — the class NAMED so E4.7's charter cannot miss it:
  E4.1 [P3-5]'s stale allows (`workspace_mutator_lock.rs:44-47`,
  `catalog.rs`), the `E4.2-E4.6`-range and "awaiting R2-E consumer
  conversion" reasons at `catalog.rs:19-22,27-29,34-36,46`,
  `coordinator/mod.rs:16`, `entry.rs:299-300`, `bootstrap/managed.rs:45-46`,
  `checked_artifact/mod.rs:12,18,29-31,37,44,50,59`, checker `:358` — each
  EXPIRES or is RE-REASONED AS PERMANENT, since some surfaces never gain
  the promised consumer; E0.2 §7.1's per-converted-consumer `finish()`-
  reachability record (`SemanticsAmendment-DRAFT.md:1235-1239`); the A-1
  rider's reopen-condition check (E7-Acceptance :179); the disposition of
  the dead `gc_archived` family (§5); and the phase close-out records.
  The LEGACY IN-PLACE-WRITER RETIREMENT (E4.7's first clause) is NOT
  dropped: it is RE-OWNED to DR-1 as O14's outcome — the legacy writer IS
  the pre-catalog `CheckedArtifact` that every converted path still rides,
  so its retirement is decided where the write-authority architecture is
  — and the retire-the-area question (OPEN-R1's resolution, RelocationPlan
  :78-81) travels with it. The records that say "E4.7 remains the stronger
  full-retirement mechanism" (plan :179/:193/:203; E7-Acceptance
  :170/:306), OPEN-R1's row (plan :213-216), E4.7's step (plan :531-534),
  RelocationPlan :78-81, `catalog_names.rs:44` ("still live until E4.7")
  and E7-Acceptance :179 each gain a dated bracket recording the re-own;
  the checkpoint entry carries one sentence making this consequential
  re-own visible to the operator. "O3 was DISCHARGED" (R1.3, 2026-09-01)
  stands as recorded and is NOT the ground for any of this; O3's
  discharge and E4.7's stronger mechanism were always distinct records.
- **RR's re-examination point** (RR §2 :130-134, §5 :203-206; plan O14
  :138-139, :145-151; plan E4.3 outcome :509-511) moves from "O14's fork
  at E4.6 chartering" to DR-1, because E4.6 as a conversion step
  dissolves under §7 — dated brackets at each. RR §4 :184-197 and the
  plan's gate-note pointer :373-384 gain the bracket that E4.4 as a bound
  step is gone and that the clause's option (c) — "the capability-free
  list amended at the amendment tier" — is CLOSED by the ruling; the
  clause otherwise survives, binding E4.5-B/E4.6-B. RR §6 :218-227 (the
  archive-files sentence, now inverted — no duty remains for E4.4's
  charter author to be protected from) and E7-Acceptance :180, :175,
  :308, plan :88-115 (both O13 brackets: ":90 empties across E4.2–E4.4",
  ":92 … E4.4's", ":110 empties … for the CONVERTIBLE files", ":114 the
  archive files E4.4's"), plan :493-494 (E4.3's C2 bracket) and the
  checker's O13 block `:321-344` + messages `:1200-1213` each gain the
  dated fact: the O13 inventory is now PERMANENT-DOCUMENTED in all three
  rows (creation converted at E4.2; rewrite carved by RR; archive carved
  here) and never empties.

## 5. Tier 2 deferred; O8's route dispositioned; DR-1 minted

**Tier 2.** The comparable sub-surface the E5 ruling requires "BY
AMENDMENT WITH DUAL REVIEW, not by the implementing step" is UNMINTED
(prep §4: five corpus hits, all saying it is owed; unsatisfiability
re-verified — `ArchivedMergeProjection`'s three fields, two differing by
construction). This amendment does not invent it. It is EXPLICITLY
DEFERRED: DR-1 mints the sub-surface BY AMENDMENT WITH ITS OWN DUAL and
names the execution carrier for the eight byte-preserved-v0-origin corpus
rows; no E4 step executes tier 2. The R2-F half of the encumbrance is
satisfied by the landed relocation (plan :170-174; E7-Acceptance :305).
The eight machine-enforced carrier strings in
`gwz-core/dev-docs/GwzM5-8I2CompatibilityPredicates.json` ("R2-E E4.4
terminal-archive consumer …") are re-pointed to DR-1 by the pins package
(a gwz-core edit, dated), with the record homes at plan :53 (O8 row),
:512-527 (E4.4 step), E7-Acceptance :175/:308, R4bG-Evidence :1200/:1264,
`SemanticsAmendment-DRAFT.md:1170-1175`.

**O8's `gc_archived` production route.** The standalone GC decode fix
(§7) routes deletion through the LIVE path (`store/gc.rs`, `retention.rs`)
and is NOT O8's route: the checked `gc_archived` / `gc_archived_with_hook`
/ `remove_archive` family behind `archive.rs:108-111` was built for a
converted archive, and with the archive carved it has no consumer to
arrive. Its disposition: E4.7's allowance expiry either deletes the family
or re-reasons its allowance as permanent-pending-DR-1; the route itself
RE-OWNS to DR-1, conditional on (C) resurrecting the archive conversion.
Homes: plan :53, :512-513; `SemanticsAmendment-DRAFT.md:1170-1175`;
`archive.rs:108-111`.

**DR-1, the phase-end design round — minted by the operator (ruling point
5), chartered here.** ONE design object with its own dual (the refined
tier policy's line, RR's precedent), opened AFTER the pins package and
E4.7 land, NOT inside E4. Agenda, recorded as [ruling items] plus
[lane-routed items with their hooks]:
- *Ruling items:* (C) the non-identity / degraded boundary mode — the
  product answer to "checked guarantees where the filesystem allows,
  ordinary operation everywhere"; reader-side record reconciliation (the
  record root's option (a)); O14's fork (route converted leaf writes
  through `authorize_write`/`RetainedWriteAuthorityV1`, or amend §8
  `:239-240` / §9 `:264-266`).
- *Lane-routed, with hooks:* the tier-2 comparable sub-surface (ruling
  point 4's "explicitly defer" needs a target; DR-1 is the only phase-end
  object); the record-root exception's re-examination (RR §2/§5 attach it
  to O14; it travels with O14); §6's already-shipped narrowing (a sub-item
  of (C), the only cure); the legacy in-place-writer retirement and the
  retire-the-area question (O14's outcome, §4); O8's checked-archive route
  (conditional on (C)).
- *Records:* the composite scope, including the E4.7 re-own AND the
  wider abort fact (the shipped abort is capability-free on fewer paths
  than the ruling's premise assumed — three checked-door paths, two
  independent of publication, §6), goes in the checkpoint entry as a
  SCOPING NOTE the operator reads (not a request);
  plan §2 :269-271 gains the dated bracket on the two-dual budget; plan §3
  gains a dated DR-1 row after E4.7 (:534) pointing here, and the
  checkpoint's NEXT line names it. One question is NAMED for the operator,
  not answered here: DR-1 opens after E4.7, outside Phase E4 and outside
  the plan's phase list — is it R2-E's (a new phase E8) or a new lane?

## 6. The abort sentence, settled from the tree — scoped BY PATH

The E4.1 remedy sentence and its class say abort is capability-free
unconditionally: `capability.rs:50` ("`--abort` never does") and `:54-55`
(the remedy string, "`gwz merge --abort`, which needs no such
filesystem"); `docs/OperationModel.md:131-132` ("An abort never asks"),
`:147-149` ("What never refuses: `gwz merge --abort`, with or without
`--preserve`, on a record of either version — so a `--no-ff` merge left
open on a workspace that later becomes incapable can always be cleared"),
`:158`; `v1_lifecycle/checked.rs:129-133`; `runtime/dispatch.rs:437-438,
447-448`; `v1_lifecycle/service.rs:112-115`; and the E4.1 release-notes
line, which has NO on-disk home (E4.1 [P3-1]: it lives in a commit message;
carrier the release train) — the corrected line is recorded below and the
pins package's delivery restates it verbatim for the train.

Settled from the tree (§1's three paths, each link read by both axes): a
v1 abort takes a checked door — and therefore the durable-identity probe —
whenever it must re-verify a checked artifact: published evidence
(post-publication, `abort/evidence.rs:148-165`), a preservation bundle
(`--preserve` with an integrated participant, `preservation/entry.rs:102-104`),
or a selected root's manifest/lock (`@root` selected,
`validate/journal.rs:253-258` → `root/abort.rs:131,145,380`). None of these
is gated on publication alone; the "pre-publication aborts need no such
filesystem" universal — inherited from RR §1b and adopted there on the
evidence chain's cites — is false for paths (ii) and (iii), and
`OperationModel.md:147-149` is false today. **Disposition:** the sentence
class is SCOPED BY PATH at every home: *"an abort that touches no checked
artifact needs no such filesystem; aborts that must re-verify checked
artifacts — preservation bundles, a selected root's manifest and lock, or
the merge's published evidence, re-verified through the checked boundary —
need persistent file handles and a mount identity."* Not "the same
filesystem the merge needed": the abort's doors go through the LEGACY
identity probe (`identity.rs:312-367`, `name_to_handle_at` + `statx` mount
id, no ext4 magic), which ADMITS btrfs/xfs/zfs where the catalog's
`require_ext4` (`provider/platform/linux.rs:136-151`) refuses — the
post-publication abort needs a STRICTLY WEAKER filesystem than the merge,
coinciding only on substrates without `name_to_handle_at`. And "the
merge's published evidence" not "own checked evidence": the forward path
wrote it raw (`finalization/execute.rs:45,48,51`). The narrowing is DATED
as a residual: shipped with A1's v1 reverse-path conversion; "never driven
on a non-admitted filesystem" holds by absence (E4.1's PR-3 drove the
lease, not these doors; no FAT32 lab; a Linux non-ext4 drive, if ever
available, would show these doors PASSING, not failing); cured only by
DR-1's (C). This is a record of an already-shipped fact reconciled with
§5.2's second clause (§2, reading (b)), not an amendment of the list's
operative text — the State axis's ruling.

## 7. What E4.4–E4.6 become — and the pins package's complete list

E4.4, E4.5 and E4.6 as originally chartered DO NOT START (ruling). No
E4.4 conversion candidate exists; the prep's recommended E4.4a/E4.4b
boundary shapes are SUPERSEDED (its header gains a dated status line). On
this amendment's GO:

**The pins package (E4.4-6-B), one reviewed step — Opus builder, interior
single-axis review. gwz-core side:** the generalized inventory (§3, changes
(i)-(iii)) with three-direction red-probes per row; the per-ARM negative
scans; one dated doc sentence at each carved site; the abort-sentence
class scoped at its six in-tree homes and OperationModel's three sentences
(§6) — and the remedy string's "ext4 only" clause plus
`OperationModel.md:139` reconciled so neither contradicts the new
by-path abort sentence at its own home (State axis round 2); the eight tier-2 carrier strings in
`GwzM5-8I2CompatibilityPredicates.json` re-pointed (§5); `archive.rs:108-111`
and `catalog_names.rs:44` re-reasoned; the `E4.2-E4.6`-range allowance
class annotated by name for E4.7 (checker `:358` included); the checker's
O13 block and messages re-annotated for three permanent rows; a SHARED
STRING-LITERAL MASKER for the negative scans (a NAMED OBLIGATION on this
package, E4.3-B review round 2's recommendation adopted: E4.3-B's
record-root tripwire strips `//` without string awareness and errs QUIET
on the absence half — a door on a line whose earlier `//` sits inside a
string literal is invisible; the pins package's thirteen scans and
E4.3-B's share one masker that blanks string contents before stripping,
the checker's `mask_non_code` idiom, and E4.3-B's scan is re-pointed at
it); E4.1's carriers dispositioned by name — [P3-C1]'s guard row ("`--abort` succeeds
end to end on an obstructed workspace", the drivable form) becomes the
package's pin that AN ABORT THAT TOUCHES NO CHECKED DOOR stays
capability-free (§6's by-path vocabulary — Code axis [R2-P3-2]), and [P3-8] closes (nothing
converts, no snapshot exclusion grows). **Root side (the lane owner's,
with the landing):** every home named in §4/§5/§6 — plan :15-16, :46, :53,
:88-115, :138-139, :145-151, :179/:193/:203, :213-216, :269-271, :331,
:333-340, :373-384, :454-459, :493-494, :509-534 (+ the DR-1 row);
ConsumerCheckpoint :275-279 (per-row), :280, :282-286;
`SemanticsAmendment-DRAFT.md:913-917` (the §2 reading bracket), :1170-1175,
:1235-1239; RR §2/§3/§4/§5/§6; E7-Acceptance :15-20 (pointer), :168, :170,
:175, :179, :180, :306, :308; R4bG-Evidence :1200/:1264; RelocationPlan
:78-81; the prep's header; this amendment's landing; the checkpoint entry
carrying the ruling, the tier recording, DR-1's composite scope as a
scoping note, and the E4.7 re-own sentence. Dated verdicts and reports are
left as written (house discipline).

**The convertible forward arms:** E4.5-B is VACUOUS for `:276` (no forward
bundle writer) and real for `:277` (`execute.rs:45`); E4.6-B is real for
`:278` (`execute.rs:48`) and `:279` (`execute.rs:51`, needing a
git-directory replace door in `entry.rs`). Three sites, one file, one
reachability proof (§1's chain) — CHARTERED as ONE small step (E4.5/6-B)
after the pins package, per the ruling's "convert only arms already on
`acquire_activated`" (an instruction, not an option), each arm proven
reached ONLY under `acquire_activated`; none may touch a carved arm; the
rows' cells the arms owe are §3's table. O1's re-scoped text therefore
lists these arms as CONVERTED; should E4.5/6-B fail its own loop
terminally, the arms fall to the [R2-P3-1] residual record and O1's text
is re-annotated — not silently.

**The GC decode fix (standalone, launched 2026-09-02 on the ruling):**
three v0-only decode sites on the live GC path — `gc.rs:188`
(`decode_production_v0` for `preflight_backup_artifacts`), `store/gc.rs:15`
→ `store/mod.rs:394`, and `store/retention.rs:8` (id-less path) — fail a
v1 envelope at `decode.rs:119-121` → `header.rs:159-163` (the `:128-133`
arm is the unreachable typed twin), so every completed `--no-ff` merge
since v0.11.0 is un-GC-able; the fix is read-side, adds no probe, routes
deletion through the live `store/gc.rs`/`retention.rs` writers, and does
not wire the dead `gc_archived` family (§5).

**E4.7 (re-scoped, §4):** the named allowance-expiry class, the surviving
duties, O1's close, the `gc_archived` family's disposition, the phase
close-out records; then DR-1.

E4.3-B (the record-root pins) is unaffected and lands on its own review;
its builder is told the `managed.rs:44-47` range wording and the
digest-backstop sentence (§3) so its landing is not stale on arrival.

*[Post-adoption corrections 2026-09-02, from the pins package's own
enumeration (E4.4-6-B builder, `b99bfb7`, each verified in the tree and
recorded here so §1/§3 are not read as exhaustive): (1) `:275`'s
`store/archive.rs` carries THREE raw `std::fs` mutations, not four
(`create_dir_all:61`, `remove_file:54,:77`). (2) A SECOND carved `:276`
home the tables missed: `workspace_ops/handle_stash/commands.rs` — six
`stash::write_bundle` calls under `guarded_workspace_root(StashMutate)`
(`handle_stash.rs:43`), the mutation guard's broad reading — pinned as
the inventory's 19th row. (3) Further carved sites cited by neither
cell: a third `write_lock` in `pull_head_member_preflight.rs:458` *[:457 as adopted; corrected 2026-09-02, the pins review [P3-7]]*, and
the `:278` manifest/lock writers (`write_manifest_and_lock`/`write_lock`)
in `handle_create_repo.rs`, `handle_materialize.rs`,
`handle_repo_lifecycle.rs`, `handle_branch.rs`,
`handle_init_from_sources.rs` — all under their guards or the bare
lock, all pinned. (4) Digest coverage, MEASURED: 3 of the 19 carved
files are pinned — `preserve/artifacts.rs` (flat), `v1_lifecycle/archive.rs`
and `store/archive.rs` (the `v1_lifecycle/mod.rs` tree root, which also
covers `store/rewrite.rs`); the other 16 are unpinned; the count map is
their only pin, as §3 chose. (5) The `_checked(` seam spelling is
UNUSABLE as a negative-scan needle (40+ production hits: `split_at_checked`,
`merge_upstream_checked`, `preflight_checked`, …), so the scans use
`checked_artifact` + `artifact_facts` and the git seam needs no
exclusion. (6) Two allowance-class members §4's list omits:
`interface_tests/catalog_activation_pin.rs:43,136` ("E4.2-E4.6") and
`src/git/tests/g12.rs:834` ("still live until E4.7") — E4.7's charter
sweeps for the STRINGS, not this enumeration. (7) Pin references are
counted with `#[cfg(test)]` modules dropped first (`stash/mod.rs` would
otherwise pin 11 references of which 8 are its own tests'), a stated
departure from O13's scan. (8) The "E4.3-B +1-line `--abort` content
pin" named in §7 does not exist; the remedy's content pins are
`interface_tests/contracts.rs:158,:181`, both kept green. (9) *[2026-09-02,
the pins review [P2-1], lane-owner ruling CONFIRMED by mechanism]* A
TWENTIETH carved row that §1, the E4.4 charter prep and both axes of this
amendment's dual all missed: `workspace_ops/merge/store/archived.rs::archive`
— row `:275`'s "`MergeArchive` when missing" bootstrap on the v0 path
(`create_dir_all:39`, `remove_file:47`, `rename_durable:49`, `sync_dir:51,:52`),
reached through `archive_merge_record` from ordinary v0 merge finalization
(`finalize_dispatch.rs:34`, `finalize_support.rs:99`) and BOTH abort forms
(`abort/mod.rs:111,:189,:218`) — the two operations E0.2 §5.2 names first. It
was pinned by nothing (no inventory row, no O13 row, no digest, no scan, no
annotation) while its two same-directory siblings `store/gc.rs` and
`store/retention.rs` had all four. The pins package's fold adds the row, the
`:275` site annotation, the `PURE_CARVED_FILES` entry and the new key-set
digest; §3's inventory is twenty rows, not nineteen, and (4)'s "3 of the 19"
reads "3 of the 20". The same review's [P3-5] adds a flat digest on
`artifact/mod.rs` (the `write_atomic` family's home), because converting the
family's implementation would convert every carved `:277`/`:278`/`:279`
caller while moving no count in any row.]*

*[E4.5-B OUTCOME 2026-09-02 (later), recorded pending the operator's
disposition — `GwzM5-8R2E-E45B-Report.md`, verified by the lane owner: the
build of `:45` STOPPED on its trigger. (i) Converting the marker write strands
`gwz merge --abort` after an interrupted checked publication — the residue's
authority is minted for the forward pair and the shipped reverse classifier
refuses it (`classification.rs:175-177` → `Ambiguous`; `abort/evidence.rs:306-311`
→ `Other`; the record enters recovery) — the charter prep's §2.4 hazard, which
its §2.5 table did not apply to `:45`. The cure class (abort-side observer
reconciliation) is what the ruling routes to DR-1. (ii) The disposition below
and §7's "one small step" rest on a FALSE premise: `gwz.conf/markers` is not
created by the raw marker write but by workspace creation
(`refresh_conf_integrity_marker`, `artifact/mod.rs:383,:392`), so a
refuse-when-missing conversion regresses no fresh workspace and the parent
bootstrap was unnecessary — and its prescribed placement is unbuildable
(`DurableMerge` needs a record-derived owner; `acquire_for_merge_start` runs
before `create_open`). The operator's packet (checkpoint, position 2026-09-02
E4.5-B) recommends `:45` join `:48`/`:51` in the [R2-P3-1] dated residual on
the directional-residue ground; §7's text is NOT rewritten until that ruling.]*

*[E4.5/6-B DISPOSITION 2026-09-02, from the read-only charter prep
(`GwzM5-8R2E-E45-6B-CharterPrep.md`, every claim cited at main `0dae0d5`; the third prep this
phase to falsify a premise before a build): reachability HOLDS — all
three `finalization/execute.rs` sites are reached under
`acquire_activated` and under nothing else (four joins re-driven). But
the durability half of §7's "one small step" premise fails: `:48` (lock)
and `:51` (boundary) are `Bytes → Bytes` replacements, so the boundary's
`replace_exact` DETACHES before publishing and the leaf is briefly
absent — an absence the shipped FORWARD observer refuses to classify
(`live.rs:110-112` `Ok(None)` → `Ambiguous` → RecoveryRequired) and the
shipped ABORT refuses too (`abort/evidence.rs:299-304` `classify_file`
missing-is-not-baseline → `Other` → the evidence preflight errors). That
is RR §1a's observation-dead window in the observers, on two more
leaves; the cure is observer-side reconciliation — DR-1's class (option
(a)'s), forbidden inside E4 by the ruling. THEREFORE `:48` and `:51`
STAY RAW as the DATED RESIDUAL this amendment's §3 provided for ([R2-P3-1]:
"a v1 FORWARD arm that stays raw … needs its own dated residual record"),
on the stronger ground: *the detach-then-publish shape opens an
observation-dead window on two leaves whose absence the shipped forward
and reverse observers both refuse to classify; the raw `write_atomic`
rename is atomic and opens none.* E4.6-B does not open as a build; its
in-tree residual sentence at `execute.rs:48,:51` rides E4.5-B's commit
(the same file); O1's re-scoped text reads these two arms as
"carved-as-v1-forward-arm residual", not "converted", per §7's own
clause. RECORDED WITH IT: row `:279`'s frozen cell 2 ("checked
Git-directory artifact action") ALREADY contradicts the shipped v1
REVERSE writer, which reconciles `.git/info/exclude` through the
WORKSPACE door (`abort/evidence.rs:148-153` → `boundary_relative` →
`entry.rs:159-166`, `CheckedArtifactPolicy::workspace`) — a discrepancy
predating E4; a git-directory replace door is feasible (the root kind
exists, `CheckedArtifact::acquire` is root-kind agnostic and already
acquires `<git-dir>/info/exclude` at `preservation_root/files.rs:28-35`)
but would be the first production write under `<git-dir>/gwz/`,
falsifying `policy.rs:41-45`, and would resolve a different path than
the reverse arm on linked worktrees — the cell-2 wording question goes
to DR-1 as a frozen-text item. `:45` (the marker) is CLEAR of the window
(`WriteMarker` issues only from the `Baseline` prefix, which requires
`marker_absent`, so the expected fact is `Missing` — the `MissingReplace`
shape that never detaches). Its parent, `gwz.conf/markers`, is a declared
managed purpose (`RootPreservationMarkers`) that NOTHING bootstraps on the
v1 forward path — E4.2's start session is sealed to `[MergeStore,
PreservationBundles]` and today's raw `write_atomic` creates the
directory by construction — so refuse-when-missing would regress a fresh
workspace's first `--no-ff` publication (the prep's stop-trigger 1
fires at charter time). E4.5-B is therefore CHARTERED AS ONE STEP in
E4.2's shape: bootstrap the marker's parent at merge start through a
`DurableMerge` door (`ManagedParentRequestAuthorityV1::DurableMerge`,
`managed.rs:132-147`, permits exactly `{PreservationBundles,
RootPreservationMarkers}`; zero production callers, no door today) in a
second admission/execution session under the activated start lease, then
convert `execute.rs:45` with refuse-when-missing now safe; cap 500;
stop-and-report if the freeze's "one reproducible pre-record request"
doctrine (`managed.rs:119-128`) forbids a second sealed start-time
request, or if the marker's expected fact is ever not `Missing`. It
sequences after the pins package and the GC fix land, on a main whose
inventory (which must NOT list `finalization/execute.rs` — verified: the
19 rows exclude it) and pins are settled.]*

*[E4.7 CHARTER CORRECTIONS 2026-09-02, from the read-only charter prep
(`GwzM5-8R2E-E47-CharterPrep.md`, every claim at main `0dae0d5`): (1) §4's allowance-class
NEEDLE SET misses its own §5 member — `v1_lifecycle/archive.rs:110`
(the `gc_archived` allowance) contains none of the prescribed strings,
and neither does its byte-identical, previously unnamed twin at
`merge/gc.rs:31-34`; the prep's corrected needle set governs E4.7's
sweep. (2) Six actionable allows no authority names, now in scope:
`coordinator/mod.rs:23-28`, `capability/pre_catalog/provider.rs:15,25,41`,
`namespace/mod.rs:20-23`, `merge/gc.rs:31-34` — and `namespace/mod.rs:10-19`,
whose own doc says "if E4 has not landed by the R2-E settle, E7 owes this
allow a dated re-owning rather than letting it become permanent by
silence": E7 never discharged it and E4 has now landed; E4.7 discharges
it. (3) Two allows ALREADY suppress nothing — `workspace_mutator_lock.rs:44-47`
(four production callers: `dispatch.rs:453`, `checked.rs:161,179,180`)
and `namespace/mod.rs:47-51` (both names consumed via
`bootstrap/managed/provider.rs:75-78`) — their removal will not fail
`cargo check`; `checked_artifact/mod.rs:27-32` is a third candidate
(all thirteen `entry.rs` doors have production callers) to be MEASURED,
not assumed. The full disposition: 23 actionable sites — 2 expire
(proven), 1 expiry candidate, 18 re-reasoned permanent, 2
delete-or-permanent — plus 12 prose/checker rewrites; 23 "A1
activation: reached only by this tree's own suites" allows swept and
dispositioned NO CHANGE. (4) E0.2 §7.1's `finish()`-reachability record:
the A-1 decision does NOT reopen — E4.1's activation and E4.2's parent
bootstrap take the checked retirement (`namespace_mutation.rs:301`),
E4.2's record creation and E4.5-B's marker arm reach `finish()`
(`entry.rs:390` → `transition.rs:106` → `cleanup.rs:15`), and
`cleanup.rs:146-158` runs unconditionally in `finish()`; no consumer
bypasses both. Cite drift: the reopen condition lives at E7-Acceptance
`:181` (the O12 row), not `:179` (now O10). (5) The dead `gc_archived`
family is SEVEN functions and one struct across two files
(`v1_lifecycle/archive.rs` + `merge/gc.rs`'s `preflight_archived_cleanup`
/ `delete_preflighted_backup_refs` / `require_backup_refs_absent` /
`PreparedArchivedCleanup`), not §5's three; deleting it drops
`archive.rs`'s `sync_dir` count 2 → 0 and fires the O13 inventory's
fail-closed SHRINKAGE arm — an amendment-tier event under §3 — and moves
the `v1_lifecycle/mod.rs` tree digest. E4.7 therefore KEEPS it with a
permanent-pending-DR-1 reason; the delete option is a named DR-1
sub-item. (6) SEAM SETTLED: the pins package (E4.4-6-B, `b99bfb7`)
already writes `archive.rs:108-111`, `catalog_names.rs:44` and the
checker's O13 block; E4.7 builds on that landed text and does not
re-write those three sites.]*

## 8. Review record

**Round 1 (2026-09-02), peer-blind, Fable×2.** Code axis: NO-GO — 2 P1
(the `:279` over-carve: `finalization/execute.rs:51` is reached ONLY under
`acquire_activated`, the conversion the ruling orders, foreclosed by the
draft's "no fork"; §6's publication-state scoping false for the
`--preserve` bundle door and the selected-`@root` metadata doors), 5 P2
(the abort's legacy probe strictly weaker than the catalog's; `:275`'s GC
writer a dead arm with the live writer and two further decode sites
elsewhere; the carved set under-enumerated and `:276` conflating a
converted writer; per-file negative scans unsound for mixed files and the
map needing three named changes; the digest backstop absent for 12 of 13
carved files), 7 P3 — all text folds, "the ruling's scoping is intact and
nothing returns to the operator on this axis". State axis:
GO-WITH-CONDITIONS — 1 P1 (the draft's §4 widened the ruling on E4.7,
dropping its legacy-writer clause and two surviving duties on a ground the
discharge records negate — narrowed above, the retirement RE-OWNED to
DR-1), 7 P2 (the §5.2 reading brackets; the rows' own cells; the primitive
classes; O1's text re-scope and sequencing; tier-2's carrier and O8's
route; DR-1's authority recording; the unowned RR/plan/E7-Acceptance
annotations), 8 P3; §6 ruled NOT list-amendment-by-stealth on two grounds
(the ruling's own "scope it … or date the residual"; §5.2's second clause
"a checked-artifact action refuses, typed"); DR-1's minting ruled within
the lane's routing authority as consequential routing, with the
"phase or new lane" question named for the operator; completeness sweep
50 hits → 34 unowned must-cure, now all named in §7. Every finding of both
axes is folded in this v2 (draft v1 preserved in session scratch).

**Round 2 (2026-09-02), Code axis — GO (terminal).** All six round-1
items confirmed folded with the folded sentence quoted; of 35
cite-strings new to v2, 26 re-spell round-1-verified cites and the nine
genuinely new tree cites were opened and verified; zero unverified-claim
flags. Two non-blocking notes folded above: [R2-P3-1] the `:280`
device does not reach a v1 forward arm that stays raw (its own residual
record if E4.5/6-B is not taken); [R2-P3-2] [P3-C1]'s guard row labeled
in §6's by-path vocabulary. Report `-ReviewCode.md` "## ROUND 2".

**Round 2 (2026-09-02), State axis — GO (terminal).** [P1-1], [P2-1],
[P2-2], [P2-3] and the extended §7 list each CONFIRMED with the folded
text quoted (34/34 sweep rows checked by home); [P2-4]–[P2-7] and the P3s
landed and waived on the fold record; §6 with three paths ruled STILL NOT
list-amendment-by-stealth (both grounds count-independent). One
mandatory landing fold — [P3-1] had regressed ("verbatim" over a
non-verbatim quote; now the frozen sentence itself, :915-917) — and three
precision notes, all folded above: E4.5/6-B's status made definite
(chartered, not "if taken", O1's text coherent); the checkpoint scoping
note to carry the wider abort fact; the remedy string's "ext4 only"
clause and `OperationModel.md:139` reconciled by the pins package.
Report `-ReviewState.md` "## ROUND 2". **THE DUAL IS COMPLETE; THIS
AMENDMENT IS ADOPTED.** It lands with the pins package and the root-side
records of §7.
