# GWZ client, core and transport — clean-slate proposals

Date: 2026-09-24. Status: **DRAFT proposals for an operator decision; not reviewed; no design, implementation or activation authority.** The Python concurrency design train was retired to [history](history/) on 2026-09-24. It ran from the concurrency NO-GO finding through the v4 foundation draft, and its reviews, verdicts and remediation plans went with it, together with the draft transport-delivery amendments paired with it. This document starts again from the documented architecture and the facts of the current code. It reuses none of the retired designs' text.

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

## 3. What exists and what is missing

Assets:

- **gwz-transport:** stream emulation over typed `Envelope` messages. It provides binding, request registration, multiplexed virtual streams with flow-control windows, cancellation and failure. It performs no I/O: its host moves messages through ports (`next_message`, `deliver`). Codec utilities serialize envelopes as CBOR, capped at 128 KiB.
- **Core's transport host (candidate builds only):**
  - a driver session where libgit2's transports open virtual streams;
  - a local endpoint session with SSH and HTTPS workers and pools;
  - an in-process forwarding thread between the two;
  - a client endpoint session for client placement, which only tests connect.
- **The GWZ Taut service** ([gwz.taut.py](../gwz-core/protocol/gwz.taut.py)):
  - every operation method;
  - `events.subscribe`, a taut-shape log;
  - `operation.result`;
  - `ResponseMeta.operation_id` with an `accepted` aggregate status for long-running work.
- **taut-shape:** delivery-shape engines, the log among them, in Rust and pure Python. Its length-prefixed tagged-CBOR framing already carries an interop matrix between the two languages ([taut-shape](../taut-shape/README.md)).
- **Core's unused `OperationRuntime`**, with `submit`, `subscribe` and `wait`.
- **gwz-cli's per-command transport wrapper** ([local_command.rs](../gwz-core/src/transport_host/local_command.rs)), proven for one operation per process.

Missing:

- a channel abstraction, with an in-process adapter and a byte-stream adapter;
- a core session host that owns the operations it receives over a channel;
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
- When the endpoint is local, the transport flow stays inside core's process. When the endpoint is in the client, it rides the channel.

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
- The session host never blocks on an operation. It runs the channel, timers and the transport runtime on an asynchronous executor, and dispatches handlers to a bounded worker pool it owns.
- Per-session limits (top-level operations, retained records, event bytes) are session-host policy.
- Whether a session shares one transport runtime across its operations or gives each operation its own is internal to core and invisible to clients.

**Clients.** Library clients expose only asynchronous calls for anything that waits (G7). The CLI, being an application, may block its main thread on the channel.

## 6. Proposals

### P1 — Embedded session

```text
gwz-cli:  CLI ── in-process channel ── core session host ── local endpoint
gwz-py:   asyncio API ── pump thread ── FFI (open, send, recv, close) ── in-process channel ── core session host ── local endpoint
remote:   either client ── byte-stream channel ── core session host (same code)
```

Core links into each client, and each client talks to its core session only through the in-process channel.

- **gwz-cli** builds a request, sends it, and prints the reply and events, as G2 describes.
- **gwz-py's native extension** shrinks to four calls: open a session, send a frame, receive a frame, and close.
  - One pump thread receives, blocking with the GIL released, and wakes asyncio futures on their own event loops.
  - Everything else is Python.
- **The endpoint** stays local. Client placement comes later over a transport lane.

**Strengths:** no packaging change, one process, and the lowest cost per call.

**Costs:**
- The byte-stream adapter runs only in a dedicated CI test.
- Core's threads, and any native crash or hang, live in the client's process.
- gwz-py keeps a native extension.

### P2 — Core process for language bindings

```text
gwz-py:   asyncio API ── subprocess pipes (byte-stream channel) ── gwz core process: session host ── local endpoint
gwz-cli:  embedded as in P1 locally; the same core process over SSH for a remote core
```

gwz-py starts one core process per Client, running the gwz binary in a stdio core mode (name to be decided). It speaks the session protocol over the process's pipes using asyncio streams.

