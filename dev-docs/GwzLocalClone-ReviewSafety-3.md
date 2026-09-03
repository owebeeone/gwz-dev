# GwzLocalClone — SAFETY-AXIS REVIEW (round 3)

**Review object:** working-tree file `dev-docs/GwzLocalCloneDesign.md` (UNTRACKED; DRAFT revision 4, 2026-09-03; SHA-256 `8ce70d8add0163839952ef2ac84623040d75be2a0d7c284ee1511b6dde770e3e`).
**Baseline:** gwz-dev HEAD `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`. End-of-review re-hash and `git rev-parse HEAD` matched the start tuple; the object did not move. RemPlan-2 SHA-256 `5c3a114a1c89368332a82fbb88caf2ceec6c56f99ddc54a9f52b108e7172b5db`.
**Date:** 2026-09-03
**Axis:** SAFETY — degraded and mixed-version paths; irreversible steps and their preconditions; disclosure/privacy scale; stuck states reachable under the text’s own rules; whether “never worse than the status quo” survives concrete interleavings; scope creep that widens blast radius.
**Verdict: GO** — every round-1 and round-2 blocking finding is closed on revision 4. The recover/install grammar now covers dest-complete (recapture, `-b`, omit-list, `gwz.conf` last), `disbanding`, `keep` / named `force_hazards`, source mutator exclusion on verbatim, and refuse of absolute git config that would still name source. No new P0–P2. **No new architectural root cause**; the two-round cap does not fire.

---

## Prior-finding closure table

