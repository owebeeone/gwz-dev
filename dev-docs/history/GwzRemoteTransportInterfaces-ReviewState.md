# GWZ Remote Transport Phase 1/2 Interface Freeze Candidate — State Review

**Date:** 2026-09-19  
**Axis:** State  
**Verdict:** **GO**  
**Findings:** P0: 0 · P1: 0 · P2: 0 · P3: 1

## Reviewed object

This review covers the Phase 1 typed schema, admission and message-handoff interface and the Phase 2 stream/pool runtime API candidate. It does not qualify physical delivery, production host dispatchers, SSH/HTTPS adapters, native platforms, publication, registry resolution, or remote CI.

| Repository | Commit | Tree |
|---|---|---|
| workspace root | `9bb74ace2fca56f770af34b6ec1bdf6bab4e695e` | `6fadda10c4e45a049164e3a74ebf4cbd471e773b` |
| `gwz-core` | `ce3bb967f9696b1791fcbefb65b6ccdd0c8ab618` | `b0fe75ea55d7d4e85719acb4c10ca4586485828d` |
| `gwz-transport` | `163feebe439edd5d1fbaf11e163882e80d4f2257` | `4cebefa1058d0b87c838ecdb70e0a0ecc70d527c` |
| `taut` | `733e8a78897a90f017f4726e4331aed95e8cb977` | `bb0694d0e31d217b82024cb73c2e1212dfa5c72f` |

The tuple and trees matched at both the beginning and end of review. Member repositories remained clean. The root had untracked review-process files outside the committed object; no current peer prompt or report was opened.

## Evidence base

I inspected the committed range from transport `e8b9a1c5408cc9ea9528939b3a602acbeb697814` and core `435e936b593476f24fad4cc4e70f5d06b784ed7d`, including:

- `dev-docs/GwzRemoteTransportInterfaces-Checkpoint.md`
- `gwz-core/dev-docs/GwzRemoteTransportPool-InterfaceGate.md`
- `gwz-core/dev-docs/GwzRemoteTransportDesign.md` §10.1–10.2
- the Phase 1/2 requirements and plan
- transport stream clocks, state machine, incoming/outgoing paths and async facade
- pool allocation, clock, lifecycle, cleanup and async ownership
- binding, policy and generated codec surfaces
- core admission, message handoff and pool-host consumer fixtures
- owner generation, package and consumer archive-proof machinery

Executed evidence:

- Focused Rust 1.95 command covering `io_clock`, `io_clock_async`, `pool_io_clock`, `network_timeouts`, and `policy`: **21 tests passed**.
- Owner regeneration check: **4 generated artifacts verified**.
- Source-pinned consumer regeneration check: **passed**.
- Exact archive proof for SHA-256 `8c7d91d54f0e176109f286beec65c24544f35f23506380939fdb081e6cef87a6` and source revision `163feebe439edd5d1fbaf11e163882e80d4f2257`: **16 isolated consumer tests passed**.
- Transport/core whitespace checks: **passed**.
- Generated schema, IR and protocol artifacts were unchanged from the accepted schema baseline.

The checkpoint’s full-suite, random-walk, packaging and tooling results were treated as recorded evidence and were not rerun.

## Finding

### P3-1 — Clock-only mutations wake every unrelated async waiter

**Root cause and locations**

One shared revision is used both for readiness changes and clock bookkeeping:

- `gwz-transport/src/pool/clock.rs:6-52`
- `gwz-transport/src/pool/asynchronous.rs:24-45`
- `gwz-transport/src/stream/machine.rs:281-320`
- `gwz-transport/src/stream/asynchronous.rs:27-47`

`PoolMachine::advance` unconditionally calls `touch()` after every non-backward tick, even when no deadline crossed, state changed, or action became runnable. The async facade interprets that revision change by waking all checkout waiters and the driver.

The new stream clock controls similarly call `touch()` for nonterminal I/O-state changes and every positive peer-progress report. The stream async facade then wakes every application waiter and its dispatcher, although changing or resetting a deadline alone cannot make a blocked read, write, flush, close, or message dispatch ready.

**Violated invariant**

Clock maintenance must preserve liveness without creating repeated work proportional to every unrelated pending operation. A deadline update should wake consumers only when it changes observable readiness or terminal state, or signal a distinct timer-rearm subscriber if such a subscriber exists.

**Concrete sequence**

