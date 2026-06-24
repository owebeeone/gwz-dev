# GWZ Documentation Plan

Status: recommendation review only. Updated after the tag redesign and
`ls`/`forall` features landed. Do not implement the broad docs set until branch,
stash, and any adjacent command-surface changes stabilize.

Date: 2026-06-25

## Purpose

This document records the recommended GWZ documentation work after the current
feature set stabilizes. It is intentionally a plan, not an implementation. Tag
management and `ls`/`forall` are now implemented and should be documented as
current behavior. Branch support, stash support, and any adjacent command-surface
changes are still treated as documentation gates.

The target outcome is a coherent documentation set for:

- the root `gwz-dev` workspace;
- the primary CLI in `gwz-cli`;
- the embeddable API and protocol in `gwz-core`;
- the GWZ taut protocol message catalog.

## Current Inventory

Root workspace:

- There is no root `README.md`.
- There is no root `AGENTS_GWZ.md`.
- There is no root `AGENTS.md`, although `gwz-cli/AGENTS.md` and
  `gwz-core/AGENTS.md` both currently say to follow the root `AGENTS.md`.
- The root `Cargo.toml` is a unified Cargo workspace for `gwz-core` and
  `gwz-cli`, with `gwz-cli` depending on `gwz-core` by path.
- The root workspace is itself GWZ-managed through tracked `gwz.conf/`
  metadata.

`gwz-cli`:

- `gwz-cli/README.md` has useful install instructions, a current command list,
  examples, and development commands.
- There is no `gwz-cli/docs/` directory yet.
- The README states that Clap command definitions should be the source of truth
  for help and generated Markdown reference docs such as `docs/CLI.md`.
- The command list now includes implemented `tag`, `ls`, and `forall` behavior,
  but remains somewhat unstable while branch, stash, and related workflow
  changes are still in flight.

`gwz-core`:

- `gwz-core/README.md` gives a good conceptual introduction and embedding
  example.
- `gwz-core/docs/Reference.md` is a useful start, but it is incomplete relative
  to the current protocol and code. The current protocol includes
  `CaptureRequest`, `CommitRequest`, `StageRequest`, `LsRequest`, `TagRequest`
  with `TagOp`, and CLI-local `Exec*` messages that are not covered in the
  request table.
- `gwz-core/protocol/gwz.taut.py` is the source for the GWZ protocol service,
  enums, messages, and field tags.
- `gwz-core/protocol/corpus/` is already the conformance corpus location.
- `gwz-core/docs/` needs an API documentation set, not only a single reference
  page.

Design and planning docs:

- `gwz-core/dev-docs/GWZDesign.md` is marked authoritative, but some older text
  still conflicts with current implementation details. Current code uses
  `gwz.conf/gwz.yml`, `gwz.conf/gwz.lock.yml`, and `gwz.conf/snapshots`.
  `gwz.conf/tags` and `TagArtifact` have been removed; real tags now live in
  Git as `refs/tags/*`. Older docs mention names such as `workspace.gwz.yaml`,
  `.gwz/snapshots`, and old tag artifacts.
- Implemented feature plans now live under `gwz-core/dev-docs/history/`,
  including `GWZAddPlan.md`, `GWZTagPlan.md`, and `GWZForallPlan.md`.
- `gwz-core/dev-docs/history/GWZTagPlan.md` records the implemented tag
  redesign. `gwz tag` is now real Git tag management across selected members,
  with local create/list/delete, remote push/fetch/list/delete, and
  `materialize --tag` as tag checkout.
- `gwz-core/dev-docs/history/GWZForallPlan.md` records the implemented
  `ls`/`forall` work. `gwz ls` is a real core operation for member listing, and
  `gwz forall` is a CLI-local executor that runs commands in selected
  materialized members.
- `gwz-core/dev-docs/GwzBranchPlan.md` describes proposed branch management.
- `gwz-cli/dev-docs/GwzStashSpec.md` and `GwzStashPlan.md` describe stash
  behavior that is not implemented yet.

`taut`:

