# HTTPS SSPI composition amendment — DRAFT

Status: **accepted design contract**, 2026-10-04, at root
`721e07d65aa78a8bd79d41dae86ad99629a7aefc` and the member tuple in
[acceptance](GwzSspiHttpsCompositionAcceptance.md), after Consistency/Safety GO
and retained caller Surface GO. The historical DRAFT filename is retained for
review references. This authorizes the bounded implementation of step 4b after
accepted host packaging/bootstrap step 4a, including the API and taut changes
specified below. It does not accept an implementation or authorize Windows
activation or release. Proposed-language sections below describe the accepted
implementation contract; characterization of the baseline remains historical.

Baseline: root `ac950cc7c88fb897938a5c300fb228fd17d3ea41`, core
`8cb3a3f01d79699a5ad07b6ec7cfc78321224d31`, SSPI
`616e32cceeea1b7df1d7bbe1c1695a409a733f6d`, CLI
`0c7dfaf0199731648d2360358284010b2b4575c1`, Python
`ded47130af23720099e7b6a92ccb9a161bb5db9a`.

## 1. Clock proposal and timeout-zero disposition

Retain SSPI's one finite immutable absolute deadline. There is no existing
finite operation-wide deadline to copy at the actual HTTPS challenge callsite.
The current physical connection deadline ends at connection establishment;
reusing a pooled connection does not revive it. The request's 30,000 ms
allocation timer is a different domain and cannot supply authentication time.
The active-I/O budget is cumulative charged time, not an absolute wall-clock
deadline, and may be disabled.

Recommend explicitly extending the existing positive HTTPS setup aggregate
into a **logical Open authentication-setup deadline**, captured once immediately
before its first physical checkout or adoption. It follows the anonymous Open
and its authenticated continuation, including redirects and a carried or idle
lease. It is never captured afresh at a 401, a native round, or a new physical
connection inside that logical Open. The normal positive aggregate is 30 seconds;
this is a proposed extension of that existing domain, not a new SSPI allowance.

The exact compatibility disposition selected by the lane owner is: when the effective
connect aggregate is disabled, refuse selection of SSPI before credential
publication with a fixed unsupported finite-deadline-policy failure. Do not
quietly restore 30 seconds, borrow allocation/network/helper time, or launch a
cancellation-only worker. Anonymous and existing Basic behavior remain governed
by their existing zero-timeout contract. An already-expired positive deadline
produces Timeout, not this unsupported-policy refusal.

On 2026-10-04 the operator instructed the owner to settle timeout-zero behavior
and proceed through review to implementation. Under that delegation, the owner
selects finite native setup or refusal at zero, preserving the SSPI deadline
contract without an invented allowance. This is a recorded owner disposition,
not an inference from elapsed time. Independent acceptance of this corrected
proposal is recorded in the linked acceptance before dependent implementation.

This also makes the positive aggregate run through challenge-dependent helper
work on a route that can select SSPI. There are **no pauses or extensions**.
Helper admission and interaction still have their independently captured M10
and M4 allowances; expiration of the enclosing authentication-setup deadline
cancels the operation with setup/authentication timeout provenance, without a
helper cause or a default-logon fallback. This tighter enclosing boundary is a
specific proposed compatibility change; “M4/M10 unchanged” must not conceal it.
If the operator requires helper work to outlive this boundary, that requires a
different explicit clock-source decision before implementation, not a pause
hidden in the adapter.

## 2. Authority and exact amendments proposed

[GwzSspiDesign §6](GwzSspiDesign.md) currently requires an “absolute existing
operation/setup deadline”, unchanged M4/M10 provenance, and matching originating
caller identity. The code does not currently supply the first or a transferable
capture suitable for the last. Proposed clarifications are:

1. Replace the ambiguous clock source with the logical Open source and transition
   rules in §4 below. Preserve the fixed deadline requirement of SSPI §§5–6,
   including late-output rejection and retained cleanup ownership.
2. State the enclosing positive aggregate's effect on challenge-dependent helper
   work, and the disabled-aggregate refusal above. No helper allowance, helper
   timeout tag, or new timeout setting is added.
3. Add the minimal owned originating-thread capture seam in §6 below. Preserve
   primary SID/LUID/session matching, impersonation refusal, original-thread
   liveness, charged launch-time recheck, and all captured-handle cleanup rules.

[GwzRemoteTransportRetryPlan §4 and §§7–8](../gwz-core/dev-docs/GwzRemoteTransportRetryPlan.md)
currently end HTTPS retry setup before the first request byte and describe zero
as disabling the aggregate. The proposed authentication-setup clock extends the
deadline's scope only; it **does not extend retry eligibility** beyond that byte.
Help must explicitly describe the new native-authentication zero disposition and
positive aggregate scope. Existing setup attempts/backoff, stall controls and
post-effect prohibition remain unchanged.

