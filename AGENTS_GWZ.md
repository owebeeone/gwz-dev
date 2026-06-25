<!-- gwz-managed-file: sha256=f153fd3108df3a9b0cb04f8f2f93b9e1685df7e3247b1d9ec2bf0861cbae66aa -->

# GWZ Workspace

This repository is managed by GWZ, a multi-repository workspace tool.

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
- https://github.com/owebeeone/gwz-cli/tree/main/docs
