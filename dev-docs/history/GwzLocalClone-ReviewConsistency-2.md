# GwzLocalClone — CONSISTENCY-AXIS REVIEW (round 2)

**Review object:** working-tree file `dev-docs/GwzLocalCloneDesign.md` (UNTRACKED; DRAFT revision 3, 2026-09-03; SHA-256 `1c543d04afe2850177c0e1098013f507f7412be77dacc8e441bea60b443a8df7`)
**Baseline:** gwz-dev HEAD `68b6ba43050426a77447f3d9c4a00fc8eb2d380b` (start and end; object and remplan digests unchanged). Remplan `dev-docs/GwzLocalClone-RemPlan.md` SHA-256 `0995242ab8ef41ad881b73845b44f7f346e8b6429ca50c2cadd9538da79672a1`. Round-1 object `579c9fd8…` is gone; counterexamples reconstructed from `dev-docs/GwzLocalClone-ReviewConsistency.md` and re-traced on revision 3.
**Date:** 2026-09-03
**Axis:** CONSISTENCY — the document against its controlling graph
**Verdict: NO-GO** — all round-1 blocking IDs are closed on revision 3; two new P2s (P2-8, P2-9) are open. Not a new architectural root cause (two-round cap not triggered). I pre-commit to GO on a revision that resolves **P2-8** and **P2-9** as specified.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| **P0-1** | Install dest `.gwz/`; never copy `local-family.yml`; always write new `family-root`; both index+pointer → refuse; refuse verbatim while a gwz merge is open | Original 8.1 from root without an open merge: dest is **installed**, not a copied root. §4.1 denylist omits `local-family.yml` and source `family-root`; post-copy requires pointer, forbids index; dest-complete requires “no index file on dest”. §8.1: “family-root → root + `family_id`; no `local-family.yml`”. From A, `--name B` “registers on root, not on A”. Original step 4 (dest looks like root) does not hold. If a sloppy copy left both files, dest-complete fails → recover drops the dest (fail-closed), not a second live root. | **CLOSED** |
| **P2-1** | New request/method; do not make `CloneWorkspaceRequest.url` optional; name live types; §7 table ticks §8 CLI against `gwz.taut.py` | Live: `ActionKind` ends at `log=26` (`gwz.taut.py` :162–169); 27/28 unused. `CloneWorkspaceRequest.url=F(2, STR)` still required (:1445). `MergeRequest.source_ref=F(3)` (:1658–1659); `filesystem_strict=F(8)` (:1670); F(9) unused. `PushRequest.remote=F(2)` (:1622). `PullHeadRequest` is `meta` only (:1610–1611); `OperationPolicy.remote=F(5)` (:828). Sketch names `CloneLocalWorkspaceRequest` / `LocalFamilyRequest`, not `CloneRequest`/`PullRequest`/`MergeRequest.source`. Every distinct §8 family CLI shape in the §7 table maps to a live type or an explicitly new tag. | **CLOSED** |
| **P2-2** | Bare merge token stays a git ref; family source is `--remote` / `local_source_name`; do not reuse `source_ref` | Original: `gwz merge feature/x` under §6 `UnknownLocal`. Now §6 :239: bare merge token is a git ref and is **not** `UnknownLocal`. §8.2 :400–401: `gwz merge feature/x` git ref; `gwz merge A` is git ref `A`, not clone A. Family merge is `gwz merge --remote A` → `local_source_name=F(9)` (:311–312). After dispose, `gwz merge --remote C` is `UnknownLocal`; `gwz merge C` would be git ref `C`. Closure matrix (clone `A` and branch `A`) is written in §8.2. Residual dual-denotation on **pull/push `--remote`** is **P2-8**, not this ID. | **CLOSED** |
| **P2-3** | Hub is a workspace: `gwz.conf/gwz.yml` + `.gwz/family-root`; `core.bare=true`; layout-only non-bare `.git` withdrawn | Original “only `.git` folders / no checkout” is gone. §4.3 :193–197: real bare (`core.bare=true`), hub **is** a gwz workspace so `discover_workspace_root` works; `local list` / `dispose` run here. §8.1 :356–357 matches. | **CLOSED** |
| **P2-4** | Reserve `origin` (and `HEAD`, `FETCH_HEAD`, `/`, `:`); refuse a name that is already a git remote; do not persist family git remotes; dispose does not delete remotes by name-equality | Original `gwz clone --local --name origin` is now refused (§2 :36–38; §8.1 :362–363). §6 :246–248: bindings resolved at operation time; `repo sync` / capture must not write family URLs into `gwz.yml`. §5 :225–227: sweep is operation-time bindings, “no persisted git remotes to delete”. Original dispose-sweeps-`origin` does not hold. | **CLOSED** |
| **P2-5** | Never copy the pointer; write dest-relative or canonical path + `family_id`; mismatch → `FamilyRootMismatch` | Original source→dest rewrite of a copied pointer is gone. §3 :56–57 “never copied”; §4.1 table installs a **new** file; §8.1 :350 “B’s family-root is computed, not copied”. Non-sibling dest still allowed if unique; the write is computed, not copied. | **CLOSED** |
| **P2-6** | Split timelines; refuse verbatim while open; §8.3 uses clean clones | Original: 8.1 left A with a copied open merge, then 8.3 started `feature/x` on A. Now §4.1 :160–162 refuse verbatim while a gwz merge is open; §8.1 opens a merge only to demonstrate that refuse, then `--abort`. §8.3 :404–409 starts `feature/x` / `feature/y` on **clean** B and C (no leftover copied merge). After 8.1, those starts are legal. | **CLOSED** |
| **P2-7** | Row state `creating`→`ready`→`disposing`; one index commit point; `recover()` of intent × observed dest/pointer; path existence is not `ready` | Original mid-copy: path exists ⇒ live member. Now `creating` ∧ incomplete → delete partial dest, drop row (§3.1 :105; §4 :126–127). Original dispose drop-name-then-rm: now `disposing` is the commit point; dest still present → finish delete then sweep (:107). `ready` ∧ dest missing stays `ready` and use fail-closed (:109). Original `exists()`-as-stale is gone. A **new** clash between §4’s “crash before ready → drop” and the table’s creating∧complete → `ready` is **P2-9**, not an unclosed original. | **CLOSED** |
| **P3-1** | Folded into dest-nested refuse (S-P1-3) | §2 :33–35; §4 :121 refuse nested dest / dest that already contains `gwz.conf/gwz.yml`. | **CLOSED** |
| **P3-2** | Recapture dest lock after clean/`-b` (S-P2-1) | §4.2 :174–184 freeze; recapture dest lock to dest HEAD; `-b` after `ready` then recapture desired branches. | **CLOSED** |
| **P3-3** | `family_id` in index+pointer; optional on the new request | §3 :81–84; `CloneLocalWorkspaceRequest.family_id=F(7)` optional; “Strike on the wire if index+pointer suffice” satisfied as optional copy, authority files. | **CLOSED** |

