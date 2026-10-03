# GWZ SSPI native worker checkpoint

## Current object: remediation 1 pending re-verdict

2026-10-03. Initial Code/Surface GO and State NO-GO are preserved verbatim.
[One merged remediation](GwzSspiNative-RemPlan.md) corrected all six finding
records in member `425e13dc011c42e94fdea31779a8e5967aedc82b`.
[Drafter report](GwzSspiNative-DrafterReport-1.md) records portable/cross/package
results and the two guard RED/GREEN regressions. Public API, dependencies, wire
and authentication policy are unchanged; Digest remains unavailable.

Owner disabled-branch scope check passed. Corrected committed source passed
actual Windows11/MSVC production Supervisor/native/default suites. Suspended
children distinguish Job termination from EOF; the no-kill control remained
alive until explicitly terminated, with held-handle cleanup. Actual parent death
and forced after-spawn/PID/observer failures confirmed helper/worker exit and
scratch disposal. Real Negotiate query returned status0, NTLM selection and a
returned allocation with checked successful release. These are local initial
tokens, not completed remote authentication. The direct helper no-op supplies no
separate containment proof.

The documented recipe's unchanged ownership/restoration logic passed all four
absent/prior-value × success/injected-failure cases with actual owned-root removal
after synthetic receipt retention. Its first long encoded collector invocation
failed at PowerShell parsing; exact failed evidence is preserved. File invocation
of the same harness passed. Final owned-path inventory: 0, E: ReFS, build26200.

Private [corrected campaign](../gwz-core-evidence/campaigns/transport-qualification/runs/2026-10-03-sspi-native-remediation-1/README.md)
(private access required) is committed at evidence
`9adf06beab7a15d1e5c22ec1cecf8e35966e1d7c`; archive/tracked checks and exact manifest
hashes pass. Original raw receipts remain immutable. Earlier Job-drop/parent-loss
receipts observed exit with EOF as a competing cause; the stronger claim belongs
only to the corrected suspended-child run.

Next: same Code/State/Surface reviewers verify their original counterexamples and
classify the changed range at the corrected settled tuple. The owner does not
self-close findings. No activation/push/tag/release; open Digest, installed host
composition and full Windows qualification gates remain. Sections below describe
the original implementation checkpoint.

2026-10-03. **Implementation in progress; not accepted.** Operator authorized
plan step 3 after parent supervision acceptance. Parent baseline is root
e220886f0641e4d3e5e67443171808666e191ff7 and gwz-sspi
aec1b9c65b75ad53dd3ae780fc1e04af18de795c; reference core remains
8cb3a3f01d79699a5ad07b6ec7cfc78321224d31.

## Scope and stop

Implement the serial native SSPI conversation and shared early worker entry,
using the accepted private taut schema, fixed zeroizing storage, actual primary
identity Hello and the existing private inherited-pipe bootstrap. Cover default
and explicit Unicode identity, provider cap admission before credential work,
context/token status and mechanism observations, CBT and Digest inputs, normal
credential/context/buffer disposal, EOF and malformed/partial IPC. Digest product
activation remains gated on provider parity. No worker pool, general framework,
new wire fields, host integration, release activation, push or publication.

Recorded gate: mandatory peer-blind Code/State native secret ownership/disposal
review, plus cold Surface review of the worker-entry caller documentation.
Settle the exact tuple after focused gates, then review. Follow review-loop
bounded merged remediation and keep reviewer reports verbatim in dev-docs.

Fast tests use the production worker/native ownership bridge with private fake
ports, deterministic faults and seeded schedules; no processes/threads/sleeps.
Native Windows fixtures are public and opt-in. Native execution and provider
parity must be distinguished from cross-compilation. Raw campaigns and logs
belong in the private evidence member; build/runtime caches remain outside it.
Tests use normal runner exit status, without test-count gates.

## Native execution preparation

Read-only inspection confirmed gianni@dabeest is reachable through OpenSSH,
Windows 11 build 26200, with the pinned Rust 1.95.0 MSVC toolchain installed.
The prior containment campaign records the operator's explicit E: ReFS fixture
choice, overriding the generic D: archive guidance. Use a fresh E:/gwz-tests
directory, never resume or overwrite historical runs. SSH disables local
forwardings and agent forwarding. Runtime/source copies, binaries and Cargo
targets stay outside the evidence archive. No account, trust, proxy, service or
machine authentication policy change is part of these fixtures.

Production runtime rows to distinguish in the report: verified primary Hello
and finish-before-Begin; native NTLM first token for current-logon and synthetic
explicit credentials followed by normal Finish; provider rejection/EOF cleanup;
deadline and owned Job exit/pipe-thread reaping; held-handle parent loss where
the public fixture can observe it. Synthetic bindings exercise SSPI layout only,
not TLS binding provenance or EPA parity. An outbound first token cannot prove
successful remote authentication. Blocked real-provider cancellation, Kerberos,
Digest and full TLS/HTTP parity remain qualification rows unless actually proved.

## Discovered Digest contract gap (open)

