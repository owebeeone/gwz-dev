# Gwz local clone — Requirements & Design

Status: **DRAFT 2026-09-03 revision 5** — folds
`GwzLocalClone-RemPlan-3.md` (Consistency NO-GO P2-10 / Safety GO on
revision 4: `GwzLocalClone-ReviewConsistency-3.md`,
`GwzLocalClone-ReviewSafety-3.md`, object SHA-256 `8ce70d8a…` at
gwz-dev `68b6ba4`). Non-architectural example reorder plus two P3s.
GwzWt remains unaccepted (`GwzWtDesign.md` / `GwzWtPlan.md`); this
document **supersedes GwzWt for agent-lane isolation** (the W-* rows
are not implementation authority). Design only; no implementation
until a later plan. Consistency re-verdicts P2-10.

Framing: a second **workspace**, not a worktree. Create is a tree copy
(reflink when the FS answers) with a **named dest `.gwz/` install**, not
a byte copy of identity files. The taut IR adds **new request kinds**
(old `CloneWorkspaceRequest.url` stays required). Not a merge-record
version. Not `--filesystem-strict`.

## 1. Problem

Agents want isolated lanes. Hand-rolled worktrees share a refdb and
break `gwz`. Network `gwz clone <url>` isolates but is slow and
nameless. A one-hop `primary` pointer is not enough: `root→A→B` and
`root→C→D` must still be one namespace, so D sees A, B, and root, not
only C, and disposing C cleans C out of everyone.

## 2. Product

A **local family** is one original workspace, reserved name `root`,
plus zero or more named local clones (including optional bare clones).

- Every clone has a **name**. Create without a name refuses.
- Names are unique in the family. Collision refuses, typed, names the
  holder path. Dest paths are unique. Nested dest (inside any family
  member prefix, or a directory that already contains `gwz.conf/gwz.yml`)
  refuses.
- Reserved names: `root`, `origin`, `HEAD`, `FETCH_HEAD`, empty, any
  name containing `/` or `:`. Create also refuses a name that is already
  a git remote in any source member.
- The **index** lives only at `root`. Everyone else stores a pointer
  to `root` (`family_id` + path) and reads the index from there. The
  graph of who cloned from whom is not the namespace.
- `gwz push` / `pull --head` `--remote` is **family name if that name
  is a `ready` member, else today’s git remote** (`origin`, `upstream`,
  …). `gwz merge` keeps `source_ref` as a **git ref**. Family merge is
  `gwz merge --remote <name> [<ref>]` (optional
  `MergeRequest.local_source_name`) and is **family-only** (no git-remote
  fallback; `--remote origin` is a family miss).
- Two clones may each have an open merge. They do not share `.gwz/`
  identity stores.

Disk create is a filesystem copy (reflink when the FS answers, else
copy) plus the dest `.gwz/` install (§4.1). Source is not modified.

## 3. The family index — `root` holds it

```text
root/.gwz/local-family.yml     # the index (untracked)
clone/.gwz/family-root         # pointer: family_id + dest-relative
                               # or canonical path to root (never copied)
```

A dest that contains **both** `local-family.yml` and `family-root`
**refuses** (false root). Only `root` has the index; only clones have
the pointer.

The index is **not** in `gwz.conf/gwz.yml`. Host paths must not be
committed. `gwz.yml` remotes stay origin-only.

Index contents (conceptual):

```text
family_id: <nonce minted when root first clones>
family_state: ready | disbanding     # index-level; not a member row
root: { path: ".", kind: checkout }
members:
  A: { path: ../gwz-dev-A, kind: checkout, state: ready,
       intent: { dest, mode, source, family_id, branch, keep,
                 force_hazards, freeze } }
       # freeze = recorded source lock + member HEADs at creating commit
```

Paths in the index are **root-relative**. A clone resolves names by
opening `family-root`, reading `family_id`, opening the index, comparing
ids. Mismatch → `FamilyRootMismatch`, no write. Never ask the creator.

`family_id` lives in the **index and the pointer file**. It is not a
taut field unless the local-clone request copies it for the dest bind
(optional on that new request). A stray path cannot join by echoing an
id the pointer does not already hold.

`root` cannot be taken as a clone name. `dispose root` **never** deletes
the original tree. Disband = `family_state: disbanding`, then drop
everyone’s pointers and the index; trees stay. `gwz local repair --root
<path>` is named, not v0.