Round-1 counterexamples reconstructed from `GwzLocalClone-ReviewSafety.md` (revision 2 gone). Round-2 counterexamples reconstructed from `GwzLocalClone-ReviewSafety-2.md` (revision 3 gone). “Verified” = that sequence re-traced on revision 4 text.

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| **P0-1** | Dest `.gwz/` install; omit identity stores; refuse verbatim while a gwz merge is open; both index and pointer refuse | Open-merge-then-verbatim **refuses** under the source mutator lock (§4.1:204–206, §8.1:413–416). Omit table includes index, catalog, checked-artifacts, locks, stash, **and `.gwz/merge/` open+`done/`** (§4.1:190–200). Both files together refuse (§3:63–65). Create from dest registers at the real root (§8.1:422–424). | **CLOSED** |
| **P0-2** | Refuse gitfile / external `commondir` / worktree sharing | Gitfile / linked-worktree source refuses (§4.0:171–178, §10:536–538). Dest `--git-common-dir` must be inside dest. Shared-refdb CE does not survive. | **CLOSED** |
| **P1-1** | Row state + `recover()` of intent × observed; path existence is not `ready` | §3.1 is the only create-crash authority (§3.1:108–109, §4:167). `creating` / `ready` / `disposing` plus index `disbanding`. Crash before copy: dest missing → drop row (§3.1:128). Mid-copy without `gwz.conf` and matching freeze → delete dest, drop row. Partial dest is not `ready`. | **CLOSED** |
| **P1-2** | Root-index lock + CAS; name-taken inside the lock | §3.1:99–101, §4:153–162. Two racing creates for `C` serialize; second sees the name taken. | **CLOSED** |
| **P1-3** | Dest absent or empty+unregistered; dest path unique; nested dest refuses | §2:33–36, §4:157–159, §8.1:442–443. | **CLOSED** |
| **P1-4** | Refuse dirty / open merge unless `--force` **names** hazards; hold C’s mutator lock | §5:267–271, §8.4:496–499 (`--force` with no names refuses). Sweep is `disposing` recover. No persisted family remotes. | **CLOSED** |
| **P1-5** | `--keep` deletes dest pointer; `NotAFamilyMember`; rejoin explicit | §5:272–275, §8.4:501–503. Kept tree cannot dispose a sibling. | **CLOSED** |
| **P1-6** | Never copy the pointer; write fresh path + `family_id` | §3:59–60, §4.1:195, §8.1:422–424. | **CLOSED** |
| **P1-7** | Family bindings not persisted git remotes; `gwz.yml` origin-only | §6:314–316, §3:67–68, §4.1:202. Pull-then-`repo sync` cannot write family URLs into `gwz.yml`. | **CLOSED** |
| **P2-1** (still **OPEN** in round 2) | Recapture dest lock **before** dest-complete; dest-complete includes lock/HEAD; `-b` before `ready`; failed recapture does not flip `ready` | dest-complete requires `--clean`/`--bare` lock members’ commits equal dest HEADs, “recapture already done” (§3.1:120–121). `-b` exists on every dest member (§3.1:122). Recapture and `-b` run **before** dest-complete (§4:164–166, §4.2:222–229). `creating` ∧ dest-complete except recapture/`-b` cannot finish → **do not** flip `ready` (§3.1:131). Original CE (source lock pin X, live HEAD Y, crash after install before recapture): dest-complete is false; dest is not `ready` with lock ≠ HEAD. Success path: dest lock commits equal dest HEADs (§8.1:419–420). | **CLOSED** |
| **P2-2** | Bare merge token is a git ref; family is `--remote`; reserved names | §2:43–48, §6:309–312, §8.2:479–480. `--name origin` refuses. `pull --head --remote origin` remains git-`origin` (§6:305–307, §8.2:465–466). | **CLOSED** |
| **P2-3** | Hub member `.git` is `core.bare=true`; hub is a gwz workspace | §4.3:241–245, §8.1:429–431. | **CLOSED** |
| **P2-4** | `dispose root` never deletes the original tree; disband keeps trees; `--force` without a row never deletes by path guess | §3:92–95, §5:264, 280–287, §8.4:511–518. | **CLOSED** |
| **P2-5** | `family_id` in the pointer; mismatch → `FamilyRootMismatch`, no write | §3:83–90, §3.1:136. | **CLOSED** |
| **P2-6** | Pair by dest lock member id / `source_id`; set mismatch refuses | §6:297–301. | **CLOSED** |
| **P1-8** (round 2) | Index `disbanding`; recover deletes pointers only; never delete trees; do not route leftover `ready` rows through dispose-delete | `family_state: disbanding` (§3:74, 93–94). recover: delete remaining clone `family-root` only; when none remain, drop the index; **never** delete trees (§3.1:126). While `disbanding`, `dispose <name>` is `UnknownLocal` (or pointer removal only), not `rm -rf` (§3.1:138–140, §5:261, 284–287). Original CE (crash after N of M pointers, then `dispose A` deletes A): **refuses / UnknownLocal**; next local verb finishes pointer removal; every dest directory still exists. | **CLOSED** |
| **P1-9** (round 2) | dest-complete → CAS `ready`, dest retained; auto-delete only if not discoverable **and** git matches freeze; drifted or `gwz.conf` present → stay `creating`, `CreatingDest`; only `dispose --force` (named) unlinks; `gwz.conf` last; omit-list during copy | Original CE (copy finished, no pointer, dest has `gwz.conf`, human unique commit, next recover `rm -rf`s dest): `creating` ∧ incomplete ∧ (`gwz.conf` present **or** dest git drifted) → stay `creating`; fail-closed; only named `--force` unlinks (§3.1:130). Auto-delete only when no `gwz.conf` **and** dest git matches recorded freeze (§3.1:129). dest-complete includes omit-list absent, every dest-manifest `.git` inside dest, recapture (`--clean`), recorded `branch` (§3.1:111–122). Install writes `gwz.conf` **last**; crash handling is §3.1 only (§3.1:108–109, §4:163–167). Planted unique dest commit → recover must not delete. | **CLOSED** |
| **P1-10** (round 2) | Always omit `.gwz/merge/` (open and `done/`); dest-complete fails if non-empty; verbatim holds source mutator lock from check through copy+install | Omit table row `.gwz/merge/` (open **and** `done/`); dest merge store starts empty (§4.1:199, 210). dest-complete requires omit-list absent including empty merge store (§3.1:115). Verbatim **and** `--clean` hold the source mutator lock from §4.0 / open-merge check through copy+install (§4:153–155, §4.1:187–188, 204–206). Original CE (merge starts during copy, dest ingest `.gwz/merge/`): source mutator lock blocks the merge start; merge files are omitted anyway. | **CLOSED** |
| **P2-7** (round 2) | Refuse absolute `include.path` / `includeIf` / `hooksPath` / `core.worktree` / `insteadOf` that would still name a path outside dest; dest-complete fails if dest config names a path outside dest | §4.0:175–181 default **refuse**. dest-complete §3.1:118–119. In-member absolute `hooksPath` still names source after copy → refuse. `repo sync` cannot pick up remotes from those includes on a dest that was allowed to complete. | **CLOSED** |
| **P2-8** (round 2) | Record `keep`, named `force_hazards`, `branch` on the row; BOOL-only `force` withdrawn; `-b` before `ready`; recover never finish-delete a `--keep` dest | Intent sketch includes `branch`, `keep`, `force_hazards`, `freeze` (§3:77–80). Wire: `force_hazards=F(5, STR, repeated=True)`; BOOL-only withdrawn (§7:352–355, §10:547). `-b` before dest-complete (§4.2:225–229). `disposing` ∧ recorded `keep` → dest remains, pointer absent, **never** finish-delete (§3.1:132). `--force` with no names refuses (§5:269, §8.4:498). `--clean -b lane/x` success ⇒ branch exists on every member **or** dest is still `creating` (dest-complete §3.1:122, 131). | **CLOSED** |

