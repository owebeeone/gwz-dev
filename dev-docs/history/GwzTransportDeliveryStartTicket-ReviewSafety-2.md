# Independent delivery and start tickets — SAFETY-AXIS REVIEW (round 3)

**Date:** 2026-09-23  
**Axis:** Safety  
**Verdict:** **GO for the proposed design contract** — no P0, P1 or P2 findings. This does not approve implementation or activation.

**Review object:** Root `dev-docs/GwzOperationStartTicketDesign.md`, `dev-docs/GwzOperationStartTicketCallerGuideDraft.md` and the prior merged plan `dev-docs/GwzTransportDeliveryStartTicket-RemPlan-2.md`; core `dev-docs/GwzIndependentTransportDeliveryAmendment.md`, `dev-docs/GwzRemoteTransportCapacityAmendment.md` and `docs/TransportPlacement.md`. The accepted `gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md` remains the controlling contract except where the delivery amendment explicitly replaces it.

**Exact tuple at review start:** root `918627cb88ee35228850eb65ee2cc1b54917f9d5`; `gwz-core` `045eb2774c847dcc8979b2fba33f9b0e0ed209d9`; `gwz-py` `45bcd7b3ea102ca927935cee1b41b43934d68140`; `gwz-transport` `46e65a9a888fbd4a5bbeace946996581dcf23333`.

## Prior-finding closure table

| Prior finding | Round-3 assessment | Counterexample retraced |
| --- | --- | --- |
| Original Safety S-1 / P1: blocked Data strands control | **Closed in the proposed contract.** Per-key ordered dispatch and a separate ready urgent selector let eligible control advance around another stream’s blocked Data. Every queued or in-flight item has a 30-second admission deadline; expiry fails the binding and wakes owners. | Saturate one stream’s Data destination while a peer needs Window or Cancel. The ready peer control can be selected; a control blocked at admission has a bounded failure outcome. |
| Original Safety S-2 / P2: handleless refused tickets exhaust local slots | **Closed in the proposed contract.** Settled no-permit refusals have `ticket.release()` and finite local expiry; pending and effect-uncertain work retains its distinct ownership rules. | Release 64 stateless refusals and start another ticket. A lost committed start remains marked effect-uncertain and cannot be treated as a proved no-effect refusal. |
| Round-2 Safety P2-1: Failed overtakes Opened; analogous Cancel before Open/CheckIdentity | **Closed in the proposed contract.** The urgent selector admits only causally ready items. A dependent control waits for the receiver’s opening transition to be admitted, or the binding fails on the predecessor’s deadline. Pre-opening cancellation seals and suppresses unsent opening traffic. | Pause before Opened, then fail the stream: Failed cannot reach an `Opening` receiver ahead of Opened. Pause before Open or CheckIdentity, then cancel: no orphan Cancel is sent; an opening already in flight resolves its admission race before Cancel becomes eligible. |
| Round-2 Safety P2-2: full urgent reserve with no ordered traffic stalls indefinitely | **Closed in the proposed contract.** The deadline covers urgent items while queued, waiting or in `deliver()`, independently of ordered traffic. A Rust watchdog observes item timestamps and admission even while the Python loop runs. | Fill the destination control reserve and leave ordered traffic idle. Within 30 seconds, the urgent item is admitted or the binding fails and wakes affected owners. |

These are contract closures. The amendment explicitly requires implementation and focused tests before the placement can be advertised.

## Changed-range analysis

The material safety change is in `gwz-core/dev-docs/GwzIndependentTransportDeliveryAmendment.md:19–25`: ready-selective per-key dispatch, explicit opening dependencies, atomic delivery-versus-abort resolution, an enqueue-to-peer-admission deadline for both lanes, and an independent Rust watchdog. Its registration rule at `:31` moves request-context installation into charged core/endpoint commit admission and specifies sealing and drain after partial registration or close. The embedding guide now describes those candidate rules.

The root caller contract adds a uniform cancellation-progress shape, repeatable post-close acceptance reads, and a local-only release path for an unknown committed ticket. The core capacity amendment has no material change from round 2.

**New architectural root cause: none found.** The cross-lane opening boundary identified in round 2 is now an explicit delivery dependency. I found no third new architectural root on this review object.

## 0. Evidence base

I read the review object, both prior Safety reports, the prior merged plan, the accepted placement design, and applicable process rules. I compared the amended schedule with current `gwz-transport` mux transitions: `Opening` accepts `Opened` or `OpenFailed`, while `Failed` is valid only for a stream (`src/mux/routing.rs:225–278`); current `deliver()` waits on `WouldBlock` (`src/mux/asynchronous.rs:128–135`), and destination queues can return it (`src/mux/mod.rs:623–647`). Those current mechanisms establish why the new selectors, barriers and watchdog are required; they do not implement the proposed design today.

Inspection was read-only. No builds, edits or current-round peer reports were used. The reviewed files had no working-tree changes; unrelated workspace changes were present.

## 1. Findings

No P0, P1, P2 or P3 findings on this safety axis.

## 2. Invariant analysis

For a stream whose `Opened` acknowledgement is pending, the endpoint may have advanced locally while the initiator remains `Opening`. The amended dependency prevents its urgent `Failed` from reaching that initiator before a valid opening transition. A failure that remains in opening uses the ordered `OpenFailed` path. For cancellation before Open or CheckIdentity, the contract either suppresses the unsent opening and seals its owner or waits for an in-flight predecessor’s admission before making Cancel eligible. An unresolved predecessor reaches explicit binding failure by the host deadline. This preserves the current mux’s legal receive transitions without using a malformed control frame to make progress.

A blocked urgent admission has the same 30-second bound as an ordered item, including when no ordered item exists. The Rust watchdog is assigned observation independent of Python scheduling. Failure wakes dispatchers and stream/request waiters, while cleanup ownership remains charged until actual local drain. A binding-wide timeout can interrupt healthy peers, but only after an actual delivery deadline; an unready control for one key cannot itself hold the ready selector behind a FIFO head.

Charged commit admission owns endpoint construction, both context registrations and their losing cleanup. Neither an uncommitted permit nor Python forwarding creates a request registration. Close, cancellation and route loss seal or transfer partial work before releasing charges. Terminal publication, a five-second cleanup snapshot, and peer teardown are not used as substitutes for proved local completion. Unknown committed starts retain conservative effect evidence and do not authorize replay.

## 3. Risks and next action

The selective port API, atomic admission/abort behavior and Rust watchdog remain proposed work. The amendment’s activation gate must demonstrate the paused opening races, an urgent reserve full with ordered traffic idle, dispatcher loss between dequeue and delivery, partial registration versus close, healthy peer behavior, and retained cleanup ownership. Design GO freezes this contract for that work; it does not establish that current ports satisfy it.

**Final tuple recheck:** root `918627cb88ee35228850eb65ee2cc1b54917f9d5`; `gwz-core` `045eb2774c847dcc8979b2fba33f9b0e0ed209d9`; `gwz-py` `45bcd7b3ea102ca927935cee1b41b43934d68140`; `gwz-transport` `46e65a9a888fbd4a5bbeace946996581dcf23333`. All four HEADs match the starting tuple.
