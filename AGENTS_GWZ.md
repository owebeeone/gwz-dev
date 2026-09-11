<!-- gwz-managed-file: sha256=5484503d08631335b00a1656c5e5d9e23be0a73ff84975caaa1848dab2077e81 -->

# GWZ Workspace

This repository is managed by GWZ, a multi-repository workspace tool.

For workspace-wide status, staging, and commits, use `gwz status`, `gwz add`,
and `gwz commit`. Do not substitute per-repository Git loops.

## Workspace Integrity

All structural changes to the workspace must be performed via the `gwz` CLI.
Direct text-based edits to `gwz.conf/` are strictly forbidden — gwz records a
digest of these files when it writes them, and refuses any structural command
that finds an uncommitted hand edit.

- **Never manually edit `gwz.conf/gwz.yml`, `gwz.conf/gwz.lock.yml`, or anything else under `gwz.conf/`.** These files are the authoritative, system-managed state of the workspace.
- To add, remove, or re-point member repositories, use `gwz repo <add|clone|create|detach|attach|sync>`.
- There is no rename or move verb. To relocate or rename a member, `gwz repo detach` it and then re-add it at the new path or name.
- This ensures `gwz.lock.yml` and `gwz.yml` stay synchronized with the actual filesystem state.
- If you have already hand-edited these files, revert the edit and run the equivalent `gwz` command. To accept the current on-disk state instead, run `gwz init --update --force`.
- Add `--commit` to commit the accepted configuration and updated bootstrap files together; unrelated staged work stays staged.
- Note that `--force` there authorizes two things at once: overwriting a locally edited `AGENTS_GWZ.md` and accepting the current `gwz.conf` state. Use it deliberately.
- **Never manually edit this file.** It is system-managed. Change agent instructions via the `gwz` template and `gwz init --update`.

## Searching a Workspace

Member repositories are listed in the root repository's `.git/info/exclude`, so
any tool that honours git ignore rules walks past every member when run from the
workspace root. `rg pattern .` reports no matches and does not say that members
were skipped.

- **Treat a zero-hit search from the workspace root as inconclusive** until it is repeated with `rg -uu` or run inside a member. A silent skip reads exactly like an absent symbol.
- Search with `rg -uu` from the root, or `cd` into a member and search there.
- `gwz forall -- <cmd>` runs the search once per member, each under its own repository's ignore rules.

Install `gwz` from the latest release:

```sh
curl --proto '=https' --tlsv1.2 -LsSf \
  https://github.com/owebeeone/gwz-cli/releases/latest/download/gwz-installer.sh | sh
```

Or install from source:

```sh
cargo install --git https://github.com/owebeeone/gwz-cli
```

If the workspace is not cloned yet:

```sh
gwz clone <workspace-git-url> [directory]
```

If this root repository is already cloned:

```sh
gwz materialize --lock
gwz status
```

Docs:

- `gwz --help`
- Quick Start: https://owebeeone.github.io/gwz-cli/QuickStart/
- Full documentation: https://owebeeone.github.io/gwz-cli/
