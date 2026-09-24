# GWZ core session — contract design

Date: 2026-09-24. Status: **DRAFT contract design, revision 1; review required; no implementation or activation authority.**

Revision 1 applies the [first remediation plan](GwzCoreSessionDesign-RemPlan.md) as one patch to the revision the [first verdict](GwzCoreSessionDesign-Verdict.md) reviewed (root `e4b8d43`).

This contract gives the shape recommended in [the clean-slate proposals §8](GwzClientCoreTransportProposals.md) exact behaviour, under that document's fixed requirements G1–G11. It pairs with DRAFT paragraphs in gwz-core's [GWZDesign](../gwz-core/dev-docs/GWZDesign.md) and [GWZRequirements](../gwz-core/dev-docs/GWZRequirements.md), as gwz-core's `AGENTS.md` requires before core behaviour expands. It also adds pointers in gwz-py's [package design](../gwz-py/dev-docs/GwzPyDesign.md) and [transport design](../gwz-py/dev-docs/GwzPyTransportDesign.md).

## 1. Scope

**In scope:**
- the session protocol: frames, calls and replies;
- the channel contract and its two adapters;
- the core session host: ownership, admission, workers, cancellation, retention and closure;
- the session context and operation gates, which replace process-global inputs;
- the gwz-py extension boundary;
- the mapping of the unchanged Python API onto calls;
- gwz-cli's use of the session host;
- the wire proof;
- the schema additions and the model-to-wire error mapping.

**Out of scope:**
- client placement, for which only a transport lane is reserved;
- remote deployment;
- connection reuse across operations;
- any change to gwz-transport, the virtual-stream protocol or the public Python API;
- the code changes that remove the process-global state inventoried in §5.7. This contract fixes the rule; the check named there tracks the work.

This contract adopts the proposals' recommended answers to their open decisions, and review may overturn them:
- one transport runtime per operation;
- `operation.cancel` and `operation.release` as service methods, plus `operation.response` and `session.close`;
- channel closure cancels live work;
- close waits for workers within a bound (proposals §8.1);
- these limits, each set at `open` (§9) unless marked fixed:

| Limit | Default |
| --- | --- |
| running operations per session | 8 |
| queued operations | 64 |
| operation table: live operations and terminal submitted records | 128 entries |
| direct-method workers | 8 |
| event log per operation | 4096 events |
| open `diff.output` and `log.output` logs | 64 |
| outstanding calls | 1024 |
| control reserve per channel queue | 64 frames |
| bytes returned by one read | 1 MiB |
| wait held by a read | at most 30 seconds (fixed) |
| frame size | at most 64 MiB (fixed) |
| close wait (`close_wait`) | 60 seconds |

## 2. Parties and ownership

| Party | Where it runs | What it owns |
| --- | --- | --- |
| Channel | between one client and one session host | frames in transit; delivery only |
| Session host | gwz-core, one per channel | every call from receipt to its reply; every operation record from admission to release, eviction or session end; the session context (§5.6); admission, locks and retention |
| Worker | one thread per running operation or direct call, created and owned by the session host | the running operation: its transport runtime, handler and finish, all reached through its operation gate (§5.6) |
| Client bridge | gwz-py's `NativeCoreBridge`, gwz-cli's driver, a test bridge | its outstanding calls and their waiters, and its own view of results; nothing inside core |

Nine rules follow. Every later section applies them.

- **O1 — Receipt transfers ownership.** The session host owns a call from the moment it takes the call's frame off the channel. Until then the frame is in transit, and the client knows only that it sent it.
- **O2 — One reply per call.** The session host sends exactly one reply, a response or an error, for every call it receives. If the session ends first, the channel's closure stands in for the reply to every outstanding call. A frame that reuses an outstanding `call_id` ends the session (§3), so no call ever receives a second reply.
- **O3 — One terminal per operation.** A running operation's terminal outcome is written once, by its worker through its gate, when the handler returns, returns an error or panics. The session host writes a terminal itself only for:
  - an operation that never started: one cancelled while queued, including by close, or one whose worker thread could not be created;
  - an operation whose worker it detached at the close bound (§8).
- **O4 — Intents are recorded, not enacted.**
  - Cancel cancels the operation's token (O7). The operation's gate and its transport request observe the token.
  - Release discards a terminal record.
  - Close starts session shutdown (§8).
  - None of these writes a running operation's terminal.
- **O5 — Only owners' timers act.** Core's existing transport deadlines act inside a worker's runtime. The session host's own timers are the bounded read wait and the close bound. The close bound ends the session; it never decides the outcome of an operation that is still attached. A client-side timeout only stops waiting, or sends a cancel.
- **O6 — Nothing but frames crosses.** Client and session host share no memory, lock, callback or thread.
- **O7 — Handles are created before the action.** The party that will use a control handle creates it before the action starts and passes it in. Downstream work attaches to it. Nothing on the session path returns a control handle after the action has begun.
  - The client chooses each `call_id` before sending, and `operation.cancel` accepts it, so a client never waits for an `operation_id` before it can cancel.
  - The bridge registers each call's waiter under its `call_id` before calling `send`, so a fast reply always finds its waiter.
  - At admission, the session host creates the operation's record, its cancellation token and its gate, before it replies `accepted` and before any worker exists. The worker receives the token and the gate as arguments.
- **O8 — Resources are reached through a gate.** A worker never holds session state or session resources directly. Its gate knows the operation's token and the session's lifetime. After cancellation, effectful requests through it fail with `Cancelled`. After revocation, reports through it are ignored as well (§5.6).
- **O9 — Context, not globals.** Everything session-relevant travels in the session context, created at `open` and reached through the gate. Code on the session path reads no environment variable and no process-global mutable state. The process-global state that remains is inventoried in §5.7, and a check in the gwz-core and gwz-py test runs keeps new state out.

## 3. The channel