### 3.1 Row state and `recover()`

Every index mutation takes the **root-index lock** and is a **CAS** on
the index bytes. Name-taken, dest-unique, and dest-empty checks run
inside that lock.

Member row state: `creating` → `ready` → `disposing`. Index-level
`family_state`: `ready` | `disbanding`. Path existence is not `ready`.

`recover()` is a function of recorded intent plus observed dest and
pointer (not `exists()` folklore). Every local verb runs it before
using a row. Create crash handling is **this table only** — §4 does
not override it.

**dest-complete** (all must hold):

- dest `family-root` present, target is the registering root, `family_id` matches;
- no dest `local-family.yml`;
- omit-list paths (§4.1) **absent** on dest (including empty `.gwz/merge/`);
- every dest-manifest member `.git` is **inside** dest;
- dest `gwz.conf/gwz.yml` present;
- dest git config names no path outside dest (`include.path`,
  `includeIf`, `hooksPath`, `core.worktree`, `insteadOf`);
- `--clean` / `--bare`: dest lock members’ commits equal dest HEADs
  (recapture already done);
- recorded `branch` (if any) exists on every dest git member.

| Intent × observed | recover() |
|---|---|
| `family_state: disbanding` | delete remaining clone `family-root` files only; when none remain, drop the index; **never** delete trees; do **not** route leftover `ready` rows through dispose-delete |
| `creating` ∧ dest-complete | CAS to `ready`; dest retained |
| `creating` ∧ dest missing | drop row |
| `creating` ∧ incomplete ∧ dest not discoverable (no `gwz.conf`) ∧ dest git matches recorded freeze/source (no extra commits, no extra members) | delete dest; drop row (do **not** resume copy) |
| `creating` ∧ incomplete ∧ (`gwz.conf` present **or** dest git drifted from freeze/source) | stay `creating`; family and ordinary gwz verbs **fail-closed** (`CreatingDest`); only `dispose --force` with named hazards unlinks |
| `creating` ∧ dest-complete except recapture/`-b` cannot finish | incomplete; do **not** flip `ready`; then the two incomplete rows above |
| `disposing` ∧ recorded `keep` | finish sweep; drop row; dest remains; pointer absent. **Never** finish-delete a `--keep` dest |
| `disposing` ∧ not `keep` ∧ dest still present | finish delete, then finish sweep; sweep is idempotent |
| `disposing` ∧ dest gone | finish sweep; drop row |
| `ready` ∧ dest missing | stay `ready`; use (pull/merge/--remote) **fail-closed**; only `dispose` consumes |
| pointer `family_id` ≠ index `family_id` | `FamilyRootMismatch`; no index write |

While `disbanding`, `dispose <name>` is `UnknownLocal` (or finishes
pointer removal only) — it does not `rm -rf` a tree the operator asked
only to detach.

Dest is not a discoverable workspace until install writes `gwz.conf`
**last** (ordinary gwz verbs seeing a `creating` sentinel or missing
manifest refuse `CreatingDest`). Omit-list exclusions run **during**
copy, not as a later pass that races dest-complete.

## 4. Create

`gwz clone --local --name <Name> [dest]` from the cwd workspace (any
`ready` family member, or `root`). Dest defaults to
`../<root-dirname>-<Name>`. Copy source is cwd (or `--from <name|path>`).

Under the **root-index lock** and the **source mutator lock** (verbatim
and `--clean`; held from the §4.0 / open-merge check through
copy+install):

1. Refuse reserved / taken name / dest that exists (unless empty and
   unregistered) / dest equal to another row / dest nested in a family
   member / dest already a workspace / dest inside cwd’s member prefix.
2. Commit a `creating` row (name, dest, mode, source, `family_id`,
   recorded `branch` if `-b`, freeze = source lock + member HEADs).
   That is the create commit point.
3. Copy the tree (§4.1–4.3) with omit-list applied **during** copy,
   **install dest `.gwz/`** (pointer, strip/rewrite config, `--clean`
   recapture dest lock and create recorded `branch` **before**
   dest-complete). Write dest `gwz.conf` **last**. dest-complete →
   `ready`. Crash before `ready` → **§3.1 only**.

### 4.0 Source git identity (all modes)

Refuse (aggregating) if any source git member has:

