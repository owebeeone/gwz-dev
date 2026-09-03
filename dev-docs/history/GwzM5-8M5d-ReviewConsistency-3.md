# GwzM5-8M5d — CONSISTENCY-AXIS REVIEW

**Review object:** working-tree file `dev-docs/GwzM5-8M5d-Charter.md` revision 3 (UNTRACKED; CHARTERED 2026-09-03; SHA-256 `ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`). Controlling DRAFT document: the object itself.
**Baseline:** gwz-dev HEAD `3d0740a3c86f5f31a7bd734a0227a34887e1db98` (verified at start and end). gwz-core HEAD `57502e461055809503ca9dc0d7cb791b8b332f6c` (verified at end). Object bytes read from the working tree. Tuple did not move.
**Date:** 2026-09-03
**Axis:** CONSISTENCY — focused P3 confirm on revision 3 after RemPlan-2. Round-1 P0–P2 stay closed unless this delta reopens them. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on its report (RemPlan-2 IDs are legitimate round-3 input). Filed verbatim by the lane owner.

**Verdict: GO** — 0 P0 / 0 P1 / 0 P2 open. Round-2 P3-1 and P3-2 closed on these bytes. No new architectural root cause.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
| --- | --- | --- | --- |
| **R1 P1-1** | Production §2 refuse not landable while floor is V0; (1)–(4) not independently published to `main` | Unchanged and still present: §9:286–298; §7:252–257; §1:62–66. Original published-`main` refuse-before-floor walk remains unconstructable. | **CLOSED** (stays) |
| **R1 P2-1** | §10 exact I2 superseded/retained list | §10 still present; §5:180 row remains a quoted I2 sentence with a post-close replacement (see P3-2). Archive §5:181–182 still retained. | **CLOSED** (stays) |
| **R1 P2-2** | No v0 lifecycle engine; archive decoder remains | §1:42–45, §8:274–279 unchanged in substance. | **CLOSED** (stays) |
| **R1 P2-3** | M5c superseded for all ship-2; restatements control; not “kept” | Banner lines 5–9; §4:175. | **CLOSED** (stays) |
| **R1 P2-4** | Name CapabilityFree + F-3 as affected | §7 CapabilityFree and F-3 rows still named; P3-1 only corrected the carved-set forecast. | **CLOSED** (stays) |
| **R2 P3-1** | Replace “seven v0 rows out” with v0-engine files, not `store/gc.rs` / `store/retention.rs` | Original misread (treat seven `merge/*` inventory keys as “v0 rows out,” including live GC) is gone. §7:247: “The v0-engine carved files (`finalize.rs`, v0 abort/preserve/archived writers — **not** `store/gc.rs` / `store/retention.rs`) may leave; one `entry.rs` raw primitive may enter (S1 measurement, including adding a token if `write_atomic_verified` is not in `CAPABILITY_FREE_WRITER_TOKENS` today).” Same table still retains I2 §7 GC. | **CLOSED** |
| **R2 P3-2** | §10.1 §5:180 Replacement = only the after-close sentence; until-close stays in §7/§9 | Original copy-into-I2 walk (freeze “until the close commit (unpublished trees): this sentence still holds”) is gone. §10.1 §5:180 Replacement is only: “A mutating command that finds open v0 refuses with this charter’s §2 sentence; it does not write v0.” Until-close remains in §7:255–256 (“Until that commit, mutating v0 stays on today's I2 §5 lifecycle”) and §9:293–298. | **CLOSED** |

---

## Changed-range analysis

RemPlan-2 applied three edits on revision 2 → 3:

1. **C-P3-1 fold** — CapabilityFree §7 cell as quoted above.
2. **C-P3-2 fold** — §10.1 §5:180 Replacement stripped of the two-phase charter note.
3. **S-P1-2 take** — §3 (b)(2) in-place `git merge --abort` + delete `.gwz/merge/<id>.yaml` **struck**. Sole (b) escape: copy the workspace onto a handle-capable volume, then `gwz merge --abort` (add `--preserve` if that was the stuck door). Explicit “do **not** delete … in place” / “do **not** treat `git merge --abort` as a stand-in.”

No other sections were rewritten. Refuse-before-floor, occupancy, archive decoder, unpublished-landing text is intact.

**Internal-contradiction check (S-P1-2 delta vs §7/§8/§9):** none. §8 still requires that selected-root/`--preserve` abort “prints the §3 escape and leaves Git/record in a state that escape actually clears”; §3 now names only the copy-then-abort escape, so the two agree. §7 `transactional-apis.mdc` remains crash-arm only, not live abort — copy-then-`gwz merge --abort` on a handle-capable volume is that live abort, not a porcelain recipe in the product sentence. §9 still forbids reverse-path raw as a step and does not refuse start.

**NEW ARCHITECTURAL root causes:** none. This delta is a P3 count/copyability fold plus striking a recipe this axis did not own. The two-round cap is not triggered on this axis.

---

## 0. Evidence base

Commands (cwd `/Users/owebeeone/limbo/gwz-dev`), start and end:

- `git rev-parse HEAD` → `3d0740a3c86f5f31a7bd734a0227a34887e1db98`
- `shasum -a 256 dev-docs/GwzM5-8M5d-Charter.md` → `ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`
- `git -C gwz-core rev-parse HEAD` → `57502e461055809503ca9dc0d7cb791b8b332f6c`

Read: RemPlan-2 in full; charter revision 3 banner, §1 floor/strict, §3 reverse doors, §7 table, §8, §9, §10.1 §5:180. Targeted tests: none. Not read: Safety-axis report (except RemPlan-2 dispositions); `GwzM5-8M5d-Review.md`.

---

## 1. Findings

None open. Round-2 P3-1 and P3-2 counterexamples do not reproduce on these bytes.

---

## 2. Invariant analysis

- **P3-1.** “Seven v0 rows out” is absent. The carved-set sentence names the v0-engine files and carves out the two I2 §7 GC inventory rows. S1 is still instructed to measure `entry.rs` / `write_atomic_verified` rather than assume the current token list already sees it.
- **P3-2.** An implementer who substitutes the Replacement column into I2 at close gets only the post-close refuse rule. The pre-close lifecycle is not in that cell.
- **Round-1 blockers.** Spot-check of §1/§7/§8/§9/§10 shows the round-2 closed text for refuse-before-floor, engine vs archive, M5c restatement, and named F-3/CapabilityFree pins was not reverted.
- **S-P1-2 strike.** Not re-opened as a consistency finding. §3/§8/§7/§9 tell the same story: one copy escape; no in-place yaml delete; no start refuse; no reverse-path-raw milestone.

---

## 3. Risks and next action

No residual below the finding bar on this axis. Dual GO still requires Safety GO on **this same SHA-256** (`ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`).

**Next action:** accept revision 3 on the consistency axis. Do not implement. Do not cut 0.14.0. Do not start M6.
