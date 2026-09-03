# GwzLocalClone — SAFETY-AXIS REVIEW

**Review object:** working-tree file `dev-docs/GwzLocalCloneDesign.md` (UNTRACKED; SHA-256 `579c9fd82e32877aee9b02b59b4693d4bf5b7cca38e04adc7c42d49bb08b52ba`), controlling DRAFT (revision 2, 2026-09-03).
**Baseline:** gwz-dev HEAD `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`. End-of-review re-hash and `git rev-parse HEAD` matched the start tuple; the object did not move.
**Date:** 2026-09-03
**Axis:** SAFETY — degraded and mixed-version paths; irreversible steps and their preconditions; disclosure/privacy scale; stuck states reachable under the text’s own rules; whether “never worse than the status quo” survives concrete interleavings; scope creep that widens blast radius.
**Verdict: NO-GO** — two P0 identity-isolation failures, plus P1 recovery/irreversibility holes and P2 pairing/publication defects. The text authorizes family and git identities that are not actually isolated, and it has no `recover()` for the mutation it introduces.

I pre-commit to GO on a revision that resolves P0-1, P0-2, P1-1, P1-2, P1-3, P1-4, P1-5, P1-6, P1-7, P2-1, P2-2, P2-3, P2-4, P2-5, P2-6 as specified.

---

## 0. Evidence base

Tuple (start and end, cwd `/Users/owebeeone/limbo/gwz-dev`):

- `shasum -a 256 dev-docs/GwzLocalCloneDesign.md` → `579c9fd82e32877aee9b02b59b4693d4bf5b7cca38e04adc7c42d49bb08b52ba`
- `git rev-parse HEAD` → `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`

Read (cited below by line):

- Object: `dev-docs/GwzLocalCloneDesign.md` (full text, 347 lines)
- `.cursor/rules/transactional-apis.mdc` (mutation/recovery/publication/identity)
- `dev-docs/GwzM5-8I2RecordContract.md` §1 (record location, no sidecar journal; path authority under `.gwz/merge/`)
- `gwz-core` (present and readable): `handle_materialize.rs` clone-to-lock; `push_member.rs` remote resolution; `open_merge_gate.rs`; `workspace_mutator_lock.rs`; `handle_create_repo.rs` `sync_member_remotes`; `merge/plan.rs` dirty/source_ref; `merge/start.rs`; `checked.rs` canonical `.gwz/merge` location
- `dev-docs/GwzWtDesign.md` lines 1–7, 21–38 only, to check the object’s “not GwzWt / no shared refdb” blast-radius claim (not reviewed as the object)

Out of scope as instructed: `GwxMergeFindingsDraft.md`, `GwzAiReqDd.md`, `--filesystem-strict`, production code changes, a plan, GwzWt implementation.

---

## 1. Findings

### [P0-1] Verbatim copy of `.gwz/` duplicates family identity and merge identity stores

**Root cause:** Create’s default mode copies the source tree’s `.gwz/` as bytes, then rewrites strings that named the *source path*. It never installs dest as a clone (write `family-root`, drop any copied index) and never strips identity-bearing stores other than `catalog-final`.

**Location:** §4.1 lines 83–98; index layout §3 lines 43–46 and 51–62; primary example §8.1 lines 210–216 clones *from root*; §11.1 default copies the merge record and drops only the catalog.

**Violated invariant:** “The index lives only at `root`. Everyone else stores a pointer to `root`” (§3). Two workspaces must not share one catalog identity (§4.1) — and, by the same isolation claim, must not share a family index or a checked-artifact/merge-store identity. I2: the open record’s identity is its canonical path under that workspace’s `.gwz/merge/`; leftover `.gwz/checked-artifacts/` is a second store for the same `merge_id` + `workspace_id`.

**Reproduction:**

1. In `~/limbo/gwz-dev` (root) start a catalog-backed merge and stall it (§8.1).
2. `gwz clone --local --name A ../gwz-dev-A` (verbatim, from root).
3. Dest receives `root/.gwz/local-family.yml` (the index is untracked under `.gwz/`, so it is in the copy). Root has no `family-root` file to copy. After the source-path rewrite, A either still has an index and no pointer (A *is* a second root) or has both files with unspecified precedence.
4. From A, `gwz clone --local --name B ../gwz-dev-B`. Registration walks “root”. If A is treated as root, B is written into A’s copied index; the real root’s index does not contain B. Split-brain family.
5. Independently: dest keeps `.gwz/merge/<id>.yaml` and `.gwz/checked-artifacts/` and drops `catalog-final`. Dest `--continue` is declared uncatalogued (§4.1, §11.1). Live continue in gwz-core activates the catalog first (`activate_workspace_catalog` / v1 prologue). A new catalog over copied checked-artifacts is foreign-obstruction or silent adoption of source’s merge identity.

