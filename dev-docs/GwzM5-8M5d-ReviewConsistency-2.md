# GwzM5-8M5d — CONSISTENCY-AXIS REVIEW

**Review object:** working-tree file `dev-docs/GwzM5-8M5d-Charter.md` revision 2 (UNTRACKED; CHARTERED 2026-09-03; SHA-256 `38b7d630620899487c388c96c88b2f57d2a6e39e1b8454d47470dd702afaa624`). Controlling DRAFT document: the object itself. Supersession trail in scope: working-tree `dev-docs/GwzM5-8M5c-Charter.md` status banner / §0 heading.
**Baseline:** gwz-dev HEAD `3d0740a3c86f5f31a7bd734a0227a34887e1db98` (verified at start and end). gwz-core HEAD `57502e461055809503ca9dc0d7cb791b8b332f6c` (locked; verified at end). Object bytes read from the working tree. I2.md body not rewritten (amendment text is charter §10; I2.md waits for the close dual, as authorized). Tuple did not move.
**Date:** 2026-09-03
**Axis:** CONSISTENCY — the document against its controlling graph. Round-2 re-verdict of this axis’s round-1 findings against RemPlan dispositions. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on its report (RemPlan IDs are legitimate round-2 input). Filed verbatim by the lane owner.

**Verdict: GO** — 0 P0 / 0 P1 / 0 P2 open. Round-1 P1-1 and P2-1…P2-4 are closed on the corrected bytes. Two new P3 residuals (not architectural). No new architectural root cause.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| **P1-1** | Strike “can land before the floor”; production §2 refuse and floor raise are one reviewed change; unpublished intermediates still abort v0 | Original walk (land production refuse while `version.rs:39` is `V0` → ordinary start writes v0 → continue/abort refused) is **unconstructable as a published `main` tree**. §9 heading + lines 287–300: (1)–(4) are not independently published to `main`; refuse helper may be test-only; “Do **not** publish the production §2 refuse while `ACTIVE_WRITER_FLOOR` is `V0`”; “ordinary start then `--abort` still aborts a v0 record.” §7 lines 254–259 and §1 lines 62–66 agree. | **CLOSED** |
| **P2-1** | Bounded I2 amendment block: quote superseded and retained sentences; list §1:36, §2:102–103, annotation, §8:289, §9:304–305, predicates JSON | §10 exists. Quotes checked against I2 HEAD at the cited clauses (wrap-normalized): §1:36, §2:102–103, §5:180, §5:183–184 `UnsupportedLegacyMode`, §8:289, §9:304–305 are verbatim. §4 entire is deleted as a production path (not “§4–§5 as wholes” with archive sentences unsaved). §5:181–182 archive archival is **retained**. Predicates JSON is in §7 and §10.3. I2 banner trail is specified at close. Original counterexample (I2 patch that keeps §2:102–103 / §1:36 / adapter / eligible migration, or a §4–§5 wholesale rewrite that drops archive archival) cannot be produced from §10. | **CLOSED** (P3-2: §5:180 replacement column still carries a pre-close sequencing sentence; see §1) |
| **P2-2** | Replace “no production v0 merge type” with no v0 **lifecycle engine**; archive decoder remains | Original fork (delete `RecordVersion::V0`/archive decoder to satisfy §8 vs keep I2 §7) is gone. §1:42–45, §2:102–108, §5:216–218, §8:274–281: engine vs I2 §7 decoder are disjoint; §8 “is checked off without deleting it, and cannot be read as requiring that deletion.” Named archived-v0 fixture still projects; GC never deletes unread/unreadable archives. | **CLOSED** |
| **P2-3** | Scope M5c supersession; restatements in §3–§4 control; stop “kept”; §7.2 formula on M5c; §0–§6 not current authority | Original fork (M5d §4 thin kinds-only vs M5c §2.2 “kept”) is gone. Object banner lines 5–9 and §4:176–177: restatements control; “not ‘M5c kept.’” §4 events now include before observation/apply, `MergeRepoSummary`, ordering discipline, per-participant counts (matches M5c §2.2). M5c WT banner uses the §7.2 formula; §0 heading is “Historical: operator decisions that **bound the superseded** M5c charter”; “§0–§6 below are **not current authority**.” Dropping M5c would not change M5d §3–§4 meaning for those surfaces. | **CLOSED** |
| **P2-4** | Name CapabilityFree §3 / inventory and F-3 seam floor as affected; deletion paired with those revisions | Original walk (delete `finalize.rs` / `persist_merge_record` with no named pin) now hits §7 rows and §4:202–212 / §9.4: F-3 floor is redefined (or a test-only seam remains); CapabilityFree carved-set revision is in the S1 dual; relocating `write_atomic_verified` is “not sufficient.” | **CLOSED** (P3-1: the “seven v0 rows out” forecast is the wrong count; see §1) |
| **P3-1** | §1: starts and can complete the forward path; reverse doors refuse | §1:54–58. Matches §3 reverse-door product. | **CLOSED** |
| **P3-2** | DR-1 ship-2 pointer and A1 carrier retarget to M5d | §7 table: A1 dated note retargets carrier name, does not rewrite historical landing; Ship (1) **mechanism** unchanged, ship-2 **pointer** retargets. | **CLOSED** |
| **P3-3** | Name `transactional-apis.mdc` excepted for crash arm only | §3:169–172 and §7 row. Explicitly does not except live abort. | **CLOSED** |

