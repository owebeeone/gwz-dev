# GwzM5-8 R2-E Phase E4 Close — the §10 conversion train, settled by carve-out

**Object.** Phase E4 of the R2-E plan (`GwzM5-8R2E-Plan.md` §E4): the §10
conversion train — each row of the R2-D conversion table (`:273`–`:280`) onto
the checked artifact boundary — as re-scoped by two adopted amendments and five
operator rulings. This record closes the phase in the E7-Acceptance form
(`GwzM5-8R2E-E7-Acceptance.md`): the step ledger, the residual register, the
carve-out and pin census, the matrix acceptance per landing, the record acts,
and the carriers leaving the phase. Written by the lane owner from the landed
tree and the dated records; every claim cites its record.

**Framing statement.** Phase E4 CONVERTS exactly two things — the catalog
activation (E4.1) and the first merge record (E4.2). Everything else the
train was chartered to convert is, at the close, one of: CARVED OUT under the
capability-free list (twenty raw writers, documented and pinned), DATED
RESIDUAL on a measured ground (the record root; the three forward arms of
`finalization/execute.rs`), or DISSOLVED (E4.4 as chartered; E4.6-B). The
one shipped defect found on the way (v1 archives un-GC-able since 0.11.0) is
fixed standalone and read-side. Operator ruling, 2026-09-02, verbatim: "Phase
E4 conversions are E4.1 and E4.2; the rest is carve-out, pins, GC, and
close-out."

## 1. The step ledger (gwz-core main, in landing order)

