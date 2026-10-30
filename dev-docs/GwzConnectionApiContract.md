# GWZ Connection API contract

Accepted acquisition amendment: [GwzCredentialAcquisitionAmendment.md](GwzCredentialAcquisitionAmendment.md)
at root 5dd59da1 controls the exact acquisition clauses listed in its §7.
Its one directly awaited acquisition replaces the old public selection chain;
prior acceptance below remains historical for its recorded tuple.

Date: 2026-10-19. Revision 2. **Accepted as an interface/design contract at root
`f4aff9b0b6524711b5065eb0d088e41ef43ecabe` after
[Consistency-2](GwzConnectionApiContract-ReviewConsistency-2.md) and
[Safety-2](GwzConnectionApiContract-ReviewSafety-2.md) reported GO.**
Acceptance does not authorize source implementation, repository creation,
dependency change or release activation. See [verdict](GwzConnectionApiContract-Verdict.md).

Authority: the operator requested a concrete API contract and review addressing
the current churn, after the accepted
[Connection remediation plan](GwzConnectionRemediationPlan.md). The
[audit](GwzAntiPatternRemediationPlan.md) supplies evidence, not additional scope.
This contract selects a design for review; adoption amends only the clauses named
in §15. Until adoption, existing canonical requirements remain authoritative.

## 1. Outcome and boundary

One long-lived communication owner serves independent operations. Successive
operations can reuse eligible physical connections without sharing their mux,
bootstrap, cancellation, request results, credentials or retry state. Correct
construction is mandatory; attaching the right context later is not supported.

The independently testable library is Git-independent. It handles reusable SSH
and HTTPS communication through ports; HTTP compatibility is explicit below.
gwz-git owns single-repository Git behavior. Core owns workspace coordination
and application taut encoding. Existing gwz-transport remains the pure discrete
message stream/pool kernel, without physical I/O or a new wire carrier.

Editable now: this contract and its review records in root dev-docs. Potential
later changes: a communication crate, gwz-transport pool owner APIs, gwz-git and
core adapters, CLI/Python construction and relevant native launch integration.
git2-rs, libgit2 and other dependencies are reference-only. A failed preferred
implementation does not authorize an upgrade or a larger session programme.

## 2. Decisions that stop the recurring ownership churn

| Decision | Fixed by this contract | Mistake prevented |
| --- | --- | --- |
| D1 | Share endpoint instances; create independent operation bindings | Keeping one long-lived application mux and rolling it over to recover reuse |
| D2 | Separate HostOwner, HostHandle and Operation capabilities | An operation or clone destroying another operation's pool |
| D3 | Supply ports and operation inputs at construction/admission | Hidden private pools, native fallback and late environment recapture |
| D4 | Resource ownership survives timeout until disposal is confirmed | Reporting cancellation as cleanup or reusing occupied capacity |
| D5 | Admission precedes worker creation; terminal results are caller-owned | Eight active operations hiding unlimited queued threads/results |
| D6 | No identifier reuse within a host; finite per-binding state | A host-wide 256-registration exhaustion/rollover programme |
| D7 | Contract tests and real-consumer tests are different required gates | An internal reuse fixture being presented as actual Python reuse |
| D8 | One composition/migration gate for each capability | Publishing another unused library while implementation stays in core |

No replacement application session host, server, daemon, physical byte framing,
iroh adapter or new credential-management framework is required by D1–D8.

## 3. Package placement and permitted edges

**Selected proposal:** crate `gwz-connection` at
`gwz-transport/crates/gwz-connection/`, in that repository's Cargo workspace.
The existing root gwz-transport package retains its no-I/O/no-dependency charter.
The new crate coordinates injected ports; it does not implement sockets, native
authentication, helper spawning or a runtime. Fast tests use its own fakes and
gwz-transport, never core, Git or an installed helper.

| Package | Permitted first-party production dependencies / role |
| --- | --- |
| gwz-transport | None; pure stream/mux/sequence/pool mechanics |
| gwz-connection | gwz-transport only; integration through ports it owns |
| gwz-git | gwz-connection and the qualified Git bindings; Git implementation |
| gwz-core | gwz-git and gwz-connection; composition/adapters/application protocol |
| CLI / Python | Supported core entry; retain an explicitly supplied HostHandle |

Native ports initially remain small core adapters over existing endpoint code,
including gwz-sspi. They may depend on the interfaces they implement; no reverse
dependency from Connection into core is allowed. A later extraction of native
adapters requires its own demonstrated need. The API is independent despite
their initial location; it must never import them for fast tests.

A separate repository would add publication/pinning before proving consumption.
A core-hosted crate would preserve the current temptation to reach core internals.
Neither improves the small proof. No new member repository or extra contract
crate is selected. Actual workspace/dependency changes await implementation
authorization. Use explicit package metadata, no parent-package inheritance.

## 4. Public capabilities and signatures

The following Rust-shaped signatures are normative about ownership, cancellation
and outcomes. They are a design surface, not compiling implementation code.
All constructors validate finite limits; opaque values have private fields.

```rust,ignore
pub fn build(config: HostConfig, ports: HostPorts)
    -> Result<(HostOwner, HostHandle), BuildError>;

pub fn cleanup_domain(config: DomainConfig, retention: RetentionPort)
    -> Result<(CleanupDomainOwner, CleanupDomain), BuildError>;
impl CleanupDomainOwner {
    pub fn close(self) -> DomainClose;
}

impl HostHandle {
    pub fn admit(&self, input: PreparedOperation)
        -> Result<Admission, AdmissionError>;
    pub fn snapshot(&self) -> HostSnapshot;
}
impl Admission {
    pub async fn acquire(self) -> Result<AdmissionPermit, AdmissionError>;
}
impl AdmissionPermit {
    pub fn register(self) -> Operation;
}
impl Operation {
    pub fn cancel_handle(&self) -> OperationCancel;
    pub fn open(&self, spec: ExchangeSpec) -> Result<Opening, OpenError>;
    pub fn finish(self) -> OperationFinish;
}
impl OperationCancel {
    pub fn cancel(&self) -> CancelState;
}
impl Opening {
    pub async fn wait(self) -> Result<Exchange, OpenFailure>;
}
impl Exchange {
    pub fn stream(&mut self) -> &mut MessageStream;
    pub async fn response_head(&mut self) -> Result<HttpResponseHead, StreamError>;
    pub fn finish(self) -> ExchangeFinish;
    pub fn abort(self);
}
impl HostOwner {
    pub fn close(self) -> HostClose;
}
```

`Admission`, `Opening`, `OperationFinish`, `ExchangeFinish` and `HostClose` are
owned futures/awaitable values. `OperationFinish` yields `OperationEnd`,
`ExchangeFinish` yields `ExchangeEnd`, and `HostClose` yields `CleanupStatus`.
`DomainClose` also yields `CleanupStatus`. CleanupDomain is a cloneable submission
capability with no close/reset method; its owner is Send, not Clone.
Construction of finish/close objects performs the sealing transition immediately,
before polling; an unpolled future cannot leave admission open.

| Type | Capability and data ownership |
| --- | --- |
| HostOwner | Send, not Clone; only explicit host shutdown authority |
| HostHandle | Send + Sync + Clone; submission/observation only, no shutdown |
| Admission / AdmissionPermit | Send, not Clone; cancellable reservation, then one registration right |
| Operation | Send, not Clone; owner of one binding and its terminal receipt |
| OperationCancel | Send + Sync + Clone; cancel one operation only, weak routing token |
| Exchange / MessageStream | Send, not Clone; one exclusive stream/lease; serialized reads/writes |
| CleanupTicket | Cloneable constant-size status handle; no release authority |

Capability handles have no Default. Plain limit values may provide the stated
defaults. There is no ambient host lookup, mutable singleton, post-construction
`with_host_context`, or public constructor for Operation/Exchange. There is no
public `return_healthy(bool)`; only validated adapter settlement can pool a lease.
An Operation may create multiple exchanges concurrently within its limits.
There is no cross-operation serialization gate around the application call.

### 4.1 Port boundary

`HostPorts` is exactly four narrow capabilities, supplied together:

- `Clock`: one origin, now/wake, suspend/discontinuity behavior (§10).
- `OwnerSeed`: nonsecret host-instance namespace, supplied from the composition
  root's identifier source; deterministic in tests. No global ID counter.
- `Progress`: executes the host's bounded driver work and timers independently
  of application calls, and reports driver termination. No network primitives.
- `EndpointFactory`: creates endpoint adapters from validated immutable inputs
  and a supplied `CleanupDomain`; factory errors have explicit cleanup status.

`CleanupDomain` is an explicit externally owned resource authority passed in
`HostConfig`, not manufactured by an operation or an endpoint factory. It owns
the finite setup/helper/retained-cleanup budgets used across this composition.
It outlives all hosts made on it, cannot be reset by making a replacement host,
and retains a failed host's cleanup until confirmed. The composition root owns
its final shutdown; Connection does not acquire unrelated workspace services.

Domain construction allocates accounting only, with no I/O. Its required
RetentionPort is the composition's existing supervised cleanup service (a stepped
fake in fast tests), which accepts a finite retained-work record before any
native effect is allowed. It keeps ownership/progress after host or domain-owner
drop until disposal is confirmed or the process exits. This is not an optional
detached task, a static ledger or permission to implement a general process library.
If retention cannot be reserved, work is refused before effects.

DomainOwner close/drop synchronously seals the domain and revokes every host on
it, including Reserved claims, before returning its awaitable. Existing host
close/drop and domain close are idempotent and use one overall five-second close
bound, not five seconds serially per host. At the bound, return Pending(ticket);
the RetentionPort still owns the generation. Dropping that ticket or the last
domain handle does not release physical resources/permits or stop the retainer.
The retainer never admits new work; it retires itself only after confirmed cleanup.
Closing another host on the same domain cannot clear the first host's pending
record. Native implementations must prove the retention lifetime separately.

