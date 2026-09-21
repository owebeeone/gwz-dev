# GWZ Remote Transport Placement A remediation 1 — STATE-AXIS REVIEW

**Review object:** Focused Placement A correction: core `c0649c3d9a4804cee6eb180911da4863266c3045..018176c6783d900570ba8f918aa744bd642fc3b6` and transport `f6e9ee21fe5e39e3fc61d8484a9410062728131c..d7051eede48f6cd8b07815edc9ced1916f82585c`; controlling draft `gwz-core/dev-docs/GwzRemoteTransportPlacementA.md`, correction 1, dated 2026-09-22. Taut is unchanged.  
**Baseline:** root `e4bf1328de6ac5a56645a13827a0b6d739a70462`; core `018176c6783d900570ba8f918aa744bd642fc3b6`; transport `d7051eede48f6cd8b07815edc9ced1916f82585c`; Taut `bcf98b64d465fc54841121b6d1a2d46940f81a3c`; CLI `7db07bbdefd2897c07fd0f9e550bf032bd8b1314`; Python `d07d55dacb1725d9306be9c04d157ac29a78e000`; git2-rs `ce78628308e11b4e8901d5061602619109bce21a`; libgit2 `b172e3d187a4b6866fd9f696f40a1b8e7f56d348`; evidence `a3dc6989aa769dec65e41f39d7e53a8febb4e916`. Sources were read from immutable commits with `git show` and `git diff`.  
**Date:** 2026-09-22  
**Axis:** State—focused terminal ownership, bounded retirement, bootstrap rejection, cancellation, and closed-state recovery. Independent, adversarial, read-only. The current other-axis report was not read. Filed verbatim by the lane owner.

**Verdict: GO** — State P2-1 is closed. No new architectural root cause or P0–P3 finding was identified in the changed range.

---

## 0. Prior-finding closure

| Finding | Status | Closure evidence |
|---|---|---|
| State P2-1: `finish` could discard an admitted terminal and strand the peer | **Closed** | `Queue::discard` now preserves every terminal family; `finish` returns `WouldBlock` while request-owned outbound or local-action terminals remain; terminal transfer removes that ownership; and the original cleanup deadline closes the generation if handoff stalls. Direct and async regressions cover `OpenFailed`, both identity terminals, `Failed`, and `Closed`, including exact facts, saturation, repeated cancellation, timeout, and port drop. |

## 1. Changed-range analysis

The original sequence now terminates correctly:

1. Endpoint `send` admits and queues the terminal, then removes its route.
2. `finish` seals the request without deleting the terminal.
3. `finish` observes the terminal in the outbound queue and returns `WouldBlock`.
4. `next_message` transfers the exact terminal and facts to the application port.
5. A subsequent `finish` can retire the request. The host must then deliver the transferred attachment or disconnect under the existing port contract.
6. If transfer never occurs, the nonrenewable cleanup deadline closes the generation rather than reporting success.

Received terminal cleanup follows the equivalent local-action path: `finish` blocks until `next_action` transfers the authoritative terminal to the local consumer. Arbitrary data frames are still discarded during sealing and do not gain terminal retention.

The merged bootstrap correction introduces a closed state grammar rather than another ambiguous failure path. A valid but incompatible Bind creates one effect-free `BindRejected`, enters `Rejecting`, admits no new work, and retains the exact failure. Port transfer closes the local generation while preserving `bootstrap_failure`; a stalled rejection closes at its bounded bootstrap deadline. The initiator distinguishes `Rejected` from carrier `Closed`. Malformed Bind still fails as protocol input without inventing a negotiation rejection. No wire or schema meaning changed.

No new architectural root cause was found.

## 2. Evidence base

I read the complete round-one prompt, merged remediation plan, my filed initial State report, the exact core and transport diffs, revised Placement A checkpoint, transport README, and changed mux implementation and regressions. Inspection focused on:

- `gwz-transport/src/mux/mod.rs:57-165`, `235-285`, and `366-652`;
- `src/mux/routing.rs:100-196`;
- `src/mux/asynchronous.rs` readiness and retained-failure projection;
- `tests/mux.rs:421-769`;
- the revised lower-level lifecycle documentation.

I ran the permitted focused suite with the external target directory. Results: `async_stream` 8/8, `mux` 23/23, `mux_async` 3/3, and `placement_v2` 10/10 passed—44 focused executions total. The owner’s broader 131-test plus README-doctest result was not substituted for this inspection.

All nine tuple HEADs matched at both review boundaries. Member trees remained clean. Root contained only excluded generated current-round prompt/report artifacts and the four old prompts; no current peer report was opened.

## 3. Findings

None.

## 4. Invariant analysis

The corrected range preserves the relevant invariants:

- Sealing prevents new work while already-owned terminal cleanup remains deliverable.
- `finish` cannot report request retirement while a terminal frame remains in either mux-owned handoff queue.
- Repeated `cancel` or `finish` cannot extend the cleanup deadline.
- Queue saturation retains terminal control capacity without consuming unrelated stream ownership.
- Port drop and deadline expiry close the generation and wake peers; neither claims remote physical cleanup.
- Typed bootstrap rejection has one retained first outcome, no binding, routes, or endpoint work, and cannot be confused with malformed input or carrier loss.
- Bind rejection, terminal facts, stale-session refusal, request tombstones, and bootstrap cancellation remain bounded and fail closed.
- The change does not claim Batch B host/backend integration, a physical carrier, platform qualification, production capability advertisement, or activation.

## 5. Risks and next action

Batch B must still enforce the scoped backend and host request guard and must propagate port closure to physical endpoint work. Batch C, platform/source qualification, HTTPS, publication, and activation remain separate gates.

State-axis acceptance can proceed for Placement A on this tuple.