---

## Changed-range analysis

Revision 3 is a whole-file rewrite against every remplan row (C- and S-), not a surgical patch. Checked against remplan dispositions (without using the Safety report as evidence): dest `.gwz/` install + denylist + both-files refuse + refuse-verbatim-while-open (C-P0-1); computed pointer + `family_id` compare (C-P2-5); row states + `recover()` table (C-P2-7); git-ref vs `--remote` / `local_source_name` + reserved `origin` + no persisted family remotes (C-P2-2/C-P2-4); hub `gwz.conf` + `family-root` + `core.bare=true` (C-P2-3); new `ActionKind` 27/28 + §7 tick table (C-P2-1); §8.3 on clean clones (C-P2-6); gitfile refuse, index lock+CAS, dest uniqueness, dispose hazards, `--keep` pointer delete, op-time resolve, clean freeze, disband, pair-by-id — all present in the object.

**Outside those dispositions (or incomplete relative to them):**

- §6 states `UnknownLocal` on **all** `--remote` tokens that are not a `ready` family name, while §7 still says `PushRequest.remote` is “family name **or git remote**”. Remplan moved family names onto the existing `--remote` noun and did not write the git-remote else-clause. **P2-8.** Not a new architecture: same string-overload class as closed P2-2, now on `OperationPolicy.remote` / `PushRequest.remote`.
- §4 :126–127 “Crash before `ready` → recover drops the row and the partial dest” over-claims vs §3.1 creating ∧ dest **complete** → flip to `ready`. **P2-9.** Recover-contract bug in the remedy text, not a new coordinator.
- `--keep` is carved out of the disposing∧dest-present recover row with no replacement row (P3-4).
- Verbatim denylist omits `.gwz/merge/` (`done/` / `quarantine/`) even though remplan said copy merge only as a new dest identity or not at all (P3-5).
- No new architectural root-cause candidate for the two-round cap.

---

## 0. Evidence base

