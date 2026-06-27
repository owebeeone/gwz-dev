# GWZ Target Selection Plan

Status: draft plan, 2026-06-27.

This plan defines a workspace-wide target selection model that can represent the
workspace root repository, normal configured repositories, and future declared
sets without overloading "member" further. It is motivated by `gwz push`
needing to include the root repository by default, while `gwz ls` and
`gwz forall` should continue to default to configured repositories only.

## Source Inventory

Implementation planning was checked against these current surfaces:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/model/mod.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/convert.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/push_member.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_materialize.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_ls.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_tag.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_branch.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stash.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/status/status_member.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stage.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/stage_routing.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/globalargs.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/forall.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_shared.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/client.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_read.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_mutation.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_local.py`

Current selection is member-only:

- Taut `Selection` has `all`, `member_ids`, and `paths`.
- Rust model `Selection` stores `member_ids: Vec<MemberId>`, so `@root` cannot
  be represented without changing the model.
- Core helpers return `Vec<MemberId as String>` or `Vec<&ManifestMember>`.
- `gwz forall` is CLI-local, but it obtains its iteration list from `handle_ls`.
- `gwz-py` mirrors the same global `--member`, `--member-path`, and `--all`
  parser shape and builds the same Taut selection fields.

## Goals

- Make the workspace root addressable as `@root`.
- Make `--all` mean all workspace targets everywhere, including `@root`.
- Provide a uniform subtractive selector, including `--all --no-target @root`.
- Make no explicit selection equivalent to `--target @default`.
- Keep command defaults command-defined:
  - `ls`: default excludes `@root`.
  - `forall`: default excludes `@root`.
  - `push`: default includes `@root`.
- Reserve the `@...` selector namespace now so future declared sets can use
  names such as `@frontend`, `@docs`, or `@release`.
- Keep workspace semantics in `gwz-core`; CLIs only parse flags and render
  Taut-defined responses.
- Keep `gwz-py` parity in the same plan, not as a later cleanup.

## Non-goals

- Do not implement declared target sets in this change. Reserve their syntax
  and fail unknown `@name` selectors today.
- Do not rename every existing protocol field from `member` to `target` in one
  pass. The wire schema can grow target-aware fields while retaining legacy
  aliases during the v0 transition.
- Do not make `gwz-core` execute `forall`; `forall` remains client-local.
- Do not hide unsupported root behavior by silently dropping `@root` from an
  explicit `--all`.

## Terms

Target:

- A concrete operation subject in the workspace.
- Today this is `@root` or a configured workspace repository.

Selector:

- A token that expands to zero or more targets.
- Today this includes reserved selectors, concrete member ids, and member paths.
- Future releases may add declared set selectors under the same `@name`
  namespace.

Reserved selectors:

- `@root`: the workspace root repository.
- `@all`: every target, including `@root`.
- `@default`: the command-defined default target set.

Future reserved namespace:

- Any token beginning with `@` is selector syntax.
- Real configured member ids must not start with `@`.
- Unknown `@name` selectors fail today with a typed invalid-selection error.
- Future declared sets may use `@name` if the workspace declares them.

## Selection Algebra

Selection resolves by set algebra:

1. If no include selectors are provided, include `@default`.
2. Expand all include selectors.
3. Expand all exclude selectors.
4. Final selection is includes minus excludes.
5. Deduplicate by concrete target identity.
6. Produce deterministic target order.

Recommended deterministic order:

- Display/listing order: `@root` first, then active configured members in
  manifest order.
- Mutation execution order may override display order when the operation has a
  correctness reason. `push` should push members first and `@root` last.

Examples:

```text
gwz ls
  expands to: @default
  for ls: @default = @all - @root

gwz ls --all
  expands to: @all
  result: @root + configured members

gwz ls --all --no-target @root
  expands to: @all - @root
  result: configured members only

gwz ls --all --no-target @default
  for ls: @all - (@all - @root)
  result: @root

gwz push
  expands to: @default
  for push: @default = @all

gwz push --all --no-target @root
  result: configured members only

gwz push --target @root
  result: @root only
