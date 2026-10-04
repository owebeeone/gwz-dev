# Windows HTTPS integration and qualification — DRAFT

2026-10-04. **WH1 qualification boundary accepted after dual review; WH2 is
future scope, WH3 is required qualification. Full Windows release remains
NO-GO.** See [acceptance](GwzWindowsHttpsIntegrationAcceptance.md). The operator authorized Windows integration and real HTTPS/Git/CLI/
Python qualification. This proposes a bounded qualification build, not activation,
publication, pushing, tagging or acceptance of general Windows parity.

## 1. Baseline, authority and purpose

Read baseline: root `3db80372ce129f0008f5971ea29bdd6e152be5fb`, core
`28f564a674574eaefa43266d3137be9a4ddc38b8`, SSPI
`582ec001bd2972076ea65a7db87d81d988c6f2e7`, CLI
`6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311`, Python
`e0c5af10b33289a455f662680af8ac12fd24f9d3`. The inherited untracked SSH N2b
prompts/route-mapping draft are outside this object. The owner must record the
complete dependency tuple and dirt when settling the review object; these read
revisions are not an acceptance tuple.

[Current state](CurrentProgramCheckpoint.md),
[native preparation](GwzWindowsHttpsQualificationCheckpoint.md) and
[accepted composition §9](GwzSspiHttpsCompositionDesign-DRAFT.md) control.
The [general Windows parity draft](../gwz-core/dev-docs/GwzTransportWindowsParityDesign.md)
is unaccepted outside its explicit SSPI supersession. This proposal neither
freezes its SSH/Pageant/proxy/HOME grammar nor waives its outstanding proofs.
Root AgentProcessRules as amended by GwzProcessOptimization controls physical
spikes before freeze, exact tuples, dual boundary review and remediation limits.

Accepted native preparation proves eight Windows worker/provider fixtures;
synthetic bindings, idle IPC cancellation and Negotiate selecting NTLM are their
limits. It does not prove TLS/EPA, integrated pools, real Git or installed hosts.
The purpose here is to make those actual paths executable without claiming
unsupported Windows mechanisms or removing the ordinary product guard.

### Exact amendment on acceptance

This document proposes to supersede only the first paragraph of accepted
composition §9, beginning “Production activation remains” and ending “No
generated files change during this draft”, for WH1 qualification builds.
Its accepted replacement permits the existing callable endpoint/host module under
`all(windows, gwz_transport_candidate, gwz_windows_https_qualification)` solely
in disposable qualification artifacts. Ordinary Windows and candidate-only
Windows selection remain unchanged; all remaining composition §9 obligations
and other sections remain controlling. No wire/generated payload changes are
proposed. This qualification-only permission is effective at the acceptance below;
ordinary Windows activation remains prohibited.

## 2. Characterized dependencies

| Current source | Actual obstacle and proposed treatment |
| --- | --- |
| core `src/lib.rs`, `src/git/mod.rs`, `src/git/gitbackend{.rs,/transport_binding.rs}` | Candidate transport/caller binding are `all(unix, gwz_transport_candidate)`. Add an explicitly separate Windows qualification predicate only after the dependency closure below exists. |
| CLI `src/globalargs/dispatch.rs`; Python `native/src/client_host.rs` and route capture/run | The actual original-entry NativeCaller capture and host context attachment have the same Unix guard. A qualification build must execute these paths before handoff, rather than call a detached fixture facade. |
| core `src/transport_host/session.rs`, endpoint construction | Always constructs `ssh_local`/PlacementEndpoint and advertises SSH. Make physical SSH engine optional by admitted platform capability; construct only the actual HTTPS engine on Windows qualification. No successful dummy SSH owner. |
| `src/transport_host/mod.rs`, `endpoint_environment.rs` | Budgets/environment currently derive from SshEndpointConfig; the non-Unix environment arm is compile_error. Separate private pool/I/O budget capture from optional SSH settings and retain the unsupported ordinary Windows boundary. No synthetic HOME required for HTTPS. |
| endpoint `https_pool.rs`, `https_connection.rs`, `shared_reservation.rs`, `ssh_pool.rs` | HTTP uses generic PoolHost/Connector/Resource living under SSH names; shared reservations also implement SSH channel traits. Retain existing portable PoolHost/Connector/Resource and reservation code where it already compiles. Enclose only the actual Unix setup/helper call sites; no generic owner rename or replacement ledger. Preserve mixed-scheme accounting on Unix. |
| endpoint `https_auth/runner.rs`, `file_worker.rs`, `view.rs`, `view/framing.rs` | Unix OsStr bytes, O_NONBLOCK, slash root, /dev/null, executable rules and process groups are real helper dependencies. Configured helpers require a distinct Windows implementation package, not lossy replacements. |
| endpoint `https_auth/owner.rs`, `lookup.rs` | The non-Unix process-group kill is a no-op. It cannot be a Windows cleanup guarantee. Do not admit helpers or compile/select that no-op as a usable Windows launcher. |
| SSPI library, native HTTP bridge, TLS connector | Accepted production mechanisms are retained. No worker protocol, application API or virtual-stream ordering change is needed. |