- **Scope and guarantees.** One channel carries one session. It is bidirectional, reliable and ordered in each direction while open, bounded, and it reports closure to both ends. These are the carrier guarantees gwz-transport already states (proposals G6).
- **Frames.** A frame is a tag byte followed by a deterministic-CBOR body, taut-shape's frame layout. No frame may exceed 64 MiB.

  | Tag | Meaning |
  | --- | --- |
  | 1 | `SessionCall` |
  | 2 | `SessionReply` |
  | 3 | `SessionError` |
  | 16–31 | reserved for a transport lane (client placement); unused by this contract |

- **Protocol errors** end the session as in §8. They are:
  - any other tag;
  - a body that does not decode;
  - a frame over the size limit;
  - a `SessionCall` whose `call_id` belongs to a call that is still outstanding.

  No reply is attributed to the frame that caused a protocol error.
- **Adapters.** There are two, and both carry the same frames.
  - **In-process:** two bounded queues of frames inside the client's process. Each holds the outstanding-call limit plus a control reserve of 64 frames.
    - The client's `send(frame)` never blocks. On a full queue it fails with `transport_session_full`; it never drops a frame.
    - Control calls, `operation.cancel` and `session.close`, use the control reserve. The outstanding-call limit never refuses them.
    - `recv()` blocks until a frame arrives or the session has ended.
    - The bridge counts a call as outstanding until its reply has been taken off the reply queue, and never exceeds either bound (§10). Calls held by the host plus replies waiting in the queue therefore never exceed the queue's capacity. The host's reply queue cannot overflow, and only a client that exceeds its own bounds meets a full call queue.
  - **Byte stream:** stdin/stdout, a socket or SSH. Each frame is prefixed with its length as a little-endian `u32`, exactly as the taut-shape interop tool frames it.
- **Volume is bounded on both sides, and Git never waits on the channel.**
  - The session host only ever sends replies. What it sends is bounded by outstanding calls, and each read's reply by the read's byte limit (1 MiB by default).
  - Clients bound their own outstanding calls, and the session host refuses calls beyond its limit (§5.1).
  - Events are retained inside core and read by calls (§6). Nothing streams them onto the channel.

## 4. The protocol

### 4.1 Frame bodies

These messages are appended to the GWZ Taut schema:

```text
SessionCall  { call_id: u64 (1), method: str (2), mode: SessionMode (3, optional; default unary),
               request: bytes (4), log_verb: LogVerb (5, optional; default read) }
SessionReply { call_id: u64 (1), response: bytes (2) }
SessionError { call_id: u64 (1), error: GwzError (2), response_meta: ResponseMeta (3, optional) }
SessionMode  = unary (0) | submit (1)
LogVerb      = read (0) | end_stream (1)
```

- `request` and `response` are the Taut-encoded messages that the called method declares. Calls to the three log methods carry taut-shape's messages instead (§4.2). `log_verb` applies only to those methods; on any other method it is an `invalid_request`.
- `SessionError.error` is the existing Taut `GwzError`: code, message, member ID, member path, detail, target kind and record context. `response_meta` is the existing `ResponseMeta`, present whenever the failing call produced one. The bridge maps both onto `GwzBridgeError` exactly as today's native error does, with `machine_message` taken from `GwzError.message` (§10).
- `call_id` is chosen by the client before it sends (O7). It is unique and increasing within the session. Reusing the `call_id` of a call still outstanding is a protocol error (§3).

### 4.2 Methods

**Direct methods** reply with their final response, whatever `mode` says:
- `status`, `ls`, `resolve_forall_targets`, `list_snapshots`, `diff`, `log`;
- `transport_capabilities`, `remote_identity`;
- `configure_transport_runtime`. It sets this session's transport timeouts (§5.6) for operations admitted after its reply. It never touches another session or a running operation. Ordinary builds keep today's process-wide behaviour (§5.8).

Direct methods behave as unary calls. They run on the session's direct workers (§5.2), which do not count against the running limit, and `operation.cancel` may target their `call_id`.

**Every other existing method is an operation method,** except the log-read and result methods below. `mode` chooses its reply:
- **`unary`:** the reply is the method's final response, sent when the operation ends. The record is discarded once the reply is sent.
- **`submit`:** the reply is the method's `accepted` response (`AggregateStatus.accepted`, `ResponseMeta.operation_id`). It is sent as soon as the session host admits the operation, whether the operation starts running or waits in the queue. Accepted therefore means that core owns the operation, and that its record, token and gate exist (O7). It does not mean that Git work has started. The record is kept until it is released, evicted or the session ends.

**Log reads** cover `events.subscribe`, `diff.output` and `log.output`.
- **Read** (`log_verb = read`): the `request` is taut-shape's `LogReadRequest`. Its `log_id` carries the method's handle: the operation ID for `events.subscribe`, the log ID for `diff.output` and `log.output`. Its `stream_id` names the reader, as `diff.output`'s stream ID does today. It also carries a cursor, optional record and byte limits (1 MiB by default), and a wait of at most 30 seconds.
- The reply is taut-shape's `LogReadResponse`: the records after the cursor, the next cursor, and the log's state, including whether it is complete.
- The session host holds a read until a record is available, the log completes, the wait elapses or the session ends. A read that returns no records and an incomplete log is simply read again.
- **End stream** (`log_verb = end_stream`): the `request` is taut-shape's `LogEndStream`. It ends a `diff.output` reader stream. On `log.output` it also releases the log and deletes its spool, which is today's `log_output_release`, and repeating it is harmless.
- These logs belong to the session (§5.4).

**Results:** `operation.result(operation_id)` keeps its existing declaration and replies with the `OperationResult` once the operation is terminal.