- **gwz-py becomes pure Python for every operation, local or network.** It needs no native extension, no GIL boundary and no thread pool.
- **The gwz binary ships inside the gwz-py wheel,** so client and core versions always match. gwz-cli's release process already builds per-platform binaries.
- **Closing the Client closes the channel.** The core process then cancels its operations and exits. If it has not exited within a bound, the client terminates it. The operating system is the last-resort arbiter, as it already is for gwz-cli.
- **The core process is not a daemon** (G1). It lives and dies with its Client.
- **gwz-cli stays embedded locally,** as in P1. The same stdio mode gives it a remote core over SSH.

**Strengths:**
- The wire is Python's everyday path, so G3 is exercised continuously.
- A hung or crashed core cannot stall the Python process.
- The Python client is plain asyncio, which satisfies G7 directly.
- A remote core needs no extra adapter.

**Costs:**
- Per-platform binaries in the wheels.
- One process start per Client (to be measured).
- The child inherits the user's environment and credentials. That is the intended local-placement behaviour, the same as today's in-process runtime.

### P3 — Client-held endpoint

```text
gwz-cli:  CLI + endpoint ── channel (operations lane + transport lane) ── core session host (no network access)
gwz-py:   asyncio API + endpoint in the native extension ── in-process channel ── core session host
```

The endpoint always runs in the client. Core never opens a network connection, and every transport message rides the channel's transport lane, including in-process.

- **One transport path,** used by every operation, instead of two placements.
- **Core never holds credentials,** in any deployment. That is the security model a remote core needs, applied by default.
- **The channel needs lanes with independent credit from the start.** The accepted sequenced-stream design (profile 3) already lets each virtual stream restore its own order, so lanes may interleave freely.
- **For gwz-py, P3 combines only with P1,** because the endpoint (Rust) must run inside the Python process.

**Strengths:** one transport path, no credentials in core, and ready for a remote core.

**Costs:**
- All Git network data crosses the channel. That is cheap in-process but uses the client's bandwidth when core is remote.
- Lanes and control priority must be built before anything ships.
- A remote core loses its own network access, for example a server fetching over its fast link, unless that is added back.

## 7. Comparison

| | P1 Embedded | P2 Core process for bindings | P3 Client-held endpoint |
| --- | --- | --- | --- |
| Wire exercised in normal use | only by a CI test | yes, on every Python call | only with a remote core |
| Core threads in the Python process | yes | no | yes |
| Python native extension | four calls | none | yes, including the endpoint |
| Isolation from a hung or crashed core | none | process boundary | none |
| Packaging change | none | gwz binary in the wheel | none |
| Transport placements to maintain | two (local now, client later) | two (local now, client later) | one |
| Channel lanes needed before first ship | no | no | yes |
| Remote core | through P2's stdio core mode | same binary over SSH | native fit |
| Work before Python concurrency ships | session host, in-process adapter, FFI pump | session host, stdio core mode, Python client | session host, lanes, client endpoint host |

## 8. Recommendation and decisions

Recommendation: **P2 for gwz-py and P1 for gwz-cli.** Both run on one core session host and one session protocol, with the endpoint local. The transport lane is reserved in the frame format now and put to use, as in P3, when a remote core is scheduled.

- Python is the client that needs concurrency. Under P2 it gets concurrency with the wire as its everyday path, with no FFI and no core threads in the Python process.
- The CLI stays one process and keeps its speed.
- All core work is shared: the session host, the protocol additions, the worker pool, retention and closure. Both clients therefore prove the same code.

Decisions needed:

1. **Which proposal, or combination,** to take forward.
2. **Transport runtime scope inside core.** There are two options:
   - One runtime per session reuses connections across operations, but needs its shared-generation rules specified.
   - One runtime per operation is gwz-cli's proven path. Connections are not reused across operations, and eight overlapping operations could open up to 8×32 connections to one host.
3. **The protocol additions:** `operation.cancel`, `operation.release` and the frame lane.
4. **Channel closure:** cancel live operations (recommended), or let them finish.
5. **P2 only:** bundle the gwz binary in the wheel (recommended), or require one on `PATH`.

## 9. After the decision

1. **A contract design for the chosen proposal,** reviewed as a new object by fresh reviewers. It covers the protocol additions, the channel contract and adapters, the session host's ownership and limits, closure, and the Python API shape. The retired v2 caller guide is available as input for the API shape.
2. **A phased plan:**
   1. the core session host and in-process adapter, proven with local operations;
   2. network operations;
   3. the clients;
   4. the byte-stream test (under P2, Python's own test suite);
   5. client placement, when a remote core is scheduled.

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
