# GWZ client, core and transport — clean-slate proposals

Date: 2026-09-24. Status: **DRAFT proposals; not reviewed; no design, implementation or activation authority.**

Revised on 2026-09-24 after operator direction. gwz-py keeps running core in-process through its extension (G10) and keeps its public API (G11), and the in-process session shape in §8 is the recommendation. The decisions listed in §8 remain open.

The Python concurrency design train was retired to [history](history/) on 2026-09-24. It ran from the concurrency NO-GO finding through the v4 foundation draft, and its reviews, verdicts and remediation plans went with it, together with the draft transport-delivery amendments paired with it. This document starts again from the documented architecture and the facts of the current code. It reuses none of the retired designs' text.

## 1. Why start again

Every design in the retired train tried to make one handoff atomic: an operation passing from the Python binding into native code. Each did it with state shared across that boundary. Examples: a native record store both sides wrote, synchronous claims under one mutex, and a progress flag one thread wrote and the same thread read back after a panic. Each review found another interval where the handoff was not yet complete, and each fix added more shared state.

The boundary those designs guarded is not the boundary the architecture specifies. Core is meant to sit behind a message path that a wire can replace without code changes. Neither client uses such a path today:

- gwz-cli passes typed requests straight into core's `workspace_ops` handlers ([dispatch.rs](../gwz-cli/src/globalargs/dispatch.rs)). Events come back through a callback that core's member threads call.
- gwz-py's native layer builds the transport runtime itself, admits requests into it and runs Git on Python's threads.
- Core's operation handlers block their caller until the operation ends. Core does contain a non-blocking `OperationRuntime` with `submit`, `subscribe` and `wait` ([push_event.rs](../gwz-core/src/operation/push_event.rs)), matching the documented design. No client uses it, and it has no cancel, release or transport integration.
- The client–core communication layer that the transport design assumes was never built. Client ("CLI") placement of the network endpoint exists only in tests.

Over a wire, a handoff cannot be atomic: a sender learns that its request arrived only when a reply comes back. A design that starts from that fact does not need the machinery the retired train kept adding.

## 2. Fixed requirements

These are taken as given. Striking or changing one is an operator decision.

| ID | Requirement | Source |
| --- | --- | --- |
| G1 | Core runs in-process or behind a separate client boundary and carries the same typed operations either way. It starts no server or daemon itself, and no deployment may require a daemon. | [gwz-core README](../gwz-core/README.md); [GWZRequirements](../gwz-core/dev-docs/GWZRequirements.md) REQ-010, REQ-011 |
| G2 | Every client reaches core through one message path: build a Taut request, submit it, render the immediate response, then render events until the `OperationResult`. The path is the same for the CLI, a daemon, a UI and a test harness. | [GWZDesign](../gwz-core/dev-docs/GWZDesign.md), "CLI Driver Design" |
| G3 | The client–core boundary can be replaced by a wire with no code change on either side. | operator, 2026-09-24 |
| G4 | Core reaches SSH and HTTPS only through a Taut-defined bidirectional message service. The network endpoint runs either in core's process ("local", the default) or in the client, over a message channel. An unsupported placement refuses without falling back. | [GWZRequirements](../gwz-core/dev-docs/GWZRequirements.md), "Remote transport amendment" |
| G5 | The endpoint owns network traffic, host trust, authentication and its connection pools. Git work stays in core. | same; [TransportPlacement](../gwz-core/docs/TransportPlacement.md) |
| G6 | The carrier under gwz-transport delivers reliably while a session is open, bounds its queues with backpressure and reports closure. Profiles 1 and 2 also need ordered delivery. The accepted sequenced-stream design drops that need for profile 3 by restoring order within each virtual stream. | [remote transport design §4.1](../gwz-core/dev-docs/GwzRemoteTransportDesign.md); [sequenced-stream design](../gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md) |
| G7 | In a library client, every call that can wait is asynchronous, and only asynchronous. | operator, 2026-09-24 |
| G8 | Existing GWZ Taut messages and the gwz-transport virtual-stream protocol keep their meaning; additions are append-only. | GWZ outer protocol compatibility rule |
| G9 | One client session runs several network operations at once, each with its own identity, events, result and cancellation, within bounded per-session resources. | the 1.1.0 Phase 6 NO-GO ([Python transport design](../gwz-py/dev-docs/GwzPyTransportDesign.md) status; [1.1.0 plan](../gwz-core/dev-docs/GwzV110Plan.md) S6.3) |
| G10 | gwz-py runs core in-process through its native `gwz-core` extension. It never shells out to the `gwz` executable, and its wheels do not bundle it. A core hosted elsewhere is reached only through a remote adapter on the same typed message boundary. | [gwz-py design](../gwz-py/dev-docs/GwzPyDesign.md), "Review Of The Old Plan" and "Non-Goals"; [gwz-py README](../gwz-py/README.md); REQ-011 |
| G11 | The Python public API keeps its current shape: the `Client`, its methods, the typed messages and the errors. Changes happen beneath it. | operator, 2026-09-24 |