- `taut/docs/` is a mature docs set with an index, overview, reference, server
  guide, and per-language API pages.
- The GWZ docs should reuse that style for a local GWZ protocol catalog, but the
  GWZ catalog should be generated from `gwz-core/protocol/gwz.taut.py`, not
  copied manually from the generic taut docs.

## Recommended Root Docs

### `AGENTS_GWZ.md`

Add a small root-level `AGENTS_GWZ.md` once the feature wave is stable. It should
be short enough for agents to read quickly and should focus on operational
guidance, not full product documentation.

Recommended contents:

- What GWZ is in this workspace: the root repo coordinates nested member repos
  through tracked `gwz.conf/` metadata.
- Root discovery: from inside a workspace, `gwz` should discover the workspace
  root upward like Git. Use `--root <path>` only when running from outside the
  workspace or when deliberately targeting a different workspace.
- How to get the `gwz` tool:
  - release installer from `gwz-cli` GitHub releases;
  - `cargo install --git https://github.com/owebeeone/gwz-cli`;
  - local development fallback: `cargo run -p gwz -- ...` from the root
    workspace.
- How to inspect the root workspace:
  - `gwz status`;
  - `gwz status --combined`;
  - `gwz status --json`;
  - `gwz ls`;
  - `gwz ls --local`.
- How to manage root workspace changes:
  - `gwz add <pathspec>...`;
  - `gwz add -A`;
  - `gwz commit -m "<message>"`;
  - `gwz capture`;
  - `gwz snapshot <name>`;
  - `gwz materialize --lock`;
  - `gwz pull --head`;
  - `gwz push`;
  - `gwz tag`;
  - `gwz tag <name>`;
  - `gwz tag --push <name>`;
  - `gwz tag --push`;
  - `gwz tag --fetch`;
  - `gwz materialize --tag <name>`.
- How to run a command across materialized members:
  - `gwz forall -- git status --short`;
  - `gwz forall -c "git rev-parse --abbrev-ref HEAD"`;
  - `gwz --partial forall -- <cmd>`.
- How to add or create members:
  - `gwz repo add <repo-path>`;
  - `gwz repo create <member-path>`.
- Selection reminders:
  - `--member <member-id>`;
  - `--member-path <member-path>`;
  - `--all`.
- Agent cautions:
  - prefer GWZ commands over ad-hoc cross-repo Git scripts for root workspace
    operations;
  - treat `gwz forall` as arbitrary local command execution; it streams child
    process output and intentionally rejects `--json`/`--jsonl` in v0;
  - do not hand-edit generated protocol output;
  - do not document branch or stash as available until their implementations and
    tests are stable;
  - treat `gwz.conf/` as tracked workspace metadata.

Because the existing package-level `AGENTS.md` files point at a missing root
`AGENTS.md`, the final implementation should also choose one of these fixes:

- add a tiny root `AGENTS.md` that points to `AGENTS_GWZ.md`; or
- update package-level `AGENTS.md` files to name `AGENTS_GWZ.md` directly.

The first option is more compatible with tooling that discovers `AGENTS.md`
automatically.

### Root `README.md`

Add a root `README.md` for humans working in `gwz-dev`. This should be an
onboarding and repository-map document, not the exhaustive CLI manual.

Recommended contents:

- One-paragraph purpose: `gwz-dev` is the development super-workspace for GWZ,
  containing the CLI, core library, taut protocol tooling, and workspace
  metadata.
- Repository map:
  - `gwz-cli/`: primary `gwz` binary and user command surface;
  - `gwz-core/`: embeddable Rust engine, workspace artifacts, Git backend,
    protocol types;
  - `taut/`: schema/compiler/protocol tooling used by GWZ;
  - `gwz.conf/`: tracked GWZ root workspace metadata;
  - `dev-docs/`: root-level planning documents.
- Bootstrap:
  - install `gwz`;
  - use `cargo run -p gwz --` when hacking locally;
  - run `cargo fmt`, `cargo test`, and package-specific tests.
