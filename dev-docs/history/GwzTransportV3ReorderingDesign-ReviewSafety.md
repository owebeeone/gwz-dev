# GwzTransportV3ReorderingDesign — SAFETY-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzTransportV3ReorderingDesign.md` at core `f837a0007e6fe30866123734c6e509edb58492fe`  
**Baseline:** root `856ec97774915256691fd29ab3443555e6489d5a`; core `f837a0007e6fe30866123734c6e509edb58492fe`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`; Python `45bcd7b3ea102ca927935cee1b41b43934d68140`. Verified unchanged at review start and end.  
**Date:** 2026-09-23  
**Axis:** Safety  
**Verdict:** **NO-GO — two P2 findings.**

## 0 Evidence base

I reviewed the draft against the accepted `gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md`, the remote-transport sections of `gwz-core/dev-docs/GWZDesign.md` and `GWZRequirements.md`, the current `gwz-transport/src/mux/{mod.rs,routing.rs}` and `src/binding.rs`, and the stopped `GwzIndependentTransportDeliveryAmendment.md` and `GwzTransportDeliveryStartTicket-Verdict-3.md`. This is a design review; the current mux implements profile 2 and is evidence of transitions the profile-3 design must address.

## 1 Findings

### P2-1 — Legitimate late terminals can fail a shared binding

**Location:** `GwzTransportV3ReorderingDesign.md:27,43,46`.

**Violated invariant:** Local expiry or cancellation must contain a late terminal without changing recorded facts, resurrecting work, or unnecessarily failing an unrelated operation. The accepted placement design requires late check completion to be contained (`GwzRemoteTransportPlacementDesign.md:290-297`) and stale deliveries to have no authority (`:217-222`).

**Sequence:** Request A has an admitted CheckIdentity or Open. Its initiator locally times out or cancels and retires the route before observing the endpoint’s terminal response. The endpoint’s already legitimate `IdentityCheckFailed`, `OpenFailed`, or `Closed` then arrives. The draft says that, once locally retired without an observed terminal acknowledgement, “no incoming frame may claim to be that acknowledgement.” In the draft’s three-case lookup, this leaves no nonfatal outcome for the valid late response. Treating it as a protocol error fails the binding shared with healthy request B. The current mux has a real local-retirement path on expiry (`gwz-transport/src/mux/mod.rs:526-606`); this is not an invented transition.

**Impact:** An ordinary timeout or cancellation race on A can interrupt B and its Git exchange. The tombstone prevents authority transfer, but the specified response handling creates avoidable cross-request failure.

**Required correction:** Define a distinct *locally retired, no terminal observed* state. A correctly correlated late terminal for that state must be contained without accepting its facts, changing its outcome, releasing a lease again, or reopening work. Specify which terminal kinds are eligible and when malformed or context-conflicting frames still fail the binding.

**Closure test:** Admit A and B on one binding. Retire A locally before each legal terminal kind arrives, including both orderings around cancellation and timeout. Deliver A’s late terminal and verify A remains retired, its facts and outcome do not change, and B continues. Inject wrong request/context and effectful frames against A’s tombstone and verify fail-closed behavior.

### P2-2 — A stale-generation Open can receive a false delivery acknowledgement

**Location:** `GwzTransportV3ReorderingDesign.md:29,31,43`.

**Violated invariant:** The draft states that every successful opening-delivery acknowledgement means the peer admitted that opening to its validated action queue and route table (`:29`). The same paragraph says a different-session/generation envelope is discarded. The current `Mux::receive` returns `Ok(())` for a session mismatch before admission (`gwz-transport/src/mux/routing.rs:4-8`).

**Sequence:** During generation replacement or overlap, an in-flight Open from S1 is delivered through an S2 port because a host selector uses the replacement port. S2 discards it as a session mismatch and returns success. If that return is the host’s peer-admission acknowledgement, S1’s dispatcher marks the Open delivered, while neither peer has its route or action. The accepted design requires receiver affinity across dispatch and invalidation on replacement (`GwzRemoteTransportPlacementDesign.md:153-167`); the draft must make violation of that precondition observable rather than turn it into delivery success.

**Impact:** The original false-success failure reappears across generations. The operation can wait for a stream that does not exist, while the new binding remains apparently healthy.

**Required correction:** Give stale-session/generation discard a result distinguishable from admission success. It may leave S2 untouched, but the host must fail or explicitly abort S1’s delivery and retain its cleanup charge. State that only an `Accepted` result can acknowledge an Open/CheckIdentity; session-mismatch discard cannot.

**Closure test:** Keep S1 and S2 ports present, route an S1 Open/CheckIdentity to S2, and assert that S2 gains no route or action and the delivery is **not** acknowledged as successful. Verify S1 fails or is atomically aborted, its waiters wake, and an independent S2 operation continues.

## 2 Invariant analysis

The draft gives unseen IDs a bounded lifetime namespace and requires queue admission before route/seen installation (`GwzTransportV3ReorderingDesign.md:17-25`). That addresses the stopped design’s Open 2 before Open 1 failure when implemented atomically. It also requires a v3-only Bind offer and verified Bound before releasing openings (`:15`), and retains request, operation, endpoint, and session checks (`:23-29`). Those rules provide a defensible mixed-profile and authority boundary if the required tests pass.

The two findings concern outcomes *after* those checks: local retirement followed by a legitimate late terminal, and an opening sent to the wrong generation. Neither is resolved by choosing receiver-side out-of-order admission. The proposed proof lists late frames, old-generation envelopes, and two-peer isolation (`:43-46`), but does not assert the outcomes needed to refute these sequences.

## 3 Risks and next action

Keep the profile-3 design at **NO-GO** until both response rules and closure tests are added. The implementation gate must still prove the draft’s stated 4096-record/2 MiB bound, atomic WouldBlock retry, 30-second delivery-or-abort outcome, v3-only negotiation, and two-peer isolation. Those are existing draft preconditions, not additional findings from this review.