## 3. What exists and what is missing

Assets:

- **gwz-transport:** stream emulation over typed `Envelope` messages. It provides binding, request registration, multiplexed virtual streams with flow-control windows, cancellation and failure. It performs no I/O: its host moves messages through ports (`next_message`, `deliver`). Codec utilities serialize envelopes as CBOR, capped at 128 KiB.
- **Core's transport host (candidate builds only):**
  - a driver session where libgit2's transports open virtual streams;
  - a local endpoint session with SSH and HTTPS workers and pools;
  - an in-process forwarding thread (the transport pump) between the two;
  - a client endpoint session for client placement, which only tests connect.
- **gwz-cli's per-command transport path** ([local_command.rs](../gwz-core/src/transport_host/local_command.rs)): one runtime, one request, the handler, then finish and shutdown. It is proven for one operation per process.
- **The GWZ Taut service** ([gwz.taut.py](../gwz-core/protocol/gwz.taut.py)):
  - every operation method;
  - `events.subscribe`, a taut-shape log;
  - `operation.result`;
  - `ResponseMeta.operation_id` with an `accepted` aggregate status for long-running work.
- **taut-shape:** delivery-shape engines, the log among them, in Rust and pure Python. Its length-prefixed tagged-CBOR framing already carries an interop matrix between the two languages ([taut-shape](../taut-shape/README.md)).
- **gwz-py's bridge.** The `CoreBridge` abstraction in `bridge.py` already separates the `Client` from the native extension, and Python already encodes requests and decodes responses as Taut CBOR.
- **Core's unused `OperationRuntime`**, with `submit`, `subscribe` and `wait`.

Missing:

- **A plugin boundary that carries only messages.** Today's native calls are mostly bytes in and bytes out, but some behave in ways no wire can reproduce:
  - `call` and `submit` block Python thread-pool threads that core borrows to run Git;
  - `cancel_operation` blocks until the operation has finished;
  - `reserve_operation` mints IDs in native memory.
- a core session host, shared by gwz-cli and gwz-py, that owns the operations it receives;
- `cancel` and `release` in the GWZ service;
- clients that use the message path;
- a host for client placement;
- a test that runs the same client and core over a real byte stream.

## 4. Lessons that constrain every proposal

1. **A handoff is a message.** The receiver owns the work from receipt. The sender knows only what replies tell it. No design may depend on sender and receiver sharing memory, locks or callbacks.
2. **Timers belong to the owner of the work.** A client-side timeout stops waiting or sends a cancel; it decides nothing.
3. **Shared transport state stays inside core.** Transport generations, first-request bootstrap, the 256 lifetime registrations and pool-capacity conflicts are internal to core. Exposing them to a client spreads ownership across layers.
4. **Blocking work runs on threads its owner controls.** Git handlers block, so the core session host must own the threads they run on. Lending a client's threads ties the client's scheduling to core's ownership.
5. **Control traffic must not wait behind data.** When transport messages share a channel with other traffic, window updates, cancels and failures need capacity that bulk data cannot consume. The retired independent-delivery amendment was an attempt to meet this.

## 5. The model every proposal shares

**Roles.**
- A **client** (gwz-cli, gwz-py, another embedder) expresses operations and presents their progress.
- **Core** runs workspace operations, meaning policy, metadata and Git through libgit2, with one session host per client session.
- The **endpoint** performs network I/O to Git hosts.
- The **channel** is the only path between a client and its core session.

**Two flows.**
- The operation flow runs between client and core session and uses the GWZ Taut service.
- The transport flow runs between core and the endpoint and uses gwz-transport envelopes.
- When the endpoint is local, the transport flow stays inside core's process and its pump is Rust. When the endpoint is in the client, the flow rides the channel.

**Ownership.**
- Core owns an operation from the moment its request is received until the operation is released or the session ends. Admission, execution, cancellation, timeouts and retention are core's decisions.
- The endpoint owns a connection from the open it accepts until close or failure.
- A client owns only its view and its waits. A request with no reply yet is simply "sent".

**The channel.**
- There is one bidirectional channel per client session.
- Frames are length-prefixed tagged CBOR, taut-shape's framing. Each frame names its lane: `operations` now, with `transport` reserved for client placement.
- Each lane has the G6 guarantees and its own credit, so one lane cannot block another.
- There are two kinds of adapter: in-process (a bounded queue pair) and byte-stream (stdio, a socket or SSH). Everything above an adapter is identical, which is what G3 asks for.