The EndpointFactory returns an opaque `EndpointAdapter`. Its operations are
`connect`, `authorize_reuse`, `start_exchange`, `cancel_work`, `dispose` and
`poll_completion`, over owned typed commands/tokens. All completions include
the endpoint, binding, connection and work generation they belong to. Duplicate
or late completions never release another generation's reservation.

Adapters do I/O outside library state locks. They cannot choose pool policy,
install operation-wide capacity, mint host shutdown authority or transfer an
uncertain connection to idle. They must retain native work when a wait future
is dropped. A returned `Pending(ticket)` is not a successful disposal receipt.
Adapters remain trusted implementations; types alone cannot prove their I/O or
cleanup claims. Conformance fixtures and native qualification enforce that part.

## 5. Registration, cancellation and drop

`admit` validates the input and reserves one of the finite admission records.
It does not register an application/transport operation, start an OS worker,
invoke credentials or create a network connection. At most 64 records wait.
Exhaustion returns `QueueFull` synchronously; it never creates a waiting thread.
`Admission::acquire` fairly waits for the active slot and eligible endpoint
attachment within the admission bound. Dropping it releases all reservations.
Endpoint construction at this stage allocates metadata only: no network,
agent/trust/key lookup, helper or native authentication worker. Physical effects
can begin only on an Opening owned by a registered Operation.

`AdmissionPermit` owns preallocated control/registration storage and an atomic
registration claim. `register` is synchronous, non-cancellable, does no I/O,
invokes no port/user callback and performs no fallible allocation. It changes
Reserved -> Registered and publishes the binding under one state lock. The
claim is updated at that same linearization point. Its unwind/drop guard knows
which side of insertion occurred and cancels a registered binding; it must never
infer NotRegistered merely because a Rust call failed to return.

**Revocation of acquired permits.** Host close/drop, host-progress failure and
domain close/drop set the corresponding revoked state under the same authority
lock used by register. That transition covers both Reserved and Registered claims,
not just the live-operation table. It revokes Reserved claims and releases their
active/admission reservations exactly once; physical-effect-capable work cannot
be published from them. Settling captured inputs runs outside the lock, retaining
its charge/cleanup ownership until settlement is complete.

The infallible register signature is retained. Consuming a revoked permit returns
an already sealed **terminal Operation**, with `registration = NeverRegistered`
and a specific terminal reason (`HostClosing`, `HostFault` or `DomainClosing`).
It publishes no live binding, invokes no adapter and allocates nothing. `open`
on it synchronously returns that reason with `Effect::None`; finish returns the
same provenance and input-cleanup status. Drop is an exactly-once no-op for already
released reservations. OperationEnd for an actual insertion instead states
`Registered(binding_id)` even if shutdown immediately cancels it.

The two legal race outcomes are: registration wins and is included in revocation's
registered cleanup accounting; or revocation wins and register returns the terminal
NeverRegistered operation. Close need not be polled, and dropping its future does
not reopen this decision. No late insertion after a shutdown cancellation sweep.
The domain authority mutex guards domain/host phases and claim phases. Payload
locks, if needed, follow that mutex; never acquire them in reverse order. No
await, port call, provider destructor or blocking native call runs under either.

OOM/abort is not a recoverable registration result. Internal unwinds are caught
at the host driver/caller adapter boundary; no Rust unwind crosses Python/C.
The claim and pre-reserved cancellation slot allow cleanup without allocation
or calling a port while unwinding. Unexpected driver death faults its endpoint
ownership domain, with honest pending cleanup; it cannot fabricate completion.

| Event | State transition / permitted effect |
| --- | --- |
| Admission/permit dropped before register | Release reservations; no registration/network effect |
| OperationCancel::cancel | Mark that operation sealed/cancelled, wake its waiters, schedule owned cleanup; idempotent |
| Operation::finish | Seal new opens, settle remaining exchanges, detach after terminal/cleanup accounting; no host close |
| Operation dropped or finish future dropped | Abandon/cancel that binding using its reserved control slot |
| Opening dropped before completion | Cancel that opening only; any late physical success is disposed or separately verified, never leaked |
| Exchange dropped / abort | Discard its lease unless adapter proves protocol cleanup; no automatic healthy return |
| HostOwner close/drop | Seal host admission and cancel its operations; drive bounded disposal under CleanupDomain |
| HostHandle or cancellation clone dropped | No host shutdown and no retained terminal-result entry |

Every terminal receipt states both operation/exchange outcome and cleanup.
`CleanupStatus = Complete | Pending(CleanupTicket)`; the ticket exposes
`status()` and `wait_until(deadline)`, not `release()`. On the five-second cleanup
bound, return Pending and retain physical permits/resources in CleanupDomain.
Ticket drop does not release those permits. Confirmation updates outstanding
tickets and releases exactly the matching work generation once.

Cleaned terminal operation records leave the host table once their receipt is
transferred/dropped. The host retains only bounded pending-cleanup records and
fixed-size cumulative counters. It is not an application result store. Python's
existing completed-result store must receive its own finite ownership policy at
the caller integration gate; this API does not magically repair it.

## 6. State owners, identifiers and message delivery

| Scope | State owned |
| --- | --- |
| Composition/CleanupDomain | Resource budgets, retained disposal, progress-task ownership |
| Connection host | Endpoint registry, host IDs, admission, policy and counters |
| Endpoint instance | Immutable physical configuration, physical owners/pools, key registry |
| Operation binding | Own bootstrap/mux, request IDs, cancellation, auth context, observations and retry state |
| Exchange | Exclusive lease, stream protocol state, byte accounting and cleanup |

Endpoint instances are immutable after construction. Selection/attachment versus
retirement is one registry transition. No binding can attach to Retiring/Faulted.
Identical configuration constructions coalesce; a failed construction publishes
no usable instance. Configuration mismatch chooses another instance, never a
pool mutation. Cancelling the first waiter does not cancel other waiters' work;
construction is owned by the host under a fixed construction bound.

The host allocates checked monotonically increasing binding/connection/work
numbers under its OwnerSeed namespace. Exhaustion returns `IdentifierExhausted`
before an effect; no wrap, reset or global counter. Each operation gets a fresh
transport binding/session ID. Existing 256-request mux and 4,096-sequenced-stream
lifetime limits remain local to that binding and return typed `BindingLimit`;
they never become a host-lifetime registration bound. No host rollover waits
for other operations or tears down idle connections. At least 10,000 successive
operations are required in the small proof. Expanding within-operation limits
is separate from this cross-operation repair.

Routing uses `(host, binding, exchange, work-generation)` internally; application
request IDs alone are never keys into shared tables. The carrier adapter attaches
the known binding capability to incoming messages before dispatch. Retired binding
IDs are never reused; messages for absent/retired bindings are ignored without
creating records. Current-binding invalid messages fault that binding only.
Per-binding tombstones remain until its queues settle, then the whole binding
can retire; a host-wide forever-growing tombstone set is forbidden.

Discrete-message ordering is enforced by the neutral core of gwz-transport's
sequenced profile (§6.1), not by the scheduler. Bound adapters feed that core before applying
messages to a stream. Opens for independent streams may pass a stalled Open;
data/control ordering for each stream obeys that protocol's terminal rules.
Abortive terminal messages fence later work; orderly close cannot silently skip
already accepted data. Reorder reserve, gap timeout and stale byte counters remain
kernel rules. Control/terminal progress has reserved queue capacity.

The current profile-3 kernel is non-default and explicitly unreviewed in source.
This contract does not call it production-qualified or turn on its feature.
Its focused conformance/ordering gate is part of the small proof, before an
adapter claims this contract. It requires neither a new physical carrier nor
the application session programme. Failed conformance is reported as a concrete
kernel blocker, not repaired by imposing host-scheduler ordering.

`Progress` runs stream delivery and cleanup independently of an application
request's completion and of Python awaiting that request. In-process Rust delivery
uses the same binding ports as the current messages embedded in core/CLI/Python.
No Python callback is needed merely to advance the local Rust transport pump.
If a Python delivery adapter is used, its lifecycle service must keep forwarding
all stream traffic until that binding's terminal cleanup, independent of app calls.

### 6.1 Neutral start and ordered-stream seam

**Selected change:** factor the current profile-3 ordering algorithm from its
Git-envelope wrapper; add a typed, in-memory start entry and an ordered-event
stream entry in gwz-transport. Do not reuse a dummy GitService or rewrite a
version-3 envelope as version 2. This changes pure-kernel interfaces, not a wire
schema, and is an explicit small-proof implementation item, not existing code.

The neutral `StreamOrder` API has these operations:

| Entry/output | Required state and authority |
| --- | --- |
| `reserve(lane, limits) -> PendingLane` | Lane includes binding-generation and stream ID; reserve finite reorder/control storage; no network or start authority |
| `start(pending_lane, direction, start_ticket) -> OrderReport` | Initial sequence 0 in that direction; ticket names one prevalidated, bounded opening record owned by Connection or the profile adapter |
| `submit(lane, direction, seq, event) -> OrderReport` | Existing data/window/half-close/terminal body values; no Git Open or serializer; data may be buffered awaiting that direction's start |
| `apply_ready(lane, callback) -> OrderReport` | Callback borrows a lane-bound CandidateEvent synchronously; actual state mutation succeeds before sequence advancement or Applied receipt |
| `resume(lane)` / `tick(now)` | Retry a blocked apply callback / enforce the existing gap and retirement bounds; no sequence reset |

`OrderReport` uses the existing Buffered/Applied/Superseded/AlreadyResolved
distinctions; it is not an effect or cleanup acknowledgment. Start metadata stays
in a bounded owner table, charged before the ticket is made; the order kernel
keeps its constant-size ticket only. A stale/foreign ticket is rejected by the
binding authority before an apply can cause effects. Raw message/request IDs
cannot mint tickets, and arbitrary generic callers cannot open physical resources
by constructing a kernel alone.

