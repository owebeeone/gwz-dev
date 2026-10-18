# GwzRemoteTransportAlpha State Review

## Tuple

Verified unchanged at review start and end:

- root `3a567c65dbbf77d7b4f01a934dd431b329a27d02`
- core `3b79fb26d731cce565a2319a2ae2d512d4d54c51`
- CLI `b2b24edcaa6a09732b7cbbef65605b16925a29d8`
- evidence `37431406e4d948a0a42f61a77e453324f4c008f5`
- transport `aa40936d0805e8cb60f8027615abe20d4f2045e4`
- git2-rs `ce78628308e11b4e8901d5061602619109bce21a`
- libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`

## Evidence inspected

I inspected the committed alpha design; core constructor, environment configuration and cleanup ownership; CLI dispatch extraction and candidate selection; the monotonic mux-retirement correction; evidence manifests, source fingerprints, build/default-graph records, historical failures, corrected HTTPS clone/fetch/push run, and SSH regression.

The request metadata and operation ID passed to the host exactly match those passed to the selected handler. Normal success and error returns execute request finish followed by runtime shutdown; panic unwinding invokes the same cleanup through `Command::drop`. The helper launch remains shell-free. Evidence fingerprints match the committed source and root lockfile.

## Finding

### P2-1 — Purely local command variants are admitted through network-environment construction

**Location:** `gwz-cli/src/globalargs/dispatch.rs:353-367`, especially unconditional selection of `CliRequest::Snapshot` and every `CliRequest::Tag`; `gwz-core/src/transport_host/local_command.rs:17-32`.

`transport_meta` treats `snapshot` and all tag variants as network commands. Before their handlers run, `with_local_transport` validates CA and proxy environment and constructs the shared SSH/HTTPS runtime. However, `handle_snapshot` only reads local repository state and writes the snapshot artifact. `tag create`, local `tag list`, and local `tag delete` likewise require no remote; only tag fetch/push and list/delete with `--remote` use transport.

Reproduction: build the candidate alpha, set `HTTPS_PROXY=socks5://localhost:12345` or `GIT_SSL_CAINFO` to an unreadable file, then invoke a valid local snapshot or `tag list/create/delete` without a remote. The command fails during `tls_config` before reaching its local handler. The same repository operation succeeds through ordinary dispatch.

This fails closed and does not corrupt workspace state, but it makes durable local operations depend on unrelated endpoint configuration and expands credential/environment capture beyond the authorized network boundary.

**Required correction:** exclude `Snapshot`; select `Tag` only for fetch/push and list/delete when a remote is specified. Preserve candidate routing for the actual network variants.

**Closure test:** under deliberately invalid HTTPS proxy/CA configuration, prove snapshot and local tag create/list/delete retain ordinary behavior, while tag fetch/push and remote list/delete still enter candidate transport and reject the invalid endpoint configuration.

## Verdict

**NO-GO** due to P2-1.

I pre-commit to **GO** on a revision that narrows dispatch as specified and adds the focused variant-selection regression. No builds or files were changed during this review.