**Impact:** False composition of the family namespace (the whole point of the index) and of merge recovery identity. Subsequent create/dispose/merge-from-name operate on the wrong set of paths. This is worse than today’s nameless `cp -a`: the product then *uses* the duplicate identity as authority.

**Required correction:** After register, dest `.gwz/` is *installed*, not copied-and-string-replaced: write `family-root` (absolute or dest-relative path to the *real* root, plus `family_id`); never leave `local-family.yml` on a non-root; copy an open merge record only as an explicit, dest-new identity (new `merge_id` or refuse verbatim while a merge is open) and omit `catalog-final`, `checked-artifacts`, stash bundles, and lock files. State the precedence rule if both index and pointer exist: refuse.

**Closure test:** Design sentence plus a named create test: verbatim from a root that has `local-family.yml` and a catalog-backed open merge → dest has `family-root` and no index file, dest `family-root` resolves to the source root, dest continue cannot observe source’s catalog/checked-artifact identity, and a second create from dest registers only in the real root’s index.

---

### [P0-2] Verbatim byte-copies member `.git`; rewrite does not touch gitfiles, `commondir`, or absolute `config`

**Root cause:** Isolation is claimed at workspace-copy grain, but `.git` is copied as a tree and the only rewrite is “anything in dest `.gwz/` that named the source path” (§4.1). Linked-worktree metadata and absolute git config live outside `.gwz/`.

**Location:** §4.1 lines 85–91 (“each member’s `.git` (index, `MERGE_HEAD`, rebase state)”); rewrite scoped to dest `.gwz/` lines 89–91; §10 lines 329–330 “Not GwzWt. No shared refdb”; §1 lines 17–18 names hand-rolled worktrees as the users being rescued.

**Violated invariant:** Dest is a second *workspace*, not a worktree; no shared refdb (§2, §10). `transactional-apis.mdc`: identity is not a path string. A gitfile’s target path is not dest’s object store.

**Reproduction:**

1. Source is a worktree-shaped lane of the kind §1 and `GwzWtDesign.md` §1 describe: root `.git` is a gitfile; members are `git worktree`s whose `commondir` / gitfile point at the primary’s repos. (This is the migration the document exists to replace.)
2. From that cwd, `gwz clone --local --name A ../gwz-dev-A`.
3. Dest’s gitfiles still name the primary’s git dirs. Dest `git commit` / `gwz commit` mutates the source refdb. Family index treats A as an independent checkout.
4. Same class without worktrees: source `core.hooksPath` or `include.path` is an absolute path under source; dest runs source hooks / includes source config. `core.worktree` if present points at source.

**Impact:** False composition: family verbs (merge A, dispose A, push --remote A) assume an independent object store; git does not. Dest gc/repack/commit can move source refs. This is not “never worse than the status quo”: status-quo worktrees are at least *honest* about sharing a refdb; dest is a third worktree wearing a workspace name.

**Required correction:** Verbatim (and clean) MUST refuse a source member whose `.git` is a file, whose `commondir` is outside dest, or whose config has `core.worktree` / absolute `hooksPath` pointing outside dest — or MUST convert each member to a real clone (new object store) before registration completes. Rewrite of `.git` identity files is not optional and is not covered by `.gwz/` string replace.

**Closure test:** Design sentence plus a test that a gitfile source is refused (or fully duplicated with dest `rev-parse --git-common-dir` inside dest). A second row: absolute `core.hooksPath` in source config cannot remain pointing at source after create returns.

---

### [P1-1] Create and dispose are multi-step identity mutations with no `recover()`

**Root cause:** The index row is `{path, kind}` with no recorded intent (`creating` / `ready` / `disposing`) and no `recover()` of (intent × observed state). Stale detection is `exists(path)` only.

**Location:** Register-before-copy §4 lines 80–81; index sketch §3 lines 51–62; stale rule §3 lines 71–74; dispose sequence §5 lines 137–146. Controlling: `transactional-apis.mdc` lines 10–14 (one commit point; `recover()` is a function of recorded intent plus observed state, not heuristics; path/`exists()` is not authority). Coordinated workspace writes cannot stay non-transactional.