**Application commit boundary, preserved from the existing kernel.** StreamOrder
retains the event and its charge while it is eligible. It invokes
`callback(&CandidateEvent) -> Result<(), ApplyError>` synchronously under that
lane's exclusive application guard. CandidateEvent is a borrowed, non-Clone
capability scoped to that call; it cannot be taken, queued, spawned, awaited or
applied later. The callback performs the actual bounded byte/control transition,
not merely a readiness test or a handoff to another worker.

Only callback success advances the expected sequence and produces an Applied
receipt. That receipt contains identifiers/progress only, never an event that
still needs application. A successor or orderly terminal cannot enter its apply
callback or become Applied ahead of an uncommitted predecessor. `WouldBlock`
must mutate nothing: the event/charge remain kernel-owned at the same sequence,
and later resume retries it once. No callback blocks, awaits, does I/O or invokes
foreign code; bounded queue insertion for Start and in-memory byte transitions
are the only applications. Other lanes progress after a WouldBlock; no host
scheduler ordering requirement substitutes for this per-lane guard.

Cancellation/abortive fencing is serialized by that same lane guard. Either the
current byte transition commits before the fence, or the fence supersedes the
uncommitted event; no borrowed candidate survives to apply afterward. A rejected
byte transition or callback unwind faults/fences the lane without publishing
Applied or retrying potentially mutated state. Partial mutation cannot be hidden
by returning WouldBlock. Physical/native cleanup still belongs to its owner.

Each direction has its own sequence: RequestStart is initiator 0, ReplyStart
(opened or rejected) is endpoint 0, subsequent events begin at 1. Outbound assigns
the sequence exactly once when enqueue succeeds; retrying WouldBlock does not
consume a number. The callback for an applied RequestStart may enqueue a connector
job, only while its operation claim is still live. It returns WouldBlock while
its bounded work queue is full; that lane retains its position, while another
lane can apply its own Start. Applying Start does not mean connection setup has
completed. ReplyStart success is applied only after physical open/authorization;
until then Opening yields no Exchange. Failed/abortive starts fence buffered
later traffic and retain actual cleanup accounting.

`stream::OrderedStreamMachine::new(lane, limits)` is the selected byte-state
entry. It has **no wire profile/version field**. Its `apply(&CandidateEvent)`
runs only within that lane's StreamOrder callback and returns success only after
the byte-state mutation. The transition implementation
reuses the current byte-offset, receive-window, EOF and buffer logic after
sequencing. It does not call today's profile-1/2 `incoming(Envelope)` path.
ReplyStart creates/activates that byte machine before buffered Data is drained;
data is not applied while physical open is pending. Invalid offsets/credit still
fail even if sequence admission succeeded. Stream close/error produces its normal
progress/terminal receipt; only the physical adapter can confirm lease cleanup.
In particular, the data apply callback returns WouldBlock while its ordered byte
machine is not ready, with no byte or ordering mutation.
ReplyStart or cancellation resumes/resolves that lane using the reserved control
capacity. There is no take_applied queue or owned event handoff after an Applied
receipt. Failed byte validation faults the lane; it never retries an already
mutated event as new data.

The existing profile-3 `Inbound`/`Outbound` remain taut-envelope adapters over
the same StreamOrder. They validate the existing authored Open/CheckIdentity and
its Git metadata first, retain it in their bounded profile table, and map that
control to a start ticket at sequence 0. Existing envelope `message_seq`, binding,
direction and generation map one-to-one; other bodies become neutral events.
Profile version 3 is checked at this wrapper only. The private typed event has
no re-encoded version. Identity-check-only lanes retain their existing terminal
grammar and never instantiate a byte stream. Existing profile-1/2 interfaces
remain unchanged compatibility paths; they cannot masquerade as this ordered API.

Generic in-process SshExec and HTTP callers use Connection's bounded typed opening
table and tickets directly, with no GitService, Open envelope or new serialized
control format. Start, typed body delivery and reply enter the same order core;
the supplied local message adapter may interleave delivery but cannot bypass it.
For GWZ Git traffic, gwz-git/core's profile adapter performs the service mapping
and retains the existing envelope encoding. Any legacy raw HTTP prelude/body
conversion belongs in that profile adapter, not in generic MessageStream or
its callers. An arbitrary generic remote-open format is still not being added.

**Non-Git proof traces:** (a) register an Operation, reserve a lane, enqueue
RequestStart for SshExec `printf probe`, apply it to a fake connector, apply its
ReplyStart, then receive Data sequence 2 before 1. Only application of 1 permits
1/2 to reach OrderedStreamMachine; orderly terminal 3 follows both, adapter
cleanup returns the lease. (b) perform a generic HTTP GET with status/head from
the fake HTTP adapter and body events through the same lane; stall RequestStart
on another lane and verify the HTTP lane completes. A body-before-ReplyStart is
buffered; failed ReplyStart/cancel supersedes it under the existing terminal rules.
Neither trace names GitService, changes a wire version or trusts scheduler order.
Also pause sequence 1 at eligibility by a no-effect WouldBlock, then submit
sequence 2 and orderly terminal 3: neither becomes Applied until 1's callback
commits. An unrelated lane completes during the pause. Repeat with cancellation
before eligibility and immediately after byte commit: only the committed prefix
survives, with no post-fence application. These tests use the real callback seam,
not a test runner that happens to consume queued tokens in sequence.

This seam does not claim the current kernel already exposes these methods.
Implementation must factor one ordering/terminal algorithm, not copy it into
Connection. Run both existing profile-3 vectors and the generic traces against
that core; mismatched terminal behavior is a failed conformance gate.

## 7. Configuration, identity and authentication inputs

`PreparedOperation` is immutable and contains:

- an explicit EndpointRoute and validated EndpointProfile;
- OperationPolicy (capacity, timeout and retry inputs);
- an opaque, bounded AuthContext capability captured on the original caller entry;
- operation-local observation sink/cancellation scope. No other application state.

Core captures environment, cwd, caller identity and machine proxy once on the
original entry thread, before fan-out/GIL detach, using existing platform rules.
Adapters obtain only the views/capabilities they need. Neither library nor
subordinate backend calls `env::vars`, reads ambient cwd or captures worker-thread
identity. An invalid profile fails before endpoint/network effects.

The EndpointProfile equality definition is the reuse design §3 as amended by the
current Windows, trust and helper decisions: home/known_hosts location, selected
agent source, actual CA/trust content and policy, proxy/bypass, timeout domains
and adapter build/configuration version. Equality compares canonical nonsecret
bytes after a digest index hit; a digest collision is not equality. Do not log
or serialize this profile. Paths/agent addresses are sensitive metadata.

Operation-local capacity, repository paths, helper environment, account tokens
and per-open identity selection are not profile fields. Their compatibility is
checked by the lease/auth rules. Credentials must not be smuggled into proxy URLs
or configuration equality keys. Existing unsupported credential/proxy policies
remain typed refusals; a new auth framework is not introduced.

Ports and owners:

| Port | Inputs / outputs | Secret and lifetime rule |
| --- | --- | --- |
| SSH identity/trust adapter | AuthContext, exact identity choice, accepted public key/host key; authorization receipt | File key material/agent signing stays in existing endpoint adapter; explicit selection never falls back |
| HTTP credential adapter | Per-request origin/challenge and captured context; opaque request-scoped authorization | No token cache in host or account inherited from a TLS pool |
| TLS/trust adapter | Profile trust inputs, final origin and connection generation; validated channel binding | TLS trust and hostname verification precede credentials; CBT is actual final peer binding |
| Native caller/SSPI adapter | Original-entry capture, fixed native deadline, origin/TLS binding | Reuse contained gwz-sspi worker and its cleanup records; no worker-thread recapture |

The namespace exposes no environment snapshot, private-key/token field or OS
handle. AuthContext references a provider-owned bounded object; the provider
must report a maximum retained charge, checked against the admission resource
budget before acceptance. Core keeps its full snapshot; endpoint/SSPI exceptions
retain their existing secret handling. A retained cleanup record keeps only the
native resources necessary to settle, not a completed operation's snapshot.
No separate authentication repository is selected. Ports are trusted code and
must pass credential redaction/retention conformance; opaque does not mean safe.

### 7.1 SSH reuse authorization

Pool grouping is scheme, configured username/host and effective port, never
repository. Ambient and explicit identity pools are incompatible. An explicit
key is re-read before every lookup; a token identifies exact bytes, not a path.

For a connection last used by another binding, enforce reuse design §4: fresh
agent possession signature over the domain-separated nonce for its recorded
authenticating key; explicit-key equality; and fresh known_hosts validation for
the recorded host key. Once-per-binding proof caches contain public evidence,
not credentials. A setup performed by this binding supplies its own proof.
Supported key/signature classes follow later TR2.8 rulings, not the older design's
reversed security-key/certificate fallback sentence.

Concurrent waiters in a binding share the proof job. Each retains its own timeout;
expiry of one waiter does not cancel other waiters. Only binding cancellation
cancels the shared job. The job follows the accepted latest-waiting-request budget
rule within the existing finite identity-check ceiling; a request's own deadline
is never extended by joining. These proof deadlines are not SSPI/HTTP deadlines.

Negative possession/trust proof retires that connection and matching idle entries,
then permits a fresh setup within the requesting exchange's existing budgets.
Cancelled/timed-out proof returns a still-clean leased connection to idle before
binding cancellation; abandonment closes its lease. No claim of instantaneous
revocation on continuously busy SSH sessions is made. The ten-minute maximum
connection age is checked at release, not by killing an active exchange.

### 7.2 HTTP and connection-bound authentication

Generic HTTP mechanics live in the adapter: framing, TLS, keep-alive, challenged
authentication, origin checks and exclusive transaction settlement. Git service
paths/content types and result interpretation stay in gwz-git. The generic API
does not expose GitService, git2 types or core errors.