Independent implementer and owner inspection of Microsoft's
[Digest challenge input buffers](https://learn.microsoft.com/en-us/windows/win32/secauthn/input-buffers-for-the-digest-challenge-response)
found that HTTP WDigest requires an H(Entity) input in addition to challenge and
method. The accepted request/schema carries challenge, method, URI, target and
CBT, but cannot convey that entity-body hash. URI is the native target argument,
not a substitute. No empty-body assumption, qop parser, independent hash or
fabricated empty input is authorized. Native Digest therefore retains explicit
ProviderRejected before credential/context acquisition; test that no such native
calls occur. Profile validation and secret disposal still apply.

The current native review can accept Negotiate/NTLM and shared worker ownership
only. It cannot close all of step 3 or the Digest row. A bounded reviewed Digest
contract amendment and parity evidence are needed subsequently; this checkpoint
does not silently amend the wire or claim the original design is sufficient.

## Bootstrap metadata handoff

The shared worker entry receives the trusted host's 32-byte build fingerprint.
The minimal executable reads only a strict 64-hex compile-time packaging field;
missing or malformed metadata refuses before pipe ownership/native work/Hello.
There is no runtime environment selection, runtime executable hash, default
fingerprint or mismatch fallback. This implements the existing trusted metadata
handoff, not a wire amendment. Step 4 still supplies the actual trusted packaging
producer and matching installed host metadata. Native fixtures use an explicitly
synthetic build value and matching expected bytes; that is not installed package
provenance. Caller documentation must explain this construction and refusal.

## Dependency feature approval

The owner reviewed the released windows-sys 0.61.2 generated Identity, Credentials
and Rpc bindings and Cargo feature graph before this addition. Approve only
Win32_Security_Authentication_Identity, Win32_Security_Credentials and
Win32_System_Rpc in addition to the prior allowlist: respectively the secur32
SSPI functions/structures, SecHandle, and SEC_WINNT_AUTH_IDENTITY_W/Unicode flag.
These features only expose generated bindings and parent modules; no new crate,
build script, runtime or transitive dependency is added. Keep zeroize 1.9.0 and
windows-link 0.2.1 unchanged. No RPC service operation is authorized.

Primary native contracts consulted: Microsoft
[AcquireCredentialsHandle](https://learn.microsoft.com/en-us/windows/win32/secauthn/acquirecredentialshandle--general),
[InitializeSecurityContext](https://learn.microsoft.com/en-us/windows/win32/secauthn/initializesecuritycontext--general),
[Digest initialization](https://learn.microsoft.com/en-us/windows/win32/secauthn/initializesecuritycontext--digest),
[CompleteAuthToken](https://learn.microsoft.com/en-us/windows/win32/api/sspi/nf-sspi-completeauthtoken),
[package metadata](https://learn.microsoft.com/en-us/windows/win32/api/sspi/nf-sspi-querysecuritypackageinfow),
[negotiation observation](https://learn.microsoft.com/en-us/windows/win32/api/sspi/ns-sspi-secpkgcontext_negotiationinfow)
and [channel bindings](https://learn.microsoft.com/en-us/windows/win32/api/sspi/ns-sspi-sec_channel_bindings).
Provider buffers, queried allocations and initialized handles require explicit
owners through real native completion; requesting termination is not disposal.

## Remaining gates

This checkpoint cannot accept the full Windows release. Host packaging/build
fingerprint production, core/CLI/Python adapters and complete Windows parity
qualification remain plan steps 4–5 after the native review stop.

## Settled implementation and owner gates

Native implementation candidate gwz-sspi
610964282663c3b7844c9620d063e40d9ee76258 is committed and clean. It is not yet
review-accepted. [Drafter report](GwzSspiNative-DrafterReport.md) is preserved
verbatim; its native-execution pending statement describes its handoff time.

Darwin full all-feature tests, strict Darwin/MSVC/GNU Clippy, Cargo/all-source
formatting, schema drift/Python tests and standalone archive verification/tests
passed. Owner's independent disabled-platform source-scope check passed.
No expected test-count, test-inventory or source-mutation gate was used.

Owner then built that committed snapshot on Windows11 build26200 with pinned
Rust1.95.0 MSVC, synthetic compile-time metadata and the same production worker.
The public Supervisor fixture passed actual primary Hello, pre-Begin Finish,
initial current-logon and explicit Unicode NTLM tokens, normal Finish and confirmed
reaping. Ignored native fixtures passed normal EOF before/after native init,
held-handle exit after last Job closure and actual parent death, plus live-before-
free provider-output/UTF16/CBT wipe audits and successful context/credential free
statuses. The helper test's direct no-op run is not separate containment evidence.
Windows ordinary all-feature tests also passed. Final owned-path process inventory
was empty and confirmed E: ReFS. Tokens stayed local and were not logged.

Raw commands, statuses, source-archive hashes and logs are in the private
[native campaign](../gwz-core-evidence/campaigns/transport-qualification/runs/2026-10-03-sspi-native-bec3ba15/README.md)
(private access required). Public fixtures and build/CI remain self-contained.
The archive verifier passed; no binaries, targets or runtime checkouts are archived.

Next: settle root/reference/evidence tuple, then peer-blind native Code/State and
docs-only WorkerEntry Surface review. All activation and open Digest limitations
above remain in force; these native results do not establish remote auth or a
worst-case OS cancellation bound.
