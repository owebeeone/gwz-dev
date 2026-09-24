# Independent delivery and start tickets — SAFETY-AXIS REVIEW (round 2)

**Date:** 2026-09-23  
**Axis:** Safety  
**Verdict:** **NO-GO** — two P2 findings remain open.

**Review object:** Root `dev-docs/GwzOperationStartTicketDesign.md`, `dev-docs/GwzOperationStartTicketCallerGuideDraft.md`, and `dev-docs/GwzTransportDeliveryStartTicket-RemPlan-1.md`; core `dev-docs/GwzIndependentTransportDeliveryAmendment.md`, `dev-docs/GwzRemoteTransportCapacityAmendment.md`, and `docs/TransportPlacement.md`. This is an interface-design verdict, not an implementation or release verdict.

**Exact tuple:** root `a1f42eaabf690cac1f4ca54804550c482a1d417d`; `gwz-core` `e06c752dbe4048a7f319b28d8351702490e63e7f`; `gwz-py` `45bcd7b3ea102ca927935cee1b41b43934d68140`; `gwz-transport` `46e65a9a888fbd4a5bbeace946996581dcf23333`. All four HEADs matched at the start and end. The reviewed files had no working-tree changes; unrelated working-tree noise was present.

## Prior-finding closure table

| Original finding | Round-2 assessment | Counterexample checked |
| --- | --- | --- |
| Safety S-1 / P1: blocked Data strands Window, Cancel, Failed, Close and shutdown | **Original blocked-Data sequence addressed.** The separate urgent task can advance Window, Cancel and Failed while ordered Data waits; graceful Close has a 30-second ordered-delivery failure boundary, and shutdown disconnects immediately. This does not close the new urgent-admission stall in P2-2 below. | Destination Data queue full while the ordered task waits in `deliver()`: the urgent task is independent. A same-stream graceful Close stays ordered and reaches delivery or binding failure. |
| Safety S-2 / P2: handleless refused tickets consume all 64 Client slots | **Closed in the proposed contract.** `ticket.release()` retires settled no-permit refusals; unreleased settled refusals expire 60 seconds after local cleanup. Unknown committed tickets retain conservative effect evidence until explicit release or hard expiry. | Sixty-four stateless no-slot refusals can be released and a new ticket started. A pending admission cannot be released as a proved no-effect refusal. |

## Changed-range analysis

The round-2 changes add selective urgent and ordered delivery, a 30-second ordered stall rule, a Rust watchdog, Client-generated internal transport IDs, and the handleless ticket lifecycle. The core capacity amendment is unchanged from the prior review. `TransportPlacement.md` now points candidate hosts to the selective schedule while explicitly withholding advertisement until it is implemented.

**New architectural root cause: yes.** Splitting one ordered stream into independently scheduled lanes introduces a causal-order boundary at Open/Opened that the stated Bind/registration barrier does not cover. The same split leaves urgent-queue admission without an explicit bounded failure rule. These are interface-shape defects introduced by the remedy, not deferred physical-wire or activation work.

## 0. Evidence base

I read the review object, the prior Safety report and remediation plan, accepted placement design, relevant process rules, and the current `gwz-transport` mux/stream code. Inspection used `git rev-parse`, `git status`, `git diff`, `rg`, `sed`, and numbered source reads. No builds, tests, mutations, or peer current-round reports were used.

The relevant current port behavior is concrete: `deliver()` waits on `WouldBlock` (`gwz-transport/src/mux/asynchronous.rs:128-135`); the destination queue can return `WouldBlock` (`src/mux/mod.rs:623-647`); the receiver accepts `Opened` but not `Failed` while a route is `Opening` (`src/mux/routing.rs:243-262`), and a protocol error disconnects the mux (`src/mux/routing.rs:4-16`).

## 1. Findings

### P2-1 — Urgent failure can overtake the Opened acknowledgement for its own stream

**Root cause and location:** `gwz-core/dev-docs/GwzIndependentTransportDeliveryAmendment.md:19` assigns `Failed` to the independently scheduled urgent queue and `Open` to the ordered queue. It explicitly bars urgent traffic from overtaking Bind/registration, but does not define a barrier for the stream-opening acknowledgement, `Opened`. Its general per-stream causal-order sentence does not say when an urgent item must wait for an ordered predecessor.

**Violated invariant:** A stream failure must reach a receiver whose route has entered the stream state, or fail the binding for a real delivery failure. Lane scheduling alone must not turn one operation’s failure into a protocol error that disconnects healthy peers.

