# GwzLocalClone — SAFETY-AXIS REVIEW (round 2)

**Review object:** working-tree file `dev-docs/GwzLocalCloneDesign.md` (UNTRACKED; DRAFT revision 3, 2026-09-03; SHA-256 `1c543d04afe2850177c0e1098013f507f7412be77dacc8e441bea60b443a8df7`).
**Baseline:** gwz-dev HEAD `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`. End-of-review re-hash and `git rev-parse HEAD` matched the start tuple; the object did not move. Remplan SHA-256 `0995242ab8ef41ad881b73845b44f7f346e8b6429ca50c2cadd9538da79672a1`.
**Date:** 2026-09-03
**Axis:** SAFETY — degraded and mixed-version paths; irreversible steps and their preconditions; disclosure/privacy scale; stuck states reachable under the text’s own rules; whether “never worse than the status quo” survives concrete interleavings; scope creep that widens blast radius.
**Verdict: NO-GO** — prior P2-1 is still open (dest can become `ready` lock-drifted). The recover/install remedy adds three new P1 holes (disband has no recover; `creating` recover can `rm -rf` a usable dest; verbatim can still ingest `.gwz/merge/`) and two new P2s. No new architectural root cause: these are coverage holes in the same CAS/`recover()`/dest-install grammar.

I pre-commit to GO on a revision that resolves **P2-1, P1-8, P1-9, P1-10, P2-7, P2-8** as specified.

---

## Prior-finding closure table

Original counterexamples reconstructed from `dev-docs/GwzLocalClone-ReviewSafety.md` (revision 2 is gone). “Verified” = that sequence re-traced on revision 3 text.

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| **P0-1** | Install dest `.gwz/`; omit index/catalog/checked-artifacts/locks/stash; refuse verbatim while a gwz merge is open; both index and pointer → refuse | Open-merge-then-verbatim now **refuses** (§4.1:160–162, §8.1:339–342). Verbatim from root without a merge **installs** `family-root`, omits `local-family.yml` / catalog / checked-artifacts; both files together refuse (§4.1:147–166, §3:60–62). Create from dest registers at the real root (§8.1:348–350). | **CLOSED** (merge-during-copy residual → new **P1-10**) |
| **P0-2** | Refuse gitfile / external `commondir` / `core.worktree` / absolute `hooksPath` outside the member | Gitfile / linked-worktree source **refuses** (§4.0:129–139, §10:449–450). Dest `--git-common-dir` must be inside dest. Shared-refdb CE does not survive. | **CLOSED** (absolute `include.path` / in-member `hooksPath` residual → new **P2-7**) |
| **P1-1** | Row state + `recover()` of intent × observed dest/pointer; `creating` ∧ incomplete → drop row and dest | Crash before copy / mid-copy: `creating` ∧ missing or incomplete → delete dest, drop row; not `ready` (§3.1:103–106, §4:125–127). Dispose commit point is `disposing` **before** delete (§5:220–228). Path existence is not `ready` (§3.1:96–97, 109). | **CLOSED** (weak dest-complete / auto-delete of a usable dest → new **P1-9**; disband missing from the table → new **P1-8**) |
| **P1-2** | Root-index lock + CAS; name-taken inside the lock | Every index mutation takes the root-index lock and is a CAS (§3.1:92–94, §4:118–124). Two racing creates for `C` serialize; second sees the name taken. | **CLOSED** |
| **P1-3** | Dest absent or empty+unregistered; dest path unique; not already a workspace; nested dest refuses | §2:33–35, §4:120–122, §8.1:368–369 (`A2` onto `A`’s dest refuses). | **CLOSED** |
| **P1-4** | Refuse dirty / open merge / `MERGE_*` unless `--force` names hazards; hold C’s mutator lock; sweep is `disposing` recover | §5:218–227. Open-merge dispose refuses with no index change unless `--force` (§8.4:430–431). No persisted git remotes to tear on D (§6:246–248). | **CLOSED** (`force` is a BOOL, does not name hazards → new **P2-8**) |
| **P1-5** | `--keep` deletes dest `family-root`; family verbs `NotAFamilyMember`; rejoin is explicit | §5:211–224. Kept tree cannot `dispose` a sibling. | **CLOSED** (`--keep` not in recorded intent → new **P2-8**) |
| **P1-6** | Never copy the pointer; write fresh path + `family_id` | Pointer is never copied (§3:56–58, §4.1:152). From A, B’s pointer is computed (§8.1:348–350). Non-sibling dest does not inherit `../gwz-dev`. | **CLOSED** |
| **P1-7** | Family bindings are not persisted git remotes; `gwz.yml` origin-only; strip file/credential remotes | §6:246–248, §3:64–65, §4.1:158, §10:455–456. Original pull-then-`repo sync` CE cannot write family URLs into `gwz.yml`. | **CLOSED** (`include.path` republication → new **P2-7**) |
| **P2-1** | Freeze lock+HEAD once under source mutator lock; recapture dest lock to dest HEAD; `-b` after `ready` | Freeze-once **does** close mixed-member HEAD movement (§4.2:170–172). Recapture is a §4.2 bullet and an §8.1 comment, **not** in dest-complete or §4 step 3. Default freeze is recorded HEAD (§4.2:174–177, §11.7). Source lock pin X + live HEAD Y: dest members at Y, dest `gwz.lock.yml` still the committed X until recapture. dest-complete can flip `ready` without recapture (§3.1:106). Crash after `ready` before recapture: recover sees `ready` ∧ dest present and does nothing. Original lock-drift CE still holds. | **OPEN** |
| **P2-2** | Bare merge token stays a git ref; family is `--remote` / `local_source_name`; reserved names; refuse name that is already a git remote | §2:36–45, §6:237–239, §7:310–314, §8.2:400–401 (`gwz merge A` is git ref `A`). `--name origin` refuses (§8.1:362–363). | **CLOSED** |
| **P2-3** | Each hub `.git` is `core.bare=true`; hub is a gwz workspace | §4.3:193–201, §8.1:355–356. | **CLOSED** |
| **P2-4** | `dispose root` never deletes the original tree; disband drops index+pointers; path-gone dispose recovers the row; `--force` without a row never deletes by path guess | §3:86–88, §5:215–216, 229–231, §8.4:424–425. Original “dispose last root deletes the project” CE refuses. | **CLOSED** (disband itself has no recover → new **P1-8**) |
| **P2-5** | `family_id` in the pointer file; compare; mismatch → `FamilyRootMismatch`, no write | §3:77–84, §3.1:110. Wire field is optional copy, not authority (§11.6). Replacing root without matching pointer id fail-closes. | **CLOSED** |
| **P2-6** | Pair by dest lock member id / `source_id`; set mismatch refuses; path is not the key | §6:241–244. Divergent path, same id → refuse, no fetch. | **CLOSED** |

