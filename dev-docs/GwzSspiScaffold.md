# GWZ SSPI repository scaffold

2026-10-03. **ACCEPTED scaffold checkpoint** after
[Code GO](GwzSspiScaffold-ReviewCode.md) and
[State GO](GwzSspiScaffold-ReviewState.md) at root
`e6772cbeda821deb7f2f42d63f73d9a3581e60ec`, gwz-sspi
`bd807b0403d502fd6f25ef2f93389ce432f0c0c0`, core
`8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`.
Scaffold-only GO, not authentication implementation or release acceptance.
The operator authorized GWZ local repository creation, Rust library/test layout
and release management using Gearu. No release, tag, push or registry mutation.

## Scope and ownership

New member `gwz-sspi`, id mem_gwz_sspi/source src_gwz_sspi, created with GWZ.
Standalone Cargo workspace, explicit package metadata, own lock and exact Rust
1.95 toolchain; root Cargo excludes it. GPL-2.0-only follows adjacent new GWZ
libraries. No dependencies. An integration-class library follows the accepted
SSPI revision 2 boundary; owned contract/protocol, supervision, Windows, worker
and test directories have responsibility notes. There is no authentication API,
private schema, codec, Windows FFI or native adapter yet. Empty ownership folders
are documented directories, not public placeholder Rust modules.

The optional worker executable refuses with exit 2 and fixed stderr, no stdout
or argument echo. Its process-based test is separate from the future pure fast
library suite. Deterministic contract, seeded replay, fake support and opt-in
native fixture directories are prepared without claiming those tests exist.

Gearu init generated its managed AGENTS/RELEASE sections. `gearu.toml` selects
Rust version/lock management and guarded repository release checks. GitHub CI
builds/tests/packages independently on Linux/macOS/Windows. Release publication
workflow listens only to GitHub release.published, checks tag/version and the
same release guard before Trusted Publishing. No credentials or token fallback.
Cargo publish=false deliberately blocks the scaffold. The intended remote is
not configured; first registry registration/trusted-publisher configuration and
future packaged worker assets remain explicit setup/qualification tasks.

## Verification and limits

Test-first worker refusal: empty scaffold main yielded exit 0, test failed;
fixed refusal then passed, including no stdout and no synthetic argument echo.
Normal local cargo all-features test, strict Clippy, formatting and packaged-crate
verification pass at Rust 1.95 on this Mac. Fast library currently has no SSPI
behavior tests; this is not authentication/platform qualification. Build target
is external under evidence-build-cache/gwz-sspi/scaffold. CI jobs were authored,
not remotely executed. No Windows native code or test has run from this package.

Gearu configuration load and Rust adapter plan/validation pass. A second init
left AGENTS/RELEASE bytes unchanged. release_checks rejects the intended version
while publish=false. Early local helper attempts used the config loader/Version
API incorrectly; corrected invocations passed. Initial formatting failed only
on the new assertion layout; formatter applied and check passed. Nothing was
published. Full Gearu plan needs a committed branch and configured remote, and
was not presented as completed.

## Review tier and next boundary

Dual Code/State for this initial package/release scaffold. API Surface remains
its already accepted design gate: no actual caller API or new public CLI commands
are introduced here. Existing Gearu commands are documented with their lifecycle
and publication limits. Review exact committed scaffold plus this intent doc;
raw reports stay in root dev-docs. No surrounding unresolved Windows source is
accepted. Later code/secret boundary reviews retain the accepted plan's stops.

Next implementation: owned caller values and private taut schema/secret codecs,
fake tests, then dual secret-boundary GO before supervision work. Do not start
native/authentication implementation inside this scaffold review chunk.

## Acceptance addendum

Original raw reports and canonical prompts are filed alongside this record.
Code P3-1 (packaged README linked excluded implementation checkpoint) was fixed
by explicitly including dev-docs/Implementation.md in the Cargo whitelist. A
fresh package verification and direct archive-member inspection confirm every
local README link now resolves; runtime/authentication code unchanged. This
nonblocking correction did not open another remediation package. Workflow YAML
parsing also passes using isolated PyYAML 6.0.2; no production dependency added.
Earlier default interpreters lacked that optional validation tool; no edits to
product environments were made. No remote CI, tag, publish or native SSPI claim.
