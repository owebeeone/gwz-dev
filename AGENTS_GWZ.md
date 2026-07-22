<!-- gwz-managed-file: sha256=1377a97512381fb61757d4387293d833b2c57329b37b6c3bb60924dc319d20e8 -->

# GWZ Workspace

This repository is managed by GWZ, a multi-repository workspace tool.

For workspace-wide status, staging, and commits, use `gwz status`, `gwz add`,
and `gwz commit`. Do not substitute per-repository Git loops.

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