**Operation protocol.** The existing service, plus two append-only methods: `operation.cancel` and `operation.release`.
- The client chooses each request's `request_id`, unique within the session. Core returns the `operation_id`.
- A short operation may complete in its immediate response. A long one answers `accepted`, then publishes events on a taut-shape log and a result through `operation.result`.
- Retention is bounded per session.

**Closure.** Channel closure ends the session.
- Core cancels live operations, closes transport bindings and discards retained results.
- The client treats an unanswered request as lost, and an accepted but unfinished operation as possibly having had an effect.
- Results the client has already received stay with the client.
- Reconnection and replay are out of scope.

**Execution in core.**
- The session host never blocks on an operation.
- Each operation runs on its own worker thread, owned by core and bounded in number.
- The host runs the channel and timers without blocking.
- Per-session limits (top-level operations, retained records, event bytes) are session-host policy.
- Whether a session shares one transport runtime across its operations or gives each operation its own is internal to core and invisible to clients.

**Clients.** Library clients expose only asynchronous calls for anything that waits (G7). The CLI, being an application, may block its main thread on the channel.

## 6. Proposals

### P1 — Embedded session (recommended; §8 gives its exact shape)

```text
gwz-cli:  CLI ── in-process channel ── core session host ── per-operation workers ── transport runtime + pump (Rust)
gwz-py:   unchanged Client API ── NativeCoreBridge ── extension: open, send, recv, close ── in-process channel ── core session host ── (as above)
proof:    unchanged Client API ── stream bridge ── byte stream ── test-only core host binary ── core session host
```

Core links into each client, and each client talks to its core session only through a channel. For gwz-py, the channel is the extension's boundary. The transport runtime and its pump stay in Rust inside the process, as they are for gwz-cli.

**Strengths:**
- It satisfies G10 and G11.
- There is no packaging change and no process start.
- In-process locks can serialize mutating operations on the same member.
- The byte-stream path is proven by CI through a second bridge.

**Costs:**
- Cancellation is cooperative. It fails the operation's network I/O, but a handler stuck anywhere else keeps its thread until it returns.
- A panic is caught at the worker boundary and reported as a failed operation. Nothing can forcibly stop a native hang.

### P2 — Separately hosted core (remote adapter only)

```text
remote:   client ── stream bridge ── byte stream (for example SSH) ── core host on another machine ── core session host
```

G10 excludes P2's local form: gwz-py must not run the `gwz` executable, and its wheels must not bundle it. What remains is the remote adapter G10 allows. A core hosted elsewhere speaks the same messages over a byte stream, and the client uses a stream bridge. P1's CI proof uses exactly this shape against a test-only host binary, so the remote adapter needs no separate design, only a deployment.

### P3 — Client-held endpoint

```text
gwz-cli:  CLI + endpoint ── channel (operations lane + transport lane) ── core session host (no network access)
gwz-py:   Client API + endpoint in the extension ── in-process channel ── core session host
```

The endpoint always runs in the client. Core never opens a network connection, and every transport message rides the channel's transport lane, including in-process. For gwz-py the endpoint and its pump live in the extension, in Rust, consistent with G10.

- **One transport path,** used by every operation, instead of two placements.
- **Core never holds credentials,** in any deployment. That is the security model a remote core needs, applied by default.
- **The channel needs lanes with independent credit from the start.** The accepted sequenced-stream design (profile 3) already lets each virtual stream restore its own order, so lanes may interleave freely.

**Costs:**
- All Git network data crosses the channel. That is cheap in-process but uses the client's bandwidth when core is remote.
- Lanes and control priority must be built before anything ships.
- A remote core loses its own network access unless that is added back.

## 7. Comparison

| | P1 Embedded session | P2 Remote adapter | P3 Client-held endpoint |
| --- | --- | --- | --- |
| Allowed as gwz-py's local path | yes | no (G10) | yes, with the endpoint in the extension |
| Wire exercised | by CI through a second bridge | in every remote deployment | only with a remote core |
| Native extension | message channel: open, send, recv, close | not applicable | channel plus endpoint |
| Isolation from a hung or crashed core | cooperative cancellation; panics contained per worker | process boundary | as P1 |
| Packaging change | none | a core host deployment on the remote side | none |
| Transport placements to maintain | two (local now, client later) | as P1 | one |
| Channel lanes needed before first ship | no | no | yes |
| Work before Python concurrency ships | session host, channel, thin bridge, two-bridge CI | not on that path | session host, lanes, client endpoint host |

## 8. Recommendation: the in-process session shape

