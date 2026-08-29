<!-- gwz-managed-file: sha256=7d8cdef93f99fdcca54a8f36c1e9db813828803761c843273dc1d86cda7e9d53 -->

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
- Note that `--force` there authorizes two things at once: overwriting a locally edited `AGENTS_GWZ.md` and accepting the current `gwz.conf` state. Use it deliberately.
- **Never manually edit this file.** It is system-managed. Change agent instructions via the `gwz` template and `gwz init --update`.

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
