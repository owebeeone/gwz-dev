# GWZ Documentation Plan

Status: recommendation review only. Updated after the tag redesign and
`ls`/`forall` features landed. Implementation may proceed now against the
current v0.3.0 command surface. Branch and stash are not part of this
documentation wave.

Date: 2026-06-25

## Purpose

This document records the recommended GWZ documentation work for the current
implemented v0.3.0 surface. It is intentionally a plan, not an implementation.
Tag management and `ls`/`forall` are now implemented and should be documented as
current behavior. Branch support, stash support, and adjacent future command
surface changes are out of scope for this documentation wave.

The target outcome is a coherent documentation set for:

- the root `gwz-dev` workspace;
- the primary CLI in `gwz-cli`;
- the embeddable API and protocol in `gwz-core`;
- the GWZ taut protocol message catalog.

## Current Inventory

Root workspace:

- There is no root `README.md`.
- There is no `AGENTS_GWZ.md` bootstrap template or generated
  `AGENTS_GWZ.md` file yet.
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
- The command list now includes implemented v0.3.0 `tag`, `ls`, and `forall`
  behavior. Branch, stash, and related future workflow changes are not part of
  this documentation pass.

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

## Recommended Bootstrap Agent File

### `AGENTS_GWZ.md`

`AGENTS_GWZ.md` should be a GWZ-managed bootstrap file, not a root-specific
operations guide. Its purpose is to give an LLM just enough information to get
the `gwz` tool, clone a GWZ workspace, or materialize a workspace that has
already been cloned.

Recommended product behavior:

- GWZ creation commands should add a tracked `AGENTS_GWZ.md` to every GWZ
  repository they create. If the final surface is `gwz create`, that command
  owns the behavior; if the final surface remains split across `init`, `clone`,
  and `repo create`, document exactly which creation paths write the file.
- Working CLI proposal: `gwz init --update` updates existing GWZ-managed
  bootstrap files, including `AGENTS_GWZ.md`, to the latest bundled template.
  This should be an idempotent existing-workspace operation, not a workspace
  creation path.
- The template should be versioned with `gwz-cli`, so docs can say which CLI
  release generated or updated it.
- The template contents should be sourced from the release-branch bootstrap doc
  when available:
  `https://raw.githubusercontent.com/owebeeone/gwz-cli/refs/heads/release/docs/agent_bootstrap.md`.
  D0a must verify the exact reachable path before implementation.
- The file should be safe to overwrite when it matches a known GWZ template, and
  should refuse or require confirmation when users have edited it.
- The file should point to the hosted docs:
  `https://github.com/owebeeone/gwz-cli/tree/main/docs`.

Recommended contents:

- One sentence: this repository is managed by GWZ, a multi-repository workspace
  tool.
- Install `gwz`:
  - release installer from `gwz-cli` GitHub releases;
  - `cargo install --git https://github.com/owebeeone/gwz-cli`.
- If the workspace is not cloned yet:
  - `gwz clone <workspace-git-url> [directory]`.
- If this repository is already cloned:
  - `gwz materialize --lock`;
  - optionally `gwz status`.
- Link to CLI docs and help:
  - `gwz --help`;
  - `https://github.com/owebeeone/gwz-cli/tree/main/docs`.

Keep it deliberately brief. Do not include detailed workflow guidance, command
catalogs, contribution policy, repo-specific build instructions, or full agent
rules. Those belong in `README.md`, `AGENTS.md`, or package-specific docs.

Decision: `AGENTS_GWZ.md` is tracked in GWZ-created repositories. This is
intentional: an agent cloning a root branch should have a reasonable chance of
finding the bootstrap file and then materializing the workspace correctly.

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
  - `AGENTS_GWZ.md` as the GWZ-managed LLM bootstrap file when present;
  - `gwz-cli/docs/` for user-facing CLI docs;
  - `gwz-core/docs/` for Rust API, artifacts, protocol, and message catalog;
  - `taut/docs/` for taut itself.
- Feature status:
  - v0.3.0 is the baseline for this documentation wave;
  - tag redefinition implemented as real Git tag management;
  - `gwz ls` and `gwz forall` implemented;
  - branch and stash are future work and are not documented as available
    commands.
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
- `gwz-cli/docs/AgentBootstrap.md`: `AGENTS_GWZ.md` purpose, generated template
  contents, creation behavior, `gwz init --update` behavior, overwrite rules,
  and intended LLM bootstrap use.
- `gwz-cli/docs/QuickStart.md`: create or clone a workspace, list members,
  inspect status, add/create members, stage, commit, snapshot, tag, materialize,
  run a member command with `forall`, pull, and push.
- `gwz-cli/docs/Concepts.md`: workspace root, member repo, manifest, lock,
  member listing, snapshot, Git tag, selection, dry-run, partial, force,
  remotes, progress events, and CLI-local `forall` execution.
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

These packages are designed so many agents can work independently now. Tag and
`ls`/`forall` should be treated as implemented source-of-truth inputs, not
future work. Branch and stash are outside the v0.3.0 documentation scope. Each
package should keep edits within its listed ownership area unless a dependency
explicitly calls for cross-links.