**Violated invariant:** Family name registration is mutation + identity + publication of a workspace. Crash recovery must be determined, not operator folklore.

**Reproduction (create):**

1. Register `C` → `../gwz-dev-C`; crash before copy. Path gone. `list` shows C; other verbs teach dispose (§3). Name is stuck until an operator notices. No automatic recover. No distinction from “operator rm’d dest”.
2. Register `C`; copy dies after some members exist. Path *exists*, so §3 does **not** call it stale. `gwz merge C` / `pull --remote C` treat a partial tree as a live family member. Mixed/missing `.git` is fetched as if it were C.
3. Two creates of `C` (see also P1-2) or dest occupancy (P1-3) produce the same “index says live, tree is not a complete workspace” shape.

**Reproduction (dispose):**

1. Dispose C: drop index row, then crash before `rm`. Orphan tree with `family-root` still set; name free. Re-create `C` at another path. Two trees claim the history of C; sweep of remotes named `C` may already have run or not.
2. Crash during sweep: some members lost remote `C`, some still push to a deleted URL.

**Impact:** Stuck names, live-looking half-clones as merge/pull sources (false composition), and dispose that cannot be restarted without `--force` folklore (and §3 vs §5 disagree — P2-4). This is the exact class GWZ already paid for on merge records.

**Required correction:** Index row gains an explicit state and a single commit point. Sketch: `creating` (name reserved, dest may be absent or partial) → `ready` (dest passed a dest-complete predicate) → `disposing`. `recover()`:

- `creating` ∧ dest missing or incomplete → delete partial dest if any, drop row (or resume copy from recorded source/mode/dest — pick one, not both).
- `creating` ∧ dest complete → flip to `ready`.
- `disposing` ∧ dest still present → finish delete (unless `--keep` recorded) then finish sweep; sweep is idempotent.
- `ready` ∧ dest missing → stay `ready` but fail-closed on use; `dispose` is the only legal consumer (align §3 and §5).

Create/dispose/list/merge-from-name all run `recover()` before using a row. Path existence alone is not `ready`.

**Closure test:** Design names `recover()` and the state table. Fault-injection rows: crash after index write before copy; crash mid-copy; crash after index drop before unlink; crash mid-sweep. In every case a subsequent local verb converges to a legal state without merging from a partial dest.

---

### [P1-2] Create does not lock the root index; two creates can bind one name to two trees

**Root cause:** Dispose step 1 is “Lock the root index” (§5 line 139). Create only says “Registers `<Name>` at root before the copy; if the name is taken, stop” (§4 lines 80–81) — check-then-write, no lock, no CAS.

**Location:** §4 lines 78–81 vs §5 lines 137–139. Workspace mutator lock in gwz-core is *per workspace* (`.gwz/locks/workspace-mutator.lock`). Create from A and create from D take different mutator locks and both write root’s YAML.

**Violated invariant:** Name uniqueness in the family (§2 lines 28–30). `transactional-apis.mdc`: do not leak “check then path-rename” as the contract.

**Reproduction:** From A and D, concurrently `gwz clone --local --name C ../gwz-dev-C-a` and `--name C ../gwz-dev-C-d` (or both default dests). Both see name free; last writer wins the index row; the other dest is an unindexed full copy with `family-root` set (or both copy into one dest and clobber). Uniqueness typed-refusal never fires.

**Impact:** Duplicate or clobbered workspaces; dispose of the winning row deletes at most one tree; the loser is a ghost (P1-5 class) or a corrupted overlay.

**Required correction:** Every index mutation (create register, dispose, repair, recover) takes the same root-index lock *and* is a CAS on the index bytes. Create’s name-taken check is inside that lock and remains held until the row is `creating`. Dest path uniqueness (P1-3) is checked in the same critical section.

**Closure test:** Specified lock/CAS. A concurrency test (or an explicit serialisation contract plus a TOCTOU test that the old check-then-write is illegal) that two racing creates for `C` yield one `creating`/`ready` row and at most one dest.

---

### [P1-3] Dest path is neither required absent nor unique in the index

**Root cause:** Collision is defined only on *name* (§2, §4, §8.1). Dest default is a convention, not an invariant. Occupied dest is unmentioned. Network clone already refuses an existing workspace (`handle_clone_workspace_request`, `WorkspaceAlreadyExists`).

