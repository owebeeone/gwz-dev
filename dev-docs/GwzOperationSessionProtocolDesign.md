# GWZ operation-session protocol revision

Date: 2026-09-23. Status: **DRAFT correction 1 after round-1 NO-GO; design
re-review required before implementation**.

This is the candidate correction for the session/request/result boundary found in
[the Python concurrency review](GwzPyTransportConcurrency-RemPlan-1.md). The
[release NO-GO](GwzPyTransportConcurrencyNoGo.md) remains in force. This document
does not authorize implementation or revise the `gwz-transport` stream-message
protocol. It defines application messages and receiver ownership; they can run
inside today's process and can later be carried over a CLI/core connection.
The [candidate caller guide](GwzOperationSessionCallerGuideDraft.md) is part of
this review object so the Python Surface axis can assess the proposed API from
caller-facing documentation without reading source or internal design.
The [core capacity amendment](../gwz-core/dev-docs/GwzRemoteTransportCapacityAmendment.md)
is also part of the corrected tuple. It contains the exact retry and Phase-2
supersessions. The [round-1 merge](GwzOperationSessionProtocol-RemPlan-1.md)
maps each blocking finding to this correction.

## 1. Problem and boundary

`RequestMeta.request_id` is caller chosen. Python currently derives
`operation_id = "op_" + request_id`, stores events/results in a process-global
map, and reads that map through module-level functions. Two clients using the
same request ID can share a record; knowledge of another operation ID can also
reach its record. Concurrent operations on one client expose the problem more
often, but adding a second active slot cannot fix it.

The revised logical hierarchy is:

```
bound caller / route
  -> operation session (owner, endpoint environment, physical pools)
       -> operation execution scope (caller ID, opaque operation ID)
            -> worker completion, events, typed response and cleanup
            -> optional transport request scope
       -> transport generation(s)
            -> gwz-transport requests and streams
```

An operation session is an application owner, **not** a socket or a new
`gwz-transport` stream. The in-process Python bridge binds its native session
object to one owner. A later wire receiver must bind a logical session to its
authenticated/authorized route; the carrier's authentication and delivery are
outside this revision. A session ID or operation ID alone is never a bearer
credential. The receiver checks both IDs against the bound route on every
control or read action.

`gwz-transport` still sends discrete stream messages inside the existing Taut
envelope's optional transport field. This design neither adds a byte carrier nor
requires iroh, CLI/core socket work, or changes to Git's stream adapter. The
operation execution scope exists for **every** accepted local or network
operation. It owns worker start, cancellation, terminal recording and join.
Network work additionally owns a `TransportRequest`; local-only work does not
construct an endpoint. A successful close never reports completion while a
local handler can still mutate the workspace. If the close wait times out, it
returns typed `ClosePending`, retains the shared close task and session charge,
and a later close joins it; it does not claim a final cleanup report.

## 2. Taut application contract, version 1

The current service/schema is version 0. Add a negotiated `operation_sessions`
capability with version `1`; absence means a peer cannot use this contract.
Only add fields and methods through normal Taut schema/codegen and fingerprint
checks. The exact generated spelling and field numbers are allocated in the
schema patch, not by hand in generated files.

