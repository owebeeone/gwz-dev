# GWZ Python CLI Parity Issues

Status: updated after the 2026-06-28 Python CLI parity pass

Observed on 2026-06-28 from `/Users/owebeeone/limbo/gwz-dev`.

Compared commands:

- Rust CLI: `/Users/owebeeone/.cargo/bin/gwz`, reported `gwz 0.6.0`.
- Python CLI: `/Users/owebeeone/limbo/gwz-dev/.venv/bin/gwz-py`, package
  metadata `gwz==0.7.4`.

Version skew is context, not an excuse for the Python bugs below. `gwz-py`
ships as the Python implementation of the CLI and should intentionally match
the Rust CLI behavior unless a documented compatibility exception exists.

## Summary

`gwz-py` is closer to CLI parity, but not complete.

Resolved in the current source tree:

1. Human rendering for the visible read-only commands now covers `status`, `ls`,
   `branch`, and `stash list` instead of collapsing them to `ok`.
2. `gwz-py snapshot` without a name now lists snapshots, matching Rust `gwz`.
3. `gwz-py` accepts Rust global flags `--jsonl`, `--ssh-timeout`, and
   `-V`/`--version`.
4. `gwz-py status --porcelain` is accepted and renders porcelain status.
5. The public Python command is `add`; the old top-level `stage` command was
   removed from the parser.
6. Dirty `gwz-py status` now renders the attached status response and exits `0`,
   matching Rust `gwz status`.

Remaining high-impact gaps:

1. JSON output shape is not parity-compatible. Rust emits the CLI JSON contract
   directly; Python still emits raw taut dataclass structure with different
   wrapper names and lower-case enum names.
2. `--jsonl` currently falls back to one JSON response for non-streaming
   commands; full Rust-compatible streaming JSONL is still needed.
3. `--ssh-timeout` is accepted for parser parity, but the generated
   `OperationPolicy` has no timeout field yet, so core-level timeout behavior
   still needs a protocol/API decision.
4. The top-level `help` command and mutation command output fixtures still need
   parity work.

Likely code concentration:

- `gwz-py/src/gwz/cli_render.py`: owns current human renderers and still needs
  a Rust-compatible JSON renderer.
- `gwz-py/src/gwz/cli_shared.py`: owns global parser parity. `--ssh-timeout` is
  parser-only until the protocol exposes timeout policy.
- `gwz-py/src/gwz/cli_read.py`: owns status/ls parser options and validation.
- `gwz-py/src/gwz/cli_mutation.py`: `init` misses `--update`; `pull`,
  `materialize`, and other mutations need output/JSON parity checks.
- `gwz-py/src/gwz/cli_local.py`: owns `forall` command shape and local command
  output parity.

## Global CLI Surface

| Area | Rust `gwz` | Python `gwz-py` | Parity issue |
| --- | --- | --- | --- |
| Version | `gwz -V`, `gwz --version` | supported | Fixed in current source tree. |
| Help command | `gwz help` and `gwz help <command>` | argparse `-h` only; no top-level `help` command observed | Add `help` command or document a deliberate exception. |
| Streaming JSON | global `--jsonl` | accepted; non-streaming fallback emits one JSON response | Still needs full streaming JSONL contract. |
| SSH timeout | global `--ssh-timeout <secs>` | accepted but not plumbed into `OperationPolicy` | Requires protocol/API support before core behavior can match Rust. |
| Command names | `init clone add repo status ls forall snapshot tag branch stash materialize pull push capture commit` | aligned; old public `stage` removed | Fixed in current source tree. |
| JSON formatting | Rust CLI contract with `kind`, PascalCase enum strings such as `Ok`/`Status` | raw Python dataclass/asdict shape, lower-case enum names such as `ok`/`status` | Script-facing JSON is not compatible. |

## Reproduced Behavior Bugs

### `status`

Rust:

```text
$ gwz status
On branch main
```

Current Python source:

```text
$ gwz-py status
On branch main
```

Resolved:

- Human renderer matches the Rust clean and dirty status shape for the current
  workspace.
- `gwz-py status --porcelain` is accepted and renders porcelain status.
- Dirty status responses render and exit `0`, matching Rust `gwz status`.

Remaining:

- `gwz-py --json status` is not JSON-shape compatible. Rust emits a top-level
  response with `kind: "response"` and enum strings such as `Ok`; Python emits
  raw dataclass fields under `response` with lower-case enum strings.

Priority: P0 for JSON contract; human output fixed.

### `ls`