**Location:** §4 lines 78–81; collision examples §8.1 lines 235–240 (name and reserved `root` only).

**Violated invariant:** A family name denotes one workspace directory. Registering a name onto someone else’s tree, or two names onto one tree, is false identity.

**Reproduction:**

1. `gwz clone --local --name A ../same` then `gwz clone --local --name B ../same`. Index has A and B with the same path. `dispose A` deletes B’s tree. B remains `ready` until §3 stale heuristics.
2. Dest already holds another GWZ workspace (or a half-copy). Register-before-copy binds the name; copy overlays source onto that tree.

**Impact:** Dispose of one name deletes another name’s work; copy-into-existing corrupts a workspace the family did not own.

**Required correction:** Inside the index lock: dest must not exist (or exist empty and unregistered); dest canonical path must not equal any other row’s path; dest must not already contain `gwz.conf/gwz.yml`. Same-dest second create refuses, typed, naming the holder.

**Closure test:** Two creates with distinct names and identical dest refuse the second. Create onto an existing workspace refuses without writing the index (or recovers the row per P1-1 if the crash was after register).

---

### [P1-4] Dispose is irreversible and its exclusion domain is only the index file

**Root cause:** Dispose’s preconditions are: not `root` while others remain; not cwd; not missing unless `--force` (§5 lines 140–141). It does not require C clean / no open merge; it does not acquire C’s workspace mutator lock; it does not acquire remaining members’ locks before sweep. Step 4 is `Remove C’s directory`.

**Location:** §5 lines 137–146; §8.3 two live merges; §8.4 dispose C from D. Live merge state is per-cwd `.gwz/` (§2 line 36, §8.3). Mutator lock is per workspace, not per family.

**Violated invariant:** Irreversible delete of unique merge/uncommitted state needs recorded preconditions. Mutation of other workspaces’ git remotes mid-push is a coordinated write.

**Reproduction:**

1. Open merge on C (unique; not on root). From D, `gwz local dispose C`. C’s `.gwz/merge` and dirty trees are unlinked. No `--force` for “has unique work”. Cwd refuse does not fire (cwd is D). C’s mutator lock is not held, so dispose interleaves with `gwz merge --continue` on C.
2. D is `gwz push --remote C` (or pull). Dispose deletes C and/or removes D’s remote `C` under D’s feet. Index lock does not serialize with D’s mutator lock.

**Impact:** Data loss of C’s only copy of conflict resolutions; torn push/fetch; remotes half-swept (P1-1). Worse than status-quo `rm -rf` only in that the product then updates everyone else’s remotes as if C were gone while git operations still hold the old URL.

**Required correction:** Dispose of a checkout MUST (unless an explicit `--force` that names the hazards) refuse if C has an open gwz merge, member `MERGE_*`/`rebase`, or dirty trees. Acquire C’s mutator lock before delete. Acquire each remaining member’s mutator lock before editing that member’s remotes (or perform sweep as a recorded, idempotent `disposing` step that retries). Cwd check is workspace-root-of-cwd, including member subdirs. `dispose root`: see P2-4 (must never delete the original tree).

**Closure test:** Dispose C with an open merge refuses without index change. Dispose C while a second process holds C’s mutator lock refuses. Sweep does not edit D’s `.git/config` without D’s lock (test or specified serialisation).

---

### [P1-5] `--keep` drops the name but leaves a pointer-capable workspace that can still mutate the family

**Root cause:** `--keep` “leaves the tree, still drops the name” (§5 lines 143–144). Local verbs resolve the index via `family-root` (§3, §6), not via “am I in the index?”. Dispose is “from any live family member” without defining “live” as index membership.

**Location:** §5 lines 133–148; pointer §3 lines 44–45, 64–65.

**Violated invariant:** Only indexed names are family members. A removed name must not keep write authority over the index.

**Reproduction:** `gwz local dispose C --keep`. C’s tree remains with `family-root`. From that tree, `gwz local dispose A` still locks the root index and deletes A. Or `gwz clone --local --name C …` reuses the name at a new dest while the kept tree still has remotes and the pointer.

**Impact:** Blast radius of dispose `--keep` is not “detach this name”; it is “unindexed admin replica of the family.” Split-brain mutators.

**Required correction:** `--keep` must rewrite dest to a non-member: delete `family-root`, refuse local family verbs until `repair`/`rejoin`. Sweep remotes *from* the kept tree as well as *to* it. Rejoin is an explicit verb, not create-over-the-same-name.

