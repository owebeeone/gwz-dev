# GWZ IN-MEMORY TYPED-MESSAGE STREAM IMPLEMENTATION — STATE-AXIS REVIEW

**Review object:** Initial gwz-transport implementation at `89e9ebefee2598ababbbb227366ecbbd924b5cb5`, including checked-in source, schema, scripts, tests, and README; implementation checkpoint dated 2026-09-19, with no interface freeze. Controlling draft: gwz-core `dev-docs/GwzRemoteTransportMemoryImplementation.md` at `6a82c646fa9cff5555a182366bc3dd41707bd076`.  
**Baseline:** gwz-transport `89e9ebefee2598ababbbb227366ecbbd924b5cb5`; gwz-core `6a82c646fa9cff5555a182366bc3dd41707bd076`; workspace root `3976ef2c046b333bcf0765db916004f8387d8259`. Sources were read from immutable objects with `git show PIN:path` and `git grep PIN`.  
**Date:** 2026-09-19  
**Axis:** State — state machines, concurrency, failure and recovery. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — two P2 findings block. I pre-commit to GO on a revision that resolves P2-1 and P2-2 as specified.

---

## 0. Evidence base

The exact tuple was verified at the beginning and end. Both checks returned:

| Repository | Commit | Tree |
|---|---|---|
| workspace root | `3976ef2c046b333bcf0765db916004f8387d8259` | `87e5238dc3b3df61a33dc8cde1aab459657c4f36` |
| gwz-core | `6a82c646fa9cff5555a182366bc3dd41707bd076` | `0fa0cfdfbb5f97607e442ef17ba3c54641395e6c` |
| gwz-transport | `89e9ebefee2598ababbbb227366ecbbd924b5cb5` | `a2ae33738300ba1894920a9b2cd88cfbb51245e5` |

The uncommitted generated prompts at root and excluded `gwz-core/tests/transport_consumer/` remained outside the reviewed object. No peer report was read.

The review read:

- Root `CurrentProgramCheckpoint.md`, the applicable independent-review rules in `AgentProcessRules.md`, and `GwzProcessOptimization.md`.
- The complete memory checkpoint, `GwzRemoteTransportMemoryImplementation.md:1-87`.
- Controlling state semantics in `GwzRemoteTransportDesign.md:329-455`, stream requirements S1-S9, and the applicable implementation-plan checkpoint.
- gwz-transport `AGENTS.md`, `README.md`, package metadata, canonical schema, exported IR, regeneration/admission scripts, binding and admission code, and all stream implementation modules.
- All checked-in tests, including the deterministic stream cases, async waiter cases, protocol/binding tests, and seeded Monte Carlo harness.
- Generated protocol and codec seams relevant to terminal failure, message admission, offsets, and resource bounds.

Commands run at the pinned gwz-transport commit:

- `cargo test --locked`: passed all 28 executed integration tests; the 50,000-case campaign remained intentionally ignored.
- `cargo clippy --all-targets -- -D warnings`: passed.
- `GWZ_TRANSPORT_MC_SEED=0x5a7e20260919 GWZ_TRANSPORT_MC_CASES=10000 cargo test --locked --release --test monte_carlo seeded_message_streams -- --exact --nocapture`: passed 10,000 cases. Coverage included 1,780,261 blocked writes, 3,861,627 blocked reads, 491,915 partial writes, 141,134 flushes, 626 tiny windows, 864,743 delayed deliveries, and 1,225,173 data messages.

Tests used Rust `1.96.0`. No source was edited, no git state was mutated, and build output remained ignored.

## 1. Findings

### [P2-1] Peer failure code and effect are irreversibly collapsed into `PeerFailed`

**Location.** The schema deliberately distinguishes `Failure.code` and `Failure.effect` (`protocol/transport.taut.py:14-17,31`) and carries that structure in both `Closed.failure` and `Failed` (`:53-54,67`). The stream error vocabulary has only the payload-free `Error::PeerFailed` (`src/stream/mod.rs:71-84`). Receiving either a failed close or a standalone failure discards the complete `Failure` value and stores only `PeerFailed` (`src/stream/incoming.rs:111-124,138-140`).