[CredentialHelperTimingAmendment M4/M10](../gwz-core/dev-docs/GwzTransportCredentialHelperTimingAmendment.md)
keeps its exact independently captured allowances and helper failure fields.
Only the explicit enclosing deadline interaction above is added. A failure of
SSPI admission/Hello/provider/IPC must never acquire helper provenance.

[WindowsParityDesign §7](../gwz-core/dev-docs/GwzTransportWindowsParityDesign.md)
is a DRAFT overall. OD10 challenge-dependent helpers-first selection and OD16
unrestricted default-logon origin policy are operator directions, not open
choices for this amendment. No zone check or opt-in source policy is proposed.
Its §8's TLS/provider integration statements are implementation proposals until
reviewed and qualified. Digest's unavailable H(Entity) input remains a separate
refused gate; this draft neither adds it nor silently downgrades a selected
Digest request to Basic.

The existing transport contract needs a small explicit native source/facts
extension (§7), authored through taut after approval. This does not change the
SSPI wire contract, add an application request carrier, or authorize a blanket
protocol redesign. [Plan step 4](GwzSspiPlan.md) then closes composition; step 5
Windows qualification and release activation remain separate.

## 3. Actual production paths and existing facts

Paths below are current code, not a proposed session-host facade.

| Seam | Actual implementation | Consequence |
|---|---|---|
| CLI entry | `gwz-cli/src/globalargs/dispatch.rs:11`, `execute_invocation_selected` → `with_local_transport` | Candidate transport is only `all(unix, gwz_transport_candidate)`; no Windows SSPI handoff exists. |
| CLI descriptor | `gwz-cli/src/worker_host.rs:25`, `executable`, reexported as `sspi_worker_executable` | Trusted compiled fingerprint plus absolute current executable; callable but not yet connected to HTTP. |
| Python entry | `gwz-py/native/src/client_host.rs:156`, `ClientHost::network`, shared by `call` and `submit` | Captures route and registers operation at native entry with the GIL, before detach/thread handoff. |
| Python execution | `native/src/client_host.rs:233`, `Network::run`; `native/src/dispatch/mod.rs`, `spawn_call` | Submitted calls move to an operation thread; later endpoint work moves again to `gwz-https`. |
| Python route | `native/src/route/transport.rs:49`, `capture`, then `run` at 167 | Explicit environment/cancellation snapshot is captured before handoff; Windows currently selects the native Git2 route. |
| Python descriptor | `native/src/worker_host.rs:16`, `descriptor` | Uses the actual loaded extension image and compiled metadata, not Python attributes, runtime env, cwd or PATH. |
| Core invocation | `gwz-core/src/transport_host/local_command.rs:11`; `cancellable.rs:40` | Actual constructors carry metadata/environment/cancellation, not a finite operation deadline. |
| Logical HTTPS Open | `transport_host/request.rs:56`, `open_https_recording` | Anonymous→Gh route gate and retained allocation are real. `until` at 105 bounds allocation/admission only. |
| Endpoint task | `transport_host/https_endpoint.rs:95`, `HttpsEndpoint::new` | Dedicated HTTPS runtime; `Entry` and `Retry` retain the attempt, cancellation, budget and challenge lease. |
| Attempt | `git/endpoint/https_worker/prepare.rs:22`, `run_attempt` | Accepts only Anonymous/Gh; helper precedes scoped checkout for Gh; connect/IO budgets are durations. |
| Budget | `git/endpoint/https_worker/budget.rs:23` | Positive configured/Open values shorten; network is cumulative active IO; disabled domains are `None`. |
| Physical deadline | `git/endpoint/https_connection.rs:247`, `poll_connected` | Absolute deadline selects against establishment only. `HttpLease` has elapsed durations, no live logical deadline. |
| Final origin TLS | `git/endpoint/https_connection.rs:444`, origin `handshake` | Concrete TLS exists here before type erasure/Hyper; earlier proxy TLS is a distinct channel. No CBT capture exists today. |
| Physical exclusivity | `git/endpoint/https_pool.rs:229`, `HttpLease`; `https_worker.rs`, `ChallengeLease` | Checkout marks non-reusable; carried challenge holds the exclusive lease and validates destination/session/operation. |
| Route retirement | `https_operation.rs`, owned `Dependency`; `https_policy.rs`, `Routes` | Request registration owns route lifetime; finishing an operation waits for dependent cleanup rather than remote/RPC drop alone. |
| Effect/retry | `https_worker/serve.rs:99`; `transport_host/https_endpoint/retry.rs:78` | Receive-pack declares Possible before send; retry setup is only a failed first fresh connect, not any later auth failure. |

