# Review: `RepoClonePlan.md` (56s)

Reviewed: 2026-07-10

Subject: `dev-docs/RepoClonePlan.md`, covering `gwz repo clone` and
`gwz repo detach` with inactive manifest rows.

## Verdict

Revise before implementation. The soft-detach model is a good fit for the
existing manifest and lock design, but the plan treats active-only path
uniqueness as a local add/create preflight change. It is actually a
workspace-wide semantic change: selection, pathspec ownership, staging, diff
routing, artifact validation, and replacement identity all depend on paths
being unique across every manifest row today.

R1-R6 are correctness issues that should be settled in Wave 0 and represented
in Wave 2. R7-R10 are concrete protocol, response, Python, and failure-semantics
edits. I would not start parallel handler work until R1-R7 are incorporated in
the plan.

## Findings

### R1 — [P0] A replacement row makes path selection ambiguous; “Selection unchanged” is false

The plan allows the old inactive row and its active replacement to have the
same `path` (`RepoClonePlan.md:113-117`), but explicitly leaves selection
unchanged (`RepoClonePlan.md:369-376`). The current path resolver searches
**all** manifest rows and rejects a second match as ambiguous before checking
whether either row is active
(`gwz-core/src/workspace_ops/target_selection.rs:182-194`).

After the plan's own replacement sequence:

```text
mem_shared    path=libs/shared active=false
mem_shared_v2 path=libs/shared active=true
```

every command selected by `libs/shared` fails with `InvalidRequest: member path
selection is ambiguous`. That includes status, commit, pull, push, branch,
stash, materialize, repo sync, and the next detach. The replacement success
criterion therefore cannot pass as written.

Fix the plan to change path resolution as part of the invariant migration:

- select the single active row for a path;
- if there is no active row but one or more historical rows, return
  `MemberInactive` for an explicit path;
- retain id lookup across all rows, with `require_active` after the globally
  unique id is found;
- reject more than one active row for the same/colliding path in artifact
  validation, rather than discovering it during selection.

Add a cross-command test using a manifest with one inactive and one active row
at the same path. Selection by path must resolve the active row; selection by
the old id must return `MemberInactive`.

### R2 — [P0] Active-only paths must change ownership/routing consumers, not only registration

The same invariant breaks pathspec ownership even after R1 is fixed:

- `gwz add` builds its repository boundaries from every manifest row
  (`gwz-core/src/workspace_ops/handle_stage.rs:29-30`), so a detached checkout
  remains a stage target and `gwz add -A` still mutates it.
- Diff correctly excludes only active paths from the root diff
  (`gwz-core/src/diff/plan.rs:484-495`) but routes pathspecs using every row
  (`gwz-core/src/diff/plan.rs:503-559`). Its explicit-path validation then uses
  the first row with that path (`gwz-core/src/diff/plan.rs:593-613`), which may
  be the historical inactive row even when an active replacement exists.
- Routing maps are keyed by path, so repeated historical reuse of one path
  collapses identities even where the resolver itself does not error.

Introduce one canonical active-path view and use it for:

- manifest path collision validation;
- add/create/clone preflight;
- member path selection;
- stage ownership and fan-out;
- diff pathspec routing and explicit member lookup;
- any later path-to-member consumer.

“Path collision” here includes equal, parent, and nested member paths because
`validate_member_path_set` rejects prefix overlap, not just string equality.
The invariant should be stated as “active member roots do not overlap.” Tests
must cover inactive exact-path, parent-path, and child-path reuse.

### R3 — [P1] The advertised `gwz add -A` follow-up is unsafe after detach

Detach removes the path from `.git/info/exclude`, intentionally exposing the
checkout to the root repository. The plan then unconditionally tells users to
run `gwz add -A` (`RepoClonePlan.md:183-189`). A root Git add over a committed
nested repository stages it as a mode `160000` gitlink; an unborn nested
repository can make the add fail. Both outcomes conflict with GWZ's no-gitlink
root/member boundary.

This is not needed for the metadata change. `sync_workspace_boundary` calls
`stage_workspace_git_metadata` (`sync_workspace_boundary.rs:19-25`), which
stages `gwz.conf` directly (`stage_workspace_git_metadata.rs:7-13`).

Remove the blanket `gwz add -A` follow-up for detach. Document that, before any
root-wide add, the user must move/remove the detached checkout or add a personal
ignore rule. Add an integration test proving detach followed by the documented
commit workflow does not create a root gitlink and does not stage inside the
inactive repository.

This decision also needs to align with R2: an inactive path is root territory
for GWZ routing, but a nested `.git` directory is still hazardous Git input.

### R4 — [P1] “Exactly one target” needs raw-shape validation before normal selection

