# gwz-dev

`gwz-dev` is the complete checkout for building, testing, and changing GWZ
(Git Workspace Zone). Clone this workspace when you want the Rust CLI, core
engine, Python bindings, Taut tooling, and their coordinated revisions together.

GWZ itself manages the checkout: the root records the member repositories and
their exact revisions while each member remains an ordinary Git repository.

## Get The Development Workspace

Install [`gwz`](https://github.com/owebeeone/gwz-cli#install-and-start), then:

```sh
gwz clone https://github.com/owebeeone/gwz-dev.git gwz-dev
cd gwz-dev
gwz status
cargo test --workspace
```

New to the product? Read the
[Quick Start](https://owebeeone.github.io/gwz-cli/QuickStart/). For the problem
GWZ solves, its fit alongside tools such as vcstool, and its embeddable remote-
capable architecture, read
[Why GWZ](https://github.com/owebeeone/gwz-core/blob/main/docs/WhyGwz.md).

## Repository Map

- `gwz-cli/`: the primary, most thoroughly tested `gwz` terminal interface and
  user documentation.
- `gwz-core/`: the embeddable, message-driven Rust engine used by local or
  remote-capable clients.
- `gwz-py/`: Python bindings to `gwz-core` and a functional Python GWZ CLI.
- `taut/`: schema compiler and protocol tooling used by GWZ.
- `taut-shape/`, `taut-shape-rs/`, and `taut-shape-py/`: shared schema-shape
  implementations.
- `gwz.conf/`: the tracked manifest, lock, snapshots, and markers for this root.
- `dev-docs/`: cross-repository design and planning records.

## Contributor Workflow

Run the local Rust CLI from the workspace:

```sh
cargo run -q -p gwz -- --help
cargo run -q -p gwz -- ls --local
```

Inspect before changing repositories, then stage and commit cross-repository
work through GWZ:

```sh
gwz status
gwz diff
gwz add <pathspec>...
gwz commit -m "message"
```

Run checks in selected members or all members:

```sh
gwz forall gwz-cli gwz-core -- cargo test
gwz forall -- git status --short
```

Use snapshots and dry runs around broad changes:

```sh
gwz snapshot before-change
gwz --dry-run pull --head
gwz pull --head
```

The [root-workspace guide](https://owebeeone.github.io/gwz-cli/RootWorkspace/)
covers coordinated state, tags, selections, and release-oriented workflows.

## Documentation

- [GWZ user documentation](https://owebeeone.github.io/gwz-cli/)
- [`gwz-core` embedding documentation](gwz-core/docs/README.md)
- [`gwz-py` README](gwz-py/README.md)
- [`AGENTS.md`](AGENTS.md), the standard agent entry point for this workspace
- [`AGENTS_GWZ.md`](AGENTS_GWZ.md), the generated agent bootstrap for this
  managed root

## Verification

```sh
cargo fmt --check
cargo test --workspace
python gwz-py/run_tests.py
```
