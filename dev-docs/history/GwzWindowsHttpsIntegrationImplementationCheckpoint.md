# Windows HTTPS WH1 implementation checkpoint

2026-10-04. **Status: original implementation reviewed NO-GO; round 1 correction
and native verification in progress.** See the verbatim [Code](GwzWindowsHttpsIntegrationImplementation-ReviewCode.md)
and [State](GwzWindowsHttpsIntegrationImplementation-ReviewState.md) reports and
[merged plan](GwzWindowsHttpsIntegrationImplementation-RemPlan-1.md). Original
member revisions and results below are retained as the initial checkpoint;
the correction's final tuple and closure evidence will be recorded separately.
No blocking finding is self-closed.
This is the accepted qualification-only WH1 boundary and limited basic WH3
proof. It does not accept ordinary Windows activation or the transport release.

| Member | Exact implementation revision |
|---|---|
| gwz-core | `398158b3272e6f3a69132f8375190945dd93192a` |
| gwz-cli | `6ab16d461acb6daf9fca8281eca6384971cc0c44` |
| gwz-py | `5df15766298fbbd97da1d6ecec74c6cc9dd69fda` |

Diff bases: core `c011aaee864fbe56c12a30b17664c099b8e67512`, CLI
`6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311`, Python
`e0c5af10b33289a455f662680af8ac12fd24f9d3`. Dependencies remain SSPI
`582ec001bd2972076ea65a7db87d81d988c6f2e7`, transport
`8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578`, git2-rs
`d13951f7e0bfb6e0efcee1207ac5b140adefa455`.

Authority: [accepted design](GwzWindowsHttpsIntegrationDesign-DRAFT.md),
[acceptance](GwzWindowsHttpsIntegrationAcceptance.md),
[bounded file dispositions](GwzWindowsHttpsIntegrationBudgetDisposition.md),
core `GWZDesign.md`/`GWZRequirements.md` and unchanged SSPI HTTPS composition.
The implementation changes 49 source/test/build files and three mechanical
inventories: 744 gross added, 217 deleted lines, within the 2,600-line ceiling.

The exact Windows + transport-candidate + qualification predicate enables the
real HTTPS pool/per-remote/Session paths. Ordinary Windows and candidate-only
Windows retain their prior route. Illegal qualification guards refuse compilation.
Private settings separate existing pool/I/O budgets from optional absent SSH.
The qualification endpoint advertises HTTPS Anonymous/WindowsDefault only;
unsupported SSH/Gh/configured Opens refuse before effects. Unix helper modules
are enclosed; no successful Windows helper owner, fake HOME or no-op kill is
introduced. Existing constructors/defaults and protocol/caller surfaces remain.
The shared request constructor explicitly chooses Disabled helpers while keeping
its host context, mapping to WindowsDefault solely in qualification artifacts.
CLI/Python retain originating caller capture before fanout/detach; actual WinHTTP
DIRECT capture admits only initialized verified no-proxy output and frees partial
outputs. Native worker, finite deadline, final-origin prefixed leaf binding and
physical disposal contracts are reused.

## Verification and limits

| Check | Result |
|---|---|
| Portable actual backend/context, cleanup, captured snapshot | 1 + 2 + 2 pass |
| Unix HTTPS regressions | 201 pass before final test-only fixture enclosures |
| Public candidate preparation/build tests | 18 pass |
| Disabled-branch scope guard; inventories; changed Rust formatting | pass; inventories 45/19/6; 46 Rust files |
| Actual MSVC qualification tests on final source | 2/2 pass, 2,277 filtered |
| Actual ordinary / candidate-only Windows library checks | both pass |
| Illegal qualification predicate | explicit compile refusal observed |
| Provisioned final CLI; provisioned/installed final wheel | builds pass |
| Actual CLI HTTPS clone/fetch/push | pass; independent exact checkout/ref verification |
| Actual installed Python clone/fetch/push + fetch stream | pass; ordered events; second call with unconsumed events; close pending_local_work=0 |
| Actual CLI negative CBT / untrusted chain / hostname | refuse; TLS negatives send zero native auth rounds |

Positive GWZ traffic uses existing captured extra-CA configuration, full normal
chain/hostname validation and no revocation override. The native fixture observes
Negotiate selecting authoritative NTLM; client PATH excludes Git. Fixture server
CGI/setup and independent ref inspection use Git separately. All observed server
native contexts were disposed and its owned processes reaped. Python reports
peer_cleanup_confirmed=false truthfully; that is not translated to remote cleanup.
Actual MSVC is 1.95.0 on Windows11/E: ReFS. Runtime/builds/venv/packages are external.

Original native test compilation failed on 38 Unix fixture errors; enclosing only
fixture owners retained portable cleanup coverage and the final binary passes.
Python v1 produced a provisioned wheel then returned WinError145 deleting a dev
incremental build directory. Final v2 uses explicit CARGO_INCREMENTAL=0 and passes
packaging/cleanup before force reinstall; unrestricted dev incremental packaging
on ReFS remains unqualified. Earlier Python probe setup attempted to reconfigure
timeout after creating a backend and was correctly refused; later setup configures
first. Failed attempts and corrected source snapshots remain intact.

Strict core Clippy remains RED45 previously recorded diagnostics; three introduced
ones were removed. Existing protocol generator owner-IR pin mismatch is still RED,
with no IR/generated/pin changes. These are not release waivers. Remaining WH3
native adversity/identity transitions/deadlines/cancel, WH2 configured helpers,
provider parity, package/release/performance/selected-source/aggregate qualification
and ordinary activation remain open. The basic consumer-stall test does not prove
all scheduler/reordering adversities. Prior accepted native-provider probes remain
separate evidence; they are not relabelled as integrated tests.

Private evidence (access required): `gwz-core-evidence/campaigns/https-integration/
runs/2026-10-04-windows-https-portability/`. Named raw receipts/runners, original
failures, source hashes, readbacks and portable drafter outputs accompany this
checkpoint. Public gates do not depend on archive access. No push/tag/publication,
OS trust/account/service/proxy/policy changes or general Windows activation.

Next: same exact tuple, peer-blind Code/State review of the WH1 implementation and
honesty of these limited evidence claims. Stop on a new architectural root cause;
file reviewer reports verbatim and independently close blocking findings.