- `.git` as a **file** (gitfile / linked worktree),
- `commondir` outside that member,
- `core.worktree`, absolute `hooksPath`, `include.path`, `includeIf`,
  or `url.*.insteadOf` that would still name a path **outside dest**
  after copy.

No convert-in-v0. Default is **refuse**, not rewrite. Dest
`git rev-parse --git-common-dir` must be inside dest after create.
dest-complete fails if dest config still names a path outside dest.

### 4.1 Verbatim (default) — tree copy + dest `.gwz/` **install**

Copy the source tree as it sits: committed files, unstaged edits,
untracked files, `target/`, each member’s `.git` (index, `MERGE_HEAD`,
rebase state). Source is not modified. Holds the **source mutator
lock** for the whole copy+install (closes open-merge TOCTOU).

**Do not copy** (well-known identity; dest `.gwz/` is installed):

| Path | Dest instead |
|---|---|
| `.gwz/local-family.yml` | absent (dest is not root) |
| `.gwz/family-root` | **new** file: dest-relative or canonical path to the registering root + `family_id` |
| `.gwz/catalog-final` | omit |
| `.gwz/checked-artifacts` | omit |
| `.gwz/locks/` | omit (fresh lock on first use) |
| `.gwz/merge/` (open **and** `done/`) | omit; dest merge store starts **empty** |
| `gwz.conf/gwz.yml` | omit during copy; **write last** (dest not discoverable until then). Recapture may create `gwz.conf/gwz.lock.yml` earlier |
| stash bundles under `.gwz/` | omit |
| `.git/worktrees/` | omit |
| filesystem / credential URLs in copied `.git/config` remotes | strip; `origin` https/ssh URLs that are not `file:` stay |

**Open gwz merge:** **refuse verbatim** while source has an open gwz
merge (checked under the source mutator lock, before copy). Dirt and
per-repo `MERGE_HEAD` are still copied when no gwz merge is open.

Post-copy check: dest must not contain `local-family.yml`; dest must
contain `family-root` whose target is the registering root; both files
together → refuse; dest `.gwz/merge/` must be empty.

### 4.2 Clean (`--clean`, optional `-b <branch>`)

Not a live-HEAD race. Under the **source** mutator lock, freeze source
lock + every member HEAD **once** (stored on the `creating` row).
Every dest member (including dest root) is taken at that freeze. If
freeze cannot be held, refuse.

- Dest trees are a clean checkout at the freeze (lock pin **or**
  recorded HEAD — one choice for all members; default: **recorded HEAD**
  so a dirty-uncommitted source still has a defined commit).
- Recapture dest lock to dest HEAD **before** dest-complete may be
  true. dest-complete includes dest lock/HEAD agreement. If recapture
  cannot be done, dest is incomplete (§3.1) — do not flip `ready`.
- `-b <branch>` runs **before** dest-complete / `ready`, creating that
  branch in every git member at the frozen commit, then recaptures dest
  desired branches. Recorded on the `creating` row at the commit point.
  If `<branch>` already exists in any member at freeze time, refuse the
  whole create (aggregating) **before** the `creating` row.
- No source dirt, no member `MERGE_*`, no source `.gwz/` merge. Dest
  `.gwz/` is the same **install** as §4.1 (pointer, no index, empty
  merge store).
- `origin` remotes stay the source’s `origin` URLs (non-`file:`).
- No `target/` on `--clean`. `--clean --keep-build-cache` is not v0.

`--verbatim` and `--clean` are mutually exclusive. `--bare` implies
`--clean` (no worktree dirt).

### 4.3 Bare (`--bare --name hub`)

Same workspace **directory layout** (root + each `member.path`). Each
member `.git` is a **real bare repo** (`core.bare=true`, no worktree).
Hub **is** a gwz workspace: `gwz.conf/gwz.yml` (copy of the manifest,
written last) and `.gwz/family-root` so `discover_workspace_root`
works. `local list` and `dispose` run here.

Verbs that need a worktree (`commit`, `add`, merge that writes trees,
`status --files`) refuse, typed. `push` / fetch into hub, `log`,
`local list`, `dispose` work. `updateInstead` is not used.

## 5. Dispose

```sh
gwz local dispose C
gwz local dispose C --keep
gwz local dispose C --force open-merge,dirty
gwz local disband          # family_state disbanding; trees stay
```