| Message or method | Contract |
| --- | --- |
| `operation_session.open` | The receiver creates a cheap logical session after checking the receiver/route session budgets in §4. It returns an opaque `session_id`, selected contract version and finite published budgets. It does not build an SSH/HTTPS endpoint or inspect credentials/proxy/CA. On first network use the session captures its endpoint environment as the accepted Python design requires. |
| `RequestMeta.session_id?` | A version-1 request carries the session selected at open. The receiver also checks its route binding. Local-only operations may use the session without constructing a network endpoint. Legacy requests without this field remain on the version-0 path only. |
| `ResponseMeta.operation_id?` | For a submitted operation, the receiver allocates a fresh opaque ID independently of `request_id` and returns it at admission. It is unique within the session's lifetime and never reused there. `request_id` remains caller correlation, not authority or a lookup key. |
| `operation.events_v1` | A Taut shape-log read keyed by `session_id` and `operation_id`. The existing shape-log cursor, tail, end and retention semantics apply; no second bespoke cursor protocol. Reads are owner checked. |
| `operation.result_v1` | A terminal read keyed by both IDs. Before completion it waits or reports typed pending according to the chosen call form; after completion it returns the immutable terminal record defined below; after release or retention expiry it reports `OperationExpired`. |
| `operation.cancel_v1` | A control request keyed by both IDs. It signals only that execution scope, joins its handler and optional `TransportRequest.finish()`, and returns its cleanup report. Repeating it while the terminal record is retained returns the same report. |
| `operation.release_v1` | Idempotently releases a retained terminal record and its event log. A live operation is not destroyed by releasing a reader; the caller must cancel it or close the session. |
| `operation_session.close` | Stops admissions, cancels and joins every admitted execution scope, then shuts down the one session host. Repeated calls join one shared completion and return the same cleanup report while the session close record is retained. A bounded wait may return `ClosePending`, never a false completed report. |

One retained terminal record owns the existing `OperationResult`, the full
action-specific Taut response, the cancellation/cleanup report and their byte
charges. `operation.result_v1` returns a `OperationTerminalV1` containing
`operation_id`, caller `request_id`, action, status, optional
`OperationResult`, optional `response_message` plus its canonical Taut CBOR
`response_bytes`, optional cleanup and a typed error. The response message
name must be on the closed service method-to-response table and the decoded
`ResponseMeta` IDs/action/schema must match the terminal header. This is a
typed encoded response, **not** arbitrary JSON. Python decodes it to the
existing generated response class; Rust can do the same. Fetch retains
`FetchResponse.repos`, merge retains `MergeResponse` state/record/recovery,
and other actions retain all of their action-specific fields. A normal
successful `handle.result()` returns that generated response; a failed or
cancelled operation raises `GwzOperationError` with the terminal
`OperationResult` and code attached. `handle.terminal()` exposes the whole
immutable terminal record for callers that need status or cleanup details.
`cancel()` separately returns the retained `TransportCleanup` on completion.

If the full action response would exceed a published byte budget, the
receiver stores a small `ResultLimitExceeded` terminal instead of a partial
or falsely successful response. It identifies action, request, operation,
observed effect as `none`/`may_have_applied`/`applied`, and the limit that was
hit. It remains readable until release/expiry and `handle.result()` raises a
typed error; the caller must not infer rollback. The result serializer writes
into a charged bounded buffer and switches to this reserved small terminal
before exceeding its per-record or aggregate limit. A 4 KiB fallback charge
is reserved at `Accepted` for every operation. Full response, summary and
cleanup share one release/expiry boundary. No second process-global merge
response store is reachable by version-1 lookup.

The new role-out methods do **not** replace the version-0 scalar
`events.subscribe(operation_id)` and `operation.result(operation_id)` for an
existing client. Their legacy registry and any module-level Python compatibility
functions remain isolated from version-1 records; a legacy lookup must never
fall through into a version-1 session. A version-1 caller cannot silently
downgrade when the peer lacks the capability: it receives `UnsupportedOperation`
before submitting work. The `transport_capabilities` response or an equivalent
schema-negotiated handshake advertises the selected version before the first
operation. The exact Taut method names above are proposed names, not a claim
that they exist today. This candidate allocates the following additive schema
slots for codegen review; no generated source is hand-edited:

- `RequestMeta.session_id?` uses field 10 and optional
  `RequestMeta.placement?` uses field 11 with `local=0`, `cli=1`; current
  fields 1–9 keep their meanings. Omitted placement means local.
  `TransportCapabilitiesResponse.operation_sessions_version?` uses
  field 3; absence means version 0 only. `ResponseMeta.operation_id?` already
  uses field 5 and is not renumbered.