**New methods, all append-only:**
- **`operation.response(operation_id)`** replies with the operation method's final response once terminal. It generalizes today's merge-only response lookup.
- **`operation.cancel(target)`** takes the `call_id` of a unary or submit call, or an `operation_id`.
  - A `call_id` names the operation or direct call its call created, for as long as that record exists. A submit's `call_id` therefore stays usable after its `accepted` reply, and a cancel may arrive before that reply.
  - It replies once the target is terminal, with that operation's cleanup report (the fields of today's `TransportCleanup`).
  - Cancelling an operation that is already terminal replies at once with its retained report.
  - A target that the session never admitted gets `operation_not_found`. Because call IDs increase, the host can tell an unseen `call_id` from a spent one.
  - A released or evicted record, or a unary call that has already been answered, gets `operation_expired`. The bridge treats `operation_expired` as "already finished".
  - A `call_id` of a read, result or control call gets `invalid_request`.
- **`operation.release(operation_id)`** discards a terminal record and replies. A live operation gets `open_operation`. A repeat, or an evicted record, gets `operation_expired`.
- **`session.close()`** replies after shutdown (§8) with the close report, which has the fields of today's `TransportCleanup`. The session host then closes the channel.

**Error codes.** Two codes are appended to `GwzErrorCode` after `transport_record_limit` (74): `transport_session_full` (75) and `operation_expired` (76). None is redefined. The frames also use the existing `invalid_request`, `operation_not_found`, `open_operation`, `internal_error` and `cancelled` (73).
- `protocol/convert.rs` maps the model's `Cancelled`, `TransportSessionFull` and `OperationExpired` to `cancelled`, `transport_session_full` and `operation_expired`, instead of `io_error`.
- The session host never produces `TransportCapacityConflict`, since each operation has its own runtime, so its mapping stays.

### 4.3 Identity

- `call_id` is chosen by the client before sending. It correlates replies and names a call for `operation.cancel`.
- `operation_id` is assigned by the session host at admission. It names an admitted operation and appears in accepted responses, events and results.
- `RequestMeta.request_id` keeps its existing meaning: the caller's correlation label, and the ID the operation's transport request registers under. It is unique among a session's live operations: a request whose `request_id` matches a live operation's is refused with `invalid_request` before any effect.

## 5. The session host

### 5.1 Admission

**Resolution first.**
- For a method that acts on an existing workspace, the session host resolves that workspace's root before admission. A method that creates a workspace is keyed by its target path instead.
- Roots are resolved on the host's admission thread, never on the thread that reads the channel.
- A request that fails validation or resolution is refused before it becomes an operation.

**Records, tokens and gates are created at admission (O7).** Admission creates the operation's record, its cancellation token and its gate in one step, before the host replies `accepted` and before any worker exists. A direct call gets a token and a gate the same way. It takes no operation-table entry, and its record lasts until its reply is sent.

**Classes.** Every method belongs to exactly one class. The class table lives in core, and any method missing from it counts as W.

| Class | Methods | May run alongside |
| --- | --- | --- |
| R, reads and session settings | the direct methods of §4.2 | anything; they run on direct workers |
| N, network ref updates | `fetch`, `push` | R and N operations, except that at most one push runs per workspace at a time |
| W, workspace writes | every other method | only R operations in the same workspace |

**Serialization.**
- Each fetch or push member step, meaning that member's network exchange and ref update, takes the session's lock for that workspace and member. Two operations therefore never update the same member's refs at once. The lock manager blocks rather than refusing, and recovers from poisoning, so one panic cannot fail later operations.
- Push also takes the existing cross-process workspace mutator lock, as branch and stash mutations do. That lock is a try-lock. Because a session never starts a second push on a workspace while one runs, "already held" can come only from another process or another session. A later push on the same workspace waits in the queue.

**Limits.**
- At most 8 operations run at once per session.
- An operation that can't start yet waits in one FIFO queue per session, holding at most 64 entries. This happens when the running limit is reached or its class is blocked. N and W operations of the same workspace start in queue order.
- R calls wait only for a free direct worker, in their own FIFO order.
- A request that arrives with the queue full is refused with `transport_session_full` before any effect.
- Every admitted operation takes an entry in the operation table (§5.4). If the table is full, the host evicts the oldest terminal record whose result has already been delivered. If there is none, the request is refused with `transport_session_full` before any effect.
- More than 1024 outstanding calls are refused the same way. Control calls do not count against that limit. At most 64 may be outstanding, and one beyond that is refused the same way; the bridge never sends one (§10).

### 5.2 Workers

- An operation gets its worker thread when it starts running. If the thread cannot be created, the session host settles the operation as `Failed` (`internal_error`) before it starts, with no effect.
- Direct calls run on a pool of 8 direct workers, separate from the running limit. `status` and `transport_capabilities` therefore never wait behind eight long operations.
- The worker runs the shared dispatch. That is gwz-cli's execution path (`execute_invocation`), moved into gwz-core as the shared dispatch that gwz-py's design already asks for. The shared dispatch also absorbs gwz-cli's diff, log and hook paths, which gwz-cli dispatches outside `execute_invocation` today.
- **Transport entry.** For a transport-scope request in a build that has the transport, the worker calls a session variant of `with_local_transport` through its gate. The gate supplies three inputs: the operation's token, the endpoint environment from the session context, and the session's timeouts. It refuses the call with `Cancelled` once the token is cancelled. The entry then:
  1. builds the operation's own runtime from the session's endpoint environment and timeouts;
  2. registers the operation's single request with the token, through a new request constructor that takes the caller's token;
  3. runs the handler;
  4. finishes the request;
  5. shuts the runtime down.

  When the request registers, it attaches its cancellation to the token. If the token is already cancelled, registration fails with `Cancelled` and the handler never runs. The token is the only cancellation authority. The existing `TransportRequest::cancellation_handle()` stays for the legacy path only.