| ID | Package | Depends On | Primary Outputs | Acceptance Criteria |
| --- | --- | --- | --- | --- |
| D0a | Current source-of-truth freeze | None | Updated inventory notes in this file or a short successor note | Current v0.3.0 command list, artifact paths, protocol schema, feature status, and `AGENTS_GWZ.md` source URL are confirmed against code, tests, and reachable docs. Confirm tag, `ls`, `forall`, add, commit, snapshot, materialize, pull, push, capture, init, clone, and repo commands as current surfaces. |
| D1 | GWZ bootstrap agent template | D0a | bundled tracked `AGENTS_GWZ.md` template, `gwz init --update` behavior spec, `AgentBootstrap.md` | GWZ-created repos receive a deliberately brief tracked `AGENTS_GWZ.md`; the template is based on the verified release bootstrap doc; `gwz init --update` can update it to the latest bundled version; overwrite behavior is documented and safe. |
| D2 | Root README | D0a | root `README.md` | A new contributor can understand the repo map, bootstrap locally, run tests, and manage the root GWZ workspace using the v0.3.0 surface. |
| C1 | CLI docs skeleton | D0a | `gwz-cli/docs/README.md`, doc template, link map | Directory structure exists and all planned v0.3.0 pages are linked without broken local links. |
| C2 | CLI reference generator | D0a, C1 | generator script or task, `gwz-cli/docs/CLI.md` | Generated reference matches current `gwz --help` and command-level help; drift can be detected. Root help includes `https://github.com/owebeeone/gwz-cli/tree/main/docs` near the top and at the end. |
| C3 | CLI command pages | D0a, C1 | `gwz-cli/docs/commands/*.md` | Every currently implemented v0.3.0 command family has synopsis, mutation semantics, flags, examples, output, and failure notes. This can be split one agent per command family; `tag`, `ls`, and `forall` should be documented from the implemented code. |
| C4 | CLI workflow docs | C1, C3 | `QuickStart.md`, `Workflows.md`, `RootWorkspace.md` | Workflows are executable against the current v0.3.0 CLI, include member listing, real Git tags, and `forall`, and do not rely on deprecated tag artifacts. |
| C5 | Machine output docs | D0a, C1 | `MachineOutput.md` | JSON, JSONL, status porcelain, `ls` listings, response envelopes, event streams, `forall` machine-mode rejection, and exit behavior are documented with checked examples. |
| K1 | Core docs skeleton | D0a | `gwz-core/docs/README.md`, revised `Reference.md` outline | Core docs have a stable index and existing reference gaps are identified. |
| K2 | Rust embedding/API guide | D0a, K1 | `Embedding.md`, `RustApi.md` | Public entrypoints, request construction, responses, errors, and events are documented with compiling examples or tested snippets. |
| K3 | Workspace artifact docs | D0a, K1 | `WorkspaceArtifacts.md` | Current v0.3.0 artifact paths, schemas, examples, atomicity rules, and generated/local boundaries match code; `gwz.conf/tags` is documented only as removed history. |
| K4 | Git backend docs | D0a, K1 | `GitBackend.md` | Backend responsibilities, credential behavior, transfer progress, tag primitives, and Git command fallback rules are documented. |
| K5 | Member listing and tag API docs | D0a, K1 | `MemberListing.md`, `TagManagement.md` | `LsRequest`/`MemberEntry`, `TagRequest`/`TagOp`, `TagInfo`, local/remote tag behavior, and `materialize --tag` are documented from current core code. |
| P1 | Protocol catalog generator | D0a | catalog generator, `MessageCatalog.md` | Catalog is generated from current `protocol/gwz.taut.py`, includes methods/enums/messages/fields/tags, distinguishes service methods from CLI-local `Exec*` values, and has a drift check. |
| P2 | Protocol guide | D0a, P1 | `Protocol.md`, `Regeneration.md` | Service shape, request/response envelope, corpus generation, and schema evolution rules are documented. |
| P3 | Error/event catalogs | D0a, P1 | `ErrorCatalog.md`, `EventCatalog.md` | Error codes, aggregate/member statuses, event kinds, severity, and progress counters are documented from protocol/model sources. |
| V1 | Example verification | C2-C5, K2-K4, P1-P3 | checked examples, doc-test notes, command transcript fixtures | Examples run against the current v0.3.0 CLI/core where practical; non-executable examples are explicitly marked. |
| V2 | Link and drift checks | all docs packages | CI or local verification commands | Local links pass, generated docs are current, and docs do not mention removed tag artifacts or `gwz.conf/tags` as live behavior. |
| X1 | README consolidation | C1-C5, K1-K4, P1-P3 | trimmed `gwz-cli/README.md`, updated `gwz-core/README.md` | Package READMEs are concise entrypoints and point to the fuller v0.3.0 docs without duplicating stale command tables. |

## Suggested Sequencing

1. Run D0a now to confirm the current implemented source of truth.
2. Start D1, D2, C1, K1, and P1 in parallel.
3. Start C2, K2, K3, K4, K5, P2, and P3 once skeletons and catalog direction are
   stable.
4. Split C3 by currently implemented v0.3.0 command family and run those agents
   in parallel.
5. Write workflow docs after current command pages exist.
6. Run V1 and V2 for the current docs set.
7. Consolidate existing READMEs after the deeper docs are stable.

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
- Do not include branch or stash command docs in the v0.3.0 documentation set.

## Open Decisions

- Working proposal: use `gwz init --update` for template refresh. Should it
  update one repo, all selected repos, or every GWZ-managed repository by
  default?
- How should the CLI detect a user-edited `AGENTS_GWZ.md` versus a known bundled
  template that can be overwritten automatically?
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
- Should old `GWZDesign.md` artifact-layout sections be updated directly during
  the v0.3.0 documentation pass, or should user-facing docs simply supersede
  them and leave design history untouched?

## Not In Scope For This Planning Pass

- Creating `AGENTS_GWZ.md`.
- Creating or editing the root `README.md`.
- Creating `gwz-cli/docs/`.
- Expanding `gwz-core/docs/`.
- Generating the taut message catalog.
- Changing CLI behavior, protocol schema, or workspace artifacts.
- Documenting branch or stash commands.