- Root workspace workflow:
  - list members with `gwz ls`;
  - status before mutation;
  - stage/commit through `gwz`;
  - snapshot/materialize/tag/pull/push;
  - run member-local maintenance commands with `gwz forall`;
  - add/create member repos.
- Documentation map:
  - root `AGENTS_GWZ.md` for agent operations;
  - `gwz-cli/docs/` for user-facing CLI docs;
  - `gwz-core/docs/` for Rust API, artifacts, protocol, and message catalog;
  - `taut/docs/` for taut itself.
- Feature status:
  - tag redefinition implemented as real Git tag management;
  - `gwz ls` and `gwz forall` implemented;
  - branch support proposed/in progress;
  - stash support specified but not implemented until landed;
  - avoid relying on stale command docs for branch and stash while this wave is
    active.
- Release and install pointers:
  - `gwz-cli/README.md` release installer commands;
  - local source install command.

## Recommended `gwz-cli/docs` Set

Create `gwz-cli/docs/` as the primary user-facing CLI documentation surface.
Keep `gwz-cli/README.md` concise and make it point into `docs/`.

Recommended files:

- `gwz-cli/docs/README.md`: docs index and command-family map.
- `gwz-cli/docs/Install.md`: release installers, source install, pinned version
  installs, checksum/attestation guidance, local dev usage.
- `gwz-cli/docs/QuickStart.md`: create or clone a workspace, list members,
  inspect status, add/create members, stage, commit, snapshot, tag, materialize,
  run a member command with `forall`, pull, and push.
- `gwz-cli/docs/Concepts.md`: workspace root, member repo, manifest, lock,
  member listing, snapshot, Git tag, branch, stash bundle, selection, dry-run,
  partial, force, remotes, progress events, and CLI-local `forall` execution.
- `gwz-cli/docs/CLI.md`: generated or generated-assisted full command
  reference from Clap.
- `gwz-cli/docs/commands/*.md`: one page per command family after the final
  command surface is known.
- `gwz-cli/docs/Workflows.md`: task-oriented flows that cross command
  boundaries.
- `gwz-cli/docs/MachineOutput.md`: `--json`, `--jsonl`, status porcelain,
  response envelope shape, event stream shape, `gwz ls` listing output,
  `gwz forall` machine-mode rejection, and exit-code expectations.
- `gwz-cli/docs/RootWorkspace.md`: using `gwz` to manage the development root
  workspace and other GWZ-managed roots, including `ls`, `forall`, and tag
  workflows.
- `gwz-cli/docs/Troubleshooting.md`: missing workspace, nested workspace,
  dirty member, diverged member, missing remote, SSH timeout, credentials,
  conflicts, partial results.
- `gwz-cli/docs/Releases.md`: release process, installers, artifact
  verification, source installs.

Command pages should follow one template:

- synopsis;
- what it changes;
- required workspace state;
- key flags;
- examples;
- human output notes;
- JSON/JSONL notes where applicable;
- common failures and recovery;
- links to related commands.

Feature-specific command page requirements:

- `tag`: document that tags are real Git refs (`refs/tags/<name>`), not GWZ
  artifacts; local create/list/delete spans selected members plus the committed
  workspace root; remote push/fetch/list/delete spans members only; `gwz tag`
  lists local tags; `gwz tag <name>` creates a lightweight tag; `-m` creates an
  annotated tag; `-s` requires `-m`; `gwz tag --push <name>` pushes one tag;
  `gwz tag --push` pushes all local tags; `gwz tag --fetch` fetches all tags;
  `gwz materialize --tag <name>` checks out members that carry the tag and skips
  untagged members.
- `ls`: document that it is read-only, backed by manifest plus lock, not Git
  status; default output is absolute materialized member paths; `--local` prints
  workspace-relative paths; `--unmaterialized` includes configured members that
  are not present on disk.
