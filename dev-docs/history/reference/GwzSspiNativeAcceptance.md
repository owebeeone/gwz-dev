# GWZ SSPI native worker acceptance

2026-10-03. **Accepted** after independent Code/State/Surface GO at this exact
reviewed tuple:

| Repository | Reviewed commit |
|---|---|
| gwz-dev | `bb2387d7fdb4b5bb7c16554fabdbf59b245b8b6e` |
| gwz-sspi | `425e13dc011c42e94fdea31779a8e5967aedc82b` |
| gwz-core, reference only | `8cb3a3f01d79699a5ad07b6ec7cfc78321224d31` |
| gwz-core-evidence | `9adf06beab7a15d1e5c22ec1cecf8e35966e1d7c` |

Final member `84266f412b12a31b9b643a51158079a5412470e5` changes only packaged
acceptance/status prose. Executable/API/dependency/schema/fingerprint bytes are
unchanged after review. This accepts native Negotiate/NTLM/shared worker ownership
and bootstrap under the accepted supervision boundary only. Full Windows release
qualification remains **NO-GO**.

## Review and remediation ledger

Original [Code](GwzSspiNative-ReviewCode.md)/[Surface](GwzSspiNative-ReviewSurface.md)
GO and [State](GwzSspiNative-ReviewState.md) NO-GO are preserved verbatim. One
[merged remediation](GwzSspiNative-RemPlan.md) corrected all six finding records.
The same independent reviewers verified their own original counterexamples:

| Finding | Reviewer closure |
|---|---|
| State P2-1: fixture helper ownership across failure/unwind | [State GO](GwzSspiNative-ReviewState-1.md): immediate guard, finite held exit before wait, forced failures and scratch disposal |
| State P3-1: EOF competing with Job containment | State GO: suspended production child, no-kill negative control and actual parent death |
| Code P3-1: stale Testing availability | [Code GO](GwzSspiNative-ReviewCode-1.md): complete guide consistent with current implementation |
| Code P3-2: real Negotiate query/release coverage | Code GO: actual query returned allocation and checked release; production Supervisor path passed |
| Surface P3-1: stale CallerValues availability | [Surface GO](GwzSspiNative-ReviewSurface-1.md): cold caller pages consistent |
| Surface P3-2: missing recipe restore/undo | Surface GO: four values/absence restored, fresh owned root has guarded teardown after evidence retention |

One P2 and five P3 records discovered before acceptance; zero open findings.
Blocking remediation rounds: 1. New architectural causes: 0. Reviewers classified
the private creation extraction and zero-size production audit as preserving the
existing architecture/proofs. Code and Surface independently found stale
unconditional availability in separate guides; recorded as related maintenance
convergence. No production credential exposure, false publication or escaped
product defect was established by these reviews. No source mutation was used.

## Implemented and verified

Shared serial native entry, strict early bootstrap and trusted compile-time
fingerprint handoff; actual primary Hello before Begin; package/cap admission;
native credential/context/status/CompleteAuthToken/negotiation handling; fixed
UTF-16/CBT and provider-output ownership; wipe/free-before-publication; normal
Finish/EOF/error disposal; explicit parent containment and held completion.

Darwin all-feature tests, strict Darwin/MSVC/GNU Clippy, Cargo/include-file fmt,
pinned schema drift, standalone archive and disabled-platform source-scope checks
pass. Actual Windows11 build26200/Rust1.95.0 MSVC Supervisor/native/default suites
pass. Suspended children distinguish Job termination from normal EOF; fixture-owned
no-kill control stays alive until guarded explicit termination. Forced helper
failures confirm actual helper/worker exit and scratch removal. Real initial
Negotiate returns NTLM with query_status0, allocation returned and successful
release. Normal native wipe audits inspect live initialized bytes before release.

The recipe restoration harness passes absent/prior values × success/injected
failure, with actual unique-root disposal after synthetic evidence retention.
Its initial long encoded invocation failed parsing and is retained separately;
file invocation of the same harness passed. Final owned-path process census: 0,
E: ReFS. Counts are descriptions, never acceptance thresholds.

Raw exact commands, hashes, statuses and failed attempts are in the private
[corrected campaign](../gwz-core-evidence/campaigns/transport-qualification/runs/2026-10-03-sspi-native-remediation-1/README.md)
(private access required). Archive/tracked verifier and exact run manifest pass.
Original resumed-child receipts remain unchanged and prove exit with EOF as a
competing cause; they do not supply the corrected containment claim. Public
fixtures and package/CI are self-contained without private evidence access.

## Remaining work

Digest is unavailable: the current request lacks native HTTP H(Entity). A bounded
reviewed contract amendment and provider parity are required; no empty-body/hash
assumption was added. This acceptance does not close all plan step 3.

Next composition chunk is plan step 4: trusted installed fingerprint producer,
CLI self-exec/Python bundled worker and core identity/CBT/deadline/route adapters,
with dual secret-adapter review and installed Surface check. Completed remote
Negotiate/NTLM/Kerberos, TLS/EPA, blocked real providers, descendants, trust/proxy/
Pageant parity and aggregate installed Windows qualification remain separately
open. Forced exit is containment, not physical wiping or external-provider abort.
Remote CI is unexecuted. No push, tag, publishing, release or endpoint activation.
