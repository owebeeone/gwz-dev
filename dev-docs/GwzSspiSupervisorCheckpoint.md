# SSPI supervision kernel implementation checkpoint

2026-10-03. **Accepted implementation checkpoint** under accepted GwzSspiDesign.md
revision 2 plus §8 and GwzSspiPlan.md step 2. Operator authorized implementation
after the accepted secret-codec gate. This applies the approved mechanism, not a
new process/protocol design. [Acceptance](GwzSspiSupervisorAcceptance.md) records
Code/State/Surface GO on the exact corrected tuple. Mandatory dual Code/State gate; additional Surface
check for the newly implemented caller lifecycle. No native SSPI or product
activation is implied.

## Cohesive scope

Implement the parent Supervisor/Conversation lifecycle, owned support types and
async control, private message phase/round/terminal admission, charged launch and
reader/writer ownership, bounded reaping, shutdown/quarantine/tombstones and
Windows creation-time Job/handle/pipes adapter. No new native credential provider,
general worker abstraction, runtime dependency, HTTP/Git/core/Python integration,
schema amendment or worker authentication stub. The production worker may still
refuse until step 3; dedicated synthetic worker fixtures cannot imply SSPI support.
The library remains independently buildable and publication disabled.

Default capacity eight, range 1–64, immutable caller-supplied monotonic deadline,
no per-round extension, eight-round bound, one request/response at a time. Before
registration a dropped waiter removes its bookkeeping; afterward retained
supervision owns every resource. Install a charged record before an OS side
effect. Terminal publication arbitration is atomic and first-wins; late output
does not revive cancellation/expiry. Capture/check actual primary identity and
refuse impersonation/change before launch. Verify Hello fingerprints/identity
before Begin or secret delivery. Finish is legal before Begin, during negotiation
and after Complete. Tokens and native completion are not HTTP/Git success.

No native/blocking call, thread join, wake or host callback under the short state
lock. Payload locks guard only bytes; transitions use the state lock with an
explicit lock order. Polling does not perform native authentication/metadata, worker/thread creation,
pipe I/O, process/Job waits or joins. Pre-registration originating-thread snapshot
handle destruction is synchronous on refusal or Start Drop, outside state locks;
that narrow resource-disposal operation has no hard OS-time bound. Long-running
worker/IPC ownership remains charged and offloaded.
Launch/I/O worker storage remains owned until completion even after cancellation.
Bounded dedicated supervision outlives dropped futures/runtime/Supervisor.
Quarantined records retain permits; never admit replacement capacity against them.
Reaped requires held-handle process exit, zero Job active processes and finished
launch/read/write ownership, not kill request, EOF, PID disappearance or Finished
alone. Tombstones: 256-entry FIFO, Unknown after eviction, checked context-owned
monotonic IDs. Shutdown closes admission and uses an explicit separate deadline.

Caller-thread provenance is captured at start invocation, not inferred from an
executor thread that later polls a moved future. Windows start uses a synchronous
originating-thread metadata snapshot/refusal and held-thread-handle capture;
document those probes as synchronous, without a nonblocking/OS-time guarantee.
The charged launch owner rechecks that held originating thread and primary
identity before process creation. Future::poll performs no native metadata query. Captured-handle destruction on
pre-registration refusal is the explicit synchronous disposal exception above.
Ordinary metadata capture is distinct from worker launch side effects; launch
still requires the installed charged record first. This is an implementation
choice for the accepted caller-identity rule and must be inspected by reviewers.

Windows adapter uses unnamed KILL_ON_JOB_CLOSE, no breakaway, creation-time
STARTUPINFOEX JOB_LIST plus explicit HANDLE_LIST, suspended child already in Job,
membership check before resume, no Create/Assign fallback. Attribute memory and
its referenced Job/handle arrays live through destruction after CreateProcess.
Only two child pipe ends and NUL stderr inherit. Parent closes its copies of
child ends promptly. Exact trusted absolute executable, nonsecret private
bootstrap arguments, minimal OS-derived Windows runtime roots, no shell/PATH
search/caller environment snapshot. Late CreateProcess returns after cancellation
must contain and reap child without resume/secrets. Failed creation still retains
charged ownership until the launch thread finishes. Pipe buffers/handles cannot
be destroyed or recycled while a blocking call still uses them. Advisory I/O
cancellation and successful termination requests are not completion evidence.

## Dependency approval before addition

Owner explicitly inspected local released windows-sys 0.61.2 manifest, no_std
entry, generated Job/Threading/Pipe/Security declarations and constants, and
windows-link 0.2.1 manifest/link macro source before addition. Both MIT OR
Apache-2.0, MSRV 1.71, no build scripts. windows-sys has only windows-link with
defaults disabled; link is no_std, no transitive dependency or runtime callback.
Approve target-Windows windows-sys = '=0.61.2', defaults false, with only
Win32_Foundation, Win32_Security, Win32_Storage_FileSystem, Win32_System_IO,
Win32_System_JobObjects, Win32_System_Pipes, Win32_System_Threading,
Win32_System_SystemInformation and Win32_System_SystemServices as needed here.
No authentication-provider bindings/features or further crate is approved.
Zeroize =1.9.0 remains unchanged. Confine our unsafe code to enclosing audited
Windows modules; foreign generated dependency scopes are not our source migration.

