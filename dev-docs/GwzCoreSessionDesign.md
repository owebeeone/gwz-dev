# GWZ core session — contract design

Date: 2026-09-24. Status: **DRAFT contract design; review required; no implementation or activation authority.**

This contract gives the shape recommended in [the clean-slate proposals §8](GwzClientCoreTransportProposals.md) exact behaviour, under that document's fixed requirements G1–G11. It pairs with DRAFT paragraphs in gwz-core's [GWZDesign](../gwz-core/dev-docs/GWZDesign.md) and [GWZRequirements](../gwz-core/dev-docs/GWZRequirements.md), as gwz-core's `AGENTS.md` requires before core behaviour expands. It also adds pointers in gwz-py's [package design](../gwz-py/dev-docs/GwzPyDesign.md) and [transport design](../gwz-py/dev-docs/GwzPyTransportDesign.md).

## 1. Scope

**In scope:**
- the session protocol: frames, calls and replies;
- the channel contract and its two adapters;
- the core session host: ownership, admission, workers, cancellation, retention and closure;
- the gwz-py extension boundary;
- the mapping of the unchanged Python API onto calls;
- gwz-cli's use of the session host;
- the wire proof;
- the schema additions.

**Out of scope:**
- client placement, for which only a transport lane is reserved;
- remote deployment;
- connection reuse across operations;
- any change to gwz-transport, the virtual-stream protocol or the public Python API.

This contract adopts the proposals' recommended answers to their open decisions, and review may overturn them:
- one transport runtime per operation;
- `operation.cancel` and `operation.release` as service methods, plus `operation.response` and `session.close`;
- channel closure cancels live work;
- these default limits:

| Limit | Default |
| --- | --- |
| running operations per session | 8 |
| queued operations | 64 |
| retained terminal records | 64 |
| event log per operation | 2 MiB |
| outstanding calls | 1024 |
| wait held by a read | at most 30 seconds |

## 2. Parties and ownership

| Party | Where it runs | What it owns |
| --- | --- | --- |
| Channel | between one client and one session host | frames in transit; delivery only |
| Session host | gwz-core, one per channel | every call from receipt to its reply; every operation record from admission to release, eviction or session end; admission, locks and retention |
| Worker | one thread per running operation, created and owned by the session host | the running operation: its transport runtime, handler and finish |
| Client bridge | gwz-py's `NativeCoreBridge`, gwz-cli's driver, a test bridge | its outstanding calls and its own view of results; nothing inside core |

Six rules follow. Every later section applies them.

- **O1 — Receipt transfers ownership.** The session host owns a call from the moment it takes the call's frame off the channel. Until then the frame is in transit, and the client knows only that it sent it.
- **O2 — One reply per call.** The session host sends exactly one reply, a response or an error, for every call it receives. If the session ends first, the channel's closure stands in for the reply to every outstanding call.
- **O3 — One terminal per operation.** A running operation's terminal outcome is written once, by its worker, when the handler returns, returns an error or panics. The session host writes a terminal itself only for an operation that never started: one cancelled while queued (including by close), or one whose worker thread could not be created.
- **O4 — Intents are recorded, not enacted.**
  - Cancel is recorded on the operation and signalled to its worker's transport cancellation.
  - Release discards a terminal record.
  - Close starts session shutdown (§8).
  - None of these writes a running operation's terminal.
- **O5 — Only owners' timers act.** Core's existing transport deadlines act inside a worker's runtime, and the bounded read wait is the session host's own. A client-side timeout only stops waiting, or sends a cancel.
- **O6 — Nothing but frames crosses.** Client and session host share no memory, lock, callback or thread.

## 3. The channel

- **Scope and guarantees.** One channel carries one session. It is bidirectional, reliable and ordered in each direction while open, bounded, and it reports closure to both ends. These are the carrier guarantees gwz-transport already states (proposals G6).
- **Frames.** A frame is a tag byte followed by a deterministic-CBOR body, taut-shape's frame layout.

  | Tag | Meaning |
  | --- | --- |
  | 1 | `SessionCall` |
  | 2 | `SessionReply` |
  | 3 | `SessionError` |
  | 16–31 | reserved for a transport lane (client placement); unused by this contract |

  Any other tag, or a body that does not decode, is a protocol error: the session host ends the session as in §8.