- `OperationSessionOpenRequest` has `schema_version` field 1.
  `OperationSessionOpenResponse` has `session_id` field 1, `version` field 2
  and `limits` field 3. `OperationSessionLimitsV1` uses fields 1–22 for,
  respectively: receiver sessions, route sessions, session active operations,
  session queued operations, receiver accepted operations, session operation
  workers, receiver operation workers, session member workers, receiver member
  workers, session retained records, receiver retained records, per-record
  terminal bytes, session terminal bytes, receiver terminal bytes, per-record
  event bytes, session event bytes, receiver event bytes, completed-record
  sliding TTL, completed-record hard TTL, idle-session TTL, closed-report TTL
  and close-wait duration. All counts/bytes are positive integers; durations
  are nonnegative milliseconds.
- Version-1 event/result/cancel/release calls use `session_id` field/parameter
  1 and `operation_id` field/parameter 2. Close takes `session_id` field 1.
  `OperationCleanupV1` has `pending_local_work` field 1 and
  `peer_cleanup_confirmed` field 2. A release acknowledgement has `released`
  field 1. `OperationTerminalV1` has operation ID 1, caller request ID 2,
  action 3, status 4, optional `OperationResult` 5, optional validated
  response message name 6, optional canonical CBOR response bytes 7,
  optional cleanup 8, optional typed error 9 and observed effect 10.
- Extend `GwzErrorCode` after current value 72 with stable values:
  `session_capacity_full=73`, `capacity_busy=74`,
  `retained_capacity_full=75`, `capacity_conflict=76`,
  `placement_unavailable=77`, `operation_expired=78`,
  `result_limit_exceeded=79`, `close_pending=80`, and
  `event_cursor_gap=81`. Existing values are unchanged. Python exception
  classes may map these codes to CamelCase names but must preserve the
  numeric/code identity.

The version-0 schema/decoder and its records remain in an isolated legacy
namespace. A new receiver chooses version 1 only after the caller's
capability/schema handshake; it must not assume a version-0 peer can decode
new fields. The exact codegen and cross-version compatibility tests are an
implementation gate, not evidence already obtained here.

On a future wire route, reconnect does not implicitly reattach to an old
session. A carrier's route-loss callback immediately puts its bound sessions
in Closing, cancels their work and joins through the retained close task;
the receiver keeps each charged until cleanup completes. It does not wait for
reconnect or rely on a wire heartbeat defined here. In-process Python owner
drop triggers the same close path. Resumption, if ever needed, requires an
explicit separately reviewed authorization and retention contract.

## 3. Admission, result and caller semantics

The receiver checks, in this order, the bound session, duplicate **live**
`request_id` within that session, available operation/worker/queue/result
budget, placement support, and the physical-capacity rule in §4. It reserves
the execution scope and fallback terminal charge, allocates the opaque
operation ID, and, for network work, registers the transport request scope
before replying `Accepted`. Local work uses no transport request. No Git
handler, remote Open, credential helper or endpoint mutation may occur for
an admission refusal. `Accepted` means those checks and reservations have
completed; it does **not** mean Git work has succeeded. A queued accepted
operation already has a handle and can be cancelled before worker start.

Every accepted ID gets exactly one terminal result, including worker-launch
failure, handler panic, cancellation, and close. A failure after `Accepted`
appears in that result with the same session/operation/request attribution.
The operation result is separate from event retention, so a slow event reader
cannot erase the final outcome. Two sessions can use the same request ID, and
neither can read, cancel or release the other's operation. A duplicate live ID
in one session is refused before `Accepted`; after retirement a caller request
ID may be reused, but the opaque operation ID never is. Each operation also
gets a separate internal transport request ID unique inside its mux generation;
`RequestMeta.request_id` is never used as the mux routing key. That internal ID
is not a Python API or Taut application lookup handle. A delayed message for
the retired internal ID is rejected without touching the new operation.

Python exposes `OperationHandle` from an awaited `Client.start_*` or generic
`Client.start(...)` call. It contains `operation_id` immediately after
`Accepted`, plus `events()`, `result()`, `terminal()`, `cancel()` and
`async release()` methods. Release is idempotent for the same owned handle and
completes only after its terminal record and event log are uncharged. Existing unary
calls can await the handle's result; existing stream helpers can wrap its event
iterator. The handle is the documented way for another task to cancel a
high-level operation before its first event. A caller-supplied request ID alone
is not a cancellation handle. The existing `Client.cancel_operation(id)` may
remain as a convenience, but resolves through **that Client's** bound session.

