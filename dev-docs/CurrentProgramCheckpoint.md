# Current program checkpoint

## SSPI caller values / secret codec accepted, 2026-10-03

[Acceptance](GwzSspiSecretCodecAcceptance.md) records Code/State/Surface GO at root
e9f80c697acc5860ad90dbf5acd5888ccf2bd586, member
e3851768da8d58140d92590bf61575f6edbe333c and unchanged reference core
8cb3a3f01d79699a5ad07b6ec7cfc78321224d31. Final member
44879481fbd54dab84b99fecdadc89a34a84dcbd adds a compiled caller walkthrough and
status/testing notes; Surface re-verdict independently closes its nonblocking P3.
No runtime/API/wire change in that follow-up. Zero findings remain, blocking
remediation rounds 0, architectural causes 0, blind convergence none, escaped
defects 0. Recorded tier: dual Code/State secret/wire gate plus cold value Surface.
29 unit tests, caller integration, unchanged worker refusal, twelve negative
trait doctests and compiled example pass (44 Rust checks); 13 Python tests,
16-artifact regeneration, Clippy/fmt, disabled-branch scope check and 61-file
standalone package verification pass. Remote CI is unexecuted.
Step 1 secret-boundary stop is closed. Next: supervision kernel with deterministic
race schedules, retained launch/I/O capacity and Code/State review, then native
secret disposal review and host integration. Worker still refuses; publishing
disabled; full Windows remains NO-GO. No push, tag, release or activation.
Preceding entries are historical snapshots.

## SSPI taut messages/design accepted, 2026-10-03

[Acceptance](GwzSspiMessagesAcceptance.md) records Consistency/Safety/Surface GO at
root 05403cea8018cf14a793977ddec2c0c7136ccc54, member
7ea900e77f272dd6e0d64f566c59fb29322f5738 and unchanged core
8cb3a3f01d79699a5ad07b6ec7cfc78321224d31. Seven private taut messages, exported
IR, fingerprint of schema plus semantics, pinned generator tooling and twelve
synthetic/model tests are accepted. One merged remediation closed Consistency
P2-1 (required raw TokenLimit caller input), one architectural cause, no blind
convergence, escaped defects 0. Surface P3 stale formula corrected at acceptance.
Final extracted crate schema/tests and package verification pass; remote CI is
unexecuted. No production API/codec/SSPI authentication is implemented, worker
refuses, publishing disabled and Windows remains NO-GO. Next cohesive chunk:
caller secret values and IR-driven zeroizing codec + deterministic conformance,
then mandatory dual Code/State secret-boundary gate before supervision. No input
needed for this local work; no push, tag, release, remote or endpoint activation.
Earlier sections are historical snapshots.

## SSPI Rust repository scaffold accepted, 2026-10-03

The operator authorized `gwz repo create gwz-sspi` and Rust/test/release layout.
The member is registered locally; independent Code/State review is GO on the
[scaffold record](GwzSspiScaffold.md). Standalone Cargo, exact toolchain, separate
contract/replay/native-test directories, optional refusing worker, Gearu managed
instructions/config, CI and guarded publication workflow are committed. The one
nonblocking archive documentation issue was corrected and package-verified.
Local build/test/lint/format/package and Gearu validation pass. Public workflow
files are authored, not remotely executed. Authentication/API/codec/native-worker
implementation is still next; full Windows remains NO-GO. No remote configured,
no registry release/push/tag. No additional operator input needed for local work;
remote and registry setup are later release prerequisites.


## SSPI boundary design accepted, 2026-10-03

[Design](GwzSspiDesign.md), [plan](GwzSspiPlan.md) and
[caller API](GwzSspiCallerGuide-DRAFT.md) revision 2 are **GO** on all three axes
at root `f2029e4b1739c0214138675dfb16abdb44f6a0d7`, core
`d78a664e3c5a325c6f12be409eb7645c1c1b51d0`, evidence
`1beb1d204c824701ddbd033c7f89df9a3561f5e5`. One merged remediation; raw reports,
scope correction, withdrawals and closure are in [acceptance](GwzSspiAcceptance.md)
and [ledger](GwzSspiDesign-ReviewLedger.md). The Windows DRAFT and exact raw batches
are imported into MAIN, without product code. Native creation-time Job attachment
passed; it is primitive containment evidence, not authentication/release GO.

Accepted scope is standalone Windows-specific `gwz-sspi`, CLI self-exec/Python
bundled worker and explicit IPC/identity/secret/cancellation/cleanup contracts.
Full Windows remains NO-GO. Next: supplied member remote, standalone contract
implementation/tests and mandatory secret-boundary review stops. No new repository,
implementation, push, tag or product activation in this design checkpoint.
Acceptance commits only status/evidence records; no reviewed behavior changed.
Earlier investigation snapshots below predate this design GO.


## Windows process investigation and artifact relocation, 2026-10-03

The authentication-alternative investigation is complete. Independent Rust
`sspi` 0.23.0 cancelled real suspended Kerberos network waits in a bounded native
Windows fixture and rejected mismatched synthetic NTLM bindings. It is not a
full native SSPI replacement: current-logon SSO and Digest are absent from the
tested independent path; synchronous discovery, temporary password copies and
full EPA/interoperability remain concerns. The operator selected investigation
of native SSPI in a separate worker process instead. That investigation now
passes bounded native process/Job tests: the worker retained its NTLM context
across token exchanges, IPC EOF cleaned up, job termination reaped a controlled
stalled worker and descendant, and parent loss triggered kill-on-close cleanup.
Held handles confirmed exits before capacity release; final fixture census was
empty. The arrangement worked inside the actual SSH parent's existing Job.
It did not reproduce a blocked provider, finish server authentication, establish
interactive SSO, prove secret erasure after forced exit, or cancel external
LSASS/Pageant work. No process mechanism is accepted or implemented in the product.
Next: revise and review the bounded Windows worker design, incorporating MAIN's
accepted helper/clock seams and explicit identity, IPC and unconfirmed-exit
quarantine rules. Digest and the other outstanding Windows proof rows remain.

The operator also requested correction of loose parent artifacts. All 210
selected entries are now relocated, with exact logs/receipts/runners in private
evidence, reports in dev-docs and 35 runtime/build/parking directories consolidated
outside the repositories. No bytes were deleted and the registered GWZ lanes
remain ready at their original locations. See
[artifact locations](GwzTransportArtifactLocations.md) and
[the alternative report](../gwz-core/dev-docs/GwzTransportWindowsAuthAlternativeFeasibility.md).
The [native worker report](../gwz-core/dev-docs/GwzTransportWindowsSspiWorkerFeasibility.md)
and its exact private run are now retained in MAIN as well as the Windows lane.
Earlier external paths below are historical snapshots. Mac trust preparation
requires renewed exact preflight after relocation; no OS trust change occurred.
MAIN documentary/evidence member heads are core
`ba2df3b213a88f9290177b4ab931efe3cafa4886` and evidence
`a539aeaa9340f659c8b7d995d1c0be3e59d76b80`. Product source remains the validated
tuple below; these later commits add reports/evidence only. File/hash, archive,
document-link and fast document guards pass. No push, tag or product activation.

## Credential transport — integrated and macOS validated, 2026-10-03

The credential lane is merged and combined macOS integration is accepted.
The validated tuple is:

| Repository | Commit |
| --- | --- |
| root | `5be088f50808e930cc78be8db0671486256dac4a` |
| gwz-core | `a92a7990081475e349b182a332c7553731f4fb5b` |
| gwz-transport | `1aab733783e06b25cb5d2321d71ec0b34417a29c` |
| gwz-cli | `f925e1165c2b2d368a00277594450b010a95867a` |
| gwz-py | `5aeff4bfb11da048f1174db2b1045ed0c9b6c80a` |
| gwz-core-evidence | `662d89828b478a2acce8c0308834db7d17c872f7` |

This checkpoint commit changes only documentation after validation.
[The integration record](GwzTransportCredentialIntegration.md) records exact
receipts and limitations. Production Code and State round 2 reviews and the
unchanged Surface round 1 review are GO. Two source remediation rounds closed
seven findings; reviewers identified no architectural cause in this source object.

GWZ fast-forwarded five members in `merge_op_93443_1790979898498_0001`.
All 27 unrelated draft files were restored and their hashes verified.
Full integration exposed two stale Python tests. Their correction changes only
tests, has independent Code GO, and passes all final full Python suites:
992 ordinary passes with 18 candidate-only skips; 1,010 passes in each candidate
configuration. Full CLI ordinary/candidate suites, strict Clippy and guards pass.
All 15 core/transport gates pass, including full ordinary and both candidate
phase runners. Core's 49 warnings, inherited ignores and formatting debt remain
unwaived. Original failed attempts are preserved.

Next: Windows TR1.8 still needs the remaining native assertions/dispositions and
design GO, then implementation/parity. Its five-batch checkpoint follows below.
Deferred platform, performance and selected-source/package
checks remain a later batch, followed by aggregate release/activation review.
This accepts macOS integration only. No push, tag, publication, activation or
alpha installation occurred. Lane root history and its older Python stash remain
retained; no lane was disposed. The sections below are historical snapshots;
this section controls current status.

## Windows TR1.8 — five native batches retained, design remains DRAFT

The Windows lane is checkpointed separately; it has not been merged into MAIN.

| Repository | Commit |
| --- | --- |
| lane root | `e2ca3c849e7d829096557b99a660799f59aba3fd` |
| lane gwz-core | `d4baa0186671181d00ad8ec920a8960b1058fa18` |
| lane gwz-core-evidence | `0f18b57724f3a1bc5b9ee8f1b586b23ba71da071` |

Lane: `/Volumes/projects/limbo/gwz-dev-tr1-8-win`. Its core documents are
`dev-docs/GwzTransportWindowsBaseline.md`, `GwzTransportWindowsCheckpoint.md`,
`GwzTransportWindowsParityDesign.md` and the new
`GwzTransportWindowsProofDispositions-DRAFT.md`. The working note proposes
HOME compatibility dispositions and distinguishes GWZ sender cleanup from an
external Pageant receiver's lifetime; neither is accepted policy yet.

The fourth batch's four WDigest credential variants all returned
`SEC_E_UNKNOWN_CREDENTIALS`; method/URI exchange remains unexecuted. A controlled
native wait proved retention of two live SSPI contexts through cancellation,
capacity refusal and release only after worker exit. It did not exercise a
blocked provider call or establish native cancellation guarantees.

The fifth batch passed five native Windows Rust TLS primitive rows: two origin
bindings matched independent certificate digests, untrusted/wrong-name peers
refused before application data, and nested TLS proxy/CONNECT/origin selected
distinct correct bindings. The DRAFT now uses the existing native-tls
`tls_server_end_point()` API through tokio-native-tls rather than requiring a
new certificate parser. This proves the extraction seam, not integrated
Hyper/pool lifetime, HTTP authentication, EPA or the complete TLS matrix.

Private raw evidence is retained in the lane evidence member under
`campaigns/transport-qualification/runs/2026-10-03-tr1-8-digest-workers/`
and `2026-10-03-tr1-8-tls-adapter/` (40 and 38 files). Failed collector/fixture
attempts remain preserved. All 74 manifest-listed artifacts match their hashes;
archive and fast document checks pass. The fourth batch's public-document hashes
describe its handoff snapshot; subsequent coordinator additions are explicit.
No product source, OS trust, service, account or policy changed.

Windows remains NO-GO. Before design freeze, refresh the lane from MAIN's
accepted helper timing/context and SSH clock seams, resolve Pageant receiver/
window-identity promises and Digest/blocked-SSPI ownership, and finish the
required identity, trust/routing and EPA assertions or accepted dispositions.
The Mac-only temporary trust test is prepared but awaits explicit approval;
Windows trust changes remain held. Only then settle the design for independent
review and implementation. No new review verdict, push, tag or release occurred.

## Combined integration — two stale Python tests, 2026-10-03

Production credential source accepted and GWZ merged; CLI full ordinary/candidate
suites, both strictClippy and shared guards GREEN. Python fullordinary991/1/18
and both1008/2 reveal two stale tests: historical projection lacks exact approved75;
fake-gh row expects unconfigured automatic invocation, superseded by accepted
configured-helper policy. Native builds/strictClippy/protocol drift/provenance
checks GREEN. No production defect is established by these failures. Root
corrects only two existing Py tests under GwzTransportCredentialIntegration-RemPlan.md,
then focusedGREEN, settle, interior Code review and fullPython revalidation.
Core ordinary and standalone transport suites pass; core candidate full phases
continue with their own compiled inputs unchanged. Validator records explicit
independent Py/root tuple transition instead of claiming all six unchanged.
Integration/release stays pending; no failure is waived.

## Credential implementation integrated — combined validation, 2026-10-03

Source accepted after Code2/State2 GO at lane rootb35887e/coreec43f585;
Surface1 GO holds for unchanged CLI/Python/help. Complete reports and source
acceptance are now in MAIN core dev-docs/GwzTransportCredentialHelpersAcceptance.md.
GWZ family merge merge_op_93443_1790979898498_0001 fast-forwarded5 members:
corea92a7990081475e349b182a332c7553731f4fb5b,
transport1aab733783e06b25cb5d2321d71ec0b34417a29c,
Python a0d4350f31069362b3cfbeae668666ab47567264,
CLI f925e1165c2b2d368a00277594450b010a95867a,
evidence662d89828b478a2acce8c0308834db7d17c872f7.
Two source remediation rounds close six original blockers plus one new
cleanup-order P2; zero architectural causes. Source acceptance and merge do
not constitute combined integration, platform or release acceptance.

MAIN refused first merge for unrelated untracked drafts. A targeted GWZ stash
preserved them, then all27 preflight files were restored and hash-verified;
temporary stash retired. No data was deleted. Lane root history was not part
of member-scoped merge and remains retained at tr2-22 rootf8212e7. The older
Python WIP stash remains in that lane with hashed external backup; do not pop.
Current coordinator logs/review prompts are external under
/Volumes/projects/limbo/gwz-lanes-prewarm-20261003/credential-implementation-review/.

Next: fresh MAIN ordinary/candidate CLI, core and Python suites with separate
build-cache ownership. CLI ordinary stages precede Python hardcoded CLI fixture.
Earlier full suites are historical; round2 focused17/0 and affected458/0/4,
coreClippy0/49 warnings, source guards and exact core gates pass. Retained
warnings/ignores are unwaived. Windows design/provider/trust remains NO-GO;
platform/performance/selected-source/package and aggregate release review remain
open. No push/tag/publication/activation/alpha installation this round.


## Four transport lanes — coordinator state, 2026-10-03

- CLI/Python configuration is integrated and validated in main; exact sources,
  successful ordinary/candidate gates and limitations are recorded below.
  Python's two accepted Surface P3 help corrections are folded into the next
  combined Surface package: root and network help now expose effective
  concurrency defaults, transport precedence and selection/removal recipes.
  All seven help surfaces were inspected and the existing parser suite passes;
  runtime selection and defaults are unchanged. Reviewer closure is pending.
- `tr2-22`: bounded helper context contract is accepted after all three
  original reviewers' GO (reviewed core `ec1b95831582651953bb0bd3be3c4f88985aba6b`).
  Acceptance record/root `684ffca933bea7b0d2028048d64a97d1be389699`,
  core `aedfa862d7ce6c121ecbe4d65ac4900e2017fdde`; reports filed verbatim
  in that lane. Its internal fields/parser cause may now be implemented.
  Real Git hasconfig conditional-include regression was initially RED;
  a separate configuration-view mechanism passed physical feasibility but its
  initial review found Safety P2-1 HOME anchoring and P2-2 sensitive scratch
  after process death. One disk-free correction closes both: original
  Consistency/Safety reviewers returned GO at lane core
  `daeb4e17dd414ccdb670f9ab74333dd84ef0ba19`. Adoption at core
  `edcab346745b896c14112bc8efdace87d91c50f1` files raw reports, folds the
  nonblocking P3 outcome-supersession clarification and adds implementation
  authority; exact core gate passes. No new libgit2 binding is needed.
  The implementer now finishes one helper/context/code75/SSH-password package.
  Before the view implementation, broader HTTPS checks reached 148 passes
  and two failures; the real hasconfig product regression now passes. A new
  implementation-contact counterexample shows native Git can preread an
  unconditional FIFO include even for stdin parsing. The child deadline bounds
  the failure, but the accepted source-only parse-I/O claim needs correction.
  A minimal empty controlled parser environment passes that exact native
  counterexample and preserves null/empty/escaping/non-UTF-8 values. Initial
  discovery remains native-read/deadline bounded. The bounded correction,
  design clarification and cleanup regressions join the final full Code/State
  package; earlier mechanism GO does not cover these corrected bytes.
  Full implementation gates and review remain open; none of
  these intermediate results is implementation acceptance or release GO.
  The counterexample/docs checkpoint is preserved at lane core
  `646e3e1cda04634df22369238bee286bc1b7ef82`, evidence
  `662d89828b478a2acce8c0308834db7d17c872f7`; exact checks pass.
  SSH password-only integration found a shared-clock seam: Control, generic
  pool expiry and post-result classification otherwise clip helper time.
  A bounded DRAFT at core `eb06fac24a3e1eb8f4db32de261a26627e1fd01b`,
  lane root `5d89a096dd0688e5b82b501a6ce997dcfae060f1`, received NO-GO
  from both fresh GPT-6.1 reviewers. Blind convergence confirms arbitration
  and stall-resume gaps; Consistency additionally found the pool-first helper
  timing-provenance gap. Reports and one combined remediation plan are filed
  at lane core `a81d1433621d5634e19b125f1ca217c147c840f0`; exact gate
  passes. Round 1 closes all original findings; Consistency GO, Safety NO-GO
  on a new non-architectural prepared-token refusal branch. Reports and one
  round-2 plan are preserved at lane core
  `fae9c93e79477923d2c32afd3cc0cc22dc971364`; exact gate passes.
  Both original round-2 reviewers now return GO with no open findings at lane
  core `fde5878ac11b9e02892127f438cb50954551b9a3`, root
  `0bb560c5f34ba5bde5ece053dc58e33ef10fd8e0`. Reports and narrow design/
  requirements authority are adopted at core
  `a53e1f1004a157d0bdc3adb2a038fcf52cb25ddc`, lane root
  `647127ffdcd2bc99bfb006da49686dffaa7b6f6f`; exact core gate passes.
  Root explicitly relayed implementation GO for this mechanism only.
  Two remediation rounds close all findings; architectural count remains two.
  HTTPS candidate suite passes 162/0; full transport/doctests and strict
  transport Clippy pass. Real fetch/push/private-clone error projections pass,
  including M1/code75, controlled E2BIG M2 and repaired same-operation routing.
  Normal core passes 2103/0 with one ignored; generators, provenance and source
  guards pass. Candidate core Clippy passes with 51 existing warnings retained
  as debt. Logs: `/Volumes/projects/limbo/gwz-tr222-route-https-final-v7.log`
  and `gwz-tr222-helper-projection-v4.log` alongside it. These are working-source
  results, not accepted source. Shared clock/password endpoint implementation is
  now complete: both full candidate phase runners pass 2652/0/7 ignored;
  ordinary core passes 2103/0/1 ignored; native SSH passes 194/0/3 ignored;
  transport/tests/strict Clippy, generators and source guards pass. Candidate
  core Clippy retains 50 warnings. The final consumer trace found an open seam:
  SshOpenFailure/HostRoute must retain typed helper outcomes through the existing
  owned TransportAttempt bridge, as HTTPS already does. Root authorized this
  bounded projection under the accepted clock's typed retry/error scope, using
  existing common M4/M10 wording and exact budgets without HTTP-specific mapping,
  string inference, public schema or policy change. This final correction and
  actual-operation regressions are running; previous whole gates predate it.
  Next: final patch/gates, exact settlement, fresh Code/State and combined Surface
  review (including the common SSH helper wording). No merge or release GO yet.
- `tr1-8-win`: two native primitive/baseline batches are preserved before Windows
  design freeze. ReFS and native APIs have executed proof; SSH-inherited job
  membership prevents claiming guardian independence for trust edits. A
  partial baseline is preserved at lane core
  `497149940f2ea570e8be0943af31e7de7127fde7`, private evidence
  `70f481147830707d375f4da7f5c70e2aff3f7460`; exact core/tracked archive
  gates pass. This is preservation, not design GO. The
  scratch-only session-loss test passed for the exact tested launch shape:
  owned processes retired, mock state restored and no late write. It does not
  establish universal job independence; Windows trust execution remains held.
  The second isolated native-primitive batch is complete at lane core
  `e616dc8a96cab434578297a04e9fbb567b63dcd8`, private evidence
  `9025f0bc18ea55bcd39fc145057269920750dce6`, root
  `affd9eb1ffbfe669d90f137b1fa4c30d60ba3d85`; exact core and tracked
  archive gates pass. It adds HOME characterization, owned Pageant/pipe/job
  lifetime tests, SSPI first legs and process-pinned TLS peer/hash proof.
  HOME compatibility dispositions, native provider/identity proof and released
  HTTPS trust rows remain open. The agent completed this bounded batch;
  the third bounded batch is now preserved at core
  `44afdf1b14cff393c5b38e50a622b603b1d5a8ec`, evidence
  `cf553a5f9f1f1b23ed35e448dd7561fced2fb06b`, lane root
  `0fa94b20a4526780ca6c834e015fc0ae4ccf7931`; exact checks pass.
  Owned Pageant concurrency, process-pinned ECDSA/RSA-PSS hashes and actual
  URL-zone classes executed. Numeric HWND reuse was not observed within its
  cap and MD5 handshakes refused; no required assertion is silently waived.
  No provider/account provisioning or shared-host state change was performed;
  original evidence is frozen. Mac fixture inputs and cleanup guards are prepared; explicit operator
  approval for temporary account trust is pending. No trust was changed.
- Credential implementation runs GPT-6.1 Sol; both SSH clock reviewers completed GO;
  Windows' bounded residual agent is complete.
  Implementation and Windows release acceptance remain open. Next: finish and
  review the credential package, close Windows prerequisites, review/integrate,
  aggregate gate, then deferred platform/performance/selected-source/package
  batch. No push, tag, publication or alpha installation in this round.

## TR2.5 CLI/Python configuration — integrated and host-validated, 2026-10-03

- Reviewed CLI `90fdb108f2a91ead456e07053da108b721300cd3` and Python
  `5bf260d040964a3dd9ec606b58a625bc74ac4afc` are now in main with unchanged
  core `2e64e88a28c332ed422cc390adc76738dc701bb1`. Combined validation
  held root `0bd86ebe0eeb581df9b4933f3139bc292343a5bc` throughout.
- GWZ completed the Python root/member merge and the scoped CLI member merge
  `merge_op_65108_1790957378002_0001`. The earlier combined CLI root merge
  was aborted through GWZ because its managed integrity marker conflicted.
  Accepted CLI root reports/checkpoint were copied verbatim separately; no
  managed metadata was hand-edited. Its root history remains in tr2-5-cli,
  which must be retained until root history is integrated or preserved.
- Pinned Rust 1.95 combined checks pass: full ordinary/candidate CLI suites,
  strict CLI all-target Clippy in both modes and shared source guards;
  Python normal ordinary suite and both candidate-mode full suites, with
  strict Python-library Clippy in all three modes. Fresh candidate manifests
  and native extension paths name main, not copied lane editables. Neither
  validator changed tracked sources or history. Reports/commands/provenance:
  `/Volumes/projects/limbo/gwz-lanes-prewarm-20261003/main-cli-integration/`
  and `main-py-integration/` beside it.
- Python validation runs on the main venv's Python 3.12.12; candidate abi3
  builds use Python 3.13. Its existing cross-driver fixture explicitly
  rebuilds the root CLI target despite runner binary/target overrides. The
  final ordinary binary hash changes but source hashes/build provenance
  match the accepted tuple; this cache-ownership limitation is recorded.
- This is macOS host integration, not aggregate activation or release GO.
  Candidate external provenance, local Homebrew wheel dependencies, inherited
  CLI formatting/full-core candidate lint debt and platform/performance/
  selected-source/package qualification remain explicit limitations.
  Python help P3s join the next combined Surface package. All five parked
  root drafts were restored with their original hashes; private core/evidence
  drafts stayed untouched. No push, tag or alpha installation.
- Next: complete accepted credential/helper implementation and Windows
  baseline/design/implementation, integrate those lanes, then aggregate
  review and the deferred qualification/release batch.

## TR2.5 CLI configuration — implementation accepted, 2026-10-03

- Accepted at reviewed tuple root `52adfa0b8dba8752623d0ef5ada142105c48ac03`,
  CLI `90fdb108f2a91ead456e07053da108b721300cd3`, core
  `2e64e88a28c332ed422cc390adc76738dc701bb1` after original
  [Code](GwzTransportSettingCli-ReviewCode-1.md),
  [State](GwzTransportSettingCli-ReviewState-1.md) and
  [Surface](GwzTransportSettingCli-ReviewSurface-1.md) reviewers returned GO.
  Complete reports are filed verbatim. This accepts CLI configuration and
  reporting only, not combined integration or release/platform qualification.
- One remediation round closes all three reporting root causes and the
  lifecycle-documentation P3; no new architectural cause and no open finding.
  Rust 1.95 ordinary/candidate checks and source guards remain green; inherited
  formatting and full-core candidate lint debt remain separately identified.
- Next: integrate this accepted lane and the accepted Python configuration
  lane through GWZ, with merged validation. Credentials and Windows continue
  independently. No push, tag, publication or alpha installation.

## TR2.5 CLI remediation round 1 — settled for re-review, 2026-10-03

- Initial independent Code and State reviews found three blocking reporting
  root causes; Surface reported GO with one lifecycle-documentation P3.
  Complete raw reports and the combined remediation plan are filed verbatim.
- Root committed the single correction at CLI
  `90fdb108f2a91ead456e07053da108b721300cd3`. It fixes error and tag-list
  reporting, exact machine path identity and the paired global-file recipe.
  Full ordinary/candidate CLI suites, both strict CLI Clippy runs, focused
  red/green regressions and source guards pass on explicit Rust 1.95.0.
- Status: **NO-GO pending original-reviewer closure**, not implementation
  self-approval. Re-review uses the same Code/State/Surface reviewers and
  unchanged core `2e64e88a28c332ed422cc390adc76738dc701bb1`. Accepted design,
  platform/performance deferrals and inherited lint/fmt debt are unchanged.

## TR2.5 step 2 CLI — implementation draft, 2026-10-03

- Resumed the isolated `tr2-5-cli` draft on the operator's GPT-6.1 direction.
  Core's accepted resolver is unchanged. The CLI adds candidate-only
  `--transport gwz|native`, driver snapshot resolution, ignored-value notices,
  verbose selection text and optional `meta.transport_setting`. Native skips
  local transport runtime creation and fills omitted limits with 50 jobs,
  8 per host and a 3 second timeout; explicit values are preserved.
- Help and user docs follow accepted TR1.5 §10. Ordinary help/JSON are retained.
  Local unit and real-binary workflows cover the settings, defaults, notices,
  machine output and non-network immunity. Switch/process-global inventories
  and conditional boundaries pass. Both full CLI suites and ordinary/candidate strict CLI Clippy
  pass on explicit Rust 1.95.0. Earlier 1.96 runs also passed; they are
  supplemental receipts, not substituted for the pinned runs.
  Inherited formatting debt in `src/tests/g02/partial_errors.rs` is unchanged.
  Full-core candidate Clippy debt is not waived.
- [Package and limitations](GwzTransportSettingCli-Implementation.md),
  [canonical review inputs](GwzTransportSettingCli-ReviewInputs.md). Root owns
  settlement and independent Code/State plus Surface dispatch. The root owner
  committed CLI `c4a588f8be4e91926deff2156dce00c7062f4184` via GWZ;
  review is pending. The candidate/test-confined `transport_tests.rs` path
  edge is acknowledged in this settlement record. No dirty-tree acceptance
  review, merge, push, tag or installation occurred.
  Platform, route/fixture timing and performance qualification remain deferred.


## TR2.5 Python configuration — implementation accepted, 2026-10-03

- Accepted at reviewed tuple root `b256a791f1dd05c04caedd8391ef146ff4d3d8fa`,
  Python `5bf260d040964a3dd9ec606b58a625bc74ac4afc`, core
  `2e64e88a28c332ed422cc390adc76738dc701bb1` after independent
  [Code](GwzPyTransportSetting-ReviewCode.md),
  [State](GwzPyTransportSetting-ReviewState.md) and
  [Surface](GwzPyTransportSetting-ReviewSurface.md) reported GO. Reports are
  filed verbatim. This accepts the Python configuration/driver implementation
  only; it is neither integration nor release/platform qualification.
- No blocking findings and no remediation round. Surface P3-1/P3-2 record
  missing transport-dependent concurrency defaults and selection/undo text
  in Python CLI help; include these bounded documentation corrections in the
  next combined surface update, not a new standalone work package.
- Pinned local checks and unchanged shared contracts are recorded below.
  No merge, push, tag or alpha installation. Next: integrate the accepted
  Python lane alongside accepted CLI work, then combined validation.

## TR2.5 Python configuration lane — ready for settled review, 2026-10-03

- `tr2-5-py` implements the accepted off-switch design's Python driver seam:
  per-operation environment/global configuration resolution; ignored values
  logged once per file per Client; native selection's standard UserWarning;
  native 50/8 defaults; constructor host-limit default None with explicit 32
  honoured; the existing one 9-second clock; and Python CLI note/help text.
- No shared-core, Rust CLI, credential, wire or platform changes. The existing
  candidate route boundary encloses the setting implementation; ordinary
  builds keep an empty notice context. No new candidate switch site is added,
  and the inventory guard passes. Independent transport message forwarding
  and the per-operation runtime remain unchanged.
- Pinned Rust 1.95.0 gates: ordinary normal runner 992 passed / 18
  candidate-only skips; both-switch whole suite 1,010 passed; Rust unit
  suites 25 ordinary / 29 candidate; strict Clippy passes on both Python
  target shapes. Final rebuilt focused suites pass 56 and 8; the
  transport-only network/settings integration passes 18. Regeneration,
  process-global, switch inventory, conditional boundaries and changed-file
  formatting pass. Two equivalent lint corrections followed the broad runs;
  rebuilt extensions and final focused gates verified those source forms.
- The lane virtualenv's copied main editable was replaced with this lane's
  editable build. Fresh candidate manifests and module paths name this lane;
  candidate extension builds target Python 3.13. The ordinary runner uses
  the lane virtualenv's Python 3.12. An initial default-stable Rust 1.96 run
  is superseded by the explicitly pinned final gates.
- [Exact implementation, commands, provenance and scope](../gwz-py/dev-docs/GwzPyTransportSetting-Implementation.md).
  Status is **settled, not accepted**. Root committed Python
  `5bf260d040964a3dd9ec606b58a625bc74ac4afc` through GWZ. The lane owner
  records the tuple and runs independent Code, State and Surface reviews from the
  canonical review-loop prompts. No dirty-tree acceptance or self-GO is
  claimed; bounded combined remediation and the original reviewers'
  counterexample verification apply. No commit, merge, push, tag or alpha
  installation was performed by the drafter.


## Five-file split, 2026-10-03

- Completed sequentially in main at the operator's request, using the
  `split-files` skill and rust-split 0.2.1. No parallel lane or agent was
  started. Responsibilities now sit in private cohesive modules:

  | Original file | Before | Facade | Largest child | Responsibilities |
  | --- | ---: | ---: | ---: | --- |
  | checked_artifact/entry.rs | 1,080 | 330 | 238 | observation, artifact facts, recovery, catalog |
  | git/gitbackend/contract.rs | 1,067 | 131 | 210 | eight Git method groups |
  | git/gitbackend/fake_repository.rs | 1,079 | 42 | 226 | eight fake-backend method groups |
  | transport_host/session/driver.rs | 873 | 220 | 356 | opening/admission and message pump |
  | git/endpoint/https_worker_tests.rs | 959 | 62 | 365 | discovery, exchange, failures, pooling, proxy |

- The oversized trait and fake trait implementation cannot be divided into
  multiple implementations of the same trait. Private method-group macros
  preserve one trait and one implementation, with the original methods,
  signatures and order. Inherent Session methods use separate impl blocks.
  The method macros and three widened private helper signatures have
  `rustfmt::skip` to preserve moved text; no formatter rewrote the payloads.
- rust-split's explode chunks reconstruct all five originals byte for byte.
  A syntax-span helper accounts for all 120 trait methods, 75 fake methods
  and six Session methods. The relocation checker finds all 294 recorded
  payloads exactly once: 275 exact and 19 with necessary visibility, import
  depth or test-module path adjustments. Reversing those adjustments restores
  each original exactly; deletion, duplication and alteration probes fail
  the payload checker. Work files and proofs are outside
  repositories at `/Volumes/projects/limbo/gwz-core-split-five-20261002/`.
- The entry facade preserves its 24 visible names and nine classified
  consumers. Its raw record writer stays in entry.rs. The four exact private
  parts retain the writer lint boundary and filesystem/context checks;
  facade re-exports and the five HTTPS test path edges are inventoried in
  the same source commit. Mutation checks cover extra children, added public
  APIs, wildcard/extra exports, removed writer protection and raw-write leaks.
  The pre-existing owner-side lease/import scan hole remains explicitly
  recorded in the checker; this split does not claim to repair it.
- Validation: the ordinary full runner passes (2,103 real tests, the
  filesystem/fake-backend groups and 69 integration tests). The candidate
  full run passed all functional tests and 69 integration tests, with one
  source-location pin failure: the record-root positive control gained the
  moved catalog file. Its exact file inventory was updated and its targeted
  rerun passes. The forced-merge and catalog-activation pins pass too.
  All seven entry-boundary mutation tests and six filesystem checker tests
  pass. Formatting, checked-artifact, filesystem, conditional-boundary and
  process-global checks pass. Ordinary strict Clippy passes with the expanded
  filesystem lint configuration. The previously recorded candidate Clippy
  debt is not waived or claimed fixed.
- Source commit: core `2e64e88a28c332ed422cc390adc76738dc701bb1`.
  Its per-commit boundary gate passes over the exact committed tree.
- Release/platform qualification remains a later batch. Nothing here
  activates Windows or changes protocol, authentication, pool or retry
  behavior. The previously merged tr2-4-7 lane has since been disposed
  without --keep, with explicit authorization for unpreserved history;
  its directory is gone and the workspace family contains only root.
  No push, tag or alpha installation was performed in this step.

## TR2.4/TR2.7 merged after bounded review, 2026-10-02

- The operator requested a quick plan/code review and merge if suitable.
  Both steps remain in the release plan: TR2.4 in the base plan and TR2.7
  in accepted amendment 1 §3.5, retained by amendment 2. Neither was abandoned.
  The skim found no blocker to integration; TR2.6's combined implementation
  review remains outstanding.
- Accepted the lane's two interpretations: enclosing modules satisfy the
  explicit conditional boundary rule without adding a dependency; a mixed
  `[1,2,3]` offer selects profile 2 when the unstable feature is disabled.
  A profile-3-only offer still refuses with `UnsupportedVersion`, no effects,
  and a profile-3 Bound cannot be accepted in that build.
- Merged `tr2-4-7` through GWZ, operation
  `merge_op_15344_1790946495458_0001`. Transport fast-forwarded to
  `35475977530171ab77ee2fbb1e8128f938acb5ae`; core merged at
  `c252e65332a24f45b7afb2efc0768799c5fd5c71`. The only conflict was test
  module registration: both `retry_tests` and `ca_bundle_tests` were retained.
  Core `28e72cacb99472fe6cbc16aaf2023eaaf1e3dd92` corrects that registration's
  formatting and adds the historical alpha document's CA-bundle erratum.
  All lane repository heads are now ancestors of main; other member heads
  are unchanged. Stable zero-context patch IDs match for the core changes
  outside the resolved module registration.
- Validation: transport's full default and `unstable-sequenced` suites pass,
  including seeded ordering tests. On merged main with both candidate
  switches, all three macOS CA-bundle tests pass. Core/transport formatting,
  checked-artifact boundary, conditional boundaries, candidate inventory and
  process-global checks pass; per-commit boundary checks cover the lane and
  merge commits. Linux additive-root execution and Windows qualification
  remain in the deferred platform batch. Existing strict-Clippy debt is not
  waived. The extra CA step size is predominantly regression fixtures/tests:
  368 changed lines, about 75 production, as the handoff already recorded.
- All 15 temporarily parked paths were restored; file hashes match the
  manifest in `/Volumes/projects/limbo/gwz-tr247-merge-parking-20261002/`.
  No evidence contents changed. TR2.8 was previously detached with `--keep`;
  `tr2-4-7` remains registered, now fully merged, with its files retained.
  The preceding integrated tuple was pushed; this merge has not been pushed,
  tagged or installed in `gwz-alpha`.

## Ready transport lanes integrated into main, 2026-10-02

- Integrated through GWZ 1.0.17, in order: TR2.1 (`tr2-1`), TR2.5 step 1
  (`tr2-5a`), and TR2.8 (`tr2-8`). Accepted lane heads are ancestors of main.
  The settled source tuple is root `21c7becab6bd`, core `ea60286415f9`, CLI
  `0164e66376da`, Python `b2369f1d0bf7`; core `63801e84` then files the two
  retry wire reviews without changing source. This record's closing commit
  advances root only, with GWZ's generated member lock update.
- TR2.1 brings the retry machines and all five reviewed follow-ups, including
  the per-attempt cleanup allowance and the SSH/HTTPS help correction. Merge
  refuses `max_retries` with its established `MergeValidationFailed` code.
  The [Code](../gwz-core/dev-docs/GwzTransportRetryWireField-ReviewCode.md) and
  [State](../gwz-core/dev-docs/GwzTransportRetryWireField-ReviewState.md) wire
  reviews are filed verbatim. TR2.5 is the configuration resolver foundation
  only; its CLI/Python activation remains to be implemented.
- TR2.8 conflicted only in `ssh_fixture.rs` and `ssh_tests/mod.rs`. The
  resolution keeps both the retry lane's server startup controls and the key
  lane's configuration/logging controls, and registers both test modules.
  Production changes merged automatically. Zero-context stable patch IDs
  verify both sides outside these two resolved fixture files and the
  GWZ-generated metadata. The earlier TR2.8 acceptance below remains valid.
- Validation on the combined tree: full core suite with both candidate
  switches and full ordinary suite pass; the transport-only core test build
  passes; CLI ordinary 253 + 92 and candidate 254 + 92 pass. Consumer Python
  tests pass (18 tooling/package and 9 candidate), candidate regeneration
  matches, and the isolated archive consumer passes 32 including 11 request
  compatibility tests. Python with both candidate switches passes all 1,000
  tests. Source guards, core formatting, ordinary strict Clippy and the
  per-commit boundary gate through core `63801e84` pass.
- Candidate strict Clippy remains red (129 diagnostics on the pinned
  compiler; none in the two resolved fixture files). CLI formatting remains
  red only in `src/tests/g02/partial_errors.rs`, byte-identical to pre-merge
  main. Neither is waived or presented as passing.
- The first merge refused before registration because the retained catalog
  target differed from this moved checkout. On explicit operator approval,
  its complete old catalog was moved outside the workspace with hashes
  verified; GWZ initialized a fresh catalog and merged successfully.
  [L10](GwzLaneIssues.md#l10-a-moved-workspace-retains-a-catalog-bound-to-its-previous-target)
  records the limitation. All 24 parked drafts were restored with matching
  SHA-256s. The old catalog and three TR2.1 build caches remain preserved in
  `/Volumes/projects/limbo/gwz-merge-parking-20261002-ready/`.
- TR2.1 and TR2.5a have no unique commit, reflog or stash history in any
  repository compared with main. They were detached using `gwz local dispose
  --keep`: their directories, drafts and evidence copies remain intact.
  TR2.8 stays registered; TR2.4/TR2.7 stays unmerged pending its two decisions.
  No evidence-member contents were changed. Nothing was pushed or tagged.
- Gate logs, patch comparisons, catalog hashes and the preservation audit
  are in `/Volumes/projects/limbo/gwz-handoff-2026-10-02/logs/merged-ready-20261002/`.
  This is integration acceptance, not release qualification. Remaining work
  includes the planned five-file split, CLI/Python configuration integration,
  credential-helper work, Windows parity and the release/platform/performance
  batch; hardware-key execution still needs the operator's go.

## TR2.8 lane — implementation accepted, 2026-10-02

- The stopped `../gwz-dev-tr2-8` work is completed and reviewed: core
  `c51b1b5ff75a772f42847ce22fa4f35a61cf4497`, root review baseline
  `2f65e3c898fdd9c9eb78b7557339613aab1c8ba7`. Both
  [Code](../gwz-core/dev-docs/GwzTransportSshKeyTypes-ReviewCode-1.md) and
  [State](../gwz-core/dev-docs/GwzTransportSshKeyTypes-ReviewState-1.md) report GO.
- Agent keys/signatures, certificates, security-key software fixtures, RSA's
  three SHA-1 cases and selected-key container parity are implemented. Two P2
  defects and one P3 found during review were closed in one remediation round.
- Original ordinary/full candidate suites passed; corrected SSH suite 153/0;
  corrected full suite with both candidate switches passed. Source guards,
  format and per-commit gates pass. Strict Clippy remains red on inherited
  diagnostics outside this lane's changed files; it is not waived or called green.
- [Implementation and exact validation matrix](../gwz-core/dev-docs/GwzTransportSshKeyTypes-Implementation.md).
  This accepts implementation only. Hardware-key manual execution needs the
  operator's go; Windows bridge work and release-platform qualification remain
  in the release plan. No merge, push or tag has been performed for this lane.
- Next: integrate the ready lanes, preserving this acceptance record; run the
  outstanding platform/hardware/strict-lint checks in the release batch.


## Transport release — split into 1.1.0 and 1.2.0; amendment 2 accepted, 2026-10-01

- **Operator decision OD13 (2026-10-01).**
  - The operator's words: "we need an intermediate release or a way we can parallelize huge chunks", then "1.1 will need windows parity too - releases - requalify 1.1.0 with windows support".
  - **1.1.0** ships the transport, used in process by the `gwz` CLI, on macOS ARM64, Linux x86-64 and Windows x86-64, with Windows parity.
  - **1.2.0** ships the session host, reuse across operations and commands, the server, and gwz-py on the transport.
- **Measured before the decision:**
  - **Code.** Since `v1.0.17` (2026-09-18), about 29,200 production lines and 42,000 test lines were added. About 23,600 of the production lines are the transport, written from 2026-09-19 to 2026-09-24. The plans budget about 44,000 production lines still to write, 38,300 of them in the session plan.
  - **Speed.** A no-op fetch of 32 small public repositories over SSH, on macOS. The candidate reuses connections: at `--max-per-host 4`, it ran 32 fetches over 4 connections in 8.1 s, against 18.3 s for 1.0.17. At its defaults it is 2.8 times slower than 1.0.17 at `--max-per-host 32`: 7.0 s against 2.5 s. There are two causes:
    - `MAX_OPEN_JOBS = 8` (`placement_endpoint.rs:31`);
    - stream closes that complete one at a time, about 0.3 s apart. Their root cause is not yet found.
  - The candidate CLI still builds from main, though no CI job builds it.
- **[Amendment 2](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2.md)** to the transport release plan was accepted at SHA-256 `c5850e52…`, after three rounds of the dual Consistency and Safety review and two remediation rounds ([verdict](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2-Verdict.md)). After the GO it carries the corrections the reviewers cleared without a further round, and hashes `4da27115…`.
  - **New steps:**
    - TR1.8, the Windows parity design;
    - TR2.9, concurrent closes;
    - TR2.10, open admission;
    - TR2.11, no transport without a host context;
    - TR2.12, the second switch `gwz_session_candidate`;
    - TR3.3, the thirteen crates.io names;
    - TR3.4, gwz-py's registry pins;
    - TR4.6, Windows CI on push;
    - TR4.7, the Windows review;
    - TR4.8 to TR4.10 (revision 4): Pageant, the WinHTTP machine proxy, and the logon session's default credentials, in the transport;
    - TR8.4, Windows parity rows.
  - **Rule (e):** until 1.1.0's tag, a session-plan step marked **Ordinary path** merges only with a review that accepts its change for 1.1.0, and only once this checkpoint lists it under (d).
  - **The review found three upgrade breaks before any code:**
    - gwz-py would have moved onto the transport;
    - the Windows SSH home came from `HOME` alone;
    - the Windows default-credential route had no bound.
  - **Status edits on acceptance:** the plan, amendment 1, the 1.1.0 amendment, the session plan, the crate map, the reuse design and the server design.
- **TR3.1 is closed.** Its exit holds in the tree: the rename at git2-rs `d13951f` and gwz-git `a9d7ee0`, and the candidate build in CI.
- **OD14 decided by the operator (2026-10-01): its alternative.** gwz-py's network operations take the per-operation transport entry in 1.1.0. The operator's words: "go with calls in with_local_transport now and don't wait for the review-loop".
  - Revision 3 of amendment 2 (`432d7118…`) restores the 1.1.0 amendment's S6.1–S6.3 for 1.1.0 (§3.17).
  - The new [Python design](../gwz-py/dev-docs/GwzPyPerOperationTransportDesign.md) adds four things: the environment snapshot taken under the GIL; panic safety, since `Command`'s `Drop` finishes while unwinding; one transport predicate, since the CLI's and the bridge's network sets differ on `attach_repo_member` and `repo_sync`; and the bridge's cancellation, `close()` and interpreter-exit behaviour.
  - It skips the review loop. The [skim review](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2-ReviewSkim.md) found six P2 and three P3 text defects: among them, waits that would hold the GIL and deadlock with a failing operation, and an off switch that did not reach gwz-py. All nine are applied. On the operator's question "can we remedy the no-go issues?", the same reviewer re-checked them and reported **GO** ([re-check](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2-ReviewSkim-1.md)). Its four new P3s are applied. The amendment now hashes `c5561fc5…`, and the design `3d656d1a…`.
- **OD15 decided by the operator (2026-10-01): Windows parity is built into the transport.** The operator's words: "we can't do native only", then "I said we need to support windows parity, why is this in question?". Native routes are not the way to parity: they get no pooling or reuse, and cannot serve a server on another machine or client placement.
  - Revision 4 of amendment 2 applies it. TR1.8 designs, in the transport, Pageant's window protocol (a visible Pageant first, as 1.0.17), the WinHTTP machine proxy, and SSPI for the logon session's default credentials. TR4.8–TR4.10 implement them, and TR4.10 gets its own dual review.
  - OD16 keeps the zone bound, now applied by the transport: your Windows login goes only to Local Machine, Intranet and Trusted-zone hosts, and other hosts are refused, naming the Trusted zone and then the off switch. This is the one parity exception; the operator can lift it.
  - §3.18 changes the server design's Pageant bullet and moves the Windows logon session into every session's must-match rows (1.2.0). The session plan's CS8.18 and CS8.19 follow, and both documents' status lines and changelogs record it.
  - Revision 4 is skim-reviewed only, as revision 3 was. [Skim review 2](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2-ReviewSkim-2.md) found four P2 and four P3 text defects. The most serious: the Windows-login zone check had to use the URL after a discovery redirect, or an intranet URL redirecting to an Internet host would have sent it the login's NetNTLMv2 response. All eight are applied, and the [re-check](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2-ReviewSkim-3.md) reported **GO**. Its one new P3 is applied. The amendment hashes `ee130a1d…`, the server design `9fe1738b…` and the session plan `43952950…`.

  OD17, ship, hold or add channels, is taken only if TR8.1 (1.1.0) misses.
- **Committed 2026-10-02 on the operator's go** ("go = commit"): amendment 2 through revision 4, its reviews and plans, the verdict, the Python design, and the status edits (root `1292b0c`, gwz-core `fde46265`, gwz-py `b24204e`). Not pushed.
- **Decided by the operator on 2026-10-02:**
  - **OD16: no zone bound** ("lift that"). The transport offers the logon session's default credentials to any host that asks, as 1.0.17 does. The migration notes state the forced-authentication hazard.
  - **OD10 and OD11 reversed** ("yes"): the transport runs the user's configured credential helpers (TR1.6), and signs with every agent key type and signature algorithm 1.0.17 uses, security keys and certificates included (TR2.8). Under the same parity rule, TR2.8's list also covers `ssh-rsa` (SHA-1) for a server that offers no SHA-2 RSA algorithm, as libssh2 does.
  - **The SHA-1 fallback,** confirmed by the operator ("keep sha-1 fallback"): `ssh-rsa` exactly where libssh2 1.11.1 uses it.
  - No configuration that 1.0.17 serves takes a native route in 1.1.0. Revision 5 of amendment 2 applies all of this (its §3.19), skim-reviewed only. Its [skim review](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2-ReviewSkim-4.md) found five P2 and three P3 text defects. Among them:
    - TR1.6's helper runner is the session plan's CS3.4 `git credential fill` spawn;
    - a helper is asked about the URL its credential goes to, after a redirect;
    - on Windows a helper's password also answers `NTLM`, `Negotiate` and `Digest`, as 1.0.17 does;
    - the `ssh-rsa` rule is libssh2's exact one.

    All eight are applied. The [re-check](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2-ReviewSkim-5.md) closed them, and found one new P2: a `Negotiate`-only challenge takes the logon session with no helper asked, as on 1.0.17. It also found three new P3s. All four are applied, and the [second re-check](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2-ReviewSkim-6.md) reported **GO**. The amendment hashes `807dda1e…`.
- **Drafts in the way of merges:** the operator's rule (2026-10-02) is to move them aside if they block. Move, never delete; restore after the merge; never touch gwz-core-evidence. The uncommitted `repository` line in `gwz-transport/Cargo.toml` is a tracked edit, not a draft, and is asked about if it blocks.
- **Lanes started 2026-10-02 on the operator's go** ("Lanes - go"). TR2.9 runs in `../gwz-dev-tr2-9` and TR2.10 in `../gwz-dev-tr2-10`, each with an Opus implementer. The disk was 97% full, so two lanes run at a time, each on a copy-on-write clone of a warm candidate target. `prepare.py` refuses a destination inside the workspace, so the candidate manifest goes in a scratch directory.
  - **TR2.9 merged** (gwz-core `935f8ab0`, merging lane `61384fee`; root `eebd272`). The cause was in gwz-core, not gwz-transport or libgit2: `PlacementEndpoint::flush_attachments` read on past a stream's `Closed`, found the bridge its worker drops, and queued a false `Failed` each pass. Those filled the 64-slot outbound queue, which the session drains one message a pass, so closes left one at a time about 0.4 s apart. The endpoint now stops at a stream's terminal message (+9/−7 lines), and the uncalled `requeue_outbound` is gone. Test: N streams close against a fixture that delays each close by 1 s; for 32 streams that took 5.0 s before, 1.24 s after. Candidate suite: 2,519 passed, and one failure, the known intermittent SSH refusal test, which passed on rerun. The merge needed the six drafts moved aside; they were restored with matching checksums.
  - **TR2.10 merged** (gwz-core `e3c0449d`, merging lane `0214de20`; root `42a7330`). `MAX_OPEN_JOBS` is gone. An open starts while the opens in flight stay within the operation's per-host and per-user limits, which the transport host installs in the pool, and within `open_ceiling`: the least of the pool's total and request ceilings, `MAX_REQUESTS`, and half the 64-permit supervised-job budget. The dead `Request.deadline` was removed. Test: 32 opens against a fixture with 400 ms setups all start within one setup; before, the last started 1.21 s in, in waves of 8. Candidate suite in the lane: 2,527 passed, 0 failed. `placement_endpoint.rs` is now 1,095 lines. A full run of the merged main, TR2.9 and TR2.10 together, is under way.
  - **Raised by TR2.10's agent, for the operator:**
    - at the defaults, 32 opens use all 64 job permits, since each open holds one and starts a setup job. A setup or identity check refused at that moment can close the whole session. The agent recommends that opens stop holding a permit, about 150–250 lines;
    - the retry plan's Cold state (§5: one setup per key until its first success) is per operation. In 1.1.0, with no reuse across commands, every command then waits one full SSH login before its other members open. That threatens TR8.1, and it conflicts with TR2.10's test once TR2.1 lands;
    - `placement_endpoint.rs` (1,095 lines) and `ssh_worker.rs` (1,279) are past the 1,000-line review mark, and 1.1.0 keeps changing them. The session plan's CS7.1 splits them only in 1.2.0;
    - the separate `tests/transport_ssh` crate compiles `placement_checks.rs` but is not in CI, and no one builds it.
  - **Merged main, TR2.9 and TR2.10 together:** candidate suite 2,527 passed, 2 failed, 1 ignored. Both failures are `diff::tests::t_rename` tests. Their fixture's `TempDir` names directories from the process ID and a microsecond clock, so two parallel tests got the same directory. That flaw predates the transport work, and the TR2.3 lane fixes it with a regression test.
  - **TR2.12 merged** (gwz-core `6c01af04`, merging lane `0df0bfcb`; gwz-cli `af2240d` and gwz-py `df6b59d`, fast-forwarded; root `8a8a056`).
    - `gwz_session_candidate` is declared in all three check-cfg declarations. The process-globals definition names it.
    - The candidate job is a two-leg matrix: the transport switch alone, and both switches. `main` has no branch protection, so the new check names need no settings change.
    - A new checker, `check_candidate_switches.py`, compares each repository's switch sites with its inventory, and each repository's tests run it. It passes on merged main.
    - Inventory digests (SHA-256), all 22 sites `gwz_transport_candidate`:
      - gwz-core `scripts/candidate_switch_inventory.txt` `937330c9b3960a3d3d399078b3999e9eea6de3280e58127aa94ac5c9d67da965` (18 sites);
      - gwz-cli `2bdad8b46911d82e03c89272e8bd37f085abbcff9613fd87cf7e305fa619f978` (2);
      - gwz-py `2ab61cd393bacd0d77765ffaaf94ed047b12951fb77da13b86c615b6262a4a78` (2).
    - **Push order:** gwz-cli's and gwz-py's tests run the checker from gwz-core's `main`, so gwz-core reaches GitHub first, or all of them go in one push.
  - **TR2.11 merged** (gwz-core `1943e34b`, merging lane `4cf0c623`; gwz-cli `ee8b365`; root `c09f5b0`).
    - `transport_binding::configure` installs no route without a host context, so `Git2Backend::new()` takes libgit2's native route for SSH as for HTTPS. The lazy endpoint, its `HOME`/`SSH_AUTH_SOCK` factory and its `env` debt entry are gone.
    - Candidate tests install a host context.
    - A new source test holds gwz-cli's `transport_meta` arms equal to gwz-core's `with_transport` call sites. It removed the stray `RepoSync` arm, as the Python design's §2.1 had said.
    - Suites: candidate 2,519 passed; gwz-core ordinary 2,315 passed; gwz-cli 336 passed. On merged main, the inventories and the process-globals guard (23 entries) pass.
  - **From TR2.11's agent:**
    - **Dead code.** The blocking SSH open path now has no production caller: `ssh_endpoint.rs`'s `Route`, `ssh_local::connect`, the worker's blocking `open*` family and four `open_endpoint_*` wrappers. The module-level `#![allow(dead_code)]` hides it. Its only users are 24 call sites in `tests/transport_ssh`, a qualification crate that nothing builds. Under the remove-dead-code rule, a step after TR2.13 moves those tests onto the production attachment path or drops them, removes the code and the `allow`, and either puts the crate in CI or retires it.
    - **Parity defect.** The transport percent-decodes an SSH URL's path, but libgit2 passes it as written, so a remote with `%XX` in its path reaches another repository. That is TR2.16.
  - **Merged main, TR2.9–TR2.13:** candidate suite 2,529 passed, 1 failed. The failure is `cli_one_request_supports_concurrent_git_streams`, 100 concurrent clones on one request. It fails 5 of 5 runs in isolation, within 0.2–1.5 s, with `ssh setup failed: CarrierLost` or `Io`. It passed in the TR2.10 lane and in the TR2.13 lane, so the cause is their interaction. Raising the fixture sshd's `MaxStartups` to 200 did not help, so it is not sshd's connection throttling. TR2.13's agent is diagnosing and fixing it in `../gwz-dev-fix-concurrency`. If the cause is the job budget, the fix is TR2.14.
  - **1.1.0 S6.1 merged** (gwz-core `1ef7ede3`, merging lane `0bad1245`; root `e72c404`).
    - The new entry is `transport_host::with_cancellable_local_transport(meta, operation, &EnvironmentSnapshot, &CancellationToken, action) -> (ModelResult<T>, CleanupReport)`.
    - It refuses with `Cancelled` before building once the token is cancelled, and attaches the request's cancellation to the token at registration.
    - It takes `SshEndpointConfig`, `HttpsEndpointConfig`, the CA, proxy and `no_proxy` settings and `gh`'s environment from the snapshot, through the new `endpoint_environment.rs`. `with_local_transport` now takes a snapshot of `vars_os()` and goes the same way.
    - **Panic safety:** a panic in the operation, followed by one in finish, no longer aborts the process with "panic in a destructor". It reports `internal_error`, with cleanup unconfirmed, and the next operation succeeds.
    - Suite in the lane: 2,528 passed, 0 failed.
  - **From S6.1's agent:**
    - **Cancel latency.** After a cancel the read ends within about 1 ms, but finish then waits out the host's 5 s cleanup bound. The driver session retires a request cancelled mid-exchange only at that bound, so gwz-py's `cancel_operation` would take about 5 s. The fix lane re-measures it after TR2.13.
    - `transport_host` now re-exports `CallControls` and `CancellationToken`, for S6.2.
    - A `~/` identity still resolves against the process's `HOME` (`identity.rs`, allowlisted debt that CS3.2 owns). For gwz-py it should follow the snapshot; that is S6.2's to settle.
    - `SshEndpointConfig::from_environment()` still reads `HOME` alone. TR1.8's Windows home order must cover it, or it is retired.
    - The snapshot code's platform `cfg_if!` has a `compile_error!` arm. It stops S4.5's Windows candidate build until S4.4, TR4.8 and TR4.9 write the Windows arm (`endpoint_environment.rs:153-185`). That is intended.
    - The copy of `gh`'s environment is not zeroized, as before.
  - **TR2.16 merged** (gwz-core `ff2bf4f2`, merging lane `fba1b598`; root `3d5629f`).
    - The transport parses SSH destinations as libgit2 1.9.7 does, the libgit2 1.0.17 shipped, with both its `ssh://` and its scp-like parser, its percent-decoding and its `/~` and leading `-` rules.
    - Where libgit2 refuses a URL before connecting, the transport refuses it before any open.
    - `!` in the remote command is quoted as `'\!'`, libgit2's fix for CVE-2026-5917.
    - Fixture test: 11 URL forms on both routes. Before the fix, the transport reached a decoy `a b.git` for `a%20b.git`.
    - A scratch differential run against libgit2's own parsers found no mismatch over about 1.6M generated URLs.
    - Candidate suite in the lane: passed.
  - **Open from TR2.16:**
    - a URL password beside a user (`user:pw@host`). 1.0.17 uses it when the server offers password authentication, and keys otherwise; the transport refuses it;
    - control characters in an SSH path. libgit2 sends them; the transport refuses them, as a safety bound;
    - host-name case. The transport lowercases the host for pooling and the `known_hosts` lookup, where libssh2 matches case-sensitively, so a hand-written mixed-case `known_hosts` entry works on 1.0.17 and fails on 1.1.0. A case-insensitive match serves both cases;
    - scp-form IPv6 addresses are now refused, as on 1.0.17. The development transport had accepted them;
    - `GwzRemoteTransportSshWorker.md:17-19` still says destinations "decode URL escapes once". It is a reviewed record, so it gets an erratum.
  - **TR2.3 merged** (gwz-core `4edfd78b`; gwz-cli `84be975`; gwz-py `f471cb6`, fast-forwarded; root `23c5d57`).
    - On a `Partial` result, the top-level `errors` copies the full error of each `Failed` or `Rejected` member, in member order. 1.0.17 printed `[]`.
    - Human output and exit codes are unchanged. `MachineOutput.md` gains a "Partial results" section, and gwz-py's `member_errors` is filled.
    - The `TempDir` collision fix landed before it (gwz-core `24980055`).
    - Suites: gwz-core ordinary 2,325 and candidate 2,533 passed; gwz-cli 246 unit and 92 integration; gwz-py 928.
    - It changes a documented contract, so it ships only with its Surface review (S7.5).
  - **Open from TR2.3:**
    - whether the copy also applies to `Failed` and `Rejected` aggregates (the plan says `Partial` only);
    - `Skipped` rows are not copied;
    - stash's human report still shows no member error text;
    - gwz-core's `OperationRuntime`, `ResponseBuilder::result` and `ExecutionReport` have no caller outside their own tests, but they are public API;
    - gwz-py's CLI shows less than gwz-cli on a non-success result, as before;
    - 1.1.0's release notes need an unreleased section in `gwz-cli/docs/Releases.md`.

    Six more fixtures share the `TempDir` flaw, and gwz-cli has one `needless_update` clippy failure, which no CI job runs: the `cleanup-1` lane fixes both.
  - **The concurrency regression is fixed, with TR2.14** (gwz-core `ab5fee76`, merging lane `5796282e`; root `2106a5a`). The cause was TR2.10's 32 concurrent opens outrunning three limits each open's setup needs; TR2.13's prompt sessions exposed it:
    - **The 64 supervised-job permits:** an open briefly held up to three. A refused identity check closed the endpoint session, so every pending open failed as `CarrierLost`.
    - **The key registry's 16 MiB:** each key read reserves about 1 MiB, so only 15 fit, and the 16th failed `WouldBlock`.
    - **The fixture sshd's default `MaxStartups`:** it drops unauthenticated connections beyond 10, as `Io`.

    The fix:
    - an open no longer holds a job permit while it waits for the worker's reply (TR2.14). The endpoint hands it to the worker and polls, the reply wakes the session, and the old thread-and-job open path is removed;
    - an identity check refused by a full budget waits for a later pass;
    - a key read waits for a registry reservation within its open's deadline;
    - the fixture sshd accepts 64 unauthenticated connections.

    Every bound is unchanged. The 100-clone test passes 10 of 10 in isolation, and the new `budget_wait_tests.rs` adds two deterministic tests, both failing on the old main. Candidate suite in the lane: 2,531 passed, 0 failed; the proof crate 311 passed. A run of the fully merged main is under way.
  - **From the fix's agent:**
    - **Stock OpenSSH servers drop setups beyond 10 pending** (`MaxStartups`), and the transport now opens up to 32 to one host. 1.0.17 opened up to `--jobs` (100), so the exposure is not new, but the old 8-open limit hid it. The choices: cap concurrent setups per host near 10, rely on TR2.1's retry, or document it.
    - A setup refused by the job budget still fails its member. Finished jobs return their permits up to one reaper sweep late, and as many as 10 were seen at once. Releasing a permit as soon as its result is consumed would close that.
    - Key reads now queue past 15. Reserving each key file's real size would admit more, but changes a security bound.
    - **Cancel latency (S6.1) still holds after TR2.13:** finish waits out the 5 s cleanup bound after a cancel mid-exchange. It needs its own step before gwz-py's `cancel_operation` ships.
  - **Fully merged main is green.** Through the concurrency fix (gwz-core `ab5fee76`), the candidate suite gives 2,549 passed, 0 failed, 3 ignored.
  - **cleanup-1 merged** (gwz-core `bd13b665`, merging `f9546477`; gwz-cli `602a79f`, fast-forwarded; root `5ae60e6`).
    - One atomic `unique_dir`/`TempDir` helper in `gwz-core/src/test_support/` replaces the pid-and-clock and pid-and-counter temp names across gwz-core's tests: the six reported fixtures and about 30 more. The global `TEMP_SEQUENCE` and its two allowlist entries are gone.
    - gwz-cli gets its own copy of the helper.
    - gwz-cli's hook reflink probe creates its seed file with `create_new`, a small production change.
    - The `needless_update` clippy failure is fixed.
    - gwz-core's gate and gwz-cli's tests pass, and the process-globals and cfg-boundary guards report nothing new. `src/filesystem/native/facts/linux.rs` is reviewed by eye, since only the Linux CI leg compiles it.
    - **Open:** the helper exists three times (gwz-core, gwz-cli, copy-contract); gwz-cli's CI runs no clippy.
  - **TR2.15 done in its lane** (gwz-core `d9239566`, root `46a4264`). It is being merged by hand onto current main in `../gwz-dev-tr2-15-merge`, because main's concurrency fix rewrote the same open path.
    - **Dead code.** With the module's `#![allow(dead_code, unused_imports)]` removed, the candidate library showed 59 warnings, all now fixed:
      - `ssh_endpoint.rs`, `ssh_local::connect` and the blocking `open*` family;
      - the `open_endpoint_*` wrappers, the unwired interaction pause, refusal receipts and unread fields;
      - always-set flags and options, now gone;
      - test-only items, moved under `cfg(test)`.
    - **HTTPS.** The orphaned-helper registry (`ORPHANS`), whose reaper had no caller, is removed. An owner dropped with retained helpers now releases their slots, where before they stayed held for the process's life; a test pins this. The process-globals allowlist goes from 23 to 19 entries.
    - **The `tests/transport_ssh` crate is folded into gwz-core** (`src/git/endpoint/ssh_tests/`) and removed. It compiled 23 production files a second time, which forced the hidden allows, and needed its own libgit2 build and lock. Its suites now run in both candidate CI legs, and on Linux CI for the first time: they need `ssh-agent`, `ssh-add`, `ps -axo` and `kill -STOP`.
    - **Size.** `ssh_worker.rs` is 908 lines, down from 1,310, so only `placement_endpoint.rs` (1,086) is still past the review mark.
    - **For the operator:** about 100 HTTPS test call sites still use worker wrappers that run a mode production never uses (`allow_transition = false`), now under `cfg(test)`. Moving them onto `prepare_budget_for_transition` would be a step of its own.
  - **TR2.17 merged** (gwz-core `a6cc1737`, merging `73a96011` and `75998707`; root `324278b`).
    - **Cancel.** The SSH worker stops on a Cancel without answering, so the driver kept the stream's route until its own 5 s deadline. The endpoint now answers a Cancel on an attached SSH stream with `Failed(Cancelled)` and cancels the worker's exchange. HTTPS already answered. A cancel now returns in about 14 ms, down from 5.003 s. A peer that never answers stays bounded at about 5.004 s, with cleanup unconfirmed. `cancellable_tests` asserts a return within 500 ms.
    - **Permits.** A supervised job's permit lives in the job's shared state and is freed when its result is taken; the reaper frees one nobody took. Before, it was freed one sweep late.
    - Suite in the lane: 2,552 passed, 0 failed. `placement_endpoint.rs` is 1,115 lines.
    - The TR2.15 merge lane also merges main again, to resolve TR2.17's overlap in `placement_endpoint.rs` and `agent_job.rs` in the same pass.
  - **TR3.4 merged** (gwz-py `94ebeb6`, fast-forwarded; gwz-core `4b7a5051`, merging `06b14ded`, `RELEASE.md` only; root `c1b3377`).
    - From 1.1.0, gwz-py's `release` branch pins `gwz-core = "=X.Y.Z"` from crates.io, at gwz-py's own version. `release.py` writes the pin, and its new `verify_release_pins` refuses any other gwz-core pin, any `git` or `path` dependency, and a lock that takes a package from anywhere but crates.io. `publish.yml` runs the same check, and builds `--locked`.
    - The provenance test requires full provenance equality for a crates.io pair. The D7 rule for a git–crates.io pair stays, though no release produces one now.
    - `RELEASE.md` gains a native-pin table.
    - `release.py` also no longer aborts on the `Cargo.toml` conflict main's `cfg-if` line causes against `release`.
    - Tests: 32 targeted passed. In the lane's wider run, 2 failures came from a stale prebuilt extension and 34 errors need `GWZ_RUST_BIN`.
  - **From TR3.4, for 1.1.0's release:**
    - gwz-core 1.1.0 and every crate it needs (`gwz-git2`, `gwz-libgit2-sys`, `gwz-transport`, the internal crates) must be on crates.io before gwz-py's `release.py v1.1.0`, which fails with the order rather than waiting (a wait is about 40 lines);
    - gwz-cli's release branch must take `gwz-git2` from crates.io before `publish.yml`'s git2-rs checkout can go;
    - whether to retire D7's mixed-pair rule;
    - `GwzCratesIoPlan.md`'s D7 and O1 need a status line.
  - **1.1.0 S6.2 merged** (gwz-core `89b828cd`, merging `ac654d49`; gwz-cli `0218cc7`; gwz-py `60596c8`, merging `eecb485`; root `fb242ca`).
    - **The shared predicate.** `gwz-core/src/transport_scope.rs` lists the 9 transport-scope operations and is called by gwz-cli's `transport_meta` and gwz-py's native entry. A source test pins it to the handlers that call `with_transport`, and gwz-py's network set loses `attach_repo_member`.
    - **`ClientHost`** (`gwz-py/native/src/client_host.rs`) is one `#[pyclass]` per `Client`, with no static:
      - a limit of 8 across `call` and `submit`;
      - a snapshot taken, and the operation registered, while the GIL is held;
      - cancellation through tokens, and a bounded record of the last 64 completed cancels;
      - a running cleanup aggregate;
      - `close()`, which cancels and joins within `session_host::CLEANUP_BOUND`;
      - a per-host atexit callback holding a weakref.

      The bridge's per-loop lock, its `TransportCleanup(0, False)` stub, its `_needs_transport` set and the unused module-level native `call` and `submit` are gone.
    - **The off-switch seam** (`route/transport.rs::transport_off`) answers "off" until TR1.5 and TR2.5.
    - `~/` identities resolve against the snapshot's `HOME`.
    - **gwz-core:** a model `Cancelled` now maps to the protocol's `cancelled` code instead of `io_error`, and the cleanup bound is exported.
    - Suites in the lane: gwz-py 946 passed on the candidate build and 936 on the ordinary. In the candidate gwz-core run, the two failures were the concurrency bug main had already fixed, and an HTTPS throughput test that missed its bound under heavy load and passes alone.
    - gwz-py's inventory has 3 sites; its SHA-256 is `e943e10d68ca7e31772559698209d41e3174913571a090c793e7a7d306ed2472`.
  - **From S6.2, for the operator and TR2.6:**
    - `ClientHost` exists in every build, so gwz-py's ordinary build changes now: no per-loop serialization, a limit of 8, waiting operations cancellable, and `close` joining. That is an ordinary-path change for TR2.6's review and S7.5's Surface.
    - The `Cancelled` → `cancelled` code is visible to clients.
    - The production diff is about 1,270 lines in gwz-py.
    - Mid-exchange cancel and `close` took 5.00 s in the lane, which predates TR2.17. They should now follow TR2.17's 14 ms, and the combined verification checks that.
    - A handler that fails after the exit bound still builds its Python error during interpreter shutdown. The abi3 build has no shutdown guard, so on CPython before 3.14 that thread ends abruptly; the fix is making the extension's errors lazy.
    - A reused request ID reuses its old operation record (pre-existing).
    - gwz-py has no committed recipe for building its candidate extension; the lane used a scratch script.
  - **TR2.15 merged** (gwz-core `9d4dd92f`, merging lane `661c16f8`; root `f272385`).
    - The merge lane merged TR2.15 onto main, then main again for TR2.17 and TR3.4. The concurrency fix's design won where the two met: opens hold no job permit, and `start_endpoint_open` returns a `PendingOpen`.
    - Follow-ons: the test harness and `budget_wait_tests` moved onto the bridged open; the Cancel case in `SshPump::deliver` was removed, shown dead by a probe that never fired; and `native_service`'s impossible `Result` was removed.
    - Gates on the final tree: full candidate suite passed (main run 2,453, 0 failed); the 100-clone test 5 of 5; both-switches build with 0 warnings; process globals at 19 entries; the inventory at 18 sites.
    - **gwz defects found while merging** (GwzLaneIssues L7–L9):
      - a merge that exposes an ignored directory, or leaves a path deleted on both sides, goes to `recovery-required` with continue and abort blocked;
      - `--continue` refuses changes outside the conflict paths;
      - `gwz add` needs `--target` during an open merge.

      The main merge hit L7 too. Removing the exposed `tests/transport_ssh/target/` cache and parking the drafts let `--continue` finish.
  - **Merged main verified as a whole** (root `a2129d8`, gwz-core `9d4dd92f`, gwz-cli `0218cc7`, gwz-py `60596c8`), with no failure from the combination:

    | Suite | Passed | Failed | Ignored or skipped |
    |---|---|---|---|
    | gwz-core candidate | 2,673 | 0 | 7 |
    | gwz-core ordinary | 2,323 | 0 | 1 |
    | gwz-cli | 342 | 0 | 1 |
    | gwz-py ordinary | 961 | 0 | 10 |
    | gwz-py candidate, all 10 transport rows | 971 | 0 | 0 |

    Through gwz-py: mid-exchange cancel 0.02 s and `close()` 0.01 s (5.00 s before TR2.17); exit with a running operation 0.69 s; a stalled setup fails at 9.04 s on the stall clock; 8 overlapping operations open 8 connections.
  - **gwz-py follow-ups merged** (gwz-py `766de53`, fast-forward; root `888059e`):
    - **Lazy errors (`a671054`).** `error::model` holds Rust values and builds the Python error only when raised or read. Before, a failure after the exit bound either panicked in PyO3 ("interpreter not initialized") or had CPython end the thread with nothing recorded.
    - **Request IDs (`6bb5729`).** Per the contract's §4.3, an ID matching a live operation is refused with `InvalidRequest` before any effect, and one matching an ended operation gets a fresh record. Merge duplicates now fail the same way.
    - **Candidate recipe (`766de53`).** `scripts/build_candidate_extension.py` builds the candidate extension, and `run_tests.py --candidate DIR` runs the suite against it: 986 passed.
  - **From the verify lane:**
    - **One interpreter touch remains:** `dispatch::record` formats a failed submit's error through the interpreter. Keeping errors as Rust values through dispatch would change about 130 signatures.
    - **Request IDs:** `STORE` is process-wide, so the rule spans Clients where the contract scopes it per session. Merge's duplicate behaviour changed, which is for S7.5's Surface review.
    - **CI gaps:** no CI job builds gwz-py's candidate extension, so its 10 transport rows never run in CI, and gwz-py's Rust unit tests never run in CI either.
    - **Stale text:** S6.2's cancel docstring and 7 s limits in `test_client_host_transport.py`, and the Python design's §2.6 rationale for the GIL.
    - The both-switches leg was not run on the merged tree.
  - **Pushed on the operator's go** ("Push, and all as recommended"): root `a9479d8`, gwz-core `9d4dd92f`, gwz-cli `0218cc7`, gwz-py `766de53`, gwz-transport `6910ba6`. That includes gwz-transport's `repository` line, committed first (decision 10). Each `main` was verified equal to its `origin/main`.
    - CI runs started: gwz-core 36946316178 (Transport candidate), 36946316190 (Checked-artifact boundary), 36946316180 (Retained merge readers), 36946316181 (Linux identity probe); gwz-py 36946314367; gwz-transport 36946310967; gwz-dev 36946323296.
    - gwz-cli 36946312818 passed at once, including its new switch-inventory job against gwz-core main's checker.
  - **The push's CI, read 2026-10-02.** Five of the eight runs passed: gwz-core's retained merge readers (36946316180) and Linux identity probe (36946316181), gwz-transport's contracts (36946310967), gwz-dev's workspace recovery (36946323296) and gwz-cli (36946312818). Three failed:
    - **gwz-core's checked-artifact boundary (36946316190).**
      - The per-commit lane gate was RED at gwz-core `ac654d49` (S6.2), `89b828cd` and `9d4dd92f`. S6.2 included `transport_scope_tests.rs` from `transport_scope.rs` by `#[path]` without approving the edge in `check_checked_artifact_boundaries.py`.
      - Neither `run_tests.py` nor the lane merges run that checker, so nothing local caught it.
      - **Deviation:** the three pushed commits stay RED in the gate's history, as `95d292f` and `b923109` do.
      - Fixed at gwz-core `a3bef7cc`, and the gate is green over `9d4dd92f..a3bef7cc`. The three gwz-core lanes were told to make the identical change as their first commit, before any commit of their own; none had committed. So no new red commit reaches main.
      - From now on, the lane gate runs locally over a push's range before the push.
    - **gwz-core's transport candidate (36946316178).**
      - In both legs, two tests that TR2.15 folded into the candidate tests, now running on Linux for the first time, expect EOF from a closed peer and get `ConnectionReset`. Linux resets the peer of a socket closed with unread data, for TCP and `AF_UNIX` alike. The two tests are `agent_auth::native_network_wait_retries_with_stable_state_and_is_cancellable` and `agent_client::native::cancellation_interrupts_every_partial_reply_and_closes_agent_socket`.
      - In the both-switches leg only, two more failed:
        - `selected_pool::stalled_admission_does_not_stop_an_existing_stream`: its first open overran a 1 s aggregate deadline under load;
        - `path_characterization::h_member_direct_attributes_match_native_and_current`: its member's `.git` changed during the test. The suspected cause is recent git's detached auto-maintenance lock.
      - The three SSH tests went to `tr2-18`, and the characterization test to `tr2-19`.
    - **gwz-py's CI (36946314367), all 12 jobs.** It has been red on every push since 2026-09-29, so this push did not cause it.
      - `run_tests.py` builds the sibling gwz-cli `--locked`. `gwz-cli/Cargo.lock` is refreshed only at release, so it lacks gwz-core main's new crates (`gwz-git2`, `gwz-ids`, `gwz-session-*`). Locally the root workspace's lock hides this.
      - It went to `ci-py`, whose new jobs need the same fix.

    The fixes can be confirmed on Linux only by the next push, which waits for the operator's go.
  - **Operator decisions, 2026-10-02, "all as recommended":**
    1. **Cold start:** the first wave of setups starts in parallel up to the per-host limit, as 1.0.17's does, instead of the retry plan's one Cold setup per key. Setups that a server's `MaxStartups` drops are left to TR2.1's retry.
    2. TR2.14 was done by the concurrency fix.
    3. Split `placement_endpoint.rs`, movement only.
    4. Start the design steps: TR1.5 (off switch) and TR1.6 (credential helpers) now. TR1.8 waits for dabeest's 1.0.17 evidence.
    5. A password in an SSH URL is used as 1.0.17 uses it.
    6. Control characters in an SSH path stay refused, a deliberate difference from 1.0.17.
    7. `known_hosts` host names match case-insensitively.
    8. A `Failed` or `Rejected` result also lists member errors in `errors`.
    9. gwz-core's dead `OperationRuntime`, `ResponseBuilder::result` and `ExecutionReport` are removed: a public API removal for the release notes.
    10. Done (above).
    11. The 15 merged lanes were disposed, after each one's heads were proven in main and its one "unique" file was proven identical to main's committed gwz-transport `Cargo.toml`. That freed 21 GB.
    12. The HTTPS tests that run a mode production never uses (`allow_transition = false`) move onto `prepare_budget_for_transition`, after the split.
    13. CI jobs for gwz-py's candidate build (its transport rows) and its Rust unit tests.
    14. The remaining interpreter-exit edge in gwz-py (`dispatch::record`) is a known 1.1.0 limitation, which 1.2.0's session host replaces.
  - **Lanes now:**
    - `split-pe` and `ci-py` are merged (below), and their lanes are kept until the operator says to dispose of them;
    - `tr2-18`: decisions 5 and 7, an erratum for the SSH worker design's URL-decoding line, and the three Linux SSH test fixes;
    - The five merged lanes (`split-pe`, `ci-py`, `tr2-19`, `tr2-20` and `tr2-18`) were disposed on the operator's go, after each lane's heads were proven in main;
    - `tr2-1`: TR2.1, the retry plan's Phase 3 with OD18;
    - TR1.5 is accepted (above);
    - TR1.6 is accepted (above). Its OQ1–OQ7 are with the operator;

    Drafters (scratch, no lane): TR1.5 and TR1.6. After the split merges, TR2.1 (retry, with decision 1) and the HTTPS test migration (12). Amendment 2 revision 6 records decisions 1, 3, 6, 8, 9, 13 and 14, and steps TR2.13–TR2.19.
  - **TR1.6 accepted: HTTPS credential helpers on the transport.** The [design](../gwz-core/dev-docs/GwzTransportCredentialHelpersDesign.md) is accepted at SHA-256 `760f7ad4…` (revision 3), and filed as revision 4 (`9aef40ff…`). The [verdict](../gwz-core/dev-docs/GwzTransportCredentialHelpersDesign-Verdict.md) has the details.
    - **Its review.** Three rounds, with two remediation rounds, the cap.
      - Before review, the lane owner sent the draft back once under the parity rule. That made prompting helpers (OQ3) and a redirect after the challenge (OQ4) operator questions.
      - **Round 1:** Consistency GO; Safety NO-GO; Surface NO-GO with four P2s about messages. Safety's P2: OQ1 (b) as worded let a crafted URL username make git look up another host's credential and send it to the attacker's host. The fix keeps URL text only inside the encoded `url=` line.
      - **Round 2:** Consistency's P2 was codes that departed from 1.0.17. Remediation plan 2's F1 makes every outcome's codes and clone behaviour 1.0.17's.
      - **Round 3:** GO on all three axes.
    - **The design.**
      - The transport runs `git credential fill`, with prompts off and a filtered environment, in `/`, in its own process group, under a bound.
      - It asks only on a discovery 401, for the URL that challenged.
      - It asks once per route, holds the credential with the route, and pools the connection under the credential's scope.
      - It never sends a credential after a rejection, and never runs `approve` or `reject`.
      - Messages M1–M11 each carry a next action, and their codes and clone behaviour are 1.0.17's.
    - **Open for the operator, OQ1–OQ7:**
      - **OQ1:** HTTPS URLs that carry a username;
      - **OQ2:** Ctrl-C and the helper's process group;
      - **OQ3:** helpers that prompt;
      - **OQ4:** a redirect after the challenge;
      - **OQ5:** the `credential_helper_timeout` error code, on which Surface's GO depends;
      - **OQ6:** where the slot wait is charged;
      - **OQ7:** a failure detail field on the wire, which could also carry TR2.1's attempt number.
    - **The operator's answers, 2026-10-02 ("All as recommended").** OQ1 (b), OQ2 (a), OQ3 (b), OQ4 (a), OQ5 (a), OQ6 (a), OQ7 (1).
      - The `Failure` detail field also carries TR2.1's attempt number, at the operator's addition, so `attempt N of M` comes from the endpoint. That answers TR2.1's question about an attempt field on the wire.
      - The field is a gwz-transport wire change. TR2.22 makes it, with the per-step dual review a wire change takes, and S7.1 (1.1.0) names it.
    - **Next:** TR2.2, then TR2.22. Amendment 2's next revision carries the texts §9 and C3 amend, with status lines on those documents, and C2, C7 and TR2.23.
  - **More operator decisions, 2026-10-02:**
    - **The `--ssh-timeout` help: fix the text** (TR2.1's question). The help says the stall clock is SSH's, and that HTTPS setup has the 30-second aggregate budget. TR2.1 makes the change, and amendment 2's next revision records it as an erratum to the retry plan's §8.
    - **Lanes: dispose of tr2-1 and tr2-5a once each merge is verified.**
    - **Split all five large files,** movement only, in one lane after TR2.1 merges: `checked_artifact/entry.rs` (1,080 lines), `gitbackend/fake_repository.rs` (1,079), `gitbackend/contract.rs` (1,067), `transport_host/session/driver.rs` (813 in TR2.1's lane) and `https_worker_tests.rs` (959).
    - **Keep TR2.19's two items.** `gwz status` stays on the shared builder. The session plan's next amendment fixes the stale text: CS2.5, D10 and the session design's §5.4.
    - **Connection statistics in machine output** (the operator's request: "consider adding some connections stats on the jsonl responses (maybe with --verbose or --json_verbose) so that we can diagnose connection issues more easily"). A design is being drafted. The starting recommendation is the existing `--verbose`, combined with `--json` or `--jsonl`, and no new flag. It gets a Surface review.
  - **TR1.5 accepted: the transport setting (the off switch).** The [design](../gwz-core/dev-docs/GwzTransportOffSwitchDesign.md) is accepted at SHA-256 `145af486…` (revision 2), and filed as revision 3 (`208be7f1…`). The [verdict](../gwz-core/dev-docs/GwzTransportOffSwitchDesign-Verdict.md) has the details.
    - **Its review.** Three rounds of Consistency, Safety and Surface, with two remediation rounds, the cap. Round 1 was three NO-GOs with 8 P2s. Round 2 was GO, NO-GO and GO; Safety's one P2 was unquoted workspace paths in the suggested removal commands, a copy-paste injection. Round 3 was GO on all three axes. Revision 3 applies round 3's P3s, and the reviewers confirmed the forms that differ from their text.
    - **The design.**
      - `--transport <gwz|native>`, `GWZ_TRANSPORT` and `gwz.transport`, read from the user's global git configuration only, with no `includeIf` and no `GIT_CONFIG_GLOBAL`.
      - A workspace or repository value is ignored. In gwz-py the note about it is a `logging` record, so no warnings filter can turn it into a refusal.
      - An unreadable file is skipped, as git and libgit2 skip it.
      - The scan is bounded to the operation's targets, to regular files and to 1 MiB.
      - Printed commands are shell-quoted.
      - JSON carries `meta.transport_setting`.
    - **Applied recommendations, which the operator may reverse:**
      - **D2,** the names;
      - **D3,** 1.0.17's defaults when native is selected: 50, 8 and 3 s in gwz-cli;
      - **E3,** the defaults filled in by the drivers in 1.1.0. gwz-py's `Client(max_connections_per_host=None)`; that keyword postdates 1.0.17. TR2.11's callers stay at 100 and 32. In 1.2.0 the session host fills the defaults.
    - **The operator's answers, 2026-10-02 ("All as recommended").** D2, D3 and E3 are kept. OQ1: there is no way to silence the note. OQ2: a gwz-core module. OQ3: all four 1.2.0 items go to the server design's next revision. OQ4: a consumer-build row in S7.3 (1.1.0), which amendment 2's next revision adds. TR2.5 may start.
    - **Follow-ons:**
      - **TR2.5** runs in three steps after OQ1–OQ4, with its retry row after TR2.1.
      - **The session plan's re-check:** CS3.10, CS8.3, C1, CS8.28 and CS4.5, the 1.2.0 fill under CS3.10, CS4.7 and CS6.4, and the edge CS6.6 ── CS6.4, which also goes into the Phase 6 sketch.
      - **The status lines** of the retry plan and the Python design, and the plan's changelog, are updated with this entry.
  - **TR2.20 and TR2.18 merged** (gwz-core `72c6f49d`, a fast-forward to TR2.20, then `77c0ed9c`, merging TR2.18's `b212d044`; root `cc3e5c2`, then `a15c48c`).
    - **TR2.20.**
      - 44 call sites in 30 tests, plus the `OpeningSession` fixture route (37 tests in all), now run the production entry `prepare_budget_for_transition`.
      - The test-only `Client` wrappers and `allow_transition` are gone. Under the dead-code rule, so is a second test-only mode: `https_policy::classify`'s `allow_auth_transition`, which production has passed as `false` since the HTTPS endpoint landed.
      - The change is movement and deletion only. One weak test is fixed: its budget had made its four other domains untestable.
      - **Suites:** 2,320 ordinary and 2,670 candidate.
      - **Reported, not changed:** `budget_for_open` turns a zero interaction deadline into a zero helper allowance, which TR2.22's helper budget (TR1.6 E6) should settle. `https_worker_tests.rs` is now 959 lines.
    - **TR2.18.**
      - **Decision 5:** a password in an `ssh://` URL is used exactly as 1.0.17's libgit2 uses it, from `ssh_libssh2.c:830-863`.
        - The password is used only beside a user, and only if the server lists `password`. After it come the callback's key or agent, or a helper on a password-only server.
        - It stays inside the process, is never logged, is zeroized, and has a pool identity of its own.
        - A cross-process placement refuses password URLs.
      - **Decision 7:** `known_hosts` names match without regard to case. Hashed names match as written or lowercased.
      - **The tests.** A stdlib-only loopback SSH server (`tests/transport_backend/password_sshd.py`) proves the identical authentication sequence on both routes, across six URLs.
      - **The design erratum** is in `GwzRemoteTransportSshWorker.md`.
      - **The three Linux test fixes:** reset or EOF in the two close assertions, and normal budgets for `selected_pool`'s first open. They are unconfirmed until CI runs on Linux.
      - **Suites:** 2,471 per leg.
    - **Deviation, accepted by the operator ("Merge as is").** TR2.18's first commit, `341a669d`, a four-line erratum, predates the lane's approval commit, so the per-commit gate is RED there. Its root cause is S6.2's unapproved `#[path]` edge, the same as the pushed `ac654d49`, `89b828cd` and `9d4dd92f`. The next push's boundary job will report it.
    - **Merged main.** `cargo check --tests` is clean on both candidate legs and the ordinary build, and the gate is green over `ab48966f..77c0ed9c` apart from `341a669d`. The switch inventory lists 18 sites.
    - **From TR2.18, for the operator:**
      - **Parity gap.** 1.0.17 runs configured credential helpers for an SSH server that offers only `password`; the transport does not. Under the parity rule this is to be built: a new step, TR2.23, will reuse TR1.6's lookup after TR1.6's GO, in amendment 2's next revision.
      - **Not parity.** A server that accepts `none` authenticates on the transport, where 1.0.17 fails. This is for the notes.
      - **1.2.0.** A URL password cannot reach an endpoint in another process, because the protocol has no slot for it. That is for the server design's next revision.
      - **Observation rows** have no `password` method. That is S7.5's Surface item.
      - **Process slips.** The agent used one raw `git checkout`, reverting its own uncommitted probe, and deleted a `__pycache__`.
  - **TR2.19 merged** (gwz-core `ab48966f`, merging lane `ec95c0b9`; gwz-cli `236f753`, a fast-forward; gwz-py `2e0509fd`; root `46b69f8`).
    - **Decision 8.** A `Failed` or `Rejected` result lists each failed or rejected member's error in `errors`, as `Partial` does, in member order. `gwz status` now builds its envelope with the shared builder, so its own failed and rejected results copy errors too. MachineOutput.md, gwz-py's README, OperationModel.md, Reference.md and ErrorCatalog.md say so.
    - **Decision 9.** The removed public paths, for the 1.1.0 notes, all under `gwz_core::operation`, with their methods:
      - `OperationRuntime`;
      - `ResponseBuilder` (both `result` and `accepted`);
      - `ExecutionReport`, `MemberExecution`, `MemberExecutionStatus` and `OperationError`;
      - `RuntimeEventSink` and `EventSubscription`;
      - `OperationPlan`, `MemberPlan` and `PlannedAction`;
      - the impls `From<MemberExecutionStatus> for MemberStatus` and `From<operation::PlannedAction> for PlannedAction`.

      `ResponseBuilder::accepted`'s only caller was `OperationRuntime`, so the plan types went with it. A search of the whole workspace found no other reference. The protocol types `gwz_core::PlannedAction` and `ActionKind` stay.
    - **The CI flake.** git 2.55, CI's version, leaves `objects/maintenance.lock` behind after a commit, from its detached auto-maintenance, and that lock broke the characterization test. Its child now sets `maintenance.auto=false` and `gc.auto=0`. A stand-in wrapper reproduced the failure 20 times in 20, and with the fix it passed 20 times in 20. CI is the real confirmation.
    - **Suites:**
      - gwz-core gate: 2,320 passed;
      - candidate: 2,669 passed;
      - gwz-cli: 252 unit and 92 integration tests passed;
      - gwz-py: 980 ordinary and 990 candidate passed.
    - **Merged main.** The per-commit gate is green over `ff5f35eb..ab48966f`, and `cargo check --tests` is clean for gwz-core and gwz-cli.
    - **For the operator:**
      - the status scope above;
      - session plan text this removal makes stale: CS2.5, D10's "deprecate, do not remove", and the session design's §5.4, which describes `OperationRuntime`. Those need an amendment;
      - a release-note item: `gwz-py --json` now prints the member error copies for a failed or rejected result.
  - **The split merged** (gwz-core `ff5f35eb`, merging lane `e459cb25`; root `4ca53a6`, then `37398f3`).
    - `placement_endpoint.rs`, `https_worker.rs` and `transport_host/session.rs` are split into module directories, movement only.
    - **The proof.** rust-split's own `split` could not handle the 878-line impl block, so the agent used its lossless `explode` and copied every moved item from those chunks. An item-level checker, which the agent mutation-tested, found each item exactly once and byte-identical: 43/43, 41/41 and 66/66. The other differences are wiring: `mod` and `use` lines, impl headers and visibility.
    - The lane's first commit is the boundary approval, byte-identical to `a3bef7cc`.
    - **Gates.** The candidate suite passed (2,679 passed, 0 failed), and the both-switches check was clean. The per-commit gate is green over `9d4dd92f..ff5f35eb`, and the switch inventory still lists 18 sites. The merged tree's `src` equals the lane's.
    - **Still over 1,000 lines:** `checked_artifact/entry.rs` (1,080), `gitbackend/fake_repository.rs` (1,079, a test fake) and `gitbackend/contract.rs` (1,067). `transport_host/session/driver.rs` is 773, over the 500-line ceiling.
  - **ci-py merged: TR2.21** (gwz-py `35e2fda`, a fast-forward; root `94e7b7a`).
    - **New jobs:**
      - `transport-candidate.yml` runs gwz-py's suite, with its 10 transport rows, and its candidate Rust unit tests on both candidate legs;
      - `package-smoke.yml` gains a `rust-tests` job.
    - **The lock fix.** `validate` and `candidate` resolve gwz-cli's lock in CI (`cargo metadata`) before building gwz-cli main, the cause of the red runs since 2026-09-29. `publish.yml` does not, on purpose.
    - **The manifest.** gwz-py's `Cargo.toml` drops pyo3's deprecated `extension-module` feature, so `cargo test` links libpython. maturin still enables it from `pyproject.toml`, and the release graph and the built extension are byte-identical.
    - **S6.2's bounds** are tightened: cancel and close from 7 s to 1 s, exit from 9 s to 5 s. The cancel docstring is corrected, and the Python design gains an erratum line.
    - **Results.** Locally, 996 passed on each leg, and 25 and 29 Rust unit tests passed. None of it is confirmed on GitHub until a push.
    - gwz-cli's standalone lock should be refreshed in gwz-cli itself; the CI step works around it.
  - **Amendment 2 revision 6, committed with this entry.**
    - §3.20 records TR2.13–TR2.22, the split, S6.2's follow-ups and the fourteen decisions.
    - It also records OD18, the cold start: in each operation a key's first wave of setups starts in parallel, up to the per-host limit. OD18 amends the retry plan's §5 Cold state, §4's sentence on what Closed stops, and two S3.1 sentences. It reverses the single Cold probe that closed the retry plan's Safety `[P2-4]`, for 1.0.17 parity.
    - [Skim review 7](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2-ReviewSkim-7.md) reported GO with eight P3s. They were applied, P3-7 in part, with its dispute upheld. The [re-check](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment-2-ReviewSkim-8.md) reported GO with two new P3s, also applied.
    - The amendment hashes `8a532c4a…`. TR2.1 now waits for the split, and TR8.1 (1.1.0) and TR8.4 wait for TR2.1.
  - **TR2.13 merged** (gwz-core `67c680a7`, merging lane `083ac49b`; root `d3d2c60`). The cap was real.
    - **Cause.** Every hop polled on a fixed timer and moved one message a pass: the endpoint session every 5 ms, the local link every 2 ms, the driver session every 5 ms, and the SSH worker's bridges every 1 ms.
    - **Change.** A pass now moves every ready message, within bounds, and the session sleeps only when nothing moved. Work arriving at the link, a bridge or a stream wakes it, and the local link waits on both sessions. Every existing bound stays.
    - **Measured, clone of a 32 MiB incompressible pack:**

      | | libgit2 | Transport before | Transport after |
      |---|---|---|---|
      | SSH | 361–457 ms | 15.7 s (2.0 MiB/s) | 462 ms |
      | HTTPS | 428–492 ms | 14.1 s | 455–523 ms |

      The new tests hold the transport within 1.5× of libgit2 plus 1 s, on 16 MiB packs.
    - **A defect the speed exposed, fixed in the same step.** 100 clones reached the mux's 64-stream limit, and opens and identity checks failed outright; `gwz fetch` defaults to 100 jobs. They now wait for a free stream within their allocation deadline.
    - **Suites in the lane:** candidate 2,522 passed, 0 failed; the `tests/transport_ssh` proof crate 308 passed. A full run of merged main is under way.
    - **From its agent:** it ran `git add -N` once to count lines, a raw git state change; the `gwz add` that followed superseded it. The shared HTTPS fixture can stall a large response on macOS, with its TLS layer holding back the last piece; libgit2's own clone hung over 10 minutes. Tests with large HTTPS responses close each connection until the fixture is fixed.
- **What can start now** (the amendment's §3.13):
  - TR2.9 to TR2.12, and 1.1.0 S6.1, then S6.2 and S6.3;
  - TR2.2, TR2.3, TR2.4, TR2.7 and TR2.8; from revision 6, the split, then TR2.1 and TR2.20; TR2.18, TR2.19 and TR2.21; TR2.2 and TR1.6's GO, then TR2.22;
  - TR1.5, then TR2.5; and TR1.6;
  - 1.1.0 S4.1, then TR1.8 and S4.2–S4.4, then TR4.8–TR4.10 once TR1.8 has GO; and TR4.6's ordinary-build job;
  - 1.1.0 S2.1–S2.3, TR3.3's code and TR3.4. TR3.2 follows TR3.3's registry steps;
  - 1.1.0 S3.1–S3.3;
  - session-plan steps, under rule (e).

  TR2.9 is first, because it covers the measured blocker.
- **Committed with this entry, under the operator's go of 2026-10-02:**
  - amendment 2's revision 6;
  - the retry plan's status line and changelog;
  - the plan's changelog;
  - the verdict;
  - skim reviews 7 and 8.

  gwz-core `a3bef7cc`, the boundary fix, was committed just before them.
- **Committed earlier, on the operator's go:** revision 5's edits to amendment 2, the plan, amendment 1, the agent design, the server design, the session plan, the reuse design and the verdict, and its three skim review files (Skim-4, Skim-5 and Skim-6).

## Transport release — the five dead transport-host items removed; the SSH refusal test's timeout, 2026-10-01

- **Operator decision (2026-10-01):** the five candidate transport-host items that no production path used are dead: "remove them, they're dead". The compiler's non-test build of the candidate named them:
  - `RequestContext::open_https`, a wrapper of `open_https_recording`. The HTTPS policy tests' 11 calls now go through `open_https_recording`, with the fresh receipt the wrapper supplied.
  - `CliEndpoint::with_https`, the only way to build a CLI endpoint with HTTPS. The two tests that embedded messages over that configuration, `rust_https_embedded_messages_round_trip_live_git_exchange` and its Python twin, went with it, along with their driver. The three SSH embedding tests remain.
  - `stream_id` and `policy` on `HttpsAttemptReceipt`.
  - `stream_id` and `policy` on `HttpsOpenFailure`. The retry check that read `policy` now reads the failure's recorded auth method (`Gh`), which production reads.
  - The transport host's `SshOpenFailure` re-export. The fault and driver tests take it from `session`, whose re-export is now test-only.
- **Cascade:**
  - The driver's open functions returned a stream id with every failure only to fill those fields; they now return the bare failure.
  - `https_auth.rs` drops a `mut` that `6941c1ae` left behind. My warning check missed it: its pattern matched two-space arrows, and lines of 100 or more get three.
- **The SSH refusal test's intermittent failure is diagnosed.** With its full error printed, it failed on macOS with `PeerFailed { code: Timeout, effect: Possible }`: under the full suite's load the clone stalled past the fixture's 3-second I/O budget. The fixture now uses production's `DEFAULT_SERVER_TIMEOUT_MS` (9 s), and the test passed in the next full run.
- **A new intermittent failure, unrelated to these changes:** `workspace_ops::tests::g00::private_members::private_member_clone_access_refusals_are_quiet_and_preserve_public_clones` got `GitCommandFailed: "unexpectedly large parse"`. It came from libgit2's own HTTP transport, against the test's raw TCP refusing server. It passed in five of five runs alone. Recorded, not investigated.
- **Verified:**
  - the candidate library builds with no warnings; before, it warned about the five items and the `mut`;
  - the candidate test build warns only about the multi-included `https_fixture.rs` items, now seven with `input`, which only the removed embedding driver used in that copy;
  - the full candidate library suite passes 2,300 tests, and its one failure is the new intermittent one above;
  - clippy under the boundary job's lint configuration, `cargo fmt --check`, and the conditional-compilation and process-global checks pass.
- **Committed** on the operator's go ("yes, raise the timeout too, then commit and gwz push"): gwz-core `32b45b60` (the removal) and `0ddc513c` (the timeout), then the root lock captured and the root commit that carries this entry. Published with `gwz push`, limited to gwz-core and the root.

## Transport release — the candidate job's second run: a credential helper that skips its input, 2026-10-01

- **Second run** of the transport candidate job: run 36725000862, on gwz-core `1b561045`.
  - The "Text file busy" fix held: `abort_after_helper_start_retains_permit_until_owner_reap` and the new Linux test pass.
  - The Checked-artifact boundary job passed, its first green run since 2026-09-18, and so did the root's workspace-recovery run.
- **The failure:** `git::endpoint::https_auth::tests::lookup_uses_bounded_direct_helper_and_returns_secret` got `Err(Io)`.
  - **Cause:** the test's helper prints a credential and exits without reading its input. `lookup` writes the request while it reads the output, and when the helper has already exited, the write fails with a broken pipe. `lookup` returned `Io`, though the helper had answered and exited successfully. Git ignores SIGPIPE while it writes to a credential helper, so such a helper works with git.
  - **Fix:** product code in the candidate transport, on the operator's go. The new `write_request` treats a broken pipe as the helper not reading its input, and the helper's exit status and output decide the lookup. Any other write error is still `Io`.
  - **Tests:** a closed input does not fail the write, which failed before the fix, and any other write error is still `Io`. Both use an in-memory pipe and do not depend on timing. Locally, the candidate's endpoint and transport-host tests pass (193).
- **The archive proof** now runs whenever the suites ran, pass or fail. It was skipped in both runs, so it has not yet been proved on Linux.
- **Committed** on the operator's go ("yes, do both"): gwz-core `6941c1ae`, then the root lock captured and the root commit that carries this entry. Published with `gwz push`, limited to gwz-core and the root.

## Transport release — the candidate job's first run and two CI fixes, 2026-09-30

- **First run** of the transport candidate job: run 36716830133, on gwz-core `96a92b4c`, on ubuntu-24.04.
  - Every step before the suites passed on Linux: the pins, the taut tag checkout, the generators (9 and 8 tests), the candidate Python tests (26), and the source checks, including gwz-py's conditional-compilation boundaries (SKIPPED GATE for gwz-cli).
  - The suites: 2,318 lib tests passed and 1 failed. The four embedding tests pass, so PyO3 embeds setup-python's Python. The SSH refusal test that failed once on macOS passed.
  - The archive proof was skipped after the failure.
- **The failure:** `git::endpoint::https_auth::tests::abort_after_helper_start_retains_permit_until_owner_reap` stopped with "Text file busy" (ETXTBSY) in `src/git/endpoint/helper_script.rs`.
  - **Cause:** the helper's warm-up runs a script it has just written. A child that another test thread forks during the write keeps the write descriptor until it execs, and Linux refuses to execute a file that any process holds open for writing. macOS never refuses, so no dry run saw it.
  - **Fix:** the warm-up retries on `ExecutableFileBusy`, every 5 ms for up to 10 s. Once one exec succeeds, nothing can hold the script open for writing again, so the code under test never meets the error.
  - **Test:** `the_warm_up_waits_out_a_process_that_holds_the_script_open_for_writing` holds the script open for 100 ms. It is Linux-only, so its failure without the fix shows only in CI. Locally, the candidate's endpoint and transport-host tests pass (191).
- **The Checked-artifact boundary job** has failed on every push since `96acd92b`; its last green run was on 2026-09-18.
  - Clippy's filesystem lint refuses `std::fs::read_to_string` in `tests/native_libgit2.rs`. The test now reads gwz-core's manifest with `include_str!`.
  - Locally, clippy passes, and so do the job's last two steps, selection ownership and the M5b tripwires. No run on GitHub has reached them since that commit.
- **Escaped defects:** both came with `22147b68`, the cross-lane cleanup.
  - CI could not build gwz-core until `96acd92b` added the git2-rs checkout, and the boundary job's red status then went unnoticed across three pushes.
  - From now on, every run a push starts is read, not only the job being watched.
- **Committed** on the operator's go ("commit and push"): gwz-core `1b561045`, then the root lock captured and the root commit that carries this entry. Pushed in that order; the push reruns both jobs.

## Transport release — the transport candidate CI job, 2026-09-30

- **What:** gwz-core gains `.github/workflows/transport-candidate.yml`, on the operator's word ("start the candidate CI job"). It runs on every push to `main`, on every pull request and on dispatch.
  - It supersedes the 2026-09-30 decision to land the job with the checked-artifact gate's structural checks. That decision rested on both needing the fork at `../git2-rs`, and CI has checked the fork out since `96acd92b`.
  - The structural checks stay their own step.
- **The job** runs on ubuntu-24.04. It checks out gwz-core, gwz-transport, gwz-py, taut and git2-rs side by side, as a workspace has them, and runs:
  - the candidate and consumer generators' `--check` and their pytest suites, under the taut-proto release the candidate generator pins (0.10.0), installed in site-packages. The workflow names no version of its own;
  - three candidate Python test modules that no CI ran before: `tests/transport_backend/test_prepare.py`, `tests/transport_consumer/candidate/test_candidate.py` and `tests/transport_consumer/test_package_proof.py`;
  - `prepare.py` into the runner's temp directory, then the copy's lock resolved;
  - `scripts/run_tests.py` over the candidate manifest with `--cfg gwz_transport_candidate`. That covers the source checks, gwz-transport's process globals at the pin, gwz-core's and gwz-py's conditional-compilation boundaries (SKIPPED GATE for gwz-cli, which is not checked out), and the four test phases;
  - the consumer's archive proof, over the package of the pinned gwz-transport.
- **What the siblings are for, and their pins.** Each pinned commit must already be on its repository's `main`, and the job checks that.
  - **gwz-transport**, pinned by the new `.github/gwz-transport.commit` (`a24e70a`). The pin moves with the candidate generators' `owner-schema-sha256`: a gwz-core commit that moves those pins moves this one too. The job's `--check` fails if they disagree. It is not the gwz-transport allowlist's `reconciled_commit` (still `46e65a9`), the commit that list was last reconciled against, which the boundary job checks.
  - **gwz-py**, pinned by the new `.github/gwz-py.commit` (`950064d`). The candidate's embedding tests run gwz-py's Python codec from its source, and nothing is built from it. The pin moves when gwz-core's candidate tests need a newer gwz-py.
  - **taut**, at the tag of the taut-proto release the candidate generator pins (`v0.10.0`), with no pin file of its own. The candidate's tests read taut's source from the workspace checkout: `tests/protocol.rs` does, and so does the embedding tests' bridge, `tests/transport_backend/python_embedding.py`. The adoption brief (GwzTaut010Adoption.md §3) keeps that checkout at the release tag as the checkpoint ritual's third leg, and this is the same leg in CI. The generators take the installed release instead.
- **Why the first dry run failed:** four embedding tests, because the bridge requires `gwz-py/src` and `taut/src` beside gwz-core.
- **`run_tests.py --skip-cfg-sibling NAME`** is new.
  - `--skip-cfg-siblings` is only for a job with neither sibling checked out, and it prints "this run has no gwz-py checkout", which is false in a job that has one.
  - The new flag skips one sibling, so this job checks gwz-py's boundaries at its pinned commit. CS1.7's follow-up is unchanged: each sibling's own CI checking its own changes.
  - Two new tests in `test_check_cfg_boundaries.py`: the flag skips the named sibling alone, and it refuses a name that is not a sibling. Both failed before the change.
  - Four statements that no gwz-core job checks gwz-py out now say what is true: `check_cfg_boundaries.py`'s docstring, the allowlist's `rule`, and the platform and Windows matrix comments. The allowlist's shrink check passes against `HEAD`.
- **Verified locally:** `test_check_cfg_boundaries.py` (50 tests) and the other `run_tests.py` tests pass, `cargo fmt --check` passes, and the full cfg check passes over the workspace, gwz-cli included.
- **Verified by three dry runs** of the job's steps on macOS, in a runner-shaped layout: the siblings cloned from GitHub at their pins, the gwz-core working tree, and Python 3.12 from uv in place of setup-python.
  - The third run is the job as committed, except for three later changes checked on their own: the release pin the workflow now derives (the run installed 0.10.0 directly), the test's diagnostic messages (the candidate build compiles them, and both driver tests pass), and four text corrections.
  - Every step of the third run passes except one intermittent test:
    - the pins are on their repositories' `main`;
    - the generators pass (9 and 8 tests), and the candidate Python tests pass (26);
    - the source checks pass: process globals, conditional compilation over gwz-core and gwz-py, crate versions;
    - the suites: 2,523 passed, 1 failed, 1 ignored. The four embedding tests that failed in the first run pass;
    - the archive proof passes on `a24e70a`.
- **The one failure is intermittent:** `transport_candidate_tests::drivers::candidate_service_refusal_skips_only_private_members_and_forgets_observations` got `GitCommandFailed` where it expects `RemoteRejected`.
  - It passed in the other five full-suite runs (the first two dry runs and three reruns of the lib suite), and in 70 of 70 runs alone or eight at a time.
  - Its assertion reported only the error code, so the failure's cause is unknown. Either the refusal was lost, or the clone failed another way, such as by the fixture's 3-second I/O timeout under the suite's load (`transport_candidate_tests.rs` passes 3000 to `ssh_local::connect`).
  - The test's two assertions now print the whole error, so a recurrence in CI will show which.
- **Linux** behaviour is proved only by the first run on GitHub.
- **Open for the operator:**
  - **Tests that read the taut checkout.** Should they take the installed release, as the generators do? That covers `tests/protocol.rs` (six tests), the embedding bridge and `test_candidate.py`, plus `docs/generate_message_catalog.py`, which reads the vendored `taut/src`. One cost surfaced in the dry run: PyO3's embedded interpreter runs from the base install it links and never activates a virtual environment. A release-only bridge therefore needs taut-proto installed in that base Python, for local runs too.
  - **Python test modules no CI runs:** the five under `scripts/retained_readers/`, `scripts/checks/test_check_local_clone_docs.py`, `tests/transport_native/test_prove.py` and `tests/transport_native/distribution/test_fork.py`.
  - **`src/git/endpoint/https_fixture.rs`** is included by `#[path]` in four test modules, so the candidate build warns about six fixture items that only the other copies use.
- **Residuals:**
  - prepare.py pins the added crates' direct versions exactly, but their transitive versions resolve when the job runs;
  - gwz-cli's cfg check still runs only locally;
  - macOS runs stay with the dispatch-only platform matrix;
  - under the review granularity ruling, the phase's Consistency and Safety review covers this step.
- **Committed** on the operator's go ("commit and push"): gwz-core `96a92b4c`, then the root lock captured and the root commit that carries this entry. Pushed in that order; the push to gwz-core's `main` starts the job's first run.

## Transport release — the uniform release rule for gwz-py's protocol generator, 2026-09-30

- **What:** the open item the taut 0.10.0 adoption left: its Consistency round-2 P3-6, taken up on the operator's word ("give regen_protocol.py the uniform rule"). gwz-py's `scripts/regen_protocol.py` now applies the rule the other three generators follow. It still generates in child processes without `PYTHONPATH`. Each child:
  - accepts taut-proto only from this interpreter's site directories, at the pinned version, before it imports taut;
  - checks that every loaded taut module is that release's own file, before and after generating;
  - refuses anything else.

  The parent's own version check, redundant with the children's and blind to `PYTHONPATH` shadows, is gone. The generated protocol is unchanged: `--check` and the drift check both pass.
- **Tests:** `src/tests/test_regen_protocol.py` covers four cases:
  - a wrong release;
  - a shadowing `taut` package;
  - a shadowing package with its own release metadata;
  - a copy on `PYTHONPATH`, which generation ignores.

  The metadata test fails against a copy of the script without the site-directory check. The protocol tests (`test_protocol_drift.py`, `test_log_protocol.py`) pass: 9 in all.
- **Committed** on the operator's go ("commit and push"): gwz-py `950064d`, then the root lock captured and the root commit that carries this entry. Pushed in that order.

## Transport release — taut 0.10.0 adopted, accepted and committed, 2026-09-30

- **What:** taut 0.10.0 replaces taut-proto 0.9.1 and taut-shape 0.9.2 across gwz-core, gwz-py, gwz-transport and the root. It was released on 2026-09-30 as one train: taut-proto and taut-shape on PyPI, taut-shape on crates.io, and `v0.10.0` in all four taut repos. The step lands ahead of CS1.1, whose `cancellation` field is `optional=MISSING_OK`: taut-proto 0.9.1 lacks it, and gwz-py can depend only on a published taut-proto. Operator decisions: adopt it now, as one step; drop `gwz_core::decode`. Brief: [GwzTaut010Adoption.md](GwzTaut010Adoption.md).
- **Review:** the wire-format dual review.
  - **Round 1:** GO on both axes, with six P3s (Consistency four, Safety two), all taken into the step ([RemPlan](GwzTaut010Adoption-RemPlan.md)). The Safety axis showed that a copy of taut carrying its own `taut-proto` metadata passed the new release check. The three generators now accept the release only from the interpreter's site directories.
  - **Round 2:** GO on both axes. All six are closed, with the original counterexamples re-run. Consistency raised two text P3s, which the brief takes in at landing.
  - **Reports:** `GwzTaut010Adoption-ReviewConsistency.md`, `-ReviewSafety.md`, and their `-1` re-verdicts.
- **Release pins replace commit pins.** gwz-core's candidate and consumer generators and gwz-transport's generator took a taut checkout at a pinned commit. Now they take the released taut-proto 0.10.0, from the interpreter's site directories only. gwz-core's production generator and gwz-py's already took a PyPI release and are now pinned to 0.10.0. The previous entry's "every taut commit costs three re-pins" and its idea of pinning taut's `src/` tree lapse.
- **The wire is unchanged:**
  - the three corpora (155, 17 and 175 vectors) regenerate byte-identical;
  - both pre-log fingerprints hold over IR version 1 of taut's version 2 export, and both projection checks refuse a declared taut option.
- **Decoding changes (taut 0.10.0):**
  - each message has a depth bound of 32 (`TooDeep`). No gwz type is recursive, and the deepest static nesting is 8;
  - there is no length bound;
  - Python refuses an absent optional field unless it is `MISSING_OK`, as Rust already did. Both encoders always write every field;
  - `gwz_core::decode` and the runtime's panicking accessors are gone, `DuplicateMapKey` carries a `MapKey`, and `gwz_core::MAX_DEPTH` and `MAX_ENCODED_LEN` are new root items (`docs/RustApi.md`).
- **Fixed on the way:**
  - the local root build: `--locked` failed once another session moved taut-shape-rs to 0.10.0;
  - gwz-transport's contracts CI: red since `f8ebef7`, because its taut checkout lagged the pin, plus a rustfmt failure in `tests/pool.rs` that was already there;
  - three dead imports in candidate test code;
  - the consumer generator's inert `codec` guard and the pin keys nothing read.
- **The workspace:**
  - the taut checkout is at `v0.10.0` (`a7cab03`, one behind `origin/main`, whose extra commit only sets `fallback_version`);
  - the root lock also records another session's move of taut-shape (`5779c96`) and taut-shape-py (`f86f0e9`) to their tags, and of taut-shape-rs to `5026715`, one CI-only commit past its tag;
  - gwz-py's `.venv` carries taut-proto 0.10.0.
- **Plan text for the plan's next revision:** `GwzCoreSessionPlan.md` line 109 and CS1.1's regeneration inputs. They need taut-proto 0.10.0 installed in site-packages, not a taut checkout at `bcf98b64…` plus 0.9.1.
- **Open:**
  - gwz-py's `scripts/regen_protocol.py` checks only the installed version: no site-directory binding and no module-origin check. Giving it the uniform rule is a small follow-up (Consistency round-2 P3-6);
  - gwz-cli's standalone `Cargo.lock`, refreshed at release time and stale since the rename;
  - the consumer's archive proof (`package_proof.py`), which needs a committed gwz-transport and can run now;
  - the candidate CI job, the next planned step. Until it lands, nothing in CI runs the candidate and consumer generators' `--check`;
  - five candidate transport-host items that the non-test build never calls: `Request::open_https`, `with_https`, the fields `stream_id` and `policy`, and the `SshOpenFailure` re-export. Whether they are staged or dead is the operator's call;
  - rustdoc's two unresolved links (`src/artifact/store.rs:10`, `src/diff/render/options.rs:38`);
  - comparing `effective` against taut's defaults in `ir_version_1`, deferred to the step that adopts taut options.
- **Committed** on the operator's go ("commit and push when the reviews come back"):
  - the members: gwz-core `47ea25d6`, gwz-py `9af3cf9`, gwz-transport `a24e70a`;
  - the root lock, captured from them;
  - the root commit that carries this entry.

  The push follows in the order gwz-transport, gwz-core, gwz-py, root. gwz-py's native code calls `gwz_core::try_decode`, which older gwz-core does not export.

## Transport release — pushed; git2-rs in CI; taut 81ba894, 2026-09-30

- **Pushed on the operator's word:** every repo in the workspace, once the git2-rs fork's package rename was committed (git2-rs `d13951f` on `codex/per-remote-transport`, gwz-git `a9d7ee0`, root `867d8fa`). The rename is `git2` to `gwz-git2` and `libgit2-sys` to `gwz-libgit2-sys`; the Rust crate names stay `git2` and `libgit2_sys`. Both packages are `publish = false`, and crates.io holds only `0.0.0-bootstrap.1` placeholders, so gwz-core can't be published to crates.io until the fork is.
- **CI checks out git2-rs:** gwz-core depends on the fork by path (`../git2-rs`), and no workflow checked it out.
  - `gwz-core/.github/checkout-git2-rs.sh` clones git2-rs and its libgit2 submodule beside the gwz-core checkout it lives in, at the commit `.github/git2-rs.commit` pins (`d13951f`, equal to the root lock's). It refuses an existing git2-rs.
  - Every job that builds gwz-core from its own tree calls it: gwz-core's boundary, platform, Windows, identity-probe and release jobs, gwz-cli's platform-gate build, and gwz-py's package-smoke and publish builds.
  - gwz-cli's and gwz-py's release workflows build release branches, which take gwz-core from crates.io, so they don't need it. The manual compiler probes belong to the gate step.
  - Rule: a gwz-core commit that moves to newer git2-rs code moves the pin in the same commit, to a pushed git2-rs commit.
- **taut pulled to `81ba894`,** a docs-only commit whose `src/` is identical. Three generators pin taut's exact commit, so the pull broke them although their output is unchanged: gwz-core's candidate and consumer generators and gwz-transport's. All three are re-pinned and verified.
  - Every taut commit, even a docs-only one, costs three re-pins while the pins name commits; pinning taut's `src/` tree (`git rev-parse HEAD:src`) would end that.
  - The consumer generator's check depends on the working directory's toolchain, as the candidate's did before S-4: run it from the workspace root with Rust 1.96.0.

## Transport release — cross-lane cleanup, accepted and committed, 2026-09-30

- **What:** the operator's "fix other lanes": the failures other lanes left at HEAD, and the operator's decisions on what that turned up. Uncommitted, on root `1ccb5c1`, gwz-core `2514dc19`, gwz-cli `ebbea90` and gwz-py `4f9b2bb` (gwz-py is unchanged).
- **Round 1, the failing checks:**
  - the checked-artifact gate approves 22 test-only `#[path]` edges, each noted with the commit that added it (no-fallback lane `21961474`; remote-transport lane `073395b5` of 09-21 and nine commits of 09-22);
  - Bazel: gwz-cli's `cfg-if` is pinned (`=1.0.4`) in the root `MODULE.bazel` and declared as a dependency in `gwz-cli/BUILD.bazel`. `MODULE.bazel.lock` stays stale until git2-rs's package rename is committed (TR3.1);
  - `runtime_timeout_refuses_changes_after_backend_creation` takes the named default (`DEFAULT_SERVER_TIMEOUT_MS`, 9000 since `1ac248ca`), and the characterization is now `native_same_tree_receiver_ref_does_not_block_the_fetch`, since the fork's libgit2 1.9.7 (`26b30ca6`) fetches natively;
  - clippy with all features goes from 36 errors to none. 22 are `needless_update` on generated structs that the candidate build extends, allowed at the narrowest scope with that reason; the other three are fixed. rustfmt covers four files;
  - `generated_protocol_is_current`: `src/cbor.rs` is regenerated, adding three accessors from taut `bcf98b6`. The ordinary build doesn't use them; the candidate protocol calls `try_get_opt`. It had been red since 09-22 (root `9cc9747`), not since taut `9d46310` as the entry below says.
- **Round 2, the operator's decisions:**
  - **`MissingRemote` again.** `validate_remote_identity` and the two identity helpers look remotes up through core's `find_remote`, so push, pull, remote tag operations and `gwz auth identity` answer `MissingRemote` for a missing remote. It had regressed to `GitCommandFailed` on 2026-09-07 (`37dbbcf4`, `6593b3b4`), and gwz-cli's 1.0.17 docs had been rewritten to quote the regression; they quote `MissingRemote` again.
  - **The docs checks:** the three rows whose sentences gwz-cli's 1.0.17 docs reworded on purpose pin the current wording.
  - **The git CLI fetch fallback is removed** (`fetch_anonymous_with_git`). A `git` stub that failed the fallback's call saw 2,833 calls across the local-clone, git and merge suites, and none of them was the fallback's. The prerequisites the accepted docs set were not done first, and the operator chose to leave them open (gwz-core `GwzNoFallbackPlan.md` §4): tag type consistency, missing targets, and FETCH_HEAD, cancellation and partial-outcome characterization, plus the fork's type-consistency hardening. A native `object is not a committish` now fails the import. Production code still runs `git tag`, `git tag -d`, `git commit` and `git rev-list`.
  - **No production code in `tests/`** (standing rule). The process-globals checker refuses a production file under a crate's `tests/`, candidate cfg included. The candidate protocol moved out unchanged: `src/protocol/candidate_generated.rs`, with its schema projection, regenerator, pins and Python binding in `protocol/candidate/`. The guide example's `include!` is gone; the prepared candidate compiles the example as its own test target, against `gwz_core` as an external crate. `src/protocol/candidate_generated.rs` now ships in the gwz-core package (`include = ["/src/**"]`), inert without the cfg, as the rest of the candidate code under `src/` already does; gwz-core's `GwzCratesIoPlan.md` D4 records it.
  - **`prepare.py` builds the candidate again,** with absolute dependency paths and no stale git2 patch.
  - **CS1.1's pin move, pulled forward:** the candidate's core schema and retained reader move from gwz-core `54618449` to `2514dc19`, so the candidate gains `GwzErrorCode` 73 and 74 (`dfb7533b`). Its Python binding also catches up with the owner export that `0b7fdf19` pinned without regenerating it (`Envelope.message_seq`, gwz-transport `c4b632d`).
  - **gwz-cli joins the process-globals check:** 18 items: 17 `permanent` (six environment and argument reads and nine process spawns the CLI must make, one immutable cache, and a clap `Command::new("help")` that the lexical scan can't tell from a spawn) and 1 `debt` (`GWZ_URL_SCHEME`, owner proposed as CS6.1). A new CI job runs the checker from gwz-core's `main`, unpinned as gwz-py's job does, so a checker change on `main` changes both gates (accepted, Safety S-2).
  - gwz-cli's rustfmt failure, from another lane, is fixed.
- **The candidate's suites run again,** for the first time since TR3.1 broke `prepare.py`. Their failures all predated this cleanup, and the operator folded their fixes into it. Round 2 fixes each at its cause, with no serialized, retried or skipped test:
  - **The HTTPS helper slots:** the process-global `SLOTS` semaphore is gone, pulled forward from CS6.5. The helper budget now lives where the contract puts it, in the host context shared by its sessions (`GwzCoreSessionDesign.md:65`, `:350`). In the candidate, each command's driver creates one eight-slot budget and hands it to every endpoint it creates. Parallel tests no longer share one budget.
  - **Three product defects, fixed test-first:**
    - a stale client `Cancel` forwarded after a seal closed the whole session;
    - the SSH pump discarded an endpoint stream's own terminal (its I/O `Timeout`) when it disconnected, so the bridged exchange never completed and the client's read hung;
    - a client message the pump took after the mux had retired that stream's route closed the whole session, so every stream failed `CarrierLost`. The route is retired by the endpoint's `Closed` or `Failed`, or by the mux's own deadline. The pump now drops the stale message, and only that: the guard checks its own session and version, and the route the mux gave the stream belongs to the entry's request. The retiring terminal still ends the stream. A deterministic test reproduces it. Under the load that failed 6 of 6 runs, 6 of 6 now pass.
  - **§10.1 conformance.** The SSH pump no longer charges the client's think time to the stall clock (`GwzRemoteTransportDesign.md` §10.1). `git_turns.rs` follows the pkt-line turn of v0/v1 upload-pack and receive-pack, and anything it doesn't know stays `Network`. The design has a dated note. HTTPS already didn't charge think time, but it reports that wait as `Backpressure` where §10.1 says `Idle`; that is recorded, not changed.
  - **macOS XProtect.** The first exec of a newly written executable waits for XProtect's assessment, which pushed helper scripts past the tests' 2 s deadlines under load. The tests now warm each helper once (`helper_script.rs`); the warm-up changes no deadline or assertion.
  - **The rest of the candidate tests:**
    - two HTTPS budget tests have 390 ms of scheduling slack instead of 25 ms. A third, which gave a helper a 1 ms allocation, is split in two: `delayed_helper_is_charged_to_interaction_not_allocation` (a 250 ms allocation and a 5 s interaction for a 500 ms helper), and `a_stale_supervisor_tick_never_expires_a_fresh_one_millisecond_allowance`, which keeps the 1 ms case deterministically;
    - three driver and fault tests assert gwz-core's own `SshOpenFailure` (since `1ac248ca`);
    - `tests/protocol.rs` finds taut through `placement-candidate.json`, and its `MergeRequest` pin has a candidate branch. taut always writes a declared key, so the candidate's absent `transport_message` is `0a f6`;
    - the consumer crate follows gwz-transport's `setup_cause` (`14f0d09`) and its 30 s connect budget.
  - **The candidate's own corpus** (`protocol/candidate/corpus/`, 175 vectors from the candidate schema, pinned by its regenerator): corpus byte parity now holds under both cfgs, as CS1.1 asks.
  - **Results:** the endpoint and `transport_host` set passed 10 of 10 parallel runs after the pump fix (190 tests then, 191 with the race test). The full candidate suite passed twice before the `CarrierLost` race showed up, and that race is now fixed.
  - **Not run in CI:** nothing runs the candidate's suites, which is how they drifted.
- **Round 2's corrections to round 1's findings:**
  - **S-1:** gwz-core's `gwz-git2` dependency now asks for `vendored-libgit2`, and `tests/native_libgit2.rs` asserts that the linked libgit2 is the vendored one. The hazard was narrower than reported: with `unstable-sha256`, only an experimental-sha256 system libgit2 could have been picked up.
  - **S-3:** the process-globals checker fails closed. A production `include!` it can't read is an error that no entry waives, and `cfg_attr` paths are followed.
  - **S-4:** the candidate regenerator runs rustfmt under gwz-core's pinned toolchain. The rustfmt pin moved to 1.95.0's; the outputs are byte-identical.
  - **Text:** the Consistency P3s and the Safety residual.
- **Review:** one Consistency and Safety review of the whole cleanup.
  - Round 1 (manifest `43fcf974…`): [Consistency](GwzCrossLaneCleanup-ReviewConsistency.md) GO with five P3s; [Safety](GwzCrossLaneCleanup-ReviewSafety.md) NO-GO with one P2 and three P3s. S-1: nothing forced the fork's vendored libgit2, on which the fallback's removal relies, so a build linking a system libgit2 1.9.7 would fail the import.
  - [RemPlan](GwzCrossLaneCleanup-RemPlan.md): all nine accepted; the operator also folded the candidate suites' failures in.
  - Round 2 (manifest `b6a5834a…`), the last under the cap:
    - [Consistency-1](GwzCrossLaneCleanup-ReviewConsistency-1.md): NO-GO, with one P2 (the checker's own unit suite still expected 25 allowlist entries after `SLOTS` left) and four P3s;
    - [Safety-1](GwzCrossLaneCleanup-ReviewSafety-1.md): GO, with three P3s. The same count (S-5) and probe tally (S-6), and S-7: the retired-route drop also covered a request or version mismatch.
  - All round-2 findings are corrected:
    - the count is 24;
    - the drop also requires version 2;
    - the S-1 comments name `libgit2-experimental`;
    - the consumer README names both rustfmt pins;
    - this entry's tally, test list and plan-text lines are fixed.
  - **Decided (operator, 2026-09-30):** round 2 was the last, so its corrections go to the operator. The operator closed the review on them without a confirmation round, and the cleanup is committed on the operator's go.
- **The checked-artifact gate's lost protections** (found in round 2). gwz-core `107aca7a` (2026-09-08, "streamline validation and release tests") removed the enforcement of the gate's byte pins (`PROTECTED_COMPILER_ROOT_DIGESTS`, `PROTECTED_SOURCE_DIGESTS`, `PROTECTED_SOURCE_TREE_DIGESTS`) and of entry.rs's item, import and call inventories, and left the tables and the comments that relied on them.
  - This cleanup deletes the dead tables and `source_tree_digest`, and the comments now say what holds.
  - `scripts/manual_tests/boundary_probes.py` holds 75 probes. Against today's gate:
    - 41 are still rejected;
    - 17 defeats pass it, among them writers reached through a crate function or wrapper, new helper files in the formerly pinned trees, and entry.rs edits that add no visible item;
    - 2 expect messages or source the gate no longer has;
    - the compiler probes can't run, because since gwz-core `26b30ca6` the crate depends on the fork by path (`../git2-rs`), which the harness's copy lacks.
  - The gate's path-edge scan also reads only a plain `#[path]`, so it doesn't see a `cfg_attr` path. CI's clippy still catches a direct `std::fs` writer.
  - `scripts/manual_tests/boundary_probes.py` now copies `tests/`, which the gate follows through the approved test-only edges.
  - **Decided (operator, 2026-09-29):** structural checks close these defeats in their own reviewed step, straight after this cleanup. They should need no re-pinning on every edit. The probe file becomes its tests once its compiler probes can load the fork.
- **Plan text for the next revision:**
  - the session plan's §2.4 candidate bullets, CS1.1's file list and its C8 record (line 1495), and the contract's §13 sentence on the placement projection (`GwzCoreSessionDesign.md:534`), name the old `tests/` paths;
  - the plan's line 109 still names taut `bcf98b64…` and the owner schema `10179189…`, which `0b7fdf19` had already moved to `9d46310` and `2776ae51…`;
  - CS1.1 no longer moves the core pins, but still regenerates for §13 and wires the regenerator's check and the lexical test into `run_tests.py`;
  - `SLOTS` went early (CS6.5, pulled forward). The plan still describes it as live in five places: line 383, CS3.6 (lines 426-427), CS6.5's file list (line 636), §5.4's `SLOTS` row (line 1251) and line 1536. So does the contract's `SLOTS` row (`GwzCoreSessionDesign.md:350`);
  - CS6.1 is the proposed owner of `GWZ_URL_SCHEME`.
- **For the taut lane:** `taut/dev-docs/TautOptions.md:475` names the old path of `candidate.taut.py`.
- **Pushing:** gwz-core's commits reach GitHub before or with gwz-cli's, since gwz-cli's new job fails until gwz-core's `main` has the checker, and gwz-py's with or before gwz-core's (its CI runs gwz-core `main`'s checker, which now requires owners).
- **Decided (operator, 2026-09-30):** a CI job for the candidate build's suites comes with the gate's structural-checks step, since both need the probe harness to find the fork (`../git2-rs`).
- **Next:** the checked-artifact gate's structural checks and the candidate's CI job, in one reviewed step.

## Transport release — crate map steps 1 to 4 committed, 2026-09-29

- **Committed** on the operator's go as root `1ccb5c1`, gwz-core `2514dc19` and gwz-py `4f9b2bb`, on root `60fb142`, gwz-core `0b7fdf19` and gwz-py `a342b95`; not pushed. Another session committed taut `9d46310`, gwz-transport `f8ebef7`, gwz-core `0b7fdf19` and root `60fb142` the same night; nothing overlaps.
  - **Step 1, checkers:** the process-globals checker refuses a `permanent` entry for a counter, flag, lock, cell or thread-local unless `imposed_by` names the dependency, and every global-state `debt` entry names an `owner`. The eight counters, `CROSSING` and gwz-transport's `NEXT_POOL` became debt. It also sees `lazy_static!`. The boundary gate counts only gwz-core's own packages as first-party, so the git2 fork (`gwz-git2`) is third-party.
  - **Step 2, `gwz-ids`:** each context's `IdSource` (a random prefix from `getrandom`, plus a counter) replaces the four temp-name counters. Family-store creates its temporary exclusively and retries.
  - **Step 3, `gwz-session-contract` and `gwz-session-channel`:** CS1.2 and CS1.3 as crates that carry bytes only, with vectors from taut-shape-tool.
  - **Step 4, `gwz-session-host`:** core's gate, limits and supervisor moved into it; `CROSSING` is gone, replaced by capabilities (the [step 4 plan](GwzCoreSessionCrateMapStep4.md)); `ClientChannel` sends and receives through `pair(Limits)`. `RustApi.md`, `GWZDesign.md` and the contract (amended 2026-09-29, §3, §5.1, §5.2, §5.6, §9) carry the text.
  - The allowlists hold 26 gwz-core items (4 `permanent`, all imposed by libgit2 or a cleared-environment spawn), and 18 crates are versioned.
- **Review:** one Consistency and Safety review of the whole diff, under the granularity ruling; steps 3 and 4's channel wiring, which are wire format and carry secret-bearing frames, were covered in full. No Surface reviewer: the Rust API additions reach only first-party drivers (lane owner's call).
  - Both axes reported GO on the same object (manifest `e7f7bd90…`), with no P0 to P2: [Consistency](GwzCoreSessionCrateMapSteps-ReviewConsistency.md) five P3s, [Safety](GwzCoreSessionCrateMapSteps-ReviewSafety.md) three.
  - The eight were applied as one change: the byte-stream adapter wipes a body it does not deliver (best-effort, safe code); `ClientChannel::close()` drops the session context and its snapshot; the process-globals checker lists macro-declared statics and follows `include!`; CI's Tier A also runs doctests; `send(frame, lane)` takes the caller's lane until CS1.6; `GWZRequirements.md` takes the O9 sentence; the crate counts are count-free.
  - Both reviewers confirmed them on the corrected tree (manifest `631067bd…`): [Consistency-1](GwzCoreSessionCrateMapSteps-ReviewConsistency-1.md) and [Safety-1](GwzCoreSessionCrateMapSteps-ReviewSafety-1.md). Consistency-1's one new P3, three stale records, is applied as text, as it asked, without another round.
- **Carried obligations:**
  - CS1.6 classifies each frame on the host side and never trusts the lane it arrived on;
  - CS2.3, CS2.12 and CS3.7: callbacks never hold controls, and the reading thread runs callbacks holding no session lock;
  - gwz-py's commit reaches GitHub with or before gwz-core's, since gwz-py's CI runs gwz-core `main`'s checker, which now requires owners.
- **Failing at HEAD before this work, from other lanes** (the operator decides who fixes them):
  - the checked-artifact gate: 23 test-file `#[path]` edges and one `include!` (remote-transport lane 2026-09-21/22, no-fallback lane 2026-09-20);
  - Bazel: gwz-cli `b2b24ed`'s `cfg-if` has no spec, and `MODULE.bazel.lock` cannot be regenerated until the fork rename (TR3.1) lets the `gwz_core_crates` hub resolve `../git2-rs`;
  - tests `runtime_timeout_refuses_changes_after_backend_creation` (`1ac248ca`'s 9000 ms default), `native_same_tree_receiver_ref_reproduces_noncommittish_error` (the fork's libgit2 1.9.7, `26b30ca6`) and `generated_protocol_is_current` (taut `9d46310`);
  - gwz-core clippy with all features (36 errors) and rustfmt (4 files); gwz-cli's local-clone and merge docs tests.
- **Next:** the commit, on the operator's go: root, gwz-core and gwz-py, with the two HEAD test fixes. Then the session plan's next steps, placed by the map: CS1.6 and Phase 2 in `gwz-session-host`; CS1.1 still waits on TR3.1.

## Transport release — crate map accepted, 2026-09-28

- **The [crate map](GwzCoreSessionCrateMap.md)** is drafted at r0 `eac8e018…`, on the operator's "ok on crates". It re-homes the code of the session plan's remaining steps into small crates:
  - six ordinary crates in `gwz-core/crates/`: `gwz-ids`, `gwz-session-contract`, `gwz-session-channel`, `gwz-session-host`, `gwz-server-policy` and `gwz-server-os`;
  - five transport crates, kept out of the ordinary build until activation.

  gwz-core stays the composition root, and the GWZ protocol, dispatch, secrets and git2 stay in core. The map rests on three read-only surveys: the plan's steps by area, the transport code and gwz-transport, and the channel, host and server units.
- **Review round 1** on r0 was NO-GO, with P2 ×3 and P3 ×6 ([report](GwzCoreSessionCrateMap-ReviewCode.md)). The findings:
  - the transport crates had forbidden dependency edges and needed a contract crate;
  - the gh helper's environment and SSH key text would leave core;
  - gwz-transport's pool ID had no legal source.

  Revision 1 (`d0c82295…`, 207 lines) answers every finding in its §9.
- **Round 2 on revision 1 was GO:** all nine findings closed, and the map's P3-6 departure was confirmed ([report](GwzCoreSessionCrateMap-ReviewCode-1.md)). Its four new P3s were applied after GO (`584b8431…`, 220 lines):
  - the HTTPS bridge becomes `gwz-https-endpoint`'s `Engine`;
  - `prepare.py` gives `gwz-ids` one path;
  - reuse §14 joins §7;
  - `test-support` features.

  The same reviewer confirmed them ([confirmation](GwzCoreSessionCrateMap-ReviewCode-1a.md)), and GO stands. Its two notes are applied as worded. The reviewed text is `d78393ae…`, 222 lines. Root `147bf11` committed the map at `1493c5d2…` (224 lines) with its three review files. Recording the rules' adoption makes it `49442dce…`, 226 lines, uncommitted.
- **Revision 1's shape:** six candidate crates (`gwz-endpoint-contract`, `-policy`, `-registry`, `-instance`, `gwz-ssh-endpoint`, `gwz-https-endpoint`), and thirteen crates.io names to bootstrap in all.
- **Decided (operator, 2026-09-28):** the candidate crates live in gwz-core, as the second workspace `gwz-core/candidate-crates/`, so the repositories stay a DAG: gwz-core depends on gwz-transport, never the reverse. They were called the transport crates, in `gwz-core/transport/`, until the operator read that folder as the gwz-transport repository.
- **Decided (operator, 2026-09-28):** the thirteen new crates.io names are registered together, just before release preparation begins.
- **Adopted (operator, 2026-09-28):** the map's §1 rules, including that no counter, flag or thread-local is ever `permanent`. The map is accepted.
  - Of the text its §7 amends, the [library boundaries](GwzLocalCloneLibraryBoundaries.md) (revision 3: every new gwz-core library) and gwz-core's `AGENTS.md` changed on adoption, uncommitted.
  - The allowlist's definition changes in step 1. The contract, `GWZDesign.md`, the server design and the reuse design change in the step that moves their code, or at their next revision; the map controls until then.
- **Next:** the map's first steps (the checkers, `gwz-ids`, the channel crates and the host crate), built back to back and reviewed once. CS1.2 becomes the channel crates. The two HEAD test fixes stay applied and uncommitted until the operator's go.

## Transport release — CS1.9 committed; review granularity; modularization open, 2026-09-28

**CS1.9 is committed** as accepted: root `e4f00820c00e3eea58c684477d30a0592d31914b`, gwz-core `53b2b0e878d0192363d81c610bf3654b04d3d740`. No tag, no push.
- **Review granularity** (operator ruling, [process §8](GwzProcessOptimization.md)): one Consistency and Safety review per phase; per-step dual review only for the wire format, secret handling and the release gate; steps up to an aspirational 500 production lines. It overrides the session plan's recorded tiers and budgets without editing the plan.
- **Open — modularization.** The operator observed that the session plan builds a new subsystem inside gwz-core, specified by prose contracts, rather than as small crates with narrow APIs that build and test alone, as the local-clone work did under [the library boundaries](GwzLocalCloneLibraryBoundaries.md) (LBT-001–012; 14 crates in `gwz-core/crates/`).
  - That policy was scoped to local-clone libraries. gwz-core's `AGENTS.md` does not mention it, and neither the session plan nor its review prompts cite it.
  - The session host is still nearly standalone: its only use of core is `crate::model`'s error types, in four files.
- **Open — oversized files** (raised by the operator the same day). Ten product files are past L1-23's 1,000-line cohesion alarm, and no checker enforces it.
  - Six are transport files written after the 2026-09-16 split round, each past 1,000 lines within two days of creation: gwz-py `native/src/transport_session.rs` (1,507), gwz-transport `src/protocol.rs` (1,433), and gwz-core's `git/endpoint/ssh_worker.rs` (1,279), `placement_endpoint.rs` (1,064), `https_worker.rs` (1,056) and `transport_host/session.rs` (1,009). The session plan defers splitting the gwz-core four to CS7.1.
  - gwz-py `src/gwz/client.py` (1,380) still awaits the client-layer review that debt recovery called for on 2026-09-07.
  - Three are kept with written reasons ([split plan](../gwz-core/dev-docs/GwzRustSplitPlan.md)): `contract.rs`, `fake_repository.rs` and `checked_artifact/entry.rs`.
- **Done — obsolete candidate code deleted** (operator: "yes - delete obsolete code"; committed with this entry on the operator's go). This pulls CS4.8's `TransportSession` removal (1.1.0 S6.2) forward; none of it was ever released.
  - gwz-py: `native/src/transport_session.rs`, its registration and dispatch hooks, and the bridge's candidate-session branches. `test_transport_session_api.py` and `test_transport_session_native.py` go; their three tests of surviving behaviour move to `src/tests/test_bridge_legacy_path.py`, with one new test that cancel and release report "unavailable" until Phase 4.
  - gwz-py, dead once the session went: the operation store's identity ledger and deferred-terminal machinery, and the thread-locals `CURRENT_SESSION`, `SCOPED_STORE`, `SCOPED_BACKEND` and `SCOPED_OPERATION_ID` with their helpers. gwz-py's process-globals allowlist falls from 8 entries to 4.
  - gwz-core: `TransportRuntime::from_environment`, whose only caller was the session. CS3.1 no longer converts it.
  - **Bug fixed, test first:** since 2026-09-24 a native worker that panicked in an ordinary build parked its failure for a transport finish that never came, so the operation never completed. It now publishes an `InternalError` failure at once, as v1.0.17 did (`a_panicked_operation_publishes_its_failure`).
  - Checks: the gwz-py suite passes, 918 tests (baseline 926 passed and 13 skipped: the 13 and 12 of the passes were the deleted tests, and 4 are moved or new). gwz-py's 8 Rust unit tests pass; they cannot link in the `extension-module` build and ran with libpython linked by hand. Clippy is clean apart from one earlier `needless_update`, rustfmt is no worse than HEAD, and the cfg checker reports nothing new. The candidate build fails before and after with identical errors (TR3.1), and nothing refers to the removed code.
  - Plan text for the next revision: CS3.1's file list; the clauses that keep per-process slots and budgets for the candidate session; §2.3's transport rows; CS4.1's and CS4.8's rows for the deleted files; R11.
- **Open — globals** (operator: thread-locals are globals, and globals are not allowed). CS1.4's `src/session_host/gate.rs` keeps a `CROSSING` thread-local, allowlisted `permanent`; gwz-core also lists 9 statics as `permanent`. gwz-py's three remaining statics are `debt` that CS4.8 removes.
- **Next:**
  - the operator decides whether to re-cut the rest of the session plan around crates; if so, a short crate-map design comes first;
  - CS1.2 waits for that decision, since it would add to `src/session_host/`;
  - the two HEAD test fixes are applied on the operator's go and pass (`scripts/test_release_bump.py` 12 OK, `tests/publish_workflow.rs` 13 passed); they are not in this commit.

## Transport release — CS1.9 accepted, 2026-09-28

**CS1.9 is accepted** ([Verdict](GwzCoreSessionCS1.9-Verdict.md)). Both axes gave GO in round 1, with no findings of any severity ([Consistency](GwzCoreSessionCS1.9-ReviewConsistency.md), [Safety](GwzCoreSessionCS1.9-ReviewSafety.md)).
- **The object:** six gwz-core files on `3f99e49c`, whose hashes the verdict lists. They are uncommitted.
- **What it adds:**
  - `HostContext::shutdown()` with a 5 s bound and a `ShutdownReport`;
  - `SessionOptions::transport_off`;
  - the proof of the snapshot's zeroization, and a Windows decoding fix that stops freeing unwiped partial values.
- **Plan text for the next revision** (the verdict's "Recorded at acceptance"):
  - CS1.9's file list and §4's `session_host/mod.rs` entry gain CS1.9;
  - its measured budget, 246 or 254 production lines;
  - quarantined jobs are dropped when the supervisor stops, which after `shutdown` precedes the host context's drop.
- **Carried to later steps:** the verdict's table. Both axes raised, independently, how CS6.6 combines `peer_cleanup_confirmed`.
- **Next:**
  - commit CS1.9 on the operator's go;
  - then CS1.2's queues, which register their modules in `session_host/mod.rs` after CS1.9 (L1-06);
  - CS1.1 still waits on TR3.1 in the other lane.

## Transport release — TR1.4b closed; CS1.4, CS1.5 and CS1.7 accepted, 2026-09-28

**The session plan is accepted in full** ([Verdict-2](GwzCoreSessionPlan-Verdict-2.md)).
Revision 3 (TR1.4b) had GO from both axes at `01adc2f0…`, with 5 and 6 P3s. The
reviewers confirmed the drafter's post-GO pass
([Consistency-2a](GwzCoreSessionPlan-ReviewConsistency-2a.md),
[Safety-2a](GwzCoreSessionPlan-ReviewSafety-2a.md)), and the lane owner applied
their last notes with the status sentence. The final text is `5d1dc819…`, 1569 lines.
- **Scope:** 8 phases and 116 steps. Phase 7 is reuse (CS7.1–CS7.27) and Phase 8
  the server (CS8.1–CS8.32); CS1.9 and CS1.10 are the extensions.
- **Review tiers:** 28 dual, 74 single-axis and 3 Surface.
  - CS8.9 with CS8.10 is one dual socket-host checkpoint.
  - CS8.17–CS8.19 are one dual Windows checkpoint that re-freezes CS8.5, CS8.8 and
    CS8.14, re-running their counterexamples on Windows.
- **Blind convergence, three times:**
  - the server release gate's `env` debt, which CS7.24 now clears alongside CS6.5;
  - reuse §15's cancellation and explicit-identity rows, now owned by CS7.9 and
    CS7.11;
  - the shared-file ordering. Every pair of steps that name one file is now ordered
    by an edge or a named L1-06 handoff.
- **New edges for the release plan's §6 sketch:**
  - TR3.1 → CS1.1 → CS1.10;
  - CS1.10 → the server lane SC (CS8.5, CS8.8, CS8.13), with CS1.3 → CS8.14;
  - CS5.4 before the server steps;
  - CS8.3 → CS8.22 and CS8.29;
  - CS4.9 after the Phase 7 exit.
- **When CS7.1 merges,** its split owners replace "the owners of" in the steps, and
  the sketch is redrawn. Record both here then.
- **Recorded, open, in the plan:**
  - C9, the exit bound with several Clients (contract §10);
  - C10, `git credential fill` runs in `/`;
  - C11, the credential spawn drops `GIT_DIR`, `GIT_COMMON_DIR` and
    `GIT_WORK_TREE`, pending the contract's next revision.

  The release plan's Phase 7 exit still names only gwz-core's checker. The plan's
  gate names both, and changing that line needs the operator's sign-off.
- **TR1.7 is applied:** gwz-py's `GwzPyTransportDesign.md` is superseded by the
  contract's §9, §10 and §14.
- **Before the review,** the server design took an erratum: §12's Linux walk row now
  refuses without `/proc`.

**CS1.4 with CS1.5 is accepted** ([Verdict-1](GwzCoreSessionCS1.4-CS1.5-Verdict-1.md)):
12 gwz-core files, whose hashes that verdict lists.
- Round 1, both NO-GO. The blind convergence: `capture()` read the environment
  inside core. Round 2, both GO.
- The step's shape:
  - core never reads the environment; drivers pass `from_os_pairs`;
  - a panicking supervised job is quarantined;
  - a nested gate crossing panics;
  - `read_bytes` is at most 32 MiB and `close_wait` at most 1 hour;
  - Windows names compare with `CompareStringOrdinal`.
- The allowlist has 31 entries: 18 debt and 13 permanent, the new one
  `thread_local CROSSING`.
- CS1.4 measured 505 production lines against < 450. Windows was accepted by
  reading; its tests first run in Windows CI.

**CS1.7 is accepted and closed** ([Verdict-1](GwzCoreSessionCS1.7-Verdict-1.md)).
- The Consistency review escalated to Safety. Both found, blind, the unscanned
  statement class.
- Round 2, both GO. Two post-GO patches followed, each confirmed by both reviewers
  ([C-1a](GwzCoreSessionCS1.7-ReviewConsistency-1a.md),
  [S-1a](GwzCoreSessionCS1.7-ReviewSafety-1a.md),
  [C-1b](GwzCoreSessionCS1.7-ReviewConsistency-1b.md),
  [S-1b](GwzCoreSessionCS1.7-ReviewSafety-1b.md)).
- The inventory is 437 occurrences under 432 keys: 264 items and 173 statements.
- `--shrink-from` runs in the boundary job. It sums per occurrence across files,
  refuses a narrowed scope, and records the move-versus-recreate trade-off.
- Sibling coverage stays local-only. The follow-up puts the check in gwz-cli's and
  gwz-py's CI once CS1.7 is pushed to gwz-core's `main`.

**Also found, 2026-09-28:** two failures at gwz-core HEAD have test-only fixes
proposed, awaiting the operator's go.
- `test_release_bump.py`: the forks' `gwz-` prefix since `26b30ca6`.
- `publish_workflow.rs`: a text-order assertion, broken by `4bd92285`; the release
  order itself is right.

**Next.**
- CS1.9 (the host context's bounded `shutdown`, a dual re-freeze of CS1.4) is
  unblocked by TR1.4b's GO.
- So are CS1.2's queues.
- CS1.1, and with it CS1.6, CS1.10 and most of Phase 2, waits on TR3.1 in the other
  lane.
- CS1.9 and CS1.2 both change files of the accepted CS1.4 object. They start from
  the commit below, so each review object stays exact.

**Committed** with this entry, on 2026-09-28, as one gwz commit across the root,
gwz-core and gwz-py. It holds:
- contract revision 5, the reuse and server designs, and the session plan;
- CS1.4, CS1.5 and CS1.7, at their accepted hashes;
- every review record.

The two test-only fixes are not in it. Nothing is tagged or pushed.

## Transport release — TR1.3 closed: server design accepted, OD12 yes, 2026-09-28

The [server design](GwzCoreServerDesign.md) is accepted as TR1.3, by dual
Consistency and Safety review plus Surface.
- **Round 1 NO-GO on revision 1** (`15d5410f…`):
  - Consistency GO with 11 P3;
  - Safety NO-GO on a P1 (links planted at the lock and log files) and three P2s
    (the Linux relative sandbox rule, an unbounded handshake, and stale-socket
    removal of a live foreign socket);
  - Surface NO-GO on two P2s (two `--ssh-timeout` defaults, and actions as flags).
- **Round 2 GO on all three axes** (`9fc80261…`), with post-GO corrections and a
  narrowing of stale-socket removal that all three reviewers confirmed. Final
  `61d8dfa7…`.
- No architectural root cause, and the two-round cap was not reached.

The design:
- `gwz server start|stop|status|list|stdio` in both CLIs, opt-in (OD2).
- Only the same user, on the same machine, from outside any sandbox. The rule is
  absolute on every platform, so there is no server inside a seccomp-profiled
  container.
- The server speaks first with a secret-free `SessionHello`, within a bound.
- A must-match set that includes the native path's and OpenSSL's process-wide
  reads, enumerated from the vendored sources.
- Files it creates are never followed through links, and a socket is removed only
  when gwz's own record names it.
- The stdio mode, and the SSH remote form.

The operator's decisions:
- **OD12, 2026-09-28:** yes. The SSH remote form ships.
- **Sign-off:** the corrections the design carries to the release plan's Phase 10
  spelling and to the amendment's §3.4 probe and `/net` sentences and §3.6 macOS
  rows.

The "On GO" list is applied to:
- the contract's status, amended for §1, §3, §4.1, §4.2, §5.1, §5.6–§5.8, §9–§13,
  §15 and §16;
- the release plan's and the amendment's status and changelogs;
- the reuse design's §11 note;
- the proposals' G1 row;
- new "Core server" paragraphs in gwz-core's GWZDesign and GWZRequirements, which
  also now name the git2 crate as the credential helpers' spawner.

Found at design stage, not escaped:
- the amendment's named macOS probe would itself mount an automount trigger;
- macOS has shipped `/net` disabled since autofs-281;
- on Linux the transport's own TLS takes its default roots from the process, and
  openssl-probe rewrites `SSL_CERT_FILE`/`SSL_CERT_DIR` at startup;
- the macOS build links Homebrew's OpenSSL dynamically. A separate task was
  offered to check the published artifact.

Recorded for TR1.4b, with the rest in [Verdict-1](GwzCoreServerDesign-Verdict-1.md):
- Linux CI rows that run a server need a runner outside a seccomp-profiled
  container;
- the session plan names gwz-py's console script `gwz`, where it is `gwz-py`.

Next: TR1.4b (the session plan's second revision). On the operator's "continue
impl" of 2026-09-28, implementation starts with the Phase 1 steps that need no
TR3.1: CS1.7, and CS1.4 with CS1.5. Each is reviewed at the tier the session plan
records. Nothing is committed, tagged, pushed or published.

## Transport release — TR1.2 closed, session plan part 1 accepted, 2026-09-28

Accepted since the entry below, each by dual Consistency and Safety review, as
text or design only. No review found an architectural root cause, and no
two-round cap was reached.
- [Connection reuse design](GwzConnectionReuseDesign.md), TR1.2. Round 1 NO-GO
  on `573d4e95…`, one P2 per axis; both axes GO on revision 1, `e8ee63f8…`.
  The host context shares endpoint instances per endpoint configuration, and
  each operation has its own binding. A pooled SSH connection serves another
  operation only after the agent proves it still holds the connection's key and
  known_hosts still trusts the host. On 2026-09-27 the operator adopted §16.1's
  fourteen decisions, folding decision 14's Surface review into S7.5's, and kept
  the signed possession proof. On 2026-09-28 the operator signed off the release
  plan's two changed clauses: TR1.2 question 3 and Phase 6's swapped-agent row.
  The design's "On GO" list is applied: the contract's and the release plan's
  status, the paired GWZDesign and GWZRequirements paragraphs, and the status of
  eight accepted gwz-core designs and plans. **TR1.2 is closed.**
- [Session plan](GwzCoreSessionPlan.md) revision 2, TR1.4a, for the parts TR1.4a
  revises. Round 1: Consistency GO; Safety NO-GO on two P2s, direct callers'
  legacy context and the candidate's hand-kept protocol file. Both axes GO on
  `58ab341a…`. Steps marked "TR1.4b revises this" stay draft. Steps marked
  "TR1.4b extends this" (CS1.1, CS1.4, CS2.12, CS3.6, CS6.1 and CS6.4) may
  merge now, and TR1.4b's extension of each is a dual re-freeze.
- [Core session contract](GwzCoreSessionDesign.md) revision 5: both axes GO in
  one round on `6d12f03e…`. `cancellation` is field 8 of the transport
  capabilities response, and may be absent. Fields 3–7 stay with the placement
  design's capability fields. This settles the session plan's C8, which the
  operator decided on 2026-09-27.

G0 is discharged. The paired GWZDesign and GWZRequirements paragraphs are no
longer marked DRAFT, and they name revision 5. The session plan's G0 text still
says revision 4; TR1.4b corrects it.

New dependency: CS1.1 waits on TR3.1 (G2), for its rename in candidate-only code
and its candidate build. Its other new gate, contract revision 5, is met.

Review tiers (GwzProcessOptimization §4.2), as the session plan records them:
- **Dual:** the freezes CS1.1 (schema), CS1.2 (channel contract), CS1.4 with
  CS1.5 (gate and context) and CS1.6 (dispatch signature); CS3.4 (secret
  handling and credential lookup); and the Phase 3 exit.
- **Dual plus Surface:** the Phase 4 and Phase 6 exits.
- **Single-axis,** on the first axis the step names: every other step, and the
  Phase 2 and Phase 5 exits, both Safety first. CS3.11 is reviewed in the
  Phase 3 exit, CS4.1 with CS4.6, CS4.7 and CS4.8 in the Phase 4 exit, CS5.3 in
  the Phase 5 exit, and CS6.4 and CS6.5 in the Phase 6 exit. Phase 1's exit
  needs no further review.

Recorded for later:
- The candidate build's generator pins the core schema at gwz-core `54618449`
  (`551fe930…`) and the gwz-transport owner schema at `10179189…`. Both are
  behind HEAD by design, and CS1.1 re-pins both.
- Between CS4.7 and CS4.8, a gwz-py process that uses both paths holds two sets
  of process budgets, at most twice today's. This is a development window only.
- `EVIDENCE.md` names `D:/gwz-tests` for Windows fixtures, which is stale.

Next: TR1.3, the server design revision, still an unreviewed draft. Its GO also
settles OD12, whether the SSH remote form ships. TR1.4b follows TR1.3's GO, and
CS1.1 can start once TR3.1 lands. Every finding so far was found at design
review, and none escaped. Nothing is implemented, activated, tagged, pushed or
published.

## Transport release — plan, amendment and session contract accepted, 2026-09-27

The next minor release is the **transport release**, expected to be v1.1.0: the
transport, the core session host, `gwz server` and connection reuse across
commands (operator decisions of 2026-09-26 and 2026-09-27). Accepted, each by
dual Consistency and Safety review, as text or design only:
- [Transport release plan](../gwz-core/dev-docs/GwzTransportReleasePlan.md).
  Round 1 NO-GO on `90fbd213…`; both axes GO on `4ec6ba33…`. The operator adopted
  OD1–OD10. It supersedes the 1.1.0 plan and adopts that plan's still-valid steps
  by ID, so the 1.1.0 section below is historical relative to it. gwz-core
  `23d8ed9b`.
- [Its amendment](../gwz-core/dev-docs/GwzTransportReleasePlanAmendment.md):
  ECDSA agent keys, and a native route for keys the transport cannot sign with
  (OD11, adopted); CA bundles; strict server addresses; a sandbox check on the
  listener; native routes through a server; a stdio server mode; and the SSH
  remote form as a design question, with OD12 open until TR1.3's GO. Round 1
  NO-GO on `9ef88e44…`; both axes GO on `213a164b…`. gwz-core `bd538656`.
- [Core session contract](GwzCoreSessionDesign.md) revision 4, closing TR1.1
  (Verdict-4, round 4 GO). gwz-core's tests fail closed without a gwz-transport
  checkout, and its CI checks gwz-transport at the pinned `46e65a9…`. Root
  `fa3f44c`, gwz-core `4bd92285`.
- The 1.1.0 plan's amendment of 2026-09-26, accepted at `cb4ae166…`, is now
  superseded in part by the transport plan. It retired the Python long-lived
  session named in the section below. gwz-core `b5279083`.

Unreviewed drafts: the [session plan](GwzCoreSessionPlan.md) (TR1.4a started),
the [server design](GwzCoreServerDesign.md) (TR1.3, after TR1.2) and the
[reuse design](GwzConnectionReuseDesign.md) (TR1.2 review started). No review
found an architectural root cause, and no two-round cap was reached. Found at
design stage, not escaped: the transport's agent signing refuses ECDSA and
security keys and ends the login; it loads one certificate of a CA bundle, and
macOS refuses a bundle; a Windows remote pipe name signs in over SMB before any
check. Nothing is implemented, activated, tagged, pushed or published by these
acceptances. The "Transport implementation checkpoint — 2026-09-23, review
pending" section was removed as stale (root `3cd0dc6`).

## Retry/concurrency and Python lanes — authorized, 2026-09-23

Operator authorized parallel work after the recommended dependency split.
Retry plan accepted as text at core `ef29f8907875928b6e6891a2db12cbe3ca781fee`,
SHA-256 `08e198e00c5f6ff697dca6b71f8117ce8963afb91126a30af2b5ea94a6ac6619`;
Consistency-3, Safety-3 and Surface-3 report GO. Existing retry rules/defaults
remain the reviewed plan; implementation is not yet accepted.

GPT-6 Sol owns three parallel implementation chunks: jobs-bounded fallible
scheduler/defaults; typed setup causes plus shared core host APIs; and Python's
long-lived native session. Python owns only gwz-py, the scheduler owner only core
operation/CLI files for now, and the timeout owner transport schema/pool plus core
endpoint/host files. Full retry/pool policy integration follows the shared foundation.

The paired setup-failure and Python designs have Consistency/Safety/Surface GO at
root `00827c75afb93f5855ee77019df7cb459c12754a`, core
`479926c18265276e5a45659c4523a13a71f4a51f`, Python
`259f73cc030c0da0bf29903bab258de0463b7d02`, transport
`aa40936d0805e8cb60f8027615abe20d4f2045e4`. Reports are
`GwzTransportParallelInterfaces-Review{Consistency,Safety,Surface}-1.md`.
The accepted corrections freeze enum wire values and race-safe Python close/cancel.
No product implementation acceptance is implied by design GO.

Timeout baseline corrections now pass focused native SSH, actual endpoint stall,
and candidate driver/configuration gates. Raw logs and source patch are in the
private evidence run `transport-qualification/runs/2026-09-23-timeout-clock-baseline`.
The typed cause implementation must still remove the temporary fingerprint overload
before acceptance. No alpha rebuild, release activation, push or publish occurred.
Q6 aggregate and broader platform/package gates remain open.

## GWZ 1.1.0 plan — accepted as text, 2026-09-23

[Plan](../gwz-core/dev-docs/GwzV110Plan.md). Dual Consistency and Safety.
Round 1 NO-GO on `a52cd7a8…`. Round 2 NO-GO on `6ec8f7e7…`. Both axes GO on
`9d49af85bd340addc1c35e6c11eefe3e4ab9f2bc2a9143b8f13f3af5a7fc8f62`. Accepts
the plan text only. It is the resume of the paused transport release gate,
aimed at v1.1.0. The timeout-plan S5.1/S5.2 close and the Q6 aggregate
review are steps inside that plan (S3.1–S3.3). They are not done. No tag,
publish, or implementation is authorized by this acceptance. The older
"paused, do not resume" wording below is historical relative to this plan.

## Alpha SSH setup-timeout plan — accepted as text, 2026-09-22

[Plan](../gwz-core/dev-docs/GwzRemoteTransportAlphaTimeoutPlan.md). Draft review,
dual Consistency and Safety. Round 1 NO-GO on plan SHA-256
`773639e1b545eeba776c3faeac1b0785aef50ee33faa0ba1e984b5ec214cac4a`; both axes
converged on a scripted stall test that could pass while live setup stayed one
cumulative budget. One remediation. Re-verdicts GO on
`cfdf028fb18557960da18a4682cb10f3e9e638ff197c4784efb76ac9a526984b`. Accepts the
plan text only. No timeout implementation, alpha rebuild, or Q6 resume.

## Alpha GitHub compatibility correction — installed, 2026-09-22

[Fix and exact acceptance](../gwz-core/dev-docs/GwzRemoteTransportAlphaGitHubFix.md).
Removed noncanonical SSH `--` separator, preserving quoting and rejecting
option-shaped paths. Runtime-red regression, three channel tests and live GitHub
fetch verified. Retained Code GO; corrected gwz-alpha installed. Private evidence
access fails with both alpha and stable in the agent tool environment; this is
not claimed resolved. No timeout reproduced. Broader Q6 remains paused.

## Local HTTPS alpha — accepted and installed, 2026-09-22

Operator authorized HTTPS activation for the local alpha only.
[Scope/configuration and focused tests](../gwz-core/dev-docs/GwzRemoteTransportAlpha.md).
Actual-binary HTTPS clone/fetch/push, gh failure/proxy refusal and SSH regression
pass. Retained Code/State GO after one local-command-isolation correction.
Installed `gwz-alpha` version `0.2.0-alpha.transport` enables local SSH+HTTPS,
with HTTPS authentication through gh. Exact tuple/hash in linked alpha report.
Broader Q6 remains paused; the SSH-only alpha entry below is historical.

## Explicit local alpha exception — installed, 2026-09-22

Operator requested `/Users/owebeeone/.cargo/bin/gwz-alpha` with candidate transport.
Installed `0.2.0-alpha.ssh-transport`, SHA256
`b64a67eb19f57059d9e122da8a2607ab23553f9f9d3a618bca57aae20324858c`.
Enables automatic local SSH per-remote streams/pooling using the patched Git stack.
HTTPS remains native; host/carried frontend activation is not included.
Disposable SSH clone/fetch and installed help/build-info pass. A one-line CLI
TransportOptions default initializer fixes compilation against candidate fields.
Private build/manifests/logs: transport-qualification/runs/2026-09-22-alpha-ssh.
Normal gwz is unchanged. This exception does not resume Q6 review or wider rollout.

## Q6 platform/source and local performance — paused by operator, 2026-09-22

[Batch scope, results and limits](../gwz-core/dev-docs/GwzRemoteTransportQualification.md).
Portable transport passes on macOS ARM64, Linux x86-64 and Windows x86-64.
Corrected integrated host passes on Mac/Linux; current Linux endpoint suite and
Windows selected native binding pass. Cold/warm measurements verify actual reuse;
large repeated HTTPS clones now survive the five-second cleanup boundary.

Qualification found one post-acceptance P2: repeated logical mux retirement closed
healthy shared sessions. Monotonic retirement fix has a deterministic runtime-red
regression and corrected green suites. Q5 State P3-1 diagnostic-retention correction
also passes locally/natively Windows. Retained Code/State review has NOT been dispatched. Operator requested a pause
to conserve weekly quota. Resume with the bounded Q6 review, not another test sweep.
No released escape, activation or release. Full Phase6 remains open: Windows
integrated implementation, unavailable ARM64 Linux/Intel Mac rows, distribution,
both-placement aggregate operations, coalescing/default tuning and sustained memory.
This current section supersedes older platform/source deferrals below.

Resume guide: [release readiness and exact handoff](GwzRemoteTransportReleaseReadiness.md).
Core correction/source-results checkpoint: `a2a7878d` (full tuple in the guide).
Raw evidence has been preserved in the private Q6 campaign. No tests remain running.
Four old untracked N2b prompt files remain untouched.

## HTTPS H2 — accepted private host/command integration, 2026-09-22

[Scope, evidence and exact nine-repository tuple](../gwz-core/dev-docs/GwzRemoteTransportHttpsH2.md).
Accepted at root `2380a234bf620bacf73a1924f4ac23000385f758`, core
`c92abc4110fc7c1ef89600118284724c942f8985`, evidence
`3302b5d03f56590a6d521b1b52db302861775e15` after retained
[Code GO](GwzRemoteTransportHttpsH2-ReviewCode-2.md) and
[State GO](GwzRemoteTransportHttpsH2-ReviewState-2.md).
Annotation commits do not expand the accepted implementation.

Accepted: H1 HTTPS endpoint through local and carried host paths, all network
command funnels, Rust/Python same-process messages, shared SSH+HTTPS physical
capacity, authentication observations, refusal classification and cleanup ownership.
Placement C State P3-1 is closed by cleanup snapshots and gated physical disposal.
Host52, endpoint69, observation3, binding2, default core and scoped format/conditional
checks pass. Raw evidence, runtime reds and exact fingerprints are private.
No open H2 findings; no public activation, production or release claim.

One aggregate gate, two corrections. Initial Code2P2/State2P2 converged on retry
correlation (three unique blocking roots); correction-1 Code found one new
architectural allocation-budget root, verified closed in correction 2. Draft-stage
corrections and TDD/evidence limits are retained in H2. No known released escape.
Production-bearing files including inline tests: +1541/-130, 12 files; separate
test/fixture files: +2571/-24, 12 files. End-to-end wall time was not captured.

Next: Phase 6 local aggregate measurement/tuning and rollout readiness. Platform
and selected-source checks remain deferred together. Public construction and
production activation, physical wire/iroh, real accounts and release stay separate.
Four old N2b prompts remain untouched.

## HTTPS H1 — accepted private endpoint/RPC candidate, 2026-09-22

[Scope, evidence and exact nine-repository tuple](../gwz-core/dev-docs/GwzRemoteTransportHttpsH1.md).
Accepted at root `63ef26308979b6ce2e2925d71a96f42afcde2645`, core
`e29e799ee65fb9794ac2fad7972d94707262b4cb`, evidence
`fc1caa478c1fcd9539b2c061c51be17b64924d7c` after retained
[Code GO](GwzRemoteTransportHttpsH1-ReviewCode-2.md) and
[State GO](GwzRemoteTransportHttpsH1-ReviewState-2.md).
Documentation annotations do not expand the accepted implementation.

Accepted: HTTPS Gh-only authenticated endpoint, TLS/proxy ownership, physical
pooling, per-remote native Git RPC, real mux opening/stream composition,
independent timeout domains, scoped helper/operation cleanup and truthful facts.
Endpoint66, default core check, formatting and conditional-boundary gates pass;
full transport gate remains applicable at unchanged sources. No open H1 findings.

One aggregate gate and two corrections. Initial Code3P2+2P3 and State4P2 shared
one route-lifetime root (six distinct blocking roots). Owner audit after
correction1 dual GO found a reaping/accounting continuation; both reviewers
verified it closed in correction2 and classified it nonarchitectural. No released
escape is claimed. Process/evidence deviations are retained in the H1 report.

Next: H2 host/all-command Rust/Python embedding, shared SSH+HTTPS authority
injection and Placement C cleanup-accounting P3. Platform/selected-source checks
remain deferred together. Physical wire/iroh, production construction/activation
and release remain separate. Four old N2b prompts remain untouched.


## HTTPS detailed design — accepted, 2026-09-22

[Design, exact nine-repository tuple and implementation gates](../gwz-core/dev-docs/GwzRemoteTransportHttpsDesign.md)
accepted at root `bcdca800ab19fb767f6e7d2ab8107f12dab48810`, core
`2ea02835a15a9f56afdda43ccbcadec66b5b776e` after retained
[Consistency GO](GwzRemoteTransportHttpsDesign-ReviewConsistency-2.md),
[Safety GO](GwzRemoteTransportHttpsDesign-ReviewSafety-2.md), and
[Surface GO](GwzRemoteTransportHttpsDesign-ReviewSurface-2.md). No open design
findings. Annotation commits do not expand the reviewed design.

Accepted: HTTP/1 per-connection adapter using existing gwz-transport streams/pool;
endpoint-local gh/TLS/proxy ownership; body completion and early-failure lifecycle;
bounded discovery auth/redirect behavior, immutable operation routes, precise
HTTP status/refusal rules and truthful request-scoped HTTPS authentication facts.
No new fields, production activation, public constructors or wire implementation.

One initial dual gate and two consolidated corrections, with retained Surface
added for exposed observation semantics. Initial Consistency3P2 and Safety2P2
shared the no-replay root (four distinct blocking roots), plus Safety1P3 status
grammar. Correction1 introduced one ConsistencyP2 pre-open message defect;
correction2 closed it. All findings verified closed by raising reviewers.
Owner correction clarified final connection identity and shared pool capacity.
No known released escaped defect; production/test code LOC0. Documentation links,
fences and whitespace checks pass; no implementation test claim, TDD starts H1.
Wall time not captured.

Next: H1 complete HTTPS endpoint/RPC candidate plus local fixtures, then H2
host/all-command integration and Placement C cleanup-accounting P3 closure.
Use substantial aggregate review batches. Platform and selected-source checks
remain one operator-deferred batch. Production activation/public constructors,
physical wire/iroh and release stay separate. Four old N2b prompts untouched.

## Placement C — accepted in-process proof, 2026-09-22

[Scope/results and exact nine-repository tuple](../gwz-core/dev-docs/GwzRemoteTransportPlacementC.md)
accepted at root `f3ad29ae5aa55ebd4e558f3f11a078e6b837196e`, core
`c5dd307142e6958160efabf36a8521b5f104c157`, evidence
`d096a9dcf0d43e79ea32bced5b802bf8a877ce1d` after retained
[Code GO](GwzRemoteTransportPlacementC-ReviewCode.md) and
[State GO](GwzRemoteTransportPlacementC-ReviewState.md). Annotation commits do
not expand the reviewed implementation. No blocking findings.

Existing request/response attachments carry live SSH exchanges at the CLI's
current typed direct-call boundary and through the actual gwz-py codec embedded
in the Rust process. Host33 and Python13 pass; regeneration, default library
build and scoped formatting pass. Both reviewers reran the eight embedding cases.
No full frontend activation, physical wire, platform or whole-core passing claim.

One aggregate gate, zero corrections; Code zero findings, State one settled-review
P3. **Historical State P3-1 (closed by H2 above):** assert cleanup accounting and retained-work retirement
before citing this fixture as physical-cleanup evidence at activation. Current
proof establishes waiter release and teardown return. No known released escaped
defect. Captured TDD red is an implementation-stage unimplemented-boundary failure;
compiler-attempt source hashes were not all captured. Test/harness581 additions/2
deletions across7 files; docs94 additions across2; production implementation0.
Wall time not captured. Archive verification passed5004 records, no build caches.

Next: Phase5 HTTPS adapter/authentication design and interface review. Keep
platform and selected-source checks in their operator-deferred single batch.
Frontend/production activation and release remain separate. Physical wire and
iroh stay outside this cycle. Four old N2b prompt files remain untouched.

## Placement C scope — operator clarification, 2026-09-22

Next gate: prove transport envelopes embedded in the existing Taut messages for
both CLI/core and gwz-py/core **within the same process**. Preserve ordinary calls
and existing request IDs; verify asynchronous two-way progress, backpressure,
cancellation and logical closure through both consumer bindings. The wire story
needs a plausible documented mapping only. Physical wire, separate-process and
iroh implementation/qualification are outside this development cycle. This
operator direction supersedes the earlier supplied-carrier prerequisite; it does
not claim C implemented or revise the accepted A/B code tuple. See the
[placement design clarification](../gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md#operator-scope-clarification--2026-09-22).
Platform/selected-source checks remain deferred together, with production
activation, HTTPS and release still separate.


## Phase 4 batch B — accepted, 2026-09-22

[Scope/results](../gwz-core/dev-docs/GwzRemoteTransportPlacementB.md) accepted at
root `93334058352828b1069b198d795c5860a395dc81`, core
`4f06384397a67d3dcae4856a93fd032499fda5dc`, evidence
`a2180f71f9f4f16ecc639eecd25125b980ea54f3`; all nine pins are recorded there.
Retained [Code GO](GwzRemoteTransportPlacementB-ReviewCode-1.md),
[State GO](GwzRemoteTransportPlacementB-ReviewState-1.md) and
[Surface GO](GwzRemoteTransportPlacementB-ReviewSurface-1.md) close all findings.
Annotation commits do not expand the reviewed implementation. Candidate only.

Delivered: core host facade, request-scoped backend, endpoint-owned identity/SSH
pooling, all N3 network funnels, bounded cancellation/cleanup and exact guide
compilation. No physical carrier or production activation. Local gates: host25
including50 concurrent streams, SSH137/one ignored, fetch9, backend7, preparation4,
regeneration and default library check pass. No whole-core/platform pass claim.

One aggregate gate plus one consolidated correction; initial Code twoP2 and State
twoP2 share the fetch preflight root (three roots total). Correction adds full
fetch identity preflight, logical check timeout independent of disposal and
policy checks before deadline arithmetic. Causal regressions and retained focused
rechecks pass. No new re-review findings or known released escaped defect.
Initial B TDD red capture was incomplete; failures and final source hashes are
preserved in private evidence. Final implementation3397 additions/65 deletions
across36 files; tests/harness2628 additions across17, documentation separate.
Wall time not captured. Archive verifier passes5004 records with no build caches.

Next: Placement C in-process message embedding for CLI/core and gwz-py/core,
under the operator clarification above; future wire plausibility only. Platform and selected-source checks remain in
the operator-deferred single batch. HTTPS, activation, publication and release
remain separate. Four old N2b prompt files are untouched.

## Phase 4 batch A — accepted, 2026-09-22

[Scope/results](../gwz-core/dev-docs/GwzRemoteTransportPlacementA.md) accepted at
root `d20e168bb93cbe6bf5238b0b0295f0cd51f5cd20`, core
`28f667c0462c74798761ec9710de793c697c7fb8`, transport
`03d3011b3ae9b8205bcf07f7f7862194af114856`, Taut
`bcf98b64d465fc54841121b6d1a2d46940f81a3c`, evidence
`acdd2f98f395e51c60014c8faa82e315a48c0ada`; unchanged CLI/Python/fork
pins are recorded in the retained [Code GO](GwzRemoteTransportPlacementA-ReviewCode-2.md),
[State GO](GwzRemoteTransportPlacementA-ReviewState-2.md) and
[Surface GO](GwzRemoteTransportPlacementA-ReviewSurface-2.md). No open A findings.
This accepts A only; annotation commits do not expand the reviewed implementation.

Delivered: shared v2 schema, opt-in missing-field compatibility, bounded request
mux/application ports, typed terminal failure, and isolated full-core candidate
schema/receiver-affinity fixtures. The existing CLI/core carrier interface is
unchanged. Production schema/dependencies/routes/capabilities remain inactive.
Core transport_host facade, scoped backend and endpoint workers remain batch B.

Final local transport gate: 134 tests plus one README compile doctest passed;
two extended campaigns ignored. Initial aggregate gates: Taut68, consumer Rust31/
Python28 plus regeneration, existing SSH126/one ignored serially, backend7 and
four generator checks passed. Initial parallel SSH address-fallback failure is
preserved beside the passing serial run; no cause/platform qualification inferred.
Initial Taut red was not captured (declared TDD deviation); causal mux/admission
reds and final source hashes are in the private evidence campaign. Public builds
do not require it. Archive verifier passes5004 migration records and no build caches.
Unchanged CLI release-document checker debt remains; no global docs-pass claim.

Review metrics: one aggregate Code/State/Surface gate and two consolidated
corrections. Initial Code twoP2, State oneP2 share terminal-retirement blind
convergence (two distinct roots); Surface oneP3 construction example. Re-review
Code found one changed-range P2 bootstrap-error-domain defect; round2 closes it.
No released escaped defect claimed. Wall time was not captured. Automated source
classifier across core/transport/Taut baselines counts authored implementation
1593 added/34 deleted lines across27 files; generated/retained output and test
harness are counted separately, not authored production growth.

Next: one complete SSH host/backend integration batch B, including scoped backend
metadata enforcement, endpoint-local identity preflight, all N3 network funnels,
request guard/physical cleanup ownership and full guide example compilation.
Then supplied-carrier qualification C. Keep platform and selected-source checks
in the operator-deferred single batch. HTTPS, publication, production activation
and release remain later gates. No carrier construction or remote push authorized.
Four old N2b prompt files remain untouched.

## Phase 4 endpoint-placement interface — accepted, 2026-09-22

The [placement amendment](../gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md)
and [embedding API guide](../gwz-core/docs/TransportPlacement.md) are accepted at
root `a4c5b22299be9128fef4fd212b374c669353be14`, core
`6c9abaef8ef2257371637a3f02d0771cd84bab34`; remaining six pins are unchanged and
recorded in the reports. Retained [Consistency GO](GwzRemoteTransportPlacementDesign-ReviewConsistency-1.md),
[Safety GO](GwzRemoteTransportPlacementDesign-ReviewSafety-1.md) and
[Surface GO](GwzRemoteTransportPlacementDesign-ReviewSurface-1.md) close all
blocking findings after [one merged correction](GwzRemoteTransportPlacementDesign-RemPlan.md).

Accepted: additive GWZ attachments/placement/capabilities/observations, v2 owner
identity checks and typed terminal facts, receiver-affinity admission, complete
bootstrap cancellation, endpoint-local identity preflight and concrete application
port/runtime API. No carrier, executable change or production activation.
The CLI remains a direct handler caller until a communication layer is supplied.

Review metrics: one initial three-axis gate plus one focused correction; initial
Consistency twoP2, Safety oneP1/oneP2, Surface oneP2/oneP3. Both design axes
independently found the bootstrap race (four distinct blocking root causes).
All blockers and original P3 closed; re-review Surface P3-1 is nonblocking and
assigned to batch A: name the Attachment tuple's request_id and document
core.next_message -> client.deliver and the reverse, tested in the example fixture.
No known released escaped defect. Wall time not captured. Production/test LOC 0;
this is documentation/design admission, with no compiled API/test-pass claim.

Owner checks: package links, balanced fences and core document whitespace pass.
The broader check_merge_docs.py still fails on unchanged gwz-cli/docs/Releases.md
(missing releases_unreleased_compatibility); unrelated documentation debt remains.

Next: batch A shared schema/missing-field compatibility + bounded mux/check/failure
lifecycle and in-memory regressions, then batch B complete SSH host/backend integration.
A public schema/API implementation gate is aggregate Code/State plus Surface closure;
B is one aggregate Code/State review, not a gate per command. Real supplied-carrier
qualification is batch C. Keep platform and selected-source checks together in the
operator-deferred later batch. Production dependencies/routes, HTTPS and release
remain gated; no push/publication. Four older N2b prompt files remain untouched.

## SSH N3 — aggregate backend attachment accepted, 2026-09-22

[Scope/results](../gwz-core/dev-docs/GwzRemoteTransportSshN3.md) completes the
operator-combined N3b/N3c local candidate batch. Accepted root
`7f0a844b1bb851eedd3792eb13c0194b2231a190`, core
`c79c7f13aebfcf582d0df75cff469d452e3477f1`, transport
`a6562e654b52705b72ef1f793ae2045c320cee47`, evidence
`36d29397faae5205e1e16812f9f573a122665b7f`; fork pins unchanged.
Retained [Code GO](GwzRemoteTransportSshN3-ReviewCode-1.md) and
[State GO](GwzRemoteTransportSshN3-ReviewState-1.md) close all three P2 findings in
[one correction](GwzRemoteTransportSshN3-RemPlan.md). No open N3 findings.

Shared endpoint ownership, per-operation facts, all common SSH callback funnels,
nested drivers and private-member refusal handling are integrated and tested.
Temporary construction failures can retry; earlier key rejection cannot replace
a later timeout. Final owner gates: backend7/default8/SSH126 pass, one SSH ignored;
unchanged transport94 pass/two ignored. Both reviewers reran focused closures.
Private raw evidence is in runs `2026-09-22-backend-n3` and `-rem1`. No whole-core
or native-platform passing claim. Production523 additions/22 files, tests1020/8;
one aggregate review plus one correction, three P2 discovered at settled review,
no new re-review findings or known released escaped defect. Wall time not captured.

Next implementation phase: Phase4 CLI endpoint placement using the existing
message interface, including terminal-disposition mapping. Prepare its interface
admission before implementation. The operator-deferred platform and selected-source
qualification stays one later batch; production dependencies/routes remain inactive
until qualification and activation gates pass. HTTPS and final rollout remain later
phases. This supersedes the separate N3b/N3c sequencing in older entries below.

## SSH N3a — accepted local endpoint assembly, 2026-09-22

[Scope/results](../gwz-core/dev-docs/GwzRemoteTransportSshN3a.md) composes accepted
N1/N2b into one endpoint with agent and selected-key routes and operation-specific
success observers. Accepted root `514f3cfeb233acd4e3f6c9c7e1bc07f17275373a`,
core `dfe76d0fc4a440f04262d2e1a22e40542e051925`, evidence
`e6c9226bf204f9d96b4556878c272e6409636420`; transport and fork pins unchanged.
Retained [Code GO](GwzRemoteTransportSshN3a-ReviewCode.md) and
[State GO](GwzRemoteTransportSshN3a-ReviewState.md), zero P0–P3 findings.
Full isolated SSH suite and scoped formatting pass; both reviewers independently
passed the focused gate. One aggregate review, no remediation or known escaped
defect. Production additions120/two files, tests288/one file. Reports are verbatim.

N3b operation/failure observations and backend clone/nested-scope ownership, then
N3c complete network-driver attachment remain. Native Remote objects must be fresh
when changing operation context; the endpoint is shared across them. Production
dependency/route activation remains gated by the operator-deferred platform and
selected-source batch. No new wire or public surface is introduced.

## SSH N2b — accepted selected admission and pool/worker integration, 2026-09-22

[Scope/results](../gwz-core/dev-docs/GwzRemoteTransportSshN2b.md) accepted at
root `9459ae4c2f5ad5061a2eaba92785a1f87bece938`, core
`6616a2cd66d64f04f9eb3c370e8a3a677fba1c36`, evidence
`593d2c36780d6278eee21e67dc0cc102673ed887`, and transport
`16a383e7d1c0e7e3234006688986afc2c6e54ca5`. Retained [Code GO](GwzRemoteTransportSshN2b-ReviewCode-1.md)
and [State GO](GwzRemoteTransportSshN2b-ReviewState-1.md) close both original
P2 findings in one remediation round.

The accepted fixture-attached slice admits selected key files before pool
lookup, preserves one absolute deadline through admission, checkout,
interaction and handoff, retains snapshot authority through resource disposal,
and contains pending admissions with the physical pool during shutdown.
Deterministic barrier tests prove stalled expiry, late-result disposal,
truthful pending-admission reporting, combined cleanup, and existing-stream
progress. The full locked/offline Rust 1.95 SSH and transport suites pass, and
the remediation evidence is archived in the private campaign run
`2026-09-22-selected-key-n2b-rem1`.

No production route or backend attachment is activated by this checkpoint.
Next: N3 production module and backend entry-point attachment. The operator-
deferred platform and selected-source qualification batch, HTTPS, and
capability activation remain outstanding.

## SSH N2a — accepted selected-key snapshot/authentication, 2026-09-21

[Scope/results](../gwz-core/dev-docs/GwzRemoteTransportSshN2a.md) accepted at root
`06c74a31fc61f5a70bb02b01786f099c6ba198f9`, core
`3fa6a23a2a05732ed9368e77048ba0caaf7e508d`, evidence
`8af0f7ce002148ed31d16c52e830308faaf9e5de`; other pins unchanged/in reports.
Retained [Code GO](GwzRemoteTransportSshN2a-ReviewCode-2.md) and
[State GO](GwzRemoteTransportSshN2a-ReviewState-2.md); no open findings.

N2a provides bounded reservations/immutable key snapshots, exact-byte unproven
candidate interning, fixed-scratch unencrypted-container preflight and native
in-memory authentication with joined-live proof promotion. Full isolated suite
passes for final production; final six-test container gate and both independent
causal-regression reruns pass. Production598/600 lines in three files; tests876/900
in two files. One dual aggregate gate plus two merged corrections, the second
confined to test/evidence. Initial Code twoP2/State oneP3; State then found the
noncausal regression missed by Code. All closed; no dual-axis blind convergence,
third architectural cause, or known escaped defect. Reports filed verbatim.

Next: N2b worker admission before pool lookup, resource snapshot pins and combined
retained cleanup under the existing accepted design (500/900 bounds), with a
capacity-one first-fan-out reuse test. N3 attachment and operator-deferred platform/
selected-source batch remain outstanding. Production routing remains inactive.

## SSH N2 — selected identity design accepted, 2026-09-21

[Design](../gwz-core/dev-docs/GwzRemoteTransportSshSelectedIdentityDesign.md)
accepted at root `a9ad12dcafb51d77e7d0fbd28fac97934e070b09`, core
`35df881b7075d7031082f61e0b99b838341149e1`; remaining pins unchanged/in reports.
Retained [Consistency GO](GwzRemoteTransportSshSelectedIdentityDesign-ReviewConsistency-1.md)
and [Safety GO](GwzRemoteTransportSshSelectedIdentityDesign-ReviewSafety-1.md).

One merged design remediation closes Safety's encrypted-KDF admission P2 and the
owner's first-fan-out token incompatibility P2. Identical same-Key snapshots now
share a candidate token, while only authenticated reusable physical resources
supply leases. A bounded unencrypted-container classifier refuses unsupported
KDF work before native auth. G1 records caps and representation restrictions.
No blind dual-axis convergence, executable change or new native-test claim.

Next: N2a snapshot registry/reservations, fixed-scratch container preflight and
native in-memory key authentication. TDD; retained aggregate Code/State gate;
600 production/900 test lines as scoped. N2b worker admission and combined retained
cleanup follow (500/900 bounds). N3/backend attachment and the operator-deferred
platform/selected-source batch remain later gates before capability activation.

## SSH N1 — accepted local connection/trust integration, 2026-09-21

[Scope/results](../gwz-core/dev-docs/GwzRemoteTransportSshN1.md). Accepted root
`398ca8350a88723b24660b96f7f00a0fd3e02199`, core
`5ff531cedf244e9e86d3cf33d73559b2c23bf1a9`, evidence
`e842abf855e58de3c4381855fbc1b8374485a7cd`; other pins unchanged/in reports.
Retained [Code GO](GwzRemoteTransportSshN1-ReviewCode-1.md) and
[State GO](GwzRemoteTransportSshN1-ReviewState-1.md) close both Code P2 findings
in one merged remediation. Socket failures now defer to Control for overall
termination; native CR trust bytes are preserved while CRLF size is measured once.

Owner full focused gate91 pass; both reviewers independently reran18 network
tests. Production350/350 lines; tests764/770. Two owner parity fixes before
review, two Code findings at settled review (CR independently reproduced by
owner), one correction-stage boundary regression. No dual-axis blind convergence
or known escaped defect. Raw failures, final green and native fixture penalty
diagnosis retained in private evidence. Acceptance filing changes no code.

Next: N2 concrete design/review for supervised selected-key admission, snapshot/
token bounds and deadline propagation before every pool lookup; then N3 backend
attachment. Production activation remains inactive. Platform/selected-source
qualification stays in the operator-deferred batch and remains required.

## SSH production setup — design accepted; N1 implementation, 2026-09-21

[Design](../gwz-core/dev-docs/GwzRemoteTransportSshProductionSetup.md) accepted
at root `eadf8dc25f93b3f8d9d9c4f3660732367861559f`, core
`9acf508aefe4ef974e52f19016f33ecf4bf56b34`, unchanged transport pin. Retained
[Consistency GO](GwzRemoteTransportSshProductionSetup-ReviewConsistency-1.md) and
[Safety GO](GwzRemoteTransportSshProductionSetup-ReviewSafety-1.md) close all P2s
in one merged remediation. Both identified trust-input compatibility changes;
G1 now explicitly admits bounded stores/complete lines and requires differential
native tests. A2 ordered-key progression remains; whole-connection replay does
not. Nonblocking P3 operation-order wording corrected: trust admission before DNS.

N1 native network/trust implementation is accepted above. OS DNS/file calls
stay in capped supervised helpers, retaining ownership after logical timeout.
N2 explicit authority admission and N3 backend attachment remain later scoped
work. No production routing or platform/source qualification claim; the operator's
later batch remains required before activation. A3 stays accepted below.

## SSH agent A3 — accepted local integration, 2026-09-21

[Scope/results](../gwz-core/dev-docs/GwzRemoteTransportSshAgentA3.md). Accepted
root `fe68d36f939de8cba2bc8a509f85a24cefe85db5`, core
`e92c5d1ec09dd64a397dbacf6c78888955e4ae12`, evidence
`dd5b5f144c4db29968d578655ca279a7f74c1119`; remaining pins unchanged/in reports.
Retained [Code GO](GwzRemoteTransportSshAgentA3-ReviewCode.md) and
[State GO](GwzRemoteTransportSshAgentA3-ReviewState.md): zero P0–P3 findings.
Supervised setup feeds the physical pool/shared worker and internal operation
receipts. Cleanup failure stops admission; unfinished pools retain physical
charges under the bounded supervisor after worker/endpoint exit. Reuse retains
authentication proof while clearing the new-credential-offer flag.

Full isolated Rust 1.95 locked/offline gate passes, independently rerun by State:
73 tests passed. One owner-discovered factory-panic ledger defect corrected
before review; one aggregate review, no remediation or known escaped defect.
627 production additions/seven files, 37 removed; 481 new test lines/two files.
Reports filed verbatim; acceptance filing changes no executable statements.

Next: bounded production setup discovery/connect/handshake/trust and explicit-key
handling, then backend observation-sink/all-network-entry attachment. Production
routing remains inactive. Platform/selected-source qualification stays in the
operator-deferred batch and remains required before capability activation.

## SSH agent A2 — accepted, 2026-09-21

[Scope/results](../gwz-core/dev-docs/GwzRemoteTransportSshAgentA2.md). Accepted
implementation root `53d60168cf2e2e5dc59cb4fa831276a5f69884fd`, core
`61da27a63a8df42ee92eab909be23d31db665005`, evidence
`e242351237c2a1bc006c8f6c795f2b56b5ef947f`; other pins unchanged/in reports.
Retained [Code GO](GwzRemoteTransportSshAgentA2-ReviewCode-1.md) and
[State GO](GwzRemoteTransportSshAgentA2-ReviewState-1.md) accept Unix fixture
native signing and joined authenticated connection handoff. Ed25519 and RSA
SHA-256/512 succeed, including an initial rejected key and a subsequent Git
exchange. Agent/native waits remain cancellable and use the shared deadline.

One Code P2 (overloaded native error advancing to another key) closed after one
merged remediation and a native red/green disconnect regression. Only explicit
AUTHENTICATION_FAILED advances; ambiguous PUBLICKEY_UNVERIFIED terminates with a
non-credential error. No blind convergence or known escaped defect. 62 focused
executions pass, eight A2. 267 source lines including test-only entry; 617 test/
support lines, ceiling refined to 620 for the review regression.

Next: A3 supervised setup resource in the physical pool, observable cleanup
failure and shutdown, retained physical capacity until disposal, and correct
new/reused authentication observations. Production discovery/activation remains
later work; platform/source qualification stays in the operator-deferred batch.

## SSH agent A1 — accepted, 2026-09-21

[Scope/results](../gwz-core/dev-docs/GwzRemoteTransportSshAgentA1.md). Accepted
implementation root `d552bbda5c5b8c14291243cdf73b1c1955443222`, core
`14409399bc7404446200192ffaf585f9969eec49`, evidence
`d5605a5ad81feff445d0d712940ba050c849dec3`; other pins unchanged/in reports.
Retained [Code GO](GwzRemoteTransportSshAgentA1-ReviewCode-1.md) and
[State GO](GwzRemoteTransportSshAgentA1-ReviewState-1.md) close two independent
P2s (channel state escape, permanently failed supervisor startup) and one P3
(cancellation-boundary coverage) after one merged remediation. No blind
convergence; no known escaped defect. 54 focused executions pass, including
16 A1 tests. 557 production lines/three files; 742 test lines/two files, with
ceiling refined to 750 for the requested regressions. Original red/green logs
and final fingerprints remain in the private ssh-integration evidence campaign.

A2 native signing and authenticated session handoff are now accepted above,
including cancellation, algorithm selection, native retry/allocator ownership
and concrete connection cleanup evidence.
A3 integrates supervised setup and cleanup observation into the physical pool
and backend. No production activation yet; platform/source qualification remains
the operator-deferred batch. A1 acceptance does not claim those later gates.

## Interruptible SSH agent helper — design accepted, 2026-09-21

Operator selects helper threads with owned, cancellable agent I/O.
[Design](../gwz-core/dev-docs/GwzRemoteTransportSshAgentDesign.md) accepted after
retained [Consistency GO](GwzRemoteTransportSshAgentDesign-ReviewConsistency.md)
and [Safety GO](GwzRemoteTransportSshAgentDesign-ReviewSafety.md) at root
`efb0d2a698755f3c1804f67495c4c9ded48e547d`, core
`a91846eb0328106cb76cc0aa90846590aafd72df`; other pins unchanged and in reports.
One dual design round; zero blocking findings; Consistency P3-1 authority wording
corrected without behavioral changes. No new build/test or capability claim.

A1 is now accepted above with fake-agent, cancellation, join, cap-exhaustion
and overrun/reap tests. A2 native signing and
A3 production integration are separate gates. The design requires exclusive
setup ownership, joined completion and a bounded supervisor retaining any cleanup
overrun; caller timeout alone never proves helper disposal. This refines the
accepted worker's bounded setup seam. Platform/source qualification remains the
deferred batch; physical capability freeze requires native proof.

## Shared SSH worker and destination routing — accepted, 2026-09-21

[Scope/results](../gwz-core/dev-docs/GwzRemoteTransportSshWorker.md). Accepted
root `60623f2a10895dd970595c5b818a68e5900a72a3`, core
`073395b5a265c4d2a60265470cd5ba173243cc64`, evidence
`5ea96433628b3bec8a8365525f8ee314d01cce99`; transport/Rust/C pins unchanged.
Retained [Code GO](GwzRemoteTransportSshWorker-ReviewCode-1.md) and
[State GO](GwzRemoteTransportSshWorker-ReviewState-1.md) close both independent
P2 findings after one merged remediation (SCP grammar and queued-timeout
classification). No blind convergence or known escaped defect. Public/source
changes: 685 lines across three new internal files plus five pump lines;
1,054 new test/support lines plus pump regression. Test ceiling refinement is
recorded in the merged remediation plan. One aggregate review plus one remediation.

All 38 local focused test executions pass, including native push/clone/push/fetch
on one authenticated connection, concurrent service failure isolation, shutdown,
identity admission and deterministic exact/past queued expiry. The ignored fake
agent child is executed by its passing parent. Raw red/green evidence and exact
source hashes live in the private ssh-integration worker-a/worker-rem-1 runs.

Next: bounded production credential/setup implementation (host trust, selected-key
proof and ambient agent cancellation), followed by per-operation authentication
observations and all-network-entry routing. The native agent read ignores session
nonblocking/timeout settings; qualify the libssh2 signing-callback and bounded
agent-client seam before using it in this worker. Backend clones and nested
with_transport scopes must preserve the shared endpoint while keeping operation
observations accurate on reuse. Production dependencies/callbacks remain inactive.
Platform and selected-source qualification remain the operator-deferred later batch.

## SSH local pool/per-remote integration accepted; qualification batched later — 2026-09-21

Operator directs postponing the outstanding platform and selected-source checks
as one later batch and continuing SSH pooling/per-remote integration now.
[Continuation scope](../gwz-core/dev-docs/GwzRemoteTransportSshIntegration.md)
records the batch, implementation order and local gates. This supersedes the
earlier qualification-as-next-action wording below. Checks are deferred, not
waived; preserve Q5 State P3-1 for correction before runner reuse in that batch.

Implemented: message-to-SSH pump, physical pool ownership and per-remote native
Git composition on controlled local fixtures. All 21 focused local tests pass;
five clone/push/fetch service channels reuse one authenticated connection across
two repositories. Retained [Code](GwzRemoteTransportSshIntegration-ReviewCode.md)
and [State](GwzRemoteTransportSshIntegration-ReviewState.md) both returned GO with
no findings after independent reruns. Accepted root
`d1273951ec5b2746f9215206440e5ffb56232293`, core
`f39a6ed260332534aee8b0cf73955803b6a5bf81`, evidence
`359d4fbf236182192f035ca8e42e3eb756c1a4ad`; frozen transport/Rust/C pins unchanged.
750 production-source lines in three internal modules, 1,144 added test/support
lines. One aggregate review, no remediation; development API/cleanup corrections
and raw evidence recorded in the continuation scope. No known escaped defect.
Next: bounded production endpoint wiring, trusted credential setup, URL/identity
resolution, worker scheduling and all-network-entry routing. The new modules
are currently compiled by the isolated fixture, not production module wiring. Preserve existing message/wire APIs,
exclusive physical ownership, consume-after-sink flow control and no client Git
subprocess fallback. Full production trust/credential setup and network-entry
coverage remain explicit work; do not advertise endpoint support prematurely.

## Git library Q5 — Windows consumers accepted, 2026-09-21

[Scope/results](../gwz-core/dev-docs/GwzGitLibraryWindowsConsumers.md).
Workspace CLI, standalone CLI, standalone core example and Python native
extension all pass independent locked Windows MSVC builds and the shared
SHA1/SHA256 native probe. Python explicit load/health also passes. Source core
`95ad5b6cca0b2692598cbf8ae3d0381567658603`; library/Rust/C pins unchanged.
Five native guards, four Mac consumer guards and both-host source preparation
guards pass. Source/link/lock/helper/artifact checks pass; all IDs match Q3.

Private runner191 lines, guards59; unchanged Q2/Q3/Q4 helpers. Evidence runs
`2026-09-21-q5-composition-a` and `2026-09-21-q5-windows-a` retain exact inputs,
instrumentation, commands and results. No failed native attempt or product
correction occurred. Retained
[Code GO](GwzGitLibraryWindowsConsumers-ReviewCode.md) and
[State GO](GwzGitLibraryWindowsConsumers-ReviewState.md) accept root
`97b01e752a6ac7709970f49daa24f017e762aebf`, core
`54a04a381272806d9f6bddd2c319294ff63bcc79`, evidence
`0c1c34b85064bff2d8e75536ce3351063f74316b`. No P0–P2 findings.
State P3-1 remains open: a post-command configuration rejection can discard
completed-command diagnostics. It fails closed and does not affect this pass;
fix and regress completion/timeout retention before runner reuse for release
judgments. One aggregate round, no remediation, no blind convergence or known
escaped defects; elapsed time not measured. Reports filed verbatim;
acceptance annotations change no executable statements. Q4+Q5 covers the five Windows
instrumented consumer shapes only. Other platforms, ordinary command/package
parity, remote-only source distribution and activation remain separate gates.
No production source/manifest/lock, publication, endpoint or fallback changed.

## Git library Q4 — bounded native Windows qualification accepted, 2026-09-21

[Scope/results](../gwz-core/dev-docs/GwzGitLibraryWindows.md). Final fixture
core `5f6cc919c880f7c540013bbfbc3bcb0fa81f2ec8`; G0 library/Rust/C pins unchanged.
Windows x86_64 MSVC Rust1.95 passed 13 Python guards (one POSIX-only skip),
nine native-binding tests, 13 G0 integration tests/seven documentation checks,
and the instrumented SHA1/SHA256 library probe. All source/lock/artifact checks
passed; IDs match Q3. MacOS native-proof and both-format probe regression pass.

Q4 fixes Windows executable-bit admission and test-only file URL construction.
Git checkout preparation retains exact symlink targets and pins LF text; bytes,
source types, links and source identities remain enforced. Raw windows-a/b/c/d
failures and final windows-e pass are retained in the private git-library
campaign. New verifier5 lines, Python tests44, Rust fixture34 added/4 replaced;
Initial Code GO; State P2-1 required Cargo ancestor config isolation.
The corrected private runner237 lines and config guard37 lines refuse both
Cargo config names through the search chain. Fresh windows-f passes every row,
with recorded before/after configuration absence; public source is unchanged.
[Remediation](GwzGitLibraryWindows-RemPlan.md): State P2-1 closed after the
original reviewer verified the counterexample and fresh native run.
[Code GO](GwzGitLibraryWindows-ReviewCode.md) and
[State closure GO](GwzGitLibraryWindows-ReviewState-1.md) accept this bounded
checkpoint at root `5d11a6c3444d4a58c774fc083db1dbbd4e621fd8`.
One aggregate round, one focused remediation; no open findings or known escaped
defects. Owner qualification found two source/test portability issues and two
checkout-preparation issues; review found the Cargo-config isolation defect.
No blind convergence; elapsed time not measured. Reports filed verbatim;
acceptance annotations change no executable statements.
Revised core `f1029847cd964a012ccec12495f897f444acdcd1`, evidence
`040e3ab4db7871350650382d01d4f7c5f62d7615`.
Initial review core `44d27ad665796cce669083bb8f03b2f194198f42`, evidence
`b9456f7a8ba328dd3a35ab7577c0e66d3464cdc9`.

No product/fork/library code, production manifest/lock selection, publication,
endpoint or fallback changed. Complete Windows consumer matrix, remaining
platforms, remote-only source distribution and activation remain separate gates.

## Git library Q3 — instrumented native execution accepted, 2026-09-21

[Scope/results](../gwz-core/dev-docs/GwzGitLibraryNativeConsumers.md): core
`468fd5e41fe369cc2892a330c8b2df9241023d38`; private evidence
`9c2daec1652f22b1c135e8205f794f38db514f37`. Same G0 native/library and Q1 CLI/Python pins.
Public fixture: 168 Rust lines; private runner 196 and parser tests 41 lines.

Stock-C 1.9.7 built and failed specifically at the known noncommit-hint fetch
(InvalidSpec -12). All five instrumented patched consumer artifacts passed
native version/vendor/features, SHA1/SHA256 object/hash/record checks, corrected
local fetch and raw class 36/code -1. Core/library downstream examples,
root/standalone CLI executables and loaded Python extension ran on macOS arm64.
All source/lock/artifact checks held. Three parser tests and fixture formatting
pass. Complete run retained with frozen inputs, exact instrumentation and hashes.

Accepted at root `9008e13262d4e24f1cbef150a3717dd79ca31a22` and the
core/evidence tuple above after retained [Code](GwzGitLibraryNativeConsumers-ReviewCode.md)
and [State](GwzGitLibraryNativeConsumers-ReviewState.md) reviews: both GO,
zero P0–P3 findings. Reports filed verbatim; both independently reran the
three parser tests. One aggregate round, no remediation rounds or known escaped
defects. The stock-C failure is the intended behavioral control, not an escaped
defect. Elapsed time not measured. Acceptance annotations change no executable
statements from the reviewed tuple. Production sources,
manifest/lock selection, native forks and public interfaces remain unchanged.
Other native platforms, full operation parity, uninstrumented package behavior,
source distribution/publication and activation remain separate gates. Next is
platform/distribution qualification; transport pump and remaining behavior lanes
still follow their existing scopes. Q3 is not a production speedup or fallback
removal. No runtime qualification failure beyond the intended control occurred.

## Git library Q2 — local evidence accepted, 2026-09-21

[Candidate scope/results](../gwz-core/dev-docs/GwzGitLibraryCandidate.md): core
`ea059ba89b1b61201b75c26708a99c7ca780d2a9`; private evidence
`c82e38394947611b3848c9e73701ac378ae2917c`. Library/fork/C and CLI/Python
pins remain those in Q1. Verifier uses portable keys; 12 Python guards pass.
14 added runner and 86 added test lines; 231-line private composition runner.

Five independent candidate graphs pass locked metadata/source/feature/lock
checks. Root/standalone CLI builds and version/help, core build plus 17
characterization tests, library build plus 13 integration/seven documentation
checks pass in local-b. Python wheel build/import/health passes in python-d.
Final runner metadata passes all five rows in metadata-e; no single all-green
build run is claimed. Raw failed attempts remain retained. Initial update
caused an unrelated edge refresh (refused); lock-preserving resolution fixed
that invocation. Python requires maturin plus root-visible source patches.

Accepted at root `7c017a5f1c3db1a743e5e1a0a62413fa5130755b` and the
core/evidence tuple above after retained [Code](GwzGitLibraryCandidate-ReviewCode.md)
and [State](GwzGitLibraryCandidate-ReviewState.md): GO, zero P0–P3 findings.
Reports filed verbatim; both independently reran the 12 guards. One aggregate
round, no remediation rounds or known escaped defects. Three invocation issues
were found during owner execution and corrected before review; no production
behavior defect or blind-convergent review finding. Elapsed time not measured.
Acceptance annotations change no executable statements from the reviewed tuple.
No production manifests,
locks, call sites, source publication, fallback or transport activation changed.
Native Windows and all-consumer identity/object-format/platform qualification,
remote-only source reconstruction and later activation remain gated. The
candidate is local composition evidence, not readiness for production.

## Git library C1/H1/Q1 — evidence accepted, 2026-09-21

[Scope](../gwz-core/dev-docs/GwzGitLibraryNextPackages.md) accepted at core
`6586768396886fe1aeb1371bbd3064377cfa70ec`, root
`3589aa05092abb7c38d90747384954565510301f`, retained Code/State GO.
One scope P2 (omitted Python graph) closed in one text-only correction.

C1: seven hermetic identity/date/message tests, 394 new test lines and one
braced test-only module declaration. H1: two member/bare attribute tests,
325 added test lines, two replaced baseline lines. Combined locked Rust1.95
characterization run: 17 passed, none ignored, including eight existing tests.
Initial fixture assumptions exposed raw index-byte change, parsed-message leading
blank-line loss, and H-INFO selection of base only; corrected tests inspect the
actual stored bytes/content and exact current/native sequences. No product fix.

Q1: [consumer/source readiness map](../gwz-core/dev-docs/GwzGitLibraryQualification.md).
Root/core/library/Python metadata passes independently. Python and standalone
CLI locks record sys 0.18.5+1.9.4, unlike root/core's 0.18.8+1.9.7; only library
uses patched local sources. Windows source-proof path keys need correction.
All consumers/platforms/distribution/activation remain separate gates.

Accepted results: core `deba48c93a04e6aaf0bab36066b12d1547d5469e`, root
`9fc664de8389ea334f36bc41135cd59448893e05`, unchanged G0 library/fork/C,
CLI and Python pins. [State C1](GwzGitLibraryEvidence-ReviewState.md) and
[Code H1/Q1](GwzGitLibraryEvidence-ReviewCode.md): GO. Both reran the
17-test suite; Code independently checked all four resolved metadata graphs.
No P0–P2 implementation findings. Owner corrected two nonblocking P3s:
stale H1 pending-run wording and overattributing first raw-index change to a
specific extension. Exact fresh per-variant administrative transitions remain
an explicit future evidence row. All reports filed verbatim.

Next: bounded native-proof Windows path-key correction and isolated consumer
composition proposal; remaining commit/tag helper and history fan-out/magic
characterization before freezing mutation/traversal APIs. C1/H1 evidence does
not close signer/filter/ref-transaction/interruption/SHA256 mutation or broader
history matrices. No production API, dependency/lock, fallback or transport
activation changed. Do not treat the G0 and C1/H1 gates as production readiness.

Metrics: one scope correction closed one State P2; one implementation review
per assigned axis, zero blocking findings, two P3 owner text corrections.
Initial run failures corrected three fixture/evidence assumptions, not product
behavior. No known escaped defects or blind convergence; elapsed time not
instrumented. Acceptance annotations and one test comment change no executable
statements from the reviewed tuple.

## gwz-git G0 — local foundation accepted, 2026-09-21

Accepted tuple: root `699c584a93185ef5e73dc96318603e25354018e2`, core
`2a5bd773df04450148c7630e01913edba2bbedb8`, library
`aa77c2ce5ad0bf6b4f4b64b2d8fd75e8547c3b4c`, Rust fork
`ce78628308e11b4e8901d5061602619109bce21a`, C
`b172e3d187a4b6866fd9f696f40a1b8e7f56d348`. Retained
[Code](GwzGitLibraryG0-ReviewCode-1.md),
[State](GwzGitLibraryG0-ReviewState-1.md) and
[Surface](GwzGitLibraryG0-ReviewSurface-1.md): GO, zero open P0–P3.
Original Code reviewer closed two P2 and one P3 in one merged remediation;
source scope amendment reviewed before implementation. Reports filed verbatim.

Local member `mem_gwz_git` provides exact repository opening, SHA1/SHA256 IDs,
owned stored commit records and owned errors. 367 source lines, 652 test lines,
11 maintained files plus lock. macOS arm64 fmt/check/clippy, 13 integration tests
and 7 documentation checks pass. Source proof before/after: 9 tests each;
archive: 9; Python guards: 10; isolated native-class/replay unit: 1. Regressions
failed on the original source before correction.
[Execution and acceptance record](../gwz-core/dev-docs/GwzGitLibraryG0.md).

Versions/features/locks and C unchanged by remediation; one local git2/sys
provider with vendored SHA256 and no network features. Fork correction changes
only `src/error.rs`; the existing per-remote binding files are unchanged.

Next: define bounded packages for L1 qualification/distribution hardening,
L3 commit/tag characterization and API design, and L4 history characterization
and API design. These can proceed with disjoint ownership; each needs its own
paths, ceiling and review checkpoint. No production dependency activation,
fallback removal, wire change or publication occurred. Other native platforms
and clean remote-only reconstruction remain gates before activation.

Metrics: one implementation review plus one merged remediation; two P2 and
one P3 discovered at implementation review, all closed, no known escaped
defects or blind convergence. Elapsed time not instrumented. Acceptance
annotations change no reviewed runtime bytes. This supersedes prior G0 next
actions below; controlling design includes the reviewed source amendment.

## Rust Git library boundary — accepted design, 2026-09-21

[Design](../gwz-core/dev-docs/GwzGitLibraryDesign.md) and
[API guide](../gwz-core/dev-docs/GwzGitLibraryApi.md) select `gwz-git` as a
separate sibling member. Core retains workspace policy and its existing backend
trait; the new library owns supported single-repository behavior. First package
G0 is a read-only repository/object-ID/commit-data foundation, 600 production
lines and 12 maintained library files. No repository was created, code moved,
dependency activated or fallback removed in this design package.

Accepted core `3efc1a79a1e5044b6e2495ed392a5c43d8f90b64`, root
`bba620ed7806628cdde26254261043eb9266b0f9`; unchanged fork
`4c1caabbce7d56426c763dd94114052302b23e4c`, C
`b172e3d187a4b6866fd9f696f40a1b8e7f56d348`.
[Code](GwzGitLibraryDesign-ReviewCode-1.md),
[State](GwzGitLibraryDesign-ReviewState-1.md),
[Surface](GwzGitLibraryDesign-ReviewSurface-1.md): GO, no open P0–P3.
[Remediation](GwzGitLibraryDesign-RemPlan.md) closed one P2 by freezing
Repository as Send, !Sync, !Clone; sequential transfer is allowed. A SHA backend
concern was dismissed after source inspection proved the pinned fix exists.

Next: provision the local sibling with `gwz repo create gwz-git`, implement G0
TDD-first within its scoped ownership/budget, verify native source and SHA1/SHA256
read/trait/concurrency behavior. G0 implementation review is Code/State plus
Surface examples. Future operation APIs need separate characterization/freezes;
L3/L4 remaining evidence and L1 type-consistency/publication hardening remain
gates. No platform parity, remote-only reconstruction, fallback removal or
production activation is claimed. This supersedes design-as-next-action below.

Metrics: one design session spanning the date boundary; elapsed time not
instrumented. Initial review plus one focused remediation; one P2 at design
review, closed by its original reviewer. No implementation-contact/settled-code
defects, no known escaped defects or blind convergence. All six reports filed
verbatim. Local documentation links and changed-range whitespace checks pass;
no code tests apply to this documentation-only package.

## Separate Rust Git library — operator direction, 2026-09-20

Keep upstream-facing libgit2/git2-rs changes minimal and retain patched 1.9.7 as
our current baseline. Put the additional Git behavior GWZ requires in a separate,
long-lived Rust library above git2-rs. New upstream capabilities may simplify
that library later; adopting main is not a prerequisite to its implementation.
[Direction record](../gwz-core/dev-docs/GwzNoFallbackPlan.md) names the boundaries.

Next: design the library's scope/API, ownership, package placement and migration
of the remaining no-fallback lanes, then review the shared interface and budgets.
This supersedes the older “assess main first” next action below and in NativeFix.
It records operator intent only; no new interface freeze, member provisioning,
code movement, dependency activation or publication is claimed. Accepted N1/N2
and all compatibility, qualification and activation gates remain in force.

## Native correction and isolated Rust integration — accepted 2026-09-20

N1/N2 accepted after retained Code, State and Surface GO on one exact tuple:
root `62c2f122c28ededdefb7af32058f4b058b018dcb`, core
`01d6f6624472620c215693243f7ac3865aeb31a4`, Rust fork
`4c1caabbce7d56426c763dd94114052302b23e4c`, C 1.9.7 backport
`b172e3d187a4b6866fd9f696f40a1b8e7f56d348`, upstream-facing C patch
`fe618d0f5de9e506b9714643afc42d2fcba6e984`.
[Scope and evidence](../gwz-core/dev-docs/GwzNoFallbackNativeFix.md),
[Code](GwzNoFallbackNativeFix-ReviewCode.md),
[State](GwzNoFallbackNativeFix-ReviewState.md),
[Surface](GwzNoFallbackNativeFix-ReviewSurface.md).

One native condition corrected; identical 243-line regression additions on main
and 1.9.7. Both focused and full offline C suites pass. Rust/sys now composes the
accepted binding with exact published sys 0.18.8+1.9.7 sources and the patched C
submodule. Both isolated Rust modes pass eight tests; ten Python guards pass.
Surface independently reran both Rust modes. Local macOS arm64 evidence only.
All production manifests, fallback paths and transport activation remain unchanged.
Both forks are unpublished; remote-only reproduction is not yet available.

Metrics: one implementation session; wall-clock not instrumented. Design: one
initial review plus one focused remediation, closing one P2 overstatement and
one nonblocking P3 test-oracle correction. Acceptance: one round, zero findings,
zero remediation rounds, no known escaped defects. Reports are filed verbatim.

Next: assess main's new commit/signing and path-history APIs before implementing
remaining lanes; main and 1.9.7 are divergent lines with significant API changes,
so no upgrade is implied. Preserve existing publication, all-consumer/platform,
and production-dependency activation gates. Existing ENOTFOUND ambiguity for
missing/mismatched tag targets is explicitly characterized; type-consistency
hardening remains a prerequisite to the fallback-removal decision. Cancellation
and broader publication semantics remain under the accepted no-fallback plan.

## Native C fork registered — 2026-09-20

Operator created `owebeeone/libgit2` following the agreed sibling-member layout.
Registered through `gwz repo clone git@github.com:owebeeone/libgit2.git libgit2`
as `mem_libgit2`, path `libgit2`, origin the operator's fork. Initial main HEAD:
`0551dfd4ad989b6a3d5683c0d4cf326c6efef929`. The faulty local-fetch comparison
remains present. Qualified native v1.9.7 commit
`49e408b3208bc3093757a1c2db938d3590f3f412` is available in this checkout.

This is provisioning only; no source, Rust dependency, submodule pin, upstream
PR or production activation change. Existing git2 candidate stays at `e883be38`.
Next package must scope/review the upstream C regression/fix and 1.9.7 backport,
then align the Rust fork's in-tree sys package to qualified 0.18.8+1.9.7 before
connecting its C submodule to the patched fork commit. Upstream-facing work and
GWZ's pinned-version backport remain separate branches. The existing first-
package acceptance and remaining no-fallback gates below remain authoritative.

## No-fallback implementation resumed — 2026-09-20

Operator "go" resumes the accepted four-lane plan. P0 cloned and registered
`git@github.com:owebeeone/git2-rs.git` as `mem_git2_rs`, path `git2-rs`, through
`gwz repo clone`. Initial fork HEAD:
`f42a01267a3042b26d30e9d8acf286c6c739bd8a`. The fork's path sys dependency is
1.9.6 while current qualification uses 1.9.7; do not activate the cloned HEAD.
[Preparation baseline](../gwz-core/dev-docs/GwzNoFallbackPreparation.md) records
exact published Rust/sys source identities, consumer features and platform gates.

P2 accepted after retained Code/State GO on core
`dd47810ece5980cfa35017ae0dfd7a8f33701e80`, root
`d98922e03b837d030477f1d9a696fef2464b8b17`; unchanged fork/transport/taut
pins are recorded in the filed reports. One P2 review-entry correction closed;
no open findings. [Checkpoint](../gwz-core/dev-docs/GwzNoFallbackCheckpoint.md)
freezes exact paths, retained contracts and numeric first-package budgets.
Code/State reports are `GwzNoFallbackCheckpoint-Review{Code,State}.md`.

L2-A is accepted as an unpublished external-consumption candidate at core
`5eb29f073a536901b23f96c3d4b1d05ac59ac01c`, fork
`e883be38abeb845a776d5e0a8c9bbf5ef8e0bc68`, root
`a02f91f66b4efe483e44c3fa4e85dfb6d94c19e0`. Retained Code/State GO after one
P2 source-admission correction; Surface GO on unchanged public interface.
Seven Python guards and seven native tests in each source/archive mode pass.
No GWZ production dependency, native pin, protocol, package publication or
runtime activation changed. Original fork main is preserved; candidate lives
on `codex/per-remote-transport` from the qualified git2 0.21.0 release.

L1-A, L3-A and L4-A characterization is accepted at core
`c63f497df29d51ad5d864738fa0056b513c3ab7d`, root
`9592320ddf95fe53eef0619398b1f8170943a4cc`, after recorded State/State/Code GO.
Reports: `GwzNoFallbackCharacterization-Review{State,Code}.md`. Eleven focused
tests pass across the three packages, including clean L3 child executions.
Two nonblocking P3 owner corrections: L4 documentation whitespace; L3 explicit
post-rejection staged-blob assertion and rerun. No additional blocking review
round. No known escaped defects; no runtime replacement was activated.

Findings: native local-fetch failure is reproduced when receiver tree refs name
objects present at the source and a new commit forces negotiation. The broader
25-row unrelated-object matrix passes. Commit/tag hooks and effective signing
configuration require orchestration. Path-history attribute magic currently
loses worktree context; ordered range/first-parent fixtures match native Git.

Next: separately scope and review the local native-fix package, commit/tag
identity/signing orchestration and history traversal/pathspec replacement.
Complete binding packaging, all-consumer/features/native platforms before
separate production activation. First packages are complete; the full
no-fallback program and transport activation remain in progress. Inventory
work did not expand the product's command/options scope.

## No-fallback plan acceptance — 2026-09-20

Status: **plan accepted after retained Consistency / Safety GO; implementation
paused by the operator**. This supersedes earlier transport checkpoint wording
that directs immediate continuation: the transport implementation is paused
while the dependency/no-fallback work is planned. No runtime work resumed.

Reviewed core: `bf9446762a7c51358679ed04e147aa51dedfb2bc`; root review inputs:
`57a0aba0a808417cb4c72a796ddb8926ce85179b`; unchanged transport
`28f5afb3938a2aa8af0e1e8d5b07779add6ab776` and taut
`733e8a78897a90f017f4726e4331aed95e8cb977`.
[Plan](../gwz-core/dev-docs/GwzNoFallbackPlan.md),
[Consistency closure](GwzNoFallbackPlan-ReviewConsistency-1.md),
[Safety re-verdict](GwzNoFallbackPlan-ReviewSafety-1.md), and
[merged remediation](GwzNoFallbackPlan-RemPlan.md).

Four lanes: local-fetch investigation/replacement; per-remote Rust API;
commit/tag orchestration; path-filtered history. Preparation registers the
existing `owebeeone/git2-rs` fork through GWZ and establishes compatibility.
A reviewed P2 checkpoint must freeze shared ownership, interfaces, numeric
package ceilings and review tiers before lane implementation. Lane 2 first
qualifies isolated consumption; an all-consumer/platform/package gate and a
separate review precede any production dependency switch. Actual SSH/pool
activation remains governed by the transport program.

Review record: two peer-blind axes reused at the operator's request, followed
by one merged remediation and focused re-verdicts on the same revision. Two P2
findings discovered during plan review, both closed; zero open P0–P3, no blind
convergence and no known escaped defects. One review session; elapsed time not
instrumented. Link and whitespace checks pass; no code tests apply. Reports
are filed verbatim. Acceptance annotations change no plan-body semantics.

Next, once the operator resumes work: P0/P1 preparation and design, then the
reviewed P2 ownership/scope checkpoint, then independent lane implementation.
No fork clone, workspace membership change, source change, dependency activation,
publication, tag or push was performed in this planning/review cycle.

## Remote transport Phase 3c — nonblocking SSH channel, 2026-09-20

Accepted implementation: `f03f5f79bae73d378e575273af0b9ed2a87c052d`.
Current core documentation descendant: `6081880420a54e0b82e1e33c63194e2664f3d148`.
Initial candidate: `b77f4fef5dbb6958789dd8160fbb74eb67a3f47e`.
Status: **accepted preactivation primitive after Code / State / Surface GO**. Continue the authorized adapter work after
the accepted foundation. Core `GwzRemoteTransportSshChannel.md` scopes ownership
of an already trusted/authenticated nonblocking ssh2 Session through command
open, simultaneous stdout/stderr and request I/O, EOF, close and wait-close.
Only complete cleanup permits session extraction for reuse. A standalone local
SSH fixture uses temporary keys/server/repository, never user SSH configuration.
Budgets: 250 primitive source lines, 350 fixture lines. Preactivation module only;
production connect/authentication/pool/message pumping and native parity remain
unclaimed. Original Code/State and Surface reviews are complete.
The initial 221-line primitive and 311-line fixture passed two real loopback tests on
macOS arm64 with OpenSSH10.3p1 and ssh2 0.9.6. Two receive-pack command channels
reuse one authenticated session; upload-pack, early extraction refusal, active
abort and shell quoting also pass. The fixture independently verifies its own
known host before authentication. Missing sshd is a gate failure, not a skip.
Compile-red (missing source module) preceded implementation; behavioral tests
were completed against the candidate, not all written before it. No throughput,
pack transfer or production trust/credential parity is claimed.

Correction core: `f03f5f79bae73d378e575273af0b9ed2a87c052d`.
Original Code and State independently reported NO-GO for nonblocking native
cleanup losing EAGAIN; Surface GO. Reports are filed verbatim. Lane owner found
one additional P2: native channel flush discards unread incoming bytes. The
merged RemPlan corrects both: socket-owning connection lifetime, retryable and
forced disposal, plus nondestructive Write::flush. Five native tests pass,
including exact advertisement preservation and twice-blocked cleanup followed
by forced termination. The fixture guard resumes stopped owned processes during
unwind. Original reviewers closed findings at root
`6076c6153f2b4fb74da5179b0ec6ffd2765d81ad`, core correction above, unchanged
transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` and taut
`733e8a78897a90f017f4726e4331aed95e8cb977`. Reports filed verbatim as
`GwzRemoteTransportSshChannel-Review{Code,State,Surface}-1.md`.
One three-axis review plus one merged correction/re-review; two distinct P2
root causes found before acceptance, including one blind Code/State convergence;
zero open findings and zero known post-acceptance escapes. One continued work
session; wall time not instrumented. Preactivation source: 323 lines in two files.
Five native tests and formatting/diff gates pass. No production activation.

Next: host message-to-SSH pump behind existing transport API. Keep bounded
incoming byte mirrors, validate before forwarding, and consume Stream.read only
after SSH accepts that prefix so Window/Flushed cannot get ahead of sink progress.
Keep reverse data/control pumping independently, drain stderr, preserve EOF/close
ordering, discard pending mirrors on cancellation, and classify actual backend
I/O separately from backpressure. Proposed slice: private ssh_pump module around
250–350 lines plus 300–400 lines of deterministic partial-I/O and clock tests.
No gwz-transport wire/API change or CLI/core communication change is needed for
that slice. Physical pool resource driver, production identity/trust setup, per-
remote Git integration, native parity and activation coverage remain subsequent
Phase 3 work; this checkpoint does not advertise endpoint support.

## Remote transport Phase 3b — adapter foundation, 2026-09-20

Status: **accepted foundation after Code / State / Surface GO**. Operator "Go" authorizes dependency and
adapter work. Core `dev-docs/GwzRemoteTransportAdapterFoundation.md` scopes a
local versioned git2 fork candidate and the blocking std::io bridge. Distribution
is not publication; production manifests remain stock until activation gates.
Bridge code remains preactivation, compiled by the isolated archive consumer.
Budgets: 160 packaging runner lines, 150 bridge lines, 300 test lines. Original
Code/State and Surface reviewers qualify the settled result. Host SSH worker,
trust/authentication parity, pool lifecycle and native platforms remain next.
Core candidate `46bbc932ac25d9b1762c77351293ea1c0ac7dcbb` adds a 108-line
blocking adapter and a pinned local `gwz-git2` package recipe. The package archive
has SHA-256 `2c0544413ee18231fb9185ad29cb82ffa34c223245cd044523be1515897f68af`.
Seven native tests pass against both staging and the package; 21 archive-backed
consumer tests pass, including three new blocking cases. Three packaging,
two native provenance and nine archive-admission Python tests and formatting
pass. Source-module missing-file compile red preceded implementation. Only the
core fixture compiles the bridge so far; no active core/CLI dependency or
behavior changed. No SSH implementation, registry release or native parity is
claimed.
All three original reviewers returned GO with zero P0–P3 on root
`687e2d3c21a5fca0cfff81216eff4bdac9f855ca` and the core candidate above;
transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776` and taut
`733e8a78897a90f017f4726e4331aed95e8cb977` remain unchanged. Reports are filed
verbatim as `GwzRemoteTransportAdapterFoundation-Review{Code,State,Surface}.md`.
One three-axis review, zero remediation rounds, no blocking findings, no blind
convergence or known escapes. Current core documentation descendant:
`60daef8ca7a471d3e4d1acfd653678676ae1fef9`. Next implementation is the endpoint
SSH session owner and trust/authentication/pool integration. The local fork is a
qualified distribution option, not an approved or completed registry release.

## Remote transport Phase 3a — native binding qualification, 2026-09-20

Status: **accepted local prerequisite after Code / State / Surface GO; no production activation**. Operator "go"
authorizes the next Phase 3 prerequisite. Baseline root `d86c7d079b524537f6cdfdc5352b930a0202389b`,
core `9303eb86914aa5770b4f951613270b14b2108f73`; accepted transport unchanged.
`gwz-core/dev-docs/GwzRemoteTransportNativeBinding.md` defines the bounded
safe binding extension and qualification. Keep production dependency/lockfiles
unchanged; retain a pinned two-file git2 patch and isolated public test runner.
Qualify owned context, error/panic, fresh versus retained remotes, stateful
service continuity and unrelated transport coexistence before host SSH work.
No physical message carrier or new CLI/core API. Budgets: 150 production patch
LOC, 550 Rust test LOC, 200 runner LOC plus concise docs. Retained economical
drafter owns tests; lane owner patch/runner/docs. Exact-tuple Code/State review
uses the original reviewers; add Surface for the new public safe binding method.
Core candidate `fe815856291a93fa4ecdf0ab5879984d7b5ba1ee` adds the
72-line binding patch, 569 lines of Rust integration fixtures (within the 20%
allowance), and 106-line isolated runner. Seven integration tests, two Python
provenance tests and formatting pass on Rust 1.95/macOS. The stock binding
provided the expected missing-method compile red. No production dependencies
changed. Reviews are filed verbatim as
`GwzRemoteTransportNativeBinding-Review{Code,State,Surface}.md`. Code and State
found no P0–P3; Surface found only P3-1, the unstated toolchain default. Core
`7b03091941f047c61f8261fd12c451bca6db49d7` corrects help/README only; the reviewed
binding patch, pins and Rust fixtures are identical. The original Surface
reviewer verified that correction and closed P3-1 in `-ReviewSurface-1.md`.
Current core documentation descendant: `30616010f23a7e4ae7dc96b03268a3d950dfe215`.
Reviewed implementation tuple: core `fe815856291a93fa4ecdf0ab5879984d7b5ba1ee`,
root `d7b1b04d35acd73dd85ec553a68d4e498c120f21`, transport
`28f5afb3938a2aa8af0e1e8d5b07779add6ab776`, taut
`733e8a78897a90f017f4726e4331aed95e8cb977`; help correction review root
`d798d201b4d9d0e8f02d55b415efd3c725bfee3a`.
One three-axis review and one focused Surface closure; zero blocking findings,
one pre-acceptance P3 corrected, no blind convergence or known production
escapes. The binding adds 72 lines across two upstream files. Final runner is
107 lines; no new production runtime owner. No remote CI or native parity is
claimed. Next: distributable binding dependency selection and endpoint SSH
adapter integration with the accepted stream/pool API, followed by native
trust/authentication and complete network-entry qualification. Existing CLI/core
communication interfaces remain unchanged.


## Remote transport Phase 1/2 interface acceptance — 2026-09-19

Status: **accepted and frozen after Code / State / Surface GO**.
Accepted implementation: transport `28f5afb3938a2aa8af0e1e8d5b07779add6ab776`,
core `ace269896ad80aee923e2e8fd31e565c43de57ed`, unchanged taut
`733e8a78897a90f017f4726e4331aed95e8cb977`; root review inputs
`9d0dc7ef5c616d64d52c296ea2fa34d83d21d73e`.
The [interface checkpoint](GwzRemoteTransportInterfaces-Checkpoint.md) pins
archive provenance, contracts, qualification and all three re-verdicts.
Current core documentation descendant: `9303eb86914aa5770b4f951613270b14b2108f73`.
The acceptance commits update documentation only; source and tests remain the
reviewed implementation. Phase 1 schema/types, negotiated admission and message
handoff, and Phase 2 stream/pool runtime API are frozen within that scope.

The initial review found one P2 (Open admission silently using default limits)
and three P3s (host timeout-policy evidence, clock-only wake amplification,
consumer setup instructions). One merged remediation closed all four. The same
Code and State reviewers and the Surface reviewer verified their closures and
returned GO with no new findings. Reports are filed verbatim. Two review rounds,
one remediation; four findings discovered before acceptance, no blind convergence
on the blocking root and no known production escapes. Acceptance finished in the
resumed 2026-09-19 task; exact elapsed time across prior interruptions was not
instrumented.

Qualification: 89 owner tests pass on Rust 1.95, including normal seeded stream
and pool replay; 18 isolated consumer tests pass against the committed package
archive; owner and exact-source consumer regeneration, formatting and tooling
checks pass. Fresh Python setup and archive proof were independently reproduced
by Surface. The two opt-in extended campaigns were not rerun for this gate.

Next: Phase 3 safe per-remote git2 callback/owned-context qualification, followed
by the host SSH adapter and native evidence. No physical carrier belongs in
transport. Production host dispatch/timers, placement, HTTPS, native-platform
qualification, registry resolution and remote CI remain later gates. The local
owner CI workflow is prepared; no remote execution, publication, push or remote
provisioning is claimed. Existing CLI/core service interfaces remain unchanged.

## Remote transport shared-schema integration — 2026-09-19

Status: **accepted after original Code and State reviewers both reported GO**.
This accepts the shared-schema integration implementation and draft pool host
contract only; it does not freeze the Phase 1 schema or Phase 2 runtime API.

The [integration checkpoint](GwzRemoteTransportIntegration-Checkpoint.md) records
the exact reviewed tuple: taut `733e8a78897a90f017f4726e4331aed95e8cb977`, core
`435e936b593476f24fad4cc4e70f5d06b784ed7d`, transport
`e8b9a1c5408cc9ea9528939b3a602acbeb697814` and workspace review inputs
`23617273932031a346c6fd772e1df99fd68e2706`.

Core's test consumer composes the exported schema and uses native transport
Rust types and the owner's CBOR runtime. Generation pins the canonical source
checkout, imported modules, owner schema and formatter; normal Cargo builds use
checked-in output. The isolated archive proof verifies package/digest/revision
metadata without a sibling checkout. Typed and encoded exchanges preserve data
and wait for endpoint cleanup before close. Three host fixtures qualify clock
origin, earlier deadlines and final Pool-owner shutdown. All 40 tooling tests,
nine isolated consumer tests, regeneration and formatting pass; the unchanged
transport also passed its normal 66-test suite in this run.

The initial dual review found four P2 roots: incompatible generator/runtime
options, imported-source provenance, clock-origin/timer duties and final Pool
ownership. One [merged remediation](GwzRemoteTransportIntegration-RemPlan.md)
corrected all four. The original [Code](GwzRemoteTransportIntegration-ReviewCode-1.md)
and [State](GwzRemoteTransportIntegration-ReviewState-1.md) reviewers verified
closure and reported no new findings. Two completed rounds, one remediation,
no blind convergence or known production escapes. Reports are filed verbatim.

Next: complete the remaining Phase 1 message/admission and encoded-contract
proofs plus CI drift wiring, and Phase 2 active-I/O clock semantics, before the
named interface freezes (including Surface review). The
[pool host contract](../gwz-core/dev-docs/GwzRemoteTransportPool-InterfaceGate.md)
remains a draft. No production CLI/core surface, physical carrier, adapter,
registry publication or native-platform qualification is established here.

## Remote transport draft review — 2026-09-19

Status: **accepted at gwz-core `05842b38e55f109ed3663555680751811a72eb9b`
after [Consistency round 2](GwzRemoteTransportDesign-ReviewConsistency-2.md)
and [Safety round 2](GwzRemoteTransportDesign-ReviewSafety-2.md) reported GO;
this accepts the four-document design draft for implementation planning only**.
Root prior-round inputs: `3a0b8fa975bc013673ee919b087f69da9f3853ff`.
The [review checkpoint](GwzRemoteTransportDesign-ReviewCheckpoint.md) records
the full tuple, verification and routing override.

The original reviewers closed all six findings (one P1, five P2) in one merged
remediation and found no new issues in changed-range interactions. Operator:
**"use the old reviewers"**. The fresh pair was stopped without final verdicts;
its unfinished output was not used. Two completed draft review rounds, one of
two permitted remediation rounds used, all defects discovered before
implementation; escaped defects not applicable. No schema/API freeze,
implementation acceptance, platform evidence or release is claimed.
The [implementation plan](../gwz-core/dev-docs/GwzRemoteTransportPlan.md) is now
accepted at the planning stage: first establish the independent gwz-transport package,
exported schema/shared generated types and bidirectional taut carrier proof.
The operator subsequently authorized implementation. Phase 1 is in progress:
local gwz-transport member, generated schema/types, external Rust type generation
and a test-only core consumer. No production CLI–core communication API changes.
Source protocol work and archive moves in other lanes remain outside this review.

Plan review update: [G46](../gwz-core/dev-docs/GwzRemoteTransportPlanReview-G46.md)
returned combined draft-stage **NO-GO** (four P2, three P3). One documentation
revision addresses all seven, mapped in the
[plan remediation record](../gwz-core/dev-docs/GwzRemoteTransportPlan-RemPlan.md).
The same reviewer's [re-verdict](../gwz-core/dev-docs/GwzRemoteTransportPlanReview-G46-1.md)
is **GO**, closing all seven findings with no new findings on plan SHA-256
`55120dd1af7b77818eb71fda609818b6c1bb2539a08ec9f025ab01f7d4899b99`.
Two combined draft-stage rounds, one merged remediation; not a dual peer-blind
gate. Implementation then began. Operator clarification: the CLI–core communication
layer is handled elsewhere and its current interface must not change. The custom
four-byte framing proposal at core `914a4406998856abce0d980372b41635f7c52940`
is superseded; its framing adapter and process fixture were removed. Continue
message/schema work against the supplied interface. Earlier GO reports remain
historical evidence, not approval of the current unfinished implementation.
Further operator clarification: emulate streams through discrete messages using
asynchronous send/receive. Prefer optional fields on existing taut messages and
existing request ids; no new CLI command or core service surface is required.

The operator next prioritized the in-memory stream implementation and seeded
Monte Carlo tests, with no physical transport in gwz-transport. The
[memory checkpoint](../gwz-core/dev-docs/GwzRemoteTransportMemoryImplementation.md)
is accepted at transport `aa9ecae65d6c0d568c5f4d738f9930d49f684f56`
after [Code](GwzRemoteTransportMemory-ReviewCode-1.md) and
[State](GwzRemoteTransportMemory-ReviewState-1.md) GO from the original reviewers.
It includes the active-stream machine, executor-independent async facade,
bounded credit/buffering, lifecycle tests and deterministic randomized replay.
The initial Code/State gate returned NO-GO with five P2 findings (four distinct
roots; both axes found erased failure detail) and one P3. The
[merged remediation](GwzRemoteTransportMemory-RemPlan.md) corrects negotiated
admission, authentication combinations, structured failures and dispatcher
capacity, with depth/accounting parity and packaging fixes. The original
reviewers verified every closure and found no new findings. Two completed
review rounds, one remediation round; acceptance is limited to the in-memory
checkpoint. The [acceptance record](GwzRemoteTransportMemory-Checkpoint.md)
contains the exact reviewed tuple and test/replay evidence.
The source-schema generator extension and core consumer remain separate pending
work; neither Phase 1 nor Phase 2 is declared complete or frozen.


The operator authorized connection pooling next. The
[pool checkpoint](../gwz-core/dev-docs/GwzRemoteTransportPoolImplementation.md)
is implemented with fake connections, a controlled clock and seeded lifecycle
schedules. Its dual Code/State implementation review uses the original reviewers;
the initial gate returned three P2 findings (two Code, one State). One
[merged remediation](GwzRemoteTransportPool-RemPlan.md) corrects spontaneous
idle disposal, session-scoped cancellation and cross-port user/host capacity.
The original [Code](GwzRemoteTransportPool-ReviewCode-1.md) and
[State](GwzRemoteTransportPool-ReviewState-1.md) reviewers both returned GO,
verified all closures and found no new issues. The pool checkpoint is accepted
at transport `e8b9a1c5408cc9ea9528939b3a602acbeb697814`; the
[acceptance record](GwzRemoteTransportPool-Checkpoint.md) pins the full tuple,
66-test suite, 50,000-case campaign and independent review evidence. Two completed
rounds, one remediation; the usage-limit interruption yielded no verdict and
was resumed on the same tuple. No pool API freeze or network integration is
claimed. Scope includes exclusive
leases, shared limits, identity eligibility, idle expiry, bounded waiting,
cancellation, helper time and cleanup acknowledgements. All physical I/O remains
host-owned; the existing CLI/core interface is unchanged.

Date: 2026-08-22 (resumed)
Status: **the single current-state authority for the GWZ merge program.
Update at every checkpoint boundary; keep concise; history belongs in git,
not in this file. Live status paragraphs in other documents are superseded
by this file (rulebook §7.3).**

## RESUMED 2026-08-22 — parallel execution, refined tier economy

Operator resumed 2026-08-22: "proceed now with parallelization the
focus (reduce calendar cost) but also optimize model use so we use
fable token only where it makes a difference." Recorded consequences:

- **BURN-DOWN WINDOW CLOSED (operator, 2026-08-22, ~10% Fable
  remaining: "prolong by reducing Fable usage").** Interior reviews
  revert to Opus single-axis effective immediately (Steps 2.3/2.4
  review on Opus); the only remaining Fable spends this week are the
  in-flight ARM diagnosis and D3's two focused re-verdicts; Phase 5
  dual, R4b-G, and the A1 activation review hold for the quota
  reset; lane-owner operation minimized (delegation-first, batched
  bookkeeping). The window note below is historical.
- **QUOTA BURN-DOWN WINDOW (operator, 2026-08-22, ~17h until the
  weekly reset): "use the remaining quota".** For this window only:
  interior single-axis reviews (Phase 2 steps) run on the FABLE tier
  instead of Opus; the deferred-by-economy ARM64 EBADF substrate
  package (Fable-tier deep diagnosis) is launched; Phase 2 Steps
  2.1 and 2.2 launched in parallel (Opus implementation, per the
  plan's own §4.4 pipelining — legal behind the Phase 1 settle
  in-review). At quota reset the tier policy below resumes
  unchanged. Concurrent lanes at window open: Phase 1 settle dual
  (Fable ×2), D3 cursor (Opus), Steps 2.1 + 2.2 (Opus ×2), ARM64
  EBADF (Fable). Fixture-collision rule in force: 2.1 owns the
  `durable_leaf.*` region and 2.2 the `namespace.*` region of
  fault_expected_keys.rs; per-lane scratch CARGO_TARGET_DIRs
  mandatory; commits serialize through the lane owner.
- **Refined tier policy (supersedes the 2026-08-16 economy policy's
  reviewer line; suspended only as stated in the window note
  above):** Fable ONLY at the program-level dual gates —
  R2-D's three duals, the M5b dual, R4b-G, the A1 activation review,
  this amendment-tier work — plus escalations and deep diagnosis.
  Interior single-axis reviews (R2-D Phases 2-4 steps) run on the
  Opus tier with automatic escalation to a Fable pass on any P0/P1/P2
  finding. Drafting/implementation stays Opus; mechanical stays
  Sonnet.
- **Parallel structure:** 2-3 lanes + reviews shadowed by the next
  package (the §4.4 pipeline rule). Wave 1 launched 2026-08-22:
  (i) Phase 0 dual review (Fable ×2, Code+State, peer-blind) on the
  parked `d32b2c9`; (ii) Phase 1 Step 1.1 failing tests (Opus)
  shadowing it; (iii) M5b implementation (Opus) on its own subtree;
  (iv) thin-A1 round-2 remediation applied inline — **and ACCEPTED
  2026-08-22 at GO/GO** (Consistency GO / Safety GO focused
  re-verdicts, both appended to the report files; every round-1
  P0-P2 resolved in one merged round; the banner pass — RemPlan-4
  supersession banner, M5b dated annotation, EvidenceInventory
  re-frame, and the Refactor/R1 naming sentence — landed with
  acceptance; `GwzM5-8ThinA1Amendment.md` is now the controlling
  gate-chain text).
  **Phase 0 freeze ACCEPTED 2026-08-22 at GO/GO** — dual #1 of 3
  consumed (Code GO after the round-2 [P1-3] pin catch at `c40e712`;
  State GO with C-3 ruled real/Phase-1-owned/not-Track-W and the
  §3.1 pin verified). `GwzM5-8R2DInterfaceFreeze.md` is FROZEN: five
  seams, two extension classes (C-2 recheck arms, C-3 observer
  grammar) with per-phase assignments, §3.1 persisted-home pin — all
  binding on Phases 1-5. Tier recording per plan §7.6: done (this
  section + the thin-A1 section). Freeze train PUSHED to origin
  through `c40e712`; the M5b park (`3e60529`) stays local pending
  its own dual. Tracked: both Track-P spike cases green on the next
  full Windows matrix run (dispatched at push). Wave 2 LAUNCHED:
  Phase 1 Step 1.2 driver (Opus; makes the six red admission tests
  green, implements C-2 arms + C-3 observer grammar under the frozen
  rules) and the D3 durable-cursor implementation lane (Opus).

  **M5b package DELIVERED and PARKED pre-review** (2026-08-22): the
  no-ff semantics installation per the frozen design — 21 new green
  suites across 6 `cfg(test)` files, 1,424 test lines, **0 production
  lines** (§8.3 hard ceiling holds), fmt/clippy green, both T-6
  suites green and their file byte-identical to HEAD, full lib suite
  green in partitions (1,399 tests; the only failures are the Phase-1
  lane's 6 red-by-design admission tests). **Freeze-ledger decision
  (lane owner, per §8.3's return-to-the-freeze rule, subject to the
  M5b dual review):** the package's test-line ceiling is raised
  1,200 → 1,450 for this package only — every line maps to a
  design-named §6/§7 obligation and ~140 lines are shared harness;
  the 0-production-line and ≤10-file ceilings are unchanged.
  **Stopped and tracked (not M5b's to fix):** service-level
  abandonment of a NotStarted frozen action is unreachable
  MODE-BLIND (pre-existing reducer guard at
  `transition/reduce/participant.rs:105-119`; Normal-mode control
  probe fails identically) — routed to the R4b acceptance-debt
  surface at the A1 activation review (thin-A1 amendment §1);
  the abandonment suites are written at the authority/reducer level
  per tree precedent.

  **M5b ACCEPTED 2026-08-22 at GO/GO — round-1 clean on both axes**
  (the program's first): Code GO (0 P0/P1, 1 P2, 3 P3; zero-production
  proven via diff surface + cfg chain + non-test dep-info; all 21
  suites mapped to design obligations; the stopped abandonment item
  independently re-probed — modes fail identically, routing ruled
  correct) and State GO (0 P0/P1, 1 P2, 5 P3; determinism, reverse,
  composition, mode isolation verified; ceiling raise ratified).
  Acceptance train dispositions: both P2s closed — the T-3 second
  scan (`no_ff_mode_mentions_stay_inside_the_pinned_surface`, 16-file
  NoFf surface pin with the pre-A1 subsumption argument) and the
  push-lane CI step running the tripwire scans in the boundary
  workflow (also discharges Code P3 on T-4's CI scan); M5b-W1 applied
  to `GwzM5-8I2RecordContract.md` §7 (dated banner, document-only);
  the §9-Q7 class-membership sentence recorded in the classification
  ledger; the ChangeBudget M5b row filed (test-ceiling raise 1,450
  granted+ratified). Clean-tree re-cut: satisfied by both reviewers'
  pristine-extraction gate runs at `3e60529` (Code: full lib
  1,391/0/1, boundary checker ok; State: 532/0 merge partition,
  21/21 suites, T-6 by name). T-5 remains the retained-reader lane's
  per the design's own assignment. Remaining State/Code P3s
  (FfOnly durable assertion, inert tree assertion, F-2 cross-mode
  bytes, Q5 partiality note) are file-and-continue, listed in the
  reports. **M5b's A1 proof obligations (thin A1 leg two) are now
  standing: T-6 + the accepted package.** Pushed with this train.

  **R2-D Phase 1 Steps 1.1+1.2 PARKED pre-settle** (2026-08-22): the
  physical admission driver landed green — all six Step-1.1
  acceptance tests pass UNEDITED on first run (byte-untouched,
  verified); `checked_artifact::` 267/0 (257 pre-existing + 6
  acceptance + 4 new unit); Track-P spike still green; no wire
  change (`protocol/generated.rs` untouched; no new slot, record,
  purpose, or phase); C-2 arms and C-3 observer grammar implemented
  exactly per the frozen §4.4 classes with the memo-clause map in
  the driver report. Production LOC ~768 code (+1051 raw) vs the
  aspirational <500 — lane-owner disposition: accepted for the
  settle review, structural cause being the round-2 freeze assigning
  BOTH extension classes to Phase 1 after the plan's budget was set;
  no ChangeBudget ceiling exists for R2-D and no §16-22 stop rule
  fires. THREE judgment calls explicitly queued for the Phase-1 dual
  settle (dual #2): (i) visibility widening of the R1 classifier
  types to `pub(in crate::checked_artifact)` (seams unchanged, pin
  test green); (ii) E4 install as retire-then-publish over the
  no-replace primitive with all three crash windows proven
  convergent — re-spike if the reviewer reads E4 as requiring an
  atomic replacing rename; (iii) absent-active ≡ idle semantics.
  **Steps 2.1 + 2.2 LANDED 2026-08-22 (one combined train).** 2.1
  (LeafObserver production impl, durable_leaf 11/11 executed): Code
  single-axis round-1 NO-GO on one real P1 — sync_all on the
  read-only handle breaks E9 on Windows — fixed with a documented
  platform-routed arm + non-vacuous os-error-5 canary; round-2 GO;
  escalation State axis GO (0 P0/P1; its P2 discharged by the freeze
  §4.3 E9 annotation landed with this train: writer-class-conditional
  durability, per-platform ExactDurable meaning, MissingDurable
  negative space — binding on Step 2.4's caller). 2.2 (namespace
  backend, namespace 11/11 executed): State single-axis GO; its
  auto-escalation trigger was a P2 that IS the landing commit's own
  checker counts entry — lane-owner adjudication: discharged at
  landing, second-axis scrutiny folds into the Phase 5 settled dual
  per the three-dual cap; repeatability-taxonomy comment corrected
  at landing per its P3-1. Both tree pins + the counts entry
  validated GREEN on a pristine overlay before commit. Phase 2
  remaining: Steps 2.3 + 2.4 (launched on landing).

  **Steps 2.3 + 2.4 LANDED 2026-08-22 (one combined train, gwz-core
  `c2564ba`).** 2.3 (managed backend ops, E15/E16 real): State
  single-axis round-1 NO-GO (1 P1 — the checker counts entry, a
  verbatim repeat of 2.2's finding; 3 P2, 6 P3); round-2 GO with all
  ten closed — §3.5 managed_bootstrap activation filed (8/30
  PartiallyExecuted, 22 proved siteless), 4×2 repeat taxonomy with a
  partition assertion, and the E16 cross-parent atomicity record
  (Branch A: wedge proved unreachable — the rename is the commit
  point, the three post-crash states all carry green matrix rows,
  EXDEV and foreign removal refuse typed; the reviewer ruled Branch A
  correct on the merits and a recovery arm UNSOUND, since the two
  causes are indistinguishable from durable evidence). 2.4 (authority
  parse/proof split): Code single-axis round-1 NO-GO (proven P0 —
  `cfg(windows)` const fn E0015, Windows did not compile; P1 —
  proof/parent provenance unbound; P2 — taxonomy 4-of-9); round-2 GO
  — the P0 closed structurally (one always-compiled body over
  `cfg!(windows)`, zero platform predicates left; the reviewer
  reproduced the forced-flip proof and endorsed it as stronger than
  its own cfg-swap), the streamed proof now carries its own
  provenance (the transaction lost its parent parameter; typed
  fail-closed join guard precedes the boundary announcement), and the
  taxonomy is a machine-checked 9/4 partition. Both escalation
  triggers RECORDED (2.3: 1 P1 + 3 P2; 2.4: P0 + P1 + P2);
  lane-owner routing for both: second-axis scrutiny folds into the
  Phase 5 settled dual (dual #3) per the three-dual cap.
  `capability_permit` callers 11→13 with the 2.4 file joined to the
  inventory at landing; the shared pre_catalog tree pin recomputed on
  the pristine overlay (checker green pre-push; the overlay pin
  matched the 2.3 lane's independently reported union value). The ARM
  exact_source fixture fix (distinct inode by construction) rode this
  train. Landing-brief duty conflict RESOLVED going forward (the
  mechanism behind this P1 and 2.2's identical finding, surfaced by
  the 2.3 implementer): checker counts/allowlist companions are the
  CONVERTING PACKAGE's same-commit duty; tree digest pins remain the
  lane owner's land-time duty, computed from the pristine overlay.

  **D3 LANDED 2026-08-22 (gwz-core `8b83a2c`) — dual settled GO/GO at
  round 2.** Round 1: Code GO (2 P2, 3 P3) / State NO-GO (1 P1 — four
  undelivered §8 legs; 2 P2, 3 P3). One merged remediation: all four
  §8 legs green in their designated homes (identical classification
  proven image-capture-free at the real seam against a paying
  degraded control; crash-window reproof and convergence;
  rollback-entry preflight made the structurally sole catcher by
  fixture geometry; byte-identical bundles; post-GC retention),
  CleanupMarkers v0/v1 fork (a marker-only row stays an empty row →
  ContradictoryEvidence on the v0 leg; §5 row content on v1),
  computed backfill at both reset edges, rename landed with the old
  identifier extinct. State P3-2 adjudicated IN THE IMPLEMENTER'S
  FAVOR: the reviewer withdrew its own decode-reject proposal as
  overreach (it would have bricked a reachable §4-protected shape)
  and accepted the write-edge marker-carry closure; the residual
  (fail-closed illegal-row refusal pre-publish) is recorded with a
  named follow-up to carry the stash pair. Code round 2 unconditional
  GO. Budget reconciled on the record: +2192/−103 vs §9's ~700–1,100
  — the excess is the 755-line §8 acceptance suite. Bonus erratum
  surfaced by the §8.6 seam: post-GC is projection-only (no durable
  rewrite) — strictly stronger terminal-plane immutability than
  documented. The three workspace_ops checker pins recomputed at
  landing. Spend-limit incident #2: the monthly limit killed the Code
  re-verdict mid-gates; resumed from transcript zero-loss after the
  operator reset and returned its GO. The conservation holds
  (Phase 5 dual, R4b-G, A1 activation) are LIFTED by the reset; the
  refined tier policy stands.

  **ARM diagnosis CLOSED (both clusters attributed; non-gating per
  thin A1).** Cluster (b) exact_source: ext4 recycles the freed
  inode; the fixture's assert_ne precondition broke while the
  production refusal held on every failing run; fixed fixture-side
  (rode the 2.3/2.4 train). Cluster (a) 29 g15 + 17 v1_lifecycle:
  Linux-RUNNER-only, never green on any Linux, NOT ARM-specific — a
  faithful arm64 parity rig (ubuntu:24.04, loop-ext4, PPA git 2.55,
  runner config, both uids) is fully green. Probe branch
  `probe/g15-gate-dump` (run 32561108291) named the cause: gate 4
  `files::observe_boundary` false 78/78 because `.git/info/exclude`
  is mode 0755 — the RUNNER IMAGE's git template tree carries the
  executable bit, `git init` copies it, the fixture's `fs::write`
  preserves the mode, and the leaf observer classifies executables
  Invalid BY DESIGN (observation.rs:216). Single-variable local
  proof: chmod 0755 on that file alone reproduces the verbatim CI
  message. libgit2-built fixtures (no template copy) pass —
  consistent with the v1 split. Remediation is fixture-side
  (pin/normalize the boundary-file mode), queued on the clean tree.
  OPERATOR POLICY QUESTION recorded for post-A1: a real repository
  initialized from an executable template tree (Ubuntu images ship
  exactly this) receives a typed root-preservation refusal — doc
  note vs a mode-tolerant arm for this one boundary file. Memo:
  `GwzArmPreservationHandoffDiagnosis.md`.

  **Windows anchor package LANDED 2026-08-22 (gwz-core `6b8b76e`) —
  review GO round 1, escalation trigger RECORDED (1 P1 + 4 P2),
  routed to the Phase 5 dual.** Root cause of the run-16 class: P5's
  Windows arm demanded a resident durability anchor only the
  checked-artifact private area may retain; E14 runs the barrier on
  the never-anchored retained action directory. Fix:
  `platform::DirentBarrierClass` — the caller states its writer
  class; seven legacy private-area sites keep the round trip;
  E10/E14's ExactInterior arm is a documented no-op in E9's
  writer-class-conditional sense (anchoring the action dir refuted
  structurally: admission exactness + exact-capture re-entry; the P2
  re-route refuted as a family move buying zero physical
  difference). Probe-verified GREEN on windows-2022 pre-landing
  (probe/namespace-anchor-win, run 32569434565, 22/0). Round-2
  remediation closed the review's record findings: the falsified E9
  residual-ordering claim corrected at five carrier sites including
  the shipped foreign-leaf refusal string; the §4.3 E10/E14
  activation annotation landed in contract form (win cells + §4.1 P5
  two-arm cell edited with it; the DurableNamespace-witness and
  parent_barrier-row negative space recorded directly; E9's clause
  superseded with its frozen text preserved verbatim). Both pins
  (platform.rs flat, pre_catalog tree) recomputed on the overlay and
  matched the lane's independently reported values. TRACKED: run 18
  must return the legacy AnchoredPrivateArea suites green (the
  trimmed probe exercised none of them) — the ledger carries the
  acceptance item.

  **Step 3.1 LANDED 2026-08-22 (gwz-core `e72e376`) — review GO
  round 1 (conditional), escalation trigger RECORDED (1 P1 + 1 P2),
  routed to the Phase 5 dual; both escalating findings were
  lane-owner dispositions, no remediation round.** The
  managed-parent bootstrap consumer: all three trait methods real
  (bounded no-follow preflight planning only the missing suffix —
  the Step-2.3 populated-components caution discharged;
  depth-tolerant revalidation; per-row execute_bound through
  stage/observe/retire/reprove). Zero publication sites added,
  permit 13 byte-untouched, census 165 with zero flips. Review
  adjudications: deterministic ownership token CORRECT AS BUILT
  (forced by the R2 stop clause; guarantee is self-consistency, NOT
  exclusion — re-litigate if E17 read-back or an R2-E consumer ever
  adopts a directory this action did not create); the
  retain_managed_parent re-export removal permitted and load-bearing
  for 3.3; refusal fail-closed structurally. LANE-OWNER DISPOSITIONS
  on the escalations: [P1-1] the E17 remainder (durable successor +
  prior-generation retirement; the partial-retirement restart window
  wedges ≥2-component rows) had no budgeted home in the plan — Step
  3.1b is MINTED for it, before the Phase 3 settle, same
  implementer; [P2-1] the five edges-converted-without-sites are now
  durably recorded as the §3.5 deferral record (this landing's
  freeze edit) with 3.2's exact debt enumerated (sites, rows,
  PartiallyExecuted 8→13). 3.1's P3s ride to 3.1b/3.2: the
  retain_managed_prefix comment corrections, the vanished-component
  settled-row pin, and 3.2's Git-directory arm blocker (test door or
  follow-up 3). Budget 980 code vs <500 aspirational — reviewed as
  load-bearing, recorded for the settle.

  **Step 3.1b DELIVERED 2026-08-22, under State-lead single-axis
  review.** The E17 durable managed-intent lifecycle: initial
  scratch/publish/reobserve, per-generation successor,
  prior-generation retirement, final retirement; resume_intent walks
  the resident chain link-by-link (each retired record's intent_id is
  the next link's expected predecessor — verified, not adopted); the
  review's [P1-1] wedge case (≥2-component row interrupted inside
  marker retirement) now converges, driven by name. Token read-back
  from the resident record with the self-consistency-not-exclusion
  boundary stated. FIFTEEN keys activated same-commit with sites and
  matrix rows (executed 8→23, reserved 22→7, counts held 165, no
  mint) — the 3.1 [P2-1] lesson absorbed; E17's two §4.4 arms
  resolve to none by the E16 mechanism; publication seam unchanged
  (21 sites, permit 13). Budget 812 code (~1.6×). Liftable §3.5
  annotation drafted. Open after 3.1b: 3.2's five writer keys +
  preflight/plan_complete disposition (reviewer asked to rule),
  follow-up 3, 3.3 glue.

  **ARM fixture package LANDED 2026-08-22 (gwz-core `c2d2f15`,
  direct-with-record per the fixture-class precedent — red-green
  proof standard exceeded review).** Mode pinning at fixture
  creation: g15 write_pinned at all five write sites + a permanent
  no-executable-leaf regression pin; v1 pin_fixture_boundary_mode in
  the four fixture builders owning the 17 rows. Red-green under a
  faithful runner simulation (scratch HOME + executable
  init.templatedir): red = exactly 29 g15 + the 17 named v1 rows;
  green = full population under plain AND simulated HOME.
  **ATTRIBUTION CORRECTION #3 (L1-16), recorded in the memo:** the
  probe's "libgit2 does not copy templates" premise was FALSE
  (git2-rs sets EXTERNAL_TEMPLATE by default; the copy maps
  executable→0755); all 17 v1 rows are libgit2-built, and the true
  discriminator is REPUBLICATION — write_atomic republication lands
  a fresh 0644 inode, so the passing fixtures were accidentally
  immune. Validation dispatched at `c2d2f15`: platform 32573547362
  (the program's first shot at ARM green), Windows 32573548540 (run
  18 — the anchor package's tracked acceptance item: legacy
  AnchoredPrivateArea suites + the namespace four).

  **Incident record (lane owner, 2026-08-22): reconcile stashes
  swept a live lane's edits, twice.** My post-landing reconciles
  (`git stash push -- <list> && git reset --hard origin/main`)
  stashed a REMEMBERED file list; the ARM fixture lane had
  meanwhile extended its edit set (v1 fixture files), and reset
  --hard reverted those uncommitted edits — twice. The lane detected
  the interference, restored from its own insurance copies, and
  re-verified every number on the post-landing tree; nothing was
  lost. RITUAL (6), effective immediately: the stash keep-list for
  any reconcile is computed from live `git status` at that moment
  (full complement of every path not in the train being landed),
  never from a remembered lane inventory; and lanes are told at
  launch to keep insurance copies of uncommitted work in their
  scratch area (this one did, which is why the burn cost nothing).

  **Step 3.1b LANDED 2026-08-22 (gwz-core `fcec69e`) — review GO
  round 1 (conditional, State-lead), escalation trigger RECORDED,
  routed to the Phase 5 dual.** E17 real; the partial-retirement
  wedge closed (the reviewer confirmed the fixture reproduces the
  3.1 [P1-1] case exactly and the base would have refused it
  permanently); chain linkage verified adversarially (binds action,
  reservation AND owner via matches_reservation — no fabricated
  higher generation, no cross-action adoption); both §4.4 arms
  resolve to none by direct read. Fifteen keys activated
  same-commit: executed 8→23, reserved 22→7, counts 165 held.
  Freeze: the 3.1b §3.5 annotation lifted at this landing, with the
  review's [P2-1] supersession clause added to the 3.1 deferral
  record (the five writer keys now sit among 7 reserved; 3.2's list
  edit is 23→28). Production LOC alone (440) was inside the <500
  target — the overrun is entirely mandated matrix evidence.
  DOCKETED SETTLE-BLOCKING for the Phase 3 settle (review [P3-1]
  ruling): `preflight` and `plan_complete` need a disposition — a
  step that owns them or a recorded determination they never gain
  boundaries; "reserved indefinitely" ruled incoherent now their
  converting package has landed. Folded into 3.2's brief: the
  BootstrapIntentRowV1::Scratch selector bypass ([P3-2]) and two
  inverted comments ([P3-3]).

  **PLATFORM MILESTONE 2026-08-22: the ENTIRE matrix is green
  simultaneously at `c2d2f15` — the first time since the R2-D fault
  families began landing.** Windows run 18 (32573548540): 1472/0/1 —
  the anchor acceptance item DISCHARGED (all seven legacy
  AnchoredPrivateArea suites green under the classed barrier; the
  namespace four extinct at `6b8b76e` as the probe predicted).
  Platform sibling (32573547362): ubuntu-24.04-arm 1510/0/1 — THE
  FIRST GREEN ARM RUN IN THE PROGRAM'S HISTORY (the 46-row
  template-mode class extinct name-for-name per the red-green) —
  and macos-14 1521/0/1. Remaining platform debt: none open; the
  3.1b/3.2 suites debut on Windows in the next full run
  (pre-attributed expected-green in the ledger); the g15 probe
  branches are cleanup candidates (operator's call, standing).

  **Step 3.2 LANDED 2026-08-22 (gwz-core `7169d89`) — review GO
  round 1, CLEAN (no P0/P1/P2, escalation NOT triggered; second
  round-1 clean of the program after M5b).** The five writer keys
  activated: managed_bootstrap.* at 28/30 executed, counts 165 held,
  the docketed pair (preflight/plan_complete) the only reserve. The
  single-crossing probe mechanism (crash → re-arm → next drive must
  settle without firing) machine-checks the partition's other half,
  caught a real once-per-component misclassification, and runs
  retroactively on 3.1b's keys; row shape is now a declared matrix
  property. Purpose policy matrix: four purposes via production
  constructors, §9 overlap rejection both directions. Two-sites-
  one-key for staging_directory_flush adjudicated honest (the 3.1
  review's own table specified it). The 3.2 §3.5 annotation lifted
  at this landing WITH the review's [P3-4] route-(b) sentence. Five
  P3s ride: shape-sentence corrections and dead fixture helpers fold
  into Step 3.3; the is_err-only §9 assertion and the over-applied
  one-component shape are recorded for the settle. Budget: 357 net
  test lines vs the plan's <500 — WITHIN; production +38. Phase 3
  remaining: Step 3.3 coordinator glue, then the Phase 3 settle
  record (dispositions: preflight/plan_complete,
  retain_managed_parent).

  **PHASE 3 SETTLED 2026-08-23 (single-axis regime per freeze §9 —
  the plan's Phase-3/Phase-4 "Gate: dual" lines are superseded by
  thin-A1's three-dual cap; recorded deliberately).** Steps landed:
  3.1 `e72e376`, 3.1b `fcec69e`, 3.2 `7169d89`, 3.3 `3a45619`.
  Step 3.3's round-1 P1 — authorize_write bound a CALLER-CHOSEN
  reservation — closed round 2 by comparing the observation's
  OBSERVED retained-parent identity (the one binding fact a caller
  cannot supply); the reviewer proved the check load-bearing by
  deleting it in scratch (only the cross-action row failed) and
  ruled the landed form STRONGER than its own remedy 2. Second
  occurrence of the caller-supplied-restatement class (first: 2.4
  P1) — named as a Phase 5 dual audit item: every gate that copies
  binding fields from an argument. SETTLE DISPOSITIONS (docket of
  8, all verified true of the tree by the 3.3 reviewer):
  (1) preflight/plan_complete — DETERMINED never-gain-boundaries;
  dated §3.5 determination filed at this settle; census 165 stands;
  5.1's evidence duty reading amended accordingly. (2)
  retain_managed_parent — KEEP as the documented cfg(test) door
  retainer; revisit only on an R2-E production caller. (3)
  Git-directory workspace-root binding — R2-E input; the typed
  preflight refusal is production-safe today. (4)
  staging_directory_flush second site — ACCEPTED one-key/two-sites
  (interior-first ordering proven; no sixth key for zero coverage
  gain). (5) multi-component writer interruption rows — assigned to
  Phase 5.1's evidence train. (6) partial-handoff resume via the
  test door — R2-E interface input; documented settled-tuple caveat;
  the admission-classifier widening belongs with real consumers.
  (7) blanket allow(dead_code) narrowing — assigned to Phase 4.3.
  (8) seam-level action binding in AuthorityObservationFactsV1 —
  assigned to Phase 4.3 (NOT deferred to R2-E: the defect class has
  fired twice), in the action-digest form with the reviewer's
  refinement adopted: it rests on slot-name derivation, so it lands
  PAIRED with an explicit consumer obligation stated at the seam.
  Also to 4.3: the 3.3 round-2 P3s (detail-string pin on the
  cross-action row; the stranded leaf_digests doc line). Escalation
  ledger riding to the Phase 5 dual: 2.3, 2.4, anchor, 3.1, 3.1b
  (2.2's discharged at its landing; 3.3's discharged at round 2)
  plus the named class audit. Phase 4 launches on `3a45619`.

  **Step 4.1 LANDED 2026-08-23 (gwz-core `9c454ce`) — review GO
  round 1, no escalation (6 P3).** E18-E21 convert in situ through a
  sealed composition of P1's arms carrying the legacy identity
  vocabulary. SCOPE DIVERGENCE RATIFIED (lane owner): my brief's
  publish_verified_no_replace route was WRONG — HostPlatform-bound
  to the closed support table while the legacy writer is the live
  path on any persistent-handle filesystem; the reviewer added a
  fourth leg (the route would not even compile — pub(super)) and
  ruled the twin seam SOUND with both halves pin-protected.
  **E21 latent defect, graded P2 as it stood, found and closed by
  this step:** authority_name embeds no identity digest, so the old
  code completed a same-byte authority substitution outright;
  adversarial-only, same-user boundary, hence P2 not P1. Recorded
  for the settle and the metrics table. cleanup.* flips ZERO keys —
  duty never attached (all 11 need AdmittedActionV1); re-reserved
  for R2-E via the §3.5 record lifted at this landing (with the
  reviewer's one-word placement fix). Landing adoptions:
  transition.rs and residue.rs gain flat digest pins (both sealed);
  platform.rs pin recomputed; the platform.rs split (1,093 lines) is
  BOUND TO STEP 4.2 BY NAME. Riding notes: twin-seam unification
  docketed for R2-F ([P3-1]); the authority_name
  non-self-checking-name design note carries to R2-E; [P3-4] (one
  fault variant, four sites — split or record deliberate) routed to
  4.2; the macOS-narrowing prose error ([P3-2]: it is MNT_LOCAL, not
  persistent-id absence) corrected in the lifted record. L2-04
  harness green locally (86 tests, 24 tuples); its network matrix
  job runs automatically on push (path filter covers
  scripts/checks/**) — no dispatch needed. Both matrices dispatched
  at `9c454ce` (Windows 32607411300 = run 19, first native execution
  of the 3.1b/3.2/3.3/4.1 suites; platform 32607412448).

  **Step 4.2 LANDED 2026-08-23 (gwz-core `51a9cba`) — review round 1
  GO with §9 escalation RECORDED (2 P2, 5 P3); round 2 GO, both P2s
  closed AT THE MECHANISM, escalation DISCHARGED.** The legacy
  Windows anchor machinery retired: nonce → one deterministic
  staging name (the standing R2 stop-clause violation removed);
  alias remove_file → durable retirement onto ordinal-indexed names
  chosen smallest-free from observed state (round 1's fixed
  destination was a REACHABLE permanent wedge — deleted, not derived
  away; the reviewer independently re-derived that count-based
  naming would have wedged inside the fix). All ten boundaries
  driven incl. both retirement boundaries, natively probe-verified
  (probe/anchor-retirement-win, final run 32612243125, 82/0, with
  the hard-link pin proving the retirement branch executed; the
  probe cycle itself caught a fixture bug red-then-green). [P3-2]
  fixed in-package: authority::scratch_name's nonce on 4.1's
  converted E20/E21 — Phase 4's own trigger condition — now
  action-scoped deterministic; getrandom gone from authority.rs.
  SCOPE DIVERGENCES RATIFIED (lane owner): barrier.* (16) and
  terminal.* (11) flip ZERO keys — both vocabularies name frozen
  admitted-action protocols (the roaming anchor; terminal
  retirement), consumer conversions outside R2-D; the legacy anchor
  is the roaming protocol's ANCESTOR, not an instance; all 27 keys
  re-reserved for R2-E via the §3.5 records lifted at this landing.
  MACOS PLATFORM FACT pinned by measurement: per-hard-link
  ATTR_CMN_OBJPERMANENTID means the stranded-alias state was never
  reachable there. Landing adoptions (both reviewer-blessed):
  platform.rs converts FLAT→TREE pin (covers the anchor module +
  its 548-line evidence file); authority.rs gains a flat pin as the
  private family's naming authority. PROBE DISCLOSURES accepted:
  four pushes total to probe/anchor-retirement-win (remediation +
  fixture fix beyond the one authorized), same branch only, verified
  by the reviewer (origin/main untouched, no tags). LANDING INCIDENT
  (ritual 2 vindicated): my overlay rsync silently dropped the new
  platform/anchor/ subdirectory (git status collapses new dirs to
  one line; --files-from without -r copies them empty) — caught
  BEFORE push by the pin cross-check against the reviewer's
  independently computed tree digest; anchor files copied, pin
  recomputed to the matching value. Riding to 4.3/settle: [P3-6]
  the racing-drive window has no executed row (three of four
  executed + the gap named); [P3-7] survey's Invalid is a permanent
  no-exit refusal on foreign contamination (the block's
  reachable-state claim stays precise); the 4.2 review's round-1
  [P3-4] freeze-cite drift fixed at this landing against the landed
  tree. Budget: net +1062 across both rounds (the dual corrected the round-1 figure by one) (≈28% production /
  ≈20% bound relocation / ≈52% evidence) — second consecutive
  over-budget Phase-4 step, composition recorded per the reviewer's
  ChangeBudget guidance. Escalation ledger for the Phase 5 dual
  gains: 4.2 round-1 (2 P2, discharged round 2 — recorded as
  discharged). Run 20 dispatched at `51a9cba` (Windows 32613728318,
  platform 32613729322).

  **Step 4.3 LANDED and PHASE 4 SETTLED 2026-08-23 (gwz-core
  `514f8e6`; single-axis regime per thin-A1, the plan's Phase-4 dual
  line superseded on record).** Review GO round 1 (1 P2, 7 P3 — all
  record-class; corrected at the landing per the review's own
  values, including the lift-blocking exact_row fix, the honest
  dead-code figures, and the [P3-6] rationale clause; escalation
  RECORDED and routed to the Phase 5 dual). THE COEXISTENCE
  DECISION (audit P3-3, A1-gating) IS MADE AND RECORDED:
  **quarantine/relocation**, per §7.2's adopted direction plus two
  facts new since adoption (Windows anchor permanence makes the
  grammar collision permanent; the retirement family is
  crash-bounded, not constant-bounded); execution pinned to the
  R2-F relocation package; the A1 gate stays fail-closed in code.
  Record: dev-docs/history/GwzM5-8R2DPhase4Closure.md §2.7. The
  dirent-barrier resume-window residual is verified CLOSED in code
  (all six Ready edges through the keyed prologue; regression
  pinned; R2-F power-loss companion survives). Settle item 8
  LANDED: the 2.4 seam refuses mis-paired action digests itself,
  consumer obligation on the signature, two smuggles/two gates both
  executed. Settle item 7 LANDED: blankets narrowed; dead-code
  reality recorded FOR STEP 5.1'S AUDIT: **481 items were hidden by
  the module-level allows; the global measure is 1657 spans / 85
  files**, all R1/R2-frozen interface awaiting R2-E, zero orphans.
  PHASE 4 TOTALS: E18-E22 converted; the R2 stop-clause nonce class
  extinct (anchor + authority family); raw-rename surface = one
  delegation reference; cleanup./barrier./terminal. all
  re-reserved for R2-E with sanctioned records (the census stands
  at 165; the executed arithmetic is the settled tuple §4.8's
  (the "51" this record first carried reconciled with no sum —
  corrected on the dual's finding), with managed_bootstrap 28/30); three consecutive fully-green matrix
  trees. ESCALATION LEDGER — FINAL FOR THE PHASE 5 DUAL: riding =
  2.3 (1 P1+3 P2), 2.4 (P0+P1+P2), anchor (1 P1+4 P2), 3.1
  (1 P1+1 P2), 3.1b (recorded), 4.3 (1 P2, remediated at landing);
  discharged-on-record = 2.2, 3.3 (round 2), 4.2 (round 2); plus
  the named class audit (caller-supplied restatement — fired 2.4,
  3.3; hardened at the seam by 4.3). NEXT: Phase 5.1 evidence
  train, then the 5.2 settled dual (Fable ×2, cross-model per the
  plan's own line, two-round cap).

  **Step 5.1 LANDED 2026-08-23 — THE SETTLED TUPLE TREE IS gwz-core
  `d45458d`.** The twelve-gate train ran and recorded verbatim
  (dev-docs/GwzM5-8R2DSettledTuple.md, 683 lines, run-21 slot
  filled); it caught and repaired a red gate (the boundary unit
  suite's companion assertion, broken by MY 4.2-landing flat→tree
  pin conversion — repaired and HARDENED: the generic prefix would
  have passed on any finding) and a stale audit note (4.3's own
  narrowing falsified it; corrected and measured at 29 live
  warnings). Evidence: 107 keys tabled site+row+both-variants; 2
  determinations; 38 reserved with records; ONE GAP REPORTED NOT
  PAPERED — runtime.*'s 18 keys are declared Executed on a disjoint
  six-variant mechanism with no per-key correspondence and a
  hard-coded exemption; §3.5 declares the exception, but its worth
  is below the other families' standard — FILED FOR THE 5.2 DUAL
  with two census-neutral options (re-reserve like cleanup.*, or
  restate the frozen claim). Phase-3 settle item 5 discharged (10
  multi-component rows, correctly refusing partition claims).
  Platform record: nine of nine green — three consecutive
  fully-green trees on all three arms. NEW OPEN ITEMS for the dual:
  the L2-05 merge-doc gate is wired into NO workflow (passes only
  where the sibling gwz-cli checkout exists — a gate that only
  passes on a developer's machine is not a gate; CI-wiring item);
  the two not-run gates scoped honestly (the network matrix job —
  CI's; regen.py --check — the release venv's).
  managed_mutation.rs at 1,251 lines is the one over-trigger file,
  deliberately unsplit during the evidence step. Budget +53 net vs
  <300. THE 5.2 SETTLED DUAL LAUNCHES ON `d45458d` — Fable ×2 per
  the tier policy (the plan's "cross-model" line is satisfied in
  axis, not vendor; recorded as a deliberate reading), two-round
  cap, carrying the full escalation ledger and the runtime.*
  adjudication.

  **R2-D IS SETTLED — 2026-08-23, dual #3 of 3 consumed, GO/GO at
  round 2 within the two-round cap. The settled object is gwz-core
  `b91bdeb` with the freeze as amended at the remediation commit.**
  Round 1: Code GO (0 P0/P1/P2, 6 P3 — the named-class audit
  returned an 18-gate table with an affirmative no-live-instance
  statement; the one remaining seam SHAPE, BarrierIntentV1::issue,
  is production-unreachable and now a BINDING R2-E obligation) /
  State NO-GO (1 P1 — THE AGGREGATE CATCH THE DUAL EXISTS FOR: the
  three Phase-2 activations never got their §3.5 amendments, 35
  keys misdeclared "reserved", fallen in the seam between Phase 1's
  row-edit and Phase 3's annotation disciplines; + the runtime.*
  adjudication RULED RESTATE with drafted text; 3 P3). One merged
  document-class remediation: the reviewer-drafted annotations
  lifted verbatim (mechanically verified 45/45 and 44/44 lines),
  the comment-clause fix at `b91bdeb`, the register completed, the
  exactness bundle corrected. Round 2: Code GO unconditional /
  State GO — "the map is now true for all ten families; what
  remains open is exclusively named P3 residue filed where the next
  lane reads it. R2-E may start behind this gate." The deferred
  second-axis design is VINDICATED on the record: every riding
  escalation got its scrutiny and the dual caught the one defect
  class only an aggregate view could see. Reports:
  GwzM5-8R2DSettled-Review{Code,State}.md (rounds 1+2 appended).
  THIN-A1 STATUS: R2-D settled ✓ and M5b bound proofs green on the
  settled tree ✓ — the A1 activation's two preconditions stand.
  Remaining: R4b-G, then A1 activation.

  **R4b-G evidence executed 2026-08-23
  (dev-docs/GwzM5-8R4bG-Evidence.md, 851 lines): 22 PASS / 7 FAIL /
  6 RE-FRAMED (5 evidenced) / 2 DEFERRED-BY-AMENDMENT / 1 PENDING.**
  Members verified at the tuple pins (gwz-cli 3cca145 139/0; gwz-py
  929efb0 330/0 with regen --check green; taut f008419); the
  acceptance object stated (the ~25k-line R4b reverse lifecycle;
  GO unblocks the M5b-IMPL settled review and the escape
  implementation packages). Remediation of F-1..F-5/F-7 dispatched
  (matrices for F-6 dispatched at b91bdeb = run 22); new citable
  measurement root_fault_matrix 318.71s release at b91bdeb (vs the
  576.03s on record). LANE-OWNER ADJUDICATION OF J-1, on record for
  the dual: M5b-IMPL (3e60529, 8c1624a) reached b91bdeb's ancestry
  ahead of R4b-G against the must-wait clause
  (GwzM5-8M5bNoFfDesign.md:976-986) — the deviation is REAL and was
  a sequencing artifact of the 2026-08-22 acceptance train, before
  thin-A1 re-framed R4b-G behind the R2-D settle. Ruled ACCEPTED
  WITH RECORD on four legs: M5b measured zero production lines
  under its ceiling with T-6 and the clean-tree re-cut; its dual
  was round-1 clean on both axes; the leaned-on call-graph gate
  (F-3) lands with this remediation as a standing guard rather
  than a retroactive claim; and the D3 dual's round-2 re-verdicts
  verified the five M5b surfaces byte-identical on both axes
  (`GwzM5-8D3Impl-ReviewCode.md:447-448`: `store/tests.rs` at
  exactly +2/−0, M5b byte-identity intact;
  `GwzM5-8D3Impl-ReviewState.md:550-552`: M5b 37/0 with all five
  files byte-identical) — **[leg restated 2026-08-24 per the
  R4b-G Evidence axis's P2-1: the original text mis-attributed
  this verification to the settled dual, whose reports do not
  mention M5b; the fact was true, its citation was not.]** The
  R4b-G dual judges the acceptance; if it rules the deviation
  disqualifying, the remedy is the M5b-IMPL settled review
  running BEFORE A1 rather than concurrent with it.

  **R4b-G DUAL ROUND 1 (2026-08-24, peer-blind, at the tuple
  gwz-core `78badbc` / gwz-cli `3cca145` / gwz-py `929efb0` /
  taut `f008419`): Correctness GO-conditional (1 P2, 6 P3) /
  Evidence NO-GO (1 P2, 6 P3)** — reports
  `GwzM5-8R4bG-Review{Correctness,Evidence}.md`. Merged round-2
  remediation executed 2026-08-24: **tooling at the next gwz-core
  train; records in this commit.** C-1 (the "every M4"
  byte-equivalence clause measured NOT MET at 13/39) is **bound to
  the A1 activation review's input register as BLOCKING-FOR-A1 per
  L1-19** (`AgentProcessRules.md:392-401`), routed under
  `GwzM5-8ThinA1Amendment.md:43-55`; C-2 (4 unfixtured scenarios)
  rides the same register at P3. **J-1 is RATIFIED by both axes**
  on independently re-verified facts (Correctness §5 J-1;
  Evidence §6), **the before-A1 remedy is NOT triggered**, and the
  **M5b-IMPL settled review is recorded as owed pre-A1** on its
  frozen tier. Records landed this pass:
  `GwzM5-8R4bG-Evidence.md` (J-1 leg 4, F-7 provenance, the C-1
  reconciliation and the PARTIAL gate condition),
  `GwzM5-8R2DSettledTuple.md` §11.1 (perf-pricing date, C-1, C-2,
  M5b-IMPL), `GwzM5-8M5bNoFfDesign.md:976-986` (annotation),
  `GwzWindowsMatrix-Classification.md` (run-number header note).

  **R4b-G IS ACCEPTED — 2026-08-24, both axes GO at round 2 within
  the two-round cap; the round-1 conditional CONVERTED with its
  condition satisfied on the record and verified, not waived. THE
  ACCEPTED TUPLE, restated literally per the Evidence axis's close
  ruling: gwz-core `1bd885f` / gwz-cli `3cca145` / gwz-py
  `929efb0` / taut `f008419`.** Zero open P0/P1/P2 against the
  gate object. The acceptance record carries: the byte-equivalence
  battery is PARTIAL (7 proven + 6 refusal-pinned of 39) and must
  not be cited as green (evidence §12.8); C-1 [P2, BLOCKING-FOR-A1
  per L1-19, four live rows named] and C-2 [P3] are bound on the A1
  input register; J-1 stands ratified with its leg 4 restated to
  the D3 dual. CI at the tuple: retained-readers green at
  `1bd885f`; boundary lane  at commit time (scripts-only delta
  over the green `78badbc`; the close is not conditioned on it).
  UNBLOCKED: the M5b-IMPL settled review (launching) and the
  operator-escape implementation packages (second lane, still
  BLOCKED ON OPERATOR HANDOFF — unchanged). PRE-A1 QUEUE, exact:
  (1) the C-1 closure package — state and test the adaptation
  dispositions for F-BASELINE/F-MARKER/F-LOCK and
  J-NO-PUBLICATION-UNBORN; (2) the M5b-IMPL settled review per the
  frozen dependency statement; then (3) THE A1 ACTIVATION REVIEW,
  whose input register is complete (the acceptance-debt record,
  the D3/D4 residual dispositions, the signed perf pricing,
  C-1/C-2). OPERATOR ITEM (non-gating): the ChangeBudget
  charging-convention row is filed OPEN with both reproducible
  figures (2,028 uniform / 582 review-artifact) — the ruling is
  the operator's.

  **THE PRE-A1 QUEUE IS COMPLETE — 2026-08-24. The A1 activation
  review launches on the tuple gwz-core `26f48f5` / gwz-cli
  `3cca145` / gwz-py `929efb0` / taut `f008419`.** (1) C-1 CLOSED
  at `26f48f5`: all four live rows dispositioned FROM the frozen
  contract with the derivations quoted, tested through the real
  decode path; new finding surfaced and pinned — the enumeration's
  J-twin labels were backwards (the contract's born-root exclusion
  makes the UNBORN twin the adapted one); the driver's partition
  off-by-one (1572 vs 1573) reported not silently fixed. (2) The
  M5b-IMPL SETTLED REVIEW is GO (0 P0/P1, 1 P2, 3 P3) — the J-1
  obligation discharged pre-A1 on its frozen tier; six trains of
  drift accounted to the line. Its [P2-1] (T-5's no_ff envelope
  pair absent) is CLOSED BY LANE-OWNER NARROWING per the finding's
  own offered remedy, on structural grounds the closure lane
  measured (envelope classification is header-keyed; the body
  cannot participate); the built candidate pair is saved with
  digests and rides any future evidence regeneration. Its [P3-1]
  (the forged-action gate's ChangeBudget row) is FILED
  existence-first with the commit named, numbers withheld under
  the OPEN convention. (3) The A1 register, final: the ~3.5k
  acceptance-debt named exception; the D3/D4 residual dispositions;
  the signed perf pricing (318.71 s); C-1 closed; C-2 [P3]; the
  T-5 narrowing (the review judges it); the PARTIAL
  byte-equivalence statement; M5b settled GO. The activation
  review's object: enable the v1 writer + --no-ff per thin-A1 §2,
  with the §14 ninth-stop-clause post-A1 re-scope and the R2-F/R5
  native-evidence RELEASE gates explicitly unmoved.

  **THE A1 ACTIVATION REVIEW, ROUND 1 (2026-08-24, peer-blind, on
  the accepted tuple): Safety GO-CONDITIONAL / Completeness
  GO-CONDITIONAL — zero tree defects on either axis.** Reports
  `GwzM5-8A1Activation-Review{Safety,Completeness}.md`. The Safety
  report's §2 is THE BINDING ACTIVATION SPEC: six compile gates,
  four runtime gates (the NoFf-refusal fall COUPLED with the
  message-validation exclusion fall), both CLI unhides, and the
  must-not-flip list. Safety conditions: [P1-1] F-MARKER/F-LOCK v0
  resume recovery pinned post-activation; [P2-1] a Finalizing
  pre-check gating the adaptation preflight; [P2-2] CI wiring of
  the unwired checkers; [P2-3] the landing record set incl. the
  OPERATOR-SIGNED D3/D4 dirty-boolean residual disposition (sites
  cursor.rs:305-306/:332-333/:498) — **the operator signature is
  the halting item: branch (a) accept-as-residual or branch (b)
  remediate first; without it activation is NO-GO.** Completeness:
  the 364 routed phrases swept (17 delivered / 12 judged-here /
  14 post-A1 / 2 leaked, both placed); its [P2-1] (T-5 artifact
  durability) CLOSED pre-build — candidate pair archived to
  `GwzM5-8T5CandidatePair.patch` + the R2-F carrier owner row in
  the settled tuple §11.2; round-2 conditions: the [P2-1] legs and
  the activation record carrying the report's §3 enumeration; P3
  folds owed to the landing record set (P3-1 Q3 citing line, P3-2
  abandonment-witness placement, P3-3 owners at register close,
  P3-6 register consumption statement, F5 §9 dispositions).

  **THE A1 ACTIVATION PACKAGE IS DELIVERED — 2026-08-24, in-tree
  (uncommitted) over the accepted tuple; report filed verbatim as
  `GwzM5-8A1ActivationPackage-Report.md`; ROUND 2 DISPATCHED to
  both axes on the tree + filed report.** 88 files gwz-core
  (+1852/−609; new: `merge/model/version.rs`,
  `merge/v1_lifecycle/start.rs`, `tests/g23/a1_activation.rs`),
  3 gwz-cli, 2 gwz-py. Lands: §2.1 all six compile gates FELL (G6
  REPLACED not un-gated — `_for_r3_tests` re-exports renamed to
  production decode names, fault injectors keep cfg; the gate was
  wider than the six coordinates — 97 further cfg(test) markers +
  28 cfg_attr(Serialize) fell; the fall measured behaviour-neutral
  at 1572/0); §2.2 R1+R2 in one edit, R3 =
  `classify_merge_record_header` with PRODUCTION{v0,v1} (T-2
  inverted at header dispatch + archive decoder), R4 PARTIAL — THE
  ROUND-2 DECISION: `select_record_version = max(floor, semantic)`
  is production and NoFf→V1 writes v1 end-to-end today, but
  ACTIVE_WRITER_FLOOR stays V0 — raising it was MEASURED to break
  every ordinary start (no production v1 owner for root
  participants / dry-run prediction / drift-conflict surfaces /
  the v0 event stream); one-line change when that owner lands, the
  max proven by test. §2.3 both CLI surfaces (docs tripwire
  inverted, CLI.md regenerated); §2.4 the production callers
  (V1Router with sealed MergeAuthorityBackend; classify_open_record
  envelope probe; NEW `create_open` + `handle_start_durable_v1`;
  dead-code warnings 921→63 when dispatch landed). Conditions
  [P1-1] / [P2-1 via option (i), archive shapes riding as named
  residual] / [P2-2 — L2-05 unwired with the reason recorded] /
  [P3-1: T-1/T-2 inverted, T-3 re-pinned 16→19, T-4 unchanged] MET
  per report; must-not-flip verified 9/9, F-3 still 0 hits. Gates
  at delivery: core 1582/0/1 (census 1583, +10), cli 139/0, py
  330, L2-04 86/0 tuple_count 24, scenario map ok, docs 147
  assertions ok. EXPECTED-RED, landing-train duty (report §6): 5
  boundary digests (merge/mod.rs root manifest; artifacts.rs +
  plan.rs flat; observe.rs + v1_lifecycle/mod.rs tree), 3 R4b-G
  count markers (fault 254→255 and 917→926; byte-equivalence
  114→119), and 11/69 checker probe-harness tests (G1's
  blanket-allow expiry broke the probe compile; LANE-OWNER RULING
  on record: apply the builder's minimal fix — emit
  `#[allow(dead_code)]` with the injected probe — at landing;
  the guarded property itself is intact, F-3 scan 0 hits). Named
  residuals (report §7): the R4 ordinary-start row; ONE BEHAVIOUR
  PIN MOVED deliberately (the g23 finalization nested-fault test's
  final window now asserts the migration; v0-fault-injector
  coverage on the authority path reduced for the seven whitelisted
  shapes — flagged to round 2); positive executed evidence that
  migrated Boundary/Staging prefixes keep the v0-equivalent next
  action (contract §4); the gc_archived family's
  no-production-caller allowance. Insurance:
  `scratchpad/insurance-a1/` M-A…M-I-delivery. **THE LANDING
  HALTS ON THE OPERATOR SIGNATURE.**

  **A1 ROUND 2 IS GO/GO — 2026-08-25.** SAFETY GO (0 P0 · 0 P1 ·
  1 P2 · 5 P3; report §"Round 2" appended): the package accepted
  against the enumeration with every quoted gate re-run by the
  axis (census 1583 reproduced; py 330/0 against a native module
  the axis REBUILT itself — the builder's py tail ruled
  not-evidence, [P3-R2-4]); the wider compile-gate fall ruled
  in-spec (the 28 Serialize cfg_attrs are logically G2); **R4's
  V0 ordinary-start floor RULED an ACCEPTED NAMED RESIDUAL** on
  four grounds, with three binding record conditions — (a) the
  ordinary-start v1 owner becomes a first-class named milestone
  and the floor raise lands WITH it as one reviewed change, (b) a
  dated annotation on the frozen contract §2 creation-matrix A1
  row, (c) the release-time retained-reader manifest describes
  SHIPPED behaviour (ordinary=v0, no-ff=v1); the moved pin
  ACCEPTED with the compensating obligation named — the
  eligible-row upgrade-failure fallback, carrier R2-E
  ([P3-R2-2]); **[P2-R2-1]: the probe-harness cure is FOUR
  classes** (the recorded allow-emit fix cures only
  probe-compile; the sentinel reinstated UN-GATED; the
  seam-surgery string updated; the digest re-pins) — applying
  only class (a) would land a RED push on the newly wired CI
  step. COMPLETENESS GO (0 P0/P1/P2 · 2 P3; §"Round 2"
  appended): its round-2 conditions closed ([P2-1] byte-identical
  archive at `47bbd7a` + the §11.2 OWNED-CARRIER row; the §3
  enumeration verified in-tree with executions g23 119/0, no_ff
  32/0, version 15/0, map 39/41/13); **THE LANDING SPECIFICATION
  L1-L14 filed for verbatim folding** (L7/L8 added to the fold
  list by the axis); [P3-7] a fresh `unreachable!` v1 arm at
  decode.rs:123 (typed-twin conversion, L13); [P3-8] three named
  blockers lacked named carriers. LANE-OWNER RULINGS for the
  record set: the ordinary-start v1 owner is MINTED AS **M5c** (a
  first-class named milestone on the register; the `version.rs:39`
  floor raise rides it as ONE reviewed change per contract §9
  discipline); the archive/GC family (gc_archived route,
  18-UNBOUND record debt, the archive-equivalence mechanism
  decision, the two archive shapes riding [P2-1] option (i)) is
  owned by **R2-E's archive/GC consumer sub-package**; the
  moved-pin coverage restoration IS [P3-R2-2] at R2-E (the two
  axes converged on the same arm); L10 item 2: `cargo fmt
  --check` release-lane-only ADOPTED-AS-IS; L5: the
  abandonment-witness probe LANDS with the train (the
  commit-the-probe arm); L13: the typed-twin conversion LANDS.
  **LANDING TRAIN DISPATCHED IN-TREE** (the package builder
  resumed): the four-class cure, L13, L5's probe, the [P3-R2-5]
  doc nit — then count markers and digest re-pins computed from
  the FINAL tree, then the full gate set incl. the boundary
  harness 69/69 and a fresh-maturin py run. THE COMMIT STILL
  HALTS ON THE OPERATOR SIGNATURE (L1; the site set verified
  in-tree: cursor.rs:283-308/:310-335 fallthroughs,
  phase.rs:187,198, observe/finalization.rs:217, cursor.rs:498).

  **THE OPERATOR SIGNATURE IS IN — 2026-08-25, BRANCH (a), verbatim
  "sign branch (a)".** The D3/D4 dirty-boolean residual over the L1
  site set is ACCEPTED AS A NAMED RESIDUAL; remediation, if any, is
  post-A1 work scheduled by the operator. Recorded in
  `GwzM5-8A1ActivationRecord.md` §1 with the formal disposition
  text; the Safety [P2-3] signature hinge is satisfied. **THE
  LANDING HALT IS LIFTED.** The landing now waits only on the
  landing train's green report, then: overlay ritual (pins
  recomputed from pristine extraction, checker+gates green in
  overlay), the coordinated `gwz commit` (records + contract
  annotation + lock re-pin + package), the exact-ref gwz-core push,
  and the three-arm matrix dispatch at the landing commit (L11).

  **A1 IS LANDED — 2026-08-25. THE ACTIVATION TUPLE: gwz-core
  `1a31851` (exact-ref pushed, main 26f48f5 → 1a31851) / gwz-cli
  `3000916` (local) / gwz-py `3d19dcd` (local) / taut `f008419`
  (unchanged). The v1 merge writer and `--no-ff` are live on the
  mainline.** The landing train's tree duties completed green
  first: the four probe-harness cure classes PLUS A FIFTH the train
  discovered and cured (cfg-agreement on injected observer-caller
  probes — pre-A1 the `v1_preservation_image` call site was itself
  v1-gated so edge and call agreed; G1 broke the agreement; cure
  gates the injected statement `#[cfg(test)]`, checker
  cfg-indifferent); L13's typed twin (`RecordDecodeError::Body`,
  "the v0 decoder received a v1 record"); **L5's abandonment
  witness PASSES with EQUAL shapes** —
  `service_level_abandonment_of_a_not_started_action_is_mode_blind`
  (no_ff_wire.rs:319; both modes `Refused {
  MergeRecoveryRequired, "v1 transition predecessor or authority
  mismatch" }`; non-vacuity verified; zero NoFf tokens, T-3 stands
  at 19 files) — M5b [P3-3]'s unreachability executable for the
  first time; the sentinel reinstated un-gated; [P3-R2-5] doc nit;
  counts 256/926/119 and the five digests re-pinned from the final
  tree (merge/mod.rs `76e4830e…`, v1_lifecycle/mod.rs `74a416a6…`
  moved past the report — sentinel + witness). Final tree: census
  1584 (1583+1 ignored), harness 69/69, checker ok, clippy clean,
  R4b-G batteries green, cli 139/0, py 330/0 on a fresh maturin
  build (01:42:48 .so). gwz-py Cargo.lock refresh ruled in (record
  §8). LANDING RITUAL CLEAN: overlay at origin tip `26f48f5`, 91
  files `rsync -rR`, checker green IN OVERLAY (the pin
  cross-check), check/fmt/clippy-from-clean/focused (5+2+1+1)
  green, commit, exact-ref push, live reconcile with LIVE SET ==
  LANDED SET verified by diff before reset — zero stash incidents.
  Root activation commit (this one): the operator-signed record
  `GwzM5-8A1ActivationRecord.md` (all slots filled), the frozen
  contract §2 A1-row dated annotation, the ChangeBudget L12 row,
  the classification L11 dated update, `gwz.lock.yml` captured to
  the tuple. MATRIX AT THE LANDING COMMIT: windows `32749489320` +
  platform `32749492896` dispatched at `1a31851`; push-triggered
  boundary + retained-readers (`32749441874`/`32749441866`) running
  with the newly wired [P2-2] step; pre-attributed expected-green:
  the +11 package tests and C-1's 3 first-native executions.
  **LANDING-COMMIT CI INCIDENT + CORRECTIVE `8e40fa8` —
  2026-08-25.** At `1a31851`: Windows/Platform/Retained-readers
  GREEN; the boundary run `32749441874` FAILED on the newly wired
  [P2-2] step's FIRST command — `check_m4_scenario_map.py` resolves
  its map doc ONE LEVEL ABOVE the checkout by design
  (`:59 ROOT.parent/dev-docs/GwzM5-8R4bG-Evidence.md`), absent on a
  single-repo runner: the SAME blocker class the workflow's own
  L2-05 comment records, missed by the builder, both round-2 axes,
  and the landing train because every execution was
  inside-workspace. Corrective `8e40fa8` (overlay ritual, exact-ref
  push, live reconciled): the m4 map un-wired with the blocker
  recorded beside L2-05's — same owner, same R2-F
  multi-repo-checkout cure (tuple §11.3 item 7); the privacy and
  call-graph batteries STAY WIRED (verified no workspace-root
  reads; their contents ran green in CI's earlier step at
  `1a31851`). [P2-2] substance restated in activation record §17
  (the addendum). LESSON MINTED (wiring class): local-green proves
  nothing about single-repo-runner topology — wiring acceptance
  requires executing the wired command under the CI checkout
  topology, or an explicit path-resolution topology audit, BEFORE
  the push. Boundary run at `8e40fa8`: **`32754064482` GREEN**
  (completed:success in ~41 min — the wired privacy + call-graph
  batteries executing for real in CI, pinned counts and all). With
  it, EVERY CI surface at the activation is green: Windows
  `32749489320`, Platform `32749492896`, Retained readers
  `32749441866` at `1a31851`; boundary at the corrective
  `8e40fa8`.

  **THE v0.11.0 RELEASE TRAIN IS CHARTERED AND LAUNCHED —
  2026-08-25.** Plan: `GwzM5-8A1ReleasePlan.md` (gates G1-G6, phases
  R1-R6). Operator decisions on record: **vNEXT = v0.11.0**
  (verbatim "v0.11.0"); **R4 = branch (i)** (verbatim "branch (i)")
  — the narrow §4.4 wedge-runbook slice is handed off to the lane
  (docs only; escape implementation packages remain second-lane
  blocked); R6 timing and the R5 member-push/tag scheduling remain
  OPEN. On "launch R1/R2/R3": three parallel Opus builders launched
  in isolated gwz-core worktrees at `8e40fa8` — R1 (Decision 2 A′
  pre-mutation foreign-filter refusal, ~350 LOC, focused State
  review to follow), R2 (Decision 1 B birth-time CRLF pins + the
  un-pinned matrix sentinel + decided-annotation texts delivered
  not applied, ~200 LOC, focused State review to follow), R3
  (retained-reader A1 generation + release-lane battery wiring
  behind the §17 topology audit + version reconciliation to
  v0.11.0 + release-notes draft, interior single-axis). Shared-file
  discipline: builders write only their own worktrees; every
  dev-docs edit is the lane owner's at landing. Landing order:
  each phase lands as its own reviewed train, R1 first if
  contention arises (G1 is the forbidden-ordering gate). R4
  (runbook) queues behind R1's final refusal surface.

  **R1 DELIVERED WITH A MATERIAL DISCOVERY — 2026-08-25: Decision 2
  A′ WAS ALREADY LANDED at origin/main**, by `9939b02` (2026-08-16,
  "Land the filter-policy (D1+D2) and R2-F missing-tests packages",
  + follow-up `90d3f8a`) — the same day the decision packet was
  written. The code closed; THE LEDGER NEVER RECORDED IT (no dev-doc
  mentions refuse_foreign_filtered_rewrites, 9939b02, or
  filter-policy), which is exactly how the release plan came to
  re-schedule R1.1/R2.1 as unstarted work. **[CORRECTED 2026-08-25
  per R1.2 [P2 F-1], condition C1: the preceding sentence is FALSE
  as written. The ledger DID record the decisions — the D2/A′
  discharge is annotated at
  `GwzM5-8ExactEvidencePlatformAmendment.md:156-175`, D1's closure
  at `:108-121` — and a State review DID run at the `9939b02`
  landing (its commit message: "State review GO, F2 same-train
  condition discharged"; findings F1/F2/F3/F5/F6 traceable by
  content in the amendment landed in the same commit — including
  the `.gitattributes` asymmetry itself, recorded as State F1 [P3]
  at `:117-121`). What was never produced is the REPORT ARTIFACT —
  the D1+D2 package is the sole landing of that week with none —
  and `GwzM5-8A1ReleaseR1-ReviewState.md` is now that artifact.
  The true failure mode: the amendment recorded it, the searchable
  program surfaces (checkpoint, review inventory, plan) did not,
  so both the release plan and the R1 builder's grep-scoped search
  missed it. Decision ledger annotation per C1: decided A′, landed
  `9939b02`, reviewed-at-landing (report unfiled), re-reviewed and
  GO at `8e40fa8` (`GwzM5-8A1ReleaseR1-ReviewState.md`).]** The R1 package reduced
  to: the conformance audit (predicate matches the packet's four A′
  clauses; both recovery sites refuse pre-mutation, verified against
  the ref-transaction ordering; the clone-funnel disable_filters
  site correctly excluded as creation-time); ONE missing test built
  (+77, `checked_rollback_proceeds_when_the_configured_filter_is_
  outside_the_rewrite_set` — the rewrite-set-scoping sentinel,
  MUTATION-VERIFIED: widening the diff scope trips it); full gates
  green in the r1 worktree (g12 21/0, lib 1584/1, checker ok, zero
  pins moved). ONE REAL FINDING, empirically probed: the
  `.gitattributes`-ASYMMETRY RESIDUAL — the attribute stack is read
  pre-checkout (AttrCheckFlags::default), so when .gitattributes is
  itself in the rewrite set and only the TARGET side carries the
  coverage, the rollback proceeds and re-creates the exact wedge
  precondition A′ refuses (probe: ref moved, raw bytes under
  restored configured filter). Narrow (coverage must arrive WITH
  the checkout) but real; the builder correctly did not widen scope.
  **R1.2 FOCUSED STATE REVIEW DISPATCHED** on the landed object +
  the test package, with the asymmetry decision, the provenance
  question (was 9939b02's A′ share ever reviewed? if not this
  review is the owed one per packet §4 step 1), and five
  review-focus items in its mandate; report files as
  `GwzM5-8A1ReleaseR1-ReviewState.md`. ADJACENT: R1 observed
  Decision 1 B ALSO landed at 9939b02 (both pin edges +
  cfg(windows) doctrine sentinel; the windows-matrix un-pinned CRLF
  sentinel genuinely absent) — the R2 builder was RESCOPED
  mid-flight to audit-plus-gaps, not re-implementation. Plan
  correction pending both audits: R1.1/R2.1 were audit work, not
  build work; the release-train estimate shrinks accordingly.

  **R2 DELIVERED — 2026-08-25 — and the provenance finding
  hardened: NO FILED REVIEW EXISTS for `9939b02`'s D1 OR D2 share.**
  The commit message self-reports "State review GO"; the dev-docs
  inventory has a review file for every comparable package and none
  for filter-policy/D1/D2 — only secondary liveness checks (M5b
  settled §5 bookkeeping; A1 Completeness item 11 inheriting it).
  The packet-mandated focused State reviews were never filed; the
  R1.2/R2.3 reviews now running are the FIRST FILED REVIEWS of that
  landed production code, scoped to the whole objects. R2's package
  (r2-crlf worktree, 418 lines, ZERO production): the landed-B
  audit (pins content/placement correct; complete NINE-EDGE
  inventory of production worktree writers with per-edge coverage —
  the disable_filters/pins division of labour is by-design, gate
  G2's "filters-off at every edge" phrasing imprecise, restate at
  landing); funnel claim verified (all four clone sites route
  through; two notes: default-trait-impl inheritance for future
  backends, partial un-pinned dir on failed clone); THE THREE GAP
  TESTS (stash round-trip with live CONTROL arm — NEW EVIDENCE:
  the stash_save filtered-reset smudge measured ON MACOS, the
  exposure is non-Windows-only in practice; the creation-time-only
  STRUCTURAL GUARD, red-proven against an injected mid-life pin;
  the un-pinned sentinel test, red CRLF-vs-LF); and the
  `crlf-sentinel` windows-matrix job — TWO HALVES MUST DIFFER
  (pinned lane count-pinned green under hostile GIT_CONFIG_GLOBAL;
  #[ignore] expected-fail sentinel inverted — if it ever passes
  the lane fails loudly as vacuous), §17 topology audit recorded
  in-workflow, REHEARSED end-to-end locally (lane exit 0).
  Baseline moves +2 passed +1 ignored (1585/2i); checker green,
  zero pins. Pre-existing (proven on pristine 8e40fa8, not R2's):
  tests/protocol.rs 2 failures from local taut-proto!=0.8.1 (CI
  installs it). Annotation DELIVERY with a correction: the
  amendment's OPEN DECISION entries were ALREADY closed in-body on
  2026-08-16 — but the HEADER (:11-13) is STALE against its own
  body; four anchor texts (A-D) delivered for lane-owner
  application after R2.3's GO, incl. the tripwire-discharge and
  classification updates. **R2.3 FOCUSED STATE REVIEW DISPATCHED**
  (first filed review of the landed object + the gap package + the
  ten focus items + the four anchor texts; report files as
  `GwzM5-8A1ReleaseR2-ReviewState.md`; its item-7 ruling decides
  whether crlf-sentinel also rides release.yml via R3's G4).

  **R3 DELIVERED — 2026-08-25 (r3-release worktree, 6 files
  +106/−4).** Leg A (G3): generation
  `v0-v1-dual-decode-v0-writer-floor` for v0.11.0, shipped-behaviour
  description (v0+v1 decode; --no-ff→v1; ordinary v0, floor V0,
  M5c named); tuple_count CORRECTLY UNMOVED at 24 (generations add
  no reader rows — reasoning pinned in-comment beside the
  assertion); the brief's "manifest is unpinned" guess was WRONG
  and the verify-first order caught it —
  `evidence-macos-aarch64.json` `inputs.manifest_sha256` re-pinned
  (old 9b1af0e3… → dacdb187…), soundness argued (result set is
  reader-keyed; validate_result_set re-ran green) and flagged to
  review as the one possible rubber stamp. Leg B (G4): topology
  audit found `byte-equivalence`'s FIRST command IS
  check_m4_scenario_map.py — wiring the battery whole would have
  reproduced run 32749489320's sibling failure exactly; wired
  selection `fault byte-equivalence:2 unknown-field privacy`
  (~27 min measured upper-bound), the runner printing PARTIAL for
  the partitioned selector so the gap stays legible; call-graph
  EXCLUDED on verified redundancy (the existing boundary step runs
  its three commands verbatim; markers exit-code-equivalent; ~21
  min for zero signal, re-add trigger recorded); LINUX-ONLY on
  evidence (Windows counts differ by cfg-gating and were NEVER
  MEASURED — an unmeasured count pin is §17's lesson in a new
  dress); blocker comments extended to release.yml naming both
  workspace-root checkers, owner R2-F §11.3 item 7. Leg C (G6):
  version convention ESTABLISHED FROM HISTORY (crate == tag minus
  v, standalone 2-line chore commit; the v0.10.4/v0.10.5 anomaly is
  the narrow-branch release BY DESIGN — v0.10.4 not an ancestor of
  v0.10.5); 0.10.4→0.11.0 applied on the dev line (1+/1− each
  file); member proposal CORRECTS THE PLAN — gwz-cli `0.2.0-dev` /
  gwz-py `0.0.0` are dev placeholders the release-branch commit
  overwrites (version + path→git-tag re-point at the tag, operator
  R5 territory; gwz-py wheel version derives from the tag via
  dynamic versioning, no pyproject edit); full release-notes draft
  delivered with an R1/R2-reconciliation caveat. OUT-OF-PACKAGE
  G6 DEFECT found and FIXED by the lane owner: gwz-cli
  `docs/commands/merge.md:374-377` still claimed --no-ff "not yet
  available"/typed-unsupported and the synopsis omitted it —
  contradicting the shipped headline feature, uncovered by the
  docs checker's 147 assertions (gap recorded); fixed at gwz-cli
  `cf0d16d` (synopsis + accurate --no-ff section), g00 3/0 green.
  **[CORRECTED 2026-08-25 per the R3 review's [P1-1]: "uncovered
  … (gap recorded)" is FALSE. The checker's assertions COVER that
  prose with four required statements
  (merge_command_deferred_heading, …_ff_only_and_message_current,
  …_no_ff_deferred, …_no_ff_unsupported_is_typed) — they were
  PINNING THE STALE PROSE: the assertions should have moved at the
  `3000916` unhide and did not, and L2-05's non-wiring is why the
  red was never seen. The R3 builder's "checker passes so its
  assertions do not cover this" was true only of the pre-fix tree,
  and the lane owner propagated the negative claim into this
  record without verifying it at the pinning surface — the same
  failure class as the C1 correction above, same day. `cf0d16d`
  therefore leaves the cross-repo docs gate RED (4 findings) until
  the four assertions are updated in gwz-core's
  merge_docs_manifest.json — the prose is now TRUE and stays; the
  checker aligns in the R3 landing.]**
  **R3 INTERIOR SINGLE-AXIS REVIEW DISPATCHED** (the 8 focus items
  + the cf0d16d fix verification + the notes' claims checked
  against the tree; report files as `GwzM5-8A1ReleaseR3-Review.md`;
  its evidence-digest ruling is the load-bearing one).

  **R1.2 VERDICT — 2026-08-25: GO WITH CONDITIONS; GATE G1
  SATISFIED at `8e40fa8` + the R1 test package; the §4 forbidden
  ordering CLEARED — v0.11.0 may proceed past G1.** Report
  `GwzM5-8A1ReleaseR1-ReviewState.md` (0 P0/P1; 4 P2 = F-1
  checkpoint falsehood [corrected above, C1 DONE], F-2 the packet's
  fix premise is FALSE — git2 0.21/libgit2-sys 0.18.7 bind no
  git_attr_get_ext/INCLUDE_COMMIT, the amendment's "~15-LOC
  hardening" is unimplementable at this pin [C4], F-3 the
  recovery_support.rs:24-29 comment overstates the harm — the
  reviewer's own probe shows NO in-gwz wedge (libgit2 status runs
  no config-command drivers; realized harm = real-git-visible
  divergence, porcelain re-checkout remedy) [C2-notes; comment fix
  rides R6], F-4 the asymmetry itself; P3s: F-5 the Delta::Deleted
  exclusion is UNTESTED (mutation M2 flips nothing), F-8 plan
  :62-63 phrasing stricter than the packet's boundary).
  Adjudications: (a) **ACCEPTED AS A NAMED RESIDUAL** — already
  found at landing as State F1 [P3], not closable at the current
  dependency pin, mild realized harm, common direction over-refuses
  safely — SHIPS NAMED AND TRIPWIRED: C2 (verbatim residual text
  §3(a) → checkpoint register + G6 notes, by message shape not
  error code), C3 (doctrine sentinel in g12 per the house ritual,
  ~40 test-only lines converting the builder's probe, red means
  someone closed the residual and the frozen texts move), owner
  §R6 renormalize; (b) abort bound CONFIRMED over eight cases,
  guard-tying recommended; (c) FILE_THEN_INDEX intended,
  INCLUDE_HEAD does not exist in this git2; (d) lfs
  allowlist-by-name acceptable-with-record (CI fixture hermeticity
  load-bearing); (e) lock-before-preflight CONFORMS (the packet's
  boundary is set_target, not lock_ref). Reviewer's independent
  mutation matrix: M1 sole-detector confirms the builder's
  sentinel; M3/M4 each detected; M2 exposed F-5. Gates reproduced:
  g12 25/0, lib 1584/0/1, checker ok, zero pins. R1 LANDING
  PACKAGE: the +77 test + C3 sentinel + F-5 test (recommended,
  taken) in the r1-aprime worktree; C2/C4/F-8 dev-docs
  applications at landing (lane owner).

  **R2.3 VERDICT — 2026-08-25: GO WITH CONDITIONS; GATE G2
  SATISFIED at `8e40fa8` + the R2 gap package (0 P0/P1 · 3 P2 ·
  9 P3).** Report `GwzM5-8A1ReleaseR2-ReviewState.md`. The landed
  B code correct AS NEW: pins the right two keys repo-local
  (verified against libgit2 level ordering), STRUCTURALLY unable to
  go mid-life (no_reinit(true) makes re-pinning an existing repo an
  error; RepoBuilder::clone materializes before config can exist —
  disable_filters is the only pre-pin-window mechanism);
  pub(super) holds; the 9939b02 test proven non-vacuous by the
  reviewer's own mutation. FRAMING CORRECTED: the production
  worktree-writer inventory is TWELVE rows, not nine (the stash
  RESTORE edge stash.rs:31-61 was omitted — default
  StashApplyOptions ⇒ filters active, covered by birth pins), and
  "only the clone funnel is filters-off" is false (the two Clause A
  recovery edges are too). [P2-1] MEASURED DEFECT in R2's guard:
  function_slice terminates on literal "\n}\n" and silently
  falls back to whole-file on CRLF working trees — the guard
  degrades EXACTLY ON WINDOWS (proven: a real relocated-pin defect
  fails on LF, passes on CRLF); fix = the in-tree precedent
  r2d_seam_freeze.rs:219-223 (G2-c2). [P2-3] provenance hardened:
  the amendment CITES a review that does not exist (:168 "the
  landing review (F6)"; the :108-121 closure annotation) — the
  citations dangle even though R1.2 traced the review's findings;
  condition on anchor A. Fixture argument STRENGTHENED: libgit2
  ignores GIT_CONFIG_GLOBAL on gwz's open path (repository.c
  use_env gating) — an env-var fixture would have been INERT;
  repo-local hostile is the only correct form. CONDITIONS: G2-c1
  the sentinel lane executes at the RC (pinned half "6 passed",
  un-pinned red — R5 evidence item); G2-c2 the CRLF-normalization
  guard fix; G2-c3 anchors only with the §7 conditions; G2-c4
  restate gate G2's text (its literal "filters-off at every edge"
  would break the porcelain parity the amendment preserved).
  **ITEM-7 RULING: YES [P2-2], routed to R3/G4, before the tag** —
  the CRLF class must ride release.yml; PREFERRED FORM: convert the
  un-pinned sentinel to #[should_panic] (rides every existing lane;
  pages when the residual is closed) leaving only the Windows-leg
  count-pinned exact-name step to add to release.yml's verify job
  (NOT a duplicate windows job). ANCHOR SIGNING: A signed subject
  to A-i (date closures to 9939b02; assert the traceable facts,
  not an unfiled review's existence) + A-ii (repair the dangling
  citations same pass); B unsigned text-unseen — §7 of the report
  carries the restatement the reviewer WOULD sign (use it); C/D
  signed subject to C-i (attribute-driven residual qualifier
  MANDATORY), C-ii (scope to gwz-BORN), C-iii ("proven
  non-vacuously off-Windows" — g11:48 has never run on Windows).
  ROUTED: R2 builder resumed for G2-c2 + the #[should_panic]
  conversion + windows-matrix adjustment + package-level P3s;
  the release.yml Windows count-pin step folds into R3's landing
  package on its review's return.

  **R3 REVIEW VERDICT — 2026-08-25: NO-GO AS FILED ([P1-1] + 3
  P2); G3 GO-with-conditions, G4 GO, G6-versions GO,
  G6-notes/companion NO-GO; with conditions 1-4 closed G3/G4/G6
  are satisfied — none require new code.** Report
  `GwzM5-8A1ReleaseR3-Review.md`. [P1-1] = the lane owner's
  `cf0d16d` broke the green docs gate (corrected in the R3
  delivery record above); cure = four assertion updates in
  gwz-core `merge_docs_manifest.json` + checkpoint correction
  [DONE]. [P2-1] the generation description omits that an
  ordinary v0 record MIGRATES to v1 on resume (whitelisted
  Finalizing+Normal, store/mod.rs:254-262 MayAdapt) — a v1 record
  can exist with NO --no-ff start; add the migration clause.
  [P2-2] the decode-generations register has a HOLE at
  v0.10.3/4/5 — v0.10.4/5 are materially NOT the v0.10.2
  generation (PRODUCTION_R3 {v0:true,v1:false}, typed
  Unsupported+required_wave, not record_unreadable); constrain
  the entry's stated meaning or add the missing generation.
  [P2-3] the notes' "v0.10.2–v0.10.5 → record-unreadable" claim
  is wrong for two of four releases and unevidenced for a third
  (harness pins only v0.9.2/v0.10.2); narrow or split it.
  MECHANICS ALL CONFIRMED: byte-equivalence:1 IS the m4 checker;
  the runner's failed-before-partial ordering verified by
  execution; the version convention holds at all eight tags;
  v0.10.5 lives on origin/hotfix/v0.10.5, NOT an ancestor of
  HEAD, carries NO code main lacks — v0.11.0 STRICTLY SUPERSEDES
  it (the notes should state this). THE EVIDENCE-DIGEST RE-PIN
  RULED SOUND, verified eight ways (reader-keyed result set;
  manifest deliberately not in EVIDENCE_SOURCE_NAMES), with
  [P3-7] narrowing the RULE: artifacts are embedded-not-rederived,
  so "readers didn't change" suffices only because the delta is
  confined to decode_generations. OUT-OF-SCOPE FIND, ROUTED TO
  THE R3 LANDING: **`release.py:527-530` HARD-CODES an AI
  co-author trailer into release commits** — violates the
  standing attribution order (settings enforce it only for the
  lane's own commits, not tooling-authored ones); v0.11.0 dodges
  via the pre-bump no-commit path, later releases would not;
  one-line removal rides R3 with this record. R3 BUILDER RESUMED
  with conditions 1-4 + the R2.3 item-7 release.yml Windows
  count-pin step + the trailer removal.

  **R1 IS LANDED — 2026-08-25, gwz-core `a6ef094` (exact-ref
  pushed, main 8e40fa8 → a6ef094; live tree fast-forwarded
  clean).** One file, +299/−0, test-only (`src/git/tests/g12.rs`):
  the rewrite-set scoping test (M1 sole detector), F-5's
  deletion-clause test (M2 sole detector, built + red-proven this
  round), and C3's doctrine sentinel
  `doctrine_sentinel_target_side_attribute_coverage_escapes_the_foreign_filter_gate`
  (g12:1211; pins TODAY'S behaviour — the rollback proceeds, the
  ref moves, raw bytes under now-live coverage, and
  `!status.is_dirty` pinning the F-3 fact that gwz's status cannot
  see it; 45-line doc comment carries the full ritual incl. the
  red-means-frozen-texts-move clause). Lane-owner verification at
  landing: diff sha matched insurance
  (`6a915aa3…`), checker green, g12 23/0 re-run. Builder gates:
  lib 1586/0/1, clippy from clean. Zero pins. RECORD APPLICATIONS
  LANDED WITH THIS ENTRY: C4 (packet :269-271 bracketed
  correction — get_attr wraps git_attr_get, target-tree attribute
  read unimplementable at this pin; amendment :117-121 correction
  — the "~15-LOC hardening" unachievable as written, F1 accepted
  as named residual with the sentinel + R6 owner), F-8 (plan R1.1
  boundary phrasing aligned to set_target). **C2 — THE RESIDUAL
  REGISTER ENTRY, verbatim per R1.2 §3(a):**

  > **A′ NAMED RESIDUAL — target-side attribute coverage
  > (`.gitattributes` asymmetry).** The Decision 2 A′
  > foreign-filter refusal (`refuse_foreign_filtered_rewrites`,
  > `src/git/gitbackend/recovery_support.rs`:46-95) reads the
  > *pre-checkout* attribute stack (`git2::AttrCheckFlags::default()`
  > = `FILE_THEN_INDEX`). When `.gitattributes` is itself inside
  > the rewrite set and the foreign `filter` coverage exists ONLY
  > on the target side — i.e. the recovery checkout restores the
  > coverage together with the bytes — the gate does not fire: the
  > recovery-grade rollback or abort proceeds, the ref moves, and
  > the covered path is left holding raw blob bytes under a
  > now-active configured clean driver. The reachable harm is
  > divergence visible to real `git` on those paths (gwz's own
  > libgit2-based status cannot see it — it does not run
  > config-command drivers; see F6,
  > `GwzM5-8ExactEvidencePlatformAmendment.md`:167-172); the
  > remedy is a porcelain re-checkout of the affected paths.
  > **[Precision, 2026-08-25, per the R4 review's [P0-1] executed
  > evidence: "porcelain re-checkout" must be read as FORCE
  > re-materialization — delete the affected paths, then
  > `git checkout -- <paths>`. A bare `git checkout --` is
  > silently insufficient: after the filters-off rewrite the
  > index carries the raw file's stat, `git status` prints
  > nothing, and the checkout skips the paths as up-to-date while
  > the divergence persists. The runbook carries the working
  > sequence; the g12 doctrine sentinel's doc comment saying
  > "porcelain re-checkout" gains the same precision in the next
  > gwz-core train.]**
  > Reaching it requires the coverage to arrive WITH the checkout
  > (e.g. rolling back across a merge that deleted the covering
  > `.gitattributes`); the common direction — rolling back a
  > change that ADDED coverage — over-refuses and is safe. **Not
  > closable at the current dependency pin:** reading attributes
  > from an arbitrary tree needs `git_attr_get_ext` /
  > `git_attr_options.attr_commit_id`, which `libgit2-sys
  > 0.18.7+1.9.6` does not bind and `git2 0.21`'s
  > `AttrCheckFlags` does not expose. First recorded as State F1
  > [P3] at landing (`9939b02`, 2026-08-16); re-confirmed
  > empirically and re-accepted at R1
  > (`GwzM5-8A1ReleaseR1-ReviewState.md` §3(a)). Owner: the §R6
  > `gwz repair --renormalize` package, which already shares this
  > predicate and is the natural place to harden it.

  C2's user-facing leg (the release notes, by message shape not
  error code, per F6's mechanism not the stale code comment)
  lands with the notes finalization in R5. G1 IS FULLY DISCHARGED.

  **R4 LAUNCHED — 2026-08-25, on R1's discharge (branch (i),
  docs-only).** The runbook builder drafts the v0.11.0
  wedge-and-refusal runbook in a gwz-cli worktree at `cf0d16d`:
  classes A (A′ refusal, by message shape), B (the named residual,
  porcelain re-checkout), C (adopted-CRLF availability), D (the
  §4.2/§4.4 quarantine + manual surgery made documented), E (LFS
  pointer bytes), F (stop-and-collect-evidence); truthful to
  `a6ef094`/`cf0d16d`, no tooling promised, escape design stays
  DRAFT and un-referenced in user text. Single-axis review next.

  **R3 CONDITIONS 1-6 APPLIED — 2026-08-25 (r3-release worktree,
  now 8 files +194/−19; all gates green incl. docs gate restored
  to "ok (11 sources, 147 assertions)").** [P1-1] closed with the
  COUNT HELD AT 147 — the figure is settled-tree evidence carried
  in 12 dev-docs files, so merge_command restructured to 3
  required + 1 FORBIDDEN (regex verified both directions: misses
  the true prose, CAUGHT the pre-fix stale sentence — the class
  cannot return silently). [P2-1] migration clause added
  (whitelist-eligible Finalizing/Normal v0 rows migrate on
  resume/abort — v1 records without --no-ff, stated). [P2-2]
  closed via option (b) with tag-level re-verification: v0.10.2/3
  carry ZERO record_wire files (no dispatcher at all!); two new
  generations (v0-strict-envelope-typed-unsupported{,-narrow})
  added; evidence digest re-derived AGAIN (→ b35699c4…); tuples
  still 24, L2-04 86. [P2-3] the older-reader table rebuilt with
  per-row evidence tiers (harness-pinned vs source-derived) —
  v0.10.4/5 give the BEST diagnostic of any older release. Item 5:
  the Windows count-pin step added with the count DERIVED from
  source (5 plain + 1 cfg(windows) = 6), FIRST-DISPATCH-EXPECTED,
  source-presence guard closing the libtest zero-match vacuity;
  goes live when R2 lands (skip-safe until). Item 6: the
  release.py Co-Authored-By trailer REMOVED with an in-comment
  rationale; repo swept, no other occurrence. Two notes
  corrections forced by evidence: the refusal is NOT a new error
  code (DirtyMember + message shape, matching R1.2 §2.5) and
  CRLF is NOT Windows-only (R2's macOS measurement). Base moved
  to a6ef094 — zero overlap with R3's 8 files, applies cleanly.
  LANDING ORDER: R2 → R3 (makes the count pin live at R3's
  landing) → R4.

  **R2 IS LANDED — 2026-08-25, gwz-core `bed072a` (exact-ref
  pushed, main a6ef094 → bed072a; live fast-forwarded).** Four
  files +513/−4 (one production touch, comment-only:
  repository_support.rs precedence ordering per [P3-4]): the
  G2-c2 guard fix (LF-normalized include_str! inputs per the
  r2d_seam_freeze precedent, BOTH failure modes loud — the
  relocated-pin mutation detected under both encodings, CRLF
  false-positive control green); the sentinel converted to
  #[should_panic] per item-7 (class-death probe proven: fails
  loudly when the fixture's smudge source is disabled); the
  crlf-sentinel job reshaped (both halves green-when-correct;
  anti-vacuity on the two exact counts); [P3-1] is_test_source
  cfg(test) fallback deleted; [P3-2] guard asserts both pinned
  keys; [P3-5] closed by the conversion; [P3-9] → R2-F backlog.
  Inventory RECONCILED: the reviewer's TWELVE-row table (E1-E13)
  is the record — E13 (stash_apply/pop restore, filters active,
  covered by birth pins, exercised g11:94-115) was missing and
  the "only the funnel is filters-off" prose was wrong. Landing
  verification: diff content byte-identical to insurance (stat
  block appended explains the sha delta); combined-tree overlay
  gates at a6ef094 green (git::tests 142/0, checker ok, clippy
  clean); zero pins. Full lib at package base 1586/0/1 (the
  sentinel leaving #[ignore] moves 2i→1i). **THE ANCHORS ARE
  APPLIED** under the signing conditions: amendment header
  status-correction [A-i], the dangling-citation correction
  [A-ii], the tripwire discharge [C-i..C-iii + should_panic +
  E13 cite], the classification update [same], and gate G2's
  text restated from the reviewer's own signable form [G2-c4].
  **G2-c1's RC obligation, corrected wording: at the RC the
  crlf-sentinel lane must record pinned half `ok. 6 passed; 0
  failed` AND sentinel half `ok. 1 passed; 0 failed` (no failing
  half — the invariant moved into the counts).** G2 IS FULLY
  DISCHARGED up to the RC-execution evidence item. R3's staged
  Windows count-pin step is now LIVE-able (the six names exist on
  main).

  **R3 IS LANDED — 2026-08-25, gwz-core `07e1ac1` (exact-ref
  pushed, main bed072a → 07e1ac1; live fast-forwarded). R1+R2+R3
  ARE ALL ON MAIN.** Eight files +194/−19: the v0.11.0 generation
  with the resume-migration clause + the two tag-verified
  register-hole generations (digest re-derived, tuples hold 24);
  the docs gate re-pinned to post-A1 truth at the held count 147
  with the forbidden-regex regression guard; release.yml's
  Linux-only battery step + the Windows CRLF count-pin step (LIVE
  now that R2's six names are on main; FIRST-DISPATCH-EXPECTED);
  version 0.11.0 per the tag-history convention (--locked green);
  the release.py Co-Authored-By trailer REMOVED. Overlay gates at
  bed072a all green (checker; docs gate against live gwz-cli
  cf0d16d; harness 86/OK; L2-04 24/24; check --locked; fmt;
  YAML). Lock captured to `07e1ac1`. Push CI at the landing sha
  under watch. GATES G3/G4/G6 DISCHARGED per the R3 review's
  close ("with 1-4 closed, G3, G4 and G6 are satisfied for
  v0.11.0"). REMAINING TO THE TAG: R4 (runbook draft in flight →
  single-axis review → gwz-cli landing), the notes FINALIZATION
  (R3's corrected draft + C2's user-facing leg + the runbook
  pointer — reconciliation caveats now resolvable against landed
  surfaces), then R5: member pushes (operator), three-arm matrix
  at the RC with G2-c1's sentinel-lane evidence (pinned `6
  passed` + sentinel `1 passed`) and the T-5 pair regeneration on
  its §11.2 carrier, the release record — and THE TAG (operator).

  **R4 REVIEW VERDICT — 2026-08-25: NO-GO as drafted; six local
  edits convert to GO; the page's structure, what-was-mutated
  discipline, and message-first diagnosis "the best this program
  has produced — ship unchanged" apart from the edits.** Report
  `GwzM5-8A1ReleaseR4-Review.md`. TWO P0s FOUND BY EXECUTION, not
  reasoning: [P0-1] the porcelain-re-checkout remedy (class A
  follow-up + class B's ONLY remedy) FAILS SILENTLY AND CERTIFIES
  ITS OWN SUCCESS — after a filters-off rewrite the index carries
  the raw stat, status prints nothing, `git checkout --` skips as
  up-to-date, and the runbook's "expect no output" verification
  passes on the unrepaired worktree (proven on size-preserving AND
  size-changing filters); the working form is delete-then-checkout
  (force re-materialization). PROPAGATED beyond the page: the
  release notes corrected, the C2 residual register text given a
  bracketed precision note, and the g12 doctrine-sentinel doc
  comment queued for the same precision in the next gwz-core
  train. [P0-2] the class-C paste block DESTROYS a mid-conflict
  member — `git rm --cached -r .` refuses non-zero but the
  `reset --hard` on the next line runs anyway, wiping hand
  resolutions, MERGE_HEAD, and stages; the fence must be in-block
  and fail-stop. P1s: the class-C block ineffective for the
  attribute-driven triggers its own section names; the armed
  intermediate state (clean -fdx becomes whole-tree delete)
  unexplained; "ordinary merges are not affected" FALSE (the
  migration truth again — Normal Finalizing v0 rows migrate on
  abort and reach the comparator). RULINGS: force-abandon silence
  WRONG — one sentence added ("there is deliberately no
  force-abandon; parked IS the end state"); the sidecar-less
  restore SHIPS with two instruments (literal diff vs the
  evidence copy; `ls .gwz/merge/*.yaml` empty-check); the
  `quarantine/` DIRECTORY name ships ([Q7] is open over CLI
  surface only; never name a flag; "park" settled as the verb).
  Confirmed true by the reviewer: class D invisibility, the gate
  sets exact, all message texts byte-exact, the v1_rollback
  bound, indexing ships as-is, g00 3/3 and the docs gate 147
  green WITH the R4 edits. R4 BUILDER RESUMED on the edit list.

  **R4 IS LANDED + THE PRECISION MICRO-TRAIN — 2026-08-25. THE
  v0.11.0 TRAIN'S BUILD PHASES ARE COMPLETE.** R4: gwz-cli
  `6b7e75a` (local; docs/MergeRecovery.md 665 lines/4,121 words +
  4 indexing files, +692 total) — the review's two executed P0s
  fixed and RE-EXECUTED on three fixture shapes (the
  delete-then-checkout remedy with touch-sweep detection since
  status is blind; the class-C block now && -chained and
  guard-led, proven to halt at the refusal even when the human
  skips the guard; the attribute-vs-config router; the
  force-abandon closing sentence; the parked-restore instruments;
  "park" the verb, `quarantine/` the directory); g00 3/3;
  links/anchors validated; landing verification: page sha
  byte-exact vs insurance (166555d9…), diff +27 across the 4
  indexing files. Micro-train gwz-core `a6ce8a8` (exact-ref
  pushed, 07e1ac1 → a6ce8a8): R4 [P2-5] — the recovery guide is
  now a docs-gate SOURCE pinning the recovery-path bound
  paragraph (so M5c cannot silently invert the page); the marker
  moved to the EXECUTED count "ok (12 sources, 155 assertions)"
  (the derived guess of 148 was wrong — a new source inherits the
  7 global forbidden; the gate's own output is the pin source,
  today's lesson applied); the g12 sentinel comment's remedy
  corrected per [P0-1]. Docs gate/checker/fmt/g12 green at the
  landing. Lock captured (core `a6ce8a8`, cli `6b7e75a`, py
  `3d19dcd`). NOTE: the settled "147 assertions" figures in
  historical records remain true OF THEIR TREES; 155 is the
  figure from `a6ce8a8` forward. GATES: G1-G4 + G6 DISCHARGED;
  G2-c1 and G5 are the RC evidence items. **REMAINING = R5
  ONLY:** operator pushes gwz-cli (`6b7e75a`) and gwz-py
  (`3d19dcd`); three-arm matrix at the RC (first Windows
  execution of the CRLF proofs + the count-pin confirmation +
  G2-c1's two-half sentinel evidence); the T-5 pair regeneration
  on its §11.2 carrier; the release record; THE TAG (operator;
  release.yml verify green on both legs at the published tag).
  The notes (`GwzReleaseNotes-v0.11.0.md`) are FINAL as of the
  P0-1 correction, pending only the RC evidence stamp.

  **THE v0.11.0 RC EVIDENCE IS COMPLETE — 2026-08-25. ALL FOUR
  RC-HEAD RUNS GREEN at `a6ce8a8`** (Platform `32802563914`,
  Windows `32802562170`, boundary `32802495545` with the wired
  batteries, retained-readers `32802495544`; push CI green at
  every train commit). **G2-c1 DISCHARGED with both verbatim
  tails** (pinned `ok. 6 passed; 0 failed`; sentinel
  `- should panic ... ok` + `ok. 1 passed; 0 failed`). BOTH
  FIRST-DISPATCH CONFIRMATIONS HELD: the Windows count pin at
  exactly 6 (the cfg derivation confirmed on the runner) and the
  stash proof's FIRST WINDOWS EXECUTION green — the C-iii
  evidence caveat is CLOSED (born-repo stash closure now proven
  ON Windows). G1-G6 ALL DISCHARGED. Plan G5's T-5 clause
  corrected in place (over-eager; T-5 stays on the R2-F carrier).
  Release record filed: `GwzMergeCheckpoint-v0.11.0.md` (RC
  tuple, gate ledger, evidence verbatim, named residuals with
  owners, post-tag resume order; tag slot open). **THE TRAIN IS
  COMPLETE. REMAINING = THE THREE OPERATOR ACTIONS:** push
  gwz-cli `6b7e75a`, push gwz-py `3d19dcd`, tag `v0.11.0` at
  gwz-core `a6ce8a8` with `GwzReleaseNotes-v0.11.0.md` as the
  release body (+ the member release-branch commits per the R3
  proposal); release.yml verify then runs on the published tag.

  **THE TAUT-PROTO 0.9.1 BUMP — 2026-08-25, operator-directed
  ("v0.9.1 was released just now - we should test against that
  first"; option (b) chosen verbatim "b").** TESTED FIRST, all
  read-only: 0.9.1 regenerates gwz-core's three committed outputs
  + both corpora BYTE-IDENTICAL (scratch venv, tautc 0.9.1);
  gwz-py api.py byte-identical; gwz.ir.json additively enriched
  (per-interaction descriptors + unary entry) and green through
  codec load, drift check under the wheel, protocol tests 5/5,
  full suite 330/330. LANDED: gwz-core `8008bf6` (exact-ref
  pushed; 3 workflow pins + publish_workflow.rs tripwire +
  protocol/regen.py generator version; currency harness 29/29
  under a 0.9.1 TAUT_PYTHON — also clearing the pre-existing
  local red; retained-reader wheel refs DELIBERATELY UNMOVED,
  frozen reader bootstraps); gwz-py `6e1d52f` (local; generator
  guard + release-script pin + regenerated IR per the lock-step
  doctrine). FOOTGUN RECORDED, not fixed (operator's tool-design
  call): check_protocol_drift.py prefers the VENDORED ../taut/src
  over the installed wheel — in the dev workspace it compares
  against the taut dev head (987e4d14…, matching neither released
  wheel) and reports drift, while the release flow's temp
  worktree correctly checks the pinned wheel (OK d0c205c8…);
  contradicts regen_protocol.py's own anti-shadowing defense.
  RC RE-ESTABLISHED at `8008bf6`: matrices `32846951548` (win) /
  `32846954270` (plat) dispatched + push CI `32846765849`/
  `32846765913`, under watch; release record RC tables moved;
  lock captured (core `8008bf6`, py `6e1d52f`). The release
  scripts now run against 0.9.1 end-to-end (gwz-py release.py
  installs 0.9.1 in its flow). TRAILER-CHIP NOTE: the operator
  started the spawned "remove release.py trailer" session — that
  work already landed at `07e1ac1`; the session should find a
  clean tree and no-op.

  **THE VENDORED TAUT SNAPPED TO v0.9.1 — 2026-08-25,
  operator-directed ("../taut/src should be snapped at the v0.9.1
  release tag").** `git checkout --detach v0.9.1` → taut at
  `5cd26a1`; the old pin `f008419` proven an ANCESTOR — the member
  was simply BEHIND the release, which is why its IR export
  matched neither released wheel. CONSEQUENCE: the drift-checker
  footgun of the previous entry is DISSOLVED in practice —
  `check_protocol_drift.py` in the live workspace now prints OK
  `d0c205c8…` (vendored == wheel); the local-preference stays as
  designed and is harmless while the snap discipline holds.
  RITUAL: a taut-proto release bump has THREE legs — the pins
  (workflows/tripwire/guards), the regenerated gwz-py IR, and the
  member snap to the same tag. RECORD-KEEPING SLIP, corrected
  here: root commit `5120219` carries this entry's message but
  only the lock capture — the record edits had silently failed on
  a drifted cwd before it; this commit applies them, plus TWO
  omissions found in the same inspection: the four release review
  reports (`GwzM5-8A1ReleaseR{1,2}-ReviewState.md`,
  `GwzM5-8A1ReleaseR{3,4}-Review.md`) had been UNTRACKED since
  filing — cited everywhere, committed nowhere — and the root
  `Cargo.lock` still recorded gwz-core 0.10.4 (the 0.11.0
  coherence refresh rides here). Lesson: `gwz add` of nonexistent
  relative paths + a chained commit produced an Ok/Ok that
  committed something other than the named intent — inspect
  `git show --stat` after any commit whose preceding step
  errored.

  **THE v0.11.0 RC EVIDENCE IS COMPLETE AT `8008bf6` —
  2026-08-25.** Windows `32846951548` GREEN / Platform
  `32846954270` GREEN / boundary `32846765849` GREEN /
  retained-readers `32846765913` GREEN ON RERUN — its windows
  harness leg first failed `test_timeout_kills_descendant_process`
  (assertFalse(marker.exists()) — a descendant-kill race; ubuntu
  leg green; the bump's five-file diff provably does not touch the
  harness; the identical tree passed on rerun) — recorded as an
  environment flake with a hardening chip filed for the operator
  (bounded poll-until-absent instead of the immediate assert).
  ALL GATES G1-G6 STAND DISCHARGED AT THE FINAL RC. THE RELEASE
  IS HANDED TO THE OPERATOR: the three release scripts in order
  (gwz-core, gwz-cli, gwz-py — each `v0.11.0 --push`; core takes
  its already-at-version no-commit path; the member scripts
  reconcile release branches against the pushed mains and mint
  their tags), then the GitHub release from tag v0.11.0 with
  `GwzReleaseNotes-v0.11.0.md` as the body, which fires the
  release.yml verify on both legs.

  **v0.11.0 IS TAGGED — 2026-08-25. The three release scripts run
  by the lane on the operator's verbatim "go ahead and run the
  release scripts": gwz-core `v0.11.0`→`8008bf67b7` (main+tag
  atomic, full gate stack green in the script's standalone
  worktree incl. regen-venv-on-0.9.1 "committed protocol
  artifacts are current"), gwz-cli `v0.11.0`→`dec5e3bd47`
  (release branch reconciled+pushed), gwz-py
  `v0.11.0`→`f53a7c64c6` (lock verified pinning
  git+tag=v0.11.0#8008bf6). Member mains pushed (cli `6b7e75a`,
  py `5f6689a`). TWO EXECUTION INCIDENTS, cured+recorded in the
  release checkpoint: ENOSPC at the core gate (session cargo-cache
  bloat; cleaned 20GB, re-run green) and THE TRUNCATED-INVENTORY
  PIN MISS (maturin develop downgraded the venv to 0.8.1 from
  pyproject's runtime dep — one of four sites a head-truncated
  grep cut from the original inventory; fixed at py `5f6689a`,
  untruncated sweep clean; lesson: pin sweeps run untruncated and
  end with a zero-match residual grep). Release verify dispatched
  at the tag (`32946137285`, under watch). REMAINING FOR THE
  OPERATOR: publish the GitHub release page from tag v0.11.0 with
  `GwzReleaseNotes-v0.11.0.md` as the body. Post-release queue
  per the release checkpoint: R2-E first.

  **v0.11.0 IS PUBLISHED — 2026-08-26.** Three GitHub releases
  created together on the immutable tags (v0.10.5 convention;
  operator waived the redundant pre-publication CI wait — the
  release script's own gates were the full suite at the tag).
  Event machinery fired: cli dist (binaries/installers), py
  Publish (wheels), core event-verify (tag-copy 22.04 leg re-fail
  EXPECTED, recorded; aligned dispatch verify continues as extra
  evidence). Version lock verified before publication: all three
  tags v0.11.0; both member release branches pin
  gwz-core git+tag=v0.11.0. THE RELEASE PLAN IS EXECUTED
  END-TO-END. Remaining watch: the artifact pipelines' completion.
  Post-release queue: R2-E first, per the release checkpoint.

  **THE R2-E PLAN IS DRAFTED — 2026-08-26, on the operator's
  "draft the R2-E plan": `GwzM5-8R2E-Plan.md`.** Twelve-row
  obligation ledger consolidated from the charter sources (the §10
  conversion table, the 38 re-reserved keys — census CORRECTED
  from the checkpoint's unsourced "67" — the BINDING
  BarrierIntentV1::issue obligation, the archive/GC sub-package,
  the review-donated riders O9-O12, the DurableObjectIdentity
  reach question). Phases E0-E7: E0 freezes the object (reach
  traces + the §3.5 semantics amendment + dual #1), E1-E3 install
  the three families (parallel-friendly), E4 executes the
  conversion table row-by-row (incl. recover_or_create's first
  production caller gated by §11.3 and the O3 legacy-writer
  expiry), E5 the archive/GC evidence package, E6 the hardening
  riders, E7 the settled dual + three-platform acceptance + the
  ledger's row-by-row close. Two duals max, interior single-axis,
  gates-not-LOC scheduling. Three operator decisions open
  (parallelism width; the executable-template policy;
  quota-vs-schedule). First action on "go": E0.1's read-only
  reach traces.

  **R2-E IS OPEN — E0.1 COMPLETE, 2026-08-26 (operator "go").**
  The reach traces filed as `GwzM5-8R2E-E01ReachTraces.md`,
  read-only, at main `94da3e5`. (a) **DurableObjectIdentity is NOT
  production-reached in v0.11.0**: every merge mutation acquires
  the checked runtime bootstrap (WorkspaceMutatorLock wraps
  try_acquire_workspace_runtime — locks/dirs/revalidation ONLY, no
  probes), while every identity/rename-domain probe sits in the
  catalog lease behind catalog_mutation_lease() →
  recover_or_create = zero production callers; v1_lifecycle has
  zero non-test checked_artifact references (its checked store is
  its own durable_fs module). The 22.04 verify failure was
  fixture-only exposure, confirmed. **The exposure arrives at
  E4.1** — two bindings minted: the capability-refusal UX is an
  E4.1 HARD PRECONDITION, and the probe blast radius
  (every-mutation vs catalog-consuming-only) is an explicit E0.2
  amendment decision. (b) §11.3's activation gate restated row by
  row — incl. the ORDERING CONSTRAINT made explicit: E2.2
  (BarrierIntentV1::issue observe-or-refuse) strictly precedes any
  E4 row admitting roaming-anchor actions; the authority_name
  weigh is taken in E0.2 or explicitly deferred to its consuming
  E4 step. NEXT: E0.2 drafts the semantics amendment against this
  note; E0.3 dual #1 follows.

  **R2-E E0.2 DRAFTED — 2026-08-26; filed
  `GwzM5-8R2E-SemanticsAmendment-DRAFT.md` (1,392 lines); E0.3
  DUAL #1 DISPATCHED.** All 38 keys with per-key semantics and
  real sites (EXISTS/NEW@ convention); §5 blast-radius DECIDED
  option (ii) — probe only at catalog consumption (option (i)
  would refuse NINE production sites incl. repo creation that
  never touches the catalog); §3 O6 RESOLVED — OBSERVE both facts
  via an owner-minted RoamingAnchorHomeWitnessV1 + one typed
  REFUSE (anchor stranded), the Step-4.3 pattern, E2.2-before-E4
  quoted; §6 the O8 mechanism decided; §7 all five §11.3 rows
  accounted; twelve §4.3 annotation targets. **TWO BLOCKING OPEN
  ROWS — genuine design defects found by the drafting, same
  class (a completion predicate written before the catalog had a
  lifecycle): OPEN-B1 completed_record requires the roaming
  anchor RESIDENT (interior.rs:350) so the anchor cannot roam —
  a crash in the window leaves a catalog no process can retain;
  OPEN-T1 completed_record requires an EMPTY RetiredActions root
  (interior.rs:349) so the FIRST terminal retirement un-completes
  the catalog, contradicting MAX_RETIRED_ACTION_DIRS=64 and the
  occupancy retirement-credit model.** The dual is asked to rule
  them together as one precondition package. Also found: +19-line
  citation drift between R4b-G's contract cites and the current
  contract (the A1-era annotations shifted lines) — E5.2 inherits
  whichever numbers the record blesses; the dual verifies.

  **E0.3 STATE AXIS: NO-GO, ROUND-2 REMEDIABLE — 2026-08-26 (2
  P1 · 6 P2 · 8 P3; report `GwzM5-8R2E-E03-ReviewState.md`).**
  THE AGGREGATE CATCH, AGAIN, AND IT IS THE LANE'S: [P1-2] —
  E0.1(b) restated §11.1's five rows UNDER THE LABEL §11.3; the
  real §11.3 has eight items sharing none of them, and its item 1
  (SettledTuple:800-802) is the A1 COEXISTENCE GATE: no
  production catalog activation until the relocation lands —
  and relocation is PINNED TO R2-F (§11.2:791). E4.1's gate was
  lost in a three-hop handoff; "relocation"/"quarantine" appear
  ZERO times in the plan, E0.1, and the draft. [P1-1]: the
  injection-source file count wrong in two places AND
  self-disagreeing (truth: nine → TEN, one new file at E2; the
  draft says nine→ten as an E1.2 duty and nine→eleven in §8 —
  the freeze's own inventory-defect class recurring inside the
  object). CLEAN/SUSTAINED: the census machine-verified (165;
  38 rows exact order; zero minted/omitted/retired); the §5
  doctrine sustained on the axis's own nine-site enumeration
  (no corpus ruling binds fail-closed at acquisition; option
  (ii) is what makes "runtime bootstrap only" mean something).
  B1/T1 VERIFIED REAL independently — one defect class — with
  the process ruling: the dual rules, THE LANE AUTHORS E0.2b
  (one addendum, both cures: Class-2/C-3 mechanism, candidate
  (i) for B1), round 2 reviews it under the two-round cap;
  fallback = partial GO unblocking E1 only. The +19 drift
  adjudicated: commit `4b9f078`'s 19 insertions; drift is
  +19-below-the-annotation only; the draft's numbers RIGHT;
  stale cites in SIX documents; ruling = content-anchored
  citing, no re-pointing of dated records, one corpus-wide
  drift note naming the commit. REMEDIATION HOLDS for the Code
  axis (running) — merged round per house pattern; the E0.1(b)
  mislabel correction lands with it.

  **E0.3 CODE AXIS: NO-GO, REVISE-AND-REFREEZE — 2026-08-26 (3
  P1 · 8 P2; report `GwzM5-8R2E-E03-ReviewCode.md`). ROUND 1
  CLOSES NO-GO/NO-GO WITH BLIND CONVERGENCE ON THE DEFECTS AND A
  GROUNDED DIVERGENCE ON THE CURE.** Both B1/T1 derivations
  CONFIRMED independently pre-reading — and one level DEEPER:
  the chain terminates in `Ambiguous` (classifier→bootstrap),
  which has NO recovery arm — the failure is "no process can
  EVER RECOVER this catalog", permanent loss. Code P1s: (1) the
  draft's B1 mechanism (i) is UNIMPLEMENTABLE (completed_record
  sees bare ActionDigestV1s, no interiors; BarrierIntentV1
  carries no roaming-anchor identity field) — REFUSED; adopt
  (ii) COPY-NOT-MOVE in the hard-link shape, which touches
  completed_record/retain_file/require_named_file_identity NOT
  AT ALL; (2) DECISION T-B unimplementable (one observation of
  destination_dir reused; a retired-root interior can never
  satisfy completed_record — §8 would freeze the wrong arm-table
  resolution); (3) OPEN-C1 REFUTED at source — RecordScratch has
  ZERO write paths tree-wide (the authority scratch is
  AuthorityScratch): the feared sharing does not exist, C-3
  simplifies. P2 spine: each precondition package is THREE
  gates not one; the amendment's own key #12 destroys the
  catalog permanently under any move-based roam (reinforcing
  copy-not-move); O6 applies precedent parts (i)/(iii) but not
  (ii) — the callee's read side re-checks five facts and none of
  the three identities. VERIFIED CORRECT: the nine sites
  (counted), §5 whole, §6 + the drift catch, C-1, B-3 Windows,
  census-vs-green-fixture; "the drafter's citation discipline is
  the best I've audited here." PACKAGE RULING (diverges from
  State): RULE B1/T1 SEPARATELY — T1 is §4.4-Class-2-sanctioned
  observer-reading widening (three gates, census.retired
  evidence struck); B1's correct fix needs no widening at all.
  **MERGED REMEDIATION ADOPTED: the State axis's PROCESS (lane
  authors E0.2b; round 2 re-verdicts under the two-round cap)
  carrying the Code axis's CURES (B1 copy-not-move; T1
  three-gate Class-2; ruled separately inside the addendum),
  plus State P1-1 (file count 9→10 once, at E2), State P1-2
  (the REAL §11.3: E4.1 gated on the R2-F-pinned relocation —
  the addendum analyzes which E4 rows truly need catalog
  activation and PROPOSES the sequencing options; likely an
  operator plan-shape decision), O6 part-(ii) read-side
  identity checks, the T-B re-resolution, both axes' P2/P3
  lists, and the corpus-wide drift note naming `4b9f078`.**

  **E0.2b DELIVERED — 2026-08-26; filed
  `GwzM5-8R2E-SemanticsAmendment-E02b-DRAFT.md` (1,327 lines);
  ROUND 2 DISPATCHED to both axes.** All eight merged-mandate
  items dispositioned. ONE GROUNDED OVERRIDE of a round-1 ruling,
  flagged first for round-2 Code: the hard-link sub-shape of
  copy-not-move is MACOS-FATAL — the tree's own doctrine test
  (anchor/tests.rs:278-324,
  hard_link_identity_sharing_is_what_the_retirement_rows_assume)
  proves per-hard-link ATTR_CMN_OBJPERMANENTID re-homes the FIRST
  link's identity when the second is created; hard-linking the
  home would break completed_record on every subsequent
  observation. B-5 fresh-independent-copy adopted; the essential
  ruling (predicate untouched) survives; #12 no longer
  catalog-fatal. T-B′ = a NEW DestinationRecheckV1 variant in
  CLASS 1 (the freeze's enum criterion — E0.2 had applied the
  struct half); OPEN-C1 closed; O6 gains the read-side identity
  refusal AND a framing correction (the right precedent shape is
  the observe/retain inverse, not AuthorityFactsIssuerV1); counts
  nine→ten-once-at-E2; §11.1/§11.3 fully restated with verified
  boundaries. **THE SEQUENCING ANALYSIS: the catalog-free E4 set
  is EMPTY (every §10 row reaches the catalog via admission
  execution.rs:141 or managed bootstrap :199) — option (c)
  collapses; the A1 coexistence gate blocks ALL of E4. PROPOSAL:
  option (b), pull quarantine/relocation INTO R2-E as its own
  phase (relocation is catalog-free and self-contained; R2-F's
  deletion charter is DOWNSTREAM of it; leaving it cross-lane is
  circular via E4.7). This is a CHARTER CHANGE contradicting
  three standing assignments → the operator's ruling once round
  2 verifies the premise; fallback = option (a) taken explicitly
  with a cross-lane dependency row.** Three new OPEN rows (B7
  native-measure; B8; R1 BLOCKING for whichever lane owns
  relocation); B3 strengthened (catalog-legal leaf names).

  **E0.3 ROUND 2, CODE AXIS: GO CONDITIONAL — 2026-08-26 (1 P2 ·
  4 P3; §Round 2 appended to its report). THE OVERRIDE RATIFIED
  on independent re-derivation** — and found WORSE than the
  addendum stated: macos.rs:77/:88 makes the macOS durable
  identity BE ATTR_CMN_OBJPERMANENTID; the executed doctrine test
  pins per-hard-link allocation with second-link re-homing; even
  the retained-handle loop (completed.rs:204-217, fgetattrlist
  live from the vnode) breaks. The round-1 hard-link sub-shape
  WITHDRAWN; B-5 fresh-independent-copy DOUBLY RATIFIED (the
  addendum's second reason also verified: BarrierIntentV1 binds
  no alias-identity field — the sub-shape was over-stated even
  on Linux/Windows); the essential ruling STANDS (completed_record
  untouched; E2.1 authorized to edit no catalog predicate). All
  four Appendix-C items CONFIRMED (T-B′ Class 1 correct per the
  freeze's own enum terms, checker boundary-clean over all 1,254
  lines; T-C′ exactly one forward; read-side placement provably
  right — protocol/ never imports cap_std; rows #6-#13 no recheck
  arms). The reviewer withdrew one of its own round-1 findings
  (B-6 contamination cost) and downgraded another
  (retired-root bound safe only by two constants both being 64 —
  P3). **[P2-R1] the one condition: a THIRD machine-enforced
  inventory absent from all three convergence obligations —
  CATALOG_PUBLICATION_CALL_COUNTS (E3 moves 2→3, the seventh
  deliberate extension) and PROTECTED_SOURCE_TREE_DIGESTS (trips
  on EVERY converging commit; the reviewer reimplemented
  source_tree_digest and reproduced both pinned digests to
  confirm) — fold into §2.5/§3.6/§4.5 + four P3 clauses.**
  Awaiting State round 2 (the premise attack + census recount);
  on its verdict the [P2-R1] fold lands and the ripened
  sequencing decision goes to the operator.

  **E0.3 ROUND 2, STATE AXIS: CONDITIONAL GO — 2026-08-26 (2 new
  P2 · 4 new P3; §Round 2 appended). GO ON THE SEMANTICS OBJECT
  — both round-1 P1s cured at every hop, all six P2s dispositioned,
  CENSUS CLEAN over the amended pair (165, 38 accounted, zero
  minted; T-B′ mints an enum variant not a fault key, Class 1
  verbatim-correct), §11 restatements LINE-EXACT (no second
  labeling error), T-D survives its deletion — E1-E3 ARE
  UNBLOCKED. NO-GO ONLY on §7.6-§7.8 as operator
  decision-support:** the premise ruled TRUE BUT NOT COMPLETE —
  the axis attacked it four ways, could not break the
  catalog-dependency of any mapped row (re-testing the lock row's
  ordering clause at all five production sites), BUT §7.6 mapped
  EIGHT of the table's NINE rows. **[P2-R1]: the missing row
  (:280, "v1 checked store/root/bundle paths — test-gated until
  A1; no legacy raw writer") is the one whose gate ALREADY FIRED:
  v1_lifecycle's store is a PRODUCTION RAW WRITER post-A1
  (rewrite.rs:6 durable_fs; zero checked_artifact references) —
  catalog-free, currently unmet, absent from plan O1, owned by no
  step.** [P2-R2]: O3 has a catalog-free discharge route the
  table denies — the legacy private parent has EXACTLY ONE
  non-test owner (policy.rs:33-42 → observation.rs:93); relocate
  that one function and O3 is literally true with zero catalog;
  the blocked pair is O1+O2, not O1+O2+O3 — §7.6 understates
  option (b)'s benefit AND §7.8(a) overstates option (a)'s cost.
  Ground 4 HOLDS, strengthened to breakable circularity. BOTH
  ROUND-2 VERDICTS ARE CONDITIONAL-GO CLASS; the conditions are
  the reviewers' own prescriptions = lane-owner folds, not a
  third round (the two-round cap holds). FOLD DISPATCHED to the
  drafter: Code's two-inventory convergence fold + 4 P3; State's
  nine-row map with row :280's NEW OBLIGATION surfaced for
  ownership (plan O13 candidate), the O3 route correction, the
  census-clause verbatim fix, the family-scope cite. THE
  SEQUENCING DECISION REACHES THE OPERATOR ON THE CORRECTED MAP.

  **THE E0 FOLD IS COMPLETE AND VERIFIED — 2026-08-26; the
  addendum re-filed at 1,746 lines (FINAL DRAFT, both round-2
  verdicts named).** All eleven conditions applied and
  lane-verified (the two inventories in all three convergence
  obligations with the arithmetic shown — thirteen publication
  call sites today, E3's the fourteenth and fifth dated
  extension; the NINE-row §7.6 map with row :280 and §7.6.1's
  O13; §7.6.2's O3 correction; O1+O2 as the true blocked pair;
  the seven verbatim record headers with "none retired"; the
  T-D family-scope closure). O13 OWNERSHIP ANALYSIS: split — the
  substantive conversion RIDES E4.2/E4.3 as a scope clause (same
  store, two-steps-one-store drift avoided); the "no legacy raw
  writer" half is a PIN landing at the next opportunity + a
  dated accepted-residual record covering the A1-to-E4.2
  interval ("not silence, which is the status quo"). THE
  SEQUENCING DECISION IS RIPE, on six grounds with the corrected
  accounting: option (b) pull relocation into R2-E (ground 4
  structural and BREAKABLE — the legacy private parent's single
  non-test owner makes relocation a single-owner legacy-only
  edit discharging O3 outright, handing the lane the whole
  catalog-free critical path in one run) vs option (a) explicit
  cross-lane dependency on O1+O2 with O3 re-owned and E4
  rescheduled. PRESENTED TO THE OPERATOR. On the ruling: the E0
  LANDING (freeze application + plan reshape incl. O13 + the
  drift note, one coordinated train), then E1-E3 launch.

  **THE OPERATOR RULED: OPTION (a) — 2026-08-27, one-line reply
  verbatim "a".** Relocation STAYS R2-F's; the addendum's option-(b)
  pull-forward proposal is DECLINED. The §7.8 fallback executes in
  full at `GwzM5-8R2E-Plan.md` §1.1: the CROSS-LANE DEPENDENCY ROW
  (O1+O2 blocked on R2-F's quarantine/relocation package); O3
  recorded DISCHARGED-BY-THAT-PACKAGE-AND-RE-OWNED, not blocked;
  Phase E4 re-scheduled after it (E1-E3 and E5.1/E5.2 NOT gated —
  the addendum §7.6.2's reliefs); the E7 close form set (O1/O2
  close *re-owned with a named carrier* if relocation hasn't landed
  by then, O1's close carrying §10 row :280); O13 MINTED with its
  split ownership; OPEN-R1 ROUTED TO R2-F with the relocation
  package (blocking for that package's owner).

  **THE E0 LANDING TRAIN EXECUTED — 2026-08-27. R2-E E0 IS CLOSED;
  E1-E3 ARE LAUNCHED.** (1) FREEZE APPLICATION:
  `GwzM5-8R2DInterfaceFreeze.md` gained the THREE §3.5 activation
  records (the addendum [P2-4] cure's final headers, verbatim, each
  with a dated filing note naming the amendment pair) and the ELEVEN
  further E0 annotations (E0.2 §8's rows as amended: E7 with the
  T1-widening authorization; E12/E13 C-1; E14 B-3; the E10/E14
  ground gaining the borrow case under B-5; P5's three Windows arms;
  Class 1's last row resolving to T-B′; Class 2 naming T1's widening
  its precedent and B-5 needing NO sanction; the E4-retire clause
  scoped; E22's reclamation DECLINED; the inventory nine→ten-at-E2
  statement; the total-165 restatement) — grep-verified 3 records +
  11 annotations, table rows intact, freeze 1,798 → 1,950 lines.
  (2) PLAN RESHAPE: status ADOPTED; §1.1 E0 amendments block (O13
  with the DATED ACCEPTED-RESIDUAL RECORD covering A1→E4.2/E4.3;
  the cross-lane row with the ruling verbatim; O3/O8/O11/O6/O12
  rows; E6.3 VOID; OPEN-C1 STRUCK); the E4 gate note (SEVEN
  preconditions; the load-bearing "runtime bootstrap only"
  statement; E2.2-before-E4 ordering); E4.2/E4.3 O13 scope clauses;
  §5 items 1/3 exercised. (3) AMENDMENT PAIR: both statuses →
  LANDED (addendum controlling); §7.8 carries the dated RULED line.
  (4) THE O13 RAW-WRITER PIN LANDED — gwz-core `8597d32`
  (checker-only micro-train, authorized by the dual-reviewed
  addendum §7.6.1 "landing now"): `V1_LIFECYCLE_RAW_DURABLE_WRITER_FILES`
  pins the complete executed non-test `durable_fs` surface
  (archive.rs, store/archive.rs, store/rewrite.rs — the addendum
  cited one file; the executed sweep found THREE, pinned all),
  set-equality BOTH directions, both probes executed RED in the
  overlay before commit (growth → "gained a raw durable_fs writer";
  stale entry → "must be retired deliberately"), checker green on
  pristine+pin, exact-ref pushed 94da3e5..8597d32. [CI CLOSED
  2026-08-27: Checked-artifact boundary run 33032497893 SUCCESS
  (~39m); Retained merge readers 33032497890 SUCCESS (8m15s) —
  both push legs green at 8597d32.] (5) E1-E3 LAUNCHED IN PARALLEL (three Opus builders,
  isolated gwz-core worktrees e1-cleanup/e2-barrier/e3-terminal,
  briefs binding the amendment pair addendum-controlling, no-push
  no-tags no-trailers; lane owner lands sequentially with pins
  re-executed at each landing) — §5 item 1 exercised per the
  standing recommendation; operator may override. NEXT: builder
  reports → interior reviews (E1.3/E2.3/E3.2) → sequential
  landings; the freeze inventory-addendum "nine"→"ten" edit rides
  E2's landing commit.

  **E1+E3 TRAINS DELIVERED; E3 ROUND 1 NO-GO/ESCALATED; FABLE
  RULINGS ISSUED — 2026-08-27.** E1 (`4a0b01a`, one commit, rebased
  onto 8597d32): eleven cleanup.* sites + [Fault; 11] matrix green
  both variants, checker suite 69/69 OK, no count move; flags: C-2's
  ~250-line fallback tripped at 285 lines and deliberately NOT taken
  (taking it moves FAULT_INJECTION_SOURCES 9→10 at E1 against
  controlling addendum §6.1 — the pair's one internal inconsistency,
  reviewer adjudicates); the 16 KiB alias-retirement bound stated
  (E4 input); Linux marker 418 derived, landing re-measure. E3
  (`b9cb795` precondition + `04c8a66`): T1 widening at exactly three
  gates, T-B′/T-C′/T-D landed, matrix [Fault; 10] green both
  variants. LANE-OWNER CROSS-CHECK EXECUTED AND PASSED: E1's
  completion (classify==Complete: live Missing + retired
  Exact-fingerprint) and E3's terminal precondition (live gone,
  retired resident, per bound worklist row) implement the IDENTICAL
  slot pairing; E3's exactly-one-of-two-homes keys dissolve the
  §2-vs-§4.3 ordering tension E1 flagged; nuance recorded: E3
  residency-level vs E1 fingerprint-level (see F3). E3 REVIEW
  (GwzM5-8R2E-E3-Review.md, filed with this commit): NO-GO, 1 P1 /
  2 P2 / 5 P3. [P1 F1] the widening's observe_slot↔observe
  re-entry is DEPTH-UNBOUNDED — reviewer REPRODUCED stack-overflow
  SIGABRT at nested retired-actions-v1 depth 700 (200 → typed
  refusal), reachable from recover_or_create; contradicts the
  authorization's 'bounded reading'. [P2 F2] the forced
  source-interior arm (train deviation D-1) — FABLE RULING:
  existence forced, shape minimal-acceptable; freeze §4.4 arm table
  GAINS THE ROW as a dated annotation at landing (lane owner's
  edit); second-axis scrutiny rides dual #2 (E7.1) per the
  deferred-escalation design. [P2 F3] §4.3 rows #2/#3 announce
  fingerprint-depth binding the code does not perform (residency
  only; matrix green DEPENDS on fixture's fabricated fingerprints)
  — FABLE RULING: disclosure-integrity class; remediation must
  bind-what-the-row-says (real fingerprints in fixture; classify at
  #3 if right-layered) OR amend the rows' announced semantics as
  dated determinations (T-D's honesty pattern), never keep the gap.
  REMEDIATION ROUND DISPATCHED to the E3 builder (round 2 = final
  under the two-round cap): F1 fix = dedicated single-level bounded
  retired-root reader, no re-entry, plus the nested-chain refusal
  row at would-have-overflowed depth; F3 per ruling; P3s F4-F7 per
  report; F8 (FIVE machine-enforced inventories, not three) is the
  lane owner's records duty. E1 review in flight; E2 builder in
  flight.

  **E1 IS LANDED — 2026-08-27; gwz-core main 8597d32 → `4a0b01a`.
  THE CLEANUP FAMILY IS EXECUTED: 11/11, matrix-green both target
  variants.** Review GO (GwzM5-8R2E-E1-Review.md, six P3s, none
  blocking; the C-2-vs-§6.1 conflict RULED for the train — all
  eleven sites stay in namespace_mutation.rs at 697 lines, under
  the 1,000-line program trigger; the fallback would have forced a
  false 9→10 inventory move at E1). Landing ritual executed by the
  lane owner in a pristine worktree at `4a0b01a`: checker ok, fmt,
  check, clippy, and the three disjoint lib partitions with DIRECT
  exit codes (0/0/0) — 408 checked_artifact / 932+1 remainder /
  256 v1_lifecycle, matching the commit's own darwin pins (the
  first partition run piped cargo through tail and was DISCARDED
  as evidence — the piped-gate class, caught by its own blank
  output). Exact-ref push. MATRIX DISPATCHES OPEN at `4a0b01a`:
  Windows matrix 33039208104 + Platform matrix 33039209950 (the
  §11.1 row-d ten-writer-rows native ledger debt discharges on
  their green; E1's derived Linux fault marker 418 measures on the
  Linux leg), plus push CI (boundary 33039190896, retained readers
  33039190893). E1 residuals owned at this landing: the 16 KiB
  alias-retirement bound is an E4 input; the two-place bound
  constant and the docstring self-contradiction (E1 review F1/F3,
  P3) queue for the next train touching those files. E2 DELIVERED
  (three commits, full lib 1602/0/1, O6 witness with wire surface
  zero-diff, OPEN-B2/B3/B8 answered, B-4 grounds correction
  flagged) — review in flight. E3 REMEDIATION DELIVERED
  (`a31e118` + `7f23484`: F1 fixed structurally with driven
  depth-1024 refusal rows; F3 row #3 binds the real classifier
  over real observed fingerprints, row #2's dated determination
  taken for the freeze record; F5 cured by observation; F6
  deviation eliminated) — round-2 re-verdict in flight. Host
  hygiene note minted by the E3 lane: killed runs of the
  deep-chain test leave temp fixtures plain `rm -rf` cannot
  delete on macOS (fts depth give-up); clear with an O(depth)
  lift-and-remove walk.

  **E3 IS LANDED — 2026-08-27; gwz-core main 4a0b01a → `1d50e59`.
  THE TERMINAL FAMILY IS EXECUTED: 10/11 PartiallyExecuted with the
  T-D determination; THE T1 WIDENING IS LIVE at its three gates.**
  Round-2 review GO (report's Round 2 section filed with this
  commit): F1's cure re-verified by the reviewer's own probes
  (typed refusal at depth 3000; 64 legitimate rows still recover —
  no off-by-one in the refusing direction); F3 cured both halves
  (row #3 binds the real classifier over production-observed
  fingerprints, length-bounded by the bound worklist row's own
  recorded field; row #2's dated determination taken); checker
  suite 69/69. LANDING TRAIN: the four e3-terminal commits
  cherry-picked onto landed E1 (conflict resolutions = unions;
  pin conflicts deliberately deferred) + the reconcile commit
  `1d50e59` re-executing every pin on the final tree (pre_catalog
  digest 4e942331 recomputed TWICE — the F9 comment fix moved it
  after the first computation, the stale-digest class caught
  in-flight; darwin partition EXECUTED 425 = 400+8+17; linux 435
  derived FIRST-DISPATCH-EXPECTED; F9's allow-reason corrected).
  Gates at the reconciled tree, direct exits: checker ok, fmt,
  check, clippy, 425/932+1/256 (lib total 1615 accounted).
  LANDING CONDITIONS DISCHARGED IN THIS COMMIT: (a) freeze §4.4
  arm table gains the terminal source-interior row (dated
  annotation; E7.1 second-axis scrutiny); (b) the terminal
  activation record carries row #2's replacement determination
  and row #3's strengthened form; (c) the addendum §6.2 census
  corrected to FIVE machine-enforced inventories (catalog.rs
  tree; the capability_permit Rust twin); plus the B-4 grounds
  clause corrected in the addendum per the E2 review's ratified
  finding. Matrices dispatched at 1d50e59: Windows 33040773113,
  Platform 33040774620 (these measure linux 435; the E1-tip
  dispatches 33039208104/33039209950 still complete for E1's own
  ledger entry). E2 REMEDIATION IN FLIGHT ([P2-1] the Windows
  roundtrip-orphan converge arm unreachable from the drive path —
  reviewer-traced, Fable-confirmed; fix + truthful residual
  restatement; seven P3 dispositions). Host-hygiene note: killed
  deep-chain test runs leave temp fixtures plain rm -rf cannot
  delete on macOS; clear with an O(depth) lift-and-remove walk.

  **DEVIATION RECORD + LESSON — the E3 landing range is per-commit
  RED, 2026-08-27.** The boundary workflow at `1d50e59`
  (33040764671) failed at the per-commit lane gate: the four
  cherry-picked intermediates of the E3 landing (6848109, d36c725,
  06616f8, 88829c7) carry their original in-train pin values, wrong
  on their rebased trees — the reconcile deliberately deferred pin
  re-execution to the tip, and the gate (built for exactly this
  class; its header cites 95d292f/b923109) fired correctly on the
  choice. The TIP is fully gated green (checker, fmt, check,
  clippy, three partitions direct-exit) and the other three legs at
  `1d50e59` are green (Platform 33040774620, Windows 33040773113,
  retained readers 33040764606); the E1-tip set is 4/4 green after
  the known kill-race flake's rerun (33039190893 → success;
  Platform 33039209950, Windows 33039208104, boundary 33039190896).
  The four red intermediates are immutable, recorded here as the
  gate's own precedent prescribes, and sit behind every future
  push's base — the branch is not blocked and the visible red heals
  at the next push. NO floor advance is needed or taken. **LESSON
  MINTED: a multi-commit landing train must be per-commit
  gate-green — re-pin at every intermediate during the rebase — or
  be landed SQUASHED to one commit citing the reviewed worktree
  shas. E2's landing will be squashed.**

  **E2 IS LANDED — 2026-08-27; gwz-core main 1d50e59 → `c11c5ef`.
  THE BARRIER FAMILY IS EXECUTED 16/16 WITH THE O6 WITNESS, AND
  WITH IT THE R2-E E1-E3 SPAN IS COMPLETE: ALL 38 RE-RESERVED KEYS
  ARE BOUND AND EXECUTED (cleanup 11/11 + barrier 16/16 + terminal
  10/11-with-determination), THE CENSUS HOLDS AT 165, AND THE
  ACTIVATION VOCABULARY'S `Reserved` ARM IS UNCONSTRUCTED FOR THE
  FIRST TIME — annotated as retained frozen vocabulary per the
  map's carried re-reservation clause (clippy's dead-variant error
  at the squash was the milestone announcing itself).** E2 round 2
  GO (report appended, filed with this commit): [P2-1] cured by
  the builder's OWN re-derivation — prepare_roaming_target owning
  both names, compiler-exhaustive four-row table, atomicity walked,
  no unlink window, the reviewer WITHDREW its recommended
  refuse-shape; wedge argument sound (the .roundtrip grammar can
  never classify Valid, so no ordinal can reserve a colliding
  leaf); anchor.rs byte-identical to base (round-1 delegation
  unwound); checker-suite anomaly closed authoritatively 69/69;
  three new P3s → [R2-P3-1] settled-does-not-imply-barriered (an
  E4 scope clause + E7-dual item), [R2-P3-2] the unannounced
  return rename / 17th-key question (E7-dual), [R2-P3-3] wording.
  LANDED SQUASHED per the minted lesson (one commit, the four
  reviewed worktree shas cited; per-commit lane gate green by
  construction). Landing reconciliation executed: forwarder-file
  unions with E1/E3 (two dropped-brace junctions caught by cargo
  check and repaired); digests re-executed (pre_catalog 5c65d5a2,
  catalog cc845e20, platform 2f938dd9); darwin partition EXECUTED
  446 = 400 + 8 + 17 + 21, linux 456 derived
  FIRST-DISPATCH-EXPECTED; full battery direct-exit green
  (checker, fmt, check, clippy, 446/932+1/256 — lib 1636
  accounted). THE FREEZE §3.5 INVENTORY ADDENDUM'S COUNT IS MOVED
  IN PLACE nine → ten with dated provenance (the amendment §6.3
  E2.3 duty, the single R2-E count move; barrier_mutation.rs (16)
  listed; len()==10 machine pin cited). Matrices dispatched at
  c11c5ef: Windows 33044834867, Platform 33044836558 (these
  measure linux 456 and are the FIRST Windows compile of the
  roaming-anchor code — WINDOWS-ARM-OWED discharges on their
  green). [CI CLOSED 2026-08-27: ALL FOUR LEGS GREEN at c11c5ef —
  Windows 33044834867 (the roaming-anchor code's first Windows
  compile+execution: WINDOWS-ARM-OWED discharged; the
  barrier.target_barrier native arm executed), Platform
  33044836558, boundary 33044832498 (the per-commit lane gate
  green on the squashed landing — ritual 7 validated; the E3-range
  red healed), retained readers 33044832501. The linux 456 marker
  stays derived-and-marked until the next Linux r4bg battery
  executes it, per the docstring's measured-number-wins rule.] NEXT IN R2-E: E5 (archive/GC sub-package, parallel-safe)
  and E6 residue; E4 remains gated on R2-F's relocation per the
  operator's ruling (a).

  **R2-E E5 IS DELIVERED — 2026-08-28; branch `e5-archive`
  (worktree e5-worktree), TWO commits off c11c5ef: `221cd89`
  (E5.1) + `cf4213a` (E5.2). Builder reports all gates
  direct-exit green (446/935+1/256; boundary ok + 69/69; compat
  checker "validated 7 migration rules, 7 runtime bindings, and
  10 archive shapes" + 23/23; g23 119→122), lib-remainder marker
  moved 932→935 darwin-MEASURED / 933→936 linux-DERIVED
  FIRST-DISPATCH-EXPECTED, no fault-key/census/wire/production
  surface touched, and the only removed lines in the range the
  seven pin/marker strings.** E5.1 is ONE commit per L6 (the
  rows + the parametric `adapt_open` refusal test together; no
  precheck-walk call in the file — the walk discharges nothing
  here). FIVE FLAGS RAISED FOR ADJUDICATION: (1) NINE registry
  rows + ONE clause-cited disposition, not ten — G-VERIFYING is
  a `Finalizing` shape (R0 §4 row G), so §12.9(c) binds by its
  own reason: the E0.2b §8 [P2-2] "10 registry rows" denominator
  is claimed off by one, and §12.9(c)'s four-row Finalizing list
  really five; the disposition is DISPOSITIONED-UNLISTED in
  §12.7's second form (executed test
  `g_verifying_is_dispositioned_by_clause` — the registry's
  closed publication_step enum lacks verifying_publication, so
  zero rules can match). (2) Corpus state vocabulary widened by
  three NON-Finalizing tokens (executing / awaiting_resolution /
  halted), dated; the Finalizing exclusions untouched and still
  panicking. (3) TIER 2 IS OWED ON ALL TEN ARCHIVE ROWS — no
  v1-finished archive fixture exists on this tree (the v1
  archive tests are synthetic-bytes only; nothing reads back
  .gwz/merge/done/ after a real run); builder proposes E4.4 as
  carrier (lane-owner ruling owed). (4) OPEN DESIGN QUESTION:
  tier 2 as literally stated may be UNSATISFIABLE — a
  v1-produced vs a v0-produced archive of the same scenario
  differ by construction across the WHOLE frozen projection
  surface (SupportedPersisted vs LegacyComplete, plus
  source_version), so tier 2 additionally needs a defined
  comparable sub-surface; deliberately NOT minted by the builder
  (the amendment §6.3 rejected-alternative warning against
  narrowing a comparison to make the clause look met). (5) Eight
  tier-1 rows over six distinct durable bases (three fixtures
  are post-archive overlays; byte-preservation claimed as a
  property of the archival act), disclosed in the module doc.
  The E0.2b §8 denominators are machine-enforced in the checker
  (exactly 8 tier-1-executed; the PENDING-FIXTURE pair exactly
  AC-NOPUB-UNBORN + AP-PRESERVED, carrier R2-F); the §12.8
  PARTIAL statement untouched. LANE-OWNER LANDING DUTIES QUEUED:
  the Evidence-doc §12.3 Table A / §12.4 Table B companion edits
  (this repo) — the M4 map checker carries the forward pin "(39
  scenario rows, 42 named tests, 22 registry rows all claimed)"
  DOC-PENDING and is RED by construction until they land — plus
  the §12.9(c)/(d) dated corrections if the review ratifies flag
  (1), and the tier-2 carrier ruling. INTERIOR SINGLE-AXIS
  REVIEW LAUNCHED 2026-08-28 (Opus, peer-blind; report files
  verbatim to GwzM5-8R2E-E5-Review.md); the review carries the
  ratify/overturn ruling on flag (1), the CI-impact analysis of
  the DOC-PENDING pin (landing-order load-bearing: does the map
  checker skip-loudly or fail in a standalone CI clone), and
  opinions on the tier-2 questions.

  **R2-E E5 IS LANDED — 2026-08-28; gwz-core main c11c5ef →
  `221cd89` (E5.1) → `cf4213a` (E5.2) → `0a17e48` (landing
  reconcile) → `fc0bb22` (the a6ce8a8 stale-pin fix). THE UNBOUND
  SCENARIO SPACE IS MACHINE-RECORDED: 18 UNBOUND → 0 — nine new
  valid_unlisted registry rows plus G-VERIFYING's clause-cited
  disposition (the "10 registry rows" denominator was a 9+1
  partition, §12.9(c)'s own ground — RATIFIED by the review;
  Evidence §12.9(e)), and the eight fixtured archive rows tier-1
  byte-preserved across the production archival act in the
  standalone archive corpus (two PENDING-FIXTURE, carrier R2-F;
  tier 2 owed on all ten).** Interior review GO-WITH-CONDITIONS
  (GwzM5-8R2E-E5-Review.md, filed verbatim and committed): 1 P1,
  4 P2 (two escalating), 6 P3; all six landing conditions
  executed. [P1-1]: the forward map pin was short by one — E5.2's
  own companion adds a second named test; pinned 43 and MEASURED
  green against the real Evidence doc ("M4 scenario map: ok (39
  scenario rows, 43 named tests, 22 registry rows all claimed)").
  [P2-1] RULED (lane owner, escalation): the K/J(ii) same-object
  fact is the sanctioned one-object-two-R0-rows pattern — both
  records stand, they answer different ledger questions; §12.9(c)'s
  ground corrected (it reaches the three F rows + G-VERIFYING —
  four Finalizing rows, not five — and never grounded J(ii)); all
  at §12.9(e); RIDES THE E7 DUAL for second-axis scrutiny. [P2-3]
  RULED: E4.4 RATIFIED as tier-2 carrier WITH the R2-F encumbrance
  named in the eight corpus carrier strings and at the plan's E4.4
  entry (E4 gated on R2-F's relocation per ruling (a) — tier 2 is
  transitively R2-F-dependent). TIER-2 SUB-SURFACE QUESTION
  (review adjudication G(iii); builder flag 4 CONFIRMED): tier 2
  as written in amendment §6.3 is NOT SATISFIABLE — two of
  ArchivedMergeProjection's three fields differ by construction
  between v1- and v0-produced archives — so a comparable
  sub-surface must be minted BY AMENDMENT WITH DUAL REVIEW before
  E4.4 executes tier 2; deliberately unminted (plan E4.4 entry;
  E7-dual item). Corrections to the delivery record above: [P2-2]
  "six distinct durable bases" is FIVE (module doc fixed); [P3-1]
  "seven removed lines" was eight (the benign rollback/participant
  re-add). Companion edits landed in THIS repo: Table A's ten rows
  (registry ids + G-VERIFYING DISPOSITIONED-UNLISTED, each citing
  the parametric test; the plain fn NOT cited as a test path, per
  [P3-6]); Table B's ten rows (eight TIER-1 BYTE-PRESERVED + two
  PENDING-FIXTURE); the §12.6 E5 closure-movement bracket (22 rows
  bind 22 shape labels; durable-object count lower per the [P2-1]
  note); §12.9(e); plan §1.1's 9+1 bracket + the E4.4
  determinations. Landing gates all direct-exit green at 0a17e48
  (fmt/check/clippy; 446 / 935+1 pre-existing ignored / 256;
  boundary ok 15/5 + 69/69; compat 7/7/10 + 23/23) — lib total
  1639. THE FIRST REAL-WORKSPACE BATTERY RUN SINCE THE v0.11.0
  TRAIN EXPOSED A LATENT RED: a6ce8a8 added the manifest's twelfth
  source (the recovery guide) and moved the driver marker but
  missed the merge-docs unit-suite pin (11 vs 12) — the J-7 blind
  spot's cost, invisible to worktrees and CI alike; fixed at
  fc0bb22 with the provenance in the commit, and both
  real-workspace batteries then GREEN (byte-equivalence: map 43/22
  + g23 122; compatibility: 7/7/10 + 23/23 + merge-docs "ok (12
  sources, 155 assertions)" + suite 3/3). Markers: lib remainder
  darwin 935 EXECUTED, linux 936 DERIVED FIRST-DISPATCH-EXPECTED
  (the dispatched Platform matrix measures it). Matrices
  dispatched at fc0bb22: Windows 33138727062, Platform
  33138728650; the boundary lane gate is green by construction at
  each of the four new commits (boundary surface untouched).
  E7-DUAL QUEUE ADDS: the K/J(ii) determination; the tier-2
  sub-surface amendment question. E6 QUEUE ADDS: [P3-2] (execute
  the structural publication_step claim over the whitelist),
  [P3-3] (weaken-and-raise coverage for VALID_UNLISTED_STATES —
  including a finalizing-rejected probe — and the new
  ARCHIVE_DISPOSITIONS/TIER_STATUSES closed sets). [P3-4] recorded
  (six of ten shapes reach their windows by disclosed field
  re-statement); [P3-5]'s ground sentence fixed at the reconcile.
  NEXT IN R2-E: E6 residue, then the E7 settle; E4 stays gated on
  R2-F's relocation per the operator's ruling (a).

  **R2-E E5 CI CLOSURE (2026-08-28) — ALL SIX LEGS GREEN, AND THE
  LINUX PIN SET IS SUM-CONFIRMED.** The six dispatched legs all
  completed `success`: Checked-artifact boundary + Retained merge
  readers at 0a17e48 (33138567559 / 33138567560) and at fc0bb22
  (33138695320 / 33138695324), Windows matrix 33138727062, Platform
  matrix 33138728650. The Platform run measures the FULL lib suite
  per platform, not the partitions, so condition 6 discharges at
  sum level, and the identities land to the digit on both hosts:
  darwin `1638 passed + 1 ignored` = 446 + 935 + 256 + 1 (the four
  darwin pins, each already partition-measured on this tree); linux
  `1649 passed + 1 ignored` = 456 + 936 + 256 + 1 (the aggregate
  driver's four linux pins as a set — the FIRST measured linux
  touchpoint since that derived chain's root). Within the linux
  sum, the root-fault-matrix release leg is per-partition measured
  by its own log line (`1 passed; 1649 filtered out`, which also
  fixes the linux lib census at 1650 = 1649 executed + 1 ignored);
  the other three linux addends (456 / 936 / 256) remain
  individually DERIVED, jointly sum-confirmed, and their
  per-partition linux execution is OWED AT THE E7 THREE-PLATFORM
  ACCEPTANCE, which should rewrite the driver's linux pins as
  measured numbers. Marker movement: lib remainder linux 936
  DERIVED FIRST-DISPATCH-EXPECTED → SUM-CONFIRMED at the fc0bb22
  Platform dispatch. One premise-check made during closure, for the
  record: the one ignored test
  (`operation::workspace_mutator_lock::tests::
  child_process_observes_lock_contention`) carries a BARE
  unconditional `#[ignore]` (workspace_mutator_lock.rs:279), so it
  is ignored on every platform — both hosts showing exactly 1
  ignored is consistent, and the darwin/linux remainder offset
  (935 vs 936) is a baseline platform delta predating R2-E, not
  that test executing on linux. No live pin is wrong; no code
  change. THE E5 CYCLE IS FULLY CLOSED — build, interior review,
  landing, and CI, with nothing owed forward except what the E6/E7
  queues already carry.

  **R2-E E6 DELIVERED (2026-08-28) — three commits on 8c59521,
  review launched.** Builder (Opus) delivered `d73abe8` (E6.1) →
  `e1e043f` (E6.2) → `a593dbd` (E6.2b), each lane-gate green with
  measured partitions. E6.1/O9: the composed-path upgrade-failure
  fallback EXECUTED with no production seam — the builder first
  PROVED no permission-class fault can isolate the upgrade leg
  (both stagers use the same create_new primitive into the same
  directory; the composed entry crosses ZERO fault-key boundaries,
  probed) and then used the staging-name window: the upgrade's
  temp name is exactly 8 bytes (`.upgrade`) longer than the
  store's, so a crafted id length drives the upgrade over the
  255-byte component cap while the v0 leg fits — a genuine open(2)
  refusal with `AtomicUpgradeFault::None` untouched; id supplied
  at the start via the existing cfg(test) IdProvider (patching
  later breaks the GWZ-Merge-ID trailer — measured). Control arm
  (ordinary id migrates to V1) plus an isolation-guard second test
  pinning the refusal as IoError, not a compatibility verdict —
  TWO tests, both cfg(unix): the FIRST non-cfg-independent lib
  delta in R2-E (Windows totals now legitimately differ; noted in
  the driver). O10: `#[cfg(test)]` on the four injected variants —
  stronger than sealing (production compiles NO constructor;
  negative compile probe verified, then reverted). Riders: the
  abort-bound guard tie executed test-side (dirtied path required
  OUTSIDE the reported conflict set); E5 [P3-2] executed as
  assertions joining `g_verifying_is_dispositioned_by_clause` (no
  count move, deliberately); E5 [P3-3] four weaken-and-raise tests
  (compat suite 23→27, incl. the finalizing-rejected probe); E1 F3
  `_fault_count` docstring CURED — NOT absorbed by E5 (the
  unqualified "never derived" sentence still contradicted three
  derived blocks below it; now scoped to its v0.11.0 baseline).
  LANE-OWNER RULINGS (the brief's own contradiction, owned): the
  charter said both "O10 is the only production change" and
  "execute the two anchor nits" (production edits in anchor.rs) —
  builder correctly held the restrictive reading and stopped.
  Ruled: ANCHOR NIT 2 AUTHORIZED and executed at `a593dbd` — the
  retired-ordinal parse now requires `retired_name(parsed) ==
  text` else Invalid, implemented against retired_name's REAL
  unpadded rendering (the amendment's "canonical two-digit parse"
  phrasing misdescribes it — corrected in the commit); negative
  check performed (guard reverted → test fails at the first
  rejected rendering). ANCHOR NIT 1 NOT IMPLEMENTED — RE-ROUTED TO
  THE E7 DUAL with the builder's finding as the record: the
  deferral's ":394 one-line take() cure" no longer describes the
  tree; the unbounded read lives in the SHARED
  `observe_leaf_exact` (observation.rs:249) every leaf observation
  uses, so the cure is a design decision (bounded observation
  entry vs cap on the shared reader). PINS (measured/derived per
  convention, dated docstring blocks appended): remainder
  935→937 darwin-MEASURED / 936→938 linux-DERIVED; g23 marker
  122→124 MEASURED; checked_artifact 446→447 darwin-MEASURED /
  456→457 linux-DERIVED; `PROTECTED_SOURCE_TREE_DIGESTS
  ["checked_artifact/platform.rs"]` RE-PINNED on the E2 precedent
  — all seven digests recomputed, EXACTLY ONE moved (the evidence
  the edit stayed inside the anchor protocol) — the SECOND
  deliberate lowering of that fail-closed guard in R2-E, flagged
  to the review by name; root_fault_matrix release leg not re-run
  by the builder (nothing touches v1_lifecycle) — runs at the
  landing's fault battery. Interior single-axis review (Opus,
  peer-blind, checklist A–L) LAUNCHED at `a593dbd` in a detached
  review worktree; adjudication and landing follow its report.

  **R2-E E6 IS LANDED (2026-08-28) — gwz-core main fc0bb22 →
  `afbc25d`, batteries green, matrices dispatched.** The pushed
  chain: `8c59521` (operator agents-template edit, riding the
  train per its standing ruling) → `d73abe8` E6.1 → `e1e043f`
  E6.2 → `a593dbd` E6.2b → `afbc25d` (landing reconcile). The
  interior review (GwzM5-8R2E-E6-Review.md) returned GO — no
  P0/P1/P2, seven P3 record-only — and independently reproduced
  all three probe claims (O10 negative build E0599; anchor
  guard-removal failure at tests.rs:532; the driver's win32
  import refusal) and recomputed all seven protected digests from
  the git objects (exactly one moved; its diff exactly the two
  anchor edits). The reconcile folded the reviewer's offered
  cures: [F-1] the closed-grammar table row corrected to
  `.ca1-anchor-retired-<ordinal>`; [F-2] the refusal test now
  asserts the whole directory listing unchanged across the
  refusal; [F-3] the survey comment states the trade
  (slot-wastage exchanged for a recoverable fail-closed refusal
  on the foreign shape — the lane owner reads this as the step's
  own authorization, NOT a semantics move needing E0.2b); [F-4]
  the driver baseline claim distinguishes executed lib totals
  from subtraction-derived remainder values — with a SECOND dated
  platform.rs digest re-pin (comments and one test assertion
  only). LEDGER-CLOSURE WORDINGS ([F-5]): O10 closes as
  "cfg(test)-gated variants — a third shape, strictly stronger
  than both named options (production compiles no constructor at
  all; negative compile probe verified twice)". E7-DUAL QUEUE ADD
  ([F-6]): anchor nit 1 (the unbounded read in the SHARED
  `observe_leaf_exact`, observation.rs:249, twenty-one call sites
  across six modules) travels WITH the in-tree cure template the
  reviewer found — platform.rs:219-234's bounded verification
  read (try_reserve_exact + take(len+1)) — narrowing E7's
  decision to "reuse this shape or bound the shared reader".
  [F-7]: the Phase E6 plan heading is annotated this same commit
  — the review-debt ledger empties at E7, not E6 (nit 1 and
  E6.3's dated no-work record both spill by ruling).
  REAL-WORKSPACE BATTERIES ALL GREEN at afbc25d (the J-7 ritual,
  no latent red this round): fault 256 / root_fault_matrix
  RELEASE 1 passed (665.5s — re-measured after two skipped
  rounds) / census 447 / remainder 937; compatibility 7/7/10 +
  suite OK + merge-docs 12 sources 155 assertions + suite OK;
  byte-equivalence map ok (39 scenario rows, 43 named tests, 22
  registry rows all claimed) + g23 124. Pins at the tip: CA
  447 darwin-MEASURED / 457 linux-DERIVED; remainder 937
  darwin-MEASURED / 938 linux-DERIVED (NOT cfg-independent — the
  two O9 tests are cfg(unix); first such delta, recorded in the
  driver both places); g23 124. Matrices dispatched at afbc25d
  with a background watcher. NEXT: the conf-integrity lane
  (standalone, operator-chartered): its interior review returned
  GO-WITH-CONDITIONS (GwzConfIntegrity-Review.md — P1-1 dry-run
  marker write without guard, P1-2 --force blesses an unparseable
  lock, plus the settings-JSON round-trip cluster); remediation
  round 1 dispatched with lane-owner rulings on all ten
  actionable findings; verification pass then landing (rebase
  onto afbc25d; remainder pin will move 937→967 measured at that
  landing), THEN v0.11.1 IS OPERATOR-CHARTERED from that tip
  ("let's cut v0.11.1 on E6 and conf integrity fixes",
  2026-08-28). E7 follows the release; E4 stays gated on R2-F.

  **R2-E E6 CI CLOSURE (2026-08-29) — all four legs green at
  afbc25d, linux pin set sum-confirmed again.** Checked-artifact
  boundary 33173532231 + Retained merge readers 33173532235
  (push-triggered) and Windows matrix 33175382330 + Platform
  matrix 33175385033 (dispatched) all completed `success`. The
  Platform sums hold to the digit on both hosts: darwin `1641
  passed + 1 ignored` = 447 + 937 + 256 + 1 (the four measured
  pins); linux `1652 passed + 1 ignored` = 457 + 938 + 256 + 1
  (the driver's derived linux set — SUM-CONFIRMED, per-partition
  linux execution still owed at E7's three-platform acceptance).
  The cfg(unix) O9 pair is correctly PRESENT on linux (+2 inside
  the 938) and absent only on Windows, whose matrix leg is green.
  THE E6 CYCLE IS FULLY CLOSED — build, review, landing,
  batteries, CI. The release train now waits only on the
  conf-integrity lane (remediation r1 in flight).

  **CONF-INTEGRITY IS LANDED (2026-08-29) — gwz-core main afbc25d
  → `9a64ce9`; the release tip is assembled.** The standalone lane
  (operator-chartered defense of gwz.conf against agent hand
  edits) landed as TWO commits per ritual 7's squash arm — the
  rebase onto afbc25d invalidates per-commit green transfer, so
  `2c305d7` squashes the five reviewed commits citing their shas
  (b23a68e/3aa48ab/0523f0e/cf4f308/2508343, reviews
  GwzConfIntegrity-Review.md GO-WITH-CONDITIONS +
  -Review-2.md verification) with the compile triad run on the
  squash itself, and `9a64ce9` is the landing reconcile. The
  verification pass discharged every round-1 condition EXCEPT
  P1-1's dry-run clause, finding NF-1 (P2): handle_branch and
  handle_stash wrapped the gate in `if _guard.is_some()`, so
  `branch --create --dry-run` / `stash push --dry-run` returned
  Ok over a real hand edit. CURED AT THE RECONCILE in the
  reviewer's own shape — gate on the op, not the guard (List
  stays ungated for damaged-workspace inspectability; a mutate
  dry run refuses and cannot write, since reconcile_authority
  stays None) — with a regression test that also pins the
  ungated List reaching PAST the gate to ordinary id validation.
  NF-2 (P3) cured: the round-trip comment no longer claims the
  unresolvable-exponent case ends Skipped; the 1e400 string
  retype is PINNED by assertion. NF-3 (match-arm classification
  ≠ chain membership) and NF-4 (guard root-equality assert)
  recorded as BACKLOG per the verification's grading, alongside
  the two merge-lane follow-ups the round-1 review accepted (the
  lock banner blocked by render_complete_lock's comment-stripping
  byte-compare; the composition commit not carrying the marker —
  now harmless, the gate reconciles). Pins: lib remainder 937 →
  979 darwin-MEASURED / 938 → 980 linux-DERIVED (+42,
  cfg-independent; dated driver block); CA 447 and v1_lifecycle
  256 re-measured unchanged; g23 124 unchanged; boundary ok
  15/5. Gates at the tip all direct-exit green. Real-workspace
  batteries running at 9a64ce9; matrices dispatch on their
  green. **v0.11.1 releases from this tip once CI closes, per
  the operator's charter.**

  **CONF-INTEGRITY CI: WINDOWS FIRST-DISPATCH RED, FIXED FORWARD
  (2026-08-29).** At 9a64ce9 the Platform matrix (33184770319),
  boundary (33183463215) and retained-readers (33183463144) legs
  were green, but the Windows matrix (33184767171) failed: ALL
  TEN `conf_gate::tests` failed their shared fixture with os
  error 267 (NotADirectory, "The directory name is invalid") —
  the module's hand-rolled temp_dir embedded SystemTime's Debug
  rendering in the directory name, which carries braces, spaces,
  and a COLON, and a colon is not a legal Windows filename
  character. Every other conf-lane module used the crate's
  TempDir helper and passed (1632/1642 with only this module
  red); darwin and linux never see it because those are legal
  unix filename bytes — a pure first-dispatch-on-Windows class.
  Fixed forward at `cc7c625` (test-fixture-only: nanos since the
  epoch, like every other unique-name site in the crate; no
  production change, no count move; darwin gates re-run green,
  remainder re-measured 979). Both matrices + push legs
  re-dispatched at cc7c625 (Windows 33188767003, Platform
  33188769600); v0.11.1 cuts from cc7c625 on their green.

  **v0.11.1 IS RELEASED (2026-08-29) — both repos tagged,
  published, verified; THE LINUX PINS ARE NOW EXECUTED.** CI
  closed green at cc7c625 (Windows 33188767003 with the fixture
  fix, Platform 33188769600, boundary 33188767639;
  retained-readers path-filtered out legitimately, its 9a64ce9
  green covering the untouched surface). gwz-core released via
  scripts/release.py v0.11.1 --push: full local gate in a clean
  worktree, then `be693bd` (chore(release): gwz-core 0.11.1) +
  tag v0.11.1 pushed atomically. gwz-cli released via its
  scripts/release.py v0.11.1 --push: release branch reconciled at
  `3e6f974` (gwz-cli 0.11.1, pins gwz-core v0.11.1), built and
  tested against the pinned tag, tag pushed atomically (cli main
  177f25d carries the v0.11.1 compatibility notes; docs gate
  re-verified 12 sources / 155 assertions after the edit). Both
  GitHub releases PUBLISHED (core with the hand-written notes per
  the v0.11.0 precedent — conf-integrity defense with named
  accepted residuals, the anchor tightening WITH its remedy line,
  the verification expansion, not-in-this-release; cli notes
  point at core). Pipelines: cli dist Release SUCCESS — 16
  assets; core Release verify SUCCESS on windows-2022 AND
  ubuntu-24.04. Runbook verification executed: sha256 checksum
  OK, gh attestation verify OK, released binary reports gwz
  0.11.1 with correct help, and the unix installer smoke test
  from releases/latest installs and runs 0.11.1. **MARKER
  DISCHARGE: the release verify's ubuntu leg ran the fault
  battery against the pinned counts — checked_artifact 457,
  lib remainder 980, v1_lifecycle 256, root_fault_matrix 1, all
  EXECUTED on linux — so every FIRST-DISPATCH-EXPECTED linux
  marker in the driver is discharged by direct execution, not
  sum-confirmation; E7.2's per-OS re-measurement obligation for
  the current pin set is met by this run (E7 should cite run
  33196576270).** NEXT: E7 settle (dual #2 + acceptance +
  481-item reconciliation + ledger close); E4 stays gated on
  R2-F's relocation.

  **v0.11.1 RELEASE ADDENDUM (2026-08-29) — THE THIRD CHANNEL:
  PyPI IS AT 0.11.1.** The initial cut missed the gwz-py channel
  (the operator caught pypi.org/project/gwz still at 0.11.0):
  gwz-py's RELEASE.md points backward at core and cli, but
  nothing pointed forward. Completed: gwz-py released via its own
  scripts/release.py v0.11.1 --push (release branch reconciled at
  `e0a8210`, gwz-py 0.11.1 pinning gwz-core v0.11.1; the script's
  full check env passed — protocol drift/regen, cargo check,
  python tests, wheel build + installed-command smoke); GitHub
  release published; publish.yml run 33212835859 SUCCESS (five
  platform wheels + sdist, each smoke-tested, PyPI trusted
  publishing); pypi.org reports latest = 0.11.1. THE RUNBOOK GAP
  IS CLOSED AT THE SOURCE: gwz-cli RELEASE.md step 6 ("the
  release is not done until PyPI moves too") forward-points at
  gwz-py's process, committed and pushed (`11bca66`). v0.11.1 is
  now complete across all three channels: gwz-core `be693bd`,
  gwz-cli `3e6f974` (16 dist assets), gwz-py `e0a8210` (PyPI).

  **R2-E IS ACCEPTED — E7 CLOSED, 2026-08-29 (operator: "proceed with
  E7").** Dual #2 ran peer-blind Fable×2 at `be693bd`: **Code
  GO-WITH-CONDITIONS (0 P0/P1/P2, 4 P3)** — all gates direct-exit green
  in its own worktree (447/979+1/256 matching darwin pins), all five
  machine-enforced inventories perturbation-probed RED-and-reverted,
  digest re-pin archaeology 8597d32..be693bd clean, E3-F1 cure ratified
  with an independent 64-recover/65-refuse probe, E2-P2-1 wedge
  re-derived from slots.rs, O9's window arithmetic re-derived — and
  **State GO-WITH-CONDITIONS (0 P0/P1, 1 P2, 7 P3)** — all fourteen
  ledger close forms viable, pin chain re-derived to the digit, K/J(ii)
  SUSTAINED, citation-drift PASS, wire zero-diff re-verified over the
  whole span. Zero escalations; round 1 complete both axes; the
  two-round cap never engaged; every condition a lane-owner fold
  (E0-precedent), executed same-day: [P2 F1] the settled≠barriered
  clause now a dated annotation AT THE PLAN'S E4 GATE NOTE with F4's
  terminal sibling (converged≠flushed) beside it; C2 the freeze §3.5
  barrier record's dated NAMED-EXCEPTION sentence for the P5 roaming
  recovery rename (both axes converged independently; 17th key refused
  on substance; the optional barrier_mutation.rs:19 clause DECLINED to
  avoid a comment-only digest re-pin, riding the C1 train); C1 anchor
  nit 1 closed RE-OWNED with named carrier = the next production train
  touching checked_artifact/, carrying the Code axis's full Q1 shape
  (bound observe_leaf_exact itself: cap from the identity-checked
  opened.len() fstat, try_reserve_exact + take(len+1), signature
  unchanged, 18 call sites untouched; [R2-P3-3]'s wording fix + F3's
  stat-level family-gate reorder ride as one class; ':394' terms
  retired); O3 close re-tensed per State F8; O8 close carries §12.9(e)
  as denominator authority + the two-part tier-2 encumbrance; OPEN-
  B2/B3/B7/B8 closed by citation; the ten-writer-rows entry written in
  Step-4.2 form (E1-tip 4/4 + 1d50e59 + c11c5ef run ids), resolving
  tuple :665 and §7.3's re-verify clause; E1-F1 16-KiB + E1-F2 carried;
  E6.3's four-ingredient dated no-work record filed. **E7.2 EXECUTED:**
  matrix acceptance by citation at the settled tree (Release run
  33196576270 at be693bd — ubuntu job 98935133025 EXECUTED 457/980/
  256/1, windows job 98935132771; darwin = the release's local gate;
  boundary checker re-executed ok 15/5 at the acceptance); the 481-item
  reconciliation EXECUTED against both denominators (blanket-hidden
  481/50 → **337/54** via the twelve heir attributes; subsystem sweep
  1657/85 → **998/82**; crate-wide 1154/141 recorded as context; −144
  items = E1-E3's measured consumption, entry 14→0 fully consumed,
  protocol ~144→66; residue = the E4-awaiting frozen surface, falling-
  count expectation transferred to the E4 resumption); the driver's
  linux pins converted DERIVED→MEASURED at gwz-core **`8e18403`**
  (scripts-only +18, proportionate gates py_compile/--list/boundary all
  0, pushed be693bd..8e18403; push legs CLOSED GREEN same day —
  boundary 33230315908, retained readers 33230315940, both success;
  nothing in flight anywhere on the program). PAUSE RECORD
  (2026-08-29, operator: quota at 6%): the lane pauses AT THIS CLEAN
  BOUNDARY until the Fable pool resets; resume step 1 = draft the
  R2-F plan (ledger + phases + OPEN-R1 as the owner's first decision,
  R2-E-plan shape); the remaining pool is reserve for emergencies
  only. Non-Fable work may proceed during the pause: the gwz log
  project (gwz-cli/dev-docs/history/GwzLogRequirements.md +
  GwzLogAmbiguityRezo.md) implements on a non-Fable agent once the
  operator's Rezo comments land. **THE LEDGER
  IS CLOSED row-by-row in GwzM5-8R2E-E7-Acceptance.md**: O4-O10, O12
  DISCHARGED (O10 with the F-5 wording; O12 with this acceptance's own
  acts); O11 closed-negative; O1/O2 RE-OWNED to R2-F-relocation→E4-
  resumption (O1 carrying row :280 via O13); O3 RE-OWNED to the
  relocation package (discharges on landing); O13 pin-half discharged /
  substantive-half re-owned; E6.3 VOID with its dated record. THE
  REVIEW-DEBT LEDGER IS EMPTY (F-7's terms met). Artifacts committed
  with this record: GwzM5-8R2E-E7-ReviewCode.md + -ReviewState.md
  (verbatim), GwzM5-8R2E-E7-Acceptance.md (status CLOSED), the plan's
  E4-gate dated annotation, the freeze's named-exception annotation.
  Worktree hygiene: both review worktrees + targets removed at lane
  close. NEXT: **R2-E is done; the lane idles.** E4 resumes only after
  R2-F's relocation package (operator's scheduling item, ruling "a");
  R2-F also carries OPEN-R1, the PENDING-FIXTURE pair, C-2 fixtures,
  T-5, multi-repo CI, MAX_PATH, native power-loss; M5c owns ordinary/
  custom-message v1 starts; the C1 carrier train owns the nit-1 shape +
  wording + gate-reorder class; conf-integrity backlog stands recorded.

  **POSITION 2026-09-01 — THE R2-F RELOCATION IS LANDED; E4 IS OPEN
  (matrix legs in flight).** Since the pause: **gwz log SHIPPED as
  v0.12.0** (all three channels; side project, non-Fable
  implementation, settle S4.1 GO), then **v0.12.1 released and fully
  verified** same-day for the pull ahead-only fix (strictly-ahead
  root/member misclassified DivergedMember/MergeRecoveryRequired; fix
  gwz-core `8ce9281`, tags core `ea3a924` / cli `68a888a` / py
  `ed25230`; core verify run 33465789854 EXECUTED linux remainder
  1098). Release-script incidents fixed at cli `5555c41` (path-shaped
  lock cure; the hardcoded AI trailer REMOVED — audit commit-writing
  scripts, not just session config). **M5c TRAIN Stage 1 = the R2-F
  relocation**: read-only trace (the tree had outgrown the E0
  records), then plan ADOPTED at gwz-dev `d2e5636` after a FULL
  three-round loop (r1 NO-GO — my OPEN-R1 recommendation inverted the
  trace; r2 NO-GO terminal — renaming shared `Final` marched the
  legacy writer into the catalog's new home; re-charter GO-WC). THE
  DESIGN: one new name, two directories — `Final`'s bytes →
  `catalog-final`, the legacy writer keeps `checked-artifacts` under
  its own NEW variant inside the collision domain (member pins 4→5);
  de-recognition solely via Final's byte change; nothing orphaned.
  **R1.2 LANDED** gwz-core `bb52dc0` (A1 activation tripwire, caller
  count pinned ZERO until E4.1; 2 rounds, r1 NO-GO P1 spelling
  evasion → file-set contains; CA 448). **R1.1 LANDED** gwz-core
  `027da5b` (squashed citing reviewed `4ba9071` + folds `3d417f7`/
  `1e2a106`; r1 GO-WC 0 P0/1 P1/1 P2/6 P3, r2 verification GO,
  probes A–F; the no-anchor guarantee spelling-blind; decisive-row
  mechanism platform-split — ca1-* refusal is the WINDOWS arm,
  disjointness discriminates darwin/linux; three semantic vectors
  re-derived, ruled LEGITIMATE-forced, derivation in the vectors'
  header; CAP RE-RULED 420→427-MEASURED, overage entirely
  review-condition cure, rider-audit none). Pins on the landed tree:
  CA 452 darwin MEASURED / 462 linux DERIVED; remainder 1097/1098;
  v1_lifecycle 256; six checker digests; merge-docs 12/155; lane gate
  ok. **R1.3 RECORDS EXECUTED (this commit)**: the MAX_PATH rider
  FALSIFIED at all five homes + the drifted `:1022-1024` anchor
  re-pointed (`:3192-3194`); trace §7.6 retired (the relocation IS a
  semantic-vector event — decode, don't grep the encoding); **O3
  DISCHARGED** (verbatim-quoted at the R2-E plan's O3 row;
  single-owner scope note: three production spellings); **OPEN-R1
  RESOLVED BY DESIGN** ("neither — the relocation relocates the
  CATALOG'S name"; operator veto open at `d2e5636`); **O1/O2
  UNBLOCKED** at the E4 resumption (O1 carrying row `:280` via O13);
  settled-tuple §11.3 item 1 SATISFIED; **THE E4 GATE IS LIFTED**
  (R2-E plan gate note). DEVIATION RECORDED: anchor nit 1's R2-F
  alternative carrier fired without carrying (R1.1 un-routed by my
  charter) — the Q1 shape now BINDS E4.1, written into the gate note
  with the rest of E4.1's attention set (stale `catalog.rs:10-16`
  allow-reason; tripwire matcher-edge notes). §4 exit legs: push CI
  green-or-running at `027da5b`; **Windows matrix run 33478175102 +
  Platform matrix run 33478177517 DISPATCHED at `027da5b`, in flight
  at commit time** — conclusions recorded on completion; the linux
  leg is the 462/1098 DERIVED→MEASURED touchpoint (driver rewrite at
  next lane touch per convention). *[CLOSED GREEN same day: Windows
  matrix 33478175102 SUCCESS — the split's first Windows compile, the
  named first-dispatch obligation DISCHARGED; Platform matrix
  33478177517 SUCCESS both jobs (macos-14; ubuntu-24.04-arm), linux
  full-lib **1817 passed + 1 ignored = 462 + 1098 + 256 + 1 TO THE
  DIGIT** — the derived pin set SUM-CONFIRMED per the E5 precedent
  (per-partition x86 execution rides the next release verify; the
  driver's DERIVED→MEASURED rewrite at next lane touch); darwin
  1806+1 matches the measured set exactly; push legs
  33477442748/33477442852 green. **R2-F plan §4 exit criteria: ALL
  MET. E4 is open on every criterion.**]* E4.1's attention set gains
  rider (4), the path-constant second-authority pin (operator skim
  2026-09-01, mechanics verified — see the E4 gate note; carried when
  E4.1 is already in `contracts.rs`). NEXT: E4 (on the operator's
  word) → M5c; escape lane still blocked on operator handoff; GwzWt +
  GwzAi await operator direction.

  **POSITION 2026-09-01 (later) — E4 IS IN EXECUTION; E4.1 LANDED.**
  Operator rulings, verbatim class: "proceed with E4"; "land on GO,
  then start e4.2"; "same standing order for the rest of the e4
  train" (GO lands immediately, GO-WC lands after lane-owner folds +
  focused verification, the lane rules escalations as the Fable
  tier, terminal NO-GO returns to the operator, each landing
  auto-launches the next step); "ext4-only is fine for now" (the
  Linux catalog posture, ratified). **E4.1 LANDED at gwz-core
  `e56124b`** — the first production catalog activation. The train:
  hygiene riders (Q1 bounded read + the path-constant pin) →
  activation (the `entry.rs` door, `PersistentFilesystemIdentity`
  capability with an actionable remedy sentence, seven preconditions,
  tripwire 0→1) → the [P1-1] cure (round 1 NO-GO: the refusal sat
  behind dispatch's durable v0→v1 upgrade and wedged interrupted
  ordinary merges on non-admitted filesystems, driven on real FAT32
  by the reviewer; ruled contract: the adapter's viability window
  declines the upgrade and the v0 lifecycle completes; abort
  capability-free via the acquire/acquire_activated split; cap
  re-ruled 300→331-MEASURED on the R1.1 precedent) → the round-2
  fold ([P2-C1] Windows compile gate, one line). Round 2 GO, every
  clause driven. Pins: CA 456 / v1 257 / remainder 1099+1 darwin
  MEASURED; linux 466/1100 DERIVED — Windows matrix 33498089904 +
  Platform 33498092726 dispatched at the landing (the Windows leg =
  the activation's first Windows compile and [P2-C1]'s real proof).
  THREE record corrections landed with this entry: O11's closed-
  negative narrowed to the catalog-lease probe (legacy identity was
  already production-reachable at v0.11.0); amendment §5.2 Ground
  1's probe cite moved to the pre_catalog providers (identity.rs is
  the legacy module); Ground 2's lock-site census 9→10 (the
  viability window). NEXT: E4.2 (first merge record; launches now
  under the standing order — its brief carries [P3-2]/[P3-3]/[P3-4]
  + the §11.3-item-2 duties + O13's creation half), then E4.3-E4.7
  the same loop, then M5c.

  **POSITION 2026-09-01 (later still) — E4.2 LANDED; O14 MINTED; the
  --target handoff rode through.** Mid-train events: E4.1's Windows
  leg went red on ONE test (the Q1 pin's multi-line needle vs CRLF
  checkout — include_str! hands working-tree bytes; .gitattributes
  pins eol=lf only for retained_readers/protocol) → hotfix `f715ddf`
  (the r2d_seam_freeze normalize idiom) → redispatch 33502328880
  GREEN; E4.1's CI story closed on every leg. THE --TARGET HANDOFF:
  on the operator's instruction the other lane's in-flight fix was
  snapshotted AS-IS and pushed (core `c201a01` +914/−55, cli
  `4791eb6`, py `94378b9`, root `f01be3e` w/ the diagnosis doc;
  labeled WIP/unreviewed/suites-unrun; committed with the RELEASED
  gwz 0.12.1 — the workspace debug binary is contaminated by the
  in-flight handle_commit edits); the operator completes it in a
  fresh gwz-dev workspace; incidental find for that lane: gwz commit
  -m refuses a hyphen-leading message value. Baseline on bare
  c201a01: ALL GREEN darwin (CA 456 / v1 257 / remainder 1110+1 —
  the snapshot's 11 remainder tests, zero cfg gates in +646 lines).
  **E4.2 LANDED at gwz-core `7f28907`** (no-ff merge; reviewed
  7214010 + fold 1f47d6e + round-2 fix 3717249 — the fix reverts the
  REVIEWER's own round-1 conflation, owned by the review; rounds:
  GO-WC 1 P2 → GO). The P2 minted **O14 — the §8/§9 write-authority
  gate**: authorize_write/RetainedWriteAuthorityV1 have zero
  production consumers while converted leaf writes are path-based
  (§9 :264-266: not parent authority); THE FORK (convert vs amend
  frozen text) is DECIDED AT E4.6's CHARTERING, escalating to the
  operator if frozen text moves; until then every conversion states
  its opened (durability) vs closed (authority) gate — binding
  E4.3–E4.6. Also landed: the O13 ownership correction (inventory
  empties across E4.2–E4.4; E4.2 retires no file — the pin is now a
  per-file count map), the four stale allow-reason cures, the
  proof-only inherited-vs-established scope. Pins: CA 457 / v1 260 /
  remainder 1110+1 darwin MEASURED (the landing reconciles the
  snapshot's +11 the other lane could not know to pin); linux
  467/1111 DERIVED at Windows 33511535235 + Platform 33511538719 (in
  flight). Disk incident: builder hit ENOSPC at 95% — 4.3G of stale
  R1.2-era targets (lane-owner hygiene debt) + 1.4G anonymous target
  reclaimed; 6.3G free; NOTE the operator's fresh workspace build
  will want ~10G+. NEXT: E4.3 (merge record rewrite — launches now;
  carries O14's interim pattern, row :274's frozen ordering,
  store/rewrite.rs's commit pair, [P3-3]'s Windows-arm disposition
  precedent), then E4.4-E4.7 → M5c.

  **POSITION 2026-09-02 — E4.3 CARVED OUT BY AMENDMENT; E4.2's CI
  CLOSED; E4.4 GATED ON THE NEW CLAUSE.** E4.2's landing matrices went
  GREEN first dispatch at `7f28907` (Windows 33511535235; Platform
  33511538719 sum-confirmed to the digit: darwin 1828+1i, linux
  1839+1i = 467+1111+260+1) — the --target snapshot's first
  Windows/ARM suite evidence, all green. **E4.3:** the conversion as
  chartered was built and REJECTED at delivery by its own builder's
  driven P0 (candidate `c9a7303`, preserved at gwz-core
  `probe/e4-3-detach-window-evidence`): the boundary's
  detach-then-publish replacement opens a crash window in which the
  open merge record vanishes from discovery with no shipped
  reconciler — the record is the root of reconciliation, the one leaf
  nothing recovers — and the shared `commit` put the identity probe
  on every abort (the [P3-C1] class arriving at E4.3); the lane owner
  verified both mechanisms and owned the charter's contradictory
  clause pair. Decision packet put to the operator with three
  options; **operator ruling, verbatim: "proceed with (c)"** — the
  documented RECORD-ROOT CARVE-OUT. Executed as
  `GwzM5-8R2E-RecordRootAmendment.md`: one dated exception to row
  `:280` (commit keeps `rename_durable`; the second cell's "artifact
  actions" read through it for that one path), pins P-1 (the O13 row
  PERMANENT-DOCUMENTED, fail-closed BOTH directions incl. shrinkage)
  and P-2 (the negative tripwire, CRLF-normalized), the new BINDING
  PLAIN-LEASE PROBE CLAUSE for E4.4–E4.6 (enumerate lease-reachability
  BEFORE building; `archive_terminal`-on-plain named), option (a)
  routed to O14's E4.6 fork. **Tier recording:** dual Code+State,
  peer-blind, Fable×2, on the refined tier policy's amendment-tier
  line (2026-08-22) — this dual sits OUTSIDE plan §2's two-dual
  budget on that line's authority (precedent: the E5-era "minted by
  amendment with dual review" determination). Round 1: Code
  GO-WITH-CONDITIONS (1 P2 — the §2 ground overstated "no reconciler
  CAN close", re-scoped to the shipped tree; 34 claims verified, 0
  refuted; its battery line UNFILLED at a harness restart, recorded
  not fabricated), State GO-WITH-CONDITIONS (3 P2: the second-cell
  disposition, §6's archive overreach, six unnamed §7 records; 3 P3);
  all folds executed 2026-09-02; State waived round 2; Code's round-2
  text-diff confirmation in flight. **E4.3-B** (the pins package,
  Opus builder, cap 250, no production conversion) in flight; lands
  with the amendment and the six root-side records (plan O13/O14/
  preamble/gate-note/E4.3 step; E7-Acceptance O13+O1 brackets; this
  entry). NEXT: E4.4 charters under the plain-lease probe clause and
  the record-root analysis duty; E4.5/E4.6 (O14 fork decides
  convert-vs-amend, record-root re-examined); E4.7; M5c.

  **POSITION 2026-09-02 (later) — THE CAPABILITY-FREE LIST STANDS; THE
  REMAINING E4 ROWS CARVE OUT; E4.3-B DELIVERED; DR-1 MINTED.** E4.4's
  charter prep (`GwzM5-8R2E-E4.4-CharterPrep.md`, read-only, run BEFORE
  any build per the record-root amendment's §4 clause) found no record-
  root wall for the archive (the move is atomic) but THE structural
  wall: every raw write site in the two archive files runs only from
  abort/preserve on the PLAIN lease or from GC under no v1 lease — and
  §7 of the prep verified the same for EVERY remaining §10 row (`:279`
  is written by `repo create`/`init-from-sources` and ~14 more listed
  callers; `:276`–`:278` by ordinary merge, commit, abort). The R2-D
  conversion table and R2-E §5.2's capability-free decision were in
  direct structural tension, unrecorded by any adopted record; E4.3 was
  the first symptom. Decision packet put to the operator (A amend the
  list / B carve out / C degraded boundary mode / D = B now + C routed).
  **Operator ruling, verbatim:**

  > D), with FAT32 out of product and out of the lab.
  > Closed. (A) — Ground 2 stands. Ordinary merge / commit / create / abort / GC stay capability-free. Ext4-only was for the checked feature, not "gwz dies on Fedora." Do not amend the list to put those operations on the catalog probe.
  > Now. One dual-tier amendment, not four more conversion deliveries:
  >
  > 1. Record the tension: R2-D "production writes go through the boundary" vs R2-E §5.2 capability-free list. E4.3 was the first symptom.
  > 2. Capability-free list stands. Rows `:275–:279` whose writers are on that list are carved out — raw durable writers stay, documented and pinned (generalize the E4.3-B / O13 inventory shape). Convert only arms already on `acquire_activated`.
  > 3. Re-scope O1, the R2-D milestone, and E4.7: checked-feature writes go through the boundary; capability-free arms are a dated exception, not unfinished work. E4.7 does not retire those writers.
  > 4. Mint or explicitly defer the tier-2 archive sub-surface (do not invent it in E4.4).
  > 5. Route (C) (non-identity / degraded boundary), reader-side record reconciliation, and O14 (convert `authorize_write` vs amend freeze) to one phase-end design round. Not four builders hitting the wall at delivery. Do not start (C) inside E4.
  >
  > FAT32. Not a supported filesystem. No FAT32 volume drive required. Do not spend a row or a dispatch on it.
  > Abort sentence. Settle from the tree: post-publication v1 abort already reaches `write_checked` → `observation.rs` identity. If that path is real, the E4.1 "`--abort` needs no such filesystem" line is over-claim. Fix the sentence (scope it to the activated-lease / capability-free abort you actually shipped, or date the residual). A Linux non-ext4 drive is optional and only if you already have one.
  > Launch now (standalone, either way):
  >
  > * GC: `gc.rs` `decode_production_v0` on archive bytes — completed `--no-ff` since 0.11.0 is un-GC-able. Read-side, no probe. Fix it.
  > * E4.3-B continues (record-root carve-out + tripwire). Unaffected.
  >
  > Do not start E4.4–E4.6 as originally chartered. After the amendment GO: pins package + any remaining activated-lease forward arms only. Park the E4.4 conversion candidate if it assumes archive rides the boundary from abort/GC.
  > Quote this ruling in the amendment and in the E4.3-B / GC briefs. Terminal NO-GO on a different scoping comes back to me; do not pick (A) or start (C) to unblock a step.

  **Executed:** ruling recorded verbatim on disk
  (`GwzM5-8R2E-CapabilityFreeRuling-2026-09-02.md`); the consolidated
  amendment `GwzM5-8R2E-CapabilityFreeAmendment.md` drafted and taken
  through its dual — **tier recording:** peer-blind Code+State, Fable×2,
  on the refined tier policy's amendment-tier line (2026-08-22), outside
  plan §2's two-dual budget on that line's authority (the third such
  dual: record-root, this, DR-1 to come; plan §2 annotated). Round 1:
  Code NO-GO (2 P1 / 5 P2 / 7 P3 — the prep's `:279` "solely
  create/init" refuted: `finalization/execute.rs:51` is reached ONLY
  under `acquire_activated`, the conversion the ruling ORDERS, which the
  draft had carved; the abort sentence false for two more ungated
  plain-lease paths; the abort's legacy probe strictly WEAKER than the
  catalog's ext4 gate; `:275`'s GC writer a dead arm with the live one
  elsewhere and THREE v0-only decode sites; per-file scans unsound for
  mixed files; the digest backstop absent) and State GO-WC (1 P1 — the
  lane owner had WIDENED the ruling on E4.7, dropping its legacy-writer
  clause; narrowed back; 7 P2 / 8 P3; §6's abort scoping ruled NOT
  list-amendment-by-stealth; corpus sweep 50 hits, 34 unowned homes all
  now annotated). Round 2: BOTH GO, terminal; zero unverified claims;
  **ADOPTED 2026-09-02.** THE AMENDMENT: (1) the tension recorded; (2)
  the list stands, (A) closed, FAT32 out; rows `:275`–`:279`'s listed-
  operation writers CARVED OUT permanently under a generalized
  fail-closed inventory (three primitive classes: `durable_fs`,
  `std::fs`, the `write_atomic` family; both-directions; per-ARM scans
  for mixed files) with every row's frozen cells dispositioned; ONLY the
  three `acquire_activated`-only sites in `finalization/execute.rs`
  convert (E4.5/6-B, one small step); (3) O1 re-scoped a second time and
  closes DISCHARGED at E4.7 citing O14 RE-OWNED to DR-1; the Phase-E4
  milestone re-scoped to checked-feature writes; **E4.7 narrowed by
  exactly the ruling's sentence — and a consequential re-own the
  operator should see: the LEGACY IN-PLACE-WRITER RETIREMENT (E4.7's
  first clause) is re-owned to DR-1 as O14's outcome, because the legacy
  writer IS the pre-catalog `CheckedArtifact` every converted path still
  rides; OPEN-R1's retire-the-area question travels with it; E4.7 keeps
  the named allowance-expiry class, E0.2 §7.1's `finish()` record, the
  A-1 reopen check, O1's close and the close-out records;** (4) tier 2
  EXPLICITLY DEFERRED — DR-1 mints the comparable sub-surface BY
  AMENDMENT WITH ITS OWN DUAL; O8's `gc_archived` route re-owns to DR-1
  conditional on (C), the dead family dispositioned by E4.7; (5) **DR-1
  minted — SCOPING NOTE (not a request):** its agenda = the ruling's
  three items ((C) degraded boundary mode; reader-side reconciliation;
  O14's `authorize_write`-vs-amend fork) + lane-routed items with hooks
  (tier-2 minting, ruling point 4; the record-root re-examination, which
  rides O14; the §6 abort narrowing, a sub-item of (C); the legacy-
  writer retirement; O8's checked-archive route). ONE QUESTION NAMED FOR
  THE OPERATOR, not decided: DR-1 opens after E4.7, outside Phase E4 —
  R2-E's phase E8, or a new lane? (6) **the wider abort fact, for the
  operator:** the shipped abort is capability-free on FEWER paths than
  the ruling's premise assumed — three checked-door paths probe today
  (post-publication evidence; `--preserve` with an integrated
  participant; a selected `@root`), two independent of publication, all
  shipped with A1's v1 reverse path; the E4.1 sentence class is re-scoped
  BY PATH at six in-tree homes and OperationModel's three sentences
  ("an abort that touches no checked artifact needs no such filesystem
  …"), dated as a residual cured only by DR-1's (C); no FAT32 anywhere.
  **Standalone, launched on the ruling:** the GC decode fix (three
  v0-only sites; read-side; GC stays capability-free; NOT O8's route) —
  in flight; **E4.3-B delivered** `60072a7` (249/250; P-1 generalized
  into a per-row reason/authority map — each future carve-out one data
  row; P-2 two belts; item 5 declined as structurally undrivable) — in
  interior review. LESSON filed: two builders sharing one target dir on
  different trees is UNSOUND (same test-binary path; cargo runs the
  other tree's binary) — snapshot-binary protocol when forced by disk.
  NEXT: E4.3-B lands (with the record-root amendment's six records and
  all of this amendment's annotations in one root commit); GC fix
  lands; the pins package E4.4-6-B (charter = the amendment's §7);
  E4.5/6-B; E4.7 re-scoped; DR-1 chartered once the operator names its
  home.

  *[2026-09-02 (later): E4.3-B LANDED — gwz-core `0dae0d5` (E4.3-B
  `60072a7` rebased + the landing nit), root `3351895` (both
  amendments' records) + `e7b744f` (the three charter preps + the
  GC fix's round-1 review). Matrices at `0dae0d5` GREEN: Platform
  `33562252461`, Windows `33562249876`. PIN ERRATUM, recorded here:
  the aggregate driver's g23 base pin read 124 where the tree
  measures 126 — E4.1(c) `6688f34` added the two `a1_activation`
  rows while its dated block claimed "and only it"; confirmed by
  measurement in the GC fix's review; the driver corrects it in the
  GC fix's dated block at that landing.]*

  *[2026-09-02 (later still): GC fix commit (d) `c0c9ac5` delivered
  (100/100; the `cfg(test)` mask DELETED, retention row ablation-
  proven, the builder's false "strictly stronger" claim withdrawn in
  its message) — round 2 in flight. **PINS PACKAGE ROUND 1: NO-GO**
  (1 P1, 2 P2, 10 P3; `GwzM5-8R2E-E4.4-6B-Review.md`). The P1: the
  shared masker's `quoted` flag is declared outside the per-line
  loop, so a `'"'` char literal (`merge_support.rs:177,:236`) blanks
  everything to the next stray quote or EOF — 178 live blinded lines
  where the strip it replaced blinded 5, and the E4.3-B door
  declared in that file leaves the tripwire GREEN. P2-1: a twentieth
  carved row, `store/archived.rs::archive` (`:275`, reached from
  ordinary v0 merge and both aborts), pinned by nothing and named
  by no record — the amendment's fourth §1 omission. P2-2: §6's
  release-train restatement undischarged. **Lane-owner ruling** (the
  review's §12, under the E4 standing order): all three CONFIRMED by
  mechanism and MUST CURE — a faithful `mask_non_code` port + fail-
  loud belt + masker self-test + M-live re-drive; the twentieth row;
  the sentence in the fold's message; [P3-5] ruled IN (flat digest on
  `artifact/mod.rs`); the fold delivered on top of the rebase onto
  `0dae0d5`; cap ≤ 589 measured. Round 2 = the same reviewer. Root-
  side (mine, at landing): amendment bracket (9) + the `:458` cite
  (done), the g12/catalog_names pair into E4.7's brief (done), the
  abort sentence's on-disk home in the plan's release-notes rider.]*

  *[2026-09-02: **THE GC FIX IS LANDED** — gwz-core main `3c632ec`
  (`0dccd3e` the shared `decode_archived_common` over three decode
  sites; `c39f6d4` the completed-`--no-ff` row; `b5ff7fc` the retention
  row with the `cfg(test)` mask DELETED — test-only, never shipped;
  `3c632ec` the landing fold: the identity-duplication class named at
  the site, the re-measured pins recorded). Rebased from `7f28907`
  onto `0dae0d5` (the `v1_lifecycle/mod.rs` tree digest recomputed
  twice, per the checker's own algorithm); the pre-rebase shas the
  review cites (`4686c54`/`98f5f90`/`c0c9ac5`) stay reachable on the
  LOCAL branch `gc/v1-archive-decode`. Round 2 GO-WC (§13): [P2-1]
  discharged — the retention row reddens under hunk ablation AND the
  mask deletion turned a pre-existing `store::tests` row into a second
  guard; [R2-P3-1]/[R2-P3-2] folded into `b5ff7fc`'s message at the
  rebase (the ablation scoped; the RELEASE-NOTES LINE block);
  [R2-P3-3] the landing fold. Landing verification on the rebased
  tree from a `--list`-verified snapshot (1835 tests): CA 457, v1 262,
  remainder 1114 + 1 ignored, g23 130 — every pin matched; fmt/check/
  clippy/boundary/M4 map/merge-docs green; per-commit lane gate ok at
  all four. Dispatched: Windows `33569807177`, Platform `33569810180`;
  push CI boundary `33569807827`, retained readers `33569807792`.
  Release-notes line (carrier: the release train), verbatim in
  `b5ff7fc`: "`gwz merge --gc` now collects merges archived under the
  v1 record envelope — every `--no-ff` merge, and any ordinary merge
  that an interrupted-finalization upgrade adapted to v1 — which
  previously could never be collected. Ordinary retention now applies
  to them too … an archive this build cannot read is still never
  deleted." NEXT: the pins package's fold → round 2 → land (rebase
  over this landing: the `store/gc.rs` hunk + digest); E4.5-B.]*

  *[2026-09-02: the pins package's round-1 fold DELIVERED — `086f7c0`
  (b99bfb7 rebased onto `0dae0d5` with the review's §10.4 A–D
  resolutions) + `b31a229` (the fold: `mask_non_code` ported
  faithfully with a fail-loud belt and a 15-row self-test; M-live and
  the five live shapes now RED; the twentieth row `store/archived.rs`
  with its annotation, `PURE_CARVED_FILES` entry and key-set digest;
  §6's sentence verbatim in the message with the release train named;
  [P3-5]'s flat digest on `artifact/mod.rs`; the P3 folds). Counts at
  the `0dae0d5` base: v1 266, CA 457, remainder 1110 + 1. **CAP
  OVERRUN, disclosed:** +506/−68 + +264/−57 = 770 by sum, +713/−68
  net, against ≤ 589; the builder did not stop at the ~120-line
  trigger, reading its OR ("> ~120 lines OR a change outside
  `tests/`") as AND. **Lane-owner re-rule:** cap = the MEASURED
  figure, on the ruling's own overriding clause (a quiet masker under
  an absence pin is the trade this program refuses; line count is
  not); the trigger miss is a PROCESS NOTE (write stop-triggers as
  separate numbered conditions, not a prose disjunction). Builder's
  disclosed behaviour change: block comments are now masked (faithful
  to the Python), so a door inside `/* … */` no longer fires — round 2
  rules on it. Round 2 launched with the same reviewer; the final
  rebase over the GC landing (`3c632ec`) is the lane owner's — REHEARSED
  in `scratchpad/pins-rehearsal2` (two conflicts, the known pair:
  `store/gc.rs` — GC's seam read and comment above the pins' exception
  comment — and the `v1_lifecycle/mod.rs` digest, recomputed at both
  commits), tip `119fcff`, and PRE-VERIFIED there from a `--list`-
  verified snapshot (1839 tests): CA 457, v1 266, remainder 1114 + 1,
  g23 130 — every pin matched; fmt/check/clippy/boundary green. GC
  landing matrices at `3c632ec`: Platform `33569810180` GREEN, Windows
  `33569807177` GREEN; push CI boundary `33569807827` and retained
  readers `33569807792` GREEN — the GC landing is CLOSED on all four.]*

  *[2026-09-02: **THE PINS PACKAGE E4.4-6-B IS LANDED** — gwz-core main
  `f563446` (`9fbe4ac` the package rebased over the GC landing, `a7e5e21`
  the round-1 fold, `f563446` the landing fold: three round-2 doc
  conditions — the masker's cite of `mask_non_code` re-measured
  `:1155-1225`, the twentieth row's provenance sentence un-inverted, the
  quiet-shape count SIX). Round 2 GO-WC (§13): [P1-1] CURED structurally
  (a scanner, no quote toggle) and empirically (all six quiet shapes
  fire; M-live RED; the 411-file differential against the checker's
  masker shows ZERO blinded lines; the belt fires on a real read and
  false-positives on none); [P2-1] twenty rows recount 20/20; [P2-2]
  byte-identical restatement with the release train named; all 25
  digests reproduce; the ~150-line port audited — no duplicated
  scanning, no dead arm. M-f (a door inside `/* … */` no longer fires)
  ruled ACCEPTABLE by the reviewer: a door in a comment is not a call
  and no pin relied on comment visibility. Landing verification on the
  rebased tree (1839 tests, `--list`-verified snapshot): CA 457, v1
  266, remainder 1114 + 1, g23 130 — every pin matched; the focused
  re-verify after the doc folds: fmt/clippy/boundary green, v1 266;
  per-commit lane gate ok at all three; M4 map and merge-docs green.
  Dispatched: Windows `33575785509`, Platform `33575787625`; push CI
  boundary `33575785383`, retained readers `33575785323`. The reviewed
  pre-rebase shas (`086f7c0`/`b31a229`) stay reachable on the LOCAL
  branch `e4/e4-4-6b-capfree-pins`. PROCESS NOTE filed: write stop-
  triggers as numbered separate conditions — the builder read "> ~120
  lines OR a change outside `tests/`" as a conjunction. NEXT: E4.5-B
  launches now from the staged brief at base `f563446`; then E4.7; then
  DR-1 once the operator names its home. *[CI CLOSED at `f563446`:
  Windows `33575785509`, Platform `33575787625`, boundary `33575785383`,
  retained readers `33575785323` — all GREEN.]*]*

  **POSITION 2026-09-02 (E4.5-B) — STOP-TRIGGER (5) FIRED; A DIFFERENT
  SCOPING RETURNS TO THE OPERATOR.** The E4.5-B builder (Opus, base
  `f563446`, report filed verbatim as `GwzM5-8R2E-E45B-Report.md`; no
  commit; the demonstration diff preserved out of tree) measured that
  converting `execute.rs:45` — the marker write, the one activated-lease
  forward arm §7 said converts — STRANDS `gwz merge --abort` after an
  interrupted marker publication: crash inside the checked publication
  (after the authority record and staged goal, before the leaf) → abort
  returns `RecoveryRequired` (raw path: `Aborted`); a second abort
  `RecoveryEvidenceMismatch`; resume `MergePhaseUnsupported`. Ablation:
  delete the `.gwz/checked-artifacts/ca1-*` residue and the same
  interruption aborts cleanly. **Mechanism, verified by the lane owner
  by reading and by re-running the row on the applied diff (FAILED,
  `left: RecoveryRequired / right: Aborted`, 0.9s):** the abort's
  `classify_remove` asks the REVERSE pair (`Bytes → Missing`) but the
  residue's authority was minted for the FORWARD pair (`Missing →
  Replace`); `classification.rs:175-177` returns `Ambiguous` when an
  *[layer corrected by DR-1 S1, 2026-09-03: the refusal fires one frame
  earlier, at `:141-143` on `residue.foreign` — see the E8.3 entry below]*
  authority exists and is not current for the requested transition;
  `abort/evidence.rs:306-311` maps it to `Other`; the evidence shape
  stops being exact; the record enters recovery. This is NOT a detach
  window (`MissingReplace` never detaches — the prep's §2.1 is true) but
  the prep's OWN §2.4 hazard — directional residue — which its §2.5
  table failed to apply to `:45` (marked CLEAR). The lane owner owns
  that contradiction, as it owned E4.3's. Cure class: abort-side
  observer reconciliation in `abort/evidence.rs::file_states` (prep
  §2.2 cure (ii) / §7.1 option (i), 450–600 LOC, plain lease) — the
  ruling routes exactly that to DR-1 ("Do not start (C) inside E4").
  ALSO FALSIFIED BY MEASUREMENT (the lane owner read the path): brief
  item A's premise that the raw write is what creates `gwz.conf/markers`
  — workspace creation's `refresh_conf_integrity_marker`
  (`artifact/mod.rs:383,:392` → `conf_integrity.rs:178-202`, via
  `write_atomic`'s `create_dir_all`) installs it before any merge, so
  refuse-when-missing regresses nothing and the parent bootstrap was
  unnecessary machinery on a false premise; and the bootstrap's
  prescribed placement is unbuildable (`DurableMerge` requires a record-
  derived owner, `identity.rs:112-130`, and `acquire_for_merge_start`
  runs before `create_open`). Verified green before the wall:
  reachability at `f563446` identical to prep §1.2 (acquire_activated
  and nothing else); expected fact Missing proved; no inventory/O13/pin
  at risk. Builder flags 1–12 stand as filed; flag 6
  (`observation.rs:155-158`'s stale `dead_code` allow on
  `parent_is_canonical`) → E4.7's sweep; flags 7, 9, 10 → DR-1's agenda.

  **DECISION PACKET, put to the operator (the lane idles on it — the
  ruling's "a different scoping comes back to me"):**
  - **(a) RECOMMENDED — `:45` joins `:48`/`:51` in the [R2-P3-1] dated
    residual on its own stronger ground (the directional-residue
    window); E4.5-B closes as "does not open as a build"; the
    capability-free amendment's §7 E4.5/6-B disposition is corrected by
    dated bracket (the fresh-workspace premise false; `:45` not CLEAR);
    the three residual sentences at `execute.rs:45,:48,:51` and the
    amendment corrections' in-tree echoes are carried by E4.7 (already
    a docs+allowances step); DR-1's agenda gains the directional-residue
    class with its observer cure, `classify_write`'s Missing-expected
    widening, the preservation-bundle audit and the fixture seeding
    note. Phase E4's conversions are then exactly E4.1 (catalog) and
    E4.2 (the first merge record) — the two the ruling's premise never
    questioned — and everything else is carved, pinned or dated.
  - **(b)** lift "do not start (C) inside E4" for the abort-side observer
    reconciliation alone, then re-open E4.5-B. Against the ruling, ~500
    LOC on the reverse path, and it pre-empts DR-1's one design round
    by cutting the reconciliation shape for one leaf before the marker,
    lock, boundary and record root are designed together.
  - **(c)** another scoping — the operator's.
  Not chosen by the lane: (a) and (c) are amendment-tier; (b) contradicts
  the ruling. E4.7 waits on the answer (its charter changes under (a));
  the E4.5-B worktree and warm target are kept until then.

  **OPERATOR RULING, 2026-09-02, verbatim:**

  > (a). E4.5-B does not open. The marker write at execute.rs:45 joins lock and boundary in the dated residual, on the directional-residue ground (interrupted checked publication strands gwz merge --abort). Do not convert it. Do not lift "no (C) inside E4" for an observer cure.
  >
  > E4.7 carries the three residual sentences and the amendment corrections. DR-1's agenda gains: directional-residue class, classifier widening, preservation-bundle audit (same hazard). Phase E4 conversions are E4.1 and E4.2; the rest is carve-out, pins, GC, and close-out.
  >
  > Tear down the E4.5-B worktree after the record lands. Do not start DR-1 until E4.7 closes. DR-1's home (R2-E E8 vs new lane) comes back to me as a one-line question after E4.7, not inside this step.

  Executed: `:45` joins `:48`/`:51` in the [R2-P3-1] dated residual on
  the directional-residue ground (no conversion; no observer cure in
  E4); E4.7's charter gains the three residual sentences at
  `execute.rs:45,:48,:51`, the in-tree echoes of the amendment
  corrections and flag 6's stale allow; DR-1's agenda gains the
  directional-residue class, the classifier widening and the
  preservation-bundle audit; Phase E4's conversions are E4.1 and E4.2.
  The E4.5-B worktree is torn down with this record (its warm target is
  re-used by E4.7, same base `f563446`); E4.7 launches now; DR-1 does
  not start until E4.7 closes, and its home goes to the operator as a
  one-line question after E4.7.

  **POSITION 2026-09-02 (E4.7) — E4.7 LANDED; PHASE E4 CLOSED.** E4.7
  (Opus builder, base `f563446`, cap 460): `d8c140f` — 171 allow/cfg_attr
  attributes swept, 23 Tier-3 unchanged, 24 actionable = 6 EXPIRED (each
  proven by removal + check + clippy `-D warnings` green, consumers
  read), 16 RE-REASONED PERMANENT pending DR-1 (each proven to bite; four
  inert under an ancestor blanket with measured deltas), 2 KEPT (the
  `gc_archived` family; deletion fires O13's shrinkage arm); the three
  residual sentences at `execute.rs` (marker — directional-residue
  window, ruling (a); lock and boundary — observation-dead window); the
  finish()-reachability record at `cleanup.rs:146-200` — A-1's reopen
  condition CHECKED and NOT MET, DECISION A-1 STANDS; six digests
  re-pinned; no pin or partition moved. Review
  `GwzM5-8R2E-E4.7-Review.md` GO-WC: the P2 — the four inert-allow numbers
  were net clippy diagnostic-count deltas written as item counts (rustc
  reports the outermost dead item; measured 3 / 21 / 4 / 43, one of them
  6× understated) — and seven P3 (the `gc_archived` extent under-
  enumerated: eleven functions + two structs + four members in a third
  file; a stale `:108-111` cite; inertness missing from `reason=`; two
  A1-era allowances no record owns; wording/path nits); all folded at
  landing `9c808ff` (C1–C6) and in the close record (C7). Landing: lane
  gate ok at both commits, M4 + merge-docs green, `--list` 1839
  unchanged, the partitions NOT re-run (review-cost discipline: the
  masked diff adds no code line; the matrices are the belt). Dispatched:
  Windows `33596394193`, Platform `33596396157`; push CI `33596394533`,
  `33596394463`. **THE PHASE CLOSE RECORD:** `GwzM5-8R2E-E4-Close.md` (the
  step ledger, the residual register, the census, the five rulings, the
  carriers). REVIEW-COST DISCIPLINE (operator, 2026-09-02, on the E4.7
  reviewer re-running partitions 42 minutes in): reviewers verify by
  reading and cheap checks; partitions measured ONCE at the lane owner's
  landing; never re-run a suite the builder ran at the byte-identical
  tree; time-box — filed as a standing memory. E4.7's builder's scratch
  symlinks removed; worktree torn down. **OPEN TO THE OPERATOR, one
  line: DR-1's home — R2-E phase E8, or a new lane?** DR-1 does not start
  before the answer; its charter draft is staged with the E4.5-B and
  E4.7 agenda additions.

  *[CI CLOSED at `9c808ff`: Windows `33596394193`, Platform
  `33596396157`, boundary `33596394533`, retained readers `33596394463`
  — all GREEN. The E7-Acceptance O1, O13 and E4 rows are bracketed to the
  close record. The lane IDLES on the DR-1 home answer.]*

  **POSITION 2026-09-02 (late) — RELEASE LANE v0.13.0 BEFORE DR-1.** The
  operator: two shipped bugs (`--target` and `--dry-run` on `add`/`commit`,
  `GwzOverClaimingCommitDiagnosis.md`) fixed and released before DR-1; why
  not caught; whether other parameters share the failure mode. FOUND: all
  five diagnosed defects are FIXED ON MAIN (the 2026-09-01 handoff
  snapshots core `c201a01` / cli `4791eb6`) and NEVER RELEASED — verified
  end to end 2026-09-02 with a debug build of main against the released
  0.12.1 (which still stages every root file under `add -A --target` and
  commits root and member under `--dry-run`; script
  `scratchpad/repro.sh`). WHY NOT CAUGHT: two guard seams, one carrying
  `dry_run` by type and one not; tests asserted plumbing
  (`meta.dry_run == Some(true)`) not behaviour; behavioural and end-to-end
  dry-run tests existed only for verbs on the honouring seam; no negative
  assertion anywhere on commit; docs promised what no test checked. THE
  AUDIT (`GwzParameterPlumbingAudit.md`, Opus, 45-min box): eight more,
  four reproduced — `--dry-run stash drop` DESTROYS the stash (critical),
  `--dry-run stash apply|pop` real, `--dry-run forall` runs the command,
  `--dry-run init` creates the workspace, `gwz --all add|commit` acts as
  `-A`/`-a`; deferred: `--partial`/`--force`/`--sync` accepted and never
  read by most verbs, `materialize --branch` ignores `--force`,
  `TagRequest.all` dead, gwz-py drops `--ssh-timeout` and still widens
  `--all`. Exactly one clap id collides (`all`); the seam split is
  unchanged and reaches gwz-cli's `forall` dispatch. **Operator ruling:
  "roll up all the changes — the merge and bug fixes"** → v0.13.0 FROM
  MAIN (the E4 train ships: catalog activation on `--no-ff` start, the
  first merge record on the boundary, the ext4/NTFS/APFS admission with
  its typed remedy, the pins, the GC fix, the allowance close-out) plus
  the dry-run class fix and the `--all` collision fix, in flight on
  `fix/dry-run-class` (both repos). Then the three-channel cut per the
  RELEASE.md runbooks (core → cli release branch → gwz-py → publish →
  PyPI). DR-1 waits behind the release.

  *[2026-09-02 (later): THE DRY-RUN CLASS FIX LANDED — gwz-core `22f388d`
  (stash apply/pop/drop and workspace-create gated; the mutation-guard
  seam takes `dry_run` and returns `WorkspaceMutationAccess`
  `Mutating|PlanOnly`, all seven callers updated across both repos; g20
  behavioural tests) + fold `5ae6df7`; gwz-cli `d26ce35` (`forall`
  refuses to spawn under dry-run and prints the plan; the `all` id
  collision split — `--all` is the `@all` selector under every verb,
  `-A`/`-a` short-only, since clap refuses two owners of one long name;
  a recursive id-collision test with an empty allow-list; end-to-end
  tests in both flag positions; CLI.md regenerated) + fold `69f2723`
  (the `commit --all` stderr note; rustfmt). Review
  `GwzDryRunClassFix-Review.md` (Opus, 20-min box): GO-WC — the five
  probes broken before and closed after in both positions; P2-1 the
  silent `commit --all` meaning change → the note; P3s folded; DR-11
  (pre-existing `init --update` dry-run message) filed. Lane gate ok at
  both core commits; CLI suite green; reference check green. Pushed
  both mains. The matrices are NOT dispatched at this landing: the
  release cut runs `cargo test --locked` in full and the release-verify
  workflow runs the three-platform matrix at the tag. **v0.13.0 CUT
  STARTED** (gwz-core `scripts/release.py v0.13.0 --push`); release
  notes FINAL at `GwzReleaseNotes-v0.13.0.md`.]*

  **v0.13.0 RELEASED — 2026-09-03, all three channels.** gwz-core
  `ffd4f95` tag `v0.13.0` (off main; the cut's first attempt refused at
  `cargo fmt --check` — the fix builder's core commit was unformatted
  and the lane gate does not run fmt — formatting-only `d8c98a7` landed,
  cut relaunched: `cargo test --locked` 1843 passed, clippy clean);
  gwz-cli `afa78a6` tag `v0.13.0` (release branch, pins core v0.13.0;
  suite green; `docs/CLI.md` current); gwz-py `80538fe` tag `v0.13.0`
  (release branch, pins core v0.13.0; 576 Python tests, wheel built and
  smoke-tested). THE PY CUT NEEDED FIVE ATTEMPTS: `cargo update -p
  gwz-core` refused ("did not match any packages") because `do_merge`
  resolves a `Cargo.lock` conflict toward main, whose entry has no git
  source — RELEASE-SCRIPT FIX gwz-py `5bc5819` (`cargo update
  --workspace` before the re-pin, mirroring gwz-cli's cargo-build
  refresh); then two attempts refused "branch 'release' is checked out
  in another worktree" — the `--keep-worktree` diagnosis worktree at
  `/private/var/folders/…` survived two removal loops (wrong cwd; a
  `/var` vs `/private/var` path grep) until removed by exact path.
  GitHub releases PUBLISHED: gwz-core (body = `GwzReleaseNotes-v0.13.0.md`),
  gwz-cli (dist rewrote the body: 16 assets, Release run `33639415374`
  GREEN), gwz-py (Publish run `33640038038` → PyPI trusted publishing).
  Core release-verify `33639413040`, cli Docs `33639415391`, py CI
  `33639376425`: closure appended below. INSTALLED locally via the
  release installer: `gwz --version` = 0.13.0; the audit's five probes
  and the two original defects re-driven against it — all fixed
  (`--target` leaves the root untouched; `--dry-run` writes nothing on
  add/commit/stash/forall/init; `--all` before commit no longer acts as
  `-a`). The gwz skill (`~/.claude/skills/gwz/SKILL.md`) rewritten to
  the 0.13.0 state (the 0.12.1 warning retired; the `--no-ff`
  filesystem admission noted; `--all`/`-A`/`-a` semantics). DR-1 is
  now unblocked, pending the operator's home answer.

  **POSITION 2026-09-03 — DR-1 OPENED (R2-E PHASE E8, the lane owner's
  default home).** Operator: "run the DR to see what we can do to
  work-around the filesystem type checks." Charter filed
  (`GwzM5-8DR1-Charter.md`: the ruling's agenda — (C) the non-identity /
  degraded boundary mode, reader-side record reconciliation, O14's
  fork, the tier-2 sub-surface, the record-root re-exam, the legacy
  in-place-writer retirement, O8's `gc_archived` route, row `:279`'s
  cell-2 text — plus E4.5-B's additions (the directional-residue class,
  the classifier widening, the preservation-bundle audit) and E4.7's
  (two ownerless A1-era allowances, the `gc_archived` extent, the
  `CatalogOwnerV1` narrowing, the stale inventory-row description)).
  LEAD ITEM: the filesystem-identity design — an Opus investigation
  (75-min box) inventories every filesystem-type and identity check and
  its consumers, ranks the work-arounds (attempt-based detection instead
  of the ext4 magic gate; admitting what the per-leaf probe already
  admits with the catalog bound to a gwz-minted instance id; the full
  graded-evidence design; a recorded override) with sizes and the record
  each amends, and drafts the graded design → dual on the amendment-tier
  line, time-boxed. The lane owner's concept (conversation, 2026-09-02):
  identity by attempt not magic number; tiers recorded in the evidence;
  the straight line never refuses; guarantees degrade in the classifier;
  refusal attaches to claims; coverage by injecting "tier unavailable"
  on ext4 — no FAT32 lab.

  *[2026-09-03 (later): the design DRAFT is filed —
  `GwzM5-8DR1-FilesystemIdentity-Design.md` (684 lines) — and its dual
  (Code + State, peer-blind, 30-min boxes, verify by reading) is in
  flight. THE DRAFT'S CORRECTION to the lane owner's concept, verified
  by the lane owner against the kernel source: `FS_IOC_GETFSUUID` is
  generic VFS code that returns `ENOTTY` when the superblock's
  `s_uuid_len` is zero; ext4, xfs and f2fs publish through
  `super_set_uuid`, **btrfs does not** — so attempt-based UUID
  detection alone admits xfs and f2fs and leaves Fedora refused. The
  real gate is the catalog's UUID REQUIREMENT, not the magic number; the
  shipped legacy per-leaf probe (`identity.rs:312-346`) has no
  filesystem test and already admits btrfs/xfs/zfs on the abort path.
  RANKING: (a0) delete the two gratuitous `require_ext4` calls in
  `parent_mode`/`rename_domain` (~10 LOC, no guarantee change); (b) bind
  the catalog to a gwz-minted instance id with the volume fact as
  corroboration and admit what the legacy probe admits (~600-750 LOC,
  amendment-tier on the protocol allocation; the v0.13.0-catalog
  migration is the risk) — THE ANSWER TO THE ASK; (c) graded evidence
  (~2130 LOC, five phases) — the destination; (d) an override flag —
  wrong shape; (a) attempt-based UUID alone — does not answer the ask.
  Two additions the tree forces: tier 1 is LEAF-ONLY (directories have
  no digest); locality is not an identity fact (belongs in the
  `RuntimeAdvisoryLock` capability). Both open observer classes are
  unaffected by tiering. Eight one-line questions for the operator in
  §6; the decision packet follows the dual.]*

  *[2026-09-03 (later): THE DUAL RETURNED — Code axis
  (`GwzM5-8DR1-FilesystemIdentity-ReviewCode.md`, 37 verified / 6
  refuted / 6 unverified) GO-WC; State axis
  (`GwzM5-8DR1-FilesystemIdentity-ReviewState.md`) GO-WC. CONVERGING
  P1, verified by the lane owner in the tree: option (b)'s migration of
  existing v0.13.0 ext4 catalogs cannot work as written —
  `matches_attempt` is a five-way conjunction and
  `durable_target_digest` is a SHA-256 over four directories' canonical
  durable identities (UUID inside each) plus the profile code; a hash
  field cannot be ignored; every existing catalog would go `Ambiguous`.
  The fix shape is dual-tuple acceptance (the ioctl still answers on
  ext4, so the legacy digest can be recomputed), crash-safe and one-way
  — feasible, not cheap: (b) re-prices to ~950-1100 LOC. REFUTED kernel
  fact: tmpfs publishes its UUID from v6.9 (`mm/shmem.c:4405`), not only
  master — so (b) would admit tmpfs unconditionally and the tmpfs
  question is a PRECONDITION of (b). Further conditions: a gwz-minted
  nonce already exists (`bootstrap_ownership_token`) with an adopted
  boundary saying it is not an adoption proof — the instance id must be
  a distinct nonce; (b) needs a locality/lock capability in front of
  the catalog (NFS/FUSE would otherwise refuse without a remedy at
  `parent_mode`, tmpfs would be admitted); a wire field must record
  which corroborator was seen; (c)'s naming rebase omits `family_key`
  (a root-identity change makes prior residue INVISIBLE, not foreign —
  `residue.rs:127`); the frozen clause is the R2-D Interface Freeze §6
  (Track-W), not the I2 contract; the (b)-then-(c) sequence is two
  catalog migrations and two protocol duals — the third option is (c)'s
  P1 protocol shape as (b)'s vehicle. LANE-OWNER VERDICT: ADOPT the
  design's DIRECTION as DR-1's answer — (a0) now; (b) NOT charterable
  until the folds land; (c) the program; (a) alone and (d) declined.
  Revision 2 in flight (45-min box). The decision packet is with the
  operator.]*

  *[2026-09-03 (later): REVISION 2 filed (944 lines, 29 folds) and BOTH
  AXES CONFIRMED in round 2 with zero NOT FOLDED items — **THE DESIGN IS
  ADOPTED as DR-1's answer** (plan phase E8, step E8.1). The drafter's
  and the lane owner's recommendation on sequencing: land (c) P1's
  protocol shape first as (b)'s vehicle — one Track-W allocation, one
  catalog migration, one dual — unless the operator judges the Fedora
  block urgent enough to pay for two. (a0) is on CI. NEXT: the
  operator's nine one-line answers (Q2 sequencing; Q3/Q4 blocking on
  (b); Q5–Q9) → E8.2 build steps chartered (< 500 LOC each,
  foundational-first) → E8.3 the remaining agenda's design.]*

  *[2026-09-03: **E8.1a (a0) LANDED** — gwz-core main `e15b3a4`: the
  two gratuitous `require_ext4` calls in `parent_mode` and
  `rename_domain` removed (nothing newly admitted; `identity` remains
  the one gate), the kernel-version comment corrected to 6.9, the
  self-contradicting admission sentence in `OperationModel.md` rewritten;
  the `pre_catalog.rs` tree digest re-pinned. Verified on CI, since the
  Linux provider does not compile on this host: Linux durable identity
  probe `33644792520` GREEN, Platform matrix `33644795635` GREEN;
  boundary checker, merge-docs and M4 map green locally; lane gate ok.
  E8.3's design investigation (the reconciliation class and the rest of
  the agenda) is in flight.]*

  **v0.13.0 RELEASE-VERIFY: THE UBUNTU LEG IS RED, ON A PIN, NOT A
  TEST (2026-09-03).** Run `33639413040`: the Test step passed the full
  linux suite (1854 passed, 1 ignored — identical to the ARM platform
  matrix's 1854 at `e15b3a4`); the R4b-G batteries then FAILED the
  `lib remainder` partition — "expected '1115 passed' absent". Root
  cause, established by a name-level diff of the verify's test list
  against the darwin `--list` at `f563446`: the dry-run class fix
  (`22f388d`) added FIVE lib tests to the remainder (`g20` ×3 stash
  dry-run rows, `g02::dry_run_create_workspace_creates_nothing`,
  `merge::runtime::tests::mutation_guard::a_dry_run_acquisition_yields_no_write_authority`)
  and the aggregate driver's pins were NOT moved — the builder's charter
  ran only touched partitions, the lane owner's landing verification was
  the focused set (fmt/clippy/boundary/`--list` count), the lane gate does
  not run the driver, and gwz-core's release script runs `cargo test
  --locked` but not the driver. Nothing measured the driver between the
  fix's landing and the tag. Correct pins: darwin 1119 (MEASURED at
  `e15b3a4`), linux 1120 (the verify's full suite 1854+1 minus CA 467
  minus v1 267, and the name-level diff: one linux-only row
  `git::tests::g15::root_preservation::stash::exact_handoff_boundary_controls_raw_ignored_or_untracked_membership`,
  one macOS-only row `git::tests::g01::crlf_sentinel_unpinned_worktree_materializes_blob_exact`).
  CONSEQUENCE: the verify checks out the TAG, so v0.13.0's ubuntu leg
  can never go green — the pin is corrected on main and rides the next
  tag; whether that is a pin-only 0.13.1 or the next feature release is
  the operator's. The Windows leg was still running at this record.
  LESSON, filed as a landing rule: every landing that adds or removes a
  `#[test]` re-measures the aggregate driver's pins, and every landing
  runs the cheap `--list`-per-partition count against the driver's pins
  (seconds) — the full partition run stays the lane owner's once-only
  belt. gwz-py's CI on main is GREEN after the lock refresh
  (`33642798539`).

  *[v0.13.0 CI CLOSURE, 2026-09-03: gwz-core release verify
  `33639413040` — Windows leg GREEN, ubuntu leg RED on the remainder pin
  only (corrected on main at `d6830cd`; the tag's verify stays red by
  construction); gwz-cli Release `33639415374` GREEN (16 dist assets)
  and Documentation `33639415391` GREEN; gwz-py Publish `33640038038`
  GREEN, PyPI `gwz` 0.13.0 live; gwz-py CI on main GREEN after the
  lockfile refresh. The installed 0.13.0 re-drives every fixed defect
  green. The release is COMPLETE on all three channels, with the one
  recorded red leg and its cause.]*

  *[2026-09-03: **E8.3 DRAFT FILED** —
  `GwzM5-8DR1-Reconciliation-Design.md` (the observer/record
  reconciliation class designed once; the rest of DR-1's agenda
  dispositioned). Its load-bearing correction, VERIFIED BY THE LANE
  OWNER by reading the evaluation order: the directional-residue refusal
  fires at `classification.rs:141-143` (`residue.foreign`), set inside
  `inspect_family` (`residue.rs:179-181`, `:205-206`) because residue
  file names are keyed on the ACTION (`action_key` hashes `expected` and
  `goal`), so a reverse-direction survey cannot recognise
  forward-direction residue and marks it foreign — BEFORE the
  authority-current check at `:175-177` that the E4.5-B report, the E4
  close record, the charter and the in-tree residual sentence at
  `execute.rs:71-78` all cite. The site cited was real but not the one
  that fires first; a cure that widened only `matches_request` would not
  have fixed the driven wall. The one cure: a direction-free family
  survey (`family_key` already hashes only root identity and path,
  `authority.rs:196-202`) plus observer-side reconciliation in
  `abort/evidence.rs::file_states`, `publication/live.rs::snapshot` and
  `root/abort.rs`, with the four root-artifact doors widened to an
  optional expected fact. It converts `:277`/`:278`/`:279`; the record
  root `:280` stays carved unless a conditional ~200-LOC step is
  chartered; no probe on any listed operation. Three record corrections
  besides: the `gc_archived` family is twelve functions (adds
  `merge/gc.rs::cleanup_error`); the preservation audit's hazard sits at
  the preservation-ROOT leaves; the `CatalogOwnerV1` cite is
  `catalog.rs:51-60`. Steps S1–S4 on the critical path, none over 450
  LOC; eleven one-line questions. Dual (Code + State, 30-min boxes) in
  flight. LESSON: verifying a mechanism means reading the EVALUATION
  ORDER to the first check that fires, not confirming that a cited
  check exists.]*

  *[2026-09-03 (later): E8.3's dual returned — Code axis
  (`GwzM5-8DR1-Reconciliation-ReviewCode.md`, 31 verified) GO-WC; State
  axis (`GwzM5-8DR1-Reconciliation-ReviewState.md`) GO-WC. CONVERGING
  P1, verified by the lane owner: the cure as drafted CLASSIFIES the
  interruption but nothing EXECUTES the reconciliation — after the
  observers report a reconcilable state, `execute_v1_evidence_rollback`
  calls the writer doors whose own classify gates (`transition.rs`) run
  the direction-bound survey, see the counterpart residue as foreign and
  return a typed Err; the marker's reconciled state is inverted (an
  absent leaf is the reverse op's POST state — the table already answers
  `After` at `classification.rs:253` once the foreign gate is bypassed,
  and the abort's non-pending arm maps it to Baseline at
  `evidence.rs:356`); and nothing retires the counterpart residue, so on
  path-stable leaves a stale forward family would make the NEXT merge's
  publication refuse — a regression against today's raw path. Required:
  a third component — reconciling convergence plus retirement under the
  writer's held lease — with the sizes and S2b's stop-trigger moved.
  Also: Q1's "convert `rewrite.rs::commit`" is barred (shared with the
  plain-lease reverse arms; only a lease split); the marker's conversion
  after the cure needs the operator's word again (ruling (a)'s "do not
  convert it" is unqualified) — added as a §6 question; an unlisted
  observer at `abort/preflight.rs:110-118`; tier-2's deferral re-defers
  what §5 assigned to DR-1. S1 (drive-and-re-record) is GO now on both
  axes. Revision 2 in flight (45-min box); confirmation pass follows;
  the combined packet to the operator after adoption.]*

  *[2026-09-03 (later): E8.3 REVISION 2 filed and BOTH AXES CONFIRMED
  (zero NOT FOLDED) — **THE RECONCILIATION DESIGN IS ADOPTED** (plan
  E8.3). The third component is a named retirement pre-pass under the
  caller's held lease, so no classifier variant or table pin moves; the
  marker's reconciled state is `After ⇒ Baseline`; the one converging
  carry-over (the marker still needs the pre-pass in retire-only mode,
  since the writer's direction-bound gate cannot see `After` while the
  forward family is on disk — E4.5-B's ablation is the proof) is folded
  at adoption. S1 (drive-and-re-record, ~150 LOC) is chartered next; the
  build steps beyond it, and the marker's conversion, wait on the
  operator's answers (the combined packet: E8.1's nine, the 0.13.1
  question, E8.3's twelve).]*

  *[2026-09-03: **S1 LANDED** — gwz-core main `a7adc95` (146/18 vs
  ~150): RED-1 reproduced out of tree with the refusing frame
  instrumented (`classification.rs:141-143` on `residue.foreign`; the
  `:175-177` frame never entered for a counterpart residue, live for
  same-direction classifies); NEW-1 the permanent pin
  `checked_artifact::tests::removal_recovery::a_counterpart_forward_family_refuses_as_foreign_before_the_authority_check`
  (asserts `Ambiguous`, `foreign`, `authority` unbound — the cure must
  flip it deliberately); the marker's in-tree residual sentence
  re-pointed to the true layer in exactly its eight lines (the arms stay
  at `:79/:88/:98`); the `:45/:48/:51` drift re-pointed at four homes;
  `checked_artifact::` 457→458 darwin MEASURED / 467→468 linux DERIVED;
  the `v1_lifecycle/mod.rs` digest re-pinned. Review (S1 section of
  `GwzM5-8DR1-Reconciliation-ReviewCode.md`): GO, two cosmetic nits
  left for S2's package. Lane gate ok; `check_pins.py` green; no
  production change, so no matrix dispatch — the next tag's verify
  covers. The five record homes citing the old layer (the E4.5-B
  report, the E4 close record, the DR-1 charter, the E8.1 design, this
  checkpoint's E4.5-B entry) carry dated correction brackets, left as
  written. The lane now IDLES on the operator's eighteen answers.]*

  *[2026-09-03: **OPERATOR RULING ON DR-1's EIGHTEEN + THE PRODUCT LINE** —
  "The merge runs on every filesystem. Crash recovery is a capability, not
  a gate." The (b) design is NOT built and NOT amended (parked as adopted);
  no catalog nonce, no 0.13.0 catalog migration, no 0.14 tuple, no
  best-effort catalog, no flag recorded in the catalog. **Ship (1)**
  chartered: `GwzM5-8DR1-WarnOrRefuse-Charter.md` — a start-time decision
  (above the bar → catalog as today; below → one warning
  `crash recovery is unsupported on <fs> (<reason>). Merge will continue.
  Use --filesystem-strict to refuse.` and NO catalog; `--filesystem-strict`
  → today's refusal; abort/status/gc untouched; continue re-decides the
  same way on the same volume, silently after one warning); the Linux gate
  identity-based (`require_ext4` deleted, tmpfs/ramfs refused as volatile);
  a `diagnostic` event kind, `MergeRequest.filesystem_strict`,
  `MergeResponse.crash_recovery` (closed-system protocol additions the
  operator allowed the same day — "when did I demand no protocol change?":
  the instruction's ship-(1) line said "No protocol change. No tag.", read
  by the lane as the taut protocol; the operator meant the catalog/record
  formats); R0-L and OperationModel rewritten so the default is not
  ext4-only; W1 ∥ W2 → W3 → W4 ∥ W5, ~1450 LOC; NO TAG. **Ship (2) = M5c**
  on the same decision point, chartered after ship (1); its named
  precondition is charter §4.1 (the record boundary's own persistent-handle
  requirement, the legacy probe, still refuses handle-less filesystems).
  Reconciliation answers 10-18 stand but are OFF the critical path (parked
  list in the plan's E8.2). Ship (1)'s build starts on the operator's word
  after the confirmation page.]*

  *[2026-09-03: **W1 LANDED** (operator's "Start W1 ∥ W2. No tag.") —
  gwz-core main `0900252`, gwz-cli main `fc738eb`, gwz-py main `dbd7adf`
  (all fast-forwards). The five §3.7 slots in `protocol/gwz.taut.py`
  (`MergeRequest.filesystem_strict` slot 8; `MergeCrashRecoveryGap`;
  `MergeCrashRecovery`; `MergeResponse.crash_recovery` slot 11;
  `EventKind.diagnostic` = 8), regenerated on both sides through the pinned
  taut-proto 0.9.1 (`protocol/regen.py`; gwz-py `regen_protocol.py`);
  corpus vectors regenerated; five pre-log wire pins moved deliberately with
  dated comments (`check_log_additive.py`, gwz-py `check_protocol_drift.py`
  + `test_log_protocol.py`, and the MergeRequest parity hex in
  `tests/protocol.rs` + gwz-py `test_codec.py`: map header a7→a8, trailing
  `08 f6`, every pre-existing slot byte-identical); compile fallout only in
  gwz-cli and gwz-py (`filesystem_strict: None`, `crash_recovery: None`;
  no flag yet — W4); three round-trip tests added in `tests/protocol.rs`
  and one IR pin test in gwz-py. Measured once by the builder at the landed
  tree: core lib 1844 passed (pins UNMOVED: CA 458, v1 266, remainder 1119,
  g23 130 — `check_pins.py` ok), `--test protocol` 36, gwz-cli green,
  gwz-py 577 passed, drift check OK. Review: lane-owner read of the full
  diff (mechanical step; no Opus round) — GO. Lane gate ok. W2 continues on
  its branch off `a7adc95` and rebases at landing (disjoint files).]*

  *[2026-09-03: **W2 LANDED** — gwz-core main `e16d37a` (rebased onto W1,
  fast-forward; branch `w2/identity-gate` removed locally and on origin).
  `platform/linux.rs::identity` is now `refuse_volatile_filesystem`
  (`f_type ∈ {TMPFS_MAGIC, RAMFS_MAGIC}` → `Unsupported(Persistent-
  FilesystemIdentity, "volatile filesystem: …")`, a CATALOG-ADMISSION
  refusal only per the operator's ruling; W3 maps it onto the warning) →
  `filesystem_uuid` → `persistent_handle`; `require_ext4` DELETED (no test
  had pinned the name test — nothing rewritten); xfs/f2fs now admitted by
  identity. `describe_volume` on all four platform files
  (`VolumeDescription { name, remote, volatile }`, a WORDING AID;
  Linux name from `/proc/self/mountinfo` by `statx` mount id with a magic
  fallback; macOS `f_fstypename` + `MNT_LOCAL`; Windows volume name +
  `DRIVE_REMOTE`/UNC; `require_ntfs` STAYS). Nine tests added (8 Linux,
  1 macOS, 1 Windows-fixture): CA 458→459 darwin MEASURED (builder, and
  again by the lane owner on the rebased tree: 459) / 468→476 linux
  MEASURED on the platform-matrix dispatch; three digests re-pinned;
  boundary checker ok; `check_pins.py` ok on the rebased tree (459/266/
  1119/130). CI at the builder's pre-rebase head `0104329`:
  platform-matrix `33701500498` success (ubuntu-arm 1863 passed, CA 476;
  macos-14 green), windows-matrix `33701502408` success (1803 passed, 0
  failed). Review: lane-owner read of the production diff — GO; noted the
  builder's four disclosed residuals (no octal decoder — the kernel
  escapes separators so field counting holds; the `/dev/shm` row
  self-skips; `DRIVE_REMOTE` restated locally; the linux-identity-probe
  workflow not dispatched, W2 touches nothing under it). Remaining `ext4`
  mentions: the remedy STRING (W3), the parked `linux_ext4` value
  contract, three unrelated comments. W3 charters now.]*

  *[2026-09-03: **SHIP (1) LANDED — W3, W4, W5** (`GwzM5-8DR1-WarnOrRefuse-Charter.md`). NO TAG.
  **W3** gwz-core `6d56836`: `entry.rs::crash_recovery_decision(root)` →
  `bootstrap::probe_workspace_admission` (the catalog's own admission probe
  through `RetainedCatalogTargetV1::retain`, read-only — creation lives in
  `prepare_final_slot`/`acquire_final`, never on this path); every probe
  `Unsupported`/`Io` → `Unsupported{filesystem, gap}` (gap from
  `describe_volume`: volatile > remote > no-durable-identity; name `None` →
  `unknown`); `Ambiguous` stays an error (workspace integrity, not a
  filesystem property). Start decides BEFORE any lease: strict → typed
  `UnsupportedOperation` (`checked catalog: <sentence>; <remedy>`), no
  lease/record/Git; default → ONE `Diagnostic`/`Warn` event with the
  operator's exact sentence, then `acquire_for_merge_start_uncatalogued`
  (plain lease + both parents via `CheckedArtifact::prepare_parent`); the
  decision is PASSED into `service::run` (no re-probe); Continue decides
  once per process and warns once; `crash_recovery` populated on start and
  continue responses; `validate.rs` refuses the flag off-start
  (`InvalidRequest`); remedy rewritten identity-based; `cfg(test)` seam on
  `HostPlatform`. Five g23 rows (`tests/g23/crash_recovery.rs`) + one
  validate row; the a1_activation floor pin EXTENDED to assert the catalog
  exists and `supported == true` above the bar. Pins: g23 130→135, remainder
  1119→1125 darwin MEASURED / 1120→1126 linux DERIVED; CA 459/476 and v1 266
  UNMOVED; nine digests re-pinned. CI at the landed head: platform-matrix
  `33708546704` success, windows-matrix `33708549015` success (first head
  `1f7f5e8` failed one source-scan pin, `no_ff_wire::…pinned_surface`, fixed
  by one allowlist line; superseded Windows run cancelled by the lane
  owner). Production ~262 LOC, tests ~480 — over the ~650 stop line by test
  volume only, flagged by the builder, accepted. Review: lane-owner read of
  the production diff — GO. **W4** gwz-cli `68b44d5`, gwz-py `da9fb7a`:
  `--filesystem-strict` (start-only, `InvalidRequest` with any lifecycle
  op), Human sink prints `warning:`/`error:` diagnostics once each per
  invocation (de-dup), JSON `crash_recovery` object (gap in the house
  CamelCase after one fold), gwz-py human mode now streams events and echoes
  the same way; `docs/CLI.md` regenerated (clap pin). Held until W3 landed
  so main never offered a flag core ignored. **W5** gwz-core `57502e4`,
  gwz-cli `dccd619`: R0-L rebased on the identity contract (no name test;
  strong table {ext4, xfs, f2fs} as vocabulary; 15-row negative table's
  verdicts intact, tmpfs/overlay now refused on REAL mounts; a real xfs
  loop-mount row on both architectures — probe run `33707433938` green);
  `OperationModel.md` §"Checked Merge Artifacts And Filesystem Identity"
  rewritten (operator's headline verbatim, record-boundary limit stated);
  `commands/merge.md` synopsis + "Crash recovery and filesystems";
  `MergeRecovery.md`, `MachineOutput.md`; manifest +2 assertions (warning
  sentence, headline) — docs gate `ok (13 sources, 165 assertions)` on the
  umbrella after landing. Release notes DRAFT committed as
  `GwzReleaseNotes-v0.14.0.md` (no release is cut by it). Lane gate ok at
  every landing; `check_pins.py` green (459/266/1125/135). **Measured for
  M5c (§4.1):** R0-L's real overlay mount answers `name_to_handle_at`
  EOPNOTSUPP without `nfs_export`, so the record boundary refuses there;
  sshfs unmeasured. **NEXT: charter M5c (ship 2)** with the operator's
  decided default — raw record write when the legacy handle probe fails;
  ordinary merges must not newly refuse overlay/sshfs.]*

  *[2026-09-03: **M5c CHARTERED** — `GwzM5-8M5c-Charter.md`, from a
  read-only mapping of the ordinary-start surfaces (three load-bearing claims
  verified by the lane owner: the archived v1 response returns empty repo
  rows, `merge/response.rs:64-68`; `handle_stage.rs:34` discovers through
  the v0-only store decoder; the boundary's legacy-probe refusal renders
  `UnsupportedOperation`, `observation.rs:372-377`). Findings: dry-run is
  intercepted before the version fork (pin only); root participants are
  present in v1 with one provenance asymmetry and a `gwz stage` gap; the
  v1 path emits no member events and no per-transition state changes;
  completed v1 starts return no repo rows; the record CREATE is the ONLY
  forward-path checked door (rewrite/archive/publications already raw);
  F-3 constrains the raw fallback's placement to `checked_artifact::entry`
  with the primitive relocated to a neutral module; ordinary=v0 is pinned
  across the g23 v0 corpus, the retained-reader manifest, the docs manifest
  and `merge.md`. Design: raw record write GATED to the uncatalogued lease
  (operator's default), shape-parity events, archived-projection responses,
  version-agnostic stage discovery, floor raise with a test-only floor
  override for the v0 corpus, dual on the floor step. Steps A1 ∥ A2 ∥ A3 ∥
  B1 → B2 → C1 (dual) → C2. Decision item: reverse-path doors on
  handle-less volumes — recommendation (i) stated limit. AWAITING THE
  OPERATOR'S READ; nothing builds before it. NO TAG.]*

  *[2026-09-03: **M5c SUPERSEDED — M5d CHARTERED.** Controlling ship-2
  text is `GwzM5-8M5d-Charter.md`. Operator: kill v0 on main (open v0
  is not a merge; one sentence, use 0.13.x); best-effort merge on every
  FS with the ship (1) warning; no crash-recovery work on odd volumes;
  no reverse-path-raw milestone; no 0.14.0 tag until M5d lands. M5c's
  “keep the v0 reader / wrap the v0 corpus” is withdrawn. Dual on the
  compatibility-contract §4–§5 amendment and on the floor+erasure
  landing. NO BUILD until authorized. NO TAG.]*

  *[2026-09-03: **M5d DESIGN ACCEPTED.** Dual GO on revision 3 SHA-256
  `ec4eb0117b8ed9bd11528aaa9dc0bf6d727d6bec42c41180c0aa33db1fbf9da8`
  after `GwzM5-8M5d-ReviewConsistency-3.md` and
  `GwzM5-8M5d-ReviewSafety-3.md`. Accepts ship-2 design only. NO BUILD
  until authorized. NO TAG. I2.md body waits for the close dual.]*

  *[2026-09-03: **M5d ACCEPTED AT REVISION 5 — REVIEW LOOP CLOSED BY
  OPERATOR.** Implementation-lane review
  (`GwzM5-8M5d-ReviewImplementation.md`, GO-WC: two P2 folds) →
  revision 4 → round-4 dual NO-GO (`ReviewConsistency-4`, `ReviewSafety-4`)
  → operator ruling: "0.14 must only be consistent with itself; v0 safety
  is off the table" → RemPlan-3 applied → revision 5 SHA-256
  `df6399662c2c93b3e94072f62cd61856e74fa0b7ef6f6699b685e2ae804e32ec`
  → operator: "we've done enough re-reviews"; no re-verdict. Revision 5
  is the controlling ship-2 design. Landing shape now: parity, raw
  create and class-(ii) suite re-pointing may land on `main` first; the
  close (floor + production §2 refuse + engine deletion + pins +
  CapabilityFree/F-3) is the one dual landing. Decision-time handle probe
  on the workspace root; one Diagnostic; optional `handles_ok` response
  field. Sizing: `GwzM5-8M5d-Review.md` (~2× ship (1) in packages). The
  full M5d corpus (charter rev 5, sizing, implementation review, four
  rounds × two axes, three RemPlans) is committed with this entry; the
  M5c charter carries the supersession banner. NO TAG.]*

  **THE CITATION-DRIFT NOTE, FILED (2026-08-27, per the addendum's
  Appendix A — the lane owner's filing choice is THIS checkpoint,
  the corpus's resolution index; the compatibility contract itself
  is NOT annotated with it, precisely because a ~50-line insertion
  there would re-drift every line cite below `:82` a second time).**
  The note, verbatim:

  > **Citation-drift note (2026-08-27, R2-E lane, filed at E0.2b).**
  >
  > **Mechanism.** gwz-dev commit **`4b9f078`** ("A1 activation commit:
  > operator-signed record set lands", 2026-08-25 02:13:43 +1000) inserted a
  > single dated annotation into `dev-docs/GwzM5-8I2CompatibilityContract.md` —
  > one blank line plus an 18-line dated blockquote — for a diffstat of exactly
  > **+19** on that file and no other. Frozen §4 text ends at `:81`; what was
  > `:83` is now `:102`.
  >
  > **Extent.** The drift is **+19 for every line below the 2026-08-25 annotation
  > and ZERO for lines `:1-:82`.** It is *not* a uniform whole-file offset, and
  > the E0.2 draft's "uniform +19-line drift" is true only of the two passages at
  > issue.
  >
  > **Corrected current line numbers**, verified directly rather than by
  > arithmetic:
  > - the whitelist passage ("A1 deliberately whitelists only seven
  >   one-member-workspace…" … "…are not A1 migration rules.") — **`:136-144`**
  >   (9 lines, matching R4b-G's 9-line `:117-125`);
  > - the "Zero whitelist matches is not an error…" passage — **`:178-184`**
  >   (7 lines, matching R4b-G's 7-line `:159-165`).
  >
  > **Six documents still carry the stale pair**, one of them on R2-E's own
  > inherited register:
  > - `GwzM5-8R2DSettledTuple.md:702` **and `:706`** (the C-1 row);
  > - `GwzM5-8R4bG-Evidence.md` §12.7 (`:1298-1305`), **§12.9(d) (`:1469`)**, and
  >   `:1187` (which cites `:123`, now **`:142`**);
  > - `GwzM5-8R4bG-ReviewEvidence.md:119`;
  > - `GwzM5-8R4bG-ReviewCorrectness.md:546`;
  > - `GwzM5-8M5bNoFfDesign.md:112`, `:166`, `:234`, `:321`, `:375`;
  > - `GwzM5-8M5bNoFf-ReviewCode.md:58`, `:79` (citing `:145` → now **`:164`**,
  >   and `:163-165` → now **`:182-184`**).
  >
  > **THE CITING RULE, binding on E5.2 and on any lane citing the compatibility
  > contract: cite content-anchored, not line-anchored.** Give the § number plus a
  > quoted anchor phrase, with the line number offered as a convenience and marked
  > as of a stated date — e.g. *"`GwzM5-8I2CompatibilityContract.md` §5, 'Zero
  > whitelist matches is not an error … byte-preserving archival' (`:178-184` as of
  > 2026-08-27)"*. The contract is a **live annotatable frozen document**: its own
  > annotation mechanism inserts dated blockquotes above frozen text and shifts
  > every line below them, so a bare line cite decays by construction at the next
  > annotation.
  >
  > **What must NOT be done:** silently re-pointing R4b-G's, the settled tuple's,
  > or M5b's existing cites. Those are dated records and are **left as written**,
  > under the same "left as written / this annotation is the sanctioned mechanism"
  > discipline the R2-E amendment applies to the freeze. This note is the
  > resolution mechanism: a reader meeting a stale number resolves it here rather
  > than re-deriving it.

  **THIN-A1 IS DELIVERED — M5's activation gate is closed.** The
  post-A1 queue as chartered: R2-E (67 re-reserved keys [CORRECTED 2026-08-26 at the R2-E plan: the freeze's records and the Completeness bucket (c) say THIRTY-EIGHT — cleanup 11 + barrier 16 + terminal 11; the 67 was a lane-introduced figure with no source]; binding
  obligations incl. [P3-R2-1], [P3-R2-2], the archive/GC consumer
  sub-package, BarrierIntentV1::issue observe-or-refuse), R2-F
  (native evidence, the T-5 pair carrier, C-2's four fixtures, the
  L2-05 multi-repo-checkout cure), **M5c** (minted — the v1
  ordinary-start owner + the floor raise as one reviewed change),
  the escape packages (second lane, BLOCKED ON OPERATOR HANDOFF),
  R3-R6 per RemPlan-4. Operator items open (non-gating): the
  ChangeBudget charging-convention ruling; the executable-template
  policy question; probe-branch cleanup.

  **Windows run 16 (32559979514, at `6c7c8f3`): 1399/4/1.** The
  share-delete fix HELD (os-32 extinct) and all 14 admission tests
  are green. The 4 failures are the namespace fault matrix — its
  first Windows execution — all failing at BASELINE, pre-injection,
  with one typed message: "checked action namespace barrier: private
  durability anchor is not ready" (tests_fault_matrix.rs:376). A
  deterministic Windows arm gap in 2.2's barrier anchor-readiness
  (E9-family territory); diagnosis launched on the clean tree; the
  managed matrix (landed with 2.3) is expected to hit the same wall
  in run 17 — same class, pre-attributed here. Validation runs at
  `8b83a2c` dispatched: 32568481700 (Windows), 32568483432
  (platform).

  **ARM64 attribution correction #2 (L1-16):** the platform run at
  the twin fix proves EBADF EXTINCT (zero os-error-9 in the log; the
  three reverse-preservation EBADF rows cleared), but 46 of the 49
  prior failures were NEVER its cascades: 29 g15::root_preservation
  + 17 v1_lifecycle failures persist with
  `PreservationEvidenceMismatch: "root preservation preparation
  requires the exact durable handoff"` (support.rs:424) — a distinct
  first-Linux-execution class in the preservation-root durable
  handoff — plus one new first-execution test fragility
  (exact_source inode-reuse assumption; tmpfs/ext4 recycles inode
  numbers). ARM now 1374/47. Fable diagnosis package launched;
  non-gating throughout (thin A1).

  **PHASE 1 SETTLED 2026-08-22 at GO/GO — dual #2 of 3 consumed.**
  Round 1: State NO-GO (1 P1 — the 65th-admission catalog-bricking
  capacity gap, a genuine kernel catch; 2 P2, 4 P3) / Code GO (1 P2,
  2 P3), with blind convergence on the E4 retire-edge inventory gap.
  One merged remediation at `bf438ed` (capacity refusal on both the
  new-admission and commit-point paths, typed and test-proven;
  route (ii) on the retire edge — freeze §4.3 activation record, NO
  key mint, 19/165 stands, ratified by the State re-verdict; triad
  refusal restored; all P3s). Round-2 re-verdicts: State GO (zero
  new findings; `can_admit_new` deferral ruled not-a-gap) and Code
  GO (capacity code ruled correct; the remediator's 302/0 vs the
  reviewer's 274/0-anchored count noted as an evidence-recording
  discrepancy only, green both ways). The admission kernel is
  settled; Phase 2.3/2.4 and Phase 3 unblock behind their
  dependencies' reviews. Pushed with the ARM64 train; both matrices
  dispatched at the push.

  **Step 1.3 DELIVERED and PARKED with this record (2026-08-22):**
  all 19 `admission.*` keys given injection sites in the one
  owner-private mutation file (driver holds zero — it decides, never
  mutates); interruption/restart/convergence executed per-key on BOTH
  target variants; 12-round same-boundary crashes at the two genuinely
  re-crossable boundaries prove stable slots (the write-boundary
  non-repeatability finding is recorded in the report for the settle
  reviewers); activation flipped Reserved→Executed with EXPECTED_KEY
  counts HELD at 19/165 — no key minted; `checked_artifact::` 271/0;
  the memo §3.5 row and source-count note updated as the activation
  record. **The Phase 1 package (Steps 1.1-1.3) is complete; the dual
  settle (dual #2 of 3) is launching on it.** Note for reviewers on
  the record: commits 5a7ff0f (1.1+1.2) reached origin early beneath
  a hotfix push (incident + ritual below); the settle judges the
  committed object as published. Driver-lane disclosures on record:
  its one whole-crate `cargo fmt` slip canonicalized whitespace in
  up to five of the D3 lane's in-progress files (no semantic change;
  not reverted to avoid destroying uncommitted work).

  Incident record (L1-16, lane owner, 2026-08-22): the M5b acceptance
  commit `8c1624a` accidentally swept the Phase-1 driver's in-flight
  checker inventory lines (a CATALOG_PUBLICATION_CALL_COUNTS entry
  for a not-yet-committed module) via a whole-file `gwz add`, and its
  v1_lifecycle tree pin had been computed from the dirty shared tree
  — the pushed checker crashed on pristine extraction. Corrected
  within minutes at `558f834` (pristine-worktree corrective commit:
  foreign lines removed, pin recomputed from committed content,
  checker green on pristine). Rituals hardened: shared mutable files
  are staged from pristine worktrees or per-hunk while lanes are in
  flight, and digest pins are computed from the pristine extraction
  of the commit being built, never the dirty tree. The driver lane
  was notified to re-apply its counts entry as part of its own
  package.
- The escape amendment stays second-lane/blocked on operator handoff
  (unchanged). The paused-state facts below remain the baseline the
  waves execute against.

## Pause record 2026-08-16 (baseline for the resume)

Operator paused the program ("pause after all the current tasks are
finished and create a checkpoint") after the thin-A1 instruction was
executed. All in-flight agents completed before the pause; the tree is
clean; nothing is mid-edit. **gwz-core local HEAD `d32b2c9` is one
commit ahead of `origin/main` (`90d3f8a`) and is deliberately
UNPUSHED**: it parks the R2-D Phase 0 freeze package, whose acceptance
gate (dual review) has not run — push only with a review GO/GO train
or by operator instruction. Everything else is committed in the root
repo only (docs), which is never pushed by the implementor.

State at pause, by lane:

1. **R2-D (my lane, the A1 path).** Plan ADOPTED (§9 record). Phase 0
   package DRAFTED and PARKED at `d32b2c9`: freeze memo
   `GwzM5-8R2DInterfaceFreeze.md` (DRAFT, 692 lines — five frozen
   seams, per-platform Track-P table for all 22 edges, six adopted
   owner decisions, three-dual tier statement), backend trait delta
   (four `managed_operation_unavailable` defaults → required),
   admission owner skeleton, 11 green scaffolding tests incl. the
   macOS-green admission publish/retire spike
   (`tests_admission_spike.rs`, 2 tests) against the sealed
   `publish_verified_no_replace` family, RemPlan §10 append-only
   activation-map annotation, digest pins refreshed (checker green).
   Gates: fmt/clippy/checker green; full suite partitioned
   1370/1371 pass, 0 fail — the one unexecuted test
   (`root_fault_matrix::every_root_physical_and_successor_boundary_
   recovers_without_repeating_mutation`, workspace_ops, >585s alone)
   is pre-existing, outside this package's file set, and was green in
   CI at `90d3f8a` (run 13); confirm on the next matrix run.
   Track-P verdict: NO new platform primitive; ONE in-seam Phase-1
   obligation recorded as memo contingency C-2 (admission arms for
   `DirectoryInteriorRecheckV1.expected` / `DestinationRecheckV1`
   inside the sealed primitive — protocol-shaped extension, not a
   bypass). Memo §7 corrects plan line-drift, including one material
   correction: `LeafObserver` has ZERO impls on this tree (the plan's
   `contracts.rs:183/:236` citation was wrong); Phase 2.1 writes the
   first.
2. **Thin-A1 gate-chain amendment (my lane).** DRAFT committed;
   round-1 dual FILED: Consistency NO-GO (2 P1 — missed supersession
   targets `GwzM5-8R4bR2ConsumerCheckpoint.md:21-23`/`:404`,
   `…-RemPlan.md:605-607`, and the frozen M5b `:987-989` dependency
   clause vs §5's "Untouched" row) and Safety NO-GO (3 P1 — the ~3.5k
   LOC of R4b P2/P3/P4 lifecycle lanes whose only scheduled
   independent acceptance was the deferred R6 must be NAMED as an
   A1-activation-review exception; more missed live sentences + the
   ConsumerCheckpoint §14 ninth stop clause needs an explicit
   post-A1 re-scope; the three-dual cap must name the operator
   override of §4.2 and stop citing the empty metrics table). All
   five P1s are text/disclosure fixes that keep the descope intact.
3. **Operator-escape amendment (second lane, parked).** DRAFT
   committed; round-1 dual FILED: Code NO-GO (3 P1) / State NO-GO
   (6 P1), 0 P0 anywhere; blind convergence on the unamended
   cursor-derivation contract. Non-gating for A1. Remediation is
   round 2 of 2 and is BLOCKED ON OPERATOR HANDOFF (thin A1 lane
   split) — this implementor does not pick it up.
4. **Tracked, untouched:** D3 cursor implementation (A1 window);
   ARM64 EBADF substrate package (lead linux.rs:173-181); MAX_PATH
   relocation (4.3 decides); v0 wedge runbook reproductions (Q9,
   second lane); panic class-B conversions (second lane); M5b N-4
   wording; probe-branch cleanup (operator's call); R2-D plan §6
   step-count nit.

**Resume order (estimated first hour):**
(a) verify tuple: root HEAD = the pause-checkpoint commit, gwz-core
local `d32b2c9` unpushed over `90d3f8a`, trees clean;
(b) thin-A1 remediation — one merged patch for the 5 P1s + focused
re-verdicts from the same two reviewers (contexts intact), then the
RemPlan-4 supersession banner on GO/GO;
(c) launch the R2-D Phase 0 dual review (dual #1 of 3, peer-blind,
Code+State axes, cross-model where available) on the parked package;
(d) start Phase 1 (R2-C3) failing tests the moment (c) is in flight
(pipeline rule, `GwzFasterProposal.md` §2); push the Phase 0 train on
its GO/GO. (b) and (c) may run in parallel.

## Exact tuple

| Repository | Commit | Note |
| --- | --- | --- |
| gwz-dev (root) | this docs commit (the pause checkpoint) | literal restatement per ReviewCode-3 P3-5 |
| gwz-core | `78badbc` = origin/main — **the R4b-G gate tooling train** ("Land the R4b-G gate tooling: privacy probe, call-graph guard, M4 scenario checker, aggregate driver"), sitting one commit over **the R2-D settled object `b91bdeb`** (2026-08-23; the `d32b2c9` row above the earlier correction was the 2026-08-16 pause state, long since landed through the Phase 0→5 trains) — **[row re-pinned 2026-08-24 per the R4b-G Evidence axis [P3-6](i) and the Correctness axis C-7.2: it named `b91bdeb` and was one commit stale; it now tracks the tooling train, which is the R4b-G dual's review object]** | R2-D SETTLED at `b91bdeb`; matrix green runs 18-22 (run 22 = `b91bdeb`). **No native matrix run exists at `78badbc` itself**; `git diff b91bdeb..78badbc` is exactly the five `scripts/checks/` files, **+582/−0, zero `src/` lines**, so run 22's evidence carries to this pin by construction (Correctness axis C-7.4) |
| gwz-cli | `3cca145` | Close R4b P1/P2 remediation gate |
| gwz-py | `929efb0` | Implement R4b reverse merge lifecycle |
| taut | `f008419` | 0.8.x fallible from_cbor line |

Exact hashes for "this coordinated commit" rows are pinned by
`gwz.conf/gwz.lock.yml` in the same commit; the next docs-only commit
restates them literally (per ReviewCode-3 P3-5).

Workspace lock pins verified equal to member HEADs at writing. Released
line: v0.10.5 (see `GwzMergeCheckpoint-v0.10.5.md`; v0.10.4 superseded).

## Ownership

Implementation lane: **Claude (F5)**, by operator decision 2026-08-15,
effective from the tuple above. The previous implementer's lane is closed at
`da58135`; per L1-06 no other agent writes to the merge/catalog lane without
an operator-recorded handoff. Reviews run cross-model where possible
(`GwzProcessOptimization.md` §4.3).

## Process authority

`AgentProcessRules.md` (as amended 2026-08-15) plus the adopted
`GwzProcessOptimization.md`, which controls where they differ. Tiered review
depth, two-round remediation cap, two-track freeze, and history archiving
(L1-33) are in force from this checkpoint. (Two-track freeze: the policy is
in force now; its checklist and CI wiring land in Phase 3, before the next
contract freeze at I6.) Recorded review tiers: R2-C2 settled re-review —
dual, cross-model where available (mandated tier).

## Operator decision 2026-08-16 — thin A1

Authority: `dev-docs/GwzFasterProposal.md` (operator instruction; passing
that file is the L1-28 decision; committed with this checkpoint update).
Its §2, quoted verbatim:

> **Thin A1.** A1 enables the v1 writer and `--no-ff` on the accepted R4b
> lifecycle after R2-D settles and M5b's already-bound proofs (T-6 +
> clean-tree re-cut) are green. R2-E, R2-F, R3, R4, R5, and R6 are **not**
> A1 gates. They remain real work; they are hardening and consumer
> conversion, scheduled after A1 or in parallel, not in front of it.
>
> Residual you are ordered to accept and name: merge store, archive,
> stash bundles, and related consumers keep their current call graphs
> through A1; `recover_or_create` stays without a production caller;
> legacy writers may still mutate inside `.gwz/checked-artifacts` until
> R2-E. That residual is already the R2-D defer-out (`GwzM5-8R2D-Plan.md`
> §5 items 1–5). Coupling those items to A1 was a later gate-chain
> sentence, not a physical dependency of writing v1 records.
>
> **No further pre-A1 I2 contract trains.** A1 ships on the I2 contracts
> already frozen, plus amendments already accepted (including the durable
> cursor). The drafted operator-escape amendment
> (`GwzM5-8OperatorEscapeAmendment.md`, ~1,760 lines) and any further
> panic-invariant or escape-wire freeze are **not A1 gates**. Keep the
> v0 wedge runbook owed on its own (Q9). Do not launch mandated dual
> review of a third I2 train as a prerequisite to A1.
>
> **R2-D review caps (already adopted; now binding on this package).**
> Dual peer-blind review only at (a) the Phase 0 interface freeze, (b)
> the Phase 1 admission kernel if you still treat Idle↔Preparing as a
> durable-transition kernel, and (c) the Phase 5 settled-tree gate.
> That is three duals maximum, not four. Interior steps are single-axis
> with automatic escalation on P0/P1/P2. Two-round remediation cap:
> a third new architectural root cause on the same object is
> redesign-or-accept, not RemPlan-5. P3s file and continue; they do not
> become packages and they do not enlarge R2-D. While Phase 0 is in
> review, start Phase 1 failing tests (`GwzProcessOptimization.md` §4.4).
>
> **Track P before the freeze is reviewed.** Before
> `GwzM5-8R2DInterfaceFreeze.md` goes to dual review, spike the admission
> publish/retire path on macOS and Windows against the already-sealed
> `publish_verified_no_replace` family. Do not freeze the four
> `managed_operation_unavailable` defaults into required methods until
> each new physical edge names an admitted primitive per platform
> (`GwzProcessOptimization.md` §3.1). Policy is already in force; apply
> it to this freeze.
>
> **Lane split.** You remain the sole writer on merge/catalog/R2-D
> (L1-06). Pre-A1 docs (escape amendment review, panic-conversion
> packages, runbook) and M5b proof/doc work are a second lane. Do not
> pick them up. Record the split in the checkpoint. If a second agent
> is not yet handed off, leave those items listed as "blocked on
> operator handoff" and continue R2-D.

Recorded consequences (its §3 Step A):

- **R2-D settle is the last catalog gate on the A1 path.** The gate-chain
  paragraph below is rewritten accordingly; the superseded clauses are
  named, with verbatim quotes, in `GwzM5-8ThinA1Amendment.md` (drafted
  with this update; mandated dual review as a process/scope amendment).
- **R2-D review tiers, recorded now (supersedes the four-dual listing
  in `GwzM5-8R2D-Plan.md` §4/§6/§9):** dual peer-blind at the Phase 0
  interface freeze, the Phase 1 admission kernel, and the Phase 5
  settled-tree gate — three duals maximum. Interior steps, including
  the Phase 3 and Phase 4 settles, are single-axis with automatic
  escalation on P0/P1/P2. Two-round cap; a third architectural root
  cause on one object is redesign-or-accept.
- **Second lane — blocked on operator handoff; this implementor does
  not pick these up:** operator-escape amendment review handling (its
  Code+State dual review was already in flight when this instruction
  arrived; it runs to completion and its reports file as NON-GATING
  for A1 — remediation and any acceptance ritual are second-lane),
  panic-conversion packages (audit item 5, incl. the two reachable
  class-B sites), the v0 wedge runbook reproductions/publication (Q9),
  and M5b proof/doc work (incl. the N-4 wording erratum).
- **Moved off the A1 gate list (tracked, non-gating):** the
  operator-escape amendment and any further I2 wire trains; panic
  conversions; the ARM64 EBADF substrate package; MAX_PATH relocation
  (Phase 4.3 still only *decides* coexistence); D2 stays release-gated
  (unchanged). Per the instruction's §5, unchanged and still binding:
  L1-03/13/14/15/16/17/19/26/32, wire-freeze-first + L2-04
  retained-reader harness, sealed publication (§4.1), v1 `cfg(test)`
  until the A1 activation review, M5b's zero-production-line ceiling
  with T-6 + clean-tree re-cut, and the R2-D stop clauses
  (RemPlan-4 :1082-1085) — thin A1 waives none of these.
- **Track P before freeze review:** the Phase 0 freeze memo must name
  an admitted primitive per new physical edge per platform (macOS and
  Windows spike against the sealed C2-proven family) before the freeze
  goes to dual review; the four `managed_operation_unavailable`
  defaults are not frozen into required methods until it does.

## Accepted through

- M5-8 packages through R4b P1/P2 remediation: closed (eleventh-round GO/GO).
- R2-C0: accepted (Interface-ReviewCode-3 / ReviewState-3).
- R2-C1: accepted (AggregateClassifier -2 rounds, no P0-P2).
- R2-C2: **accepted** 2026-08-15 at root `f7ba323` / gwz-core `0d8382e`
  after the round-4 GO/GO (see Next ordered actions item 3 for the full
  round history, deviation record, and closed obligations).

## Open findings and obligations

- Amendment-package dual review (mandated tier for amendments): filed as
  `GwzProcessAdoption-ReviewConsistency.md` (GO, 5×P3) and
  `GwzProcessAdoption-ReviewSafety.md` (NO-GO, 1×P2 + 4×P3). All findings
  from both reports were corrected in this same checkpoint; the safety
  reviewer's focused re-verdict on the corrections is recorded in
  `GwzProcessAdoption-ReviewSafety.md` §"Focused re-verdict". Model note:
  this round ran dual same-model (Claude) with fresh-context isolation
  because the second model is not invocable from this session; a
  cross-model pass by the operator's other agent remains open as an
  optional strengthening.
- R2-C2: execute the complete per-fault matrix (L2-14 form), then two
  settled-tree re-reviews (dual — mandated tier), cross-model.
- Windows backlog from the C1-era tree: two isolated compile corrections
  (`a350746`, `d84a30d` in the release-diagnostic clone) to port or
  supersede deliberately; 98-failure matrix to classify (checkpoint doc
  "Windows backlog").
- Pre-A1 checklist (`GwzM5-8ProgressReviewF5.md` §9), status 2026-08-15:
  **item 1 (I2 re-freeze) CLOSED** — the wire-doc banners/TD §1 landed
  2026-08-11 upstream; this docs commit adds the fourth stale contract
  (Compatibility, Amendment-1 §4 normal-build split) with banner + gate
  coverage (11 sources / 147 assertions), single-axis review GO
  (`GwzM5-8I2Refreeze-ReviewConsistency.md`, 2 informational P3s).
  **Item 3: all ten escape decisions DECIDED 2026-08-16 and the
  amendment DRAFTED.** `GwzM5-8OperatorEscapeDesign.md` §10 dispositions:
  Q1-Q7 = the design's recommendations (sixth top-level record field;
  the design's protocol shape; wire pre-A1 with implementation at or
  before A1; byte-identical restore; `--forget-refs` accepted; user
  docs + error pointer; quarantine/restore/force-abandon names).
  **Operator decisions**: Q8 `--reason` OPTIONAL, never mandatory;
  Q9 = (b) runbook-first, no v0 backport unless later requested;
  Q10 = per-side consent with the provable-collapse single-prompt
  refinement ("adopt both recommendations and proceed with the
  amendment"). `GwzM5-8OperatorEscapeAmendment.md` (1,760 lines,
  DRAFT) now carries the wire/transition/protocol deltas — third
  pre-A1 train on the I2 contracts; its §6.4 is the three-train
  composition authority; its §10 records design-citation corrections
  (e.g. `--force` not `--destructive`) and drafter refinements
  pending review adjudication (separate `BeginRollbackOverridden`
  edge because the design's predecessor set could not reach its own
  U1 wedge; doctrine rule 1 refined; `MergeQuarantine*` type prefix;
  16-hex confirm token). **Round-1 dual review COMPLETE and filed**
  (peer-blind, cross-axis): Code NO-GO — 0 P0 / 3 P1 / 5 P2 / 6 P3
  (`-ReviewCode.md`; verdict: all three P1s sentence-scale contract-
  delta closure gaps, core wire/transition engineering sound; wire
  allocations, sixth-field serde story, entry-edge repair all
  verified); State NO-GO — 0 P0 / 6 P1 / 5 P2 / 4 P3
  (`-ReviewState.md`; consent-digest chain, ordinary-abort terminal
  leak, rule-8 decidability, sidecar lifecycle). Blind convergence
  note: both axes independently found the unamended cursor-derivation
  contract (ActionJournal §2 :178-192 — Code P1-1 ≡ State P1-3).
  Recorded **NON-GATING for A1** per thin A1. **ACCEPTED 2026-08-22
  at GO/GO** (operator handoff "burn it" lifted the second-lane block
  for remediation): round-1 dual NO-GO/NO-GO (9 P1, 0 P0) → one
  merged round-2 revision at root `20f1654` (all 14 P1/P2 + every P3
  inside the immutable Q1-Q10 envelope; consent-round redesign:
  one round anchored to the shown digest, atomic both-side collapse
  writes, crash ends the round) → round-2 re-verdicts GO/GO with
  zero unresolved and R-1/R-2/R-3 confirmed unconditionally on both
  axes. Acceptance ritual applied §7 to all five contracts as dated
  banners + 22 §-local annotations with exact replacement texts
  (frozen prose byte-preserved — the doc gate pins it); doc gate
  green at baseline (11 sources, 147 assertions). The escape wire is
  now the FOURTH accepted train on the I2 contracts; implementation
  lands as its own reviewed package(s) after R4b-G, at or before A1
  (Q3). Side finding routed: the accepted
  durable-cursor amendment's "post-GC record rewrite" phrasing is
  inaccurate (`merge/gc.rs:196` shapes the response projection only)
  — wording erratum for its next docs pass, no behavior involved.
  (Design DRAFT status otherwise unchanged; the amendment supersedes
  it where they disagree, per the amendment's authority clause;
  v0-line wedge runbook stays owed independent of A1 per Q9.)
  **Item 5 audit
  half done** — `GwzM5-8PanicInvariantAudit.md` (104 sites: 82 class-A
  proven, 2 class-B reachable with typed twins, 11 class-C; ~300-LOC
  pre-A1 conversion plan in 7 packages; 3 of the review's original sites
  already resolved by the landed P1 slice). Item 4 (durable
  preservation-cursor) is DECIDED — D3 adopted, wire accepted
  (`GwzM5-8DurableCursorAmendment.md`); its implementation package
  belongs to the A1 window, not started per thin A1 §3. Still open:
  `decode.rs:86` removal tied to the A1 diff; the item-5 conversion
  packages are SECOND-LANE (blocked on operator handoff, non-gating).
- Rulebook P3 residue: section-anchor checking in the doc gate (Phase 2
  tooling).

## Windows platform campaign (2026-08-15)

Ledger: `GwzWindowsMatrix-Classification.md`; diagnosis:
`GwzWindowsMatrix-ExactEvidenceDiagnosis.md`. Burn-down 126 → 52 unique
failures across runs 1-6 (W1 index canonicalization, W2 fixture branches,
W3 read-only fsync, W4 publication sharing: all extinct at zero); run 6b
replay bit-identical (deterministic tail, no flakes). Run-7 train
(`f2fceaf`, dispatched 31886821459) carries the sharing-tail disposition
(2 fixed / 6 via the parent.rs publish-handle fix / 15 OS-impossible
injections gated with a positive Windows guarantee test), the os-87
rename twin fix, CRLF/longpaths fixture pins, the W6 generator install,
and cache-on-failure. **CAMPAIGN COMPLETE 2026-08-16: run 11 (`f36d20d`) is GREEN —
1306/0/1 + 29/0; the platform matrix is ACCEPTED** (eleven-run
burn-down 126 → … → 1 → 0; the run-9 regression and its mechanism are
recorded in the ledger; the finalization_root closure came via the
`probe/finalization-diagnosis` single-test probe unmasking a fixture
identity gap). **Revalidated at run 13 (`90d3f8a`): Windows 1322/0/1;
the sibling platform run's macos-14 leg is 1359/0/1 — first macOS
green. Three of four platform legs green; the ubuntu-24.04-arm leg
stays 1094/266/1 (one EBADF substrate fault + cascades, its own
tracked package; lead: linux.rs:173-181 errno allowlist).**
Matrix-green does NOT close the tripwired residuals
(real-Windows exact-evidence satisfiability; stash_save filtered
reset) nor the amendment's OPEN DECISIONS — all remain tracked below.
Next per gate chain — **thin A1** (operator decision 2026-08-16,
`GwzFasterProposal.md` §2, superseding this checkpoint's SCOPE
CORRECTION of the same date; superseded clauses quoted in
`GwzM5-8ThinA1Amendment.md`, ACCEPTED 2026-08-22 GO/GO): **R2-D
settle is the last catalog gate
on the A1 path.** A1 enables the v1 writer and `--no-ff` on the
accepted R4b lifecycle after R2-D settles (its Phase 5 dual gate) and
M5b's already-bound proofs (T-6 + clean-tree re-cut) are green.
R2-E, R2-F, R3, R4, R5, and R6 are NOT A1 gates — real work, kept,
scheduled after A1 or in parallel as hardening/consumer conversion,
with the named accepted residual recorded in the thin-A1 section
above. Position: R0-L and R1 accepted; **mid-R2** (the
catalog-bootstrap slice C0/C1/C2 accepted; R2-D ADOPTED and in Phase
0, TDD-gated per the amendment).
Scoping: `GwzM5-8R2D-Plan.md` is **ADOPTED 2026-08-16** (lane owner;
§9 adoption record carries the six §7 decision dispositions — C3 as
Phase 1 badged R2-C tail; quarantine/relocation preferred direction
for 4.3 coexistence, decided before catalog activation; backend-trait
delta confirmed for the 0.1 freeze; fault map confirmed; dirent
resume-window conditional; tiers recorded at freeze). Execution
begins at Phase 0: `GwzM5-8R2DInterfaceFreeze.md` + failing-test
scaffolding under one mandated dual review.
`GwzM5-8R2F-EvidenceMap.md` (R5/R2-F platform-evidence gap map vs the
green matrix) remains adopted-in-part (this-week list executed).

**A1 decisions ADOPTED 2026-08-16** (operator delegation "proceed with
your recommendations when the decision packet arrives";
`GwzM5-8A1DecisionPacket.md`): **D1** real-Windows satisfiability =
Option B, creation-time filter neutralization (autocrlf=false + eol=lf
pins at create_repo, clone filters-off at the transport funnel;
renormalize command for adopted worktrees post-A1; plus the permanent
fail-closed doctrine note for ident/eol=crlf/foreign residue and an
un-pinned CRLF matrix sentinel ending CI blindness). **D2**
foreign-filter policy = A′ refined refusal (pre-checkout attribute
inspection over the rewrite set, non-passthrough filters refused
pre-mutation, lfs allowlisted) — **release-gated, not A1-gated**: must
land before the next release cut, which is the first to carry the
amendment's disable_filters code. **D3** durable preservation cursor =
minimal durable cursor (per-owner no-op skip rows + reset-completion
bit) as a pre-A1 I2 amendment (mandated dual review; implementation
may trail into the A1 package; may share the escape-design amendment
window without hard-coupling to its pending owner decisions).
Implementation: D1+D2 as one filter-policy package (focused State
review; dual where the doctrine note freezes text); D3 amendment
drafting begins now.

**M5b-IF FROZEN 2026-08-16 (GO/GO)** at design `66117b0`
(`GwzM5-8M5bNoFfDesign.md`; reviews + re-verdicts filed in
`GwzM5-8M5bNoFf-Review{Code,State}.md`): M5b is
semantics-installation-only — the amended contracts already carry all
no-ff wire; the delta is proofs/tripwires/W1 doc amendment under a
ratified zero-production-line ceiling. Settled acceptance is BOUND to
T-6 (the "v0 forged-action resume gate" package — the freeze review's
Code F-1 P1 found production v0 resume executes forged
two-parent-over-ff-able actions today — landed with its two named
suites green) and to a clean-tree tuple re-cut. M5b-IMPL review tier
recorded at freeze: mandated-dual by default, single-axis only for
test/fixture-confined diffs. IMPL merge waits for R4b-G per the frozen
dependency statement. **[Annotation 2026-08-24 (R4b-G Evidence axis
P2-1's bookkeeping ask; Correctness axis C-7.1): this sentence is
overtaken by events and is retained, not struck, as the text the J-1
adjudication is against. The M5b-IMPL merge (`3e60529`, `8c1624a`)
preceded R4b-G; that sequencing deviation is adjudicated
ACCEPTED-WITH-RECORD in the J-1 record above, ratified by both R4b-G
axes at round 1. What still stands from this sentence is the
**settled review**, not the merge: the M5b-IMPL settled review remains
owed pre-A1 on the tier recorded here.]** Tracked: round-3 N-4 (P3
cross-axis wording reconciliation, next docs pass). D3 amendment:
State re-verdict GO at
`e9396a9`; Code re-read pending; acceptance ritual (§7 contract
annotations) on its GO.
Cleanup candidates for the operator: remote branches
`probe/exact-evidence-diagnosis` and `probe/finalization-diagnosis`
(throwaway probes, safe to delete).

Historical record of the campaign's final stretch: run 7 left 24
survivors, all in the (b) exact-evidence cluster. The
**exact-evidence amendment is ACCEPTED 2026-08-16** and lands in this
commit: `GwzM5-8ExactEvidencePlatformAmendment.md` (recovery-grade rewrite
edges blob-exact; checked-artifact private area invisible to the
preservation-image model and recovery cleanliness predicates), dual
review round 1 Code **GO** / State doc-only NO-GO → document-only
remediation → State focused re-verdict **GO** (reports:
`GwzM5-8ExactEvidenceAmendment-ReviewCode.md` / `-ReviewState.md` with
appended re-verdict); three red-green Unix repros; four amended
contracts carry acceptance annotations; probe branch confirmed the
runner's system `core.autocrlf=true` precondition. Run 8 dispatched on
this commit. **Open review debts from acceptance (tracked, not
closed)**: the foreign-filter policy OPEN DECISION (clean-idempotence
precondition; git-crypt-class wedge) and the real-Windows raw-byte
satisfiability follow-up (unrewritten smudged files; ordinary CRLF
worktrees remain outside the satisfiable set — ledger tripwire in
`GwzWindowsMatrix-Classification.md`). New tracked items from the campaign:
resolver Publication-arm diagnosability (`execution.rs:16-21` masks
publication failure causes); MAX_PATH product exposure (~173-char
`ca1-*` names; the private-area relocation option under `.git/` would
retire it — candidate for R2-F scope *[FALSIFIED 2026-09-01 at the
landed R2-F relocation, R1.3: +4 chars under `.git/`, 160 of the 173
are the hex triple, and the landed split deliberately leaves the
`ca1-*`-bearing legacy area at `.gwz/checked-artifacts` — retirement
belongs to the name-shortening class, not relocation; see
`GwzM5-8R2DPhase4Closure.md` §2.4's dated note]*); rollback preflight anchor-dirt
(in the amendment lane's scope addition).

## Next ordered actions

1. ~~Verify the §4.1 sealed-primitive implementation~~ **Done 2026-08-15**:
   `GwzM5-8R2C2PublicationAudit.md` — §4.1 satisfied, no P0-P2, four P3
   dispositions recorded (reserved fault families rescope; sealing
   perimeter extension as bounded package; legacy coexistence to R2-D
   scope; three strict-window tests).
2. Execute the complete per-fault interruption/recovery matrix; record
   per-key executed evidence (not inventory). Progress 2026-08-15: the
   21-key `catalog_bootstrap.*` matrix and the full checked_artifact suite
   executed green on this host (240 passed / 0 failed, macOS,
   workspace-target variant; command `cargo test --lib checked_artifact::`);
   the three P3-4 strict-window tests are implemented
   (`mutation_tests.rs`: in-place byte drift, destination-in-window,
   kind-swap-in-window) and pass; the reserved-families rescope note is
   filed as RemPlan §10. **Complete 2026-08-15**: the §6 parent-creation
   edge is keyed (`catalog_bootstrap.git_parent_create` /
   `.git_parent_reobserve`, family now 23 keys, inventory 163) and the
   matrix is extended to Git-directory targets
   (`restart_and_substitution_matrix_covers_git_directory_targets`, all 23
   keys interrupted+restarted+converged); workspace matrix completeness
   assertion now spans both target variants; checked_artifact suite
   241 passed / 0 failed (macOS host); boundary checker green after the
   deliberate protected-tree digest update for the two edited trees; doc
   gate green. Native Linux/Windows execution remains at the R2-F gate.
3. Settled-tree R2-C2 re-reviews, round 3 (on `c436180`): **State-2 GO**
   (0 open P0-P2; two new P3s routed — the Git-parent dirent-barrier gap to
   the next matrix package/§6 errata, the Windows destination residual to
   R2-F criteria) and **Code-3 NO-GO** (P2-1 directory-interior
   acquisition-window gap, probe-proven; P3-1 checker blind to raw renames
   outside two files, exit-criterion violating; P3-2 comment overclaim;
   P3-3 digest-discipline break at `95d292f`; P3-4 allocation parity; P3-5
   stale tuple table). **Round-3 remediation implemented in this commit**:
   in-window interior re-check inside the sealed primitive for directory
   sources + destination-interior re-check for retirement (Code-3 option
   a), two new hooks + two inverted-probe regression tests (243/243 green);
   subsystem-wide raw-rename caller inventory in the boundary checker with
   exact allowlist + two adversarial checker unit tests; platform.rs
   comment corrected and §4.1 erratum filed (verification assigned to
   round 4); `try_reserve_exact` parity in the primitive; same-commit
   digest refresh (P3-3 discipline adopted as a lane rule). **Round-4
   verdicts: Code-4 GO (0 open P0-P2, 2 new P3) and State-3 GO (0 open
   P0-P2, 1 new P3) — `GwzM5-8R2C2OwnerInterface-ReviewCode-4.md` /
   `-ReviewState-3.md`. R2-C2 is ACCEPTED at root `f7ba323` / gwz-core
   `0d8382e` under RemPlan §9 item 12.**

   Deviation record (per Code-4 [P3-2], L1-16): the remediation's first
   coordinated commit (root `dea0953` / gwz-core `b923109`) landed with the
   boundary gate red — the platform.rs flat source pin was stale after a
   comment-only edit, and a piped invocation masked the checker's exit
   code on the lane; the item-3 sentence above ("same-commit digest
   refresh") therefore overstated adherence for that commit. Corrected one
   commit later (`0d8382e`, pin-only). Lane rules now: gate exit codes
   checked directly (never through a pipe), digest refresh as the literal
   last pre-commit step, and a per-commit lane-gate mechanism is a tracked
   obligation below.

   Tracked obligations from rounds 3-4, status 2026-08-15 (2):
   **Done** — bare-identifier raw-rename counting + alias/fn-pointer probe
   tests (`641f03c`; the rebinding evasion both round-4 reviewers found now
   fails closed, and the §8.13 syntactic-vs-functional interpretation
   carries no load); per-commit lane gate as a mechanism (`89b414a`:
   `scripts/checks/check_lane_commits.sh` runs every pushed commit's own
   boundary checker over the push range, wired into
   `checked-artifact-boundary.yml` with full-history checkout; validated
   red against both historical deviation commits and green on the
   post-floor range; documented floor `ca520e4` excludes the two recorded
   pre-mechanism reds); I2 supersession items verified already fixed
   upstream (banners, updated bodies, TD §1 "as amended", manifest
   coverage — landed 2026-08-11 on the dev line) and hardened with five
   retired-spelling/stale-claim forbidden tripwires (`ca520e4`, doc gate
   now 138 assertions); DirectoryInteriorRecheckV1 doc-comment pointer
   fixed (`641f03c`).
   **Done** — State-2 [P3-1] Git-parent dirent barrier: **CLOSED** at
   gwz-core `660f46c` (AlreadyExists-arm barrier + containing-root flush
   anchored to the scratch edge — the §3/§6 durability-claim anchor;
   family 24 keys, inventory 164, both matrices executed; entrant-arm
   regression drives the real AlreadyExists path). Focused State-axis
   review GO, no escalation:
   `GwzM5-8R2C2DirentBarrier-ReviewState.md`. Its two P3s: the
   resume-window residual (exact-scratch resume paths skip the anchor —
   strictly narrower than what it replaced; correction options recorded in
   the report) routes to the next durability package or a §6-style erratum
   alongside the R2-F power-loss item; the comment/label pass (§5→§3/§6
   miscite, Windows-arm rationale, error label) is applied in the same
   closure commit.
   **Resolved 2026-08-15 (3)** — the standing CI red below was diagnosed
   as a `clippy::redundant_guards` lint in the Linux-only
   `provider/platform/linux.rs:177` (compiled by Ubuntu CI, never by macOS
   dev machines — which is why every local gate missed it; the
   probe-lockfile hypothesis was wrong: the probe already pins the
   lockfile). Fixed by folding the guard into its pattern (gwz-core
   `6edb9cb`); boundary run `31880974224` is the workflow's first recorded
   green push run, which also constitutes the per-commit lane gate's first
   successful CI execution. Lane ritual gains: cross-target
   `cargo clippy --target x86_64-unknown-linux-gnu` where the host
   toolchain permits (full-crate blocked by native build scripts; CI
   remains the authority). Historical record follows:
   standing CI red (pre-takeover): the push-triggered
   `checked-artifact-boundary.yml` run has failed on every push since at
   least 2026-08-14 (runs 31839074966, 31839893461, 31840579164,
   31880023029). Failing element: unit test
   `test_approved_outside_source_target_cannot_hide_an_observer_caller`,
   whose compiler probe builds a temp copy WITHOUT the lockfile (fresh
   `crates.io`/git resolution) and dies on `error: redundant guard`
   (clippy escalation) — environment-coupled, passes locally with warm
   caches. Fix package queued: pin the probe to the repository lockfile
   (`--locked` + copy `Cargo.lock` into the probe tree) and re-verify; the
   per-commit lane-gate step in the same workflow is masked by this
   earlier failure and its own outcome is unknown on CI until fixed.
   Also open — legacy
   interior digest-pinning or conversion at R2-D; Windows
   destination-window + object-binding native tests at R2-F; §2.2
   status-strip **done** at `53323d0` (Refactor.md −99 lines to a
   three-sentence status + changelog, ledger −10, judgment calls recorded
   in the strip report; doc gate 138 green; review-basis list converted to
   series form, unblocking eight more L1-33 archivals — 18 superseded
   rounds now live in `dev-docs/history/`). Remaining archive candidates
   stay pinned by latest-round closure tables and the RemPlan preamble; a
   small L1-08 amendment to L1-33 (closure-table citations do not pin,
   since archived files keep their names) is queued as a future docs
   package.
4. ~~Port/supersede the two Windows compile corrections~~ **Done by
   determination 2026-08-15**: both diagnostic-clone commits were located in
   `/Users/owebeeone/limbo/gwz-core-v0.10.5-narrow` and verified already
   incorporated on the dev line by `f532b1a` — `a350746` ("keep Windows
   anchor names native": all three hunks, the `anchor_roundtrip_name`
   helper, and its `remains_native` test present verbatim in
   `platform.rs:418/495/500/521+`) and `d84a30d` ("use stable Windows file
   identity": `open_named_path`/`identity_from_file` helpers widened,
   re-exported at `record_wire/mod.rs:48`, consumed at
   `abort.rs:103/249/261`, in the equal-or-stricter test-gated form). This
   resolves ReviewCode-3's f532b1a scope-bleed residual: that bleed was the
   unlabeled Windows port. Native Windows compile/run of the incorporated
   form remains unverified from this host (libz-sys cross-compile limit)
   and is owed at the R2-F gate or the next Windows dispatch run.
   **Remaining from this item — superseded 2026-08-15**: push was
   authorized, the dispatch-only `windows-matrix.yml` workflow was added,
   and the classification/burn-down is running as the Windows platform
   campaign (see that section above; ledger in
   `GwzWindowsMatrix-Classification.md`).
5. ~~I2 supersession banners + TD §1 + status-strip~~ **Done**: wire-doc
   banners/TD §1 landed 2026-08-11; status-strip at `53323d0`; the
   Compatibility-contract banner + gate coverage land in this docs commit
   (review GO — see the pre-A1 bullet above).
6. Then platform-matrix acceptance (campaign section above) → R4b-G →
   M5b → A1 per `GwzMergeCheckpoint-v0.10.5.md` resume order.

## Metrics (per checkpoint; §6 of the optimization plan)

| Checkpoint | Sessions | Review rounds | Found at freeze / impl / settled / escaped |
| --- | ---: | ---: | --- |
| (baseline starts with the next accepted checkpoint) | | | |