- **Adapters.** There are two, and both carry the same frames.
  - **In-process:** two bounded queues of frames inside the client's process.
    - The client's `send(frame)` never blocks.
    - `recv()` blocks until a frame arrives or the session has ended.
  - **Byte stream:** stdin/stdout, a socket or SSH. Each frame is prefixed with its length as a little-endian `u32`, exactly as the taut-shape interop tool frames it.
- **Volume is bounded on both sides, and Git never waits on the channel.**
  - The session host only ever sends replies, so what it sends is bounded by outstanding calls.
  - Clients bound their own outstanding calls, and the session host refuses calls beyond its limit (§5.1).
  - Events are retained inside core and read by calls (§6); nothing streams them onto the channel.

## 4. The protocol

### 4.1 Frame bodies

These messages are appended to the GWZ Taut schema:

```text
SessionCall  { call_id: u64 (1), method: str (2), mode: SessionMode (3, optional; default unary), request: bytes (4) }
SessionReply { call_id: u64 (1), response: bytes (2) }
SessionError { call_id: u64 (1), code: GwzErrorCode (2), message: str (3), meta: bytes (4, optional) }
SessionMode  = unary (0) | submit (1)
```

- `request` and `response` are the Taut-encoded messages that the called method declares.
- `call_id` is chosen by the client, and is unique and increasing within the session. A call that reuses the `call_id` of a call still outstanding is refused with `InvalidRequest`.

### 4.2 Methods

**Direct methods** reply with their final response, whatever `mode` says:
- `status`, `ls`, `resolve_forall_targets`, `list_snapshots`, `diff`, `log`;
- `transport_capabilities`, `remote_identity`;
- `configure_transport_runtime`, which changes session-wide transport settings for operations admitted after its reply and leaves running operations unaffected.

Direct methods behave as unary calls: they run on a worker (§5.2), count toward the running limit, and `operation.cancel` may target their `call_id`.

**Every other existing method is an operation method,** except the log-read and result methods below. `mode` chooses its reply:
- **`unary`:** the reply is the method's final response, sent when the operation ends. The record is discarded once the reply is sent.
- **`submit`:** the reply is the method's `accepted` response (`AggregateStatus.accepted`, `ResponseMeta.operation_id`), sent as soon as the session host admits the operation, whether it starts running or waits in the queue. Accepted therefore means that core owns the operation; it does not mean that Git work has started. The record is kept until it is released, evicted or the session ends.

**Log reads** cover `events.subscribe` (by `operation_id`), `diff.output` and `log.output` (by `log_id`).
- A read call names the log, a cursor, optional record and byte limits, and a wait of at most 30 seconds.
- The reply carries the records after the cursor, the next cursor, and whether the log is complete.
- The session host holds a read until a record is available, the log completes, the wait elapses or the session ends. A read that returns no records and an incomplete log is simply read again.
- Ending a `diff.output` reader stream and releasing a `log.output` log are calls as well, with the semantics the bridge has today.

**Results:** `operation.result(operation_id)` replies with the `OperationResult` once the operation is terminal.

**New methods, all append-only:**
- **`operation.response(operation_id)`** replies with the operation method's final response once terminal. It generalizes today's merge-only response lookup.
- **`operation.cancel(target)`** takes an `operation_id` or the `call_id` of a unary call.
  - It replies once the target is terminal, with that operation's cleanup report (the fields of today's `TransportCleanup`).
  - Cancelling an operation that is already terminal replies at once with its retained report.
  - An unknown target gets `OperationNotFound`. A target that was released or evicted, or a unary call that has already been answered, gets `OperationExpired`; the bridge treats that as "already finished".
- **`operation.release(operation_id)`** discards a terminal record and replies. A live operation gets `OpenOperation`; a repeat, or an evicted record, gets `OperationExpired`.
- **`session.close()`** replies after shutdown (§8) with the close report, the fields of today's `TransportCleanup`. The session host then closes the channel.

**Error codes:** all reused, none added: `InvalidRequest`, `OperationNotFound`, `OperationExpired`, `OpenOperation`, `TransportSessionFull`, `InternalError`, and `cancelled` (73).