```

## CLI Surface

New preferred global selectors:

```text
--target <selector>
--no-target <selector>
--all
```

Aliases retained for compatibility:

```text
--member <selector>       alias for --target <selector>
--no-member <selector>    alias for --no-target <selector>
--member-path <path>      legacy path selector; maps to --target <path>
--no-member-path <path>   optional legacy path exclusion; maps to --no-target <path>
--all                     alias for --target @all
```

Parser rules:

- `--all` may combine with exclusions.
- `--all` may combine with additional includes, but duplicate includes are
  harmless after dedupe.
- `--target @default --target @root` means command default plus root.
- `--all --no-target @default` is valid and command-relative.
- `--target @name` where `@name` is not reserved and not declared fails in
  core, not in a hard-coded CLI table.
- Existing `--all cannot be combined with --member or --member-path` validation
  must be removed from both Rust and Python CLIs.

Help text should describe "workspace targets" while noting that `--member` is a
compatibility alias. It should not describe `--all` as "all workspace members".

## Command Defaults

Every command declares an `@default` expansion. The selector syntax is uniform;
defaults are not.

| Command | Default | Root support in this plan |
| --- | --- | --- |
| `ls` | `@all - @root` | list root when selected |
| `forall` | `@all - @root` | run in root when selected |
| `push` | `@all` | push root, with root executed last |
| `status` | `@all` | reconcile existing root status with target selection |
| `pull --head` | `@all` | root pull already belongs in workspace sync behavior |
| `commit` | `@all` | root commit already belongs in workspace commit behavior |
| `add` / `stage` | pathspec-driven unless target selection is explicit | root is already a routing target |
| `capture` | `@all - @root` until root capture is specified | explicit `@root` rejected |
| `snapshot` | `@all - @root` until root snapshots are specified | explicit `@root` rejected |
| `materialize` | `@all - @root` until root materialize is specified | explicit `@root` rejected |
| `tag` | `@all - @root` until root tag policy is specified | explicit `@root` rejected |
| `branch` | `@all - @root` until root branch support lands | explicit `@root` rejected |
| `stash` | `@all - @root` until root stash support lands | explicit `@root` rejected |

Important rule:

- Unsupported-target policy applies after selector expansion and subtraction.
  `materialize --all --no-target @root` is valid because the final target set
  does not contain `@root`.
- A root-unsupported command must reject clearly when the final target set
  contains `@root`, for example `materialize --all` or
  `materialize --target @root`.
- A command must not silently subtract unsupported targets on the user's behalf;
  only explicit user exclusions such as `--no-target @root` can remove them.

This preserves the meaning of `--all`: it includes `@root`. Commands that are
not root-capable keep root out of their default by defining `@default` as
`@all - @root`.

## Taut Protocol Changes

The protocol source of truth remains:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`

Recommended schema direction:

```python
TargetKind=Enum(
    root=0,
    member=1,
)

Selection=Msg(
    # Legacy. Deprecated after target selectors land.
    # New clients should send targets/exclude_targets.
    all=F(1, BOOL, optional=True),
    member_ids=F(2, List(STR)),
    paths=F(3, List(STR)),

    # Include selector tokens. Examples: "@default", "@all", "@root",
    # "mem_app", "repos/app", and future declared sets such as "@docs".
    targets=F(4, List(STR)),

    # Exclude selector tokens, applied after include expansion.
    exclude_targets=F(5, List(STR)),
)

MemberEntry=Msg(
    id=F(1, STR),
    path=F(2, STR),
    abspath=F(3, STR),
    materialized=F(4, BOOL),
    # Optional for old clients. Root entries set target_kind=root.
    target_kind=F(5, Ref.TargetKind, optional=True),
)
```

Field-name notes:

- `targets` and `exclude_targets` are preferred over a nested
  `TargetSelector` message for v0. The selector token is the contract.
- Existing `member_ids` and `paths` remain accepted during migration.
- Existing `all=true` should normalize to `@all` once this plan lands.
- Legacy `member_ids` may include `@root` only if the compatibility path maps
  them into selector strings before member-id validation.
- W0 also adds optional `target_kind` fields, using the next unused field number
  in each message, to `MemberResponse`, `OperationEvent`, and `GwzError`. This
  makes target identity a Taut-defined protocol fact for list entries, operation
  responses, progress events, and diagnostics.
- `member_id="@root"` and `member_path="."` remain the v0 compatibility
  rendering for root-scoped responses, but they are not the only target
  identity contract once `target_kind` lands.