The first compiler spike must establish the complete transitive closure; the
table is a source inventory, not proof that these are the only edits. Existing
candidate builders' symlink assumptions also need Windows characterization.
Use real file copies where symlink creation is unavailable; record identical
input bytes and exact dependency pins rather than silently use another checkout.

## 3. Ordered work packages and selection boundary

**WH1: HTTPS-only qualification boundary and default-logon path.** Introduce
`gwz_windows_https_qualification`, declared to each affected build script's
check-cfg. Effective qualification is exactly
`all(windows, gwz_transport_candidate, gwz_windows_https_qualification)`.
Reject the qualification cfg without its required candidate/Windows platform
at compile time inside an enclosing cfg_if section. Existing Unix candidate
selection remains exactly `all(unix, gwz_transport_candidate)`. Ordinary Windows
and Windows candidate without qualification retain the existing native Git2
route. Qualification binaries/wheels use fresh external paths and are never
release artifacts by this gate.

Compile the minimum private endpoint/host composition under the union of those
two valid predicates. Keep unimplemented SSH modules, physical construction,
channel trait implementations and Unix helper platform code enclosed separately.
Do not broaden the public transport module to every Windows candidate build.
This makes the already-existing callable host route available in the explicit
qualification build; it adds no public application request or wire field.

Use real existing HTTP connection/pool/worker, Session/mux, stream_io, per-remote
smart transport and Git2Backend routes. Private structural extraction is permitted
only to stop HTTPS pulling Unix mechanisms. Neutral resource disposal retains
the same ledger; optional SSH means absence, not a resource which returns success.
Keep existing SSH method shapes where drivers require them, but return typed
UnsupportedOperation before filesystem, helper, agent, DNS or network effects
when no SSH engine exists. A remote rejected by this qualification transport
must not silently fall through to libgit2 native transport. Explicit existing
native-route controls retain their own behavior and are excluded from positive
qualification evidence.

WH1 advertises only HTTPS and the exactly usable policies Anonymous and
WindowsDefault, with profile 2 and existing limits. Bound, public capability
projection and Open validation must agree. No SshAmbient/SshExplicit,
WindowsConfigured or Gh offer is emitted. A forged unsupported Open refuses
before effects, including before current-logon capture consumption/worker launch.
There is no CLI/Python helper-disable selector today. WH1 qualification
artifacts apply an explicit fixed construction rule at
`TransportRuntime::open_request` (`src/transport_host/mod.rs`, currently the
host-bound `Git2Backend::new()` assignment): inside the exact Windows
qualification predicate, construct `Git2Backend::without_credential_helpers()`
and attach the same `RequestContext` using `with_host_context`. The existing
Disabled policy then maps to WindowsDefault. Outside that predicate keep the
existing AllowConfigured construction. Do not change the constructor defaults,
request schema, CLI options, Python API, ordinary Windows, Unix, or explicitly
selected native-route backend. This is a limited artifact disposition, not an
ambient setting or automatic authentication downgrade. Explicit/forged
WindowsConfigured Opens still refuse before effects, even when a Negotiate-only
challenge could avoid a helper. Do not reinterpret it as WindowsDefault, anonymous or
Basic. This deliberately limited qualification build is not full Windows parity.