### 4.3 Identity

- `call_id` correlates replies.
- `operation_id` is assigned by the session host. It names an admitted operation and appears in accepted responses, events and results.
- `RequestMeta.request_id` keeps its existing meaning: the caller's correlation label and the ID the operation's transport request registers under. With one transport runtime per operation, no uniqueness rule applies across operations beyond core's existing validation.

## 5. The session host

### 5.1 Admission

**Resolution first.** For a method that acts on an existing workspace, the session host resolves that workspace's root before admission. A method that creates a workspace is keyed by its target path instead. A request that fails validation or resolution is refused before it becomes an operation.

**Classes.** Every method belongs to exactly one class. The class table lives in core, and any method missing from it counts as W.

| Class | Methods | May run alongside |
| --- | --- | --- |
| R, reads and session settings | the direct methods of §4.2 | anything |
| N, network ref updates | `fetch`, `push` | R and N operations |
| W, workspace writes | every other method | only R operations in the same workspace |

**Serialization.**
- Each fetch or push member step, meaning that member's network exchange and ref update, takes the session's lock for that workspace and member. Two operations therefore never update the same member's refs at once.
- The existing cross-process workspace mutator lock still applies to branch and stash mutations.

**Limits.**
- At most 8 operations run at once per session.
- An operation that can't start yet waits in one FIFO queue per session, holding at most 64 entries. This happens when the running limit is reached or its class is blocked. N and W operations of the same workspace start in queue order; R operations wait only for a running slot.
- A request that arrives with the queue full is refused with `TransportSessionFull` before any effect.
- More than 1024 outstanding calls are refused the same way.

### 5.2 Workers

- An operation gets its worker thread when it starts running. If the thread cannot be created, the session host settles the operation as `Failed` (`InternalError`) before it starts, with no effect.
- The worker runs gwz-cli's execution path (`execute_invocation`), moved into gwz-core as the shared dispatch that gwz-py's design already asks for.
  - For a transport-scope request in a build that has the transport, it runs `with_local_transport(meta, operation_id, handler)`. That builds the operation's own runtime, registers its single request, runs the handler, finishes the request and shuts the runtime down.
  - Otherwise the handler runs with the default backend.
- The worker appends the handler's events to the operation's log (§6). It then reports exactly one terminal:
  - `Completed`, with the response, if the handler succeeded;
  - `Cancelled` (`cancelled`, 73), if it did not succeed and cancellation had been signalled;
  - `Failed` otherwise.
- A panic is caught at the worker boundary and reported as `Failed` (`InternalError`), with its cleanup unconfirmed. The session carries on.

### 5.3 Cancellation

- **A queued operation** is settled `Cancelled` by the session host before it starts, with no effect, and the cancel is answered.
- **A running operation** gets the intent recorded and its worker's transport cancellation signalled, which fails its network I/O. The cancel's reply waits for the worker's terminal.
  - A handler that has succeeded keeps its `Completed` terminal.
  - A local handler has no cancellation point. It runs to completion, and its own outcome is reported.
- **Unary calls** are cancelled by their `call_id`.

### 5.4 Retention

- **A submitted record** outlives its terminal until it is released, evicted or the session ends. At most 64 terminal submitted records are kept: when another operation's record becomes terminal beyond that, the oldest terminal record is evicted, and later calls on it report `OperationExpired`. Live records are never evicted.
- **An operation's event log** holds at most 2 MiB. On overflow, the oldest incremental events are dropped and a reset marker is kept, so readers know the history is incomplete. The terminal event and the result are never dropped. This is GWZDesign's existing event-buffer rule.
- **A unary record** is discarded once its reply is sent.

### 5.5 Timeouts

Workers run under core's existing transport deadlines: admission, bootstrap, cleanup. The session host adds only the bounded wait of a held read. No timer decides an operation's outcome from outside its worker.

## 6. Events and results

- Handlers append events to their operation's log as they occur, and readers pull them with `events.subscribe` read calls.
- Any number of readers, each with its own cursor, may read one log.
- Nothing is pushed onto the channel, so a slow or absent reader never slows Git.
- Results and final responses are read with `operation.result` and `operation.response`.

