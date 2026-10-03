# GWZ SSPI process boundary

Date: 2026-10-03. Revision 2 (merged remediation 1). **ACCEPTED — bounded design/API only**, at root
`f2029e4b1739c0214138675dfb16abdb44f6a0d7`, core
`d78a664e3c5a325c6f12be409eb7645c1c1b51d0`, evidence
`1beb1d204c824701ddbd033c7f89df9a3561f5e5`, after
[Consistency](GwzSspiDesign-ReviewConsistency-1.md),
[Safety](GwzSspiDesign-ReviewSafety-1.md) and
[Surface](GwzSspiDesign-ReviewSurface-1.md) GO.
[Acceptance](GwzSspiAcceptance.md) records scope and next work. Product remains
unimplemented and full Windows separately NO-GO.

## 1. Decision and authority

The operator selected a Windows-specific SSPI library named `gwz-sspi`. A fresh
process owns each native authentication conversation. This replaces the proposed
in-process native-call worker in Windows parity §8. It is not a generic worker
framework, a new network transport, or a change to the CLI/core application API.
The CLI may dispatch the worker inside its own executable; Python distributes a
standalone worker executable alongside its extension. Both use the same library.

This is a bounded mechanism review. GO permits standalone implementation against
this contract. It does not accept the entire Windows parity draft, activate its
endpoint sources, remove its guards or qualify a release. Digest, real blocked
providers, interactive identity, EPA, TLS binding lifetime, trust, proxy, Pageant
and the remaining baseline/primitive rows retain their existing gates.

Controlling graph: [Windows parity](../gwz-core/dev-docs/GwzTransportWindowsParityDesign.md)
§§2,8–11 as amended below; [helper design](../gwz-core/dev-docs/GwzTransportCredentialHelpersDesign.md),
[helper timing](../gwz-core/dev-docs/GwzTransportCredentialHelperTimingAmendment.md),
[SSH helper clock](../gwz-core/dev-docs/GwzTransportSshHelperClockAmendment.md),
[helper configuration view](../gwz-core/dev-docs/GwzTransportCredentialHelperConfigurationViewAmendment.md),
[crate map](GwzCoreSessionCrateMap.md) §1 and [library boundaries](GwzLocalCloneLibraryBoundaries.md).
The imported Windows draft's old source tuples describe its experiments, not
MAIN's accepted helper implementation. No imported Windows product code is in scope.

## 2. Boundary and dependency direction

`gwz-sspi` is a separate member repository after acceptance and a supplied remote
exists. It builds/tests without core, git2, CLI, Python or gwz-transport. It owns
native SSPI, IPC, process admission and supervision; core owns HTTP, TLS, origins,
credential selection, helper invocation, routes, retry/effect decisions and the
network operation's deadline. A thin core adapter composes these domains. No core
callback, full environment snapshot, Git type or GWZ message crosses the API.

This is a scoped exception to the map's in-core crate placement and secret rule:
owned identity/password, input/output authentication tokens and channel-binding
bytes necessarily enter this library. No helper lookup/spawn policy enters it.
Its private IPC uses a separate **taut-authored schema**, exported/generated in
this repository. It is not a shadow encoding of GWZ calls, nor part of the public
CLI/core envelope. GWZ message encoding remains in core. No dependency runs from
this library back to core. No mutable global or thread-local is introduced.

The library provides parent APIs plus `worker_entry(bootstrap)` for a host's early
internal dispatch, and a minimal executable calling that entry for Python. The
worker path is an absolute, host-supplied trusted installed executable; no PATH,
cwd, repository setting or environment variable selects it. CLI dispatch happens
before ordinary argument parsing, Git init, logging and application runtime.
Python locates its bundled executable relative to the installed package, without
importing a child interpreter. Missing/mismatched workers fail explicitly.

## 3. Identity and native conversation