`ExchangeSpec` is `SshExec { target, command, identity }` or
`Http { origin, method, target, headers, auth }`. Fields are bounded owned values;
commands are remote SSH exec payloads, not local shell invocations. HTTP headers
reject controls and caller-supplied authorization/proxy-authorization; credentials
come only through the selected provider. HTTP exchange input/output are streaming
bodies with separate response status/headers, not caller-written HTTP framing.
`response_head` awaits that bounded owned status/header record exactly once;
another call returns `WrongState`. On an SSH exchange it returns `WrongScheme`.
Its drop cancels only that wait, not the exchange, and preserves the response
for a later call. `MessageStream` carries HTTP body bytes; finish cannot pool a
connection with an unread/unsettled response body merely because its headers arrived.

For request-scoped Basic/gh authentication, TLS can be reused across requests
without inheriting account or authenticated observations. NTLM/Negotiate must
retain the accepted connection/caller/origin scope: an in-progress or authenticated
connection is not a generic account-free TLS lease. The adapter returns an opaque
security-scope tag bound to original caller identity and generation; only equal
scope may reuse it, or it is disposed as the current native contract requires.
No cross-account downgrade to generic reuse is allowed.

Preserve the current approved mechanism matrix: anonymous and configured helper
paths where allowed, Windows Negotiate/NTLM under their caller contract, Digest
refusal, challenged POST refusal and the explicit proxy limitations. Redirects
or challenge retries are requested by the existing protocol profile/policy with
effect/deadline accounting; this crate never guesses Git discovery idempotence.

The API can represent generic HTTP, but the GWZ integration keeps the accepted
plain-HTTP native/local-only route. Selecting a remote endpoint for an unsupported
scheme refuses before effects, never silently falls back to local networking.
Adding generic remote plain HTTP is not part of this contract adoption.

### 7.3 Native process-safety gate

A NativeAdapter must supply a qualified ProcessSafety capability covering the
actual process launch behavior it uses. Two independently created hosts cannot
manufacture independent gates and claim exclusion over one process. All libraries'
spawns potentially inheriting the creation-window handles must be covered, or
the unsafe inheritable window must be eliminated. A parent mutex that only known
GWZ callers voluntarily take is not proof about arbitrary third-party spawns.

This contract deliberately chooses **fail closed, no unproved native capability**:
if a launcher cannot demonstrate that scope, native authentication is unavailable
on that adapter and refuses before credentials/effects. This is an implementation
qualification blocker for promised Windows parity, not permission to ship a
reduced release or silently take another identity. Existing SSPI mechanics are
reference implementations, not declared process-safe by this API review.
The native remedy is selected by a focused feasibility/qualification package;
it is not a prerequisite to the portable fake-owner proof and creates no general
process-library programme. Cross-client launch and unrelated-spawn tests must
reproduce the audited race before claiming its closure.

## 8. Pool, lease and fault contract

Idle entries have no operation owner; last-lessee metadata is separate. Acquiring
an idle entry or opening a new one charges that operation's limits. Opening,
idle, leased and closing physical entries retain the shared reservation until
actual disposal; cancellation never manufactures space.

Use reuse design §5's no-install capacity semantics: each operation's total is
max(256, jobs), max_requests is max(1024, jobs), and its per-host/user-host limits
come from its resolved max-per-host. The shared physical total/host ceilings
remain the accepted provisional 256, taking max(ceiling, requesting limit) for
new physical allocations. This preserves the no-clamp rule and existing fan-out
meaning. No TransportCapacityConflict or live pool reinstallation is permitted.

Fairness is FIFO among eligible requests; an exhausted per-operation limit cannot
hold the shared queue head against other eligible owners. When idle entries fill
a shared ceiling, retire the oldest eligible idle entry across SSH/HTTPS and wait
for disposal. A raced-to-leased entry is a no-op and selection retries within the
same allocation bound. Never close another operation's active exchange to resize.

One connection serves at most one exchange. `Exchange::finish` returns it to idle
only after the native protocol confirms complete request/response cleanup and
reusability. A dropped/unread HTTP body, failed SSH channel teardown, unknown
write effect or pending native work discards the lease. An adapter's explicit
NoNetworkEffect receipt may release an unchanged idle connection after a failed
pre-exchange check. This receipt cannot be constructed by application callers.

| Failure | Scope / recovery |
| --- | --- |
| Opening/stream protocol error or operation cancellation | That binding and its leases, not another binding's idle connections |
| One physical connection fails | Exchanges on it fail; discard; unrelated connections remain usable |
| Endpoint cleanup fault with physical owner still alive | Stop selecting instance; existing exchanges finish; retire under retained owner |
| Endpoint worker/driver dies or panics | Fault that instance, fail its active exchanges; no reuse; later operation can create another within retained-cleanup budget |
| Host progress driver dies | Seal this host, fail its bindings, retain endpoint disposal; no fabricated Complete |

Other operations are isolated from an operation's logical cancellation, not
immune to shared physical-owner failure. CleanupDomain exhaustion refuses new
work before effects and names the exhausted budget; recreating a host on that
domain does not bypass retained work.

## 9. Stream and replay contract

`MessageStream` provides asynchronous `read(&mut [u8])`, `write(&[u8])`,
`write_all(&[u8])`, `flush()` and `shutdown_write()`, using the existing
gwz-transport data/credit/terminal semantics. `write` may accept a nonzero prefix;
`write_all` continues until all input is accepted or an error reports progress.
Zero-length buffers are no-ops. A clean read EOF requires validated end-of-stream;
cancel, timeout, protocol failure and carrier loss are errors, never invented EOF.
`MessageStream::progress()` returns fixed-size accepted-write, consumed-read and
peer-confirmed offsets. StreamError carries the failure kind and that progress;
a dropped write_all future must be resolved by inspecting progress, not by
blindly resending its original buffer. Peer-confirmed bytes are not a remote
application success acknowledgment. Read cancellation before delivery consumes
no bytes; write cancellation may have accepted a prefix and cannot retract it.

The 100 ms coalescing ceiling and immediate full-buffer/flush behavior remain;
read size and write chunk size do not affect bytes reconstructed. Credit is
returned on consumption, not arrival. No caller or host scheduler supplies the
per-stream ordering guarantee; the sequenced message adapter does (§6).

The public Connection API does not export gwz-transport's Git-specific Open type.
Existing GWZ envelopes remain a transport profile: core decodes/encodes them,
gwz-git maps service semantics to generic ExchangeSpec, and Connection receives
typed Rust inputs. Generic in-process callers use the same API directly. No
alternate serialized Open format or hand-coded shadow protocol is introduced.
Any future generic remote-open schema must be taut-authored and separately
reviewed; it is not necessary to prove the current generic in-process API.

Before an SSH exec is submitted, or an HTTP request is handed to the HTTP engine,
a stale idle connection may be replaced within the original budgets. After that
boundary the effect is `PossiblySent` even if no byte acknowledgment arrived;
Connection cannot replay. `Effect = None | PossiblySent | ResponseStarted` is
included in OpenFailure/ExchangeEnd. Typed Git success remains gwz-git's result.
Adaptive/protocol-approved retry decisions belong to the operation's existing
policy adapter and require explicit evidence; no generic retry of a mutating body.

## 10. Clocks and finite defaults

One Clock origin supplies all time comparisons within a host. It includes sleep
where available; otherwise a wall/monotonic discrepancy above the idle interval
expires idle entries and recomputes active deadlines per the accepted clock
rules. Read Clock outside state locks and pass the captured time into transitions.

| Quantity | Initial value / rule | Owner |
| --- | --- | --- |
| Active operation slots / waiting admissions | 8 / 64; finite host defaults | Host |
| Endpoint instances | 16; coalesce equal profiles, LRU retire unbound | Host |
| Setup jobs / retained cleanup owners / helper slots | 64 / 64 / 8, explicit CleanupDomain | Composition |
| Admission / allocation / cleanup | 5 s / 30 s / 5 s | Reservation / opening / retiring owner |
| Idle expiry / maximum connection age | 60 s from release / 10 min checked at release | Endpoint |
| Positive default network stall / setup aggregate | Resolved policy, initially 9 s / 30 s | Operation/opening |
| Interaction allowance | 120 s, cumulative; distinct from network time | Opening |
| Stream payload / buffers / queues | Negotiated gwz-transport limits; never enlarged implicitly | Binding/stream |
| Observation retention | 64 fixed-size diagnostic entries; counters saturate with overflow flag | Host |
| AuthContext retained charge | At most 16 MiB per admission, 64 MiB total per host including waiting/cleanup records | Host/provider |
| Profile/HTTP metadata | Existing negotiated metadata limits; public CA input retains current platform limits | Admission/adapter |

These internal defaults derive from the accepted reuse/setup contracts except
the diagnostic ring and retained-charge caps, explicit bounded implementation
choices for the new API. Charge exhaustion returns `InputBudget` before
registration; it does not silently omit environment/credential inputs. Shared
provider objects use one actual ownership charge while retained, not zero because
an Arc was cloned. Input limits
validate at construction; they are not new CLI flags. Raw frame/metadata limits
are checked before allocations using the existing kernel admission rules.

`NetworkTimeout = Disabled | Enabled { stall, aggregate }` prevents confusing
zero with zero remaining. Zero disables network stall and aggregate together
on routes currently supporting it; it never disables allocation, interaction,
cancellation, cleanup, idle reaping or admission. Native SSPI at network-timeout
zero refuses before credential publication, preserving its finite-deadline rule.

Never reset a deadline because of a poll, a 401, a redirect, pool reuse, a new
physical generation or another native round. Genuine completed network attempts
renew only the stall as the existing SetupClock permits; the aggregate does not
renew. Preserve only explicitly authorized charging transitions: setup interaction
pauses under SetupClock, and D19's helper-Basic pause. An adaptive requeue is a
new policy-authorized attempt with its own D and retained auth rounds, not an
unlabelled extension. Native call cutoffs are minted from the remaining budget
once for that conversation. The adapter cannot invent another 30-second fallback.
Every pause/requeue is a typed budget transition tested under an injected clock;
central policy owns it, rather than arbitrary callers editing deadline numbers.

## 11. Error, observation and shutdown surface