Taut regeneration outputs:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/corpus/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/protocol/generated/api.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/protocol/generated/gwz.ir.json`

Required protocol gates:

- `python protocol/regen.py --check` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- `cargo test --test protocol` in `/Users/owebeeone/limbo/gwz-dev/gwz-core`
- `python scripts/regen_protocol.py --check` in `/Users/owebeeone/limbo/gwz-dev/gwz-py`

## Core Model Changes

The internal Rust model should stop representing request selection as
`Vec<MemberId>` at the top level. `MemberId::parse_str` enforces the `mem_`
prefix, which is correct for real members but wrong for selector tokens.

Recommended internal types:

```rust
pub enum TargetSelector {
    Default,
    All,
    Root,
    Token(String),
}

pub struct Selection {
    pub include: Vec<String>,
    pub exclude: Vec<String>,
    pub legacy_all: Option<bool>,
    pub legacy_member_ids: Vec<String>,
    pub legacy_paths: Vec<String>,
}

pub enum SelectedTarget {
    Root,
    Member(String),
}

pub struct ResolvedTargetSelection {
    pub targets: Vec<SelectedTarget>,
}
```

Normalization responsibilities:

- Convert absent selection to include `@default`.
- Convert `all=true` to include `@all`.
- Convert legacy `member_ids` and `paths` to include tokens.
- Convert `targets` and `exclude_targets` directly.
- Reject empty explicit selection only after legacy and new fields are
  normalized.
- Reject duplicate concrete targets if duplication indicates ambiguous user
  input; otherwise dedupe stable repeats from aliases.
- Preserve current `MemberId` validation when a token is resolved as a real
  member id.

Recommended new module:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/target_selection.rs`

This module should own:

- reserved selector constants;
- command default expansion;
- selector normalization;
- manifest lookup by id and path;
- future `@name` extension point;
- unsupported-target detection per command.

## Core Operation Surfaces

Current resolver choke points:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/push_member.rs`
  - `resolve_manifest_selection`
  - `resolve_explicit_locked_selection`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_materialize.rs`
  - `resolve_locked_selection`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/status/status_member.rs`
  - `resolve_selection`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_ls.rs`
  - list projection from manifest plus lock

These should become thin callers around the shared target-selection module.
Where an operation only supports members, it should ask the resolver for
members with `RootPolicy::RejectExplicit` and command default `@all - @root`.

Root response representation:

- Short-term compatibility can use existing per-repo response lists with
  `member_id="@root"` and `member_path="."`.
- Protocol additions must mark root entries with `target_kind=root` in
  `MemberEntry`, `MemberResponse`, `OperationEvent`, and `GwzError`.
- A later cleanup may rename response fields from "member" to "target"; this
  plan does not require that broad churn.

## `ls` Behavior

Default:

```text
gwz ls == gwz ls --target @default
@default = @all - @root
```

Required behavior:

- `gwz ls` lists configured repositories only.
- `gwz ls --all` lists `@root` plus configured repositories.
- `gwz ls --target @root` lists only root.
- `gwz ls --all --no-target @root` lists configured repositories only.
- `gwz ls --all --no-target @default` lists only root.

Root `MemberEntry` projection:

```text
id = "@root"
path = "."
abspath = workspace root absolute path
materialized = true
target_kind = root
```

`include_unmaterialized` applies to configured members only. `@root` is always
materialized if a workspace root was found.

## `forall` Behavior

Default:

```text
gwz forall -- <cmd> == gwz forall --target @default -- <cmd>
@default = @all - @root
```

Required behavior:

- `gwz forall -- <cmd>` runs in configured materialized repositories only.
- `gwz forall --all -- <cmd>` runs in root plus configured materialized
  repositories.
- `gwz forall @root -- <cmd>` runs in root only.
- `gwz forall mem_app @root -- <cmd>` runs in `mem_app` and root.
- `gwz forall --all --no-target @root -- <cmd>` runs in configured
  repositories only.

Implementation note:

- Positional `projects` in `forall` should be treated as selector tokens, not
  as a post-filter over the default `ls` result. Otherwise `@root` cannot be
  selected because default `ls` excludes it.
- Positional `projects` are command-local include selectors. They are unioned
  with global include selectors before global and command-local exclusions are
  applied.
- Any explicit include selector, whether global or positional, suppresses the
  implicit `@default` include. This preserves current `gwz forall mem_app --`
  behavior as "run only `mem_app`".
- Exclusions apply to the final include union. For example,
  `gwz forall --all --no-target @root mem_app -- <cmd>` resolves to all
  configured members, because `@all + mem_app - @root` still contains every
  configured member.
