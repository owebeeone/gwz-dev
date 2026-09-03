# GwzM5-8M5d — SAFETY-AXIS REVIEW (round 2)

**Review object:** working-tree file `dev-docs/GwzM5-8M5d-Charter.md` revision 2 (UNTRACKED; CHARTERED 2026-09-03; SHA-256 `38b7d630620899487c388c96c88b2f57d2a6e39e1b8454d47470dd702afaa624`). Controlling DRAFT is the object itself. RemPlan: `dev-docs/GwzM5-8M5d-RemPlan.md`.
**Baseline:** gwz-dev HEAD `3d0740a3c86f5f31a7bd734a0227a34887e1db98`; gwz-core HEAD `57502e461055809503ca9dc0d7cb791b8b332f6c`. End-of-review `git rev-parse HEAD`, `shasum -a 256`, and `git -C gwz-core rev-parse HEAD` matched the start tuple; the object did not move.
**Date:** 2026-09-03
**Axis:** SAFETY — degraded and mixed-version paths; irreversible steps and their preconditions; disclosure/privacy scale; stuck states reachable under the text’s own rules; whether “never worse than the status quo” survives concrete interleavings; scope creep that widens blast radius. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — P1-2 remains open. P1-1, P1-3, P2-1, and P2-2 closed on this tree. No new architectural root cause.

I pre-commit to GO on a revision that resolves P1-2 as specified (strike or replace the in-place recipe).

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| P1-1 | Strike “can land before the floor”; production §2 refuse, floor `V1`, engine deletion, and `--filesystem-strict` on ordinary starts flip in the same reviewed change; until then mutating v0 stays on today’s lifecycle | Original walk (land step (1) on `main`, floor still `V0`, ordinary start writes v0, same binary `--abort` hits §2 refuse) is **unconstructable**: §7 lines 254–259 and §9 lines 285–300 forbid publishing the production refuse while `ACTIVE_WRITER_FLOOR` is `V0`; unpublished intermediates must still abort v0 | **CLOSED** |
| P1-2 | Do not refuse start; do not add reverse-path raw as a milestone; (a) participant-only abort clears; (b) selected-root / `--preserve` / published-evidence abort refuses with a **new** sentence; that sentence names a working escape needing neither handles nor 0.13.x (copy **or** exact git + delete-open-record); start warns; 0.13.0 is not an accepted pass for (b) | Original walk still starts on overlay and still refuses those abort doors (intended). Circular boundary sentence is gone. **Copy then `--abort` on a handle-capable volume clears** (preferred arm). **In-place arm fails the original (b) subset** and, if followed, deletes the open record that the preferred arm needs as authority. Closure test “escape actually clears” is not met for a procedure the charter still names as a working escape | **OPEN** |
| P1-3 | Pin `v0.13.0` as leftover-open escape; deletion unauthorized if that artifact is not published; 0.14 notes are a precondition of erasure; third occupancy: envelope v0 is an open operation, not a merge, not idle; gate / commit / add / stage refuse with the §2 sentence; no body decode | Original walk (upgrade, leftover open v0, 0.14 abort/commit/add/stage): §2 lines 70–96 and §8 lines 274–276 require refuse without body decode and without idle; §2 lines 70–76 pin `v0.13.0` and make notes a precondition of (4). `discover_open` returning “no merge” is forbidden | **CLOSED** |
| P2-1 | Replace “no production v0 merge type” with no v0 **lifecycle engine**; I2 §7 archive decoder/GC remain production, `done/` only; decode of archived v0 is not “the v0 engine” | Original walk (delete `decode_production_v0` under §5/§8): §1 lines 42–45, §5 lines 216–221, §8 lines 276–281, §10.2 now forbid reading deletion of the archive decoder as acceptance. Open v0 is envelope-only | **CLOSED** |
| P2-2 | Restate §9 as order inside one unpublished branch / one landing commit, not landable `main` steps; dual object is one tree | Original walk (publish step (3) to `main` before (4)): §9 title and lines 287–293, 307–311 forbid independent `main` publication; dual object lists v1 ordinary, raw create, §4 surfaces, production §2 refuse, and live `--filesystem-strict` together | **CLOSED** |

---

## Changed-range analysis