One process, one context, one conversation, at most eight challenge rounds. No
worker pooling or native context reuse. Acquire/Initialize/Complete and disposal
are serial on the worker's native thread. Negotiate/NTLM keep Windows §8's target,
flags and opaque token handling; WDigest retains its method/URI contract, but
support is gated on its unresolved native parity row. Basic is outside this API.
No independent Rust NTLM/Kerberos/Digest implementation is substituted.
Negotiate explicitly permits either Windows Kerberos or NTLM; callers requiring
Kerberos-only must not start it. This version adds no per-mechanism policy knob.
Each Token reports MechanismObservation: Unresolved or Selected { mechanism:
Kerberos/Ntlm/Digest, authoritative: bool }. For Negotiate the worker queries
SECPKG_ATTR_NEGOTIATION_INFO after native processing; IN_PROGRESS/OPTIMISTIC or
unavailable intermediate queries are not authoritative. COMPLETE with a known
package is authoritative. Unknown/unavailable selection at Token Complete refuses
before publication. Direct Ntlm/Digest select their known requested provider.
Intermediate unresolved tokens are permitted only under the caller's explicit
acceptance of either Negotiate mechanism, never as evidence of Kerberos-only use.
Query output ownership is freed normally; no provider strings escape the worker.
See [Microsoft's negotiation-info contract](https://learn.microsoft.com/en-us/windows/win32/api/sspi/ns-sspi-secpkgcontext_negotiationinfoa).
Production native tests must verify this observation for each supported provider.

`Identity::CurrentLogon` supplies NULL authentication data. The supervisor captures
its actual primary token SID, authentication LUID and session ID; the child returns
its actual corresponding values in Hello, verified before any credentials or
challenge are sent. Caller-thread impersonation is refused at supervisor creation
and before each launch. Changed primary identity is refused. This bounds the
contract to the same primary caller logon; it does not promise impersonated,
alternate-token, service-to-desktop or interactive-user discovery. Those require
separate evidence/design. Child token equality does not authorize route reuse.

`Identity::Explicit` owns Unicode user/domain/password in non-Clone zeroizing
storage; DOMAIN\user splits once, UPN keeps empty domain. It lives through all
possible native use. Target host is core's canonical U, never reverse DNS.
Core captures binding from the actual verified final origin TLS connection using
the existing native-tls API. The owned binding travels with this conversation;
no host cache, empty fallback or old connection's bytes. Core closes this
conversation on connection loss, redirect, binding/peer change or route retirement.
It cannot resume a conversation on another socket. Tokens stay opaque to core
except existing HTTP bounds/encoding and auth status handling.

Normal completion copies provider output into owned secret storage, wipes/releases
provider buffers, and calls DeleteSecurityContext and FreeCredentialsHandle once
for each initialized handle. Forced termination bypasses these destructors: it
reclaims worker address-space ownership, **not proof of physical erasure or
cancellation inside LSASS, a provider, a remote KDC or Pageant**. The worker's
normal paths wipe all temporary encode/decode/native buffers, including errors.
No credential, identity text, token, CBT, raw provider error string or IPC body is
logged. Numeric fixed error classes/status codes are sufficient diagnostics.

## 4. Creation containment and IPC ownership

Minimum mechanism platform is Windows 10/Server 2016 with creation-time Job-list
support. Create an unnamed Job with KILL_ON_JOB_CLOSE, no breakaway, held only by
parent supervision. CreateProcessW uses STARTUPINFOEX with both JOB_LIST and an
explicit HANDLE_LIST. The child is created suspended **already in that Job**;
there is no Create→Assign gap or fallback to subsequent assignment. Creation,
attribute, inheritance or containment-check failure refuses and cleans up; never
resume or deliver secrets to an uncontained process. Unsupported creation is an
explicit error, not an in-process fallback. Check membership before resume.

Only the child's two private anonymous pipe ends and a dedicated NUL error handle
are inherited. The Job, parent pipe ends, unrelated descriptors and console
handles are not. The exact absolute executable and nonsecret bootstrap handle
numbers are passed directly, with no shell. Use only minimal trusted Windows
runtime environment roots; never credentials or the captured application
environment. Close parent copies of the child's pipe ends immediately after
creation so the parent cannot keep EOF alive. Launch records and permits exist
before any OS side effect. Parent loss closes the last Job handle even between
CreateProcess return and ResumeThread; descendants cannot break away.

Anonymous Windows pipes do not support overlapped I/O. Use two dedicated blocking
IPC threads per live slot (one reader, one writer), with owned buffers and handles,
not an unbounded shared blocking pool. The parent control/deadline future never
waits synchronously for native calls or these threads. Killing the owned Job and
closing pipe endpoints triggers cleanup; resources/permits remain held until
threads actually finish. Blocked process creation likewise runs in a charged
launch thread. Its record owns the Job and attributes: if creation returns after
cancellation, the returned contained child is killed without resume or secrets.
A stalled kernel call is not declared interrupted; its slot is quarantined.

Each pipe has one writer. At most one request and response are pending per
conversation. Private framing is a little-endian u32 byte length followed by one
taut message. Reject zero, unknown type/state and lengths above 100,000 before
allocation; reject truncated EOF and trailing bytes in a message. Tokens are
bounded by min(provider maximum, 65,536 bytes, existing HTTP header limit). Text
and binding lengths are independently checked within the frame bound. No growing
queue or worker-emitted freeform text is allowed. A terminal state ignores late
frames; they cannot publish a token or revive a conversation.

Protocol v1 is closed: worker Hello (protocol version, build/schema fingerprint,
actual identity); parent Begin (selected package, target, identity, CBT and optional
Digest initial_challenge/method/URI); serialized Challenge; worker Token (Continue or Complete,
attributes, MechanismObservation and opaque output); parent Finish; worker Finished; worker Error
(fixed enum/status). Supervisor::start stops after verified Hello; the first step(None) sends Begin
and performs the first Initialize, with empty input when appropriate; for Digest Begin's initial_challenge supplies the entire validated
challenge at the first Initialize. All three Digest fields are required for
Digest and prohibited for Negotiate/Ntlm; Digest requires Explicit identity.
Initial challenge is nonempty owned zeroizing bytes bounded by the same token,
HTTP and frame limits. Missing/oversized/wrong-package fields refuse before native
work, with all owned inputs wiped. Subsequent Initialize runs only from Challenge. Round 8 continuing
refuses rather than issuing round 9. Hello mismatch refuses before Begin. Finish
is legal after verified Hello, before Begin, during negotiation or after Complete;
pre-Begin cleanup has no native credentials/context to dispose. It performs normal
cleanup and confirmed exit; cancel does not depend on receiving Finish or acknowledgement. Generated secret
messages must not derive Debug/Clone or retain ordinary String/Vec copies: codec
adapters and generated types must offer owned zeroizing decoding/encoding, or
implementation must stop for a reviewed schema/codec correction before secrets
enter production. Build/schema mismatch never negotiates a degraded mode.

## 5. Supervisor, cancellation and bounded cleanup

A host context owns one supervisor shared by its endpoint instances. Default
capacity is eight workers, configurable only through the library constructor to
1–64. This is separate from jobs, per-host socket caps and helper slots. No new
CLI setting. Every Launching, Authenticating, Closing, Reaping or Quarantined
record holds one permit; only Reaped releases it. Admission can await capacity
within the passed operation deadline/cancellation; never create a child first.

Public states: Waiting (no native record); Launching; Authenticating; Closing;
Reaping; Reaped; Quarantined (exit or local I/O not confirmed). A context-owned
record is installed before launch. Caller futures own handles to it, not its
only storage. Dropping start/step futures or Conversation is cancellation of the
whole conversation; before registration it removes the waiter, after registration
it closes result publication and transfers cleanup to the supervisor. No orphan
record, borrowed credential buffer or detached uncharged native call is allowed.

First terminal result wins under a short state lock. On cancellation/deadline,
first revoke result publication, then stop writes, terminate the Job, close local
IPC and reap independently. Concurrent token completion cannot override terminal
cancellation, even if a worker wrote before the deadline and the parent reads
later. No native, blocking I/O, join, core/Git/Python call runs under that lock.
Record fields use the supervisor state lock; IPC payload locks guard only buffers,
never phase transitions. Worker callbacks cannot reenter the supervisor.

Terminal operation errors return within the caller's observed control/deadline
scheduling bound; **no numerical real-time OS guarantee is claimed**. Cleanup may
continue after the caller returns. `finish` returns only after normal disposal,
held process handle exit, Job active-process count zero and both IPC threads plus
launch thread finished. Failure/cancel completion separately reports cleanup as
Confirmed or Pending with opaque record ID. It does not infer exit from a kill
request, IPC EOF, PID disappearance or a failed wait. Failed termination/wait,
stuck launch or I/O keeps a Quarantined permit and owned handles/storage. A bounded
reaper retries observation; no replacement worker is admitted against that slot.
If all slots are retained, subsequent calls wait/refuse by their own deadlines.
No secret/native-operation fallback or automatic retry exists in this library.
Cleanup-status tombstones form a 256-entry FIFO per supervisor; evicted IDs return
Unknown, never Confirmed. Monotonic context-owned IDs refuse on counter overflow.

`shutdown(deadline)` closes admission, cancels all records and waits for confirmed
cleanup until its independent shutdown deadline; returns outstanding opaque IDs
on expiry. A Pending report means the supervisor still owns resources. Drop is
nonblocking: it closes admission, revokes all result publication and leaves owned
supervision/reaping alive until confirmed exit; no borrowed runtime/caller object
is retained. Bounded dedicated supervision threads own Arc records, so async
runtime teardown cannot abandon them. Outstanding supervision may persist until
host process termination; last Job handle closure still contains children. This
is explicitly bounded retained ownership, not a claim every Windows call can be
cancelled or every shutdown empties its resources.

## 6. Core composition and clocks

The accepted [HTTPS composition amendment](GwzSspiHttpsCompositionDesign-DRAFT.md),
with exact review tuple in [acceptance](GwzSspiHttpsCompositionAcceptance.md),
clarifies this section's clock source and caller handoff. For native-capable
HTTPS, capture one positive effective setup aggregate as an absolute logical Open
deadline before first checkout/adoption. Preserve it across discovery, redirects,
helper work, carried/reused leases and all native rounds. Zero refuses native
selection before Begin or credential publication; an expired positive deadline
is Timeout. Allocation and cumulative active HTTP I/O allowances cannot replace
that deadline. This extends the positive setup clock's scope only, without
extending retry eligibility. An enclosing expiry during helper work cancels with
setup/authentication provenance and cannot cause default-logon fallback; M4/M10's
own captured allowances and helper failure provenance remain distinct.

The same amendment specifies Supervisor-bound owned CallerCapture and
start_captured, captured synchronously on the original host entry before fanout
or Python submission. Start owns a private origin reference without executor
recapture; launch rechecks identity and thread liveness. Native Facts preserve
actual mechanism authority independently of native Complete; authenticated
success additionally requires credential publication and valid remote acceptance.
These clarifications supersede ambiguous clock/capture wording below, without
changing the library's immutable deadline or retained ownership guarantees.

The core adapter passes an **absolute existing operation/setup deadline** and
cancellation observation. Launch, Hello, native rounds and IPC all consume that
same deadline; no fresh allowance per challenge, helper-local budget or 120-second
SSPI allowance. SSPI does not borrow SSH's LocalAdmission/LocalInteraction phase
or pause network clocks. Core owns its accepted setup/Control progression and
network idle semantics; the library receives a translated monotonic deadline
once per conversation. It is immutable after start; a caller chooses any shorter
value before start. If its enclosing deadline moves earlier while work is pending,
the caller signals the supplied Cancellation at that boundary. No worker budget
or parent-clock timestamp is sent over IPC: parent supervision alone enforces the
deadline, including during native calls. There is no way to extend it.

Accepted helper interaction/allocation M4/M10 and failure provenance remain
unchanged. Do not invent helper_budget_ms or a helper timeout cause for SSPI
capacity/launch/provider expiry. The adapter projects fixed SSPI failure to the
existing operation failure/timeout model with no helper provenance. Credential
helper execution completes in core before Explicit identity enters this API.
Native default identity remains separate. No global environment snapshot is
passed. Capture matching caller identity in host context remains core's contract.

On cancellation/failure core revokes the conversation's publication and closes
or retires its challenged exclusive HTTP connection; a pending worker cannot
publish credentials into a reused route. Auth rejection is terminal. Any eligible
pre-effect network retry is core's existing bounded retry, creating a fresh
conversation on a fresh lease from the same source. Partial/complete POST is not
replayed for credentials. Native worker success means a token is available,
not authenticated HTTP or Git success. Core alone decides remote success.

## 7. Evidence, acceptance and implementation gates

Existing [Rust alternative](../gwz-core/dev-docs/GwzTransportWindowsAuthAlternativeFeasibility.md)
does not preserve current-logon/Digest coverage. The
[native process feasibility](../gwz-core/dev-docs/GwzTransportWindowsSspiWorkerFeasibility.md)
proves context retention, EOF/length checks, controlled stalls, normal cleanup,
held-handle exits, nested Job use and parent death, not actual provider preemption.
[Creation containment](../gwz-core/dev-docs/GwzSspiCreationContainmentFeasibility.md)
adds creation-time attachment, earliest returned-handle parent death without
resume/assignment, resumed descendant containment and invalid-Job creation refusal.
These are native mechanism observations, not production implementation acceptance.

Implementation follows [the plan](GwzSspiPlan.md). Fast standalone tests use fake
OS/clock/IPC ports: deterministic schedule exploration and seeded random sequences
around every state/effect boundary, malformed/partial/max frames, cancellation
before/after registration, late completion, saturation/quarantine and shutdown.
Fake protocol tests cover exact first Digest input, wrong-package fields, pre-Begin
finish and typed mechanism publication; native tests reproduce creation/Job/IPC rows with owned handles, then real SSPI
context cleanup and secret-owner cleanup. Public CI fixtures are self-contained;
private raw evidence is optional. Identity/CBT fidelity, per-route rejection and
CLI/Python installed worker matching are required composition tests. Normal-path
secret wipe checks observe every owned encode/decode/native temporary; forced
exit is tested for containment only, never physical wipe.

Three design GO reports accept this mechanism/API object only. Implementation
then needs TDD, per-step reviews at secret/interface boundaries, and final dual
Code/State plus installed API Surface review. Windows product activation still
requires Windows parity §11's complete native and compatibility gates. Any
mechanism deviation affecting identity, containment, secret handling or lifetime
returns to design review. No product code, repository creation, push or tag is
part of this draft package.

## 8. Accepted bounded token-limit amendment (message remediation 1)

Accepted after Consistency/Safety and revised caller-guide Surface GO at
[the exact message tuple](GwzSspiMessagesAcceptance.md). This explicitly
supplements §4's HTTP token bound and §6's adapter inputs: AuthRequest owns a
required token_limit: TokenLimit, constructed from raw bytes 1–65,536 with no
default. Core derives it from its existing HTTP header allowance after
scheme/base64 overhead and the parent copies it exactly into Begin.token_limit.
Parent and worker enforce it for initial/subsequent input and output, with native
provider maximum narrowing it before credential/context initialization. No HTTP
policy dependency, new setting, deadline change or retry is added. The updated
caller guide defines the checked constructor, units and error boundaries.
The revision-2 API's omission of a cap carrier is superseded only by this scoped
amendment; previous Surface GO does not cover the added field/constructor.