Primary API checks: Microsoft documents the creation-time
[Job/handle attributes and referenced-storage lifetime](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-updateprocthreadattribute),
[advisory synchronous-I/O cancellation](https://learn.microsoft.com/en-us/windows/win32/api/ioapiset/nf-ioapiset-cancelsynchronousio)
and [ReadFile buffer lifetime](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile).
These support ownership obligations; they do not prove native cleanup here.

## Tests, evidence and review

TDD against fake OS/IPC/clock outcomes before production paths. Deterministic
schedule exploration plus seeded random traces around admission/registration,
launch return/resume, Hello validation, Begin/Challenge/Token/Finish/Finished,
cancellation/expiry, Drop, shutdown, malformed/partial frames and reaping.
Test late successful launch/output, saturated/quarantined capacity, failed kill/
wait/Job observations, reader/writer/launch thread panics or failure, slot release
only after all proofs, ID overflow and tombstone eviction/cross-context lookup.
No sleeps/processes/services in fast tests. Synthetic success exists only at
injected fake ports, never as a production/native claim. Retain seed/input/trace.

Standalone tests, strict Clippy/fmt, pinned schema drift, syntax-aware disabled
cfg scope check, archive validation and Windows-target check are local gates.
Cross-compilation is not native execution. Windows process fixtures are a separate
opt-in tier; native SSPI/provider qualification and installed-host composition
remain later. Build/cache targets outside repos; campaign raw evidence private,
public CI fixtures/runners public. Follow evidence policy before any new campaign.
Settle exact root/member/core tuple via GWZ before Code/State and cold Surface.
One drafter, peer-blind initial reviewers, same reviewers for bounded correction;
two-remediation cap. No push, release, registry, host or endpoint activation.

## Implementation submitted for settled review

Parent lifecycle now lives in `gwz-sspi/src/supervisor/`, with a pure phase kernel,
context-owned records, separate deadline driver and charged launch dispatcher,
per-record launch/reaper and reader/writer owners. The dispatcher performs thread
creation and finished launch joins; caller polling and deadline control do neither.
The reaper retires pending payloads and held child handles before actual launch
completion permits record removal. Windows FFI is confined to enclosing platform
modules. A private per-context test clock permits deterministic arbitration tests;
production samples monotonic time under its short state guard.

Owner local gates on Darwin: all-feature Rust tests (50 unit, three integrations,
two compiled examples and sixteen negative trait doctests), strict all-target and
all-feature Clippy, Cargo formatting and all-source rustfmt including include! and
disabled Windows files, 16-artifact schema drift check and 13 schema tests pass.
Windows MSVC and GNU all-target/all-feature strict Clippy pass by cross-compilation.
Standalone crate packaging/verification and extracted-archive all-feature tests
pass. These observed numbers describe the run, not an expected-count release gate.
No native Windows process execution, native authentication, remote CI or installed
host composition is claimed. The schema, semantics and fingerprints are unchanged.

Implemented caller documentation is `gwz-sspi/docs/Supervision.md`, included in
Rustdoc with a compiled async walkthrough. The shared caller guide now labels the
parent implementation and its pending acceptance separately from future native
provider/worker entry. Existing pure secret ownership guarantees remain in force.

## Settled-review remediation

Initial Code review found one P2; State found two distinct P2s. Surface was GO
with one P3. Code/State independently converged on missing production ownership
bridge coverage (P3), while Code also found a too-broad polling description (P3).
[One merged remediation plan](GwzSspiSupervisor-RemPlan.md) maps every finding to
its correction and closure. Remediation round 1 is in progress; acceptance remains
pending. No protocol redesign or native provider activation is proposed.

Remediation candidate member `a75485cbdd03607909d11637c07f97548dd7902c`
passed owner full Rust tests (60 unit, three integrations, three compiled examples,
sixteen compile-fail doctests), strict Darwin/MSVC/GNU Clippy, both format checks,
61-file disabled-branch scan, unchanged 16-artifact schema check, standalone
96-file archive verification and extracted-archive full tests. Drafter also ran
13 Python tests. Three original blocking counterexamples were RED before correction
and GREEN afterward. New production iteration/fake-owner coverage supplements
the retained pure kernel schedules. Await original reviewers; no native claim.

All original reviewers independently verified closure and returned GO on the
corrected tuple; see GwzSspiSupervisorAcceptance.md. No native qualification or
activation is accepted. Earlier implementation/review paragraphs are historical.
