# GWZ Local Clone Post-Create Script Proposal

Status: draft proposal, not implemented or approved as a GWZ feature.

## 1. Goal

A developer or agent should create a working lane with one command. They
should not need to activate an environment, repair editable installs, fix
launchers, or remember a sequence of post-copy commands.

The intended workflow is:

```text
clone -> verify repository independence -> prepare destination -> verify usability
```

For a workspace enrolled in this workflow, successful completion means that
its declared development tasks can run in the destination without accidentally
using source-workspace environments or project packages.

This is not a promise that arbitrary copied applications are relocatable.
The workspace defines its preparation and acceptance checks; GWZ orchestrates
them and reports the result honestly.

## 2. Motivation And Evidence

The Pyrolyze workspace exposed a difference between a complete local copy and
a usable development environment:

- GWZ verified the copied repositories and their independent object stores.
- The copied Python environment initially imported Astichi, YIDL,
  YIDL-lifecycle, and Pyrolyze from the source workspace.
- Editable-install `.pth` registrations contained absolute source paths.
- Reinstalling those four packages against the destination corrected their
  import origins.
- A copied `pytest` launcher still invoked the source environment's Python.
  Correct imports through the destination Python therefore did not prove that
  all copied commands were isolated.

These were environment-relocation problems, not incorrect Git remotes or
source-code imports. The separate large-blob connectivity fix makes the Git
copy verifiable; it does not repair runtime environments.

Python accepts relative entries in `.pth` files, relative to the containing
site-packages directory. Some workspace registrations already use that form.
However, editable-install tooling can generate absolute registrations, and
launchers, activation scripts, native artifacts, and editor configuration can
also embed paths. Changing `.pth` entries alone is not a complete solution.

## 3. Existing Contract Must Remain Clear

The current local-clone contract is documented in:

- [Local Clone Design](GwzLocalCloneDesign.md), especially sections 3 and 4.
- [Local Clone Library Boundaries](GwzLocalCloneLibraryBoundaries.md).
- [Released Local Clone Documentation](../gwz-cli/docs/LocalClones.md).

Existing family `ready` means that copy, Git independence, configuration, and
destination-connectivity checks completed. It does not mean that project
environments were rebuilt or application tests passed.

Do not silently redefine that state, mark a complete Git copy as an incomplete
copy because preparation failed, or claim that current GWZ already has a
post-create preparation hook.

Keep two results:

| Result | Meaning |
| --- | --- |
| Copy ready | Existing GWZ local-clone checks passed. |
| Work ready | The declared preparation and usability checks passed. |

Work readiness is scoped to the declared checks and the inputs observed for
that preparation attempt. It is not certification of every possible task or
of future edits in the lane.

## 4. Recommended Practice

### 4.1 Workspace-Owned Preparation

Each participating workspace provides a version-controlled, repeatable setup
script and its dependency inputs. The script knows which environments,
packages, native components, and commands that workspace needs.

For example, a Python workspace could own:

```text
dev-setup/
    create_local_lane.py
    prepare_local_lane.py
    verify_local_lane.py
    local-clone.toml
    requirements.lock
```

These are proposed files, not files assumed to exist today. The bootstrap
scripts should use only the standard library until they have created the
destination environment.

The scripts derive all workspace paths from the destination root. Committed
recipes contain repository-relative paths, not a developer's filesystem path.

### 4.2 One Command, No Manual Activation

The first implementation should be a workspace wrapper around existing GWZ:

```sh
# Proposed workspace helper, not an existing GWZ command.
python3 dev-setup/create_local_lane.py codecache ../workspace-codecache
```

The wrapper invokes `gwz local clone`, waits for its successful copy result,
then runs the destination's preparer and verifier. It must obtain the actual
destination from a supported structured result or explicitly supply and check
the destination; it must not scrape human-readable output.

Its bootstrap interpreter is an explicitly selected host tool, not a copied
environment executable. Source-side orchestration may use the source's
existing tools to perform the copy; destination preparation and acceptance
must not depend on its Python environment.

The eventual user experience for an opted-in workspace should be ordinary
`gwz local clone`: the registered post-create script runs automatically.
The wrapper proves the practice before adding that GWZ feature.

### 4.3 Recreate Environments, Do Not Guess At Relocation