`gwz-core/src/lib.rs` and `git/gitbackend/transport_binding.rs` enclose candidate
transport in `all(unix, gwz_transport_candidate)`. `AuthPolicy` currently has
SshAmbient, SshExplicit, Anonymous, Gh; `AuthMethod` has None, SshAgent, SshKey,
Gh. `gwz-transport/src/policy.rs` pairs HTTPS Anonymous with CredentialsDisabled,
and Gh with Ambient. Current CredentialsDisabled means no credentials in this
transport; it cannot silently become Windows “helpers disabled, current logon
allowed”. The taut source is `gwz-transport/protocol/transport.taut.py`.

Current challenge parsing projects bounded diagnostic scheme names only
(`https_worker/challenges.rs`). Native challenge tokens need their own strict,
bounded private parser/owned secret path, not a diagnostic string or new transport
wire credential field. Native token completion cannot imply HTTP success.

## 4. Fixed clock transitions proposed

Use the effective positive connect aggregate already selected from configured
policy and a positive smaller Open cap. Capture `D = now + aggregate` once at
logical Open's first lease checkout/adoption, after request admission. This is a
new logical anchor using an existing duration, not the physical pool resource's
creation instant. Allocation waiting remains allocation; no part of its 30-second
timer is donated. Monotonic arithmetic must be checked; no lossy conversion may
make a deadline later. Core and SSPI use the same monotonic domain.

| Transition | Deadline behavior | Retry/provenance |
|---|---|---|
| Fresh first lease | Capture D before checkout; DNS/TCP/proxy/final TLS establishment consumes it | Existing pre-first-byte fresh-connect retry remains eligible only under its closed classifier. |
| Reused first lease | Capture D at this logical Open's adoption; do not use the expired resource's old connect deadline | Reuse is not a retryable setup failure. |
| Anonymous discovery GET / validated redirect | Keep D; HTTP wait and redirect connection establishment consume it | First request byte already ended retry setup; no redirect-auth retry promotion. |
| Anonymous→authenticated continuation / carried lease | Carry the exact D with route/budget/lease; never reconstruct from remaining connect duration | Same logical Open and final U; no fresh 401 allowance. |
| Challenge-dependent helper admission/execution | D keeps running; M10/M4 remain independent; enclosing expiry cancels rather than extending D | No helper label for enclosing expiry; no current-logon fallback after timeout/cancel. |
| SSPI capacity, launch, Hello, Begin and ≤8 native rounds | Pass D once to start; every wait/step uses it | Native admission/provider/IPC timeout is terminal with no helper cause. |
| Existing active HTTP IO await exhausts its remaining budget, or explicit cancel | Signal the conversation's Cancellation on that actual timeout/cancel observation | D is not replaced or extended; first terminal arbitration remains authoritative. |
| Native Complete | D remains in force for Finish and the outstanding authentication exchange | A token is only an offered credential; server acceptance is still required. |
| Valid remote authentication response and native Finish | End this setup phase after terminal publication checks and disposal decision | Streaming/body uses existing IO/effect rules, never the old physical connect timer. |

There are no pauses of D. The existing network budget remains cumulative active
HTTP IO time, charged at the already existing HTTP awaits; it is not charged
for helper gaps, SSPI capacity/IPC/provider work or other intervals that are not
those awaits. A remaining Duration is not a continuously running timer. Core
must not invent a network timer during those gaps or claim one already observes
earlier IO expiry. The observer is the existing per-HTTP-await timeout with its
retained remaining allowance, plus exhaustion checks before the next await.
When that observer reports timeout, core immediately signals the retained native
conversation's Cancellation and discards the lease. Native rounds are serial
with their HTTP awaits; D independently runs throughout both and must have its
own control observation, including during helper/native work. Thus this proposal
extends the setup aggregate explicitly and does not retime active IO or borrow
its allowance. Existing continuously running cancellation observers stay active.

Retained allocation/backoff rules remain their existing
separate rules; a new eligible first-connect attempt may get the next attempt's
existing aggregate under the accepted retry plan. Once a first request byte was
sent, no new attempt or logical clock is created to recover authentication. Route
continuations retain D even when the duration-based `Budget.connect` has not been
charged for elapsed HTTP/helper work.

