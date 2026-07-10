# GWZ Repo Membership Lifecycle Plan (`clone` + `detach` + `attach`)

Status: design + implementation plan, 2026-07-10 (revised after ReviewF5 and
Review56s: frozen identity, re-add/reactivation, active-path, boundary,
concurrency, and protocol rulings).

No implementation is part of this document.

## Purpose

Ship three complementary membership commands in one plan — they touch the same
protocol, core handlers, CLI/`gwz-py` surfaces, docs, and path/id collision
rules:

| Command | Intent |
|---------|--------|
| `gwz repo clone <url> [member-path]` | Clone a remote into an **existing** workspace and register an **active** member |
| `gwz repo detach <member>` | Soft-remove a member from the **current** workspace composition by marking it **inactive** |
| `gwz repo attach <member-id>` | Reactivate an **inactive designation** at its existing checkout and preserve its identity |

Together they close the grow/shrink gap:

- `gwz init <url>…` — clone remotes only while **creating** a workspace;
- `gwz repo add <path>` — register a repo that **already exists on disk**;
- `gwz repo create <path>` — create an **empty** local member;
- `gwz clone <url>` — clone a **workspace root** and materialize the lock;
- `gwz materialize --lock` — clone members **already** listed in the lock;
- **no** one-shot “add remote member to existing workspace”;
- **no** supported way to stop managing a member without hand-editing
  `gwz.conf`;
- **no** explicit inverse that reactivates the same historical designation.

## Why inactive (not delete)

`GWZDesign.md` already treats **`member_id` as stable identity** and **`path` as
a materialization location, not identity**. The manifest already has
`active: bool`. Selection already filters with `active_members` and fails
explicit selection of inactive members with `MemberInactive`.

**Detach must not delete the manifest entry.** It sets `active: false`.

That preserves:

- historical identity for snapshots / markers / archaeology that still name
  `mem_…`;
- a clear audit trail in `gwz.yml` of members that used to be part of the
  workspace;
- the ability to later register a **different** Git repository at the **same
  path**, as a **new** member with a **new** `member_id` (and usually a new
  `source_id`).

Deleting the YAML row would erase that designation. Soft-inactive is the
designation model becoming operational.

Snapshots and markers are **not** rewritten by clone, detach, or attach.
Attach reuses the same `member_id`, so historical references remain connected
to the reactivated designation.

## Source Inventory

Checked against:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_init_from_sources.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_create_repo.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_materialize.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/target_selection.rs`
  (`active_members`, `require_active`, `MemberInactive`)
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GWZDesign.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/commands/repo.md`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/UserDocsPlan.md`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/GwzPyCliParityIssues.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_read.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/client.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/native/src/dispatch/`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/RepoClonePlan-ReviewF5.md`
- `/Users/owebeeone/limbo/gwz-dev/dev-docs/RepoClonePlan-Review56s.md`

## Goals

### Shared

- One plan, one release train, shared collision/identity rules.
- **`gwz-py` parity in this plan** (same rule as sibling plans /
  `GwzPyCliParityIssues.md`).
- Do not auto-commit root metadata. Continue using
  `sync_workspace_boundary` to stage `gwz.conf`; the user explicitly commits.
- Do not recommend a blanket root `gwz add -A` while a detached nested Git
  checkout remains on disk.
- Do not rewrite snapshots or markers.
- Dry-run support for all three commands.

### `repo clone`

- Clone one remote into the current workspace; register an **active** member;
  write manifest + lock; sync workspace boundary.
- Default path from `repo_name_from_url`; optional explicit member path.
- Distinct from top-level `gwz clone` (workspace root).

### `repo detach`

- Mark an existing **active** member `active: false` in `gwz.yml`.
- Drop that member from the **current lock** (lock = current composition).
- Refresh workspace boundary for the new active set and any inactive Git
  checkout that still needs protection from root Git.
- Leave the on-disk Git checkout alone (no delete).
- Leave snapshots / markers alone.
- After detach, default selection ignores the member; explicit select by id/path
  continues to fail with `MemberInactive` (existing behavior).

### `repo attach`

- Explicitly reactivate one inactive row by **member id**, preserving
  `member_id`, `source_id`, and historical lineage.
- Require an existing Git checkout at the row's recorded path in v0.
- Reject attach when that path overlaps a different active member.
- Require every snapshot/marker commit recorded for that member id to exist in
  the checkout; any missing commit rejects attach.
- Observe the checkout, refresh its manifest Git metadata, add its current
  state to the lock, and refresh the workspace boundary.
- Do not infer attach from a path or from `repo add`.

### Identity and re-adding a detached repository (frozen)

There are two different user intents:

1. **Register a repository again as a new workspace designation.** This is
   supported by `repo add` (already on disk) or `repo clone` (new clone).
2. **Reactivate the original designation with the original `member_id`.** Use
   explicit `repo attach <member-id>`, or let `repo add` use historical commit
   evidence when exactly one inactive row at that path matches.

`repo clone` and `repo create` always create a new manifest row. `repo add`
creates a new row when given a new explicit `--member-id`, but a bare
`repo add <path>` may reactivate an inactive row when GWZ can verify it from
snapshot/marker commit history. Path and remote URL alone never reactivate a
row.

Rules:

| Field | Invariant | Evidence-backed add / explicit attach | New designation, same repository | Different repository |
|-------|-----------|---------------------------------------|----------------------------------|----------------------|
| `member_id` | Unique across all rows | Reuse the inactive id | New id | New id |
| `source_id` | Logical source identity; may be shared | Reuse existing id | May explicitly reuse old id | New id |
| `path` | Active member roots must not overlap | Reactivate recorded path if available | Exact path may be reused | Exact path may be reused |

Default id rules are deterministic:

- Derive `member_id = mem_{path_slug}`. If that id is already present in any
  row, fail with an error that requires `--member-id`; do not auto-suffix.
- If inactive rows record the exact requested path, create/clone require an
  explicit new `--member-id`. Add first runs historical evidence matching; if
  it cannot identify exactly one old designation, a new explicit member id is
  required. Historical path reuse must be intentional or evidence-backed.
- If `--source-id` is absent, derive the source id from the **final member id**:
  `mem_shared_v2` → `src_shared_v2`. This produces a new source identity when
  the user supplies a new member id.
- A user re-adding the same logical Git repository may pass the historical
  `--source-id src_shared`. Source ids are intentionally not unique membership
  keys; `member_id` is the designation key.

Re-add the same checkout left on disk:

```sh
gwz repo detach mem_shared
# Explicit, deterministic inverse:
gwz repo attach mem_shared