**Violated invariant.** A terminal typed failure, including whether a remote effect is possible, must remain observable after any accepted byte prefix. The controlling design requires typed stream failure and preserves uncertain remote outcomes rather than silently replaying them (`GwzRemoteTransportDesign.md:391-421,452-455`).

**Reproduction.**

1. Deliver valid `Data` at offset zero so the receiver has an accepted prefix.
2. Deliver `Failed { code: InvalidRequest, effect: None }`.
3. Read the accepted prefix; the next read returns `Error::PeerFailed`.
4. Repeat with `Failed { code: Io, effect: Possible }`. The caller observes exactly the same prefix followed by exactly the same `Error::PeerFailed`.
5. The same collapse occurs when either failure is supplied through `Closed.failure`.

The host cannot recover the information from the stream later: `MessageEndpoint::deliver` consumes the envelope, and neither `StreamMachine` nor `Stream` exposes the discarded failure.

**Impact.** The operation layer cannot distinguish a retry-safe refusal from a failure after a possible remote effect. It must either classify every peer failure as uncertain, losing required diagnostic and recovery parity, or risk an unsafe retry. The schema’s explicit effect field becomes false composition at the state-machine boundary.

**Required correction.** Preserve the received `Failure` as the terminal cause and expose its exact `ErrorCode` and `Effect` after buffered bytes have been read. This may be a structured stream-error variant or an equivalent terminal-cause accessor, but both `Failed` and `Closed.failure` must use it. Local terminal transitions should likewise map their typed code/effect deliberately rather than relying on an undifferentiated peer error.

**Closure test.** Deliver an accepted prefix followed separately by `(InvalidRequest, None)` and `(Io, Possible)` through both `Failed` and `Closed.failure`. Assert that the prefix is returned first and that the next operation reports the exact code and effect in all four cases.

### [P2-2] Application waiters can consume every slot and permanently evict the sole outgoing-message pump

**Location.** `Config.max_waiters` accepts any value from 1 through 1024 (`src/stream/mod.rs:21-68`). All async operations share one capped `BTreeMap` of waiters (`src/stream/asynchronous.rs:14-18,55-83`). `MessageEndpoint::next_message`, the sole documented outgoing-message dispatcher, uses the same `Waiter` mechanism as application reads, writes, flushes, end-write, and close (`:147-194,215-230`). A future rejected with `WaiterCapacity` is returned ready and is not registered for a later wake.

**Violated invariant.** Data saturation or application concurrency must not consume the capacity needed to emit window, cancellation, or terminal control. The design explicitly reserves control progress so blocked data operations cannot deadlock teardown (`GwzRemoteTransportDesign.md:370-374`).

**Reproduction.**

1. Construct a valid stream with `max_waiters = 1`.
2. Poll a read on an empty stream. It returns pending and occupies the only waiter slot.
3. Poll `MessageEndpoint::next_message`. No message is currently ready, so it attempts registration and instead returns `Err(WaiterCapacity)`.
4. Cancel the stream or drop its final owner. The machine creates a terminal `Cancel` message and wakes the registered read, but there is no registered dispatcher waker.
5. The read observes cancellation and unregisters. The message pump already completed with an error and receives no notification that capacity became available. Unless the host invents an out-of-band retry loop, the terminal control message remains trapped.

With larger limits, the same sequence occurs when application futures occupy all slots. Starting the pump first merely reverses the failure: an application lifecycle future can be rejected because the infrastructure waiter consumed the final slot.

**Impact.** A valid configuration and ordinary task scheduling can stop outgoing delivery or prevent an application lifecycle call from waiting. Cancellation and close progress then depend on undocumented polling outside the async API, violating the reserved-control and wakeup contract.

**Required correction.** Give the single `MessageEndpoint::next_message` dispatcher a dedicated wake slot that application waiters cannot consume, or otherwise reserve infrastructure capacity and keep the pump pending until state changes. `max_waiters` should bound application waiters without allowing them to evict the stream’s outgoing-control path.

**Closure test.** With `max_waiters = 1`, register an application read first and the message pump second. The pump must remain pending rather than fail capacity; a subsequent write and cancellation must wake it and yield the expected messages. Repeat with the pump registered first and enough application futures to exhaust the application cap, proving that overflow is confined to application calls and control delivery remains live.

## 2. Invariant analysis

The following attacks held:

