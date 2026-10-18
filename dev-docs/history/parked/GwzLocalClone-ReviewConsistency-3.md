# GwzLocalClone — CONSISTENCY-AXIS REVIEW (round 3)

**Review object:** working-tree file `dev-docs/GwzLocalCloneDesign.md` (UNTRACKED; DRAFT revision 4, 2026-09-03; SHA-256 `8ce70d8add0163839952ef2ac84623040d75be2a0d7c284ee1511b6dde770e3e`)
**Baseline:** gwz-dev HEAD `68b6ba43050426a77447f3d9c4a00fc8eb2d380b` (start and end; object and RemPlan-2 digests unchanged). RemPlan-2 `dev-docs/history/GwzLocalClone-RemPlan-2.md` SHA-256 `5c3a114a1c89368332a82fbb88caf2ceec6c56f99ddc54a9f52b108e7172b5db`. Round-2 object `1c543d04…` is gone; counterexamples reconstructed from `dev-docs/history/GwzLocalClone-ReviewConsistency-2.md` and re-traced on revision 4. Round-1 report `dev-docs/history/GwzLocalClone-ReviewConsistency.md`.
**Date:** 2026-09-03
**Axis:** CONSISTENCY — the document against its controlling graph
**Verdict: NO-GO** — round-1 P0-1 and P2-1…P2-7 stay closed; round-2 P2-8 and P2-9 are closed on revision 4. One new P2 (P2-10) is open. **Not a new architectural root cause** (example-sequence coverage hole in the same §8 narrative; two-round cap does not fire). I pre-commit to GO on a revision that resolves **P2-10** as specified.

---