**Closure test:** After `--keep`, commands from the kept tree that mutate the index refuse `NotAFamilyMember`. A create reusing the name does not share a dest with the kept tree (P1-3).

---

### [P1-6] `family-root` is a relative path that verbatim clone-of-clone does not rewrite

**Root cause:** Pointer is “relative path to root” (§3 line 45). Verbatim rewrite changes dest `.gwz/` strings that named the *source* path (§4.1). The pointer names *root*, so it is left intact. Dest default is a sibling (§4, §11.3); dest is allowed to be any path (`[dest]`).

**Location:** §3 lines 64–65; §4 lines 78–81 `--from`; §8.1 lines 223–229 clone B from A, D from C.

**Violated invariant:** Every clone reads the one index at the real root. A wrong pointer is joining the wrong family (or none).

**Reproduction:** A at `~/limbo/gwz-dev-A` with `family-root` = `../gwz-dev`. From A, `gwz clone --local --name B /tmp/B`. Copied pointer `../gwz-dev` from `/tmp/B` is `/tmp/gwz-dev`. If that path is absent, local verbs break (repair is not v0, §11.4). If a different workspace exists there, B **registers in that workspace’s index** (create writes the pointed-to index before copy). Wrong family, or creates a new split-brain index at `/tmp/gwz-dev` if P0-1 also applies.

**Impact:** Silent join of a stray workspace; names published into the wrong family; dispose in the wrong tree. §11.6’s `family_id` does not save this (P2-5): the pointer is resolved *before* the index is read.

**Required correction:** Persist `family_id` *in the pointer file* and a dest-stable location of root (canonical path, or a hop that is re-resolved and checked). Verbatim/clean create writes a *new* pointer; it does not copy A’s pointer bytes. If root cannot be opened or `family_id` mismatches, refuse (fail-closed), do not register.

**Closure test:** Create from A with dest outside the sibling directory; dest’s pointer opens the original root and dest’s stored `family_id` matches. A planted foreign workspace at the naive relative path is not written.

---

### [P1-7] Installing family names as git remotes publishes host paths into committed `gwz.yml`

**Root cause:** §6 lines 152–156 install/refresh a git remote per family name so `git` can see it. Existing `gwz repo sync` copies *observed* git remotes into the manifest (`sync_member_remotes` / `observed_remotes` in `handle_create_repo.rs` / `handle_repo_lifecycle.rs`), including new names and replacement URLs. The design forbids host paths in `gwz.yml` (§3 lines 48–49, §10 line 335) but does not exclude family remotes from that capture.

**Location:** §6; §3 “Host paths must not be committed”; §8.2 `push --remote hub` / `pull --remote A`. Verbatim also copies existing `.git/config` remotes (§4.1) onto dest, which sync will then ingest.

**Violated invariant:** Host paths must not be committed. Disclosure of laptop layout / sibling clone paths on the next `gwz commit` of dest or of a member that was synced.

**Reproduction:** Create A; use `gwz pull --remote A` once (installs git remotes with filesystem URLs). `gwz repo sync` (or add-existing). `gwz.yml` member remotes gain `A` → `/Users/…/gwz-dev-A/gwz-core`. Commit and push origin. GitHub has the reviewer’s home directory map. Verbatim dest copies those remotes even without a pull.

**Impact:** Unrecoverable disclosure after a normal gwz commit path — not a zip-of-`.git` edge case. File URLs can also carry `https://user:token@…` already in config; sync would persist those too if rewritten. Dispose sweep deleting git remotes does not rewrite history that already committed them.

**Required correction:** Family bindings MUST NOT be git remotes that `repo sync` / capture can see, *or* sync/capture MUST ignore a reserved remote-name prefix / `insteadOf` namespace, *or* remotes are ephemeral per operation and deleted before return. State that `gwz.yml` / lock remotes remain origin-only. Verbatim must strip filesystem remotes and credential-bearing URLs from copied `config` before create returns.

**Closure test:** After pull/merge/push using a family name, `gwz.yml` bytes contain no dest-host path and no extra remote names. `repo sync` is a required row, not only `git remote -v`.

---

### [P2-1] `--clean` is not the lock-pinned clone gwz already has; dest can be mixed vs its own lock