Keep a private scheme-neutral budget value for pool/connect/I/O/helper
admission allowances, deriving existing settings at their existing capture point.
Represent SSH settings as absent on Windows. Preserve the existing Unix public
constructors by conversion at their boundary; no new public config API is needed.
The legacy public `SshEndpointConfig::from_environment` must refuse Windows
qualification use rather than manufacture a usable SSH configuration.
WH1 needs no SSH HOME/known_hosts/agent selection. Capture the actual Windows
machine proxy setting once at original runtime entry. Only verified DIRECT is
admitted; a non-direct/PAC/named setting is refused before endpoint effects,
not ignored or translated using Unix environment proxy precedence. Free all
WinHTTP-returned storage, including partial failure. A private direct connector
fixture may supply existing connector-local trust roots, but such a bypass
cannot be counted as installed host environment qualification. No OS proxy
configuration change is authorized here. Windows environment key handling must
respect native case-insensitive names without lossy UTF-16 conversions.

**WH2: configured helper portability.** Depends on WH1's neutral boundary and
separate accepted physical/helper contract. Implement actual Windows helper
executable discovery, lossless owned paths/environment/config parameter transfer,
safe neutral cwd/null config source, bounded file reading and contained child
tree ownership. Until WH2 is accepted, WindowsConfigured/Gh remain unavailable.
WH2 must not be smuggled into WH1 merely to make configured tests pass.

**WH3: integrated qualification and host packaging.** Starts default-logon
qualification after WH1; configured-helper rows wait for WH2. Assemble an
installed qualification CLI and Python wheel, with the accepted fingerprint/
descriptor/worker image contracts and actual routes. Use existing callable
request surfaces; no fixture-only exported authentication API. Qualified cases
and refusals are recorded separately. Full transport release remains a distinct
aggregate/activation gate after other Windows parity work.

## 4. Invariants carried unchanged

NativeCaller is captured on the actual original CLI/native Python entry before
GIL detach, operation/executor threads or endpoint fan-out. Preserve origin thread
liveness, no impersonation, primary SID/LUID/session comparison, launch-time
recheck and owned handle disposal. Python submit/call/cancel/close exercise the
existing independent transport delivery contract; no request gate serializes
unrelated transport streams. Do not replace capture with worker-thread identity.

Every logical Open owns its one positive immutable deadline D immediately before
first checkout/adoption. Allocation, helper and active-I/O clocks remain separate.
401, new physical generation, pool reuse, redirects and native rounds never
reset/extend D. Native selection at timeout zero refuses before credential
publication under the accepted finite-deadline policy. Existing anonymous/Basic
zero semantics are unchanged on supported routes. No hidden 30-second fallback.

Actual final origin Schannel TLS supplies the prefixed `tls-server-end-point:`
binding (32/48/64 digest bytes); missing/invalid binding refuses native auth.
Never substitute a root certificate, synthetic test bytes or prior-generation
binding. Strict TLS trust/hostname checks precede tokens. One exclusive physical
lease owns up to eight native rounds, one source and pinned final U. Native
Complete, authoritative selected mechanism and actual accepted HTTP response are
independent prerequisites for authenticated success. Proxy TLS is not origin CBT.

On terminal/cancel/expiry, revoke publication, discard the physical lease and
retire its route scope. Preserve native/process/Job/I/O and physical pool charges
until each real cleanup completes. No late token/HTTP result revives a terminal
or frees capacity. Cleanup unknown/pending remains visible through existing
operation reporting. Native process containment does not cancel LSASS/external
provider work or prove erasure after forced process exit.

Receive-pack preserves Effect::Possible before POST and forbids replay after
request bytes/effects. Fetch body failure is also not setup retry. Fresh eligible
connect retries keep their exact existing causes/backoff. Do not downgrade a
selected/refused Digest, Kerberos, helper or default identity to another mechanism.

Secrets occupy initialized bounded wiping owners before copying; no Debug/Clone/
logs of credentials, identities, tokens, CBT or native error strings. Account for
Windows UTF-16 transient owners and dependency HTTP/TLS/OS copies honestly. No
mutable globals/TLS, new ambient rereads or generic worker framework.

## 5. Physical spikes before freezing platform promises

