# Bootstrap a crate name on crates.io

This is the process used on 2026-09-12 to publish `gwz-core` and `gwz` as
`0.0.0-bootstrap.1`, owned by **owebeeone**, through GitHub Actions.

A bootstrap is a real, published prerelease containing documentation only. It
establishes ownership while the production crate's dependencies and release
workflow are prepared. It has **no implementation, public API, or executable**.
It must not be presented as an installable GWZ release.

Crates.io has no separate project-creation or name-reservation operation: the
first publication establishes ownership. Use this for actual projects under
active development. The [crates.io policy](https://crates.io/policies) prohibits
prolonged name reservations without genuine functionality, purpose, or
significant development activity in the corresponding repository.

## Working examples

| Crate name | GitHub repository | Bootstrap commit | Successful publication |
|---|---|---|---|
| `gwz-core` | `owebeeone/gwz-core` | [`4a8bf63`](https://github.com/owebeeone/gwz-core/commit/4a8bf63) | [Actions run](https://github.com/owebeeone/gwz-core/actions/runs/34687213661) |
| `gwz` | `owebeeone/gwz-cli` | [`de04a4a`](https://github.com/owebeeone/gwz-cli/commit/de04a4a) | [Actions run](https://github.com/owebeeone/gwz-cli/actions/runs/34687228994) |

The repository name need not match the crate name. `[package].name` controls
the name on crates.io; this is why the `gwz-cli` repository publishes `gwz`.
The bootstrap packages live separately from the production manifests, so their
prerelease versions do not change the production package versions.

## 1. Choose the name and check the publishing identities

Run these commands from the repository that will own the workflow. Replace the
example values throughout this guide before publishing.

```sh
BOOTSTRAP_CRATE=example-crate
BOOTSTRAP_REPO=owebeeone/example-repo
BOOTSTRAP_MANIFEST=.github/bootstrap-crate/Cargo.toml
BOOTSTRAP_WORKFLOW=bootstrap-crate.yml

gh api user --jq .login
gh repo view "$BOOTSTRAP_REPO" --json defaultBranchRef,url
curl --silent --show-error --output /dev/null --write-out '%{http_code}\n' \
  "https://crates.io/api/v1/crates/$BOOTSTRAP_CRATE"
```

The GitHub account should be `owebeeone`. HTTP **404** means no crate currently
exists at that name; **200** means it already exists. Any other status is an
error to investigate, not evidence that the name is available. Availability can
change before publication.

GitHub CLI authentication and Git-over-SSH authentication are separate. Before
an SSH push, check `ssh -T git@github.com`: the greeting must identify
`owebeeone`, not `gripd`. GitHub returns exit status 1 even for a successful SSH
authentication test. If the wrong identity is selected, use the currently
supplied `SSH_AUTH_SOCK` and `SSH_AGENT_PID` environment values for the correct
agent; do not copy an old session's socket or PID into a permanent script.

The **crates.io token's account**, not the Git commit author or SSH account,
becomes the crate owner. Create the token while logged into crates.io as
`owebeeone`, with a verified email address.

## 2. Create the isolated bootstrap package

Create this directory structure:

```text
.github/
  bootstrap-crate/
    .gitignore
    Cargo.toml
    Cargo.lock          # generated below, then committed
    README.md
    src/lib.rs
  workflows/
    bootstrap-crate.yml
```

`.github/bootstrap-crate/Cargo.toml`:

```toml
[package]
name = "example-crate"
version = "0.0.0-bootstrap.1"
edition = "2024"
license = "GPL-2.0-only"
description = "Bootstrap prerelease for the actively developed example-crate project; no implementation included."
repository = "https://github.com/owebeeone/example-repo"
readme = "README.md"
publish = ["crates-io"]
include = ["Cargo.toml", "Cargo.lock", "README.md", "src/lib.rs"]

# Keep this dependency-free bootstrap separate from the production workspace.
[workspace]
```

Use the real project's license. The empty `[workspace]` makes this package its
own Cargo workspace, avoiding the production workspace's dependency graph and
lockfile. Do not add production dependencies to the bootstrap.

`.github/bootstrap-crate/README.md`:

```markdown
# example-crate: bootstrap prerelease

This is an initial registration of the **example-crate** crate for the actively
developed [project](https://github.com/owebeeone/example-repo).

**This prerelease contains no implementation, public API, or executable.** It is
not a usable release and should not be added as a dependency or installed.
The production code and existing release instructions are in the linked repository.

This publication establishes crate ownership while the production dependency
chain is prepared for crates.io. Subsequent releases will contain the implementation.
```

`.github/bootstrap-crate/src/lib.rs`:

```rust
#![doc = include_str!("../README.md")]
```

`.github/bootstrap-crate/.gitignore`:

```gitignore
/target/
```

## 3. Verify locally without publishing

Use a current Rust toolchain supporting edition 2024. These commands generate
the bootstrap's own lockfile, inspect its contents, and verify that its packaged
source compiles. **Actual publication happens only in GitHub Actions.**

```sh
cargo generate-lockfile --manifest-path "$BOOTSTRAP_MANIFEST"
cargo fmt --manifest-path "$BOOTSTRAP_MANIFEST" -- --check
cargo package --list --locked --allow-dirty --manifest-path "$BOOTSTRAP_MANIFEST"
cargo publish --dry-run --locked --allow-dirty --manifest-path "$BOOTSTRAP_MANIFEST"
```

The expected package contains only `Cargo.toml`, Cargo's normalized/original
manifest metadata, `Cargo.lock`, `README.md`, and `src/lib.rs`. Production source,
build outputs, and credentials must not be included. `--allow-dirty` is used
here because the new files have not yet been committed; the workflow below
verifies the committed package without it.

## 4. Add the manual publication workflow

`.github/workflows/bootstrap-crate.yml`:

```yaml
name: Bootstrap crate on crates.io

on:
  workflow_dispatch:

permissions:
  contents: read

concurrency:
  group: crates-io-bootstrap
  cancel-in-progress: false

jobs:
  publish:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v6
        with:
          persist-credentials: false
      - name: Inspect bootstrap package
        run: cargo package --list --locked --manifest-path .github/bootstrap-crate/Cargo.toml
      - name: Verify bootstrap package
        run: cargo publish --dry-run --locked --manifest-path .github/bootstrap-crate/Cargo.toml
      - name: Publish bootstrap prerelease
        run: cargo publish --locked --manifest-path .github/bootstrap-crate/Cargo.toml
        env:
          CARGO_REGISTRY_TOKEN: ${{ secrets.CRATES_IO_BOOTSTRAP_TOKEN }}
```

Commit the six files shown above, including `Cargo.lock`, and push them through
the repository's normal process. Inside the managed `gwz-dev` workspace, follow
`AGENTS_GWZ.md` and use `gwz add` / `gwz commit` for workspace staging and commits.
The workflow must exist on the repository's **default branch** before it can be
manually dispatched. This workflow does not require a release tag or GitHub
Release, and does not trigger the production release workflow.

## 5. Install the temporary first-publication credential

As of this run, crates.io Trusted Publishing requires the crate to exist first.
The initial publication therefore uses an API token with permission to publish
**new crates**, rather than only update existing crates. Use a short expiry and
crate-name restrictions where supported. The original bootstrap used a seven-day
token. See the [Trusted Publishing documentation](https://crates.io/docs/trusted-publishing).

Store the token as an Actions repository secret using the interactive prompt:

```sh
gh secret set CRATES_IO_BOOTSTRAP_TOKEN --repo "$BOOTSTRAP_REPO"
```

Paste the token into that prompt, not into this document, a source file, command
arguments, or logs. For several repositories, set the secret separately in each
repository. Never commit a Cargo credentials file.

The crates.io `/api/v1/me` endpoint did not accept the publishing token during
this run; that failure alone did not mean the token was invalid. Confirm the
account in crates.io when creating the token, then verify the published owner
using the public endpoint below.

## 6. Dispatch, wait, and verify ownership

```sh
gh workflow run "$BOOTSTRAP_WORKFLOW" --repo "$BOOTSTRAP_REPO" --ref main
gh run list --repo "$BOOTSTRAP_REPO" --workflow "$BOOTSTRAP_WORKFLOW" --limit 3
```

Use the actual default branch if it is not `main`. Copy the run ID returned by
the dispatch or run listing, then wait for it:

```sh
BOOTSTRAP_RUN_ID=123456789
gh run watch "$BOOTSTRAP_RUN_ID" --repo "$BOOTSTRAP_REPO" --exit-status

curl --fail --silent --show-error \
  "https://crates.io/api/v1/crates/$BOOTSTRAP_CRATE/0.0.0-bootstrap.1" \
  | jq '.version | {num, yanked}'
curl --fail --silent --show-error \
  "https://crates.io/api/v1/crates/$BOOTSTRAP_CRATE/owners" \
  | jq '[.users[].login]'
```

Confirm version `0.0.0-bootstrap.1`, `yanked: false`, and owner `owebeeone`.
Also check the crate page's repository link and bootstrap description.

If dispatch reports an HTTP 500, inspect the workflow's run list before retrying:
the request may have started a run despite the error. This happened for `gwz`;
the first request created no run and a subsequent dispatch succeeded. If a run
fails after attempting publication, check crates.io before rerunning it. An
already published version cannot be overwritten.

## 7. Remove the temporary secret

Once the job has finished and ownership is verified:

```sh
gh secret delete CRATES_IO_BOOTSTRAP_TOKEN --repo "$BOOTSTRAP_REPO"
gh secret list --repo "$BOOTSTRAP_REPO"
```

For a shared token, wait until all intended jobs finish before revoking it in
crates.io. Removing an Actions secret does not revoke the underlying token.
The two original repository secrets were removed after successful publication.

The bootstrap workflow is deliberately manual. Once its secret is removed it
cannot publish again. Keep the bootstrap version as history; do not reuse it
for the real implementation. Configure a Trusted Publisher for the actual
production workflow once that workflow is ready, and publish normal versions
from the production manifests.

## Several crate names in one repository

Give each additional crate a separate isolated directory and manual workflow,
for example:

```text
.github/bootstrap-crates/gwz-family-model/Cargo.toml
.github/bootstrap-crates/gwz-family-model/Cargo.lock
.github/bootstrap-crates/gwz-family-model/README.md
.github/bootstrap-crates/gwz-family-model/.gitignore
.github/bootstrap-crates/gwz-family-model/src/lib.rs
.github/workflows/bootstrap-gwz-family-model.yml
```

Use that crate's real name in its manifest and README, update **all three**
`--manifest-path` arguments in its workflow, and set `BOOTSTRAP_MANIFEST` and
`BOOTSTRAP_WORKFLOW` accordingly. Give the workflow a distinct display name.
Run these workflows one at a time; they can share the repository secret and
the concurrency group. Remove the secret only after the final run finishes.
Do not overwrite the existing `gwz-core` bootstrap to register a different name.

Bootstrapping the names does not resolve the production dependency graph.
Published production dependencies still need registry versions, publishable
manifests, and publication in dependency order.

## After the bootstrap: trusted publishing only

Added 2026-09-14. The bootstrap token's purpose ended once the real crates existed.

- gwz-core and its thirteen internal crates publish only from the `publish` job in
  gwz-core's `release.yml`, authenticated by crates.io Trusted Publishing with owner
  `owebeeone`, repository `gwz-core`, workflow `release.yml` and environment `crates-io`.
  Each of those crates is set to trusted-publishing-only, so no API token can publish it.
- The first publication of the thirteen internal names used a short-lived token stored as
  the `crates-io` environment secret on gwz-core. That secret was deleted afterwards. The
  run is recorded in `gwz-core/dev-docs/GwzCratesIo-Rehearsal-20260912.md`.
- Run [34788250937](https://github.com/owebeeone/gwz-core/actions/runs/34788250937)
  exercised the setup with no secret present: the auth step exchanged the workflow's
  GitHub identity for a crates.io token, the publisher skipped all fourteen
  already-published crates, and the token was revoked when the job ended. crates.io
  scopes such a token to the crates whose configuration matches the repository, workflow
  and environment, so the run proves those values for at least one crate; each crate's
  own entry is proven the next time it publishes.
- `gwz` gets its trusted publisher when gwz-cli's publish workflow exists
  (`GwzCratesIoPlan.md` step S3.2).
- A future new crate name still needs one token-authenticated first publication before a
  trusted publisher can be added and the trusted-publishing-only flag set. Sections 5 to 7
  above describe the token and its removal.