- `forall`: document `gwz forall [projects...] -- <cmd> [args...]` and
  `gwz forall [projects...] -c "<shell>"`; projects match member id or member
  path; `{@}` expands to the member path in argv mode; child processes receive
  `GWZ_MEMBER_ID`, `GWZ_MEMBER_PATH`, `GWZ_MEMBER_ABSPATH`, and `GWZ_ROOT`;
  output is streamed live with per-member banners unless `--no-banner`; execution
  is sequential; default stops on first failure; global `--partial` continues;
  `--json` and `--jsonl` are rejected in v0.

Do not hand-maintain a long static command reference if a Clap-derived generator
can provide it. A small generator should emit the mechanical synopsis and flag
tables, while workflow pages can remain hand-authored.

CLI help should also advertise the hosted documentation URL:
`https://github.com/owebeeone/gwz-cli/tree/main/docs`. Put it near the top of
the root `gwz --help` output so users see it immediately, and again at the end
of the help output as the final pointer after command and option details.
Generated Markdown docs should preserve or explicitly mirror that URL.

## Recommended `gwz-core/docs` Set

Expand `gwz-core/docs/` from one practical reference into a complete library and
protocol documentation set.

Recommended files:

- `gwz-core/docs/README.md`: core docs index.
- `gwz-core/docs/Embedding.md`: how an application, agent, or UI should call
  `gwz-core` directly.
- `gwz-core/docs/OperationModel.md`: request metadata, selection, policy,
  dry-run, attribution, operation ids, aggregate status, member status, partial
  results, event flow.
- `gwz-core/docs/RustApi.md`: curated Rust API guide for public modules and
  intended entrypoints.
- `gwz-core/docs/WorkspaceArtifacts.md`: current manifest, lock, snapshot, stash
  metadata when implemented, local runtime state, schema versions, atomic write
  rules, and artifact examples. It should state clearly that `gwz tag` no longer
  writes a GWZ tag artifact; it manages real Git refs.
- `gwz-core/docs/GitBackend.md`: `GitBackend` boundary, Git2 backend behavior,
  credentials, transfer progress, tag primitives, and fallback rules.
- `gwz-core/docs/MemberListing.md`: `LsRequest`, `LsResponse`, `MemberEntry`,
  materialized vs unmaterialized members, and how `forall` reuses member listing.
- `gwz-core/docs/TagManagement.md`: `TagRequest`, `TagOp`, local vs remote tag
  behavior, selected members plus root rules, `TagInfo`, and
  `materialize --tag` semantics.
- `gwz-core/docs/Protocol.md`: GWZ service methods, transport expectations,
  request/response envelope rules, and corpus/regeneration rules.
- `gwz-core/docs/MessageCatalog.md`: generated catalog of all taut service
  methods, enums, messages, field tags, optional fields, and reserved/evolution
  metadata.
- `gwz-core/docs/ErrorCatalog.md`: stable `GwzErrorCode` values, likely causes,
  which commands can emit them, and suggested caller/user recovery.
- `gwz-core/docs/EventCatalog.md`: `OperationEvent`, event kinds, severity,
  progress counters, and JSONL rendering expectations.
- `gwz-core/docs/Regeneration.md`: how `protocol/gwz.taut.py`,
  `src/protocol/generated.rs`, `protocol/corpus/golden.json`, and
  `protocol/corpus/rust/vectors.rs` are regenerated and checked.

The core docs should distinguish three contract layers:

- Rust library API: what callers can import and call.
- Taut protocol API: what drivers can serialize, route, and evolve.
- Workspace artifact schemas: what appears on disk under `gwz.conf/` or local
  runtime state.

## Taut Message Catalog

The GWZ taut message catalog should be generated from
`gwz-core/protocol/gwz.taut.py`. It should not be manually transcribed from
`src/protocol/generated.rs`, because generated Rust is an output, not the
contract source.

Recommended catalog sections:

- Schema identity and source file path.
- Service methods:
  - method name;
  - role;
  - shape;
  - params;
  - response type.
- CLI-local protocol values:
  - `ExecMode`;
  - `ExecRequest`;
  - `ExecResponse`;
  - `ExecResult`;
  - why these messages are not service methods and have no `gwz-core` handler.