- **Exact byte and offset preservation.** Incoming data requires the next exact offset, a nonempty bounded payload, and an end within advertised credit (`src/stream/incoming.rs:26-50`). Outgoing offsets advance only by bytes drained into each emitted `Data` message (`src/stream/outgoing.rs:69-88`). Deterministic and randomized tests independently reconstructed both byte streams.
- **Bounded per-stream memory.** Writes accept only available send-buffer capacity; incoming data cannot exceed the configured receive window. Payload and configuration maxima are validated. Typed-message admission rejects excess metadata, payload, collection, and allocation costs before copying into the machine. Aggregate delivery queues remain an explicitly documented host responsibility.
- **Credit follows consumption.** Decoding alone does not increase advertised credit. Reads and explicit close discard advance `consumed`; only then can `Window` increase the absolute limit (`src/stream/machine.rs:134-161,204-220`; `src/stream/outgoing.rs:34-43`). Reverse flush acknowledges bounded read-adapter admission, while forward flush waits for endpoint consumption.
- **Batching progresses independently of callers.** The first buffered byte establishes the deadline; later writes do not restart it. A clock advance crossing the deadline changes the revision and wakes the message pump. Full buffers, flush, end-write, close, and blocking reads make data immediately eligible.
- **Half-close and graceful close ordering.** End-write waits for buffered data. Initiator close starts its deadline immediately, discards unread reverse bytes explicitly, requests end-write, and emits `Close` only after `EndWrite`. Endpoint completion requires both directions ended, request bytes consumed, and flushes settled (`src/stream/machine.rs:194-249`; `src/stream/outgoing.rs:90-110`).
- **Close cannot wait indefinitely for credit.** The close deadline includes time spent waiting to send buffered data. Expiry clears pending work and emits cancellation or failure according to side.
- **Prefix-then-error ordering.** Terminal failure does not clear the receive queue. Reads return buffered bytes before returning the stored error (`src/stream/machine.rs:134-161,333-370`). P2-1 concerns loss of the structured cause, not loss of the prefix.
- **Late traffic cannot reopen terminal state.** Once failed or completed, incoming messages are ignored and cannot mutate offsets, release another resource, or create a second terminal transition (`src/stream/incoming.rs:6-14`).
- **Cancellation and ownership are single-shot.** Cloned `Stream` handles share one mutex-protected machine and one byte position. The atomic owner count requests cancellation only when the final stream handle drops. Concurrent transition functions are serialized, and wakers are invoked outside the mutex.
- **Cancelled futures remove their registrations.** A pending operation’s `Waiter` removes its entry on drop. Re-polling replaces its waker without growing the map. The failed attack is infrastructure starvation under the shared cap in P2-2.
- **Delivery loss wakes blocked operations.** Dropping `MessageEndpoint` marks carrier loss and wakes registered operations. Buffered prefixes remain readable before that error.
- **Random-test replay is deterministic.** The generator uses fixed SplitMix64 arithmetic, derives per-case seeds independently, records a bounded trace, and reruns every sixteenth default case to compare digest and coverage. The byte, offset, and credit ledgers are separate from the machine’s internal counters.

The physical carrier, aggregate multi-stream scheduler, real sink, pool/lease ownership, and timer task are correctly absent under the operator-defined boundary. The crate exposes the state transitions those hosts must drive rather than claiming those external facilities exist.

## 3. Risks and next action

The randomized suite strongly attacks successful bidirectional transfer, buffering, credit, flush, timer, and half-close behavior. Cancellation, graceful close, terminal errors, async waiter ownership, and delivery loss rely mainly on named deterministic tests. That split is acceptable for this checkpoint, but the two missing cross-boundary cases correspond directly to the findings above.

The implementation still depends on the host honoring ordered delivery, bounded aggregate queues, one dispatcher, monotonic clock service, sink admission, and cleanup/reuse ownership. Those are explicit supplied responsibilities and are not findings in this object.

The next action is one bounded state-machine correction that preserves peer `Failure` code/effect and reserves the outgoing dispatcher’s wake path, with the named regression sequences added to the deterministic suite. Re-run the full suite, clippy, and seeded randomized campaign, verify a new exact tuple, and request a focused State re-verdict on P2-1 and P2-2.
