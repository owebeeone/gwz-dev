# GWZ Remote Transport Placement A — STATE-AXIS REVIEW

**Review object:** Phase 4 Placement A aggregate: core `54618449b74a312abb192cf9fca456f45aab2e4e..c0649c3d9a4804cee6eb180911da4863266c3045`, transport `a6562e654b52705b72ef1f793ae2045c320cee47..f6e9ee21fe5e39e3fc61d8484a9410062728131c`, and Taut `733e8a78897a90f017f4726e4331aed95e8cb977..bcf98b64d465fc54841121b6d1a2d46940f81a3c`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportPlacementA.md`, dated 2026-09-22.  
**Baseline:** root `9cc9747c9466e877579bf95779dece092864b97e`; core `c0649c3d9a4804cee6eb180911da4863266c3045`; transport `f6e9ee21fe5e39e3fc61d8484a9410062728131c`; Taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `39a93c945b872d3a63fab08047165cc89853cc9f`. Sources were read from exact commits with `git show` and `git diff`.  
**Date:** 2026-09-22  
**Axis:** State—request/session state machines, cancellation, terminal ordering, bounded queues, receiver generations, and closed recovery grammar. Independent, adversarial, read-only. The other axis runs in parallel; nothing here relies on it. Filed verbatim by the lane owner.

**Verdict: NO-GO** — one P2 finding blocks Placement A acceptance. I pre-commit to GO on a revision that resolves P2-1 as specified.

---

## 0. Evidence base

I read the complete generated prompt; `GwzRemoteTransportPlacementA.md`; the controlling placement design’s lifecycle clauses; the updated embedding guide and transport README; the committed source ranges in core, transport, and Taut; and the candidate receiver-admission fixture.

State inspection covered `gwz-transport/src/mux/{mod,routing,asynchronous}.rs`, binding/profile validation, terminal-fact handling, async streams, generated compatibility changes, candidate admission, and the focused mux/profile tests.

I ran the permitted focused command with `CARGO_TARGET_DIR=/tmp/gwz-placement-review-state`. Results: `async_stream` 8/8, `mux` 13/13, `mux_async` 3/3, and `placement_v2` 10/10 passed. These tests do not cover the terminal-retirement sequence below.

All nine tuple HEADs matched at both review boundaries. Member trees remained clean. Root contained only excluded generated prompt/report artifacts and the four old prompts; the current peer report was not opened.

## 1. Findings

### [P2-1] Finishing an endpoint request can discard its queued terminal reply

**Location:** `gwz-transport/src/mux/mod.rs:116-136`, `346-364`, and `429-484`; `src/mux/routing.rs:221-236`; transport README lines 372–374. This violates placement design lines 193–196, which require registration through terminal cleanup and allow already-owned cleanup to finish.

**Sequence:**

1. The endpoint receives an `Open` for request `r`; the route enters `Opening`, and its worker takes the action.
2. The worker calls `send("r", OpenFailed)`.
3. `send` queues `OpenFailed`, then `transition` immediately removes the route because it is terminal.
4. The host calls `finish("r")`, as permitted once no routes remain.
5. `finish` calls `cancel`; `Queue::discard("r")` retains only `Open`, `CheckIdentity`, and bootstrap messages. It deletes the queued `OpenFailed`.
6. With no route remaining, `finish` unregisters `r` and returns success. `Port::next_message` can no longer deliver the terminal.
7. The initiator retains its `Opening` route. Opens have no mux deadline, so it can wait indefinitely until unrelated external cancellation or connection loss.

The same root cause affects queued `IdentityChecked`, `IdentityCheckFailed`, `Failed`, and `Closed` messages. Existing test `retired_request_ids_and_stream_ids_cannot_reopen_work` queues `IdentityChecked` and then calls endpoint `finish`, but never asserts that the reply remains deliverable.

**Impact:** A locally completed endpoint request can strand its peer and falsely report cleanup while suppressing the authoritative terminal outcome and facts. This is a concrete stuck state in Batch A itself.

**Required correction:** Track outbound terminal handoff as request-owned cleanup. Sealing must reject new work while preserving terminal frames. `finish` must return `WouldBlock` until every route is terminal and each already-owned terminal has been transferred through `next_message`, or the port/session has closed. Once transferred, the host’s existing “deliver or disconnect” rule may own subsequent delivery. Do not retain arbitrary data frames merely to close this gap.

**Closure test:** For each terminal family, queue the terminal and immediately call `finish`. Assert `WouldBlock`, exact terminal delivery through both direct and async port paths, peer route retirement with facts preserved, and successful endpoint `finish` only after transfer. Include saturated data queues and drop/cancel paths to prove terminal control capacity and no indefinite peer route.

## 2. Invariant analysis

The remaining attacked invariants held:

- Bootstrap cancellation correctly distinguishes abandonment before Bound installation from cancellation after installation; waiters close or reuse only the established generation.
- Request and stream tombstones prevent identifier reuse; stale sessions and unregistered requests cannot create work.
- Full request/operation/session/endpoint correlation is checked before endpoint dispatch.
- Queue accounting includes allocation and request overhead, preserves control capacity, and closes the generation on async waiter exhaustion.
- Pending async receive cancellation consumes nothing; last Owner/Port drop closes the local generation and wakes waiters.
- Candidate receiver permits revalidate under the same generation lock used by invalidation/replacement, preventing an old-core dispatch after replacement.
- Profile 2 remains negotiated after v1 bootstrap; retained v1 readers and new readers preserve the tested compatibility boundary.
- Closed facts remain the sole authority for failed `Closed`; terminal failure codes and facts preserve first-terminal ordering.
- No inspected code claims physical cleanup, production routing, platform acceptance, or the deferred Batch B/C integration.

## 3. Risks and next action

Batch B’s host facade, scoped backend, SSH funnels, physical containment, guide compilation, and all deferred carrier/platform/activation work remain outside this verdict.

Correct P2-1 in the mux request-retirement state machine and add the terminal-handoff regressions before a focused State re-verdict.