Default to recreating declared Python environments at their destination paths.
Do not attempt a global source-path substitution across the copied tree.

In the destination only:

1. Check that each environment path is declared, generated, untracked, and
   inside the destination. Refuse symlink escapes or ambiguous ownership.
2. Replace the copied environment with a fresh environment using the selected
   Python version. Bootstrap must not execute its copied installers first.
3. Install the locked third-party dependencies.
4. Install each declared workspace Python package in editable mode from the
   destination, including regenerated command entry points.
5. Install or rebuild native components as required, then exercise them.
6. Generate destination-local editor/interpreter settings where needed.
7. Run the acceptance checks before reporting work readiness.

Replacement is permitted only for declared reproducible output, not arbitrary
directories named `.venv` or `target`. Do not destroy tracked files or unique
untracked source work hidden inside an environment directory. If ownership or
reproducibility is uncertain, preserve it and fail with a diagnostic.

The source remains unchanged. The script must not stash, reset, commit, push,
switch branches, or modify workspace membership as an environment-setup step.

## 5. Reproducible Inputs And Performance

The observed workspace has dependency requirements but no complete preparation
recipe that proves repeatable reconstruction. Establish that recipe before
calling the process automatic or reproducible.

The Python recipe should declare:

- Supported Python version and environment locations.
- Exact third-party versions and the project's chosen lock/integrity format.
- Editable workspace package paths separately from third-party dependencies.
- Native build and smoke-test requirements.
- Host-tool prerequisites and acceptable versions.
- Whether dependency downloads are permitted for this workflow.

A pinned requirements lock with integrity checks is sufficient for a pip/uv
workspace; a workspace already using `uv.lock` should use that established
mechanism instead. Do not introduce two competing dependency authorities.

Use package download caches and immutable, compatible wheel caches to avoid
repeated downloads and unnecessary builds. Do not share a writable virtual
environment between lanes. Do not enable source-workspace build outputs as an
implicit fallback when a rebuild fails.

An external base Python installation and ordinary tool/package caches may be
shared intentionally. The isolation requirement concerns lane-specific project
code, installed environment state, and mutable build products, not duplication
of every system tool.

Preparation cost should be measured separately from copy/verification cost.
Report cold-cache and warm-cache timings, with the environments and acceptance
checks identified. No time target is claimed by this proposal.

## 6. Proposed Automatic Hook Contract

This section describes a future GWZ feature, not the existing CLI or schema.
Implementation requires an amendment to the authoritative design and request/
response contracts before code is changed.

### 6.1 Registration And Trust

The workspace explicitly registers one post-create preparation recipe through
a GWZ-managed configuration command. Exact command and schema spelling remain
for the feature design; never hand-edit `gwz.conf/` to install this proposal.

Do not execute a script merely because a file with a conventional name exists.
Enrollment authorizes automatic execution for this local workspace. A newly
downloaded workspace does not silently inherit executable-hook trust.

This executes project code and possibly dependency build code; it is not a
sandbox. Registration must explain that fact and identify the recipe. A change
to the registered command or trust boundary requires explicit acceptance, not
silent execution of a different command.

### 6.2 Invocation

The recipe supplies an argument vector, not a shell command string. Avoid shell
interpolation of lane names and paths. Platform-specific argument vectors may
be explicitly declared when a single portable command is insufficient.

For illustration, not as an implemented configuration schema:

```toml
command = ["uv", "run", "--no-project", "--isolated", "--python", "3.12",
           "python", "dev-setup/prepare_local_lane.py"]
verify_command = ["uv", "run", "--no-project", "--isolated", "--python", "3.12",
                  "python", "dev-setup/verify_local_lane.py"]
```

The declared host tools must be available independently of copied environments.
The example's exact bootstrap/tool options must be validated by the workspace
implementation; they are not a universal Python policy.

Invocation rules:

- Working directory is the canonical destination root.
- The script and its recipe are read from the copied destination. Reject an
  entry-script path or symlink resolution that escapes that boundary.
- Source root, destination root, lane name, and preparation-attempt identity
  are supplied as structured invocation data or dedicated environment values.
  Runtime absolute paths are legitimate here; do not commit them into recipes.
- Remove inherited environment redirection such as `VIRTUAL_ENV`,
  `PYTHONPATH`, and `PYTHONHOME`. Select a host-tool search path explicitly,
  excluding source and copied environment launchers. Account for each supported
  platform's path comparison rules.