| Step | Outcome | gwz-core | Review (interior, Opus, single-axis unless stated) | Matrices at the landing |
| --- | --- | --- | --- | --- |
| E4.1 — catalog activation (`activate_workspace_catalog`; the adapter viability window; the lease split) | LANDED | `e56124b`; CRLF hotfix `f715ddf` | `GwzM5-8R2E-E4.1-Review.md`: round 1 NO-GO ([P1] the refusal-behind-upgrade wedge) → lane ruling R1–R7 (adapter viability window declines the upgrade; abort capability-free by lease split) → GO-WC; cap re-ruled 300→331 | Windows `33498089904` + Platform `33498092726` (Windows red on a CRLF needle → hotfix `f715ddf` → redispatch `33502328880` GREEN) |
| E4.2 — the first merge record (`:273`, `MergeStore`; `acquire_for_merge_start`; the managed-parent session) | LANDED; O14 MINTED | `7f28907` | `GwzM5-8R2E-E4.2-Review.md`: GO-WC; round-2 [P2-R2-1] the reviewer's own conflation reverted (`3717249`) | Windows `33511535235` + Platform `33511538719` GREEN, sums confirmed to the digit |
| E4.3 — the merge-record rewrite (`store/rewrite.rs` onto the boundary) | REJECTED at delivery by its own builder's driven P0 (candidate `c9a7303`, preserved on `probe/e4-3-detach-window-evidence`): the detach-then-publish window makes the open record vanish from discovery with no reconciler; the shared commit put the identity probe on every abort | — | Decision packet → operator ruling **"proceed with (c)"** → `GwzM5-8R2E-RecordRootAmendment.md` (Fable×2 dual Code+State, both GO-WC, folds executed) | — |
| E4.3-B — the record-root pins (P-1 the permanent O13 row fail-closed both ways; P-2 the negative tripwire) | LANDED, no production conversion | `0dae0d5` (E4.3-B `60072a7` + fold `2b0eed9` + landing nit) | `GwzM5-8R2E-E4.3B-Review.md`: GO-WC (P1 = the lane owner's own false digest claim folded into adopted text — withdrawn with dated notes in both amendments) | Platform `33562252461` + Windows `33562249876` GREEN |
| E4.4 — terminal archive source-only | DISSOLVED as chartered (charter prep `GwzM5-8R2E-E4.4-CharterPrep.md`: every archive writer runs from a listed operation on the plain lease; `remove_archive` dead) → the structural wall named: the R2-D conversion table vs E0.2 §5.2's capability-free list | — | Decision packet (A/B/C/D) → operator ruling **(D)** (`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`) → `GwzM5-8R2E-CapabilityFreeAmendment.md` (Fable×2 dual, ADOPTED) | — |
| GC fix (standalone, ordered by the ruling) — v1 archives un-GC-able since 0.11.0: three v0-only decode sites cured through `record_wire::decode_archived_common` | LANDED, read-side, GC capability-free (verified by chain) | `3c632ec` (`0dccd3e`/`c39f6d4`/`b5ff7fc`/fold) | `GwzM5-8R2E-GcArchiveDecode-Review.md`: round 1 GO-WC ([P2-1] the unpinned retention site, masked by a `cfg(test)` twin — DELETED, never shipped) → round 2 GO-WC (three message/comment P3s folded at landing) | Windows `33569807177` + Platform `33569810180`; push CI `33569807827`/`33569807792` — all GREEN |
| E4.4-6-B — the pins package (charter = the amendment's §7): the twenty-row `CAPABILITY_FREE_RAW_WRITER_INVENTORY`, key-set digest, per-ARM negative scans, carved-site annotations, the abort sentence re-scoped by path at ten homes, eight JSON carriers, allowance annotations, the faithful shared masker | LANDED, no production conversion | `f563446` (`9fbe4ac`/`a7e5e21`/fold) | `GwzM5-8R2E-E4.4-6B-Review.md`: round 1 NO-GO (P1 the first masker's quote-state leak — 178 live blinded lines, the E4.3 door invisible; P2 the twentieth row `store/archived.rs`; P2 the §6 restatement) → lane ruling (§12) → round 2 GO-WC (§13: zero blinded lines on a 411-file differential; 20/20; 25 digests) | Windows `33575785509` + Platform `33575787725`; push CI `33575785383`/`33575785323` — all GREEN |
| E4.5-B — the marker write `execute.rs:45` + the parent bootstrap | STOPPED on trigger (5), no commit (`GwzM5-8R2E-E45B-Report.md`): the conversion strands `gwz merge --abort` after an interrupted checked publication (directional residue); the parent premise false; the bootstrap placement unbuildable | — | Lane-owner mechanism verification (row re-run FAILED `RecoveryRequired` vs `Aborted`; `classification.rs:175-177` → `abort/evidence.rs` → `Other`) → decision packet → operator ruling **(a)** (verbatim in the checkpoint, the plan and the amendment's §7 bracket) | — |
| E4.6-B — lock `:48` + boundary `:51` | NOT OPENED as a build (charter prep `GwzM5-8R2E-E45-6B-CharterPrep.md`: both hit the observation-dead window in the forward and abort observers) | — | Dated residual ([R2-P3-1] form), recorded in the amendment's §7 | — |
| E4.7 — the close-out (allowance-class expiry/re-reasoning; the three residual sentences; the amendment corrections' in-tree echoes; the finish()-reachability record) | LANDED — docs and allowances only: masked of comments and string contents the whole change is six removed attributes and zero added code lines (review §2) | `9c808ff` (E4.7 `d8c140f` + the landing fold `9c808ff`) | `GwzM5-8R2E-E4.7-Review.md`: GO-WC — no P1; one P2 (the four inert-allow numbers were net clippy diagnostic-count deltas written as item counts — measured 3 / 21 / 4 / 43) and seven P3, all text-only, all folded at landing (C1–C6 in `9c808ff`, C7 in this record) | Windows `33596394193` + Platform `33596396157`; push CI boundary `33596394533`, retained readers `33596394463` — dispatched at the landing; closure appended below |

## 2. The residual register (every arm that stays raw, with its dated ground)

| Site | Row | Ground | Record | Pin |
| --- | --- | --- | --- | --- |
| `store/rewrite.rs` (`commit` keeps `rename_durable`) | `:280` | RECORD-ROOT EXCEPTION: the boundary's detach-then-publish replacement opens a discovery-dead window for the one leaf that is the root of reconciliation | `GwzM5-8R2E-RecordRootAmendment.md` §1–§3; operator "(c)" | O13 PERMANENT row (fail-closed both ways) + P-2 tripwire + `v1_lifecycle/mod.rs` tree digest + `entry.rs` byte pin |
| `finalization/execute.rs:48` (lock), `:51` (boundary) | `:278`, `:279` forward arms | OBSERVATION-DEAD WINDOW: `Bytes → Bytes` replacement detaches before publishing; the shipped forward (`live.rs`) and abort (`abort/evidence.rs classify_file`) observers refuse to classify the absence | Amendment §7 E4.5/6-B disposition ([R2-P3-1] form); `GwzM5-8R2E-E45-6B-CharterPrep.md` §2 | The three residual sentences (E4.7) |
| `finalization/execute.rs:45` (marker) | `:277` forward arm | DIRECTIONAL-RESIDUE WINDOW: an interrupted checked publication mints a forward-pair authority record the reverse-pair `classify_remove` reads as `Ambiguous` → `Other` → `RecoveryRequired`, stranding `gwz merge --abort` | `GwzM5-8R2E-E45B-Report.md`; operator ruling (a) | The three residual sentences (E4.7) |
| The twenty carved raw writers of rows `:275`–`:279` (listed-operation writers: ordinary merge, commit, `repo create`, `init-from-sources`, abort, GC, the mutation guard's operations) | `:275`–`:279` | CAPABILITY-FREE LIST STANDS (E0.2 §5.2, read broadly); operator ruling (D)/(A) | `GwzM5-8R2E-CapabilityFreeAmendment.md` §3, §7 (+ brackets (1)–(9)) | `CAPABILITY_FREE_RAW_WRITER_INVENTORY` (20 rows, key-set digest, growth/shrinkage/file-gone/set-moved arms) + per-ARM negative scans + carved-site annotations |

## 3. Census at the close

- **Conversions landed:** 2 (E4.1, E4.2). **Doors in `entry.rs`:** `activate_workspace_catalog`, `bootstrap_merge_start_parents`, `create_merge_store_record`, `render_catalog_refusal`; `rewrite_merge_store_record` exists only on the probe branch and is pinned ABSENT.
- **Production catalog activation sites:** 1 (`PRODUCTION_CALLER_COUNT`, `catalog_activation_pin.rs`).
- **Carved raw writers pinned:** 20 rows, three primitive classes (`durable_fs` / `std::fs` / the `write_atomic` family), key-set digest `867c580f…`; flat digests on `artifact/mod.rs` and `checked_artifact/capability.rs` added as backstops.
- **O13 (`V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES`):** three rows, counts unchanged through E4.7 (`archive.rs` `sync_dir: 2` is the `gc_archived` KEEP); three permanent exceptions incl. the record-root row; the `gc_archived` family (seven functions + one struct across `v1_lifecycle/archive.rs` and `merge/gc.rs`) KEPT permanent-pending-DR-1 — deletion fires O13's shrinkage arm (amendment-tier), the delete option is a named DR-1 sub-item.
- **Partitions at the close tree** (`--list`-verified snapshot, 1839 tests, unchanged by E4.7 — a docs+allowances step; the aggregate driver is not edited): checked_artifact:: 457 (467 linux), v1_lifecycle:: 266, lib remainder 1114 + 1 ignored (1115 linux), g23 130.
- **Protected digests:** 18 flat + 7 tree; E4.7 re-pins six deliberately (flat `checked_artifact/entry.rs`, `checked_artifact/mod.rs`, `operation/workspace_mutator_lock.rs`; tree `checked_artifact/catalog.rs`, `checked_artifact/capability/pre_catalog.rs`, `workspace_ops/merge/v1_lifecycle/mod.rs`), all comment/`reason`-only edits, dated provenance in the checker; every other digest unchanged.
- **Allowance class (E4.7, `d8c140f` + fold `9c808ff`, review-verified):** 171 `allow`/`cfg_attr` attributes swept in `src/`; 23 Tier-3 ("A1 activation: reached only by this tree's own suites") unchanged; 24 actionable = 6 EXPIRED (each proven by removal + `cargo check --all-targets` + `clippy -D warnings` green: `workspace_mutator_lock.rs`, `checked_artifact/mod.rs` `mod entry`, `namespace/mod.rs:47-51`, `observation.rs` `parent_is_canonical`, `bootstrap/managed.rs` `use provider::*`, `provider.rs` `mod leaf_observation`), 16 RE-REASONED PERMANENT pending DR-1 (each proven to bite, four of them inert under an ancestor blanket with the measured delta written into the reason), 2 KEPT permanent-pending-DR-1 (the `gc_archived` family); 12 prose/comment/checker rewrites. **The finish()-reachability record** (`checked_artifact/cleanup.rs:146-200`, above the unmoved `:201` recheck): E4.1/E4.2's parent half — `finish()` NOT reachable, checked retirement taken; E4.2's record half (`entry.rs:394` → `transition.rs:105-106`) — `finish()` IS reachable, the compensating observation runs; the marker arm's row VACATED by ruling (a); **A-1's reopen condition CHECKED and NOT MET — DECISION A-1 STANDS** (E7-Acceptance's O12 row is `:181`). Two "A1 activation review" allowances at `merge/store/archived.rs:3-9` and `merge/store/mod.rs:44-50` are OWNED BY NO ADOPTED RECORD and left untouched under the builder's stop-trigger (5) — routed to DR-1.

## 4. Operator rulings governing the phase (all verbatim on disk)

1. "ext4-only is fine for now — continue the loop" (2026-09-01; the Linux catalog posture).
2. The E4 standing order (2026-09-01): GO lands; GO-WC lands after lane-owner folds + focused verification; round-1 NO-GO → the lane owner rules and remediates within the two-round cap; terminal NO-GO or a different scoping returns to the operator; each landing auto-launches the next step.
3. "proceed with (c)" (E4.3 → the record-root carve-out).
4. (D)/(A) — `GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md` (the list stands; FAT32 out; the GC fix standalone; E4.4–E4.6 not as chartered; DR-1 minted; "do not start (C) inside E4").
5. (a) on E4.5-B (2026-09-02): the marker joins the dated residual; E4.7 carries the sentences; DR-1's agenda gains the directional-residue class, classifier widening, preservation-bundle audit; "Phase E4 conversions are E4.1 and E4.2".

## 5. Record acts at this close

- The E4.7 landing's root record (member pin gwz-core → the close tree) and this document, `GwzM5-8R2E-E4-Close.md`.
- The plan's Phase E4 line annotated CLOSED; the checkpoint position entry; the E7-Acceptance O13/O1 brackets re-pointed at this close.
- **C7 of the E4.7 review, discharged here:** (i) `protocol/authority_record.rs:50-53` — an R2-D-era forward reference to a completed step, adjacent to E4.7's class, routed to DR-1's sweep; (ii) the `CatalogOwnerV1` re-export narrowing (`catalog.rs:44-48`, the prep's option (b)) DECLINED at E4.7 as a surface change and named a DR-1 sub-item; (iii) the E4.7 review's flag-8 observation — the `ENTRY_REFERENCES` correction was made by an APPENDED dated bracket (the amendment's mechanism), with one in-place edit at `checked_artifact/mod.rs:10` noted by the review's §10; (iv) the checker's inventory-row description still citing `:108-111` (routed, not edited: the brief barred inventory rows; the key-set digest hashes keys only).
- The E4.7 landing fold (`9c808ff`): the review's C1–C6 folded text-only — the four inert-allow numbers restated as measured item counts (3 / 21 / 4 / 43) with the net-delta explanation; the `gc_archived` extent corrected; the `:108-111` cite re-pointed; the inertness qualifier in each `reason=`; dated SWEPT lines at the two A1-era allowances; the residual sentences' wording and full paths.

## 6. Carriers leaving the phase

- **DR-1** (the phase-end design round; opens AFTER this close; home = the operator's one-line answer, R2-E E8 vs a new lane): the two `A1 activation review` allowances no record owns (`store/archived.rs:3-9`, `store/mod.rs:44-50`); "delete the `gc_archived` family, or rebuild it against (C)'s boundary"; (C) the non-identity / degraded boundary mode; reader-side record reconciliation; O14's write-authority fork; the tier-2 archive sub-surface (deferred explicitly); the record-root re-examination; the legacy in-place-writer retirement; O8's `gc_archived` route conditional on (C); row `:279`'s frozen cell-2 discrepancy; **the directional-residue class** (abort-side observer reconciliation, designed once for marker, lock, boundary and record root); **the classifier widening** (`classify_write`/`classify_remove` Missing-expected); **the preservation-bundle audit** (same hazard); the fixture seeding note. Draft charter: the lane owner's, staged.
- **Release train:** the GC fix's release-notes line (`b5ff7fc`); the corrected abort sentence (the pins fold's message; on-disk home the plan's E4.1 rider).
- **Lessons filed** (process): verify the mechanism before folding any reviewer claim, positive or negative; snapshot-binary protocol whenever a target is shared; CRLF-normalize every multi-line source needle; a charter prep's own hazard section must be applied to every row of its summary table (E4.3's clause pair; E4.5-B's §2.4 vs §2.5); write stop-triggers as numbered separate conditions.

## 7. Close

**PHASE E4 IS CLOSED — 2026-09-02.** The close tree is gwz-core main `9c808ff`
(E4.7 + its landing fold). Landing verification at the close: fmt / clippy
`-D warnings` / boundary checker green at `9c808ff`; the per-commit lane gate
ok at `d8c140f` and `9c808ff`; M4 map and merge-docs green; the `--list`
test count 1839 unchanged from `f563446`, and the review's masked diff proves
the change adds no code line — so the lane owner did NOT re-run the four
partitions for this landing (the review-cost discipline, 2026-09-02); the
three-platform matrices dispatched at the landing are the belt. The two
conversions, the carve-out, the pins, the GC fix and this close-out are all on
main; every residual is dated and grounded; every carrier is named in §6.
DR-1 does not start until the operator names its home (one-line question:
R2-E phase E8, or a new lane).

*[CI CLOSURE: appended when the four runs complete.]*