- Rust and Python `forall` implementations must share this behavior.

Environment for root execution:

```text
GWZ_MEMBER_ID=@root
GWZ_MEMBER_PATH=.
GWZ_MEMBER_ABSPATH=<workspace root>
GWZ_ROOT=<workspace root>
GWZ_TARGET_KIND=root
```

For configured members, `GWZ_TARGET_KIND=member` may be added as a compatible
extension.

## `push` Behavior

Default:

```text
gwz push == gwz push --target @default
@default = @all
```

Required behavior:

- Plain `gwz push` pushes configured repositories and the root repository.
- `gwz push --all --no-target @root` pushes configured repositories only.
- `gwz push --target @root` pushes root only.
- `gwz push --target mem_app --target @root` pushes `mem_app` and root.
- Push should preflight every selected target before mutating any remote when
  that is feasible.
- Push execution order should be configured repositories first, root last.

Root push policy:

- Use the root repository's Git remote configuration.
- Honor request `remote` and `refspec` overrides.
- If no override exists, follow the current branch upstream or the same default
  policy used by member push.
- If root has no pushable remote/upstream, report a root-scoped rejected result,
  not a member-scoped error.

Response policy:

- Include a root-scoped response in the same operation envelope.
- Use `@root` and `.` until a broader response rename lands.
- Event emission should use `member_id="@root"` and `member_path="."` for
  compatibility, with `target_kind=root` as the Taut-defined target identity.

## `status`, `pull`, `commit`, and `stage`

These commands already have root behavior or root path routing and must be
audited in the same target-selection work.

Status:

- Current combined status always computes root status separately.
- After this plan, root status should be included only if the resolved target
  set contains `@root`.
- Default remains root-aware by setting `@default = @all`.
- `gwz status --all --no-target @root` should suppress root status.

Pull head:

- Workspace sync semantics should include root by default.
- Default `@default = @all`.
- Root integration must continue to repair GWZ lock metadata without taking
  ownership of user `.gitignore` merges.

Commit:

- Workspace commit semantics include root by default.
- Default `@default = @all`.
- Existing command-specific `-a/--all` remains "stage tracked changes before
  commit" and must not be confused with global target `--all`.

Stage/add:

- Pathspec routing already treats root as an owning repo.
- Command-specific `-A/--all` remains "stage all changes" and must not be
  confused with global target `--all`.
- Explicit target selection should scope command-specific `-A` across selected
  targets, including `@root` when selected.

## `gwz-py` Parity

Python must not lag this change:

- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_shared.py`
  - add `--target`, `--no-target`, `--no-member`, optional `--no-member-path`;
  - remove the old `--all` combination rejection;
  - build target-aware meta fields.
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/client.py`
  - accept `targets` and `exclude_targets`;
  - keep `all_members`, `member_ids`, and `paths` as compatibility args;
  - normalize to generated `Selection`.
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_local.py`
  - update `forall` positional project resolution to use selector semantics.
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_read.py`
  - update `ls` help and tests.
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_mutation.py`
  - update push help and behavior.

Required tests:

- Python parser tests for `--all --no-target @root`.
- Python client request-construction tests for target/exclusion fields.
- Python `forall` tests proving `@root` can be selected positionally.
- Python protocol drift check after Taut regeneration.

## Documentation Surfaces

Update after behavior lands:

- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/CLI.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/commands/forall.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/commands/capture.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/commands/materialize.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/MachineOutput.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/docs/RootWorkspace.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/Reference.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/MessageCatalog.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/docs/Regeneration.md`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/README.md`

Docs must state:

- `--all` means `@all`, including `@root`.
- `@default` is command-relative.
- `--all --no-target @root` is the canonical all-except-root spelling.
- `--member` is a compatibility alias for `--target`.
- Unknown `@name` selectors are reserved for future declared sets and fail
  today.

## Implementation Phases

Phases are milestones. Steps are single goals, aspirationally under 500 LOC.
Foundational protocol and resolver work lands before command behavior. The
TDD-first rule applies: failing tests, implementation, green tests, then
refactor.

This rollout is intended for multiple agents. Agents may use short-lived Git
worktrees for isolated branches, but worktrees are disposable implementation
scratch space. The final integration must land in the main working tree through
reviewed patches, not by treating a temporary worktree as durable state.

### Chokepoints and Merge Order

These files are shared chokepoints and must land in the order below:

1. `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
2. Generated protocol outputs:
   - `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs`
   - `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/corpus/`
   - `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/protocol/generated/api.py`
   - `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/protocol/generated/gwz.ir.json`