## 7. Sequences

A submitted operation, from `submit` through release:

```text
client                                   session host                         worker
SessionCall(fetch, submit) ───────────►  admit; queue or start ─────────────► runtime, handler
◄────── SessionReply(accepted, op_id)
SessionCall(events.subscribe read) ────►  hold until events ◄──────────────── append events
◄────── SessionReply(records, cursor)
SessionCall(operation.result) ─────────►  hold until terminal ◄────────────── terminal (once)
◄────── SessionReply(OperationResult)
SessionCall(operation.release) ────────►  discard record
◄────── SessionReply(ok)
```

A unary call is one `SessionCall(fetch, unary)` followed by one reply when the operation ends. If the client stops waiting, the bridge sends `operation.cancel` for that `call_id` and waits for its reply, exactly as the Python client cancels and joins today.

## 8. Closure

**`session.close()`.** The session host:
1. refuses every later call with `InvalidRequest` ("session closing");
2. settles every queued operation `Cancelled`, with no effect;
3. signals cancellation to every running operation;
4. waits for every worker to report its terminal;
5. answers every outstanding call: held reads return whatever records remain and completion, results and cancels return terminals;
6. answers the close with the close report, which sums the operations' cleanup reports;
7. closes the channel.

Repeated `session.close()` calls, accepted until step 7, receive the same report.

**Close waits for every worker.** Network work fails promptly once cancelled. A local handler, such as a merge, finishes its current work first. So nothing core started is still writing the workspace when close returns. The cost is that a handler which never returns keeps close waiting. A client may stop waiting without stopping shutdown.

**Channel closure without `session.close()`** covers a client that dropped its session, a failed adapter and a protocol error. The session host performs steps 2–4 and ends. No replies are possible.

## 9. The gwz-py extension boundary

The extension exposes one session object with four operations:

| Operation | Behaviour |
| --- | --- |
| `open(options)` | creates the session host and its in-process channel; `options` carry the limits in §1 |
| `send(frame)` | non-blocking; fails once the session has ended |
| `recv()` | blocks with the GIL released; returns a frame, or `None` once the session has ended |
| `close()`, or dropping the object | closes the channel |

Nothing else crosses: no Python callback into Rust, no Rust thread running Python code, no Python thread lent to core work, and no synchronous read of core state.

Today's other entry points leave the bridge's path and are not public API:
- `call`, `submit`, `wait_events`, `operation_result`, `merge_operation_response`, `cancel_operation`, `release_operation` and `reserve_operation`;
- the module-level diff and log functions;
- the module-level operation store.

Whether the extension keeps them for compatibility is an implementation choice.

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
| `diff_log_read`, `diff_log_end_stream` | `diff.output` reads and stream end |
| `log_output_read`, `log_output_release` | `log.output` reads and release |
| `close()` | `session.close`; the reply is the close report |

**The pump.** One daemon thread per session calls `recv()`. For each reply it completes the waiting future on the event loop that issued the call, using `call_soon_threadsafe`. If that loop has closed, the reply is dropped. When `recv()` returns `None`, every outstanding call fails with a typed closed-session error.

**Waiting.** Every bridge method that waits is async (G7). Outstanding calls are bounded by an async semaphore of 1024, so `send()` never waits.

**Task cancellation.**
- Cancelling a task awaiting a unary call sends `operation.cancel` for that `call_id`. It waits for the cancel's reply, shielded against further cancellation, and then propagates. This is today's cancel-and-join.
- Cancelling a task that is iterating events or awaiting a read stops only that read, and its reply is dropped.

Custom bridges are unaffected.

## 11. gwz-cli

In a later phase the CLI opens an in-process session and sends its request:
- as a unary call;
- or, for `--jsonl` progress, as a submit followed by event reads.

It renders replies exactly as it renders results today. Its main thread may block on `recv()`, since the CLI is an application rather than a library. User-visible behaviour does not change. This satisfies GWZDesign's CLI driver design.

## 12. Proof that the boundary is a wire (G3)

