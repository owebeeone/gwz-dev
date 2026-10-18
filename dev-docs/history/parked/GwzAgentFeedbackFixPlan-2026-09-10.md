# Fix plan: paths and outcome reporting from agent trials

Status: implementation integrated, final validation and patch release in progress.
Date: 2026-09-10. Current commits, test results and limitations are recorded in
[the delivery report](GwzAgentFeedbackDelivery-2026-09-10.md).
Code findings below describe the original defect baseline, not outstanding work.

The JSON-first trial completed 56 episodes without improving success rates.
Its actionable findings are product contracts and diagnostics, not a need to
instruct models more forcefully. Fix these before another skill comparison.

## 0. Fix the request boundary, not the shared-process accident

CLI and core may execute in different processes with different working
directories. Sharing cwd today is not a contract. This is the underlying
architectural issue: path meaning must survive serialization and dispatch.

- Capture the caller's cwd once at the driver entry point. Resolve command-line
  relative roots/operands against that captured base using one shared path
  preparation mechanism, not the receiving process's cwd.
- Requests must carry an explicit workspace location and sufficient path context
  for every operand. Preserve existing workspace-relative logical cwd fields for
  diff/log; use the same explicit semantics for other commands rather than
  inventing an independent interpretation in each handler. Where an operand is
  caller-relative (e.g. adding a checkout outside the caller's current workspace
  subtree), transmit its resolved location or its explicit base—never a bare
  relative string whose meaning is supplied by the server process.
- Put any missing context in the taut-defined request contract and carry it across
  serialization. An out-of-band Rust `start` argument is insufficient unless the
  transport explicitly reconstructs it from that request/session contract.
- Core validates and consumes this context. It must not fall back to its process
  cwd or silently treat an unresolved/outside-workspace cwd as workspace root.
  Missing or ambiguous context is an actionable error.
- Absolute paths are qualified by the filesystem where execution occurs. Separate
  processes sharing a filesystem can use caller-resolved absolute paths. A remote
  host needs workspace/path mapping at the transport boundary; a client absolute
  path is not automatically meaningful there. Returned `abspath` is absolute in
  the execution filesystem, not a promise that the client's machine has that path.

Specific boundary sites:
`gwz-cli/src/globalargs/invocation.rs::invocation_from_cli`,
`gwz-cli/src/clirequest/invocation.rs::request_meta` and `command_request`,
`gwz-cli/src/clirequest/common.rs::workspace_relative_cwd`,
`gwz-cli/src/clirequest/repo.rs`, `gwz-core/protocol/gwz.taut.py`, and
`gwz-py/native/src/dispatch/` (including `mod.rs`, `read.rs`, `materialize.rs`,
`branch_stash.rs`, `local_family.rs`, `diff.rs`, and `log.rs`). Audit all request
path fields and record each field's base/namespace before making changes.
Include clone destinations and identity-file paths, not just add/ls operands.

The current `workspace_relative_cwd` canonicalizes relative paths using ambient
cwd and returns an empty string when cwd cannot be expressed under the root.
That loses information: distinguish actual root cwd from outside/unresolved cwd.
Keep legal outside-root invocation behavior explicit rather than changing its base.

Acceptance: prepare and serialize a request for caller cwd A, deserialize/execute
it with core cwd B containing deliberately misleading same-named paths, and
verify only A's intended objects are used. Cover omitted/relative/absolute root,
member cwd and outside-root invocation. Avoid process-global chdir in parallel
unit tests: inject contexts, plus one small subprocess integration check. Verify
CLI and Python use identical boundary semantics. No remote service implementation
is required here, but the request must not depend on the in-process shortcut.

## 1. Make `abspath` absolute — first priority

Observed: `gwz --root "Harbor Workspace" --json ls` returned a relative
`entries[].abspath`. This is an API defect, not merely poor wording.

Exact sites:
- `gwz-core/src/workspace_ops/handle_create_repo.rs::resolve_workspace_root`
  currently returns an explicit root as `PathBuf::from(root)` unchanged.
- `gwz-core/src/workspace_ops/target_listing.rs::target_entries` uses that root
  directly for both root and member `abspath` values.
- `gwz-cli/src/append_branch_summary/response_listing.rs` publishes the value.
- `gwz-cli/src/forall.rs` uses it for `current_dir` and `GWZ_MEMBER_ABSPATH`.
  Execution can therefore be affected as well as presentation.

Apply the explicit request-boundary contract above: core receives or derives an
absolute normalized root from transmitted execution context, then uses it for
target listing and consumers. It never resolves a request against its own cwd.
Keep `path` workspace-relative and `abspath` absolute, including unmaterialized
members. Do not rename `abspath`, patch only the JSON renderer, or introduce a
second independent path resolver. Use existing injected filesystem/path services
where filesystem facts are needed; do not add ambient cwd reads inside helpers.

