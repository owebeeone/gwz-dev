# Debt recovery checkpoint — PAUSED

2026-09-07: operator requested snapshot and pause because quota is exhausted.
Do not continue until the operator says resume. The operator subsequently authorized a local checkpoint commit without new tests.
This snapshot records unfinished work, not release acceptance. No push, PR or
remote CI activation is authorized by this checkpoint request.
Use only GPT-6 or GPT-5.6 Sol/Terra/Luna; no Qwen/Gemma or subagents. Preserve
user edits and swap file for GwzRemoteAuthProposal.md. Follow AGENTS_GWZ.md;
use gwz for workspace status/staging/commits, never hand-edit gwz.conf.

## Exact stopping point — bounded callback batch completed

The operator authorized only the small callback/compile batch before quota reset;
that batch is complete and work is paused again. The prior missing-helper compile
failure is fixed. `transport_support::explicit_credential` is extracted and called
by the production callback. It permits username negotiation, offers only the
selected file once, refuses repeat SSH attempts, and refuses default/password
fallback. The attempt counter cannot wrap back into another credential offer.

Verified on the current tree:
- Three identity/callback tests passed: `/tmp/gwz-debt-auth-callback-bounded.log`.
- Local-only credential-option refusal test passed: `/tmp/gwz-debt-auth-local-bounded.log`.
- `cargo check -p gwz-core --all-targets --locked` passed without warnings:
  `/tmp/gwz-debt-auth-check-bounded.log`.

These are focused tests and a compile check, not production SSH integration or
full-suite acceptance. No broad suites or remote tests were started in this batch.
All batch processes finished. The subsequent checkpoint request saves this state locally; no publication. Next resume: complete
remote-override preflight and effective clone/fetch identity routing below.

## Latest completed batch

- Publication now refuses non-Git dependencies with UnsupportedSourceKind and
  Git entries lacking a pinned commit; checks manifest/lock source-kind agreement.
  All 11 g08 publication tests passed (`gwz-debt-publication-final.log`).
- Partial family merge reports root history was not integrated and identifies
  @root integration or preservation before disposal. Engine messages and import
  summary are both retained. All 10 family-merge tests passed.
- Tag publication plans all concrete object IDs before any transfer; annotated
  tag objects remain intact after a local tag moves. All 5 g17 tests passed.
- Mixed root/member branch switch identity guard: 12 branch tests passed.
- Inventory baseline now requires matching known build profile as well as
  platform/architecture/package. Eight inventory controls passed; frozen binary
  remainder execution reconciled 1118 passed and one ignored helper earlier.
- Boundary compiler fixtures now copy build.rs/build_support (needed for
  provenance) and exclude .regen-venv/.venv/__pycache__ as well as target and
  nested Cargo.lock. New copy-exclusion control reproduced then passed.
- Provenance scanner now fails on unreadable input metadata rather than silently
  omitting it. Core Clippy passed before the authentication work.

## Authentication implementation IN PROGRESS

Six isolated loopback SSH capability cases passed; see
GwzDebtRecoveryAuthCapability.md and `/tmp/gwz-debt-ssh-capability.json`.
They used generated keys/private agent/server, all cleaned up. Safe git2 cannot
select one encrypted agent key; no workaround/fallback claiming otherwise.

Authoritative core design now specifies RequestMeta.transport separately from
attribution, invocation remote override > invocation default > repo-local Git
remote identity > existing agent behavior. Planned flags: --identity PATH and
repeatable --remote-identity NAME=PATH (split first equals). Local configuration
command is still NOT IMPLEMENTED. No user keys/configuration were changed.

Taut schema adds RemoteSshIdentity(remote, private_key_path), TransportOptions
(default_identity, remote_identities), optional RequestMeta.transport slot 8.
Rust and Python artifacts regenerated. Exact measured removal of these additions
reproduced prior complete schema. Pre-log guard pins in core, Python drift script
and Python log test advanced from
6fd2f8829a920d6e4264a102f995a28ccc5d3dc47b25c66eca98980ad5488ca7 to
09f98f645608b84b2eb9dbaede79f2b0d3750e8e6c337f2b254eca2b0da990ce.
Evidence: /tmp/gwz-debt-before-auth.taut.py and auth-wire-before/after.json.
No existing fields/slots changed. Python native extension NOT rebuilt yet.

New identity owner at git/gitbackend/transport_support/identity.rs resolves paths,
validates duplicate/unknown overrides, precedence, file availability (regular
file, nonblocking open on Unix), local remote.<name>.gwzSshIdentity lookup.
Two resolver tests passed. A nonempty transport option on local Status was
reproduced accepted, now refuses; focused test passed. Options are bound to a
fresh Git2Backend value via GitBackend.with_transport; default custom backend
refuses explicit options. No process-global credentials. Entry points wired:
push, pull-head, tag, materialize, clone-workspace, clone-member, init-from-sources,
pull-snapshot. Clone-workspace resolves invocation paths against current_dir
because its API has no start argument. OperationContext.from_meta also guards
local-only actions (including direct forall); this latest extra guard needs test.

