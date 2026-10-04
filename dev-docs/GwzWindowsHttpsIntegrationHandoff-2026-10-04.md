# Windows HTTPS integration — LLM handoff, 2026-10-04

## Handoff boundary and immediate action

The operator requested handoff to another LLM. This session stopped after filing
both **remediation-round-1 closure reviews**, before any next patch. This is a
settled, auditable boundary: product/evidence commits are complete, native runs
are finished, reviewers are finished, and the remaining defect is identified.
No round-2 code or contract change has started.

**Merged verdict: NO-GO for limited WH1 acceptance.**
[Code-1](GwzWindowsHttpsIntegrationImplementation-ReviewCode-1.md) is GO;
[State-1](GwzWindowsHttpsIntegrationImplementation-ReviewState-1.md) is NO-GO on
**State P2-3**, the deadline/publication-lock gap. Both original State capacity
and constructor findings are closed. Code closes its original deadline schedules
and constructor finding, but State demonstrates an untested remaining schedule.
The merged verdict must retain that P2 despite Code GO.

Start by reading State-1 P2-3 and the actual asynchronous Owner/Session call path.
Then settle the smallest synchronized publication correction and a failing
production-path regression. Do not merely repeat the direct-Mux tests or move
the same check to another place before lock acquisition.

## Authority and navigation

Workspace: `/Volumes/projects/limbo/gwz-dev`;
`/Users/owebeeone/limbo` is a symlink to `/Volumes/projects/limbo`.

Read `AGENTS_GWZ.md`, root/member AGENTS, `EVIDENCE.md`, and
[CurrentProgramCheckpoint](CurrentProgramCheckpoint.md) first. Process authority:
[AgentProcessRules](AgentProcessRules.md) as amended by
[GwzProcessOptimization](GwzProcessOptimization.md). Review-loop skill:
`/Users/owebeeone/.claude/skills/review-loop/SKILL.md`, with its canonical prompt
template under `references/`. No need to read the full historical checkpoint.

Controlling product/package documents:

- [WH1 design](GwzWindowsHttpsIntegrationDesign-DRAFT.md),
  [design acceptance](GwzWindowsHttpsIntegrationAcceptance.md),
  [budget dispositions](GwzWindowsHttpsIntegrationBudgetDisposition.md).
- [SSPI HTTPS composition](GwzSspiHttpsCompositionDesign-DRAFT.md), especially
  §4's fresh-time publication obligation; accepted composition and core
  `gwz-core/dev-docs/GWZDesign.md`/`GWZRequirements.md` remain authority.
- [Original implementation checkpoint](GwzWindowsHttpsIntegrationImplementationCheckpoint.md),
  [merged remediation plan](GwzWindowsHttpsIntegrationImplementation-RemPlan-1.md),
  [round-1 checkpoint](GwzWindowsHttpsIntegrationImplementationCheckpoint-1.md).
- Original [Code](GwzWindowsHttpsIntegrationImplementation-ReviewCode.md) and
  [State](GwzWindowsHttpsIntegrationImplementation-ReviewState.md) reports,
  then the two `-1` reports. All testimony is filed verbatim.
- [Release readiness](GwzRemoteTransportReleaseReadiness.md). The actual split
  is **1.1.0 transport / 1.2.0 sessions-server** under the release-plan amendment;
  older “v1.10.0” conversation wording is not version authority.

## Exact settled implementation and reviewed tuple

The final closure reviews inspect root
`0d2db2c23afd83d496ca9eb55d8264bf8366314e`. The handoff landing adds only status,
reports, generated prompts and this document after that reviewed root. Obtain
its final root commit with `git rev-parse HEAD`; do not confuse that documentation
landing with a newly reviewed product tuple. All member revisions stay:

| Repository | Revision |
|---|---|
| gwz-core | `261eaca55dca4067548027e8976ff0249a34d2f3` |
| gwz-cli | `6ab16d461acb6daf9fca8281eca6384971cc0c44` |
| gwz-py | `5df15766298fbbd97da1d6ecec74c6cc9dd69fda` |
| gwz-core-evidence | `053121cc97664e46539c07d77cdad4effb481955` |
| gwz-sspi | `582ec001bd2972076ea65a7db87d81d988c6f2e7` |
| gwz-transport | `8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578` |
| git2-rs | `d13951f7e0bfb6e0efcee1207ac5b140adefa455` |
| libgit2 | `b172e3d187a4b6866fd9f696f40a1b8e7f56d348` |
| gwz-git | `a9d7ee09cce6dd1407be99d8ec3676a1d288b4bf` |
| taut | `a7cab03ea487cc272f471e8a7b9e376dc28ccbdd` |
| taut-shape | `5779c9676d29750528e256714adb4dd24373e09a` |
| taut-shape-rs | `502671593f453f4b2e5915fe90e4c34b863e9b50` |
| taut-shape-py | `f86f0e9fa793e4ce5576706f25868ece84337687` |

Core round-1 diff base: `398158b3272e6f3a69132f8375190945dd93192a`.
Cumulative WH1 bases: core `c011aaee864fbe56c12a30b17664c099b8e67512`,
CLI `6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311`, Python
`e0c5af10b33289a455f662680af8ac12fd24f9d3`.