From a **family member** (index membership; `--keep` trees are not
members). While `disbanding`, do not dispose-delete.

1. `recover()`, then root-index lock + CAS.
2. `C` is `root` → refuse to delete the original tree. Use `disband`.
3. Cwd (workspace root of cwd, including member subdirs) is C → refuse
   (leave first).
4. Checkout C has an open gwz merge, member `MERGE_*`/rebase, or dirty
   trees → refuse unless `--force` **names** those hazards.
   `--force` with no hazard names **refuses**.
5. Hold C’s mutator lock. Commit `disposing` on the row **with
   recorded `keep` and `force_hazards`** (commit point).
6. Unless recorded `keep`: delete C’s directory. With `keep`: delete
   dest `family-root` only; family verbs from that tree refuse
   `NotAFamilyMember`. Rejoin is an explicit verb, not create-over-the-
   same-name.
7. Sweep is part of `disposing` recover: drop **operation-time** family
   bindings for `C` (there are no persisted git remotes to delete — §6).
   Idempotent. `--keep` also strips bindings *from* the kept tree.

Path-gone + row present: `dispose C` (no `--force`) is recover of that
row. Not in index: `--force` only sweeps leftover bindings, **never**
deletes a tree by path guess.

**Disband:** CAS `family_state: disbanding`. recover (§3.1) removes
clone pointers then the index. Trees stay. Crash after N of M pointers
removed: next local verb finishes the rest. Subsequent `dispose A` is
`UnknownLocal`, not `rm -rf`.

After dispose C, D does not know `C`. D still knows `root`, `A`, `B`, `D`.

## 6. Resolving a name

Load pointer → index (`recover()` first).

**Pull / push `--remote` (`OperationPolicy.remote` / `PushRequest.remote`):**

1. Token is a `ready` family name → bind from the index: take that
   member’s path; for each **selected dest member**, pair by dest lock
   **member id / `source_id`**. Path is not the key. Set mismatch
   (missing / extra / path moved for the same id) refuses aggregating;
   no fetch.
2. Else resolve as today’s git remote (`origin`, `upstream`, …).
3. Neither → existing `missing_remote`.

After any local clone exists, `gwz pull --head --remote origin` and
`gwz push --remote origin` remain git-`origin`. Omitted `--remote` on
pull still defaults to `origin` as today.

**Merge `--remote` / `local_source_name`:** family-only. Name not in
index or not `ready` → `UnknownLocal`. `--remote origin` is a family
miss (reserved / not a member). A bare `gwz merge feature/x` is a git
ref and is **not** `UnknownLocal`.

Family bindings are **not** persisted as git remotes. Resolve at
operation time from the index. `repo sync` / capture must not write
family URLs into `gwz.yml`.

## 7. Protocol

Three layers.

| Layer | What | New? |
|---|---|---|
| **Taut IR** | **New** `ActionKind` values and request messages. `CloneWorkspaceRequest.url` stays required. Optional `MergeRequest.local_source_name`. Drift fingerprint moves. | **Yes** |
| **Family index** | `.gwz/local-family.yml` + `.gwz/family-root` (`family_id` in both). | **Yes** |
| **Merge record / catalog** | v1 body, catalog tuple | **No** |

Live names (do not invent `CloneRequest` / `PullRequest` /
`MergeRequest.source`):

```text
ActionKind.clone_workspace = 19          # unchanged; url required
ActionKind.clone_local_workspace = 27    # NEW
ActionKind.local_family = 28             # NEW  list | dispose | disband

CloneLocalWorkspaceRequest = Msg(
  meta=F(1, RequestMeta),
  name=F(2, STR),                         # required
  dest=F(3, STR, optional=True),
  mode=F(4, LocalCloneMode),              # verbatim=0, clean=1, bare=2
  branch=F(5, STR, optional=True),        # -b; clean/bare only; before ready
  from=F(6, STR, optional=True),          # family name or path
  family_id=F(7, STR, optional=True),     # dest bind copy of pointer id
)

LocalCloneMode = Enum(verbatim=0, clean=1, bare=2)

LocalFamilyRequest = Msg(
  meta=F(1, RequestMeta),
  op=F(2, LocalFamilyOp),                 # list=0, dispose=1, disband=2
  name=F(3, STR, optional=True),          # dispose
  keep=F(4, BOOL, optional=True),
  force_hazards=F(5, List(STR)),          # named hazards; absent/empty = no --force
)
# BOOL-only force is withdrawn.

MergeRequest.source_ref = F(3, STR, optional=True)   # git ref; UNCHANGED
MergeRequest.local_source_name = F(9, STR, optional=True)  # NEW; start only
# F(8) is filesystem_strict (ship 1). Do not reuse.

PushRequest.remote = F(2, STR, optional=True)        # ready family name, else git remote
PullHeadRequest.meta.policy.remote                   # OperationPolicy.remote=F(5); same dispatch
```