Native fetch/clone/list/push callbacks now accept an optional selected file and
refuse broad fallback. Anonymous family transfers remain callback-free.
ls_remote_url now receives the committed remote name and optional member repo
for identity configuration; publication no longer accidentally chooses root's
local config for an absent member. Push local preflight validates selected
remote key availability. `cargo check --all-targets` passed before the last
callback test and a few later small edits (see auth-scope-check.log).

Still REQUIRED before auth can be claimed complete:
- Callback helper is complete and focused-tested. Still finish production fixture
  tests and errors/diagnostics including selected vs authenticated identity.
- Actually invoke validate_transport_remotes at every operation's complete
  preflight; currently defined/tested but handlers do not call it. Unknown
  overrides must refuse before effects, not silently go unused.
- Ensure every fetch/clone uses the effective source remote name. Native generic
  clone currently assumes origin; materialize may clone a manifest fetch remote
  with a different name. Fix that authority path, not just CLI parsing.
- Complete local identity preflight across all selected operations/remotes,
  including root committed-lock dependency URLs, before transfers. Current push
  checks cover selected member/root key files, not all dependency authentication.
- Add sanctioned core local-config service/command, driver flags and parity,
  transport observations in human/JSON results. No private key/passphrase bytes.
- Review non-SSH behavior: current for_remote refuses an explicit identity for
  non-SSH URLs; preserve intended HTTPS helper policy and document the choice.
- Review nested request dispatch, dry-run behavior, all local-only refusals.
- Update contracts/error catalog, docs, generated guards and CI test evidence.

## Verification state and processes

At pause, stopped the active broad core test (cargo PID 8839, test PID 9045;
INT sent to test) and boundary unittest runner (PID 9277, INT sent) at operator
request. Logs are partial, NOT passing evidence:
`/tmp/gwz-debt-core-final-batch.log`,
`/tmp/gwz-debt-boundary-tests-current.log`.
The earlier boundary runner was also intentionally stopped after identifying
obsolete fixture copies; `/tmp/gwz-debt-boundary-tests.log` is interrupted.
Verify no descendants remain before starting a new run. Latest tool sessions
23913 (broad), 34305 (boundary), 65937 (completed RED callback test).

Earlier full core run passed 1842/0 failed/1 ignored in 952.96s before later
changes. Rust CLI lib174 and lane9 + separate natural lifecycle acceptance passed.
Python broad738 + all42 corrected fixture/environment rechecks passed. The bounded callback batch above supplies a newer passing core compile check;
old broad evidence and pre-auth Clippy still do not establish full auth acceptance.

Boundary hash pins were reviewed/advanced for lib.rs, workspace_ops/mod.rs,
artifact/mod.rs and git/gitbackend.rs before auth. Auth changed gitbackend.rs
again: re-review/re-measure that pin, never blindly repin protected code.
Other protocol consumers/golden gates may need the measured additive amendment.
Bazel/Cargo binaries and Python extension are stale after latest edits. Build
provenance comparison passed earlier; repeat after final code stabilizes.

## Remaining plan and environment

The full plan remains authorized when resumed. DR0 has a contracts/action/error
catalog document but incomplete matrix. DR1 workflows/helpers are drafted and
locally tested; remote CI and required check activation require reviewed commits
and GWZ_WORKSPACE_REVISION repository variables. DR2/3/4 substantially implemented
with evidence above; broader matrices and publication cases remain. DR5 unfinished
as detailed above. DR6 inventories remain alongside old count pins; Linux/macOS
merge-base inventories and CI integration/retirement are pending. Nothing shipped.

Core is a separate Cargo workspace: core commands must run inside gwz-core.
Root workspace package is gwz (Rust CLI); do NOT run cargo test -p gwz-core at root.
Python uses gwz-py/.venv and maturin develop, with GWZ_RUST_BIN set to exact root
binary for integration tests. Core/root Rust1.95; Python generator taut-proto0.9.1.
Bazel9.1.1 available. Root Python3.10 lacks tomllib; core cwd selects newer Python.
Last free disk ~14GiB; target/checked-boundary-probe ~1.2GiB. Use supported Cargo
cleanup only after checking active consumers. An earlier rm-rf cleanup was
rejected by automatic review; do not bypass. Workspace status snapshot:
`/tmp/gwz-debt-paused-status.txt` (gwz status --porcelain).
