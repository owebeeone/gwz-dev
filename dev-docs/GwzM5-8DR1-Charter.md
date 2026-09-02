# GwzM5-8 DR-1 — the phase-end design round: charter (FILED 2026-09-03, opened on the operator's "run the DR to see what we can do to work-around the filesystem type checks")

Date: 2026-09-02 (draft). Author: the implementation lane. Status: DRAFT —
not opened; opens AFTER E4.7's phase close record lands, never inside E4
(operator ruling 2026-09-02, point 5: "Do not start (C) inside E4").
HOME: **R2-E phase E8** — the lane owner's default (2026-09-03): the operator
opened the round with "run the DR" without naming a home, after asking that the
question come back as one line; E8 keeps the plan, ledger and checkpoint in one
place. Moves to a new lane on the operator's word.

LEAD ITEM (the operator's ask, verbatim): "run the DR to see what we can do to
work-around the filesystem type checks." The first deliverable is
`GwzM5-8DR1-FilesystemIdentity-Design.md` — an inventory of every filesystem-type
and identity check, the consumers of durable identity, the ranked work-arounds
with sizes and the record each would amend, and the graded-evidence design
(identity by attempt, tiers recorded in the evidence, the straight line never
refusing, guarantees degrading in the classifier, the catalog bound to a
gwz-minted instance id). Its dual runs on the amendment-tier line, time-boxed per
the review-cost discipline (2026-09-02).

## 1. Object

ONE design round that decides, with evidence already on the record, the
architecture questions that four E4 steps surfaced and the ruling forbade
deciding piecemeal. Its outputs are design documents and amendments, each
with its own dual; it lands NO production code itself — implementation
steps are chartered from its adopted outputs.

## 2. Agenda — ruling items, then lane-routed items with their hooks

Ruling items (verbatim in substance):
- (C) THE NON-IDENTITY / DEGRADED BOUNDARY MODE — "checked guarantees where
  the filesystem allows, ordinary operation everywhere": a boundary path for
  substrates without durable identity. Inputs: E4.1's capability posture
  (ext4-only for the checked feature, ratified; ordinary ops capability-free,
  ruled); CapFree §6's three already-probing abort paths (the only cure);
  the E4.1 FAT32 refusal transcript (no FAT32 lab — evidence by citation).
- READER-SIDE RECORD RECONCILIATION — discovery/recovery learn to reconcile
  from the private-area residue. Inputs: the record-root window (RR §1a,
  driven at gwz-core probe/e4-3-detach-window-evidence c9a7303: discovery
  reports no open merge; abort cannot find it); the forward-observer window
  on the lock and boundary leaves (E45-6B prep §2: the shipped forward
  observer live.rs:110-112 and the abort's classify_file both refuse an
  absent leaf; the residue is content- and discovery-sufficient).
- O14's FORK — route converted leaf writes through authorize_write /
  RetainedWriteAuthorityV1, or amend §8 :239-240 / §9 :264-266 to the
  landed architecture. Inputs: E4.2 review flag 6 (zero production
  consumers; the type's own doc promises R2-E consumers take it; §9 names
  path-writing as NOT parent authority); the C1(iii) gate statements at
  every converted seam (opened = durability; closed = authority).

Lane-routed items (hooks named):
- THE TIER-2 COMPARABLE SUB-SURFACE (ruling point 4 "explicitly defer"):
  mint it BY AMENDMENT WITH ITS OWN DUAL, name the execution carrier for the
  eight byte-preserved-v0-origin corpus rows. Inputs: E4.4 prep §4
  (ArchivedMergeProjection's three fields, two differing by construction —
  unsatisfiable as written); the E5 ruling's "not by the implementing step".
- THE RECORD-ROOT EXCEPTION's re-examination (RR §2/§5, rides O14): if leaf
  writes move onto the write authority with reconciliation, the record root
  converts or the exception is re-affirmed, deliberately.
- THE §6 NARROWING (sub-item of (C)): the shipped abort is capability-free
  exactly when it touches no checked door; only (C) widens that.
- THE LEGACY IN-PLACE-WRITER RETIREMENT (O14's outcome; CapFree §4): the
  legacy writer IS the pre-catalog CheckedArtifact every converted path
  rides; OPEN-R1's retire-the-area question travels with it (RelocationPlan
  §2).
- O8's CHECKED-ARCHIVE ROUTE, conditional on (C): the dead gc_archived
  family (7 functions + 1 struct across v1_lifecycle/archive.rs and
  merge/gc.rs; kept permanent-pending-DR-1 by E4.7; deletion fires O13's
  shrinkage arm — the delete option is a named sub-item here).
- ROW :279's CELL-2 WORDING ("checked Git-directory artifact action") vs the
  shipped reverse writer's WORKSPACE door (E45-6B prep §3): amend the cell
  or re-route the writer — frozen text either way. Inputs: policy.rs:41-45
  (no production write under <git-dir>/gwz/ today); linked-worktree path
  resolution.
- THE FORWARD-ARM RESIDUAL (:48/:51, CapFree [R2-P3-1]): converts only with
  the reader-side reconciliation above; else stays raw permanently.

## 3. Constraints the round inherits (binding)

- The capability-free list STANDS (ruling; (A) closed): ordinary merge /
  commit / create / abort / GC never take the catalog probe. (C) must not
  regress ordinary operation on any filesystem.
- ext4-only is the CHECKED feature's Linux posture, ratified "for now".
- No FAT32 in product or lab.
- Frozen-text moves (the §10 cells, §8/§9, the 165-key census, §5.2) need
  their own dual; the design round proposes, the dual adopts.
- The three-strikes lesson: every load-bearing conclusion verified against
  the tree's MECHANISM before it is written (four authority premises were
  falsified by charter preps this phase).