WH1 freezes only the qualified compilation boundary, absent SSH/helper admission,
existing caller/deadline semantics and the native DIRECT read. Those physical
primitives must be characterized before WH1 acceptance. Real HTTP/Git/TLS/EPA
and installed-host observations remain WH3 **qualification exit obligations**,
not claims proved by accepting WH1. WH2 helper/Job/path primitives must be spiked
before WH2's own contract freezes. No new TLS/provider primitive is frozen here:
WH1 uses the already accepted connector and SSPI provider implementations.

Run disposable, independently replayable spikes on gianni@dabeest, Windows 11,
pinned Rust 1.95 MSVC, fresh E: ReFS runtime. Raw receipts/runners belong in the
private HTTPS-integration campaign; targets, packages, certificates and runtime
repos remain external. Public tests/CI must not depend on that archive.

| Spike | Required observation and gate |
| --- | --- |
| Dependency/build closure | Exact-source Windows MSVC test/build through private HTTPS closure; no SSH construction or Unix-only imports. Compile guard negative cases and unchanged ordinary Windows route. |
| Default/direct capture | Real native environment case rules and WinHTTP DIRECT capture/storage cleanup on originating entry; no machine setting mutation. Non-direct row may be fake at parser port until an already configured host is available, and must be labeled accordingly. |
| WH3 exit: actual TLS binding | Connector-local fixture root, real Schannel leaf binding and native AcceptSecurityContext verifier requiring CBT; valid/mismatch positives/negatives without system trust installation. |
| WH3 exit: installed worker route | Exact CLI image/Python extension descriptor and packaged worker launch/Hello on Windows, including paths with spaces/non-ASCII. No PATH discovery fallback for SSPI. |
| WH2 only: helper launch | Job attached at process creation before child runs, allowlisted inherited pipe handles, descendant survival challenge, cancellation/drop/reap and retained capacity. Never rely on spawn-then-assign race or parent exit alone. |
| WH2 only: Git config/path | Executed Git-for-Windows origin/UTF-8 output vs UTF-16 filesystem rules, quoting/null/cwd/executable extension and regular-file refusal/ownership, with non-ASCII paths and case-varied environment keys. |

A failed spike is not permission for a success stub. Amend scope/contract and
re-review before dependent implementation. Portable structural code may be
spiked before freeze; unproved platform policy does not become normative merely
because a cfg compiles. No new provider-accepted explicit identity is inferred
from qualification authorization. If WH2 needs another account or credential,
prepare the concrete fixture and ask for that narrow disposition first.

WH2's child Job owner remains private to HTTPS helpers in core; it is not a
second SSPI owner and not a public general process library. It must preserve
host/endpoint helper permit ownership through process, Job and local pipe
completion, including dropped futures, inherited pipe writers and descendants.
Normal child exit alone cannot confirm the descendant tree empty. Windows file
reads retain permits/buffers when the filesystem call remains pending; reject
pipe/device/unsupported path classes before blocking opens. CancelIoEx or retained
blocking worker semantics must be proved, not assumed. Path conversion failure
is explicit ConfigurationRefused; replacement characters are forbidden.

## 6. Qualification matrix and attribution

Public deterministic tests use actual production orchestration with existing
ports for schedules; native tests remain opt-in. The normal runner's exit status
is authoritative; no expected pass-count census gate or concealed timing reruns.

| Area | Required real Windows cases |
| --- | --- |
| Build/guard | Ordinary Windows, candidate-only Windows and qualification build; Unix normal/candidate preservation; illegal cfg rejection and disabled-branch syntax-scope check. |
| Capabilities | Actual Bound and public projection offer HTTPS supported policies only; SSH/configured/Gh and malformed native Open refuse pre-effect. No native transport fallback in qualification context. |
| TLS/auth | Trusted origin NTLM, Negotiate selecting NTLM, native Complete plus accepted response; wrong hostname/untrusted leaf sends no token; EPA-required matched/mismatched CBT; fresh generation after close and same-origin discovery redirect. |
| Pool/effects | Actual reuse within allowed opaque scope, no cross-operation credential reuse, idle reap/caps, replacement generation, concurrent Opens/jobs/per-host; receive-pack POST failure never retried; fetch post-byte failure terminal. |
| Deadline/cleanup | Original D across reuse/401/native rounds, timeout zero native refusal, exact expiry/late token, cancellation during HTTP/native IPC, cleanup retained before replacement admission, shutdown pending/confirmed. Idle native cancellation is distinct from truly stalled provider work. |
| Git | Real advertisement and pack traffic: clone expected tree, fetch changed refs/objects, push ref accepted/rejected; independently verify bare remote refs and content. Native Complete alone is not Git success. |
| CLI | Installed qualification binary executes normal dispatch with the fixed qualification-only Disabled backend policy, original caller capture, real HTTPS route, worker provenance refusal and truthful JSON/human errors/cleanup. |
| Python | Installed wheel call and concurrent submit, transport delivery while application consumer stalls, cancellation/close and origin capture before detach; actual loaded extension/worker Hello and rejected mismatch; assert the same host-bound Disabled → WindowsDefault construction for call/submit. |
| WH2 | Configured explicit identity accepted/rejected with no default retry, Basic existing semantics, helper timeout/cancel/overflow/descendant hangs and missing executable classifications; lossless config includes/path/environment and wiping owners. |