---

## Changed-range analysis

The charter was rewritten in place (still untracked). M5c working-tree change is banner + §0 heading only (in scope as the supersession trail). I2.md body unchanged, as authorized.

In-disposition (RemPlan take + non-blocking folds, including Safety IDs as they now appear in this object):

- §9 is implementation order inside one unpublished branch / one landing commit; production refuse is bound to (4).
- Engine vs archive decoder split throughout §1/§2/§5/§8.
- §10 L1-08 table; §7 affected-interface rows (predicates JSON, CapabilityFree, F-3, transactional-apis, DR-1 pointer, A1 carrier).
- M5c §7.2 banner; event/response/`stage` restated in §4 without “kept.”
- Reverse-door product (participant-only abort clears; selected-root/`--preserve` new escape; start diagnostic states the limit; `v0.13.0` not an escape for open **v1**).
- Open v0 as third occupancy; `v0.13.0` pin as precondition of engine deletion; mutation gate / commit / add / stage refuse without idle.

**Outside dispositions:** none of the new sections are unexplained extras. The reverse-door escape path `.gwz/merge/<merge-id>.yaml` matches live `store/persistence.rs:44` and `store/mod.rs:30,311`. Not a new defect.

**New-root-cause candidates:** (1) §7 “seven v0 rows out” vs the live five v0-engine inventory files and two retained GC files — **not architectural** (count/scope error in a named disposition). (2) §10.1 §5:180 replacement mixing charter sequencing into the post-close I2 sentence — **not architectural**. **No new architectural root cause.** The two-round cap is not triggered on this axis.

---

## 0. Evidence base

Commands (cwd `/Users/owebeeone/limbo/gwz-dev`), start and end:

- `git rev-parse HEAD` → `3d0740a3c86f5f31a7bd734a0227a34887e1db98`
- `shasum -a 256 dev-docs/GwzM5-8M5d-Charter.md` → `38b7d630620899487c388c96c88b2f57d2a6e39e1b8454d47470dd702afaa624`
- `git -C gwz-core rev-parse HEAD` → `57502e461055809503ca9dc0d7cb791b8b332f6c`

Read: RemPlan in full; charter revision 2 in full (364 lines); M5c banner + §0 heading (and §2.2–§2.4 only to re-check restatement completeness). I2 HEAD §1:36, §2:83–108, §4–§5:119–204, §7 pointer, §8:289, §9:304–305 for verbatim quote check. Live floor still `ACTIVE_WRITER_FLOOR = V0` at `version.rs:39` (unchanged gwz-core pin — used only to confirm P1-1’s walk is now forbidden, not performed). CapabilityFree inventory `check_checked_artifact_boundaries.py:544–564`. Open-record path `persistence.rs:44`. Targeted tests: none. Not read: `GwzM5-8M5d-Review.md`; Safety-axis report (except RemPlan dispositions).

---

## 1. Findings

### [P3-1] §7 “seven v0 rows out” is not the live v0-engine carved set and can be misread as deleting retained GC writers

**Location.** Object §7 CapabilityFree row: “Net may shrink: seven v0 rows out, one `entry.rs` raw primitive in.” Live `CAPABILITY_FREE_RAW_WRITER_INVENTORY` v0-**engine** files: `abort/evidence.rs`, `abort/preflight.rs`, `finalize.rs`, `preserve/artifacts.rs`, `store/archived.rs` (five). The other two `workspace_ops/merge/*` inventory rows are `store/gc.rs` and `store/retention.rs` — live GC, which the same table retains (“I2 §7 archive decoder / GC | **retained**”).

**Violated invariant.** A named inventory disposition must not count retained GC writers as “v0 rows out.”

**Reproduction.** An S1 dual that takes “seven” as the seven `merge/*` inventory keys would delete `gc.rs` / `retention.rs` counts. That fails I2 §7 retention in the same §7 table. The original P2-4 walk (delete `finalize.rs` without a CapFree revision) is already closed; this is the forecast inside the revision.

