# gwz-sspi caller guide

2026-10-03. **Accepted revision 2 baseline plus reviewed token-limit amendment.**
GwzSspiMessagesAcceptance.md records Consistency/Safety/Surface GO after remediation 1.
Package and worker are not released yet. Owned caller values and private codecs
passed their separate [Code/State/Surface gate](GwzSspiSecretCodecAcceptance.md); authentication
and process supervision remain unimplemented.
[Acceptance](GwzSspiAcceptance.md) records the exact reviewed tuple and Surface GO.
The historical DRAFT filename is retained until the implementation documentation lands.
Windows-specific native SSPI authentication, one contained process per conversation.
It produces authentication tokens; it does not make HTTP requests or decide whether
a server authenticated you. Core/CLI/Python application protocols do not change.

Install the matching `gwz-sspi` library and worker with your application. A CLI can
call `worker_entry(bootstrap)` from an internal self-exec mode before its normal
startup. A Python wheel bundles the dedicated worker executable alongside its
extension and uses its absolute installed path. No PATH lookup, shell or child
Python interpreter. Removing/upgrading the application removes/replaces its worker
together. Missing workers and protocol/build mismatches are errors, not fallbacks.

The following signatures specify the full public API. Only owned caller values
and TokenLimit are implemented; see [implemented caller values](../gwz-sspi/docs/CallerValues.md)
for their constructors, source ownership and validation boundaries. Supervisor,
Conversation, deadlines, cancellation and worker_entry remain future APIs.
All async methods return owned results. SecretBytes/SecretText have zeroizing
storage and no Debug/Clone. Conversation and Supervisor own resources; there are
no raw native handles or pointers in this caller API.

| API | Contract |
|---|---|
| `Supervisor::new(WorkerExecutable, Options) -> Result<Supervisor, Error>` | Absolute installed worker path; capture current primary Windows identity. Refuse impersonation or unsupported OS. Options defaults: `max_workers = 8`; accepted range 1–64. |
| `TokenLimit::new(raw_bytes: u32) -> Result<TokenLimit, Error>` | Required raw authentication-token byte cap, 1–65,536 inclusive. Invalid values return InvalidRequest before start/registration. No default; the host derives this from its existing HTTP limit after scheme/base64 overhead. Immutable nonsecret numeric value, no new CLI setting. |
| `Supervisor::start(AuthRequest, Deadline, Cancellation) -> Future<Result<Conversation, Failure>>` | Wait for capacity, launch and verify matching worker; no native authentication until `step(None)`. Deadline is the caller's absolute monotonic deadline. No default timeout. |
| `Conversation::step(Option<SecretBytes>) -> Future<Result<TokenStep, Failure>>` | First step is None and sends Begin; later steps supply challenge bytes. At most eight total steps. One step at a time through mutable ownership. Output status Continue or Complete, checked attributes, MechanismObservation and owned SecretBytes. Complete forbids further steps. |
| `Conversation::finish(self) -> Future<Result<(), Failure>>` | Legal immediately after start, during negotiation or after Complete. Normal native cleanup and confirmed process/Job/thread exit; pre-first-step finish initializes no native handles. Uses original deadline; drop of this future cancels and retains supervision. May return Pending cleanup in Failure if deadline expires. |
| `Conversation::cancel(self) -> CancellationReceipt` | Immediately revoke result publication and initiate termination. Receipt names opaque record ID with cleanup Pending or Confirmed; not a claim termination is already complete. |
| `Supervisor::cleanup_status(RecordId) -> CleanupStatus` | Pending or Confirmed. Terminal tombstones last for the supervisor lifetime and are bounded by a 256-entry FIFO; evicted IDs return Unknown, never inferred Confirmed. IDs are context-owned monotonic values with checked overflow refusal. |
| `Supervisor::shutdown(Deadline) -> Future<ShutdownReport>` | Close admission, cancel records and await cleanup up to supplied deadline. Report gives confirmed count and outstanding record IDs. Idempotent; repeated calls may observe further cleanup. No default timeout. |
| `worker_entry(WorkerBootstrap) -> WorkerExit` | Host's internal child-only dispatch using supplied private bootstrap pipe handles. No ordinary commands or network connection. Runtime bootstrap parsing is an internal integration API, not user CLI flags. |

`AuthRequest` owns Package (Negotiate, Ntlm or Digest), canonical host target,
Identity, channel-binding bytes and a required `token_limit: TokenLimit`.
The request owns this checked cap; there is no implicit fallback to 65,536. Parent
copies its raw-byte value exactly into Begin.token_limit. Parent checks initial
Digest and subsequent challenges before sending, and worker checks them before
native context work. Both refuse output exceeding this cap before publication.
The worker further intersects it with its provider maximum before credential/context
initialization; that can narrow but never enlarge the supplied cap. Oversized
caller input is InvalidRequest, oversized worker output is Protocol (provider
overproduction inside worker is ProviderRejected); failures cancel the conversation
and retain the existing cleanup rules. Caps do not change during a conversation.