Timeout zero supplies no finite D. Refuse only native selection before Begin or
Authorization publication, preserving offered scheme diagnostics and the fixed
unsupported-policy reason. Positive D that expired while discovery/helper work
was in progress yields Timeout. Do not call either case “helper timeout”. A
result written/read before D but not yet eligible for core publication when D
expires must not publish; guard arbitration uses fresh time, not stale pre-lock
time. Test exact boundary equality as expired.

The broader alternative is to amend SSPI to accept an absent deadline and rely
solely on cancellation. That would change its frozen public constructor/start
contract and weaken bounded control: a caller that never cancels can retain every
permit indefinitely during a stalled provider call. Cleanup might remain owned,
but there would be no finite terminal control deadline. It needs a separate
safety/API amendment and is not recommended or implied by timeout zero. This
draft does not authorize that alternative.

## 5. Source selection, identity and final origin

Compose the operator's OD10/OD16 policy, without a new caller mechanism knob:
initial discovery anonymous; verified final repository base U after allowed
discovery redirects; complete case-insensitive challenge set; helper once for U
when NTLM/Basic/Digest is offered and AllowConfigured applies. Usable helper
identity selects highest offered Negotiate, NTLM, Digest, Basic. Negotiate-only
does not ask a helper. No usable helper permits current logon, Negotiate before
NTLM. Disabled disables helpers only on this explicit Windows policy. Timeout,
cancel and rejected explicit identity are terminal. No source/scheme replay.
Unsupported Digest is a fixed refusal; no implicit downgrade or guessed body
hash. No zone restriction is introduced.

Core converts helper username/password directly into fixed owned zeroizing SSPI
text, retaining accepted DOMAIN/user and UPN semantics; no ordinary String/Vec
secret intermediate. CurrentLogon is distinct from empty helper credentials.
The library remains source-policy independent. Target is `HTTP/<canonical U
host>` without port/path/IPv6 brackets. Existing core canonicalization supplies
the host, not an adapter-created stricter DNS policy.

Capture tls-server-end-point at the concrete final-origin native TLS stream,
after certificate verification and before erasing its type. Bind it to this
physical lease generation and verified U. Proxy TLS/CONNECT credentials are never
origin CBT or origin identity. Missing/error/unusable CBT refuses SSPI before
offering tokens, without disabling anonymous/Basic. Native-tls returns an ordinary
dependency-owned digest Vec; copy into the checked zeroizing binding owner and
wipe the initialized dependency buffer immediately. Its allocation/hidden
internal copies are a documented dependency limit, not full-erasure evidence.
The worker owns its SEC_CHANNEL_BINDINGS layout and prefix exactly once.

A replacement physical connection requires new TLS capture and a fresh native
conversation from the same selected source, and only when existing retry rules
permit replacement. Equal CBT bytes are not proof of the same physical lease.
Authenticated redirects terminate; never send an old context/token to another
origin. Peer/renegotiation behavior and actual Windows EPA remain qualification
rows, not facts established by portable composition tests.

## 6. Originating-thread capability and caller-guide proposal

`Supervisor::new` captures primary identity, but `Supervisor::start` also
synchronously captures the thread calling start (`supervisor/api.rs:51`). Its
Windows origin holds a real non-inheritable thread handle and launch verifies
thread liveness, no impersonation, and primary SID/LUID/session
(`supervisor/windows/identity.rs:131`). Calling start on `gwz-https` therefore
captures that thread, not the original Python/CLI caller. At the original entry,
final U, concrete TLS binding and the challenge request do not exist yet. Merely
moving a future or capturing only the constructor cannot solve this seam.

Propose exactly this public addition, exported from `gwz_sspi`:

```rust
pub struct CallerCapture { /* private fields */ }

impl Supervisor {
    pub fn capture_caller(&self) -> Result<CallerCapture, Error>;

    pub fn start_captured(
        &self,
        caller: &CallerCapture,
        request: AuthRequest,
        deadline: Deadline,
        cancellation: Cancellation,
    ) -> impl Future<Output = Result<Conversation, Failure>> + Send + use<>;
}
```

These are proposed signatures, not executable declarations or implemented APIs.
`CallerCapture: Send + Sync`; it implements neither Clone nor Debug, exposes no
fields/getters/raw handles, and contains a shared immutable origin owner plus
the opaque identity of the issuing Supervisor context. The private Origin port
becomes Send + Sync; its verification only reads the held original-thread handle
and captured primary owner. Private per-Start origin tickets retain that owner
with Arc, without duplicating or recapturing an OS thread handle. No callback,
mutable global, new admission limit or process owner is introduced. The capture
retains context identity, not an ability to keep admission open after shutdown.

