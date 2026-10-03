# gwz-sspi caller guide

2026-10-03. DRAFT API contract; the package and worker are not released yet.
Windows-specific native SSPI authentication, one contained process per conversation.
It produces authentication tokens; it does not make HTTP requests or decide whether
a server authenticated you. Core/CLI/Python application protocols do not change.

Install the matching `gwz-sspi` library and worker with your application. A CLI can
call `worker_entry(bootstrap)` from an internal self-exec mode before its normal
startup. A Python wheel bundles the dedicated worker executable alongside its
extension and uses its absolute installed path. No PATH lookup, shell or child
Python interpreter. Removing/upgrading the application removes/replaces its worker
together. Missing workers and protocol/build mismatches are errors, not fallbacks.

The following signatures specify the public API; implementation has not landed.
All async methods return owned results. SecretBytes/SecretText have zeroizing
storage and no Debug/Clone. Conversation and Supervisor own resources; there are
no raw native handles or pointers in this caller API.

| API | Contract |
|---|---|
| `Supervisor::new(WorkerExecutable, Options) -> Result<Supervisor, Error>` | Absolute installed worker path; capture current primary Windows identity. Refuse impersonation or unsupported OS. Options defaults: `max_workers = 8`; accepted range 1–64. |
| `Supervisor::start(AuthRequest, Deadline, Cancellation) -> Future<Result<Conversation, Failure>>` | Wait for capacity, launch and verify matching worker; no native authentication until `step(None)`. Deadline is the caller's absolute monotonic deadline. No default timeout. |
| `Conversation::step(Option<SecretBytes>) -> Future<Result<TokenStep, Failure>>` | First step is None and sends Begin; later steps supply challenge bytes. At most eight total steps. One step at a time through mutable ownership. Output status Continue or Complete, checked attributes and owned SecretBytes. Complete forbids further steps. |
| `Conversation::finish(self) -> Future<Result<(), Failure>>` | Normal native cleanup and confirmed process/Job/thread exit. Uses original deadline; drop of this future cancels and retains supervision. May return Pending cleanup in Failure if deadline expires. |
| `Conversation::cancel(self) -> CancellationReceipt` | Immediately revoke result publication and initiate termination. Receipt names opaque record ID with cleanup Pending or Confirmed; not a claim termination is already complete. |
| `Supervisor::cleanup_status(RecordId) -> CleanupStatus` | Pending or Confirmed. Terminal tombstones last for the supervisor lifetime and are bounded by a 256-entry FIFO; evicted IDs return Unknown, never inferred Confirmed. IDs are context-owned monotonic values with checked overflow refusal. |
| `Supervisor::shutdown(Deadline) -> Future<ShutdownReport>` | Close admission, cancel records and await cleanup up to supplied deadline. Report gives confirmed count and outstanding record IDs. Idempotent; repeated calls may observe further cleanup. No default timeout. |
| `worker_entry(WorkerBootstrap) -> WorkerExit` | Host's internal child-only dispatch using supplied private bootstrap pipe handles. No ordinary commands or network connection. Runtime bootstrap parsing is an internal integration API, not user CLI flags. |

`AuthRequest` owns Package (Negotiate, Ntlm or Digest), canonical host target,
Identity and channel-binding bytes. Identity is CurrentLogon or Explicit with
owned Unicode user/domain/password. DOMAIN\\user splits once; UPN uses empty domain.
Negotiate/Ntlm target is `HTTP/<canonical host>` without port/path. Digest additionally
requires the actual method, percent-encoded URI and initial challenge; Digest use
remains gated on native provider qualification. Channel binding must come from
the actual verified final origin TLS connection, not a configured trust root.
The library rejects missing/invalid binding for an SSPI offer. Basic uses another path.

`Deadline` is an absolute monotonic timestamp in the parent clock domain;
`Cancellation` is an owned observable signal, not a callback into your application.
A request may shorten but never extend its initial deadline. Launch, native calls
and token delivery consume it. There is no per-round timer reset. `Failure` carries
a fixed ErrorKind, optional numeric native status and CleanupStatus/RecordId;
no secret, identity or native error text. Kinds: UnsupportedPlatform, InvalidRequest,
IdentityMismatch, WorkerUnavailable, WorkerMismatch, ContainmentFailed, Protocol,
ProviderRejected, Timeout, Cancelled, Closed and CapacityUnavailable (closed admission).
Malformed IPC/provider failure is terminal, without an automatic fallback or retry.

A minimal host sequence:

1. Create Supervisor once per host context with a trusted installed worker path.
2. Capture the verified connection's binding and construct AuthRequest. Pass your
   existing operation deadline and cancellation signal to `start`.
3. Await `step(None)`, send the token on that exclusively leased HTTP connection,
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
logging is supported. Non-Windows constructors return UnsupportedPlatform.
