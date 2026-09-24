# GwzTransportV3ReorderingDesign — CONSISTENCY-AXIS RE-REVIEW 2

**Review object:** `gwz-core/dev-docs/GwzTransportV3ReorderingDesign.md` at core `07e3d1afbe24326476629cb64736b344b138563d`  
**Baseline:** root `acfc36eb7f99f707a88ee712611885e3236db410`; core `07e3d1afbe24326476629cb64736b344b138563d`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`; Python `45bcd7b3ea102ca927935cee1b41b43934d68140`. Verified at review start and end.  
**Date:** 2026-09-23  
**Axis:** Consistency  
**Verdict:** **GO for the draft transport contract, with one P3 proof-coverage correction.** No P0–P2 inconsistency was established. This does not approve implementation or activate the stopped independent-delivery amendment.

## Prior-finding closure table

| Prior finding | Disposition at this tuple |
| --- | --- |
| Local retirement before a legal late terminal or Cancel | **Closed in the design contract.** A draining route retains validation state, context, dispatch ownership and a cleanup deadline through peer terminal and the host fence (`GwzTransportV3ReorderingDesign.md:27–30`). It cannot adopt facts, change A’s fixed outcome or transfer lease ownership. |
| Late Opened followed by established traffic | **Closed.** Late Opened advances A’s charged draining route to Stream; ordered Data and controls are validated under the live stream grammar before a legal Failed or Closed. Closed still requires the actual EndWrite/Close predecessors (`:27,48`). |
| CheckIdentity after Bound 3 versus placement’s “v2-only” rule | **Closed.** The draft expressly supersedes that placement §6 sentence for profile 3 and requires a post-Bound CheckIdentity test on both roles (`:15,50`). |
| Wrong-generation acknowledgement across profiles | **Closed in the design contract.** The host pins and checks the sender/intended-peer port-generation pair before delivery, regardless of receiving profile. A profile-2 or unbound replacement cannot turn its legacy result into S1’s admission receipt (`:40,50`). The separate profile-3 receiver safeguard has the P3 test gap below. |

## Changed-range analysis

The revision replaces the approximate, locally retired tombstone with a charged draining route and adds an immutable host-owned port-generation pair to each delivery item. These changes address both architectural roots from re-review 1. I found **no new architectural root** in the changed contract. The remaining issue is that one required test, as phrased, cannot exercise the receiver safeguard it claims to verify.

## 0 Evidence base

I compared the pinned draft and prior reports with `GwzRemoteTransportPlacementDesign.md`, `GWZDesign.md`, `GWZRequirements.md`, the stopped `GwzIndependentTransportDeliveryAmendment.md`, and the delivery/start-ticket verdict. I inspected the pinned transport mux, binding, stream machine and async port source as feasibility evidence; that code remains a profile-2 implementation, not proof of profile 3. The stream machine accepts Data within advertised credit, permits a legal endpoint Failed terminal, and requires a prior initiator Close and received EndWrite for Closed (`gwz-transport/src/stream/incoming.rs:28–41,115–118,149–159`). The current profile-2 mux returns `Ok(())` on a session mismatch (`src/mux/routing.rs:4–8`), confirming why the host check must be profile-independent. This review was read-only.

## 1 Findings

### P3 — The mixed-profile host test does not exercise profile-3 `WrongSession`

**Location:** `GwzTransportV3ReorderingDesign.md:50`, read with the host pre-delivery rule at `:40` and receiver rule at `:32`.

**Violated proof invariant:** The host generation guard and the profile-3 receiver’s `WrongSession` result are distinct safeguards. Each needs a test that reaches its own decision point.

**Credible sequence:** The specified host test selects S2 for an S1 item. Line 40 requires the host to reject that port-pair mismatch *before* calling `deliver()`. Repeating the same test with S2 on profile 3 therefore proves the host guard again; S2’s receiver never sees the envelope. A regression that returns `Ok` rather than `WrongSession` for a foreign session could pass this host test.

**Impact:** The receiver safeguard remains unproved, weakening diagnosis and defense if an item reaches a profile-3 port outside the correctly guarded host path. The primary host-generation contract remains coherent, so this is P3 coverage rather than a design blocker.

**Correction:** Keep the host misroute tests, and specify a separate direct profile-3 port-delivery test for a foreign-session envelope.

**Closure test:** Deliver a foreign-session Open and CheckIdentity directly to a profile-3 receiving port. Assert `WrongSession`, no action or route, no admission acknowledgement, and an unaffected healthy binding. Separately verify that the host rejects a mismatched port pair without invoking that port.

## 2 Invariant analysis

For the original A/B counterexample, A’s Open can be admitted before A’s user-visible timeout while B shares the binding. An endpoint Opened already in flight advances A’s draining route to Stream without changing A’s result or facts. Data within established credit and a subsequent legal Failed are validated and consumed; B stays active. The route remains charged until peer terminal and the host’s two-direction cleanup fence. If cleanup cannot finish by its finite deadline, the contract explicitly fails the binding rather than silently dropping A’s route (`:27,30,48`). A Closed immediately after an Opening timeout is correctly rejected without a real Close predecessor; the separate established-stream Closed test supplies that predecessor. Window test frames still need the ordinary credit history required by the stream machine.

Fresh lower IDs receive a route only after validation, bounded queue admission and an atomic route/table commit; WouldBlock leaves no seen record (`:23–25`). The 4096 lifetime-ID records at no more than 512 bytes each give the stated 2 MiB table ceiling. Active and draining routes share the separate 64-route limit (`:17–19,30,49`). The representation and admission atomicity remain explicit implementation proofs, not claims about current code.

The host’s pinned sender/target pair prevents a profile-2 or unbound S2 from falsely acknowledging S1. It also prevents an in-flight item from following a mutable replacement pointer (`:40`). The accepted placement design’s receiver-affinity requirement is preserved (`GwzRemoteTransportPlacementDesign.md:150–167`). Profile-3 CheckIdentity has an explicit, narrow supersession of placement §6; profiles 1 and 2 retain their stated behavior (`GwzTransportV3ReorderingDesign.md:15,50`).

## 3 Risks and next action

The draft can proceed to the separate Safety verdict. Clarify the direct `WrongSession` proof before activation, then perform the draft’s required authoritative-design update and implementation gates only if both review axes permit them. The earlier delivery/start-ticket object remains stopped. No files were modified.
