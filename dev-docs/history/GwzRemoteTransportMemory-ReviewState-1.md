# GWZ IN-MEMORY TYPED-MESSAGE STREAM IMPLEMENTATION — STATE-AXIS RE-VERDICT

**Review object:** Remediation-round-1 gwz-transport implementation at `aa9ecae65d6c0d568c5f4d738f9930d49f684f56`, including checked-in source, schema, scripts, tests, and README; implementation checkpoint dated 2026-09-19, with no interface freeze. Controlling draft: gwz-core `dev-docs/GwzRemoteTransportMemoryImplementation.md` at `6a82c646fa9cff5555a182366bc3dd41707bd076`.  
**Baseline:** Corrected gwz-transport `aa9ecae65d6c0d568c5f4d738f9930d49f684f56`; gwz-core `6a82c646fa9cff5555a182366bc3dd41707bd076`; workspace root `a93224a7d8b21a49534174073b7fa55ee62ce4ce`. The remediation diff was read from immutable objects between transport `89e9ebefee2598ababbbb227366ecbbd924b5cb5` and `aa9ecae65d6c0d568c5f4d738f9930d49f684f56` using `git show` and `git diff`.  
**Date:** 2026-09-19  
**Axis:** State — state machines, concurrency, failure and recovery. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: GO** — both original P2 findings are closed. No new P0, P1, P2, or P3 finding was found in the changed ranges or their state-machine interactions.

---

## Prior-finding closure

| ID | Disposition claimed | Verified on corrected tree | Status |
|---|---|---|---|
| State P2-1 | Retain exact peer `Failure.code` and `Failure.effect`; restrict `Cancel` to representable reasons | `Error::PeerFailed { code, effect }` retains both fields (`src/stream/mod.rs:107-123`). Both `Failed` and `Closed.failure` install that exact value (`src/stream/incoming.rs:112-154`). Buffered bytes remain readable before the stored terminal error (`src/stream/machine.rs:134-161`). The regression covers representative failure codes with both effects through both terminal message forms, after a byte prefix (`tests/stream.rs:230-286`), and checks async wake/error parity (`tests/async_stream.rs:207-238`). Invalid `Cancel` reasons fail admission; `Cancelled` and `Timeout` remain distinct (`src/codec/validate.rs:142-148`; `tests/stream.rs:288-306`). | **CLOSED** |
| State P2-2 | Reserve outgoing-dispatcher capacity outside the application waiter cap | Registrations carry an explicit dispatcher class (`src/stream/asynchronous.rs:14-22,51-64`). Capacity is counted separately: one dispatcher registration plus `Config.max_waiters` application registrations (`:65-104`). `MessageEndpoint::next_message` always uses the dispatcher class (`:239-256`). The cap-1 regression covers dispatcher-first and application-first registration, application overflow, data wakeup, cancellation wakeup, and cleanup (`tests/async_stream.rs:156-205`). | **CLOSED** |

## Changed-range analysis