- Scripts explicitly select the destination interpreter for package and test
  commands. They must not rely on ambient activation.
- Capture exit status, stage, and useful diagnostics. Bound retained output and
  avoid recording credentials or the caller's complete environment.

Shared caches and network policy are declared setup choices. Passing a policy
to a trusted preparer is not an operating-system enforcement boundary.

### 6.3 Ordering And Ownership

Preflight the registration, trust, and bootstrap-tool availability before
allocating the copy. If copy or repository verification fails, do not run setup.

The post-create script runs after GWZ publishes the complete copy and releases
the family lock. Do not hold that lock during dependency downloads, builds, or
tests. Preparation must not modify the source observations used to certify the
copy; existing source-quiescence requirements still apply during copying.

Use destination-scoped ownership to prevent simultaneous preparation, disposal,
or an incompatible operation on that destination. Do not invent an unchecked
side-file lock or manually write GWZ family metadata. The core design must
define this interaction before integration; independent sibling lanes should
not remain blocked while one lane installs dependencies.

GWZ owns registration, invocation boundaries, result publication, and lifecycle
coordination. The workspace owns environment construction and project checks.
GWZ should not acquire a Python packaging engine or a universal path-rewriter.

## 7. Results, Failure, And Retry

Keep the existing copy result separate from preparation state:

| Preparation state | Meaning |
| --- | --- |
| Not configured | Raw copy only; no work-readiness claim. |
| Pending | Copy succeeded; preparation has not completed. |
| Running | An identified attempt owns preparation. |
| Succeeded | Preparation and declared verification passed. |
| Failed | A stage failed; copy and diagnostics are retained. |
| Unknown/interrupted | Completion cannot be established; not work ready. |

For enrolled workspaces, the combined command returns success only after
verification. A nonzero exit, failed verification, timeout, cancellation, or
interruption must not print an unqualified "working lane created" result.
It must identify that the Git copy remains complete and which preparation
stage did not complete.

Do not automatically delete the destination or undo copied source edits after
setup failure. Do not repeat the clone into an existing destination to retry.
Retry preparation on the same complete lane through an explicit entry point;
the workspace script must tolerate partial previous output and perform fresh
checks before declaring success.

Initially, the wrapper can keep its report in ignored
`scratch/lane-preparation/`. A future integrated GWZ implementation owns its
runtime records through normal GWZ APIs; scripts do not write `.gwz/` or
`gwz.conf/` directly.

A preparation report should record recipe/lock input identities, selected tool
versions, declared environments, stage results, and verification scope. Reusing
a successful report requires rechecking relevant inputs and the destination;
a copied source report or an old report alone is never authority.

A future copy-only escape hatch must explicitly report skipped preparation.
It must not be a silent fallback when setup fails. Existing unenrolled raw
clone behavior remains compatible.

## 8. Acceptance Checks For A Python Workspace

Checks must exercise ordinary developer entry points, not just one manually
corrected import command:

1. Each declared environment's Python reports the expected destination prefix.
2. Every declared editable project package resolves under its corresponding
   destination member. Do not equate a successful import with correct origin.
3. The package entry points and test launchers select the destination environment
   on each supported platform. Exercise them directly where practical.
4. Tests launched through the destination Python and ordinary launchers both
   use the declared destination package set.
5. Native components import and perform their required smoke operation. Record
   which backend ran; a silent fallback is not evidence of a working native build.
6. Ordinary use from a neutral working directory, with no source activation,
   succeeds through explicit destination entry points.
7. No declared project package, environment launcher, or mutable build product
   depends on the source tree. Static checks supplement runtime checks; do not
   scan and rewrite every occurrence of the source path in arbitrary data.
8. Source tracked files, environment registrations, and repository state remain
   unchanged by preparation.

The canonical automated fixture should place source and destination in separate
temporary trees, with distinguishable project values. After preparing the lane,
make the fixture source unavailable and run the destination's declared smoke
commands again. This catches dependence missed by origin-only checks without
renaming or disrupting a real developer's active workspace.

## 9. Verification Matrix

Use a canonical local-clone/preparation fixture for success paths. Keep bespoke
tests for lifecycle mechanics, failures, and diagnostics.