- Enums:
  - enum name;
  - member name;
  - wire value;
  - short meaning.
- Messages:
  - message name;
  - field name;
  - tag;
  - type;
  - optional/transient/merge flags;
  - description from adjacent schema comments where available.
- Request/response matrix:
  - request type;
  - response type;
  - core handler;
  - CLI command family, when one exists;
  - whether the value is a callable core operation or CLI-local support data.
- Evolution notes:
  - field tags are stable;
  - never reuse retired tags or names;
  - regenerate corpus after schema edits;
  - catalog drift should fail verification.

The generator can start simple: load the taut schema, export its neutral IR, and
render Markdown tables. It should later grow comment extraction only if the
underlying taut API exposes comments cleanly; do not use brittle source slicing
as the permanent approach.

## Parallel Work Packages

These packages are designed so many agents can work independently after branch
and stash stabilize. Tag and `ls`/`forall` should be treated as implemented
source-of-truth inputs, not future work. Each package should keep edits within
its listed ownership area unless a dependency explicitly calls for cross-links.

| ID | Package | Depends On | Primary Outputs | Acceptance Criteria |
| --- | --- | --- | --- | --- |
| D0 | Source-of-truth freeze | Branch, stash, and adjacent command changes landed | Updated inventory notes in this file or a short successor note | Final command list, artifact paths, protocol schema, and feature status are confirmed against code and tests. Confirm tag, `ls`, and `forall` as implemented surfaces. |
| D1 | Root agent guide | D0 | `AGENTS_GWZ.md`, optional root `AGENTS.md` pointer | Agents can install or run `gwz`, inspect the root workspace, stage/commit through GWZ, list members, run `forall`, manage tags, and avoid stale branch/stash guidance. |
| D2 | Root README | D0 | root `README.md` | A new contributor can understand the repo map, bootstrap locally, run tests, and manage the root GWZ workspace. |
| C1 | CLI docs skeleton | D0 | `gwz-cli/docs/README.md`, doc template, link map | Directory structure exists and all planned pages are linked without broken local links. |
| C2 | CLI reference generator | D0, C1 | generator script or task, `gwz-cli/docs/CLI.md` | Generated reference matches `gwz --help` and command-level help; drift can be detected. Root help includes `https://github.com/owebeeone/gwz-cli/tree/main/docs` near the top and at the end. |
| C3 | CLI command pages | D0, C1 | `gwz-cli/docs/commands/*.md` | Every stable command family has synopsis, mutation semantics, flags, examples, output, and failure notes. This can be split one agent per command family; `tag`, `ls`, and `forall` should be documented from the implemented code. |
| C4 | CLI workflow docs | C1, C3 | `QuickStart.md`, `Workflows.md`, `RootWorkspace.md` | Workflows are executable against the final CLI, include member listing, real Git tags, and `forall`, and do not rely on deprecated tag artifacts. |
| C5 | Machine output docs | D0, C1 | `MachineOutput.md` | JSON, JSONL, status porcelain, `ls` listings, response envelopes, event streams, `forall` machine-mode rejection, and exit behavior are documented with checked examples. |
| K1 | Core docs skeleton | D0 | `gwz-core/docs/README.md`, revised `Reference.md` outline | Core docs have a stable index and existing reference gaps are identified. |
| K2 | Rust embedding/API guide | D0, K1 | `Embedding.md`, `RustApi.md` | Public entrypoints, request construction, responses, errors, and events are documented with compiling examples or tested snippets. |
| K3 | Workspace artifact docs | D0, K1 | `WorkspaceArtifacts.md` | Current artifact paths, schemas, examples, atomicity rules, and generated/local boundaries match code; `gwz.conf/tags` is documented only as removed history. |
| K4 | Git backend docs | D0, K1 | `GitBackend.md` | Backend responsibilities, credential behavior, transfer progress, tag primitives, and Git command fallback rules are documented. |
| K5 | Member listing and tag API docs | D0, K1 | `MemberListing.md`, `TagManagement.md` | `LsRequest`/`MemberEntry`, `TagRequest`/`TagOp`, `TagInfo`, local/remote tag behavior, and `materialize --tag` are documented from current core code. |
| P1 | Protocol catalog generator | D0 | catalog generator, `MessageCatalog.md` | Catalog is generated from `protocol/gwz.taut.py`, includes methods/enums/messages/fields/tags, distinguishes service methods from CLI-local `Exec*` values, and has a drift check. |
| P2 | Protocol guide | D0, P1 | `Protocol.md`, `Regeneration.md` | Service shape, request/response envelope, corpus generation, and schema evolution rules are documented. |
| P3 | Error/event catalogs | D0, P1 | `ErrorCatalog.md`, `EventCatalog.md` | Error codes, aggregate/member statuses, event kinds, severity, and progress counters are documented from protocol/model sources. |
| V1 | Example verification | C2-C5, K2-K4, P1-P3 | checked examples, doc-test notes, command transcript fixtures | Examples run against the final CLI/core where practical; non-executable examples are explicitly marked. |
| V2 | Link and drift checks | all docs packages | CI or local verification commands | Local links pass, generated docs are current, and docs do not mention removed tag artifacts or `gwz.conf/tags` as live behavior. |
| X1 | README consolidation | C1-C5, K1-K4, P1-P3 | trimmed `gwz-cli/README.md`, updated `gwz-core/README.md` | Package READMEs are concise entrypoints and point to the fuller docs without duplicating stale command tables. |