Tuple verified twice (`git rev-parse HEAD`; `shasum -a 256` on the design and remplan): HEAD `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`; design `1c543d04afe2850177c0e1098013f507f7412be77dacc8e441bea60b443a8df7`; remplan `0995242ab8ef41ad881b73845b44f7f346e8b6429ca50c2cadd9538da79672a1`. Neither moved.

Read: revision 3 entire; remplan entire; round-1 Consistency report entire (counterexample reconstruction). Live IR `gwz-core/protocol/gwz.taut.py` `ActionKind` :142–169, `OperationPolicy.remote` :827–828, `CloneWorkspaceRequest` :1442–1447, `PullHeadRequest` :1610–1611, `PushRequest` :1619–1624, `MergeRequest` :1655–1670 (`source_ref=F(3)`, `filesystem_strict=F(8)`). `gwz-core/docs/Protocol.md` Evolution (additive optional fields; new kinds the old binary never sends). Pull default remote: `pull_head_member_preflight.rs` :378–386 (policy.remote else `origin`). CLI: `gwz-cli/src/push_after.rs` example `gwz push --remote origin`. Discovery: `gwz-core/src/workspace/mod.rs` `WORKSPACE_MANIFEST` / `discover_workspace_root`. Transactional-apis.mdc :10–14. GwzWt banners only: `GwzWtDesign.md` :3–7, `GwzWtPlan.md` :3–5 UNACCEPTED / superseded by this file. I2 record location unchanged; revision 3 no longer rewrites merge-record paths (open merge ⇒ refuse verbatim).

Out of scope as briefed. Did not read `GwzLocalClone-ReviewSafety.md`.

---

## 1. Findings

### [P2-8] `--remote` is specified as family-only (`UnknownLocal`) and as “family name or git remote”

**Root cause.** Family names were moved onto the live `--remote` field without a dispatch else-clause for existing git remotes.

**Location.** Object §6 :237–248; §7 :292–293, :308–309, :313; §2 :42–45. Live: `OperationPolicy.remote=F(5)` “Preferred remote name for fetch/push” (`gwz.taut.py` :827–828); `PushRequest.remote=F(2)` (:1622); pull uses `policy.remote` if set else `origin` (`pull_head_member_preflight.rs` :378–386). CLI documents `gwz push --remote origin` (`push_after.rs` :3).

**Violated invariant.** §7: `PushRequest.remote` is “family name or git remote”. §6: name not in index or not `ready` → `UnknownLocal` on `--remote`. Those cannot both hold for `origin` (reserved as a clone name, therefore never a family member, and the normal git remote in `gwz.yml`).

**Reproduction (original P2-2 merge tokens do not apply; this is the pull/push field after the remedy).** After §8.1’s first successful `gwz clone --local --name A` (root now has an index):

1. `gwz pull --head --remote origin` — `origin` ∉ members → §6 `UnknownLocal`. Today it fetches GitHub `origin`.
2. `gwz push --remote origin` — same.
3. `gwz pull --head` with `--remote` omitted still defaults to `origin` in live pull (policy.remote absent) and is not covered by §6’s `--remote` rule — so the **explicit** documented spelling breaks and the implicit one does not.

`gwz pull --head --remote A` remains the family spelling and is consistent.

**Impact.** After any local clone, documented `gwz push --remote origin` / `gwz pull --head --remote origin` become `UnknownLocal` inside the family, including on `root`. Unstated incompatibility with live `OperationPolicy.remote` and `repo` remotes that §6 promised to leave origin-only in `gwz.yml`.

**Required correction.** Per-verb dispatch, in §6 and the §7 comments: if the token is a `ready` family name, bind from the index; else resolve as today’s git remote (`origin`, `upstream`, …); if neither, existing `missing_remote` (pull/push) or `UnknownLocal` (merge `--remote` / `local_source_name`, which stay family-only). Do not apply `UnknownLocal` to `--remote origin`. Merge `--remote origin` may still refuse (reserved / not a member).

**Closure / regression test.** After a successful local clone, `gwz pull --head --remote origin` and `gwz push --remote origin` remain git-`origin` operations; `gwz pull --head --remote A` is family bind; `gwz pull --head --remote C` after dispose C is `UnknownLocal`; `gwz merge --remote origin` stays a family miss.

This is **not** a new architectural root cause: it is P2-2’s dual-denotation left on the field the remplan assigned family names to.

---

### [P2-9] Create prose deletes a dest-complete `creating` row; `recover()` flips that same state to `ready`

**Root cause.** §4 summarises every crash-before-`ready` as drop; §3.1 distinguishes incomplete (drop) from dest-complete (promote).

