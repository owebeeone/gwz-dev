# Gwz local clone — Requirements & Design

Status: **DRAFT 2026-09-05 revision 8** — retains the operator's
“best effort is good enough” direction and adds review corrections for
preflight/dry-run behavior and independent library boundaries. Supersedes
revision 7 (SHA-256
`5fe266c7b947f147f3522c8f857d2ec4cb4fed77fd9e09f876b42b2c4a8ec3a8`).
Prior reviews remain evidence about their recorded inputs, not independent
acceptance of this revision. GwzWt remains superseded for agent-lane
isolation. Planning only; no implementation or release acceptance claimed.

**Engineering constraint:** use ordinary filesystem/Git operations and
existing merge behavior. Do not build a new checked-artifact catalog,
native identity framework, durable transaction journal, merge-record
version, or cross-platform crash-recovery protocol for local clones.
An interrupted operation may require inspection and manual cleanup.
That is an accepted outcome, not a reason to expand the subsystem.

The [implementation architecture](GwzLocalCloneImplementationArchitecture.md)
and [library boundary adoption](GwzLocalCloneLibraryBoundaries.md) require
independently compiled/tested new libraries, small explicit contracts and
seconds-scale fast-test targets. This adopts the shared library policy for
local clone only, without a refactor of the existing merge subsystem.

Source independence, explicit errors, and the default refusal to delete
unpreserved work remain requirements. Best effort describes filesystem
consistency/recovery guarantees; it does not turn a failed copy into a
success or a clean Git status into proof that history is saved.

Framing: a second workspace, using native copy-on-write when available
and ordinary copying otherwise, followed by fresh destination `.gwz/`
installation. New requests remain taut-defined; the existing network
clone's URL stays required. Existing merge safety behavior is unchanged.

## 1. Problem

Agents want isolated lanes, including independent structural merges.
Linked worktrees share branch refs and GWZ merge-evidence surfaces;
the historical GwzWt design therefore refused structural merges in
linked lanes. Independent local repositories remove that coupling.
Network `gwz clone <url>` also isolates, but requires network transfer
and supplies no local family names. Clean local Git clones are another
possible implementation strategy; filesystem copy additionally preserves
staged changes, dirt, and build output. Reflinks are an optimization,
not a guarantee of constant-time or atomic workspace copying.

A one-hop `primary` pointer is not enough: `root→A→B` and
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
  is a `ready` member, else today's Git remote if no family row has
  that name** (`origin`, `upstream`, …). A non-ready row refuses on its
  lifecycle state. `gwz merge` keeps `source_ref` as a **git ref**. Family merge is
  `gwz merge --remote <name> [<ref>]` (optional
  `MergeRequest.local_source_name`) and is **family-only** (no git-remote
  fallback; `--remote origin` is a family miss).
- Two clones may each have an open merge. They do not share `.gwz/`
  identity stores.

Disk create is a filesystem copy (reflink when the FS answers, else
copy) plus the dest `.gwz/` install (§4.1). Source Git and worktree
contents are not modified; registering the clone updates the root index.

**Concurrency envelope:** same-user local use, with the involved source,
receiver, deletion target and preservation witnesses quiescent while a
local operation runs. This includes unrelated GWZ commands, editors,
builds, raw Git, and Git maintenance. Other lanes may continue working
when they are not endpoints or witnesses of this operation.

A single advisory family lock serializes family-changing commands and
family exchanges. Ordinary GWZ commands keep their current locking;
local clone does not retrofit family admission into every mutator.
Recheck source manifest/lock/HEADs/refs and obvious path changes before
publication or deletion. These are useful checks, not an atomic snapshot
or a guarantee against concurrent replacement or power loss.

Normal write/copy errors stop the command. An interrupted create leaves
files to inspect; an interrupted delete leaves whatever remains. No
later read or unrelated command automatically finishes deleting them.

## 3. The family index — `root` holds it

```text
root/.gwz/local-family.yml    # one untracked family index
root/.gwz/local-family.lock   # advisory lock for local-family mutations
clone/.gwz/family-root        # family_id + path to registering root
clone/.gwz/local-clone-allocation  # ordinary allocation-id marker
```