The object was rewritten in place (still untracked). Round-1 SHA-256 `cc3635c8419e258ba51b7d8891ff3ed18c589d362aa7f9f77bfc27de6e7f2211` → `38b7d630620899487c388c96c88b2f57d2a6e39e1b8454d47470dd702afaa624`. M5c working-tree banner / §0 heading uses the §7.2 supersession formula (out of scope except as the named trail). I2.md body unchanged (amendment text is new charter §10). gwz-dev / gwz-core HEADs unchanged.

In-scope deltas vs round 1, mapped to dispositions:

- §1 / §7 / §9: refuse-before-floor and landable-`main` series (P1-1, P2-2).
- §2 / §8: `v0.13.0` pin, irreversible-step preconditions, third occupancy (P1-3).
- §1 / §5 / §8 / §10.2: lifecycle engine vs archive decoder (P2-1).
- §3 / §8: reverse-door product, new refuse sentence, two named escapes (P1-2).
- §4 / §7: F-3 redefine, CapabilityFree carved-set revision (Consistency C-P2-4; not a Safety round-1 ID).
- §10: exact I2 superseded/retained list (Consistency C-P2-1).
- C-P3 folds: “starts and can complete the forward path”; transactional-apis crash-only exception; A1/DR-1 carrier retarget.

Outside the dispositions: none that change runtime authority except the **in-place git recipe** in §3 (b)(2), which is the failed half of the P1-2 cure (see Findings). That is a **text defect in the remediation**, not a new architectural root cause: no new coordinator, no new recover() grammar, no new identity store. The two-round cap does **not** trip.

---

## 0. Evidence base

Tuple (start and end, cwd `/Users/owebeeone/limbo/gwz-dev`):

- `git rev-parse HEAD` → `3d0740a3c86f5f31a7bd734a0227a34887e1db98`
- `shasum -a 256 dev-docs/GwzM5-8M5d-Charter.md` → `38b7d630620899487c388c96c88b2f57d2a6e39e1b8454d47470dd702afaa624`
- `git -C gwz-core rev-parse HEAD` → `57502e461055809503ca9dc0d7cb791b8b332f6c`

Read: object revision 2 (full, 364 lines); `GwzM5-8M5d-RemPlan.md` (full); M5c banner lines 1–13 only; live abort doors unchanged at the core pin (`abort/evidence.rs:664-719` v0 evidence rollback still raw; `root/artifact_facts.rs:39-50` v1 root rollback still `CheckedArtifact::acquire`). Round-1 live cites (floor `V0`, `create_merge_store_record`, `write_atomic_verified`, overlay `name_to_handle_at`) still hold; this charter is not implementation.

Out of scope: `GwzM5-8M5d-Review.md`; local clone; production code; I2.md body; tagging. Targeted tests: NONE.

---

## 1. Findings

### [P1-2] In-place “working escape” does not clear the original (b) subset and destroys the preferred escape

**Root cause:** §3 (b) still names two escapes for selected-root / `--preserve` / published-evidence abort on handle-fail volumes. The preferred copy path works. The in-place path (`git merge --abort` where `MERGE_HEAD` exists, then delete `.gwz/merge/<merge-id>.yaml`) does not roll back evidence commits, baseline lock/manifest/marker/boundary, or preserved dirty work; it removes occupancy first. That is not an “exact git + delete-open-record procedure” for the doors it is offered for.

**Location:** Object §3 lines 140–158 (especially 149–152); §8 lines 266–270 (“leaves Git/record in a state that escape actually clears”). RemPlan S-P1-2 required the git procedure to be exact **or** copy; both were shipped as working.

**Violated invariant:** A documented escape for a stuck merge must actually restore Git and GWZ state for the subset that is stuck, and must not delete the open record that the remaining working escape (copy, then `gwz merge --abort`) needs as authority. Round-1 closure test: after (b) refuse, following the named escape clears. 0.13.0 is still not an accepted pass (record is v1) — that half of the disposition holds.

**Reproduction (original (b), re-traced):**