An admission refusal is a typed exception from `await start/submit`, before an
operation ID exists. A unary convenience call raises it at invocation; a stream
helper raises it when its first iteration performs admission. After acceptance,
all three forms expose the same typed terminal outcome through the handle or
high-level wrapper. `CapacityBusy` means the session's finite live-work budget
is full and may be retried when work retires. `RetainedCapacityFull` means
completed records or their reserved bytes occupy the retention budget and may
be retried after explicit release or expiry. `SessionCapacityFull` means the
receiver/route has reached its open-session budget and may be retried after a
session closes. `CapacityConflict` means the
requested physical capacity cannot be installed under active leases and may be
retried after those leases and cleanup retire. `PlacementUnavailable` means the
requested placement has no usable bound endpoint and should not be retried
unchanged. These reasons must be stable machine values, not parsed messages.

Python `close()` starts one native close completion; cancelling an asyncio
waiter does not cancel that completion. A later `close()` joins it and returns
the retained report, or a typed `ClosePending` if bounded waiting expires.
`__aexit__` waits for the same completion even when the
enclosing task is cancelled, then propagates cancellation. Repeated
per-operation cancellation returns its retained cleanup report until release
or expiry; a foreign ID is refused, and an owned expired ID reports
`OperationExpired` while its bounded expiry marker exists.

## 4. Placement, capacity and resource ownership

The core endpoint is the physical-capacity authority. One Python session can
admit many compatible operations, each with its own `jobs`, per-host work
limit, retries, cancellation and result. Pool checkout `max_requests`, logical
live-operation slots, Python operation-worker slots and core member-worker
permits are **different budgets**. Caller-controlled `jobs` enlarges none of
the last three. A shared session/receiver permit scheduler caps aggregate
member workers while each operation also respects its own `jobs` and
per-host work limit. Queueing for a permit is bounded and cancellable; a
worker never spins up simply because the caller raised `jobs`. A bounded
operation-worker queue or fallible launcher reserves an active or queued slot
before `Accepted`; start failure terminalizes the reserved operation and
wakes close. A worker unwind follows the same terminal path.

The exact proposed supersession of retry §3 item 7, §6, S1.4 and the transport
plan Phase-2 exit sentence is in the paired
[core amendment](../gwz-core/dev-docs/GwzRemoteTransportCapacityAmendment.md).
An installed capacity admits a componentwise equal or lower request under a
live lease without resize; the new operation's own work limits still apply.
A needed raise refuses with `CapacityConflict`. At a truly quiescent boundary,
one leader publishes a new exact capacity; same-target waiters join it,
different-target waiters refuse, and cancellation/close wakes all. Quiescence
includes admitted operations that have not yet opened a lease. The older
accepted clauses remain authority until this paired amendment receives GO;
code must not implement the corrected rule from this draft alone.

For explicit in-process `cli` placement, the bound CLI endpoint must perform
the same capacity check **before** handler dispatch or remote Open. If it
cannot, the version-1 session refuses that placement at admission with
`PlacementUnavailable`; local placement remains usable. No Python-only guard
is claimed to protect a different physical owner.

The transport mux's current 256-ID ceiling is a lifetime tombstone bound, not
a live-operation budget. The host must rotate to a fresh transport generation
before that ceiling is exhausted while old generations drain. A generation
owns its internal request IDs and rejects late messages from prior generations;
endpoint trust/credential context remains the session's captured context.
Retain a finite number of draining generations and refuse excess admissions
transiently if they cannot drain. More than 256 sequential operations must
work on one session, and delayed old messages must never attach to new work.
Generation rollover is internal to core/`gwz-transport`, not a new Taut wire
carrier. Its exact implementation and finite generation bound need their own
reviewed proof before activation.

Version 1 fixes the following initial limits; they are returned by
`operation_session.open` and are not caller-configurable in this version.
They can be revised in a later negotiated version, never silently enlarged
by `jobs` or by opening another session. Receiver limits include all its
Python/native clients, and each in-process Client is one route for this
accounting purpose.