The wire sketch correctly says empty, `@default`, `@all`, and multi-target
selection are invalid (`RepoClonePlan.md:301-306`). The proposed handler order
instead resolves with the shared resolver and then checks for one member
(`RepoClonePlan.md:350-354`). That is insufficient:

- no selection defaults to all active members;
- `@default` expands to active members;
- `@all` expands to root plus active members;
- excludes can reduce a broad selection to one member.

In a one-member workspace, omitted selection or `@default` can therefore
resolve to exactly one member and accidentally detach it.

Add a detach-specific structural validator before expansion. It should accept
exactly one literal member id/path across `targets`, legacy `member_ids`, or
legacy `paths`, and reject `all`, any `@...` token, exclusions, an empty include,
or more than one include. Then pass the one opaque token to the ordinary member
resolver. Test omitted selection and `@default`/`@all` specifically in a
one-member workspace, plus `@all` reduced by an exclusion.

### R5 — [P1] The `source_id` rule and replacement examples contradict current behavior

The plan says `source_id` is unique across all rows and claims that is already
the behavior (`RepoClonePlan.md:219-222`). It is not:

- init planning rejects a duplicate source id
  (`handle_init_from_sources.rs:267-315`);
- add and create derive `source_id` but check only `member_id`
  (`handle_create_repo.rs:92-97`, `181-186`);
- `ManifestArtifact::validate` validates the id syntax but does not check
  duplicate member or source ids (`artifact/mod.rs:39-48`, `70-83`).

The documented replacement supplies only `--member-id`
(`RepoClonePlan.md:158-164`, `528-531`). Because source id is derived from the
path today, it still becomes `src_shared`, colliding with the inactive row under
the plan's proposed invariant.

Freeze one source-identity policy in Wave 0. If source ids are unique per
manifest across active and inactive rows, then:

- enforce duplicate member and source ids in `ManifestArtifact::validate`;
- add a shared `reject_duplicate_source_id` to add/create/clone;
- expose both id flags on add/create as a requirement, not an open suggestion;
- either require both flags for replacement, derive the default source id from
  the explicit member id, or define deterministic suffixing;
- update every replacement example and test.

If source ids may repeat for the same logical source, remove the blanket
uniqueness claim and specify how a different remote is prevented from silently
reusing the old source identity. `GWZDesign.md:464-466` already treats
`source_id` as a logical-source identity that may be shared across workspaces,
so this needs a deliberate ruling rather than “same as today.”

### R6 — [P1] Clone/detach can lose concurrent manifest or lock updates

`handle_clone_repo_member` is planned to read the manifest before a potentially
long network clone, then write a whole manifest and lock
(`RepoClonePlan.md:331-342`). A concurrent detach, add, create, repo sync,
capture, materialize, pull, branch, or commit can write newer artifact state in
between. The clone then publishes its stale copies and silently resurrects or
erases the other change. Detach has the same read-modify-write race, just a
shorter window.

There is already a process-wide/file-system-wide `WorkspaceMutatorLock`
(`gwz-core/src/operation/workspace_mutator_lock.rs:15-46`). The plan must either:

1. acquire the same workspace mutation lock before reading and hold it through
   clone/write/boundary sync; or
2. clone to a private temporary location, then acquire the lock, re-read and
   revalidate current manifest/lock state, publish the checkout, and merge the
   artifact change without replacing unrelated updates.

Whichever model is selected must cover all competing manifest/lock writers,
especially add/create/repo sync, or the lock only protects the new commands
from each other. Add contention and lost-update tests.

### R7 — [P1] `init_source_plans` cannot honor the clone request's id overrides

The clone request exposes optional `member_id` and `source_id`, but the proposed
handler converts the request to `SourceUrl` and calls `init_source_plans`
(`RepoClonePlan.md:331-337`). That helper accepts only `&[SourceUrl]` and always
derives both ids from the path (`handle_init_from_sources.rs:262-299`).

Use a single-source plan constructor that accepts optional ids, or extend the
helper input so overrides survive. The plan should name the sequence explicitly:
derive path, choose requested-or-derived ids, validate syntax, apply global id
rules, validate active path overlap, validate the disk target, then clone.
Tests must assert that explicit ids are persisted into both manifest and lock,
not merely accepted by the parser.

### R8 — [P1] Detach dry-run and success response are not representable as described

The plan suggests composing `WriteManifest` and `WriteLock` in dry-run output
(`RepoClonePlan.md:324-327`, `359`). `MemberResponse.planned` is a single
optional `PlannedChange`, and `PlannedChange` contains one enum action
(`gwz.taut.py:865-883`). There is no composition surface.

Add one append-only `PlannedAction.detach_member=15` and describe both artifact
effects in its message. That is smaller and clearer than changing the response
field to a list.

