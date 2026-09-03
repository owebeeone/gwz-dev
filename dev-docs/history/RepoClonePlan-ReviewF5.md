# Review: `RepoClonePlan.md` (F5)

Reviewed: 2026-07-10. Subject: `dev-docs/RepoClonePlan.md` (revision: parallel
`repo clone` / `repo detach`, inactive membership model).

Review method: every checkable claim in the plan was verified against the
current source (protocol, core handlers, CLI, `gwz-py`). Findings are ordered
by weight: **F1–F3 should be resolved before Wave 1**, the rest are nits,
confirmations, or answers to the plan's own open questions.

## Verdict

The plan is sound and well-grounded. The inactive-not-delete model matches the
existing design (manifest `active: bool`, `active_members` filtering,
`MemberInactive`), the collision-rule diagnosis is accurate, the
lock/boundary reasoning is correct, and the selection-on-`RequestMeta` wire
design for detach matches how every other member-scoped op works. The
fact-check found no wrong load-bearing claims — but it did find one real
protocol-design gap (F1), one under-specified invariant (F2), and one
`gwz-py` routing error (F3).

## Fact-check: claims verified as correct

- **ActionKind values free.** Highest is `diff=21`
  (`gwz-core/protocol/gwz.taut.py:138`); 22/23 are unused. ✓
- **Selection wire shape.** `RequestMeta.selection` exists
  (`gwz.taut.py:560`); `Selection` has `targets` (F4) plus legacy
  `member_ids`/`paths` (`gwz.taut.py:517-528`). `RepoSyncRequest` is
  meta-only (`gwz.taut.py:985-986`); Status/Materialize also target via
  `meta.selection`. The plan's rejection of a dedicated `member=` field is
  right. ✓
- **Add/create already carry optional ids on the wire.**
  `AddExistingRepoRequest` and `CreateRepoRequest` both have optional
  `member_id`/`source_id` (`gwz.taut.py:963-982`). Exposing `--member-id` /
  `--source-id` on the CLI is plumbing, not protocol work. ✓
- **`reject_existing_member_path` rejects inactive paths today.** It scans all
  `manifest.members` with no `active` filter
  (`handle_create_repo.rs:535-551`), called from both create (line 88) and
  add-existing (line 180). The plan's required change is real. ✓
- **`reject_duplicate_member_id` checks all rows** regardless of `active`
  (`handle_create_repo.rs:553-565`) — consistent with the plan's "global id
  uniqueness, unchanged". ✓
