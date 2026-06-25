<!-- gwz-managed-file: sha256=99b2d4844ca1e49417a1a5bfc9c6edb1668dcdd2e94ff883dde3beaebd637340 -->

# GWZ Agent Bootstrap

This repository is managed by GWZ, a multi-repository workspace tool.

Install `gwz` from the GWZ CLI releases:

<https://github.com/owebeeone/gwz-cli/releases>

Or install from source:

```sh
cargo install --git https://github.com/owebeeone/gwz-cli
```

If the workspace is not cloned yet:

```sh
gwz clone <workspace-git-url> [directory]
```

If this repository is already cloned:

```sh
gwz materialize --lock
gwz status
```

Use `gwz --help` for command help. CLI docs live at
<https://github.com/owebeeone/gwz-cli/tree/main/docs>.