Old binary does not send 27/28. New CLI / old core refuses typed.

§8 CLI → live message (after this allocation):

| CLI | Message |
|---|---|
| `gwz clone --local --name A ../gwz-dev-A` | `CloneLocalWorkspaceRequest` mode=verbatim name=A dest=… |
| `gwz clone --local --clean -b lane/x --name C dest` | same, mode=clean, branch=lane/x |
| `gwz clone --local --bare --name hub dest` | same, mode=bare |
| `gwz local list` | `LocalFamilyRequest` op=list |
| `gwz local dispose C` | `LocalFamilyRequest` op=dispose name=C |
| `gwz local dispose C --keep` | same, keep=true |
| `gwz local dispose C --force open-merge` | same, force_hazards=["open-merge"] |
| `gwz local disband` | `LocalFamilyRequest` op=disband |
| `gwz pull --head --remote A` | `PullHeadRequest`; `OperationPolicy.remote=A` (family bind) |
| `gwz pull --head --remote origin` | same; git remote `origin` (not UnknownLocal) |
| `gwz --sync ff-only pull --head --remote root` | family bind root |
| `gwz merge feature/x` | `MergeRequest` op=start `source_ref=feature/x` |
| `gwz merge --remote A` | `MergeRequest` op=start `local_source_name=A` (HEAD per paired member) |
| `gwz merge --remote C lane/agent-17` | `local_source_name=C` `source_ref=lane/agent-17` |
| `gwz merge --remote origin` | `UnknownLocal` (family-only; reserved / not a member) |
| `gwz push --remote hub` | `PushRequest` remote=hub (family) |
| `gwz push --remote origin` | `PushRequest` remote=origin (git) |
| `gwz merge A` | **git ref `A`**, not a family name |

## 8. Examples

Family after creates (`~/limbo/gwz-dev` is `root`):

```text
~/limbo/gwz-dev          root     (checkout)
~/limbo/gwz-dev-A        A        (verbatim; no open gwz merge)
~/limbo/gwz-dev-B        B        (clean, from A; index still root)
~/limbo/gwz-dev-C        C        (clean -b)
~/limbo/gwz-dev-D        D        (clean, from C)
~/limbo/gwz-dev-hub      hub      (bare)
```

### 8.1 Create

```sh
cd ~/limbo/gwz-dev
echo dirty >> gwz-core/src/lib.rs
gwz clone --local --name A ../gwz-dev-A
# A has the dirty file and staged bits. dest .gwz/ is installed:
# family-root → root + family_id; no local-family.yml, no catalog,
# empty .gwz/merge/. root is unchanged.

# open gwz merge — verbatim refuses (source mutator lock held through copy)
gwz merge feature/x
gwz clone --local --name nope ../gwz-dev-nope
# refuse: source has an open gwz merge. abort or --clean.

gwz merge --abort
gwz clone --local --clean -b lane/agent-17 --name C ../gwz-dev-C
# clean; freeze; recapture dest lock; branch before ready.

cd ../gwz-dev-A
gwz clone --local --clean -b lane/from-A --name B ../gwz-dev-B
# registers on root, not on A. B’s family-root is computed, not copied.

cd ../gwz-dev-C
gwz clone --local --clean --name D ../gwz-dev-D

cd ~/limbo/gwz-dev
gwz clone --local --bare --name hub ../gwz-dev-hub
# core.bare=true per member; gwz.conf last + family-root on hub.

gwz clone --local --name A ../elsewhere
# refuse: name A already holds ../gwz-dev-A

gwz clone --local --name origin ../nope
# refuse: origin is reserved / already a git remote

gwz clone --local --name root ../nope
# refuse: root is reserved

gwz clone --local --name A2 ../gwz-dev-A
# refuse: dest path already a family member (or already a workspace)
```

`gwz local list` **in D** (pointer → root index):