## What works and what is still wrong

WH1 enables the real HTTP pool/per-remote Git/Session route only under
`all(windows, gwz_transport_candidate, gwz_windows_https_qualification)`.
Ordinary Windows and candidate-only Windows retain the old route. Qualification
supports HTTPS Anonymous/WindowsDefault and captured WinHTTP DIRECT; SSH,
Gh/configured policies and usable helper lookup refuse. This is a deliberate
qualification subset, not finished release parity or GitHub authentication proof.

Implemented original-entry caller/environment capture, Disabled-helper backend
mapping to WindowsDefault, real final-origin prefixed leaf CBT, fixed native D,
process-contained SSPI, truthful completion/remote facts and retained cleanup.
CLI and Python communicate through the existing messages; no new byte carrier,
application schema, iroh implementation or separate wire test was introduced.

Round 1 corrects HTTPS-only capacity installation and empty SSH-only constructor
admission. It also corrects delayed preparation collection and repeated
backpressured handoff after D. Native advertisement serving is held until
successful publication. Existing Unix/nonnative controls remain green.

**Remaining State P2-3 interleaving:**

1. Native preparation succeeds before D; an authenticated Opened is pending.
2. `HttpsEndpoint::before_handoff` samples time at D−epsilon and allows Opened.
3. Publishing thread pauses or waits before acquiring `Owner`'s mux mutex.
4. It acquires the mutex after D; `Mux::send` does not recheck native D.
5. Opened queues, the route becomes Stream, `handed_off` clears the deadline
   guard, and native advertisement serving becomes eligible.

Read `gwz-core/src/transport_host/session/driver/pump.rs:183`,
`https_endpoint.rs:303`, and
`gwz-transport/src/mux/asynchronous.rs:22`/`:74`. `Shared::change` acquires
`inner` only after the outer check. Earlier `owner.advance(now)` uses an earlier
pass timestamp and cannot close the gap. No current public guarded-send or
arbitrary transactional Owner access was found in the inspected Owner API.

The existing new tests use **direct Mux**, so they do not reproduce the mutex
wait. Required regression: real asynchronous Owner/Session boundary, pause or
contend after outer guard and before acquiring publication lock, release at/after
D, assert no Opened, one Timeout, preserved facts, revoked authenticated route
and retained physical charge until disposal; include pre-D/equality controls.

State explicitly classifies this as **incomplete correction of the existing
publication-arbitration root**, not a new architectural root or the third-root
stop trigger. One merged remediation round is used. A bounded round 2 remains
possible. However, any new shared Owner interface/call-graph boundary requires
its explicit scope disposition and appropriate fresh review under the skill;
do not treat a new library method as if it already existed. No broad transport
redesign has been established as necessary. Keep authentication policy in core,
transport neutral, lock ordering explicit, and callbacks under locks bounded.

## Executed evidence and artifacts

Private archive (access required):
`gwz-core-evidence/campaigns/https-integration/runs/2026-10-04-windows-https-portability/`.
Read its README and `portable-rem1/implementation-status.json`/`commands.json`.
No public gate depends on private access. Raw attempts, original failures,
versioned runners and before-source snapshots are retained unchanged.

Portable corrected ordinary runners pass: endpoint/mux6, qualification4,
HTTPS208, paired constructor/capacity16, cleanup2, source-boundary4, cfg guard,
inventories, changed Rust formatting and whitespace. Final snapshot manifest
`4917c1a8216e430b1b41b3dfda6bbf3109e36a2b4620750429a6df3e510e3d92`.

Actual Windows11/MSVC1.95/E: ReFS:

- Original regression binary compiles then fails constructor/capacity cases;
  existing controls pass. Corrected native qualification runner passes5.
- Original CLI and installed wheel clone fail at existing one-connection limit.
  Corrected provisioned CLI `--max-per-host 1` and installed
  `Client(max_connections_per_host=1)` clone/fetch/push pass, with independent
  exact checkout/ref verification, normal TLS and no Git in client PATH.
- Python also streams ordered events and finishes another call with application
  events unconsumed. Close: pending_local_work=0, peer_cleanup_confirmed=false.
- Native verifier observes Negotiate selecting authoritative NTLM; all observed
  server contexts disposed and owned servers/processes reaped.
- Corrected actual wrong-CBT case refuses. Untrusted-chain and hostname cases
  refuse before native auth rounds. No system trust or revocation override.

Named `raw/wh1-rem1-*` receipts identify commands/exits/hashes. These outcomes
support normal-path qualification and two closed defects; they do not prove the
remaining asynchronous publication schedule. Original harness errors are not
mislabelled as meaningful counterexamples. Source refresh readback is zero
mismatches in both copied/prepared views; no source selection patch applied.

External native runtime: `E:/gwz-tests/https-portability-20261004-a`.
Source: `wh1-source-v1/source/`; prepared core: `wh1-source-v1/candidate/`.
Final CLI: `wh1-cli-target/x86_64-pc-windows-msvc/debug/gwz.exe`.
Final wheel: `wh1-rem1-wheels-v1/gwz-0.0.0-cp310-abi3-win_amd64.whl`.
Installed venv: `qualification-venv/Scripts/python.exe`.
The fixture package version0.0.0 is not a release version.