| Changed surface | State-axis attack | Result |
|---|---|---|
| Structured failures and cancellation | Replayed both original terminal sequences, including an accepted prefix followed by `Failed`, a failed `Closed`, pending async readers, and unsupported cancellation reasons. | Exact peer code/effect reaches read, write, close-result, and async callers. Prefix ordering remains intact. Unsupported `Cancel` reasons become protocol failure rather than collapsing into cancellation. |
| Async waiter accounting | Saturated `max_waiters = 1` before and after registering the outgoing pump; then generated data and terminal cancellation. Examined registration, re-poll, ready, and future-drop cleanup paths under the mutex. | The application waiter and dispatcher coexist. Application overflow cannot evict the pump, state changes wake both classes, and ready or dropped futures remove their registrations. No lost-wakeup interleaving was found. |
| Directional negotiated limits | Checked construction, incoming typed admission, outgoing payload/window configuration, and endpoint close-fact retention for receiver/peer direction reversal or post-retention rejection. | `receive_limits` govern incoming values and local receive capacity; `peer_limits` govern emitted limits and caller-provided `Closed` facts (`src/stream/mod.rs:21-37,56-103`; `src/stream/incoming.rs:17-25`; `src/stream/machine.rs:224-255`). Invalid close facts are rejected before retention and remain retryable. Boundary tests cover metadata and construction expansion (`tests/stream_limits.rs:27-120`). |
| Typed admission accounting | Compared the generated typed walk with serialized preflight for nesting, collection entries, canonical encoded length, allocation charge, map keys, absent optionals, lists, and bootstrap limits. | The typed walker now charges the same node depth, entries, canonical bytes, and conservative allocation as preflight without constructing a CBOR tree (`src/budget.rs:3-99`; `src/admission.rs`; `src/codec/preflight.rs:3-125`). Boundary sweeps compare typed admission, encode, and decode (`tests/protocol.rs:77-125`). No path-dependent admission gap was found. |
| Authentication relation and binding | Tried capability sets containing dangling or cross-scheme policy combinations and checked whether rejection could install state. | The central relation admits only supported scheme/policy/identity triples (`src/policy.rs:4-39`). Binding acceptance and verification reject sets with no executable pair before returning a `Binding`; exhaustive matrix tests pass. No new lifecycle or effect-before-rejection path was found. |
| Packaging and randomized diagnostics | Checked whether narrowed package inclusion or seed-reporting changes altered runtime state transitions or test independence. | These edits do not change the stream machine. Packaging excludes cache files while retaining authored schema, tests, and scripts. Constructor, case, and replay failures now identify their seeds. |

No new architectural root cause was found.

## 0. Evidence base

The exact tuple was verified at the beginning and end of the review. Both checks returned:

| Repository | Commit | Tree |
|---|---|---|
| workspace root | `a93224a7d8b21a49534174073b7fa55ee62ce4ce` | `a962437fa4435066d7204fc647c1c500eadc6ee2` |
| gwz-core | `6a82c646fa9cff5555a182366bc3dd41707bd076` | `0fa0cfdfbb5f97607e442ef17ba3c54641395e6c` |
| gwz-transport | `aa9ecae65d6c0d568c5f4d738f9930d49f684f56` | `f60b9fc47f24d66f7e75e40c36336c53dd571380` |

Tracked status was unchanged at both checks. Build output remained ignored. No source or Git state was modified, and no current-round peer report was read.

The review read:

- Root `CurrentProgramCheckpoint.md`, `AgentProcessRules.md`, `GwzProcessOptimization.md`, the merged `GwzRemoteTransportMemory-RemPlan.md`, and the original State report at the root pin.
- The complete memory checkpoint, `GwzRemoteTransportMemoryImplementation.md:1-87`.
- Controlling state semantics in `GwzRemoteTransportDesign.md:329-455`, requirements S1-S9 at `GwzRemoteTransportRequirements.md:243-278`, and the applicable implementation-plan sections.
- The complete 19-file immutable remediation diff, including stream runtime, binding and policy, admission budgets and code generation, codec validation, tests, package metadata, randomized diagnostics, and README.
- The corrected stream modules in full and the surrounding unchanged outgoing transition logic needed to assess wakeups, control priority, terminal messages, credit, and close ordering.

Commands run independently on the pinned corrected transport tree with Rust `1.96.0`:

- `cargo test --locked`: passed all 38 executed tests; the separate 50,000-case campaign remained intentionally ignored.
- `cargo clippy --locked --all-targets -- -D warnings`: passed.
- `GWZ_TRANSPORT_MC_SEED=0x51a7e20260919 GWZ_TRANSPORT_MC_CASES=10000 cargo test --locked --release --test monte_carlo seeded_message_streams -- --exact --nocapture`: passed 10,000 cases. Coverage included 1,803,475 blocked writes, 3,918,098 blocked reads, 499,486 partial writes, 142,801 flushes, 635 tiny windows, 39 empty streams, 142,538 zero operations, 877,282 delayed deliveries, and 1,251,998 data messages.
- `git diff --check` over the old-to-corrected transport range: passed.