---

## Changed-range analysis

Revision 3 is one design patch applying the remplan’s Safety (and the remplan’s merged Consistency) rows: dest `.gwz/` install and omit table; pointer + `family_id`; row state + `recover()` + root-index CAS; dest uniqueness; gitfile refuse; dispose preconditions and `--keep` disarm; operation-time name resolution (no persisted family remotes); `--clean` freeze; reserved names / `--remote` vs `source_ref`; real bare hub; pair-by-id; `dispose root` vs `disband`; new taut `ActionKind`s.

**Outside those dispositions (new surface to attack):**

- dest-complete is a **four-check file predicate** (pointer, no index, `.git` inside dest, `gwz.conf` present). The remplan named “dest-complete” but not these checks. The predicate is weaker than the install contract (no omit-list, no lock recapture, no “all dest-manifest members present”).
- `LocalFamilyRequest.force` is a **BOOL** while prose still says `--force` **names** hazards.
- Index `intent` sketch is `{dest, mode, source, family_id}` — no `keep`, no `branch`, no recapture bit.
- `.gwz/merge/` is **not** in the well-known omit table; “Do not copy `.gwz/merge/`” sits only in the open-merge refuse paragraph.
- Verbatim create holds the **root-index** lock (§4:118–127), not the **source** mutator lock (that lock is only on `--clean` freeze, §4.2:170–172).
- `disband` is a new family-wide mutation with **no** row state in §3.1.