`capture_caller` synchronously checks that admission is open, duplicates the
current caller's thread into an owned non-inheritable handle, refuses its
impersonation and verifies primary SID/LUID/session against this Supervisor.
It rechecks closed admission before returning; closure returns ErrorKind::Closed.
Capture/verification failure uses existing ErrorKind::IdentityMismatch, including
thread-handle capture failure; no native provider status is attached. No record,
capacity permit, worker or request is allocated by capture. Synchronous metadata
and eventual last-reference handle disposal have no hard OS time bound. No OS
metadata lookup is moved into Future::poll.

`start_captured` first checks issuing-context identity and request profile,
retains one private origin ticket, then uses the existing owned Start admission
and terminal machinery. Foreign context or invalid request returns
Failure.kind() == ErrorKind::InvalidRequest, with no native status, no record and
Confirmed cleanup for that refused Start. It does not invalidate/drop the
caller's capture. Closed admission returns Closed; cancellation/expiry return
Cancelled/Timeout through the same first-terminal arbitration as ordinary start.
No worker/native effect occurs on any pre-registration refusal. Original-thread
exit, impersonation or primary mismatch at charged launch verification returns
IdentityMismatch. All other worker/containment/provider/cleanup errors retain
their existing classifications and record-status semantics.

The synchronous captured-start call does not query metadata, duplicate a native
handle or create a thread/worker: it validates owned values and retains references.
Future::poll obeys the existing prohibition on metadata/provider/creation/IPC/
wait/join and caller-waker operations under state locks. Request and private
ticket owners are released on completed pre-registration refusal outside locks,
even if the caller retains the completed future. Last-reference handle disposal
may be synchronous under the already accepted disposal exception.

Proposed caller-guide behavior (also extracted in
[the cold draft guide](GwzSspiHttpsCallerGuide-DRAFT.md)):

1. The explicit host owner constructs a Supervisor from its checked installed
   descriptor; existing synchronous constructor metadata/driver creation limits
   remain documented. CLI owns one context for its invocation; Python ClientHost
   owns one explicit context for its operations. Do not multiply an eight-worker
   default by every member or add a mutable global supervisor.
2. At CLI invocation dispatch or Python `ClientHost::network`, capture the
   originating-thread capability synchronously and move it once into that
   registered operation's core route owner. Failed capture refuses native use;
   do not replace it with endpoint/executor identity.
3. A captured-start entry borrows that operation capability only during the
   synchronous call and retains its own private checked origin reference in the
   returned owned Send + 'static Start. Each Start's admission ticket is one-use;
   polling/moving it cannot duplicate registration or launch. The operation
   capability itself may serve multiple sequential/concurrent Opens under the
   same existing context limit: it is not publicly clonable, and is not consumed
   by the first Open. This multiplicity must be explicit in the reviewed API.
   Eligible fresh-connect retries derive a new one-use admission ticket from
   the same retained operation capture, never recapture a member/executor thread.
   Ticket derivation retains the existing origin owner; it does not guess that
   duplicating an OS handle proves identity. Every launch rechecks that owner.
4. Dropping the operation capability releases its own held origin reference.
   Already admitted/queued Starts keep the actual origin alive independently;
   dropping/refusing each Start releases its reference outside state locks, with
   the accepted synchronous captured-handle-disposal exception and no hard OS
   bound. Drop never transfers identity or publication authority.
5. Original-thread exit, later impersonation or primary mismatch still refuses
   at charged launch verification. Moving to an operation/HTTPS thread does not
   change provenance. Conversation cleanup/permit/tombstone behavior is unchanged.

Ordinary start retains its current capture-at-call contract. Context mismatch is
a fixed refusal before worker/credential effects. The proposed extraction is a
public SSPI API amendment, not already covered by the frozen caller guide.
An API claiming a consumed one-use origin for every conversation would require
unknown-many captures before host handoff; do not encode that unusable alternative
or make a hidden recapture on the HTTPS thread.

The captured-start future is owned Send + 'static and borrows neither capture
nor Supervisor. The finite Deadline is D from §4, not `now + 30s` at this call.
Both capture sharing and owned future lifetime require positive Send/Sync/'static
checks, negative Clone/Debug checks and compiled caller recipes during subsequent
implementation. There is no new token format or mechanism parameter. Surface
reviews this exact proposal; it does not choose deferred code semantics.

Connect these owners directly to the actual entries in §3: CLI's checked
self-executable descriptor; Python's private loaded-image descriptor and native
route capture; core TransportRuntime/RequestContext/endpoint operation owner.
Do not add an unconnected future HostContext/session_host facade. Absent metadata,
missing installed worker, mismatch Hello, unsupported platform and constructor
failure are fixed native-unavailable refusals; no in-process/PATH/native-Git
fallback after selecting this transport. Packaging receipt is not runtime trust;
compiled expected fingerprint and worker Hello remain the check.