---

## Changed-range analysis

Revision 4 is one design patch applying RemPlan-2. Every Safety-2 row is present in the text: dest-complete extended (omit-list, recapture, `-b`, dest-local config); `creating` recover split discoverable/drifted vs not; `gwz.conf` last; `family_state: disbanding`; `.gwz/merge/` on the omit table; source mutator lock on verbatim; `keep` / `force_hazards` / `branch` / `freeze` on the row; refuse of absolute git config; pull/push `--remote` family-if-ready-else-git, merge `--remote` family-only.

**Outside those dispositions:** lock acquisition is “root-index lock **and** source mutator lock” for the whole copy (§4:153–155) with index named first, then dest mutator only on dispose of C — same order, no new coordinator. `CreatingDest` as a typed refuse. Pull `--remote C` after dispose is `missing_remote` (not `UnknownLocal`) because of git-remote fallback; merge `--remote C` stays `UnknownLocal` (§8.4:506–507). That is the RemPlan-2 Consistency dispatch, not a safety widening of blast radius.

**NEW ARCHITECTURAL root-cause candidates:** **none.** This is still dest-install + one index CAS + `recover(intent × observed)`. Added rows (`disbanding`, dest-complete conjuncts, keep/force_hazards) are coverage of that grammar. The cap does not fire.

---

## 0. Evidence base

Tuple (start and end, cwd `/Users/owebeeone/limbo/gwz-dev`):

- `git rev-parse HEAD` → `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`
- `shasum -a 256 dev-docs/GwzLocalCloneDesign.md` → `8ce70d8add0163839952ef2ac84623040d75be2a0d7c284ee1511b6dde770e3e`
- `shasum -a 256 dev-docs/history/GwzLocalClone-RemPlan-2.md` → `5c3a114a1c89368332a82fbb88caf2ceec6c56f99ddc54a9f52b108e7172b5db`

Read: revision 4 object (full); RemPlan-2 (full); filed `GwzLocalClone-ReviewSafety.md` and `GwzLocalClone-ReviewSafety-2.md`; `.cursor/rules/transactional-apis.mdc`; `GwzWtDesign.md` status banner only (UNACCEPTED). Did not read Consistency-axis reports.

---

## 1. Findings

None. No still-open P0–P2 and no new blocking defect on revision 4.

Attacks that did **not** yield a defect (so an auditor can see they were run):

