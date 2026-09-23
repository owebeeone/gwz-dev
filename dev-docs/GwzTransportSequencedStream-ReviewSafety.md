# GWZ sequenced virtual-stream design — SAFETY-AXIS REVIEW

**Review object:** `gwz-core/dev-docs/GwzTransportSequencedStreamRequirements.md` and `GwzTransportSequencedStreamDesign.md`, introduced at core HEAD.  
**Baseline SHAs/read method:** Workspace `38d5235926c61c66f3fda4cc37f4da01b3c140d0`; core `bf60d472f41404316d35927624b82600d0713c63`; transport `46e65a9a888fbd4a5bbeace946996581dcf23333`. Read-only inspection of the committed documents, controlling references, protocol schema and current mux/stream shape. All three SHAs matched at the start and end. Unrelated working-tree changes were excluded.  
**Date:** 2026-09-24  
**Axis:** Safety; peer-blind.

**Verdict: GO — design contract only. P0: 0; P1: 0; P2: 0; P3: 0.** The attempted interleavings below did not expose a concrete path to false success, duplicate effects, an unreleased lease, or an indefinite wait under the stated contract. This does not accept an implementation or activation.

## 0. Evidence base

The controlling requirements demand independent stream progress, ordered application within each sending direction, truthful terminal outcomes, bounded gaps and cleanup, and generation-pinned receipts ([requirements, lines 7–23](gwz-core/dev-docs/GwzTransportSequencedStreamRequirements.md)). The existing transport contract supplies the close, cancellation, carrier-loss and pool-health rules ([remote requirements §5.4, lines 271–305](gwz-core/dev-docs/GwzRemoteTransportRequirements.md); [remote design §6, lines 397–455](gwz-core/dev-docs/GwzRemoteTransportDesign.md)). The placement design supplies request registration, receiver affinity and bounded host queues ([placement design, lines 153–161 and 188–234](gwz-core/dev-docs/GwzRemoteTransportPlacementDesign.md)). Current schema and state-machine code were inspected for feasibility, not reviewed as profile-3 implementation.

## 2. Invariant analysis

- **Abortive overtake:** A failure or legal cancellation at sequence N fixes a failure and supersedes missing predecessors below N. Later covered frames have no effect or second `Applied` receipt; frames above the sender’s terminal are invalid. Opposite-direction work remains charged until delivered or atomically aborted ([design, lines 27–31](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)). This addresses the prior late `Window`/`Flushed` failure path without treating an abort as a successful byte delivery.
- **Pre-Open and identity:** A post-opening frame arriving before sequence-1 Open/check is provisional and cannot create a route or endpoint effect. A correlated Cancel can seal that ID; otherwise the fixed gap deadline fails the binding. Seen-ID tracking permits Open 2 before fresh Open 1, while lifetime and active-ID caps prevent unbounded provisional state ([design, lines 15 and 19–21](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)).
- **Graceful truthfulness:** A graceful terminal cannot skip its own sending-direction prefix. Data and final offsets remain independent byte checks, and Closed retains the existing Close/EndWrite prerequisites. Healthy pool return waits for the full handshake and cleanup fence; terminal receipt alone does not claim Git success ([design, lines 13 and 25–31](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)).
- **Progress and ownership:** Submission distinguishes bounded buffering from application. Cancelled application waiters do not discard unresolved tickets. Reorder caps, a predecessor reserve, fair missing-sequence scheduling, a non-resetting 30-second gap deadline and a finite cleanup deadline provide an explicit failure path when progress cannot be established ([design, lines 23 and 31–37](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)). Generation pinning prevents an old receiving Port from acknowledging the intended peer’s delivery ([design, lines 15 and 41](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)).

## 3. Risks and next action

The document leaves the aggregate memory representation and ticket-budget proof to the implementation gate and explicitly permits lowering the candidate 4096-ID cap before profile freeze ([design, line 37](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md)). That proof, the directed interleavings and seed-replayable tests in [lines 41–45](gwz-core/dev-docs/GwzTransportSequencedStreamDesign.md) remain required before activation. No tests or builds were run for this read-only design review.