## Suggested Sequencing

1. Wait for branch, stash, and any other remaining command-surface changes to
   land. Treat tag and `ls`/`forall` as already landed.
2. Run D0 to confirm the final source of truth.
3. Start D1, D2, C1, K1, and P1 in parallel.
4. Start C2, K2, K3, K4, P2, and P3 once skeletons and catalog direction are
   stable.
5. Split C3 by command family and run those agents in parallel.
6. Write workflow docs after command pages exist.
7. Run V1 and V2 last.
8. Consolidate existing READMEs after the deeper docs are stable.

## Documentation Rules For Implementation

- Use current code, generated help, `gwz-core/protocol/gwz.taut.py`, and tests
  as the source of truth.
- Preserve `dev-docs/history/` as history; do not rewrite historical docs unless
  a separate cleanup task asks for it.
- Mark superseded design text clearly instead of letting it conflict silently
  with user-facing docs.
- Keep `gwz-cli` documentation user-facing and task-oriented.
- Keep `gwz-core` documentation API/protocol/artifact-oriented.
- Keep the taut catalog generated or generated-assisted.
- Treat command examples as test assets where possible.
- Document tag, `ls`, and `forall` as implemented behavior.
- Avoid documenting planned branch or stash behavior as available until the
  corresponding feature is merged and verified.

## Open Decisions

- Should the root file be only `AGENTS_GWZ.md`, or should a root `AGENTS.md`
  point to it for automatic agent discovery?
- Should `gwz-cli/docs/CLI.md` be fully generated, or should it be generated
  into command pages with hand-authored examples beneath each page?
- Should the GWZ taut message catalog live only in `gwz-core/docs/`, or should a
  generated copy also be published with release artifacts?
- Should artifact schema examples be generated from test fixtures to guarantee
  drift checks?
- Where should CLI-local `Exec*` messages be documented while they temporarily
  live in `gwz-core/protocol/gwz.taut.py` but are not core service methods?
- Should `gwz forall` get captured machine-output mode later, or should v0's
  `--json`/`--jsonl` rejection remain the documented long-term behavior?
- Should old `GWZDesign.md` artifact-layout sections be updated directly after
  branch/stash land, or should user-facing docs simply supersede them and leave
  design history untouched?

## Not In Scope For This Planning Pass

- Creating `AGENTS_GWZ.md`.
- Creating or editing the root `README.md`.
- Creating `gwz-cli/docs/`.
- Expanding `gwz-core/docs/`.
- Generating the taut message catalog.
- Changing CLI behavior, protocol schema, or workspace artifacts.
