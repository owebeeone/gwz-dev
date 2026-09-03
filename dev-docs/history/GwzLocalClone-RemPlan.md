# GwzLocalClone — RemPlan

**Status:** remplan for design revision 3. Not implementation.
**Reviewed object:** `dev-docs/GwzLocalCloneDesign.md` SHA-256
`579c9fd82e32877aee9b02b59b4693d4bf5b7cca38e04adc7c42d49bb08b52ba`
at gwz-dev `68b6ba43050426a77447f3d9c4a00fc8eb2d380b`.
**Reviews:** `GwzLocalClone-ReviewConsistency.md` (NO-GO; pre-commit
P0-1, P2-1…P2-7), `GwzLocalClone-ReviewSafety.md` (NO-GO; pre-commit
P0-1, P0-2, P1-1…P1-7, P2-1…P2-6).
**Lane-owner merge:** dual NO-GO. One design patch. No code.

IDs below are `C-` (Consistency) and `S-` (Safety). Blind convergence
is named. P3s do not block; fold if cheap.

## Blind convergence (highest confidence)

| Defect | IDs | Disposition |
|---|---|---|
| Verbatim from root copies the family index / dest becomes a second root; merge/catalog identity also copied | **C-P0-1, S-P0-1** | **Install dest `.gwz/`.** Never copy `local-family.yml`, `catalog-final`, `checked-artifacts`, stash bundles, or lock files. Always write a new `family-root` (path to the *registering* root + `family_id`). Both index and pointer on dest → refuse. Verbatim may copy `.gwz/merge/` only as a **new** dest identity (new `merge_id`) or refuse if a gwz merge is open — default: **refuse verbatim while a gwz merge is open**. |
| `family-root` is copied / source-path rewrite; non-sibling dest joins the wrong tree | **C-P2-5, S-P1-6, S-P2-5** | **Never copy the pointer.** Write a fresh dest-relative or canonical path plus `family_id` in the pointer file. Open pointer, read index, compare ids; mismatch → `FamilyRootMismatch`, no write. |
| Create/dispose are porcelain + `exists()` | **C-P2-7, S-P1-1** | Index row state: `creating` → `ready` → `disposing`. One commit point on the root index. `recover()` of (intent × observed dest/pointer). Default: `creating` ∧ incomplete → delete partial dest, drop row (do not resume copy). `ready` ∧ dest missing → fail-closed on use; only `dispose` consumes. Path existence is not `ready`. |
| Family name vs git `source_ref` / remotes | **C-P2-2, S-P2-2, C-P2-4** | Bare merge token stays a **git ref**. Family source is `--remote <name>` and/or `name:ref`, plus optional `local_source_name` on the live `MergeRequest` — do not reuse `source_ref`. Reserve `root`, `origin`, `HEAD`, `FETCH_HEAD`, empty, `/`, `:`. Create refuses a name that is already a git remote in any member. |
| Bare hub is not a workspace and not bare | **C-P2-3, S-P2-3** | Each hub member `.git` is `core.bare=true`. Hub still has `gwz.conf/gwz.yml` (stub/copy of manifest) and `.gwz/family-root` so discovery works. `local list`/`dispose` run here. Layout-only non-bare `.git` is withdrawn. |

## Consistency-only

| ID | Disposition | Closure |
|---|---|---|
| C-P2-1 taut sketch ≠ live IR | New request/method for local clone (new `ActionKind`). Do not make `CloneWorkspaceRequest.url` optional. Name live types: `source_ref`, `PushRequest.remote`, `OperationPolicy.remote`, `PullHeadRequest`. Table in §7: each §8 CLI line → exact message + field tags. | Reviewer ticks the table against `gwz.taut.py`. |
| C-P2-6 §8.3 illegal after §8.1 | Split timelines. After verbatim-with-open-merge (now refused), §8.3 uses **clean** clones A and C, or abort first. | Example script: every `gwz merge` start is legal. |

## Safety-only

| ID | Disposition | Closure |
|---|---|---|
| S-P0-2 gitfile / commondir / absolute hooks | **Refuse** source members whose `.git` is a file, `commondir` is outside dest, or `core.worktree` / absolute `hooksPath` points outside dest. No convert-in-v0. | Gitfile source refuses; dest `rev-parse --git-common-dir` is inside dest. |
| S-P1-2 create has no index lock | Every index mutation takes the **root-index lock** + CAS on index bytes. Name-taken and dest-unique checks are inside that lock. | Two racing creates for `C` → one row, one dest. |
| S-P1-3 dest path not unique | Dest must not exist (or empty + unregistered); canonical dest ≠ any other row; dest must not already contain `gwz.conf/gwz.yml`. Nested dest (C-P3-1) refuses. | Two names, one dest → second refuses. |
| S-P1-4 dispose deletes unique work | Refuse dispose of a checkout with open gwz merge, member `MERGE_*`/rebase, or dirty trees, unless `--force` names the hazards. Hold C’s mutator lock before delete. Sweep is part of `disposing` recover. | Open-merge dispose refuses; no index change. |
| S-P1-5 `--keep` ghost admin | `--keep` deletes dest `family-root`; family verbs from that tree refuse `NotAFamilyMember`. Sweep remotes from and to the kept tree. | Kept tree cannot `dispose` a sibling. |
| S-P1-7 host paths via `repo sync` | Family bindings are **not** persisted git remotes that `repo sync` captures. Resolve at operation time from the index. `gwz.yml` remotes stay origin-only. Verbatim strips filesystem remotes and credential URLs from copied `config`. | After pull-from-name, `gwz.yml` has no host path and no extra remote names. |
| S-P2-1 `--clean` mixed vs lock | Freeze source lock+HEAD once under source mutator lock; every member (incl. dest root) is taken at that freeze. `-b` after dest is `ready`. Recapture dest lock to dest HEAD (C-P3-2). | HEAD≠lock → dest matches freeze on every member or refuses. |
| S-P2-4 dispose root / `--force` | `dispose root` **never** deletes the original tree. Disband = drop index + everyone’s pointers; trees stay. Path-gone + row = dispose recovers that row (no `--force`). `--force` without a row only sweeps leftover remotes, never deletes by path guess. | Documented; `dispose root` cannot unlink `gwz.conf/`. |
| S-P2-6 pair by `member.path` | Pair by dest lock **member id / `source_id`**. Set mismatch refuses (aggregating). Path is not the key. | Divergent path, same id → refuse, no fetch. |

## Non-blocking (fold)

- C-P3-1 nested dest — folded into S-P1-3.
- C-P3-2 lock after `-b` — folded into S-P2-1.
- C-P3-3 `family_id` on the wire — pointer file + index; not a taut field unless the local-clone request carries it for the dest bind. Strike “on the wire” if index+pointer suffice.

## Next

One revision of `GwzLocalCloneDesign.md` applying every row above. Same
reviewers re-verdict. Do not implement from revision 2.