- **Boundary is lock-driven.** `ensure_workspace_exclude` regenerates the
  managed block from `lock.members.values()` paths only
  (`sync_workspace_boundary.rs:31-59`; markers at lines 11-12 match the
  plan's quoted `# BEGIN/END GWZ managed member repositories`). Dropping the
  lock entry before `sync_workspace_boundary` removes the exclude line
  exactly as the plan describes. ✓
- **Existing-workspace `gwz init <url>` is plan-only.** With an existing
  manifest, `handle_init_from_sources` returns `Accepted` + planned members
  and performs no clone/write (`handle_init_from_sources.rs:42-60`). ✓
- **Clone-failure rollback exists.** Init precomputes `fresh_clone_paths` and
  `remove_dir_all`s them on first error, writing nothing
  (`handle_init_from_sources.rs:104-109, 191-199`). The clone handler's step
  6 has a direct precedent. ✓
- **`preflight_init_execution_targets` exists**
  (`handle_init_from_sources.rs:339`), as do all other named helpers
  (`read_lock_or_empty`, `write_manifest_and_lock`, `desired_from_head`,
  `ensure_member_target_available`, `resolved_member`, `path_slug`,
  `repo_name_from_url`, `clone_repo_with_progress`). ✓
- **CLI mapping precedent.** `repo sync` maps its positional into a fresh
  `Selection { targets: vec![member_path] }` and errors if `meta.selection`
  is already set (`gwz-cli/src/clirequest.rs:577-592`). Copyable as-is for
  detach. ✓
- **`GwzPyCliParityIssues.md` already tracks this plan.** The `repo` row
  (line 286) names planned `clone` + `detach` and same-release parity. ✓
- **CLI.md is generated and freshness-tested.**
  `scripts/generate_cli_reference.py --write`, enforced by
  `gwz-cli/src/tests/g00.rs:29-37`. Wave 5's "regenerate CLI.md" is a hard
  requirement, not a courtesy. ✓

## Findings

### F1 — Planned-action design doesn't match the protocol's actual shape (fix in Wave 0/1)

The plan says: "reuse `PlannedAction` where possible … otherwise compose
`WriteManifest` + `WriteLock`". Two corrections:

1. `PlannedAction` is an **enum**, not a family of messages
   (`gwz.taut.py:276-291`): `noop=0 … reset=14`, including `write_manifest=7`
   and `write_lock=8`. Next free value is **15**.
2. "Composing" two planned actions per member is **not representable**: the
   dry-run surface is `MemberResponse.planned = F(6, Ref.PlannedChange,
   optional=True)` (`gwz.taut.py:883`) — a single optional `PlannedChange`
   per member, and `PlannedChange` carries exactly one `action`
   (`gwz.taut.py:865-872`).

So the choice isn't "distinct verb vs compose" — it's "distinct verb vs
repurpose an existing one". Recommendation: add `detach_member=15` (one verb,
one `PlannedChange`, honest JSON output). Decide in Wave 0 alongside the
ActionKind confirmation; do not plan on multi-action composition without also
widening `MemberResponse.planned` to a list, which is a bigger change than
this plan needs.

### F2 — "Active-only path uniqueness" must also cover prefix/nesting collisions (spec gap)

Path collision in this codebase is not string equality.
`validate_member_path_set` uses `paths_collide`
(`gwz-core/src/workspace/mod.rs:63-75, 146-153`), which rejects **exact match
OR one path being a `/`-delimited prefix of the other** (no nested members).
The plan's collision table and rules only discuss reusing "the same path".

When path checks become active-only, decide explicitly: does an **inactive**
row's path still block a **nested or parent** path for a new active member?
The consistent answer is no (inactive rows drop out of all path collision,
equality and nesting alike — "at most one active member per path *subtree*"),
but the plan should say so, and Wave 2's cross-tests should add:

- detach `libs/shared` → `repo clone <url> libs/shared/sub` succeeds;
- detach `libs/shared` → `repo clone <url> libs` succeeds (parent);
- both fail while `mem_shared` is still active.

Also note the call-site shape: `handle_create_repo`/`handle_add_existing_repo`
build the path set from **all** `manifest.members`
(`handle_create_repo.rs:125-129`, `handle_add_existing_repo.rs:209-213`), and
`init_source_plans` does the same (`handle_init_from_sources.rs:262-318`). The
active filter belongs where those slices are built, not only inside
`reject_existing_member_path`.

### F3 — `gwz-py` dispatch routing for clone is wrong in the parity table

The plan routes `clone_repo_member` through `dispatch/read.rs` + allow-list.
But `read.rs` is the **single-shot** path (`dispatch/mod.rs:28-44` routes
`create_workspace`, `add_existing_repo`, `create_repo`, `repo_sync`, `status`,
`ls`, … there), while long-running clone-type ops go through the **streaming
`submit`** path (`dispatch/mod.rs:47-70`: `init_from_sources`, `materialize`,
`clone_workspace`, `pull_head`, `pull_snapshot`, `push`), which is what feeds
`Client._stream_call` and the progress rendering `gwz clone` already has
(`cli_local.py:99-143`).

`clone_repo_member` should follow `clone_workspace`: submit/streaming route,
`clone_repo_member_stream` on the client, progress rendered in the nested
`repo clone` CLI. `detach_repo_member` in `read.rs` is fine (it sits next to
other quick mutations like `create_repo`). This also mostly answers open
question 6 — see below.