## 4. Deliverables

1. A design document for (C) with the reconciliation design as its core —
   the two are one architecture (a boundary that can reconcile from residue
   is a boundary that can degrade honestly).
2. The O14 decision recorded as an amendment to §8/§9 (or the conversion
   plan for authorize_write consumers), with dual.
3. The tier-2 sub-surface amendment, with dual, naming the carrier.
4. The legacy-writer retirement plan (or its deliberate permanence), with
   OPEN-R1's area question answered.
5. Dispositions for the archive route, the :279 cell, the forward-arm
   residual, the record-root exception.
6. The implementation steps chartered from the above, each <500 LOC per the
   house cap, each with the lease-reachability enumeration and the
   record-root analysis in its charter BEFORE the build.

## 5. Sequencing and stop conditions

Opens only after E4.7's close record lands. Terminal NO-GO on any dual, or a
scoping different from this charter, returns to the operator. The round's
outputs are adopted one at a time; nothing implements ahead of adoption.


## Agenda additions from the operator's E4.5-B ruling (a), 2026-09-02 (verbatim: "DR-1's agenda gains: directional-residue class, classifier widening, preservation-bundle audit (same hazard)")

- **The directional-residue class.** An interrupted checked publication leaves a forward-pair authority record (`Missing → Replace`) that the abort's reverse-pair `classify_remove` (`Bytes → Missing`) reads as `Ambiguous` (`classification.rs:175-177`), which `abort/evidence.rs` maps to `Other`, stranding `gwz merge --abort` in `RecoveryRequired` (then `RecoveryEvidenceMismatch` / `MergePhaseUnsupported`). Measured and ablation-proven at `f563446` (`GwzM5-8R2E-E45B-Report.md`). Cure class: abort-side observer reconciliation in `abort/evidence.rs::file_states` (charter prep §2.2 cure (ii) / §7.1 option (i), 450–600 LOC, plain lease) — to be designed ONCE with the observation-dead-window class (record root, lock, boundary), not per leaf.
- **Classifier widening.** `artifact_facts::classify_write` and `classify_remove` cannot express a `Missing` expected fact (E4.5-B report flag 7: on the interrupted marker `classify_write` returns `Ambiguous`); any observer cure must widen the classify side.
- **Preservation-bundle audit.** The preservation-bundle leaves also carry checked writers on more than one arm (E4.5-B report flag 9) — audit them for the same directional-residue hazard before any further conversion is chartered.
- **Fixture note for any re-charter** (flag 10): `v1_lifecycle/tests/finalization.rs::fixture` and six sibling fixtures seed `gwz.conf` but not `gwz.conf/markers`.


## Agenda additions from E4.7's close (2026-09-02; `GwzM5-8R2E-E4.7-Review.md` §12 C7, P3-4, P3-5, flag 5)

- **Two A1-era allowances no adopted record owns**, swept at E4.7 and left dated: `workspace_ops/merge/store/archived.rs:3-9` (MEASURED REDUNDANT — removal compiles clippy-clean) and `workspace_ops/merge/store/mod.rs:44-50` (MEASURED LOAD-BEARING — `load_archived` has no production caller). Decide: expire the first; retire or consume `load_archived` for the second.
- **`protocol/authority_record.rs:50-53`** — an R2-D-era forward reference to a completed step ("plan §4 Step 3.3 wires its production consumer"; Step 3.3 landed without wiring it). Adjacent to E4.7's class, not a member; the next sweep should not rediscover it.
- **The `CatalogOwnerV1` re-export narrowing** at `checked_artifact/catalog.rs:44-48` — the E4.7 charter prep's option (b); declined at E4.7 as a surface change outside "allows, reasons, comments only" (the allow bites on exactly that one name). A DR-1 sub-item, not an E4 act.
- **The `gc_archived` family's measured extent** (eleven functions + two structs across `v1_lifecycle/archive.rs` and `merge/gc.rs`, plus four family-only members of `ValidatedArchivedMerge` in `archive_result.rs`) — the size DR-1's "delete or rebuild against (C)" decision is made from; deletion fires O13's shrinkage arm (amendment-tier).
- **A stale inventory-row description**: `check_checked_artifact_boundaries.py` `CAPABILITY_FREE_RAW_WRITER_INVENTORY`'s `v1_lifecycle/archive.rs` row still says "behind the `:108-111` allowance" (the allow now sits at `:135-144`); the key-set digest hashes keys only, so the fix moves no pin — the next pins package or DR-1 takes it.
- **The four inert-under-blanket allows** (`bootstrap/managed.rs`, `pre_catalog/provider.rs` ×2, `namespace/mod.rs`) with their measured item counts (3 / 21 / 4 / 43): when `checked_artifact/mod.rs`'s blanket allows are lifted, these become load-bearing by exactly those sets.
