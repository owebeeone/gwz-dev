# GwzM5-8M5d — SAFETY-AXIS REVIEW (round 3)

**Review object:** working-tree file `dev-docs/GwzM5-8M5d-Charter.md` revision 3 (UNTRACKED; CHARTERED 2026-09-03; SHA-256 `ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`). Controlling DRAFT is the object itself. RemPlan-2: `dev-docs/GwzM5-8M5d-RemPlan-2.md`.
**Baseline:** gwz-dev HEAD `3d0740a3c86f5f31a7bd734a0227a34887e1db98`; gwz-core HEAD `57502e461055809503ca9dc0d7cb791b8b332f6c`. End-of-review `git rev-parse HEAD`, `shasum -a 256`, and `git -C gwz-core rev-parse HEAD` matched the start tuple; the object did not move.
**Date:** 2026-09-03
**Axis:** SAFETY — degraded and mixed-version paths; irreversible steps and their preconditions; disclosure/privacy scale; stuck states reachable under the text’s own rules; whether “never worse than the status quo” survives concrete interleavings; scope creep that widens blast radius. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — 0 P0 · 0 P1 · 0 P2 open. Round-2 P1-2 is closed on this tree. P1-1, P1-3, P2-1, and P2-2 remain closed. No new architectural root cause; the two-round cap does not fire.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P1-1 | Production §2 refuse, floor `V1`, engine deletion, and `--filesystem-strict` on ordinary starts flip in one reviewed change; unpublished intermediates still abort v0 | Delta does not touch §7 lines 252–257 or §9 lines 285–298. Original walk (publish refuse to `main` while floor is `V0`) remains unconstructable | **CLOSED** (stays) |
| P1-2 | Strike §3 (b)(2). Only named (b) escape is copy onto a handle-capable volume then `gwz merge --abort` (add `--preserve` if that was the stuck door). Do not advertise `git merge --abort` + delete yaml. Do not refuse start. Do not add reverse-path raw | Original overlay + selected-root/evidence (and separately `--preserve`) walk: start still runs; abort still refuses in place; 0.13.0 is still not a v1 escape. In-place recipe is **absent** (only appears as a prohibition, lines 148–150). Copy then 0.14 abort on APFS/ext4/xfs/f2fs/NTFS re-acquires handles and runs the v1 reverse path, which clears record + evidence/root/`--preserve` state. Closure test holds | **CLOSED** |
| P1-3 | Pin `v0.13.0`; third occupancy; gate/commit/add/stage refuse; notes precondition of erasure | §2 / §8 occupancy and `v0.13.0` pin unchanged by this delta | **CLOSED** (stays) |
| P2-1 | No v0 **lifecycle engine**; I2 §7 archive decoder remains; cannot read deletion of that decoder as acceptance | §1 / §5 / §8 / §10.2 unchanged on that split | **CLOSED** (stays) |
| P2-2 | (1)–(4) not independently published to `main`; dual object is one tree | §9 title and lines 285–291 unchanged | **CLOSED** (stays) |

---

## Changed-range analysis

Revision 2 SHA-256 `38b7d630620899487c388c96c88b2f57d2a6e39e1b8454d47470dd702afaa624` → revision 3 `ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`. HEADs unchanged.

Delta vs revision 2 (Safety-relevant):

- **§3 (b)** (lines 140–150): in-place git+delete recipe **struck**. One escape: copy the whole workspace onto a handle-proving volume, then `gwz merge --abort` / `--abort --preserve`. Explicit **do not** delete `.gwz/merge/<id>.yaml` in place; **do not** treat `git merge --abort` as evidence/root/`--preserve` rollback. Start still not refused. Reverse-path raw still not a phase.
- **§7 CapabilityFree row:** “seven v0 rows out” replaced by named v0-engine carved files (`finalize.rs`, v0 abort/preserve/archived writers — not `store/gc.rs` / `store/retention.rs`). Consistency C-P3-1 fold. Does not change abort-on-overlay product or occupancy.
- **§10.1 §5:180 Replacement:** until-close sentence removed from the I2 replacement; after-close refuse only. Until-close remains in §7/§9. Consistency C-P3-2 fold. Does **not** reopen P1-1: production refuse still must not land while the floor is `V0`.