**Root cause:** `--clean` is “as if `git clone <remote>` per member, then a new workspace” at “the source’s current branch” / `-b` at source `HEAD` (§4.2 lines 102–111). Network `gwz clone` materializes members to the committed lock (`handle_clone_workspace` → `MaterializeTargetKind::Lock`). Clean does not freeze lock.yaml first, does not copy members at lock pins, and does not stop if source HEAD moves during a sequential member copy. `-b` collision is checked aggregating *before* copy, then branches are created member-by-member (crash → P1-1).

**Location:** §4.2; contrast clone implementation cited above. Example §8.1 lines 218–221.

**Violated invariant:** A workspace’s member HEADs and `gwz.lock.yml` agree after a clean create, or the operation refuses. Mixed member commits are not silently composed (the same doctrine GwzWt used for `--branch` exists-in-SOME → refuse).

**Reproduction:** Source lock pins `gwz-core` at commit X; live HEAD is Y (or moves to Z after dest’s root clone and before dest’s core clone). Dest root contains lock-at-X; dest `gwz-core` is at Y/Z. `gwz status` is drift on a brand-new “clean” lane. Two dest members taken at different instants are mixed with each other.

**Impact:** False composition of a “clean” workspace. Subsequent merge/push from that dest publishes a tree no source lock ever named. Worse than `gwz clone <url>`.

**Required correction:** `--clean`/`--bare` freeze source lock+HEAD once under source’s mutator lock, copy/clone each member at that freeze (lock pin or recorded HEAD — pick one and use it for *all* members including dest root), refuse if freeze cannot be held. `-b` creates dest branches from the frozen commit after dest is complete, or abort via P1-1 recover.

**Closure test:** Source member HEAD ≠ lock pin → dest either matches the chosen freeze on every member or refuses. Injected HEAD movement mid-copy cannot produce dest members at two commits.

---

### [P2-2] Family names occupy the same token as git refs and remotes

**Root cause:** `MergeRequest.source` and `--remote` become family names (§6, §7, §8.2, §11.2 default `gwz merge A` / `A:branch`). Reserved set is only `root` (§3 line 67). Create will accept `origin`, `main`, `HEAD`. Live merge start requires `source_ref` and resolves it as a git ref on each dest member (`plan.rs`).

**Location:** §6 lines 152–159; §8.2 lines 265–272; §11.2.

**Violated invariant:** A merge/push source is unambiguous. Names that shadow `origin` or a branch must not silently change `gwz merge main` from “git ref main” to “family member main’s HEADs”.

**Reproduction:** `gwz clone --local --name origin …` then `gwz merge origin` or `gwz push --remote origin`. If family-name wins, GitHub origin is never used; if git-ref wins, the clone named origin is unaddressable. `gwz merge main` after naming a clone `main` merges the wrong object.

**Impact:** Wrong-source merge/push — false composition with ordinary git usage. Not hypothetical: `origin` is the first name an agent will try for “the other lane”.

**Required correction:** Create refuses names in a closed reserved set (`root`, `origin`, `HEAD`, `FETCH_HEAD`, empty, names with `/` or `:`, existing dest branches if merge `A` would be ambiguous). Merge/push either requires `local:Name` / `Name:` form, or resolves family names only when they are in the index *and* are not also a dest ref — collision refuses.

**Closure test:** Create `--name origin` refuses. `gwz merge main` with no clone `main` remains a git-ref merge. With a clone `main`, the command refuses or uses the documented disambiguator — never a silent bind.

---

### [P2-3] `--bare` claims `denyCurrentBranch` does not apply without making the git repos bare

**Root cause:** Bare is “same directory layout … only `.git` folders — no checkouts” (§4.3 lines 122–124) and “`receive.denyCurrentBranch` does not apply”. That git knob is off only for `core.bare=true` (or equivalent). A non-bare `member/.git` with no worktree still has HEAD; receive still refuses the checked-out branch. `commit` is refused by gwz verbs, not by git.

**Location:** §4.3; §8.2 lines 274–275; §8.5.

**Violated invariant:** Hub push of the current branch succeeds. A layout-only `.git` is not a push hub.

**Reproduction:** Create hub as specified. `gwz push --remote hub` of the branch HEAD names. Git receive on a non-bare dest refuses. Operator “runs commit anyway” via `git -C hub/gwz-core commit`: worktree is the member directory (exists, almost empty); git can create a commit or fail opaquely. Hub is then a weird checkout others fetch.