**Impact.** Bounded: the operative sentence (“revise the carved set when v0-engine files leave production”) is correct; the number is not. S1 would have to invent which seven.

**Required correction.** Replace “seven v0 rows out” with “the v0-engine carved files (`finalize.rs`, v0 abort/preserve/archived writers — not `store/gc.rs` / `store/retention.rs`).” Keep “one `entry.rs` raw primitive in” as an S1 measurement, including adding a token if `write_atomic_verified` is not in `CAPABILITY_FREE_WRITER_TOKENS` today.

**Closure test.** The CapabilityFree row cannot be read as authorizing shrinkage of the I2 §7 GC writers.

### [P3-2] §10.1 §5:180 replacement is a two-phase charter note, not the post-close I2 sentence

**Location.** §10.1 row §5:180 Replacement: “**Until the close commit** (unpublished trees): this sentence still holds. **After close:** a mutating command that finds open v0 refuses with the §2 sentence; it does not write v0.” Charter §7 already states the until-close lifecycle. §10’s job is “Implementers produce the I2 diff from this list.”

**Violated invariant.** The I2 file amended at close should contain only the post-close rule. Copying this replacement column into I2 would freeze “until the close commit (unpublished trees): this sentence still holds” as accepted compatibility text after that commit has landed.

**Reproduction.** Produce the I2 diff by substituting the Replacement column. Accepted I2 still talks about unpublished trees and a future close.

**Impact.** Bounded stale sentence in the compatibility contract; does not re-enable production refuse-before-floor (that is forbidden by §7/§9 independently).

**Required correction.** Replacement = only the after-close sentence. Keep the until-close fact in §7/§9, not in the I2 substitution table.

**Closure test.** Every §10.1 Replacement cell is copyable into I2 at close without describing a state that has already ended.

---

## 2. Invariant analysis

Re-traced attacks that **hold as closed** (this GO rests on them):

- **Refuse-before-floor (P1-1).** Live ordinary starts still write v0 (`version.rs:39`, `start.rs` V0 fallthrough). The charter no longer authorizes publishing the §2 refuse in that tree. Test-only helper + “unpublished abort still clears v0” is the remaining legal intermediate.
- **L1-08 I2 exactness (P2-1).** §10.1 quotes the sentences that contradicted refuse; §10.2 keeps §5:181–182 and I2 §7; predicates JSON and the 0.14 decode generation (`v0.13.0`) are listed. Line numbers smear on wrapped I2 lines (e.g. `UnsupportedLegacyMode` sits on 183–184 sharing a line with the end of the retained archive sentence); the **quoted clauses** match. Residual: P3-2; `writer_version` “V0-to-v1 migration therefore sets…” is instructed to die in §10.2 and does not contradict refuse, so it does not reopen P2-1.
- **Engine vs archive type (P2-2).** Acceptance §8 is satisfiable with I2 §7 kept.
- **M5c supersession (P2-3).** No current sentence treats a M5c section as controlling. §4 restatement is complete for events/responses/`stage` relative to M5c §2.2–§2.4, with open-v0 `stage` correctly redirected to §2 refuse.
- **Fail-closed inventories (P2-4).** Named. Relocate-`write_atomic_verified` is not claimed to be the whole F-3 story.
- **Ship (1) warn/strict/bar.** Still the v1 door; ordinary `--filesystem-strict` flips only in the same change as the floor (§1, §7, §9).
- **Reverse-door product vs §0 (i) / transactional-apis.** Start is not newly refused; reverse-path raw is not a milestone; crash `recover()` remains excepted; live abort is not excepted. Copy-workspace / operator git+delete-open-record is disclosed as 0.14-does-not-do-this, path-accurate. Ship (1)’s identity sentence is explicitly **not** reused because it advertises `--abort` as the escape.
- **Occupancy vs idle.** §2/§8: commit/add/stage/status/continue/abort on open v0 refuse without body decode and without writing; `stage` does not take the §4 v1-body path. Internally consistent.
- **`v0.13.0` pin.** Engine deletion unauthorized unless that artifact is published and installable; 0.14 notes are a precondition of (4). Agrees with I2 §8 first bullet as used in §10.1 §8:289.

---

## 3. Risks and next action

P3-1 and P3-3 (round-1) class residuals do not block. Fold P3-1 and P3-2 in the I2-close / S1 dual briefing if cheap; they are not a third architectural root cause.

**Next action:** accept this charter revision 2 on the consistency axis. Do not implement, do not cut 0.14.0, do not start M6, until the dual merge (both axes GO on this SHA-256) says otherwise. I2.md body still waits for that close dual, with §10 as the exact clause list.