### F4 — Nit: `Ref.Response` doesn't exist

Every response wrapper in the protocol is
`response=F(1, Ref.ResponseEnvelope)` (e.g. `CloneWorkspaceResponse`,
`gwz.taut.py:1159-1160`; `RepoSyncResponse`, `gwz.taut.py:1168-1169`). The
plan's `CloneRepoMemberResponse` / `DetachRepoMemberResponse` sketches say
`Ref.Response`. Cosmetic in a plan, but fix it before Wave 1 copies it into
the Taut source.

### F5 — Nit: method naming — neither open-question option matches convention

Existing verbs: `add_existing_repo`, `create_repo`, `repo_sync`,
`clone_workspace`, `create_workspace`, `init_from_sources`
(`gwz.taut.py:9-112`). The plan's open question 1 offers `clone_repo_member`
vs `repo_clone`; the closest existing family is arguably `clone_repo` /
`detach_repo` (matching `create_repo`), though `repo_sync` argues for the
`repo_*` prefix. No strong recommendation here beyond: pick from the actual
inventory in Wave 0, not from the two options as listed — and note the
existing `ActionKind` names will constrain it (`create_repo=3`,
`repo_sync=16` — the enum is already inconsistent, so match the request
messages instead: `AddExistingRepoRequest`/`CreateRepoRequest` suggest
`CloneRepoRequest`/`DetachRepoRequest` + `clone_repo`/`detach_repo`).

### F6 — Suggestion: consider `SourceUrl` instead of four flat fields on the clone request

`SourceUrl` already models exactly the proposed payload: `url`, optional
`path`, optional `remote_name`, optional `branch` (`gwz.taut.py:613-620`),
and it's what `init_source_plans` consumes. A
`CloneRepoMemberRequest{ meta, source=SourceUrl, member_id?, source_id? }`
reuses the message, the plan-building code, and the same v0
reject-non-default-remote/branch validation as init, instead of duplicating
the four fields flat. Not a defect — the flat shape works — but the reuse is
nearly free and keeps one canonical "clone one source" shape.

### F7 — Note: `init_source_plans` is a loose fit for single-member clone

Core step 3 leans on `init_source_plans`, which is multi-source and derives
ids internally with its own duplicate checks
(`handle_init_from_sources.rs:262-318`). For a single member, the
add/create registration sequence in `handle_create_repo.rs` is the tighter
reuse: `resolve_workspace_root` → `assert_workspace_id` →
active-aware path rejection → `path_slug`/`repo_name_from_url` id derivation →
`reject_duplicate_member_id` → `ensure_member_target_available` → clone →
`desired_from_head` → `resolved_member` → `write_manifest_and_lock` →
`sync_workspace_boundary`. Either works; whichever is chosen, make sure the
plan's optional explicit `member_id`/`source_id` override the derived ones the
same way add/create already honor their optional wire fields.

### F8 — Note: the "already detached" resolution happens inside the resolver, not the handler