**Impact:** Stuck hub (the share-point in §8.5) or a non-bare hub that accepts commits into an empty worktree and serves them.

**Required correction:** Each hub git directory is a real bare repo (`core.bare=true`, no worktree). Gwz commit/add/merge-continue refuse as already stated. Push path is filesystem URL to that bare repo. State that `updateInstead` is not used (already in §10).

**Closure test:** Design requires `core.bare=true`. A test (when implemented) that push of dest’s current branch into hub succeeds and `git rev-parse --is-bare-repository` is true on each hub member.

---

### [P2-4] Stale dispose, `--force`, and `dispose root` do not define a safe terminal

**Root cause:** §3 lines 71–74: missing path → `dispose C` completes cleanup. §5 lines 140–141: refuse if C is missing unless `--force` (index-only sweep). “Missing” is ambiguous (not in index vs path gone). `dispose root` refuses only “while any other name remains” (§3 line 67–68, §8.4). When others are gone, step 4 would `rm` the original workspace — unless cwd refuse always fires because the only remaining member *is* cwd. Neither end state is written: impossible teardown vs delete-the-project.

**Location:** §3 lines 67–74; §5 lines 139–146; §8.4 lines 307–312.

**Violated invariant:** Fail-closed, one meaning of dispose. Root’s tree is not a clone to delete.

**Reproduction:** Path-gone C: operator follows §3, hits §5 refuse, name stuck (or uses `--force` without understanding). Empty family: `gwz local dispose root` from `/tmp` with `--workspace` pointing at root (cwd refuse may not apply) deletes the original project. From inside root, dispose root may be impossible forever; `local-family.yml` cannot be retired.

**Impact:** Stuck names, or data loss of the original workspace. Irreversibility without a stated precondition.

**Required correction:** Define terms. Path-gone + row present: `dispose` (no `--force`) is the recover of that row (P1-1). Not in index: `--force` only sweeps leftover remotes, never deletes a tree by path guess. `dispose root` always refuses to delete the original tree; the only root operation is “disband family” = drop the index file and everyone’s pointers, trees stay.

**Closure test:** Documented procedure for path-gone C and for disbanding the family. A row that `dispose root` cannot unlink `gwz.conf/` or the root worktree.

---

### [P2-5] `family_id` cannot authenticate the pointer

**Root cause:** `family_id` is minted in the index at root (§3 line 54). The pointer file is only a relative path (§3 line 45). §11.6 puts `family_id` “on the wire” so “a stray path cannot join the wrong family” — but join is opening a path, then believing that path’s index, which can mint or echo any id.

**Location:** §3 lines 44–45, 54, 64–65; §11.6 lines 345–347.

**Violated invariant:** A stray or replaced root directory must not silently rebind clones. Path is not authority (`transactional-apis.mdc`).

**Reproduction:** Clones A,B point at root. Operator replaces `~/limbo/gwz-dev` with a different workspace (or a copied index from another machine). Next `gwz local list` in A reads the new index. A’s `family_id` expectation was never stored on A. Split-brain or wrong-family join. Copying `local-family.yml` to another tree (P0-1) is the same class.

**Impact:** Namespace rebind without a typed failure. Offline/unreadable root is then indistinguishable from “root was replaced”.

**Required correction:** Store `family_id` in `family-root` at create. Every local verb: open pointer, read index, compare ids, refuse `FamilyRootMismatch` on disagreement. Replacing root without `repair --root` cannot succeed quietly. (Repair remains named; v0 must still fail-closed without it.)

**Closure test:** Pointer with id X, index with id Y → refuse, no index write. Wire field without on-disk pointer field is specified as insufficient.

---

### [P2-6] Merge/pull from a name pairs dest members by dest `member.path` under A’s root

**Root cause:** §6: for each selected member, URL = `{A’s path}/{member.path}`. It does not say whose `member.path`, and does not check A’s lock/manifest `source_id` / member id. Family clones may later `gwz repo add/detach` independently (§10 does not freeze member sets).

**Location:** §6 lines 152–156; §8.2 `gwz merge A` “A’s HEAD in each member”.

**Violated invariant:** Workspace merge pairs the same logical member, not “whatever directory of that relative path exists under A”.

**Reproduction:** A moves `core` from `gwz-core` to `core`. D still has `gwz-core`. `gwz merge A` fetches `A/gwz-core` (missing or a different repo) into D’s `gwz-core`, or fetches `A/core` into the wrong dest member if selection is by path. Alternatively A added a member D does not have: silently skipped vs composing a partial merge as success.