Artifact hashes in `raw/wh1-rem1-installed-green-v1.stdout.txt`:
CLI `880f1acf04d9c83d58f09c955556e53e6e112f61533793019b9712114eeaf423`,
wheel `3b8e08420331709f7ad618d5cda1b251e73294f768524c7089cf44e05ddbad33`;
that receipt also contains installed PYD and worker hashes.

External Mac drafter state:
`/Volumes/projects/limbo/evidence-build-cache/gwz-wh1-rem1-portable-20261004-a/`.
Its records are also archived under `portable-rem1/`. Reused build source/target
are `gwz-wh1-portable-20261004-a` and its `-target` sibling. The prepared source
uses links to product files: do not accidentally edit product through those
links or copy targets into evidence.

SSH host `gianni@dabeest`, Bash/Mingw. Use ClearAllForwardings=yes,
ForwardAgent=no, ForwardX11=no, BatchMode=yes and ConnectTimeout=10. User chose
E: ReFS, overriding older evidence D: guidance. Pin cargo/rustc +1.95.0 MSVC.
Do not mutate OS accounts, trust, services, proxies, zones or policy.

Replay runners: `wh1_rem1_refresh.py LABEL EXPLICIT_MANIFEST` snapshots before
inputs and performs readback; `wh1_rem1_collect_build.py LABEL PHASE` runs the
already transferred native builder. That collector does not upload the builder.
Client/Python collectors upload named scripts and use immutable new labels.
Never overwrite old receipts/runners or refresh while a native build is active.
Keep keys/repos/venvs/packages/builds external. Installed wheel uses explicit
CARGO_INCREMENTAL=0; original ReFS dev incremental cleanup WinError145 is retained
and unrestricted incremental packaging remains unqualified.

## Next work after closing the publication defect

1. Obtain settled review closure/acceptance for limited WH1; no self-GO.
2. Continue WH3 integrated Windows deadline/cancellation/identity transitions,
   installed paths with spaces/Unicode, worker provenance/missing/mismatch,
   pool reuse/replacement/concurrency and effects/retry adversity. Existing
   provider8 preparation tests are separate proof, not all integrated cases.
3. WH2 configured helpers/Job/path is separate unfinished work. The release
   scope includes configured helpers; WH1's temporary exclusion is not a waiver.
4. Provider/parity dispositions remain, including actual Kerberos/domain fixture,
   Digest, proxy/native407, Windows SSH/Pageant, differing identity and truly
   stalled provider qualification. NTLM selection does not prove Kerberos.
5. Remaining platform/selected-source/performance/package/strict/aggregate
   activation/release gates. Full strict core Clippy is RED45 with unchanged
   diagnostic categories/multiplicities. Existing generator owner-IR pin mismatch
   is RED; no generated/IR/pin edits were made. Neither is waived.

**Ordinary Windows activation and full transport release remain NO-GO.**
Nothing was pushed, tagged, published or installed globally by this session.

## Working tree, agents and safe operations

No native build/client/fixture process remains running from these attempts.
Both reviewers and the implementation drafter are completed. No pending user
answer is needed to understand the remaining defect. Relevant prior agent names:
`/root/windows_https_implementation`, `/root/windows_wh1_code`,
`/root/windows_wh1_state`. Reuse their contexts only if available in the receiving
harness; their complete durable testimony is in dev-docs. This round used 6.1
reviewers. One older portability drafter failed on quota and has no active work.

Unrelated dirt was not touched: root SSHN2b PromptCode/State and `-1` variants,
`GwzWorkspaceRouteMappingDesign.md`; core untracked
`dev-docs/GwzRemoteTransportBugReport.md`; private old-alpha untracked evidence
under `campaigns/transport-qualification/runs/2026-09-22-alpha-setup-timeout/`.
Do not absorb those files into the next package. Product tracked sources are
clean. Handoff/status/review outputs are the only intended landing changes.

All workspace staging/commits/structural operations use
`/Users/owebeeone/.cargo/bin/gwz` **from the root**. Exact workspace-relative
paths even with --target. Commit --no-commit-marker; member commits stage
managed root lock/integrity updates, which need a root commit. Read-only Git
inspection is permitted. Never hand edit gwz.conf. No pushes/tags/publishing
or production activation are authorized by this handoff.

TDD; braced control bodies; Rust cfg_if/enclosing platform boundaries; inspect
disabled branches. No mutable globals/TLS, secret logs or shadow protocol.
Cumulative WH1 is52 source/test/build plus3inventories,1350grossadded/273deleted;
current allowance55source files/2600grossadded. Stop for disposition before scope,
API/dependency/owner/mutation/platform-boundary growth. Review-loop requires
verbatim reports, exact committed tuple, peer blindness, and original-finding
verification. New interface/architecture/call graph invalidates old proof and
requires fresh reviewers. One patch per remediation; cap rules remain in force.
