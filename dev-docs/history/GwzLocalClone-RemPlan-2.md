# GwzLocalClone — RemPlan-2

**Status:** remplan for design revision 4. Not implementation. **Last
remediation round** on this object (round 1 was `GwzLocalClone-RemPlan.md`
→ revision 3).
**Reviewed object:** `dev-docs/GwzLocalCloneDesign.md` SHA-256
`1c543d04afe2850177c0e1098013f507f7412be77dacc8e441bea60b443a8df7`
at gwz-dev `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`.
**Reviews:** `GwzLocalClone-ReviewConsistency-2.md` (NO-GO; pre-commit
P2-8, P2-9), `GwzLocalClone-ReviewSafety-2.md` (NO-GO; still-open P2-1;
new P1-8, P1-9, P1-10, P2-7, P2-8).
**Lane-owner merge:** dual NO-GO. One design patch. No code.
**Cap:** both reviewers classified **no new architectural root cause**.
Coverage holes in the same recover/install grammar. Cap does not fire.

IDs below are `C-` (Consistency-2) and `S-` (Safety-2). Blind
convergence is named. P3s do not block; fold if cheap.

Round-1 blocking IDs stay **closed** unless a row below reopens one
(S-P2-1 is the only still-open round-1 ID).

## Blind convergence (highest confidence)

| Defect | IDs | Disposition |
|---|---|---|
| §4 “crash before ready → drop dest” vs §3.1 dest-complete → `ready`; auto-delete of a dest that is already a workspace | **C-P2-9, S-P1-9** | **One recover table; no §4 override.** dest-complete → CAS `ready`, dest retained. Incomplete **and** dest is not a discoverable workspace (no `gwz.conf`) → delete dest, drop row. Incomplete **and** dest is already a workspace (`gwz.conf` present) **or** dest git has commits/members not in the recorded source/freeze → stay `creating`, fail-closed, only `dispose --force` (named) unlinks. Dest is not discoverable until install finishes: write `gwz.conf` last (or a `creating` sentinel that ordinary gwz verbs refuse as `CreatingDest`). dest-complete also requires: omit-list paths absent, every dest-manifest member `.git` inside dest, pointer present, no dest index. |
| `.gwz/merge/` not on the omit table; check-then-copy TOCTOU | **C-P3-5, S-P1-10** | **Always omit `.gwz/merge/`** (open and `done/`). dest merge store starts empty. dest-complete fails if dest `.gwz/merge/` is non-empty. Verbatim holds the **source mutator lock** from open-merge/gitfile check through copy+install (same exclusion `--clean` already uses). |
| `--keep` carved out of recover with no replacement row; `keep` not in intent | **C-P3-4, S-P2-8 (keep half)** | Record `keep` on the disposing row. recover: `disposing` ∧ `keep` → finish sweep, drop row, dest remains, pointer absent. Never finish-delete a `--keep` dest. |

These are **not** a third architecture. Same install + recover grammar,
missing rows and a missing omit.

## Consistency-only

| ID | Disposition | Closure |
|---|---|---|
| C-P2-8 `--remote` family-only vs “family or git remote” | Per-verb dispatch in §6 and §7: if token is a `ready` family name, bind from the index; **else** today’s git remote (`origin`, `upstream`, …); if neither, existing `missing_remote` (pull/push) or `UnknownLocal` (merge `--remote` / `local_source_name`, family-only). `gwz pull --head --remote origin` stays git-origin after a local clone exists. Merge `--remote origin` may still refuse (reserved / not a member). | After a successful local clone: `pull --head --remote origin` and `push --remote origin` are git-`origin`; `--remote A` is family; `--remote C` after dispose C is `UnknownLocal` on family verbs; `merge --remote origin` is a family miss. |

## Safety-only

| ID | Disposition | Closure |
|---|---|---|
| S-P2-1 dest `ready` lock-drifted | Recapture dest lock (and dest desired branches) **before** dest-complete may be true. dest-complete includes dest lock/HEAD agreement at the freeze. `creating` recover does not flip `ready` until that holds; if recapture cannot be done, dest is incomplete (then C-P2-9/S-P1-9 rules apply — do not `ready` a drifted dest). `-b` completes **before** `ready` (recorded `branch` on the creating row). | Source HEAD ≠ lock pin; `--clean`; crash after dest `.gwz/` install before recapture → not `ready` with lock ≠ HEAD. Success: dest lock members’ commits equal dest HEADs. |
| S-P1-8 `disband` has no recover | Index state `disbanding` (CAS, same root-index lock). recover: for each remaining clone pointer, delete `family-root` only; when no clone pointers remain, drop the index; **never** delete trees. Do not route leftover `ready` rows through dispose-delete while `disbanding`. `dispose root` stays refuse. | Crash after N of M pointers removed; next local verb finishes pointer removal and drops the index; every dest directory still exists. Subsequent `dispose A` is `UnknownLocal`, not `rm -rf`. |
| S-P2-7 absolute `include.path` / in-member `hooksPath` | Refuse **or** rewrite/strip any absolute `include.path` / `includeIf` / `hooksPath` / `core.worktree` / `insteadOf` that does not resolve inside **dest** after copy. dest-complete fails if dest config still names a path outside dest. | Planted absolute `include.path` on source → create refuses or dest config has no path outside dest. `repo sync` after verbatim does not add remotes. In-member absolute `hooksPath` cannot remain pointing at source. |
| S-P2-8 `--force` BOOL; `-b` after `ready` | Record `keep`, named `force_hazards`, and `branch` on the row at the disposing/creating commit point. `--force` without named hazards refuses. Wire: replace BOOL `force` with a named-hazards field (or keep BOOL **and** a repeated STR of hazard names; BOOL-only is withdrawn). `-b` before `ready` (see S-P2-1). | `--keep` + crash after pointer removal → dest remains, row dropped, `NotAFamilyMember`. `--force` with no hazard names refuses. `--clean -b lane/x` success ⇒ `lane/x` exists on every member **or** dest is still `creating`. |

## Non-blocking (fold)

- C-P3-4 / C-P3-5 — folded into the convergence rows above (they are
  blocking on the Safety axis).

## Next

One revision of `GwzLocalCloneDesign.md` applying every row above.
Same reviewers re-verdict. Do not implement from revision 3.
This is the last allowed remediation patch; a **new architectural**
root cause on that re-verdict stops the lane.