```text
root  checkout  ready  /Users/…/limbo/gwz-dev
A     checkout  ready  /Users/…/limbo/gwz-dev-A
B     checkout  ready  /Users/…/limbo/gwz-dev-B
C     checkout  ready  /Users/…/limbo/gwz-dev-C
D     checkout  ready  /Users/…/limbo/gwz-dev-D
hub   bare      ready  /Users/…/limbo/gwz-dev-hub
```

### 8.2 Pull and merge from a name

```sh
# in D — family name is --remote when it is a ready member
gwz pull --head --remote A

gwz --sync ff-only pull --head --remote root

gwz pull --head --remote origin   # git origin; not UnknownLocal
gwz push --remote origin          # git origin

# merge A’s paired HEADs into D
gwz merge --remote A

# merge a branch that exists on C
gwz merge --remote C lane/agent-17

gwz merge --remote origin         # UnknownLocal (family-only)

gwz push --remote hub             # current branch; hub is bare
gwz push --remote root            # refuse if that branch is checked out at root

gwz merge feature/x               # git ref; legal
gwz merge A                       # git ref A, not clone A
```

### 8.3 Two open merges (clean clones; no leftover copied merge)

```sh
cd ~/limbo/gwz-dev-B && gwz merge feature/x    # stall
cd ~/limbo/gwz-dev-C && gwz merge feature/y    # stall
# both legal. status/abort/continue are per cwd workspace.
```

### 8.4 Dispose C

```sh
cd ~/limbo/gwz-dev-D

# C has an open merge:
gwz local dispose C                      # refuse
gwz local dispose C --force              # refuse (no hazard names)
gwz local dispose C --force open-merge   # allowed; C’s directory gone

# alternate (no open merge): keep the tree
# gwz local dispose C --keep
# pointer gone; tree stays; NotAFamilyMember from that tree

gwz local list          # no C
gwz pull --head --remote C    # missing_remote (not family, not a git remote)
gwz merge --remote C          # UnknownLocal (family-only)
gwz merge --remote A          # still works
gwz pull --head --remote origin   # still git origin

gwz local dispose root
# refuse: never deletes the original tree. gwz local disband to drop the family.

gwz local dispose D     # from inside D
# refuse: cwd is D
```

### 8.5 Bare hub as the share point

```sh
cd ~/limbo/gwz-dev-B && gwz push --remote hub
cd ~/limbo/gwz-dev-D && gwz pull --head --remote hub
cd ~/limbo/gwz-dev-D && gwz merge --remote B
```

### 8.6 Teardown (disband last)

```sh
gwz local disband
# pointers + index gone; every dest directory still exists
# gwz push --remote hub  → missing_remote (hub was never a persisted git remote)
```

## 9. Build directories

Verbatim reflinks `target/` (disk, not a shared `CARGO_TARGET_DIR`).
`--clean` / `--bare` start without it.

## 10. What this is not

- Not GwzWt. No shared refdb, no shared exclude, no shared catalog.
  Gitfile sources refuse. Absolute `include.path` / `hooksPath` that
  would still name source after copy refuse.
- Not git alternates / `--shared`.
- Not `updateInstead` on a non-bare checkout.
- Not a merge-record version bump.
- Not `--filesystem-strict`.
- Not putting host paths in `gwz.yml`.
- Not persisted family git remotes.
- Not `gwz merge A` as a family name.
- Not `UnknownLocal` on `gwz pull --head --remote origin`.
- Not BOOL-only `--force`.

## 11. Open questions (defaults stated)

1. Verbatim while a gwz merge is open — **refuse** (folded).
2. Family merge spelling — **`gwz merge --remote <name> [<ref>]`**
   (folded). `name:ref` not used. Pull/push `--remote` has git-remote
   fallback; merge `--remote` does not.
3. Dest path default — sibling `../<root-dir>-<Name>`.
4. `local repair --root` — named, not in the first build; v0 fail-closed
   on `FamilyRootMismatch`.
5. `--clean --keep-build-cache` — not in v0.
6. `family_id` on the taut local-clone request — optional copy of the
   pointer id; authority is index + pointer files.
7. Clean freeze = recorded HEAD vs lock pin — default **recorded HEAD**
   + recapture dest lock **before** `ready`.
8. Absolute git config that would name source after copy — **refuse**
   (not rewrite) in v0.