| Case | Required result |
| --- | --- |
| Source editable installs embed absolute paths | Destination imports copied project code. |
| Copied launcher invokes source Python | Rebuilt launcher uses destination environment. |
| Relative `.pth` registrations | Remain valid; no unnecessary rewriting is required. |
| Source environment activated / `PYTHONPATH` set | Destination setup and verification do not inherit project redirection. |
| Source becomes unavailable after fixture setup | Declared destination tasks still run. |
| Spaces and non-ASCII characters in destination path | Argument-vector invocation and path checks work. |
| Supported macOS, Linux, and Windows layouts | Native launchers and environment behavior are verified. |
| Missing host tool or untrusted recipe | Fail before copying where the problem is known at preflight. |
| Git copy/connectivity failure | Hook is not called. |
| Install, build, or smoke-test failure | Nonzero combined result; preserve complete copy and useful diagnostics. |
| Interrupted setup followed by retry | No second clone; converge safely or report the remaining blocker. |
| Environment path is tracked, unique, or escapes through a symlink | Refuse destructive replacement. |
| Two preparation attempts / concurrent disposal | Destination ownership prevents conflicting operations. |
| No registered hook | Existing copy-only semantics; no implicit execution. |
| Inherited source preparation report | Never accepted as destination verification. |
| Copied staged, unstaged, and untracked application edits | Preserved; setup does not reset or stash them. |

Cold and warm preparation timings belong in a concise product report. Raw
campaign evidence follows [EVIDENCE.md](../EVIDENCE.md) and must not become a
public CI dependency.

## 10. Delivery Checkpoints

### P0. Contract And Workspace Recipe

Inventory environments, editable packages, host tools, launchers, native builds,
and dependency inputs. Define the scoped work-readiness checks and lock format.
Review this proposal's separation of copy readiness and environment readiness.

Exit: agreed recipe and acceptance fixture; no generic GWZ feature added yet.

### P1. Standalone Preparation And Verification

Implement the workspace-owned preparer and verifier. Prove them against a
copied environment with stale editable paths and a stale launcher. Preserve
source state and support retry of a partly prepared destination.

Exit: the canonical fixture runs while its source tree is unavailable.

### P2. One-Command Workspace Wrapper

Compose existing GWZ local clone with the destination preparer/verifier. Report
copy and preparation separately, retaining failures. Use a supported structured
copy result, not output scraping. No changes to managed GWZ configuration.

Exit: a single command creates a usable lane without manual activation or repair.

### P3. GWZ Hook Design And Enrollment

Amend authoritative design and protocol definitions. Specify the registration
command, trust policy, argument-vector contract, destination ownership,
structured preparation result, retry operation, and compatibility behavior.
Use existing lifecycle/session machinery rather than a parallel state engine.

Exit: reviewed contract before core/CLI implementation. Keep this out of the
separate large-blob connectivity patch.

### P4. Automatic Integration And Qualification

Implement the accepted hook so an enrolled workspace's ordinary local clone
performs preparation automatically. Validate platform behavior, interruption,
concurrency, source preservation, and warm-cache cost. Update public docs and
agent-facing lane instructions with exact supported commands.

Exit: no wrapper-specific manual step remains for enrolled workspaces, and
copy-only or failed preparation is never presented as work ready.

## 11. Non-Goals

- Making every copied application or arbitrary directory relocatable.
- Blind rewriting of absolute paths across code, data, logs, or Git metadata.
- Making virtual environments themselves a portable binary format.
- Automatically deleting incomplete or failed lanes.
- Hiding missing dependencies with source-workspace imports or backend fallback.
- Replacing the existing local-clone connectivity checks or family-state model.
- Implicit hooks for remote clones, bare destinations, merge, or disposal.
- Changing Python packaging tools to emit different editable-install formats.
- Committing, pushing, or implementing this proposal as part of documenting it.

## 12. Recommendation

Adopt the workspace-owned preparer and one-command wrapper first. They give
the requested automatic workflow without adding guessed relocation rules to
GWZ. Once proven, integrate it as an explicitly enrolled post-create hook.

The invariant is simple: an opted-in workflow must either deliver a verified
usable destination or clearly report preparation failure while preserving the
copy. It must never accidentally run against the source workspace and call
that success.
