# Debt recovery checkpoint — local implementation verified

Latest acceptance update (2026-09-07): the source was committed and pushed, then
built natively on Dabeest/Windows. An initial signed-credential-bitmask compile
failure was fixed and published (root `8f40897`, core `4d35577`). Untouched
clone/disposal and root+member work → one default family merge → ordinary
disposal passed without force. Separate detach/retain also passed. Windows
network push remains blocked by credential acquisition; detached marker status
and stale command prose need follow-up. See [the detailed acceptance record](GwzDabeestAcceptance.md).
Earlier no-publication statements below describe the prior checkpoint.

2026-09-07: operator resumed the full parked plan after quota reset. The resumed local implementation and verification batch is finished.
The earlier quota pause is superseded. The operator previously authorized a local checkpoint commit without new tests.
This snapshot records unfinished work, not release acceptance. No push, PR or
remote CI activation is authorized by this checkpoint request.
Use only GPT-6 or GPT-5.6 Sol/Terra/Luna; no Qwen/Gemma or subagents. Preserve
user edits and swap file for GwzRemoteAuthProposal.md. Follow AGENTS_GWZ.md;
use gwz for workspace status/staging/commits, never hand-edit gwz.conf.

## Current resumed work — latest evidence

The older entries below are historical, superseded where stated here. No new
commit or publication has been made since the checkpoint tuple.

- Native early ModelError failures retain generated ResponseMeta through CBOR;
  Python and Rust render that evidence. Failed ordinary and streamed Python
  operation results also retain it (new red/green tests). The focused reporting,
  errors and merge-rendering suite passes 32 tests.
- Python `--ssh-timeout` now calls the typed startup configuration service before
  backend creation. A changed value after startup refuses; an identical value is
  idempotent. Nine native read/configuration tests pass, including subprocess
  isolation. The current additive projection is
  `8aa25038218daf2d085b62bb37fb4438afd06bb77628746dac80efe53a56e76c`.
- Rust CLI library: 177 passed. Python focused battery: 91 passed before the
  final error-rendering fix. Core all-targets Clippy passed before the current
  captured-publication batch. Merge record-wire 75 tests and compatibility-error
  projection 1 test pass after boxing the private decode error.
- Push captures member source objects and effective destination URLs before
  transfer callbacks, in addition to its existing root capture/dependency barrier.
  A regression moves both HEAD and the configured push URL in OperationStarted;
  the captured commit reaches only the original remote. Publication suite: 14 pass.
  Native wildcard/annotated-tag/shorthand/deletion/matching-refspec capture passes.
  Tag publication uses the same captured transport; focused checks and the
  parallel fake-backend preflight regression pass.
- Ten production-backend SSH cases passed with server fingerprint evidence and
  observation assertions. Exact encrypted-agent selection remains unsupported;
  file identity does not close that separate library capability limitation.
- Boundary hashes were reviewed and advanced; provenance rebuilds, selection
  audit and local verification are finished. Remote CI acceptance remains open.

Python's native module now exposes its compiled core provenance alongside
version/health metadata, using the existing core-owned string. Four native bridge
tests pass, including comparison with the exact GWZ_RUST_BIN. Cargo, Bazel and
the Python native core all agree in `/tmp/gwz-debt-final-build-identity.json`.
The CI evidence gate additionally requires actual protocol-test execution;
all 15 workspace helper tests pass. The contract audit contains the invariant /
owner / command / topology / platform / required-job coverage manifest.

## Broad run accounting — completed

Core: **1,863 passed, 2 failed, 1 intentionally ignored**, 1,137.02 seconds,
`/tmp/gwz-debt-resumed-core-full.log`. Both failures were stale guards, not accepted
failures: named-clone creation-funnel assertion and the 27→28 open-merge command
census. The first passes in `/tmp/gwz-debt-filter-funnel-green.log`; all 10 config
gate tests and both open-merge gate tests pass after the second correction.
The additional root-retry/fresh-clone test passes separately. Captured publication
15 scenarios, native refspec/tracking tests, inactive dependency proof and root
branch/tag/stash dry-runs have focused green evidence after the broad run's
compiled snapshot. Do not describe these as one all-green full-suite run.

Python: **784 passed, 13 failed**, 408.57 seconds; all 13 were the same local-family
JSON-envelope compatibility regression. The correction passed the 195-test
error/local-family suite, then 208 native/error/local-family checks after rebuilding.
No known failure remains from either broad run. Core protocol 37 tests, current
Clippy, Rust library/lifecycle/identity acceptance and Cargo/Bazel builds pass.
The boundary-checker negative/compiler suite passed **all 75 tests** in 746.636
seconds: `/tmp/gwz-debt-final-boundary-suite.log`. All verification jobs finished.