1. Overlay without `nfs_export`. After this close: ordinary `gwz merge <source>` writes v1 raw and runs (table row handle-fail; start diagnostic includes the reverse-door limit).
2. `@root` participated and publication evidence is on disk (composition commit, lock, marker, boundary), **or** the user is stuck on `--abort --preserve`.
3. `gwz merge --abort` refuses with the **new** sentence (not `PERSISTENT_FILESYSTEM_IDENTITY_REMEDY`). 0.13.0 sees v1 and hits the same checked doors. Unchanged from round 1, and intended.
4. **Preferred arm (works):** `cp -a` the workspace onto ext4/APFS; on that volume `CheckedArtifact::acquire` succeeds; `gwz merge --abort` (or `--abort --preserve`) runs the v1 reverse path including `execute_v1_evidence_rollback` / root metadata rollback. Record archives. This arm **would** close P1-2 by itself.
5. **In-place arm (fails, and is still in the product):** operator runs `git merge --abort` in the root and in members that still have `MERGE_HEAD`. Members (and root) whose evidence/merge commit is already `HEAD` have **no** `MERGE_HEAD` — the recipe is a no-op there. It does not `rollback_evidence_commit`, does not restore baseline lock/boundary/marker (`abort/evidence.rs:677-709` on v0; v1 equivalent is checked `write_checked`). `--preserve` work is discarded if `git merge --abort` runs, which is the opposite of that door. Then they delete `.gwz/merge/<id>.yaml`.
6. After step 5: no open record (gate idle), publication/Git progress remains, copy-then-abort can no longer run because the v1 body is gone. Worse than the round-1 stuck state.

**Impact:** Operators who follow the in-place recipe on the only subset that needs an escape (plain abort already clears per (a)) get a workspace the mutation gate treats as idle while merge publication is live. The preferred escape is then unavailable. Status quo v0 overlay abort still would have cleared via raw writers; this charter still refuses in-place abort on (b), which is accepted — the defect is advertising a second escape that does not clear and that burns the first.

**Required correction:** Strike §3 (b)(2) entirely, **or** replace it with a procedure that (i) never deletes the open yaml until Git/lock/marker/boundary/evidence match the aborted baseline, (ii) does not use `git merge --abort` as a stand-in for evidence/root/`--preserve` rollback, and (iii) for `--preserve`, is “copy, then `gwz merge --abort --preserve` on the copy” only. Preferred copy path stays. Do not refuse start. Do not add reverse-path raw as a phase.

**Closure test:** §3 lists only escapes that, followed on the original overlay + selected-root/evidence (and separately `--preserve`) counterexample, leave no open record **and** no leftover composition commit / candidate lock / marker / preservation-needed dirty tree. An in-place recipe that deletes `.gwz/merge/<id>.yaml` while those remain is a failing row.

---

No other round-1 Safety IDs remain open. No new P0–P2 except the still-open P1-2 above.

---

## 2. Invariant analysis

**Closed on this tree**

- **Refuse-before-floor (P1-1).** §9 step (1) may draft I2 text and a test-only helper; production dispatch must not emit §2 while the floor is `V0`. Same-binary “create v0 then refuse abort” is forbidden, including on unpublished intermediates.
- **Leftover open v0 occupancy (P1-3).** Envelope-only open operation; gate/commit/add/stage/status/continue/abort refuse with `use tag v0.13.0`; not idle. Engine deletion unauthorized without published `v0.13.0`. Notes are a precondition of §9 (4), not a follow-on.
- **Archive decoder vs engine (P2-1).** Acceptance cannot be checked off by deleting I2 §7. GC still never deletes unread/unreadable archives.
- **Landable series (P2-2).** (1)–(4) are not independently published to `main`. Dual object is one tree including live `--filesystem-strict` on ordinary starts.
- **Circular abort clause.** (b) refuse is a new sentence; `PERSISTENT_FILESYSTEM_IDENTITY_REMEDY` is not reused there.
- **Raw create + kill inventing progress.** Unchanged fail-closed `write_atomic_verified` / `*.yaml`-only discover. Not reopened.
- **Plain overlay abort.** (a) still holds against live reverse lease + raw `commit`.
- **Crash-only transactional-apis exception.** §3 lines 169–172; does not cover live abort. Holds.

**Still failing**

- **“Escape actually clears” for every named (b) procedure.** Copy-then-abort holds. In-place git+delete does not, on the original (b) counterexample, and is destructive of the copy path. P1-2 open.

Not architectural: the in-place recipe is a bad operator procedure, not a new mutation/recovery/identity design.

---

## 3. Risks and next action

Residual below the bar: sshfs still unmeasured; Windows `require_ntfs` until a Windows-verified step; ship (1) warning text grows the reverse-door limit in the same diagnostic (pin movement is implementation); copy-the-workspace is operator `cp`, not `gwz clone --local` (deferred).

Next action: one bounded text patch striking or replacing §3 (b)(2) so P1-2’s closure test passes. Do not implement. Do not tag. Do not treat P1-1/P1-3/P2-1/P2-2 as still open on this axis.