3. `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/model/mod.rs`
4. New core resolver module:
   - `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/target_selection.rs`
5. CLI selection builders:
   - `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/globalargs.rs`
   - `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs`
   - `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_shared.py`
   - `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/client.py`

After those are stable, command verticals can proceed in parallel. Agents are
not alone in the codebase; each must check current status before editing, avoid
reverting other work, and adapt to already-landed changes.

### Parallel Agent Ownership

| Agent | Ownership | Primary waves | May run in parallel with | Must wait for |
| --- | --- | --- | --- | --- |
| Atlas, coordinator | Merge order, semantics freeze, worktree/branch coordination, final gate matrix | All waves | All agents | None |
| Schema, protocol agent | Taut schema, generated Rust/Python protocol, corpus, protocol conversion tests | W0 | None for schema edits | Atlas semantics freeze |
| Resolver, core selection agent | Internal target-selection model and resolver module, legacy normalization, command default policies | W1 | Lens, PyBridge, Docs after W0 | Schema W0 |
| Lens, Rust CLI agent | Rust global flags, selection request construction, parser/help tests | W1-W2 | Resolver, PyBridge, Docs | Schema W0; integrates with Resolver API before W2 gate |
| PyBridge, Python agent | Python global flags, client meta construction, generated protocol drift, parser/client tests | W1-W3 | Lens, Docs; command parity after Resolver | Schema W0 |
| LocalExec, ls/forall agent | `handle_ls`, Rust `forall`, root `MemberEntry`, execution env behavior | W2 | ForgePush, SyncAudit, PyBridge | Resolver W1 |
| ForgePush, push agent | Root-aware push planning, backend primitive/adaptation, events, push tests | W2 | LocalExec, SyncAudit, Docs | Resolver W1; root response decision from Atlas |
| SyncAudit, sync/status agent | Status, pull-head, commit, stage/add root selection audit and unsupported-root rejections | W2 | LocalExec, ForgePush, PyBridge | Resolver W1 |
| MemberOps, member-only command agent | Capture, snapshot, materialize, tag, branch, and stash default/exclusion/rejection behavior | W2 | LocalExec, ForgePush, PyBridge | Resolver W1 |
| Docs, documentation agent | CLI docs, core protocol/reference docs, Python docs, message catalog update | W1-W3 | All agents after W0 | Schema names from W0; implemented behavior before final docs |
| Sentinel, review/test agent | Independent plan review, adversarial implementation review, targeted workflow and release gates | W0-W3 | All agents | Updated plan or candidate branch |

### Optional Worktree Workflow

Atlas may create temporary worktrees per independent branch, for example:

```text
gwz-target-schema
gwz-target-resolver
gwz-target-rust-cli
gwz-target-python
gwz-target-ls-forall
gwz-target-push
gwz-target-sync-audit
gwz-target-member-ops
gwz-target-docs
```

Rules:

- Worktrees are convenience only and can be deleted after merge.
- Chokepoint worktrees must merge in the order listed above.
- Command vertical worktrees must not regenerate protocol outputs.
- Workers must not edit unrelated files to make their branch easier to merge.
- If a worktree discovers a semantics problem, it updates the plan or reports
  back to Atlas before inventing a local rule.

### W0 - Semantics and Taut Protocol

Serial phase. No command implementation starts before this gate.

Touchpoints:

- This plan.
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/gwz.taut.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/generated.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/protocol/corpus/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/protocol/convert.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/protocol/generated/api.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/protocol/generated/gwz.ir.json`

Work:

- Ratify reserved selector names: `@root`, `@all`, `@default`.
- Ratify preferred flags: `--target`, `--no-target`.
- Ratify aliases: `--member`, `--no-member`, `--member-path`.
- Ratify command default matrix.
- Decide whether `--no-member-path` lands in the first pass.
- Ratify root identity shape: `target_kind` is added to `MemberEntry`,
  `MemberResponse`, `OperationEvent`, and `GwzError`; root-scoped compatibility
  fields remain `member_id="@root"` and `member_path="."`.
- Add `TargetKind`.
- Add `Selection.targets` and `Selection.exclude_targets`.
- Add optional `MemberEntry.target_kind`.
- Add optional `MemberResponse.target_kind`, `OperationEvent.target_kind`, and
  `GwzError.target_kind`.
- Regenerate Rust protocol output and corpus.
- Regenerate Python protocol output and IR.
- Update Rust conversion code so legacy and new fields round trip.
- Perform the mechanical constructor/default-field sweep required by generated
  Rust structs and Python dataclasses after adding fields. Known constructor
  touchpoints include:
  - `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs`
  - `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_ls.rs`
  - `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/forall.rs`
  - `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/client.py`
- Add protocol corpus cases for:
  - `targets=["@all"]`;
  - `exclude_targets=["@root"]`;
  - legacy `all=true`;
  - `MemberEntry{id="@root", path=".", target_kind=root}`;
  - root-scoped `MemberResponse`, `OperationEvent`, and `GwzError` examples
    with `target_kind=root`.

Acceptance:

- The command default matrix has no `TBD`.
- Unsupported-root commands have explicit reject behavior.
- `python protocol/regen.py --check` passes in `gwz-core`.
- `cargo test --test protocol` passes in `gwz-core`.
- `python scripts/regen_protocol.py --check` passes in `gwz-py`.
- `cargo check --locked` passes in `gwz-core`.
- `cargo check --locked` passes in `gwz-cli`.
- Python generated protocol imports and `python -m compileall src/gwz` pass in
  `gwz-py`.
- Old selection payloads still decode.
- New target/exclusion payloads round trip through Rust and Python.

### W1 - Parallel Selection Foundations

These workstreams can run after W0. Resolver owns semantics; CLI and Python
agents may proceed with parser/request construction while syncing against the
resolver API before the W1 gate.

#### W1a - Core Target Resolver

Owner: Resolver.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/model/mod.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/target_selection.rs`
- Existing wrappers in:
  - `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/push_member.rs`
  - `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_materialize.rs`
  - `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/status/status_member.rs`

Work:

- Introduce internal target selection structs.
- Normalize legacy fields and new fields into include/exclude selector lists.
- Implement reserved selector expansion.
- Implement command default expansion.
- Implement manifest lookup by member id and member path.
- Reject unknown `@name` selectors.
- Reject unsupported root targets according to command policy.
- Keep member-only helpers as wrappers where needed.

Acceptance:

- Unit tests cover:
  - absent selection expands to command `@default`;
  - `--all` expands to `@root` plus members;
  - `--all --no-target @root` excludes root;
  - `--all --no-target @default` is command-relative;
  - `@root` is not parsed as `MemberId`;
  - real member ids still require the existing `mem_` validation;
  - unknown `@set` fails today;
  - unsupported root fails for member-only commands when the final set contains
    `@root`;
  - `--all --no-target @root` succeeds for member-only commands because the
    final set excludes `@root`;
  - legacy `all=true` normalizes to `@all`;
  - legacy `member_ids=["@root"]` normalizes through the compatibility alias
    path before `MemberId` validation.

#### W1b - Rust CLI Selection Builder

Owner: Lens.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/globalargs.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/clirequest.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/tests/`

Work:

- Add global `--target`.
- Add global `--no-target`.
- Add global `--no-member` as an alias.
- Keep `--member` as an alias for includes.
- Keep `--member-path` as path include compatibility.
- Remove the old `--all` combination rejection.
- Build `Selection.targets` and `Selection.exclude_targets`.
- Keep legacy fields populated only where compatibility requires it; prefer
  target fields for new CLI output.
- Update help text from "members" to "targets" where appropriate.

Acceptance:

- Parser tests cover:
  - `gwz --all --no-target @root ls`;
  - `gwz --target @default --target @root ls`;
  - `gwz --member @root push`;
  - `gwz --no-member @root push`;
  - legacy `--member mem_app`;
  - legacy `--member-path repos/app`.
- `--all` no longer errors when combined with exclusions.
- Command-specific `add -A/--all` and `commit -a/--all` help text remains
  distinct from global target `--all`.

#### W1c - Python Selection Builder

Owner: PyBridge.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_shared.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/client.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/tests/`

Work:

- Mirror Rust CLI global target flags.
- Mirror client meta construction.
- Remove Python's old `--all` combination rejection.
- Keep `all_members`, `member_ids`, and `paths` as compatibility API args.
- Add parser and client request-construction tests.

Acceptance:

- Python parser accepts `--all --no-target @root`.
- Python client builds the same Taut `Selection` as Rust for target selectors.
- Rust and Python request-construction fixtures agree for `targets` and
  `exclude_targets`, including `--all --no-target @root`.
- Python compatibility API `all_members=True` maps to `@all` target semantics
  and is documented as a legacy name, not member-only behavior.
- Python protocol drift check remains green.

W1 gate:

- Resolver, Rust CLI, and Python selection tests pass.
- `cargo test --test protocol` remains green.
- No command vertical has introduced its own selector parser.

### W2 - Parallel Command Verticals

These workstreams can run after W1. Each vertical consumes the shared resolver
and should not fork selection semantics locally.

#### W2a - `ls` and Rust `forall`

Owner: LocalExec.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_ls.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/tests/g18.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/forall.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/tests/`

Work:

- Make `handle_ls` use target selection with `@default = @all - @root`.
- Emit root `MemberEntry` when selected.
- Add optional `target_kind` to root and member entries.
- Change Rust `forall` positional projects to selector tokens.
- Ensure `forall` requests `ls` with the correct explicit selection when
  positionals are present.
- Add root execution env variables.

Acceptance:

- `gwz ls` excludes root.
- `gwz ls --all` includes root.
- `gwz ls --all --no-target @root` excludes root.
- `gwz ls --all --no-target @default` returns only root.
- `gwz forall @root -- pwd` runs once at workspace root.
- `gwz forall --all -- pwd` runs root plus materialized members.
- `gwz forall --all --no-target @root -- pwd` runs members only.
- `gwz forall --target mem_a mem_b -- pwd` runs `mem_a` plus `mem_b`.
- `gwz forall --all --no-target @root mem_a -- pwd` runs all configured
  members, not only `mem_a`.
- Unknown positional `@name` fails as an unknown selector, not as a missing
  member id.

#### W2b - Root-Aware Push

Owner: ForgePush.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/push_member.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/git/gitbackend.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/operation/push_event.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/tests/`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/push_long.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-cli/src/push_after.rs`

Work:

- Make push use target selection with `@default = @all`.
- Add a root push plan and preflight.
- Implement or adapt Git backend root push primitive.
- Execute member pushes first and root push last.
- Return root-scoped push response.
- Emit root-scoped progress/lifecycle events using compatible fields.
- Document root remote/upstream/refspec behavior.

Acceptance:

- Plain `gwz push` attempts members and root.
- `gwz push --all --no-target @root` attempts members only.
- `gwz push --target @root` attempts root only.
- Root push missing remote/upstream reports a root-scoped rejected result.
- If preflight detects an invalid selected target, no remote is mutated.
- Existing member push concurrency and per-host limits remain covered.

#### W2c - Status, Pull, Commit, and Stage Audit

Owner: SyncAudit.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/status/status_member.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/pull_head_member_preflight.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_commit.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stage.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/stage_routing.rs`

Work:

- Gate existing status root projection on resolved `@root`.
- Make pull-head default target selection root-aware.
- Make commit default target selection root-aware.
- Make explicit target selection scope stage `-A` correctly.
- Reject explicit root on commands that are not root-capable yet.

Acceptance:

- `gwz status` includes root by default.
- `gwz status --all --no-target @root` suppresses root status.
- `gwz pull --head` includes root by default without changing `.gitignore`
  handling.
- `gwz commit -m ...` includes root by default.
- `gwz --target @root add -A` stages only root changes.
- Tests distinguish global target `--all` from command-local `add -A/--all`
  and `commit -a/--all`, in both parser output and handler behavior.
- Unsupported-root commands produce a clear error when root is explicit.

#### W2d - Member-Only Command Migration

Owner: MemberOps.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_materialize.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_tag.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_branch.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/handle_stash.rs`
- `/Users/owebeeone/limbo/gwz-dev/gwz-core/src/workspace_ops/tests/`

Work:

- Migrate capture, snapshot, materialize, tag, branch, and stash handlers from
  direct locked-member selection to the shared target resolver.
- Keep their defaults as `@all - @root`.
- Permit `--all --no-target @root` as the explicit all-configured-members
  spelling.
- Reject final selections that contain `@root` until root behavior is designed
  per command.
- Ensure rejection happens after include-minus-exclude resolution.

Acceptance:

- For each command family, default selection excludes root and continues to
  operate on configured members.
- For each command family, `--all --no-target @root` succeeds as
  configured-members-only.
- For each command family, `--all` rejects because final selection contains
  root.
- For each command family, `--target @root` rejects clearly.
- Existing branch/stash/tag/snapshot/materialize behavior remains covered.

#### W2e - Python Command Parity

Owner: PyBridge.

Touchpoints:

- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_local.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_read.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/gwz/cli_mutation.py`
- `/Users/owebeeone/limbo/gwz-dev/gwz-py/src/tests/`