## Verification and CI continuation

- Compatibility battery passes all six commands, including both document gates
  and their negative suites. Reviewed source-boundary gate passes (24 visible
  entries, 9 classified modules). Bazel pin drift, build versions and selection
  boundaries pass; protocol additive and packaged-IR checks pass.
- Captured push retains named-remote tracking updates when its URL is unchanged;
  URL drift uses the captured in-memory endpoint. The native tracking regression
  passes. Root rejection/member-success/root-retry/fresh-clone acceptance passes.
- Broader Python run: 784 passed, 13 failed, all the old local-family error JSON
  shape. The fix only emits additional metadata when transport rows exist;
  all 195 affected error/transport/local-family tests pass afterward.
- The broad core run and both corrected-guard rechecks finished; see the exact
  accounting above. Their semantic coverage is preserved.
- CI now selects the exact core test executable from Cargo JSON, freezes it,
  generates the platform inventory and executes/reconciles all test identities.
  Workspace evidence refuses an unexecuted or incomplete census. Nine census
  controls and 15 workspace helper tests pass. This runs alongside old named
  gates; same-platform merge-base comparison and Linux acceptance still gate
  retirement of manual count pins. No workflow has been activated remotely.

The final captured-publication native check preserves wildcard expansion,
annotated tag objects, shorthand, deletion, matching refs and normal tracking-ref
updates. Root-only dependency proof also passes with an inactive/fetch-only member.
Root branch/tag/stash lifecycle cases verify dry-run leaves their native state
unchanged. No source test identity was removed: current census 1,867 vs prior
local snapshot 1,844 (23 additions). This is not a merge-base comparison.

Current-source Cargo and Bazel both built successfully and report:
CLI source `22ebcd6a11b96f4d27baa207663b898a0ba68ab478e80741d6099aa99e12b4ec`;
core source `07b04f891a67ec101dd6f462f8a7e0a828c0b30491c914d4017824a0fbe2afcb`.
Their package versions, revisions and dirty flags agree; only build-system labels
differ. Evidence: `/tmp/gwz-debt-final-build-identity.json`. Rust acceptance is
177 library + 10 lifecycle + 1 identity test passed. Python native/error delta
is 208 passed. Generated CLI help is current; all-repository whitespace checks pass.

CI tuple pinning now corrects branch-following materialization on ephemeral
runners: after GWZ creates member paths, every member is detached at the resolved
locked/candidate SHA and reverified. A new regression proves a floating sibling
is pinned too. All 15 workspace helper tests pass. No live workspace member was
checked out or structurally changed by this CI-only helper during local work.

## Maintenance ownership and deferred moves

Owner: current debt-recovery task. Razel has no executable on PATH, no live
workspace daemon and no active GWZ CI/build command consumer. Root Cargo/Bazel
comments no longer advertise it. Taut's independent razel protocol/tests remain
legitimate sources and were not removed. Generated root `.razel-exec`, cache,
crate downloads, stale daemon log/socket were moved without following their
workspace symlinks to
`/var/folders/02/bn9c9g2x5qj8bb42zb857p7c0000gn/T/gwz-retired-razel-sa2codxy`.
This is reversible retirement, not disk-space reclamation. Defensive ignore and
traversal exclusions remain as defensive exclusions; Cargo/Bazel builds pass.

`rust-split 0.1.1` is installed at `/Users/owebeeone/.cargo/bin/rust-split`.
The boundary document now records L1-23 and the brief convention. Cohesion review:
local-disposal/src/lib.rs (2,277 lines) mixes admission orchestration and substantial
tests; schedule tests into a sibling module, then review admission vs removal
ownership, after the operator lifts the existing split deferral. Python client.py
(1,224 lines) combines generated-type imports, request construction and lifecycle
helpers; retain it for this behavior batch and schedule a separate client-layer
review. push_member.rs (758) remains one publication orchestration owner, with
capture/transport in backend push_plan.rs and lock proof in publication.rs.
No mechanical moves or test renames were performed. A future movement-only
checkpoint must preserve the generated inventory and report any intentional
identity changes; it must not mix new disposal behavior with splitting.

## Remaining release and deferred work

- Review and commit the source tuple, then publish and activate remote CI / required
  checks. This continuation made no commit, push, PR or remote configuration change.
- Review same-platform merge-base census baselines on Linux and macOS before
  retiring manual count pins. Windows requires its separate platform acceptance.
- Exact encrypted-agent selection remains unsupported and separately scoped:
  estimate 5–10 engineering days plus upstream latency and Windows investigation.