Rust:

```text
$ gwz ls
/Users/owebeeone/limbo/gwz-dev/gwz-cli
/Users/owebeeone/limbo/gwz-dev/gwz-core
/Users/owebeeone/limbo/gwz-dev/taut
/Users/owebeeone/limbo/gwz-dev/gwz-py

$ gwz ls --local
gwz-cli
gwz-core
taut
gwz-py
```

Current Python source:

```text
$ gwz-py ls
/Users/owebeeone/limbo/gwz-dev/gwz-cli
/Users/owebeeone/limbo/gwz-dev/gwz-core
/Users/owebeeone/limbo/gwz-dev/taut
/Users/owebeeone/limbo/gwz-dev/gwz-py

$ gwz-py ls --local
gwz-cli
gwz-core
taut
gwz-py
```

Resolved:

- Human renderer prints workspace target paths.
- `--local` now switches to workspace-relative paths.

Remaining:

- `gwz-py --json ls` uses `{"members": [...], "response": ...}` while Rust uses
  `{"kind": "members", "entries": [...]}`.
- Python has an extra `--materialized-only` flag that Rust help does not show.
  Keep only if documented as a Python extension.

Priority: P0 for JSON contract; human output fixed.

### `snapshot`

Rust:

```text
$ gwz snapshot
3 snapshots:
  phase3-start-py-build  unix-ms:1782396621826  unknown  (4 members)
  start-py-build         unix-ms:1782392507520  unknown  (4 members)
  target-selection-start unix-ms:1782568756224  unknown  (4 members)
```

Current Python source:

```text
$ gwz-py snapshot
3 snapshots:
  phase3-start-py-build  unix-ms:1782396621826  unknown  (4 members)
  start-py-build         unix-ms:1782392507520  unknown  (4 members)
  target-selection-start unix-ms:1782568756224  unknown  (4 members)
```

Resolved:

- Omitted `name` lists snapshots, matching Rust.

Remaining:

- JSON listing has the same Python/Rust shape risk as other commands and should
  be checked against the Rust CLI contract.

Priority: P0 for JSON contract; default human dispatch fixed.

### `branch`

Rust:

```text
$ gwz branch
status: Ok
mem_gwz_cli gwz-cli Listed main <sha>
...
```

Current Python source:

```text
$ gwz-py branch
status: Ok
mem_gwz_cli gwz-cli Listed main <sha>
...
```

Resolved:

- Default branch list operation renders branch rows instead of aggregate status.

Remaining:

- `gwz-py --json branch` returns the branch records, but the JSON contract is
  not Rust-compatible: Python uses `repos` and lower-case enum values while Rust
  uses `branch_repos` and PascalCase enum values.

Priority: P1 for JSON contract; human output fixed.

### `stash`

Rust:

```text
$ gwz stash list
status: Ok
```

Current Python source:

```text
$ gwz-py stash list
status: Ok
```

Resolved:

- Empty stash list renders `status: Ok`, matching Rust.

Remaining:

- Non-empty stash lists and `--expanded` need fixture coverage because current
  renderer has no stash-specific human output.
- JSON shape differs: Rust emits `stash_bundles`; Python emits `bundles`.

Priority: P1 for expanded/non-empty fixtures and JSON contract; empty human
list fixed.

### Global Flags In Live Commands

Rust:

```text
$ gwz --jsonl status
{...}

$ gwz --ssh-timeout 3 status
On branch main
```

Current Python source:

```text
$ gwz-py --jsonl status
{...}

$ gwz-py --ssh-timeout 3 status
On branch main
```

Resolved:

- `--jsonl` is accepted by the parser.
- `--ssh-timeout` is accepted by the parser.

Remaining:

- `--jsonl` still needs full Rust-compatible streaming records.
- `--ssh-timeout` still needs core/protocol plumbing.

Priority: P1 for streaming JSONL and SSH/network behavior.

## Command-by-Command Inventory