**Impact:** Wrong-repo merge; integration commits that mix unrelated histories. Ordinary merge dirty/MERGE_HEAD preflights (`plan.rs`) do not catch pairing errors.

**Required correction:** Resolve by member id / `source_id` as in dest lock, require A’s corresponding repo to exist and match identity, refuse on set mismatch (aggregating). Path is not the key.

**Closure test:** Divergent path for the same member id → refuse, no git fetch. Extra/missing member on A → refuse, not a successful N−1 merge.

---

## 2. Invariant analysis

| Surface | What the text permits | Survives? |
|---|---|---|
| Register-before-copy | Name reserved with dest absent or partial; `exists(path)` is liveness | No — P1-1 |
| Two creates, same name | Check-then-write, no create lock | No — P1-2 |
| Dest occupancy / two names one path | Unspecified; name uniqueness only | No — P1-3 |
| Root moved/deleted | Pointers break; repair not v0; no pointer `family_id` | Fail-open rebind — P1-6, P2-5 |
| Clone of clone | Copy A’s relative pointer; rewrite is source-path only | Wrong root — P1-6 |
| Split-brain index | Verbatim from root copies `local-family.yml` | No — P0-1 |
| Copied open merge | Same `merge_id`, drop catalog only; checked-artifacts remain | Stuck/false recovery — P0-1 |
| Path rewrite | `.gwz/` source-path strings; not gitfiles/config | Shared refdb — P0-2 |
| Two live merges | Intended; OK *if* stores are dest-local | OK only after P0-1 |
| `MERGE_HEAD` / rebase copy | Intended for verbatim | OK if `.git` is not a gitfile |
| Secrets / `.git/config` | Verbatim copies; remotes then enter `gwz.yml` via sync | Disclosure — P1-7 |
| `--clean -b` | Sequential clone at live HEAD; collision check is TOCTOU vs copy | Mixed dest — P2-1, P1-1 |
| Bare hub | Layout-only `.git`; denyCurrentBranch “n/a” | Push stuck / fake bare — P2-3 |
| Dispose C | Deletes unique merge; sweep without other locks | Loss / torn remotes — P1-4 |
| `--keep` | Unindexed pointer replica | Ghost admin — P1-5 |
| Dispose root | Refuse only if others remain; cwd rule unspecified | Delete project or stuck family — P2-4 |
| Name as merge source | Token overload; path pairing | Wrong merge — P2-2, P2-6 |
| Reflink/COW | “Reflink else copy”; not hardlink | Authorized fallback is copy — no defect. Hardlink not permitted. `CARGO_TARGET_DIR` not set shared — OK. Cross-device → copy — OK. |
| `recover()` half-registered | Operator dispose / “teach missing” | Not a function of intent — P1-1 |
| “Never worse than status quo” | Network clone pins lock; worktrees honestly share refdb; `cp -a` does not then *drive* a family index | Fails on P0-1, P0-2, P2-1, P1-7 |
| GwzWt blast radius | Claims no shared refdb/catalog | Claim is the product goal; verbatim of the motivating worktree source reintroduces shared refdb (P0-2). Catalog drop is incomplete (P0-1). Not a GwzWt implementation sneak. |

Mixed-version binaries: old CLI does not send `--local` (§7) — OK. Old binary on dest after verbatim still runs dest’s copied merge against dest’s `.gwz/` / `.git`; corruption there is P0-1/P0-2, not an additional protocol-version hole. New CLI / old core typed refuse is stated.

---

## 3. Risks and next action

The design’s safety story is “second workspace, index at root, copy the disk.” That is not enough for a named family that other clones will merge, pull, push, and dispose by. The index is new identity; create/dispose are coordinated mutations; verbatim copy is not an identity install.

Do not implement from revision 2. Revise the DRAFT to close P0–P2 (install dest identity; refuse or convert gitfile sources; state machine + `recover()` + one index lock; dest uniqueness; dispose preconditions and `--keep` disarm; pointer `family_id`; no host-path remotes in `gwz.yml`; clean freeze; reserved names; real bare repos; pairing by member id; safe root disband). Then the same safety axis re-verdicts this object.

P3 not blocking: fail-closed messages for unreadable root while repair is out of v0 (once P2-5 exists, the message is determined). `family list` printing absolute paths is appropriate on the owning machine and is not a defect by itself.