- Otherwise the handler runs with the default backend.
- The worker also passes the token to the handler's context, so handlers can check it at safe points later without another API change.
- The worker appends the handler's events to the operation's log through its gate (§6). It then reports exactly one terminal through its gate:
  - `Completed`, with the response, if the handler succeeded;
  - `Cancelled` (`cancelled`, 73), if it did not succeed and the token had been cancelled;
  - `Failed` otherwise.
- **Panics.** The worker catches a handler panic first. It then runs finish and shutdown under their own `catch_unwind`, as today's extension does. It reports `Failed` (`internal_error`) with unconfirmed cleanup. The transport entry never finishes from `Drop` while unwinding, so a second panic cannot abort the process. The session carries on.

### 5.3 Cancellation

- **The token is the only authority.** `operation.cancel` and close cancel the operation's token. Nothing else does.
- **A queued operation** is settled `Cancelled` by the session host before it starts, with no effect, and the cancel is answered. The host starts queued operations and settles queued cancels under the same table lock, so an operation is either settled before it starts or started with its token.
- **A running operation:**
  - its transport request fails its network I/O with `Cancelled`;
  - its gate refuses new effectful requests with `Cancelled`, so every gate crossing is a cancellation point, local handlers included;
  - the cancel's reply waits for the worker's terminal;
  - a handler that has succeeded keeps its `Completed` terminal;
  - a local handler observes cancellation only at gate crossings. Once it holds what it needs, such as its member and workspace locks, it runs to its end and reports its own outcome.
- **Unary and submitted calls** are cancelled by their `call_id` as well as by `operation_id` (§4.2).
- **Ordinary builds** have no transport cancellation (§5.8).

### 5.4 Retention

- **The operation table.** Each session has one operation table, 128 entries by default. It holds live operations and terminal submitted records: at most 8 running plus 64 queued are live, and the rest hold terminal records.
  - When admission needs an entry and the table is full, the host evicts the oldest terminal record whose result has already been delivered once, by `operation.result` or `operation.response`.
  - Live records, and terminal records whose result has never been delivered, are never evicted. If nothing can be evicted, the request is refused with `transport_session_full` before any effect.
  - Later calls on an evicted record get `operation_expired`.
- **A submitted record** outlives its terminal until it is released, evicted or the session ends.
- **A unary record** is discarded once its reply is sent.
- **An operation's event log** follows GWZDesign's event-buffer rule unchanged. It is a ring of at most 4096 events. On overflow, older incremental events are dropped and a reset event is kept, followed by later events, so readers know the history is incomplete. The final `OperationResult` is kept separately and is never dropped. Today's `OperationRuntime` clears the whole buffer on overflow, so it must be brought to this rule when it is reused.
- **`diff.output` and `log.output` logs** belong to the session that created them and live in its context (§5.6).
  - At most 64 are open at once. The next open is refused with `transport_session_full` before any effect.
  - They are released, and their spools deleted, when the session ends.
  - Another session cannot read them: it gets `operation_not_found`.

### 5.5 Timeouts

Workers run under core's existing transport deadlines, such as admission, bootstrap and cleanup, using the timeouts in the session context. The session host adds only its own two timers: the bounded wait of a held read, and the close bound (§8). No timer decides an attached operation's outcome from outside its worker.

### 5.6 The session context and operation gates

**The session context** is created by `open(options)` on the calling thread. It holds:
- the endpoint environment, captured once (below);
- the transport timeouts;
- the member lock manager;
- the operation table;
- the `diff.output` and `log.output` registries;
- the limits of §1.

Every operation reaches the context only through its gate. Code on the session path reads no environment variable and no process-global mutable state (O9).

**The endpoint environment is captured by the driver and passed in.**
- The driver captures the process environment once, at its edge, and passes it to `open` in `options`. gwz-cli captures it at startup. The Python bridge takes it from `os.environ` when it opens the session.
- Core and the extension never read the environment themselves.
- The snapshot is kept as captured. Each network operation derives its endpoint configuration from it: the SSH home and agent socket, TLS roots, proxies, and the `gh` environment.
- An invalid proxy or CA setting therefore refuses only network operations, and local-only operations never parse it, as gwz-py's transport design already requires.
- A later change to the process environment does not change a session's credentials or trust context.

**Timeouts.** `configure_transport_runtime` changes the context's timeouts for operations admitted after its reply. It never touches another session or a running operation.

**The host context.** Some budgets are shared by the sessions that one embedder runs:
- the SSH setup supervisor, with its helper and cleanup budgets;
- the HTTPS helper slots.

They live in a host context. An embedder that runs several sessions may pass one host context to each `open` to share them; otherwise each session gets its own. No static holds a host context. HTTPS helper cleanup is owned by the endpoint's `AuthOwner`, never by a process-wide registry.

**Operation gates.** The session host creates each operation's gate at admission, together with its token (O7). A worker reaches these only through its gate:
- the operation's event log and its terminal report;
- member locks and the workspace mutator lock;
- transport runtime construction, which uses the context's endpoint environment and timeouts;
- log registrations.

The gate has two further states:
- **After cancellation** of the token, the gate refuses new effectful requests (a lock, a transport open, a log registration) with `Cancelled`.
- **After revocation** at the close bound (§8), the gate also ignores reports (events and terminals). A detached worker therefore cannot touch session state, and it unwinds at its next gated step. Only work already inside a call, such as a libgit2 transfer or a file write in progress, can finish that call.

### 5.7 Process-global state

The authoritative inventory is two allowlists: gwz-core's `scripts/checks/process_globals_allowlist.json` and gwz-py's `scripts/process_globals_allowlist.json`. gwz-core's `check_process_globals.py` runs in gwz-core's `scripts/run_tests.py` and its CI boundary job, and in gwz-py's `test_process_globals.py`. It fails on any unlisted static, thread-local, environment read, libgit2 global option or process-wide hook, and on any listed item that no longer exists, so the lists only shrink. Code compiled only under `cfg(test)` is outside the check. The contract's O9 is met when neither allowlist holds a `debt` entry on the session path.