**NEW ARCHITECTURAL root-cause candidates:** none. Incomplete recover coverage (disband, dest-complete, intent fields) is the same intent×observed grammar with missing rows. Incomplete omit + missing source lock is the same dest-install grammar with a hole. Do not trip the two-round cap.

---

## 0. Evidence base

Tuple (start and end, cwd `/Users/owebeeone/limbo/gwz-dev`):

- `git rev-parse HEAD` → `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`
- `shasum -a 256 dev-docs/GwzLocalCloneDesign.md` → `1c543d04afe2850177c0e1098013f507f7412be77dacc8e441bea60b443a8df7`
- `shasum -a 256 dev-docs/GwzLocalClone-RemPlan.md` → `0995242ab8ef41ad881b73845b44f7f346e8b6429ca50c2cadd9538da79672a1`

Read: revision 3 object (full); remplan (full); filed round-1 `GwzLocalClone-ReviewSafety.md`; `transactional-apis.mdc`; I2 record location (`FileMergeStore.discover_open` walks `.gwz/merge/` open files); live push remote resolution (named remotes); `GwzWtDesign.md` status banner only (UNACCEPTED, superseded). Did not read `GwzLocalClone-ReviewConsistency.md`.

---

## 1. Findings

### [P2-1] still open — dest can be `ready` with source lock.yaml vs freeze HEADs

**Root cause:** `--clean` freezes member HEADs once, but dest lock recapture is not part of dest-complete or of recover’s `creating` → `ready` flip. dest-complete is the only authority for `ready`.

**Location:** dest-complete §3.1:106; create step 3 §4:125–127 (copy, install, dest-complete → `ready`); recapture only in §4.2:174–177 and §8.1:346; default freeze = recorded HEAD §11.7.

**Violated invariant:** After a successful `--clean` create (and after `recover()`), dest member HEADs and dest `gwz.lock.yml` agree, or the operation has not reached `ready`.

**Original CE on revision 3:** Source lock pins `gwz-core` at X; live HEAD is Y. `--clean --name C`. Freeze records Y for every member. Dest checkouts are at Y. Dest root still has committed lock X until recapture. dest-complete does not look at lock vs HEAD, so a crash after pointer install (or an implementer who follows §4 step 3 and treats §4.2 recapture as commentary) publishes `ready` lock-drifted. `gwz status` on the new lane is drift — the original impact.

**Impact:** Clean lane is not internally consistent; later merge/push from C composes a lock the freeze never named. The mixed-across-members half of P2-1 is cured; this half is not.

**Required correction:** Recapture dest lock (and dest desired branches) **before** dest-complete may return true. dest-complete includes lock/HEAD agreement at the freeze. `creating` recover does not flip `ready` until that holds; if recapture cannot be done, treat dest as incomplete (delete+drop, same default as other incomplete creates). `-b` must not be the first time recapture runs.

**Closure test:** Source HEAD ≠ lock pin; `--clean` create; inject crash after dest `.gwz/` install and before any recapture; next local verb must not leave a `ready` dest whose lock ≠ HEAD. Success path: dest lock members’ commits equal dest HEADs.

---

### [P1-8] `disband` is a multi-workspace mutation with no `recover()`

**Root cause:** The remplan’s safe terminal for root (“drop index + everyone’s pointers; trees stay”) has no intent row and no recover table. §3.1 only knows `creating` / `ready` / `disposing`.

**Location:** §3:86–88; §5:208, 215–216; §3.1 table (no `disbanding`); taut `LocalFamilyOp` disband=2 with no sequenced fields.

**Violated invariant:** Coordinated identity mutation has one commit point and recover(intent × observed) (`transactional-apis.mdc`). Disband promises trees stay.

**Reproduction:**

1. Family `{root, A, B, C}`. `gwz local disband`.
2. Crash after deleting A’s and B’s `family-root`, index still present, C still has a pointer.
3. Index still lists A,B as `ready` (paths exist). From C, `gwz local list` shows A. `gwz local dispose A` holds A’s mutator lock and **deletes A’s directory** — the tree disband promised to keep.
4. Alternate crash: index file already gone, leftover pointers → `FamilyRootMismatch` (fail-closed, trees stay). That interleaving is safe; (2)–(3) is not.

