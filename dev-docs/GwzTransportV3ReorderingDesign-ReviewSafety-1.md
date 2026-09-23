# GwzTransportV3ReorderingDesign — SAFETY-AXIS RE-REVIEW 1

**Review object:** `gwz-core/dev-docs/GwzTransportV3ReorderingDesign.md` at core `5b6af440158714fe997ac78933d161cd4492d8a5`  
**Baseline:** root `d8dacb26e625522ce01584a5916e7fc939109472`; core `5b6af440158714fe997ac78933d161cd4492d8a5`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`; Python `45bcd7b3ea102ca927935cee1b41b43934d68140`. Verified at review start and end.  
**Date:** 2026-09-23  
**Axis:** Safety  
**Verdict:** **NO-GO — one P2 mixed-profile handoff finding.**

## Prior-finding closure

| Prior finding | Disposition |
| --- | --- |
| Safety P2-1: legal late terminal after local retirement fails a shared binding | **Closed in the design contract.** The distinct locally retired tombstone retains context and phase, contains race-legal late frames without adopting facts or leases, and requires an A/B isolation test (`GwzTransportV3ReorderingDesign.md:19,28,44`). Implementation remains unproved. |
| Consistency P2: CheckIdentity after Bound 3 conflicts with placement’s “v2-only” rule | **Closed.** The revision expressly amends that rule and tests CheckIdentity after Bound 3 while preserving profiles 1 and 2 (`:15,46`). |
| Safety P2-2: wrong-generation Open receives a false acknowledgement | **Closed for a profile-3 receiving port; incomplete across profiles.** A profile-3 receiver must return `WrongSession`, and the host must fail or abort the sender’s delivery (`:30,46`). The finding below concerns a replacement port still using profile 2. |

## Changed-range analysis

The changed lines address all three named prior counterexamples. The locally retired tombstone’s shadow phase permits a late `Opened` followed by cleanup without adopting a lease; its cleanup owner remains charged (`:28`). The fixed record cap and 2 MiB claim remain explicit implementation proof obligations (`:17–19,45`), not a new contradiction established by this review.

**NEW ARCHITECTURAL root cause:** The wrong-generation detection signal is conditional on the *receiving* port’s negotiated profile, while the delivery owner can have draining and replacement generations on different profiles. The revision requires the right outcome but its proof exercises only a receiver that returns `WrongSession`.

## 0 Evidence base

I reviewed the pinned draft, the prior Safety and Consistency reports and merged remediation plan, the accepted `GwzRemoteTransportPlacementDesign.md`, the remote-transport requirements in `GWZDesign.md` and `GWZRequirements.md`, the stopped `GwzIndependentTransportDeliveryAmendment.md` and start-ticket verdict, and current `gwz-transport` binding, mux, routing, and port code. The current transport code is feasibility evidence for profile-2 behavior; it is not a profile-3 implementation. No files were modified.

## 1 Findings

### P2-1 — A profile-2 replacement port can still acknowledge a misrouted profile-3 opening

**Location:** `GwzTransportV3ReorderingDesign.md:15,30,46`; current `gwz-transport/src/mux/routing.rs:4–8` and `src/mux/asynchronous.rs:128–134`.

**Root cause and invariant:** The new `WrongSession` result is specified “on profile 3,” and profile-2 mismatch behavior is expressly unchanged. Yet an opening’s delivery acknowledgement must mean that its intended peer admitted its action and route. The draft permits a host to choose profile-2 FIFO placement separately before binding (`:15`), so overlapping generations need not have the same profile.

**Sequence:** S1 is a draining profile-3 binding with an Open or CheckIdentity in flight. The host installs S2 using profile 2 and mistakenly hands S1’s item to S2’s port. Current profile-2 `Mux::receive` returns `Ok(())` on a session mismatch before inspecting the item; `Port::deliver` exposes that `Ok` as delivery success. A forwarding host that relies on the new receiver result therefore acknowledges S1’s opening although S1’s peer has no action or route. S2 remains apparently healthy. The draft’s required S1/S2 test expects S2 to return `WrongSession`, which a retained profile-2 port does not do.

**Impact:** S1 can wait for a stream that was never opened. This recreates false delivery success during a supported mixed-profile generation transition and may leave cleanup waiting until its deadline.

**Required correction:** Make the delivery owner verify the exact sender and target port generations before treating any `Ok` as admission, independently of the target’s negotiated profile. On mismatch it must fail or atomically abort S1’s item, wake S1’s waiter, and retain its cleanup charge; S2 must receive no effect. Keep `WrongSession` as the profile-3 receiver safeguard, but do not make it the only handoff safeguard.

**Closure test:** Keep S1 profile 3 live while S2 is profile 2, then misroute an S1 Open and CheckIdentity to S2. Verify that S2’s legacy `Ok` cannot acknowledge S1, S1’s delivery fails or is atomically aborted with its waiter awakened, cleanup stays charged, and independent S2 work continues. Repeat while S2 is not yet Bound, when no profile-3 receiver result is available.

## 2 Invariant analysis

The revision’s unseen-ID transaction admits a lower fresh ID after a higher one and requires queue and route admission before success (`GwzTransportV3ReorderingDesign.md:23–25`). Its tombstone rules distinguish peer-terminal from local retirement and preserve the outcome and facts of a retired A while B continues (`:27–28`). Its v3-only Bind offer prevents the per-key dispatcher from silently running on a profile-2 binding (`:15`). These are coherent safety rules subject to the stated implementation proofs.

Receiver-side `WrongSession` closes the original S1-to-S2 counterexample when S2 is profile 3. It cannot provide the same evidence when S2 is profile 2 or not yet Bound. The accepted placement design already requires receiver affinity across dispatch (`GwzRemoteTransportPlacementDesign.md:153–167`); the revised contract needs that generation check at the host’s acknowledgement boundary for every profile combination.

## 3 Risks and next action

Keep this design at **NO-GO** until the mixed-profile delivery acknowledgement rule and closure test are added. The existing atomic admission, bounded history, late-cleanup, deadline, and two-peer tests remain required implementation gates.