- **Host binary:** a test-only binary in gwz-core serves one session over stdin/stdout through the byte-stream adapter. It is never shipped. G10 still holds, because no wheel contains a `gwz` executable.
- **Test bridge:** `StreamCoreBridge`, a test bridge in gwz-py, sends the same frames to that binary over asyncio subprocess pipes. An asyncio reader task takes the place of the pump thread.
- **CI:** CI runs gwz-py's whole test suite twice, once through `NativeCoreBridge` and once through `StreamCoreBridge`. Any difference between the two runs is a defect.

## 13. Schema additions (append-only, G8)

- `SessionCall`, `SessionReply`, `SessionError` and `SessionMode`, with the tag registry of §3.
- Request and reply messages for log reads, stream end and release, and for the four new methods.
- Service methods `operation.response`, `operation.cancel`, `operation.release` and `session.close`.
- No new error codes, and no change to existing messages, gwz-transport envelopes or the virtual-stream protocol.

## 14. Relationship to existing documents

- **GWZDesign "Operation Runtime".** This session host is that runtime, extended with:
  - cancel, release, response and close;
  - admission classes;
  - per-operation transport runtimes.

  Its V0 synchronous `submit`/`subscribe`/`wait` API becomes this protocol. Its event-buffer rule is adopted unchanged.
- **GWZDesign "CLI Driver Design"** is met by §11.
- **gwz-py's package design:**
  - Its native bridge ABI is replaced by §9's four operations.
  - Its "shared gwz-core dispatch API" is realized by §5.2.
  - Its instruction to release the GIL around blocking handlers becomes: handlers run on core's threads, and the only blocking call, `recv()`, releases the GIL.
- **gwz-py's transport design, for the Python Client,** is superseded in:
  - §2's per-call runtime flow;
  - the single-active-operation rule;
  - the Python network lock.

  §4's capability rule survives as an ordinary `transport_capabilities` call.
- **Unchanged:** the remote transport, placement and sequenced-stream designs. Client placement remains future work on the reserved transport lane.

## 15. Verification required after acceptance

Assertions check typed fields and codes only.

1. **One reply per call.** Every call receives exactly one reply. A call whose client stops waiting still completes, and its reply is dropped. A reused outstanding `call_id` is refused.
2. **Concurrency.** Eight overlapping fetch and push operations on one Client complete independently. A ninth waits in the queue and then runs. With 64 operations queued, the next request is refused with `TransportSessionFull` and has no effect.
3. **Admission classes.**
   - A W operation excludes N and W operations on its workspace, while R operations run alongside it.
   - Two fetches touching one member serialize that member's step.
   - A method missing from the table is treated as W.
4. **Cancellation.**
   - A queued operation becomes `Cancelled` with no effect.
   - A running fetch is cancelled promptly and returns its cleanup report.
   - Success racing a cancel stays `Completed`.
   - A local handler runs to completion and reports its own outcome.
5. **Panics.** A handler panic yields `Failed` (`InternalError`), and the next call succeeds.
6. **Retention.**
   - A 65th terminal submitted record evicts the oldest, which then reports `OperationExpired`.
   - Release refuses live operations with `OpenOperation`.
   - An event log that overflows keeps its reset marker and its terminal event.
7. **Closure.**
   - Close cancels queued and running work and answers only after every worker has reported.
   - Repeated close returns the same report.
   - Dropping the channel without close performs the same shutdown.
8. **Python mapping.**
   - gwz-py's whole existing suite passes through `NativeCoreBridge`.
   - A cancelled unary call cancels and joins.
   - Calls issued from different event loops complete on their own loops.
   - Replies for closed loops are dropped.
9. **Wire proof.** The same suite passes through `StreamCoreBridge`.
10. **gwz-cli.** Its suite passes when it runs through the in-process session (§11).

## 16. Risks and open points

- Cancellation is cooperative in-process. A handler that never returns keeps its worker and holds up close.
- Connections are not reused across operations. Eight overlapping operations may open up to 8×32 connections to one host.
- A misclassified writer would run concurrently. Defaulting to W limits the damage.
- Serializing member steps changes the fetch and push handlers, which must take the session's member lock.
- Building a transport runtime per operation starts that runtime's threads for every operation. The cost must be measured.
- The transport stays candidate-only, as it is for gwz-cli. Ordinary builds run handlers with the default backend.