## Prior-finding closure table

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| **P0-1** | Dest `.gwz/` install; never copy index; new `family-root`; both files refuse; refuse verbatim while open merge | Original: verbatim from root copies `local-family.yml`, dest has no pointer, dest is a second root. Rev4 §4.1 denylist omits index and source pointer; installs new `family-root`; dest-complete requires no dest index + pointer present; both files refuse. §8.1: “family-root → root + `family_id`; no `local-family.yml`”. From A, B “registers on root, not on A”. Original step 4 does not hold. | **CLOSED** (stays closed) |
| **P2-1** | New `ActionKind`; `CloneWorkspaceRequest.url` stays required; §7 table ticks live types | Live `ActionKind` ends at `log=26` (`gwz.taut.py` :169); 27/28 unused. `url=F(2, STR)` required (:1445). `source_ref=F(3)` (:1658); `filesystem_strict=F(8)` (:1670); F(9) unused. `PushRequest.remote=F(2)` (:1622). `PullHeadRequest` is `meta` only; `OperationPolicy.remote=F(5)` (:828). Sketch does not invent `CloneRequest` / `PullRequest` / `MergeRequest.source`. §7 table maps each distinct §8 family CLI shape to a live type or an explicitly new tag (including `--remote origin` / `--force` hazards). | **CLOSED** (stays closed) |
| **P2-2** | Bare merge token is a git ref; family merge is `--remote` / `local_source_name` | Original: `gwz merge feature/x` was §6 `UnknownLocal`. Rev4 §6 :311–312: bare merge token is a git ref, not `UnknownLocal`. §8.2 :479–480: `gwz merge feature/x` git ref; `gwz merge A` is git ref `A`. Family merge: `gwz merge --remote A` → `local_source_name`. After dispose, `gwz merge --remote C` is `UnknownLocal`. | **CLOSED** (stays closed) |
| **P2-3** | Hub is a workspace: `gwz.conf` + `family-root`; `core.bare=true` | Original “only `.git` folders” is still gone. §4.3 :241–245: real bare, hub **is** a gwz workspace (`gwz.conf` last + `family-root`). | **CLOSED** (stays closed) |
| **P2-4** | Reserve `origin`; refuse names that are already git remotes; no persisted family remotes | Original `--name origin` still refused (§2 :37–39; §8.1 :436–437). §6 :314–316: op-time resolve; `repo sync` must not write family URLs. Dispose does not delete git remotes by name. | **CLOSED** (stays closed) |
| **P2-5** | Never copy pointer; write dest-relative/canonical path + `family_id` | Original source→dest rewrite of a copied pointer still gone. §3 :59–60 never copied; §4.1 installs a **new** file; §8.1 :424 computed, not copied. | **CLOSED** (stays closed) |
| **P2-6** | Refuse verbatim while open; §8.3 uses clean clones | Original: 8.1 left A with a copied open merge, 8.3 started on A. Rev4: verbatim refuses open merge (§4.1 :204–206); 8.1 aborts then clean-creates; 8.3 starts on clean B and C. After 8.1 those starts remain legal. (New 8.4→8.5 sequencing is **P2-10**, not a reopen.) | **CLOSED** (stays closed) |
| **P2-7** | Row states; `recover()` of intent × observed; path existence is not `ready` | Original mid-copy `exists()` ⇒ live member. Rev4: `creating` ∧ incomplete ∧ not discoverable ∧ git matches freeze → delete dest, drop row; `ready` ∧ dest missing stays `ready`, use fail-closed. Dispose commit point is `disposing` with recorded `keep` / `force_hazards`. Original `exists()`-as-stale does not hold. | **CLOSED** (stays closed) |
| **P3-1** | Nested dest refuses | §2 :34–36; §4 :157–159. | **CLOSED** |
| **P3-2** | Recapture dest lock; `-b` before `ready` | §4.2 :222–229 recapture before dest-complete; `-b` before `ready`; dest-complete includes lock/HEAD agreement and recorded branch exists. | **CLOSED** |
| **P3-3** | `family_id` in index+pointer; optional on new request | §3 :87–90; `CloneLocalWorkspaceRequest.family_id=F(7)` optional. | **CLOSED** |
| **P2-8** | Per-verb `--remote` dispatch: ready family name else git remote; merge family-only | Original: after first local clone, `gwz pull --head --remote origin` and `gwz push --remote origin` were §6 `UnknownLocal`. Rev4 §2 :43–48, §6 :295–307: pull/push step 1 family-if-ready, else today’s git remote, else `missing_remote`. §7 :380, :387 and §8.2 :465–466 / §10 :546: those two commands remain git-`origin`. `gwz pull --head --remote A` is family bind. `gwz merge --remote origin` is `UnknownLocal` (family-only). After dispose C, pull `--remote C` is `missing_remote` (§8.4 :506), merge `--remote C` is `UnknownLocal` — matches the required correction’s pull vs merge split (the round-2 closure sentence that said `UnknownLocal` for pull-C is superseded by the remplan’s `missing_remote` for pull/push; the **origin** counterexample is gone). | **CLOSED** |
| **P2-9** | One recover table; no §4 override; dest-complete → `ready`, dest retained | Original: `creating` ∧ dest-complete ∧ crash before `ready` CAS: table said promote, §4 said drop dest. Rev4 §3.1 :108–109 “Create crash handling is **this table only** — §4 does not override it.” Table :127 dest-complete → CAS `ready`; dest retained. §4 :167 “Crash before `ready` → **§3.1 only**.” Original fork does not hold. | **CLOSED** |
| **P3-4** | Record `keep`; `disposing` ∧ `keep` → sweep, drop row, dest remains, pointer absent | Original: `--keep` carved out of recover with no replacement. Rev4 §3.1 :132 that exact row; §5 :270–275 records `keep` at the disposing commit point. | **CLOSED** |
| **P3-5** | Always omit `.gwz/merge/` (open and `done/`); dest-complete fails if non-empty | Original: denylist omitted `.gwz/merge/`. Rev4 §4.1 :199 omit open **and** `done/`; dest merge store starts empty; dest-complete :115 omit-list absent including empty `.gwz/merge/`; post-copy :210 dest `.gwz/merge/` must be empty. | **CLOSED** |

---

## Changed-range analysis

Revision 4 is a whole-file fold of every RemPlan-2 row onto revision 3, not a surgical patch.

**Applied as specified:**