| Item | Disposition |
| --- | --- |
| `TIMEOUT_STATE` and libgit2's timeout options (`transport_support.rs`) | Transport builds use the session context's timeouts. Ordinary builds keep libgit2's process-wide options (§5.8). |
| The transport host's environment reads: `SshEndpointConfig::from_environment`, `environment_config` and the transport binding | Replaced by the session's captured endpoint environment. |
| gwz-py's process-global diff and log registries (`diff_logs.rs`, `log_outputs.rs`) and legacy operation store (`operations.rs`) | Become members of the session context. |
| gwz-py's thread-local backend, operation-ID and current-session scoping (`shims.rs`, `operations.rs`, `transport_session.rs`) | Replaced by explicit parameters taken from the operation's gate. |
| The SSH setup supervisor and its helper and cleanup budgets (`agent_job.rs`: `HUB`, `INIT`, `COUNT`, `CLEANUPS`) | Become members of the host context. |
| The HTTPS helper orphan registry and slot semaphore (`https_auth.rs`: `ORPHANS`, `ORPHAN_REAPING`, `SLOTS`) | Cleanup is owned by the endpoint's `AuthOwner`. The slot budget moves into the host context. |
| The other ambient reads in core: `with_local_transport`'s whole-environment snapshot (`local_command.rs`), `~/` identity resolution (`identity.rs`) and repo-inspect's `Environment::Process` | Taken from the environment the driver passes in. |
| gwz-py's working-directory reads (`lib.rs`, `transport_session.rs`) | The Python API passes the start directory with each call (§10). |
| Test hooks compiled into production builds (`V1_PRESERVATION_IMAGE_CAPTURES`, gwz-py's `GWZ_PY_TEST_EVENT_DELAY_MS`) | Gated with `cfg(test)`, or moved behind a test-only hook. |
| ID counters (the transport session serial, temp-file sequences), and fault hooks compiled only under `cfg(test)` | Kept. They carry no session-relevant state. The counters are `permanent` allowlist entries. |

### 5.8 Ordinary builds

Without the transport, handlers run with the default backend.
- Cancelling a running network operation waits for it to finish, because libgit2's own transfers cannot be cancelled.
- Close waits up to the close bound.
- `transport_capabilities` reports whether cancellation is supported, in a field appended to its response (§13), so the bridge promises no promptness it cannot deliver.
- `configure_transport_runtime` keeps today's process-wide behaviour. libgit2's server timeouts are process-wide: they can be set before the first backend exists and are refused afterwards. This contract makes no session-isolation claim for them.

## 6. Events and results

- Handlers append events to their operation's log through the gate as they occur. Readers pull them with `events.subscribe` read calls.
- Any number of readers, each with its own cursor, may read one log.
- Nothing is pushed onto the channel, so a slow or absent reader never slows Git.
- Results and final responses are read with `operation.result` and `operation.response`.
- **How terminals appear in `OperationResult`:**
  - A `Cancelled` terminal has `aggregate_status = failed` and `errors[0] = GwzError{code: cancelled}`. If the handler failed for its own reason after the token was cancelled, that error follows as the second entry, so a real cause is never masked. Member rows are the planned rows, marked skipped; a record that never planned has none.
  - A `Failed (internal_error)` terminal has `aggregate_status = failed` and `errors[0] = GwzError{code: internal_error}`.
  - An operation detached at the close bound is reported `Cancelled`, with its cleanup unconfirmed (§8).

## 7. Sequences

A submitted operation, from `submit` through release:

```text
client                                   session host                           worker
SessionCall(fetch, submit) ───────────►  admit: record, token, gate;
                                         queue or start ───────────────────────► runtime, handler
◄────── SessionReply(accepted, op_id)
SessionCall(events.subscribe read) ────►  hold until events ◄─────────────────── append events (gate)
◄────── SessionReply(records, cursor)
SessionCall(operation.result) ─────────►  hold until terminal ◄───────────────── terminal (gate, once)
◄────── SessionReply(OperationResult)
SessionCall(operation.release) ────────►  discard record
◄────── SessionReply(ok)
```

A client may cancel a submit by its `call_id` before the `accepted` reply arrives. The host handles frames in order, so the cancel finds the admitted operation and its token.

A unary call is one `SessionCall(fetch, unary)` followed by one reply when the operation ends. If the client stops waiting, the bridge sends `operation.cancel` for that `call_id` and waits for its reply, as the Python client cancels and joins today. If the unary reply won the race, the cancel gets `operation_expired`, which the bridge treats as "already finished".

## 8. Closure

**`session.close()`.** The session host:
1. refuses every later call other than `session.close` with `invalid_request` ("session closing");
2. settles every queued operation, and every direct call still waiting for a worker, `Cancelled`, with no effect;
3. cancels the token of every running operation and direct call;
4. waits for their workers to report terminals, up to `close_wait` (60 seconds by default, set at `open`);
5. revokes the gate of every worker still running at the bound, and settles its operation `Cancelled` with cleanup unconfirmed;
6. answers every outstanding call: held reads return whatever records remain and completion, and results and cancels return terminals;
7. releases the session's `diff.output` and `log.output` logs and deletes their spools;
8. answers the close with the close report, which sums the operations' cleanup reports and counts each detached worker in `pending_local_work`, with `peer_cleanup_confirmed` false when any worker was detached;
9. closes the channel.

A session that ran no network operation reports `(0, false)`, as today, because no peer cleanup occurred. Repeated `session.close()` calls, accepted until the channel closes, receive the same report. After the channel has closed, the bridge's `close()` returns its retained report.

**Close waits for workers, within a bound.** Network work fails promptly once its token is cancelled, in builds that have the transport. A local handler stops at its next gate crossing, or finishes its current work first. A worker still running at the bound is detached: its gate is revoked, so it cannot touch session state, and it unwinds at its next gated step. A detached worker can still be finishing a call that writes the workspace, so close guarantees that no core thread is writing the workspace only when `pending_local_work` is 0. The close report says how many workers were detached. A client may stop waiting without stopping shutdown.

**Channel closure without `session.close()`** covers a client that dropped its session, a failed adapter and a protocol error. The session host performs steps 2–5 and 7, then ends. No replies are possible.

## 9. The gwz-py extension boundary

The extension exposes one session object with four operations:

| Operation | Behaviour |
| --- | --- |
| `open(options)` | creates the session host, its session context and its in-process channel. `options` carry the limits of §1, the endpoint environment captured by the bridge (§5.6), and optionally a host context to share. |
| `send(frame)` | non-blocking. Fails with `transport_session_full` on a full queue, and fails once the session has ended. |
| `recv()` | blocks with the GIL released; returns a frame, or `None` once the session has ended |
| `close()`, or dropping the object | closes the channel |

Nothing else crosses: no Python callback into Rust, no Rust thread running Python code, no Python thread lent to core work, and no synchronous read of core state. The extension reads neither the environment nor the process's working directory.

Today's other entry points leave the bridge's path and are not public API:
- `call`, `submit`, `wait_events`, `operation_result`, `merge_operation_response`, `cancel_operation`, `release_operation` and `reserve_operation`;
- the module-level diff and log functions;
- the module-level operation store.

Whether the extension keeps them for compatibility is an implementation choice. Any it keeps are outside the session path, and the §5.7 check still covers them.

## 10. The Python bridge (public API unchanged)

`Client` and `CoreBridge` keep their shape (G11). `NativeCoreBridge` implements each bridge method with session calls:

| `CoreBridge` method | Session calls |
| --- | --- |
| `call(method, …)` | `SessionCall(method, unary)`; the reply is the final response |
| `submit(method, …)` | `SessionCall(method, submit)`; the reply is the accepted response |
| `subscribe_events(operation_id)` | `events.subscribe` reads from cursor 0 until complete |
| `operation_result(operation_id)` | `operation.result` |
| `merge_operation_response(operation_id)` | `operation.response` |
| `cancel_operation(operation_id)` | `operation.cancel`; the reply is the `TransportCleanup` |
| `release_operation(operation_id)` | `operation.release` |
| `diff_log_read`, `diff_log_end_stream` | `diff.output` reads and end stream |
| `log_output_read`, `log_output_release` | `log.output` reads, and end stream to release |
| `close()` | `session.close`; the reply is the close report |

**Calls.**
- The bridge allocates each `call_id`, registers the call's waiter under it and sends the frame under one lock, so its IDs reach the host in increasing order and a fast reply always finds its waiter (O7).
- Every request carries the caller's start directory in `InvocationContext.caller_cwd`, as the Python client already sends it.
- A `SessionError` becomes a `GwzBridgeError` exactly as today's native error does: code, member ID, member path, target kind, detail and record context from `GwzError`, `machine_message` from `GwzError.message`, and `response_meta` from `ResponseMeta`.

**The pump.** One daemon thread per session calls `recv()`.
- For each reply it completes the waiting future on the event loop that issued the call, using `call_soon_threadsafe`. If that loop has closed, including a close that races the call, the reply is dropped and the pump continues.
- A reply payload that does not decode as its call's expected response fails only that call, with a protocol error.
- When `recv()` returns `None`, every outstanding call fails with a typed closed-session error.
- The pump never exits silently. Any exception in its loop, or a frame whose tag or CBOR body does not decode:
  1. closes the channel;
  2. marks the bridge closed;
  3. fails every outstanding call with the typed closed-session error.

  Later sends then fail.

**Waiting and bounds.**
- Every bridge method that waits is async (G7).
- The bridge bounds outstanding calls per session with two thread-safe counters: ordinary calls up to the outstanding-call limit, and control calls up to the control reserve. A call that must wait for a slot waits on its own event loop.
- A control call never takes an ordinary slot, so a shielded cancel never waits behind the calls it is cancelling.
- A second cancellation of the same target joins the cancel already in flight, as today. At most 16 targets can be running at once (8 operations and 8 direct calls), and cancels of queued or terminal targets are answered at once, so the control reserve is not a practical limit.

**Task cancellation.**
- Cancelling a task awaiting a unary call sends `operation.cancel` for that `call_id`. The task waits for the cancel's reply, shielded against further cancellation, and then propagates. This is today's cancel-and-join. `operation_expired` means the call had already finished, and raises nothing.
- Cancelling a task that is iterating events or awaiting a read stops only that read, and its reply is dropped.

**Interpreter exit.** The bridge registers a finalizer that closes the channel and joins the pump before interpreter finalization, waiting at most the close bound. `close()` joins the pump too. Exiting with an open session therefore waits up to the close bound for running operations, which keeps most of today's behaviour of joining worker threads at exit.

Custom bridges are unaffected.

## 11. gwz-cli

In a later phase, the CLI opens an in-process session, passing the environment it captured at startup, and sends its request:
- as a submit followed by event reads whenever its renderer consumes events, which includes JSONL output and the progress line in human mode on a terminal;
- as a unary call otherwise.

It renders replies exactly as it renders results today. Its main thread may block on `recv()`, since the CLI is an application rather than a library. User-visible behaviour does not change. This satisfies GWZDesign's CLI driver design.

## 12. Proof that the boundary is a wire (G3)

- **Host binary:** a development-only target in gwz-core serves one session over stdin/stdout through the byte-stream adapter. It captures its own process environment once at start, as a driver does. It is not a published binary. G10 still holds, because no wheel contains a `gwz` executable.
- **Test bridge:** `StreamCoreBridge`, a test bridge in gwz-py, sends the same frames to that binary over asyncio subprocess pipes. An asyncio reader task takes the place of the pump thread.
- **CI:** CI runs gwz-py's Client-level and native-integration tests twice, once through `NativeCoreBridge` and once through `StreamCoreBridge`. Any difference between the two runs is a defect. Bridge-internal tests are rewritten against a fake channel. The implementation plan lists the assertions that change, such as a foreign cancel now reporting `operation_not_found`.
- **What the two runs cannot show:** process-global state shared by the in-process path, because the stream run has one session per host process. The §5.7 check covers that.

## 13. Schema additions (append-only, G8)

- `SessionCall`, `SessionReply`, `SessionError`, `SessionMode` and `LogVerb`, with the tag registry of §3. `SessionError` carries the existing `GwzError` and `ResponseMeta`.
- Service methods `operation.response`, `operation.cancel`, `operation.release` and `session.close`, with GWZ request and reply messages for them.
- `operation.result` keeps its existing declaration.
- Log reads and end-stream calls use taut-shape's `LogReadRequest`, `LogReadResponse` and `LogEndStream`. No GWZ read messages are added.
- Two error codes are appended to `GwzErrorCode`: `transport_session_full` (75) and `operation_expired` (76). None is redefined.
- `protocol/convert.rs` maps three model codes to their own wire members (§4.2).
- One field is appended to `TransportCapabilitiesResponse`: `cancellation` (3, bool, optional), true when a running network operation can be cancelled (§5.8).
- No other change to existing messages, and no change to gwz-transport envelopes or the virtual-stream protocol.

## 14. Relationship to existing documents

- **GWZDesign "Operation Runtime".** This session host is that runtime, extended with:
  - cancel, release, response and close;
  - admission classes;
  - per-operation transport runtimes.

  Its V0 synchronous `submit`/`subscribe`/`wait` API becomes this protocol. Its event-buffer rule is adopted unchanged (§5.4).
- **GWZDesign "CLI Driver Design"** is met by §11.
- **GWZDesign's process-wide native timeout paragraph** ("Native connection/read timeout configuration is process-wide") still governs ordinary builds (§5.8). In transport builds, the session context's timeouts replace it.
- **gwz-core's SSH agent design** ([GwzRemoteTransportSshAgentDesign](../gwz-core/dev-docs/GwzRemoteTransportSshAgentDesign.md)) described the setup supervisor as "process-local infrastructure" with a fixed process-wide cap of 64 helpers. The supervisor and the cap become members of the host context (§5.6). The cap's value and its accounting are unchanged.
- **gwz-py's package design** (Bridge Contract):
  - These statements are replaced by §9's four operations: "The native implementation should … accept encoded taut request bytes plus method/message metadata for `GwzCore` service calls, dispatch into `gwz-core`, and return encoded taut response bytes", and the native build path's list of exposed calls, submission, event subscription and result lookup.
  - "move toward a shared `gwz-core` dispatch API" is realized by §5.2.
  - "release the GIL around blocking Rust handlers … and deliver streaming events back through bounded async-safe queues" becomes: handlers run on core's threads, the only blocking call, `recv()`, releases the GIL, and events are pulled by read calls.
  - The CBOR ABI rule is unchanged: frames are deterministic CBOR.
- **gwz-py's transport design, for the Python Client.** Superseded:
  - §1's single long-lived `TransportRuntime` inside the native `TransportSession`, and §2's lazy host lifecycle (Uninitialized through Closed) with its one host and pool across operations;
  - §2's prohibition on calling `with_local_transport` per call;
  - §3's single-active-operation refusal, the Python network `asyncio` lock, and the per-request cancellation handle. §3's frozen core addition `TransportRequest::cancellation_handle()` remains for the legacy path only;
  - §3's use of `asyncio.to_thread` and GIL detachment around `call`, `submit` and event waits, which the pump replaces;
  - §4's same-generation capability clause, since capability checks become ordinary `transport_capabilities` calls;
  - §4's cancel error codes (`InvalidRequest` for unknown, foreign, older completed or expired IDs) and its "latest completed cancellation snapshot" rule. These become `operation_not_found`, `operation_expired` and the retained report of each terminal record;
  - §4's statement that `configure_transport_runtime` keeps its existing server-timeout setting, in transport builds (§5.6);
  - §5's physical-session-reuse test, the construction and admission barrier tests, and the overlapping-direct-native-call and Python-lock tests.
- **Surviving from gwz-py's transport design:**
  - §2's environment stability and its local-only isolation from invalid proxy or CA settings, kept by §5.6;
  - the file-identity preflight;
  - the `TransportCleanup` shape and close's retained report;
  - `Client.meta(max_retries)`;
  - gh-only HTTPS and sanitisation of Python-visible errors;
  - §5's release, registry-pin and credential-hygiene checks.
- **GwzV110Plan S6.3.** Its test becomes "two overlapping Python operations complete independently on one Client, each on its own runtime". The plan text is amended when this contract is accepted.
- **Reference documents** are updated at implementation: gwz-core's MessageCatalog, ErrorCatalog, Protocol and Reference, gwz-cli's MachineOutput, and gwz-py's release notes.
- **Unchanged:** the remote transport, placement and sequenced-stream designs. Client placement remains future work on the reserved transport lane. The placement guide's premise of keeping a runtime alive across operations to reuse connections needs amending when client placement is scheduled.

## 15. Verification required after acceptance

Assertions check typed fields and codes only.

1. **One reply per call.**
   - Every call receives exactly one reply.
   - A call whose client stops waiting still completes, and its reply is dropped.
   - A stream-bridge test sends a duplicate outstanding `call_id`: the session ends, and the original call fails with the closed-session error, never `invalid_request`.
2. **Error codes.**
   - A `SessionError` carrying each code this contract names survives a round trip through the Rust and Python projections as the same member.
   - A queue-full refusal reads `transport_session_full` on both bridges.
   - A queued operation cancelled by close reads `errors[0].code == cancelled`.
   - A cancel racing its unary reply raises nothing.
3. **Structured errors.** A member-scoped model error and a merge-record error, raised through both bridges, carry `member_id`, `target_kind`, `record_context` and `response_meta.request_id` equal to today's in-process values.
4. **Concurrency and admission.**
   - Eight overlapping fetch and push operations on one Client complete independently. A ninth waits in the queue and then runs.
   - With 64 operations queued, the next request is refused with `transport_session_full` and has no effect.
   - Two pushes on one workspace in one session complete one after the other, with no `UnsupportedOperation`. Fetch and push run concurrently, and so do two fetches.
   - A W operation excludes N and W operations on its workspace, while R calls run alongside it.
   - Two fetches touching one member serialize that member's step.
   - A method missing from the table is treated as W.
   - `status` and `transport_capabilities` answer while eight long operations run.
   - A second live operation with the same `request_id` is refused with `invalid_request` before any effect.
5. **Cancellation.**
   - Core test: an operation blocked opening a stream to a latch-held endpoint is cancelled through its token. The cancel returns within the admission deadline with a cleanup report, and the handler's I/O fails with `Cancelled`.
   - Core test: the token is cancelled while the runtime is being built. Registration fails with `Cancelled`, no handler runs, and the operation settles `Cancelled`.
   - Protocol test: a submit is cancelled by its `call_id` before its accepted reply arrives, and the operation settles `Cancelled`.
   - A queued operation becomes `Cancelled` with no effect.
   - Success racing a cancel stays `Completed`.
   - A local handler past its last gate crossing runs to completion and reports its own outcome.
   - A cancelled queued submit and a cancelled running fetch show `aggregate_status = failed` and `errors[0].code == cancelled` on both bridges.
6. **Panics.**
   - A handler panic yields `Failed` (`internal_error`), and the next call succeeds.
   - A fault-injected panic in finish after a handler panic yields `Failed`; the next call succeeds and the process stays alive.
7. **Retention.**
   - 72 submitted operations run and complete, and every result is still readable.
   - With the table full of unread terminal records, a new request is refused before any effect. After results are read or released, admission resumes.
   - Release refuses live operations with `open_operation`.
   - An event log that overflows keeps its reset event and later events, and the result stays readable.
   - With 64 logs open, the next open is refused. Session end releases them and leaves the spool directory empty. Another session reading one of their IDs gets `operation_not_found`.
8. **Session context.**
   - Two sessions in one process: A sets a zero timeout, and B's next operation keeps B's own deadline.
   - After `open`, changing `HOME`, `SSH_AUTH_SOCK` or `GIT_SSL_CAINFO` leaves a later operation's endpoint configuration equal to the one captured at open.
   - With an invalid proxy in the captured environment, local-only operations succeed and a network operation is refused.
   - `check_process_globals.py` passes in gwz-core and gwz-py, and fails on any unlisted item.
9. **Closure.**
   - With a handler blocked on a test latch, `session.close()` answers within the bound, with `pending_local_work == 1` and unconfirmed cleanup.
   - Releasing the latch afterwards: the worker's events and terminal are ignored, its next member-lock or transport request fails with `Cancelled`, and session state is unchanged.
   - Repeated close returns the same report, including a close issued after the channel has closed.
   - The byte-stream host closes its channel.
   - Dropping the channel without close performs the same shutdown.
10. **Channel.**
    - 1024 held reads plus a cancel and a close complete without host blocking or reply loss.
    - 1024 outstanding unary calls whose tasks are all cancelled: every cancel is delivered and answered.
    - `send` on a full queue raises `transport_session_full`; it never blocks or drops.
    - A frame over 64 MiB ends the session.
11. **Pump.**
    - A test host injects an undecodable frame: every outstanding call fails with the closed-session error, and later sends fail.
    - A test host injects one undecodable response payload: only that call fails.
    - A script opens a Client, runs one operation and exits without close. On CPython 3.10–3.13, on Linux and macOS, it exits with code 0 and does not abort.
12. **Python mapping.**
    - gwz-py's Client-level and native-integration tests pass through `NativeCoreBridge`.
    - A cancelled unary call cancels and joins.
    - Calls issued from different event loops complete on their own loops.
    - Replies for closed loops are dropped.
13. **Wire proof.** The same tests pass through `StreamCoreBridge`.
14. **gwz-cli.** Its suite passes when it runs through the in-process session (§11). A pseudo-terminal test shows the progress line still appears.
15. **Ordinary builds.** Cancelling a running fetch returns after it completes, with `Completed`. `transport_capabilities` reports no cancellation support.

## 16. Risks and open points

- Cancellation is cooperative in-process. A handler that never returns is detached at the close bound. It keeps its thread, and any cross-process lock it holds, until it returns.
- Process exit does not join a detached worker. A local write it has in progress can be torn by exit, the same exposure the CLI has under Ctrl-C.
- Connections are not reused across operations. Eight overlapping operations may open up to 8×32 connections to one host.
- A misclassified writer would run concurrently. Defaulting to W limits the damage.
- **Core API changes:**
  - serializing member steps changes the fetch and push handlers, which must take the session's member lock through the gate;
  - the transport API gains a request constructor that takes the caller's token;
  - a session variant of `with_local_transport` takes the token, the endpoint environment and the timeouts;
  - handler contexts gain the token.
- Building a transport runtime per operation starts that runtime's threads for every operation. The cost must be measured.
- The transport stays candidate-only, as it is for gwz-cli. Ordinary builds run handlers with the default backend, with the degraded cancellation of §5.8.
- The process-global state in §5.7 remains until the implementation removes it. Until then, two sessions in one process can still interfere through the `debt` entries in the allowlists.