The dispatch also records lane-owner validation at the same tuple on Rust `1.95`: the full/default 3,000-case suite, clippy, regeneration, direct case `0x1234`, the fixed 50,000-case release campaign with run seed `0x202609195eed`, and a standalone package build passed. Those results are corroborating evidence; the independent results above form this review’s executed evidence.

## 2. Invariant analysis

The original counterexamples no longer reproduce:

- **Structured peer failure survives the stream boundary.** `Failed` and failed `Closed` copy both schema fields into the terminal runtime error. The receive queue is not cleared by peer failure, so an accepted prefix is returned before the exact cause. Subsequent read, write, close-result, and pending async operations observe the same cause.
- **Control delivery survives application waiter saturation.** Application registrations consume only `max_waiters`; the outgoing dispatcher has its own single slot. Registration and the state check occur under the same mutex, so a state change cannot occur between observing `WouldBlock` and installing the waker. Wakers are invoked after releasing the mutex. Future cancellation removes either class of registration.

The following broader changed-range attacks also held:

- **Directional limits remain coherent.** Local receive admission and buffer/window capacity cannot exceed `receive_limits`; outgoing payload/window settings cannot exceed `peer_limits`. Construction rejects invalid or unusable negotiations before allocating the machine.
- **Local metadata cannot poison terminal state.** `complete_close` validates the full typed `Closed` message against peer limits before retaining caller-owned facts. A rejected attempt creates no terminal message and can be retried.
- **Ingress remains bounded before copying into machine state.** Every received typed envelope passes receiver-limited admission before message-specific transition logic. The corrected walker accounts for nested containers, absent optional nodes, list elements, encoded size, and allocation consistently with serialized preflight.
- **Exact byte, offset, and credit preservation remains intact.** Incoming offsets must be contiguous and within advertised credit. Outgoing offsets advance only with emitted payload. Credit advances only after reads or explicit close discard. The independent randomized ledger found no divergence.
- **Control ordering remains intact.** Terminal messages, flush acknowledgement, window updates, `Closed`, data, `EndWrite`, and `Close` retain the intended priority and direction ordering. Cancellation can abandon unsent data and is not blocked by data credit.
- **Half-close and graceful close remain bounded.** `EndWrite` preserves the reverse direction. Close starts its deadline before waiting for credit, discards unread reverse data explicitly, and cannot be extended by exhausted credit.
- **Terminal state remains single-shot.** Late messages after error or completion are ignored. Last-owner drop requests cancellation once; endpoint loss wakes blocked operations with `CarrierLost`.
- **Timer and randomized-oracle independence remain intact.** First-byte coalescing deadlines are not restarted by later writes. Clock advancement changes the revision and wakes the dispatcher. Case seeds are derived independently, replayed periodically, and checked against an external byte/offset/credit ledger.

The host still owns ordered delivery, aggregate bounded queues, fair scheduling, monotonic clock service, session/stream routing, sinks, and cleanup. Those are explicit supplied responsibilities and were not treated as missing crate behavior.

## 3. Risks and next action

The randomized campaign concentrates on successful bidirectional transfer, pressure, batching, flush, and half-close. Terminal failures, close cleanup, ownership, and async registration remain primarily deterministic tests. The corrected regressions directly exercise the two formerly missing boundary cases, so this split does not reach the finding threshold.

This GO applies only to the in-memory typed-message implementation checkpoint. It does not accept a physical carrier, production core/CLI integration, remote adapters, external-type generation, schema/API freeze, native-platform behavior, or Phase 1 completion.

The State axis requires no further remediation. The next action is to combine this verdict with the independent current-round axis verdict and accept the checkpoint only if that gate also returns GO.
