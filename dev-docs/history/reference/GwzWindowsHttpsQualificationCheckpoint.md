# Windows HTTPS qualification checkpoint

2026-10-04. **Worker/provider preparation accepted. Full Windows
HTTPS qualification and release remain NO-GO.** The operator authorized focused
qualification after accepting HTTPS composition. This checkpoint accepts no
activation, push, tag, registry publication or broader Windows parity disposition.

[Code](GwzWindowsHttpsQualification-ReviewCode.md) and
[State](GwzWindowsHttpsQualification-ReviewState.md) returned GO at root
`c1db8d490bbce380c726ac4793493aec87053a00`, SSPI
`582ec001bd2972076ea65a7db87d81d988c6f2e7`, evidence
`1930542b7264bcbc5d9b10c67887c0f350798cb1`, and unchanged core. Code's one P3
inventory finding is independently closed by the original
[reviewer](GwzWindowsHttpsQualification-ReviewCode-1.md) at root
`71d7621c42b498a761e8be6a806899e72d964ba4`, evidence
`d505cddaca791ed6cadb11f9fb5ab4fd88e0fa51`, with SSPI/core unchanged. State GO is
retained on the unchanged public fixtures. Zero open findings; no new architecture.
One initial dual review and one bounded evidence-only nonblocking correction.
This accepts worker/provider preparation and its corrected owned-path inventory
only. Eight native tests are executed; integrated HTTPS remains unexecuted.

## Exact scope

Production sources remain at the composition acceptance tuple: core
`28f564a674574eaefa43266d3137be9a4ddc38b8`, SSPI
`c88fa0e174b185a43e0d0d0c91660cb0957e380a`, transport
`8a2ec7fc2e6d6a0da4c5519d72eac90c9ca4c578`, CLI
`6a9c0dac8aebe7b9d20ecfa20a8c402ec4bf4311`, Python
`e0c5af10b33289a455f662680af8ac12fd24f9d3`. The new scope is two SSPI public test
files (`tests/native_worker.rs`, `tests/native/completion.rs`), this checkpoint
and private campaign evidence. No dependency, public API, wire or production
owner changes. The settled review tuple is recorded in the review prompts.

Control: [SSPI design](GwzSspiDesign.md),
[composition design §9](GwzSspiHttpsCompositionDesign-DRAFT.md),
[implementation acceptance](GwzSspiHttpsCompositionImplementationAcceptance.md),
and SSPI `docs/NativeFixtures.md`. Native secret/FFI fixtures receive independent
Code and State review; no new caller surface requires Surface review.

## Executed preparation

Native Windows 11 build 26200, Rust 1.95.0 MSVC, operator-selected E: ReFS.
Fresh runtime `E:/gwz-tests/https-sspi-qual-20261004-a`; source/target/scratch
remain outside repositories. SSH forwarding disabled. No OS trust, account,
service, proxy, zone or authentication-policy changes. No supplied operator
password or token/identity output. The current-logon exchange stays local.

The public opt-in test executes the existing production worker and Supervisor
against a native `AcceptSecurityContext` verifier. Both sides use independently
constructed synthetic binding bytes; the verifier does not allow missing
bindings. Fixed initialized wiping storage owns input/output/binding bytes.
Native verifier context/credential cleanup statuses and token-handle closure
are checked. Every positive worker exchange separately requires native Complete,
authoritative NTLM selection, verifier success and confirmed Supervisor cleanup.

Final native-worker target: **8/8 passed**, comprising six new tests and two
existing first-leg/pre-Begin tests:

- NTLM completed by worker and native verifier; accepted context yields a token.
- Negotiate completed by worker and verifier, selecting NTLM. No Kerberos claim.
- A different verifier binding produces native `SEC_E_BAD_BINDINGS`.
- Captured live original thread works across executor handoff; retired thread
  refuses with IdentityMismatch.
- Self-impersonation on a fresh originating thread refuses both new capture and
  launch using its earlier capture. RevertToSelf and a subsequent actual capture
  establish restoration before thread exit; no other thread/account is changed.
- Cancellation and original fixed deadline expiry dispose an idle worker after
  its initial token. Actual Supervisor cleanup is Confirmed, shutdown has no
  outstanding records. This covers idle native IPC, not a blocked provider call.

Corrected owned-path process inventory: **zero**, ReFS verified, after an actual
live-process negative control. The original inventory-final-v3 zero attribution
is withdrawn: Code P3-1 found an unnormalized slash comparison could miss live
owned Windows paths. Corrected inventory normalizes both paths and includes a
directory boundary. The live control proves original miss, corrected detection,
actual gate refusal (exit1), sibling exclusion, held control exit and subsequent
zero gate (exit0). Original raw receipts are retained; reviewer closure is filed.
Local ordinary SSPI
all-feature tests/doctests, Windows-target strict Clippy, explicit fixture rustfmt
and diff checks pass. Windows target Clippy is source evidence only. The shared
cfg guard passes its existing core/CLI/Python scope; it does not scan SSPI.
The SSPI fixture has an enclosing Windows module and was compiled/inspected.

Private [raw campaign](../gwz-core-evidence/campaigns/https-integration/runs/2026-10-04-windows-sspi-qualification/README.md)
requires private access; public fixtures/builds do not. It retains original and
corrected fixture bytes, exact input/runner hashes, source readback, all statuses
and failed attempts. Initial completion-v1 was 1 PASS/2 FAIL because the fixture
incorrectly assumed two Negotiate rounds and empty output on verifier failure.
The corrected fixture permits the native rounds/error token, without changing
production code. Completion-v2 passed all three; final-v3 passed all eight.
These are fixture corrections, not product defect repairs or concealed retries.

## Qualification prerequisite found

The previous sequencing understated remaining implementation: the accepted
composition exists behind `all(unix, gwz_transport_candidate)`, so a Windows
build does not compile/select the endpoint or either installed caller route.
Core `src/lib.rs`, `src/git/mod.rs`, CLI `src/globalargs/dispatch.rs` and Python
`native/src/client_host.rs` establish that gate.

Removing the guard merely to test would also expose real portability work:
core `src/git/endpoint/https_auth/runner.rs` unconditionally uses Unix OsStr byte
APIs and Unix null/current-directory assumptions; `file_worker.rs` uses Unix
OpenOptionsExt/O_NONBLOCK. Windows helper process containment and lossless path/
environment handling must preserve accepted ownership and policy. Replacing
those components with success-shaped stubs would not qualify the integration.

Next is a bounded Windows HTTPS qualification-entry/portability package under
composition §9: inventory the necessary platform seams, expose the minimum
private/test-only composition entry, port those seams with real implementations
and characterization, and review the settled boundary. Keep normal Windows
endpoint selection disabled. This is a platform prerequisite, not evidence of
a new virtual-stream protocol defect. Do not present these local provider tests
as completion of that package.

After it exists, run verified native TLS→core HTTP→SSPI worker→native verifier,
including actual leaf-derived CBT/EPA-required positive and negative cases;
helper/default identity and origin handoff; lease replacement; fixed Open clocks,
cancel/cleanup retention; real Git fetch/push/clone and installed CLI/Python
hosts. A provider-accepted disposable explicit identity and server fixtures are
still prerequisites for explicit-helper authentication. No account/trust mutation
is inferred from this qualification authorization; any necessary change needs
a concrete scoped disposition. Prefer connector-local fixture roots.

Digest/H(Entity), Kerberos, differing-account/primary changes, real provider
blocking, broader Windows SSH/Pageant/proxy parity, installed provenance,
performance/platform/source batches and strict-core Clippy debt remain open.
Qualification does not waive the separate reviewed activation or release gate.
