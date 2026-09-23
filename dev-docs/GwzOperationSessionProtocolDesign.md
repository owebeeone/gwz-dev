# GWZ operation-session protocol revision

Date: 2026-09-23. Status: **DRAFT; design review required before implementation**.

This is the candidate correction for the session/request/result boundary found in
[the Python concurrency review](GwzPyTransportConcurrency-RemPlan-1.md). The
[release NO-GO](GwzPyTransportConcurrencyNoGo.md) remains in force. This document
does not authorize implementation or revise the `gwz-transport` stream-message
protocol. It defines application messages and receiver ownership; they can run
inside today's process and can later be carried over a CLI/core connection.
The [candidate caller guide](GwzOperationSessionCallerGuideDraft.md) is part of
this review object so the Python Surface axis can assess the proposed API from
caller-facing documentation without reading source or internal design.

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
       -> operation (caller request ID, opaque server operation ID)
            -> events, final result, cancellation and cleanup
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
requires iroh, CLI/core socket work, or changes to Git's stream adapter.

## 2. Taut application contract, version 1

The current service/schema is version 0. Add a negotiated `operation_sessions`
capability with version `1`; absence means a peer cannot use this contract.
Only add fields and methods through normal Taut schema/codegen and fingerprint
checks. The exact generated spelling and field numbers are allocated in the
schema patch, not by hand in generated files.

| Message or method | Contract |
| --- | --- |
| `operation_session.open` | The receiver creates a cheap logical session, returns an opaque `session_id`, selected contract version and finite published budgets. It does not build an SSH/HTTPS endpoint or inspect credentials/proxy/CA. On first network use the session captures its endpoint environment as the accepted Python design requires. |
| `RequestMeta.session_id?` | A version-1 request carries the session selected at open. The receiver also checks its route binding. Local-only operations may use the session without constructing a network endpoint. Legacy requests without this field remain on the version-0 path only. |
| `ResponseMeta.operation_id?` | For a submitted operation, the receiver allocates a fresh opaque ID independently of `request_id` and returns it at admission. It is unique within the session's lifetime and never reused there. `request_id` remains caller correlation, not authority or a lookup key. |
| `operation.events_v1` | A Taut shape-log read keyed by `session_id` and `operation_id`. The existing shape-log cursor, tail, end and retention semantics apply; no second bespoke cursor protocol. Reads are owner checked. |
| `operation.result_v1` | A terminal-result read keyed by both IDs. Before completion it waits or reports typed pending according to the chosen call form; after completion it returns exactly one immutable terminal result; after explicit release or retention expiry it reports `OperationExpired`. |
| `operation.cancel_v1` | A control request keyed by both IDs. It cancels only that operation, joins its `TransportRequest.finish()`, and returns its cleanup report. Repeating it while the terminal record is retained returns the same report. |
| `operation.release_v1` | Idempotently releases a retained terminal record and its event log. A live operation is not destroyed by releasing a reader; the caller must cancel it or close the session. |
| `operation_session.close` | Stops admissions, cancels and joins every admitted operation, then shuts down the one session host. Repeated calls join one shared completion and return the same cleanup report while the session close record is retained. |

The new role-out methods do **not** replace the version-0 scalar
`events.subscribe(operation_id)` and `operation.result(operation_id)` for an
existing client. Their legacy registry and any module-level Python compatibility
functions remain isolated from version-1 records; a legacy lookup must never
fall through into a version-1 session. A version-1 caller cannot silently
downgrade when the peer lacks the capability: it receives `UnsupportedOperation`
before submitting work. The `transport_capabilities` response or an equivalent
schema-negotiated handshake advertises the selected version before the first
operation. The exact Taut method names above are proposed names, not a claim
that they exist today.

On a future wire route, reconnect does not implicitly reattach to an old
session. The old receiver owns close/expiry. Resumption, if ever needed, will
require an explicit separately reviewed authorization and retention contract.

## 3. Admission, result and caller semantics

The receiver checks, in this order, the bound session, duplicate **live**
`request_id` within that session, available operation/worker/result budget,
placement support, and the physical-capacity rule in §4. It reserves the
operation record and its eventual terminal-result slot, allocates the opaque
operation ID, and registers its transport request scope before replying
`Accepted`. No Git handler, remote Open, credential helper or endpoint mutation
may occur for an admission refusal. `Accepted` means those checks and
reservations have completed; it does **not** mean Git work has succeeded.

Every accepted ID gets exactly one terminal result, including worker-launch
failure, handler panic, cancellation, and close. A failure after `Accepted`
appears in that result with the same session/operation/request attribution.
The operation result is separate from event retention, so a slow event reader
cannot erase the final outcome. Two sessions can use the same request ID, and
neither can read, cancel or release the other's operation. A duplicate live ID
in one session is refused before `Accepted`; after retirement a request ID may
be reused, but the opaque operation ID never is.