- C-P2-9 / S-P1-9: one recover table; dest-complete → `ready`; incomplete+not-discoverable+git-matches-freeze → delete; incomplete+`gwz.conf` or git-drift → stay `creating` / `CreatingDest`; §4 defers to §3.1; `gwz.conf` last / `CreatingDest` sentinel language; dest-complete includes omit-list, pointer, no dest index, config-inside-dest, lock/HEAD for clean/bare, recorded branch.
- C-P3-5 / S-P1-10: `.gwz/merge/` always omitted; source mutator lock through verbatim copy+install.
- C-P3-4 / S-P2-8 keep: `keep` on disposing row; recover never finish-deletes a `--keep` dest.
- C-P2-8: per-verb `--remote` dispatch in §2, §6, §7, §8, §10.
- S-P2-1: recapture and `-b` before `ready`; dest-complete includes lock/HEAD; creating recover does not `ready` a drifted dest.
- S-P1-8: `family_state: disbanding`; pointer-only recover; never delete trees; `dispose` during disband is `UnknownLocal`.
- S-P2-7: refuse absolute `include.path` / `hooksPath` / `core.worktree` / `insteadOf` that would still name outside dest; dest-complete fails if dest config still does.
- S-P2-8 force/`-b`: BOOL-only `force` withdrawn; `force_hazards` repeated STR; `--force` with no names refuses; `-b` before `ready`.

**Outside those dispositions:**

- §8.4 now ends with `gwz local disband` (illustrates S-P1-8) and §8.5 still uses family `--remote hub`. That sequence was not in RemPlan-2’s closure tests. **P2-10.** Same class as closed P2-6 (evidence section not satisfiable as written). **Not a new architectural root cause.**
- Copy-as-it-sits still includes committed `gwz.conf/` while install says write `gwz.conf` last (P3-6). Same install grammar, omit-list incomplete relative to the write-last sentence.
- New-message sketch uses `repeated=True` rather than taut `List(STR)` (P3-7).
- No third architectural root cause. Recover/install/`--remote` grammar is the revision-3 architecture with missing rows filled.

---

## 0. Evidence base

Tuple verified twice (`git rev-parse HEAD`; `shasum -a 256` on the design and RemPlan-2): HEAD `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`; design `8ce70d8add0163839952ef2ac84623040d75be2a0d7c284ee1511b6dde770e3e`; RemPlan-2 `5c3a114a1c89368332a82fbb88caf2ceec6c56f99ddc54a9f52b108e7172b5db`. Neither moved.

Read: revision 4 entire; RemPlan-2 entire; round-2 Consistency report (P2-8/P2-9/P3-4/P3-5 counterexamples); round-1 Consistency report as needed for P0-1…P2-7. Live IR: `gwz.taut.py` `ActionKind` :142–169 (`log=26`), `Selection.member_ids=F(2, List(STR))` :808 (taut list spelling), `OperationPolicy.remote=F(5)`, `CloneWorkspaceRequest` :1442–1447, `PushRequest.remote=F(2)` :1622, `MergeRequest` :1655–1670 (`source_ref=F(3)`, `filesystem_strict=F(8)`). Discovery keys on `gwz.conf/gwz.yml`. Transactional-apis.mdc :10–14. GwzWt banners only: UNACCEPTED, superseded by this file. Did not read Safety round-1 or round-2 reports.

---

## 1. Findings

### [P2-10] §8.5 is illegal after §8.4 `disband`

**Root cause.** Disband was appended to the sequential §8 narrative to close S-P1-8; §8.5 still assumes a live family and a `ready` hub.

**Location.** Object §8.4 :517–518 `gwz local disband` / “pointers + index gone”; §8.5 :521–526 `gwz push --remote hub`, `gwz pull --head --remote hub`, `gwz merge --remote B`; §6 :297–303 pull/push dispatch (ready family name else git remote else `missing_remote`); §6 :314–316 family bindings are not persisted git remotes.

**Violated invariant.** §8 is one family timeline (8.3’s open merge on C is the hazard 8.4 disposes; 8.1 creates hub). After disband, hub is not a `ready` family name and was never a persisted git remote → `--remote hub` is `missing_remote`. `gwz merge --remote B` is family-only `UnknownLocal`. The evidence section is not satisfiable as written — the same invariant as closed P2-6.