**Credible state sequence:** The endpoint accepts Open, enqueues Opened on its ordered lane, and enters stream state locally. Before that ordered item is delivered, its stream fails and enqueues Failed on the urgent lane. The urgent task delivers Failed first. The initiator still has an `Opening` route, where the current mux permits Opened or OpenFailed from the endpoint but rejects Failed. That protocol error disconnects the shared binding and interrupts another healthy operation.

**Impact:** A normal setup/failure race can turn one request’s typed failure into binding-wide failure, defeating peer isolation and making the outcome of other operations uncertain.

**Remedy:** Define a per-stream delivery dependency: an urgent Failed cannot pass the Opened acknowledgement required to establish its stream at the receiver. Apply the analogous Open/CheckIdentity dependency to urgent cancellation. Specify whether the scheduler holds the urgent item or fails the binding if its required predecessor cannot be delivered.

**Closure test:** Pause ordered delivery immediately before Opened, produce Failed on that stream, and let urgent delivery run. Verify the initiator receives a valid ordered opening/failure sequence and an unrelated operation remains usable. Repeat with cancellation before Open or CheckIdentity delivery.

### P2-2 — A blocked urgent admission has no stated deadline

**Root cause and location:** `gwz-core/dev-docs/GwzIndependentTransportDeliveryAmendment.md:21` applies the fixed 30-second delivery-stall deadline to an **ordered** frame. Line 23 specifies a watchdog for a **loop-stalled** binding. Neither clause says what ends an urgent task awaiting peer admission when the event loop remains responsive but the destination’s reserved control queue is full. Under the accepted port contract, `deliver()` waits for admission; a full queue yields `WouldBlock`, not a bounded-queue error.

**Violated invariant:** Window, Cancel and Failed must either progress or cause bounded, explicit binding failure. Reserved capacity alone cannot guarantee progress after that finite reserve fills.

**Credible state sequence:** The peer’s control queue fills while its action consumer stops draining. With no ordered frame in flight, the urgent task dequeues a Window and waits in `deliver()`. Cancel and Failed then queue behind it. The event loop and ordered task remain responsive, so the specified loop-stall watchdog need not fire; the ordered-frame deadline does not apply. Cancellation and cleanup remain pending indefinitely despite the proposed control lane.

**Impact:** A stalled control consumer can strand cancellation and failure notification, hold operation and endpoint charges, and eventually refuse unrelated admissions. This is a bounded-resource state, but it lacks the contract’s required bounded fail-closed outcome.

**Remedy:** Apply a delivery-admission deadline to **each** in-flight urgent item as well as ordered items. On expiry, atomically fail the binding, wake both lanes and all affected owners, and retain cleanup charges until actual local drain. Define the watchdog’s observation so a task awaiting admission is covered even while the Python loop runs normally.

**Closure test:** Fill the peer control reserve, stop its consumer, leave the ordered lane idle, then enqueue Window followed by Cancel and Failed. Within the stated deadline, prove either control admission or binding failure, waiter wake-up, retained ownership, and no final-cleanup report before local drain.

## 2. Invariant analysis

The revised ticket contract gives a defined retirement path for settled handleless refusals and keeps effect-uncertain committed starts distinguishable from no-effect starts. Owner-checked status, high-water sequence rejection, charged admission through constructor/registration, and the separation of terminal outcome from local and peer cleanup remain coherent on this axis.

The revised delivery contract resolves the original serial blocked-Data path, including ordered graceful Close through bounded failure. Its new lane split still needs explicit stream-opening dependencies and an urgent-admission deadline. Neither issue is repaired by the capacity amendment or by the stated implementation gate alone, because implementations need the missing scheduling rules as their test oracle.

## 3. Risks and next action

Amend the delivery schedule with the two rules above, add their race and saturation closure tests to the activation gate, then re-review the settled tuple. P0/P1/P2 findings block design GO. Implementation, physical wire, processes, iroh, and version-1 activation remain deferred.

**Final tuple recheck:** root `a1f42eaabf690cac1f4ca54804550c482a1d417d`; `gwz-core` `e06c752dbe4048a7f319b28d8351702490e63e7f`; `gwz-py` `45bcd7b3ea102ca927935cee1b41b43934d68140`; `gwz-transport` `46e65a9a888fbd4a5bbeace946996581dcf23333`. All match the starting tuple.