`git merge --abort` and in-place yaml delete appear only as prohibitions (grep: those two lines). No new mutation API, recover() grammar, or identity store.

**New architectural root causes:** none. Striking a false operator recipe is not architecture. Cap: a third new architectural root cause on this object would stop the lane — **does not fire**.

---

## 0. Evidence base

Tuple (start and end, cwd `/Users/owebeeone/limbo/gwz-dev`):

- `git rev-parse HEAD` → `3d0740a3c86f5f31a7bd734a0227a34887e1db98`
- `shasum -a 256 dev-docs/GwzM5-8M5d-Charter.md` → `ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`
- `git -C gwz-core rev-parse HEAD` → `57502e461055809503ca9dc0d7cb791b8b332f6c`

Read: object revision 3 (full, 362 lines); `GwzM5-8M5d-RemPlan-2.md` (full). Live abort doors at the unchanged core pin were not re-read in full this round; the round-2 cites still apply (`CheckedArtifact::acquire` on v1 root/`--preserve`/evidence; v0 evidence rollback raw). Targeted tests: NONE.

Out of scope: `GwzM5-8M5d-Review.md`; local clone; production code; I2.md body; tagging.

---

## 1. Findings

None open. No new P0–P2.

P1-2 original counterexample, re-traced on revision 3:

1. Overlay without `nfs_export`. Ordinary start writes v1 raw and runs (table handle-fail row). Intended.
2. `@root` has published evidence, **or** `--abort --preserve`. `gwz merge --abort` hits `CheckedArtifact::acquire`, handle probe fails, refuses with the **new** sentence (not `PERSISTENT_FILESYSTEM_IDENTITY_REMEDY`). 0.13.0 sees v1 and hits the same doors. Intended; 0.13.0 is not an accepted pass.
3. There is **no** in-place “`git merge --abort` then delete yaml” product. Following that would be operator improvisation the charter now forbids.
4. Named escape: copy the whole workspace onto APFS/HFS+/ext4/xfs/f2fs/NTFS. Open yaml, Git indexes, `MERGE_HEAD`, lock/marker/evidence commits, and dirty trees copy with the tree. On the destination, `name_to_handle_at` / equivalent succeeds. `gwz merge --abort` (or `--abort --preserve`) takes the capability-free lease and the reverse checked doors now succeed: evidence rollback, selected-root manifest/lock restore, preservation bundles. Record archives. No leftover open file; no leftover composition commit / candidate lock / marker / preservation-needed dirty tree **on the workspace the operator continues on**.
5. Start was not refused. Reverse-path raw was not added as a phase.

That is the round-1 required correction’s working arm, with the destructive arm removed. Finding closed.

---

## 2. Invariant analysis

- **Stuck (b) merge has a working, non-circular escape that does not need 0.13.x or in-place record deletion.** Holds. Copy-then-abort is 0.14 v1 reverse on a volume the probe admits.
- **In-place yaml delete cannot be read as an official escape.** Holds (prohibited).
- **Refuse-before-floor, occupancy, archive decoder, unpublished landing.** Unchanged; still hold.
- **Plain overlay abort (a).** Unchanged; still clears without copy.
- **Crash-only transactional-apis exception.** Unchanged; does not cover live abort.
- **§10.1 §5:180 after-close-only replacement.** Does not authorize a published main with refuse while floor is `V0`; §7/§9 still forbid that.

Failed residual attacks (not findings): copy leaves the original overlay tree stuck — the product is to continue on the copy; disposing the overlay tree is operator cleanup, not a second recipe. Host bind-mount of APFS into an overlay container can abort from the host without copy; that is a stricter-volume view of the same escape, not a contradiction.

---

## 3. Risks and next action

Residual below the bar: sshfs still unmeasured; Windows `require_ntfs` until a Windows-verified step; copy is operator `cp`, not local clone; ship (1) warning diagnostic includes the reverse-door limit (pin movement at implement).

Next action: this axis **GO** on revision 3 SHA-256 `ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`. Do not implement from this report. Do not tag. Acceptance of ship 2 still waits on the dual merge and a later authorized build.