**Impact:** Partial disband plus ordinary dispose is data loss of workspaces the operator asked only to detach from the family. Stuck split-membership (some clones `NotAFamilyMember`, some still admins) until someone disposes the rest.

**Required correction:** Record `disbanding` on the index (CAS, same lock). recover: for each remaining clone pointer, delete `family-root` only; when no clone pointers remain, drop the index file; **never** delete trees. Do not route leftover `ready` rows through dispose-delete. `dispose root` stays refuse.

**Closure test:** Crash after N of M pointers removed; next local verb finishes pointer removal and drops the index; every dest directory still exists. A subsequent `dispose A` is `UnknownLocal`, not `rm -rf`.

---

### [P1-9] `creating` recover deletes a dest that is already a usable workspace

**Root cause:** dest-complete requires a pointer, so “copy finished, pointer not yet written” is **incomplete**, and incomplete means **delete dest**. dest already has `gwz.conf/gwz.yml` (discoverable) and member `.git`s (committable). recover does not distinguish a half-copy from a complete tree a human has started using. Recorded intent does not include a dest content digest.

**Location:** §3.1:105–106; §4:125–127 “Crash before `ready` → recover drops the row and the partial dest”; dest-complete does not include omit-list absence, all-manifest-members-present, or “dest git matches recorded source/freeze.”

**Violated invariant:** recover of `creating` may drop a failed copy; it must not `rm -rf` unique commits that are not the recorded copy. Path/`exists()` is not authority; neither is “no pointer file.”

**Reproduction (crash between copy and pointer write):**

1. `creating` row committed; tree copy of C completes (all members, `gwz.conf` present); crash before `family-root` is written. Lock released.
2. Dest is a GWZ workspace (`discover_workspace_root` keys on `gwz.conf`). Human `cd`s in, `git commit` unique work on a member (or `gwz status` then edit).
3. Any local verb on root/A runs recover: `creating` ∧ incomplete (no pointer) → **delete dest**, drop row.
4. Unique commits gone. Family name C is free. Same if dest-complete is wrong in the other direction: pointer written, catalog leftover not in the predicate → flip `ready` with source catalog identity (omit-list not checked).

**Impact:** The chosen “do not resume copy” default is silent data loss on the most likely crash window after a large verbatim copy. Worse than status-quo `cp` dying mid-way: the next `gwz local list` actively destroys the leftover.

**Required correction:** dest must not be a discoverable workspace until dest-complete (write `gwz.conf` / drop a `creating` sentinel as part of install; family and ordinary gwz verbs refuse `CreatingDest`). recover **delete** only if dest git state matches recorded source/freeze (no extra commits, no extra members). If dest has drifted, stay `creating`, fail-closed, require `dispose --force` (named) to unlink. dest-complete must also require: omit-list paths absent, **every** dest-manifest member `.git` present **inside** dest, and (for `--clean`) lock recapture (P2-1). Copy exclusions happen **during** copy, not as a later install that races dest-complete.

**Closure test:** Crash after full tree copy before pointer; plant a unique dest commit; next `recover()` must not delete dest. Crash after pointer before omit of `catalog-final`: must not flip `ready` (incomplete). dest without `gwz.conf` may still be deleted as a partial copy.

---

### [P1-10] Verbatim still byte-copies `.gwz/merge/` unless a merge was already open at check time

**Root cause:** The well-known omit table (§4.1:149–158) does not include `.gwz/merge/`. “Do not copy `.gwz/merge/`” is only in the open-merge refuse paragraph (§4.1:160–162). Verbatim does not take the **source** mutator lock (only the root-index lock). Open-merge refuse is check-then-copy.

**Location:** omit table vs §4.1:160–162; locks §4:118–127 vs §4.2:170–172; live `discover_open` reads whatever sits in dest `.gwz/merge/` (`store/mod.rs:82–98`). dest-complete ignores merge records.

**Violated invariant:** Dest `.gwz/` identity stores are **installed**, not copied (framing lines 12–14, §2:46–47). Two workspaces do not share a merge identity (I2: record identity is canonical path under **that** workspace’s `.gwz/merge/`). Check-then-copy is not the contract (`transactional-apis.mdc`).

**Reproduction:**