Native availability is not a prerequisite for anonymous HTTPS, existing Basic or
SSH. An entry that cannot obtain the native descriptor/Supervisor/caller capture
retains its fixed native-unavailable error as an operation-owned outcome; it
does not fail an otherwise ordinary operation merely for lacking a worker.
Selecting a native scheme later consumes that refusal before native/token
effects. No late executor-thread recapture or fallback is attempted. Missing
worker/fingerprint and caller-capture failures remain distinct fixed kinds;
only the native-selected path requires this capability.

CLI capture belongs in `execute_invocation_selected` before entering
`with_local_transport`: that function constructs its runtime before invoking
the action that can fan out into member jobs. Python capture belongs in
`ClientHost::network` while still on the native caller, before `call` detaches or
`submit` schedules an operation thread, and before operation-slot waiting. The
later `with_cancellable_local_transport` ingress is already on a worker thread
for submit and is too late to infer original identity. Registration, admission,
fanout, cancellation and final request cleanup retain the explicit operation
capture. No per-stream callback to the original user thread or global cache is
required.

## 7. Minimal transport policy/facts amendment proposed

The chosen proposal adds these unused tags to the existing taut schema. Existing
names, tags and encoded fields retain their meanings. No generated change occurs
in this draft.

| Schema addition | Exact name and tag | Meaning |
|---|---|---|
| AuthPolicy enum | `windows_configured = 5` / Rust WindowsConfigured | Helpers allowed under OD10; current logon permitted under OD16. |
| AuthPolicy enum | `windows_default = 6` / Rust WindowsDefault | Helpers disabled; current logon permitted under OD16. |
| AuthMethod enum | `sspi = 5` / Rust Sspi | Native origin authentication selected; not evidence of remote acceptance. |
| Facts field | `native = F(7, Ref.NativeFacts, optional=MISSING_OK)` | Omitted for existing methods; present exactly for Sspi. |
| NativeSource enum | `configured = 1`, `current_logon = 2` | Actual selected source, never inferred merely from helper invocation. |
| NativeScheme enum | `negotiate = 1`, `ntlm = 2`, `digest = 3` | Selected HTTP authentication scheme; Digest remains refused. |
| NativeObservation enum | `not_started = 1`, `unresolved = 2`, `selected = 3` | Separates pre-provider refusal from an actual native observation. |
| NativeMechanism enum | `kerberos = 1`, `ntlm = 2` | Mechanisms implemented by this checkpoint; no Digest mechanism is added. |

Exact new message shape:

```python
NativeFacts=Msg(
    source=F(1, Ref.NativeSource),
    scheme=F(2, Ref.NativeScheme),
    observation=F(3, Ref.NativeObservation),
    mechanism=F(4, Ref.NativeMechanism, optional=True),
    authoritative=F(5, BOOL),
)
```

The existing identity shape is sufficient: permit only the additional pairs
`(Https, WindowsConfigured, Ambient)` and `(Https, WindowsDefault, Ambient)`.
There is no new IdentityMode; Ambient here means source selection by the origin
policy, not that a helper-produced Explicit identity is already in the Open.
Both key_path/path_base must remain null. Anonymous/CredentialsDisabled remains
no credentials; Gh/Ambient retains its existing configured-helper Basic meaning.
No new CLI/Python application request field is added: when this gated Windows
transport is eventually selected, AllowConfigured maps to WindowsConfigured,
Disabled to WindowsDefault. Portable platforms retain their existing mapping.

Closed native facts validity, checked in every Opened/Closed/Failure facts path:

- `method == Sspi` iff `native` is present. Other methods omit tag 7, not encode
  it as null. Sspi has null key_fingerprint and ssh_exit_status.
- NotStarted requires null mechanism, authoritative=false and
  credential_offered=false. Digest is legal only with Configured + NotStarted;
  it records the fixed pre-provider refusal, never a token or successful result.
- Unresolved requires Negotiate, null mechanism and authoritative=false. It
  represents an actual unresolved Continue observation; direct NTLM cannot use
  it. Source may be Configured or CurrentLogon.
- Selected requires nonnull mechanism. Negotiate permits Kerberos or Ntlm;
  Ntlm scheme permits only Ntlm. Preserve the actual MechanismObservation
  authoritative value independently of TokenStatus. Continue may be provisional
  or authoritative; direct NTLM can already be authoritative on Continue.
  Core tracks native Complete separately in the owned conversation state;
  authoritative mechanism selection alone does not mean negotiation completed.