| Resource | Per session | Per receiver | Rule |
| --- | ---: | ---: | --- |
| Open sessions | — | 32 | At most 4 per route; excess open returns `SessionCapacityFull` before a host is allocated. |
| Live accepted operations, including queued | 24 | 128 | At most 8 executing and 16 queued per session. Full admission returns `CapacityBusy`. |
| Operation worker threads | 8 | 64 | Queue is the 16 accepted nonexecuting slots above; fallible launch. |
| Core member workers | 128 | 256 | Shared fair permits; a request's `jobs` is an additional upper limit. |
| Retained terminal records | 64 | 256 | Active terminal fallback reservations also count against the byte budgets below. |
| Terminal record bytes | 16 MiB each, 64 MiB total | 256 MiB total | Includes action-specific response, operation summary, cleanup, encoding overhead and 4 KiB fallback reservation per accepted operation. |
| Event-log bytes | 1 MiB each, 8 MiB total | 64 MiB total | Shape-log cursor gap on eviction; final terminal independent. |
| Completed record lifetime | 10 minutes | — | Since completion or last successful owned read, with a hard 30-minute maximum since completion. Explicit release frees it sooner. |
| Idle session lifetime | 10 minutes | — | No live work or readers and no owned activity; expiry starts close. |
| Closed-session report lifetime | 60 seconds | — | Repeat close joins/returns report during this interval; then `OperationExpired`. |

The receiver and route limits apply **before** creating a logical session.
Owner drop or route loss starts close immediately, even if operations remain;
an active session with a healthy route is not expired solely for doing long
work. Close may report `ClosePending` after five seconds but keeps its session,
worker and byte charges until all child work actually joins; repeat close
can observe the eventual final report. A session whose route is gone cannot
be reattached or controlled from the replacement route. If local work cannot
yet provide bounded cancellation checkpoints, its worker stays charged and
close remains pending rather than claiming success; implementation acceptance
must prove it cannot mutate after a successful close.

At admission each operation reserves the 4 KiB fallback terminal charge.
If full retained-record slots or byte reservations prevent that, the request
gets `RetainedCapacityFull` before `Accepted`. A full response is serialized
only while both per-record and aggregate byte charges remain available;
otherwise the reserved `ResultLimitExceeded` terminal replaces it. Event
logs use bounded Taut shape-log retention: a reader behind the retained
cursor gets a typed gap/expired-cursor result, while the final terminal stays
readable until its own release/expiry. A shape-log reader can resume at the
oldest retained cursor after a gap or read the terminal result. Every held
reader is included in session lifetime accounting. No silent truncation or
unbounded process-global store is permitted.

## 5. Review and closure

This is a protocol design draft, not a release GO. Before implementation:

1. Review this corrected design, caller guide and paired core capacity
   amendment as one exact tuple. Obtain fresh independent Consistency and
   Safety verdicts and a **docs-only** Python Surface verdict. The first
   round's NO-GO reports remain evidence, not acceptance of this revision.
2. Apply the accepted Taut field allocation through generated Rust/Python
   code and fingerprint checks. Keep version-0 dispatch/storage isolated;
   test both version selections and explicit refusal of downgrade. No
   generated file is hand-maintained.
3. Implement the session-bound store and API, then test two clients using the
   same explicit request ID with opposite completion orders; foreign lookups;
   repeated caller IDs within one session; full fetch/merge typed response
   retention and bounded output-limit terminals; accepted worker-launch and
   handler failure paths; early handle cancellation; same-client overlapping
   operations; blocked local-only cancel/close; aggregate member worker and
   queue ceilings; capacity and placement refusals before Open; close-waiter
   cancellation; session/route limit+1 and abandonment; finite event/result
   byte retention; and more than 256 sequential operations with delayed
   stale transport messages.
4. Keep `gwz-py` Phase 6/7 and release activation NO-GO until implementation
   and release reviews close these cases. Do not infer wire-carrier readiness
   from in-process message-contract tests.
