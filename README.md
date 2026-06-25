# gwz-dev

`gwz-dev` is the development super-workspace for GWZ. It contains the primary
CLI, the embeddable core library, the taut protocol tooling used by GWZ, and
the tracked `gwz.conf/` metadata that lets GWZ dogfood its own multi-repository
workspace model.

## Repository Map

- `gwz-cli/`: primary `gwz` binary and user command surface.
- `gwz-core/`: embeddable Rust engine, workspace artifacts, Git backend, and
  protocol types.
- `taut/`: schema, compiler, and protocol tooling used by GWZ.
- `gwz.conf/`: tracked GWZ root workspace metadata, including the manifest,
  lock, and snapshots.
- `dev-docs/`: root-level planning documents.

## Bootstrap

Install `gwz` from the release installers described in
`gwz-cli/README.md`, or install from source:

```sh
cargo install --git https://github.com/owebeeone/gwz-cli
```

When hacking in this checkout, run the local CLI through Cargo:

```sh
cargo run -q -p gwz -- --help
cargo run -q -p gwz -- ls --local
```

Use the normal Rust checks before sharing changes:

```sh
cargo fmt
cargo test
```

Package-specific tests may also live under `gwz-cli/`, `gwz-core/`, or `taut/`.

## Root Workspace Workflow

List the materialized members:

```sh
gwz ls --local
```

Inspect workspace state before mutating repositories:

```sh
gwz status
gwz status --porcelain
```

Stage and commit through GWZ when a change crosses repository boundaries:

```sh
gwz add <pathspec>...
gwz add -A
gwz commit -m "message"
```

Record and restore coordinated workspace states:

```sh
gwz capture
gwz snapshot <name>
gwz materialize --lock
gwz materialize --snapshot <name>
gwz materialize --tag <name>
```

Move members forward or publish member refs:

```sh
gwz pull --head
gwz push
```

Manage release markers with real Git tags across the selected members:

```sh
gwz tag <name>
gwz tag --list
gwz tag --push <name>
gwz tag --fetch
```

Run member-local maintenance commands from the root:

```sh
gwz forall gwz-cli gwz-core -- cargo test
gwz forall --no-banner -c "git status --short"
```

Add existing repositories or create new members:

```sh
gwz repo add <repo-path>
gwz repo create <member-path>
```

## Documentation Map

- `AGENTS_GWZ.md` is the brief GWZ-managed bootstrap hint for LLM agents when
  present in a workspace root.
- `gwz-cli/README.md` is the concise CLI entrypoint; fuller user-facing pages
  live under `gwz-cli/docs/`.
- `gwz-core/docs/` covers the Rust API, workspace artifacts, protocol, and
  message catalog work for the core crate.
- `taut/docs/` documents taut itself.
- `dev-docs/` records planning context and historical design notes.

## Feature Status

This documentation wave targets the implemented v0.3.0 command surface. Git tag
management is implemented as real Git refs, `gwz ls` is implemented for member
listing, and `gwz forall` is implemented for member-local command execution.

Branch and stash workflows remain future work for this documentation wave. Do
not treat branch or stash planning documents as available `gwz` commands unless
the CLI help grows those commands.