- Selected source/scheme cannot change through a logical Open; a resolved
  mechanism cannot later switch. Producer/state checks enforce these temporal
  rules; structural codec checks alone do not prove a conversation history.
- Existing `Facts.authenticated` stays `Option<bool>`: field 3 is a required
  nullable BOOL (`optional=True` in taut), encoded null for None/unknown, false
  for Some(false), true for Some(true). No new tri-state enum or field is added.
  Native/provider failure or Complete alone leaves None. Actual remote rejection
  after publication sets Some(false); accepted remote authentication sets
  Some(true), requiring credential_offered=true, Selected/authoritative=true,
  independently observed native Complete and valid remote acceptance under the
  existing terminal/publication checks. An authoritative Continue cannot satisfy
  that completion prerequisite. Cancellation/expiry before Complete preserves
  the actual mechanism authority and leaves authenticated=None unless an actual
  remote rejection was observed.
  `http_status` remains the actual response status, not a synthetic native status.
- `credential_offered` becomes true only when Authorization is actually
  published. It remains monotonic for the logical Open. Local failure before
  publication must not manufacture remote rejection or HTTP success.

Binding and profile disposition is exact: reuse profile 2 and optional profile 3;
do not add profile 4. Bootstrap envelopes remain version 1. Profile 1 post-bind
Open/messages cannot use native policy, Sspi or NativeFacts. Profile 3 retains
its existing unstable-sequenced feature and message_seq requirements; this
amendment does not stabilize or activate it. Native hosts explicitly offer
profile 2 (and may additionally offer 3 under its existing feature), rather than
changing default `binding::bind`'s `[1]` offer.

The endpoint's normal capability intersection admits native policies only when
both offer and Bound explicitly include the selected native policy, Scheme::Https
and a negotiated profile >=2. For a Bound at profile 1, native policies must be
removed from its intersection; if that leaves no supported policy, BindRejected
is UnsupportedOperation. A native Open under an otherwise valid Binding that
did not acknowledge that exact policy is UnsupportedOperation before effects.
Wrong scheme/identity, native fields in a non-native method, or native payload
under profile 1 is InvalidRequest/codec InvalidMessage. Existing envelope limits
and unknown/missing/duplicate/type checks apply to the bounded five-field record.
Opened reuse validation must permit offered Sspi under a native binding while
retaining its existing non-native rules; this is not permission to reuse an
unfinished or mismatched physical authentication scope.

An old decoder has closed AuthPolicy/AuthMethod enums and can reject a bootstrap
offer containing new policy tags before it can send capability rejection. That
is a fail-closed protocol refusal, not an UnsupportedVersion promise or automatic
downgrade. For offers containing only old policy tags, an upgraded endpoint must
omit native policy tags from Bound and never emit Sspi/tag 7; old profiles and
wire shapes then remain unchanged. A peer falsely advertising native policies
but unable to consume native facts violates the negotiated capability contract;
the host closes that binding without replay or fallback. Capability admission
therefore supplies the feature boundary without a profile bump.

Source/scheme/provider observations are bounded nonsecret enums, never a token,
password, username, SID, target or CBT diagnostic. Existing effect/setup/helper
provenance and bounded offered-scheme fields remain unchanged. Digest completion
would require a later explicit mechanism/schema and native-input amendment;
there is no inferred future tag or current permission to use that path.

## 8. Physical lease, publication, cleanup and effects

One exclusive HttpLease owns all challenge rounds. Core serializes strict bounded
401 token parsing, SSPI steps and HTTP Authorization requests; no unrelated task
can use the socket. Retain one source, final U, immutable D, physical generation,
CBT, conversation and operation dependency together. At most eight native rounds;
header/token budget is derived from existing HTTP limits before allocation and
native admission. A final native token may still need HTTP publication; native
Complete is not permission to release the socket or declare authentication.

On failure/cancel/deadline, revoke native publication, discard the physical lease,
and retire that authenticated route scope. Pending native cleanup stays owned by
the existing Supervisor with retained capacity; pending connection cleanup stays
owned by the existing pool/endpoint. Keep the operation dependency/retirement
record until both are accounted for. Late Token/HTTP success cannot reopen the
scope, make the lease reusable or overwrite the saved terminal outcome. Do not
add a second process owner or block a state lock on cleanup/wake/native work.