| Command | Rust behavior | Python behavior | Known parity issues |
| --- | --- | --- | --- |
| `init` | Creates workspace or initializes from source URLs; supports `--update` to refresh root bootstrap files. | Parser supports URLs and `--path`; no `--update`. | Missing `--update`; human/JSON output needs mutation fixture comparison. |
| `clone` | Clones workspace root and materializes members; has Rust help examples and network globals. | Parser shape mostly matches positional URL/directory; clone progress exists from package smoke. | Verify human progress and JSONL parity with Rust; `--ssh-timeout` still needs core plumbing. |
| `add` | Multi-repo git add; Rust top-level command. | Present; old public top-level `stage` removed. | Check `add` output parity. |
| `repo` | Nested `add`, `create`, `sync`; planned `clone` + `detach` (`dev-docs/RepoClonePlan.md`) — clone registers an active member from a URL; detach marks a member inactive (keeps the `gwz.yml` row; drops lock entry; leaves snapshots/markers/disk alone). | Nested `add`, `create`, `sync` today; **`clone` and `detach` must ship with Rust** in the same plan/release train. | Parser appears broadly aligned for existing subcommands; mutation output and JSON shape still need fixtures. Track `repo clone` / `repo detach` as same-plan parity, not later cleanup. |
| `status` | Human status and `--porcelain`; JSON contract. | Human and porcelain output now match the current workspace; dirty status exits `0`. | JSON shape still differs. |
| `ls` | Prints absolute paths by default; `--local` prints workspace-relative paths. | Human output and `--local` now match; extra `--materialized-only` remains. | JSON shape still differs; document or remove extra flag. |
| `forall` | Supports `[projects...] -- <cmd>` and `-c, --command-string <string>`. | Supports token form, `-c`, and `--command-string`. | Compare banner/exit aggregation/env vars. |
| `snapshot` | No name or `--list` lists snapshots; name records snapshot; `--branch` optional value. | No name and `--list` now list snapshots; recording path exists. | Verify JSON shape. |
| `tag` | No name or `--list` lists; supports create/delete/push/fetch, `-m`, `-s`, remote. | Observed no-name list matches human output in this workspace. | Parser seems close; JSON shape should be checked; help differs and Python includes long `--message` where Rust only advertises `-m`. |
| `branch` | No flag defaults to list and renders branch rows. | No flag defaults to list and renders branch rows. | JSON shape still differs. |
| `stash` | Nested push/list/apply/pop/drop; list renders `status: Ok`; `list --expanded` supported. | Nested commands exist; empty list renders `status: Ok`; `--expanded` exists on nested list. | JSON shape still differs; non-empty stash fixture needed. |
| `materialize` | Default lock target; supports `--lock`, `--head`, `--snapshot`, `--tag`, `--switch`. | Parser shape appears aligned. | Streaming/human output and JSONL parity need fixtures; `--ssh-timeout` still needs core plumbing. |
| `pull` | Pulls heads or snapshot; network globals matter. | Parser supports `--head`/`--snapshot`. | Streaming/human output and JSONL parity need fixtures; `--ssh-timeout` still needs core plumbing. |
| `push` | Pushes selected targets; network globals matter. | Parser has no command-specific flags. | Streaming/human output and JSONL parity need fixtures; `--ssh-timeout` still needs core plumbing. |
| `capture` | Records live state into lock. | Command exists. | Human/JSON output parity needs fixture. |
| `commit` | Requires `-m/--message`; supports `-a/--all`. | Parser matches visible flags. | Mutation output and JSON shape need fixture. |

## Remaining Fix Order

1. Normalize JSON rendering to the Rust CLI contract, not raw generated dataclass
   names. This may belong in a dedicated CLI JSON renderer instead of
   `dataclasses.asdict`.
2. Add parity tests that execute both CLIs against the same fixture workspace:
   - `status`, `status --porcelain`, `--json status`
   - `ls`, `ls --local`, `--json ls`
   - `snapshot`, `snapshot --list`, `--json snapshot`
   - `tag`, `branch`, `stash list`
   - parser acceptance for `--jsonl`, `--ssh-timeout`, and `--version`
3. Implement Rust-compatible streaming `--jsonl` output where Rust emits
   operation events.
4. Decide or implement protocol/API plumbing for `--ssh-timeout`.
5. Add the top-level `help` command or document argparse `-h` as the Python
   exception.
6. Add mutation fixtures in disposable workspaces for `init`, `repo`, `add`,
   `capture`, `commit`, `materialize`, `pull`, `push`, `clone`, and stash
   push/apply/pop/drop.

## Acceptance Criteria

- For every Rust `gwz` command, `gwz-py` accepts the same documented flags unless
  an exception is written down in this document or release notes.
- Human output for read-only commands matches Rust output closely enough for a
  user to swap `gwz` and `gwz-py` without surprise.
- `--json` output uses the Rust CLI JSON contract.
- `--jsonl` is accepted and produces Rust-compatible streaming records where
  Rust does.
- Parity fixtures are pinned in tests so future parser/rendering drift is caught
  before release.