Names resolve through the registering root, not through the clone's
creator. Clones of clones therefore remain in one namespace. A clone
must not contain both an index and a pointer. Core derives `family_id`
from the admitted index/pointer; it is not client-supplied authority.
Host paths never enter `gwz.conf` or persisted Git remotes.
The index has a format version, core-minted family id, root entry and
member map. Paths are root-relative. `root` always names the original
workspace and cannot be disposed. The allocation marker detects ordinary
mix-ups; it is not authoritative proof of persistent filesystem identity.

Conceptual row:

```text
name -> { path, kind: checkout|bare, state: creating|ready|disposing,
          allocation_id, source_path, mode, last_error? }
```

Keep the index small: maximum encoded size 1 MiB; oversize or malformed
input refuses before mutation. There is no persisted object-graph proof,
copy progress log, or replayable deletion authorization in this file.

### 3.1 Metadata updates and interrupted operations

Use an advisory lock, reread/validate the index, then ordinary
same-directory temporary-file write and rename. Check every error and
use checked ordinary flush operations available on the platform. This is best-effort metadata
publication, not a power-loss-safe multi-file transaction. Do not make
catalog activation or native persistent identity a prerequisite.

Name/path collisions are checked inside the family lock. Allocate only
a new or explicitly admitted empty destination. Use canonical paths,
no-follow checks where available, pointer equality and an allocation
marker to catch ordinary mistakes/replacements. These are same-user
checks, not a durable identity proof against arbitrary path substitution.

`gwz local list` is **observation-only**: it reports recorded and observed
state, missing paths and malformed/incomplete metadata. It never writes,
repairs, promotes, removes pointers, or deletes files.

| Observed state | Behavior |
|---|---|
| `ready`, valid pointer/path | use normally |
| `creating`, whether partial or apparently complete | report incomplete; do not auto-promote, resume copy, or delete |
| `disposing`, target still present | report interrupted deletion; never resume directory deletion automatically |
| row present, target absent | list reports missing; explicit dispose may remove the stale row |
| mismatched id, unexpected path, malformed metadata | refuse mutation; retain files for inspection |
| interrupted pointer-only detach/disband | an explicit repeat may remove remaining matching pointers/rows; never directory contents |

`dispose --keep` detaches an incomplete or interrupted lane and retains
all remaining contents. It does not repair the tree or restore deleted
files. A directory with a stale `creating`/`disposing` row is not a family
exchange endpoint, even if its manifest happens to exist. Operators must
not use an incomplete destination as a working lane.

`disband` removes matching clone pointers and finally the root index,
retaining all trees. It is repeatable on explicit invocation. No global
recovery daemon or implicit recovery on reads is introduced.

### 3.2 Lock scope

Local create, dispose, disband, and family exchange take the one root
family lock, refusing busy. Keep it through the local command, including
a synchronous call into the existing merge/pull implementation. This
serializes family operations but does not serialize ordinary GWZ writers.
The quiescence assumption in §2 supplies that narrower concurrency model.

Do not pre-acquire endpoint workspace mutator locks around the merge
engine. Let it acquire/release its existing locks normally. Family
resolution is completed before delegating; the inner engine must not
resolve the family name again or reacquire the family lock. No changes
to `WorkspaceMutationGuard`, `V1MutationLease`, or the merge lifecycle's
lock handoff are required by this feature.

A failed best-effort index update reports failure even if some filesystem
effects already occurred. Ambiguous metadata leaves the affected lane
unusable for family operations until explicitly detached or inspected;
it does not license guessed cleanup.

## 4. Create

`gwz clone --local --name <Name> [dest]` from the cwd workspace (any
`ready` family member, or `root`). Dest defaults to
`../<root-dirname>-<Name>`. Copy source is cwd (or `--from <name|path>`).

Under the family lock and §2's quiescence assumption:

1. Aggregate name, path, source-layout and nested-repository checks before
   reservation. Refuse overlap, a nonempty destination, or a destination
   that is already a workspace. Do not fetch missing repositories.
2. Write a `creating` row, allocate the destination and write its marker.
   Capture source manifest, lock and repository HEAD/ref observations in
   memory for this invocation. Stop on any failed step.
3. Copy with exclusions applied during traversal, install fresh destination
   metadata, and validate Git independence. Recheck the source observations.
   For clean/bare, create branches and recapture configuration before ready.