**Location.** §4 :125–127 “Crash before `ready` → recover drops the row and the partial dest.” vs §3.1 :105–106 `creating` ∧ dest missing or incomplete → drop; `creating` ∧ dest complete (pointer present, no index on dest, members’ `.git` inside dest, dest `gwz.conf` present) → flip to `ready`. Transactional-apis.mdc :13: `recover()` is a function of recorded intent plus observed state — one function, not two.

**Violated invariant.** One recover() table. A dest that already satisfies the dest-complete predicate is not a “partial dest”. Implementing §4 as written destroys a finished clone whose only missing step is the `ready` CAS.

**Reproduction.** Create `--name A`: `creating` row committed; copy+install finished (pointer present, no dest index, `gwz.conf` present, `.git` inside dest); crash before the `ready` CAS. Next local verb:

- per table: recover → `ready`; A is a member.
- per §4: recover drops the row and deletes dest.

**Impact.** Implementers fork. The window is exactly the “one commit point + recover” story the remplan added. Deleting a dest-complete tree is data loss of a finished clone.

**Required correction.** Strike §4’s blanket drop. Create crash handling is **only** the §3.1 table: incomplete → delete dest + drop row; dest-complete → CAS to `ready`. Say that create’s in-process success path is the same promote, not a different rule.

**Closure / regression test.** Crash-table row: `creating` ∧ dest-complete ∧ not yet `ready` → unique outcome `ready`, dest retained; `list` from another member shows A. Incomplete dest still drops.

---

## 2. Invariant analysis

**Held (including remplan-required invariants).**

- Index only at root; clones have a computed pointer; both files on dest refuse (§3 :60–62). `family_id` compared; mismatch is `FamilyRootMismatch`.
- `CloneWorkspaceRequest.url` remains required; local clone is a new kind (`ActionKind.clone_local_workspace=27`); `MergeRequest.filesystem_strict=F(8)` is not reused; `local_source_name=F(9)` is free in live IR.
- Bare merge token is a git ref; `gwz merge A` is not clone A (§10 :457). Family merge is `--remote` + optional `source_ref`.
- Host paths stay out of `gwz.yml`; family URLs are not persisted remotes (§6, §10 :455–456).
- GwzWt banners: UNACCEPTED, superseded by this file for agent-lane isolation; object :6–8 names that scope (W-* not implementation authority).
- I2 v1 body / catalog tuple: not bumped; open gwz merge is not copied.
- Nested dest refuse; dest uniqueness; gitfile/`commondir` refuse; pair-by member id / `source_id`.
- §8.3 is legal after §8.1 under refuse-verbatim-while-open: A has no copied merge; B and C are clean.

**Broken** — P2-8, P2-9.

**Unstated impacts (non-blocking).** Live `CommandArgs::Clone` still refuses `--dry-run`; revision 3 reuses the `gwz clone` verb for a new request kind and does not say whether `--dry-run` becomes a plan of the `creating` row. Mutator-lock copy is omitted (`§4.1` `.gwz/locks/`). Discovery via `gwz.conf/gwz.yml` is stated for hub. `repo sync` is named as must-not-capture family URLs.

---

## 3. Risks and next action

NO-GO on revision 3: round-1 P0-1 and P2-1…P2-7 (and P3-1…P3-3) are closed on the text, not merely claimed in the remplan. The remaining blockers are remedy residuals: `--remote` still needs a git-remote fallback (P2-8), and create must defer crash recovery to the table (P2-9). Neither is a third architectural root cause.

I pre-commit to GO on a revision that resolves **P2-8** and **P2-9** as specified. Fold P3-4/P3-5 if cheap; they do not independently block.

### Non-blocking

**[P3-4] `--keep` has no `recover()` row.** §3.1 :107 “`disposing` ∧ dest still present **(unless `--keep` recorded)**” carves `--keep` out of finish-delete and does not map it. Crash after pointer delete, before sweep/drop-row: dest present, no pointer, row still `disposing`. Correction: `disposing` ∧ `--keep` → finish sweep, drop row, dest remains, pointer absent. Test: that unique outcome.

**[P3-5] Verbatim denylist omits `.gwz/merge/`.** Remplan: copy merge only as a new dest identity, or refuse open. Revision 3 refuses open and says “Do not copy `.gwz/merge/`” only in that refuse paragraph. With no open merge, `done/` and `quarantine/` copy with source `merge_id`s. Correction: add `.gwz/merge/` to the denylist (or rewrite ids if copied). Test: dest has no `.gwz/merge/` after verbatim from a root that has `done/` archives.