1. **A core session host in gwz-core,** shared by gwz-cli and gwz-py. gwz-py's design already asks for a shared core dispatch API, so the CLI and the extension stop keeping parallel routing tables. The host:
   - owns each operation from the moment its request arrives;
   - gives each operation its own core-owned worker, running gwz-cli's per-command path: `with_local_transport`, the handler, then finish;
   - bounds the number of workers;
   - sends a cancel message to that worker's transport cancellation;
   - when the session closes, cancels every worker and waits for them within a bound;
   - keeps events and results until they are released or the session closes.
2. **The extension's boundary becomes a message channel:** open, send, receive and close. Requests, responses, events, results, cancel, release and close all cross it as messages. Nothing else crosses: no borrowed Python threads, no callbacks and no synchronous state reads.
3. **`NativeCoreBridge` becomes a thin client.** It encodes and sends. One pump thread receives with the GIL released, decodes, and wakes waiting futures on their own event loops. These are the bounded async-safe queues that gwz-py's design already requires.
4. **The transport runtime and its pump stay in Rust in the extension,** as in gwz-cli, so no transport message reaches Python. For client placement later, the extension hosts the endpoint and pumps transport messages between the channel and the endpoint's port, also in Rust.
5. **The Python public API is unchanged (G11).** Concurrency needs no new API: overlapping calls such as `await client.fetch(...)` are simply several requests in flight, each running on its own core worker.
6. **Proof of G3:**
   - A second bridge speaks the same messages over a byte stream to a test-only core host binary.
   - CI runs gwz-py's whole test suite through both bridges.
   - The in-process path already passes encoded bytes, so the only differences over a real stream are framing and the carrier.
   - The wheel ships no `gwz` executable.

Costs of this shape:
- Cancellation is cooperative.
- A native hang cannot be forcibly stopped.
- With one transport runtime per operation, connections are not reused across operations. Eight overlapping operations could open up to 8×32 connections to one host.

Decisions still open:

1. **Transport runtime scope inside core.** One runtime per operation is gwz-cli's proven path and the recommended start. One runtime per session reuses connections across operations, but its shared-generation rules would need specifying.
2. **The protocol additions:** `operation.cancel`, `operation.release` and the frame lane.
3. **Channel closure:** cancel live operations (recommended), or let them finish.
4. **Per-session limits:** the number of concurrent top-level operations and retained records, and whether mutating operations on the same member are serialized with in-process member locks.

## 9. After the decisions

1. **A contract design for the in-process session shape,** reviewed as a new object by fresh reviewers. The draft is [GwzCoreSessionDesign.md](GwzCoreSessionDesign.md). It covers:
   - the protocol additions;
   - the channel contract and its two adapters;
   - the session host's ownership, workers and limits;
   - closure;
   - how the unchanged Python API maps onto messages.
2. **A phased plan:**
   1. the core session host and in-process channel, proven with local operations;
   2. network operations;
   3. the thin `NativeCoreBridge` and the extension's message channel;
   4. the two-bridge CI proof;
   5. gwz-cli onto the session host;
   6. client placement, when a remote core is scheduled.

## Appendix: what was retired on 2026-09-24

Nothing retired is normative. Moves used `git mv`, so lineage is preserved.

- **Root [dev-docs/history](history/)** (120 files): the design objects, reviews, verdicts, remediation plans, prompts and caller-guide drafts of these families:
  - `GwzPyTransportConcurrency*`
  - `GwzOperationSessionProtocol*`, `GwzOperationSessionCallerGuideDraft`
  - `GwzOperationCleanupOwnership*`, `GwzOperationCleanupCallerGuideDraft`
  - `GwzOperationSessionV1*`
  - `GwzOperationStartTicket*`
  - `GwzTransportDeliveryStartTicket*`
  - `GwzPyTransportSessionV2*`, including the foundation and implementation checkpoint
  - `GwzPyTransportSessionV3Foundation*`
  - `GwzPyTransportSessionV4Foundation*`
  - `GwzTransportV3Reordering*`
- **[gwz-core/dev-docs/history](../gwz-core/dev-docs/history/)** (3 files):
  - the draft independent-delivery amendment;
  - the draft concurrent capacity amendment;
  - the stopped profile-3 reordering design, which the accepted sequenced-stream design superseded.

  An [extract](../gwz-core/dev-docs/history/GwzPythonSessionTrainExcerpts-20260924.md) preserves, verbatim, the text removed from current gwz-core documents:
  - the Python session sections of GWZDesign and GWZRequirements;
  - three pending plan candidates;
  - the placement guide's amendment paragraphs. The guide was restored to its accepted text.
- **[gwz-py/dev-docs/history](../gwz-py/dev-docs/history/)** (2 files): both concurrency caller guides. An [extract](../gwz-py/dev-docs/history/GwzPyTransportDesign-TrainExcerpts-20260924.md) holds the paragraphs removed from the Python transport design.