1. Source has **no** open gwz merge (verbatim allowed). `.gwz/merge/done/` may still exist.
2. Create starts (root-index lock). Copy walks the tree.
3. Concurrently, source `gwz merge feature/x` takes the source mutator lock and writes `.gwz/merge/<id>.yaml` (and git `MERGE_HEAD` on some members). Copy includes those files (`merge/` not in the omit table).
4. Install writes `family-root`, omits catalog. dest-complete passes. `ready`.
5. Dest `discover_open` finds source’s merge record. Dest push/commit **block** (`OpenMergeCommand::Push` is Block). Dest `--continue`/`--abort` run source’s `merge_id` / `before_commit` against dest trees. Family treats dest as a clean clone.

Without the race, **done/** records from prior merges are still copied on the success path if “Do not copy `.gwz/merge/`” is read as applying only when refuse fired (vacuous).

**Impact:** False merge identity on a `ready` clone; dest stuck in someone else’s merge; abort may reset dest members to source `before_commit`. This is the P0-1 merge half returning through a TOCTOU the refuse did not close.

**Required correction:** Put `.gwz/merge/` (open **and** done) on the omit table for all modes — dest merge store starts empty. Verbatim holds the **source** mutator lock from the open-merge/gitfile check through copy+install (same exclusion `--clean` already uses for freeze). dest-complete fails if dest `.gwz/merge/` is non-empty after install.

**Closure test:** Inject an open merge after the verbatim preflight and during copy → dest has no `.gwz/merge/` file and is not `discover_open`. Success verbatim from a source with archived `done/` records → dest `done/` absent.

---

### [P2-7] Absolute git `include.path` / in-member `hooksPath` survive dest install and can republish host paths

**Root cause:** §4.0 refuses gitfile, external `commondir`, `core.worktree` / absolute `hooksPath` **pointing outside that (source) member**. It does not refuse or rewrite `include.path`, `includeIf`, `url.*.insteadOf`, or an absolute `hooksPath` **inside** the source member. §4.1 strip is only filesystem/credential **remote URLs**. After copy, those absolute paths still name **source**.

**Location:** §4.0:131–136; §4.1:158; `repo sync` still copies observed remotes into the manifest (live `sync_member_remotes`) — design forbids family remotes but not remotes revealed via included config.

**Violated invariant:** Dest git identity is dest-local (§10: no shared exclude/hooks/refdb). Host paths must not land in `gwz.yml` (§3:64–65, P1-7’s invariant).

**Reproduction:**

1. Source member `config` has `include.path=/Users/…/gwz-dev/.gitconfig-extra` (or `hooksPath=/Users/…/gwz-dev/gwz-core/tools/hooks`, inside the member so §4.0 does not refuse).
2. Verbatim create A. Dest config still points at source files. Dest git loads source includes (credentials, `file://` remotes, `insteadOf`). Dest commits run source hook scripts.
3. `gwz repo sync` on dest observes remotes from the included config and writes host paths into dest `gwz.yml` — P1-7’s publication path without a persisted family remote.

**Impact:** Shared hooks/config (weaker than shared refdb, not P0-2). Disclosure of laptop paths/credentials on the next dest commit of `gwz.yml`.

**Required correction:** Refuse **or** rewrite/strip any absolute `include.path` / `includeIf` / `hooksPath` / `core.worktree` / `insteadOf` that does not resolve inside **dest** after copy. dest-complete fails if dest config still names a path outside dest. Keep `repo sync` from seeing those remotes (already required: `gwz.yml` remotes origin-only).

**Closure test:** Planted absolute `include.path` on source → create refuses or dest config has no path outside dest. `repo sync` after verbatim does not add remotes. In-member absolute `hooksPath` cannot remain pointing at source.

---

### [P2-8] `--keep`, `--force` hazards, and `-b` are not in recorded intent; `force` is a BOOL

**Root cause:** recover can only apply what the row recorded. Intent sketch is `{dest, mode, source, family_id}` (§3:73–74). `-b` runs **after** `ready` (§4.2:180–184). `keep` is a request BOOL (§7:284). Prose requires `--force` to **name** hazards (§5:218–219); the wire is `force=F(5, BOOL)` (§7:285).

**Location:** §3:73–74; §3.1:107 `unless --keep recorded`; §4.2:180–184; §5:218–219; §7:273, 284–285.

**Violated invariant:** recover(intent × observed). A BOOL cannot name hazards. Post-`ready` steps are not recovered.

**Reproduction:**

1. `--keep`: commit `disposing` without `keep` in the row (intent sketch has no field). Crash after deleting `family-root`, dest still present. recover: `disposing` ∧ dest present ∧ keep **not** recorded → **finish delete**. Operator asked to keep the tree; recover `rm -rf`s it.
2. `--force` as BOOL: `gwz local dispose C --force` deletes unique open-merge work without naming the merge. The round-1 correction required naming so force is not a blanket override.
3. `-b`: dest becomes `ready`, then `-b` creates branches. Crash: dest `ready` without `lane/x`. Retry create → name taken. recover cannot finish `-b` (`branch` not in intent).

**Impact:** `--keep` is not durable across crash (data loss). `--force` is an unrecorded blanket delete. `-b` is best-effort after publication.

**Required correction:** Record `keep`, named `force_hazards`, and `branch` on the row **at the disposing/creating commit point**. `--force` without named hazards refuses. `-b` completes before `ready`, or `ready` is not committed until `-b` + recapture succeed; crash stays `creating` and recover deletes (P1-9’s safe delete) or finishes `-b` from recorded branch. recover honors recorded `keep` (never delete dest).

**Closure test:** `--keep` + crash after pointer removal → dest directory remains, row dropped, `NotAFamilyMember`. `--force` with no hazard names refuses. `--clean -b lane/x` + crash after `ready` before branch create cannot be the specified success path; either dest is still `creating` or `lane/x` exists on every member.

---

## 2. Invariant analysis

| Surface | Revision 3 | Survives? |
|---|---|---|
| Verbatim copies family index / catalog / checked-artifacts | Omit + install + both-files refuse | Yes, if copy exclusions are during copy (P1-9) |
| Gitfile shared refdb | §4.0 refuse | Yes |
| Register-before-copy crash | `creating` recover drops row | Yes for empty dest; **no** if dest is already a workspace — P1-9 |
| Two creates, same name | Index lock + CAS | Yes |
| Dest occupancy / two names one path | Unique dest; nested refuse | Yes |
| Clone-of-clone pointer | Fresh pointer + `family_id` | Yes |
| Open merge copied | Refuse if already open; `.gwz/merge/` not in omit table; no source mutator lock | **No — P1-10** |
| Family remotes in `gwz.yml` | Operation-time resolve | Yes for family names; **include.path — P2-7** |
| `--clean` mixed HEADs | Freeze once | Yes |
| `--clean` dest lock vs HEAD | Recapture not in dest-complete | **No — P2-1** |
| Name vs git ref | `--remote` / reserved names | Yes |
| Bare hub | `core.bare=true` | Yes |
| Dispose unique work | Refuse unless `--force` | Yes if `--force` names hazards; BOOL — **P2-8** |
| `--keep` ghost admin | Delete pointer | Yes; recover may still delete dest — **P2-8** |
| Dispose root deletes original | `disband`; never unlink root tree | Yes; disband recover — **P1-8** |
| Pair by path | Pair by id / `source_id` | Yes |
| `family_id` authenticates pointer | In pointer + compare | Yes |
| Reflink else copy | Unchanged; hardlink not authorized | Yes |
| GwzWt blast radius | Gitfile refuse; no shared catalog on success path | Yes except P1-10 merge identity |

---

## 3. Risks and next action

Revision 3 actually installs dest identity and has a recover table; the round-1 P0s and most P1s do not reproduce. The ready/recover predicates were not carried through the new operations: dest-complete is a short `exists()` list (lock recapture, omit-list, “usable dest” are outside it), `disband` was added beside that table rather than into it, verbatim still copies `.gwz/merge/` under a check the source mutator can lose, and `--keep`/`-b`/`--force` are not recovered intent.

Do not implement from revision 3. Fold P2-1 and P1-8…P2-8 into dest-complete, the recover table (`disbanding`, keep, branch, no auto-delete of a drifted dest), the omit table (always `.gwz/merge/`), and source mutator exclusion for verbatim. Same safety axis re-verdicts. No third architectural root cause; the cap does not fire.