- Actual cohesion/file splits remain explicitly deferred; the owners and review
  triggers above are recorded, but documentation does not complete those moves.
- Complete settled-tree and release acceptance. Local evidence includes corrected
  focused reruns, not a claim that one final full-suite invocation passed everything.

## Archived pre-resume checkpoint (historical)

## Prior stopping point — bounded callback batch completed

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


## Earlier continuation evidence (historical)

Transport reporting continuation: taut-defined `TransportObservation`, three enums,
optional `ResponseMeta.transport` slot 8 and `OperationResult.transport` slot 10
are generated. Exact removal reproduced the old schema; new pre-log projection
is `9f338f2287cf7127b760b5dfaf4e86a5f5152fb38234fb9c5ca94949db3e271d`.
Native callbacks record offered versus authenticated, and response wrappers collect
operation-scoped observations without changing OperationContext or protected v1
modules. The Python extension was rebuilt. Python renderer tests (2), Rust
renderer test (1), and 75 Python client/parser/protocol checks pass. The production
SSH fixture now passes 10 cases including per-remote and local configuration,
both agent orders, explicit wrong/missing/encrypted keys and host mismatch;
observation assertions agree with the server's acceptance log. Evidence:
`/tmp/gwz-debt-product-ssh-evidence.json`.

New native backends install bounded 3-second connect/read defaults (red/green
regression), and configuration identity choices are frozen per operation with
separate caches for separate requests (5 resolver tests pass). Push/tag actual
read preflight now runs before transfer effects; publication 13 and tag 6 tests
passed before observation wiring. Most recent response/stream propagation and
protocol rechecks are still running. Rust CLI full library previously passed
174 with 2 failures (argument-id collision and generated help); both were fixed,
with full rerun pending. Root source boundary pins require a reviewed advance for
the new handler registration, identity delegates and observations export.

Remaining reporting gap: handlers returning ModelError before an envelope can
lose transport observations; failure-path propagation still needs a design and
implementation. Public fingerprints remain unknown, honestly absent. Python's
existing --ssh-timeout flag is parsed but its explicit value still needs startup
wiring; the core default is now bounded. No new checkpoint commit or publication.


`auth identity REMOTE [--set PATH | --unset]` now has a core-owned operation,
common selection policy (ActionKind 29), native Git-local storage and both CLI
surfaces. Core local configuration round-trip and all-target preflight tests
pass; Python native read suite passes 7 tests, client/parser suite passes 53,
and Rust identity parser passes. Removing the new method, three messages,
operation enum and action exactly reproduces the previous protocol; new guard
projection is `f45ebbb8cfa1ed81f29cf18c4e6df03314ee45d4584229d2a8daa1b9e16bdc73`.
Structured transport observations and actual-network preflight remain pending;
configuration command success is not authentication proof.


Latest continuation (2026-09-07): Python now probes the taut-defined
`transport_capabilities` service before nonempty identity options reach either
call or submit. Legacy/unsupported cores refuse first; ordinary calls do not
probe. Native extension rebuilt and the typed capability/version test passes.
52 client/parser tests pass. Exact-agent capability remains false. Removing the
new service method and two messages exactly reproduced the previous schema;
new pre-log projection is `ab44d75d4ef6bca60864c7150c44f381c318aaa642db143aad619951fa4ff44a`.

Root dependency identity validation now precedes member push effects (red test
proved a member remote advanced before the missing dependency key was found).
All 13 publication tests pass. Tag preflight shares the same dependency helper;
its next focused verification is pending. These are uncommitted changes after
the existing checkpoint; no new commit or remote publication was performed.


Working from root checkpoint 47fe210 (core37dbbcf, CLI0238606, Pythoned5f129).
New regressions and fixes: unknown remote identity overrides refuse before push,
remote-tag operations and root merge-pull integration. Materialize clones use
the manifest fetch-remote name, including identity selection and native remote
creation. URL clone with named overrides inspects the committed remote manifest
in temporary bare storage before allocating its destination.
Push12, tag6, merge-pull6, named materialize and URL-clone focused tests pass.
Both drivers parse --identity and repeated --remote-identity NAME=PATH; Rust
parser test and Python46 parser/client tests pass. Python Client.meta now passes
transport=None by default, repairing the checkpoint's generated-constructor
mismatch. Four identity-owner tests pass, including invocation override of bad
local config and preserving HTTPS behavior for an invocation-wide SSH default.
Local tag-create/branch-materialize option refusal has been fixed after a red
test, but needs recheck. Core/CLI formatting and broad checks still pending.
The remaining auth/config/diagnostics/capability work below is still open.

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