The plan also says the success `MemberResponse` reflects inactive state
(`RepoClonePlan.md:363`), but neither `MemberResponse` nor
`ResolvedMemberState` has an `active` field. After lock removal there is also no
post-operation resolved lock state to return. Freeze one of these contracts:

- return identity + `MemberStatus::Ok`, `state=None`, and a clear envelope
  message; or
- append an explicit active/member-lifecycle field to a response projection.

Add exact golden assertions for dry-run and success JSON/CBOR.

### R9 — [P1] `gwz-py` clone needs both direct-call and submit/stream routing

The parity table sends clone through `dispatch/read.rs`
(`RepoClonePlan.md:412-417`). Existing clone-like operations have two routes:

- direct `call` for a final response, used by JSON and dry-run clients;
- background `submit` plus event subscription for human progress
  (`gwz-py/native/src/dispatch/mod.rs:22-69`,
  `gwz-py/src/gwz/client.py:220-242`).

The Python CLI's top-level clone demonstrates the split: JSON uses
`clone_workspace`, while human output uses `clone_workspace_stream`
(`gwz-py/src/gwz/cli_local.py:99-113`). `repo clone` should add both
`clone_repo_member` and `clone_repo_member_stream`, with native call routing in
a backend/event-capable dispatch module and submit routing for progress. Detach
belongs on the quick direct-call path.

Because this plan requires clone dry-run parity, “stream vs single-shot” should
not remain open: dry-run and machine output use the direct response; mutating
human output may stream. Test both native routes, final operation result, and
dry-run with no filesystem mutation.

### R10 — [P2] Failure and publication semantics need a state table

“On failure roll back fresh dir; no writes” only describes clone failure
(`RepoClonePlan.md:339`). Failures can also occur while reading HEAD/status/
remotes, serializing or publishing manifest/lock, rewriting the exclude block,
or staging root metadata.

`write_manifest_and_lock` stages both files but publishes manifest first and
lock last. Cross-file publication is intentionally not fully atomic. Boundary
sync then occurs after both files are published. A blanket rollback after every
error would be wrong if the manifest was already published: deleting the clone
would leave a registered but missing member.

Add a small failure-state contract:

| Failure point | Required state |
|---|---|
| clone or post-clone Git inspection, before artifact publication | remove the fresh target; artifacts unchanged |
| pre-publication serialization/staging | remove the fresh target; artifacts unchanged |
| manifest/lock publication | preserve a recoverable state; do not blindly delete a now-registered target |
| boundary sync after artifact publication | membership change remains committed; report repairable boundary/staging failure |

Inject failures at least after clone, during Git inspection, and during boundary
sync. Define the analogous detach result when artifact publication succeeds but
boundary sync fails.

## Protocol and wave corrections

- The response sketches use `Ref.Response`, which does not exist
  (`RepoClonePlan.md:298-311`). They must use `Ref.ResponseEnvelope`, matching
  every current response wrapper (`gwz.taut.py:1151-1172`).
- Add the new operations to both generated `ActionKind` and the internal mirror,
  its `From<ActionKind>` conversion, `OperationRequest` and `context()`
  (`gwz-core/src/operation/push_event.rs:11-53`, `89-115`, `518-542`). Include
  CLI dispatch and the clone progress label in the Wave 1/3 checklist.
- Make active-path migration a named Wave 2 deliverable spanning artifact
  validation, selection, stage, and diff—not just “shared preflight edits.”
- Move source-id policy, exact-one detach selection, planned detach action,
  concurrent mutation strategy, and success response shape into Wave 0.

## Confirmed parts of the plan

- Soft detach is consistent with the existing `active` field and default active
  member selection.
- Removing the detached id from the lock is consistent with
  lock-as-current-composition and is necessary for the lock-driven exclude
  block.
- Snapshots and markers can remain untouched because they key historical state
  by stable member id.
- `RequestMeta.selection` is the correct wire location for detach targeting.
- `ActionKind` values 22 and 23 are currently free after `diff=21`.
- Add/create already carry optional member/source ids on the wire.
- Existing-workspace `init_from_sources` is plan-only today, so a first-class
  clone-member handler is appropriate.

## Recommended acceptance gate

Before implementation begins, update the plan so the following sequence is an
explicit cross-command test, not only a clone test:

```text
detach old id
move/remove old checkout
clone replacement with new identities
select replacement by path in status/ls/repo sync/diff/add/commit
select old id and receive MemberInactive
commit root metadata without creating a gitlink
reload manifest and lock from disk and validate all invariants
```

With R1-R10 resolved, the overall inactive-membership design is implementable
and the clone/detach pairing is coherent.