Work:

- Mirror `ls` target behavior.
- Mirror `forall` selector semantics.
- Mirror push help and request path.
- Add Python command tests for root selection.

Acceptance:

- Python `forall @root -- pwd` runs at root.
- Python `forall --target mem_a mem_b -- pwd` runs `mem_a` plus `mem_b`.
- Python `forall --all --no-target @root mem_a -- pwd` runs all configured
  members, not only `mem_a`.
- Python `ls --all` includes root.
- Python `push --target @root` reaches the native bridge with root selection.
- `python run_tests.py` passes in `gwz-py`.

W2 gate:

- Targeted `gwz-core` workspace operation tests pass.
- Targeted `gwz-cli` parser and workflow tests pass.
- `python run_tests.py` passes in `gwz-py`.
- No vertical changes Taut schema or generated protocol outputs.

### W3 - Documentation, Review, and Release Gates

These workstreams can begin drafting after W0 but must finalize after W2
behavior is implemented.

Touchpoints:

- CLI docs listed in this plan.
- Core protocol/reference docs listed in this plan.
- Python README and CLI docs.
- Generated message catalog.

Work:

- Update CLI reference and command docs.
- Update protocol docs and message catalog.
- Update machine output examples for root `MemberEntry`, root-scoped
  `MemberResponse`, root-scoped `OperationEvent`, and root-scoped `GwzError`.
- Update Python docs.
- Add troubleshooting notes for unsupported root commands and unknown
  future-set selectors.
- Sentinel performs adversarial review of merged behavior and tests.

Acceptance:

- `cargo fmt --check` and targeted tests pass in `gwz-core`.
- `cargo fmt --check` and targeted tests pass in `gwz-cli`.
- `python protocol/regen.py --check` passes in `gwz-core`.
- `cargo test --test protocol` passes in `gwz-core`.
- `python scripts/regen_protocol.py --check` and `python run_tests.py` pass in
  `gwz-py`.
- Docs describe `@root`, `@all`, `@default`, `--target`, `--no-target`,
  `--member` aliases, and command defaults consistently.
- Sentinel review has no unresolved P0/P1 findings.

## Risk Matrix

| Risk | Mitigation | Gate |
| --- | --- | --- |
| `--all` changes from member-only to root-inclusive. | Command defaults keep member-only commands on `@default = @all - @root`; explicit `--all` is documented as root-inclusive. | Parser and command tests for every migrated command. |
| Root is silently dropped by commands that cannot handle it. | Resolver validates the final include-minus-exclude target set and rejects unsupported root instead of subtracting it implicitly. | Unsupported-root tests, including `--all` reject and `--all --no-target @root` success. |
| `@root` is parsed as `MemberId` and rejected before selector resolution. | Model stores selector strings until expansion; member id validation happens after reserved selector handling. | Resolver unit test. |
| `forall @root` cannot work because `forall` filters after default `ls`. | Treat `forall` positionals as selector tokens and call `ls` with explicit selection. | Rust and Python `forall @root` tests. |
| Rust CLI and Python CLI drift. | W1c and W2e are required parity workstreams, not optional cleanup. | `python run_tests.py` plus parser/client tests. |
| Taut schema drift or shadow protocol types. | Add fields only in `gwz.taut.py`, regenerate checked-in Rust/Python outputs, and keep corpus tests green. | Regen and protocol tests. |
| Response field names still say "member" for root. | Use compatible `@root` entries short-term and add `target_kind` to list entries, operation responses, events, and errors; defer broad response rename. | Machine-output docs and response/event/error tests. |
| Future declared sets conflict with real member ids. | Reserve all `@...` tokens and reject real member ids with that prefix. | Manifest/member-id validation tests. |

## Open Decisions Before Implementation

- Should `--no-member-path` land in W1, or is `--no-target <path>` enough?
- Should root push use a new backend primitive or adapt the existing member push
  primitive with a synthetic target descriptor?