Acceptance: default root, `.`, relative root, absolute root, root selection and
member selections all produce absolute paths to the intended objects. Verify a
listed `abspath` can be used directly as a process cwd when materialized, and
that `forall` receives the same absolute path. Include spaces and Windows paths.

## 2. Repair explicit-relative-root pathspec routing

Observed from inside a lane: `gwz --root . add README.md` returned `PathEscape`;
omitting `--root .` allowed staging. Do not document this as user error.

Trace and fix the full path chain:
- `gwz-cli/src/globalargs/invocation.rs::invocation_from_cli`
- `gwz-cli/src/clirequest/common.rs::workspace_relative_cwd`
- `gwz-core/src/workspace_ops/handle_create_repo.rs::resolve_workspace_root`
- `gwz-core/src/workspace_ops/pathspec_routing.rs::route_pathspec`

Currently `route_pathspec` normalizes the operand and strips the root prefix,
while an explicit root can remain relative. Use the shared normalized root from
step 1 and ensure both operands have compatible path representations. Verify
Windows drive/verbatim prefixes and existing symlink/junction containment rules;
do not remove escape checks or canonicalize nonexistent/deleted file operands.

Acceptance: explicit `--root .` and omitted root route the same requested files
for add and diff, from root and member cwd. Sibling escapes still fail. `--target`
selects participating repos; it does not change the base of path operands.

## 3. Explain operand resolution and rejected paths

`handle_add_existing_repo_in` in `handle_create_repo.rs` resolves repository
operands through `resolve_input_path(start, ...)`, then reports only
`repository_path is not a git repository` on failure. The trial repeatedly tried
a workspace-relative path while its actual cwd was the parent directory.

Keep the existing cwd-relative operand semantics. Report the supplied operand,
resolved absolute path and base directory, and explain that `--root` selects the
workspace rather than changing cwd. Apply the same factual detail to PathEscape:
show the resolved candidate and allowed root. Keep existing error codes. For
`--root @root`/`@all`, give a selector-versus-path hint when resolution fails;
do not reserve those as forbidden literal directory names.

Update parser/command help in `gwz-cli/src/globalargs/parser.rs`,
`gwz-cli/src/repo_long.rs`, `gwz-cli/src/repo_sync_long.rs`, and
`gwz-cli/docs/commands/repo.md` / `add.md`. Verify the actual owning help module
before editing. JSON and human output must convey the same diagnostic facts.

## 4. Make lock comparison consistent and explain differences

Two confirmed, conflicting producers:
- `gwz-core/src/workspace_ops/handle_repo_lifecycle.rs::ok_member` hard-codes
  `LockMatch::Matches` for successful member operations.
- `gwz-core/src/status/member_not_materialized.rs::lock_match` intentionally
  requires a clean worktree plus matching commit, branch and attachment.

Preserve the second contract: recording `dirty: true` cannot prove that arbitrary
uncommitted contents match a commit. Remove the unconditional success assignment;
share a comparison function fed by observed state. If an operation has not
observed enough to establish equality, do not claim Matches.

Add optional structured difference reasons to the taut-owned member response
(e.g. dirty worktree, different commit, branch or attachment, missing lock entry).
Define them in `gwz-core/protocol/gwz.taut.py`, regenerate through
`gwz-core/protocol/regen.py`, and retain existing enum values/field identifiers.
Do not invent CLI-only protocol fields. Update CLI and Python projections and
compatibility checks together. Human output should explain the same reasons.

For dirty-only divergence: explain that uncommitted work differs from the locked
commit and does not by itself mean registration is inconsistent. A successful
`repo sync` returning Noop should explain that source/remote metadata was already
current; sync does not remove worktree changes. Do not suggest commit/reset when
the user asked to preserve uncommitted work.

Acceptance: register a dirty repo, inspect status, then sync. Registration is
successful, the original edit remains untouched, comparison/reasons agree across
responses, and Noop has a meaningful explanation. Also cover clean equality,
changed HEAD/branch/attachment, missing lock entries and unavailable observations.

## 5. CLI/Python parity is a release requirement

Every shared behavior fix must ship through both drivers. A correct CLI with an
unfixed Python boundary is an incomplete fix. Use the same core contracts and
request preparation policy; do not maintain separate path or comparison logic.

Capture caller context before Python dispatch/background handoff. Audit
`gwz-py/native/src/lib.rs::{call,submit}`, native dispatch, and worker creation:
a worker must retain the submitting call's context rather than read cwd later.
Update Python generated models/decoders for taut changes in the same delivery.

Use one shared case matrix, exercised through CLI requests and Python synchronous
and submitted operations wherever each operation supports those entry points:

| Case | Required agreement |
| --- | --- |
| Caller cwd A, executor cwd B; serialized request | Same intended workspace and operands, no access to misleading B paths |
| Omitted, `.`, relative and absolute root; member/outside cwd | Same resolution or same defined error; no silent root fallback |
| Root/member listing, including unmaterialized members | Same absolute execution paths and workspace-relative member paths |
| Add/diff with relative operands, legitimate escapes rejected | Same owning repository, changes or refusal code |
| Dirty registration, status, sync Noop | Same comparison meaning/reasons; original uncommitted work preserved |
| Rejected repository/path | Same error code and resolved-path/base facts |
| Async execution after caller context changes | Original captured request meaning retained |

Compare semantics and observable effects, not presentation bytes, timestamps,
request IDs or operation IDs. CLI human/JSON/JSONL projections and Python decoded
responses must preserve the same facts. Core fixtures alone do not establish
this parity: exercise actual driver request construction and response decoding.
Record both drivers' results against the same release candidate core revision.

## 6. Remove misleading no-change success messages

The lane traces also contain add/commit with no edits, followed by `status: Ok`
without evidence that any commit was created. This is not the model's overwrite
bug, but it makes a false assumption easier. Locate the response producer and
renderer before changing it; distinguish successful changes from successful
no-ops and explicitly report that no commit was created when nothing was staged.
Preserve exit success for benign no-ops. Do not equate a successful command with
completion of the user's broader task.

Likewise keep up-to-date merge results explicit about zero transferred changes;
the observed self-merge must not be presented as integration into another
workspace. A new self-merge prohibition is not required for this release.
Apply shared outcome semantics in core and project them consistently to CLI and
Python. Targeted checks must cover no-op versus actual changes; no new expensive
repository matrix is needed.

## Release scope and stopping point

Bundle these product fixes into the next release:
1. Explicit caller context across drivers/core, absolute listings, and correct
   relative-root routing (sections 0–2).
2. Resolved-path diagnostics, consistent lock reasons, useful sync/no-change
   outcomes, with CLI/Python parity (sections 3–6).
3. The separately assigned NTFS-name-gate replacement, per
   `GwzFilesystemCapabilityGateFix.md`. Its implementation is already present in
   the working tree; take that agent's validation evidence, not just code presence,
   as the completion record. Do not duplicate that work here.
4. Complete and validate the already requested compact top-level help and JSON
   help (`gwz-cli/src/help.rs`); regenerate its docs. This is CLI presentation,
   so parity does not require inventing a Python CLI-help feature.

Before cutting: verify those bounded cases, supported-platform builds and
protocol/driver compatibility; record any remaining failures explicitly. Do not
hold the release for Gemma to become reliable, broader context/factory migrations,
compiler-mutation tests, or another full model experiment. A subsequent fixed-
binary usability comparison is follow-up evidence, not a release prerequisite.

Use the existing release scripts: core must finish successfully first, then CLI
and Python can release in parallel. Each driver release version must have its
corresponding core tag. Pick the next unused version at release time; if a driver
requires a new version after a failed release, cut the matching core version too.
Scripts own tagging/pushing/release orchestration; do not duplicate those actions.
This document scopes the intended release; it does not launch one.

## Delivery and validation

1. Update behavior authority in `gwz-core/dev-docs/GWZDesign.md` and
   `GWZRequirements.md` for the path and response contracts before implementation.
2. Land explicit caller-context preparation/transport, absolute listings and
   routing together; then
   diagnostics; then comparison/reason reporting. Keep each step reviewable.
3. Start with focused failing tests. Use pure path tests and injected fake
   services for decision cases; run a small native Windows regression on Dabeest
   for path prefixes, actual cwd usage and add/diff behavior. Check CLI/Python
   response parity and protocol compatibility. No compiler-mutation suite.
4. Update `gwz-cli/docs/commands/ls.md`, `status.md`, and relevant repo docs.
   Regenerate `gwz-cli/docs/CLI.md` after help changes. The compact/JSON help work
   already parked in `GwzHelpCheckpoint-2026-09-10.md` is related but unfinished;
   coordinate edits rather than overwriting it or treating it as validated.
5. Replay the exact offending command sequences deterministically and complete
   the driver parity matrix. After the release fixes, a separately scheduled model
   comparison may hold the skill fixed and compare binaries; distinguish correct
   outcomes from clean termination. This does not gate the release.

Evidence: `E:/gwz-agent-eval/runs/skill-json-first-v1`, especially
`repeat-1/P2/qwen3.8-P2-B`; relative-root staging failures are also recorded in
`skill-compact-v2/repeat-2/P4/gemma4-P4-B/conversation.json`.

Out of scope: correcting the models' general editing/reasoning failures or adding
generic verification prose to skills. NTFS implementation remains owned by its
assigned agent, but its completed fix is included in the release bundle.
No release, commit or push is performed by this planning update.