Identity is CurrentLogon or Explicit with
owned Unicode user/domain/password. DOMAIN\user splits once; UPN uses empty domain.
Negotiate/Ntlm target is `HTTP/<canonical host>` without port/path. Digest additionally
requires Explicit identity, actual method, percent-encoded URI and nonempty initial
challenge. These three Digest fields are prohibited on other packages. Begin carries
them to the first native call on step(None), in owned zeroizing storage. Initial
challenge/token cap is min(provider maximum, supplied raw-byte TokenLimit).
TokenLimit already applies the 65,536-byte ceiling and the host's HTTP allowance
after scheme/base64 overhead,
with the entire private frame capped at 100,000 bytes. Missing/oversized or
wrong-package fields return InvalidRequest before native work; owned inputs wipe.
Digest use remains gated on native provider qualification. Channel binding must come from
the actual verified final origin TLS connection, not a configured trust root.
The library rejects missing/invalid binding for an SSPI offer. Basic uses another path.

`MechanismObservation` is Unresolved or Selected { mechanism: Kerberos/Ntlm/Digest,
authoritative: bool }. Negotiate permits Windows to select Kerberos or NTLM;
requesting it means the host permits both before the first offer. This version
cannot enforce Kerberos-only or other per-mechanism restrictions: such a caller
must reject this unsupported request before start and publish no tokens. No
requested Package or raw attributes prove an actual Negotiate selection. A
Continue may be unresolved or provisional; publish it only when accepting either
mechanism. Complete requires authoritative native identity, otherwise the library
fails before returning a publishable token. Caller never parses opaque tokens to
infer identity. Direct Ntlm/Digest have their selected provider identity.

`Deadline` is an absolute monotonic timestamp in the parent clock domain;
`Cancellation` is an owned observable signal, not a callback into your application.
Choose a shorter deadline before start if needed; it is immutable after start.
For an earlier enclosing deadline D2 during a pending step, signal Cancellation
at D2. There is no extension operation: a later D3 cannot replace the original.
Launch, native calls and token delivery consume it. There is no per-round timer reset. `Failure` carries
a fixed ErrorKind, optional numeric native status and CleanupStatus/RecordId;
no secret, identity or native error text. Kinds: UnsupportedPlatform, InvalidRequest,
IdentityMismatch, WorkerUnavailable, WorkerMismatch, ContainmentFailed, Protocol,
ProviderRejected, Timeout, Cancelled, Closed and CapacityUnavailable (closed admission).
Malformed IPC/provider failure is terminal, without an automatic fallback or retry.

A minimal host sequence:

1. Create Supervisor once per host context with a trusted installed worker path.
2. Capture the verified connection's binding. Derive the raw token allowance from
   your HTTP header limit after scheme/base64 overhead; construct
   `TokenLimit::new(raw_bytes)?`, then include it as `AuthRequest.token_limit`.
   Values 0 and 65,537 refuse; there is no default. Construct the rest of
   AuthRequest and pass your
   existing operation deadline and cancellation signal to `start`.
3. Await `step(None)`, send the token on that exclusively leased HTTP connection,
   after checking MechanismObservation against the permissive package choice,
   and pass any validated next challenge to `step(Some(challenge))`. Continue means
   another challenge is permitted; Complete means native token generation completed,
   not that the server accepted it. Keep native/provider and remote success separate.
4. On success call `finish`. On connection loss, redirect, cancellation or route
   retirement call `cancel`, discard any pending token and close that HTTP lease.
   Never move the conversation to another connection.
5. At host shutdown call `shutdown` with an explicit cleanup deadline. If outstanding
   IDs remain, keep their ownership in the supervisor; it will continue reaping.

Dropping start/step/finish futures or Conversation cancels the whole conversation.
Before launch registration it removes the waiter; afterwards the supervisor keeps
all native/IPC storage. Dropping Supervisor closes admission and cancels all records;
owned supervision remains alive for pending cleanup. A returned timeout means
result publication stopped, not that every OS call or external provider stopped.
Quarantined workers keep their capacity slot until exit plus Job emptiness plus
IPC/launch-thread completion are confirmed. Saturation waits/refuses by each
caller's deadline; max_workers does not bound time to cleanup. Forced exit does not
promise physical secret erasure or cancellation in LSASS/Pageant. No token/secret
logging is supported. Future native Supervisor construction on non-Windows returns
UnsupportedPlatform. Pure owned-value constructors work on every platform and
do not perform authentication.