The local fixture server owns native inbound verifier and optional smart-HTTP
backend, bounded separately from the production client. A server-side Git
subprocess used to serve a disposable bare repo is fixture plumbing, not a
client fallback; receipts distinguish those process roles. Do not simulate Git
success with a static HTTP 200. Connector-local roots and disposable leaf keys
stay outside the repo/archive; runners must leave system trust, account, service,
proxy and zone/auth policy unchanged. Record fixed deadlines, attempts, exact
sources, failed runs, exits and owned-process cleanup without sensitive bytes.

Kerberos requires authoritative Kerberos selection and an appropriate domain
fixture; Negotiate selecting NTLM does not close it. Digest/H(Entity), arbitrary
proxy/native407, Pageant/SSH, differing account, genuinely stalled native provider,
TLS algorithm breadth, perf/platform/source batches and release strict-Clippy
debt remain explicit separate rows, not silently PASS or waived.

## 7. Budget, gates and owner dispositions

> **Operator decision, 2026-10-04, after acceptance:** this section's line and file ceilings are withdrawn. That covers WH1's 2,600 lines and 24 files, WH2's 2,000 and 14, and WH3's 2,000 and 16, and with them the ">120% growth" trigger. The structural stop triggers below remain. WH2 and WH3 are planned as steps of about 500 lines. See the [budget disposition](GwzWindowsHttpsIntegrationBudgetDisposition.md).

WH1 proposal ceiling: 2,600 handwritten added production/test lines, 24 source
files across core/CLI/Python plus affected build-script declarations; movement
counted separately and reviewed for preservation. WH2 separate proposal ceiling:
2,000 handwritten lines, 14 files. WH3 public tests/packaging allowance: 2,000
lines, 16 files; private campaign runners reported separately. No dependency/
schema addition is authorized by these estimates. Existing Windows bindings may
need scoped feature additions for WinHTTP in WH1 and Job/process APIs in WH2;
name exact imports/features in the settled physical spike before freeze.

Stop for owner disposition on new public API/wire/dependency/runtime owner,
ownership crossing, unexpected supported-mechanism change or >120% growth.
Descoping must be stated before a ceiling increase. Keep files cohesive; a
new library follows core's candidate-crate rules rather than absorbing core
secrets/protocol. A private extraction alone does not need a new repository.

Physical spike receipt and this boundary receive peer-blind Consistency/Safety
design review before implementation consumes them. WH1's completed cohesive
phase receives dual settled-tree review because platform/capability/caller
ownership changes; add Surface only if an actual public caller-facing shape
changes. WH2's secret/process boundary has mandatory dual review. WH3 aggregate
qualification/installed-host gate is dual, with Surface for caller guide/help
changes. P0/P1/P2 block; original reviewer closes remediation; two-round cap
applies. Ordinary member tests, targeted strict Clippy, formatting, generation
checks and disabled-branch syntax checks pass before settlement. Existing full
core strict-Clippy red is disclosed separately, not converted to a local waiver.

Proposed owner dispositions before freeze are: (1) accept WH1's qualification-only
cfg and explicit HTTPS/default-logon/direct subset, including configured-policy
refusal; (2) accept minimal private budget separation and absent SSH engine;
(3) accept WH2 as separate helper Job/path package with no claims from SSPI Job
proof; (4) agree required proof rows and classify blocked credential/domain/proxy
fixtures honestly. The operator's proceed authorizes the investigation and
qualification goal; independent acceptance and exact tested behavior are still
required before the draft becomes the implementation contract. No broad Windows
parity approval or production activation is implied.