4. Write the final manifest last, then update the row to `ready`. Errors
   or interruption leave the directory and diagnostic row for inspection;
   there is no automatic cleanup or promotion, even when it looks complete.

Use native copy-on-write when supported, falling back to ordinary copying
for a classified unsupported operation. Never use hardlinks to the source.
Permission, space, I/O and metadata failures are errors, not “unsupported.”
Preserve file contents, symlinks and normal permissions; use native metadata
copying where available and report unsupported metadata. No new portable
ACL/xattr/stream equivalence framework is part of the feature.

### 4.0 Source git identity (all modes)

Inventory every included Git repository before copying: workspace root,
materialized members, and unmanaged/ignored nested repositories. Apply
these checks and exclusions to each, or refuse an unsupported nested
layout before reservation. Refuse (aggregating) if any has:

- `.git` as a **file** (gitfile / linked worktree),
- `commondir` outside that member,
- `objects/info/alternates` or `objects/info/http-alternates`, including
  an inherited file from an earlier `clone --shared` / `--reference`,
- Git metadata or an object-store path that resolves through a symlink
  outside that repository's own copied boundary,
- `core.worktree`, effective `core.hooksPath` (absolute or relative),
  `include.path`, `includeIf`,
  or `url.*.insteadOf` that would still name a path **outside dest**
  after copy.