**Reproduction.** Execute §8.1 through §8.4 including the final `gwz local disband`. Then §8.5: `cd ~/limbo/gwz-dev-B && gwz push --remote hub`. Index is gone; no git remote `hub` (§6). Dispatch step 3: `missing_remote`. Same for pull `--remote hub` and merge `--remote B`.

**Impact.** A reader using §8 as the spec will believe hub still works after disband. The S-P1-8 illustration (disband leaves trees) is correct; it must not run before 8.5.

**Required correction.** Move `gwz local disband` to after 8.5 (e.g. §8.6 teardown), or mark 8.5 as a separate pre-disband vignette, or drop disband from the 8.1–8.5 script and keep it as a standalone example that does not precede hub use.

**Closure / regression test.** Numbered script: after 8.1–8.3, 8.5’s three commands succeed (family `--remote hub` / `--remote B`); disband runs last; after it, `--remote hub` is `missing_remote` and trees still exist.

**Cap classification:** not a new architecture. Same §8 sequential-satisfiability hole as P2-6; disband is new surface without re-ordering the later example.

---

## 2. Invariant analysis

**Held.**

- Index only at root; computed pointer; both files refuse; `family_id` compare.
- `CloneWorkspaceRequest.url` required; local clone is `ActionKind` 27; `filesystem_strict=F(8)` not reused; `local_source_name=F(9)`.
- Pull/push `--remote origin` remains git-`origin` after a local clone exists; merge `--remote origin` is a family miss; `gwz merge A` is a git ref.
- Recover table is the only create crash authority; dest-complete → `ready`; `--keep` has a row; `.gwz/merge/` omitted; `disbanding` never `rm -rf`.
- BOOL-only `--force` withdrawn; empty `force_hazards` refuses.
- `-b` and lock recapture complete before `ready`.
- Host paths stay out of `gwz.yml`; no persisted family remotes.
- GwzWt banners: UNACCEPTED; object supersedes GwzWt for agent-lane isolation.
- I2 merge-record version not bumped; dest merge store starts empty.
- §8.1–8.3 remain legal under refuse-verbatim-while-open.

**Broken** — P2-10 only (blocking).

**Unstated (non-blocking).** Live `gwz clone` still refuses `--dry-run`; local clone reuses the verb for a new request kind. `repo sync` is named as must-not-capture family URLs. Mutator-lock files are omitted.

---

## 3. Risks and next action

NO-GO: RemPlan-2 closed C-P2-8 and C-P2-9 (and the folded P3-4/P3-5) on the text, not merely by claim. The remaining blocker is §8.5 running after disband (P2-10). That is not a third architectural root cause; the cap does not fire. A third remediation confined to this non-architectural example reorder is permitted under process; a new architecture on that patch would stop the lane.

I pre-commit to GO on a revision that resolves **P2-10** as specified. Fold P3-6 and P3-7 if cheap; they do not independently block.

### Non-blocking

**[P3-6] `gwz.conf/` is not on the omit list, yet install writes `gwz.conf` last.** §4.1 copies “committed files” (includes `gwz.conf/gwz.yml`); §4 :167 / §3.1 :142–144 require writing `gwz.conf` last so dest is not discoverable. Recapture writes `gwz.conf/gwz.lock.yml` before dest-complete. Correction: omit `gwz.conf/gwz.yml` (the discovery file) during copy and write it last; lock recapture may create `gwz.lock.yml` earlier. If `gwz.yml` is copied early, recover’s `CreatingDest` row still fail-closes, so this is not P2-9 reopened. Test: mid-copy dest has no `gwz.yml`; dest-complete is false until that write.

**[P3-7] `force_hazards=F(5, STR, repeated=True)` is not taut DSL.** Live lists are `List(STR)` (`gwz.taut.py` :808). Correction: `force_hazards=F(5, List(STR))`. Test: §7 sketch parses as the same shape `regen.py` accepts.
