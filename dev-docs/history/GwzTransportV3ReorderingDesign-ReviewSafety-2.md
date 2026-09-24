# GwzTransportV3ReorderingDesign — SAFETY-AXIS RE-REVIEW 2

**Review object:** `gwz-core/dev-docs/GwzTransportV3ReorderingDesign.md` at core `07e3d1afbe24326476629cb64736b344b138563d`  
**Baseline:** root `acfc36eb7f99f707a88ee712611885e3236db410`; core `07e3d1afbe24326476629cb64736b344b138563d`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`; Python `45bcd7b3ea102ca927935cee1b41b43934d68140`. Verified unchanged at review start and end.  
**Date:** 2026-09-23  
**Axis:** Safety  
**Verdict:** **NO-GO — one P2 terminal-ordering finding.**

## Prior-finding closure table

| Prior finding | Disposition at this tuple |
| --- | --- |
| Safety P2-1: legal late completion after local retirement fails a shared binding | **Closed for the cited sequence.** A charged draining route validates late Opened, stream traffic and a legal terminal without changing the fixed result or facts (`GwzTransportV3ReorderingDesign.md:19,27–30,48`). The new finding concerns the ordering of traffic already in flight when that terminal arrives. |
| Safety P2-2: wrong-generation Open receives false success | **Closed in the design contract.** The host pins and checks the exact sender/peer Port generations before delivery and treats only the intended peer’s admission as the item’s receipt (`:40,50`). |
| Re-review 1 Safety P2-1: a profile-2 or unbound replacement falsely acknowledges S1 | **Closed in the design contract.** The host check applies independently of the receiving profile, and the required test includes S2 at profile 2 and unbound (`:40,50`). |
| Re-review 1 Consistency P2: a late Opened cannot continue through ordinary stream traffic | **Closed for the cited sequence.** The draining route retains the full stream validation state, accepts legal intervening Window/Data, and requires a real Close predecessor for Closed (`:27,48`). |
| Initial Consistency P2: CheckIdentity conflicts with placement’s “v2-only” rule | **Closed.** The draft explicitly extends CheckIdentity to profile 3 while preserving profiles 1 and 2 (`:15,50`). |

## Changed-range analysis

The changed text replaces premature route removal with a charged draining route and replaces receiver-only generation detection with a host-owned, exact-Port admission receipt. Both corrections address their direct prior counterexamples.

**NEW ARCHITECTURAL root cause:** The draining route remains charged after a peer terminal, but its permitted post-terminal inputs exclude an earlier same-key control frame that was already dequeued or in flight. The proposed urgent dispatcher gives Window and Failed only the opening/Opened admission dependency; it does not require Failed to wait for an earlier Window’s delivery-or-abort decision.

## 0 Evidence base

I inspected the pinned draft, prior Safety and Consistency reports and remediation plans, the accepted `GwzRemoteTransportPlacementDesign.md`, the remote-transport portions of `GWZDesign.md` and `GWZRequirements.md`, the stopped `GwzIndependentTransportDeliveryAmendment.md` and start-ticket verdict, and current `gwz-transport` mux, binding, async Port and stream-machine source. Current code implements profile 2 and supplies transition and scheduling feasibility evidence; it is not a profile-3 implementation. This review modified no files.

## 1 Findings

### P2-1 — A terminal can overtake an earlier Window and fail an unrelated stream

**Location:** `GwzTransportV3ReorderingDesign.md:27,30,38,48`; proposed urgent schedule in `GwzIndependentTransportDeliveryAmendment.md:19–23`; current terminal-first stream selection in `gwz-transport/src/stream/outgoing.rs:7–12` and Window emission at `:32–42`.

**Violated invariant:** A correctly generated, bounded frame already owned by the delivery host must be admitted or atomically aborted before a later same-key terminal makes it illegal at the receiver. A local outcome on A must not fail healthy B merely because A’s valid traffic and terminal complete in a different host scheduling order.

**Credible sequence:** A and B share a profile-3 binding. A is established and then becomes a charged draining route after its user-visible timeout. The endpoint emits a valid Window for A; the host dequeues it, but its peer admission remains pending. The endpoint then produces a Failed terminal for A. The proposed urgent selector can treat both controls as ready once Opened was admitted, and the current stream machine selects a terminal ahead of ordinary output. Failed reaches A’s draining receiver first. While the cleanup fence is still pending, the earlier Window completes delivery. The draft permits only an exact duplicate terminal or an opposite-direction in-flight Cancel after the peer terminal. It therefore classifies the Window as Protocol and fails the binding, interrupting B. The cleanup fence cannot prevent this: it resolves in-flight delivery after the terminal, while the receiver has already rejected that delivery.

**Impact:** A normal timeout/failure race can interrupt another operation and its Git exchange. The explicit cleanup deadline bounds the stall but does not prevent this avoidable binding-wide failure.

**Required correction:** Make terminal admission a same-key barrier over every earlier generated or dequeued control frame, including Window and other non-Data controls. Before Failed can be admitted, each predecessor must either reach peer admission or be atomically aborted with its in-flight delivery race resolved. Preserve the existing ability to abort earlier unsent Data. If the host cannot establish the ordering or abort outcome by the delivery deadline, fail the binding explicitly before claiming terminal admission.

**Closure test:** Establish A and B on one binding, make A draining, and hold an already dequeued valid Window short of peer admission. Generate A’s Failed, exercise both attempted delivery orders, and prove either that Window is admitted before Failed or that it is atomically aborted and can never arrive afterward. A’s fixed outcome and facts stay fixed, and B continues. Repeat with an already dequeued Flushed or other legal same-key control whose delivery precedes the terminal.

## 2 Invariant analysis

The unseen-ID transaction reserves bounded history and a route slot before acknowledging queue admission, so Open 2 before Open 1 no longer implies Open 1 is stale (`GwzTransportV3ReorderingDesign.md:23–25`). The 4096-ID cap, 2 MiB table charge, 64 active-plus-draining route cap and finite cleanup deadline give explicit resource bounds, subject to implementation proof (`:17–19,30,49`). The host’s pinned Port pair closes the cited S1-to-S2 false-success path across profile-3, profile-2 and unbound replacements (`:40,50`). A profile-3 `WrongSession` remains an additional receiver safeguard.

The outstanding defect is within a correctly selected profile-3 binding. Retaining a draining route preserves the state needed to validate late traffic, but retaining it does not make a valid preterminal Window legal *after* Failed. The host must settle that predecessor before admitting the terminal.

## 3 Risks and next action

Keep this design at **NO-GO** until the terminal barrier and its in-flight control test are specified. This is a contract finding; no implementation or production activation is established by the current draft.