Admission errors are `InvalidInput`, `QueueFull`, `AdmissionTimeout`,
`CleanupCapacity`, `HostClosing`, `IdentifierExhausted`, `ForkedHost` and
`UnavailableAdapter`, plus `InputBudget` for the retained-charge checks performed
by admit. `BuildError` is `InvalidConfig(reason)`, `MissingPort`,
`UnavailableProgress` or `DomainClosing`; invalid charge/limit configuration is
`InvalidConfig(InputBudget)`, not a registered/opening failure.
Opening/stream errors add `BindingLimit`, `Unsupported`,
`AllocationTimeout`, `SetupStall`, `SetupAggregate`, `InteractionTimeout`,
`Authentication`, `Trust`, `Cancelled`, `Protocol`, `CarrierLost`, `Io` and
`EndpointFault`, with `InputBudget`, `WrongState` and `WrongScheme` for the
bounded-input/lifecycle violations specified above. Each retains origin/scope,
effect and cleanup as applicable.
Terminal permits additionally expose `HostFault`/`DomainClosing` on open as
specified in §5. An over-budget AuthContext must return
`AdmissionError::InputBudget` with no registration, endpoint construction or effect.
Dependency error code and class remain distinct diagnostic domains, not compared
as interchangeable integers. No unbounded native message or credential text is
automatically included in Display/Debug.

HostSnapshot/OperationEnd expose fixed-size counts: opening, idle, leased,
closing, retained cleanup, active/waiting bindings, created/reused/discarded
connections and exchange totals. Diagnostic IDs are opaque and host-scoped.
Per-operation rows use that operation's selection and auth facts only; no member
path, credential, URL or config from another operation is retained or reported.
The application consumes its own rows under backpressure; the host does not keep
all completed rows/results. Counters are observations, not proof of Git success.

HostOwner close revokes admission synchronously, cancels bindings, disposes
instances concurrently and reports Complete/Pending within the cleanup bound.
Unpolled or dropped close/finish futures leave their cleanup owned by the domain.
Progress termination is observable and cannot silently strand waiters forever.

After fork, inherited handles fail `ForkedHost` before touching locks or I/O.
The composition checks its process stamp on entry and does not drop/join/shutdown
inherited physical owners in the child. It forgets those resources there and
constructs a fresh owner; parent resources remain usable. This adapter obligation
needs a native fork fixture, not a claim from pure Rust types.

## 12. Real-consumer construction and first-use walkthrough

The composition root builds one owner and injects its handle. An in-process CLI
command owns it for the command; Python's explicit interpreter/module composition
object retains it across client operations and supplies handles to clients.
No static or thread-local hides that owner; no Client/backend constructor silently
manufactures a substitute. Direct Rust callers choose their owner lifetime.

```rust,ignore
let (domain_owner, domain) = cleanup_domain(domain_config, retained_supervisor)?;
let config = config.with_cleanup_domain(domain.clone()); // config data, not host patch-up
let (owner, host) = build(config, supplied_ports)?;
let a = host.admit(capture_on_original_entry(input_a)?)?.acquire().await?;
let op_a = a.register();
run_git_via_gwz_git(&op_a).await?;
let end_a = op_a.finish().await; // host and eligible idle connections survive
let b = host.admit(capture_on_original_entry(input_b)?)?.acquire().await?;
let op_b = b.register();
run_git_via_gwz_git(&op_b).await?;
let end_b = op_b.finish().await;
let cleanup = owner.close().await; // explicit composition shutdown
let domain_cleanup = domain_owner.close().await; // pending ownership remains supervised
```

`run_git_via_gwz_git` accepts the operation communication capability; it cannot
fall back by constructing a private host. Its synchronous libgit2 stream bridge
runs on a suitable worker and handles internal WouldBlock by waiting under the
same cancellation/budget contract. It does not report transient readiness as EOF
or assume that libgit2 callbacks support asynchronous suspension.

The example omits application error projection; production uses RAII on every
early return, so abandoned operations/owner still follow §5. Client close cancels
its own operation scopes; it has no HostOwner capability and does not close a
module-shared pool used by another Client. Module close owns shared shutdown.

For two hosts, clone the same CleanupDomain into both build configurations.
Close host A and keep its Pending(ticket_A), then close host B. DomainOwner close
now seals the composition and returns Complete or Pending(ticket_domain) within
one bound. If A remains pending, both tickets still name that same retained
generation; the RetentionPort owns it even if both tickets and every domain handle
are dropped. B's successful close does not release A's reservation. A late verified
disposal completes the corresponding ticket states and releases that generation
once. Test both explicit final close and final DomainOwner drop using a stepped
retainer; never rely on an application caller to continue polling a lost future.

## 13. Independent proof and migration gates

Implement only after review and authorization, in these complete packages:

1. **Portable owner proof:** build/admit/register/open/finish/close with fake
   EndpointFactory, Clock, Progress and CleanupDomain. Two operations must use
   one physical fake connection; Operation A finish must not close that host.
2. **Adversity/sustained proof:** the cases in §14, including actual sequenced
   profile conformance, 10,000 operations and no host-wide tombstone growth.
3. **Native adapter conformance:** adapt current SSH/HTTPS/SSPI mechanisms and
   validate identity/trust, cleanup, time and ProcessSafety claims separately.
4. **Production adoption:** core delegates single-repository communication to
   gwz-git using this API; CLI/Python execute actual operations across it. Count
   physical connection creation and cold/warm/sustained timings from installed
   artifacts. Retire replaced owner/construction paths in that same review package.

The first proof does not compile core/git2, access filesystem/network, spawn a
helper or require Windows. Native/platform matrices run for changed adapters,
then the final settled client composition. Do not requalify the whole application
for every pure state-machine edit. Existing mandatory lane/platform checks still
apply to changed source paths; this contract does not waive them.

## 14. Invariant and counterexample matrix

| Invariant / former churn root | Enforcement category | Required counterexample test |
| --- | --- | --- |
| A1/A7 owner cannot be optional or operation-owned | Private constructors and separate capabilities | Real Python A/B uses one host; dropping A leaves B and idle pool usable; unsupported private-constructor route does not compile |
| Registration provenance never inferred from a missing return | Preallocated claim + one state transition; unwind test | Panic at every pre/post insertion boundary, cancel before/after admission, verify no orphan/double settle |
| Reserved permits cannot revive a closed host | One authority transition and terminal NeverRegistered operation | Acquire P, construct unpolled close, register/open P; zero effects and exactly-once release. Repeat owner drop, driver failure, domain close/drop and both register-vs-close race orders |
| Operation fault differs from instance failure | Runtime state ownership + typed errors | A bad message closes only A; kill physical owner fails affected exchanges and subsequent operation rebuilds |
| A4 no waiter/thread growth before admission | Finite synchronous reservation; native adapter conformance | More than 8 active + 64 queued rejects before worker creation; drop queued futures; bound retained resources |
| A3/A5 terminal records and binding IDs are sustainable | Runtime accounting + caller policy | 10,000 operations with repeated external request IDs, delayed stale events and abandoned receipts; table size returns to bound |
| Lower capacity never mutates another operation's pool | Per-owner counters / shared authority | A at max-per-host 4 and B at 32 overlap; neither clamps/evicts the other's active work; cross-scheme idle eviction frees only after disposal |
| Cleanup is not cancellation acknowledgment | Domain-owned permits + qualified receipts | Late success/duplicate completion after cancellation, pending helper and host recreation; no double release or replacement beyond domain budget |
| Domain teardown cannot orphan retained work | Owner/handle split and mandatory RetentionPort | Two hosts share domain; A pending, B complete, domain closes/drops, all tickets drop, retainer still owns A until verified disposal |
| A8 one immutable input per operation | Required PreparedOperation and no ambient constructors | Change env/cwd/agent behind path after capture; only explicit permitted revalidation reads current trust/key |
| Authentication cannot leak across reused HTTPS connections | Typed security scope + native conformance | Two callers/accounts/origins, anonymous->Basic reuse, native connection-bound auth, redirects and CBT generation changes |
| Signed SSH proof is per operation/key and host trust is fresh | Provider contract + native tests | Agent withdrawal/replacement/confirmation refusal, changed key file and known_hosts; no eligible reused row on failure |
| A2 process safety cannot be per-host theater | Capability gate + native race fixture | Two hosts plus unrelated inheriting spawn during launch window; refuse unqualified path before credentials |
| Stream ordering is protocol-owned | Kernel sequencing + bounded reservations | Interleave streams, reorder data/close/fail, stall one Open; no byte corruption, ghost lease or cross-binding event |
| Generic opens do not invent Git metadata or a version cast | Neutral Start and synchronous CandidateEvent apply callback | Both §6.1 non-Git traces use actual factored kernel and no GitService; existing profile-3 vectors still pass; no direct v3-to-v2 input path |
| Applied means actual byte/control mutation, not event handoff | Exclusive lane callback and commit-after-success | Hold predecessor at WouldBlock; successor/terminal cannot commit; another lane progresses. Cancel before/after callback commit; no delayed post-fence application |
| Stream effect never claims Git success/replay safety | Typed effect boundary + Git integration | Disconnect after exec/HTTP submission and before ack; never replay mutating exchange |
| Budgets are centralized | Typed clock transitions + fake clock | 401/redirect/native rounds/reuse do not renew D; only named pause/new-attempt transitions; zero refuses native auth and still cleans up |
| Git boundary remains used | Production constructor/dependency guard | Real core entry calls gwz-git, no duplicate direct binding in migrated area; dependency update changes only implementation behind boundary |

Seeded Monte Carlo runs vary chunk sizes, reads, credit, legal interleavings,
fault timing and cleanup completion order. Every failure prints seed, limits,
event trace and the exact source tuple for replay. Adapters must obey the stated
delivery/reliability contract; random loss is not confused with valid reorder.
The matrix states what types enforce versus what trusted adapters/tests prove;
it does not claim that public traits make a malicious implementation safe.

## 15. Clause-specific amendments on adoption

This is the bounded amendment list, not a rewrite of all historical documents.
Canonical core paragraphs change only in the authorized implementation package
that introduces the boundary; this contract controls these points on acceptance.