- **Disbanding vs leftover `ready` rows:** recover runs first on every local verb and finishes pointer removal; `dispose A` while `disbanding` is not `rm -rf` (§3.1:138–140).
- **`creating` incomplete + discoverable:** `gwz.conf` present or dest git drifted from freeze → stay `creating`, `CreatingDest`; auto-delete is only the non-discoverable + freeze-matching partial copy (§3.1:129–130).
- **dest-complete:** recapture, `-b`, omit-list (including empty merge store), every dest-manifest `.git` inside dest, dest-local config, pointer, no dest index — all required before `ready` (§3.1:111–122).
- **`gwz.conf` last:** crash after copy before last write is “not discoverable” if the manifest is not yet there; crash after last write with dest-complete true recovers to `ready` and retains dest (§3.1:127). Write-last is the controlling sentence for when dest becomes a workspace (§3.1:142–144); the copy omit table is identity stores, not a second copy of that rule.
- **Source mutator lock on verbatim:** held from gitfile/open-merge check through copy+install; concurrent `gwz merge` on source cannot inject `.gwz/merge/` (§4:153–155, §4.1:187–188).
- **`--force` named / `--keep` recorded:** empty `force_hazards` refuses; keep recover never finish-deletes (§5:269–275, §3.1:132).
- **`include.path`:** refuse if it would still name a path outside dest; dest-complete re-checks (§4.0:175–181).
- **Pull/push `--remote` fallback:** `origin` stays git-`origin` after a local clone exists; family names that are `ready` bind from the index; `origin` cannot be a clone name (reserved). Merge `--remote origin` is a family miss, not a silent git bind (§6:295–312).
- **Lock order:** create takes root-index then source mutator (sentence order matches dispose: index then C’s mutator). No ABBA deadlock specified.

---

## 2. Invariant analysis

| Surface | Revision 4 | Survives? |
|---|---|---|
| Verbatim copies family index / catalog / merge store | Omit table + dest-complete + both-files refuse + source mutator lock | Yes |
| Gitfile shared refdb | §4.0 refuse | Yes |
| Register-before-copy / half dest | §3.1 only; `ready` is dest-complete | Yes |
| Two creates, same name | Index lock + CAS | Yes |
| Dest occupancy / two names one path | Unique dest; nested refuse | Yes |
| Clone-of-clone pointer | Fresh pointer + `family_id` | Yes |
| Open merge copied | Omit `.gwz/merge/`; lock through copy | Yes |
| Family remotes in `gwz.yml` | Operation-time bind; origin-only remotes | Yes |
| `--clean` mixed HEADs / lock-drifted `ready` | Freeze once; recapture in dest-complete | Yes |
| Name vs git ref | Merge token vs `--remote`; origin reserved | Yes |
| Bare hub | `core.bare=true` | Yes |
| Dispose unique work | Named `--force` or refuse | Yes |
| `--keep` ghost admin | Pointer deleted; keep recorded | Yes |
| Dispose root / partial disband | `disbanding`; trees stay | Yes |
| Pair by path | Pair by id / `source_id` | Yes |
| Pointer `family_id` | Compare; mismatch no write | Yes |
| `creating` recover deletes unique dest commits | Only non-discoverable + freeze-matching partials | Yes |
| Absolute git config / include.path | Refuse; dest-complete | Yes |
| Reflink else copy | Unchanged; hardlink not authorized | Yes |
| GwzWt blast radius | Gitfile refuse; empty dest merge store; no shared catalog | Yes |

---

## 3. Risks and next action

Revision 4 closes the round-2 safety NO-GO on the same recover/install grammar. Dest cannot be published `ready` lock-drifted; disband cannot delete trees via leftover `ready` rows; `creating` recover cannot `rm -rf` a dest that is already a workspace or has drifted git; verbatim cannot ingest `.gwz/merge/` under a check-then-copy race.

This axis **GO**s revision 4 at SHA-256 `8ce70d8add0163839952ef2ac84623040d75be2a0d7c284ee1511b6dde770e3e` (HEAD `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`). Acceptance still needs the other axis’s GO on this same tuple. Do not implement until that merge. No third architectural root cause; the lane is not stopped for redesign.