## 8. Executed prerequisite characterization

Private evidence (access required):
`gwz-core-evidence/campaigns/https-integration/runs/2026-10-04-windows-https-portability/`.
Baseline tuple/dirt are in `baseline.json`; exact input bytes in
`input-hashes.json`; named raw receipts retain commands, exits and hashes.

- Exact tracked-source provisioning first failed on Git2's tracked test symlink
  because Mingw tar could not create it. The sole adaptation copied that target's
  identical bytes as a regular fixture file; original exit 2 remains recorded.
  Native readback verified **14,435 source files**, zero content mismatches.
- Forced-visibility `cargo +1.95.0 check --locked --lib -j 4` on MSVC returned
  23 compile errors in the Unix helper/SSH seams. Native Rust/C dependencies
  built; package resolution is retained separately. No dependency version
  upgrade is inferred from adding the already-required candidate packages.
- External prototype v1 failed on the generator's extra closing brace; v2
  failed on the remaining SSH-only `agent_job::start_setup` dependency. Both
  failures remain frozen. V3 encloses that unused SSH method on Unix and passes
  native `cargo check` under candidate plus Windows qualification cfg. The
  unchanged HTTPS job path uses the actual portable Job implementation.
- V3 native readback matched **18 patched source files**. Its exact manifest
  and lock hashes are in `raw/input-readback-v1.stdout.txt`. Scoped binding
  addition is existing `windows-sys` feature `Win32_Networking_WinHttp` with
  `WinHttpGetDefaultProxyConfiguration`, `WINHTTP_PROXY_INFO`,
  `WINHTTP_ACCESS_TYPE_NO_PROXY`, and existing Foundation `GlobalFree`.
- Actual Windows 11 build 26200/E: ReFS, Python 3.13.5, native WinHTTP capture
  returned DIRECT; all returned storage was freed. No settings were changed.

**Limits:** V3 is external, unreviewed characterization, not product code.
It carries an empty unused SSH home in the legacy budget bag to isolate the
compiler question; final WH1 must instead represent absent SSH settings and
use the private budget separation specified above. Its helper facade can never
spawn and always refuses lookup; production must retain truthful absent-helper
ownership. It emits 139 warnings, so this is not strict Clippy acceptance.
Illegal guard cases, compiled public tests, CLI/Python builds, live TLS/auth/Git,
installed hosts, configured helpers, and runtime refusal/effect tests have not
executed in this run. The newly written inbound verifier is unexecuted fixture
source and supplies no qualification evidence. No production source changed.


## 9. Installed caller recipe and construction regression (P2-1)

After WH1 lands, build/provision disposable artifacts with both
`--cfg gwz_transport_candidate --cfg gwz_windows_https_qualification`, using the
accepted SSPI fingerprint producer. The fixture supplies its ordinary workspace,
HTTPS remote and connector-local CA file via existing `GIT_SSL_CAINFO`. It uses
existing `GWZ_TRANSPORT=gwz` to prevent a global native selection; this setting
selects transport, not authentication policy. It changes no user config or trust.

Run the installed CLI through `gwz --transport gwz --json fetch` against that
fixture workspace. Run the installed wheel through normal `Client.fetch()`
and `Client.fetch_stream()` (which calls the existing bridge `submit`) with
normal event/result consumption,
using the fixture workspace. No helper-disable argument exists or is needed:
the qualification runtime itself uses the specified Disabled backend.

Public WH1 regression tests must observe the actual host-bound backend policy
at the shared request construction point and its Disabled → WindowsDefault
mapping, verify the context remains attached, and execute the normal CLI and
Python call/submit route paths. Forged configured/Gh Opens still refuse before
effects. Guard tests prove other builds use the original AllowConfigured backend
and the explicit native route remains unchanged. WH3 then executes the installed
recipes with real TLS/SSPI/Git, independently verifies resulting refs/content,
and reports them separately from these construction tests. The existing helper
failure diagnostic stating CLI/Python always enable helpers must be qualified
for this build; no new user-facing selector is introduced.