| Existing authority | Replacement / retained rule |
| --- | --- |
| Core crate map §1: role edges limited to its listed contract/pure crates; §8 placement of all candidate transport crates in core | Add gwz-connection as an integration crate in gwz-transport repository, depending only on its pure kernel. Core supplies adapters; no dependency back into core. Other candidate crates are not automatically moved. |
| Library boundaries §2: all new core packages under core/crates or candidate-crates; narrow interfaces/no core dependency | The selected external communication package is the named placement exception; ownership/port/test rules remain. No exemption for arbitrary dependencies or generated core protocol types. |
| Reuse design §2 and §7 / core session composite: pool registry and process-wide Python host coupled to larger HostContext | Connection's explicit owner holds only communication registry/budgets; core/module composes it without member locks/workspace registry. Keep separate operation bindings and fork semantics. No requirement to implement full session/server host. |
| Reuse design §2 “three threads per instance” / one link thread per binding | Logical progress owners are fixed; thread/executor counts are adapter decisions constrained by finite budgets and independent delivery. No mandatory OS thread per queued operation or binding. |
| Reuse design §3 core snapshot/config fields and §6 gh-only environment wording | Snapshot stays in core; named adapters receive narrow immutable views/AuthContext. Apply later Windows/trust/helper amendments; do not revive obsolete gh-only/native HTTP assumptions. |
| Reuse design §§4–10: eligibility, signed proofs, capacity, idle/max-age, stale replacement, fault scope, retry and observations | Retain as expressly refined in §§6–11 here; per-request native security scope supplements account-free TLS reuse. No new credential revocation promise, generic replay or cap clamping. |
| Reuse design §2 detail: the registry, instance and binding tables, "Keys include the binding", "Limits stay per operation", "Threads" (refines the §2 rows above) | Superseded by §§4–6 here: the registry is owned by `HostOwner` and reached through `HostHandle` (§4); a binding is what a registered `Operation` owns (§5); shared tables route by `(host, binding, exchange, work-generation)` (§6). D1 and §2's rejected alternatives stay as rationale. |
| Reuse design §3 "Configuration ownership by selection" and "The agent's identity" | The profile is derived from `PreparedOperation`, captured once on the original caller entry (§7), not at the transport entry from the session's snapshot. The field table, "Excluded", "No secrets" and byte-equality-after-digest rules stay as §7 here states them, and per-field derivation stays the core adapter's rule. The agent is still selected by address and authorized by possession at lease (§7.1). |
| Reuse design §4 "Cross-operation revalidation" | §7.1 enforces it. §4 stays authoritative for what §7.1 does not restate: the SSHSIG possession proof (nonce, preamble, namespace, hash, confirm-key behaviour, the supervised job under the request's clocks), the explicit-key re-read, the known_hosts re-check, per-binding proofs and their cost, the outcome table and "Why retire". Replaced: "Key types" by later TR2.8 rulings (§7.1, last sentence); the HTTPS bullet "Nothing beyond configuration equality" by §7.2's per-request security scope for connection-bound authentication. |
| Reuse design §5 "Capacity and the shared authority" | §8 here restates the no-install capacity rule. §5 stays authoritative for the per-owner accounting, the instance ceilings and their provisional values, the list of capacity code that goes (`TransportCapacityConflict`, install and resize), and the reserve-before-connect and allocation-deadline rules; where §8 is silent, §5 applies. |
| Reuse design §7 "Statics" and CS §5.7: the setup-job cap, cleanup permits and `gh` slots move to the host context | Their owner is the externally owned `CleanupDomain` (§4.1), which outlives the hosts made on it, with the 64 / 64 / 8 budgets of §10. The rest of §7 (instance bound and disposal, stale idle replacement) stays; its clocks, idle expiry, maximum age, shutdown and fork rules are restated by §10 and §11 here with the same values. |
| Reuse design §11 "Client placement is not in this release (TRP §2). A `cli`-placed endpoint would belong to its command, never to the registry." | Replaced for the release sequenced by `GwzTransportReleaseConvergencePlan.md`: the `cli` placement ships, and its endpoint side is a `HostOwner` built by the client process's composition (§12), whose registry and idle connections serve its operations like any other host's. The server, stdio and SSH-remote rows of §11 and "The lazy endpoint retires" stay. |
| Placement design §2 and `docs/TransportPlacement.md`: the client endpoint supplied by the host application (`CliEndpoint::new(SshEndpointConfig)`, `install_cli`, `remove_cli`, `ClientRequest`; "one CLI endpoint connection supplied by the host") | At the `cli` placement the endpoint side is the client process's `HostOwner`, and the core process builds no endpoint with credentials for it. The placement design's capability, Bind/Bound, receiver-affinity, request-registration and identifier rules stay and apply per binding (§6). The carrier between the two sides is a composition adapter outside this library, delivering the same discrete messages through the binding ports (§6). |
| Placement design status and operator scope clarification of 2026-09-22 (in-process embedding only; no new command, physical framing, socket between CLI and core, or separate-process and process-kill qualification) | Superseded for the release sequenced by the convergence plan: the first form of the `cli` placement runs the endpoint side and the core side as two processes over a same-user local-socket carrier, with a command that serves the core side (plan Phase 2, D-D). The carrier and command are composition work outside `gwz-connection` and `gwz-transport`, so §1's "without physical I/O or a new wire carrier" and §2's last paragraph stand for the library. No network listener, reconnect or replay scheme, or credential-management feature is added; in-process embedding stays the default form. |
| gwz-git design/API: G0 is read-only and future network adaptation remains a later phase | Add Connection operation as the required communication capability for migrated remote behavior; preserve owned Git API values, not exposure of core/git2/OS types. G0 behavior remains compatible. |
| Current core/Python interim per-operation transport route | Replaced only when production consumer proof passes: module/command owner survives operation binding teardown; private per-operation host construction is removed from the supported pooled path. |
| Current unreviewed sequenced kernel feature status | Unchanged by document GO. A small-proof conformance result and implementation review are required before any production claim or feature activation. |
| Sequenced-stream design §§2–4: first Open/CheckIdentity, per-direction numbering, terminal application and port context; current profile-1/2 byte-machine entry | Factor a neutral StreamOrder start/ticket and synchronous CandidateEvent callback per §6.1. Git profile's authored first messages/sequence numbers stay unchanged in its wrapper; generic in-process starts are typed only. Add version-free OrderedStreamMachine applying inside that callback; never cast a v3 envelope to v2. Retain the existing atomic apply-before-Applied/sequence-advance rule, no-effect WouldBlock, terminal fences, reorder/queue bounds, delivery generations and profile-1/2 compatibility. |
| §7.3 "A parent mutex that only known GWZ callers voluntarily take is not proof"; fail closed | Recorded at step 1.3a (gwz-core `cc43e2c8`, merged `ee22a7d7`) under operator decision D-E of `GwzTransportReleaseConvergencePlan.md` §10: the Windows `ProcessSafety` is a named kernel semaphore (`Local\gwz-spawn-gate-<pid>`) shared by every gwz host in the process, made at one minting site (`transport_host::composition::process_safety()`, enforced by `scripts/checks/check_spawn_gate_minting.py`), accepted only when its owner SID is the process's default owner, and failing closed otherwise (`hold()`/`state()` report `SafetyUnavailable`; the native route refuses before any credential is read). The claim is scoped to gwz callers; the residual, a third-party spawn in the same process that does not take the gate, is recorded for the release notes (plan 6.4). §7.3's "not proof" sentence stands for that residual. |
| §9 (`ExchangeSpec` and the Git services); §4.1 (`EndpointAdapter` ports) | Recorded at step 1.3b (gwz-transport `70516c0e`, pushed; gwz-core `dc9de784` (was `fb26d4dc`, not an object; corrected 2026-10-31), merged `869531e6`, in lane p1-trunk): `ExchangeSpec::git(key, identity, service, path)` carries `GitService { UploadPackAdvertisement, UploadPackExchange, ReceivePackAdvertisement, ReceivePackExchange }` and a validated repository path (refused when empty, over 16 KiB, containing NUL or starting with `-`); `ExchangeSpec::new` is removed. Adapter ports: `start_exchange(connection, &ExchangeSpec)`; `ExchangeIo::poll_finish(cx)` replaces `finish`; `Completion::IdleClosed` reports an idle connection the adapter lost. The crate gains an executor-run driver (`pump.rs`, one task per host on the host's own `Progress`, stopped by the owner's close or the domain's revocation); `HostPorts` stays exactly four. Open for 1.3c/1.5/1.6 and the Phase 1 review: the §6.1 ordered seam is not yet the path stream data takes (a per-exchange in-process stream pair crosses `ExchangeIo`), `ExchangeIo` has no `shutdown_write`, adapter setups draw on the agent-job supervisor's budget rather than the `CleanupDomain`'s, and the pool's `network_deadline` is not carried in `ConnectCommand`. |
| §4.1 (`EndpointAdapter` ports) for HTTPS; §7.2 (HTTPS authentication scope) | Recorded at step 1.3c (gwz-core `c318376c`, "Add the HTTPS endpoint adapter and route the production EndpointFactory by scheme", 17 files, +2583/-59; merged `869531e6` on 2026-10-26 with `dc9de784` and `b7e9188b`; evidence `2026-10-25-p1-3c`). The commit subject does not name the step, and the root records the merge only as the opaque `2efae357` and `e258b54f`. `git/endpoint/https_adapter.rs` and its `worker`, `exchange`, `auth` and `pipe` children, `https_worker/native/connection.rs`, and the scheme router in `transport_host/connection_factory.rs`; fixture rows in `connection_host/https_tests.rs`, `https_auth_tests.rs` and, on Windows, `https_sspi_tests.rs`. No step-specific review was filed. The Windows receipt `p1-3c-20261025-a` failed on 5 of 1.3b's SSH rows (the 10 rows of 1.3c passed); run `2026-10-25-p1-3bwin` found test-row defects, fixed them in `b7e9188b`, and got the PASS receipt `p1-3bwin-20261025-b` on `c318376c` plus that patch. **Open, owned by C2(b) and being closed by lane p1c-https:** `start_exchange` takes the operation's `AuthContext` and ignores it (`https_adapter.rs:258-265`; the helper is still the instance's `HttpsInputs::auth`), so the helper in use is the instance's, not the operation's; `https_worker/reuse_tests.rs:141` and `:149` hold the named defect and an `#[ignore]`d row for the desired behaviour; HTTPS routes must be named uniquely per binding, never by the caller's operation name or `Input.session`; a challenge to a POST is refused (`https_adapter.rs:17`), so an authenticated push cannot go through the host. Limitations from the step's README: errors carry no `Effect` or `Facts`; discovery redirects are not followed; each adapter runs its own runtime; setups draw on the shared reservation's supervisor, not the `CleanupDomain`; nothing in production uses the host until Phase 3; candidate clippy was not run. |
| §7 (`EndpointProfile`, `AuthContext`); §4.1 (`ConnectCommand.auth`, `start_exchange(connection, &ExchangeSpec, &AuthContext)`); §8 (allocation deadline) | Recorded at step 1.5 (gwz-transport `bd7cf4f6`, `adf808d` and `9fbe7448`, merge `9f8f091`, pushed; gwz-core `d8dd9625` and `aa7eaff5`; Code and State round 2 GO and GO; evidence `2026-10-25-p1-5`, Windows receipt `p1-5-rem1-20261025-c` PASS): `PreparedOperation` carries the `EndpointRoute` and the validated `EndpointProfile`, captured once at the original caller entry (`capture_operation`), with the helper environment and account tokens in a per-operation `AuthContext` that `ConnectCommand.auth` and `start_exchange` carry and the adapter drops when the connect completes or is cancelled (never kept for a later exchange). The host's registry serves a request only on an equal profile. **Rulings:** the authentication scope partitions HTTPS keys only (`Identity::HttpsScoped`, an instance-local token per scope); SSH connections are not partitioned by the helper environment, because SSH reuse authorization is §7.1's possession proof (1.6). On a scope misfit the untouched lease is released `Reusable` and the opening is resubmitted once, fresh, under its original allocation deadline recorded at `submit`; `abandon` identifies a pending by its slot. Field 12 (machine proxy) and field 11 (`no_proxy`) have canonical bounded encodings (bypass list at most 8192 characters normalized as parsed, proxy string at most 4096, own addresses as octets); on OpenSSL builds the SHA-256 of the certificate bytes at the resolved verify paths is in the profile and the captured certificates are what the instance trusts. `capture_operation` refuses `tls.proxy.authorization`. **§7 amendment (Code round 2):** profile fields longer than their maximum are compared by SHA-256 (marker byte), so for those fields a SHA-256 collision counts as equality; this narrows "a digest collision is not equality", which previously held for the index digest, and extends the CA-content digest. **Two scopes on HTTPS identities:** the instance gives every HTTPS request `HttpsScoped("scope-N")`; the pool's `scope_https` (§7.2 per-connection NTLM/Negotiate scope) refuses a lease scoped differently. This is unreachable today (no adapter API scopes a lease); how §7.2's per-connection scope combines with the operation scope is still to be stated before Negotiate runs through the host, and belongs to C2(b) (1.3c). **Adapter conformance:** trunk adapters (`ssh_adapter.rs`, `connection_factory.rs`, `https_adapter.rs`) must report `command.identity` as their `Connected` proof or `None`; anything else fails the connect (identity mismatch). **Recorded limitation, decided 2026-10-31 (plan §10 D-R, operator):** if one authentication scope keeps every `per_user_host` connection (32 by default) busy and its openings keep arriving, a request of another scope waits and ends in `AllocationTimeout` at its original deadline, never `NoFittingConnection`; it is bounded and typed, covered by `host_tests.rs` `under_saturated_load_of_one_scope_another_scope_is_served_after_at_most_one_misfit`, and reopens only if the Phase 3 sweep shows a real two-scope workload. Observation: OpenSSL builds read and hash the root store on every `capture_operation` (unmeasured cost). The crate has no observation surface for an event-level R4 row. |
| §7.1 (possession proof at a cross-binding lease); §4.1 (`EndpointAdapter::authorize_reuse` / `cancel_reuse`, `Completion::Reuse`); reuse design §4 "Key types" | Recorded at step 1.6 (gwz-transport `a21e972`, `055d091` and `dd93098`, pushed; gwz-core `726e9062` and `9734f3f8`; Code and State round 2 GO and GO; evidence `2026-10-25-p1-6`, receipt `p1-6-20261025-d` PASS): before an idle connection last used by another binding is leased, the host issues `ReuseCommand { connection, binding, proof: ProofId }` and the adapter answers `Completion::Reuse { connection, proof, verdict }` with `Authorized`, `Revoked` (connection discarded, matching idle connections retired as `IdleClosed`) or `Unprovable` (back to idle for the binding that used it, WITHOUT re-stamping its idle clock; the opening is resubmitted excluding it, so a binding's own idle connection is offered); the host matches a verdict on `(connection, proof)` and drops a stale one; `cancel_reuse(proof)` is issued when the opening is abandoned or the host sealed. SSH proofs: an agent's fresh SSHSIG possession signature for ambient keys (nonce, preamble and namespace as reuse design §4), an explicit key re-read and compared, a fresh known_hosts check. An `ssh-rsa` answer to an `rsa-sha2-512` request is `Unprovable`, never `Refused`; RSA keys outside 2048-8192 bits are refused before any agent request. HTTPS connections authenticated by a helper token or by the Windows logon session are per-exchange scoped and never reused across bindings (§7.2). **Lane-owner departure from reuse design C9:** ECDSA P-521 is not a provable key type in this release (ring has no P-521); a P-521 connection is `Unprovable` at a cross-binding lease and is never shared across bindings. A shared proof job outlives its last waiter and records its verdict for the binding. **Retry rules (decided by the lane owner after State round 2, N-P3-1):** a `Revoked` verdict asks again FRESH (exclusion cleared), restoring the design rule, in both the crate and gwz-core; an `Unprovable` or no-evidence verdict uses `release_unused` (which keeps `idle_since`) and retries with the exclusion. No contract amendment is needed for `Revoked`. The SSH adapter's worker now holds the proof (see the integration row). Observations carried to the Phase 1 review: two last-lessee records (crate `BindingId` and the gwz-core bridge `session_id`) until the adapters wrap the worker; no maximum connection age is enforced anywhere yet (§10's 10 minutes); no rows for the certificate and security-key proof paths; the `runner.rs` split (operator); ring's duplicate dependencies. |
| §4.1 and §7.1 as steps 1.5 and 1.6 changed them, applied to the 1.3b and 1.3c adapters (Phase 1 integration fix-up; not a plan step) | Recorded at the Phase 1 integration (gwz-transport `3980469`; gwz-core `844f9511`, `22aebcee` and `5e97bd2d`; Code and State round 2 GO and GO, filed at root `e0f3e3c5`, conditional on a full matrix). `start_exchange` takes the operation's `AuthContext` in the SSH and HTTPS adapters and the scheme router, which also forwards `authorize_reuse` and `cancel_reuse`. The HTTPS adapter authorizes reuse from its scope records (an anonymous connection serves any binding; a helper-token or logon-session one never does). The SSH adapter's worker proves a connection another binding last used from its reuse evidence (key possession, host-key trust, per-binding proofs, siblings retired as `IdleClosed` after a `Revoked` verdict, a cancelled proof still recorded); leases of one binding that need the same proof join one job and every waiter is settled, and `cancel_reuse` removes one waiter; the proof deadline is the earlier of the opening's remaining time and the connect bound; a proof passing on a connection no longer idle is `Unprovable`. In the crate, `cancel_binding` sweeps the binding's authorizing openings before the pool cancels its owner; an authorizing opening past its allocation deadline ends `AllocationTimeout`; `ReuseCommand` carries `remaining_ms`; a `start_exchange` that reports `NotConnected` is a lost lease and the opening asks again. **Recorded here (Code round 2):** a shared proof job runs under the first waiter's deadline, not the latest waiter's. **The condition is met:** the round-2 verdicts depended on a full matrix; matrix `root-p1-20261027-b` failed on parity, and matrix `root-p1-20261027-c` PASSED at gwz-core `e0cb12f7` (logs under `/Volumes/projects/limbo/build-targets/gwz-dev-logs/remote-root-p1-20261027-c/`). The fix-up has no evidence run directory of its own. Open (State round 2): the HTTPS adapter still ignores `AuthContext` (C2(b), see the 1.3c row); no HTTPS retention row; no `Denied` end-to-end row in gwz-core; no production caller of `ExchangeSpec::git` until adoption in 2.5. |
| §8 (capacity per binding); reuse design §5 | Recorded at step 1.4 (gwz-transport `b0e5ad5` and `a22d43c`; gwz-core `a5ce2941`, `0c288b7b`, `408a8b24`, `4703a4e8` and `58e506ee`; State axis only, no second reviewer this round: NO-GO, NO-GO, GO at `-ReviewState-3`, then a follow-up GO at `-ReviewState-4` with P2-1 closed at every pool-owner site; root `acade138` and `85c9d8e5`; evidence `2026-10-29-p1-4`, receipt `-e` PASS at `4703a4e8`, receipts `-c` and `-d` failed on host load): the limits of an operation travel with its binding. The pool's `Request::limits` holds each `Owner.session` to its own limits in total, per host, per user and host, and `max_requests`; `Config` is the instance ceilings, applied as max(ceiling, limit); an owner at its limits evicts nothing. gwz-connection's `OperationPolicy::limits()` is carried by the binding onto every pool request. `TransportCapacityConflict`, `install_capacity` and the capacity gates are gone from gwz-core; the shared authority's ceilings are the largest limits of the live requests (stricter than §8's max(256, limit) on the legacy path only, which Phase 3 retires; lane-owner ruling), one request is one pool owner on both legacy schemes, and idle connections a lowered ceiling leaves over the reservations are retired. Open, for the Phase 1 review and Phase 3: no production consumer of binding limits (the factory builds its own `Authority` with a per-host 32 and never calls `admit_operation`, so a binding above 32 would be clamped; a hypothesis for Phase 3); cross-scheme idle eviction is not built; three P3s for the findings list: site-granular retirement, edge-triggered retirement, in-flight disposals still counted as held. |
| §5 "Revocation of acquired permits" (the sentences "Consuming a revoked permit returns an already sealed **terminal Operation**, with `registration = NeverRegistered` and a specific terminal reason (`HostClosing`, `HostFault` or `DomainClosing`)", "OperationEnd for an actual insertion instead states `Registered(binding_id)`…" and "The two legal race outcomes are: registration wins…; or revocation wins and register returns the terminal NeverRegistered operation") and §§4–9 public types; `GwzSplitCommandChannel.md` §3.4 (a served core holds the same `Admission`/`Operation`/`Opening`/`Exchange`) | Recorded at step 2.2c (rework), amended by remediation round 1. A served core holds the crate's own `HostHandle`, `Admission`, `Acquire`, `AdmissionPermit`, `Operation`, `OperationCancel`, `Opening`, `OpenWait`, `Exchange`, `MessageStream` and `ExchangeFinish`, each over a private local or carrier-backed representation; no `Remote*` type is public. **Admission.** `HostHandle::admit` takes `impl Into<AdmitInput>`: `AdmitInput::Prepared(PreparedOperation)` (a local host) or `AdmitInput::Reference(OperationRef)` (a carrier-backed `remote::handle`), each refusing the other with `AdmissionError::WrongInput`. `OperationRef::new(request_id)` takes a non-empty `request_id` of at most 128 bytes (the bound of a lane request name) with no control character. `AdmissionError` gains `WrongInput`, `UnknownCall` (the `request_id` is not a live call of the session), `DuplicateOperation` and `CarrierLost`. `OpenError` gains `Refused(remote::TableRefusal)`. `OpenFailure` gains `Refused(Failure)`, `CarrierLost { effect }` and `Fault(ErrorCode)`. `StreamError` gains `CarrierLost { effect }`, `Failed { code, effect }`, `Closed` and `Protocol`, and is now `#[non_exhaustive]`. `OperationEnd` gains the field `revoked: Option<TerminalReason>` and `ExchangeEnd` the field `failure: Option<(ErrorCode, Effect)>`; both structs are exhaustive, so a struct literal outside the crate breaks. `Operation::registered()` returns the new future `Registered<'_>`. **Registration (amends the three §5 sentences named in the left column).** `Registration` gains `Pending` (a carrier-backed deferred commit: `register` returns an operation whose registration the client decides; a local registration is never pending) and `TerminalReason` gains `ClientRefused(ErrorCode)`, `CarrierLost`, `TimedOut` and `Cancelled`; both lose `Eq` (their `ErrorCode` is `PartialEq` only) and stay `#[non_exhaustive]`. The registration of a carrier-backed operation is decided after `register`, by events §5's table does not list, and ends: `Registered(binding_id)` on `bound`; `NeverRegistered(ClientRefused(code))` on `bind_rejected`, where `code` is `capacity`, `invalid_request` or `cancelled` from the client, `protocol` for a binding the carrier faulted or a message under another call's name, and `unsupported_version` or `unsupported_operation` from the mux's bootstrap; `NeverRegistered(CarrierLost)` when the carrier ends first (the third revocation outcome: zero effects; a live binding is instead revoked, `OperationEnd::revoked = Some(CarrierLost)`, its open exchanges fail `CarrierLost { effect }`, and a started `receive_pack_exchange` reports `Effect::Possible`); `NeverRegistered(TimedOut)` when the client does not answer within the 5 s bootstrap bound; and `NeverRegistered(Cancelled)` when the operation is cancelled or finished before `bound`. So "the two legal race outcomes" become, for a carrier-backed handle, a third: `register` returns a pending operation, and a registered one is never reported `Pending` again. A cancelled opening, pending or registered and whatever the client answers (`opened`, `open_failed`) resolves `OpenFailure::Cancelled`, and ends `CarrierLost` if the carrier ends before it is polled; `OpenFailure::Fault(code)` is a rule of the transport broken (`protocol`, or the table's `invalid_request`), with one known exception carried to step 2.3 (2.2c re-verdict Code P3-8): an open issued right after an abort at the binding's stream bound can answer `Fault(capacity)` while the client's terminal for the aborted stream is still in flight. **Snapshot.** `HostHandle::snapshot()` on a carrier-backed handle is a refusal (`HostSnapshot::refused()`): every count is meaningless, `phase` and `namespace` are the handle's own (`phase` is `Closed` once the carrier is lost). **The `remote` module's public ports** (new): `handle`, `handle_with`, `RemoteConfig`, `BindBounds` (what a carrier holds a binding's inbound state to: the binding's receive window and its mux's streams), the carrier ports `Lane` (with `retire` and `faulted`), `CoreBinding`, `ClientSession` (both with `progress()`), `LaneSend`, `LaneEnded`; the client's `Receiver`, `serve_bindings`, `Invocation`, `Invocations`, `NamedKey`, `RefusalNote`; and the admission types `AllowedSet`, `Allowance`, `Canonical`, `CanonError`, `Refusal`, `ServiceClass`, `admit_service`, `TableRefusal`. **Wire.** gwz-transport's codec admits exactly the five `bind_rejected` codes of §3.4 (`unsupported_version`, `unsupported_operation`, `capacity`, `invalid_request`, `cancelled`) with `Effect::None`, no facts and no failure detail (a wire change; its README says so). A byte-stream carrier carries the lane body as the generated `TransportLaneFrame`, opens the core's bindings as their `bind` goes out and admits the client's, each held to `BindBounds`. **Release and scope.** The core's `finish()` sends nothing that releases the client's binding (§3.3 has no kind for it); the client's call end does, so client slot occupancy is call-scoped: one binding per `request_id`, and a call cannot run a second operation on another route (an operator decision for 2.0 §3.4: a release kind or a multi-bind rule). The client's `admission_bound_ms` (5 s by default) equals the core's bootstrap bound (5 s): a client that obtains its slot at about 5 s registers after the core has timed out, and the orphan registration lasts until the call ends. **Decided 2026-10-31** (plan §10 D-P, operator): a call has one operation and a second route is a second call; the client's `admission_bound_ms` default is set below the 5 s bootstrap bound (4 s, lane p1c-crate); a multi-bind rule or a release kind is recorded for Phase 7. |
| `GwzSplitCommandChannel.md` §4.3 ("It runs the plan's 1.7 driver-only builder: no endpoint, no agent, `native` transport refused") and §4.6 (`--transport native`, `GWZ_TRANSPORT=native`, `gwz.transport native`; `file`, clone-family, `http://` and `git://` refused before any effect); §12 for a served core | Recorded at step 1.7 (gwz-core `a681cf26` (was `25941ce9`, not an object; corrected 2026-10-31), merged `7e7d36a9`; reviews `GwzDriverOnly-Step1.7-Review{Code,State}{,-2,-3}.md`, GO). `transport_host::DriverOnlyBuilder::new(io_timeout_ms, connect_timeout_ms)` with `with_transport_flag(Option<Transport>)` and `build(&EnvironmentSnapshot) -> (DriverOnly, TransportPort)`: the host is the cli placement's driver session and nothing else, with no `EndpointSettings`, `Handoff`, agent, key, helper, native gate or local endpoint session (`scripts/checks/check_driver_only_boundary.py`, an import allow-list with no aliases and no local-transport entry points, run by `run_tests.py` and the boundary workflow). It makes no `HostHandle`: the core's requests reach the client through the session port until phase 3, and the carrier-backed handle (2.2c) answers `snapshot()` with a refusal. `build` resolves the transport setting and refuses native before anything is built, naming the form that selected it (`--transport native`, `GWZ_TRANSPORT=native`, `gwz.transport native` in a named file); every request must be `cli`, any other placement is refused. Under cli every URL-taking operation judges the URL libgit2 will connect to, after `url.<base>.insteadOf` and `pushInsteadOf`, with the one classifier the binding binds on (`transport_binding::bound_route`); identity selection reads the same classifier. `file://`, bare local paths (including a backslash before the first colon), `http://`, `git://`, other schemes and the clone family's anonymous local peer are refused by name before any connection, agent or key read, or ref, target or scratch write. In a cli context the binding installs a refusing transport for any route it does not bind, so libgit2's own transport never runs under cli. Clone and remote-file read judge the global configuration's rules before the target exists; a rewrite only the new repository's configuration makes (`includeIf "gitdir:"`) is refused by name when the clone creates its remote, before any connection, and libgit2 removes the target it created; a same-kind rewrite rides the forced transport, a cross-kind one is refused ("different route"). Bracketed IPv6 SCP spellings (`[::1]:repo`) are bound to the SSH route but not served: the core session driver refuses them ("invalid SSH destination") before the endpoint, with the selected identity recorded. Local and native placements are unchanged. |

No existing taut field tag, method surface or user flag is changed by this design.
If adapting the chosen generic Rust surface actually requires a wire-schema
extension, document the failed existing route and return for scope review; do not
manufacture a new wire transport or hide Git types in a supposedly generic API.

## 16. Review and handoff

Review this contract on Consistency (with API Surface) and Safety, including
authority/necessity, actual consumer wiring and recurrence prevention. Pin the
committed document and reference tuples; two peer-blind reviewers, at most two
remediation rounds under the review-loop skill. GO accepts this interface/design
scope only, not native qualification, implementation or release.

Next authorized implementation decision, if requested: a concrete small-proof
boundary limited to the new crate, required pure-kernel seams and fake tests.
Native process-safety remedy and actual consumer migration remain explicit gates,
not tasks that can silently replace the proof or the release's reuse outcomes.
