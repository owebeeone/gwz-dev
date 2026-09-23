# GwzTransportV3ReorderingDesign — CONSISTENCY-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzTransportV3ReorderingDesign.md`  
**Baseline:** root `856ec97774915256691fd29ab3443555e6489d5a`; core `f837a0007e6fe30866123734c6e509edb58492fe`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`; Python `45bcd7b3ea102ca927935cee1b41b43934d68140`. Verified at start and end.  
**Date:** 2026-09-23  
**Axis:** Consistency  
**Verdict:** **NO-GO — two P2 findings.** Both have bounded contract corrections. A consistency-axis GO can follow those corrections; implementation and the independent Safety gate remain separate.

## 0 Evidence base

I reviewed the draft against the accepted placement design, `GWZDesign.md`, `GWZRequirements.md`, the current transport mux and binding code, and the stopped independent-delivery amendment and verdict as historical input. The current mux is profile 2: it uses `highest_id` to ignore lower openings (`gwz-transport/src/mux/routing.rs:34–40`), removes routes on terminal messages (`routing.rs:264–279`), and can retire a route on a local deadline before a peer response arrives (`gwz-transport/src/mux/mod.rs:542–595`). Those are implementation facts to address, not evidence that profile 3 is already implemented.

## 1 Findings

### P2 — Local retirement makes ordinary late cleanup a binding-wide protocol failure

**Location:** Draft `GwzTransportV3ReorderingDesign.md:27`, with the required test list at lines 43 and 46.

**Violated invariant:** A locally retired stream cannot regain authority or alter facts, but valid cleanup already in flight must not fail unrelated operations on the shared binding. The accepted placement contract preserves bounded cleanup after user-visible completion and says late completion cannot resurrect a retired check (`GwzRemoteTransportPlacementDesign.md:217–223, 290–297`). The proposed delivery schedule allows each item up to 30 seconds to reach peer admission (`GwzIndependentTransportDeliveryAmendment.md:21–25`).

**Credible sequence:** Two requests share a binding. Request A is cancelled or reaches its local cleanup deadline; its mux retires the route while A’s peer terminal or Cancel is still in flight. The current mux already has this local-retirement transition (`gwz-transport/src/mux/mod.rs:542–595`). The delayed, correctly correlated frame then arrives. Under draft line 27, no terminal acknowledgement may arrive if none was previously observed, and another effectful frame against the tombstone is a protocol error. Draft line 29 fails the binding on that error. Request B consequently loses a healthy stream; during a push, its publication outcome may become uncertain.

**Required correction:** Specify tombstone handling for a locally retired route separately from duplicate handling after an observed terminal. Validate the delayed frame’s session, request, operation, endpoint, direction, kind and bounded body; suppress race-legal late completion and cleanup without changing outcome, facts or lease ownership. Continue to fail closed for conflicting context or an impossible transition. State which late frame classes are race-legal on each role.

**Closure test:** With A and B active on one binding, locally retire A before its first peer terminal is observed, then deliver A’s valid late terminal and delayed Cancel in both orders. Assert that A’s outcome and facts remain fixed, B continues, no lease is released twice, and a conflicting terminal or context still fails closed. This case is not established by the draft’s “exact and conflicting terminal acknowledgements” test, which does not require local retirement before the first acknowledgement.

### P2 — The draft permits profile-3 CheckIdentity without superseding the accepted “v2-only” rule

**Location:** Draft `GwzTransportV3ReorderingDesign.md:5, 15, 48`; accepted `GwzRemoteTransportPlacementDesign.md:290–297`; current `gwz-transport/src/binding.rs:103–113`.

**Violated invariant:** A negotiated profile must give both roles one answer about which message kinds are legal. The draft says it replaces the opening-order assumption and keeps CheckIdentity legal under profile 3. The accepted placement design explicitly calls CheckIdentity “v2-only”; the current binding validator enforces `bound.version == 2`. Thus a conforming implementation of the accepted wording rejects a CheckIdentity sent after a valid profile-3 Bound.

**Credible sequence:** A host offers `[3]`, receives and verifies Bound 3, then sends a version-3 CheckIdentity as the draft requires. An endpoint retaining the accepted v2-only validation refuses it. The host cannot perform the required identity preflight despite successful profile selection.

**Required correction:** Explicitly supersede the “v2-only” sentence for profile 3: CheckIdentity is legal in profiles 2 and 3, with the same body, identity policy and terminal rules, and remains illegal in profile 1. Identify this as a second precise amendment to placement §6 alongside the opening-order change. Carry that scope into the authoritative design and requirements update before implementation.

**Closure test:** Negotiate profile 3 and complete CheckIdentity in both mux roles; verify profile 1 still rejects it, profile 2 behavior is unchanged, and a v3-only offer to a v2 endpoint rejects before effects. The test must exercise a CheckIdentity after Bound 3, not just negotiate Bound 3.

## 2 Invariant analysis

The draft gives Open and CheckIdentity one binding-scoped positive-ID namespace, permits lower unseen IDs, retains admitted IDs until binding retirement, and distinguishes an aborted opening from an admitted one (`GwzTransportV3ReorderingDesign.md:11, 19, 23–31`). Those rules address the stopped verdict’s concrete Open-2-before-Open-1 loss (`dev-docs/GwzTransportDeliveryStartTicket-Verdict-3.md:7–9`). The admission transaction also requires the queue and route/table commit to be atomic; `WouldBlock` leaves no receiver record or operation binding (`GwzTransportV3ReorderingDesign.md:25`). The current `enqueue`/route sequence shows why that is an implementation gate, not a present guarantee (`gwz-transport/src/mux/routing.rs:67–84`).

The fixed 4096 lifetime-ID ceiling and 512-byte maximum record charge imply the stated 2 MiB table ceiling. The draft expressly requires implementation to prove a bounded representation or lower the cap before activation (`GwzTransportV3ReorderingDesign.md:17–19, 44`). No separate contradiction in that arithmetic was established. The 64 active-route and 256 request-ID figures are current defaults (`gwz-transport/src/mux/mod.rs:44–54`), while the lifetime-ID cap is the new fixed profile rule. Capacity tests must account for that distinction.

The profile-3-only Bind offer prevents accidental selection of profile 2 for a per-key opening dispatcher (`GwzTransportV3ReorderingDesign.md:15`). Existing negotiation selects the highest common offered version (`GwzRemoteTransportPlacementDesign.md:269–278`; `gwz-transport/src/binding.rs:158–162`). The remaining version inconsistency is CheckIdentity’s explicit v2-only scope, identified above. The stopped amendment’s independent scheduling remains historical proposal rather than an accepted implementation (`dev-docs/GwzTransportDeliveryStartTicket-Verdict-3.md:11`).

## 3 Risks and next action

Revise the tombstone rule to accommodate validated late cleanup after local retirement, and explicitly extend CheckIdentity legality to profile 3. Add the closure tests above to the required proof list. The existing current mux and binding code need later implementation work, but their lack of profile-3 support is not a design-review finding. No files were modified.