Detach handler steps 4–5 read as if the handler loads the row and checks
`active` itself. In reality `find_member_by_id` / `find_member_by_path` call
`require_active` internally (`target_selection.rs:174-193`), so an inactive
target never reaches the handler — the resolver returns `MemberInactive`
first, and an unknown token returns `MemberNotFound`. That means the plan's
recommended answer to open question 3 (reuse selection's `MemberInactive`) is
the **zero-cost** path; a friendlier "already detached" message requires a
resolver variant that skips `require_active`, which is real (if small) extra
surface. The parenthetical in step 5 already hints at this — just be aware
the default is decided by the resolver, not the handler.

### F9 — Confirmation: lock entries have no `active` field, so removal is forced

`ResolvedMemberArtifact` (`artifact/mod.rs:167-185`) has no `active` flag, and
the boundary exclude is built from `lock.members`. Keeping a stale lock row
for an inactive member would therefore also keep its exclude line — i.e. the
plan's open question 4 isn't really open: **retain is not viable** without
also adding an `active` field to the lock or filtering in
`ensure_workspace_exclude`. Remove-from-lock is the only option consistent
with the rest of the plan. (`lock.members` is a `BTreeMap` keyed by member
id, so removal is a single map remove.)

### F10 — Minor: manifest field is `id`, not `member_id`

`ManifestMember` (`artifact/mod.rs:57-68`) serializes the identity field as
`id`. The plan's concept-level use of `member_id` is fine, but Wave 5's
`GWZDesign.md` edits and any YAML examples should use the real field name.

### F11 — Minor: the `repo sync` CLI guard is broader than "--target/--member"

The plan says to reject the positional combined with "global `--target` /
`--member`". The actual check is `meta.selection.is_some()`
(`clirequest.rs:578-582`), which also covers `--member-path`, `--all`, and
all exclude flags (any of which produce a `Selection`,
`clirequest.rs:318-340`). Copying the existing check verbatim gives detach
the broader (correct) behavior for free — just describe it that way in help
text and tests.

### F12 — Minor: decide `gwz-py` dry-run behavior for `repo clone` up front

`gwz-py`'s existing `clone` handler rejects `--dry-run` outright
(`cli_local.py:100-101`) because the streaming path has no dry-run story. The
plan requires dry-run for `repo clone` on both surfaces; for `gwz-py` that
means the nested `repo clone` either takes the non-streaming call when
`--dry-run` is set, or the streaming submit path must pass `dry_run` through.
Small, but it's a parity-bar item ("same argv/defaults") so name the choice
in Wave 4 rather than discovering it there.

## Answers to the plan's open questions

1. **Method names** — see F5: choose from the real inventory;
   `clone_repo`/`detach_repo` fits the request-message convention best.
2. **Derived-id collision** — agree with the recommendation (require explicit
   `--member-id`). Note the failure already exists as `InvalidRequest`
   "member id is already registered" (`handle_create_repo.rs:553-565`,
   `handle_init_from_sources.rs:304-309`); the deliverable is the error
   message pointing at `--member-id`, not new detection logic.
3. **Detach token resolution** — agree: reuse selection's `MemberInactive`
   (see F8; it's the zero-cost default). Only build the clearer message if it
   stays a one-liner variant of the resolver.
4. **Lock removal vs retain** — not actually open; retain is inconsistent
   with the lock-driven boundary (F9). Remove.
5. **`--member-id` on add/create** — yes; the wire fields already exist
   (`gwz.taut.py:963-982`), so this is CLI-only and cheap, and it keeps the
   replacement story symmetric across clone and add.
6. **`gwz-py` clone progress** — stream, via the existing submit route (F3);
   the infrastructure (`_stream_call`, event rendering) already exists for
   `clone_workspace`. Only the dry-run edge (F12) needs a decision.
7. **Existing-workspace init** — agree, leave plan-only; verified it is
   plan-only today.
8. **Reactivate** — agree it's out of v0. One consistency note for the
   follow-on: because path uniqueness becomes active-only, `repo attach` will
   need its own path-collision preflight (the reactivated row's path may have
   been taken by a newer active member).

## Wave-level notes

- **Wave 0** should additionally freeze: the `PlannedAction` verb (F1, next
  free enum value 15) and the nesting semantics of active-only path
  uniqueness (F2). Both are spec, not implementation.
- **Wave 2** cross-tests: add the prefix/nesting replacement cases from F2.
- **Wave 4**: correct the dispatch routing row (F3) and decide dry-run
  behavior (F12).
- **Wave 5**: `CLI.md` regeneration is test-enforced
  (`gwz-cli/src/tests/g00.rs:29-37`) — run
  `python scripts/generate_cli_reference.py --write`.

## Bottom line

Approve with revisions: fix F1 (planned-action shape) and F4 (`Ref.Response`)
before the Wave 1 protocol lands, specify F2 (nesting semantics) in Wave 0,
and correct F3 (gwz-py routing) in the parity table. Everything else in the
plan checked out against source.