# Or evidence-backed convenience (reactivates mem_shared only on one match):
gwz repo detach mem_shared
gwz repo add libs/shared
```

This is the normal reversible lifecycle: the original id and source lineage
are preserved.

To deliberately create a new designation for the same checkout instead:

```sh
gwz repo detach mem_shared
gwz repo add libs/shared \
  --member-id mem_shared_v2 \
  --source-id src_shared
```

The result is one inactive historical row (`mem_shared`) and one active row
(`mem_shared_v2`) at `libs/shared`, both referring to the same logical source.

Replace it with a different repository:

```sh
gwz repo detach mem_shared
mv libs/shared ../shared-old       # or remove it
gwz repo clone git@github.com:org/replacement.git libs/shared \
  --member-id mem_replacement
# source_id defaults to src_replacement
```

`repo create`/`repo clone` still require an available target. `repo add` is the
path for a detached checkout that already exists on disk.

### Historical commit evidence gate (frozen)

Remote URLs are hints, not identity. For a historical member id, collect every
non-null Git commit from:

- `snapshots/*.yaml` → `members.<member_id>.commit`; and
- `markers/*.yaml` → `members.<member_id>.commit`.

Deduplicate hashes but retain artifact provenance for errors. Add a
`GitBackend::commit_exists(repo, oid)` primitive that verifies the object exists
and peels to a commit in the candidate repository; checking a ref name is not
enough.

Rules:

- **Explicit attach:** every recorded hash for that member id must exist. Any
  missing hash fails with `SourceIdentityMismatch`; do not mutate artifacts.
- **Bare repo add at a historical path:** evaluate every inactive exact-path
  candidate. A candidate is an automatic match only when its evidence set is
  non-empty and every hash exists. Exactly one match reactivates that row;
  zero or multiple matches return an actionable error requiring explicit
  `repo attach <id>` or a new `--member-id`.
- **Reuse of an existing source id:** add/create/clone must verify all recorded
  hashes for manifest rows carrying that source id before accepting the reuse.
  Any missing hash rejects the source-id claim.
- An empty evidence set cannot drive automatic add→attach inference. Explicit
  attach/source-id reuse may proceed because the user named the identity and
  there is no contradictory recorded hash; return a warning that no historical
  commit evidence was available.
- Do not fetch automatically. A shallow or incomplete checkout fails when a
  recorded object is missing; the error lists missing hashes/artifacts and
  tells the user to fetch the history before retrying.
- An unreadable snapshot/marker fails closed; identity verification requires a
  complete view of the recorded evidence.

## Non-goals (v0)

- Hard-delete of manifest rows (`repo remove --purge` or similar).
- Deleting the member worktree (`--delete-files`).
- Rewriting snapshots/markers to drop inactive members.
- Batch multi-URL `repo clone`.
- Custom remote name / branch on clone (wire may reserve fields; execution
  rejects non-defaults like init).
- Making existing-workspace `gwz init <url>` mutate (still plan-only); optional
  follow-on can delegate to clone.
- Fixing global `gwz-py` JSON-shape debt (do not block on it).

## User-facing commands

### Clone

```text
gwz repo clone [OPTIONS] <url> [member-path]
```

```sh
gwz repo clone git@github.com:org/shared-lib.git
gwz repo clone git@github.com:org/shared-lib.git libs/shared
gwz --dry-run repo clone git@github.com:org/shared-lib.git libs/shared
# replacement after detach when mem_shared still exists inactive:
gwz repo clone git@github.com:org/other-lib.git libs/shared --member-id mem_other_lib
```

### Re-add an existing detached checkout

The identity-preserving path is `attach`:

```text
gwz repo attach <member-id>
```

```sh
gwz repo detach mem_shared
gwz repo attach mem_shared
```

Attach requires the inactive member id, not a path, so it cannot select the
wrong row after a path has accumulated multiple historical designations. It
still fails if any snapshot/marker commit recorded for that id is absent from
the checkout.

`repo add` provides evidence-backed convenience at the old path:

```text
gwz repo add [OPTIONS] <repo-path>
```

```sh
# Reactivates the one historical row only when all of its recorded commits
# exist in this checkout:
gwz repo add libs/shared

# New designation, same logical repository/source:
gwz repo add libs/shared --member-id mem_shared_v2 --source-id src_shared

# New designation and new logical source (source defaults from member id):
gwz repo add libs/shared --member-id mem_other_lib
```

If zero or multiple historical candidates match, `repo add` returns an
actionable error: attach one named historical id, or pass a new `--member-id`.
If the user supplies an old id to `repo add`, return “member id already exists;
use `gwz repo attach <id>` to reactivate it.”

### Detach

```text
gwz repo detach [OPTIONS] <member>
```

`<member>` is one literal member id (`mem_…`) or workspace-relative member
path. Selection sets/defaults/exclusions are rejected. Only active members may
be detached; an inactive id/path returns `MemberInactive` with “already
detached” detail.

```sh
gwz repo detach libs/shared
gwz repo detach mem_shared
gwz --dry-run repo detach libs/shared
```

The boundary helper already stages `gwz.conf`. Follow-up:

```sh
gwz status
gwz commit -m "Adjust workspace members"
```

Do not run a blanket root `gwz add -A` merely to record detach. A detached
nested Git checkout is protected from root Git while it remains on disk; after
moving/removing it, `gwz add` refreshes the local boundary before staging.

### Mental model

| Command | Intent |
|---------|--------|
| `gwz clone <url>` | Clone **workspace root** + materialize locked members |
| `gwz repo clone <url> [path]` | Add a **new active** member from a remote URL |
| `gwz repo add <path>` | Register an **already local** Git repo; evidence-backed reattach or explicit new designation |
| `gwz repo create <path>` | Create a **new empty** local active member |
| `gwz repo detach <member>` | Mark member **inactive**; drop from current lock |
| `gwz repo attach <member-id>` | Reactivate the same inactive designation and identity |
| `gwz init <url>…` | Create workspace **and** clone initial members |
| `gwz repo sync` | Refresh metadata for registered (active) members |

## Membership / collision design (shared Wave 0–2 work)

### Manifest invariants

`ManifestArtifact::validate` becomes authoritative for the full model:

- every row's path and ids remain syntactically valid;
- `member_id` is unique across **all** rows (active + inactive);
- `source_id` may repeat because it identifies a logical source, not a
  membership designation;
- paths for **active** rows have no equal, parent, or child overlap;
- inactive rows retain their historical path and do not participate in active
  overlap validation.

Change `reject_existing_member_path` to
`reject_existing_active_member_path_overlap` and use component-aware
`paths_collide`, not string equality. Add/create/clone and init planning use the
same invariant.

### Selection and path ownership after reuse

All path-to-member consumers use a canonical active-member view:

- selection by path returns the one active row at that path;
- if no active row exists but historical row(s) do, explicit selection returns
  `MemberInactive`;
- selection by old member id returns `MemberInactive`;
- `gwz add` repository routing and fan-out use active paths only;
- diff pathspec routing and explicit member lookup use active paths only;
- status/ls/default selection continue to use active rows only.

This is required for a manifest containing inactive `mem_shared` and active
`mem_shared_v2` at the same path. The former “Selection unchanged” ruling is
withdrawn.

### Lock on detach

```text
detach repo member
  writes gwz.conf/gwz.yml          # active: false on that member
  writes gwz.conf/gwz.lock.yml     # remove that member_id from lock.members
  sync_workspace_boundary          # refresh physical nested-repo protection
```

Rationale: lock is the **current** resolved composition. Snapshots retain
historical member sets. Leaving a lock entry for an inactive member would
confuse materialize/status defaults.

### Disk on detach

Do **not** delete or move the checkout. If the user wants to reuse the path
for a new clone, they relocate or remove the directory first; clone preflight
still requires an empty/available target via `ensure_member_target_available`.

### Boundary / `.git/info/exclude` on detach (safety ruling)

The lock remains the current **composition**, but the local exclude block is a
**physical nested-repository safety boundary**, not a second composition file.
Do not expose a retained nested Git checkout to root `git add` merely because
it was detached.

Change `sync_workspace_boundary` / `ensure_workspace_exclude` to receive
backend + manifest + lock and generate the managed block from the union of:

1. lock member paths (including a recoverable stale-lock window);
2. all active manifest member paths; and
3. inactive manifest paths that still contain a Git repository on disk.

Consequences:

- Detach removes the member from the lock and from command selection, but its
  checkout remains excluded from root Git while it exists.
- Attaching that designation promotes the same row back to active and restores
  its lock entry; the physical exclude line remains continuously in place.
- Re-adding that checkout with `repo add` keeps the same physical exclude line
  and creates a new active member row/lock entry.
- After the user moves/removes the inactive checkout, the next boundary refresh
  drops the obsolete line. `gwz add` refreshes the boundary before root staging
  so a later ordinary directory at that path is not hidden indefinitely.
- Root-wide staging must never create a `160000` gitlink for a retained
  detached checkout or stage inside it as though it were active.

Selection/ownership and physical exclusion are deliberately different:
inactive rows are not operation targets, but an inactive Git checkout remains
protected from accidental root absorption.

Update every existing `sync_workspace_boundary` caller to pass the manifest.
Before root staging, `handle_stage` calls the exclude-only refresh (not the
metadata-staging wrapper) using the current manifest/lock/disk state.

## Protocol design

Add three first-class core operations (do not overload init / add-existing).

### Proposed Taut additions (`gwz.taut.py`)

```python
method("clone_repo_member", role="in",
       params=Params(request=Ref.CloneRepoMemberRequest),
       out=Ref.CloneRepoMemberResponse),
method("detach_repo_member", role="in",
       params=Params(request=Ref.DetachRepoMemberRequest),
       out=Ref.DetachRepoMemberResponse),
method("attach_repo_member", role="in",
       params=Params(request=Ref.AttachRepoMemberRequest),
       out=Ref.AttachRepoMemberResponse),
```

Method and action names are frozen. Values 22-24 follow
`diff=21`:

```python
clone_repo_member=22,
detach_repo_member=23,
attach_repo_member=24,
```

```python
CloneRepoMemberRequest=Msg(
    meta=F(1, Ref.RequestMeta),
    # Canonical clone-source shape: url + optional path/remote/branch.
    source=F(2, Ref.SourceUrl),
    member_id=F(3, STR, optional=True),
    source_id=F(4, STR, optional=True)),

CloneRepoMemberResponse=Msg(
    response=F(1, Ref.ResponseEnvelope)),

# Soft-remove a member from current workspace composition (active → false).
# Target the member via RequestMeta.selection (same as repo sync / status /
# materialize): clients put one opaque selector token in selection.targets
# (member id or workspace-relative path). Core resolves it; clients must not
# look up or rewrite the token. Exactly one concrete member target required;
# @all / @default / multi-target / empty selection are InvalidRequest.
DetachRepoMemberRequest=Msg(
    meta=F(1, Ref.RequestMeta)),

DetachRepoMemberResponse=Msg(
    response=F(1, Ref.ResponseEnvelope)),

# Reactivate one historical designation. The selector must contain exactly one
# literal member id; paths and @-selectors are invalid.
AttachRepoMemberRequest=Msg(
    meta=F(1, Ref.RequestMeta)),

AttachRepoMemberResponse=Msg(
    response=F(1, Ref.ResponseEnvelope)),
```

**How this compares to other member targeting**

| Layer | Behavior |
|-------|----------|
| **Resolution** | A detach-specific raw validator accepts exactly one literal member id/path, then the normal active-member resolver resolves it. Clients do not pre-resolve. |
| **Wire** | Same as `RepoSyncRequest`, `StatusRequest`, `MaterializeRequest`, etc.: selection lives on **`RequestMeta.selection`**, not a dedicated `member=` field on the op request. Preferred field: `selection.targets` (legacy `member_ids` / `paths` still accepted by the resolver during migration). |
| **CLI** | Same pattern as `gwz repo sync [member-path]`: positional `<member>` → `meta.selection.targets = [token]`; reject it when `meta.selection.is_some()`, covering every global include/exclude/all selector. |

So the earlier draft `member=F(2, STR)` was **not** how other targeting works.
Detach follows `repo sync` at the wire level but has stricter validation.

Before normal expansion, `validate_single_detach_selector` must:

- require a `Selection`;
- reject `all == true`, exclusions, empty include, and multiple includes;
- combine `targets`, legacy `member_ids`, and legacy `paths` and require exactly
  one total token;
- reject all `@...` tokens, including `@default`, `@all`, and `@root`;
- pass the one literal token to active member id/path resolution.

This prevents an omitted/default selector from detaching the only member in a
one-member workspace.

Append `PlannedAction.detach_member=15`. `MemberResponse.planned` contains one
`PlannedChange`, so manifest+lock effects are described by this single action's
message; they are not represented as two composed actions.

Append `PlannedAction.attach_member=16`. Attach uses a sibling raw selector
validator that requires exactly one literal `mem_...` id. It rejects paths
because inactive path history can be ambiguous. An already-active id returns a
successful Noop; an unknown id returns `MemberNotFound`.

Append `GwzErrorCode.source_identity_mismatch=36`. Use it when a candidate
repository lacks any commit required by snapshot/marker identity evidence.
Include the member/source ids and missing hash provenance in error detail.

## Core design

### `handle_clone_repo_member`

```text
1. Resolve workspace root; assert request workspace id.
2. For mutation, acquire WorkspaceMutatorLock before reading artifacts and hold
   it through clone, artifact publication, and boundary sync.
3. Read manifest + lock; build a single-source plan from request.source.
4. Derive/validate path and final member id; reject global member-id collision.
5. Derive source id from the final member id unless explicitly supplied.
6. Reject overlap with active member paths; preflight target and v0
   remote/branch limits.
7. dry_run → Accepted + planned Clone (no lock file or artifact mutation).
8. clone_repo_with_progress; observe head/status/remotes.
9. Build active ManifestMember + lock entry (desired_from_head).
10. write_manifest_and_lock; sync_workspace_boundary(manifest, lock).
```

Create a `single_source_plan` helper that accepts optional ids. Do not call
`init_source_plans` unchanged: it accepts only `SourceUrl` and discards the
request's id overrides. Reuse its path/URL validation and the init clone loop,
plus add/create registration helpers. Do **not** call
`handle_init_from_sources` wholesale (existing-manifest plan-only stub).

### `handle_detach_repo_member`

```text
1. Resolve workspace root; validate the raw selector shape.
2. For mutation, acquire WorkspaceMutatorLock before reading artifacts and hold
   it through artifact publication and boundary sync.
3. Read manifest + lock; assert workspace id; resolve exactly one active row.
4. Unknown → MemberNotFound; inactive id/path → MemberInactive with
   “already detached” detail.
5. dry_run → Accepted + PlannedAction::DetachMember; no artifact mutation.
6. Set member.active = false (keep path, remotes, desired, ids).
7. Remove member_id from lock.members.
8. write_manifest_and_lock.
9. sync_workspace_boundary(manifest, lock). If the checkout remains a Git repo,
   its physical exclude line remains for root safety.
10. Return one MemberResponse with status Ok, the detached identity/path,
    state=None, lock_match=Missing, and an envelope message that the checkout
    was retained. The standard response does not claim to encode active=false.
```

Do **not** require the worktree to be clean in v0 (warn in docs). Optional
`--force` later if we add dirty checks.

### `handle_add_existing_repo` re-add extension

After resolving the repository and rejecting active path overlap:

```text
1. If request.member_id is a new explicit id, plan a new designation.
2. If request.member_id names an existing inactive row, reject with directions
   to use repo attach; add does not overload an explicit existing id.
3. If member_id is absent, find inactive rows at the exact member path.
4. For each candidate, collect snapshot/marker commit evidence and verify the
   objects in the repository.
5. Exactly one candidate with non-empty, fully satisfied evidence → execute the
   common attach plan for that row, while retaining ActionKind::AddExistingRepo.
6. Zero or multiple matches → InvalidRequest with candidate/missing-evidence
   detail and the attach-or-new-member-id choices.
7. With no historical candidates, use normal new-member add behavior.
```

Dry-run of an evidence-backed add returns `PlannedAction::AttachMember` and the
historical member id. Execution reuses the attach mutation helper so add and
attach cannot diverge on identity checks, metadata refresh, lock state, or
boundary behavior. Its response explicitly says “reattached `<member_id>`” and
reports the verified historical commit count; it must not look like a newly
allocated member.

### `handle_attach_repo_member`

```text
1. Resolve workspace root; validate exactly one literal mem_... selector.
2. For mutation, acquire WorkspaceMutatorLock before reading artifacts and hold
   it through artifact publication and boundary sync.
3. Read manifest + lock; assert workspace id; find the row by globally unique
   member id without applying require_active.
4. Unknown → MemberNotFound; already active → successful Noop.
5. Reject a non-Git source or overlap with any other active member path.
6. Require an existing Git repository at root/member.path.
7. Collect every snapshot/marker commit for the member id; require every object
   to exist and peel to a commit in the checkout.
8. Observe head/status/remotes and build the reactivated row + lock state.
9. dry_run → Accepted + PlannedAction::AttachMember; artifacts unchanged.
10. Set active=true; retain member_id/source_id; refresh desired/remotes from the
   observed checkout; insert the observed state into lock.members.
11. write_manifest_and_lock; sync_workspace_boundary(manifest, lock).
12. Return one Ok MemberResponse with the observed state and lock_match=Matches;
    the envelope message reports the number of historical commits verified (or
    warns that none were recorded).
```

The explicit historical member id selects the designation; snapshot/marker
commit presence verifies it superficially. Remote URLs are not used as proof
because they can be absent, renamed, or changed. Any missing recorded commit is
`SourceIdentityMismatch`, even for explicit attach. Attach does not rewrite
snapshots or markers. If the checkout is absent, v0 fails with instructions to
restore it; attach-and-materialize is a follow-on. A dirty checkout is allowed
and recorded honestly in the new lock state.

### Historical identity helpers

| Helper | Contract |
|--------|----------|
| `historical_member_commits(root, member_ids)` | Read all snapshots/markers, extract and deduplicate `members.<id>.commit`, retain provenance, fail closed on unreadable artifacts |
| `GitBackend::commit_exists(repo, oid)` | True only when the object exists locally and peels to a commit |
| `verify_historical_identity(repo, evidence)` | Check every hash; return count on success or SourceIdentityMismatch with all missing hashes |

When add/create/clone proposes a `source_id` already present in the manifest,
collect evidence for every row carrying that source id and run the same gate.
For clone, verification occurs after cloning but before artifact publication;
failure removes the fresh target under the normal pre-publication rollback
rule. A default source id that collides with an existing logical source must be
confirmed with explicit `--source-id` rather than silently treated as reuse.

### Shared preflight edits (required across the lifecycle)

| Helper | Change |
|--------|--------|
| `ManifestArtifact::validate` | Unique member ids across all rows; active-only path-overlap validation |
| `reject_existing_member_path` | Replace with component-aware active path-overlap rejection |
| `init_source_plans` / path set validation | Validate existing active paths + all new plans |
| `single_source_plan` | Preserve explicit ids; default source id from final member id |
| path selection | Prefer the active row; historical-only path → MemberInactive |
| stage/diff routing | Build ownership from active paths only |
| `validate_single_detach_selector` | Exactly one literal id/path; reject defaults/sets/excludes |
| `validate_single_attach_selector` | Exactly one literal member id; reject paths/defaults/sets/excludes |
| historical identity helpers | Verify all snapshot/marker commits before attach or source-id reuse |
| boundary helper | Active paths + materialized inactive Git checkouts |

Add/create must use the same active-only path rule so detach → add at same
path works without a special clone-only backdoor. Expose `--member-id` and
`--source-id` on add/create in the same release; this is required, not optional.
When add sees inactive history at the exact path and lacks a new explicit id,
its error presents the two intentional choices: attach an old id or add a new
designation with `--member-id`.

### Mutation serialization

Every command that writes the manifest or lock must participate in the same
`WorkspaceMutatorLock` discipline. Audit and convert add, create, repo sync,
clone-member, detach-member, attach-member, capture, materialize/pull, branch,
commit, and any other artifact writer. Otherwise a long clone can publish stale
whole-file copies and erase a concurrent detach or lock update.

v0 uses the simple policy: mutating clone holds the lock across the network
operation and competing mutations fail fast with the existing lock-contention
error. Read-only commands remain available. Dry-run reads a point-in-time view
without acquiring/creating the runtime lock file.

### Failure/publication contract

| Failure point | Required state |
|---------------|----------------|
| clone or post-clone Git inspection, before artifact publication | Remove the fresh target; artifacts unchanged |
| artifact serialization/staging before publication | Remove the fresh target; artifacts unchanged |
| manifest/lock publication | Preserve a recoverable state; never blindly delete a target that may now be registered |
| boundary sync after artifact publication | Membership mutation remains committed; return a repairable boundary/staging error |

Apply the last row to detach and attach as well. The next boundary-writing
operation or `gwz add` boundary refresh repairs the local projection.

## CLI design

### Clap (`clirequest.rs`)

```rust
RepoCommandArgs::Clone(RepoCloneArgs { url, member_path, member_id, source_id })
RepoCommandArgs::Detach(RepoDetachArgs { member })
RepoCommandArgs::Attach(RepoAttachArgs { member_id })
```

Expose optional `--member-id` / `--source-id` on clone (flags, not positionals)
so replacement after detach is possible when the path slug is taken.

Expose the same flags on `repo add` and `repo create`. `repo add` is the
supported way to re-add the checkout left behind by detach: bare add may
perform evidence-backed reactivation, while an explicit new member id creates a
new designation.

Detach: one positional `<member>`. Map like `repo sync`:

```rust
meta.selection = Some(Selection { targets: vec![member], ..Default::default() });
// reject when meta.selection.is_some(): all global include/exclude/all forms
```

Attach maps its one positional member id the same way, rejects other global
selection, and validates the `mem_` syntax client-side for early usage errors.

Update `REPO_LONG` / after-help for all three. Regenerate `CLI.md`.

### Dispatch

Wire all three next to existing repo handlers; clone emits progress events;
detach/attach are quick metadata mutations (operation started/finished events
are optional but cheap).

## `gwz-py` parity (same plan)

Extend the existing `repo` surface in parallel for all three subcommands:

| Layer | Clone | Detach | Attach |
|-------|-------|--------|--------|
| Protocol regen | yes | yes | yes |
| Native direct call | backend/event-capable route | quick route | backend quick route |
| Native submit | accepted/background/event route | not required | not required |
| `client.py` | direct + `clone_repo_member_stream` | `detach_repo_member` | `attach_repo_member` |
| `cli_read.py` | nested `clone` | nested `detach` | nested `attach` |
| CLI execution | JSON/JSONL/dry-run direct; mutating human output streams | direct | direct |
| Tests | parser + direct + submit/stream | parser + direct | parser + direct |
| `GwzPyCliParityIssues.md` | inventory all three repo lifecycle commands | same | same |

Parity bar: same argv/defaults/core request fields. Global JSON-shape debt
does not block shipping the subcommands.

## Implementation waves (parallel tracks)

Clone, detach, and attach share Wave 0–1 and the collision-rule edits in Wave 2.
Handler/CLI/`gwz-py` work can proceed in parallel after protocol lands.

### Wave 0 — Spec freeze (complete in this revision)

- Inactive-not-delete; clone/create never reactivate; attach is explicit and
  bare add may reactivate only from one non-empty fully verified evidence set.
- Active-path overlap only; member id globally unique; source id shareable.
- Lock entry removed on detach; materialized inactive checkout remains excluded
  from root Git as a physical safety boundary.
- Derived member-id collision requires explicit `--member-id`; source id
  defaults from the final member id.
- Methods/actions: `clone_repo_member=22`, `detach_repo_member=23`,
  `attach_repo_member=24`; planned detach/attach actions are 15/16.
- Exactly-one detach/attach selectors, mutation locking, response shapes, and
  failure contract as above.
- Historical commit evidence is the identity gate for attach, evidence-backed
  add, and reuse of an existing source id.

### Wave 1 — Protocol

- All three methods, ActionKinds, request/response messages, planned
  detach/attach actions, and `SourceIdentityMismatch=36`.
- Regen Rust + `gwz-py` protocol and golden corpus.
- Update the internal ActionKind/PlannedAction/ErrorCode mirrors and
  conversions, `OperationRequest::context`, events, dispatch allow-lists, and
  CLI progress label.

### Wave 2 — Core (shared rules + all three handlers)

- Manifest invariant migration; active path selection; stage/diff active-path
  routing; physical boundary projection.
- Active-only path collision helpers; update add/create/init call sites + tests.
- Expose/consume add/create member/source ids and shared default-id helper.
- Historical evidence collection + commit-existence backend primitive; common
  attach plan reused by explicit attach and evidence-backed repo add.
- Workspace mutation lock audit/conversion for every manifest/lock writer.
- `handle_clone_repo_member` + unit tests.
- `handle_detach_repo_member` + unit tests.
- `handle_attach_repo_member` + unit tests.
- Cross-test: detach → clone/add same path with new member id succeeds;
  same path without new id fails clearly; selection resolves the active row;
  detach → attach preserves the old id; snapshots untouched; no root gitlink.

### Wave 3 — Rust CLI (parallel)

- Clap + dispatch + help for `repo clone`, `repo detach`, and `repo attach`; add
  `--member-id`/`--source-id` to repo add/create.
- Parse tests + `local_workflows` smokes for clone, detach, re-add, and
  replacement.

### Wave 4 — `gwz-py` (parallel)

- Direct + submit dispatch for clone, direct dispatch for detach/attach, client
  direct + stream methods, nested CLI for all three.
- Parser/direct/stream/dry-run smoke tests.
- Update `GwzPyCliParityIssues.md` `repo` row.

### Wave 5 — Docs

- `repo.md`, Workflows / StartHere grow+shrink, `init.md` pointer,
  `GWZDesign.md` write policy + identity/active/source/evidence/boundary rules,
  regenerate `CLI.md`, `UserDocsPlan.md` bullets.

### Wave 6 — Optional follow-ons

- `repo attach --materialize` when the historical checkout is absent.
- Existing-workspace `init <url>` delegates to clone.
- Multi-URL clone / path-prefix.
- Branch/remote selection.
- Hard purge / worktree delete flags.
- Dirty-worktree warnings on detach.

## Tests

### Core — clone

1. Happy path into existing workspace (manifest/lock/remotes/boundary).
2. Default path from URL; explicit path.
3. Active exact/parent/child path collision; inactive historical overlap does
   not block after disk preflight is satisfied.
4. Global member-id collision; explicit ids reach manifest+lock; default
   source id derives from final member id.
5. Reusing an existing source id verifies every historical snapshot/marker
   commit; a missing commit rolls back the fresh clone.
6. Dry-run: no target/artifact/runtime-lock mutation.
7. Clone/Git-inspection failure rollback; publication and boundary failure
   states match the failure contract.
8. Unsupported branch/remote if fields set.

### Core — detach

1. Detach by path and by id → `active: false`; lock entry removed; checkout
   still on disk and its managed exclude line retained while it remains a Git
   repository.
2. Dry-run: no manifest/lock mutation.
3. Omitted selection, `@default`, `@all`, `@root`, exclusion, and multi-target
   are invalid even in a one-member workspace.
4. Unknown member → `MemberNotFound`; already inactive → `MemberInactive` with
   clear “already detached” detail.
5. Success response has identity/path, Ok, state absent, lock match Missing,
   and checkout-retained message; dry-run uses DetachMember.
6. Snapshots/markers directories unchanged (fixture with a snapshot that still
   names the member).
7. After detach, default `status`/`ls` omit member; explicit old id/path returns
   `MemberInactive` until a new active row takes the path.
8. After moving/removing the checkout, a boundary refresh drops its exclude;
   root staging never creates a gitlink for the retained checkout.

### Core — re-add and replacement (the designation story)

1. Detach `libs/shared` (`mem_shared` inactive, path retained on row).
2. `repo attach mem_shared` reactivates the original row and lock identity.
3. Detach again; bare `repo add libs/shared` with one non-empty fully satisfied
   evidence set reactivates `mem_shared` and reports AttachMember.
4. Missing evidence hash, zero verified candidates, or multiple verified
   candidates prevents inference and returns attach-or-new-designation choices.
5. Deliberately re-add the retained checkout with `--member-id mem_shared_v2 --source-id
   src_shared` → old row inactive, new row active, shared logical source, lock
   contains only the new member id.
6. Re-add with only `--member-id mem_shared_v3` → source defaults to
   `src_shared_v3`.
7. Move/remove the checkout; clone a different repository with
   `--member-id mem_other` → source defaults to `src_other`.
8. Path selection resolves the new active row; old id returns
   `MemberInactive`; repeated historical rows at the same path do not make the
   active path ambiguous.
9. `repo create` and `repo add` use the same active overlap/id rules.

### Core — attach

1. Detach → attach verifies all snapshot/marker commits, preserves
   member/source ids, sets active=true, observes the
   retained checkout, refreshes desired/remotes, inserts the current lock row,
   and leaves snapshots/markers untouched.
2. Attach dry-run returns AttachMember and changes no artifacts.
3. Already active → Noop; unknown id → MemberNotFound.
4. Path selector, defaults, sets, exclusions, and multi-target are invalid;
   attach requires one literal member id.
5. Missing/non-Git checkout, any missing historical commit, and path overlap
   with a newer active designation are rejected without mutation.
6. Explicit old id selects the historical row; commit evidence verifies it.
   Remote URL differences alone do not silently select or reject a row.
7. Empty evidence is allowed only for explicit attach, with a warning; it never
   drives automatic add→attach inference.

### Core — historical identity evidence

1. Collect commits from both snapshot and marker member maps; ignore null
   commits, deduplicate hashes, and retain every artifact provenance label.
2. `commit_exists` rejects a missing object and an object that does not peel to
   a commit.
3. One missing hash among many returns `SourceIdentityMismatch` with no
   manifest/lock mutation and identifies the missing hash/artifact(s).
4. A shallow checkout missing old objects fails and succeeds after the history
   is fetched; GWZ performs no automatic network access.
5. An unreadable snapshot or marker fails identity verification closed.
6. Bare add auto-reactivates only one non-empty verified candidate; zero and
   multiple matches are actionable ambiguity/no-match errors.
7. Reusing an existing source id checks evidence for every manifest row with
   that source id.

### Core — serialization

1. Clone vs detach/add/repo-sync contention cannot lose a manifest update.
2. Clone vs lock writer contention cannot lose a lock update.
3. Every manifest/lock writer participates in `WorkspaceMutatorLock`.

### CLI / `gwz-py` / integration

- Parse matrices for all three subcommands.
- Direct and streamed clone parity; direct dry-run parity.
- Attach and evidence-backed add expose verified-count/warning/mismatch results
  consistently on Rust and Python surfaces.
- End-to-end: init → clone → detach → attach original designation → detach →
  deliberate new designation → detach → replacement clone →
  status/ls/diff/add/commit metadata.
- Commit the staged root metadata without a blanket `gwz add -A`; assert no
  root gitlink.

## Documentation snippets (target copy)

````markdown
## `gwz repo clone`

Clone a remote Git repository into the current workspace and register it as an
active member (manifest + lock).

## `gwz repo detach`

Stop managing a member in the current workspace composition. GWZ sets
`active: false` in `gwz.yml`, removes the member from `gwz.lock.yml`, and
leaves snapshots, markers, and the on-disk repository alone. A retained nested
Git checkout stays locally excluded from the root repository for safety.

## `gwz repo attach`

Reactivate the original inactive designation, preserving its member and source
identity. Attach requires the retained checkout and the historical member id.
Every snapshot/marker commit recorded for that id must exist in the checkout;
otherwise attach fails:

```sh
gwz repo detach mem_shared
gwz repo attach mem_shared
```

At the historical path, bare `repo add` can perform the same reactivation when
exactly one inactive row has non-empty commit evidence and every object exists:

```sh
gwz repo add libs/shared
```

To deliberately create a new designation for the same logical repository,
reuse its source id but choose a new member id:

```sh
gwz repo detach mem_shared
gwz repo add libs/shared \
  --member-id mem_shared_v2 \
  --source-id src_shared
```

To put a different repository at the same path, detach the old member, then
move/remove its checkout and clone or add with a **new** member id:

```sh
gwz repo detach mem_shared_v2
mv libs/shared ../shared-old
gwz repo clone git@github.com:org/replacement.git libs/shared --member-id mem_replacement
```

Clone/create never reactivate an inactive `member_id`. Bare add may reactivate
only from one unambiguous, fully verified historical commit set; explicit
`repo attach` selects the old designation directly and applies the same hash
verification.
````

## Success criteria

- `repo clone`, `repo detach`, and `repo attach` ship together on Rust `gwz`
  and `gwz-py`.
- Detach is soft-inactive, not row deletion; snapshots/markers untouched.
- Active member roots do not overlap; historical paths may repeat;
  `member_id` remains globally unique and `source_id` is shareable.
- Identity-preserving attach, deliberate re-add as a new designation, and
  replacement at the same path all work end-to-end.
- Attach and evidence-backed add verify every snapshot/marker commit and reject
  missing objects with `SourceIdentityMismatch`.
- Path selection/routing resolves the active row after historical reuse.
- Retained detached Git checkouts cannot be accidentally staged as root
  gitlinks.
- Concurrent manifest/lock mutations cannot overwrite each other.
- Docs make `gwz clone` vs `gwz repo clone` vs `gwz repo detach` vs
  `gwz repo attach` unambiguous.
- `GwzPyCliParityIssues.md` inventories all three subcommands.

## Resolved design questions

1. Methods/actions are `clone_repo_member=22`, `detach_repo_member=23`, and
   `attach_repo_member=24`.
2. Derived member-id collisions require explicit `--member-id`; no auto-suffix.
3. Source id defaults from the final member id and may be explicitly shared for
   the same logical source.
4. Detach requires one literal selector and returns `MemberInactive` for an
   already inactive designation.
5. Detach removes the lock row. Physical exclusion remains only while a nested
   Git checkout exists.
6. Add/create expose member/source id flags in this release.
7. Python provides both direct and stream clone paths; dry-run is direct.
8. Identity-preserving `repo attach` is in v0 and requires an existing checkout;
   attach-with-materialization and existing-workspace init remain Wave 6
   follow-ons.
9. Snapshot/marker member commits are the local source-identity evidence.
   Missing objects fail attach/source-id reuse; one non-empty unambiguous match
   permits bare repo add to reactivate the old row.