Resolve hook paths in the proposed destination layout **before reservation**.
A relative hook path is relative to the hook's working directory, not
necessarily the config file: normally the worktree root, or the Git
directory for bare repositories and push-triggered hooks. Check applicable
bases and symlink resolution; keep valid internal relative paths. Unknown
or escaping effective paths refuse. This follows Git's
[hooks-path configuration](https://git-scm.com/docs/git-config#Documentation/git-config.txt-corehooksPath)
and [hook working-directory rules](https://git-scm.com/docs/githooks).

Reject invocation-level `GIT_ALTERNATE_OBJECT_DIRECTORIES`,
`GIT_OBJECT_DIRECTORY`, `GIT_COMMON_DIR`, `GIT_DIR`, or `GIT_WORK_TREE`
overrides for local create; do not let environment redirection bypass
the per-repository checks. v0 does not dissociate borrowed objects or
convert gitfile sources. An internal partial-clone/promisor dependency
must also refuse if the objects needed for the promised local copy are
not available locally; no implicit network hydration.

No convert-in-v0. Default is **refuse**, not rewrite. Dest
`git rev-parse --git-common-dir` must be inside dest after create.
Validate reachable object connectivity with alternates and external
object-directory overrides disabled. dest-complete fails on missing
objects or if dest config still names a path outside dest. Merely
having `.git` and an internal common directory is insufficient.

### 4.1 Verbatim (default) — tree copy + dest `.gwz/` **install**

Copy the source tree as it sits: committed files, unstaged edits,
untracked files, `target/`, each member’s `.git` (index, `MERGE_HEAD`,
rebase state). Source is not modified. The operator keeps it quiescent
for the entire copy/install invocation; the family lock does not lock
ordinary writers.

**Copy exclusions and destination install:** exclusions run during
copy. An excluded source path need not be absent after installation;
the final column is the completion rule.

| Path | During copy | At ready |
|---|---|---|
| `.gwz/local-family.yml` | omit | **absent** (dest is not root) |
| `.gwz/local-family.lock` | omit | **absent** (the family lock lives at root) |
| `.gwz/local-clone-allocation` | omit | **regenerated** for this destination |
| `.gwz/family-root` | omit | **regenerated**: registering root path + `family_id` |
| `.gwz/catalog-final` | omit | **absent** |
| `.gwz/checked-artifacts` | omit | **absent** |
| `.gwz/locks/` | omit | absent or **fresh destination-owned** locks created during install; never copied leases |
| `.gwz/merge/` (open **and** `done/`) | omit | **absent**, including an empty directory; created on first merge |
| `gwz.conf/gwz.yml` | omit | **regenerated**, written last; dest not discoverable until then |
| stash bundles under `.gwz/` | omit | **absent** |
| `.git/worktrees/` in every included repository | omit | **absent** |
| filesystem / credential URLs in copied `.git/config` remotes | remove in install | absent; ordinary non-credential https/ssh origin URLs remain |

Recapture may write `gwz.conf/gwz.lock.yml` before the manifest.
Regenerate the conf-integrity marker for the final manifest and lock
bytes as part of install; do not publish a copied marker vouching for
superseded bytes. The source checks and destination object validation
in §4.0 apply independently of this exclusion table.

**Open gwz merge:** **refuse verbatim** while source has an open gwz
merge (checked before copy under the quiescence assumption). Dirt and
per-repo `MERGE_HEAD` are still copied when no gwz merge is open.

Post-copy check: dest must not contain `local-family.yml`; dest must
contain `family-root` whose target is the registering root; both files
together → refuse; dest `.gwz/merge/` must be absent.

### 4.2 Clean (`--clean`, optional `-b <branch>`)

Under the family lock and §2's quiescence assumption, capture
source manifest, lock, refs, and HEADs **once**, including the root
repository (held in memory for this invocation).
Every destination member, including root, uses that captured vector.
Observed source drift stops publication; no atomic snapshot is promised.

- Dest trees are a clean checkout at the freeze (lock pin **or**
  recorded HEAD — one choice for all members; default: **recorded HEAD**
  so a dirty-uncommitted source still has a defined commit).
- Recapture dest lock to dest HEAD **before** dest-complete may be
  true. dest-complete includes dest lock/HEAD agreement. If recapture
  cannot be done, dest is incomplete (§3.1) — do not flip `ready`.
- `-b <branch>` runs **before** dest-complete / `ready`, creating that
  branch in every git member at the frozen commit, then recaptures dest
  desired branches. The requested branch is part of the in-memory plan.
  If `<branch>` already exists in any member at freeze time, refuse the
  whole create (aggregating) **before** the `creating` row.
- No source dirt, no member `MERGE_*`, no source `.gwz/` merge. Dest
  `.gwz/` is the same **install** as §4.1 (pointer, no index, empty
  merge store).
- `origin` remotes stay the source’s `origin` URLs (non-`file:`).
- No `target/` on `--clean`. `--clean --keep-build-cache` is not v0.

“Clean” means source worktree/index dirt is not inherited. Recapturing
the destination lock and desired branches can leave generated changes
under `gwz.conf/` relative to the frozen root commit. Report those changes;
do not silently create a root commit to hide them. Any later command
requiring a clean root uses its ordinary commit/refusal rules.

`--verbatim` and `--clean` are mutually exclusive. `--bare` implies
`--clean` (no worktree dirt).

### 4.3 Bare (`--bare --name hub`)

Same workspace **directory layout** (root + each `member.path`). Each
member `.git` is a **real bare repo** (`core.bare=true`, no worktree).
Hub **is** a gwz workspace: `gwz.conf/gwz.yml` (copy of the manifest,
written last) and `.gwz/family-root` so `discover_workspace_root`
works. `local list` and `dispose` run here.

Verbs that need a worktree (`commit`, `add`, merge that writes trees,
worktree-file status inspection) refuse, typed. `push` / fetch into hub, `log`,
`local list`, `dispose` work. `updateInstead` is not used.

## 5. Dispose

```sh
gwz local dispose C
gwz local dispose C --keep
gwz local dispose C --force unpreserved-history
gwz local dispose C --force open-merge,dirty,unpreserved-history
gwz local disband          # pointer/index removal only; trees stay
```

From a **family member** (index membership; `--keep` trees are not
members). The family lock excludes a simultaneous disband or exchange.

**Default chosen by the operator (2026-09-05): refuse deletion unless
history is verifiably preserved elsewhere.** No automatic archive or
background retention policy. A clean working tree is not preservation
proof. The rule applies equally to checkouts and bare hubs, including
their workspace root repositories.

### 5.1 Pre-deletion checks

Inspect every repository in the deletion tree, including root and
unmanaged nested repositories. Unknown layouts/evidence refuse ordinary
deletion. Protect all refs, HEAD (including detached), retained reflog
roots, annotated objects, native stashes and older stash entries. Already
unreachable objects named by none of these roots are outside this scope.
GWZ stash coordination records additionally need surviving interpretable
copies and their referenced Git objects; otherwise refuse.

Look in root and other ready family members outside the deletion subtree,
paired by member/source identity, with root paired explicitly. Verify
protected graphs are reachable from surviving refs/HEAD with locally
available self-contained objects. Retained local-import refs (§6.2) are
ordinary persistent witness roots. Existing temporary operation refs are
not witnesses. Unreferenced object presence, target-local origin tracking,
a prior push, or a squash with the same patch does not establish coverage.
Network-only preservation is not certified; fetch into a survivor first
or keep the lane.

The verifier is read-only. Bound its inventory/graph work and return
`Unknown` on unsupported evidence or a resource limit. Compute its result
in memory for this invocation; do not persist graphs or proof artifacts
in the family index. The target and witnesses must remain quiescent until
deletion completes. No historical proof will be replayed after a crash.

Work inspection includes staged/unstaged/untracked and ignored user data.
Ignored does not mean disposable. For tracked paths with assume-unchanged,
skip-worktree or unsupported index flags, inspect actual bytes without
altering the index, or refuse as unknown. Valid sparse absence requires
explicit handling; ordinary Git status alone is insufficient.

### 5.2 One-shot disposal

1. Take the family lock and validate name, pointer and path. Refuse root,
   cwd, path overlap, unexpected replacements and unrecognized targets.
2. With `--keep`, remove only the matching pointer and row. Skip dirty/
   history checks because all files remain, including incomplete data.
3. For ordinary deletion, require an intact ready target and run fresh
   work/history checks under §2. Known open-merge, dirty or unpreserved
   history hazards refuse unless explicitly named by the existing force
   syntax. Empty/unknown force names and keep+force refuse. A path mismatch
   or an earlier interrupted deletion is not forceable through this path.
4. Write `disposing` before removing contents; check the write result.
   Remove only the validated target using ordinary filesystem operations,
   without following symlink entries into external trees. Stop on an error.
5. When deletion succeeds, remove the row. If contents are already gone,
   a later explicit dispose may remove the stale row after validation.

An interruption may leave a partial tree. There is **no deletion replay**,
rollback, witness journal, or automatic escalation to force. A later
command reports the interrupted state; `--keep` detaches the remainder.
Manual cleanup is an accepted recovery path. A detected changed target
or witness before deletion stops the command, but no claim is made about
racing ordinary writers or power-loss ordering between metadata and files.

An incomplete create is also retained; local-clone automatic cleanup never
deletes directories in any milestone. Full forced removal of incomplete
or interrupted trees is outside this version. Explicit named force for
an intact ready tree is an operator loss waiver, not crash recovery.

Disband only removes pointers/index and may be explicitly repeated after
an error. It never routes a remaining row through directory deletion.

## 6. Resolving a name

Load the root's own index, or a clone's pointer → index; validate under
§3.2. Inspection performs no recovery writes. For pull/push, treat a name that
exists but is not ready as a lifecycle refusal; do not silently fall
back to a same-named Git remote while that row is creating/disposing.
An absent family name may take the pull/push fallback below.

**Pull / push `--remote` (`OperationPolicy.remote` / `PushRequest.remote`):**

1. Token is a `ready` family name → bind from the index: take that
   member’s path; for each **selected dest member**, pair by dest lock
   **member id / `source_id`**. Path is not the key. Set mismatch
   (missing / extra / path moved for the same id) refuses aggregating;
   no fetch. Pair a selected `@root` separately with the other
   workspace's root repository: it has no member-lock entry. Preserve
   each verb's existing selection defaults.
2. If no family row has the name, resolve as today's Git remote
   (`origin`, `upstream`, …).
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

### 6.1 Branch and ref contract

| Operation | Source to integrate or publish | Destination |
|---|---|---|
| `pull --head --remote A` | A's `refs/heads/<branch>` where `<branch>` is the receiving member's locked branch, else desired branch, else `main`; for selected `@root`, use its current attached branch | receiving repository's existing pull target; ordinary sync policy applies |
| `merge --remote A` | A's HEAD commit, resolved independently for every paired participant | receiving participant's current merge target |
| `merge --remote A <ref>` | `<ref>` resolved **in A**, independently for every paired participant | receiving participant's current merge target |
| `push --remote A` | each selected repository's current attached branch | the same `refs/heads/<branch>` in A |

An explicit existing `PushRequest.refspec` retains its explicit mapping
and normal validation; absence means the same-branch rule above. No new
pull source-ref flag is introduced in v0. Missing branches/refs and
unsupported detached-head operations refuse aggregating before transfer.
For cross-branch integration, use `merge --remote A [<ref>]`.

`pull --head` means the selected **branch tip**, not the remote
repository's symbolic HEAD. Pushing `lane/from-A` into a hub creates or
updates that branch; it does not change the hub's HEAD, update `main`,
or populate `lane/agent-17`. A push into a checked-out branch refuses;
GWZ does not enable or honor `updateInstead` as a family-push shortcut.

### 6.2 Transfer using retained import refs

Validate request shape and supported modes before any family-side effects.
In v0, local create and family merge/pull/push refuse unsupported dry-run
before creating a family lock file, reserving metadata or importing refs.
Resolve pull/push fallback with read-only observations first so ordinary
Git-remote dry-run behavior remains unchanged. An invalid family merge
start request must not fetch first. Existing engine-state failures after
import may leave retained refs as described below.

Under the family lock and quiescence assumption, resolve every pairing
and selected source ref once. Refuse mismatches before transfer. Fetch
through anonymous local transport with explicit refspecs into receiving
repositories, then verify every received OID against the captured vector
before invoking merge/pull. A partial or mismatched fetch does not start
a partly sourced merge.

The current backend's named-remote fetch/push methods do not provide this
port. LCM1.0 adds a narrow local-only anonymous transport adapter, explicit
refspec support and test-double coverage under the shared integration owner;
this is planned work, not an already implemented seam.

Use one fresh collision-checked import name in every paired receiver:

```text
refs/gwz/local-imports/<transfer-id>
```

These are **retained ordinary Git refs**, separate from the existing
`refs/gwz/merge/...` preservation/cleanup namespace. Each receiver may
have a different captured OID under the common import name. After all
fetches verify, the adapter supplies that local source ref to the
existing planner/engine. Existing records continue to hold their usual
per-participant source OIDs; no record field/version or durable owner
handoff is added. The engine keeps its current locking and recovery.

No automatic pruning of import refs in v0, on success, error, cancellation,
crash, detach, or disband. The refs themselves retain the objects through
ordinary Git GC. Leftover imports and extra disk usage are accepted costs;
they are listed with ordinary Git refs and count in preservation checks.
A retry uses a fresh transfer id. A later explicit pruning feature may
be designed separately; users must not manually prune refs needed by an
open merge. This avoids a second recovery/GC protocol.

After merge start, source advancement, disposal or name reuse does not
change its imported commits. An incomplete import before engine entry
has only the refs it managed to create; inspect or retry explicitly.
Do not claim stronger power-loss guarantees than ordinary Git provides.

Do not persist family remotes or overwrite origin tracking refs. Family
push keeps existing per-repository partial-result behavior and checked-out
branch protection. No multi-repository transaction is introduced.

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
  # Field 7 is not a public family_id input; core derives family identity.
)

LocalCloneMode = Enum(verbatim=0, clean=1, bare=2)

LocalFamilyRequest = Msg(
  meta=F(1, RequestMeta),
  op=F(2, LocalFamilyOp),                 # list=0, dispose=1, disband=2
  name=F(3, STR, optional=True),          # dispose
  keep=F(4, BOOL, optional=True),
  force_hazards=F(5, List(STR)),          # §5.2 named hazards; includes unpreserved-history
)
# Absent/empty list = no force. CLI rejects bare --force before encoding;
# core rejects unknown hazards and keep+force. BOOL-only force is withdrawn.

MergeRequest.source_ref = F(3, STR, optional=True)   # git ref; UNCHANGED
MergeRequest.local_source_name = F(9, STR, optional=True)  # NEW; start only
# F(8) is filesystem_strict (ship 1). Do not reuse.

PushRequest.remote = F(2, STR, optional=True)        # ready family; absent row -> Git remote
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
| `gwz local dispose C --force unpreserved-history` | same, force_hazards=["unpreserved-history"] |
| `gwz local dispose C --force open-merge,dirty,unpreserved-history` | same, force_hazards=["open-merge", "dirty", "unpreserved-history"] |
| `gwz local disband` | `LocalFamilyRequest` op=disband |
| `gwz pull --head --remote A` | `PullHeadRequest`; `OperationPolicy.remote=A` (family bind) |
| `gwz pull --head --remote origin` | same; git remote `origin` (not UnknownLocal) |
| `gwz --sync ff-only pull --head --remote root` | family bind root |
| `gwz merge feature/x` | `MergeRequest` op=start `source_ref=feature/x` |
| `gwz merge --remote A` | `MergeRequest` op=start `local_source_name=A` (HEAD per paired member) |
| `gwz merge --remote C lane/agent-17` | `local_source_name=C` `source_ref=lane/agent-17` |
| `gwz merge --remote origin` | `UnknownLocal` (family-only; reserved / not a member) |
| `gwz push --remote hub` | `PushRequest` remote=hub (family); current branch to the same branch in hub |
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

Common setup: root and its members start clean on `main`; the named
lane branches do not yet exist. Settle any reported generated
`gwz.conf/` changes with `gwz commit` before later clean-root operations.

```sh
cd ~/limbo/gwz-dev
gwz clone --local --name A ../gwz-dev-A
# A has the source tree and Git state. dest .gwz/ is installed:
# family-root → root + family_id; no local-family.yml, no catalog,
# no .gwz/merge/. Source Git/worktree unchanged; root index updated.

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

Verbatim also preserves staged edits, unstaged edits, and untracked
files when those exist at copy time. Quiesce writers first (§2).
The common setup above starts clean so the following exchange examples
are not implicitly blocked by leftover dirt.

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
# D is on lane/agent-17; A and root have only main from the setup.
cd ~/limbo/gwz-dev-D
gwz pull --head --remote A
# refuse: A does not have refs/heads/lane/agent-17

# Explicit cross-branch integration of A's paired HEADs into D:
gwz merge --remote A

# Same-branch pull from C; alternatively name its branch for merge:
gwz --sync ff-only pull --head --remote C
gwz merge --remote C lane/agent-17

gwz merge --remote origin         # UnknownLocal (family-only)

# A and root are both on main, so root's checked-out branch is protected:
cd ~/limbo/gwz-dev-A
gwz push --remote root            # refuse: main is checked out at root
gwz --sync ff-only pull --head --remote root  # same-branch source
```

The spelling `gwz pull --head --remote origin` / `gwz push --remote
origin` still selects the Git remote, with its ordinary branch,
credentials, and divergence checks; family creation does not promise
that a newly named lane branch already exists there. `gwz merge
feature/x` and `gwz merge A` resolve Git refs in the receiving
repositories, not family names; those refs must actually exist.

### 8.3 Two open merges

For this conflict scenario, assume B and C have developed commits on
their respective lane branches and the selected members contain the
named conflicting feature refs. Both start with clean trees and no
open merge. This is explicit scenario work after the common setup,
not a claim that every arbitrary `feature/x` already exists.

```sh
cd ~/limbo/gwz-dev-B && gwz merge feature/x    # conflicts; stays open
cd ~/limbo/gwz-dev-C && gwz merge feature/y    # conflicts; stays open
# Both legal. status/abort/continue are per cwd workspace.

cd ~/limbo/gwz-dev-C
gwz clone --local --name nope ../gwz-dev-nope
# refuse: source has an open gwz merge. Abort it or use --clean.
```

### 8.4 Dispose C

C still has the open merge from §8.3. Assume its history has not all
been preserved in another family member.

```sh
cd ~/limbo/gwz-dev-D
gwz local dispose C
# refuse: open-merge, dirty, and any unpreserved-history
gwz local dispose C --force
# refuse: no hazard names
gwz local dispose C --force open-merge
# refuse: dirt and unpreserved history were not authorized

gwz local dispose C --keep
# Detach only: C's entire tree, open merge, and history stay on disk.
# C's own ordinary gwz merge recovery remains usable after detachment.
# The remaining examples follow this --keep path.
```

The explicit destructive alternative is
`gwz local dispose C --force open-merge,dirty,unpreserved-history`.
Run it only instead of `--keep`; it deletes C and waives the named
loss checks. Incomplete or interrupted targets are retained; use `--keep`
and explicit manual cleanup instead of force-replaying deletion. A clean
lane with unique commits still refuses without `unpreserved-history`;
a bare hub gets the same history check.

```sh
gwz local list          # no C
gwz pull --head --remote C    # missing_remote (not family, not a git remote)
gwz merge --remote C          # UnknownLocal (family-only)
gwz merge --remote A          # still works

gwz local dispose root
# refuse: never deletes the original tree. gwz local disband to drop the family.

gwz local dispose D     # from inside D
# refuse: cwd is D
```

### 8.5 Bare hub as the share point

```sh
cd ~/limbo/gwz-dev-B
gwz merge --abort                # close B's §8.3 merge before publishing
gwz push --remote hub
# Publishes lane/from-A in every selected repository, including @root.
# Hub HEAD remains main; no lane/agent-17 branch is created by this push.
gwz pull --head --remote hub     # same branch: lane/from-A

cd ~/limbo/gwz-dev-D
gwz pull --head --remote hub
# refuse: hub has no lane/agent-17 from this scenario
gwz merge --remote hub lane/from-A
# Explicitly integrates B's published member commits into D's lane.
# Finish/resolve this merge before starting another one in D.
```

Direct integration without publishing to a hub is the alternative
`gwz merge --remote B`. Neither form implicitly merges `@root`; use
the existing explicit root-selection surface when root history is
part of the intended integration. A later disposal of B still checks
all its refs, reflogs, stashes, and root history, not just the branch
that was pushed here.

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
  Included gitfile sources, including unmanaged nested repositories,
  refuse. Absolute `include.path` / `hooksPath` that would still name
  source after copy refuse.
- Not git alternates / `--shared`.
- Not automatically archiving deleted lanes. Default deletion needs a
  verified surviving history copy; `--keep` retains the original tree.
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
6. `family_id` is core-derived from the index/pointer; no public request
   field grants family membership.
7. Clean freeze = recorded HEAD vs lock pin — default **recorded HEAD**
   + recapture dest lock **before** `ready`.
8. Absolute git config that would name source after copy — **refuse**
   (not rewrite) in v0.
9. Disposal — **refuse without verified preservation** (operator
   choice, 2026-09-05). v0 proof uses surviving family repositories;
   network-only preservation is not automatically certified. `--keep`
   detaches without loss; `--force unpreserved-history` explicitly
   waives the history check. Dirty/open-merge hazards remain separate.
10. Pull branch selection — **keep existing same-branch semantics**;
    cross-branch integration uses family merge with HEAD or an explicit
    source ref. A hub's symbolic HEAD is not a pull routing mechanism.

## 12. Acceptance cases for the implementation plan

These are behavioral tests for the best-effort contract. Native copy
smoke tests and forced byte-copy tests are required where enabled; an
exhaustive platform durability/identity matrix is not a prerequisite.

| Case | Required outcome |
|---|---|
| Native copy unavailable | ordinary independent copy; actual method reported |
| Copy permission/space/I/O error | fail, retain partial destination, source unchanged |
| root → A → B | one namespace, independent repositories and runtime stores |
| Included ignored nested gitfile/alternates/external metadata | refuse before reservation |
| `local list` sees creating/disposing/missing/malformed rows | report only; no writes or deletion |
| Interrupted create, including manifest-before-ready gap | retain and report; explicit keep can detach |
| Observed source drift before publication | fail without marking ready |
| Unique root/branch/reflog/tag/stash history | refuse ordinary deletion until covered in a survivor |
| Edited assume-unchanged/present skip-worktree file | dirty or unknown; never silently clean |
| Unknown or oversized work/history inventory | refuse ordinary deletion before disposing |
| Clean intact lane with all protected history elsewhere | explicit dispose deletes without creating an archive |
| Error/interruption during deletion | stop; retain/report remainder; no later automatic deletion |
| Dirty/open/incomplete lane with keep | detach matching metadata and retain all remaining files |
| Two local-family mutation commands | family lock makes one busy; unrelated ordinary writers require operator quiescence |
| Family merge delegates to existing engine | no preheld receiver workspace lock; existing lifecycle unchanged |
| Import crashes before engine entry | retained Git refs may remain; no record repair or automatic pruning |
| Open merge after source detach/disposal or ordinary Git GC | retained import ref keeps source objects local |
| Hub receives lane/from-A, receiver uses lane/agent-17 | same-branch pull refuses missing branch; explicit merge selects intended import; hub HEAD unchanged |
| Disband after partial pointer removal | explicit repeat can finish pointer removal; all trees remain |