1. Use the default pool limit and leave 1,024 checkout futures pending while their connections are still opening or capacity is saturated.
2. Leave the driver pending with no current action.
3. Use the explicitly permitted periodic-tick host strategy from `gwz-transport/README.md:195-200`, for example a 10 ms tick before any deadline.
4. Each `advance` performs no expiry or useful transition but increments the revision.
5. `Shared::change` clones and wakes every checkout waker plus the driver. They repoll and return pending.
6. Repeating the permitted tick can therefore request about 102,400 fruitless checkout wakes per second under the default bound until a timeout or another real transition occurs. With disabled network timing, the opening interval need not end through the network clock.

The analogous stream sequence registers blocked application operations and the dispatcher, then reports small positive physical-backend transfers before a complete envelope or acknowledgement exists. Each progress report resets the deadline and wakes as many as 1,024 application waiters plus the dispatcher even though no application operation became ready.

**Impact**

The memory bounds remain intact and no progress, bytes, terminal cause, or replay decision is invented. The consequence is bounded CPU amplification and possible event-loop starvation under valid host behavior. This is a robustness defect and does not require a public API or schema change.

**Required correction**

Separate readiness notification from clock/deadline bookkeeping:

- In the pool, notify async waiters only when expiry, scheduling, cleanup or another operation changes an action, checkout result, terminal condition, or shutdown result.
- In the stream, resetting an I/O deadline or changing a nonterminal clock classification must not wake unrelated application and dispatcher waiters.
- If timer rearming later gains an async subscription, give it a distinct notification path. The present contract calls `next_deadline()` a snapshot and requires the mutating host to recompute its timer.

**Closure test**

Use counting wakers to establish all of the following:

1. Repeated pool `advance` calls strictly before the next deadline produce zero checkout/driver wakes when no state or action changes.
2. Advancing exactly to an expiry wakes the affected checkout and driver as required.
3. Positive stream progress updates the deadline without waking unrelated blocked application operations or the message dispatcher.
4. A resulting timeout, received envelope, credit change, acknowledgement, close result, or other genuine readiness transition still wakes the necessary waiters.

## State-invariant analysis

- **First terminal cause:** Terminal checks precede later event handling, and `fail` is idempotent once an error or completion is retained. Late cancellation, progress, delivery, disconnect and cleanup cannot replace the first cause.
- **Exact deadlines:** Stream and pool expiry use `now >= deadline`; an event at the boundary is late. Backward clock updates are ignored.
- **Pause versus reset:** `Idle`, `Backpressure`, and helper interaction preserve the remaining network allowance. Repeated state reports do not refill it. Only positive peer-byte progress in `Network` restores the stream I/O allowance.
- **Zero, EOF and native bounds:** Zero-byte progress is a no-op; EOF requires a state change and is not progress. Native connect/I/O values accept zero through `i32::MAX`; zero disables only network timing while helper and cleanup limits remain active.
- **Cumulative helper budget:** Repeated connect interactions share the pool allowance. The host contract explicitly requires connect/auth helper elapsed time to be subtracted when constructing the active stream, including on a reused connection. The focused seam test exercises that remaining-budget transfer.
- **Mixed-direction classification:** The aggregate contract gives `Network` precedence whenever either direction can make peer progress; `Backpressure` applies only when all pending work is locally blocked. The API can represent this rule. A real host classifier remains deferred.
- **Readable prefixes and loss:** A timeout or carrier loss preserves already received bytes for draining and then returns the retained terminal error. It does not turn uncertainty into EOF or Git success.
- **Close takeover:** Starting or receiving Close stops the active I/O clock and transfers control to the independent cleanup deadline. Later state/progress reports are rejected.
- **Shutdown, drop and disposal:** Checkout cancellation, lease drop, session/operation cancellation, final pool-owner drop and driver loss preserve explicit ownership. Capacity is not reclaimed until physical disposal is acknowledged; cleanup expiry emits abort work but does not pretend disposal completed.
- **Admission and typed parity:** Binding, Open identity/capability/deadline checks precede effects in both typed and serialized paths. The consumer fixtures cover all envelope variants, bidirectional data, flush, half-close, reverse drain, cancellation, loss and timeout.
- **Resource bounds:** Buffers, waiter registrations, request counts, connection counts and textual identity inputs are bounded. P3-1 is the remaining bounded wake-amplification issue.
- **Generation and archive identity:** Owner and consumer regeneration agreed, and the isolated consumer used the exact verified archive rather than a sibling path.

## Residual risks and next action

The State axis supports advancing this tuple as the Phase 1/2 interface-freeze candidate. P3-1 can be corrected internally without changing the proposed schema or runtime API and does not block this verdict.

Combine this report with the independent Code and Surface verdicts before recording the freeze. Track P3-1 through a counting-waker regression before production adapter or performance qualification. Physical delivery, production timer/dispatcher behavior, SSH/HTTPS semantics, native-platform runs, publication, registry resolution and remote CI remain separate later gates.