Python exposes `OperationHandle` from an awaited `Client.start_*` or generic
`Client.start(...)` call. It contains `operation_id` immediately after
`Accepted`, plus `events()`, `result()` and `cancel()` methods. Existing unary
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
is full and may be retried when work retires. `CapacityConflict` means the
requested physical capacity cannot be installed under active leases and may be
retried after those leases and cleanup retire. `PlacementUnavailable` means the
requested placement has no usable bound endpoint and should not be retried
unchanged. These reasons must be stable machine values, not parsed messages.

Python `close()` starts one native close completion; cancelling an asyncio
waiter does not cancel that completion. A later `close()` joins it and returns
the retained report. `__aexit__` waits for the same completion even when the
enclosing task is cancelled, then propagates cancellation. Repeated
per-operation cancellation returns its retained cleanup report until release
or expiry; a foreign ID is refused, and an owned expired ID reports
`OperationExpired` while its bounded expiry marker exists.

## 4. Placement, capacity and resource ownership

The core endpoint is the physical-capacity authority. One Python session can
admit many compatible operations, each with its own `jobs`, per-host work
limit, retries, cancellation and result. Pool checkout `max_requests`, logical
live-operation slots and Python worker slots are **different budgets**.
Caller-controlled `jobs` must not enlarge the latter two. A bounded worker
queue or fallible worker launcher reserves a slot before `Accepted`; start
failure terminalizes the already reserved operation and wakes close.

The accepted retry plan currently derives physical pool capacity from each
operation and refuses every request under a non-idle lease. This conflicts
with the transport plan's different-policy overlap exit test. The proposed
supersession is: if the installed physical capacity already accommodates a new
operation, admit it without resizing or resetting idle timers, even when its
own work limit is lower. A request needing to **raise** physical capacity while
leases are live gets `CapacityConflict`. At a quiescent boundary, a capacity
change has one transition leader and one target; same-target admissions wait
for atomic publication, different-target requests refuse, and cancellation or
close wakes all waiters. The revised Phase-2 different-limit case must start
with capacity that accommodates both and verify each operation's own fan-out
limit. This explicitly amends the retry plan's blanket refusal and narrows
the transport plan's exit row; neither older text can silently remain the
current rule. A separate reviewed capacity amendment must record the exact
core policy before code changes.

For explicit in-process `cli` placement, the bound CLI endpoint must perform
the same capacity check **before** handler dispatch or remote Open. If it
cannot, the version-1 session refuses that placement at admission with
`PlacementUnavailable`; local placement remains usable. No Python-only guard
is claimed to protect a different physical owner.

The transport mux's current 256-ID ceiling is a lifetime tombstone bound, not
a live-operation budget. The host must rotate to a fresh transport generation
before that ceiling is exhausted while old generations drain. A generation
owns its request IDs and rejects late messages from prior generations;
endpoint trust/credential context remains the session's captured context.
Retain a finite number of draining generations and refuse excess admissions
transiently if they cannot drain. More than 256 sequential operations must
work on one session, and delayed old messages must never attach to new work.
Generation rollover is internal to core/`gwz-transport`, not a new Taut wire
carrier. Its exact implementation and finite generation bound need their own
reviewed proof before activation.

Each session publishes finite limits for live operations, worker/queue slots,
event-log bytes, retained terminal records and retention duration. A live
record always has a reserved terminal-result slot. When retained-result
capacity is full, new submissions refuse before `Accepted`; records are
released explicitly or expire after the advertised interval. Event logs use
bounded Taut shape-log retention: a reader behind the retained cursor gets a
typed gap/expired-cursor result, while the final operation result remains
readable until its own release/expiry. No silent event truncation or unbounded
global store is permitted. The implementation design must choose and publish
numeric defaults before review; they are not inferred from pool limits.

## 5. Review and closure

This is a protocol design draft, not a release GO. Before implementation:

1. Settle the proposed Taut method/field allocation, negotiation and legacy
   isolation with generated Rust/Python shape checks. Record the capacity
   supersession in the retry and Phase-2 authorities, including numeric
   resource defaults and the CLI-placement admission owner.
2. Obtain independent Consistency, Safety and **docs-only** Python Surface
   design reviews of one committed document tuple. The earlier first-draft
   NO-GO reviews remain evidence, not acceptance of this revision.
3. Implement the session-bound store and API, then test two clients using the
   same explicit request ID with opposite completion orders; foreign lookups;
   accepted-result failure paths; early handle cancellation; same-client
   overlapping operations; capacity and placement refusals before Open;
   close-waiter cancellation; finite retention; and more than 256 sequential
   operations with delayed stale transport messages.
4. Keep `gwz-py` Phase 6/7 and release activation NO-GO until implementation
   and release reviews close these cases. Do not infer wire-carrier readiness
   from in-process message-contract tests.
