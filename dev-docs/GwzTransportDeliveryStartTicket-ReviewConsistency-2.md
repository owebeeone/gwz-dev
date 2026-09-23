# Independent delivery and start tickets — CONSISTENCY-AXIS REVIEW (round 3)

**Review object:** Root `dev-docs/GwzOperationStartTicketDesign.md` and `dev-docs/GwzOperationStartTicketCallerGuideDraft.md`; core `dev-docs/GwzIndependentTransportDeliveryAmendment.md`, `dev-docs/GwzRemoteTransportCapacityAmendment.md`, and `docs/TransportPlacement.md`.  
**Exact tuple:** root `918627cb88ee35228850eb65ee2cc1b54917f9d5`; gwz-core `045eb2774c847dcc8979b2fba33f9b0e0ed209d9`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`.  
**Date:** 2026-09-23. **Axis:** Consistency.  
**Verdict: NO-GO** — one P2 finding. This is a design-contract verdict.

## Prior-finding closure table

| Round-2 finding | Disposition on this tuple |
| --- | --- |
| P2-1 — pre-commit host registration leaks | **Closed.** Prepare allocates an ID without registering it. A charged commit admission installs the request cleanup ticket before core and endpoint registration; refusal, cancellation and close seal or transfer that registration. The no-commit sequence leaves no host registration. |
| P2-2 — guide retains the attachment gate | **Closed.** The guide’s next integration gate now requires bidirectional `GwzTransportDeliveryV1` events while application dispatch is blocked or idle. Optional attachments have separate compatibility fixtures. |
| P2-3 — active cleanup described as an expiring marker | **Closed for the reported sequence.** The occupancy rule assigns active cleanup-only records `owned_cleanup` or `external_cleanup` and reserves `marker_expiry` for locally final markers whose TTL has begun. It also states a priority for mixed occupancy. |
| P2-4 — urgent controls overtake Open/CheckIdentity/Opened | **Closed for the reported cross-lane sequence.** Urgent eligibility now depends on peer admission of the opening transition; premature cancellation suppresses an unsent opening, and `Failed` cannot replace `OpenFailed` while the peer is Opening. A distinct cross-stream ordering defect remains below. |
| P2-5 — replay key excludes caller identity | **Closed.** Duplicate prepare compares canonical fields 1–6, including caller ID, method, message name and action bytes. A changed field refuses. |

## Changed-range analysis

The revised start-ticket text moves request registration into charged commit admission, specifies the complete prepare replay key, and separates active cleanup records from final markers. The embedding guide replaces its conflicting attachment gate. The delivery amendment adds stream-opening admission barriers, ready-selective dispatch, and a deadline for every queued or in-flight delivery item. The capacity amendment did not change in this round.

**NEW ARCHITECTURAL root cause — P2-1 below:** The new per-key schedule allows cross-stream opening reordering, while the current mux uses one global high-water stream ID to reject stale openings. This is distinct from round-2’s urgent-versus-ordered opening barrier. The prior merged plan identifies a later third new architectural root as the review-loop stop condition; this finding triggers that condition for this object.

## 0. Evidence base

I read the five review-object files, the prior Consistency reports and round-2 merged plan, the controlling placement design, the relevant current mux transitions, `AgentProcessRules.md`, and `GwzProcessOptimization.md`. I compared the changed root and core ranges against the prior reviewed tuple. Inspection used repository reads and diffs only; no files were modified and no builds or tests were run. The five review-object files were clean. Unrelated working-tree changes were present.

## 1. Findings

### P2-1 — Per-key delivery can silently discard a fresh lower-numbered Open

**Location:** Delivery amendment [§2, per-key selection and “no global FIFO”](/Users/owebeeone/limbo/gwz-dev/gwz-core/dev-docs/GwzIndependentTransportDeliveryAmendment.md:19); current mux [stream ID allocation](/Users/owebeeone/limbo/gwz-dev/gwz-transport/src/mux/mod.rs:341) and [incoming Open/CheckIdentity high-water check](/Users/owebeeone/limbo/gwz-dev/gwz-transport/src/mux/routing.rs:34).

**Invariant:** Every fresh Open or CheckIdentity that the host reports admitted must establish its peer stream context, or fail the binding explicitly. Independent Data progress may not turn a fresh opening into a stale duplicate.

**Credible sequence:** On a ready binding, operation A enqueues Open with stream ID 1; operation B enqueues Open with stream ID 2. They have different request/stream keys. The amended dispatcher permits B to reach the peer first because it promises no global FIFO across unrelated keys. The receiving mux admits B and sets `highest_id=2`. When A arrives, `receive_in` sees `1 <= highest_id` and returns `Ok(())` before creating A’s route. The delivery host can treat A as successfully admitted, so its 30-second item deadline no longer detects the loss. The same sequence works with CheckIdentity or mixed Open/check openings.

**Impact:** A valid operation can wait for a response to an opening the peer silently discarded. Its request may remain pending until a separate cancellation or timeout, while the host reports successful delivery. This breaks the proposed overlap and bounded delivery-failure contract without a malformed frame or failed carrier.

**Remedy:** Freeze a cross-key admission rule for *new* Open/CheckIdentity stream IDs that preserves their global creation order while allowing unrelated established-stream Data and eligible controls to progress. Alternatively, amend the mux’s stale/duplicate model to accept out-of-order fresh IDs with bounded, context-checked replay state. Either choice must specify how a blocked earlier opening reaches the 30-second fail-closed outcome.

**Closure test:** Enqueue openings 1 and 2 under different requests, force the selector to prefer 2, and repeat with Open, CheckIdentity and mixed kinds. Prove that both fresh contexts are established in valid order, or that the binding explicitly fails by the delivery deadline. No opening may receive a successful admission acknowledgement while being ignored as stale.

## 2. Invariant analysis

The original five counterexamples no longer reproduce on the revised text: registration has a charged owner before its first side effect; the guide names the independent event gate; active cleanup has a non-expiry diagnosis; urgent controls wait for their required opening transitions; and prepare replay covers caller identity. The capacity amendment’s pre-accept epoch charge remains consistent with the new registration owner.

The remaining break is between the delivery schedule and the existing mux identity rule. A per-key ordering guarantee preserves order *within* a stream, but the mux assigns and validates opening IDs *across* streams. The amendment’s opening barriers address dependencies within one stream and do not constrain the order in which two fresh streams are first admitted.

## 3. Risks and next action

Keep this tuple at **NO-GO**. The new cross-stream opening defect is an architectural root under the round-2 merged plan’s stop rule. Redesign and re-freeze the delivery schedule against the mux’s stream-ID semantics before another acceptance review. This report does not assess implementation, a physical wire, separate processes, iroh, activation or release.

**End-of-review tuple check:** root `918627cb88ee35228850eb65ee2cc1b54917f9d5`; gwz-core `045eb2774c847dcc8979b2fba33f9b0e0ed209d9`; gwz-py `45bcd7b3ea102ca927935cee1b41b43934d68140`; gwz-transport `46e65a9a888fbd4a5bbeace946996581dcf23333`. All match the starting tuple.
