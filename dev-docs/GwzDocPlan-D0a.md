# GWZ Documentation D0a Source Freeze

Date: 2026-06-25

This note records the source-of-truth freeze used to implement the v0.3.0
documentation wave from `GwzDocPlan.md`.

## Scope

Document the current implemented surface only. Branch and stash command docs are
out of scope for this wave.

## CLI Surface

Confirmed from `cargo run -q -p gwz -- --help`:

- `init`: create a workspace or initialize one from source URLs.
- `clone`: clone a workspace root and materialize members.
- `add`: stage file contents across workspace repos.
- `repo`: manage workspace repositories with `repo add` and `repo create`.
- `status`: show workspace Git status.
- `ls`: list workspace members.
- `forall`: run a command in materialized members.
- `snapshot`: record the current workspace selection.
- `tag`: manage real Git tags across workspace repos.
- `materialize`: materialize workspace members to lock, head, snapshot, tag, or
  commit target.
- `pull`: update workspace members to an explicit target.
- `push`: push workspace member refs.
- `capture`: record live worktree state into the lock.
- `commit`: commit staged changes across members and the workspace root.

The local binary currently reports `gwz 0.2.0-dev`; this documentation wave
still uses the plan's v0.3.0 baseline name for the implemented command surface.

Global root behavior: `--root <path>` exists, but the default is to use the
current directory and discover the workspace root upward where handlers accept a
workspace reference. User docs should mention `--root` once and otherwise rely
on default discovery, like Git.

The root help now advertises
`https://github.com/owebeeone/gwz-cli/tree/main/docs` near the top and at the
end.

## Implemented Feature Status

- Tags are real Git refs at `refs/tags/<name>`, not `gwz.conf/tags` artifacts.
- Local tag create/list/delete spans selected members plus the committed
  workspace root.
- Remote tag push/fetch/list/delete spans members only.
- `materialize --tag <name>` checks out each tagged member at
  `refs/tags/<name>` and skips untagged members for default selection.
- `gwz ls` is a read-only core operation backed by manifest and lock data.
- `gwz forall` is CLI-local. It uses member listing to select materialized
  members, runs sequentially, sets `GWZ_MEMBER_ID`, `GWZ_MEMBER_PATH`,
  `GWZ_MEMBER_ABSPATH`, and `GWZ_ROOT`, and rejects `--json`/`--jsonl` in v0.
- Branch and stash are not implemented command surfaces for this documentation
  wave.
- `gwz init --update` and automatic `AGENTS_GWZ.md` generation/update behavior
  were not found in current help or repository search. D1 docs should describe
  root-only refresh and managed-file digest overwrite behavior as planned/spec
  behavior, not as an available v0.3.0 command.

## Artifacts

Current tracked workspace artifacts:

- `gwz.conf/gwz.yml`: workspace manifest, schema `gwz.workspace/v0`.
- `gwz.conf/gwz.lock.yml`: lock file, schema `gwz.lock/v0`.
- `gwz.conf/snapshots/*.yaml`: snapshots, schema `gwz.snapshot/v0`.

Current root workspace members from `gwz ls --local`:

- `gwz-cli`
- `gwz-core`
- `taut`

The root Cargo workspace currently includes `gwz-core` and `gwz-cli`; `taut` is
a GWZ-managed member but not a root Cargo workspace member.

Removed historical artifact:

- `gwz.conf/tags`: do not document as live behavior.

## Protocol Sources

Protocol source of truth:

- `gwz-core/protocol/gwz.taut.py`

Core generated output and corpus:

- `gwz-core/src/protocol/generated.rs`
- `gwz-core/protocol/corpus/golden.json`
- `gwz-core/protocol/corpus/rust/vectors.rs`

Important catalog distinction: `ExecMode`, `ExecRequest`, `ExecResponse`, and
`ExecResult` are CLI-local values used by `gwz forall`. They are schema values
but not `GwzCore` service methods and have no core service handler.

## Bootstrap Agent File

`AGENTS_GWZ.md` is root-only and tracked. It is a deliberately brief bootstrap
hint for agents that clone a GWZ workspace root.

The future release raw URL is:

`https://raw.githubusercontent.com/owebeeone/gwz-cli/refs/heads/release/docs/agent_bootstrap.md`

That URL is expected to return 404 until the docs/release branch is pushed.
Implementation should not block on the URL resolving before publication.
