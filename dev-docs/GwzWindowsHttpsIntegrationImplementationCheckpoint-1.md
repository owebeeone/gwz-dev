# Windows HTTPS WH1 — remediation checkpoint 1

2026-10-04. **Implemented and tested; original reviewer closure pending.**
This corrects all blocking findings in the [merged plan](GwzWindowsHttpsIntegrationImplementation-RemPlan-1.md).
It accepts no release, ordinary Windows activation or broader WH3 matrix outcome.

Core revision `261eaca55dca4067548027e8976ff0249a34d2f3` corrects reviewed core
`398158b3272e6f3a69132f8375190945dd93192a`. CLI source stays
`6ab16d461acb6daf9fca8281eca6384971cc0c44`; Python source stays
`5df15766298fbbd97da1d6ecec74c6cc9dd69fda`. Both actual artifact producers
were rerun against the corrected core. SSPI, transport and Git dependencies
remain unchanged from the original implementation checkpoint. Private evidence is
committed at `053121cc97664e46539c07d77cdad4effb481955`.

| Original finding | Correction and executed regression |
|---|---|
| Code P2-1 | Retain native fixed D through completed-result collection and every pending Opened handoff; fresh clock, equality expired. Revoke authenticated route, cancel/discard retained work, preserve facts and disposal charges. Native advertisement serving begins only after successful mux publication; nonnative timing unchanged. Actual endpoint/mux late-collection and backpressure regressions fail before correction and pass after; pre-D success control, equality, single terminal, revoked route and retained real Connection/authority charge are covered. |
| Code P2-2 / State P2-2 | Direct legacy SSH-only and shared no-HTTPS constructors refuse typed UnsupportedOperation before qualification owners; actual offers require HTTPS. Native test wrongly succeeds before correction and refuses after. Existing native Bound/policy controls and Unix public constructor remain green. |
| State P2-1 | Install capacity on present HTTPS/SSH pools; paired Unix transaction and existing shared authority remain. First/later limit changes, conflicting overlap, dropped retirement and actual physical disposal retention pass. Native first-policy/retirement and CLI/Python one-connection cases fail before correction and pass after. |

These are implementation corrections to existing obligations, not new API,
wire, dependency, owner or platform promises. Cumulative WH1 is 52 source/test/
build files plus 3 inventories, 1,350 gross added/273 deleted lines, within
owner-disposed 55/2,600 ceilings. Final manifest SHA256:
`4917c1a8216e430b1b41b3dfda6bbf3109e36a2b4620750429a6df3e510e3d92`.
Native corrected refresh reads back 40 core inputs in candidate and copied
source, zero mismatches. Before-input snapshots and original REDs are retained.

## Verification

Portable normal runners pass: real endpoint/mux6, qualification4 (including
Unix constructor), HTTPS208, paired constructor/capacity16, cleanup2, source
boundary4, disabled-branch cfg guard, inventories, all49 changed Rust files'
formatting, whitespace. Strict Clippy remains RED45 with unchanged diagnostic
categories/multiplicities; existing generator owner-IR mismatch remains open.
No release waiver or broad disabled-platform migration is claimed.

Actual Windows11/MSVC1.95/E: ReFS normal qualification test runner passes5.
Before correction its existing2 controls pass but constructor and both capacity
regressions fail; compilation succeeds before these test failures. Final
provisioned CLI and provisioned wheel package/cleanup/install pass. Python uses
explicit CARGO_INCREMENTAL=0 as previously disclosed; unrestricted ReFS dev
incremental packaging is not qualified. Exact installed wheel/PYD/worker/CLI
hashes are recorded in the installed artifact receipt.

Actual corrected CLI clone/fetch/push with existing --max-per-host1 passes,
independently checking checkout bytes, fetched and pushed bare refs. Actual
installed Python Client(max_connections_per_host=1) clones/fetches/pushes,
streams ordered events, completes another call with unconsumed application
events, and closes with pending_local_work=0/peer_cleanup_confirmed=false.
Native fixture reports authoritative NTLM, all observed contexts disposed;
owned servers/processes reaped. These are normal-path consumer proofs, not all
scheduling/cancellation/identity adversities or peer cleanup confirmation.
Actual corrected CLI wrong CBT refuses; untrusted chain/hostname refuse before
native auth rounds. Positive client TLS uses captured extra CA plus normal
chain/hostname validation, no revocation override, and no Git in client PATH.
Server CGI/setup and independent ref verification use fixture-only Git.

Private evidence, access required: `gwz-core-evidence/campaigns/https-integration/
runs/2026-10-04-windows-https-portability/`. New immutable `raw/wh1-rem1-*`,
`portable-rem1/`, versioned remediation runners and before-source snapshots.
The initial harness type/identity failures remain labeled as harness failures;
genuine deadline RED controls and native REDs remain distinct. No compiled
outputs/venvs/keys/runtime repos in the archive; no public gate depends on it.

Next: same original Code/State reviewers close their own findings on one settled
corrected tuple. Scope remains limited WH1. Full integrated native adversity,
identity transitions, installed spaces/Unicode/provenance paths, WH2 helpers,
provider/parity decisions and release/platform/source/performance/package gates
remain open; ordinary Windows activation and full release remain NO-GO.
No push, tag, publication or OS policy mutation.