Successful authentication needs the valid expected HTTP response, no earlier
terminal control result, and a completed native Finish/disposal decision before
reusable publication. If native Finish is Pending or fails, discard the socket
and keep cleanup status explicit; do not label it reusable. Core may preserve an
already accepted remote result separately from cleanup, but cannot manufacture
confirmed cleanup or overwrite a saved first terminal. The exact success/cleanup
projection must use existing operation cleanup reporting, with tests of both
publication orders.

Receive-pack marks Effect::Possible before sending its POST today. Preserve this
conservative boundary: no partial/full POST replay, source switch, alternate
scheme or new auth attempt. For an unauthenticated later service request, any
authentication exchange must complete before POST effect starts; a 401 received
after POST submission is terminal. Fetch/body failure also remains outside setup
retry. The existing first-connect classifier decides pre-byte retry; “setup” in
the new clock name is not a classifier shortcut.

Secret owners allocate initialized fixed storage before copying and wipe before
deallocation on every path. Authorization formatting/base64 is a direct bounded
write into zeroizing fixed output. Hyper HeaderValue and HTTP/TLS buffers make
dependency-owned copies; sensitive marking only redacts diagnostics, it does not
wipe. Keep these limits explicit and assess bounded lifetime/drop behavior. No
claim of whole HTTP stack erasure follows from SSPI's allocation audit.

## 9. Test visibility, budget and exit gates

Production activation remains `all(unix, gwz_transport_candidate)` until separate
Windows qualification/activation approval. Extract only the minimum private
composition state/ports into enclosing portable test modules, or add an explicit
test-only enclosing boundary, so production orchestration can be tested without
making the Windows endpoint selectable. Avoid widening the public transport
module to Windows merely to compile tests. Windows target-checking of the native
capture/CBT adapter is source evidence, not live endpoint authentication evidence.
No generated files change during this draft.

Proposed cohesive implementation ceiling after acceptance: 3,500 handwritten
added lines including tests, 26 production/test files across core/transport/SSPI/
CLI/Python, and eight member/API docs. Count generated projections and lockfiles
separately. Use existing retry/effect/IO/pool ports and no new runtime dependency
or process owner. Stop for owner disposition before 120% or a structural change;
do not split review into individual native functions. This estimate is a budget
proposal, not authorization for new dependencies or all files within that count.

Meaningful RED first, then GREEN on the same production bridge:

- Fake monotonic schedules: fresh and reused lease anchors; exact expiry;
  discovery redirects; Anonymous→authenticated carry; helper work crossing D;
  timeout zero; earlier network expiry; eight rounds; no reset after 401.
- Origin capture on original entry, moved/submitted futures and endpoint threads;
  wrong context, exited/impersonating origin and primary mismatch; capability
  drop before/after start; multiple Opens with one operation owner; no OS in poll.
- Real callable CLI/Python descriptor handoff, unavailable/missing/malformed
  packaging and fake Hello mismatch; no helper/default/provider effect after
  refusal. No real credentials or external network in fast tests.
- Full challenge/source matrix including helpers disabled, Negotiate-only,
  mixed offers, no usable identity, helper timeout/cancel, explicit rejection,
  Digest refusal and no silent downgrade. Authoritative mechanism facts and
  incapable-peer admission tests against independently generated wire vectors.
- Native facts preserve authoritative NTLM Continue, including cancellation or
  expiry before Complete; an early remote response cannot bypass native Complete.
  Only native Complete plus valid remote acceptance may publish authenticated
  success. Cover unresolved/provisional Negotiate and remote rejection separately.
- Lease exclusivity, final-origin CBT vs proxy TLS, replacement generation,
  authenticated redirect refusal; seeded late output/cancel/cleanup schedules
  and both terminal-publication orders through actual orchestration ports.
- Effect::Possible before receive-pack send; every post-byte auth/body failure
  terminal; existing eligible fresh-connect retries retain accepted provenance.
- Per-owner wipe observations before deallocation for challenge/identity/CBT/
  Authorization and all failure paths, with honest dependency-copy limits.

Exit gates: standalone member tests, strict targeted Clippy and changed-source
formatting, public transport generation/check, disabled-branch syntax-scope
guard, host integration/extracted packaging regression, and compile-fail/caller
recipe tests for the proposed capture surface. Builds remain external. Separate
later Windows campaigns must prove original-thread/native runtime behavior,
NTLM/Kerberos HTTP completion, final-origin EPA/TLS algorithms, physical cleanup
and installed host Hello mismatch; they are not authorized by this draft.

No production source, API/schema, build or campaign has been changed or run in
this characterization. The acceptance blockers are the explicit logical-clock
extension/zero refusal and caller-capture/public facts contracts above, rather
than any uncertainty about whether a native deadline can be reset: it cannot.
